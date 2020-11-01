/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: S1B class

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

#include "s1bsystems.h"
#include "PanelSDK/PanelSDK.h"
#include "pyro.h"
#include "soundlib.h"

//
// Data structure passed from main vessel to SIVB to configure stage.
//

///
/// Flags structure indicating which of the S1b settings are valid.
///
/// \brief S1b settings flags.
/// \ingroup SepStageSettings
///
union S1bSettingFlags
{
	struct {
		unsigned S1B_SETTINGS_MASS:1; 		///< Mass settings are valid.
		unsigned S1B_SETTINGS_FUEL:1;		///< Fuel mass settings are valid.
		unsigned S1B_SETTINGS_GENERAL:1;	///< General settings (e.g. Mission Time) are valid.
		unsigned S1B_SETTINGS_ENGINES:1;	///< Engine settings (e.g. ISP) are valid.
	};
	unsigned int word;						///< Set to zero to clear all flags.

	///
	/// \brief Constructor: clear all flags by default.
	///
	S1bSettingFlags() { word = 0; };
};

///
/// Data structure passed from main vessel to S1b to configure it after staging.
///
/// \brief S1b setup structure.
/// \ingroup SepStageSettings
///
typedef struct {

	S1bSettingFlags SettingsType;			///< Which settings are valid?

	int VehicleNo;							///< Saturn vehicle number.
	int RetroNum;							///< Number of retros.
	int EngineNum;							///< Number of engines.

	double THRUST_FIRST_VAC;				///< Single-engine thrust in vacuum.
	double ISP_FIRST_VAC;					///< ISP in vacuum.
	double ISP_FIRST_SL;					///< ISP at sea-level.
	double EmptyMass;						///< Empty mass in kg.
	double MainFuelKg;						///< Current fuel mass in kg.
	double CurrentThrust;					///< Current thrust level (0.0 to 1.0)

	bool LowRes;							///< Low-res meshes?

} S1BSettings;

//
// Stage states.
//

///
/// Specifies the main state of the S1b
///
/// \brief S1b state.
/// \ingroup SepStageSettings
///
enum S1bState
{
	SIB_STATE_HIDDEN = -1,				///< S1b is waiting for setup call.
	S1B_STATE_SHUTTING_DOWN,			///< S1b is firing motors to jettison.
	S1B_STATE_WAITING					///< S1b is idle after motor burnout.
};

class S1B;

class SIBConnector : public Connector
{
public:
	SIBConnector();
	~SIBConnector();

	void SetSIB(S1B *sat) { OurVessel = sat; };

protected:
	S1B *OurVessel;
};

class SIBtoSIVBConnector : public SIBConnector
{
public:
	SIBtoSIVBConnector();
	~SIBtoSIVBConnector();

	bool ReceiveMessage(Connector *from, ConnectorMessage &m);
};

///
/// This code simulates the seperated S1b stage. Basically it simulates thrust decay if there is any fuel 
/// left, fires any retro rockets to push it away from the Saturn and then sits around waiting to be deleted.
///
/// \brief S1b stage simulation.
/// \ingroup SepStages
///
class S1B : public ProjectApolloConnectorVessel {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	S1B (OBJHANDLE hObj, int fmodel);
	virtual ~S1B();

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
	/// \brief Orbiter dock state function. Does the S1b need this?
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);

	void clbkPostCreation();

	///
	/// Pass settings from the main DLL to the jettisoned S1b. This call must be virtual 
	/// so it can be called from other DLLs without building in the S1b code.
	//
	/// \brief Setup jettisoned S1b.
	/// \param state S1b state settings.
	///
	virtual void SetState(S1BSettings &state);
	virtual void LoadMeshes(bool lowres);

	virtual SIBSystems* GetSIB() { return &sibsys; }

	void SetH1ThrusterDir(int n, double beta_y, double beta_p);
	void SwitchSelector(int channel);
	bool GetLowLevelSensorsDry();
	bool GetSIPropellantDepletionEngineCutoff();
	void GetSIThrustOK(bool *ok);

protected:

	void AddEngines();
	void SeparateSIVB();

	int GetMainState();
	void SetMainState(int s);

	void CreateAirfoils();

	int MissionNo;
	int VehicleNo;
	S1bState State;
	int RetroNum;
	int EngineNum;

	bool RetrosFired;
	bool LowRes;

	double EmptyMass;
	double PayloadMass;
	double MainFuel;

	double THRUST_FIRST_VAC;
	double ISP_FIRST_VAC;
	double ISP_FIRST_SL;
	double CurrentThrust;

	MESHHANDLE hsat1stg1, hSat1intstg;
	THRUSTER_HANDLE th_retro[4], th_main[8];
	THGROUP_HANDLE thg_retro, thg_main;
	PROPELLANT_HANDLE ph_retro, ph_main;
	DOCKHANDLE hDockSIVB;                        // docking connector to S-IVB

	PanelSDK Panelsdk;
	SIBSystems sibsys;
	Pyro SIB_SIVB_Sep;
	Sound LaunchS, SShutS;

	SIBtoSIVBConnector sibSIVBConnector;
};
