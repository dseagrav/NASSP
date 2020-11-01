/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5

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
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "resource.h"

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "ioChannels.h"

#include "s1csystems.h"
#include "saturn.h"
#include "saturnv.h"
#include "s1c.h"
#include "LVDC.h"
#include "iu.h"
#include "tracer.h"

//
// Random functions from Yaagc.
//

extern "C" {
	void srandom(unsigned int x);
	long int random();
}

//
// Set the file name for the tracer code.
//

char trace_file[] = "ProjectApollo Saturn5.log";

//
// Various bits of real global data for the DLL. Most things should be vessel
// specific, but these aren't: most are set once at DLL creation and then never
// need to change.
//

const double N   = 1.0;
const double kN  = 1000.0;
const double KGF = N*G;
const double SEC = 1.0*G;
const double KG  = 1.0;
const double  CSM_THRUST_ATT   = 200.34*KGF;
const double  CSM_ISP          = 773*SEC;

static int refcount = 0;

const double BASE_SII_MASS = 39869;		// Stage + SII/SIVB interstage

GDIParams g_Param;

// 3 was 80.5
//
// SaturnV constructor, derived from basic Saturn class.
//

SaturnV::SaturnV (OBJHANDLE hObj, int fmodel) : Saturn (hObj, fmodel),
	SICSIISepPyros("SIC-SII-Separation-Pyros", Panelsdk),
	SIIInterstagePyros("SII-Interstage-Pyros", Panelsdk),
	SIISIVBSepPyros("SII-SIVB-Separation-Pyros", Panelsdk)

{
	TRACESETUP("SaturnV");
	
	hMaster = hObj;
	sic = NULL;
	sii = NULL;
	initSaturnV();
}

//
// Do all the initialisation.
//

void SaturnV::initSaturnV()

{
	// DS20060301 Save DLL instance handle for later abuse
	dllhandle = g_Param.hDLL;

	//
	// Do the basic initialisation from the
	// generic Saturn class first.
	//

	initSaturn();

	strcpy(StagesString, "S1C:SII:SIVB:CSM");
	SaturnType = SAT_SATURNV;
	HasProbe = true;

	//
	// Default ISP and thrust values.
	//

	ISP_FIRST_SL    = 2601.3; // Average From AP8 Post Flight Eval p114 - Was 2594.4
	ISP_FIRST_VAC   = 2979.4;
	ISP_SECOND_SL   = 300*G;//300*G;
	ISP_SECOND_VAC  = 418*G;//421*G;
	ISP_THIRD_VAC   = 424*G;//421*G;

	//
	// Note: thrust values are _per engine_, not per stage. For now, assume that the second
	// and third stage thrusts are the same.
	//

	THRUST_FIRST_VAC	= 8062309;
	THRUST_SECOND_VAC   = 1023000;
	THRUST_THIRD_VAC    = 1023000;

	//
	// Engines per stage.
	//

	SI_EngineNum = 5;
	SII_EngineNum = 5;
	SIII_EngineNum = 1;

	//
	// State variables.
	//

	TLICapableBooster = true;
	GoHover = false;
	Burned = false;

	//
	// Default masses.
	//

	Interstage_Mass = 3982;

	S4B_EmptyMass = 13439; // Stage + SLA
	S4B_FuelMass = 107428;

	SII_EmptyMass = BASE_SII_MASS;
	SII_FuelMass = 443500;
	SII_UllageNum = 8;

	SI_EmptyMass = 133602;			// Stage mass, approx
	SI_FuelMass = 2146040;

	SI_RetroNum = 8;
	SII_RetroNum = 4;

	SM_FuelMass = SPS_DEFAULT_PROPELLANT;
	SM_EmptyMass = 4100;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
												/// \todo The Apollo 15-17 SM was heavier, about 5500 kg
	CM_EmptyMass = 5430;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
	CM_FuelMass =  CM_RCS_FUEL_PER_TANK * 2.;	// The CM has 2 tanks

	CalculateStageMass();

	// load wave (Handle defined in begining of file)
	soundlib.LoadSound(SpeedS, "reentry.wav");
	soundlib.LoadSound(DockS, "docking.wav");

	soundlib.LoadSound(SRover, "LRover.WAV");
}

SaturnV::~SaturnV()

{
	TRACESETUP("~SaturnV");

	ReleaseSurfaces();

	if (iu)
	{
		delete iu;
		iu = 0;
	}
	if (sic)
	{
		delete sic;
		sic = 0;
	}
	if (sii)
	{
		delete sii;
		sii = 0;
	}
}

void SaturnV::CalculateStageMass ()

{
	if (SaturnType != SAT_INT20)
	{
		SI_Mass = SI_EmptyMass + SI_FuelMass + (SI_RetroNum * 125);
		SII_Mass = SII_EmptyMass + SII_FuelMass + (SII_UllageNum * 175);
		S4B_Mass = S4B_EmptyMass + S4B_FuelMass;
		SM_Mass = SM_EmptyMass + SM_FuelMass;
		CM_Mass = CM_EmptyMass;

		Stage3Mass = S4B_EmptyMass + S4PL_Mass + SM_Mass + CM_Mass;
		Stage2Mass = Stage3Mass + SII_EmptyMass + S4B_FuelMass + Abort_Mass + Interstage_Mass;
		Stage1Mass = Stage2Mass + SI_EmptyMass + SII_FuelMass;
	}
	else
	{
		SI_Mass = SI_EmptyMass + SI_FuelMass + (SI_RetroNum * 125);
		SII_Mass = 0;
		S4B_Mass = S4B_EmptyMass + S4B_FuelMass;
		SM_Mass = SM_EmptyMass + SM_FuelMass;
		CM_Mass = CM_EmptyMass;

		Stage3Mass = S4B_EmptyMass + S4PL_Mass + SM_Mass + CM_Mass;
		Stage2Mass = 0;
		Stage1Mass = Stage3Mass + SI_EmptyMass + S4B_FuelMass + Abort_Mass;
	}
}

void SaturnV::MoveEVA()

{
	TRACESETUP("MoveEVA");
	double cm ;
	double lat;
	double lon;
	double cap;
	double timeW=1;
	VESSELSTATUS eva;
	GetStatus(eva);
	oapiGetHeading(GetHandle(),&cap);
	timeW=oapiGetTimeAcceleration();
	lon=eva.vdata[0].x;
	lat=eva.vdata[0].y;

//	SRover->play(LOOP, 245);

	cm = 5.36e-8*timeW;

	if (KEY1){
		eva.vdata[0].z = eva.vdata[0].z - ((PI*0.1/180)*timeW/2);
		if(eva.vdata[0].z <=-2*PI){
			eva.vdata[0].z = eva.vdata[0].z + 2*PI;
		}
		KEY1=false;
	}
	else if (KEY3){
		eva.vdata[0].z = eva.vdata[0].z +(( PI*0.1/180)*timeW/2);
		if(eva.vdata[0].z >=2*PI){
			eva.vdata[0].z = eva.vdata[0].z - 2*PI;
		}
		KEY3=false;
	}
	else if (KEY2){
		lat = lat - cos(cap) * cm;
		lon = lon - sin(cap) * cm;
		KEY2=false;
	}
	else if (KEY4){
		lat = lat + sin(cap) * cm;
		lon = lon - cos(cap) * cm;
		KEY4=false;
	}
	else if (KEY5){
		KEY5=false;
	}
	else if (KEY6){
		lat = lat - sin(cap) * cm;
		lon = lon + cos(cap) * cm;
		KEY6=false;
	}
	else if (KEY7){
		KEY7=false;
	}
	else if (KEY8||GoHover){// we go ahead whatever our headign
		lat = lat + cos(cap) * cm;
		lon = lon + sin(cap) * cm;
		KEY8=false;
	}
	else if (KEY9){
		KEY9=false;
	}else{
		SRover.stop();
	}

	eva.vdata[0].x=lon;
	eva.vdata[0].y=lat;
	DefSetState(&eva);
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	TRACESETUP("ovcInit");

	VESSEL *v;

	if (!refcount++) {
		TRACE("refcount == 0");
		LoadSat5Meshes();
		SaturnInitMeshes();
	}

	TRACE("Meshes loaded");

	BaseInit();

	TRACE("Got colors");

	v = new SaturnV (hvessel, flightmodel);
	return v;
}

DLLCLBK void ovcExit (VESSEL *vessel)

{
	TRACESETUP("ovcExit");

	--refcount;

	if (!refcount) {
		TRACE("refcount == 0");

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) delete (SaturnV *)vessel;
}

void SaturnV::clbkSetClassCaps (FILEHANDLE cfg)

{
	TRACESETUP("SaturnV::clbkSetClassCaps");
	//
	// For some reason things get screwy if we do all the initialisation in the constructor. I think
	// that Orbiter may be keeping a pool of objects and reusing them, so we have to reinitialise here
	// to ensure that it works correctly.
	//

	initSaturnV();

	//
	// Scan the config file for specific information about this class.
	//

	char *line, buffer[1000];

	while (oapiReadScenario_nextline (cfg, line)) {
		ProcessConfigFileLine(cfg, line);
	}

	//
	// Scan the launchpad config file.
	//

	sprintf(buffer, "%s.launchpad.cfg", GetClassName());
	FILEHANDLE hFile = oapiOpenFile(buffer, FILE_IN, CONFIG);

	while (oapiReadScenario_nextline(hFile, line)) {
		ProcessConfigFileLine(hFile, line);
	}
	oapiCloseFile(hFile, FILE_IN);
}

void SaturnV::DoFirstTimestep(double simt)

{
	//
	// Do housekeeping on the first timestep after
	// initialisation. This allows us to pick up any
	// data that isn't saved and must be recreated after
	// loading... but which can only be done when the
	// whole system has been initialised.
	//

	//
	// Get the handles for any odds and ends that are out there.
	//

	char VName[256];
	char ApolloName[64];

	GetApolloName(ApolloName);

	strcpy (VName, ApolloName); strcat (VName, "-TWR");
	hesc1= oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-STG1");
	hstg1= oapiGetVesselByName(VName);

	LookForSIVb();

	strcpy (VName, ApolloName); strcat (VName, "-S4B1");
	hs4b1 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-S4B2");
	hs4b2 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-S4B3");
	hs4b3 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-S4B4");
	hs4b4 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-SM");
	hSMJet = oapiGetVesselByName(VName);

	LookForLEM();

	strcpy (VName, ApolloName); strcat (VName, "-DCKPRB");
	hPROBE = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-EVA");
	hEVA = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-INTSTG");
	hintstg = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-STG2");
	hstg2 = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-APEX");
	hApex = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-DROGUECHUTE");
	hDrogueChute = oapiGetVesselByName(VName);
	strcpy (VName, ApolloName); strcat (VName, "-MAINCHUTE");
	hMainChute = oapiGetVesselByName(VName);	
	strcpy (VName, ApolloName); strcat (VName, "-OPTICSCOVER");
	hOpticsCover = oapiGetVesselByName(VName);	
	
	habort = oapiGetVesselByName("Saturn_Abort");
}

// Orbiter calls here via callback prior to every timestep.
// This function must call GenericTimestep() to operate the CSM.

void SaturnV::Timestep(double simt, double simdt, double mjd)

{
	//
	// On the first timestep we just do basic setup
	// stuff and return. We seem to get called in at
	// least some cases before Orbiter is properly set
	// up, so the last thing we want to do is point the
	// engines in a wacky direction and then not be
	// called again for several seconds.
	//

	if (FirstTimestep) {
		DoFirstTimestep(simt);
		LastTimestep = simt;
		FirstTimestep = false;
		return;
	}

	GenericTimestep(simt, simdt, mjd);

	if (stage <= LAUNCH_STAGE_ONE)
	{
		sic->Timestep(MissionTime, simdt);
	}
	else if (stage == LAUNCH_STAGE_TWO || stage == LAUNCH_STAGE_TWO_ISTG_JET)
	{
		sii->Timestep(simdt);
	}

	if (stage < CSM_LEM_STAGE) {
	} else {
		GenericTimestepStage(simt, simdt);
	}

	//
	// S-IC/S-II separation
	//

	if (!LaunchFail.SIIAutoSepFail && SICSIISepPyros.Blown() && stage == LAUNCH_STAGE_ONE)
	{
		SeparateStage(LAUNCH_STAGE_TWO);
		SetStage(LAUNCH_STAGE_TWO);
		ActivateStagingVent();
		NextMissionEventTime = MissionTime + 1.7;
	}

	//Deactivate S-IC/S-II staging vent
	if (stagingvent[0] && MissionTime >= NextMissionEventTime)
	{
		DeactivateStagingVent();
	}

	//
	// S-II Interstage separation
	//

	if (SIIInterstagePyros.Blown() && stage == LAUNCH_STAGE_TWO)
	{
		SeparateStage(LAUNCH_STAGE_TWO_ISTG_JET);
		SetStage(LAUNCH_STAGE_TWO_ISTG_JET);
	}

	//
	// S-II/S-IVB separation
	//

	if (SIISIVBSepPyros.Blown() && stage < LAUNCH_STAGE_SIVB)
	{
		SPUShiftS.done(); // Make sure it's done
		SeparateStage(LAUNCH_STAGE_SIVB);
		SetStage(LAUNCH_STAGE_SIVB);
		AddRCS_S4B();
	}

	//
	// CSM/LV separation
	//

	if (CSMLVPyros.Blown() && stage < CSM_LEM_STAGE) {
		SeparateStage(CSM_LEM_STAGE);
		SetStage(CSM_LEM_STAGE);
	}

	//
	// CM/SM separation pyros
	//

	if (CMSMPyros.Blown() && stage < CM_STAGE)
	{
		SeparateStage(CM_STAGE);
		SetStage(CM_STAGE);
	}

	LastTimestep = simt;
}

void SaturnV::clbkPostStep (double simt, double simdt, double mjd) {

	Saturn::clbkPostStep(simt, simdt, mjd);
}

void SaturnV::SetVehicleStats(){
	//
	// Adjust performance as appropriate based on the vehicle number. Early Saturn V rockets
	// had less thrust than later models, and later models removed some retro and ullage
	// rockets to save weight.
	//

	SI_RetroNum = 8;
	SII_RetroNum = 8;

	if (VehicleNo > 500 && VehicleNo < 503) {
		if (!S1_ThrustLoaded)
			THRUST_FIRST_VAC = 7835000; // 7653000; /// \todo Temporary fix, otherwise the autopilot is not working properly, we should replace this when we figured out all parameters (masses, fuel masses etc.) of these vehicles
		if (!S2_ThrustLoaded)
			THRUST_SECOND_VAC = 1001000;
		if (!S3_ThrustLoaded)
			THRUST_THIRD_VAC = 1001000;

		SII_UllageNum = 8;
	}
	else if (VehicleNo >= 503 && VehicleNo < 510) {
		if (VehicleNo == 503){
			// Apollo 8 -- For LVDC++ experiments.
			if (!S1_ThrustLoaded){
				// F1 thrust computed as follows:
				// 6782000 @ Sea Level, From AP8 LV Evaluation, averaged predicted value
				// Was making 8000000 @ S1C OECO				
				THRUST_FIRST_VAC = 8000100; 
				THRUST_SECOND_VAC = 1017000;//1001000;
				THRUST_THIRD_VAC = 1024009;//1001000;//901557;
			}
		}else{
			if (!S1_ThrustLoaded)
				THRUST_FIRST_VAC = 7835000;
		}

		SII_UllageNum = 4;
	}
	else {
		SII_UllageNum = 0;
		SI_RetroNum = 4;
	}

	//
	// Apollo 13 and later had lightweight SII stages, about 1500kg lighter
	// than earlier missions.
	//

	if (!SII_MassLoaded) {
		if (VehicleNo >= 508) {
			SII_EmptyMass = BASE_SII_MASS - 1500;
		}
		else {
			SII_EmptyMass = BASE_SII_MASS;
		}
	}

	CalculateStageMass ();

	//
	// Calculate Apollo mission number.
	//

	if (!ApolloNo && (VehicleNo >= 503 && VehicleNo <= 512)) {
		ApolloNo = VehicleNo - 495;
	}
}

void SaturnV::SaveVehicleStats(FILEHANDLE scn)

{
	//
	// Fuel mass on launch.
	//

	oapiWriteScenario_float (scn, "SIFUELMASS", SI_FuelMass);
	oapiWriteScenario_float (scn, "SIIFUELMASS", SII_FuelMass);
	oapiWriteScenario_float (scn, "S4FUELMASS", S4B_FuelMass);

	//
	// Stage masses.
	//

	oapiWriteScenario_float (scn, "SIEMPTYMASS", SI_EmptyMass);
	oapiWriteScenario_float (scn, "SIIEMPTYMASS", SII_EmptyMass);
	oapiWriteScenario_float (scn, "S4EMPTYMASS", S4B_EmptyMass);
	oapiWriteScenario_float(scn, "INTERSTAGE", Interstage_Mass);
}

void SaturnV::CreateStageSpecificSystems()
{
	if (stage <= LAUNCH_STAGE_ONE)
	{
		sic = new SICSystems(this, th_1st, ph_1st, SICSIISepPyros, LaunchS, SShutS, contrailLevel);
	}
	if (stage <= LAUNCH_STAGE_TWO_ISTG_JET)
	{
		sii = new SIISystems(this, th_2nd, ph_2nd, thg_ull, SIIInterstagePyros, SIISIVBSepPyros, SPUShiftS, SepS);
	}
	if (stage < CSM_LEM_STAGE)
	{
		iu = new IUSV;
		sivb = new SIVB500Systems(this, th_3rd[0], ph_3rd, th_aps_rot, th_aps_ull, th_3rd_lox, thg_ver);
	}
}

void SaturnV::CheckSaturnSystemsState()
{
	Saturn::CheckSaturnSystemsState();

	if (stage > LAUNCH_STAGE_ONE)
	{
		if (sic)
		{
			delete sic;
			sic = 0;
		}
	}
	if (stage > LAUNCH_STAGE_TWO_ISTG_JET)
	{
		if (sii)
		{
			delete sii;
			sii = 0;
		}
	}
}

void SaturnV::LoadIU(FILEHANDLE scn)
{
	iu->LoadState(scn);
}

void SaturnV::LoadLVDC(FILEHANDLE scn)
{
	iu->LoadLVDC(scn);
}

void SaturnV::LoadSIVB(FILEHANDLE scn)
{
	sivb->LoadState(scn);
}

void SaturnV::SaveSI(FILEHANDLE scn)
{
	sic->SaveState(scn);
}

void SaturnV::LoadSI(FILEHANDLE scn)
{
	sic->LoadState(scn);
}

void SaturnV::clbkLoadStateEx (FILEHANDLE scn, void *status)

{
	TRACESETUP("SaturnV::clbkLoadStateEx");

	GetScenarioState(scn, status);

	ClearMeshes();
	SetupMeshes();

	//
	// This code all needs to be fixed up.
	//

	switch (stage) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		if (buildstatus < 6){
			ChangeSatVBuildState(buildstatus);
		}
		else{
			SetFirstStage();
			SetFirstStageEngines();
		}
		break;

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SetSecondStage();
		SetSecondStageEngines(-STG1O);
		break;

	case LAUNCH_STAGE_SIVB:
		SetThirdStage();
		SetThirdStageEngines(-STG2O);
		if (StageState >= 4) {
			AddRCS_S4B();
		}
		break;

	case STAGE_ORBIT_SIVB:
		SetThirdStage();
		SetThirdStageEngines(-STG2O);
		AddRCS_S4B();
		break;

	default:
		SetGenericStageState();
		break;
	}

	//
	// Setup of the generic systems
	//

	GenericLoadStateSetup();

	if (stage < LAUNCH_STAGE_SIVB) {
		if (Crewed) {
			soundlib.LoadMissionSound(SPUShiftS, PUSHIFT_SOUND, PUSHIFT_SOUND);
		}
	}
}

void SaturnV::ConfigureStageMeshes(int stage_state)

{
	ClearMeshes();

	//
	// This code all needs to be fixed up.
	//

	switch (stage_state) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		if (buildstatus < 6){
			ChangeSatVBuildState(buildstatus);
		}
		else {
			SetFirstStage();
		}
		break;

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SetSecondStage();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetThirdStage();
		break;

	case CSM_LEM_STAGE:
		SetCSMStage();
		break;

	case CM_STAGE:
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_TWO:
		SetReentryStage();
		break;

	case CM_ENTRY_STAGE_THREE:
		SetChuteStage1();
		break;

	case CM_ENTRY_STAGE_FOUR:
		SetChuteStage2();
		break;

	case CM_ENTRY_STAGE_FIVE:
		SetChuteStage3();
		break;

	case CM_ENTRY_STAGE_SIX:
		SetChuteStage4();
		break;

	case CM_ENTRY_STAGE_SEVEN:
		SetSplashStage();
		break;

	case CM_RECOVERY_STAGE:
		SetRecovery();
		break;

	case CM_ENTRY_STAGE:
		SetReentryStage();
		break;
	}
}

void SaturnV::ConfigureStageEngines(int stage_state)
{

	//
	// This code all needs to be fixed up.
	//

	switch (stage_state) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStageEngines();
		break;

	case LAUNCH_STAGE_TWO:
	case LAUNCH_STAGE_TWO_ISTG_JET:
		SetSecondStageEngines (-STG1O);
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetThirdStageEngines(-STG2O);
		break;
	}
}

int SaturnV::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	// Assembly tests
	/* 
	if (!KEYMOD_SHIFT(kstate) && !KEYMOD_CONTROL(kstate) && !KEYMOD_ALT(kstate)) {

		if (stage == ROLLOUT_STAGE) {
			if (key == OAPI_KEY_B && down == true) {
				LaunchVehicleBuild();
				return 1;
			}
			if (key == OAPI_KEY_U && down == true) {
				LaunchVehicleUnbuild();
				return 1;
			}
		}
	}
	*/
	return Saturn::clbkConsumeBufferedKey(key, down, kstate);
}

void SaturnV::LaunchVehicleRolloutEnd() {
	// called by crawler after arrival on launch pad

	SetFirstStage();
	SetFirstStageEngines();

	SetStage(ONPAD_STAGE);
}

void SaturnV::LaunchVehicleBuild() {
	// called by crawler
	
	if (stage == ROLLOUT_STAGE && buildstatus < 5) {
		buildstatus++;
		ChangeSatVBuildState(buildstatus);
	}
}

void SaturnV::LaunchVehicleUnbuild() {
	// called by crawler

	if (stage == ROLLOUT_STAGE && buildstatus > 0) {
		buildstatus--;
		ChangeSatVBuildState(buildstatus);
	}
}

void SaturnV::SISwitchSelector(int channel)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	sic->SwitchSelector(channel);
}

void SaturnV::SIISwitchSelector(int channel)
{
	if (stage > LAUNCH_STAGE_TWO_ISTG_JET) return;

	sii->SwitchSelector(channel);
}

void SaturnV::GetSIThrustOK(bool *ok)
{
	for (int i = 0;i < 15;i++)
	{
		ok[i] = false;
	}

	if (stage > LAUNCH_STAGE_ONE) return;

	sic->GetThrustOK(ok);
}

bool SaturnV::GetSIPropellantDepletionEngineCutoff()
{
	if (stage > LAUNCH_STAGE_ONE) return false;

	return sic->GetPropellantDepletionEngineCutoff();
}

bool SaturnV::GetSIInboardEngineOut()
{
	if (stage > LAUNCH_STAGE_ONE) return false;

	return sic->GetInboardEngineOut();
}

bool SaturnV::GetSIOutboardEngineOut()
{
	if (stage > LAUNCH_STAGE_ONE) return false;

	return sic->GetOutboardEngineOut();
}

void SaturnV::SetSIThrusterDir(int n, double yaw, double pitch)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	sic->SetThrusterDir(n, yaw, pitch);
}

bool SaturnV::GetSIIPropellantDepletionEngineCutoff()
{
	if (stage != LAUNCH_STAGE_TWO && stage != LAUNCH_STAGE_TWO_ISTG_JET) return false;

	return sii->GetPropellantDepletionEngineCutoff();
}

bool SaturnV::GetSIIEngineOut()
{
	if (stage != LAUNCH_STAGE_TWO && stage != LAUNCH_STAGE_TWO_ISTG_JET) return false;

	return sii->GetEngineOut();
}

void SaturnV::SIEDSCutoff(bool cut)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	sic->EDSEnginesCutoff(cut);
}

void SaturnV::GetSIIThrustOK(bool *ok)
{
	for (int i = 0;i < 5;i++)
	{
		ok[i] = false;
	}

	if (stage != LAUNCH_STAGE_TWO && stage != LAUNCH_STAGE_TWO_ISTG_JET) return;

	sii->GetThrustOK(ok);
}

void SaturnV::SetSIIThrusterDir(int n, double yaw, double pitch)
{
	if (stage != LAUNCH_STAGE_TWO && stage != LAUNCH_STAGE_TWO_ISTG_JET) return;

	sii->SetThrusterDir(n, yaw, pitch);
}

void SaturnV::SIIEDSCutoff(bool cut)
{
	if (stage != LAUNCH_STAGE_TWO && stage != LAUNCH_STAGE_TWO_ISTG_JET) return;

	sii->EDSEnginesCutoff(cut);
}

double SaturnV::GetSIIFuelTankPressurePSI()
{
	if (stage <= LAUNCH_STAGE_TWO_ISTG_JET)
		return sii->GetLH2TankUllagePressurePSI();

	return 0.0;
}

void SaturnV::SaveSII(FILEHANDLE scn)
{
	sii->SaveState(scn);
}

void SaturnV::LoadSII(FILEHANDLE scn)
{
	sii->LoadState(scn);
}

void SaturnV::SetEngineFailure(int failstage, int faileng, double failtime, bool fail)
{
	if (failstage == 1)
	{
		sic->SetEngineFailureParameters(faileng, failtime, fail);
	}
	else if (failstage == 2)
	{
		sii->SetEngineFailureParameters(faileng, failtime, fail);
	}
}

void SaturnV::GetEngineFailure(int failstage, int faileng, bool &fail, double &failtime)
{
	if (failstage == 1 && sic)
	{
		sic->GetEngineFailureParameters(faileng, fail, failtime);
	}
	else if (failstage == 2 && sii)
	{
		sii->GetEngineFailureParameters(faileng, fail, failtime);
	}
}

double SaturnV::GetSIThrustLevel()
{
	if (stage > LAUNCH_STAGE_ONE) return 0.0;

	double lvl = 0.0;
	for (int i = 0;i < 5;i++)
	{
		lvl += GetThrusterLevel(th_1st[i]);
	}

	return lvl / 5.0;
}