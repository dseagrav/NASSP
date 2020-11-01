/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017-2019

Saturn Emergency Detection System 602A5

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

#include "ioChannels.h"
#include "LVIMU.h"
#include "iu.h"

#include "soundlib.h"

#include "saturn.h"
#include "papi.h"

#include "EDS.h"

EDS::EDS(IU *iu)
{
	this->iu = iu;

	TwoEngineOutAutoAbortInhibitNo1 = false;
	TwoEngineOutAutoAbortInhibitNo2 = false;
	TwoEngineOutAutoAbortInhibitNo3 = false;
	ExcessiveRateAutoAbortInhibitPY1 = false;
	ExcessiveRateAutoAbortInhibitPY2 = false;
	ExcessiveRateAutoAbortInhibitPY3 = false;
	ExcessiveRateAutoAbortInhibitR1 = false;
	ExcessiveRateAutoAbortInhibitR2 = false;
	ExcessiveRateAutoAbortInhibitR3 = false;
	LVEnginesCutoffEnable1 = false;
	LVEnginesCutoffEnable2 = false;
	SIVBEngineOutIndicationA = false;
	SIVBEngineOutIndicationB = false;
	EDSLiftoffEnableA = false;
	EDSLiftoffEnableB = false;
	EDSLiftoffInhibitA = false;
	EDSLiftoffInhibitB = false;
	LiftoffA = false;
	LiftoffB = false;
	LVEnginesCutoffFromSC1 = false;
	LVEnginesCutoffFromSC2 = false;
	LVEnginesCutoffFromSC3 = false;
	SIVBEngineCutoffDisabled = false;
	SIEDSCutoff = false;
	SIVBEDSCutoff = false;
	EDSAbortSignal1 = false;
	EDSAbortSignal2 = false;
	EDSAbortSignal3 = false;
	SIVBRestartAlert = false;
	LiftoffRelay = false;
	SCControlEnableRelay = false;
	LVAttRefFail1 = false;
	LVAttRefFail2 = false;
	IUCommandSystemEnable = false;
	LVEnginesCutoffCommand1 = false;
	LVEnginesCutoffCommand2 = false;
	LVEnginesCutoffCommand3 = false;
	GSEEngineThrustIndicationEnableA = false;
	GSEEngineThrustIndicationEnableB = false;
	SIVBEngineThrustMonitorA = false;
	SIVBEngineThrustMonitorB = false;
	PadAbortRequest = false;
	RangeSafetyDestructArmedAFromSIVB = false;
	RangeSafetyDestructArmedBFromSIVB = false;
	ExcessiveRollRateVotingA = false;
	ExcessiveRollRateVotingB = false;
	ExcessivePitchRateVotingA = false;
	ExcessivePitchRateVotingB = false;
	ExcessiveYawRateVotingA = false;
	ExcessiveYawRateVotingB = false;
	ExcessiveRollRateIndication = false;
	ExcessivePitchYawRateIndicationA = false;
	ExcessivePitchYawRateIndicationB = false;
	SIAllEnginesOKA = false;
	SIAllEnginesOKB = false;
	AutoAbort1AToSC = false;
	AutoAbort1BToSC = false;
	AutoAbort2AToSC = false;
	AutoAbort2BToSC = false;
	AutoAbort3AToSC = false;
	AutoAbort3BToSC = false;
	AutoAbortBusGSEMonitor = false;

	AutoAbortBus = false;
	IUEDSBusPowered = true;

	AbortLightSignal = false;

	PlatformFailure = false;
	PlatformFailureTime = 0.0;
	LiftoffCircuitAFailure = false;
	LiftoffCircuitBFailure = false;

	LVRateAutoSwitchOff = false;
	TwoEngineOutAutoSwitchOff = false;
	Stage = 0;
	EDSBus1Powered = false;
	EDSBus2Powered = false;
	EDSBus3Powered = false;
	BECOA = false;
	BECOB = false;
}

void EDS::Timestep(double simdt)
{
	bool tempsignal1, tempsignal2;

	if (iu->GetAuxPowrDistr()->IsIUEDSBusPowered())
		IUEDSBusPowered = true;
	else
		IUEDSBusPowered = false;

	if (iu->ESEGetCommandVehicleLiftoffIndicationInhibit())
		LiftoffRelay = true;
	else
		LiftoffRelay = false;

	if (iu->ESEPadAbortRequest())
		PadAbortRequest = true;
	else
		PadAbortRequest = false;

	if (iu->ESEGetEngineThrustIndicationEnableInhibitA())
		GSEEngineThrustIndicationEnableA = true;
	else
		GSEEngineThrustIndicationEnableA = false;

	if (iu->ESEGetEngineThrustIndicationEnableInhibitB())
		GSEEngineThrustIndicationEnableB = true;
	else
		GSEEngineThrustIndicationEnableB = false;

	if (iu->ESEEDSLiftoffInhibitA())
		EDSLiftoffInhibitA = true;
	else
		EDSLiftoffInhibitA = false;

	if (iu->ESEEDSLiftoffInhibitB())
		EDSLiftoffInhibitB = true;
	else
		EDSLiftoffInhibitB = false;

	//S-IVB Thrust Monitor
	if (IUEDSBusPowered && SIVBEngineOutIndicationA && iu->GetLVCommandConnector()->GetSIVBThrustOK() == false)
		SIVBEngineThrustMonitorA = true;
	else
		SIVBEngineThrustMonitorA = false;

	if (IUEDSBusPowered && SIVBEngineOutIndicationB && iu->GetLVCommandConnector()->GetSIVBThrustOK() == false)
		SIVBEngineThrustMonitorB = true;
	else
		SIVBEngineThrustMonitorB = false;

	//Input signals
	LVRateAutoSwitchOff = iu->GetCommandConnector()->LVRateAutoSwitchState() == TOGGLESWITCH_DOWN;
	TwoEngineOutAutoSwitchOff = iu->GetCommandConnector()->TwoEngineOutAutoSwitchState() == TOGGLESWITCH_DOWN;
	Stage = iu->GetLVCommandConnector()->GetStage();
	EDSBus1Powered = iu->GetCommandConnector()->IsEDSBusPowered(1);
	EDSBus2Powered = iu->GetCommandConnector()->IsEDSBusPowered(2);
	EDSBus3Powered = iu->GetCommandConnector()->IsEDSBusPowered(3);
	BECOA = iu->GetCommandConnector()->GetBECOCommand(true);
	BECOB = iu->GetCommandConnector()->GetBECOCommand(false);

	//Auto Abort Relays
	//Two-Engines Out
	tempsignal1 = iu->GetControlDistributor()->GetTwoEnginesOutAutoAbortInhibit() || (IUEDSBusPowered && TwoEngineOutAutoSwitchOff);
	if (iu->ESEGetTwoEngineOutAutoAbortInhibit(1) || tempsignal1)
		TwoEngineOutAutoAbortInhibitNo1 = true;
	else
		TwoEngineOutAutoAbortInhibitNo1 = false;

	if (iu->ESEGetTwoEngineOutAutoAbortInhibit(2) || tempsignal1)
		TwoEngineOutAutoAbortInhibitNo2 = true;
	else
		TwoEngineOutAutoAbortInhibitNo2 = false;

	if (iu->ESEGetTwoEngineOutAutoAbortInhibit(3) || tempsignal1)
		TwoEngineOutAutoAbortInhibitNo2 = true;
	else
		TwoEngineOutAutoAbortInhibitNo3 = false;

	//Excessive Rate
	tempsignal1 = iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitchOff);
	if (iu->ESEGetExcessivePitchYawRateAutoAbortInhibit(1) || tempsignal1)
		ExcessiveRateAutoAbortInhibitPY1 = true;
	else
		ExcessiveRateAutoAbortInhibitPY1 = false;

	if (iu->ESEGetExcessivePitchYawRateAutoAbortInhibit(2) || tempsignal1)
		ExcessiveRateAutoAbortInhibitPY2 = true;
	else
		ExcessiveRateAutoAbortInhibitPY2 = false;

	if (iu->ESEGetExcessivePitchYawRateAutoAbortInhibit(3) || tempsignal1)
		ExcessiveRateAutoAbortInhibitPY3 = true;
	else
		ExcessiveRateAutoAbortInhibitPY3 = false;

	tempsignal1 = iu->GetControlDistributor()->GetExcessiveRatePYRAutoAbortInhibit() || iu->GetControlDistributor()->GetExcessiveRateRollAutoAbortInhibit() || (IUEDSBusPowered && LVRateAutoSwitchOff);
	if (iu->ESEGetExcessiveRollRateAutoAbortInhibit(1) || tempsignal1)
		ExcessiveRateAutoAbortInhibitR1 = true;
	else
		ExcessiveRateAutoAbortInhibitR1 = false;

	if (iu->ESEGetExcessiveRollRateAutoAbortInhibit(2) || tempsignal1)
		ExcessiveRateAutoAbortInhibitR2 = true;
	else
		ExcessiveRateAutoAbortInhibitR2 = false;

	if (iu->ESEGetExcessiveRollRateAutoAbortInhibit(3) || tempsignal1)
		ExcessiveRateAutoAbortInhibitR3 = true;
	else
		ExcessiveRateAutoAbortInhibitR3 = false;

	//Backup LV Engines Cutoff Enable
	if (LVEnginesCutoffCommand2 == false && iu->GetEngineCutoffEnableTimer()->ContactClosed())
	{
		LVEnginesCutoffEnable2 = true;
	}

	//LV Engines EDS Cutoff Inhibit
	if ((IUEDSBusPowered && !BECOA) || iu->ESEGetEDSLVCutoffSimulate(1))
		LVEnginesCutoffFromSC1 = true;
	else
		LVEnginesCutoffFromSC1 = false;

	if ((IUEDSBusPowered && (!BECOA || !BECOB)) || iu->ESEGetEDSLVCutoffSimulate(2))
		LVEnginesCutoffFromSC2 = true;
	else
		LVEnginesCutoffFromSC2 = false;

	if ((IUEDSBusPowered && !BECOB) || iu->ESEGetEDSLVCutoffSimulate(3))
		LVEnginesCutoffFromSC3 = true;
	else
		LVEnginesCutoffFromSC3 = false;

	//LV Engines Cutoff Command
	if (IUEDSBusPowered && LVEnginesCutoffEnable1 && TripleVoting(!LVEnginesCutoffFromSC1, !LVEnginesCutoffFromSC2, !LVEnginesCutoffFromSC3))
		LVEnginesCutoffCommand1 = true;
	else
		LVEnginesCutoffCommand1 = false;

	if (IUEDSBusPowered && TripleVoting(!LVEnginesCutoffFromSC1, !LVEnginesCutoffFromSC2, !LVEnginesCutoffFromSC3))
		LVEnginesCutoffCommand2 = true;
	else
		LVEnginesCutoffCommand2 = false;

	if (IUEDSBusPowered && LVEnginesCutoffEnable2 && TripleVoting(!LVEnginesCutoffFromSC1, !LVEnginesCutoffFromSC2, !LVEnginesCutoffFromSC3))
		LVEnginesCutoffCommand3 = true;
	else
		LVEnginesCutoffCommand3 = false;

	//Overrate Auto Abort
	//Roll
	if (iu->ESEGetGSEOverrateSimulate(3) || IUEDSBusPowered && iu->GetContSigProc()->GetRollNo3Overrate())
		ExcessiveRollRateVotingA = true;
	else
		ExcessiveRollRateVotingA = false;

	tempsignal1 = iu->ESEGetGSEOverrateSimulate(2) || IUEDSBusPowered && iu->GetContSigProc()->GetRollNo2Overrate();

	if (tempsignal1)
		ExcessiveRollRateVotingB = true;
	else
		ExcessiveRollRateVotingB = false;

	tempsignal2 = iu->ESEGetGSEOverrateSimulate(1) || IUEDSBusPowered && iu->GetContSigProc()->GetRollNo1Overrate();

	if ((tempsignal1 && ExcessiveRollRateVotingA) || (tempsignal2 && ExcessiveRollRateVotingB))
		ExcessiveRollRateIndication = true;
	else
		ExcessiveRollRateIndication = false;

	//Pitch
	if (iu->ESEGetGSEOverrateSimulate(6) || IUEDSBusPowered && iu->GetContSigProc()->GetPitchNo3Overrate())
		ExcessivePitchRateVotingA = true;
	else
		ExcessivePitchRateVotingA = false;

	tempsignal1 = iu->ESEGetGSEOverrateSimulate(5) || IUEDSBusPowered && iu->GetContSigProc()->GetPitchNo2Overrate();

	if (tempsignal1)
		ExcessivePitchRateVotingB = true;
	else
		ExcessivePitchRateVotingB = false;

	tempsignal2 = iu->ESEGetGSEOverrateSimulate(4) || IUEDSBusPowered && iu->GetContSigProc()->GetPitchNo1Overrate();

	if ((tempsignal1 && ExcessivePitchRateVotingA) || (tempsignal2 && ExcessivePitchRateVotingB))
		ExcessivePitchYawRateIndicationA = true;
	else
		ExcessivePitchYawRateIndicationA = false;

	//Yaw
	if (iu->ESEGetGSEOverrateSimulate(9) || IUEDSBusPowered && iu->GetContSigProc()->GetYawNo3Overrate())
		ExcessiveYawRateVotingA = true;
	else
		ExcessiveYawRateVotingA = false;

	tempsignal1 = iu->ESEGetGSEOverrateSimulate(8) || IUEDSBusPowered && iu->GetContSigProc()->GetYawNo2Overrate();

	if (tempsignal1)
		ExcessiveYawRateVotingB = true;
	else
		ExcessiveYawRateVotingB = false;

	tempsignal2 = iu->ESEGetGSEOverrateSimulate(7) || IUEDSBusPowered && iu->GetContSigProc()->GetYawNo1Overrate();

	if ((tempsignal1 && ExcessiveYawRateVotingA) || (tempsignal2 && ExcessiveYawRateVotingB))
		ExcessivePitchYawRateIndicationB = true;
	else
		ExcessivePitchYawRateIndicationB = false;

	//Both relays gets the same signal, so they enable each other for easier logic
	if (ExcessivePitchYawRateIndicationA && !ExcessivePitchYawRateIndicationB) ExcessivePitchYawRateIndicationB = true;
	if (ExcessivePitchYawRateIndicationB && !ExcessivePitchYawRateIndicationA) ExcessivePitchYawRateIndicationA = true;

	//Auto Abort Logic
	AutoAbortBus = false;
	if (TripleVoting(ExcessiveRateAutoAbortInhibitPY1, ExcessiveRateAutoAbortInhibitPY2, ExcessiveRateAutoAbortInhibitPY3) == false && (ExcessivePitchYawRateIndicationA || ExcessivePitchYawRateIndicationB)) AutoAbortBus = true;
	if (TripleVoting(ExcessiveRateAutoAbortInhibitR1, ExcessiveRateAutoAbortInhibitR2, ExcessiveRateAutoAbortInhibitR3) == false && ExcessiveRollRateIndication) AutoAbortBus = true;

	//LV Guidance Light
	if (iu->GetLVDA()->GetGuidanceReferenceFailure())
	{
		LVAttRefFail1 = true;
		LVAttRefFail2 = true;
	}
	else
	{
		LVAttRefFail1 = false;
		LVAttRefFail2 = false;
	}

	if ((LVAttRefFail1 || LVAttRefFail2) && iu->GetSCControlPoweredFlight())
	{
		SCControlEnableRelay = true;
	}

	if (LVAttRefFail1 && (EDSBus1Powered || EDSBus3Powered))
		iu->GetCommandConnector()->SetLVGuidLight();
	else
		iu->GetCommandConnector()->ClearLVGuidLight();

	//LV Rates Light
	tempsignal1 = (EDSBus1Powered && ((!ExcessiveRateAutoAbortInhibitPY2 && LVAttRefFail2) || ExcessiveRollRateIndication || ExcessivePitchYawRateIndicationA || ExcessivePitchYawRateIndicationB));
	tempsignal2 = (EDSBus3Powered && ((!ExcessiveRateAutoAbortInhibitPY3 && LVAttRefFail2) || ExcessiveRollRateIndication || ExcessivePitchYawRateIndicationA || ExcessivePitchYawRateIndicationB));

	if (tempsignal1 || tempsignal2)
		iu->GetCommandConnector()->SetLVRateLight();
	else
		iu->GetCommandConnector()->ClearLVRateLight();

	//Abort Light
	if ((EDSBus1Powered || EDSBus3Powered) && (PadAbortRequest || RangeSafetyDestructArmedAFromSIVB || RangeSafetyDestructArmedBFromSIVB))
		AbortLightSignal = true;
	else
		AbortLightSignal = false;

	//Liftoff Circuits
	if (EDSBus1Powered && !EDSLiftoffInhibitA && EDSLiftoffEnableA)
	{
		LiftoffA = true;
	}
	else
	{
		LiftoffA = false;
	}

	if (EDSBus3Powered && !EDSLiftoffInhibitB && EDSLiftoffEnableB)
	{
		LiftoffB = true;
	}
	else
	{
		LiftoffB = false;
	}

	//Failure code
	if (PlatformFailure && iu->GetLVCommandConnector()->GetMissionTime() > PlatformFailureTime && !iu->GetLVIMU()->IsFailed())
	{
		iu->GetLVIMU()->SetFailed();
	}

	//Guidance Reference Release
	if (iu->ESEGetGuidanceReferenceRelease())
		iu->GetCommandConnector()->SetAGCInputChannelBit(030, GuidanceReferenceRelease, true);
	else
		iu->GetCommandConnector()->SetAGCInputChannelBit(030, GuidanceReferenceRelease, false);

	//Liftoff
	if (LiftoffRelay)
		iu->GetCommandConnector()->SetAGCInputChannelBit(030, LiftOff, false);
	else
		iu->GetCommandConnector()->SetAGCInputChannelBit(030, LiftOff, true);
}

void EDS::AutoAbortCircuits()
{
	if (AutoAbortBus || iu->ESEGetEDSAutoAbortSimulate(1))
		AutoAbort1AToSC = true;
	else
		AutoAbort1AToSC = false;

	if (AutoAbortBus || iu->ESEGetEDSAutoAbortSimulate(2))
		AutoAbort1BToSC = true;
	else
		AutoAbort1BToSC = false;

	if (AutoAbortBus || iu->ESEGetEDSAutoAbortSimulate(3))
		AutoAbort2AToSC = true;
	else
		AutoAbort2AToSC = false;

	if (AutoAbortBus || iu->ESEGetEDSAutoAbortSimulate(4))
		AutoAbort2BToSC = true;
	else
		AutoAbort2BToSC = false;

	if (AutoAbortBus || iu->ESEGetEDSAutoAbortSimulate(5))
		AutoAbort3AToSC = true;
	else
		AutoAbort3AToSC = false;

	if (AutoAbortBus || iu->ESEGetEDSAutoAbortSimulate(6))
		AutoAbort3BToSC = true;
	else
		AutoAbort3BToSC = false;

	if (AutoAbortBus)
		AutoAbortBusGSEMonitor = true;
	else
		AutoAbortBusGSEMonitor = false;

	if (EDSBus1Powered && ((!AutoAbort2BToSC && !AutoAbort3AToSC) || (!AutoAbort3AToSC && !AutoAbort1BToSC) || (!AutoAbort2BToSC && AutoAbort3AToSC && !AutoAbort1BToSC)))
	{
		EDSAbortSignal1 = true;
	}
	else
	{
		EDSAbortSignal1 = false;
	}
	if (EDSBus2Powered && ((!AutoAbort3BToSC && !AutoAbort2AToSC) || (!AutoAbort2AToSC && !AutoAbort1BToSC) || (!AutoAbort3BToSC && AutoAbort2AToSC && !AutoAbort1BToSC)))
	{
		EDSAbortSignal2 = true;
	}
	else
	{
		EDSAbortSignal2 = false;
	}
	if (EDSBus3Powered && ((!AutoAbort3BToSC && !AutoAbort1AToSC) || (!AutoAbort1AToSC && !AutoAbort2BToSC) || (!AutoAbort3BToSC && AutoAbort1AToSC && !AutoAbort2BToSC)))
	{
		EDSAbortSignal3 = true;
	}
	else
	{
		EDSAbortSignal3 = false;
	}

	//sprintf(oapiDebugString(), "Relays: %d %d %d %d %d %d Signals: %d %d %d", AutoAbort1AToSC, AutoAbort1BToSC, AutoAbort2AToSC, AutoAbort2BToSC, AutoAbort3AToSC, AutoAbort3BToSC, EDSAbortSignal1, EDSAbortSignal2, EDSAbortSignal3);
}

void EDS::SetPlatformFailureParameters(bool PlatFail, double PlatFailTime)
{
	PlatformFailure = PlatFail;
	PlatformFailureTime = PlatFailTime;
}

bool EDS::GetEDSAbort(int n)
{
	if (n == 1)
	{
		return EDSAbortSignal1;
	}
	else if (n == 2)
	{
		return EDSAbortSignal2;
	}
	else if (n == 3)
	{
		return EDSAbortSignal3;
	}

	return false;
}

double EDS::GetLVTankPressure(int n)
{
	//S-IVB LOX No. 1 Tank Pressure
	if (n == 1)
	{
		return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
	}
	//S-IVB LOX No. 2 Tank Pressure
	else if (n == 2)
	{
		return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
	}
	//S-IVB Fuel No. 1 Tank Pressure
	else if (n == 3)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}
	//S-IVB Fuel No. 2 Tank Pressure
	else if (n == 4)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}

	return 0.0;
}

bool EDS::GetSCControl()
{
	return (SCControlEnableRelay && iu->GetCommandConnector()->GetCMCSIVBTakeover());
}

bool EDS::GetAllSIEnginesRunning()
{
	return (!SIAllEnginesOKA && !SIAllEnginesOKB);
}

bool EDS::IsEDSUnsafeA()
{
	return iu->GetCommandConnector()->IsEDSUnsafeA();
}

bool EDS::IsEDSUnsafeB()
{
	return iu->GetCommandConnector()->IsEDSUnsafeB();
}

void EDS::ResetBus1()
{
	LVEnginesCutoffEnable1 = false;
	SIVBRestartAlert = false;
	SIVBEngineOutIndicationB = false;
	IUCommandSystemEnable = false;
}
void EDS::ResetBus2()
{
	LVEnginesCutoffEnable2 = false;
	SIVBEngineOutIndicationA = false;
	SCControlEnableRelay = false;
}

bool EDS::TripleVoting(bool vote1, bool vote2, bool vote3)
{
	//2 out of 3 need to be true
	if (vote1 && vote2) return true;
	if (vote1 && vote3) return true;
	if (vote2 && vote3) return true;

	return false;
}

bool EDS::GetIUCommandSystemEnable()
{
	return IUEDSBusPowered && (IUCommandSystemEnable || iu->GetCommandConnector()->GetIUUPTLMAccept() || LVEnginesCutoffCommand2);
}

void EDS::SaveState(FILEHANDLE scn) {
	papiWriteScenario_bool(scn, "AUTOABORTBUS", AutoAbortBus);
	papiWriteScenario_bool(scn, "EDSLIFTOFFENABLEA", EDSLiftoffEnableA);
	papiWriteScenario_bool(scn, "EDSLIFTOFFENABLEB", EDSLiftoffEnableB);
	papiWriteScenario_bool(scn, "LIFTOFFA", LiftoffA);
	papiWriteScenario_bool(scn, "LIFTOFFB", LiftoffB);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIVBENGINEOUTINDICATIONB", SIVBEngineOutIndicationB);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFFENABLE1", LVEnginesCutoffEnable1);
	papiWriteScenario_bool(scn, "LVENGINESCUTOFFENABLE2", LVEnginesCutoffEnable2);
	papiWriteScenario_bool(scn, "SIEDSCUTOFF", SIEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBEDSCUTOFF", SIVBEDSCutoff);
	papiWriteScenario_bool(scn, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL1", EDSAbortSignal1);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL2", EDSAbortSignal2);
	papiWriteScenario_bool(scn, "EDSABORTSIGNAL3", EDSAbortSignal3);
	papiWriteScenario_bool(scn, "SIVBRESTARTALERT", SIVBRestartAlert);
	papiWriteScenario_bool(scn, "SCCONTROLENABLERELAY", SCControlEnableRelay);
	papiWriteScenario_bool(scn, "IUEDSBUSPOWERED", IUEDSBusPowered);
	papiWriteScenario_bool(scn, "IUCOMMANDSYSTEMENABLE", IUCommandSystemEnable);
	papiWriteScenario_bool(scn, "ABORTLIGHTSIGNAL", AbortLightSignal);
	papiWriteScenario_bool(scn, "LIFTOFFRELAY", LiftoffRelay);
}

void EDS::LoadState(char *line)
{
	papiReadScenario_bool(line, "AUTOABORTBUS", AutoAbortBus);
	papiReadScenario_bool(line, "EDSLIFTOFFENABLEA", EDSLiftoffEnableA);
	papiReadScenario_bool(line, "EDSLIFTOFFENABLEB", EDSLiftoffEnableB);
	papiReadScenario_bool(line, "LIFTOFFA", LiftoffA);
	papiReadScenario_bool(line, "LIFTOFFB", LiftoffB);
	papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONA", SIVBEngineOutIndicationA);
	papiReadScenario_bool(line, "SIVBENGINEOUTINDICATIONB", SIVBEngineOutIndicationB);
	papiReadScenario_bool(line, "LVENGINESCUTOFFENABLE1", LVEnginesCutoffEnable1);
	papiReadScenario_bool(line, "LVENGINESCUTOFFENABLE2", LVEnginesCutoffEnable2);
	papiReadScenario_bool(line, "SIEDSCUTOFF", SIEDSCutoff);
	papiReadScenario_bool(line, "SIVBEDSCUTOFF", SIVBEDSCutoff);
	papiReadScenario_bool(line, "SIVBENGINECUTOFFDISABLED", SIVBEngineCutoffDisabled);
	papiReadScenario_bool(line, "EDSABORTSIGNAL1", EDSAbortSignal1);
	papiReadScenario_bool(line, "EDSABORTSIGNAL2", EDSAbortSignal2);
	papiReadScenario_bool(line, "EDSABORTSIGNAL3", EDSAbortSignal3);
	papiReadScenario_bool(line, "SIVBRESTARTALERT", SIVBRestartAlert);
	papiReadScenario_bool(line, "SCCONTROLENABLERELAY", SCControlEnableRelay);
	papiReadScenario_bool(line, "IUEDSBUSPOWERED", IUEDSBusPowered);
	papiReadScenario_bool(line, "IUCOMMANDSYSTEMENABLE", IUCommandSystemEnable);
	papiReadScenario_bool(line, "ABORTLIGHTSIGNAL", AbortLightSignal);
	papiReadScenario_bool(line, "LIFTOFFRELAY", LiftoffRelay);
}

EDS1B::EDS1B(IU *iu) : EDS(iu)
{
	for (int i = 0;i < 8;i++)
	{
		SIThrustNotOK[i] = false;
		ThrustOKSignal[i] = false;
	}
}

void EDS1B::Timestep(double simdt)
{
	if (iu->GetLVCommandConnector() == NULL) return;
	if (iu->GetCommandConnector() == NULL) return;

	EDS::Timestep(simdt);

	SIAllEnginesOKA = false;
	SIAllEnginesOKB = false;

	//S-IB Thrust Monitor
	if (IUEDSBusPowered && Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(ThrustOKSignal, 24);
		for (int i = 0;i < 8;i++)
		{
			SIThrustNotOK[i] = TripleVoting(!ThrustOKSignal[3 * i], !ThrustOKSignal[3 * i + 1], !ThrustOKSignal[3 * i + 2]);

			if (SIThrustNotOK[i])
			{
				SIAllEnginesOKA = true;
				SIAllEnginesOKB = true;
			}
		}
	}
	else
	{
		for (int i = 0;i < 8;i++)
		{
			SIThrustNotOK[i] = false;
		}
	}

	//EDS Engine Cutoff
	if (LVEnginesCutoffCommand1 || LVEnginesCutoffCommand3)
	{
		if (Stage == LAUNCH_STAGE_ONE)
		{
			if (!SIEDSCutoff)
			{
				SIEDSCutoff = true;
				iu->GetLVCommandConnector()->SIEDSCutoff(true);
			}
		}
		else if (Stage == LAUNCH_STAGE_SIVB || Stage == STAGE_ORBIT_SIVB)
		{
			if (!SIVBEngineCutoffDisabled && !SIVBEDSCutoff)
			{
				SIVBEDSCutoff = true;
				iu->GetLVCommandConnector()->SIVBEDSCutoff(true);
			}
		}
	}
	else
	{
		if (SIEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIEDSCutoff(false);
			SIEDSCutoff = false;
		}
		if (SIVBEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIVBEDSCutoff(false);
			SIVBEDSCutoff = false;
		}
	}

	//Two Engines Out Auto Abort
	if (IUEDSBusPowered && !TripleVoting(TwoEngineOutAutoAbortInhibitNo1, TwoEngineOutAutoAbortInhibitNo2, TwoEngineOutAutoAbortInhibitNo3))
	{
		int enginesout = 0;

		for (int i = 0;i < 8;i++)
		{
			if (SIThrustNotOK[i]) enginesout++;
		}

		if (enginesout >= 2) AutoAbortBus = true;
	}

	//Auto Abort Logic
	AutoAbortCircuits();

	// Update engine indicators and failure flags
	bool LVIndicatorsPower = ((EDSBus1Powered || EDSBus3Powered) && (!GSEEngineThrustIndicationEnableA || !GSEEngineThrustIndicationEnableB));

	//Engine Light 1
	if (LVIndicatorsPower && (SIThrustNotOK[0] || SIVBEngineThrustMonitorA || SIVBEngineThrustMonitorB))
		iu->GetCommandConnector()->SetEngineIndicator(1);
	else
		iu->GetCommandConnector()->ClearEngineIndicator(1);
	//Engine Lights 2-8
	for (int i = 1;i < 8;i++)
	{
		if (LVIndicatorsPower && SIThrustNotOK[i])
			iu->GetCommandConnector()->SetEngineIndicator(i + 1);
		else
			iu->GetCommandConnector()->ClearEngineIndicator(i + 1);
	}
}

void EDS1B::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	EDS::SaveState(scn);

	oapiWriteLine(scn, end_str);
}

void EDS1B::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		EDS::LoadState(line);
	}
}

EDSSV::EDSSV(IU *iu) : EDS(iu)
{
	for (int i = 0;i < 5;i++)
	{
		SIThrustNotOK[i] = false;
		SIIEngineThrustMonitorA[i] = false;
		SIIEngineThrustMonitorB[i] = false;
		ThrustOKSignal[i] = false;
	}
	SIISIVBNotSeparated = false;
	SIISIVBSepSeqStart = false;
	UllageThrustIndicate = false;
	SIIEngineOutIndicationA = false;
	SIIEngineOutIndicationB = false;
	SIIEDSCutoff = false;
}

double EDSSV::GetLVTankPressure(int n)
{
	if (n == 1)
	{
		if (SIISIVBNotSeparated)
		{
			//S-II LH2 Tank Ullage Pressure No. 1
			return iu->GetLVCommandConnector()->GetSIIFuelTankPressurePSI();
		}
		else
		{
			//S-IVB LOX No. 1 Tank Pressure
			return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
		}
	}
	else if (n == 2)
	{
		if (SIISIVBNotSeparated)
		{
			//S-II LH2 Tank Ullage Pressure No. 2
			return iu->GetLVCommandConnector()->GetSIIFuelTankPressurePSI();
		}
		else
		{
			//S-IVB LOX No. 2 Tank Pressure
			return iu->GetLVCommandConnector()->GetSIVBLOXTankPressurePSI();
		}
	}
	//S-IVB Fuel No. 1 Tank Pressure
	else if (n == 3)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}
	//S-IVB Fuel No. 2 Tank Pressure
	else if (n == 4)
	{
		return iu->GetLVCommandConnector()->GetSIVBFuelTankPressurePSI();
	}

	return 0.0;
}

void EDSSV::Timestep(double simdt)
{
	if (iu->GetLVCommandConnector() == NULL) return;
	if (iu->GetCommandConnector() == NULL) return;

	EDS::Timestep(simdt);

	if (Stage < LAUNCH_STAGE_SIVB)
		SIISIVBNotSeparated = true;
	else
		SIISIVBNotSeparated = false;

	SIAllEnginesOKA = false;
	SIAllEnginesOKB = false;

	//S-IC Thrust Monitor
	if (IUEDSBusPowered && Stage <= LAUNCH_STAGE_ONE)
	{
		iu->GetLVCommandConnector()->GetSIThrustOK(ThrustOKSignal, 15);
		for (int i = 0;i < 5;i++)
		{
			SIThrustNotOK[i] = TripleVoting(!ThrustOKSignal[3 * i], !ThrustOKSignal[3 * i + 1], !ThrustOKSignal[3 * i + 2]);

			if (SIThrustNotOK[i])
			{
				SIAllEnginesOKA = true;
				SIAllEnginesOKB = true;
			}
		}
	}
	else
	{
		for (int i = 0;i < 5;i++)
		{
			SIThrustNotOK[i] = false;
		}
	}

	//S-II Thrust Monitor
	if (IUEDSBusPowered && (Stage == LAUNCH_STAGE_TWO || Stage == LAUNCH_STAGE_TWO_ISTG_JET))
	{
		iu->GetLVCommandConnector()->GetSIIThrustOK(ThrustOKSignal);
		for (int i = 0;i < 5;i++)
		{
			SIIEngineThrustMonitorA[i] = SIIEngineOutIndicationA && !ThrustOKSignal[SIIEngInd[i]];
			SIIEngineThrustMonitorB[i] = SIIEngineOutIndicationB && !ThrustOKSignal[SIIEngInd[i]];
		}
	}
	else
	{
		for (int i = 0;i < 5;i++)
		{
			SIIEngineThrustMonitorA[i] = false;
			SIIEngineThrustMonitorB[i] = false;
		}
	}

	//EDS Engine Cutoff
	if (LVEnginesCutoffCommand1 || LVEnginesCutoffCommand3)
	{
		if (Stage == LAUNCH_STAGE_ONE)
		{
			if (!SIEDSCutoff)
			{
				SIEDSCutoff = true;
				iu->GetLVCommandConnector()->SIEDSCutoff(true);
			}
		}
		else if (Stage == LAUNCH_STAGE_TWO || Stage == LAUNCH_STAGE_TWO_ISTG_JET)
		{
			if (!SIIEDSCutoff)
			{
				SIIEDSCutoff = true;
				iu->GetLVCommandConnector()->SIIEDSCutoff(true);
			}
		}
		else if (Stage == LAUNCH_STAGE_SIVB || Stage == STAGE_ORBIT_SIVB)
		{
			if (!SIVBEngineCutoffDisabled && !SIVBEDSCutoff)
			{
				SIVBEDSCutoff = true;
				iu->GetLVCommandConnector()->SIVBEDSCutoff(true);
			}
		}
	}
	else
	{
		if (SIEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIEDSCutoff(false);
			SIEDSCutoff = false;
		}
		if (SIIEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIIEDSCutoff(false);
			SIIEDSCutoff = false;
		}
		if (SIVBEDSCutoff == true)
		{
			iu->GetLVCommandConnector()->SIVBEDSCutoff(false);
			SIVBEDSCutoff = false;
		}
	}

	if (IUEDSBusPowered && !TripleVoting(TwoEngineOutAutoAbortInhibitNo1, TwoEngineOutAutoAbortInhibitNo2, TwoEngineOutAutoAbortInhibitNo3))
	{
		int enginesout = 0;

		for (int i = 0;i < 5;i++)
		{
			if (SIThrustNotOK[i]) enginesout++;
		}

		if (enginesout >= 2) AutoAbortBus = true;
	}

	//Auto Abort Logic
	AutoAbortCircuits();

	//Update engine indicators and failure flags
	bool LVIndicatorsPower = ((EDSBus1Powered || EDSBus3Powered) && (!GSEEngineThrustIndicationEnableA || !GSEEngineThrustIndicationEnableB));

	//Engine Light 1
	if (LVIndicatorsPower && (SIThrustNotOK[0] || SIIEngineThrustMonitorA[0] || SIIEngineThrustMonitorB[0] || SIVBEngineThrustMonitorA || SIVBEngineThrustMonitorB))
		iu->GetCommandConnector()->SetEngineIndicator(1);
	else
		iu->GetCommandConnector()->ClearEngineIndicator(1);

	//Engine Light 2-5
	for (int i = 1;i < 5;i++)
	{
		if (LVIndicatorsPower && (SIThrustNotOK[i] || SIIEngineThrustMonitorA[i] || SIIEngineThrustMonitorB[i]))
			iu->GetCommandConnector()->SetEngineIndicator(i + 1);
		else
			iu->GetCommandConnector()->ClearEngineIndicator(i + 1);
	}

	//Second Plane Separation Monitor
	bool SIISecPlaneSepA = EDSBus1Powered && SIIEngineOutIndicationA && (SIVBRestartAlert || Stage < LAUNCH_STAGE_TWO_ISTG_JET);
	bool SIISecPlaneSepB = EDSBus3Powered && SIIEngineOutIndicationB && (SIVBRestartAlert || Stage < LAUNCH_STAGE_TWO_ISTG_JET);

	if (SIISecPlaneSepA || SIISecPlaneSepB)
		iu->GetCommandConnector()->SetSIISep();
	else
		iu->GetCommandConnector()->ClearSIISep();

	//S-II/S-IVB Separation Sequence Start
	if (IUEDSBusPowered && iu->GetCommandConnector()->GetSIISIVbDirectStagingSignal())
		SIISIVBSepSeqStart = true;
	else
		SIISIVBSepSeqStart = false;

	//CMC Ullage Thrust Indication
	if (UllageThrustIndicate)
		iu->GetCommandConnector()->SetAGCInputChannelBit(030, UllageThrust, true);
	else
		iu->GetCommandConnector()->SetAGCInputChannelBit(030, UllageThrust, false);

	//sprintf(oapiDebugString(), "%f", PlatformFailureTime);
}

void EDSSV::ResetBus1()
{
	EDS::ResetBus1();

	SIIEngineOutIndicationA = false;
}

void EDSSV::ResetBus2()
{
	EDS::ResetBus2();

	SIIEngineOutIndicationB = false;
}

void EDSSV::SaveState(FILEHANDLE scn, char *start_str, char *end_str) {
	oapiWriteLine(scn, start_str);

	EDS::SaveState(scn);

	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
	papiWriteScenario_bool(scn, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
	papiWriteScenario_bool(scn, "SIIEDSCUTOFF", SIIEDSCutoff);
	papiWriteScenario_bool(scn, "ULLAGETHRUSTINDICATE", UllageThrustIndicate);

	oapiWriteLine(scn, end_str);
}

void EDSSV::LoadState(FILEHANDLE scn, char *end_str) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	int end_len = strlen(end_str);

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, end_str, end_len)) {
			break;
		}

		EDS::LoadState(line);

		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONA", SIIEngineOutIndicationA);
		papiReadScenario_bool(line, "SIIENGINEOUTINDICATIONB", SIIEngineOutIndicationB);
		papiReadScenario_bool(line, "SIIEDSCUTOFF", SIIEDSCutoff);
		papiReadScenario_bool(line, "ULLAGETHRUSTINDICATE", UllageThrustIndicate);
	}
}