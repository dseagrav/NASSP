/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: S1B class

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
#include "connector.h"

#include "nasspdefs.h"
#include "sivb.h"
#include "s1b.h"

#include <stdio.h>
#include <string.h>


PARTICLESTREAMSPEC solid_exhaust = {
	0, 0.5, 250, 35.0, 0.1, 0.15, 0.5, 1.0, 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC s1b_exhaust = {
	0,		// flag
	3.2,	// size
	7000,	// rate
	180.0,	// velocity
	0.15,	// velocity distribution
	0.33,	// lifetime
	4.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 1.0,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

PARTICLESTREAMSPEC srb_exhaust = {
	0,		// flag
	2.75,	// size
	2000,	// rate
	60.0,	// velocity
	0.1,	// velocity distribution
	0.4,	// lifetime
	2.0,	// growthrate
	0.0,	// atmslowdown 
	PARTICLESTREAMSPEC::EMISSIVE,
	PARTICLESTREAMSPEC::LVL_PSQRT, 0, 0.5,
	PARTICLESTREAMSPEC::ATM_FLAT, 1.0, 1.0
};

SIBConnector::SIBConnector()
{
	OurVessel = NULL;
}

SIBConnector::~SIBConnector()
{

}

SIBtoSIVBConnector::SIBtoSIVBConnector()
{
	type = SIVB_SI_COMMAND;
}

SIBtoSIVBConnector::~SIBtoSIVBConnector()
{

}

bool SIBtoSIVBConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)
{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	SIVBSIMessageType messageType;

	messageType = (SIVBSIMessageType)m.messageType;

	switch (messageType)
	{
	case SIVB_SI_SWITCH_SELECTOR:
		if (OurVessel)
		{
			OurVessel->SwitchSelector(m.val1.iValue);
		}
		return true;
	case SIVB_SI_THRUSTER_DIR:
		if (OurVessel)
		{
			OurVessel->SetH1ThrusterDir(m.val1.iValue, m.val2.dValue, m.val3.dValue);
		}
		return true;
	case SIVB_SI_SIB_LOW_LEVEL_SENSORS_DRY:
			if (OurVessel)
			{
				m.val1.bValue = OurVessel->GetLowLevelSensorsDry();
			}
		return true;
	case SIVB_SI_PROPELLANT_DEPLETION_ENGINE_CUTOFF:
		if (OurVessel)
		{
			m.val1.bValue = OurVessel->GetSIPropellantDepletionEngineCutoff();
		}
		return true;
	case SIVB_SI_GETSITHRUSTOK:
		if (OurVessel)
		{
			OurVessel->GetSIThrustOK((bool *)m.val1.pValue);
		}
		return true;
	}

	return false;
}

S1B::S1B (OBJHANDLE hObj, int fmodel) : ProjectApolloConnectorVessel(hObj, fmodel),
SIB_SIVB_Sep("SIB-SIVB-Sep-Pyros", Panelsdk),
sibsys(this, th_main, ph_main, SIB_SIVB_Sep, LaunchS, SShutS)
{
	int i;

	State = SIB_STATE_HIDDEN;

	hsat1stg1 = 0;
	hSat1intstg = 0;

	ph_retro = 0;
	ph_main = 0;

	thg_retro = 0;
	thg_main = 0;

	EmptyMass = 37500.0;
	MainFuel = 5000.0;

	CurrentThrust = 0.0;

	RetrosFired = false;
	LowRes = false;

	THRUST_FIRST_VAC = 1008000;
	ISP_FIRST_SL = 262 * G;
	ISP_FIRST_VAC = 294 * G;

	for (i = 0; i < 4; i++)
		th_retro[i] = 0;

	for (i = 0; i < 8; i++)
		th_main[i] = 0;

	VehicleNo = 0;
	RetroNum = 4;

	sibSIVBConnector.SetSIB(this);

	hDockSIVB = CreateDock(_V(0.0, 0, 16.70513 + 5.7023), _V(0, 0, 1), _V(0, 1, 0));
	RegisterConnector(0, &sibSIVBConnector);

	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo\\SIBSystems");
}

S1B::~S1B()

{
	// Nothing for now.
}

void S1B::clbkPreStep(double simt, double simdt, double mjd)
{
	if (State == SIB_STATE_HIDDEN)
	{
		return;
	}

	sibsys.Timestep(simt, simdt);
	Panelsdk.Timestep(simt);

	if (sibsys.FireRetroRockets())
	{
		SetThrusterGroupLevel(thg_retro, 1);
	}
	if (SIB_SIVB_Sep.Blown() && hDockSIVB)
	{
		SeparateSIVB();
	}
}

void S1B::clbkSaveState (FILEHANDLE scn)

{
	VESSEL2::clbkSaveState (scn);

	oapiWriteScenario_int (scn, "MAINSTATE", GetMainState());
	oapiWriteScenario_int (scn, "VECHNO", VehicleNo);
	oapiWriteScenario_int (scn, "STATE", State);
	oapiWriteScenario_float (scn, "EMASS", EmptyMass);
	oapiWriteScenario_float (scn, "FMASS", MainFuel);
	oapiWriteScenario_float (scn, "T1V", THRUST_FIRST_VAC);
	oapiWriteScenario_float (scn, "I1S", ISP_FIRST_SL);
	oapiWriteScenario_float (scn, "I1V", ISP_FIRST_VAC);
	oapiWriteScenario_float (scn, "CTR", CurrentThrust);

	sibsys.SaveState(scn);

	Panelsdk.Save(scn);
}

typedef union
{
	struct
	{
		unsigned int RetrosFired:1;
		unsigned int LowRes:1;
	} u;
	unsigned long word;
} MainState;

int S1B::GetMainState()

{
	MainState state;

	state.word = 0;
	state.u.RetrosFired = RetrosFired;
	state.u.LowRes = LowRes;

	return state.word;
}

void S1B::AddEngines()

{
	//
	// Just in case someone removes all the retros, do nothing.
	//

	if (!RetroNum)
		return;

	VECTOR3 m_exhaust_pos2= {-2.5,-2.5, 20.2};
	VECTOR3 m_exhaust_pos3= {-2.5,2.5, 20.2};
	VECTOR3 m_exhaust_pos4= {2.5,-2.5, 20.2};
	VECTOR3 m_exhaust_pos5= {2.5,2.5, 20.2};

	//4x 267 lbm of propellant
	if (!ph_retro)
		ph_retro = CreatePropellantResource(484.4);

	if (!ph_main && MainFuel > 0.0)
		ph_main = CreatePropellantResource(MainFuel);

	//36720 lbf each
	double thrust = 163338.7;

	//2050 m/s ISP gives 1.52 seconds burn time
	if (!th_retro[0]) {
		th_retro[0] = CreateThruster (m_exhaust_pos2, _V(0.15, 0.15, -0.9), thrust, ph_retro, 2050.0);
		th_retro[1] = CreateThruster (m_exhaust_pos3, _V(0.15, -0.15, -0.9), thrust, ph_retro, 2050.0);
		th_retro[2] = CreateThruster (m_exhaust_pos4, _V(-0.15, 0.15, -0.9), thrust, ph_retro, 2050.0);
		th_retro[3] = CreateThruster (m_exhaust_pos5, _V(-0.15, -0.15, -0.9), thrust, ph_retro, 2050.0);
	}

	thg_retro = CreateThrusterGroup(th_retro, 4, THGROUP_RETRO);

	int i;
	SURFHANDLE tex = oapiRegisterExhaustTexture("ProjectApollo/Exhaust2");
	for (i = 0; i < 4; i++) {
		AddExhaust(th_retro[i], 8.0, 0.2, tex);
		AddExhaustStream(th_retro[i], &solid_exhaust);
	}

	double Offset1st = -65.5;

	m_exhaust_pos5= _V(0,1.414,Offset1st+55);
    VECTOR3 m_exhaust_pos6= {1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos7= {0,-1.414,Offset1st+55};
	VECTOR3 m_exhaust_pos8= {-1.414,0,Offset1st+55};
	VECTOR3 m_exhaust_pos1= _V(2.12,2.12,Offset1st+55);
    m_exhaust_pos2= _V(2.12,-2.12,Offset1st+55);
	m_exhaust_pos3= _V(-2.12,-2.12,Offset1st+55);
	m_exhaust_pos4= _V(-2.12,2.12,Offset1st+55);

	// orbiter main thrusters
	th_main[0] = CreateThruster (m_exhaust_pos1, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[1] = CreateThruster (m_exhaust_pos2, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[2] = CreateThruster (m_exhaust_pos3, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[3] = CreateThruster (m_exhaust_pos4, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[4] = CreateThruster (m_exhaust_pos5, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[5] = CreateThruster (m_exhaust_pos6, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[6] = CreateThruster (m_exhaust_pos7, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);
	th_main[7] = CreateThruster (m_exhaust_pos8, _V( 0,0,1), THRUST_FIRST_VAC , ph_main, ISP_FIRST_VAC, ISP_FIRST_SL);

	thg_main = CreateThrusterGroup (th_main, 8, THGROUP_MAIN);

	EXHAUSTSPEC es_1st[8] = {
		{ th_main[0], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
		{ th_main[1], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
		{ th_main[2], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
		{ th_main[3], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
		{ th_main[4], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
		{ th_main[5], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
		{ th_main[6], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex },
		{ th_main[7], NULL, NULL, NULL, 30.0, 0.80, 0, 0.1, tex }
	};

	for (i = 0; i < 8; i++)
		AddExhaust(es_1st + i);

	srb_exhaust.tex = oapiRegisterParticleTexture("ProjectApollo/Contrail_Saturn2");
	s1b_exhaust.tex = oapiRegisterParticleTexture("ProjectApollo/Contrail_Saturn");

	double exhpos = -4;
	double exhpos2 = -5;

	AddExhaustStream(th_main[0], m_exhaust_pos1 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_main[1], m_exhaust_pos2 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_main[2], m_exhaust_pos3 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_main[3], m_exhaust_pos4 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_main[4], m_exhaust_pos5 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_main[5], m_exhaust_pos6 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_main[6], m_exhaust_pos7 + _V(0, 0, exhpos2), &s1b_exhaust);
	AddExhaustStream(th_main[7], m_exhaust_pos8 + _V(0, 0, exhpos2), &s1b_exhaust);

	AddExhaustStream(th_main[0], m_exhaust_pos1 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_main[1], m_exhaust_pos2 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_main[2], m_exhaust_pos3 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_main[3], m_exhaust_pos4 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_main[4], m_exhaust_pos5 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_main[5], m_exhaust_pos6 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_main[6], m_exhaust_pos7 + _V(0, 0, exhpos), &srb_exhaust);
	AddExhaustStream(th_main[7], m_exhaust_pos8 + _V(0, 0, exhpos), &srb_exhaust);
}

void S1B::SetMainState(int s)

{
	MainState state;

	state.word = s;

	RetrosFired = (state.u.RetrosFired != 0);
	LowRes = (state.u.LowRes != 0);
}

void S1B::CreateAirfoils()
{
	if (hDockSIVB && DockingStatus(0))
	{
		SetCW(0.01, 1.1, 2, 2.4);
	}
	else
	{
		SetCW(0.5, 1.1, 2, 2.4);
	}
}

void S1B::clbkLoadStateEx (FILEHANDLE scn, void *vstatus)

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
			int i;
			sscanf (line+5, "%d", &i);
			State = (S1bState) i;
		}
		else if (!strnicmp(line, "SISYSTEMS_BEGIN", sizeof("SISYSTEMS_BEGIN"))) {
			sibsys.LoadState(scn);
		}
		else if (!strnicmp(line, "<INTERNALS>", 11)) { //INTERNALS signals the PanelSDK part of the scenario
			Panelsdk.Load(scn);			//send the loading to the Panelsdk
		}
		else
		{
			ParseScenarioLineEx (line, vstatus);
        }
	}
}

void S1B::clbkSetClassCaps (FILEHANDLE cfg)
{
	VESSEL2::clbkSetClassCaps (cfg);
	ClearMeshes();
	ClearThrusterDefinitions();
	ClearExhaustRefs();
	ClearAttExhaustRefs();

	SetSize(20);
	SetPMI(_V(78.45, 78.45, 12.00));
	SetCOG_elev(19.0);
	SetCrossSections(_V(230.35, 229.75, 58.85));
	CreateAirfoils();
	SetRotDrag(_V(2, 2, 2));
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0);

	SetEmptyMass(EmptyMass);

	double td_mass = 450000.0;
	double td_width = 10.0;
	double td_tdph = -20.955 - 3.0;
	double td_height = 40.0;

	static DWORD ntdp = 4;
	static TOUCHDOWNVTX td[4];
	double stiffness = (-1)*(td_mass*9.80655) / (3 * -0.05);
	double damping = 0.9*(2 * sqrt(td_mass*stiffness));
	for (int i = 0; i < 4; i++) {
		td[i].damping = damping;
		td[i].mu = 3;
		td[i].mu_lng = 3;
		td[i].stiffness = stiffness;
	}
	td[0].pos.x = -cos(30 * RAD)*td_width;
	td[0].pos.y = -sin(30 * RAD)*td_width;
	td[0].pos.z = td_tdph;
	td[1].pos.x = 0;
	td[1].pos.y = 1 * td_width;
	td[1].pos.z = td_tdph;
	td[2].pos.x = cos(30 * RAD)*td_width;
	td[2].pos.y = -sin(30 * RAD)*td_width;
	td[2].pos.z = td_tdph;
	td[3].pos.x = 0;
	td[3].pos.y = 0;
	td[3].pos.z = td_tdph + td_height;

	SetTouchdownPoints(td, ntdp);
}

void S1B::LoadMeshes(bool lowres)

{
	LowRes = lowres;
	if (LowRes)
	{
		hsat1stg1 = oapiLoadMeshGlobal("ProjectApollo/LowRes/nsat1stg1");
		hSat1intstg = oapiLoadMeshGlobal("ProjectApollo/LowRes/nsat1intstg");
	}
	else
	{
		hsat1stg1 = oapiLoadMeshGlobal("ProjectApollo/nsat1stg1");
		hSat1intstg = oapiLoadMeshGlobal("ProjectApollo/nsat1intstg");
	}	
	solid_exhaust.tex = oapiRegisterParticleTexture ("Contrail2");
}

void S1B::clbkDockEvent(int dock, OBJHANDLE connected)
{
	//S-IB/S-IVB staging
	if (dock == 0 && connected == NULL)
	{
		CreateAirfoils();
	}
}

void S1B::clbkPostCreation()
{
	LoadMeshes(LowRes);
	VECTOR3 mesh_dir = _V(0, 0, 0);
	AddMesh(hsat1stg1, &mesh_dir);

	mesh_dir = _V(0, 0, 16.2);
	AddMesh(hSat1intstg, &mesh_dir);
	AddEngines();

	if (SIB_SIVB_Sep.Blown())
	{
		if (hDockSIVB) {
			DelDock(hDockSIVB);
			hDockSIVB = NULL;
		}
	}

	CreateAirfoils();
}

void S1B::SeparateSIVB()
{
	if (hDockSIVB) {
		Undock(0);
		DelDock(hDockSIVB);
		hDockSIVB = NULL;
	}
}

void S1B::SetState(S1BSettings &state)

{
	if (state.SettingsType.S1B_SETTINGS_GENERAL)
	{
		VehicleNo = state.VehicleNo;
		RetroNum = state.RetroNum;
	}

	if (state.SettingsType.S1B_SETTINGS_MASS)
	{
		EmptyMass = state.EmptyMass;
	}

	if (state.SettingsType.S1B_SETTINGS_FUEL)
	{
		MainFuel = state.MainFuelKg;
	}

	if (state.SettingsType.S1B_SETTINGS_ENGINES)
	{
		THRUST_FIRST_VAC = state.THRUST_FIRST_VAC;
		ISP_FIRST_SL = state.ISP_FIRST_SL;
		ISP_FIRST_VAC = state.ISP_FIRST_VAC;
		CurrentThrust = state.CurrentThrust;
		EngineNum = state.EngineNum;
	}

	State = S1B_STATE_SHUTTING_DOWN;
	
	//For Saturn class
	SIB_SIVB_Sep.SetBlown(true);
	SeparateSIVB();
	sibsys.SetPropellantLevelSensorsEnable();
	sibsys.Set_SIB_SIVB_SeparationCmdLatch();
}

void S1B::SetH1ThrusterDir(int n, double beta_y, double beta_p)
{
	sibsys.SetThrusterDir(n, beta_y, beta_p);
}

void S1B::SwitchSelector(int channel)
{
	sibsys.SwitchSelector(channel);
}

bool S1B::GetLowLevelSensorsDry()
{
	return sibsys.GetLowLevelSensorsDry();
}

bool S1B::GetSIPropellantDepletionEngineCutoff()
{
	return sibsys.GetOutboardEnginesCutoff();
}

void S1B::GetSIThrustOK(bool *ok)
{
	sibsys.GetThrustOK(ok);
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	return new S1B(hvessel, flightmodel);
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	delete (S1B *)vessel;
}
