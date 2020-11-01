/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SIVb class

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
#include "orbiterSDK.h"

#include "nasspdefs.h"
#include "soundlib.h"
#include "OrbiterMath.h"

#include "PanelSDK/PanelSDK.h"
#include "PanelSDK/Internals/Esystems.h"

#include "powersource.h"
#include "connector.h"
#include "iu.h"
#include "SIVBSystems.h"

#include "toggleswitch.h"
#include "apolloguidance.h"
#include "lemcomputer.h"

#include "payload.h"
#include "sivb.h"
#include "astp.h"
#include "lem.h"
#include "LVDC.h"

#include <stdio.h>
#include <string.h>

//
// Meshes are globally loaded.
//

static MESHHANDLE hSat1stg2;
static MESHHANDLE hSat1stg2low;
static MESHHANDLE hSat1stg21;
static MESHHANDLE hSat1stg22;
static MESHHANDLE hSat1stg23;
static MESHHANDLE hSat1stg24;
static MESHHANDLE hSat1stg2cross;
static MESHHANDLE hsat5stg3;
static MESHHANDLE hsat5stg31;
static MESHHANDLE hsat5stg32;
static MESHHANDLE hsat5stg33;
static MESHHANDLE hsat5stg34;
static MESHHANDLE hsat5stg3low;
static MESHHANDLE hsat5stg31low;
static MESHHANDLE hsat5stg32low;
static MESHHANDLE hsat5stg33low;
static MESHHANDLE hsat5stg34low;
static MESHHANDLE hastp;
static MESHHANDLE hCOAStarget;
static MESHHANDLE hapollo8lta;
static MESHHANDLE hlta_2r;

static SURFHANDLE SMMETex;

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

//
// Spew out particles to simulate the junk thrown out by stage
// seperation explosives.
//

static PARTICLESTREAMSPEC seperation_junk = {
	0,		// flag
	0.02,	// size
	1000,	// rate
	0.3,    // velocity
	5.0,    // velocity distribution
	100,	// lifetime
	0,	    // growthrate
	0,      // atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_FLAT, 1.0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

void SIVbLoadMeshes()

{
	//
	// Saturn 1b.
	//

	hSat1stg2 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg2");
	hSat1stg2low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/nsat1stg2");
	hSat1stg21 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg21");
	hSat1stg22 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg22");
	hSat1stg23 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg23");
	hSat1stg24 = oapiLoadMeshGlobal ("ProjectApollo/nsat1stg24");
	hSat1stg2cross = oapiLoadMeshGlobal("ProjectApollo/nsat1stg2cross");
	hastp = oapiLoadMeshGlobal ("ProjectApollo/nASTP3");
	hCOAStarget = oapiLoadMeshGlobal ("ProjectApollo/sat_target");

	//
	// Saturn V
	//

	hsat5stg3 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg3");
	hsat5stg31 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg31");
	hsat5stg32 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg32");
	hsat5stg33 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg33");
	hsat5stg34 = oapiLoadMeshGlobal ("ProjectApollo/sat5stg34");

	hsat5stg3low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg3");
	hsat5stg31low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg31");
	hsat5stg32low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg32");
	hsat5stg33low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg33");
	hsat5stg34low = oapiLoadMeshGlobal ("ProjectApollo/LowRes/sat5stg34");

	hapollo8lta = oapiLoadMeshGlobal ("ProjectApollo/apollo8_lta");
	hlta_2r = oapiLoadMeshGlobal ("ProjectApollo/LTA_2R");

	SMMETex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_j2");
	seperation_junk.tex = oapiRegisterParticleTexture("ProjectApollo/junk");
}

SIVB::SIVB(OBJHANDLE hObj, int fmodel) : ProjectApolloConnectorVessel(hObj, fmodel),
CSMLVSeparationInitiator("CSM-LV-Separation-Initiator", Panelsdk),
LMSLASeparationInitiators("LM-SLA-Separation-Initiators", Panelsdk),
SLAPanelDeployInitiator("SLA-Panel-Deploy-Initiator", Panelsdk)
{
	PanelSDKInitalised = false;

	InitS4b();
}

SIVB::~SIVB()

{
	if (!iuinitflag && iu)
	{
		delete iu;
		iu = 0;
	}

	if (sivbsys)
	{
		delete sivbsys;
		sivbsys = 0;
	}

	//
	// Delete LM PAD data.
	//
	if (LMPad) {
		delete[] LMPad;
		LMPad = 0;
	}
	if (AEAPad) {
		delete[] AEAPad;
		AEAPad = 0;
	}
}

void SIVB::InitS4b()

{
	int i;

	iu = NULL;
	sivbsys = NULL;

	iuinitflag = false;

	PayloadType = PAYLOAD_EMPTY;
	PanelsHinged = false;
	PanelsOpened = false;
	State = SIVB_STATE_SETUP;
	LowRes = false;
	IUSCContPermanentEnabled = true;
	PayloadCreated = false;

	ph_aps1 = 0;
	ph_aps2 = 0;
	ph_main = 0;
	thg_sep = 0;
	thg_sepPanel = 0;
	thg_ver = 0;
	hDock = 0;
	hDockSI = 0;
	hDockCSM = 0;

	EmptyMass = 15000.0;
	PayloadMass = 0.0;
	MainFuel = 5000.0;
	ApsFuel1Kg = ApsFuel2Kg = S4B_APS_FUEL_PER_TANK;

	THRUST_THIRD_VAC = 1000.0;
	ISP_THIRD_VAC = 300.0;

	CurrentThrust = 0;
	RotationLimit = 0.25;
	PayloadEjectionForce = 0.0;

	FirstTimestep = false;
	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	for (i = 0; i < 6; i++)
		th_aps_rot[i] = 0;
	for (i = 0; i < 2; i++)
		th_aps_ull[i] = 0;

	th_main[0] = 0;
	th_lox_vent = 0;
	panelProc = 0;
	panelProcPlusX = 0;
	panelTimestepCount = 0;
    panelMesh1SaturnV = -1;
	panelMesh2SaturnV  = -1;
	panelMesh3SaturnV  = -1;
	panelMesh4SaturnV  = -1;
    panelMesh1SaturnVLow = -1;
	panelMesh2SaturnVLow  = -1;
	panelMesh3SaturnVLow  = -1;
	panelMesh4SaturnVLow  = -1;
    panelMesh1Saturn1b = -1;
	panelMesh2Saturn1b  = -1;
	panelMesh3Saturn1b  = -1;
	panelMesh4Saturn1b  = -1;
    panelAnim = 0;
	panelAnimPlusX = 0;

	meshSivbSaturnV = -1;
	meshSivbSaturnVLow = -1;
	meshSivbSaturn1b = -1;
	meshSivbSaturn1bLow = -1;
	meshASTP_A = -1;
	meshASTP_B = -1;
	meshCOASTarget_A = -1;
	meshCOASTarget_B = -1;
	meshCOASTarget_C = -1;
	meshApollo8LTA = -1;
	meshLTA_2r = -1;

	//
	// Default payload name.
	//
	sprintf(PayloadName, "%s-PAYLOAD", GetName());

	payloadSettings.DescentFuelKg = 8375.0;
	payloadSettings.AscentFuelKg = 2345.0;
	payloadSettings.AscentEmptyKg = 2150.0;
	payloadSettings.DescentEmptyKg = 2224.0;
	payloadSettings.MissionNo = 0;
	Payloaddatatransfer = false;

	//
	// Checklist
	//

	payloadSettings.checklistFile[0] = 0;

	//
	// LM PAD data.
	//

	LMPadCount = 0;
	LMPad = 0;
	LMPadLoadCount = 0;
	LMPadValueCount = 0;

	AEAPadCount = 0;
	AEAPad = 0;
	AEAPadLoadCount = 0;
	AEAPadValueCount = 0;

	//
	// Set up the connections.
	//

	IUCommandConnector.SetSIVb(this);
	payloadSeparationConnector.SetSIVb(this);
	sivbSIConnector.SetSIVb(this);

	if (!PanelSDKInitalised)
	{
		Panelsdk.RegisterVessel(this);
		Panelsdk.InitFromFile("ProjectApollo\\SIVBSystems");
		PanelSDKInitalised = true;
	}

	MainBattery = static_cast<Battery *> (Panelsdk.GetPointerByString("ELECTRIC:POWER_BATTERY"));
}

void SIVB::Boiloff()

{
	//
	// The SIVB stage boils off a small amount of fuel while in orbit.
	//
	// For the time being we'll ignore any thrust created by the venting
	// of this fuel.
	//

	if (ph_main) {
		double NewFuelMass = GetPropellantMass(ph_main) * 0.99998193;
		SetPropellantMass(ph_main, NewFuelMass);
	}
}

bool SIVB::GetDockingPortFromHandle(OBJHANDLE port, UINT &num)
{
	if (port == NULL) return false;

	for (UINT i = 0;i < DockCount();i++)
	{
		if (port == GetDockHandle(i))
		{
			num = i;
			return true;
		}
	}

	return false;
}

bool SIVB::IsLowerStageDocked()
{
	if (hDockSI && GetDockStatus(hDockSI))
	{
		return true;
	}

	return false;
}

void SIVB::CreateSISIVBInterface()
{
	if (hDockSI == NULL)
	{
		//TBD: position could maybe use some tweaking
		hDockSI = CreateDock(_V(0.0, 0, -5.5), _V(0, 0, -1), _V(0, 1, 0));
	}
}

void SIVB::CreateAirfoils()
{
	double cw_z_pos, cw_z_neg;
	if (hDockCSM && GetDockStatus(hDockCSM))
	{
		cw_z_pos = 0.01;
	}
	else
	{
		cw_z_pos = 0.2;
	}
	if (hDockSI && GetDockStatus(hDockSI))
	{
		cw_z_neg = 0.01;
	}
	else
	{
		cw_z_neg = 0.3;
	}

	SetCW(cw_z_pos, cw_z_neg, 1.4, 1.4);
}

void SIVB::SetS4b()

{
	double mass = EmptyMass;

	ClearThrusterDefinitions();
	SetSize (15);
	SetPMI (_V(94,94,20));
	SetCOG_elev (10);
	SetCrossSections (_V(267, 267, 97));
	CreateAirfoils();
	SetRotDrag (_V(0.7,0.7,1.2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	HideAllMeshes();

	double TCPS4B = -11;

	double MassS4 = 40296;
	double ro = 7;
	TOUCHDOWNVTX td[4];
	double x_target = -0.1;
	double stiffness = (-1)*(MassS4*9.80655) / (3 * x_target);
	double damping = 0.9*(2 * sqrt(MassS4*stiffness));
	for (int i = 0; i<4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = -cos(30 * RAD)*ro;
	td[0].pos.y = -sin(30 * RAD)*ro;
	td[0].pos.z = TCPS4B;
	td[1].pos.x = 0;
	td[1].pos.y = 1 * ro;
	td[1].pos.z = TCPS4B;
	td[2].pos.x = cos(30 * RAD)*ro;
	td[2].pos.y = -sin(30 * RAD)*ro;
	td[2].pos.z = TCPS4B;
	td[3].pos.x = 0;
	td[3].pos.y = 0;
	td[3].pos.z = TCPS4B + 25;

	SetTouchdownPoints(td, 4);

	VECTOR3 dockpos = { 0,0.03, 12.6 };
	VECTOR3 dockdir = { 0,0,1 };
	VECTOR3 dockrot = { -0.8660254, -0.5, 0 };

	if (SaturnVStage)
	{
		if (LowRes) {
			SetMeshVisibilityMode(meshSivbSaturnVLow, MESHVIS_EXTERNAL);
		}
		else {
			SetMeshVisibilityMode(meshSivbSaturnV, MESHVIS_EXTERNAL);
		}

        // Hide unneeded meshes
        if (PanelsHinged || !PanelsOpened) {
			if (LowRes) {
				SetMeshVisibilityMode(panelMesh1SaturnVLow, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh2SaturnVLow, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh3SaturnVLow, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh4SaturnVLow, MESHVIS_EXTERNAL);
			}
			else {
				SetMeshVisibilityMode(panelMesh1SaturnV, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh2SaturnV, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh3SaturnV, MESHVIS_EXTERNAL);
				SetMeshVisibilityMode(panelMesh4SaturnV, MESHVIS_EXTERNAL);
			}
		}
	}
	else {
		if (LowRes) {
			SetMeshVisibilityMode(meshSivbSaturn1bLow, MESHVIS_EXTERNAL);
		}
		else {
			SetMeshVisibilityMode(meshSivbSaturn1b, MESHVIS_EXTERNAL);
		}

        // Hide unneeded meshes
        if (PanelsHinged || !PanelsOpened) {
			SetMeshVisibilityMode(panelMesh1Saturn1b, MESHVIS_EXTERNAL);
			SetMeshVisibilityMode(panelMesh2Saturn1b, MESHVIS_EXTERNAL);
			SetMeshVisibilityMode(panelMesh3Saturn1b, MESHVIS_EXTERNAL);
			SetMeshVisibilityMode(panelMesh4Saturn1b, MESHVIS_EXTERNAL);
       }
	}


	int i = 0;

	switch (PayloadType) {
	case PAYLOAD_LEM:
	case PAYLOAD_LM1:
		dockpos = { 0, 0, 9.0 };
		SetDockParams(dockpos, dockdir, dockrot);
		hDock = GetDockHandle(0);
		RegisterConnector(i, &payloadSeparationConnector);
		CreatePayload();
		i++;
		break;

	case PAYLOAD_LTA:
	case PAYLOAD_LTA6:
		SetMeshVisibilityMode(meshLTA_2r, MESHVIS_EXTERNAL);
		ClearDockDefinitions();
		mass += PayloadMass;
		break;

	case PAYLOAD_LTA8:
		SetMeshVisibilityMode(meshApollo8LTA, MESHVIS_EXTERNAL);
		ClearDockDefinitions();
		mass += PayloadMass;
		break;

	//
	// For now the docking adapter for the SIVB to Venus test flights is simulated
	// with the ASTP mesh and COAS target.
	//

	case PAYLOAD_DOCKING_ADAPTER:
		SetMeshVisibilityMode(meshASTP_A, MESHVIS_EXTERNAL);
		SetMeshVisibilityMode(meshCOASTarget_A, MESHVIS_EXTERNAL);
		dockpos = _V(0.0, 0.0, 9.1);
		dockrot = _V(-1.0, 0.0, 0);
		SetDockParams(dockpos, dockdir, dockrot);
		hDock = GetDockHandle(0);
		hattDROGUE = CreateAttachment(true, dockpos, dockdir, dockrot, "PADROGUE");
		mass += PayloadMass;
		break;

	case PAYLOAD_EMPTY:
		ClearDockDefinitions();
		break;

	case PAYLOAD_TARGET:
		SetMeshVisibilityMode(meshCOASTarget_B, MESHVIS_EXTERNAL);
		if(SaturnVStage == false) SetMeshVisibilityMode(meshSivbSaturn1bcross, MESHVIS_EXTERNAL);
		ClearDockDefinitions();
		mass += PayloadMass;
		break;

	case PAYLOAD_ASTP:
		dockpos = _V(0.0, 0.16, 8.5);
		dockrot = _V(-1.0, 0.0, 0);
		SetDockParams(dockpos, dockdir, dockrot);
		hDock = GetDockHandle(0);
		CreatePayload();
		break;
	}

	//Docking port for the connection to the lower stage (S-IB or S-II)
	CreateSISIVBInterface();
	RegisterConnector(i, &sivbSIConnector);
	//Docking port for the connection to the stage attached to the front of the SLA (usually CSM, also Apollo 5 nosecap)
	hDockCSM = CreateDock(_V(0.0, 0, 14.8), _V(0, 0, 1), _V(1, 0, 0));

	SetEmptyMass(mass);
	SetAnimation(panelAnim, panelProc);
	SetAnimation(panelAnimPlusX, panelProcPlusX);

	AddRCS_S4B();

	if (PayloadType == PAYLOAD_DOCKING_ADAPTER)
	{
		iu->SetMissionInfo(true, IUSCContPermanentEnabled);

		//
		// Set up the IU connections.
		//

	}
	iu->ConnectToLV(&IUCommandConnector);
}

void SIVB::clbkPreStep(double simt, double simdt, double mjd)
{
	if (FirstTimestep)
	{
		FirstTimestep = false;
		return;
	}

	MissionTime += simdt;

	//
	// Seperate or open the SLA panels.
	//

	if (SLAPanelDeployInitiator.Blown())
	{
		if (panelTimestepCount < 2) {
			panelTimestepCount++;
		}
		else {
			if (PanelsHinged) {
				if (panelProc < RotationLimit) {
					// Activate separation junk
					if (thg_sep)
						SetThrusterGroupLevel(thg_sep, 1);

					panelProc = min(RotationLimit, panelProc + simdt / 40.0);
					SetAnimation(panelAnim, panelProc);
				}
				// Special handling Apollo 7
				if (VehicleNo == 205) {
					// The +X (+Y in Apollo axes) moved to about 25° only at first, 
					// during the rendezvous in orbit 19 (about MET 30h) the panel was found 
					// hinged completely, so we do that at MET 15h (see Mission Report 11.7)
					if (MissionTime < 15. * 3600.) {
						if (panelProcPlusX < 0.1388) {
							panelProcPlusX = min(0.1388, panelProcPlusX + simdt / 40.0);
							SetAnimation(panelAnimPlusX, panelProcPlusX);
						}
					}
					else if (panelProcPlusX < RotationLimit) {
						panelProcPlusX = min(RotationLimit, panelProcPlusX + simdt / 40.0);
						SetAnimation(panelAnimPlusX, panelProcPlusX);
					}
				}
				else if (panelProcPlusX < RotationLimit) {
					panelProcPlusX = min(RotationLimit, panelProcPlusX + simdt / 40.0);
					SetAnimation(panelAnimPlusX, panelProcPlusX);
				}
			}
			else if (!PanelsOpened) {
				if (panelProc < RotationLimit) {
					// Activate separation junk
					if (thg_sep)
						SetThrusterGroupLevel(thg_sep, 1);

					panelProc = min(RotationLimit, panelProc + simdt / 40.0);
					SetAnimation(panelAnim, panelProc);
				}
				else {
					char VName[256];

					//
					// I'm not sure that all this code is really needed, but
					// it came from saturn1bmesh.cpp.
					//

					VESSELSTATUS vs2;
					VESSELSTATUS vs3;
					VESSELSTATUS vs4;
					VESSELSTATUS vs5;
					VECTOR3 ofs2 = _V(0, 0, 0);
					VECTOR3 ofs3 = _V(0, 0, 0);
					VECTOR3 ofs4 = _V(0, 0, 0);
					VECTOR3 ofs5 = _V(0, 0, 0);
					VECTOR3 vel2 = _V(0, 0, 0);
					VECTOR3 vel3 = _V(0, 0, 0);
					VECTOR3 vel4 = _V(0, 0, 0);
					VECTOR3 vel5 = _V(0, 0, 0);

					GetStatus(vs2);
					GetStatus(vs3);
					GetStatus(vs4);
					GetStatus(vs5);

					vs2.eng_main = vs2.eng_hovr = 0.0;
					vs3.eng_main = vs3.eng_hovr = 0.0;
					vs4.eng_main = vs4.eng_hovr = 0.0;
					vs5.eng_main = vs5.eng_hovr = 0.0;

					if (SaturnVStage) {
						ofs2 = _V(-3.25, -3.3, 12.2);
						vel2 = _V(-0.5, -0.5, -0.55);
						ofs3 = _V(3.25, -3.3, 12.2);
						vel3 = _V(0.5, -0.5, -0.55);
						ofs4 = _V(3.25, 3.3, 12.2);
						vel4 = _V(0.5, 0.5, -0.55);
						ofs5 = _V(-3.25, 3.3, 12.2);
						vel5 = _V(-0.5, 0.5, -0.55);
					}
					else {
						ofs2 = _V(2.5, 2.5, 15.0);
						vel2 = _V(0.5, 0.5, -0.55);
						ofs3 = _V(-2.5, 2.5, 15.0);
						vel3 = _V(-0.5, 0.5, -0.55);
						ofs4 = _V(2.5, -2.5, 15.0);
						vel4 = _V(0.5, -0.5, -0.55);
						ofs5 = _V(-2.5, -2.5, 15.0);
						vel5 = _V(-0.5, -0.5, -0.55);
					}

					VECTOR3 rofs2, rvel2 = { vs2.rvel.x, vs2.rvel.y, vs2.rvel.z };
					VECTOR3 rofs3, rvel3 = { vs3.rvel.x, vs3.rvel.y, vs3.rvel.z };
					VECTOR3 rofs4, rvel4 = { vs4.rvel.x, vs4.rvel.y, vs4.rvel.z };
					VECTOR3 rofs5, rvel5 = { vs5.rvel.x, vs5.rvel.y, vs5.rvel.z };
					Local2Rel(ofs2, vs2.rpos);
					Local2Rel(ofs3, vs3.rpos);
					Local2Rel(ofs4, vs4.rpos);
					Local2Rel(ofs5, vs5.rpos);
					GlobalRot(vel2, rofs2);
					GlobalRot(vel3, rofs3);
					GlobalRot(vel4, rofs4);
					GlobalRot(vel5, rofs5);
					vs2.rvel.x = rvel2.x + rofs2.x;
					vs2.rvel.y = rvel2.y + rofs2.y;
					vs2.rvel.z = rvel2.z + rofs2.z;
					vs3.rvel.x = rvel3.x + rofs3.x;
					vs3.rvel.y = rvel3.y + rofs3.y;
					vs3.rvel.z = rvel3.z + rofs3.z;
					vs4.rvel.x = rvel4.x + rofs4.x;
					vs4.rvel.y = rvel4.y + rofs4.y;
					vs4.rvel.z = rvel4.z + rofs4.z;
					vs5.rvel.x = rvel5.x + rofs5.x;
					vs5.rvel.y = rvel5.y + rofs5.y;
					vs5.rvel.z = rvel5.z + rofs5.z;

					//
					// This should be rationalised really to use the same parameters
					// with different config files.
					//

					if (SaturnVStage) {
						vs2.vrot.x = -0.1;
						vs2.vrot.y = 0.1;
						vs2.vrot.z = 0.0;
						vs3.vrot.x = -0.1;
						vs3.vrot.y = -0.1;
						vs3.vrot.z = 0.0;
						vs4.vrot.x = 0.1;
						vs4.vrot.y = -0.1;
						vs4.vrot.z = 0.0;
						vs5.vrot.x = 0.1;
						vs5.vrot.y = 0.1;
						vs5.vrot.z = 0.0;

						GetApolloName(VName); strcat(VName, "-S4B1");
						hs4b1 = oapiCreateVessel(VName, "ProjectApollo/sat5stg31", vs2);
						GetApolloName(VName); strcat(VName, "-S4B2");
						hs4b2 = oapiCreateVessel(VName, "ProjectApollo/sat5stg32", vs3);
						GetApolloName(VName); strcat(VName, "-S4B3");
						hs4b3 = oapiCreateVessel(VName, "ProjectApollo/sat5stg33", vs4);
						GetApolloName(VName); strcat(VName, "-S4B4");
						hs4b4 = oapiCreateVessel(VName, "ProjectApollo/sat5stg34", vs5);

						MATRIX3 rv, rx, ry, rz, rnx, rny, rnz;
						GetRotationMatrix(rv);
						GetRotMatrixX(34.5 * RAD, rx);
						GetRotMatrixX(-34.5 * RAD, rnx);
						GetRotMatrixY(30 * RAD, ry);
						GetRotMatrixY(-30 * RAD, rny);
						GetRotMatrixZ(9.5 * RAD, rz);
						GetRotMatrixZ(-9.5 * RAD, rnz);

						VESSEL *v = oapiGetVesselInterface(hs4b1);
						v->SetRotationMatrix(mul(rv, mul(rz, mul(ry, rnx))));
						v = oapiGetVesselInterface(hs4b2);
						v->SetRotationMatrix(mul(rv, mul(rnz, mul(rny, rnx))));
						v = oapiGetVesselInterface(hs4b3);
						v->SetRotationMatrix(mul(rv, mul(rz, mul(rny, rx))));
						v = oapiGetVesselInterface(hs4b4);
						v->SetRotationMatrix(mul(rv, mul(rnz, mul(ry, rx))));

						// Hide unneeded meshes
						SetMeshVisibilityMode(panelMesh1SaturnV, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh2SaturnV, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh3SaturnV, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh4SaturnV, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh1SaturnVLow, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh2SaturnVLow, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh3SaturnVLow, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh4SaturnVLow, MESHVIS_NEVER);
					}
					else {
						vs2.vrot.x = 0.1;
						vs2.vrot.y = -0.1;
						vs2.vrot.z = 0.0;
						vs3.vrot.x = 0.1;
						vs3.vrot.y = 0.1;
						vs3.vrot.z = 0.0;
						vs4.vrot.x = -0.1;
						vs4.vrot.y = -0.1;
						vs4.vrot.z = 0.0;
						vs5.vrot.x = -0.1;
						vs5.vrot.y = 0.1;
						vs5.vrot.z = 0.0;

						GetApolloName(VName); strcat(VName, "-S4B1");
						hs4b1 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg21", vs2);
						GetApolloName(VName); strcat(VName, "-S4B2");
						hs4b2 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg22", vs3);
						GetApolloName(VName); strcat(VName, "-S4B3");
						hs4b3 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg23", vs4);
						GetApolloName(VName); strcat(VName, "-S4B4");
						hs4b4 = oapiCreateVessel(VName, "ProjectApollo/nsat1stg24", vs5);

						MATRIX3 rv, rx, ry, rz, rnx, rny, rnz;
						GetRotationMatrix(rv);
						GetRotMatrixX(34.5 * RAD, rx);
						GetRotMatrixX(-34.5 * RAD, rnx);
						GetRotMatrixY(30.8 * RAD, ry);
						GetRotMatrixY(-30.8 * RAD, rny);
						GetRotMatrixZ(8.5 * RAD, rz);
						GetRotMatrixZ(-8.5 * RAD, rnz);

						VESSEL *v = oapiGetVesselInterface(hs4b1);
						v->SetRotationMatrix(mul(rv, mul(rz, mul(rny, rx))));
						v = oapiGetVesselInterface(hs4b2);
						v->SetRotationMatrix(mul(rv, mul(rnz, mul(ry, rx))));
						v = oapiGetVesselInterface(hs4b3);
						v->SetRotationMatrix(mul(rv, mul(rnz, mul(rny, rnx))));
						v = oapiGetVesselInterface(hs4b4);
						v->SetRotationMatrix(mul(rv, mul(rz, mul(ry, rnx))));

						// Hide unneeded meshes
						SetMeshVisibilityMode(panelMesh1Saturn1b, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh2Saturn1b, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh3Saturn1b, MESHVIS_NEVER);
						SetMeshVisibilityMode(panelMesh4Saturn1b, MESHVIS_NEVER);
					}

					// Activate panel separation junk
					if (thg_sepPanel)
						SetThrusterGroupLevel(thg_sepPanel, 1);

					PanelsOpened = true;
				}
			}
		}
	}

	//
	// Now update whatever needs updating.
	//

	switch (State) {
	case SIVB_STATE_WAITING:

		//
		// If we still have fuel left, boil some off.
		//

		if (MissionTime >= NextMissionEventTime) {
			Boiloff();
			NextMissionEventTime = MissionTime + 10.0;
		}
		break;
	}

	//
	// For a Saturn V SIVB, at some point it will dump all remaining fuel out the engine nozzle to
	// thrust it out of the way of the CSM.
	//

	sivbsys->Timestep(simdt);
	iu->Timestep(MissionTime, simt, simdt, mjd);
	Panelsdk.Timestep(MissionTime);
}

void SIVB::clbkPostStep(double simt, double simdt, double mjd)
{
	iu->PostStep(simt, simdt, mjd);
}

void SIVB::GetApolloName(char *s)

{
	sprintf(s, "AS-%d", VehicleNo);
}


void SIVB::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "S4PL", PayloadType);
	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "PMASS", PayloadMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float(scn, "APSFMASS1", ApsFuel1Kg);
	oapiWriteScenario_float(scn, "APSFMASS2", ApsFuel2Kg);
	oapiWriteScenario_float (scn, "T3V", THRUST_THIRD_VAC);
	oapiWriteScenario_float (scn, "I3V", ISP_THIRD_VAC);
	oapiWriteScenario_int(scn, "MISSIONNO", payloadSettings.MissionNo);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "CTR", CurrentThrust);
	oapiWriteScenario_float (scn, "PANELPROC", panelProc);
	oapiWriteScenario_float (scn, "PANELPROCPLUSX", panelProcPlusX);
	oapiWriteScenario_float (scn, "ROTL", RotationLimit);
	oapiWriteScenario_float(scn, "PAYLOADEJECTIONFORCE", PayloadEjectionForce);

	if (PayloadName[0])
	{
		oapiWriteScenario_string (scn, "PAYN", PayloadName);
	}
	if (!Payloaddatatransfer)
	{
		if (payloadSettings.checklistFile[0]) {
			oapiWriteScenario_string(scn, "LEMCHECK", payloadSettings.checklistFile);
		}
		oapiWriteScenario_float (scn, "LMDSCFUEL", payloadSettings.DescentFuelKg);
		oapiWriteScenario_float (scn, "LMASCFUEL", payloadSettings.AscentFuelKg);
		oapiWriteScenario_float(scn, "LMDSCEMPTY", payloadSettings.DescentEmptyKg);
		oapiWriteScenario_float(scn, "LMASCEMPTY", payloadSettings.AscentEmptyKg);
		if (LMPadCount > 0 && LMPad) {
			oapiWriteScenario_int (scn, "LMPADCNT", LMPadCount);
			char str[64];
			for (int i = 0; i < LMPadCount; i++) {
				sprintf(str, "%04o %05o", LMPad[i * 2], LMPad[i * 2 + 1]);
				oapiWriteScenario_string (scn, "LMPAD", str);
			}
		}
		if (AEAPadCount > 0 && AEAPad) {
			oapiWriteScenario_int(scn, "AEAPADCNT", AEAPadCount);
			char str[64];
			for (int i = 0; i < AEAPadCount; i++) {
				sprintf(str, "%04o %05o", AEAPad[i * 2], AEAPad[i * 2 + 1]);
				oapiWriteScenario_string(scn, "AEAPAD", str);
			}
		}
	}

	sivbsys->SaveState(scn);
	iu->SaveState(scn);
	iu->SaveLVDC(scn);
	Panelsdk.Save(scn);
}

int SIVB::GetMainState()

{
	MainState state;

	state.word = 0;
	state.PanelsHinged = PanelsHinged;
	state.PanelsOpened = PanelsOpened;
	state.SaturnVStage = SaturnVStage;
	state.LowRes = LowRes;
	state.Payloaddatatransfer = Payloaddatatransfer;
	state.IUSCContPermanentEnabled = IUSCContPermanentEnabled;
	state.PayloadCreated = PayloadCreated;

	return state.word;
}

double SIVB::GetMainBatteryPower()

{
	if (MainBattery)
	{
		return MainBattery->Capacity();
	}

	return 0.0;
}


double SIVB::GetMainBatteryPowerDrain()

{
	if (MainBattery)
	{
		return MainBattery->Voltage() * MainBattery->Current();
	}

	return 0.0;
}

double SIVB::GetMainBatteryVoltage()

{
	if (MainBattery)
	{
		return MainBattery->Voltage();
	}

	return 0.0;
}


double SIVB::GetMainBatteryCurrent()

{
	if (MainBattery)
	{
		return MainBattery->Current();
	}

	return 0.0;
}

void SIVB::AddRCS_S4B()

{
	const double ATTCOOR2 = 3.61;
	const double TRANZ=-3.2;
	const double RCSX=0.35;
	VECTOR3 m_exhaust_pos2= {0,ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos3= {0,-ATTCOOR2,TRANZ};
	VECTOR3 m_exhaust_pos4= {-ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_pos5= {ATTCOOR2,0,TRANZ};
	VECTOR3 m_exhaust_ref2 = {0,0.1,-1};
	VECTOR3 m_exhaust_ref3 = {0,-0.1,-1};
	VECTOR3 m_exhaust_ref4 = {-0.1,0,-1};
	VECTOR3 m_exhaust_ref5 = {0.1,0,-1};
	double offset = -2.05;

	if (!ph_aps1)
		ph_aps1  = CreatePropellantResource(S4B_APS_FUEL_PER_TANK, ApsFuel1Kg);

	if (!ph_aps2)
		ph_aps2 = CreatePropellantResource(S4B_APS_FUEL_PER_TANK, ApsFuel2Kg);

	if (!ph_main)
		ph_main = CreatePropellantResource(MainFuel);

	SetDefaultPropellantResource (ph_main);
	
	mainExhaustPos = _V(0, 0, -11.7);

	//
	// Unless this is dockable, the main engine is just venting fuel through the exhaust: low thrust and low ISP.
	//

	//
	// Clear old thrusters.
	//
	if (thg_main)
		DelThrusterGroup(THGROUP_MAIN, true);

	th_main[0] = CreateThruster(mainExhaustPos, _V(0, 0, 1), THRUST_THIRD_VAC, ph_main, ISP_THIRD_VAC);
	thg_main = CreateThrusterGroup(th_main, 1, THGROUP_MAIN);
	
	EXHAUSTSPEC es_main[1] = {
		{ th_main[0], NULL, NULL, NULL, 30.0, 2.9, 0, 0.1, SMMETex }
	};

	AddExhaust(es_main);

	sivbsys->RecalculateEngineParameters(THRUST_THIRD_VAC);

	// LOX venting thruster

	th_lox_vent = CreateThruster(mainExhaustPos, _V(0, 0, 1), 3300.0, ph_main, 157.0, 157.0);

	AddExhaustStream(th_lox_vent, &fuel_venting_spec);

	//
	// Rotational thrusters are 150lb (666N) thrust. ISP is estimated at 3000.0.
	//

	SURFHANDLE SIVBRCSTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");

	th_aps_rot[0] = CreateThruster (_V(0,ATTCOOR2+0.15,TRANZ-0.25+offset), _V(0, -1,0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[1] = CreateThruster (_V(0,-ATTCOOR2-0.15,TRANZ-0.25+offset), _V(0,1,0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);
	
	AddExhaust (th_aps_rot[0], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[1], 0.6, 0.078, SIVBRCSTex);

	th_aps_rot[2] = CreateThruster (_V(RCSX,ATTCOOR2-0.2,TRANZ-0.25+offset), _V(-1,0,0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[3] = CreateThruster (_V(-RCSX,-ATTCOOR2+0.2,TRANZ-0.25+offset), _V( 1,0,0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[4] = CreateThruster (_V(-RCSX,ATTCOOR2-.2,TRANZ-0.25+offset), _V( 1,0,0), S4B_APS_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_rot[5] = CreateThruster (_V(RCSX,-ATTCOOR2+.2,TRANZ-0.25+offset), _V(-1,0,0), S4B_APS_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);

	AddExhaust (th_aps_rot[2], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[3], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[4], 0.6, 0.078, SIVBRCSTex);
	AddExhaust (th_aps_rot[5], 0.6, 0.078, SIVBRCSTex);

	//
	// APS linear thrusters are 310N (72 pounds) thrust
	//
	th_aps_ull[0] = CreateThruster (_V(0,ATTCOOR2-0.15,TRANZ-.25+offset), _V(0,0,1), S4B_APS_ULL_THRUST, ph_aps1, S4B_APS_ISP, S4B_APS_ISP_SL);
	th_aps_ull[1] = CreateThruster (_V(0,-ATTCOOR2+.15,TRANZ-.25+offset), _V(0,0,1), S4B_APS_ULL_THRUST, ph_aps2, S4B_APS_ISP, S4B_APS_ISP_SL);
	AddExhaust (th_aps_ull[0], 7, 0.15, SIVBRCSTex);
	AddExhaust (th_aps_ull[1], 7, 0.15, SIVBRCSTex);

	//
	// Seperation junk 'thrusters'.
	//

	int i;
	double junkOffset;
	junkOffset = 16;

	VECTOR3	s_exhaust_pos1= _V(1.41,1.41,junkOffset);
	VECTOR3 s_exhaust_pos2= _V(1.41,-1.41,junkOffset);
	VECTOR3	s_exhaust_pos3= _V(-1.41,1.41,junkOffset);
	VECTOR3 s_exhaust_pos4= _V(-1.41,-1.41,junkOffset);
	VECTOR3	s_exhaust_pos6= _V(0,2.0,junkOffset);
	VECTOR3 s_exhaust_pos7= _V(0,-2.0,junkOffset);
	VECTOR3	s_exhaust_pos8= _V(2.0,0,junkOffset);
	VECTOR3 s_exhaust_pos9= _V(-2.0,0,junkOffset);

	PROPELLANT_HANDLE ph_sep = CreatePropellantResource(0.2);

	THRUSTER_HANDLE th_sep[8];
	th_sep[0] = CreateThruster (s_exhaust_pos1, _V( -1,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[1] = CreateThruster (s_exhaust_pos2, _V( -1,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[2] = CreateThruster (s_exhaust_pos3, _V( 1,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[3] = CreateThruster (s_exhaust_pos4, _V( 1,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[4] = CreateThruster (s_exhaust_pos6, _V( 0,-1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[5] = CreateThruster (s_exhaust_pos7, _V( 0,1,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[6] = CreateThruster (s_exhaust_pos8, _V( -1,0,0), 1.0, ph_sep, 10.0, 10.0);
	th_sep[7] = CreateThruster (s_exhaust_pos9, _V( 1,0,0), 1.0, ph_sep, 10.0, 10.0);

	for (i = 0; i < 8; i++) {
		AddExhaustStream(th_sep[i], &seperation_junk);
	}
	thg_sep = CreateThrusterGroup(th_sep, 8, THGROUP_USER);

	//
	// Panel seperation junk 'thrusters'.
	//

	junkOffset = 9.4;

	double r = 2.15;
	VECTOR3	sPanel_exhaust_pos1= _V(r,r,junkOffset);
	VECTOR3 sPanel_exhaust_pos2= _V(r,-r,junkOffset);
	VECTOR3	sPanel_exhaust_pos3= _V(-r,r,junkOffset);
	VECTOR3 sPanel_exhaust_pos4= _V(-r,-r,junkOffset);

	PROPELLANT_HANDLE ph_sepPanel = CreatePropellantResource(0.1);

	THRUSTER_HANDLE th_sepPanel[4];
	th_sepPanel[0] = CreateThruster (sPanel_exhaust_pos1, _V( -1,-1,0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[1] = CreateThruster (sPanel_exhaust_pos2, _V( -1,1,0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[2] = CreateThruster (sPanel_exhaust_pos3, _V( 1,-1,0), 1.0, ph_sepPanel, 10.0, 10.0);
	th_sepPanel[3] = CreateThruster (sPanel_exhaust_pos4, _V( 1,1,0), 1.0, ph_sepPanel, 10.0, 10.0);

	for (i = 0; i < 4; i++) {
		AddExhaustStream(th_sepPanel[i], &seperation_junk);
	}
	thg_sepPanel = CreateThrusterGroup(th_sepPanel, 4, THGROUP_USER);
}

void SIVB::SetMainState(int s)

{
	MainState state;

	state.word = s;

	SaturnVStage = (state.SaturnVStage != 0);
	PanelsHinged = (state.PanelsHinged != 0);
	PanelsOpened = (state.PanelsOpened != 0);
	LowRes = (state.LowRes != 0);
	Payloaddatatransfer = (state.Payloaddatatransfer != 0);
	IUSCContPermanentEnabled = (state.IUSCContPermanentEnabled != 0);
	PayloadCreated = (state.PayloadCreated != 0);
}

void SIVB::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline (scn, line))
	{
		if (!strnicmp(line, "S4PL", 4))
		{
			sscanf(line + 4, "%d", &PayloadType);
		}
		else if (!strnicmp (line, "MAINSTATE", 9))
		{
            int MainState = 0;;
			sscanf (line+9, "%d", &MainState);
			SetMainState(MainState);

			if (SaturnVStage)
			{
				sivbsys = new SIVB500Systems(this, th_main[0], ph_main, th_aps_rot, th_aps_ull, th_lox_vent, thg_ver);
				iu = new IUSV;
			}
			else
			{
				sivbsys = new SIVB200Systems(this, th_main[0], ph_main, th_aps_rot, th_aps_ull, th_lox_vent, thg_ver);
				iu = new IU1B;
			}
		}
		else if (!strnicmp (line, "VECHNO", 6))
		{
			sscanf (line+6, "%d", &VehicleNo);
		}
		else if (!strnicmp (line, "EMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			EmptyMass = flt;
		}
		else if (!strnicmp (line, "PMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			PayloadMass = flt;
		}
		else if (!strnicmp (line, "FMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			MainFuel = flt;
		}
		else if (!strnicmp(line, "APSFMASS1", 9))
		{
			sscanf(line + 9, "%g", &flt);
			ApsFuel1Kg = flt;
		}
		else if (!strnicmp(line, "APSFMASS2", 9))
		{
			sscanf(line + 9, "%g", &flt);
			ApsFuel2Kg = flt;
		}
		else if (!strnicmp(line, "T3V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			THRUST_THIRD_VAC = flt;
		}
		else if (!strnicmp(line, "I3V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_THIRD_VAC = flt;
		}
		else if (!strnicmp(line, "MISSIONNO", 9))
		{
			sscanf(line + 9, "%d", &payloadSettings.MissionNo);
		}
		else if (!strnicmp(line, "MISSNTIME", 9))
		{
            sscanf (line+9, "%f", &flt);
			MissionTime = flt;
		}
		else if (!strnicmp(line, "NMISSNTIME", 10))
		{
            sscanf (line + 10, "%f", &flt);
			NextMissionEventTime = flt;
		}
		else if (!strnicmp(line, "LMISSNTIME", 10))
		{
            sscanf (line + 10, "%f", &flt);
			LastMissionEventTime = flt;
		}
		else if (!strnicmp(line, "CTR", 3))
		{
            sscanf (line + 3, "%f", &flt);
			CurrentThrust = flt;
		}
		else if (!strnicmp(line, "ROTL", 4))
		{
            sscanf (line + 4, "%f", &flt);
			RotationLimit = flt;
		}
		else if (!strnicmp(line, "PANELPROCPLUSX", 14))
		{
            sscanf (line + 14, "%f", &flt);
			panelProcPlusX = flt;
		}
		else if (!strnicmp(line, "PANELPROC", 9))
		{
            sscanf (line + 9, "%f", &flt);
			panelProc = flt;
		}
		else if (!strnicmp(line, "PAYLOADEJECTIONFORCE", 20))
		{
			sscanf(line + 20, "%f", &flt);
			PayloadEjectionForce = flt;
		}
		else if (!strnicmp (line, "STATE", 5))
		{
			int i;
			sscanf (line+5, "%d", &i);
			State = (SIVbState) i;
		}
		else if (!strnicmp(line, "LEMCHECK", 8)) {
			strcpy(payloadSettings.checklistFile, line + 9);
		}
		else if (!strnicmp(line, "PAYN", 4)) {
			strncpy (PayloadName, line + 5, 64);
		}
		else if (!strnicmp(line, "LMDSCFUEL", 9)) {
			sscanf(line + 9, "%f", &flt);
			payloadSettings.DescentFuelKg = flt;
		}
		else if (!strnicmp(line, "LMASCFUEL", 9)) {
			sscanf(line + 9, "%f", &flt);
			payloadSettings.AscentFuelKg = flt;
		}
		else if (!strnicmp(line, "LMDSCEMPTY", 10)) {
			sscanf(line + 10, "%f", &flt);
			payloadSettings.DescentEmptyKg = flt;
		}
		else if (!strnicmp(line, "LMASCEMPTY", 10)) {
			sscanf(line + 10, "%f", &flt);
			payloadSettings.AscentEmptyKg = flt;
		}
		else if (!strnicmp (line, "LMPADCNT", 8)) {
			if (!LMPad) {
				sscanf (line+8, "%d", &LMPadCount);
				if (LMPadCount > 0) {
					LMPad = new unsigned int[LMPadCount * 2];
				}
			}
		}
		else if (!strnicmp (line, "LMPAD", 5)) {
			unsigned int addr, val;
			sscanf (line+5, "%o %o", &addr, &val);
			LMPadValueCount++;
			if (LMPad && LMPadLoadCount < (LMPadCount * 2)) {
				LMPad[LMPadLoadCount++] = addr;
				LMPad[LMPadLoadCount++] = val;
			}
		}
		else if (!strnicmp(line, "AEAPADCNT", 9)) {
			if (!AEAPad) {
				sscanf(line + 9, "%d", &AEAPadCount);
				if (AEAPadCount > 0) {
					AEAPad = new unsigned int[AEAPadCount * 2];
				}
			}
		}
		else if (!strnicmp(line, "AEAPAD", 6)) {
			unsigned int addr, val;
			sscanf(line + 6, "%o %o", &addr, &val);
			AEAPadValueCount++;
			if (AEAPad && AEAPadLoadCount < (AEAPadCount * 2)) {
				AEAPad[AEAPadLoadCount++] = addr;
				AEAPad[AEAPadLoadCount++] = val;
			}
		}
		else if (!strnicmp(line, SIVBSYSTEMS_START_STRING, sizeof(SIVBSYSTEMS_START_STRING))) {
			sivbsys->LoadState(scn);
		}
		else if (!strnicmp(line, IU_START_STRING, sizeof(IU_START_STRING))) {
			iu->LoadState(scn);
		}
		else if (!strnicmp(line, LVDC_START_STRING, sizeof(LVDC_START_STRING))) {
			iu->LoadLVDC(scn);
		}
		else if (!strnicmp (line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
			Panelsdk.Load(scn);			//send the loading to the Panelsdk
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}

	SetS4b();
}

void SIVB::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	// Switch to compatible dock mode 
	SetDockMode(0);

	InitS4b();
    
    // Define all animations here and delete the unneeded later,
    // otherwise Orbiter crashes
    ClearMeshes();
	panelAnim = CreateAnimation(0.0);
	panelAnimPlusX = CreateAnimation(0.0);

    // SaturnV panel animations
	VECTOR3 mesh_dir = _V(-1.48, -1.48, 12.55);
	panelMesh1SaturnV = AddMesh(hsat5stg31, &mesh_dir);
	panelMesh1SaturnVLow = AddMesh(hsat5stg31low, &mesh_dir);
	mesh_dir = _V(1.48, -1.48, 12.55);
	panelMesh2SaturnV = AddMesh(hsat5stg32, &mesh_dir);
	panelMesh2SaturnVLow = AddMesh(hsat5stg32low, &mesh_dir);
	mesh_dir = _V(1.48, 1.48, 12.55);
	panelMesh3SaturnV = AddMesh(hsat5stg33, &mesh_dir);
	panelMesh3SaturnVLow = AddMesh(hsat5stg33low, &mesh_dir);
	mesh_dir = _V(-1.48, 1.48, 12.55);
	panelMesh4SaturnV  = AddMesh(hsat5stg34, &mesh_dir);
	panelMesh4SaturnVLow  = AddMesh(hsat5stg34low, &mesh_dir);

	static MGROUP_ROTATE panel1SaturnV(panelMesh1SaturnV, NULL, 0, _V(-0.6, -0.6, -3.2), _V(  1, -1, 0) / length(_V(  1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2SaturnV(panelMesh2SaturnV, NULL, 0, _V( 0.6, -0.6, -3.2), _V(  1,  1, 0) / length(_V(  1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3SaturnV(panelMesh3SaturnV, NULL, 0, _V( 0.6,  0.6, -3.2), _V( -1,  1, 0) / length(_V( -1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4SaturnV(panelMesh4SaturnV, NULL, 0, _V(-0.6,  0.6, -3.2), _V( -1, -1, 0) / length(_V( -1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel1SaturnVLow(panelMesh1SaturnVLow, NULL, 0, _V(-0.6, -0.6, -3.2), _V(  1, -1, 0) / length(_V(  1, -1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2SaturnVLow(panelMesh2SaturnVLow, NULL, 0, _V( 0.6, -0.6, -3.2), _V(  1,  1, 0) / length(_V(  1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3SaturnVLow(panelMesh3SaturnVLow, NULL, 0, _V( 0.6,  0.6, -3.2), _V( -1,  1, 0) / length(_V( -1,  1, 0)), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4SaturnVLow(panelMesh4SaturnVLow, NULL, 0, _V(-0.6,  0.6, -3.2), _V( -1, -1, 0) / length(_V( -1, -1, 0)), (float)(1.0 * PI));

	AddAnimationComponent(panelAnim, 0, 1, &panel1SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel2SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel3SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel4SaturnV);
	AddAnimationComponent(panelAnim, 0, 1, &panel1SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel2SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel3SaturnVLow);
	AddAnimationComponent(panelAnim, 0, 1, &panel4SaturnVLow);

    // Saturn1b panel animations
	mesh_dir = _V(2.45, 0, 10.55);
    panelMesh1Saturn1b = AddMesh(hSat1stg21, &mesh_dir);
	mesh_dir = _V(0, 2.45, 10.55);
	panelMesh2Saturn1b = AddMesh(hSat1stg22, &mesh_dir);
	mesh_dir = _V(0, -2.45, 10.55);
	panelMesh3Saturn1b = AddMesh(hSat1stg23, &mesh_dir);
	mesh_dir = _V(-2.45, 0, 10.55);
	panelMesh4Saturn1b = AddMesh(hSat1stg24, &mesh_dir);

	static MGROUP_ROTATE panel1Saturn1b(panelMesh1Saturn1b, NULL, 0, _V( 0.37,  0,    -1.2), _V(  0,  1, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel2Saturn1b(panelMesh2Saturn1b, NULL, 0, _V( 0,     0.37, -1.2), _V( -1,  0, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel3Saturn1b(panelMesh3Saturn1b, NULL, 0, _V( 0,    -0.37, -1.2), _V(  1,  0, 0), (float)(1.0 * PI));
	static MGROUP_ROTATE panel4Saturn1b(panelMesh4Saturn1b, NULL, 0, _V(-0.37,  0,    -1.2), _V(  0, -1, 0), (float)(1.0 * PI));
    	
	AddAnimationComponent(panelAnimPlusX, 0, 1, &panel1Saturn1b);
	AddAnimationComponent(panelAnim,	  0, 1, &panel2Saturn1b);
	AddAnimationComponent(panelAnim,	  0, 1, &panel3Saturn1b);
	AddAnimationComponent(panelAnim,	  0, 1, &panel4Saturn1b);

	// All other meshes, Add/DelMesh in SetS4b wasn't working...

	//
	// All SIVB meshes are aligned to have the same origin. So we use the same offset
	// for both.
	//
	mesh_dir = _V(0, 0, 0);
	meshSivbSaturnVLow = AddMesh(hsat5stg3low, &mesh_dir);
	meshSivbSaturnV = AddMesh(hsat5stg3, &mesh_dir);
	meshSivbSaturn1bLow = AddMesh(hSat1stg2low, &mesh_dir);
	meshSivbSaturn1b = AddMesh(hSat1stg2, &mesh_dir);
	meshSivbSaturn1bcross = AddMesh(hSat1stg2cross, &mesh_dir);

	mesh_dir = _V(0, 0, 9.6);	
	meshLTA_2r = AddMesh(hlta_2r, &mesh_dir);

	mesh_dir = _V(0.0, 0, 8.8);
	meshApollo8LTA = AddMesh(hapollo8lta, &mesh_dir);

	// ShiftMesh in SetS4b wasn't working...
	mesh_dir = _V(0, -0.15, 7.8);
	meshASTP_A = AddMesh(hastp, &mesh_dir);

	mesh_dir = _V(0, 0, 8.6);
	meshASTP_B = AddMesh(hastp, &mesh_dir);

	// ShiftMesh in SetS4b wasn't working...
	mesh_dir = _V(-1.04, 1.04, 9.1);
	meshCOASTarget_A = AddMesh(hCOAStarget, &mesh_dir);

	mesh_dir = _V(-1.3, 0, 9.6);
	meshCOASTarget_B = AddMesh(hCOAStarget, &mesh_dir);

	mesh_dir = _V(0, 0, 8.6);
	meshCOASTarget_C = AddMesh(hCOAStarget, &mesh_dir);
}

void SIVB::clbkDockEvent(int dock, OBJHANDLE connected)
{
	if (connected)
	{
		DockConnectors(dock);
	}
	else
	{
		UndockConnectors(dock);
		CreateAirfoils();
	}
}

void SIVB::clbkPostCreation()
{
	//Delete the connection to the lower stage (S-II or S-IB) if nothing is docked. S-IB/S-II are the active part of the connection, so there is no pyro to check.
	//If putting stages together in the VAB or on the launchpad becomes supported (again) the docking port will have to be created again at that time
	if (hDockSI && GetDockStatus(hDockSI) == NULL)
	{
		DelDock(hDockSI);
		hDockSI = NULL;
	}
	if (hDockCSM && CSMLVSeparationInitiator.Blown())
	{
		DelDock(hDockCSM);
		hDockCSM = NULL;
	}
	if (LMSLASeparationInitiators.Blown())
	{
		if (hDock)
		{
			DelDock(hDock);
			hDock = NULL;
		}
	}
	CreateAirfoils();
}

void SIVB::SetState(SIVBSettings &state)

{
	if (state.SettingsType.SIVB_SETTINGS_PAYLOAD)
	{
		PayloadType = state.Payload;
	}

	if (state.SettingsType.SIVB_SETTINGS_PAYLOAD_INFO)
	{
		if (state.PayloadName[0])
		{
			strcpy(PayloadName, state.PayloadName);
		}

		if (state.LEMCheck[0]) {
			strcpy(payloadSettings.checklistFile, state.LEMCheck);
		}

		payloadSettings.DescentFuelKg = state.LMDescentFuelMassKg;
		payloadSettings.AscentFuelKg = state.LMAscentFuelMassKg;
		payloadSettings.DescentEmptyKg = state.LMDescentEmptyMassKg;
		payloadSettings.AscentEmptyKg = state.LMAscentEmptyMassKg;

		//
		// Copy LM PAD data across.
		//
		LMPadCount = state.LMPadCount;

		if (LMPadCount > 0) {
			int i;
			LMPad = new unsigned int[LMPadCount * 2];
			for (i = 0; i < (LMPadCount * 2); i++)
			{
				LMPad[i] = state.LMPad[i];
			}
		}

		AEAPadCount = state.AEAPadCount;

		if (AEAPadCount > 0) {
			int i;
			AEAPad = new unsigned int[AEAPadCount * 2];
			for (i = 0; i < (AEAPadCount * 2); i++)
			{
				AEAPad[i] = state.AEAPad[i];
			}
		}
	}

	if (state.SettingsType.SIVB_SETTINGS_GENERAL)
	{
		MissionTime = state.MissionTime;
		SaturnVStage = state.SaturnVStage;
		PanelsHinged = state.PanelsHinged;
		VehicleNo = state.VehicleNo;
		LowRes = state.LowRes;
		IUSCContPermanentEnabled = state.IUSCContPermanentEnabled;
		payloadSettings.MissionNo = state.MissionNo;
		strncpy(payloadSettings.CSMName, state.CSMName, 63);
		payloadSettings.Crewed = state.Crewed;

		//
		// Limit rotation angle to 0-150 degrees.
		//
		double RotationAngle = min(150.0, state.SLARotationLimit);
		RotationLimit = max(0.0, (RotationAngle / 180.0));

		//
		// \todo For now, only allow 45 degrees if it's not hinged. The panel release code needs to be updated for other angles.
		//
		if (!PanelsHinged)
		{
			RotationLimit = 0.25;
		}

		if (state.PanelProcess)
		{
			panelProc = panelProcPlusX = state.PanelProcess;
			SetAnimation(panelAnim, panelProc);
		}

		if (SaturnVStage)
		{
			sivbsys = new SIVB500Systems(this, th_main[0], ph_main, th_aps_rot, th_aps_ull, th_lox_vent, thg_ver);
		}
		else
		{
			sivbsys = new SIVB200Systems(this, th_main[0], ph_main, th_aps_rot, th_aps_ull, th_lox_vent, thg_ver);
		}
		iu = state.iu_pointer;
		iuinitflag = true;
		iu->DisconnectIU();
	}

	if (state.SettingsType.SIVB_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
		PayloadMass = state.PayloadMass;
	}

	if (state.SettingsType.SIVB_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
		ApsFuel1Kg = state.ApsFuel1Kg;
		ApsFuel2Kg = state.ApsFuel2Kg;
	}

	if (state.SettingsType.SIVB_SETTINGS_ENGINES)
	{
		THRUST_THIRD_VAC = state.THRUST_VAC;
		ISP_THIRD_VAC = state.ISP_VAC;
	}

	State = SIVB_STATE_WAITING;
	//Set these pyros as blown, otherwise the S-IVB wouldn't have been created by the Saturn class anyway
	CSMLVSeparationInitiator.SetBlown(true);
	SLAPanelDeployInitiator.SetBlown(true);
	//Set S-IVB here, including docking ports
	SetS4b();
	//docking port to CSM and S-IB/S-II got created in SetS4b, so now delete it again
	DelDock(hDockSI);
	hDockSI = NULL;
	DelDock(hDockCSM);
	hDockCSM = NULL;

	FirstTimestep = true;
}

int SIVB::GetVehicleNo()
{
	return VehicleNo;
}

void SIVB::SetSIVBThrusterDir(double yaw, double pitch)
{
	sivbsys->SetThrusterDir(yaw, pitch);
}

void SIVB::SetAPSAttitudeEngine(int n, bool on)
{
	sivbsys->SetAPSAttitudeEngine(n, on);
}

bool SIVB::GetSIVBThrustOK()
{
	return sivbsys->GetThrustOK();
}

void SIVB::SIVBSwitchSelector(int channel)
{
	sivbsys->SwitchSelector(channel);
}

void SIVB::SIVBEDSCutoff(bool cut)
{
	sivbsys->EDSEngineCutoff(cut);
}

double SIVB::GetSIVbPropellantMass()

{
	return GetPropellantMass(ph_main);
}

bool SIVB::PayloadIsDetachable()

{
	switch (PayloadType)
	{
	case PAYLOAD_LEM:
	case PAYLOAD_ASTP:
	case PAYLOAD_LM1:
		return true;

	default:
		return false;
	}
}

void SIVB::CreatePayload() {

	if (PayloadCreated) return;

	char *plName = 0;

	//
	// Get the payload config file name.
	//

	switch (PayloadType)
	{
	case PAYLOAD_LEM:
	case PAYLOAD_LM1:
		plName = "ProjectApollo/LEM";
		break;
	case PAYLOAD_ASTP:
		plName = "ProjectApollo/ASTP";
		break;

	default:
		return;
	}

	Payload *payloadvessel;
	VESSELSTATUS2 vslm2;
	VESSELSTATUS2::DOCKINFOSPEC dckinfo;

	//
	// Now Lets create a payload and dock it to the SIVB
	//

	OBJHANDLE hSIVBdock = GetHandle();

	vslm2.version = 2;
	vslm2.flag = 0;
	vslm2.fuel = 0;
	vslm2.thruster = 0;
	vslm2.ndockinfo = 1;
	vslm2.dockinfo = &dckinfo;

	GetStatusEx(&vslm2);

	vslm2.dockinfo[0].idx = 1;
	vslm2.dockinfo[0].ridx = 0;
	vslm2.dockinfo[0].rvessel = hSIVBdock;
	vslm2.ndockinfo = 1;
	vslm2.flag = VS_DOCKINFOLIST;
	vslm2.version = 2;

	OBJHANDLE hPayload = oapiCreateVesselEx(PayloadName, plName, &vslm2);

	//
	// We have already gotten these information from the CSM
	//

	payloadSettings.MissionTime = MissionTime;

	//
	// Initialise the state of the LEM AGC information.
	//

	payloadvessel = static_cast<Payload *> (oapiGetVesselInterface(hPayload));
	payloadvessel->SetupPayload(payloadSettings);
	Payloaddatatransfer = true;

	GetStatusEx(&vslm2);

	vslm2.dockinfo = &dckinfo;
	vslm2.dockinfo[0].idx = 0;
	vslm2.dockinfo[0].ridx = 1;
	vslm2.dockinfo[0].rvessel = hPayload;
	vslm2.ndockinfo = 1;
	vslm2.flag = VS_DOCKINFOLIST;
	vslm2.version = 2;

	DefSetStateEx(&vslm2);

	//
	// Finally, do any special case configuration.
	//

	switch (PayloadType)
	{
	case PAYLOAD_LEM:
	case PAYLOAD_LM1:
	{
		//
		// PAD load.
		//

		LEM *lmvessel = static_cast<LEM *> (payloadvessel);

		if (LMPad && LMPadCount > 0)
		{
			int i;
			for (i = 0; i < LMPadCount; i++) {
				lmvessel->PadLoad(LMPad[i * 2], LMPad[i * 2 + 1]);
			}
		}

		if (AEAPad && AEAPadCount > 0)
		{
			int i;
			for (i = 0; i < AEAPadCount; i++) {
				lmvessel->AEAPadLoad(AEAPad[i * 2], AEAPad[i * 2 + 1]);
			}
		}
	}
	break;

	default:
		break;
	}

	PayloadCreated = true;
}

void SIVB::StartSeparationPyros()
{
	if (!PayloadIsDetachable())
	{
		return;
	}

	if (PayloadCreated) {
		LMSLASeparationInitiators.SetBlown(true);
		UINT i;
		if (GetDockingPortFromHandle(hDock, i))
		{
			DelDock(hDock);
			hDock = NULL;
		}
	}
}

void SIVB::StopSeparationPyros()

{
	//
	// Stop separation. For now this will probably do nothing.
	//
}

void SIVB::StartSLASeparationPyros()
{
	SLAPanelDeployInitiator.SetBlown(true);
}

void SIVB::SeparateCSM()
{
	if (hDockCSM) {
		CSMLVSeparationInitiator.SetBlown(true);
		UINT i;
		if (GetDockingPortFromHandle(hDockCSM, i))
		{
			OBJHANDLE target;
			VESSEL *vTgt = NULL;
			if (target = GetDockStatus(hDockCSM))
			{
				vTgt = oapiGetVesselInterface(target);
			}
			DelDock(hDockCSM);
			hDockCSM = NULL;

			if (vTgt)
			{
				//Spring system
				vTgt->AddForce(_V(0, 0, 1)*PayloadEjectionForce / oapiGetSimStep(), _V(0, 0, 0));
			}
		}
	}
}

double SIVB::GetPayloadMass()
{
	return PayloadMass;
}

void SIVB::SISwitchSelector(int channel)
{
	sivbSIConnector.SISwitchSelector(channel);
}

void SIVB::SetSIThrusterDir(int n, double yaw, double pitch)
{
	sivbSIConnector.SetSIThrusterDir(n, yaw, pitch);
}

bool SIVB::GetSIBLowLevelSensorsDry()
{
	return sivbSIConnector.GetLowLevelSensorsDry();
}

bool SIVB::GetSIPropellantDepletionEngineCutoff()
{
	return sivbSIConnector.GetSIPropellantDepletionEngineCutoff();
}

void SIVB::HideAllMeshes()

{
	SetMeshVisibilityMode(panelMesh1SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4SaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh1SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4SaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh1Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh2Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh3Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(panelMesh4Saturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturnV, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturnVLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturn1b, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturn1bLow, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshSivbSaturn1bcross, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshLTA_2r, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshApollo8LTA, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshASTP_A, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshASTP_B, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshCOASTarget_A, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshCOASTarget_B, MESHVIS_NEVER);
	SetMeshVisibilityMode(meshCOASTarget_C, MESHVIS_NEVER);
}


static int refcount = 0;

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	VESSEL *v;

	if (!refcount++)
	{
		SIVbLoadMeshes();
	}

	v = new SIVB (hvessel, flightmodel);
	return v;
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	--refcount;

	if (!refcount)
	{

		//
		// This code could tidy up allocations when refcount == 0
		//

	}

	if (vessel) 
		delete static_cast<SIVB *> (vessel);
}

SIVbConnector::SIVbConnector()

{
	OurVessel = 0;
}

SIVbConnector::~SIVbConnector()

{
}

SIVbToIUCommandConnector::SIVbToIUCommandConnector()

{
	type = LV_IU_COMMAND;
}

SIVbToIUCommandConnector::~SIVbToIUCommandConnector()

{
}

bool SIVbToIUCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IULVMessageType messageType;

	messageType = (IULVMessageType) m.messageType;

	switch (messageType)
	{

	case IULV_GET_STAGE:
		if (OurVessel)
		{
			if (OurVessel->IsLowerStageDocked())
			{
				m.val1.iValue = LAUNCH_STAGE_ONE;
			}
			else
			{
				m.val1.iValue = STAGE_ORBIT_SIVB;
			}
			return true;
		}
	case IULV_GET_GLOBAL_ORIENTATION:
		if (OurVessel)
		{
			VECTOR3 *arot = static_cast<VECTOR3 *> (m.val1.pValue);
			VECTOR3 ar;

			OurVessel->GetGlobalOrientation(ar);

			*arot = ar;
			return true;
		}
		break;

	case IULV_GET_MASS:
		if (OurVessel)
		{
			m.val1.dValue = OurVessel->GetMass();
			return true;
		}
		break;

	case IULV_GET_GRAVITY_REF:
		if (OurVessel)
		{
			m.val1.hValue = OurVessel->GetGravityRef();
			return true;
		}
		break;

	case IULV_GET_RELATIVE_POS:
		if (OurVessel)
		{
			VECTOR3 pos;
			VECTOR3 *v = (VECTOR3 *) m.val2.pValue;

			OurVessel->GetRelativePos(m.val1.hValue, pos);

			v->data[0] = pos.data[0];
			v->data[1] = pos.data[1];
			v->data[2] = pos.data[2];

			return true;
		}
		break;

	case IULV_GET_RELATIVE_VEL:
		if (OurVessel)
		{
			VECTOR3 vel;
			VECTOR3 *v = (VECTOR3 *) m.val2.pValue;

			OurVessel->GetRelativeVel(m.val1.hValue, vel);

			v->data[0] = vel.data[0];
			v->data[1] = vel.data[1];
			v->data[2] = vel.data[2];

			return true;
		}
		break;

	case IULV_GET_GLOBAL_VEL:
		if (OurVessel)
		{
			OurVessel->GetGlobalVel(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_WEIGHTVECTOR:
		if (OurVessel)
		{
			m.val2.bValue = OurVessel->GetWeightVector(*(VECTOR3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_ROTATIONMATRIX:
		if (OurVessel)
		{
			OurVessel->GetRotationMatrix(*(MATRIX3 *) m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_ANGULARVEL:
		if (OurVessel)
		{
			OurVessel->GetAngularVel(*(VECTOR3 *)m.val1.pValue);
			return true;
		}
		break;

	case IULV_GET_SIVB_THRUST_OK:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIVBThrustOK();
			return true;
		}
		break;

	case IULV_SIVB_EDS_CUTOFF:
		if (OurVessel)
		{
			OurVessel->SIVBEDSCutoff(m.val1.bValue);
			return true;
		}
		break;

	case IULV_SET_APS_ATTITUDE_ENGINE:
		if (OurVessel)
		{
			OurVessel->SetAPSAttitudeEngine(m.val1.iValue, m.val2.bValue);
			return true;
		}
		break;

	case IULV_SET_SIVB_THRUSTER_DIR:
		if (OurVessel)
		{
			OurVessel->SetSIVBThrusterDir(m.val1.dValue, m.val2.dValue);
			return true;
		}
		break;

	case IULV_SIVB_SWITCH_SELECTOR:
		if (OurVessel)
		{
			OurVessel->SIVBSwitchSelector(m.val1.iValue);
			return true;
		}
		break;

	case IULV_CSM_SEPARATION_SENSED:
		if (OurVessel)
		{
			m.val1.bValue = true;
			return true;
		}
		break;

	case IULV_SI_SWITCH_SELECTOR:
		if (OurVessel)
		{
			OurVessel->SISwitchSelector(m.val1.iValue);
			return true;
		}
		break;
	case IULV_SET_SI_THRUSTER_DIR:
		if (OurVessel)
		{
			OurVessel->SetSIThrusterDir(m.val1.iValue, m.val2.dValue, m.val3.dValue);
			return true;
		}
		break;
	case IULV_GET_SIB_LOW_LEVEL_SENSORS_DRY:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIBLowLevelSensorsDry();
			return true;
		}
		break;
	case IULV_GET_SI_PROPELLANT_DEPLETION_ENGINE_CUTOFF:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIPropellantDepletionEngineCutoff();
			return true;
		}
		break;
	case IULV_NOSECAP_JETTISON:
		if (OurVessel)
		{
			OurVessel->SeparateCSM();
			return true;
		}
		break;
	case IULV_DEPLOY_SLA_PANEL:
		if (OurVessel)
		{
			OurVessel->StartSLASeparationPyros();
			return true;
		}
		break;
	case IULV_GET_VEHICLENO:
		if (OurVessel)
		{
			m.val1.iValue = OurVessel->GetVehicleNo();
			return true;
		}
		break;
	case IULV_GET_SI_THRUST_OK:
		if (OurVessel)
		{
			OurVessel->GetSIVBSIConnector()->GetSIThrustOK((bool *)m.val1.pValue, m.val2.iValue);
			return true;
		}
		break;
	}

	return false;
}

PayloadToSLACommandConnector::PayloadToSLACommandConnector()
{
	type = PAYLOAD_SLA_CONNECT;
}

PayloadToSLACommandConnector::~PayloadToSLACommandConnector()
{
}

bool PayloadToSLACommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)
{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	PayloadSIVBMessageType messageType;

	messageType = (PayloadSIVBMessageType)m.messageType;

	switch (messageType)
	{
	case SLA_START_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StartSeparationPyros();
			return true;
		}
		break;

	case SLA_STOP_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StopSeparationPyros();
			return true;
		}
		break;
	}

	return false;
}

SIVBToSIConnector::SIVBToSIConnector()
{
	type = SIVB_SI_COMMAND;
	OurVessel = 0;
}

SIVBToSIConnector::~SIVBToSIConnector()
{

}

void SIVBToSIConnector::SISwitchSelector(int channel)
{
	ConnectorMessage cm;

	cm.destination = SIVB_SI_COMMAND;
	cm.messageType = SIVB_SI_SWITCH_SELECTOR;
	cm.val1.iValue = channel;

	SendMessage(cm);
}

void SIVBToSIConnector::SetSIThrusterDir(int n, double yaw, double pitch)
{
	ConnectorMessage cm;

	cm.destination = SIVB_SI_COMMAND;
	cm.messageType = SIVB_SI_THRUSTER_DIR;
	cm.val1.iValue = n;
	cm.val2.dValue = yaw;
	cm.val3.dValue = pitch;

	SendMessage(cm);
}

bool SIVBToSIConnector::GetLowLevelSensorsDry()
{
	ConnectorMessage cm;

	cm.destination = SIVB_SI_COMMAND;
	cm.messageType = SIVB_SI_SIB_LOW_LEVEL_SENSORS_DRY;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool SIVBToSIConnector::GetSIPropellantDepletionEngineCutoff()
{
	ConnectorMessage cm;

	cm.destination = SIVB_SI_COMMAND;
	cm.messageType = SIVB_SI_PROPELLANT_DEPLETION_ENGINE_CUTOFF;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

void SIVBToSIConnector::GetSIThrustOK(bool *ok, int n)
{
	ConnectorMessage cm;

	cm.destination = SIVB_SI_COMMAND;
	cm.messageType = SIVB_SI_GETSITHRUSTOK;
	cm.val1.pValue = ok;

	if (SendMessage(cm))
	{
		return;
	}

	for (int i = 0;i < n;i++)
	{
		ok[i] = false;
	}
}