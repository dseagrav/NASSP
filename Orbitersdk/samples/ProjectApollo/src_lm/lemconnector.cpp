/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

LM connector classes

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
#include "apolloguidance.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "iu.h"
#include "sivbsystems.h"
#include "sivb.h"
#include "lemconnector.h"

LEMConnector::LEMConnector(LEM *l)

{
	OurVessel = l;
}

LEMConnector::~LEMConnector()

{
}

LEMECSConnector::LEMECSConnector(LEM *l) : LEMConnector(l)
{
	type = NO_CONNECTION;
	connectedTo = 0;
}

bool LEMECSConnector::ReceiveMessage(Connector *from, ConnectorMessage &m) {

	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	if (m.messageType == 0)
	{
		m.val1.pValue = OurVessel->GetLMTunnelPipe();

		return true;
	}
	else if (m.messageType == 1)
	{
		OurVessel->ConnectTunnelToCabinVent();

		return true;
	}

	return false;
}

// UMBILICAL
LEMPowerConnector::LEMPowerConnector(LEM *l) : LEMConnector(l)
{
	type = NO_CONNECTION;
	connectedTo = 0;
	csm_power_latch = 0;
}

bool LEMPowerConnector::ReceiveMessage(Connector *from, ConnectorMessage &m) {
	// This should only get messages of type 42 from the CM telling it to switch relay states
	// on our side. EDIT: And now also the command for SLA separation.
	if (from != this) {
		switch (m.messageType)
		{
		case 42:
			// Relay Event
			// When connected, the CSM feeds the LM via two 7.5A umbilicals. Both feed the same stuff, they are redundant.
			// The CSM power comes in via the CDRs DC bus and returns to the CSM via the CDR and LMP XLUNAR busses.
			// The XLUNAR busses are latched to the CDR and LMP negative busses to provide return when LM PWR switch is in CSM.
			// When CSM power is commanded on, it turns off the descent ECAs and prevents them from being turned back on.
			// Ascent power is not affected.
			// The ECA design makes it impossible to charge the LM batteries from the CSM power supply, since it prevents current from flowing backwards.
			switch (m.val1.iValue) {
			case 0: // Disconnect				
				csm_power_latch = -1;
				// sprintf(oapiDebugString(),"LM/CSM Conn: Latch Reset");
				break;
			case 1: // Connect
				csm_power_latch = 1;
				// sprintf(oapiDebugString(),"LM/CSM Conn: Latch Set");
				break;
			default:
				sprintf(oapiDebugString(), "LM/CSM Conn: Relay Event: Bad parameter %d", m.val1.iValue);
				return false;
			}
			break;
		default:
			return false;
		
		}
		return true;
	}
	// Debug: Complain if we got garbage
	sprintf(oapiDebugString(), "LM/CSM Conn: Bad message: Type %d parameter %d", m.messageType, m.val1.iValue);
	return false;
}

LMToSIVBConnector::LMToSIVBConnector(LEM *l) : LEMConnector(l)
{
}

LMToSIVBConnector::~LMToSIVBConnector()
{
}

void LMToSIVBConnector::StartSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_START_SEPARATION;

	SendMessage(cm);
}

void LMToSIVBConnector::StopSeparationPyros()
{
	ConnectorMessage cm;

	cm.destination = type;
	cm.messageType = SLA_STOP_SEPARATION;

	SendMessage(cm);
}

LEMCommandConnector::LEMCommandConnector(LEM *l) : LEMConnector(l)
{
	type = CSM_PAYLOAD_COMMAND;
}

LEMCommandConnector::~LEMCommandConnector()
{

}

bool LEMCommandConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	PayloadSIVBMessageType messageType;

	messageType = (PayloadSIVBMessageType)m.messageType;

	switch (messageType)
	{
	case SLA_START_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StartSeparationPyros();
			return true;
		}
		break;

	case SLA_STOP_SEPARATION:
		if (OurVessel)
		{
			OurVessel->StopSeparationPyros();
			return true;
		}
		break;

	}

	return false;
}

LM_RRtoCSM_RRT_Connector::LM_RRtoCSM_RRT_Connector(LEM *l, LEM_RR *lm_rr) : LEMConnector(l)
{
	type = RADAR_RF_SIGNAL;
	lemrr = lm_rr;
}

LM_RRtoCSM_RRT_Connector::~LM_RRtoCSM_RRT_Connector()
{

}

void LM_RRtoCSM_RRT_Connector::SendRF(double freq, double XMITpow, double XMITgain, double Phase)
{
	ConnectorMessage cm;

	cm.destination = RADAR_RF_SIGNAL;
	cm.messageType = CW_RADAR_SIGNAL;

	cm.val1.dValue = freq;
	cm.val2.dValue = XMITpow;
	cm.val3.dValue = XMITgain;
	cm.val4.dValue = Phase;

	SendMessage(cm);

	//sprintf(oapiDebugString(), "Hey this Function got called at %lf", oapiGetSimTime());

}

bool LM_RRtoCSM_RRT_Connector::ReceiveMessage(Connector * from, ConnectorMessage & m)
{
	//sprintf(oapiDebugString(), "Hey this Function got called at %lf", oapiGetSimTime()); //debugging
	if (m.destination != type)
	{
		return false;
	}

	if (!lemrr)
	{
		return false;
	}

	LM_RRmessageType messageType;
	messageType = (LM_RRmessageType)m.messageType;

	switch (messageType)
	{
		case RR_XPDR_SIGNAL:
		{
			//sprintf(oapiDebugString(),"Frequency Received: %lf MHz", m.val1.dValue);
			lemrr->SetRCVDrfProp(m.val1.dValue, m.val2.dValue, m.val3.dValue, m.val4.dValue);

			return true;
		}
		case CW_RADAR_SIGNAL:
		{
			return false;
		}
	}

	return false;
}