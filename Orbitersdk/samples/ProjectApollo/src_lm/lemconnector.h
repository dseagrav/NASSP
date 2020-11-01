/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

LM connector classes (Header)

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

#include "connector.h"

class LEM;
class LEMcomputer;

class LEMConnector : public Connector
{
public:
	LEMConnector(LEM *l);
	~LEMConnector();

	void SetLEM(LEM *lem) { OurVessel = lem; };

protected:
	LEM *OurVessel;
};

class LEMECSConnector : public LEMConnector
{
public:
	LEMECSConnector(LEM *l);
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

// ELECTRICAL
// LEM to CSM Power Connector
class LEMPowerConnector : public LEMConnector
{
public:
	LEMPowerConnector(LEM *l);
	int csm_power_latch;
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

///
/// \ingroup Connectors
/// \brief LM to S-IVB connector type.
///
class LMToSIVBConnector : public LEMConnector
{
public:
	LMToSIVBConnector(LEM *l);
	~LMToSIVBConnector();

	void StartSeparationPyros();
	void StopSeparationPyros();
};

//CSM to LM command connector

class LEMCommandConnector : public LEMConnector
{
public:
	LEMCommandConnector(LEM *l);
	~LEMCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

class LEM_RR;

class LM_RRtoCSM_RRT_Connector : public LEMConnector
{
public:
	LM_RRtoCSM_RRT_Connector(LEM *l, LEM_RR *lm_rr); //constructor
	~LM_RRtoCSM_RRT_Connector(); //descructor

	void SendRF(double freq, double XMITpow, double XMITgain, double Phase);
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	void SetRR(LEM_RR* lm_rr) { lemrr = lm_rr; };

protected:
	LEM_RR* lemrr; //pointer to the instance of the RR that's doing the sending
};