/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: SIVb class

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

#if !defined(_PA_SIVB_H)
#define _PA_SIVB_H

#include "PanelSDK/PanelSDK.h"
#include "payload.h"
#include "pyro.h"

//
// Data structure passed from main vessel to SIVB to configure stage.
//

///
/// Flags structure indicating which of the SIVB settings are valid.
///
/// \brief SIVB settings flags.
/// \ingroup SepStageSettings
///
union SIVbSettingFlags
{
	struct {
		unsigned SIVB_SETTINGS_MASS:1; 			///< Mass settings are valid.
		unsigned SIVB_SETTINGS_FUEL:1;			///< Fuel mass settings are valid.
		unsigned SIVB_SETTINGS_GENERAL:1;		///< General settings (e.g. Mission Time) are valid.
		unsigned SIVB_SETTINGS_PAYLOAD:1;		///< Payload type settings are valid.
		unsigned SIVB_SETTINGS_ENGINES:1;		///< Engine settings are valid.
		unsigned SIVB_SETTINGS_PAYLOAD_INFO:1;	///< Detailed payload settings (e.g. LEM name/mass/PAD) are valid.
	};
	unsigned int word;						///< Set to zero to clear all flags.

	///
	/// \brief Constructor: clear all flags by default.
	///
	SIVbSettingFlags() { word = 0; };
};

class IU;
class SIVBSystems;

///
/// Data structure passed from main vessel to SIVb to configure it after staging.
///
/// \brief SIVb setup structure.
/// \ingroup SepStageSettings
///
struct SIVBSettings 
{
	SIVbSettingFlags SettingsType;	///< Which settings are valid?

	int Payload;					///< Payload type.
	int VehicleNo;					///< Saturn vehicle number.
	int MissionNo;					///< Mission number.

	double THRUST_VAC;				///< Vacuum thrust.
	double ISP_VAC;					///< Vacuum ISP.

	double MissionTime;				///< Current MET in seconds.
	double EmptyMass;				///< Empty mass in kg.
	double PayloadMass;				///< Payload mass in kg.
	double ApsFuel1Kg;				///< APS fuel no. 1 in kg.
	double ApsFuel2Kg;				///< APS fuel no. 2 in kg.
	double MainFuelKg;				///< Remaining fuel in kg.

	bool PanelsHinged;				///< Are SLA panels hinged?
	double PanelProcess;			///< SLA Panels opening progress
	bool SaturnVStage;				///< Saturn V stage or Saturn 1b stage?
	bool LowRes;					///< Low-res meshes?
	bool IUSCContPermanentEnabled;

	double SLARotationLimit;		///< SLA rotation limit in degrees (usually 45.0).

	//
	// Payload settings.
	//

	double LMDescentFuelMassKg;		///< Mass of fuel in descent stage of LEM.
	double LMAscentFuelMassKg;		///< Mass of fuel in ascent stage of LEM.
	double LMDescentEmptyMassKg;	///< Empty mass of descent stage of LEM.
	double LMAscentEmptyMassKg;		///< Empty mass of ascent stage of LEM.
	char PayloadName[64];			///< Payload Name
	char CSMName[64];
	bool Crewed;

	int LMPadCount;					///< Count of LM PAD data.
	unsigned int *LMPad;			///< LM PAD data.
	int AEAPadCount;				///< Count of AEA PAD data.
	unsigned int *AEAPad;			///< AEA PAD data.

	///
	/// LEM checklist file
	///
	char LEMCheck[100];

	SIVBSettings() { LMPad = 0; LMPadCount = 0; AEAPad = 0; AEAPadCount = 0; LEMCheck[0] = 0;};

	IU *iu_pointer;
};

class SIVB;
class Battery;

///
/// \ingroup Connectors
/// \brief SIVB class connector base class.
///
class SIVbConnector : public Connector
{
public:
	SIVbConnector();
	~SIVbConnector();

	void SetSIVb(SIVB *sat) { OurVessel = sat; };

protected:
	SIVB *OurVessel;
};

///
/// \ingroup Connectors
/// \brief SIVb to IU command connector type.
///
class SIVbToIUCommandConnector : public SIVbConnector
{
public:
	SIVbToIUCommandConnector();
	~SIVbToIUCommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

///
/// \ingroup Connectors
/// \brief LM to SIVb command connector type.
///
class PayloadToSLACommandConnector : public SIVbConnector
{
public:
	PayloadToSLACommandConnector();
	~PayloadToSLACommandConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

//Messages to S-IB or S-II
enum SIVBSIMessageType
{
	SIVB_SI_SWITCH_SELECTOR,
	SIVB_SI_THRUSTER_DIR,
	SIVB_SI_SIB_LOW_LEVEL_SENSORS_DRY,
	SIVB_SI_PROPELLANT_DEPLETION_ENGINE_CUTOFF,
	SIVB_SI_GETSITHRUSTOK
};

//S-IVB to S-IB or S-II Connector
class SIVBToSIConnector : public SIVbConnector
{
public:
	SIVBToSIConnector();
	~SIVBToSIConnector();

	void SISwitchSelector(int channel);
	void SetSIThrusterDir(int n, double yaw, double pitch);

	bool GetLowLevelSensorsDry();
	bool GetSIPropellantDepletionEngineCutoff();
	void GetSIThrustOK(bool *ok, int n);
};

///
/// This code simulates the seperated SIVb stage. Basically it simulates thrust decay if there is any fuel 
/// left, fires any retro rockets to push it away from the Saturn and then sits around waiting to be deleted.
///
/// \brief SIVb stage simulation.
/// \ingroup SepStages
///
class SIVB : public ProjectApolloConnectorVessel {

public:

	///
	/// Specifies the main state of the SIVb
	///
	/// \brief SIVb state.
	/// \ingroup SepStageSettings
	///
	enum SIVbState
	{
		SIVB_STATE_SETUP = -1,				///< SII is waiting for setup call.
		SIVB_STATE_WAITING					///< SII is idle after motor burnout.
	};

	///
	/// \ingroup ScenarioState
	/// \brief Main SIVB state-saving structure.
	///
	typedef union {
		struct {
			unsigned PanelsHinged:1;
			unsigned PanelsOpened:1;
			unsigned SaturnVStage:1;
			unsigned LowRes:1;
			unsigned IUSCContPermanentEnabled:1;
			unsigned PayloadCreated:1;
			unsigned Payloaddatatransfer:1;
		};
		unsigned long word;
	} MainState;

	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	SIVB (OBJHANDLE hObj, int fmodel);
	virtual ~SIVB();

	///
	/// \brief Orbiter state saving function.
	/// \param scn Scenario file to save to.
	///
	void clbkSaveState (FILEHANDLE scn);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPreStep(double simt, double simdt, double mjd);

	void clbkPostStep(double simt, double simdt, double mjd);

	///
	/// \brief Orbiter state loading function.
	/// \param scn Scenario file to load from.
	/// \param status Pointer to current vessel status.
	///
	void clbkLoadStateEx (FILEHANDLE scn, void *status);

	///
	/// \brief Orbiter class configuration function.
	/// \param cfg File to load configuration defaults from.
	///
	void clbkSetClassCaps (FILEHANDLE cfg);

	///
	/// \brief Orbiter dock state function.
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);
	void clbkPostCreation();

	///
	/// Pass settings from the main DLL to the jettisoned SIVb. This call must be virtual 
	/// so it can be called from other DLLs without building in the LES code.
	/// \brief Setup jettisoned SIVb.
	/// \param state SIVb state settings.
	///
	virtual void SetState(SIVBSettings &state);

	int GetVehicleNo();

	bool GetSIVBThrustOK();

	void SetSIVBThrusterDir(double yaw, double pitch);
	void SetAPSAttitudeEngine(int n, bool on);
	void SIVBEDSCutoff(bool cut);
	void SIVBSwitchSelector(int channel);

	//Signals to lower stages
	void SISwitchSelector(int channel);
	void SetSIThrusterDir(int n, double yaw, double pitch);
	bool GetSIBLowLevelSensorsDry();
	bool GetSIPropellantDepletionEngineCutoff();

	IU *GetIU() { return iu; };

	///
	/// \brief Get main propellant mass.
	/// \return Propellant mass in kg.
	///
	double GetSIVbPropellantMass();

	virtual double GetPayloadMass();

	///
	/// \brief Get main battery power.
	/// \return Power in joules.
	///
	double GetMainBatteryPower();

	///
	/// \brief Get main battery power drain.
	/// \return Power in joules.
	///
	double GetMainBatteryPowerDrain();

	///
	/// \brief Get main battery voltage.
	/// \return Voltage in volts.
	///
	double GetMainBatteryVoltage();

	///
	/// \brief Get main battery current.
	/// \return Current in amps.
	///
	double GetMainBatteryCurrent();

	///
	/// \Create payload vessel
	///
	void CreatePayload();

	///
	/// \brief Start payload separation.
	///
	void StartSeparationPyros();

	///
	/// \brief Stop payload separation.
	///
	void StopSeparationPyros();

	void StartSLASeparationPyros();
	void SeparateCSM();
	bool IsLowerStageDocked();

	SIVBToSIConnector *GetSIVBSIConnector() { return &sivbSIConnector; }

protected:
	///
	/// PanelSDK functions as a interface between the
	/// actual System & Panel SDK and VESSEL class
	///
	/// Note that this must be defined early in the file, so it will be initialised
	/// before any other classes which rely on it at creation. Don't move it further
	/// down without good reason, or you're likely to crash!
	///
	/// \brief Panel SDK library.
	///
    PanelSDK Panelsdk;

	bool PanelSDKInitalised;

	///
	/// \brief Set SIVb state.
	///
	void SetS4b();

	///
	/// \brief Initialise SIVb state.
	///
	void InitS4b();

	///
	/// \brief Get the main state flags as a 32-bit value to save to the scenario file.
	/// \return 32-bit flags value.
	///
	int GetMainState();

	///
	/// \brief Set the main state flags from a 32-bit value loaded from the scenario file.
	/// \param s 32-bit flags value.
	///
	void SetMainState(int s);

	///
	/// \brief Get the spacecraft name based on the Apollo mission and vehicle number.
	/// \param s String to hold the spacecraft name.
	///
	void GetApolloName(char *s);
	void AddRCS_S4B();				///< Add RCS for SIVb control.
	void Boiloff();					///< Boil off some LOX/LH2 in orbit.

	void CreateSISIVBInterface();
	bool GetDockingPortFromHandle(OBJHANDLE port, UINT &num);
	void CreateAirfoils();

	bool PayloadIsDetachable();		///< Is the payload detachable?

	VECTOR3	mainExhaustPos;			///< Position of main thruster exhaust.

	int PayloadType;				///< Payload type.
	int VehicleNo;					///< Saturn vehicle number.
	SIVbState State;				///< Main stage state.
	PayloadSettings payloadSettings;

	double EmptyMass;				///< Empty mass in kg.
	double PayloadMass;				///< Payload mass in kg.
	double MainFuel;				///< Main fuel mass in kg.
	double ApsFuel1Kg;				///< APS fuel no. 1 in kg.
	double ApsFuel2Kg;				///< APS fuel no. 2 in kg.

	double MissionTime;				///< Current MET in seconds.
	double NextMissionEventTime;	///< Next event time for automated operation.
	double LastMissionEventTime;	///< Last event time.

	bool PanelsHinged;				///< SLA panels are hinged.
	bool PanelsOpened;				///< SLA Panels are open.
	bool SaturnVStage;				///< Stage from Saturn V.
	bool LowRes;					///< Using low-res meshes.
	bool IUSCContPermanentEnabled;
	bool PayloadCreated;

	double RotationLimit;			///< Panel rotation limit from 0.0 to 1.0 (1.0 = 180 degrees).
	double CurrentThrust;			///< Current thrust level (0.0 to 1.0).

	double THRUST_THIRD_VAC;		///< J2 engine thrust vacuum level in Newtons.
	double ISP_THIRD_VAC;			///< J2 engine ISP in vacuum.

	double PayloadEjectionForce;	///< Force applied at "undocking" of the payload attached at the front of the SLA (CSM, nosecone etc.)

	// Exterior light definitions
	BEACONLIGHTSPEC dockingLights[5];             // docking lights

	//
	// LM PAD
	//

	int LMPadCount;					///< Count of LM PAD values.
	unsigned int *LMPad;			///< LM PAD load data.

	int LMPadLoadCount;
	int LMPadValueCount;

	int AEAPadCount;				///< Count of AEA PAD values.
	unsigned int *AEAPad;			///< AEA PAD load data.

	int AEAPadLoadCount;
	int AEAPadValueCount;

	char PayloadName[64];			///< Name of payload, if appropriate.

	bool Payloaddatatransfer;		///< Have we transferred data to the payload?

	bool FirstTimestep;

	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;

	ATTACHMENTHANDLE hattDROGUE;

	///
	/// \brief Instrument Unit.
	///
	IU* iu;

	bool iuinitflag;

	SIVBSystems *sivbsys;

	///
	/// \brief Command connector from SIVb to IU.
	///
	SIVbToIUCommandConnector IUCommandConnector;

	///
	/// \brief Command connector from LM to SIVb.
	///

	PayloadToSLACommandConnector payloadSeparationConnector;
	SIVBToSIConnector sivbSIConnector;

	///
	/// \brief Handle of docked vessel.
	///
	DOCKHANDLE hDock, hDockSI, hDockCSM;

	Battery *MainBattery;

	THRUSTER_HANDLE th_aps_rot[6], th_main[1], th_aps_ull[2];                 // handles for APS engines
	THRUSTER_HANDLE th_lox_vent;
	THGROUP_HANDLE thg_main, thg_sep, thg_sepPanel, thg_ver;
	PROPELLANT_HANDLE ph_aps1, ph_aps2, ph_main;

	UINT panelAnim;
	UINT panelAnimPlusX;
	double panelProc;
	double panelProcPlusX;
	int panelTimestepCount;
	int panelMesh1SaturnV, panelMesh2SaturnV, panelMesh3SaturnV, panelMesh4SaturnV;
	int panelMesh1SaturnVLow, panelMesh2SaturnVLow, panelMesh3SaturnVLow, panelMesh4SaturnVLow;
	int panelMesh1Saturn1b, panelMesh2Saturn1b, panelMesh3Saturn1b, panelMesh4Saturn1b;
	int meshSivbSaturnV, meshSivbSaturnVLow, meshSivbSaturn1b, meshSivbSaturn1bLow, meshSivbSaturn1bcross;
	int meshASTP_A, meshASTP_B, meshCOASTarget_A, meshCOASTarget_B, meshCOASTarget_C;
	int meshApollo8LTA, meshLTA_2r;

	void HideAllMeshes();

	Pyro CSMLVSeparationInitiator;
	Pyro LMSLASeparationInitiators;
	Pyro SLAPanelDeployInitiator;
};

///
/// \ingroup Connectors
/// \brief Message type to send from the payload to the SIVb.
///
enum PayloadSIVBMessageType
{
	SLA_START_SEPARATION,			///< Start charging separation pyros.
	SLA_STOP_SEPARATION,			///< Stop charging separation pyros.
};

#endif // _PA_SIVB_H
