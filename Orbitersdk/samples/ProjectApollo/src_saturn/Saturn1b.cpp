/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn IB

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
#include "resource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "ioChannels.h"

#include "s1bsystems.h"
#include "saturn.h"
#include "saturn1b.h"

#include "s1b.h"
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

char trace_file[] = "ProjectApollo Saturn1b.log";

#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

// ==============================================================
// Global parameters
// ==============================================================

GDIParams g_Param;

static int refcount = 0;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hastp;

Saturn1b::Saturn1b (OBJHANDLE hObj, int fmodel) : Saturn (hObj, fmodel),
	SIBSIVBSepPyros("SIB-SIVB-Separation-Pyros", Panelsdk)
{
	hMaster = hObj;
	initSaturn1b();
}

Saturn1b::~Saturn1b()

{
	ReleaseSurfaces();

	if (iu)
	{
		delete iu;
		iu = 0;
	}
	if (sib)
	{
		delete sib;
		sib = 0;
	}
}


void Saturn1b::initSaturn1b()

{
	// Save DLL instance handle for later abuse
	dllhandle = g_Param.hDLL;
	
	//
	// Do the basic initialisation from the
	// generic Saturn class first.
	//

	initSaturn();

	SaturnType = SAT_SATURN1B;
	RelPos = _V(0.0,0.0,0.0);
	hSoyuz = 0;
	hAstpDM = 0;
	hNosecapVessel = 0;
	Burned = false;

	if (strcmp(GetName(), "AS-211")==0)
	{
		ASTPMission = true;
	}

	//
	// Apollo 7 ISP and thrust values.
	// Note: thrust values are _per engine_, not per stage.
	/// \todo other Saturn 1b missions
	//

	ISP_FIRST_SL     = 262*G;	// See Apollo 7 Saturn IB Report, NTRS ID 19900067467
	ISP_FIRST_VAC    = 294*G; 
	THRUST_FIRST_VAC = 1008000;  // between 875.000-1.025.000 N, calibrated to meet staging target

	ISP_SECOND_SL   = 424*G;
	ISP_SECOND_VAC  = 424*G;

	THRUST_SECOND_VAC  = 1009902;

	SM_EmptyMass = 3900;						// Calculated from Apollo 7 Mission Report and "Apollo by the numbers"
	SM_FuelMass = 4430;							// Apollo 7 (according to Mission Report), 

	CM_EmptyMass = 5430;						// Calculated from Apollo 11 Mission Report and "Apollo by the numbers"
	CM_FuelMass = CM_RCS_FUEL_PER_TANK * 2.;	// The CM has 2 tanks

	SII_EmptyMass = 12495;
	SII_FuelMass = 105795;

	SI_EmptyMass = 41874;
	SI_FuelMass = 411953;

	CalculateStageMass();

	//
	// Save the S4B information in the place where the Saturn class expects to find it. Maybe we should switch to
	// using the S4B variables rather than the 'stage 2' variables?
	//
	S4B_EmptyMass = SII_EmptyMass;
	S4B_FuelMass = SII_FuelMass;
	S4B_Mass = SII_Mass;

	//
	// Engines per stage.
	//

	SI_EngineNum = 8;
	SII_EngineNum = 1;
	SIII_EngineNum = 1;

	//
	// Default to not separating SLA panels.
	//
	SLAWillSeparate = false;

	// Moved to instantiation time
	// lvdc.init(this);
}

// ==============================================================
// API interface
// ==============================================================

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		Saturn1bLoadMeshes();
		SaturnInitMeshes();
	}

	BaseInit();
	return new Saturn1b (hvessel, flightmodel);
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	--refcount;
	if (vessel) delete (Saturn1b *) vessel;
}

void Saturn1b::DoFirstTimestep(double simt)

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

	habort = oapiGetVesselByName("Saturn_Abort");

	GetApolloName(VName); strcat (VName, "-TWR");
	hesc1 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-STG1");
	hstg1 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4BSTG");
	hs4bM = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B1");
	hs4b1 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B2");
	hs4b2 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B3");
	hs4b3 = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-S4B4");
	hs4b4=oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-SM");
	hSMJet = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-ASTPDM");
	hAstpDM = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-DCKPRB");
	hPROBE = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-EVA");
	hEVA = oapiGetVesselByName(VName);
	hSoyuz = oapiGetVesselByName("SOYUZ19");
	GetApolloName(VName); strcat (VName, "-INTSTG");
	hintstg = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-APEX");
	hApex = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-DROGUECHUTE");
	hDrogueChute = oapiGetVesselByName(VName);
	GetApolloName(VName); strcat (VName, "-MAINCHUTE");
	hMainChute = oapiGetVesselByName(VName);	
	GetApolloName(VName); strcat (VName, "-OPTICSCOVER");
	hOpticsCover = oapiGetVesselByName(VName);	
	GetApolloName(VName); strcat(VName, "-NOSECAP");
	hNosecapVessel = oapiGetVesselByName(VName);
}

void Saturn1b::Timestep (double simt, double simdt, double mjd)

{
	//
	// On the first timestep we just do basic setup
	// stuff and return. We seem to get called in at
	// least some cases before Orbiter is properly set
	// up, so the last thing we want to do is point the
	// engines in a wacky direction and then not be
	// called again for several seconds.
	//
	if (FirstTimestep){
		DoFirstTimestep(simt);
		FirstTimestep = false;
		return;
	}

	GenericTimestep(simt, simdt, mjd);

	if (stage <= LAUNCH_STAGE_ONE)
	{
		sib->Timestep(MissionTime, simdt);
	}

	if (stage < CSM_LEM_STAGE) {
	} else {
		GenericTimestepStage(simt, simdt);
	}

	//S-IB/S-IVB separation

	if (SIBSIVBSepPyros.Blown() && stage <= LAUNCH_STAGE_ONE)
	{
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

void Saturn1b::clbkPostStep (double simt, double simdt, double mjd) {

	Saturn::clbkPostStep(simt, simdt, mjd);
}

void Saturn1b::SISwitchSelector(int channel)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	sib->SwitchSelector(channel);
}

void Saturn1b::GetSIThrustOK(bool *ok)
{
	for (int i = 0;i < 24;i++)
	{
		ok[i] = false;
	}

	if (stage > LAUNCH_STAGE_ONE) return;

	sib->GetThrustOK(ok);
}

void Saturn1b::SIEDSCutoff(bool cut)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	sib->EDSEnginesCutoff(cut);
}

bool Saturn1b::GetSIPropellantDepletionEngineCutoff()
{
	if (stage > LAUNCH_STAGE_ONE) return false;

	return sib->GetOutboardEnginesCutoff();
}

bool Saturn1b::GetSIInboardEngineOut()
{
	if (stage > LAUNCH_STAGE_ONE) return false;

	return sib->GetInboardEngineOut();
}

bool Saturn1b::GetSIOutboardEngineOut()
{
	if (stage > LAUNCH_STAGE_ONE) return false;

	return sib->GetOutboardEngineOut();
}

bool Saturn1b::GetSIBLowLevelSensorsDry()
{
	if (stage > LAUNCH_STAGE_ONE) return false;

	return sib->GetLowLevelSensorsDry();
}

void Saturn1b::SetSIThrusterDir(int n, double yaw, double pitch)
{
	if (stage > LAUNCH_STAGE_ONE) return;

	sib->SetThrusterDir(n, yaw, pitch);
}

double Saturn1b::GetSIThrustLevel()
{
	if (stage > LAUNCH_STAGE_ONE) return 0.0;

	double lvl = 0.0;
	for (int i = 0;i < 8;i++)
	{
		lvl += GetThrusterLevel(th_1st[i]);
	}

	return lvl / 8.0;
}

//
// Save any state specific to the Saturn 1b.
//

void Saturn1b::SaveVehicleStats(FILEHANDLE scn){
	//
	// Fuel mass on launch. This could be made generic in saturn.cpp
	//

	oapiWriteScenario_float (scn, "SIFUELMASS", SI_FuelMass);
	oapiWriteScenario_float (scn, "SIIFUELMASS", SII_FuelMass);

	//
	// Stage masses.
	//

	oapiWriteScenario_float (scn, "SIEMPTYMASS", SI_EmptyMass);
	oapiWriteScenario_float (scn, "SIIEMPTYMASS", SII_EmptyMass);
}

void Saturn1b::LoadIU(FILEHANDLE scn)
{
	iu->LoadState(scn);
}

void Saturn1b::LoadLVDC(FILEHANDLE scn)
{
	iu->LoadLVDC(scn);
}

void Saturn1b::LoadSIVB(FILEHANDLE scn) {
	sivb->LoadState(scn);
}

void Saturn1b::SaveSI(FILEHANDLE scn)
{
	sib->SaveState(scn);
}

void Saturn1b::LoadSI(FILEHANDLE scn)
{
	sib->LoadState(scn);
}

void Saturn1b::clbkLoadStateEx (FILEHANDLE scn, void *vs){
	GetScenarioState(scn, vs);

	SetupMeshes();

	switch (stage) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStage();
		SetFirstStageEngines();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStage();
		SetSecondStageEngines();
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

	if (stage < STAGE_ORBIT_SIVB) {
		if (Crewed) {
			soundlib.LoadMissionSound(SPUShiftS, PUSHIFT_SOUND, PUSHIFT_SOUND);
		}
	}
}

void Saturn1b::CreateStageSpecificSystems()
{
	if (stage <= LAUNCH_STAGE_ONE)
	{
		sib = new SIBSystems(this, th_1st, ph_1st, SIBSIVBSepPyros, LaunchS, SShutS);
	}
	if (stage < CSM_LEM_STAGE)
	{
		iu = new IU1B;
		sivb = new SIVB200Systems(this, th_3rd[0], ph_3rd, th_aps_rot, th_aps_ull, th_3rd_lox, thg_ver);
	}
}

void Saturn1b::CheckSaturnSystemsState()
{
	Saturn::CheckSaturnSystemsState();

	if (stage > LAUNCH_STAGE_ONE)
	{
		if (sib)
		{
			delete sib;
			sib = 0;
		}
	}
}

void Saturn1b::ConfigureStageMeshes(int stage_state)
{
	//
	// This code all needs to be fixed up.
	//

	ClearMeshes();

	switch (stage_state) {

	case ROLLOUT_STAGE:
	case ONPAD_STAGE:
	case LAUNCH_STAGE_ONE:
	case PRELAUNCH_STAGE:
		SetFirstStage();
		break;

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStage();
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

void Saturn1b::ConfigureStageEngines(int stage_state)
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

	case LAUNCH_STAGE_SIVB:
	case STAGE_ORBIT_SIVB:
		SetSecondStageEngines();
		break;
	}
}

void Saturn1b::clbkSetClassCaps (FILEHANDLE cfg)

{
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

void Saturn1b::SetVehicleStats()

{
	switch (VehicleNo)
	{
	case 206:
	case 207:
	case 208:
		SkylabSM = true;
		SkylabCM = true;

	//
	// Fall through. I think that Apollo 7 would have the
	// S1b panel with eight lights.
	//
	case 205:
		S1bPanel = true;
		break;
	}
}

void Saturn1b::CalculateStageMass()

{
	SI_Mass = SI_EmptyMass + SI_FuelMass;
	SII_Mass = SII_EmptyMass + SII_FuelMass;
	SM_Mass = SM_EmptyMass + SM_FuelMass;
	CM_Mass = CM_EmptyMass;

	//
	// This needs fixing.
	//

	if (SaturnHasCSM()) {
		Stage3Mass = SM_Mass + CM_Mass;
	}
	else {
		Stage3Mass = 0.0;
	}

	Stage2Mass = Stage3Mass + SII_EmptyMass + S4PL_Mass;
	Stage1Mass = Stage2Mass + SI_EmptyMass + SII_FuelMass + (LESAttached ? Abort_Mass : 0.0);
}

int Saturn1b::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (FirstTimestep) return 0;

	// Nothing for now

	return Saturn::clbkConsumeBufferedKey(key, down, kstate);
}

void Saturn1b::SetEngineFailure(int failstage, int faileng, double failtime, bool fail)
{
	if (failstage == 1)
	{
		sib->SetEngineFailureParameters(faileng, failtime, fail);
	}
}

void Saturn1b::GetEngineFailure(int failstage, int faileng, bool &fail, double &failtime)
{
	if (failstage == 1 && sib)
	{
		sib->GetEngineFailureParameters(faileng, fail, failtime);
	}
}