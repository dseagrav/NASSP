/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Sequential Events Controller simulation.

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

  **************************** Revision History ****************************/

#if !defined(_PA_SECS_H)
#define _PA_SECS_H

#include "DelayTimer.h"
#include "smjc.h"

class Saturn;
class FloatBag;

//Reaction Control System Controller
class RCSC
{
public:
	RCSC();
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	void ControlVessel(Saturn *v);
	bool GetMode1ASignal() { return Mode1ASignal; };
	bool GetCMTransferMotor1() { return CMTransferMotor1; };
	bool GetCMTransferMotor2() { return CMTransferMotor2; };
	bool GetCMTransferMotor(bool IsSystemA);
	bool GetInterconnectAndPropellantBurnRelayA() { return InterconnectAndPropellantBurnA; }
	bool GetInterconnectAndPropellantBurnRelayB() { return InterconnectAndPropellantBurnB; }
	bool GetOxidFuelPurgeRelay() { return FuelAndOxidBypassPurgeA || FuelAndOxidBypassPurgeB; }
	bool GetFuelDumpRelay() { return TD3.ContactClosed() || TD2.ContactClosed(); }
	bool GetOxidizerDumpRelay() { return OxidizerDumpA || OxidizerDumpB; }
	void StartPropellantDumpInhibitTimerA() { TD1.SetStart(true); }
	void StartPropellantDumpInhibitTimerB() { TD8.SetStart(true); }
	bool GetPropellantDumpInhibitA() { return TD1.ContactClosed(); }
	bool GetPropellantDumpInhibitB() { return TD8.ContactClosed(); }

	void SetPropellantDumpInhibitTimers(double del) { TD1.SetDelay(del); TD8.SetDelay(del); }

protected:

	void TimerTimestep(double simdt);

	//Source 6
	bool CMRCSLogicA();
	bool CMRCSLogicB();
	//Source 7
	bool CMPropellantDumpLogicA();
	bool CMPropellantDumpLogicB();
	//Source 8
	bool CMPropellantPurgeLogicA();
	bool CMPropellantPurgeLogicB();
	//Source 9
	bool CMRCSHeDumpLogicA();
	bool CMRCSHeDumpLogicB();

	//Motor Switches

	//S1
	bool CMTransferMotor1;
	//S2
	bool CMTransferMotor2;

	//Relays

	//K1
	bool OxidizerDumpA;
	//K2
	bool OxidizerDumpB;
	//K3
	bool InterconnectAndPropellantBurnA;
	//K4
	bool InterconnectAndPropellantBurnB;
	//K5
	bool FuelAndOxidBypassPurgeA;
	//K6
	bool FuelAndOxidBypassPurgeB;
	//K13
	bool RCSCCMSMTransferA;
	//K14
	bool RCSCCMSMTransferB;

	//Propellant Dump and Purge Disable Timer
	RestartableDelayTimer TD1, TD8;

	//CM RCS Propellant Dump Delay
	RestartableDelayTimer TD3, TD2;

	//Purge Delay
	DelayTimer TD5, TD4;

	//Main DC Bus Delay
	DelayTimer TD7, TD6;

	//Misc
	bool Mode1ASignal;

	Saturn *Sat;

	//NOTEHANDLE RCSCDisplay;
};

//Master Events Sequence Controller
class MESC
{
public:
	MESC();
	void Init(Saturn *v, DCbus *LogicBus, DCbus *PyroBus, MissionTimer *MT, ThreePosSwitch *MTC, EventTimer *ET, ThreePosSwitch *ETC, MESC* OtherMESCSystem, int IsSysA);
	void CBInit(CircuitBrakerSwitch *SECSLogic, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch *RCSLogicCB, CircuitBrakerSwitch *ELSBatteryCB, CircuitBrakerSwitch *EDSBreaker);
	void Timestep(double simdt);

	bool GetApexCoverJettisonRelay() { return ApexCoverJettison; }
	bool GetCSMLVSeparateRelay() { return CSMLVSeparateRelay; }
	bool GetCMSMSeparateRelay() { return CMSMSeparateRelay; }
	bool GetCMSMDeadFace() { return CMSMDeadFace && MESCLogicBus(); }
	bool GetCMRCSPressRelay() { return CMRCSPress; }
	bool GetAutoAbortEnableRelay() { return AutoAbortEnableRelay; }
	bool GetAutoRCSEnableRelay() { return RCSEnableDisableRelay; }
	bool GetLESAbortRelay() { return LESAbortRelay;	}
	bool GetLETJettisonAndFrangibleNutsRelay() { return LETJettisonAndFrangibleNutsRelay; }
	void SetAutoRCSEnableRelay(bool relay) { RCSEnableDisableRelay = relay; };
	bool FireUllage() { return MESCLogicArm && UllageRelay; };
	bool BECO() { return BoosterCutoffAbortStartRelay; };
	bool ELSActivateLogic();
	bool EDSUnsafeIndicateSignal();

	//Source 12
	bool MESCLogicBus();
	//Source 23/26
	bool EDSLiftoffCircuitPower();
	//Source 31
	bool EDSMainPower();

	void LoadState(FILEHANDLE scn, char *end_str);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);

	//Telemetry

	//CM RCS Pressurization Event
	bool CMRCSPressureSignal;
	//CM-SM Separation Event
	bool CMSMSepRelayClose;
	//SLA Separation Event
	bool SLASepRelay;
	//RCS Activate Event
	bool RCSActivateSignal;
	//EDS Abort Logic Output
	bool EDSAbortLogicOutput;
	//Forward Heatshield Jettison Event
	bool FwdHeatshieldJett;
	//Hand Controller Input Event
	bool CrewAbortSignal;
protected:

	void TimerTimestep(double simdt);

	bool EDSVote();

	//Source 11
	bool SequentialLogicBus();
	//Source 15
	bool SequentialArmBus();
	//Source 21
	bool EDSLogicPower();
	//Source P4
	bool SequentialPyroBus();

	//MESC Relays

	//Z1K1
	bool EDSAbort2Relay;
	//Z1K2
	bool EDSAbort1Relay;
	//Z2K2
	bool EDSAbort3Relay;
	//Z2K2
	bool EDSBusChangeover;
	//Z3
	bool LETPhysicalSeparationMonitor;
	//Z4
	bool AutoAbortEnableRelay;
	//Z5
	bool BoosterCutoffAbortStartRelay;
	//Z6
	bool LESAbortRelay;
	//Z7
	bool CMRCSPress;
	//Z8
	bool CMSMDeadFace;
	//Z9
	bool CMSMSeparateRelay;
	//Z10
	bool LESMotorFire;
	//Z11
	bool CanardDeploy;
	//Z12
	bool ELSActivateRelay;
	//Z13
	bool LETJettisonAndFrangibleNutsRelay;
	//Z14
	bool ApexCoverJettison;
	//Z16
	bool UllageRelay;
	//Z17
	bool CSMLVSeparateRelay;
	//Z18
	bool RCSEnableArmRelay;
	//Z19 (latching)
	bool RCSEnableDisableRelay;
	//Z20
	bool MESCLogicArm;
	//Z23
	bool PyroCutout;
	//Z24
	bool PitchControlMotorFire;
	//Z25
	bool ApexCoverDragChuteDeploy;
	bool ELSActivateSolidStateSwitch;

	//Miscellaneous
	bool CMSMSeparateLogic;
	bool AutoTowerJettison;
	bool SSSInput1;
	bool SSSInput2;
	bool IsSystemA;
	MESC* OtherMESC;

	//Abort Start Delay
	DelayTimer TD1;
	//CM/SM Seperate Delay
	DelayTimer TD3;
	//Canard Deploy Delay
	DelayTimer TD5;
	//ELS Activate Delay
	DelayTimer TD7;
	//CSM/LV Separation Delay
	DelayTimer TD11;
	//RCS Enable Arm Delay
	DelayTimer TD13;
	//RCS CMD Enable Delay
	DelayTimer TD15;
	//Apex Cover Jettison Delay
	DelayTimer TD17;
	//Pyro Cutout Delay
	DelayTimer TD23;

	Saturn *Sat;

	DCbus *SECSLogicBus;
	DCbus *SECSPyroBus;
	CircuitBrakerSwitch *SECSLogicBreaker;
	CircuitBrakerSwitch *SECSArmBreaker;
	CircuitBrakerSwitch *RCSLogicCircuitBreaker;
	CircuitBrakerSwitch *ELSBatteryBreaker;
	CircuitBrakerSwitch *EDSBatteryBreaker;
	MissionTimer *MissionTimerDisplay;
	EventTimer *EventTimerDisplay;
	ThreePosSwitch *EventTimerControl;
	ThreePosSwitch *MissionTimerControl;

	CircuitBrakerSwitch *EDSLogicBreaker;

	//NOTEHANDLE MESCDisplay;
};

//Lunar Docking Events Controller

class LDEC
{
public:
	LDEC();
	void Init(Saturn *v, MESC* connectedMESC, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch* DockProbe, ThreePosSwitch *DockingProbeRetract, ToggleSwitch *PyroArmSw, DCbus *PyroB, PowerMerge *PyroBusFeed);
	void InitSIMJett(CircuitBrakerSwitch *SMSec1Power);
	void Timestep(double simdt);
	void LoadState(FILEHANDLE scn, char *end_str);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);

	bool GetDockingRingFinalSeparation() { return DockingRingFinalSeparation; }
	bool GetLMSLASeparationInitiate() { return LMSLASeparationInitiate; }
	bool GetDockingProbeRetract1() { return DockingProbeRetract1; }
	bool GetDockingProbeRetract2() { return DockingProbeRetract2; }
	bool GetSIMPyroArmRelay() { return SIMPyroArmRelay; }
	void ResetLMSLASeparationInitiate() { LMSLASeparationInitiate = false; }

	//Telemetry

	bool CSM_LEM_LockRingSepRelaySignal;
protected:
	//Relays

	//Motor Switches

	//K1
	bool SECSPyroBusMotor;

	//Relays

	//Z1 (Latching)
	bool LMSLASeparationInitiate;
	//Z2
	bool DockingProbeRetract2;
	//Z3
	bool DockingProbeRetract1;
	//Z4
	bool DockingRingFinalSeparation;
	//Z5 (Only on Apollo 15 and later)
	bool SIMPyroArmRelay;

	//Delay Timers

	//LM-SLA Separation Initiate
	DelayTimer TD1;

	//Source 15
	bool SequentialArmBus();

	Saturn *Sat;
	MESC* mesc;
	CircuitBrakerSwitch *SECSArmBreaker;
	CircuitBrakerSwitch *DockProbeBreaker;
	ThreePosSwitch *DockingProbeRetractSwitch;
	ToggleSwitch *PyroArmSwitch;
	DCbus *PyroBus;
	PowerMerge *PyroBusFeeder;
	CircuitBrakerSwitch *SMSector1LogicPowerBreaker;
};

///
/// This class simulates the Sequential Events Control System in the CM.
/// \ingroup InternalSystems
/// \brief SECS simulation.
///
class SECS { 

public:
	SECS();
	virtual ~SECS();

	void ControlVessel(Saturn *v);
	void Realize();
	void Timestep(double simt, double simdt);

	bool AbortLightPowerA();
	bool AbortLightPowerB();
	bool LiftoffLightPower();
	bool NoAutoAbortLightPower();

	bool GetDockingProbeRetractPrim1() { return LDECA.GetDockingProbeRetract1(); }
	bool GetDockingProbeRetractPrim2() { return LDECA.GetDockingProbeRetract2(); }
	bool GetDockingProbeRetractSec1() { return LDECB.GetDockingProbeRetract1(); }
	bool GetDockingProbeRetractSec2() { return LDECB.GetDockingProbeRetract2(); }

	SMJC *GetSMJC(bool isSysA);

	void InitSIMJett(CircuitBrakerSwitch *SMSec1PowerA, CircuitBrakerSwitch *SMSec1PowerB);
	void SetSaturnType(int sattype);

	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);

	//Master Events Sequence Controller A
	MESC MESCA;
	//Master Events Sequence Controller B
	MESC MESCB;
	//Reaction Control System Controller
	RCSC rcsc;
	//Lunar Docking Events Controller A
	LDEC LDECA;
	//Lunar Docking Events Controller B
	LDEC LDECB;	
	//Service Module Jettison Controller A
	SMJC* SMJCA;
	//Service Module Jettison Controller B
	SMJC* SMJCB;

protected:
	bool IsLogicPoweredAndArmedA();
	bool IsLogicPoweredAndArmedB();
	
	int State;

	Saturn *Sat;
};

class BaroSwitch
{
public:
	BaroSwitch(double open, double close);

	void ControlVessel(Saturn *v);
	void Timestep();
	bool GetStatus() { return status; }
	void SetStatus(bool stat) { status = stat; }
	bool IsClosed() { return status == 1; }
	bool IsOpen() { return status == 0; }
protected:
	bool status;
	double OpenPa, ClosePa;

	Saturn *Sat;
};


//Pyro Continuity Verification Box
class PCVB
{
public:
	PCVB();
	void Init(Saturn *v);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	bool GetMainChuteReleaseA() { return MainChuteReleaseA; }
	bool GetMainChuteReleaseB() { return MainChuteReleaseB; }
	bool GetDrogueChuteDeployA() { return DrogueChuteDeployA; }
	bool GetDrogueChuteDeployB() { return DrogueChuteDeployB; }
	bool GetMainChuteDeployA() { return DrogueChuteReleasePilotChuteDeployA; }
	bool GetMainChuteDeployB() { return DrogueChuteReleasePilotChuteDeployB; }
protected:
	//Relays

	//Z2K1
	bool DrogueChuteDeployA;
	//Z2K4
	bool DrogueChuteReleasePilotChuteDeployA;
	//Z2K5
	bool MainChuteReleaseA;

	//Z1K1
	bool DrogueChuteDeployB;
	//Z1K4
	bool DrogueChuteReleasePilotChuteDeployB;
	//Z1K5
	bool MainChuteReleaseB;

	Saturn *Sat;
};

//Earth Landing Sequence Controller
class ELSC
{
public:
	ELSC();
	void Init(Saturn *v, CircuitBrakerSwitch *ELSBatteryCB, MESC* ConnectedMESC, ELSC *OtherELSCSystem);
	void Timestep(double simdt);
	void SaveState(FILEHANDLE scn, char *start_str, char *end_str);
	void LoadState(FILEHANDLE scn, char *end_str);

	bool GetDrogueParachuteDeployRelay() { return DrogueParachuteDeploy; }
	bool GetMainParachuteDeployRelay() { return PilotParachuteDeploy; }
protected:

	void TimerTimestep(double simdt);
	bool ELSBatteryPower();

	//Relays

	//K1
	bool BaroswitchLockIn;
	//K2
	bool DrogueParachuteDeploy;
	//K3
	bool PilotParachuteDeploy;

	//Delay Timers:
	
	//Drogue Parachute Deploy
	DelayTimer TD1;
	//Pilot Parachute Deploy
	DelayTimer TD3;

	CircuitBrakerSwitch *ELSBatteryBreaker;

	ELSC* OtherELSC;
	MESC* mesc;

	Saturn *Sat;

	//NOTEHANDLE ELSCDisplay;

	friend class PCVB;
};

///
/// This class simulates the Earth Landing System in the CM.
/// \ingroup InternalSystems
/// \brief ELS simulation.
///
class ELS { 

public:
	ELS();
	virtual ~ELS();

	void ControlVessel(Saturn *v);
	void Timestep(double simt, double simdt);
	void SystemTimestep(double simdt);
	void LoadState(FILEHANDLE scn);
	void SaveState(FILEHANDLE scn);
	double *GetDyeMarkerLevelRef() { return &DyeMarkerLevel; }

	//Baroswitches
	BaroSwitch BaroSwitch24k;
	BaroSwitch BaroSwitch10k;

	//Earth Landing Sequence Controller A
	ELSC ELSCA;
	//Earth Landing Sequence Controller B
	ELSC ELSCB;
	//Pyro Continuity Verification Box
	PCVB pcvb;

protected:
	double NewFloatBagSize(double size, ThreePosSwitch *sw, CircuitBrakerSwitch *cb, double simdt);

	int State;

	double FloatBag1Size;
	double FloatBag2Size;
	double FloatBag3Size;
	double DyeMarkerLevel;
	double DyeMarkerTime;

	Saturn *Sat;
	FloatBag *FloatBagVessel;
};

//
// Strings for state saving.
//

#define SECS_START_STRING		"SECS_BEGIN"
#define SECS_END_STRING			"SECS_END"

#define ELS_START_STRING		"ELS_BEGIN"
#define ELS_END_STRING			"ELS_END"

#endif // _PA_SECS_H
