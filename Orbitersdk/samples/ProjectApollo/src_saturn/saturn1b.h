/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  The actual Saturn1b class.

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

class SIBSystems;

///
/// \brief Saturn V launch vehicle class.
/// \ingroup Saturns
///

class Saturn1b: public Saturn {
friend class LVDC1B;
public:
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	Saturn1b (OBJHANDLE hObj, int fmodel);
	virtual ~Saturn1b();

	void initSaturn1b();

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
	int  clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPostStep (double simt, double simdt, double mjd);

	/// 
	/// \brief LVDC "Switch Selector" staging support utility function
	/// 
	void SISwitchSelector(int channel);

	void SIEDSCutoff(bool cut);
	void GetSIThrustOK(bool *ok);
	bool GetSIInboardEngineOut();
	bool GetSIOutboardEngineOut();
	bool GetSIPropellantDepletionEngineCutoff();
	bool GetSIBLowLevelSensorsDry();
	void SetSIThrusterDir(int n, double yaw, double pitch);
	double GetSIThrustLevel();

	void ActivatePrelaunchVenting();
	void DeactivatePrelaunchVenting();

	//Subsystem Access
	SIBSystems *GetSIB() { return sib; }

protected:

	VECTOR3 RelPos;

	OBJHANDLE hSoyuz;
	OBJHANDLE hAstpDM;

	void SetupMeshes();
	void SetFirstStage ();
	void SetFirstStageMeshes(double offset);
	void SetFirstStageEngines ();
	void SetSecondStage ();
	void SetSecondStageMeshes(double offset);
	void SetSecondStageEngines ();

	void ConfigureStageMeshes(int stage_state);
	void ConfigureStageEngines(int stage_state);
	void CreateStageOne();
	void CreateStageSpecificSystems();
	void SaveVehicleStats(FILEHANDLE scn);
	void LoadIU(FILEHANDLE scn);
	void LoadLVDC(FILEHANDLE scn);
	void LoadSIVB(FILEHANDLE scn);
	void SaveSI(FILEHANDLE scn);
	void LoadSI(FILEHANDLE scn);
	void SeparateStage (int stage);
	void CheckSaturnSystemsState();
	void DoFirstTimestep(double simt);
	void Timestep (double simt, double simdt, double mjd);
	void SetVehicleStats();
	void CalculateStageMass ();
	void ActivateStagingVent();
	void DeactivateStagingVent();
	void SetEngineFailure(int failstage, int faileng, double failtime, bool fail);
	void GetEngineFailure(int failstage, int faileng, bool &fail, double &failtime);

	SIBSystems *sib;

	Pyro SIBSIVBSepPyros;
};


const VECTOR3 OFS_STAGE1 = { 0, 0, -14};
const VECTOR3 OFS_STAGE12 = { 0, 0, -9.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, -24 + 1.45};
const VECTOR3 OFS_STAGE21 = { 1.85,1.85,32};
const VECTOR3 OFS_STAGE22 = { -1.85,1.85,32};
const VECTOR3 OFS_STAGE23 = { 1.85,-1.85,32};
const VECTOR3 OFS_STAGE24 = { -1.85,-1.85,32};
const VECTOR3 OFS_SM = { 0, 0, -2.05};
const VECTOR3 OFS_CM_CONE = { 0, 0, 36.05-12.25-21.5};

const double STG0O = 0;
const double STG1O = 10.25;

extern void Saturn1bLoadMeshes();
