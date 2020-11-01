/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: Connector class

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

#if !defined(_PA_CONNECTOR_H)
#define _PA_CONNECTOR_H

///
/// \ingroup Connectors
/// \brief Connector type.
///
enum ConnectorType
{
	NO_CONNECTION,				///< Dummy.
	CSM_IU_COMMAND,				///< Passes commands and data between CSM and IU.
	LV_IU_COMMAND,				///< Passes commands between launch vehicle and IU.
	CSM_LEM_DOCKING,			///< Docking connector between CSM and LEM.
	LEM_CSM_POWER,				///< Power connection from CSM to LEM.
	MFD_PANEL_INTERFACE,		///< Connector from an MFD to a panel.
	LEM_CSM_ECS,				///< Tunnel connection from CSM to LEM.
	CHECKLIST_DATA_INTERFACE,	///< Data connector from checklist controller to vessel
	PAYLOAD_SLA_CONNECT,		///< Passes commands and data between payload and SIVb.
	CSM_PAYLOAD_COMMAND,		///< Docking connector between CSM and Payload
	SIVB_SI_COMMAND,			///< Docking connector between S-IVB and S-IB/S-II
	SII_SIC_COMMAND,			///< Docking connector between S-II and S-IC
	RADAR_RF_SIGNAL,			///< Radar connector betwen LM rendezvous radar amd CSM rendezvous radar transponder
};

#define VIRTUAL_CONNECTOR_PORT	(0xffff)		///< Port ID for 'virtual' connectors which don't physically exist.

//
// The message can pass various different parameters in this union. The receiver
// will determine which is correct based on the message type.
//
// \brief Message value for connector messages.
//
union ConnectorMessageValue
{
	int iValue;				///< Integer message value.
	double dValue;			///< Floating point message value.
	bool bValue;			///< Boolean message value.
	void *pValue;			///< Pointer message value.
	OBJHANDLE hValue;		///< Orbiter handle.
	VECTOR3 vValue;			///< Vector message value;
};

///
/// \ingroup Connectors
/// \brief Connector message to be passed through the system.
///
struct ConnectorMessage
{
	///
	/// \brief What kind of connector should this message go to?
	///
	ConnectorType destination;

	///
	/// \brief Connection-specific message type.
	///
	unsigned int messageType;

	///
	/// \brief Message value 1.
	///
	ConnectorMessageValue val1;

	///
	/// \brief Message value 2.
	///
	ConnectorMessageValue val2;

	///
	/// \brief Message value 3.
	///
	ConnectorMessageValue val3;

	///
	/// \brief Message value 4.
	///
	ConnectorMessageValue val4;
};

///
/// \ingroup Connectors
/// \brief Connector class. Specific connectors will be derived from this class.
///
class Connector {
public:
	///
	/// \brief Constructor.
	///
	Connector();

	///
	/// \brief Destructor. Disconnects the connector when called.
	///
	virtual ~Connector();

	///
	/// \brief Get the type of the connector.
	/// \return Connector type.
	///
	virtual ConnectorType GetType();

	///
	/// \brief Set the type of the connector.
	/// \param t Connector type.
	///
	void SetType(ConnectorType t) { type = t; };

	///
	/// \brief Connect to another connector.
	/// \param other Other end of the connection.
	/// \return True if the connector is the correct type and we connected.
	///
	virtual bool ConnectTo(Connector *other);

	///
	/// \brief Disconnect from the far end of the connection.
	///
	virtual void Disconnect();

	///
	/// \brief Has been disconnected from the far end of the connection.
	///
	virtual void Disconnected();

	///
	/// Send a message through the connector. Note that the receiver can update the value in the
	/// connector message, in order to return data to the caller.
	///
	/// \brief Send message.
	/// \param m Message to send.
	/// \return False if the connector isn't connected to anything or the message wasn't handled
	/// at the far end of the connection.
	///
	virtual bool SendMessage(ConnectorMessage &m);

	///
	/// Receieve a message through the connector. Note that the receiver can update the value in the
	/// connector message, in order to return data to the caller.
	///
	/// The default connector is output-only, and will return an error if the other end of the connection
	/// tries to send it data.
	///
	/// \brief Receive message.
	/// \param from The connector that sent the message.
	/// \param m Message received.
	/// \return False if we don't handle the message.
	///
	virtual bool ReceiveMessage(Connector *from, ConnectorMessage &m);

	///
	/// \brief Connector we're connected to, if any.
	///
	Connector *connectedTo;

protected:
	///
	/// \brief Type of connection.
	///
	ConnectorType type;
};

///
/// \ingroup Connectors
/// \brief Connector class for multiple connectors, typically used for docked ships.
///
class MultiConnector : public Connector
{
public:
	MultiConnector();
	~MultiConnector();

	///
	/// \brief Add another connection to this connector.
	/// \param other Connector to add.
	/// \return True if we could add the new connector.
	///
	virtual bool AddTo(Connector *other);

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
	void Disconnect();

#define N_MULTICONNECT_INPUTS 16

private:
	Connector *Inputs[N_MULTICONNECT_INPUTS];
};

class PowerDrainConnectorObject;

///
/// \ingroup Connectors
/// \brief Connector class for power drain.
///
class PowerDrainConnector : public Connector
{
public:
	PowerDrainConnector();
	~PowerDrainConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
	void Disconnected();

	void SetPowerDrain(PowerDrainConnectorObject *p);

private:
	PowerDrainConnectorObject *power_drain;
};

class PanelSwitches;
class ChecklistController;

///
/// \ingroup Connectors
/// \brief Connector class for panel interface.
///
class PanelConnector : public Connector
{
public:

	///
	/// \ingroup Connectors
	/// \brief Message type to send from the CSM to the SIVb.
	///
	enum PanelConnectorMessageType
	{
		MFD_PANEL_FLASH_ITEM,					///< Turn flash on or off.
		MFD_PANEL_GET_ITEM_STATE,				///< Get the item's current state.
		MFD_PANEL_SET_ITEM_STATE,				///< Set the item's current state.
		MFD_PANEL_GET_FAILED_STATE,				///< Get the item's failed state.
		MFD_PANEL_CHECKLIST_AUTOCOMPLETE,		///< Checklist autocomplete.
		MFD_PANEL_GET_CHECKLIST_ITEM,			///< Get the an checklist item.
		MFD_PANEL_GET_CHECKLIST_LIST,			///< Get list of allowed checklists.
		MFD_PANEL_FAIL_ITEM,					///< Fail a checklist step.
		MFD_PANEL_COMPLETE_ITEM,				///< Complete a checklist step.
		MFD_PANEL_CHECKLIST_AUTOCOMPLETE_QUERY,	///< Find out the autocomplete state.
		MFD_PANEL_CHECKLIST_NAME,				///< Find out the name of the current checklist
		MFD_PANEL_RETRIEVE_CHECKLIST,			///< Get an entire, non-controlled, checklist
		MFD_PANEL_CHECKLIST_FLASHING,			///< Checklist item flashing.
		MFD_PANEL_CHECKLIST_FLASHING_QUERY,		///< Checklist item flashing.
		MFD_PANEL_GET_ITEM_FLASHING,			///< Get the item's current flashing.
	};

	PanelConnector(PanelSwitches &p, ChecklistController &c);
	~PanelConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);

private:
	PanelSwitches &panel;
	ChecklistController &checklist;
};

///
/// ProjectApollo-specific vessel which allows us to get connectors to communicate
/// while docked.
/// \ingroup Connectors
///

class ProjectApolloConnectorVessel : public VESSEL4
{
public:

	struct ConnectorDefinition
	{
		int port;
		Connector *c;

		ConnectorDefinition() { port = 0; c = 0; };
	};

	///
	/// \brief Constructor.
	///
	ProjectApolloConnectorVessel(OBJHANDLE hObj, int fmodel);

	///
	/// Other vessels can call this function to get a connector to talk to when they
	/// are docked. They need to specify the docking port number so we can have multiple
	/// connectors of the same time on vessels with multiple ports; for example a space
	/// station may supply power to each docking port.
	///
	/// \brief Get a pointer to a connector of specified type.
	/// \param port Docking port number.
	/// \param t Connector type to look for.
	/// \return Pointer to connector if one is registered, NULL if none registered.
	///
	virtual Connector *GetConnector(int port, ConnectorType t);

	///
	/// This is a sanity-check. If the validation fails, then the vessel probably isn't really of this class!
	///
	/// \return True if this is a valid vessel.
	///
	bool ValidateVessel();

	///
	/// \brief Set up connectors on docking.
	///
	void DockConnectors(int port);

	///
	/// \brief Disconnect connectors on undocking.
	///
	void UndockConnectors(int port);

protected:

	///
	/// \brief Register a connector for use by other vessels.
	/// \param port Docking port number.
	/// \param c Pointer to a connector.
	/// \return True if registered, false if not (e.g. too many registered already).
	///
	bool RegisterConnector(int port, Connector *c);

#define PACV_N_VALIDATION	0x5a715a75

	///
	/// We store a known value here for validation. If a vessel is not of this class then it's unlikely to
	/// have the same value at this location and will fail to validate; unfortunately it may also crash if
	/// the vessel happens to have been allocated at the end of the heap!
	///
	/// \brief Validation value.
	///
	unsigned int ValidationValue;

#define PACV_N_CONNECTORS 16

	ConnectorDefinition ConnectorList[PACV_N_CONNECTORS];
};

///
/// \ingroup Connectors
///
/// This function tries to get a connector from the specified docking port on the
/// specified vessel. It tries to determine whether the connector is a Project
/// Apollo vessel and then tries to get the connector pointer if it is.
///
/// \brief Get a connector from a vessel.
/// \param v Vessel, which may or may not be one of ours.
/// \param port Docking port number.
/// \param t Connector type to look for.
/// \return Connector if found, or NULL if not.
///
extern Connector *GetVesselConnector(VESSEL *v, int port, ConnectorType t);

///
/// \ingroup Connectors
/// \brief Radar Messages
///
enum LM_RRmessageType {
	CW_RADAR_SIGNAL, ///< Continuous Wave Radar Signal
	RR_XPDR_SIGNAL, ///< Radar Transponder Signal
};

#endif // _PA_CONNECTOR_H
