/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-IVB Systems

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

#include "sivbsystems.h"

SIVBSystems::SIVBSystems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THRUSTER_HANDLE &lox, THGROUP_HANDLE &ver) :
	j2engine(j2), loxvent(lox), vernier(ver), main_propellant(j2prop), apsThrusters(aps), ullage(ull) {

	vessel = v;

	FirstBurnRelay = false;
	SecondBurnRelay = false;
	EngineStart = false;
	LVDCEngineStopRelay = false;
	EDSEngineStop = false;
	EngineReady = false;
	EnginePower = false;
	PropellantDepletionSensors = false;
	RSSEngineStop = false;
	ThrustOKRelay = false;
	LOXVentValveOpen = false;
	FireUllageIgnition = false;
	PointLevelSensorArmed = false;
	LH2ContinuousVentValveOpen = false;
	ThrustOKCutoffInhibit = false;
	PrevalvesCloseOn = false;
	//Gets switched on at about T-8 minutes
	AuxHydPumpFlightMode = true;

	for (int i = 0;i < 2;i++)
	{
		APSUllageOnRelay[i] = false;
	}

	PUValveState = PUVALVE_NULL;

	J2DefaultThrust = 0.0;
	ThrustTimer = 0.0;
	ThrustLevel = 0.0;
	BoiloffTime = 0.0;
	LH2TankUllagePressurePSI = 50.0;
	LOXTankUllagePressurePSI = 50.0;
}

SIVBSystems::~SIVBSystems()
{}

void SIVBSystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SIVBSYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "FIRSTBURNRELAY", FirstBurnRelay);
	papiWriteScenario_bool(scn, "SECONDBURNRELAY", SecondBurnRelay);
	papiWriteScenario_bool(scn, "ENGINESTART", EngineStart);
	papiWriteScenario_bool(scn, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
	papiWriteScenario_bool(scn, "EDSENGINESTOP", EDSEngineStop);
	papiWriteScenario_bool(scn, "RSSENGINESTOP", RSSEngineStop);
	papiWriteScenario_bool(scn, "ENGINESTOP", EngineStop);
	papiWriteScenario_bool(scn, "ENGINEREADY", EngineReady);
	papiWriteScenario_bool(scn, "LOXVENTVALVEOPEN", LOXVentValveOpen);
	papiWriteScenario_bool(scn, "FIREULLAGEIGNITION", FireUllageIgnition);
	papiWriteScenario_bool(scn, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
	papiWriteScenario_bool(scn, "LH2CONTINUOUSVENTVALVEOPEN", LH2ContinuousVentValveOpen);
	papiWriteScenario_bool(scn, "THRUSTOKCUTOFFINHIBIT", ThrustOKCutoffInhibit);
	papiWriteScenario_boolarr(scn, "APSULLAGEONRELAY", APSUllageOnRelay, 2);
	oapiWriteScenario_int(scn, "PUVALVESTATE", PUValveState);
	papiWriteScenario_double(scn, "BOILOFFTIME", BoiloffTime);
	papiWriteScenario_double(scn, "THRUSTTIMER", ThrustTimer);
	papiWriteScenario_double(scn, "THRUSTLEVEL", ThrustLevel);
	papiWriteScenario_double(scn, "J2DEFAULTTHRUST", J2DefaultThrust);

	oapiWriteLine(scn, SIVBSYSTEMS_END_STRING);
}

void SIVBSystems::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SIVBSYSTEMS_END_STRING, sizeof(SIVBSYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "FIRSTBURNRELAY", FirstBurnRelay);
		papiReadScenario_bool(line, "SECONDBURNRELAY", SecondBurnRelay);
		papiReadScenario_bool(line, "ENGINESTART", EngineStart);
		papiReadScenario_bool(line, "LVDCENGINESTOPRELAY", LVDCEngineStopRelay);
		papiReadScenario_bool(line, "EDSENGINESTOP", EDSEngineStop);
		papiReadScenario_bool(line, "RSSENGINESTOP", RSSEngineStop);
		papiReadScenario_bool(line, "ENGINESTOP", EngineStop);
		papiReadScenario_bool(line, "ENGINEREADY", EngineReady);
		papiReadScenario_bool(line, "LOXVENTVALVEOPEN", LOXVentValveOpen);
		papiReadScenario_bool(line, "FIREULLAGEIGNITION", FireUllageIgnition);
		papiReadScenario_bool(line, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
		papiReadScenario_bool(line, "LH2CONTINUOUSVENTVALVEOPEN", LH2ContinuousVentValveOpen);
		papiReadScenario_bool(line, "THRUSTOKCUTOFFINHIBIT", ThrustOKCutoffInhibit);
		papiReadScenario_boolarr(line, "APSULLAGEONRELAY", APSUllageOnRelay, 2);
		papiReadScenario_int(line, "PUVALVESTATE", PUValveState);
		papiReadScenario_double(line, "BOILOFFTIME", BoiloffTime);
		papiReadScenario_double(line, "THRUSTTIMER", ThrustTimer);
		papiReadScenario_double(line, "THRUSTLEVEL", ThrustLevel);
		papiReadScenario_double(line, "J2DEFAULTTHRUST", J2DefaultThrust);

	}
}

void SIVBSystems::Timestep(double simdt)
{
	if (j2engine == NULL) return;

	//Thrust OK switch
	bool ThrustOK = vessel->GetThrusterLevel(j2engine) > 0.65;

	//Propellant Systems
	if (main_propellant)
	{
		LOXTankUllagePressurePSI = vessel->GetPropellantMass(main_propellant) / vessel->GetPropellantMaxMass(main_propellant)*50.0;
		LH2TankUllagePressurePSI = LOXTankUllagePressurePSI * 0.9362 + 3.19;
	}

	//Propellant Depletion
	if (PropellantLowLevel())
	{
		PropellantDepletionSensors = true;
	}
	else
	{
		PropellantDepletionSensors = false;
	}

	//Engine Control Power Switch (EDS no. 1, range safety no. 1)
	if (EDSEngineStop || RSSEngineStop)
	{
		AftPowerDisableRelay = true;
	}
	else
	{
		AftPowerDisableRelay = false;
	}

	//Engine Power
	if (!AftPowerDisableRelay)
	{
		EnginePower = true;
	}
	else
	{
		EnginePower = false;
	}

	if (EnginePower && ThrustOK)
	{
		ThrustOKRelay = true;
	}
	else
	{
		ThrustOKRelay = false;
	}

	if (EngineReady && EngineStart && !ThrustOKRelay)
	{
		ThrustOKCutoffInhibit = true;
	}

	//Engine Cutoff Bus (switch selector, range safety system no. 2, EDS no. 2, propellant depletion sensors)
	if (LVDCEngineStopRelay || (!EDSCutoffDisabled && EDSEngineStop) || RSSEngineStop || PropellantDepletionSensors)
	{
		EngineCutoffBus = true;
	}
	else
	{
		EngineCutoffBus = false;
	}

	if (EngineCutoffBus)
	{
		EngineStop = true;
	}
	else
	{
		EngineStop = false;
	}

	if (EngineStop)
	{
		if (EngineReady == true)
		{
			ThrustTimer = 0.0;
			EngineStart = false;
			EngineReady = false;
			ThrustOKCutoffInhibit = false;
		}

		if (ThrustLevel > 0.0)
		{
			ThrustTimer += simdt;

			// Cutoff transient thrust
			if (ThrustTimer < 2.0) {
				if (ThrustTimer < 0.25) {
					// 95% of thrust dies in the first .25 second
					ThrustLevel = 1.0 - (ThrustTimer*3.3048);
					vessel->SetThrusterLevel(j2engine, ThrustLevel);
				}
				else {
					if (ThrustTimer < 1.5) {
						// The remainder dies over the next 1.25 second
						ThrustLevel = 0.1738 - ((ThrustTimer - 0.25)*0.1390);
						vessel->SetThrusterLevel(j2engine, ThrustLevel);
					}
					else {
						// Engine is completely shut down at 1.5 second
						ThrustLevel = 0.0;
						vessel->SetThrusterLevel(j2engine, ThrustLevel);
					}
				}
			}
		}
	}
	else if ((EngineReady && EngineStart) || ThrustLevel > 0.0 || ThrustOKCutoffInhibit)
	{
		ThrustTimer += simdt;

		if (SecondBurnRelay)
		{
			//Second Burn
			if (ThrustTimer >= 8.6 && ThrustTimer < 11.4) {
				ThrustLevel = (ThrustTimer - 8.6)*0.357;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
			}
			else if (ThrustTimer > 11.4 && ThrustLevel < 1.0)
			{
				ThrustLevel = 1.0;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
				ThrustOKCutoffInhibit = false;
			}
		}
		else
		{
			//First Burn
			if (ThrustTimer >= 3.0 && ThrustTimer < 5.8) {
				ThrustLevel = (ThrustTimer - 3.0)*0.357;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
			}
			else if (ThrustTimer > 5.8 && ThrustLevel < 1.0)
			{
				ThrustLevel = 1.0;
				vessel->SetThrusterLevel(j2engine, ThrustLevel);
				ThrustOKCutoffInhibit = false;
			}
		}
	}
	else if (ThrustTimer > 0.0)
	{
		ThrustTimer = 0.0;
	}

	//Venting

	if (loxvent)
	{
		if (vessel->GetThrusterLevel(loxvent) < 1.0 && LOXVentValveOpen)
		{
			vessel->SetThrusterLevel(loxvent, 1.0);
		}
		else if (vessel->GetThrusterLevel(loxvent) > 0.0 && !LOXVentValveOpen)
		{
			vessel->SetThrusterLevel(loxvent, 0.0);
		}

	}

	if (LH2ContinuousVentValveOpen)
	{
		BoiloffTime += simdt;

		if (BoiloffTime > 10.0) {
			SIVBBoiloff();
			BoiloffTime -= 10.0;
		}
	}

	if (vernier)
	{
		if (vessel->GetThrusterGroupLevel(vernier) < 1.0 && FireUllageIgnition)
		{
			vessel->SetThrusterGroupLevel(vernier, 1.0);
			FireUllageIgnition = false;
		}
	}

	//sprintf(oapiDebugString(), "First %d Second %d Start %d Stop %d Ready %d Cut Inhibit %d Level %f Timer %f", FirstBurnRelay, SecondBurnRelay, EngineStart, EngineStop, EngineReady, ThrustOKCutoffInhibit, ThrustLevel, ThrustTimer);
}

void SIVBSystems::SIVBBoiloff()
{
	double FuelMass = vessel->GetPropellantMass(main_propellant) * 0.99998193;
	vessel->SetPropellantMass(main_propellant, FuelMass);
}

void SIVBSystems::SetThrusterDir(double beta_y, double beta_p)
{
	if (j2engine == NULL) return;
	//Check if aux or main hydraulic pumps are on
	if (!AuxHydPumpFlightMode && vessel->GetThrusterLevel(j2engine) < 0.1) return;

	VECTOR3 j2vector;

	if (beta_y > 7.0*RAD)
	{
		j2vector.x = 7.0*RAD;
	}
	else if (beta_y < -7.0*RAD)
	{
		j2vector.x = -7.0*RAD;
	}
	else
	{
		j2vector.x = beta_y;
	}

	if (beta_p > 7.0*RAD)
	{
		j2vector.y = 7.0*RAD;
	}
	else if (beta_p < -7.0*RAD)
	{
		j2vector.y = -7.0*RAD;
	}
	else
	{
		j2vector.y = beta_p;
	}

	j2vector.z = 1.0;

	vessel->SetThrusterDir(j2engine, j2vector);
}

bool SIVBSystems::PropellantLowLevel()
{
	if (PointLevelSensorArmed)
	{
		if (main_propellant)
		{
			if (vessel->GetPropellantMass(main_propellant) < 50.0)
			{
				return true;
			}
		}
	}

	return false;
}

void SIVBSystems::APSUllageEngineOn(int n)
{
	if (n < 1 || n > 2) return;
	if (ullage[n - 1])
		vessel->SetThrusterLevel(ullage[n - 1], 1);
}

void SIVBSystems::APSUllageEngineOff(int n)
{
	if (n < 1 || n > 2) return;
	if (ullage[n - 1])
		vessel->SetThrusterLevel(ullage[n - 1], 0);
}

void SIVBSystems::SetAPSAttitudeEngine(int n, bool on)
{
	if (apsThrusters[0])
	{
		if (on)
		{
			vessel->SetThrusterLevel(apsThrusters[n], 1.0);
		}
		else
		{
			vessel->SetThrusterLevel(apsThrusters[n], 0.0);
		}
	}
}

void SIVBSystems::SetPUValve(int state)
{
	if (state == PUVALVE_CLOSED)
	{
		PUValveState = PUVALVE_CLOSED;
	}
	else if (state == PUVALVE_NULL)
	{
		PUValveState = PUVALVE_NULL;
	}
	else if (state == PUVALVE_OPEN)
	{
		PUValveState = PUVALVE_OPEN;
	}

	RecalculateEngineParameters();
}

void SIVBSystems::RecalculateEngineParameters(double BaseThrust)
{
	J2DefaultThrust = BaseThrust;

	RecalculateEngineParameters();
}

//
// Get the J2 ISP from the mixture ratio and calculate the thrust adjustment.
//

#define MR_STATS 5

static double MixtureRatios[MR_STATS] = { 6.0, 5.5, 5.0, 4.3, 4.0 };
static double MRISP[MR_STATS] = { 421.4 * G, 423.4 * G, 426.5 * G, 432.6 * G, 437.6 * G };
static double MRThrust[MR_STATS] = { 1.1, 1.0, .898, .7391, .7 };

void SIVBSystems::GetJ2ISP(double ratio, double &isp, double &ThrustAdjust)

{
	isp = 421 * G;

	// From Usenet:
	// It had roughly three stops. 178,000 lbs at 425s Isp and an O/F of 4.5,
	// 207,000 lbs at 421s Isp and an O/F of 5.0, and 230,500 lbs at 418s Isp
	// and an O/F of 5.5.

	for (int i = 0; i < MR_STATS; i++) {
		if (ratio >= MixtureRatios[i]) {
			double delta = (ratio - MixtureRatios[i]) / (MixtureRatios[i - 1] - MixtureRatios[i]);

			isp = MRISP[i] + ((MRISP[i - 1] - MRISP[i]) * delta);
			ThrustAdjust = MRThrust[i] + ((MRThrust[i - 1] - MRThrust[i]) * delta);

			return;
		}
	}
}

SIVB200Systems::SIVB200Systems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THRUSTER_HANDLE &lox, THGROUP_HANDLE &ver)
	: SIVBSystems(v, j2, j2prop, aps, ull, lox, ver)
{
}

void SIVB200Systems::RecalculateEngineParameters()
{
	double MixtureRatio;

	if (PUValveState == PUVALVE_CLOSED)
	{
		MixtureRatio = 5.5;
	}
	else if (PUValveState == PUVALVE_OPEN)
	{
		MixtureRatio = 4.5;
	}
	else
	{
		MixtureRatio = 5.0;
	}

	SetSIVBMixtureRatio(MixtureRatio);
}

void SIVB200Systems::SetSIVBMixtureRatio(double ratio)

{
	double isp, thrust;

	// Hardcoded ISP and thrust according to the the Apollo 7 Saturn IB Report, NTRS ID 19900067467
	if (ratio >= 5.25) {
		thrust = 1009902;
		isp = 424 * G;

	}
	else if (ratio >= 4.75){
		thrust = 889951.0;
		isp = 426 * G;
	}
	else {
		thrust = 770000.;
		isp = 428 * G;
	}

	//
	// For simplicity assume no ISP change at sea-level: SIVb stage should always
	// be in near-vacuum anyway.
	//

	vessel->SetThrusterIsp(j2engine, isp, isp);
	vessel->SetThrusterMax0(j2engine, thrust);
}

void SIVB200Systems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 9: //Engine Ignition Sequence Start
		EngineStartOn();
		break;
	case 10: //Engine Ready Bypass On
		EngineReadyBypass();
		break;
	case 12: //S-IVB Engine Cutoff No. 1 On
		LVDCEngineCutoff();
		break;
	case 13: //S-IVB Engine Cutoff No. 1 Off
		LVDCEngineCutoffOff();
		break;
	case 19: //Engine Ready Bypass On
		EngineReadyBypass();
		break;
	case 22: //LOX Chilldown Pump On
		break;
	case 23: //LOX Chilldown Pump Off
		break;
	case 28: //Aux Hydraulic Pump Flight Mode On
		AuxHydPumpFlightModeOn();
		break;
	case 29: //Aux Hydraulic Pump Flight Mode Off
		AuxHydPumpFlightModeOff();
		break;
	case 32: //P.U. Mixture Ratio 4.5 On
		SetPUValve(PUVALVE_OPEN);
		//SPUShiftS.play(NOLOOP, 255);
		//SPUShiftS.done();
		break;
	case 33: //P.U. Mixture Ratio 4.5 Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 34: //P.U. Mixture Ratio 5.5 On
		SetPUValve(PUVALVE_CLOSED);
		break;
	case 35: //P.U. Mixture Ratio 5.5 Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 43: //S-IVB Engine Cutoff No. 2 On
		LVDCEngineCutoff();
		break;
	case 49: //S-IVB Engine Cutoff No. 2 Off
		LVDCEngineCutoffOff();
		break;
	case 54: //Charge Ullage Ignition On
		break;
	case 55: //Charge Ullage Jettison On
		break;
	case 56: //Fire Ullage Ignition On
		FireUllageIgnitionOn();
		break;
	case 58: //Fuel Chilldown Pump On
		break;
	case 59: //Fuel Chilldown Pump Off
		break;
	case 79: //LOX Tank Flight Pressurization Shutoff Valves Close On
		EndLOXVenting();
		break;
	case 80: //LOX Tank Flight Pressurization Shutoff Valves Close Off
		StartLOXVenting();
		break;
	case 82: //Prevalves Close On
		PrevalvesCloseOn = true;
		break;
	case 83: //Prevalves Close Off
		PrevalvesCloseOn = false;
		break;
	case 85: //Passivation Enable
		break;
	case 97: //Point Level Sensor Arming
		PointLevelSensorArming();
		break;
	case 98: //Point Level Sensor Disarming
		PointLevelSensorDisarming();
		break;
	default:
		break;
	}
}

SIVB500Systems::SIVB500Systems(VESSEL *v, THRUSTER_HANDLE &j2, PROPELLANT_HANDLE &j2prop, THRUSTER_HANDLE *aps, THRUSTER_HANDLE *ull, THRUSTER_HANDLE &lox, THGROUP_HANDLE &ver)
	: SIVBSystems(v, j2, j2prop, aps, ull, lox, ver)
{
}

void SIVB500Systems::RecalculateEngineParameters()
{
	double MixtureRatio;

	if (PUValveState == PUVALVE_CLOSED)
	{
		MixtureRatio = 5.5;
	}
	else if (PUValveState == PUVALVE_OPEN)
	{
		MixtureRatio = 4.5;
	}
	else
	{
		MixtureRatio = 4.946;
	}

	SetSIVBMixtureRatio(MixtureRatio);
}

void SIVB500Systems::SetSIVBMixtureRatio(double ratio)

{
	double isp, thrust, ThrustAdjust;

	GetJ2ISP(ratio, isp, ThrustAdjust);
	thrust = J2DefaultThrust * ThrustAdjust;

	//
	// For simplicity assume no ISP change at sea-level: SIVb stage should always
	// be in near-vacuum anyway.
	//

	vessel->SetThrusterIsp(j2engine, isp, isp);
	vessel->SetThrusterMax0(j2engine, thrust);
}

void SIVB500Systems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 1: //Passivation Enable
		break;
	case 2: //Passivation Disable
		break;
	case 3: //LOX Tank Repressurization Control Valve Open On
		break;
	case 4: //LOX Tank Repressurization Control Valve Open Off
		break;
	case 5: //PU Activate On
		break;
	case 6: //PU Activate Off
		break;
	case 7: //PU Inverter and DC Power On
		break;
	case 8: //PU Inverter and DC Power Off
		break;
	case 9: //S-IVB Engine Start On
		EngineStartOn();
		break;
	case 10: //Engine Ready Bypass
		EngineReadyBypass();
		break;
	case 11: //Fuel Injection Temperature OK Bypass
		break;
	case 12: //S-IVB Engine Cutoff
		LVDCEngineCutoff();
		break;
	case 13: //S-IVB Engine Cutoff Off
		LVDCEngineCutoffOff();
		break;
	case 14: //Engine Mainstage Control Valve Open On
		StartLOXVenting();
		break;
	case 15: //Engine Mainstage Control Valve Open Off
		EndLOXVenting();
		break;
	case 16: //Fuel Injector Temperature OK Bypass Reset
		break;
	case 17: //PU Valve Hardover Position On
		SetPUValve(PUVALVE_OPEN);
		break;
	case 18: //PU Valve Hardover Position Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 19: //S-IVB Engine EDS Cutoff No. 2 Disable
		EDSCutoffDisable();
		break;
	case 20: //LOX Chilldown Pump Purge Control Valve Enable On
		break;
	case 21: //LOX Chilldown Pump Purge Control Valve Enable Off
		break;
	case 22: //LOX Chilldown Pump On
		break;
	case 23: //LOX Chilldown Pump Off
		break;
	case 24: //Engine Pump Purge Control Valve Enable On
		break;
	case 25: //Engine Pump Purge Control Valve Enable Off
		break;
	case 26: //Burner LH2 Propellant Valve Open On
		break;
	case 27: //S-IVB Engine Start Off
		EngineStartOff();
		break;
	case 28: //Aux Hydraulic Pump Flight Mode On
		AuxHydPumpFlightModeOn();
		break;
	case 29: //Aux Hydraulic Pump Flight Mode Off
		AuxHydPumpFlightModeOff();
		break;
	case 30: //Start Bottle Vent Control Valve Open On
		break;
	case 31: //Start Bottle Vent Control Valve Open Off
		break;
	case 32: //Second Burn Relay On
		SecondBurnRelayOn();
		break;
	case 33: //Second Burn Relay Off
		SecondBurnRelayOff();
		break;
	case 34: //PU Fuel Boil Off Bias Command On
		break;
	case 35: //PU Fuel Boil Off Bias Command Off
		break;
	case 36: //Repressurization System Mode Select On (Amb)
		break;
	case 37: //Repressurization System Mode Select Off (Amb)
		break;
	case 38: //LH2 Tank Vent Open Command On
		break;
	case 39: //LH2 Tank Repressurization Control Valve Open On
		break;
	case 40: //PCM Group System On
		break;
	case 41: //PCM Group System Off
		break;
	case 42: //S-IVB Ullage Engine No. 1 On
		APSUllageEngineOn(1);
		break;
	case 43: //S-IVB Ullage Engine No. 1 Off
		APSUllageEngineOff(1);
		break;
	case 44: //LOX Tank NPV Valve Latch Open On
		break;
	case 45: //LOX Tank NPV Valve Latch Open Off
		break;
	case 46: //Single Sideband FM Transmitter On
		break;
	case 47: //Single Sideband FM Transmitter Off
		break;
	case 48: //Inflight Calibration Mode On
		break;
	case 49: //Inflight Calibration Mode Off
		break;
	case 50: //Heat-Exchanger Bypass Valve Control Enable On
		break;
	case 51: //Heat-Exchanger Bypass Valve Control Enable Off
		break;
	case 52: //Measurement Transfer Mode Position "B"
		break;
	case 53: //Spare
		break;
	case 54: //Charge Ullage Ignition On
		break;
	case 55: //Charge Ullage Jettison On
		break;
	case 56: //Fire Ullage Ignition On
		FireUllageIgnitionOn();
		break;
	case 57: //Fire Ullage Jettison On
		break;
	case 58: //Fuel Chilldown Pump On
		break;
	case 59: //Fuel Chilldown Pump Off
		break;
	case 60: //Burner LH2 Propellant Valve Close On
		break;
	case 61: //Burner LH2 Propellant Valve Close Off
		break;
	case 62: //TM Calibrate On
		break;
	case 63: //TM Calibrate Off
		break;
	case 64: //LH2 Tank Latching Relief Valve Latch On
		break;
	case 65: //LH2 Tank Latching Relief Valve Latch Off
		break;
	case 66: //RF Assembly Power Command On
		break;
	case 67: //RF Assembly Power Command Off
		break;
	case 68: //First Burn Relay On
		FirstBurnRelayOn();
		break;
	case 69: //First Burn Relay Off
		FirstBurnRelayOff();
		break;
	case 70: //Burner Exciters On
		break;
	case 71: //Burner Exciters Off
		break;
	case 72: //Burner LH2 Propellant Valve Open Off
		break;
	case 73: //Ullage Firing Reset
		break;
	case 74: //Burner LOX Shutdown Valve Close On
		break;
	case 75: //Burner LOX Shutdown Valve Close Off
		break;
	case 76: //LH2 Tank Vent Open Command Off
		break;
	case 77: //LH2 Tank Vent and Latching Relief Valve Boost Close On
		break;
	case 78: //LH2 Tank Vent and Latching Relief Valve Boost Close Off
		break;
	case 79: //LOX Tank Pressurization Shutoff Valves Close
		break;
	case 80: //LOX Tank Pressurization Shutoff Valves Open
		break;
	case 81: //LH2 Tank Repressurization Control Valve Open Off
		break;
	case 82: //Prevalves Close On
		PrevalvesCloseOn = true;
		break;
	case 83: //Prevalves Close Off
		PrevalvesCloseOn = false;
		break;
	case 84: //LH2 Tank Continuous Vent Valve Close On
		LH2ContinuousVentValveCloseOn();
		break;
	case 85: //Burner Automatic Cutoff System Arm
		break;
	case 86: //Burner Automatic Cutoff System Disarm
		break;
	case 87: //LH2 Tank Continuous Vent Valve Close Off
		break;
	case 88: //Ullage Charging Reset
		break;
	case 89: //Burner LOX Shutdown Valve Open On
		break;
	case 90: //Burner LOX Shutdown Valve Open Off
		break;
	case 91: //Chilldown Shut-Off Pilot On
		break;
	case 92: //Chilldown Shut-Off Pilot Off
		break;
	case 93: //LOX Vent Open Command On
		break;
	case 94: //LOX Vent Open Command Off
		break;
	case 95: //LOX Tank Vent and NPV Valv Boost Close On
		break;
	case 96: //LOX Tank Vent and NPV Valv Boost Close Off
		break;
	case 97: //Point Level Sensor Arming
		PointLevelSensorArming();
		break;
	case 98: //Point Level Sensor Disarming
		PointLevelSensorDisarming();
		break;
	case 99: //LH2 Tank Latching Relief Valve Open On
		break;
	case 100: //LH2 Tank Latching Relief Valve Open Off
		break;
	case 101: //S-IVB Ullage Engine No. 2 On
		APSUllageEngineOn(2);
		break;
	case 102: //S-IVB Ullage Engine No. 2 Off
		APSUllageEngineOff(2);
		break;
	case 103: //LOX Tank Flight Pressure System On
		break;
	case 104: //LOX Tank Flight Pressure System Off
		break;
	case 105: //LOX Tank NPV Valve Open On
		break;
	case 106: //LOX Tank NPV Valve Open Off
		break;
	case 107: //LH2 Tank Continous Vent Relief Override Shutoff Valve Open On
		break;
	case 108: //LH2 Tank Continous Vent Relief Override Shutoff Valve Open Off
		break;
	case 109: //Engine He Control Valve Open On
		break;
	case 110: //Engine He Control Valve Open Off
		break;
	case 111: //LH2 Tank Continuous Vent Orifice Shutoff Valve Open On
		LH2ContinuousVentValveOpenOn();
		break;
	case 112: //LH2 Tank Continuous Vent Orifice Shutoff Valve Open Off
		break;
	default:
		break;
	}
}