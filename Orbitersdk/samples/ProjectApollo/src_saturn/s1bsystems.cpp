/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IB Systems

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

#include "Orbitersdk.h"

#include "soundlib.h"

#include "apolloguidance.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "papi.h"

#include "s1bsystems.h"
#include "SCMUmbilical.h"

H1Engine::H1Engine(VESSEL *v, THRUSTER_HANDLE &h1, bool cangimbal, double pcant, double ycant)
	:vessel(v), th_h1(h1)
{
	EngineStart = false;
	EngineStop = false;
	ProgrammedCutoff = false;
	EDSCutoff = false;
	GSECutoff = false;
	RSSCutoff = false;
	EngineRunning = false;
	ThrustOK = false;
	EngineFailed = false;
	ThrustNotOKCutoff = false;

	ThrustTimer = 0.0;
	ThrustLevel = 0.0;

	EngineCanGimbal = cangimbal;
	pitchCant = pcant;
	yawCant = ycant;
}

void H1Engine::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	papiWriteScenario_bool(scn, "THRUSTOK", ThrustOK);
	papiWriteScenario_bool(scn, "ENGINESTART", EngineStart);
	papiWriteScenario_bool(scn, "ENGINESTOP", EngineStop);
	papiWriteScenario_bool(scn, "PROGRAMMEDCUTOFF", ProgrammedCutoff);
	papiWriteScenario_bool(scn, "EDSCUTOFF", EDSCutoff);
	papiWriteScenario_bool(scn, "GSECUTOFF", GSECutoff);
	papiWriteScenario_bool(scn, "RSSCUTOFF", RSSCutoff);
	papiWriteScenario_bool(scn, "THRUSTNOTOKCUTOFF", ThrustNotOKCutoff);
	papiWriteScenario_bool(scn, "ENGINERUNNING", EngineRunning);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	oapiWriteLine(scn, end_str);
}

void H1Engine::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "THRUSTOK", ThrustOK);
		papiReadScenario_bool(line, "ENGINESTART", EngineStart);
		papiReadScenario_bool(line, "ENGINESTOP", EngineStop);
		papiReadScenario_bool(line, "PROGRAMMEDCUTOFF", ProgrammedCutoff);
		papiReadScenario_bool(line, "EDSCUTOFF", EDSCutoff);
		papiReadScenario_bool(line, "GSECUTOFF", GSECutoff);
		papiReadScenario_bool(line, "RSSCUTOFF", RSSCutoff);
		papiReadScenario_bool(line, "THRUSTNOTOKCUTOFF", ThrustNotOKCutoff);
		papiReadScenario_bool(line, "ENGINERUNNING", EngineRunning);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
	}
}

void H1Engine::Timestep(double simdt)
{
	if (th_h1 == NULL) return;

	//Thrust OK switch
	ThrustOK = vessel->GetThrusterLevel(th_h1) > 0.9 && !EngineFailed;

	if (ProgrammedCutoff || EDSCutoff || GSECutoff || RSSCutoff || ThrustNotOKCutoff)
	{
		EngineStop = true;
	}

	if (EngineStop)
	{
		EngineStart = false;
		EngineRunning = false;

		if (ThrustLevel > 0)
		{

			double tm_1, tm_2, tm_3;

			ThrustTimer += simdt;

			tm_1 = 0.1;
			tm_2 = 0.3;
			tm_3 = 0.7;

			if (ThrustTimer >= tm_1)
			{
				if (ThrustTimer < tm_2)
				{
					ThrustLevel = min(ThrustLevel, 1.0 - 3.35*(ThrustTimer - tm_1));
				}
				else
				{
					if (ThrustTimer < tm_3)
					{
						ThrustLevel = min(ThrustLevel, 0.33 - 0.825*(ThrustTimer - tm_2));
					}
					else
					{
						ThrustLevel = 0;
					}
				}
			}
			else
			{
				ThrustLevel = min(ThrustLevel, 1);
			}

			vessel->SetThrusterLevel(th_h1, ThrustLevel);
		}
	}
	else if (EngineStart && !EngineRunning)
	{
		// SATURN IB ENGINE STARTUP
		// Engine startup was staggered 2-2-2-2, with engine 7+5 starting first, then 6+8, then 2+4, then 3+1

		// Engine 7+5 combustion chamber ignition was at T-2.998,  6+8 at T-2.898, 2+4 at T-2.798, 1+3 at T-2.698
		// The engines idled in low-range thrust (about 2.5% thrust) for about 0.3 seconds
		// and then rose to 93% thrust in 0.085 seconds.
		// The rise from 93 to 100 percent thrust took 0.75 second.
		// Total engine startup time was 1.9 seconds.

		// Source: Apollo 7 LV Flight Evaluation

		double tm_1, tm_2, tm_3, tm_4;

		ThrustTimer += simdt;

		tm_1 = -2.998 + 3.1;
		tm_2 = tm_1 + 0.3;  // Start of 1st rise
		tm_3 = tm_2 + 0.085; // Start of 2nd rise
		tm_4 = tm_3 + 0.75; // End of 2nd rise
		if (ThrustTimer >= tm_1) {
			// Light CC
			if (ThrustTimer < tm_2) {
				// Idle at 2.5% thrust
				ThrustLevel = 0.025;
			}
			else {
				if (ThrustTimer < tm_3) {
					// the actual rise is so fast that any 'smoothing' is pointless
					ThrustLevel = 0.93;
				}
				else {
					if (ThrustTimer < tm_4) {
						// Rise to 100% at a rate of 9 percent per second.
						ThrustLevel = 0.93 + (0.09*(ThrustTimer - tm_3));
					}
					else {
						// Hold 100%
						ThrustLevel = 1;
						EngineRunning = true;
					}
				}
			}
		}
		else
		{
			ThrustLevel = 0.0;
		}

		vessel->SetThrusterLevel(th_h1, ThrustLevel);
	}
	else if (ThrustTimer > 0.0)
	{
		ThrustTimer = 0.0;
	}
}

void H1Engine::SetThrusterDir(double beta_y, double beta_p)
{
	if (th_h1 == NULL) return;
	if (!EngineCanGimbal) return;

	double pitch, yaw;
	VECTOR3 f1vector;

	if (beta_y > GIMBALLIMIT)
	{
		yaw = GIMBALLIMIT;
	}
	else if (beta_y < -GIMBALLIMIT)
	{
		yaw = -GIMBALLIMIT;
	}
	else
	{
		yaw = beta_y;
	}

	if (beta_p > GIMBALLIMIT)
	{
		pitch = GIMBALLIMIT;
	}
	else if (beta_p < -GIMBALLIMIT)
	{
		pitch = -GIMBALLIMIT;
	}
	else
	{
		pitch = beta_p;
	}

	f1vector = unit(_V(yaw + yawCant, pitch + pitchCant, 1.0));

	vessel->SetThrusterDir(th_h1, f1vector);
}

SIBSystems::SIBSystems(VESSEL *v, THRUSTER_HANDLE *h1, PROPELLANT_HANDLE &h1prop, Pyro &SIB_SIVB_Sep, Sound &LaunchS, Sound &SShutS) :
	main_propellant(h1prop),
	h1engine1(v, h1[0], true, 2.45*RAD, 2.45*RAD),
	h1engine2(v, h1[1], true, -2.45*RAD, 2.45*RAD),
	h1engine3(v, h1[2], true, -2.45*RAD, -2.45*RAD),
	h1engine4(v, h1[3], true, 2.45*RAD, -2.45*RAD),
	h1engine5(v, h1[4], false, 0, 0),
	h1engine6(v, h1[5], false, 0, 0),
	h1engine7(v, h1[6], false, 0, 0),
	h1engine8(v, h1[7], false, 0, 0),
	SIB_SIVB_Separation_Pyros(SIB_SIVB_Sep),
	SShutSound(SShutS),
	LaunchSound(LaunchS)
{
	vessel = v;

	for (int i = 0;i < 8;i++)
	{
		ThrustOK[i] = false;
		EarlySICutoff[i] = false;
		FirstStageFailureTime[i] = 0.0;
		EngineCutoffRelay[i] = false;
	}

	MultiEngineCutoffInhibitBusPowered = false;
	LiftoffRelay = false;
	InboardEnginesCutoffRelay = false;
	OutboardEnginesCutoffRelay = false;
	LowPropellantLevelRelay = false;
	PropLevelSensorsEnabledAndRedundantChargingRelay = false;
	SingleEngineCutoffEnabledLatch = false;
	SingleEngineCutoffInhibitRelay = false;
	MultipleEngineCutoffEnabledLatch1 = false;
	MultipleEngineCutoffEnabledLatch2 = false;
	PropellantLevelSensorsEnabledLatch = false;
	InboardEnginesCutoffLatch = false;
	LOXDepletionCutoffEnabledLatch = false;
	OutboardEnginesCutoffLatch = false;
	LOXDepletionCutoffEnabledRelay = false;
	FuelDepletionCutoffEnabledLatch = false;
	PropellantLevelSensorsEnabledRelay = false;
	LOXLowLevelSensorInhibitRelay = false;
	FuelLowLevelSensorInhibitRelay = false;
	MultiEngineCutoffInhibitRelay = false;
	FuelLevelSensor = false;
	LOXLevelSensor = false;
	FuelDepletionSensors1 = false;
	FuelDepletionSensors2 = false;
	FuelDepletionCutoffEnabledRelay = false;
	FuelDepletionCutoffInhibitRelay1 = false;
	FuelDepletionCutoffInhibitRelay2 = false;
	OutboardEnginesCutoffSignal = false;
	SIB_SIVB_SeparationCmdLatch = false;
	EBWFiringUnit1TriggerRelay = false;
	EBWFiringUnit2TriggerRelay = false;

	h1engines[0] = &h1engine1;
	h1engines[1] = &h1engine2;
	h1engines[2] = &h1engine3;
	h1engines[3] = &h1engine4;
	h1engines[4] = &h1engine5;
	h1engines[5] = &h1engine6;
	h1engines[6] = &h1engine7;
	h1engines[7] = &h1engine8;

	SCMUmb = NULL;
}

void SIBSystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SISYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "MULTIENGINECUTOFFINHIBITBUSPOWERED", MultiEngineCutoffInhibitBusPowered);
	papiWriteScenario_bool(scn, "LIFTOFFRELAY", LiftoffRelay);
	papiWriteScenario_bool(scn, "INBOARDENGINESCUTOFFRELAY", InboardEnginesCutoffRelay);
	papiWriteScenario_bool(scn, "OUTBOARDENGINESCUTOFFRELAY", OutboardEnginesCutoffRelay);
	papiWriteScenario_bool(scn, "LOWPROPELLANTLEVELRELAY", LowPropellantLevelRelay);
	papiWriteScenario_bool(scn, "PROPLEVELSENSORSENABLEDANDREDUNDANTCHARGINGRELAY", PropLevelSensorsEnabledAndRedundantChargingRelay);
	papiWriteScenario_bool(scn, "SINGLEENGINECUTOFFENABLEDLATCH", SingleEngineCutoffEnabledLatch);
	papiWriteScenario_bool(scn, "SINGLEENGINECUTOFFINHIBITRELAY", SingleEngineCutoffInhibitRelay);
	papiWriteScenario_bool(scn, "MULTIPLEENGINECUTOFFENABLEDLATCH1", MultipleEngineCutoffEnabledLatch1);
	papiWriteScenario_bool(scn, "MULTIPLEENGINECUTOFFENABLEDLATCH2", MultipleEngineCutoffEnabledLatch2);
	papiWriteScenario_bool(scn, "PROPELLANTLEVELSENSORSENABLEDLATCH", PropellantLevelSensorsEnabledLatch);
	papiWriteScenario_bool(scn, "INBOARDENGINESCUTOFFLATCH", InboardEnginesCutoffLatch);
	papiWriteScenario_bool(scn, "LOXDEPLETIONCUTOFFENABLEDLATCH", LOXDepletionCutoffEnabledLatch);
	papiWriteScenario_bool(scn, "OUTBOARDENGINESCUTOFFLATCH", OutboardEnginesCutoffLatch);
	papiWriteScenario_bool(scn, "LOXDEPLETIONCUTOFFENABLEDRELAY", LOXDepletionCutoffEnabledRelay);
	papiWriteScenario_bool(scn, "FUELDEPLETIONCUTOFFENABLEDRELAY", FuelDepletionCutoffEnabledRelay);
	papiWriteScenario_bool(scn, "PROPELLANTLEVELSENSORSENABLEDRELAY", PropellantLevelSensorsEnabledRelay);
	papiWriteScenario_bool(scn, "LOXLOWLEVELSENSORSINHIBITRELAY", LOXLowLevelSensorInhibitRelay);
	papiWriteScenario_bool(scn, "FUELLOWLEVELSENSORSINHIBITRELAY", FuelLowLevelSensorInhibitRelay);
	papiWriteScenario_bool(scn, "MULTIENGINECUTOFFINHIBITRELAY", MultiEngineCutoffInhibitRelay);
	papiWriteScenario_bool(scn, "FUELLEVELSENSOR", FuelLevelSensor);
	papiWriteScenario_bool(scn, "LOXLEVELSENSOR", LOXLevelSensor);
	papiWriteScenario_bool(scn, "FUELDEPLETIONSENSORS1", FuelDepletionSensors1);
	papiWriteScenario_bool(scn, "FUELDEPLETIONSENSORS2", FuelDepletionSensors2);
	papiWriteScenario_bool(scn, "FUELDEPLETIONCUTOFFENABLEDRELAY", FuelDepletionCutoffEnabledRelay);
	papiWriteScenario_bool(scn, "FUELDEPLETIONCUTOFFINHIBITRELAY1", FuelDepletionCutoffInhibitRelay1);
	papiWriteScenario_bool(scn, "FUELDEPLETIONCUTOFFINHIBITRELAY2", FuelDepletionCutoffInhibitRelay2);
	papiWriteScenario_bool(scn, "OUTBOARDENGINESCUTOFFSIGNAL", OutboardEnginesCutoffSignal);
	papiWriteScenario_bool(scn, "SIBSIVBSEPARATIONCMDLATCH", SIB_SIVB_SeparationCmdLatch);
	papiWriteScenario_boolarr(scn, "THRUSTOK", ThrustOK, 24);

	h1engine1.SaveState(scn, "ENGINE1_BEGIN", "ENGINE_END");
	h1engine2.SaveState(scn, "ENGINE2_BEGIN", "ENGINE_END");
	h1engine3.SaveState(scn, "ENGINE3_BEGIN", "ENGINE_END");
	h1engine4.SaveState(scn, "ENGINE4_BEGIN", "ENGINE_END");
	h1engine5.SaveState(scn, "ENGINE5_BEGIN", "ENGINE_END");
	h1engine6.SaveState(scn, "ENGINE6_BEGIN", "ENGINE_END");
	h1engine7.SaveState(scn, "ENGINE7_BEGIN", "ENGINE_END");
	h1engine8.SaveState(scn, "ENGINE8_BEGIN", "ENGINE_END");

	oapiWriteLine(scn, SISYSTEMS_END_STRING);
}

void SIBSystems::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SISYSTEMS_END_STRING, sizeof(SISYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "MULTIENGINECUTOFFINHIBITBUSPOWERED", MultiEngineCutoffInhibitBusPowered);
		papiReadScenario_bool(line, "LIFTOFFRELAY", LiftoffRelay);
		papiReadScenario_bool(line, "INBOARDENGINESCUTOFFRELAY", InboardEnginesCutoffRelay);
		papiReadScenario_bool(line, "OUTBOARDENGINESCUTOFFRELAY", OutboardEnginesCutoffRelay);
		papiReadScenario_bool(line, "LOWPROPELLANTLEVELRELAY", LowPropellantLevelRelay);
		papiReadScenario_bool(line, "PROPLEVELSENSORSENABLEDANDREDUNDANTCHARGINGRELAY", PropLevelSensorsEnabledAndRedundantChargingRelay);
		papiReadScenario_bool(line, "SINGLEENGINECUTOFFENABLEDLATCH", SingleEngineCutoffEnabledLatch);
		papiReadScenario_bool(line, "SINGLEENGINECUTOFFINHIBITRELAY", SingleEngineCutoffInhibitRelay);
		papiReadScenario_bool(line, "MULTIPLEENGINECUTOFFENABLEDLATCH1", MultipleEngineCutoffEnabledLatch1);
		papiReadScenario_bool(line, "MULTIPLEENGINECUTOFFENABLEDLATCH2", MultipleEngineCutoffEnabledLatch2);
		papiReadScenario_bool(line, "PROPELLANTLEVELSENSORSENABLEDLATCH", PropellantLevelSensorsEnabledLatch);
		papiReadScenario_bool(line, "INBOARDENGINESCUTOFFLATCH", InboardEnginesCutoffLatch);
		papiReadScenario_bool(line, "LOXDEPLETIONCUTOFFENABLEDLATCH", LOXDepletionCutoffEnabledLatch);
		papiReadScenario_bool(line, "OUTBOARDENGINESCUTOFFLATCH", OutboardEnginesCutoffLatch);
		papiReadScenario_bool(line, "LOXDEPLETIONCUTOFFENABLEDRELAY", LOXDepletionCutoffEnabledRelay);
		papiReadScenario_bool(line, "FUELDEPLETIONCUTOFFENABLEDRELAY", FuelDepletionCutoffEnabledRelay);
		papiReadScenario_bool(line, "PROPELLANTLEVELSENSORSENABLEDRELAY", PropellantLevelSensorsEnabledRelay);
		papiReadScenario_bool(line, "LOXLOWLEVELSENSORSINHIBITRELAY", LOXLowLevelSensorInhibitRelay);
		papiReadScenario_bool(line, "FUELLOWLEVELSENSORSINHIBITRELAY", FuelLowLevelSensorInhibitRelay);
		papiReadScenario_bool(line, "MULTIENGINECUTOFFINHIBITRELAY", MultiEngineCutoffInhibitRelay);
		papiReadScenario_bool(line, "FUELLEVELSENSOR", FuelLevelSensor);
		papiReadScenario_bool(line, "LOXLEVELSENSOR", LOXLevelSensor);
		papiReadScenario_bool(line, "FUELDEPLETIONSENSORS1", FuelDepletionSensors1);
		papiReadScenario_bool(line, "FUELDEPLETIONSENSORS2", FuelDepletionSensors2);
		papiReadScenario_bool(line, "FUELDEPLETIONCUTOFFENABLEDRELAY", FuelDepletionCutoffEnabledRelay);
		papiReadScenario_bool(line, "FUELDEPLETIONCUTOFFINHIBITRELAY1", FuelDepletionCutoffInhibitRelay1);
		papiReadScenario_bool(line, "FUELDEPLETIONCUTOFFINHIBITRELAY2", FuelDepletionCutoffInhibitRelay2);
		papiReadScenario_bool(line, "OUTBOARDENGINESCUTOFFSIGNAL", OutboardEnginesCutoffSignal);
		papiReadScenario_bool(line, "SIBSIVBSEPARATIONCMDLATCH", SIB_SIVB_SeparationCmdLatch);
		papiReadScenario_boolarr(line, "THRUSTOK", ThrustOK, 24);

		if (!strnicmp(line, "ENGINE1_BEGIN", sizeof("ENGINE1_BEGIN"))) {
			h1engine1.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE2_BEGIN", sizeof("ENGINE2_BEGIN"))) {
			h1engine2.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE3_BEGIN", sizeof("ENGINE3_BEGIN"))) {
			h1engine3.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE4_BEGIN", sizeof("ENGINE4_BEGIN"))) {
			h1engine4.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE5_BEGIN", sizeof("ENGINE5_BEGIN"))) {
			h1engine5.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE6_BEGIN", sizeof("ENGINE6_BEGIN"))) {
			h1engine6.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE7_BEGIN", sizeof("ENGINE7_BEGIN"))) {
			h1engine7.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE8_BEGIN", sizeof("ENGINE8_BEGIN"))) {
			h1engine8.LoadState(scn, "ENGINE_END");
		}
	}
}

void SIBSystems::Timestep(double misst, double simdt)
{
	h1engine1.Timestep(simdt);
	h1engine2.Timestep(simdt);
	h1engine3.Timestep(simdt);
	h1engine4.Timestep(simdt);
	h1engine5.Timestep(simdt);
	h1engine6.Timestep(simdt);
	h1engine7.Timestep(simdt);
	h1engine8.Timestep(simdt);

	//Thrust OK
	for (int i = 0;i < 8;i++)
	{
		for (int j = 0;j < 3;j++)
		{
			ThrustOK[i * 3 + j] = h1engines[i]->GetThrustOK() || ESEGetSIBThrustOKSimulate(i + 1, j + 1);
		}
	}

	if (IsUmbilicalConnected())
	{
		LiftoffRelay = true;
	}
	else
	{
		LiftoffRelay = false;
	}

	if (SIB_SIVB_SeparationCmdLatch)
	{
		EBWFiringUnit1TriggerRelay = true;
		EBWFiringUnit2TriggerRelay = true;
	}
	else
	{
		EBWFiringUnit1TriggerRelay = false;
		EBWFiringUnit2TriggerRelay = false;
	}

	if (EBWFiringUnit1TriggerRelay || EBWFiringUnit2TriggerRelay)
	{
		SIB_SIVB_Separation_Pyros.SetBlown(true);
	}

	if (PropellantLevelSensorsEnabledLatch)
	{
		PropLevelSensorsEnabledAndRedundantChargingRelay = true;
	}
	else
	{
		PropLevelSensorsEnabledAndRedundantChargingRelay = false;
	}

	if (PropLevelSensorsEnabledAndRedundantChargingRelay)
	{
		PropellantLevelSensorsEnabledRelay = true;
	}
	else
	{
		PropellantLevelSensorsEnabledRelay = false;
	}

	//Propellant sensors
	LOXLevelSensor = vessel->GetPropellantMass(main_propellant) < 24000.0;
	FuelLevelSensor = vessel->GetPropellantMass(main_propellant) < 24000.0;
	FuelDepletionSensors1 = vessel->GetPropellantMass(main_propellant) < 4211.152;
	FuelDepletionSensors2 = vessel->GetPropellantMass(main_propellant) < 4211.152;

	if (LOXLowLevelSensorInhibitRelay || (!PropellantLevelSensorsEnabledRelay && LOXLevelSensor))
	{
		LOXLowLevelSensorInhibitRelay = true;
	}
	else
	{
		LOXLowLevelSensorInhibitRelay = false;
	}

	if (FuelLowLevelSensorInhibitRelay || (!PropellantLevelSensorsEnabledRelay && FuelLevelSensor))
	{
		FuelLowLevelSensorInhibitRelay = true;
	}
	else
	{
		FuelLowLevelSensorInhibitRelay = false;
	}

	if (PropellantLevelSensorsEnabledRelay && ((LOXLevelSensor && !LOXLowLevelSensorInhibitRelay) || (FuelLevelSensor && !FuelLowLevelSensorInhibitRelay)))
	{
		LowPropellantLevelRelay = true;
	}
	else
	{
		LowPropellantLevelRelay = false;
	}

	if (!MultipleEngineCutoffEnabledLatch1 && !MultipleEngineCutoffEnabledLatch2)
	{
		MultiEngineCutoffInhibitRelay = true;
		MultiEngineCutoffInhibitBusPowered = true;
	}
	else
	{
		MultiEngineCutoffInhibitRelay = false;
		MultiEngineCutoffInhibitBusPowered = false;
	}

	if (!SingleEngineCutoffEnabledLatch && !LiftoffRelay)
	{
		SingleEngineCutoffInhibitRelay = true;
	}
	else
	{
		SingleEngineCutoffInhibitRelay = false;
	}

	if ((EngineCutoffRelay[0] || EngineCutoffRelay[1] || EngineCutoffRelay[2] || EngineCutoffRelay[3] ||
		EngineCutoffRelay[4] || EngineCutoffRelay[5] || EngineCutoffRelay[6] || EngineCutoffRelay[7]) && 
		MultiEngineCutoffInhibitBusPowered)
	{
		SingleEngineFailedInhibitRelay = true;
	}
	else
	{
		SingleEngineFailedInhibitRelay = false;
	}

	if (InboardEnginesCutoffLatch)
	{
		InboardEnginesCutoffRelay = true;
	}
	else
	{
		InboardEnginesCutoffRelay = false;
	}

	if (LOXDepletionCutoffEnabledLatch)
	{
		LOXDepletionCutoffEnabledRelay = true;
	}
	else
	{
		LOXDepletionCutoffEnabledRelay = false;
	}

	if (FuelDepletionCutoffEnabledLatch)
	{
		FuelDepletionCutoffEnabledRelay = true;
	}
	else
	{
		FuelDepletionCutoffEnabledRelay = false;
	}

	if (FuelDepletionCutoffInhibitRelay1 || (FuelDepletionSensors1 && !FuelDepletionCutoffEnabledRelay))
	{
		FuelDepletionCutoffInhibitRelay1 = true;
	}
	else
	{
		FuelDepletionCutoffInhibitRelay1 = false;
	}

	if (FuelDepletionCutoffInhibitRelay2 || (FuelDepletionSensors2 && !FuelDepletionCutoffEnabledRelay))
	{
		FuelDepletionCutoffInhibitRelay2 = true;
	}
	else
	{
		FuelDepletionCutoffInhibitRelay2 = false;
	}

	if (OutboardEnginesCutoffLatch)
	{
		OutboardEnginesCutoffRelay = true;
	}
	else if (FuelDepletionCutoffEnabledRelay && ((!FuelDepletionCutoffInhibitRelay1 && FuelDepletionSensors1) || (!FuelDepletionCutoffInhibitRelay2 && FuelDepletionSensors2)))
	{
		OutboardEnginesCutoffSignal = true;
		OutboardEnginesCutoffRelay = true;
	}
	else
	{
		OutboardEnginesCutoffRelay = false;
	}

	//Outboard Engines
	for (int i = 0;i < 4;i++)
	{
		if (((!SingleEngineFailedInhibitRelay && (!MultiEngineCutoffInhibitRelay || !SingleEngineCutoffInhibitRelay)) || EngineCutoffRelay[i]) && !ThrustOK[i])
		{
			EngineCutoffRelay[i] = true;
		}
		else if (LOXDepletionCutoffEnabledRelay && OutboardEnginesCutoffRelay)
		{
			OutboardEnginesCutoffSignal = true;
			EngineCutoffRelay[i] = true;
		}
		else
		{
			EngineCutoffRelay[i] = false;
		}
	}

	//Inboard Engines
	for (int i = 4;i < 8;i++)
	{
		if (((!SingleEngineFailedInhibitRelay && !SingleEngineCutoffInhibitRelay) || EngineCutoffRelay[i]) && !ThrustOK[i])
		{
			EngineCutoffRelay[i] = true;
		}
		else if (PropLevelSensorsEnabledAndRedundantChargingRelay && InboardEnginesCutoffRelay)
		{
			EngineCutoffRelay[i] = true;
		}
		else
		{
			EngineCutoffRelay[i] = false;
		}
	}

	for (int i = 0;i < 8;i++)
	{
		if (EngineCutoffRelay[i] && !LiftoffRelay)
		{
			h1engines[i]->SetThrustNotOKCutoff();
		}
	}

	//Failure code
	for (int i = 0;i < 8;i++)
	{
		if (EarlySICutoff[i] && (misst > FirstStageFailureTime[i]) && !h1engines[i]->GetFailed())
		{
			h1engines[i]->SetFailed();
		}
	}

	//sprintf(oapiDebugString(), "Startup: %f %f %f %f %f %f %f %f", h1engine1.GetThrustLevel(), h1engine2.GetThrustLevel(),
	//	h1engine3.GetThrustLevel(), h1engine4.GetThrustLevel(), h1engine5.GetThrustLevel(), h1engine6.GetThrustLevel(),
	//	h1engine7.GetThrustLevel(), h1engine8.GetThrustLevel());
	//sprintf(oapiDebugString(), "Single Fail: Liftoff %d Latch: %d Inhibit: %d", LiftoffRelay, SingleEngineCutoffEnabledLatch, SingleEngineCutoffInhibitRelay);
	//sprintf(oapiDebugString(), "Multi Fail: Latch1 %d Latch2 %d Bus %d Relay %d Single Fail %d", MultipleEngineCutoffEnabledLatch1, MultipleEngineCutoffEnabledLatch2, MultiEngineCutoffInhibitBusPowered, MultiEngineCutoffInhibitRelay, SingleEngineFailedInhibitRelay);
	//sprintf(oapiDebugString(), "Inboard: Latch %d Relay1 %d Relay2 %d Sensor %d Interrupt %d CutCmd %d CutRelay %d", 
	//	PropellantLevelSensorsEnabledLatch, PropLevelSensorsEnabledAndRedundantChargingRelay, PropellantLevelSensorsEnabledRelay, 
	//	LOXLevelSensor, LowPropellantLevelRelay, InboardEnginesCutoffLatch, InboardEnginesCutoffRelay);
	//sprintf(oapiDebugString(), "Outboard: LOX Latch %d Relay %d Fuel Latch %d Relay %d Sensor %d Cmd %d",
	//	LOXDepletionCutoffEnabledLatch, LOXDepletionCutoffEnabledRelay, FuelDepletionCutoffEnabledLatch, FuelDepletionCutoffEnabledRelay,
	//	FuelDepletionSensors1, OutboardEnginesCutoffRelay);
}

void SIBSystems::SetEngineStart(int n)
{
	if (n < 1 || n > 8) return;

	h1engines[n - 1]->SetEngineStart();
}

void SIBSystems::SetThrusterDir(int n, double beta_y, double beta_p)
{
	if (n < 0 || n > 3) return;

	h1engines[n]->SetThrusterDir(beta_y, beta_p);
}

void SIBSystems::EDSEnginesCutoff(bool cut)
{
	if (cut)
	{
		h1engine1.SetEDSCutoff();
		h1engine2.SetEDSCutoff();
		h1engine3.SetEDSCutoff();
		h1engine4.SetEDSCutoff();
		h1engine5.SetEDSCutoff();
		h1engine6.SetEDSCutoff();
		h1engine7.SetEDSCutoff();
		h1engine8.SetEDSCutoff();
	}
}

bool SIBSystems::GetLowLevelSensorsDry()
{
	return LowPropellantLevelRelay;
}

bool SIBSystems::GetOutboardEngineOut()
{
	for (int i = 0;i < 4;i++)
	{
		if (!ThrustOK[i]) return true;
	}

	return false;
}

bool SIBSystems::GetInboardEngineOut()
{
	for (int i = 4;i < 8;i++)
	{
		if (!ThrustOK[i]) return true;
	}

	return false;
}
bool SIBSystems::GetOutboardEnginesCutoff()
{
	return OutboardEnginesCutoffSignal;
}

bool SIBSystems::FireRetroRockets()
{
	//Charge and Trigger
	return (LowPropellantLevelRelay && (EBWFiringUnit1TriggerRelay || EBWFiringUnit2TriggerRelay));
}

double SIBSystems::GetSumThrust()
{
	double thrust = 0.0;

	for (int i = 0;i < 8;i++)
	{
		thrust += h1engines[i]->GetThrustLevel();
	}

	return thrust;
}

void SIBSystems::SetEngineFailureParameters(bool *SICut, double *SICutTimes)
{
	for (int i = 0;i < 8;i++)
	{
		EarlySICutoff[i] = SICut[i];
		FirstStageFailureTime[i] = SICutTimes[i];
	}
}

void SIBSystems::SetEngineFailureParameters(int n, double SICutTimes, bool fail)
{
	if (n < 1 || n > 8) return;

	EarlySICutoff[n - 1] = fail;
	FirstStageFailureTime[n - 1] = SICutTimes;
}

void SIBSystems::GetEngineFailureParameters(int n, bool &fail, double &failtime)
{
	if (n < 1 || n > 8) return;

	fail = EarlySICutoff[n - 1];
	failtime = FirstStageFailureTime[n - 1];
}

bool SIBSystems::GetEngineStop()
{
	for (int i = 0;i < 5;i++) if (h1engines[i]->GetEngineStop()) return true;

	return false;
}

void SIBSystems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 0: //Liftoff (NOT A REAL SWITCH SELECTOR EVENT)
		if (LaunchSound.isValid() && !LaunchSound.isPlaying()) {	// And play launch sound
			LaunchSound.play(NOLOOP, 255);
			LaunchSound.done();
		}
		break;
	case 2: //Telemeter Calibration On
		break;
	case 15: //Multiple Engine Cutoff Enable No. 2
		SetMultipleEngineCutoffEnable2();
		break;
	case 16: //Multiple Engine Cutoff Enable No. 1
		SetMultipleEngineCutoffEnable1();
		break;
	case 18: //Outboard Engines Cutoff
		SetOutboardEnginesCutoff();
		break;
	case 23: //S-IB/S-IVB Separation On
		Set_SIB_SIVB_SeparationCmdLatch();
		break;
	case 39: //Telemeter Calibration Off
		break;
	case 79: //Fuel Depletion Cutoff Enable
		SetFuelDepletionCutoffEnable();
		break;
	case 97: //LOX Depletion Cutoff Enable
		SetLOXDepletionCutoffEnable();
		break;
	case 98: //Inboard Engines Cutoff
		SetInboardEnginesCutoff();
		SShutSound.play(NOLOOP, 235);
		SShutSound.done();
		break;
	case 100: //Single Engine Cutoff Enable
		SetSingleEngineCutoffEnable();
		break;
	case 104: //Propellant Level Sensors Enable
		SetPropellantLevelSensorsEnable();
		break;
	default:
		break;
	}
}

void SIBSystems::ConnectUmbilical(SCMUmbilical *umb)
{
	SCMUmb = umb;
}

void SIBSystems::DisconnectUmbilical()
{
	SCMUmb = NULL;
}

bool SIBSystems::IsUmbilicalConnected()
{
	if (SCMUmb && SCMUmb->IsUmbilicalConnected()) return true;

	return false;
}

bool SIBSystems::ESEGetSIBThrustOKSimulate(int eng, int n)
{
	if (!IsUmbilicalConnected()) return false;

	return SCMUmb->ESEGetSIBThrustOKSimulate(eng, n);
}

void SIBSystems::GetThrustOK(bool *ok)
{
	for (int i = 0;i < 24;i++)
	{
		ok[i] = ThrustOK[i];
	}
}

void SIBSystems::GSEEnginesCutoff(bool cut)
{
	if (cut)
	{
		for (int i = 0;i < 8;i++)
		{
			h1engines[i]->SetGSECutoff();
		}
	}
}