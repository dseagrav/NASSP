/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  Saturn Instrument Unit header

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
#include "stdio.h"
#include "math.h"

#include "soundlib.h"
#include "nasspsound.h"
#include "nasspdefs.h"

#include "saturn.h"
#include "papi.h"
#include "IUUmbilical.h"

#include "iu.h"


IU::IU() :
dcs(this),
AuxiliaryPowerDistributor1(this),
AuxiliaryPowerDistributor2(this),
lvimu(this),
ControlSignalProcessor(this)
{
	State = 0;
	MissionTime = 0.0;
	LastCycled = 0.0;

	Crewed = true;
	SCControlPoweredFlight = false;

	commandConnector.SetIU(this);

	IuUmb = NULL;
}

IU::~IU()
{
	if (IuUmb)
	{
		IuUmb->AbortDisconnect();
	}
}

void IU::SetMissionInfo(bool crewed, bool sccontpowered)
{
	Crewed = crewed;
	SCControlPoweredFlight = sccontpowered;
}

void IU::Timestep(double misst, double simt, double simdt, double mjd)
{
	//
	// Update mission time.
	//
	MissionTime = misst;

	AuxiliaryPowerDistributor1.Timestep(simdt);
	AuxiliaryPowerDistributor2.Timestep(simdt);

	//Set the launch stage here
	if (!IsUmbilicalConnected() && lvCommandConnector.GetStage() == PRELAUNCH_STAGE)
	{
		lvCommandConnector.SetStage(LAUNCH_STAGE_ONE);
	}

	lvimu.Timestep(mjd);
	lvrg.Timestep(simdt);

	if (GetControlDistributor()->GetGSECommandVehicleLiftoffIndicationInhibit() == false)
	{
		GetEngineCutoffEnableTimer()->SetRunning(true);
	}
}

void IU::LVDCTimestep(double simt, double simdt)
{
	//GetLVDC()->TimeStep(simdt);

	if (LastCycled == 0) {					// Use simdt as difference if new run
		LastCycled = (simt - simdt);
	}
	double ThisTime = LastCycled;			// Save here

	long cycles = (long)((simt - LastCycled) / LVDC_TIMESTEP);	// Get number of CPU cycles to do
	LastCycled += (LVDC_TIMESTEP * cycles);						// Preserve the remainder
	long x = 0;
	while (x < cycles) {
		GetLVDC()->TimeStep(LVDC_TIMESTEP);
		ThisTime += LVDC_TIMESTEP;								// Add time
		x++;
	}
}

void IU::PostStep(double simt, double simdt, double mjd) {

}

void IU::SaveState(FILEHANDLE scn)

{
	oapiWriteLine(scn, IU_START_STRING);

	oapiWriteScenario_int(scn, "STATE", State);

	GetFCC()->SaveState(scn, "FCC_BEGIN", "FCC_END");
	GetEDS()->SaveState(scn, "EDS_BEGIN", "EDS_END");
	GetControlDistributor()->SaveState(scn, "CONTROLDISTRIBUTOR_BEGIN", "CONTROLDISTRIBUTOR_END");
	GetEngineCutoffEnableTimer()->SaveState(scn, "ENGINECUTOFFENABLETIMER_BEGIN", "ENGINECUTOFFENABLETIMER_END");
	lvda.SaveState(scn);
	
	oapiWriteLine(scn, IU_END_STRING);
}

void IU::LoadState(FILEHANDLE scn)

{
	char *line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, IU_END_STRING, sizeof(IU_END_STRING)))
			return;

		papiReadScenario_int(line, "STATE", State);
		if (!strnicmp(line, "FCC_BEGIN", sizeof("FCC_BEGIN"))) {
			GetFCC()->LoadState(scn, "FCC_END");
		}
		else if (!strnicmp(line, "EDS_BEGIN", sizeof("EDS_BEGIN"))) {
			GetEDS()->LoadState(scn, "EDS_END");
		}
		else if (!strnicmp(line, "CONTROLDISTRIBUTOR_BEGIN", sizeof("CONTROLDISTRIBUTOR_BEGIN"))) {
			GetControlDistributor()->LoadState(scn, "CONTROLDISTRIBUTOR_END");
		}
		else if (!strnicmp(line, "ENGINECUTOFFENABLETIMER_BEGIN", sizeof("ENGINECUTOFFENABLETIMER_BEGIN"))) {
			GetEngineCutoffEnableTimer()->LoadState(scn, "ENGINECUTOFFENABLETIMER_END");
		}
		else if (!strnicmp(line, LVDA_START_STRING, sizeof(LVDA_START_STRING))) {
			lvda.LoadState(scn);
		}
	}
}

void IU::ConnectToCSM(Connector *csmConnector)

{
	commandConnector.ConnectTo(csmConnector);
}

void IU::ConnectToLV(Connector *CommandConnector)

{
	lvCommandConnector.ConnectTo(CommandConnector);
}

bool IU::GetSIPropellantDepletionEngineCutoff()
{
	return lvCommandConnector.GetSIPropellantDepletionEngineCutoff();
}

bool IU::GetSIInboardEngineOut()
{
	return lvCommandConnector.GetSIInboardEngineOut();
}

bool IU::GetSIOutboardEngineOut()
{
	return lvCommandConnector.GetSIOutboardEngineOut();
}

bool IU::SIBLowLevelSensorsDry()
{
	return false;
}

bool IU::GetSIIPropellantDepletionEngineCutoff()
{
	return false;
}

bool IU::GetSIIEngineOut()
{
	return false;
}

bool IU::GetSIVBEngineOut()
{
	int stage = lvCommandConnector.GetStage();
	if (stage != LAUNCH_STAGE_SIVB && stage != STAGE_ORBIT_SIVB) return false;

	if (lvCommandConnector.GetSIVBThrustOK() == false)
	{
		return true;
	}

	return false;
}

VECTOR3 IU::GetTheodoliteAlignment(double azimuth)
{
	MATRIX3 R, R_left, Rot;
	VECTOR3 pos, X_SM, Y_SM, Z_SM, X_NB, U_Z, E, S, Z_NB, Y_NB;

	lvCommandConnector.GetRotationMatrix(R_left);
	R = _M(R_left.m11, R_left.m13, R_left.m12, R_left.m31, R_left.m33, R_left.m32, R_left.m21, R_left.m23, R_left.m22);
	X_NB = _V(R.m12, R.m22, R.m32);
	Z_NB = _V(R.m13, R.m23, R.m33);
	Y_NB = -_V(R.m11, R.m21, R.m31);
	lvCommandConnector.GetRelativePos(oapiGetObjectByName("Earth"), pos);
	X_SM = unit(_V(pos.x, pos.z, pos.y));
	//Get polar axis
	oapiGetRotationMatrix(oapiGetObjectByName("Earth"), &Rot);
	U_Z =_V(Rot.m12, Rot.m32, Rot.m22);

	E = unit(crossp(-X_SM, U_Z));
	S = unit(crossp(E, -X_SM));
	Z_SM = E * sin(azimuth) + S * cos(azimuth);
	Y_SM = crossp(Z_SM, X_SM);

	VECTOR3 a_MG = unit(crossp(X_NB, Y_SM));
	double cos_OGA = dotp(a_MG, Z_NB);
	double sin_OGA = dotp(a_MG, Y_NB);
	double OGA = atan2(sin_OGA, cos_OGA);
	if (OGA < 0)
	{
		OGA += PI2;
	}
	double cos_MGA = dotp(Y_SM, crossp(a_MG, X_NB));
	double sin_MGA = dotp(Y_SM, X_NB);
	double MGA = atan2(sin_MGA, cos_MGA);
	if (MGA < 0)
	{
		MGA += PI2;
	}
	double cos_IGA = dotp(a_MG, Z_SM);
	double sin_IGA = dotp(a_MG, X_SM);
	double IGA = atan2(sin_IGA, cos_IGA);
	if (IGA < 0)
	{
		IGA += PI2;
	}
	return _V(OGA, IGA, MGA);
}

bool IU::DCSUplink(int type, void *upl)
{
	return dcs.Uplink(type, upl);
}

bool IU::IsUmbilicalConnected()
{
	if (IuUmb && IuUmb->IsIUUmbilicalConnected()) return true;

	return false;
}

void IU::ConnectUmbilical(IUUmbilical *umb)
{
	IuUmb = umb;
}

void IU::DisconnectUmbilical()
{
	IuUmb = NULL;
}

void IU::DisconnectIU()
{
	lvCommandConnector.Disconnect();
	commandConnector.Disconnect();
}

bool IU::ESEGetCommandVehicleLiftoffIndicationInhibit()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetCommandVehicleLiftoffIndicationInhibit();
}

bool IU::ESEGetExcessiveRollRateAutoAbortInhibit(int n)
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetExcessiveRollRateAutoAbortInhibit(n);
}

bool IU::ESEGetExcessivePitchYawRateAutoAbortInhibit(int n)
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetExcessivePitchYawRateAutoAbortInhibit(n);
}

bool IU::ESEGetTwoEngineOutAutoAbortInhibit(int n)
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetTwoEngineOutAutoAbortInhibit(n);
}

bool IU::ESEGetGSEOverrateSimulate(int n)
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetGSEOverrateSimulate(n);
}

bool IU::ESEGetEDSPowerInhibit()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetEDSPowerInhibit();
}

bool IU::ESEPadAbortRequest()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEPadAbortRequest();
}

bool IU::ESEGetEngineThrustIndicationEnableInhibitA()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetThrustOKIndicateEnableInhibitA();
}

bool IU::ESEGetEngineThrustIndicationEnableInhibitB()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetThrustOKIndicateEnableInhibitB();
}

bool IU::ESEEDSLiftoffInhibitA()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEEDSLiftoffInhibitA();
}

bool IU::ESEEDSLiftoffInhibitB()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEEDSLiftoffInhibitB();
}

bool IU::ESEGetSIBurnModeSubstitute()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetSIBurnModeSubstitute();
}

bool IU::ESEGetGuidanceReferenceRelease()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetGuidanceReferenceRelease();
}

bool IU::ESEESEGetQBallSimulateCmd()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetQBallSimulateCmd();
}

bool IU::ESEGetEDSAutoAbortSimulate(int n)
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetEDSAutoAbortSimulate(n);
}

bool IU::ESEGetEDSLVCutoffSimulate(int n)
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetEDSLVCutoffSimulate(n);
}

IUToCSMCommandConnector::IUToCSMCommandConnector()

{
	type = CSM_IU_COMMAND;
	ourIU = 0;
}

IUToCSMCommandConnector::~IUToCSMCommandConnector()

{
}

void IUToCSMCommandConnector::SetAGCInputChannelBit(int channel, int bit, bool val)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_INPUT_CHANNEL_BIT;
	cm.val1.iValue = channel;
	cm.val2.iValue = bit;
	cm.val3.bValue = val;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetSIISep()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_SII_SEP_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearSIISep()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_SII_SEP_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetLVRateLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_RATE_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearLVRateLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_RATE_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetLVGuidLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_GUID_LIGHT;
	cm.val1.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearLVGuidLight()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_LV_GUID_LIGHT;
	cm.val1.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::SetEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATOR;

	cm.val1.iValue = eng;
	cm.val2.bValue = true;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATOR;

	cm.val1.iValue = eng;
	cm.val2.bValue = false;

	SendMessage(cm);
}

void IUToCSMCommandConnector::ClearEngineIndicators()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_SET_ENGINE_INDICATORS;

	cm.val1.bValue = false;

	SendMessage(cm);
}

bool IUToCSMCommandConnector::GetCMCSIVBTakeover()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_CMC_SIVB_TAKEOVER;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::GetCMCSIVBIgnitionSequenceStart()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_CMC_SIVB_IGNITION;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::GetCMCSIVBCutoff()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_CMC_SIVB_CUTOFF;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::GetEngineIndicator(int eng)

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_ENGINE_INDICATOR;
	cm.val1.iValue = eng;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::GetTLIInhibitSignal()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_TLI_INHIBIT;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::GetIUUPTLMAccept()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_IU_UPTLM_ACCEPT;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::IsEDSUnsafeA()
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_IS_EDS_UNSAFE_A;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::IsEDSUnsafeB()
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_IS_EDS_UNSAFE_B;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::GetSIISIVbDirectStagingSignal()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_SIISIVB_DIRECT_STAGING;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

int IUToCSMCommandConnector::LVRateAutoSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_LV_RATE_AUTO_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

int IUToCSMCommandConnector::TwoEngineOutAutoSwitchState()

{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_TWO_ENGINE_OUT_AUTO_SWITCH_STATE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return -1;
}

bool IUToCSMCommandConnector::GetBECOCommand(bool IsSysA)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_BECO_COMMAND;
	cm.val1.bValue = IsSysA;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}

bool IUToCSMCommandConnector::IsEDSBusPowered(int eds)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_IS_EDS_BUS_POWERED;
	cm.val1.iValue = eds;

	if (SendMessage(cm))
	{
		return cm.val2.bValue;
	}

	return false;
}

int IUToCSMCommandConnector::GetAGCAttitudeError(int axis)
{
	ConnectorMessage cm;

	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_GET_AGC_ATTITUDE_ERROR;
	cm.val1.iValue = axis;

	if (SendMessage(cm))
	{
		return cm.val2.iValue;
	}

	return 0;
}

void IUToCSMCommandConnector::TLIBegun()
{
	ConnectorMessage cm;
	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_TLI_BEGUN;

	SendMessage(cm);
}

void IUToCSMCommandConnector::TLIEnded()
{
	ConnectorMessage cm;
	cm.destination = CSM_IU_COMMAND;
	cm.messageType = IUCSM_TLI_ENDED;

	SendMessage(cm);
}

//
// Process incoming messages from the CSM.
//

bool IUToCSMCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	IUCSMMessageType messageType;

	messageType = (IUCSMMessageType) m.messageType;

	switch (messageType)
	{
	case CSMIU_SET_VESSEL_STATS:
		if (ourIU)
		{
			//ourIU->SetVesselStats(m.val1.dValue, m.val2.dValue);
		}
		return true;

	case CSMIU_START_TLI_BURN:
		if (ourIU)
		{
			//ourIU->StartTLIBurn(m.val1.vValue, m.val2.vValue, m.val3.vValue, m.val4.dValue);
		}
		return true;

	case CSMIU_GET_LIFTOFF_CIRCUIT:
		if (ourIU)
		{
			if (m.val1.bValue)
			{
				m.val2.bValue = ourIU->GetEDS()->GetLiftoffCircuitA();
			}
			else
			{
				m.val2.bValue = ourIU->GetEDS()->GetLiftoffCircuitB();
			}
			return true;
		}
		break;

	case CSMIU_GET_EDS_ABORT:
		if (ourIU)
		{
			m.val2.bValue = ourIU->GetEDS()->GetEDSAbort(m.val1.iValue);
			return true;
		}
		break;

	case CSMIU_GET_LV_TANK_PRESSURE:
		if (ourIU)
		{
			m.val2.dValue = ourIU->GetEDS()->GetLVTankPressure(m.val1.iValue);
			return true;
		}
		break;
	case CSMIU_GET_ABORT_LIGHT_SIGNAL:
		if (ourIU)
		{
			m.val1.bValue = ourIU->GetEDS()->GetAbortLightSignal();
			return true;
		}
		break;
	case CSMIU_GET_QBALL_POWER:
		if (ourIU)
		{
			m.val1.bValue = ourIU->GetControlDistributor()->GetQBallPower();
			return true;
		}
		break;
	case CSMIU_GET_QBALL_SIMULATE_CMD:
		if (ourIU)
		{
			m.val1.bValue = ourIU->ESEESEGetQBallSimulateCmd();
			return true;
		}
		break;
	}

	return false;
}

IUToLVCommandConnector::IUToLVCommandConnector()

{
	type = LV_IU_COMMAND;
}

IUToLVCommandConnector::~IUToLVCommandConnector()

{
}

void IUToLVCommandConnector::SetAPSAttitudeEngine(int n, bool on)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_APS_ATTITUDE_ENGINE;
	cm.val1.iValue = n;
	cm.val2.bValue = on;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIEDSCutoff(bool cut)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SI_EDS_CUTOFF;
	cm.val1.bValue = cut;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIIEDSCutoff(bool cut)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SII_EDS_CUTOFF;
	cm.val1.bValue = cut;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIVBEDSCutoff(bool cut)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SIVB_EDS_CUTOFF;
	cm.val1.bValue = cut;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetSIThrusterDir(int n, double yaw, double pitch)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SI_THRUSTER_DIR;
	cm.val1.iValue = n;
	cm.val2.dValue = yaw;
	cm.val3.dValue = pitch;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetSIIThrusterDir(int n, double yaw, double pitch)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SII_THRUSTER_DIR;
	cm.val1.iValue = n;
	cm.val2.dValue = yaw;
	cm.val3.dValue = pitch;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetSIVBThrusterDir(double yaw, double pitch)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_SIVB_THRUSTER_DIR;
	cm.val1.dValue = yaw;
	cm.val2.dValue = pitch;

	SendMessage(cm);
}

void IUToLVCommandConnector::SISwitchSelector(int channel)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SI_SWITCH_SELECTOR;
	cm.val1.iValue = channel;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIISwitchSelector(int channel)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SII_SWITCH_SELECTOR;
	cm.val1.iValue = channel;

	SendMessage(cm);
}

void IUToLVCommandConnector::SIVBSwitchSelector(int channel)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SIVB_SWITCH_SELECTOR;
	cm.val1.iValue = channel;

	SendMessage(cm);
}

void IUToLVCommandConnector::SeparateStage(int stage)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SEPARATE_STAGE;
	cm.val1.iValue = stage;

	SendMessage(cm);
}

void IUToLVCommandConnector::SetStage(int stage)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_SET_STAGE;
	cm.val1.iValue = stage;

	SendMessage(cm);
}

void IUToLVCommandConnector::JettisonNosecap()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_NOSECAP_JETTISON;

	SendMessage(cm);
}

void IUToLVCommandConnector::DeploySLAPanel()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_DEPLOY_SLA_PANEL;

	SendMessage(cm);
}

double IUToLVCommandConnector::GetMass()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MASS;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

int IUToLVCommandConnector::GetStage()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_STAGE;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return NULL_STAGE;
}

void IUToLVCommandConnector::GetGlobalOrientation(VECTOR3 &arot)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GLOBAL_ORIENTATION;
	cm.val1.pValue = &arot;

	SendMessage(cm);
}

OBJHANDLE IUToLVCommandConnector::GetGravityRef()

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GRAVITY_REF;

	if (SendMessage(cm))
	{
		return cm.val1.hValue;
	}

	return 0;
}

void IUToLVCommandConnector::GetRelativePos(OBJHANDLE ref, VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_RELATIVE_POS;
	cm.val1.hValue = ref;
	cm.val2.pValue = &v;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetRelativeVel(OBJHANDLE ref, VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_RELATIVE_VEL;
	cm.val1.hValue = ref;
	cm.val2.pValue = &v;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetGlobalVel(VECTOR3 &v)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_GLOBAL_VEL;
	cm.val1.pValue = &v;

	SendMessage(cm);
}

bool IUToLVCommandConnector::GetWeightVector(VECTOR3 &w)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_WEIGHTVECTOR;
	cm.val1.pValue = &w;

	if (SendMessage(cm))
	{		
		return cm.val2.bValue; 
	}

	return false;
}

void IUToLVCommandConnector::GetRotationMatrix(MATRIX3 &rot)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ROTATIONMATRIX;
	cm.val1.pValue = &rot;

	SendMessage(cm);
}

void IUToLVCommandConnector::GetAngularVel(VECTOR3 &avel)

{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_ANGULARVEL;
	cm.val1.pValue = &avel;

	SendMessage(cm);
}

double IUToLVCommandConnector::GetMissionTime()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_MISSIONTIME;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

int IUToLVCommandConnector::GetVehicleNo()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_VEHICLENO;

	if (SendMessage(cm))
	{
		return cm.val1.iValue;
	}

	return 0;
}

void IUToLVCommandConnector::GetSIThrustOK(bool *ok, int n)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SI_THRUST_OK;
	cm.val1.pValue = ok;
	cm.val2.iValue = n;

	(SendMessage(cm));
}

bool IUToLVCommandConnector::GetSIPropellantDepletionEngineCutoff()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SI_PROPELLANT_DEPLETION_ENGINE_CUTOFF;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToLVCommandConnector::GetSIInboardEngineOut()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SI_INBOARD_ENGINE_OUT;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToLVCommandConnector::GetSIOutboardEngineOut()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SI_OUTBOARD_ENGINE_OUT;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToLVCommandConnector::GetSIBLowLevelSensorsDry()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIB_LOW_LEVEL_SENSORS_DRY;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

void IUToLVCommandConnector::GetSIIThrustOK(bool *ok)
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SII_THRUST_OK;
	cm.val1.pValue = ok;

	if (SendMessage(cm))
	{
		return;
	}

	for (int i = 0;i < 5;i++)
	{
		ok[i] = false;
	}
}

bool IUToLVCommandConnector::GetSIIEngineOut()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SII_ENGINE_OUT;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToLVCommandConnector::GetSIIPropellantDepletionEngineCutoff()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SII_PROPELLANT_DEPLETION_ENGINE_CUTOFF;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

bool IUToLVCommandConnector::GetSIVBThrustOK()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIVB_THRUST_OK;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

double IUToLVCommandConnector::GetSIIFuelTankPressurePSI()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SII_FUEL_TANK_PRESSURE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSIVBLOXTankPressurePSI()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIVB_LOX_TANK_PRESSURE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

double IUToLVCommandConnector::GetSIVBFuelTankPressurePSI()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_GET_SIVB_FUEL_TANK_PRESSURE;

	if (SendMessage(cm))
	{
		return cm.val1.dValue;
	}

	return 0.0;
}

bool IUToLVCommandConnector::CSMSeparationSensed()
{
	ConnectorMessage cm;

	cm.destination = LV_IU_COMMAND;
	cm.messageType = IULV_CSM_SEPARATION_SENSED;

	if (SendMessage(cm))
	{
		return cm.val1.bValue;
	}

	return false;
}

void IU::SaveLVDC(FILEHANDLE scn) {
	GetLVDC()->SaveState(scn);
	lvimu.SaveState(scn);
}

IU1B::IU1B() : fcc(this), eds(this), ControlDistributor(this), EngineCutoffEnableTimer(40.0), lvdc(lvda)
{
	lvda.Init(this);

	lvrg.Init(&lvCommandConnector);			// LV Rate Gyro Package
	lvimu.CoarseAlignEnableFlag = false;	// Clobber this
	lvdc.Init();
}

IU1B::~IU1B()
{

}

void IU1B::Timestep(double misst, double simt, double simdt, double mjd)
{
	IU::Timestep(misst, simt, simdt, mjd);

	EngineCutoffEnableTimer.Timestep(simdt);
	ControlDistributor.Timestep(simdt);
	ControlSignalProcessor.Timestep();
	eds.Timestep(simdt);
	LVDCTimestep(simt, simdt);
	fcc.Timestep(simdt);
}

bool IU1B::SIBLowLevelSensorsDry()
{
	return lvCommandConnector.GetSIBLowLevelSensorsDry();
}

void IU1B::LoadLVDC(FILEHANDLE scn) {

	char *line;

	lvdc.LoadState(scn);

	if (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, LVIMU_START_STRING, sizeof(LVIMU_START_STRING))) {
			lvimu.LoadState(scn);
		}
	}
}

void IU1B::SwitchSelector(int item)
{
	switch (item)
	{
	case 1: //Q-Ball Power Off
		ControlDistributor.SetQBallPower(false);
		break;
	case 2: //Excess Rate (P,Y,R) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "A"
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibit(true);
		break;
	case 3: //S-IVB Engine EDS Cutoffs Disable
		eds.SetSIVBEngineOutIndicationA(false);
		eds.SetSIVBEngineOutIndicationB(false);
		eds.SetSIVBEngineCutoffDisabled();
		break;
	case 4: //Flight Control Computer Switch Point No. 4
		ControlDistributor.SetFCCSwitchPoint4On();
		break;
	case 5: //Flight Control Computer S-IVB Burn Mode Off "B"
		ControlDistributor.SetSIVBBurnModeB(false);
		break;
	case 6: //Flight Control Computer S-IVB Burn Mode On "B"
		ControlDistributor.SetSIVBBurnModeB(true);
		break;
	case 9: //S-IVB Engine Out Indication "A" Enable
		eds.SetSIVBEngineOutIndicationA(true);
		break;
	case 12: //Flight Control Computer S-IVB Burn Mode Off "A"
		ControlDistributor.SetSIVBBurnModeA(false);
		break;
	case 13: //Excess Rate (P,Y,R) Auto-Abort Inhibit Off
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibitEnable(false);
		break;
	case 15: //Excess Rate (P,Y,R) Auto-Abort Inhibit Enable
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibitEnable(true);
		break;
	case 16: //Auto-Abort Enable Relays Reset
		eds.LiftoffEnableReset();
		break;
	case 18: //S/C Control of Saturn Enable
		eds.EnableSCControl();
		break;
	case 20: //Excess Rate (Roll) Auto Abort Off
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibit(false);
		break;
	case 21: //Flight Control Computer Switch Point No. 2
		ControlDistributor.SetFCCSwitchPoint2On();
		break;
	case 22: //Flight Control Computer Switch Point No. 3
		ControlDistributor.SetFCCSwitchPoint3On();
		break;
	case 23: //Telemetry Calibrator Inflight Calibrate On
		break;
	case 24: //Telemetry Calibrator Inflight Calibrate Off
		break;
	case 29: //S-IVB Engine Out Indication "B" Enable
		eds.SetSIVBEngineOutIndicationB(true);
		break;
	case 34: //Excess Rate (Roll) Auto-Abort Inhibit Enable
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibitEnable(true);
		break;
	case 35: //S-IB Two Engines Out Auto-Abort Inhibit
		ControlDistributor.SetTwoEngOutAutoAbortInhibit();
		break;
	case 38: //Launch Vehicle Engines EDS Cutoff Enable
		eds.SetLVEnginesCutoffEnable1();
		break;
	case 41: //Excess Rate(P, Y, R) Auto Abort Inhibit Off
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibit(false);
		break;
	case 42: //Excess Rate (Roll) Auto-Abort Inhibit Off
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibitEnable(false);
		break;
	case 43: //Flight Control Computer Switch Point No. 1
		ControlDistributor.SetFCCSwitchPoint1On();
		break;
	case 44: //Flight Control Computer Switch Point No. 5
		ControlDistributor.SetFCCSwitchPoint5On();
		break;
	case 50: //Excess Rate (Roll) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "B"
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibit(true);
		break;
	case 51: //S-IB Two Engines Out Auto-Abort Inhibit Enable
		ControlDistributor.SetTwoEngOutAutoAbortInhibitEnable();
		break;
	case 53: //Flight Control Computer S-IVB Burn Mode On "A"
		ControlDistributor.SetSIVBBurnModeA(true);
		break;
	case 110: //Nose Cone Jettison (Apollo 5, not a real switch selector event!)
		lvCommandConnector.JettisonNosecap();
		break;
	case 111: //SLA Panel Deployment (Apollo 5, not a real switch selector event!)
		lvCommandConnector.DeploySLAPanel();
		break;
	default:
		break;
	}
}

IUSV::IUSV() : fcc(this), eds(this), ControlDistributor(this), EngineCutoffEnableTimer(30.0), lvdc(lvda)
{
	lvda.Init(this);

	lvrg.Init(&lvCommandConnector);			// LV Rate Gyro Package
	lvimu.CoarseAlignEnableFlag = false;	// Clobber this
	lvdc.Init();
}

IUSV::~IUSV()
{

}

void IUSV::Timestep(double misst, double simt, double simdt, double mjd)
{
	IU::Timestep(misst, simt, simdt, mjd);

	EngineCutoffEnableTimer.Timestep(simdt);
	ControlDistributor.Timestep(simdt);
	ControlSignalProcessor.Timestep();
	eds.Timestep(simdt);
	LVDCTimestep(simt, simdt);
	fcc.Timestep(simdt);
}

void IUSV::LoadLVDC(FILEHANDLE scn) {

	char *line;

	lvdc.LoadState(scn);

	if (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, LVIMU_START_STRING, sizeof(LVIMU_START_STRING))) {
			lvimu.LoadState(scn);
		}
	}
}

bool IUSV::GetSIIPropellantDepletionEngineCutoff()
{
	return lvCommandConnector.GetSIIPropellantDepletionEngineCutoff();
}

bool IUSV::GetSIIEngineOut()
{
	return lvCommandConnector.GetSIIEngineOut();
}

bool IUSV::ESEGetSICOutboardEnginesCantInhibit()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetSICOutboardEnginesCantInhibit();
}

bool IUSV::ESEGetSICOutboardEnginesCantSimulate()
{
	if (!IsUmbilicalConnected()) return false;

	return IuUmb->ESEGetSICOutboardEnginesCantSimulate();
}

void IUSV::SwitchSelector(int item)
{
	switch (item)
	{
	case 1: //Q-Ball Power Off
		ControlDistributor.SetQBallPower(false);
		break;
	case 2: //Excess Rate (P,Y,R) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "A"
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibit(true);
		break;
	case 3: //Tape Recorder Playback Reverse Off
		break;
	case 4: //Flight Control Computer Switch Point No. 4
		ControlDistributor.SetFCCSwitchPoint4On();
		break;
	case 5: //Flight Control Computer Switch Point No. 6
		ControlDistributor.SetFCCSwitchPoint6On();
		break;
	case 6: //Spare
		break;
	case 7: //Flight Control Computer Switch Point No. 7
		ControlDistributor.SetFCCSwitchPoint7On();
		break;
	case 8: //Spare
		break;
	case 9: //S-IVB Engine Out Indication "A" Enable
		eds.SetSIVBEngineOutIndicationA(true);
		break;
	case 10: //Spare
		break;
	case 11: //S-IVB Engine Out Indication "B" Enable
		eds.SetSIVBEngineOutIndicationB(true);
		break;
	case 12: //Flight Control Computer S-IVB Burn Mode Off "A"
		ControlDistributor.SetSIVBBurnModeA(false);
		break;
	case 13: //Excess Rate (P,Y,R) Auto-Abort Inhibit Off
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibitEnable(false);
		break;
	case 14: //IU Tape Recorder Playback Off
		break;
	case 15: //Excess Rate (P,Y,R) Auto-Abort Inhibit Enable
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibitEnable(true);
		break;
	case 16: //Auto-Abort Enable Relays Reset
		eds.LiftoffEnableReset();
		break;
	case 17: //Tape Recorder Record Off
		break;
	case 18: //S-IVB Engine Out Indication "A" Enable Reset
		eds.SetSIVBEngineOutIndicationA(false);
		break;
	case 19: //Tape Recorder Playback Reverse On
		break;
	case 20: //Excess Rate (Roll) Auto-Abort Inhibit Enable Off
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibit(false);
		break;
	case 21: //Flight Control Computer Switch Pointer No. 2
		ControlDistributor.SetFCCSwitchPoint2On();
		break;
	case 22: //Flight Control Computer Switch Pointer No. 3
		ControlDistributor.SetFCCSwitchPoint3On();
		break;
	case 23: //Telemetry Calibrator Inflight Calibrate On
		break;
	case 24: //Telemetry Calibrator Inflight Calibrate Off
		break;
	case 26: //Flight Control Computer Switch Pointer No. 1
		ControlDistributor.SetFCCSwitchPoint1On();
		break;
	case 27: //Flight Control Computer Switch Pointer No. 9
		ControlDistributor.SetFCCSwitchPoint9On();
		break;
	case 28: //S-II Engine Out Indication "A" Enable; S-II Aft Interstage Separation Indication "A" Enable
		eds.SetSIIEngineOutIndicationA();
		break;
	case 29: //S-IVB Engine EDS Cutoff No. 1 Disable
		eds.SetSIVBEngineCutoffDisabled();
		break;
	case 30: //Spare
		break;
	case 31: //Flight Control Computer S-IVB Burn Mode On "A"
		ControlDistributor.SetSIVBBurnModeA(true);
		break;
	case 32: //Spare
		break;
	case 33: //Switch Engine Control to S-II and S-IC Outboard Engine Cant Off "A"
		ControlDistributor.SetSIIBurnModeEngineCantOff();
		break;
	case 34: //Excess Rate (Roll) Auto-Abort Inhibit Enable
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibitEnable(true);
		break;
	case 35: //S-IC Two Engines Out Auto-Abort Inhibit
		ControlDistributor.SetTwoEngOutAutoAbortInhibit();
		break;
	case 36: //Switch S-IVB LOX to S-II Fuel Tank Pressure Indicator On
		break;
	case 37: //Switch S-IVB LOX to S-II Fuel Tank Pressure Indicator Reset
		break;
	case 38: //Launch Vehicle Engines EDS Cutoff Enable
		eds.SetLVEnginesCutoffEnable1();
		break;
	case 39: //Tape Recorder Record On
		break;
	case 40: //Tape Recorder Playback On
		break;
	case 41: //Excess Rate (P,Y,R) Auto-Abort Inhibit Off
		ControlDistributor.SetExcessiveRatePYRAutoAbortInhibit(false);
		break;
	case 42: //Excess Rate (Roll) Auto-Abort Inhibit Off
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibitEnable(false);
		break;
	case 43: // S-IVB Ullage Thrust Present Indication On
		eds.SetUllageThrustIndicate(true);
		break;
	case 44: //Flight Control Computer Switch Point No. 5
		ControlDistributor.SetFCCSwitchPoint5On();
		break;
	case 45: //Spare
		break;
	case 46: //S-IVB Ullage Thrust Present Indication Off
		eds.SetUllageThrustIndicate(false);
		break;
	case 47: //Flight Control Computer Switch Point No. 8
		ControlDistributor.SetFCCSwitchPoint8On();
		break;
	case 48: //S-II Engine Out Indication "B" Enable; S-II Aft Interstage Separation Indication "B" Enable
		eds.SetSIIEngineOutIndicationB();
		break;
	case 49: //Spare
		break;
	case 50: //Excess Rate (Roll) Auto-Abort Inhibit and Switch Rate Gyro SC Indication "B"
		ControlDistributor.SetExcessiveRateRollAutoAbortInhibit(true);
		break;
	case 51: //S-IC Two Engines Out Auto-Abort Inhibit Enable
		ControlDistributor.SetTwoEngOutAutoAbortInhibitEnable();
		break;
	case 52: //LET Jettison "A"
		break;
	case 53: //S-IVB Engine Out Indication "B" Enable Reset
		eds.SetSIVBEngineOutIndicationB(false);
		break;
	case 54: //C-Band Transponder No. 1 & No. 2 On
		break;
	case 55: //Inhibit C-Band Transponder No. 1
		break;
	case 56: //Inhibit C-Band Transponder No. 2
		break;
	case 60: //PCM Coax Switch Low Gain Antenna
		break;
	case 62: //PCM Coax Switch High Gain Antenna
		break;
	case 63: //CCS Coax Switch High Gain Antenna
		break;
	case 65: //CCS Coax Switch Low Gain Antenna
		break;
	case 68: //S/C Control of Saturn Enable
		eds.EnableSCControl();
		break;
	case 69: //S/C Control of Saturn Disable
		eds.DisableSCControl();
		break;
	case 74: //Flight Control Computer S-IVB Burn Mode On "B"
		ControlDistributor.SetSIVBBurnModeB(true);
		break;
	case 75: //Flight Control Computer S-IVB Burn Mode Off "B"
		ControlDistributor.SetSIVBBurnModeB(false);
		break;
	case 80: //S-IVB Restart Alert On
		eds.SetSIVBRestartAlert(true);
		break;
	case 81: //S-IVB Restart Alert Off
		eds.SetSIVBRestartAlert(false);
		break;
	case 82: //IU Command System Enable
		eds.EnableCommandSystem();
		break;
	case 83: //S-IC Outboard Engines Cant On "A"
		ControlDistributor.SetSICEngineCantAOn();
		break;
	case 84: //S-IC Outboard Engines Cant On "B"
		ControlDistributor.SetSICEngineCantB(true);
		break;
	case 85: //S-IC Outboard Engines Cant On "C"
		ControlDistributor.SetSICEngineCantC(true);
		break;
	case 86: //S-IC Outboard Engines Cant Off "B"
		ControlDistributor.SetSICEngineCantB(false);
		break;
	case 87: //S-IC Outboard Engines Cant Off "C"
		ControlDistributor.SetSICEngineCantC(false);
		break;
	case 98: //AZUSA X-Ponder Power Off
		break;
	case 106: //S-I RF Assembly Power Off
		break;
	case 107: //Water Coolant Valve Open
		break;
	case 108: //Water Coolant Valve Closed
		break;
	case 109: //Sensor Bias On
		break;
	case 110: //Cooling System Electronic Assembly Power Off
		break;
	case 112: //Measuring Rack Power Off
		break;
	default:
		break;
	}
}