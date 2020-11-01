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

#include "DelayTimer.h"

class LEM;

class LEM_RGA {
	// RATE GYRO ASSEMBLY
public:
	LEM_RGA();
	void Init(LEM *v, e_object *dcsource, h_HeatLoad *hl, h_HeatLoad *sechl);
	void Timestep(double simdt);
	void SystemTimestep(double simdt);

	VECTOR3 GetRates() { return rates; }
	double GetPitchRateVRMS() { return rates.x * 0.14*DEG; }
	double GetYawRateVRMS() { return rates.y * 0.14*DEG; }
	double GetRollRateVRMS() { return rates.z * 0.14*DEG; }
protected:
	bool powered;
	VECTOR3 rates;

	e_object *dc_source;
	LEM *lem;
	h_HeatLoad *RGAHeat;
	h_HeatLoad *SecRGAHeat;
};

#define ATCA_START_STRING	"ATCA_BEGIN"
#define ATCA_END_STRING		"ATCA_END"

class ATCA {
	// ATTITUDE & TRANSLATION CONTROL ASSEMBLY
public:
	ATCA();								// Cons
	void Init(LEM *vessel, h_HeatLoad *hl, h_HeatLoad *sechl);				// Init
	double GetPrimPowerVoltage();
	double GetBackupPowerVoltage();
	double GetRGAPickoffExcitationVoltage();
	double GetRGASpinMotorVoltage();
	double GetPlus15VDCSupplyVoltage();
	double GetMinus15VDCSupplyVoltage();
	double GetPlus43VDCSupplyVoltage();
	double GetPlus6VDCSupplyVoltage();
	double GetMinus6VDCSupplyVoltage();
	void Timestep(double simt, double simdt);			// Timestep
	void SystemTimestep(double simdt);
	void ProcessLGC(int ch, int val);   // To process LGC commands

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	double GetDPSPitchGimbalError();
	double GetDPSRollGimbalError();
	double GetPitchAttenuatorErrorVRMS() { return aea_attitude_error.y * 3.5 / 15.0; }
	double GetYawAttenuatorErrorVRMS() { return aea_attitude_error.z * 3.5 / 15.0; }
	double GetRollAttenuatorErrorVRMS() { return aea_attitude_error.x * 3.5 / 15.0; }
	double GetXTransCommandVoltage() { return translationCommands.x; }
	double GetYTransCommandVoltage() { return translationCommands.y; }
	double GetZTransCommandVoltage() { return translationCommands.z; }
	double GetPitchLogicInputErrorVoltage() { return thrustLogicInputError.y; }
	double GetYawLogicInputErrorVoltage() { return thrustLogicInputError.z; }
	double GetRollLogicInputErrorVoltage() { return thrustLogicInputError.x; }

	LEM *lem;
	h_HeatLoad *ATCAHeat;
	h_HeatLoad *SECATCAHeat;
	int lgc_err_x,lgc_err_y,lgc_err_z;	// LGC attitude error counters
	int lgc_err_ena;                    // LGC error counter enabled
	int jet_request[16];				// Jet request list
	int jet_last_request[16];			// Jet request list at last timestep
	double jet_start[16],jet_stop[16];  // RCS jet start/stop times

protected:

	double PRMDutyRatio(double volt);
	double PRMPulseWidth(double volt);
	bool PRMTimestep(int n, double simdt, double pp, double pw);
	void Limiter(double &val, double lim);

	VECTOR3 aea_attitude_error;
	VECTOR3 aca_rates;
	VECTOR3 att_rates;
	VECTOR3 thrustLogicInputError;	//Inputs to the summing amplifiers
	VECTOR3 translationCommands;
	double SummingAmplifierOutput[8];
	bool PRMPulse[8];
	double PRMCycleTime[8];
	double PRMOffTime[8];
	bool hasAbortPower;
	bool hasPrimPower;
	VECTOR3 ACARateGain;
	VECTOR3 RateGain;
	VECTOR3 DeadbandGain;
	double pitchGimbalError;
	double rollGimbalError;

	//Relays:

	//Zero Cross Detector
	bool K1;
	//Zero Cross Detector
	bool K2;
	//Zero Cross Detector
	bool K3;
	//Miss Select Yaw (Cmd Rate)
	bool K8;
	//Miss Select Pitch (Cmd Rate)
	bool K9;
	//Miss Select Roll (Cmd Rate)
	bool K10;
	//Miss Select Yaw (Limiter)
	bool K11;
	//Miss Select Pitch (Limiter)
	bool K12;
	//Miss Select Roll (Limiter)
	bool K13;
	//Dead Band Select Yaw
	bool K14;
	//Dead Band Select Pitch
	bool K15;
	//Dead Band Select Roll
	bool K16;
	//Pulse Mode Yaw
	bool K19;
	//Pulse Mode Pitch
	bool K20;
	//Pulse Mode Roll
	bool K21;
};

class DECA {
	// DESCENT ENGINE CONTROL ASSEMBLY
public:
	DECA();										// Cons
	void Init(LEM *vessel, e_object *dcbus);	// Init
	void ResetRelays();
	void Timestep(double simdt);				// Timestep
	void SystemTimestep(double simdt);			// System Timestep
	void ProcessLGCThrustCommands(int val);

	bool GetK1() { return K1; }
	bool GetK10() { return K10; }
	bool GetK16() { return K16; }
	bool GetK21() { return K21; }
	bool GetK22() { return K22; }
	bool GetK23() { return K23; }
	bool GetK25() { return K25; }
	bool GetThrustOn() { return engOn; }
	bool GetEngArm();
	double GetAutoThrustVoltage() { return AutoThrust; }
	double GetManualThrustVoltage() { return ManualThrust * 14.6; }

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	LEM *lem;
	e_object *dc_source;			     // DC source to use when powered
	bool powered;					 // Data valid flag.
	int pitchactuatorcommand, rollactuatorcommand;
	bool engOn, DEArm;
	double AutoThrust, ManualThrust;
	//Voltage, 0-12V
	double lgcAutoThrust;

	//Relays

	//Descent Engine Arm
	bool K1;
	//+43 +/-15V Power Supply Failure
	bool K2;
	//Descent Engine Manual Stop
	bool K3;
	//Pitch Gimbal Off
	bool K4;
	//Roll Gimbal Off
	bool K5;
	//Auto Descent Control Latch
	bool K6;
	//Descent Engine Manual Start
	bool K7;
	//Pitch Trim Shutdown
	bool K8;
	//Roll Trim Shutdown
	bool K9;
	//Arm Pre-Valves
	bool K10;
	//Pitch Trim Mode
	bool K13;
	//Roll Trim Mode
	bool K14;
	//Auto Thrust Disable
	bool K15;
	//Close On Descent Engine Start
	bool K16;
	//Pitch Trim Failure
	bool K21;
	//Roll Trim Failure
	bool K22;
	//Descent Engine Arm
	bool K23;
	//+15V Power Supply Failure
	bool K24;
	//400 Hz Power
	bool K25;
	//Power Supply Failure
	bool K26;
	//Trim Malfunction Reset
	bool K27;
	//Auto Descent Engine Control
	bool K28;

private:
	double LMR, dposcmd, dpos, poscmdsign;
};

class GASTA {
	//GIMBAL ANGLE SEQUENCING TRANSFORMATION ASSEMBLY
public:
	GASTA();
	void Init(LEM *s, e_object *dcsource, e_object *acsource, h_HeatLoad *hl, IMU* imu);
	void Timestep(double simt);				// Timestep
	void SystemTimestep(double simdt);		// System Timestep
	bool IsPowered();
	VECTOR3 GetTotalAttitude() { return gasta_att; }

	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	LEM *lem;
	e_object *dc_source;			     // DC source to use when powered
	e_object *ac_source;			     // AC source to use when powered
	h_HeatLoad *heat_load;
	IMU *imu;							 //Connected IMU
	VECTOR3 imu_att;
	VECTOR3 gasta_att;
};

//STABILIZATION & CONTROL CONTROL ASSEMBLY NO. 1
class SCCA1
{
public:
	SCCA1();
	void Init(LEM *s);
	void Timestep(double simdt);

	bool GetK1() { return K1; }
	bool GetK3() { return K3; }
	bool GetK5() { return K5; }
	bool GetK7() { return K7; }
	bool GetK8() { return K8; }
	bool GetK9() { return K9; }
	bool GetK15() { return K15; }
	bool GetK17() { return K17; }
	bool GetK18() { return K18; }
	bool GetK19() { return K19; }
	bool GetK20() { return K20; }
	bool GetK203() { return K203; }
	bool GetK204() { return K204; }
	bool GetThrustOn() { return thrustOn; }

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
protected:

	DelayTimer AbortStageDelay;

	//Relays

	//Yaw Pulse Direct Cont.
	bool K1;
	//Pitch Pulse Direct Cont.
	bool K3;
	//Roll Pulse Direct Cont.
	bool K5;
	//PGNS-AGS Sel Follow Up
	bool K7;
	//Controller Out Of Detent
	bool K8;
	//PGNS-AGS Sel. Abort Stage
	bool K9;
	//Abort Stage
	bool K10;
	//PGNS-AGS Sel.
	bool K11;
	//28VDC PGNS Preamp in ATCA
	bool K12;
	//Abort Stage Cmd
	bool K13;
	//Ascent Engine Arm
	bool K14;
	//Descent Engine Thrust
	bool K15;
	//Ascent Engine On
	bool K16;
	//Auto Engine Off Cmd
	bool K17;
	//Auto Engine On Cmd
	bool K18;
	//Abort Stage Cmd
	bool K19;
	//Power Switchover & Descent Engine Stop
	bool K20;
	//Abort Stage Cmd
	bool K21;
	//AELD Reset
	bool K22;
	//Abort Stage Cmd
	bool K23;
	//Auto Ascent Engine On Cmd
	bool K24;
	//Auto Ascent Engine Off Cmd
	bool K25;
	//Abort Stage Cmd
	bool K201;
	//Ascent Engine On Cmd
	bool K203;
	//Ascent Engine On Cmd
	bool K204;
	//Ascent Engine On Cmd
	bool K205;
	//Engine Arm
	bool K206;
	//Abort or Auto Start
	bool K207;

	bool AutoOn;
	bool armedOn;
	bool thrustOn;

	LEM *lem;
};

//STABILIZATION & CONTROL CONTROL ASSEMBLY NO. 2
class SCCA2
{
public:
	SCCA2();
	void Init(LEM *s);
	void Timestep(double simdt);

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	bool GetAutoEngOn() { return AutoEngOn; }
	bool GetAutoEngOff() { return AutoEngOff; }
	bool GetK2() { return K2; }
	bool GetK3() { return K3; }
	bool GetK4() { return K4; }
	bool GetK5() { return K5; }
	bool GetK7() { return K7; }
	bool GetK8() { return K8; }
	bool GetK11() { return K11; }
	bool GetK12() { return K12; }
	bool GetK13() { return K13; }
	bool GetK15() { return K15; }
	bool GetK16() { return K16; }
	bool GetK17() { return K17; }
	bool GetK19() { return K19; }
	bool GetK23() { return K23; }
	bool GetK24() { return K24; }
protected:
	//Latching Relays

	//PGNS-AGS Sel. Cont.
	bool K1;
	//PGNS-AGS Yaw Rate Cmd
	bool K2;
	//PGNS-AGS Pitch Rate Cmd
	bool K3;
	//PGNS-AGS Roll Rate Cmd
	bool K4;
	//800 Hz PGNS-AGS Sel.
	bool K5;
	//PGNS-AGS Sel. Cont.
	bool K6;
	//PGNS-AGS Sel.
	bool K7;
	//PGNS-AGS Sel. Follow Up
	bool K8;
	//PGNS-AGS Sel.
	bool K9;
	//PGNS-AGS Sel.
	bool K11;
	//28VDC PGNS Preamps in ATCA
	bool K12;
	//28VDC PGNS Preamps in ATCA
	bool K13;
	//Engine Fire Override (Set-Reset)
	bool K15;
	//Engine Fire Override (Set-Reset)
	bool K19;
	//Engine Fire Override (Set-Reset)
	bool K22;

	//Non-Latching Relays

	//Engine Armed
	bool K14;
	//Descent Engine On
	bool K16;
	//Descent Engine On
	bool K17;
	//Stimulate Staging Cmd
	bool K23;
	//Stimulate Staging Cmd
	bool K24;

	bool AutoEngOn;
	bool AutoEngOff;
	bool tempsignal;

	LEM *lem;
};

//STABILIZATION & CONTROL CONTROL ASSEMBLY NO. 3
class SCCA3
{
public:
	SCCA3();
	void Init(LEM *s);
	void Timestep(double simdt);
	void ResetRelays();

	bool GetK5() { return (K5_1 && K5_2); }
	bool GetK6() { return (K6_1 || K6_2); }
	bool GetContactLightLogic() { return (K2_1 || K2_2) && (K3_1 || K3_2); }

	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);
protected:
	//Relays

	//Descent Engine Thrusting
	bool K1_1;
	//Descent Engine Thrusting
	bool K2_1;
	//Descent Engine Thrusting
	bool K3_1;
	//Manual Descent Engine Stop
	bool K4_1;
	//Descent Engine Override
	bool K5_1;
	//Descent Engine Start Enable
	bool K6_1;
	//Lunar Probe Y & Z
	bool K1_2;
	//Lunar Probe Y & Z
	bool K2_2;
	//Lunar Probe Y & Z
	bool K3_2;
	//Engine Stop
	bool K4_2;
	//Descent Engine Override
	bool K5_2;
	//Descent Engine Start Enable
	bool K6_2;
	//Descent Engine Thrusting
	bool K7_3;

	bool EngStopPower;

	LEM *lem;

};