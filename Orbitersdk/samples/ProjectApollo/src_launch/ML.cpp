/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ML vessel

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
#include "nasspsound.h"
#include "soundlib.h"
#include "tracer.h"

#include "RCA110A.h"
#include "ML.h"
#include "nasspdefs.h"
#include "saturn.h"
#include "papi.h"
#include "IUUmbilical.h"
#include "TSMUmbilical.h"
#include "IU_ESE.h"
#include "SIC_ESE.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo ML.log";

// States
#define STATE_VABBUILD		   -2
#define STATE_VABREADY		   -1
#define STATE_ROLLOUT			0
#define STATE_PRELAUNCH			1
#define STATE_CMARM1			2
#define STATE_CMARM2			3
#define STATE_TERMINAL_COUNT	4
#define STATE_SICINTERTANKARM	5
#define STATE_SICFORWARDARM		6
#define STATE_IGNITION_SEQUENCE 7
#define STATE_LIFTOFFSTREAM		8
#define STATE_LIFTOFF			9
#define STATE_POSTLIFTOFF		10

// Pad and VAB coordinates
#define VAB_LON -80.6509353	///\todo fix for Orbiter 2010-P1
#define VAB_LAT 28.5860469

#define PAD_LONA -80.6041122
#define PAD_LATA 28.6083723

#define PAD_LONB -80.6208972
#define PAD_LATB 28.6272345

#define PAD_LV_LONA -80.6041140
#define PAD_LV_LATA 28.6082888

#define PAD_LV_LONB -80.620899
#define PAD_LV_LATB 28.627151


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

	return new ML(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (ML*)vessel;
}


ML::ML(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	firstTimestepDone = false;
	LVName[0] = '\0';
	moveToPadA = false;
	moveToPadB = false;
	moveToVab = false;
	moveLVToPadA = false;
	moveLVToPadB = false;
	touchdownPointHeight = -86.677; // pad height
	hLV = 0;
	state = STATE_ROLLOUT;
	Hold = false;
	bCommit = false;
	TCSSequence = 0;

	craneProc = 0;
	cmarmProc = 0.00001;
	s2aftarmState.Set(AnimState::CLOSED, 0.0);
	damperarmState.Set(AnimState::CLOSED, 0.0);
	s1cintertankarmState.Set(AnimState::CLOSED, 0.0);
	s1cforwardarmState.Set(AnimState::CLOSED, 0.0);
	swingarmState.Set(AnimState::CLOSED, 0.0);
	mastState.Set(AnimState::CLOSED, 0.0);
	mastcoversState.Set(AnimState::CLOSED, 0.0);

	int i;
	for (i = 0; i < 2; i++) {
		liftoffStream[i] = NULL;
	}
	liftoffStreamLevel = 0;

	soundlib.InitSoundLib(hObj, SOUND_DIRECTORY);

	sat = NULL;

	IuUmb = new IUUmbilical(this);
	IuESE = new IUSV_ESE(IuUmb, this);
	TSMUmb = new TSMUmbilical(this);
	SICESE = new SIC_ESE(TSMUmb, this);
	rca110a = new RCA110AM(this);
}

ML::~ML() {
	delete IuUmb;
	delete IuESE;
	delete TSMUmb;
	delete SICESE;
	delete rca110a;
}

void ML::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(100000);
	SetSize(120);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0,0,0);
    meshindexML = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Saturn5ML"), &meshoffset);
    //meshindexML = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\LUT"), &meshoffset);
	SetMeshVisibilityMode(meshindexML, MESHVIS_ALWAYS);

	DefineAnimations();

	liftoffStream[0] = AddParticleStream(&liftoffStreamSpec, _V(0, -80, 0), _V(0, 0.4,  1), &liftoffStreamLevel);
	liftoffStream[1] = AddParticleStream(&liftoffStreamSpec, _V(0, -80, 0), _V(0, 0.4, -1), &liftoffStreamLevel);

	SetTouchdownPointHeight(touchdownPointHeight);
}

void ML::clbkPostCreation()
{	
	char buffer[256];

	if (swingarmState.action == AnimState::CLOSED)
	{
		double vcount = oapiGetVesselCount();
		for (int i = 0; i < vcount; i++) {
			OBJHANDLE h = oapiGetVesselByIndex(i);
			oapiGetObjectName(h, buffer, 256);
			if (!strcmp(LVName, buffer)) {
				hLV = h;
				Saturn *sat = (Saturn *)oapiGetVesselInterface(hLV);
				if (sat->GetStage() < LAUNCH_STAGE_ONE)
				{
					IuUmb->Connect(sat->GetIU());
					TSMUmb->Connect(sat->GetSIC());
				}
			}
		}
	}

	SetAnimation(craneAnim, craneProc);
	SetAnimation(cmarmAnim, cmarmProc);
	SetAnimation(s2aftarmAnim, s2aftarmState.pos);
	SetAnimation(damperarmAnim, damperarmState.pos);
	SetAnimation(s1cintertankarmAnim, s1cintertankarmState.pos);
	SetAnimation(s1cforwardarmAnim, s1cforwardarmState.pos);
	SetAnimation(swingarmAnim, swingarmState.pos);
	SetAnimation(mastAnim, mastState.pos);
	SetAnimation(mastcoversAnim, mastcoversState.pos);

}

void ML::clbkPreStep(double simt, double simdt, double mjd) {

	if (s1cintertankarmState.Moving()) {
		double dp;
		if (s1cintertankarmState.Closing())
		{
			dp = simdt * ML_SIC_INTERTANK_ARM_CONNECTING_SPEED;
		}
		else
		{
			dp = simdt * ML_SIC_INTERTANK_ARM_RETRACT_SPEED;
		}
		s1cintertankarmState.Move(dp);
		SetAnimation(s1cintertankarmAnim, s1cintertankarmState.pos);
	}

	if (s1cforwardarmState.Moving()) {
		double dp;
		if (s1cforwardarmState.Closing())
		{
			dp = simdt * ML_SIC_FORWARD_ARM_CONNECTING_SPEED;
		}
		else
		{
			dp = simdt * ML_SIC_FORWARD_ARM_RETRACT_SPEED;
		}
		s1cforwardarmState.Move(dp);
		SetAnimation(s1cforwardarmAnim, s1cforwardarmState.pos);
	}
	if (s2aftarmState.Moving()) {
		double dp;
		if (s2aftarmState.Closing())
		{
			dp = simdt * ML_SWINGARM_CONNECTING_SPEED;
		}
		else
		{
			dp = simdt * ML_SWINGARM_RETRACT_SPEED;
		}
		s2aftarmState.Move(dp);
		SetAnimation(s2aftarmAnim, s2aftarmState.pos);
	}
	if (swingarmState.Moving()) {
		double dp;
		if (swingarmState.Closing())
		{
			dp = simdt * ML_SWINGARM_CONNECTING_SPEED;
		}
		else
		{
			dp = simdt * ML_SWINGARM_RETRACT_SPEED;
		}
		swingarmState.Move(dp);
		SetAnimation(swingarmAnim, swingarmState.pos);
	}
	if (damperarmState.Moving()) {
		double dp;
		if (damperarmState.Closing())
		{
			dp = simdt * DAMPERARM_CONNECTING_SPEED;
		}
		else
		{
			dp = simdt * DAMPERARM_RETRACT_SPEED;
		}
		damperarmState.Move(dp);
		SetAnimation(damperarmAnim, damperarmState.pos);
	}
	if (mastState.Moving()) {
		double dp;
		if (mastState.Closing())
		{
			dp = simdt * ML_TAIL_SERVICE_MAST_CONNECTING_SPEED;
		}
		else
		{
			dp = simdt * ML_TAIL_SERVICE_MAST_RETRACT_SPEED;
		}
		mastState.Move(dp);
		SetAnimation(mastAnim, mastState.pos);
	}
	if (mastcoversState.Moving()) {
		double dp;
		if (mastcoversState.Closing())
		{
			dp = simdt * ML_TAIL_SERVICE_MAST_COVERS_CONNECTING_SPEED;
		}
		else
		{
			dp = simdt * ML_TAIL_SERVICE_MAST_COVERS_RETRACT_SPEED;
		}
		mastcoversState.Move(dp);
		SetAnimation(mastcoversAnim, mastcoversState.pos);
	}

	ATTACHMENTHANDLE ah;

	if (!firstTimestepDone) DoFirstTimestep();
	
	switch (state) {
	case STATE_VABBUILD:

		if (craneProc < 0.25) {
			craneProc = min(0.25, craneProc + simdt / 600.0);
			SetAnimation(craneAnim, craneProc);
		}
		if (cmarmProc < 0.09) {
			cmarmProc = min(0.09, cmarmProc + simdt / 1000.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}
		if (damperarmState.action != AnimState::OPEN) damperarmState.action = AnimState::OPENING;
		if (s2aftarmState.action != AnimState::OPEN) s2aftarmState.action = AnimState::OPENING;
		if (swingarmState.action != AnimState::OPEN) swingarmState.action = AnimState::OPENING;
		if (s1cintertankarmState.action != AnimState::OPEN) s1cintertankarmState.action = AnimState::OPENING;
		if (s1cforwardarmState.action != AnimState::OPEN) s1cforwardarmState.action = AnimState::OPENING;
		if (mastState.action != AnimState::OPEN) mastState.action = AnimState::OPENING;
		if (mastcoversState.action != AnimState::OPEN) mastcoversState.action = AnimState::OPENING;
		break;

	case STATE_VABREADY:
	case STATE_ROLLOUT:

		if (craneProc > 0) {
			craneProc = max(0, craneProc - simdt / 600.0);
			SetAnimation(craneAnim, craneProc);
		}
		if (cmarmProc > 0.00001) {
			cmarmProc = max(0.00001, cmarmProc - simdt / 1000.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}

		if (damperarmState.action != AnimState::CLOSED) damperarmState.action = AnimState::CLOSING;
		if (s2aftarmState.action != AnimState::CLOSED) s2aftarmState.action = AnimState::CLOSING;
		if (swingarmState.action != AnimState::CLOSED) swingarmState.action = AnimState::CLOSING;
		if (s1cintertankarmState.action != AnimState::CLOSED) s1cintertankarmState.action = AnimState::CLOSING;
		if (s1cforwardarmState.action != AnimState::CLOSED) s1cforwardarmState.action = AnimState::CLOSING;
		if (mastState.action != AnimState::CLOSED) mastState.action = AnimState::CLOSING;
		if (mastcoversState.action != AnimState::CLOSED) mastcoversState.action = AnimState::CLOSING;

		if (state == STATE_VABREADY) break;

		// Detached from crawler?
		ah = GetAttachmentHandle(true, 0);
		if (GetAttachmentStatus(ah) != NULL) break;
			
		// On pad?
		if ((GetDistanceTo(PAD_LONA, PAD_LATA) > 10.0) && (GetDistanceTo(PAD_LONB, PAD_LATB) > 10.0)) break;

		// T-3h or later?
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);
		if (sat->GetMissionTime() > -3 * 3600) {
			
			if (oapiGetTimeAcceleration() > 1) {
				oapiSetTimeAcceleration(1);
			}
			
			// Detach Saturn
			ah = GetAttachmentHandle(false, 0);
			if (GetAttachmentStatus(ah) != NULL) {
				DetachChild(ah);
				// and move to pad
				if (GetDistanceTo(PAD_LONB, PAD_LATB) < 10.0)
				{
					moveLVToPadB = true;
				}
				else
				{
					moveLVToPadA = true;
				}
				// and notify Saturn
				sat->LaunchVehicleRolloutEnd();
				
			}
			state = STATE_PRELAUNCH;
		}
		break;

	case STATE_PRELAUNCH:
		// Move hammerhead crane, no clue if 60s are OK?
		if (craneProc < 1) {
			craneProc = min(1.0, craneProc + simdt / 60.0);
			SetAnimation(craneAnim, craneProc);

			// Move S2 AFT forward arm
			s2aftarmState.action = AnimState::OPENING;

			// Move Damper arm
			damperarmState.action = AnimState::OPENING;

		}

		// T-43min or later?
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		sat->ActivatePrelaunchVenting();

		if (sat->GetMissionTime() > -43 * 60) {
			craneProc = 1;
			SetAnimation(craneAnim, craneProc);
			state = STATE_CMARM1;
		}
		break;

	case STATE_CMARM1:
		// Move CM arm 12 deg, no clue if 60s are OK?
		if (cmarmProc < 12.0 / 180.0) {
			cmarmProc = min(12.0 / 180.0, cmarmProc + simdt / 60.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}

		// T-5min or later?
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		sat->ActivatePrelaunchVenting();

		if (sat->GetMissionTime() > -5 * 60) {
			cmarmProc = 12.0 / 180.0;
			SetAnimation(cmarmAnim, cmarmProc);
			state = STATE_CMARM2;
		}
		break;

	case STATE_CMARM2:
		// Move CM arm to 180 deg, no clue if 60s are OK?
		if (cmarmProc < 1) {
			cmarmProc = min(1.0, cmarmProc + simdt / 60.0);
			SetAnimation(cmarmAnim, cmarmProc);
		}

		// T-30s or later?
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		sat->ActivatePrelaunchVenting();

		if (sat->GetMissionTime() >= -187.0)
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
		
		if (!hLV) break;
		sat = (Saturn *)oapiGetVesselInterface(hLV);

		sat->ActivatePrelaunchVenting();
		
		if (sat->GetMissionTime() > -30)
		{
			if (CutoffInterlock())
			{
				Hold = true;
			}
			else if (Hold == false)
			{
				// Move SIC intertank arm
				s1cintertankarmState.action = AnimState::OPENING;
				state = STATE_SICINTERTANKARM;
			}
		}
		break;
	case STATE_SICINTERTANKARM:

		
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		sat->ActivatePrelaunchVenting();

		//GRR should happen at a fairly precise time and usually happens on the next timestep, so adding oapiGetSimStep is a decent solution
		if (sat->GetMissionTime() >= -(17.0 + oapiGetSimStep()))
		{
			IuESE->SetGuidanceReferenceRelease(true);
		}
		else
		{
			IuUmb->LVDCPrepareToLaunch();
		}

		// T-16.2s or later?
		if (sat->GetMissionTime() > -16.2)
		{
			if (CutoffInterlock())
			{
				Hold = true;
			}
			else if (Hold == false)
			{
				// Move SIC forward arm
				s1cforwardarmState.action = AnimState::OPENING;
				state = STATE_SICFORWARDARM;
			}
		}
		break;

	case STATE_SICFORWARDARM:

		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		sat->ActivatePrelaunchVenting();

		// T-8.9s or later?
		if (sat->GetMissionTime() > -8.9)
		{
			if (CutoffInterlock())
			{
				Hold = true;
				break;
			}
			else if (Hold == false)
			{
				sat->DeactivatePrelaunchVenting();
				state = STATE_IGNITION_SEQUENCE;
			}
			else break;
		}
		else break;
		//Fall into
	case STATE_IGNITION_SEQUENCE:

		if (CutoffInterlock())
		{
			Hold = true;
			TSMUmb->SIGSECutoff(true);
		}
		else if (Hold == false)
		{
			if (sat->GetMissionTime() > -8.9)
			{
				TSMUmb->SetEngineStart(5);
			}
			if (sat->GetMissionTime() > -8.62)
			{
				TSMUmb->SetEngineStart(1);
				TSMUmb->SetEngineStart(3);
			}
			if (sat->GetMissionTime() > -8.2)
			{
				TSMUmb->SetEngineStart(2);
				TSMUmb->SetEngineStart(4);
			}

			// T-4.9s or later?
			if (sat->GetMissionTime() > -4.9) {
				state = STATE_LIFTOFFSTREAM;
			}
		}
		break;

	case STATE_LIFTOFFSTREAM:
		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);

		if (sat->GetMissionTime() < -2.0)
			liftoffStreamLevel = sat->GetSIThrustLevel()*(sat->GetMissionTime() + 4.9) / 2.9;
		else
			liftoffStreamLevel = sat->GetSIThrustLevel();

		// T-1s or later?
		if (CutoffInterlock())
		{
			Hold = true;
			TSMUmb->SIGSECutoff(true);
		}
		else if (Hold == false)
		{
			//Hold-down force
			if (sat->GetMissionTime() > -4.0) {
				sat->AddForce(_V(0, 0, -5. * sat->GetFirstStageThrust()), _V(0, 0, 0));
			}

			// T-1s or later?
			if (sat->GetMissionTime() > -1) {
				state = STATE_LIFTOFF;
			}
		}

		break;
	
	case STATE_LIFTOFF:

		if (!hLV) break;
		sat = (Saturn *)oapiGetVesselInterface(hLV);

		liftoffStreamLevel = sat->GetSIThrustLevel();

		//Cutoff
		if (sat->GetMissionTime() > 6.0 && sat->GetStage() <= PRELAUNCH_STAGE)
		{
			TSMUmb->SIGSECutoff(true);
		}

		if (CutoffInterlock())
		{
			Hold = true;
			TSMUmb->SIGSECutoff(true);
		}
		else if (Hold == false)
		{
			// Soft-Release Pin Dragging
			if (sat->GetMissionTime() < 0.5)
			{
				double PinDragFactor = min(1.0, 1.0 - (sat->GetMissionTime() * 2.0));
				sat->AddForce(_V(0, 0, -(sat->GetFirstStageThrust() * PinDragFactor)), _V(0, 0, 0));
			}

			if (bCommit == false && sat->GetMissionTime() >= (-0.05 - simdt))
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
					IuUmb->Disconnect();
					TSMUmb->Disconnect();
					// Move swingarms
					swingarmState.action = AnimState::OPENING;
					// Move masts
					mastState.action = AnimState::OPENING;
				}
				else
				{
					Hold = true;
					TSMUmb->SIGSECutoff(true);
				}
			}

			// T+0.1s or later?
			if (sat->GetMissionTime() > 1.8) {
				// Close Covers
				mastcoversState.action = AnimState::OPENING;
			}

			// T+8s or later?
			if (sat->GetMissionTime() > 8) {
				state = STATE_POSTLIFTOFF;
			}
		}
		break;

	case STATE_POSTLIFTOFF:

		if (!hLV) break;
		sat = (Saturn *) oapiGetVesselInterface(hLV);
		if (sat->GetMissionTime() < 13.0)
			liftoffStreamLevel = sat->GetSIThrustLevel()*(sat->GetMissionTime() - 13.0) / -5.0;
		else
		{
			liftoffStreamLevel = 0;

			//
			// Once the stream is finished, forget about the vessel since we won't be
			// using it again. This prevents a crash if we later delete the vessel.
			//
			hLV = 0;
			sat = 0;
		}
		break;
	}

	//IU ESE
	if (sat && state >= STATE_ROLLOUT)
	{
		IuESE->Timestep(sat->GetMissionTime(), simdt);
		SICESE->Timestep();
		rca110a->Timestep(simt, simdt);
	}

	// sprintf(oapiDebugString(), "Dist %f", GetDistanceTo(VAB_LON, VAB_LAT));
}

void ML::clbkPostStep (double simt, double simdt, double mjd) {

	if (!firstTimestepDone) return;

	// Move ML to pad!
	if (moveToPadA) {
		VESSELSTATUS vs;
		GetStatus(vs);

		vs.status = 1;
		vs.vdata[0].x = PAD_LONA * RAD;
		vs.vdata[0].y = PAD_LATA * RAD;
		vs.vdata[0].z = 0.58 * RAD; 
		DefSetState(&vs);
		moveToPadA = false;
	}

	if (moveToPadB) {
		VESSELSTATUS vs;
		GetStatus(vs);

		vs.status = 1;
		vs.vdata[0].x = PAD_LONB * RAD;
		vs.vdata[0].y = PAD_LATB * RAD;
		vs.vdata[0].z = 0.58 * RAD;
		DefSetState(&vs);
		moveToPadB = false;
	}

	// Move ML to VAB, position is hardcoded!
	if (moveToVab) {
		VESSELSTATUS vs;
		GetStatus(vs);

		vs.status = 1;
		vs.vdata[0].x = VAB_LON * RAD;
		vs.vdata[0].y = VAB_LAT * RAD;
		vs.vdata[0].z = 75.04 * RAD; 
		DefSetState(&vs);
		moveToVab = false;
	}

	// Move LV to pad!
	if (moveLVToPadA) {
		if (hLV) {
			VESSELSTATUS vs;
			Saturn *sat = (Saturn *) oapiGetVesselInterface(hLV);
			sat->GetStatus(vs);

			vs.status = 1;
			vs.vdata[0].x = PAD_LV_LONA * RAD;
			vs.vdata[0].y = PAD_LV_LATA * RAD;
			vs.vdata[0].z = 270.0 * RAD; 
			sat->DefSetState(&vs);
		}
		moveLVToPadA = false;
	}
	else if (moveLVToPadB) {
		if (hLV) {
			VESSELSTATUS vs;
			Saturn *sat = (Saturn *)oapiGetVesselInterface(hLV);
			sat->GetStatus(vs);

			vs.status = 1;
			vs.vdata[0].x = PAD_LV_LONB * RAD;
			vs.vdata[0].y = PAD_LV_LATB * RAD;
			vs.vdata[0].z = 270.0 * RAD;
			sat->DefSetState(&vs);
		}
		moveLVToPadB = false;
	}
}

void ML::DoFirstTimestep()
{
	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	firstTimestepDone = true;
}

bool ML::Detach() {

	// Is the crawler attached? 
	ATTACHMENTHANDLE ah = GetAttachmentHandle(true, 0);
	if (GetAttachmentStatus(ah) == NULL) return false;

	// Is the pad near?
	if (GetDistanceTo(PAD_LONA, PAD_LATA) < 10.0) {
		
		SetTouchdownPointHeight(-86.677);	// pad height
		moveToPadA = true;
		return true;
	}
	else if (GetDistanceTo(PAD_LONB, PAD_LATB) < 10.0) {

		SetTouchdownPointHeight(-86.677);	// pad height
		moveToPadB = true;
		return true;
	}

	// Is the VAB near?
	if (GetDistanceTo(VAB_LON, VAB_LAT) < 10.0) {
		
		SetTouchdownPointHeight(-71.827);
		moveToVab = true;
		state = STATE_VABREADY;
		return true;
	}
	return false;
}

bool ML::Attach() {

	// Statuscheck
	if (state != STATE_VABREADY && state != STATE_ROLLOUT) 
		return false;

	// Is the crawler detached? 
	ATTACHMENTHANDLE ah = GetAttachmentHandle(true, 0);
	if (GetAttachmentStatus(ah) != NULL) return false;

	// Is the Saturn attached?
	ah = GetAttachmentHandle(false, 0);
	if (GetAttachmentStatus(ah) == NULL) return false;

	state = STATE_ROLLOUT;
	return true;
}

void ML::SetVABBuildState() {

	if (state == STATE_VABREADY) {
		state = STATE_VABBUILD;
	}
}

void ML::SetVABReadyState() {

	if (state == STATE_VABBUILD) {
		state = STATE_VABREADY;
	}
}

bool ML::IsInVAB() {
	
	return (state == STATE_VABREADY || state == STATE_VABBUILD); 
}

double ML::GetDistanceTo(double lon, double lat) {

	double mylon, mylat, myrad, dlon, dlat;

	GetEquPos(mylon, mylat, myrad);
	dlon = (lon / DEG - mylon) * oapiGetSize(GetGravityRef());
	dlat = (lat / DEG - mylat) * oapiGetSize(GetGravityRef());

	return sqrt(dlon * dlon + dlat * dlat);
}

void ML::SetTouchdownPointHeight(double height) {

	touchdownPointHeight = height;
	SetTouchdownPoints(_V(  0, touchdownPointHeight,  10), 
					   _V(-10, touchdownPointHeight, -10), 
					   _V( 10, touchdownPointHeight, -10));
}

void ML::DefineAnimations() {

	// Hammerhead crane
	static UINT crane_groups[3] = { 21, 22, 50 };
	static MGROUP_ROTATE crane(meshindexML, crane_groups, 3, _V(0.0874, 57.8447, 14.0843), _V(0, 1, 0), (float)(0.5 * PI));
	craneAnim = CreateAnimation(0.0);
	AddAnimationComponent(craneAnim, 0, 1, &crane);

	// Damperarm
	static UINT damper_arm[1] = { 30 };
	static MGROUP_ROTATE damperarm(meshindexML, damper_arm, 1, _V(0.2221, 42.1020, 6.8092), _V(1, 0, 0), (float)(90.0 / 180.0 * PI));
	damperarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(damperarmAnim, 0, 1, &damperarm);

	// S2 Aft arm
	static UINT s2aftarm_groups[2] = { 15, 53 };
	static MGROUP_ROTATE s2aftarm(meshindexML, s2aftarm_groups, 2, _V(5.5716, -16.5039, 6.1556), _V(0, 1, 0), (float)(71.0 / 180.0 * PI));
	s2aftarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(s2aftarmAnim, 0, 1, &s2aftarm);

	// CM access arm
	static UINT cmarm_groups[3] = { 11, 12, 51 };
	static MGROUP_ROTATE cmarm1(meshindexML, cmarm_groups, 3, _V(8.7432, 39.8173, 8.9631), _V(0, 1, 0), (float)(3.0 / 180.0 * PI));
	static MGROUP_ROTATE cmarm2(meshindexML, cmarm_groups, 3, _V(8.7432, 39.8173, 8.9631), _V(0, 1, 0), (float)(-195.0 / 180.0 * PI));
	cmarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(cmarmAnim, 0, 0.00001, &cmarm1);
	AddAnimationComponent(cmarmAnim, 0.00001, 1, &cmarm2);

	// SIC intertank arm
	static UINT s1cintertankarm_groups[2] = { 13, 49 };
	static MGROUP_ROTATE s1cintertankarm(meshindexML, s1cintertankarm_groups, 2, _V(5.5646, -39.9844, 5.2651), _V(0, 1, 0), (float)(71.0 / 180.0 * PI));
	s1cintertankarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(s1cintertankarmAnim, 0, 1, &s1cintertankarm);

	// SIC forward arm
	static UINT s1cforwardarm_groups[2] = { 14, 52 };
	static MGROUP_ROTATE s1cforwardarm(meshindexML, s1cforwardarm_groups, 2, _V(5.5716, -21.0929, 6.1556), _V(0, 1, 0), (float)(71.0 / 180.0 * PI));
	s1cforwardarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(s1cforwardarmAnim, 0, 1, &s1cforwardarm);

	// Swingarms
	static UINT swingarm_groups[10] = {16, 17, 18, 19, 20, 54, 55, 56, 57, 58 };
	static MGROUP_ROTATE swingarm(meshindexML, swingarm_groups, 10, _V(5.5716, -16.5039, 6.1556), _V(0, 1, 0), (float)(71.0 / 180.0 * PI));
	swingarmAnim = CreateAnimation(0.0);
	AddAnimationComponent(swingarmAnim, 0, 1, &swingarm);

	// Masts
	static UINT mast_groups1[1] = { 3 };
	static UINT mast_groups2[1] = { 5 };
	static UINT mast_groups3[1] = { 6 };
	static MGROUP_ROTATE mast1(meshindexML, mast_groups1, 1, _V(8.6640, -54.1755, -6.2681), _V(0.2543, -0.0000, -0.9671), (float)(72.0 / 180.0 * PI));
	static MGROUP_ROTATE mast2(meshindexML, mast_groups2, 1, _V(-8.7285, -54.1755, -7.1316), _V(0.2819, -0.0000, 0.9594), (float)(72.0 / 180.0 * PI));
	static MGROUP_ROTATE mast3(meshindexML, mast_groups3, 1, _V(-8.3939, -54.1755, -12.3403), _V(-0.2664, -0.0000, 0.9639), (float)(72.0 / 180.0 * PI));
	mastAnim = CreateAnimation(0.0);
	AddAnimationComponent(mastAnim, 0, 1, &mast1);
	AddAnimationComponent(mastAnim, 0, 1, &mast2);
	AddAnimationComponent(mastAnim, 0, 1, &mast3);

	// MastCovers
	static UINT mast_cover1[1] = { 39 }; 
	static UINT mast_cover2[1] = { 38 };
	static UINT mast_cover3[1] = { 37 };
	static MGROUP_ROTATE mastCover1(meshindexML, mast_cover1, 1, _V(8.5142, -50.4372, -6.1115), _V(0.2509, 0.0000, -0.9680), (float)(-75.0 / 180.0 * PI));
	static MGROUP_ROTATE mastCover2(meshindexML, mast_cover2, 1, _V(-8.6822, -50.4372, -7.3419), _V(0.2864, 0.0000, 0.9581), (float)(-75.0 / 180.0 * PI));
	static MGROUP_ROTATE mastCover3(meshindexML, mast_cover3, 1, _V(-8.2421, -50.4372, -12.4947), _V(-0.2630, 0.0000, 0.9648), (float)(-75.0 / 180.0 * PI));
	mastcoversAnim = CreateAnimation(0.0);
	AddAnimationComponent(mastcoversAnim, 0, 1, &mastCover1);
	AddAnimationComponent(mastcoversAnim, 0, 1, &mastCover2);
	AddAnimationComponent(mastcoversAnim, 0, 1, &mastCover3);
}

void ML::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		
		papiReadScenario_bool(line, "HOLD", Hold);
		papiReadScenario_bool(line, "COMMIT", bCommit);

		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%i", &state);
		} else if (!strnicmp (line, "TOUCHDOWNPOINTHEIGHT", 20)) {
			sscanf (line + 20, "%lf", &touchdownPointHeight);
		} else if (!strnicmp (line, "CRANEPROC", 9)) {
			sscanf (line + 9, "%lf", &craneProc);
		} else if (!strnicmp (line, "CMARMPROC", 9)) {
			sscanf (line + 9, "%lf", &cmarmProc);
		} else if (!strnicmp(line, "S2AFTARMPROC", 12)) {
			sscan_state(line + 12, s2aftarmState);
		} else if (!strnicmp(line, "DAMPERARMPROC", 13)) {
			sscan_state(line + 13, damperarmState);
		} else if (!strnicmp (line, "S1CINTERTANKARMPROC", 19)) {
			sscan_state(line + 19, s1cintertankarmState);
		} else if (!strnicmp (line, "S1CFORWARDARMPROC", 17)) {
			sscan_state(line + 17, s1cforwardarmState);
		} else if (!strnicmp (line, "SWINGARMPROC", 12)) {
			sscan_state(line + 12, swingarmState);
		} else if (!strnicmp (line, "MASTPROC", 8)) {
			sscan_state(line + 8, mastState);
		} else if (!strnicmp(line, "MASTCOVERSPROC", 14)) {
			sscan_state(line + 14, mastcoversState);
		} else if (!strnicmp (line, "LVNAME", 6)) {
			strncpy (LVName, line + 7, 64);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
	SetTouchdownPointHeight(touchdownPointHeight);
}

void ML::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	oapiWriteScenario_int(scn, "STATE", state);
	papiWriteScenario_bool(scn, "HOLD", Hold);
	papiWriteScenario_bool(scn, "COMMIT", bCommit);
	papiWriteScenario_double(scn, "TOUCHDOWNPOINTHEIGHT", touchdownPointHeight);
	papiWriteScenario_double(scn, "CRANEPROC", craneProc);
	papiWriteScenario_double(scn, "CMARMPROC", cmarmProc);
	WriteScenario_state(scn, "S2AFTARMPROC", s2aftarmState);
	WriteScenario_state(scn, "DAMPERARMPROC", damperarmState);
	WriteScenario_state(scn, "S1CINTERTANKARMPROC", s1cintertankarmState);
	WriteScenario_state(scn, "S1CFORWARDARMPROC", s1cforwardarmState);
	WriteScenario_state(scn, "SWINGARMPROC", swingarmState);
	WriteScenario_state(scn, "MASTPROC", mastState);
	WriteScenario_state(scn, "MASTCOVERSPROC", mastcoversState);
	if (LVName[0])
		oapiWriteScenario_string(scn, "LVNAME", LVName);
}

int ML::clbkConsumeDirectKey(char *kstate) {

	if (KEYMOD_SHIFT(kstate)) {
		return 0; 
	}

/*	ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 0); 
	VECTOR3 pos, dir, rot;
	GetAttachmentParams (ah, pos, dir, rot);

	double step = 0.1;
	if (KEYMOD_CONTROL(kstate))
		step = 0.01;

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
	double moveStep = 1.0e-8;
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

int ML::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0;
	}
	return 0;
}

bool ML::CutoffInterlock()
{
	return (IuUmb->IsEDSUnsafeA() || IuUmb->IsEDSUnsafeB() || TSMUmb->SIStageLogicCutoff());
}

bool ML::Commit()
{
	if (!sat) return false;
	return IuUmb->AllSIEnginesRunning() && !CutoffInterlock();
}

bool ML::ESEGetCommandVehicleLiftoffIndicationInhibit()
{
	return IuESE->GetCommandVehicleLiftoffIndicationInhibit();
}

bool ML::ESEGetSICOutboardEnginesCantInhibit()
{
	return IuESE->GetSICOutboardEnginesCantInhibit();
}

bool ML::ESEGetSICOutboardEnginesCantSimulate()
{
	return IuESE->GetSICOutboardEnginesCantSimulate();
}

bool ML::ESEGetExcessiveRollRateAutoAbortInhibit(int n)
{
	return IuESE->GetExcessiveRollRateAutoAbortInhibit(n);
}

bool ML::ESEGetExcessivePitchYawRateAutoAbortInhibit(int n)
{
	return IuESE->GetExcessivePitchYawRateAutoAbortInhibit(n);
}

bool ML::ESEGetTwoEngineOutAutoAbortInhibit(int n)
{
	return IuESE->GetTwoEngineOutAutoAbortInhibit(n);
}

bool ML::ESEGetGSEOverrateSimulate(int n)
{
	return IuESE->GetOverrateSimulate(n);
}

bool ML::ESEGetEDSPowerInhibit()
{
	return IuESE->GetEDSPowerInhibit();
}

bool ML::ESEPadAbortRequest()
{
	return IuESE->GetEDSPadAbortRequest();
}

bool ML::ESEGetThrustOKIndicateEnableInhibitA()
{
	return IuESE->GetThrustOKIndicateEnableInhibitA();
}

bool ML::ESEGetThrustOKIndicateEnableInhibitB()
{
	return IuESE->GetThrustOKIndicateEnableInhibitB();
}

bool ML::ESEEDSLiftoffInhibitA()
{
	return IuESE->GetEDSLiftoffInhibitA();
}

bool ML::ESEEDSLiftoffInhibitB()
{
	return IuESE->GetEDSLiftoffInhibitB();
}

bool ML::ESEGetSIBurnModeSubstitute()
{
	return IuESE->GetSIBurnModeSubstitute();
}

bool ML::ESEGetGuidanceReferenceRelease()
{
	return IuESE->GetGuidanceReferenceRelease();
}

bool ML::ESEGetQBallSimulateCmd()
{
	return IuESE->GetQBallSimulateCmd();
}

bool ML::ESEGetEDSAutoAbortSimulate(int n)
{
	return IuESE->GetEDSAutoAbortSimulate(n);
}

bool ML::ESEGetEDSLVCutoffSimulate(int n)
{
	return IuESE->GetEDSLVCutoffSimulate(n);
}

bool ML::ESEGetSICThrustOKSimulate(int eng, int n)
{
	return SICESE->GetSICThrustOKSimulate(eng, n);
}

void ML::MobileLauncherComputer(int mdo, bool on)
{
	switch (mdo)
	{
	case 84: //EDV SIC INTK ARM EXTEND
		break;
	case 202: //EDV COM MOD RETRACT PARK
		break;
	case 251: //EDV COM MOD AUTO ARM RETRACT
		break;
	case 252: //EDV COM MOD AUTO ARM EXTEND
		break;
	case 737:
		//EDS SIMULATE LIFTOFF A
		if (on)
		{
			IuESE->SetEDSLiftoffInhibitA(false);
			IuUmb->SetEDSLiftoffEnableA();
		}
		else
		{
			IuESE->SetEDSLiftoffInhibitA(true);
			IuUmb->EDSLiftoffEnableReset();
		}
		break;
	case 738:
		//EDS SIMULATE LIFTOFF B
		if (on)
		{
			IuESE->SetEDSLiftoffInhibitB(false);
			IuUmb->SetEDSLiftoffEnableB();
		}
		else
		{
			IuESE->SetEDSLiftoffInhibitB(true);
			IuUmb->EDSLiftoffEnableReset();
		}
		break;
	case 799:
	case 800:
	case 801:
		//EDS COMD PWR FAIL SIM
		IuESE->SetEDSPowerInhibit(on);
		break;
	case 802:
		//EDS CUTOFF CMD NO 1 FROM S/C
		IuESE->SetEDSCutoffFromSC(1, on);
		break;
	case 803:
		//EDS CUTOFF CMD NO 2 FROM S/C
		IuESE->SetEDSCutoffFromSC(2, on);
		break;
	case 804:
		//EDS CUTOFF CMD NO 3 FROM S/C
		IuESE->SetEDSCutoffFromSC(3, on);
		break;
	case 1584: //IVB MAIN STAGE OK SIM ON
		break;
	case 1903: //IU EDS RG SYSTEM POWER ON
		IuUmb->SetControlSignalProcessorPower(on);
		break;
	}
}

void ML::TerminalCountdownSequencer(double MissionTime)
{
	switch (TCSSequence)
	{
	case 0:
		if (MissionTime >= -187.0)
		{
			//S-II End Turbine Start Bottle Pressurization (GH2)
			//S-II Bleed Turbine Start Bottle Press. Umbilical Line
			//S-II End Ground Prepressurization Line Purge (GHe)
			//S-II Begin LOX Tank Prepressurization (GHe)
			TCSSequence++;
		}
		break;
	case 1:
		if (MissionTime >= -182.0)
		{
			//S-II Begin LOX Tank Prepressurization + 5 Seconds
			TCSSequence++;
		}
		break;
	case 2:
		if (MissionTime >= -172.0)
		{
			//S-II End LOX Replenish
			//S-II Begin LOX Tank Fill Line Drain and Purge
			TCSSequence++;
		}
		break;
	case 3:
		if (MissionTime >= -167.0)
		{
			//S-IVB Begin LOX Tank Prepressurization
			TCSSequence++;
		}
		break;
	case 4:
		if (MissionTime >= -152.0)
		{
			//S-IVB End LOX Replenish
			//S-IVB Begin LOX Tank Fill Line Drain and Purge
			TCSSequence++;
		}
		break;
	case 5:
		if (MissionTime >= -102.0)
		{
			//S-IC Bypass Engine Hydraulic Interlocks
			TCSSequence++;
		}
		break;
	case 6:
		if (MissionTime >= -97.0)
		{
			//S-IC Begin Fuel Tank Prepressurization (GHe)
			//S-II End LH2 Tank Pressurization System Purge (80 PSI)
			//S-II Begin LH2 Tank Prepressurization (GHe)
			//S-IVB Begin GH2 Vent Line Purge
			//S-IVB Begin LH2 Tank Prepressurization
			TCSSequence++;
		}
		break;
	case 7:
		if (MissionTime >= -96.0)
		{
			//S-II End LH2 Tank Pressurization System Purge (80 PSI) + 1 Second
			TCSSequence++;
		}
		break;
	case 8:
		if (MissionTime >= -92.0)
		{
			//S-II Begin LH2 Tank Prepressurization (GHe) + 5 Seconds
			TCSSequence++;
		}
		break;
	case 9:
		if (MissionTime >= -82.0)
		{
			//S-II End LH2 Replenish
			//S-II Begin LH2 Tank Fill Line Drain and Purge
			//S-IVB End LH2 Replenish
			//S-IVB Begin LH2 Tank Fill Line Drain and Purge
			TCSSequence++;
		}
		break;
	case 10:
		if (MissionTime >= -72.0)
		{
			//S-IC Begin LOX Tank Prepressurization (GHe)
			//S-IC End LOX Replenish
			//S-IC End LOX Suction Line Helium Bubbling (Lines 1 + 3)
			//S-IC Begin LOX Fill Line Drain and Purge
			TCSSequence++;
		}
		break;
	case 11:
		if (MissionTime >= -51.0)
		{
			//S-IC Begin Engine Calorimeter Purge (GN2)
			TCSSequence++;
		}
		break;
	case 12:
		if (MissionTime >= -50.0)
		{
			//Veh. Ground Power to Space Vehicle Off
			//Veh. Space Vehicle Internal Power On
			TCSSequence++;
		}
		break;
	case 13:
		if (MissionTime >= -40.0)
		{
			//S-IVB LH2 Vent Directional Control Valve - Ground Pos. Off
			//S-IVB LH2 Vent Directional Control Valve - Flight Pos. On
			TCSSequence++;
		}
		break;
	case 14:
		if (MissionTime >= -30.0)
		{
			//Veh. Begin Retracting S-IC Intertank Service Arm
			//S-IC Fuel and LOX Fill and Drain Valves Disabled
			//S-IC Engine Hydraulic System Flight Activation
			//S-IC End Intertank Umbilical Carrier Disconnect Purge
			//S-IC End Intertank Umbilical Disconnect Pressurization
			//S-II End Engine Helium Bottles Pressurization (GHe)
			//S-II Bleed Engine Helium Bottle Press. Umbilical Line
			//S-II End LOX Tank Prepressurization (GHe)
			//S-II Bleed LOX Tank Pressurization Umbilical Line
			//S-II End LH2 Tank Prepressurization (GHe)
			//S-II Bleed LH2 Tank Pressurization Umbilical Line
			//S-II End Actuation System (Recirc) Pressurization (GHe)
			//S-II Bleed Actuation Sys Pressurization Umbilical
			//S-II Bleed Start Bottle Vent Valve Umbilical Line
			TCSSequence++;
		}
		break;
	case 15:
		if (MissionTime >= -29.0)
		{
			//S-II End LH2 Tank Prepressurization (GHe) + 1 Second
			//S-II End LOX Tank Prepressurization (GHe) + 1 Second
			TCSSequence++;
		}
		break;
	case 16:
		if (MissionTime >= -22.0)
		{
			//Guidance Reference Release Alert
			TCSSequence++;
		}
		break;
	case 17:
		if (MissionTime >= -20.0)
		{
			//S-IC LOX Interconnect Valves Closed
			TCSSequence++;
		}
		break;
	case 18:
		if (MissionTime >= -17.0)
		{
			//Veh. S-IC Intertank Service Arm Retracted and Locked
			//Veh. Range Safety Command Receiver On Internal Power
			//IU Guidance Reference Release
			TCSSequence++;
		}
		break;
	case 19:
		if (MissionTime >= -16.2)
		{
			//Veh. Begin Retracting S-IC Forward Service Arm
			//S-IC End Forward Umbilical Disconnect Pressurization
			//S-IC End Electrical Housing Disconnect Purge
			//S-IC End Fwd Compartment Instr. Cont. Conditioning
			//S-IC Bleed LOX Suction Line Bubbling Umbilical 
			//Veh. S-IC Foward Service Arm Retracted and Locked
			TCSSequence++;
		}
		break;
	case 20:
		if (MissionTime >= -15.0)
		{
			//S-IC Bleed LOX Suction Line Bubbling Umbilical 
			TCSSequence++;
		}
		break;
	case 21:
		if (MissionTime >= -10.2)
		{
			//Veh. S-IC Foward Service Arm Retracted and Locked
			TCSSequence++;
		}
		break;
	}
}

void ML::SLCCCheckDiscreteInput(RCA110A *c)
{
	c->SetInput(646, IuUmb->GetEDSAutoAbortBus());
	c->SetInput(647, IuUmb->IsEDSUnsafeA());
	c->SetInput(648, IuUmb->IsEDSUnsafeB());
	c->SetInput(861, IuESE->GetFCCPowerIsOn());
}

bool ML::SLCCGetOutputSignal(size_t n)
{
	return rca110a->GetOutputSignal(n);
}

void ML::ConnectGroundComputer(RCA110A *c)
{
	rca110a->Connect(c);
}

void ML::IssueSwitchSelectorCmd(int stage, int chan)
{
	IuUmb->SwitchSelector(stage, chan);
}