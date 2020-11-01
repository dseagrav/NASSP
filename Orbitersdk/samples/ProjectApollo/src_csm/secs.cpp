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

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "stdio.h"
#include "math.h"
#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"
#include "ioChannels.h"
#include "toggleswitch.h"
#include "saturn.h"
#include "FloatBag.h"
#include "secs.h"
#include "papi.h"

RCSC::RCSC():
TD1(42.0),
TD2(5.0),
TD3(5.0),
TD4(13.0),
TD5(13.0),
TD6(13.0),
TD7(13.0),
TD8(42.0)
{
	OxidizerDumpA = false;
	OxidizerDumpB = false;
	InterconnectAndPropellantBurnA = false;
	InterconnectAndPropellantBurnB = false;
	FuelAndOxidBypassPurgeA = false;
	FuelAndOxidBypassPurgeB = false;
	RCSCCMSMTransferA = false;
	RCSCCMSMTransferB = false;
	CMTransferMotor1 = false;
	CMTransferMotor2 = false;

	Mode1ASignal = false;

	//RCSCDisplay = oapiCreateAnnotation(false, 0.65, _V(1, 1, 0));
	//oapiAnnotationSetPos(RCSCDisplay, 0.66, 0.1, 0.99, 1);
}

void RCSC::TimerTimestep(double simdt)
{
	TD1.Timestep(simdt);
	TD2.Timestep(simdt);
	TD3.Timestep(simdt);
	TD4.Timestep(simdt);
	TD5.Timestep(simdt);
	TD6.Timestep(simdt);
	TD7.Timestep(simdt);
	TD8.Timestep(simdt);
}

void RCSC::Timestep(double simdt)
{
	TimerTimestep(simdt);

	if (Sat->secs.MESCA.EDSLiftoffCircuitPower())
	{
		if (!GetCMTransferMotor1())
			StartPropellantDumpInhibitTimerA();
	}
	if (Sat->secs.MESCB.EDSLiftoffCircuitPower())
	{
		if (!GetCMTransferMotor2())
			StartPropellantDumpInhibitTimerB();
	}

	Mode1ASignal = false;
	
	if (Sat->secs.MESCA.GetCMSMDeadFace())
	{
		RCSCCMSMTransferA = true;
		if (Sat->PropDumpAutoSwitch.IsUp() && !TD1.ContactClosed())
		{
			Mode1ASignal = true;
			
			OxidizerDumpA = true;
			TD3.SetStart(true);
		}
	}
	else
	{
		RCSCCMSMTransferA = false;
	}

	if (RCSCCMSMTransferA)
	{
		Sat->agc.SetInputChannelBit(030, CMSMSeperate, true);
	}
	else
	{
		Sat->agc.SetInputChannelBit(030, CMSMSeperate, false);
	}

	if (Sat->secs.MESCB.GetCMSMDeadFace())
	{
		RCSCCMSMTransferB = true;
		if (Sat->PropDumpAutoSwitch.IsUp() && !TD8.ContactClosed())
		{
			Mode1ASignal = true;
			
			OxidizerDumpB = true;
			TD2.SetStart(true);
		}
	}
	else
	{
		RCSCCMSMTransferB = false;
	}

	// CM/SM transfer motors
	if (Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnACircuitBraker.IsPowered() && RCSCCMSMTransferA) {
		CMTransferMotor1 = true;
	}
	if (Sat->CMRCSLogicSwitch.IsUp() && Sat->RCSLogicMnBCircuitBraker.IsPowered() && RCSCCMSMTransferB) {
		CMTransferMotor2 = true;
	}
	if (Sat->RCSTrnfrSwitch.IsUp()) {
		if (Sat->RCSLogicMnACircuitBraker.IsPowered()) {
			CMTransferMotor1 = true;
		}
		if (Sat->RCSLogicMnBCircuitBraker.IsPowered()) {
			CMTransferMotor2 = true;
		}
	}
	else if (Sat->RCSTrnfrSwitch.IsDown()) {
		if (Sat->RCSLogicMnACircuitBraker.IsPowered()) {
			CMTransferMotor1 = false;
		}
		if (Sat->RCSLogicMnBCircuitBraker.IsPowered()) {
			CMTransferMotor2 = false;
		}
	}

	//TD1 logic
	if (CMRCSLogicA() && !CMTransferMotor1) {
		TD1.SetRunning(TD1.GetStart());
	}
	else
	{
		TD1.SetStart(false);
	}
	
	if (CMRCSLogicA()) {

		TD3.SetRunning(true);

		if (TD3.ContactClosed())
		{
			TD5.SetRunning(true);
		}
	}

	if (CMPropellantDumpLogicA() || (CMRCSLogicA() && OxidizerDumpA))
	{
		InterconnectAndPropellantBurnA = true;
	}
	else
	{
		InterconnectAndPropellantBurnA = false;
	}

	//TD2 logic
	if (CMRCSLogicB() && !CMTransferMotor2) {
		TD8.SetRunning(TD8.GetStart());
	}
	else
	{
		TD8.SetStart(false);
	}

	if (CMRCSLogicB()) {

		TD2.SetRunning(true);

		if (TD2.ContactClosed())
		{
			TD4.SetRunning(true);
		}
	}

	if (CMPropellantDumpLogicB() || (CMRCSLogicB() && OxidizerDumpB))
	{
		InterconnectAndPropellantBurnB = true;
	}
	else
	{
		InterconnectAndPropellantBurnB = false;
	}

	if (TD5.ContactClosed() || CMPropellantPurgeLogicA() || CMRCSHeDumpLogicA())
	{
		FuelAndOxidBypassPurgeA = true;
	}
	else
	{
		FuelAndOxidBypassPurgeA = false;
	}

	if (TD4.ContactClosed() || CMPropellantPurgeLogicB() || CMRCSHeDumpLogicB())
	{
		FuelAndOxidBypassPurgeB = true;
	}
	else
	{
		FuelAndOxidBypassPurgeB = false;
	}

	//Mode 1A Display
	/*char buffer[1024];
	sprintf(buffer, "REACTION CONTROL SYSTEM CONTROLLER - MODE 1A\n\n");
	if (OxidizerDumpA)
	{
		sprintf(buffer, "%sOxidizer Dump Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sOxidizer Dump Relay - Deenergized\n", buffer);
	}
	if (InterconnectAndPropellantBurnA)
	{
		sprintf(buffer, "%sHelium & Oxidizer Interconnect Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sHelium & Oxidizer Interconnect Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sCM RCS Fuel Dump Timer %.2f s\n", buffer, TD3.GetTime());
	sprintf(buffer, "%sCM RCS Purge Timer %.2f s\n", buffer, TD5.GetTime());
	if (FuelAndOxidBypassPurgeA)
	{
		sprintf(buffer, "%sFuel & Oxidizer Bypass Purge Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sFuel & Oxidizer Bypass Purge Relay - Deenergized\n", buffer);
	}
	if (GetOxidizerDumpRelay())
	{
		sprintf(buffer, "%sOxidizer Dumping - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sOxidizer Dumping - Deenergized\n", buffer);
	}
	if (GetFuelDumpRelay())
	{
		sprintf(buffer, "%sFuel Dumping - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sFuel Dumping - Deenergized\n", buffer);
	}
	if (GetOxidFuelPurgeRelay())
	{
		sprintf(buffer, "%sOxidizer & Fuel Purging - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sOxidizer & Fuel Purging - Deenergized\n", buffer);
	}

	oapiAnnotationSetText(RCSCDisplay, buffer);*/
}

void RCSC::ControlVessel(Saturn *v)
{
	Sat = v;
}

bool RCSC::CMRCSLogicA() 
{ 
	return (Sat->RCSLogicMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) && Sat->CMRCSLogicSwitch.IsUp(); 
}
bool RCSC::CMRCSLogicB() 
{
	return (Sat->RCSLogicMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) && Sat->CMRCSLogicSwitch.IsUp(); 
}

bool RCSC::CMPropellantDumpLogicA() 
{ 
	return CMRCSLogicA() && Sat->CMPropDumpSwitch.IsUp(); 
}

bool RCSC::CMPropellantDumpLogicB() 
{ 
	return CMRCSLogicB() && Sat->CMPropDumpSwitch.IsUp(); 
}

bool RCSC::CMPropellantPurgeLogicA() 
{ 
	return CMPropellantDumpLogicA() && Sat->CMPropPurgeSwitch.IsUp(); 
}

bool RCSC::CMPropellantPurgeLogicB() 
{ 
	return CMPropellantDumpLogicB() && Sat->CMPropPurgeSwitch.IsUp(); 
}

bool RCSC::CMRCSHeDumpLogicA() 
{ 
	return (Sat->RCSLogicMnACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) && Sat->CmRcsHeDumpSwitch.GetState(); 
}

bool RCSC::CMRCSHeDumpLogicB() 
{ 
	return (Sat->RCSLogicMnBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) && Sat->CmRcsHeDumpSwitch.GetState(); 
}

bool RCSC::GetCMTransferMotor(bool IsSystemA)
{
	if (IsSystemA) return GetCMTransferMotor1();

	return GetCMTransferMotor2();
}

void RCSC::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "OXIDIZERDUMPA", OxidizerDumpA);
	papiWriteScenario_bool(scn, "OXIDIZERDUMPB", OxidizerDumpB);
	papiWriteScenario_bool(scn, "INTERCONNECTANDPROPELLANTBURNA", InterconnectAndPropellantBurnA);
	papiWriteScenario_bool(scn, "INTERCONNECTANDPROPELLANTBURNB", InterconnectAndPropellantBurnB);
	papiWriteScenario_bool(scn, "FUELANDOXIDBYPASSPURGEA", FuelAndOxidBypassPurgeA);
	papiWriteScenario_bool(scn, "FUELANDOXIDBYPASSPURGEB", FuelAndOxidBypassPurgeB);
	papiWriteScenario_bool(scn, "RCSCCMSMTRANSFERA", RCSCCMSMTransferA);
	papiWriteScenario_bool(scn, "RCSCCMSMTRANSFERB", RCSCCMSMTransferB);
	papiWriteScenario_bool(scn, "CMTRANSFERMOTOR1", CMTransferMotor1);
	papiWriteScenario_bool(scn, "CMTRANSFERMOTOR2", CMTransferMotor2);

	TD1.SaveState(scn, "TD1_BEGIN", "TD1_END");
	TD2.SaveState(scn, "TD2_BEGIN", "TD_END");
	TD3.SaveState(scn, "TD3_BEGIN", "TD_END");
	TD4.SaveState(scn, "TD4_BEGIN", "TD_END");
	TD5.SaveState(scn, "TD5_BEGIN", "TD_END");
	TD6.SaveState(scn, "TD6_BEGIN", "TD_END");
	TD7.SaveState(scn, "TD7_BEGIN", "TD_END");
	TD8.SaveState(scn, "TD8_BEGIN", "TD_END");

	oapiWriteLine(scn, end_str);
}

void RCSC::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "OXIDIZERDUMPA", OxidizerDumpA);
		papiReadScenario_bool(line, "OXIDIZERDUMPB", OxidizerDumpB);
		papiReadScenario_bool(line, "INTERCONNECTANDPROPELLANTBURNA", InterconnectAndPropellantBurnA);
		papiReadScenario_bool(line, "INTERCONNECTANDPROPELLANTBURNB", InterconnectAndPropellantBurnB);
		papiReadScenario_bool(line, "FUELANDOXIDBYPASSPURGEA", FuelAndOxidBypassPurgeA);
		papiReadScenario_bool(line, "FUELANDOXIDBYPASSPURGEB", FuelAndOxidBypassPurgeB);
		papiReadScenario_bool(line, "RCSCCMSMTRANSFERA", RCSCCMSMTransferA);
		papiReadScenario_bool(line, "RCSCCMSMTRANSFERB", RCSCCMSMTransferB);
		papiReadScenario_bool(line, "CMTRANSFERMOTOR1", CMTransferMotor1);
		papiReadScenario_bool(line, "CMTRANSFERMOTOR2", CMTransferMotor2);

		if (!strnicmp(line, "TD1_BEGIN", sizeof("TD1_BEGIN"))) {
			TD1.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD2_BEGIN", sizeof("TD2_BEGIN"))) {
			TD2.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD3_BEGIN", sizeof("TD3_BEGIN"))) {
			TD3.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD4_BEGIN", sizeof("TD4_BEGIN"))) {
			TD4.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD5_BEGIN", sizeof("TD5_BEGIN"))) {
			TD5.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD6_BEGIN", sizeof("TD6_BEGIN"))) {
			TD6.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD7_BEGIN", sizeof("TD7_BEGIN"))) {
			TD7.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD8_BEGIN", sizeof("TD8_BEGIN"))) {
			TD8.LoadState(scn, "TD_END");
		}
	}
}

MESC::MESC():
	TD1(0.03),
	TD3(0.1),
	TD5(11.0),
	TD7(3.0),
	TD11(3.0),
	TD13(0.8),
	TD15(1.0),
	TD17(0.4),
	TD23(1.7)
{
	MESCLogicArm = false;
	BoosterCutoffAbortStartRelay = false;
	LETPhysicalSeparationMonitor = false;
	LESAbortRelay = false;
	AutoAbortEnableRelay = false;
	CMSMDeadFace = false;
	CMSMSeparateRelay = false;
	PyroCutout = false;
	CMRCSPress = false;
	CanardDeploy = false;
	UllageRelay = false;
	CSMLVSeparateRelay = false;
	LESMotorFire = false;
	PitchControlMotorFire = false;
	RCSEnableArmRelay = false;
	RCSEnableDisableRelay = false;
	LETJettisonAndFrangibleNutsRelay = false;
	ApexCoverJettison = false;
	ApexCoverDragChuteDeploy = false;
	ELSActivateRelay = false;
	EDSBusChangeover = false;
	EDSAbort1Relay = false;
	EDSAbort2Relay = false;
	EDSAbort3Relay = false;
	ELSActivateSolidStateSwitch = false;

	CrewAbortSignal = false;

	AutoTowerJettison = false;
	SSSInput1 = false;
	SSSInput2 = false;
	IsSystemA = false;

	EDSLogicBreaker = NULL;
	SECSLogicBus = NULL;
	SECSPyroBus = NULL;
	MissionTimerDisplay = NULL;
	MissionTimerControl = NULL;
	EventTimerDisplay = NULL;
	EventTimerControl = NULL;
	Sat = NULL;
	OtherMESC = NULL;
	SECSLogicBreaker = NULL;
	SECSArmBreaker = NULL;
	RCSLogicCircuitBreaker = NULL;
	ELSBatteryBreaker = NULL;
	EDSBatteryBreaker = NULL;
	//MESCDisplay = NULL;
}

void MESC::Init(Saturn *v, DCbus *LogicBus, DCbus *PyroBus, MissionTimer *MT, ThreePosSwitch *MTC, EventTimer *ET, ThreePosSwitch *ETC, MESC* OtherMESCSystem, int IsSysA)
{
	SECSLogicBus = LogicBus;
	SECSPyroBus = PyroBus;
	MissionTimerDisplay = MT;
	MissionTimerControl = MTC;
	EventTimerDisplay = ET;
	EventTimerControl = ETC;
	Sat = v;
	OtherMESC = OtherMESCSystem;
	IsSystemA = IsSysA;

	//MESCDisplay = oapiCreateAnnotation(false, 0.65, _V(1, 1, 0));
	//oapiAnnotationSetPos(MESCDisplay, 0, 0.1, 0.33, 1);
}

void MESC::CBInit(CircuitBrakerSwitch *SECSLogic, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch *RCSLogicCB, CircuitBrakerSwitch *ELSBatteryCB, CircuitBrakerSwitch *EDSBreaker)
{
	SECSLogicBreaker = SECSLogic;
	SECSArmBreaker = SECSArm;
	RCSLogicCircuitBreaker = RCSLogicCB;
	ELSBatteryBreaker = ELSBatteryCB;
	EDSBatteryBreaker = EDSBreaker;
}

void MESC::TimerTimestep(double simdt)
{
	TD1.Timestep(simdt);
	TD3.Timestep(simdt);
	TD5.Timestep(simdt);
	TD7.Timestep(simdt);
	TD11.Timestep(simdt);
	TD13.Timestep(simdt);
	TD15.Timestep(simdt);
	TD17.Timestep(simdt);
	TD23.Timestep(simdt);
}

void MESC::Timestep(double simdt)
{
	TimerTimestep(simdt);

	//
	// SECS Logic Bus
	//

	bool switchOn = (Sat->SECSLogic1Switch.IsUp() || Sat->SECSLogic2Switch.IsUp());
	if (switchOn && SECSArmBreaker->IsPowered()) {
		MESCLogicArm = true;
		SECSLogicBus->WireTo(SECSLogicBreaker);
	}
	else {
		MESCLogicArm = false;
		SECSLogicBus->Disconnect();
	}

	EDSAbort1Relay = Sat->iuCommandConnector.GetEDSAbort(1);
	EDSAbort2Relay = Sat->iuCommandConnector.GetEDSAbort(2);
	EDSAbort3Relay = Sat->iuCommandConnector.GetEDSAbort(3);

	EDSAbortLogicOutput = false;

	//EDS Logic
	if (Sat->EDSPowerSwitch.IsUp())
	{
		//EDS Changeover
		if (EDSBatteryBreaker->IsPowered())
		{
			EDSBusChangeover = false;
		}
		else
		{
			EDSBusChangeover = true;
		}

		//Switch EDS Power
		if (!EDSBusChangeover)
		{
			EDSLogicBreaker = EDSBatteryBreaker;
		}
		else
		{
			EDSLogicBreaker = &Sat->EDS2BatCCircuitBraker;
		}

		//EDS Auto Abort
		if (EDSLogicPower())
		{
			if (EDSVote())
			{
				EDSAbortLogicOutput = true;

				if (AutoAbortEnableRelay)
				{
					TD1.SetRunning(true);
				}
			}
		}
	}
	else if (EDSLogicBreaker)
	{
		EDSLogicBreaker = NULL;
	}

	//Liftoff Circuits

	if (EDSLiftoffCircuitPower())
	{
		if (MissionTimerControl->IsUp())
		{
			MissionTimerDisplay->Reset();
		}
		if (EventTimerControl->IsCenter())
		{
			EventTimerDisplay->Reset();
			EventTimerDisplay->SetRunning(true);
		}
	}

	// Monitor LET Status
	if ((MESCLogicBus() || EDSLogicPower()) && Sat->LESAttached && (Sat->TowerJett1Switch.IsDown() || Sat->TowerJett2Switch.IsDown()))
	{
		LETPhysicalSeparationMonitor = true;
	}
	else
	{
		LETPhysicalSeparationMonitor = false;
	}

	//Auto Abort Logic
	if (((!Sat->LaunchFail.AutoAbortEnableFail && EDSLiftoffCircuitPower()) || ((AutoAbortEnableRelay || Sat->LiftoffNoAutoAbortSwitch.GetState()) && LETPhysicalSeparationMonitor && SequentialLogicBus())) && Sat->EDSSwitch.GetState())
	{
		AutoAbortEnableRelay = true;
	}
	else
	{
		AutoAbortEnableRelay = false;
	}

	// Abort Handling
	if ((Sat->THCRotary.IsCounterClockwise() && SequentialLogicBus()) || TD1.ContactClosed())
	{
		BoosterCutoffAbortStartRelay = true;
	}
	else
	{
		BoosterCutoffAbortStartRelay = false;
	}

	//LES Abort Relay
	if (SequentialLogicBus() && BoosterCutoffAbortStartRelay && LETPhysicalSeparationMonitor)
	{
		LESAbortRelay = true;
	}
	else if (!MESCLogicBus())
	{
		LESAbortRelay = false;
	}

	//Start Event Timer
	if (SequentialLogicBus() && BoosterCutoffAbortStartRelay)
	{
		if (Sat->EventTimerContSwitch.IsCenter())
		{
			Sat->EventTimerDisplay.Reset();
			Sat->EventTimerDisplay.SetRunning(true);
		}
	}

	//Canard Deploy Timer
	if (MESCLogicBus() && LESAbortRelay)
	{
		TD5.SetRunning(true);
	}

	//SM Jettison Controller Start
	if (Sat->GetStage() < CM_STAGE && SequentialArmBus() && (Sat->CmSmSep1Switch.IsUp() || Sat->CmSmSep2Switch.IsUp()))
	{
		if (Sat->secs.rcsc.GetCMTransferMotor(IsSystemA) == false)
		{
			Sat->secs.GetSMJC(IsSystemA)->SMJettControllerStart();
		}
	}

	//CM/SM Separation Logic
	CMSMSeparateLogic = (MESCLogicBus() && LESAbortRelay) || (SequentialArmBus() && (Sat->CmSmSep1Switch.IsUp() || Sat->CmSmSep2Switch.IsUp())) || (MESCLogicBus() && CMSMSeparateRelay);

	//CM/SM Deadface Relay
	if (CMSMSeparateLogic && !PyroCutout)
	{
		CMSMDeadFace = true;
	}
	else
	{
		CMSMDeadFace = false;
	}

	//Pyro Cutout Logic
	if (MESCLogicBus() && CMSMSeparateRelay)
	{
		TD23.SetRunning(true);
	}

	if (TD23.ContactClosed())
	{
		PyroCutout = true;
	}
	else if (!CMSMSeparateLogic)
	{
		PyroCutout = false;
	}

	if (MESCLogicBus() && CMSMDeadFace)
	{
		TD3.SetRunning(true);

		if (Sat->PropDumpAutoSwitch.IsDown())
		{
			TD15.SetRunning(true);
		}
	}

	//CM/SM Separate Relay
	if (TD3.ContactClosed())
	{
		CMSMSeparateRelay = true;
	}
	else
	{
		CMSMSeparateRelay = false;
	}

	// CM RCS Press Relay Logic
	if ((MESCLogicBus() && CMSMDeadFace) || (SequentialArmBus() && Sat->CMRCSPressSwitch.IsUp()))
	{
		CMRCSPress = true;
	}
	else
	{
		CMRCSPress = false;
	}

	//LES Motor Fire Logic
	if ((Sat->LesMotorFireSwitch.GetState() && SequentialArmBus()) || (CMSMSeparateRelay && MESCLogicBus()))
	{
		LESMotorFire = true;
	}
	else
	{
		LESMotorFire = false;
	}

	//
	// Fire the LEM.
	//
	if (LESMotorFire && SequentialPyroBus())
	{
		Sat->FireLaunchEscapeMotor();
		
		Sat->JettisonLET();
	}

	//Tower Jettison Relay
	if ((AutoTowerJettison && !Sat->LaunchFail.LETAutoJetFail) || ((MESCLogicBus() || EDSLogicPower()) && (Sat->TowerJett1Switch.IsUp() || Sat->TowerJett2Switch.IsUp())))
	{
		LETJettisonAndFrangibleNutsRelay = true;
	}
	else
	{
		LETJettisonAndFrangibleNutsRelay = false;
	}

	//Jettison Tower
	if (SequentialPyroBus() && LETJettisonAndFrangibleNutsRelay)
	{
		Sat->CutLESLegs();

		if (!Sat->LaunchFail.LESJetMotorFail)
		{
			Sat->FireTowerJettisonMotor();
			Sat->JettisonLET();
		}
	}

	//Canard Deploy
	if ((TD5.ContactClosed() && MESCLogicBus()) || (Sat->CanardDeploySwitch.GetState() && SequentialArmBus()))
	{
		CanardDeploy = true;
	}
	else
	{
		CanardDeploy = false;
	}

	if (SequentialPyroBus() && CanardDeploy)
	{
		Sat->DeployCanard();
	}

	//Pitch Control Motor
	if (LESMotorFire && Sat->secs.rcsc.GetMode1ASignal())
	{
		PitchControlMotorFire = true;
	}
	else
	{
		PitchControlMotorFire = false;
	}

	//Fire Pitch Control Motor
	if (PitchControlMotorFire && SequentialPyroBus())
	{
		Sat->FirePitchControlMotor();
	}

	UllageRelay = false;

	//SPS Abort
	if (SequentialLogicBus())
	{
		if (BoosterCutoffAbortStartRelay && !LETPhysicalSeparationMonitor && !LESAbortRelay)
		{
			UllageRelay = true;
			TD11.SetRunning(true);
		}
	}

	if (IsSystemA)
	{
		if (UllageRelay)
		{
			Sat->agc.SetInputChannelBit(030, SIVBSeperateAbort, true);
		}
		else
		{
			Sat->agc.SetInputChannelBit(030, SIVBSeperateAbort, false);
		}
	}

	//CSM LV Separate Relay
	if (TD11.ContactClosed() || (Sat->CsmLvSepSwitch.GetState() && SequentialArmBus()))
	{
		CSMLVSeparateRelay = true;
	}
	else
	{
		CSMLVSeparateRelay = false;
	}

	//Separate Launch Vehicle
	if (SequentialPyroBus() && CSMLVSeparateRelay)
	{
		//Right now this is done in the SECS class
	}

	if (MESCLogicBus() && CSMLVSeparateRelay)
	{
		TD13.SetRunning(true);
		SLASepRelay = true;
	}
	else
	{
		SLASepRelay = false;
	}

	if (MESCLogicBus() && TD13.ContactClosed())
	{
		RCSEnableArmRelay = true;
	}
	else
	{
		RCSEnableArmRelay = false;
	}

	//RCS Enable/Disable Logic
	if ((MESCLogicBus() && RCSEnableArmRelay) || (SequentialArmBus() && Sat->RCSCMDSwitch.IsUp()) || TD15.ContactClosed())
	{
		RCSEnableDisableRelay = true;
	}
	else if (SequentialArmBus() && Sat->RCSCMDSwitch.IsDown())
	{
		RCSEnableDisableRelay = false;
	}

	//ELS Activate Delay
	if (MESCLogicBus() && CanardDeploy)
	{
		TD7.SetRunning(true);
	}

	//Solid State Switch

	SSSInput1 = false;
	SSSInput2 = false;

	SSSInput1 = MESCLogicBus() && (TD7.ContactClosed() || Sat->ELSLogicSwitch.IsUp());
	
	if (MESCLogicBus() && (CanardDeploy || Sat->ELSLogicSwitch.IsUp()) && Sat->els.BaroSwitch24k.IsClosed() && Sat->ELSAutoSwitch.IsUp())
	{
		SSSInput2 = true;
	}
	else if (MESCLogicBus() && ELSActivateRelay)
	{
		SSSInput2 = true;
	}

	if (SSSInput1 && SSSInput2)
	{
		ELSActivateSolidStateSwitch = true;
	}
	else
	{
		ELSActivateSolidStateSwitch = false;
	}

	//ELS Activate Relay
	if (ELSActivateSolidStateSwitch || (MESCLogicBus() && OtherMESC->ELSActivateRelay) || ((ELSBatteryBreaker->Voltage() > SP_MIN_DCVOLTAGE) && (Sat->MainDeploySwitch.GetState() || Sat->DrogueDeploySwitch.GetState())))
	{
		ELSActivateRelay = true;
	}
	else
	{
		ELSActivateRelay = false;
	}

	AutoTowerJettison = false;

	if (MESCLogicBus())
	{
		if (CanardDeploy || Sat->ELSAutoSwitch.IsUp())
		{
			if (ELSActivateRelay && Sat->els.BaroSwitch24k.IsClosed())
			{
				TD17.SetRunning(true);
				AutoTowerJettison = true;
				RCSEnableDisableRelay = false;
			}
		}
	}

	//Apex Cover Relay
	if (((ELSBatteryBreaker->Voltage() > SP_MIN_DCVOLTAGE) && Sat->ApexCoverJettSwitch.GetState()))
	{
		ApexCoverJettison = true;
	}
	else if (((MESCLogicBus() && OtherMESC->ApexCoverJettison) || TD17.ContactClosed()) && !Sat->LandFail.CoverFail)
	{
		ApexCoverJettison = true;
	}
	else
	{
		ApexCoverJettison = false;
	}

	//Apex Cover Drag Chute Deploy Relay
	if (MESCLogicBus() && ApexCoverJettison && !Sat->ApexCoverAttached)
	{
		ApexCoverDragChuteDeploy = true;
	}
	else
	{
		ApexCoverDragChuteDeploy = false;
	}

	//Telemetry
	if (MESCLogicBus() && CMRCSPress)
	{
		CMRCSPressureSignal = true;
	}
	else
	{
		CMRCSPressureSignal = false;
	}

	if (MESCLogicBus() && CMSMSeparateRelay)
	{
		CMSMSepRelayClose = true;
	}
	else
	{
		CMSMSepRelayClose = false;
	}

	if (MESCLogicBus() && RCSEnableDisableRelay)
	{
		RCSActivateSignal = true;
	}
	else
	{
		RCSActivateSignal = false;
	}

	if (MESCLogicBus() && ApexCoverJettison)
	{
		FwdHeatshieldJett = true;
	}
	else
	{
		FwdHeatshieldJett = false;
	}

	if (Sat->THCRotary.IsCounterClockwise() && SequentialLogicBus())
	{
		CrewAbortSignal = true;
	}
	else
	{
		CrewAbortSignal = false;
	}

	//Mode 1A Display
	/*char buffer[1024];
	sprintf(buffer, "MASTER EVENTS SEQUENCE CONTROLLER - MODE 1A\n\n");
	if (Sat->THCRotary.IsCounterClockwise())
	{
		sprintf(buffer, "%sTHC - CCW\n", buffer);
	}
	else if (Sat->THCRotary.IsClockwise())
	{
		sprintf(buffer, "%sTHC - CW\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sTHC - Neutral\n", buffer);
	}
	if (BoosterCutoffAbortStartRelay)
	{
		sprintf(buffer, "%sBooster Cutoff & Abort Start Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sBooster Cutoff & Abort Start Relay - Deenergized\n", buffer);
	}
	if (LETPhysicalSeparationMonitor)
	{
		sprintf(buffer, "%sLET Physical Separation Monitor Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sLET Physical Separation Monitor Relay - Deenergized\n", buffer);
	}
	if (LESAbortRelay)
	{
		sprintf(buffer, "%sLES Abort Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sLES Abort Relay - Deenergized\n", buffer);
	}
	if (CMSMDeadFace)
	{
		sprintf(buffer, "%sCM/SM Deadface Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sCM/SM Deadface Relay - Deenergized\n", buffer);
	}
	if (CMRCSPress)
	{
		sprintf(buffer, "%sCM RCS Press Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sCM RCS Press Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sCM/SM Separate Delay Timer %.2f s\n", buffer, TD3.GetTime());
	if (CMSMSeparateRelay)
	{
		sprintf(buffer, "%sCM/SM Separate Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sCM/SM Separate Relay - Deenergized\n", buffer);
	}
	if (LESMotorFire)
	{
		sprintf(buffer, "%sLES Motor Fire Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sLES Motor Fire Relay - Deenergized\n", buffer);
	}
	if (PitchControlMotorFire)
	{
		sprintf(buffer, "%sPitch Control Motor Fire Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sPitch Control Motor Fire Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sPyro Cutout Delay Timer %.2f s\n", buffer, TD23.GetTime());
	if (PyroCutout)
	{
		sprintf(buffer, "%sPyro Cutout Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sPyro Cutout Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sCanard Deploy Timer %.2f s\n", buffer, TD5.GetTime());
	if (CanardDeploy)
	{
		sprintf(buffer, "%sCanard Deploy Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sCanard Deploy Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sELS Activate Timer %.2f s\n", buffer, TD7.GetTime());
	if (ELSActivateRelay)
	{
		sprintf(buffer, "%sELS Activate Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sELS Activate Relay - Deenergized\n", buffer);
	}
	if (LETJettisonAndFrangibleNutsRelay)
	{
		sprintf(buffer, "%sTower Jettison Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sTower Jettison Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sApex Cover Jettison Timer %.2f s\n", buffer, TD17.GetTime());
	if (ApexCoverJettison)
	{
		sprintf(buffer, "%sApex Cover Jettison Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sApex Cover Jettison Relay - Deenergized\n", buffer);
	}

	oapiAnnotationSetText(MESCDisplay, buffer);*/
}

bool MESC::SequentialLogicBus()
{
	return (SECSLogicBreaker->Voltage() > SP_MIN_DCVOLTAGE);
}

bool MESC::SequentialArmBus()
{
	return SECSArmBreaker->IsPowered();
}

bool MESC::SequentialPyroBus()
{
	return (SECSPyroBus->Voltage() > SP_MIN_DCVOLTAGE);
}

bool MESC::MESCLogicBus() {

	if (SECSLogicBus->Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

bool MESC::EDSLogicPower()
{
	if (EDSLogicBreaker)
		return (EDSLogicBreaker->Voltage() > SP_MIN_DCVOLTAGE) && Sat->EDSPowerSwitch.IsUp();

	return false;
}

bool MESC::EDSLiftoffCircuitPower()
{
	if (Sat->stage >= CSM_LEM_STAGE) return false;

	return Sat->iuCommandConnector.GetLiftOffCircuit(IsSystemA);
}

bool MESC::EDSMainPower()
{
	return EDSBatteryBreaker->IsPowered() && Sat->EDSPowerSwitch.IsUp();
}

bool MESC::EDSVote()
{
	return (!EDSAbort1Relay && !EDSAbort2Relay) || (!EDSAbort1Relay && !EDSAbort3Relay) || (!EDSAbort2Relay && !EDSAbort3Relay);
}

bool MESC::EDSUnsafeIndicateSignal()
{
	return (!EDSAbort1Relay || !EDSAbort2Relay || !EDSAbort3Relay);
}

bool MESC::ELSActivateLogic()
{ 
	return MESCLogicBus() && ELSActivateRelay && Sat->ELSAutoSwitch.IsUp();
}

void MESC::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "MESCLOGICARM", MESCLogicArm);
	papiWriteScenario_bool(scn, "BOOSTERCUTOFFABORTSTARTRELAY", BoosterCutoffAbortStartRelay);
	papiWriteScenario_bool(scn, "LETPHYSICALSEPARATIONMONITOR", LETPhysicalSeparationMonitor);
	papiWriteScenario_bool(scn, "LESABORTRELAY", LESAbortRelay);
	papiWriteScenario_bool(scn, "AUTOABORTENABLERELAY", AutoAbortEnableRelay);
	papiWriteScenario_bool(scn, "CMSMDEADFACE", CMSMDeadFace);
	papiWriteScenario_bool(scn, "CMSMSEPARATERELAY", CMSMSeparateRelay);
	papiWriteScenario_bool(scn, "PYROCUTOUT", PyroCutout);
	papiWriteScenario_bool(scn, "CMRCSPRESS", CMRCSPress);
	papiWriteScenario_bool(scn, "CANARDDEPLOY", CanardDeploy);
	papiWriteScenario_bool(scn, "ULLAGERELAY", UllageRelay);
	papiWriteScenario_bool(scn, "CSMLVSEPARATERELAY", CSMLVSeparateRelay);
	papiWriteScenario_bool(scn, "LESMOTORFIRE", LESMotorFire);
	papiWriteScenario_bool(scn, "PITCHCONTROLMOTORFIRE", PitchControlMotorFire);
	papiWriteScenario_bool(scn, "RCSENABLEARMRELAY", RCSEnableArmRelay);
	papiWriteScenario_bool(scn, "RCSENABLEDISABLERELAY", RCSEnableDisableRelay);
	papiWriteScenario_bool(scn, "LETJETTISONANDFRANGIBLENUTSRELAY", LETJettisonAndFrangibleNutsRelay);
	papiWriteScenario_bool(scn, "APEXCOVERJETTISON", ApexCoverJettison);
	papiWriteScenario_bool(scn, "APEXCOVERDRAGCHUTEDEPLOY", ApexCoverDragChuteDeploy);
	papiWriteScenario_bool(scn, "ELSACTIVATERELAY", ELSActivateRelay);
	papiWriteScenario_bool(scn, "EDSBUSCHANGEOVER", EDSBusChangeover);
	papiWriteScenario_bool(scn, "EDSABORT1RELAY", EDSAbort1Relay);
	papiWriteScenario_bool(scn, "EDSABORT2RELAY", EDSAbort2Relay);
	papiWriteScenario_bool(scn, "EDSABORT3RELAY", EDSAbort3Relay);
	papiWriteScenario_bool(scn, "ELSACTIVATESOLIDSTATESWITCH", ELSActivateSolidStateSwitch);

	TD1.SaveState(scn, "TD1_BEGIN", "TD1_END");
	TD3.SaveState(scn, "TD3_BEGIN", "TD_END");
	TD5.SaveState(scn, "TD5_BEGIN", "TD_END");
	TD7.SaveState(scn, "TD7_BEGIN", "TD_END");
	TD11.SaveState(scn, "TD11_BEGIN", "TD_END");
	TD13.SaveState(scn, "TD13_BEGIN", "TD_END");
	TD15.SaveState(scn, "TD15_BEGIN", "TD_END");
	TD17.SaveState(scn, "TD17_BEGIN", "TD_END");
	TD23.SaveState(scn, "TD23_BEGIN", "TD_END");

	oapiWriteLine(scn, end_str);
}

void MESC::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	float flt = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "MESCLOGICARM", MESCLogicArm);
		papiReadScenario_bool(line, "BOOSTERCUTOFFABORTSTARTRELAY", BoosterCutoffAbortStartRelay);
		papiReadScenario_bool(line, "LETPHYSICALSEPARATIONMONITOR", LETPhysicalSeparationMonitor);
		papiReadScenario_bool(line, "LESABORTRELAY", LESAbortRelay);
		papiReadScenario_bool(line, "AUTOABORTENABLERELAY", AutoAbortEnableRelay);
		papiReadScenario_bool(line, "CMSMDEADFACE", CMSMDeadFace);
		papiReadScenario_bool(line, "CMSMSEPARATERELAY", CMSMSeparateRelay);
		papiReadScenario_bool(line, "PYROCUTOUT", PyroCutout);
		papiReadScenario_bool(line, "CMRCSPRESS", CMRCSPress);
		papiReadScenario_bool(line, "CANARDDEPLOY", CanardDeploy);
		papiReadScenario_bool(line, "ULLAGERELAY", UllageRelay);
		papiReadScenario_bool(line, "CSMLVSEPARATERELAY", CSMLVSeparateRelay);
		papiReadScenario_bool(line, "LESMOTORFIRE", LESMotorFire);
		papiReadScenario_bool(line, "PITCHCONTROLMOTORFIRE", PitchControlMotorFire);
		papiReadScenario_bool(line, "RCSENABLEARMRELAY", RCSEnableArmRelay);
		papiReadScenario_bool(line, "RCSENABLEDISABLERELAY", RCSEnableDisableRelay);
		papiReadScenario_bool(line, "LETJETTISONANDFRANGIBLENUTSRELAY", LETJettisonAndFrangibleNutsRelay);
		papiReadScenario_bool(line, "APEXCOVERJETTISON", ApexCoverJettison);
		papiReadScenario_bool(line, "APEXCOVERDRAGCHUTEDEPLOY", ApexCoverDragChuteDeploy);
		papiReadScenario_bool(line, "ELSACTIVATERELAY", ELSActivateRelay);
		papiReadScenario_bool(line, "EDSBUSCHANGEOVER", EDSBusChangeover);
		papiReadScenario_bool(line, "EDSABORT1RELAY", EDSAbort1Relay);
		papiReadScenario_bool(line, "EDSABORT2RELAY", EDSAbort2Relay);
		papiReadScenario_bool(line, "EDSABORT3RELAY", EDSAbort3Relay);
		papiReadScenario_bool(line, "ELSACTIVATESOLIDSTATESWITCH", ELSActivateSolidStateSwitch);

		if (!strnicmp(line, "TD1_BEGIN", sizeof("TD1_BEGIN"))) {
			TD1.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD3_BEGIN", sizeof("TD3_BEGIN"))) {
			TD3.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD5_BEGIN", sizeof("TD5_BEGIN"))) {
			TD5.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD7_BEGIN", sizeof("TD7_BEGIN"))) {
			TD7.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD11_BEGIN", sizeof("TD11_BEGIN"))) {
			TD11.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD13_BEGIN", sizeof("TD13_BEGIN"))) {
			TD13.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD15_BEGIN", sizeof("TD15_BEGIN"))) {
			TD15.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD17_BEGIN", sizeof("TD17_BEGIN"))) {
			TD17.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD23_BEGIN", sizeof("TD23_BEGIN"))) {
			TD23.LoadState(scn, "TD_END");
		}
	}
}

LDEC::LDEC():
	TD1(0.03)
{
	SECSPyroBusMotor = false;
	LMSLASeparationInitiate = false;
	DockingProbeRetract2 = false;
	DockingProbeRetract1 = false;
	DockingRingFinalSeparation = false;
	CSM_LEM_LockRingSepRelaySignal = false;
	SIMPyroArmRelay = false;

	SMSector1LogicPowerBreaker = NULL;
}

void LDEC::Init(Saturn *v, MESC* connectedMESC, CircuitBrakerSwitch *SECSArm, CircuitBrakerSwitch* DockProbe,ThreePosSwitch *DockingProbeRetract, ToggleSwitch *PyroArmSw, DCbus *PyroB, PowerMerge *PyroBusFeed)
{
	Sat = v;
	mesc = connectedMESC;
	SECSArmBreaker = SECSArm;
	DockProbeBreaker = DockProbe;
	DockingProbeRetractSwitch = DockingProbeRetract;
	PyroArmSwitch = PyroArmSw;
	PyroBus = PyroB;
	PyroBusFeeder = PyroBusFeed;
}

void LDEC::InitSIMJett(CircuitBrakerSwitch *SMSec1Power)
{
	SMSector1LogicPowerBreaker = SMSec1Power;
}

void LDEC::Timestep(double simdt)
{
	TD1.Timestep(simdt);

	//Pyro Bus Motor

	if (SECSArmBreaker->IsPowered()) {
		if (PyroArmSwitch->IsUp() && !SECSPyroBusMotor) {
			SECSPyroBusMotor = true;
			PyroBus->WireTo(PyroBusFeeder);
		}
		else if (PyroArmSwitch->IsDown() && SECSPyroBusMotor) {
			SECSPyroBusMotor = false;
			PyroBus->Disconnect();
		}
	}

	if (Sat->SIVBPayloadSepSwitch.IsUp() && SequentialArmBus())
	{
		TD1.SetRunning(true);
	}

	if (TD1.ContactClosed())
	{
		LMSLASeparationInitiate = true;
	}

	if (Sat->DockingProbeExtdRelSwitch.IsDown() && DockingProbeRetractSwitch->IsUp() && DockProbeBreaker->IsPowered())
	{
		DockingProbeRetract1 = true;
	}
	else
	{
		DockingProbeRetract1 = false;
	}

	if (Sat->DockingProbeExtdRelSwitch.IsDown() && DockingProbeRetractSwitch->IsDown() && DockProbeBreaker->IsPowered())
	{
		DockingProbeRetract2 = true;
	}
	else
	{
		DockingProbeRetract2 = false;
	}

	if (mesc->MESCLogicBus() && mesc->GetLESAbortRelay() && mesc->GetLETJettisonAndFrangibleNutsRelay())
	{
		DockingRingFinalSeparation = true;
	}
	else if (SequentialArmBus() && (Sat->CsmLmFinalSep1Switch.IsUp() || Sat->CsmLmFinalSep2Switch.IsUp()))
	{
		DockingRingFinalSeparation = true;
	}
	else
	{
		DockingRingFinalSeparation = false;
	}

	//Apollo 15 and later
	if (Sat->Panel181)
	{
		if (SMSector1LogicPowerBreaker)
		{
			if (SMSector1LogicPowerBreaker->IsPowered() && (Sat->Panel181->SMSector1LogicPower1Switch.IsUp() || Sat->Panel181->SMSector1LogicPower2Switch.IsUp()))
			{
				SIMPyroArmRelay = true;
			}
			else
			{
				SIMPyroArmRelay = false;
			}
		}
	}

	//Telemetry
	if (mesc->MESCLogicBus() && DockingRingFinalSeparation)
	{
		CSM_LEM_LockRingSepRelaySignal = true;
	}
	else
	{
		CSM_LEM_LockRingSepRelaySignal = false;
	}

	//sprintf(oapiDebugString(), "MotorSwitch %d LM/SLA Sep %d Probe Retract1 %d Probe Retract2 %d Ring Final Sep %d", SECSPyroBusMotor, LMSLASeparationInitiate, DockingProbeRetract1, DockingProbeRetract2, DockingRingFinalSeparation);
}

bool LDEC::SequentialArmBus()
{
	return SECSArmBreaker->IsPowered();
}

void LDEC::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "SECSPYROBUSMOTOR", SECSPyroBusMotor);
	papiWriteScenario_bool(scn, "LMSLASEPARATIONINITIATE", LMSLASeparationInitiate);
	papiWriteScenario_bool(scn, "DOCKINGPROBERETRACT1", DockingProbeRetract1);
	papiWriteScenario_bool(scn, "DOCKINGPROBERETRACT2", DockingProbeRetract2);
	papiWriteScenario_bool(scn, "DOCKINGRINGFINALSEPARATION", DockingRingFinalSeparation);
	TD1.SaveState(scn, "TD1_BEGIN", "TD_END");

	oapiWriteLine(scn, end_str);
}

void LDEC::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	float flt = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "SECSPYROBUSMOTOR", SECSPyroBusMotor);
		papiReadScenario_bool(line, "LMSLASEPARATIONINITIATE", LMSLASeparationInitiate);
		papiReadScenario_bool(line, "DOCKINGPROBERETRACT1", DockingProbeRetract1);
		papiReadScenario_bool(line, "DOCKINGPROBERETRACT2", DockingProbeRetract2);
		papiReadScenario_bool(line, "DOCKINGRINGFINALSEPARATION", DockingRingFinalSeparation);

		if (!strnicmp(line, "TD1_BEGIN", sizeof("TD1_BEGIN"))) {
			TD1.LoadState(scn, "TD_END");
		}
	}

	// connect pyro bus
	if (SECSPyroBusMotor)
		PyroBus->WireTo(PyroBusFeeder);
	else
		PyroBus->Disconnect();
}

SECS::SECS()
{

	State = 0;
	Sat = 0;

	SMJCA = NULL;
	SMJCB = NULL;
}

SECS::~SECS()
{
	if (SMJCA)
	{
		delete SMJCA;
		SMJCA = NULL;
	}
	if (SMJCB)
	{
		delete SMJCB;
		SMJCB = NULL;
	}
}

void SECS::ControlVessel(Saturn *v)
{
	Sat = v;
	rcsc.ControlVessel(v);
}

void SECS::Realize()
{
	MESCA.Init(Sat, &Sat->SECSLogicBusA, &Sat->PyroBusA, &Sat->MissionTimer306Display, &Sat->MissionTimer306Switch, &Sat->EventTimer306Display, &Sat->EventTimerControl306Switch, &MESCB, true);
	MESCA.CBInit(&Sat->SECSLogicBatACircuitBraker, &Sat->SECSArmBatACircuitBraker, &Sat->RCSLogicMnACircuitBraker, &Sat->ELSBatACircuitBraker, &Sat->EDS1BatACircuitBraker);
	MESCB.Init(Sat, &Sat->SECSLogicBusB, &Sat->PyroBusB, &Sat->MissionTimerDisplay, &Sat->MissionTimerSwitch, &Sat->EventTimerDisplay, &Sat->EventTimerContSwitch, &MESCA, false);
	MESCB.CBInit(&Sat->SECSLogicBatBCircuitBraker, &Sat->SECSArmBatBCircuitBraker, &Sat->RCSLogicMnBCircuitBraker, &Sat->ELSBatBCircuitBraker, &Sat->EDS3BatBCircuitBraker);
	LDECA.Init(Sat, &MESCA, &Sat->SECSArmBatACircuitBraker, &Sat->DockProbeMnACircuitBraker, &Sat->DockingProbeRetractPrimSwitch, &Sat->PyroArmASwitch, &Sat->PyroBusA, &Sat->PyroBusAFeeder);
	LDECB.Init(Sat, &MESCB, &Sat->SECSArmBatBCircuitBraker, &Sat->DockProbeMnBCircuitBraker, &Sat->DockingProbeRetractSecSwitch, &Sat->PyroArmBSwitch, &Sat->PyroBusB, &Sat->PyroBusBFeeder);
}

void SECS::InitSIMJett(CircuitBrakerSwitch *SMSec1PowerA, CircuitBrakerSwitch *SMSec1PowerB)
{
	LDECA.InitSIMJett(SMSec1PowerA);
	LDECB.InitSIMJett(SMSec1PowerB);
}

void SECS::SetSaturnType(int sattype)
{
	if (sattype == SAT_SATURN1B)
	{
		rcsc.SetPropellantDumpInhibitTimers(61.0);
	}
}

void SECS::Timestep(double simt, double simdt)

{
	if (!Sat) return;

	MESCA.Timestep(simdt);
	MESCB.Timestep(simdt);
	rcsc.Timestep(simdt);
	LDECA.Timestep(simdt);
	LDECB.Timestep(simdt);
	if (SMJCA) SMJCA->Timestep(simdt, Sat->MainBusAController.IsSMBusPowered());
	if (SMJCB) SMJCB->Timestep(simdt, Sat->MainBusBController.IsSMBusPowered());

	//
	// CSM LV separation relays
	//

	bool pyroA = false, pyroB = false;

	if (MESCA.GetCSMLVSeparateRelay()) {
		// Blow Pyro A
		pyroA = true;
	}
	if (MESCB.GetCSMLVSeparateRelay()) {
		// Blow Pyro B
		pyroB = true;
	}
	Sat->CSMLVPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
									  (pyroB ? &Sat->PyroBusB : NULL));

	//
	// S-IVB/LM separation relays
	//

	pyroA = false, pyroB = false;

	if (Sat->SIVBPayloadSepSwitch.IsUp()) {
		if (LDECA.GetLMSLASeparationInitiate() && LDECB.GetLMSLASeparationInitiate())
		{
			if (Sat->SIVBLMSepPyroACircuitBraker.IsPowered()) {
				// Blow Pyro A
				pyroA = true;
			}
			if (Sat->SIVBLMSepPyroBCircuitBraker.IsPowered()) {
				// Blow Pyro B
				pyroB = true;
			}
		}
	}
	/// \todo This assumes instantaneous separation of the LM, but it avoids connector calls each time step
	if (pyroA || pyroB) {
		Sat->StartSeparationPyros();
	}

	//
	// SIM Bay Jettison
	//

	if (Sat->Panel181)
	{
		pyroA = false, pyroB = false;

		if (Sat->Panel181->SMSector1DoorJettisonSwitch.IsUp())
		{
			if (LDECA.GetSIMPyroArmRelay() && Sat->PyroBusAFeeder.Voltage() > SP_MIN_DCVOLTAGE)
			{
				pyroA = true;
			}
			if (LDECB.GetSIMPyroArmRelay() && Sat->PyroBusBFeeder.Voltage() > SP_MIN_DCVOLTAGE)
			{
				pyroB = true;
			}
		}

		if (pyroA || pyroB)
		{
			if (!Sat->SIMBayPanelJett)
			{
				Sat->SIMBayPanelJett = true;
				Sat->JettisonSIMBayPanel();
				Sat->SetSIMBayPanelMesh();
			}
		}
	}

	//
	// Docking ring separation relays
	//

	pyroA = false, pyroB = false;

	if (LDECA.GetDockingRingFinalSeparation()) {
		// Blow Pyro A
		pyroA = true;
	}
	if (LDECB.GetDockingRingFinalSeparation()) {
		// Blow Pyro B
		pyroB = true;
	}

	Sat->CMDockingRingPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
											  (pyroB ? &Sat->PyroBusB : NULL));

	//
	// CM/SM separation relays
	//

	pyroA = false, pyroB = false;

	if (MESCA.GetCMSMSeparateRelay())
	{
		// Blow Pyro A
		pyroA = true;

		// Auto Tie Main Buses
		if (Sat->PyroBusA.Voltage() > SP_MIN_DCVOLTAGE) {

			Sat->MainBusAController.SetTieAuto(true);
			Sat->MainBusBController.SetTieAuto(true);
		}
	}

	if (MESCB.GetCMSMSeparateRelay())
	{
		// Blow Pyro B
		pyroB = true;

		// Auto Tie Main Buses
		if (Sat->PyroBusB.Voltage() > SP_MIN_DCVOLTAGE) {

			Sat->MainBusAController.SetTieAuto(true);
			Sat->MainBusBController.SetTieAuto(true);
		}
	}

	Sat->CMSMPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
									 (pyroB ? &Sat->PyroBusB : NULL));

	//
	// Apex Cover Jettison relays
	//

	pyroA = false, pyroB = false;

	if (MESCA.GetApexCoverJettisonRelay()) {
		// Blow Pyro A
		pyroA = true;
	}
	if (MESCB.GetApexCoverJettisonRelay()) {
		// Blow Pyro B
		pyroB = true;
	}
	Sat->ApexCoverPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
		(pyroB ? &Sat->PyroBusB : NULL));

	//
	// Pyros
	//
	
	if (Sat->CMDockingRingPyros.Blown() && Sat->HasProbe && Sat->dockingprobe.IsEnabled())
	{
		if (!Sat->dockingprobe.IsDocked() && !Sat->LESAttached) {
			Sat->JettisonDockingProbe();

		} else if (Sat->GetDockHandle(0)) {
			// Undock, the docking probe remains in the drogue of the other vessel
			Sat->Undock(0);
		}

		//Time to hear the Stage separation
		Sat->SMJetS.play(NOLOOP);
		// Disable docking probe because it's jettisoned 
		Sat->dockingprobe.SetEnabled(false);
		Sat->HasProbe = false;
		Sat->SetDockingProbeMesh();
	}
}

bool SECS::IsLogicPoweredAndArmedA() {

	if (Sat->SECSArmBatACircuitBraker.IsPowered() && (Sat->SECSLogicBusA.Voltage() > SP_MIN_DCVOLTAGE))
		return true;

	return false;
}

bool SECS::IsLogicPoweredAndArmedB() {

	if (Sat->SECSArmBatBCircuitBraker.IsPowered() && (Sat->SECSLogicBusB.Voltage() > SP_MIN_DCVOLTAGE))
		return true;

	return false;
}

bool SECS::AbortLightPowerA()
{
	return MESCA.EDSMainPower();
}

bool SECS::AbortLightPowerB()
{
	return MESCB.EDSMainPower();
}

bool SECS::LiftoffLightPower()
{
	return (MESCA.EDSLiftoffCircuitPower() || MESCB.EDSLiftoffCircuitPower());
}

bool SECS::NoAutoAbortLightPower()
{
	if ((MESCA.EDSLiftoffCircuitPower() && !MESCB.GetAutoAbortEnableRelay()) || (MESCB.EDSLiftoffCircuitPower() && !MESCA.GetAutoAbortEnableRelay()))
		return true;

	return false;
}

SMJC *SECS::GetSMJC(bool isSysA)
{
	if (isSysA) return SMJCA;
	return SMJCB;
}

void SECS::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, SECS_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	rcsc.SaveState(scn, "RCSC_BEGIN", "RCSC_END");
	MESCA.SaveState(scn, "MESCA_BEGIN", "MESC_END");
	MESCB.SaveState(scn, "MESCB_BEGIN", "MESC_END");
	LDECA.SaveState(scn, "LDECA_BEGIN", "LDEC_END");
	LDECB.SaveState(scn, "LDECB_BEGIN", "LDEC_END");
	if (SMJCA) SMJCA->SaveState(scn, SMJCA_START_STRING);
	if (SMJCB) SMJCB->SaveState(scn, SMJCB_START_STRING);
	
	oapiWriteLine(scn, SECS_END_STRING);
}

void SECS::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, SECS_END_STRING, sizeof(SECS_END_STRING)))
			break;

		papiReadScenario_int(line, "STATE", State);
		
		if (!strnicmp(line, "RCSC_BEGIN", sizeof("RCSC_BEGIN"))) {
			rcsc.LoadState(scn, "RCSC_END");
		}
		else if (!strnicmp(line, "MESCA_BEGIN", sizeof("MESCA_BEGIN"))) {
			MESCA.LoadState(scn, "MESC_END");
		}
		else if (!strnicmp(line, "MESCB_BEGIN", sizeof("MESCB_BEGIN"))) {
			MESCB.LoadState(scn, "MESC_END");
		}
		else if (!strnicmp(line, "LDECA_BEGIN", sizeof("LDECA_BEGIN"))) {
			LDECA.LoadState(scn, "LDEC_END");
		}
		else if (!strnicmp(line, "LDECB_BEGIN", sizeof("LDECB_BEGIN"))) {
			LDECB.LoadState(scn, "LDEC_END");
		}
		else if (!strnicmp(line, SMJCA_START_STRING, sizeof(SMJCA_START_STRING))) {
			if (SMJCA) SMJCA->LoadState(scn);
		}
		else if (!strnicmp(line, SMJCB_START_STRING, sizeof(SMJCB_START_STRING))) {
			if (SMJCB) SMJCB->LoadState(scn);
		}
	}
}

BaroSwitch::BaroSwitch(double open, double close)
{
	status = true;
	OpenPa = open;
	ClosePa = close;
}

void BaroSwitch::ControlVessel(Saturn *v)
{
	Sat = v;
}

void BaroSwitch::Timestep()
{
	if (Sat->GetAtmPressure() > ClosePa && !status)
	{
		status = true;
	}
	else if (Sat->GetAtmPressure() < OpenPa && status)
	{
		status = false;
	}
}

PCVB::PCVB()
{
	DrogueChuteDeployA = false;
	DrogueChuteReleasePilotChuteDeployA = false;
	MainChuteReleaseA = false;
	DrogueChuteDeployB = false;
	DrogueChuteReleasePilotChuteDeployB = false;
	MainChuteReleaseB = false;
}

void PCVB::Init(Saturn *v)
{
	Sat = v;
}

void PCVB::Timestep(double simdt)
{
	if (Sat->els.ELSCA.TD1.ContactClosed() || (Sat->DrogueDeploySwitch.GetState() && Sat->els.ELSCA.ELSBatteryPower()))
	{
		DrogueChuteDeployA = true;
	}
	else if (Sat->els.ELSCA.ELSBatteryPower() && DrogueChuteDeployB)
	{
		DrogueChuteDeployA = true;
	}
	else
	{
		DrogueChuteDeployA = false;
	}

	if (Sat->els.ELSCB.TD1.ContactClosed() || (Sat->DrogueDeploySwitch.GetState() && Sat->els.ELSCB.ELSBatteryPower()))
	{
		DrogueChuteDeployB = true;
	}
	else if (Sat->els.ELSCB.ELSBatteryPower() && DrogueChuteDeployA)
	{
		DrogueChuteDeployB = true;
	}
	else
	{
		DrogueChuteDeployB = false;
	}

	if ((Sat->els.ELSCA.TD3.ContactClosed() && Sat->els.BaroSwitch10k.IsClosed()) || (Sat->els.ELSCA.ELSBatteryPower() && Sat->MainDeploySwitch.GetState()))
	{
		DrogueChuteReleasePilotChuteDeployA = true;
	}
	else if (Sat->els.ELSCA.ELSBatteryPower() &&(DrogueChuteReleasePilotChuteDeployA || DrogueChuteReleasePilotChuteDeployB))
	{
		DrogueChuteReleasePilotChuteDeployA = true;
	}
	else
	{
		DrogueChuteReleasePilotChuteDeployA = false;
	}

	if ((Sat->els.ELSCB.TD3.ContactClosed() && Sat->els.BaroSwitch10k.IsClosed()) || (Sat->els.ELSCB.ELSBatteryPower() && Sat->MainDeploySwitch.GetState()))
	{
		DrogueChuteReleasePilotChuteDeployB = true;
	}
	else if (Sat->els.ELSCB.ELSBatteryPower() && (DrogueChuteReleasePilotChuteDeployA || DrogueChuteReleasePilotChuteDeployB))
	{
		DrogueChuteReleasePilotChuteDeployB = true;
	}
	else
	{
		DrogueChuteReleasePilotChuteDeployB = false;
	}

	if (Sat->els.ELSCA.TD3.ContactClosed() && Sat->els.BaroSwitch10k.IsClosed() && Sat->MainReleaseSwitch.IsUp())
	{
		MainChuteReleaseA = true;
	}
	else
	{
		MainChuteReleaseA = false;
	}

	if (Sat->els.ELSCB.TD3.ContactClosed() && Sat->els.BaroSwitch10k.IsClosed() && Sat->MainReleaseSwitch.IsUp())
	{
		MainChuteReleaseB = true;
	}
	else
	{
		MainChuteReleaseB = false;
	}
}

void PCVB::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "DROGUECHUTEDEPLOYA", DrogueChuteDeployA);
	papiWriteScenario_bool(scn, "DROGUECHUTERELEASEPILOTCHUTEDEPLOYA", DrogueChuteReleasePilotChuteDeployA);
	papiWriteScenario_bool(scn, "MAINCHUTERELEASEA", MainChuteReleaseA);
	papiWriteScenario_bool(scn, "DROGUECHUTEDEPLOYB", DrogueChuteDeployB);
	papiWriteScenario_bool(scn, "DROGUECHUTERELEASEPILOTCHUTEDEPLOYB", DrogueChuteReleasePilotChuteDeployB);
	papiWriteScenario_bool(scn, "MAINCHUTERELEASEB", MainChuteReleaseB);

	oapiWriteLine(scn, end_str);
}

void PCVB::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	float flt = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}
		papiReadScenario_bool(line, "DROGUECHUTEDEPLOYA", DrogueChuteDeployA);
		papiReadScenario_bool(line, "DROGUECHUTERELEASEPILOTCHUTEDEPLOYA", DrogueChuteReleasePilotChuteDeployA);
		papiReadScenario_bool(line, "MAINCHUTERELEASEA", MainChuteReleaseA);
		papiReadScenario_bool(line, "DROGUECHUTEDEPLOYB", DrogueChuteDeployB);
		papiReadScenario_bool(line, "DROGUECHUTERELEASEPILOTCHUTEDEPLOYB", DrogueChuteReleasePilotChuteDeployB);
		papiReadScenario_bool(line, "MAINCHUTERELEASEB", MainChuteReleaseB);
	}
}

ELSC::ELSC():
	TD1(2.0),
	TD3(14.0)
{
	BaroswitchLockIn = false;
	DrogueParachuteDeploy = false;
	PilotParachuteDeploy = false;

	ELSBatteryBreaker = NULL;

	OtherELSC = NULL;
	mesc = NULL;

	Sat = NULL;
	//ELSCDisplay = NULL;
}

void ELSC::Init(Saturn *v, CircuitBrakerSwitch *ELSBatteryCB, MESC* ConnectedMESC, ELSC *OtherELSCSystem)
{
	ELSBatteryBreaker = ELSBatteryCB;
	mesc = ConnectedMESC;
	OtherELSC = OtherELSCSystem;
	Sat = v;

	//ELSCDisplay = oapiCreateAnnotation(false, 0.65, _V(1, 1, 0));
	//oapiAnnotationSetPos(ELSCDisplay, 0.33, 0.1, 0.66, 1);
}

void ELSC::TimerTimestep(double simdt)
{
	TD1.Timestep(simdt);
	TD3.Timestep(simdt);
}

void ELSC::Timestep(double simdt)
{
	TimerTimestep(simdt);

	//24,000 feet Lock Up
	if (mesc->ELSActivateLogic() && (Sat->els.BaroSwitch24k.IsClosed() || BaroswitchLockIn || OtherELSC->BaroswitchLockIn))
	{
		BaroswitchLockIn = true;
	}
	else
	{
		BaroswitchLockIn = false;
	}

	if (BaroswitchLockIn)
	{
		TD1.SetRunning(true);
	}

	//Drogue Deploy Logic
	if ((TD1.ContactClosed() && !Sat->LandFail.DrogueFail) || (Sat->DrogueDeploySwitch.GetState() && ELSBatteryPower()))
	{
		DrogueParachuteDeploy = true;
	}
	else
	{
		DrogueParachuteDeploy = false;
	}

	if (mesc->ELSActivateLogic())
	{
		TD3.SetRunning(true);
	}

	if ((TD3.ContactClosed() && Sat->els.BaroSwitch10k.IsClosed() && !Sat->LandFail.MainFail) || (ELSBatteryPower() && Sat->MainDeploySwitch.GetState()))
	{
		PilotParachuteDeploy = true;
	}
	else
	{
		PilotParachuteDeploy = false;
	}

	//Mode 1A Display
	/*char buffer[1024];
	sprintf(buffer, "EARTH LANDING SEQUENCE CONTROLLER - MODE 1A\n\n");
	if (BaroswitchLockIn)
	{
		sprintf(buffer, "%sBaroswitch Lock-In Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sBaroswitch Lock-In Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sDrogue Parachute Timer %.2f s\n", buffer, TD1.GetTime());
	if (DrogueParachuteDeploy)
	{
		sprintf(buffer, "%sDrogue Parachute Deploy Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sDrogue Parachute Deploy Relay - Deenergized\n", buffer);
	}
	sprintf(buffer, "%sMain Parachute Timer %.2f s\n", buffer, TD3.GetTime());
	if (PilotParachuteDeploy)
	{
		sprintf(buffer, "%sMain Parachute Deploy Relay - Energized\n", buffer);
	}
	else
	{
		sprintf(buffer, "%sMain Parachute Deploy Relay - Deenergized\n", buffer);
	}

	oapiAnnotationSetText(ELSCDisplay, buffer);*/
}

bool ELSC::ELSBatteryPower()
{
	return (ELSBatteryBreaker->Voltage() > SP_MIN_DCVOLTAGE);
}

void ELSC::SaveState(FILEHANDLE scn, char *start_str, char *end_str)
{
	oapiWriteLine(scn, start_str);

	papiWriteScenario_bool(scn, "BAROSWITCHLOCKIN", BaroswitchLockIn);
	papiWriteScenario_bool(scn, "DROGUEPARACHUTEDEPLOY", DrogueParachuteDeploy);
	papiWriteScenario_bool(scn, "PILOTPARACHUTEDEPLOY", PilotParachuteDeploy);
	TD1.SaveState(scn, "TD1_BEGIN", "TD1_END");
	TD3.SaveState(scn, "TD3_BEGIN", "TD_END");

	oapiWriteLine(scn, end_str);
}

void ELSC::LoadState(FILEHANDLE scn, char *end_str)

{
	char *line;
	float flt = 0;
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		papiReadScenario_bool(line, "BAROSWITCHLOCKIN", BaroswitchLockIn);
		papiReadScenario_bool(line, "DROGUEPARACHUTEDEPLOY", DrogueParachuteDeploy);
		papiReadScenario_bool(line, "PILOTPARACHUTEDEPLOY", PilotParachuteDeploy);

		if (!strnicmp(line, "TD1_BEGIN", sizeof("TD1_BEGIN"))) {
			TD1.LoadState(scn, "TD_END");
		}
		else if (!strnicmp(line, "TD3_BEGIN", sizeof("TD3_BEGIN"))) {
			TD3.LoadState(scn, "TD_END");
		}

	}
}

ELS::ELS() :
	BaroSwitch10k(50634.2, 69697.4),
	BaroSwitch24k(18823.7, 39319.4)
{
	State = 0;

	FloatBag1Size = 0;
	FloatBag2Size = 0;
	FloatBag3Size = 0;

	Sat = 0;
	FloatBagVessel = 0;

	DyeMarkerLevel = 0;
	DyeMarkerTime = 3600; // 1 hour
}

ELS::~ELS()

{
}

void ELS::ControlVessel(Saturn *v)

{
	Sat = v;
	BaroSwitch10k.ControlVessel(Sat);
	BaroSwitch24k.ControlVessel(Sat);
	ELSCA.Init(Sat, &Sat->ELSBatACircuitBraker, &Sat->secs.MESCA, &ELSCB);
	ELSCB.Init(Sat, &Sat->ELSBatBCircuitBraker, &Sat->secs.MESCB, &ELSCA);
	pcvb.Init(Sat);
}

void ELS::Timestep(double simt, double simdt)

{
	if (!Sat)	return;

	BaroSwitch10k.Timestep();
	BaroSwitch24k.Timestep();
	ELSCA.Timestep(simdt);
	ELSCB.Timestep(simdt);
	pcvb.Timestep(simdt);

	//
	// Drogue Chute Deploy relays
	//

	bool pyroA = false, pyroB = false;

	if (ELSCA.GetDrogueParachuteDeployRelay() && (pcvb.GetDrogueChuteDeployA() || pcvb.GetDrogueChuteDeployB())) {
		// Blow Pyro A
		pyroA = true;
	}
	if (ELSCB.GetDrogueParachuteDeployRelay() && (pcvb.GetDrogueChuteDeployA() || pcvb.GetDrogueChuteDeployB())) {
		// Blow Pyro B
		pyroB = true;
	}
	Sat->DrogueChutesDeployPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
		(pyroB ? &Sat->PyroBusB : NULL));

	//
	// Main Chute Deploy relays
	//

	pyroA = false, pyroB = false;

	if (ELSCA.GetMainParachuteDeployRelay() && (pcvb.GetMainChuteDeployA() || pcvb.GetMainChuteDeployB())) {
		// Blow Pyro A
		pyroA = true;
	}
	if (ELSCB.GetMainParachuteDeployRelay() && (pcvb.GetMainChuteDeployA() || pcvb.GetMainChuteDeployB())) {
		// Blow Pyro B
		pyroB = true;
	}
	Sat->MainChutesDeployPyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
		(pyroB ? &Sat->PyroBusB : NULL));


	//
	// Main Chute Release relays
	//

	pyroA = false, pyroB = false;

	if (pcvb.GetMainChuteReleaseA() && Sat->MainReleasePyroACircuitBraker.IsPowered()) {
		// Blow Pyro A
		pyroA = true;
	}
	if (pcvb.GetMainChuteReleaseB() && Sat->MainReleasePyroBCircuitBraker.IsPowered()) {
		// Blow Pyro B
		pyroB = true;
	}
	Sat->MainChutesReleasePyrosFeeder.WireToBuses((pyroA ? &Sat->PyroBusA : NULL),
		(pyroB ? &Sat->PyroBusB : NULL));


	//
	// Float Bags
	//

	if (!Sat->ApexCoverAttached) {
		VESSELSTATUS vs;
		Sat->GetStatus(vs);

		if (!FloatBagVessel) {
			char VName[256]="";
			Sat->GetApolloName(VName);
			strcat(VName, "-FLOATBAG");
			OBJHANDLE hFloatBag = oapiGetVesselByName(VName);

			// Create the float bag vessel
			if (!hFloatBag) {
				OBJHANDLE hFloatBag = oapiCreateVessel(VName, "ProjectApollo/FloatBag", vs);
				FloatBagVessel = (FloatBag *) oapiGetVesselInterface(hFloatBag);
				// Attach it
				ATTACHMENTHANDLE ah = Sat->GetAttachmentHandle(false, 0);
				ATTACHMENTHANDLE ahc = FloatBagVessel->GetAttachmentHandle(true, 0);
				Sat->AttachChild(hFloatBag, ah, ahc);
			} else {
				FloatBagVessel = (FloatBag *) oapiGetVesselInterface(hFloatBag);
			}
		}

		// Float Bag sizes
		FloatBag1Size = NewFloatBagSize(FloatBag1Size, &Sat->FloatBagSwitch1, &Sat->FloatBag1BatACircuitBraker, simdt);
		FloatBag2Size = NewFloatBagSize(FloatBag2Size, &Sat->FloatBagSwitch2, &Sat->FloatBag2BatBCircuitBraker, simdt);
		FloatBag3Size = NewFloatBagSize(FloatBag3Size, &Sat->FloatBagSwitch3, &Sat->FloatBag3FLTPLCircuitBraker, simdt);
		FloatBagVessel->SetBagSize(1, FloatBag1Size);
		FloatBagVessel->SetBagSize(2, FloatBag2Size);
		FloatBagVessel->SetBagSize(3, FloatBag3Size);

		// Beacon
		if (Sat->GetStage() >= CM_ENTRY_STAGE_SIX) {
			// Extend beacon automatically
			FloatBagVessel->ExtendBeacon();
			// Control the light
			if (Sat->FloatBag3FLTPLCircuitBraker.Voltage() < SP_MIN_DCVOLTAGE || Sat->PostLandingBCNLTSwitch.IsCenter()) {
				FloatBagVessel->SetBeaconLight(false, false);
			} else if (Sat->PostLandingBCNLTSwitch.IsDown()) {
				FloatBagVessel->SetBeaconLight(true, false);
			} else {
				FloatBagVessel->SetBeaconLight(true, true);
			}
		}

		// Dye marker
		if (Sat->GetStage() >= CM_ENTRY_STAGE_SEVEN && DyeMarkerTime > 0) {
			// Turned on?			
			if (Sat->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->PostLandingDYEMarkerSwitch.IsUp()) { 
				DyeMarkerLevel = 1;
				DyeMarkerTime -= simdt;
			} else {
				DyeMarkerLevel = 0;
			}
		} else {
			DyeMarkerLevel = 0;
		}
	}
}

double ELS::NewFloatBagSize(double size, ThreePosSwitch *sw, CircuitBrakerSwitch *cb, double simdt) 

{
	if (cb->Voltage() > SP_MIN_DCVOLTAGE) {
		if (sw->IsDown()) {
			size -= simdt / (7. * 60.);	// same as fill? 
			size = max(0, size);
		} else if (sw->IsUp() && (Sat->UprightingSystemCompressor1CircuitBraker.IsPowered() || Sat->UprightingSystemCompressor1CircuitBraker.IsPowered())) {
			size += simdt / (7. * 60.);	// Apollo 15 entry checklist
			size = min(1, size);
		}
	}
	return size;
}

void ELS::SystemTimestep(double simdt) 

{
	// Float bag compressor 1
	bool comp = false;
	if (Sat->FloatBag1BatACircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->FloatBagSwitch1.IsUp()) {
		comp = true;
	}

	// Float bag compressor 2 
	if ((Sat->FloatBag2BatBCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->FloatBagSwitch2.IsUp()) ||
		(Sat->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE && Sat->FloatBagSwitch3.IsUp())) {
		comp = true;
	}

	if (comp) {
		if (Sat->UprightingSystemCompressor1CircuitBraker.IsPowered()) {
			Sat->UprightingSystemCompressor1CircuitBraker.DrawPower(424); // Systems handbook
		}
		if (Sat->UprightingSystemCompressor2CircuitBraker.IsPowered()) {
			Sat->UprightingSystemCompressor2CircuitBraker.DrawPower(424); // Systems handbook
		}
	}

	// Beacon light
	if (Sat->GetStage() >= CM_ENTRY_STAGE_SIX && Sat->FloatBag3FLTPLCircuitBraker.Voltage() > SP_MIN_DCVOLTAGE) {
		// LO/HI
		if (Sat->PostLandingBCNLTSwitch.IsDown()) {
			Sat->FloatBag3FLTPLCircuitBraker.DrawPower(10);	// guessed
		} else if (Sat->PostLandingBCNLTSwitch.IsUp()) {
			Sat->FloatBag3FLTPLCircuitBraker.DrawPower(40);	// guessed
		}
	}

	// Dye marker
	if (DyeMarkerLevel == 1) {
		Sat->FloatBag3FLTPLCircuitBraker.DrawPower(10);	// guessed
	}
}

void ELS::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, ELS_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);
	oapiWriteScenario_int(scn, "BAROSWITCH10K", BaroSwitch10k.GetStatus());
	oapiWriteScenario_int(scn, "BAROSWITCH24K", BaroSwitch24k.GetStatus());
	oapiWriteScenario_float(scn, "FLOATBAG1SIZE", FloatBag1Size);
	oapiWriteScenario_float(scn, "FLOATBAG2SIZE", FloatBag2Size);
	oapiWriteScenario_float(scn, "FLOATBAG3SIZE", FloatBag3Size);
	oapiWriteScenario_float(scn, "DYEMARKERLEVEL", DyeMarkerLevel);
	oapiWriteScenario_float(scn, "DYEMARKERTIME", DyeMarkerTime);
	ELSCA.SaveState(scn, "ELSCA_BEGIN", "ELSC_END");
	ELSCB.SaveState(scn, "ELSCB_BEGIN", "ELSC_END");
	pcvb.SaveState(scn, "PCVB_BEGIN", "PCVB_END");

	oapiWriteLine(scn, ELS_END_STRING);
}

void ELS::LoadState(FILEHANDLE scn)

{
	char *line;
	float flt = 0;
	int it = 0;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, ELS_END_STRING, sizeof(ELS_END_STRING)))
			return;

		if (!strnicmp (line, "STATE", 5)) {
			sscanf (line + 5, "%d", &State);
		}
		else if (!strnicmp(line, "BAROSWITCH10K", 13)) {
			sscanf(line + 13, "%d", &it);
			BaroSwitch10k.SetStatus(it);
		}
		else if (!strnicmp(line, "BAROSWITCH24K", 13)) {
			sscanf(line + 13, "%d", &it);
			BaroSwitch24k.SetStatus(it);
		}
		else if (!strnicmp (line, "FLOATBAG1SIZE", 13)) {
			sscanf (line+13, "%lf", &FloatBag1Size);
		}
		else if (!strnicmp (line, "FLOATBAG2SIZE", 13)) {
			sscanf (line+13, "%lf", &FloatBag2Size);
		}
		else if (!strnicmp (line, "FLOATBAG3SIZE", 13)) {
			sscanf (line+13, "%lf", &FloatBag3Size);
		}
		else if (!strnicmp (line, "DYEMARKERLEVEL", 14)) {
			sscanf (line+14, "%lf", &DyeMarkerLevel);
		}
		else if (!strnicmp (line, "DYEMARKERTIME", 13)) {
			sscanf (line+13, "%lf", &DyeMarkerTime);
		}
		else if (!strnicmp(line, "ELSCA_BEGIN", sizeof("ELSCA_BEGIN"))) {
			ELSCA.LoadState(scn, "ELSC_END");
		}
		else if (!strnicmp(line, "ELSCB_BEGIN", sizeof("ELSCB_BEGIN"))) {
			ELSCB.LoadState(scn, "ELSC_END");
		}
		else if (!strnicmp(line, "PCVB_BEGIN", sizeof("PCVB_BEGIN"))) {
			pcvb.LoadState(scn, "PCVB_END");
		}
	}
}
