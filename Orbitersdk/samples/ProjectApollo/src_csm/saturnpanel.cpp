/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Saturn CSM panel code

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
#include "dsky.h"
#include "csmcomputer.h"
#include "ioChannels.h"

#include "saturn.h"
#include "tracer.h"
#include "Mission.h"

extern GDIParams g_Param;

void BaseInit() 

{
	//
	// need to init device-dependent resources here in case the screen mode has changed
	//

	g_Param.col[2] = oapiGetColour(154, 154, 154);
	g_Param.col[3] = oapiGetColour(3, 3, 3);
	g_Param.col[4] = oapiGetColour(255, 0, 255);
	g_Param.col[5] = oapiGetColour(255, 0, 255);
}

//
//Needle function by Rob Conley from Mercury code
//

void DrawNeedle (HDC hDC, int x, int y, double rad, double angle, HPEN pen0, HPEN pen1)
{
	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;

	oldObj = SelectObject (hDC, pen1);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, oldObj);
	oldObj = SelectObject (hDC, pen0);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
	SelectObject (hDC, oldObj);
}

//
// Altimeter Needle function by Rob Conley from Mercury code, Heavily modified to have non linear gauge range... :):)
//

void Saturn::RedrawPanel_Alt (SURFHANDLE surf)
{
	double alpha;
	double range;
	double press;

	press = GetAtmPressure();
	alpha = GetAltitude();
	alpha = alpha / 0.3048;

#define ALTIMETER_X_CENTER	68
#define ALTIMETER_Y_CENTER	69
#define ALTIMETER_RADIUS	55.0

	//sprintf(oapiDebugString(), "altitude %f", alpha);
	if (alpha > 55000 || press < 1000.0) alpha = 55000;

	if (alpha < 4001){
		range = 120 * RAD;
		range = range / 4000;
		alpha = 4000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+150*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 4001 && alpha < 6001){
		range = 35 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+185*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 6001 && alpha < 8001){
		range = 25 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+165*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 8001 && alpha < 10001){
		range = 30 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+180*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 10001 && alpha < 20001){
		range = 45 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+60*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 20001 && alpha < 40001){
		range = 65 * RAD;
		range = range / 20000;
		alpha = 20000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+15*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else {
		range = 20 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER_X_CENTER, ALTIMETER_Y_CENTER, ALTIMETER_RADIUS, (alpha*range)+10*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	oapiBlt(surf, srf[SRF_ALTIMETER], 0, 0, 0, 0, 137, 137, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_Alt2 (SURFHANDLE surf)
{
	double alpha;
	double range;

	alpha = GetAltitude();
	alpha = alpha / 0.305;

#define ALTIMETER2_X_CENTER	80
#define ALTIMETER2_Y_CENTER	80
#define ALTIMETER2_RADIUS	70.0

	//sprintf(oapiDebugString(), "altitude %f", alpha);
	if (alpha > 50000) alpha = 50000;

	if (alpha < 4001){
		range = 120 * RAD;
		range = range / 4000;
		alpha = 4000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+150*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 4001 && alpha < 6001){
		range = 35 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+185*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 6001 && alpha < 8001){
		range = 25 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+165*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 8001 && alpha < 10001){
		range = 20 * RAD;
		range = range / 2000;
		alpha = 2000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+150*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 10001 && alpha < 20001){
		range = 55 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+70*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else if (alpha > 20001 && alpha < 40001){
		range = 65 * RAD;
		range = range / 20000;
		alpha = 20000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+15*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	else {
		range = 20 * RAD;
		range = range / 10000;
		alpha = 10000 - alpha;
		HDC hDC = oapiGetDC (surf);
		DrawNeedle (hDC, ALTIMETER2_X_CENTER, ALTIMETER2_Y_CENTER, ALTIMETER2_RADIUS, (alpha*range)+10*RAD, g_Param.pen[1], g_Param.pen[4]);//(alpha * range)
		oapiReleaseDC (surf, hDC);
	}
	oapiBlt(surf, srf[SRF_ALTIMETER2], 0, 0, 0, 0, 161, 161, SURF_PREDEF_CK);
}

void Saturn::RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset, int ydist) {

	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, g_Param.font[2]);
	SetTextColor (hDC, RGB(196, 196, 196));
	SetTextAlign (hDC, TA_CENTER);
	SetBkMode (hDC, TRANSPARENT);
	const char *label;
	for (int bt = 0; bt < 6; bt++) {
		if (label = oapiMFDButtonLabel (mfd, bt+side*6))
			TextOut (hDC, 10 + xoffset, 3 + ydist * bt + yoffset, label, strlen(label));
		else break;
	}
	oapiReleaseDC (surf, hDC);
}


//
// Free all allocated surfaces.
//

void Saturn::ReleaseSurfaces ()
{
	for (int i = 0; i < nsurf; i++) {
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
	}
}

//
// Load all the surfaces and initialise the panel.
//

void Saturn::InitPanel (int panel)

{
	//
	// I'm pretty sure that some of these bitmaps aren't used anymore.
	// We need to go through these and delete the unused ones at some
	// point, and remove them from the resource.rc file so as not to
	// bloat the DLL.
	//

	srf[SRF_INDICATOR]								= oapiCreateSurface (LOADBMP (IDB_INDICATOR));
	srf[SRF_NEEDLE]									= oapiCreateSurface (LOADBMP (IDB_NEEDLE));
	srf[SRF_DIGITAL]								= oapiCreateSurface (LOADBMP (IDB_DIGITAL));
	srf[SRF_DIGITAL2]								= oapiCreateSurface (LOADBMP (IDB_DIGITAL2));
	srf[SRF_SWITCHUP]								= oapiCreateSurface (LOADBMP (IDB_SWITCHUP));
	srf[SRF_SWITCHLEVER]							= oapiCreateSurface (LOADBMP (IDB_SWLEVER));
	srf[SRF_SWITCHGUARDS]							= oapiCreateSurface (LOADBMP (IDB_SWITCHGUARDS));
	srf[SRF_SWITCHGUARDPANEL15]						= oapiCreateSurface (LOADBMP (IDB_SWITCHGUARDPANEL15));
	srf[SRF_ABORT]									= oapiCreateSurface (LOADBMP (IDB_ABORT));
	srf[SRF_LV_ENG]									= oapiCreateSurface (LOADBMP (IDB_LV_ENG));
	srf[SRF_ALTIMETER]								= oapiCreateSurface (LOADBMP (IDB_ALTIMETER));
	srf[SRF_THRUSTMETER]							= oapiCreateSurface (LOADBMP (IDB_THRUST));
	srf[SRF_DCVOLTS]								= oapiCreateSurface (LOADBMP (IDB_DCVOLTS));
	srf[SRF_DCVOLTS_PANEL101]						= oapiCreateSurface (LOADBMP (IDB_DCVOLTS_PANEL101));
	srf[SRF_DCAMPS]									= oapiCreateSurface (LOADBMP (IDB_DCAMPS));
	srf[SRF_ACVOLTS]								= oapiCreateSurface (LOADBMP (IDB_ACVOLTS));
	srf[SRF_SEQUENCERSWITCHES]						= oapiCreateSurface (LOADBMP (IDB_SEQUENCERSWITCHES));
	srf[SRF_MASTERALARM_BRIGHT]						= oapiCreateSurface (LOADBMP (IDB_MASTER_ALARM_BRIGHT));
	srf[SRF_DSKY]									= oapiCreateSurface (LOADBMP (IDB_DSKY_LIGHTS));
	srf[SRF_THREEPOSSWITCH]							= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH));
	srf[SRF_MFDFRAME]								= oapiCreateSurface (LOADBMP (IDB_MFDFRAME));
	srf[SRF_MFDPOWER]								= oapiCreateSurface (LOADBMP (IDB_MFDPOWER));
	srf[SRF_SM_RCS_MODE]							= oapiCreateSurface (LOADBMP (IDB_DOCKINGSWITCHES));
	srf[SRF_ROTATIONALSWITCH]						= oapiCreateSurface (LOADBMP (IDB_ROTATIONALSWITCH));
	srf[SRF_SUITCABINDELTAPMETER]					= oapiCreateSurface (LOADBMP (IDB_SUITCABINDELTAPMETER));
	srf[SRF_THREEPOSSWITCH305]						= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH305));
	srf[SRF_THREEPOSSWITCH305LEFT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH305LEFT));
	srf[SRF_SWITCH305LEFT]							= oapiCreateSurface (LOADBMP (IDB_SWITCH305LEFT));
	srf[SRF_DSKYDISP]       						= oapiCreateSurface (LOADBMP (IDB_DSKY_DISP));
	srf[SRF_FDAI]	        						= oapiCreateSurface (LOADBMP (IDB_FDAI));
	srf[SRF_FDAIROLL]       						= oapiCreateSurface (LOADBMP (IDB_FDAI_ROLL));
	srf[SRF_CWSLIGHTS]       						= oapiCreateSurface (LOADBMP (IDB_CWS_LIGHTS));
	srf[SRF_EVENT_TIMER_DIGITS]    					= oapiCreateSurface (LOADBMP (IDB_EVENT_TIMER));
	srf[SRF_DSKYKEY]		    					= oapiCreateSurface (LOADBMP (IDB_DSKY_KEY));
	srf[SRF_ECSINDICATOR]							= oapiCreateSurface (LOADBMP (IDB_ECSINDICATOR));
	srf[SRF_SWITCHUPSMALL]							= oapiCreateSurface (LOADBMP (IDB_SWITCHUPSMALL));
	srf[SRF_CMMFDFRAME]								= oapiCreateSurface (LOADBMP (IDB_CMMFDFRAME));
	srf[SRF_COAS]									= oapiCreateSurface (LOADBMP (IDB_COAS));
	srf[SRF_THUMBWHEEL_SMALLFONTS]					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALLFONTS));
	srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL]			= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALLFONTS_DIAGONAL));
	srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT]	= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT));
	srf[SRF_CIRCUITBRAKER]          				= oapiCreateSurface (LOADBMP (IDB_CIRCUITBRAKER));
	srf[SRF_CIRCUITBRAKER_YELLOW]          			= oapiCreateSurface (LOADBMP (IDB_CIRCUITBRAKER_YELLOW));
	srf[SRF_THREEPOSSWITCH20]						= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH20));
	srf[SRF_THREEPOSSWITCH30]						= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH30));
	srf[SRF_THREEPOSSWITCH30LEFT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH30LEFT));
	srf[SRF_SWITCH20]								= oapiCreateSurface (LOADBMP (IDB_SWITCH20));
	srf[SRF_SWITCH30]								= oapiCreateSurface (LOADBMP (IDB_SWITCH30));
	srf[SRF_SWITCH30LEFT]							= oapiCreateSurface (LOADBMP (IDB_SWITCH30LEFT));
	srf[SRF_SWITCH20LEFT]							= oapiCreateSurface (LOADBMP (IDB_SWITCH20LEFT));
	srf[SRF_THREEPOSSWITCH20LEFT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH20LEFT));
	srf[SRF_GUARDEDSWITCH20]						= oapiCreateSurface (LOADBMP (IDB_GUARDEDSWITCH20));
	srf[SRF_FDAIPOWERROTARY]						= oapiCreateSurface (LOADBMP (IDB_FDAIPOWERROTARY));
	srf[SRF_DIRECTO2ROTARY]							= oapiCreateSurface (LOADBMP (IDB_DIRECTO2ROTARY));
	srf[SRF_ECSGLYCOLPUMPROTARY]					= oapiCreateSurface (LOADBMP (IDB_ECSGLYCOLPUMPROTARY));
	srf[SRF_GTACOVER]								= oapiCreateSurface (LOADBMP (IDB_GTACOVER));
	srf[SRF_POSTLDGVENTVLVLEVER]					= oapiCreateSurface (LOADBMP (IDB_POSTLDGVENTVLVLEVER));
	srf[SRF_SPSMAXINDICATOR]						= oapiCreateSurface (LOADBMP (IDB_SPSMAXINDICATOR));
	srf[SRF_SPSMININDICATOR]						= oapiCreateSurface (LOADBMP (IDB_SPSMININDICATOR));
	srf[SRF_ECSROTARY]								= oapiCreateSurface (LOADBMP (IDB_ECSROTARY));
	srf[SRF_CSM_MNPNL_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_MNPNL_WDW_LES));
	srf[SRF_CSM_RNDZ_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_RNDZ_WDW_LES));
	srf[SRF_CSM_RIGHT_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_RIGHT_WDW_LES));
	srf[SRF_CSM_LEFT_WDW_LES]						= oapiCreateSurface (LOADBMP (IDB_CSM_LEFT_WDW_LES));
	srf[SRF_GLYCOLLEVER]							= oapiCreateSurface (LOADBMP (IDB_GLYCOLLEVER));
	srf[SRF_FDAIOFFFLAG]       						= oapiCreateSurface (LOADBMP (IDB_FDAIOFFFLAG));
	srf[SRF_FDAINEEDLES]							= oapiCreateSurface (LOADBMP (IDB_FDAINEEDLES));
	srf[SRF_THUMBWHEEL_LARGEFONTS]					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_LARGEFONTS));
	srf[SRF_SPS_FONT_WHITE]							= oapiCreateSurface (LOADBMP (IDB_SPS_FUEL_FONT_WHITE));
	srf[SRF_SPS_FONT_BLACK]							= oapiCreateSurface (LOADBMP (IDB_SPS_FUEL_FONT_BLACK));
	srf[SRF_THUMBWHEEL_SMALL]						= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_SMALL));
	srf[SRF_THUMBWHEEL_LARGEFONTSINV] 				= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_LARGEFONTSINV));
	srf[SRF_SWLEVERTHREEPOS] 						= oapiCreateSurface (LOADBMP (IDB_SWLEVERTHREEPOS));
	srf[SRF_ORDEAL_ROTARY] 							= oapiCreateSurface (LOADBMP (IDB_ORDEAL_ROTARY));
	srf[SRF_LV_ENG_S1B]								= oapiCreateSurface (LOADBMP (IDB_LV_ENGINE_LIGHTS_S1B));
	srf[SRF_SPS_INJ_VLV]						    = oapiCreateSurface (LOADBMP (IDB_SPS_INJ_VLV));
	srf[SRF_THUMBWHEEL_GPI_PITCH]  					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_GPI_PITCH));
	srf[SRF_THUMBWHEEL_GPI_YAW]  					= oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_GPI_YAW));
	srf[SRF_THC]				  					= oapiCreateSurface (LOADBMP (IDB_THC));
	srf[SRF_EMS_LIGHTS]			  					= oapiCreateSurface (LOADBMP (IDB_EMS_LIGHTS));
	srf[SRF_SUITRETURN_LEVER]	 					= oapiCreateSurface (LOADBMP (IDB_SUITRETURN_LEVER));
	srf[SRF_CABINRELIEFUPPERLEVER]	 				= oapiCreateSurface (LOADBMP (IDB_CABINRELIEFUPPERLEVER));
	srf[SRF_CABINRELIEFLOWERLEVER]	 				= oapiCreateSurface (LOADBMP (IDB_CABINRELIEFLOWERLEVER));
	srf[SRF_CABINRELIEFGUARDLEVER]	 				= oapiCreateSurface (LOADBMP (IDB_CABINRELIEFGUARDLEVER));
	srf[SRF_OPTICS_HANDCONTROLLER]	 				= oapiCreateSurface (LOADBMP (IDB_OPTICS_HANDCONTROLLER));
	srf[SRF_MARK_BUTTONS]	 						= oapiCreateSurface (LOADBMP (IDB_MARK_BUTTONS));
	srf[SRF_THREEPOSSWITCHSMALL]	 				= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCHSMALL));
	srf[SRF_OPTICS_DSKY]	 						= oapiCreateSurface (LOADBMP (IDB_OPTICS_DSKY));
	srf[SRF_MINIMPULSE_HANDCONTROLLER] 				= oapiCreateSurface (LOADBMP (IDB_MINIMPULSE_HANDCONTROLLER));
	srf[SRF_EMS_SCROLL_LEO]							= oapiCreateSurface (LOADBMP (IDB_EMS_SCROLL_LEO));
	srf[SRF_EMS_SCROLL_BORDER]						= oapiCreateSurface (LOADBMP (IDB_EMS_SCROLL_BORDER));
	srf[SRF_EMS_RSI_BKGRND]                         = oapiCreateSurface (LOADBMP (IDB_EMS_RSI_BKGRND));
	srf[SRF_EMSDVSETSWITCH]							= oapiCreateSurface (LOADBMP (IDB_EMSDVSETSWITCH));
	srf[SRF_ALTIMETER2]								= oapiCreateSurface (LOADBMP (IDB_ALTIMETER2));
	srf[SRF_OXYGEN_SURGE_TANK_VALVE]				= oapiCreateSurface (LOADBMP (IDB_OXYGEN_SURGE_TANK_VALVE));
	srf[SRF_GLYCOL_TO_RADIATORS_KNOB]				= oapiCreateSurface (LOADBMP (IDB_GLYCOL_TO_RADIATORS_KNOB));
	srf[SRF_ACCUM_ROTARY]							= oapiCreateSurface (LOADBMP (IDB_ACCUM_ROTARY));
	srf[SRF_GLYCOL_ROTARY]							= oapiCreateSurface (LOADBMP (IDB_GLYCOL_ROTARY));
	srf[SRF_PRESS_RELIEF_VALVE]						= oapiCreateSurface (LOADBMP (IDB_PRESS_RELIEF_VALVE));
	srf[SRF_CABIN_REPRESS_VALVE]					= oapiCreateSurface (LOADBMP (IDB_CABIN_REPRESS_VALVE));
	srf[SRF_SELECTOR_INLET_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_SELECTOR_INLET_ROTARY));							
	srf[SRF_SELECTOR_OUTLET_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_SELECTOR_OUTLET_ROTARY));
	srf[SRF_EMERGENCY_PRESS_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_EMERGENCY_PRESS_ROTARY));
	srf[SRF_SUIT_FLOW_CONTROL_LEVER]				= oapiCreateSurface (LOADBMP (IDB_CSM_SUIT_FLOW_CONTROL_LEVER));
	srf[SRF_CSM_SEC_CABIN_TEMP_VALVE]				= oapiCreateSurface (LOADBMP (IDB_CSM_SEC_CABIN_TEMP_VALVE));
	srf[SRF_CSM_FOOT_PREP_WATER_LEVER]				= oapiCreateSurface (LOADBMP (IDB_CSM_FOOT_PREP_WATER_LEVER));
	srf[SRF_CSM_LM_TUNNEL_VENT_VALVE]				= oapiCreateSurface (LOADBMP (IDB_CSM_LM_TUNNEL_VENT_VALVE));
	srf[SRF_CSM_WASTE_MGMT_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_CSM_WASTE_MGMT_ROTARY));
	srf[SRF_CSM_DEMAND_REG_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_CSM_DEMAND_REG_ROTARY));
	srf[SRF_CSM_SUIT_TEST_LEVER]					= oapiCreateSurface (LOADBMP (IDB_CSM_SUIT_TEST_LEVER));
	srf[SRF_CSM_GEAR_BOX_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_CSM_GEAR_BOX_ROTARY));
	srf[SRF_CSM_PUMP_HANDLE_ROTARY]					= oapiCreateSurface (LOADBMP (IDB_CSM_PUMP_HANDLE_ROTARY));
	srf[SRF_CSM_VENT_VALVE_HANDLE]					= oapiCreateSurface (LOADBMP (IDB_CSM_VENT_VALVE_HANDLE));
	srf[SRF_CSM_PUMP_HANDLE_ROTARY_OPEN]			= oapiCreateSurface (LOADBMP (IDB_CSM_PUMP_HANDLE_ROTARY_OPEN));
	srf[SRF_CSM_PANEL_351_SWITCH]					= oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_351_SWITCH));
	srf[SRF_CSM_PANEL_600]							= oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_600));
	srf[SRF_CSM_PANEL_600_SWITCH]					= oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_600_SWITCH));
	srf[SRF_CSM_PANEL_382_COVER]					= oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_382_COVER));
	srf[SRF_CSM_WASTE_DISPOSAL_ROTARY]				= oapiCreateSurface (LOADBMP (IDB_CSM_WASTE_DISPOSAL_ROTARY));
	srf[SRF_THREEPOSSWITCH90_LEFT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH90_LEFT));
	srf[SRF_EMS_SCROLL_BUG]							= oapiCreateSurface (LOADBMP (IDB_EMS_SCROLL_BUG));
	srf[SRF_SWITCH90]								= oapiCreateSurface (LOADBMP (IDB_SWITCH90));
	srf[SRF_CSM_CABINPRESSTESTSWITCH]				= oapiCreateSurface (LOADBMP (IDB_CSM_CABINPRESSTESTSWITCH));
	srf[SRF_ORDEAL_PANEL]							= oapiCreateSurface (LOADBMP (IDB_ORDEAL_PANEL));
	srf[SRF_CSM_TELESCOPECOVER]						= oapiCreateSurface (LOADBMP (IDB_CSM_TELESCOPECOVER));	
	srf[SRF_CSM_SEXTANTCOVER]						= oapiCreateSurface (LOADBMP (IDB_CSM_SEXTANTCOVER));
	srf[SRF_CWS_GNLIGHTS]      						= oapiCreateSurface (LOADBMP (IDB_CWS_GNLIGHTS));
	srf[SRF_EVENT_TIMER_DIGITS90]					= oapiCreateSurface (LOADBMP (IDB_EVENT_TIMER90));
	srf[SRF_DIGITAL90]								= oapiCreateSurface (LOADBMP (IDB_DIGITAL90));
	srf[SRF_CSM_PRESS_EQUAL_HANDLE]                 = oapiCreateSurface (LOADBMP (IDB_CSM_PRESS_EQUAL_HANDLE));
	srf[SRF_CSM_PANEL_181]                          = oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_181));
	srf[SRF_CSM_PANEL_277]                          = oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_277));
	srf[SRF_CSM_PANEL_278_CSM112]                   = oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_278_CSM112));
	srf[SRF_CSM_PANEL_278_CSM114]                   = oapiCreateSurface (LOADBMP (IDB_CSM_PANEL_278_CSM114));
	srf[SRF_INDICATOR90]                            = oapiCreateSurface (LOADBMP (IDB_INDICATOR90));
	srf[SRF_THREEPOSSWITCH90_RIGHT]					= oapiCreateSurface (LOADBMP (IDB_THREEPOSSWITCH90_RIGHT));
	srf[SRF_CRYO_SWITCHES_J]                        = oapiCreateSurface (LOADBMP (IDB_CRYO_SWITCHES_J));
	srf[SRF_CRYO_IND_J]                             = oapiCreateSurface (LOADBMP (IDB_CRYO_IND_J));
	srf[SRF_SWITCHGUARDS90_RIGHT]                   = oapiCreateSurface (LOADBMP (IDB_SWITCHGUARDS90_RIGHT));


	//
	// Flashing borders.
	//

	srf[SRF_BORDER_31x31]			= oapiCreateSurface (LOADBMP (IDB_BORDER_31x31));
	srf[SRF_BORDER_34x29]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x29));
	srf[SRF_BORDER_34x61]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x61));
	srf[SRF_BORDER_55x111]			= oapiCreateSurface (LOADBMP (IDB_BORDER_55x111));
	srf[SRF_BORDER_46x75]			= oapiCreateSurface (LOADBMP (IDB_BORDER_46x75));
	srf[SRF_BORDER_39x38]			= oapiCreateSurface (LOADBMP (IDB_BORDER_39x38));
	srf[SRF_BORDER_92x40]			= oapiCreateSurface (LOADBMP (IDB_BORDER_92x40));
	srf[SRF_BORDER_34x33]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x33));
	srf[SRF_BORDER_29x29]			= oapiCreateSurface (LOADBMP (IDB_BORDER_29x29));
	srf[SRF_BORDER_34x31]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x31));
	srf[SRF_BORDER_50x158]			= oapiCreateSurface (LOADBMP (IDB_BORDER_50x158));
	srf[SRF_BORDER_38x52]			= oapiCreateSurface (LOADBMP (IDB_BORDER_38x52));
	srf[SRF_BORDER_34x34]			= oapiCreateSurface (LOADBMP (IDB_BORDER_34x34));
	srf[SRF_BORDER_90x90]			= oapiCreateSurface (LOADBMP (IDB_BORDER_90x90));
	srf[SRF_BORDER_84x84]			= oapiCreateSurface (LOADBMP (IDB_BORDER_84x84));
	srf[SRF_BORDER_70x70]			= oapiCreateSurface (LOADBMP (IDB_BORDER_70x70));
	srf[SRF_BORDER_23x20]			= oapiCreateSurface (LOADBMP (IDB_BORDER_23x20));
	srf[SRF_BORDER_78x78]			= oapiCreateSurface (LOADBMP (IDB_BORDER_78x78));
	srf[SRF_BORDER_32x160]			= oapiCreateSurface (LOADBMP (IDB_BORDER_32x160));
	srf[SRF_BORDER_72x72]			= oapiCreateSurface (LOADBMP (IDB_BORDER_72x72));
	srf[SRF_BORDER_75x64]			= oapiCreateSurface (LOADBMP (IDB_BORDER_75x64));
	srf[SRF_BORDER_58x58]			= oapiCreateSurface (LOADBMP (IDB_BORDER_58x58));
	srf[SRF_BORDER_160x32]			= oapiCreateSurface (LOADBMP (IDB_BORDER_160x32));
	srf[SRF_BORDER_57x57]			= oapiCreateSurface (LOADBMP (IDB_BORDER_57x57));
	srf[SRF_BORDER_47x47]			= oapiCreateSurface (LOADBMP (IDB_BORDER_47x47));
	srf[SRF_BORDER_48x48]			= oapiCreateSurface (LOADBMP (IDB_BORDER_48x48));
	srf[SRF_BORDER_65x65]			= oapiCreateSurface (LOADBMP (IDB_BORDER_65x65));
	srf[SRF_BORDER_87x111]			= oapiCreateSurface (LOADBMP (IDB_BORDER_87x111));
	srf[SRF_BORDER_23x23]			= oapiCreateSurface (LOADBMP (IDB_BORDER_23x23));
	srf[SRF_BORDER_118x118]			= oapiCreateSurface (LOADBMP (IDB_BORDER_118x118));
	srf[SRF_BORDER_38x38]			= oapiCreateSurface (LOADBMP (IDB_BORDER_38x38));
	srf[SRF_BORDER_116x116]			= oapiCreateSurface (LOADBMP (IDB_BORDER_116x116));
	srf[SRF_BORDER_45x36]			= oapiCreateSurface (LOADBMP (IDB_BORDER_45x36));
	srf[SRF_BORDER_17x36]			= oapiCreateSurface (LOADBMP (IDB_BORDER_17x36));
	srf[SRF_BORDER_33x43]			= oapiCreateSurface (LOADBMP (IDB_BORDER_33x43));
	srf[SRF_BORDER_36x17]			= oapiCreateSurface (LOADBMP (IDB_BORDER_36x17));
	srf[SRF_BORDER_38x37]			= oapiCreateSurface (LOADBMP (IDB_BORDER_38x37));
	srf[SRF_BORDER_150x80]			= oapiCreateSurface (LOADBMP (IDB_BORDER_150x80));
	srf[SRF_BORDER_200x80]			= oapiCreateSurface (LOADBMP (IDB_BORDER_200x80));
	srf[SRF_BORDER_72x109]			= oapiCreateSurface (LOADBMP (IDB_BORDER_72x109));
	srf[SRF_BORDER_200x300]			= oapiCreateSurface (LOADBMP (IDB_BORDER_200x300));
	srf[SRF_BORDER_150x200]			= oapiCreateSurface (LOADBMP (IDB_BORDER_150x200));
	srf[SRF_BORDER_240x240]			= oapiCreateSurface (LOADBMP (IDB_BORDER_240x240));
	srf[SRF_BORDER_55x91]			= oapiCreateSurface (LOADBMP (IDB_BORDER_55x91));
	srf[SRF_BORDER_673x369]			= oapiCreateSurface (LOADBMP (IDB_BORDER_673x369));
	srf[SRF_BORDER_673x80]			= oapiCreateSurface (LOADBMP (IDB_BORDER_673x80));
	srf[SRF_BORDER_110x29]			= oapiCreateSurface (LOADBMP (IDB_BORDER_110x29));
	srf[SRF_BORDER_29x30]			= oapiCreateSurface (LOADBMP (IDB_BORDER_29x30));
	srf[SRF_BORDER_62x129]			= oapiCreateSurface (LOADBMP (IDB_BORDER_62x129));
	srf[SRF_BORDER_194x324]			= oapiCreateSurface (LOADBMP (IDB_BORDER_194x324));
	srf[SRF_BORDER_36x69]			= oapiCreateSurface (LOADBMP (IDB_BORDER_36x69));
	srf[SRF_BORDER_62x31]			= oapiCreateSurface (LOADBMP (IDB_BORDER_62x31));
	srf[SRF_BORDER_45x49]			= oapiCreateSurface (LOADBMP (IDB_BORDER_45x49));
	srf[SRF_BORDER_28x32]			= oapiCreateSurface (LOADBMP (IDB_BORDER_28x32));

	//
	// Set color keys where appropriate.
	//

	oapiSetSurfaceColourKey (srf[SRF_NEEDLE],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHLEVER],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHUP],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHGUARDS],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHGUARDPANEL15],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ALTIMETER],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THRUSTMETER],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SEQUENCERSWITCHES],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ROTATIONALSWITCH],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SUITCABINDELTAPMETER],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH305],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH305LEFT],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH305LEFT],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH20],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH20LEFT],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH20],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH20LEFT],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_GUARDEDSWITCH20],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH30],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH30LEFT],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH30],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH30LEFT],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_DSKYDISP],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_FDAI],									g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_FDAIROLL],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHUPSMALL],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_COAS],									g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_SMALLFONTS],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL],		g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT],	g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CIRCUITBRAKER],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CIRCUITBRAKER_YELLOW],					g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_FDAIPOWERROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DIRECTO2ROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ECSGLYCOLPUMPROTARY],					g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_GTACOVER],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_POSTLDGVENTVLVLEVER],					g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SPSMAXINDICATOR],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SPSMININDICATOR],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ECSROTARY],							g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_CSM_MNPNL_WDW_LES],					g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_CSM_RNDZ_WDW_LES],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_RIGHT_WDW_LES],					g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_CSM_LEFT_WDW_LES],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_GLYCOLLEVER],							g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_FDAIOFFFLAG],							g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_FDAINEEDLES],							g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_LARGEFONTS],				g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ACVOLTS],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DCVOLTS],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DCAMPS],								g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_DCVOLTS_PANEL101],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_SMALL],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_LARGEFONTSINV],				g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SWLEVERTHREEPOS],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_ORDEAL_ROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_SPS_INJ_VLV],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_GPI_PITCH],    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THUMBWHEEL_GPI_YAW],    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THC],				    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SUITRETURN_LEVER],	    				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABINRELIEFUPPERLEVER],   				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABINRELIEFLOWERLEVER],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABINRELIEFGUARDLEVER],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_OPTICS_HANDCONTROLLER],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_MARK_BUTTONS],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCHSMALL],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_MINIMPULSE_HANDCONTROLLER],			g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_EMS_SCROLL_BORDER],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ALTIMETER2],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SM_RCS_MODE],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_OXYGEN_SURGE_TANK_VALVE],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_GLYCOL_TO_RADIATORS_KNOB],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_ACCUM_ROTARY],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_GLYCOL_ROTARY],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_PRESS_RELIEF_VALVE],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CABIN_REPRESS_VALVE],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SELECTOR_INLET_ROTARY],				g_Param.col[4]);							
	oapiSetSurfaceColourKey (srf[SRF_SELECTOR_OUTLET_ROTARY],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_EMERGENCY_PRESS_ROTARY],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SUIT_FLOW_CONTROL_LEVER],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_SEC_CABIN_TEMP_VALVE],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_FOOT_PREP_WATER_LEVER],			g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_LM_TUNNEL_VENT_VALVE],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_WASTE_MGMT_ROTARY],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_DEMAND_REG_ROTARY],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_SUIT_TEST_LEVER],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_GEAR_BOX_ROTARY],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PUMP_HANDLE_ROTARY],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_VENT_VALVE_HANDLE],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PUMP_HANDLE_ROTARY_OPEN],			g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PANEL_351_SWITCH],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PANEL_600],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PANEL_600_SWITCH],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PANEL_382_COVER],					g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_WASTE_DISPOSAL_ROTARY],			g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH90_LEFT],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_DSKYKEY],								g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_EMS_SCROLL_BUG],						g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCH90],								g_Param.col[4]);	
	oapiSetSurfaceColourKey (srf[SRF_CSM_CABINPRESSTESTSWITCH],				g_Param.col[4]);	
	oapiSetSurfaceColourKey (srf[SRF_ORDEAL_PANEL],							g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PRESS_EQUAL_HANDLE],               g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CSM_PANEL_181],                        g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_THREEPOSSWITCH90_RIGHT],				g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CRYO_SWITCHES_J],                      g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_CRYO_IND_J],                           g_Param.col[4]);
	oapiSetSurfaceColourKey (srf[SRF_SWITCHGUARDS90_RIGHT],                 g_Param.col[4]);

	//
	// Borders need to set the center color to transparent so only the outline
	// is visible.
	//

	oapiSetSurfaceColourKey	(srf[SRF_BORDER_31x31],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x29],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x61],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_55x111],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_46x75],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_39x38],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_92x40],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x33],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_29x29],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x31],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_50x158],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_38x52],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x34],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_90x90],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_84x84],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_70x70],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_23x20],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_78x78],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_32x160],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_72x72],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_75x64],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_58x58],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_160x32],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_57x57],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_47x47],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_48x48],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_65x65],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_87x111],	g_Param.col[4]);			
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_23x23],		g_Param.col[4]);			
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_118x118],	g_Param.col[4]);			
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_116x116],	g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_45x36],		g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_17x36],		g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_33x43],		g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_36x17],		g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_38x37],		g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_38x38],		g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_150x80],	g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_200x80],	g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_72x109],	g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_200x300],	g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_150x200],	g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_240x240],	g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_55x91],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_673x369],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_673x80],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_110x29],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_29x30],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_62x129],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_194x324],	g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_36x69],	    g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_62x31],		g_Param.col[4]);
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_45x49],		g_Param.col[4]);	
	oapiSetSurfaceColourKey	(srf[SRF_BORDER_28x32],		g_Param.col[4]);	

	SetSwitches(panel);
}

int Saturn::GetRenderViewportIsWideScreen() {

	HMODULE hpac = GetModuleHandle("Modules\\Startup\\ProjectApolloConfigurator.dll");
	if (hpac) {
		int (*pacRenderViewportIsWideScreen)();
		pacRenderViewportIsWideScreen = (int (*)()) GetProcAddress(hpac, "pacRenderViewportIsWideScreen");
		if (pacRenderViewportIsWideScreen) {
			return pacRenderViewportIsWideScreen();
		}
	}
	return 0;
}

bool Saturn::clbkLoadPanel (int id) {

	TRACESETUP("Saturn::clbkLoadPanel");

	//
	// Release all surfaces
	//
	ReleaseSurfaces();

	//
	// Should we display a panel for unmanned flights?
	//
	if (!Crewed)
		return false;

	//
	// No panel in engineering camera view.
	//
	if (viewpos == SATVIEW_ENG1 || viewpos == SATVIEW_ENG2 || viewpos == SATVIEW_ENG3)
		return false;

	//
	// Get screen info from the configurator
	//
	int renderViewportIsWideScreen = GetRenderViewportIsWideScreen();

	//
	// Load panel background image
	//
	HBITMAP hBmp;

	if ((id == SATPANEL_GN && !GNSplit) || (id == SATPANEL_GN_LEFT && !GNSplit) || (id == SATPANEL_GN_RIGHT && !GNSplit)) { // guidance & navigation lower equipment bay (unsplit)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_RIGHT_CB, SATPANEL_TUNNEL, SATPANEL_TELESCOPE);

		AddLeftLowerPanelAreas();
		AddLeftCenterLowerPanelAreas(0);
		AddCenterLowerPanelAreas(0);
		AddRightCenterLowerPanelAreas(0);
		AddRightLowerPanelAreas(0);

		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}
	if (id == SATPANEL_GN_LEFT && GNSplit) { // guidance & navigation lower equipment bay Left third (split)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL_LEFT));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(-1, SATPANEL_GN_CENTER, SATPANEL_LOWER_LEFT, SATPANEL_TELESCOPE);

		AddLeftLowerPanelAreas();
		AddLeftCenterLowerPanelAreas(0);
		
		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_GN_CENTER && GNSplit) { // guidance & navigation lower equipment bay Center third (split)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL_CENTER));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_GN_LEFT, SATPANEL_GN_RIGHT, SATPANEL_TUNNEL, SATPANEL_TELESCOPE);

		AddLeftCenterLowerPanelAreas(-1019);
		AddCenterLowerPanelAreas(-1019);
		AddRightCenterLowerPanelAreas(-1019);
		

		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_GN_RIGHT && GNSplit) { // guidance & navigation lower equipment bay Right third (split)
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_PANEL_RIGHT));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_GN_CENTER, -1, SATPANEL_RIGHT_CB, SATPANEL_TELESCOPE);

		AddRightCenterLowerPanelAreas(-2397);
		AddRightLowerPanelAreas(-2397);
		
		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (MainPanelSplit && id == SATPANEL_MAIN_LEFT) {
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_LEFT_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_LEFT, SATPANEL_MAIN_MIDDLE, SATPANEL_LEFT_RNDZ_WINDOW, SATPANEL_LOWER_LEFT);

		AddLeftMainPanelAreas();
		AddLeftMiddleMainPanelAreas(0);

		// Dummy 1px MFDs in order to force Orbiter to load the MFD data from the scenario (needs to be 2px otherwise MapMFD crashes)
		MFDSPEC mfds_user1 = {{ 0, 0, 2, 1}, 0, 0, 0, 0};
		MFDSPEC mfds_user2 = {{ 0, 0, 2, 1}, 0, 0, 0, 0};
		MFDSPEC mfds_right = {{ 0, 0, 2, 1}, 0, 0, 0, 0};
		oapiRegisterMFD(MFD_USER1, mfds_user1);
		oapiRegisterMFD(MFD_USER2, mfds_user2);
		oapiRegisterMFD(MFD_RIGHT, mfds_right);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (MainPanelSplit && id == SATPANEL_MAIN_MIDDLE) {
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_MIDDLE_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_MAIN_LEFT, SATPANEL_MAIN_RIGHT, SATPANEL_HATCH_WINDOW, SATPANEL_LOWER_MAIN);

		AddLeftMiddleMainPanelAreas(-1022);
		AddRightMiddleMainPanelAreas(-1022);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (MainPanelSplit && id == SATPANEL_MAIN_RIGHT) {

		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_RIGHT_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_MAIN_MIDDLE, SATPANEL_RIGHT, SATPANEL_RIGHT_RNDZ_WINDOW, SATPANEL_RIGHT_CB);
		
		AddRightMiddleMainPanelAreas(-1772);
		AddRightMainPanelAreas(-1772);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (!MainPanelSplit && id == SATPANEL_MAIN) { // main instrument panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_MAIN_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiSetPanelNeighbours(SATPANEL_LEFT, SATPANEL_RIGHT, SATPANEL_HATCH_WINDOW, SATPANEL_LOWER_MAIN);
		
		AddLeftMainPanelAreas();
		AddLeftMiddleMainPanelAreas(0);
		AddRightMiddleMainPanelAreas(0);
		AddRightMainPanelAreas(0);

		// Dummy 1px MFDs in order to force Orbiter to load the MFD data from the scenario (needs to be 2px otherwise MapMFD crashes)
		MFDSPEC mfds_user1 = {{ 0, 0, 2, 1}, 0, 0, 0, 0};
		MFDSPEC mfds_user2 = {{ 0, 0, 2, 1}, 0, 0, 0, 0};
		oapiRegisterMFD(MFD_USER1, mfds_user1);
		oapiRegisterMFD(MFD_USER2, mfds_user2);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LEFT) { // left instrument panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_MAIN_LEFT, -1, SATPANEL_LOWER_LEFT);
		else
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_MAIN, -1, SATPANEL_LOWER_LEFT);

		oapiRegisterPanelArea (AID_CSM_LEFT_WDW_LES,							_R( 552,  243, 1062,  733), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_LEFTCOASSWITCH,								_R(1316,   63, 1350,   94), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTTUTILITYPOWERSWITCH,						_R(1425,   81, 1459,  112), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POSTLANDINGBCNLTSWITCH,						_R(1468,   88, 1502,  119), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POSTLANDINGDYEMARKERSWITCH,		    		_R(1508,   71, 1554,  146), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POSTLANDINGVENTSWITCH,						_R(1592,  109, 1626,  140), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMODEINTERCOMVOXSENSTHUMBWHEEL,			_R(1383,  304, 1416,  347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTPOWERMASTERVOLUMETHUMBWHEEL,				_R(1506,  279, 1539,  322), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTPADCOMMVOLUMETHUMBWHEEL,					_R(1340,  411, 1373,  454), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTINTERCOMVOLUMETHUMBWHEEL,				_R(1461,  392, 1494,  435), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTSBANDVOLUMETHUMBWHEEL,					_R(1297,  519, 1330,  562), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTVHFVOLUMETHUMBWHEEL,						_R(1418,  499, 1451,  542), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMODEINTERCOMSWITCH,						_R(1336,  292, 1370,  326), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTAUDIOPOWERSWITCH,						_R(1551,  304, 1585,  338), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTPADCOMMSWITCH,							_R(1293,  398, 1327,  432), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTINTERCOMSWITCH,							_R(1506,  417, 1540,  451), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTSBANDSWITCH,								_R(1250,  507, 1284,  541), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTVHFAMSWITCH,								_R(1463,  523, 1497,  557), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTAUDIOCONTROLSWITCH,						_R(1255,  632, 1289,  666), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTSUITPOWERSWITCH,							_R(1320,  658, 1354,  692), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_VHFRNGSWITCH,								_R(1385,  684, 1419,  718), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_STABCONTCIRCUITBREAKERS,						_R( 433,  898,  597,  927), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_AUTORCSSELECTSWITCHES,						_R( 659,  893, 1368,  922), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS,	_R( 454, 1001, 1015, 1030), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS2,	_R( 473, 1082,  844, 1111), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOODDIMSWITCH,								_R( 898, 1071,  932, 1100), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOODFIXEDSWITCH,							_R( 943, 1071,  977, 1100), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_REACTIONCONTROLSYSTEMCIRCUITBREAKERS,		_R( 494, 1161, 1017, 1190), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOATBAGSWITCHES,							_R(1037, 1123, 1191, 1175), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SEQEVENTSCONTSYSTEM,							_R(1200, 1123, 1371, 1175), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SERVICEPROPULSIONSYSCIRCUITBREAKERS,			_R( 505, 1243, 1052, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOATBAGCIRCUITBREAKERS,						_R(1062, 1243, 1167, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SEQEVENTSCONTSYSCIRCUITBREAKERS,				_R(1176, 1243, 1319, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EDSCIRCUITBREAKERS,							_R( 559, 1323,  664, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ELSCIRCUITBREAKERS,							_R( 673, 1323,  740, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PLVENTCIRCUITBREAKER,						_R( 749, 1323,  778, 1352), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EDSPOWERSWITCH,								_R(1099, 1399, 1133, 1432), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TVCSERVOPOWERSWITCHES,						_R( 996, 1432, 1055, 1500), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LOGICPOWERSWITCH,							_R( 941, 1505,  975, 1538), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SIGCONDDRIVERBIASPOWERSWITCHES,				_R( 857, 1546,  916, 1614), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTINTERIORLIGHTROTARIES,					_R(1032,  989, 1360, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FDAIPOWERROTARY,								_R( 866, 1401,  956, 1491), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SCSELECTRONICSPOWERROTARY,					_R( 739, 1408,  829, 1498), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BMAGPOWERROTARY1,							_R( 600, 1419,  690, 1509), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BMAGPOWERROTARY2,							_R( 666, 1511,  756, 1601), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DIRECTO2ROTARY,								_R( 765, 1575,  835, 1645), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCIRCUITRETURNVALVE,						_R(  65, 1252,  225, 1285), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CSM_SUIT_TEST_LEVER,							_R( 206, 1334,  400, 1658), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_CSM_DEMAND_REG_ROTARY,						_R( 134, 1438,  204, 1508), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_OXYGEN_SURGE_TANK_VALVE,						_R( 121,  201,  155,  235), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(-1.0, 0.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_RIGHT) { // right instrument panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_MAIN_RIGHT, -1, -1, SATPANEL_RIGHT_CB);
		else
			oapiSetPanelNeighbours(SATPANEL_MAIN, -1, -1, SATPANEL_RIGHT_CB);

		oapiRegisterPanelArea (AID_CSM_RIGHT_WDW_LES,							_R( 620,  243, 1130,  733), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_FUELCELLPUMPSSWITCHES,      					_R( 311,  881,  475,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TELCOMSWITCHES,								_R( 672, 1416,  762, 1527), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSWITCHES,      				_R( 825, 1428,  901, 1519), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSSWITCH,						_R( 734, 1525,  824, 1615), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EPSSENSORSIGNALDCCIRCUITBRAKERS,				_R( 856,  871,  923,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EPSSENSORSIGNALACCIRCUITBRAKERS,				_R( 962,  871, 1092,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CWCIRCUITBRAKERS,				            _R(1130,  871, 1197,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMPWRCIRCUITBRAKERS,				            _R(1206,  871, 1238,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INVERTERCONTROLCIRCUITBRAKERS,				_R( 856,  949,  961,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EPSSENSORUNITCIRCUITBRAKERS,					_R( 987,  949, 1127,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BATRLYBUSCIRCUITBRAKERS,					    _R(1135,  949, 1201,  978), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSRADIATORSCIRCUITBRAKERS,					_R( 680, 1032,  955, 1061), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BATTERYCHARGERCIRCUITBRAKERS,				_R( 963, 1032, 1184, 1061), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INSTRUMENTLIGHTINGCIRCUITBRAKERS,			_R( 312, 1121,  588, 1150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSCIRCUITBRAKERS,			                _R( 616, 1121, 1191, 1150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSLOWERROWCIRCUITBRAKERS,			        _R( 358, 1206, 1158, 1235), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GUIDANCENAVIGATIONCIRCUITBRAKERS,			_R( 389, 1288,  780, 1317), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC1ACIRCUITBRAKER,			_R( 821, 1634,  850, 1663), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC1BCIRCUITBRAKER,			_R( 843, 1603,  872, 1632), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC1CCIRCUITBRAKER,			_R( 865, 1572,  894, 1601), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC2ACIRCUITBRAKER,			_R( 899, 1525,  928, 1554), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC2BCIRCUITBRAKER,			_R( 921, 1494,  950, 1523), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCOMPRESSORSAC2CCIRCUITBRAKER,			_R( 943, 1463,  972, 1492), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC1ACIRCUITBRAKER,				_R( 889, 1658,  918, 1687), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC1BCIRCUITBRAKER,				_R( 912, 1626,  941, 1655), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC1CCIRCUITBRAKER,				_R( 935, 1594,  964, 1623), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC2ACIRCUITBRAKER,				_R( 957, 1562,  986, 1591), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC2BCIRCUITBRAKER,				_R( 980, 1531, 1009, 1560), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ECSGLYCOLPUMPSAC2CCIRCUITBRAKER,				_R(1003, 1499, 1032, 1528), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTCOASSWITCH,								_R( 330,   63,  364,   94), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MODEINTERCOMVOXSENSTHUMBWHEEL,				_R( 138,  280,  171,  323), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_POWERMASTERVOLUMETHUMBWHEEL,					_R( 262,  299,  295,  342), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PADCOMMVOLUMETHUMBWHEEL,						_R( 181,  387,  214,  430), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INTERCOMVOLUMETHUMBWHEEL,					_R( 307,  412,  340,  455), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SBANDVOLUMETHUMBWHEEL,						_R( 224,  496,  257,  539), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_VHFVOLUMETHUMBWHEEL,							_R( 350,  519,  383,  562), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_AUDIOCONTROLSWITCH,							_R( 347,  649,  381,  683), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITPOWERSWITCH,								_R( 403,  627,  437,  661), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTUTILITYPOWERSWITCH,						_R( 221,   81,  255,  112), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTDOCKINGTARGETSWITCH,					_R(  54,  109,   88,  140), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTMODEINTERCOMSWITCH,						_R(  92,  305,  126,  339), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTAUDIOPOWERSWITCH,						_R( 308,  287,  342,  321), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTPADCOMMSWITCH,							_R( 135,  411,  169,  445), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTINTERCOMSWITCH,							_R( 353,  400,  387,  434), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTSBANDSWITCH,							_R( 178,  520,  212,  554), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTVHFAMSWITCH,							_R( 396,  507,  430,  541), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GNPOWERSWITCH,								_R( 506,  881,  540,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAINBUSTIESWITCHES,							_R( 608,  881,  687,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_BATTERYCHARGERSWITCH,						_R( 698,  881,  732,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_NONESSBUSSWITCH,								_R( 763,  881,  797,  910), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_INTERIORLIGHTSFLOODSSWITCHES,				_R( 570, 1003,  649, 1032), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SPSGAUGINGSWITCH,							_R( 626, 1401,  660, 1434), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTINTERIORLIGHTROTARIES,					_R( 319,  974,  542, 1064), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL275CIRCUITBRAKERS,				        _R(1467, 1092, 1496, 1464), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL275CIRCUITBRAKERS_LOWER,				_R(1467, 1494, 1496, 1717), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL276,									_R(1399,  878, 1490,  996), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL278,									_R(1422,  549, 1527,  628), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL227,									_R(1237,  206, 1267,  236), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL229CIRCUITBRAKERS,						_R( 288, 1550,  456, 1678), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);

		if (Panel277)
		{
			oapiRegisterPanelArea(AID_CSM_PANEL_277, _R(1356, 662, 1511, 857), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		}

		// Panel 278 Mission-Specific Additions

		if (Panel278J)
		{
			oapiRegisterPanelArea(AID_CSM_PANEL_278J, _R(1356, 114, 1544, 547), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		}
		
		SetCameraDefaultDirection(_V(1.0, 0.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LEFT_RNDZ_WINDOW) { // left rendezvous window
		int xoffset = 0, xoffset1 = 0, yoffset = 0, yoffset1 = 0, yoffset2 = 0;
		if (renderViewportIsWideScreen == 1) {			
			xoffset = 200;
			yoffset = 5;
			xoffset1 = 400;
			yoffset1 = 26;
			yoffset2 = 19;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_RNDZ_WINDOW_WIDE));
		} else if (renderViewportIsWideScreen == 2) {
			xoffset = 293;
			yoffset = 5;
			xoffset1 = 586;
			yoffset1 = 26;
			yoffset2 = 19;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_RNDZ_WINDOW_16_9));
		} else {
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LEFT_RNDZ_WINDOW));
		}
		if (!hBmp) {
			return false;
		}

		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_LEFT_317_WINDOW, SATPANEL_HATCH_WINDOW, -1, SATPANEL_MAIN_LEFT);
		else
			oapiSetPanelNeighbours(SATPANEL_LEFT_317_WINDOW, SATPANEL_HATCH_WINDOW, -1, SATPANEL_MAIN);

		MFDSPEC mfds_dock = {{1019 + xoffset1, 784 + yoffset1, 1238 + xoffset1, 999 + yoffset1}, 6, 6, 31, 31};
        oapiRegisterMFD (MFD_RIGHT, mfds_dock);	// MFD_USER1

		oapiRegisterPanelArea (AID_MFDDOCK,	        _R( 979 + xoffset1,  773 + yoffset1, 1280 + xoffset1, 1024 + yoffset1), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MFDDOCK_POWER,   _R( 958 + xoffset , 1004 + yoffset,   978 + xoffset,  1019 + yoffset),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,				       PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SM_RCS_MODE,     _R(1205 + xoffset1,  700 + yoffset1, 1280 + xoffset1,  773 + yoffset1), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,					   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_COAS,		    _R( 527 + xoffset,     0 + yoffset,  1210 + xoffset,   627 + yoffset),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,					   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ALTIMETER2,		_R( 788 + xoffset,   860 + yoffset2,  949 + xoffset,  1021 + yoffset2), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,					   PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LEFT_317_WINDOW) { // left 31.7 degree line window

		hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_CSM_LEFT_317_WINDOW));

		if (!hBmp) {
			return false;
		}

		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);
		oapiSetPanelNeighbours(-1, SATPANEL_LEFT_RNDZ_WINDOW, -1, SATPANEL_MAIN);

		SetCameraDefaultDirection(_V(0.0, 0.5254716511, 0.8508111094));
		oapiCameraSetCockpitDir(0, 0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_RIGHT_RNDZ_WINDOW) { // right rendezvous window
		int xoffset = 0, yoffset = 0;
		if (renderViewportIsWideScreen == 1) {			
			xoffset = 200;
			yoffset = 13;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_RNDZ_WINDOW_WIDE));
		} else if (renderViewportIsWideScreen == 2) {
			xoffset = 293;
			yoffset = 13;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_RNDZ_WINDOW_16_9));
		} else {
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_RNDZ_WINDOW));
		}
		if (!hBmp) {
			return false;
		}

		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_HATCH_WINDOW, -1, -1, SATPANEL_MAIN_RIGHT);
		else
			oapiSetPanelNeighbours(SATPANEL_HATCH_WINDOW, -1, -1, SATPANEL_MAIN);

		oapiRegisterPanelArea (AID_CSM_RNDZ_WDW_LES, _R( 463 + xoffset, 102 + yoffset, 1105 + xoffset, 840 + yoffset), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_HATCH_WINDOW) { // hatch window
		int xoffset = 0;
		if (SideHatch.IsOpen()) {
			if (renderViewportIsWideScreen == 1) {			
				xoffset = 205;
				hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_HATCH_WINDOW_OPEN_WIDE));
			} else if (renderViewportIsWideScreen == 2) {
				xoffset = 205 + 137;
				hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_HATCH_WINDOW_OPEN_16_9));
			} else {
				hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_HATCH_WINDOW_OPEN));
			}
		} else {
			xoffset = 360;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_HATCH_WINDOW));
		}
		if (!hBmp) {
			return false;
		}

		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_MOVEOUT_LEFT|PANEL_ATTACH_RIGHT,  g_Param.col[4]);

		if (MainPanelSplit) 
			oapiSetPanelNeighbours(SATPANEL_LEFT_RNDZ_WINDOW, SATPANEL_RIGHT_RNDZ_WINDOW, -1, SATPANEL_MAIN_MIDDLE);
		else
			oapiSetPanelNeighbours(SATPANEL_LEFT_RNDZ_WINDOW, SATPANEL_RIGHT_RNDZ_WINDOW, -1, SATPANEL_MAIN);

		if (SideHatch.IsOpen()) {
			oapiRegisterPanelArea (AID_CSM_PUMP_HANDLE_ROTARY_OPEN,		_R(874 + xoffset, 163, 1024 + xoffset, 363), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CSM_HATCH_TOGGLE,				_R(750 + xoffset, 400,  950 + xoffset, 700), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

			oapiRegisterPanelArea (AID_CSM_HATCH_600_LEFT,				_R(  0          ,  54,  150          , 768), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CSM_HATCH_600_RIGHT,				_R(875 + xoffset, 466, 1024 + xoffset, 768), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		} else {
			oapiRegisterPanelArea (AID_CSM_GEAR_BOX_ROTARY,				_R(1405 + xoffset,  50, 1649 + xoffset,  290), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CSM_PUMP_HANDLE_ROTARY,			_R(1275 + xoffset, 534, 1515 + xoffset,  774), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CSM_VENT_VALVE_HANDLE,			_R(  64 + xoffset, 703,  328 + xoffset,  967), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CSM_HATCH_TOGGLE,				_R(1340 + xoffset, 790, 1540 + xoffset, 1090), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

			oapiRegisterPanelArea (AID_CSM_HATCH_600_LEFT,				_R(   0 + xoffset,  636,   150 + xoffset, 1350), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea (AID_CSM_HATCH_600_RIGHT,				_R(1530 + xoffset, 1047,  1679 + xoffset, 1349), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

		}
		SetCameraDefaultDirection(_V(0.0, 0.83867, 0.544639));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_CABIN_PRESS_PANEL) { // cabin pressurization controls panel
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_CABIN_PRESS_PANEL));
		if (!hBmp) {
			return false;
		}

		oapiSetPanelNeighbours(-1, SATPANEL_LEFT, -1, SATPANEL_GN);
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		oapiRegisterPanelArea (AID_GLYCOLTORADIATORSLEVER,			_R(1488,   46, 1520,  206), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CABINPRESSURERELIEFLEVER1,		_R(1544,  412, 1695,  492), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CABINPRESSURERELIEFLEVER2,		_R(1431,  547, 1697,  635), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GLYCOLRESERVOIRROTARIES,			_R(1496,  705, 1574,  995), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OXYGENROTARIES,					_R(1498, 1146, 1788, 1224), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ORDEALSWITCHES,					_R( 448,    1,  925,  203), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OXYGEN_SURGE_TANK_VALVE,			_R(1150,  201, 1184,  235), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GLYCOL_TO_RADIATORS_KNOB,		_R( 273,  362,  304,  393), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITCIRCUITRETURNVALVE,			_R(1094, 1252, 1254, 1284), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_CSM_SUIT_TEST_LEVER,				_R(1235, 1334, 1429, 1658), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_CSM_DEMAND_REG_ROTARY,			_R(1163, 1438, 1233, 1508), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);					
		oapiRegisterPanelArea (AID_GLYCOL_ROTARY,					_R( 117,  978,  189, 1050), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ACCUM_ROTARY,					_R( 669, 1025,  727, 1083), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PLVC_SWITCH,						_R( 999,  110, 1028,  141), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,						PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_PANEL_352,						_R(  91, 2973,  390, 3200), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CABIN_REPRESS_VALVE,				_R( 612, 3096,  660, 3144), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_WATER_GLYCOL_TANKS_ROTARIES,		_R(1001, 2965, 1085, 3182), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EMERGENCY_CABIN_PRESSURE_ROTARY,	_R( 773, 3130,  838, 3195), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_REGULATOR_SWITCHES,			_R(1197, 3037, 1307, 3145), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CSM_PANEL_382,					_R( 112, 1198,  785, 1567), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,						PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CSM_CABINPRESSTESTSWITCH,		_R( 785, 3220,  825, 3260), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,						PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(-1.0, 0.0, 0.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LOWER_LEFT) { 
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_LOWER_LEFT_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (GNSplit) 
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_TUNNEL, SATPANEL_LEFT, SATPANEL_GN_LEFT);
		else
			oapiSetPanelNeighbours(SATPANEL_CABIN_PRESS_PANEL, SATPANEL_TUNNEL, SATPANEL_LEFT, SATPANEL_GN);

		///////////////////////////
		// Panel 300/301/302/303 //
		///////////////////////////
		
		oapiRegisterPanelArea (AID_SUIT_FLOW_CONTROL_LEVER_300,			_R(1356,  146, 1443,  257), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUIT_FLOW_CONTROL_LEVER_301,			_R( 998,  146, 1085,  257), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUIT_FLOW_CONTROL_LEVER_302,			_R( 998,  396, 1085,  507), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CSM_SEC_CABIN_TEMP_VALVE,			_R(1105,  775, 1128,  798), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FOOD_PREPARATION_WATER,				_R(1164, 1044, 1419, 1162), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CSM_PANEL_306,						_R(1093, 1410, 1123, 1720), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CSM_PANEL_306_MISSIONTIMERSWITCH,	_R(1211, 1619, 1240, 1650), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EVENT_TIMER306,						_R(1185, 1431, 1203, 1502), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MISSION_CLOCK306,					_R(1302, 1411, 1325, 1554), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);


		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_LOWER_MAIN) {
		hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_CSM_LOWER_MAIN_PANEL));

		if (!hBmp)
		{
			return false;
		}

		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		int top;

		if (MainPanelSplit)
			top = SATPANEL_MAIN_MIDDLE;
		else
			top = SATPANEL_MAIN;

		oapiSetPanelNeighbours(SATPANEL_LOWER_LEFT, SATPANEL_RIGHT_CB, top, SATPANEL_TUNNEL);

		/////////////////
		// Panel 10/12 //
		/////////////////

		oapiRegisterPanelArea(AID_PANEL10_LEFT_SWITCHES,               _R(774,  476,  808, 731), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_PANEL10_LEFT_THUMWBWHEELS,           _R(836,  472,  853, 734), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_PANEL10_CENTER_SWITCHES,             _R(943,  588,  977, 731), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_PANEL10_RIGHT_THUMBWHEELS,           _R(1067, 472, 1084, 734), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_PANEL10_RIGHT_SWITCHES,              _R(1112, 476, 1146, 731), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LM_TUNNEL_VENT_VALVE,                _R(1709, 297, 1747, 335), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LM_DP_GAUGE,                         _R(1681, 448, 1767, 530), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0, 0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_TUNNEL) {
		if (ForwardHatch.IsOpen())
		{
			hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_CSM_TUNNEL_OPEN));
		}
		else
		{
			hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_CSM_TUNNEL));
		}

		if (!hBmp)
		{
			return false;
		}

		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		int bottom;
		if (GNSplit)
			bottom = SATPANEL_GN_CENTER;
		else
			bottom = SATPANEL_GN;

		oapiSetPanelNeighbours(SATPANEL_LOWER_LEFT, SATPANEL_RIGHT_CB, SATPANEL_LOWER_MAIN, bottom);

		if (!ForwardHatch.IsOpen())
		{
			oapiRegisterPanelArea(AID_PRESS_EQUAL_VALVE_HANDLE,      _R(900, 480, 1021, 601), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                    PANEL_MAP_BACKGROUND);
		}

		oapiRegisterPanelArea(AID_FORWARD_HATCH,                     _R(735, 367,  874, 702), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                    PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0, 0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_RIGHT_CB) { 
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_CSM_RIGHT_CB_PANEL));

		if ( !hBmp )
		{
			return false;
		}

		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		if (GNSplit) 
			oapiSetPanelNeighbours(SATPANEL_TUNNEL, -1, SATPANEL_RIGHT, SATPANEL_GN_RIGHT);
		else
			oapiSetPanelNeighbours(SATPANEL_TUNNEL, -1, SATPANEL_RIGHT, SATPANEL_GN);

		oapiRegisterPanelArea (AID_PANEL225CIRCUITBRAKERS,			_R(  77,  404,  370,  833), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL226CIRCUITBRAKERS,			_R( 572, 1097,  994, 1437), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL229CIRCUITBRAKERS,			_R(  64,   79,  582,  232), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL250CIRCUITBRAKERS,			_R(1203, 1735, 1758, 1764), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL275CIRCUITBRAKERS_LOWER,	_R(1232,   28, 1261,  251), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_WASTE_MGMT_OVBD_DUMP,			_R(1521, 1483, 1637, 1599), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_WASTE_MGMT_BATT_VENT,			_R(1920, 1470, 2036, 1586), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_WASTE_MGMT_STOAGE_VENT,			_R(1920, 1304, 2036, 1420), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CSM_WASTE_DISPOSAL,			    _R( 765,  738, 1063, 1008), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	}

	if (id == SATPANEL_SEXTANT) { // Sextant

		int offset1 = 0, offset2 = 0;
		if (renderViewportIsWideScreen == 1) {
			offset1 = 103;
			offset2 = 205;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_SEXTANT_WIDE));
		} else if (renderViewportIsWideScreen == 2) {
			offset1 = 171;
			offset2 = 342;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_SEXTANT_16_9));
		} else {
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_SEXTANT));
		}
		if ( !hBmp ) {
			return false;
		}

		oapiSetPanelNeighbours(-1, SATPANEL_TELESCOPE, SATPANEL_GN, SATPANEL_GN);
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		oapiRegisterPanelArea (AID_CSM_SEXTANTCOVER,			_R( 244 + offset1,  115,  780 + offset1,  650), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_OPTICS_HANDCONTROLLER,		_R( 879 + offset1,  637,  924 + offset1,  686), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MARKBUTTON,					_R( 928 + offset1,  639,  956 + offset1,  671), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MARKREJECT,					_R( 961 + offset1,  655,  984 + offset1,  681), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OPTICSMODESPEEDSWITCHES,		_R( 401 + offset1,  719,  525 + offset1,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,	_R( 585 + offset1,  719,  608 + offset1,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OPTICS_DSKY,					_R( 721 + offset2,    0, 1024 + offset2,  349), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MINIMPULSE_HANDCONTROLLER,	_R( 103 + offset1,  637,  148 + offset1,  686), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_OPTICSCLKAREASEXT,			_R(0, 0, 10, 10),								PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.53765284, 0.84316631));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange( PI/2., PI/2., PI/2., PI/2.);
	}

	if (id == SATPANEL_TELESCOPE) { // Telescope
		int offset1 = 0, offset2 = 0;
		if (renderViewportIsWideScreen == 1) {
			offset1 = 103;
			offset2 = 205;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_TELESCOPE_WIDE));
		} else if (renderViewportIsWideScreen == 2) {
			offset1 = 171;
			offset2 = 342;
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_TELESCOPE_16_9));
		} else {
			hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_TELESCOPE));
		}
		if ( !hBmp ) {
			return false;
		}

		oapiSetPanelNeighbours(SATPANEL_SEXTANT, -1, SATPANEL_GN, SATPANEL_GN);
		oapiRegisterPanelBackground (hBmp, PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);

		oapiRegisterPanelArea (AID_CSM_TELESCOPECOVER,			_R( 244 + offset1,  115,  780 + offset1,  650), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_OPTICS_HANDCONTROLLER,		_R( 879 + offset1,  637,  924 + offset1,  686), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MARKBUTTON,					_R( 928 + offset1,  639,  956 + offset1,  671), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MARKREJECT,					_R( 961 + offset1,  655,  984 + offset1,  681), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OPTICSMODESPEEDSWITCHES,		_R( 401 + offset1,  719,  525 + offset1,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,	_R( 585 + offset1,  719,  608 + offset1,  739), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_OPTICS_DSKY,					_R( 721 + offset2,    0, 1024 + offset2,  349), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MINIMPULSE_HANDCONTROLLER,	_R( 103 + offset1,  637,  148 + offset1,  686), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_OPTICSCLKAREATELE,			_R(0, 0, 10, 10),								PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.53765284, 0.84316631));
		oapiCameraSetCockpitDir(0,0);
		SetCameraRotationRange( PI/2., PI/2., PI/2., PI/2.);			
	}

	InitPanel (id);

	//
	// Change to desired panel next timestep.
	//
    if (!InPanel && id != PanelId && !(!MainPanelSplit && (PanelId == SATPANEL_MAIN_MIDDLE || PanelId == SATPANEL_MAIN_RIGHT))) {
		CheckPanelIdInTimestep = true;
	} else {
	    PanelId = id;
	}
	InVC = false;
	InPanel = true;

	//
	// Set view parameter
	//

	SetView();

	return hBmp != NULL;
}

void Saturn::AddLeftMainPanelAreas() {

	oapiRegisterPanelArea (AID_THC,											_R(   0, 1251,   72, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);	
	oapiRegisterPanelArea (AID_ABORT_LIGHT,									_R( 862,  600,  924,  631), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SEQUENCERSWITCHES,							_R( 802,  918,  990, 1100), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LV_ENGINE_LIGHTS,							_R( 843,  735,  944,  879), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_IMU_CAGE_SWITCH,								_R( 289, 1237,  325, 1306), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MASTER_ALARM,								_R( 464,  497,  509,  533), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ATTITUDE_CONTROL_SWITCHES,					_R( 190,  838,  482,  867), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_BMAG_SWITCHES,								_R( 125, 1036,  258, 1065), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_THRUSTMETER,									_R( 498,  920,  593, 1011), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CMC_SWITCH,									_R( 343,  635,  377,  664), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FDAI_SWITCHES,								_R( 265,  742,  484,  771), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ALTIMETER,									_R( 835,   86,  972,  223), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ELS_SWITCHES,								_R( 702, 1150,  957, 1219), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EVENT_TIMER_SWITCHES,						_R( 701, 1260,  950, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ROT_PWR,										_R( 104,  948,  324,  977), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_dVTHRUST_SWITCHES,							_R( 442, 1046,  572, 1115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EVENT_TIMER,									_R( 866,  670,  937,  688), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS,											_R( 299, 1051,  337, 1103), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_GMETER,										_R( 403,  605,  482,  684), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SCCONTCMCMODESWITCHES,      					_R( 383,  948,  460,  977), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SCSTVCSWITCHES,      						_R( 380, 1173,  457, 1202), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSGIMBALMOTORSSWITCHES,      				_R( 472, 1173,  654, 1202), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ENTRYSWITCHES,      							_R( 336, 1260,  413, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LVSPSINDICATORSWITCHES,      				_R( 422, 1260,  499, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_TVCGIMBALDRIVESWITCHES,      				_R( 508, 1260,  585, 1289), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_GDCALIGNBUTTON,								_R( 290, 1169,  329, 1207), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DIRECT_ULLAGE_THRUST_ON,						_R( 370, 1037,  409, 1128), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,   PANEL_MAP_BACKGROUND);
	// EMS
	oapiRegisterPanelArea (AID_EMSFUNCTIONSWITCH,      						_R( 595,  280,  685,  370), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_GTASWITCH,		    						_R( 904,  291,  959,  402), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ENTRY_MODE_SWITCH,							_R( 593,  402,  628,  432), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);	
	oapiRegisterPanelArea (AID_EMSDVSETSWITCH,								_R( 906,  428,  961,  519), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EMSDVDISPLAY,								_R( 743,  518,  900,  539), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_LIGHT,									_R( 815,  456,  856,  472), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PT05G_LIGHT,									_R( 749,  456,  790,  472), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EMS_SCROLL_LEO,								_R( 731,  296,  875,  448), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_EMS_RSI_BKGRND,								_R( 602,  463,  690,  550), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
	// ASCP
	oapiRegisterPanelArea (AID_ASCPDISPLAYROLL,								_R( 199, 1144,  229, 1156), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,	                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPDISPLAYPITCH,							_R( 199, 1206,  229, 1218), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,	                PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPDISPLAYYAW,								_R( 199, 1268,  229, 1280), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,	                PANEL_MAP_BACKGROUND);		
	oapiRegisterPanelArea (AID_ASCPROLL,									_R( 124, 1126,  142, 1163), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPPITCH,									_R( 124, 1188,  142, 1225), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ASCPYAW,										_R( 124, 1250,  142, 1287), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
	// GPFPI DISPLAYS
	oapiRegisterPanelArea (AID_GPFPI_METERS,								_R( 629,  927,  791, 1032), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSGIMBALPITCHTHUMBWHEEL,					_R( 655, 1058,  672, 1094), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSGIMBALYAWTHUMBWHEEL,						_R( 739, 1067,  775, 1084), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	// FDAI
	fdaiLeft.RegisterMe(AID_FDAI_LEFT, 533, 612);
	if (!hBmpFDAIRollIndicator)	hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE (IDB_FDAI_ROLLINDICATOR));
	// ORDEAL
	oapiRegisterPanelArea (AID_ORDEALSWITCHES,								_R( 359,   28,  836,  230), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
}

void Saturn::AddLeftMiddleMainPanelAreas(int offset) {

	oapiRegisterPanelArea (AID_SEPARATIONSWITCHES,		    				_R(1087 + offset,  935, 1341 + offset, 1004), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SUITCABINDELTAPMETER,	    				_R(1445 + offset,  106, 1491 + offset,  150), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SMRCS_HELIUM1_SWITCHES,						_R(1585 + offset,  430, 1748 + offset,  459), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SMRCS_HELIUM2_SWITCHES,						_R(1411 + offset,  564, 1748 + offset,  633), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PRIM_PRPLNT_SWITCHES,						_R(1411 + offset,  718, 1748 + offset,  747), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SEC_PRPLT_SWITCHES,							_R(1411 + offset,  848, 1748 + offset,  877), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_GAUGES,									_R(1385 + offset,  385, 1548 + offset,  511), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LV_SWITCHES,									_R(1043 + offset, 1138, 1173 + offset, 1207), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MAIN_RELEASE_SWITCH,							_R(1042 + offset, 1228, 1078 + offset, 1297), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ABORT_ROW,									_R(1042 + offset, 1027, 1294 + offset, 1096), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CWS_LIGHTS_LEFT,								_R(1540 + offset,   97, 1752 + offset,  205), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_HELIUM1_TALKBACK,						_R(1591 + offset,  368, 1741 + offset,  391), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_HELIUM2_TALKBACK,						_R(1591 + offset,  526, 1741 + offset,  549), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_PROP1_TALKBACK,							_R(1591 + offset,  659, 1741 + offset,  682), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_PROP2_TALKBACK,							_R(1503 + offset,  792, 1739 + offset,  815), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DOCKINGPROBESWITCHES,    					_R(1388 + offset,  256, 1509 + offset,  325), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DOCKINGPROBEINDICATORS,      				_R(1396 + offset,  179, 1419 + offset,  229), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CSMLIGHTSWITCHES,      						_R(1518 + offset,  279, 1663 + offset,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LMPOWERSWITCH,      							_R(1692 + offset,  279, 1726 + offset,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

	// Display & keyboard (DSKY), main panel uses the main DSKY.
	oapiRegisterPanelArea (AID_DSKY_DISPLAY,								_R(1239 + offset,  589, 1344 + offset,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY_LIGHTS,									_R(1095 + offset,  594, 1197 + offset,  714), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY_KEY,			                        _R(1077 + offset,  785, 1363 + offset,  905), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

	// FDAI
	fdaiRight.RegisterMe(AID_FDAI_RIGHT, 1090 + offset, 284);
	if (!hBmpFDAIRollIndicator)	hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE (IDB_FDAI_ROLLINDICATOR));

	// MFDs
	MFDSPEC mfds_mainleft = {{1405 + offset, 1019, 1715 + offset, 1328}, 6, 6, 55, 44 };
    oapiRegisterMFD(MFD_LEFT, mfds_mainleft);
	oapiRegisterPanelArea (AID_MFDMAINLEFT,	    							_R(1347 + offset, 1006, 1772 + offset, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
}

void Saturn::AddRightMiddleMainPanelAreas(int offset) {

	oapiRegisterPanelArea (AID_MISSION_CLOCK,								_R(1834 + offset,  303, 1977 + offset,  326), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CYROTANKSWITCHES,        					_R(1902 + offset,  451, 2492 + offset,  537), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CYROTANKINDICATORS,        					_R(2173 + offset,  295, 2495 + offset,  439), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SUITCOMPRDELTAPMETER,       					_R(2069 + offset,  726, 2115 + offset,  770), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CABININDICATORS,        						_R(2278 + offset,  593, 2504 + offset,  717), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CAUTIONWARNING_SWITCHES,						_R(1908 + offset,  400, 2140 + offset,  434), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MISSION_TIMER_SWITCHES,						_R(2019 + offset,  299, 2139 + offset,  328), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RCS_INDICATORS,								_R(1785 + offset,  445, 1875 + offset,  535), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECS_INDICATOR_SWITCH,						_R(1785 + offset,  582, 1875 + offset,  672), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CWS_LIGHTS_RIGHT,							_R(1795 + offset,   97, 2008 + offset,  205), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSRADTEMPMETERS,							_R(1951 + offset,  604, 1997 + offset,  649), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSEVAPTEMPMETERS,							_R(2069 + offset,  604, 2115 + offset,  649), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSPRESSMETERS,								_R(2186 + offset,  604, 2232 + offset,  649), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSQUANTITYMETERS,							_R(2186 + offset,  726, 2232 + offset,  770), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSRADIATORINDICATOR,		 				_R(1799 + offset,  683, 1822 + offset,  706), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSRADIATORSWITCHES,         				_R(1796 + offset,  743, 2023 + offset,  772), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ECSSWITCHES,					 				_R(1787 + offset,  848, 2327 + offset,  877), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNAPITCHPOSITIONSWITCH,			_R(2268 + offset, 1016, 2358 + offset, 1116), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CABINTEMPAUTOCONTROLSWITCH,					_R(2441 + offset,  843, 2458 + offset,  879), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_POSTLANDINGVENTVALVELEVER,      				_R(2062 + offset,   76, 2112 + offset,  234), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNAUPPERSWITCHES,      			_R(2185 + offset,  943, 2262 + offset,  972), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNALOWERSWITCHES,      			_R(2381 + offset, 1157, 2458 + offset, 1186), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNAYAWPOSITIONSWITCH,      		_R(2398 + offset, 1016, 2488 + offset, 1116), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,					PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_HIGHGAINANTENNAMETERS,						_R(2283 + offset,  933, 2496 + offset,  985), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

	// MFDs
	MFDSPEC mfds_mainright = {{1843 + offset, 1019, 2153 + offset, 1328}, 6, 6, 55, 44};
    oapiRegisterMFD(MFD_RIGHT, mfds_mainright);
	oapiRegisterPanelArea (AID_MFDMAINRIGHT,								_R(1785 + offset, 1006, 2210 + offset, 1360), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
}

void Saturn::AddRightMainPanelAreas(int offset) {

	oapiRegisterPanelArea (AID_CSM_MNPNL_WDW_LES,							_R(3071 + offset,    0, 3430 + offset,  160), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_FUELCELLINDICATORS,		    				_R(2763 + offset,  319, 2913 + offset,  443), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLPHRADTEMPINDICATORS,	  				_R(2822 + offset,  490, 3019 + offset,  513), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLRADIATORSINDICATORS,    				_R(2822 + offset,  539, 2931 + offset,  562), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLRADIATORSSWITCHES,    				_R(2816 + offset,  607, 2937 + offset,  637), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLINDICATORSSWITCH,    				_R(3027 + offset,  627, 3117 + offset,  717), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLHEATERSSWITCHES,	    				_R(2817 + offset,  695, 2938 + offset,  725), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLPURGESWITCHES,	    				_R(2815 + offset,  817, 3123 + offset,  846), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLREACTANTSINDICATORS,    				_R(2823 + offset,  893, 3061 + offset,  917), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLREACTANTSSWITCHES,    				_R(2800 + offset,  955, 3131 + offset,  984), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (ADI_MAINBUSAINDICATORS,		    				_R(2953 + offset,  758, 3062 + offset,  781), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_FUELCELLLATCHSWITCHES,	    				_R(2593 + offset, 1251, 2670 + offset, 1280), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_TELECOMTB,    								_R(3119 + offset, 1042, 3142 + offset, 1115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SBAND_NORMAL_SWITCHES,						_R(2593 + offset, 1050, 2858 + offset, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MASTER_ALARM2,								_R(2960 + offset,  652, 3005 + offset,  688), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DCVOLTS,										_R(3154 + offset,  761, 3253 + offset,  860), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DCAMPS,										_R(3135 + offset,  656, 3234 + offset,  755), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ACVOLTS,										_R(3365 + offset, 1069, 3464 + offset, 1168), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_VHFANTENNAROTARY,      						_R(2593 + offset,  184, 2683 + offset,  274), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSTESTSWITCH,								_R(2714 + offset,  711, 2748 + offset,  740), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSOXIDFLOWVALVESWITCHES,      				_R(2640 + offset,  798, 2717 + offset,  827), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSPUGMODESWITCH,      						_R(2726 + offset,  798, 2760 + offset,  827), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSSWITCHES,      							_R(2593 + offset,  955, 2791 + offset,  984), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DCINDICATORSROTARY,      					_R(3180 + offset,  896, 3270 + offset,  986), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_BATTERYCHARGEROTARY,      					_R(3336 + offset,  896, 3426 + offset,  986), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SBANDAUXSWITCHES,      						_R(2877 + offset, 1050, 2964 + offset, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_UPTLMSWITCHES,								_R(2983 + offset, 1050, 3070 + offset, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SBANDANTENNASWITCHES,      					_R(2593 + offset, 1155, 2670 + offset, 1184), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_VHFAMTHUMBWHEELS,      						_R(2745 + offset, 1149, 2762 + offset, 1283), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_VHFSWITCHES,      							_R(2838 + offset, 1152, 3060 + offset, 1181), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_TAPERECORDERSWITCHES,      					_R(2838 + offset, 1250, 2958 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_POWERSWITCHES,      							_R(2967 + offset, 1250, 3044 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PCMBITRATESWITCH,							_R(3053 + offset, 1250, 3130 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ACINVERTERSWITCHES,							_R(3182 + offset, 1050, 3345 + offset, 1279), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_ACINDICATORROTARY,							_R(3386 + offset, 1205, 3476 + offset, 1295), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSOXIDFLOWVALVEINDICATOR,					_R(2605 + offset,  792, 2627 + offset,  834), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSOXIDUNBALMETER,       					_R(2619 + offset,  696, 2679 + offset,  757), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSHELIUMVALVEINDICATORS,    				_R(2598 + offset,  896, 2664 + offset,  919), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPSMETERS,       		    				_R(2583 + offset,  319, 2733 + offset,  443), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_OXID_PERCENT_DISPLAY,					_R(2664 + offset,  628, 2702 + offset,  641), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_FUEL_PERCENT_DISPLAY,					_R(2664 + offset,  657, 2702 + offset,  670), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SPS_INJ_VLV,                 				_R(2596 + offset,  523, 2774 + offset,  557), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
}

void Saturn::AddLeftLowerPanelAreas()
{
	// Panel 100
	oapiRegisterPanelArea (AID_PANEL100SWITCHES,      						_R( 613,  678,  964,  747), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PANEL100LIGHTINGROTARIES,					_R( 602,  813,  946,  903), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
				
	// Panel 163
	oapiRegisterPanelArea (AID_SCIUTILPOWERSWITCH,      					_R( 300, 1953,  334, 1982), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
}

void Saturn::AddLeftCenterLowerPanelAreas(int offset)
{
	// Panel 101
	oapiRegisterPanelArea (AID_DCVOLTS_PANEL101,							_R(1121 + offset,  65, 1231 + offset,  175), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SYSTEMTESTROTARIES,							_R(1069 + offset,  213, 1280 + offset,  304), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_RNDZXPDRSWITCH,      						_R(1218 + offset,  350, 1252 + offset,  379), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_PANEL101LOWERSWITCHES,      				    _R(1093 + offset,  486, 1251 + offset,  515), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_GNZEROSWITCH,								_R(1365 + offset,  951, 1399 + offset,  980), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_TELESCOPETRUNNIONSWITCH,						_R(1496 + offset,  951, 1530 + offset,  980), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_CONTROLLERCOUPLINGSWITCH,					_R(1605 + offset,  951, 1639 + offset,  980), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_OPTICSMODESPEEDSWITCHES,						_R(1496 + offset, 1090, 1639 + offset, 1119), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);

	MFDSPEC mfds_gnleft  =     {{ 1048 + 58 + offset,  1726 + 13, 1048 + 368 + offset, 1726 + 322}, 6, 6, 55, 44 };
	MFDSPEC mfds_gnuser1 =     {{ 1484 + 58 + offset,  1726 + 13, 1484 + 368 + offset, 1726 + 322}, 6, 6, 55, 44 };

	oapiRegisterMFD(MFD_LEFT, mfds_gnleft);
	oapiRegisterMFD(MFD_USER1, mfds_gnuser1);

	oapiRegisterPanelArea (AID_MFDGNLEFT,									_R(1048 + offset, 1726, 1048 + 425 + offset, 1726 + 354), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MFDGNUSER1,									_R(1484 + offset, 1726, 1484 + 425 + offset, 1726 + 354), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);

	// "Accelerator" areas
	oapiRegisterPanelArea (AID_SWITCHTO_SEXTANT1,	     				    _R(1620 + offset,  585, 1760 + offset,  690), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SWITCHTO_SEXTANT2,	      				    _R(1340 + offset, 1190, 1605 + offset, 1445), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
}

void Saturn::AddCenterLowerPanelAreas(int offset)
{
	MFDSPEC mfds_gnuser2 =     {{ 1920 + 58 + offset,  1726 + 13, 1920 + 368 + offset, 1726 + 322}, 6, 6, 55, 44 };
	MFDSPEC mfds_gnright =     {{ 2356 + 58 + offset,  1726 + 13, 2356 + 368 + offset, 1726 + 322}, 6, 6, 55, 44 };

	oapiRegisterMFD(MFD_USER2, mfds_gnuser2);
	oapiRegisterMFD(MFD_RIGHT, mfds_gnright);		

	oapiRegisterPanelArea (AID_MFDGNUSER2,									_R(1920 + offset, 1726, 1920 + 425 + offset, 1726 + 354), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MFDGNRIGHT,									_R(2356 + offset, 1726, 2356 + 425 + offset, 1726 + 354), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN|PANEL_MOUSE_LBPRESSED, PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_CWS_GNLIGHTS,								_R(2102 + offset,  925, 2102+53 + offset, 925+76), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_MASTER_ALARM3,								_R(2104 + offset, 1036, 2149 + offset, 1072), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_CONDITIONLAMPSSWITCH,						_R(2230 + offset,  966, 2264 + offset,  995), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_UPLINKTELEMETRYSWITCH,						_R(2230 + offset, 1106, 2264 + offset, 1153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LOWEREQUIPMENTBAYCWLIGHTS,					_R(2100 + offset,  923, 2154 + offset,  999), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_LOWEREQUIPMENTBAYOPTICSLIGHTS,				_R(1363 + offset, 1054, 1437 + offset, 1153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

	oapiRegisterPanelArea (AID_OPT_SHAFTDISPLAY,							_R(1893 + offset,  570, 1944 + offset,  582), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_OPT_TRUNDISPLAY,								_R(2021 + offset,  520, 2072 + offset,  532), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);

	// "Accelerator" areas
	oapiRegisterPanelArea (AID_SWITCHTO_TELESCOPE1,      				    _R(2030 + offset,  580, 2165 + offset,  700), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_SWITCHTO_TELESCOPE2,      				    _R(2100 + offset, 1190, 2360 + offset, 1445), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,		PANEL_MAP_BACKGROUND);
}

void Saturn::AddRightCenterLowerPanelAreas(int offset)
{
	//
	// Lower panel uses the second DSKY.
	//
	oapiRegisterPanelArea (AID_DSKY2_DISPLAY,								_R(2602 + offset,  700, 2707 + offset,  876), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY2_LIGHTS,								_R(2458 + offset,  705, 2560 + offset,  825), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				PANEL_MAP_BACKGROUND);
	oapiRegisterPanelArea (AID_DSKY2_KEY,			                        _R(2440 + offset,  896, 2725 + offset, 1016), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,	PANEL_MAP_BACKGROUND);
	
	if (Panel181)
	{
		oapiRegisterPanelArea(AID_CSM_PANEL_181, _R(3241 + offset, 2180, 3673 + offset, 2431), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
	}
}

void Saturn::AddRightLowerPanelAreas(int offset)
{
	return;
}
//
// Wire up all the control panel switches. Note that this can
// be called multiple times, so be sure to initialise all the panel
// and row variables at the start of the function to reset internal
// state..
//

void Saturn::SetSwitches(int panel) {

	MainPanel.Init(0, this, &soundlib, this);

	//
	// SATPANEL_MAIN
	//

	MasterAlarmSwitchRow.Init(0, MainPanel);
	MasterAlarmSwitch.Init(&cws);
	MasterAlarmSwitchRow.AddSwitch(&MasterAlarmSwitch);

	AccelGMeterRow.Init(AID_GMETER, MainPanel);
	AccelGMeter.Init(g_Param.pen[4], g_Param.pen[4], AccelGMeterRow, this);

	THCRotaryRow.Init(AID_THC, MainPanel);
	THCRotary.Init(0, 0, 72, 109, srf[SRF_THC], srf[SRF_BORDER_72x109], THCRotaryRow, this);

	SequencerSwitchesRow.Init(AID_SEQUENCERSWITCHES, MainPanel);
	LiftoffNoAutoAbortSwitch.Init     ( 20,   3, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, &secs, 0, 81);
	LiftoffNoAutoAbortSwitch.InitGuard(  0,   1, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	LesMotorFireSwitch.Init			  ( 20,  49, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 119, 117, 231);
	LesMotorFireSwitch.InitGuard      (  0,  47, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	CanardDeploySwitch.Init           ( 20,  95, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 157, 99, 281);
	CanardDeploySwitch.InitGuard      (  0,  93, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	CsmLvSepSwitch.Init				  ( 20, 141, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 195, 99, 319);
	CsmLvSepSwitch.InitGuard		  (  0, 139, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40]);
	ApexCoverJettSwitch.Init          (127,   3, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 233, 195, 193);
	ApexCoverJettSwitch.InitGuard     ( 94,   1, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);
	DrogueDeploySwitch.Init			  (127,  49, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 271, 195, 231);
	DrogueDeploySwitch.InitGuard      ( 94,  47, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);
	MainDeploySwitch.Init             (127,  95, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 309, 195, 269);
	MainDeploySwitch.InitGuard        ( 94,  93, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);
	CmRcsHeDumpSwitch.Init			  (127, 141, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], SequencerSwitchesRow, 0, 347, 195, 307);
	CmRcsHeDumpSwitch.InitGuard		  ( 94, 139, 92, 40, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_92x40], 0, 40);

	SeparationSwitchesRow.Init(AID_SEPARATIONSWITCHES, MainPanel);
	EDSSwitch.Init				  (  0,	23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CsmLmFinalSep1Switch.Init	  ( 43, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CsmLmFinalSep1Switch.InitGuard( 42,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	CsmLmFinalSep2Switch.Init	  ( 87, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CsmLmFinalSep2Switch.InitGuard( 86,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	CmSmSep1Switch.Init			  (131, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CmSmSep1Switch.InitGuard      (130,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69], 72);
	CmSmSep1Switch.SetFailed(SwitchFail.SMJett1Fail != 0);
	CmSmSep2Switch.Init			  (175, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
	CmSmSep2Switch.InitGuard      (174,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69], 72);
	CmSmSep2Switch.SetFailed(SwitchFail.SMJett2Fail != 0);

	if (!SkylabCM) {
		SIVBPayloadSepSwitch.Init		  (219, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SeparationSwitchesRow);
		SIVBPayloadSepSwitch.InitGuard     (218,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	}

	CryoTankSwitchesRow.Init(AID_CYROTANKSWITCHES, MainPanel);
	CabinFan1Switch.Init (  10, 39, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       CryoTankSwitchesRow);
	CabinFan2Switch.Init ( 69, 39, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       CryoTankSwitchesRow);
	H2Heater1Switch.Init (124, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	H2Heater2Switch.Init (167, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Heater1Switch.Init (210, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Heater2Switch.Init (260, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2PressIndSwitch.Init(303, 39, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       CryoTankSwitchesRow);
	H2Fan1Switch.Init    (359, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	H2Fan2Switch.Init    (423, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Fan1Switch.Init    (488, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);
	O2Fan2Switch.Init    (551, 39, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CryoTankSwitchesRow);

	SCContCMCModeSwitchesRow.Init(AID_SCCONTCMCMODESWITCHES, MainPanel);
	SCContSwitch.Init(  0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SCContCMCModeSwitchesRow, this);
	CMCModeSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SCContCMCModeSwitchesRow, &agc);
	
	SCSTvcSwitchesRow.Init(AID_SCSTVCSWITCHES, MainPanel);
	SCSTvcPitchSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SCSTvcSwitchesRow);
	SCSTvcYawSwitch.Init  (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SCSTvcSwitchesRow);
	
	SPSGimbalMotorsRow.Init(AID_SPSGIMBALMOTORSSWITCHES, MainPanel);
	Pitch1Switch.Init(  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);
	Pitch2Switch.Init( 49, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);
	Yaw1Switch.Init  ( 98, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);
	Yaw2Switch.Init  (148, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSGimbalMotorsRow);

	EntrySwitchesRow.Init(AID_ENTRYSWITCHES, MainPanel);
	EMSRollSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EntrySwitchesRow); 
	GSwitch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EntrySwitchesRow); 
	
	LVSPSIndSwitchesRow.Init(AID_LVSPSINDICATORSWITCHES, MainPanel);
	LVSPSPcIndicatorSwitch.Init        ( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVSPSIndSwitchesRow); 
	LVFuelTankPressIndicatorSwitch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVSPSIndSwitchesRow);
	
	TVCGimbalDriveSwitchesRow.Init(AID_TVCGIMBALDRIVESWITCHES, MainPanel);
	TVCGimbalDrivePitchSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TVCGimbalDriveSwitchesRow);
	TVCGimbalDriveYawSwitch.Init  (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TVCGimbalDriveSwitchesRow);

	CSMLightSwitchesRow.Init(AID_CSMLIGHTSWITCHES, MainPanel);
	RunEVALightSwitch.Init(  0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CSMLightSwitchesRow);
	RndzLightSwitch.Init  ( 57, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CSMLightSwitchesRow);
	TunnelLightSwitch.Init(111, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CSMLightSwitchesRow);

	LMPowerSwitchRow.Init(AID_LMPOWERSWITCH, MainPanel);
	LMPowerSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], LMPowerSwitchRow, this);
	LMPowerSwitch.SetDelayTime(1);

	PostLDGVentValveLeverRow.Init(AID_POSTLANDINGVENTVALVELEVER, MainPanel);
	PostLDGVentValveLever.Init( 0, 0, 50, 158, srf[SRF_POSTLDGVENTVLVLEVER], srf[SRF_BORDER_50x158], PostLDGVentValveLeverRow);

	GDCAlignButtonRow.Init(AID_GDCALIGNBUTTON, MainPanel);
	GDCAlignButton.Init( 0, 0, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], GDCAlignButtonRow, 157, 119);

	HighGainAntennaUpperSwitchesRow.Init(AID_HIGHGAINANTENNAUPPERSWITCHES, MainPanel);
	GHATrackSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], HighGainAntennaUpperSwitchesRow); 
	GHABeamSwitch.Init (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], HighGainAntennaUpperSwitchesRow);
	
	HighGainAntennaLowerSwitchesRow.Init(AID_HIGHGAINANTENNALOWERSWITCHES, MainPanel);
	GHAPowerSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], HighGainAntennaLowerSwitchesRow); 
	GHAServoElecSwitch.Init (43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], HighGainAntennaLowerSwitchesRow);
	
	//
	// SM RCS Helium 1
	//

	SMRCSHelium1Row.Init(AID_SMRCS_HELIUM1_SWITCHES, MainPanel);
	SMRCSHelium1ASwitch.Init (0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row); 
	SMRCSHelium1BSwitch.Init (43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row);
	SMRCSHelium1CSwitch.Init (86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row);
	SMRCSHelium1DSwitch.Init (129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium1Row);

	SMRCSHelium1TalkbackRow.Init(AID_RCS_HELIUM1_TALKBACK, MainPanel);
	SMRCSHelium1ATalkback.Init(0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, SMQuadARCS.GetHeliumValve1(), true);
	SMRCSHelium1BTalkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, SMQuadBRCS.GetHeliumValve1(), true);
	SMRCSHelium1CTalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, SMQuadCRCS.GetHeliumValve1(), true);
	SMRCSHelium1DTalkback.Init(127, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium1TalkbackRow, SMQuadDRCS.GetHeliumValve1(), true);

	//
	// SM RCS Helium 2
	//

	SMRCSHelium2Row.Init(AID_SMRCS_HELIUM2_SWITCHES, MainPanel);
	SMRCSHelium2ASwitch.Init (174, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row);
	SMRCSHelium2BSwitch.Init (217, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row);
	SMRCSHelium2CSwitch.Init (260, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row);
	SMRCSHelium2DSwitch.Init (303, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSHelium2Row);

	CMUplinkSwitch.Init(0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row, &agc);
	CMUplinkSwitch.SetChannelData(033, 10, false);	// Down is 'Block Uplink Input'

	if (!SkylabCM)
		IUUplinkSwitch.Init(43, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row);

	CMRCSPressSwitch.Init(87, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row);
	CMRCSPressSwitch.InitGuard(86, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	SMRCSIndSwitch.Init(131, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SMRCSHelium2Row);

	SMRCSHelium2TalkbackRow.Init(AID_RCS_HELIUM2_TALKBACK, MainPanel);
	SMRCSHelium2ATalkback.Init(0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, SMQuadARCS.GetHeliumValve2(), true);
	SMRCSHelium2BTalkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, SMQuadBRCS.GetHeliumValve2(), true);
	SMRCSHelium2CTalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, SMQuadCRCS.GetHeliumValve2(), true);
	SMRCSHelium2DTalkback.Init(127, 0, 23, 23, srf[SRF_INDICATOR], SMRCSHelium2TalkbackRow, SMQuadDRCS.GetHeliumValve2(), true);

	//
	// RCS Gauges.
	//

	RCSGaugeRow.Init(AID_RCS_GAUGES, MainPanel, &Panel276CB3);
	RCSQuantityMeter.Init(srf[SRF_NEEDLE], RCSGaugeRow, &RCSIndicatorsSwitch, &SMRCSIndSwitch);
	RCSFuelPressMeter.Init(srf[SRF_NEEDLE], RCSGaugeRow, &RCSIndicatorsSwitch);
	RCSHeliumPressMeter.Init(srf[SRF_NEEDLE], RCSGaugeRow, &RCSIndicatorsSwitch);
	RCSTempMeter.Init(srf[SRF_NEEDLE], RCSGaugeRow, &RCSIndicatorsSwitch);

	//
	// SM RCS Primary Propellant.
	//

	SMRCSProp1Row.Init(AID_PRIM_PRPLNT_SWITCHES, MainPanel);
	SMRCSHeaterASwitch.Init (2, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:PRIMSMRCSQUADAHEATER"), NULL, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:SECSMRCSQUADAHEATER"));
	SMRCSHeaterBSwitch.Init (45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:PRIMSMRCSQUADBHEATER"), NULL, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:SECSMRCSQUADBHEATER"));
	SMRCSHeaterCSwitch.Init (88, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:PRIMSMRCSQUADCHEATER"), NULL, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:SECSMRCSQUADCHEATER"));
	SMRCSHeaterDSwitch.Init (131, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:PRIMSMRCSQUADDHEATER"), NULL, 
							(Boiler *) Panelsdk.GetPointerByString("ELECTRIC:SECSMRCSQUADDHEATER"));

	SMRCSProp1ASwitch.Init (174, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);
	SMRCSProp1BSwitch.Init (217, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);
	SMRCSProp1CSwitch.Init (260, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);
	SMRCSProp1DSwitch.Init (303, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp1Row);

	SMRCSProp1TalkbackRow.Init(AID_RCS_PROP1_TALKBACK, MainPanel);
	SMRCSProp1ATalkback.Init(0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, SMQuadARCS.GetPrimPropellantValve(), true);
	SMRCSProp1BTalkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, SMQuadBRCS.GetPrimPropellantValve(), true);
	SMRCSProp1CTalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, SMQuadCRCS.GetPrimPropellantValve(), true);
	SMRCSProp1DTalkback.Init(127, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp1TalkbackRow, SMQuadDRCS.GetPrimPropellantValve(), true);

	//
	// SM RCS Secondary Propellant.
	//

	SMRCSProp2Row.Init(AID_SEC_PRPLT_SWITCHES, MainPanel);

	RCSCMDSwitch.Init (2, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row);
	RCSTrnfrSwitch.Init (45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row); 

	CMRCSProp1Switch.Init (88, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row); 
	CMRCSProp2Switch.Init (131, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row); 

	SMRCSProp2ASwitch.Init (174, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row);
	SMRCSProp2BSwitch.Init (217, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row);
	SMRCSProp2CSwitch.Init (260, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row);
	SMRCSProp2DSwitch.Init (303, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SMRCSProp2Row);

	SMRCSProp2TalkbackRow.Init(AID_RCS_PROP2_TALKBACK, MainPanel);
	SMRCSProp2ATalkback.Init(85, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, SMQuadARCS.GetSecPropellantValve(), false);
	SMRCSProp2BTalkback.Init(128, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, SMQuadBRCS.GetSecPropellantValve(), false);
	SMRCSProp2CTalkback.Init(171, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, SMQuadCRCS.GetSecPropellantValve(), false);
	SMRCSProp2DTalkback.Init(213, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, SMQuadDRCS.GetSecPropellantValve(), false);
	
	CMRCSProp1Talkback.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CMRCS1.GetPropellantValve(), true);
	CMRCSProp2Talkback.Init(42, 0, 23, 23, srf[SRF_INDICATOR], SMRCSProp2TalkbackRow, CMRCS2.GetPropellantValve(), true);

	//
	// Attitude control, etc.
	//

	AttitudeControlRow.Init(AID_ATTITUDE_CONTROL_SWITCHES, MainPanel);
	ManualAttRollSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlRow);
	ManualAttPitchSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlRow);
	ManualAttYawSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlRow);
	LimitCycleSwitch.Init(129, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);
	AttDeadbandSwitch.Init(172, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);
	AttRateSwitch.Init(215, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);
	TransContrSwitch.Init(258, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AttitudeControlRow);

	//
	// BMAG
	//

	BMAGRow.Init(AID_BMAG_SWITCHES, MainPanel);
	BMAGRollSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BMAGRow);
	BMAGPitchSwitch.Init(49, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BMAGRow);
	BMAGYawSwitch.Init(99, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BMAGRow);

	//
	// CMC Att switch.
	//

	CMCAttRow.Init(AID_CMC_SWITCH, MainPanel);
	CMCAttSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CMCAttRow);

	//
	// FDAI switches.
	//

	// *** DANGER WILL ROBINSON: FDAISourceSwitch and FDAISelectSwitch ARE REVERSED! ***

	FDAISwitchesRow.Init(AID_FDAI_SWITCHES, MainPanel);
	FDAIScaleSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FDAISwitchesRow);
	FDAISelectSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FDAISwitchesRow);
	FDAISourceSwitch.Init(142, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FDAISwitchesRow);
	FDAIAttSetSwitch.Init(185, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FDAISwitchesRow);

	//
	// Caution and Warning switches.
	//

	CautionWarningRow.Init(AID_CAUTIONWARNING_SWITCHES, MainPanel);
	MissionTimerSwitch.Init(190, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &MissionTimerDisplay);
	CautionWarningModeSwitch.Init(7, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);
	CautionWarningCMCSMSwitch.Init(55, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);
	CautionWarningPowerSwitch.Init(104, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);
	CautionWarningLightTestSwitch.Init(147, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], CautionWarningRow, &cws);

	//
	// Mission Timer switches.
	//

	MissionTimerSwitchesRow.Init(AID_MISSION_TIMER_SWITCHES, MainPanel);
	MissionTimerHoursSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MissionTimerSwitchesRow, TIME_UPDATE_HOURS, &MissionTimerDisplay);
	MissionTimerMinutesSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MissionTimerSwitchesRow, TIME_UPDATE_MINUTES, &MissionTimerDisplay);
	MissionTimerSecondsSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MissionTimerSwitchesRow, TIME_UPDATE_SECONDS, &MissionTimerDisplay);

	//
	// IMU Cage switch.
	//

	IMUCageSwitchRow.Init(AID_IMU_CAGE_SWITCH, MainPanel);
	IMUGuardedCageSwitch.Init(1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], IMUCageSwitchRow, &imu);
	IMUGuardedCageSwitch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);

	//
	// RCS Indicators rotary switch.
	//

	RCSIndicatorsSwitchRow.Init(AID_RCS_INDICATORS, MainPanel);
	RCSIndicatorsSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], RCSIndicatorsSwitchRow);
	RCSIndicatorsSwitch.SetCMSource(0, &CMRCS1);
	RCSIndicatorsSwitch.SetCMSource(1, &CMRCS2);
	RCSIndicatorsSwitch.SetSMSource(2, &SMQuadARCS);
	RCSIndicatorsSwitch.SetSMSource(3, &SMQuadBRCS);
	RCSIndicatorsSwitch.SetSMSource(4, &SMQuadCRCS);
	RCSIndicatorsSwitch.SetSMSource(5, &SMQuadDRCS);

	//
	// ECS Indicators rotary switch.
	//

	ECSIndicatorsSwitchRow.Init(AID_ECS_INDICATOR_SWITCH, MainPanel);
	ECSIndicatorsSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], ECSIndicatorsSwitchRow);

	//
	// LV switches.
	//

	LVRow.Init(AID_LV_SWITCHES, MainPanel);
	LVGuidanceSwitch.Init	  ( 1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVRow, &agc);
	LVGuidanceSwitch.InitGuard( 0,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69], 180);
	LVGuidanceSwitch.SetChannelData(030, 9, false);

	if (!SkylabCM) {
		SIISIVBSepSwitch.Init     (48, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVRow);
		SIISIVBSepSwitch.InitGuard(47,  0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
		TLIEnableSwitch.Init      (96, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LVRow, this);
	}

	//
	// ELS Switches.
	//

	ELSRow.Init(AID_ELS_SWITCHES, MainPanel);
	CGSwitch.Init(0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow, &agc);
	CGSwitch.SetChannelData(032, 11, true);	// LM Attached flag.
	ELSLogicSwitch.Init(44, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	ELSLogicSwitch.InitGuard(43, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	ELSAutoSwitch.Init(88, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	CMRCSLogicSwitch.Init(131, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	CMPropDumpSwitch.Init(175, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	CMPropDumpSwitch.InitGuard(174, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	CMPropPurgeSwitch.Init(220, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ELSRow);
	CMPropPurgeSwitch.InitGuard(219, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);

	//
	// Event Timer Switches
	//

	EventTimerRow.Init(AID_EVENT_TIMER_SWITCHES, MainPanel);
	// FCSMSPSASwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EventTimerRow);
	// FCSMSPSBSwitch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EventTimerRow);
	EventTimerUpDownSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, &EventTimerDisplay);
	EventTimerUpDownSwitch.SetDelayTime(1);
	EventTimerContSwitch.Init(129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, &EventTimerDisplay);
	EventTimerContSwitch.SetDelayTime(1);
	EventTimerMinutesSwitch.Init(172, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, TIME_UPDATE_MINUTES, &EventTimerDisplay);
	EventTimerSecondsSwitch.Init(215, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerRow, TIME_UPDATE_SECONDS, &EventTimerDisplay);
	SaturnEventTimerDisplay.Init(EventTimerRow, this); 	// dummy switch/display for checklist controller

	//
	// Main chute release.
	//

	MainReleaseRow.Init(AID_MAIN_RELEASE_SWITCH, MainPanel);
	MainReleaseSwitch.Init(1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], MainReleaseRow);
	MainReleaseSwitch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	MainReleaseSwitch.SetDelayTime(1);

	//
	// Abort switches.
	//

	AbortRow.Init(AID_ABORT_ROW, MainPanel);
	PropDumpAutoSwitch.Init(0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AbortRow);
	TwoEngineOutAutoSwitch.Init(49, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AbortRow);
	LVRateAutoSwitch.Init(110, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], AbortRow);
	TowerJett1Switch.Init(169, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AbortRow);
	TowerJett1Switch.InitGuard(168, 0, 36, 69, srf[SRF_SWITCHGUARDS], 180);
	TowerJett1Switch.SetFailed(SwitchFail.TowerJett1Fail != 0);
	TowerJett2Switch.Init(217, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AbortRow);
	TowerJett2Switch.InitGuard(216, 0, 36, 69, srf[SRF_SWITCHGUARDS], 180);
	TowerJett2Switch.SetFailed(SwitchFail.TowerJett2Fail != 0);

	//
	// Rotational controller power switches.
	//

	RotContrPowerRow.Init(AID_ROT_PWR, MainPanel);
	RotPowerNormal1Switch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);
	RotPowerNormal2Switch.Init(59, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);
	RotPowerDirect1Switch.Init(118, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);
	RotPowerDirect2Switch.Init(186, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RotContrPowerRow);

	//
	// dV Thrust switches.
	//

	dvThrustRow.Init(AID_dVTHRUST_SWITCHES, MainPanel);
	dVThrust1Switch.Init(1, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], dvThrustRow, &PilotValveMnACircuitBraker, NULL);
	dVThrust1Switch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
	dVThrust2Switch.Init(95, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], dvThrustRow, &PilotValveMnBCircuitBraker, NULL);
	dVThrust2Switch.InitGuard(94, 0, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);

	//
	// Fuel Cell Switches.
	//

	FuelCellPhRadTempIndicatorsRow.Init(AID_FUELCELLPHRADTEMPINDICATORS, MainPanel);
	FuelCellPhIndicator.Init     (  0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellPhRadTempIndicatorsRow);
	FuelCellRadTempIndicator.Init(174, 0, 23, 23, srf[SRF_INDICATOR], FuelCellPhRadTempIndicatorsRow);

	FuelCellRadiatorsIndicatorsRow.Init(AID_FUELCELLRADIATORSINDICATORS, MainPanel);
	FuelCellRadiators1Indicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);
	FuelCellRadiators2Indicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);
	FuelCellRadiators3Indicator.Init(86, 0, 23, 23, srf[SRF_INDICATOR], FuelCellRadiatorsIndicatorsRow);

	FuelCellRadiatorsSwitchesRow.Init(AID_FUELCELLRADIATORSSWITCHES, MainPanel);
	FuelCellRadiators1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellRadiatorsSwitchesRow);
	FuelCellRadiators2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellRadiatorsSwitchesRow);
	FuelCellRadiators3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellRadiatorsSwitchesRow);

	FuelCellIndicatorsSwitchRow.Init(AID_FUELCELLINDICATORSSWITCH, MainPanel);
	FuelCellIndicatorsSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], FuelCellIndicatorsSwitchRow);

	FuelCellHeatersSwitchesRow.Init(AID_FUELCELLHEATERSSWITCHES, MainPanel);
	FuelCellHeater1Switch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellHeatersSwitchesRow);
	FuelCellHeater2Switch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellHeatersSwitchesRow);
	FuelCellHeater3Switch.Init(86, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellHeatersSwitchesRow);

	FuelCellPurgeSwitchesRow.Init(AID_FUELCELLPURGESWITCHES, MainPanel);
	FuelCellPurge1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);
	FuelCellPurge2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);
	FuelCellPurge3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);

	FuelCellReactantsIndicatorsRow.Init(AID_FUELCELLREACTANTSINDICATORS, MainPanel);
	FuelCellReactants1Indicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);
	FuelCellReactants2Indicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);
	FuelCellReactants3Indicator.Init(86, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow);

	FuelCellReactantsSwitchesRow.Init(AID_FUELCELLREACTANTSSWITCHES, MainPanel);
	FuelCellReactants1Switch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);
	FuelCellReactants2Switch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);
	FuelCellReactants3Switch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);

	FuelCellLatchSwitchesRow.Init(AID_FUELCELLLATCHSWITCHES, MainPanel);
	FCReacsValvesSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellLatchSwitchesRow);
	H2PurgeLineSwitch.Init  (43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FuelCellLatchSwitchesRow);

	SPSRow.Init(AID_SPS, MainPanel);
	SPSswitch.Init(0, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SPSRow);

	SPSGimbalPitchThumbwheelRow.Init(AID_SPSGIMBALPITCHTHUMBWHEEL, MainPanel);
	SPSGimbalPitchThumbwheel.Init(0, 0, 17, 36, srf[SRF_THUMBWHEEL_GPI_PITCH], srf[SRF_BORDER_17x36], SPSGimbalPitchThumbwheelRow);

	SPSGimbalYawThumbwheelRow.Init(AID_SPSGIMBALYAWTHUMBWHEEL, MainPanel);
	SPSGimbalYawThumbwheel.Init(0, 0, 36, 17, srf[SRF_THUMBWHEEL_GPI_YAW], srf[SRF_BORDER_36x17], SPSGimbalYawThumbwheelRow);

	//
	// Communication switches (s-band, vhf etc.)
	//

	TelecomTBRow.Init(AID_TELECOMTB, MainPanel);
	PwrAmplTB.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], TelecomTBRow);
	DseTapeTB.Init( 0,50, 23, 23, srf[SRF_INDICATOR], TelecomTBRow, &dataRecorder);

	SBandAuxSwitchesRow.Init(AID_SBANDAUXSWITCHES, MainPanel);
	SBandAuxSwitch1.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAuxSwitchesRow);
	SBandAuxSwitch2.Init(53, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAuxSwitchesRow);

	UPTLMSwitchesRow.Init(AID_UPTLMSWITCHES, MainPanel);
	UPTLMSwitch1.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], UPTLMSwitchesRow);
	UPTLMSwitch2.Init(53, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], UPTLMSwitchesRow);
	UPTLMSwitch2.SetDelayTime(1);

	SBandAntennaSwitchesRow.Init(AID_SBANDANTENNASWITCHES, MainPanel);
	SBandAntennaSwitch1.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAntennaSwitchesRow);
	SBandAntennaSwitch2.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandAntennaSwitchesRow);

	VHFAmThumbwheelsRow.Init(AID_VHFAMTHUMBWHEELS, MainPanel);
	SquelchAThumbwheel.Init(0,  0, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], VHFAmThumbwheelsRow);
	SquelchBThumbwheel.Init(0, 98, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], VHFAmThumbwheelsRow);

	VHFSwitchesRow.Init(AID_VHFSWITCHES, MainPanel);
	VHFAMASwitch.Init    (  0,  0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	VHFAMBSwitch.Init    ( 51,  0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	RCVOnlySwitch.Init   (102,  0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	VHFBeaconSwitch.Init (145,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], VHFSwitchesRow);
	VHFRangingSwitch.Init(188,  0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], VHFSwitchesRow);

	TapeRecorderSwitchesRow.Init(AID_TAPERECORDERSWITCHES, MainPanel);
	TapeRecorderPCMSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], TapeRecorderSwitchesRow);
	TapeRecorderRecordSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TapeRecorderSwitchesRow);
	TapeRecorderForwardSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TapeRecorderSwitchesRow);

	PowerSwitchesRow.Init(AID_POWERSWITCHES, MainPanel);
	SCESwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], PowerSwitchesRow);
	PMPSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], PowerSwitchesRow);

	PCMBitRateSwitchRow.Init(AID_PCMBITRATESWITCH, MainPanel);
	PCMBitRateSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], PCMBitRateSwitchRow);

	ACInverterSwitchesRow.Init(AID_ACINVERTERSWITCHES, MainPanel);
	MnA1Switch.Init       (  0,   0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &InverterPower1MainACircuitBraker, 0);
	MnB2Switch.Init       ( 63,   0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &InverterPower2MainBCircuitBraker, 0);
	MnA3Switch.Init       (126,   0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &InverterPower3MainACircuitBraker, 0, &InverterPower3MainBCircuitBraker);
	AcBus1Switch1.Init    (  0, 101, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 1,1, this);
	AcBus1Switch2.Init    ( 43, 101, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 1,2, this);
	AcBus1Switch3.Init    ( 86, 101, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 1,3, this);
	AcBus1ResetSwitch.Init(129, 101, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &EPSSensorUnitAcBus1CircuitBraker, &EPSSensorUnitAcBus1CircuitBraker, 0);
	AcBus2Switch1.Init    (  0, 200, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 2,1, this);
	AcBus2Switch2.Init    ( 43, 200, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 2,2, this);
	AcBus2Switch3.Init    ( 86, 200, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, 2,3, this);
	AcBus2ResetSwitch.Init(129, 200, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ACInverterSwitchesRow, &EPSSensorUnitAcBus2CircuitBraker, &EPSSensorUnitAcBus2CircuitBraker, 0);
	
	MainBusASwitch1.Init(129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow); 
	MainBusASwitch2.Init(179, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);
	MainBusASwitch3.Init(230, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow);
	MainBusAResetSwitch.Init(273, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPurgeSwitchesRow, &EPSSensorUnitDcBusACircuitBraker, &EPSSensorUnitDcBusACircuitBraker, 0);
	MainBusAIndicatorsRow.Init(ADI_MAINBUSAINDICATORS, MainPanel);
	MainBusAIndicator1.Init(0, 0, 23, 23, srf[SRF_INDICATOR], MainBusAIndicatorsRow, &MainBusAController, 1);
	MainBusAIndicator2.Init(43, 0, 23, 23, srf[SRF_INDICATOR], MainBusAIndicatorsRow, &MainBusAController, 2);
	MainBusAIndicator3.Init(86, 0, 23, 23, srf[SRF_INDICATOR], MainBusAIndicatorsRow, &MainBusAController, 3);

	MainBusBSwitch1.Init(129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow); 
	MainBusBSwitch2.Init(185, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);
	MainBusBSwitch3.Init(241, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow);
	MainBusBResetSwitch.Init(297, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellReactantsSwitchesRow, &EPSSensorUnitDcBusBCircuitBraker, &EPSSensorUnitDcBusBCircuitBraker, 0);
	MainBusBIndicator1.Init(129, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow, &MainBusBController, 1);
	MainBusBIndicator2.Init(172, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow, &MainBusBController, 2);
	MainBusBIndicator3.Init(215, 0, 23, 23, srf[SRF_INDICATOR], FuelCellReactantsIndicatorsRow, &MainBusBController, 3);

	SBandNormalSwitchesRow.Init(AID_SBAND_NORMAL_SWITCHES, MainPanel);
	SBandNormalXPDRSwitch.Init    (	  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalPwrAmpl1Switch.Init(  43, 0, 34, 29, srf[SRF_SWITCHUP],		 srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalPwrAmpl2Switch.Init(  86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalMode1Switch.Init   (	145, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalMode2Switch.Init   (	188, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SBandNormalSwitchesRow);
	SBandNormalMode3Switch.Init   (	231, 0, 34, 29, srf[SRF_SWITCHUP],		 srf[SRF_BORDER_34x29],	SBandNormalSwitchesRow);

	ACVoltMeterRow.Init(AID_ACVOLTS, MainPanel);
	CSMACVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], ACVoltMeterRow, &ACIndicatorRotary);
	CSMACVoltMeter.SetSurface(srf[SRF_ACVOLTS], 99, 98);

	DCVoltMeterRow.Init(AID_DCVOLTS, MainPanel);
	CSMDCVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], DCVoltMeterRow, &DCIndicatorsRotary);
	CSMDCVoltMeter.SetSurface(srf[SRF_DCVOLTS], 99, 98);

	DCAmpMeterRow.Init(AID_DCAMPS, MainPanel);
	DCAmpMeter.Init(g_Param.pen[4], g_Param.pen[4], DCAmpMeterRow, this, &DCIndicatorsRotary);
	DCAmpMeter.SetSurface(srf[SRF_DCAMPS], 99, 98);

	CryoTankMetersRow.Init(AID_CYROTANKINDICATORS, MainPanel, &InstrumentationPowerFeeder);
	H2Pressure1Meter.Init(1, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	H2Pressure2Meter.Init(2, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	O2Pressure1Meter.Init(1, srf[SRF_NEEDLE], CryoTankMetersRow, this, &O2PressIndSwitch);
	O2Pressure2Meter.Init(2, srf[SRF_NEEDLE], CryoTankMetersRow, this, &O2PressIndSwitch);
	H2Quantity1Meter.WireTo(&ACBus1PhaseC);
	H2Quantity1Meter.Init("H2", 1, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	H2Quantity2Meter.WireTo(&ACBus2PhaseC);
	H2Quantity2Meter.Init("H2", 2, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	O2Quantity1Meter.WireTo(&ACBus1PhaseC);
	O2Quantity1Meter.Init("O2", 1, srf[SRF_NEEDLE], CryoTankMetersRow, this);
	O2Quantity2Meter.WireTo(&ACBus2PhaseC);
	O2Quantity2Meter.Init("O2", 2, srf[SRF_NEEDLE], CryoTankMetersRow, this);

	FuelCellMetersRow.Init(AID_FUELCELLINDICATORS, MainPanel, &GaugePower);
	FuelCellH2FlowMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);
	FuelCellO2FlowMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);
	FuelCellTempMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);
	FuelCellCondenserTempMeter.Init(srf[SRF_NEEDLE], FuelCellMetersRow, this, &FuelCellIndicatorsSwitch);

	CabinMetersRow.Init(AID_CABININDICATORS, MainPanel, &GaugePower);
	SuitTempMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	SuitTempMeter.WireTo(&ECSTempTransducerFeeder);
	CabinTempMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	CabinTempMeter.WireTo(&ECSTempTransducerFeeder);
	SuitPressMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	SuitPressMeter.WireTo(&ECSPressGroups1Feeder);
	CabinPressMeter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	CabinPressMeter.WireTo(&ECSPressGroups2Feeder);
	PartPressCO2Meter.Init(srf[SRF_NEEDLE], CabinMetersRow, this);
	PartPressCO2Meter.WireTo(&ECSPressGroups2Feeder);

	SuitComprDeltaPMeterRow.Init(AID_SUITCOMPRDELTAPMETER, MainPanel, &GaugePower);
	SuitComprDeltaPMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitComprDeltaPMeterRow, this);
	SuitComprDeltaPMeter.WireTo(&ECSPressGroups1Feeder);
	LeftO2FlowMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitComprDeltaPMeterRow, this);

	SuitCabinDeltaPMeterRow.Init(AID_SUITCABINDELTAPMETER, MainPanel, &GaugePower);
	SuitCabinDeltaPMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitCabinDeltaPMeterRow, this);
	SuitCabinDeltaPMeter.FrameSurface = srf[SRF_SUITCABINDELTAPMETER];
	SuitCabinDeltaPMeter.WireTo(&Panel276CB2);
	RightO2FlowMeter.Init(g_Param.pen[4], g_Param.pen[4], SuitCabinDeltaPMeterRow, this);
	RightO2FlowMeter.FrameSurface = srf[SRF_SUITCABINDELTAPMETER];

	EcsRadTempMetersRow.Init(AID_ECSRADTEMPMETERS, MainPanel, &GaugePower);
	EcsRadTempInletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsRadTempMetersRow, this, &ECSIndicatorsSwitch);
	EcsRadTempPrimOutletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsRadTempMetersRow, this);

	EcsEvapTempMetersRow.Init(AID_ECSEVAPTEMPMETERS, MainPanel, &GaugePower);
	EcsRadTempSecOutletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsEvapTempMetersRow, this);
	GlyEvapTempOutletMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsEvapTempMetersRow, this, &ECSIndicatorsSwitch);

	EcsPressMetersRow.Init(AID_ECSPRESSMETERS, MainPanel, &GaugePower);
	GlyEvapSteamPressMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsPressMetersRow, this, &ECSIndicatorsSwitch);
	GlycolDischPressMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsPressMetersRow, this, &ECSIndicatorsSwitch);

	EcsQuantityMetersRow.Init(AID_ECSQUANTITYMETERS, MainPanel, &GaugePower);
	AccumQuantityMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsQuantityMetersRow, this, &ECSIndicatorsSwitch);
	H2oQuantityMeter.Init(g_Param.pen[4], g_Param.pen[4], EcsQuantityMetersRow, this, &H2oQtyIndSwitch, &ECSWastePotTransducerFeeder);

	EcsRadiatorIndicatorRow.Init(AID_ECSRADIATORINDICATOR, MainPanel, &GaugePower);
	EcsRadiatorIndicator.Init( 0, 0, 23, 23, srf[SRF_ECSINDICATOR], EcsRadiatorIndicatorRow);

	EcsRadiatorSwitchesRow.Init(AID_ECSRADIATORSWITCHES, MainPanel);
	EcsRadiatorsFlowContAutoSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsFlowContPwrSwitch.Init( 50, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsManSelSwitch.Init(     100, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsHeaterPrimSwitch.Init( 150, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsRadiatorSwitchesRow);
	EcsRadiatorsHeaterSecSwitch.Init(  193, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsRadiatorSwitchesRow);

	EcsSwitchesRow.Init(AID_ECSSWITCHES, MainPanel);
	PotH2oHtrSwitch.Init                   (  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SuitCircuitH2oAccumAutoSwitch.Init     ( 43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SuitCircuitH2oAccumOnSwitch.Init       ( 86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SuitCircuitHeatExchSwitch.Init         (129, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SecCoolantLoopEvapSwitch.Init          (172, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	SecCoolantLoopPumpSwitch.Init          (221, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow, &ECSSecCoolLoopAc1CircuitBraker, NULL, &ECSSecCoolLoopAc2CircuitBraker);
	H2oQtyIndSwitch.Init                   (270, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);
	GlycolEvapTempInSwitch.Init            (313, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);
	GlycolEvapSteamPressAutoManSwitch.Init (364, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);
	GlycolEvapSteamPressIncrDecrSwitch.Init(411, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	GlycolEvapH2oFlowSwitch.Init           (456, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EcsSwitchesRow);
	CabinTempAutoManSwitch.Init            (506, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29],       EcsSwitchesRow);

	CabinTempAutoControlSwitchRow.Init(AID_CABINTEMPAUTOCONTROLSWITCH, MainPanel);
	CabinTempAutoControlSwitch.Init(0, 0, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], CabinTempAutoControlSwitchRow);

	EcsGlycolPumpsSwitchRow.Init(AID_ECSGLYCOLPUMPSSWITCH, MainPanel);
	EcsGlycolPumpsSwitch.Init(0, 0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_90x90], EcsGlycolPumpsSwitchRow,
		                      (Pump *) Panelsdk.GetPointerByString("ELECTRIC:PRIMGLYCOLPUMP"),
							  &ECSGlycolPumpsAc1ACircuitBraker, &ECSGlycolPumpsAc1BCircuitBraker, &ECSGlycolPumpsAc1CCircuitBraker,
							  &ECSGlycolPumpsAc2ACircuitBraker, &ECSGlycolPumpsAc2BCircuitBraker, &ECSGlycolPumpsAc2CCircuitBraker);

	HighGainAntennaPitchPositionSwitchRow.Init(AID_HIGHGAINANTENNAPITCHPOSITIONSWITCH, MainPanel);
	HighGainAntennaPitchPositionSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], HighGainAntennaPitchPositionSwitchRow);

	HighGainAntennaYawPositionSwitchRow.Init(AID_HIGHGAINANTENNAYAWPOSITIONSWITCH, MainPanel);
	HighGainAntennaYawPositionSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], HighGainAntennaYawPositionSwitchRow);
	
	HighGainAntennaMetersRow.Init(AID_HIGHGAINANTENNAMETERS, MainPanel, &GaugePower);
	HighGainAntennaPitchMeter.Init(g_Param.pen[4], g_Param.pen[4], HighGainAntennaMetersRow, this);
	HighGainAntennaStrengthMeter.Init(g_Param.pen[4], g_Param.pen[4], HighGainAntennaMetersRow, this);
	HighGainAntennaYawMeter.Init(g_Param.pen[4], g_Param.pen[4], HighGainAntennaMetersRow, this);

	VHFAntennaRotaryRow.Init(AID_VHFANTENNAROTARY, MainPanel);
	VHFAntennaRotarySwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], VHFAntennaRotaryRow);
	
	//
	// SPS switches (panel 3)
	//
	
	SPSInjectorValveIndicatorsRow.Init(AID_SPS_INJ_VLV, MainPanel);
	SPSInjectorValve1Indicator.Init(  0, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);
	SPSInjectorValve2Indicator.Init( 48, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);
	SPSInjectorValve3Indicator.Init( 96, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);
	SPSInjectorValve4Indicator.Init(144, 0, 34, 34, srf[SRF_SPS_INJ_VLV], SPSInjectorValveIndicatorsRow);

	SPSTestSwitchRow.Init(AID_SPSTESTSWITCH, MainPanel);
	SPSTestSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSTestSwitchRow);

	SPSOxidFlowValveIndicatorsRow.Init(AID_SPSOXIDFLOWVALVEINDICATOR, MainPanel);
	SPSOxidFlowValveMaxIndicator.Init(0,  0, 22, 12, srf[SRF_SPSMAXINDICATOR], SPSOxidFlowValveIndicatorsRow);
	SPSOxidFlowValveMinIndicator.Init(0, 30, 22, 12, srf[SRF_SPSMININDICATOR], SPSOxidFlowValveIndicatorsRow);

	SPSOxidFlowValveSwitchesRow.Init(AID_SPSOXIDFLOWVALVESWITCHES, MainPanel);
	SPSOxidFlowValveSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSOxidFlowValveSwitchesRow);
	SPSOxidFlowValveSelectorSwitch.Init(43, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SPSOxidFlowValveSwitchesRow);

	SPSPugModeSwitchRow.Init(AID_SPSPUGMODESWITCH, MainPanel);
	SPSPugModeSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSPugModeSwitchRow);
	
	SPSHeliumValveIndicatorsRow.Init(AID_SPSHELIUMVALVEINDICATORS, MainPanel);
	SPSHeliumValveAIndicator.Init( 0, 0, 23, 23, srf[SRF_INDICATOR], SPSHeliumValveIndicatorsRow);
	SPSHeliumValveBIndicator.Init(43, 0, 23, 23, srf[SRF_INDICATOR], SPSHeliumValveIndicatorsRow);

	SPSSwitchesRow.Init(AID_SPSSWITCHES, MainPanel);
	SPSHeliumValveASwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);
	SPSHeliumValveBSwitch.Init(43, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);
	SPSLineHTRSSwitch.Init(86, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);
	SPSPressIndSwitch.Init(164, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], SPSSwitchesRow);

	SPSOxidPercentRow.Init(AID_SPS_OXID_PERCENT_DISPLAY, MainPanel);
	SPSOxidPercentMeter.Init(srf[SRF_SPS_FONT_BLACK], srf[SRF_SPS_FONT_WHITE], SPSOxidPercentRow, this);

	SPSFuelPercentRow.Init(AID_SPS_FUEL_PERCENT_DISPLAY, MainPanel);
	SPSFuelPercentMeter.Init(srf[SRF_SPS_FONT_BLACK], srf[SRF_SPS_FONT_WHITE], SPSFuelPercentRow, this);

	SPSOxidUnbalMeterRow.Init(AID_SPSOXIDUNBALMETER, MainPanel);
	SPSOxidUnbalMeter.Init(g_Param.pen[3], g_Param.pen[3], SPSOxidUnbalMeterRow, this);

	SPSMetersRow.Init(AID_SPSMETERS, MainPanel);
	SPSTempMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this, &Panel276CB3);
	SPSHeliumNitrogenPressMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this, &SPSPressIndSwitch); // Needs wiring to multiple sources
	SPSFuelPressMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this, true, &Panel276CB4);
	SPSOxidPressMeter.Init(srf[SRF_NEEDLE], SPSMetersRow, this, false, &Panel276CB3);

	LVSPSPcMeterRow.Init(AID_THRUSTMETER, MainPanel, &GaugePower);
	LVSPSPcMeter.Init(g_Param.pen[4], g_Param.pen[4], LVSPSPcMeterRow, this, &LVSPSPcIndicatorSwitch, srf[SRF_THRUSTMETER]);

	GPFPIMeterRow.Init(AID_GPFPI_METERS, MainPanel);
	GPFPIPitch1Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, 1, 15);
	GPFPIPitch2Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, 2, 53);
	GPFPIYaw1Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, 1, 91);
	GPFPIYaw2Meter.Init(srf[SRF_NEEDLE], GPFPIMeterRow, this, 2, 129);

	DirectUllageThrustOnRow.Init(AID_DIRECT_ULLAGE_THRUST_ON, MainPanel);
	DirectUllageButton.Init( 0, 0, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], DirectUllageThrustOnRow, 79, 119);
	ThrustOnButton.Init( 0, 53, 39, 38, srf[SRF_SEQUENCERSWITCHES], srf[SRF_BORDER_39x38], DirectUllageThrustOnRow, 79, 157);

	//
	// Electricals switches & indicators
	//

	DCIndicatorsRotaryRow.Init(AID_DCINDICATORSROTARY, MainPanel);
	DCIndicatorsRotary.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], DCIndicatorsRotaryRow);

	ACIndicatorRotaryRow.Init(AID_ACINDICATORROTARY, MainPanel);
	ACIndicatorRotary.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], ACIndicatorRotaryRow);
	
	BatteryChargeRotaryRow.Init(AID_BATTERYCHARGEROTARY, MainPanel);
	BatteryChargeRotary.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], BatteryChargeRotaryRow);
	
	//
	// Docking probe switches
	//
	DockingProbeSwitchesRow.Init(AID_DOCKINGPROBESWITCHES, MainPanel);
	DockingProbeExtdRelSwitch.Init(1, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], DockingProbeSwitchesRow);
	DockingProbeExtdRelSwitch.InitGuard(0, 0, 36, 69, srf[SRF_SWITCHGUARDS], 180);
	DockingProbeRetractPrimSwitch.Init(44, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], DockingProbeSwitchesRow);
	DockingProbeRetractSecSwitch.Init(87, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], DockingProbeSwitchesRow);

	DockingProbeIndicatorsRow.Init(AID_DOCKINGPROBEINDICATORS, MainPanel);
	DockingProbeAIndicator.Init(  0,  0, 23, 23, srf[SRF_INDICATOR], DockingProbeIndicatorsRow);
	DockingProbeBIndicator.Init(  0, 27, 23, 23, srf[SRF_INDICATOR], DockingProbeIndicatorsRow);

	//
	// EMS switches
	//

	EMSFunctionSwitchRow.Init(AID_EMSFUNCTIONSWITCH, MainPanel);
	EMSFunctionSwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], EMSFunctionSwitchRow);

	EMSModeRow.Init(AID_ENTRY_MODE_SWITCH, MainPanel);
	EMSModeSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], EMSModeRow);
	
	GTASwitchRow.Init(AID_GTASWITCH, MainPanel);
	GTASwitch.Init(10, 49, 34,  29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], GTASwitchRow);
	GTASwitch.InitGuard(0,  0, 55, 111, srf[SRF_GTACOVER], srf[SRF_BORDER_55x111]);
	
	EMSDvDisplayRow.Init(AID_EMSDVDISPLAY, MainPanel, &GaugePower);
	EMSDvDisplay.Init(srf[SRF_DIGITAL], EMSDvDisplayRow, this);
	EMSScrollDisplay.Init(EMSDvDisplayRow, this); 	// dummy switch/display for checklist controller

	//
	// SATPANEL_RIGHT
	//

	FuelCellPumpsSwitchesRow.Init(AID_FUELCELLPUMPSSWITCHES, MainPanel);
	FuelCellPumps1Switch.Init(  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPumpsSwitchesRow, &ACBus1, NULL, &ACBus2);
	FuelCellPumps2Switch.Init( 65, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPumpsSwitchesRow, &ACBus1, NULL, &ACBus2);
	FuelCellPumps3Switch.Init(130, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FuelCellPumpsSwitchesRow, &ACBus1, NULL, &ACBus2);

	SuitCompressorSwitchesRow.Init(AID_SUITCOMPRESSORSWITCHES, MainPanel);
	SuitCompressor1Switch.Init( 1, 58, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], SuitCompressorSwitchesRow,
 							   &SuitCompressorsAc1ACircuitBraker, &SuitCompressorsAc1BCircuitBraker, &SuitCompressorsAc1CCircuitBraker,
							   &SuitCompressorsAc2ACircuitBraker, &SuitCompressorsAc2BCircuitBraker, &SuitCompressorsAc2CCircuitBraker);
	SuitCompressor2Switch.Init(42,  0, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], SuitCompressorSwitchesRow,
 							   &SuitCompressorsAc1ACircuitBraker, &SuitCompressorsAc1BCircuitBraker, &SuitCompressorsAc1CCircuitBraker,
							   &SuitCompressorsAc2ACircuitBraker, &SuitCompressorsAc2BCircuitBraker, &SuitCompressorsAc2CCircuitBraker);

	RightCOASPowerSwitchRow.Init(AID_RIGHTCOASSWITCH, MainPanel);
	RightCOASPowerSwitch.Init( 0, 0, 34, 31, srf[SRF_SWITCH20], srf[SRF_BORDER_34x31], RightCOASPowerSwitchRow);

	EpsSensorSignalDcCircuitBrakersRow.Init(AID_EPSSENSORSIGNALDCCIRCUITBRAKERS, MainPanel);
	EpsSensorSignalDcMnaCircuitBraker.Init( 0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalDcCircuitBrakersRow);
	EpsSensorSignalDcMnbCircuitBraker.Init(38, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalDcCircuitBrakersRow);

	EpsSensorSignalAcCircuitBrakersRow.Init(AID_EPSSENSORSIGNALACCIRCUITBRAKERS, MainPanel);
	EpsSensorSignalAc1CircuitBraker.Init(  0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalAcCircuitBrakersRow);
	EpsSensorSignalAc2CircuitBraker.Init(101, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EpsSensorSignalAcCircuitBrakersRow);

	CWCircuitBrakersRow.Init(AID_CWCIRCUITBRAKERS, MainPanel);
	CWMnaCircuitBraker.Init( 0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], CWCircuitBrakersRow, MainBusA, 5.0);
	CWMnbCircuitBraker.Init(38, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], CWCircuitBrakersRow, MainBusB, 5.0);

	LMPWRCircuitBrakersRow.Init(AID_LMPWRCIRCUITBRAKERS, MainPanel);
	MnbLMPWR1CircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], LMPWRCircuitBrakersRow, MainBusB, 7.5);
	MnbLMPWR2CircuitBraker.Init(3, 78, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], LMPWRCircuitBrakersRow, MainBusB, 7.5);

	InverterControlCircuitBrakersRow.Init(AID_INVERTERCONTROLCIRCUITBRAKERS, MainPanel);
	InverterControl1CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InverterControlCircuitBrakersRow, &MnA1Switch, 70.0);
	InverterControl2CircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InverterControlCircuitBrakersRow, &MnB2Switch, 70.0);
	InverterControl3CircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InverterControlCircuitBrakersRow, &MnA3Switch, 70.0);

	EPSSensorUnitCircuitBrakersRow.Init(AID_EPSSENSORUNITCIRCUITBRAKERS, MainPanel);
	EPSSensorUnitDcBusACircuitBraker.Init(  0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);
	EPSSensorUnitDcBusBCircuitBraker.Init( 37,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);
	EPSSensorUnitAcBus1CircuitBraker.Init( 74,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);
	EPSSensorUnitAcBus2CircuitBraker.Init(111,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EPSSensorUnitCircuitBrakersRow, &BatteryRelayBus, 5.0);

	BATRLYBusCircuitBrakersRow.Init(AID_BATRLYBUSCIRCUITBRAKERS, MainPanel);
	BATRLYBusBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BATRLYBusCircuitBrakersRow, &BatteryBusA, 15.0);
	BATRLYBusBatBCircuitBraker.Init(37,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BATRLYBusCircuitBrakersRow, &BatteryBusB, 15.0);

	ECSRadiatorsCircuitBrakersRow.Init(AID_ECSRADIATORSCIRCUITBRAKERS, MainPanel);
	ControllerAc1CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	ControllerAc2CircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	CONTHTRSMnACircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow, MainBusA, 5.0);
	CONTHTRSMnBCircuitBraker.Init(172,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow, MainBusB, 5.0);
	HTRSOVLDBatACircuitBraker.Init(209,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);
	HTRSOVLDBatBCircuitBraker.Init(246,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSRadiatorsCircuitBrakersRow);

	BatteryChargerCircuitBrakersRow.Init(AID_BATTERYCHARGERCIRCUITBRAKERS, MainPanel);
	BatteryChargerBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow);
	BatteryChargerBatBCircuitBraker.Init(37,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow);
	BatteryChargerMnACircuitBraker.Init( 74,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow, MainBusA);
	BatteryChargerMnBCircuitBraker.Init(111,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow, MainBusB);
	BatteryChargerAcPwrCircuitBraker.Init(192,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], BatteryChargerCircuitBrakersRow, &BatCHGRSwitch);

	InstrumentLightingCircuitBrakersRow.Init(AID_INSTRUMENTLIGHTINGCIRCUITBRAKERS, MainPanel);
	InstrumentLightingESSMnACircuitBraker.Init(  0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow, MainBusA, 15.0);
	InstrumentLightingESSMnBCircuitBraker.Init( 38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow, MainBusB, 15.0);
	InstrumentLightingNonESSCircuitBraker.Init(133,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingSCIEquipSEP1CircuitBraker.Init(171,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingSCIEquipSEP2CircuitBraker.Init(209,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);
	InstrumentLightingSCIEquipHatchCircuitBraker.Init(247,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], InstrumentLightingCircuitBrakersRow);

	ECSCircuitBrakersRow.Init(AID_ECSCIRCUITBRAKERS, MainPanel);
	ECSPOTH2OHTRMnACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusA, 5.0);
	ECSPOTH2OHTRMnBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusB, 5.0);
	ECSH2OAccumMnACircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSH2OAccumMnBCircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow);
	ECSTransducerWastePOTH2OMnACircuitBraker.Init(262,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusA, 5.0);
	ECSTransducerWastePOTH2OMnBCircuitBraker.Init(300,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusB, 5.0);
	ECSTransducerPressGroup1MnACircuitBraker.Init(338,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusA, 5.0);
	ECSTransducerPressGroup1MnBCircuitBraker.Init(376,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusB, 5.0);
	ECSTransducerPressGroup2MnACircuitBraker.Init(432,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusA, 5.0);
	ECSTransducerPressGroup2MnBCircuitBraker.Init(470,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusB, 5.0);
	ECSTransducerTempMnACircuitBraker.Init(508,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusB, 5.0);
	ECSTransducerTempMnBCircuitBraker.Init(546,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSCircuitBrakersRow, MainBusB, 5.0);

	ECSLowerRowCircuitBrakersRow.Init(AID_ECSLOWERROWCIRCUITBRAKERS, MainPanel);
	ECSSecCoolLoopAc1CircuitBraker.Init(  0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1);
	ECSSecCoolLoopAc2CircuitBraker.Init(102,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2);
	ECSSecCoolLoopRADHTRMnACircuitBraker.Init(186,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, MainBusA, 5.0);
	ECSSecCoolLoopXducersMnACircuitBraker.Init(224,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, MainBusA, 5.0);
	ECSSecCoolLoopXducersMnBCircuitBraker.Init(262,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, MainBusB, 5.0);
	ECSWasteH2OUrineDumpHTRMnACircuitBraker.Init(355,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, MainBusA, 5.0);
	ECSWasteH2OUrineDumpHTRMnBCircuitBraker.Init(393,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, MainBusB, 5.0);
	ECSCabinFanAC1ACircuitBraker.Init(585,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1PhaseA, 2.0);
	ECSCabinFanAC1BCircuitBraker.Init(622,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1PhaseB, 2.0);
	ECSCabinFanAC1CCircuitBraker.Init(659,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus1PhaseC, 2.0);
	ECSCabinFanAC2ACircuitBraker.Init(696,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2PhaseA, 2.0);
	ECSCabinFanAC2BCircuitBraker.Init(733,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2PhaseB, 2.0);
	ECSCabinFanAC2CCircuitBraker.Init(771,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSLowerRowCircuitBrakersRow, &ACBus2PhaseC, 2.0);

	GNCircuitBrakersRow.Init(AID_GUIDANCENAVIGATIONCIRCUITBRAKERS, MainPanel);
	GNPowerAc1CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, &ACBus1, 2.0);
	GNPowerAc2CircuitBraker.Init(57,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, &ACBus2, 2.0);
	GNIMUMnACircuitBraker.Init(103,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 25.0);
	GNIMUMnBCircuitBraker.Init(140,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 25.0);
	GNIMUHTRMnACircuitBraker.Init(177,  0, 29, 29, srf[SRF_CIRCUITBRAKER_YELLOW], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 7.5);
	GNIMUHTRMnBCircuitBraker.Init(214,  0, 29, 29, srf[SRF_CIRCUITBRAKER_YELLOW], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 7.5);
	GNComputerMnACircuitBraker.Init(251,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 5.0);
	GNComputerMnBCircuitBraker.Init(288,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 5.0);
	GNOpticsMnACircuitBraker.Init(325,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusA, 10.0);
	GNOpticsMnBCircuitBraker.Init(362,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], GNCircuitBrakersRow, MainBusB, 10.0);

	SuitCompressorsAc1ACircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC1ACIRCUITBRAKER, MainPanel);
	SuitCompressorsAc1ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc1ACircuitBrakerRow, &ACBus1PhaseA, 2.0);

	SuitCompressorsAc1BCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC1BCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc1BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc1BCircuitBrakerRow, &ACBus1PhaseB, 2.0);

	SuitCompressorsAc1CCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC1CCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc1CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc1CCircuitBrakerRow, &ACBus1PhaseC, 2.0);

	SuitCompressorsAc2ACircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC2ACIRCUITBRAKER, MainPanel);
	SuitCompressorsAc2ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc2ACircuitBrakerRow, &ACBus2PhaseA, 2.0);

	SuitCompressorsAc2BCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC2BCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc2BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc2BCircuitBrakerRow, &ACBus2PhaseB, 2.0);

	SuitCompressorsAc2CCircuitBrakerRow.Init(AID_SUITCOMPRESSORSAC2CCIRCUITBRAKER, MainPanel);
	SuitCompressorsAc2CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SuitCompressorsAc2CCircuitBrakerRow, &ACBus2PhaseC, 2.0);

	ECSGlycolPumpsAc1ACircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC1ACIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc1ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc1ACircuitBrakerRow, &ACBus1PhaseA, 2.0);

	ECSGlycolPumpsAc1BCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC1BCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc1BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc1BCircuitBrakerRow, &ACBus1PhaseB, 2.0);

	ECSGlycolPumpsAc1CCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC1CCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc1CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc1CCircuitBrakerRow, &ACBus1PhaseC, 2.0);

	ECSGlycolPumpsAc2ACircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC2ACIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc2ACircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc2ACircuitBrakerRow, &ACBus2PhaseA, 2.0);

	ECSGlycolPumpsAc2BCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC2BCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc2BCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc2BCircuitBrakerRow, &ACBus2PhaseB, 2.0);
	
	ECSGlycolPumpsAc2CCircuitBrakerRow.Init(AID_ECSGLYCOLPUMPSAC2CCIRCUITBRAKER, MainPanel);
	ECSGlycolPumpsAc2CCircuitBraker.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ECSGlycolPumpsAc2CCircuitBrakerRow, &ACBus2PhaseC, 2.0);

	ModeIntercomVOXSensThumbwheelSwitchRow.Init(AID_MODEINTERCOMVOXSENSTHUMBWHEEL, MainPanel);
	ModeIntercomVOXSensThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], srf[SRF_BORDER_33x43], ModeIntercomVOXSensThumbwheelSwitchRow);

	PowerMasterVolumeThumbwheelSwitchRow.Init(AID_POWERMASTERVOLUMETHUMBWHEEL, MainPanel);
	PowerMasterVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], srf[SRF_BORDER_33x43], PowerMasterVolumeThumbwheelSwitchRow); // , VOLUME_COMMS, &soundlib);

	PadCommVolumeThumbwheelSwitchRow.Init(AID_PADCOMMVOLUMETHUMBWHEEL, MainPanel);
	PadCommVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], srf[SRF_BORDER_33x43], PadCommVolumeThumbwheelSwitchRow);

	IntercomVolumeThumbwheelSwitchRow.Init(AID_INTERCOMVOLUMETHUMBWHEEL, MainPanel);
	IntercomVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], srf[SRF_BORDER_33x43], IntercomVolumeThumbwheelSwitchRow);

	SBandVolumeThumbwheelSwitchRow.Init(AID_SBANDVOLUMETHUMBWHEEL, MainPanel);
	SBandVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], srf[SRF_BORDER_33x43], SBandVolumeThumbwheelSwitchRow);

	VHFAMVolumeThumbwheelSwitchRow.Init(AID_VHFVOLUMETHUMBWHEEL, MainPanel);
	VHFAMVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL], srf[SRF_BORDER_33x43], VHFAMVolumeThumbwheelSwitchRow);

	AudioControlSwitchRow.Init(AID_AUDIOCONTROLSWITCH, MainPanel);
	AudioControlSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30], srf[SRF_BORDER_34x34], AudioControlSwitchRow);

	SuitPowerSwitchRow.Init(AID_SUITPOWERSWITCH, MainPanel);
	SuitPowerSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30], srf[SRF_BORDER_34x34], SuitPowerSwitchRow);

	RightUtilityPowerSwitchRow.Init(AID_RIGHTUTILITYPOWERSWITCH, MainPanel);
	RightUtilityPowerSwitch.Init(0, 0, 34, 31, srf[SRF_SWITCH20], srf[SRF_BORDER_34x31], RightUtilityPowerSwitchRow);

	RightDockingTargetSwitchRow.Init(AID_RIGHTDOCKINGTARGETSWITCH, MainPanel);
	RightDockingTargetSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH20], srf[SRF_BORDER_34x31], RightDockingTargetSwitchRow, this);

	RightModeIntercomSwitchRow.Init(AID_RIGHTMODEINTERCOMSWITCH, MainPanel);
	RightModeIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightModeIntercomSwitchRow);

	RightAudioPowerSwitchRow.Init(AID_RIGHTAUDIOPOWERSWITCH, MainPanel);
	RightAudioPowerSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightAudioPowerSwitchRow);

	RightPadCommSwitchRow.Init(AID_RIGHTPADCOMMSWITCH, MainPanel);
	RightPadCommSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightPadCommSwitchRow);

	RightIntercomSwitchRow.Init(AID_RIGHTINTERCOMSWITCH, MainPanel);
	RightIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightIntercomSwitchRow);

	RightSBandSwitchRow.Init(AID_RIGHTSBANDSWITCH, MainPanel);
	RightSBandSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightSBandSwitchRow);

	RightVHFAMSwitchRow.Init(AID_RIGHTVHFAMSWITCH, MainPanel);
	RightVHFAMSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30], srf[SRF_BORDER_34x31], RightVHFAMSwitchRow);

	GNPowerSwitchRow.Init(AID_GNPOWERSWITCH, MainPanel);
	GNPowerSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], GNPowerSwitchRow);

	MainBusTieSwitchesRow.Init(AID_MAINBUSTIESWITCHES, MainPanel);
	MainBusTieBatAcSwitch.Init( 0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MainBusTieSwitchesRow);
	MainBusTieBatBcSwitch.Init(45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], MainBusTieSwitchesRow);

	BatCHGRSwitchRow.Init(AID_BATTERYCHARGERSWITCH, MainPanel);
	BatCHGRSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], BatCHGRSwitchRow, &ACBus1, NULL, &ACBus2);

	NonessBusSwitchRow.Init(AID_NONESSBUSSWITCH, MainPanel);
	NonessBusSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], NonessBusSwitchRow, MainBusA, 0, MainBusB);

	InteriorLightsFloodSwitchesRow.Init(AID_INTERIORLIGHTSFLOODSSWITCHES, MainPanel);
	InteriorLightsFloodDimSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], InteriorLightsFloodSwitchesRow);
	InteriorLightsFloodFixedSwitch.Init(45, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], InteriorLightsFloodSwitchesRow);

	SPSGaugingSwitchRow.Init(AID_SPSGAUGINGSWITCH, MainPanel);
	SPSGaugingSwitch.Init(0, 0, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], SPSGaugingSwitchRow, &GaugingAc1CircuitBraker, NULL, &GaugingAc2CircuitBraker);

	TelcomSwitchesRow.Init(AID_TELCOMSWITCHES, MainPanel);
	TelcomGroup1Switch.Init(0, 78, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], TelcomSwitchesRow, &ACBus1, NULL, &ACBus2);
	TelcomGroup2Switch.Init(56, 0, 34, 33, srf[SRF_THREEPOSSWITCH305], srf[SRF_BORDER_34x33], TelcomSwitchesRow, &ACBus1, NULL, &ACBus2);

	RightInteriorLightRotariesRow.Init(AID_RIGHTINTERIORLIGHTROTARIES, MainPanel);
	RightIntegralRotarySwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], RightInteriorLightRotariesRow);
	RightFloodRotarySwitch.Init( 133,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], RightInteriorLightRotariesRow);

	SystemTestAttenuator.Init(this, &LeftSystemTestRotarySwitch, &RightSystemTestRotarySwitch, &FlightBus);

	SystemTestRotariesRow.Init(AID_SYSTEMTESTROTARIES, MainPanel);
	LeftSystemTestRotarySwitch.Init(0, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], SystemTestRotariesRow);
	RightSystemTestRotarySwitch.Init(120, 0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], SystemTestRotariesRow);

	SystemTestMeterRow.Init(AID_DCVOLTS_PANEL101, MainPanel);
	SystemTestVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], SystemTestMeterRow, &SystemTestAttenuator);

	SystemTestVoltMeter.SetSurface(srf[SRF_DCVOLTS_PANEL101], 110, 110);

	RNDZXPDRSwitchRow.Init(AID_RNDZXPDRSWITCH, MainPanel);
	RNDZXPDRSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RNDZXPDRSwitchRow);
	
	Panel101LowerSwitchRow.Init(AID_PANEL101LOWERSWITCHES, MainPanel);
    CMRCSHTRSSwitch.Init   (  0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel101LowerSwitchRow);
	WasteH2ODumpSwitch.Init( 78, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel101LowerSwitchRow);
	UrineDumpSwitch.Init   (124, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel101LowerSwitchRow);
	
	Panel100SwitchesRow.Init(AID_PANEL100SWITCHES, MainPanel);
	UtilityPowerSwitch.Init      (  0, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	Panel100FloodDimSwitch.Init  ( 48, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);	
	Panel100FloodFixedSwitch.Init(113, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	GNPowerOpticsSwitch.Init     (200, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	GNPowerIMUSwitch.Init        (260, 23, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	GNPowerIMUSwitch.InitGuard   (259,  0, 36, 69, srf[SRF_SWITCHGUARDS], 0, 180);
	Panel100RNDZXPDRSwitch.Init  (317, 23, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel100SwitchesRow);
	
	Panel100LightingRoatariesRow.Init(AID_PANEL100LIGHTINGROTARIES, MainPanel);
	Panel100NumericRotarySwitch.Init (  0,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], Panel100LightingRoatariesRow);
	Panel100FloodRotarySwitch.Init   (137,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], Panel100LightingRoatariesRow);
	Panel100IntegralRotarySwitch.Init(254,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], Panel100LightingRoatariesRow);
	
	SCIUtilPowerSwitchRow.Init (AID_SCIUTILPOWERSWITCH, MainPanel);
	SCIUtilPowerSwitch.Init( 0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SCIUtilPowerSwitchRow);

	//
	// Panel 225/226/229/250/251/252
	//

	Panel225CircuitBreakersRow.Init(AID_PANEL225CIRCUITBRAKERS, MainPanel);
	PCMTLMGroup1CB.Init			(  0, 100, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow);
	PCMTLMGroup2CB.Init			(  0,   0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow);
	FLTBusMNACB.Init			( 77, 228, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, MainBusA);
	FLTBusMNBCB.Init			( 77, 157, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, MainBusB);
	PMPPowerPrimCB.Init			( 77,  86, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 5.0);
	PMPPowerAuxCB.Init			( 77,  15, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 5.0);
	VHFStationAudioLCB.Init		(170, 395, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightPostLandingBus, 5.0);
	VHFStationAudioCTRCB.Init	(170, 354, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightPostLandingBus, 5.0);
	VHFStationAudioRCB.Init		(170, 313, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightPostLandingBus, 5.0);
	UDLCB.Init					(170, 272, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow);
	HGAFLTBus1CB.Init			(170, 231, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 5.0);
	HGAGroup2CB.Init			(171, 157, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &TelcomGroup2Switch, 2.0);
	SBandFMXMTRFLTBusCB.Init	(171,  85, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 5.0);
	SBandFMXMTRGroup1CB.Init	(171,  15, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &TelcomGroup1Switch, 2.0);
	CentralTimingEquipMNACB.Init(264, 400, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow);
	CentralTimingEquipMNBCB.Init(264, 357, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow);
	RNDZXPNDRFLTBusCB.Init		(264, 314, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 10.0);
	SIGCondrFLTBusCB.Init		(264, 271, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 5.0);
	SBandPWRAmpl1FLTBusCB.Init	(264, 228, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 5.0);
	SBandPWRAmpl1Group1CB.Init	(264, 157, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &TelcomGroup1Switch, 2.0);
	SBandPWRAmpl2FLTBusCB.Init	(264,  86, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &FlightBus, 5.0);
	SBandPWRAmpl2Group1CB.Init	(264,  15, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel225CircuitBreakersRow, &TelcomGroup2Switch, 2.0);
	
	Panel226CircuitBreakersRow.Init(AID_PANEL226CIRCUITBRAKERS, MainPanel);
	FuelCell1PumpsACCB.Init		 (  0, 292, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &FuelCellPumps1Switch);
	FuelCell1ReacsCB.Init		 (  0, 222, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 10.);
	FuelCell1BusContCB.Init		 (  0, 182, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 10.);
	FuelCell1PurgeCB.Init		 (  0, 121, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, MainBusA);
	FuelCell1RadCB.Init			 (  0,  82, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 5.);
	CryogenicH2HTR1CB.Init		 (  0,  43, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	CryogenicH2HTR2CB.Init		 (  0,   4, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	FuelCell2PumpsACCB.Init		 (102, 292, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &FuelCellPumps2Switch);
	FuelCell2ReacsCB.Init		 (102, 222, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 10.);
	FuelCell2BusContCB.Init		 (102, 182, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 10.);
	FuelCell2PurgeCB.Init		 (102, 121, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, MainBusB);
	FuelCell2RadCB.Init			 (102,  82, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 5.);
	CryogenicO2HTR1CB.Init		 (102,  43, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	CryogenicO2HTR2CB.Init		 (102,   4, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	FuelCell3PumpsACCB.Init		 (205, 292, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &FuelCellPumps3Switch);
	FuelCell3ReacsCB.Init		 (205, 222, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 10.);
	FuelCell3BusContCB.Init		 (205, 182, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 10.);
	FuelCell3PurgeCB.Init		 (205, 121, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, MainBusB);
	FuelCell3RadCB.Init			 (205,  82, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &BatteryRelayBus, 5.);
	CryogenicQTYAmpl1CB.Init	 (205,  43, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &ACBus1, 2.);
	CryogenicQTYAmpl2CB.Init	 (205,   4, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &ACBus2, 2.);
	CryogenicFanMotorsAC1ACB.Init(297, 305, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	CryogenicFanMotorsAC1BCB.Init(297, 266, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	CryogenicFanMotorsAC1CCB.Init(297, 227, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	CryogenicFanMotorsAC2ACB.Init(297, 188, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	CryogenicFanMotorsAC2BCB.Init(297, 121, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	CryogenicFanMotorsAC2CCB.Init(297,  82, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	LightingRndzMNACB.Init		 (297,  43, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	LightingRndzMNBCB.Init		 (297,   4, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	LightingFloodMNACB.Init		 (393, 311, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	LightingFloodMNBCB.Init		 (393, 268, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	LightingFloodFLTPLCB.Init	 (393, 225, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	LightingNumIntLEBCB.Init	 (393, 182, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &ACBus2);
	LightingNumIntLMDCCB.Init	 (393, 129, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow, &ACBus1);
	LightingNumIntRMDCCB.Init	 (393,  86, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	RunEVATRGTAC1CB.Init		 (393,  43, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	RunEVATRGTAC2CB.Init		 (393,   0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel226CircuitBreakersRow);
	
	Panel229CircuitBreakersRow.Init(AID_PANEL229CIRCUITBRAKERS, MainPanel);
	if (panel == SATPANEL_RIGHT) {
		UtilityCB1.Init             (  0, 33, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		UtilityCB2.Init             ( 18,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		EPSBatBusACircuitBraker.Init(121, 99, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, &BatteryBusA, 20.0);
		EPSBatBusBCircuitBraker.Init(139, 65, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, &BatteryBusB, 20.0);
	} else {
		TimersMnACircuitBraker.Init       (  0,  41, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusA, 5.0);
		TimersMnBCircuitBraker.Init       (  0,   0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusB, 5.0);
		EPSMnBGroup1CircuitBraker.Init    (  0,  82, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusB);
		EPSMnAGroup1CircuitBraker.Init    (  0, 123, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusA);
		SPSLineHtrsMnBCircuitBraker.Init  ( 71,   9, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		SPSLineHtrsMnACircuitBraker.Init  ( 71,  47, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		EPSMnBGroup2CircuitBraker.Init    ( 71,  85, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusB);
		EPSMnAGroup2CircuitBraker.Init    ( 71, 123, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusA);
		O2VacIonPumpsMnBCircuitBraker.Init(140,   9, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		O2VacIonPumpsMnACircuitBraker.Init(140,  47, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		EPSMnBGroup3CircuitBraker.Init    (140,  85, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusB);
		EPSMnAGroup3CircuitBraker.Init    (140, 123, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusA);
		MainReleasePyroBCircuitBraker.Init(210,   9, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, &PyroBusB, 5.0);
		MainReleasePyroACircuitBraker.Init(210,  47, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, &PyroBusA, 5.0);
		EPSMnBGroup4CircuitBraker.Init    (210,  85, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusB);
		EPSMnAGroup4CircuitBraker.Init    (210, 123, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusA);
		EPSMnBGroup5CircuitBraker.Init    (281,  85, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusB);
		EPSMnAGroup5CircuitBraker.Init    (281, 123, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, MainBusA);
		UtilityCB2.Init                   (352,  85, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		UtilityCB1.Init                   (352, 123, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow);
		EPSBatBusBCircuitBraker.Init      (489,  85, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, &BatteryBusB, 20.0);
		EPSBatBusACircuitBraker.Init      (489, 123, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel229CircuitBreakersRow, &BatteryBusA, 20.0);
	}

	Panel250CircuitBreakersRow.Init(AID_PANEL250CIRCUITBRAKERS, MainPanel);
	BatBusAToPyroBusTieCircuitBraker.Init	(  0, 0, 29, 29, srf[SRF_CIRCUITBRAKER_YELLOW], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, &BatteryBusA, 20.0);
	PyroASeqACircuitBraker.Init				( 55, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, PyroBatteryA, 20.0);
	BatBusBToPyroBusTieCircuitBraker.Init	(115, 0, 29, 29, srf[SRF_CIRCUITBRAKER_YELLOW], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, &BatteryBusB, 20.0);
	PyroBSeqBCircuitBraker.Init				(170, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, PyroBatteryB, 20.0);
	BatAPWRCircuitBraker.Init				(246, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, EntryBatteryA, 80.0);
	BatBPWRCircuitBraker.Init				(304, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, EntryBatteryB, 80.0);
	BatCPWRCircuitBraker.Init				(362, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, EntryBatteryC, 80.0);
	BatCtoBatBusACircuitBraker.Init			(420, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, &BatCPWRCircuitBraker, 80.0);
	BatCtoBatBusBCircuitBraker.Init			(478, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, &BatCPWRCircuitBraker, 80.0);
	BatCCHRGCircuitBraker.Init				(526, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel250CircuitBreakersRow, &BatCPWRCircuitBraker, 10.0);
	
	WasteMGMTOvbdDrainDumpRotaryRow.Init(AID_WASTE_MGMT_OVBD_DUMP, MainPanel);	
	WasteMGMTOvbdDrainDumpRotary.Init( 0, 0, 116, 116, srf[SRF_CSM_WASTE_MGMT_ROTARY], srf[SRF_BORDER_116x116], WasteMGMTOvbdDrainDumpRotaryRow);
		
	WasteMGMTBatteryVentRotaryRow.Init(AID_WASTE_MGMT_BATT_VENT, MainPanel);
	WasteMGMTBatteryVentRotary.Init( 0, 0, 116, 116, srf[SRF_CSM_WASTE_MGMT_ROTARY], srf[SRF_BORDER_116x116], WasteMGMTBatteryVentRotaryRow);
	
	WasteMGMTStoageVentRotaryRow.Init(AID_WASTE_MGMT_STOAGE_VENT, MainPanel);
	WasteMGMTStoageVentRotary.Init( 0, 0, 116, 116, srf[SRF_CSM_WASTE_MGMT_ROTARY], srf[SRF_BORDER_116x116], WasteMGMTStoageVentRotaryRow);
	
	WasteDisposalSwitchRow.Init(AID_CSM_WASTE_DISPOSAL, MainPanel);
	WasteDisposalSwitch.Init( 0, 0, 298, 270, srf[SRF_CSM_WASTE_DISPOSAL_ROTARY], NULL, WasteDisposalSwitchRow);

	//
	// Panel 275
	//
	
	Panel275CircuitBrakersRow.Init(AID_PANEL275CIRCUITBRAKERS, MainPanel);
	InverterPower3MainBCircuitBraker.Init     (  0,   0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusB, 70.0);
	InverterPower3MainACircuitBraker.Init     (  0,  59, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusA, 70.0);
	InverterPower2MainBCircuitBraker.Init     (  0, 104, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusB, 70.0);
	InverterPower1MainACircuitBraker.Init     (  0, 149, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusA, 70.0);
	FlightPostLandingMainBCircuitBraker.Init  (  0, 194, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusB, 10.0);
	FlightPostLandingMainACircuitBraker.Init  (  0, 253, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, MainBusA, 10.0);
	FlightPostLandingBatCCircuitBraker.Init   (  0, 298, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, &BatCPWRCircuitBraker,  7.5);
	FlightPostLandingBatBusBCircuitBraker.Init(  0, 343, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersRow, &BatteryBusB,   7.5);
	
	Panel275CircuitBrakersLowerRow.Init(AID_PANEL275CIRCUITBRAKERS_LOWER, MainPanel);
	FlightPostLandingBatBusACircuitBraker.Init(  0,   0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersLowerRow, &BatteryBusA,   7.5);
	MainBBatBusBCircuitBraker.Init            (  0,  45, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersLowerRow, &BatteryBusB,  80.0);
	MainBBatCCircuitBraker.Init               (  0,  90, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersLowerRow, &BatCPWRCircuitBraker, 80.0);
	MainABatCCircuitBraker.Init               (  0, 135, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersLowerRow, &BatCPWRCircuitBraker, 80.0);
	MainABatBusACircuitBraker.Init            (  0, 194, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel275CircuitBrakersLowerRow, &BatteryBusA,  80.0);
	
	//
	// Panel 276
	//
	
	Panel276CBRow.Init(AID_PANEL276, MainPanel);
	Panel276CB1.Init( 0, 89, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel276CBRow, &InstrumentationPowerFeeder, 5.0);
	Panel276CB2.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel276CBRow, &InstrumentationPowerFeeder, 5.0);
	Panel276CB3.Init(62, 89, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel276CBRow, &InstrumentationPowerFeeder, 5.0);
	Panel276CB4.Init(62,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel276CBRow, &InstrumentationPowerFeeder, 5.0);

	//
	// Panel 277 (Apollo 15 and later)
	//

	if (Panel277)
	{
		Panel277->Panel277SwitchesRow.Init(AID_CSM_PANEL_277, MainPanel);
		Panel277->SPSPressIndFuelSwitch.Init(43, 33, 29, 30, srf[SRF_SWITCH90], srf[SRF_BORDER_29x30], Panel277->Panel277SwitchesRow);
		Panel277->SPSPressIndOxidSwitch.Init(110, 33, 29, 30, srf[SRF_SWITCH90], srf[SRF_BORDER_29x30], Panel277->Panel277SwitchesRow);
		Panel277->CSMLMFinalSepABatABraker.Init(71, 147, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel277->Panel277SwitchesRow);
		Panel277->CSMLMFinalSepBBatBBraker.Init(71, 97, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel277->Panel277SwitchesRow);
	}

	//
	// Panel 278
	//
	
	Panel278CBRow.Init(AID_PANEL278, MainPanel);
	UprightingSystemCompressor1CircuitBraker.Init( 0, 50, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel278CBRow, &BatteryBusA, 25.0);
	UprightingSystemCompressor2CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel278CBRow, &BatteryBusB, 25.0);
	SIVBLMSepPyroACircuitBraker.Init(76, 50, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel278CBRow, &PyroBusA, 7.5);
	SIVBLMSepPyroBCircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel278CBRow, &PyroBusB, 7.5);

	// Panel 278 Mission-Specific Additions

	if (Panel278J)
	{
		Panel278J->Panel278JSwitchesRow.Init(AID_CSM_PANEL_278J, MainPanel);
		Panel278J->ExperimentCovers1Switch.Init(125, 337, 29, 30, srf[SRF_THREEPOSSWITCH90_RIGHT], srf[SRF_BORDER_29x30], Panel278J->Panel278JSwitchesRow);
		Panel278J->ExperimentCovers2Switch.Init(125, 293, 29, 30, srf[SRF_THREEPOSSWITCH90_RIGHT], srf[SRF_BORDER_29x30], Panel278J->Panel278JSwitchesRow);
		Panel278J->SMPowerSourceSwitch.Init(125, 124, 29, 30, srf[SRF_THREEPOSSWITCH90_RIGHT], srf[SRF_BORDER_29x30], Panel278J->Panel278JSwitchesRow);
		Panel278J->SMPowerSourceSwitch.InitGuard(102, 121, 69, 36, srf[SRF_SWITCHGUARDS90_RIGHT], 345);
		Panel278J->O2Tank3IsolSwitch.Init(125, 53, 29, 30, srf[SRF_THREEPOSSWITCH90_RIGHT], srf[SRF_BORDER_29x30], Panel278J->Panel278JSwitchesRow);
		Panel278J->ExperimentCovers1Indicator.Init(36, 341, 23, 23, srf[SRF_INDICATOR90], Panel278J->Panel278JSwitchesRow);
		Panel278J->ExperimentCovers2Indicator.Init(36, 297, 23, 23, srf[SRF_INDICATOR90], Panel278J->Panel278JSwitchesRow);
		Panel278J->O2Tank3IsolIndicator.Init(37, 57, 23, 23, srf[SRF_INDICATOR90], Panel278J->Panel278JSwitchesRow);
		Panel278J->ExperimentCoversDeployBraker.Init(124, 395, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel278J->Panel278JSwitchesRow);
	}

	SCIInstSwitchRow.Init(AID_PANEL227, MainPanel);
	SCIInstSwitch.Init(0, 0, 29, 30, srf[SRF_SWITCH90], srf[SRF_BORDER_29x30], SCIInstSwitchRow); 

	//
	// SATPANEL_LEFT
	//

	LeftCOASPowerSwitchRow.Init(AID_LEFTCOASSWITCH, MainPanel);
	LeftCOASPowerSwitch.Init(0, 0, 34, 31, srf[SRF_SWITCH20LEFT], srf[SRF_BORDER_34x31], LeftCOASPowerSwitchRow);

	LeftUtilityPowerSwitchRow.Init(AID_LEFTTUTILITYPOWERSWITCH, MainPanel);
	LeftUtilityPowerSwitch.Init(0, 0, 34, 31, srf[SRF_SWITCH20LEFT], srf[SRF_BORDER_34x31], LeftUtilityPowerSwitchRow);

	PostLandingBCNLTSwitchRow.Init(AID_POSTLANDINGBCNLTSWITCH, MainPanel);
	PostLandingBCNLTSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH20LEFT], srf[SRF_BORDER_34x31], PostLandingBCNLTSwitchRow);

	PostLandingDYEMarkerSwitchRow.Init(AID_POSTLANDINGDYEMARKERSWITCH, MainPanel);
	PostLandingDYEMarkerSwitch.Init		(6, 25, 34, 31, srf[SRF_GUARDEDSWITCH20], srf[SRF_BORDER_34x31], PostLandingDYEMarkerSwitchRow);
	PostLandingDYEMarkerSwitch.InitGuard(0,  0, 46, 75, srf[SRF_SWITCHGUARDPANEL15], srf[SRF_BORDER_46x75]);

	PostLandingVentSwitchRow.Init(AID_POSTLANDINGVENTSWITCH, MainPanel);
	PostLandingVentSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH20LEFT], srf[SRF_BORDER_34x31], PostLandingVentSwitchRow);

	LeftModeIntercomVOXSensThumbwheelSwitchRow.Init(AID_LEFTMODEINTERCOMVOXSENSTHUMBWHEEL, MainPanel);
	LeftModeIntercomVOXSensThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], srf[SRF_BORDER_33x43], LeftModeIntercomVOXSensThumbwheelSwitchRow);

	LeftPowerMasterVolumeThumbwheelSwitchRow.Init(AID_LEFTPOWERMASTERVOLUMETHUMBWHEEL, MainPanel);
	LeftPowerMasterVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], srf[SRF_BORDER_33x43], LeftPowerMasterVolumeThumbwheelSwitchRow); // , VOLUME_COMMS2, &soundlib);

	LeftPadCommVolumeThumbwheelSwitchRow.Init(AID_LEFTPADCOMMVOLUMETHUMBWHEEL, MainPanel);
	LeftPadCommVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], srf[SRF_BORDER_33x43], LeftPadCommVolumeThumbwheelSwitchRow);

	LeftIntercomVolumeThumbwheelSwitchRow.Init(AID_LEFTINTERCOMVOLUMETHUMBWHEEL, MainPanel);
	LeftIntercomVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], srf[SRF_BORDER_33x43], LeftIntercomVolumeThumbwheelSwitchRow);

	LeftSBandVolumeThumbwheelSwitchRow.Init(AID_LEFTSBANDVOLUMETHUMBWHEEL, MainPanel);
	LeftSBandVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], srf[SRF_BORDER_33x43], LeftSBandVolumeThumbwheelSwitchRow);

	LeftVHFAMVolumeThumbwheelSwitchRow.Init(AID_LEFTVHFVOLUMETHUMBWHEEL, MainPanel);
	LeftVHFAMVolumeThumbwheelSwitch.Init(0, 0, 33, 43, srf[SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT], srf[SRF_BORDER_33x43], LeftVHFAMVolumeThumbwheelSwitchRow);

	LeftModeIntercomSwitchRow.Init(AID_LEFTMODEINTERCOMSWITCH, MainPanel);
	LeftModeIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftModeIntercomSwitchRow);

	LeftAudioPowerSwitchRow.Init(AID_LEFTAUDIOPOWERSWITCH, MainPanel);
	LeftAudioPowerSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftAudioPowerSwitchRow);

	LeftPadCommSwitchRow.Init(AID_LEFTPADCOMMSWITCH, MainPanel);
	LeftPadCommSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftPadCommSwitchRow);

	LeftIntercomSwitchRow.Init(AID_LEFTINTERCOMSWITCH, MainPanel);
	LeftIntercomSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftIntercomSwitchRow);

	LeftSBandSwitchRow.Init(AID_LEFTSBANDSWITCH, MainPanel);
	LeftSBandSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftSBandSwitchRow);

	LeftVHFAMSwitchRow.Init(AID_LEFTVHFAMSWITCH, MainPanel);
	LeftVHFAMSwitch.Init(0, 0, 34, 31, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x31], LeftVHFAMSwitchRow);

	LeftAudioControlSwitchRow.Init(AID_LEFTAUDIOCONTROLSWITCH, MainPanel);
	LeftAudioControlSwitch.Init(0, 0, 34, 34, srf[SRF_THREEPOSSWITCH30LEFT], srf[SRF_BORDER_34x34], LeftAudioControlSwitchRow);

	LeftSuitPowerSwitchRow.Init(AID_LEFTSUITPOWERSWITCH, MainPanel);
	LeftSuitPowerSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30LEFT], srf[SRF_BORDER_34x34], LeftSuitPowerSwitchRow);

	VHFRNGSwitchRow.Init(AID_VHFRNGSWITCH, MainPanel);
	VHFRNGSwitch.Init(0, 0, 34, 34, srf[SRF_SWITCH30LEFT], srf[SRF_BORDER_34x34], VHFRNGSwitchRow);

	StabContSystemCircuitBrakerRow.Init(AID_STABCONTCIRCUITBREAKERS, MainPanel);
	StabContSystemTVCAc1CircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabContSystemCircuitBrakerRow, &ACBus1PhaseA);
	StabContSystemAc1CircuitBraker.Init( 52,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabContSystemCircuitBrakerRow, &ACBus1);
	StabContSystemAc2CircuitBraker.Init(135,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabContSystemCircuitBrakerRow, &ACBus2);

	AutoRCSSelectSwitchesRow.Init(AID_AUTORCSSELECTSWITCHES, MainPanel);
	AcRollA1Switch.Init(  0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &ACRollMnACircuitBraker, 0, &ACRollMnBCircuitBraker);
	AcRollC1Switch.Init( 45, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &ACRollMnACircuitBraker, 0, &ACRollMnBCircuitBraker);
	AcRollA2Switch.Init( 90, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &ACRollMnACircuitBraker, 0, &ACRollMnBCircuitBraker);
	AcRollC2Switch.Init(135, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &ACRollMnACircuitBraker, 0, &ACRollMnBCircuitBraker);
	BdRollB1Switch.Init(180, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &BDRollMnACircuitBraker, 0, &BDRollMnBCircuitBraker);
	BdRollD1Switch.Init(225, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &BDRollMnACircuitBraker, 0, &BDRollMnBCircuitBraker);
	BdRollB2Switch.Init(270, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &BDRollMnACircuitBraker, 0, &BDRollMnBCircuitBraker);
	BdRollD2Switch.Init(315, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &BDRollMnACircuitBraker, 0, &BDRollMnBCircuitBraker);
	PitchA3Switch.Init(360, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &PitchMnACircuitBraker, 0, &PitchMnBCircuitBraker);
	PitchC3Switch.Init(405, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &PitchMnACircuitBraker, 0, &PitchMnBCircuitBraker);
	PitchA4Switch.Init(450, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &PitchMnACircuitBraker, 0, &PitchMnBCircuitBraker);
	PitchC4Switch.Init(495, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &PitchMnACircuitBraker, 0, &PitchMnBCircuitBraker);
	YawB3Switch.Init(540, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &YawMnACircuitBraker, 0, &YawMnBCircuitBraker);
	YawD3Switch.Init(585, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &YawMnACircuitBraker, 0, &YawMnBCircuitBraker);
	YawB4Switch.Init(630, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &YawMnACircuitBraker, 0, &YawMnBCircuitBraker);
	YawD4Switch.Init(675, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], AutoRCSSelectSwitchesRow, &YawMnACircuitBraker, 0, &YawMnBCircuitBraker);

	StabilizationControlSystemCircuitBrakerRow.Init(AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS, MainPanel);
	ECATVCAc2CircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &ACBus2PhaseA);
	DirectUllMnACircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnAGroup5CircuitBraker);
	DirectUllMnBCircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnBGroup5CircuitBraker);
	ContrDirectMnA1CircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnAGroup5CircuitBraker);
	ContrDirectMnB1CircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnBGroup5CircuitBraker);
	ContrDirectMnA2CircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnAGroup5CircuitBraker);
	ContrDirectMnB2CircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnBGroup5CircuitBraker);
	ACRollMnACircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnAGroup2CircuitBraker, 15);
	ACRollMnBCircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnBGroup2CircuitBraker, 15);
	BDRollMnACircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnAGroup4CircuitBraker, 15);
	BDRollMnBCircuitBraker.Init(380,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnBGroup4CircuitBraker, 15);
	PitchMnACircuitBraker.Init(418,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnAGroup1CircuitBraker, 15);
	PitchMnBCircuitBraker.Init(456,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnBGroup1CircuitBraker, 15);
	YawMnACircuitBraker.Init(494,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnAGroup3CircuitBraker, 15);
	YawMnBCircuitBraker.Init(532,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystemCircuitBrakerRow, &EPSMnBGroup3CircuitBraker, 15);

	StabilizationControlSystem2CircuitBrakerRow.Init(AID_STABILIZATIONCONTROLSYSTEMCIRCUITBREAKERS2, MainPanel);
	OrdealAc2CircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &ACBus2);
	OrdealMnBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnBGroup3CircuitBraker);
	ContrAutoMnACircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnAGroup1CircuitBraker);
	ContrAutoMnBCircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnBGroup1CircuitBraker);
	LogicBus12MnACircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnAGroup3CircuitBraker, 3.0);
	LogicBus34MnACircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnAGroup1CircuitBraker, 3.0);
	LogicBus14MnBCircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnBGroup3CircuitBraker, 3.0);
	LogicBus23MnBCircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnBGroup1CircuitBraker, 3.0);
	SystemMnACircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnAGroup2CircuitBraker, 15.0);
	SystemMnBCircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], StabilizationControlSystem2CircuitBrakerRow, &EPSMnBGroup2CircuitBraker, 15.0);

	FloodDimSwitchRow.Init(AID_FLOODDIMSWITCH, MainPanel);
	FloodDimSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FloodDimSwitchRow);

	FloodFixedSwitchRow.Init(AID_FLOODFIXEDSWITCH, MainPanel);
	FloodFixedSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], FloodFixedSwitchRow);

	ReactionControlSystemCircuitBrakerRow.Init(AID_REACTIONCONTROLSYSTEMCIRCUITBREAKERS, MainPanel);
	CMHeater1MnACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnAGroup5CircuitBraker);
	CMHeater2MnBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnBGroup5CircuitBraker);
	SMHeatersAMnBCircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnBGroup3CircuitBraker, 7.5);
	SMHeatersCMnBCircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnBGroup1CircuitBraker, 7.5);
	SMHeatersBMnACircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnAGroup3CircuitBraker, 7.5);
	SMHeatersDMnACircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnAGroup1CircuitBraker, 7.5);
	PrplntIsolMnACircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnAGroup1CircuitBraker, 10.0);
	PrplntIsolMnBCircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnBGroup1CircuitBraker, 10.0);
	RCSLogicMnACircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnAGroup5CircuitBraker);
	RCSLogicMnBCircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnBGroup5CircuitBraker);
	EMSMnACircuitBraker.Init(380,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnAGroup4CircuitBraker);
	EMSMnBCircuitBraker.Init(418,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnBGroup4CircuitBraker);
	DockProbeMnACircuitBraker.Init(456,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnAGroup4CircuitBraker);
	DockProbeMnBCircuitBraker.Init(494,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ReactionControlSystemCircuitBrakerRow, &EPSMnBGroup4CircuitBraker);

	FloatBagSwitchRow.Init(AID_FLOATBAGSWITCHES, MainPanel);
	FloatBagSwitch1.Init(  0, 0, 38, 52, srf[SRF_SWLEVERTHREEPOS], srf[SRF_BORDER_38x52], FloatBagSwitchRow);
	FloatBagSwitch2.Init( 58, 0, 38, 52, srf[SRF_SWLEVERTHREEPOS], srf[SRF_BORDER_38x52], FloatBagSwitchRow);
	FloatBagSwitch3.Init(116, 0, 38, 52, srf[SRF_SWLEVERTHREEPOS], srf[SRF_BORDER_38x52], FloatBagSwitchRow);

	SeqEventsContSystemSwitchesRow.Init(AID_SEQEVENTSCONTSYSTEM, MainPanel);
	SECSLogic1Switch.Init( 0, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow);
	SECSLogic2Switch.Init(40, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow);	
	PyroArmASwitch.Init( 80, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow);
	PyroArmBSwitch.Init(133, 0, 38, 52, srf[SRF_SWITCHLEVER], srf[SRF_BORDER_38x52], SeqEventsContSystemSwitchesRow);

	EDSPowerSwitchRow.Init(AID_EDSPOWERSWITCH, MainPanel);
	EDSPowerSwitch.Init(0, 0, 34, 33, srf[SRF_SWITCH305LEFT], srf[SRF_BORDER_34x33], EDSPowerSwitchRow);

	TVCServoPowerSwitchesRow.Init(AID_TVCSERVOPOWERSWITCHES, MainPanel);
	TVCServoPower1Switch.Init( 0, 0, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], TVCServoPowerSwitchesRow);
	TVCServoPower2Switch.Init(25, 35, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], TVCServoPowerSwitchesRow);

	LogicPowerSwitchRow.Init(AID_LOGICPOWERSWITCH, MainPanel);
	LogicPowerSwitch.Init(0, 0, 34, 33, srf[SRF_SWITCH305LEFT], srf[SRF_BORDER_34x33], LogicPowerSwitchRow);

	SIGCondDriverBiasPowerSwitchesRow.Init(AID_SIGCONDDRIVERBIASPOWERSWITCHES, MainPanel);
	SIGCondDriverBiasPower1Switch.Init( 0,  0, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], SIGCondDriverBiasPowerSwitchesRow, 
		&StabContSystemAc1CircuitBraker, NULL, &StabContSystemAc2CircuitBraker);
	SIGCondDriverBiasPower2Switch.Init(25, 35, 34, 33, srf[SRF_THREEPOSSWITCH305LEFT], srf[SRF_BORDER_34x33], SIGCondDriverBiasPowerSwitchesRow,
		&StabContSystemAc1CircuitBraker, NULL, &StabContSystemAc2CircuitBraker);

	LeftInteriorLightRotariesRow.Init(AID_LEFTINTERIORLIGHTROTARIES, MainPanel);
	NumericRotarySwitch.Init(0,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], LeftInteriorLightRotariesRow);
	FloodRotarySwitch.Init(119,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], LeftInteriorLightRotariesRow);
	IntegralRotarySwitch.Init(238,  0, 90, 90, srf[SRF_ROTATIONALSWITCH], srf[SRF_BORDER_90x90], LeftInteriorLightRotariesRow);
	
	FDAIPowerRotaryRow.Init(AID_FDAIPOWERROTARY, MainPanel);
	FDAIPowerRotarySwitch.Init(0,  0, 90, 90, srf[SRF_FDAIPOWERROTARY], srf[SRF_BORDER_90x90], FDAIPowerRotaryRow, &fdaiLeft, &fdaiRight,
		                       &SystemMnACircuitBraker, &SystemMnBCircuitBraker, &StabContSystemAc1CircuitBraker, &StabContSystemAc2CircuitBraker,
							   &GPFPIPitch1Meter, &GPFPIPitch2Meter, &GPFPIYaw1Meter, &GPFPIYaw2Meter, &eda);

	SCSElectronicsPowerRotaryRow.Init(AID_SCSELECTRONICSPOWERROTARY, MainPanel);
	SCSElectronicsPowerRotarySwitch.Init(0,  0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_90x90], SCSElectronicsPowerRotaryRow);

	BMAGPowerRotary1Row.Init(AID_BMAGPOWERROTARY1, MainPanel);
	BMAGPowerRotary1Switch.Init(0,  0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_90x90], BMAGPowerRotary1Row, &bmag1);

	BMAGPowerRotary2Row.Init(AID_BMAGPOWERROTARY2, MainPanel);
	BMAGPowerRotary2Switch.Init(0,  0, 90, 90, srf[SRF_ECSGLYCOLPUMPROTARY], srf[SRF_BORDER_90x90], BMAGPowerRotary2Row, &bmag2);

	DirectO2RotaryRow.Init(AID_DIRECTO2ROTARY, MainPanel);
	DirectO2RotarySwitch.Init(0,  0, 70, 70, srf[SRF_DIRECTO2ROTARY], srf[SRF_BORDER_70x70], DirectO2RotaryRow, (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE"));

	ServicePropulsionSysCircuitBrakerRow.Init(AID_SERVICEPROPULSIONSYSCIRCUITBREAKERS, MainPanel);
	GaugingMnACircuitBraker.Init(0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &EPSMnAGroup4CircuitBraker);
	GaugingMnBCircuitBraker.Init( 38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &EPSMnBGroup4CircuitBraker);
	GaugingAc1CircuitBraker.Init( 76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &ACBus1);
	GaugingAc2CircuitBraker.Init(114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &ACBus2);
	HeValveMnACircuitBraker.Init(152,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &EPSMnAGroup4CircuitBraker);
	HeValveMnBCircuitBraker.Init(190,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &EPSMnBGroup4CircuitBraker);
	PitchBatACircuitBraker.Init(228,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusA);
	PitchBatBCircuitBraker.Init(266,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusB);
	YawBatACircuitBraker.Init(304,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusA);
	YawBatBCircuitBraker.Init(342,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &BatteryBusB);
	PilotValveMnACircuitBraker.Init(380,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &EPSMnAGroup5CircuitBraker);
	PilotValveMnBCircuitBraker.Init(418,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ServicePropulsionSysCircuitBrakerRow, &EPSMnBGroup5CircuitBraker);

	FloatBagCircuitBrakerRow.Init(AID_FLOATBAGCIRCUITBREAKERS, MainPanel);
	FloatBag1BatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], FloatBagCircuitBrakerRow, &EPSBatBusACircuitBraker, 5.0);
	FloatBag2BatBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], FloatBagCircuitBrakerRow, &EPSBatBusBCircuitBraker, 5.0);
	FloatBag3FLTPLCircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], FloatBagCircuitBrakerRow, &FlightPostLandingBus, 5.0);

	SeqEventsContSysCircuitBrakerRow.Init(AID_SEQEVENTSCONTSYSCIRCUITBREAKERS, MainPanel);
	SECSLogicBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &EPSBatBusACircuitBraker, 15.0);
	SECSLogicBatBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &EPSBatBusBCircuitBraker, 15.0);
	SECSArmBatACircuitBraker.Init(  76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &EPSBatBusACircuitBraker, 5.0); 
	SECSArmBatBCircuitBraker.Init( 114,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], SeqEventsContSysCircuitBrakerRow, &EPSBatBusBCircuitBraker, 5.0); 
	
	EDSCircuitBrakerRow.Init(AID_EDSCIRCUITBREAKERS, MainPanel);
	EDS1BatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EDSCircuitBrakerRow, &EPSBatBusACircuitBraker, 5.0);
	EDS2BatCCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EDSCircuitBrakerRow, &BatCCHRGCircuitBraker, 5.0);
	EDS3BatBCircuitBraker.Init(76,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], EDSCircuitBrakerRow, &EPSBatBusACircuitBraker, 5.0);

	ELSCircuitBrakerRow.Init(AID_ELSCIRCUITBREAKERS, MainPanel);
	ELSBatACircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ELSCircuitBrakerRow, &EPSBatBusACircuitBraker, 5.0);
	ELSBatBCircuitBraker.Init(38,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], ELSCircuitBrakerRow, &EPSBatBusBCircuitBraker, 5.0);

	PLVentCircuitBrakerRow.Init(AID_PLVENTCIRCUITBREAKER, MainPanel);
	FLTPLCircuitBraker.Init( 0,  0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], PLVentCircuitBrakerRow, &FlightPostLandingBus);

	//RightWindowCoverRow.Init(AID_RIGHTWINDOWCOVER, MainPanel);
	//RightWindowCoverSwitch.Init(0, 0, 525, 496, srf[SRF_CSMRIGHTWINDOWCOVER], RightWindowCoverRow);

	//
	// SATPANEL_LEFT_RNDZ_WINDOW
	//

	OrbiterAttitudeToggleRow.Init(AID_SM_RCS_MODE, MainPanel);
	OrbiterAttitudeToggle.Init(28, 33, 23, 20, srf[SRF_SWITCHUPSMALL], srf[SRF_BORDER_23x20], OrbiterAttitudeToggleRow);

	/////////////////////////////
	// G&N lower equipment bay //
	/////////////////////////////

	ZeroSwitchRow.Init(AID_GNZEROSWITCH, MainPanel);
	OpticsZeroSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ZeroSwitchRow, 68);

	TelescopeTrunnionSwitchRow.Init(AID_TELESCOPETRUNNIONSWITCH, MainPanel);
	ControllerTelescopeTrunnionSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], TelescopeTrunnionSwitchRow);

	ControllerCouplingSwitchRow.Init(AID_CONTROLLERCOUPLINGSWITCH, MainPanel);
	OpticsModeSpeedSwitchesRow.Init(AID_OPTICSMODESPEEDSWITCHES, MainPanel);

	if (panel == SATPANEL_SEXTANT || panel == SATPANEL_TELESCOPE) {
		OpticsModeSwitch.Init(0, 0, 23, 20, srf[SRF_SWITCHUPSMALL], srf[SRF_BORDER_23x20], OpticsModeSpeedSwitchesRow);
		ControllerSpeedSwitch.Init(101, 0, 23, 20, srf[SRF_THREEPOSSWITCHSMALL], srf[SRF_BORDER_23x20], OpticsModeSpeedSwitchesRow);
		ControllerCouplingSwitch.Init(0, 0, 23, 20, srf[SRF_SWITCHUPSMALL], srf[SRF_BORDER_23x20], ControllerCouplingSwitchRow);
	} else {
		OpticsModeSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], OpticsModeSpeedSwitchesRow);
		ControllerSpeedSwitch.Init(109, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], OpticsModeSpeedSwitchesRow);
		ControllerCouplingSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ControllerCouplingSwitchRow);
	}

	ConditionLampsSwitchRow.Init(AID_CONDITIONLAMPSSWITCH, MainPanel);
	ConditionLampsSwitch.Init(0, 0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ConditionLampsSwitchRow);

	UPTLMSwitchRow.Init(AID_UPLINKTELEMETRYSWITCH, MainPanel);
	UPTLMSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], UPTLMSwitchRow);

	OpticsHandcontrollerSwitchRow.Init(AID_OPTICS_HANDCONTROLLER, MainPanel);
	OpticsHandcontrollerSwitch.Init(0, 0, 45, 49, srf[SRF_OPTICS_HANDCONTROLLER], srf[SRF_BORDER_45x49], OpticsHandcontrollerSwitchRow, this);

	OpticsMarkButtonRow.Init(AID_MARKBUTTON, MainPanel);
	OpticsMarkButton.Init(0, 0, 28, 32, srf[SRF_MARK_BUTTONS], srf[SRF_BORDER_28x32], OpticsMarkButtonRow);

	OpticsMarkRejectButtonRow.Init(AID_MARKREJECT, MainPanel);
	OpticsMarkRejectButton.Init(0, 0, 23, 26, srf[SRF_MARK_BUTTONS], NULL, OpticsMarkRejectButtonRow, 0, 32);

	MinImpulseHandcontrollerSwitchRow.Init(AID_MINIMPULSE_HANDCONTROLLER, MainPanel);
	MinImpulseHandcontrollerSwitch.Init(0, 0, 45, 49, srf[SRF_MINIMPULSE_HANDCONTROLLER], srf[SRF_BORDER_45x49], MinImpulseHandcontrollerSwitchRow, this);

	if (Panel181)
	{
		Panel181->SMSector1SwitchesRow.Init(AID_CSM_PANEL_181, MainPanel);
		Panel181->SMSector1DoorJettisonSwitch.Init(79, 169, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1DoorJettisonSwitch.InitGuard(78, 146, 36, 69, srf[SRF_SWITCHGUARDS], srf[SRF_BORDER_36x69]);
		Panel181->SMSector1LogicPower1Switch.Init(144, 155, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1LogicPower2Switch.Init(188, 155, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1Cryo3ACPowerSwitch.Init(82, 70, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1SMACPowerSwitch.Init(188, 70, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1AC2ASystemBraker.Init(264, 70, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1AC2BSystemBraker.Init(302, 70, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1AC2CSystemBraker.Init(340, 70, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1LogicPowerMNABraker.Init(264, 155, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel181->SMSector1SwitchesRow);
		Panel181->SMSector1LogicPowerMNBBraker.Init(302, 155, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel181->SMSector1SwitchesRow);
	}

	///////////////////////////////////
	// Panel 300/301/302/303/305/306 //
	///////////////////////////////////
	
	SuitCircuitFlow300SwitchRow.Init(AID_SUIT_FLOW_CONTROL_LEVER_300, MainPanel);
	SuitCircuitFlow300Switch.Init(0, 0, 87, 111, srf[SRF_SUIT_FLOW_CONTROL_LEVER], srf[SRF_BORDER_87x111], SuitCircuitFlow300SwitchRow);
	
	SuitCircuitFlow301SwitchRow.Init(AID_SUIT_FLOW_CONTROL_LEVER_301, MainPanel);
	SuitCircuitFlow301Switch.Init(0, 0, 87, 111, srf[SRF_SUIT_FLOW_CONTROL_LEVER], srf[SRF_BORDER_87x111], SuitCircuitFlow301SwitchRow);
	
	SuitCircuitFlow302SwitchRow.Init(AID_SUIT_FLOW_CONTROL_LEVER_302, MainPanel);
	SuitCircuitFlow302Switch.Init(0, 0, 87, 111, srf[SRF_SUIT_FLOW_CONTROL_LEVER], srf[SRF_BORDER_87x111], SuitCircuitFlow302SwitchRow);
	
	SecondaryCabinTempValveRow.Init(AID_CSM_SEC_CABIN_TEMP_VALVE, MainPanel);
	SecondaryCabinTempValve.Init(0, 0, 23, 23, srf[SRF_CSM_SEC_CABIN_TEMP_VALVE], srf[SRF_BORDER_23x23], SecondaryCabinTempValveRow);
	
	FoodPreparationWaterLeversRow.Init(AID_FOOD_PREPARATION_WATER, MainPanel);
	FoodPreparationWaterHotLever.Init (  0, 0, 118, 118, srf[SRF_CSM_FOOT_PREP_WATER_LEVER], srf[SRF_BORDER_118x118], FoodPreparationWaterLeversRow);
	FoodPreparationWaterColdLever.Init(137, 0, 118, 118, srf[SRF_CSM_FOOT_PREP_WATER_LEVER], srf[SRF_BORDER_118x118], FoodPreparationWaterLeversRow);

	Panel306Row.Init(AID_CSM_PANEL_306, MainPanel);
	EventTimerUpDown306Switch.Init(0, 0, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], Panel306Row, &EventTimer306Display);
	EventTimerUpDown306Switch.SetDelayTime(1);
	EventTimerControl306Switch.Init(0, 46, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], Panel306Row, &EventTimer306Display);
	EventTimerControl306Switch.SetDelayTime(1);
	EventTimer306MinutesSwitch.Init(0, 92, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], Panel306Row, TIME_UPDATE_MINUTES, &EventTimer306Display);
	EventTimer306SecondsSwitch.Init(0, 138, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], Panel306Row, TIME_UPDATE_SECONDS, &EventTimer306Display);
	SaturnEventTimer306Display.Init(Panel306Row, this); 	// dummy switch/display for checklist controller
	MissionTimer306HoursSwitch.Init(0, 184, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], Panel306Row, TIME_UPDATE_HOURS, &MissionTimer306Display);
	MissionTimer306MinutesSwitch.Init(0, 230, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], Panel306Row, TIME_UPDATE_MINUTES, &MissionTimer306Display);
	MissionTimer306SecondsSwitch.Init(0, 276, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], Panel306Row, TIME_UPDATE_SECONDS, &MissionTimer306Display);

	MissionTimer306SwitchRow.Init(AID_CSM_PANEL_306_MISSIONTIMERSWITCH, MainPanel);
	MissionTimer306Switch.Init(0, 0, 29, 30, srf[SRF_THREEPOSSWITCH90_LEFT], srf[SRF_BORDER_29x30], MissionTimer306SwitchRow, &MissionTimer306Display);


	/////////////////
	// Panel 10/12 //
	/////////////////
	
	LeftAudioSwitchesRow.Init(AID_PANEL10_LEFT_SWITCHES, MainPanel);
	ModeIntercomSwitch.Init	 (0,   0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], LeftAudioSwitchesRow);
	PadComSwitch.Init	     (0, 112, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], LeftAudioSwitchesRow);
	SBandSwitch.Init	     (0, 226, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], LeftAudioSwitchesRow);

	LeftAudioThumbwheelsRow.Init(AID_PANEL10_LEFT_THUMWBWHEELS, MainPanel);
	LeftAudioVOXSensThumbwheel.Init     (0,   0, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], LeftAudioThumbwheelsRow);
	LeftAudioPadComVolumeThumbwheel.Init(0, 112, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], LeftAudioThumbwheelsRow);
	LeftAudioSBandVolumeThumbwheel.Init (0, 226, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], LeftAudioThumbwheelsRow);

	CenterAudioSwitchesRow.Init(AID_PANEL10_CENTER_SWITCHES, MainPanel);
	CenterSuitPowerSwitch.Init   (0,   0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CenterAudioSwitchesRow);
	CenterAudioControlSwitch.Init(0, 114, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], CenterAudioSwitchesRow);
	
	RightAudioThumbwheelsRow.Init(AID_PANEL10_RIGHT_THUMBWHEELS, MainPanel);
	RightAudioMasterVolumeThumbwheel.Init  (0,   0, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], RightAudioThumbwheelsRow);
	RightAudioIntercomVolumeThumbwheel.Init(0, 112, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], RightAudioThumbwheelsRow);
	RightAudioVHFAMVolumeThumbwheel.Init   (0, 226, 17, 36, srf[SRF_THUMBWHEEL_SMALLFONTS], srf[SRF_BORDER_17x36], RightAudioThumbwheelsRow);
	
	RightAudioSwitchesRow.Init(AID_PANEL10_RIGHT_SWITCHES, MainPanel);
	PowerAudioSwitch.Init(0,   0, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RightAudioSwitchesRow);
	IntercomSwitch.Init  (0, 112, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RightAudioSwitchesRow);
	VHFAMSwitch.Init     (0, 226, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], RightAudioSwitchesRow);
	
	LMTunnelVentValveRow.Init(AID_LM_TUNNEL_VENT_VALVE, MainPanel);
	LMTunnelVentValve.Init(0, 0, 38, 38, srf[SRF_CSM_LM_TUNNEL_VENT_VALVE], srf[SRF_BORDER_38x38], LMTunnelVentValveRow);
	
	LMDPGaugeRow.Init(AID_LM_DP_GAUGE, MainPanel, &GaugePower);
	LMDPGauge.Init(g_Param.pen[6], g_Param.pen[6], LMDPGaugeRow, this);

	PressEqualValveRow.Init(AID_PRESS_EQUAL_VALVE_HANDLE, MainPanel);
	PressEqualValve.Init(0, 0, 121, 121, srf[SRF_CSM_PRESS_EQUAL_HANDLE], NULL, PressEqualValveRow);
	
	////////////////////////
	// Panel 325/326 etc. //
	////////////////////////
	
	GlycolToRadiatorsLeverRow.Init(AID_GLYCOLTORADIATORSLEVER, MainPanel);
	GlycolToRadiatorsLever.Init(0, 0, 32, 160, srf[SRF_GLYCOLLEVER], srf[SRF_BORDER_32x160], GlycolToRadiatorsLeverRow);

	CabinPressureReliefLever1Row.Init(AID_CABINPRESSURERELIEFLEVER1, MainPanel);
	CabinPressureReliefLever1.Init(0, 0, 150, 80, srf[SRF_CABINRELIEFUPPERLEVER], srf[SRF_BORDER_150x80], CabinPressureReliefLever1Row);

	CabinPressureReliefLever2Row.Init(AID_CABINPRESSURERELIEFLEVER2, MainPanel);
	CabinPressureReliefLever2.Init(66, 8, 200, 80, srf[SRF_CABINRELIEFLOWERLEVER], srf[SRF_BORDER_200x80], CabinPressureReliefLever2Row);
	CabinPressureReliefLever2.InitGuard(srf[SRF_CABINRELIEFGUARDLEVER], &soundlib);

	GlycolReservoirRotariesRow.Init(AID_GLYCOLRESERVOIRROTARIES, MainPanel);
	GlycolReservoirInletRotary.Init (0, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], GlycolReservoirRotariesRow);
	GlycolReservoirBypassRotary.Init(0, 106, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78],  GlycolReservoirRotariesRow);
	GlycolReservoirOutletRotary.Init(0, 212, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78],  GlycolReservoirRotariesRow);
	
	OxygenRotariesRow.Init(AID_OXYGENROTARIES, MainPanel);
	OxygenSurgeTankRotary.Init      (  0, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], OxygenRotariesRow);
	OxygenSMSupplyRotary.Init       (106, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], OxygenRotariesRow);
	OxygenRepressPackageRotary.Init (212, 0, 78, 78, srf[SRF_ECSROTARY], srf[SRF_BORDER_78x78], OxygenRotariesRow);

	ORDEALSwitchesRow.Init(AID_ORDEALSWITCHES, MainPanel);
	ORDEALFDAI1Switch.Init	 ( 55,  43, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALFDAI2Switch.Init	 (168,  43, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALEarthSwitch.Init	 (264,  43, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALLightingSwitch.Init( 55, 132, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow); 
	ORDEALModeSwitch.Init	 (215, 132, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);	
	ORDEALSlewSwitch.Init	 (264, 132, 34, 29, srf[SRF_THREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALAltSetRotary.Init  (346,  63, 84, 84, srf[SRF_ORDEAL_ROTARY], srf[SRF_BORDER_84x84], ORDEALSwitchesRow);

	PanelOrdeal.Init(ORDEALSwitchesRow, this); 	// dummy switch/display for checklist controller

	///////////////
	// Panel 380 //
	///////////////

	SuitCircuitReturnValveLeverRow.Init(AID_SUITCIRCUITRETURNVALVE, MainPanel);
	SuitCircuitReturnValveLever.Init(0, 0, 160, 32, srf[SRF_SUITRETURN_LEVER], srf[SRF_BORDER_160x32], SuitCircuitReturnValveLeverRow);

	O2DemandRegulatorRotaryRow.Init(AID_CSM_DEMAND_REG_ROTARY, MainPanel);
	O2DemandRegulatorRotary.Init(  0, 0, 70, 70, srf[SRF_CSM_DEMAND_REG_ROTARY], srf[SRF_BORDER_70x70], O2DemandRegulatorRotaryRow);

	SuitTestRotaryRow.Init(AID_CSM_SUIT_TEST_LEVER, MainPanel);
	SuitTestRotary.Init(  0, 0, 324, 324, srf[SRF_CSM_SUIT_TEST_LEVER], srf[SRF_BORDER_194x324], SuitTestRotaryRow);

	///////////////////////////
	// Panel 375/377/378/379 //
	///////////////////////////
	
	OxygenSurgeTankValveRotaryRow.Init(AID_OXYGEN_SURGE_TANK_VALVE, MainPanel);
	OxygenSurgeTankValveRotary.Init(0, 0, 34, 34, srf[SRF_OXYGEN_SURGE_TANK_VALVE], srf[SRF_BORDER_34x34], OxygenSurgeTankValveRotaryRow);
	
	GlycolToRadiatorsRotaryRow.Init(AID_GLYCOL_TO_RADIATORS_KNOB, MainPanel);
	GlycolToRadiatorsRotary.Init(0, 0, 31, 31, srf[SRF_GLYCOL_TO_RADIATORS_KNOB], srf[SRF_BORDER_31x31], GlycolToRadiatorsRotaryRow);

	GlycolRotaryRow.Init(AID_GLYCOL_ROTARY, MainPanel);
	GlycolRotary.Init(0, 0, 72, 72, srf[SRF_GLYCOL_ROTARY], srf[SRF_BORDER_72x72], GlycolRotaryRow);
	
	AccumRotaryRow.Init(AID_ACCUM_ROTARY, MainPanel);
	AccumRotary.Init(0, 0, 58, 58, srf[SRF_ACCUM_ROTARY], srf[SRF_BORDER_58x58], AccumRotaryRow);

	PLVCSwitchRow.Init(AID_PLVC_SWITCH, MainPanel);
	PLVCSwitch.Init(0, 0, 29, 30, srf[SRF_SWITCH90], srf[SRF_BORDER_29x30], PLVCSwitchRow); 

	///////////////
	// Panel 252 //
	///////////////
	
	WaterControlPanelRow.Init(AID_PANEL_352, MainPanel);
	PressureReliefRotary.Init    (126,   0, 57, 57, srf[SRF_PRESS_RELIEF_VALVE], srf[SRF_BORDER_57x57], WaterControlPanelRow);
	WasteTankInletRotary.Init    (  0, 166, 57, 57, srf[SRF_PRESS_RELIEF_VALVE], srf[SRF_BORDER_57x57], WaterControlPanelRow);
	PotableTankInletRotary.Init  (119, 166, 57, 57, srf[SRF_PRESS_RELIEF_VALVE], srf[SRF_BORDER_57x57], WaterControlPanelRow);
	WasteTankServicingRotary.Init(239, 165, 57, 57, srf[SRF_PRESS_RELIEF_VALVE], srf[SRF_BORDER_57x57], WaterControlPanelRow);

	///////////////
	// Panel 351 //
	///////////////
	
	CabinRepressValveRotaryRow.Init(AID_CABIN_REPRESS_VALVE, MainPanel);
	CabinRepressValveRotary.Init(0, 0, 48, 48, srf[SRF_CABIN_REPRESS_VALVE], srf[SRF_BORDER_48x48], CabinRepressValveRotaryRow);

	WaterGlycolTanksRotariesRow.Init(AID_WATER_GLYCOL_TANKS_ROTARIES, MainPanel);
	SelectorInletValveRotary.Init (0,   0, 84, 84, srf[SRF_SELECTOR_INLET_ROTARY], srf[SRF_BORDER_84x84], WaterGlycolTanksRotariesRow);
	SelectorOutletValveRotary.Init(0, 133, 84, 84, srf[SRF_SELECTOR_OUTLET_ROTARY], srf[SRF_BORDER_84x84], WaterGlycolTanksRotariesRow);
	
	EmergencyCabinPressureRotaryRow.Init(AID_EMERGENCY_CABIN_PRESSURE_ROTARY, MainPanel);
	EmergencyCabinPressureRotary.Init(0, 0, 65, 65, srf[SRF_EMERGENCY_PRESS_ROTARY], srf[SRF_BORDER_65x65], EmergencyCabinPressureRotaryRow);

	O2MainRegulatorSwitchesRow.Init(AID_MAIN_REGULATOR_SWITCHES, MainPanel);
	O2MainRegulatorASwitch.Init(0,  0, 110, 29, srf[SRF_CSM_PANEL_351_SWITCH], srf[SRF_BORDER_110x29], O2MainRegulatorSwitchesRow);
	O2MainRegulatorBSwitch.Init(0, 79, 110, 29, srf[SRF_CSM_PANEL_351_SWITCH], srf[SRF_BORDER_110x29], O2MainRegulatorSwitchesRow);
	O2MainRegulatorBSwitch.SetOffset(220, 0);

	EmergencyCabinPressureTestSwitchRow.Init(AID_CSM_CABINPRESSTESTSWITCH, MainPanel);
	EmergencyCabinPressureTestSwitch.Init(0, 0, 40, 40, srf[SRF_CSM_CABINPRESSTESTSWITCH], srf[SRF_BORDER_38x38], EmergencyCabinPressureTestSwitchRow); 

	///////////////////////////
	// Panel 382             //
	///////////////////////////

	Panel382Row.Init(AID_CSM_PANEL_382, MainPanel);
	EvapWaterControlPrimaryRotary.Init         (149, 229, 48, 48, srf[SRF_CABIN_REPRESS_VALVE], srf[SRF_BORDER_48x48], Panel382Row);
	EvapWaterControlSecondaryRotary.Init       (149,  94, 48, 48, srf[SRF_CABIN_REPRESS_VALVE], srf[SRF_BORDER_48x48], Panel382Row);
	WaterAccumulator1Rotary.Init               ( 23, 124, 48, 48, srf[SRF_CABIN_REPRESS_VALVE], srf[SRF_BORDER_48x48], Panel382Row);
	WaterAccumulator2Rotary.Init               ( 23, 220, 48, 48, srf[SRF_CABIN_REPRESS_VALVE], srf[SRF_BORDER_48x48], Panel382Row);
	PrimaryGlycolEvapInletTempRotary.Init      (349, 237, 31, 31, srf[SRF_GLYCOL_TO_RADIATORS_KNOB], srf[SRF_BORDER_31x31], Panel382Row);
	SuitFlowReliefRotary.Init                  (461, 228, 31, 31, srf[SRF_GLYCOL_TO_RADIATORS_KNOB], srf[SRF_BORDER_31x31], Panel382Row);
	SuitHeatExchangerPrimaryGlycolRotary.Init  (579, 242, 31, 31, srf[SRF_GLYCOL_TO_RADIATORS_KNOB], srf[SRF_BORDER_31x31], Panel382Row);
	SuitHeatExchangerSecondaryGlycolRotary.Init(260, 136, 31, 31, srf[SRF_GLYCOL_TO_RADIATORS_KNOB], srf[SRF_BORDER_31x31], Panel382Row);
	Panel382Cover.Init(Panel382Row, this); 	// dummy switch/display for checklist controller

	//
	// SATPANEL_HATCH_WINDOW
	//

	HatchGearBoxSelectorRow.Init(AID_CSM_GEAR_BOX_ROTARY, MainPanel);
	HatchGearBoxSelector.Init(0, 0, 240, 240, srf[SRF_CSM_GEAR_BOX_ROTARY], srf[SRF_BORDER_240x240], HatchGearBoxSelectorRow);

	HatchActuatorHandleSelectorRow.Init(AID_CSM_PUMP_HANDLE_ROTARY, MainPanel);
	HatchActuatorHandleSelector.Init(0, 0, 240, 240, srf[SRF_CSM_PUMP_HANDLE_ROTARY], srf[SRF_BORDER_240x240], HatchActuatorHandleSelectorRow);

	HatchVentValveRotaryRow.Init(AID_CSM_VENT_VALVE_HANDLE, MainPanel);
	HatchVentValveRotary.Init(0, 0, 264, 264, srf[SRF_CSM_VENT_VALVE_HANDLE], NULL, HatchVentValveRotaryRow);

	HatchActuatorHandleSelectorOpenRow.Init(AID_CSM_PUMP_HANDLE_ROTARY_OPEN, MainPanel);
	HatchActuatorHandleSelectorOpen.Init(0, 0, 150, 200, srf[SRF_CSM_PUMP_HANDLE_ROTARY_OPEN], srf[SRF_BORDER_150x200], HatchActuatorHandleSelectorOpenRow);

	HatchToggleRow.Init(AID_CSM_HATCH_TOGGLE, MainPanel);
	HatchToggle.Init( 0, 0, 200, 300, NULL, srf[SRF_BORDER_200x300], HatchToggleRow); 
	HatchToggle.SetCallback(new PanelSwitchCallback<SaturnSideHatch>(&SideHatch, &SaturnSideHatch::SwitchToggled));

	HatchPanel600LeftRow.Init(AID_CSM_HATCH_600_LEFT, MainPanel, &GaugePower);
	HatchEmergencyO2ValveSwitch.Init(26, 34, 62, 129, srf[SRF_CSM_PANEL_600_SWITCH], srf[SRF_BORDER_62x129], HatchPanel600LeftRow, 186, 0);
	HatchOxygenRepressPressMeter.Init(g_Param.pen[0], g_Param.pen[0], HatchPanel600LeftRow, this);
	HatchOxygenRepressPressMeter.FrameSurface = srf[SRF_CSM_PANEL_600];

	HatchPanel600RightRow.Init(AID_CSM_HATCH_600_RIGHT, MainPanel);
	HatchRepressO2ValveSwitch.Init(61, 34, 62, 129, srf[SRF_CSM_PANEL_600_SWITCH], srf[SRF_BORDER_62x129], HatchPanel600RightRow, 186, 0);

	Panel600.Init(HatchPanel600LeftRow, this); 	// dummy switch/display for checklist controller

	//
	// DSKYs
	//

	DskySwitchRow.Init(AID_DSKY_KEY, MainPanel);
	DskySwitchVerb.Init(0, 20, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 0, 20);
	DskySwitchNoun.Init(0, 60, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 0, 60);
	DskySwitchPlus.Init(41, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 41, 0);
	DskySwitchMinus.Init(41, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 41, 40);
	DskySwitchZero.Init(41, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 41, 80);
	DskySwitchSeven.Init(82, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 82, 0);
	DskySwitchFour.Init(82, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 82, 40);
	DskySwitchOne.Init(82, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 82, 80);
	DskySwitchEight.Init(123, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 123, 0);
	DskySwitchFive.Init(123, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 123, 40);
	DskySwitchTwo.Init(123, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 123, 80);
	DskySwitchNine.Init(164, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 164, 0);
	DskySwitchSix.Init(164, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 164, 40);
	DskySwitchThree.Init(164, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 164, 80);
	DskySwitchClear.Init(205, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 205, 0);
	DskySwitchProg.Init(205, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 205, 40);
	DskySwitchKeyRel.Init(205, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 205, 80);
	DskySwitchEnter.Init(246, 20, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 246, 20);
	DskySwitchReset.Init(246, 60, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 246, 60);

	int dx = 0, dy = 0;
	if (panel == SATPANEL_SEXTANT || panel == SATPANEL_TELESCOPE) {
		dx = 9;
		dy = 219;
		Dsky2SwitchRow.Init(AID_OPTICS_DSKY, MainPanel);
	} else {
		Dsky2SwitchRow.Init(AID_DSKY2_KEY, MainPanel);
	}
	Dsky2SwitchVerb.Init(0 + dx, 20 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 0, 20);
	Dsky2SwitchNoun.Init(0 + dx, 60 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 0, 60);
	Dsky2SwitchPlus.Init(41 + dx, 0 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 41, 0);
	Dsky2SwitchMinus.Init(41 + dx, 40 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 41, 40);
	Dsky2SwitchZero.Init(41 + dx, 80 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 41, 80);
	Dsky2SwitchSeven.Init(82 + dx, 0 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 82, 0);
	Dsky2SwitchFour.Init(82 + dx, 40 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 82, 40);
	Dsky2SwitchOne.Init(82 + dx, 80 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 82, 80);
	Dsky2SwitchEight.Init(123 + dx, 0 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 123, 0);
	Dsky2SwitchFive.Init(123 + dx, 40 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 123, 40);
	Dsky2SwitchTwo.Init(123 + dx, 80 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 123, 80);
	Dsky2SwitchNine.Init(164 + dx, 0 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 164, 0);
	Dsky2SwitchSix.Init(164 + dx, 40 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 164, 40);
	Dsky2SwitchThree.Init(164 + dx, 80 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 164, 80);
	Dsky2SwitchClear.Init(205 + dx, 0 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 205, 0);
	Dsky2SwitchProg.Init(205 + dx, 40 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 205, 40);
	Dsky2SwitchKeyRel.Init(205 + dx, 80 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 205, 80);
	Dsky2SwitchEnter.Init(246 + dx, 20 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 246, 20);
	Dsky2SwitchReset.Init(246 + dx, 60 + dy, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], Dsky2SwitchRow, 246, 60);

	ASCPRollSwitch.Init(GDCAlignButtonRow, this, 0); 	// dummy switch/display for checklist controller, GDCAlignButtonRow is arbitrary
	ASCPPitchSwitch.Init(GDCAlignButtonRow, this, 1);
	ASCPYawSwitch.Init(GDCAlignButtonRow, this, 2);
}

void SetupgParam(HINSTANCE hModule) {

	g_Param.hDLL = hModule;

	//
	// allocate GDI resources
	//

	g_Param.font[0]  = CreateFont (-13, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[2]  = CreateFont (-8, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
	g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
	g_Param.brush[2] = CreateSolidBrush (RGB(154,154,154));  // Grey
	g_Param.brush[3] = CreateSolidBrush (RGB(3,3,3));  // Black
	g_Param.pen[0] = CreatePen (PS_SOLID, 3, RGB(224, 224, 224));
	g_Param.pen[1] = CreatePen (PS_SOLID, 4, RGB(  0,   0,   0));
	g_Param.pen[2] = CreatePen (PS_SOLID, 1, RGB(  0,   0,   0));
	g_Param.pen[3] = CreatePen (PS_SOLID, 3, RGB( 77,  77,  77));
	g_Param.pen[4] = CreatePen (PS_SOLID, 3, RGB(  0,   0,   0));
	g_Param.pen[5] = CreatePen (PS_SOLID, 1, RGB(255,   0,   0));
	g_Param.pen[6] = CreatePen (PS_SOLID, 3, RGB(255, 255, 255));
}

void DeletegParam() {

	int i;

	//
	// deallocate GDI resources
	//

	for (i = 0; i < 3; i++) DeleteObject (g_Param.font[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.brush[i]);
	for (i = 0; i < 6; i++) DeleteObject (g_Param.pen[i]);
}

bool Saturn::clbkPanelMouseEvent (int id, int event, int mx, int my)

{
	static int ctrl = 0;

	//
	// Foreward the mouse clicks on the optics cover to the DSKYs 
	// because of overlapping
	//

	if (id == AID_CSM_SEXTANTCOVER || id == AID_CSM_TELESCOPECOVER) {
		if (GetRenderViewportIsWideScreen() == 0) {
			int tx = mx + 244 - 721;
			int ty = my + 115 - 0;

			if (tx >= 0 && ty <= 349) {
				return clbkPanelMouseEvent(AID_OPTICS_DSKY, event, tx, ty);
			}		
		}
	}


	//
	// Special handling optics DSKY
	//

	if (id == AID_OPTICS_DSKY) {
		if (mx >= 285 && my <= 100 && event == PANEL_MOUSE_LBDOWN) { 
			if (opticsDskyEnabled == 0)
				opticsDskyEnabled = 1;
			else
				opticsDskyEnabled = -1;
			SwitchClick();
			return true;
		}
		if (opticsDskyEnabled == 0)
			return false;
	}

	//
	// Special handling ORDEAL
	//

	if (id == AID_ORDEALSWITCHES && PanelId == SATPANEL_MAIN) {
		if (event & PANEL_MOUSE_LBDOWN) { 
			if (ordealEnabled == 0) {
				ordealEnabled = 1;
				SwitchClick();
				return true;
			} else if (mx <= 50) {
				ordealEnabled = -1;
				SwitchClick();
				return true;
			}
		}
		if (ordealEnabled == 0)
			return false;
	}

	//
	// Panel
	//

	if (MainPanel.CheckMouseClick(id, event, mx, my))
		return true;

	switch (id) {
	// ASCP
	case AID_ASCPDISPLAYROLL:
		ascp.RollDisplayClicked();
		return true;

	case AID_ASCPDISPLAYPITCH:
		ascp.PitchDisplayClicked();
		return true;

	case AID_ASCPDISPLAYYAW:
		ascp.YawDisplayClicked();
		return true;

	case AID_ASCPROLL:
		ascp.RollClick(event, mx, my);
		return true;

	case AID_ASCPPITCH:
		ascp.PitchClick(event, mx, my);
		return true;

	case AID_ASCPYAW:
		ascp.YawClick(event, mx, my);			
		return true;

	// EMS
	case AID_EMSDVSETSWITCH:
		return EMSDvSetSwitch.CheckMouseClick(event, mx, my);			

	// Forward Hatch
	case AID_FORWARD_HATCH:
		ForwardHatch.Toggle();
		return true;

	// CWS
	case AID_MASTER_ALARM:
	case AID_MASTER_ALARM2:
	case AID_MASTER_ALARM3:
		return cws.CheckMasterAlarmMouseClick(event);

	case AID_MFDMAINLEFT:
		MousePanel_MFDButton(MFD_LEFT, event, mx, my);
		return true;

	case AID_MFDMAINRIGHT:
		MousePanel_MFDButton(MFD_RIGHT, event, mx, my);
		return true;

	case AID_MFDGNLEFT:
		MousePanel_MFDButton(MFD_LEFT, event, mx, my);
		return true;

	case AID_MFDGNUSER1:
		MousePanel_MFDButton(MFD_USER1, event, mx, my);
		return true;

	case AID_MFDGNUSER2:
		MousePanel_MFDButton(MFD_USER2, event, mx, my);
		return true;

	case AID_MFDGNRIGHT:
		MousePanel_MFDButton(MFD_RIGHT, event, mx, my);
		return true;

	case AID_MFDDOCK:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
			if (my > 234 && my < 249) {
				if (event & PANEL_MOUSE_LBDOWN) {
					if (mx > 47 && mx < 68) {
						ButtonClick();
						oapiToggleMFD_on(MFD_RIGHT);	// MFD_USER1
					} else if (mx > 208 && mx < 229) {
						ButtonClick();
						oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_F1);		// MFD_USER1
					} else if (mx > 239 && mx < 252) {
						ButtonClick();
						oapiSendMFDKey(MFD_RIGHT, OAPI_KEY_GRAVE);		// MFD_USER1
					}
				}
			} else if (mx > 5 && mx < 26 && my > 38 && my < 208) {
				if ((my - 38) % 31 < 14) {
					int bt = (my - 38) / 31 + 0;
					if (event & PANEL_MOUSE_LBDOWN)
						ButtonClick();
					oapiProcessMFDButton(MFD_RIGHT, bt, event);	// MFD_USER1
				}
			} else if (mx > 273 && mx < 294 && my > 38 && my < 208) {
				if ((my - 38) % 31 < 14) {
					int bt = (my - 38) / 31 + 6;
					if (event & PANEL_MOUSE_LBDOWN)
						ButtonClick();
					oapiProcessMFDButton(MFD_RIGHT, bt, event);	// MFD_USER1
				}
			}
		} else if (event & PANEL_MOUSE_LBDOWN) {
			ButtonClick();
			oapiToggleMFD_on(MFD_RIGHT); // MFD_USER1
		}
		return true;

	case AID_MFDDOCK_POWER:
		if (oapiGetMFDMode(MFD_RIGHT) == MFD_NONE) {	// MFD_USER1
			ButtonClick();
			oapiToggleMFD_on(MFD_RIGHT);	// MFD_USER1
		}
		return true;

	case AID_COAS:
		if (coasEnabled)
			coasEnabled = false;
		else
			coasEnabled = true;
		SwitchClick();
		return true;

	case AID_CSM_HATCH_600_LEFT:
		if (mx <= 200 && my >= 480 && event == PANEL_MOUSE_LBDOWN) { 
			if (hatchPanel600EnabledLeft == 0) {
				hatchPanel600EnabledLeft = 1;
				hatchPanel600EnabledRight = 1;
			} else {
				hatchPanel600EnabledLeft = -1;
				hatchPanel600EnabledRight = -1;
			}
			SwitchClick();
			return true;
		}
		return false;

	case AID_CSM_HATCH_600_RIGHT:
		if (mx >= 50 && my >= 200 && event == PANEL_MOUSE_LBDOWN) { 
			if (hatchPanel600EnabledRight == 0) {
				hatchPanel600EnabledLeft = 1;
				hatchPanel600EnabledRight = 1;
			} else {
				hatchPanel600EnabledLeft = -1;
				hatchPanel600EnabledRight = -1;
			}
			SwitchClick();
			return true;
		}
		return false;

	case AID_CSM_PANEL_382:
		if (event == PANEL_MOUSE_LBDOWN) { 
			if (panel382Enabled == 0) {
				panel382Enabled = 1;
				SwitchClick();
			} else if (my <= 80) { 
				panel382Enabled = 0; 
				SwitchClick();
			}
			return true;
		}
		return false;

	case AID_SWITCHTO_TELESCOPE1:
	case AID_SWITCHTO_TELESCOPE2:
		if (event == PANEL_MOUSE_LBDOWN) { 
			PanelId = SATPANEL_TELESCOPE;
			CheckPanelIdInTimestep = true;
		}
		return true;

	case AID_SWITCHTO_SEXTANT1:
	case AID_SWITCHTO_SEXTANT2:
		if (event == PANEL_MOUSE_LBDOWN) { 
			PanelId = SATPANEL_SEXTANT;
			CheckPanelIdInTimestep = true;
		}
		return true;
	}
	return false;
}

void Saturn::PanelSwitchToggled(ToggleSwitch *s) {

	if (s == &O2Heater1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1HEATER:PUMP"));

	} else if (s == &O2Heater2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2HEATER:PUMP"));

	} else if (s == &H2Heater1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1HEATER:PUMP"));

	} else if (s == &H2Heater2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2HEATER:PUMP"));

	} else if (s == &O2Fan1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1FAN:PUMP"));

	} else if (s == &O2Fan2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2FAN:PUMP"));

	} else if (s == &H2Fan1Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1FAN:PUMP"));

	} else if (s == &H2Fan2Switch) {
		CryoTankHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2FAN:PUMP"));


	} else if (s == &FuelCellRadiators1Switch) {
		if (FuelCell1RadCB.IsPowered()) {
			if (FuelCellRadiators1Switch.IsUp())
				FuelCellCoolingBypass(1, false);
			else if (FuelCellRadiators1Switch.IsDown())
				FuelCellCoolingBypass(1, true);
		}

	} else if (s == &FuelCellRadiators2Switch) {
		if (FuelCell2RadCB.IsPowered()) {
			if (FuelCellRadiators2Switch.IsUp())
				FuelCellCoolingBypass(2, false);
			else if (FuelCellRadiators2Switch.IsDown())
				FuelCellCoolingBypass(2, true);
		}

	} else if (s == &FuelCellRadiators3Switch) {
		if (FuelCell3RadCB.IsPowered()) {
			if (FuelCellRadiators3Switch.IsUp())
				FuelCellCoolingBypass(3, false);
			else if (FuelCellRadiators3Switch.IsDown())
				FuelCellCoolingBypass(3, true);
		}


	} else if (s == &FuelCellHeater1Switch) {
		FuelCellHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1HEATER:PUMP"));

	} else if (s == &FuelCellHeater2Switch) {
		FuelCellHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2HEATER:PUMP"));

	} else if (s == &FuelCellHeater3Switch) {
		FuelCellHeaterSwitchToggled(s,
			(int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3HEATER:PUMP"));

		//Fuel cell reaactant valve switches now control the reactant valves themselves and not fuel cell operation directly
	} else if (s == &FuelCellReactants1Switch) {
		FuelCellReactantsSwitchToggled(s, &FuelCell1ReacsCB, &FuelCell1BusContCB,
			(int*) Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1MANIFOLD:IN:OPEN"), (int*) Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD:IN:OPEN"));

	} else if (s == &FuelCellReactants2Switch) {
		FuelCellReactantsSwitchToggled(s, &FuelCell2ReacsCB, &FuelCell2BusContCB,
			(int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2MANIFOLD:IN:OPEN"), (int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2MANIFOLD:IN:OPEN"));

	} else if (s == &FuelCellReactants3Switch) {
		FuelCellReactantsSwitchToggled(s, &FuelCell3ReacsCB, &FuelCell3BusContCB,
			(int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3MANIFOLD:IN:OPEN"), (int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD:IN:OPEN"));


	} else if (s == &MainBusTieBatAcSwitch) {
		MainBusAController.SetTieState(s->GetState());
		PanelRotationalSwitchChanged(&BatteryChargeRotary);

	} else if (s == &MainBusTieBatBcSwitch) {
		MainBusBController.SetTieState(s->GetState());
		PanelRotationalSwitchChanged(&BatteryChargeRotary);

	} else if (s == &GlycolToRadiatorsLever) {
		if (GlycolToRadiatorsLever.IsDown()) {
			// Prim. radiators are bypassed
			PrimEcsRadiatorExchanger1->SetBypassed(true);
			PrimEcsRadiatorExchanger2->SetBypassed(true);
		} else {
			PrimEcsRadiatorExchanger1->SetBypassed(false);
			PrimEcsRadiatorExchanger2->SetBypassed(false);
		}

	} else if (s == &EcsRadiatorsFlowContPwrSwitch || s == &EcsRadiatorsManSelSwitch) {
		if (EcsRadiatorsFlowContPwrSwitch.IsUp()) {
			PrimEcsRadiatorExchanger1->SetPumpOn(); 
			PrimEcsRadiatorExchanger2->SetPumpOn(); 

		} else if (EcsRadiatorsFlowContPwrSwitch.IsDown()) {
			if (EcsRadiatorsManSelSwitch.IsUp()) {
				PrimEcsRadiatorExchanger1->SetPumpOn(); 
				PrimEcsRadiatorExchanger2->SetPumpOff(); 

			} else if (EcsRadiatorsManSelSwitch.IsCenter()) {
				PrimEcsRadiatorExchanger1->SetPumpOff(); 
				PrimEcsRadiatorExchanger2->SetPumpOff(); 

			} else if (EcsRadiatorsManSelSwitch.IsDown()) {
				PrimEcsRadiatorExchanger1->SetPumpOff(); 
				PrimEcsRadiatorExchanger2->SetPumpOn(); 
			}
		}

	} else if (s == &EcsRadiatorsHeaterPrimSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("ELECTRIC:PRIMECSRADIATORSHEATER:PUMP");
		if (EcsRadiatorsHeaterPrimSwitch.IsCenter())
			*pump = SP_PUMP_OFF;
		else
			*pump = SP_PUMP_AUTO;

	} else if (s == &EcsRadiatorsHeaterSecSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("ELECTRIC:SECECSRADIATORSHEATER:PUMP");
		if (EcsRadiatorsHeaterSecSwitch.IsDown()) {
			*pump = SP_PUMP_OFF;
		} else {
			*pump = SP_PUMP_AUTO;
		}

	} else if (s == &GlycolEvapSteamPressIncrDecrSwitch) {
		int *pump = (int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:VALVE");
		if (GlycolEvapSteamPressIncrDecrSwitch.IsUp())
			*pump = SP_VALVE_CLOSE;
		else if (GlycolEvapSteamPressIncrDecrSwitch.IsDown())
			*pump = SP_VALVE_OPEN;
		else
			*pump = SP_VALVE_NONE;

	} else if (s == &SPSTestSwitch) {
		SPSPropellant.SPSTestSwitchToggled();
	
	} else if (s == &SPSLineHTRSSwitch) {  
		int *pump1 = (int*) Panelsdk.GetPointerByString("ELECTRIC:SPSPROPELLANTLINEHEATERA:PUMP");
		int *pump2 = (int*) Panelsdk.GetPointerByString("ELECTRIC:SPSPROPELLANTLINEHEATERB:PUMP");

		if (s->IsUp()) {
			*pump1 = SP_PUMP_ON;
			*pump2 = SP_PUMP_ON;

		} else if (s->IsDown()) {
			*pump1 = SP_PUMP_ON;
			*pump2 = SP_PUMP_OFF;

		} else {
			*pump1 = SP_PUMP_OFF;
			*pump2 = SP_PUMP_OFF;
		}
	} else if (s == &EMSModeSwitch) {
		ems.SwitchChanged();

	} else if (s == &OpticsMarkButton) {
		if (s->GetState() == 1) {
			agc.SetInputChannelBit(016, Mark, 1);
		} else {
			agc.SetInputChannelBit(016, Mark, 0);
		}

	} else if (s == &OpticsMarkRejectButton) {
		if (s->GetState() == 1) {
			agc.SetInputChannelBit(016, MarkReject, 1);
		} else {
			agc.SetInputChannelBit(016, MarkReject, 0);
		}
	} else if (s == &OpticsModeSwitch || s == &OpticsZeroSwitch) {
		optics.OpticsSwitchToggled();
	}
}

void Saturn::PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) {

	if (s == &FuelCellPhIndicator) {
		if (stage <= CSM_LEM_STAGE && FuelCell1PumpsACCB.IsPowered())
			FuelCellPhIndicator.SetState(1);	// Not simulated at the moment. Gets power from FC1 pump CB.
		else
			FuelCellPhIndicator.SetState(0);	

	} else if (s == &FuelCellRadTempIndicator) {
		FuelCellStatus fc;
		GetFuelCellStatus(FuelCellIndicatorsSwitch.GetState() + 1, fc);
		if (fc.CoolingTempF < -30.0 || stage > CSM_LEM_STAGE)	// indication if temperature below -30°F
			FuelCellRadTempIndicator.SetState(0);
		else
			FuelCellRadTempIndicator.SetState(1);

	} else if (s == &FuelCellRadiators1Indicator) {
		FuelCellRadiators1Indicator.SetState(!FuelCellCoolingBypassed(1) && stage <= CSM_LEM_STAGE ? 1 : 0);

	} else if (s == &FuelCellRadiators2Indicator) {
		FuelCellRadiators2Indicator.SetState(!FuelCellCoolingBypassed(2) && stage <= CSM_LEM_STAGE ? 1 : 0);

	} else if (s == &FuelCellRadiators3Indicator) {
		FuelCellRadiators3Indicator.SetState(!FuelCellCoolingBypassed(3) && stage <= CSM_LEM_STAGE ? 1 : 0);

		
	}
	//Reaction valves for Apollo 13 and before were wired in series with the indicators so both valves had to close before the talkback would barberpole
	else if (s == &FuelCellReactants1Indicator) {
		if ((*(int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1MANIFOLD:IN:ISOPEN") == 0) &&
			(*(int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD:IN:ISOPEN") == 0) &&
			FuelCell1BusContCB.IsPowered()) FuelCellReactants1Indicator.SetState(0);
		else FuelCellReactants1Indicator.SetState(1);

	}
	else if (s == &FuelCellReactants2Indicator) {
		if ((*(int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2MANIFOLD:IN:ISOPEN") == 0) &&
			(*(int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2MANIFOLD:IN:ISOPEN") == 0) &&
			FuelCell2BusContCB.IsPowered()) FuelCellReactants2Indicator.SetState(0);
		else FuelCellReactants2Indicator.SetState(1);

	}
	else if (s == &FuelCellReactants3Indicator) {
		if ((*(int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3MANIFOLD:IN:ISOPEN") == 0) &&
			(*(int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD:IN:ISOPEN") == 0) &&
			FuelCell3BusContCB.IsPowered()) FuelCellReactants3Indicator.SetState(0);
		else FuelCellReactants3Indicator.SetState(1);

	} else if (s == &EcsRadiatorIndicator) {
		if (EcsRadiatorsFlowContPwrSwitch.IsUp()) {
			if (EcsRadiatorsFlowContAutoSwitch.IsDown())
				EcsRadiatorIndicator.SetState(0);
			else
				EcsRadiatorIndicator.SetState(1);
		}
	} else if (s == &DockingProbeAIndicator) {
		DockingProbeAIndicator.SetState((dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_RETRACTED || dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_EXTENDED) ? 1 : 0);

	} else if (s == &DockingProbeBIndicator) {
		DockingProbeBIndicator.SetState((dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_RETRACTED || dockingprobe.GetStatus() == DOCKINGPROBE_STATUS_EXTENDED) ? 1 : 0);
	
	} else if (s == &SPSOxidFlowValveMaxIndicator) {
		SPSOxidFlowValveMaxIndicator.SetState(!SPSPropellant.IsOxidFlowValveMax() ? 1 : 0);
		
	} else if (s == &SPSOxidFlowValveMinIndicator) {
		SPSOxidFlowValveMinIndicator.SetState(!SPSPropellant.IsOxidFlowValveMin() ? 1 : 0);

	} else if (s == &SPSHeliumValveAIndicator) {
		SPSHeliumValveAIndicator.SetState(SPSPropellant.IsHeliumValveAOpen() ? 1 : 0);

	} else if (s == &SPSHeliumValveBIndicator) {
		SPSHeliumValveBIndicator.SetState(SPSPropellant.IsHeliumValveBOpen() ? 1 : 0);
	
	}  else if (s == &SPSInjectorValve1Indicator) {
		SPSInjectorValve1Indicator.SetState(SPSEngine.GetInjectorValves12Open() ? 1 : 0);

	}  else if (s == &SPSInjectorValve2Indicator) {
		SPSInjectorValve2Indicator.SetState(SPSEngine.GetInjectorValves12Open() ? 1 : 0);

	}  else if (s == &SPSInjectorValve3Indicator) {
		SPSInjectorValve3Indicator.SetState(SPSEngine.GetInjectorValves34Open() ? 1 : 0);

	}  else if (s == &SPSInjectorValve4Indicator) {
		SPSInjectorValve4Indicator.SetState(SPSEngine.GetInjectorValves34Open() ? 1 : 0);
	}
}

void Saturn::PanelRotationalSwitchChanged(RotationalSwitch *s) {

	if (s == &BatteryChargeRotary) {
		if (s->GetState() == 1 && MainBusTieBatAcSwitch.IsDown()) 
			BatteryCharger.Charge(1);
		else if (s->GetState() == 2 && MainBusTieBatBcSwitch.IsDown())  
			BatteryCharger.Charge(2);
		else if (s->GetState() == 3 && MainBusTieBatAcSwitch.IsDown() && MainBusTieBatBcSwitch.IsDown()) 
			BatteryCharger.Charge(3);
		else
			BatteryCharger.Charge(0);
	
	} else if (s == &EMSFunctionSwitch) {
		ems.SwitchChanged();

	} else if (s == &GlycolToRadiatorsRotary) {
		if (GlycolToRadiatorsRotary.GetState() == 0) {
			// Sec. radiators are bypassed, as there's no flow control we're setting pump directly
			SecEcsRadiatorExchanger1->SetPumpOff();
			SecEcsRadiatorExchanger2->SetPumpOff();
		} else {
			SecEcsRadiatorExchanger1->SetPumpOn();
			SecEcsRadiatorExchanger2->SetPumpOn();
		}
	}
}

void Saturn::CryoTankHeaterSwitchToggled(ToggleSwitch *s, int *pump) {

	if (s->IsUp())
		*pump = SP_PUMP_AUTO;
	else if (s->IsCenter())
		*pump = SP_PUMP_OFF;
	else if (s->IsDown())
		*pump = SP_PUMP_ON;
}

void Saturn::FuelCellHeaterSwitchToggled(ToggleSwitch *s, int *pump) {

	if (s->IsUp())
		*pump = SP_PUMP_AUTO;
	else
		*pump = SP_PUMP_OFF;
}

void Saturn::FuelCellReactantsSwitchToggled(ToggleSwitch *s, CircuitBrakerSwitch *cb, CircuitBrakerSwitch *cbLatch, int *h2open, int *o2open) {

	// Is the reactants valve latched?
	if (s->IsDown() && FCReacsValvesSwitch.IsDown() && cbLatch->IsPowered()) return;
	// Switch powered?
	if (!cb->IsPowered()) return;

	if (s->IsUp()) {
	*h2open = SP_VALVE_OPEN, *o2open = SP_VALVE_OPEN;
}
	else if (s->IsCenter()) {
		*h2open = SP_VALVE_NONE, *o2open = SP_VALVE_NONE;
	}
	else if (s->IsDown()) {
		*h2open = SP_VALVE_CLOSE, 
		*o2open = SP_VALVE_CLOSE;
	}
}

void Saturn::PanelRefreshForwardHatch() {

	if (InPanel && PanelId == SATPANEL_TUNNEL) {
		if (oapiCameraInternal()) {
			oapiSetPanel(SATPANEL_TUNNEL);
		} else {
			RefreshPanelIdInTimestep = true;
		}
	}
}

void Saturn::PanelRefreshSideHatch() {

	if (InPanel && PanelId == SATPANEL_HATCH_WINDOW) {
		if (oapiCameraInternal()) {
			oapiSetPanel(SATPANEL_HATCH_WINDOW);
		}
		else {
			RefreshPanelIdInTimestep = true;
		}
	}
}

void Saturn::MousePanel_MFDButton(int mfd, int event, int mx, int my) {

	if (oapiGetMFDMode(mfd) != MFD_NONE) {
		if (my > 330 && my < 352) {
			if (event & PANEL_MOUSE_LBDOWN) {
				if (mx > 67 && mx < 96) {
					ButtonClick();
					oapiToggleMFD_on(mfd);
				}
				else if (mx > 295 && mx < 324) {
					ButtonClick();
					oapiSendMFDKey(mfd, OAPI_KEY_F1);
				}
				else if (mx > 329 && mx < 356) {
					ButtonClick();
					oapiSendMFDKey(mfd, OAPI_KEY_GRAVE);
				}
			}
		}
		else if (mx > 10 && mx < 38 && my > 53 && my < 294) {
			if ((my - 53) % 44 < 21) {
				int bt = (my - 53) / 44 + 0;
				if (event & PANEL_MOUSE_LBDOWN)
					ButtonClick();
				oapiProcessMFDButton(mfd, bt, event);
			}
		}
		else if (mx > 386 && mx < 416 && my > 53 && my < 294) {
			if ((my - 53) % 44 < 21) {
				int bt = (my - 53) / 44 + 6;
				if (event & PANEL_MOUSE_LBDOWN)
					ButtonClick();
				oapiProcessMFDButton(mfd, bt, event);
			}
		}
	}
	else if (event & PANEL_MOUSE_LBDOWN) {
		ButtonClick();
		oapiToggleMFD_on(mfd);
	}
}

void Saturn::SwitchClick()

{
	Sclick.play(NOLOOP,255);
}

void Saturn::ButtonClick()

{
	Bclick.play(NOLOOP,255);
}

void Saturn::GuardClick()

{
	Gclick.play(NOLOOP,255);
}

void Saturn::ProbeSound()

{
	Psound.play(NOLOOP,255);
}

void Saturn::CabinFanSound()

{
	double volume = 0.0;

	//
	// We base the volume on the number of fans and the power supply to them.
	//

	if (CabinFan1Switch.IsUp()) {
		volume += ECSCabinFanAC1ACircuitBraker.Voltage();
		volume += ECSCabinFanAC1BCircuitBraker.Voltage();
		volume += ECSCabinFanAC1CCircuitBraker.Voltage();
	}

	if (CabinFan2Switch.IsUp()) {
		volume += ECSCabinFanAC2ACircuitBraker.Voltage();
		volume += ECSCabinFanAC2BCircuitBraker.Voltage();
		volume += ECSCabinFanAC2CCircuitBraker.Voltage();
	}

	//
	// Scale volume appropriately based on the expected max voltage (115V per phase)
	//

	CabinFans.play((int) ((64.0 * volume / 400.0) + 127.0));
}

void Saturn::StopCabinFanSound()

{
	CabinFans.stop();
}

//
// Signal a master alarm until reset.
//

void Saturn::MasterAlarm()

{
	//
	// Enable master alarm.
	//

	cws.SetMasterAlarm(true);
}

//
// And stop it.
//

void Saturn::StopMasterAlarm()

{
	SMasterAlarm.stop();
	cws.SetMasterAlarm(false);
}

void Saturn::RenderS1bEngineLight(bool EngineOn, SURFHANDLE dest, SURFHANDLE src, int xoffs, int yoffs)

{
	if (EngineOn)
	{
		oapiBlt(dest, src, xoffs, yoffs, xoffs, yoffs, 29, 29);
	}
	else
	{
		oapiBlt(dest, src, xoffs, yoffs, xoffs + 101, yoffs, 29, 29);
	}
}

bool Saturn::clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf)

{
	// Enable this to trace the redraws, but then it's running horrible slow!
	// char tracebuffer[100];
	// sprintf(tracebuffer, "Saturn::clbkPanelRedrawEvent id %i", id);
	// TRACESETUP(tracebuffer);

	//
	// Note: if you crash in this function with a NULL surf handle, odds are you screwed up
	// the region definition so maxX < minX or maxY < minY.
	//

	//
	// Special handling for docking panel
	//

	if (id == AID_SM_RCS_MODE) {
		if (PanelId == SATPANEL_LEFT_RNDZ_WINDOW) {
			if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
				oapiBlt(surf, srf[SRF_SM_RCS_MODE], 0, 0, 0, 0, 75, 73, SURF_PREDEF_CK);
				OrbiterAttitudeToggle.SetVisible(true);
			} else {
				OrbiterAttitudeToggle.SetVisible(false);
			}
		} else {
			OrbiterAttitudeToggle.SetVisible(true);
		}
	}

	//
	// Special handling for the side hatch
	//

	if (id == AID_CSM_HATCH_600_LEFT) {
		if (hatchPanel600EnabledLeft == -1) {
			hatchPanel600EnabledLeft = 0;
			return true;
		}
		if (hatchPanel600EnabledLeft == 0) { 
			if (Panel600.IsFlashing()) {
				if (PanelFlashOn) {
					oapiBlt(surf, srf[SRF_BORDER_150x200], 0, 514, 0, 0, 150, 200, SURF_PREDEF_CK);
				}
				return true;
			}
			return false;
		}
		oapiBlt(surf, srf[SRF_CSM_PANEL_600], 0, 0,   130, 0, 149, 302, SURF_PREDEF_CK);
		oapiBlt(surf, srf[SRF_CSM_PANEL_600], 0, 304,   0, 0, 129, 410, SURF_PREDEF_CK);
		if (Panel600.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_150x200], 0, 514, 0, 0, 150, 200, SURF_PREDEF_CK);
			}
		}

	} else 	if (id == AID_CSM_HATCH_600_RIGHT) {
		if (hatchPanel600EnabledRight == -1) {
			hatchPanel600EnabledRight = 0;
			return true;
		}
		if (hatchPanel600EnabledRight == 0) { 
			if (Panel600.IsFlashing()) {
				if (PanelFlashOn) {
					oapiBlt(surf, srf[SRF_BORDER_90x90], 59, 212, 0, 0, 90, 90, SURF_PREDEF_CK); 
				}
				return true;
			}
			return false;
		}
		oapiBlt(surf, srf[SRF_CSM_PANEL_600], 0, 0, 280, 0, 149, 302, SURF_PREDEF_CK);
		if (Panel600.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_90x90], 59, 212, 0, 0, 90, 90, SURF_PREDEF_CK); 
			}
		}
	}

	//
	// Special handling panel 382 cover
	//

	if (id == AID_CSM_PANEL_382) {
		if (panel382Enabled == 0) { 
			oapiBlt(surf, srf[SRF_CSM_PANEL_382_COVER], 0, 0, 0, 0, 673, 369, SURF_PREDEF_CK);
			if (Panel382Cover.IsFlashing()) {
				if (PanelFlashOn) {
					oapiBlt(surf, srf[SRF_BORDER_673x369], 0, 0, 0, 0, 673, 369, SURF_PREDEF_CK);
				}
			}
			return true;
		} else {
			if (Panel382Cover.IsFlashing()) {
				if (PanelFlashOn) {
					oapiBlt(surf, srf[SRF_BORDER_673x80], 0, 0, 0, 0, 673, 80, SURF_PREDEF_CK);
				}
			}
		}
	}

	//
	// Special handling optics DSKY
	//

	if (id == AID_OPTICS_DSKY) {
		if (opticsDskyEnabled == -1) {
			opticsDskyEnabled = 0;
			return true;
		}
		if (opticsDskyEnabled == 0) {
			return false;
		}
		oapiBlt(surf, srf[SRF_OPTICS_DSKY], 0, 0, 0, 0, 303, 349);
		dsky2.RenderLights(surf, srf[SRF_DSKY], 27, 28, false);
		dsky2.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP], 171, 23);
	}

	//
	// Special handling ORDEAL
	//

	if (id == AID_ORDEALSWITCHES && PanelId == SATPANEL_MAIN) {
		if (ordealEnabled == -1) {
			ordealEnabled = 0;
			return true;
		}
		if (ordealEnabled == 0) { 
			if (PanelOrdeal.IsFlashing()) {
				if (PanelFlashOn) {
					oapiBlt(surf, srf[SRF_BORDER_50x158], 0, 22, 0, 0, 50, 158, SURF_PREDEF_CK);
				}
				return true;
			}
			return false;
		}
		oapiBlt(surf, srf[SRF_ORDEAL_PANEL], 0, 0, 0, 0, 477, 202, SURF_PREDEF_CK);
		if (PanelOrdeal.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_50x158], 0, 22, 0, 0, 50, 158, SURF_PREDEF_CK);
			}
		}
	}

	//
	// Special handling panels 181,277,278,cryo (Apollo 15 and later)
	//
	if (id == AID_CSM_PANEL_181)
		oapiBlt(surf, srf[SRF_CSM_PANEL_181], 0, 0, 0, 0, 432, 251, SURF_PREDEF_CK);

	if (id == AID_CSM_PANEL_277)
		oapiBlt(surf, srf[SRF_CSM_PANEL_277], 0, 0, 0, 0, 155, 195, SURF_PREDEF_CK);

	if (id == AID_CSM_PANEL_278J)
	{
		if (pMission->GetPanel278Version() == 2)
		{
			oapiBlt(surf, srf[SRF_CSM_PANEL_278_CSM112], 0, 0, 0, 0, 188, 433, SURF_PREDEF_CK);
		}
		else if (pMission->GetPanel278Version() == 3)
		{
			oapiBlt(surf, srf[SRF_CSM_PANEL_278_CSM114], 0, 0, 0, 0, 188, 433, SURF_PREDEF_CK);
		}
	}

	/*if (pMission->IsJMission())   // Disabled for now
	{
		if (id == AID_CYROTANKINDICATORS)
		{
			oapiBlt(surf, srf[SRF_CRYO_IND_J], 0, 0, 0, 0, 321, 9, SURF_PREDEF_CK);
		}

		if (id == AID_CYROTANKSWITCHES)
		{
			oapiBlt(surf, srf[SRF_CRYO_SWITCHES_J], 0, 0, 0, 0, 590, 86, SURF_PREDEF_CK);
		}
	}*/

	//
	// Process all the generic switches.
	//

	if (MainPanel.DrawRow(id, surf, PanelFlashOn))
		return true;

	//
	// Now special case the rest.
	//

	switch (id) {
	
	
	case AID_CSM_MNPNL_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_MNPNL_WDW_LES], 0, 0, 0, 0, 356, 156, SURF_PREDEF_CK);
		}
		return true;

	case AID_CSM_RNDZ_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_RNDZ_WDW_LES], 0, 0, 0, 0, 641, 735, SURF_PREDEF_CK);
		}
		return true;

	case AID_CSM_RIGHT_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_RIGHT_WDW_LES], 0, 0, 0, 0, 508, 484, SURF_PREDEF_CK);
		}
		return true;

	case AID_CSM_LEFT_WDW_LES:
		if (LESAttached) {
			oapiBlt(surf,srf[SRF_CSM_LEFT_WDW_LES], 0, 0, 0, 0, 508, 484, SURF_PREDEF_CK);
		}
		return true;

	case AID_CSM_TELESCOPECOVER:
		// No optics cover on pad in order to be able to use P03
		if (optics.OpticsCovered && stage >= LAUNCH_STAGE_ONE) {
			oapiBlt(surf,srf[SRF_CSM_TELESCOPECOVER], 0, 0, 0, 0, 536, 535);
		}
		return true;

	case AID_CSM_SEXTANTCOVER:
		// No optics cover on pad in order to be able to use P03
		if (optics.OpticsCovered && stage >= LAUNCH_STAGE_ONE) {
			oapiBlt(surf,srf[SRF_CSM_SEXTANTCOVER], 0, 0, 0, 0, 535, 535);
		}
		return true;

	case AID_DSKY_LIGHTS:
		dsky.RenderLights(surf, srf[SRF_DSKY]);
		return true;

	case AID_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP]);
		return true;

	// ASCP
	case AID_ASCPDISPLAYROLL:
		ascp.PaintRollDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_ASCPDISPLAYPITCH:
		ascp.PaintPitchDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_ASCPDISPLAYYAW:
		ascp.PaintYawDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_ASCPROLL:
		ascp.PaintRoll(surf, srf[SRF_THUMBWHEEL_SMALL]);
		if (ASCPRollSwitch.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_17x36], 0, 0, 0, 0, 17, 36, SURF_PREDEF_CK);
			}
		}
		return true;

	case AID_ASCPPITCH:
		ascp.PaintPitch(surf, srf[SRF_THUMBWHEEL_SMALL]);
		if (ASCPPitchSwitch.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_17x36], 0, 0, 0, 0, 17, 36, SURF_PREDEF_CK);
			}
		}
		return true;

	case AID_ASCPYAW:
		ascp.PaintYaw(surf, srf[SRF_THUMBWHEEL_SMALL]);
		if (ASCPYawSwitch.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_17x36], 0, 0, 0, 0, 17, 36, SURF_PREDEF_CK);
			}
		}
		return true;

	// FDAIs
	case AID_FDAI_LEFT:
		if (!fdaiDisabled){  // Is this FDAI enabled?
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;

			euler_rates = eda.GetFDAI1AttitudeRate();
			attitude = eda.GetFDAI1Attitude();
			errors = eda.GetFDAI1AttitudeError();

			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			if(errors.x > 41){ errors.x = 41; }else{ if(errors.x < -41){ errors.x = -41; }}
			if(errors.y > 41){ errors.y = 41; }else{ if(errors.y < -41){ errors.y = -41; }}
			if(errors.z > 41){ errors.z = 41; }else{ if(errors.z < -41){ errors.z = -41; }}
			fdaiLeft.PaintMe(attitude, 0, euler_rates, errors, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);			
		}
		return true;

	case AID_FDAI_RIGHT:
		if (!fdaiDisabled){  // Is this FDAI enabled?
			VECTOR3 euler_rates;
			VECTOR3 attitude;
			VECTOR3 errors;

			euler_rates = eda.GetFDAI2AttitudeRate();
			attitude = eda.GetFDAI2Attitude();
			errors = eda.GetFDAI2AttitudeError();

			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			if(errors.x > 41){ errors.x = 41; }else{ if(errors.x < -41){ errors.x = -41; }}
			if(errors.y > 41){ errors.y = 41; }else{ if(errors.y < -41){ errors.y = -41; }}
			if(errors.z > 41){ errors.z = 41; }else{ if(errors.z < -41){ errors.z = -41; }}
			fdaiRight.PaintMe(attitude, 0, euler_rates, errors, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);
		}
		return true;

	case AID_DSKY2_LIGHTS:
		dsky2.RenderLights(surf, srf[SRF_DSKY]);
		return true;

	case AID_DSKY2_DISPLAY:
		dsky2.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP]);
		return true;

	case AID_ABORT_LIGHT:
		if ((secs.AbortLightPowerA() && ((cws.UplinkTestState & 001) != 0)) || ((secs.AbortLightPowerB() && ((cws.UplinkTestState & 002) != 0))) || (iuCommandConnector.GetAbortLight())) {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 62, 0, 62, 31);
		} else {
			oapiBlt(surf,srf[SRF_ABORT], 0, 0, 0, 0, 62, 31);
		}
		return true;

	case AID_LV_ENGINE_LIGHTS:
		{
			if (SI_EngineNum > 5)
			{
				RenderS1bEngineLight(ENGIND[0], surf, srf[SRF_LV_ENG_S1B], 64, 42);
				RenderS1bEngineLight(ENGIND[1], surf, srf[SRF_LV_ENG_S1B], 64, 98);
				RenderS1bEngineLight(ENGIND[2], surf, srf[SRF_LV_ENG_S1B], 8, 98);
				RenderS1bEngineLight(ENGIND[3], surf, srf[SRF_LV_ENG_S1B], 7, 43);
				RenderS1bEngineLight(ENGIND[4], surf, srf[SRF_LV_ENG_S1B], 36, 41);
				RenderS1bEngineLight(ENGIND[5], surf, srf[SRF_LV_ENG_S1B], 51, 69);
				RenderS1bEngineLight(ENGIND[6], surf, srf[SRF_LV_ENG_S1B], 36, 98);
				RenderS1bEngineLight(ENGIND[7], surf, srf[SRF_LV_ENG_S1B], 22, 69);
			}
			else
			{
				if (ENGIND[0]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,44,55,44,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,44,157,44,27,27);
				}

				if (ENGIND[1]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,98,55,98,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],55,98,157,98,27,27);
				}
				if (ENGIND[2]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,98,20,98,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,98,122,98,27,27);
				}
				if (ENGIND[3]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,44,20,44,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],20,44,122,44,27,27);
				}
				if (ENGIND[4]) 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],37,71,37,71,27,27);
				}
				else 
				{
					oapiBlt(surf,srf[SRF_LV_ENG],37,71,140,71,27,27);
				}
			}
		}

		if (LVRateLight) 
		{
			oapiBlt(surf,srf[SRF_LV_ENG],6,4,6,4,27,27);
		}
		else 
		{
			oapiBlt(surf,srf[SRF_LV_ENG],6,4,108,4,27,27);
		}

		//
		// Saturn 1b doesn't have an SII sep light.
		//

		if (SaturnType == SAT_SATURNV)
		{
			if (SIISepState)
			{
				oapiBlt(surf,srf[SRF_LV_ENG],37,4,37,4,27,27);
			}
			else
			{
				oapiBlt(surf,srf[SRF_LV_ENG],37,4,139,4,27,27);
			}
		}

		if (LVGuidLight)
		{
			oapiBlt(surf,srf[SRF_LV_ENG],69,4,69,4,27,27);
		}
		else
		{
			oapiBlt(surf,srf[SRF_LV_ENG],69,4,171,4,27,27);
		}
		return true;

	// OPTICS
	case AID_OPTICSCLKAREASEXT:
		if (optics.SextDualView && optics.SextDVLOSTog){
			oapiCameraSetCockpitDir (-optics.OpticsShaft,-PI/2.,true); //when both are true show fixed line of sight
		}
		else
		{
			oapiCameraSetCockpitDir (-optics.OpticsShaft, optics.SextTrunion - PI/2., true); //negative allows Optics shaft to rotate clockwise positive, the PI/2 allows rotation around the perpindicular axis
		}
		//sprintf(oapiDebugString(), "Shaft %f, Trunion %f", optics.OpticsShaft/RAD, optics.SextTrunion/RAD);
		//sprintf(oapiDebugString(), "Shaft %f, Trunion %f", optics.OpticsShaft, optics.SextTrunion);
		return true;

	case AID_OPTICSCLKAREATELE:
		oapiCameraSetCockpitDir (-optics.OpticsShaft, optics.TeleTrunion - PI/2., true); //negative allows Optics shaft to rotate clockwise positive, the PI/2 allows rotation around the perpindicular axis
		//sprintf(oapiDebugString(), "Shaft %f, Trunion %f", optics.OpticsShaft/RAD, optics.TeleTrunion/RAD);
		return true;

	case AID_OPT_SHAFTDISPLAY:
		optics.PaintShaftDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_OPT_TRUNDISPLAY:
		optics.PaintTrunnionDisplay(surf,srf[SRF_THUMBWHEEL_LARGEFONTSINV]);
		return true;

	case AID_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_DIGITAL2], true);
		return true;

	case AID_MISSION_CLOCK306:
		MissionTimer306Display.Render90(surf, srf[SRF_DIGITAL90], true);
		return true;

	case AID_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_EVENT_TIMER_DIGITS]);
		return true;

	case AID_EVENT_TIMER306:
		EventTimer306Display.Render90(surf, srf[SRF_EVENT_TIMER_DIGITS90]);
		return true;

	case AID_ALTIMETER:
		RedrawPanel_Alt(surf);
		return true;

	case AID_ALTIMETER2:
		RedrawPanel_Alt2(surf);
		return true;

	case AID_MASTER_ALARM:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], (PanelFlashOn && MasterAlarmSwitch.IsFlashing() ? srf[SRF_BORDER_45x36] : 0), CWS_MASTERALARMPOSITION_LEFT);
		return true;

	case AID_MASTER_ALARM2:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], (PanelFlashOn && MasterAlarmSwitch.IsFlashing() ? srf[SRF_BORDER_45x36] : 0), CWS_MASTERALARMPOSITION_RIGHT);
		return true;

	case AID_MASTER_ALARM3:
		cws.RenderMasterAlarm(surf, srf[SRF_MASTERALARM_BRIGHT], (PanelFlashOn && MasterAlarmSwitch.IsFlashing() ? srf[SRF_BORDER_45x36] : 0), CWS_MASTERALARMPOSITION_NONE);
		return true;

	case AID_CWS_LIGHTS_LEFT:
		cws.RenderLights(surf, srf[SRF_CWSLIGHTS], true);
		return true;

	case AID_CWS_LIGHTS_RIGHT:
		cws.RenderLights(surf, srf[SRF_CWSLIGHTS], false);
		return true;

	case AID_CWS_GNLIGHTS:
		cws.RenderGNLights(surf, srf[SRF_CWS_GNLIGHTS]);
		return true;

	case AID_MFDMAINLEFT:
		if (oapiGetMFDMode(MFD_LEFT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton(surf, MFD_LEFT, 0, 14, 57, 44);
			RedrawPanel_MFDButton(surf, MFD_LEFT, 1, 392, 57, 44);
		}
		return true;

	case AID_MFDMAINRIGHT:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton(surf, MFD_RIGHT, 0, 14, 57, 44);
			RedrawPanel_MFDButton(surf, MFD_RIGHT, 1, 392, 57, 44);
		}
		return true;

	case AID_MFDGNLEFT:
		if (oapiGetMFDMode(MFD_LEFT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton(surf, MFD_LEFT, 0, 14, 57, 44);
			RedrawPanel_MFDButton(surf, MFD_LEFT, 1, 392, 57, 44);
		}
		return true;

	case AID_MFDGNUSER1:
		if (oapiGetMFDMode(MFD_USER1) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton(surf, MFD_USER1, 0, 14, 57, 44);
			RedrawPanel_MFDButton(surf, MFD_USER1, 1, 392, 57, 44);
		}
		return true;

	case AID_MFDGNUSER2:
		if (oapiGetMFDMode(MFD_USER2) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton(surf, MFD_USER2, 0, 14, 57, 44);
			RedrawPanel_MFDButton(surf, MFD_USER2, 1, 392, 57, 44);
		}
		return true;

	case AID_MFDGNRIGHT:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {
			oapiBlt(surf, srf[SRF_CMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton(surf, MFD_RIGHT, 0, 14, 57, 44);
			RedrawPanel_MFDButton(surf, MFD_RIGHT, 1, 392, 57, 44);
		}
		return true;

	case AID_MFDDOCK:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	// MFD_USER1
			oapiBlt(surf, srf[SRF_MFDFRAME], 0, 0, 0, 0, 301, 251);

			RedrawPanel_MFDButton(surf, MFD_RIGHT, 0, 7, 38, 31);	// MFD_USER1
			RedrawPanel_MFDButton(surf, MFD_RIGHT, 1, 274, 38, 31);	// MFD_USER1
		}
		return true;

	case AID_MFDDOCK_POWER:
		if (oapiGetMFDMode(MFD_RIGHT) == MFD_NONE) {	// MFD_USER1
			oapiBlt(surf, srf[SRF_MFDPOWER], 0, 0, 0, 0, 20, 15);
		}
		return true;

	case AID_COAS:
		if (coasEnabled) {
			oapiBlt(surf, srf[SRF_COAS], 0, 0, 0, 0, 683, 627, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_COAS], 0, 0, 0, 627, 683, 627, SURF_PREDEF_CK);
		}
		return true;

	case AID_SPS_LIGHT:
		if (ems.SPSThrustLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 41, 16, 41, 16);
		} else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 16, 41, 16);
		}
		return true;

	case AID_PT05G_LIGHT:
		if (ems.pt05GLight()) {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 41, 0, 41, 16);
		} else {
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 0, 0, 0, 0, 41, 16);
		}
		return true;

	case AID_EMS_SCROLL_LEO:
	{

		HDC hDC;

		hDC = oapiGetDC(srf[SRF_EMS_SCROLL_LEO]);

		SetBkMode(hDC, TRANSPARENT);
		HGDIOBJ oldObj = SelectObject(hDC, g_Param.pen[2]);

		Polyline(hDC, ems.ScribePntArray, ems.ScribePntCnt);
	
		SelectObject(hDC, oldObj);
		oapiReleaseDC(srf[SRF_EMS_SCROLL_LEO], hDC);

		oapiBlt(surf, srf[SRF_EMS_SCROLL_LEO], 5, 4, ems.GetScrollOffset(), 0, 132, 143);
		oapiBlt(surf, srf[SRF_EMS_SCROLL_BUG], 42, ems.GetGScribe() + 2, 0, 0, 5, 5, SURF_PREDEF_CK);
		oapiBlt(surf, srf[SRF_EMS_SCROLL_BORDER], 0, 0, 0, 0, 142, 150, SURF_PREDEF_CK);
		return true;
	}
	case AID_EMS_RSI_BKGRND:
	{
		HDC hDC;
		HGDIOBJ brush = NULL;
		HGDIOBJ pen = NULL;

		oapiBlt(surf, srf[SRF_EMS_RSI_BKGRND], 0, 0, 0, 0, 86, 84);
		switch (ems.LiftVectLight()) {
		case 1:
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 33, 8, 82, 6, 20, 6);
			break;
		case -1:
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 32, 69, 82, 22, 22, 10);
			break;
		case 0:
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 33, 8, 82, 0, 20, 6);
			oapiBlt(surf, srf[SRF_EMS_LIGHTS], 32, 69, 82, 12, 22, 10);
			break;
		}

		hDC = oapiGetDC(srf[SRF_EMS_RSI_BKGRND]);
		SetBkMode(hDC, TRANSPARENT);
		pen = SelectObject(hDC, GetStockObject(WHITE_PEN));
		Ellipse(hDC, 14, 14, 71, 68);
		brush = SelectObject(hDC, GetStockObject(BLACK_BRUSH));
		Polygon(hDC, ems.RSITriangle, 3);
		SelectObject(hDC, pen);
		SelectObject(hDC, brush);
		oapiReleaseDC(srf[SRF_EMS_RSI_BKGRND], hDC);
		return true;
	}
	case AID_EMSDVSETSWITCH:		
		switch ((int)EMSDvSetSwitch.GetPosition()) {
			case 1:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 0, 0, 55, 91);
				break;
			case 2:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 55, 0, 55, 91);
				break;
			case 3:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 220, 0, 55, 91);
				break;
			case 4:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 165, 0, 55, 91);
				break;
			default:
				oapiBlt(surf, srf[SRF_EMSDVSETSWITCH], 0, 0, 110, 0, 55, 91);
				break;
		}
		if (EMSDvDisplay.IsFlashing() || EMSScrollDisplay.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_55x91], 0, 0, 0, 0, 55, 91, SURF_PREDEF_CK);
			}
		}
		return true;
	}
	return false;
}

void Saturn::clbkMFDMode (int mfd, int mode) {

	switch (mfd) {
	case MFD_LEFT:
		TriggerPanelRedrawArea(SATPANEL_MAIN, AID_MFDMAINLEFT);
		TriggerPanelRedrawArea(SATPANEL_MAIN_MIDDLE, AID_MFDMAINLEFT);
		TriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNLEFT);
		break;

	case MFD_RIGHT:
		if (!MainPanelSplit) TriggerPanelRedrawArea(SATPANEL_MAIN, AID_MFDMAINRIGHT);
		TriggerPanelRedrawArea(SATPANEL_MAIN_MIDDLE, AID_MFDMAINRIGHT);
		TriggerPanelRedrawArea(SATPANEL_MAIN_RIGHT, AID_MFDMAINRIGHT);
		TriggerPanelRedrawArea(SATPANEL_LEFT_RNDZ_WINDOW, AID_MFDDOCK);
		TriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNRIGHT);
		break;

	case MFD_USER1:
		TriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNUSER1);
		break;

	case MFD_USER2:
		TriggerPanelRedrawArea(SATPANEL_GN, AID_MFDGNUSER2);
		break;

	}
}

//
// Set switches to default state.
//

void Saturn::InitSwitches() {

	coasEnabled = false;
	ordealEnabled = 0;
	opticsDskyEnabled = false;
	hatchPanel600EnabledLeft = 0;
	hatchPanel600EnabledRight = 0;
	panel382Enabled = 0;

	MasterAlarmSwitch.Register(PSH, "MasterAlarmSwitch", TOGGLESWITCH_DOWN);

	AccelGMeter.Register(PSH, "AccelGMeter", -1, 15, 1);

	THCRotary.AddPosition(0, 330);
	THCRotary.AddPosition(1, 0);
	THCRotary.AddPosition(2, 30);
	THCRotary.Register(PSH, "THCRotary", 1);

	LiftoffNoAutoAbortSwitch.Register(PSH, "LiftoffNoAutoAbortSwitch", false, false);
	LesMotorFireSwitch.Register(PSH, "LesMotorFireSwitch", false, false);
	CanardDeploySwitch.Register(PSH, "CanardDeploySwitch", false, false);
	CsmLvSepSwitch.Register(PSH, "CsmLvSepSwitch", false, false);
	CsmLvSepSwitch.SetDelayTime(1);
	ApexCoverJettSwitch.Register(PSH, "ApexCoverJettSwitch", false, false);
	DrogueDeploySwitch.Register(PSH, "DrogueDeploySwitch", false, false);
	MainDeploySwitch.Register(PSH, "MainDeploySwitch", false, false);
	CmRcsHeDumpSwitch.Register(PSH, "CmRcsHeDumpSwitch", false, false);

	if (!SkylabCM) {
		SIVBPayloadSepSwitch.Register(PSH, "SIVBPayloadSepSwitch", TOGGLESWITCH_DOWN, 0, SPRINGLOADEDSWITCH_DOWN);
		SIVBPayloadSepSwitch.SetDelayTime(2);
	}

	MissionTimerSwitch.Register(PSH, "MissionTimerSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);
	CautionWarningModeSwitch.Register(PSH, "CautionWarningModeSwitch", THREEPOSSWITCH_UP);
	CautionWarningCMCSMSwitch.Register(PSH, "CautionWarningCMCSMSwitch", 1);
	CautionWarningPowerSwitch.Register(PSH, "CautionWarningPowerSwitch", THREEPOSSWITCH_CENTER);
	CautionWarningLightTestSwitch.Register(PSH, "CautionWarningLightTestSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER, NULL);

	CabinFan1Switch.Register(PSH, "CabinFan1Switch", false);
	CabinFan2Switch.Register(PSH, "CabinFan2Switch", false);
	H2Heater1Switch.Register(PSH, "H2Heater1Switch", THREEPOSSWITCH_CENTER);
	H2Heater2Switch.Register(PSH, "H2Heater2Switch", THREEPOSSWITCH_CENTER);
	O2Heater1Switch.Register(PSH, "O2Heater1Switch", THREEPOSSWITCH_CENTER);
	O2Heater2Switch.Register(PSH, "O2Heater2Switch", THREEPOSSWITCH_CENTER);
	O2PressIndSwitch.Register(PSH, "O2PressIndSwitch", true);
	H2Fan1Switch.Register(PSH, "H2Fan1Switch", THREEPOSSWITCH_CENTER);
	H2Fan2Switch.Register(PSH, "H2Fan2Switch", THREEPOSSWITCH_CENTER);
	O2Fan1Switch.Register(PSH, "O2Fan1Switch", THREEPOSSWITCH_CENTER);
	O2Fan2Switch.Register(PSH, "O2Fan2Switch", THREEPOSSWITCH_CENTER);

	SCContSwitch.Register(PSH, "SCContSwitch", false);
	CMCModeSwitch.Register(PSH, "CMCModeSwitch", THREEPOSSWITCH_DOWN);

	SCSTvcPitchSwitch.Register(PSH, "SCSTvcPitchSwitch", THREEPOSSWITCH_CENTER);
	SCSTvcYawSwitch.Register(PSH, "SCSTvcYawSwitch", THREEPOSSWITCH_CENTER);

	Pitch1Switch.Register(PSH, "Pitch1Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	Pitch1Switch.SetDelayTime(1);
	Pitch2Switch.Register(PSH, "Pitch2Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	Pitch2Switch.SetDelayTime(1);
	Yaw1Switch.Register(PSH, "Yaw1Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	Yaw1Switch.SetDelayTime(1);
	Yaw2Switch.Register(PSH, "Yaw2Switch", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	Yaw2Switch.SetDelayTime(1);

	EMSRollSwitch.Register(PSH, "EMSRollSwitch", false);
	GSwitch.Register(PSH, "GSwitch", false);

	LVSPSPcIndicatorSwitch.Register(PSH, "LVSPSPcIndicatorSwitch", TOGGLESWITCH_UP);
	LVFuelTankPressIndicatorSwitch.Register(PSH, "LVFuelTankPressIndicatorSwitch", TOGGLESWITCH_UP);

	TVCGimbalDrivePitchSwitch.Register(PSH, "TVCGimbalDrivePitchSwitch", THREEPOSSWITCH_CENTER);
	TVCGimbalDriveYawSwitch.Register(PSH, "TVCGimbalDriveYawSwitch", THREEPOSSWITCH_CENTER);

	RunEVALightSwitch.Register(PSH, "RunEVALightSwitch", false);
	RndzLightSwitch.Register(PSH, "RndzLightSwitch", THREEPOSSWITCH_CENTER);
	TunnelLightSwitch.Register(PSH, "TunnelLightSwitch", false);

	LMPowerSwitch.Register(PSH, "LMPowerSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);

	PostLDGVentValveLever.Register(PSH, "PostLDGVentValveLever", 1);

	GDCAlignButton.Register(PSH, "GDCAlignButton", false);
	GDCAlignButton.SetDelayTime(2);

	GHATrackSwitch.Register(PSH, "GHATrackSwitch", THREEPOSSWITCH_UP);
	GHABeamSwitch.Register(PSH, "GHABeamSwitch", THREEPOSSWITCH_UP);

	GHAPowerSwitch.Register(PSH, "GHAPowerSwitch", THREEPOSSWITCH_DOWN);
	GHAServoElecSwitch.Register(PSH, "GHAServoElecSwitch", TOGGLESWITCH_UP);
	
	FuelCellPhIndicator.Register(PSH, "FuelCellPhIndicator", false);
	FuelCellRadTempIndicator.Register(PSH, "FuelCellRadTempIndicator", false);

	FuelCellRadiators1Indicator.Register(PSH, "FuelCellRadiators1Indicator", false);
	FuelCellRadiators2Indicator.Register(PSH, "FuelCellRadiators2Indicator", false);
	FuelCellRadiators3Indicator.Register(PSH, "FuelCellRadiators3Indicator", false);
	FuelCellRadiators1Switch.Register(PSH, "FuelCellRadiators1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellRadiators2Switch.Register(PSH, "FuelCellRadiators2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellRadiators3Switch.Register(PSH, "FuelCellRadiators3Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	FuelCellIndicatorsSwitch.AddPosition(0, 330);
	FuelCellIndicatorsSwitch.AddPosition(1,   0);
	FuelCellIndicatorsSwitch.AddPosition(2,  30);
	FuelCellIndicatorsSwitch.Register(PSH, "FuelCellIndicatorsSwitch", 0);

	FuelCellHeater1Switch.Register(PSH, "FuelCellHeater1Switch", true);
	FuelCellHeater2Switch.Register(PSH, "FuelCellHeater2Switch", true);
	FuelCellHeater3Switch.Register(PSH, "FuelCellHeater3Switch", true);

	FuelCellPurge1Switch.Register(PSH, "FuelCellPurge1Switch", THREEPOSSWITCH_CENTER);
	FuelCellPurge2Switch.Register(PSH, "FuelCellPurge2Switch", THREEPOSSWITCH_CENTER);
	FuelCellPurge3Switch.Register(PSH, "FuelCellPurge3Switch", THREEPOSSWITCH_CENTER);

	FuelCellReactants1Indicator.Register(PSH, "FuelCellReactants1Indicator", false);
	FuelCellReactants2Indicator.Register(PSH, "FuelCellReactants2Indicator", false);
	FuelCellReactants3Indicator.Register(PSH, "FuelCellReactants3Indicator", false);
	FuelCellReactants1Switch.Register(PSH, "FuelCellReactants1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellReactants2Switch.Register(PSH, "FuelCellReactants2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	FuelCellReactants3Switch.Register(PSH, "FuelCellReactants3Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	FCReacsValvesSwitch.Register(PSH, "FCReacsValvesSwitch", true);
	H2PurgeLineSwitch.Register  (PSH, "H2PurgeLineSwitch",   false);

	FuelCellPumps1Switch.Register(PSH, "FuelCellPumps1Switch", THREEPOSSWITCH_UP);
	FuelCellPumps2Switch.Register(PSH, "FuelCellPumps2Switch", THREEPOSSWITCH_DOWN);
	FuelCellPumps3Switch.Register(PSH, "FuelCellPumps3Switch", THREEPOSSWITCH_DOWN);

	SuitCompressor1Switch.Register(PSH, "SuitCompressor1Switch", THREEPOSSWITCH_CENTER);
	SuitCompressor2Switch.Register(PSH, "SuitCompressor2Switch", THREEPOSSWITCH_CENTER);

	RightCOASPowerSwitch.Register(PSH, "RightCOASPowerSwitch", false);

	AudioControlSwitch.Register(PSH, "AudioControlSwitch", TOGGLESWITCH_UP);

	SuitPowerSwitch.Register(PSH, "SuitPowerSwitch", false);

	RightUtilityPowerSwitch.Register(PSH, "RightUtilityPowerSwitch", false);

	RightDockingTargetSwitch.Register(PSH, "RightDockingTargetSwitch", THREEPOSSWITCH_DOWN);

	RightModeIntercomSwitch.Register(PSH, "RightModeIntercomSwitch", THREEPOSSWITCH_UP);

	RightAudioPowerSwitch.Register(PSH, "RightAudioPowerSwitch", THREEPOSSWITCH_CENTER);

	RightPadCommSwitch.Register(PSH, "RightPadCommSwitch", THREEPOSSWITCH_UP);

	RightIntercomSwitch.Register(PSH, "RightIntercomSwitch", THREEPOSSWITCH_UP);

	RightSBandSwitch.Register(PSH, "RightSBandSwitch", THREEPOSSWITCH_UP);

	RightVHFAMSwitch.Register(PSH, "RightVHFAMSwitch", THREEPOSSWITCH_UP);

	GNPowerSwitch.Register(PSH, "GNPowerSwitch", THREEPOSSWITCH_UP);

	MainBusTieBatAcSwitch.Register(PSH, "MainBusTieBatAcSwitch", THREEPOSSWITCH_DOWN);
	MainBusTieBatBcSwitch.Register(PSH, "MainBusTieBatBcSwitch", THREEPOSSWITCH_DOWN);

	BatCHGRSwitch.Register(PSH, "BatCHGRSwitch", THREEPOSSWITCH_UP);

	NonessBusSwitch.Register(PSH, "NonessBusSwitch", THREEPOSSWITCH_CENTER);

	InteriorLightsFloodDimSwitch.Register(PSH, "InteriorLightsFloodDimSwitch", TOGGLESWITCH_UP);
	InteriorLightsFloodFixedSwitch.Register(PSH, "InteriorLightsFloodFixedSwitch", false);

	FloatBagSwitch1.Register(PSH, "FloatBagSwitch1", THREEPOSSWITCH_DOWN);
	FloatBagSwitch2.Register(PSH, "FloatBagSwitch2", THREEPOSSWITCH_DOWN);
	FloatBagSwitch3.Register(PSH, "FloatBagSwitch3", THREEPOSSWITCH_DOWN);

	SECSLogic1Switch.Register(PSH, "SECSLogic1Switch", false);
	SECSLogic2Switch.Register(PSH, "SECSLogic2Switch", false);
	PyroArmASwitch.Register(PSH, "PyroArmASwitch", false);
	PyroArmBSwitch.Register(PSH, "PyroArmBSwitch", false);

	EDSPowerSwitch.Register(PSH, "EDSPowerSwitch", false);

	TVCServoPower1Switch.Register(PSH, "TVCServoPower1Switch", THREEPOSSWITCH_CENTER);
	TVCServoPower2Switch.Register(PSH, "TVCServoPower2Switch", THREEPOSSWITCH_CENTER);

	LogicPowerSwitch.Register(PSH, "LogicPowerSwitch", true);

	SIGCondDriverBiasPower1Switch.Register(PSH, "SIGCondDriverBiasPower1Switch", THREEPOSSWITCH_CENTER);
	SIGCondDriverBiasPower2Switch.Register(PSH, "SIGCondDriverBiasPower2Switch", THREEPOSSWITCH_CENTER);

	SPSGaugingSwitch.Register(PSH, "SPSGaugingSwitch", THREEPOSSWITCH_UP);

	TelcomGroup1Switch.Register(PSH, "TelcomGroup1Switch", THREEPOSSWITCH_UP);
	TelcomGroup2Switch.Register(PSH, "TelcomGroup2Switch", THREEPOSSWITCH_DOWN);

	LeftCOASPowerSwitch.Register(PSH, "LeftCOASPowerSwitch", false);

	LeftUtilityPowerSwitch.Register(PSH, "LeftUtilityPowerSwitch", false);

	PostLandingBCNLTSwitch.Register(PSH, "PostLandingBCNLTSwitch", THREEPOSSWITCH_CENTER);

	PostLandingDYEMarkerSwitch.Register(PSH, "PostLandingDYEMarkerSwitch", false, false);

	GTASwitch.Register(PSH, "GTASwitch", false, false);
	
	PostLandingVentSwitch.Register(PSH, "PostLandingVentSwitch", THREEPOSSWITCH_DOWN);

	LeftModeIntercomVOXSensThumbwheelSwitch.Register(PSH, "LeftModeIntercomVOXSensThumbwheelSwitch", 5, 9);

	LeftPowerMasterVolumeThumbwheelSwitch.Register(PSH, "LeftPowerMasterVolumeThumbwheelSwitch", 5, 9);

	LeftPadCommVolumeThumbwheelSwitch.Register(PSH, "LeftPadCommVolumeThumbwheelSwitch", 5, 9);

	LeftIntercomVolumeThumbwheelSwitch.Register(PSH, "LeftIntercomVolumeThumbwheelSwitch", 5, 9);

	LeftSBandVolumeThumbwheelSwitch.Register(PSH, "LeftSBandVolumeThumbwheelSwitch", 5, 9);

	LeftVHFAMVolumeThumbwheelSwitch.Register(PSH, "LeftVHFAMVolumeThumbwheelSwitch", 5, 9);

	LeftAudioVOXSensThumbwheel.Register(PSH, "LeftAudioVOXSensThumbwheel", 5, 9);
	LeftAudioPadComVolumeThumbwheel.Register(PSH, "LeftAudioPadComVolumeThumbwheel", 5, 9);
	LeftAudioSBandVolumeThumbwheel.Register(PSH, "LeftAudioSBandVolumeThumbwheel", 5, 9);

	RightAudioMasterVolumeThumbwheel.Register(PSH, "RightAudioMasterVolumeThumbwheel", 5, 9);
	RightAudioIntercomVolumeThumbwheel.Register(PSH, "RightAudioIntercomVolumeThumbwheel", 5, 9);
	RightAudioVHFAMVolumeThumbwheel.Register(PSH, "RightAudioVHFAMVolumeThumbwheel", 5, 9);

	LeftModeIntercomSwitch.Register(PSH, "LeftModeIntercomSwitch", THREEPOSSWITCH_UP);

	LeftAudioPowerSwitch.Register(PSH, "LeftAudioPowerSwitch", THREEPOSSWITCH_CENTER);

	LeftPadCommSwitch.Register(PSH, "LeftPadCommSwitch", THREEPOSSWITCH_UP);

	LeftIntercomSwitch.Register(PSH, "LeftIntercomSwitch", THREEPOSSWITCH_UP);

	LeftSBandSwitch.Register(PSH, "LeftSBandSwitch", THREEPOSSWITCH_UP);

	LeftVHFAMSwitch.Register(PSH, "LeftVHFAMSwitch", THREEPOSSWITCH_UP);

	LeftAudioControlSwitch.Register(PSH, "LeftAudioControlSwitch", THREEPOSSWITCH_UP);

	LeftSuitPowerSwitch.Register(PSH, "LeftSuitPowerSwitch", false);

	VHFRNGSwitch.Register(PSH, "VHFRNGSwitch", false, SPRINGLOADEDSWITCH_DOWN);

	AcRollA1Switch.Register(PSH, "AcRollA1Switch", THREEPOSSWITCH_CENTER);
	AcRollC1Switch.Register(PSH, "AcRollC1Switch", THREEPOSSWITCH_CENTER);
	AcRollA2Switch.Register(PSH, "AcRollA2Switch", THREEPOSSWITCH_CENTER);
	AcRollC2Switch.Register(PSH, "AcRollC2Switch", THREEPOSSWITCH_CENTER);
	BdRollB1Switch.Register(PSH, "BdRollB1Switch", THREEPOSSWITCH_CENTER);
	BdRollD1Switch.Register(PSH, "BdRollD1Switch", THREEPOSSWITCH_CENTER);
	BdRollB2Switch.Register(PSH, "BdRollB2Switch", THREEPOSSWITCH_CENTER);
	BdRollD2Switch.Register(PSH, "BdRollD2Switch", THREEPOSSWITCH_CENTER);
	PitchA3Switch.Register(PSH, "PitchA3Switch", THREEPOSSWITCH_CENTER);
	PitchC3Switch.Register(PSH, "PitchC3Switch", THREEPOSSWITCH_CENTER);
	PitchA4Switch.Register(PSH, "PitchA4Switch", THREEPOSSWITCH_CENTER);
	PitchC4Switch.Register(PSH, "PitchC4Switch", THREEPOSSWITCH_CENTER);
	YawB3Switch.Register(PSH, "YawB3Switch", THREEPOSSWITCH_CENTER);
	YawD3Switch.Register(PSH, "YawD3Switch", THREEPOSSWITCH_CENTER);
	YawB4Switch.Register(PSH, "YawB4Switch", THREEPOSSWITCH_CENTER);
	YawD4Switch.Register(PSH, "YawD4Switch", THREEPOSSWITCH_CENTER);

	FloodDimSwitch.Register(PSH, "FloodDimSwitch", TOGGLESWITCH_UP);

	FloodFixedSwitch.Register(PSH, "FloodFixedSwitch", THREEPOSSWITCH_CENTER);

	//RightWindowCoverSwitch.Register(PSH, "RightWindowCoverSwitch", false);

	SBandNormalXPDRSwitch.Register(PSH, "SBandNormalXPDRSwitch", THREEPOSSWITCH_CENTER);
	SBandNormalPwrAmpl1Switch.Register(PSH, "SBandNormalPwrAmpl1Switch", TOGGLESWITCH_UP);
	SBandNormalPwrAmpl2Switch.Register(PSH, "SBandNormalPwrAmpl2Switch", THREEPOSSWITCH_CENTER);
	SBandNormalMode1Switch.Register(PSH, "SBandNormalMode1Switch", THREEPOSSWITCH_UP);
	SBandNormalMode2Switch.Register(PSH, "SBandNormalMode2Switch", THREEPOSSWITCH_UP);
	SBandNormalMode3Switch.Register(PSH, "SBandNormalMode3Switch", TOGGLESWITCH_UP);

	PwrAmplTB.Register(PSH, "PwrAmplTB", false);
	DseTapeTB.Register(PSH, "DseTapeTB", false);

	SBandAuxSwitch1.Register(PSH, "SBandAuxSwitch1", THREEPOSSWITCH_CENTER);
	SBandAuxSwitch2.Register(PSH, "SBandAuxSwitch2", THREEPOSSWITCH_CENTER);

	UPTLMSwitch1.Register(PSH, "UPTLMSwitch1", TOGGLESWITCH_UP);
	UPTLMSwitch2.Register(PSH, "UPTLMSwitch2", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);

	SBandAntennaSwitch1.Register(PSH, "SBandAntennaSwitch1", THREEPOSSWITCH_CENTER);
	SBandAntennaSwitch2.Register(PSH, "SBandAntennaSwitch2", THREEPOSSWITCH_UP);

	VHFAMASwitch.Register(PSH, "VHFAMASwitch", THREEPOSSWITCH_CENTER);
	VHFAMBSwitch.Register(PSH, "VHFAMBSwitch", THREEPOSSWITCH_CENTER);
	RCVOnlySwitch.Register(PSH, "RCVOnlySwitch", THREEPOSSWITCH_CENTER);
	VHFBeaconSwitch.Register(PSH, "VHFBeaconSwitch", false);
	VHFRangingSwitch.Register(PSH, "VHFRangingSwitch", false);

	TapeRecorderPCMSwitch.Register(PSH, "TapeRecorderPCMSwitch", TOGGLESWITCH_UP);
	TapeRecorderRecordSwitch.Register(PSH, "TapeRecorderRecordSwitch", THREEPOSSWITCH_UP);
	TapeRecorderForwardSwitch.Register(PSH, "TapeRecorderForwardSwitch", THREEPOSSWITCH_CENTER);

	SCESwitch.Register(PSH, "SCESwitch", THREEPOSSWITCH_UP);
	PMPSwitch.Register(PSH, "PMPSwitch", THREEPOSSWITCH_UP);

	PCMBitRateSwitch.Register(PSH, "PCMBitRateSwitch", TOGGLESWITCH_UP);

	MnA1Switch.Register(PSH, "MnA1Switch", true);
	MnB2Switch.Register(PSH, "MnB2Switch", true);
	MnA3Switch.Register(PSH, "MnA3Switch", THREEPOSSWITCH_CENTER);
	AcBus1Switch1.Register(PSH, "AcBus1Switch1", true);
	AcBus1Switch2.Register(PSH, "AcBus1Switch2", false);
	AcBus1Switch3.Register(PSH, "AcBus1Switch3", false);
	AcBus1ResetSwitch.Register(PSH, "AcBus1ResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP, NULL);
	AcBus1ResetSwitch.SetDelayTime(1);
	AcBus2Switch1.Register(PSH, "AcBus2Switch1", false);
	AcBus2Switch2.Register(PSH, "AcBus2Switch2", true);
	AcBus2Switch3.Register(PSH, "AcBus2Switch3", false);
	AcBus2ResetSwitch.Register(PSH, "AcBus2ResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP, NULL);
	AcBus2ResetSwitch.SetDelayTime(1);
	
	MainBusASwitch1.Register(PSH, "MainBusASwitch1", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusASwitch1.SetDelayTime(1);
	MainBusASwitch2.Register(PSH, "MainBusASwitch2", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusASwitch2.SetDelayTime(1);
	MainBusASwitch3.Register(PSH, "MainBusASwitch3", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusASwitch3.SetDelayTime(1);
	MainBusAResetSwitch.Register(PSH, "MainBusAResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBSwitch1.Register(PSH, "MainBusBSwitch1", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBSwitch1.SetDelayTime(1);
	MainBusBSwitch2.Register(PSH, "MainBusBSwitch2", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBSwitch2.SetDelayTime(1);
	MainBusBSwitch3.Register(PSH, "MainBusBSwitch3", THREEPOSSWITCH_DOWN, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBSwitch3.SetDelayTime(1);
	MainBusBResetSwitch.Register(PSH, "MainBusBResetSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	MainBusBIndicator1.Register(PSH, "MainBusBIndicator1", true);
	MainBusBIndicator2.Register(PSH, "MainBusBIndicator2", true);
	MainBusBIndicator3.Register(PSH, "MainBusBIndicator3", true);
	MainBusAIndicator1.Register(PSH, "MainBusAIndicator1", true);
	MainBusAIndicator2.Register(PSH, "MainBusAIndicator2", true);
	MainBusAIndicator3.Register(PSH, "MainBusAIndicator3", true);
		
	MissionTimerHoursSwitch.Register(PSH, "MissionTimerHoursSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	MissionTimerMinutesSwitch.Register(PSH, "MissionTimerMinutesSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	MissionTimerSecondsSwitch.Register(PSH, "MissionTimerSecondsSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	SMRCSHelium1ASwitch.Register(PSH, "SMRCSHelium1ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium1ASwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadARCS.GetHeliumValve1(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSHelium1BSwitch.Register(PSH, "SMRCSHelium1BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium1BSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadBRCS.GetHeliumValve1(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSHelium1CSwitch.Register(PSH, "SMRCSHelium1CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium1CSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadCRCS.GetHeliumValve1(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSHelium1DSwitch.Register(PSH, "SMRCSHelium1DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium1DSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadDRCS.GetHeliumValve1(), &SMRCSHeliumValve::SwitchToggled));

	SMRCSHelium1ATalkback.Register(PSH, "SMRCSHelium1ATalkback", true);
	SMRCSHelium1BTalkback.Register(PSH, "SMRCSHelium1BTalkback", true);
	SMRCSHelium1CTalkback.Register(PSH, "SMRCSHelium1CTalkback", true);
	SMRCSHelium1DTalkback.Register(PSH, "SMRCSHelium1DTalkback", true);

	SMRCSHelium2ASwitch.Register(PSH, "SMRCSHelium2ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium2ASwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadARCS.GetHeliumValve2(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSHelium2BSwitch.Register(PSH, "SMRCSHelium2BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium2BSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadBRCS.GetHeliumValve2(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSHelium2CSwitch.Register(PSH, "SMRCSHelium2CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium2CSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadCRCS.GetHeliumValve2(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSHelium2DSwitch.Register(PSH, "SMRCSHelium2DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSHelium2DSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadDRCS.GetHeliumValve2(), &SMRCSHeliumValve::SwitchToggled));

	SMRCSHelium2ATalkback.Register(PSH, "SMRCSHelium2ATalkback", true);
	SMRCSHelium2BTalkback.Register(PSH, "SMRCSHelium2BTalkback", true);
	SMRCSHelium2CTalkback.Register(PSH, "SMRCSHelium2CTalkback", true);
	SMRCSHelium2DTalkback.Register(PSH, "SMRCSHelium2DTalkback", true);

	SMRCSProp1ASwitch.Register(PSH, "SMRCSProp1ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp1ASwitch.SetCallback(new PanelSwitchCallback<SMRCSPropellantSource>(&SMQuadARCS, &SMRCSPropellantSource::PropellantSwitchToggled));
	SMRCSProp1BSwitch.Register(PSH, "SMRCSProp1BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp1BSwitch.SetCallback(new PanelSwitchCallback<SMRCSPropellantSource>(&SMQuadBRCS, &SMRCSPropellantSource::PropellantSwitchToggled));
	SMRCSProp1CSwitch.Register(PSH, "SMRCSProp1CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp1CSwitch.SetCallback(new PanelSwitchCallback<SMRCSPropellantSource>(&SMQuadCRCS, &SMRCSPropellantSource::PropellantSwitchToggled));
	SMRCSProp1DSwitch.Register(PSH, "SMRCSProp1DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp1DSwitch.SetCallback(new PanelSwitchCallback<SMRCSPropellantSource>(&SMQuadDRCS, &SMRCSPropellantSource::PropellantSwitchToggled));

	SMRCSProp1ATalkback.Register(PSH, "SMRCSProp1ATalkback", false);
	SMRCSProp1BTalkback.Register(PSH, "SMRCSProp1BTalkback", false);
	SMRCSProp1CTalkback.Register(PSH, "SMRCSProp1CTalkback", false);
	SMRCSProp1DTalkback.Register(PSH, "SMRCSProp1DTalkback", false);

	SMRCSProp2ASwitch.Register(PSH, "SMRCSProp2ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp2ASwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadARCS.GetSecPropellantPressureValve(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSProp2BSwitch.Register(PSH, "SMRCSProp2BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp2BSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadBRCS.GetSecPropellantPressureValve(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSProp2CSwitch.Register(PSH, "SMRCSProp2CSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp2CSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadCRCS.GetSecPropellantPressureValve(), &SMRCSHeliumValve::SwitchToggled));
	SMRCSProp2DSwitch.Register(PSH, "SMRCSProp2DSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SMRCSProp2DSwitch.SetCallback(new PanelSwitchCallback<SMRCSHeliumValve>(SMQuadDRCS.GetSecPropellantPressureValve(), &SMRCSHeliumValve::SwitchToggled));

	SMRCSProp2ATalkback.Register(PSH, "SMRCSProp2ATalkback", false);
	SMRCSProp2BTalkback.Register(PSH, "SMRCSProp2BTalkback", false);
	SMRCSProp2CTalkback.Register(PSH, "SMRCSProp2CTalkback", false);
	SMRCSProp2DTalkback.Register(PSH, "SMRCSProp2DTalkback", false);

	SPSInjectorValve1Indicator.Register(PSH, "SPSInjectorValve1Indicator", false);
	SPSInjectorValve2Indicator.Register(PSH, "SPSInjectorValve2Indicator", false);
	SPSInjectorValve3Indicator.Register(PSH, "SPSInjectorValve3Indicator", false);
	SPSInjectorValve4Indicator.Register(PSH, "SPSInjectorValve4Indicator", false);

	SPSOxidFlowValveMaxIndicator.Register(PSH, "SPSOxidFlowValveMaxIndicator", true);
	SPSOxidFlowValveMinIndicator.Register(PSH, "SPSOxidFlowValveMinIndicator", true);

	SPSOxidFlowValveSwitch.Register(PSH, "SPSOxidFlowValveSwitch", THREEPOSSWITCH_UP);	
	SPSOxidFlowValveSelectorSwitch.Register(PSH, "SPSOxidFlowValveSelectorSwitch", TOGGLESWITCH_UP);
	SPSPugModeSwitch.Register(PSH, "SPSPugModeSwitch", THREEPOSSWITCH_CENTER);

	SPSHeliumValveAIndicator.Register(PSH, "SPSHeliumValveAIndicator", true);
	SPSHeliumValveBIndicator.Register(PSH, "SPSHeliumValveBIndicator", true);

	SPSHeliumValveASwitch.Register(PSH, "SPSHeliumValveASwitch", THREEPOSSWITCH_UP);
	SPSHeliumValveBSwitch.Register(PSH, "SPSHeliumValveBSwitch", THREEPOSSWITCH_UP);
	SPSLineHTRSSwitch.Register(PSH, "SPSLineHTRSSwitch", THREEPOSSWITCH_CENTER);
	SPSPressIndSwitch.Register(PSH, "SPSPressIndSwitch", THREEPOSSWITCH_UP);
	
	CMUplinkSwitch.Register(PSH, "CMUplinkSwitch", TOGGLESWITCH_DOWN);

	if (!SkylabCM)
		IUUplinkSwitch.Register(PSH, "IUUplinkSwitch", TOGGLESWITCH_DOWN);

	CMRCSPressSwitch.Register(PSH, "CMRCSPressSwitch", 0, 0, SPRINGLOADEDSWITCH_DOWN);
	SMRCSIndSwitch.Register(PSH, "SMRCSIndSwitch", TOGGLESWITCH_UP);

	RCSQuantityMeter.Register(PSH, "RCSQuantityMeter", 0.0, 1.0, 2);
	RCSFuelPressMeter.Register(PSH, "RCSFuelPressMeter", 0.0, 400.0, 2);
	RCSHeliumPressMeter.Register(PSH, "RCSHeliumPressMeter", 0.0, 5000.0, 2);
	RCSTempMeter.Register(PSH, "RCSTempMeter", 0.0, 300.0, 2);

	SMRCSHeaterASwitch.Register(PSH, "SMRCSHeaterASwitch", THREEPOSSWITCH_CENTER);
	SMRCSHeaterBSwitch.Register(PSH, "SMRCSHeaterBSwitch", THREEPOSSWITCH_CENTER);
	SMRCSHeaterCSwitch.Register(PSH, "SMRCSHeaterCSwitch", THREEPOSSWITCH_CENTER);
	SMRCSHeaterDSwitch.Register(PSH, "SMRCSHeaterDSwitch", THREEPOSSWITCH_CENTER);

	RCSCMDSwitch.Register(PSH, "RCSCMDSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSCMDSwitch.SetDelayTime(1);
	RCSTrnfrSwitch.Register(PSH, "RCSTrnfrSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSTrnfrSwitch.SetDelayTime(1);

	CMRCSProp1Switch.Register(PSH, "CMRCSProp1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	CMRCSProp1Switch.SetCallback(new PanelSwitchCallback<CMRCSPropellantSource>(&CMRCS1, &CMRCSPropellantSource::PropellantSwitchToggled));
	CMRCSProp2Switch.Register(PSH, "CMRCSProp2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	CMRCSProp2Switch.SetCallback(new PanelSwitchCallback<CMRCSPropellantSource>(&CMRCS2, &CMRCSPropellantSource::PropellantSwitchToggled));

	CMRCSProp1Talkback.Register(PSH, "CMRCSProp1Talkback", false);
	CMRCSProp2Talkback.Register(PSH, "CMRCSProp2Talkback", false);

	ManualAttRollSwitch.Register(PSH, "ManualAttRollSwitch", THREEPOSSWITCH_CENTER);
	ManualAttPitchSwitch.Register(PSH, "ManualAttPitchSwitch", THREEPOSSWITCH_UP);
	ManualAttYawSwitch.Register(PSH, "ManualAttYawSwitch", THREEPOSSWITCH_CENTER);

	LimitCycleSwitch.Register(PSH, "LimitCycleSwitch", 0);
	AttDeadbandSwitch.Register(PSH, "AttDeadbandSwitch", 0);
	AttRateSwitch.Register(PSH, "AttRateSwitch", TOGGLESWITCH_UP);
	TransContrSwitch.Register(PSH, "TransContrSwitch", 0);

	BMAGRollSwitch.Register(PSH, "BMAGRollSwitch", THREEPOSSWITCH_UP);
	BMAGPitchSwitch.Register(PSH, "BMAGPitchSwitch", THREEPOSSWITCH_UP);
	BMAGYawSwitch.Register(PSH, "BMAGYawSwitch", THREEPOSSWITCH_UP);

	EMSModeSwitch.Register(PSH, "EMSModeSwitch", THREEPOSSWITCH_CENTER);
	CMCAttSwitch.Register(PSH, "CMCAttSwitch", 1);

	FDAIScaleSwitch.Register(PSH, "FDAIScaleSwitch", THREEPOSSWITCH_UP);
	FDAISelectSwitch.Register(PSH, "FDAISelectSwitch", THREEPOSSWITCH_UP);
	FDAISourceSwitch.Register(PSH, "FDAISourceSwitch", THREEPOSSWITCH_UP);
	FDAIAttSetSwitch.Register(PSH, "FDAIAttSetSwitch", false);

	IMUGuardedCageSwitch.Register(PSH, "IMUGuardedCageSwitch", 0, 0);

	RCSIndicatorsSwitch.AddPosition(0, 280);
	RCSIndicatorsSwitch.AddPosition(1, 320);
	RCSIndicatorsSwitch.AddPosition(2, 340);
	RCSIndicatorsSwitch.AddPosition(3, 20);
	RCSIndicatorsSwitch.AddPosition(4, 40);
	RCSIndicatorsSwitch.AddPosition(5, 70);
	RCSIndicatorsSwitch.Register(PSH, "RCSIndicatorsSwitch", 1);

	LVGuidanceSwitch.Register(PSH, "LVGuidanceSwitch", TOGGLESWITCH_UP, false);
	LVGuidanceSwitch.SetGuardResetsState(false);

	if (!SkylabCM) {
		SIISIVBSepSwitch.Register(PSH, "SIISIVBSepSwitch", false, false);
		TLIEnableSwitch.Register(PSH, "TLIEnableSwitch", true);
	}

	ECSIndicatorsSwitch.AddPosition(0, 340);
	ECSIndicatorsSwitch.AddPosition(1, 20);
	ECSIndicatorsSwitch.Register(PSH, "ECSIndicatorsSwitch", 0);

	CGSwitch.Register(PSH, "CGSwitch", 1);
	ELSLogicSwitch.Register(PSH, "ELSLogicSwitch", 0, 0);
	ELSAutoSwitch.Register(PSH, "ELSAutoSwitch", 1);
	CMRCSLogicSwitch.Register(PSH, "CMRCSLogicSwitch", 0);
	CMPropDumpSwitch.Register(PSH, "CMPropDumpSwitch", 0, 0);
	CMPropPurgeSwitch.Register(PSH, "CMPropPurgeSwitch", 0, 0);

	// FCSMSPSASwitch.Register(PSH, "FCSMSPSASwitch", 0);
	// FCSMSPSBSwitch.Register(PSH, "FCSMSPSBSwitch", 0);
	EventTimerUpDownSwitch.Register(PSH, "EventTimerUpDownSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerContSwitch.Register(PSH, "EventTimerControlSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerMinutesSwitch.Register(PSH, "EventTimerMinutesSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	EventTimerSecondsSwitch.Register(PSH, "EventTimerSecondsSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SaturnEventTimerDisplay.Register(PSH, "SaturnEventTimerDisplay", 0, 0, 0);	// dummy switch/display for checklist controller

	MainReleaseSwitch.Register(PSH, "MainReleaseSwitch", 0, 0, SPRINGLOADEDSWITCH_DOWN);

	PropDumpAutoSwitch.Register(PSH, "PropDumpAutoSwitch", 1);
	TwoEngineOutAutoSwitch.Register(PSH, "TwoEngineOutAutoSwitch", 1);
	LVRateAutoSwitch.Register(PSH, "LVRateAutoSwitch", 1);
	TowerJett1Switch.Register(PSH, "TowerJett1Switch", THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	TowerJett1Switch.SetGuardResetsState(false);
	TowerJett1Switch.SetDelayTime(2);
	TowerJett2Switch.Register(PSH, "TowerJett2Switch", THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	TowerJett2Switch.SetGuardResetsState(false);
	TowerJett2Switch.SetDelayTime(2);

	CmSmSep1Switch.Register(PSH, "CmSmSep1Switch", 0, 0, SPRINGLOADEDSWITCH_DOWN);
	CmSmSep1Switch.SetDelayTime(1);
	CmSmSep2Switch.Register(PSH, "CmSmSep2Switch", 0, 0, SPRINGLOADEDSWITCH_DOWN);
	CmSmSep2Switch.SetDelayTime(1);

	EDSSwitch.Register(PSH, "EDSSwitch", 0, SPRINGLOADEDSWITCH_NONE, "EDS Auto switch");
	CsmLmFinalSep1Switch.Register(PSH, "CsmLmFinalSep1Switch",  THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_DOWN);
	CsmLmFinalSep2Switch.Register(PSH, "CsmLmFinalSep2Switch",  THREEPOSSWITCH_DOWN, false, SPRINGLOADEDSWITCH_DOWN);

	RotPowerNormal1Switch.Register(PSH, "RotPowerNormal1Switch", THREEPOSSWITCH_CENTER);
	RotPowerNormal2Switch.Register(PSH, "RotPowerNormal2Switch", THREEPOSSWITCH_CENTER);
	RotPowerDirect1Switch.Register(PSH, "RotPowerDirect1Switch", THREEPOSSWITCH_CENTER);
	RotPowerDirect2Switch.Register(PSH, "RotPowerDirect2Switch", THREEPOSSWITCH_CENTER);

	dVThrust1Switch.Register(PSH, "dVThrust1Switch", 0, 0);
	dVThrust2Switch.Register(PSH, "dVThrust2Switch", 0, 0);

	SPSswitch.Register(PSH, "SPSswitch", false);

	SPSGimbalPitchThumbwheel.Register(PSH, "SPSGimbalPitchThumbwheel", 8, 16, false, 5);
	SPSGimbalYawThumbwheel.Register(PSH, "SPSGimbalYawThumbwheel", 8, 16, true, 5);

	SPSOxidPercentMeter.Register(PSH, "SPSOxidPercentMeter", 0, 0.999, 1, 0.999);
	SPSFuelPercentMeter.Register(PSH, "SPSFuelPercentMeter", 0, 0.999, 1, 0.999);
	SPSOxidUnbalMeter.Register(PSH, "SPSOxidUnbalMeter", -600, 600, 10);

	SPSTempMeter.Register(PSH, "SPSTempMeter", 0, 200, 2);
	SPSHeliumNitrogenPressMeter.Register(PSH, "SPSHeliumNitrogenPressMeter", 0, 5000, 2);
	SPSFuelPressMeter.Register(PSH, "SPSFuelPressMeter", 0, 250, 2);
	SPSOxidPressMeter.Register(PSH, "SPSOxidPressMeter", 0, 250, 2);

	LVSPSPcMeter.Register(PSH, "LVSPSPcMeter", 0, 150, 2);

	GPFPIPitch1Meter.Register(PSH, "GPFPIPitch1Meter", 0, 92, 2);
	GPFPIPitch2Meter.Register(PSH, "GPFPIPitch2Meter", 0, 92, 2);
	GPFPIYaw1Meter.Register(PSH, "GPFPIYaw1Meter", 0, 92, 2);
	GPFPIYaw2Meter.Register(PSH, "GPFPIYaw2Meter", 0, 92, 2);

	DirectUllageButton.Register(PSH, "DirectUllageButton", false);
	ThrustOnButton.Register(PSH, "ThrustOnButton", false);

	H2Pressure1Meter.Register(PSH, "H2Pressure1Meter", 0, 400, 10);
	H2Pressure2Meter.Register(PSH, "H2Pressure2Meter", 0, 400, 10);
	O2Pressure1Meter.Register(PSH, "O2Pressure1Meter", 100, 1050, 10);
	O2Pressure2Meter.Register(PSH, "O2Pressure2Meter", 100, 1050, 10);
	H2Quantity1Meter.Register(PSH, "H2Quantity1Meter", 0, 1, 10);
	H2Quantity2Meter.Register(PSH, "H2Quantity2Meter", 0, 1, 10);
	O2Quantity1Meter.Register(PSH, "O2Quantity1Meter", 0, 1, 10);
	O2Quantity2Meter.Register(PSH, "O2Quantity2Meter", 0, 1, 10);

	CSMACVoltMeter.Register(PSH, "ACVoltMeter", 85, 145, 3);
	CSMDCVoltMeter.Register(PSH, "DCVoltMeter", 17.5, 47.5, 3);
	SystemTestVoltMeter.Register(PSH, "SystemTestMeter", 0.0, 5.0, 3);
	DCAmpMeter.Register(PSH, "DCAmpMeter", 0, 100, 3);

	FuelCellH2FlowMeter.Register(PSH, "FuelCellH2FlowMeter", 0, 0.2, 2);
	FuelCellO2FlowMeter.Register(PSH, "FuelCellO2FlowMeter", 0, 1.6, 2);
	FuelCellTempMeter.Register(PSH, "FuelCellTempMeter", 100, 550, 2);
	FuelCellCondenserTempMeter.Register(PSH, "FuelCellCondenserTempMeter", 150, 250, 2);

	SuitTempMeter.Register(PSH, "SuitTempMeter", 20, 95, 2);
	CabinTempMeter.Register(PSH, "CabinTempMeter", 40, 120, 2);
	SuitPressMeter.Register(PSH, "SuitPressMeter", 0, 16, 2);
	CabinPressMeter.Register(PSH, "CabinPressMeter", 0, 16, 2);
	PartPressCO2Meter.Register(PSH, "PartPressCO2Meter", 0, 30, 2);

	SuitComprDeltaPMeter.Register(PSH, "SuitComprDeltaPMeter", -0.1, 1.1, 5);
	LeftO2FlowMeter.Register(PSH, "LeftO2FlowMeter", 0.1, 1.1, 5);

	SuitCabinDeltaPMeter.Register(PSH, "SuitCabinDeltaPMeter", -6, 6, 5);
	RightO2FlowMeter.Register(PSH, "RightO2FlowMeter", 0.1, 1.1, 5);

	EcsRadTempInletMeter.Register(PSH, "EcsRadTempInletMeter", 50, 130, 5);
	EcsRadTempPrimOutletMeter.Register(PSH, "EcsRadTempPrimOutletMeter", -60, 110, 5, 50);

	EcsRadTempSecOutletMeter.Register(PSH, "EcsRadTempSecOutletMeter", 26, 74, 5);
	GlyEvapTempOutletMeter.Register(PSH, "GlyEvapTempOutletMeter", 26, 74, 5);

	GlyEvapSteamPressMeter.Register(PSH, "GlyEvapSteamPressMeter", 0.03, 0.27, 5);
	GlycolDischPressMeter.Register(PSH, "GlycolDischPressMeter", -6, 66, 5);

	AccumQuantityMeter.Register(PSH, "AccumQuantityMeter", 0, 1, 5);
	H2oQuantityMeter.Register(PSH, "H2oQuantityMeter", 0, 1, 3);

	EcsRadiatorIndicator.Register(PSH, "EcsRadiatorIndicator", true);

	EcsRadiatorsFlowContAutoSwitch.Register(PSH, "EcsRadiatorsFlowContAutoSwitch", THREEPOSSWITCH_UP);
	EcsRadiatorsFlowContPwrSwitch.Register(PSH, "EcsRadiatorsFlowContPwrSwitch", THREEPOSSWITCH_CENTER);
	EcsRadiatorsManSelSwitch.Register(PSH, "EcsRadiatorsManSelSwitch", THREEPOSSWITCH_UP);
	EcsRadiatorsHeaterPrimSwitch.Register(PSH, "EcsRadiatorsHeaterPrimSwitch", THREEPOSSWITCH_CENTER);
	EcsRadiatorsHeaterSecSwitch.Register(PSH, "EcsRadiatorsHeaterSecSwitch", false);

	PotH2oHtrSwitch.Register(PSH, "PotH2oHtrSwitch", THREEPOSSWITCH_CENTER);
	SuitCircuitH2oAccumAutoSwitch.Register(PSH, "SuitCircuitH2oAccumAutoSwitch", THREEPOSSWITCH_CENTER);
	SuitCircuitH2oAccumAutoSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
											  &SaturnGlycolCoolingController::H2oAccumSwitchesToggled));
	SuitCircuitH2oAccumOnSwitch.Register(PSH, "SuitCircuitH2oAccumOnSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	SuitCircuitH2oAccumOnSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
											&SaturnGlycolCoolingController::H2oAccumSwitchesToggled));
	SuitCircuitHeatExchSwitch.Register(PSH, "SuitCircuitHeatExchSwitch", THREEPOSSWITCH_CENTER);
	SecCoolantLoopEvapSwitch.Register(PSH, "SecCoolantLoopEvapSwitch", THREEPOSSWITCH_CENTER);
	SecCoolantLoopEvapSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
									     &SaturnGlycolCoolingController::SecEvapSwitchesToggled));
	SecCoolantLoopPumpSwitch.Register(PSH, "SecCoolantLoopPumpSwitch", THREEPOSSWITCH_CENTER);
	H2oQtyIndSwitch.Register(PSH, "H2oQtyIndSwitch", TOGGLESWITCH_UP);
	GlycolEvapTempInSwitch.Register(PSH, "GlycolEvapTempInSwitch", false);
	GlycolEvapTempInSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
		                               &SaturnGlycolCoolingController::GlycolEvapTempInSwitchToggled));
	GlycolEvapSteamPressAutoManSwitch.Register(PSH, "GlycolEvapSteamPressAutoManSwitch", false);
	GlycolEvapSteamPressAutoManSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
												  &SaturnGlycolCoolingController::PrimEvapSwitchesToggled));
	GlycolEvapSteamPressIncrDecrSwitch.Register(PSH, "GlycolEvapSteamPressIncrDecrSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	GlycolEvapH2oFlowSwitch.Register(PSH, "GlycolEvapH2oFlowSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);
	GlycolEvapH2oFlowSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
									    &SaturnGlycolCoolingController::PrimEvapSwitchesToggled));
	CabinTempAutoManSwitch.Register(PSH, "CabinTempAutoManSwitch", false);
	CabinTempAutoManSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
									   &SaturnGlycolCoolingController::CabinTempSwitchToggled));

	CabinTempAutoControlSwitch.Register(PSH, "CabinTempAutoControlSwitch", 0, 9);
	CabinTempAutoControlSwitch.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
									       &SaturnGlycolCoolingController::CabinTempSwitchToggled));

	SquelchAThumbwheel.Register(PSH, "SquelchAThumbwheel", 2, 9);
	SquelchBThumbwheel.Register(PSH, "SquelchBThumbwheel", 2, 9);
	
	SPSTestSwitch.Register(PSH, "SPSTestSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);

	EcsGlycolPumpsSwitch.AddPosition(0, 240);
	EcsGlycolPumpsSwitch.AddPosition(1, 270);
	EcsGlycolPumpsSwitch.AddPosition(2, 300);
	EcsGlycolPumpsSwitch.AddPosition(3, 330);
	EcsGlycolPumpsSwitch.AddPosition(4,   0);
	EcsGlycolPumpsSwitch.Register(PSH, "EcsGlycolPumpsSwitch", 2);

	RightIntegralRotarySwitch.AddPosition(0,  210);
	RightIntegralRotarySwitch.AddPosition(1,  240);
	RightIntegralRotarySwitch.AddPosition(2,  270);
	RightIntegralRotarySwitch.AddPosition(3,  300);
	RightIntegralRotarySwitch.AddPosition(4,  330);
	RightIntegralRotarySwitch.AddPosition(5,    0);
	RightIntegralRotarySwitch.AddPosition(6,   30);
	RightIntegralRotarySwitch.AddPosition(7,   60);
	RightIntegralRotarySwitch.AddPosition(8,   90);
	RightIntegralRotarySwitch.AddPosition(9,  120);
	RightIntegralRotarySwitch.AddPosition(10, 150);
	RightIntegralRotarySwitch.Register(PSH, "RightIntegralRotarySwitch", 0);

	RightFloodRotarySwitch.AddPosition(0,  210);
	RightFloodRotarySwitch.AddPosition(1,  240);
	RightFloodRotarySwitch.AddPosition(2,  270);
	RightFloodRotarySwitch.AddPosition(3,  300);
	RightFloodRotarySwitch.AddPosition(4,  330);
	RightFloodRotarySwitch.AddPosition(5,    0);
	RightFloodRotarySwitch.AddPosition(6,   30);
	RightFloodRotarySwitch.AddPosition(7,   60);
	RightFloodRotarySwitch.AddPosition(8,   90);
	RightFloodRotarySwitch.AddPosition(9,  120);
	RightFloodRotarySwitch.AddPosition(10, 150);
	RightFloodRotarySwitch.Register(PSH, "RightFloodRotarySwitch", 0);

	HighGainAntennaPitchPositionSwitch.AddPosition(0,   0);
	HighGainAntennaPitchPositionSwitch.AddPosition(1,  15);
	HighGainAntennaPitchPositionSwitch.AddPosition(2,  30);
	HighGainAntennaPitchPositionSwitch.AddPosition(3,  45);
	HighGainAntennaPitchPositionSwitch.AddPosition(4,  60);
	HighGainAntennaPitchPositionSwitch.AddPosition(5,  75);
	HighGainAntennaPitchPositionSwitch.AddPosition(6,  90);
	HighGainAntennaPitchPositionSwitch.AddPosition(7,  105);
	HighGainAntennaPitchPositionSwitch.AddPosition(8,  120);
	HighGainAntennaPitchPositionSwitch.AddPosition(9,  135);
	HighGainAntennaPitchPositionSwitch.AddPosition(10, 150);
	HighGainAntennaPitchPositionSwitch.AddPosition(11, 165);
	HighGainAntennaPitchPositionSwitch.AddPosition(12, 180);
	HighGainAntennaPitchPositionSwitch.Register(PSH, "HighGainAntennaPitchPositionSwitch", 3);

	HighGainAntennaYawPositionSwitch.AddPosition(0,    0);
	HighGainAntennaYawPositionSwitch.AddPosition(1,   15);
	HighGainAntennaYawPositionSwitch.AddPosition(2,   30);
	HighGainAntennaYawPositionSwitch.AddPosition(3,   45);
	HighGainAntennaYawPositionSwitch.AddPosition(4,   60);
	HighGainAntennaYawPositionSwitch.AddPosition(5,   75);
	HighGainAntennaYawPositionSwitch.AddPosition(6,   90);
	HighGainAntennaYawPositionSwitch.AddPosition(7,  105);
	HighGainAntennaYawPositionSwitch.AddPosition(8,  120);
	HighGainAntennaYawPositionSwitch.AddPosition(9,  135);
	HighGainAntennaYawPositionSwitch.AddPosition(10, 150);
	HighGainAntennaYawPositionSwitch.AddPosition(11, 165);
	HighGainAntennaYawPositionSwitch.AddPosition(12, 180);
	HighGainAntennaYawPositionSwitch.AddPosition(13, 195);
	HighGainAntennaYawPositionSwitch.AddPosition(14, 210);
	HighGainAntennaYawPositionSwitch.AddPosition(15, 225);
	HighGainAntennaYawPositionSwitch.AddPosition(16, 240);
	HighGainAntennaYawPositionSwitch.AddPosition(17, 255);
	HighGainAntennaYawPositionSwitch.AddPosition(18, 270);
	HighGainAntennaYawPositionSwitch.AddPosition(19, 285);
	HighGainAntennaYawPositionSwitch.AddPosition(20, 300);
	HighGainAntennaYawPositionSwitch.AddPosition(21, 315);
	HighGainAntennaYawPositionSwitch.AddPosition(22, 330);
	HighGainAntennaYawPositionSwitch.AddPosition(23, 345);
	HighGainAntennaYawPositionSwitch.Register(PSH, "HighGainAntennaYawPositionSwitch", 6);
	HighGainAntennaYawPositionSwitch.SetWraparound(true);

	HighGainAntennaPitchMeter.Register(PSH, "HighGainAntennaPitchMeter", -90, 90, 5, 90);
	HighGainAntennaStrengthMeter.Register(PSH, "HighGainAntennaStrengthMeter", 0, 100, 5);
	HighGainAntennaYawMeter.Register(PSH, "HighGainAntennaYawMeter", 0, 360, 5, 0);

	EMSFunctionSwitch.AddPosition(0,  180);
	EMSFunctionSwitch.AddPosition(1,  210);
	EMSFunctionSwitch.AddPosition(2,  240);
	EMSFunctionSwitch.AddPosition(3,  270);
	EMSFunctionSwitch.AddPosition(4,  300);
	EMSFunctionSwitch.AddPosition(5,  330);
	EMSFunctionSwitch.AddPosition(6,    0);
	EMSFunctionSwitch.AddPosition(7,   30);
	EMSFunctionSwitch.AddPosition(8,   60);
	EMSFunctionSwitch.AddPosition(9,   90);
	EMSFunctionSwitch.AddPosition(10, 120);
	EMSFunctionSwitch.AddPosition(11, 150);
	EMSFunctionSwitch.Register(PSH, "EMSFunctionSwitch", 0);
	EMSFunctionSwitch.SetWraparound(true);

	EMSDvDisplay.Register(PSH, "EMSDvDisplay", -1000, 14000, 1, 0);
	EMSScrollDisplay.Register(PSH, "EMSScrollDisplay",0, 0, 0, 0);	// dummy switch/display for checklist controller

	VHFAntennaRotarySwitch.AddPosition(0, 300);
	VHFAntennaRotarySwitch.AddPosition(1,   0);
	VHFAntennaRotarySwitch.AddPosition(2,  60);
	VHFAntennaRotarySwitch.Register(PSH, "VHFAntennaRotarySwitch", 1);

	DCIndicatorsRotary.AddPosition(0, 210);
	DCIndicatorsRotary.AddPosition(1, 240);
	DCIndicatorsRotary.AddPosition(2, 270);
	DCIndicatorsRotary.AddPosition(3, 300);
	DCIndicatorsRotary.AddPosition(4, 330);
	DCIndicatorsRotary.AddPosition(5,   0);
	DCIndicatorsRotary.AddPosition(6,  30);
	DCIndicatorsRotary.AddPosition(7,  60);
	DCIndicatorsRotary.AddPosition(8,  90);
	DCIndicatorsRotary.AddPosition(9, 120);
	DCIndicatorsRotary.AddPosition(10,150);
	DCIndicatorsRotary.Register(PSH, "DCIndicatorsRotary", 3);

	DCIndicatorsRotary.SetSource(0, FuelCells[0]);
	DCIndicatorsRotary.SetSource(1, FuelCells[1]);
	DCIndicatorsRotary.SetSource(2, FuelCells[2]);
	DCIndicatorsRotary.SetSource(3, MainBusA);
	DCIndicatorsRotary.SetSource(4, MainBusB);
	DCIndicatorsRotary.SetSource(5, &BatteryBusA);
	DCIndicatorsRotary.SetSource(6, &BatteryBusB);
	DCIndicatorsRotary.SetSource(7, &BatteryCharger);	
	DCIndicatorsRotary.SetSource(8, EntryBatteryC);
	DCIndicatorsRotary.SetSource(9, &PyroBusAFeeder);
	DCIndicatorsRotary.SetSource(10, &PyroBusBFeeder);

	ACIndicatorRotary.AddPosition(0, 290);
	ACIndicatorRotary.AddPosition(1, 315);
	ACIndicatorRotary.AddPosition(2, 340);
	ACIndicatorRotary.AddPosition(3, 20);
	ACIndicatorRotary.AddPosition(4, 45);
	ACIndicatorRotary.AddPosition(5, 70);
	ACIndicatorRotary.Register(PSH, "ACIndicatorRotary", 5);

	ACIndicatorRotary.SetSource(0, &ACBus1PhaseA);
	ACIndicatorRotary.SetSource(1, &ACBus1PhaseB);
	ACIndicatorRotary.SetSource(2, &ACBus1PhaseC);
	ACIndicatorRotary.SetSource(3, &ACBus2PhaseA);
	ACIndicatorRotary.SetSource(4, &ACBus2PhaseB);
	ACIndicatorRotary.SetSource(5, &ACBus2PhaseC);

	BatteryChargeRotary.AddPosition(0, 300);
	BatteryChargeRotary.AddPosition(1, 330);
	BatteryChargeRotary.AddPosition(2,   0);
	BatteryChargeRotary.AddPosition(3,  30);
	BatteryChargeRotary.Register(PSH, "BatteryChargeRotary", 0);
	
	DockingProbeExtdRelSwitch.Register(PSH, "DockingProbeExtdRelSwitch", THREEPOSSWITCH_CENTER, false, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	DockingProbeExtdRelSwitch.SetDelayTime(5);
	DockingProbeExtdRelSwitch.SetGuardResetsState(false);
	DockingProbeRetractPrimSwitch.Register(PSH, "DockingProbeRetractPrimSwitch", THREEPOSSWITCH_CENTER);
	DockingProbeRetractSecSwitch.Register(PSH, "DockingProbeRetractSecSwitch", THREEPOSSWITCH_CENTER);
	DockingProbeAIndicator.Register(PSH, "DockingProbeAIndicator", false);
	DockingProbeBIndicator.Register(PSH, "DockingProbeBIndicator", false);

	NumericRotarySwitch.AddPosition(0,  210);
	NumericRotarySwitch.AddPosition(1,  240);
	NumericRotarySwitch.AddPosition(2,  270);
	NumericRotarySwitch.AddPosition(3,  300);
	NumericRotarySwitch.AddPosition(4,  330);
	NumericRotarySwitch.AddPosition(5,    0);
	NumericRotarySwitch.AddPosition(6,   30);
	NumericRotarySwitch.AddPosition(7,   60);
	NumericRotarySwitch.AddPosition(8,   90);
	NumericRotarySwitch.AddPosition(9,  120);
	NumericRotarySwitch.AddPosition(10, 150);
	NumericRotarySwitch.Register(PSH, "NumericRotarySwitch", 4);

	FloodRotarySwitch.AddPosition(0,  210);
	FloodRotarySwitch.AddPosition(1,  240);
	FloodRotarySwitch.AddPosition(2,  270);
	FloodRotarySwitch.AddPosition(3,  300);
	FloodRotarySwitch.AddPosition(4,  330);
	FloodRotarySwitch.AddPosition(5,    0);
	FloodRotarySwitch.AddPosition(6,   30);
	FloodRotarySwitch.AddPosition(7,   60);
	FloodRotarySwitch.AddPosition(8,   90);
	FloodRotarySwitch.AddPosition(9,  120);
	FloodRotarySwitch.AddPosition(10, 150);
	FloodRotarySwitch.Register(PSH, "FloodRotarySwitch", 4);

	IntegralRotarySwitch.AddPosition(0,  210);
	IntegralRotarySwitch.AddPosition(1,  240);
	IntegralRotarySwitch.AddPosition(2,  270);
	IntegralRotarySwitch.AddPosition(3,  300);
	IntegralRotarySwitch.AddPosition(4,  330);
	IntegralRotarySwitch.AddPosition(5,    0);
	IntegralRotarySwitch.AddPosition(6,   30);
	IntegralRotarySwitch.AddPosition(7,   60);
	IntegralRotarySwitch.AddPosition(8,   90);
	IntegralRotarySwitch.AddPosition(9,  120);
	IntegralRotarySwitch.AddPosition(10, 150);
	IntegralRotarySwitch.Register(PSH, "IntegralRotarySwitch", 4);

	FDAIPowerRotarySwitch.AddPosition(0,  0);
	FDAIPowerRotarySwitch.AddPosition(1, 30);
	FDAIPowerRotarySwitch.AddPosition(2, 60);
	FDAIPowerRotarySwitch.AddPosition(3, 90);
	FDAIPowerRotarySwitch.Register(PSH, "FDAIPowerRotarySwitch", 0);

	SCSElectronicsPowerRotarySwitch.AddPosition(0, 30);
	SCSElectronicsPowerRotarySwitch.AddPosition(1, 60);
	SCSElectronicsPowerRotarySwitch.AddPosition(2, 90);
	SCSElectronicsPowerRotarySwitch.Register(PSH, "SCSElectronicsPowerRotarySwitch", 0);

	BMAGPowerRotary1Switch.AddPosition(0, 30);
	BMAGPowerRotary1Switch.AddPosition(1, 60);
	BMAGPowerRotary1Switch.AddPosition(2, 90);
	BMAGPowerRotary1Switch.Register(PSH, "BMAGPowerRotary1Switch", 1);

	BMAGPowerRotary2Switch.AddPosition(0, 30);
	BMAGPowerRotary2Switch.AddPosition(1, 60);
	BMAGPowerRotary2Switch.AddPosition(2, 90);
	BMAGPowerRotary2Switch.Register(PSH, "BMAGPowerRotary2Switch", 1);

	DirectO2RotarySwitch.AddPosition(0, 330);
	DirectO2RotarySwitch.AddPosition(1,   0);
	DirectO2RotarySwitch.AddPosition(2,  30);
	DirectO2RotarySwitch.AddPosition(3,  60);
	DirectO2RotarySwitch.AddPosition(4,  90);
	DirectO2RotarySwitch.AddPosition(5, 120);
	DirectO2RotarySwitch.AddPosition(6, 150);
	DirectO2RotarySwitch.Register(PSH, "DirectO2RotarySwitch", 6);

	GlycolReservoirInletRotary.AddPosition(0,  90);
	GlycolReservoirInletRotary.AddPosition(1, 180);
	GlycolReservoirInletRotary.Register(PSH, "GlycolReservoirInletRotary", 0);

	GlycolReservoirBypassRotary.AddPosition(0,  90);
	GlycolReservoirBypassRotary.AddPosition(1, 180);
	GlycolReservoirBypassRotary.Register(PSH, "GlycolReservoirBypassRotary", 1);
	
	GlycolReservoirOutletRotary.AddPosition(0,  90);
	GlycolReservoirOutletRotary.AddPosition(1, 180);
	GlycolReservoirOutletRotary.Register(PSH, "GlycolReservoirOutletRotary", 0);

	OxygenSurgeTankRotary.AddPosition(0,  0);
	OxygenSurgeTankRotary.AddPosition(1, 90);
	OxygenSurgeTankRotary.Register(PSH, "OxygenSurgeTankRotary", 1);

	OxygenSMSupplyRotary.AddPosition(0,  0);
	OxygenSMSupplyRotary.AddPosition(1, 90);
	OxygenSMSupplyRotary.Register(PSH, "OxygenSMSupplyRotary", 1);
	
	OxygenRepressPackageRotary.AddPosition(0, 330);
	OxygenRepressPackageRotary.AddPosition(1,  60);
	OxygenRepressPackageRotary.AddPosition(2, 120);
	OxygenRepressPackageRotary.Register(PSH, "OxygenRepressPackageRotary", 1);

	ORDEALAltSetRotary.AddPosition(0, 240);
	ORDEALAltSetRotary.AddPosition(1, 270);
	ORDEALAltSetRotary.AddPosition(2, 330);
	ORDEALAltSetRotary.AddPosition(3,   0);
	ORDEALAltSetRotary.AddPosition(4,  60);
	ORDEALAltSetRotary.AddPosition(5,  90);
	ORDEALAltSetRotary.AddPosition(6, 150);
	ORDEALAltSetRotary.Register(PSH, "ORDEALAltSetRotary", 2);

	LeftSystemTestRotarySwitch.AddPosition(0,  240);
	LeftSystemTestRotarySwitch.AddPosition(1,  270);
	LeftSystemTestRotarySwitch.AddPosition(2,  300);
	LeftSystemTestRotarySwitch.AddPosition(3,  330);
	LeftSystemTestRotarySwitch.AddPosition(4,    0);
	LeftSystemTestRotarySwitch.AddPosition(5,   30);
	LeftSystemTestRotarySwitch.AddPosition(6,   60);
	LeftSystemTestRotarySwitch.AddPosition(7,   90);
	LeftSystemTestRotarySwitch.AddPosition(8,  120);
	LeftSystemTestRotarySwitch.Register(PSH, "LeftSystemTestRotarySwitch", 4);
	
	RightSystemTestRotarySwitch.AddPosition(0,  315);
	RightSystemTestRotarySwitch.AddPosition(1,  340);
	RightSystemTestRotarySwitch.AddPosition(2,   20);
	RightSystemTestRotarySwitch.AddPosition(3,   45);
	RightSystemTestRotarySwitch.Register(PSH, "RightSystemTestRotarySwitch", 1);
	
	Panel100NumericRotarySwitch.AddPosition(0,  210);
	Panel100NumericRotarySwitch.AddPosition(1,  240);
	Panel100NumericRotarySwitch.AddPosition(2,  270);
	Panel100NumericRotarySwitch.AddPosition(3,  300);
	Panel100NumericRotarySwitch.AddPosition(4,  330);
	Panel100NumericRotarySwitch.AddPosition(5,    0);
	Panel100NumericRotarySwitch.AddPosition(6,   30);
	Panel100NumericRotarySwitch.AddPosition(7,   60);
	Panel100NumericRotarySwitch.AddPosition(8,   90);
	Panel100NumericRotarySwitch.AddPosition(9,  120);
	Panel100NumericRotarySwitch.AddPosition(10, 150);
	Panel100NumericRotarySwitch.Register(PSH, "Panel100NumericRotarySwitch", 4);

	Panel100FloodRotarySwitch.AddPosition(0,  210);
	Panel100FloodRotarySwitch.AddPosition(1,  240);
	Panel100FloodRotarySwitch.AddPosition(2,  270);
	Panel100FloodRotarySwitch.AddPosition(3,  300);
	Panel100FloodRotarySwitch.AddPosition(4,  330);
	Panel100FloodRotarySwitch.AddPosition(5,    0);
	Panel100FloodRotarySwitch.AddPosition(6,   30);
	Panel100FloodRotarySwitch.AddPosition(7,   60);
	Panel100FloodRotarySwitch.AddPosition(8,   90);
	Panel100FloodRotarySwitch.AddPosition(9,  120);
	Panel100FloodRotarySwitch.AddPosition(10, 150);
	Panel100FloodRotarySwitch.Register(PSH, "Panel100FloodRotarySwitch", 0);

	Panel100IntegralRotarySwitch.AddPosition(0,  210);
	Panel100IntegralRotarySwitch.AddPosition(1,  240);
	Panel100IntegralRotarySwitch.AddPosition(2,  270);
	Panel100IntegralRotarySwitch.AddPosition(3,  300);
	Panel100IntegralRotarySwitch.AddPosition(4,  330);
	Panel100IntegralRotarySwitch.AddPosition(5,    0);
	Panel100IntegralRotarySwitch.AddPosition(6,   30);
	Panel100IntegralRotarySwitch.AddPosition(7,   60);
	Panel100IntegralRotarySwitch.AddPosition(8,   90);
	Panel100IntegralRotarySwitch.AddPosition(9,  120);
	Panel100IntegralRotarySwitch.AddPosition(10, 150);
	Panel100IntegralRotarySwitch.Register(PSH, "Panel100IntegralRotarySwitch", 4);

	OxygenSurgeTankValveRotary.AddPosition(0,  330);
	OxygenSurgeTankValveRotary.AddPosition(1,    0);
	OxygenSurgeTankValveRotary.AddPosition(2,   30);
	OxygenSurgeTankValveRotary.AddPosition(3,   60);
	OxygenSurgeTankValveRotary.AddPosition(4,   90);
	OxygenSurgeTankValveRotary.AddPosition(5,  120);
	OxygenSurgeTankValveRotary.AddPosition(6,  150);
	OxygenSurgeTankValveRotary.AddPosition(7,  180);
	OxygenSurgeTankValveRotary.AddPosition(8,  210);
	OxygenSurgeTankValveRotary.Register(PSH, "OxygenSurgeTankValveRotary", 8);

	GlycolToRadiatorsRotary.AddPosition(0,  0);
	GlycolToRadiatorsRotary.AddPosition(1, 90);
	GlycolToRadiatorsRotary.Register(PSH, "GlycolToRadiatorsRotary", 0);

	GlycolRotary.AddPosition(0, 90);
	GlycolRotary.AddPosition(1,180);
	GlycolRotary.Register(PSH, "GlycolRotary", 0);
	
	AccumRotary.AddPosition(0, 90);
	AccumRotary.AddPosition(1,180);
	AccumRotary.Register(PSH, "AccumRotary", 1);

	PLVCSwitch.Register(PSH, "PLVCSwitch", TOGGLESWITCH_UP); 
	PLVCSwitch.SetSideways(1);

	PressureReliefRotary.AddPosition(0,   0);
	PressureReliefRotary.AddPosition(1,  90);
	PressureReliefRotary.AddPosition(2, 180);
	PressureReliefRotary.AddPosition(3, 270);
	PressureReliefRotary.Register(PSH, "PressureReliefRotary", 1);
	PressureReliefRotary.SetWraparound(true);

	WasteTankInletRotary.AddPosition(0,  0);
	WasteTankInletRotary.AddPosition(1, 90);
	WasteTankInletRotary.Register(PSH, "WasteTankInletRotary", 0);

	PotableTankInletRotary.AddPosition(0,  0);
	PotableTankInletRotary.AddPosition(1, 90);
	PotableTankInletRotary.Register(PSH, "PotableTankInletRotary", 1);

	WasteTankServicingRotary.AddPosition(0,  0);
	WasteTankServicingRotary.AddPosition(1, 90);
	WasteTankServicingRotary.Register(PSH, "WasteTankServicingRotary", 1);

	CabinRepressValveRotary.AddPosition(0,  90);
	CabinRepressValveRotary.AddPosition(1, 120);
	CabinRepressValveRotary.AddPosition(2, 150);
	CabinRepressValveRotary.AddPosition(3, 180);
	CabinRepressValveRotary.AddPosition(4, 210);
	CabinRepressValveRotary.AddPosition(5, 240);
	CabinRepressValveRotary.AddPosition(6, 270);
	CabinRepressValveRotary.Register(PSH, "CabinRepressValveRotary", 0);

	SelectorInletValveRotary.AddPosition(0,   0);
	SelectorInletValveRotary.AddPosition(1,  90);
	SelectorInletValveRotary.AddPosition(2, 180);
	SelectorInletValveRotary.AddPosition(3, 270);
	SelectorInletValveRotary.Register(PSH, "SelectorInletValveRotary", 3);
	SelectorInletValveRotary.SetWraparound(true);

	SelectorOutletValveRotary.AddPosition(0,   0);
	SelectorOutletValveRotary.AddPosition(1,  90);
	SelectorOutletValveRotary.AddPosition(2, 180);
	SelectorOutletValveRotary.AddPosition(3, 270);
	SelectorOutletValveRotary.Register(PSH, "SelectorOutletValveRotary", 3);
	SelectorOutletValveRotary.SetWraparound(true);

	EmergencyCabinPressureRotary.AddPosition(0,   0);
	EmergencyCabinPressureRotary.AddPosition(1,  90);
	EmergencyCabinPressureRotary.AddPosition(2, 180);
	EmergencyCabinPressureRotary.AddPosition(3, 270);
	EmergencyCabinPressureRotary.Register(PSH, "EmergencyCabinPressureRotary", 3);
	EmergencyCabinPressureRotary.SetWraparound(true);

	O2MainRegulatorASwitch.Register(PSH, "O2MainRegulatorASwitch", 0);
	O2MainRegulatorBSwitch.Register(PSH, "O2MainRegulatorBSwitch", 0);

	EmergencyCabinPressureTestSwitch.Register(PSH, "EmergencyCabinPressureTestSwitch", 0);

	EvapWaterControlPrimaryRotary.AddPosition(0,  0);
	EvapWaterControlPrimaryRotary.AddPosition(1, 90);
	EvapWaterControlPrimaryRotary.Register(PSH, "EvapWaterControlPrimaryRotary", 1);
	EvapWaterControlPrimaryRotary.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
									          &SaturnGlycolCoolingController::PrimEvapSwitchesToggled));
	EvapWaterControlSecondaryRotary.AddPosition(0,  0);
	EvapWaterControlSecondaryRotary.AddPosition(1, 90);
	EvapWaterControlSecondaryRotary.Register(PSH, "EvapWaterControlSecondaryRotary", 1);
	EvapWaterControlSecondaryRotary.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
									            &SaturnGlycolCoolingController::SecEvapSwitchesToggled));

	WaterAccumulator1Rotary.AddPosition(0, 200);
	WaterAccumulator1Rotary.AddPosition(1, 240);
	WaterAccumulator1Rotary.AddPosition(2, 290);
	WaterAccumulator1Rotary.Register(PSH, "WaterAccumulator1Rotary", 0);
	WaterAccumulator1Rotary.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
										&SaturnGlycolCoolingController::H2oAccumSwitchesToggled));

	WaterAccumulator2Rotary.AddPosition(0, 200);
	WaterAccumulator2Rotary.AddPosition(1, 240);
	WaterAccumulator2Rotary.AddPosition(2, 290);
	WaterAccumulator2Rotary.Register(PSH, "WaterAccumulator2Rotary", 0);
	WaterAccumulator2Rotary.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
										&SaturnGlycolCoolingController::H2oAccumSwitchesToggled));

	PrimaryGlycolEvapInletTempRotary.AddPosition(0,  90);
	PrimaryGlycolEvapInletTempRotary.AddPosition(1, 120);
	PrimaryGlycolEvapInletTempRotary.AddPosition(2, 150);
	PrimaryGlycolEvapInletTempRotary.AddPosition(3, 180);
	PrimaryGlycolEvapInletTempRotary.Register(PSH, "PrimaryGlycolEvapInletTempRotary", 0);
	PrimaryGlycolEvapInletTempRotary.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
												 &SaturnGlycolCoolingController::PrimaryGlycolEvapInletTempRotaryToggled));

	SuitFlowReliefRotary.AddPosition(0,  90);
	SuitFlowReliefRotary.AddPosition(1, 270);
	SuitFlowReliefRotary.Register(PSH, "SuitFlowReliefRotary", 1);

	SuitHeatExchangerPrimaryGlycolRotary.AddPosition(0,   0);
	SuitHeatExchangerPrimaryGlycolRotary.AddPosition(1, 180);
	SuitHeatExchangerPrimaryGlycolRotary.Register(PSH, "SuitHeatExchangerPrimaryGlycolRotary", 0);

	SuitHeatExchangerSecondaryGlycolRotary.AddPosition(0,  0);
	SuitHeatExchangerSecondaryGlycolRotary.AddPosition(1, 90);
	SuitHeatExchangerSecondaryGlycolRotary.Register(PSH, "SuitHeatExchangerSecondaryGlycolRotary", 0);

	Panel382Cover.Register(PSH, "Panel382Cover",0, 0, 0, 0);	// dummy switch/display for checklist controller

	SecondaryCabinTempValve.AddPosition(0,  0);
	SecondaryCabinTempValve.AddPosition(1,  60);
	SecondaryCabinTempValve.AddPosition(2,  90);
	SecondaryCabinTempValve.AddPosition(3, 150);
	SecondaryCabinTempValve.AddPosition(4, 180);
	SecondaryCabinTempValve.Register(PSH, "SecondaryCabinTempValve", 4);
	SecondaryCabinTempValve.SetCallback(new PanelSwitchCallback<SaturnGlycolCoolingController>(&GlycolCoolingController, 
										&SaturnGlycolCoolingController::CabinTempSwitchToggled));

	FoodPreparationWaterHotLever.Register(PSH, "FoodPreparationWaterHotLever", 0, SPRINGLOADEDSWITCH_DOWN);
	FoodPreparationWaterHotLever.SetCallback(new PanelSwitchCallback<SaturnWaterController>(&WaterController, &SaturnWaterController::FoodPreparationWaterSwitchToggled));
	FoodPreparationWaterColdLever.Register(PSH, "FoodPreparationWaterColdLever", 0, SPRINGLOADEDSWITCH_DOWN);
	FoodPreparationWaterColdLever.SetCallback(new PanelSwitchCallback<SaturnWaterController>(&WaterController, &SaturnWaterController::FoodPreparationWaterSwitchToggled));

	EventTimerUpDown306Switch.Register(PSH, "EventTimerUpDown306Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerUpDown306Switch.SetSideways(1);
	EventTimerControl306Switch.Register(PSH, "EventTimerControl306Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerControl306Switch.SetSideways(1);
	EventTimer306MinutesSwitch.Register(PSH, "EventTimer306MinutesSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	EventTimer306MinutesSwitch.SetSideways(1);
	EventTimer306SecondsSwitch.Register(PSH, "EventTimer306SecondsSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	EventTimer306SecondsSwitch.SetSideways(1);
	SaturnEventTimer306Display.Register(PSH, "SaturnEventTimer306Display", 0, 0, 0);	// dummy switch/display for checklist controller
	MissionTimer306HoursSwitch.Register(PSH, "MissionTimer306HoursSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	MissionTimer306HoursSwitch.SetSideways(1);
	MissionTimer306MinutesSwitch.Register(PSH, "MissionTimer306MinutesSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	MissionTimer306MinutesSwitch.SetSideways(1);
	MissionTimer306SecondsSwitch.Register(PSH, "MissionTimer306SecondsSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	MissionTimer306SecondsSwitch.SetSideways(1);
	
	MissionTimer306Switch.Register(PSH, "MissionTimer306Switch", THREEPOSSWITCH_UP, SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN); // Default state UP is correct!
	MissionTimer306Switch.SetSideways(1);

	LMTunnelVentValve.AddPosition(0, 300);
	LMTunnelVentValve.AddPosition(1, 330);
	LMTunnelVentValve.AddPosition(2,  30);
	LMTunnelVentValve.AddPosition(3,  60);
	LMTunnelVentValve.Register(PSH, "LMTunnelVentValve", 0);

	LMDPGauge.Register(PSH, "LMDPGauge", -1, 4, 5);

	PressEqualValve.AddPosition(0, 120);
	PressEqualValve.AddPosition(1, 150);
	PressEqualValve.AddPosition(2, 180);
	PressEqualValve.AddPosition(3, 210);
	PressEqualValve.Register(PSH, "PressEqualValve", 3);

	WasteMGMTOvbdDrainDumpRotary.AddPosition(0,   0);
	WasteMGMTOvbdDrainDumpRotary.AddPosition(1,  90);
	WasteMGMTOvbdDrainDumpRotary.AddPosition(2, 180);
	WasteMGMTOvbdDrainDumpRotary.AddPosition(3, 270);
	WasteMGMTOvbdDrainDumpRotary.Register(PSH, "WasteMGMTOvbdDrainDumpRotary", 1);

	WasteMGMTBatteryVentRotary.AddPosition(0,   0);
	WasteMGMTBatteryVentRotary.AddPosition(1, 180);
	WasteMGMTBatteryVentRotary.AddPosition(2, 270);
	WasteMGMTBatteryVentRotary.Register(PSH, "WasteMGMTBatteryVentRotary", 1);
	WasteMGMTBatteryVentRotary.SetWraparound(true);

	WasteMGMTStoageVentRotary.AddPosition(0,   0);
	WasteMGMTStoageVentRotary.AddPosition(1, 180);
	WasteMGMTStoageVentRotary.AddPosition(2, 270);
	WasteMGMTStoageVentRotary.Register(PSH, "WasteMGMTStoageVentRotary", 1);
	WasteMGMTStoageVentRotary.SetWraparound(true);

	WasteDisposalSwitch.Register(PSH, "WasteDisposalSwitch", THREEPOSSWITCH_UP);
		
	OrbiterAttitudeToggle.SetActive(false);

	EpsSensorSignalDcMnaCircuitBraker.Register(PSH, "EpsSensorSignalDcMnaCircuitBraker", 1);
	EpsSensorSignalDcMnbCircuitBraker.Register(PSH, "EpsSensorSignalDcMnbCircuitBraker", 1);

	EpsSensorSignalAc1CircuitBraker.Register(PSH, "EpsSensorSignalAc1CircuitBraker", 1);
	EpsSensorSignalAc2CircuitBraker.Register(PSH, "EpsSensorSignalAc2CircuitBraker", 1);

	CWMnaCircuitBraker.Register(PSH, "CWMnaCircuitBraker", 1);
	CWMnbCircuitBraker.Register(PSH, "CWMnbCircuitBraker", 1);

	MnbLMPWR1CircuitBraker.Register(PSH, "MnbLMPWR1CircuitBraker", 1);
	MnbLMPWR2CircuitBraker.Register(PSH, "MnbLMPWR2CircuitBraker", 1);

	InverterControl1CircuitBraker.Register(PSH, "InverterControl1CircuitBraker", 1);
	InverterControl2CircuitBraker.Register(PSH, "InverterControl2CircuitBraker", 1);
	InverterControl3CircuitBraker.Register(PSH, "InverterControl3CircuitBraker", 1);

	EPSSensorUnitDcBusACircuitBraker.Register(PSH, "EPSSensorUnitDcBusACircuitBraker", 1);
	EPSSensorUnitDcBusBCircuitBraker.Register(PSH, "EPSSensorUnitDcBusBCircuitBraker", 1);
	EPSSensorUnitAcBus1CircuitBraker.Register(PSH, "EPSSensorUnitAcBus1CircuitBraker", 1);
	EPSSensorUnitAcBus2CircuitBraker.Register(PSH, "EPSSensorUnitAcBus2CircuitBraker", 1);

	BATRLYBusBatACircuitBraker.Register(PSH, "BATRLYBusBatACircuitBraker", 1);
	BATRLYBusBatBCircuitBraker.Register(PSH, "BATRLYBusBatBCircuitBraker", 1);

	ControllerAc1CircuitBraker.Register(PSH, "ControllerAc1CircuitBraker", 1);
	ControllerAc2CircuitBraker.Register(PSH, "ControllerAc2CircuitBraker", 1);
	CONTHTRSMnACircuitBraker.Register(PSH, "CONTHTRSMnACircuitBraker", 1);
	CONTHTRSMnBCircuitBraker.Register(PSH, "CONTHTRSMnBCircuitBraker", 1);
	HTRSOVLDBatACircuitBraker.Register(PSH, "HTRSOVLDBatACircuitBraker", 1);
	HTRSOVLDBatBCircuitBraker.Register(PSH, "HTRSOVLDBatBCircuitBraker", 1);

	BatteryChargerBatACircuitBraker.Register(PSH, "BatteryChargerBatACircuitBraker", 1);
	BatteryChargerBatBCircuitBraker.Register(PSH, "BatteryChargerBatBCircuitBraker", 1);
	BatteryChargerMnACircuitBraker.Register(PSH, "BatteryChargerMnACircuitBraker", 1);
	BatteryChargerMnBCircuitBraker.Register(PSH, "BatteryChargerMnBCircuitBraker", 1);
	BatteryChargerAcPwrCircuitBraker.Register(PSH, "BatteryChargerAcPWRCircuitBraker", 1);

	InstrumentLightingESSMnACircuitBraker.Register(PSH, "InstrumentLightingESSMnACircuitBraker", 1);
	InstrumentLightingESSMnBCircuitBraker.Register(PSH, "InstrumentLightingESSMnBCircuitBraker", 1);
	InstrumentLightingNonESSCircuitBraker.Register(PSH, "InstrumentLightingNonESSCircuitBraker", 0);
	InstrumentLightingSCIEquipSEP1CircuitBraker.Register(PSH, "InstrumentLightingSCIEquipSEP1CircuitBraker", 1);
	InstrumentLightingSCIEquipSEP2CircuitBraker.Register(PSH, "InstrumentLightingSCIEquipSEP2CircuitBraker", 1);
	InstrumentLightingSCIEquipHatchCircuitBraker.Register(PSH, "InstrumentLightingSCIEquipHatchCircuitBraker", 1);

	ECSPOTH2OHTRMnACircuitBraker.Register(PSH, "ECSPOTH2OHTRMnACircuitBraker", 1);
	ECSPOTH2OHTRMnBCircuitBraker.Register(PSH, "ECSPOTH2OHTRMnBCircuitBraker", 1);
	ECSH2OAccumMnACircuitBraker.Register(PSH, "ECSH2OAccumMnACircuitBraker", 1);
	ECSH2OAccumMnBCircuitBraker.Register(PSH, "ECSH2OAccumMnBCircuitBraker", 1);
	ECSTransducerWastePOTH2OMnACircuitBraker.Register(PSH, "ECSTransducerWastePOTH2OMnACircuitBraker", 1);
	ECSTransducerWastePOTH2OMnBCircuitBraker.Register(PSH, "ECSTransducerWastePOTH2OMnBCircuitBraker", 1);
	ECSTransducerPressGroup1MnACircuitBraker.Register(PSH, "ECSTransducerPressGroup1MnACircuitBraker", 1);
	ECSTransducerPressGroup1MnBCircuitBraker.Register(PSH, "ECSTransducerPressGroup1MnBCircuitBraker", 1);
	ECSTransducerPressGroup2MnACircuitBraker.Register(PSH, "ECSTransducerPressGroup2MnACircuitBraker", 1);
	ECSTransducerPressGroup2MnBCircuitBraker.Register(PSH, "ECSTransducerPressGroup2MnBCircuitBraker", 1);
	ECSTransducerTempMnACircuitBraker.Register(PSH, "ECSTransducerTempMnACircuitBraker", 1);
	ECSTransducerTempMnBCircuitBraker.Register(PSH, "ECSTransducerTempMnBCircuitBraker", 1);

	ECSSecCoolLoopAc1CircuitBraker.Register(PSH, "ECSSecCoolLoopAc1CircuitBraker", 1);
	ECSSecCoolLoopAc2CircuitBraker.Register(PSH, "ECSSecCoolLoopAc2CircuitBraker", 1);
	ECSSecCoolLoopRADHTRMnACircuitBraker.Register(PSH, "ECSSecCoolLoopRADHTRMnACircuitBraker", 1);
	ECSSecCoolLoopXducersMnACircuitBraker.Register(PSH, "ECSSecCoolLoopXducersMnACircuitBraker", 1);
	ECSSecCoolLoopXducersMnBCircuitBraker.Register(PSH, "ECSSecCoolLoopXducersMnBCircuitBraker", 1);
	ECSWasteH2OUrineDumpHTRMnACircuitBraker.Register(PSH, "ECSWasteH2OUrineDumpHTRMnACircuitBraker", 0);
	ECSWasteH2OUrineDumpHTRMnBCircuitBraker.Register(PSH, "ECSWasteH2OUrineDumpHTRMnBCircuitBraker", 0);
	ECSCabinFanAC1ACircuitBraker.Register(PSH, "ECSCabinFanAC1ACircuitBraker", 1);
	ECSCabinFanAC1BCircuitBraker.Register(PSH, "ECSCabinFanAC1BCircuitBraker", 1);
	ECSCabinFanAC1CCircuitBraker.Register(PSH, "ECSCabinFanAC1CCircuitBraker", 1);
	ECSCabinFanAC2ACircuitBraker.Register(PSH, "ECSCabinFanAC2ACircuitBraker", 1);
	ECSCabinFanAC2BCircuitBraker.Register(PSH, "ECSCabinFanAC2BCircuitBraker", 1);
	ECSCabinFanAC2CCircuitBraker.Register(PSH, "ECSCabinFanAC2CCircuitBraker", 1);

	GNPowerAc1CircuitBraker.Register(PSH, "GNPowerAc1CircuitBraker", 1);
	GNPowerAc2CircuitBraker.Register(PSH, "GNPowerAc2CircuitBraker", 1);
	GNIMUMnACircuitBraker.Register(PSH, "GNIMUMnACircuitBraker", 1);
	GNIMUMnBCircuitBraker.Register(PSH, "GNIMUMnBCircuitBraker", 1);
	GNIMUHTRMnACircuitBraker.Register(PSH, "GNIMUHTRMnACircuitBraker", 1);
	GNIMUHTRMnBCircuitBraker.Register(PSH, "GNIMUHTRMnBCircuitBraker", 1);
	GNComputerMnACircuitBraker.Register(PSH, "GNComputerMnACircuitBraker", 0);
	GNComputerMnBCircuitBraker.Register(PSH, "GNComputerMnBCircuitBraker", 0);
	GNOpticsMnACircuitBraker.Register(PSH, "GNOpticsMnACircuitBraker", 1);
	GNOpticsMnBCircuitBraker.Register(PSH, "GNOpticsMnBCircuitBraker", 1);

	SuitCompressorsAc1ACircuitBraker.Register(PSH, "SuitCompressorsAc1ACircuitBraker", 1);
	SuitCompressorsAc1BCircuitBraker.Register(PSH, "SuitCompressorsAc1BCircuitBraker", 1);
	SuitCompressorsAc1CCircuitBraker.Register(PSH, "SuitCompressorsAc1CCircuitBraker", 1);
	SuitCompressorsAc2ACircuitBraker.Register(PSH, "SuitCompressorsAc2ACircuitBraker", 1);
	SuitCompressorsAc2BCircuitBraker.Register(PSH, "SuitCompressorsAc2BCircuitBraker", 1);
	SuitCompressorsAc2CCircuitBraker.Register(PSH, "SuitCompressorsAc2CCircuitBraker", 1);

	ECSGlycolPumpsAc1ACircuitBraker.Register(PSH, "ECSGlycolPumpsAc1ACircuitBraker", 1);
	ECSGlycolPumpsAc1BCircuitBraker.Register(PSH, "ECSGlycolPumpsAc1BCircuitBraker", 1);
	ECSGlycolPumpsAc1CCircuitBraker.Register(PSH, "ECSGlycolPumpsAc1CCircuitBraker", 1);
	ECSGlycolPumpsAc2ACircuitBraker.Register(PSH, "ECSGlycolPumpsAc2ACircuitBraker", 1);
	ECSGlycolPumpsAc2BCircuitBraker.Register(PSH, "ECSGlycolPumpsAc2BCircuitBraker", 1);
	ECSGlycolPumpsAc2CCircuitBraker.Register(PSH, "ECSGlycolPumpsAc2CCircuitBraker", 1);

	StabContSystemTVCAc1CircuitBraker.Register(PSH, "StabContSystemTVCAc1CircuitBraker", 1);
	StabContSystemAc1CircuitBraker.Register(PSH, "StabContSystemAc1CircuitBraker", 1);
	StabContSystemAc2CircuitBraker.Register(PSH, "StabContSystemAc2CircuitBraker", 1);

	ECATVCAc2CircuitBraker.Register(PSH, "StabContSystemTVCAc1CircuitBraker", 1);
	DirectUllMnACircuitBraker.Register(PSH, "DirectUllMnACircuitBraker", 1);
	DirectUllMnBCircuitBraker.Register(PSH, "DirectUllMnBCircuitBraker", 1);
	ContrDirectMnA1CircuitBraker.Register(PSH, "ContrDirectMnA1CircuitBraker", 1);
	ContrDirectMnB1CircuitBraker.Register(PSH, "ContrDirectMnB1CircuitBraker", 1);
	ContrDirectMnA2CircuitBraker.Register(PSH, "ContrDirectMnA2CircuitBraker", 1);
	ContrDirectMnB2CircuitBraker.Register(PSH, "ContrDirectMnB2CircuitBraker", 1);
	ACRollMnACircuitBraker.Register(PSH, "ACRollMnACircuitBraker", 1);
	ACRollMnBCircuitBraker.Register(PSH, "ACRollMnBCircuitBraker", 1);
	BDRollMnACircuitBraker.Register(PSH, "BDRollMnACircuitBraker", 1);
	BDRollMnBCircuitBraker.Register(PSH, "BDRollMnBCircuitBraker", 1);
	PitchMnACircuitBraker.Register(PSH, "PitchMnACircuitBraker", 1);
	PitchMnBCircuitBraker.Register(PSH, "PitchMnBCircuitBraker", 1);
	YawMnACircuitBraker.Register(PSH, "YawMnACircuitBraker", 1);
	YawMnBCircuitBraker.Register(PSH, "YawMnBCircuitBraker", 1);

	OrdealAc2CircuitBraker.Register(PSH, "OrdealAc2CircuitBraker", 1);
	OrdealMnBCircuitBraker.Register(PSH, "OrdealMnBCircuitBraker", 1);
	ContrAutoMnACircuitBraker.Register(PSH, "ContrAutoMnACircuitBraker", 1);
	ContrAutoMnBCircuitBraker.Register(PSH, "ContrAutoMnBCircuitBraker", 1);
	LogicBus12MnACircuitBraker.Register(PSH, "LogicBus12MnACircuitBraker", 1);
	LogicBus34MnACircuitBraker.Register(PSH, "LogicBus34MnACircuitBraker", 1);
	LogicBus14MnBCircuitBraker.Register(PSH, "LogicBus14MnBCircuitBraker", 1);
	LogicBus23MnBCircuitBraker.Register(PSH, "LogicBus23MnBCircuitBraker", 1);
	SystemMnACircuitBraker.Register(PSH, "SystemMnACircuitBraker", 1);
	SystemMnBCircuitBraker.Register(PSH, "SystemMnBCircuitBraker", 1);

	CMHeater1MnACircuitBraker.Register(PSH, "CMHeater1MnACircuitBraker", 0);
	CMHeater2MnBCircuitBraker.Register(PSH, "CMHeater2MnBCircuitBraker", 0);
	SMHeatersAMnBCircuitBraker.Register(PSH, "SMHeatersAMnBCircuitBraker", 1);
	SMHeatersCMnBCircuitBraker.Register(PSH, "SMHeatersCMnBCircuitBraker", 1);
	SMHeatersBMnACircuitBraker.Register(PSH, "SMHeatersBMnACircuitBraker", 1);
	SMHeatersDMnACircuitBraker.Register(PSH, "SMHeatersDMnACircuitBraker", 1);
	PrplntIsolMnACircuitBraker.Register(PSH, "PrplntIsolMnACircuitBraker", 1);
	PrplntIsolMnBCircuitBraker.Register(PSH, "PrplntIsolMnBCircuitBraker", 1);
	RCSLogicMnACircuitBraker.Register(PSH, "RCSLogicMnACircuitBraker", 1);
	RCSLogicMnBCircuitBraker.Register(PSH, "RCSLogicMnBCircuitBraker", 1);
	EMSMnACircuitBraker.Register(PSH, "EMSMnACircuitBraker", 1);
	EMSMnBCircuitBraker.Register(PSH, "EMSMnBCircuitBraker", 1);
	DockProbeMnACircuitBraker.Register(PSH, "DockProbeMnACircuitBraker", 1);
	DockProbeMnBCircuitBraker.Register(PSH, "DockProbeMnBCircuitBraker", 1);

	GaugingMnACircuitBraker.Register(PSH, "GaugingMnACircuitBraker", 1);
	GaugingMnBCircuitBraker.Register(PSH, "GaugingMnBCircuitBraker", 1);
	GaugingAc1CircuitBraker.Register(PSH, "GaugingAc1CircuitBraker", 1);
	GaugingAc2CircuitBraker.Register(PSH, "GaugingAc2CircuitBraker", 1);
	HeValveMnACircuitBraker.Register(PSH, "HeValveMnACircuitBraker", 1);
	HeValveMnBCircuitBraker.Register(PSH, "HeValveMnBCircuitBraker", 1);
	PitchBatACircuitBraker.Register(PSH, "PitchBatACircuitBraker", 0);
	PitchBatBCircuitBraker.Register(PSH, "PitchBatBCircuitBraker", 1);
	YawBatACircuitBraker.Register(PSH, "YawBatACircuitBraker", 0);
	YawBatBCircuitBraker.Register(PSH, "YawBatBCircuitBraker", 1);
	PilotValveMnACircuitBraker.Register(PSH, "PilotValveMnACircuit", 1);
	PilotValveMnBCircuitBraker.Register(PSH, "PilotValveMnBCircuit", 1);

	FloatBag1BatACircuitBraker.Register(PSH, "FloatBag1BatACircuitBraker", 0);
	FloatBag2BatBCircuitBraker.Register(PSH, "FloatBag2BatBCircuitBraker", 0);
	FloatBag3FLTPLCircuitBraker.Register(PSH, "FloatBag3FLTPLCircuitBraker", 0);

	SECSLogicBatACircuitBraker.Register(PSH, "SECSLogicBatACircuitBraker", 0);
	SECSLogicBatBCircuitBraker.Register(PSH, "SECSLogicBatBCircuitBraker", 0);
	SECSArmBatACircuitBraker.Register(PSH, "SECSArmBatACircuitBraker", 0);
	SECSArmBatBCircuitBraker.Register(PSH, "SECSArmBatBCircuitBraker", 0);

	EDS1BatACircuitBraker.Register(PSH, "EDS1BatACircuitBraker", 1);
	EDS2BatCCircuitBraker.Register(PSH, "EDS2BatCCircuitBraker", 1);
	EDS3BatBCircuitBraker.Register(PSH, "EDS3BatBCircuitBraker", 1);

	ELSBatACircuitBraker.Register(PSH, "ELSBatACircuitBraker", 1);
	ELSBatBCircuitBraker.Register(PSH, "ELSBatBCircuitBraker", 1);

	FLTPLCircuitBraker.Register(PSH, "FLTPLCircuitBraker", 1);

	ModeIntercomVOXSensThumbwheelSwitch.Register(PSH, "ModeIntercomVOXSensThumbwheelSwitch", 5, 9);

	PowerMasterVolumeThumbwheelSwitch.Register(PSH, "PowerMasterVolumeThumbwheelSwitch", 5, 9);

	PadCommVolumeThumbwheelSwitch.Register(PSH, "PadCommVolumeThumbwheelSwitch", 5, 9);

	IntercomVolumeThumbwheelSwitch.Register(PSH, "IntercomVolumeThumbwheelSwitch", 5, 9);

	SBandVolumeThumbwheelSwitch.Register(PSH, "SBandVolumeThumbwheelSwitch", 5, 9);

	VHFAMVolumeThumbwheelSwitch.Register(PSH, "VHFAMVolumeThumbwheelSwitch", 5, 9);

	OpticsZeroSwitch.Register(PSH, "OpticsZeroSwitch", TOGGLESWITCH_UP);
	ControllerSpeedSwitch.Register(PSH, "ControllerSpeedSwitch", THREEPOSSWITCH_DOWN);
	ControllerCouplingSwitch.Register(PSH, "ControllerCouplingSwitch", TOGGLESWITCH_UP);
	OpticsModeSwitch.Register(PSH, "OpticsModeSwitch", TOGGLESWITCH_DOWN);
	ControllerTelescopeTrunnionSwitch.Register(PSH, "ControllerTelescopeTrunnionSwitch", THREEPOSSWITCH_UP);	
	ConditionLampsSwitch.Register(PSH, "ConditionLampsSwitch", THREEPOSSWITCH_UP, SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);
	ConditionLampsSwitch.SetCallback(new PanelSwitchCallback<CSMCautionWarningSystem>(&cws, &CSMCautionWarningSystem::GNLampSwitchToggled));
	UPTLMSwitch.Register(PSH, "UPTLMSwitch", TOGGLESWITCH_UP);
	OpticsHandcontrollerSwitch.Register(PSH, "OpticsHandcontrollerSwitch");
	OpticsMarkButton.Register(PSH, "OpticsMarkButton", false);
	OpticsMarkButton.SetDelayTime(1);
	OpticsMarkRejectButton.Register(PSH, "OpticsMarkRejectButton", false);
	OpticsMarkRejectButton.SetDelayTime(1);
	MinImpulseHandcontrollerSwitch.Register(PSH, "MinImpulseHandcontrollerSwitch", true);

	GlycolToRadiatorsLever.Register(PSH, "GlycolToRadiatorsLever", 1);
	CabinPressureReliefLever1.Register(PSH, "CabinPressureReliefLever1", 2, 2, true);
	CabinPressureReliefLever2.Register(PSH, "CabinPressureReliefLever2", 2, 3, true);

	SuitCircuitReturnValveLever.Register(PSH, "SuitCircuitReturnValveLever", 1);

	O2DemandRegulatorRotary.AddPosition(0, 0);
	O2DemandRegulatorRotary.AddPosition(1, 120);
	O2DemandRegulatorRotary.AddPosition(2, 180);
	O2DemandRegulatorRotary.AddPosition(3, 240);
	O2DemandRegulatorRotary.Register(PSH, "O2DemandRegulatorRotary", 2);

	SuitTestRotary.AddPosition(0, 0);
	SuitTestRotary.AddPosition(1, 60);
	SuitTestRotary.AddPosition(2, 90);
	SuitTestRotary.Register(PSH, "SuitTestRotary", 0);

	ORDEALFDAI1Switch.Register(PSH, "ORDEALFDAI1Switch", false);
	ORDEALFDAI2Switch.Register(PSH, "ORDEALFDAI2Switch", false);
	ORDEALEarthSwitch.Register(PSH, "ORDEALEarthSwitch", THREEPOSSWITCH_CENTER);
	ORDEALLightingSwitch.Register(PSH, "ORDEALLightingSwitch", THREEPOSSWITCH_CENTER);
	ORDEALModeSwitch.Register(PSH, "ORDEALModeSwitch", false);	
	ORDEALSlewSwitch.Register(PSH, "ORDEALSlewSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	PanelOrdeal.Register(PSH, "PanelOrdeal",0, 0, 0, 0);	// dummy switch/display for checklist controller

	RNDZXPDRSwitch.Register(PSH, "RNDZXPDRSwitch", TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);

	CMRCSHTRSSwitch.Register(PSH, "CMRCSHTRSSwitch", false);
	WasteH2ODumpSwitch.Register(PSH, "WasteH2ODumpSwitch", THREEPOSSWITCH_CENTER);
	UrineDumpSwitch.Register(PSH, "UrineDumpSwitch", THREEPOSSWITCH_CENTER);

	UtilityPowerSwitch.Register(PSH, "UtilityPowerSwitch", false);
	Panel100FloodDimSwitch.Register(PSH, "Panel100FloodDimSwitch", TOGGLESWITCH_UP);	
	Panel100FloodFixedSwitch.Register(PSH, "Panel100FloodFixedSwitch", false);
	GNPowerOpticsSwitch.Register(PSH, "GNPowerOpticsSwitch", false);
	GNPowerIMUSwitch.Register(PSH, "GNPowerIMUSwitch", TOGGLESWITCH_DOWN, false);
	GNPowerIMUSwitch.SetGuardResetsState(false);
	Panel100RNDZXPDRSwitch.Register(PSH, "Panel100RNDZXPDRSwitch", THREEPOSSWITCH_CENTER);

	SCIUtilPowerSwitch.Register(PSH, "SCIUtilPowerSwitch", false);

	SCIInstSwitch.Register(PSH, "SCIInstSwitch", TOGGLESWITCH_UP);
	SCIInstSwitch.SetSideways(1);

	CenterSuitPowerSwitch.Register(PSH, "CenterSuitPowerSwitch", false);
	CenterAudioControlSwitch.Register(PSH, "CenterAudioControlSwitch", TOGGLESWITCH_UP);

	ModeIntercomSwitch.Register(PSH, "ModeIntercomSwitch", THREEPOSSWITCH_UP);
	PadComSwitch.Register(PSH, "PadComSwitch", THREEPOSSWITCH_UP);
	SBandSwitch.Register(PSH, "SBandSwitch", THREEPOSSWITCH_UP);

	PowerAudioSwitch.Register(PSH, "PowerAudioSwitch", THREEPOSSWITCH_CENTER);
	IntercomSwitch.Register(PSH, "IntercomSwitch", THREEPOSSWITCH_UP);
	VHFAMSwitch.Register(PSH, "VHFAMSwitch", THREEPOSSWITCH_UP);

	InverterPower3MainBCircuitBraker.Register(PSH, "InverterPower3MainBCircuitBraker", 1);
	InverterPower3MainACircuitBraker.Register(PSH, "InverterPower3MainACircuitBraker", 1);
	InverterPower2MainBCircuitBraker.Register(PSH, "InverterPower2MainBCircuitBraker", 1);
	InverterPower1MainACircuitBraker.Register(PSH, "InverterPower1MainACircuitBraker", 1);
	FlightPostLandingMainBCircuitBraker.Register(PSH, "FlightPostLandingMainBCircuitBraker", 1);
	FlightPostLandingMainACircuitBraker.Register(PSH, "FlightPostLandingMainACircuitBraker", 1);
	FlightPostLandingBatCCircuitBraker.Register(PSH, "FlightPostLandingBatCCircuitBraker", 0);
	FlightPostLandingBatBusBCircuitBraker.Register(PSH, "FlightPostLandingBatBusBCircuitBraker", 0);
	FlightPostLandingBatBusACircuitBraker.Register(PSH, "FlightPostLandingBatBusACircuitBraker", 0);
	MainBBatBusBCircuitBraker.Register(PSH, "MainBBatBusBCircuitBraker", 1);
	MainBBatCCircuitBraker.Register(PSH, "MainBBatCCircuitBraker", 0);
	MainABatCCircuitBraker.Register(PSH, "MainABatCCircuitBraker", 0);
	MainABatBusACircuitBraker.Register(PSH, "MainABatBusACircuitBraker", 1);

	Panel276CB1.Register(PSH, "Panel276CB1", 1);
	Panel276CB2.Register(PSH, "Panel276CB2", 1);
	Panel276CB3.Register(PSH, "Panel276CB3", 1);
	Panel276CB4.Register(PSH, "Panel276CB4", 1);

	UprightingSystemCompressor1CircuitBraker.Register(PSH, "UprightingSystemCompressor1CircuitBraker", 0);
	UprightingSystemCompressor2CircuitBraker.Register(PSH, "UprightingSystemCompressor2CircuitBraker", 0);
	SIVBLMSepPyroACircuitBraker.Register(PSH, "SIVBLMSepPyroACircuitBraker", 1);
	SIVBLMSepPyroBCircuitBraker.Register(PSH, "SIVBLMSepPyroBCircuitBraker", 1);

	PCMTLMGroup1CB.Register(PSH, "PCMTLMGroup1CB", 1);
	PCMTLMGroup2CB.Register(PSH, "PCMTLMGroup2CB", 1);
	FLTBusMNACB.Register(PSH, "FLTBusMNACB", 1);
	FLTBusMNBCB.Register(PSH, "FLTBusMNBCB", 1);
	PMPPowerPrimCB.Register(PSH, "PMPPowerPrimCB", 1);
	PMPPowerAuxCB.Register(PSH, "PMPPowerAuxCB", 1);
	VHFStationAudioLCB.Register(PSH, "VHFStationAudioLCB", 1);
	VHFStationAudioCTRCB.Register(PSH, "VHFStationAudioCTRCB", 1);
	VHFStationAudioRCB.Register(PSH, "VHFStationAudioRCB", 1);
	UDLCB.Register(PSH, "UDLCB", 1);
	HGAFLTBus1CB.Register(PSH, "HGAFLTBus1CB", 0);
	HGAGroup2CB.Register(PSH, "HGAGroup2CB", 0);
	SBandFMXMTRFLTBusCB.Register(PSH, "SBandFMXMTRFLTBusCB", 1);
	SBandFMXMTRGroup1CB.Register(PSH, "SBandFMXMTRGroup1CB", 1);
	CentralTimingEquipMNACB.Register(PSH, "CentralTimingEquipMNACB", 1);
	CentralTimingEquipMNBCB.Register(PSH, "CentralTimingEquipMNBCB", 1);
	RNDZXPNDRFLTBusCB.Register(PSH, "RNDZXPNDRFLTBusCB", 1);
	SIGCondrFLTBusCB.Register(PSH, "SIGCondrFLTBusCB", 1);
	SBandPWRAmpl1FLTBusCB.Register(PSH, "SBandPWRAmpl1FLTBusCB", 1);
	SBandPWRAmpl1Group1CB.Register(PSH, "SBandPWRAmpl1Group1CB", 1);
	SBandPWRAmpl2FLTBusCB.Register(PSH, "SBandPWRAmpl2FLTBusCB", 1);
	SBandPWRAmpl2Group1CB.Register(PSH, "SBandPWRAmpl2Group1CB", 1);

	FuelCell1PumpsACCB.Register(PSH, "FuelCell1PumpsACCB", 1);
	FuelCell1ReacsCB.Register(PSH, "FuelCell1ReacsCB", 0);
	FuelCell1BusContCB.Register(PSH, "FuelCell1BusContCB", 1);
	FuelCell1PurgeCB.Register(PSH, "FuelCell1PurgeCB", 1);
	FuelCell1RadCB.Register(PSH, "FuelCell1RadCB", 1);
	CryogenicH2HTR1CB.Register(PSH, "CryogenicH2HTR1CB", 1);
	CryogenicH2HTR2CB.Register(PSH, "CryogenicH2HTR2CB", 1);
	FuelCell2PumpsACCB.Register(PSH, "FuelCell2PumpsACCB", 1);
	FuelCell2ReacsCB.Register(PSH, "FuelCell2ReacsCB", 0);
	FuelCell2BusContCB.Register(PSH, "FuelCell2BusContCB", 1);
	FuelCell2PurgeCB.Register(PSH, "FuelCell2PurgeCB", 1);
	FuelCell2RadCB.Register(PSH, "FuelCell2RadCB", 1);
	CryogenicO2HTR1CB.Register(PSH, "CryogenicO2HTR1CB", 1);
	CryogenicO2HTR2CB.Register(PSH, "CryogenicO2HTR2CB", 1);
	FuelCell3PumpsACCB.Register(PSH, "FuelCell3PumpsACCB", 1);
	FuelCell3ReacsCB.Register(PSH, "FuelCell3ReacsCB", 0);
	FuelCell3BusContCB.Register(PSH, "FuelCell3BusContCB", 1);
	FuelCell3PurgeCB.Register(PSH, "FuelCell3PurgeCB", 1);
	FuelCell3RadCB.Register(PSH, "FuelCell3RadCB", 1);
	CryogenicQTYAmpl1CB.Register(PSH, "CryogenicQTYAmpl1CB", 1);
	CryogenicQTYAmpl2CB.Register(PSH, "CryogenicQTYAmpl2CB", 1);
	CryogenicFanMotorsAC1ACB.Register(PSH, "CryogenicFanMotorsAC1ACB", 1);
	CryogenicFanMotorsAC1BCB.Register(PSH, "CryogenicFanMotorsAC1BCB", 1);
	CryogenicFanMotorsAC1CCB.Register(PSH, "CryogenicFanMotorsAC1CCB", 1);
	CryogenicFanMotorsAC2ACB.Register(PSH, "CryogenicFanMotorsAC2ACB", 1);
	CryogenicFanMotorsAC2BCB.Register(PSH, "CryogenicFanMotorsAC2BCB", 1);
	CryogenicFanMotorsAC2CCB.Register(PSH, "CryogenicFanMotorsAC2CCB", 1);
	LightingRndzMNACB.Register(PSH, "LightingRndzMNACB", 1);
	LightingRndzMNBCB.Register(PSH, "LightingRndzMNBCB", 1);
	LightingFloodMNACB.Register(PSH, "LightingFloodMNACB", 1);
	LightingFloodMNBCB.Register(PSH, "LightingFloodMNBCB", 1);
	LightingFloodFLTPLCB.Register(PSH, "LightingFloodFLTPLCB", 1);
	LightingNumIntLEBCB.Register(PSH, "LightingNumIntLEBCB", 1);
	LightingNumIntLMDCCB.Register(PSH, "LightingNumIntLMDCCB", 1);
	LightingNumIntRMDCCB.Register(PSH, "LightingNumIntRMDCCB", 1);
	RunEVATRGTAC1CB.Register(PSH, "RunEVATRGTAC1CB", 1);
	RunEVATRGTAC2CB.Register(PSH, "RunEVATRGTAC2CB", 1);

	TimersMnACircuitBraker.Register(PSH, "TimersMnACircuitBraker", 1);
	TimersMnBCircuitBraker.Register(PSH, "TimersMnBCircuitBraker", 1);
	EPSMnAGroup1CircuitBraker.Register(PSH, "EPSMnAGroup1CircuitBraker", 1);
	EPSMnBGroup1CircuitBraker.Register(PSH, "EPSMnBGroup1CircuitBraker", 1);
	SPSLineHtrsMnACircuitBraker.Register(PSH, "SPSLineHtrsMnACircuitBraker", 1);
	SPSLineHtrsMnBCircuitBraker.Register(PSH, "SPSLineHtrsMnBCircuitBraker", 1);
	EPSMnAGroup2CircuitBraker.Register(PSH, "EPSMnAGroup2CircuitBraker", 1);
	EPSMnBGroup2CircuitBraker.Register(PSH, "EPSMnBGroup2CircuitBraker", 1);
	O2VacIonPumpsMnACircuitBraker.Register(PSH, "O2VacIonPumpsMnACircuitBraker", 0);
	O2VacIonPumpsMnBCircuitBraker.Register(PSH, "O2VacIonPumpsMnBCircuitBraker", 0);
	EPSMnAGroup3CircuitBraker.Register(PSH, "EPSMnAGroup3CircuitBraker", 1);
	EPSMnBGroup3CircuitBraker.Register(PSH, "EPSMnBGroup3CircuitBraker", 1);
	MainReleasePyroACircuitBraker.Register(PSH, "MainReleasePyroACircuitBraker", 0);
	MainReleasePyroBCircuitBraker.Register(PSH, "MainReleasePyroBCircuitBraker", 0);
	EPSMnAGroup4CircuitBraker.Register(PSH, "EPSMnAGroup4CircuitBraker", 1);
	EPSMnBGroup4CircuitBraker.Register(PSH, "EPSMnBGroup4CircuitBraker", 1);
	EPSMnAGroup5CircuitBraker.Register(PSH, "EPSMnAGroup5CircuitBraker", 1);
	EPSMnBGroup5CircuitBraker.Register(PSH, "EPSMnBGroup5CircuitBraker", 1);
	UtilityCB1.Register(PSH, "UtilityCB1", 1);
	UtilityCB2.Register(PSH, "UtilityCB2", 1);
	EPSBatBusACircuitBraker.Register(PSH, "EPSBatBusACircuitBraker", 1);
	EPSBatBusBCircuitBraker.Register(PSH, "EPSBatBusBCircuitBraker", 1);

	BatBusAToPyroBusTieCircuitBraker.Register(PSH, "BatBusAToPyroBusTieCircuitBraker", 0);
	PyroASeqACircuitBraker.Register(PSH, "PyroASeqACircuitBraker", 1);
	BatBusBToPyroBusTieCircuitBraker.Register(PSH, "BatBusBToPyroBusTieCircuitBraker", 0);
	PyroBSeqBCircuitBraker.Register(PSH, "PyroBSeqBCircuitBraker", 1);
	BatAPWRCircuitBraker.Register(PSH, "BatAPWRCircuitBraker", 1);
	BatBPWRCircuitBraker.Register(PSH, "BatBPWRCircuitBraker", 1);
	BatCPWRCircuitBraker.Register(PSH, "BatCPWRCircuitBraker", 1);
	BatCtoBatBusACircuitBraker.Register(PSH, "BatCtoBatBusACircuitBraker", 0);
	BatCtoBatBusBCircuitBraker.Register(PSH, "BatCtoBatBusBCircuitBraker", 0);
	BatCCHRGCircuitBraker.Register(PSH, "BatCCHRGCircuitBraker", 1);

	SuitCircuitFlow300Switch.Register(PSH, "SuitCircuitFlow300Switch", THREEPOSSWITCH_DOWN);
	SuitCircuitFlow301Switch.Register(PSH, "SuitCircuitFlow301Switch", THREEPOSSWITCH_DOWN);
	SuitCircuitFlow302Switch.Register(PSH, "SuitCircuitFlow302Switch", THREEPOSSWITCH_DOWN);

	HatchGearBoxSelector.AddPosition(0, 60);
	HatchGearBoxSelector.AddPosition(1, 90);
	HatchGearBoxSelector.AddPosition(2, 120);
	HatchGearBoxSelector.Register(PSH, "HatchGearBoxSelector", 2);

	HatchActuatorHandleSelector.AddPosition(0, 150);
	HatchActuatorHandleSelector.AddPosition(1, 180);
	HatchActuatorHandleSelector.AddPosition(2, 210);
	HatchActuatorHandleSelector.Register(PSH, "HatchActuatorHandleSelector", 2);

	HatchVentValveRotary.AddPosition(0, 120);
	HatchVentValveRotary.AddPosition(1, 150);
	HatchVentValveRotary.AddPosition(2, 180);
	HatchVentValveRotary.AddPosition(3, 210);
	HatchVentValveRotary.AddPosition(4, 240);
	HatchVentValveRotary.AddPosition(5, 270);
	HatchVentValveRotary.AddPosition(6, 300);
	HatchVentValveRotary.AddPosition(7, 330);
	HatchVentValveRotary.Register(PSH, "HatchVentValveRotary", 7);

	HatchActuatorHandleSelectorOpen.AddPosition(0, 180);
	HatchActuatorHandleSelectorOpen.AddPosition(1, 200);
	HatchActuatorHandleSelectorOpen.AddPosition(2, 210);
	HatchActuatorHandleSelectorOpen.Register(PSH, "HatchActuatorHandleSelectorOpen", 2);

	HatchToggle.Register(PSH, "HatchToggle", false);

	HatchEmergencyO2ValveSwitch.Register(PSH, "HatchEmergencyO2ValveSwitch", 0);
	HatchRepressO2ValveSwitch.Register(PSH, "HatchRepressO2ValveSwitch", THREEPOSSWITCH_DOWN);
	HatchOxygenRepressPressMeter.Register(PSH, "HatchOxygenRepressPressMeter", 0, 1200, 1, 900);
	Panel600.Register(PSH, "Panel600",0, 0, 0, 0);	// dummy switch/display for checklist controller

	DskySwitchVerb.Register(PSH, "DskySwitchVerb", false);
	DskySwitchNoun.Register(PSH, "DskySwitchNoun", false);
	DskySwitchPlus.Register(PSH, "DskySwitchPlus", false);
	DskySwitchMinus.Register(PSH, "DskySwitchMinus", false);
	DskySwitchZero.Register(PSH, "DskySwitchZero", false);
	DskySwitchOne.Register(PSH, "DskySwitchOne", false);
	DskySwitchTwo.Register(PSH, "DskySwitchTwo", false);
	DskySwitchThree.Register(PSH, "DskySwitchThree", false);
	DskySwitchFour.Register(PSH, "DskySwitchFour", false);
	DskySwitchFive.Register(PSH, "DskySwitchFive", false);
	DskySwitchSix.Register(PSH, "DskySwitchSix", false);
	DskySwitchSeven.Register(PSH, "DskySwitchSeven", false);
	DskySwitchEight.Register(PSH, "DskySwitchEight", false);
	DskySwitchNine.Register(PSH, "DskySwitchNine", false);
	DskySwitchClear.Register(PSH, "DskySwitchClear", false);
	DskySwitchProg.Register(PSH, "DskySwitchProg", false);
	DskySwitchKeyRel.Register(PSH, "DskySwitchKeyRel", false);
	DskySwitchEnter.Register(PSH, "DskySwitchEnter", false);
	DskySwitchReset.Register(PSH, "DskySwitchReset", false);

	DskySwitchVerb.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::VerbCallback));
	DskySwitchNoun.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::NounCallback));
	DskySwitchPlus.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::PlusCallback));
	DskySwitchMinus.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::MinusCallback));
	DskySwitchZero.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::zeroCallback));
	DskySwitchOne.SetCallback( new PanelSwitchCallback<DSKY>(&dsky, &DSKY::oneCallback));
	DskySwitchTwo.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::twoCallback));
	DskySwitchThree.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::threeCallback));
	DskySwitchFour.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::fourCallback));
	DskySwitchFive.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::fiveCallback));
	DskySwitchSix.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::sixCallback));
	DskySwitchSeven.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::sevenCallback));
	DskySwitchEight.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::eightCallback));
	DskySwitchNine.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::nineCallback));
	DskySwitchProg.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::ProgCallback));
	DskySwitchClear.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::ClearCallback));
	DskySwitchKeyRel.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::KeyRelCallback));
	DskySwitchEnter.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::EnterCallback));
	DskySwitchReset.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::ResetCallback));

	Dsky2SwitchVerb.Register(PSH, "Dsky2SwitchVerb", false);
	Dsky2SwitchNoun.Register(PSH, "Dsky2SwitchNoun", false);
	Dsky2SwitchPlus.Register(PSH, "Dsky2SwitchPlus", false);
	Dsky2SwitchMinus.Register(PSH, "Dsky2SwitchMinus", false);
	Dsky2SwitchZero.Register(PSH, "Dsky2SwitchZero", false);
	Dsky2SwitchOne.Register(PSH, "Dsky2SwitchOne", false);
	Dsky2SwitchTwo.Register(PSH, "Dsky2SwitchTwo", false);
	Dsky2SwitchThree.Register(PSH, "Dsky2SwitchThree", false);
	Dsky2SwitchFour.Register(PSH, "Dsky2SwitchFour", false);
	Dsky2SwitchFive.Register(PSH, "Dsky2SwitchFive", false);
	Dsky2SwitchSix.Register(PSH, "Dsky2SwitchSix", false);
	Dsky2SwitchSeven.Register(PSH, "Dsky2SwitchSeven", false);
	Dsky2SwitchEight.Register(PSH, "Dsky2SwitchEight", false);
	Dsky2SwitchNine.Register(PSH, "Dsky2SwitchNine", false);
	Dsky2SwitchClear.Register(PSH, "Dsky2SwitchClear", false);
	Dsky2SwitchProg.Register(PSH, "Dsky2SwitchProg", false);
	Dsky2SwitchKeyRel.Register(PSH, "Dsky2SwitchKeyRel", false);
	Dsky2SwitchEnter.Register(PSH, "Dsky2SwitchEnter", false);
	Dsky2SwitchReset.Register(PSH, "Dsky2SwitchReset", false);

	Dsky2SwitchVerb.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::VerbCallback));
	Dsky2SwitchNoun.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::NounCallback));
	Dsky2SwitchPlus.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::PlusCallback));
	Dsky2SwitchMinus.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::MinusCallback));
	Dsky2SwitchZero.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::zeroCallback));
	Dsky2SwitchOne.SetCallback( new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::oneCallback));
	Dsky2SwitchTwo.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::twoCallback));
	Dsky2SwitchThree.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::threeCallback));
	Dsky2SwitchFour.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::fourCallback));
	Dsky2SwitchFive.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::fiveCallback));
	Dsky2SwitchSix.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::sixCallback));
	Dsky2SwitchSeven.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::sevenCallback));
	Dsky2SwitchEight.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::eightCallback));
	Dsky2SwitchNine.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::nineCallback));
	Dsky2SwitchProg.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::ProgCallback));
	Dsky2SwitchClear.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::ClearCallback));
	Dsky2SwitchKeyRel.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::KeyRelCallback));
	Dsky2SwitchEnter.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::EnterCallback));
	Dsky2SwitchReset.SetCallback(new PanelSwitchCallback<DSKY>(&dsky2, &DSKY::ResetCallback));

	DskySwitchProg.SetDelayTime(1.5);
	Dsky2SwitchProg.SetDelayTime(1.5);

	ASCPRollSwitch.Register(PSH, "ASCPRollSwitch", 0, 0, 0, 0);	// dummy switch/display for checklist controller
	ASCPPitchSwitch.Register(PSH, "ASCPPitchSwitch", 0, 0, 0, 0);
	ASCPYawSwitch.Register(PSH, "ASCPYawSwitch", 0, 0, 0, 0);
}
