/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  VAB vessel

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

#include "VAB.h"
#include "ML.h"
#include "nasspdefs.h"
#include "saturn.h"
#include "papi.h"

HINSTANCE g_hDLL;
char trace_file[] = "ProjectApollo VAB.log";


DLLCLBK void InitModule(HINSTANCE hModule) {
	g_hDLL = hModule;
}


DLLCLBK VESSEL *ovcInit(OBJHANDLE hvessel, int flightmodel) {

	return new VAB(hvessel, flightmodel);
}


DLLCLBK void ovcExit(VESSEL *vessel) {

	if (vessel) delete (VAB*)vessel;
}


VAB::VAB(OBJHANDLE hObj, int fmodel) : VESSEL2 (hObj, fmodel) {

	firstTimestepDone = false;
	meshcountSaturn = 0;
	animCraneCount = 0;
	highBay1Door_Status = DOOR_CLOSED;
	highBay1Door_Proc = 0;
	highBay3Door_Status = DOOR_CLOSED;
	highBay3Door_Proc = 0;
	crane_Status = CRANE_BEGIN;
	currentAnimCrane = -1;
	crane_Proc = 0.00001;
	saturnStage1_Proc = 0;
	adjustSaturnStage1 = false;
	LVName[0] = '\0';
	LVVisible = false;

	int i;
	for (i = 0; i < 5; i++) {
		platform_Proc[i] = 0;
	}
	for (i = 0; i < 5; i++) {
		for (int j = 0; j < 16; j++) 
			mgroupCrane[i][j] = 0;
	}
	for (i = 0; i < 24; i++) {
		for (int j = 0; j < 8; j++) 
			mgroupSaturn[i][j] = 0;
	}
	for (i = 0; i < 5; i++) {
		for (int j = 0; j < 8; j++) 
			mgroupCrane2[i][j] = 0;
	}

	soundlib.InitSoundLib(oapiGetVesselInterface(hObj), SOUND_DIRECTORY);
}

VAB::~VAB() {

	int i;
	for (i = 0; i < 5; i++) {
		for (int j = 0; j < 16; j++) 
			if (mgroupCrane[i][j]) delete mgroupCrane[i][j];
	}
	for (i = 0; i < 24; i++) {
		for (int j = 0; j < 8; j++) 
			if (mgroupSaturn[i][j]) delete mgroupSaturn[i][j];
	}
	for (i = 0; i < 5; i++) {
		for (int j = 0; j < 8; j++) 
			if (mgroupCrane2[i][j]) delete mgroupCrane2[i][j];
	}
}

void VAB::clbkSetClassCaps(FILEHANDLE cfg) {

	SetEmptyMass(450000000);
	SetSize(160);

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 meshoffset = _V(0,0,0);
    meshindexVAB = AddMesh(oapiLoadMeshGlobal("ProjectApollo\\VAB_exterior"), &meshoffset);
	SetMeshVisibilityMode(meshindexVAB, MESHVIS_ALWAYS);

	//
	// Saturn 5 assembly
	//

	meshcountSaturn = 24;
	meshoffset = _V(120, 10, -1.5);
	meshindexSaturn[0] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg1"), &meshoffset);

	meshoffset = _V(120.11, 20, -1.5);
	meshindexSaturn[1] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg2base"), &meshoffset);
	meshoffset = _V(120.11, 20 - 13.3, -1.5);
	meshindexSaturn[2] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5intstg4"), &meshoffset);

	meshoffset = _V(120.11, 25, -1.5);
	meshindexSaturn[3] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg3base"), &meshoffset);
	meshoffset = _V(120.11, 25 - 19.2, -1.5);
	meshindexSaturn[4] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg2intstg"), &meshoffset);

	meshoffset = _V(120.11, 25, -1.5);
	meshindexSaturn[5] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/CM"), &meshoffset);
	meshoffset = _V(120.11, 25. - 4.15, -1.5);
	meshindexSaturn[6] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-core"), &meshoffset);
	meshindexSaturn[7] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-RCSHI"), &meshoffset);
	meshindexSaturn[8] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-Panel1"), &meshoffset);
	meshindexSaturn[9] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-Panel2"), &meshoffset);
	meshindexSaturn[10] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-Panel3"), &meshoffset);
	meshindexSaturn[11] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-Panel4"), &meshoffset);
	meshindexSaturn[12] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-Panel5"), &meshoffset);
	meshindexSaturn[13] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/SM-Panel6"), &meshoffset);

	meshoffset = _V(120.11 - 1.48, 25. - 8.7, -1.5 - 1.48);
	meshindexSaturn[14] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg31"), &meshoffset);
	meshoffset = _V(120.11 - 1.48, 25. - 8.7, -1.5 + 1.48);
	meshindexSaturn[15] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg32"), &meshoffset);
	meshoffset = _V(120.11 + 1.48, 25. - 8.7, -1.5 + 1.48);
	meshindexSaturn[16] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg33"), &meshoffset);
	meshoffset = _V(120.11 + 1.48, 25. - 8.7, -1.5 - 1.48);
	meshindexSaturn[17] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg34"), &meshoffset);
	meshoffset = _V(120.11 + 0.1, 25. - 11.25, -1.5 + 0.05);
	meshindexSaturn[18] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/LM_SLA"), &meshoffset);
	meshoffset = _V(120.11, 25, -1.5);
	meshindexSaturn[19] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/CM-HatchC"), &meshoffset);
	meshoffset = _V(120.11, 25, -1.5);
	meshindexSaturn[20] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/CM-Probe"), &meshoffset);
	meshoffset = _V(120.11, 25. - 21.25, -1.5);
	meshindexSaturn[21] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/sat5stg3intstg"), &meshoffset);
	meshoffset = _V(120.11, 25, -1.5);
	meshindexSaturn[22] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/CM-Interior"), &meshoffset);

	meshoffset = _V(120.11, 25, -1.5);
	meshindexSaturn[23] = AddMesh(oapiLoadMeshGlobal("ProjectApollo/BoostCover"), &meshoffset);
	
	int i;
	for (i = 0; i < meshcountSaturn; i++) 
		SetMeshVisibilityMode(meshindexSaturn[i], MESHVIS_NEVER);

	DefineAnimations();
	SetTouchdownPoints(_V(0, 0.05, 10), _V(-10, 0.05, -10), _V(10, 0.05, -10));
	SetCameraOffset(_V(-27.0, -12.0, -48.0));

}

void VAB::DefineAnimations() {

	int i = 0;

	// High bay 1 door
	static UINT highBay1Door1_groups[1] = {16};
	static MGROUP_TRANSLATE highBay1Door1 (0, highBay1Door1_groups, 1, _V(12, 0, 0));

	static UINT highBay1Door2_groups[1] = {18};
	static MGROUP_TRANSLATE highBay1Door2 (0, highBay1Door2_groups, 1, _V(-12, 0, 0));

	static UINT highBay1Door3_groups[2] = {19, 18};
	static MGROUP_TRANSLATE highBay1Door3 (0, highBay1Door3_groups, 2, _V(-12, 0, 0));

	static UINT highBay1Door4_groups[2] = {17, 16};
	static MGROUP_TRANSLATE highBay1Door4 (0, highBay1Door4_groups, 2, _V(12, 0, 0));

	static UINT highBay1Door5_groups[1] = {36};
	static MGROUP_TRANSLATE highBay1Door5 (0, highBay1Door5_groups, 1, _V(0, 106, 0));

	static UINT highBay1Door6_groups[1] = {35};
	static MGROUP_TRANSLATE highBay1Door6 (0, highBay1Door6_groups, 1, _V(0, 91.5, 0));

	static UINT highBay1Door7_groups[1] = {34};
	static MGROUP_TRANSLATE highBay1Door7 (0, highBay1Door7_groups, 1, _V(0, 77.36, 0));

	static UINT highBay1Door8_groups[1] = {33};
	static MGROUP_TRANSLATE highBay1Door8 (0, highBay1Door8_groups, 1, _V(0, 63, 0));

	static UINT highBay1Door9_groups[1] = {32};
	static MGROUP_TRANSLATE highBay1Door9 (0, highBay1Door9_groups, 1, _V(0, 48, 0));

	static UINT highBay1Door10_groups[1] = {31};
	static MGROUP_TRANSLATE highBay1Door10 (0, highBay1Door10_groups, 1, _V(0, 34, 0));

	static UINT highBay1Door11_groups[1] = {30};
	static MGROUP_TRANSLATE highBay1Door11 (0, highBay1Door11_groups, 1, _V(0, 19.73, 0));

	anim_HighBay1Door = CreateAnimation(0.0);
	AddAnimationComponent(anim_HighBay1Door, 0,      0.3, &highBay1Door1);
	AddAnimationComponent(anim_HighBay1Door, 0,      0.3, &highBay1Door2);
	AddAnimationComponent(anim_HighBay1Door, 0.3,	 0.6, &highBay1Door3);
	AddAnimationComponent(anim_HighBay1Door, 0.3,    0.6, &highBay1Door4);
	AddAnimationComponent(anim_HighBay1Door, 0.7,    1.0, &highBay1Door5);
	AddAnimationComponent(anim_HighBay1Door, 0.7435, 1.0, &highBay1Door6);
	AddAnimationComponent(anim_HighBay1Door, 0.787,  1.0, &highBay1Door7);
	AddAnimationComponent(anim_HighBay1Door, 0.8305, 1.0, &highBay1Door8);
	AddAnimationComponent(anim_HighBay1Door, 0.87,   1.0, &highBay1Door9);
	AddAnimationComponent(anim_HighBay1Door, 0.91,   1.0, &highBay1Door10);
	AddAnimationComponent(anim_HighBay1Door, 0.95,   1.0, &highBay1Door11);

	// High bay 3 door
	static UINT highBay3Door1_groups[1] = {37};
	static MGROUP_TRANSLATE highBay3Door1 (0, highBay3Door1_groups, 1, _V(12, 0, 0));

	static UINT highBay3Door2_groups[1] = {39};
	static MGROUP_TRANSLATE highBay3Door2 (0, highBay3Door2_groups, 1, _V(-12, 0, 0));

	static UINT highBay3Door3_groups[2] = {39, 40};
	static MGROUP_TRANSLATE highBay3Door3 (0, highBay3Door3_groups, 2, _V(-12, 0, 0));

	static UINT highBay3Door4_groups[2] = {37, 38};
	static MGROUP_TRANSLATE highBay3Door4 (0, highBay3Door4_groups, 2, _V(12, 0, 0));

	static UINT highBay3Door5_groups[1] = {48};
	static MGROUP_TRANSLATE highBay3Door5 (0, highBay3Door5_groups, 1, _V(0, 106, 0));

	static UINT highBay3Door6_groups[1] = {47};
	static MGROUP_TRANSLATE highBay3Door6 (0, highBay3Door6_groups, 1, _V(0, 91.5, 0));

	static UINT highBay3Door7_groups[1] = {46};
	static MGROUP_TRANSLATE highBay3Door7 (0, highBay3Door7_groups, 1, _V(0, 77.36, 0));

	static UINT highBay3Door8_groups[1] = {45};
	static MGROUP_TRANSLATE highBay3Door8 (0, highBay3Door8_groups, 1, _V(0, 63, 0));

	static UINT highBay3Door9_groups[1] = {44};
	static MGROUP_TRANSLATE highBay3Door9 (0, highBay3Door9_groups, 1, _V(0, 48, 0));

	static UINT highBay3Door10_groups[1] = {43};
	static MGROUP_TRANSLATE highBay3Door10 (0, highBay3Door10_groups, 1, _V(0, 34, 0));

	static UINT highBay3Door11_groups[1] = {42};
	static MGROUP_TRANSLATE highBay3Door11 (0, highBay3Door11_groups, 1, _V(0, 19.73, 0));

	anim_HighBay3Door = CreateAnimation(0.0);
	AddAnimationComponent(anim_HighBay3Door, 0,      0.3, &highBay3Door1);
	AddAnimationComponent(anim_HighBay3Door, 0,      0.3, &highBay3Door2);
	AddAnimationComponent(anim_HighBay3Door, 0.3,    0.6, &highBay3Door3);
	AddAnimationComponent(anim_HighBay3Door, 0.3,    0.6, &highBay3Door4);
	AddAnimationComponent(anim_HighBay3Door, 0.7,    1.0, &highBay3Door5);
	AddAnimationComponent(anim_HighBay3Door, 0.7435, 1.0, &highBay3Door6);
	AddAnimationComponent(anim_HighBay3Door, 0.787,  1.0, &highBay3Door7);
	AddAnimationComponent(anim_HighBay3Door, 0.8305, 1.0, &highBay3Door8);
	AddAnimationComponent(anim_HighBay3Door, 0.87,   1.0, &highBay3Door9);
	AddAnimationComponent(anim_HighBay3Door, 0.91,   1.0, &highBay3Door10);
	AddAnimationComponent(anim_HighBay3Door, 0.95,   1.0, &highBay3Door11);

	// Saturn Stage 1
	DefineCraneAnimation(mgroupCrane[0], 110 + 18.5, 80.85 + 3.63);	
	DefineCrane2Animation(mgroupCrane2[0], 36.2, 78.5 - 8.65);

	mgroupSaturn[0][0] = new MGROUP_ROTATE    (meshindexSaturn[0], NULL, 0, _V(0, 0, 0), _V( 0, 1, 0), (float)(-0.5 * PI));
	mgroupSaturn[0][1] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(-78.5 + 8.65, 0, 0)); // _V(-78.5, 0, 0));	
	mgroupSaturn[0][2] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, 55.8, 0));
	mgroupSaturn[0][3] = new MGROUP_ROTATE    (meshindexSaturn[0], NULL, 0, _V(-135.2 + 8.65, 0, 0), _V(0, 0, 1), (float)(-0.5 * PI)); // _V(-135.2, 0, 0), _V(0, 0, 1), (float)(-0.5 * PI));
	mgroupSaturn[0][4] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, 55.0 + 18.38 , 0));
	mgroupSaturn[0][5] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, 0, 42.5 + 0.45));
	mgroupSaturn[0][6] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, -(80.85 + 3.63 - 3.0), 0));
	mgroupSaturn[0][7] = new MGROUP_TRANSLATE (meshindexSaturn[0], NULL, 0, _V(0, -3, 0));

	// Saturn Stage 2
	DefineCraneAnimation(mgroupCrane[1], 107.4 + 3.0, 56.9 + 0.29 + 3.0);
	DefineCrane2Animation(mgroupCrane2[1], 23.4, 99.7 - 8.65);
	DefineSaturnAnimation(mgroupSaturn[1], meshindexSaturn[1], 107.4 + 3.0, 56.9 + 0.29 + 3.0);
	DefineSaturnAnimation(mgroupSaturn[2], meshindexSaturn[2], 107.4 + 3.0, 56.9 + 0.29 + 3.0);

	// Saturn Stage 3
	DefineCraneAnimation(mgroupCrane[2], 113.8 - 7.0, 49.4 - 7.0);
	DefineCrane2Animation(mgroupCrane2[2], 20, 99.7 - 8.65);
	DefineSaturnAnimation(mgroupSaturn[3], meshindexSaturn[3], 113.8 - 7.0, 49.4 - 7.0);
	DefineSaturnAnimation(mgroupSaturn[4], meshindexSaturn[4], 113.8 - 7.0, 49.4 - 7.0);

	// Saturn Stage CSM/LTA
	DefineCraneAnimation(mgroupCrane[3], 113.0 - 1.9, 22 + 5.35 - 1.9);
	DefineCrane2Animation(mgroupCrane2[3], 24.2, 99.7 - 8.65);
	for (i = 5; i < 23; i++) 
		DefineSaturnAnimation(mgroupSaturn[i], meshindexSaturn[i], 113 - 1.9, 22 + 5.35 - 1.9);

	// Saturn Stage Tower
	DefineCraneAnimation(mgroupCrane[4], 114.5 - 7.1, 13.5 + 10.4 - 7.1);
	DefineCrane2Animation(mgroupCrane2[4], 20.5, 99.7 - 8.65);
	DefineSaturnAnimation(mgroupSaturn[23], meshindexSaturn[23], 114.5 - 7.1, 13.5 + 10.4 - 7.1);

	// Create animations
	animCraneCount = 5;
	for (i = 0; i < animCraneCount; i++) {
		animCrane[i] = CreateAnimation(0.0);
		AddAnimationComponent(animCrane[i], 0,        0.000005, mgroupCrane[i][0]);
		AddAnimationComponent(animCrane[i], 0,        0.000005, mgroupCrane[i][1]);
		AddAnimationComponent(animCrane[i], 0.000005, 0.00001,  mgroupCrane[i][2]);
		AddAnimationComponent(animCrane[i], 0.1876,   0.2,      mgroupCrane[i][3]);
		AddAnimationComponent(animCrane[i], 0.2,  0.3, mgroupCrane[i][4]);
		AddAnimationComponent(animCrane[i], 0.2,  0.3, mgroupCrane[i][5]);
		AddAnimationComponent(animCrane[i], 0.3,  0.4, mgroupCrane[i][6]);
		AddAnimationComponent(animCrane[i], 0.3,  0.4, mgroupCrane[i][7]);
		AddAnimationComponent(animCrane[i], 0.4,  0.6, mgroupCrane[i][8]);
		AddAnimationComponent(animCrane[i], 0.6,  0.7, mgroupCrane[i][9]);
		AddAnimationComponent(animCrane[i], 0.6,  0.7, mgroupCrane[i][10]); 
		AddAnimationComponent(animCrane[i], 0.7,  0.8, mgroupCrane[i][11]); 
		AddAnimationComponent(animCrane[i], 0.7,  0.8, mgroupCrane[i][12]); 
		AddAnimationComponent(animCrane[i], 0.8,  0.9, mgroupCrane[i][13]); 
		AddAnimationComponent(animCrane[i], 0.8,  0.9, mgroupCrane[i][14]); 
		AddAnimationComponent(animCrane[i], 0.9,  1.0, mgroupCrane[i][15]); 

		AddAnimationComponent(animCrane[i], 0,       0.00001, mgroupCrane2[i][0]);
		AddAnimationComponent(animCrane[i], 0,       0.00001, mgroupCrane2[i][1]);
		AddAnimationComponent(animCrane[i], 0,       0.00001, mgroupCrane2[i][2]);
		AddAnimationComponent(animCrane[i], 0,       0.00001, mgroupCrane2[i][3]);
		AddAnimationComponent(animCrane[i], 0.00001, 0.2,     mgroupCrane2[i][4]);
		AddAnimationComponent(animCrane[i], 0.2,     0.20272, mgroupCrane2[i][5]);
		AddAnimationComponent(animCrane[i], 0.2,     0.20272, mgroupCrane2[i][6]);
		AddAnimationComponent(animCrane[i], 0.20272, 0.233,   mgroupCrane2[i][7]);
	}

	int j = 0;
	for (i = 1; i < meshcountSaturn; i++) {
		if (i <= 2) j = 1;
		else if (i <= 4) j = 2;
		else if (i <= 22) j = 3;
		else if (i <= 23) j = 4;

		AddAnimationComponent(animCrane[j], 0,        0.000005, mgroupSaturn[i][0]);
		AddAnimationComponent(animCrane[j], 0.000005, 0.00001,  mgroupSaturn[i][1]);
		AddAnimationComponent(animCrane[j], 0.00001,  0.2,      mgroupSaturn[i][2]);
		AddAnimationComponent(animCrane[j], 0.3,      0.4,      mgroupSaturn[i][3]);
		AddAnimationComponent(animCrane[j], 0.4,      0.6,      mgroupSaturn[i][4]);
		AddAnimationComponent(animCrane[j], 0.6,      0.7,      mgroupSaturn[i][5]);
		AddAnimationComponent(animCrane[j], 0.7,      0.8,      mgroupSaturn[i][6]);
	}
	currentAnimCrane = animCrane[0];

	// Special handling Saturn Stage 1
	animSaturnStage1 = CreateAnimation(0.0);
	AddAnimationComponent(animSaturnStage1, 0,        0.00001, mgroupSaturn[0][0]);
	AddAnimationComponent(animSaturnStage1, 0.00001,  0.2,     mgroupSaturn[0][1]);
	ANIMATIONCOMPONENT_HANDLE ach = 
    AddAnimationComponent(animSaturnStage1, 0.3,      0.3428,  mgroupSaturn[0][2]);
	AddAnimationComponent(animSaturnStage1, 0.3,      0.3428,  mgroupSaturn[0][3], ach);
	AddAnimationComponent(animSaturnStage1, 0.3428,   0.4,     mgroupSaturn[0][4]);
	AddAnimationComponent(animSaturnStage1, 0.4,      0.6,     mgroupSaturn[0][5]);
	AddAnimationComponent(animSaturnStage1, 0.6,      0.7,     mgroupSaturn[0][6]);
	AddAnimationComponent(animSaturnStage1, 0.7,      0.8,     mgroupSaturn[0][7]);

	// Platforms
	static UINT platform0L_groups[1] = {20};
	static UINT platform0R_groups[1] = {25};
	static UINT platform1L_groups[1] = {21};
	static UINT platform1R_groups[1] = {26};
	static UINT platform2L_groups[1] = {22};
	static UINT platform2R_groups[1] = {27};
	static UINT platform3L_groups[1] = {23};
	static UINT platform3R_groups[1] = {28};
	static UINT platform4L_groups[1] = {24};
	static UINT platform4R_groups[1] = {29};
	static MGROUP_TRANSLATE platform0L(0, platform0L_groups, 1, _V(-10.4, 0, 0));
	static MGROUP_TRANSLATE platform0R(0, platform0R_groups, 1, _V(10.4, 0, 0));
	static MGROUP_TRANSLATE platform1L(0, platform1L_groups, 1, _V(-10.4, 0, 0));
	static MGROUP_TRANSLATE platform1R(0, platform1R_groups, 1, _V(10.4, 0, 0));
	static MGROUP_TRANSLATE platform2L(0, platform2L_groups, 1, _V(-10.4, 0, 0));
	static MGROUP_TRANSLATE platform2R(0, platform2R_groups, 1, _V(10.4, 0, 0));
	static MGROUP_TRANSLATE platform3L(0, platform3L_groups, 1, _V(-10.4, 0, 0));
	static MGROUP_TRANSLATE platform3R(0, platform3R_groups, 1, _V(10.4, 0, 0));
	static MGROUP_TRANSLATE platform4L(0, platform4L_groups, 1, _V(-10.4, 0, 0));
	static MGROUP_TRANSLATE platform4R(0, platform4R_groups, 1, _V(10.4, 0, 0));

	animPlatform[0] = CreateAnimation(0.0);
	AddAnimationComponent(animPlatform[0], 0, 1.0, &platform0L);
	AddAnimationComponent(animPlatform[0], 0, 1.0, &platform0R);
	animPlatform[1] = CreateAnimation(0.0);
	AddAnimationComponent(animPlatform[1], 0, 1.0, &platform1L);
	AddAnimationComponent(animPlatform[1], 0, 1.0, &platform1R);
	animPlatform[2] = CreateAnimation(0.0);
	AddAnimationComponent(animPlatform[2], 0, 1.0, &platform2L);
	AddAnimationComponent(animPlatform[2], 0, 1.0, &platform2R);
	animPlatform[3] = CreateAnimation(0.0);
	AddAnimationComponent(animPlatform[3], 0, 1.0, &platform3L);
	AddAnimationComponent(animPlatform[3], 0, 1.0, &platform3R);
	animPlatform[4] = CreateAnimation(0.0);
	AddAnimationComponent(animPlatform[4], 0, 1.0, &platform4L);
	AddAnimationComponent(animPlatform[4], 0, 1.0, &platform4R);
}

void VAB::DefineCraneAnimation(MGROUP_TRANSFORM *mgroup[], double pickupHeight, double releaseHeight) {

	// Crane
	static UINT crane1_groups[1] = {4};
	static UINT crane2_groups[2] = {1, 2};
	static UINT crane3_groups[4] = {4, 1, 3, 2};
	static UINT crane4_groups[5] = {4, 1, 3, 5, 2};
	static UINT crane5_groups[1] = {4};
	static UINT crane6_groups[2] = {1, 2};
	static UINT crane7_groups[1] = {4};
	static UINT crane8_groups[2] = {1, 2};
	static UINT crane9_groups[5] = {4, 1, 3, 5, 2};
	static UINT crane10_groups[1] = {4};
	static UINT crane11_groups[2] = {1, 2};
	static UINT crane12_groups[1] = {4};
	static UINT crane13_groups[2] = {1, 2};
	static UINT crane14_groups[1] = {4};
	static UINT crane15_groups[2] = {1, 2};
	static UINT crane16_groups[5] = {4, 1, 3, 5, 2};

	mgroup[0]  = new MGROUP_SCALE     (0, crane1_groups,  1, _V(0, 154.7,    0), _V(1, 0.1, 1));
	mgroup[1]  = new MGROUP_TRANSLATE (0, crane2_groups,  2, _V(0, 114.6,  0));	
	mgroup[2]  = new MGROUP_TRANSLATE (0, crane3_groups,  4, _V(0, 0, 0)); 
	mgroup[3]  = new MGROUP_TRANSLATE (0, crane4_groups,  5, _V(0, 0, 0.1)); 
	mgroup[4]  = new MGROUP_SCALE     (0, crane5_groups,  1, _V(0, 154.7,  0), _V(1, (pickupHeight / 114.6) * 9.0 + 1.0, 1));
	mgroup[5]  = new MGROUP_TRANSLATE (0, crane6_groups,  2, _V(0, -pickupHeight,    0));
	mgroup[6]  = new MGROUP_SCALE     (0, crane7_groups,  1, _V(0, 154.7,  0), _V(1, 1.0 / ((pickupHeight / 114.6) * 9.0 + 1.0), 1));
	mgroup[7]  = new MGROUP_TRANSLATE (0, crane8_groups,  2, _V(0, pickupHeight,    0));
	mgroup[8]  = new MGROUP_TRANSLATE (0, crane9_groups,  5, _V(0, 0, 42.5 + 0.45));
	mgroup[9]  = new MGROUP_SCALE     (0, crane10_groups, 1, _V(0, 154.7,  0), _V(1, ((releaseHeight - 3.0) / 114.6) * 9.0 + 1.0, 1));
	mgroup[10] = new MGROUP_TRANSLATE (0, crane11_groups, 2, _V(0, -(releaseHeight - 3.0), 0));
	mgroup[11] = new MGROUP_SCALE     (0, crane12_groups, 1, _V(0, 154.7,  0), _V(1, (releaseHeight + 13.63) / ((releaseHeight - 3.0) + 13.63), 1));
	mgroup[12] = new MGROUP_TRANSLATE (0, crane13_groups, 2, _V(0, -3,  0));
	mgroup[13] = new MGROUP_SCALE     (0, crane14_groups, 1, _V(0, 154.7,  0), _V(1, 1.0 / ((releaseHeight / 114.6) * 9.0 + 1.0), 1));
	mgroup[14] = new MGROUP_TRANSLATE (0, crane15_groups, 2, _V(0, releaseHeight, 0));
	mgroup[15] = new MGROUP_TRANSLATE (0, crane16_groups, 5, _V(0, 0, -42.5 - 0.45 - 0.1));
}

void VAB::DefineSaturnAnimation(MGROUP_TRANSFORM *mgroup[], int meshindex, double pickupHeight, double releaseHeight) {

	mgroup[0] = new MGROUP_ROTATE    (meshindex, NULL, 0, _V(0, 0, 0), _V(-1, 0, 0), (float)( 0.5 * PI));
	mgroup[1] = new MGROUP_ROTATE    (meshindex, NULL, 0, _V(0, 0, 0), _V( 0, 1, 0), (float)(-0.5 * PI));
	mgroup[2] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(-99.7 + 8.65, 0, 0));
	mgroup[3] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, pickupHeight, 0));
	mgroup[4] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, 0, 42.5 + 0.45));
	mgroup[5] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, -(releaseHeight - 3.0), 0));
	mgroup[6] = new MGROUP_TRANSLATE (meshindex, NULL, 0, _V(0, -3, 0));
}

void VAB::DefineCrane2Animation(MGROUP_TRANSFORM *mgroup[], double pickupHeight, double length) {

	// Crane 2
	static UINT crane0_groups[4] = {12, 9, 10, 11};	
	static UINT crane1_groups[5] = {12, 9, 10, 11, 13};
	static UINT crane2_groups[1] = {12};
	static UINT crane3_groups[2] = {9, 10};
	static UINT crane4_groups[5] = {12, 9, 10, 11, 13};
	static UINT crane5_groups[1] = {12};
	static UINT crane6_groups[2] = {9, 10};
	static UINT crane7_groups[5] = {12, 9, 10, 11, 13};

	mgroup[0] = new MGROUP_TRANSLATE (0, crane0_groups,  4, _V(0, 0, -1));
	mgroup[1] = new MGROUP_TRANSLATE (0, crane1_groups,  5, _V(120, 0, 0));
	mgroup[2] = new MGROUP_SCALE     (0, crane2_groups,  1, _V(0, 55.1, 0), _V(1, pickupHeight / 43.5, 1));
	mgroup[3] = new MGROUP_TRANSLATE (0, crane3_groups,  2, _V(0, 43.5 - pickupHeight,  0));
	mgroup[4] = new MGROUP_TRANSLATE (0, crane4_groups,  5, _V(-length, 0, 0));
	mgroup[5] = new MGROUP_SCALE     (0, crane5_groups,  1, _V(0, 55.1, 0), _V(1, (pickupHeight - 3.0) / pickupHeight, 1));
	mgroup[6] = new MGROUP_TRANSLATE (0, crane6_groups,  2, _V(0, 3, 0));
	mgroup[7] = new MGROUP_TRANSLATE (0, crane7_groups,  5, _V(length, 0, 0));
}

void VAB::clbkPostCreation() {

	//
	// High bay doors
	//
	SetAnimation(anim_HighBay1Door, highBay1Door_Proc);
	SetAnimation(anim_HighBay3Door, highBay3Door_Proc);
	
	//
	// Saturn 5 assembly 
	//
	if (currentAnimCrane >= 0)
		SetAnimation(currentAnimCrane, crane_Proc);
	else
		SetAnimation(animCrane[1], crane_Proc);

	SetAnimation(animSaturnStage1, saturnStage1_Proc);

	int i;
	for (i = 0; i < 5; i++) {
		SetAnimation(animPlatform[i], platform_Proc[i]);
	}

	// Get the saturn
	Saturn *lav = 0;
	if (LVName[0] != '\0') {
		OBJHANDLE hLV = oapiGetVesselByName(LVName);
		if (hLV) {
			lav = (Saturn *) oapiGetVesselInterface(hLV);
		}
	}
	if (lav && LVVisible) {
		SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_ALWAYS);	
	}
	if (lav && !LVVisible) {
		SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_NEVER);	
	}
}

void VAB::clbkPreStep(double simt, double simdt, double mjd) {

	if (!firstTimestepDone) DoFirstTimestep();
}

void VAB::clbkPostStep (double simt, double simdt, double mjd) {

	int i = 0;

	// High bay 1 door
	if (highBay1Door_Status == DOOR_CLOSING || highBay1Door_Status == DOOR_OPENING) {
		double da = simdt / 90.0;
		if (highBay1Door_Status == DOOR_CLOSING) {
			if (highBay1Door_Proc > 0.0)
				highBay1Door_Proc = max(0.0, highBay1Door_Proc - da);
			else
				highBay1Door_Status = DOOR_CLOSED;

		} else { // door opening
			if (highBay1Door_Proc < 1.0)
				highBay1Door_Proc = min(1.0, highBay1Door_Proc + da);
			else
				highBay1Door_Status = DOOR_OPEN;
		}
		SetAnimation(anim_HighBay1Door, highBay1Door_Proc);
	}

	// High bay 3 door
	if (highBay3Door_Status == DOOR_CLOSING || highBay3Door_Status == DOOR_OPENING) {
		double da = simdt / 90.0;
		if (highBay3Door_Status == DOOR_CLOSING) {
			if (highBay3Door_Proc > 0.0)
				highBay3Door_Proc = max(0.0, highBay3Door_Proc - da);
			else
				highBay3Door_Status = DOOR_CLOSED;

		} else { // door opening
			if (highBay3Door_Proc < 1.0)
				highBay3Door_Proc = min(1.0, highBay3Door_Proc + da);
			else
				highBay3Door_Status = DOOR_OPEN;
		}
		SetAnimation(anim_HighBay3Door, highBay3Door_Proc);
	}

	// Crane
	if (crane_Status == CRANE_BUILDING || crane_Status == CRANE_UNBUILDING) {
		// Platforms
		if (currentAnimCrane == -1) {
			ML *ml = 0;
			OBJHANDLE hML = oapiGetVesselByName("ML");
			if (hML) {
				ml = (ML *) oapiGetVesselInterface(hML);
			}
			if (crane_Status == CRANE_BUILDING) {
				if (platform_Proc[0] > 0) {
					for (i = 0; i < 5; i++) {
						platform_Proc[i] = max(0.0, platform_Proc[i] - simdt / 1000.0);
					}
				} else {
					if (ml) {
						ml->SetVABReadyState();
					}
					crane_Status = CRANE_END;
				}

			} else { // unbuilding
				if (platform_Proc[0] < 1.0) {
					for (i = 0; i < 5; i++) {
						platform_Proc[i] = min(1.0, platform_Proc[i] + simdt / 1000.0);
					}
					if (ml) {
						ml->SetVABBuildState();
					}
				} else 
					crane_Status = CRANE_BEGIN;
			} 
		} else {
			// Get the saturn
			Saturn *lav = 0;
			if (LVName[0] != '\0') {
				OBJHANDLE hLV = oapiGetVesselByName(LVName);
				if (hLV) {
					lav = (Saturn *) oapiGetVesselInterface(hLV);
				}
			}

			// Calculate animation speed
			double da;
			if (crane_Proc < 0.2) da = 0.2 * simdt / 2000.0;
			else if (crane_Proc < 0.4) da = 0.1 * simdt / 6600.0;
			else if (crane_Proc < 0.6) da = 0.2 * simdt / 850.0;
			else if (crane_Proc < 0.7) {
				if (currentAnimCrane == (int) animCrane[0]) da = 0.1 * simdt / 4695.0;
				if (currentAnimCrane == (int) animCrane[1]) da = 0.1 * simdt / 2910.0;
				if (currentAnimCrane == (int) animCrane[2]) da = 0.1 * simdt / 1995.0;
				if (currentAnimCrane == (int) animCrane[3]) da = 0.1 * simdt / 1140.0;
				if (currentAnimCrane == (int) animCrane[4]) da = 0.1 * simdt / 630.0;
			} else if (crane_Proc < 0.8) da = 0.1 * simdt / 720.0;
			else if (crane_Proc < 0.9) {
				if (currentAnimCrane == (int) animCrane[0]) da = 0.1 * simdt / (4695.0 + 180.0);
				if (currentAnimCrane == (int) animCrane[1]) da = 0.1 * simdt / (2910.0 + 180.0);
				if (currentAnimCrane == (int) animCrane[2]) da = 0.1 * simdt / (1995.0 + 180.0);
				if (currentAnimCrane == (int) animCrane[3]) da = 0.1 * simdt / (1140.0 + 180.0);
				if (currentAnimCrane == (int) animCrane[4]) da = 0.1 * simdt / (630.0 + 180.0);					
			} else da = 0.1 * simdt / 726.0;

			if (crane_Status == CRANE_BUILDING) {
				if (crane_Proc < 1.0)
					crane_Proc = min(1.0, crane_Proc + da);
				else
					crane_Status = CRANE_END;

				if (!LVVisible && crane_Proc > 0.00001 && crane_Proc < 0.8 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_ALWAYS);
					LVVisible = true;
				}
				
				if (LVVisible && crane_Proc >= 0.8 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_NEVER);
					lav->LaunchVehicleBuild();
					LVVisible = false;
				} 

				// Platforms
				if (lav && crane_Proc > 0.8 && platform_Proc[lav->GetBuildStatus() - 1] < 1.0)
					platform_Proc[lav->GetBuildStatus() - 1] = min(1.0, platform_Proc[lav->GetBuildStatus() - 1] + simdt / 1000.0);

				// Saturn Stage 1
				if (currentAnimCrane == (int) animCrane[0])
					saturnStage1_Proc = crane_Proc;

			} else { // unbuilding
				if (crane_Proc > 0.00001)
					crane_Proc = max(0.00001, crane_Proc - da);
				else
					crane_Status = CRANE_BEGIN;

				if (!LVVisible && crane_Proc > 0.00001 && crane_Proc < 0.8 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus() - 1, MESHVIS_ALWAYS);
					lav->LaunchVehicleUnbuild();
					LVVisible = true;
				} 
			
				if (LVVisible && crane_Proc <= 0.00001 && lav) {
					SetSaturnMeshVisibilityMode(lav->GetBuildStatus(), MESHVIS_NEVER);
					LVVisible = false;
				}

				// Platforms
				if (lav &&  crane_Proc < 0.95 && crane_Proc > 0.8 && platform_Proc[lav->GetBuildStatus() - 1] > 0)
					platform_Proc[lav->GetBuildStatus() - 1] = max(0, platform_Proc[lav->GetBuildStatus() - 1] - simdt / 1000.0);

				// Saturn Stage 1
				if (currentAnimCrane == (int) animCrane[0]) {
					if (adjustSaturnStage1) {
						if (saturnStage1_Proc <= 0.35) {
							saturnStage1_Proc += da / 3.9;
						} else {
							saturnStage1_Proc = 1.0;
							adjustSaturnStage1 = false;
						}
					} else {
						saturnStage1_Proc = crane_Proc;
					}
				}
			}
			SetAnimation(currentAnimCrane, crane_Proc);
			SetAnimation(animSaturnStage1, saturnStage1_Proc);
		}
		for (i = 0; i < 5; i++) {
			SetAnimation(animPlatform[i], platform_Proc[i]);
		}
	}
	// sprintf(oapiDebugString(), "platform_Proc %f crane_Status %d", platform_Proc, crane_Status);		
}
			
void VAB::SetSaturnMeshVisibilityMode(int buildStatus, WORD mode) {

	int i;

	if (buildStatus == 0) {
		SetMeshVisibilityMode(meshindexSaturn[0], mode);
	}
	else if (buildStatus == 1) {
		SetMeshVisibilityMode(meshindexSaturn[1], mode);
		SetMeshVisibilityMode(meshindexSaturn[2], mode);
	}
	else if (buildStatus == 2) {
		SetMeshVisibilityMode(meshindexSaturn[3], mode);
		SetMeshVisibilityMode(meshindexSaturn[4], mode);
	}
	else if (buildStatus == 3) {
		for (i = 5; i < 23; i++) 
			SetMeshVisibilityMode(meshindexSaturn[i], mode);
	}
	else if (buildStatus == 4) {
		SetMeshVisibilityMode(meshindexSaturn[23], mode);
	}
}

void VAB::DoFirstTimestep() {

	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

	firstTimestepDone = true;
}

void VAB::clbkLoadStateEx(FILEHANDLE scn, void *status) {

	int i;
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp (line, "LVNAME", 6)) {
			strncpy (LVName, line + 7, 64);
		} else if (!strnicmp (line, "CURRENTANIMCRANE", 16)) {
			sscanf (line + 16, "%i", &currentAnimCrane);
		} else if (!strnicmp (line, "ADJUSTSATURNSTAGE1", 18)) {
			sscanf (line + 18, "%i", &i);
			adjustSaturnStage1 = (i != 0);
		} else if (!strnicmp (line, "CRANESTATUS", 11)) {
			sscanf (line + 11, "%i", &crane_Status);
		} else if (!strnicmp (line, "HIGHBAY1DOORSTATUS", 18)) {
			sscanf (line + 18, "%i", &highBay1Door_Status);
		} else if (!strnicmp (line, "HIGHBAY3DOORSTATUS", 18)) {
			sscanf (line + 18, "%i", &highBay3Door_Status);
		} else if (!strnicmp (line, "LVVISIBLE", 9)) {
			sscanf (line + 9, "%i", &LVVisible);
		} else if (!strnicmp (line, "CRANEPROC", 9)) {
			sscanf (line + 9, "%lf", &crane_Proc);
		} else if (!strnicmp (line, "SATURNSTAGE1PROC", 16)) {
			sscanf (line + 16, "%lf", &saturnStage1_Proc);
		} else if (!strnicmp (line, "HIGHBAY1DOORPROC", 16)) {
			sscanf (line + 16, "%lf", &highBay1Door_Proc);
		} else if (!strnicmp (line, "HIGHBAY3DOORPROC", 16)) {
			sscanf (line + 16, "%lf", &highBay3Door_Proc);
		} else if (!strnicmp (line, "PLATFORMPROC0", 13)) {
			sscanf (line + 13, "%lf", &platform_Proc[0]);
		} else if (!strnicmp (line, "PLATFORMPROC1", 13)) {
			sscanf (line + 13, "%lf", &platform_Proc[1]);
		} else if (!strnicmp (line, "PLATFORMPROC2", 13)) {
			sscanf (line + 13, "%lf", &platform_Proc[2]);
		} else if (!strnicmp (line, "PLATFORMPROC3", 13)) {
			sscanf (line + 13, "%lf", &platform_Proc[3]);
		} else if (!strnicmp (line, "PLATFORMPROC4", 13)) {
			sscanf (line + 13, "%lf", &platform_Proc[4]);
		} else {
			ParseScenarioLineEx (line, status);
		}
	}
}

void VAB::clbkSaveState(FILEHANDLE scn) {

	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int(scn, "CURRENTANIMCRANE", currentAnimCrane);
	oapiWriteScenario_int(scn, "ADJUSTSATURNSTAGE1", (adjustSaturnStage1 ? 1 : 0));
	oapiWriteScenario_int(scn, "CRANESTATUS", crane_Status);
	oapiWriteScenario_int(scn, "HIGHBAY1DOORSTATUS", highBay1Door_Status);
	oapiWriteScenario_int(scn, "HIGHBAY3DOORSTATUS", highBay3Door_Status);
	oapiWriteScenario_int(scn, "LVVISIBLE", LVVisible);
	papiWriteScenario_double(scn, "CRANEPROC", crane_Proc);
	papiWriteScenario_double(scn, "SATURNSTAGE1PROC", saturnStage1_Proc);
	papiWriteScenario_double(scn, "HIGHBAY1DOORPROC", highBay1Door_Proc);
	papiWriteScenario_double(scn, "HIGHBAY3DOORPROC", highBay3Door_Proc);
	papiWriteScenario_double(scn, "PLATFORMPROC0", platform_Proc[0]);
	papiWriteScenario_double(scn, "PLATFORMPROC1", platform_Proc[1]);
	papiWriteScenario_double(scn, "PLATFORMPROC2", platform_Proc[2]);
	papiWriteScenario_double(scn, "PLATFORMPROC3", platform_Proc[3]);
	papiWriteScenario_double(scn, "PLATFORMPROC4", platform_Proc[4]);
	if (LVName[0])
		oapiWriteScenario_string (scn, "LVNAME", LVName);
}

int VAB::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	if (!firstTimestepDone) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0;
	}

	if (key == OAPI_KEY_NUMPAD7 && down == true) {
		ToggleHighBay1Door();			
		return 1;
	}
	if (key == OAPI_KEY_NUMPAD8 && down == true) {
		ToggleHighBay3Door();			
		return 1;
	}
	if (key == OAPI_KEY_B && down == true) {
		BuildSaturnStage();
		return 1;
	}
	if (key == OAPI_KEY_U && down == true) {
		UnbuildSaturnStage();
	}

	return 0;
}


int VAB::clbkConsumeDirectKey(char *kstate) {

	if (KEYMOD_SHIFT(kstate)) {
		return 0; 
	}

	/*
	double moveStep = 1.0e-7;
	if (KEYMOD_CONTROL(kstate))
		moveStep = 1.0e-9;

	VESSELSTATUS vs;
	GetStatus(vs);

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
		SetTouchdownPoints (_V(0, -GetCOG_elev() + 0.01, 1), _V(-1, -GetCOG_elev() + 0.01, -1), _V(1, -GetCOG_elev() + 0.01, -1));
		sprintf(oapiDebugString(), "COG_elev %f", GetCOG_elev());
		RESETKEY(kstate, OAPI_KEY_A);
	}
	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		SetTouchdownPoints (_V(0, -GetCOG_elev() - 0.01, 1), _V(-1, -GetCOG_elev() - 0.01, -1), _V(1, -GetCOG_elev() - 0.01, -1));
		sprintf(oapiDebugString(), "COG_elev %f", GetCOG_elev());
		RESETKEY(kstate, OAPI_KEY_S);
	}
	*/	

	return 0;
}

