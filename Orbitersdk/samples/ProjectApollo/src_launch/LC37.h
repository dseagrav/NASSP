/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2018

  LC37 vessel

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

#include "soundlib.h"
#include "IUUmbilicalInterface.h"
#include "SCMUmbilicalInterface.h"
#include "LCCPadInterface.h"

class S1B;
class SIVB;
class IUUmbilical;
class IU_ESE;
class SCMUmbilical;
class SIB_ESE;
class RCA110AM;

///
/// \ingroup Ground
///
class LC37: public VESSEL2, public IUUmbilicalInterface, public SCMUmbilicalInterface, public LCCPadInterface {

public:
	LC37(OBJHANDLE hObj, int fmodel);
	virtual ~LC37();

	void clbkSetClassCaps(FILEHANDLE cfg);
	void clbkPostCreation();
	void clbkLoadStateEx(FILEHANDLE scn, void *status);
	void clbkSaveState(FILEHANDLE scn);
	int clbkConsumeDirectKey(char *kstate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	void clbkPreStep(double simt, double simdt, double mjd);
	void clbkPostStep(double simt, double simdt, double mjd);

	// LC-37/IU Interface
	bool ESEGetCommandVehicleLiftoffIndicationInhibit();
	bool ESEGetExcessiveRollRateAutoAbortInhibit(int n);
	bool ESEGetExcessivePitchYawRateAutoAbortInhibit(int n);
	bool ESEGetTwoEngineOutAutoAbortInhibit(int n);
	bool ESEGetGSEOverrateSimulate(int n);
	bool ESEGetEDSPowerInhibit();
	bool ESEPadAbortRequest();
	bool ESEGetThrustOKIndicateEnableInhibitA();
	bool ESEGetThrustOKIndicateEnableInhibitB();
	bool ESEEDSLiftoffInhibitA();
	bool ESEEDSLiftoffInhibitB();
	bool ESEGetEDSAutoAbortSimulate(int n);
	bool ESEGetEDSLVCutoffSimulate(int n);
	bool ESEGetSIBurnModeSubstitute();
	bool ESEGetGuidanceReferenceRelease();
	bool ESEGetQBallSimulateCmd();

	//ML/S-IC Interface
	bool ESEGetSIBThrustOKSimulate(int eng, int n);

	// LCC/LC-37 Interface
	void SLCCCheckDiscreteInput(RCA110A *c);
	bool SLCCGetOutputSignal(size_t n);
	void ConnectGroundComputer(RCA110A *c);
	void IssueSwitchSelectorCmd(int stage, int chan);

protected:
	bool firstTimestepDone;
	bool abort;
	double touchdownPointHeight;
	char SIBName[256];
	char SIVBName[256];
	SoundLib soundlib;
	int state;
	double LaunchMJD;
	double MissionTime;

	PSTREAM_HANDLE liftoffStream[2];
	double liftoffStreamLevel;

	void DoFirstTimestep();
	void SetTouchdownPointHeight(double height);
	void DefineAnimations();

	IUUmbilical *IuUmb;
	SCMUmbilical *SCMUmb;
	IU_ESE *IuESE;
	SIB_ESE *SIBESE;
	RCA110AM *rca110a;
	S1B *s1b;
	SIVB *sivb;
};
