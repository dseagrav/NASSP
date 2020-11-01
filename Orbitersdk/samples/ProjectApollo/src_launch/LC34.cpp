/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LC34 vessel

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

#define ORBITER_MODULE

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"

#include "IUUmbilical.h"
#include "IU_ESE.h"
#include "SCMUmbilical.h"
#include "SIB_ESE.h"
#include "LC34.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "saturn1b.h"
#include "s1b.h"
#include "sivb.h"
#include "papi.h"
#include "RCA110A.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo LC34.log";


// States
#define STATE_PRELAUNCH			0
#define STATE_CMARM1			1
#define STATE_CMARM2			2
#define STATE_TERMINAL_COUNT	3
#define STATE_IGNITION_SEQUENCE 4
#define STATE_LIFTOFF			5
#define STATE_POSTLIFTOFF		6


PARTICLESTREAMSPEC liftoffStreamSpec = {
	0, 
	12.0,	// size
	50,		// rate
	120.0,	// velocity
	0.5,	// velocity distribution
	6.0,	// lifetime
	4,		// growthrate
	0.4,	// atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.7, 0.7
};


DLLCLBK void InitModule(HINSTANCE hModule) {

	g_hDLL = hModule;
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new LC34(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (LC34*)vessel;
}


LC34::LC34(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	firstTimestepDone = false;
	LVName[0] = '\0';
	SIBName[0] = '\0';
	SIVBName[0] = '\0';
	touchdownPointHeight = -0.01; // pad height
	hLV = 0;
	sat = 0;
	s1b = NULL;
	sivb = NULL;
	state = STATE_PRELAUNCH;
	LaunchMJD = 99999.9;
	MissionTime = 0.0;
	Hold = false;
	bCommit = false;

	mssProc = 0;
	cmarmProc = 0;
	swingarmState.Set(AnimState::CLOSED, 0.0);

	int i;
	for (i = 0; i < 2; i++) {
		liftoffStream[i] = NULL;
	}
	liftoffStreamLevel = 0;

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	//meshoffsetMSS = _V(0,0,0);

	IuUmb = new IUUmbilical(this);
	IuESE = new IU_ESE(IuUmb, this);
	SCMUmb = new SCMUmbilical(this);
	SIBESE = new SIB_ESE(SCMUmb, this);
	rca110a = new RCA110AM(this);
}

LC34::~LC34() {
	delete IuUmb;
	delete IuESE;
	delete SCMUmb;
	delete SIBESE;
	delete rca110a;
}

void LC34::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(100000);
	SetSize(120);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0,0,0);
    meshindexLUT = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\CanaveralLC34LUT"), &meshoffset);
	SetMeshVisibilityMode(meshindexLUT, MESHVIS_ALWAYS);

	meshoffset = _V(0.74, -0.18, 2.61);
    meshindexMSS = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\CanaveralLC34MSS"), &meshoffset);
	SetMeshVisibilityMode(meshindexMSS, MESHVIS_ALWAYS);

	DefineAnimations();

	liftoffStream[0] = AddParticleStream(&liftoffStreamSpec, _V(0, 7, 0), _V(-1, 0.4,  1), &liftoffStreamLevel);
	liftoffStream[1] = AddParticleStream(&liftoffStreamSpec, _V(0, 7, 0), _V( 1, 0.4, -1), &liftoffStreamLevel);

	SetTouchdownPointHeight(touchdownPointHeight);
}

void LC34::clbkPostCreation()
{
	char buffer[256];

	if (swingarmState.action == AnimState::CLOSED)
	{
		double vcount = oapiGetVesselCount();
		for (int i = 0; i < vcount; i++) {
			OBJHANDLE h = oapiGetVesselByIndex(i);
			oapiGetObjectName(h, buffer, 256);
			//Connect either the Saturn1b class or S-IB and S-IVB
			if (!strcmp(LVName, buffer)) {
				hLV = h;
				sat = (Saturn1b *)oapiGetVesselInterface(hLV);
				if (sat->GetStage() < LAUNCH_STAGE_ONE)
				{
					IuUmb->Connect(sat->GetIU());
					SCMUmb->Connect(sat->GetSIB());
				}
			}
			else if (!strcmp(SIBName, buffer)) {
				s1b = (S1B *)oapiGetVesselInterface(h);
				SCMUmb->Connect(s1b->GetSIB());
			}
			else if (!strcmp(SIVBName, buffer)) {
				sivb = (SIVB *)oapiGetVesselInterface(h);
				IuUmb->Connect(sivb->GetIU());
			}
		}
	}

	SetAnimation(mssAnim, mssProc);
	SetAnimation(cmarmAnim, cmarmProc);
	SetAnimation(swingarmAnim, swingarmState.pos);
}

void LC34::clbkPreStep(double simt, double simdt, double mjd)
{
	if (!firstTimestepDone) DoFirstTimestep();

	MissionTime = (oapiGetSimMJD() - LaunchMJD)*24.0*3600.0;

	if (swingarmState.Moving()) {
		double dp = simdt * 0.25;
		swingarmState.Move(dp);
		SetAnimation(swingarmAnim, swingarmState.pos);
	}

	switch (state) {
	case STATE_PRELAUNCH:
		// Move MSS, no clue if 20min are OK?
		if (mssProc < 1) {
			mssProc = min(1.0, mssProc + simdt / (60.0 * 20.0));
			SetAnimation(mssAnim, mssProc);
		}

		if (MissionTime > -3 * 3600)
		{
			if (sat) sat->ActivatePrelaunchVenting();
		}

		// T-33min or later?
		if (MissionTime > -33 * 60) {
			mssProc = 1;
			SetAnimation(mssAnim, mssProc);
			state = STATE_CMARM1;
		}
		break;

	case STATE_CMARM1:
		// Move CM arm 12 deg, no clue if 60s are OK?
		if (cmarmProc < 12.0 / 180.0 * 0.7) {
			cmarmProc = min(12.0 / 180.0 * 0.7, cmarmProc + simdt / 60.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}

		if (sat) sat->ActivatePrelaunchVenting();

		// T-5min or later?
		if (MissionTime > -5 * 60) {
			cmarmProc = 12.0 / 180.0 * 0.7;
			SetAnimation(cmarmAnim, cmarmProc);
			state = STATE_CMARM2;
		}
		break;

	case STATE_CMARM2:
		// Move CM arm to retracted position, no clue if 60s are OK?
		if (cmarmProc < 1) {
			cmarmProc = min(1.0, cmarmProc + simdt / 60.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}

		if (sat) sat->ActivatePrelaunchVenting();

		// T-2:43min or later?
		if (MissionTime > -163.0)
		{
			if (CutoffInterlock())
			{
				Hold = true;
			}
			else if (Hold == false)
			{
				cmarmProc = 1;
				SetAnimation(cmarmAnim, cmarmProc);
				state = STATE_TERMINAL_COUNT;
			}
		}

		break;
	case STATE_TERMINAL_COUNT:

		if (sat) sat->ActivatePrelaunchVenting();

		///GRR should happen at a fairly precise time and usually happens on the next timestep, so adding oapiGetSimStep is a decent solution
		if (MissionTime >= -(17.0 + oapiGetSimStep()))
		{
			IuESE->SetGuidanceReferenceRelease(true);
		}
		else
		{
			IuUmb->LVDCPrepareToLaunch();
		}

		// T-3.1s or later?
		if (MissionTime > -3.1)
		{
			if (CutoffInterlock())
			{
				Hold = true;
				break;
			}
			else if (Hold == false)
			{
				if (sat) sat->DeactivatePrelaunchVenting();
				state = STATE_IGNITION_SEQUENCE;
			}
			else break;
		}
		else break;
		//Fall Into
	case STATE_IGNITION_SEQUENCE:

		double lvl;
		if (sat)
		{
			lvl = sat->GetSIThrustLevel();
		}
		else if (s1b)
		{
			lvl = s1b->GetThrusterGroupLevel(THGROUP_MAIN);
		}
		else
		{
			lvl = 0.0;
		}
		if (MissionTime > -2.0 && MissionTime < -1.0)
			liftoffStreamLevel = lvl * (MissionTime + 2.0) / 1.0;
		else
			liftoffStreamLevel = lvl;

		if (CutoffInterlock())
		{
			Hold = true;
			SCMUmb->SIGSECutoff(true);
		}
		else if (Hold == false)
		{
			//Hold-down force
			if (sat)
			{
				sat->AddForce(_V(0, 0, -8. * sat->GetFirstStageThrust()), _V(0, 0, 0)); // Maintain hold-down lock
			}
			else if (s1b)
			{
				s1b->AddForce(_V(0, 0, -8. * 1008000), _V(0, 0, 0)); // Maintain hold-down lock
			}

			if (MissionTime > -3.1)
			{
				SCMUmb->SetEngineStart(5);
				SCMUmb->SetEngineStart(7);
			}
			if (MissionTime > -3.0)
			{
				SCMUmb->SetEngineStart(6);
				SCMUmb->SetEngineStart(8);
			}
			if (MissionTime > -2.9)
			{
				SCMUmb->SetEngineStart(2);
				SCMUmb->SetEngineStart(4);
			}
			if (MissionTime > -2.8)
			{
				SCMUmb->SetEngineStart(1);
				SCMUmb->SetEngineStart(3);
			}

			// T-1s or later?
			if (MissionTime > -1) {
				state = STATE_LIFTOFF;
			}
		}

		break;

	case STATE_LIFTOFF:

		if (sat)
		{
			liftoffStreamLevel = sat->GetSIThrustLevel();
		}
		else if (s1b)
		{
			liftoffStreamLevel = s1b->GetThrusterGroupLevel(THGROUP_MAIN);
		}
		else
		{
			liftoffStreamLevel = 0.0;
		}

		//Cutoff
		if (MissionTime > 6.0 && (sat && sat->GetStage() <= PRELAUNCH_STAGE))
		{
			SCMUmb->SIGSECutoff(true);
		}

		if (CutoffInterlock())
		{
			Hold = true;
			SCMUmb->SIGSECutoff(true);
		}
		else if (Hold == false)
		{
			// Soft-Release Pin Dragging
			if (MissionTime < 0.5) {
				double PinDragFactor = min(1.0, 1.0 - (MissionTime * 2.0));
				if (sat)
				{
					sat->AddForce(_V(0, 0, -(sat->GetFirstStageThrust() * PinDragFactor)), _V(0, 0, 0));
				}
				else if (s1b)
				{
					s1b->AddForce(_V(0, 0, -8. * 1008000 * PinDragFactor), _V(0, 0, 0));
				}
			}

			if (bCommit == false && MissionTime >= (-0.05 - simdt))
			{
				if (Commit())
				{
					bCommit = true;
				}
				else
				{
					bCommit = false;
				}

				if (bCommit)
				{
					// Disconnect Umbilicals
					IuUmb->Disconnect();
					SCMUmb->Disconnect();

					// Move swingarms
					swingarmState.action = AnimState::OPENING;
				}
				else
				{
					Hold = true;
					SCMUmb->SIGSECutoff(true);
				}
			}

			// T+4s or later?
			if (MissionTime > 4) {
				state = STATE_POSTLIFTOFF;
			}
		}

		break;

	case STATE_POSTLIFTOFF:

		if (MissionTime < 10.0)
		{
			double lvl;
			if (sat)
			{
				lvl = sat->GetSIThrustLevel();
			}
			else if (s1b)
			{
				lvl = s1b->GetThrusterGroupLevel(THGROUP_MAIN);
			}
			else
			{
				lvl = 0.0;
			}

			liftoffStreamLevel = lvl *(MissionTime - 10.0) / -6.0;
		}
		else
		{
			liftoffStreamLevel = 0;

			//
			// Once the stream is finished, forget about the vessel since we won't be
			// using it again. This prevents a crash if we later delete the vessel.
			//
			hLV = 0;
			sat = 0;
			SIBName[0] = '\0';
			SIVBName[0] = '\0';
			s1b = NULL;
			sivb = NULL;
		}
		break;
	}

	//IU ESE
	if (sat && state >= STATE_PRELAUNCH)
	{
		IuESE->Timestep(MissionTime, simdt);
		SIBESE->Timestep();
	}
}

void LC34::clbkPostStep (double simt, double simdt, double mjd) {

	if (!firstTimestepDone) return;

	// Nothing for now
}

void LC34::DoFirstTimestep()
{
	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	firstTimestepDone = true;
}

void LC34::SetTouchdownPointHeight(double height) {

	touchdownPointHeight = height;
	SetTouchdownPoints(_V(  0, touchdownPointHeight,  10), 
					   _V(-10, touchdownPointHeight, -10), 
					   _V( 10, touchdownPointHeight, -10));
}

void LC34::DefineAnimations() {

	// MSS
	static MGROUP_TRANSLATE mgroupMSS(meshindexMSS, NULL, 0, _V(136, 0, 150));	
	mssAnim = CreateAnimation(0.0);
	AddAnimationComponent(mssAnim, 0, 1, &mgroupMSS);

	// CM access arm
	static UINT cmarm_groups[2] = {16, 20};
	static MGROUP_ROTATE mgroupCMArm(meshindexLUT, cmarm_groups, 2, _V(-9.6, 0, -6.8), _V( 0, 1, 0), (float)(-0.7 * PI));
	cmarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(cmarmAnim, 0, 1, &mgroupCMArm);

	// Swingarms
	static UINT swingarm_groups1[2] = {17, 19};
	static UINT swingarm_groups2[2] = {18, 21};
	static UINT swingarm_groups3[2] = {2, 4};
	static UINT swingarm_groups4[2] = {0, 1};
	static MGROUP_ROTATE mgroupSwingarm1(meshindexLUT, swingarm_groups1, 2, _V(-9.3, 0, -6.5), _V( 0,  1, 0), (float)(75.0 / 180.0 * PI));
	static MGROUP_ROTATE mgroupSwingarm2(meshindexLUT, swingarm_groups2, 2, _V(-9.0, 0, -6.3), _V( 0,  1, 0), (float)(75.0 / 180.0 * PI));
	static MGROUP_ROTATE mgroupSwingarm3(meshindexLUT, swingarm_groups3, 2, _V(-8.6, 0, -6.0), _V( 0,  1, 0), (float)(75.0 / 180.0 * PI));
	static MGROUP_ROTATE mgroupSwingarm4(meshindexLUT, swingarm_groups4, 2, _V(-8.6, 0, -6.0), _V( 0, -1, 0), (float)(90.0 / 180.0 * PI));
	swingarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm1);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm2);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm3);
	AddAnimationComponent(swingarmAnim, 0, 1, &mgroupSwingarm4);
}

void LC34::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {

		papiReadScenario_double(line, "LAUNCHMJD", LaunchMJD);
		papiReadScenario_bool(line, "HOLD", Hold);
		papiReadScenario_bool(line, "COMMIT", bCommit);
		papiReadScenario_string(line, "SIBNAME", SIBName);
		papiReadScenario_string(line, "SIVBNAME", SIVBName);
		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%i", &state);
		} else if (!strnicmp (line, "TOUCHDOWNPOINTHEIGHT", 20)) {
			sscanf (line + 20, "%lf", &touchdownPointHeight);
		} else if (!strnicmp (line, "MSSPROC", 7)) {
			sscanf (line + 7, "%lf", &mssProc);
		} else if (!strnicmp (line, "CMARMPROC", 9)) {
			sscanf (line + 9, "%lf", &cmarmProc);
		} else if (!strnicmp (line, "SWINGARMSTATE", 13)) {
			sscan_state(line + 13, swingarmState);
		} else if (!strnicmp (line, "LVNAME", 6)) {
			strncpy (LVName, line + 7, 64);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
	SetTouchdownPointHeight(touchdownPointHeight);
}

void LC34::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "STATE", state);
	papiWriteScenario_bool(scn, "HOLD", Hold);
	papiWriteScenario_bool(scn, "COMMIT", bCommit);
	papiWriteScenario_double(scn, "TOUCHDOWNPOINTHEIGHT", touchdownPointHeight);
	papiWriteScenario_double(scn, "MSSPROC", mssProc);
	papiWriteScenario_double(scn, "CMARMPROC", cmarmProc);
	WriteScenario_state(scn, "SWINGARMSTATE", swingarmState);
	if (LVName[0])
		oapiWriteScenario_string(scn, "LVNAME", LVName);
	if (SIBName[0])
		oapiWriteScenario_string(scn, "SIBNAME", SIBName);
	if (SIVBName[0])
		oapiWriteScenario_string(scn, "SIVBNAME", SIVBName);
	papiWriteScenario_double(scn, "LAUNCHMJD", LaunchMJD);
}

int LC34::clbkConsumeDirectKey(char *kstate) {

	if (KEYMOD_SHIFT(kstate)) {
		return 0; 
	}

/*	DelMesh(meshindexMSS);

	double step = 0.01;
	if (KEYMOD_CONTROL(kstate))
		step = 0.001;

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {
		meshoffsetMSS.x -= step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {
		meshoffsetMSS.x += step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		meshoffsetMSS.y -= step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		meshoffsetMSS.y += step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD1)) {
		meshoffsetMSS.z -= step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		meshoffsetMSS.z += step;
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}

    meshindexMSS = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\CanaveralLC34MSS"), &meshoffsetMSS);
	SetMeshVisibilityMode(meshindexMSS, MESHVIS_ALWAYS);
	sprintf(oapiDebugString(), "x %f y %f z %f", meshoffsetMSS.x, meshoffsetMSS.y, meshoffsetMSS.z);
*/

/*	ATTACHMENTHANDLE ah = GetAttachmentHandle(true, 0);
	VECTOR3 pos, dir, rot;
	GetAttachmentParams (ah, pos, dir, rot);

	double step = 0.01;
	if (KEYMOD_CONTROL(kstate))
		step = 0.001;

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {
		pos.x -= step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {
		pos.x += step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		pos.y -= step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		pos.y += step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD1)) {
		pos.z -= step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		pos.z += step;
		SetAttachmentParams (ah, pos, dir, rot);
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}

	sprintf(oapiDebugString(), "x %f y %f z %f", pos.x, pos.y, pos.z);
*/
/*
	VESSELSTATUS vs;
	GetStatus(vs);
	double moveStep = 1.0e-7;
	if (KEYMOD_CONTROL(kstate))
		moveStep = 1.0e-9;

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		vs.vdata[0].x += moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {			
		vs.vdata[0].y -= moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);			
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {			
		vs.vdata[0].y += moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		vs.vdata[0].x -= moveStep;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD1)) {
		vs.vdata[0].z -= 1.0e-4;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		vs.vdata[0].z += 1.0e-4;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		SetTouchdownPoints (_V(0, -GetCOG_elev() + 0.01, 1), _V(-1, -GetCOG_elev() + 0.01, -1), _V(1, -GetCOG_elev() + 0.01, -1));
		RESETKEY(kstate, OAPI_KEY_A);
	}
	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		SetTouchdownPoints (_V(0, -GetCOG_elev() - 0.01, 1), _V(-1, -GetCOG_elev() - 0.01, -1), _V(1, -GetCOG_elev() - 0.01, -1));
		RESETKEY(kstate, OAPI_KEY_S);
	}
	sprintf(oapiDebugString(), "GetCOG_elev %f", GetCOG_elev());
*/

	return 0;
}

int LC34::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0;
	}
	return 0;
}

bool LC34::CutoffInterlock()
{
	if (IuUmb->IsEDSUnsafeA())
	{
		oapiWriteLog("LC34: EDS Unsafe A signal received. Countdown Hold!");
		return true;
	}
	else if (IuUmb->IsEDSUnsafeB())
	{
		oapiWriteLog("LC34: EDS Unsafe B signal received. Countdown Hold!");
		return true;
	}
	else if (SCMUmb->SIStageLogicCutoff())
	{
		oapiWriteLog("LC34: S-I Stage Logic Cutoff signal received. Countdown Hold!");
		return true;
	}

	return false;
}

bool LC34::Commit()
{
	if (!sat && (!s1b || !sivb)) return false;

	if (IuUmb->AllSIEnginesRunning() == false)
	{
		oapiWriteLog("LC34: All Engines Running signal not received. Countdown Hold!");
		return false;
	}
	else if (CutoffInterlock())
	{
		return false;
	}

	return true;
}

bool LC34::ESEGetCommandVehicleLiftoffIndicationInhibit()
{
	return IuESE->GetCommandVehicleLiftoffIndicationInhibit();
}

bool LC34::ESEGetExcessiveRollRateAutoAbortInhibit(int n)
{
	return IuESE->GetExcessiveRollRateAutoAbortInhibit(n);
}

bool LC34::ESEGetExcessivePitchYawRateAutoAbortInhibit(int n)
{
	return IuESE->GetExcessivePitchYawRateAutoAbortInhibit(n);
}

bool LC34::ESEGetTwoEngineOutAutoAbortInhibit(int n)
{
	return IuESE->GetTwoEngineOutAutoAbortInhibit(n);
}

bool LC34::ESEGetGSEOverrateSimulate(int n)
{
	return IuESE->GetOverrateSimulate(n);
}

bool LC34::ESEGetEDSPowerInhibit()
{
	return IuESE->GetEDSPowerInhibit();
}

bool LC34::ESEPadAbortRequest()
{
	return IuESE->GetEDSPadAbortRequest();
}

bool LC34::ESEGetThrustOKIndicateEnableInhibitA()
{
	return IuESE->GetThrustOKIndicateEnableInhibitA();
}

bool LC34::ESEGetThrustOKIndicateEnableInhibitB()
{
	return IuESE->GetThrustOKIndicateEnableInhibitB();
}

bool LC34::ESEEDSLiftoffInhibitA()
{
	return IuESE->GetEDSLiftoffInhibitA();
}

bool LC34::ESEEDSLiftoffInhibitB()
{
	return IuESE->GetEDSLiftoffInhibitB();
}

bool LC34::ESEGetEDSAutoAbortSimulate(int n)
{
	return IuESE->GetEDSAutoAbortSimulate(n);
}

bool LC34::ESEGetEDSLVCutoffSimulate(int n)
{
	return IuESE->GetEDSLVCutoffSimulate(n);
}

bool LC34::ESEGetSIBurnModeSubstitute()
{
	return IuESE->GetSIBurnModeSubstitute();
}

bool LC34::ESEGetGuidanceReferenceRelease()
{
	return IuESE->GetGuidanceReferenceRelease();
}

bool LC34::ESEGetQBallSimulateCmd()
{
	return IuESE->GetQBallSimulateCmd();
}

bool LC34::ESEGetSIBThrustOKSimulate(int eng, int n)
{
	return SIBESE->GetSIBThrustOKSimulate(eng, n);
}

void LC34::SLCCCheckDiscreteInput(RCA110A *c)
{
	c->SetInput(0, true);
	c->SetInput(1, false);
	c->SetInput(861, IuESE->GetFCCPowerIsOn());
}

bool LC34::SLCCGetOutputSignal(size_t n)
{
	return rca110a->GetOutputSignal(n);
}

void LC34::ConnectGroundComputer(RCA110A *c)
{
	rca110a->Connect(c);
}

void LC34::IssueSwitchSelectorCmd(int stage, int chan)
{
	IuUmb->SwitchSelector(stage, chan);
}