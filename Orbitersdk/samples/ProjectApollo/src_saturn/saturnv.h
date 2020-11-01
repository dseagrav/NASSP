/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  The actual SaturnV class.

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

class SICSystems;

///
/// \brief Saturn V launch vehicle class.
/// \ingroup Saturns
///
class SaturnV: public Saturn {

public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	SaturnV (OBJHANDLE hObj, int fmodel);
	virtual ~SaturnV();

	//
	// General functions that handle calls from Orbiter.
	//

	void Timestep(double simt, double simdt, double mjd);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPostStep (double simt, double simdt, double mjd);

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
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);

	void initSaturnV();

	// called by crawler 
	void LaunchVehicleRolloutEnd();		// after arrival on launch pad
	void LaunchVehicleBuild();			// build/unbuild during assembly
	void LaunchVehicleUnbuild();

	///
	/// \brief Set up J2 engines as fuel venting thruster.
	/// \todo Not implemented yet, see Saturn1b
	virtual void SetVentingJ2Thruster() {};

	///
	/// \brief LVDC "Switch Selector" staging support utility function
	/// 
	void SISwitchSelector(int channel);
	void SIISwitchSelector(int channel);

	void GetSIThrustOK(bool *ok);
	void SIEDSCutoff(bool cut);
	bool GetSIPropellantDepletionEngineCutoff();
	bool GetSIInboardEngineOut();
	bool GetSIOutboardEngineOut();
	void SetSIThrusterDir(int n, double yaw, double pitch);
	double GetSIThrustLevel();

	void GetSIIThrustOK(bool *ok);
	void SIIEDSCutoff(bool cut);
	bool GetSIIPropellantDepletionEngineCutoff();
	bool GetSIIEngineOut();
	void SetSIIThrusterDir(int n, double yaw, double pitch);
	double GetSIIFuelTankPressurePSI();

	SICSystems *GetSIC() { return sic; }

	//
	// Functions that external code shouldn't need to access.
	//

private:

	void ChangeSatVBuildState (int bstate);
	void SetFirstStage ();
	void SetFirstStageEngines ();
	void SetSecondStage ();
	void SetSecondStageMesh (double offset);
	void SetSecondStageEngines (double offset);
	void SetThirdStage ();
	void SetThirdStageMesh (double offset);
	void SetThirdStageEngines (double offset);
	MESHHANDLE GetInterstageMesh();
	void MoveEVA();

	void SeparateStage (int stage);
	void CheckSaturnSystemsState();

	void SetVehicleStats();

	///
	/// Calculate the mass of the various stages of the vehicle once we know
	/// what type it is and what state it's in.
	/// \brief Calculate stage masses.
	///
	void CalculateStageMass ();

	void SetupMeshes();

protected:

	//
	// Mission stage functions.
	//

	void ConfigureStageMeshes(int stage_state);
	void ConfigureStageEngines(int stage_state);
	void CreateStageOne();
	void CreateStageSpecificSystems();

	//
	// Mission-support functions.
	//

	void SaveVehicleStats(FILEHANDLE scn);
	void LoadIU(FILEHANDLE scn);
	void LoadLVDC(FILEHANDLE scn);
	void LoadSIVB(FILEHANDLE scn);
	void SaveSI(FILEHANDLE scn);
	void LoadSI(FILEHANDLE scn);
	void SaveSII(FILEHANDLE scn);
	void LoadSII(FILEHANDLE scn);

	//
	// Odds and ends.
	//

	void DoFirstTimestep(double simt);
	void ActivatePrelaunchVenting();
	void DeactivatePrelaunchVenting();
	void ActivateStagingVent();
	void DeactivateStagingVent();
	void SetEngineFailure(int failstage, int faileng, double failtime, bool fail);
	void GetEngineFailure(int failstage, int faileng, bool &fail, double &failtime);

	//
	// Class variables.
	//

	bool GoHover;

	//
	// End state.
	//

	///
	/// \brief Number of retro rockets on stage one.
	///
	int SI_RetroNum;

	///
	/// \brief Number of retro rockets on stage two.
	///
	int SII_RetroNum;

	///
	/// \brief Number of ullage rockets on stage two.
	///
	int SII_UllageNum;

	//
	// Sounds.
	//

	Sound DockS;
	Sound SpeedS;
	Sound S5P100;
	Sound SRover;
	Sound SecoSound;

	SICSystems *sic;
	SIISystems *sii;

	Pyro SICSIISepPyros;
	Pyro SIIInterstagePyros;
	Pyro SIISIVBSepPyros;

	friend class MCC;
	friend class ApolloRTCCMFD;
};

extern void LoadSat5Meshes();


const double STG0O = 20.4;
const double STG1O = -5.25;
const VECTOR3 OFS_STAGE1 =  { 0, 0, -54.0 + STG0O};
const VECTOR3 OFS_STAGE12 =  { 0, 0, -30.5 -STG1O};
const VECTOR3 OFS_STAGE2 =  { 0, 0, -17.2 - STG1O};
const VECTOR3 OFS_STAGE3 =  { 0, 0, 2. - STG2O};
const VECTOR3 OFS_STAGE31 =  { -1.48,-1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE32 =  { 1.48,-1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE33 =  { 1.48,1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_STAGE34 =  { -1.48,1.48,5.6+STG2O+21.5};
const VECTOR3 OFS_SM = {0, 0, -2.05}; 
const VECTOR3 OFS_ABORT =  { 0,0,STG0O};
const VECTOR3 OFS_ABORT2 =  { 0,0,-STG1O};
const VECTOR3 OFS_CM_CONE =  { 0, 0, 36.05-12.25-21.5};
