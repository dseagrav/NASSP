/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Service Module class

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
#include "nasspsound.h"

#include "soundlib.h"

#include "sm.h"

#include <stdio.h>
#include <string.h>

//
// Meshes are globally loaded. Only HiRes for time being
//

MESHHANDLE hSM;
MESHHANDLE hSMRCS;
MESHHANDLE hSMRCSLow;
MESHHANDLE hSMSPS;
MESHHANDLE hSMPanel1;
MESHHANDLE hSMPanel2;
MESHHANDLE hSMPanel3;
MESHHANDLE hSMPanel4;
MESHHANDLE hSMPanel5;
MESHHANDLE hSMPanel6;
MESHHANDLE hSMhga;
MESHHANDLE hSMCRYO;


#define LOAD_MESH(var, name) var = oapiLoadMeshGlobal(name);

void SMLoadMeshes()

{
	//
	// SM meshes
	//

	LOAD_MESH(hSM, "ProjectApollo/SM-core");
    LOAD_MESH(hSMRCS, "ProjectApollo/SM-RCSHI");
	LOAD_MESH(hSMRCSLow, "ProjectApollo/SM-RCSLO");
	LOAD_MESH(hSMSPS, "ProjectApollo/SM-SPS");
	LOAD_MESH(hSMPanel1, "ProjectApollo/SM-Panel1");
	LOAD_MESH(hSMPanel2, "ProjectApollo/SM-Panel2");
	LOAD_MESH(hSMPanel3, "ProjectApollo/SM-Panel3");
	LOAD_MESH(hSMPanel4, "ProjectApollo/SM-Panel4");
	LOAD_MESH(hSMPanel5, "ProjectApollo/SM-Panel5");
	LOAD_MESH(hSMPanel6, "ProjectApollo/SM-Panel6");
	LOAD_MESH(hSMhga, "ProjectApollo/SM-HGA");
	LOAD_MESH(hSMCRYO, "ProjectApollo/SM-CRYO");

}

SM::SM (OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	InitSM();
	DefineAnimations();

	soundlib.InitSoundLib(oapiGetVesselInterface(hObj), SOUND_DIRECTORY);
	soundlib.LoadSound(BreakS, CRASH_SOUND);
}

SM::~SM()

{
	//
	// Unfortunately this makes Orbiter crash on shutdown.
	//
#if 0
	//
	// Delete any vessel parts which are lying around.
	//
	if (hHGA)
	{
		oapiDeleteVessel(hHGA, GetHandle());
		hHGA = 0;
	}
#endif
}

void SM::InitSM()

{
	State = SM_STATE_SETUP;

	EmptyMass = 6100.0;
	MainFuel = 5000.0;

	RetrosFired = false;
	LowRes = false;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	ph_rcsa = 0;
	ph_rcsb = 0;
	ph_rcsc = 0;
	ph_rcsd = 0;

	showSPS = true;
	showHGA = true;
	showCRYO = true;
	showRCS = true;

	showPanel1 = true;
	showPanel2 = true;
	showPanel3 = true;
	showPanel4 = true;
	showPanel5 = true;
	showPanel6 = true;

	CMTex = oapiRegisterReentryTexture("reentry");

	Temperature = 250.0;

	umbilical_proc = 0;

	hHGA = 0;
	hSPS = 0;
	hPanel1 = 0;
	hPanel2 = 0;
	hPanel3 = 0;
	hPanel4 = 0;
	hPanel5 = 0;
	hPanel6 = 0;

	Alpha = 0;
	Beta = 0;
	Gamma = 0;

	anim_HGAalpha = -1;
	anim_HGAbeta = -1;
	anim_HGAgamma = -1;

	hga_proc[0] = 0.0;
	hga_proc[1] = 0.0;
	hga_proc[2] = 0.0;

	SMBusAPowered = false;
	SMBusBPowered = false;

	FirstTimestep = true;

	int i;

	for (i = 0; i < 4; i++)
	{
		th_rcs_a[i] = 0;
		th_rcs_b[i] = 0;
		th_rcs_c[i] = 0;
		th_rcs_d[i] = 0;
	}

	for (i = 0; i < 24; i++)
	{
		th_att_lin[i] = 0;
		th_att_rot[i] = 0;
	}
}

const double SMVO = 0.0;//-0.14;

void SM::SetSM()

{
	ClearMeshes();

	double mass = EmptyMass + MainFuel;
	
	SetSize (5);

	SetPMI(_V(3.3258, 3.529, 1.914));
	SetCrossSections (_V(40,40,14));
	SetCW (0.1, 0.3, 1.4, 1.4);
	SetRotDrag (_V(0.7,0.7,0.3));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);

    ClearExhaustRefs();
    ClearAttExhaustRefs();

	VECTOR3 mesh_dir=_V(0, 0, 0);

	SMMeshIndex=0;
	AddMesh (hSM, &mesh_dir);

	if (LowRes)
		AddMesh (hSMRCSLow, &mesh_dir);
	else
		AddMesh (hSMRCS, &mesh_dir);

	if (showPanel1)
		AddMesh (hSMPanel1, &mesh_dir);

	if (showPanel2)
		AddMesh (hSMPanel2, &mesh_dir);

	if (showPanel3)
		AddMesh (hSMPanel3, &mesh_dir);

	if (showPanel4)
		AddMesh (hSMPanel4, &mesh_dir);

	if (showPanel5)
		AddMesh (hSMPanel5, &mesh_dir);

	if (showPanel6)
		AddMesh (hSMPanel6, &mesh_dir);

	if (showCRYO)
		AddMesh (hSMCRYO, &mesh_dir);

	if (showSPS) 
	{
		mesh_dir = _V(0, SMVO, -1.654);
		AddMesh(hSMSPS, &mesh_dir);
	}

	if (showHGA)
	{
		UINT HGAidx;
		mesh_dir=_V(-1.308,-1.18,-1.258);
		HGAidx = AddMesh (hSMhga, &mesh_dir);
		DefineAnimationsHGA(HGAidx);
		hga_proc[0] = Alpha / PI2;
		if (hga_proc[0] < 0) hga_proc[0] += 1.0;
		hga_proc[1] = Beta / PI2;
		if (hga_proc[1] < 0) hga_proc[1] += 1.0;
		hga_proc[2] = (Gamma - PI05) / PI2;
		if (hga_proc[2] < 0) hga_proc[2] += 1.0;
		SetAnimation(anim_HGAalpha, hga_proc[0]);
		SetAnimation(anim_HGAbeta, hga_proc[1]);
		SetAnimation(anim_HGAgamma, hga_proc[2]);
	}

	SetEmptyMass (mass);

	AddEngines ();

	if (CMTex)
		SetReentryTexture(CMTex,1e6,5,0.7);
}

void SM::DoFirstTimestep()

{
	//
	// Get the handles for any odds and ends that are out there.
	//

	char VName[256];
	char ApolloName[64];

	GetApolloName(ApolloName);

	strcpy (VName, ApolloName); strcat (VName, "-HGA");
	hHGA = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hHGA);

	strcpy (VName, ApolloName); strcat (VName, "-SPS");
	hSPS = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hSPS);

	strcpy (VName, ApolloName); strcat (VName, "-PANEL1");
	hPanel1 = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hPanel1);

	strcpy (VName, ApolloName); strcat (VName, "-PANEL2");
	hPanel2 = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hPanel2);

	strcpy (VName, ApolloName); strcat (VName, "-PANEL3");
	hPanel3 = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hPanel3);

	strcpy (VName, ApolloName); strcat (VName, "-PANEL4");
	hPanel4 = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hPanel4);

	strcpy (VName, ApolloName); strcat (VName, "-PANEL5");
	hPanel5 = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hPanel5);

	strcpy (VName, ApolloName); strcat (VName, "-PANEL6");
	hPanel6 = oapiGetVesselByName(VName);
	AddReentryTextureToObject(hPanel6);

	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
	soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, FALSE);
	soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);
	soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
	soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);
}

void SM::AddReentryTextureToObject(OBJHANDLE handle)

{
	if (handle)
	{
		VESSEL *vSep = oapiGetVesselInterface(handle);
		if (CMTex && vSep)
			vSep->SetReentryTexture(CMTex,1e6,5,0.7);
	}
}

bool SM::TryToDelete(OBJHANDLE &handle, OBJHANDLE hCamera)

{
	if (handle)
	{
		oapiDeleteVessel(handle, hCamera);
		handle = 0;

		return true;
	}

	return false;
}

bool SM::TidyUpMeshes(OBJHANDLE hCamera)

{
	if (TryToDelete(hHGA, hCamera))
		return true;
	if (TryToDelete(hSPS, hCamera))
		return true;
	if (TryToDelete(hPanel1, hCamera))
		return true;
	if (TryToDelete(hPanel2, hCamera))
		return true;
	if (TryToDelete(hPanel3, hCamera))
		return true;
	if (TryToDelete(hPanel4, hCamera))
		return true;
	if (TryToDelete(hPanel5, hCamera))
		return true;
	if (TryToDelete(hPanel6, hCamera))
		return true;

	return false;
}

void SM::clbkPreStep(double simt, double simdt, double mjd)

{
	MissionTime += simdt;

	if (FirstTimestep)
	{
		DoFirstTimestep();
		FirstTimestep = false;
	}

	char VName[256];
	char ApolloName[64];

	VECTOR3 ofs1;
	VECTOR3 vel1;
	VESSELSTATUS vs1;

	VECTOR3 rofs1, rvel1;

	double da = simdt * UMBILICAL_SPEED;
	if (umbilical_proc < 1.0)
	{
		umbilical_proc = min (1.0, umbilical_proc+da);
	}
    SetAnimation (anim_umbilical, umbilical_proc);

	//
	// See section 2.9.4.13.2 of the Apollo Operations Handbook Seq Sys section for
	// details on RCS operation after SM sep.
	//
	// -X engines start at seperation. +R engines fire 2.0 seconds later, and stop
	// 5.5 seconds after that. -X engines continue to fire until the fuel depletes
	// or the fuel cells stop providing power.
	//

	SMJCA.Timestep(simdt, SMBusAPowered);
	SMJCB.Timestep(simdt, SMBusBPowered);

	if (SMJCA.GetFireMinusXTranslation() || SMJCB.GetFireMinusXTranslation())
	{
		SetThrusterLevel(th_rcs_a[3], 1.0);
		SetThrusterLevel(th_rcs_b[3], 1.0);
		SetThrusterLevel(th_rcs_c[4], 1.0);
		SetThrusterLevel(th_rcs_d[4], 1.0);
	}
	else
	{
		SetThrusterLevel(th_rcs_a[3], 0.0);
		SetThrusterLevel(th_rcs_b[3], 0.0);
		SetThrusterLevel(th_rcs_c[4], 0.0);
		SetThrusterLevel(th_rcs_d[4], 0.0);
	}

	if (SMJCA.GetFirePositiveRoll() || SMJCB.GetFirePositiveRoll())
	{
		SetThrusterLevel(th_rcs_a[1], 1.0);
		SetThrusterLevel(th_rcs_b[1], 1.0);
		SetThrusterLevel(th_rcs_c[1], 1.0);
		SetThrusterLevel(th_rcs_d[1], 1.0);
	}
	else
	{
		SetThrusterLevel(th_rcs_a[1], 0.0);
		SetThrusterLevel(th_rcs_b[1], 0.0);
		SetThrusterLevel(th_rcs_c[1], 0.0);
		SetThrusterLevel(th_rcs_d[1], 0.0);
	}

	switch (State) 
	{
	case SM_UMBILICALDETACH_PAUSE:
		//Someone who knows how, please add a small Particle stream going from detach Point.
		NextMissionEventTime = MissionTime + 1.0;
		State = SM_STATE_WAITING;
		break;

	//
	// Wait until destroyed. Break up the SM as it burns up in
	// the atmosphere.
	//

	case SM_STATE_WAITING:
		if (MissionTime >= NextMissionEventTime)
		{
			NextMissionEventTime = MissionTime + 1.0;

			//
			// Do heating calculations and break up when we get too hot. These
			// only need to be approximate, since we just want to time breakup
			// based on how tough the re-entry is.
			//

			//
			// approximate SM area:
			// total: 116
			// front: 12.5
			//
			// So we can assume it radiates away from 116 and absorbs from 12.5
			// for a simple measure.
			//

			//
			// This should be the heat flux in watts per square meter.
			//

			double heatflux = GetDynPressure() * GetAirspeed();
			double heatinput = heatflux * 12.5;

			double heatrad = 5.67e-8 * 116 * pow(Temperature, 4);
			double Heat = (heatinput - heatrad) * simdt;

			//
			// Adjust temperature.
			//

			Temperature += (Heat / 1250000.0); // Need thermal capacity

			//
			// Set a sane lowest temperature.
			//

			if (Temperature < 200.0)
			{
				Temperature = 200.0;
			}
	
			//
			// Initial breakup code.
			//
			if (showHGA && Temperature > 600.0)
			{
				showHGA = false;
				SetSM();

				//
				// We now need to create an HGA 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-HGA");

				ofs1 = _V(-1.308,-1.18,-1.258);
				vel1 = _V(-0.2, -0.2, -0.05);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;
					
				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				hHGA = oapiCreateVessel(VName, "ProjectApollo/SM-HGA", vs1);
				AddReentryTextureToObject(hHGA);

				BreakS.play();
			}
			else if (showPanel1 && Temperature > 800.0)
			{
				showPanel1 = false;
				showCRYO = true;

				SetSM();

				//
				// Delete HGA.
				//

				TryToDelete(hHGA, GetHandle());

				//
				// We now need to create a panel 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-PANEL1");

				ofs1 = _V(0, 0, 0);
				vel1 = _V(0.0, 1.5, 0.05);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;

				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				hPanel1 = oapiCreateVessel(VName, "ProjectApollo/SM-Panel1", vs1);
				AddReentryTextureToObject(hPanel1);

				BreakS.play();
			}
			else if (showPanel2 && Temperature > 1000.0)
			{
				showPanel2 = false;
				SetSM();

				//
				// We now need to create a panel 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-PANEL2");

				ofs1 = _V(0, 0, 0);
				vel1 = _V(1.0, 0.25, 0.05);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;

				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				hPanel2 = oapiCreateVessel(VName, "ProjectApollo/SM-Panel2", vs1);
				AddReentryTextureToObject(hPanel2);

				BreakS.play();
			}
			else if (showPanel3 && Temperature > 1400.0)
			{
				showPanel3 = false;
				SetSM();

				//
				// We now need to create a panel 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-PANEL3");

				ofs1 = _V(0, 0, 0);
				vel1 = _V(0.25, -1.0, 0.1);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;

				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				hPanel3 = oapiCreateVessel(VName, "ProjectApollo/SM-Panel3", vs1);
				AddReentryTextureToObject(hPanel3);

				BreakS.play();
			}
			else if (showPanel4 && Temperature > 1700.0)
			{
				showPanel4 = false;
				SetSM();

				//
				// We now need to create a panel 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-PANEL4");

				ofs1 = _V(0, 0, 0);
				vel1 = _V(0.0, -1.25, 0.1);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;

				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				hPanel4 = oapiCreateVessel(VName, "ProjectApollo/SM-Panel4", vs1);
				AddReentryTextureToObject(hPanel4);

				BreakS.play();
			}
			else if (showPanel5 && Temperature > 1800.0)
			{
				showPanel5 = false;
				SetSM();

				//
				// We now need to create a panel 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-PANEL5");

				ofs1 = _V(0, 0, 0);
				vel1 = _V(-0.5, 0.75, 0.01);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;

				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				hPanel5 = oapiCreateVessel(VName, "ProjectApollo/SM-Panel5", vs1);
				AddReentryTextureToObject(hPanel5);

				BreakS.play();
			}
			else if (showPanel6 && Temperature > 2000.0)
			{
				showPanel6 = false;
				SetSM();

				//
				// We now need to create a panel 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-PANEL6");

				ofs1 = _V(0, 0, 0);
				vel1 = _V(-0.75, 0.5, -0.025);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;

				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				hPanel6 = oapiCreateVessel(VName, "ProjectApollo/SM-Panel6", vs1);
				AddReentryTextureToObject(hPanel6);

				BreakS.play();
			}
			else if (showSPS && Temperature > 2500.0)
			{
				showSPS = false;
				SetSM();

				//
				// We now need to create an SPS 'vessel' falling away from the SM.
				//

				GetApolloName(ApolloName);

				strcpy (VName, ApolloName); strcat (VName, "-SPS");

				ofs1 = _V(0, SMVO, -1.654);
				vel1 = _V(-0.5, 0.25, -0.25);

				GetStatus (vs1);
				vs1.eng_main = vs1.eng_hovr = 0.0;
				vs1.status = 0;
					
				rvel1 = _V(vs1.rvel.x, vs1.rvel.y, vs1.rvel.z);

				Local2Rel (ofs1, vs1.rpos);
				GlobalRot (vel1, rofs1);

				vs1.rvel.x = rvel1.x+rofs1.x;
				vs1.rvel.y = rvel1.y+rofs1.y;
				vs1.rvel.z = rvel1.z+rofs1.z;

				vs1.vrot.x += 0.005;
				vs1.vrot.y += 0.05;
				vs1.vrot.z += 0.02;

				hSPS = oapiCreateVessel(VName, "ProjectApollo/SM-SPS", vs1);
				AddReentryTextureToObject(hSPS);

				BreakS.play();
			}
		}
		break;

	default:
		break;
	}
}

void SM::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "HEAT", Temperature);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "UPRC", umbilical_proc);
	oapiWriteScenario_float(scn, "ALPHA", Alpha);
	oapiWriteScenario_float(scn, "BETA", Beta);
	oapiWriteScenario_float(scn, "GAMMA", Gamma);
	SMJCA.SaveState(scn, SMJCA_START_STRING);
	SMJCB.SaveState(scn, SMJCB_START_STRING);
}

typedef union {
	struct {
		unsigned int showSPS:1;
		unsigned int showHGA:1;
		unsigned int showCRYO:1;
		unsigned int showRCS:1;
		unsigned int showPanel1:1;
		unsigned int showPanel2:1;
		unsigned int showPanel3:1;
		unsigned int showPanel4:1;
		unsigned int showPanel5:1;
		unsigned int showPanel6:1;
		unsigned int LowRes:1;
		unsigned int A13Exploded:1;
		unsigned int SMBusAPowered : 1;
		unsigned int SMBusBPowered : 1;
	} u;
	unsigned long word;
} MainState;

int SM::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.showSPS = showSPS;
	state.u.showHGA = showHGA;
	state.u.showCRYO = showCRYO;
	state.u.showRCS = showRCS;
	state.u.showPanel1 = showPanel1;
	state.u.showPanel2 = showPanel2;
	state.u.showPanel3 = showPanel3;
	state.u.showPanel4 = showPanel4;
	state.u.showPanel5 = showPanel5;
	state.u.showPanel6 = showPanel6;
	state.u.LowRes = LowRes;
	state.u.A13Exploded = A13Exploded;
	state.u.SMBusAPowered = SMBusAPowered;
	state.u.SMBusBPowered = SMBusBPowered;

	return state.word;
}

void SM::AddEngines()

{
	ClearThrusterDefinitions();

	//
	// Add the RCS. SPS won't fire with SM seperated.
	//

	if (!ph_rcsa)
		ph_rcsa = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsb)
		ph_rcsb = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsc)
		ph_rcsc = CreatePropellantResource(RCS_FUEL_PER_QUAD);
	if (!ph_rcsd)
		ph_rcsd = CreatePropellantResource(RCS_FUEL_PER_QUAD);

	double TRANZ = 1.9;

	int i;
	const double ATTCOOR = 0;
	const double ATTCOOR2 = 2.05;
	const double ATTZ = 2.85;
	const double TRANCOOR = 0;
	const double TRANCOOR2 = 0.1;
	const double ATTWIDTH = .2;
	const double ATTHEIGHT = .5;
	const double TRANWIDTH = .2;
	const double TRANHEIGHT = 1;
	const double RCSOFFSET = 0.25;
	const double RCSOFFSET2 = -0.25;
	const double RCSOFFSETM = -0.05;
	const double RCSOFFSETM2 = 0.02; // Was 0.05

	//
	// Clear any old thrusters.
	//

	for (i = 0; i < 24; i++)
	{
		th_att_lin[i] = 0;
		th_att_rot[i] = 0;
	}

	//
	// Adjust ISP and thrust based on realism level.
	// -> Disabled for now, wrong thrust and ISP causes weired behavior of the DAP
	//
	double RCS_ISP = SM_RCS_ISP;		// (SM_RCS_ISP * (15.0 - Realism)) / 5.0;
	double RCS_Thrust = SM_RCS_THRUST;	// (SM_RCS_THRUST * (15.0 - Realism)) / 5.0;

	const double CENTEROFFS = 0.25;

	th_att_lin[0] = th_att_rot[0] = CreateThruster(_V(-CENTEROFFS, ATTCOOR2, TRANZ + RCSOFFSET2), _V(0, -0.1, 1), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[1] = th_att_rot[3] = CreateThruster(_V(CENTEROFFS, -ATTCOOR2, TRANZ + RCSOFFSET2), _V(0, 0.1, 1), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[2] = th_att_rot[4] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS, TRANZ + RCSOFFSET2), _V(0.1, 0, 1), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[3] = th_att_rot[7] = CreateThruster(_V(ATTCOOR2, CENTEROFFS, TRANZ + RCSOFFSET2), _V(-0.1, 0, 1), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[4] = th_att_rot[2] = CreateThruster(_V(-CENTEROFFS, ATTCOOR2, TRANZ + RCSOFFSET), _V(0, -0.1, -1), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[5] = th_att_rot[1] = CreateThruster(_V(CENTEROFFS, -ATTCOOR2, TRANZ + RCSOFFSET), _V(0, 0.1, -1), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[6] = th_att_rot[6] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS, TRANZ + RCSOFFSET), _V(0.1, 0, -1), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[7] = th_att_rot[5] = CreateThruster(_V(ATTCOOR2, CENTEROFFS, TRANZ + RCSOFFSET), _V(-0.1, 0, -1), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[8] = th_att_rot[16] = th_att_rot[17] = CreateThruster(_V(-CENTEROFFS - 0.2, ATTCOOR2, TRANZ + RCSOFFSETM), _V(1, -0.1, 0), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[9] = th_att_rot[8] = th_att_rot[9] = CreateThruster(_V(CENTEROFFS - 0.2, -ATTCOOR2, TRANZ + RCSOFFSETM2), _V(1, 0.1, 0), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[12] = th_att_rot[10] = th_att_rot[11] = CreateThruster(_V(-CENTEROFFS + 0.2, ATTCOOR2, TRANZ + RCSOFFSETM2), _V(-1, -0.1, 0), RCS_Thrust, ph_rcsa, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[13] = th_att_rot[18] = th_att_rot[19] = CreateThruster(_V(CENTEROFFS + 0.2, -ATTCOOR2, TRANZ + RCSOFFSETM), _V(-1, 0.1, 0), RCS_Thrust, ph_rcsc, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[16] = th_att_rot[14] = th_att_rot[15] = CreateThruster(_V(ATTCOOR2, CENTEROFFS - 0.2, TRANZ + RCSOFFSETM2), _V(-0.1, 1, 0), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[17] = th_att_rot[22] = th_att_rot[23] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS - 0.2, TRANZ + RCSOFFSETM), _V(-0.1, 1, 0), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);

	th_att_lin[20] = th_att_rot[20] = th_att_rot[21] = CreateThruster(_V(ATTCOOR2, CENTEROFFS + 0.2, TRANZ + RCSOFFSETM), _V(-0.1, -1, 0), RCS_Thrust, ph_rcsb, RCS_ISP, SM_RCS_ISP_SL);
	th_att_lin[21] = th_att_rot[12] = th_att_rot[13] = CreateThruster(_V(-ATTCOOR2, -CENTEROFFS + 0.2, TRANZ + RCSOFFSETM2), _V(0.1, -1, 0), RCS_Thrust, ph_rcsd, RCS_ISP, SM_RCS_ISP_SL);

	//
	// We don't create thruster groups here as the user shouldn't be able to control the SM after
	// it seperates.
	//

	SURFHANDLE SMExhaustTex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust_atrcs");

	for (i = 0; i < 24; i++)
	{
		if (th_att_lin[i])
			AddExhaust(th_att_lin[i], 3.0, 0.15, SMExhaustTex);
	}

	//
	// Map thrusters to RCS quads. Note that we don't use entry zero, we're matching the array to
	// Apollo numbering for simplicity... we also have to include the fake thrusters here so we
	// can enable and disable them.
	//

	th_rcs_a[1] = th_att_rot[16];
	th_rcs_a[2] = th_att_rot[10];
	th_rcs_a[3] = th_att_rot[2];
	th_rcs_a[4] = th_att_rot[0];

	th_rcs_b[1] = th_att_rot[20];
	th_rcs_b[2] = th_att_rot[14];
	th_rcs_b[3] = th_att_rot[5];
	th_rcs_b[4] = th_att_rot[7];

	th_rcs_c[1] = th_att_rot[18];
	th_rcs_c[2] = th_att_rot[8];
	th_rcs_c[3] = th_att_rot[3];
	th_rcs_c[4] = th_att_rot[1];

	th_rcs_d[1] = th_att_rot[22];
	th_rcs_d[2] = th_att_rot[12];
	th_rcs_d[3] = th_att_rot[4];
	th_rcs_d[4] = th_att_rot[6];
}

void SM::DefineAnimations()
{

	static UINT umbilical_group = {2}; // participating groups
	static MGROUP_ROTATE umbilical
	(
		0,				// mesh index
		&umbilical_group, 1,		// group list and # groups
		_V(0,-1.9540,3.168), // rotation reference point
		_V(1,0,0),		// rotation axis
		(float)(50.0 * PI / 180.0) // angular rotation range
	);
	anim_umbilical = CreateAnimation (0.0);
	AddAnimationComponent (anim_umbilical, 0, 1, &umbilical);
}

void SM::DefineAnimationsHGA(UINT idx) {

	// HGA animation definition
	ANIMATIONCOMPONENT_HANDLE	ach_HGAalpha, ach_HGAbeta, ach_HGAgamma;
	const VECTOR3	HGA_PIVOT1 = { -0.460263, -0.596586, -0.062961 }; // Pivot Point
	const VECTOR3	HGA_PIVOT2 = { -0.530745, -0.687882, -0.062966 }; // Pivot Point
	const VECTOR3	HGA_PIVOT3 = { -0.589306, -0.764893, -0.06296 }; // Pivot Point
	const VECTOR3	HGA_AXIS_YAW = { sin(RAD * 37.75),cos(RAD * 37.75), 0.00 }; //Pivot Axis
	const VECTOR3	HGA_AXIS_PITCH = { -sin(RAD * 52.25),cos(RAD * 52.25), 0.00 }; //Pivot Axis

	static UINT meshgroup_Pivot1 = { 2 };
	static UINT meshgroup_Pivot2 = { 3 };
	static UINT meshgroup_Main[2] = { 1, 4 };

	static MGROUP_ROTATE mgt_HGA_Alpha(idx, &meshgroup_Pivot1, 1, HGA_PIVOT1, HGA_AXIS_YAW, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_HGA_Beta(idx, &meshgroup_Pivot2, 1, HGA_PIVOT2, _V(0, 0, 1), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_HGA_Gamma(idx, meshgroup_Main, 2, HGA_PIVOT3, HGA_AXIS_PITCH, (float)(RAD * 360));
	anim_HGAalpha = CreateAnimation(0.0);
	anim_HGAbeta = CreateAnimation(0.0);
	anim_HGAgamma = CreateAnimation(0.0);
	ach_HGAalpha = AddAnimationComponent(anim_HGAalpha, 0.0f, 1.0f, &mgt_HGA_Alpha);
	ach_HGAbeta = AddAnimationComponent(anim_HGAbeta, 0.0f, 1.0f, &mgt_HGA_Beta, ach_HGAalpha);
	ach_HGAgamma = AddAnimationComponent(anim_HGAgamma, 0.0f, 1.0f, &mgt_HGA_Gamma, ach_HGAbeta);
}

void SM::SetMainState(int s)

{
	MainState state;

	state.word = s;

	showSPS = (state.u.showSPS != 0);
	showHGA = (state.u.showHGA != 0);
	showCRYO = (state.u.showCRYO != 0);
	showRCS = (state.u.showRCS != 0);
	showPanel1 = (state.u.showPanel1 != 0);
	showPanel2 = (state.u.showPanel2 != 0);
	showPanel3 = (state.u.showPanel3 != 0);
	showPanel4 = (state.u.showPanel4 != 0);
	showPanel5 = (state.u.showPanel5 != 0);
	showPanel6 = (state.u.showPanel6 != 0);
	LowRes = (state.u.LowRes != 0);
	A13Exploded = (state.u.A13Exploded != 0);
	SMBusAPowered = (state.u.SMBusAPowered != 0);
	SMBusBPowered = (state.u.SMBusBPowered != 0);
}

void SM::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;

	while (oapiReadScenario_nextline (scn, line))
	{
		if (!strnicmp (line, "MAINSTATE", 9))
		{
            int MainState = 0;;
			sscanf (line+9, "%d", &MainState);
			SetMainState(MainState);
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
		else if (!strnicmp (line, "FMASS", 5))
		{
			sscanf (line+5, "%g", &flt);
			MainFuel = flt;
		}
		else if (!strnicmp (line, "HEAT", 4))
		{
			sscanf (line + 4, "%g", &flt);
			Temperature = flt;
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
		else if (!strnicmp (line, "STATE", 5))
		{
			int i;
			sscanf (line + 5, "%d", &i);
			State = (SMState) i;
		}
		else if (!strnicmp (line, "UPRC", 4))
		{
			sscanf (line + 4, "%g", &flt);
			umbilical_proc = flt;
		}
		else if (!strnicmp(line, "ALPHA", 5))
		{
			sscanf(line + 5, "%g", &flt);
			Alpha = flt;
		}
		else if (!strnicmp(line, "BETA", 4))
		{
			sscanf(line + 4, "%g", &flt);
			Beta = flt;
		}
		else if (!strnicmp(line, "GAMMA", 5))
		{
			sscanf(line + 5, "%g", &flt);
			Gamma = flt;
		}
		else if (!strnicmp(line, SMJCA_START_STRING, sizeof(SMJCA_START_STRING))) {
			SMJCA.LoadState(scn);
		}
		else if (!strnicmp(line, SMJCB_START_STRING, sizeof(SMJCB_START_STRING))) {
			SMJCB.LoadState(scn);
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}

	SetSM();
}

void SM::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	InitSM();
}

void SM::clbkDockEvent(int dock, OBJHANDLE connected)

{
}

void SM::SetState(SMSettings &state)

{
	if (state.SettingsType.SM_SETTINGS_GENERAL)
	{
		MissionTime = state.MissionTime;
		VehicleNo = state.VehicleNo;
		showHGA = state.showHGA;
		A13Exploded = state.A13Exploded;

		//
		// If the SM exploded, panel 4 was blown off earlier.
		//

		if (A13Exploded)
		{
			showPanel4 = false;
		}

		//
		// If HGA is present, get position.
		//

		if (showHGA)
		{
			Alpha = state.HGAalpha;
			Beta = state.HGAbeta;
			Gamma = state.HGAgamma;
		}

		SMBusAPowered = state.SMBusAPowered;
		SMBusBPowered = state.SMBusBPowered;
		SMJCA.SetState(state.SMJCAState);
		SMJCB.SetState(state.SMJCBState);
	}

	if (state.SettingsType.SM_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
	}

	if (state.SettingsType.SM_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType.SM_SETTINGS_ENGINES)
	{
		//
		// Nothing for now. Later we can enable or disable RCS as
		// appropriate based on seperation state.
		//
	}

	SetSM();

	//
	// Now the RCS propellant resource has been created, set the
	// fuel levels.
	///

	if (state.SettingsType.SM_SETTINGS_FUEL)
	{
		SetPropellantMass(ph_rcsa, state.RCSAFuelKg);
		SetPropellantMass(ph_rcsb, state.RCSBFuelKg);
		SetPropellantMass(ph_rcsc, state.RCSCFuelKg);
		SetPropellantMass(ph_rcsd, state.RCSDFuelKg);
	}

	State = SM_UMBILICALDETACH_PAUSE;
}


void SM::GetApolloName(char *s)

{
	sprintf(s, "AS-%d", VehicleNo);
}

static int refcount = 0;

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	VESSEL *v;

	if (!refcount++)
	{
		SMLoadMeshes();
	}

	v = new SM (hvessel, flightmodel);
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
		delete (SM *)vessel;
}
