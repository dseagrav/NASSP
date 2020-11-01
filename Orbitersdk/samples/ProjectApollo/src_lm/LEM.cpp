/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn V Module Parked/Docked mode

  Project Apollo is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  Project Apollo is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Project Apollo; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

  See http://nassp.sourceforge.net/license/ for more details.

  **************************************************************************/

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "lm_channels.h"
#include "mcc.h"

#include "LEM.h"
#include "tracer.h"
#include "papi.h"
#include "Mission.h"

#include "connector.h"

char trace_file[] = "ProjectApollo LM.log";


// ==============================================================
// Global parameters
// ==============================================================

static int refcount;

const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};

const int TO_EVA=1;


// Modif x15 to manage landing sound
#ifdef DIRECTSOUNDENABLED
static SoundEvent sevent        ;
static double NextEventTime = 0.0;
#endif

static GDIParams g_Param;

// ==============================================================
// API interface
// ==============================================================

BOOL WINAPI DllMain (HINSTANCE hModule,
					 DWORD ul_reason_for_call,
					 LPVOID lpReserved)
{
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		InitGParam(hModule);
		g_Param.hDLL = hModule; // DS20060413 Save for later
		break;

	case DLL_PROCESS_DETACH:
		FreeGParam();
		break;
	}
	return TRUE;
}

DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel)

{
	LEM *lem;

	if (!refcount++) {
		LEMLoadMeshes();
	}

	// VESSELSOUND 

	lem = new LEM(hvessel, flightmodel);
	return static_cast<VESSEL *> (lem);
}

DLLCLBK void ovcExit(VESSEL *vessel)

{
	TRACESETUP("ovcExit LMPARKED");

	--refcount;

	if (!refcount) {
		TRACE("refcount == 0");

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) delete static_cast<LEM *> (vessel);
}

// DS20060302 DX8 callback for enumerating joysticks
BOOL CALLBACK EnumJoysticksCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pLEM)
{
	class LEM * lem = (LEM*)pLEM; // Pointer to us
	HRESULT hr;

	if(lem->js_enabled > 1){  // Do we already have enough joysticks?
		return DIENUM_STOP; } // If so, stop enumerating additional devices.

	// Obtain an interface to the enumerated joystick.
    hr = lem->dx8ppv->CreateDevice(pdidInstance->guidInstance, &lem->dx8_joystick[lem->js_enabled], NULL);
	
	if(FAILED(hr)) {              // Did that work?
		return DIENUM_CONTINUE; } // No, keep enumerating (if there's more)

	lem->js_enabled++;      // Otherwise, Next!
	return DIENUM_CONTINUE; // and keep enumerating
}

// DX8 callback for enumerating joystick axes
BOOL CALLBACK EnumAxesCallback( const DIDEVICEOBJECTINSTANCE* pdidoi, VOID* pLEM )
{
	class LEM * lem = (LEM*)pLEM; // Pointer to us

    if (pdidoi->guidType == GUID_ZAxis) {
		if (lem->js_current == lem->rhc_id) {
			lem->rhc_rzx_id = 1;
		} else {
			lem->thc_rzx_id = 1;
		}
	}

    if (pdidoi->guidType == GUID_RzAxis) {
		if (lem->js_current == lem->rhc_id) {
			lem->rhc_rot_id = 2;
		} else {
			lem->thc_rot_id = 2;
		}
	}

    if (pdidoi->guidType == GUID_POV) {
		if (lem->js_current == lem->rhc_id) {
			lem->rhc_pov_id = 0;
		} else {
			lem->thc_pov_id = 0;
		}
	}
    return DIENUM_CONTINUE;
}

// Constructor
LEM::LEM(OBJHANDLE hObj, int fmodel) : Payload (hObj, fmodel), 
	
	CDRs28VBus("CDR-28V-Bus",NULL),
	LMPs28VBus("LMP-28V-Bus",NULL),
	DES_CDRs28VBusA("DES-CDR-28V-BusA",NULL),
	DES_CDRs28VBusB("DES-CDR-28V-BusB",NULL),
	DES_LMPs28VBusA("DES-LMP-28V-BusA",NULL),
	DES_LMPs28VBusB("DES-LMP-28V-BusB",NULL),
	ED28VBusA("ED-28V-BusA", NULL),
	ED28VBusB("ED-28V-BusB", NULL),
	ACBusA("AC-Bus-A",NULL),
	ACBusB("AC-Bus-B",NULL),
	dsky(soundlib, agc, 015),
	LandingGearPyros("Landing-Gear-Pyros", Panelsdk),
	LandingGearPyrosFeeder("Landing-Gear-Pyros-Feeder", Panelsdk),
	StagingBoltsPyros("Staging-Bolts-Pyros", Panelsdk),
	StagingNutsPyros("Staging-Nuts-Pyros", Panelsdk),
	CableCuttingPyros("Cable-Cutting-Pyros", Panelsdk),
	CableCuttingPyrosFeeder("Cable-Cutting-Pyros-Feeder", Panelsdk),
	DescentPropVentPyros("Descent-Prop-Vent-Pyros", Panelsdk),
	DescentPropVentPyrosFeeder("Descent-Prop-Vent-Pyros-Feeder", Panelsdk),
	DescentEngineStartPyros("Descent-Engine-Start-Pyros", Panelsdk),
	DescentEngineStartPyrosFeeder("Descent-Engine-Start-Pyros-Feeder", Panelsdk),
	DescentEngineOnPyros("Descent-Engine-On-Pyros", Panelsdk),
	DescentEngineOnPyrosFeeder("Descent-Engine-On-Pyros-Feeder", Panelsdk),
	DescentPropIsolPyros("Descent-Prop-Isol-Pyros", Panelsdk),
	DescentPropIsolPyrosFeeder("Descent-Prop-Isol-Pyros-Feeder", Panelsdk),
	AscentHeliumIsol1Pyros("Ascent-Helium-Isol1-Pyros", Panelsdk),
	AscentHeliumIsol1PyrosFeeder("Ascent-Helium-Isol1-Pyros-Feeder", Panelsdk),
	AscentHeliumIsol2Pyros("Ascent-Helium-Isol2-Pyros", Panelsdk),
	AscentHeliumIsol2PyrosFeeder("Ascent-Helium-Isol2-Pyros-Feeder", Panelsdk),
	AscentOxidCompValvePyros("Ascent-Oxid-Comp-Valve-Pyros", Panelsdk),
	AscentOxidCompValvePyrosFeeder("Ascent-Oxid-Comp-Valve-Pyros-Feeder", Panelsdk),
	AscentFuelCompValvePyros("Ascent-Fuel-Comp-Valve-Pyros", Panelsdk),
	AscentFuelCompValvePyrosFeeder("Ascent-Fuel-Comp-Valve-Pyros-Feeder", Panelsdk),
	RCSHeliumSupplyAPyros("RCS-Helium-Supply-A-Pyros", Panelsdk),
	RCSHeliumSupplyAPyrosFeeder("RCS-Helium-Supply-A-Pyros-Feeder", Panelsdk),
	RCSHeliumSupplyBPyros("RCS-Helium-Supply-B-Pyros", Panelsdk),
	RCSHeliumSupplyBPyrosFeeder("RCS-Helium-Supply-B-Pyros-Feeder", Panelsdk),
	agc(soundlib, dsky, imu, scdu, tcdu, Panelsdk),
	CSMToLEMPowerSource("CSMToLEMPower", Panelsdk),
	ACVoltsAttenuator("AC-Volts-Attenuator", 62.5, 125.0, 20.0, 40.0),
	RadarSignalStrengthAttenuator("RadarSignalStrengthAttenuator", 0.0, 5.0, 0.0, 5.0),
	RadarSignalStrengthMeter(0.0, 5.0, 220.0, -50.0),
	checkControl(soundlib),
	MFDToPanelConnector(MainPanel, checkControl),
	imu(agc, Panelsdk),
	scdu(agc, RegOPTX, 0140, 1),
	tcdu(agc, RegOPTY, 0141, 1),
	aea(Panelsdk, deda),
	deda(this,soundlib, aea),
	CWEA(soundlib, Bclick),
	DPS(th_hover),
	DPSPropellant(ph_Dsc, Panelsdk),
	APSPropellant(ph_Asc, Panelsdk),
	RCSA(ph_RCSA, Panelsdk, false),
	RCSB(ph_RCSB, Panelsdk, true),
	tca1A(2, 5, 8, 3, 5, 3, 7),
	tca2A(2, 2, 3, 11, 10, 3, 3),
	tca3A(4, 7, 10, 11, 5, 11, 8),
	tca4A(4, 5, 6, 11, 2, 11, 4),
	tca1B(2, 6, 7, 3, 6, 3, 8),
	tca2B(2, 1, 4, 11, 9, 3, 4),
	tca3B(4, 8, 9, 11, 6, 11, 7),
	tca4B(4, 3, 4, 11, 1, 11, 3),
	MissionTimerDisplay(Panelsdk),
	EventTimerDisplay(Panelsdk),
	omni_fwd(_V(0.0, 0.0, 1.0)),
	omni_aft(_V(0.0, 0.0, -1.0)),
	ForwardHatch(HatchOpenSound, HatchCloseSound),
	OverheadHatch(HatchOpenSound, HatchCloseSound),
	CabinFan(CabinFans),
	CrewStatus(CrewDeadSound),
	ecs(Panelsdk),
	CSMToLEMECSConnector(this),
	CSMToLEMPowerConnector(this),
	LEMToSLAConnector(this),
	CSMToLEMCommandConnector(this),
	lm_rr_to_csm_connector(this, &RR),
	cdi(this),
	AOTLampFeeder("AOT-Lamp-Feeder", Panelsdk)
{
	dllhandle = g_Param.hDLL; // DS20060413 Save for later
	InitLEMCalled = false;

	//Mission File
	InitMissionManagementMemory();
	pMission = paGetDefaultMission();

	// VESSELSOUND initialisation
	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	// Switch to compatible dock mode
	SetDockMode(0);

	// Docking port (0)
	SetLmDockingPort(2.6);

	// Docking port used for LM/SLA connection (1)
	VECTOR3 dockpos = { 0.0 , -1.0, 0.0 };
	VECTOR3 dockdir = { 0,-1,0 };
	VECTOR3 dockrot = { -0.8660254, 0, 0.5 };
	docksla = CreateDock(dockpos, dockdir, dockrot);

	// Init further down
	Init();
}

LEM::~LEM()
{
	ReleaseSurfaces();
	ReleaseSurfacesVC();
	DeleteVCAnimations();

	ClearMissionManagementMemory();

#ifdef DIRECTSOUNDENABLED
    sevent.Stop();
	sevent.Done();
#endif

	// DS20060413 release DirectX stuff
	if(js_enabled > 0){
		// Release joysticks
		while(js_enabled > 0){
			js_enabled--;
			dx8_joystick[js_enabled]->Unacquire();
			dx8_joystick[js_enabled]->Release();
		}
		dx8ppv->Release();
		dx8ppv = NULL;
	}
}

void LEM::Init()

{
	DebugLineClearTimer = 0;

	ToggleEva=false;
	CDREVA_IP=false;
	refcount = 0;
	viewpos = LMVIEW_CDR;
	stage = 0;
	status = 0;
	CDRinPLSS = 0;
	LMPinPLSS = 0;

	InVC = false;
	InPanel = false;
	ExtView = false;
	CheckPanelIdInTimestep = false;
	RefreshPanelIdInTimestep = false;
	InFOV = true;
	SaveFOV = 0;
	VcInfoActive = false;
	VcInfoEnabled = false;

	Crewed = true;
	AutoSlow = false;

	MissionTime = 0;
	FirstTimestep = true;

	SwitchFocusToLeva = 0;

	agc.ControlVessel(this);
	imu.SetVessel(this, TRUE);
	
	ph_Dsc = 0;
	ph_Asc = 0;
	ph_RCSA = 0;
	ph_RCSB = 0;

	for (int i = 0;i < 16;i++)
	{
		th_rcs[i] = 0;
	}

	th_hover[0] = 0;

	// Clobber checklist variables
	for (int i = 0; i < 16; i++)
	{
		Checklist_Variable[i][0] = 0;
	}

	// Camera jostle.
	ViewOffsetx = 0;
	ViewOffsety = 0;
	ViewOffsetz = 0;

	DPSPropellant.SetVessel(this);
	APSPropellant.SetVessel(this);
	RCSA.SetVessel(this);
	RCSB.SetVessel(this);

	DescentFuelMassKg = 8375.0;
	AscentFuelMassKg = 2345.0;
	AscentEmptyMassKg = 2150.0;
	DescentEmptyMassKg = 2224.0;

	ApolloNo = 0;
	Landed = false;

	// Mesh Indexes
	ascidx = -1;
	dscidx = -1;
	vcidx = -1;

	drogue = NULL;
	probes = NULL;
	cdrmesh = NULL;
	lmpmesh = NULL;
	vcmesh = NULL;

	pMCC = NULL;

	trackLightPos = _V(0, 0, 0);
	for (int i = 0;i < 5;i++)
	{
		dockingLightsPos[i] = _V(0, 0, 0);
	}

	//
	// VAGC Mode settings
	//

	VAGCChecklistAutoSlow = false;
	VAGCChecklistAutoEnabled = false;

	// DS20160916 Physical parameters updation
	CurrentFuelWeight = 0;
	LastFuelWeight = 999999; // Ensure update at first opportunity
	currentCoG = _V(0, 0, 0);

	LEMToCSMConnector.SetType(CSM_LEM_DOCKING);
	CSMToLEMPowerConnector.SetType(LEM_CSM_POWER);
	CSMToLEMECSConnector.SetType(LEM_CSM_ECS);
	LEMToSLAConnector.SetType(PAYLOAD_SLA_CONNECT);
	lm_rr_to_csm_connector.SetType(RADAR_RF_SIGNAL);

	LEMToCSMConnector.AddTo(&CSMToLEMPowerConnector);
	LEMToCSMConnector.AddTo(&CSMToLEMCommandConnector);
	CSMToLEMPowerSource.SetConnector(&CSMToLEMPowerConnector);

	//
	// Panel flash.
	//

	NextFlashUpdate = MINUS_INFINITY;
	PanelFlashOn = false;

	//
	// Initial sound setup
	//

	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);
	/// \todo Disabled for now because of the LEVA and the descent stage vessel
	///		  Enable before CSM docking
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);

	// Disable Rolling, landing, speedbrake, crash sound. This causes issues in Orbiter 2016.
	soundlib.SoundOptionOnOff(PLAYLANDINGANDGROUNDSOUND, FALSE);

	strncpy(AudioLanguage, "English", 64);
	soundlib.SetLanguage(AudioLanguage);
	SoundsLoaded = false;

	exhaustTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_atrcs");

	//
	// Register visible connectors.
	//
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &MFDToPanelConnector);
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &cdi);
	RegisterConnector(0, &LEMToCSMConnector);
	RegisterConnector(0, &CSMToLEMECSConnector);
	RegisterConnector(1, &LEMToSLAConnector);
	RegisterConnector(VIRTUAL_CONNECTOR_PORT, &lm_rr_to_csm_connector);

	// Do this stuff only once
	if(!InitLEMCalled){
		SystemsInit();
		InitVCAnimations();

		// Panel items
		fdaiDisabled = false;
		fdaiSmooth = false;

		PanelId = LMPANEL_MAIN;	// default panel
		InitSwitches();
		// "dummy" SetSwitches to enable the panel event handling
		SetSwitches(PanelId);

		//
		// Default channel setup.
		//
		agc.SetInputChannelBit(030, DescendStageAttached, true);	// Descent stage attached.


		InitLEMCalled = true;
	}
}

void LEM::DoFirstTimestep()

{
	checkControl.linktoVessel(this);
	// Load sounds in case of dynamic creation, otherwise during clbkLoadStageEx
	if (!SoundsLoaded) {
		LoadDefaultSounds();
	}

#ifdef DIRECTSOUNDENABLED
	NextEventTime = 0.0;
#endif

	char VName10[256]="";

	strcpy (VName10, GetName()); strcat (VName10, "-LEVA");
	hLEVA=oapiGetVesselByName(VName10);
}

void LEM::LoadDefaultSounds()

{
    char buffers[80];

	soundlib.SetLanguage(AudioLanguage);
	sprintf(buffers, "Apollo%d", ApolloNo);
    soundlib.SetSoundLibMissionPath(buffers);

	//
	// load sounds now that the audio language has been set up.
	//

	soundlib.LoadMissionSound(LunarAscent, LUNARASCENT_SOUND, LUNARASCENT_SOUND);
	soundlib.LoadSound(StageS, "Stagesep.wav");
	soundlib.LoadMissionSound(Scontact, LUNARCONTACT_SOUND, LUNARCONTACT_SOUND);
	soundlib.LoadSound(Sclick, CLICK_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Rclick, ROTARY_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(Bclick, "button.wav", INTERNAL_ONLY);
	soundlib.LoadSound(Gclick, "guard.wav", INTERNAL_ONLY);
	soundlib.LoadSound(CabinFans, "cabin.wav", INTERNAL_ONLY);
	soundlib.LoadSound(Vox, "vox.wav");
	soundlib.LoadSound(Afire, "des_abort.wav");
	soundlib.LoadSound(RCSFireSound, RCSFIRE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(RCSSustainSound, RCSSUSTAIN_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(HatchOpenSound, HATCHOPEN_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(HatchCloseSound, HATCHCLOSE_SOUND, INTERNAL_ONLY);
	soundlib.LoadSound(GlycolPumpSound, "GlycolPump.wav", INTERNAL_ONLY);
	soundlib.LoadSound(SuitFanSound, "LMSuitFan.wav", INTERNAL_ONLY);
	soundlib.LoadSound(CrewDeadSound, CREWDEAD_SOUND);

	// Configure sound options where needed
	SuitFanSound.setFadeTime(5);
	SuitFanSound.setFrequencyShift(3000, 11025);
	GlycolPumpSound.setRiseTime(3);
	GlycolPumpSound.setFadeTime(3);
	GlycolPumpSound.setFrequencyShift(3000, 11025);

// MODIF X15 manage landing sound
#ifdef DIRECTSOUNDENABLED
    sevent.LoadMissionLandingSoundArray(soundlib,"sound.csv");
    sevent.InitDirectSound(soundlib);
#endif
	SoundsLoaded = true;
}

int LEM::clbkConsumeBufferedKey(DWORD key, bool down, char *keystate) {

	// rewrote to get key events rather than monitor key state - LazyD

	// DS20060404 Allow keys to control DSKY like in the CM
	if (KEYMOD_SHIFT(keystate)){
		// Do DSKY stuff
		if(down){
			switch(key){
				case OAPI_KEY_DECIMAL:
					dsky.ClearPressed();
					break;
				case OAPI_KEY_PRIOR:
					dsky.ResetPressed();
					break;
				case OAPI_KEY_HOME:
					dsky.KeyRel();
					break;
				case OAPI_KEY_NUMPADENTER:
					dsky.EnterPressed();
					break;
				case OAPI_KEY_DIVIDE:
					dsky.VerbPressed();
					break;
				case OAPI_KEY_MULTIPLY:
					dsky.NounPressed();
					break;
				case OAPI_KEY_ADD:
					dsky.PlusPressed();
					break;
				case OAPI_KEY_SUBTRACT:
					dsky.MinusPressed();
					break;
				case OAPI_KEY_END:
					dsky.ProgPressed();
					break;
				case OAPI_KEY_NUMPAD1:
					dsky.NumberPressed(1);
					break;
				case OAPI_KEY_NUMPAD2:
					dsky.NumberPressed(2);
					break;
				case OAPI_KEY_NUMPAD3:
					dsky.NumberPressed(3);
					break;
				case OAPI_KEY_NUMPAD4:
					dsky.NumberPressed(4);
					break;
				case OAPI_KEY_NUMPAD5:
					dsky.NumberPressed(5);
					break;
				case OAPI_KEY_NUMPAD6:
					dsky.NumberPressed(6);
					break;
				case OAPI_KEY_NUMPAD7:
					dsky.NumberPressed(7);
					break;
				case OAPI_KEY_NUMPAD8:
					dsky.NumberPressed(8);
					break;
				case OAPI_KEY_NUMPAD9:
					dsky.NumberPressed(9);
					break;
				case OAPI_KEY_NUMPAD0:
					dsky.NumberPressed(0);
					break;
				case OAPI_KEY_A:
					AbortStageSwitch.SetState(0);
					break;
				case OAPI_KEY_K:
					//kill rotation
					SetAngularVel(_V(0, 0, 0));
					break;
			}
		}else{
			// KEY UP
			switch(key){
				case OAPI_KEY_END:
					dsky.ProgReleased();
					break;

			}
		}
		return 0;
	}
	else if (KEYMOD_CONTROL(keystate)) {
		// Do DEDA stuff
		if (down) {
			switch (key) {
			case OAPI_KEY_DECIMAL:
				deda.ClearPressed();
				break;
			case OAPI_KEY_NUMPADENTER:
				deda.EnterPressed();
				break;
			case OAPI_KEY_DIVIDE:
				deda.HoldPressed();
				break;
			case OAPI_KEY_MULTIPLY:
				deda.ReadOutPressed();
				break;
			case OAPI_KEY_ADD:
				deda.PlusPressed();
				break;
			case OAPI_KEY_SUBTRACT:
				deda.MinusPressed();
				break;
			case OAPI_KEY_NUMPAD1:
				deda.NumberPressed(1);
				break;
			case OAPI_KEY_NUMPAD2:
				deda.NumberPressed(2);
				break;
			case OAPI_KEY_NUMPAD3:
				deda.NumberPressed(3);
				break;
			case OAPI_KEY_NUMPAD4:
				deda.NumberPressed(4);
				break;
			case OAPI_KEY_NUMPAD5:
				deda.NumberPressed(5);
				break;
			case OAPI_KEY_NUMPAD6:
				deda.NumberPressed(6);
				break;
			case OAPI_KEY_NUMPAD7:
				deda.NumberPressed(7);
				break;
			case OAPI_KEY_NUMPAD8:
				deda.NumberPressed(8);
				break;
			case OAPI_KEY_NUMPAD9:
				deda.NumberPressed(9);
				break;
			case OAPI_KEY_NUMPAD0:
				deda.NumberPressed(0);
				break;
			}
		}
		else {
			// KEY UP
			switch (key) {
			case OAPI_KEY_DECIMAL:
				deda.ResetKeyDown();
				break;

			}
		}
		return 0;
	}

	if (down){
		switch(key){
			// Valid shaft positions should be:
			// 000.00  00 (F) 
			// 057.30  60 (R)
			// 114.59 120 (Rr)
			// 171.89 180 (CL)
			// 229.18 240 (Lr)
			// 286.48 300 (L)

			case OAPI_KEY_A:
				optics.OpticsShaft--;
				if (optics.OpticsShaft < 0) {
					optics.OpticsShaft = 5; // Clobber
				}
				//Load panel to trigger change of the default camera direction
				if (PanelId == LMPANEL_AOTZOOM)
				{
					oapiSetPanel(LMPANEL_AOTZOOM);
				}
				break;

			case OAPI_KEY_D:
				optics.OpticsShaft++;
				if (optics.OpticsShaft > 5) {
					optics.OpticsShaft = 0; // Clobber
				}
				//Load panel to trigger change of the default camera direction
				if (PanelId == LMPANEL_AOTZOOM)
				{
					oapiSetPanel(LMPANEL_AOTZOOM);
				}
				break;

			case OAPI_KEY_W:
				optics.ReticleMoved = 0.52;  //Fast Rate (about 30 deg/sec)

				if (KEYMOD_ALT(keystate)) {
					optics.ReticleMoved = 0.01;  //Slow Rate (about 0.5 deg/sec)
				}
				break;

			case OAPI_KEY_S:
				optics.ReticleMoved = -0.52;  //Fast Rate (about 30 deg/sec)

				if (KEYMOD_ALT(keystate)) {
					optics.ReticleMoved = -0.01;  //Slow Rate (about 0.5 deg/sec)
				}
				break;
			case OAPI_KEY_Q:
				agc.SetInputChannelBit(016, MarkX, 1);  // Mark X
				break;
			case OAPI_KEY_Y:
				agc.SetInputChannelBit(016, MarkY, 1);  // Mark Y
				break;
			case OAPI_KEY_E:
				agc.SetInputChannelBit(016, MarkReject_LM, 1);  // Mark Reject
				break;
			case OAPI_KEY_MINUS:
				//increase descent rate
				agc.SetInputChannelBit(016, DescendMinus, 1);
				break;
			case OAPI_KEY_EQUALS:
				//decrease descent rate
				agc.SetInputChannelBit(016, DescendPlus, 1);
				break;

			case OAPI_KEY_NUMPAD0:
				//TTCA Throttle up
				ttca_throttle_vel = 1;
				break;
			case OAPI_KEY_DECIMAL:
				//TTCA Throttle down
				ttca_throttle_vel = -1;
				break;

			case OAPI_KEY_K:
				//Mode Control PGNS - cycle between Auto & Att Hold
				if (ModeControlPGNSSwitch.GetState() < 2) {
					ModeControlPGNSSwitch.SetState(2);
				} else {
					ModeControlPGNSSwitch.SetState(1);
				}
				break;

			case OAPI_KEY_M:
				//Throttle Control - cycle between Auto & Man
				if (THRContSwitch.GetState() < 1) {
					THRContSwitch.SetState(1);
				} else {
					THRContSwitch.SetState(0);
				}
				break;

		}
	}else{
		switch(key){
			case OAPI_KEY_W:
			case OAPI_KEY_S:
				optics.ReticleMoved = 0;
				break;
			case OAPI_KEY_Q:
				agc.SetInputChannelBit(016, MarkX, 0);  // Mark X
				break;
			case OAPI_KEY_Y:
				agc.SetInputChannelBit(016, MarkY, 0);  // Mark Y
				break;
			case OAPI_KEY_E:
				agc.SetInputChannelBit(016, MarkReject_LM, 0);  // Mark Reject
				break;
			case OAPI_KEY_MINUS:
				//increase descent rate
				agc.SetInputChannelBit(016, DescendMinus, 0);
				Sclick.play();
				break;
			case OAPI_KEY_EQUALS:
				//decrease descent rate
				agc.SetInputChannelBit(016, DescendPlus, 0);
				Sclick.play();
				break;

			case OAPI_KEY_NUMPAD0:
				ttca_throttle_vel = 0;
				break;
			case OAPI_KEY_DECIMAL:
				ttca_throttle_vel = 0;
				break;
		}

	}

	if (KEYMOD_SHIFT(keystate) || KEYMOD_CONTROL(keystate) || !down) {
		return 0; 
	}

	// MCC CAPCOM interface key handling                                                                                                
	if (down && !KEYMOD_SHIFT(keystate)) {
		switch (key) {
		case OAPI_KEY_TAB:
		case OAPI_KEY_1:
		case OAPI_KEY_2:
		case OAPI_KEY_3:
		case OAPI_KEY_4:
		case OAPI_KEY_5:
		case OAPI_KEY_6:
		case OAPI_KEY_7:
		case OAPI_KEY_8:
		case OAPI_KEY_9:
		case OAPI_KEY_0:
			if (pMCC != NULL)
				pMCC->keyDown(key);
			break;
		}
	}

	switch (key) {

	case OAPI_KEY_E:
		return 0;

	//
	// Used by P64
	//


	case OAPI_KEY_COMMA:
		// move landing site left
		//agc.RedesignateTarget(1,1.0);
		ButtonClick();
		return 1;

	case OAPI_KEY_PERIOD:
		// move landing site right
		//agc.RedesignateTarget(1,-1.0);
		ButtonClick();
		return 1;

	case OAPI_KEY_HOME:
		//move the landing site downrange
		//agc.RedesignateTarget(0,-1.0);
		ButtonClick();
		return 1;

	case OAPI_KEY_END:
		//move the landing site closer
		//agc.RedesignateTarget(0,1.0);
		ButtonClick();
		return 1;

	//
	// Used by P66
	//

	case OAPI_KEY_MINUS:
		//increase descent rate
		//agc.ChangeDescentRate(-0.3077);
		return 1;

	case OAPI_KEY_EQUALS:
		//decrease descent rate
		//agc.ChangeDescentRate(0.3077);
		return 1;	

	//
	// Engine start and stop
	//

	case OAPI_KEY_ADD:
		//Engine Start Button
		ManualEngineStart.Push();
		ButtonClick();
		return 1;
	case OAPI_KEY_SUBTRACT:
		//Engine Stop Button
		CDRManualEngineStop.Push();
		ButtonClick();
		return 1;
	}

	return 0;
}

//
// Timestep code.
//

void LEM::clbkPreStep (double simt, double simdt, double mjd) {

	if (CheckPanelIdInTimestep) {
		oapiSetPanel(PanelId);
		CheckPanelIdInTimestep = false;
	}

	if (RefreshPanelIdInTimestep && oapiCameraInternal()) {
		oapiSetPanel(PanelId);
		RefreshPanelIdInTimestep = false;
	}

	if (FirstTimestep)
	{
		DoFirstTimestep();
		FirstTimestep = false;
		return;
	}

	// Prevent Orbiter navmodes from doing stuff
	if (GetNavmodeState(NAVMODE_HOLDALT))
	{
		DeactivateNavmode(NAVMODE_HOLDALT);
	}

	//
	// Internal/External view check
	//

	if (!ExtView && !oapiCameraInternal()) {
		ExtView = true;
		SetLMMeshVis();
		if (!InFOV) oapiCameraSetAperture(SaveFOV);
	}

	if (ExtView && oapiCameraInternal()) {
		ExtView = false;
		SetLMMeshVis();
		SetView();
	}

	//
	// Update mission time.
	//

	MissionTime += simdt;

	//
	// Panel flash counter.
	//

	if (MissionTime >= NextFlashUpdate) {
		PanelFlashOn = !PanelFlashOn;
		NextFlashUpdate = MissionTime + 0.25;
	}

	SystemsTimestep(MissionTime, simdt);

	// RCS propellant pressurization
	// Descent Propellant Tank Prepressurization (ambient helium)
	// Descent Propellant Tank Prepressurization (supercritical helium)
	// Descent Propellant Tank Venting
	// Ascent Propellant Tank Pressurization
	if (status < 2) {
		if (StagingBoltsPyros.Blown() && StagingNutsPyros.Blown() && CableCuttingPyros.Blown()) {
			AbortFire();
			// Stage
			SeparateStage(stage);
		}
	}

	// Delete LM/SLA docking port at LM extraction from SIVB
	if (docksla && !DockingStatus(1)) {
		DelDock(docksla);
		docksla = NULL;
	}

	// Debug string for displaying descent flight info from VC view
	if (VcInfoEnabled && !Landed && GetAltitude(ALTMODE_GROUND) < 10000.0 && EngineArmSwitch.GetState() == 0 && oapiCockpitMode() == COCKPIT_VIRTUAL && viewpos == LMVIEW_LPD) {

		char pgnssw[256];
		char thrsw[256];

		if (ModeControlPGNSSwitch.GetState() == 2) {
			sprintf(pgnssw, "AUTO");
		} else if (ModeControlPGNSSwitch.GetState() == 1) {
			sprintf(pgnssw, "ATT HOLD");
		} else {
			sprintf(pgnssw, "OFF");
		}

		if (THRContSwitch.GetState() == 1) {
			sprintf(thrsw, "AUTO");
		} else {
			sprintf(thrsw, "MAN");
		}

		sprintf(oapiDebugString(), "PROG %s | Alt: %.0lf ft | Alt Rate: %.1lf ft/s | PGNS Mode Control: %s | Throttle: %s | Fuel: %.0lf %%", dsky.GetProg(), RadarTape.GetLGCAltitude() * 3.2808399, RadarTape.GetLGCAltitudeRate() * 3.2808399, pgnssw, thrsw, DPSFuelPercentMeter.QueryValue() * 100);
		if (!VcInfoActive) VcInfoActive = true;

	} else {
		if (VcInfoActive) {
			sprintf(oapiDebugString(), "");
			VcInfoActive = false;
		}
	}
}


void LEM::clbkPostStep(double simt, double simdt, double mjd)
{
	// Simulate the dust kicked up near
	// the lunar surface
	double vsAlt = GetAltitude(ALTMODE_GROUND);
	if (th_hover[0])
	{
		double dustlvl = min(1.0, max(0.0, GetThrusterLevel(th_hover[0]))*(-(vsAlt - 2.0) / 15.0 + 1.0));

		if (stage < 2 && thg_dust) {
			if (vsAlt < 15.0) {
				SetThrusterGroupLevel(thg_dust, dustlvl);
			}
			else {
				SetThrusterGroupLevel(thg_dust, 0);
			}
		}
	}

	//
	// Camera jostle.
	//

	ViewOffsetx *= 0.95;
	ViewOffsety *= 0.95;
	ViewOffsetz *= 0.95;

	if (th_hover[0] && !ExtView)
	{
		if ((GetThrusterLevel(th_hover[0]) > 0) && InVC)
		{
			double amt = max(0.02, GetThrusterLevel(th_hover[0]) / 20);
			JostleViewpoint(amt);
		}
	}

	//
	// If we switch focus to the astronaut immediately after creation, Orbitersound doesn't
	// play any sounds, or plays LEM sounds rather than astronauts sounds. We need to delay
	// the focus switch a few timesteps to allow it to initialise properly in the background.
	//

	if (SwitchFocusToLeva > 0 && hLEVA) {
		SwitchFocusToLeva--;
		if (!SwitchFocusToLeva) {
			oapiSetFocusObject(hLEVA);
		}
	}

	// DS20160916 Physical parameters updation
	CurrentFuelWeight = 0;
	if (ph_Asc != NULL) { CurrentFuelWeight += GetPropellantMass(ph_Asc); }
	if (ph_Dsc != NULL) { CurrentFuelWeight += GetPropellantMass(ph_Dsc); }
	if (ph_RCSA != NULL) { CurrentFuelWeight += GetPropellantMass(ph_RCSA); }
	if (ph_RCSB != NULL) { CurrentFuelWeight += GetPropellantMass(ph_RCSB); }
	// If the weight has changed by more than this value, update things.
	// The value is to be adjusted such that the updates are not too frequent (impacting framerate)
	// but are sufficiently fine to keep the LGC happy.
	if ((LastFuelWeight - CurrentFuelWeight) > 100.0) {
		// Update physical parameters
		VECTOR3 pmi, CoG;
		CalculatePMIandCOG(pmi, CoG);
		// Use SetPMI, ShiftCG, etc.
		VECTOR3 CoGShift = CoG - currentCoG;
		ShiftCG(CoGShift);
		SetPMI(pmi);
		currentCoG = CoG;

		//Touchdown Points
		DefineTouchdownPoints(stage);

		//Lights
		trackLightPos -= CoGShift;
		for (int i = 0;i < 5;i++)
		{
			dockingLightsPos[i] -= CoGShift;
		}

		// All done!
		LastFuelWeight = CurrentFuelWeight;
	}

	//
	// Play RCS sound in case of Orbiter's attitude control is disabled
	//

	RCSSoundTimestep();

	if (stage == 0 || pMission->LMHasLegs() == false)	{


	}else if (stage == 1 || stage == 5)	{

		if (CDREVA_IP) {
			if(!hLEVA) {
				ToggleEVA();
			}
		}

		if (ToggleEva && GroundContact()){
			ToggleEVA();
		}

		double vsAlt = GetAltitude(ALTMODE_GROUND);
		if (!Landed && (GroundContact() || (vsAlt < 1.0))) {

#ifdef DIRECTSOUNDENABLED
			if (!sevent.isValid())
#endif
				Scontact.play();

			SetLmLandedMesh();
		}

		//sprintf (oapiDebugString(),"FUEL %d",GetPropellantMass(ph_Dsc));
	}

	MainPanel.timestep(MissionTime);
	checkControl.timestep(MissionTime, DummyEvents);

    // x15 landing sound management
#ifdef DIRECTSOUNDENABLED

	char names [255]         ;
	int        todo          ;
	double     offset        ;
	int        newbuffer     ;

	
	if(simt >NextEventTime)
	{
        NextEventTime=simt+0.1;
    	todo = sevent.play(soundlib,
			    this,
				names,
				&offset,
				&newbuffer,
		        -1.0,
				MissionTime,
				-1,
				99999.0,
				-1.0,
				-1.0,
				NOLOOP,
				255);
        if (todo)
		{
           if(offset > 0.)
                sevent.PlaySound( names,newbuffer,offset);
		   else sevent.PlaySound( names,true,0);
		}
	} 
#endif
}

//
// Scenario state functions.
//

void LEM::clbkLoadStateEx (FILEHANDLE scn, void *vs)
{
	GetScenarioState(scn, vs);

	SetGenericStageState(status);

	PostLoadSetup();
}

void LEM::SetGenericStageState(int stat)
{
	switch (stat) {
	case 0:
		stage = 0;
		SetLmVesselDockStage();
		break;

	case 1:
		stage = 1;
		SetLmVesselDockStage();
		SetLmVesselHoverStage();

		if (CDREVA_IP) {
			SetupEVA();
		}
		break;

	case 2:
		stage = 2;
		SetLmVesselDockStage();
		SetLmAscentHoverStage();
		break;
	}
}

void LEM::PostLoadSetup(bool define_anims)
{
	CheckDescentStageSystems();
	if (define_anims) DefineAnimations();

	///
	// Realism Mode Settings
	//

	// Enable Orbiter's attitude control for unmanned missions
	// as long as they rely on Orbiter's navmodes (killrot etc.)

	if (!Crewed) {
		checkControl.autoExecute(true);
		checkControl.autoExecuteSlow(false);
		checkControl.autoExecuteAllItemsAutomatic(true);
	}

	// Disable it and do some other settings when not in
	// Quickstart mode

	else {
		checkControl.autoExecute(VAGCChecklistAutoEnabled);
		checkControl.autoExecuteSlow(VAGCChecklistAutoSlow);
		checkControl.autoExecuteAllItemsAutomatic(false);

	}

	//
	// Load sounds, this is mandatory if loading in cockpit view, 
	// because OrbiterSound crashes when loading sounds during clbkLoadPanel
	//
	LoadDefaultSounds();

	// Also cause the AC busses to wire up
	switch (EPSInverterSwitch.GetState()) {
	case THREEPOSSWITCH_UP:      // INV 2
		ACBusA.WireTo(&AC_A_INV_2_FEED_CB);
		ACBusB.WireTo(&AC_B_INV_2_FEED_CB);
		break;
	case THREEPOSSWITCH_CENTER:  // INV 1
		ACBusA.WireTo(&AC_A_INV_1_FEED_CB);
		ACBusB.WireTo(&AC_B_INV_1_FEED_CB);
		break;
	case THREEPOSSWITCH_DOWN:    // OFF	
		break;                   // Handled later
	}

	HRESULT         hr;
	// Having read the configuration file, set up DirectX...	
	hr = DirectInput8Create(dllhandle, DIRECTINPUT_VERSION, IID_IDirectInput8, (void **)&dx8ppv, NULL); // Give us a DirectInput context
	if (!FAILED(hr)) {
		int x = 0;
		// Enumerate attached joysticks until we find 2 or run out.
		dx8ppv->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumJoysticksCallback, this, DIEDFL_ATTACHEDONLY);
		if (js_enabled == 0) {   // Did we get anything?			
			dx8ppv->Release(); // No. Close down DirectInput
			dx8ppv = NULL;     // otherwise it won't get closed later
			//sprintf(oapiDebugString(), "DX8JS: No joysticks found");
		}
		else {
			while (x < js_enabled) {                                // For each joystick
				dx8_joystick[x]->SetDataFormat(&c_dfDIJoystick2); // Use DIJOYSTATE2 structure to report data
				dx8_jscaps[x].dwSize = sizeof(dx8_jscaps[x]);     // Initialize size of capabilities data structure
				dx8_joystick[x]->GetCapabilities(&dx8_jscaps[x]); // Get capabilities
																  // Z-axis detection
				if ((rhc_id == x && rhc_auto) || (thc_id == x && thc_auto)) {
					js_current = x;
					dx8_joystick[x]->EnumObjects(EnumAxesCallback, this, DIDFT_AXIS | DIDFT_POV);
				}
				x++;                                              // Next!
			}
		}
	}
	else {
		// We can't print an error message this early in initialization, so save this reason for later investigation.
		dx8_failure = hr;
	}
}

void LEM::GetScenarioState(FILEHANDLE scn, void *vs)
{
	char *line;
	int	SwitchState;
	float ftcp;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "CONFIGURATION", 13)) {
			sscanf(line + 13, "%d", &status);
		}
		else if (!strnicmp(line, "EVA", 3)) {
			CDREVA_IP = true;
		}
		else if (!strnicmp(line, "CSWITCH", 7)) {
			SwitchState = 0;
			sscanf(line + 7, "%d", &SwitchState);
			SetCSwitchState(SwitchState);
		}
		else if (!strnicmp(line, "MISSNTIME", 9)) {
			sscanf(line + 9, "%f", &ftcp);
			MissionTime = ftcp;
		}
		else if (!strnicmp(line, "UNMANNED", 8)) {
			int i;
			sscanf(line + 8, "%d", &i);
			Crewed = (i == 0);
		}
		else if (!strnicmp(line, "LANG", 4)) {
			strncpy(AudioLanguage, line + 5, 64);
		}
		else if (!strnicmp(line, "APOLLONO", 8)) {
			sscanf(line + 8, "%d", &ApolloNo);

			if (sscanf(line + 8, "%d", &ApolloNo) == 1)
			{
				pMission->LoadMission(ApolloNo);
			}
			else
			{
				char tempBuffer[64];
				strncpy(tempBuffer, line + 9, 63);
				pMission->LoadMission(tempBuffer);
			}
			CreateMissionSpecificSystems();
		}
		else if (!strnicmp(line, "LANDED", 6)) {
			sscanf(line + 6, "%d", &Landed);
		}
		else if (!strnicmp(line, "DSCFUEL", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			DescentFuelMassKg = ftcp;
		}
		else if (!strnicmp(line, "ASCFUEL", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			AscentFuelMassKg = ftcp;
		}
		else if (!strnicmp(line, "DSCEMPTYMASS", 12)) {
			sscanf(line + 12, "%f", &ftcp);
			DescentEmptyMassKg = ftcp;
		}
		else if (!strnicmp(line, "ASCEMPTYMASS", 12)) {
			sscanf(line + 12, "%f", &ftcp);
			AscentEmptyMassKg = ftcp;
		}
		else if (!strnicmp(line, "FDAIDISABLED", 12)) {
			sscanf(line + 12, "%i", &fdaiDisabled);
		}
		else if (!strnicmp(line, "SAVEFOV", 7)) {
			sscanf(line + 7, "%f", &ftcp);
			SaveFOV = ftcp;
		}
		else if (!strnicmp(line, "INFOV", 5)) {
			int i;
			sscanf(line + 5, "%d", &i);
			InFOV = (i == 1);
		}
		else if (!strnicmp(line, "ORDEALENABLED", 13)) {
			sscanf(line + 13, "%i", &ordealEnabled);
		}
		else if (!strnicmp(line, "CDRINPLSS", 9)) {
			sscanf(line + 9, "%i", &CDRinPLSS);
		}
		else if (!strnicmp(line, "LMPINPLSS", 9)) {
			sscanf(line + 9, "%i", &LMPinPLSS);
		}
		else if (!strnicmp(line, DSKY_START_STRING, sizeof(DSKY_START_STRING))) {
			dsky.LoadState(scn, DSKY_END_STRING);
		}
		else if (!strnicmp(line, AGC_START_STRING, sizeof(AGC_START_STRING))) {
			agc.LoadState(scn);
		}
		else if (!strnicmp(line, IMU_START_STRING, sizeof(IMU_START_STRING))) {
			imu.LoadState(scn);
		}
		else if (!strnicmp(line, "SCDU_START", sizeof("SCDU_START"))) {
			scdu.LoadState(scn, "CDU_END");
		}
		else if (!strnicmp(line, "TCDU_START", sizeof("TCDU_START"))) {
			tcdu.LoadState(scn, "CDU_END");
		}
		else if (!strnicmp(line, "DEDA_START", sizeof("DEDA_START"))) {
			deda.LoadState(scn, "DEDA_END");
		}
		else if (!strnicmp(line, "AEA_START", sizeof("AEA_START"))) {
			aea.LoadState(scn, "AEA_END");
		}
		else if (!strnicmp(line, "ASA_START", sizeof("ASA_START"))) {
			asa.LoadState(scn, "ASA_END");
		}
		else if (!strnicmp(line, "ECA_1A_START", sizeof("ECA_1A_START"))) {
			ECA_1a.LoadState(scn, "ECA_1A_END");
		}
		else if (!strnicmp(line, "ECA_2A_START", sizeof("ECA_2A_START"))) {
			ECA_2a.LoadState(scn, "ECA_2A_END");
		}
		else if (!strnicmp(line, "ECA_1B_START", sizeof("ECA_1B_START"))) {
			ECA_1b.LoadState(scn, "ECA_1B_END");
		}
		else if (!strnicmp(line, "ECA_2B_START", sizeof("ECA_2B_START"))) {
			ECA_2b.LoadState(scn, "ECA_2B_END");
		}
		else if (!strnicmp(line, "ECA_3A_START", sizeof("ECA_3A_START"))) {
			ECA_3a.LoadState(scn, "ECA_3A_END");
		}
		else if (!strnicmp(line, "ECA_4A_START", sizeof("ECA_4A_START"))) {
			ECA_4a.LoadState(scn, "ECA_4A_END");
		}
		else if (!strnicmp(line, "ECA_3B_START", sizeof("ECA_3B_START"))) {
			ECA_3b.LoadState(scn, "ECA_3B_END");
		}
		else if (!strnicmp(line, "ECA_4B_START", sizeof("ECA_4B_START"))) {
			ECA_4b.LoadState(scn, "ECA_4B_END");
		}
		else if (!strnicmp(line, "BTC_XLUNAR_START", sizeof("BTC_XLUNAR_START"))) {
			BTC_XLunar.LoadState(scn, "BTC_XLUNAR_END");
		}
		else if (!strnicmp(line, "UNIFIEDSBAND", 12)) {
			SBand.LoadState(line);
		}
		else if (!strnicmp(line, "STEERABLEANTENNA", 16)) {
			SBandSteerable.LoadState(line);
		}
		else if (!strnicmp(line, "VHFTRANSCEIVER", 14)) {
			VHF.LoadState(line);
		}
		else if (!strnicmp(line, "LCA_START", sizeof("LCA_START"))) {
			lca.LoadState(scn,"LCA_END");
		}
		else if (!strnicmp(line, CWEA_START_STRING, sizeof(CWEA_START_STRING))) {
			CWEA.LoadState(scn, CWEA_END_STRING);
		}
		else if (!strnicmp(line, "FORWARDHATCH", 12)) {
			ForwardHatch.LoadState(line);
		}
		else if (!strnicmp(line, "OVERHEADHATCH", 13)) {
			OverheadHatch.LoadState(line);
		}
		else if (!strnicmp(line, "PRIMGLYPUMPCONTROLLER", 21)) {
			PrimGlycolPumpController.LoadState(line);
		}
		else if (!strnicmp(line, "SUITFANDPSENSOR", 15)) {
			SuitFanDPSensor.LoadState(line);
		}
		else if (!strnicmp(line, "CABINPRESSURESWITCH", 19)) {
			CabinPressureSwitch.LoadState(line, 19);
		}
		else if (!strnicmp(line, "SUITPRESSURESWITCH", 18)) {
			SuitPressureSwitch.LoadState(line, 18);
		}
		else if (!strnicmp(line, "CREWSTATUS", 10)) {
			CrewStatus.LoadState(line);
		}
		else if (!strnicmp(line, "PANEL_ID", 8)) {
			sscanf(line + 8, "%d", &PanelId);
		}
		else if (!strnicmp(line, PANELSWITCH_START_STRING, strlen(PANELSWITCH_START_STRING))) {
			PSH.LoadState(scn);
		}
		else if (!strnicmp(line, "LEM_EDS_START", sizeof("LEM_EDS_START"))) {
			eds.LoadState(scn, "LEM_EDS_END");
		}
		else if (!strnicmp(line, "LEM_RR_START", sizeof("LEM_RR_START"))) {
			RR.LoadState(scn, "LEM_RR_END");
		}
		else if (!strnicmp(line, "LEM_LR_START", sizeof("LEM_LR_START"))) {
			LR.LoadState(scn, "LEM_LR_END");
		}
		else if (!strnicmp(line, "RADARTAPE_START", sizeof("RADARTAPE_START"))) {
			RadarTape.LoadState(scn, "RADARTAPE_END");
		}
		else if (!strnicmp(line, LMOPTICS_START_STRING, sizeof(LMOPTICS_START_STRING))) {
			optics.LoadState(scn);
		}
		else if (!strnicmp(line, FDAI_START_STRING, sizeof(FDAI_START_STRING))) {
			fdaiLeft.LoadState(scn, FDAI_END_STRING);
		}
		else if (!strnicmp(line, FDAI2_START_STRING, sizeof(FDAI2_START_STRING))) {
			fdaiRight.LoadState(scn, FDAI2_END_STRING);
		}
		else if (!strnicmp(line, DPSPROPELLANT_START_STRING, sizeof(DPSPROPELLANT_START_STRING))) {
			DPSPropellant.LoadState(scn);
		}
		else if (!strnicmp(line, "DPS_BEGIN", sizeof("DPS_BEGIN"))) {
			DPS.LoadState(scn, "DPS_END");
		}
		else if (!strnicmp(line, "DPSGIMBALACTUATOR_PITCH_BEGIN", sizeof("DPSGIMBALACTUATOR_PITCH_BEGIN"))) {
			DPS.pitchGimbalActuator.LoadState(scn);
		}
		else if (!strnicmp(line, "DPSGIMBALACTUATOR_ROLL_BEGIN", sizeof("DPSGIMBALACTUATOR_ROLL_BEGIN"))) {
			DPS.rollGimbalActuator.LoadState(scn);
		}
		else if (!strnicmp(line, "DECA_BEGIN", sizeof("DECA_BEGIN"))) {
			deca.LoadState(scn);
		}
		else if (!strnicmp(line, "SCCA1_BEGIN", sizeof("SCCA1_BEGIN"))) {
			scca1.LoadState(scn, "SCCA_END");
		}
		else if (!strnicmp(line, "SCCA2_BEGIN", sizeof("SCCA2_BEGIN"))) {
			scca2.LoadState(scn, "SCCA_END");
		}
		else if (!strnicmp(line, "SCCA3_BEGIN", sizeof("SCCA3_BEGIN"))) {
			scca3.LoadState(scn, "SCCA_END");
		}
		else if (!strnicmp(line, APSPROPELLANT_START_STRING, sizeof(APSPROPELLANT_START_STRING))) {
			APSPropellant.LoadState(scn);
		}
		else if (!strnicmp(line, "APS_BEGIN", sizeof("APS_BEGIN"))) {
			APS.LoadState(scn, "APS_END");
		}
		else if (!strnicmp(line, "RCSPROPELLANT_A_BEGIN", sizeof("RCSPROPELLANT_A_BEGIN"))) {
			RCSA.LoadState(scn, "RCSPROPELLANT_END");
		}
		else if (!strnicmp(line, "RCSPROPELLANT_B_BEGIN", sizeof("RCSPROPELLANT_B_BEGIN"))) {
			RCSB.LoadState(scn, "RCSPROPELLANT_END");
		}
		else if (!strnicmp(line, "RCSTCA_1A_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca1A.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, "RCSTCA_2A_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca2A.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, "RCSTCA_3A_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca3A.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, "RCSTCA_4A_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca4A.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, "RCSTCA_1B_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca1B.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, "RCSTCA_2B_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca2B.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, "RCSTCA_3B_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca3B.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, "RCSTCA_4B_BEGIN", sizeof("RCSTCA_1A_BEGIN"))) {
			tca4B.LoadState(scn, "RCSTCA_END");
		}
		else if (!strnicmp(line, ORDEAL_START_STRING, sizeof(ORDEAL_START_STRING))) {
			ordeal.LoadState(scn);
		}
		else if (!strnicmp(line, MECHACCEL_START_STRING, sizeof(MECHACCEL_START_STRING))) {
			mechanicalAccelerometer.LoadState(scn);
		}
		else if (!strnicmp(line, ATCA_START_STRING, sizeof(ATCA_START_STRING))) {
			atca.LoadState(scn);
		}
		else if (!strnicmp(line, "MISSIONTIMER_START", sizeof("MISSIONTIMER_START"))) {
			MissionTimerDisplay.LoadState(scn, MISSIONTIMER_END_STRING);
		}
		else if (!strnicmp(line, "EVENTTIMER_START", sizeof("EVENTTIMER_START"))) {
			EventTimerDisplay.LoadState(scn, EVENTTIMER_END_STRING);
		}
		else if (!strnicmp(line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
			Panelsdk.Load(scn);			//send the loading to the Panelsdk
		}
		else if (!strnicmp(line, ChecklistControllerStartString, strlen(ChecklistControllerStartString)))
		{
			checkControl.load(scn);
		}
		else
		{
			ParseScenarioLineEx(line, vs);
		}
	}
}

void LEM::clbkSetClassCaps (FILEHANDLE cfg) {

	//
	// Scan the launchpad config file.
	//
	char *line, buffer[1000];

	sprintf(buffer, "%s.launchpad.cfg", GetClassName());
	FILEHANDLE hFile = oapiOpenFile(buffer, FILE_IN, CONFIG);

	while (oapiReadScenario_nextline(hFile, line)) {
		ProcessConfigFileLine(hFile, line);
	}
	oapiCloseFile(hFile, FILE_IN);

	// Load all meshes here so that DEVMESHHANDLE'S properly initialize at initial LM creation.
	SetMeshes();
}

void LEM::clbkPostCreation()
{
	//Find MCC, if it exists
	hMCC = oapiGetVesselByName("MCC");
	if (hMCC != NULL) {
		VESSEL* pVessel = oapiGetVesselInterface(hMCC);
		if (pVessel) {
			if (!_strnicmp(pVessel->GetClassName(), "ProjectApollo\\MCC", 17)
				|| !_strnicmp(pVessel->GetClassName(), "ProjectApollo/MCC", 17)) pMCC = static_cast<MCC*>(pVessel);
		}
		else pMCC = NULL;
	}

	// Delete LM/SLA docking port if LM extracted from SIVB
	if (docksla && !DockingStatus(1)) {
		DelDock(docksla);
		docksla = NULL;
	}

	CreateAirfoils();
}

void LEM::clbkVisualCreated(VISHANDLE vis, int refcount)
{
	if (ascidx != -1) {
		drogue = GetDevMesh(vis, ascidx);
		DrogueVis();
		cdrmesh = GetDevMesh(vis, ascidx);
		lmpmesh = GetDevMesh(vis, ascidx);
		SetCrewMesh();
	}

	if (dscidx != -1 && pMission->LMHasLegs()) {
		probes = GetDevMesh(vis, dscidx);
		HideProbes();
	}

	if (vcidx != -1) vcmesh = GetDevMesh(vis, vcidx);
}

void LEM::clbkVisualDestroyed(VISHANDLE vis, int refcount)
{
	drogue = NULL;
	probes = NULL;
	cdrmesh = NULL;
	lmpmesh = NULL;
	vcmesh = NULL;
}

void LEM::clbkDockEvent(int dock, OBJHANDLE connected)
{
	//For now restrict this to docking port 1 (aka LM/SLA connection)
	if (dock == 1)
	{
		if (connected)
		{
			DockConnectors(dock);
		}
		else
		{
			UndockConnectors(dock);
			CreateAirfoils();
		}
	}
}

void LEM::DefineAnimations()
{
	// Call Animation Definitions where required
	RR.DefineAnimations(ascidx);
	SBandSteerable.DefineAnimations(ascidx);
	OverheadHatch.DefineAnimations(ascidx);
	ForwardHatch.DefineAnimations(ascidx);
	OverheadHatch.DefineAnimationsVC(vcidx);
	ForwardHatch.DefineAnimationsVC(vcidx);
	if (stage < 2) DPS.DefineAnimations(dscidx);
	if (stage < 1 && pMission->LMHasLegs()) eds.DefineAnimations(dscidx);
	DefineVCAnimations();
}

bool LEM::ProcessConfigFileLine(FILEHANDLE scn, char *line)

{
	int i;

	if (!strnicmp(line, "FDAIDISABLED", 12)) {
		sscanf(line + 12, "%i", &fdaiDisabled);
	}
	else if (!strnicmp(line, "FDAISMOOTH", 10)) {
			sscanf(line + 10, "%i", &fdaiSmooth);
	}
	else if (!strnicmp (line, "MULTITHREAD", 11)) {
		int value;
		sscanf (line+11, "%d", &value);
		isMultiThread=(value>0)?true:false;
	}
	else if (!strnicmp (line, "JOYSTICK_RHC", 12)) {
		sscanf (line + 12, "%i", &rhc_id);
		if(rhc_id > 1){ rhc_id = 1; } // Be paranoid
	}
	else if (!strnicmp (line, "JOYSTICK_RTTID", 14)) {
		sscanf (line + 14, "%i", &rhc_thctoggle_id);
		if (rhc_thctoggle_id > 128){ rhc_thctoggle_id = 128; } // Be paranoid
	}
	else if (!strnicmp (line, "JOYSTICK_RRT", 12)) {
		sscanf (line + 12, "%i", &rhc_rot_id);
		if(rhc_rot_id > 2){ rhc_rot_id = 2; } // Be paranoid
	}
	else if (!strnicmp (line, "JOYSTICK_RSL", 12)) {
		sscanf (line + 12, "%i", &rhc_sld_id);
		if(rhc_sld_id > 2){ rhc_sld_id = 2; } // Be paranoid
	}
	else if (!strnicmp (line, "JOYSTICK_RZX", 12)) {
		sscanf (line + 12, "%i", &rhc_rzx_id);
	}
	else if (!strnicmp (line, "JOYSTICK_THC", 12)) {
		sscanf (line + 12, "%i", &thc_id);
		if(thc_id > 1){ thc_id = 1; } // Be paranoid
	}
	else if (!strnicmp (line, "JOYSTICK_TRT", 12)) {
		sscanf (line + 12, "%i", &thc_rot_id);
		if(thc_rot_id > 2){ thc_rot_id = 2; } // Be paranoid
	}
	else if (!strnicmp(line, "JOYSTICK_TSL", 12)) {
		sscanf(line + 12, "%i", &thc_tjt_id);
		if (thc_tjt_id > 2) { thc_tjt_id = 2; } // Be paranoid
	}
	else if (!strnicmp (line, "JOYSTICK_TZX", 12)) {
		thc_rzx_id = 1;
	}
	else if (!strnicmp (line, "JOYSTICK_RDB", 12)) {
		rhc_debug = 1;
	}
	else if (!strnicmp (line, "JOYSTICK_TDB", 12)) {
		thc_debug = 1;
	}
	else if (!strnicmp (line, "JOYSTICK_RAUTO", 14)) {
		rhc_auto = 1;
	}
	else if (!strnicmp (line, "JOYSTICK_TAUTO", 14)) {
		thc_auto = 1;
	}
	else if (!strnicmp (line, "JOYSTICK_RTT", 12)) {
		sscanf(line + 12, "%i", &i);
		rhc_thctoggle = (i != 0);
	}
	else if (!strnicmp(line, "VAGCCHECKLISTAUTOSLOW", 21)) {
		sscanf(line + 21, "%i", &i);
		VAGCChecklistAutoSlow = (i != 0);
	}
	else if (!strnicmp(line, "VAGCCHECKLISTAUTOENABLED", 24)) {
		int i;
		sscanf(line + 24, "%d", &i);
		VAGCChecklistAutoEnabled = (i != 0);
	}
	else if (!strnicmp(line, "CHKVAR_", 7)) {
		for (int i = 0; i < 16; i++) {
			char name[16];
			sprintf(name, "CHKVAR_%d", i);
			if (!strnicmp(line, name, strlen(name))) {
				strncpy(Checklist_Variable[i], line + (strlen(name) + 1), 32);
				break;
			}
		}
	}
	else if (!strnicmp(line, "VCINFOENABLED", 13)) {
		int i;
		sscanf(line + 13, "%d", &i);
		VcInfoEnabled = (i != 0);
	}
	return true;
}

void LEM::SetStateEx(const void *status)

{
	const VESSELSTATUS2 *vslm = static_cast<const VESSELSTATUS2 *> (status);

	DefSetStateEx(status);
}

void LEM::clbkSaveState (FILEHANDLE scn)

{
	SaveDefaultState (scn);	
	oapiWriteScenario_int (scn, "CONFIGURATION", status);
	if (CDREVA_IP){
		oapiWriteScenario_int (scn, "EVA", int(TO_EVA));
	}

	oapiWriteScenario_int (scn, "CSWITCH",  GetCSwitchState());
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_string (scn, "LANG", AudioLanguage);
	oapiWriteScenario_int (scn, "PANEL_ID", PanelId);	

	oapiWriteScenario_int (scn, "APOLLONO", ApolloNo);
	oapiWriteScenario_int (scn, "LANDED", Landed);
	oapiWriteScenario_int (scn, "FDAIDISABLED", fdaiDisabled);
	oapiWriteScenario_float(scn, "SAVEFOV", SaveFOV);
	papiWriteScenario_bool(scn, "INFOV", InFOV);
	oapiWriteScenario_int(scn, "ORDEALENABLED", ordealEnabled);
	oapiWriteScenario_int(scn, "CDRINPLSS", CDRinPLSS);
	oapiWriteScenario_int(scn, "LMPINPLSS", LMPinPLSS);

	oapiWriteScenario_float (scn, "DSCFUEL", DescentFuelMassKg);
	oapiWriteScenario_float (scn, "ASCFUEL", AscentFuelMassKg);
	oapiWriteScenario_float(scn, "DSCEMPTYMASS", DescentEmptyMassKg);
	oapiWriteScenario_float(scn, "ASCEMPTYMASS", AscentEmptyMassKg);

	if (!Crewed) {
		oapiWriteScenario_int (scn, "UNMANNED", 1);
	}

	for (int i = 0; i < 16; i++) {
		if (Checklist_Variable[i][0] != 0) {
			char name[16];
			sprintf(name, "CHKVAR_%d", i);
			oapiWriteScenario_string(scn, name, Checklist_Variable[i]);
		}
	}

	dsky.SaveState(scn, DSKY_START_STRING, DSKY_END_STRING);
	agc.SaveState(scn);
	imu.SaveState(scn);
	scdu.SaveState(scn, "SCDU_START", "CDU_END");
	tcdu.SaveState(scn, "TCDU_START", "CDU_END");
	deda.SaveState(scn, "DEDA_START", "DEDA_END");
	if (pMission->HasAEA())
	{
		aea.SaveState(scn, "AEA_START", "AEA_END");
		asa.SaveState(scn, "ASA_START", "ASA_END");
	}

	//
	// Save the Panel SDK state.
	//

	Panelsdk.Save(scn);
	
	//
	// Save the state of the switches
	//

	PSH.SaveState(scn);	

	// Save ECAs
	ECA_1a.SaveState(scn,"ECA_1A_START","ECA_1A_END");
	ECA_1b.SaveState(scn,"ECA_1B_START","ECA_1B_END");
	ECA_2a.SaveState(scn,"ECA_2A_START","ECA_2A_END");
	ECA_2b.SaveState(scn,"ECA_2B_START","ECA_2B_END");
	ECA_3a.SaveState(scn,"ECA_3A_START","ECA_3A_END");
	ECA_3b.SaveState(scn,"ECA_3B_START","ECA_3B_END");
	ECA_4a.SaveState(scn,"ECA_4A_START","ECA_4A_END");
	ECA_4b.SaveState(scn,"ECA_4B_START","ECA_4B_END");
	BTC_XLunar.SaveState(scn, "BTC_XLUNAR_START", "BTC_XLUNAR_END");

	// Save COMM
	SBand.SaveState(scn);
	SBandSteerable.SaveState(scn);
	VHF.SaveState(scn);

	// Save Lighting
	lca.SaveState(scn, "LCA_START", "LCA_END");

	// Save CWEA
	CWEA.SaveState(scn, CWEA_START_STRING, CWEA_END_STRING);

	// Save ECS
	ForwardHatch.SaveState(scn);
	OverheadHatch.SaveState(scn);
	PrimGlycolPumpController.SaveState(scn);
	SuitFanDPSensor.SaveState(scn);
	CabinPressureSwitch.SaveState(scn, "CABINPRESSURESWITCH");
	SuitPressureSwitch.SaveState(scn, "SUITPRESSURESWITCH");
	CrewStatus.SaveState(scn);

	// Save EDS
	eds.SaveState(scn,"LEM_EDS_START","LEM_EDS_END");
	RR.SaveState(scn,"LEM_RR_START","LEM_RR_END");
	LR.SaveState(scn, "LEM_LR_START", "LEM_LR_END");
	RadarTape.SaveState(scn, "RADARTAPE_START", "RADARTAPE_END");

	//Save Optics
	optics.SaveState(scn);

	// Save FDAIs
	fdaiLeft.SaveState(scn, FDAI_START_STRING, FDAI_END_STRING);
	fdaiRight.SaveState(scn, FDAI2_START_STRING, FDAI2_END_STRING);

	//Save DPS
	DPSPropellant.SaveState(scn);
	DPS.SaveState(scn, "DPS_BEGIN", "DPS_END");
	//Save pitch and roll gimbal actuators
	oapiWriteLine(scn, "DPSGIMBALACTUATOR_PITCH_BEGIN");
	DPS.pitchGimbalActuator.SaveState(scn);
	oapiWriteLine(scn, "DPSGIMBALACTUATOR_ROLL_BEGIN");
	DPS.rollGimbalActuator.SaveState(scn);
	oapiWriteLine(scn, "DECA_BEGIN");
	deca.SaveState(scn);
	scca1.SaveState(scn, "SCCA1_BEGIN", "SCCA_END");
	scca2.SaveState(scn, "SCCA2_BEGIN", "SCCA_END");
	scca3.SaveState(scn, "SCCA3_BEGIN", "SCCA_END");
	APSPropellant.SaveState(scn);
	APS.SaveState(scn, "APS_BEGIN", "APS_END");
	RCSA.SaveState(scn, "RCSPROPELLANT_A_BEGIN", "RCSPROPELLANT_END");
	RCSB.SaveState(scn, "RCSPROPELLANT_B_BEGIN", "RCSPROPELLANT_END");
	tca1A.SaveState(scn, "RCSTCA_1A_BEGIN", "RCSTCA_END");
	tca2A.SaveState(scn, "RCSTCA_2A_BEGIN", "RCSTCA_END");
	tca3A.SaveState(scn, "RCSTCA_3A_BEGIN", "RCSTCA_END");
	tca4A.SaveState(scn, "RCSTCA_4A_BEGIN", "RCSTCA_END");
	tca1B.SaveState(scn, "RCSTCA_1B_BEGIN", "RCSTCA_END");
	tca2B.SaveState(scn, "RCSTCA_2B_BEGIN", "RCSTCA_END");
	tca3B.SaveState(scn, "RCSTCA_3B_BEGIN", "RCSTCA_END");
	tca4B.SaveState(scn, "RCSTCA_4B_BEGIN", "RCSTCA_END");
	ordeal.SaveState(scn);
	mechanicalAccelerometer.SaveState(scn);
	atca.SaveState(scn);
	MissionTimerDisplay.SaveState(scn, "MISSIONTIMER_START", MISSIONTIMER_END_STRING, false);
	EventTimerDisplay.SaveState(scn, "EVENTTIMER_START", EVENTTIMER_END_STRING, true);
	checkControl.save(scn);
}

bool LEM::clbkLoadGenericCockpit ()

{
	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
	oapiCameraSetCockpitDir(0,0);

	InVC = false;
	InPanel = false;
	SetView();

	return true;
}

//
// Transfer important data from the CSM to the LEM when the LEM is first
// created.
//

bool LEM::SetupPayload(PayloadSettings &ls)

{
	char CSMName[64];

	MissionTime = ls.MissionTime;

	strncpy(CSMName, ls.CSMName, 64);

	Crewed = ls.Crewed;
	AutoSlow = ls.AutoSlow;
	ApolloNo = ls.MissionNo;

	DescentFuelMassKg = ls.DescentFuelKg;
	AscentFuelMassKg = ls.AscentFuelKg;
	DescentEmptyMassKg = ls.DescentEmptyKg;
	AscentEmptyMassKg = ls.AscentEmptyKg;

	pMission->LoadMission(ApolloNo);

	agc.SetMissionInfo(pMission->GetLGCVersion(), CSMName);
	aea.SetMissionInfo(pMission->GetAEAVersion());

	// Initialize the checklist Controller in accordance with scenario settings.
	checkControl.init(ls.checklistFile, true);

	// Sounds are initialized during the first timestep
	// or here
	SetLmVesselDockStage();
	PostLoadSetup();

	return true;
}

void LEM::PadLoad(unsigned int address, unsigned int value)
{ 
	agc.PadLoad(address, value);
}

void LEM::AEAPadLoad(unsigned int address, unsigned int value)
{
	aea.PadLoad(address, value);
}

void LEM::SetRCSJet(int jet, bool fire) {
	if (th_rcs[jet] == NULL) return;  // Sanity check
	SetThrusterLevel(th_rcs[jet], fire);
}

double LEM::GetRCSThrusterLevel(int jet)
{
	if (th_rcs[jet] == NULL) return 0.0;  // Sanity check
	return GetThrusterLevel(th_rcs[jet]);
}

// Set level of RCS thruster, using primary coils
void LEM::SetRCSJetLevelPrimary(int jet, double level) {
	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/
	// The thruster is a Marquardt R-4D, which uses 46 watts @ 28 volts to fire.
	// This applies to the SM as well, someone should probably tell them about this.
	// RCS pressurized?

	// Is this thruster on?	
	switch(jet){
		// SYS A
		case 0: // QUAD 1
		case 1:
			if(RCS_A_QUAD1_TCA_CB.Voltage() > 24){ RCS_A_QUAD1_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 6: // QUAD 2
		case 7:
			if(RCS_A_QUAD2_TCA_CB.Voltage() > 24){ RCS_A_QUAD2_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 8: // QUAD 3
		case 9:
			if(RCS_A_QUAD3_TCA_CB.Voltage() > 24){ RCS_A_QUAD3_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 14: // QUAD 4
		case 15:
			if(RCS_A_QUAD4_TCA_CB.Voltage() > 24){ RCS_A_QUAD4_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;

		// SYS B
		case 2: // QUAD 1
		case 3:
			if(RCS_B_QUAD1_TCA_CB.Voltage() > 24){ RCS_B_QUAD1_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 4: // QUAD 2
		case 5:
			if(RCS_B_QUAD2_TCA_CB.Voltage() > 24){ RCS_B_QUAD2_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 10: // QUAD 3
		case 11:
			if(RCS_B_QUAD3_TCA_CB.Voltage() > 24){ RCS_B_QUAD3_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
		case 12: // QUAD 4
		case 13:
			if(RCS_B_QUAD4_TCA_CB.Voltage() > 24){ RCS_B_QUAD4_TCA_CB.DrawPower(46); }else{ level = 0; }
			break;
	}

	if (th_rcs[jet] == NULL) return;  // Sanity check

	SetThrusterLevel(th_rcs[jet], level);
}

void LEM::RCSSoundTimestep() {

	// In case of disabled Orbiter attitude thruster groups OrbiterSound plays no
	// engine sound, so this needs to be done manually

	int i;
	bool on = false;
	// LM RCS
	for (i = 0; i < 16; i++) {
		if (th_rcs[i]) {
			if (GetThrusterLevel(th_rcs[i])) on = true;
		}
	}
	// Play/stop sounds
	if (on) {
		if (RCSFireSound.isPlaying()) {
			RCSSustainSound.play(LOOP);
		}
		else if (!RCSSustainSound.isPlaying()) {
			RCSFireSound.play();
		}
	}
	else {
		RCSSustainSound.stop();
	}
}

double LEM::GetAscentStageMass()
{
	if (stage < 2)
	{
		return GetMass() - GetPropellantMass(ph_Dsc) - DescentEmptyMassKg;
	}

	return GetMass();
}

void LEM::SendVHFRangingSignal(Saturn *sat, bool isAcquiring)
{
	VHF.RangingSignal(sat, isAcquiring);
}

void LEM::StartSeparationPyros()
{
	LEMToSLAConnector.StartSeparationPyros();
}

void LEM::StopSeparationPyros()
{
	LEMToSLAConnector.StopSeparationPyros();
}

void LEM::CalculatePMIandCOG(VECTOR3 &PMI, VECTOR3 &COG)
{
	double m = GetMass();

	//Descent stage
	if (stage < 2)
	{
		//Y-coordinate of DPS propellant tanks
		double tanky = 4.067429;
		//Y-coordinate of "rest" of the full LM (empirically derived)
		double resty = 5.5;
		double fm = 0.0;
		if (ph_Dsc != NULL) { fm = GetPropellantMass(ph_Dsc); }
		double restmass = m - fm;
		double totaly = (tanky*fm + resty * restmass) / m;

		//5.4516 is the offset between the full LM mesh and the LM coordinate system
		COG = _V(0.0, totaly - 5.4516, 0.0);
		//COG = _V(0.0, 0.0, 0.0);
		PMI = _V(2.5428, 2.2871, 2.7566);
	}
	//Ascent stage
	else
	{
		//Use this when RCS minimum impulse behavior is better
		/*static double xaxis[3] = { 3.9839365e-8, -5.363325e-4, 2.625888102 };
		static double yaxis[3] = { -1.925907e-8, 2.1607777e-4, 1.255232416 };
		static double zaxis[3] = { -9.068924e-8, 9.4558155e-4, -0.7852828715 };

		COG = _V(0, -0.8755, 0);
		VECTOR3 p;

		p.x = xaxis[0] * m*m + xaxis[1] * m + xaxis[2];
		p.y = yaxis[0] * m*m + yaxis[1] * m + yaxis[2];
		p.z = zaxis[0] * m*m + zaxis[1] * m + zaxis[2];
		PMI = p;*/

		PMI = _V(2.8, 2.29, 2.37);
		COG = _V(0, 0, 0);
	}
}