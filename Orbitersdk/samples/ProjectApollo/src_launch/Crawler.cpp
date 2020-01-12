/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Crawler Transporter vessel

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

#include "Crawler.h"
#include "nasspdefs.h"
#include "saturn.h"
#include "VAB.h"
#include "ML.h"
#include "MSS.h"
#include "papi.h"

// View positions
#define VIEWPOS_FRONTCABIN				0
#define VIEWPOS_REARCABIN				1
#define VIEWPOS_ML						2
#define VIEWPOS_GROUND					3
#define VIEWPOS_FRONTGANGWAY			4
#define VIEWPOS_REARGANGWAY				5
#define VIEWPOS_RIGHTREARGANGWAY		6

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo Crawler.log";

DLLCLBK void InitModule(HINSTANCE hModule) {

	g_hDLL = hModule;
}

DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new Crawler(hvessel, flightmodel);
}

DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (Crawler*)vessel;
}

Crawler::Crawler(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	velocity = 0;
	velocityStop = false;
	targetHeading = 0;
	wheeldeflect = 0;
	viewPos = VIEWPOS_FRONTCABIN;
	firstTimestepDone = false;
	standalone = false;
	MissionTime = 0;

	lastLatLongSet = false;
	lastLat = 0;
	lastLong = 0;
	
	keyAccelerate = false;
	keyBrake= false;
	keyLeft= false;
	keyRight= false;
	keyCenter = false;

	LVName[0] = 0;
	hML = NULL;
	hLV = NULL;
	hMSS = NULL;
	vccVis = NULL;	
	vccSpeed = 0;	
	vccSteering = 0;
	meshidxCrawler = 0;
	meshidxTruck1 = 0;
	meshidxTruck2 = 0;
	meshidxTruck3 = 0;
	meshidxTruck4 = 0;
	meshidxPanel = 0;
	meshidxPanelReverse = 0;

	soundlib.InitSoundLib(oapiGetVesselInterface(hObj), SOUND_DIRECTORY);
	soundlib.LoadSound(soundEngine, "CrawlerEngine.wav", BOTHVIEW_FADED_MEDIUM);

	panelMeshoffset = _V(0,0,0);
    panelMeshidx = 0;
}

Crawler::~Crawler() {
}

void Crawler::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(2721000);
	SetSize(40);
	SetPMI(_V(133, 189, 89));

	SetSurfaceFrictionCoeff(0.005, 0.5);
	SetRotDrag (_V(0, 0, 0));
	SetCW(0, 0, 0, 0);
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0); 

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	// Front cabin panel
	VECTOR3 meshoffset = _V(16.366, 4.675, 19.812);
    meshidxPanel = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_centerpanel"), &meshoffset);
	SetMeshVisibilityMode(meshidxPanel, MESHVIS_ALWAYS);

    // Speed indicator
	vccSpeedGroup.P.transparam.shift = _V(0, 0, 0);
	vccSpeedGroup.nmesh = meshidxPanel;
	vccSpeedGroup.ngrp = 10;
	vccSpeedGroup.transform = MESHGROUP_TRANSFORM::TRANSLATE;

    // Steering wheel rotation
	vccSteering1Group.P.rotparam.ref = _V(0.148, 0.084, 0.225);
	vccSteering1Group.P.rotparam.axis = _V(0, 0.913545, -0.406736);
	vccSteering1Group.P.rotparam.angle = 0.0;  
	vccSteering1Group.nmesh = meshidxPanel;
	vccSteering1Group.ngrp = 11;
	vccSteering1Group.transform = MESHGROUP_TRANSFORM::ROTATE;
	vccSteering2Group.P.rotparam.ref = _V(0.148, 0.084, 0.225);
	vccSteering2Group.P.rotparam.axis = _V(0, 0.913545, -0.406736);
	vccSteering2Group.P.rotparam.angle = 0.0;  
	vccSteering2Group.nmesh = meshidxPanel;
	vccSteering2Group.ngrp = 12;
	vccSteering2Group.transform = MESHGROUP_TRANSFORM::ROTATE;

	// Rear cabin panel
	meshoffset = _V(-15.057, 4.675, -15.298);
    meshidxPanelReverse = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_centerpanel_reverse"), &meshoffset);
	SetMeshVisibilityMode(meshidxPanelReverse, MESHVIS_ALWAYS);

    // Speed indicator (reverse)
	vccSpeedGroupReverse.P.transparam.shift = _V(0, 0, 0);
	vccSpeedGroupReverse.nmesh = meshidxPanelReverse;
	vccSpeedGroupReverse.ngrp = 10;
	vccSpeedGroupReverse.transform = MESHGROUP_TRANSFORM::TRANSLATE;

    // Steering wheel rotation (reverse)
	vccSteering1GroupReverse.P.rotparam.ref = _V(0.076, 0.084, 0.33);
	vccSteering1GroupReverse.P.rotparam.axis = _V(0, 0.913545, 0.406736);
	vccSteering1GroupReverse.P.rotparam.angle = 0.0;  
	vccSteering1GroupReverse.nmesh = meshidxPanelReverse;
	vccSteering1GroupReverse.ngrp = 11;
	vccSteering1GroupReverse.transform = MESHGROUP_TRANSFORM::ROTATE;
	vccSteering2GroupReverse.P.rotparam.ref = _V(0.076, 0.084, 0.33);
	vccSteering2GroupReverse.P.rotparam.axis = _V(0, 0.913545, 0.406736);
	vccSteering2GroupReverse.P.rotparam.angle = 0.0;  
	vccSteering2GroupReverse.nmesh = meshidxPanelReverse;
	vccSteering2GroupReverse.ngrp = 12;
	vccSteering2GroupReverse.transform = MESHGROUP_TRANSFORM::ROTATE;

	// Crawler
	meshoffset = _V(0.767, 3.387, 2.534);
    meshidxCrawler = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_Mainbody"), &meshoffset);
	SetMeshVisibilityMode(meshidxCrawler, MESHVIS_ALWAYS);

	// Tracks
	MESHHANDLE track = oapiLoadMeshGlobal("ProjectApollo\\Crawler_drivetrucks");
	meshoffset = _V(14.539, 1.765, 15.512);
    meshidxTruck1 = AddMesh(track, &meshoffset);
	SetMeshVisibilityMode(meshidxTruck1, MESHVIS_ALWAYS);

	meshoffset = _V(-12.957, 1.765, 15.512);
    meshidxTruck2 = AddMesh(track, &meshoffset);
	SetMeshVisibilityMode(meshidxTruck2, MESHVIS_ALWAYS);

	meshoffset = _V(14.539, 1.765, -10.359);
    meshidxTruck3 = AddMesh(track, &meshoffset);
	SetMeshVisibilityMode(meshidxTruck3, MESHVIS_ALWAYS);

	meshoffset = _V(-12.957, 1.765, -10.359);
    meshidxTruck4 = AddMesh(track, &meshoffset);
	SetMeshVisibilityMode(meshidxTruck4, MESHVIS_ALWAYS);

	// Panel position test
	// panelMeshoffset = meshoffset;
	// panelMeshidx = meshidxTruck3;

	CreateAttachment(false, _V(0.0, 6.3, 0.0), _V(0, 1, 0), _V(1, 0, 0), "ML", false);

	double tph = -0.01;
	SetTouchdownPoints(_V(  0, tph,  10), _V(-10, tph, -10), _V( 10, tph, -10));
}

void Crawler::clbkPreStep(double simt, double simdt, double mjd) {

	double maxVelocity = 0.894;

	if (!firstTimestepDone) DoFirstTimestep(); 

	if (IsAttached()) maxVelocity = maxVelocity / 2.0;

	double head, dv;
	oapiGetHeading(GetHandle(), &head);
	double timeW = oapiGetTimeAcceleration();

	if (((keyAccelerate && viewPos == VIEWPOS_FRONTCABIN) || (keyBrake && viewPos == VIEWPOS_REARCABIN)) && !velocityStop) {
		dv = 0.5 * simdt * (pow(2.0, log10(timeW))) / timeW;
		if (velocity < 0 && velocity + dv >= 0) {
			velocity = 0;
			velocityStop = true;
		} else {
			velocity += dv;
		}
		if (velocity > maxVelocity) velocity = maxVelocity;

	} else if (((keyBrake && viewPos == VIEWPOS_FRONTCABIN) || (keyAccelerate && viewPos == VIEWPOS_REARCABIN)) && !velocityStop) {
		dv = -0.5 * simdt * (pow(2.0, log10(timeW))) / timeW;
		if (velocity > 0 && velocity + dv <= 0) {
			velocity = 0;
			velocityStop = true;
		} else {
			velocity += dv;
		}
		if (velocity < -maxVelocity) velocity = -maxVelocity;
	
	} else if (!keyAccelerate && !keyBrake) {
		velocityStop = false;
	}
	// Reset flags
	keyAccelerate = false;
	keyBrake = false;

	double lat, lon;
	VESSELSTATUS vs;
	GetStatus(vs);

	if (vs.status == 1) {
		lon = vs.vdata[0].x;
		lat = vs.vdata[0].y;
	
	} else if (lastLatLongSet) {
		lon = lastLong;
		lat = lastLat;
		vs.vdata[0].z = head;
	
	} else return;

	if ((keyLeft && viewPos == VIEWPOS_FRONTCABIN) || (keyRight && viewPos == VIEWPOS_REARCABIN)) {
		wheeldeflect = max(-1,wheeldeflect - (0.5 * simdt * (pow(2.0, log10(timeW))) / timeW));
	
	} else if ((keyRight && viewPos == VIEWPOS_FRONTCABIN) || (keyLeft && viewPos == VIEWPOS_REARCABIN)) {
		wheeldeflect = min(1, wheeldeflect + (0.5 * simdt * (pow(2.0, log10(timeW))) / timeW));

	} else if (keyCenter) {
		wheeldeflect = 0;
	}
	keyLeft = false;
	keyRight = false;
	keyCenter = false;

	double r = 45; // 150 ft
	double dheading = wheeldeflect * velocity * simdt / r;
	// turn speed is only doubled when time acc is multiplied by ten:
	dheading = (pow(5.0, log10(timeW))) * dheading / timeW;
	vs.vdata[0].z += dheading;
	if(vs.vdata[0].z < 0) vs.vdata[0].z += 2.0 * PI;
	if(vs.vdata[0].z >= 2.0 * PI) vs.vdata[0].z -= -2.0 * PI;

	// Bugfix by SiameseCat
	VECTOR3 dir = _V(sin(head), cos(lat) * cos(head), 0);
	dir /= length(dir);
	lon += dir.x * velocity * simdt / oapiGetSize(GetGravityRef());
	lat += dir.y * velocity * simdt / oapiGetSize(GetGravityRef());

	vs.vdata[0].x = lon;
	vs.vdata[0].y = lat;
	vs.status = 1;
	DefSetState(&vs);

	lastLat = lat;
	lastLong = lon;
	lastLatLongSet = true;
	targetHeading = head;

	if (velocity != 0) {
		// velocity dependent sound disabled
		// soundEngine.play(LOOP, (int)(127.5 + 127.5 * velocity / maxVelocity));
		soundEngine.play();
	} else {
		soundEngine.stop();
	}

	//
	// Update the VC console
	//
	if (vccVis) {
		// Speed indicators
		double v = (velocity * 0.066); 
		vccSpeedGroup.P.transparam.shift.x = float(v - vccSpeed);
		vccSpeedGroupReverse.P.transparam.shift.x = float(v - vccSpeed);
		MeshgroupTransform(vccVis, vccSpeedGroup);
		MeshgroupTransform(vccVis, vccSpeedGroupReverse);
		vccSpeed = v;

		// Steering wheel
		double a = -wheeldeflect * 90.0 * RAD;
		vccSteering1Group.P.rotparam.angle = float(a - vccSteering);
		vccSteering2Group.P.rotparam.angle = float(a - vccSteering);
		vccSteering1GroupReverse.P.rotparam.angle = -float(a - vccSteering);
		vccSteering2GroupReverse.P.rotparam.angle = -float(a - vccSteering);
		MeshgroupTransform(vccVis, vccSteering1Group);
		MeshgroupTransform(vccVis, vccSteering2Group);
		MeshgroupTransform(vccVis, vccSteering1GroupReverse);
		MeshgroupTransform(vccVis, vccSteering2GroupReverse);
		vccSteering = a;		
	}

	/* VECTOR3 pos;
	GetRelativePos(hMSS, pos);
	sprintf(oapiDebugString(), "Dist %f", length(pos)); */
}

void Crawler::clbkPostStep(double simt, double simdt, double mjd) {
	//This seems the best place to update our mission time off of the Saturn.
	Saturn *lv = NULL;
	if (!standalone) {
		// Updating internal mission time from Launch Vehichle.
		if (hLV) {
			lv = (Saturn *)oapiGetVesselInterface(hLV);
			MissionTime = lv -> GetMissionTime();
		}
	}
}

void Crawler::DoFirstTimestep() {

	SetView();

	oapiGetHeading(GetHandle(), &targetHeading);
	
	// Turn off pretty much everything that Orbitersound does by default.
	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYLANDINGANDGROUNDSOUND, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	if (!standalone) {
		if (LVName[0])
			hLV = oapiGetVesselByName(LVName);
		hML = oapiGetVesselByName("ML");
		hMSS = oapiGetVesselByName("MSS");
	}
	firstTimestepDone = true;
}

void Crawler::clbkLoadStateEx(FILEHANDLE scn, void *status) {
	
	char *line;
	
	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "VELOCITY", 8)) {
			sscanf (line + 8, "%lf", &velocity);
		} else if (!strnicmp (line, "TARGETHEADING", 13)) {
			sscanf (line + 13, "%lf", &targetHeading);
		} else if (!strnicmp (line, "WHEELDEFLECT", 12)) {
			sscanf (line + 12, "%lf", &wheeldeflect);
		} else if (!strnicmp (line, "VIEWPOS", 7)) {
			sscanf (line + 7, "%i", &viewPos);
		} else if (!strnicmp (line, "STANDALONE", 10)) {
			sscanf (line + 10, "%i", &standalone);
		} else if (!strnicmp (line, "LVNAME", 6)) {
			strncpy (LVName, line + 7, 64);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
}

void Crawler::clbkSaveState(FILEHANDLE scn) {
	
	VESSEL2::clbkSaveState (scn);

	papiWriteScenario_double(scn, "VELOCITY", velocity);
	papiWriteScenario_double(scn, "TARGETHEADING", targetHeading);
	papiWriteScenario_double(scn, "WHEELDEFLECT", wheeldeflect);	
	oapiWriteScenario_int(scn, "VIEWPOS", viewPos);
	oapiWriteScenario_int(scn, "STANDALONE", standalone);
	if (LVName[0])
		oapiWriteScenario_string(scn, "LVNAME", LVName);
}

int Crawler::clbkConsumeDirectKey(char *kstate) {

	if (!firstTimestepDone) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0; 
	}

	if (KEYDOWN(kstate, OAPI_KEY_ADD)) {
		keyAccelerate = true;				
		RESETKEY(kstate, OAPI_KEY_ADD);
	}
	if (KEYDOWN(kstate, OAPI_KEY_SUBTRACT)) {
		keyBrake = true;				
		RESETKEY(kstate, OAPI_KEY_SUBTRACT);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD1)) {
		keyLeft = true;				
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD2)) {
		keyCenter = true;				
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD3)) {
		keyRight = true;				
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD5)) {
		keyCenter = true;				
		RESETKEY(kstate, OAPI_KEY_NUMPAD5);
	}

	// touchdown point test
	/*
	VESSELSTATUS vs;
	GetStatus(vs);

	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		touchdownPointHeight += 0.001;

		SetTouchdownPoints(_V(  0, touchdownPointHeight,  10), 
						   _V(-10, touchdownPointHeight, -10), 
						   _V( 10, touchdownPointHeight, -10));
		VSSetTouchdownPoints(GetHandle(), 
						   _V(  0, touchdownPointHeight,  10), 
						   _V(-10, touchdownPointHeight, -10), 
						   _V( 10, touchdownPointHeight, -10),
						   -touchdownPointHeight);

		sprintf(oapiDebugString(), "touchdownPointHeight %f", touchdownPointHeight);
		RESETKEY(kstate, OAPI_KEY_A);
	}
	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		touchdownPointHeight -= 0.001;

		SetTouchdownPoints(_V(  0, touchdownPointHeight,  10), 
						   _V(-10, touchdownPointHeight, -10), 
						   _V( 10, touchdownPointHeight, -10));
		VSSetTouchdownPoints(GetHandle(), 
						   _V(  0, touchdownPointHeight,  10), 
						   _V(-10, touchdownPointHeight, -10), 
						   _V( 10, touchdownPointHeight, -10),
						   -touchdownPointHeight);

		sprintf(oapiDebugString(), "touchdownPointHeight %f", touchdownPointHeight);
		RESETKEY(kstate, OAPI_KEY_S);
	}
	*/

	// Panel test
	/*		
	double step = 0.01;
	if (KEYMOD_CONTROL(kstate))
		step = 0.001;

	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD4)) {
		DelMesh(panelMeshidx);
		panelMeshoffset.x += step; 
	    panelMeshidx = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_drivetrucks"), &panelMeshoffset);
		SetMeshVisibilityMode(panelMeshidx, MESHVIS_ALWAYS);
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD6)) {
		DelMesh(panelMeshidx);
		panelMeshoffset.x -= step; 
	    panelMeshidx = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_drivetrucks"), &panelMeshoffset);
		SetMeshVisibilityMode(panelMeshidx, MESHVIS_ALWAYS);
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD8)) {
		DelMesh(panelMeshidx);
		panelMeshoffset.y += step; 
	    panelMeshidx = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_drivetrucks"), &panelMeshoffset);
		SetMeshVisibilityMode(panelMeshidx, MESHVIS_ALWAYS);
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD2)) {
		DelMesh(panelMeshidx);
		panelMeshoffset.y -= step; 
	    panelMeshidx = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_drivetrucks"), &panelMeshoffset);
		SetMeshVisibilityMode(panelMeshidx, MESHVIS_ALWAYS);
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD1)) {
		DelMesh(panelMeshidx);
		panelMeshoffset.z += step; 
	    panelMeshidx = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_drivetrucks"), &panelMeshoffset);
		SetMeshVisibilityMode(panelMeshidx, MESHVIS_ALWAYS);
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}
	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD3)) {
		DelMesh(panelMeshidx);
		panelMeshoffset.z -= step; 
	    panelMeshidx = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\Crawler_drivetrucks"), &panelMeshoffset);
		SetMeshVisibilityMode(panelMeshidx, MESHVIS_ALWAYS);
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}
	sprintf(oapiDebugString(), "panelMeshoffset x %f y %f z %f", panelMeshoffset.x, panelMeshoffset.y, panelMeshoffset.z);
	*/
	return 0;
}

int Crawler::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (!firstTimestepDone) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0; 
	}

	if (key == OAPI_KEY_J && down == true) {
		if (IsAttached())
			Detach();
		else
			Attach();
		return 1;
	}

	if (key == OAPI_KEY_1 && down == true) {
		SetView(VIEWPOS_FRONTCABIN);
		return 1;
	}
	if (key == OAPI_KEY_2 && down == true) {
		SetView(VIEWPOS_REARCABIN);
		return 1;
	}
	if (key == OAPI_KEY_3 && down == true) {
		SetView(VIEWPOS_ML);
		return 1;
	}
	if (key == OAPI_KEY_4 && down == true) {
		SetView(VIEWPOS_GROUND);
		return 1;
	}
	if (key == OAPI_KEY_5 && down == true) {
		SetView(VIEWPOS_FRONTGANGWAY);
		return 1;
	}
	if (key == OAPI_KEY_6 && down == true) {
		SetView(VIEWPOS_REARGANGWAY);
		return 1;
	}
	if (key == OAPI_KEY_7 && down == true) {
		SetView(VIEWPOS_RIGHTREARGANGWAY);
		return 1;
	}

	if (key == OAPI_KEY_NUMPAD7 && down == true) {
		if (!standalone) {
			OBJHANDLE hVab = oapiGetVesselByName("VAB");
			if (hVab) {
				VAB *vab = (VAB *) oapiGetVesselInterface(hVab);
				vab->ToggleHighBay1Door();
			}
		}
		return 1;
	}

	if (key == OAPI_KEY_NUMPAD8 && down == true) {
		if (!standalone) {
			OBJHANDLE hVab = oapiGetVesselByName("VAB");
			if (hVab) {
				VAB *vab = (VAB *) oapiGetVesselInterface(hVab);
				vab->ToggleHighBay3Door();
			}
		}
		return 1;
	}

	if (key == OAPI_KEY_B && down == true) {
		if (!standalone) {
			OBJHANDLE hVab = oapiGetVesselByName("VAB");
			if (hVab) {
				VAB *vab = (VAB *) oapiGetVesselInterface(hVab);
				vab->BuildSaturnStage();
			}
		}
		return 1;
	}

	if (key == OAPI_KEY_U && down == true) {
		if (!standalone) {
			OBJHANDLE hVab = oapiGetVesselByName("VAB");
			if (hVab) {
				VAB *vab = (VAB *) oapiGetVesselInterface(hVab);
				vab->UnbuildSaturnStage();
			}
		}
		return 1;
	} 
	return 0;
}

void Crawler::Attach() {

	if (standalone) return;
	if (velocity != 0) return;
	if (IsAttached()) return;

	ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 0);
	if (hML != NULL) {
		ML *ml = (ML *)oapiGetVesselInterface(hML);

		// Is the crawler close enough to the ML?
		VECTOR3 pos;
		GetRelativePos(hML, pos);
		if (length(pos) < 73) {
			if (ml->Attach()) {
				AttachChild(hML, ah, ml->GetAttachmentHandle(true, 0));
			}
		}
	}
	if (hMSS != NULL) {
		MSS *mss = (MSS *)oapiGetVesselInterface(hMSS);

		// Is the crawler close enough to the MSS?
		VECTOR3 pos;
		GetRelativePos(hMSS, pos);
		if (length(pos) < 68) {
			if (mss->Attach()) {
				AttachChild(hMSS, ah, mss->GetAttachmentHandle(true, 0));
			}
		}
	}
}

void Crawler::Detach() {

	if (standalone) return;
	if (velocity != 0) return;
	if (!IsAttached()) return;

	ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 0);
	// Is ML attached?
	if (hML == GetAttachmentStatus(ah)) {
		ML *ml = (ML *)oapiGetVesselInterface(hML);
		if (ml->Detach()) {
			SlowIfDesired(1.0);
			DetachChild(ah);
		}
	}
	// Is MSS attached?
	if (hMSS == GetAttachmentStatus(ah)) {
		MSS *mss = (MSS *)oapiGetVesselInterface(hMSS);
		if (mss->Detach()) {
			SlowIfDesired(1.0);
			DetachChild(ah);
		}
	}
}

bool Crawler::IsAttached() {

	if (standalone) return false;

	ATTACHMENTHANDLE ah = GetAttachmentHandle(false, 0);
	return (GetAttachmentStatus(ah) != NULL);
}

void Crawler::SetView() {
	SetView(viewPos);
}

void Crawler::SetView(int viewpos) {

	viewPos = viewpos;
	if (viewPos == VIEWPOS_REARCABIN) {
		SetCameraOffset(_V(-14.97, 5.3, -14.534));
		SetCameraDefaultDirection(_V(0, -0.309017, -0.951057));
		oapiCameraSetCockpitDir(0,0);
		SetMeshesVisibility(MESHVIS_ALWAYS);

	} else if (viewPos == VIEWPOS_FRONTCABIN) {
		SetCameraOffset(_V(16.5, 5.3, 19.6));
		SetCameraDefaultDirection(_V(0, -0.309017, 0.951057));
		oapiCameraSetCockpitDir(0,0);
		SetMeshesVisibility(MESHVIS_ALWAYS);

	} else if (viewPos == VIEWPOS_ML) {
		SetCameraOffset(_V(19.9, 15.4, -25.6));
		SetCameraDefaultDirection(_V(-0.630037, 0.453991, 0.630037));
		oapiCameraSetCockpitDir(0,0);
		SetMeshesVisibility(MESHVIS_ALWAYS | MESHVIS_EXTPASS);

	} else if (viewPos == VIEWPOS_GROUND) {
		SetCameraOffset(_V(21.5, 1.75, 1));
		SetCameraDefaultDirection(_V(0, 0, 1));
		oapiCameraSetCockpitDir(0,0);
		SetMeshesVisibility(MESHVIS_ALWAYS | MESHVIS_EXTPASS);

	} else if (viewPos == VIEWPOS_FRONTGANGWAY) {
		SetCameraOffset(_V(0, 4.1, 21.3));
		SetCameraDefaultDirection(_V(0, 0, 1));
		oapiCameraSetCockpitDir(0,0);
		SetMeshesVisibility(MESHVIS_ALWAYS);

	} else if (viewPos == VIEWPOS_REARGANGWAY) {
		SetCameraOffset(_V(0, 4.1, -16.234));
		SetCameraDefaultDirection(_V(0, 0, -1));
		oapiCameraSetCockpitDir(0,0);
		SetMeshesVisibility(MESHVIS_ALWAYS);

	} else if (viewPos == VIEWPOS_RIGHTREARGANGWAY) {
		SetCameraOffset(_V(16.4, 5.7, -11.434));
		SetCameraDefaultDirection(_V(0, 0, -1));
		oapiCameraSetCockpitDir(0,0);
		SetMeshesVisibility(MESHVIS_ALWAYS);
	}	
}

void Crawler::SetMeshesVisibility(WORD mode) {

	SetMeshVisibilityMode(meshidxCrawler, mode);
	SetMeshVisibilityMode(meshidxTruck1, mode);
	SetMeshVisibilityMode(meshidxTruck2, mode);
	SetMeshVisibilityMode(meshidxTruck3, mode);
	SetMeshVisibilityMode(meshidxTruck4, mode);
	SetMeshVisibilityMode(meshidxPanel, mode);
	SetMeshVisibilityMode(meshidxPanelReverse, mode);

}

void Crawler::SlowIfDesired(double timeAcceleration) {

	if (oapiGetTimeAcceleration() > timeAcceleration) {
		oapiSetTimeAcceleration(timeAcceleration);
	}
}

void Crawler::clbkVisualCreated(VISHANDLE vis, int refcount) {

	vccVis = vis;
}

void Crawler::clbkVisualDestroyed(VISHANDLE vis, int refcount) {
	
	vccVis = NULL;
	// reset the variables keeping track of console mesh animation
	vccSpeed = 0;
	vccSteering = 0;
}

bool Crawler::clbkLoadGenericCockpit() {

	SetView();
	return true;
}
