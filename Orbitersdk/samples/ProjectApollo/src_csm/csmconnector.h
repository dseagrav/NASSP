/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: CSM connector classes

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

#if !defined(_PA_CSMCONNECTOR_H)
#define _PA_CSMCONNECTOR_H

class Saturn;
class CSMcomputer;

///
/// \ingroup Connectors
/// \brief Saturn-class connector base class.
///
class SaturnConnector : public Connector
{
public:
	SaturnConnector(Saturn *s);
	~SaturnConnector();

	void SetSaturn(Saturn *sat) { OurVessel = sat; };

protected:
	Saturn *OurVessel;
};

///
/// \ingroup Connectors
/// \brief CSM to IU connector type.
///
class CSMToIUConnector : public SaturnConnector
{
public:
	CSMToIUConnector(CSMcomputer &c, Saturn *s);
	~CSMToIUConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	bool GetLiftOffCircuit(bool sysA);
	bool GetEDSAbort(int n);
	double GetLVTankPressure(int n);
	bool GetAbortLight();
	bool GetQBallPower();
	bool GetQBallSimulateCmd();

protected:
	CSMcomputer &agc;
};

///
/// \ingroup Connectors
/// \brief Saturn to IU command connector type.
///
class SaturnToIUCommandConnector : public SaturnConnector
{
public:
	SaturnToIUCommandConnector(Saturn *s);
	~SaturnToIUCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

class h_Pipe;

class CSMToLEMECSConnector : public SaturnConnector
{
public:
	CSMToLEMECSConnector(Saturn *s);
	~CSMToLEMECSConnector();

	bool ConnectTo(Connector *other);
	void Disconnect();

	h_Pipe* GetDockingTunnelPipe();
	void ConnectLMTunnelToCabinVent();
};

class CSMToPayloadConnector : public SaturnConnector
{
public:
	CSMToPayloadConnector(Saturn *s);
	~CSMToPayloadConnector();

	void StartSeparationPyros();
	void StopSeparationPyros();
};

class RNDZXPDRSystem;

class CSM_RRTto_LM_RRConnector : public SaturnConnector
{
public:
	CSM_RRTto_LM_RRConnector(Saturn *s, RNDZXPDRSystem *rrt); //constructor
	~CSM_RRTto_LM_RRConnector(); //descructor

	void SendRF(double freq, double XMITpow, double XMITgain, double Phase);
	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

protected:
	RNDZXPDRSystem* csm_rrt; //pointer to the instance of the RR that's doing the sending
};

#endif // _PA_CSMCONNECTOR_H
