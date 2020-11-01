/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Saturn 5
  This file holds the mesh-related functions.

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

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "lemcomputer.h"

#include "saturn.h"
#include "saturnv.h"
#include "tracer.h"
#include "LEM.h"

#include "sivb.h"
#include "sii.h"
#include "s1c.h"
#include "sm.h"
#include "Sat5Abort1.h"
#include "Sat5Abort2.h"
#include "Sat5Abort3.h"
#include "Mission.h"

static PARTICLESTREAMSPEC srb_contrail = {
	0, 
	12.0,	// size
	20,		// rate
	150.0,	// velocity
	0.3,	// velocity distribution
	3.0,	// lifetime
	4,		// growthrate
	3.5,	// atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_PLIN, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.8, 0.8
};

static PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	2.85,	// size
	1500,	// rate
	60.0,	// velocity
	0.1,	// velocity distribution
	1.5,	// lifetime
	2.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

static PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0, 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

//
// Spew out particles to simulate the junk thrown out by stage
// seperation explosives.
//

static PARTICLESTREAMSPEC seperation_junk = {
	0,		// flag
	0.04,	// size
	500,	// rate
	4.0,    // velocity
	5.0,    // velocity distribution
	30,		// lifetime
	0,	    // growthrate
	0,      // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

// "prelaunch tank venting" particle streams
static PARTICLESTREAMSPEC prelaunchvent1_spec = {
	0,		// flag
	0.6,	// size
	60,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	2,		// lifetime
	0.2,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.25, 0.25,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.25, 0.25
};

static PARTICLESTREAMSPEC prelaunchvent2_spec = {
	0,		// flag
	0.5,	// size
	80,		// rate
	3,		// velocity
	0.6,	// velocity distribution
	1,		// lifetime
	0.1,	// growthrate
	0.7,    // atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.2, 0.2,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.2, 0.2
};

static PARTICLESTREAMSPEC prelaunchvent3_spec = {
	0,		// flag
	0.4,	// size
	100,	// rate
	4,	    // velocity
	0.6,    // velocity distribution
	1,		// lifetime
	0.2,	// growthrate
	0.9,    // atmslowdown
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};

// "staging vent" particle streams
static PARTICLESTREAMSPEC stagingvent_spec = {
	0,		// flag
	2.5,	// size
	100,	// rate
	10,	    // velocity
	2,		// velocity distribution
	2,		// lifetime
	2.0,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.1, 0.1,
	PARTICLESTREAMSPEC::ATM_FLAT, 0.1, 0.1
};

// "fuel venting" particle streams
static PARTICLESTREAMSPEC fuel_venting_spec = {
	0,		// flag
	0.8,	// size
	30,		// rate
	2,	    // velocity
	0.5,    // velocity distribution
	20,		// lifetime
	0.15,	// growthrate
	0.5,    // atmslowdown 
	PARTICLESTREAMSPEC::DIFFUSE,
	PARTICLESTREAMSPEC::LVL_FLAT, 0.6, 0.6,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

static MESHHANDLE hsat5stg1;
static MESHHANDLE hsat5intstg;
static MESHHANDLE hsat5intstg4;
static MESHHANDLE hsat5intstg8;
static MESHHANDLE hsat5stg2;
static MESHHANDLE hsat5stg2base;
static MESHHANDLE hsat5stg2interstage;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg3base;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;

static MESHHANDLE hsat5stg1low;
static MESHHANDLE hsat5intstglow;
static MESHHANDLE hsat5stg2low;
static MESHHANDLE hsat5stg3low;
static MESHHANDLE hsat5stg31low;
static MESHHANDLE hsat5stg32low;
static MESHHANDLE hsat5stg33low;
static MESHHANDLE hsat5stg34low;

static MESHHANDLE hCONE;

static MESHHANDLE hApollocsm;

static MESHHANDLE hLMPKD;
static MESHHANDLE hapollo8lta;
static MESHHANDLE hlta_2r;

static SURFHANDLE contrail_tex;
static SURFHANDLE exhaust_tex;

#define LOAD_MESH(var, name) var = oapiLoadMeshGlobal(name);

void LoadSat5Meshes()

{
	LOAD_MESH(hsat5stg1low, "ProjectApollo/LowRes/sat5stg1");
	LOAD_MESH(hsat5stg1, "ProjectApollo/sat5stg1");

	LOAD_MESH(hsat5intstglow, "ProjectApollo/LowRes/sat5intstg");
	LOAD_MESH(hsat5intstg, "ProjectApollo/sat5intstg");
	LOAD_MESH(hsat5intstg4, "ProjectApollo/sat5intstg4");
	LOAD_MESH(hsat5intstg8, "ProjectApollo/sat5intstg8");

	LOAD_MESH(hsat5stg2low, "ProjectApollo/LowRes/sat5stg2");
	LOAD_MESH(hsat5stg2, "ProjectApollo/sat5stg2");
	LOAD_MESH(hsat5stg2base, "ProjectApollo/sat5stg2base");
	LOAD_MESH(hsat5stg2interstage, "ProjectApollo/sat5stg2intstg");

	LOAD_MESH(hsat5stg3low, "ProjectApollo/LowRes/sat5stg3");
	LOAD_MESH(hsat5stg31low, "ProjectApollo/LowRes/sat5stg31");
	LOAD_MESH(hsat5stg32low, "ProjectApollo/LowRes/sat5stg32");
	LOAD_MESH(hsat5stg33low, "ProjectApollo/LowRes/sat5stg33");
	LOAD_MESH(hsat5stg34low, "ProjectApollo/LowRes/sat5stg34");

	LOAD_MESH(hsat5stg3, "ProjectApollo/sat5stg3");
	LOAD_MESH(hsat5stg3base, "ProjectApollo/sat5stg3base");
	LOAD_MESH(hsat5stg31, "ProjectApollo/sat5stg31");
	LOAD_MESH(hsat5stg32, "ProjectApollo/sat5stg32");
	LOAD_MESH(hsat5stg33, "ProjectApollo/sat5stg33");
	LOAD_MESH(hsat5stg34, "ProjectApollo/sat5stg34");

	LOAD_MESH(hLMPKD, "ProjectApollo/LM_SLA");
	LOAD_MESH(hapollo8lta, "ProjectApollo/apollo8_lta");
	LOAD_MESH(hlta_2r, "ProjectApollo/LTA_2R");

	contrail_tex = oapiRegisterParticleTexture("Contrail2");
	exhaust_tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");

	srb_exhaust.tex = contrail_tex;
	solid_exhaust.tex = contrail_tex;
	seperation_junk.tex = oapiRegisterParticleTexture("ProjectApollo/junk");;
}

void SaturnV::SetupMeshes()

{
	if (LowRes)
	{
		hStage1Mesh = hsat5stg1low;
		hStage2Mesh = hsat5stg2low;
		hStage3Mesh = hsat5stg3low;
		hStageSLA1Mesh = hsat5stg31low;
		hStageSLA2Mesh = hsat5stg32low;
		hStageSLA3Mesh = hsat5stg33low;
		hStageSLA4Mesh = hsat5stg34low;
	}
	else
	{
		hStage1Mesh = hsat5stg1;
		hStage2Mesh = hsat5stg2;
		hStage3Mesh = hsat5stg3;
		hStageSLA1Mesh = hsat5stg31;
		hStageSLA2Mesh = hsat5stg32;
		hStageSLA3Mesh = hsat5stg33;
		hStageSLA4Mesh = hsat5stg34;
	}
}

MESHHANDLE SaturnV::GetInterstageMesh()

{
	if (LowRes)
		return hsat5intstglow;

	switch (SII_UllageNum)
	{
	case 4:
		return hsat5intstg4;

	case 8:
		return hsat5intstg8;

	default:
		return hsat5intstg;
	}
}

void SaturnV::ChangeSatVBuildState (int bstate)

{
	TRACESETUP("ChangeSatVBuildState");

	SetSize (59.5);
	SetEmptyMass (Stage1Mass);

	SetPMI (_V(1147,1147,216.68));
	SetCrossSections (_V(1129,1133,52.4));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
	ClearMeshes();
	VECTOR3 m_exhaust_pos1= {3,3,Offset1st};
	VECTOR3 mesh_dir=_V(0,0,-80.0+STG0O);

	if (bstate >=1)
	{
		mesh_dir=_V(0,0,-54.0+STG0O);
		AddMesh (hStage1Mesh, &mesh_dir);
	}

	if (bstate >=2)
	{
		mesh_dir=_V(0,0,-30.5+STG0O);
		AddMesh (GetInterstageMesh(), &mesh_dir);
	}

	if (bstate == 2)
	{
		mesh_dir=_V(0,0,-17.2+STG0O);
		AddMesh (hsat5stg2base, &mesh_dir);
	}

	if (bstate > 2)
	{
		mesh_dir=_V(0,0,-17.2+STG0O);
		AddMesh (hStage2Mesh, &mesh_dir);
	}

	if (bstate ==3 )
	{
		mesh_dir=_V(0,0,2.+STG0O);
		AddMesh (hsat5stg3base, &mesh_dir);
	}

	if (bstate > 3)
	{
		mesh_dir=_V(0,0,2.+STG0O);
		AddMesh (hStage3Mesh, &mesh_dir);
	}

	if (bstate >=4)
	{
		if (LEM_DISPLAY && (SIVBPayload == PAYLOAD_LEM))
		{
			mesh_dir=_V(0,0,12+STG0O);
			AddMesh (hLMPKD, &mesh_dir);
		}
	}

	if (bstate >=4)
	{
		mesh_dir=_V(-1.48,-1.48,14.55+STG0O);
		AddMesh (hStageSLA1Mesh, &mesh_dir);
	}

	if (bstate >=4)
	{
		mesh_dir=_V(1.48,-1.48,14.55+STG0O);
		AddMesh (hStageSLA2Mesh, &mesh_dir);
	}

	if (bstate >=4)
	{
		mesh_dir=_V(1.48,1.48,14.55+STG0O);
		AddMesh (hStageSLA3Mesh, &mesh_dir);
	}

	if (bstate >=4){
		mesh_dir=_V(-1.48,1.48,14.55+STG0O);
		AddMesh (hStageSLA4Mesh, &mesh_dir);
	}

	if (bstate >=4)
	{
		AddSM(19.1+STG0O, false);
	}

	if (bstate >=4)
	{
		mesh_dir=_V(0,0,23.25+STG0O);
		AddMesh (hCM, &mesh_dir);
	}

	if (bstate >=4)
	{
		mesh_dir=_V(0,0,23.25+STG0O);
		AddMesh (hCMInt, &mesh_dir);
	}

	if (bstate >=4)
	{
		mesh_dir=_V(0,0,23.25+STG0O);
		AddMesh (hFHC, &mesh_dir);
	}

	if (bstate >=4)
	{
		mesh_dir=_V(0,0,23.25+STG0O);
		AddMesh (hprobe, &mesh_dir);
	}

	if (bstate >=5)
	{
		mesh_dir=_V(0,0,28.2+STG0O);
		AddMesh (hsat5tower, &mesh_dir);
	}

	Offset1st = -60.1+STG0O;
	SetCameraOffset (_V(-1,1.0,23.1+STG0O));

	double TCP=-101.5+STG0O-TCPO;
	SetTouchdownPoints (_V(0,-100.0,TCP), _V(-7,7,TCP), _V(7,7,TCP));
}

void SaturnV::SetFirstStage ()

{
	TRACESETUP("SetFirstStage");

	double EmptyMass = Stage1Mass - (InterstageAttached ? 0.0 : Interstage_Mass) - (LESAttached ? 0.0 : Abort_Mass);

	//
	// *********************** physical parameters *********************************
	//

    SetSize (59.5);
	SetEmptyMass (EmptyMass);
	//440 is average value for pitch/yaw axis, 5.875 is highest value for roll axis (at cutoff). Could use some refining later.
	SetPMI(_V(440.0, 440.0, 5.875));
	SetCrossSections (_V(1129,1133,52.4));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag(_V(0.7, 0.7, 0.1));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);

	//
	// ************************ visual parameters **********************************
	//

	ClearMeshes();
	UINT meshidx;
	double TCP = -101.5 + STG0O - TCPO;

	double td_mass = Stage1Mass + SI_FuelMass;
	double td_width = 10.0;
	double td_tdph = TCP;
	double td_height = 110.0;

	ConfigTouchdownPoints(td_mass, td_width, td_tdph, td_height, -0.05);

	VECTOR3 mesh_dir=_V(0,0,-54.0+STG0O);
	meshidx = AddMesh (hStage1Mesh, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);

	if (SaturnType == SAT_INT20)
	{
		mesh_dir = _V(0, 0, 12.35-54.0+STG0O);
		meshidx = AddMesh (hsat5stg2interstage, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);
		SetThirdStageMesh(STG0O - 24.5);
	}
	else
	{
		SetSecondStageMesh(STG0O);
	}

	buildstatus = 6;

	EnableTransponder (true);

	//
	// **************************** NAV radios *************************************
	//

	InitNavRadios (4);
}

void SaturnV::SetFirstStageEngines ()

{
	int i;

	ClearThrusterDefinitions();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	//Add CSM RCS
	if (SaturnHasCSM())
	{
		AddRCSJets(41.37, SM_RCS_THRUST);
		AddRCS_CM(CM_RCS_THRUST, 43.65, false);
	}

	//
	// ************************* propellant specs **********************************
	//

	if (!ph_1st)
		ph_1st  = CreatePropellantResource(SI_FuelMass); //1st stage Propellant

	SetDefaultPropellantResource (ph_1st); // display 1st stage propellant level in generic HUD

	//
	// *********************** thruster definitions ********************************
	//

	Offset1st = -75.1+STG0O;
	VECTOR3 m_exhaust_ref = {0,0,-1};
    VECTOR3 MAIN4a_Vector= {3,3,Offset1st+0.5};
	VECTOR3 MAIN2a_Vector={-3,-3,Offset1st+0.5};
	VECTOR3 MAIN1a_Vector= {-3,3,Offset1st+0.5};
	VECTOR3 MAIN3a_Vector={3,-3,Offset1st+0.5};
	VECTOR3 MAIN5a_Vector={0,0,Offset1st+0.5};

	//
	// orbiter main thrusters
	//

	th_1st[0] = CreateThruster (MAIN4a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[1] = CreateThruster (MAIN2a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[2] = CreateThruster (MAIN1a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[3] = CreateThruster (MAIN3a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_1st[4] = CreateThruster (MAIN5a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_1st, ISP_FIRST_VAC, ISP_FIRST_SL);

	thg_1st = CreateThrusterGroup (th_1st, SI_EngineNum, THGROUP_MAIN);
	
	EXHAUSTSPEC es_1st[5] = {
		{ th_1st[0], NULL, NULL, NULL, 120.0, 3.5, 0, 0.1, exhaust_tex },
		{ th_1st[1], NULL, NULL, NULL, 120.0, 3.5, 0, 0.1, exhaust_tex },
		{ th_1st[2], NULL, NULL, NULL, 120.0, 3.5, 0, 0.1, exhaust_tex },
		{ th_1st[3], NULL, NULL, NULL, 120.0, 3.5, 0, 0.1, exhaust_tex },
		{ th_1st[4], NULL, NULL, NULL, 120.0, 3.5, 0, 0.1, exhaust_tex }
	};

	for (i = 0; i < SI_EngineNum; i++) AddExhaust(es_1st + i);

	AddExhaustStream (th_1st[0], MAIN4a_Vector+_V(0,0,-1), &srb_exhaust);
	AddExhaustStream (th_1st[1], MAIN2a_Vector+_V(0,0,-1), &srb_exhaust);
	AddExhaustStream (th_1st[2], MAIN1a_Vector+_V(0,0,-1), &srb_exhaust);
	AddExhaustStream (th_1st[3], MAIN3a_Vector+_V(0,0,-1), &srb_exhaust);
	AddExhaustStream (th_1st[4], MAIN5a_Vector+_V(0,0,-1), &srb_exhaust);

	// Contrail
	for (i = 0; i < SI_EngineNum; i++) {
		if (contrail[i]) {
			DelExhaustStream(contrail[i]);
			contrail[i] = NULL;
		}
	}
	contrail[0] = AddParticleStream(&srb_contrail, MAIN4a_Vector+_V(0,0,-25), _V( 0,0,-1), &contrailLevel);
	contrail[1] = AddParticleStream(&srb_contrail, MAIN2a_Vector+_V(0,0,-25), _V( 0,0,-1), &contrailLevel);
	contrail[2] = AddParticleStream(&srb_contrail, MAIN1a_Vector+_V(0,0,-25), _V( 0,0,-1), &contrailLevel);
	contrail[3] = AddParticleStream(&srb_contrail, MAIN3a_Vector+_V(0,0,-25), _V( 0,0,-1), &contrailLevel);
	contrail[4] = AddParticleStream(&srb_contrail, MAIN5a_Vector+_V(0,0,-25), _V( 0,0,-1), &contrailLevel);
}

void SaturnV::SetSecondStage ()
{
	TRACESETUP("SetSecondStage");

	ClearMeshes();

	double EmptyMass = Stage2Mass - (InterstageAttached ? 0.0 : Interstage_Mass) - (LESAttached ? 0.0 : Abort_Mass);

	SetSize (35.5);
	SetCOG_elev (15.225);
	SetEmptyMass (EmptyMass);
	SetPMI (_V(374,374,60));
	SetCrossSections (_V(524,524,97));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);

	double TCPSII = -28;

	double td_mass = Stage2Mass + (SII_FuelMass / 2);
	double td_width = 12.0;
	double td_tdph = TCPSII;
	double td_height = 64.0;

	ConfigTouchdownPoints(td_mass, td_width, td_tdph, td_height, -0.1);

	SetSecondStageMesh(-STG1O);
}

void SaturnV::SetSecondStageMesh(double offset)

{
	VECTOR3 mesh_dir;
	UINT meshidx;

	if (InterstageAttached)
	{
		mesh_dir=_V(0,0,-30.5 + offset);
		meshidx = AddMesh (GetInterstageMesh(), &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);
	}

	mesh_dir=_V(0,0,-17.2 + offset);
	meshidx = AddMesh (hStage2Mesh, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);

	SetThirdStageMesh(offset);
}

void SaturnV::SetSecondStageEngines(double offset)

{
    ClearThrusterDefinitions();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	if(ph_1st) {
		DelPropellantResource(ph_1st);
		ph_1st = 0;
	}

	//Add CSM RCS
	if (SaturnHasCSM())
	{
		AddRCSJets(26.22, SM_RCS_THRUST);
		AddRCS_CM(CM_RCS_THRUST, 28.5, false);
	}

	//
	// ************************* propellant specs **********************************
	//

	if (!ph_2nd)
		ph_2nd  = CreatePropellantResource(SII_FuelMass); //2nd stage Propellant
	SetDefaultPropellantResource (ph_2nd); // display 2nd stage propellant level in generic HUD

	//
	// *********************** thruster definitions ********************************
	//

	int i;

	VECTOR3 m_exhaust_pos1= {-1.8,-1.8,-33 + offset};
	VECTOR3 m_exhaust_pos2= {1.8,1.8,-33 + offset};
	VECTOR3 m_exhaust_pos3= {-1.8,1.8,-33 + offset};
	VECTOR3 m_exhaust_pos4 = {1.8,-1.8,-33 + offset};
	VECTOR3 m_exhaust_pos5 = {0,0,-33 + offset};
	VECTOR3 s_exhaust_pos = {0, 0, -35.0 + offset};

	VECTOR3	m_exhaust_pos6= _V(0,5.07,-33.15 + offset);
	VECTOR3 m_exhaust_pos7= _V(0,-5.07,-33.15 + offset);
	VECTOR3	m_exhaust_pos8= _V(5.07,0,-33.15 + offset);
	VECTOR3 m_exhaust_pos9= _V(-5.07,0,-33.15 + offset);
	VECTOR3	m_exhaust_pos10= _V(3.55,3.7,-33.15 + offset);
	VECTOR3 m_exhaust_pos11= _V(3.55,-3.7,-33.15 + offset);
	VECTOR3	m_exhaust_pos12= _V(-3.55,3.7,-33.15 + offset);
	VECTOR3 m_exhaust_pos13= _V(-3.55,-3.7,-33.15 + offset);

	//
	// Seperation 'thruster'.
	//

	ph_sep = CreatePropellantResource(0.25);

	th_sep[0] = CreateThruster (m_exhaust_pos10, _V( 1,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[1] = CreateThruster (m_exhaust_pos11, _V( 1,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[2] = CreateThruster (m_exhaust_pos12, _V( -1,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[3] = CreateThruster (m_exhaust_pos13, _V( -1,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[4] = CreateThruster (m_exhaust_pos6, _V( 0,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[5] = CreateThruster (m_exhaust_pos7, _V( 0,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[6] = CreateThruster (m_exhaust_pos8, _V( 1,0,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[7] = CreateThruster (m_exhaust_pos9, _V( -1,0,0), 1.0, ph_sep, 10.0, 10.0);

	for (i = 0; i < 8; i++) {
		AddExhaustStream (th_sep[i], &seperation_junk);
		SetThrusterLevel(th_sep[i], 1.0);
	}

	//
	// Interstage seperation 'thruster'.
	//

	ph_sep2 = CreatePropellantResource(0.25);

	th_sep2[0] = CreateThruster (m_exhaust_pos10, _V( 1,1,0), 1.0, ph_sep2, 10.0, 10.0);
	th_sep2[1] = CreateThruster (m_exhaust_pos11, _V( 1,-1,0), 1.0, ph_sep2, 10.0, 10.0);
	th_sep2[2] = CreateThruster (m_exhaust_pos12, _V( -1,1,0), 1.0, ph_sep2, 10.0, 10.0);
	th_sep2[3] = CreateThruster (m_exhaust_pos13, _V( -1,-1,0), 1.0, ph_sep2, 10.0, 10.0);
	th_sep2[4] = CreateThruster (m_exhaust_pos6, _V( 0,1,0), 1.0, ph_sep2, 10.0, 10.0);
	th_sep2[5] = CreateThruster (m_exhaust_pos7, _V( 0,-1,0), 1.0, ph_sep2, 10.0, 10.0);
	th_sep2[6] = CreateThruster (m_exhaust_pos8, _V( 1,0,0), 1.0, ph_sep2, 10.0, 10.0);
	th_sep2[7] = CreateThruster (m_exhaust_pos9, _V( -1,0,0), 1.0, ph_sep2, 10.0, 10.0);

	for (i = 0; i < 8; i++) {
		AddExhaustStream (th_sep2[i], &seperation_junk);
	}

	//
	// orbiter main thrusters
	//
	th_2nd[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_2nd[1] = CreateThruster (m_exhaust_pos2,_V( 0,0,1),  THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_2nd[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_2nd[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);
	th_2nd[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_SECOND_VAC , ph_2nd, ISP_SECOND_VAC, ISP_SECOND_SL);

	thg_2nd = CreateThrusterGroup (th_2nd, SII_EngineNum, THGROUP_MAIN);

	EXHAUSTSPEC es_2nd[5] = {
	    { th_2nd[0], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, J2Tex },
	    { th_2nd[1], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, J2Tex },
	    { th_2nd[2], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, J2Tex },
	    { th_2nd[3], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, J2Tex },
	    { th_2nd[4], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, J2Tex }
	};
	
	for (i = 0; i < SII_EngineNum; i++) AddExhaust(es_2nd + i);

	if (SII_UllageNum) {
		
		if (!ph_ullage2)
		{
			ph_ullage2 = CreatePropellantResource(121.65*SII_UllageNum);
		}
		
		th_ull[0] = CreateThruster (m_exhaust_pos10, _V( 0,0,1),102000 , ph_ullage2, 2516);
		th_ull[1] = CreateThruster (m_exhaust_pos11, _V( 0,0,1),102000, ph_ullage2, 2516);
		th_ull[2] = CreateThruster (m_exhaust_pos12, _V( 0,0,1),102000, ph_ullage2, 2516);
		th_ull[3] = CreateThruster (m_exhaust_pos13, _V( 0,0,1),102000, ph_ullage2, 2516);
		th_ull[4] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), 102000 , ph_ullage2, 2516);
		th_ull[5] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), 102000 , ph_ullage2, 2516);
		th_ull[6] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), 102000 , ph_ullage2, 2516);
		th_ull[7] = CreateThruster (m_exhaust_pos9, _V( 0,0,1), 102000 , ph_ullage2, 2516);

		for (i = 0; i < SII_UllageNum; i ++) {
			AddExhaust (th_ull[i], 5.0, 0.3, exhaust_tex);
			AddExhaustStream (th_ull[i], &solid_exhaust);
		}

		thg_ull = CreateThrusterGroup (th_ull, SII_UllageNum, THGROUP_USER);
	}

	sii->RecalculateEngineParameters(THRUST_SECOND_VAC);
}

void SaturnV::SetThirdStage ()

{
	ClearMeshes();
	TRACESETUP("SetThirdStage");

	SetSize (15.5);
	SetCOG_elev (15.225);
	SetEmptyMass (Stage3Mass);
	SetPMI (_V(53.5,53.5,5));
	SetCrossSections (_V(167,167,47));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);

	double TCPS4B = -16;

	double td_mass = Stage3Mass + (S4B_FuelMass / 2);
	double td_width = 7.0;
	double td_tdph = TCPS4B;
	double td_height = 39.0;

	ConfigTouchdownPoints(td_mass, td_width, td_tdph, td_height, -0.1);

	//
	// Clear SII Sep light just in case the interstage hung up.
	//

	ClearSIISep();

	SetThirdStageMesh(-STG2O);
}

void SaturnV::SetThirdStageMesh (double offset)
{
	S4Offset = 2.0 + offset;

	VECTOR3 mesh_dir=_V(0, 0, S4Offset);

	AddMesh (hStage3Mesh, &mesh_dir);
	if (LEM_DISPLAY && (SIVBPayload == PAYLOAD_LEM)){
		mesh_dir=_V(0,0,12 + offset);
		AddMesh (hLMPKD, &mesh_dir);
	}

	mesh_dir=_V(-1.48,-1.48,14.55 + offset);
	AddMesh (hStageSLA1Mesh, &mesh_dir);
	mesh_dir=_V(1.48,-1.48,14.55 + offset);
	AddMesh (hStageSLA2Mesh, &mesh_dir);
	mesh_dir=_V(1.48,1.48,14.55 + offset);
    AddMesh (hStageSLA3Mesh, &mesh_dir);
	mesh_dir=_V(-1.48,1.48,14.55 + offset);
    AddMesh (hStageSLA4Mesh, &mesh_dir);

	AddSM(19.1 + offset, false);

	mesh_dir=_V(0, 0, 23.25 + offset);

	UINT meshidx;
	meshidx = AddMesh (hCMnh, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);

	// And the Crew
	if (Crewed) {
		cmpidx = AddMesh (hCMP, &mesh_dir);
		crewidx = AddMesh (hCREW, &mesh_dir);
		SetCrewMesh();
	} else {
		cmpidx = -1;
		crewidx = -1;
	}

	meshidx = AddMesh (hCMInt, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_EXTERNAL);

	meshidx = AddMesh (hCMVC, &mesh_dir);
	SetMeshVisibilityMode (meshidx, MESHVIS_VC);
	VCMeshOffset = mesh_dir;

	sidehatchidx = AddMesh (hFHC, &mesh_dir);
	sidehatchopenidx = AddMesh (hFHO, &mesh_dir);
	SetSideHatchMesh();

	//Forward Hatch
	fwdhatchidx = AddMesh(hFHF, &mesh_dir);
	SetFwdHatchMesh();

	// Optics Cover
	opticscoveridx = AddMesh (hopticscover, &mesh_dir);
	SetOpticsCoverMesh();

	probeidx = -1;
	probeextidx = -1;

	if (LESAttached)
	{
		TowerOffset = 28.2 + offset;
		mesh_dir=_V(0, 0, TowerOffset);
		meshidx = AddMesh (hsat5tower, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_VCEXTERNAL);
	}
	else {
		if (HasProbe) {
			probeidx = AddMesh(hprobe, &mesh_dir);
			probeextidx = AddMesh(hprobeext, &mesh_dir);
			SetDockingProbeMesh();
		}
	}

	VECTOR3 dockpos = {0,0,24.8 + offset};
	VECTOR3 dockdir = {0,0,1};
	VECTOR3 dockrot = {0,1,0};
	SetDockParams(dockpos, dockdir, dockrot);

	SetCameraOffset (_V(-1,1.0,32.4 + offset));
	SetView(23.1 + offset, false);
}

void SaturnV::SetThirdStageEngines (double offset)
{
	DelThrusterGroup(THGROUP_MAIN, true);
	ClearThrusterDefinitions();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	//Add CSM RCS
	if (SaturnHasCSM())
	{
		AddRCSJets(12.97, SM_RCS_THRUST);
		AddRCS_CM(CM_RCS_THRUST, 15.25, false);
	}

	// ************************* propellant specs **********************************

	if (!ph_3rd) 
		ph_3rd  = CreatePropellantResource(S4B_FuelMass); //3rd stage Propellant

	if (!ph_ullage3)
	{
		//
		// Create SIVB stage ullage rocket propellant
		//

		ph_ullage3 = CreatePropellantResource(2*26.67);
	}

	SetDefaultPropellantResource (ph_3rd); // display 3rd stage propellant level in generic HUD

	if (ph_2nd) {
		DelPropellantResource(ph_2nd);
		ph_2nd = 0;
	}

	if (ph_sep) 
	{
		DelPropellantResource(ph_sep);
		ph_sep = 0;
	}

	if (ph_sep2) 
	{
		DelPropellantResource(ph_sep2);
		ph_sep2 = 0;
	}

	if (ph_ullage2)
	{
		DelPropellantResource(ph_ullage2);
		ph_ullage2 = 0;
	}

	//
	// *********************** thruster definitions ********************************
	//

	VECTOR3	s_exhaust_pos1= _V(2.55,2.55,-3.6 + offset);
	VECTOR3 s_exhaust_pos2= _V(2.55,-2.55,-3.6 + offset);
	VECTOR3	s_exhaust_pos3= _V(-2.55,2.55,-3.6 + offset);
	VECTOR3 s_exhaust_pos4= _V(-2.55,-2.55,-3.6 + offset);
	VECTOR3	s_exhaust_pos6= _V(0,3.6,-3.6 + offset);
	VECTOR3 s_exhaust_pos7= _V(0,-3.6,-3.6 + offset);
	VECTOR3	s_exhaust_pos8= _V(3.6,0,-3.6 + offset);
	VECTOR3 s_exhaust_pos9= _V(-3.6,0,-3.6 + offset);

	//
	// Seperation 'thrusters'.
	//

	if (viewpos != SATVIEW_ENG1 && viewpos != SATVIEW_ENG2) 
	{
		int i;

		ph_sep = CreatePropellantResource(0.25);

		th_sep[0] = CreateThruster (s_exhaust_pos1, _V( 1,1,0), 1.0, ph_sep, 10.0, 10.0);
		th_sep[1] = CreateThruster (s_exhaust_pos2, _V( 1,-1,0), 1.0, ph_sep, 10.0, 10.0);
		th_sep[2] = CreateThruster (s_exhaust_pos3, _V( -1,1,0), 1.0, ph_sep, 10.0, 10.0);
		th_sep[3] = CreateThruster (s_exhaust_pos4, _V( -1,-1,0), 1.0, ph_sep, 10.0, 10.0);
		th_sep[4] = CreateThruster (s_exhaust_pos6, _V( 0,1,0), 1.0, ph_sep, 10.0, 10.0);
		th_sep[5] = CreateThruster (s_exhaust_pos7, _V( 0,-1,0), 1.0, ph_sep, 10.0, 10.0);
		th_sep[6] = CreateThruster (s_exhaust_pos8, _V( 1,0,0), 1.0, ph_sep, 10.0, 10.0);
		th_sep[7] = CreateThruster (s_exhaust_pos9, _V( -1,0,0), 1.0, ph_sep, 10.0, 10.0);

		for (i = 0; i < 8; i++) {
			AddExhaustStream (th_sep[i], &seperation_junk);
		}
	}

	VECTOR3 m_exhaust_pos1= {0,0,-9+ offset};

	//
	// orbiter main thrusters
	//

	th_3rd[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_THIRD_VAC, ph_3rd, ISP_THIRD_VAC);
	thg_3rd = CreateThrusterGroup (th_3rd, 1, THGROUP_MAIN);

	EXHAUSTSPEC es_3rd[1] = {
		{ th_3rd[0], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, J2Tex }
	};

	AddExhaust(es_3rd);

	VECTOR3	u_exhaust_pos6= _V(3.6, -0.425, -3.6 + offset);
	VECTOR3 u_exhaust_pos7= _V(-3.6, 0.925, -3.6 + offset);

	th_ver[0] = CreateThruster(u_exhaust_pos6, _V(-0.368829, 0.043542, 0.928477), 15079.47, ph_ullage3, 2188.1);
	th_ver[1] = CreateThruster(u_exhaust_pos7, _V(0.359706, -0.092425, 0.928477), 15079.47, ph_ullage3, 2188.1);

	for (int i = 0; i < 2; i++)
		AddExhaust (th_ver[i], 5.0, 0.25, exhaust_tex);

	thg_ver = CreateThrusterGroup (th_ver, 2, THGROUP_USER);

	sivb->RecalculateEngineParameters(THRUST_THIRD_VAC);

	// LOX venting thruster

	th_3rd_lox = CreateThruster(m_exhaust_pos1, _V(0, 0, 1), 3300.0, ph_3rd, 157.0, 157.0);

	fuel_venting_spec.tex = oapiRegisterParticleTexture("ProjectApollo/Contrail_SaturnVenting");
	AddExhaustStream(th_3rd_lox, &fuel_venting_spec);
}

void SaturnV::SeparateStage (int new_stage)

{
	VESSELSTATUS vs1;
	VESSELSTATUS vs2;

	VECTOR3 ofs1 = _V(0,0,0);
	VECTOR3 ofs2 = _V(0,0,0);
	VECTOR3 vel1 = _V(0,0,0);
	VECTOR3 vel2 = _V(0,0,0);

	GetStatus (vs1);
	GetStatus (vs2);

	vs1.eng_main = vs1.eng_hovr = 0.0;
	vs2.eng_main = vs2.eng_hovr = 0.0;

	if (stage == LAUNCH_STAGE_ONE && new_stage == LAUNCH_STAGE_TWO)
	{
		ofs1 = OFS_STAGE1;
		vel1 = _V(0, 0, -4.0);
	}

	if ((stage == PRELAUNCH_STAGE || stage == LAUNCH_STAGE_ONE) && new_stage > LAUNCH_STAGE_TWO)
	{
		ofs1= OFS_ABORT;
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_TWO && new_stage == LAUNCH_STAGE_TWO_ISTG_JET)
	{
		ofs1 = OFS_STAGE12;
		vel1 = _V(0,0,-4.0);
	}

	if ((stage == LAUNCH_STAGE_TWO || stage == LAUNCH_STAGE_TWO_ISTG_JET) && new_stage >= CSM_LEM_STAGE)
	{
		ofs1= OFS_ABORT2;
		vel1 = _V(0,0,-4.0);
	}

	if (stage == LAUNCH_STAGE_TWO_ISTG_JET && new_stage == LAUNCH_STAGE_SIVB)
	{
	 	ofs1 = OFS_STAGE2;
		vel1 = _V(0,0,-6.0);
	}

	if ((stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB) && new_stage != CM_STAGE)
	{
	 	ofs1 = _V(0.0, 0.0, S4Offset); // OFS_STAGE3;
		vel1 = _V(0.0 ,0.0, 0.0);
	}

	if (stage == LAUNCH_STAGE_SIVB && new_stage == CM_STAGE)
	{
		ofs1 = _V(0.0, 0.0, S4Offset - 2.0499);
		vel1 = _V(0, 0, -2.0);
	}

	if (stage == STAGE_ORBIT_SIVB && new_stage == CM_STAGE)
	{
		ofs1 = _V(0.0, 0.0, S4Offset - 2.0499);
		vel1 = _V(0, 0, -0.2);
	}

	if (stage == CSM_LEM_STAGE)
	{
	 	ofs1 = OFS_SM;
		vel1 = _V(0,0,-0.1);
	}

	if (stage == CM_STAGE)
	{
		ofs1 = OFS_CM_CONE;
		vel1 = _V(1.0,1.0,1.0);
	}

	VECTOR3 rofs1, rvel1 = {vs1.rvel.x, vs1.rvel.y, vs1.rvel.z};
	VECTOR3 rofs2, rvel2 = {vs2.rvel.x, vs2.rvel.y, vs2.rvel.z};

	Local2Rel (ofs1, vs1.rpos);
	Local2Rel (ofs2, vs2.rpos);

	GlobalRot (vel1, rofs1);
	GlobalRot (vel2, rofs2);

	vs1.rvel.x = rvel1.x+rofs1.x;
	vs1.rvel.y = rvel1.y+rofs1.y;
	vs1.rvel.z = rvel1.z+rofs1.z;
	vs2.rvel.x = rvel2.x+rofs2.x;
	vs2.rvel.y = rvel2.y+rofs2.y;
	vs2.rvel.z = rvel2.z+rofs2.z;

	if (stage == CM_ENTRY_STAGE_TWO)
	{
		SetChuteStage1 ();
	}

    if (stage == LAUNCH_STAGE_ONE && new_stage == LAUNCH_STAGE_TWO)
	{
	    vs1.vrot.x = 0.0025;
		vs1.vrot.y = 0.0025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;

		StageS.play(NOLOOP, 255);

		CreateStageOne();

		//
		// Seperate off the S1C stage and show it.
		//

		if (hstg1) {
			S1C *stage1 = (S1C *) oapiGetVesselInterface(hstg1);
			stage1->DefSetState(&vs1);

			S1CSettings S1Config;

			S1Config.SettingsType.S1C_SETTINGS_ENGINES = 1;
			S1Config.SettingsType.S1C_SETTINGS_FUEL = 1;
			S1Config.SettingsType.S1C_SETTINGS_GENERAL = 1;
			S1Config.SettingsType.S1C_SETTINGS_MASS = 1;

			S1Config.RetroNum = SI_RetroNum;
			S1Config.EmptyMass = SI_EmptyMass;
			S1Config.MainFuelKg = GetPropellantMass(ph_1st);
			S1Config.MissionTime = MissionTime;
			S1Config.VehicleNo = VehicleNo;
			S1Config.ISP_FIRST_SL = ISP_FIRST_SL;
			S1Config.ISP_FIRST_VAC = ISP_FIRST_VAC;
			S1Config.THRUST_FIRST_VAC = THRUST_FIRST_VAC;
			S1Config.CurrentThrust = GetThrusterLevel(th_1st[0]);
			S1Config.LowRes = LowRes;
			S1Config.Stretched = false;
			S1Config.S4Interstage = (SaturnType == SAT_INT20);
			S1Config.EngineNum = SI_EngineNum;

			stage1->SetState(S1Config);
		}

		ConfigureStageMeshes (new_stage);
		ConfigureStageEngines (new_stage);

		//
		// Fire 'seperation' thrusters.
		//

		if (viewpos != SATVIEW_ENG1 && viewpos != SATVIEW_ENG2) 
		{
			FireSeperationThrusters(th_sep);
		}

		if (viewpos == SATVIEW_ENG1) 
		{
			oapiSetFocusObject(hstg1);
			oapiCameraAttach(hstg1, CAM_COCKPIT);
		}

		if (SaturnType == SAT_INT20)
		{
			//
			// What's the correct value for the INT20?
			//
			ShiftCentreOfMass(_V(0, 0, STG0O + STG2O - 24.5));
		}
		else
		{
			ShiftCentreOfMass(_V(0, 0, STG0O + STG1O));
		}
	}

	if (stage == LAUNCH_STAGE_TWO && new_stage == LAUNCH_STAGE_TWO_ISTG_JET)
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;

		//CrashBumpS.play(NOLOOP, 150);

		char VName[256], *CName;

		GetApolloName(VName);
		strcat (VName, "-INTSTG");

		if (LowRes)
		{
			CName = "ProjectApollo/sat5intstglowres";
		}
		else
		{
			switch (SII_UllageNum) {
			case 4:
				CName = "ProjectApollo/sat5intstg4";
				break;

			case 8:
				CName = "ProjectApollo/sat5intstg8";
				break;

			default:
				CName = "ProjectApollo/sat5intstg";
				break;
			}
		}

		hintstg = oapiCreateVessel(VName, CName, vs1);
		InterstageAttached = false;

		//
		// Fire 'seperation' thrusters.
		//

		if (viewpos != SATVIEW_ENG1 && viewpos != SATVIEW_ENG2) 
		{
			FireSeperationThrusters(th_sep2);
		}

		ConfigureStageMeshes (new_stage);
	}

	if (stage == LAUNCH_STAGE_TWO_ISTG_JET && new_stage == LAUNCH_STAGE_SIVB)
	{
	    vs1.vrot.x = 0.025;
		vs1.vrot.y = 0.025;
		vs1.vrot.z = 0.0;
		vs2.vrot.x = 0.0;
		vs2.vrot.y = 0.0;
		vs2.vrot.z = 0.0;

		StageS.play();

		//
		// Seperate off the SII stage and initialise it.
		//

		char VName[256];

		GetApolloName(VName);
		strcat (VName, "-STG2");
		hstg2 = oapiCreateVessel(VName,"ProjectApollo/sat5stg2",vs1);

		SIISettings S2Config;

		S2Config.SettingsType.SII_SETTINGS_ENGINES = 1;
		S2Config.SettingsType.SII_SETTINGS_FUEL = 1;
		S2Config.SettingsType.SII_SETTINGS_GENERAL = 1;
		S2Config.SettingsType.SII_SETTINGS_MASS = 1;

		S2Config.RetroNum = SII_RetroNum;
		S2Config.EmptyMass = SII_EmptyMass;
		S2Config.MainFuelKg = GetPropellantMass(ph_2nd);
		S2Config.MissionTime = MissionTime;
		S2Config.VehicleNo = VehicleNo;
		S2Config.ISP_SECOND_SL = ISP_SECOND_SL;
		S2Config.ISP_SECOND_VAC = ISP_SECOND_VAC;
		S2Config.THRUST_SECOND_VAC = THRUST_SECOND_VAC;
		S2Config.CurrentThrust = GetThrusterLevel(th_2nd[0]);
		S2Config.LowRes = LowRes;

		SII *stage2 = static_cast<SII *> (oapiGetVesselInterface(hstg2));
		stage2->SetState(S2Config);

		ConfigureStageMeshes(new_stage);
		ConfigureStageEngines(new_stage);

		//
		// Fire 'seperation' thrusters.
		//

		if (viewpos != SATVIEW_ENG1 && viewpos != SATVIEW_ENG2)
		{
			FireSeperationThrusters(th_sep);
		}

		ShiftCentreOfMass(_V(0, 0, STG2O - STG1O));
	}

	if ((stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB) && new_stage != CM_STAGE)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		CreateSIVBStage("ProjectApollo/sat5stg3", vs1, true);

		SeparationS.play(NOLOOP,255);

		// Store RCS Propellant 
		double proptemp[6] = { -1,-1,-1,-1,-1,-1 };

		if (ph_rcs_cm_1) proptemp[0] = GetPropellantMass(ph_rcs_cm_1);
		if (ph_rcs_cm_2) proptemp[1] = GetPropellantMass(ph_rcs_cm_2);
		if (ph_rcs0) proptemp[2] = GetPropellantMass(ph_rcs0);
		if (ph_rcs1) proptemp[3] = GetPropellantMass(ph_rcs1);
		if (ph_rcs2) proptemp[4] = GetPropellantMass(ph_rcs2);
		if (ph_rcs3) proptemp[5] = GetPropellantMass(ph_rcs3);
		ClearPropellants();

		SetCSMStage();

		// Restore RCS Propellant
		if (proptemp[0] != -1) SetPropellantMass(ph_rcs_cm_1, proptemp[0]);
		if (proptemp[1] != -1) SetPropellantMass(ph_rcs_cm_2, proptemp[1]);
		if (proptemp[2] != -1) SetPropellantMass(ph_rcs0, proptemp[2]);
		if (proptemp[3] != -1) SetPropellantMass(ph_rcs1, proptemp[3]);
		if (proptemp[4] != -1) SetPropellantMass(ph_rcs2, proptemp[4]);
		if (proptemp[5] != -1) SetPropellantMass(ph_rcs3, proptemp[5]);

     	ShiftCentreOfMass(_V(0, 0, 13.15));
	}

	if (stage == CSM_LEM_STAGE)
	{
		char VName[256];
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		//
		// Play appropriate sound for SM seperation.
		//

		if (ApolloExploded) 
		{
			SSMSepExploded.play(NOLOOP, 200);
		}
		else
		{
			SMJetS.play();
		}
		SSMSepExploded.done();

		GetApolloName(VName); strcat (VName, "-SM");
		hSMJet = oapiCreateVessel(VName, "ProjectApollo/SM", vs1);

		SMSettings SMConfig;

		SMConfig.SettingsType.SM_SETTINGS_ENGINES = 1;
		SMConfig.SettingsType.SM_SETTINGS_FUEL = 1;
		SMConfig.SettingsType.SM_SETTINGS_GENERAL = 1;
		SMConfig.SettingsType.SM_SETTINGS_MASS = 1;

		SMConfig.EmptyMass = SM_EmptyMass;
		SMConfig.MainFuelKg = GetPropellantMass(ph_sps);
		SMConfig.RCSAFuelKg = GetPropellantMass(ph_rcs0);
		SMConfig.RCSBFuelKg = GetPropellantMass(ph_rcs1);
		SMConfig.RCSCFuelKg = GetPropellantMass(ph_rcs2);
		SMConfig.RCSDFuelKg = GetPropellantMass(ph_rcs3);
		SMConfig.MissionTime = MissionTime;
		SMConfig.VehicleNo = VehicleNo;
		SMConfig.LowRes = LowRes;
		SMConfig.showHGA = pMission->CSMHasHGA();
		SMConfig.A13Exploded = ApolloExploded;
		SMConfig.SIMBayPanelJett = SIMBayPanelJett;
		SMConfig.HGAalpha = hga.GetAlpha();
		SMConfig.HGAbeta = hga.GetBeta();
		SMConfig.HGAgamma = hga.GetGamma();
		SMConfig.SMBusAPowered = MainBusAController.IsSMBusPowered();
		SMConfig.SMBusBPowered = MainBusBController.IsSMBusPowered();
		if (secs.SMJCA) secs.SMJCA->GetState(SMConfig.SMJCAState);
		if (secs.SMJCB) secs.SMJCB->GetState(SMConfig.SMJCBState);

		SM *SMVessel = (SM *) oapiGetVesselInterface(hSMJet);
		SMVessel->SetState(SMConfig);

		// Store CM Propellant 
		double cmprop1 = -1;
		double cmprop2 = -1;
		if (ph_rcs_cm_1) cmprop1 = GetPropellantMass(ph_rcs_cm_1);
		if (ph_rcs_cm_2) cmprop2 = GetPropellantMass(ph_rcs_cm_2);

		SetReentryStage();

		// Restore CM Propellant
		if (cmprop1 != -1) SetPropellantMass(ph_rcs_cm_1, cmprop1);
		if (cmprop2 != -1) SetPropellantMass(ph_rcs_cm_2, cmprop2);

		ShiftCentreOfMass(_V(0, 0, 2.1));
	}

	if (stage == CM_STAGE)
	{
		SetChuteStage2 ();
	}

	if (stage == CM_ENTRY_STAGE_TWO)
	{
		SetChuteStage3 ();
	}

	if (stage == CM_ENTRY_STAGE_FOUR)
	{
		SetChuteStage4 ();
	}

	if (stage == CM_ENTRY_STAGE_FIVE)
	{
		SetSplashStage ();
	}

	if ((stage == PRELAUNCH_STAGE || stage == LAUNCH_STAGE_ONE) && new_stage >= CSM_LEM_STAGE)
	{
		VESSELSTATUS2 vs3;
		memset(&vs3, 0, sizeof(vs3));
		vs3.version = 2;

		GetStatusEx(&vs3);
		StageS.play();

		if (vs3.status == 1) {
			vs3.vrot.x = 102.5;

			char VName[256];
			GetApolloName(VName); strcat(VName, "-ABORT");
			habort = oapiCreateVesselEx(VName, "ProjectApollo/Saturn5Abort1", &vs3);

			Sat5Abort1 *stage1 = static_cast<Sat5Abort1 *> (oapiGetVesselInterface(habort));
			stage1->SetState(new_stage == CM_STAGE, LowRes, SIVBPayload);

			if (new_stage == CSM_LEM_STAGE)
			{
				vs3.vrot.x = 102.5 + 21;
				DefSetStateEx(&vs3);
				SetCSMStage();
			}
			else
			{
				vs3.vrot.x = 102.5 + 23.25;
				DefSetStateEx(&vs3);
				SetReentryStage();
			}
		}
		else
		{
			vs1.vrot.x = 0.0;
			vs1.vrot.y = 0.0;
			vs1.vrot.z = 0.0;

			char VName[256];
			GetApolloName(VName); strcat(VName, "-ABORT");
			habort = oapiCreateVessel(VName, "ProjectApollo/Saturn5Abort1", vs1);

			Sat5Abort1 *stage1 = static_cast<Sat5Abort1 *> (oapiGetVesselInterface(habort));
			stage1->SetState(new_stage == CM_STAGE, LowRes, SIVBPayload);

			if (new_stage == CSM_LEM_STAGE)
			{
				SetCSMStage();
				ShiftCentreOfMass(_V(0, 0, STG0O + 21));
			}
			else
			{
				SetReentryStage();
				ShiftCentreOfMass(_V(0, 0, STG0O + 23.25));
			}
		}
	}

	if ((stage == LAUNCH_STAGE_TWO || stage == LAUNCH_STAGE_TWO_ISTG_JET) && new_stage >= CSM_LEM_STAGE)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;

		StageS.play();

		char VName[256];
		GetApolloName(VName); strcat (VName, "-ABORT");
		habort = oapiCreateVessel (VName, "ProjectApollo/Saturn5Abort2", vs1);

		int InterstageConfig = -1;

		if (InterstageAttached) {
			if (SII_UllageNum == 4) {
				InterstageConfig = 4;
			}
			else if (SII_UllageNum == 8) {
				InterstageConfig = 8;
			}
			else {
				InterstageConfig = 1;
			}
		}

		Sat5Abort2 *stage2 = static_cast<Sat5Abort2 *> (oapiGetVesselInterface(habort));
		stage2->SetState(new_stage == CM_STAGE, LowRes, SIVBPayload, InterstageConfig);

		if (new_stage == CSM_LEM_STAGE)
		{
			SetCSMStage();
			ShiftCentreOfMass(_V(0, 0, -STG1O + 21));
		}
		else
		{
			SetReentryStage();
			ShiftCentreOfMass(_V(0, 0, -STG1O + 23.25));
		}
	}

	if ((stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB) && new_stage == CM_STAGE)
	{
		vs1.vrot.x = 0.0;
		vs1.vrot.y = 0.0;
		vs1.vrot.z = 0.0;
		
		StageS.play();
		
		char VName[256];
		GetApolloName(VName); strcat(VName, "-ABORT");
		habort = oapiCreateVessel(VName, "ProjectApollo/Saturn5Abort3", vs1);

		Sat5Abort3 *stage3 = static_cast<Sat5Abort3 *> (oapiGetVesselInterface(habort));
		SMJCState sta, stb;
		secs.SMJCA->GetState(sta);
		secs.SMJCB->GetState(stb);
		stage3->SetState(LowRes, VehicleNo, MainBusAController.IsSMBusPowered(), MainBusBController.IsSMBusPowered(), &sta, &stb);
		
		SetReentryStage();
		ShiftCentreOfMass(_V(0, 0, 13.15 + 2.0499));
	}
}

void SaturnV::ActivatePrelaunchVenting()

{
	//
	// "tank venting" particle streams
	//
	static double lvl = 1.0;

	//S-II
	if (!prelaunchvent[0]) prelaunchvent[0] = AddParticleStream(&prelaunchvent1_spec, _V(-5.5, -1,   -24.0 + STG0O), _V(-1,  0, 0), &lvl);
	//S-IVB
	if (!prelaunchvent[2]) prelaunchvent[2] = AddParticleStream(&prelaunchvent3_spec, _V(-3.5,  1,    -3.5 + STG0O), _V(-1,  0, 0), &lvl);
	//S-IC
	if (!prelaunchvent[1]) prelaunchvent[1] = AddParticleStream(&prelaunchvent2_spec, _V(-3.7, -3.7, -38.0 + STG0O), _V(-1, -1, 0), &lvl);
}

void SaturnV::DeactivatePrelaunchVenting()

{
	// "tank venting" particle streams
	int i;

	for (i = 0; i < 3; i++) {
		if (prelaunchvent[i]) {
			DelExhaustStream(prelaunchvent[i]);
			prelaunchvent[i] = NULL;
		}
	}
}

void SaturnV::ActivateStagingVent()

{
	// "staging vent" particle streams
	static double lvl = 1.0;

	VECTOR3	m_exhaust_pos6= _V(0,5.07,-33.15-STG1O - 5.);
	VECTOR3 m_exhaust_pos7= _V(0,-5.07,-33.15-STG1O - 5.);
	VECTOR3	m_exhaust_pos8= _V(5.07,0,-33.15-STG1O - 5.);
	VECTOR3 m_exhaust_pos9= _V(-5.07,0,-33.15-STG1O - 5.);
	VECTOR3	m_exhaust_pos10= _V(3.55,3.7,-33.15-STG1O - 5.);
	VECTOR3 m_exhaust_pos11= _V(3.55,-3.7,-33.15-STG1O - 5.);
	VECTOR3	m_exhaust_pos12= _V(-3.55,3.7,-33.15-STG1O - 5.);
	VECTOR3 m_exhaust_pos13= _V(-3.55,-3.7,-33.15-STG1O - 5.);

	if (!stagingvent[0]) stagingvent[0] = AddParticleStream (&stagingvent_spec, m_exhaust_pos10, _V( 1, 1,-1), &lvl);
	if (!stagingvent[1]) stagingvent[1] = AddParticleStream (&stagingvent_spec, m_exhaust_pos11, _V( 1,-1,-1), &lvl);
	if (!stagingvent[2]) stagingvent[2] = AddParticleStream (&stagingvent_spec, m_exhaust_pos12, _V(-1, 1,-1), &lvl);
	if (!stagingvent[3]) stagingvent[3] = AddParticleStream (&stagingvent_spec, m_exhaust_pos13, _V(-1,-1,-1), &lvl);
	if (!stagingvent[4]) stagingvent[4] = AddParticleStream (&stagingvent_spec, m_exhaust_pos6,  _V( 0, 1,-1), &lvl);
	if (!stagingvent[5]) stagingvent[5] = AddParticleStream (&stagingvent_spec, m_exhaust_pos7,  _V( 0,-1,-1), &lvl);
	if (!stagingvent[6]) stagingvent[6] = AddParticleStream (&stagingvent_spec, m_exhaust_pos8,  _V( 1, 0,-1), &lvl);
	if (!stagingvent[7]) stagingvent[7] = AddParticleStream (&stagingvent_spec, m_exhaust_pos9,  _V(-1, 0,-1), &lvl);
}

void SaturnV::DeactivateStagingVent()

{
	//
	// "staging vent" particle streams
	//
	int i;

	for (i = 0; i < 8; i++) {
		if (stagingvent[i]) {
			DelExhaustStream(stagingvent[i]);
			stagingvent[i] = NULL;
		}
	}
}

void SaturnV::CreateStageOne() {

	// Create hidden SIC vessel
	char VName[256];
	VESSELSTATUS vs;

	GetStatus(vs);
	GetApolloName(VName);
	strcat (VName, "-STG1");
	hstg1 = oapiCreateVessel(VName,"ProjectApollo/sat5stg1", vs);

	// Load only the necessary meshes
	S1C *stage1 = (S1C *) oapiGetVesselInterface(hstg1);
	stage1->LoadMeshes(LowRes);
}

