/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Saturn Launch Vehicle Data Adapter (Header)

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

#pragma once

#include <bitset>

class IU;

#define SWITCH_SELECTOR_IU 0
#define SWITCH_SELECTOR_SI 1
#define SWITCH_SELECTOR_SII 2
#define SWITCH_SELECTOR_SIVB 3

enum LVDCOutputRegister {

	ResetCommandDecoder = 1,
	RCA110AInterrupt,
	GuidanceReferenceFailureA = 3,
	GuidanceReferenceFailureB = 5,
	FiringCommitEnable = 11,
	FiringCommitInhibit,
};

class LVDA
{
public:
	LVDA();
	void Init(IU* i);
	
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

	void SwitchSelector(int stage, int channel);
	void SetFCCAttitudeError(VECTOR3 atterr);
	VECTOR3 GetLVIMUAttitude();
	void ZeroLVIMUPIPACounters();
	void ZeroLVIMUCDUs();
	void ReleaseLVIMUCDUs();
	void ReleaseLVIMU();
	void DriveLVIMUGimbals(double x, double y, double z);
	VECTOR3 GetLVIMUPIPARegisters();
	bool GetLVIMUFailure();
	bool GetGuidanceReferenceFailure();
	VECTOR3 GetTheodoliteAlignment(double azimuth);

	bool GeneralizedSwitchSelector(int stage, int channel);
	bool TimebaseUpdate(double dt);
	bool LMAbort();
	bool RestartManeuverEnable();
	bool InhibitAttitudeManeuver();
	bool Timebase8Enable();
	bool EvasiveManeuverEnable();
	bool ExecuteCommManeuver();
	bool SIVBIULunarImpact(double tig, double dt, double pitch, double yaw);
	bool LaunchTargetingUpdate(double V_T, double R_T, double theta_T, double inc, double dsc, double dsc_dot, double t_grr0);
	bool NavigationUpdate(VECTOR3 DCSRVEC, VECTOR3 DCSVVEC, double DCSNUPTIM);
	virtual void PrepareToLaunch();

	void SetOutputRegisterBit(int bit, bool state);
	virtual bool GetOutputRegisterBit(int bit);

	//LVDC Input Discretes and Interrupts

	bool GetSIInboardEngineOut();
	bool GetSIOutboardEngineOut();
	bool GetSICInboardEngineCutoff();
	bool GetSIIEngineOut();
	bool GetCMCSIVBIgnitionSequenceStart();
	bool GetCMCSIVBCutoff();
	bool GetCMCSIVBTakeover();
	bool SIVBInjectionDelay();
	bool SCInitiationOfSIISIVBSeparation();
	bool GetSIIPropellantDepletionEngineCutoff();
	bool SpacecraftSeparationIndication();
	bool GetSIVBEngineOut();
	bool GetSIPropellantDepletionEngineCutoff();
	bool SIBLowLevelSensorsDry();
	bool GetLiftoff();
	bool GetGuidanceReferenceRelease();

	//Not real LVDA functions
	void TLIBegun();
	void TLIEnded();
	int GetStage();
	void SetStage(int stage);
	int GetVehicleNo();
	void GetRelativePos(VECTOR3 &v);
	void GetRelativeVel(VECTOR3 &v);
	bool GetSCControlPoweredFlight();
protected:
	IU *iu;

	std::bitset<13> DiscreteOutputRegister;
};

#define LVDA_START_STRING "LVDA_BEGIN"
#define LVDA_END_STRING "LVDA_END"