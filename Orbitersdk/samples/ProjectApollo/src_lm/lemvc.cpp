/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: LEM virtual cockpit code

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
#include "LEMcomputer.h"
#include "LM_VC_Resource.h"
#include "Mission.h"

#include "LEM.h"

void LEM::JostleViewpoint(double amount)

{
	double j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetx += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsety += j;

	j = ((double)((rand() & 65535) - 32768) * amount) / 3276800.0;
	ViewOffsetz += j;

	if (ViewOffsetx > 0.10)
		ViewOffsetx = 0.10;
	if (ViewOffsetx < -0.10)
		ViewOffsetx = -0.10;

	if (ViewOffsety > 0.10)
		ViewOffsety = 0.10;
	if (ViewOffsety < -0.10)
		ViewOffsety = -0.10;

	if (ViewOffsetz > 0.05)
		ViewOffsetz = 0.05;
	if (ViewOffsetz < -0.05)
		ViewOffsetz = -0.05;

	SetView();
}

void LEM::SetView() {

	VECTOR3 v;
	//
	// Set camera offset
	//
	if (InVC) {
		switch (viewpos) {
		case LMVIEW_CDR:
			if (stage == 2) {
				v = _V(-0.45, -0.07, 1.25);
			}
			else {
				v = _V(-0.45, 1.68, 1.25);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;

		case LMVIEW_LMP:
			if (stage == 2) {
				v = _V(0.45, -0.07, 1.25);
			}
			else {
				v = _V(0.45, 1.68, 1.25);
			}
			SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
			break;

		case LMVIEW_LPD:
			if (stage == 2) {
				v = _V(-0.58, -0.15, 1.40);
			}
			else {
				v = _V(-0.58, 1.60, 1.40);
			}
			SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE * RAD), cos(VIEWANGLE * RAD)));
			break;
		}

		v.x += ViewOffsetx;
		v.y += ViewOffsety;
		v.z += ViewOffsetz;

	} else {

		if(InPanel)
		{
			switch(PanelId)
			{
				case LMPANEL_MAIN:
					if (stage == 2) {
						v =_V(0, 0.15, 1.26);
					}
					else {
						v =_V(0, 1.90, 1.26);
					}
					break;
				case LMPANEL_RIGHTWINDOW:
					if (stage == 2) {
						v =_V(0.576, 0.15, 1.26);
					}
					else {
						v =_V(0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_LEFTWINDOW:
					if (stage == 2) {
						v =_V(-0.576, 0.15, 1.26);
					}
					else {
						v =_V(-0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_LPDWINDOW:
					if (stage == 2) {
						v = _V(-0.58, -0.15, 1.40);
					}
					else {
						v = _V(-0.58, 1.60, 1.40);
					}
					break;
				case LMPANEL_RNDZWINDOW:
					if (stage == 2) {
						v =_V(-0.598, 0.15, 1.106);
					}
					else {
						v =_V(-0.598, 1.90, 1.106);
					}
					break;
				case LMPANEL_LEFTPANEL:
					if (stage == 2) {
						v =_V(-0.576, 0.15, 1.26);
					}
					else {
						v =_V(-0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_AOTVIEW:
					if (stage == 2) {
						v =_V(0, 1.13, 1.26);
					}
					else {
						v =_V(0, 2.88, 1.26);
					}
					break;
				case LMPANEL_AOTZOOM:
					if (stage == 2) {
						v =_V(0, 1.13, 1.26);
					}
					else {
						v =_V(0, 2.88, 1.26);
					}
					break;
				case LMPANEL_DOCKVIEW:
					if (stage == 2) {
						v =_V(-0.598, 0.15, 1.106);
					}
					else {
						v =_V(-0.598, 1.90, 1.106);
					}
					break;
				
				case LMPANEL_LEFTZOOM:
					if (stage == 2) {
						v =_V(-0.576, 0.15, 1.26);
					}
					else {
						v =_V(-0.576, 1.90, 1.26);
					}
					break;
				case LMPANEL_UPPERHATCH:
					if (stage == 2) {
						v =_V(0, -0.55, 0);
					}
					else {
						v =_V(0, 1.20, 0);
					}
					SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
					oapiCameraSetCockpitDir(180 * RAD, 0);
					break;
				case LMPANEL_FWDHATCH:
					if (stage == 2) {
						v =_V(0, -1.4, 1.5);
					}
					else {
						v =_V(0, 0.35, 1.5);
					}
					break;
			}
		}
		else
		{
			v =_V(0, 0, 0);
		}
	}

	SetCameraOffset(v - currentCoG);

	//
	// Change FOV for the LPD window and AOT zoom
	//
	if (InPanel && PanelId == LMPANEL_LPDWINDOW) {
	   // if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1080.0, 1.0)));
	}
	else if (InPanel && PanelId == LMPANEL_AOTZOOM) {
		// if this is the first time we've been here, save the current FOV
		if (InFOV) {
			SaveFOV = oapiCameraAperture();
			InFOV = false;
		}
		//set FOV to 60 degrees (except for lower resolutions)
		DWORD w, h;
		oapiGetViewportSize(&w, &h);
		oapiCameraSetAperture(atan(tan(RAD*30.0)*min(h / 1050.0, 1.0)));
	}
    else {
		if(InFOV == false) {
			oapiCameraSetAperture(SaveFOV);
			InFOV = true;
		}
	}
}

bool LEM::clbkLoadVC (int id)
{
	RegisterActiveAreas();

	switch (id) {
	case LMVIEW_CDR:
		viewpos = LMVIEW_CDR;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(-0.1, 0.0, 0.1), 0, 0, _V(-0.1, -0.15, 0.0), 90.0 * RAD, 0, _V(0.1, 0.0, 0.0), 0, 0);
		oapiVCSetNeighbours(-1, LMVIEW_LMP, -1, LMVIEW_LPD);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		return true;

	case LMVIEW_LMP:
		viewpos = LMVIEW_LMP;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.1, 0.0, 0.1), 0, 0, _V(-0.1, 0.0, 0.0), 0, 0, _V(0.1, -0.15, 0.0), -90.0 * RAD, 0);
		oapiVCSetNeighbours(LMVIEW_CDR, -1, -1, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		return true;

	case LMVIEW_LPD:
		viewpos = LMVIEW_LPD;
		SetCameraRotationRange(0.8 * PI, 0.8 * PI, 0.4 * PI, 0.4 * PI);
		SetCameraMovement(_V(0.13, 0.0, 0.0), 0, 15 * RAD, _V(0.0, 0.0, 0.0), 0, 0, _V(0.58, -0.4, -0.1), 0, -10 * RAD);
		oapiCameraSetAperture(30 * RAD);
		oapiVCSetNeighbours(-1, -1, LMVIEW_CDR, -1);
		InVC = true;
		InPanel = false;
		SetView();
		SetLMMeshVis();

		return true;

	default:
		return false;
	}
}

void LEM::RegisterActiveAreas()
{
	int i = 0;

	VECTOR3 ofs;
	if (stage > 1) { ofs = _V(0.00, -0.76, 0.00); }
	else { ofs = _V(0.00, 0.99, 0.00); }

	//Apply center of gravity offset here
	ofs -= currentCoG;

	//
	// Release all surfaces
	//
	ReleaseSurfacesVC();

	MainPanelVC.ClearSwitches();

	SURFHANDLE MainPanelTex1 = oapiGetTextureHandle(hLMVC, 2);
	SURFHANDLE MainPanelTex2 = oapiGetTextureHandle(hLMVC, 8);

	// Panel 1
	for (i = 0; i < P1_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P1_01 + i, P1_TOGGLE_POS[i] + P1_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P1_01 + i, P1_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_LM_CWS_LEFT, _R(238, 27, 559, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MISSION_CLOCK, _R(60, 259, 202, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_EVENT_TIMER, _R(276, 259, 357, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RANGE_TAPE, _R(431, 633, 475, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RATE_TAPE, _R(482, 633, 517, 796), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LEM_MA_LEFT, _R(30, 593, 77, 636), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_LEFT, _V(-0.415919, 0.599307, 1.70252) + ofs, 0.008);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_LEFT, _R(341, 891, 377, 1098), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_REG_CONTROLS_RIGHT, _R(415, 891, 451, 1098), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_OXID_QUANTITY_INDICATOR, _R(445, 218, 484, 239), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_FUEL_QUANTITY_INDICATOR, _R(445, 270, 484, 292), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MPS_HELIUM_PRESS_INDICATOR, _R(577, 259, 658, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	oapiVCRegisterArea(AID_VC_XPOINTERCDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_LEFT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_CONTACTLIGHT1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PWRFAIL_LIGHTS_P1, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL1_NEEDLES, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_THRUST_WEIGHT_IND, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	oapiVCRegisterArea(AID_VC_ABORT_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ABORT_BUTTON, _V(-0.10018, 0.436067, 1.68518) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_ABORTSTAGE_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_ABORTSTAGE_BUTTON, _V(-0.047192, 0.437682, 1.68536) + ofs, 0.01);

	MainPanelVC.AddSwitch(&RateErrorMonSwitch, AID_VC_SWITCH_P1_01, &anim_P1switch[0]);
	MainPanelVC.AddSwitch(&AttitudeMonSwitch, AID_VC_SWITCH_P1_02, &anim_P1switch[1]);
	MainPanelVC.AddSwitch(&LeftXPointerSwitch, AID_VC_SWITCH_P1_03, &anim_P1switch[2]);
	MainPanelVC.AddSwitch(&GuidContSwitch, AID_VC_SWITCH_P1_04, &anim_P1switch[3]);
	MainPanelVC.AddSwitch(&ModeSelSwitch, AID_VC_SWITCH_P1_05, &anim_P1switch[4]);
	MainPanelVC.AddSwitch(&AltRngMonSwitch, AID_VC_SWITCH_P1_06, &anim_P1switch[5]);
	MainPanelVC.AddSwitch(&ShiftTruSwitch, AID_VC_SWITCH_P1_07, &anim_P1switch[6]);
	MainPanelVC.AddSwitch(&RateScaleSwitch, AID_VC_SWITCH_P1_08, &anim_P1switch[7]);
	MainPanelVC.AddSwitch(&ACAPropSwitch, AID_VC_SWITCH_P1_09, &anim_P1switch[8]);
	MainPanelVC.AddSwitch(&THRContSwitch, AID_VC_SWITCH_P1_10, &anim_P1switch[9]);
	MainPanelVC.AddSwitch(&MANThrotSwitch, AID_VC_SWITCH_P1_11, &anim_P1switch[10]);
	MainPanelVC.AddSwitch(&EngineArmSwitch, AID_VC_SWITCH_P1_12, &anim_P1switch[11]);
	MainPanelVC.AddSwitch(&ATTTranslSwitch, AID_VC_SWITCH_P1_13, &anim_P1switch[12]);
	MainPanelVC.AddSwitch(&BALCPLSwitch, AID_VC_SWITCH_P1_14, &anim_P1switch[13]);
	MainPanelVC.AddSwitch(&ASCHeReg1Switch, AID_VC_SWITCH_P1_15, &anim_P1switch[14]);
	MainPanelVC.AddSwitch(&ASCHeReg2Switch, AID_VC_SWITCH_P1_16, &anim_P1switch[15]);
	MainPanelVC.AddSwitch(&DESHeReg1Switch, AID_VC_SWITCH_P1_17, &anim_P1switch[16]);
	MainPanelVC.AddSwitch(&DESHeReg2Switch, AID_VC_SWITCH_P1_18, &anim_P1switch[17]);
	MainPanelVC.AddSwitch(&QTYMonSwitch, AID_VC_SWITCH_P1_19, &anim_P1switch[18]);
	MainPanelVC.AddSwitch(&TempPressMonSwitch, AID_VC_SWITCH_P1_20, &anim_P1switch[19]);
	MainPanelVC.AddSwitch(&HeliumMonRotary, AID_VC_ROT_P1_01, &anim_P1_Rot[0]);

	// Panel 2
	for (i = 0; i < P2_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P2_01 + i, P2_TOGGLE_POS[i] + P2_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P2_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P2_01 + i, P2_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_LM_CWS_RIGHT, _R(1075, 27, 1375, 153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_ASC_FEED_TALKBACKS, _R(794, 413, 1031, 436), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_14_TALKBACKS, _R(794, 562, 1031, 585), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LGC_CMD_ENABLE_23_TALKBACKS, _R(794, 688, 1031, 711), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_RCS_XFEED_TALKBACK, _R(795, 844, 818, 867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_MAIN_SOV_TALKBACKS, _R(934, 844, 1027, 867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LEM_MA_RIGHT, _R(1384, 593, 1431, 636), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCSetAreaClickmode_Spherical(AID_VC_LEM_MA_RIGHT, _V(0.414751, 0.59891, 1.70247) + ofs, 0.008);

	oapiVCRegisterArea(AID_VC_PANEL2_COMPLIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PWRFAIL_LIGHTS_P2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_XPOINTERLMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_FDAI_RIGHT, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL2_NEEDLES, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);

	MainPanelVC.AddSwitch(&RCSAscFeed1ASwitch, AID_VC_SWITCH_P2_01, &anim_P2switch[0]);
	MainPanelVC.AddSwitch(&RCSAscFeed2ASwitch, AID_VC_SWITCH_P2_02, &anim_P2switch[1]);
	MainPanelVC.AddSwitch(&RCSAscFeed1BSwitch, AID_VC_SWITCH_P2_03, &anim_P2switch[2]);
	MainPanelVC.AddSwitch(&RCSAscFeed2BSwitch, AID_VC_SWITCH_P2_04, &anim_P2switch[3]);
	MainPanelVC.AddSwitch(&RCSQuad1ACmdEnableSwitch, AID_VC_SWITCH_P2_05, &anim_P2switch[4]);
	MainPanelVC.AddSwitch(&RCSQuad4ACmdEnableSwitch, AID_VC_SWITCH_P2_06, &anim_P2switch[5]);
	MainPanelVC.AddSwitch(&RCSQuad1BCmdEnableSwitch, AID_VC_SWITCH_P2_07, &anim_P2switch[6]);
	MainPanelVC.AddSwitch(&RCSQuad4BCmdEnableSwitch, AID_VC_SWITCH_P2_08, &anim_P2switch[7]);
	MainPanelVC.AddSwitch(&RCSQuad2ACmdEnableSwitch, AID_VC_SWITCH_P2_09, &anim_P2switch[8]);
	MainPanelVC.AddSwitch(&RCSQuad3ACmdEnableSwitch, AID_VC_SWITCH_P2_10, &anim_P2switch[9]);
	MainPanelVC.AddSwitch(&RCSQuad2BCmdEnableSwitch, AID_VC_SWITCH_P2_11, &anim_P2switch[10]);
	MainPanelVC.AddSwitch(&RCSQuad3BCmdEnableSwitch, AID_VC_SWITCH_P2_12, &anim_P2switch[11]);
	MainPanelVC.AddSwitch(&RCSXFeedSwitch, AID_VC_SWITCH_P2_13, &anim_P2switch[12]);
	MainPanelVC.AddSwitch(&RCSMainSovASwitch, AID_VC_SWITCH_P2_14, &anim_P2switch[13]);
	MainPanelVC.AddSwitch(&RCSMainSovBSwitch, AID_VC_SWITCH_P2_15, &anim_P2switch[14]);
	MainPanelVC.AddSwitch(&RightACAPropSwitch, AID_VC_SWITCH_P2_16, &anim_P2switch[15]);
	MainPanelVC.AddSwitch(&RightRateErrorMonSwitch, AID_VC_SWITCH_P2_17, &anim_P2switch[16]);
	MainPanelVC.AddSwitch(&RightAttitudeMonSwitch, AID_VC_SWITCH_P2_18, &anim_P2switch[17]);
	MainPanelVC.AddSwitch(&TempPressMonRotary, AID_VC_ROT_P2_01, &anim_P2_Rot[0]);
	MainPanelVC.AddSwitch(&GlycolRotary, AID_VC_ROT_P2_02, &anim_P2_Rot[1]);
	MainPanelVC.AddSwitch(&SuitFanRotary, AID_VC_ROT_P2_03, &anim_P2_Rot[2]);
	MainPanelVC.AddSwitch(&QtyMonRotary, AID_VC_ROT_P2_04, &anim_P2_Rot[3]);

	// Panel 3
	for (i = 0; i < P3_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P3_01 + i, P3_TOGGLE_POS[i] + P3_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P3_01 + i, P3_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_RDR_SIG_STR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_RR_NOTRACK, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_CONTACTLIGHT2, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_RR_SLEW_SWITCH, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCRegisterArea(AID_VC_PANEL3_NEEDLES, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCSetAreaClickmode_Quadrilateral(AID_VC_RR_SLEW_SWITCH, _V(-0.27271, 0.157539, 1.57055) + ofs, _V(-0.255647, 0.157539, 1.57055) + ofs, _V(-0.27271, 0.143561, 1.56076) + ofs, _V(-0.255647, 0.143561, 1.56076) + ofs);

	MainPanelVC.AddSwitch(&EngGimbalEnableSwitch, AID_VC_SWITCH_P3_01, &anim_P3switch[0]);
	MainPanelVC.AddSwitch(&EngineDescentCommandOverrideSwitch, AID_VC_SWITCH_P3_02, &anim_P3switch[1]);
	MainPanelVC.AddSwitch(&LandingAntSwitch, AID_VC_SWITCH_P3_03, &anim_P3switch[2]);
	MainPanelVC.AddSwitch(&RadarTestSwitch, AID_VC_SWITCH_P3_04, &anim_P3switch[3]);
	MainPanelVC.AddSwitch(&SlewRateSwitch, AID_VC_SWITCH_P3_05, &anim_P3switch[4]);
	MainPanelVC.AddSwitch(&DeadBandSwitch, AID_VC_SWITCH_P3_06, &anim_P3switch[5]);
	MainPanelVC.AddSwitch(&GyroTestLeftSwitch, AID_VC_SWITCH_P3_07, &anim_P3switch[6]);
	MainPanelVC.AddSwitch(&GyroTestRightSwitch, AID_VC_SWITCH_P3_08, &anim_P3switch[7]);
	MainPanelVC.AddSwitch(&RollSwitch, AID_VC_SWITCH_P3_09, &anim_P3switch[8]);
	MainPanelVC.AddSwitch(&PitchSwitch, AID_VC_SWITCH_P3_10, &anim_P3switch[9]);
	MainPanelVC.AddSwitch(&YawSwitch, AID_VC_SWITCH_P3_11, &anim_P3switch[10]);
	MainPanelVC.AddSwitch(&ModeControlPGNSSwitch, AID_VC_SWITCH_P3_12, &anim_P3switch[11]);
	MainPanelVC.AddSwitch(&ModeControlAGSSwitch, AID_VC_SWITCH_P3_13, &anim_P3switch[12]);
	MainPanelVC.AddSwitch(&IMUCageSwitch, AID_VC_SWITCH_P3_14, &anim_P3switch[13]);
	MainPanelVC.AddSwitch(&EventTimerCtlSwitch, AID_VC_SWITCH_P3_15, &anim_P3switch[14]);
	MainPanelVC.AddSwitch(&EventTimerStartSwitch, AID_VC_SWITCH_P3_16, &anim_P3switch[15]);
	MainPanelVC.AddSwitch(&EventTimerMinuteSwitch, AID_VC_SWITCH_P3_17, &anim_P3switch[16]);
	MainPanelVC.AddSwitch(&EventTimerSecondSwitch, AID_VC_SWITCH_P3_18, &anim_P3switch[17]);
	MainPanelVC.AddSwitch(&RCSSysQuad1Switch, AID_VC_SWITCH_P3_19, &anim_P3switch[18]);
	MainPanelVC.AddSwitch(&RCSSysQuad4Switch, AID_VC_SWITCH_P3_20, &anim_P3switch[19]);
	MainPanelVC.AddSwitch(&RCSSysQuad2Switch, AID_VC_SWITCH_P3_21, &anim_P3switch[20]);
	MainPanelVC.AddSwitch(&RCSSysQuad3Switch, AID_VC_SWITCH_P3_22, &anim_P3switch[21]);
	MainPanelVC.AddSwitch(&SidePanelsSwitch, AID_VC_SWITCH_P3_23, &anim_P3switch[22]);
	MainPanelVC.AddSwitch(&FloodSwitch, AID_VC_SWITCH_P3_24, &anim_P3switch[23]);
	MainPanelVC.AddSwitch(&RightXPointerSwitch, AID_VC_SWITCH_P3_25, &anim_P3switch[24]);
	MainPanelVC.AddSwitch(&ExteriorLTGSwitch, AID_VC_SWITCH_P3_26, &anim_P3switch[25]);
	MainPanelVC.AddSwitch(&TestMonitorRotary, AID_VC_ROT_P3_01, &anim_P3_Rot[0]);
	MainPanelVC.AddSwitch(&RendezvousRadarRotary, AID_VC_ROT_P3_02, &anim_P3_Rot[1]);
	MainPanelVC.AddSwitch(&TempMonitorRotary, AID_VC_ROT_P3_03, &anim_P3_Rot[2]);
	MainPanelVC.AddSwitch(&LampToneTestRotary, AID_VC_ROT_P3_04, &anim_P3_Rot[3]);
	MainPanelVC.AddSwitch(&FloodRotary, AID_VC_ROT_P3_05, &anim_P3_Rot[4]);

	// Panel 4
	for (i = 0; i < P4_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P4_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P4_01 + i, P4_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P4_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P4_01 + i, P4_TOGGLE_POS[i] + P4_CLICK + ofs, 0.006);
	}

	oapiVCRegisterArea(AID_VC_DSKY_DISPLAY, _R(309, 1520, 414, 1696), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_DSKY_LIGHTS,  _R(165, 1525, 267, 1694), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	MainPanelVC.AddSwitch(&DskySwitchVerb, AID_VC_PUSHB_P4_01, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNoun, AID_VC_PUSHB_P4_02, NULL);
	MainPanelVC.AddSwitch(&DskySwitchPlus, AID_VC_PUSHB_P4_03, NULL);
	MainPanelVC.AddSwitch(&DskySwitchMinus, AID_VC_PUSHB_P4_04, NULL);
	MainPanelVC.AddSwitch(&DskySwitchZero, AID_VC_PUSHB_P4_05, NULL);
	MainPanelVC.AddSwitch(&DskySwitchOne, AID_VC_PUSHB_P4_06, NULL);
	MainPanelVC.AddSwitch(&DskySwitchTwo, AID_VC_PUSHB_P4_07, NULL);
	MainPanelVC.AddSwitch(&DskySwitchThree, AID_VC_PUSHB_P4_08, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFour, AID_VC_PUSHB_P4_09, NULL);
	MainPanelVC.AddSwitch(&DskySwitchFive, AID_VC_PUSHB_P4_10, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSix, AID_VC_PUSHB_P4_11, NULL);
	MainPanelVC.AddSwitch(&DskySwitchSeven, AID_VC_PUSHB_P4_12, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEight, AID_VC_PUSHB_P4_13, NULL);
	MainPanelVC.AddSwitch(&DskySwitchNine, AID_VC_PUSHB_P4_14, NULL);
	MainPanelVC.AddSwitch(&DskySwitchClear, AID_VC_PUSHB_P4_15, NULL);
	MainPanelVC.AddSwitch(&DskySwitchProg, AID_VC_PUSHB_P4_16, NULL);
	MainPanelVC.AddSwitch(&DskySwitchKeyRel, AID_VC_PUSHB_P4_17, NULL);
	MainPanelVC.AddSwitch(&DskySwitchEnter, AID_VC_PUSHB_P4_18, NULL);
	MainPanelVC.AddSwitch(&DskySwitchReset, AID_VC_PUSHB_P4_19, NULL);
	MainPanelVC.AddSwitch(&LeftACA4JetSwitch, AID_VC_SWITCH_P4_01, &anim_P4switch[0]);
	MainPanelVC.AddSwitch(&LeftTTCATranslSwitch, AID_VC_SWITCH_P4_02, &anim_P4switch[1]);
	MainPanelVC.AddSwitch(&RightACA4JetSwitch, AID_VC_SWITCH_P4_03, &anim_P4switch[2]);
	MainPanelVC.AddSwitch(&RightTTCATranslSwitch, AID_VC_SWITCH_P4_04, &anim_P4switch[3]);

	// Panel 5

	for (i = 0; i < P5_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P5_01 + i, P5_TOGGLE_POS[i] + P5_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P5_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P5_01 + i, P5_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_START_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_START_BUTTON, _V(-0.644677, 0.073766, 1.48271) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_STOP_BUTTON_CDR, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STOP_BUTTON_CDR, _V(-0.694894, 0.085711, 1.53811) + ofs, 0.01);
	oapiVCRegisterArea(AID_VC_PLUSX_BUTTON, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_PLUSX_BUTTON, _V(-0.65603, -0.017321, 1.41484) + ofs, 0.01);

	MainPanelVC.AddSwitch(&TimerContSwitch, AID_VC_SWITCH_P5_01, &anim_P5switch[0]);
	MainPanelVC.AddSwitch(&TimerSlewHours, AID_VC_SWITCH_P5_02, &anim_P5switch[1]);
	MainPanelVC.AddSwitch(&TimerSlewMinutes, AID_VC_SWITCH_P5_03, &anim_P5switch[2]);
	MainPanelVC.AddSwitch(&TimerSlewSeconds, AID_VC_SWITCH_P5_04, &anim_P5switch[3]);
	MainPanelVC.AddSwitch(&LtgORideAnunSwitch, AID_VC_SWITCH_P5_05, &anim_P5switch[4]);
	MainPanelVC.AddSwitch(&LtgORideNumSwitch, AID_VC_SWITCH_P5_06, &anim_P5switch[5]);
	MainPanelVC.AddSwitch(&LtgORideIntegralSwitch, AID_VC_SWITCH_P5_07, &anim_P5switch[6]);
	MainPanelVC.AddSwitch(&LtgSidePanelsSwitch, AID_VC_SWITCH_P5_08, &anim_P5switch[7]);

	MainPanelVC.AddSwitch(&LtgFloodOhdFwdKnob, AID_VC_ROT_P5_01, &anim_P5_Rot[0]);
	MainPanelVC.AddSwitch(&LtgAnunNumKnob, AID_VC_ROT_P5_02, &anim_P5_Rot[1]);
	MainPanelVC.AddSwitch(&LtgIntegralKnob, AID_VC_ROT_P5_03, &anim_P5_Rot[2]);

	// Panel 6

	for (i = 0; i < P6_PUSHBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_PUSHB_P6_01 + i, PANEL_REDRAW_NEVER, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_PUSHB_P6_01 + i, P6_PUSHB_POS[i] + ofs, 0.008);
	}

	for (i = 0; i < P6_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P6_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P6_01 + i, P6_TOGGLE_POS[i] + P6_CLICK + ofs, 0.006);
	}

	oapiVCRegisterArea(AID_VC_STOP_BUTTON_LMP, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STOP_BUTTON_LMP, _V(0.421597, 0.077646, 1.50924) + ofs, 0.01);

	oapiVCRegisterArea(AID_VC_LM_DEDA_DISP, _R(1803, 98, 1937, 120), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LM_DEDA_ADR, _R(1827, 53, 1885, 75), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);
	oapiVCRegisterArea(AID_VC_LM_DEDA_LIGHTS, _R(1740, 96, 1787, 122), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex1);

	MainPanelVC.AddSwitch(&DedaSwitchPlus, AID_VC_PUSHB_P6_01, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchMinus, AID_VC_PUSHB_P6_02, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchZero, AID_VC_PUSHB_P6_03, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchOne, AID_VC_PUSHB_P6_04, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchTwo, AID_VC_PUSHB_P6_05, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchThree, AID_VC_PUSHB_P6_06, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchFour, AID_VC_PUSHB_P6_07, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchFive, AID_VC_PUSHB_P6_08, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchSix, AID_VC_PUSHB_P6_09, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchSeven, AID_VC_PUSHB_P6_10, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchEight, AID_VC_PUSHB_P6_11, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchNine, AID_VC_PUSHB_P6_12, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchClear, AID_VC_PUSHB_P6_13, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchReadOut, AID_VC_PUSHB_P6_14, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchEnter, AID_VC_PUSHB_P6_15, NULL);
	MainPanelVC.AddSwitch(&DedaSwitchHold, AID_VC_PUSHB_P6_16, NULL);

	MainPanelVC.AddSwitch(&AGSOperateSwitch, AID_VC_SWITCH_P6_01, &anim_P6switch[0]);

	// Panel 8

	for (i = 0; i < P8_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P8_01 + i, P8_TOGGLE_POS[i] + P8_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P8_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P8_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P8_01 + i, P8_TW_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_SEQ_LIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL8_TALKBACKS, _R(1389, 8, 1772, 149), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	oapiVCRegisterArea(AID_VC_STAGE_SWITCH, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
	oapiVCSetAreaClickmode_Spherical(AID_VC_STAGE_SWITCH, _V(-0.981771, 0.039868, 1.14749) + (P8_CLICK * 2) + ofs, 0.01);

	MainPanelVC.AddSwitch(&EDDesFuelVent, AID_VC_SWITCH_P8_01, &anim_P8switch[0]);
	MainPanelVC.AddSwitch(&EDDesOxidVent, AID_VC_SWITCH_P8_02, &anim_P8switch[1]);
	MainPanelVC.AddSwitch(&EDDesPrpIsol, AID_VC_SWITCH_P8_03, &anim_P8switch[2]);
	MainPanelVC.AddSwitch(&EDMasterArm, AID_VC_SWITCH_P8_04, &anim_P8switch[3]);
	MainPanelVC.AddSwitch(&EDDesVent, AID_VC_SWITCH_P8_05, &anim_P8switch[4]);
	MainPanelVC.AddSwitch(&EDASCHeSel, AID_VC_SWITCH_P8_06, &anim_P8switch[5]);
	MainPanelVC.AddSwitch(&EDLGDeploy, AID_VC_SWITCH_P8_07, &anim_P8switch[6]);
	MainPanelVC.AddSwitch(&EDHePressRCS, AID_VC_SWITCH_P8_08, &anim_P8switch[7]);
	MainPanelVC.AddSwitch(&EDHePressDesStart, AID_VC_SWITCH_P8_09, &anim_P8switch[8]);
	MainPanelVC.AddSwitch(&EDHePressASC, AID_VC_SWITCH_P8_10, &anim_P8switch[9]);
	MainPanelVC.AddSwitch(&EDStageRelay, AID_VC_SWITCH_P8_11, &anim_P8switch[10]);
	MainPanelVC.AddSwitch(&CDRAudSBandSwitch, AID_VC_SWITCH_P8_12, &anim_P8switch[11]);
	MainPanelVC.AddSwitch(&CDRAudICSSwitch, AID_VC_SWITCH_P8_13, &anim_P8switch[12]);
	MainPanelVC.AddSwitch(&CDRAudVHFASwitch, AID_VC_SWITCH_P8_14, &anim_P8switch[13]);
	MainPanelVC.AddSwitch(&CDRAudVHFBSwitch, AID_VC_SWITCH_P8_15, &anim_P8switch[14]);
	MainPanelVC.AddSwitch(&CDRAudRelaySwitch, AID_VC_SWITCH_P8_16, &anim_P8switch[15]);
	MainPanelVC.AddSwitch(&CDRAudVOXSwitch, AID_VC_SWITCH_P8_17, &anim_P8switch[16]);
	MainPanelVC.AddSwitch(&CDRAudioControlSwitch, AID_VC_SWITCH_P8_18, &anim_P8switch[17]);
	MainPanelVC.AddSwitch(&CDRCOASSwitch, AID_VC_SWITCH_P8_19, &anim_P8switch[18]);

	MainPanelVC.AddSwitch(&CDRAudSBandVol, AID_VC_TW_P8_01, &anim_P8thumbwheels[0]);
	MainPanelVC.AddSwitch(&CDRAudICSVol, AID_VC_TW_P8_02, &anim_P8thumbwheels[1]);
	MainPanelVC.AddSwitch(&CDRAudVOXSens, AID_VC_TW_P8_03, &anim_P8thumbwheels[2]);
	MainPanelVC.AddSwitch(&CDRAudVHFAVol, AID_VC_TW_P8_04, &anim_P8thumbwheels[3]);
	MainPanelVC.AddSwitch(&CDRAudVHFBVol, AID_VC_TW_P8_05, &anim_P8thumbwheels[4]);
	MainPanelVC.AddSwitch(&CDRAudMasterVol, AID_VC_TW_P8_06, &anim_P8thumbwheels[5]);

	// Panel 11
	// Row 1
	for (i = 0; i < P11R1_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R1_01 + i, P11R1_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&SE_WND_HTR_AC_CB, AID_VC_CB_P11R1_01, &anim_P11R1cbs[0]);
	MainPanelVC.AddSwitch(&HE_PQGS_PROP_DISP_AC_CB, AID_VC_CB_P11R1_02, &anim_P11R1cbs[1]);
	MainPanelVC.AddSwitch(&SBD_ANT_AC_CB, AID_VC_CB_P11R1_03, &anim_P11R1cbs[2]);
	MainPanelVC.AddSwitch(&ORDEAL_AC_CB, AID_VC_CB_P11R1_04, &anim_P11R1cbs[3]);
	MainPanelVC.AddSwitch(&AGS_AC_CB, AID_VC_CB_P11R1_05, &anim_P11R1cbs[4]);
	MainPanelVC.AddSwitch(&AOT_LAMP_ACB_CB, AID_VC_CB_P11R1_06, &anim_P11R1cbs[5]);
	MainPanelVC.AddSwitch(&LMP_FDAI_AC_CB, AID_VC_CB_P11R1_07, &anim_P11R1cbs[6]);
	MainPanelVC.AddSwitch(&NUM_LTG_AC_CB, AID_VC_CB_P11R1_08, &anim_P11R1cbs[7]);
	MainPanelVC.AddSwitch(&AC_B_INV_1_FEED_CB, AID_VC_CB_P11R1_09, &anim_P11R1cbs[8]);
	MainPanelVC.AddSwitch(&AC_B_INV_2_FEED_CB, AID_VC_CB_P11R1_10, &anim_P11R1cbs[9]);
	MainPanelVC.AddSwitch(&AC_A_INV_1_FEED_CB, AID_VC_CB_P11R1_11, &anim_P11R1cbs[10]);
	MainPanelVC.AddSwitch(&AC_A_INV_2_FEED_CB, AID_VC_CB_P11R1_12, &anim_P11R1cbs[11]);
	MainPanelVC.AddSwitch(&AC_A_BUS_VOLT_CB, AID_VC_CB_P11R1_13, &anim_P11R1cbs[12]);
	MainPanelVC.AddSwitch(&CDR_WND_HTR_AC_CB, AID_VC_CB_P11R1_14, &anim_P11R1cbs[13]);
	MainPanelVC.AddSwitch(&TAPE_RCDR_AC_CB, AID_VC_CB_P11R1_15, &anim_P11R1cbs[14]);
	MainPanelVC.AddSwitch(&AOT_LAMP_ACA_CB, AID_VC_CB_P11R1_16, &anim_P11R1cbs[15]);
	MainPanelVC.AddSwitch(&RDZ_RDR_AC_CB, AID_VC_CB_P11R1_17, &anim_P11R1cbs[16]);
	MainPanelVC.AddSwitch(&DECA_GMBL_AC_CB, AID_VC_CB_P11R1_18, &anim_P11R1cbs[17]);
	MainPanelVC.AddSwitch(&INTGL_LTG_AC_CB, AID_VC_CB_P11R1_19, &anim_P11R1cbs[18]);

	// Row 2
	for (i = 0; i < P11R2_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R2_01 + i, P11R2_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&RCS_A_MAIN_SOV_CB, AID_VC_CB_P11R2_01, &anim_P11R2cbs[0]);
	MainPanelVC.AddSwitch(&RCS_A_QUAD4_TCA_CB, AID_VC_CB_P11R2_02, &anim_P11R2cbs[1]);
	MainPanelVC.AddSwitch(&RCS_A_QUAD3_TCA_CB, AID_VC_CB_P11R2_03, &anim_P11R2cbs[2]);
	MainPanelVC.AddSwitch(&RCS_A_QUAD2_TCA_CB, AID_VC_CB_P11R2_04, &anim_P11R2cbs[3]);
	MainPanelVC.AddSwitch(&RCS_A_QUAD1_TCA_CB, AID_VC_CB_P11R2_05, &anim_P11R2cbs[4]);
	MainPanelVC.AddSwitch(&RCS_A_ISOL_VLV_CB, AID_VC_CB_P11R2_06, &anim_P11R2cbs[5]);
	MainPanelVC.AddSwitch(&RCS_A_ASC_FEED_2_CB, AID_VC_CB_P11R2_07, &anim_P11R2cbs[6]);
	MainPanelVC.AddSwitch(&RCS_A_ASC_FEED_1_CB, AID_VC_CB_P11R2_08, &anim_P11R2cbs[7]);
	MainPanelVC.AddSwitch(&THRUST_DISP_CB, AID_VC_CB_P11R2_09, &anim_P11R2cbs[8]);
	MainPanelVC.AddSwitch(&MISSION_TIMER_CB, AID_VC_CB_P11R2_10, &anim_P11R2cbs[9]);
	MainPanelVC.AddSwitch(&CDR_XPTR_CB, AID_VC_CB_P11R2_11, &anim_P11R2cbs[10]);
	MainPanelVC.AddSwitch(&RNG_RT_ALT_RT_DC_CB, AID_VC_CB_P11R2_12, &anim_P11R2cbs[11]);
	MainPanelVC.AddSwitch(&GASTA_DC_CB, AID_VC_CB_P11R2_13, &anim_P11R2cbs[12]);
	MainPanelVC.AddSwitch(&CDR_FDAI_DC_CB, AID_VC_CB_P11R2_14, &anim_P11R2cbs[13]);
	MainPanelVC.AddSwitch(&COAS_DC_CB, AID_VC_CB_P11R2_15, &anim_P11R2cbs[14]);
	MainPanelVC.AddSwitch(&ORDEAL_DC_CB, AID_VC_CB_P11R2_16, &anim_P11R2cbs[15]);
	MainPanelVC.AddSwitch(&RNG_RT_ALT_RT_AC_CB, AID_VC_CB_P11R2_17, &anim_P11R2cbs[16]);
	MainPanelVC.AddSwitch(&GASTA_AC_CB, AID_VC_CB_P11R2_18, &anim_P11R2cbs[17]);
	MainPanelVC.AddSwitch(&CDR_FDAI_AC_CB, AID_VC_CB_P11R2_19, &anim_P11R2cbs[18]);

	// Row 3
	for (i = 0; i < P11R3_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R3_01 + i, P11R3_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&PROP_DES_HE_REG_VENT_CB, AID_VC_CB_P11R3_01, &anim_P11R3cbs[0]);
	MainPanelVC.AddSwitch(&HTR_RR_STBY_CB, AID_VC_CB_P11R3_02, &anim_P11R3cbs[1]);
	MainPanelVC.AddSwitch(&HTR_RR_OPR_CB, AID_VC_CB_P11R3_03, &anim_P11R3cbs[2]);
	MainPanelVC.AddSwitch(&HTR_LR_CB, AID_VC_CB_P11R3_04, &anim_P11R3cbs[3]);
	MainPanelVC.AddSwitch(&HTR_DOCK_WINDOW_CB, AID_VC_CB_P11R3_05, &anim_P11R3cbs[4]);
	MainPanelVC.AddSwitch(&HTR_AOT_CB, AID_VC_CB_P11R3_06, &anim_P11R3cbs[5]);
	MainPanelVC.AddSwitch(&INST_SIG_CONDR_1_CB, AID_VC_CB_P11R3_07, &anim_P11R3cbs[6]);
	MainPanelVC.AddSwitch(&CDR_SCS_AEA_CB, AID_VC_CB_P11R3_08, &anim_P11R3cbs[7]);
	MainPanelVC.AddSwitch(&CDR_SCS_ABORT_STAGE_CB, AID_VC_CB_P11R3_09, &anim_P11R3cbs[8]);
	MainPanelVC.AddSwitch(&CDR_SCS_ATCA_CB, AID_VC_CB_P11R3_10, &anim_P11R3cbs[9]);
	MainPanelVC.AddSwitch(&CDR_SCS_AELD_CB, AID_VC_CB_P11R3_11, &anim_P11R3cbs[10]);
	MainPanelVC.AddSwitch(&SCS_ENG_CONT_CB, AID_VC_CB_P11R3_12, &anim_P11R3cbs[11]);
	MainPanelVC.AddSwitch(&SCS_ATT_DIR_CONT_CB, AID_VC_CB_P11R3_13, &anim_P11R3cbs[12]);
	MainPanelVC.AddSwitch(&SCS_ENG_START_OVRD_CB, AID_VC_CB_P11R3_14, &anim_P11R3cbs[13]);
	MainPanelVC.AddSwitch(&SCS_DECA_PWR_CB, AID_VC_CB_P11R3_15, &anim_P11R3cbs[14]);
	MainPanelVC.AddSwitch(&EDS_CB_LG_FLAG, AID_VC_CB_P11R3_16, &anim_P11R3cbs[15]);
	MainPanelVC.AddSwitch(&EDS_CB_LOGIC_A, AID_VC_CB_P11R3_17, &anim_P11R3cbs[16]);
	MainPanelVC.AddSwitch(&CDR_LTG_UTIL_CB, AID_VC_CB_P11R3_18, &anim_P11R3cbs[17]);
	MainPanelVC.AddSwitch(&CDR_LTG_ANUN_DOCK_COMPNT_CB, AID_VC_CB_P11R3_19, &anim_P11R3cbs[18]);

	// Row 4
	for (i = 0; i < P11R4_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R4_01 + i, P11R4_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&RCS_QUAD_4_CDR_HTR_CB, AID_VC_CB_P11R4_01, &anim_P11R4cbs[0]);
	MainPanelVC.AddSwitch(&RCS_QUAD_3_CDR_HTR_CB, AID_VC_CB_P11R4_02, &anim_P11R4cbs[1]);
	MainPanelVC.AddSwitch(&RCS_QUAD_2_CDR_HTR_CB, AID_VC_CB_P11R4_03, &anim_P11R4cbs[2]);
	MainPanelVC.AddSwitch(&RCS_QUAD_1_CDR_HTR_CB, AID_VC_CB_P11R4_04, &anim_P11R4cbs[3]);
	MainPanelVC.AddSwitch(&ECS_SUIT_FAN_1_CB, AID_VC_CB_P11R4_05, &anim_P11R4cbs[4]);
	MainPanelVC.AddSwitch(&ECS_CABIN_FAN_1_CB, AID_VC_CB_P11R4_06, &anim_P11R4cbs[5]);
	MainPanelVC.AddSwitch(&ECS_GLYCOL_PUMP_2_CB, AID_VC_CB_P11R4_07, &anim_P11R4cbs[6]);
	MainPanelVC.AddSwitch(&ECS_GLYCOL_PUMP_1_CB, AID_VC_CB_P11R4_08, &anim_P11R4cbs[7]);
	MainPanelVC.AddSwitch(&ECS_GLYCOL_PUMP_AUTO_XFER_CB, AID_VC_CB_P11R4_09, &anim_P11R4cbs[8]);
	MainPanelVC.AddSwitch(&COMM_UP_DATA_LINK_CB, AID_VC_CB_P11R4_10, &anim_P11R4cbs[9]);
	MainPanelVC.AddSwitch(&COMM_SEC_SBAND_XCVR_CB, AID_VC_CB_P11R4_11, &anim_P11R4cbs[10]);
	MainPanelVC.AddSwitch(&COMM_SEC_SBAND_PA_CB, AID_VC_CB_P11R4_12, &anim_P11R4cbs[11]);
	MainPanelVC.AddSwitch(&COMM_VHF_XMTR_B_CB, AID_VC_CB_P11R4_13, &anim_P11R4cbs[12]);
	MainPanelVC.AddSwitch(&COMM_VHF_RCVR_A_CB, AID_VC_CB_P11R4_14, &anim_P11R4cbs[13]);
	MainPanelVC.AddSwitch(&COMM_CDR_AUDIO_CB, AID_VC_CB_P11R4_15, &anim_P11R4cbs[14]);
	MainPanelVC.AddSwitch(&PGNS_SIG_STR_DISP_CB, AID_VC_CB_P11R4_16, &anim_P11R4cbs[15]);
	MainPanelVC.AddSwitch(&PGNS_LDG_RDR_CB, AID_VC_CB_P11R4_17, &anim_P11R4cbs[16]);
	MainPanelVC.AddSwitch(&PGNS_RNDZ_RDR_CB, AID_VC_CB_P11R4_18, &anim_P11R4cbs[17]);
	MainPanelVC.AddSwitch(&LGC_DSKY_CB, AID_VC_CB_P11R4_19, &anim_P11R4cbs[18]);
	MainPanelVC.AddSwitch(&IMU_SBY_CB, AID_VC_CB_P11R4_20, &anim_P11R4cbs[19]);
	MainPanelVC.AddSwitch(&IMU_OPR_CB, AID_VC_CB_P11R4_21, &anim_P11R4cbs[20]);

	// Row 5
	for (i = 0; i < P11R5_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P11R5_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P11R5_01 + i, P11R5_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&CDRBatteryFeedTieCB1, AID_VC_CB_P11R5_01, &anim_P11R5cbs[0]);
	MainPanelVC.AddSwitch(&CDRBatteryFeedTieCB2, AID_VC_CB_P11R5_02, &anim_P11R5cbs[1]);
	MainPanelVC.AddSwitch(&CDRCrossTieBalCB, AID_VC_CB_P11R5_03, &anim_P11R5cbs[2]);
	MainPanelVC.AddSwitch(&CDRCrossTieBusCB, AID_VC_CB_P11R5_04, &anim_P11R5cbs[3]);
	MainPanelVC.AddSwitch(&CDRXLunarBusTieCB, AID_VC_CB_P11R5_05, &anim_P11R5cbs[4]);
	MainPanelVC.AddSwitch(&CDRDesECAContCB, AID_VC_CB_P11R5_06, &anim_P11R5cbs[5]);
	MainPanelVC.AddSwitch(&CDRDesECAMainCB, AID_VC_CB_P11R5_07, &anim_P11R5cbs[6]);
	MainPanelVC.AddSwitch(&CDRAscECAContCB, AID_VC_CB_P11R5_08, &anim_P11R5cbs[7]);
	MainPanelVC.AddSwitch(&CDRAscECAMainCB, AID_VC_CB_P11R5_09, &anim_P11R5cbs[8]);
	MainPanelVC.AddSwitch(&CDRInverter1CB, AID_VC_CB_P11R5_10, &anim_P11R5cbs[9]);
	MainPanelVC.AddSwitch(&CDRDCBusVoltCB, AID_VC_CB_P11R5_11, &anim_P11R5cbs[10]);

	// Panel 12

	for (i = 0; i < P12_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P12_01 + i, P12_TOGGLE_POS[i] + P12_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P12_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P12_01 + i, P12_ROT_POS[i] + ofs, 0.02);
	}

	for (i = 0; i < P12_TWCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_TW_P12_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_TW_P12_01 + i, P12_TW_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_PANEL12_NEEDLES, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	//oapiVCRegisterArea(AID_VC_RECORDER_TALKBACK, _R(806, 1460, 829, 1483), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	MainPanelVC.AddSwitch(&Panel12AudioCtlSwitch, AID_VC_SWITCH_P12_01, &anim_P12switch[0]);
	MainPanelVC.AddSwitch(&Panel12UpdataLinkSwitch, AID_VC_SWITCH_P12_02, &anim_P12switch[1]);
	MainPanelVC.AddSwitch(&LMPAudSBandSwitch, AID_VC_SWITCH_P12_03, &anim_P12switch[2]);
	MainPanelVC.AddSwitch(&LMPAudICSSwitch, AID_VC_SWITCH_P12_04, &anim_P12switch[3]);
	MainPanelVC.AddSwitch(&LMPAudRelaySwitch, AID_VC_SWITCH_P12_05, &anim_P12switch[4]);
	MainPanelVC.AddSwitch(&LMPAudVHFASwitch, AID_VC_SWITCH_P12_06, &anim_P12switch[5]);
	MainPanelVC.AddSwitch(&LMPAudVHFBSwitch, AID_VC_SWITCH_P12_07, &anim_P12switch[6]);
	MainPanelVC.AddSwitch(&LMPAudVOXSwitch, AID_VC_SWITCH_P12_08, &anim_P12switch[7]);
	MainPanelVC.AddSwitch(&SBandModulateSwitch, AID_VC_SWITCH_P12_09, &anim_P12switch[8]);
	MainPanelVC.AddSwitch(&SBandXCvrSelSwitch, AID_VC_SWITCH_P12_10, &anim_P12switch[9]);
	MainPanelVC.AddSwitch(&SBandPASelSwitch, AID_VC_SWITCH_P12_11, &anim_P12switch[10]);
	MainPanelVC.AddSwitch(&SBandVoiceSwitch, AID_VC_SWITCH_P12_12, &anim_P12switch[11]);
	MainPanelVC.AddSwitch(&SBandPCMSwitch, AID_VC_SWITCH_P12_13, &anim_P12switch[12]);
	MainPanelVC.AddSwitch(&SBandRangeSwitch, AID_VC_SWITCH_P12_14, &anim_P12switch[13]);
	MainPanelVC.AddSwitch(&VHFAVoiceSwitch, AID_VC_SWITCH_P12_15, &anim_P12switch[14]);
	MainPanelVC.AddSwitch(&VHFARcvrSwtich, AID_VC_SWITCH_P12_16, &anim_P12switch[15]);
	MainPanelVC.AddSwitch(&VHFBVoiceSwitch, AID_VC_SWITCH_P12_17, &anim_P12switch[16]);
	MainPanelVC.AddSwitch(&VHFBRcvrSwtich, AID_VC_SWITCH_P12_18, &anim_P12switch[17]);
	MainPanelVC.AddSwitch(&TLMBiomedSwtich, AID_VC_SWITCH_P12_19, &anim_P12switch[18]);
	MainPanelVC.AddSwitch(&TLMBitrateSwitch, AID_VC_SWITCH_P12_20, &anim_P12switch[19]);
	MainPanelVC.AddSwitch(&TapeRecorderSwitch, AID_VC_SWITCH_P12_21, &anim_P12switch[20]);
	MainPanelVC.AddSwitch(&Panel12AntTrackModeSwitch, AID_VC_SWITCH_P12_22, &anim_P12switch[21]);

	MainPanelVC.AddSwitch(&Panel12AntPitchKnob, AID_VC_ROT_P12_01, &anim_P12_Rot[0]);
	MainPanelVC.AddSwitch(&Panel12AntYawKnob, AID_VC_ROT_P12_02, &anim_P12_Rot[1]);
	MainPanelVC.AddSwitch(&Panel12SBandAntSelKnob, AID_VC_ROT_P12_03, &anim_P12_Rot[2]);
	MainPanelVC.AddSwitch(&Panel12VHFAntSelKnob, AID_VC_ROT_P12_04, &anim_P12_Rot[3]);

	MainPanelVC.AddSwitch(&LMPAudSBandVol, AID_VC_TW_P12_01, &anim_P12thumbwheels[0]);
	MainPanelVC.AddSwitch(&LMPAudICSVol, AID_VC_TW_P12_02, &anim_P12thumbwheels[1]);
	MainPanelVC.AddSwitch(&LMPAudVOXSens, AID_VC_TW_P12_03, &anim_P12thumbwheels[2]);
	MainPanelVC.AddSwitch(&LMPAudVHFAVol, AID_VC_TW_P12_04, &anim_P12thumbwheels[3]);
	MainPanelVC.AddSwitch(&LMPAudVHFBVol, AID_VC_TW_P12_05, &anim_P12thumbwheels[4]);
	MainPanelVC.AddSwitch(&LMPAudMasterVol, AID_VC_TW_P12_06, &anim_P12thumbwheels[5]);
	MainPanelVC.AddSwitch(&VHFASquelch, AID_VC_TW_P12_07, &anim_P12thumbwheels[6]);
	MainPanelVC.AddSwitch(&VHFBSquelch, AID_VC_TW_P12_08, &anim_P12thumbwheels[7]);

	// Panel 14

	for (i = 0; i < P14_SWITCHCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_SWITCH_P14_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP);
		oapiVCSetAreaClickmode_Spherical(AID_VC_SWITCH_P14_01 + i, P14_TOGGLE_POS[i] + P14_CLICK + ofs, 0.006);
	}

	for (i = 0; i < P14_ROTCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_ROT_P14_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_ROT_P14_01 + i, P14_ROT_POS[i] + ofs, 0.02);
	}

	oapiVCRegisterArea(AID_VC_PANEL14_COMPLIGHTS, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_PANEL14_NEEDLES, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE);
	oapiVCRegisterArea(AID_VC_DSC_BATTERY_TALKBACKS, _R(550, 1002, 865, 1025), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);
	oapiVCRegisterArea(AID_VC_ASC_BATTERY_TALKBACKS, _R(932, 1027, 1176, 1050), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND, MainPanelTex2);

	MainPanelVC.AddSwitch(&EPSEDVoltSelect, AID_VC_SWITCH_P14_01, &anim_P14switch[0]);
	MainPanelVC.AddSwitch(&EPSInverterSwitch, AID_VC_SWITCH_P14_02, &anim_P14switch[1]);
	MainPanelVC.AddSwitch(&DSCSEBat1HVSwitch, AID_VC_SWITCH_P14_03, &anim_P14switch[2]);
	MainPanelVC.AddSwitch(&DSCSEBat2HVSwitch, AID_VC_SWITCH_P14_04, &anim_P14switch[3]);
	MainPanelVC.AddSwitch(&DSCCDRBat3HVSwitch, AID_VC_SWITCH_P14_05, &anim_P14switch[4]);
	MainPanelVC.AddSwitch(&DSCCDRBat4HVSwitch, AID_VC_SWITCH_P14_06, &anim_P14switch[5]);
	MainPanelVC.AddSwitch(&DSCSEBat1LVSwitch, AID_VC_SWITCH_P14_07, &anim_P14switch[6]);
	MainPanelVC.AddSwitch(&DSCSEBat2LVSwitch, AID_VC_SWITCH_P14_08, &anim_P14switch[7]);
	MainPanelVC.AddSwitch(&DSCCDRBat3LVSwitch, AID_VC_SWITCH_P14_09, &anim_P14switch[8]);
	MainPanelVC.AddSwitch(&DSCCDRBat4LVSwitch, AID_VC_SWITCH_P14_10, &anim_P14switch[9]);
	MainPanelVC.AddSwitch(&DSCBattFeedSwitch, AID_VC_SWITCH_P14_11, &anim_P14switch[10]);
	MainPanelVC.AddSwitch(&ASCBat5SESwitch, AID_VC_SWITCH_P14_12, &anim_P14switch[11]);
	MainPanelVC.AddSwitch(&ASCBat5CDRSwitch, AID_VC_SWITCH_P14_13, &anim_P14switch[12]);
	MainPanelVC.AddSwitch(&ASCBat6CDRSwitch, AID_VC_SWITCH_P14_14, &anim_P14switch[13]);
	MainPanelVC.AddSwitch(&ASCBat6SESwitch, AID_VC_SWITCH_P14_15, &anim_P14switch[14]);
	MainPanelVC.AddSwitch(&UpDataSquelchSwitch, AID_VC_SWITCH_P14_16, &anim_P14switch[15]);
	MainPanelVC.AddSwitch(&EPSMonitorSelectRotary, AID_VC_ROT_P14_01, &anim_P14_Rot[0]);

	// Panel 16
	// Row 1
	for (i = 0; i < P16R1_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R1_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R1_01 + i, P16R1_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&LMP_EVT_TMR_FDAI_DC_CB, AID_VC_CB_P16R1_01, &anim_P16R1cbs[0]);
	MainPanelVC.AddSwitch(&SE_XPTR_DC_CB, AID_VC_CB_P16R1_02, &anim_P16R1cbs[1]);
	MainPanelVC.AddSwitch(&RCS_B_ASC_FEED_1_CB, AID_VC_CB_P16R1_03, &anim_P16R1cbs[2]);
	MainPanelVC.AddSwitch(&RCS_B_ASC_FEED_2_CB, AID_VC_CB_P16R1_04, &anim_P16R1cbs[3]);
	MainPanelVC.AddSwitch(&RCS_B_ISOL_VLV_CB, AID_VC_CB_P16R1_05, &anim_P16R1cbs[4]);
	MainPanelVC.AddSwitch(&RCS_B_QUAD1_TCA_CB, AID_VC_CB_P16R1_06, &anim_P16R1cbs[5]);
	MainPanelVC.AddSwitch(&RCS_B_QUAD2_TCA_CB, AID_VC_CB_P16R1_07, &anim_P16R1cbs[6]);
	MainPanelVC.AddSwitch(&RCS_B_QUAD3_TCA_CB, AID_VC_CB_P16R1_08, &anim_P16R1cbs[7]);
	MainPanelVC.AddSwitch(&RCS_B_QUAD4_TCA_CB, AID_VC_CB_P16R1_09, &anim_P16R1cbs[8]);
	MainPanelVC.AddSwitch(&RCS_B_CRSFD_CB, AID_VC_CB_P16R1_10, &anim_P16R1cbs[9]);
	MainPanelVC.AddSwitch(&RCS_B_TEMP_PRESS_DISP_FLAGS_CB, AID_VC_CB_P16R1_11, &anim_P16R1cbs[10]);
	MainPanelVC.AddSwitch(&RCS_B_PQGS_DISP_CB, AID_VC_CB_P16R1_12, &anim_P16R1cbs[11]);
	MainPanelVC.AddSwitch(&RCS_B_MAIN_SOV_CB, AID_VC_CB_P16R1_13, &anim_P16R1cbs[12]);
	MainPanelVC.AddSwitch(&PROP_DISP_ENG_OVRD_LOGIC_CB, AID_VC_CB_P16R1_14, &anim_P16R1cbs[13]);
	MainPanelVC.AddSwitch(&PROP_PQGS_CB, AID_VC_CB_P16R1_15, &anim_P16R1cbs[14]);
	MainPanelVC.AddSwitch(&PROP_ASC_HE_REG_CB, AID_VC_CB_P16R1_16, &anim_P16R1cbs[15]);

	// Row 2
	for (i = 0; i < P16R2_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R2_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R2_01 + i, P16R2_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&LTG_FLOOD_CB, AID_VC_CB_P16R2_01, &anim_P16R2cbs[0]);
	MainPanelVC.AddSwitch(&LTG_TRACK_CB, AID_VC_CB_P16R2_02, &anim_P16R2cbs[1]);
	MainPanelVC.AddSwitch(&LTG_ANUN_DOCK_COMPNT_CB, AID_VC_CB_P16R2_03, &anim_P16R2cbs[2]);
	MainPanelVC.AddSwitch(&LTG_MASTER_ALARM_CB, AID_VC_CB_P16R2_04, &anim_P16R2cbs[3]);
	MainPanelVC.AddSwitch(&EDS_CB_LOGIC_B, AID_VC_CB_P16R2_05, &anim_P16R2cbs[4]);
	MainPanelVC.AddSwitch(&SCS_AEA_CB, AID_VC_CB_P16R2_06, &anim_P16R2cbs[5]);
	MainPanelVC.AddSwitch(&SCS_ENG_ARM_CB, AID_VC_CB_P16R2_07, &anim_P16R2cbs[6]);
	MainPanelVC.AddSwitch(&SCS_ASA_CB, AID_VC_CB_P16R2_08, &anim_P16R2cbs[7]);
	MainPanelVC.AddSwitch(&SCS_AELD_CB, AID_VC_CB_P16R2_09, &anim_P16R2cbs[8]);
	MainPanelVC.AddSwitch(&SCS_ATCA_CB, AID_VC_CB_P16R2_10, &anim_P16R2cbs[9]);
	MainPanelVC.AddSwitch(&SCS_ABORT_STAGE_CB, AID_VC_CB_P16R2_11, &anim_P16R2cbs[10]);
	MainPanelVC.AddSwitch(&SCS_ATCA_AGS_CB, AID_VC_CB_P16R2_12, &anim_P16R2cbs[11]);
	MainPanelVC.AddSwitch(&SCS_DES_ENG_OVRD_CB, AID_VC_CB_P16R2_13, &anim_P16R2cbs[12]);
	MainPanelVC.AddSwitch(&INST_CWEA_CB, AID_VC_CB_P16R2_14, &anim_P16R2cbs[13]);
	MainPanelVC.AddSwitch(&INST_SIG_SENSOR_CB, AID_VC_CB_P16R2_15, &anim_P16R2cbs[14]);
	MainPanelVC.AddSwitch(&INST_PCMTEA_CB, AID_VC_CB_P16R2_16, &anim_P16R2cbs[15]);
	MainPanelVC.AddSwitch(&INST_SIG_CONDR_2_CB, AID_VC_CB_P16R2_17, &anim_P16R2cbs[16]);
	MainPanelVC.AddSwitch(&ECS_SUIT_FLOW_CONT_CB, AID_VC_CB_P16R2_18, &anim_P16R2cbs[17]);

	// Row 3
	for (i = 0; i < P16R3_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R3_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R3_01 + i, P16R3_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&COMM_DISP_CB, AID_VC_CB_P16R3_01, &anim_P16R3cbs[0]);
	MainPanelVC.AddSwitch(&COMM_SE_AUDIO_CB, AID_VC_CB_P16R3_02, &anim_P16R3cbs[1]);
	MainPanelVC.AddSwitch(&COMM_VHF_XMTR_A_CB, AID_VC_CB_P16R3_03, &anim_P16R3cbs[2]);
	MainPanelVC.AddSwitch(&COMM_VHF_RCVR_B_CB, AID_VC_CB_P16R3_04, &anim_P16R3cbs[3]);
	MainPanelVC.AddSwitch(&COMM_PRIM_SBAND_PA_CB, AID_VC_CB_P16R3_05, &anim_P16R3cbs[4]);
	MainPanelVC.AddSwitch(&COMM_PRIM_SBAND_XCVR_CB, AID_VC_CB_P16R3_06, &anim_P16R3cbs[5]);
	MainPanelVC.AddSwitch(&COMM_SBAND_ANT_CB, AID_VC_CB_P16R3_07, &anim_P16R3cbs[6]);
	MainPanelVC.AddSwitch(&COMM_PMP_CB, AID_VC_CB_P16R3_08, &anim_P16R3cbs[7]);
	MainPanelVC.AddSwitch(&COMM_TV_CB, AID_VC_CB_P16R3_09, &anim_P16R3cbs[8]);
	MainPanelVC.AddSwitch(&ECS_DISP_CB, AID_VC_CB_P16R3_10, &anim_P16R3cbs[9]);
	MainPanelVC.AddSwitch(&ECS_GLYCOL_PUMP_SEC_CB, AID_VC_CB_P16R3_11, &anim_P16R3cbs[10]);
	MainPanelVC.AddSwitch(&ECS_LGC_PUMP_CB, AID_VC_CB_P16R3_12, &anim_P16R3cbs[11]);
	MainPanelVC.AddSwitch(&ECS_CABIN_FAN_CONT_CB, AID_VC_CB_P16R3_13, &anim_P16R3cbs[12]);
	MainPanelVC.AddSwitch(&ECS_CABIN_REPRESS_CB, AID_VC_CB_P16R3_14, &anim_P16R3cbs[13]);
	MainPanelVC.AddSwitch(&ECS_SUIT_FAN_2_CB, AID_VC_CB_P16R3_15, &anim_P16R3cbs[14]);
	MainPanelVC.AddSwitch(&ECS_SUIT_FAN_DP_CB, AID_VC_CB_P16R3_16, &anim_P16R3cbs[15]);
	MainPanelVC.AddSwitch(&ECS_DIVERT_VLV_CB, AID_VC_CB_P16R3_17, &anim_P16R3cbs[16]);
	MainPanelVC.AddSwitch(&ECS_CO2_SENSOR_CB, AID_VC_CB_P16R3_18, &anim_P16R3cbs[17]);

	// Row 4
	for (i = 0; i < P16R4_CBCOUNT; i++)
	{
		oapiVCRegisterArea(AID_VC_CB_P16R4_01 + i, PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN);
		oapiVCSetAreaClickmode_Spherical(AID_VC_CB_P16R4_01 + i, P16R4_CB_POS[i] + ofs, 0.008);
	}

	MainPanelVC.AddSwitch(&RCS_QUAD_1_LMP_HTR_CB, AID_VC_CB_P16R4_01, &anim_P16R4cbs[0]);
	MainPanelVC.AddSwitch(&RCS_QUAD_2_LMP_HTR_CB, AID_VC_CB_P16R4_02, &anim_P16R4cbs[1]);
	MainPanelVC.AddSwitch(&RCS_QUAD_3_LMP_HTR_CB, AID_VC_CB_P16R4_03, &anim_P16R4cbs[2]);
	MainPanelVC.AddSwitch(&RCS_QUAD_4_LMP_HTR_CB, AID_VC_CB_P16R4_04, &anim_P16R4cbs[3]);
	MainPanelVC.AddSwitch(&HTR_DISP_CB, AID_VC_CB_P16R4_05, &anim_P16R4cbs[4]);
	MainPanelVC.AddSwitch(&HTR_SBD_ANT_CB, AID_VC_CB_P16R4_06, &anim_P16R4cbs[5]);
	MainPanelVC.AddSwitch(&CAMR_SEQ_CB, AID_VC_CB_P16R4_07, &anim_P16R4cbs[6]);
	MainPanelVC.AddSwitch(&EPS_DISP_CB, AID_VC_CB_P16R4_08, &anim_P16R4cbs[7]);
	MainPanelVC.AddSwitch(&LMPDCBusVoltCB, AID_VC_CB_P16R4_09, &anim_P16R4cbs[8]);
	MainPanelVC.AddSwitch(&LMPInverter2CB, AID_VC_CB_P16R4_10, &anim_P16R4cbs[9]);
	MainPanelVC.AddSwitch(&LMPAscECAMainCB, AID_VC_CB_P16R4_11, &anim_P16R4cbs[10]);
	MainPanelVC.AddSwitch(&LMPAscECAContCB, AID_VC_CB_P16R4_12, &anim_P16R4cbs[11]);
	MainPanelVC.AddSwitch(&LMPDesECAMainCB, AID_VC_CB_P16R4_13, &anim_P16R4cbs[12]);
	MainPanelVC.AddSwitch(&LMPDesECAContCB, AID_VC_CB_P16R4_14, &anim_P16R4cbs[13]);
	MainPanelVC.AddSwitch(&LMPXLunarBusTieCB, AID_VC_CB_P16R4_15, &anim_P16R4cbs[14]);
	MainPanelVC.AddSwitch(&LMPCrossTieBusCB, AID_VC_CB_P16R4_16, &anim_P16R4cbs[15]);
	MainPanelVC.AddSwitch(&LMPCrossTieBalCB, AID_VC_CB_P16R4_17, &anim_P16R4cbs[16]);
	MainPanelVC.AddSwitch(&LMPBatteryFeedTieCB1, AID_VC_CB_P16R4_18, &anim_P16R4cbs[17]);
	MainPanelVC.AddSwitch(&LMPBatteryFeedTieCB2, AID_VC_CB_P16R4_19, &anim_P16R4cbs[18]);

	//
	// Initialize surfaces and switches
	//
	InitPanelVC();
}

bool LEM::clbkVCMouseEvent(int id, int event, VECTOR3 &p)
{
	switch (id) {
		case AID_VC_LEM_MA_LEFT:
		case AID_VC_LEM_MA_RIGHT:
			return CWEA.CheckMasterAlarmMouseClick(event);

		case AID_VC_ABORT_BUTTON:
			AbortSwitch.CheckMouseClickVC(event);
			return true;

		case AID_VC_ABORTSTAGE_BUTTON:
			AbortStageSwitch.CheckMouseClickVC(event);
			return true;

		case AID_VC_RR_SLEW_SWITCH:
			RadarSlewSwitch.CheckMouseClickVC(event, p);
			return true;

		case AID_VC_STAGE_SWITCH:
			EDStage.CheckMouseClickVC(event, p);
			return true;

		case AID_VC_START_BUTTON:
			ManualEngineStart.CheckMouseClickVC(event);
			return true;

		case AID_VC_STOP_BUTTON_CDR:
			CDRManualEngineStop.CheckMouseClickVC(event);
			return true;

		case AID_VC_STOP_BUTTON_LMP:
			LMPManualEngineStop.CheckMouseClickVC(event);
			return true;

		case AID_VC_PLUSX_BUTTON:
			PlusXTranslationButton.ProcessMouseVC(event, p);
			return true;
	}
	return MainPanelVC.VCMouseEvent(id, event, p);
	//return false;
}

bool LEM::clbkVCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	switch (id) {

	case AID_VC_LM_CWS_LEFT:
		CWEA.RedrawLeft(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_LM_CWS_RIGHT:
		CWEA.RedrawRight(surf, srf[SFR_VC_CW_LIGHTS]);
		return true;

	case AID_VC_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_VC_DIGITALDISP], srf[SRF_VC_DSKYDISP]);
		return true;

	case AID_VC_DSKY_LIGHTS:
		if (pMission->GetLMDSKYVersion() == 3)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, true, true);
		}
		else if (pMission->GetLMDSKYVersion() == 2)
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS]);
		}
		else
		{
			dsky.RenderLights(surf, srf[SRF_VC_DSKY_LIGHTS], 0, 0, false);
		}
		return true;

	case AID_VC_LM_DEDA_LIGHTS:
		deda.RenderOprErr(surf, srf[SRF_DEDA_LIGHTSVC]);
		return true;

	case AID_VC_LM_DEDA_DISP:
		deda.RenderData(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_LM_DEDA_ADR:
		deda.RenderAdr(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_RDR_SIG_STR:
		SetAnimation(anim_Needle_Radar, RadarSignalStrengthMeter.GetDisplayValue() / 5);
		return true;

	case AID_VC_RANGE_TAPE:
		RadarTape.RenderRangeVC(surf, srf[SRF_VC_RADAR_TAPEA], srf[SRF_VC_RADAR_TAPEB], srf[SRF_VC_RADAR_TAPE2]);
		return true;

	case AID_VC_RATE_TAPE:
		RadarTape.RenderRateVC(surf, srf[SRF_VC_RADAR_TAPEA], srf[SRF_VC_RADAR_TAPEB]);
		return true;

	case AID_VC_XPOINTERCDR:
		RedrawPanel_XPointerVC(&crossPointerLeft, anim_xpointerx_cdr, anim_xpointery_cdr);
		return true;

	case AID_VC_XPOINTERLMP:
		RedrawPanel_XPointerVC(&crossPointerRight, anim_xpointerx_lmp, anim_xpointery_lmp);
		return true;

	case AID_VC_CONTACTLIGHT1:
		if (SCS_ENG_CONT_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(0, true); // Light On
		}
		else
		{
			SetContactLight(0, false); // Light On
		}
		return true;

	case AID_VC_CONTACTLIGHT2:
		if (SCS_ATCA_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			SetContactLight(1, true); // Light On
		}
		else
		{
			SetContactLight(1, false); // Light On
		}
		return true;

	case AID_VC_FDAI_LEFT:
		if (fdaiLeft.IsPowered()) {
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (AttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

			if (RateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (AttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI1PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			AnimateFDAI(attitude, rates, errors, anim_fdaiR_cdr, anim_fdaiP_cdr, anim_fdaiY_cdr, anim_fdaiRerror_cdr, anim_fdaiPerror_cdr, anim_fdaiYerror_cdr, anim_fdaiRrate_cdr, anim_fdaiPrate_cdr, anim_fdaiYrate_cdr);
			SetAnimation(anim_attflag_cdr, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_cdr, 1.0);
		}
		return true;

	case AID_VC_FDAI_RIGHT:
		if (fdaiRight.IsPowered()) {
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (RightAttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

			if (RightRateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (RightAttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI2PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			AnimateFDAI(attitude, rates, errors, anim_fdaiR_lmp, anim_fdaiP_lmp, anim_fdaiY_lmp, anim_fdaiRerror_lmp, anim_fdaiPerror_lmp, anim_fdaiYerror_lmp, anim_fdaiRrate_lmp, anim_fdaiPrate_lmp, anim_fdaiYrate_lmp);
			SetAnimation(anim_attflag_lmp, 0.0);
		}
		else
		{
			SetAnimation(anim_attflag_lmp, 1.0);
		}
		return true;

	case AID_VC_RR_NOTRACK:
		if (lca.GetAnnunVoltage() > 2.25 && (RR.GetNoTrackSignal() || LampToneTestRotary.GetState() == 6)) { // The AC side is only needed for the transmitter
			SetCompLight(LM_VC_COMP_LIGHT_1, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_1, false); // Light Off
		}
		return true;

	case AID_VC_PANEL2_COMPLIGHTS:
		if (lca.GetAnnunVoltage() > 2.25 && (scera2.GetSwitch(12, 2)->IsClosed() || PrimGlycolPumpController.GetPressureSwitch() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_2, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_2, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && (SuitFanDPSensor.GetSuitFanFail() == true || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_3, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_3, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25) {
			if (INST_CWEA_CB.IsPowered() && ECS_CO2_SENSOR_CB.IsPowered() && (scera1.GetVoltage(5, 2) >= (7.6 / 6))) {
				SetCompLight(LM_VC_COMP_LIGHT_4, true); // Light On
			}
			else if (CO2CanisterSelectSwitch.GetState() == 0 || LampToneTestRotary.GetState() == 6) {
				SetCompLight(LM_VC_COMP_LIGHT_4, true); // Light On
			}
			else {
				SetCompLight(LM_VC_COMP_LIGHT_4, false); // Light Off
			}
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_4, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && INST_CWEA_CB.IsPowered() && (scera1.GetVoltage(5, 3) < (792.5 / 720.0) || LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_5, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_5, false); // Light Off
		}
		return true;

	case AID_VC_PANEL14_COMPLIGHTS:
		if (lca.GetCompDockVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_6, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_6, false); // Light Off
		}

		if (lca.GetAnnunVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {
			SetCompLight(LM_VC_COMP_LIGHT_7, true); // Light On
		}
		else {
			SetCompLight(LM_VC_COMP_LIGHT_7, false); // Light Off
		}
		return true;

	case AID_VC_SEQ_LIGHTS:
		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 11) > 2.5 && stage < 2 || LampToneTestRotary.GetState() == 6)) {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_A, true); // Light On
		}
		else {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_A, false); // Light Off
		}

		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 12) > 2.5 || LampToneTestRotary.GetState() == 6)) {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_B, true); // Light On
		}
		else {
			SetStageSeqRelayLight(LM_VC_STG_SEQ_B, false); // Light Off
		}
		return true;

	case AID_VC_RCS_ASC_FEED_TALKBACKS:
		RCSAscFeed1ATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSAscFeed2ATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSAscFeed1BTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSAscFeed2BTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_LGC_CMD_ENABLE_14_TALKBACKS:
		RCSQuad1ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad4ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad1BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad4BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		return true;

	case AID_VC_LGC_CMD_ENABLE_23_TALKBACKS:
		RCSQuad2ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad3ACmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad2BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		RCSQuad3BCmdEnableTB.DrawSwitchVC(surf, srf[SRF_INDICATORREDVC]);
		return true;

	case AID_VC_RCS_XFEED_TALKBACK:
		RCSXFeedTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_MAIN_SOV_TALKBACKS:
		RCSMainSovATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		RCSMainSovBTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_PANEL8_TALKBACKS:
		EDDesFuelVentTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		EDDesOxidVentTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		EDLGTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_MPS_REG_CONTROLS_LEFT:
		ASCHeReg1TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DESHeReg1TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_MPS_REG_CONTROLS_RIGHT:
		ASCHeReg2TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DESHeReg2TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P1:
		if (!pfira.GetCDRXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_1, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_1, false); // Light Off
		}

		if (!pfira.GetThrustIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_2, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_2, false); // Light Off
		}

		if (!pfira.GetPropPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_3, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_3, false); // Light Off
		}
		return true;

	case AID_VC_PWRFAIL_LIGHTS_P2:

		if (!pfira.GetRCSPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_4, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_4, false); // Light Off
		}

		if (!pfira.GetRCSQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_5, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_5, false); // Light Off
		}

		if (!pfira.GetSuitCabinPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_6, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_6, false); // Light Off
		}

		if (!pfira.GetGlyTempPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_7, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_7, false); // Light Off
		}

		if (!pfira.GetO2H2OQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_8, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_8, false); // Light Off
		}

		if (!pfira.GetLMPXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_9, true); // Light On
		}
		else {
			SetPowerFailureLight(LM_VC_PWRFAIL_LIGHT_9, false); // Light Off
		}
		return true;

	case AID_VC_LEM_MA_LEFT:
	case AID_VC_LEM_MA_RIGHT:
		CWEA.RenderMasterAlarm(surf, srf[SRF_LEM_MASTERALARMVC], NULL);
		return true;

	case AID_VC_MPS_OXID_QUANTITY_INDICATOR:
		DPSOxidPercentMeter.DoDrawSwitchVC(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;
	case AID_VC_MPS_FUEL_QUANTITY_INDICATOR:
		DPSFuelPercentMeter.DoDrawSwitchVC(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;
	case AID_VC_MPS_HELIUM_PRESS_INDICATOR:
		MainHeliumPressureMeter.DoDrawSwitchVC(surf, srf[SRF_VC_DIGITALDISP2]);
		return true;

	case AID_VC_PANEL1_NEEDLES:
		EngineThrustInd.DoDrawSwitchVC(anim_P1needles[0]);
		CommandedThrustInd.DoDrawSwitchVC(anim_P1needles[1]);
		MainFuelTempInd.DoDrawSwitchVC(anim_P1needles[2]);
		MainOxidizerTempInd.DoDrawSwitchVC(anim_P1needles[3]);
		MainFuelPressInd.DoDrawSwitchVC(anim_P1needles[4]);
		MainOxidizerPressInd.DoDrawSwitchVC(anim_P1needles[5]);
		return true;

	case AID_VC_PANEL2_NEEDLES:
		LMRCSATempInd.DoDrawSwitchVC(anim_P2needles[0]);
		LMRCSBTempInd.DoDrawSwitchVC(anim_P2needles[1]);
		LMRCSAPressInd.DoDrawSwitchVC(anim_P2needles[2]);
		LMRCSBPressInd.DoDrawSwitchVC(anim_P2needles[3]);
		LMRCSAQtyInd.DoDrawSwitchVC(anim_P2needles[4]);
		LMRCSBQtyInd.DoDrawSwitchVC(anim_P2needles[5]);
		LMSuitTempMeter.DoDrawSwitchVC(anim_P2needles[6]);
		LMCabinTempMeter.DoDrawSwitchVC(anim_P2needles[7]);
		LMSuitPressMeter.DoDrawSwitchVC(anim_P2needles[8]);
		LMCabinPressMeter.DoDrawSwitchVC(anim_P2needles[9]);
		LMGlycolTempMeter.DoDrawSwitchVC(anim_P2needles[10]);
		LMGlycolPressMeter.DoDrawSwitchVC(anim_P2needles[11]);
		LMOxygenQtyMeter.DoDrawSwitchVC(anim_P2needles[12]);
		LMWaterQtyMeter.DoDrawSwitchVC(anim_P2needles[13]);
		LMCO2Meter.DoDrawSwitchVC(anim_P2needles[14]);
		return true;

	case AID_VC_PANEL3_NEEDLES:
		TempMonitorInd.DoDrawSwitchVC(anim_P3needles[0]);
		return true;

	case AID_VC_PANEL12_NEEDLES:
		ComPitchMeter.DoDrawSwitchVC(anim_P12needles[0]);
		ComYawMeter.DoDrawSwitchVC(anim_P12needles[1]);
		Panel12SignalStrengthMeter.DoDrawSwitchVC(anim_P12needles[2]);
		return true;

	case AID_VC_PANEL14_NEEDLES:
		EPSDCVoltMeter.DoDrawSwitchVC(anim_P14needles[0]);
		EPSDCAmMeter.DoDrawSwitchVC(anim_P14needles[1]);
		return true;

	case AID_VC_THRUST_WEIGHT_IND:
		ThrustWeightInd.DoDrawSwitchVC(anim_TW_indicator);
		return true;

	case AID_VC_ABORT_BUTTON:
		AbortSwitch.RedrawVC(anim_abortbutton);
		return true;

	case AID_VC_ABORTSTAGE_BUTTON:
		AbortStageSwitch.DrawSwitchVC(anim_abortstagebutton, anim_abortstagecover);
		return true;

	case AID_VC_RR_SLEW_SWITCH:
		RadarSlewSwitch.DrawSwitchVC(anim_rrslewsitch_x, anim_rrslewsitch_y);
		//SetAnimation(anim_stopbutton_cdr, ttca_throttle_pos_dig);
		return true;

	/*case AID_VC_RECORDER_TALKBACK:
		TapeRecorderTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;*/

	case AID_VC_DSC_BATTERY_TALKBACKS:
		DSCBattery1TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DSCBattery2TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DSCBattery3TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DSCBattery4TB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		DSCBattFeedTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_ASC_BATTERY_TALKBACKS:
		ASCBattery5ATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		ASCBattery5BTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		ASCBattery6ATB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		ASCBattery6BTB.DrawSwitchVC(surf, srf[SRF_INDICATORVC]);
		return true;

	case AID_VC_STAGE_SWITCH:
		EDStage.DrawSwitchVC(anim_stageswitch, anim_stagecover);
		return true;

	case AID_VC_START_BUTTON:
		ManualEngineStart.DoDrawSwitchVC(anim_startbutton);
		return true;

	case AID_VC_STOP_BUTTON_CDR:
		CDRManualEngineStop.DoDrawSwitchVC(anim_stopbutton_cdr);
		return true;

	case AID_VC_STOP_BUTTON_LMP:
		LMPManualEngineStop.DoDrawSwitchVC(anim_stopbutton_lmp);
		return true;

	case AID_VC_PLUSX_BUTTON:
		PlusXTranslationButton.RedrawVC(anim_plusxbutton);
		return true;
	}

	return MainPanelVC.VCRedrawEvent(id, event, surf);
	//return false;
}

void LEM::InitVCAnimations() {

	anim_P1switch[P1_SWITCHCOUNT] = -1;
	anim_P1_Rot[P1_ROTCOUNT] = -1;
	anim_P1needles[P1_NEEDLECOUNT] = -1;
	anim_P2switch[P2_SWITCHCOUNT] = -1;
	anim_P2_Rot[P2_ROTCOUNT] = -1;
	anim_P2needles[P2_NEEDLECOUNT] = -1;
	anim_P3switch[P3_SWITCHCOUNT] = -1;
	anim_P3_Rot[P3_ROTCOUNT] = -1;
	anim_P3needles[P3_NEEDLECOUNT] = -1;
	anim_P4switch[P4_SWITCHCOUNT] = -1;
	anim_P5switch[P5_SWITCHCOUNT] = -1;
	anim_P5_Rot[P5_ROTCOUNT] = -1;
	anim_P6switch[P6_SWITCHCOUNT] = -1;
	anim_P8switch[P8_SWITCHCOUNT] = -1;
	anim_P8thumbwheels[P8_TWCOUNT] = -1;
	anim_P11R1cbs[P11R1_CBCOUNT] = -1;
	anim_P11R2cbs[P11R2_CBCOUNT] = -1;
	anim_P11R3cbs[P11R3_CBCOUNT] = -1;
	anim_P11R4cbs[P11R4_CBCOUNT] = -1;
	anim_P11R5cbs[P11R5_CBCOUNT] = -1;
	anim_P12switch[P12_SWITCHCOUNT] = -1;
	anim_P12_Rot[P12_ROTCOUNT] = -1;
	anim_P12thumbwheels[P12_TWCOUNT] = -1;
	anim_P12needles[P12_NEEDLECOUNT] = -1;
	anim_P14switch[P14_SWITCHCOUNT] = -1;
	anim_P14_Rot[P14_ROTCOUNT] = -1;
	anim_P14needles[P14_NEEDLECOUNT] = -1;
	anim_P16R1cbs[P16R1_CBCOUNT] = -1;
	anim_P16R2cbs[P16R2_CBCOUNT] = -1;
	anim_P16R3cbs[P16R3_CBCOUNT] = -1;
	anim_P16R4cbs[P16R4_CBCOUNT] = -1;
	anim_TW_indicator = -1;
	anim_Needle_Radar = -1;
	anim_xpointerx_cdr = -1;
	anim_xpointery_cdr = -1;
	anim_xpointerx_lmp = -1;
	anim_xpointery_lmp = -1;
	anim_abortbutton = -1;
	anim_abortstagebutton = -1;
	anim_abortstagecover = -1;
	anim_rrslewsitch_x = -1;
	anim_rrslewsitch_y = -1;
	anim_stageswitch = -1;
	anim_stagecover = -1;
	anim_startbutton = -1;
	anim_stopbutton_cdr = -1;
	anim_stopbutton_lmp = -1;
	anim_plusxbutton = -1;
	anim_fdaiR_cdr = anim_fdaiR_lmp = -1;
	anim_fdaiP_cdr = anim_fdaiP_lmp = -1;
	anim_fdaiY_cdr = anim_fdaiY_lmp = -1;
	anim_fdaiRerror_cdr = anim_fdaiRerror_lmp = -1;
	anim_fdaiPerror_cdr = anim_fdaiPerror_lmp = -1;
	anim_fdaiYerror_cdr = anim_fdaiYerror_lmp = -1;
	anim_fdaiRrate_cdr = anim_fdaiRrate_lmp = -1;
	anim_fdaiPrate_cdr = anim_fdaiPrate_lmp = -1;
	anim_fdaiYrate_cdr = anim_fdaiYrate_lmp = -1;
	anim_attflag_cdr = anim_attflag_lmp = -1;
}

void LEM::DeleteVCAnimations()
{
	int i = 0;

	for (i = 0; i < P1_SWITCHCOUNT; i++) delete mgt_P1switch[i];

	for (i = 0; i < P1_ROTCOUNT; i++) delete mgt_P1Rot[i];

	for (i = 0; i < P1_NEEDLECOUNT; i++) delete mgt_P1needles[i];

	for (i = 0; i < P2_SWITCHCOUNT; i++) delete mgt_P2switch[i];

	for (i = 0; i < P2_ROTCOUNT; i++) delete mgt_P2Rot[i];

	for (i = 0; i < P2_NEEDLECOUNT; i++) delete mgt_P2needles[i];

	for (i = 0; i < P3_SWITCHCOUNT; i++) delete mgt_P3switch[i];

	for (i = 0; i < P3_ROTCOUNT; i++) delete mgt_P3Rot[i];

	for (i = 0; i < P3_NEEDLECOUNT; i++) delete mgt_P3needles[i];

	for (i = 0; i < P4_SWITCHCOUNT; i++) delete mgt_P4switch[i];

	for (i = 0; i < P5_SWITCHCOUNT; i++) delete mgt_P5switch[i];

	for (i = 0; i < P5_ROTCOUNT; i++) delete mgt_P5Rot[i];

	for (i = 0; i < P6_SWITCHCOUNT; i++) delete mgt_P6switch[i];

	for (i = 0; i < P8_SWITCHCOUNT; i++) delete mgt_P8switch[i];

	for (i = 0; i < P8_TWCOUNT; i++) delete mgt_P8thumbwheels[i];

	for (i = 0; i < P11R1_CBCOUNT; i++) delete mgt_P11R1cbs[i];

	for (i = 0; i < P11R2_CBCOUNT; i++) delete mgt_P11R2cbs[i];

	for (i = 0; i < P11R3_CBCOUNT; i++) delete mgt_P11R3cbs[i];

	for (i = 0; i < P11R4_CBCOUNT; i++) delete mgt_P11R4cbs[i];

	for (i = 0; i < P11R5_CBCOUNT; i++) delete mgt_P11R5cbs[i];

	for (i = 0; i < P12_SWITCHCOUNT; i++) delete mgt_P12switch[i];

	for (i = 0; i < P12_ROTCOUNT; i++) delete mgt_P12Rot[i];

	for (i = 0; i < P12_TWCOUNT; i++) delete mgt_P12thumbwheels[i];

	for (i = 0; i < P12_NEEDLECOUNT; i++) delete mgt_P12needles[i];

	for (i = 0; i < P14_SWITCHCOUNT; i++) delete mgt_P14switch[i];

	for (i = 0; i < P14_ROTCOUNT; i++) delete mgt_P14Rot[i];

	for (i = 0; i < P14_NEEDLECOUNT; i++) delete mgt_P14needles[i];

	for (i = 0; i < P16R1_CBCOUNT; i++) delete mgt_P16R1cbs[i];

	for (i = 0; i < P16R2_CBCOUNT; i++) delete mgt_P16R2cbs[i];

	for (i = 0; i < P16R3_CBCOUNT; i++) delete mgt_P16R3cbs[i];

	for (i = 0; i < P16R4_CBCOUNT; i++) delete mgt_P16R4cbs[i];
}

void LEM::DefineVCAnimations()
{
	UINT mesh = vcidx;
	int i = 0;

	// Panel 1 switches/rotaries/needles
	static UINT meshgroup_P1switches[P1_SWITCHCOUNT], meshgroup_P1Rots[P1_ROTCOUNT], meshgroup_P1needles[P1_NEEDLECOUNT];
	for (int i = 0; i < P1_SWITCHCOUNT; i++)
	{
		meshgroup_P1switches[i] = VC_GRP_Sw_P1_01 + i;
		mgt_P1switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P1switches[i], 1, P1_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P1switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P1switch[i], 0.0f, 1.0f, mgt_P1switch[i]);
	}

	for (i = 0; i < P1_ROTCOUNT; i++)
	{
		meshgroup_P1Rots[i] = VC_GRP_Rot_P1_01 + i;
		mgt_P1Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P1Rots[i], 1, P1_ROT_POS[i], P1_ROT_AXIS, (float)(RAD * 360));
		anim_P1_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P1_Rot[i], 0.0f, 1.0f, mgt_P1Rot[i]);
	}

	for (int i = 0; i < P1_NEEDLECOUNT; i++)
	{
		const VECTOR3 xvector = { 0.00, 0.0625*cos(P1_TILT), 0.0625*sin(P1_TILT) };
		meshgroup_P1needles[i] = VC_GRP_Needle_P1_01 + i;
		mgt_P1needles[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P1needles[i], 1, xvector);
		anim_P1needles[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P1needles[i], 0.0f, 1.0f, mgt_P1needles[i]);
	}

	// Panel 2 switches/rotaries/needles
	static UINT meshgroup_P2switches[P2_SWITCHCOUNT], meshgroup_P2Rots[P2_ROTCOUNT], meshgroup_P2needles[P2_NEEDLECOUNT];
	for (int i = 0; i < P2_SWITCHCOUNT; i++)
	{
		meshgroup_P2switches[i] = VC_GRP_Sw_P2_01 + i;
		mgt_P2switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P2switches[i], 1, P2_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P2switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P2switch[i], 0.0f, 1.0f, mgt_P2switch[i]);
	}

	for (i = 0; i < P2_ROTCOUNT; i++)
	{
		meshgroup_P2Rots[i] = VC_GRP_Rot_P2_01 + i;
		mgt_P2Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P2Rots[i], 1, P2_ROT_POS[i], P2_ROT_AXIS, (float)(RAD * 360));
		anim_P2_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P2_Rot[i], 0.0f, 1.0f, mgt_P2Rot[i]);
	}

	for (int i = 0; i < P2_NEEDLECOUNT; i++)
	{
		const VECTOR3 xvector = { 0.00, 0.0625*cos(P2_TILT), 0.0625*sin(P2_TILT) };
		meshgroup_P2needles[i] = VC_GRP_Needle_P2_01 + i;
		mgt_P2needles[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P2needles[i], 1, xvector);
		anim_P2needles[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P2needles[i], 0.0f, 1.0f, mgt_P2needles[i]);
	}

	// Panel 3 switches/rotaries/needles
	static UINT meshgroup_P3switches[P3_SWITCHCOUNT], meshgroup_P3Rots[P3_ROTCOUNT], meshgroup_P3needles[P3_NEEDLECOUNT];
	for (int i = 0; i < P3_SWITCHCOUNT; i++)
	{
		meshgroup_P3switches[i] = VC_GRP_Sw_P3_01 + i;
		mgt_P3switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3switches[i], 1, P3_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P3switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P3switch[i], 0.0f, 1.0f, mgt_P3switch[i]);
	}

	for (i = 0; i < P3_ROTCOUNT; i++)
	{
		meshgroup_P3Rots[i] = VC_GRP_Rot_P3_01 + i;
		mgt_P3Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P3Rots[i], 1, P3_ROT_POS[i], P3_ROT_AXIS, (float)(RAD * 360));
		anim_P3_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P3_Rot[i], 0.0f, 1.0f, mgt_P3Rot[i]);
	}

	for (int i = 0; i < P3_NEEDLECOUNT; i++)
	{
		const VECTOR3 xvector = { 0.00, 0.0625*cos(P3_TILT), 0.0625*sin(P3_TILT) };
		meshgroup_P3needles[i] = VC_GRP_Needle_P3_01 + i;
		mgt_P3needles[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P3needles[i], 1, xvector);
		anim_P3needles[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P3needles[i], 0.0f, 1.0f, mgt_P3needles[i]);
	}

	// Panel 4 switches
	static UINT meshgroup_P4switches[P4_SWITCHCOUNT];
	for (int i = 0; i < P4_SWITCHCOUNT; i++)
	{
		meshgroup_P4switches[i] = VC_GRP_Sw_P4_01 + i;
		mgt_P4switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P4switches[i], 1, P4_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P4switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P4switch[i], 0.0f, 1.0f, mgt_P4switch[i]);
	}

	// Panel 5 switches/rotaries
	static UINT meshgroup_P5switches[P5_SWITCHCOUNT], meshgroup_P5Rots[P5_ROTCOUNT];
	for (int i = 0; i < P5_SWITCHCOUNT; i++)
	{
		meshgroup_P5switches[i] = VC_GRP_Sw_P5_01 + i;
		mgt_P5switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P5switches[i], 1, P5_TOGGLE_POS[i] - P5_CLICK, _V(1, 0, 0), (float)PI / 4);
		anim_P5switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P5switch[i], 0.0f, 1.0f, mgt_P5switch[i]);
	}

	for (i = 0; i < P5_ROTCOUNT; i++)
	{
		meshgroup_P5Rots[i] = VC_GRP_Rot_P5_01 + i;
		mgt_P5Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P5Rots[i], 1, P5_ROT_POS[i], P5_ROT_AXIS, (float)(RAD * 360));
		anim_P5_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P5_Rot[i], 0.0f, 1.0f, mgt_P5Rot[i]);
	}

	// Panel 6 switches
	static UINT meshgroup_P6switches[P6_SWITCHCOUNT];
	for (int i = 0; i < P6_SWITCHCOUNT; i++)
	{
		meshgroup_P6switches[i] = VC_GRP_Sw_P6_01 + i;
		mgt_P6switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P6switches[i], 1, P6_TOGGLE_POS[i], _V(1, 0, 0), (float)PI / 4);
		anim_P6switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P6switch[i], 0.0f, 1.0f, mgt_P6switch[i]);
	}

	// Panel 8 switches/thumbwheels
	static UINT meshgroup_P8switches[P8_SWITCHCOUNT], meshgroup_P8thumbwheels[P8_TWCOUNT];

	for (int i = 0; i < P8_SWITCHCOUNT; i++)
	{
		meshgroup_P8switches[i] = VC_GRP_Sw_P8_01 + i;
		mgt_P8switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P8switches[i], 1, P8_TOGGLE_POS[i] - P8_CLICK, _V(0, 0, 1), (float)PI / 4);
		anim_P8switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P8switch[i], 0.0f, 1.0f, mgt_P8switch[i]);
	}

	for (i = 0; i < P8_TWCOUNT; i++)
	{
		meshgroup_P8thumbwheels[i] = VC_GRP_TW_P8_01 + i;
		mgt_P8thumbwheels[i] = new MGROUP_ROTATE(mesh, &meshgroup_P8thumbwheels[i], 1, P8_TW_POS[i], _V(0, 0, 1), (float)(RAD * 324));
		anim_P8thumbwheels[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P8thumbwheels[i], 0.0f, 1.0f, mgt_P8thumbwheels[i]);
	}

	// Panel 11 circuit breakers
	static UINT meshgroup_P11R1cb[P11R1_CBCOUNT], meshgroup_P11R2cb[P11R2_CBCOUNT], meshgroup_P11R3cb[P11R3_CBCOUNT], meshgroup_P11R4cb[P11R4_CBCOUNT], meshgroup_P11R5cb[P11R5_CBCOUNT];
	for (int i = 0; i < P11R1_CBCOUNT; i++)
	{
		const VECTOR3 p11row1_vector = { 0.003 * sin(P11R1_TILT - (90.0 * RAD)), 0.003 * -cos(P11R1_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P11R1cb[i] = VC_GRP_CB_P11R1_01 + i;
		mgt_P11R1cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P11R1cb[i], 1, p11row1_vector);
		anim_P11R1cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P11R1cbs[i], 0.0f, 1.0f, mgt_P11R1cbs[i]);
	}

	for (int i = 0; i < P11R2_CBCOUNT; i++)
	{
		const VECTOR3 p11row2_vector = { 0.003 * sin(P11R2_TILT - (90.0 * RAD)), 0.003 * -cos(P11R2_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P11R2cb[i] = VC_GRP_CB_P11R2_01 + i;
		mgt_P11R2cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P11R2cb[i], 1, p11row2_vector);
		anim_P11R2cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P11R2cbs[i], 0.0f, 1.0f, mgt_P11R2cbs[i]);
	}

	for (int i = 0; i < P11R3_CBCOUNT; i++)
	{
		const VECTOR3 p11row3_vector = { 0.003 * sin(P11R3_TILT - (90.0 * RAD)), 0.003 * -cos(P11R3_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P11R3cb[i] = VC_GRP_CB_P11R3_01 + i;
		mgt_P11R3cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P11R3cb[i], 1, p11row3_vector);
		anim_P11R3cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P11R3cbs[i], 0.0f, 1.0f, mgt_P11R3cbs[i]);
	}

	for (int i = 0; i < P11R4_CBCOUNT; i++)
	{
		const VECTOR3 p11row4_vector = { 0.003 * sin(P11R4_TILT - (90.0 * RAD)), 0.003 * -cos(P11R4_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P11R4cb[i] = VC_GRP_CB_P11R4_01 + i;
		mgt_P11R4cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P11R4cb[i], 1, p11row4_vector);
		anim_P11R4cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P11R4cbs[i], 0.0f, 1.0f, mgt_P11R4cbs[i]);
	}

	for (int i = 0; i < P11R5_CBCOUNT; i++)
	{
		const VECTOR3 p11row5_vector = { 0.003 * sin(P11R5_TILT - (90.0 * RAD)), 0.003 * -cos(P11R5_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P11R5cb[i] = VC_GRP_CB_P11R5_01 + i;
		mgt_P11R5cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P11R5cb[i], 1, p11row5_vector);
		anim_P11R5cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P11R5cbs[i], 0.0f, 1.0f, mgt_P11R5cbs[i]);
	}

	// Panel 12 switches/rotaries/thumbwheels/needles
	static UINT meshgroup_P12switches[P12_SWITCHCOUNT], meshgroup_P12Rots[P12_ROTCOUNT], meshgroup_P12thumbwheels[P12_TWCOUNT], meshgroup_P12needles[P12_NEEDLECOUNT];

	for (int i = 0; i < P12_SWITCHCOUNT; i++)
	{
		meshgroup_P12switches[i] = VC_GRP_Sw_P12_01 + i;
		mgt_P12switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P12switches[i], 1, P12_TOGGLE_POS[i] - P12_CLICK, _V(0, 0, -1), (float)PI / 4);
		anim_P12switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P12switch[i], 0.0f, 1.0f, mgt_P12switch[i]);
	}

	for (i = 0; i < P12_ROTCOUNT; i++)
	{
		meshgroup_P12Rots[i] = VC_GRP_Rot_P12_01 + i;
		mgt_P12Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P12Rots[i], 1, P12_ROT_POS[i], P12_ROT_AXIS, (float)(RAD * 360));
		anim_P12_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P12_Rot[i], 0.0f, 1.0f, mgt_P12Rot[i]);
	}

	for (i = 0; i < P12_TWCOUNT; i++)
	{
		meshgroup_P12thumbwheels[i] = VC_GRP_TW_P12_01 + i;
		mgt_P12thumbwheels[i] = new MGROUP_ROTATE(mesh, &meshgroup_P12thumbwheels[i], 1, P12_TW_POS[i], _V(0, 0, -1), (float)(RAD * 324));
		anim_P12thumbwheels[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P12thumbwheels[i], 0.0f, 1.0f, mgt_P12thumbwheels[i]);
	}

	for (i = 0; i < P12_NEEDLECOUNT; i++)
	{
		meshgroup_P12needles[i] = VC_GRP_Needle_P12_01 + i;
		mgt_P12needles[i] = new MGROUP_ROTATE(mesh, &meshgroup_P12needles[i], 1, P12_NEEDLE_POS[i], P12_ROT_AXIS, (float)(RAD * 270));
		anim_P12needles[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P12needles[i], 0.0f, 1.0f, mgt_P12needles[i]);
	}

	// Panel 14 switches/rotaries/needles
	static UINT meshgroup_P14switches[P14_SWITCHCOUNT], meshgroup_P14Rots[P14_ROTCOUNT], meshgroup_P14needles[P14_NEEDLECOUNT];
	for (int i = 0; i < P14_SWITCHCOUNT; i++)
	{
		meshgroup_P14switches[i] = VC_GRP_Sw_P14_01 + i;
		mgt_P14switch[i] = new MGROUP_ROTATE(mesh, &meshgroup_P14switches[i], 1, P14_TOGGLE_POS[i] - P14_CLICK, _V(0, 0, -1), (float)PI / 4);
		anim_P14switch[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P14switch[i], 0.0f, 1.0f, mgt_P14switch[i]);
	}

	for (i = 0; i < P14_ROTCOUNT; i++)
	{
		meshgroup_P14Rots[i] = VC_GRP_Rot_P14_01 + i;
		mgt_P14Rot[i] = new MGROUP_ROTATE(mesh, &meshgroup_P14Rots[i], 1, P14_ROT_POS[i], P14_ROT_AXIS, (float)(RAD * 360));
		anim_P14_Rot[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P14_Rot[i], 0.0f, 1.0f, mgt_P14Rot[i]);
	}

	for (i = 0; i < P14_NEEDLECOUNT; i++)
	{
		meshgroup_P14needles[i] = VC_GRP_Needle_P14_01 + i;
		mgt_P14needles[i] = new MGROUP_ROTATE(mesh, &meshgroup_P14needles[i], 1, P14_NEEDLE_POS[i], P14_ROT_AXIS, (float)(RAD * 270));
		anim_P14needles[i] = CreateAnimation(0.5);
		AddAnimationComponent(anim_P14needles[i], 0.0f, 1.0f, mgt_P14needles[i]);
	}

	// Panel 16 circuit breakers
	static UINT meshgroup_P16R1cb[P16R1_CBCOUNT], meshgroup_P16R2cb[P16R2_CBCOUNT], meshgroup_P16R3cb[P16R3_CBCOUNT], meshgroup_P16R4cb[P16R4_CBCOUNT];
	for (int i = 0; i < P16R1_CBCOUNT; i++)
	{
		const VECTOR3 p11row1_vector = { 0.003 * -sin(P16R1_TILT - (90.0 * RAD)), 0.003 * cos(P16R1_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P16R1cb[i] = VC_GRP_CB_P16R1_01 + i;
		mgt_P16R1cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P16R1cb[i], 1, p11row1_vector);
		anim_P16R1cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P16R1cbs[i], 0.0f, 1.0f, mgt_P16R1cbs[i]);
	}

	for (int i = 0; i < P16R2_CBCOUNT; i++)
	{
		const VECTOR3 p11row2_vector = { 0.003 * -sin(P16R2_TILT - (90.0 * RAD)), 0.003 * cos(P16R2_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P16R2cb[i] = VC_GRP_CB_P16R2_01 + i;
		mgt_P16R2cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P16R2cb[i], 1, p11row2_vector);
		anim_P16R2cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P16R2cbs[i], 0.0f, 1.0f, mgt_P16R2cbs[i]);
	}

	for (int i = 0; i < P16R3_CBCOUNT; i++)
	{
		const VECTOR3 p11row3_vector = { 0.003 * -sin(P16R3_TILT - (90.0 * RAD)), 0.003 * cos(P16R3_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P16R3cb[i] = VC_GRP_CB_P16R3_01 + i;
		mgt_P16R3cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P16R3cb[i], 1, p11row3_vector);
		anim_P16R3cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P16R3cbs[i], 0.0f, 1.0f, mgt_P16R3cbs[i]);
	}

	for (int i = 0; i < P16R4_CBCOUNT; i++)
	{
		const VECTOR3 p11row4_vector = { 0.003 * -sin(P16R4_TILT - (90.0 * RAD)), 0.003 * cos(P16R4_TILT - (90.0 * RAD)), 0.0 };
		meshgroup_P16R4cb[i] = VC_GRP_CB_P16R4_01 + i;
		mgt_P16R4cbs[i] = new MGROUP_TRANSLATE(mesh, &meshgroup_P16R4cb[i], 1, p11row4_vector);
		anim_P16R4cbs[i] = CreateAnimation(0.0);
		AddAnimationComponent(anim_P16R4cbs[i], 0.0f, 1.0f, mgt_P16R4cbs[i]);
	}

	// Thrust-weight indicator
	const VECTOR3 tw_xvector = { 0.00, 0.0925*cos(P1_TILT), 0.0925*sin(P1_TILT) };
	static UINT meshgroup_TW_indicator = VC_GRP_Needle_P1_07;
	static MGROUP_TRANSLATE mgt_TW_indicator(mesh, &meshgroup_TW_indicator, 1, tw_xvector);
	anim_TW_indicator = CreateAnimation(0.0);
	AddAnimationComponent(anim_TW_indicator, 0.0f, 1.0f, &mgt_TW_indicator);

	// Radar strength meter
	static UINT meshgroup_Needle_Radar = VC_GRP_Needle_Radar;
	static MGROUP_ROTATE mgt_Needle_Radar(mesh, &meshgroup_Needle_Radar, 1, _V(-0.264141, 0.235696, 1.62835), P3_ROT_AXIS, (float)(RAD * 270));
	anim_Needle_Radar = CreateAnimation(0.5);
	AddAnimationComponent(anim_Needle_Radar, 0.0f, 1.0f, &mgt_Needle_Radar);

	// CDR & LMP X-pointer
	const VECTOR3 xvector = { 0.00, 0.075*cos(P1_TILT), 0.075*sin(P1_TILT) };
	const VECTOR3 yvector = { 0.075, 0, 0 };
	static UINT meshgroup_XpointerX_cdr = VC_GRP_XpointerX_cdr, meshgroup_XpointerX_lmp = VC_GRP_XpointerX_lmp;
	static UINT meshgroup_XpointerY_cdr = VC_GRP_XpointerY_cdr, meshgroup_XpointerY_lmp = VC_GRP_XpointerY_lmp;
	static MGROUP_TRANSLATE mgt_xpointerx_cdr(mesh, &meshgroup_XpointerX_cdr, 1, xvector);
	static MGROUP_TRANSLATE mgt_xpointery_cdr(mesh, &meshgroup_XpointerY_cdr, 1, yvector);
	static MGROUP_TRANSLATE mgt_xpointerx_lmp(mesh, &meshgroup_XpointerX_lmp, 1, xvector);
	static MGROUP_TRANSLATE mgt_xpointery_lmp(mesh, &meshgroup_XpointerY_lmp, 1, yvector);
	anim_xpointerx_cdr = CreateAnimation(0.5);
	anim_xpointery_cdr = CreateAnimation(0.5);
	anim_xpointerx_lmp = CreateAnimation(0.5);
	anim_xpointery_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_xpointerx_cdr, 0.0f, 1.0f, &mgt_xpointerx_cdr);
	AddAnimationComponent(anim_xpointery_cdr, 0.0f, 1.0f, &mgt_xpointery_cdr);
	AddAnimationComponent(anim_xpointerx_lmp, 0.0f, 1.0f, &mgt_xpointerx_lmp);
	AddAnimationComponent(anim_xpointery_lmp, 0.0f, 1.0f, &mgt_xpointery_lmp);

	// Abort button
	const VECTOR3 abortbuttonvector = { 0.00, 0.004*cos(P1_TILT - (90.0 * RAD)), 0.004*sin(P1_TILT - (90.0 * RAD)) };
	static UINT meshgroup_Abort_Button = VC_GRP_AbortButton;
	static MGROUP_TRANSLATE mgt_Abort_Button(mesh, &meshgroup_Abort_Button, 1, abortbuttonvector);
	anim_abortbutton = CreateAnimation(1.0);
	AddAnimationComponent(anim_abortbutton, 0.0f, 1.0f, &mgt_Abort_Button);

	// Abort stage button
	static UINT  meshgroup_Abortstage_Button = VC_GRP_AbortStageButton;
	static MGROUP_TRANSLATE mgt_Abortstage_Button(mesh, &meshgroup_Abortstage_Button, 1, abortbuttonvector);
	anim_abortstagebutton = CreateAnimation(1.0);
	AddAnimationComponent(anim_abortstagebutton, 0.0f, 1.0f, &mgt_Abortstage_Button);

	// Abort stage cover
	static UINT meshgroup_Abortstage_Cover = VC_GRP_AbortStageCover;
	static MGROUP_ROTATE mgt_Abortstage_Cover(mesh, &meshgroup_Abortstage_Cover, 1, _V(-0.045187, 0.468451, 1.68831), _V(1, 0, 0), (float)(RAD * 100));
	anim_abortstagecover = CreateAnimation(0.0);
	AddAnimationComponent(anim_abortstagecover, 0.0f, 1.0f, &mgt_Abortstage_Cover);

	// RR slew switch
	const VECTOR3 rrslewaxis_x = { -0.00, -sin(P3_TILT + (90.0 * RAD)), cos(P3_TILT + (90.0 * RAD)) };
	static UINT meshgroup_RRslew_switch = VC_GRP_RR_slew_switch;
	static MGROUP_ROTATE mgt_RRslew_SwitchX(mesh, &meshgroup_RRslew_switch, 1, _V(-0.264179, 0.149389, 1.56749), rrslewaxis_x, (float)PI / 4);
	static MGROUP_ROTATE mgt_RRslew_SwitchY(mesh, &meshgroup_RRslew_switch, 1, _V(-0.264179, 0.149389, 1.56749), _V(1, 0, 0), (float)PI / 4);
	anim_rrslewsitch_x = CreateAnimation(0.5);
	anim_rrslewsitch_y = CreateAnimation(0.5);
	AddAnimationComponent(anim_rrslewsitch_x, 0.0f, 1.0f, &mgt_RRslew_SwitchX);
	AddAnimationComponent(anim_rrslewsitch_y, 0.0f, 1.0f, &mgt_RRslew_SwitchY);

	// Stage switch
	static UINT  meshgroup_Stage_Switch = VC_GRP_StageSwitch;
	static MGROUP_ROTATE mgt_Stage_switch(mesh, &meshgroup_Stage_Switch, 1, _V(-0.981771, 0.039868, 1.14749) , _V(0, 0, 1), (float)PI / 4);
	anim_stageswitch = CreateAnimation(0.5);
	AddAnimationComponent(anim_stageswitch, 0.0f, 1.0f, &mgt_Stage_switch);

	// Stage switch cover
	const VECTOR3 stagecoveraxis = { sin((180 * RAD) + P8_TILT), -cos((180 * RAD) + P8_TILT), 0.00 };
	static UINT meshgroup_Stage_Cover = VC_GRP_StageCover;
	static MGROUP_ROTATE mgt_Stage_Cover(mesh, &meshgroup_Stage_Cover, 1, _V(-0.990451, 0.042666, 1.12664), stagecoveraxis, (float)(RAD * 90));
	anim_stagecover = CreateAnimation(0.0);
	AddAnimationComponent(anim_stagecover, 0.0f, 1.0f, &mgt_Stage_Cover);

	// Start button
	const VECTOR3 startbuttonvector = { 0.00, -0.004*cos(P5_TILT - (90.0 * RAD)), -0.004*sin(P5_TILT - (90.0 * RAD)) };
	static UINT meshgroup_Start_Button = VC_GRP_StartButton;
	static MGROUP_TRANSLATE mgt_Start_Button(mesh, &meshgroup_Start_Button, 1, startbuttonvector);
	anim_startbutton = CreateAnimation(0.0);
	AddAnimationComponent(anim_startbutton, 0.0f, 1.0f, &mgt_Start_Button);

	// Stop button CDR
	const VECTOR3 stopbuttonaxis_cdr = { -0.00, sin(P5_TILT + (90.0 * RAD)), -cos(P5_TILT + (90.0 * RAD)) };
	static UINT meshgroup_Stop_Button_cdr = VC_GRP_StopButtonCDR;
	static MGROUP_ROTATE mgt_Stop_Button_cdr(mesh, &meshgroup_Stop_Button_cdr, 1, _V(-0.703027, 0.102277, 1.53733), stopbuttonaxis_cdr, (float)(RAD * 5));
	anim_stopbutton_cdr = CreateAnimation(0.0);
	AddAnimationComponent(anim_stopbutton_cdr, 0.0f, 1.0f, &mgt_Stop_Button_cdr);

	// Stop button LMP
	const VECTOR3 stopbuttonaxis_lmp = { -0.00, sin(P6_TILT + (90.0 * RAD)), -cos(P6_TILT + (90.0 * RAD)) };
	static UINT meshgroup_Stop_Button_lmp = VC_GRP_StopButtonLMP;
	static MGROUP_ROTATE mgt_Stop_Button_lmp(mesh, &meshgroup_Stop_Button_lmp, 1, _V(0.413931, 0.098204, 1.50665), stopbuttonaxis_lmp, (float)(RAD * 5));
	anim_stopbutton_lmp = CreateAnimation(0.0);
	AddAnimationComponent(anim_stopbutton_lmp, 0.0f, 1.0f, &mgt_Stop_Button_lmp);

	// Plus X button
	const VECTOR3 plusxbuttonvector = { 0.00, -0.004*cos(P5_TILT - (90.0 * RAD)), -0.004*sin(P5_TILT - (90.0 * RAD)) };
	static UINT meshgroup_PlusX_Button = VC_GRP_PlusXButton;
	static MGROUP_TRANSLATE mgt_PlusX_Button(mesh, &meshgroup_PlusX_Button, 1, plusxbuttonvector);
	anim_plusxbutton = CreateAnimation(0.0);
	AddAnimationComponent(anim_plusxbutton, 0.0f, 1.0f, &mgt_PlusX_Button);

	InitFDAI(mesh);
}

void LEM::InitFDAI(UINT mesh) {

	// 3D FDAI initialization

	// Constants
	const VECTOR3 fdairollaxis = { -0.00, sin(P1_TILT), -cos(P1_TILT) };
	const VECTOR3 fdaiyawvaxis = { -0.00, sin(P1_TILT + (90.0 * RAD)), -cos(P1_TILT + (90.0 * RAD)) };
	const VECTOR3 needlexvector = { 0.00, 0.05*cos(P1_TILT), 0.05*sin(P1_TILT) };
	const VECTOR3 needleyvector = { 0.05, 0, 0 };
	const VECTOR3 ratexvector = { 0.00, 0.062*cos(P1_TILT), 0.062*sin(P1_TILT) };
	const VECTOR3 rateyvector = { 0.062, 0, 0 };
	const VECTOR3 FDAI_PIVOTCDR = { -0.297851, 0.525802, 1.75639 }; // CDR FDAI Pivot Point
	const VECTOR3 FDAI_PIVOTLMP = { 0.297069, 0.525802, 1.75565 }; // LMP FDAI Pivot Point
	const VECTOR3 attflagvector = { 0.01, 0, 0 };

	// CDR FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_cdr, ach_FDAIpitch_cdr, ach_FDAIyaw_cdr;
	static UINT meshgroup_Fdai1_cdr = { VC_GRP_FDAIBall1_cdr }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_cdr = { VC_GRP_FDAIBall_cdr };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_cdr = { VC_GRP_FDAIBall2_cdr }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_cdr(mesh, &meshgroup_Fdai1_cdr, 1, FDAI_PIVOTCDR, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_cdr(mesh, &meshgroup_Fdai2_cdr, 1, FDAI_PIVOTCDR, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_cdr(mesh, &meshgroup_Fdai3_cdr, 1, FDAI_PIVOTCDR, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_cdr = CreateAnimation(0.0);
	anim_fdaiP_cdr = CreateAnimation(0.0);
	anim_fdaiY_cdr = CreateAnimation(0.0);
	ach_FDAIroll_cdr = AddAnimationComponent(anim_fdaiR_cdr, 0.0f, 1.0f, &mgt_FDAIRoll_cdr);
	ach_FDAIyaw_cdr = AddAnimationComponent(anim_fdaiY_cdr, 0.0f, 1.0f, &mgt_FDAIYaw_cdr, ach_FDAIroll_cdr);
	ach_FDAIpitch_cdr = AddAnimationComponent(anim_fdaiP_cdr, 0.0f, 1.0f, &mgt_FDAIPitch_cdr, ach_FDAIyaw_cdr);

	// CDR FDAI error needles
	static UINT meshgroup_RollError_cdr = VC_GRP_FDAI_rollerror_cdr;
	static UINT meshgroup_PitchError_cdr = VC_GRP_FDAI_pitcherror_cdr;
	static UINT meshgroup_YawError_cdr = VC_GRP_FDAI_yawerror_cdr;
	static MGROUP_TRANSLATE mgt_rollerror_cdr(mesh, &meshgroup_RollError_cdr, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_cdr(mesh, &meshgroup_PitchError_cdr, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_cdr(mesh, &meshgroup_YawError_cdr, 1, needleyvector);
	anim_fdaiRerror_cdr = CreateAnimation(0.5);
	anim_fdaiPerror_cdr = CreateAnimation(0.5);
	anim_fdaiYerror_cdr = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_cdr, 0.0f, 1.0f, &mgt_rollerror_cdr);
	AddAnimationComponent(anim_fdaiPerror_cdr, 0.0f, 1.0f, &mgt_pitcherror_cdr);
	AddAnimationComponent(anim_fdaiYerror_cdr, 0.0f, 1.0f, &mgt_yawerror_cdr);

	// CDR FDAI rate needles
	static UINT meshgroup_RollRate_cdr = VC_GRP_FDAI_rateR_cdr;
	static UINT meshgroup_PitchRate_cdr = VC_GRP_FDAI_rateP_cdr;
	static UINT meshgroup_YawRate_cdr = VC_GRP_FDAI_rateY_cdr;
	static MGROUP_TRANSLATE mgt_rollrate_cdr(mesh, &meshgroup_RollRate_cdr, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_cdr(mesh, &meshgroup_PitchRate_cdr, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_cdr(mesh, &meshgroup_YawRate_cdr, 1, rateyvector);
	anim_fdaiRrate_cdr = CreateAnimation(0.5);
	anim_fdaiPrate_cdr = CreateAnimation(0.5);
	anim_fdaiYrate_cdr = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_cdr, 0.0f, 1.0f, &mgt_rollrate_cdr);
	AddAnimationComponent(anim_fdaiPrate_cdr, 0.0f, 1.0f, &mgt_pitchrate_cdr);
	AddAnimationComponent(anim_fdaiYrate_cdr, 0.0f, 1.0f, &mgt_yawrate_cdr);

	// CDR FDAI no attitude flag
	static UINT meshgroup_attflag_cdr = VC_GRP_FDAIflag_cdr;
	static MGROUP_TRANSLATE mgt_attflag_cdr(mesh, &meshgroup_attflag_cdr, 1, attflagvector);
	anim_attflag_cdr = CreateAnimation(1.0);
	AddAnimationComponent(anim_attflag_cdr, 0.0f, 1.0f, &mgt_attflag_cdr);

	// LMP FDAI Ball
	ANIMATIONCOMPONENT_HANDLE	ach_FDAIroll_lmp, ach_FDAIpitch_lmp, ach_FDAIyaw_lmp;
	static UINT meshgroup_Fdai1_lmp = { VC_GRP_FDAIBall1_lmp }; // Roll gimbal meshgroup (includes roll incicator)
	static UINT meshgroup_Fdai2_lmp = { VC_GRP_FDAIBall_lmp };  // Pitch gimbal meshgroup (visible ball)
	static UINT meshgroup_Fdai3_lmp = { VC_GRP_FDAIBall2_lmp }; // Yaw gimbal meshgroup
	static MGROUP_ROTATE mgt_FDAIRoll_lmp(mesh, &meshgroup_Fdai1_lmp, 1, FDAI_PIVOTLMP, fdairollaxis, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIPitch_lmp(mesh, &meshgroup_Fdai2_lmp, 1, FDAI_PIVOTLMP, _V(-1, 0, 0), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_FDAIYaw_lmp(mesh, &meshgroup_Fdai3_lmp, 1, FDAI_PIVOTLMP, fdaiyawvaxis, (float)(RAD * 360));
	anim_fdaiR_lmp = CreateAnimation(0.0);
	anim_fdaiP_lmp = CreateAnimation(0.0);
	anim_fdaiY_lmp = CreateAnimation(0.0);
	ach_FDAIroll_lmp = AddAnimationComponent(anim_fdaiR_lmp, 0.0f, 1.0f, &mgt_FDAIRoll_lmp);
	ach_FDAIyaw_lmp = AddAnimationComponent(anim_fdaiY_lmp, 0.0f, 1.0f, &mgt_FDAIYaw_lmp, ach_FDAIroll_lmp);
	ach_FDAIpitch_lmp = AddAnimationComponent(anim_fdaiP_lmp, 0.0f, 1.0f, &mgt_FDAIPitch_lmp, ach_FDAIyaw_lmp);

	// LMP FDAI error needles
	static UINT meshgroup_RollError_lmp = VC_GRP_FDAI_rollerror_lmp;
	static UINT meshgroup_PitchError_lmp = VC_GRP_FDAI_pitcherror_lmp;
	static UINT meshgroup_YawError_lmp = VC_GRP_FDAI_yawerror_lmp;
	static MGROUP_TRANSLATE mgt_rollerror_lmp(mesh, &meshgroup_RollError_lmp, 1, needleyvector);
	static MGROUP_TRANSLATE mgt_pitcherror_lmp(mesh, &meshgroup_PitchError_lmp, 1, needlexvector);
	static MGROUP_TRANSLATE mgt_yawerror_lmp(mesh, &meshgroup_YawError_lmp, 1, needleyvector);
	anim_fdaiRerror_lmp = CreateAnimation(0.5);
	anim_fdaiPerror_lmp = CreateAnimation(0.5);
	anim_fdaiYerror_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRerror_lmp, 0.0f, 1.0f, &mgt_rollerror_lmp);
	AddAnimationComponent(anim_fdaiPerror_lmp, 0.0f, 1.0f, &mgt_pitcherror_lmp);
	AddAnimationComponent(anim_fdaiYerror_lmp, 0.0f, 1.0f, &mgt_yawerror_lmp);

	// LMP FDAI rate needles
	static UINT meshgroup_RollRate_lmp = VC_GRP_FDAI_rateR_lmp;
	static UINT meshgroup_PitchRate_lmp = VC_GRP_FDAI_rateP_lmp;
	static UINT meshgroup_YawRate_lmp = VC_GRP_FDAI_rateY_lmp;
	static MGROUP_TRANSLATE mgt_rollrate_lmp(mesh, &meshgroup_RollRate_lmp, 1, rateyvector);
	static MGROUP_TRANSLATE mgt_pitchrate_lmp(mesh, &meshgroup_PitchRate_lmp, 1, ratexvector);
	static MGROUP_TRANSLATE mgt_yawrate_lmp(mesh, &meshgroup_YawRate_lmp, 1, rateyvector);
	anim_fdaiRrate_lmp = CreateAnimation(0.5);
	anim_fdaiPrate_lmp = CreateAnimation(0.5);
	anim_fdaiYrate_lmp = CreateAnimation(0.5);
	AddAnimationComponent(anim_fdaiRrate_lmp, 0.0f, 1.0f, &mgt_rollrate_lmp);
	AddAnimationComponent(anim_fdaiPrate_lmp, 0.0f, 1.0f, &mgt_pitchrate_lmp);
	AddAnimationComponent(anim_fdaiYrate_lmp, 0.0f, 1.0f, &mgt_yawrate_lmp);

	// LMP FDAI no attitude flag
	static UINT meshgroup_attflag_lmp = VC_GRP_FDAIflag_lmp;
	static MGROUP_TRANSLATE mgt_attflag_lmp(mesh, &meshgroup_attflag_lmp, 1, attflagvector);
	anim_attflag_lmp = CreateAnimation(1.0);
	AddAnimationComponent(anim_attflag_lmp, 0.0f, 1.0f, &mgt_attflag_lmp);
}

void LEM::SetCompLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - mat_L01 + 12;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 0.878f;
		mat->emissive.b = 0.506f;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.125f;
		mat->emissive.g = 0.11f;
		mat->emissive.b = 0.064f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetContactLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - mat_L01 + 10;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 0;
		mat->emissive.g = 0.541f;
		mat->emissive.b = 1;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0;
		mat->emissive.g = 0.068f;
		mat->emissive.b = 0.125f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetPowerFailureLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - mat_L01 + 0;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 0;
		mat->emissive.b = 0;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.125f;
		mat->emissive.g = 0;
		mat->emissive.b = 0;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::SetStageSeqRelayLight(int m, bool state) {

	if (!vcmesh)
		return;

	int lightmat = VC_NMAT - mat_L01 + 19;

	MATERIAL *mat = oapiMeshMaterial(hLMVC, lightmat + m);

	if (state == true)
	{   // ON
		mat->emissive.r = 1;
		mat->emissive.g = 1;
		mat->emissive.b = 1;
		mat->emissive.a = 1;
	}
	else
	{   // OFF
		mat->emissive.r = 0.125f;
		mat->emissive.g = 0.125f;
		mat->emissive.b = 0.125f;
		mat->emissive.a = 1;
	}

	oapiSetMaterial(vcmesh, lightmat + m, mat);
}

void LEM::AnimateFDAI(VECTOR3 attitude, VECTOR3 rates, VECTOR3 errors, UINT animR, UINT animP, UINT animY, UINT errorR, UINT errorP, UINT errorY, UINT rateR, UINT rateP, UINT rateY) {

	double fdai_proc[3];
	double rate_proc[3];

	// Drive FDAI ball
	fdai_proc[0] = -attitude.x / PI2; // 1.0 - attitude.x / PI2;
	fdai_proc[1] = attitude.y / PI2;
	fdai_proc[2] = attitude.z / PI2;
	if (fdai_proc[0] < 0) fdai_proc[0] += 1.0;
	if (fdai_proc[1] < 0) fdai_proc[1] += 1.0;
	if (fdai_proc[2] < 0) fdai_proc[2] += 1.0;
	SetAnimation(animY, fdai_proc[2]);
	SetAnimation(animR, fdai_proc[0]);
	SetAnimation(animP, fdai_proc[1]);

	// Drive error needles
	SetAnimation(errorR, (errors.x + 46) / 92);
	SetAnimation(errorP, (-errors.y + 46) / 92);
	SetAnimation(errorY, (errors.z + 46) / 92);

	// Drive rate needles
	rate_proc[0] = (rates.z + 1) / 2;
	rate_proc[1] = (rates.x + 1) / 2;
	rate_proc[2] = (-rates.y + 1) / 2;
	if (rate_proc[0] < 0) rate_proc[0] = 0;
	if (rate_proc[1] < 0) rate_proc[1] = 0;
	if (rate_proc[2] < 0) rate_proc[2] = 0;
	if (rate_proc[0] > 1) rate_proc[0] = 1;
	if (rate_proc[1] > 1) rate_proc[1] = 1;
	if (rate_proc[2] > 1) rate_proc[1] = 1;
	SetAnimation(rateR, rate_proc[0]);
	SetAnimation(rateP, rate_proc[1]);
	SetAnimation(rateY, rate_proc[2]);
}

