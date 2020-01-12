/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: S1C class

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
#include "s1c.h"

#include <stdio.h>
#include <string.h>


S1C::S1C (OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	InitS1c();

	//
	// We need to turn off most of the Orbitersound options.
	//

#if 0
	/// \todo The sound initialization seems to take a lot of time (up to 5s on my system) 
    /// causing problems during creation of the S1C. Since the sound inside the S1C isn't very important
	/// it's disabled for the moment. 
	
	soundlib.InitSoundLib(oapiGetVesselInterface(hObj), SOUND_DIRECTORY);
	soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
	soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);
	soundlib.SoundOptionOnOff(PLAYWHENATTITUDEMODECHANGE, FALSE);
	soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
#endif
}

S1C::~S1C()

{
	// Nothing for now.
}

void S1C::InitS1c()

{
	int i;

	State = SIC_STATE_HIDDEN;

	ph_retro = 0;
	ph_main = 0;

	thg_retro = 0;
	thg_main = 0;

	EmptyMass = 50000.0;
	MainFuel = 5000.0;
	EngineNum = 5;

	RetrosFired = false;
	LowRes = false;
	S4Interstage = false;
	Stretched = false;

	MissionTime = MINUS_INFINITY;
	NextMissionEventTime = MINUS_INFINITY;
	LastMissionEventTime = MINUS_INFINITY;

	THRUST_FIRST_VAC = 8062309;
	ISP_FIRST_SL    = 265*G;
	ISP_FIRST_VAC   = 304*G;

	CurrentThrust = 0.0;

	hsat5stg1 = 0;
	hsat5stg1low = 0;

	for (i = 0; i < 4; i++)
		th_retro[i] = 0;

	th_main[0] = 0;
}

void S1C::SetS1c()

{
	ClearMeshes();

	ClearThrusterDefinitions();
	
	SetSize (40.0);
	SetPMI (_V(119.65, 119.45, 15.45));
	SetCOG_elev (2.0);
	SetCrossSections (_V(508.35, 530.75, 115.19));
	SetCW (0.5, 1.1, 2, 2.4);
	SetRotDrag (_V(2,2,2));
	SetPitchMomentScale (0);
	SetYawMomentScale (0);
	SetLiftCoeffFunc (0);
    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();

	SetEmptyMass (EmptyMass);

	VECTOR3 v = {4.0, 0, 15.0};
	VECTOR3 v2 = {-0.15, 0, 1.0};

	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetCameraDefaultDirection(v2);
	oapiCameraSetCockpitDir(0,0);
	SetCameraOffset(v);
}

void S1C::clbkPreStep(double simt, double simdt, double mjd)

{	
	if (State == SIC_STATE_HIDDEN)
	{
		return;
	}

	MissionTime += simdt;

	//
	// Currently this just starts the retros. At some point it should
	// run down the engines if they haven't completely shut down, and
	// then do any other simulation like ejecting camera pods.
	//

	switch (State)
	{
	case S1C_STATE_SHUTTING_DOWN:
		if (!RetrosFired && thg_retro)
		{
			SetThrusterGroupLevel(thg_retro, 1.0);
			RetrosFired = true;
		}

		if (CurrentThrust > 0.0)
		{
			CurrentThrust -= simdt;
			for (int i = 0; i < 4; i++)
			{
				SetThrusterLevel(th_main[i], CurrentThrust);
			}
		}
		else
		{
			SetThrusterGroupLevel(THGROUP_MAIN, 0.0);
			State = S1C_STATE_WAITING;
		}
		break;

	case S1C_STATE_WAITING:
		//
		// Nothing for now.
		//
		break;

	default:
		break;
	}
}

void S1C::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "MISSNTIME", MissionTime);
	oapiWriteScenario_float (scn, "NMISSNTIME", NextMissionEventTime);
	oapiWriteScenario_float (scn, "LMISSNTIME", LastMissionEventTime);
	oapiWriteScenario_float (scn, "T1V", THRUST_FIRST_VAC);
	oapiWriteScenario_float (scn, "I1S", ISP_FIRST_SL);
	oapiWriteScenario_float (scn, "I1V", ISP_FIRST_VAC);
	oapiWriteScenario_float (scn, "CTR", CurrentThrust);
}

typedef union
{
	struct
	{
		unsigned int RetrosFired:1;
		unsigned int LowRes:1;
		unsigned int S4Interstage:1;
		unsigned int Stretched:1;
	} u;
	unsigned long word;
} MainState;

int S1C::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.RetrosFired = RetrosFired;
	state.u.LowRes = LowRes;
	state.u.S4Interstage = S4Interstage;
	state.u.Stretched = Stretched;

	return state.word;
}

void S1C::AddEngines()

{
	int i;
	SURFHANDLE tex = oapiRegisterExhaustTexture ("ProjectApollo/Exhaust2");

	//
	// Just in case someone removes all the retros, do nothing.
	//

	if (RetroNum)
	{
		//
		// If we have the SIVb interstage attached, assume the retros are at the front, not
		// at the back.
		//
		if (S4Interstage)
		{
			double posZ = 23.5;

			VECTOR3 m_exhaust_pos2= {-2.83,-2.83,posZ};
			VECTOR3 m_exhaust_pos3= {-2.83,2.83,posZ};
			VECTOR3 m_exhaust_ref2 = {0.1,0.1,-1};
			VECTOR3 m_exhaust_ref3 = {0.1,-0.1,-1};
			VECTOR3 m_exhaust_pos4= {2.83,-2.83,posZ};
			VECTOR3 m_exhaust_pos5= {2.83,2.83,posZ};
			VECTOR3 m_exhaust_ref4 = {-0.1,0.1,-1};
			VECTOR3 m_exhaust_ref5 = {-0.1,-0.1,-1};

			if (!ph_retro)
				ph_retro = CreatePropellantResource(66.0 * RetroNum);

			double thrust = 175500;

			if (!th_retro[0]) {
				th_retro[0] = CreateThruster (m_exhaust_pos2, m_exhaust_ref2, thrust, ph_retro, 4000);
				th_retro[1] = CreateThruster (m_exhaust_pos3, m_exhaust_ref3, thrust, ph_retro, 4000);
				th_retro[2] = CreateThruster (m_exhaust_pos4, m_exhaust_ref4, thrust, ph_retro, 4000);
				th_retro[3] = CreateThruster (m_exhaust_pos5, m_exhaust_ref5, thrust, ph_retro, 4000);
			}

			thg_retro = CreateThrusterGroup(th_retro, 4, THGROUP_RETRO);

			for (i = 0; i < 4; i++)
				AddExhaust (th_retro[i], 8.0, 0.2);
		}
		else
		{
			VECTOR3 m_exhaust_pos2= {-4,-4, -14};
			VECTOR3 m_exhaust_pos3= {-4, 4, -14};
			VECTOR3 m_exhaust_pos4= { 4,-4, -14};
			VECTOR3 m_exhaust_pos5= { 4, 4, -14};

			if (!ph_retro) {
				ph_retro = CreatePropellantResource(51.6 * RetroNum);
			}

			double thrust = 382000.;
			if (!th_retro[0]) {
				th_retro[0] = CreateThruster (m_exhaust_pos2, _V(0.1, 0.1, -0.9), thrust, ph_retro, 4000);
				th_retro[1] = CreateThruster (m_exhaust_pos3, _V(0.1, -0.1, -0.9), thrust, ph_retro, 4000);
				th_retro[2] = CreateThruster (m_exhaust_pos4, _V(-0.1, 0.1, -0.9), thrust, ph_retro, 4000);
				th_retro[3] = CreateThruster (m_exhaust_pos5, _V(-0.1, -0.1, -0.9), thrust, ph_retro, 4000);
			}

			thg_retro = CreateThrusterGroup(th_retro, 4, THGROUP_RETRO);

			for (i = 0; i < 4; i++)
				AddExhaust (th_retro[i], 15.0, 0.6, tex);
		}
	}

	if (EngineNum)
	{
		if (!ph_main && MainFuel > 0.0)
			ph_main = CreatePropellantResource(MainFuel);

		double Offset1st = -23.1;
		VECTOR3 m_exhaust_ref = {0,0,-1};
		VECTOR3 MAIN4a_Vector= {3,3,Offset1st+0.5};
		VECTOR3 MAIN2a_Vector={-3,-3,Offset1st+0.5};
		VECTOR3 MAIN1a_Vector= {-3,3,Offset1st+0.5};
		VECTOR3 MAIN3a_Vector={3,-3,Offset1st+0.5};
		VECTOR3 MAIN5a_Vector={0,0,Offset1st+0.5};

		// orbiter main thrusters

		th_main[0] = CreateThruster (MAIN4a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
		th_main[1] = CreateThruster (MAIN2a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
		th_main[2] = CreateThruster (MAIN1a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
		th_main[3] = CreateThruster (MAIN3a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
		th_main[4] = CreateThruster (MAIN5a_Vector, _V( 0,0,1), THRUST_FIRST_VAC , 0, ISP_FIRST_VAC, ISP_FIRST_SL);

		thg_main = CreateThrusterGroup (th_main, EngineNum, THGROUP_MAIN);
		for (i = 0; i < EngineNum; i++) 
			AddExhaust (th_main[i], 120.0, 3.5, tex);
	}
}

void S1C::SetMainState(int s)

{
	MainState state;

	state.word = s;

	RetrosFired = (state.u.RetrosFired != 0);
	LowRes = (state.u.LowRes != 0);
	S4Interstage = (state.u.S4Interstage != 0);
	Stretched = (state.u.Stretched != 0);
}

void S1C::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

{
	char *line;
	float flt;
	int i;

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
		else if (!strnicmp(line, "T1V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			THRUST_FIRST_VAC = flt;
		}
		else if (!strnicmp(line, "I1S", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_FIRST_SL = flt;
		}
		else if (!strnicmp(line, "I1V", 3))
		{
            sscanf (line + 3, "%f", &flt);
			ISP_FIRST_VAC = flt;
		}
		else if (!strnicmp(line, "CTR", 3))
		{
            sscanf (line + 3, "%f", &flt);
			CurrentThrust = flt;
		}
		else if (!strnicmp (line, "STATE", 5))
		{
			sscanf (line+5, "%d", &i);
			State = (S1cState) i;
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}

	LoadMeshes(LowRes); 
	if (State > SIC_STATE_HIDDEN)
		ShowS1c();	
}

void S1C::clbkSetClassCaps (FILEHANDLE cfg)

{
	VESSEL2::clbkSetClassCaps (cfg);
	SetS1c();
}

void S1C::LoadMeshes(bool lowres)

{
	LowRes = lowres;
	if (LowRes)
	{
		hsat5stg1low = oapiLoadMeshGlobal("ProjectApollo/LowRes/sat5stg1");
	}
	else
	{
		hsat5stg1 = oapiLoadMeshGlobal("ProjectApollo/sat5stg1");
	}

	hS4Interstage = oapiLoadMeshGlobal("ProjectApollo/sat5stg2intstg");
}

void S1C::clbkDockEvent(int dock, OBJHANDLE connected)

{
}

bool S1C::clbkLoadGenericCockpit ()

{
	return false;
}

bool S1C::clbkLoadVC (int id)

{
	if (!id)
		return true;

	return false;
}

void S1C::SetState(S1CSettings &state)

{
	if (state.SettingsType.S1C_SETTINGS_GENERAL)
	{
		MissionTime = state.MissionTime;
		VehicleNo = state.VehicleNo;
		RetroNum = state.RetroNum;
		LowRes = state.LowRes;
		Stretched = state.Stretched;
		S4Interstage = state.S4Interstage;
	}

	if (state.SettingsType.S1C_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
	}

	if (state.SettingsType.S1C_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType.S1C_SETTINGS_ENGINES)
	{
		THRUST_FIRST_VAC = state.THRUST_FIRST_VAC;
		ISP_FIRST_SL = state.ISP_FIRST_SL;
		ISP_FIRST_VAC = state.ISP_FIRST_VAC;
		CurrentThrust = state.CurrentThrust;
		EngineNum = state.EngineNum;

		if (EngineNum < 0)
			EngineNum = 0;

		if (EngineNum > 5)
			EngineNum = 5;
	}

	ShowS1c();
	State = S1C_STATE_SHUTTING_DOWN;
}

void S1C::ShowS1c()

{
	SetEmptyMass(EmptyMass);

	UINT meshidx;
	VECTOR3 mesh_dir = _V(0,0,0);
	if (LowRes)
	{
		if (hsat5stg1low)
		{
			meshidx = AddMesh(hsat5stg1low, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);
		}
	}
	else
	{
		if (hsat5stg1)
		{
			meshidx = AddMesh(hsat5stg1, &mesh_dir);
			SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);
		}
	}

	if (S4Interstage && hS4Interstage)
	{
		mesh_dir = _V(0, 0, 12.35);
		meshidx = AddMesh(hS4Interstage, &mesh_dir);
		SetMeshVisibilityMode (meshidx, MESHVIS_ALWAYS);
	}

	AddEngines();
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)

{
	return new S1C(hvessel, flightmodel);
}


DLLCLBK void ovcExit (VESSEL *vessel)

{
	if (vessel) 
		delete (S1C *)vessel;
}
