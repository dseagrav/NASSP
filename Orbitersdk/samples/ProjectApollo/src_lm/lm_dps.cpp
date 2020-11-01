/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Descent Propulsion System

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
#include "toggleswitch.h"
#include "nasspdefs.h"
#include "papi.h"
#include "LEM.h"
#include "lm_dps.h"
#include "LM_DescentStageResource.h"

DPSValve::DPSValve() {
	isOpen = false;
}

void DPSValve::SetState(bool open) {
	isOpen = open;
}

void DPSValve::SwitchToggled(PanelSwitchItem *s) {

	if (s->SRC && (s->SRC->Voltage() > SP_MIN_DCVOLTAGE)) {
		if (((ThreePosSwitch *)s)->IsUp()) {
			SetState(true);
		}
		else if (((ThreePosSwitch *)s)->IsDown()) {
			SetState(false);
		}
	}
}

LEMPropellantSource::LEMPropellantSource(PROPELLANT_HANDLE &h) : source_prop(h)
{
	our_vessel = 0;
}

PROPELLANT_HANDLE LEMPropellantSource::Handle()
{
	return source_prop;
}

double LEMPropellantSource::Quantity()
{
	if (source_prop && our_vessel) {
		return our_vessel->GetPropellantMass(source_prop) / our_vessel->GetPropellantMaxMass(source_prop);
	}

	return 0.0;
}

DPSPropellantSource::DPSPropellantSource(PROPELLANT_HANDLE &ph, PanelSDK &p) :
	LEMPropellantSource(ph)
{
	propellantMassToDisplay = SPS_DEFAULT_PROPELLANT;
	propellantMaxMassToDisplay = DPS_DEFAULT_PROPELLANT;
	ambientHeliumPressurePSI = 0.0;
	supercriticalHeliumPressurePSI = 0.0;
	FuelTankUllagePressurePSI = 0.0;
	OxidTankUllagePressurePSI = 0.0;
	FuelEngineInletPressurePSI = 0.0;
	OxidEngineInletPressurePSI = 0.0;
	supercriticalHeliumMass = 22.0;	//48.5 pounds
	supercriticalHeliumTemp = 12.35186667;	//Kelvin, based on loading temp of -453°F, should give 400PSI at liftoff
	ambientHeliumMass = 0.4808079;	//1.06 lbm

	fuel1LevelLow = false;
	fuel2LevelLow = false;
	oxid1LevelLow = false;
	oxid2LevelLow = false;

	//Open by default
	PrimaryHeRegulatorShutoffValve.SetState(true);
	FuelVentValve2.SetState(true);
	OxidVentValve2.SetState(true);

	lem = NULL;
}

void DPSPropellantSource::Init(LEM *l, e_object *dc1)
{
	GaugingPower = dc1;
	lem = l;
}

void DPSPropellantSource::Timestep(double simt, double simdt)
{
	if (!our_vessel) return;

	double p, pmax;

	if(our_vessel->stage > 1 || !source_prop)
	{
		p = 0;
		pmax = 1;
		ambientHeliumPressurePSI = 0.0;
		supercriticalHeliumPressurePSI = 0.0;
		heliumRegulatorManifoldPressurePSI = 0.0;
		FuelTankUllagePressurePSI = 0.0;
		OxidTankUllagePressurePSI = 0.0;
		FuelEngineInletPressurePSI = 0.0;
		OxidEngineInletPressurePSI = 0.0;
	}
	else {
		p = our_vessel->GetPropellantMass(source_prop);
		pmax = DPS_DEFAULT_PROPELLANT;
		double pMaxForPressures = our_vessel->GetPropellantMaxMass(source_prop);

		ambientHeliumPressurePSI = 3327.732344*ambientHeliumMass;

		supercriticalHeliumTemp += 7.5352301587e-5*simdt;
		supercriticalHeliumPressurePSI = 1.471989512*supercriticalHeliumMass*supercriticalHeliumTemp;

		double InletPressure1, InletPressure2;
		InletPressure1 = InletPressure2 = 0.0;

		//Primary Helium Regulator Inlet
		if (PrimaryHeRegulatorShutoffValve.IsOpen() && SupercritHeIsolValve.IsOpen())
		{
			InletPressure1 = supercriticalHeliumPressurePSI;
		}
		else
		{
			InletPressure1 = 0.0;
		}

		//Secondary Helium Regulator Inlet
		InletPressure2 = 0.0;

		if (AmbientHeIsolValve.IsOpen())
		{
			InletPressure2 = ambientHeliumPressurePSI;
		}
		if (SecondaryHeRegulatorShutoffValve.IsOpen() && SupercritHeIsolValve.IsOpen())
		{
			if (supercriticalHeliumPressurePSI > InletPressure2)
			{
				InletPressure2 = supercriticalHeliumPressurePSI;
			}
		}

		//Helium Regulator Outlet
		if (InletPressure1 > 245.0 || InletPressure2 > 245.0)
		{
			heliumRegulatorManifoldPressurePSI = 245.0;
		}
		else if (InletPressure1 > InletPressure2)
		{
			heliumRegulatorManifoldPressurePSI = InletPressure1;
		}
		else
		{
			heliumRegulatorManifoldPressurePSI = InletPressure2;
		}

		//Fuel Tank
		if (FuelCompatibilityValve.IsOpen() && heliumRegulatorManifoldPressurePSI - 8.0 > 101.0*p / pMaxForPressures)
		{
			FuelTankUllagePressurePSI = heliumRegulatorManifoldPressurePSI - 8.0;
		}
		else
		{
			FuelTankUllagePressurePSI = 101.0*p / pMaxForPressures;
		}

		//Oxidizer Tank
		if (OxidCompatibilityValve.IsOpen() && heliumRegulatorManifoldPressurePSI - 8.0 > 144.0*p / pMaxForPressures)
		{
			OxidTankUllagePressurePSI = heliumRegulatorManifoldPressurePSI - 8.0;
		}
		else
		{
			OxidTankUllagePressurePSI = 144.0*p / pMaxForPressures;
		}

		FuelEngineInletPressurePSI = FuelTankUllagePressurePSI - 15.0;
		OxidEngineInletPressurePSI = OxidTankUllagePressurePSI - 15.0;

		//Propellant Venting
		if (OxidVentValve1.IsOpen() && OxidVentValve2.IsOpen())
		{
			double dMass = 0.01*p*simdt;
			p -= dMass;
			our_vessel->SetPropellantMass(source_prop, p);

			if (AmbientHeIsolValve.IsOpen() && OxidCompatibilityValve.IsOpen())
			{
				dMass = 0.01*ambientHeliumMass*simdt;
				ambientHeliumMass -= dMass;
			}
		}
		if (FuelVentValve1.IsOpen() && FuelVentValve2.IsOpen())
		{
			double dMass = 0.01*p*simdt;
			p -= dMass;
			our_vessel->SetPropellantMass(source_prop, p);

			if (AmbientHeIsolValve.IsOpen() && FuelCompatibilityValve.IsOpen())
			{
				dMass = 0.01*ambientHeliumMass*simdt;
				ambientHeliumMass -= dMass;
			}
		}

		//Ambient Helium Isolation Valve
		if (!AmbientHeIsolValve.IsOpen() && our_vessel->DescentEngineStartPyros.Blown())
		{
			AmbientHeIsolValve.SetState(true);
		}

		//Supercritical Helium Isolation Valve
		if (!SupercritHeIsolValve.IsOpen() && our_vessel->DescentEngineOnPyros.Blown())
		{
			SupercritHeIsolValve.SetState(true);
		}

		//Propellant Compatibility Valves
		if (!OxidCompatibilityValve.IsOpen() && our_vessel->DescentPropIsolPyros.Blown())
		{
			OxidCompatibilityValve.SetState(true);
		}

		if (!FuelCompatibilityValve.IsOpen() && our_vessel->DescentPropIsolPyros.Blown())
		{
			FuelCompatibilityValve.SetState(true);
		}

		//Vent Valves
		if (!OxidVentValve1.IsOpen() && our_vessel->DescentPropVentPyros.Blown())
		{
			OxidVentValve1.SetState(true);
		}

		if (!FuelVentValve1.IsOpen() && our_vessel->DescentPropVentPyros.Blown())
		{
			FuelVentValve1.SetState(true);
		}
	}

	// Propellant Quantity Gauging Control Unit
	if (IsGaugingPowered()) {
		
		if (our_vessel->QTYMonSwitch.IsCenter())
		{
			propellantMassToDisplay = p;
		}
		else
		{
			propellantMassToDisplay = p;
		}

		//Propellant Low
		if (propellantMassToDisplay / propellantMaxMassToDisplay < 0.056)
		{
			fuel1LevelLow = true;
			fuel2LevelLow = true;
			oxid1LevelLow = true;
			oxid2LevelLow = true;
		}
		else
		{
			fuel1LevelLow = false;
			fuel2LevelLow = false;
			oxid1LevelLow = false;
			oxid2LevelLow = false;
		}
	}
	else
	{
		propellantMassToDisplay = 0.0;

		fuel1LevelLow = false;
		fuel2LevelLow = false;
		oxid1LevelLow = false;
		oxid2LevelLow = false;
	}
}

void DPSPropellantSource::SystemTimestep(double simdt)
{
	if (our_vessel->stage > 1) return;

	if (GaugingPower && IsGaugingPowered())
		GaugingPower->DrawPower(14.6);
}

double DPSPropellantSource::GetFuelPercent()
{
	if (propellantMassToDisplay / propellantMaxMassToDisplay < 0.95)
		return propellantMassToDisplay / propellantMaxMassToDisplay;

	return 0.95;
}

double DPSPropellantSource::GetOxidPercent()
{
	if (propellantMassToDisplay / propellantMaxMassToDisplay < 0.95)
		return propellantMassToDisplay / propellantMaxMassToDisplay;

	return 0.95;
}

double DPSPropellantSource::GetAmbientHeliumPressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return ambientHeliumPressurePSI;

	return 0.0;
}

double DPSPropellantSource::GetSupercriticalHeliumPressPSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return supercriticalHeliumPressurePSI;

	return 0.0;
}

double DPSPropellantSource::GetHeliumRegulatorManifoldPressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return heliumRegulatorManifoldPressurePSI;

	return 0.0;
}

double DPSPropellantSource::GetFuelTankUllagePressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return FuelTankUllagePressurePSI;

	return 0.0;
}

double DPSPropellantSource::GetOxidizerTankUllagePressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return OxidTankUllagePressurePSI;

	return 0.0;
}

double DPSPropellantSource::GetFuelEngineInletPressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return FuelEngineInletPressurePSI;

	return 0.0;
}

double DPSPropellantSource::GetOxidizerEngineInletPressurePSI()
{
	if (our_vessel->INST_SIG_SENSOR_CB.IsPowered())
		return OxidEngineInletPressurePSI;

	return 0.0;
}

bool DPSPropellantSource::IsGaugingPowered() {

	if (GaugingPower->Voltage() < SP_MIN_DCVOLTAGE) return false;

	if (our_vessel->QTYMonSwitch.IsDown()) return false;

	return true;
}

double DPSPropellantSource::GetOxidizerTank1BulkTempF()
{
	return 70.0;
}

double DPSPropellantSource::GetOxidizerTank2BulkTempF()
{
	return 70.0;
}

double DPSPropellantSource::GetFuelTank1BulkTempF()
{
	return 70.0;
}

double DPSPropellantSource::GetFuelTank2BulkTempF()
{
	return 70.0;
}

bool DPSPropellantSource::PropellantLevelLow()
{
	if (lem->stage < 2 && IsGaugingPowered() && (fuel1LevelLow || fuel2LevelLow || oxid1LevelLow || oxid2LevelLow))
		return true;

	return false;
}

void DPSPropellantSource::SaveState(FILEHANDLE scn)
{
	oapiWriteLine(scn, DPSPROPELLANT_START_STRING);

	papiWriteScenario_bool(scn, "FUEL1LEVELLOW", fuel1LevelLow);
	papiWriteScenario_bool(scn, "FUEL2LEVELLOW", fuel2LevelLow);
	papiWriteScenario_bool(scn, "OXID1LEVELLOW", oxid1LevelLow);
	papiWriteScenario_bool(scn, "OXID2LEVELLOW", oxid2LevelLow);
	papiWriteScenario_double(scn, "PROPELLANTMASSTODISPLAY", propellantMassToDisplay);
	papiWriteScenario_double(scn, "PROPELLANTMAXMASSTODISPLAY", propellantMaxMassToDisplay);
	papiWriteScenario_double(scn, "AMBIENTHELIUMPRESSUREPSI", ambientHeliumPressurePSI);
	papiWriteScenario_double(scn, "SUPERCRITICALHELIUMPRESSUREPSI", supercriticalHeliumPressurePSI);
	papiWriteScenario_double(scn, "HELIUMREGULATORMANIFOLDPRESSUREPSI", heliumRegulatorManifoldPressurePSI);
	papiWriteScenario_double(scn, "SUPERCRITICALHELIUMMASS", supercriticalHeliumMass);
	papiWriteScenario_double(scn, "SUPERCRITICALHELIUMTEMP", supercriticalHeliumTemp);
	papiWriteScenario_double(scn, "AMBIENTHELIUMMASS", ambientHeliumMass);

	papiWriteScenario_bool(scn, "PRIMREGHELIUMVALVE_ISOPEN", PrimaryHeRegulatorShutoffValve.IsOpen());
	papiWriteScenario_bool(scn, "SECREGHELIUMVALVE_ISOPEN", SecondaryHeRegulatorShutoffValve.IsOpen());
	papiWriteScenario_bool(scn, "AMBIENTHELIUMISOLVALVE_ISOPEN", AmbientHeIsolValve.IsOpen());
	papiWriteScenario_bool(scn, "SUPERCRITHELIUMISOLVALVE_ISOPEN", SupercritHeIsolValve.IsOpen());
	papiWriteScenario_bool(scn, "FUELVENTVALVE1_ISOPEN", FuelVentValve1.IsOpen());
	papiWriteScenario_bool(scn, "FUELVENTVALVE2_ISOPEN", FuelVentValve2.IsOpen());
	papiWriteScenario_bool(scn, "OXIDVENTVALVE1_ISOPEN", OxidVentValve1.IsOpen());
	papiWriteScenario_bool(scn, "OXIDVENTVALVE2_ISOPEN", OxidVentValve2.IsOpen());
	papiWriteScenario_bool(scn, "FUELCOMPATIBILITYVALVE_ISOPEN", FuelCompatibilityValve.IsOpen());
	papiWriteScenario_bool(scn, "OXIDCOMPATIBILITYVALVE_ISOPEN", OxidCompatibilityValve.IsOpen());

	oapiWriteLine(scn, DPSPROPELLANT_END_STRING);
}

void DPSPropellantSource::LoadState(FILEHANDLE scn)
{
	char *line;
	bool isOpen;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, DPSPROPELLANT_END_STRING, sizeof(DPSPROPELLANT_END_STRING))) {
			return;
		}

		papiReadScenario_bool(line, "FUEL1LEVELLOW", fuel1LevelLow);
		papiReadScenario_bool(line, "FUEL2LEVELLOW", fuel2LevelLow);
		papiReadScenario_bool(line, "OXID1LEVELLOW", oxid1LevelLow);
		papiReadScenario_bool(line, "OXID2LEVELLOW", oxid2LevelLow);
		papiReadScenario_double(line, "PROPELLANTMASSTODISPLAY", propellantMassToDisplay);
		papiReadScenario_double(line, "PROPELLANTMAXMASSTODISPLAY", propellantMaxMassToDisplay);
		papiReadScenario_double(line, "AMBIENTHELIUMPRESSUREPSI", ambientHeliumPressurePSI);
		papiReadScenario_double(line, "SUPERCRITICALHELIUMPRESSUREPSI", supercriticalHeliumPressurePSI);
		papiReadScenario_double(line, "HELIUMREGULATORMANIFOLDPRESSUREPSI", heliumRegulatorManifoldPressurePSI);
		papiReadScenario_double(line, "SUPERCRITICALHELIUMMASS", supercriticalHeliumMass);
		papiReadScenario_double(line, "SUPERCRITICALHELIUMTEMP", supercriticalHeliumTemp);
		papiReadScenario_double(line, "AMBIENTHELIUMMASS", ambientHeliumMass);

		if (papiReadScenario_bool(line, "PRIMREGHELIUMVALVE_ISOPEN", isOpen))			PrimaryHeRegulatorShutoffValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "SECREGHELIUMVALVE_ISOPEN", isOpen))			SecondaryHeRegulatorShutoffValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "AMBIENTHELIUMISOLVALVE_ISOPEN", isOpen))		AmbientHeIsolValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "SUPERCRITHELIUMISOLVALVE_ISOPEN", isOpen))		SupercritHeIsolValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "FUELVENTVALVE1_ISOPEN", isOpen))				FuelVentValve1.SetState(isOpen);
		if (papiReadScenario_bool(line, "FUELVENTVALVE2_ISOPEN", isOpen))				FuelVentValve2.SetState(isOpen);
		if (papiReadScenario_bool(line, "OXIDVENTVALVE1_ISOPEN", isOpen))				OxidVentValve1.SetState(isOpen);
		if (papiReadScenario_bool(line, "OXIDVENTVALVE2_ISOPEN", isOpen))				OxidVentValve2.SetState(isOpen);
		if (papiReadScenario_bool(line, "FUELCOMPATIBILITYVALVE_ISOPEN", isOpen))		FuelCompatibilityValve.SetState(isOpen);
		if (papiReadScenario_bool(line, "OXIDCOMPATIBILITYVALVE_ISOPEN", isOpen))		OxidCompatibilityValve.SetState(isOpen);
	}
}

// Descent Propulsion System
LEM_DPS::LEM_DPS(THRUSTER_HANDLE *dps) :
	dpsThruster(dps) {
	lem = NULL;
	thrustOn = 0;
	engPreValvesArm = 0;
	engArm = 0;
	thrustcommand = 0;
	ThrustChamberPressurePSI = 0.0;
	ActuatorValves = 0.0;
	PropellantShutoffValves = 0.0;

	anim_DPSGimbalPitch = -1;
	anim_DPSGimbalRoll = -1;

	dpsgimbal_proc[0] = 0.0;
	dpsgimbal_proc[1] = 0.0;
	dpsgimbal_proc_last[0] = 0.0;
	dpsgimbal_proc_last[1] = 0.0;

	Erosion = 0.0;
}

void LEM_DPS::Init(LEM *s) {
	lem = s;
}

void LEM_DPS::ThrottleActuator(double manthrust, double autothrust)
{
	if (engArm)
	{
		thrustcommand = manthrust + autothrust;

		if (thrustcommand > 1.0)
		{
			thrustcommand = 1.0;
		}
		else if (thrustcommand < 0.1)
		{
			thrustcommand = 0.1;
		}
	}
	else
	{
		//Without power, the throttle will be fully open
		thrustcommand = 1.0;
	}
}

void LEM_DPS::DefineAnimations(UINT idx) {

	// DPS Gimbal Animation Definition
	ANIMATIONCOMPONENT_HANDLE ach_DPSGimbalPitch, ach_DPSGimbalRoll;
	const VECTOR3 LM_DPS_PIVOT = { -0.000818, -0.383743, 0.001107 }; // Pivot Point
	static UINT meshgroup_DPSBell = DS_GRP_DPSbell;
	static MGROUP_ROTATE mgt_Gimbal_Pitch(idx, &meshgroup_DPSBell, 1, LM_DPS_PIVOT, _V(1, 0, 0), (float)PI2);
	static MGROUP_ROTATE mgt_Gimbal_Roll(idx, &meshgroup_DPSBell, 1, LM_DPS_PIVOT, _V(0, 0, 1), (float)PI2);
	anim_DPSGimbalPitch = lem->CreateAnimation(0.0);
	anim_DPSGimbalRoll = lem->CreateAnimation(0.0);
	ach_DPSGimbalPitch = lem->AddAnimationComponent(anim_DPSGimbalPitch, 0.0f, 1.0f, &mgt_Gimbal_Pitch);
	ach_DPSGimbalRoll = lem->AddAnimationComponent(anim_DPSGimbalRoll, 0.0f, 1.0f, &mgt_Gimbal_Roll);

	// Get current DPS gimbal state for animation
	dpsgimbal_proc[0] = pitchGimbalActuator.GetPosition() / 360;
	if (dpsgimbal_proc[0] < 0) dpsgimbal_proc[0] += 1.0;
	dpsgimbal_proc[1] = rollGimbalActuator.GetPosition() / 360;
	if (dpsgimbal_proc[1] < 0) dpsgimbal_proc[1] += 1.0;
	lem->SetAnimation(anim_DPSGimbalPitch, dpsgimbal_proc[0]); lem->SetAnimation(anim_DPSGimbalRoll, dpsgimbal_proc[1]);
}

void LEM_DPS::DeleteAnimations() {

	if (anim_DPSGimbalPitch != -1) lem->DelAnimation(anim_DPSGimbalPitch);
	anim_DPSGimbalPitch = -1;
	if (anim_DPSGimbalRoll != -1) lem->DelAnimation(anim_DPSGimbalRoll);
	anim_DPSGimbalRoll = -1;
}

void LEM_DPS::Timestep(double simt, double simdt) {

	if (lem == NULL) { return; }
	if (lem->stage > 1) { return; }

	// Animate DPS Gimbals
	dpsgimbal_proc[0] = pitchGimbalActuator.GetPosition() / 360;
	if (dpsgimbal_proc[0] < 0) dpsgimbal_proc[0] += 1.0;
	dpsgimbal_proc[1] = rollGimbalActuator.GetPosition() / 360;
	if (dpsgimbal_proc[1] < 0) dpsgimbal_proc[1] += 1.0;
	if (dpsgimbal_proc[0] - dpsgimbal_proc_last[0] != 0.0) lem->SetAnimation(anim_DPSGimbalPitch, dpsgimbal_proc[0]);
	if (dpsgimbal_proc[1] - dpsgimbal_proc_last[1] != 0.0) lem->SetAnimation(anim_DPSGimbalRoll, dpsgimbal_proc[1]);
	dpsgimbal_proc_last[0] = dpsgimbal_proc[0];
	dpsgimbal_proc_last[1] = dpsgimbal_proc[1];

	if ((lem->SCS_DECA_PWR_CB.IsPowered() && lem->deca.GetK10()) || (lem->SCS_DES_ENG_OVRD_CB.IsPowered() && lem->scca3.GetK5()))
	{
		engPreValvesArm = true;
	}
	else
	{
		engPreValvesArm = false;
	}

	if (lem->SCS_DECA_PWR_CB.IsPowered() && (lem->deca.GetK1() || lem->deca.GetK23()))
	{
		engArm = true;
	}
	else
	{
		engArm = false;
	}

	if (lem->deca.GetThrustOn() || (lem->SCS_DES_ENG_OVRD_CB.IsPowered() && lem->scca3.GetK5()))
	{
		thrustOn = true;
	}
	else
	{
		thrustOn = false;
	}

	if (thrustOn)
	{
		double ActuatorPressure = lem->GetDPSPropellant()->GetActuatorValvesPressurePSI();
		if (ActuatorPressure > 110.0)
		{
			ActuatorValves = 1.0;
		}
		else if (ActuatorPressure < 45.0)
		{
			ActuatorValves = 0.0;
		}
		else
		{
			ActuatorValves = (ActuatorPressure - 45.0)*1.0 / (110.0 - 45.0);
		}
	}
	else
	{
		ActuatorValves = 0.0;
	}

	if (dpsThruster[0])
	{
		//Engine Fire Command
		if (engPreValvesArm && thrustOn)
		{
			if (PropellantShutoffValves < ActuatorValves)
			{
				//Flight experience from Apollo 9, about 2.1 seconds from 0 to full
				PropellantShutoffValves += 1.0 / 2.1*simdt;
			}
			//This prevents overshooting from the code above and accounts for the actuator valves slowly closing when the actuator pressure drops
			if (PropellantShutoffValves > ActuatorValves)
			{
				PropellantShutoffValves = ActuatorValves;
			}
		}
		else
		{
			if (PropellantShutoffValves > 0)
			{
				// 38cs cutoff delay at 100% thrust in GSOP. To reach the same impulse, linear thrust decay over twice that time.
				PropellantShutoffValves -= 1.0 / 0.76*simdt;
				if (PropellantShutoffValves < 0) PropellantShutoffValves = 0.0;
			}
		}

		lem->SetThrusterLevel(dpsThruster[0], thrustcommand*PropellantShutoffValves);

		//103.4PSI at FTP (uneroded)
		ThrustChamberPressurePSI = lem->GetThrusterMax(dpsThruster[0])*lem->GetThrusterLevel(dpsThruster[0])*103.4 / DPS_FCMAX;
	}
	else
	{
		ThrustChamberPressurePSI = 0.0;
	}

	// Do GDA time steps
	pitchGimbalActuator.Timestep(simt, simdt);
	rollGimbalActuator.Timestep(simt, simdt);

	VECTOR3 dpsvector;

	if (lem->stage < 2 && dpsThruster[0]) {
		// Directions X,Y,Z
		dpsvector.x = -rollGimbalActuator.GetPosition() * RAD; // Convert deg to rad
		dpsvector.z = pitchGimbalActuator.GetPosition() * RAD;
		dpsvector.y = 1;
		lem->SetThrusterDir(dpsThruster[0], dpsvector);

		//Erosion effects and thruster parameter calculation
		if (lem->GetThrusterLevel(dpsThruster[0]) > 0.0)
		{
			//2.525 is rate of change of thrust (in Newtons per second) at full thrust command to simulation erosion effects
			//TBD: Effects at lower throttle settings, scaled linearly with thrust command for now
			Erosion += lem->GetThrusterLevel(dpsThruster[0])*2.525*simdt;
			//Set base thrust (FCMAX) plus erosion effect
			lem->SetThrusterMax0(dpsThruster[0], DPS_FCMAX + Erosion);
			//Recalculate and set ISP
			lem->SetThrusterIsp(dpsThruster[0], RecalculateISP((DPS_FCMAX + Erosion) / DPS_FMAX)*9.80665);
		}

		//sprintf(oapiDebugString(), "Start: %d, Stop: %d Lever: %f Throttle Cmd: %f engPreValvesArm %d engArm %d thrustOn: %d", lem->ManualEngineStart.GetState(), lem->CDRManualEngineStop.GetState(), lem->ttca_throttle_pos_dig, thrustcommand, engPreValvesArm, engArm, thrustOn);
		//sprintf(oapiDebugString(), "DPS %d rollc: %d, roll: %f° pitchc: %d, pitch: %f°", thrustOn, rollGimbalActuator.GetCmdPosition(), rollGimbalActuator.GetPosition(), pitchGimbalActuator.GetCmdPosition(), pitchGimbalActuator.GetPosition());
		//sprintf(oapiDebugString(), "lvl: %f Max Thr: %f Isp: %f Erosion %f", lem->GetThrusterLevel(dpsThruster[0]), lem->GetThrusterMax0(dpsThruster[0]), lem->GetThrusterIsp(dpsThruster[0]), Erosion);
	}
}

void LEM_DPS::SystemTimestep(double simdt) {
	pitchGimbalActuator.SystemTimestep(simdt);
	rollGimbalActuator.SystemTimestep(simdt);
}

double LEM_DPS::GetPitchGimbalPosition()
{
	if (lem->DECA_GMBL_AC_CB.IsPowered() && lem->deca.GetK25())
		return pitchGimbalActuator.GetPosition();

	return 0.0;
}

double LEM_DPS::GetRollGimbalPosition()
{
	if (lem->DECA_GMBL_AC_CB.IsPowered() && lem->deca.GetK25())
		return rollGimbalActuator.GetPosition();

	return 0.0;
}

double LEM_DPS::GetThrustChamberPressurePSI()
{
	if (lem->stage > 1 || !lem->INST_SIG_SENSOR_CB.IsPowered()) return 0.0;

	return ThrustChamberPressurePSI;
}

double LEM_DPS::GetInjectorActuatorPosition()
{
	if (lem->stage > 1) return 0.0;

	return ActuatorValves;
}

double LEM_DPS::RecalculateISP(double THRUST_FMAX)
{
	//Numbers from GSOP R-567 Section 6, except below 11.4% thrust, which just needed to not go below zero ISP
	if (THRUST_FMAX >= 0.925) return (488.0 - 200.0*THRUST_FMAX);
	else if (THRUST_FMAX >= 0.401) return (288.8778626 + 15.26717557*THRUST_FMAX);
	else if (THRUST_FMAX >= 0.252) return (291.2322148 + 9.395973154*THRUST_FMAX);
	else if (THRUST_FMAX >= 0.115) return (291.9445255 + 6.569343066*THRUST_FMAX);
	else if (THRUST_FMAX >= 0.05) return (-52.3 + 3000.0*THRUST_FMAX);
	else return (97.7);
}

void LEM_DPS::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);
	oapiWriteScenario_int(scn, "THRUSTON", (thrustOn ? 1 : 0));
	oapiWriteScenario_int(scn, "ENGPREVALVESARM", (engPreValvesArm ? 1 : 0));
	oapiWriteScenario_int(scn, "ENGARM", (engArm ? 1 : 0));
	papiWriteScenario_double(scn, "EROSION", Erosion);
	papiWriteScenario_double(scn, "PROPSHUTOFFVALVES", PropellantShutoffValves);
	oapiWriteLine(scn, end_str);
}

void LEM_DPS::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len))
			return;

		papiReadScenario_bool(line, "THRUSTON", thrustOn);
		papiReadScenario_bool(line, "ENGPREVALVESARM", engPreValvesArm);
		papiReadScenario_bool(line, "ENGARM", engArm);
		papiReadScenario_double(line, "EROSION", Erosion);
		papiReadScenario_double(line, "PROPSHUTOFFVALVES", PropellantShutoffValves);
	}
}

DPSGimbalActuator::DPSGimbalActuator() {

	position = 0;
	commandedPosition = 0;
	motorRunning = false;
	lem = 0;
	gimbalMotorSwitch = 0;
	motorSource = 0;
	gimbalfail = false;
}

DPSGimbalActuator::~DPSGimbalActuator() {
	// Nothing for now.
}

void DPSGimbalActuator::Init(LEM *s, AGCIOSwitch *m1Switch, e_object *m1Source) {

	lem = s;
	gimbalMotorSwitch = m1Switch;
	motorSource = m1Source;
}

void DPSGimbalActuator::Timestep(double simt, double simdt) {

	if (lem == NULL) { return; }

	// After staging
	if (lem->stage > 1) {
		position = 0;
		return;
	}

	//
	// Motors
	//

	if (motorRunning) {
		if (gimbalMotorSwitch->IsDown() || motorSource->Voltage() < SP_MIN_ACVOLTAGE) {
			motorRunning = false;
		}
	}
	else {
		if (gimbalMotorSwitch->IsUp() && motorSource->Voltage() > SP_MIN_ACVOLTAGE) {
			//if (motorStartSource->Voltage() > SP_MIN_ACVOLTAGE) {
			motorRunning = true;
			//	}
		}
	}

	//sprintf(oapiDebugString(), "Motor %d %f %d", gimbalMotorSwitch->IsUp(), motorSource->Voltage(), motorRunning);

	//
	// Process commanded position
	//

	if (IsSystemPowered() && motorRunning) {
		//position = commandedPosition; // Instant positioning
		GimbalTimestep(simdt);
	}

	gimbalfail = false;

	// Only 6.0 degrees of travel allowed.
	if (position > 6.0) { position = 6.0; gimbalfail = true; }
	if (position < -6.0) {position = -6.0; gimbalfail = true; }


	//sprintf(oapiDebugString(), "position %.3f commandedPosition %d lgcPosition %d", position, commandedPosition, lgcPosition);
}

void DPSGimbalActuator::GimbalTimestep(double simdt)
{
	double LMR, dpos;

	LMR = 0.2;	//0.2°/s maximum gimbal speed

	dpos = (double)commandedPosition*LMR*simdt;

	position += dpos;
}

void DPSGimbalActuator::SystemTimestep(double simdt) {

	if (lem->stage > 1) return;

	if (IsSystemPowered() && motorRunning) {
		DrawSystemPower();
	}
}

bool DPSGimbalActuator::IsSystemPowered() {

	if (gimbalMotorSwitch->IsUp())
	{
		if (motorSource->Voltage() > SP_MIN_ACVOLTAGE)
		{
			return true;
		}
	}
	return false;
}


void DPSGimbalActuator::DrawSystemPower() {

	if (gimbalMotorSwitch->IsUp())
	{
		motorSource->DrawPower(17.5);	/// \todo apparently 35 Watts for both actuators
	}
}

void DPSGimbalActuator::ChangeCmdPosition(int pos) {

	commandedPosition = pos;
}

void DPSGimbalActuator::SaveState(FILEHANDLE scn) {

	// START_STRING is written in LEM
	papiWriteScenario_double(scn, "POSITION", position);
	oapiWriteScenario_int(scn, "COMMANDEDPOSITION", commandedPosition);
	oapiWriteScenario_int(scn, "MOTORRUNNING", (motorRunning ? 1 : 0));

	oapiWriteLine(scn, "DPSGIMBALACTUATOR_END");
}

void DPSGimbalActuator::LoadState(FILEHANDLE scn) {

	char *line;
	int i;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "DPSGIMBALACTUATOR_END", sizeof("DPSGIMBALACTUATOR_END"))) {
			return;
		}

		if (!strnicmp(line, "POSITION", 8)) {
			sscanf(line + 8, "%lf", &position);
		}
		else if (!strnicmp(line, "COMMANDEDPOSITION", 17)) {
			sscanf(line + 17, "%d", &commandedPosition);
		}
		else if (!strnicmp(line, "MOTORRUNNING", 13)) {
			sscanf(line + 13, "%d", &i);
			motorRunning = (i != 0);
		}
	}
}