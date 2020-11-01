/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

S-II Systems

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

#include "siisystems.h"

SIISystems::SIISystems(VESSEL *v, THRUSTER_HANDLE *j2, PROPELLANT_HANDLE &j2prop, THGROUP_HANDLE &ull, Pyro &SII_Inter, Pyro &SII_SIVB_Sep, Sound &pushifts, Sound &SepS)
	:main_propellant(j2prop), ullage(ull), puShiftSound(pushifts), sepSound(SepS),
	j2engine1(v, j2[0]),
	j2engine2(v, j2[1]),
	j2engine3(v, j2[2]),
	j2engine4(v, j2[3]),
	j2engine5(v, j2[4]),
	SII_Interstage_Pyros(SII_Inter),
	SII_SIVB_Separation_Pyros(SII_SIVB_Sep)
{
	int i;

	vessel = v;

	for (i = 0;i < 5;i++)
	{
		ThrustOK[i] = false;
		EarlySIICutoff[i] = false;
		SecondStageFailureTime[i] = 0.0;
	}

	PUValveState = PUVALVE_NULL;

	PointLevelSensorArmed = false;
	UllageTrigger = false;
	PropellantDepletionSensors = false;
	OrdnanceArmed = false;
	SIISIVBOrdnanceArmed = false;
	StartPhaseLimiterCutoffArm = false;
	LH2StepPressurization = false;

	FailureTimer = 0.0;
	J2DefaultThrust = 0.0;

	LH2TankUllagePressurePSI = 50.0;

	j2engines[0] = &j2engine1;
	j2engines[1] = &j2engine2;
	j2engines[2] = &j2engine3;
	j2engines[3] = &j2engine4;
	j2engines[4] = &j2engine5;
}

void SIISystems::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, SIISYSTEMS_START_STRING);

	papiWriteScenario_bool(scn, "ULLAGETRIGGER", UllageTrigger);
	papiWriteScenario_bool(scn, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
	papiWriteScenario_bool(scn, "ORDNANCEARMED", OrdnanceArmed);
	papiWriteScenario_bool(scn, "SIISIVBORDNANCEARMED", SIISIVBOrdnanceArmed);
	papiWriteScenario_bool(scn, "STARTPHASELIMITERCUTOFFARM", StartPhaseLimiterCutoffArm);
	papiWriteScenario_bool(scn, "LH2STEPPRESSURIZATION", LH2StepPressurization);
	oapiWriteScenario_int(scn, "PUVALVESTATE", PUValveState);
	papiWriteScenario_double(scn, "J2DEFAULTTHRUST", J2DefaultThrust);

	j2engine1.SaveState(scn, "ENGINE1_BEGIN", "ENGINE_END");
	j2engine2.SaveState(scn, "ENGINE2_BEGIN", "ENGINE_END");
	j2engine3.SaveState(scn, "ENGINE3_BEGIN", "ENGINE_END");
	j2engine4.SaveState(scn, "ENGINE4_BEGIN", "ENGINE_END");
	j2engine5.SaveState(scn, "ENGINE5_BEGIN", "ENGINE_END");

	oapiWriteLine(scn, SIISYSTEMS_END_STRING);
}

void SIISystems::LoadState(FILEHANDLE scn) {
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, SIISYSTEMS_END_STRING, sizeof(SIISYSTEMS_END_STRING)))
			return;

		papiReadScenario_bool(line, "ULLAGETRIGGER", UllageTrigger);
		papiReadScenario_bool(line, "POINTLEVELSENSORARMED", PointLevelSensorArmed);
		papiReadScenario_bool(line, "ORDNANCEARMED", OrdnanceArmed);
		papiReadScenario_bool(line, "SIISIVBORDNANCEARMED", SIISIVBOrdnanceArmed);
		papiReadScenario_bool(line, "STARTPHASELIMITERCUTOFFARM", StartPhaseLimiterCutoffArm);
		papiReadScenario_bool(line, "LH2STEPPRESSURIZATION", LH2StepPressurization);
		papiReadScenario_int(line, "PUVALVESTATE", PUValveState);
		papiReadScenario_double(line, "J2DEFAULTTHRUST", J2DefaultThrust);

		if (!strnicmp(line, "ENGINE1_BEGIN", sizeof("ENGINE1_BEGIN"))) {
			j2engine1.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE2_BEGIN", sizeof("ENGINE2_BEGIN"))) {
			j2engine2.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE3_BEGIN", sizeof("ENGINE3_BEGIN"))) {
			j2engine3.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE4_BEGIN", sizeof("ENGINE4_BEGIN"))) {
			j2engine4.LoadState(scn, "ENGINE_END");
		}
		else if (!strnicmp(line, "ENGINE5_BEGIN", sizeof("ENGINE5_BEGIN"))) {
			j2engine5.LoadState(scn, "ENGINE_END");
		}
	}
}

void SIISystems::Timestep(double simdt)
{
	if (j2engines[0] == NULL) return;

	j2engine1.Timestep(simdt);
	j2engine2.Timestep(simdt);
	j2engine3.Timestep(simdt);
	j2engine4.Timestep(simdt);
	j2engine5.Timestep(simdt);

	//Propellant systems
	if (main_propellant)
	{
		LH2TankUllagePressurePSI = vessel->GetPropellantMass(main_propellant) / vessel->GetPropellantMaxMass(main_propellant)*50.0;
	}

	//Thrust OK switch
	for (int i = 0;i < 5;i++)
	{
		ThrustOK[i] = j2engines[i]->GetThrustOK();
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

	//Ullage engines
	if (OrdnanceArmed && ullage)
	{
		if (vessel->GetThrusterGroupLevel(ullage) < 1.0 && UllageTrigger)
		{
			vessel->SetThrusterGroupLevel(ullage, 1.0);
			sepSound.play(LOOP, 130);
			UllageTrigger = false;
		}
	}

	if (ullage && sepSound.isPlaying() && vessel->GetThrusterGroupLevel(ullage) < 1.0)
	{
		sepSound.stop();
	}

	//Failure code
	FailureTimer += simdt;

	for (int i = 0;i < 5;i++)
	{
		if (EarlySIICutoff[i] && (FailureTimer > SecondStageFailureTime[i]) && !j2engines[i]->GetFailed())
		{
			j2engines[i]->SetFailed();
		}
	}
}

void SIISystems::SetEngineFailureParameters(bool *SIICut, double *SIICutTimes)
{
	for (int i = 0;i < 5;i++)
	{
		EarlySIICutoff[i] = SIICut[i];
		SecondStageFailureTime[i] = SIICutTimes[i];
	}
}

void SIISystems::SetEngineFailureParameters(int n, double SIICutTimes, bool fail)
{
	if (n < 1 || n > 5) return;

	EarlySIICutoff[n - 1] = fail;
	SecondStageFailureTime[n - 1] = SIICutTimes;
}

void SIISystems::GetEngineFailureParameters(int n, bool &fail, double &failtime)
{
	if (n < 1 || n > 5) return;

	fail = EarlySIICutoff[n - 1];
	failtime = SecondStageFailureTime[n - 1];
}

#define MR_STATS 5

static double MixtureRatios[MR_STATS] = { 6.0, 5.5, 5.0, 4.3, 4.0 };
//static double MRISP[MR_STATS] = { 416*G, 418*G, 421*G, 427*G, 432*G };
static double MRISP[MR_STATS] = { 421.4 * G, 423.4 * G, 426.5 * G, 432.6 * G, 437.6 * G };
static double MRThrust[MR_STATS] = { 1.1, 1.0, .898, .7391, .7 };

void SIISystems::GetJ2ISP(double ratio, double &isp, double &ThrustAdjust)

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

void SIISystems::SetPUValve(int state)
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

void SIISystems::RecalculateEngineParameters(double BaseThrust)
{
	J2DefaultThrust = BaseThrust;

	RecalculateEngineParameters();
}

void SIISystems::RecalculateEngineParameters()
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

	SetMixtureRatio(MixtureRatio);
}

void SIISystems::SetMixtureRatio(double ratio)
{
	double isp, thrust, ThrustAdjust;

	GetJ2ISP(ratio, isp, ThrustAdjust);
	thrust = J2DefaultThrust * ThrustAdjust;

	for (int i = 0; i < 5; i++) {
		j2engines[i]->SetThrusterIsp(isp);
		j2engines[i]->SetThrusterMax0(thrust);
	}
}

void SIISystems::SetThrusterDir(int n, double beta_y, double beta_p)
{
	if (n < 0 || n > 3) return;

	j2engines[n]->SetThrusterDir(beta_y, beta_p);
}

bool SIISystems::PropellantLowLevel()
{
	if (PointLevelSensorArmed)
	{
		if (main_propellant)
		{
			if (vessel->GetPropellantMass(main_propellant) < 3866.0)
			{
				return true;
			}
		}
	}

	return false;
}

void SIISystems::GetThrustOK(bool *ok)
{
	for (int i = 0;i < 5;i++)
	{
		ok[i] = ThrustOK[i];
	}
}

bool SIISystems::GetPropellantDepletionEngineCutoff()
{
	if (PointLevelSensorArmed)
	{
		if (PropellantDepletionSensors) return true;

		for (int i = 0;i < 5;i++) if (ThrustOK[i]) return false;

		return true;
	}

	return false;
}

bool SIISystems::GetEngineOut()
{
	for (int i = 0;i < 5;i++) if (!ThrustOK[i]) return true;

	return false;
}

void SIISystems::SwitchSelector(int channel)
{
	switch (channel)
	{
	case 3: //S-II LOX Depletion Sensors Cutoff Arm
		SetDepletionSensorsCutoffArm();
		break;
	case 5: //S-II/S-IVB Separation
		if (SIISIVBOrdnanceArmed)
		{
			SII_SIVB_Separation_Pyros.SetBlown(true);
		}
		break;
	case 6: //Start Phase Limiter Cutoff Reset
		StartPhaseLimiterCutoffArmReset();
		break;
	case 7: //LH2 Step Pressurization
		LH2StepPressurization = true;
		break;
	case 8: //S-II/S-IVB Ordnance Arm
		SetSIISIVBOrdnanceArm();
		break;
	case 9: //Stop First PAM - FM/FM Calibration
		break;
	case 11: //S-II Ordnance Arm
		SetOrdnanceArm();
		break;
	case 12: //Hydraulic Accumulators Unlock
		break;
	case 14: //LOX Step Pressurization
		break;
	case 15: //S-II Center Engine Cutoff
		LVDCCenterEngineCutoff();
		break;
	case 18: //S-II Engines Cutoff
		LVDCEnginesCutoff();
		break;
	case 19: //Prevalves Lockout Reset
		break;
	case 20: //Engines Ready Bypass
		SetEnginesReadyBypass();
		break;
	case 23: //S-II Aft Interstage Separation
		if (OrdnanceArmed)
		{
			SII_Interstage_Pyros.SetBlown(true);
		}
		break;
	case 24: //S-II Ullage Trigger
		FireUllageTrigger();
		break;
	case 25: //Start Phase Limiter Cutoff Arm
		StartPhaseLimiterCutoffArm = true;
		break;
	case 30: //Start First PAM - FM/FM Relays Reset
		break;
	case 31: //S-II Engines Cutoff Reset
		LVDCEnginesCutoffReset();
		break;
	case 32: //PU System Activate
		break;
	case 33: //S-II Engine Start
		EngineStartOn();
		break;
	case 38: //LH2 Tank High Pressure Vent Mode
		break;
	case 42: //S-II LH2 Depletion Sensors Cutoff Arm
		SetDepletionSensorsCutoffArm();
		break;
	case 48: //LH2 Recirculation Pumps Off
		break;
	case 49: //Engines Ready Bypass Reset
		ResetEnginesReadyBypass();
		break;
	case 56: //Low (4.5) Engine MR Ratio On
		SetPUValve(PUVALVE_OPEN);
		puShiftSound.play(NOLOOP, 255);
		puShiftSound.done();
		break;
	case 58: //High (5.5) Engine MR Ratio Off
		SetPUValve(PUVALVE_NULL);
		break;
	case 59: //High (5.5 Engine Mixture Ratio On
		SetPUValve(PUVALVE_CLOSED);
		break;
	case 60: //PU System Open Loop Arm
		break;
	case 66: //Start Recorder Times
		break;
	case 71: //Start Data Recorders
		break;
	case 88: //Chilldown Valves Close
		break;
	case 90: //Measurement Control Switch No. 2 Activate
		break;
	case 99: //Prevalves Close Arm
		break;
	case 104: //Stop Data Recorders
		break;
	default:
		break;
	}
}

void SIISystems::LVDCEnginesCutoff()
{
	j2engine1.SetLVDCEngineCutoff();
	j2engine2.SetLVDCEngineCutoff();
	j2engine3.SetLVDCEngineCutoff();
	j2engine4.SetLVDCEngineCutoff();
	j2engine5.SetLVDCEngineCutoff();
}

void SIISystems::LVDCEnginesCutoffReset()
{
	j2engine1.ResetLVDCEnginesCutoff();
	j2engine2.ResetLVDCEnginesCutoff();
	j2engine3.ResetLVDCEnginesCutoff();
	j2engine4.ResetLVDCEnginesCutoff();
	j2engine5.ResetLVDCEnginesCutoff();
}

void SIISystems::LVDCCenterEngineCutoff()
{
	j2engine5.SetLVDCEngineCutoff();
}

void SIISystems::SetEnginesReadyBypass()
{
	j2engine1.SetEngineReadyBypass();
	j2engine2.SetEngineReadyBypass();
	j2engine3.SetEngineReadyBypass();
	j2engine4.SetEngineReadyBypass();
	j2engine5.SetEngineReadyBypass();
}

void SIISystems::ResetEnginesReadyBypass()
{
	j2engine1.ResetEngineReadyBypass();
	j2engine2.ResetEngineReadyBypass();
	j2engine3.ResetEngineReadyBypass();
	j2engine4.ResetEngineReadyBypass();
	j2engine5.ResetEngineReadyBypass();
}

void SIISystems::EngineStartOn()
{
	j2engine1.SetEngineStart();
	j2engine2.SetEngineStart();
	j2engine3.SetEngineStart();
	j2engine4.SetEngineStart();
	j2engine5.SetEngineStart();
}

void SIISystems::EngineStartOff()
{
	j2engine1.ResetEngineStart();
	j2engine2.ResetEngineStart();
	j2engine3.ResetEngineStart();
	j2engine4.ResetEngineStart();
	j2engine5.ResetEngineStart();
}

void SIISystems::EDSEnginesCutoff(bool cut)
{
	if (cut)
	{
		j2engine1.SetEDSCutoff();
		j2engine2.SetEDSCutoff();
		j2engine3.SetEDSCutoff();
		j2engine4.SetEDSCutoff();
		j2engine5.SetEDSCutoff();
	}
	else
	{
		j2engine1.ResetEDSCutoff();
		j2engine2.ResetEDSCutoff();
		j2engine3.ResetEDSCutoff();
		j2engine4.ResetEDSCutoff();
		j2engine5.ResetEDSCutoff();
	}
}