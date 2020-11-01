/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2019

IU Umbilical

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
#include "iu.h"
#include "IUUmbilicalInterface.h"
#include "IUUmbilical.h"

IUUmbilical::IUUmbilical(IUUmbilicalInterface *ml)
{
	IuUmb = ml;
	iu = NULL;
	IUUmbilicalConnected = false;
}

IUUmbilical::~IUUmbilical()
{
}

void IUUmbilical::Connect(IU *iu)
{
	if (iu)
	{
		this->iu = iu;
		iu->ConnectUmbilical(this);
		IUUmbilicalConnected = true;
	}
}

void IUUmbilical::Disconnect()
{
	if (!IUUmbilicalConnected) return;

	iu->DisconnectUmbilical();
	IUUmbilicalConnected = false;
}

void IUUmbilical::AbortDisconnect()
{
	IUUmbilicalConnected = false;
}

bool IUUmbilical::ESEGetCommandVehicleLiftoffIndicationInhibit()
{
	return IuUmb->ESEGetCommandVehicleLiftoffIndicationInhibit();
}

bool IUUmbilical::ESEGetSICOutboardEnginesCantInhibit()
{
	return IuUmb->ESEGetSICOutboardEnginesCantInhibit();
}

bool IUUmbilical::ESEGetSICOutboardEnginesCantSimulate()
{
	return IuUmb->ESEGetSICOutboardEnginesCantSimulate();
}

bool IUUmbilical::ESEGetExcessiveRollRateAutoAbortInhibit(int n)
{
	return IuUmb->ESEGetExcessiveRollRateAutoAbortInhibit(n);
}

bool IUUmbilical::ESEGetExcessivePitchYawRateAutoAbortInhibit(int n)
{
	return IuUmb->ESEGetExcessivePitchYawRateAutoAbortInhibit(n);
}

bool IUUmbilical::ESEGetTwoEngineOutAutoAbortInhibit(int n)
{
	return IuUmb->ESEGetTwoEngineOutAutoAbortInhibit(n);
}

bool IUUmbilical::ESEGetGSEOverrateSimulate(int n)
{
	return IuUmb->ESEGetGSEOverrateSimulate(n);
}

bool IUUmbilical::ESEGetEDSPowerInhibit()
{
	return IuUmb->ESEGetEDSPowerInhibit();
}

bool IUUmbilical::ESEPadAbortRequest()
{
	return IuUmb->ESEPadAbortRequest();
}

bool IUUmbilical::ESEGetThrustOKIndicateEnableInhibitA()
{
	return IuUmb->ESEGetThrustOKIndicateEnableInhibitA();
}

bool IUUmbilical::ESEGetThrustOKIndicateEnableInhibitB()
{
	return IuUmb->ESEGetThrustOKIndicateEnableInhibitB();
}

bool IUUmbilical::ESEEDSLiftoffInhibitA()
{
	return IuUmb->ESEEDSLiftoffInhibitA();
}

bool IUUmbilical::ESEEDSLiftoffInhibitB()
{
	return IuUmb->ESEEDSLiftoffInhibitB();
}

bool IUUmbilical::ESEGetSIBurnModeSubstitute()
{
	return IuUmb->ESEGetSIBurnModeSubstitute();
}

bool IUUmbilical::ESEGetGuidanceReferenceRelease()
{
	return IuUmb->ESEGetGuidanceReferenceRelease();
}

bool IUUmbilical::ESEGetQBallSimulateCmd()
{
	return IuUmb->ESEGetQBallSimulateCmd();
}

bool IUUmbilical::ESEGetEDSAutoAbortSimulate(int n)
{
	return IuUmb->ESEGetEDSAutoAbortSimulate(n);
}

bool IUUmbilical::ESEGetEDSLVCutoffSimulate(int n)
{
	return IuUmb->ESEGetEDSLVCutoffSimulate(n);
}

void IUUmbilical::SetEDSLiftoffEnableA()
{
	if (!IUUmbilicalConnected) return;

	iu->GetEDS()->SetEDSLiftoffEnableA();
}

void IUUmbilical::SetEDSLiftoffEnableB()
{
	if (!IUUmbilicalConnected) return;

	iu->GetEDS()->SetEDSLiftoffEnableB();
}

void IUUmbilical::EDSLiftoffEnableReset()
{
	if (!IUUmbilicalConnected) return;

	iu->GetEDS()->LiftoffEnableReset();
}

void IUUmbilical::SwitchFCCPowerOn()
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->SetFCCPower(true);
}

void IUUmbilical::SwitchFCCPowerOff()
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->SetFCCPower(false);
}

void IUUmbilical::SwitchQBallPowerOn()
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->SetQBallPower(true);
}

void IUUmbilical::SwitchQBallPowerOff()
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->SetQBallPower(false);
}

bool IUUmbilical::AllSIEnginesRunning()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->GetAllSIEnginesRunning();
}

bool IUUmbilical::IsEDSUnsafeA()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->IsEDSUnsafeA();
}

bool IUUmbilical::IsEDSUnsafeB()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->IsEDSUnsafeB();
}

bool IUUmbilical::GetEDSSCCutoff1()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->GetLVEnginesCutoffFromSC1();
}

bool IUUmbilical::GetEDSSCCutoff2()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->GetLVEnginesCutoffFromSC2();
}

bool IUUmbilical::GetEDSSCCutoff3()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->GetLVEnginesCutoffFromSC3();
}

bool IUUmbilical::GetEDSAutoAbortBus()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->GetAutoAbort();
}

bool IUUmbilical::GetEDSExcessiveRollRateIndication()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->GetExcessiveRollRateIndication();
}

bool IUUmbilical::GetEDSExcessivePitchYawRateIndication()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetEDS()->GetExcessivePitchYawRateIndication();
}

bool IUUmbilical::GetLVDCOutputRegisterDiscrete(int bit)
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetLVDA()->GetOutputRegisterBit(bit);
}

bool IUUmbilical::FCCPowerIsOn()
{
	if (!IUUmbilicalConnected) return false;

	return iu->GetControlDistributor()->GetFCCPowerOn();
}

void IUUmbilical::SetControlSignalProcessorPower(bool set)
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->SetControlSignalProcessorPowerOn(set);
}

void IUUmbilical::EDSGroupNo1Reset()
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->ResetBus1();
}

void IUUmbilical::EDSGroupNo2Reset()
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->ResetBus2();
}

void IUUmbilical::SwitchSelector(int stage, int channel)
{
	if (!IUUmbilicalConnected) return;

	iu->GetControlDistributor()->SwitchSelector(stage, channel);
}

void IUUmbilical::LVDCPrepareToLaunch()
{
	if (!IUUmbilicalConnected) return;

	iu->GetLVDA()->PrepareToLaunch();
}