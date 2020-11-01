/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  LM Stabilization & Control System (and associated parts)

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
#include "lmresource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "IMU.h"
#include "lm_channels.h"
#include "tracer.h"
#include "papi.h"
#include "Mission.h"
#include "LEM.h"

// RATE GYRO ASSEMBLY

LEM_RGA::LEM_RGA()
{
	powered = false;
	dc_source = false;
	rates = _V(0, 0, 0);
}

void LEM_RGA::Init(LEM *v, e_object *dcsource, h_HeatLoad *hl, h_HeatLoad *sechl)
{
	dc_source = dcsource;
	lem = v;
	RGAHeat = hl;
	SecRGAHeat = sechl;
}

void LEM_RGA::Timestep(double simdt)
{
	if (lem == NULL) { return; }

	powered = false;
	rates = _V(0, 0, 0);
	if (lem != NULL) {
		if (dc_source != NULL && dc_source->Voltage() > SP_MIN_DCVOLTAGE) {
			powered = true;
			lem->GetAngularVel(rates);
		}
	}

	if (powered)
	{
		//Test Signal
		if (lem->SCS_ATCA_AGS_CB.IsPowered() && !lem->GyroTestRightSwitch.IsCenter())
		{
			double polar = 0.0;

			if (lem->GyroTestRightSwitch.IsUp())
			{
				polar = 1.0;
			}
			else if (lem->GyroTestRightSwitch.IsDown())
			{
				polar = -1.0;
			}

			if (lem->GyroTestLeftSwitch.IsUp())
			{
				rates.z += polar*5.0*RAD;
			}
			else if (lem->GyroTestLeftSwitch.IsCenter())
			{
				rates.x += polar*5.0*RAD;
			}
			if (lem->GyroTestLeftSwitch.IsDown())
			{
				rates.y += polar*5.0*RAD;
			}
		}
	}
}

void LEM_RGA::SystemTimestep(double simdt)
{
	if (powered && dc_source) 
	{
		dc_source->DrawPower(8.7);	//TBD: Actual value
		RGAHeat->GenerateHeat(4.35);	//TBD: Actual value
		SecRGAHeat->GenerateHeat(4.35);	//TBD: Actual value
	}
}

// ATTITUDE & TRANSLATION CONTROL ASSEMBLY
ATCA::ATCA(){
	lem = NULL;

	lgc_err_ena = 0;
	lgc_err_x = 0;
	lgc_err_y = 0;
	lgc_err_z = 0;

	K1 = false;
	K2 = false;
	K3 = false;
	K8 = false;
	K9 = false;
	K10 = false;
	K11 = false;
	K12 = false;
	K13 = false;
	K14 = false;
	K15 = false;
	K16 = false;
	K19 = false;
	K20 = false;
	K21 = false;

	aea_attitude_error = _V(0, 0, 0);
	att_rates = _V(0, 0, 0);
	aca_rates = _V(0, 0, 0);
	thrustLogicInputError = _V(0.0, 0.0, 0.0);
	translationCommands = _V(0, 0, 0);

	for (int i = 0;i < 8;i++)
	{
		SummingAmplifierOutput[i] = 0.0;
		PRMPulse[i] = false;
		PRMCycleTime[i] = 0.0;
		PRMOffTime[i] = 0.0;
	}

	hasAbortPower = false;
	hasPrimPower = false;

	RateGain = _V(0.0, 0.0, 0.0);
	DeadbandGain = _V(0.0, 0.0, 0.0);
	ACARateGain = _V(0, 0, 0);
	pitchGimbalError = 0.0;
	rollGimbalError = 0.0;
}

void ATCA::Init(LEM *vessel, h_HeatLoad *hl, h_HeatLoad *sechl){
	lem = vessel;
	ATCAHeat = hl;
	SECATCAHeat = sechl;
	int x=0; while(x < 16){ jet_request[x] = 0; jet_last_request[x] = 0; jet_start[x] = 0; jet_stop[x] = 0; x++; }
}

double ATCA::GetPrimPowerVoltage() {
	if (lem->CDR_SCS_ATCA_CB.IsPowered())
		return -4.7;
	else
		return 0.0;
}
double ATCA::GetBackupPowerVoltage() {
	if (lem->SCS_ATCA_AGS_CB.IsPowered())
		return -4.7;
	else
		return 0.0;
}

double ATCA::GetRGAPickoffExcitationVoltage()
{
	if (lem->SCS_ATCA_CB.IsPowered())
		return 28.0;
	else
		return 0.0;
}

double ATCA::GetRGASpinMotorVoltage()
{
	if (lem->SCS_ATCA_CB.IsPowered())
		return 26.0;
	else
		return 0.0;
}

double ATCA::GetPlus15VDCSupplyVoltage()
{
	if (lem->SCS_ATCA_CB.IsPowered())
		return 15.0;
	else
		return 0.0;
}

double ATCA::GetMinus15VDCSupplyVoltage()
{
	if (lem->SCS_ATCA_CB.IsPowered())
		return -15.0;
	else
		return 0.0;
}

double ATCA::GetPlus6VDCSupplyVoltage()
{
	if (lem->SCS_ATCA_CB.IsPowered())
		return 6.0;
	else
		return 0.0;
}

double ATCA::GetMinus6VDCSupplyVoltage()
{
	if (lem->SCS_ATCA_CB.IsPowered())
		return -6.0;
	else
		return 0.0;
}

double ATCA::GetPlus43VDCSupplyVoltage()
{
	if (lem->SCS_ATCA_CB.IsPowered())
		return 4.3;
	else
		return 0.0;
}

void ATCA::Timestep(double simt, double simdt){
	hasPrimPower = false, hasAbortPower = false;
	bool balcpl = false;
	thrustLogicInputError = _V(0.0, 0.0, 0.0);
	translationCommands = _V(0, 0, 0);
	if(lem == NULL){ return; }

	// Determine ATCA power situation.
	if (lem->CDR_SCS_ATCA_CB.IsPowered() && !lem->scca2.GetK12() && !lem->ModeControlPGNSSwitch.IsDown())
	{
		// ATCA primary power is on.
		hasPrimPower = true;
	}

	if (lem->SCS_ATCA_AGS_CB.IsPowered() && lem->scca2.GetK13() && !lem->ModeControlAGSSwitch.IsDown())
	{
		// ATCA abort power is on.
		hasAbortPower = true;
	}

	//AEA input bits

	if (!lem->scca2.GetK8() || (lem->ModeControlAGSSwitch.IsCenter() && lem->scca1.GetK7()))
	{
		lem->aea.SetInputPortBit(IO_2020, AGSFollowUpDiscrete, false);
	}
	else
	{
		lem->aea.SetInputPortBit(IO_2020, AGSFollowUpDiscrete, true);
	}

	if (lem->ModeControlAGSSwitch.IsUp())
	{
		lem->aea.SetInputPortBit(IO_2020, AGSAutomaticDiscrete, false);
	}
	else
	{
		lem->aea.SetInputPortBit(IO_2020, AGSAutomaticDiscrete, true);
	}

	//INPUTS: ACA, RGA and AEA

	att_rates = lem->rga.GetRates();
	aea_attitude_error = lem->aea.GetAttitudeError();
	aca_rates = _V(0, 0, 0);

	if (lem->scca2.GetK2())
	{
		aca_rates.z = lem->CDR_ACA.GetACAProp(2);
	}
	if (lem->scca2.GetK3())
	{
		aca_rates.y = lem->CDR_ACA.GetACAProp(1);
	}
	if (lem->scca2.GetK4())
	{
		aca_rates.x = lem->CDR_ACA.GetACAProp(0);
	}

	if (lem->SCS_ATCA_CB.IsPowered())
	{
		if (lem->stage < 2 && (!lem->scca2.GetK23() || !lem->scca2.GetK24()))
		{
			K8 = true;
			K9 = true;
			K10 = true;
			K11 = true;
			K12 = true;
			K13 = true;
		}
		else
		{
			K8 = false;
			K9 = false;
			K10 = false;
			K11 = false;
			K12 = false;
			K13 = false;
		}

		if (lem->DeadBandSwitch.IsUp() && !lem->scca1.GetK15() && !lem->scca1.GetK203() && !lem->scca1.GetK204())
		{
			K14 = true;
			K15 = true;
			K16 = true;
		}
		else
		{
			K14 = false;
			K15 = false;
			K16 = false;
		}

		if (lem->scca1.GetK1())
		{
			K19 = true;
		}
		else
		{
			K19 = false;
		}

		if (lem->scca1.GetK3())
		{
			K20 = true;
		}
		else
		{
			K20 = false;
		}

		if (lem->scca1.GetK5())
		{
			K21 = true;
		}
		else
		{
			K21 = false;
		}

		//THRUST LOGIC INPUT

		//Gain Switching
		if (K8)
		{
			RateGain.z = 22.5;
			ACARateGain.z = 63.8403;
		}
		else
		{
			RateGain.z = 6.0;
			ACARateGain.z = 17.6403;
		}
		if (K9)
		{
			RateGain.y = 22.5;
			ACARateGain.y = 63.6302;
		}
		else
		{
			RateGain.y = 6.0;
			ACARateGain.y = 17.4302;
		}
		if (K10)
		{
			RateGain.x = 22.5;
			ACARateGain.x = 63.6302;
		}
		else
		{
			RateGain.x = 6.0;
			ACARateGain.x = 17.4302;
		}

		if (K14)
		{
			DeadbandGain.z = 48.503;
		}
		else
		{
			DeadbandGain.z = 3.59;
		}
		if (K15)
		{
			DeadbandGain.y = 47.735;
		}
		else
		{
			DeadbandGain.y = 2.63;
		}
		if (K16)
		{
			DeadbandGain.x = 47.735;
		}
		else
		{
			DeadbandGain.x = 2.63;
		}

		//Attitude Error Limiter
		if (!K14)
		{
			//Limits attitude rate to 5.0°/s
			if (K11)
			{
				Limiter(aea_attitude_error.z, 7.9*RAD);
			}
			else
			{
				Limiter(aea_attitude_error.z, 2.4*RAD);
			}
		}

		if (!K15)
		{
			//Limits attitude rate to 10.0°/s
			if (K12)
			{
				Limiter(aea_attitude_error.y, 15.3*RAD);
			}
			else
			{
				Limiter(aea_attitude_error.y, 4.3*RAD);
			}
		}

		if (!K16)
		{
			//Limits attitude rate to 5.0°/s
			if (K13)
			{
				Limiter(aea_attitude_error.x, 7.8*RAD);
			}
			else
			{
				Limiter(aea_attitude_error.x, 2.3*RAD);
			}
		}

		//Yaw
		if (K19)
		{
			if (lem->CDR_ACA.GetPlusYawBreakout())
			{
				thrustLogicInputError.z = 2.0;
			}
			else if (lem->CDR_ACA.GetMinusYawBreakout())
			{
				thrustLogicInputError.z = -2.0;
			}
		}
		else
		{
			thrustLogicInputError.z = (aca_rates.z*ACARateGain.z + aea_attitude_error.z*DEG*0.3*7.0 - att_rates.y*DEG*0.14*RateGain.z)*4.57;
			if (thrustLogicInputError.z > 0.0)
			{
				thrustLogicInputError.z = max(0.0, abs(thrustLogicInputError.z) - DeadbandGain.z)*2.0;
			}
			else
			{
				thrustLogicInputError.z = -max(0.0, abs(thrustLogicInputError.z) - DeadbandGain.z)*2.0;
			}
		}

		//Pitch
		if (K20)
		{
			if (lem->CDR_ACA.GetPlusPitchBreakout())
			{
				thrustLogicInputError.y = 2.0;
			}
			else if (lem->CDR_ACA.GetMinusPitchBreakout())
			{
				thrustLogicInputError.y = -2.0;
			}
		}
		else
		{
			thrustLogicInputError.y = (aca_rates.y*ACARateGain.y + aea_attitude_error.y*DEG*0.3*7.0 - att_rates.x*DEG*0.14*RateGain.y)*4.57;
			pitchGimbalError = thrustLogicInputError.y;
			if (thrustLogicInputError.y > 0.0)
			{
				thrustLogicInputError.y = max(0.0, abs(thrustLogicInputError.y) - DeadbandGain.y)*2.0;
			}
			else
			{
				thrustLogicInputError.y = -max(0.0, abs(thrustLogicInputError.y) - DeadbandGain.y)*2.0;
			}
		}

		//Roll
		if (K21)
		{
			if (lem->CDR_ACA.GetPlusRollBreakout())
			{
				thrustLogicInputError.x = 2.0;
			}
			else if (lem->CDR_ACA.GetMinusRollBreakout())
			{
				thrustLogicInputError.x = -2.0;
			}
		}
		else
		{
			thrustLogicInputError.x = (aca_rates.x*ACARateGain.x + aea_attitude_error.x*DEG*0.3*7.0 - att_rates.z*DEG*0.14*RateGain.x)*4.57;
			rollGimbalError = thrustLogicInputError.x;
			if (thrustLogicInputError.x > 0.0)
			{
				thrustLogicInputError.x = max(0.0, abs(thrustLogicInputError.x) - DeadbandGain.x)*2.0;
			}
			else
			{
				thrustLogicInputError.x = -max(0.0, abs(thrustLogicInputError.x) - DeadbandGain.x)*2.0;
			}
		}

		//sprintf(oapiDebugString(), "Thrust Logic Input Error: %f %f %f", thrustLogicInputError.x, thrustLogicInputError.y, thrustLogicInputError.z);

		//TRANSLATIONAL COMMANDS
		if (lem->SCS_ATCA_AGS_CB.IsPowered())
		{
			if (lem->LeftTTCATranslSwitch.IsUp() && lem->SCS_ATCA_CB.IsPowered())
			{
				if (lem->CDR_TTCA.GetPlusXTrans())
				{
					translationCommands.x = 15.0;
				}
				else if (lem->CDR_TTCA.GetMinusXTrans())
				{
					translationCommands.x = -15.0;
				}

				if (lem->CDR_TTCA.GetPlusYTrans())
				{
					translationCommands.y = 15.0;
				}
				else if (lem->CDR_TTCA.GetMinusYTrans())
				{
					translationCommands.y = -15.0;
				}

				if (lem->CDR_TTCA.GetPlusZTrans())
				{
					translationCommands.z = 15.0;
				}
				else if (lem->CDR_TTCA.GetMinusZTrans())
				{
					translationCommands.z = -15.0;
				}
			}
		}

		//JET SELECT LOGIC

		bool A, B, X1, X2, R1, Q1, R2, Q2, Y1, Y2, Z1, Z2;

		X1 = translationCommands.x > 0.0;
		X2 = translationCommands.x < 0.0;
		R1 = thrustLogicInputError.x > 0.0;
		R2 = thrustLogicInputError.x < 0.0;
		Q1 = thrustLogicInputError.y > 0.0;
		Q2 = thrustLogicInputError.y < 0.0;
		Y1 = translationCommands.y > 0.0;
		Y2 = translationCommands.y < 0.0;
		Z1 = translationCommands.z > 0.0;
		Z2 = translationCommands.z < 0.0;

		//Yaw and Y/Z Translation selection logic
		if (!Y1 && !Y2 && !Z1 && !Z2)
		{
			K3 = true;
		}
		else
		{
			K3 = false;
		}

		//Pitch, Roll and X Translation selection logic
		if (!(lem->ATTTranslSwitch.IsDown() && !lem->scca1.GetK15() && !lem->scca1.GetK203() && !lem->scca1.GetK204()))
		{
			A = false;
		}
		else
		{
			A = true;
		}

		if (lem->ATTTranslSwitch.IsUp())
		{
			B = false;
		}
		else
		{
			B = true;
		}

		if (A && !X1 && !X2 && ((!R1 && !R2) || (!Q1 && !Q2)))
		{
			K1 = true;
		}
		else
		{
			K1 = false;
		}

		if (B && !Q1 && !Q2 && !R1 && !R2)
		{
			K2 = true;
		}
		else
		{
			K2 = false;
		}

		//SUMMING AMPLIFIERS

		//1
		SummingAmplifierOutput[0] = (K3 ? 0.0 : thrustLogicInputError.z) - translationCommands.y;

		//2
		SummingAmplifierOutput[1] = 0.0 - (K3 ? 0.0 : thrustLogicInputError.z) - translationCommands.y;

		//3
		SummingAmplifierOutput[2] = (thrustLogicInputError.z - translationCommands.z);

		//4
		SummingAmplifierOutput[3] = (0.0 - thrustLogicInputError.z - translationCommands.z);

		//5
		SummingAmplifierOutput[4] = (K1 ? 0.0 : thrustLogicInputError.x) - (K1 ? 0.0 : thrustLogicInputError.y) - (K2 ? 0.0 : translationCommands.x);

		//6
		SummingAmplifierOutput[5] = thrustLogicInputError.x + (K1 ? 0.0 : thrustLogicInputError.y) - translationCommands.x;

		//7
		SummingAmplifierOutput[6] = thrustLogicInputError.y - thrustLogicInputError.x - (K2 ? 0.0 : translationCommands.x);

		//8
		SummingAmplifierOutput[7] = 0.0 - thrustLogicInputError.y - (K1 ? 0.0 : thrustLogicInputError.x) - translationCommands.x;

		//sprintf(oapiDebugString(), "Summing Amplifiers: %f %f %f %f %f %f %f %f", SummingAmplifierOutput[0], SummingAmplifierOutput[1], SummingAmplifierOutput[2],
		//	SummingAmplifierOutput[3], SummingAmplifierOutput[4], SummingAmplifierOutput[5], SummingAmplifierOutput[6], SummingAmplifierOutput[7]);

		//PULSE RATIO (DE)MODULATOR

		for (int i = 0;i < 8;i++)
		{
			if (abs(SummingAmplifierOutput[i]) > 0.5)
			{
				double dr = PRMDutyRatio(SummingAmplifierOutput[i]);
				double pw = PRMPulseWidth(SummingAmplifierOutput[i]);
				PRMPulse[i] = PRMTimestep(i, simdt, pw/dr, pw);
			}
			else
			{
				PRMPulse[i] = false;
				PRMCycleTime[i] = 0.0;
				PRMOffTime[i] = 0.0;
			}
		}

		//sprintf(oapiDebugString(), "PRM: %d %d %d %d %d %d %d %d", PRMPulse[0], PRMPulse[1], PRMPulse[2], PRMPulse[3], PRMPulse[4], PRMPulse[5], PRMPulse[6], PRMPulse[7]);
	}
	else
	{
		K1 = false;
		K2 = false;
		K3 = false;
		K8 = false;
		K9 = false;
		K10 = false;
		K11 = false;
		K12 = false;
		K13 = false;
		K14 = false;
		K15 = false;
		K16 = false;
		K19 = false;
		K20 = false;
		K21 = false;

		pitchGimbalError = 0.0;
		rollGimbalError = 0.0;

		for (int i = 0;i < 8;i++)
		{
			PRMPulse[i] = false;
		}
	}

	if (hasAbortPower)
	{
		for (int i = 0;i < 16;i++)
		{
			jet_request[i] = 0;
		}

		if (PRMPulse[0])
		{
			if (SummingAmplifierOutput[0] > 0.0)
			{
				jet_request[14] = 1;
				jet_request[2] = 0;
			}
			else
			{
				jet_request[14] = 0;
				jet_request[2] = 1;
			}
		}
		if (PRMPulse[1])
		{
			if (SummingAmplifierOutput[1] > 0.0)
			{
				jet_request[9] = 1;
				jet_request[5] = 0;
			}
			else
			{
				jet_request[9] = 0;
				jet_request[5] = 1;
			}
		}
		if (PRMPulse[2])
		{
			if (SummingAmplifierOutput[2] > 0.0)
			{
				jet_request[1] = 1;
				jet_request[6] = 0;
			}
			else
			{
				jet_request[1] = 0;
				jet_request[6] = 1;
			}
		}
		if (PRMPulse[3])
		{
			if (SummingAmplifierOutput[3] > 0.0)
			{
				jet_request[13] = 1;
				jet_request[10] = 0;
			}
			else
			{
				jet_request[13] = 0;
				jet_request[10] = 1;
			}
		}
		if (PRMPulse[4])
		{
			if (SummingAmplifierOutput[4] > 0.0)
			{
				jet_request[12] = 1;
				jet_request[15] = 0;
			}
			else
			{
				jet_request[12] = 0;
				jet_request[15] = 1;
			}
		}
		if (PRMPulse[5])
		{
			if (SummingAmplifierOutput[5] > 0.0)
			{
				jet_request[8] = 1;
				jet_request[11] = 0;
			}
			else
			{
				jet_request[8] = 0;
				jet_request[11] = 1;
			}
		}
		if (PRMPulse[6])
		{
			if (SummingAmplifierOutput[6] > 0.0)
			{
				jet_request[4] = 1;
				jet_request[7] = 0;
			}
			else
			{
				jet_request[4] = 0;
				jet_request[7] = 1;
			}
		}
		if (PRMPulse[7])
		{
			if (SummingAmplifierOutput[7] > 0.0)
			{
				jet_request[0] = 1;
				jet_request[3] = 0;
			}
			else
			{
				jet_request[0] = 0;
				jet_request[3] = 1;
			}
		}
	}

	/* THRUSTER TABLE:
		0	A1U		8	A3U
		1	A1F		9	A3R
		2	B1L		10	B3A
		3	B1D		11	B3D

		4	B2U		12	B4U
		5	B2L		13	B4F
		6	A2A		14	A4R
		7	A2D		15	A4D
	*/

	// *** Test "Balanced Couples" switch ***
	if(lem->BALCPLSwitch.GetState() == TOGGLESWITCH_UP){ balcpl = 1; }

	// *** THRUSTER MAINTENANCE ***
	// LM RCS thrusters take 12.5ms to ramp up to full thrust and 17.5ms to ramp back down. There is a dead time of 10ms before thrust starts.
	// The upshot of this is that full thrust always starts 20ms after commanded and stops 8ms after commanded. 
	// Orbiter has no hope of providing enough resolution to properly map this. It depends on framerate.
	// We can do our best though...
	int x=0;
	while(x < 16){
		double power=0;
		// If the ATCA is not powered or driver voltage is absent, it won't work.
		if(hasPrimPower == false && hasAbortPower == false){ jet_request[x] = 0; }
		// If the "Balanced Couples" switch is off, the abort preamps for the four upward-firing thrusters are disabled.
		if((hasAbortPower == true && balcpl == false) && (x == 0 || x == 4 || x == 8 || x == 12)){ jet_request[x] = 0;	}
		// Process jet request list to generate start and stop times.
		if(jet_request[x] == 1 && jet_last_request[x] == 0){
			// New fire request
			jet_start[x] = simt;
			jet_stop[x] = 0;
		}else if(jet_request[x] == 0 && jet_last_request[x] == 1){
			// New stop request
			jet_stop[x] = simt;
		}
		jet_last_request[x] = jet_request[x]; // Keep track of changes

		if (jet_start[x] == 0 && jet_stop[x] == 0) { lem->SetRCSJet(x, false); x++; continue; } // Done
		// sprintf(oapiDebugString(),"Jet %d fire %f stop %f",x,jet_start[x],jet_stop[x]); 
		if(simt > jet_start[x]+0.01 && simt < jet_start[x]+0.0125){
			// Ramp up
			power = ((simt-jet_start[x])/0.0125);			
		}
		if(jet_stop[x] > 0 && (simt > jet_stop[x]+0.0075 && simt < jet_stop[x]+0.0175)){
		    // Ramp down
			power = 1.0 - ((simt - jet_stop[x] - 0.0075) / 0.01);
		}
		if(jet_stop[x] > 0 && simt > jet_stop[x]+0.0175){
			// Thruster off
			power=0; jet_start[x] = 0; jet_stop[x] = 0;
		}else{
			if(simt > jet_start[x]+0.0125){
				// Full Power
				power=1;
			}
		}
		// FIXME: This is just for testing.
		// if(power > 0.25){ power = 0.25; }
		lem->SetRCSJetLevelPrimary(x,power);
		x++;
	}
}

void ATCA::SystemTimestep(double simdt)
{
	if (lem->SCS_ATCA_CB.IsPowered() && lem->CDR_SCS_ATCA_CB.IsPowered() && !lem->scca2.GetK12() && !lem->ModeControlPGNSSwitch.IsDown())
	{
		lem->SCS_ATCA_CB.DrawPower(55.0);	//ATCA cb under PGNS
		lem->CDR_SCS_ATCA_CB.DrawPower(2.0);
		ATCAHeat->GenerateHeat(22.0);		//ATCA & ATCA PGNS Heat
		SECATCAHeat->GenerateHeat(22.0);
	}

	else if (lem->SCS_ATCA_CB.IsPowered() && lem->SCS_ATCA_AGS_CB.IsPowered() && lem->scca2.GetK13() && !lem->ModeControlAGSSwitch.IsDown())
	{
		lem->SCS_ATCA_CB.DrawPower(65.0);	//ATCA cb under AGS
		lem->SCS_ATCA_AGS_CB.DrawPower(2.0);
		ATCAHeat->GenerateHeat(22.0);		//ATCA & ATCA AGS Heat
		SECATCAHeat->GenerateHeat(22.0);
	}
}

// Process thruster commands from LGC
void ATCA::ProcessLGC(int ch, int val){		
	if(!hasPrimPower){ val = 0; } // If not in primary mode, force jets off (so jets will switch off at programmed times)
	// When in primary, thruster commands are passed from LGC to jets.
	switch(ch){
		case 05:
			LMChannelValue5 ch5;
			ch5.Value = val;			
			if(ch5.Bits.B4U != 0){ jet_request[12] = 1; }else{ jet_request[12] = 0; }
			if(ch5.Bits.A4D != 0){ jet_request[15] = 1; }else{ jet_request[15] = 0; }
			if(ch5.Bits.A3U != 0){ jet_request[8]  = 1; }else{ jet_request[8]  = 0; }
			if(ch5.Bits.B3D != 0){ jet_request[11] = 1; }else{ jet_request[11] = 0; }
			if(ch5.Bits.B2U != 0){ jet_request[4]  = 1; }else{ jet_request[4]  = 0; }
			if(ch5.Bits.A2D != 0){ jet_request[7]  = 1; }else{ jet_request[7]  = 0; }
			if(ch5.Bits.A1U != 0){ jet_request[0]  = 1; }else{ jet_request[0]  = 0; }
			if(ch5.Bits.B1D != 0){ jet_request[3]  = 1; }else{ jet_request[3]  = 0; }			
			break;
		case 06:
			LMChannelValue6 ch6;
			ch6.Value = val;
			if(ch6.Bits.B3A != 0){ jet_request[10] = 1; }else{ jet_request[10] = 0; }
			if(ch6.Bits.B4F != 0){ jet_request[13] = 1; }else{ jet_request[13] = 0; }
			if(ch6.Bits.A1F != 0){ jet_request[1]  = 1; }else{ jet_request[1]  = 0; }
			if(ch6.Bits.A2A != 0){ jet_request[6]  = 1; }else{ jet_request[6]  = 0; }
			if(ch6.Bits.B2L != 0){ jet_request[5]  = 1; }else{ jet_request[5]  = 0; }
			if(ch6.Bits.A3R != 0){ jet_request[9]  = 1; }else{ jet_request[9]  = 0; }
			if(ch6.Bits.A4R != 0){ jet_request[14] = 1; }else{ jet_request[14] = 0; }
			if(ch6.Bits.B1L != 0){ jet_request[2]  = 1; }else{ jet_request[2]  = 0; }			
			break;
		default:
			sprintf(oapiDebugString(),"ATCA::ProcessLGC: Bad channel %o",ch);
	}
}

double ATCA::PRMDutyRatio(double volt)
{
	if (abs(volt) > 0.5)
	{
		return exp(0.3345588235*(min(abs(volt), 9.999) - 10.0));
	}
	return 0.0;
}

double ATCA::PRMPulseWidth(double volt)
{
	if (abs(volt) > 0.5)
	{
		return -1.0/(7.8125*(min(abs(volt), 9.999) - 10.0));
	}
	return 0.0;
}

bool ATCA::PRMTimestep(int n, double simdt, double pp, double pw)
{
	PRMCycleTime[n] += simdt;
	PRMOffTime[n] += simdt;

	if (PRMCycleTime[n] > pp)
	{
		PRMCycleTime[n] =  0.0;
		PRMOffTime[n] = 0.0;
	}

	if (PRMOffTime[n] < pw + simdt)
	{
		return true;
	}

	return false;
}

void ATCA::Limiter(double &val, double lim)
{
	if (val > lim)
	{
		val = lim;
	}
	else if (val < -lim)
	{
		val = -lim;
	}
}

double ATCA::GetDPSPitchGimbalError()
{
	if (!K20)
		return pitchGimbalError;
	return 0.0;
}

double ATCA::GetDPSRollGimbalError()
{
	if (!K21)
		return rollGimbalError;
	return 0.0;
}

void ATCA::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, ATCA_START_STRING);

	oapiWriteScenario_int(scn, "FDAIERRENA", lgc_err_ena);
	oapiWriteScenario_int(scn, "FDAIERRX", lgc_err_x);
	oapiWriteScenario_int(scn, "FDAIERRY", lgc_err_y);
	oapiWriteScenario_int(scn, "FDAIERRZ", lgc_err_z);
	papiWriteScenario_bool(scn, "K1", K1);
	papiWriteScenario_bool(scn, "K2", K2);
	papiWriteScenario_bool(scn, "K3", K3);
	papiWriteScenario_bool(scn, "K8", K8);
	papiWriteScenario_bool(scn, "K9", K9);
	papiWriteScenario_bool(scn, "K10", K10);
	papiWriteScenario_bool(scn, "K11", K11);
	papiWriteScenario_bool(scn, "K12", K12);
	papiWriteScenario_bool(scn, "K13", K13);
	papiWriteScenario_bool(scn, "K14", K14);
	papiWriteScenario_bool(scn, "K15", K15);
	papiWriteScenario_bool(scn, "K16", K16);
	papiWriteScenario_bool(scn, "K19", K19);
	papiWriteScenario_bool(scn, "K20", K20);
	papiWriteScenario_bool(scn, "K21", K21);

	oapiWriteLine(scn, ATCA_END_STRING);
}

void ATCA::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, ATCA_END_STRING, sizeof(ATCA_END_STRING))) {
			return;
		}

		papiReadScenario_int(line, "FDAIERRENA", lgc_err_ena);
		papiReadScenario_int(line, "FDAIERRX", lgc_err_x);
		papiReadScenario_int(line, "FDAIERRY", lgc_err_y);
		papiReadScenario_int(line, "FDAIERRZ", lgc_err_z);
		papiReadScenario_bool(line, "K1", K1);
		papiReadScenario_bool(line, "K2", K2);
		papiReadScenario_bool(line, "K3", K3);
		papiReadScenario_bool(line, "K8", K8);
		papiReadScenario_bool(line, "K9", K9);
		papiReadScenario_bool(line, "K10", K10);
		papiReadScenario_bool(line, "K11", K11);
		papiReadScenario_bool(line, "K12", K12);
		papiReadScenario_bool(line, "K13", K13);
		papiReadScenario_bool(line, "K14", K14);
		papiReadScenario_bool(line, "K15", K15);
		papiReadScenario_bool(line, "K16", K16);
		papiReadScenario_bool(line, "K19", K19);
		papiReadScenario_bool(line, "K20", K20);
		papiReadScenario_bool(line, "K21", K21);
	}
}

// DESCENT ENGINE CONTROL ASSEMBLY
DECA::DECA() {
	lem = NULL;
	powered = FALSE;
	dc_source = NULL;
	pitchactuatorcommand = 0;
	rollactuatorcommand = 0;
	engOn = false;
	lgcAutoThrust = 0.0;
	ManualThrust = 0.0;
	//85.9% for 12V
	LMR = 0.859*12.0;

	ResetRelays();
}

void DECA::ResetRelays()
{
	K1 = false;
	K2 = false;
	K3 = false;
	K4 = false;
	K5 = false;
	K6 = false;
	K7 = false;
	K8 = false;
	K9 = false;
	K10 = false;
	K13 = false;
	K14 = false;
	K15 = false;
	K16 = false;
	K21 = false;
	K22 = false;
	K23 = false;
	K24 = false;
	K25 = false;
	K26 = false;
	K27 = false;
	K28 = false;
}

void DECA::Init(LEM *v, e_object *dcbus) {
	// Initialize
	lem = v;
	dc_source = dcbus;
}

void DECA::Timestep(double simdt) {
	powered = false;
	if (lem == NULL) return;

	if (lem->stage > 1)
	{
		//Set everything to false and then return
		lem->DPS.pitchGimbalActuator.ChangeCmdPosition(0);
		lem->DPS.rollGimbalActuator.ChangeCmdPosition(0);
		lgcAutoThrust = 0.0;
		ManualThrust = 0.0;
		AutoThrust = 0.0;

		ResetRelays();

		return;
	}

	//Needs voltage and a descent stage. The DECA is mounted on it.
	if (dc_source->Voltage() > SP_MIN_DCVOLTAGE && lem->stage < 2) {
		powered = true;
	}

	//Process input and output
	//Input:
	//-Descent Engine Arm (Switch)
	//-Manual Descent Engine Start-Stop Commands (Buttons)
	//-Automatic/Manual Throttle Select (Switch)
	//-Automatic Descent Engine on-off commands (PGNS)
	//-Automatic PGNS Descent Engine Trim Commands (PGNS)
	//-Automatic Throttle Commands (PGNS)
	//-Gimbal Feedback Commands (Actuators)
	//-Manual Descent Engine Throttle Commands (TTCA)
	//-Automatic AGS Descent Engine Trim Commands (ATCA)
	//
	//Output:
	//-Descent Engine Trim Indication (PGNS)
	//-Throttle Commands (DPS)
	//-Engine On-Off Commands (DPS)
	//-Engine Arm Command (DPS)
	//-Gimbal Trim Commands(Actuators)

	//Descent Engine Control Section
	if ((lem->SCS_ENG_ARM_CB.IsPowered() || (lem->AbortSwitch.GetState() == 1 && lem->SCS_ENG_CONT_CB.IsPowered())) && lem->EngineArmSwitch.IsDown())
	{
		DEArm = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->AbortSwitch.GetState() == 0)
	{
		DEArm = true;
	}
	else
	{
		DEArm = false;
	}

	//DECA Power Supply Failure
	if (DEArm && !powered)
	{
		K26 = true;
	}
	else
	{
		K26 = false;
	}

	if (DEArm)
	{
		K1 = true;
		K10 = true;
		K23 = true;
		K25 = true;
	}
	else
	{
		K1 = false;
		K10 = false;
		K23 = false;
		K25 = false;
	}

	if (DEArm && !K26)
	{
		K2 = true;
		K24 = true;
	}
	else
	{
		K2 = false;
		K24 = false;
	}

	//Descent Engine Stop
	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->EngineArmSwitch.IsUp() && lem->scca1.GetK20())
	{
		K3 = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && (lem->CDRManualEngineStop.GetState() == 1 || lem->LMPManualEngineStop.GetState() == 1))
	{
		K3 = true;
	}
	else
	{
		K3 = false;
	}

	//Descent Engine Start
	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->EngineArmSwitch.IsDown() && lem->scca2.GetK19() && !K3)
	{
		K7 = true;
	}
	else
	{
		K7 = false;
	}

	if (lem->SCS_DECA_PWR_CB.IsPowered() && lem->scca3.GetK6() && !K6)
	{
		K28 = true;
	}
	else
	{
		K28 = false;
	}

	//Auto Engine On-Off
	bool X = lem->scca1.GetK18();
	bool Y = lem->scca1.GetK17();
	bool Q = !K28;

	if ((X && !Y) || (Q && ((!X && !Y) || (X && Y))))
	{
		if (lem->SCS_DECA_PWR_CB.IsPowered() && (K1 || K23) && !K3)
		{
			K6 = true;
		}
		else
		{
			K6 = false;
		}
	}
	else
	{
		K6 = false;
	}

	if (lem->SCS_DECA_PWR_CB.IsPowered() && (lem->scca3.GetK6()) && (K7 || K6))
	{
		engOn = true;
		K16 = true;
	}
	else
	{
		engOn = false;
		K16 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->THRContSwitch.IsDown())
	{
		K15 = true;
	}
	else
	{
		K15 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && K16)
	{
		lem->aea.SetInputPortBit(IO_2020, AGSDescentEngineOnDiscrete, false);
	}
	else
	{
		lem->aea.SetInputPortBit(IO_2020, AGSDescentEngineOnDiscrete, true);
	}

	//GIMBALING SIGNAL

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->EngGimbalEnableSwitch.IsDown())
	{
		K8 = true;
		K4 = true;
		K9 = true;
		K5 = true;
	}
	else
	{
		K8 = false;
		K4 = false;
		K9 = false;
		K5 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->scca2.GetK11())
	{
		K14 = true;
		K13 = true;
	}
	else
	{
		K14 = false;
		K13 = false;
	}

	ChannelValue val11, val12;
	val11 = lem->agc.GetOutputChannel(011);
	val12 = lem->agc.GetOutputChannel(012);

	if (K13)
	{
		double atcaPitch = lem->atca.GetDPSPitchGimbalError();

		if (atcaPitch > 0.5)
		{
			pitchactuatorcommand = 1;
		}
		else if (atcaPitch < -0.5)
		{
			pitchactuatorcommand = -1;
		}
		else
		{
			pitchactuatorcommand = 0;
		}
	}
	else
	{
		//Process Pitch Gimbal Actuator command
		int valx = val12[PlusPitchVehicleMotion];
		int valy = val12[MinusPitchVehicleMotion];
		pitchactuatorcommand = valx - valy;
	}

	if (K14)
	{
		double atcaRoll = lem->atca.GetDPSRollGimbalError();

		if (atcaRoll > 0.5)
		{
			rollactuatorcommand = 1;
		}
		else if (atcaRoll < -0.5)
		{
			rollactuatorcommand = -1;
		}
		else
		{
			rollactuatorcommand = 0;
		}
	}
	else
	{
		//Process Roll Gimbal Actuator command
		int valx = val12[PlusRollVehicleMotion];
		int valy = val12[MinusRollVehicleMotion];
		rollactuatorcommand = valx - valy;
	}

	if (powered && !K4 && !K5 && !K8 && !K9 && K25) //If off, send out all zeros
	{
		lem->DPS.pitchGimbalActuator.ChangeCmdPosition(pitchactuatorcommand);
		lem->DPS.rollGimbalActuator.ChangeCmdPosition(rollactuatorcommand);
	}
	else
	{
		lem->DPS.pitchGimbalActuator.ChangeCmdPosition(0);
		lem->DPS.rollGimbalActuator.ChangeCmdPosition(0);
	}

	//Gimbal Failure Indication
	if (powered && (K1 || K23) && lem->DPS.pitchGimbalActuator.GimbalFail())
	{
		K21 = true;
	}
	else
	{
		K21 = false;
	}

	if (powered && (K1 || K23) && lem->DPS.rollGimbalActuator.GimbalFail())
	{
		K22 = true;
	}
	else
	{
		K22 = false;
	}

	if (K21 || K22)
	{
		lem->agc.SetInputChannelBit(032, ApparentDecscentEngineGimbalsFailed, 1);
	}

	// THROTTLING SIGNAL

	//Reset auto throttle counter in manual mode
	if (lem->SCS_ATCA_CB.IsPowered() && K2 && K15 && !K26)
	{
		lgcAutoThrust = 0.0;
	}
	else if (!K2)	//And simply when the engine isn't armed... until we figure out how it works properly
	{
		lgcAutoThrust = 0.0;
	}

	if (lem->SCS_ATCA_CB.IsPowered() && K24 && !K15 && !K26)
	{
		//Auto Thrust commands are generated in ProcessLGCThrustCommands()
		//DECA creates a voltage for the throttle command, this voltage can only change the thrust at a rate of 40,102 Newtons/second according to the GSOP.
		//Rounded this is 85.9% of the total throttle range, which should be a decent estimate for all missions.
		dposcmd = lgcAutoThrust - AutoThrust;
		poscmdsign = abs(lgcAutoThrust - AutoThrust) / (lgcAutoThrust - AutoThrust);
		if (abs(dposcmd)>LMR*simdt)
		{
			dpos = poscmdsign*LMR*simdt;
		}
		else
		{
			dpos = dposcmd;
		}

		AutoThrust += dpos;
	}
	else
	{
		AutoThrust = 0.0;
	}

	//PGNS or AGS Control
	if ((lem->IMU_OPR_CB.IsPowered() && !lem->scca2.GetK5()) || (lem->SCS_ATCA_CB.IsPowered() && lem->scca2.GetK5()))
	{
		double ttca_throttle_pos;

		if (lem->MANThrotSwitch.IsUp())
		{
			ttca_throttle_pos = lem->CDR_TTCA.GetThrottlePosition();
		}
		else
		{
			ttca_throttle_pos = 0.0;
		}

		if (ttca_throttle_pos > 0.51 / 0.66)
		{
			ManualThrust = 1.993081081*ttca_throttle_pos - 0.9930810811;
		}
		else
		{
			ManualThrust = 0.5785055644*ttca_throttle_pos + 0.1;
		}
	}
	else
	{
		ManualThrust = 0.0;
	}

	if (!(lem->SCS_ATCA_CB.IsPowered() && (lem->DECA_GMBL_AC_CB.Voltage() > SP_MIN_ACVOLTAGE) && K2 && K24 && K25))
	{
		ManualThrust = 0.0;
	}

	//TBD: Manual thrust also as a voltage. Throttle actuator input should be voltages eventually, too.
	lem->DPS.ThrottleActuator(ManualThrust, AutoThrust*0.9 / 12.0);

	//sprintf(oapiDebugString(), "engOn: %d engOff: %d Thrust: %f", engOn, engOff, dpsthrustcommand);
	//sprintf(oapiDebugString(), "Manual: K1 %d K3 %d K7 %d K10 %d K16 %d K23 %d K28 %d", K1, K3, K7, K10, K16, K23, K28);
	//sprintf(oapiDebugString(), "Auto: X %d Y %d Q %d K6 %d K10 %d K15 %d K16 %d K23 %d K28 %d", X, Y, Q, K6, K10, K15, K16, K23, K28);
}

void DECA::ProcessLGCThrustCommands(int val) {

	int pulses;
	double thrust_cmd;

	if (powered == 0) { return; }

	if (val & 040000) { // Negative
		pulses = -((~val) & 077777);
	}
	else {
		pulses = val & 077777;
	}

	thrust_cmd = (0.0035*pulses);

	lgcAutoThrust += thrust_cmd;

	if (lgcAutoThrust > 12.0)
	{
		lgcAutoThrust = 12.0;
	}
	else if (lgcAutoThrust < 0)
	{
		lgcAutoThrust = 0.0;
	}

	//sprintf(oapiDebugString(), "Thrust val: %o, Thrust pulses: %d, thrustchange: %f, lgcAutoThrust: %f", val, pulses, thrust_cmd, lgcAutoThrust);
}

void DECA::SystemTimestep(double simdt) {

	if (powered && dc_source)
		dc_source->DrawPower(10.6);  // take DC power
}

bool DECA::GetEngArm()
{
	if (lem->stage < 2 && dc_source && dc_source->Voltage() > SP_MIN_DCVOLTAGE && (K1 || K23))
		return true;

	return false;
}

void DECA::SaveState(FILEHANDLE scn) {

	// START_STRING is written in LEM
	oapiWriteScenario_int(scn, "PITCHACTUATORCOMMAND", pitchactuatorcommand);
	oapiWriteScenario_int(scn, "ROLLACTUATORCOMMAND", rollactuatorcommand);
	papiWriteScenario_double(scn, "AUTOTHRUST", AutoThrust);
	papiWriteScenario_double(scn, "LGCAUTOTHRUST", lgcAutoThrust);
	papiWriteScenario_bool(scn, "K1", K1);
	papiWriteScenario_bool(scn, "K2", K2);
	papiWriteScenario_bool(scn, "K3", K3);
	papiWriteScenario_bool(scn, "K4", K4);
	papiWriteScenario_bool(scn, "K5", K5);
	papiWriteScenario_bool(scn, "K6", K6);
	papiWriteScenario_bool(scn, "K7", K7);
	papiWriteScenario_bool(scn, "K8", K8);
	papiWriteScenario_bool(scn, "K9", K9);
	papiWriteScenario_bool(scn, "K10", K10);
	papiWriteScenario_bool(scn, "K13", K13);
	papiWriteScenario_bool(scn, "K14", K14);
	papiWriteScenario_bool(scn, "K15", K15);
	papiWriteScenario_bool(scn, "K16", K16);
	papiWriteScenario_bool(scn, "K21", K21);
	papiWriteScenario_bool(scn, "K22", K22);
	papiWriteScenario_bool(scn, "K23", K23);
	papiWriteScenario_bool(scn, "K24", K24);
	papiWriteScenario_bool(scn, "K25", K25);
	papiWriteScenario_bool(scn, "K26", K26);
	papiWriteScenario_bool(scn, "K27", K27);
	papiWriteScenario_bool(scn, "K28", K28);

	oapiWriteLine(scn, "DECA_END");
}

void DECA::LoadState(FILEHANDLE scn) {

	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "DECA_END", sizeof("DECA_END"))) {
			return;
		}

		papiReadScenario_int(line, "PITCHACTUATORCOMMAND", pitchactuatorcommand);
		papiReadScenario_int(line, "ROLLACTUATORCOMMAND", rollactuatorcommand);
		papiReadScenario_double(line, "AUTOTHRUST", AutoThrust);
		papiReadScenario_double(line, "LGCAUTOTHRUST", lgcAutoThrust);
		papiReadScenario_bool(line, "K1", K1);
		papiReadScenario_bool(line, "K2", K2);
		papiReadScenario_bool(line, "K3", K3);
		papiReadScenario_bool(line, "K4", K4);
		papiReadScenario_bool(line, "K5", K5);
		papiReadScenario_bool(line, "K6", K6);
		papiReadScenario_bool(line, "K7", K7);
		papiReadScenario_bool(line, "K8", K8);
		papiReadScenario_bool(line, "K9", K9);
		papiReadScenario_bool(line, "K10", K10);
		papiReadScenario_bool(line, "K13", K13);
		papiReadScenario_bool(line, "K14", K14);
		papiReadScenario_bool(line, "K15", K15);
		papiReadScenario_bool(line, "K16", K16);
		papiReadScenario_bool(line, "K21", K21);
		papiReadScenario_bool(line, "K22", K22);
		papiReadScenario_bool(line, "K23", K23);
		papiReadScenario_bool(line, "K24", K24);
		papiReadScenario_bool(line, "K25", K25);
		papiReadScenario_bool(line, "K26", K26);
		papiReadScenario_bool(line, "K27", K27);
		papiReadScenario_bool(line, "K28", K28);
	}
}

GASTA::GASTA()
{
	imu_att = _V(0, 0, 0);
	gasta_att = _V(0, 0, 0);

	heat_load = NULL;
}

void GASTA::Init(LEM *v, e_object *dcsource, e_object *acsource, h_HeatLoad *hl, IMU* imu) {
	// Initialize
	lem = v;
	dc_source = dcsource;
	ac_source = acsource;
	heat_load = hl;
	this->imu = imu;
}

bool GASTA::IsPowered()
{
	if (ac_source && dc_source) {
		if (ac_source->Voltage() > SP_MIN_ACVOLTAGE && dc_source->Voltage() > SP_MIN_DCVOLTAGE)
		{
			return true;
		}
	}
	return false;
}

void GASTA::Timestep(double simt)
{
	if (lem == NULL) return;

	if (!IsPowered())
	{
		gasta_att = _V(0, 0, 0);	//I guess the FDAI would show all zeros when no attitude signal is supplied? It's not like turning off BMAG power in the CSM. There the attitude freezes.
		return;
	}

	//This is all I do. P.S. Now I do more!
	imu_att = imu->GetTotalAttitude();

	gasta_att.z = asin(-cos(imu_att.z)*sin(imu_att.x));
	if (abs(sin(gasta_att.z)) != 1.0)
	{
		gasta_att.y = atan2(((sin(imu_att.y)*cos(imu_att.x) + cos(imu_att.y)*sin(imu_att.z)*sin(imu_att.x)) / cos(gasta_att.z)), (cos(imu_att.y)*cos(imu_att.x) - sin(imu_att.y)*sin(imu_att.z)*sin(imu_att.x)) / cos(gasta_att.z));
	}

	if (abs(sin(gasta_att.z)) != 1.0)
	{
		gasta_att.x = atan2(sin(imu_att.z), cos(imu_att.z)*cos(imu_att.x));
	}

	//Map angles between 0° and 360°, just to be sure
	if (gasta_att.x < 0)
	{
		gasta_att.x += PI2;
	}
	if (gasta_att.y < 0)
	{
		gasta_att.y += PI2;
	}
	if (gasta_att.z < 0)
	{
		gasta_att.z += PI2;
	}

	//sprintf(oapiDebugString(), "OGA: %f, IGA: %f, MGA: %f, Roll: %f, Pitch: %f, Yaw: %f", imu_att.x*DEG, imu_att.y*DEG, imu_att.z*DEG, gasta_att.x*DEG, gasta_att.y*DEG, gasta_att.z*DEG);
}

void GASTA::SystemTimestep(double simdt)
{
	if (IsPowered())
	{
		if (ac_source)
		{
			ac_source->DrawPower(10); //10 Watts from AC BUS A
		}
		if (dc_source)
		{
			dc_source->DrawPower(7.8); //7.8 Watts from CDR DC BUS
		}
		if (heat_load)
			heat_load->GenerateHeat(25.9 + 12.5);
	}
}

SCCA1::SCCA1() :
	AbortStageDelay(0.4)
{
	K1 = false;
	K3 = false;
	K5 = false;
	K7 = false;
	K8 = false;
	K9 = false;
	K10 = false;
	K11 = false;
	K12 = false;
	K13 = false;
	K14 = false;
	K15 = false;
	K16 = false;
	K17 = false;
	K18 = false;
	K19 = false;
	K20 = false;
	K21 = false;
	K22 = false;
	K23 = false;
	K24 = false;
	K25 = false;
	K201 = false;
	K203 = false;
	K204 = false;
	K205 = false;
	K206 = false;
	K207 = false;
	AutoOn = false;
}

void SCCA1::Init(LEM *s)
{
	lem = s;
}

void SCCA1::Timestep(double simdt)
{
	if (lem == NULL) { return; }

	AbortStageDelay.Timestep(simdt);

	if (lem->SCS_ATT_DIR_CONT_CB.IsPowered() && !lem->YawSwitch.IsUp())
	{
		K1 = true;
	}
	else
	{
		K1 = false;
	}

	if (lem->SCS_ATT_DIR_CONT_CB.IsPowered() && !lem->PitchSwitch.IsUp())
	{
		K3 = true;
	}
	else
	{
		K3 = false;
	}

	if (lem->SCS_ATT_DIR_CONT_CB.IsPowered() && !lem->RollSwitch.IsUp())
	{
		K5 = true;
	}
	else
	{
		K5 = false;
	}

	if (lem->SCS_ATCA_AGS_CB.IsPowered() && lem->CDR_ACA.GetOutOfDetent())
	{
		K7 = true;
	}
	else
	{
		K7 = false;
	}

	if (lem->CDR_SCS_ATCA_CB.IsPowered() && lem->CDR_ACA.GetOutOfDetent())
	{
		K8 = true;
	}
	else
	{
		K8 = false;
	}

	//Abort Stage Handling

	if (lem->SCS_ABORT_STAGE_CB.IsPowered() && lem->AbortStageSwitch.GetState() == 0)
	{
		K9 = true;
		K10 = true;
		AbortStageDelay.SetRunning(true);
		if (AbortStageDelay.ContactClosed())
		{
			K23 = true;
		}
		else
		{
			K23 = false;
		}
	}
	else
	{
		K9 = false;
		K10 = false;
		K23 = false;
	}

	if (lem->CDR_SCS_ABORT_STAGE_CB.IsPowered() && lem->AbortStageSwitch.GetState() == 0)
	{
		K19 = true;
		K20 = true;
		K21 = true;
		K201 = true;
	}
	else
	{
		K19 = false;
		K20 = false;
		K21 = false;
		K201 = false;
	}

	if (lem->EngineArmSwitch.IsUp() && lem->SCS_ENG_ARM_CB.IsPowered())
	{
		K22 = true;
		K206 = true;
	}
	else
	{
		K22 = false;
		K206 = false;
	}

	//Automatic

	if (lem->SCS_ENG_ARM_CB.IsPowered())
	{
		K18 = lem->scca2.GetAutoEngOn();
		K24 = K18;
		K17 = lem->scca2.GetAutoEngOff();
		K25 = K17;
	}
	else
	{
		K17 = false;
		K18 = false;
		K24 = false;
		K25 = false;
	}

	//Ascent Engine Logic Circuit
	//ON when:
	//EngineOn = 1, EngineOff = 0
	//EngineOn = 0, EngineOff = 0, AutoOn = 1, Engine Armed or Abort Stage
	//OFF when:
	//Any other case

	if ((K24 && !K25) || ((AutoOn && (K22 || K23)) && (K24 && K25 || !K24 && !K25)))
	{
		AutoOn = true;
	}
	else
	{
		AutoOn = false;
	}

	//Manual

	if ((lem->PROP_DISP_ENG_OVRD_LOGIC_CB.IsPowered() || lem->SCS_ENG_START_OVRD_CB.IsPowered()) && lem->EngineArmSwitch.IsUp() && lem->scca2.GetK19())
	{
		K11 = true;
		K12 = true;
	}
	else
	{
		K11 = false;
		K12 = false;
	}

	if (K206 && K22 && lem->EngineArmSwitch.IsUp() && lem->SCS_ENG_ARM_CB.IsPowered())
	{
		K13 = true;
	}
	else if (!K206 && !K22 && K23 && lem->AbortStageSwitch.GetState() == 0 && lem->SCS_ABORT_STAGE_CB.IsPowered())
	{
		K13 = true;
	}
	else
	{
		K13 = false;
	}

	if (AutoOn && lem->CDRManualEngineStop.GetState() == 0 && lem->LMPManualEngineStop.GetState() == 0 && lem->EngineArmSwitch.IsUp())
	{
		K14 = true;
	}
	else if (AutoOn && K21)
	{
		K14 = true;
	}
	else
	{
		K14 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->deca.GetK16())
	{
		K15 = true;
	}
	else
	{
		K15 = false;
	}

	if (K14 && (lem->SCS_AELD_CB.IsPowered() || lem->SCS_ENG_ARM_CB.IsPowered()))
	{
		K207 = true;
	}
	else
	{
		K207 = false;
	}

	if (lem->SCS_AELD_CB.IsPowered() && ((K11 && K12) || (K13 && K207)))
	{
		lem->eds.RelayBoxB.SetStagingRelay();
		thrustOn = true;
		K204 = true;
		K205 = true;
	}
	else if (lem->CDR_SCS_AELD_CB.IsPowered() && ((K11 && K207) || (K13 && K12)))
	{
		lem->eds.RelayBoxB.SetStagingRelay();
		thrustOn = true;
		K204 = true;
		K205 = true;
	}
	else
	{
		thrustOn = false;
		K204 = false;
		K205 = false;
	}

	if (lem->SCS_AELD_CB.IsPowered() && ((K11 && K207) || (K13 && K12)))
	{
		lem->eds.RelayBoxA.SetStagingRelay();
		armedOn = true;
		K16 = true;
		K203 = true;
	}
	else if (lem->CDR_SCS_AELD_CB.IsPowered() && ((K11 && K12) || (K13 && K207)))
	{
		lem->eds.RelayBoxA.SetStagingRelay();
		armedOn = true;
		K16 = true;
		K203 = true;
	}
	else
	{
		armedOn = false;
		K16 = false;
		K203 = false;
	}

	//AEA Ascent Engine On
	if (K16 || K205)
	{
		lem->aea.SetInputPortBit(IO_2020, AGSAscentEngineOnDiscrete, false);
	}
	else
	{
		lem->aea.SetInputPortBit(IO_2020, AGSAscentEngineOnDiscrete, true);
	}

	//Start LGC Abort Stage
	if (K10 || K21)
	{
		lem->agc.SetInputChannelBit(030, AbortWithAscentStage, true);
	}
	else
	{
		lem->agc.SetInputChannelBit(030, AbortWithAscentStage, false);
	}

	//Start AEA Abort Stage
	if (K9 || K201)
	{
		lem->aea.SetInputPortBit(IO_2020, AGSAbortStageDiscrete, false);
	}
	else
	{
		lem->aea.SetInputPortBit(IO_2020, AGSAbortStageDiscrete, true);
	}

	//Send engine fire commands to APS

	lem->APS.armedOn = armedOn;
	lem->APS.thrustOn = thrustOn;

	//sprintf(oapiDebugString(), "Manual: K19 %d K22 %d K11 %d K12 %d K13 %d K14 %d", K19, K22, K11, K12, K13, K14);
	//sprintf(oapiDebugString(), "Auto: K13 %d K207 %d K206 %d K14 %d K24 %d K25 %d AutoOn %d", K13, K207, K206, K14, K24, K25, AutoOn);
	//sprintf(oapiDebugString(), "Abort: K21 %d K23 %d K13 %d K14 %d AutoOn %d armedOn %d thrustOn %d", K21, K23, K13, K14, AutoOn, armedOn, thrustOn);
}

void SCCA1::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOON", AutoOn);
	papiWriteScenario_bool(scn, "ARMEDON", armedOn);
	papiWriteScenario_bool(scn, "THRUSTON", thrustOn);
	papiWriteScenario_bool(scn, "K1", K1);
	papiWriteScenario_bool(scn, "K3", K3);
	papiWriteScenario_bool(scn, "K5", K5);
	papiWriteScenario_bool(scn, "K7", K7);
	papiWriteScenario_bool(scn, "K8", K8);
	papiWriteScenario_bool(scn, "K9", K9);
	papiWriteScenario_bool(scn, "K10", K10);
	papiWriteScenario_bool(scn, "K11", K11);
	papiWriteScenario_bool(scn, "K12", K12);
	papiWriteScenario_bool(scn, "K13", K13);
	papiWriteScenario_bool(scn, "K14", K14);
	papiWriteScenario_bool(scn, "K15", K15);
	papiWriteScenario_bool(scn, "K16", K16);
	papiWriteScenario_bool(scn, "K17", K17);
	papiWriteScenario_bool(scn, "K18", K18);
	papiWriteScenario_bool(scn, "K19", K19);
	papiWriteScenario_bool(scn, "K20", K20);
	papiWriteScenario_bool(scn, "K21", K21);
	papiWriteScenario_bool(scn, "K22", K22);
	papiWriteScenario_bool(scn, "K23", K23);
	papiWriteScenario_bool(scn, "K24", K24);
	papiWriteScenario_bool(scn, "K25", K25);
	papiWriteScenario_bool(scn, "K201", K201);
	papiWriteScenario_bool(scn, "K203", K203);
	papiWriteScenario_bool(scn, "K204", K204);
	papiWriteScenario_bool(scn, "K205", K205);
	papiWriteScenario_bool(scn, "K206", K206);
	papiWriteScenario_bool(scn, "K207", K207);

	oapiWriteLine(scn, end_str);
}

void SCCA1::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "AUTOON", AutoOn);
		papiReadScenario_bool(line, "ARMEDON", armedOn);
		papiReadScenario_bool(line, "THRUSTON", thrustOn);
		papiReadScenario_bool(line, "K1", K1);
		papiReadScenario_bool(line, "K3", K3);
		papiReadScenario_bool(line, "K5", K5);
		papiReadScenario_bool(line, "K7", K7);
		papiReadScenario_bool(line, "K8", K8);
		papiReadScenario_bool(line, "K9", K9);
		papiReadScenario_bool(line, "K10", K10);
		papiReadScenario_bool(line, "K11", K11);
		papiReadScenario_bool(line, "K12", K12);
		papiReadScenario_bool(line, "K13", K13);
		papiReadScenario_bool(line, "K14", K14);
		papiReadScenario_bool(line, "K15", K15);
		papiReadScenario_bool(line, "K16", K16);
		papiReadScenario_bool(line, "K17", K17);
		papiReadScenario_bool(line, "K18", K18);
		papiReadScenario_bool(line, "K19", K19);
		papiReadScenario_bool(line, "K20", K20);
		papiReadScenario_bool(line, "K21", K21);
		papiReadScenario_bool(line, "K22", K22);
		papiReadScenario_bool(line, "K23", K23);
		papiReadScenario_bool(line, "K24", K24);
		papiReadScenario_bool(line, "K25", K25);
		papiReadScenario_bool(line, "K201", K201);
		papiReadScenario_bool(line, "K203", K203);
		papiReadScenario_bool(line, "K204", K204);
		papiReadScenario_bool(line, "K205", K205);
		papiReadScenario_bool(line, "K206", K206);
		papiReadScenario_bool(line, "K207", K207);

	}
}

SCCA2::SCCA2()
{
	K1 = false;
	K2 = false;
	K3 = false;
	K4 = false;
	K5 = false;
	K6 = false;
	K7 = false;
	K8 = false;
	K9 = false;
	K11 = false;
	K12 = false;
	K13 = false;
	K14 = false;
	K15 = false;
	K16 = false;
	K17 = false;
	K19 = false;
	K22 = false;
	K23 = false;
	K24 = false;
	AutoEngOn = false;
	AutoEngOff = false;
}

void SCCA2::Init(LEM *s)
{
	lem = s;
}

void SCCA2::Timestep(double simdt)
{
	if (lem == NULL) { return; }

	if (lem->CDR_SCS_ATCA_CB.IsPowered() && lem->GuidContSwitch.IsUp())
	{
		K1 = false;
		K2 = false;
		K3 = false;
		K4 = false;
		K5 = false;
		K6 = false;
		K7 = false;
		K8 = false;
		K9 = false;
		K11 = false;
		K12 = false;
		K13 = false;
	}
	if (lem->SCS_ATCA_AGS_CB.IsPowered() && lem->GuidContSwitch.IsDown())
	{
		K1 = true;
		K2 = true;
		K3 = true;
		K4 = true;
		K5 = true;
		K6 = true;
		K7 = true;
		K8 = true;
		K9 = true;
		K11 = true;
		K12 = true;
		K13 = true;
	}

	if (lem->SCS_ENG_START_OVRD_CB.IsPowered() && lem->ManualEngineStart.GetState() == 1)
	{
		K15 = true;
		K19 = true;
		K22 = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && (lem->CDRManualEngineStop.GetState() == 1 || lem->LMPManualEngineStop.GetState() == 1))
	{
		K15 = false;
		K19 = false;
		K22 = false;
	}

	if (lem->SCS_ENG_ARM_CB.IsPowered() && !lem->EngineArmSwitch.IsCenter())
	{
		K14 = true;
	}
	else
	{
		K14 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->deca.GetK16())
	{
		K16 = true;
	}
	else
	{
		K16 = false;
	}

	if (K16 && lem->SCS_ENG_CONT_CB.IsPowered() && lem->deca.GetK16() && lem->GroundContact())
	{
		K17 = true;
	}
	else
	{
		K17 = false;
	}

	if (K7)
	{
		lem->agc.SetInputChannelBit(030, GNControlOfSC, false);
	}
	else
	{
		lem->agc.SetInputChannelBit(030, GNControlOfSC, true);
	}

	if (K14)
	{
		lem->agc.SetInputChannelBit(030, EngineArmed, true);
	}
	else
	{
		lem->agc.SetInputChannelBit(030, EngineArmed, false);
	}

	ChannelValue val11;
	AGSChannelValue40 agsval40;

	val11 = lem->agc.GetOutputChannel(011);
	agsval40 = lem->aea.GetOutputChannel(IO_ODISCRETES);

	if (K8)
	{
		AutoEngOn = ~agsval40[AGSEngineOn];
	}
	else
	{
		AutoEngOn = val11[EngineOn];
	}

	if (K9)
	{
		if (lem->ModeControlAGSSwitch.IsUp())
		{
			AutoEngOff = ~agsval40[AGSEngineOff];
		}
		else
		{
			AutoEngOff = false;
		}
	}
	else
	{
		if (lem->ModeControlPGNSSwitch.IsUp())
		{
			AutoEngOff = val11[EngineOff];
		}
		else
		{
			AutoEngOff = false;
		}
	}

	//TBD: K23 and K24 are only used by GSE
	K23 = false;
	K24 = false;

	if (lem->stage > 1 || (K23 && K24))
	{
		tempsignal = false;
	}
	else
	{
		tempsignal = true;
	}
	if (lem->pMission->IsLMStageBitInverted()) tempsignal = !tempsignal;
	lem->agc.SetInputChannelBit(030, DescendStageAttached, tempsignal);
}

void SCCA2::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "AUTOENGON", AutoEngOn);
	papiWriteScenario_bool(scn, "AUTOENGOFF", AutoEngOff);
	papiWriteScenario_bool(scn, "K1", K1);
	papiWriteScenario_bool(scn, "K2", K2);
	papiWriteScenario_bool(scn, "K3", K3);
	papiWriteScenario_bool(scn, "K4", K4);
	papiWriteScenario_bool(scn, "K5", K5);
	papiWriteScenario_bool(scn, "K6", K6);
	papiWriteScenario_bool(scn, "K7", K7);
	papiWriteScenario_bool(scn, "K8", K8);
	papiWriteScenario_bool(scn, "K9", K9);
	papiWriteScenario_bool(scn, "K11", K11);
	papiWriteScenario_bool(scn, "K12", K12);
	papiWriteScenario_bool(scn, "K13", K13);
	papiWriteScenario_bool(scn, "K14", K14);
	papiWriteScenario_bool(scn, "K15", K15);
	papiWriteScenario_bool(scn, "K16", K16);
	papiWriteScenario_bool(scn, "K17", K17);
	papiWriteScenario_bool(scn, "K19", K19);
	papiWriteScenario_bool(scn, "K22", K22);
	papiWriteScenario_bool(scn, "K23", K23);
	papiWriteScenario_bool(scn, "K24", K24);

	oapiWriteLine(scn, end_str);
}

void SCCA2::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "AUTOENGON", AutoEngOn);
		papiReadScenario_bool(line, "AUTOENGOFF", AutoEngOff);
		papiReadScenario_bool(line, "K1", K1);
		papiReadScenario_bool(line, "K2", K2);
		papiReadScenario_bool(line, "K3", K3);
		papiReadScenario_bool(line, "K4", K4);
		papiReadScenario_bool(line, "K5", K5);
		papiReadScenario_bool(line, "K6", K6);
		papiReadScenario_bool(line, "K7", K7);
		papiReadScenario_bool(line, "K8", K8);
		papiReadScenario_bool(line, "K9", K9);
		papiReadScenario_bool(line, "K11", K11);
		papiReadScenario_bool(line, "K12", K12);
		papiReadScenario_bool(line, "K13", K13);
		papiReadScenario_bool(line, "K14", K14);
		papiReadScenario_bool(line, "K15", K15);
		papiReadScenario_bool(line, "K16", K16);
		papiReadScenario_bool(line, "K17", K17);
		papiReadScenario_bool(line, "K19", K19);
		papiReadScenario_bool(line, "K22", K22);
		papiReadScenario_bool(line, "K23", K23);
		papiReadScenario_bool(line, "K24", K24);

	}
}

SCCA3::SCCA3()
{
	ResetRelays();
}

void SCCA3::ResetRelays()
{
	EngStopPower = false;
	K1_1 = false;
	K2_1 = false;
	K3_1 = false;
	K4_1 = false;
	K5_1 = false;
	K6_1 = false;
	K1_2 = false;
	K2_2 = false;
	K3_2 = false;
	K4_2 = false;
	K5_2 = false;
	K6_2 = false;
	K7_3 = false;
}

void SCCA3::Init(LEM *s)
{
	lem = s;
}

void SCCA3::Timestep(double simdt)
{
	if (lem == NULL) { return; }

	if (lem->stage > 1)
	{
		ResetRelays();
		return;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->deca.GetK16())
	{
		K7_3 = true;
	}
	else
	{
		K7_3 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->AbortSwitch.GetState() == 1 && lem->EngineArmSwitch.IsDown())
	{
		EngStopPower = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->AbortSwitch.GetState() == 0)
	{
		EngStopPower = true;
	}
	else if (lem->SCS_ENG_ARM_CB.IsPowered() && lem->EngineArmSwitch.IsDown())
	{
		EngStopPower = true;
	}
	else
	{
		EngStopPower = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && lem->EngineArmSwitch.IsUp() && lem->scca1.GetK20())
	{
		K4_1 = true;
	}
	else if (lem->SCS_ENG_CONT_CB.IsPowered() && (lem->CDRManualEngineStop.GetState() == 1 || lem->LMPManualEngineStop.GetState() == 1))
	{
		K4_1 = true;
	}
	else
	{
		K4_1 = false;
	}

	if (EngStopPower && (lem->CDRManualEngineStop.GetState() == 1 || lem->LMPManualEngineStop.GetState() == 1))
	{
		K4_2 = true;
	}
	else
	{
		K4_2 = false;
	}

	if (EngStopPower && lem->CDRManualEngineStop.GetState() == 0 && lem->LMPManualEngineStop.GetState() == 0 && !lem->scca1.GetK9())
	{
		K6_1 = true;
		K6_2 = true;
	}
	else
	{
		K6_1 = false;
		K6_2 = false;
	}

	if (lem->SCS_ABORT_STAGE_CB.IsPowered() && lem->AbortStageSwitch.GetState() == 1 && lem->EngineDescentCommandOverrideSwitch.IsUp() && !K4_1 && !K4_2)
	{
		K5_1 = true;
	}
	else
	{
		K5_1 = false;
	}

	if ((lem->PROP_DISP_ENG_OVRD_LOGIC_CB.IsPowered() || lem->SCS_ENG_START_OVRD_CB.IsPowered()) && lem->EngineArmSwitch.IsDown() && lem->EngineDescentCommandOverrideSwitch.IsUp() && !K4_1 && !K4_2)
	{
		K5_2 = true;
	}
	else
	{
		K5_2 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && K5_1)
	{
		K1_1 = true;
	}
	else
	{
		K1_1 = false;
	}

	if (lem->SCS_ATCA_CB.IsPowered() && K5_2)
	{
		K1_2 = true;
	}
	else
	{
		K1_2 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && (K7_3 || K1_1))
	{
		K2_1 = true;
	}
	else
	{
		K2_1 = false;
	}

	if (lem->SCS_ATCA_CB.IsPowered() && (K7_3 || K1_2))
	{
		K2_2 = true;
	}
	else
	{
		K2_2 = false;
	}

	if (lem->SCS_ENG_CONT_CB.IsPowered() && (K7_3 || K1_1) && lem->GroundContact())
	{
		K3_1 = true;
	}
	else
	{
		K3_1 = false;
	}

	if (lem->SCS_ATCA_CB.IsPowered() && (K7_3 || K1_2) && lem->GroundContact())
	{
		K3_2 = true;
	}
	else
	{
		K3_2 = false;
	}

	//sprintf(oapiDebugString(), "DE Command Override: K4 %d %d K5 %d %d K6 %d %d", K4_1, K4_2, K5_1, K5_2, K6_1, K6_2);
}

void SCCA3::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "K1_1", K1_1);
	papiWriteScenario_bool(scn, "K2_1", K2_1);
	papiWriteScenario_bool(scn, "K3_1", K3_1);
	papiWriteScenario_bool(scn, "K4_1", K4_1);
	papiWriteScenario_bool(scn, "K5_1", K5_1);
	papiWriteScenario_bool(scn, "K6_1", K6_1);
	papiWriteScenario_bool(scn, "K1_2", K1_2);
	papiWriteScenario_bool(scn, "K2_2", K2_2);
	papiWriteScenario_bool(scn, "K3_2", K3_2);
	papiWriteScenario_bool(scn, "K4_2", K4_2);
	papiWriteScenario_bool(scn, "K5_2", K5_2);
	papiWriteScenario_bool(scn, "K6_2", K6_2);
	papiWriteScenario_bool(scn, "K7_3", K7_3);

	oapiWriteLine(scn, end_str);
}

void SCCA3::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "K1_1", K1_1);
		papiReadScenario_bool(line, "K2_1", K2_1);
		papiReadScenario_bool(line, "K3_1", K3_1);
		papiReadScenario_bool(line, "K4_1", K4_1);
		papiReadScenario_bool(line, "K5_1", K5_1);
		papiReadScenario_bool(line, "K6_1", K6_1);
		papiReadScenario_bool(line, "K1_2", K1_2);
		papiReadScenario_bool(line, "K2_2", K2_2);
		papiReadScenario_bool(line, "K3_2", K3_2);
		papiReadScenario_bool(line, "K4_2", K4_2);
		papiReadScenario_bool(line, "K5_2", K5_2);
		papiReadScenario_bool(line, "K6_2", K6_2);
		papiReadScenario_bool(line, "K7_3", K7_3);
	}
}