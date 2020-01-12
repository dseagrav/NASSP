/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  MSS vessel

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

#include "MSS.h"
#include "nasspdefs.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo MSS.log";

// Pad and park coordinates
#define PARK_LON -80.6199329
#define PARK_LAT 28.5953797

#define PAD_LON -80.604151	///\todo fix for Orbiter 2010-P1
#define PAD_LAT 28.60839
 

DLLCLBK void InitModule(HINSTANCE hModule) {

	g_hDLL = hModule;
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new MSS(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (MSS*)vessel;
}


MSS::MSS(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	firstTimestepDone = false;
	LVName[0] = '\0';
	moveToPad = false;
	moveToVab = false;
	moveLVToPad = false;
	touchdownPointHeight = -67.25;		// park height
	hLV = 0;

	soundlib.InitSoundLib(oapiGetVesselInterface(hObj), SOUND_DIRECTORY);
}

MSS::~MSS() {
}

void MSS::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(100000);
	SetSize(120);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0,0,0);
    meshindexMSS = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\ApolloLC39_MSS"), &meshoffset);
	SetMeshVisibilityMode(meshindexMSS, MESHVIS_ALWAYS);

	SetTouchdownPointHeight(touchdownPointHeight);
}

void MSS::clbkPostCreation() {
}

void MSS::clbkPreStep(double simt, double simdt, double mjd) {

	if (!firstTimestepDone) DoFirstTimestep();
	
	// sprintf(oapiDebugString(), "Dist %f", GetDistanceTo(VAB_LON, VAB_LAT));
}

void MSS::clbkPostStep (double simt, double simdt, double mjd) {

	if (!firstTimestepDone) return;

	// Move MSS to pad, KSC Pad 39A is hardcoded!
	if (moveToPad) {
		VESSELSTATUS vs;
		GetStatus(vs);

		vs.status = 1;
		vs.vdata[0].x = PAD_LON * RAD;
		vs.vdata[0].y = PAD_LAT * RAD;
		vs.vdata[0].z = 180.26 * RAD; 
		DefSetState(&vs);
		moveToPad = false;
	}

	// Move MSS to park position, position is hardcoded!
	if (moveToVab) {
		VESSELSTATUS vs;
		GetStatus(vs);

		vs.status = 1;
		vs.vdata[0].x = PARK_LON * RAD;
		vs.vdata[0].y = PARK_LAT * RAD;
		vs.vdata[0].z = 245.34 * RAD; 
		DefSetState(&vs);
		moveToVab = false;
	}
}

void MSS::DoFirstTimestep() {

	char buffer[256];

	double vcount = oapiGetVesselCount();
	for (int i = 0; i < vcount; i++)	{
		OBJHANDLE h = oapiGetVesselByIndex(i);
		oapiGetObjectName(h, buffer, 256);
		if (!strcmp(LVName, buffer)){
			hLV = h;
		}
	}

	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	firstTimestepDone = true;
}

bool MSS::Detach() {

	// Is the crawler attached? 
	ATTACHMENTHANDLE ah = GetAttachmentHandle(true, 0);
	if (GetAttachmentStatus(ah) == NULL) return false;

	// Is the pad near?
	if (GetDistanceTo(PAD_LON, PAD_LAT) < 10.0) {
		
		SetTouchdownPointHeight(-79.3);		// pad height
		moveToPad = true;
		return true;
	}

	// Is the parking near?
	if (GetDistanceTo(PARK_LON, PARK_LAT) < 10.0) {
		
		SetTouchdownPointHeight(-67.25);	// park height
		moveToVab = true;
		return true;
	}
	return false;
}

bool MSS::Attach() {

	return true;
}

double MSS::GetDistanceTo(double lon, double lat) {

	double mylon, mylat, myrad, dlon, dlat;

	GetEquPos(mylon, mylat, myrad);
	dlon = (lon / DEG - mylon) * oapiGetSize(GetGravityRef());
	dlat = (lat / DEG - mylat) * oapiGetSize(GetGravityRef());

	return sqrt(dlon * dlon + dlat * dlat);
}

void MSS::SetTouchdownPointHeight(double height) {

	touchdownPointHeight = height;
	SetTouchdownPoints(_V(  0, touchdownPointHeight,  10), 
					   _V(-10, touchdownPointHeight, -10), 
					   _V( 10, touchdownPointHeight, -10));
}

void MSS::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "TOUCHDOWNPOINTHEIGHT", 20)) {
			sscanf (line + 20, "%lf", &touchdownPointHeight);
		} else if (!strnicmp (line, "LVNAME", 6)) {
			strncpy (LVName, line + 7, 64);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
	SetTouchdownPointHeight(touchdownPointHeight);
}

void MSS::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState(scn);

	papiWriteScenario_double(scn, "TOUCHDOWNPOINTHEIGHT", touchdownPointHeight);
	if (LVName[0])
		oapiWriteScenario_string(scn, "LVNAME", LVName);
}

int MSS::clbkConsumeDirectKey(char *kstate) {

	if (KEYMOD_SHIFT(kstate)) {
		return 0; 
	}

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
	double moveStep = 1.0e-8;
	double heightStep = 0.1;
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
		vs.vdata[0].z -= 1.0e-3;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		vs.vdata[0].z += 1.0e-3;
		DefSetState(&vs);
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);						
	}
	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		SetTouchdownPoints (_V(0, -GetCOG_elev() + heightStep, 1), _V(-1, -GetCOG_elev() + heightStep, -1), _V(1, -GetCOG_elev() + heightStep, -1));
		RESETKEY(kstate, OAPI_KEY_A);
	}
	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		SetTouchdownPoints (_V(0, -GetCOG_elev() - heightStep, 1), _V(-1, -GetCOG_elev() - heightStep, -1), _V(1, -GetCOG_elev() - heightStep, -1));
		RESETKEY(kstate, OAPI_KEY_S);
	}
	sprintf(oapiDebugString(), "GetCOG_elev %f", GetCOG_elev());
*/
	return 0;
}

int MSS::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0;
	}
	return 0;
}
