/****************************************************************************
This file is part of Project Apollo - NASSP

Real-Time Computer Complex (RTCC)

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
//#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include "soundlib.h"
#include "ioChannels.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "LEMcomputer.h"
#include "LEM.h"
#include "sivb.h"
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "../src_rtccmfd/TLMCC.h"
#include "../src_rtccmfd/TLIGuidanceSim.h"
#include "../src_rtccmfd/CSMLMGuidanceSim.h"
#include "../src_rtccmfd/GeneralizedIterator.h"
#include "mcc.h"
#include "rtcc.h"

// SCENARIO FILE MACROLOGY
#define SAVE_BOOL(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_INT(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_DOUBLE(KEY,VALUE) papiWriteScenario_double(scn, KEY, VALUE)
#define SAVE_V3(KEY,VALUE) papiWriteScenario_vec(scn, KEY, VALUE)
#define SAVE_M3(KEY,VALUE) papiWriteScenario_mx(scn, KEY, VALUE)
#define SAVE_STRING(KEY,VALUE) oapiWriteScenario_string(scn, KEY, VALUE)
#define LOAD_BOOL(KEY,VALUE) if(strnicmp(line, KEY, strlen(KEY)) == 0){ sscanf(line + strlen(KEY), "%i", &tmp); if (tmp == 1) { VALUE = true; } else { VALUE = false; } }
#define LOAD_INT(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%i",&VALUE); }
#define LOAD_DOUBLE(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf",&VALUE); }
#define LOAD_V3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf",&VALUE.x,&VALUE.y,&VALUE.z); }
#define LOAD_M3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&VALUE.m11,&VALUE.m12,&VALUE.m13,&VALUE.m21,&VALUE.m22,&VALUE.m23,&VALUE.m31,&VALUE.m32,&VALUE.m33); }
#define LOAD_STRING(KEY,VALUE,LEN) if(strnicmp(line,KEY,strlen(KEY))==0){ strncpy(VALUE, line + (strlen(KEY)+1), LEN); }

FIDOOrbitDigitals::FIDOOrbitDigitals()
{
	A = 0.0;
	E = 0.0;
	GAM = 0.0;
	GET = 0.0;
	GETA = 0.0;
	GETCC = 0.0;
	GETID = 0.0;
	GETL = 0.0;
	GETP = 0.0;
	GMTID = 0.0;
	H = 0.0;
	HA = 0.0;
	HP = 0.0;
	I = 0.0;
	K = 0.0;
	L = 0.0;
	LA = 0.0;
	LNPP = 0.0;
	LP = 0.0;
	LPP = 0.0;
	NV1 = 0;
	NV2 = 0;
	ORBWT = 0.0;
	PA = 0.0;
	PP = 0.0;
	PPP = 0.0;
	sprintf(REF, "");
	REV = 0;
	REVL = 0;
	REVR = 0;
	sprintf(STAID, "");
	TAPP = 0.0;
	TO = 0.0;
	V = 0.0;
	sprintf(VEHID, "");
	sprintf(REFR, "");
	GETBV = 0.0;
	HAR = 0.0;
	PAR = 0.0;
	LAR = 0.0;
	GETAR = 0.0;
	HPR = 0.0;
	PPR = 0.0;
	LPR = 0.0;
	GETPR = 0.0;
}

SpaceDigitals::SpaceDigitals()
{
	TUP = 0;
	sprintf(VecID, "");
	WEIGHT = 0.0;
	GMTV = 0.0;
	GETV = 0.0;
	GETAxis = 0.0;
	GETR = 0.0;
	GET = 0.0;
	sprintf(REF, "");
	V = 0.0;
	PHI = 0.0;
	H = 0.0;
	ADA = 0.0;
	GAM = 0.0;
	LAM = 0.0;
	PSI = 0.0;
	sprintf(VEHID, "");
	GETVector1 = 0.0;
	sprintf(REF1, "");
	WT = 0.0;
	GETA = 0.0;
	HA = 0.0;
	HP = 0.0;
	H1 = 0.0;
	V1 = 0.0;
	GAM1 = 0.0;
	PSI1 = 0.0;
	PHI1 = 0.0;
	LAM1 = 0.0;
	HS = 0.0;
	HO = 0.0;
	PHIO = 0.0;
	IEMP = 0.0;
	W1 = 0.0;
	OMG = 0.0;
	PRA = 0.0;
	A1 = 0.0;
	L1 = 0.0;
	E1 = 0.0;
	I1 = 0.0;
	GETVector2 = 0.0;
	GETSI = 0.0;
	GETCA = 0.0;
	VCA = 0.0;
	HCA = 0.0;
	PCA = 0.0;
	LCA = 0.0;
	PSICA = 0.0;
	GETMN = 0.0;
	HMN = 0.0;
	PMN = 0.0;
	LMN = 0.0;
	DMN = 0.0;
	GETVector3 = 0.0;
	GETSE = 0.0;
	GETEI = 0.0;
	VEI = 0.0;
	GEI = 0.0;
	PEI = 0.0;
	LEI = 0.0;
	PSIEI = 0.0;
	GETVP = 0.0;
	VVP = 0.0;
	HVP = 0.0;
	PVP = 0.0;
	LVP = 0.0;
	PSIVP = 0.0;
	IE = 0.0;
	LN = 0.0;
	TUN1 = TUN2 = TUN3 = 0;
}

CheckoutMonitor::CheckoutMonitor()
{
	sprintf_s(VEH, "CSM");
	GET = 0.0;
	GMT = 0.0;
	sprintf_s(VID, "");
	Pos = _V(0, 0, 0);
	Vel = _V(0, 0, 0);
	V_i = 0.0;
	gamma_i = 0.0;
	psi = 0.0;
	phi_c = 0.0;
	lambda = 0.0;
	h_s = 0.0;
	h_a = 0.0;
	h_p = 0.0;
	h_o_NM = 0.0;
	h_o_ft = 0.0;
	phi_D = 0.0;
	lambda_D = 0.0;
	a = 0.0;
	e = 0.0;
	i = 0.0;
	omega_p = 0.0;
	Omega = 0.0;
	nu = 0.0;
	m = 0.0;
	R = 0.0;
	R_Day[0] = 0;
	R_Day[1] = 0;
	R_Day[2] = 0;
	A = 0.0;
	K_Fac = 0.0;
	sprintf_s(CFG, "");
	sprintf_s(RF, "ECI");
	WT = 0.0;
	WC = 0.0;
	WL = 0.0;
	SPS = 0.0;
	RCS_C = 0.0;
	APS = 0.0;
	DPS = 0.0;
	RCS_L = 0.0;
	J2 = 0.0;
	LOC = 0.0;
	GRRC = 0.0;
	ZSC = 0.0;
	GRRS = 0.0;
	LAL = 0.0;
	LOL = 0.0;
	ZSL = 0.0;
	ZSA = 0.0;
	EPHB = 0.0;
	EPHE = 0.0;
	UpdateNo = 0;
	NV = 0;
	THT = 0.0;
	RTT = 0.0;
	LSB = 0.0;
	LLS = 0.0;
	deltaL = 0.0;
	VSF = 0.0;
	EB2 = 0.0;
	EE2 = 0.0;
	U_T = _V(-2, 0, 0); //Indicator to blank display (value < -2)
	sprintf_s(Option, "GMT");

	unit = 0;
	TABlank = false;
	LSTBlank = false;
	MABlank = false;
	HOBlank = false;
}

DetailedManeuverTable::DetailedManeuverTable()
{
	sprintf_s(C_STA_ID, "");
	C_GMTV = 0.0;
	C_GETV = 0.0;
	sprintf_s(CODE, "");
	sprintf_s(L_STA_ID, "");
	L_GMTV = 0.0;
	L_GETV = 0.0;
	sprintf_s(REF, "");
	sprintf_s(X_STA_ID, "");
	X_GMTV = 0.0;
	X_GETV = 0.0;
	GETR = 0.0;
	WT = 0.0;
	WC = 0.0;
	WL = 0.0;
	WF = 0.0;
	GETI = 0.0;
	PETI = 0.0;
	DVM = 0.0;
	DVREM = 0.0;
	DVC = 0.0;
	DT_B = 0.0;
	DT_U = 0.0;
	DT_TO = 0.0;
	DV_TO = 0.0;
	sprintf_s(REFSMMAT_Code, "");
	DEL_P = 0.0;
	DEL_Y = 0.0;
	VG = _V(0, 0, 0);
	IMUAtt = _V(0, 0, 0);
	FDAIAtt = _V(0, 0, 0);
	LVLHAtt = _V(0, 0, 0);
	VF = 0.0;
	VS = 0.0;
	VD = 0.0;
	H_BI = 0.0;
	P_BI = 0.0;
	L_BI = 0.0;
	F_BI = 0.0;
	HA = 0.0;
	HP = 0.0;
	L_AN = 0.0;
	E = 0.0;
	I = 0.0;
	WP = 0.0;
	VP = 0.0;
	THETA_P = 0.0;
	DELTA_P = 0.0;
	P_LLS = 0.0;
	L_LLS = 0.0;
	R_LLS = 0.0;
	DH = 0.0;
	PHASE = 0.0;
	PHASE_DOT = 0.0;
	WEDGE_ANG = 0.0;
	YD = 0.0;
	UntilDay = false;
	TimeUntil = 0.0;
	sprintf_s(PGNS_Veh, "");
	sprintf_s(AGS_Veh, "");
	PGNS_GETI = 0.0;
	PGNS_DV = _V(0, 0, 0);
	AGS_GETI = 0.0;
	AGS_DV = _V(0, 0, 0);
	Lam_GETI = 0.0;
	Lam_TF = 0.0;
	Lam_R = _V(0, 0, 0);
	CFP_ID = 0;
	CFP_GETI = 0.0;
	CFP_APSIS = 0;
	CFP_ELEV = 0.0;
	CFP_TPI = 0.0;
	CFP_DT = 0.0;
	sprintf_s(CFP_OPTION, "");
	isCSMTV = true;
}

TradeoffDataDisplayBuffer::TradeoffDataDisplayBuffer()
{
	curves = 0;
	XLabels[0] = 0;
	XLabels[1] = 0;
	XLabels[2] = 0;
	YLabels[0] = 0;
	YLabels[1] = 0;
	YLabels[2] = 0;
	for (int i = 0;i < 10;i++)
	{
		TZDisplay[i] = 0;
		TZxval[i] = 0.0;
		TZyval[i] = 0.0;
		NumInCurve[i] = 0;
		for (int j = 0;j < 44;j++)
		{
			xval[i][j] = 0.0;
			yval[i][j] = 0.0;
		}
	}
}

MPTManeuver::MPTManeuver()
{
	GMTFrozen = 0.0;
	AttitudeCode = 0;
	Thruster = 0;
	UllageThrusterOpt = false;
	AttitudesInput = false;
	ConfigCodeBefore = 0;
	TVC = 0;
	TrimAngleInd = 0;
	FrozenManeuverInd = false;
	CoordSysInd = 0;
	HeadsUpDownInd = false;
	DockingAngle = 0.0;
	GMTMAN = 0.0;
	dt_ullage = 0.0;
	DT_10PCT = 0.0;
	dt = 0.0;
	dv = 0.0;
	A_T = _V(0, 0, 0);
	X_B = _V(0, 0, 0);
	Y_B = _V(0, 0, 0);
	Z_B = _V(0, 0, 0);
	DPSScaleFactor = 0.0;
	dV_inertial = _V(0, 0, 0);
	dV_LVLH = _V(0, 0, 0);
	Word67d = 0.0;
	Word68 = 0.0;
	Word69 = 0.0;
	Word70 = 0.0;
	Word71 = 0.0;
	Word72 = 0.0;
	Word73 = 0.0;
	Word74 = 0.0;
	Word75 = 0.0;
	Word76 = 0.0;
	Word77 = 0.0;
	Word78d = 0.0;
	Word79 = 0.0;
	Word80 = 0.0;
	Word81 = 0.0;
	Word83 = 0.0;
	Word83 = 0.0;
	Word84 = 0.0;
	GMTI = 0.0;
	TrajDet[0] = 0;
	TrajDet[1] = 0;
	TrajDet[2] = 0;
	R_BI = _V(0, 0, 0);
	V_BI = _V(0, 0, 0);
	GMT_BI = 0.0;
	R_BO = _V(0, 0, 0);
	V_BO = _V(0, 0, 0);
	GMT_BO = 0.0;

	TotalMassAfter = 0.0;
	TotalAreaAfter = 0.0;
}

MPTManDisplay::MPTManDisplay()
{
	DELTAV = 0.0;
	DVREM = 0.0;
	HA = 0.0;
	HP = 0.0;
}

StationContact::StationContact()
{
	GMTAOS = 0.0;
	GMTLOS = 0.0;
	GMTEMAX = 0.0;
	MAXELEV = 0.0;
	BestAvailableAOS = false;
	BestAvailableLOS = false;
	BestAvailableEMAX = false;
	REV = 0;
}

bool StationContact::operator<(const StationContact& rhs) const
{
	if (GMTAOS == rhs.GMTAOS)
	{
		return GMTLOS < rhs.GMTLOS;
	}

	return GMTAOS < rhs.GMTAOS;
}

NextStationContactsTable::NextStationContactsTable()
{
	GET = 0.0;
	for (int i = 0;i < 2;i++)
	{
		for (int j = 0;j < 6;j++)
		{
			GETHCA[i][j] = 0.0;
			DTKLOS[i][j] = 0.0;
			EMAX[i][j] = 0.0;
			DTPASS[i][j] = 0.0;
			DTKH[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableEMAX[i][j] = false;
		}
	}
}

PredictedSiteAcquisitionTable::PredictedSiteAcquisitionTable()
{
	curpage = 1;
	pages = 1;
	for (int i = 0;i < 2;i++)
	{
		numcontacts[i] = 0;
		for (int j = 0;j < 21;j++)
		{
			REV[i][j] = 0;
			GETHCA[i][j] = 0.0;
			GETHCD[i][j] = 0.0;
			ELMAX[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableLOS[i][j] = false;
			BestAvailableEMAX[i][j] = false;
		}
	}
}

ExperimentalSiteAcquisitionTable::ExperimentalSiteAcquisitionTable()
{
	curpage = 1;
	pages = 1;
	for (int i = 0;i < 2;i++)
	{
		numcontacts[i] = 0;
		for (int j = 0;j < 20;j++)
		{
			REV[i][j] = 0;
			GETAOS[i][j] = 0.0;
			GETLOS[i][j] = 0.0;
			GNDRNG[i][j] = 0.0;
			ALT[i][j] = 0.0;
			GETCA[i][j] = 0.0;
			ELMAX[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableLOS[i][j] = false;
			BestAvailableEMAX[i][j] = false;
		}
	}
}

LandmarkAcquisitionTable::LandmarkAcquisitionTable()
{
	curpage = 1;
	pages = 1;
	TUN = 0;
	for (int i = 0;i < 3;i++)
	{
		numcontacts[i] = 0;
		for (int j = 0;j < 20;j++)
		{
			GETAOS[i][j] = 0.0;
			GETLOS[i][j] = 0.0;
			GETCA[i][j] = 0.0;
			GETSR[i][j] = 0.0;
			GETSS[i][j] = 0.0;
			BestAvailableAOS[i][j] = false;
			BestAvailableLOS[i][j] = false;
			Lambda[i][j] = 0.0;
			h[i][j] = 0.0;
		}
	}
}

CapeCrossingTable::CapeCrossingTable()
{
	TUP = 0;
	NumRev = 0;
	NumRevFirst = 0;
	NumRevLast = 0;
	GMTEphemFirst = 0.0;
	GMTEphemLast = 0.0;
	GMTCrossPrev = 0.0;
	for (int i = 0;i < 30;i++)
	{
		GMTCross[i] = 0.0;
	}
}

LunarLiftoffTimeOpt::LunarLiftoffTimeOpt()
{
	theta_1 = 17.0*RAD;
	dt_1 = 7.0*60.0 + 15.0;
	DH_SRCH = 15.0*1852.0;
	IsInsVelInput = false;
	v_LH = 5509.5*0.3048;
	v_LV = 19.5*0.3048;
	theta_F = 130.0*RAD;
	E = 26.6*RAD;
	I_TPI = 1;
}

LunarDescentPlanningTable::LunarDescentPlanningTable()
{
	LMWT = 0.0;
	GMTV = 0.0;
	GETV = 0.0;
	MODE = 0;
	LAT_LLS = 0.0;
	LONG_LLS = 0.0;
	for (int i = 0;i < 4;i++)
	{
		GETTH[i] = 0.0;
		GETIG[i] = 0.0;
		LIG[i] = 0.0;
		DV[i] = 0.0;
		AC[i] = 0.0;
		HPC[i] = 0.0;
		DEL[i] = 0.0;
		THPC[i] = 0.0;
		DVVector[i] = _V(0, 0, 0);
	}
	PD_ThetaIgn = 0.0;
	PD_PropRem = 0.0;
	PD_GETTH = 0.0;
	PD_GETIG = 0.0;
	PD_GETTD = 0.0;
	sprintf(DescAzMode, "");
	DescAsc = 0.0;
	SN_LK_A = 0.0;
}

RTCC::RTEConstraintsTable::RTEConstraintsTable()
{
	DVMAX = 10000.0;
	TZMIN = 0.0;
	TZMAX = 350.0;
	GMAX = 4.0;
	HMINMC = 50.0;
	IRMAX = 40.0;
	RRBIAS = 0.0;
	VRMAX = 36323.0;
	VECID = 0;
	TGTLN = 1;
	VECTYPE = 7;
	MOTION = 0;
	for (int i = 0;i < 5;i++)
	{
		PTPLatitude[i] = 0.0;
		PTPLongitude[i] = 0.0;
		for (int j = 0;j < 10;j++)
		{
			ATPCoordinates[i][j] = 1e10;
		}
	}

	//0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific

	//MPL
	ATPSite[0] = "MPL";
	ATPCoordinates[0][0] = 40.0*RAD;
	ATPCoordinates[0][1] = -175.0*RAD;
	ATPCoordinates[0][2] = 15.0*RAD;
	ATPCoordinates[0][3] = -175.0*RAD;
	ATPCoordinates[0][4] = 0.0*RAD;
	ATPCoordinates[0][5] = -165.0*RAD;
	ATPCoordinates[0][6] = -40.0*RAD;
	ATPCoordinates[0][7] = -165.0*RAD;

	//EPL
	ATPSite[1] = "EPL";
	ATPCoordinates[1][0] = 40.0*RAD;
	ATPCoordinates[1][1] = -135.0*RAD;
	ATPCoordinates[1][2] = 21.0*RAD;
	ATPCoordinates[1][3] = -122.0*RAD;
	ATPCoordinates[1][4] = -11.0*RAD;
	ATPCoordinates[1][5] = -89.0*RAD;
	ATPCoordinates[1][6] = -40.0*RAD;
	ATPCoordinates[1][7] = -83.0*RAD;

	//AOL
	ATPSite[2] = "AOL";
	ATPCoordinates[2][0] = 40.0*RAD;
	ATPCoordinates[2][1] = -30.0*RAD;
	ATPCoordinates[2][2] = 10.0*RAD;
	ATPCoordinates[2][3] = -30.0*RAD;
	ATPCoordinates[2][4] = -5.0*RAD;
	ATPCoordinates[2][5] = -25.0*RAD;
	ATPCoordinates[2][6] = -40.0*RAD;
	ATPCoordinates[2][7] = -25.0*RAD;

	//IOL
	ATPSite[3] = "IOL";
	ATPCoordinates[3][0] = 15.0*RAD;
	ATPCoordinates[3][1] = 65.0*RAD;
	ATPCoordinates[3][2] = -40.0*RAD;
	ATPCoordinates[3][3] = 65.0*RAD;

	//WPL
	ATPSite[4] = "WPL";
	ATPCoordinates[4][0] = 40.0*RAD;
	ATPCoordinates[4][1] = 150.0*RAD;
	ATPCoordinates[4][2] = 10.0*RAD;
	ATPCoordinates[4][3] = 150.0*RAD;
	ATPCoordinates[4][4] = -15.0*RAD;
	ATPCoordinates[4][5] = 170.0*RAD;
	ATPCoordinates[4][6] = -40.0*RAD;
	ATPCoordinates[4][7] = 170.0*RAD;

	sprintf_s(RTEManeuverCode, "CSU");
}

RTCC::RendezvousEvaluationDisplay::RendezvousEvaluationDisplay()
{
	ID = 0;
	M = 0;
	NumMans = 0;
	isDKI = false;
	for (int i = 0;i < 5;i++)
	{
		GET[i] = 0.0;
		if (i > 0)
		{
			DT[i - 1] = 0.0;
		}
		DV[i] = 0.0;
		CODE[i] = 0.0;
		PHASE[i] = 0.0;
		HEIGHT[i] = 0.0;
		HA[i] = 0.0;
		HP[i] = 0.0;
		Pitch[i] = 0.0;
		Yaw[i] = 0.0;
		DVVector[i] = _V(0, 0, 0);
	}
}

RTCC::RTCC()
{
	mcc = NULL;
	TimeofIgnition = 0.0;
	SplashLatitude = 0.0;
	SplashLongitude = 0.0;
	DeltaV_LVLH = _V(0.0, 0.0, 0.0);
	calcParams.EI = 0.0;
	calcParams.TEI = 0.0;
	calcParams.TLI = 0.0;
	calcParams.R_TLI = _V(0,0,0);
	calcParams.V_TLI = _V(0,0,0);
	calcParams.alt_node = 0.0;
	calcParams.GET_node = 0.0;
	calcParams.lat_node = 0.0;
	calcParams.lng_node = 0.0;
	calcParams.LOI = 0.0;
	calcParams.SEP = 0.0;
	calcParams.DOI = 0.0;
	calcParams.PDI = 0.0;
	calcParams.TLAND = 0.0;
	calcParams.LSAlt = 0.0;
	calcParams.LSAzi = 0.0;
	calcParams.LSLat = 0.0;
	calcParams.LSLng = 0.0;
	calcParams.LunarLiftoff = 0.0;
	calcParams.Insertion = 0.0;
	calcParams.Phasing = 0.0;
	calcParams.CSI = 0.0;
	calcParams.CDH = 0.0;
	calcParams.TPI = 0.0;
	calcParams.src = NULL;
	calcParams.tgt = NULL;
	REFSMMATType = 0;
	calcParams.StoredREFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	calcParams.TEPHEM = 0.0;
	calcParams.PericynthionLatitude = 0.0;
	calcParams.TIGSTORE1 = 0.0;
	calcParams.DVSTORE1 = _V(0, 0, 0);
	calcParams.SVSTORE1.gravref = NULL;
	calcParams.SVSTORE1.mass = 0.0;
	calcParams.SVSTORE1.MJD = 0.0;
	calcParams.SVSTORE1.R = _V(0, 0, 0);
	calcParams.SVSTORE1.V = _V(0, 0, 0);

	MCVLMC = 0.0;
	MCVCMC = 0.5;
	MCTCT1 = 441.5*2.0;
	MCTCT2 = 441.5*4.0;
	MCTCT3 = 441.5*2.0;
	MCTCT4 = 441.5*4.0;
	MCTCT5 = 874.37837;
	MCTCT6 = 874.37837*2.0;
	MCTCW1 = 0.31038;
	MCTCW2 = 0.62076;
	MCTCW3 = 0.31038;
	MCTCW4 = 0.62076;
	MCTLT1 = 441.5*2.0;
	MCTLT2 = 441.5*4.0;
	MCTLT3 = 441.5*2.0;
	MCTLT4 = 441.5*4.0;
	MCTLT5 = 889.6;
	MCTLT6 = 1779.2;
	MCTLW1 = 0.31038;
	MCTLW2 = 0.62076;
	MCTLW3 = 0.31038;
	MCTLW4 = 0.62076;
	MCTAT1 = 15297.43;
	MCTAT2 = 693.0329277;
	MCTAT4 = 15297.43;
	MCTAT9 = 15569.0;
	MCTAW1 = 5.049776716;
	MCTAW2 = 0.2290767466;
	MCTAW4 = 5.049776716;
	MCTAD2 = 0.308;
	MCTAD3 = 0.192;
	MCTAD9 = 0.5;
	MCTDT1 = 9712.5 / LBF;
	MCTDT2 = 1.0 / LBF;
	MCTDT3 = 467.7 / LBF;
	MCTDT4 = 1180.0 / LBF;
	MCTDT5 = 4326.5 / LBF;
	MCTDT6 = 10500.0 / LBF;
	MCTDT9 = 43670.0;
	MCTDW1 = 14.63626597;
	MCTDW2 = 0.000126;
	MCTDW3 = 0.7048;
	MCTDW4 = 1.778203048;
	MCTDW5 = 6.519823328;
	MCTDW6 = 15.82299024;
	MCTDD2 = 0.5;
	MCTDD3 = 3.1;
	MCTDD4 = 26.0;
	MCTDD5 = 0.4;
	MCTDD6 = 95.0;
	MCTDD9 = 0.5;
	MCTST1 = 91188.544;
	MCTST2 = 424.015 / LBF;
	MCTST4 = 91188.544;
	MCTST9 = 91188.544;
	MCTSW1 = 29.60667013;
	MCTSW2 = 0.6107646462;
	MCTSW4 = 29.60667013;
	MCTSD2 = 0.0;
	MCTSD3 = 0.0;
	MCTSD9 = 1.0;
	MCTAK1 = 12455.0;
	MCTAK2 = 31138.0;
	MCTAK3 = 15569.0;
	MCTAK4 = 6181.0;
	MCTDTF = 0.925;

	MCTJD1 = 570.0;
	MCTJD3 = 2.5;
	MCTJDS = 1.0;
	MCTJD5 = 99999.0;
	MCTJD6 = 1.8;

	MCTJT1 = 38.8 / LBF;
	MCTJT2 = 5889.0 / LBF;
	MCTJT3 = 110900.0 / LBF;
	MCTJT4 = 178147.0 / LBF;
	MCTJTL = 202097.0 / LBF;
	MCTJTH = 202256.0 / LBF;
	MCTJT5 = 202256.0 / LBF;
	MCTJT6 = 27784.0 / LBF;

	MCTJW1 = 0.111 / (LBS*1000.0);
	MCTJW2 = 0.75 / (LBS*1000.0);
	MCTJW3 = 157.6 / (LBS*1000.0);
	MCTJW4 = 412.167 / (LBS*1000.0);
	MCTJWL = 472.121 / (LBS*1000.0);
	MCTJWH = 472.18 / (LBS*1000.0);
	MCTJW5 = 472.18 / (LBS*1000.0);
	MCTJW6 = 61.7 / (LBS*1000.0);

	MCTSAV = 202097.0 / LBF;
	MCTWAV = 472.121 / (LBS*1000.0);

	MCVIGM = 584.0;
	MCVWMN = 0.0;
	MCVKPC = 0.0;
	MCVCPQ = 10.0;
	MCVEP1 = 1000.0;
	MCVEP2 = 30.0;
	MCVEP3 = 30.0;
	MCVEP4 = 3.0;
	MCVYMX = 45.0*RAD;
	MCVPDL = 1.0*RAD;
	MCVYDL = 1.0*RAD;
	MCVTGQ = 300.0;
	MCVRQV = -0.4;
	MCVRQT = 1.0;
	MCTIND = 0;
	MCVVX3 = 4198.1678;
	MCVWD3 = 472.159 / (LBS*1000.0);
	MCVTB2 = 1.0;

	GMTBASE = 0.0;
	MCGMTL = MGLGMT = 0.0;
	MCGECC = 0.85;
	MCGSMA = 25.0;
	MCTSPP = -2.15*RAD;
	MCTSYP = 0.95*RAD;
	MCTSPG = 0.0;
	MCTSYG = 0.0;
	MCTAPG = 0.0;
	MCTARG = 0.0;
	MCTDPG = 0.0;
	MCTDRG = 0.0;
	MCGREF = 0.0;
	MCEBAS = 1.0;
	MCEASQ = 4.0619437e13;
	MCEBSQ = 4.0619437e13;
	MCSMLR = OrbMech::R_Moon;
	//LC-39A
	MCLSDA = sin(28.608202*RAD);
	MCLCDA = cos(28.608202*RAD);
	MCLGRA = -80.604133*RAD;
	//Time from launch to EOI, seconds
	MDLIEV[0] = 0.76673814e3;
	MDLIEV[1] = -0.18916781e1;
	MDLIEV[2] = 0.10202785e-1;
	MDLIEV[3] = 0.16441395e-5;
	//Declination of EOI, deg
	MDLIEV[4] = 0.60994221e2;
	MDLIEV[5] = -0.36710496e0;
	MDLIEV[6] = -0.68781953e-3;
	MDLIEV[7] = 0.44783353e-5;
	//Longitude of EOI, deg
	MDLIEV[8] = -0.7232328e2;
	MDLIEV[9] = 0.55465072e0;
	MDLIEV[10] = -0.48610649e-2;
	MDLIEV[11] = 0.90988410e-5;
	//Inertial azimuth of EOI, deg
	MDLIEV[12] = 0.12344699e2;
	MDLIEV[13] = 0.13104156e1;
	MDLIEV[14] = -0.46697509e-2;
	MDLIEV[15] = 0.14584433e-4;
	//Radius of EOI in NM
	MDLEIC[0] = 3.5439336e3;
	//Flight path angle of EOI in deg
	MDLEIC[1] = 0.0;
	//Inertial velocity of EI, fps
	MDLEIC[2] = 25567.72868;

	MCCCEX = 3404;
	MCCLEX = 3433;
	MCCCRF = 1735;
	MCCCRF_DL = 01735;
	MCCCXS = 306;
	MCCLRF = 1733;
	MCCLRF_DL = 01733;
	MCCLXS = 3606;

	MGRTAG = 1;

	EZJGMTX1.data[RTCC_REFSMMAT_TYPE_CUR - 1].REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	EZJGMTX3.data[RTCC_REFSMMAT_TYPE_CUR - 1].REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);
	EZJGMTX3.data[RTCC_REFSMMAT_TYPE_AGS - 1].REFSMMAT = _M(1, 0, 0, 0, 1, 0, 0, 0, 1);

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	//CG tables
	LMDSCCGTAB.Weight[0] = 6350.288000;
	LMDSCCGTAB.CG[0] = _V(5.657285, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[1] = 6577.084000;
	LMDSCCGTAB.CG[1] = _V(5.602462, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[2] = 6803.880000;
	LMDSCCGTAB.CG[2] = _V(5.551295, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[3] = 7030.676000;
	LMDSCCGTAB.CG[3] = _V(5.503428, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[4] = 7257.472000;
	LMDSCCGTAB.CG[4] = _V(5.458553, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[5] = 7484.268000;
	LMDSCCGTAB.CG[5] = _V(5.416398, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[6] = 7711.064000;
	LMDSCCGTAB.CG[6] = _V(5.376722, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[7] = 7937.860000;
	LMDSCCGTAB.CG[7] = _V(5.339314, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[8] = 8164.656000;
	LMDSCCGTAB.CG[8] = _V(5.303984, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[9] = 8391.452000;
	LMDSCCGTAB.CG[9] = _V(5.270563, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[10] = 8618.248000;
	LMDSCCGTAB.CG[10] = _V(5.238902, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[11] = 8845.044000;
	LMDSCCGTAB.CG[11] = _V(5.208864, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[12] = 9071.840000;
	LMDSCCGTAB.CG[12] = _V(5.180328, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[13] = 9298.636000;
	LMDSCCGTAB.CG[13] = _V(5.153184, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[14] = 9525.432000;
	LMDSCCGTAB.CG[14] = _V(5.127333, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[15] = 9752.228000;
	LMDSCCGTAB.CG[15] = _V(5.102684, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[16] = 9979.024000;
	LMDSCCGTAB.CG[16] = _V(5.079156, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[17] = 10205.820000;
	LMDSCCGTAB.CG[17] = _V(5.056673, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[18] = 10432.616000;
	LMDSCCGTAB.CG[18] = _V(5.035167, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[19] = 10659.412000;
	LMDSCCGTAB.CG[19] = _V(5.014577, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[20] = 10886.208000;
	LMDSCCGTAB.CG[20] = _V(4.994845, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[21] = 11113.004000;
	LMDSCCGTAB.CG[21] = _V(4.975918, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[22] = 11339.800000;
	LMDSCCGTAB.CG[22] = _V(4.957748, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[23] = 11566.596000;
	LMDSCCGTAB.CG[23] = _V(4.940291, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[24] = 11793.392000;
	LMDSCCGTAB.CG[24] = _V(4.923505, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[25] = 12020.188000;
	LMDSCCGTAB.CG[25] = _V(4.907353, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[26] = 12246.984000;
	LMDSCCGTAB.CG[26] = _V(4.891799, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[27] = 12473.780000;
	LMDSCCGTAB.CG[27] = _V(4.876810, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[28] = 12700.576000;
	LMDSCCGTAB.CG[28] = _V(4.862357, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[29] = 12927.372000;
	LMDSCCGTAB.CG[29] = _V(4.848411, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[30] = 13154.168000;
	LMDSCCGTAB.CG[30] = _V(4.834946, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[31] = 13380.964000;
	LMDSCCGTAB.CG[31] = _V(4.821937, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[32] = 13607.760000;
	LMDSCCGTAB.CG[32] = _V(4.809362, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[33] = 13834.556000;
	LMDSCCGTAB.CG[33] = _V(4.797199, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[34] = 14061.352000;
	LMDSCCGTAB.CG[34] = _V(4.785429, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[35] = 14288.148000;
	LMDSCCGTAB.CG[35] = _V(4.774032, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[36] = 14514.944000;
	LMDSCCGTAB.CG[36] = _V(4.762991, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[37] = 14741.740000;
	LMDSCCGTAB.CG[37] = _V(4.752290, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[38] = 14968.536000;
	LMDSCCGTAB.CG[38] = _V(4.741913, 0.000000, 0.000000);
	LMDSCCGTAB.Weight[39] = 15195.332000;
	LMDSCCGTAB.CG[39] = _V(4.731846, 0.000000, 0.000000);
	LMDSCCGTAB.N = 40;
}

RTCC::~RTCC()
{
}

void RTCC::Init(MCC *ptr)
{
	mcc = ptr;
}

void RTCC::AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad)
{
	EarthEntryOpt entopt;
	EntryResults res;
	double v_e, m1, Vc;

	char weather[] = "GOOD";

	v_e = calcParams.src->GetThrusterIsp0(calcParams.src->GetGroupThruster(THGROUP_MAIN, 0));

	entopt.vessel = calcParams.src;
	entopt.GETbase = getGETBase();
	entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
	entopt.nominal = RTCC_ENTRY_NOMINAL;
	entopt.ReA = 0;
	entopt.entrylongmanual = true;

	for (int i = 0;i < 8;i++)
	{
		pad.Area[i][0] = 0;
		pad.Wx[i][0] = 0;
	}

	for (int i = 0;i < opt->n;i++)
	{
		entopt.lng = opt->lng[i];
		entopt.TIGguess = opt->GETI[i];

		BlockDataProcessor(&entopt, &res);

		m1 = calcParams.src->GetMass()*exp(-length(res.dV_LVLH) / v_e);
		Vc = length(res.dV_LVLH)*cos(-2.15*RAD)*cos(0.95*RAD);// -60832.18 / m1;

		sprintf(pad.Area[i], opt->area[i].c_str());
		sprintf(pad.Wx[i], weather);

		pad.dVC[i] = Vc/0.3048;
		pad.GETI[i] = res.P30TIG;
		pad.Lat[i] = res.latitude*DEG;
		pad.Lng[i] = res.longitude*DEG;
	}
}

void RTCC::AP11BlockData(AP11BLKOpt *opt, P37PAD &pad)
{
	EntryOpt entopt;
	EntryResults res;

	entopt.entrylongmanual = true;
	entopt.GETbase = getGETBase();
	entopt.enginetype = RTCC_ENGINETYPE_CSMSPS;
	entopt.ReA = 0;
	entopt.type = RTCC_ENTRY_ABORT;
	entopt.vessel = calcParams.src;

	if (opt->useSV)
	{
		entopt.RV_MCC = opt->RV_MCC;
	}
	else
	{
		entopt.RV_MCC = StateVectorCalc(calcParams.src);
	}

	for (int i = 0;i < opt->n;i++)
	{
		entopt.TIGguess = opt->GETI[i];
		entopt.lng = opt->lng[i];

		EntryTargeting(&entopt, &res);

		pad.dVT[i] = length(res.dV_LVLH) / 0.3048;
		pad.GET400K[i] = res.GET05G;
		pad.GETI[i] = opt->GETI[i];
		pad.lng[i] = opt->lng[i] * DEG;
	}
}

void RTCC::BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res)
{
	EarthEntry* entry;
	double GET;
	bool stop;
	SV sv;

	stop = false;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}
	GET = (sv.MJD - opt->GETbase)*24.0*3600.0;

	entry = new EarthEntry(sv.R, sv.V, sv.MJD, sv.gravref, opt->GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->nominal, opt->entrylongmanual);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = entry->EntryTIGcor;
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = entry->t2;
	res->GET05G = entry->EntryRET;
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	double TIG_imp;
	VECTOR3 DV_imp;

	TIG_imp = res->P30TIG;
	DV_imp = res->dV_LVLH;

	PoweredFlightProcessor(sv, opt->GETbase, TIG_imp, opt->enginetype, 0.0, DV_imp, true, res->P30TIG, res->dV_LVLH, res->sv_preburn, res->sv_postburn);

	RZRFTT.GMTI_Primary = GMTfromGET(res->P30TIG);
	RZRFTT.DeltaV_Primary = res->dV_LVLH;
}

void RTCC::EntryTargeting(EntryOpt *opt, EntryResults *res)
{
	RTEEarth* entry;
	double GET;
	bool stop;
	SV sv;

	stop = false;

	sv = opt->RV_MCC;
	GET = (sv.MJD - opt->GETbase)*24.0*3600.0;

	entry = new RTEEarth(sv.R, sv.V, sv.MJD, sv.gravref, opt->GETbase, opt->TIGguess, opt->ReA, opt->lng, opt->type, opt->entrylongmanual, opt->r_rbias, opt->dv_max);

	while (!stop)
	{
		stop = entry->EntryIter();
	}

	res->dV_LVLH = entry->Entry_DV;
	res->P30TIG = entry->EntryTIGcor;
	res->latitude = entry->EntryLatcor;
	res->longitude = entry->EntryLngcor;
	res->GET400K = entry->t2;
	res->GET05G = entry->EntryRET;
	res->RTGO = entry->EntryRTGO;
	res->VIO = entry->EntryVIO;
	res->ReA = entry->EntryAng;
	res->precision = entry->precision;

	delete entry;

	double TIG_imp, LMmass;
	VECTOR3 DV_imp;

	TIG_imp = res->P30TIG;
	DV_imp = res->dV_LVLH;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	PoweredFlightProcessor(sv, opt->GETbase, TIG_imp, opt->enginetype, LMmass, DV_imp, true, res->P30TIG, res->dV_LVLH, res->sv_preburn, res->sv_postburn);

	PZREAP.RTEPrimaryData.IgnitionGET = res->P30TIG;
	PZREAP.RTEPrimaryData.IgnitionGMT = GMTfromGET(res->P30TIG);
	PZREAP.RTEPrimaryData.UplinkDV = res->dV_LVLH;
	PZREAP.RTEPrimaryData.ThrusterCode = opt->enginetype;
}

int RTCC::PCZYCF(double R1, double R2, double PHIT, double DELT, double VXI2, double VYI2, double VXF1, double VYF1, double SQRMU, int NREVS, int body, double &a, double &e, double &f_T, double &t_PT)
{
	double S, P, Q, a_lim, delta_lim, SIGN, dt_lim, S_L, S_P, S_Q, eps, delta, TERM, P_P, a_i, AFAIL, da, da_i, a_N, C_N, P_T, sin_f_T, cos_f_T;
	double eps_N, delta_N, C_ep, C_e, C_f, C_em, C_fm, C_fp, a_Ni, da_N, dt_N, da_Ni, a_apo, e_apo, f_T_apo, t_pt_apo, V_T, gamma_T, l_R, f_R, gamma_R, V_R, DV_T, DV_T_apo;
	int i, k, i_N, L;

	if (body == BODY_EARTH)
	{
		AFAIL = 1.02*OrbMech::R_Earth;
	}
	else
	{
		AFAIL = OrbMech::R_Moon;
	}

	S = sqrt(R1*R1 + R2 * R2 - 2.0*R1*R2*cos(PHIT));
	P = R1 + R2 + S;
	Q = R1 + R2 - S;
	a_lim = P / 4.0;
	delta_lim = 2.0*atan(sqrt(Q / (P - Q)));
	i = 0;
	k = 0;
	SIGN = sign(PHIT - PI);
	if (NREVS > 0)
	{
		goto RTCC_PCZYCF_4_1;
	}
	if (DELT <= 1.0 / (6.0*SQRMU)*(pow(P, 1.5) + SIGN * pow(Q, 1.5)))
	{
		return -1;
	}
	//Set semi-major axis to start iteration
	a = a_lim + 10000.0*0.3048;
	dt_lim = pow(a_lim, 1.5) / SQRMU * (PI + SIGN * (delta_lim - sin(delta_lim)));
	if (DELT > dt_lim)
	{
		S_L = SIGN;
	}
	else
	{
		S_L = -SIGN;
	}
RTCC_PCZYCF_2_1:
	i++;
	if (i > 15)
	{
		a = a_i;
	}
	S_P = sqrt(P / (4.0*a - P));
	S_Q = sqrt(Q / (4.0*a - Q));
	eps = 2.0*atan(S_P);
	delta = 2.0*atan(S_Q);
	if (i > 15)
	{
		goto RTCC_PCZYCF_7_1;
	}
	if (i > 1)
	{
		goto RTCC_PCZYCF_3_1;
	}
	if (DELT <= dt_lim || k > 0)
	{
		SIGN = 1.0;
		TERM = PI2 * (double)NREVS;
	}
	else
	{
		SIGN = -1.0;
		TERM = PI2 * (double)(NREVS + 1);
	}
RTCC_PCZYCF_3_1:
	P_P = TERM + SIGN * (eps - sin(eps) - S_L * (delta - sin(delta)));
	a_i = a - (SQRMU*DELT - pow(a, 1.5)*P_P) / (pow(a, 0.5)*(SIGN*((1.0 - cos(eps))*S_P - S_L * (1.0 - cos(delta))*S_Q) - 1.5*P_P));
	//Computed value large enough?
	if (a_i <= a_lim)
	{
		if (a_lim > AFAIL)
		{
			return -1;
		}
		a_i = AFAIL;
	}
	da = abs(a_i - a);
	a = a_i;
	if (50.0*0.3048 > da)
	{
		goto RTCC_PCZYCF_7_1;
	}
	if (i > 1)
	{
		if (da_i <= da)
		{
			goto RTCC_PCZYCF_2_1;
		}
		da_i = da;
		a_i = a;
	}
	else
	{
		if (a > a_lim)
		{
			da_i = da;
			a_i = a;
		}
		else
		{
			a = a_lim + 100.0*0.3048;
			da_i = da;
		}
	}
	goto RTCC_PCZYCF_2_1;
RTCC_PCZYCF_4_1:
	//Set semi-major axis to start multiple rev iteration. Initialize iteration counter
	a_N = a_lim + 50000.0*0.3048;
	i_N = 0;
	C_N = 6.0*PI*(double)(NREVS);
RTCC_PCZYCF_4_2:
	i_N++;
	if (i_N > 25)
	{
		a_N = a_Ni;
		goto RTCC_PCZYCF_5_2;
	}
	eps_N = 2.0*atan(sqrt(P / (4.0*a_N - P)));
	delta_N = 2.0*atan(sqrt(Q / (4.0*a_N - Q)));
	C_e = cos(eps_N);
	C_f = cos(delta_N);
	C_em = 1.0 - C_e;
	C_fm = 1.0 - C_f;
	C_fp = 1.0 + C_f;
	C_ep = 1.0 + C_e;
	a_Ni = a_N - (C_N + 3.0*(eps_N - SIGN * delta_N) - (5.0 + C_e)*sqrt(C_em / C_ep) - SIGN * (5.0 + C_f)*sqrt(C_fm / C_fp)) / (2.0*(pow(C_em, 3.5) / (P*pow(C_ep, 1.5)) + SIGN * pow(C_fm, 3.5) / Q / pow(C_fp, 1.5)));
	da_N = abs(a_Ni - a_N);
	a_N = a_Ni;
	if (da_N <= 10.0*0.3048)
	{
		goto RTCC_PCZYCF_5_2;
	}
	if (i_N > 1)
	{
		if (da_Ni <= da_N)
		{
			goto RTCC_PCZYCF_4_2;
		}
		da_Ni = da_N;
		a_Ni = a_N;
	}
	else
	{
		if (a_N > a_lim)
		{
			da_Ni = da_N;
			a_Ni = a_N;
		}
		else
		{
			a_N = a_lim + 50.0*0.3048;
			da_Ni = da_N;
		}
	}
	goto RTCC_PCZYCF_4_2;
RTCC_PCZYCF_5_2:
	eps_N = 2.0*atan(sqrt(P / (4.0*a_N - P)));
	delta_N = 2.0*atan(sqrt(Q / (4.0*a_N - Q)));
	dt_N = pow(a_N, 1.5) / SQRMU * (PI2*(double)(NREVS)+eps_N-sin(eps_N)+SIGN*(delta_N-sin(delta_N)));
	if (DELT <= dt_N)
	{
		return -1;
	}
	dt_lim = pow(a_lim, 1.5) / SQRMU * (PI*(double)(2 * NREVS + 1) + SIGN * (delta_lim - sin(delta_lim)));
	if (SIGN > 0)
	{
		if (DELT > dt_lim)
		{
			L = 12;
			S_L = 1.0;
		}
		else
		{
			L = 10;
			S_L = -1.0;
		}
	}
	else
	{
		if (DELT > dt_lim)
		{
			L = 11;
			S_L = -1.0;
		}
		else
		{
			L = 9;
			S_L = 1.0;
		}
	}
	a = a_lim + 10000.0*0.3048;
	goto RTCC_PCZYCF_2_1;
RTCC_PCZYCF_7_1:
	e = sqrt(pow((R1 - a - (R2 - a)*cos(eps - delta)) / sin(eps - delta), 2) + pow(a - R2, 2)) / a;
	P_T = a * (1.0 - e * e);
	cos_f_T = (P_T - R2) / e / R2;
	sin_f_T = (cos_f_T*cos(PHIT) - (P_T - R1) / e / R1) / sin(PHIT);
	f_T = atan2(sin_f_T, cos_f_T);
	if (f_T < 0)
	{
		f_T += PI2;
	}
	t_PT = a * sqrt(P_T) / SQRMU * (2.0 / sqrt(1.0 - e * e)*atan(sqrt((1.0 - e) / (1.0 + e))*tan(f_T / 2.0)) - e * sin_f_T / (1.0 + e * cos_f_T));
	if (t_PT < 0)
	{
		t_PT += PI2 * pow(a, 1.5) / SQRMU;
	}
	if (NREVS <= 0)
	{
		return 0;
	}
	V_T = SQRMU * sqrt(2.0 / R1 - 1.0 / a);
	gamma_T = atan(e*sin_f_T / (1.0*e*cos_f_T));
	l_R = pow(t_PT + DELT, -1.5)*SQRMU;
	f_R = OrbMech::MeanToTrueAnomaly(l_R, e);
	gamma_R = atan(e*sin(f_R)/(1.0*e*cos(f_R)));
	V_R = SQRMU * sqrt(2.0 / R1 - 1.0 / a);
	DV_T = sqrt(pow(V_T*cos(gamma_T) - VXI2, 2) + pow(V_T*sin(gamma_T) - VYI2, 2)) + sqrt(pow(VXF1 - V_R * cos(gamma_R), 2) + pow(VYF1 - V_R * sin(gamma_R), 2));
	if (k <= 0)
	{
		a_apo = a;
		e_apo = e;
		f_T_apo = f_T;
		t_pt_apo = t_PT;
		DV_T_apo = DV_T;
		k = 1;
		i = 0;
		if (L == 10 || L == 12)
		{
			S_L = -1.0;
		}
		else
		{
			S_L = 1.0;
		}
		a = a_N + 10000.0*0.3048;
		goto RTCC_PCZYCF_2_1;
	}
	//Output lower DV solution
	if (DV_T_apo < DV_T)
	{
		a = a_apo;
		e = e_apo;
		f_T = f_T_apo;
		t_PT = t_pt_apo;
	}
	return 0;
}

void POSVEL(double A, double B, double C, double D, double *E, double SQRMU)
{
	double gamma;
	if (E[0])
	{
		//Compute velocity
		double a = A;
		double e = B;
		double f = C;
		double R = D;
		gamma = atan(e*sin(f) / (1.0 + e * cos(f)));
		double V = SQRMU * sqrt(2.0 / R - 1.0 / a);
		E[0] = V * cos(gamma);
		E[1] = V * sin(gamma);
	}
	else
	{
		//Compute position
		double i = A;
		double h = B;
		double u = C;
		double R = D;
		gamma = sin(u);
		double alpha = gamma * cos(i);
		double beta = cos(u);
		double eta = cos(h);
		double psi = sin(h);
		E[0] = R * (beta*eta - alpha * psi);
		E[1] = R * (beta*psi + alpha * eta);
		E[2] = R * (gamma*psi);
	}
}

int RTCC::PMMTIS(EphemerisData sv_A1, EphemerisData sv_P1, double dt, double DH, double theta, EphemerisData &sv_A1_apo, EphemerisData &sv_A2, EphemerisData &sv_A2_apo)
{
	EphemerisData sv_P2;
	VECTOR3 RP2off, VP2off, VA1_apo, DR, R3;
	CELEMENTS elem_CE, elem_T;
	double WT3, theta_0, mu, u_CE, R_CE, u_T, R_T, f_T, f_CE, f3, RPLIM;
	CELEMENTS elem_C;
	//VECTOR3 R_i;
	//double sin_u_C, TEMP1, TEMP2, DEN, sin_u3, r3, u3, SIGN, T_PC, sin_h_C, cos_h_C, E[3], f_C, V_Xi, V_Yi, V_Xf, V_Yf, u_C, R_C;
	int N, IC;
	bool prograde;

	if (sv_A1.RBI == BODY_EARTH)
	{
		prograde = true;
		mu = OrbMech::mu_Earth;
		//SIGN = 1.0;
		RPLIM = OrbMech::R_Earth + 10.0*1852.0;
	}
	else
	{
		prograde = false;
		mu = OrbMech::mu_Moon;
		//SIGN = -1.0;
		RPLIM = MCSMLR;
	}

	IC = 0;
	/*R_i = sv_A1.R;
	sv_A1_apo = sv_A1;

	//Calculate chaser orbital elements
	elem_C = OrbMech::GIMIKC(sv_A1.R, sv_A1.V, mu);
	f_C = OrbMech::MeanToTrueAnomaly(elem_C.l, elem_C.e);
	u_C = elem_C.g + f_C;
	if (u_C >= PI2)
	{
		u_C -= PI2;
	}
	R_C = length(R_i);
	//Calculate target orbital elements
	elem_T = OrbMech::GIMIKC(sv_P1.R, sv_P1.V, mu);
	f_T = OrbMech::MeanToTrueAnomaly(elem_T.l, elem_T.e);
	u_T = elem_T.g + f_T;
	if (u_T >= PI2)
	{
		u_T -= PI2;
	}
	R_T = length(sv_P1.R);*/

	f_T = 0.0;
	theta_0 = OrbMech::PHSANG(sv_P1.R, sv_P1.V, sv_A1.R);
	f3 = f_T + dt * PI2 / OrbMech::period(sv_P1.R, sv_P1.V, mu);
	//r3 = elem_T.a*(1.0 - elem_T.e*elem_T.e) / (1.0 + elem_T.e*cos(f3)) - DH;
	WT3 = f3 - f_T + theta_0 - theta;
	/*u3 = u_C + WT3;
	while (u3 >= PI2)
	{
		u3 -= PI2;
	}
	while (u3 < 0)
	{
		u3 += PI2;
	}

	E[0] = 0.0;
	POSVEL(elem_T.i, elem_T.h, u3, r3, E, sqrt(mu));
	R3 = _V(E[0], E[1], E[2]);*/

	N = (int)(WT3 / PI2);
	sv_P2 = coast(sv_P1, dt);

	//Calculate target orbital elements at final time
	elem_T = OrbMech::GIMIKC(sv_P2.R, sv_P2.V, mu);
	f_T = OrbMech::MeanToTrueAnomaly(elem_T.l, elem_T.e);
	u_T = elem_T.g + f_T;
	if (u_T >= PI2)
	{
		u_T -= PI2;
	}
	R_T = length(sv_P2.R);

	if (theta != 0 || DH != 0)
	{
		elem_CE.a = elem_T.a - DH;
		elem_CE.e = elem_T.e*elem_T.a / elem_CE.a;
		f_CE = f_T - theta;
		if (f_CE >= PI2)
		{
			f_CE -= PI2;
		}
		if (f_CE < 0)
		{
			f_CE += PI2;
		}
		u_CE = u_T - theta;
		if (u_CE >= PI2)
		{
			u_CE -= PI2;
		}
		if (u_CE < 0)
		{
			u_CE += PI2;
		}
		R_CE = elem_CE.a*(1.0 - elem_CE.e*elem_CE.e) / (1.0 + elem_CE.e*cos(f_CE));
		elem_CE.l = OrbMech::TrueToMeanAnomaly(f_CE, elem_CE.e);
	}
	else
	{
		elem_CE.a = elem_T.a;
		elem_CE.e = elem_T.e;
		f_CE = f_T;
		u_CE = u_T;
		R_CE = R_T;
		elem_CE.l = elem_T.l;
	}

	/*if (N != 0)
	{
		E[0] = 1.0;
		POSVEL(elem_C.a, elem_C.e, f_C, R_C, E, sqrt(mu));
		V_Xi = E[0];
		V_Yi = E[1];
		E[0] = 1.0;
		POSVEL(elem_CE.a, elem_CE.e, f_CE, R_CE, E, sqrt(mu));
		V_Xf = E[0];
		V_Yf = E[1];
	}
	else
	{
		V_Xi = 0.0;
		V_Yi = 0.0;
		V_Xf = 0.0;
		V_Yf = 0.0;
	}*/

	elem_CE.i = elem_T.i;
	elem_CE.g = elem_T.g;
	elem_CE.h = elem_T.h;
	OrbMech::GIMKIC(elem_CE, mu, RP2off, VP2off);
/*RTCC_PMMTIS_3_3:
	WT3 = acos(dotp(unit(R_i), unit(R3)));
	if (SIGN*(R_i.x*R3.y- R_i.y*R3.x) < 0)
	{
		WT3 = PI2 - WT3;
	}

	PCZYCF(R_C, R_CE, WT3, dt, V_Xi, V_Yi, V_Xf, V_Yf, sqrt(mu), N, sv_A1.RBI, elem_C.a, elem_C.e, f_C, T_PC);

	//Periapsis limit
	if (elem_C.a*(1.0 - elem_C.e) < RPLIM)
	{
		return -1;
	}
	u_C = atan(sin(WT3) / (R3.z*length(R_i) / (R_i.z*length(R3)) - cos(WT3)));
	if (u_C > 0)
	{
		if (R_i.z < 0)
		{
			u_C += PI;
		}
	}
	else if (u_C < 0)
	{
		if (R_i.z > 0)
		{
			u_C += PI;
		}
	}
	u3 = u_C + WT3;
	sin_u_C = sin(u_C);
	elem_C.i = atan(abs(R_i.z) / sqrt(pow(length(R_i)*sin_u_C, 2) - R_i.z*R_i.z));
	sin_u3 = sin(u3);
	TEMP1 = length(R3) / sin_u_C;
	TEMP2 = length(R_i) / sin_u_C;
	DEN = cos(u3)*sin_u_C - cos(u_C)*sin_u3;
	sin_h_C = (R3.y / TEMP1 - R_i.y / TEMP2) / DEN;
	cos_h_C = (R3.x / TEMP1 - R_i.x / TEMP2) / DEN;
	elem_C.h = atan2(sin_h_C, cos_h_C);
	if (elem_C.h < 0)
	{
		elem_C.h += PI2;
	}
	elem_C.l = T_PC * pow(elem_C.a, -1.5)*sqrt(mu);
	if (elem_C.l < 0)
	{
		elem_C.l += PI2;
	}
	elem_C.g = u_C - f_C;
	if (elem_C.g < 0)
	{
		elem_C.g += PI2;
	}
	OrbMech::GIMKIC(elem_C, mu, sv_A1_apo.R, sv_A1_apo.V);
	sv_A2 = coast(sv_A1_apo, dt);
	DR = sv_A2.R - RP2off;
	if (length(DR) < 100.0*0.3048)
	{
		//sv_A2_apo
		return 0;
	}
	IC++;
	if (IC >= 15)
	{
		return -1;
	}
	R3 = R3 - DR;
	r3 = length(R3);
	goto RTCC_PMMTIS_3_3;*/

	sv_A1_apo = sv_A1;
	R3 = RP2off;
	do
	{
		VA1_apo = OrbMech::elegant_lambert(sv_A1.R, sv_A1.V, R3, dt, N, prograde, mu);
		if (length(VA1_apo) == 0.0)
		{
			return 1;
		}
		sv_A1_apo.V = VA1_apo;
		elem_C = OrbMech::GIMIKC(sv_A1_apo.R, sv_A1_apo.V, mu);
		if (elem_C.a*(1.0 - elem_C.e) < RPLIM)
		{
			return 1;
		}
		sv_A2 = coast(sv_A1_apo, dt);
		DR = sv_A2.R - RP2off;
		R3 = R3 - DR;
		IC++;
		if (IC > 15)
		{
			return 1;
		}
	} while (length(DR) > 100.0*0.3048);

	//VA1_apo = OrbMech::Vinti(sv_A1.R, sv_A1.V, RP2off, OrbMech::MJDfromGET(sv_A1.GMT, GMTBASE), dt, N, prograde, sv_A1.RBI, sv_A1.RBI, sv_A1.RBI, _V(0.0, 0.0, 0.0), 100.0*0.3048); //Vinti Targeting: For non-spherical gravity

	sv_A1_apo = sv_A1;
	sv_A1_apo.V = VA1_apo;
	sv_A2 = coast(sv_A1_apo, dt);
	sv_A2_apo = sv_A2;
	sv_A2_apo.V = VP2off;
	return 0;
}

int RTCC::PMSTICN_ELEV(EphemerisData sv_A1, EphemerisData sv_P1, double phi_req, double mu, double &T_ELEV)
{
	EphemerisData sv_A, sv_P, SV_store[2];
	CELEMENTS elem_C;
	double T_S, T, u, X_R_L, Y_R_L, R_C, eps_phi, phi, X_S, Y_S, A, B, C, D;
	int IC;

	eps_phi = 0.001*RAD;
	IC = 0;
	T_S = 0.0;
	T = sv_A1.GMT;
	sv_A = sv_A1;
	sv_P = sv_P1;
PMSTICN_ELEV_18_3:
	SV_store[0] = sv_A;
	SV_store[1] = sv_P;
PMSTICN_ELEV_18_4:
	sv_A = coast(SV_store[0], T - SV_store[0].GMT);
	sv_P = coast(SV_store[1], T - SV_store[1].GMT);
	//TBD: Error
	elem_C = OrbMech::GIMIKC(sv_A.R, sv_A.V, mu);
	u = elem_C.g + OrbMech::MeanToTrueAnomaly(elem_C.l, elem_C.e);
	R_C = length(sv_A.R);
	X_R_L = sv_P.R.x*(-sin(u)*cos(elem_C.h) - sin(elem_C.h)*cos(u)*cos(elem_C.i)) + sv_P.R.y*(-sin(u)*sin(elem_C.h) + cos(elem_C.h)*cos(u)*cos(elem_C.i)) + sv_P.R.z*(cos(u)*sin(elem_C.i));
	Y_R_L = R_C - (sv_P.R.x*(cos(u)*cos(elem_C.h) - sin(u)*cos(elem_C.i)*sin(elem_C.h)) + sv_P.R.y*(cos(u)*sin(elem_C.h) + sin(u)*cos(elem_C.i)*cos(elem_C.h)) + sv_P.R.z*(sin(u)*sin(elem_C.i)));
	phi = atan2(-Y_R_L, X_R_L);
	if (abs(phi - phi_req) <= eps_phi)
	{
		goto PMSTICN_ELEV_21_1;
	}
	if (T_S == 0.0)
	{
		X_S = X_R_L;
		Y_S = Y_R_L;
		T_S = T;
		T = T + 100.0;
		goto PMSTICN_ELEV_18_3;
	}
	else
	{
		A = (X_R_L - X_S) / (T - T_S);
		B = (X_R_L*T_S - X_S * T) / (T_S - T);
		C = (Y_R_L - Y_S) / (T - T_S);
		D = (Y_R_L*T_S - Y_S * T) / (T_S - T);
		X_S = X_R_L;
		Y_S = Y_R_L;
		T_S = T;
		T = (-B * sin(phi_req) - D * cos(phi_req)) / (A*sin(phi_req) + C * cos(phi_req));
		IC++;
		if (IC > 10)
		{
			T_ELEV = sv_A1.GMT;
			return 1;
		}
		goto PMSTICN_ELEV_18_4;
	}
PMSTICN_ELEV_21_1:
	T_ELEV = T;
	return 0;
}

void PMSTICN_PY(VECTOR3 R_A, VECTOR3 V_A, VECTOR3 R_B, VECTOR3 V_B, double &Pitch, double &Yaw)
{
	VECTOR3 H_A, H_B;
	double r_B, sin_delta, r_A_dot, r_B_dot, dr_dot, DV, h_A, h_B, VH_A, VH_B, DV_H;

	H_A = crossp(R_A, V_A);
	h_A = length(H_A);
	H_A = unit(H_A);
	H_B = crossp(R_B, V_B);
	h_B = length(H_B);
	H_B = unit(H_B);
	r_B = length(R_B);
	sin_delta = dotp(crossp(H_A, H_B), R_B) / r_B;
	r_B_dot = dotp(R_B, V_B) / r_B;
	r_A_dot = dotp(R_A, V_A) / r_B;
	dr_dot = r_A_dot - r_B_dot;
	DV = length(V_B - V_A);
	Pitch = asin(dr_dot / DV);
	VH_B = h_B / r_B;
	VH_A = h_A / r_B;
	DV_H = DV * cos(Pitch);
	Yaw = asin(VH_A*sin_delta / DV_H);
	if (VH_A*sqrt(1.0 - sin_delta * sin_delta) < VH_B)
	{
		Yaw = PI - Yaw;
		if (Yaw > PI)
		{
			Yaw -= PI2;
		}
	}
}

void RTCC::PMMTISS()
{

}

void RTCC::PMSTICN(const TwoImpulseOpt &opt, TwoImpulseResuls &res)
{
	TwoImpulseMultipleSolutionTableEntry entry[13];
	EphemerisData sv_A1, sv_P1, sv_A1_apo, sv_A2, sv_A2_apo;
	double DH, PhaseAngle, Elev, WT, T1, T2, mu, DV_opt, DVT, T_WSR, DH_WSR, theta_WSR, TMAX, theta_T_min, HthetaR, theta_max, theta_min, du_dot, DV_TP, T_slip;
	double DV_OPTH, DH_OPTH, theta_OPTH, TIME_OPTH, K, D[4];
	int err, soln = 0, OPCASE, display;
	std::vector<std::string> str;

	//Initialization logic
	if (opt.sv_A.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		//ratio of catch-up rate to height difference (roughly at 160NM altitude)
		K = 5.9853114 / 3600.0 / OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		//ratio of catch-up rate to height difference (roughly at 60NM altitude)
		K = 16.38140036 / 3600.0 / OrbMech::R_Earth;
	}
	DV_opt = 10000000.0;
	//TPI velocity as a function of travel angle
	D[0] = 0.0;
	D[1] = 0.0;
	D[2] = 0.0;
	D[3] = 0.0;

	//What type of request?
	if (opt.mode == 3 || opt.mode == 4)
	{
		goto RTCC_PMSTICN_10_1;
	}
	if (opt.mode == 5)
	{
		goto RTCC_PMSTICN_9_1;
	}
	//Corrective Combination?
	if (opt.mode == 1)
	{
		goto RTCC_PMSTICN_13_1;
	}

	//Load MED request parameters
	T1 = opt.T1;
	T2 = opt.T2;
	//DTREAD: GZGENCSN Blks. 9-12
	DH = GZGENCSN.TIDeltaH;
	PhaseAngle = GZGENCSN.TIPhaseAngle;
	Elev = GZGENCSN.TIElevationAngle;
	WT = GZGENCSN.TITravelAngle;

	//DTWRITE: PZTIPREG (Set T-I Table to updating condition)
	PZTIPREG.Updating = true;
	//Load T-I Table area with necessary MED quantities
	if (DH != 0.0 && PhaseAngle != 0.0 && Elev != 0.0 && WT != 0.0 && T1 > 0 && T2 > 0)
	{
		PZTIPREG.showTPI = true;
	}
	else
	{
		PZTIPREG.showTPI = false;
	}
RTCC_PMSTICN_3_1:
	if (T1 < 0)
	{
		err = PMSTICN_ELEV(opt.sv_A, opt.sv_P, Elev, mu, T1);
		if (err)
		{
			PMXSPT("PMSTICN", 30);
			goto RTCC_PMSTICN_7_1;
		}
	}
	if (T1 >= T2)
	{
		T2 = T1 + OrbMech::time_theta(opt.sv_P.R, opt.sv_P.V, WT, mu);
	}
	if (opt.IVFLAG == 1)
	{
		TMAX = T2 + opt.TimeRange;
	}
	else if (opt.IVFLAG == 2)
	{
		TMAX = T1 + opt.TimeRange;
	}
RTCC_PMSTICN_3_3:
	sv_A1 = coast(opt.sv_A, T1 - opt.sv_A.GMT);
	sv_P1 = coast(opt.sv_P, T1 - opt.sv_P.GMT);
	//TBD: Error?
	if (!(opt.mode == 3 || opt.mode == 4 || opt.mode == 5))
	{
		if (soln == 0)
		{
			//Save state vector
			PZMYSAVE.SV_mult[0] = sv_A1;
			PZMYSAVE.SV_mult[1] = sv_P1;
		}
	}
RTCC_PMSTICN_4_2:
	err = PMMTIS(sv_A1, sv_P1, T2 - T1, DH, PhaseAngle, sv_A1_apo, sv_A2, sv_A2_apo);
	if (opt.mode == 5)
	{
		goto RTCC_PMSTICN_9_2;
	}
	if (opt.mode == 3)
	{
		goto RTCC_PMSTICN_11_2;
	}
	if (opt.mode == 4)
	{
		goto RTCC_PMSTICN_12_2;
	}
	if (err)
	{
		goto RTCC_PMSTICN_7_1;
	}
	//Store DVs and times for data table
	entry[soln].DELV1 = length(sv_A1_apo.V - sv_A1.V);
	entry[soln].DELV2 = length(sv_A2_apo.V - sv_A2.V);
	entry[soln].Time1 = sv_A1.GMT;
	entry[soln].Time2 = sv_A2.GMT;
	PMSTICN_PY(sv_A1_apo.R, sv_A1_apo.V, sv_A1.R, sv_A1.V, entry[soln].PITCH1, entry[soln].YAW1);
	PMSTICN_PY(sv_A2_apo.R, sv_A2_apo.V, sv_A2.R, sv_A2.V, entry[soln].PITCH2, entry[soln].YAW2);
	DVT = length(sv_A2_apo.V - sv_A2.V) + length(sv_A1_apo.V - sv_A1.V);
	if (soln == 0)
	{
		//TBD: Compute next environment change following frozen maneuver of first plan
	}
	//TBD: Compute next environment change following variable maneuver of this plan
	
	//Compute TPI time under special conditions
	entry[soln].T_TPI = 0.0;
	if (PZTIPREG.showTPI)
	{
		double T3;

		err = PMSTICN_ELEV(sv_A2_apo, sv_P1, Elev, mu, T3);
		if (err == 0)
		{
			entry[soln].T_TPI = T3;
		}
	}
	soln++;
RTCC_PMSTICN_7_1:
	if (opt.IVFLAG < 1)
	{
		goto RTCC_PMSTICN_8_3;
	}
	if (opt.IVFLAG == 1)
	{
		//Increment time of 2nd maneuver
		T2 += opt.TimeStep;
	}
	else
	{
		//Increment time of 1st maneuver
		T1 += opt.TimeStep;
		if (T1 >= T2)
		{
			goto RTCC_PMSTICN_8_3;
		}
	}
	//Has the number of requested cases been computed?
	if (opt.IVFLAG == 1 && T2 > TMAX)
	{
		goto RTCC_PMSTICN_8_3;
	}
	if (opt.IVFLAG == 2 && T1 > TMAX)
	{
		goto RTCC_PMSTICN_8_3;
	}
	//Was a solution available for the lost case?
	if (1 != 1)
	{
		goto RTCC_PMSTICN_8_2;
	}
	//Did the last solution have the lowest fuel expenditure so far?
	if (DVT < DV_opt)
	{
		OPCASE = soln;
		DV_opt = DVT;
	}
	if (soln >= 13)
	{
		if (OPCASE > 7)
		{
			//Set number of solutions = 12 and delete the 1st solution of present 13 available
			for (int i = 0;i < 12;i++)
			{
				entry[i] = entry[i + 1];
			}
			OPCASE--;
			soln = 12;
		}
		else
		{
			goto RTCC_PMSTICN_8_3;
		}
	}
RTCC_PMSTICN_8_2:
	//Restore elements saved at the time of the first maneuver of the first solution computed
	sv_A1 = PZMYSAVE.SV_mult[0];
	sv_P1 = PZMYSAVE.SV_mult[1];
	if (opt.IVFLAG == 2)
	{
		goto RTCC_PMSTICN_3_3;
	}
	goto RTCC_PMSTICN_4_2;
RTCC_PMSTICN_8_3:
	for (int i = 0;i < soln;i++)
	{
		PZTIPREG.data[i] = entry[i];
	}
	PZTIPREG.Solutions = soln;
	PZTIPREG.IVFLAG = opt.IVFLAG;
	PZTIPREG.MAN_VEH = opt.ChaserVehicle;
	PZTIPREG.Updating = false;
	display = 63;
	goto RTCC_PMSTICN_24_2;
RTCC_PMSTICN_9_1:
	DH = opt.DH;
	PhaseAngle = opt.PhaseAngle;
	WT = opt.WT;
	T1 = opt.T1;
	T2 = opt.T2;
	//Let the external request go through the full logic
	goto RTCC_PMSTICN_3_1;
	//goto RTCC_PMSTICN_4_2;
RTCC_PMSTICN_9_2:
	if (err)
	{
		//DKI error return
		res.SolutionFound = false;
	}
	else
	{
		//Pass solution back
		res.dV = sv_A1_apo.V - sv_A1.V;
		res.dV2 = sv_A2_apo.V - sv_A2.V;
		res.dV_LVLH = mul(OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V), res.dV);
		res.dV_LVLH2 = mul(OrbMech::LVLH_Matrix(sv_A2.R, sv_A2.V), res.dV2);
		res.T1 = GETfromGMT(T1);
		res.T2 = GETfromGMT(T2);
		res.SolutionFound = true;
	}
	goto RTCC_PMSTICN_24_3;
RTCC_PMSTICN_10_1:
	if (opt.SingSolTable == 1)
	{
		if (opt.SingSolNum > PZTIPREG.Solutions)
		{
			PMXSPT("PMSTICN", 29);
			goto RTCC_PMSTICN_24_3;
		}
		T1 = PZTIPREG.data[opt.SingSolNum - 1].Time1;
		T2 = PZTIPREG.data[opt.SingSolNum - 1].Time2;
	}
	else
	{
		if (opt.SingSolNum > PZTIPCCD.Solutions)
		{
			PMXSPT("PMSTICN", 29);
			goto RTCC_PMSTICN_24_3;
		}
	}
	//Get original state vectors for requested solution
	if (opt.SingSolTable == 1)
	{
		sv_A1 = PZMYSAVE.SV_mult[0];
		sv_P1 = PZMYSAVE.SV_mult[1];
	}
	else
	{
		sv_A1 = PZMYSAVE.SV_CC[0];
		sv_P1 = PZMYSAVE.SV_CC[1];
	}
	if (opt.SingSolTable == 2)
	{
		goto RTCC_PMSTICN_11_1;
	}
	//Set up STAIDs, mnvrs, threshold times and desired offsets for re-computation of desired solution
	DH = GZGENCSN.TIDeltaH;
	PhaseAngle = GZGENCSN.TIPhaseAngle;
	//Is same?
	if (T1 == sv_A1.GMT)
	{
		goto RTCC_PMSTICN_4_2;
	}
	else
	{
		goto RTCC_PMSTICN_3_3;
	}
RTCC_PMSTICN_11_1:
	//Set up STAIDs, mnvrs, threshold times and desired offsets for re-computation of desired CC solution
	goto RTCC_PMSTICN_4_2;
RTCC_PMSTICN_11_2:
	if (err)
	{
		goto RTCC_PMSTICN_24_3;
	}
	//TBD: Compute pitch and yaw
	//TBD: PMMDAN twice
	//PMMTISS();
	//Set up single solution display
	goto RTCC_PMSTICN_24_2;
RTCC_PMSTICN_12_2:
	//Transfer plan
	if (err)
	{
		goto RTCC_PMSTICN_24_1;
	}

	PZMYSAVE.SV_before[0] = sv_A1;
	PZMYSAVE.V_after[0] = sv_A1_apo.V;
	PZMYSAVE.SV_before[1] = sv_A2;
	PZMYSAVE.V_after[1] = sv_A2_apo.V;
	PZMYSAVE.code[0] = "NC1";
	PZMYSAVE.code[1] = "NC2";
	if (opt.SingSolTable == 1)
	{
		PZMYSAVE.plan[0] = PZTIPREG.MAN_VEH;
		PZMYSAVE.plan[1] = PZTIPREG.MAN_VEH;
	}
	else
	{
		PZMYSAVE.plan[0] = PZTIPCCD.MAN_VEH;
		PZMYSAVE.plan[1] = PZTIPCCD.MAN_VEH;
	}

	PMMMED("72", str);
	return;
RTCC_PMSTICN_13_1:
	//Corrective Combination
	//Load MED request quantities
	T1 = opt.T1;
	//Set C.C. Table to updating condition
	PZTIPCCD.Updating = true;
	//DTREAD: GZGENCSN Blks. 12-15
	WT = GZGENCSN.TITravelAngle;
	T_WSR = GZGENCSN.TINSRNominalTime;
	DH_WSR = GZGENCSN.TINSRNominalDeltaH;
	theta_WSR = GZGENCSN.TINSRNominalPhaseAngle;
	//Build first block of corrective combination table (Station IDs, threshold times, time of 1st maneuver)
	//Advance chaser and target to the time of the NCC maneuver
	sv_A1 = coast(opt.sv_A, T1 - opt.sv_A.GMT);
	sv_P1 = coast(opt.sv_P, T1 - opt.sv_P.GMT);
	//TBD: AEG error?
	//Save initial elements for regeneration
	PZMYSAVE.SV_CC[0] = sv_A1;
	PZMYSAVE.SV_CC[1] = sv_P1;
	//Initialize optimum DV to max, height offset and time of NSR maneuver to MED request minimum
	DV_opt = 10000000.0;
	DH = opt.DH_min;
	T2 = opt.T2_min;
	theta_T_min = theta_WSR - (opt.T2_min - T_WSR)*K*DH_WSR;
	if (theta_T_min == 0.0 || DH_WSR == 0.0)
	{
		//Error
		goto RTCC_PMSTICN_18_1;
	}
	//Compute phase angle for 1st case
	HthetaR = DH_WSR / theta_T_min;
	PhaseAngle = opt.DH_min / HthetaR;
RTCC_PMSTICN_15_1:
	du_dot = DH * K;
	DV_TP = abs((D[0] + WT * (D[1] + WT * (D[2] + WT * D[3])))*DH);//*CTERMV;
	//Is this a slip time option request?
	if (opt.CCReqInd == 1)
	{
		theta_max = PhaseAngle - opt.TPILimit * du_dot;
		theta_min = PhaseAngle + opt.TPILimit * du_dot;
		PhaseAngle = theta_min;
	}
	else
	{
		T2 = opt.T2_min;
	}
RTCC_PMSTICN_15_2:
	err = PMMTIS(sv_A1, sv_P1, T2 - T1, DH, PhaseAngle, sv_A1_apo, sv_A2, sv_A2_apo);
	if (err == 0)
	{
		DVT = length(sv_A2_apo.V - sv_A2.V) + length(sv_A1_apo.V - sv_A1.V) + DV_TP;
		if (DVT < DV_opt)
		{
			DV_OPTH = DVT;
			DH_OPTH = DH;
			TIME_OPTH = T2;
			theta_OPTH = PhaseAngle;
		}
	}
	//Restore original chaser and target elements for next case
	sv_A1 = PZMYSAVE.SV_CC[0];
	sv_P1 = PZMYSAVE.SV_CC[1];
	//Compute phase angle for next solution
	PhaseAngle = PhaseAngle - du_dot * opt.dt_inc;
	//Is this a slip time option request?
	if (opt.CCReqInd == 1)
	{
		goto RTCC_PMSTICN_17_1;
	}
	//Compute next NSR time at this height
	T2 = T2 + opt.dt_inc;
	//Is the new NSR time past the MED maximum?
	if (T2 >= opt.T2_max)
	{
		goto RTCC_PMSTICN_17_2;
	}
	goto RTCC_PMSTICN_15_2;
RTCC_PMSTICN_17_1:
	//Have all phase angles been tried at this height?
	if (PhaseAngle > theta_max)
	{
		goto RTCC_PMSTICN_15_2;
	}
RTCC_PMSTICN_17_2:
	if (err == 0)
	{
		//Reset optimum to maximum
		DV_opt = 100000000.0;
		if (opt.CCReqInd == 1)
		{
			T_slip = (theta_min - opt.TPILimit*du_dot - theta_OPTH) / du_dot;
		}
		else
		{
			T_slip = 0.0;
		}
		if (soln >= 13)
		{
			goto RTCC_PMSTICN_18_1;
		}
	}
	DH = DH + opt.dh_inc;
	if (DH > opt.DH_max)
	{
		goto RTCC_PMSTICN_18_1;
	}
	PhaseAngle = DH / HthetaR;
	goto RTCC_PMSTICN_15_1;
RTCC_PMSTICN_18_1:
	//Write solutions
	//Set up display
	display = 0;
	goto RTCC_PMSTICN_24_2;
RTCC_PMSTICN_24_1:
	PMXSPT("PMSTICN", 29);
	if (opt.mode == 1)
	{
		goto RTCC_PMSTICN_18_1;
	}
	if (opt.mode == 2)
	{
		goto RTCC_PMSTICN_7_1;
	}
	goto RTCC_PMSTICN_24_3;
RTCC_PMSTICN_24_2:
	EMSNAP(0, display);
RTCC_PMSTICN_24_3:
	return;
}

void RTCC::LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res)
{
	SV sv_A1, sv_A1_apo, sv_A2, sv_P1, sv_P2;
	double dt1, dt1_apo, dt2, mu, T1, T2;
	int N;
	OBJHANDLE gravref;
	int body;
	bool prograde;

	gravref = lambert->sv_A.gravref;
	N = lambert->N;

	if (gravref == oapiGetObjectByName("Earth"))	//Hardcoded: Always prograde for Earth, always retrograde for Moon
	{
		prograde = true;
		mu = OrbMech::mu_Earth;
		body = BODY_EARTH;
	}
	else
	{
		prograde = false;
		mu = OrbMech::mu_Moon;
		body = BODY_MOON;
	}

	if (lambert->mode == 2)
	{
		if (lambert->T1 >= 0)
		{
			T1 = lambert->T1;
		}
		else
		{
			T1 = TPISearch(lambert->sv_A, lambert->sv_P, lambert->GETbase, lambert->ElevationAngle);
		}
	}
	else
	{
		T1 = lambert->T1;
	}

	dt1 = T1 - (lambert->sv_A.MJD - lambert->GETbase) * 24.0 * 60.0 * 60.0;
	dt1_apo = T1 - (lambert->sv_P.MJD - lambert->GETbase) * 24.0 * 60.0 * 60.0;

	if (lambert->Perturbation == 1)
	{
		sv_A1 = coast(lambert->sv_A, dt1);
		sv_P1 = coast(lambert->sv_P, dt1_apo);
	}
	else
	{
		OrbMech::rv_from_r0v0(lambert->sv_A.R, lambert->sv_A.V, dt1, sv_A1.R, sv_A1.V, mu);
		OrbMech::rv_from_r0v0(lambert->sv_P.R, lambert->sv_P.V, dt1_apo, sv_P1.R, sv_P1.V, mu);
	}

	if (lambert->mode == 2)
	{
		if (lambert->T2 >= 0)
		{
			T2 = lambert->T2;
		}
		else
		{
			T2 = T1 + OrbMech::time_theta(sv_P1.R, sv_P1.V, lambert->TravelAngle, mu);
		}
	}
	else
	{
		T2 = lambert->T2;
	}

	dt2 = T2 - T1;

	if (lambert->Perturbation == 1)
	{
		sv_P2 = coast(lambert->sv_P, dt1_apo + dt2);
	}
	else
	{
		OrbMech::rv_from_r0v0(lambert->sv_P.R, lambert->sv_P.V, dt1_apo + dt2, sv_P2.R, sv_P2.V, mu);
	}

	MATRIX3 Q_Xx;
	VECTOR3 RP2off, VP2off, VA1_apo;

	if (lambert->mode == 0)
	{
		double angle;
		angle = lambert->Offset.x / length(sv_P2.R);

		OrbMech::rv_from_r0v0_ta(sv_P2.R, sv_P2.V, angle, RP2off, VP2off, mu);

		VECTOR3 i, j, k;
		MATRIX3 Q_Xx2;

		k = -unit(RP2off);
		j = unit(crossp(VP2off, RP2off));
		i = crossp(j, k);
		Q_Xx2 = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);

		RP2off = RP2off + tmul(Q_Xx2, _V(0.0, lambert->Offset.y, lambert->Offset.z));
	}
	else
	{
		if (lambert->PhaseAngle != 0 || lambert->DH != 0)
		{
			CELEMENTS elem_T, elem_CE;
			double f_T, f_CE;
			elem_T = OrbMech::GIMIKC(sv_P2.R, sv_P2.V, mu);
			f_T = OrbMech::MeanToTrueAnomaly(elem_T.l, elem_T.e);
			elem_CE.a = elem_T.a - lambert->DH;
			elem_CE.e = elem_T.e*elem_T.a / elem_CE.a;
			f_CE = f_T - lambert->PhaseAngle;
			elem_CE.l = OrbMech::TrueToMeanAnomaly(f_CE, elem_CE.e);
			elem_CE.i = elem_T.i;
			elem_CE.g = elem_T.g;
			elem_CE.h = elem_T.h;
			OrbMech::GIMKIC(elem_CE, mu, RP2off, VP2off);
		}
		else
		{
			RP2off = sv_P2.R;
			VP2off = sv_P2.V;
		}
	}

	if (lambert->Perturbation == RTCC_LAMBERT_PERTURBED)
	{
		VA1_apo = OrbMech::Vinti(sv_A1.R, sv_A1.V, RP2off, sv_A1.MJD, dt2, N, prograde, body, body, body, _V(0.0, 0.0, 0.0), 100.0*0.3048); //Vinti Targeting: For non-spherical gravity
	
		sv_A1_apo = sv_A1;
		sv_A1_apo.V = VA1_apo;
		sv_A2 = coast(sv_A1_apo, dt2);
	}
	else
	{
		if (lambert->axis == RTCC_LAMBERT_MULTIAXIS)
		{
			VA1_apo = OrbMech::elegant_lambert(sv_A1.R, sv_A1.V, RP2off, dt2, N, prograde, mu);	//Lambert Targeting
		}
		else
		{
			OrbMech::xaxislambert(sv_A1.R, sv_A1.V, RP2off, dt2, N, prograde, mu, VA1_apo, lambert->Offset.z);	//Lambert Targeting
		}

		sv_A1_apo = sv_A1;
		sv_A1_apo.V = VA1_apo;
		sv_A2 = sv_A1_apo;
		OrbMech::rv_from_r0v0(sv_A1_apo.R, sv_A1_apo.V, dt2, sv_A2.R, sv_A2.V, mu);
	}

	Q_Xx = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);
	res.dV = VA1_apo - sv_A1.V;
	res.dV2 = sv_P2.V - sv_A2.V;
	res.dV_LVLH = mul(Q_Xx, res.dV);
	res.T1 = T1;
	res.T2 = T2;

	if (lambert->axis == RTCC_LAMBERT_XAXIS)
	{
		res.dV_LVLH.y = 0.0;
	}

	if (lambert->mode == 1)
	{
		SV sv_A2_apo;
		VECTOR3 u, R_A2, V_A2, R_PC, V_PC, DV;
		double dt_TPI;

		sv_A2_apo = sv_A2;

		u = unit(crossp(sv_P2.R, sv_P2.V));

		R_A2 = unit(sv_A2.R - u * dotp(sv_A2.R, u))*length(sv_A2.R);
		V_A2 = unit(sv_A2.V - u * dotp(sv_A2.V, u))*length(sv_A2.V);

		OrbMech::RADUP(sv_P2.R, sv_P2.V, R_A2, mu, R_PC, V_PC);
		DV = OrbMech::CoellipticDV(R_A2, R_PC, V_PC, mu) - V_A2;
		sv_A2_apo.V += DV;

		dt_TPI = OrbMech::findelev(sv_A2_apo.R, sv_A2_apo.V, sv_P2.R, sv_P2.V, sv_P2.MJD, lambert->ElevationAngle, gravref);
		res.t_TPI = OrbMech::GETfromMJD(sv_P2.MJD, lambert->GETbase) + dt_TPI;
	}

	if (lambert->storesolns == false) return;

	PZMYSAVE.SV_before[0] = ConvertSVtoEphemData(sv_A1);
	PZMYSAVE.V_after[0] = sv_A1_apo.V;
	PZMYSAVE.plan[0] = lambert->ChaserVehicle;
	if (lambert->mode == 0)
	{
		PZMYSAVE.code[0] = "LAM";
	}
	else if (lambert->mode == 1)
	{
		PZMYSAVE.code[0] = "NCC";
	}
	else
	{
		PZMYSAVE.code[0] = "TPI";
	}

	PZMYSAVE.SV_before[1] = ConvertSVtoEphemData(sv_A2);
	PZMYSAVE.V_after[1] = VP2off;
	PZMYSAVE.plan[1] = lambert->ChaserVehicle;
	if (lambert->mode == 0)
	{
		PZMYSAVE.code[1] = "TAR";
	}
	else if (lambert->mode == 1)
	{
		PZMYSAVE.code[1] = "NSR";
	}
	else
	{
		PZMYSAVE.code[1] = "TPF";
	}
}

void RTCC::LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step)
{
	double bt, dv_10p, mass40p, bt_40p, dv_40p, mass100p, bt_100p, time_10p, v_G, F_L, K4, mintime, t_throttleup;
	bool S_TH;

	F_L = 200.0*4.448222;
	K4 = 1050.0*4.448222;

	mintime = 4.0;
	t_throttleup = 26.0;

	v_G = dV_LVLH - 4.0*F_L / mass;
	if (v_G*mass / K4 < 95.0)
	{
		//LGC won't throttle up to 100% at 26 seconds
		S_TH = true;
	}
	else
	{
		//LGC will throttle up to 100% at 26 seconds
		S_TH = false;
	}

	if (mass > 30000.0)
	{
		//heavy docked burn, throttle up to 40% at 5 seconds
		time_10p = 5.0;
	}
	else
	{
		//undocked or light docked burn, throttle up to 40% at 15 seconds
		time_10p = 15.0;
	}

	//Throttle Profile:
	//Basic principle: spend at least 4 seconds at one throttle setting before cutoff
	//Burntime at 10% smaller than 21 seconds: full burn at 10%
	//

	bt = v_e / (0.1*F) *(mass)*(1.0 - exp(-dV_LVLH / v_e)); //burn time at 10% thrust

	if (bt > time_10p + mintime * 0.4 / 0.1)	//Burn longer than 4 seconds at 40%
	{
		dv_10p = v_e * log((mass) / (mass - 0.1*F / v_e * time_10p)); //Five seconds at 10%
		mass40p = mass - 0.1*F / v_e * time_10p;
		bt_40p = v_e / (0.4*F) *(mass40p)*(1.0 - exp(-(dV_LVLH - dv_10p) / v_e)); // rest burn time at 40% thrust

		if (S_TH == false)	//LGC will throttle up to 100% at 26 seconds
		{
			dv_40p = v_e * log((mass40p) / (mass40p - 0.4*F / v_e * (t_throttleup - time_10p))); //21 seconds at 40%
			mass100p = mass40p - 0.4*F / v_e * (t_throttleup - time_10p);
			bt_100p = v_e / F * (mass100p)*(1.0 - exp(-(dV_LVLH - dv_10p - dv_40p) / v_e)); // rest burn time at 100% thrust

			F_average = (0.1 * time_10p + 0.4*(t_throttleup - time_10p) + bt_100p) / (t_throttleup + bt_100p)*F;
			ManPADBurnTime = bt_100p + 26.0;
			bt_var = bt_100p;

			step = 2;
		}
		else
		{
			F_average = (0.1 * time_10p + 0.4*bt_40p) / (bt_40p + time_10p)*F;
			ManPADBurnTime = bt_40p + time_10p;
			bt_var = bt_40p;

			step = 1;
		}
	}
	else
	{
		F_average = 0.1*F;	//Full burn at 10%
		ManPADBurnTime = bt;
		bt_var = bt;

		step = 0;
	}
}

void RTCC::AP10CSIPAD(AP10CSIPADOpt *opt, AP10CSI &pad)
{
	SV sv1, sv2;
	MATRIX3 Q_Xx, M, M_R;
	VECTOR3 V_G, UX, UY, UZ, IMUangles, FDAIangles, dV_AGS;
	double dt, TIG_AGS;

	dt = opt->t_CSI - (opt->sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(opt->sv0, dt);

	PoweredFlightProcessor(sv1, opt->GETbase, opt->t_CSI, opt->enginetype, 0.0, opt->dV_LVLH, true, TIG_AGS, dV_AGS, false);

	Q_Xx = OrbMech::LVLH_Matrix(sv1.R, sv1.V);
	V_G = tmul(Q_Xx, opt->dV_LVLH);

	UX = unit(V_G);

	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.DEDA373 = (opt->t_CSI - opt->KFactor) / 60.0;
	pad.DEDA275 = (opt->t_TPI - opt->KFactor) / 60.0;
	pad.PLM_FDAI = FDAIangles.y*DEG;
	pad.t_CSI = opt->t_CSI;
	pad.t_TPI = opt->t_TPI;
	pad.dV_LVLH = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = dV_AGS / 0.3048;
}

void RTCC::AP11LMManeuverPAD(AP11LMManPADOpt *opt, AP11LMMNV &pad)
{
	MATRIX3 M_R, M, Q_Xx;
	VECTOR3 V_G, X_B, UX, UY, UZ, IMUangles, FDAIangles;
	double dt, mu, CSMmass, ManPADBurnTime, apo, peri, ManPADApo, ManPADPeri, ManPADDVR, ManBSSpitch, ManBSSXPos, R_E, headsswitch;
	int ManCOASstaroct;
	SV sv, sv1, sv2;

	//State vector from PAD options or get a new one
	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	//Coast until TIG
	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv, dt);

	//Docked mass
	if (opt->csmlmdocked == true)
	{
		CSMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		CSMmass = 0.0;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(sv1, opt->GETbase, opt->TIG, opt->dV_LVLH, CSMmass, opt->enginetype, Q_Xx, V_G);
	ManPADBurnTime = (sv2.MJD - sv1.MJD)*24.0*3600.0;

	//Only use landing site radius for the Moon
	if (sv1.gravref == oapiGetObjectByName("Moon"))
	{
		R_E = opt->R_LLS;
		mu = OrbMech::mu_Moon;
	}
	else
	{
		R_E = OrbMech::R_Earth;
		mu = OrbMech::mu_Earth;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = unit(V_G);
	if (opt->enginetype == RTCC_ENGINETYPE_LMRCSMINUS2 || opt->enginetype == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		UX = -X_B;
	}
	else
	{
		UX = X_B;
	}
	if (abs(dotp(UX, unit(sv1.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv1.R*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, sv1.V));
	}
	UZ = unit(crossp(UX, UY));


	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);


	ManPADDVR = length(opt->dV_LVLH);

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));
	pad.IMUAtt = IMUangles;

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), ManCOASstaroct, ManBSSpitch, ManBSSXPos);
	
	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.burntime = ManPADBurnTime;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.dV_AGS = mul(Q_Xx, V_G) / 0.3048;
	pad.GETI = opt->TIG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	pad.SPA = ManBSSpitch*DEG;
	pad.SXP = ManBSSXPos*DEG;
	pad.dVR = ManPADDVR / 0.3048;
	pad.CSMWeight = CSMmass / 0.45359237;
	pad.LMWeight = sv1.mass / 0.45359237;
}

void RTCC::AP11ManeuverPAD(AP11ManPADOpt *opt, AP11MNV &pad)
{
	MATRIX3 Q_Xx, M_R, M, M_RTM;
	VECTOR3 V_G, X_B, UX, UY, UZ, IMUangles, GDCangles;
	double dt, LMmass, headsswitch, mu, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim, p_T, y_T, ManPADDVC, ManPADBurnTime;
	double Mantrunnion, Manshaft, ManBSSpitch, ManBSSXPos, R_E;
	int GDCset, Manstaroct, ManCOASstaroct;
	SV sv, sv1, sv2;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	sv1 = coast(sv, dt);

	if (opt->vesseltype == 1)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(sv1, opt->GETbase, opt->TIG, opt->dV_LVLH, LMmass, opt->enginetype, Q_Xx, V_G);
	ManPADBurnTime = (sv2.MJD - sv1.MJD)*24.0*3600.0;

	//Only use landing site radius for the Moon
	if (sv1.gravref == oapiGetObjectByName("Moon"))
	{
		R_E = opt->R_LLS;
		mu = OrbMech::mu_Moon;
	}
	else
	{
		R_E = OrbMech::R_Earth;
		mu = OrbMech::mu_Earth;
	}

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		double T, WDOT;
		unsigned IC;
		if (LMmass > 0)
		{
			IC = 13;
		}
		else
		{
			IC = 1;
		}
		GIMGBL(sv1.mass, LMmass, p_T, y_T, T, WDOT, RTCC_ENGINETYPE_CSMSPS, IC, 1, 0, 0.0);

		ManPADPTrim = p_T + 2.15*RAD;
		ManPADYTrim = y_T - 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, sv1.R*headsswitch));
		UZ = unit(crossp(X_B, crossp(X_B, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::tmat(M_R), M);

		ManPADDVC = length(opt->dV_LVLH)*cos(p_T)*cos(y_T);// -60832.18 / m1;
	}
	else
	{
		ManPADPTrim = 0.0;
		ManPADYTrim = 0.0;

		X_B = unit(V_G);
		if (opt->enginetype == RTCC_ENGINETYPE_CSMRCSPLUS2 || opt->enginetype == RTCC_ENGINETYPE_CSMRCSPLUS4)
		{
			UX = X_B;
			ManPADDVC = length(opt->dV_LVLH);
		}
		else
		{
			UX = -X_B;
			ManPADDVC = -length(opt->dV_LVLH);
		}
		UY = unit(crossp(UX, sv1.R*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	}

	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	GDCangles = OrbMech::backupgdcalignment(opt->REFSMMAT, sv1.R, oapiGetSize(sv1.gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), Manstaroct, Mantrunnion, Manshaft);

	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(IMUangles.x*DEG)*RAD, OrbMech::round(IMUangles.y*DEG)*RAD, OrbMech::round(IMUangles.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), ManCOASstaroct, ManBSSpitch, ManBSSXPos);

	pad.Att = _V(OrbMech::imulimit(IMUangles.x*DEG), OrbMech::imulimit(IMUangles.y*DEG), OrbMech::imulimit(IMUangles.z*DEG));
	pad.BSSStar = ManCOASstaroct;
	pad.burntime = ManPADBurnTime;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.GDCangles = _V(OrbMech::imulimit(GDCangles.x*DEG), OrbMech::imulimit(GDCangles.y*DEG), OrbMech::imulimit(GDCangles.z*DEG));
	pad.GETI = opt->TIG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;

	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		sprintf(pad.PropGuid, "SPS/G&N");
	}
	else
	{
		sprintf(pad.PropGuid, "RCS/G&N");
	}

	pad.pTrim = ManPADPTrim*DEG;
	pad.yTrim = ManPADYTrim*DEG;

	if (length(GDCangles) == 0.0)
	{
		sprintf(pad.SetStars, "N/A");
	}
	else
	{
		if (GDCset == 0)
		{
			sprintf(pad.SetStars, "Vega, Deneb");
		}
		else if (GDCset == 1)
		{
			sprintf(pad.SetStars, "Navi, Polaris");
		}
		else
		{
			sprintf(pad.SetStars, "Acrux, Atria");
		}
	}
	pad.Shaft = Manshaft*DEG;
	pad.SPA = ManBSSpitch*DEG;
	pad.Star = Manstaroct;
	pad.SXP = ManBSSXPos*DEG;
	pad.Trun = Mantrunnion*DEG;
	pad.Vc = ManPADDVC / 0.3048;
	pad.Vt = length(opt->dV_LVLH) / 0.3048;
	pad.Weight = sv1.mass / 0.45359237;
	pad.LMWeight = LMmass / 0.45359237;

	pad.GET05G = 0;
	pad.lat = 0;
	pad.lng = 0;
	pad.RTGO = 0;
}

void RTCC::AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad)
{
	MATRIX3 Q_Xx, M, M_R, M_RTM;
	VECTOR3 V_G, X_B, UX, UY, UZ, Att;
	double dt, LMmass, mu, headsswitch, apo, peri, ManPADApo, ManPADPeri, ManPADPTrim, ManPADYTrim, y_T, p_T, R_E;
	SV sv, sv1, sv2;

	if (opt->useSV)
	{
		sv = opt->RV_MCC;
	}
	else
	{
		sv = StateVectorCalc(opt->vessel);
	}

	dt = opt->TIG - (sv.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	sv1 = coast(sv, dt);

	if (opt->vesseltype == 1)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	//Execute maneuver, output state vector at cutoff
	sv2 = ExecuteManeuver(sv1, opt->GETbase, opt->TIG, opt->dV_LVLH, LMmass, opt->enginetype, Q_Xx, V_G);
	pad.burntime = (sv2.MJD - sv1.MJD)*24.0*3600.0;

	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = OrbMech::R_Moon;
	}

	if (opt->HeadsUp)
	{
		headsswitch = 1.0;
	}
	else
	{
		headsswitch = -1.0;
	}

	OrbMech::periapo(sv2.R, sv2.V, mu, apo, peri);
	ManPADApo = apo - R_E;
	ManPADPeri = peri - R_E;

	pad.Weight = sv1.mass / 0.45359237;

	if (opt->enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		double T, WDOT;
		unsigned IC;
		if (LMmass > 0)
		{
			IC = 13;
		}
		else
		{
			IC = 1;
		}
		GIMGBL(sv1.mass, LMmass, p_T, y_T, T, WDOT, RTCC_ENGINETYPE_CSMSPS, IC, 1, 0, 0.0);

		ManPADPTrim = p_T + 2.15*RAD;
		ManPADYTrim = y_T - 0.95*RAD;

		X_B = unit(V_G);
		UX = X_B;
		UY = unit(crossp(X_B, sv1.R*headsswitch));
		UZ = unit(crossp(X_B, UY));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
		M_RTM = mul(OrbMech::tmat(M_R), M);

		pad.Vc = length(opt->dV_LVLH)*cos(p_T)*cos(y_T);// -60832.18 / m1;
	}
	else
	{
		ManPADPTrim = 0.0;
		ManPADYTrim = 0.0;

		X_B = unit(V_G);
		if (opt->enginetype == RTCC_ENGINETYPE_CSMRCSPLUS2 || opt->enginetype == RTCC_ENGINETYPE_CSMRCSPLUS4)
		{
			UX = X_B;
			pad.Vc = length(opt->dV_LVLH);
		}
		else
		{
			UX = -X_B;
			pad.Vc = -length(opt->dV_LVLH);
		}
		UY = unit(crossp(UX, sv1.R*headsswitch));
		UZ = unit(crossp(UX, crossp(UX, sv1.R*headsswitch)));


		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	}
	Att = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));
	//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);

	//GDCangles = OrbMech::backupgdcalignment(REFSMMAT, R1B, oapiGetSize(gravref), GDCset);

	VECTOR3 Rsxt, Vsxt;

	OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->sxtstardtime, Rsxt, Vsxt, sv1.gravref, sv1.gravref);

	OrbMech::checkstar(opt->REFSMMAT, _V(round(Att.x*DEG)*RAD, round(Att.y*DEG)*RAD, round(Att.z*DEG)*RAD), Rsxt, oapiGetSize(sv1.gravref), pad.Star, pad.Trun, pad.Shaft);

	if (opt->navcheckGET != 0.0)
	{
		VECTOR3 Rnav, Vnav;
		double alt, lat, lng;
		OrbMech::oneclickcoast(sv1.R, sv1.V, sv1.MJD, opt->navcheckGET - opt->TIG, Rnav, Vnav, sv1.gravref, sv1.gravref);
		navcheck(Rnav, Vnav, opt->GETbase + opt->navcheckGET / 24.0 / 3600.0, sv1.gravref, lat, lng, alt);

		pad.NavChk = opt->navcheckGET;
		pad.lat = lat*DEG;
		pad.lng = lng*DEG;
		pad.alt = alt / 1852;
	}

	pad.Att = _V(OrbMech::imulimit(Att.x*DEG), OrbMech::imulimit(Att.y*DEG), OrbMech::imulimit(Att.z*DEG));

	pad.GETI = opt->TIG;
	pad.pTrim = ManPADPTrim*DEG;
	pad.yTrim = ManPADYTrim*DEG;
	pad.dV = opt->dV_LVLH / 0.3048;
	pad.Vc /= 0.3048;
	pad.Shaft *= DEG;
	pad.Trun *= DEG;
	pad.HA = min(9999.9, ManPADApo / 1852.0);
	pad.HP = ManPADPeri / 1852.0;
}

void RTCC::AP7TPIPAD(const AP7TPIPADOpt &opt, AP7TPI &pad)
{
	SV sv_A2, sv_P2, sv_A3, sv_P3;
	double dt1, dt2;
	VECTOR3 u, U_L, UX, UY, UZ, U_R, U_R2, U_P, TPIPAD_BT, TPIPAD_dV_LOS;
	MATRIX3 Rot1, Rot2;
	double TPIPAD_AZ, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5,TPIPAD_dH, TPIPAD_ddH;
	VECTOR3 U_F, LOS, U_LOS, NN;

	dt1 = opt.TIG - (opt.sv_A.MJD - opt.GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt.TIG - (opt.sv_P.MJD - opt.GETbase) * 24.0 * 60.0 * 60.0;

	sv_A3 = coast(opt.sv_A, dt1);
	sv_P3 = coast(opt.sv_P, dt2);

	UY = unit(crossp(sv_A3.V, sv_A3.R));
	UZ = unit(-sv_A3.R);
	UX = crossp(UY, UZ);

	Rot1 = _M(UX.x, UY.x, UZ.x, UX.y, UY.y, UZ.y, UX.z, UY.z, UZ.z);

	u = unit(crossp(sv_A3.R, sv_A3.V));
	U_L = unit(sv_P3.R - sv_A3.R);
	UX = U_L;
	UY = unit(crossp(crossp(u, UX), UX));
	UZ = crossp(UX, UY);

	Rot2 = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);

	TPIPAD_dV_LOS = mul(Rot2, mul(Rot1, opt.dV_LVLH));
	//TPIPAD_dH = abs(length(RP3) - length(RA3));
	double mass, F;

	mass = opt.sv_A.mass;
	F = 200.0 * 4.448222;
	TPIPAD_BT = _V(abs(0.5*TPIPAD_dV_LOS.x), abs(TPIPAD_dV_LOS.y), abs(TPIPAD_dV_LOS.z))*mass / F;

	VECTOR3 i, j, k, dr, dv, dr0, dv0, Omega;
	MATRIX3 Q_Xx;
	double t1, t2, dxmin, n, dxmax;

	j = unit(sv_P3.V);
	k = unit(crossp(sv_P3.R, j));
	i = crossp(j, k);
	Q_Xx = _M(i.x, i.y, i.z, j.x, j.y, j.z, k.x, k.y, k.z);
	dr = sv_A3.R - sv_P3.R;
	n = length(sv_P3.V) / length(sv_P3.R);
	Omega = k*n;
	dv = sv_A3.V - sv_P3.V - crossp(Omega, dr);
	dr0 = mul(Q_Xx, dr);
	dv0 = mul(Q_Xx, dv);
	t1 = 1.0 / n*atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y));
	t2 = 1.0 / n*(atan(-dv0.x / (3.0*n*dr0.x + 2.0 * dv0.y)) + PI);
	dxmax = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t1) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t1);
	dxmin = 4.0 * dr0.x + 2.0 / n*dv0.y + dv0.x / n*sin(n*t2) - (3.0 * dr0.x + 2.0 / n*dv0.y)*cos(n*t2);

	sv_A2 = coast(sv_A3, -5.0*60.0);
	sv_P2 = coast(sv_P3, -5.0*60.0);

	U_R2 = unit(sv_P2.R - sv_A2.R);

	TPIPAD_dH = -dr0.x;
	TPIPAD_ddH = abs(dxmax - dxmin);
	TPIPAD_R = abs(length(sv_P2.R - sv_A2.R));
	TPIPAD_Rdot = dotp(sv_P2.V - sv_A2.V, U_R2);

	U_L = unit(sv_P2.R - sv_A2.R);
	U_P = unit(U_L - sv_A2.R*dotp(U_L, sv_A2.R) / length(sv_A2.R) / length(sv_A2.R));

	TPIPAD_ELmin5 = acos(dotp(U_L, U_P*OrbMech::sign(dotp(U_P, crossp(u, sv_A2.R)))));

	U_F = unit(crossp(crossp(sv_A2.R, sv_A2.V), sv_A2.R));
	U_R = unit(sv_A2.R);
	LOS = sv_P2.R - sv_A2.R;
	U_LOS = unit(LOS - U_R*dotp(LOS, U_R));
	TPIPAD_AZ = acos(dotp(U_LOS, U_F));//atan2(-TPIPAD_dV_LOS.z, TPIPAD_dV_LOS.x);
	NN = crossp(U_LOS, U_F);
	if (dotp(NN, sv_A2.R) < 0)
	{
		TPIPAD_AZ = PI2 - TPIPAD_AZ;
	}

	pad.AZ = TPIPAD_AZ*DEG;
	pad.Backup_bT = TPIPAD_BT;
	pad.Backup_dV = TPIPAD_dV_LOS / 0.3048;
	pad.R = TPIPAD_R / 1852.0;
	pad.Rdot = TPIPAD_Rdot / 0.3048;
	pad.EL = TPIPAD_ELmin5*DEG;
	pad.GETI = opt.TIG;
	pad.Vg = opt.dV_LVLH / 0.3048;
	pad.dH_TPI = TPIPAD_dH / 1852.0;
	pad.dH_Max = TPIPAD_ddH / 1852.0;
}

void RTCC::AP9LMTPIPAD(AP9LMTPIPADOpt *opt, AP9LMTPI &pad)
{
	SV sv_A1, sv_P1;
	MATRIX3 Rot1, Rot2, M;
	VECTOR3 u, U_L, dV_LOS, IMUangles, FDAIangles, R1, R2, R3;
	double R, Rdot;

	sv_A1 = coast(opt->sv_A, opt->TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, opt->GETbase));
	sv_P1 = coast(opt->sv_P, opt->TIG - OrbMech::GETfromMJD(opt->sv_P.MJD, opt->GETbase));
	Rot1 = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);

	u = unit(crossp(sv_A1.R, sv_A1.V));
	U_L = unit(sv_P1.R - sv_A1.R);
	R1 = U_L;
	R2 = unit(crossp(crossp(u, R1), R1));
	R3 = crossp(R1, R2);
	Rot2 = _M(R1.x, R1.y, R1.z, R2.x, R2.y, R2.z, R3.x, R3.y, R3.z);

	dV_LOS = mul(Rot2, tmul(Rot1, opt->dV_LVLH));

	R = abs(length(sv_P1.R - sv_A1.R));
	Rdot = dotp(sv_P1.V - sv_A1.V, U_L);

	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), Rot1));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.Backup_dV = dV_LOS / 0.3048;
	pad.dVR = length(opt->dV_LVLH) / 0.3048;
	pad.GETI = opt->TIG;
	pad.R = R / 1852.0;
	pad.Rdot = Rdot / 0.3048;
	pad.Vg = opt->dV_LVLH / 0.3048;
}

void RTCC::AP9LMCDHPAD(AP9LMCDHPADOpt *opt, AP9LMCDH &pad)
{
	SV sv_A1;
	MATRIX3 Rot1, M;
	VECTOR3 IMUangles, FDAIangles, V_G;

	sv_A1 = coast(opt->sv_A, opt->TIG - OrbMech::GETfromMJD(opt->sv_A.MJD, opt->GETbase));
	Rot1 = OrbMech::LVLH_Matrix(sv_A1.R, sv_A1.V);
	V_G = tmul(Rot1, opt->dV_LVLH);

	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), Rot1));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Pitch = OrbMech::imulimit(FDAIangles.y*DEG);
	pad.Vg_AGS = mul(Rot1, V_G) / 0.3048;
	pad.GETI = opt->TIG;
	pad.Vg = opt->dV_LVLH / 0.3048;
}

void RTCC::CSMDAPUpdate(VESSEL *v, AP10DAPDATA &pad)
{
	double CSMmass, LMmass, p_T, y_T;

	CSMmass = v->GetMass();
	LMmass = GetDockedVesselMass(v);

	double T, WDOT;
	unsigned IC;
	if (LMmass > 0)
	{
		IC = 13;
	}
	else
	{
		IC = 1;
	}
	GIMGBL(CSMmass, LMmass, p_T, y_T, T, WDOT, RTCC_ENGINETYPE_CSMSPS, IC, 1, 0, 0.0);

	pad.ThisVehicleWeight = CSMmass / 0.45359237;
	pad.OtherVehicleWeight = LMmass / 0.45359237;
	pad.PitchTrim = p_T * DEG + 2.15;
	pad.YawTrim = y_T * DEG - 0.95;
}

void RTCC::LMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool asc)
{
	double CSMmass, LMmass;

	if (asc)
	{
		if (!stricmp(v->GetClassName(), "ProjectApollo\\LEM") ||
			!stricmp(v->GetClassName(), "ProjectApollo/LEM")) {
			LEM *lem = (LEM *)v;
			LMmass = lem->GetAscentStageMass();
		}
		else
		{
			LMmass = v->GetMass();
		}
	}
	else
	{
		LMmass = v->GetMass();
	}

	CSMmass = GetDockedVesselMass(v);

	pad.ThisVehicleWeight = LMmass / 0.45359237;
	pad.OtherVehicleWeight = CSMmass / 0.45359237;
	pad.PitchTrim = 6.0;
	pad.YawTrim = 6.0;
}

void RTCC::EarthOrbitEntry(const EarthEntryPADOpt &opt, AP7ENT &pad)
{
	double EMSAlt, theta_T, m1,v_e, EIAlt, lat, lng, KTETA;
	double dt;//from SV time to deorbit maneuver
	double t_go; //from deorbit TIG to shutdown
	double dt2; //from shutdown to EI
	double dt3; //from EI to 300k
	double dt4; //from 300k to 0.05g
	VECTOR3 UX, UY, UZ, DV_P, DV_C, V_G, R2B, V2B, R05G, V05G, EIangles, REI, VEI, R300K, V300K;
	VECTOR3 UXD, UYD, UZD;
	MATRIX3 M_R;

	double ALFATRIM = -20.0*RAD;

	EMSAlt = 284643.0*0.3048;
	EIAlt = 400000.0*0.3048;

	KTETA = 1000.0;

	if (opt.preburn)
	{
		SV sv1;
		double F, m_cut, EntryRTGO, EntryVIO, EntryRET;

		F = SPS_THRUST;
		v_e = SPS_ISP;

		dt = opt.P30TIG - (opt.sv0.MJD - opt.GETbase) * 24.0 * 60.0 * 60.0;
		sv1 = coast(opt.sv0, dt);

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		DV_P = UX*opt.dV_LVLH.x + UZ*opt.dV_LVLH.z;
		theta_T = length(crossp(sv1.R, sv1.V))*length(opt.dV_LVLH)*sv1.mass / OrbMech::power(length(sv1.R), 2.0) / SPS_THRUST;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*opt.dV_LVLH.y;
		OrbMech::poweredflight(sv1.R, sv1.V, sv1.MJD, sv1.gravref, F, v_e, sv1.mass, V_G, R2B, V2B, m_cut, t_go);

		dt2 = OrbMech::time_radius_integ(R2B, V2B, sv1.MJD + t_go / 3600.0 / 24.0, OrbMech::R_Earth + EIAlt, -1, sv1.gravref, sv1.gravref, REI, VEI);
		dt3 = OrbMech::time_radius_integ(REI, VEI, sv1.MJD + (t_go + dt2) / 3600.0 / 24.0, OrbMech::R_Earth + 300000.0*0.3048, -1, sv1.gravref, sv1.gravref, R300K, V300K);
		dt4 = OrbMech::time_radius_integ(R300K, V300K, sv1.MJD + (t_go + dt2 + dt3) / 3600.0 / 24.0, OrbMech::R_Earth + EMSAlt, -1, sv1.gravref, sv1.gravref, R05G, V05G);

		EntryCalculations::Reentry(REI, VEI, sv1.MJD + (t_go + dt2) / 3600.0 / 24.0, false, lat, lng, EntryRTGO, EntryVIO, EntryRET);

		UX = unit(-V05G);
		UY = unit(crossp(UX, -R05G));
		UZ = unit(crossp(UX, crossp(UX, -R05G)));

		UYD = UY;
		UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
		UZD = crossp(UXD, UYD);

		M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
		EIangles = OrbMech::CALCGAR(opt.REFSMMAT, M_R);

		m1 = sv1.mass*exp(-length(opt.dV_LVLH) / v_e);

		double WIE, WT, theta_rad, LSMJD;
		VECTOR3 RTE, UTR, urh, URT0, URT, R_LS, R_P;
		MATRIX3 Rot2;
		LSMJD = (t_go + dt2 + dt3 + dt4) / 24.0 / 3600.0 + sv1.MJD;
		R_P = unit(_V(cos(opt.lng)*cos(opt.lat), sin(opt.lat), sin(opt.lng)*cos(opt.lat)));
		Rot2 = OrbMech::GetRotationMatrix(BODY_EARTH, LSMJD);
		R_LS = mul(Rot2, R_P);
		//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
		URT0 = R_LS;
		WIE = 72.9211505e-6;
		UZ = _V(0, 0, 1);
		RTE = crossp(UZ, URT0);
		UTR = crossp(RTE, UZ);
		urh = unit(R05G);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
		theta_rad = acos(dotp(URT0, urh));
		for (int i = 0;i < 10;i++)
		{
			WT = WIE*(KTETA*theta_rad);
			URT = URT0 + UTR*(cos(WT) - 1.0) + RTE*sin(WT);
			theta_rad = acos(dotp(URT, urh));
		}

		pad.Att400K[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
		pad.dVTO[0] = 0.0;//-60832.18 / m1 / 0.3048;
		if (opt.lat == 0)
		{
			pad.Lat[0] = lat*DEG;
			pad.Lng[0] = lng*DEG;
		}
		else
		{
			pad.Lat[0] = opt.lat*DEG;
			pad.Lng[0] = opt.lng*DEG;
		}
		pad.Ret05[0] = t_go + dt2 + dt3 + dt4;
		pad.RTGO[0] = theta_rad*3437.7468;//entry->EntryRTGO;
		pad.VIO[0] = EntryVIO / 0.3048;
	}
	else
	{
		double EMSTime, LSMJD, dt5, theta_rad, theta_nm;
		VECTOR3 R_P, R_LS;
		MATRIX3 Rot2;

		dt = opt.P30TIG - (opt.sv0.MJD - opt.GETbase) * 24.0 * 60.0 * 60.0;
		dt2 = OrbMech::time_radius_integ(opt.sv0.R, opt.sv0.V, opt.sv0.MJD, OrbMech::R_Earth + EIAlt, -1, opt.sv0.gravref, opt.sv0.gravref, REI, VEI);
		dt3 = OrbMech::time_radius_integ(REI, VEI, opt.sv0.MJD + dt2 / 24.0 / 3600.0, OrbMech::R_Earth + 300000.0*0.3048, -1, opt.sv0.gravref, opt.sv0.gravref, R300K, V300K);
		dt4 = OrbMech::time_radius_integ(R300K, V300K, opt.sv0.MJD + (dt2 + dt3) / 24.0 / 3600.0, OrbMech::R_Earth + EMSAlt, -1, opt.sv0.gravref, opt.sv0.gravref, R05G, V05G);

		UX = unit(-V05G);
		UY = unit(crossp(UX, -R05G));
		UZ = unit(crossp(UX, crossp(UX, -R05G)));

		UYD = UY;
		UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
		UZD = crossp(UXD, UYD);

		M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
		EIangles = OrbMech::CALCGAR(opt.REFSMMAT, M_R);

		dt5 = 500.0;
		EMSTime = dt2 + dt3 + dt4 + (opt.sv0.MJD - opt.GETbase) * 24.0 * 60.0 * 60.0;

		/*for (int i = 0;i < 10;i++)
		{
			LSMJD = (EMSTime + dt5) / 24.0 / 3600.0 + opt->GETbase;
			R_P = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)));
			Rot2 = OrbMech::GetRotationMatrix2(gravref, LSMJD);
			R_LS = mul(Rot2, R_P);
			R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
			theta_rad = acos(dotp(R_LS, unit(R300K)));
			theta_nm = 3437.7468*theta_rad;
			if (length(V300K) >= 26000.0*0.3048)
			{
				dt5 = theta_nm / 3.0;
			}
			else
			{
				dt5 = 8660.0*theta_nm / (length(V300K) / 0.3048);
			}
		}*/
		double WIE, WT;
		VECTOR3 RTE, UTR, urh, URT0, URT;
		LSMJD = EMSTime / 24.0 / 3600.0 + opt.GETbase;
		R_P = unit(_V(cos(opt.lng)*cos(opt.lat), sin(opt.lat), sin(opt.lng)*cos(opt.lat)));
		Rot2 = OrbMech::GetRotationMatrix(BODY_EARTH, LSMJD);
		R_LS = mul(Rot2, R_P);
		//R_LS = mul(Rot, _V(R_LS.x, R_LS.z, R_LS.y));
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);
		URT0 = R_LS;
		WIE = 72.9211505e-6;
		UZ = _V(0, 0, 1);
		RTE = crossp(UZ, URT0);
		UTR = crossp(RTE, UZ);
		urh = unit(R05G);//unit(r)*cos(theta) + crossp(unit(r), -unit(h_apo))*sin(theta);
		theta_rad = acos(dotp(URT0, urh));
		for (int i = 0;i < 10;i++)
		{
			WT = WIE*(KTETA*theta_rad);
			URT = URT0 + UTR*(cos(WT) - 1.0) + RTE*sin(WT);
			theta_rad = acos(dotp(URT, urh));
		}
		theta_nm = theta_rad*3437.7468;

		pad.PB_RTGO[0] = theta_nm;//-3437.7468*acos(dotp(unit(R300K), unit(R05G)));
		pad.PB_R400K[0] = EIangles.x*DEG;
		pad.PB_Ret05[0] = dt2 + dt3 + dt4 - dt;
		pad.PB_VIO[0] = length(V05G) / 0.3048;
	}
}

void RTCC::LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad)
{
	VECTOR3 UX, UY, UZ, EIangles, UREI;
	MATRIX3 M_R;
	double dt, dt2, dt3, EIAlt, Alt300K, EMSAlt, S_FPA, g_T, V_T, v_BAR, RET05, liftline, EntryPADV400k, EntryPADVIO;
	double LSMJD, theta_nm, EntryPADDO, EntryPADGMax, EntryPADgamma400k, EntryPADHorChkGET, EIGET, EntryPADHorChkPit;
	OBJHANDLE hEarth, hMoon;
	SV sv1;		// "Now" or just after the maneuver
	SV svEI;	// EI/400K
	SV sv300K;  // 300K
	SV sv05G;   // EMS Altitude / 0.05G

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	EIAlt = 400000.0*0.3048;
	Alt300K = 300000.0*0.3048;
	EMSAlt = 297431.0*0.3048;

	if (opt->direct || length(opt->dV_LVLH) == 0.0)	//Check against a DV of 0
	{
		sv1 = opt->sv0;
	}
	else
	{
		sv1 = ExecuteManeuver(opt->sv0, opt->GETbase, opt->P30TIG, opt->dV_LVLH, 0.0, RTCC_ENGINETYPE_CSMSPS);
	}

	if (sv1.gravref == hMoon)
	{
		double dt_r = OrbMech::time_radius(sv1.R, sv1.V, 64373760.0, 1.0, OrbMech::mu_Moon);
		sv1 = coast(sv1, dt_r);
	}

	dt = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, OrbMech::R_Earth + EIAlt, -1, sv1.gravref, hEarth, svEI.R, svEI.V);
	svEI.gravref = hEarth;
	svEI.mass = sv1.mass;
	svEI.MJD = sv1.MJD + dt / 24.0 / 3600.0;

	double EntryRET, lat, lng, rtgo, vio;

	EntryCalculations::Reentry(svEI.R, svEI.V, svEI.MJD, true, lat, lng, rtgo, vio, EntryRET);

	LSMJD = svEI.MJD + EntryRET / 24.0 / 3600.0;

	dt2 = OrbMech::time_radius_integ(svEI.R, svEI.V, svEI.MJD, OrbMech::R_Earth + Alt300K, -1, hEarth, hEarth, sv300K.R, sv300K.V);
	sv300K.gravref = hEarth;
	sv300K.mass = svEI.mass;
	sv300K.MJD = svEI.MJD + dt2 / 24.0 / 3600.0;

	dt3 = OrbMech::time_radius_integ(sv300K.R, sv300K.V, sv300K.MJD, OrbMech::R_Earth + EMSAlt, -1, hEarth, hEarth, sv05G.R, sv05G.V);
	sv05G.gravref = hEarth;
	sv05G.mass = sv300K.mass;
	sv05G.MJD = sv300K.MJD + dt3 / 24.0 / 3600.0;

	EntryPADVIO = length(sv05G.V);
	theta_nm = OrbMech::CMCEMSRangeToGo(sv05G.R, sv05G.MJD, opt->lat, opt->lng);

	UX = unit(-sv05G.V);
	UY = unit(crossp(UX, -sv05G.R));
	UZ = unit(crossp(UX, crossp(UX, -sv05G.R)));

	double ALFATRIM = -20.0*RAD;
	VECTOR3 UXD, UYD, UZD;

	UYD = UY;
	UXD = unit(crossp(UYD, UX))*sin(ALFATRIM) + UX*cos(ALFATRIM);
	UZD = crossp(UXD, UYD);

	M_R = _M(UXD.x, UXD.y, UXD.z, UYD.x, UYD.y, UYD.z, UZD.x, UZD.y, UZD.z);
	EIangles = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

	S_FPA = dotp(unit(sv300K.R), sv300K.V) / length(sv300K.V);
	g_T = asin(S_FPA);
	V_T = length(sv300K.V);
	v_BAR = (V_T / 0.3048 - 36000.0) / 20000.0;
	EntryPADGMax = 4.0 / (1.0 + 4.8*v_BAR*v_BAR)*(abs(g_T)*DEG - 6.05 - 2.4*v_BAR*v_BAR) + 10.0;

	UREI = unit(svEI.R);
	EntryPADV400k = length(svEI.V);
	S_FPA = dotp(UREI, svEI.V) / EntryPADV400k;
	EntryPADgamma400k = asin(S_FPA);
	EIGET = (svEI.MJD - opt->GETbase)*24.0*3600.0;
	RET05 = (sv05G.MJD - svEI.MJD)*24.0*3600.0;

	double vei;
	vei = length(svEI.V) / 0.3048;

	EntryPADDO = 4.2317708e-9*vei*vei + 1.4322917e-6*vei - 1.600664062;

	liftline = 4.055351e-10*vei*vei - 3.149125e-5*vei + 0.503280635;
	if (S_FPA > atan(liftline))
	{
		sprintf(pad.LiftVector[0], "DN");
	}
	else
	{
		sprintf(pad.LiftVector[0], "UP");
	}

	SV svHorCheck, svSxtCheck;
	svHorCheck = coast(svEI, -17.0*60.0);
	svSxtCheck = coast(svEI, -60.0*60.0);

	EntryPADHorChkGET = EIGET - 17.0*60.0;

	double Entrytrunnion, Entryshaft, EntryBSSpitch, EntryBSSXPos;
	int Entrystaroct, EntryCOASstaroct;
	OrbMech::checkstar(opt->REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), svSxtCheck.R, OrbMech::R_Earth, Entrystaroct, Entrytrunnion, Entryshaft);
	OrbMech::coascheckstar(opt->REFSMMAT, _V(OrbMech::round(EIangles.x*DEG)*RAD, OrbMech::round(EIangles.y*DEG)*RAD, OrbMech::round(EIangles.z*DEG)*RAD), svSxtCheck.R, OrbMech::R_Earth, EntryCOASstaroct, EntryBSSpitch, EntryBSSXPos);

	double horang, coastang, IGA, cosIGA, sinIGA;
	VECTOR3 X_NB, Y_NB, Z_NB, X_SM, Y_SM, Z_SM, A_MG;

	horang = asin(OrbMech::R_Earth / length(svHorCheck.R));
	coastang = dotp(unit(svEI.R), unit(svHorCheck.R));

	Z_NB = unit(-svEI.R);
	Y_NB = unit(crossp(svEI.V, svEI.R));
	X_NB = crossp(Y_NB, Z_NB);

	X_SM = _V(opt->REFSMMAT.m11, opt->REFSMMAT.m12, opt->REFSMMAT.m13);
	Y_SM = _V(opt->REFSMMAT.m21, opt->REFSMMAT.m22, opt->REFSMMAT.m23);
	Z_SM = _V(opt->REFSMMAT.m31, opt->REFSMMAT.m32, opt->REFSMMAT.m33);
	A_MG = unit(crossp(X_NB, Y_SM));
	cosIGA = dotp(A_MG, Z_SM);
	sinIGA = dotp(A_MG, X_SM);
	IGA = atan2(sinIGA, cosIGA);

	EntryPADHorChkPit = PI2 - (horang + coastang + 31.7*RAD) + IGA;

	pad.Att05[0] = _V(OrbMech::imulimit(EIangles.x*DEG), OrbMech::imulimit(EIangles.y*DEG), OrbMech::imulimit(EIangles.z*DEG));
	pad.BSS[0] = EntryCOASstaroct;
	pad.DLMax[0] = 0.0;
	pad.DLMin[0] = 0.0;
	pad.DO[0] = EntryPADDO;
	pad.Gamma400K[0] = EntryPADgamma400k*DEG;
	pad.GETHorCheck[0] = EntryPADHorChkGET;
	pad.Lat[0] = opt->lat*DEG;
	pad.Lng[0] = opt->lng*DEG;
	pad.MaxG[0] = EntryPADGMax;
	pad.PitchHorCheck[0] = OrbMech::imulimit(EntryPADHorChkPit*DEG);
	pad.RET05[0] = RET05;
	pad.RETBBO[0] = 0.0;
	pad.RETDRO[0] = 0.0;
	pad.RETEBO[0] = 0.0;
	pad.RETVCirc[0] = 0.0;
	pad.RRT[0] = EIGET;
	pad.RTGO[0] = theta_nm;
	pad.SXTS[0] = Entrystaroct;
	pad.SFT[0] = Entryshaft*DEG;
	pad.TRN[0] = Entrytrunnion*DEG;
	pad.SPA[0] = EntryBSSpitch*DEG;
	pad.SXP[0] = EntryBSSXPos*DEG;
	pad.V400K[0] = EntryPADV400k / 0.3048;
	pad.VIO[0] = EntryPADVIO / 0.3048;
	pad.VLMax[0] = 0.0;
	pad.VLMin[0] = 0.0;
}

MATRIX3 RTCC::GetREFSMMATfromAGC(agc_t *agc, double AGCEpoch, int addroff)
{
	MATRIX3 REFSMMAT;
	char Buffer[100];
	int REFSMMAToct[20];

	int REFSaddr = 01735 + addroff;

	unsigned short REFSoct[20];
	REFSoct[2] = agc->Erasable[0][REFSaddr];
	REFSoct[3] = agc->Erasable[0][REFSaddr + 1];
	REFSoct[4] = agc->Erasable[0][REFSaddr + 2];
	REFSoct[5] = agc->Erasable[0][REFSaddr + 3];
	REFSoct[6] = agc->Erasable[0][REFSaddr + 4];
	REFSoct[7] = agc->Erasable[0][REFSaddr + 5];
	REFSoct[8] = agc->Erasable[0][REFSaddr + 6];
	REFSoct[9] = agc->Erasable[0][REFSaddr + 7];
	REFSoct[10] = agc->Erasable[0][REFSaddr + 8];
	REFSoct[11] = agc->Erasable[0][REFSaddr + 9];
	REFSoct[12] = agc->Erasable[0][REFSaddr + 10];
	REFSoct[13] = agc->Erasable[0][REFSaddr + 11];
	REFSoct[14] = agc->Erasable[0][REFSaddr + 12];
	REFSoct[15] = agc->Erasable[0][REFSaddr + 13];
	REFSoct[16] = agc->Erasable[0][REFSaddr + 14];
	REFSoct[17] = agc->Erasable[0][REFSaddr + 15];
	REFSoct[18] = agc->Erasable[0][REFSaddr + 16];
	REFSoct[19] = agc->Erasable[0][REFSaddr + 17];
	for (int i = 2; i < 20; i++)
	{
		sprintf(Buffer, "%05o", REFSoct[i]);
		REFSMMAToct[i] = atoi(Buffer);
	}
	
	REFSMMAT.m11 = OrbMech::DecToDouble(REFSoct[2], REFSoct[3])*2.0;
	REFSMMAT.m12 = OrbMech::DecToDouble(REFSoct[4], REFSoct[5])*2.0;
	REFSMMAT.m13 = OrbMech::DecToDouble(REFSoct[6], REFSoct[7])*2.0;
	REFSMMAT.m21 = OrbMech::DecToDouble(REFSoct[8], REFSoct[9])*2.0;
	REFSMMAT.m22 = OrbMech::DecToDouble(REFSoct[10], REFSoct[11])*2.0;
	REFSMMAT.m23 = OrbMech::DecToDouble(REFSoct[12], REFSoct[13])*2.0;
	REFSMMAT.m31 = OrbMech::DecToDouble(REFSoct[14], REFSoct[15])*2.0;
	REFSMMAT.m32 = OrbMech::DecToDouble(REFSoct[16], REFSoct[17])*2.0;
	REFSMMAT.m33 = OrbMech::DecToDouble(REFSoct[18], REFSoct[19])*2.0;
	
	return mul(REFSMMAT, OrbMech::J2000EclToBRCS(AGCEpoch));
}

double RTCC::GetClockTimeFromAGC(agc_t *agc)
{
	return agc->Erasable[AGC_BANK(025)][AGC_ADDR(025)] + agc->Erasable[AGC_BANK(024)][AGC_ADDR(024)] * pow((double) 2., (double) 14.);
}

double RTCC::GetTEPHEMFromAGC(agc_t *agc)
{
	return agc->Erasable[AGC_BANK(01710)][AGC_ADDR(01710)] +
		agc->Erasable[AGC_BANK(01707)][AGC_ADDR(01707)] * pow((double) 2., (double) 14.) +
		agc->Erasable[AGC_BANK(01706)][AGC_ADDR(01706)] * pow((double) 2., (double) 28.);
}

void RTCC::navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt)
{
	//R and V in the J2000

	MATRIX3 Rot2;
	VECTOR3 Requ, Recl, u;
	double sinl, a, b, gamma,r_0;

	a = 6378166;
	b = 6356784;

	if (gravref == oapiGetObjectByName("Earth"))
	{
		gamma = b * b / a / a;
		r_0 = OrbMech::R_Earth;
		Rot2 = OrbMech::GetRotationMatrix(BODY_EARTH, MJD);
	}
	else
	{
		gamma = 1;
		r_0 = OrbMech::R_Moon;
		Rot2 = OrbMech::GetRotationMatrix(BODY_MOON, MJD);
	}

	Recl = _V(R.x, R.z, R.y);

	Requ = tmul(Rot2, Recl);
	Requ = _V(Requ.x, Requ.z, Requ.y);

	u = unit(Requ);
	sinl = u.z;

	lat = atan(u.z/(gamma*sqrt(u.x*u.x + u.y*u.y)));
	lng = atan2(u.y, u.x);
	alt = length(Requ) - r_0;
}

SV RTCC::StateVectorCalc(VESSEL *vessel, double SVMJD)
{
	VECTOR3 R, V;
	double dt;
	OBJHANDLE gravref;
	SV sv, sv1;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	sv.MJD = oapiGetSimMJD();

	sv.R = _V(R.x, R.z, R.y);
	sv.V = _V(V.x, V.z, V.y);

	sv.gravref = gravref;
	sv.mass = vessel->GetMass();

	if (SVMJD != 0.0)
	{
		dt = (SVMJD - sv.MJD)*24.0*3600.0;
		sv1 = coast(sv, dt);
	}
	else
	{
		sv1 = sv;
	}

	return sv1;
}

EphemerisData RTCC::StateVectorCalcEphem(VESSEL *vessel, double SVGMT)
{
	VECTOR3 R, V;
	double MJD;
	OBJHANDLE gravref;
	EphemerisData sv;

	gravref = AGCGravityRef(vessel);

	vessel->GetRelativePos(gravref, R);
	vessel->GetRelativeVel(gravref, V);
	MJD = oapiGetSimMJD();

	sv.R = _V(R.x, R.z, R.y);
	sv.V = _V(V.x, V.z, V.y);
	sv.GMT = OrbMech::GETfromMJD(MJD, GMTBASE);
	if (gravref == hEarth)
	{
		sv.RBI = BODY_EARTH;
	}
	else
	{
		sv.RBI = BODY_MOON;
	}
	return sv;
}

OBJHANDLE RTCC::AGCGravityRef(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 rsph;

	gravref = oapiGetObjectByName("Moon");
	vessel->GetRelativePos(gravref, rsph);
	if (length(rsph) > 64373760.0)
	{
		gravref = oapiGetObjectByName("Earth");
	}
	return gravref;
}

double RTCC::getGETBase()
{
	double GET, SVMJD;
	SVMJD = oapiGetSimMJD();
	GET = mcc->cm->GetMissionTime();
	return SVMJD - GET / 24.0 / 3600.0;
}

double RTCC::CalcGETBase()
{
	return GMTBASE + MCGMTL / 24.0;
}

double RTCC::GETfromGMT(double GMT)
{
	return GMT - MCGMTL * 3600.0;
}

double RTCC::GMTfromGET(double GET)
{
	return GET + MCGMTL * 3600.0;
}

MATRIX3 RTCC::REFSMMATCalc(REFSMMATOpt *opt)
{
	VECTOR3 UX, UY, UZ;
	OBJHANDLE hMoon, hEarth;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	//Here the options that don't require a state vector or thrust parameters
	if (opt->REFSMMATopt == 7)
	{
		MATRIX3 M;
		VECTOR3 X_NB, Y_NB, Z_NB, X_NB_apo, Y_NB_apo, Z_NB_apo;

		M = OrbMech::CALCSMSC(opt->IMUAngles);
		X_NB_apo = _V(M.m11, M.m12, M.m13);
		Y_NB_apo = _V(M.m21, M.m22, M.m23);
		Z_NB_apo = _V(M.m31, M.m32, M.m33);
		X_NB = tmul(opt->PresentREFSMMAT, X_NB_apo);
		Y_NB = tmul(opt->PresentREFSMMAT, Y_NB_apo);
		Z_NB = tmul(opt->PresentREFSMMAT, Z_NB_apo);

		return _M(X_NB.x, X_NB.y, X_NB.z, Y_NB.x, Y_NB.y, Y_NB.z, Z_NB.x, Z_NB.y, Z_NB.z);
	}
	else if (opt->REFSMMATopt == 4)
	{
		//For now a default LC-39A, 72° launch
		return OrbMech::LaunchREFSMMAT(28.608202*RAD, -80.604064*RAD, opt->GETbase, 72 * RAD);
	}
	else if (opt->REFSMMATopt == 6)
	{
		double MoonPos[12];
		double PTCMJD;
		VECTOR3 R_ME;

		PTCMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		CELBODY *cMoon = oapiGetCelbodyInterface(hMoon);

		cMoon->clbkEphemeris(PTCMJD, EPHEM_TRUEPOS, MoonPos);

		R_ME = -_V(MoonPos[0], MoonPos[1], MoonPos[2]);

		UX = unit(crossp(_V(0.0, 1.0, 0.0), unit(R_ME)));
		//UX = mul(Rot, _V(UX.x, UX.z, UX.y));
		UX = _V(UX.x, UX.z, UX.y);

		UZ = _V(0.0, 0.0, -1.0);
		UY = crossp(UZ, UX);
		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else if (opt->REFSMMATopt == 8)
	{
		MATRIX3 Rot2;
		VECTOR3 R_P, R_LS, R_LPO, V_LPO, H_LPO, axis, H_C;
		double LSMJD;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat)));
		LSMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;
		Rot2 = OrbMech::GetRotationMatrix(BODY_MOON, LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);

		OrbMech::adbar_from_rv(1.0, 1.0, opt->LSLng, opt->LSLat, PI05, opt->LSAzi, R_LPO, V_LPO);
		H_LPO = crossp(R_LPO, V_LPO);
		axis = unit(H_LPO);
		H_C = unit(rhmul(Rot2, axis));

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}

	double dt, LMmass;
	VECTOR3 X_B;
	VECTOR3 DV_P, DV_C, V_G, X_SM, Y_SM, Z_SM;
	double theta_T;
	SV sv0, sv2;
	THGROUP_TYPE th_main;

	if (opt->vessel->GetGroupThruster(THGROUP_MAIN, 0) == NULL)
	{
		th_main = THGROUP_HOVER;
	}
	else
	{
		th_main = THGROUP_MAIN;
	}

	if (opt->useSV)
	{
		sv0 = opt->RV_MCC;
	}
	else
	{
		sv0 = StateVectorCalc(opt->vessel);
	}

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	sv2 = sv0;

	if (opt->REFSMMATopt == 5)
	{
		double LSMJD;
		VECTOR3 R_P, R_LS, H_C;
		MATRIX3 Rot2;
		SV sv3;

		LSMJD = opt->REFSMMATTime / 24.0 / 3600.0 + opt->GETbase;

		R_P = unit(_V(cos(opt->LSLng)*cos(opt->LSLat), sin(opt->LSLat), sin(opt->LSLng)*cos(opt->LSLat)));

		Rot2 = OrbMech::GetRotationMatrix(BODY_MOON, LSMJD);

		R_LS = mul(Rot2, R_P);
		R_LS = _V(R_LS.x, R_LS.z, R_LS.y);

		OrbMech::oneclickcoast(sv2.R, sv2.V, sv2.MJD, (LSMJD - sv2.MJD)*24.0*3600.0, sv3.R, sv3.V, sv2.gravref, hMoon);
		sv3.gravref = hMoon;
		sv3.mass = sv2.mass;
		H_C = crossp(sv3.R, sv3.V);

		UX = unit(R_LS);
		UZ = unit(crossp(H_C, R_LS));
		UY = crossp(UZ, UX);

		return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	}
	else
	{
		SV sv4;

		if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1 || opt->REFSMMATopt == 2)
		{
			dt = opt->REFSMMATTime - (sv2.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
			sv4 = coast(sv2, dt);
		}
		else
		{
			dt = OrbMech::time_radius_integ(sv2.R, sv2.V, sv2.MJD, OrbMech::R_Earth + 400000.0*0.3048, -1, sv2.gravref, hEarth, sv4.R, sv4.V);
		}

		if (opt->REFSMMATopt == 0 || opt->REFSMMATopt == 1)
		{
			UY = unit(crossp(sv4.V, sv4.R));
			UZ = unit(-sv4.R);
			UX = crossp(UY, UZ);

			double headsswitch;

			if (opt->HeadsUp)
			{
				headsswitch = 1.0;
			}
			else
			{
				headsswitch = -1.0;
			}

			DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
			if (length(DV_P) != 0.0)
			{
				theta_T = length(crossp(sv4.R, sv4.V))*length(opt->dV_LVLH)*(sv4.mass + LMmass) / OrbMech::power(length(sv4.R), 2.0) / SPS_THRUST;
				DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
				V_G = DV_C + UY*opt->dV_LVLH.y;
			}
			else
			{
				V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
			}

			if (opt->vesseltype < 2)
			{
				MATRIX3 M, M_R, M_RTM;
				double p_T, y_T;

				double T, WDOT;
				unsigned IC;
				if (LMmass > 0)
				{
					IC = 13;
				}
				else
				{
					IC = 1;
				}
				GIMGBL(sv4.mass, LMmass, p_T, y_T, T, WDOT, RTCC_ENGINETYPE_CSMSPS, IC, 1, 0, 0.0);

				if (opt->REFSMMATopt == 0)
				{
					X_B = unit(V_G);
				}
				else
				{
					headsswitch = 1.0;
					p_T = 2.15*RAD;
					X_B = -unit(V_G);
				}
				UX = X_B;
				UY = unit(crossp(UX, sv4.R*headsswitch));
				UZ = unit(crossp(UX, UY));

				M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
				M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
				M_RTM = mul(OrbMech::tmat(M_R), M);
				X_SM = mul(M_RTM, _V(1.0, 0.0, 0.0));
				Y_SM = mul(M_RTM, _V(0.0, 1.0, 0.0));
				Z_SM = mul(M_RTM, _V(0.0, 0.0, 1.0));
			}
			else
			{
				VECTOR3 U_FDI;

				U_FDI = unit(V_G);

				if (opt->REFSMMATopt == 0)
				{
					X_SM = U_FDI;
				}
				else
				{
					X_SM = -U_FDI;
				}
				if (abs(dotp(X_SM, unit(sv4.R))) < cos(0.01*RAD))
				{
					Y_SM = unit(crossp(X_SM, -sv4.R*headsswitch));
				}
				else
				{
					Y_SM = unit(crossp(X_SM, sv4.V));
				}
				Z_SM = unit(crossp(X_SM, Y_SM));
			}

			return _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

			//IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(transpose_matrix(M), M_R));
			//sprintf(oapiDebugString(), "%f, %f, %f", IMUangles.x*DEG, IMUangles.y*DEG, IMUangles.z*DEG);
		}
		else
		{
			if (opt->vesseltype < 2)
			{
				UY = unit(crossp(sv4.V, sv4.R));
				UZ = unit(-sv4.R);
				UX = crossp(UY, UZ);
			}
			else
			{
				UX = unit(sv4.R);
				UY = unit(crossp(sv4.V, sv4.R));
				UZ = unit(crossp(UX, UY));
			}

			return _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		}
		//
	}
}

double RTCC::FindDH(MPTSV sv_A, MPTSV sv_P, double GETbase, double TIGguess, double DH)
{
	SV sv_A1, sv_P1;
	double dt, dt2, t_A, t_P, SVMJD, c1, c2, dt2_apo, CDHtime_cor;
	int n = 0;

	t_A = OrbMech::GETfromMJD(sv_A.MJD, GETbase);
	t_P = OrbMech::GETfromMJD(sv_P.MJD, GETbase);
	sv_A1 = coast(sv_A, TIGguess - t_A);
	sv_P1 = coast(sv_P, TIGguess - t_P);
	SVMJD = GETbase + TIGguess / 24.0 / 3600.0;

	dt = 0.0;
	dt2 = dt + 10.0;
	
	//A secant search method is used to find the time, when the desired delta height is reached. Other values might work better.
	while (abs(dt2 - dt) > 0.1 && n <= 20)					//0.1 seconds accuracy should be enough
	{
		c1 = OrbMech::NSRsecant(sv_A1.R, sv_A1.V, sv_P1.R, sv_P1.V, SVMJD, dt, DH, sv_A1.gravref);		//c is the difference between desired and actual DH
		c2 = OrbMech::NSRsecant(sv_A1.R, sv_A1.V, sv_P1.R, sv_P1.V, SVMJD, dt2, DH, sv_A1.gravref);

		dt2_apo = dt2 - (dt2 - dt) / (c2 - c1)*c2;						//secant method
		dt = dt2;
		dt2 = dt2_apo;
		n++;
	}

	CDHtime_cor = dt2 + (SVMJD - GETbase) * 24 * 60 * 60;		//the new, calculated CDH time
	return CDHtime_cor;
}

double RTCC::TPISearch(SV sv_A, SV sv_P, double GETbase, double elev)
{
	SV sv_P1;
	double dt;

	sv_P1 = coast(sv_P, (sv_A.MJD - sv_P.MJD)*24.0*3600.0);
	dt = OrbMech::findelev(sv_A.R, sv_A.V, sv_P1.R, sv_P1.V, sv_A.MJD, elev, sv_A.gravref);

	return OrbMech::GETfromMJD(sv_A.MJD + dt / 24.0 / 3600.0, GETbase);
}

void RTCC::DOITargeting(DOIMan *opt, VECTOR3 &dv, double &P30TIG)
{
	double CR, t_L, t_PDI;

	DOITargeting(opt, dv, P30TIG, t_PDI, t_L, CR);
}

void RTCC::DOITargeting(DOIMan *opt, VECTOR3 &DV, double &P30TIG, double &t_PDI, double &t_L, double &CR)
{
	double GET, h_DP, theta_F, t_F, t_DOI;
	VECTOR3 R_LSA;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");

	GET = (opt->sv0.MJD - opt->GETbase)*24.0*3600.0;

	R_LSA = _V(cos(opt->lng)*cos(opt->lat), sin(opt->lng)*cos(opt->lat), sin(opt->lat))*opt->R_LLS;
	h_DP = opt->PeriAlt;
	theta_F = opt->PeriAng;
	t_F = 718.0;

	if (opt->opt == 0)
	{
		OrbMech::LunarLandingPrediction(opt->sv0.R, opt->sv0.V, GET, opt->EarliestGET, R_LSA, h_DP, theta_F, t_F, hMoon, opt->GETbase, OrbMech::mu_Moon, opt->N, t_DOI, t_PDI, t_L, DV, CR);
	}
	else
	{
		OrbMech::LunarLandingPrediction2(opt->sv0.R, opt->sv0.V, GET, opt->EarliestGET, R_LSA, h_DP, 1.0, theta_F, t_F, hMoon, opt->GETbase, OrbMech::mu_Moon, opt->N, t_DOI, t_PDI, t_L, DV, CR);
	}

	P30TIG = t_DOI;
}

void RTCC::PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	SV sv_pre, sv_post;

	PlaneChangeTargeting(opt, dV_LVLH, P30TIG, sv_pre, sv_post);
}

void RTCC::PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_pre, SV &sv_post)
{
	double cosA, sinA, A, dt, MJD_A, lng, lat, rad, GET, mu, LMmass, mass;
	MATRIX3 Rot;
	VECTOR3 RLS, n1, u, n2, V_PC2, DV, loc;
	SV sv0, sv1, sv_PC;

	sv0 = opt->RV_MCC;

	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;
	MJD_A = opt->GETbase + opt->t_A / 24.0 / 3600.0;
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD_A);
	mu = GGRAV*oapiGetMass(sv0.gravref);

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	if (opt->landed)
	{
		opt->target->GetEquPos(lng, lat, rad);
		loc = unit(_V(cos(lng)*cos(lat), sin(lat), sin(lng)*cos(lat)))*rad;
	}
	else
	{
		loc = unit(_V(cos(opt->lng)*cos(opt->lat), sin(opt->lat), sin(opt->lng)*cos(opt->lat)))*oapiGetSize(sv0.gravref);
	}

	RLS = mul(Rot, loc);
	RLS = _V(RLS.x, RLS.z, RLS.y);
	sv1 = coast(sv0, opt->EarliestGET - GET);

	n1 = unit(crossp(sv1.R, sv1.V));
	u = unit(crossp(-RLS, n1));
	cosA = dotp(u, unit(sv1.R));
	sinA = sqrt(1.0 - cosA*cosA);
	A = atan2(sinA, cosA);
	dt = OrbMech::time_theta(sv1.R, sv1.V, A, mu);
	sv_PC = coast(sv1, dt);
	n2 = unit(crossp(sv_PC.R, -RLS));
	V_PC2 = unit(crossp(n2, sv_PC.R))*length(sv_PC.V);

	DV = V_PC2 - sv_PC.V;

	PoweredFlightProcessor(sv_PC, opt->GETbase, opt->EarliestGET + dt, RTCC_ENGINETYPE_CSMSPS, LMmass, DV, false, P30TIG, dV_LVLH, sv_pre, sv_post);
}

int RTCC::LunarDescentPlanningProcessor(SV sv, double GETbase, double lat, double lng, double rad, LunarDescentPlanningTable &table)
{
	LDPPOptions opt;

	opt.azi_nom = med_k17.Azimuth;
	opt.GETbase = GETbase;
	opt.H_DP = med_k17.DescIgnHeight;
	opt.H_W = med_k16.DesiredHeight;
	opt.IDO = med_k16.Sequence - 2;
	if (med_k17.Azimuth != 0.0)
	{
		opt.I_AZ = 1;
	}
	else
	{
		opt.I_AZ = 0;
	}
	if (med_k17.PoweredDescSimFlag)
	{
		opt.I_PD = 1;
	}
	else
	{
		opt.I_PD = 0;
	}
	opt.I_SP = 0.0;
	opt.I_TPD = 0;
	opt.Lat_LS = lat;
	opt.Lng_LS = lng;
	opt.M = med_k17.DwellOrbits;
	opt.MODE = med_k16.Mode;
	opt.R_LS = rad;
	opt.sv0 = sv;
	opt.TH[0] = med_k16.GETTH1;
	opt.TH[1] = med_k16.GETTH2;
	opt.TH[2] = med_k16.GETTH3;
	opt.TH[3] = med_k16.GETTH4;
	opt.theta_D = med_k17.DescentFlightArc;
	opt.t_D = med_k17.DescentFlightTime;
	opt.T_PD = med_k17.PoweredDescTime;
	opt.W_LM = 0.0;

	LDPP ldpp;
	LDPPResults res;
	ldpp.Init(opt);
	int error = ldpp.LDPPMain(res);

	if (error) return error;

	//Store in PZLDPELM
	for (int i = 0;i < 4;i++)
	{
		PZLDPELM.sv_man_bef[i].R = res.sv_before[i].R;
		PZLDPELM.sv_man_bef[i].V = res.sv_before[i].V;
		PZLDPELM.sv_man_bef[i].GMT = OrbMech::GETfromMJD(res.sv_before[i].MJD, GMTBASE);
		if (res.sv_before[i].gravref == hEarth)
		{
			PZLDPELM.sv_man_bef[i].RBI = BODY_EARTH;
		}
		else
		{
			PZLDPELM.sv_man_bef[i].RBI = BODY_MOON;
		}
		
		PZLDPELM.V_man_after[i] = res.V_after[i];
		PZLDPELM.plan[i] = med_k16.Vehicle;
	}
	PZLDPELM.num_man = res.i;

	PMDLDPP(opt, res, table);

	PZLDPELM.code[0] = table.MVR[0];
	PZLDPELM.code[1] = table.MVR[1];
	PZLDPELM.code[2] = table.MVR[2];
	PZLDPELM.code[3] = table.MVR[3];

	return 0;
}

void RTCC::LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG)
{
	SV sv_node, sv_pre, sv_post;

	LOITargeting(opt, dV_LVLH, P30TIG, sv_node, sv_pre, sv_post);
}

void RTCC::LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node)
{
	SV sv_pre, sv_post;

	LOITargeting(opt, dV_LVLH, P30TIG, sv_node, sv_pre, sv_post);
}

void RTCC::LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node, SV &sv_pre, SV &sv_post)
{
	SV sv0, sv1;
	VECTOR3 axis, u_LPO, H, u_LAH, u_node, p, q, H_LPO;
	MATRIX3 Rot;
	double MJD_LAND, dt, dt_node, R_M, apo, peri, a, e, h, mass, LMmass, GET, r, GET_node;
	OBJHANDLE hMoon;

	sv0 = opt->RV_MCC;
	GET = (sv0.MJD - opt->GETbase)*24.0*3600.0;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0.0;
	}

	mass = LMmass + sv0.mass;

	hMoon = oapiGetObjectByName("Moon");
	sv_node.gravref = hMoon;
	sv_node.mass = sv0.mass;

	MJD_LAND = opt->GETbase + opt->t_land / 24.0 / 3600.0;
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD_LAND);

	//Lunar radius above LLS
	R_M = opt->R_LLS;

	//Initial guess for the node, at pericynthion
	dt = OrbMech::timetoperi_integ(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hMoon, sv1.R, sv1.V);
	sv1 = coast(sv0, dt);
	H = crossp(sv1.R, sv1.V);

	//Lunar Approach Hyperbola orientation
	u_LAH = unit(H);

	VECTOR3 R_LPO, V_LPO;
	double r_LPO, v_LPO;

	r_LPO = R_M + 60.0*1852.0;
	v_LPO = sqrt(OrbMech::mu_Moon / r_LPO);

	OrbMech::adbar_from_rv(r_LPO, v_LPO, opt->lng, opt->lat, PI05, opt->azi, R_LPO, V_LPO);

	if (opt->type == 0)
	{
		H_LPO = crossp(R_LPO, V_LPO);

		axis = unit(H_LPO);

		//Lunar Parking Orbit orientation
		u_LPO = unit(rhmul(Rot, axis));

		//Node unit vector
		u_node = unit(crossp(u_LPO, u_LAH));

		//Periapsis vector and node should be within 90°, if not the actual intersection is in the other direction
		if (dotp(u_node, unit(sv1.R)) < 0.0)
		{
			u_node = -u_node;
		}

		//state vector at the node
		dt_node = OrbMech::timetonode_integ(sv1.R, sv1.V, sv1.MJD, hMoon, u_node, sv_node.R, sv_node.V);
	}
	else
	{
		axis = unit(rhmul(Rot, R_LPO));

		//Lunar Parking Orbit orientation
		u_LPO = unit(crossp(axis, unit(sv1.R)));

		if (dotp(u_LPO, u_LAH) < 0.0)
		{
			u_LPO = -u_LPO;
		}

		u_node = unit(sv1.R);
		sv_node.R = sv1.R;
		sv_node.V = sv1.V;
		dt_node = 0.0;
	}

	//Time at node
	GET_node = GET + dt + dt_node;
	sv_node.MJD = opt->GETbase + GET_node / 24.0 / 3600.0;

	apo = R_M + opt->h_apo;
	peri = R_M + opt->h_peri;
	r = length(sv_node.R);

	if (r > apo)													//If the maneuver radius is higher than the desired apoapsis, then we would get no solution
	{
		apo = r;													//sets the desired apoapsis to the current radius, so that we can calculate a maneuver
	}
	else if (r < peri)												//If the maneuver radius is lower than the desired periapsis, then we would also get no solution
	{
		peri = r;													//sets the desired periapsis to the current radius, so that we can calculate a maneuver
	}

	a = (apo + peri) / 2.0;
	e = (apo - peri) / (apo + peri);
	h = sqrt(OrbMech::mu_Moon*a*(1.0 - e*e));

	double ta[2];
	VECTOR3 DV[2], R_ref[2], V_ref[2];
	MATRIX3 Q_Xx;
	int sol;

	ta[0] = acos(min(1.0, max(-1.0, (a / r*(1.0 - e*e) - 1.0) / e)));	//The true anomaly of the desired orbit, min and max just to make sure this value isn't out of bounds for acos
	ta[1] = PI2 - ta[0];												//Calculates the second possible true anomaly of the desired orbit

	for (int ii = 0;ii < 2;ii++)
	{

		p = OrbMech::RotateVector(u_LPO, -ta[ii], u_node);
		q = unit(crossp(u_LPO, p));

		R_ref[ii] = (p*cos(ta[ii]) + q*sin(ta[ii]))*h*h / OrbMech::mu_Moon / (1.0 + e*cos(ta[ii]));
		V_ref[ii] = (-p*sin(ta[ii]) + q*(e + cos(ta[ii])))*OrbMech::mu_Moon / h;

		DV[ii] = V_ref[ii] - sv_node.V;
	}

	if (opt->EllipseRotation == 0)
	{
		if (length(DV[1]) < length(DV[0]))
		{
			sol = 1;
		}
		else
		{
			sol = 0;
		}
	}
	else
	{
		if (opt->EllipseRotation == 1)
		{
			sol = 0;
		}
		else
		{
			sol = 1;
		}
	}

	if (opt->impulsive == 0)
	{
		VECTOR3 Llambda, R_cut, V_cut;
		double f_T, isp, t_slip, MJD_cut, m_cut;

		EngineParametersTable(opt->enginetype, f_T, isp);

		OrbMech::impulsive(sv_node.R, sv_node.V, sv_node.MJD, hMoon, f_T, isp, mass, R_ref[sol], V_ref[sol], Llambda, t_slip, R_cut, V_cut, MJD_cut, m_cut);
		sv_pre = coast(sv_node, t_slip);

		dV_LVLH = PIEXDV(sv_pre.R, sv_pre.V, mass, f_T, Llambda, false);
		P30TIG = GET_node + t_slip;

		sv_post = sv_node;
		sv_post.R = R_cut;
		sv_post.V = V_cut;
		sv_post.MJD = MJD_cut;
		sv_post.mass = m_cut - LMmass;
	}
	else
	{
		Q_Xx = OrbMech::LVLH_Matrix(sv_node.R, sv_node.V);

		dV_LVLH = mul(Q_Xx, DV[sol]);
		P30TIG = GET_node;

		sv_post = sv_node;
		sv_post.R = R_ref[sol];
		sv_post.V = V_ref[sol];

		PZLRBELM.sv_man_bef[0].R = sv_node.R;
		PZLRBELM.sv_man_bef[0].V = sv_node.V;
		PZLRBELM.sv_man_bef[0].GMT = OrbMech::GETfromMJD(sv_node.MJD, GMTBASE);
		PZLRBELM.sv_man_bef[0].RBI = BODY_MOON;
		PZLRBELM.V_man_after[0] = sv_post.V;
	}

}

void RTCC::TranslunarMidcourseCorrectionProcessor(SV sv0, double CSMmass, double LMmass)
{
	TLMCCDataTable datatab;
	TLMCCMEDQuantities medquant;
	TLMCCMissionConstants mccconst;
	TLMCCOutputData out;

	datatab = PZSFPTAB.blocks[PZMCCPLN.SFPBlockNum - 1];

	medquant.Mode = PZMCCPLN.Mode;
	medquant.Config = PZMCCPLN.Config;
	medquant.T_MCC = GMTfromGET(PZMCCPLN.MidcourseGET);
	medquant.GMTBase = GetGMTBase();
	medquant.GETBase = CalcGETBase();
	medquant.sv0 = sv0;
	medquant.CSMMass = CSMmass;
	medquant.LMMass = LMmass;
	medquant.H_pl = PZMCCPLN.h_PC;
	medquant.H_pl_mode5 = PZMCCPLN.h_PC_mode5;
	medquant.INCL_fr = PZMCCPLN.incl_fr;
	medquant.H_pl_min = PZMCCPLN.H_PCYN_MIN;
	medquant.H_pl_max = PZMCCPLN.H_PCYN_MAX;
	medquant.AZ_min = PZMCCPLN.AZ_min;
	medquant.AZ_max = PZMCCPLN.AZ_max;
	medquant.H_A_LPO1 = PZMCCPLN.H_A_LPO1;
	medquant.H_P_LPO1 = PZMCCPLN.H_P_LPO1;
	medquant.H_A_LPO2 = PZMCCPLN.H_A_LPO2;
	medquant.H_P_LPO2 = PZMCCPLN.H_P_LPO2;
	medquant.Revs_LPO1 = PZMCCPLN.REVS1;
	medquant.Revs_LPO2 = PZMCCPLN.REVS2;
	medquant.TA_LOI = PZMCCPLN.ETA1;
	medquant.site_rotation_LPO2 = PZMCCPLN.SITEROT;
	medquant.useSPS = true;
	medquant.T_min_sea = PZMCCPLN.TLMIN + MCGMTL;
	if (PZMCCPLN.TLMAX <= 0)
	{
		medquant.T_max_sea = 1000.0;
	}
	else
	{
		medquant.T_max_sea = PZMCCPLN.TLMAX + MCGMTL;
	}
	medquant.Revs_circ = 1;
	medquant.H_T_circ = 60.0*1852.0;
	medquant.lat_bias = PZMCCPLN.LATBIAS;

	mccconst.V_pcynlo = 5480.0*0.3048;
	mccconst.H_LPO = 60.0*1852.0;
	mccconst.lambda_IP = 190.0*RAD;
	mccconst.dt_bias = 0.332;
	mccconst.m = PZMCCPLN.LOPC_M;
	mccconst.n = PZMCCPLN.LOPC_N;
	mccconst.T_t1_min_dps = PZMCCPLN.TT1_DPS_MIN;
	mccconst.T_t1_max_dps = PZMCCPLN.TT1_DPS_MAX;
	mccconst.INCL_PR_MAX = PZMCCPLN.INCL_PR_MAX;

	TLMCCProcessor tlmcc;
	tlmcc.Init(&pzefem, datatab, medquant, mccconst);
	tlmcc.Main(out);

	//Update display data
	PZMCCDIS.data[PZMCCPLN.Column - 1] = out.display;

	//Update MPT transfer table
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].R = out.R_MCC;
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].V = out.V_MCC;
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].GMT = out.GMT_MCC;
	PZMCCXFR.sv_man_bef[PZMCCPLN.Column - 1].RBI = out.RBI;
	PZMCCXFR.V_man_after[PZMCCPLN.Column - 1] = out.V_MCC_apo;

	//Update skeleton flight plan table
	PZMCCSFP.blocks[PZMCCPLN.Column - 1] = out.outtab;
	PZMCCSFP.blocks[PZMCCPLN.Column - 1].GMTTimeFlag = RTCCPresentTimeGMT();
}

void RTCC::TranslunarMidcourseCorrectionTargetingNodal(MCCNodeMan &opt, TLMCCResults &res)
{
	MATRIX3 Rot, M_EMP;
	VECTOR3 R_selen, R, R_EMP, DV1, DV2, var_conv1, var_conv2, DV3;
	double dt1, dt2, NodeMJD;
	double lat_EMP, lng_EMP;
	OBJHANDLE hMoon;
	SV sv0, sv1, sv_tig, sv_peri2;

	hMoon = oapiGetObjectByName("Moon");
	sv0 = opt.RV_MCC;

	dt1 = opt.MCCGET - (sv0.MJD - opt.GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt.NodeGET - opt.MCCGET;

	sv1 = coast(sv0, dt1);

	NodeMJD = opt.NodeGET / 24.0 / 3600.0 + opt.GETbase;

	//Convert selenographic coordinates to EMP latitude and longitude

	R_selen = OrbMech::r_from_latlong(opt.lat, opt.lng);
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, NodeMJD);
	R = rhmul(Rot, R_selen);
	M_EMP = OrbMech::EMPMatrix(NodeMJD);
	R_EMP = mul(M_EMP, R);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	if (lng_EMP < 0.0)
	{
		lng_EMP += PI2;
	}

	//Step 1: Converge TLMC to the desired end conditions in order to provide good first guesses.
	TLMCFirstGuessConic(sv1, lat_EMP, opt.h_node, 0.0, NodeMJD, DV1, var_conv1);

	//Step 2: Converge integrated TLMC
	IntegratedTLMC(sv1, lat_EMP, opt.h_node, 0.0, NodeMJD, var_conv1, DV2, var_conv2, sv_peri2);

	//Step 3: Converge integrated XYZ and T trajectory
	TLMCIntegratedXYZT(sv1, lat_EMP, lng_EMP, opt.h_node, NodeMJD, DV2, DV3);

	res.TIG = OrbMech::GETfromMJD(sv1.MJD, opt.GETbase);
	res.DV = DV3;
	res.dV_LVLH_MCC = mul(OrbMech::LVLH_Matrix(sv1.R, sv1.V), res.DV);

	//TBD
	PZMCCXFR.sv_man_bef[0].R = sv1.R;
	PZMCCXFR.sv_man_bef[0].V = sv1.V;
	PZMCCXFR.sv_man_bef[0].GMT = OrbMech::GETfromMJD(sv1.MJD, GMTBASE);
	if (sv1.gravref == hEarth)
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_EARTH;
	}
	else
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_MOON;
	}
	PZMCCXFR.V_man_after[0] = sv1.V + res.DV;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingFreeReturn(MCCFRMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_reentry2, sv_peri3, sv_reentry3, sv_peri, sv_reentry, sv_peri5, sv_node3, sv_node;
	double dt1, dt2, PeriMJDguess, lat_EMP3;
	double r_peri3, v_peri3, lng_peri3, lat_peri3, fpav_peri3, azi_peri3;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 DV, DV1, var_conv1, DV2, DV3, R_EMP3, V_EMP3, DV5, var_converged5;
	SV sv_tig;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");
	
	sv0 = opt->RV_MCC;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);
	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, DV1, var_conv1);

	//Step 2: Converge conic, free-return trajectory
	TLMCFlybyConic(sv1, opt->lat, opt->h_peri, DV1, DV2, sv_peri2, sv_reentry2);

	//Step 3: Converge conic full mission
	if (!TLMCConic_BAP_FR_LPO(opt, sv1, opt->lat, opt->h_peri, DV2, DV3, sv_peri3, sv_node3, sv_reentry3, lat_EMP3))
	{
		return false;
	}

	OrbMech::EclToEMP(sv_peri3.R, sv_peri3.V, sv_peri3.MJD, R_EMP3, V_EMP3);
	OrbMech::rv_from_adbar(R_EMP3, V_EMP3, r_peri3, v_peri3, lng_peri3, lat_peri3, fpav_peri3, azi_peri3);

	//Step 5: Converge integrated TLMC to new targets
	IntegratedTLMC(sv1, lat_EMP3, opt->h_peri, 0.0, sv_peri3.MJD, _V(v_peri3, azi_peri3, lng_peri3), DV5, var_converged5, sv_peri5);

	//Step 6: Converge integrated full mission
	if (!TLMC_BAP_FR_LPO(opt, sv1, lat_EMP3, opt->h_peri, DV5, DV, sv_peri, sv_node, sv_reentry, res->EMPLatitude))
	{
		return false;
	}

	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	res->TIG = OrbMech::GETfromMJD(sv1.MJD, opt->GETbase);
	res->DV = DV;
	res->dV_LVLH_MCC = mul(OrbMech::LVLH_Matrix(sv1.R, sv1.V), res->DV);

	//TBD
	PZMCCXFR.sv_man_bef[0].R = sv1.R;
	PZMCCXFR.sv_man_bef[0].V = sv1.V;
	PZMCCXFR.sv_man_bef[0].GMT = OrbMech::GETfromMJD(sv1.MJD, GMTBASE);
	if (sv1.gravref == hEarth)
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_EARTH;
	}
	else
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_MOON;
	}
	PZMCCXFR.V_man_after[0] = sv1.V + res->DV;

	//Calculate nodal target
	OrbMech::latlong_from_J2000(sv_node.R, sv_node.MJD, sv_node.gravref, res->NodeLat, res->NodeLng);
	res->NodeAlt = length(sv_node.R) - OrbMech::R_Moon;
	res->NodeGET = (sv_node.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot_reentry;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot_reentry = OrbMech::GetRotationMatrix(BODY_EARTH, sv_reentry.MJD);
	R_geo = rhtmul(Rot_reentry, sv_reentry.R);
	V_geo = rhtmul(Rot_reentry, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = OrbMech::R_Earth + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, OrbMech::mu_Earth);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingNonFreeReturn(MCCNFRMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_node4, sv_node2;
	double dt1, dt2, PeriMJD, lat_EMP2, r_nd2, v_nd2, fpa_nd2, azi_nd2;
	double h_nd2, lat_nd2, lng_nd2, lat_nd4, lng_nd4, h_nd4;
	OBJHANDLE hMoon;
	VECTOR3 DV1, var_conv1, DV2, DV4, R_EMP2, V_EMP2, var_conv4, DV5, R_EMP4, V_EMP4;
	SV sv_tig;

	hMoon = oapiGetObjectByName("Moon");
	sv0 = opt->RV_MCC;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJD = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJD, DV1, var_conv1);

	//Step 2: Converge a conic non-free select full mission
	if (!TLMCConic_BAP_NFR_LPO(opt, sv1, opt->lat, opt->h_peri, PeriMJD, DV1, DV2, sv_peri2, sv_node2, lat_EMP2))
	{
		return false;
	}

	OrbMech::EclToEMP(sv_node2.R, sv_node2.V, sv_node2.MJD, R_EMP2, V_EMP2);

	OrbMech::rv_from_adbar(R_EMP2, V_EMP2, r_nd2, v_nd2, lng_nd2, lat_nd2, fpa_nd2, azi_nd2);
	h_nd2 = r_nd2 - OrbMech::R_Moon;

	//Step 4: Converge integrated TLMC
	IntegratedTLMC(sv1, lat_nd2, h_nd2, fpa_nd2 - PI05, sv_node2.MJD, _V(v_nd2, azi_nd2, lng_nd2), DV4, var_conv4, sv_node4);

	OrbMech::EclToEMP(sv_node4.R, sv_node4.V, sv_node4.MJD, R_EMP4, V_EMP4);
	OrbMech::latlong_from_r(R_EMP4, lat_nd4, lng_nd4);

	if (lng_nd4 < 0.0)
	{
		lng_nd4 += PI2;
	}

	h_nd4 = length(R_EMP4) - OrbMech::R_Moon;

	//Step 5: Converge precision trajectory to optimized LOI conditions
	TLMCIntegratedXYZT(sv1, lat_nd4, lng_nd4, h_nd4, sv_node4.MJD, DV4, DV5);

	//Calculate nodal target
	OrbMech::latlong_from_J2000(sv_node4.R, sv_node4.MJD, sv_node4.gravref, res->NodeLat, res->NodeLng);
	res->NodeAlt = length(sv_node4.R) - OrbMech::R_Moon;
	res->NodeGET = (sv_node4.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate LOI DV
	LOIMan loiopt;
	double TIG_LOI, MJD_meridian;
	SV sv_node6, sv_preLOI, sv_postLOI, sv_postLOI2;

	sv_node4.mass = sv1.mass;

	loiopt.R_LLS = opt->R_LLS;
	loiopt.azi = opt->azi;
	loiopt.csmlmdocked = false;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.impulsive = 1;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.RV_MCC = sv_node4;
	loiopt.type = opt->type;
	loiopt.t_land = opt->t_land;
	loiopt.vessel = NULL;
	loiopt.enginetype = 0;
	loiopt.EllipseRotation = opt->LOIEllipseRotation;

	LOITargeting(&loiopt, res->dV_LVLH_LOI, TIG_LOI, sv_node6, sv_preLOI, sv_postLOI);
	sv_postLOI2 = coast(sv_postLOI, 3600.0);
	MJD_meridian = OrbMech::P29TimeOfLongitude(sv_postLOI2.R, sv_postLOI2.V, sv_postLOI2.MJD, sv_postLOI2.gravref, 180.0*RAD);
	res->t_Rev2Meridian = OrbMech::GETfromMJD(MJD_meridian, opt->GETbase);

	if (opt->DOIType == 1)
	{
		//Calculate DOI DV
		DOIMan doiopt;
		SV sv_DOI, sv_postDOI;
		MATRIX3 Q_DOI;
		VECTOR3 dV_DOI;
		double TIG_DOI, r_DOI_apo, r_DOI_peri;

		doiopt.R_LLS = opt->R_LLS;
		doiopt.EarliestGET = TIG_LOI + 3.5*3600.0;
		doiopt.GETbase = opt->GETbase;
		doiopt.lat = opt->LSlat;
		doiopt.lng = opt->LSlng;
		doiopt.N = opt->N;
		doiopt.opt = opt->DOIType;
		doiopt.sv0 = sv_postLOI2;
		doiopt.PeriAng = opt->DOIPeriAng;
		doiopt.PeriAlt = opt->DOIPeriAlt;

		DOITargeting(&doiopt, dV_DOI, TIG_DOI);
		sv_DOI = coast(sv_postLOI2, TIG_DOI - OrbMech::GETfromMJD(sv_postLOI2.MJD, opt->GETbase));
		Q_DOI = OrbMech::LVLH_Matrix(sv_DOI.R, sv_DOI.V);
		res->dV_LVLH_DOI = mul(Q_DOI, dV_DOI);

		sv_postDOI = sv_DOI;
		sv_postDOI.V = sv_DOI.V + dV_DOI;
		OrbMech::periapo(sv_postDOI.R, sv_postDOI.V, OrbMech::mu_Moon, r_DOI_apo, r_DOI_peri);

		res->h_apo_postDOI = r_DOI_apo - opt->R_LLS;
		res->h_peri_postDOI = r_DOI_peri - opt->R_LLS;
	}

	res->TIG = OrbMech::GETfromMJD(sv1.MJD, opt->GETbase);
	res->DV = DV5;
	res->dV_LVLH_MCC = mul(OrbMech::LVLH_Matrix(sv1.R, sv1.V), res->DV);

	//TBD
	PZMCCXFR.sv_man_bef[0].R = sv1.R;
	PZMCCXFR.sv_man_bef[0].V = sv1.V;
	PZMCCXFR.sv_man_bef[0].GMT = OrbMech::GETfromMJD(sv1.MJD, GMTBASE);
	if (sv1.gravref == hEarth)
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_EARTH;
	}
	else
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_MOON;
	}
	PZMCCXFR.V_man_after[0] = sv1.V + res->DV;

	res->EMPLatitude = lat_nd4;

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingFlyby(MCCFlybyMan *opt, TLMCCResults *res)
{
	SV sv0, sv1, sv_peri2, sv_peri3, sv_reentry3;
	double dt1, dt2, PeriMJDguess;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 DV1, var_conv1, DV2, var_conv2, DV3;
	SV sv_tig;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	sv0 = opt->RV_MCC;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	//Step 1: Converge conic TLMC
	TLMCFirstGuessConic(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, DV1, var_conv1);

	//Step 2: Converge integrated TLMC
	IntegratedTLMC(sv1, opt->lat, opt->h_peri, 0.0, PeriMJDguess, var_conv1, DV2, var_conv2, sv_peri2);

	//Step 3: Converge integrated free-return trajectory
	if (!TLMCFlyby(sv1, opt->lat, opt->h_peri, DV2, DV3, sv_peri3, sv_reentry3))
	{
		return false;
	}

	res->PericynthionGET = (sv_peri3.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry3.MJD - opt->GETbase)*24.0*3600.0;

	res->TIG = OrbMech::GETfromMJD(sv1.MJD, opt->GETbase);
	res->DV = DV3;
	res->dV_LVLH_MCC = mul(OrbMech::LVLH_Matrix(sv1.R, sv1.V), res->DV);

	//TBD
	PZMCCXFR.sv_man_bef[0].R = sv1.R;
	PZMCCXFR.sv_man_bef[0].V = sv1.V;
	PZMCCXFR.sv_man_bef[0].GMT = OrbMech::GETfromMJD(sv1.MJD, GMTBASE);
	if (sv1.gravref == hEarth)
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_EARTH;
	}
	else
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_MOON;
	}
	PZMCCXFR.V_man_after[0] = sv1.V + res->DV;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(BODY_EARTH, sv_reentry3.MJD);
	R_geo = rhtmul(Rot, sv_reentry3.R);
	V_geo = rhtmul(Rot, sv_reentry3.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = OrbMech::R_Earth + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry3.R, -sv_reentry3.V, r_EI, 1.0, OrbMech::mu_Earth);
	OrbMech::oneclickcoast(sv_reentry3.R, sv_reentry3.V, sv_reentry3.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry3.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(MCCSPSLunarFlybyMan *opt, TLMCCResults *res, int &step)
{
	SV sv0, sv1, sv_peri, sv_reentry;
	double dt1, dt2, PeriMJDguess;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 DV, var_converged;
	SV sv_tig;

	step = 0;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	sv0 = opt->RV_MCC;

	dt1 = opt->MCCGET - (sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;
	dt2 = opt->PeriGET - opt->MCCGET;

	sv1 = coast(sv0, dt1);

	PeriMJDguess = opt->PeriGET / 24.0 / 3600.0 + opt->GETbase;

	SV sv_p1, sv_p2, sv_r2, sv_p3, sv_r3, sv_p3B, S1, S2C, sv_p4, sv_r4, sv_p5, sv_r5, S_apo, sv_p6, sv_r6;
	VECTOR3 DV1, DV2, DV3, DV3B, DV4, DV5, DV6_guess, DV6, DV7_guess;
	double h_peri_fg, LunarRightAscension, LunarDeclination, LunarDistance, InclFRGuess, lat_EMP_SPLIT, lat_EMP_Target3, lat_EMP_Target6, lattemp;
	double lat_pc_max, lat_pc_min;

	h_peri_fg = 80.0*1852.0;

	//TBD: the 80NM first guess for the perilune height could be a manual input. 2000+ NM flybys don't work very well with this initial guess.
	//So for now limit the difference between the heights to 1000NM.
	if (opt->h_peri - h_peri_fg > 1000.0*1852.0)
	{
		h_peri_fg += opt->h_peri - h_peri_fg - 1000.0*1852.0;
	}

	step++;

	//Step 1: Conic TLMC First Guess
	TLMCFirstGuessConic(sv1, opt->lat, h_peri_fg, 0.0, PeriMJDguess, DV1, var_converged);
	step++;

	//Step 2: Conic Flyby to minimum inclination flyby

	OrbMech::GetLunarEquatorialCoordinates(PeriMJDguess, LunarRightAscension, LunarDeclination, LunarDistance);

	//User input constraint. Inclination has to be lunar declination at pericynthion passage plus 5 degrees
	if (opt->FRInclination < abs(LunarDeclination) + 5.0*RAD)
	{
		return false;
	}

	InclFRGuess = abs(LunarDeclination) + 2.0*RAD;

	if (!TLMCConicFlybyToInclinationSubprocessor(sv1, h_peri_fg, InclFRGuess, DV1, DV2, sv_p2, sv_r2, lat_EMP_SPLIT))
	{
		return false;
	}

	step++;

	//Step 3: Execute conic flyby to the biased latitude of perilune
	if (opt->AscendingNode)
	{
		lat_EMP_Target3 = lat_EMP_SPLIT + 2.0*RAD;
	}
	else
	{
		lat_EMP_Target3 = lat_EMP_SPLIT - 2.0*RAD;
	}

	if (!TLMCFlybyConic(sv1, lat_EMP_Target3, h_peri_fg, DV2, DV3, sv_p3, sv_r3))
	{
		return false;
	}

	S1 = sv1;
	S2C = sv1;
	S2C.V += DV3;

	//Step 3B: Integrated first guess with new latitude
	TLMCFirstGuess(sv1, lat_EMP_Target3, h_peri_fg, sv_p3.MJD, DV3B, sv_p3B);
	step++;

	//Step 4: Execute integrated flyby to the same latitude of perilune as step 3
	if (!TLMCFlyby(sv1, lat_EMP_Target3, h_peri_fg, DV3B, DV4, sv_p4, sv_r4))
	{
		return false;
	}

	step++;

	//Step 5: Execute integrated flyby to same inclination of free return and height as step 2
	if (!TLMCIntegratedFlybyToInclinationSubprocessor(sv1, h_peri_fg, InclFRGuess, DV4, DV5, sv_p5, sv_r5, lattemp))
	{
		return false;
	}

	S_apo = S2C;
	S_apo.V -= DV5;
	DV6_guess = S2C.V - S_apo.V;

	if (opt->AscendingNode)
	{
		lat_pc_max = 20.0*RAD;
		lat_pc_min = lat_EMP_SPLIT - 0.5*RAD;
	}
	else
	{
		lat_pc_max = lat_EMP_SPLIT + 0.5*RAD;
		lat_pc_min = -20.0*RAD;
	}

	step++;

	//Step 6: Converge a conic free return (select mode using S_apo as the state vector)
	if (!TLMCConicFlybyToInclinationSubprocessor(S_apo, opt->h_peri, opt->FRInclination, DV6_guess, DV6, sv_p6, sv_r6, lat_EMP_Target6))
	{
		return false;
	}

	step++;

	S2C = S_apo;
	S2C.V += DV6;
	DV7_guess = S2C.V - S_apo.V;

	//Step 7: Converge integrated flyby to specified inclination of free return and height using state S1
	if (!TLMCIntegratedFlybyToInclinationSubprocessor(sv1, opt->h_peri, opt->FRInclination, DV7_guess, DV, sv_peri, sv_reentry, res->EMPLatitude))
	{
		return false;
	}

	step++;

	res->TIG = OrbMech::GETfromMJD(sv1.MJD, opt->GETbase);
	res->DV = DV;

	//TBD
	PZMCCXFR.sv_man_bef[0].R = sv1.R;
	PZMCCXFR.sv_man_bef[0].V = sv1.V;
	PZMCCXFR.sv_man_bef[0].GMT = OrbMech::GETfromMJD(sv1.MJD, GMTBASE);
	if (sv1.gravref == hEarth)
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_EARTH;
	}
	else
	{
		PZMCCXFR.sv_man_bef[0].RBI = BODY_MOON;
	}
	PZMCCXFR.V_man_after[0] = sv1.V + res->DV;

	res->PericynthionGET = (sv_peri.MJD - opt->GETbase)*24.0*3600.0;
	res->EntryInterfaceGET = (sv_reentry.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate Reentry Parameters
	MATRIX3 Rot;
	VECTOR3 R_geo, V_geo, H_geo;

	Rot = OrbMech::GetRotationMatrix(BODY_EARTH, sv_reentry.MJD);
	R_geo = rhtmul(Rot, sv_reentry.R);
	V_geo = rhtmul(Rot, sv_reentry.V);
	H_geo = crossp(R_geo, V_geo);
	res->FRInclination = acos(H_geo.z / length(H_geo));

	double rtgo, vio, ret, r_EI, dt_EI;
	VECTOR3 R_EI, V_EI;

	r_EI = OrbMech::R_Earth + 400000.0*0.3048;
	dt_EI = OrbMech::time_radius(sv_reentry.R, -sv_reentry.V, r_EI, 1.0, OrbMech::mu_Earth);
	OrbMech::oneclickcoast(sv_reentry.R, sv_reentry.V, sv_reentry.MJD, -dt_EI, R_EI, V_EI, hEarth, hEarth);

	EntryCalculations::Reentry(R_EI, V_EI, sv_reentry.MJD - dt_EI / 24.0 / 3600.0, true, res->SplashdownLat, res->SplashdownLng, rtgo, vio, ret);

	return true;
}

bool RTCC::GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG)
{
	GPMPRESULTS res;
	return GeneralManeuverProcessor(opt, dV_i, P30TIG, res);
}

bool RTCC::GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG, GPMPRESULTS &res)
{
	//SV at threshold time
	SV sv1;
	//SV at TIG
	SV sv2;

	VECTOR3 DV;
	double dt1, R_E, mu, dt2;
	int code, body;

	code = opt->ManeuverCode;
	dt1 = opt->TIG_GET + (opt->GETbase - opt->RV_MCC.MJD)*24.0*3600.0;
	sv1 = coast(opt->RV_MCC, dt1);

	//Body radius
	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		if (opt->AltRef == 0)
		{
			R_E = 6371.0e3;
		}
		else
		{
			R_E = OrbMech::R_Earth;
		}

		body = BODY_EARTH;
		mu = OrbMech::mu_Earth;
	}
	else
	{
		if (opt->AltRef == 0)
		{
			R_E = OrbMech::R_Moon;
		}
		else
		{
			R_E = opt->R_LLS;
		}

		body = BODY_MOON;
		mu = OrbMech::mu_Moon;
	}

	//Advance state to maneuver time
	//Apogee
	if (code == RTCC_GMP_HAO || code == RTCC_GMP_FCA || code == RTCC_GMP_PHA || code == RTCC_GMP_HNA ||
		code == RTCC_GMP_CRA || code == RTCC_GMP_CPA || code == RTCC_GMP_CNA || code == RTCC_GMP_SAA)
	{
		dt2 = OrbMech::timetoapo_integ(sv1.R, sv1.V, sv1.MJD, sv1.gravref);
		sv2 = coast(sv1, dt2);
	}
	//Equatorial crossing
	else if (code == RTCC_GMP_PCE || code == RTCC_GMP_FCE)
	{
		dt2 = OrbMech::FindNextEquatorialCrossing(sv1.R, sv1.V, sv1.MJD, sv1.gravref);
		sv2 = coast(sv1, dt2);
	}
	//Perigee
	else if (code == RTCC_GMP_HPO || code == RTCC_GMP_FCP || code == RTCC_GMP_PHP || code == RTCC_GMP_HNP ||
		code == RTCC_GMP_CRP || code == RTCC_GMP_CPP || code == RTCC_GMP_CNP)
	{
		dt2 = OrbMech::timetoperi_integ(sv1.R, sv1.V, sv1.MJD, sv1.gravref, sv1.gravref);
		sv2 = coast(sv1, dt2);
	}
	//Longitude
	else if (code == RTCC_GMP_PCL || code == RTCC_GMP_CRL || code == RTCC_GMP_HOL || code == RTCC_GMP_FCL||
		code == RTCC_GMP_SAL || code == RTCC_GMP_PHL || code == RTCC_GMP_CPL || code == RTCC_GMP_HBL || 
		code == RTCC_GMP_CNL || code == RTCC_GMP_HNL || code == RTCC_GMP_NSL || code == RTCC_GMP_NHL)
	{
		double MJD_TIG;
		MJD_TIG = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, sv1.gravref, opt->long_D);
		sv2 = coast(sv1, (MJD_TIG - sv1.MJD)*24.0*3600.0);
	}
	//Height
	else if (code == RTCC_GMP_CRH || code == RTCC_GMP_HBH || code == RTCC_GMP_FCH || code == RTCC_GMP_CPH ||
		code == RTCC_GMP_PCH || code == RTCC_GMP_NSH || code == RTCC_GMP_HOH || code == RTCC_GMP_CNH)
	{
		double dt21, dt22, r_H;

		r_H = R_E + opt->H_D;
		dt21 = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, r_H, 1.0, sv1.gravref, sv1.gravref);
		dt22 = OrbMech::time_radius_integ(sv1.R, sv1.V, sv1.MJD, r_H, -1.0, sv1.gravref, sv1.gravref);

		if (abs(dt21) > abs(dt22))
		{
			dt2 = dt22;
		}
		else
		{
			dt2 = dt21;
		}

		sv2 = coast(sv1, dt2);
	}
	//Time
	else if (code == RTCC_GMP_PCT || code == RTCC_GMP_HOT || code == RTCC_GMP_NST || code == RTCC_GMP_HBT || code == RTCC_GMP_FCT ||
		code == RTCC_GMP_NHT || code == RTCC_GMP_PHT || code == RTCC_GMP_SAT || code == RTCC_GMP_HNT || code == RTCC_GMP_CRT ||
		code == RTCC_GMP_CPT || code == RTCC_GMP_CNT)
	{
		sv2 = sv1;
	}
	//Optimum Node Shift
	else if (code==RTCC_GMP_NSO)
	{
		OELEMENTS coe;
		MATRIX3 Rot;
		VECTOR3 R1_equ, V1_equ;
		double u_b, u_D;

		Rot = OrbMech::GetObliquityMatrix(body, sv1.MJD);
		R1_equ = rhtmul(Rot, sv1.R);
		V1_equ = rhtmul(Rot, sv1.V);
		coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
		u_b = fmod(coe.TA + coe.w, PI2);

		u_D = atan2(1.0 + cos(opt->dLAN), -sin(opt->dLAN)*cos(coe.RA));
		if (u_D < 0)
		{
			u_D += PI2;
		}
		if (u_b >= u_D)
		{
			if (u_b < fmod(u_D + PI, PI2))
			{
				u_D += PI;
			}
		}

		bool error2;
		sv2 = OrbMech::PMMAEGS(sv1, 2, u_D, error2);
	}
	//Optimum apogee and perigee change
	else if (code == RTCC_GMP_HBO)
	{
		SV sv_a, sv_p;
		OELEMENTS coe_b, coe_p, coe_a;
		double p_d, p_b, r_AD, r_PD, a_d, e_d, a_b, e_b, r_a_b, r_p_b, R_MD, dt21, dt22, T_p, a_p, a_a, dr_a_max, dr_p_max, R_MD_apo, cos_theta, theta, r_p_apo, r_a_apo;
		int K1, n, nmax;

		//If desired apogee is equal to perigee, bias perigee to prevent zero eccentricity
		if (opt->H_P == opt->H_A)
		{
			opt->H_P -= 0.1*1852.0;
		}

		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		coe_b = OrbMech::coe_from_sv(sv1.R, sv1.V, mu);

		PMMAPD(sv1, sv_a, sv_p);
		r_a_b = length(sv_a.R);
		r_p_b = length(sv_p.R);
		coe_p = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
		a_p = coe_p.h*coe_p.h / (mu*(1.0 - coe_p.e * coe_p.e));
		coe_a = OrbMech::coe_from_sv(sv_a.R, sv_a.V, mu);
		a_a = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e * coe_a.e));

		T_p = OrbMech::period(sv1.R, sv1.V, mu);

		if (r_AD<r_p_b || r_PD>r_a_b)
		{
			return false;
		}

		if (r_PD > r_a_b && r_PD > r_p_b)
		{
			K1 = -1;
		}
		else if (r_PD < r_a_b && r_PD < r_p_b)
		{
			K1 = -1;
		}
		else
		{
			K1 = 1;
		}

		dr_a_max = (a_p*(1.0 + coe_p.e) - r_a_b)*(r_AD / r_a_b);
		dr_p_max = (a_a*(1.0 - coe_a.e) - r_p_b)*(r_PD / r_p_b);
		a_d = (r_AD + r_PD) / 2.0;
		e_d = abs((a_d - r_PD) / a_d);

		e_b = coe_b.e;
		a_b = coe_b.h*coe_b.h / (mu*(1.0 - e_b * e_b));

		n = 0;
		nmax = 5;
		R_MD_apo = 1.0;
		sv2 = sv1;

		p_d = a_d * (1.0 - e_d * e_d);
		p_b = a_b * (1.0 - e_b * e_b);

		if (K1 < 0)
		{
			R_MD = 2.0*(p_d - p_b)*a_d*a_b / (p_d*a_d - p_b * a_b);
		}
		else
		{
			R_MD = (e_b*p_d - e_d * p_b) / (e_b - e_d);
		}

		do
		{
			R_MD_apo = R_MD;

			dt21 = OrbMech::time_radius_integ(sv2.R, sv2.V, sv2.MJD, R_MD, 1.0, sv2.gravref, sv2.gravref);
			if (dt21 < 0 && n == 0)
			{
				dt21 += T_p;
			}
			dt22 = OrbMech::time_radius_integ(sv2.R, sv2.V, sv2.MJD, R_MD, -1.0, sv2.gravref, sv2.gravref);
			if (dt22 < 0 && n == 0)
			{
				dt22 += T_p;
			}

			if (dt21 > dt22)
			{
				dt2 = dt22;
			}
			else
			{
				dt2 = dt21;
			}
			sv2 = coast(sv2, dt2);
			coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
			e_b = coe_b.e;
			a_b = coe_b.h*coe_b.h / (mu*(1.0 - e_b * e_b));

			cos_theta = (a_d*(1.0 - e_d) - R_MD_apo) / (e_d*R_MD_apo);
			if (abs(cos_theta) > 1.0)
			{
				if (cos_theta <= 0)
				{
					cos_theta = -1.0;
				}
				else
				{
					cos_theta = 1.0;
				}
			}
			theta = acos(cos_theta);
			r_a_apo = r_AD + (PI - theta) / PI * dr_a_max;
			r_p_apo = r_PD + theta / PI * dr_p_max;
			a_d = (r_a_apo + r_p_apo) / 2.0;
			e_d = abs((a_d - r_p_apo) / a_d);

			p_d = a_d * (1.0 - e_d * e_d);
			p_b = a_b * (1.0 - e_b * e_b);

			if (K1 < 0)
			{
				R_MD = 2.0*(p_d - p_b)*a_d*a_b / (p_d*a_d - p_b * a_b);
			}
			else
			{
				R_MD = (e_b*p_d - e_d * p_b) / (e_b - e_d);
			}

			n++;
		} while (abs(R_MD - R_MD_apo) > 1.0 && n < nmax);
	}
	//Optimum apse line rotation
	else if (code == RTCC_GMP_SAO)
	{
		OELEMENTS coe_b;
		double phi, dphi, a, b, theta_11, theta_12, dtheta1, dtheta2, dtheta;

		coe_b = OrbMech::coe_from_sv(sv1.R, sv1.V, mu);

		a = coe_b.e*coe_b.h*coe_b.h*(1.0 - cos(opt->dLOA));
		b = -coe_b.e*coe_b.h*coe_b.h*sin(opt->dLOA);
		phi = atan2(b, a);
		dphi = PI05;
		theta_11 = fmod(phi + dphi, PI2);
		theta_12 = fmod(phi - dphi, PI2);
		dtheta1 = fmod(theta_11 - coe_b.TA, PI2);
		dtheta2 = fmod(theta_12 - coe_b.TA, PI2);

		if (dtheta1 < dtheta2)
		{
			dtheta = dtheta1;
		}
		else
		{
			dtheta = dtheta2;
		}

		dt2 = OrbMech::time_theta(sv1.R, sv1.V, dtheta, mu);
		sv2 = coast(sv1, dt2);
	}
	//Apogee and perigee change + apse line rotation to specified longitude
	else if (code == RTCC_GMP_HAS)
	{
		SV sv_a, sv_p, sv2_apo;
		double r_AD, r_PD, lat_p, lng_p, dlng, dt, ddt, dlng_apo, eps, w_E, T_P;
		int n, nmax;

		ddt = 5.0*60.0;
		dt = 0.0;
		n = 0;
		nmax = 100;
		eps = 0.01*RAD;
		dlng = 1.0;
		w_E = PI2 / oapiGetPlanetPeriod(sv1.gravref);

		//If desired apogee is equal to perigee, bias perigee to prevent zero eccentricity
		if (opt->H_P == opt->H_A)
		{
			opt->H_P -= 0.1*1852.0;
		}

		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		while (abs(dlng) > eps && n < nmax)
		{
			sv2 = coast(sv1, dt);

			DV = ApoapsisPeriapsisChangeInteg(sv2, r_AD, r_PD);
			sv2_apo = sv2;
			sv2_apo.V += DV;
			T_P = OrbMech::period(sv2_apo.R, sv2_apo.V, mu);

			PMMAPD(sv2_apo, sv_a, sv_p);
			OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_p.gravref, lat_p, lng_p);
			lng_p += -T_P * w_E*(double)opt->N;
			lng_p = fmod(lng_p, PI2);
			dlng = OrbMech::calculateDifferenceBetweenAngles(lng_p, opt->long_D);

			if (n > 0 && dlng*dlng_apo < 0 && abs(dlng_apo) < PI05)
			{
				ddt = -ddt / 2.0;
			}

			dt += ddt;
			dlng_apo = dlng;
			n++;
		}
	}

	//Maneuver calculation

	//Plane change
	if (code == RTCC_GMP_PCE || code == RTCC_GMP_PCL || code == RTCC_GMP_PCT || code == RTCC_GMP_PCH)
	{
		OELEMENTS coe_b, coe_a;
		MATRIX3 Rot;
		VECTOR3 R_a, V_a, R2_equ, V2_equ;

		Rot = OrbMech::GetObliquityMatrix(body, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a = OrbMech::PlaneChange(coe_b, opt->dW);

		OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
		DV = rhmul(Rot, V_a - V2_equ);
	}
	//Circularization
	else if (code == RTCC_GMP_CRL || code == RTCC_GMP_CRH || code == RTCC_GMP_CRT || code == RTCC_GMP_CRA || code == RTCC_GMP_CRP)
	{
		DV = CircularizationManeuverInteg(sv2);
	}
	//Height Maneuver
	else if (code == RTCC_GMP_HOL || code == RTCC_GMP_HOT || code == RTCC_GMP_HAO || code == RTCC_GMP_HPO || code == RTCC_GMP_HOH)
	{
		DV = HeightManeuverInteg(sv2, opt->dH_D);
	}
	//Node Shift
	else if (code == RTCC_GMP_NSL || code == RTCC_GMP_NST || code == RTCC_GMP_NSH || code == RTCC_GMP_NSO)
	{
		OELEMENTS coe_b, coe_a;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a, V_a;

		Rot = OrbMech::GetObliquityMatrix(body, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a = OrbMech::NodeShift(coe_b, opt->dLAN);

		OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
		DV = rhmul(Rot, V_a - V2_equ);
	}
	//Change apogee and perigee
	else if (code == RTCC_GMP_HBT || code == RTCC_GMP_HBH || code == RTCC_GMP_HBO || code == RTCC_GMP_HBL)
	{
		OELEMENTS coe_b, coe_a;
		double r_AD, r_PD;

		coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		DV = ApoapsisPeriapsisChangeInteg(sv2, r_AD, r_PD);
	}
	//Flight Controller Input
	else if (code == RTCC_GMP_FCT || code == RTCC_GMP_FCL || code == RTCC_GMP_FCH || code == RTCC_GMP_FCA || code == RTCC_GMP_FCP || code == RTCC_GMP_FCE)
	{
		MATRIX3 Rot;
		VECTOR3 dV_FC;

		dV_FC = _V(opt->dV*cos(opt->Pitch)*cos(opt->Yaw), opt->dV*sin(opt->Yaw), -opt->dV*sin(opt->Pitch)*cos(opt->Yaw));
		Rot = OrbMech::LVLH_Matrix(sv2.R, sv2.V);
		DV = tmul(Rot, dV_FC);
	}
	//Node Shift + Apogee/Perigee Change
	else if (code == RTCC_GMP_NHT || code == RTCC_GMP_NHL)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a, V_a, DV1, DV2;
		double r_AD, r_PD;

		sv2_apo1 = sv2;
		r_AD = R_E + opt->H_A;
		r_PD = R_E + opt->H_P;

		Rot = OrbMech::GetObliquityMatrix(body, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::NodeShift(coe_b, opt->dLAN);

		OrbMech::sv_from_coe(coe_a1, mu, R_a, V_a);
		sv2_apo1.V = rhmul(Rot, V_a);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = ApoapsisPeriapsisChangeInteg(sv2_apo1, r_AD, r_PD);
		DV = DV1 + DV2;
	}
	//Height change + plane change
	else if (code == RTCC_GMP_PHL || code == RTCC_GMP_PHT || code == RTCC_GMP_PHA || code == RTCC_GMP_PHP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(body, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::PlaneChange(coe_b, opt->dW);

		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = HeightManeuverInteg(sv2_apo1, opt->dH_D);
		DV = DV1 + DV2;
	}
	//Circularization + plane change
	else if (code == RTCC_GMP_CPL || code == RTCC_GMP_CPH || code == RTCC_GMP_CPT || code == RTCC_GMP_CPA || code == RTCC_GMP_CPP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(body, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::PlaneChange(coe_b, opt->dW);

		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = CircularizationManeuverInteg(sv2_apo1);
		DV = DV1 + DV2;
	}
	//Shift line-of-apsides
	else if (code == RTCC_GMP_SAT || code == RTCC_GMP_SAO || code == RTCC_GMP_SAL)
	{
		OELEMENTS coe_b, coe_a;
		VECTOR3 R_a, V_a;
		double u;

		coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
		u = fmod(coe_b.TA + coe_b.w, PI2);

		coe_a.e = coe_b.e*cos(coe_b.TA) / cos(coe_b.TA - opt->dLOA);
		coe_a.h = coe_b.h;
		coe_a.i = coe_b.i;
		coe_a.RA = coe_b.RA;
		coe_a.TA = fmod(coe_b.TA - opt->dLOA, PI2);
		coe_a.w = fmod(u - coe_a.TA, PI2);

		OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
		DV = V_a - sv2.V;
	}
	//Shift line-of-apsides to specified longitude
	else if (code == RTCC_GMP_SAA)
	{
		SV sv_a, sv_p, sv2_apo;
		OELEMENTS coe_b, coe_a;
		VECTOR3 R_a, V_a;
		double u, lat_p, lng_p, dLOA, eps, ddLOA;
		int n, nmax;

		coe_b = OrbMech::coe_from_sv(sv2.R, sv2.V, mu);
		u = fmod(coe_b.TA + coe_b.w, PI2);

		sv2_apo = sv2;

		n = 0;
		nmax = 20;
		eps = 0.01*RAD;
		dLOA = 0.0;
		ddLOA = 1.0;

		while (abs(ddLOA) > eps && n < nmax)
		{
			PMMAPD(sv2_apo, sv_a, sv_p);
			OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_p.gravref, lat_p, lng_p);

			ddLOA = OrbMech::calculateDifferenceBetweenAngles(lng_p, opt->long_D);
			dLOA += ddLOA;

			coe_a.e = coe_b.e*cos(coe_b.TA) / cos(coe_b.TA - dLOA);
			coe_a.h = coe_b.h;
			coe_a.i = coe_b.i;
			coe_a.RA = coe_b.RA;
			coe_a.TA = fmod(coe_b.TA - dLOA, PI2);
			coe_a.w = fmod(u - coe_a.TA, PI2);

			OrbMech::sv_from_coe(coe_a, mu, R_a, V_a);
			DV = V_a - sv2.V;
			sv2_apo.V = sv2.V + DV;
			n++;
		}
	}
	//Circularization + node shift
	else if (code == RTCC_GMP_CNL || code == RTCC_GMP_CNH || code == RTCC_GMP_CNT || code == RTCC_GMP_CNA || code == RTCC_GMP_CNP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(body, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::NodeShift(coe_b, opt->dLAN);
		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = CircularizationManeuverInteg(sv2_apo1);
		DV = DV1 + DV2;
	}
	//Height change + node shift
	else if (code == RTCC_GMP_HNL || code == RTCC_GMP_HNT || code == RTCC_GMP_HNA || code == RTCC_GMP_HNP)
	{
		SV sv2_apo1;
		OELEMENTS coe_b, coe_a1;
		MATRIX3 Rot;
		VECTOR3 R2_equ, V2_equ, R_a1, V_a1, DV1, DV2;

		sv2_apo1 = sv2;

		Rot = OrbMech::GetObliquityMatrix(body, sv2.MJD);
		R2_equ = rhtmul(Rot, sv2.R);
		V2_equ = rhtmul(Rot, sv2.V);
		coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);

		coe_a1 = OrbMech::NodeShift(coe_b, opt->dLAN);
		OrbMech::sv_from_coe(coe_a1, mu, R_a1, V_a1);
		sv2_apo1.V = rhmul(Rot, V_a1);
		DV1 = sv2_apo1.V - sv2.V;

		DV2 = HeightManeuverInteg(sv2_apo1, opt->dH_D);
		DV = DV1 + DV2;
	}

	dV_i = DV;
	P30TIG = (sv2.MJD - opt->GETbase)*24.0*3600.0;

	//Calculate orbital elements
	SV sv_a, sv_p, sv2_apo;
	OELEMENTS coe_a, coe_b;
	MATRIX3 obl, Q_Xx;
	VECTOR3 dV_LVLH, R2_equ, V2_equ, V2_equ_apo;

	sv2_apo = sv2;
	sv2_apo.V += DV;
	obl = OrbMech::GetObliquityMatrix(body, sv2.MJD);
	R2_equ = rhtmul(obl, sv2.R);
	V2_equ = rhtmul(obl, sv2.V);
	V2_equ_apo = rhtmul(obl, sv2_apo.V);
	coe_b = OrbMech::coe_from_sv(R2_equ, V2_equ, mu);
	coe_a = OrbMech::coe_from_sv(R2_equ, V2_equ_apo, mu);
	PMMAPD(sv2_apo, sv_a, sv_p);
	Q_Xx = OrbMech::LVLH_Matrix(sv2.R, sv2.V);
	dV_LVLH = mul(Q_Xx, DV);

	res.GET_A = (sv_a.MJD - opt->GETbase)*24.0*3600.0;
	res.HA = length(sv_a.R) - R_E;
	OrbMech::latlong_from_J2000(sv_a.R, sv_a.MJD, sv_a.gravref, res.lat_A, res.long_A);
	res.GET_P = (sv_p.MJD - opt->GETbase)*24.0*3600.0;
	res.HP = length(sv_p.R) - R_E;
	OrbMech::latlong_from_J2000(sv_p.R, sv_p.MJD, sv_p.gravref, res.lat_P, res.long_P);
	res.A = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e*coe_a.e));
	res.E = coe_a.e;
	res.I = coe_a.i;
	res.Node_Ang = coe_a.RA;
	res.Del_G = coe_a.w - coe_b.w;
	if (res.Del_G < 0)
	{
		res.Del_G += PI2;
	}
	res.Pitch_Man = atan2(-dV_LVLH.z, dV_LVLH.x);
	res.Yaw_Man = atan2(dV_LVLH.y, sqrt(dV_LVLH.x*dV_LVLH.x + dV_LVLH.z*dV_LVLH.z));
	res.H_Man = length(sv2.R) - R_E;
	OrbMech::latlong_from_J2000(sv2.R, sv2.MJD, sv2.gravref, res.lat_Man, res.long_Man);

	/*coe_before.elem.a = coe_b.h*coe_b.h / (mu*(1.0 - coe_b.e*coe_b.e));
	coe_before.elem.e = coe_b.e;
	coe_before.elem.i = coe_b.i;
	coe_before.elem.theta = coe_b.RA;
	coe_before.param.ApD = length(sv_a.R) - R_E;
	coe_before.param.ApT = (sv_a.MJD - opt->GETbase)*24.0*3600.0;
	coe_before.param.T = PI2 * sqrt(pow(coe_before.elem.a, 3) / mu);
	coe_before.param.PeD = length(sv_p.R) - R_E;
	coe_before.param.PeT = (sv_p.MJD - opt->GETbase)*24.0*3600.0;
	coe_before.param.TrA = coe_b.TA;

	PMMAPD(sv2_apo, sv_a, sv_p);
	coe_after.elem.a = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e*coe_a.e));
	coe_after.elem.e = coe_a.e;
	coe_after.elem.i = coe_a.i;
	coe_after.elem.theta = coe_a.RA;
	coe_after.param.ApD = length(sv_a.R) - R_E;
	coe_after.param.ApT = (sv_a.MJD - opt->GETbase)*24.0*3600.0;
	coe_after.param.T = PI2 * sqrt(pow(coe_after.elem.a, 3) / mu);
	coe_after.param.PeD = length(sv_p.R) - R_E;
	coe_after.param.PeT = (sv_p.MJD - opt->GETbase)*24.0*3600.0;
	coe_after.param.TrA = coe_a.TA;*/

	//Store elements
	PZGPMELM.SV_before.R = sv2.R;
	PZGPMELM.SV_before.V = sv2.V;
	PZGPMELM.SV_before.GMT = OrbMech::GETfromMJD(sv2.MJD, GMTBASE);
	if (sv2.gravref == hEarth)
	{
		PZGPMELM.SV_before.RBI = BODY_EARTH;
	}
	else
	{
		PZGPMELM.SV_before.RBI = BODY_MOON;
	}
	PZGPMELM.V_after = sv2_apo.V;
	PZGPMELM.code = "GPM";

	return true;
}

void RTCC::TLI_PAD(TLIPADOpt* opt, TLIPAD &pad)
{
	MATRIX3 M_R, M, M_RTM;
	VECTOR3 UX, UY, UZ, DV_P, DV_C, V_G, U_TD, X_B, IgnAtt, SepATT, ExtATT;
	double boil, m0, dt, t_go, F, v_e, theta_T, dVC;
	SV sv1, sv2, sv3;

	boil = (1.0 - 0.99998193) / 10.0;
	m0 = opt->vessel->GetEmptyMass();

	dt = opt->TIG - (opt->sv0.MJD - opt->GETbase) * 24.0 * 60.0 * 60.0;

	sv1 = coast(opt->sv0, dt);
	sv1.mass = m0 + (opt->sv0.mass - m0)*exp(-boil*dt);

	if (opt->uselvdc)
	{
		t_go = opt->TLI - opt->TIG;

		UY = unit(crossp(sv1.V, -sv1.R));
		UZ = unit(sv1.R);
		UX = crossp(UY, UZ);

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
		IgnAtt = OrbMech::CALCGAR(opt->REFSMMAT, M_R);

		sv2 = sv1;
		sv2.R = opt->R_TLI;
		sv2.V = opt->V_TLI;
		sv2.MJD += t_go / 24.0 / 3600.0;

		sv3 = coast(sv2, 900.0);
	}
	else
	{
		//TBD: This is broken
		sv2 = ExecuteManeuver(sv1, opt->GETbase, opt->TIG, opt->dV_LVLH, 0.0, RTCC_ENGINETYPE_CSMSPS);

		sv3 = coast(sv2, 900.0);

		t_go = (sv2.MJD - sv1.MJD)*24.0*3600.0;

		UY = unit(crossp(sv1.V, sv1.R));
		UZ = unit(-sv1.R);
		UX = crossp(UY, UZ);

		v_e = opt->vessel->GetThrusterIsp0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));
		F = opt->vessel->GetThrusterMax0(opt->vessel->GetGroupThruster(THGROUP_MAIN, 0));

		DV_P = UX*opt->dV_LVLH.x + UZ*opt->dV_LVLH.z;
		if (length(DV_P) != 0.0)
		{
			theta_T = length(crossp(sv1.R, sv1.V))*length(opt->dV_LVLH)*opt->vessel->GetMass() / OrbMech::power(length(sv1.R), 2.0) / F;
			DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
			V_G = DV_C + UY*opt->dV_LVLH.y;
		}
		else
		{
			V_G = UX*opt->dV_LVLH.x + UY*opt->dV_LVLH.y + UZ*opt->dV_LVLH.z;
		}

		U_TD = unit(V_G);
		X_B = unit(V_G);

		UX = X_B;
		UY = unit(crossp(X_B, -sv1.R));
		UZ = unit(crossp(X_B, crossp(X_B, -sv1.R)));

		M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);

		IgnAtt = OrbMech::CALCGAR(opt->REFSMMAT, M_R);
	}

	UY = unit(crossp(sv3.V, sv3.R));
	UZ = unit(-sv3.R);
	UX = crossp(UY, UZ);
	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = OrbMech::CALCSMSC(_V(PI - opt->SeparationAttitude.x, opt->SeparationAttitude.y, opt->SeparationAttitude.z));
	M_RTM = mul(M, M_R);

	SepATT = OrbMech::CALCGAR(opt->REFSMMAT, M_RTM);
	ExtATT = _V(300.0*RAD - SepATT.x, SepATT.y + PI, PI2 - SepATT.z);

	dVC = length(opt->dV_LVLH);

	pad.BurnTime = t_go;
	pad.dVC = dVC / 0.3048;
	pad.IgnATT = _V(OrbMech::imulimit(IgnAtt.x*DEG), OrbMech::imulimit(IgnAtt.y*DEG), OrbMech::imulimit(IgnAtt.z*DEG));
	pad.SepATT = _V(OrbMech::imulimit(SepATT.x*DEG), OrbMech::imulimit(SepATT.y*DEG), OrbMech::imulimit(SepATT.z*DEG));
	pad.ExtATT = _V(OrbMech::imulimit(ExtATT.x*DEG), OrbMech::imulimit(ExtATT.y*DEG), OrbMech::imulimit(ExtATT.z*DEG));
	pad.TB6P = opt->TIG - 577.6;
	pad.VI = length(sv2.V) / 0.3048;
}

bool RTCC::PDI_PAD(PDIPADOpt* opt, AP11PDIPAD &pad)
{
	SV sv1, sv2, sv_I;
	MATRIX3 REFSMMAT;
	VECTOR3 U_FDP;
	double C_R, TTT, t_IG;

	if (opt->direct)
	{
		sv2 = opt->sv0;
	}
	else
	{
		sv2 = ExecuteManeuver(opt->sv0, opt->GETbase, opt->P30TIG, opt->dV_LVLH, 0.0, RTCC_ENGINETYPE_LMDPS);
	}

	if (!PDIIgnitionAlgorithm(sv2, opt->GETbase, opt->R_LS, opt->t_land, sv_I, TTT, C_R, U_FDP, REFSMMAT))
	{
		return false;
	}

	t_IG = OrbMech::GETfromMJD(sv_I.MJD, opt->GETbase);

	VECTOR3 X_B, UX, UY, UZ, IMUangles, FDAIangles;
	MATRIX3 M, M_R;
	double headsswitch;

	if (opt->HeadsUp)
	{
		headsswitch = -1.0;
	}
	else
	{
		headsswitch = 1.0;
	}

	X_B = tmul(opt->REFSMMAT, unit(U_FDP));
	UX = X_B;
	if (abs(dotp(UX, unit(sv_I.R))) < cos(0.01*RAD))
	{
		UY = unit(crossp(UX, sv_I.R*headsswitch));
	}
	else
	{
		UY = unit(crossp(UX, sv_I.V));
	}
	UZ = unit(crossp(UX, UY));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	IMUangles = OrbMech::CALCGAR(opt->REFSMMAT, mul(OrbMech::tmat(M), M_R));

	FDAIangles.z = asin(-cos(IMUangles.z)*sin(IMUangles.x));
	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.y = atan2(((sin(IMUangles.y)*cos(IMUangles.x) + cos(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z)), (cos(IMUangles.y)*cos(IMUangles.x) - sin(IMUangles.y)*sin(IMUangles.z)*sin(IMUangles.x)) / cos(FDAIangles.z));
	}

	if (abs(sin(FDAIangles.z)) != 1.0)
	{
		FDAIangles.x = atan2(sin(IMUangles.z), cos(IMUangles.z)*cos(IMUangles.x));
	}

	if (FDAIangles.x < 0)
	{
		FDAIangles.x += PI2;
	}
	if (FDAIangles.y < 0)
	{
		FDAIangles.y += PI2;
	}
	if (FDAIangles.z < 0)
	{
		FDAIangles.z += PI2;
	}

	pad.Att = _V(OrbMech::imulimit(FDAIangles.x*DEG), OrbMech::imulimit(FDAIangles.y*DEG), OrbMech::imulimit(FDAIangles.z*DEG));
	pad.CR = C_R / 1852.0;
	pad.DEDA231 = length(opt->R_LS) / 0.3048 / 100.0;
	pad.GETI = t_IG;
	pad.t_go = -TTT;

	return true;
}

void RTCC::LunarAscentPAD(ASCPADOpt opt, AP11LMASCPAD &pad)
{
	SV sv_Ins, sv_CSM_TIG;
	MATRIX3 Rot_LG, Rot_VG;
	VECTOR3 R_LSP, Q, U_R, Z_B, X_AGS, Y_AGS, Z_AGS;
	double CR, MJD_TIG, SMa, R_D, delta_L, sin_DL, cos_DL;

	MJD_TIG = OrbMech::MJDfromGET(opt.TIG, opt.GETbase);
	sv_CSM_TIG = coast(opt.sv_CSM, opt.TIG - OrbMech::GETfromMJD(opt.sv_CSM.MJD, opt.GETbase));

	Rot_LG = OrbMech::GetRotationMatrix(BODY_MOON, MJD_TIG);
	R_LSP = rhmul(Rot_LG, opt.R_LS);

	Q = unit(crossp(sv_CSM_TIG.V, sv_CSM_TIG.R));
	U_R = unit(R_LSP);
	R_D = length(R_LSP) + 60000.0*0.3048;
	CR = -R_D * asin(dotp(U_R, Q));

	sv_Ins.R = _V(R_D, 0, 0);
	sv_Ins.V = _V(opt.v_LV, opt.v_LH, 0);
	sv_Ins.gravref = sv_CSM_TIG.gravref;
	SMa = GetSemiMajorAxis(sv_Ins);

	Rot_VG = OrbMech::GetVesselToGlobalRotMatrix(opt.Rot_VL, Rot_LG);
	Z_B = mul(Rot_VG, _V(0, 0, 1));
	Z_B = unit(_V(Z_B.x, Z_B.z, Z_B.y));
	X_AGS = U_R;
	Z_AGS = unit(crossp(-Q, X_AGS));
	Y_AGS = unit(crossp(Z_AGS, X_AGS));

	delta_L = atan2(dotp(Z_B, Z_AGS), dotp(Z_B, Y_AGS));
	sin_DL = sin(delta_L);
	cos_DL = cos(delta_L);

	pad.CR = CR / 1852.0;
	pad.TIG = opt.TIG;
	pad.V_hor = opt.v_LH / 0.3048;
	pad.V_vert = opt.v_LV / 0.3048;
	pad.DEDA225_226 = SMa / 0.3048 / 100.0;
	pad.DEDA231 = length(opt.R_LS) / 0.3048 / 100.0;
	sprintf(pad.remarks, "");
	pad.DEDA047 = OrbMech::DoubleToDEDA(sin_DL, 14);
	pad.DEDA053 = OrbMech::DoubleToDEDA(cos_DL, 14);
}

void RTCC::CMCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH)
{
	CMMAXTDV(P30TIG, dV_LVLH);
	V71Update(str, CZAXTRDV.Octals, 10);
}

void RTCC::LGCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH)
{
	CMMLXTDV(P30TIG, dV_LVLH);
	V71Update(str, CZLXTRDV.Octals, 10);
}

void RTCC::AGCStateVectorUpdate(char *str, SV sv, bool csm, double AGCEpoch, double GETbase, bool v66)
{
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");

	MATRIX3 Rot = OrbMech::J2000EclToBRCS(AGCEpoch);

	VECTOR3 vel, pos;
	double get;
	int emem[24];

	pos = mul(Rot, sv.R);
	vel = mul(Rot, sv.V)*0.01;
	get = (sv.MJD - GETbase)*24.0*3600.0;

	if (sv.gravref == hMoon) {

		emem[0] = 21;
		emem[1] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (csm)
		{
			emem[2] = 2;
		}
		else
		{
			emem[2] = 77775;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	else if (sv.gravref == hEarth) {

		emem[0] = 21;
		emem[1] = 1501;

		if (csm)
		{
			emem[2] = 1;
		}
		else
		{
			emem[2] = 77776;	// Octal coded decimal
		}

		emem[3] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		emem[4] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		emem[5] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		emem[6] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		emem[7] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		emem[8] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		emem[9] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		emem[10] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		emem[11] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		emem[12] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		emem[13] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		emem[14] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		emem[15] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		emem[16] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}
	V71Update(str, emem, 17);
	if (v66)
	{
		sprintf(str, "%sV66EV37E00E", str);
	}
}

void RTCC::LandingSiteUplink(char *str, double lat, double lng, double alt, int RLSAddr)
{
	VECTOR3 R_P, R;
	double r_0;
	int emem[8];

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)));
	r_0 = OrbMech::R_Moon;

	R = R_P * (r_0 + alt);

	emem[0] = 10;
	emem[1] = RLSAddr;
	emem[2] = OrbMech::DoubleToBuffer(R.x, 27, 1);
	emem[3] = OrbMech::DoubleToBuffer(R.x, 27, 0);
	emem[4] = OrbMech::DoubleToBuffer(R.y, 27, 1);
	emem[5] = OrbMech::DoubleToBuffer(R.y, 27, 0);
	emem[6] = OrbMech::DoubleToBuffer(R.z, 27, 1);
	emem[7] = OrbMech::DoubleToBuffer(R.z, 27, 0);

	V71Update(str, emem, 8);
}

void RTCC::IncrementAGCTime(char *list, double dt)
{
	int emem[2];

	sprintf(list, "V73E");

	emem[0] = OrbMech::DoubleToBuffer(dt, 28, 1);
	emem[1] = OrbMech::DoubleToBuffer(dt, 28, 0);

	sprintf(list, "%s%dE", list, emem[0]);
	sprintf(list, "%s%dE", list, emem[1]);
	sprintf(list, "%sV33E", list);
}

void RTCC::V71Update(char *list, int *emem, int n)
{
	sprintf(list, "V71E%dE", emem[0]);
	for (int i = 1;i < n;i++)
	{
		sprintf(list, "%s%dE", list, emem[i]);
	}
	sprintf(list, "%sV33E", list);
}

void RTCC::TLANDUpdate(char *list, double t_land, int tlandaddr)
{
	int emem[5];
	emem[0] = 5;

	emem[1] = tlandaddr;
	emem[3] = tlandaddr + 1;

	emem[2] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 1);
	emem[4] = OrbMech::DoubleToBuffer(t_land*100.0, 28, 0);

	V72Update(list, emem, 5);
}

void RTCC::V72Update(char *list, int *emem, int n)
{
	sprintf(list, "V72E%dE", emem[0]);
	for (int i = 1;i < n;i++)
	{
		sprintf(list, "%s%dE", list, emem[i]);
	}
	sprintf(list, "%sV33E", list);
}

void RTCC::SunburstAttitudeManeuver(char *list, VECTOR3 imuangles)
{
	int emem[3];

	if (imuangles.x > PI)
	{
		imuangles.x -= PI2;
	}
	if (imuangles.y > PI)
	{
		imuangles.y -= PI2;
	}
	if (imuangles.z > PI)
	{
		imuangles.z -= PI2;
	}

	emem[0] = OrbMech::DoubleToBuffer(imuangles.x / PI, 0, 1);
	emem[1] = OrbMech::DoubleToBuffer(imuangles.y / PI, 0, 1);
	emem[2] = OrbMech::DoubleToBuffer(imuangles.z / PI, 0, 1);

	sprintf(list, "V21N1E372E0EV25N1E1631E%dE%dE%dEV25N26E20001E2067E70063EV30ER", emem[0], emem[1], emem[2]);
}

void RTCC::SunburstLMPCommand(char *list, int code)
{
	sprintf(list, "V67E%oEV33ER", code);
}

void RTCC::SunburstMassUpdate(char *list, double masskg)
{
	int emem[2];

	emem[0] = OrbMech::DoubleToBuffer(masskg, 15, 1);
	emem[1] = OrbMech::DoubleToBuffer(masskg, 15, 0);

	sprintf(list, "V24N1E1320E%dE%dER", emem[0], emem[1]);
}

void RTCC::AGCDesiredREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, bool cmc, bool AGCCoordSystem)
{
	MATRIX3 a;
	int emem[24];

	if (AGCCoordSystem)
	{
		a = REFSMMAT;
	}
	else
	{
		a = mul(REFSMMAT, OrbMech::tmat(OrbMech::J2000EclToBRCS(AGCEpoch)));
	}

	emem[0] = 24;
	if (cmc)
	{
		emem[1] = 306;
	}
	else
	{
		emem[1] = 3606;
	}
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	V71Update(list, emem, 20);
}

void RTCC::AGCREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, int offset, bool AGCCoordSystem)
{
	MATRIX3 a;
	int emem[24];

	if (AGCCoordSystem)
	{
		a = REFSMMAT;
	}
	else
	{
		a = mul(REFSMMAT, OrbMech::tmat(OrbMech::J2000EclToBRCS(AGCEpoch)));
	}

	emem[0] = 24;
	emem[1] = 1735 + offset;
	emem[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	emem[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	emem[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	emem[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	emem[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	emem[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	emem[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	emem[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	emem[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	emem[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	emem[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	emem[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	emem[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	emem[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	emem[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	emem[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	emem[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	emem[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	V71Update(list, emem, 20);
}

void RTCC::CMCRetrofireExternalDeltaVUpdate(char *list, double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH)
{
	int emem[24];
	double getign = P30TIG;

	emem[0] = 16;
	emem[1] = 3400;
	emem[2] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 1);
	emem[3] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 0);
	emem[4] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 1);
	emem[5] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 0);
	emem[6] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 1);
	emem[7] = OrbMech::DoubleToBuffer(dV_LVLH.x / 100.0, 7, 0);
	emem[8] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 1);
	emem[9] = OrbMech::DoubleToBuffer(dV_LVLH.y / 100.0, 7, 0);
	emem[10] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 1);
	emem[11] = OrbMech::DoubleToBuffer(dV_LVLH.z / 100.0, 7, 0);
	emem[12] = OrbMech::DoubleToBuffer(getign*100.0, 28, 1);
	emem[13] = OrbMech::DoubleToBuffer(getign*100.0, 28, 0);

	V71Update(list, emem, 14);
}

void RTCC::CMCEntryUpdate(char *list, double LatSPL, double LngSPL)
{
	int emem[24];

	emem[0] = 06;
	emem[1] = 3400;
	emem[2] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 1);
	emem[3] = OrbMech::DoubleToBuffer(LatSPL / PI2, 0, 0);
	emem[4] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 1);
	emem[5] = OrbMech::DoubleToBuffer(LngSPL / PI2, 0, 0);

	V71Update(list, emem, 6);
}

void RTCC::AGSStateVectorPAD(AGSSVOpt *opt, AP11AGSSVPAD &pad)
{
	SV sv1;
	VECTOR3 R, V;
	double AGSEpochTime, AGSEpochTime2, AGSEpochTime3, dt, scalR, scalV;

	AGSEpochTime = (opt->sv.MJD - opt->GETbase)*24.0*3600.0;
	AGSEpochTime2 = ceil(AGSEpochTime / 6.0)*6.0;

	dt = AGSEpochTime2 - AGSEpochTime;

	sv1 = coast(opt->sv, dt);

	R = mul(opt->REFSMMAT, sv1.R);
	V = mul(opt->REFSMMAT, sv1.V);

	AGSEpochTime3 = AGSEpochTime2 - opt->AGSbase;

	if (sv1.gravref == oapiGetObjectByName("Earth"))
	{
		scalR = 1000.0;
		scalV = 1.0;
	}
	else
	{
		scalR = 100.0;
		scalV = 0.1;
	}

	if (opt->csm)
	{
		pad.DEDA244 = R.x / 0.3048 / scalR;
		pad.DEDA245 = R.y / 0.3048 / scalR;
		pad.DEDA246 = R.z / 0.3048 / scalR;
		pad.DEDA264 = V.x / 0.3048 / scalV;
		pad.DEDA265 = V.y / 0.3048 / scalV;
		pad.DEDA266 = V.z / 0.3048 / scalV;
		pad.DEDA272 = AGSEpochTime3 / 60.0;
	}
	else
	{
		pad.DEDA240 = R.x / 0.3048 / scalR;
		pad.DEDA241 = R.y / 0.3048 / scalR;
		pad.DEDA242 = R.z / 0.3048 / scalR;
		pad.DEDA260 = V.x / 0.3048 / scalV;
		pad.DEDA261 = V.y / 0.3048 / scalV;
		pad.DEDA262 = V.z / 0.3048 / scalV;
		pad.DEDA254 = AGSEpochTime3 / 60.0;
	}
}

void RTCC::NavCheckPAD(SV sv, AP7NAV &pad, double GETbase, double GET)
{
	double lat, lng, alt, navcheckdt;
	VECTOR3 R1, V1;
	OBJHANDLE outgrav = NULL;
	
	if (GET == 0.0)
	{
		navcheckdt = 30 * 60;
		pad.NavChk[0] = (sv.MJD - GETbase)*24.0*3600.0 + navcheckdt;
	}
	else
	{
		navcheckdt = GET - (sv.MJD - GETbase)*24.0*3600.0;
		pad.NavChk[0] = GET;
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, navcheckdt, R1, V1, sv.gravref, outgrav);
	navcheck(R1, V1, sv.MJD + navcheckdt/24.0/3600.0, outgrav, lat, lng, alt);

	pad.alt[0] = alt / 1852.0;
	pad.lat[0] = lat*DEG;
	pad.lng[0] = lng*DEG;
}

void RTCC::P27PADCalc(P27Opt *opt, double AGCEpoch, P27PAD &pad)
{
	double lat, lng, alt, get, SVMJD;
	VECTOR3 pos, vel;
	OBJHANDLE hMoon = oapiGetGbodyByName("Moon");
	OBJHANDLE hEarth = oapiGetGbodyByName("Earth");
	bool csm = true;
	SV sv, sv1;

	SVMJD = opt->GETbase + opt->SVGET / 24.0 / 3600.0;

	sv = StateVectorCalc(opt->vessel, SVMJD);
	sv1 = coast(sv, opt->navcheckGET - opt->SVGET);

	navcheck(sv1.R, sv1.V, sv1.MJD, sv1.gravref, lat, lng, alt);
	
	sprintf(pad.Purpose[0], "SV");
	pad.GET[0] = opt->SVGET;
	pad.alt = alt / 1852.0;
	pad.lat = lat*DEG;
	pad.lng = lng*DEG;
	pad.NavChk = opt->navcheckGET;
	pad.Index[0] = 21;
	pad.Verb[0] = 71;

	pos = mul(OrbMech::J2000EclToBRCS(AGCEpoch), sv.R);
	vel = mul(OrbMech::J2000EclToBRCS(AGCEpoch), sv.V)*0.01;
	get = opt->SVGET;

	if (sv.gravref == hMoon) {

		pad.Data[0][0] = 1501;

		//if (g_Data.vessel->GetHandle()==oapiGetFocusObject()) 
		if (csm)
		{
			pad.Data[0][1] = 2;
		}
		else
		{
			pad.Data[0][1] = 77775;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 27, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 27, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 27, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 27, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 27, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 27, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 5, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 5, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 5, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 5, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 5, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 5, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

	if (sv.gravref == hEarth) {

		pad.Data[0][0] = 1501;

		if (csm)
		{
			pad.Data[0][1] = 1;
		}
		else
		{
			pad.Data[0][1] = 77776;	// Octal coded decimal
		}

		pad.Data[0][2] = OrbMech::DoubleToBuffer(pos.x, 29, 1);
		pad.Data[0][3] = OrbMech::DoubleToBuffer(pos.x, 29, 0);
		pad.Data[0][4] = OrbMech::DoubleToBuffer(pos.y, 29, 1);
		pad.Data[0][5] = OrbMech::DoubleToBuffer(pos.y, 29, 0);
		pad.Data[0][6] = OrbMech::DoubleToBuffer(pos.z, 29, 1);
		pad.Data[0][7] = OrbMech::DoubleToBuffer(pos.z, 29, 0);
		pad.Data[0][8] = OrbMech::DoubleToBuffer(vel.x, 7, 1);
		pad.Data[0][9] = OrbMech::DoubleToBuffer(vel.x, 7, 0);
		pad.Data[0][10] = OrbMech::DoubleToBuffer(vel.y, 7, 1);
		pad.Data[0][11] = OrbMech::DoubleToBuffer(vel.y, 7, 0);
		pad.Data[0][12] = OrbMech::DoubleToBuffer(vel.z, 7, 1);
		pad.Data[0][13] = OrbMech::DoubleToBuffer(vel.z, 7, 0);
		pad.Data[0][14] = OrbMech::DoubleToBuffer(get*100.0, 28, 1);
		pad.Data[0][15] = OrbMech::DoubleToBuffer(get*100.0, 28, 0);
	}

}

void RTCC::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, RTCC_START_STRING);
	// Booleans
	// Integers
	SAVE_INT("RTCC_REFSMMATType", REFSMMATType);
	// Floats
	SAVE_DOUBLE("RTCC_P30TIG", TimeofIgnition);
	SAVE_DOUBLE("RTCC_SplLat", SplashLatitude);
	SAVE_DOUBLE("RTCC_SplLng", SplashLongitude);
	SAVE_DOUBLE("RTCC_TEI", calcParams.TEI);
	SAVE_DOUBLE("RTCC_EI", calcParams.EI);
	SAVE_DOUBLE("RTCC_TLI", calcParams.TLI);
	SAVE_DOUBLE("RTCC_LOI", calcParams.LOI);
	SAVE_DOUBLE("RTCC_SEP", calcParams.SEP);
	SAVE_DOUBLE("RTCC_DOI", calcParams.DOI);
	SAVE_DOUBLE("RTCC_PDI", calcParams.PDI);
	SAVE_DOUBLE("RTCC_TLAND", calcParams.TLAND);
	SAVE_DOUBLE("RTCC_LSAlt", calcParams.LSAlt);
	SAVE_DOUBLE("RTCC_LSAzi", calcParams.LSAzi);
	SAVE_DOUBLE("RTCC_LSLat", calcParams.LSLat);
	SAVE_DOUBLE("RTCC_LSLng", calcParams.LSLng);
	SAVE_DOUBLE("RTCC_LunarLiftoff", calcParams.LunarLiftoff);
	SAVE_DOUBLE("RTCC_Insertion", calcParams.Insertion);
	SAVE_DOUBLE("RTCC_Phasing", calcParams.Phasing);
	SAVE_DOUBLE("RTCC_CSI", calcParams.CSI);
	SAVE_DOUBLE("RTCC_CDH", calcParams.CDH);
	SAVE_DOUBLE("RTCC_TPI", calcParams.TPI);
	SAVE_DOUBLE("RTCC_alt_node", calcParams.alt_node);
	SAVE_DOUBLE("RTCC_GET_node", calcParams.GET_node);
	SAVE_DOUBLE("RTCC_lat_node", calcParams.lat_node);
	SAVE_DOUBLE("RTCC_lng_node", calcParams.lng_node);
	SAVE_DOUBLE("RTCC_TEPHEM", calcParams.TEPHEM);
	SAVE_DOUBLE("RTCC_PericynthionLatitude", calcParams.PericynthionLatitude);
	SAVE_DOUBLE("RTCC_TIGSTORE1", calcParams.TIGSTORE1);
	// Strings
	// Vectors
	SAVE_V3("RTCC_DVLVLH", DeltaV_LVLH);
	SAVE_V3("RTCC_R_TLI", calcParams.R_TLI);
	SAVE_V3("RTCC_V_TLI", calcParams.V_TLI);
	SAVE_V3("RTCC_DVSTORE1", calcParams.DVSTORE1);
	// Matrizes
	SAVE_M3("RTCC_StoredREFSMMAT", calcParams.StoredREFSMMAT);
	// State vectors
	papiWriteScenario_SV(scn, "RTCC_SVSTORE1", calcParams.SVSTORE1);
	oapiWriteLine(scn, RTCC_END_STRING);
}

// Load State
void RTCC::LoadState(FILEHANDLE scn) {
	char *line;
	int tmp = 0; // Used in boolean type loader

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, RTCC_END_STRING, sizeof(RTCC_END_STRING))) {
			break;
		}
		LOAD_INT("RTCC_REFSMMATType", REFSMMATType);
		LOAD_DOUBLE("RTCC_P30TIG", TimeofIgnition);
		LOAD_DOUBLE("RTCC_SplLat", SplashLatitude);
		LOAD_DOUBLE("RTCC_SplLng", SplashLongitude);
		LOAD_DOUBLE("RTCC_TEI", calcParams.TEI);
		LOAD_DOUBLE("RTCC_EI", calcParams.EI);
		LOAD_DOUBLE("RTCC_TLI", calcParams.TLI);
		LOAD_DOUBLE("RTCC_LOI", calcParams.LOI);
		LOAD_DOUBLE("RTCC_SEP", calcParams.SEP);
		LOAD_DOUBLE("RTCC_DOI", calcParams.DOI);
		LOAD_DOUBLE("RTCC_PDI", calcParams.PDI);
		LOAD_DOUBLE("RTCC_TLAND", calcParams.TLAND);
		LOAD_DOUBLE("RTCC_LSAlt", calcParams.LSAlt);
		LOAD_DOUBLE("RTCC_LSAzi", calcParams.LSAzi);
		LOAD_DOUBLE("RTCC_LSLat", calcParams.LSLat);
		LOAD_DOUBLE("RTCC_LSLng", calcParams.LSLng);
		LOAD_DOUBLE("RTCC_LunarLiftoff", calcParams.LunarLiftoff);
		LOAD_DOUBLE("RTCC_Insertion", calcParams.Insertion);
		LOAD_DOUBLE("RTCC_Phasing", calcParams.Phasing);
		LOAD_DOUBLE("RTCC_CSI", calcParams.CSI);
		LOAD_DOUBLE("RTCC_CDH", calcParams.CDH);
		LOAD_DOUBLE("RTCC_TPI", calcParams.TPI);
		LOAD_DOUBLE("RTCC_alt_node", calcParams.alt_node);
		LOAD_DOUBLE("RTCC_GET_node", calcParams.GET_node);
		LOAD_DOUBLE("RTCC_lat_node", calcParams.lat_node);
		LOAD_DOUBLE("RTCC_lng_node", calcParams.lng_node);
		LOAD_DOUBLE("RTCC_TEPHEM", calcParams.TEPHEM);
		LOAD_DOUBLE("RTCC_PericynthionLatitude", calcParams.PericynthionLatitude);
		LOAD_DOUBLE("RTCC_TIGSTORE1", calcParams.TIGSTORE1);
		LOAD_V3("RTCC_DVLVLH", DeltaV_LVLH);
		LOAD_V3("RTCC_R_TLI", calcParams.R_TLI);
		LOAD_V3("RTCC_V_TLI", calcParams.V_TLI);
		LOAD_V3("RTCC_DVSTORE1", calcParams.DVSTORE1);
		LOAD_M3("RTCC_StoredREFSMMAT", calcParams.StoredREFSMMAT);
		papiReadScenario_SV(line, "RTCC_SVSTORE1", calcParams.SVSTORE1);
	}
	return;
}

void RTCC::SetManeuverData(double TIG, VECTOR3 DV)
{
	TimeofIgnition = (TIG - getGETBase())*24.0*3600.0;
	DeltaV_LVLH = DV;
}

int RTCC::SPSRCSDecision(double a, VECTOR3 dV_LVLH)
{
	double t;

	t = length(dV_LVLH) / a;

	if (t > 0.5)
	{
		return RTCC_ENGINETYPE_CSMSPS;
	}
	else
	{
		return RTCC_ENGINETYPE_CSMRCSPLUS4;
	}
}

SV RTCC::ExecuteManeuver(SV sv, double GETbase, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster)
{
	MATRIX3 Q_Xx;
	VECTOR3 V_G;

	return ExecuteManeuver(sv, GETbase, P30TIG, dV_LVLH, attachedMass, Thruster, Q_Xx, V_G);
}

SV RTCC::ExecuteManeuver(SV sv, double GETbase, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster, MATRIX3 &Q_Xx, VECTOR3 &V_G)
{
	//INPUT:
	//vessel: vessel interface
	//GETbase: usually launch MJD
	//P30TIG: Time of ignition in seconds relative to GETbase
	//dV_LVLH: DV Vector in LVLH coordinates
	//sv: state vector on trajectory before the maneuver, doesn't have to be at TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//optional:
	//F: thrust in Newton
	//isp: specific impulse in m/s
	//OUTPUT:
	//sv3: state vector at cutoff

	double t_go, theta_T, F, v_e, F_average;
	VECTOR3 UX, UY, UZ, DV, DV_P, DV_C;
	SV sv2, sv3;

	EngineParametersTable(Thruster, F, v_e);

	if (Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		double ManPADBurnTime, bt;
		int step;

		//Estimates for average thrust (relevant for finite burntime compensation), complete and variable burntime
		LMThrottleProgram(F, v_e, attachedMass + sv.mass, length(dV_LVLH), F_average, ManPADBurnTime, bt, step);
	}
	else
	{
		F_average = F;
	}

	OrbMech::oneclickcoast(sv.R, sv.V, sv.MJD, P30TIG - (sv.MJD - GETbase)*24.0*3600.0, sv2.R, sv2.V, sv.gravref, sv2.gravref);
	sv2.mass = sv.mass;
	sv2.MJD = GETbase + P30TIG / 24.0 / 3600.0;

	UY = unit(crossp(sv2.V, sv2.R));
	UZ = unit(-sv2.R);
	UX = crossp(UY, UZ);
	Q_Xx = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	DV = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	DV_P = UX*dV_LVLH.x + UZ*dV_LVLH.z;

	if (length(DV_P) != 0.0)
	{
		theta_T = length(crossp(sv2.R, sv2.V))*length(dV_LVLH)*(sv2.mass + attachedMass) / OrbMech::power(length(sv2.R), 2.0) / F;
		DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, UY))*sin(theta_T / 2.0))*length(DV_P);
		V_G = DV_C + UY*dV_LVLH.y;
	}
	else
	{
		V_G = UX*dV_LVLH.x + UY*dV_LVLH.y + UZ*dV_LVLH.z;
	}

	OrbMech::poweredflight(sv2.R, sv2.V, sv2.MJD, sv2.gravref, F_average, v_e, sv2.mass + attachedMass, V_G, sv3.R, sv3.V, sv3.mass, t_go);
	sv3.gravref = sv2.gravref;
	sv3.MJD = sv2.MJD + t_go / 24.0 / 3600.0;
	sv3.mass -= attachedMass;

	return sv3;
}

void RTCC::RTEMoonTargeting(RTEMoonOpt *opt, EntryResults *res)
{
	RTEMoon* teicalc;
	SV sv0, sv1, sv2;
	bool endi = false;
	double EMSAlt, dt22, MJDguess, LMmass, TZMINI;
	VECTOR3 R05G, V05G;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");

	EMSAlt = 297431.0*0.3048;

	sv0 = opt->RV_MCC;

	if (opt->TIGguess == 0.0)
	{
		//TEI targeting uses "now" as the initial guess
		MJDguess = sv0.MJD;

		if (opt->RevsTillTEI != 0)
		{
			double t_period, dt;
			t_period = OrbMech::period(sv0.R, sv0.V, OrbMech::mu_Moon);
			dt = t_period * (double)opt->RevsTillTEI;
			MJDguess += dt / 24.0 / 3600.0;
		}
	}
	else
	{
		MJDguess = opt->GETbase + opt->TIGguess / 24.0 / 3600.0;
	}

	sv1 = coast(sv0, (MJDguess - sv0.MJD)*24.0*3600.0);

	if (opt->SMODE == 12 || opt->SMODE == 14 || opt->SMODE == 16)
	{
		sv2 = sv1;
	}
	else
	{
		double TIG = OrbMech::P29TimeOfLongitude(sv1.R, sv1.V, sv1.MJD, hMoon, 180.0*RAD);
		sv2 = coast(sv1, (TIG - sv1.MJD)*24.0*3600.0);
	}

	if (opt->returnspeed != -1)
	{
		double DT_TEI_EI;

		DT_TEI_EI = 62.0*3600.0;

		if (opt->returnspeed == 0)
		{
			DT_TEI_EI += 24.0*3600.0;
		}
		else if (opt->returnspeed == 2)
		{
			DT_TEI_EI -= 24.0*3600.0;
		}

		double t0 = OrbMech::GETfromMJD(sv2.MJD, opt->GETbase);
		TZMINI = t0 + DT_TEI_EI;
	}
	else
	{
		TZMINI = opt->t_zmin;
	}

	double LINE[10];

	if (opt->entrylongmanual)
	{
		LINE[0] = PI05;
		LINE[1] = opt->EntryLng;
		LINE[2] = -PI05;
		LINE[3] = opt->EntryLng;
		LINE[4] = 1e10;
		LINE[5] = 1e10;
	}
	else
	{
		if (opt->ATPLine < 0 || opt->ATPLine>4)
		{
			return;
		}

		for (int i = 0;i < 10;i++)
		{
			LINE[i] = PZREAP.ATPCoordinates[opt->ATPLine][i];
		}
	}
	
	teicalc = new RTEMoon(sv2.R, sv2.V, sv2.MJD, sv2.gravref, opt->GETbase, LINE);
	teicalc->READ(opt->SMODE, PZREAP.IRMAX, PZREAP.VRMAX, PZREAP.RRBIAS, PZREAP.MOTION, PZREAP.HMINMC, 2, 0.3, PZREAP.DVMAX, 0.0, opt->Inclination, 1.0*1852.0, TZMINI, 0.0);

	endi = teicalc->MASTER();

	if (endi == false)
	{
		delete teicalc;
		return;
	}

	dt22 = OrbMech::time_radius(teicalc->R_EI, teicalc->V_EI, OrbMech::R_Earth + EMSAlt, -1, OrbMech::mu_Earth);
	OrbMech::rv_from_r0v0(teicalc->R_EI, teicalc->V_EI, dt22, R05G, V05G, OrbMech::mu_Earth); //Entry Interface to 0.05g

	res->latitude = teicalc->EntryLatcor;
	res->longitude = teicalc->EntryLngcor;
	res->P30TIG = (teicalc->TIG - opt->GETbase)*24.0*3600.0;
	res->dV_LVLH = teicalc->Entry_DV;
	res->ReA = teicalc->EntryAng;
	res->GET400K = (teicalc->EIMJD - opt->GETbase)*24.0*3600.0;
	res->GET05G = res->GET400K + dt22;
	res->RTGO = OrbMech::CMCEMSRangeToGo(R05G, OrbMech::MJDfromGET(res->GET05G, opt->GETbase), res->latitude, res->longitude);
	res->VIO = length(V05G);
	res->precision = teicalc->precision;
	res->Incl = teicalc->ReturnInclination;
	res->FlybyAlt = teicalc->FlybyPeriAlt;
	res->solutionfound = true;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	SV sv_tig;

	sv_tig.gravref = hMoon;
	sv_tig.mass = sv0.mass;
	sv_tig.MJD = teicalc->TIG;
	sv_tig.R = teicalc->Rig;
	sv_tig.V = teicalc->Vig;

	PoweredFlightProcessor(sv_tig, opt->GETbase, 0.0, opt->enginetype, LMmass, teicalc->Vig_apo - teicalc->Vig, false, res->P30TIG, res->dV_LVLH, res->sv_preburn, res->sv_postburn);

	delete teicalc;

	PZREAP.RTEPrimaryData.IgnitionGET = res->P30TIG;
	PZREAP.RTEPrimaryData.IgnitionGMT = GMTfromGET(res->P30TIG);
	PZREAP.RTEPrimaryData.UplinkDV = res->dV_LVLH;
	PZREAP.RTEPrimaryData.ThrusterCode = opt->enginetype;
}

void RTCC::LunarOrbitMapUpdate(SV sv0, double GETbase, AP10MAPUPDATE &pad, double pm)
{
	double ttoLOS, ttoAOS, ttoSS, ttoSR, ttoPM;
	OBJHANDLE hEarth, hSun;

	double t_lng;

	hEarth = oapiGetObjectByName("Earth");
	hSun = oapiGetObjectByName("Sun");

	ttoLOS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hEarth, 0, 0, true);
	ttoAOS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hEarth, 1, 0, true);

	pad.LOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoLOS;
	pad.AOSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoAOS;

	ttoSS = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hSun, 0, 0, true);
	ttoSR = OrbMech::sunrise(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hSun, 1, 0, true);

	pad.SSGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoSS;
	pad.SRGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoSR;

	t_lng = OrbMech::P29TimeOfLongitude(sv0.R, sv0.V, sv0.MJD, sv0.gravref, pm);
	ttoPM = (t_lng - sv0.MJD)*24.0 * 3600.0;
	pad.PMGET = (sv0.MJD - GETbase)*24.0*3600.0 + ttoPM;
}

void RTCC::LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad)
{
	SV sv1;
	VECTOR3 R_P, u;
	double dt1, dt2, MJDguess, sinl, gamma, r_0, LmkRange;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	for (int i = 0;i < opt->entries;i++)
	{
		MJDguess = opt->GETbase + opt->LmkTime[i] / 24.0 / 3600.0;
		sv1 = coast(opt->sv0, (MJDguess - opt->sv0.MJD)*24.0*3600.0);

		R_P = unit(_V(cos(opt->lng[i])*cos(opt->lat[i]), sin(opt->lng[i])*cos(opt->lat[i]), sin(opt->lat[i])))*(oapiGetSize(sv1.gravref) + opt->alt[i]);

		dt1 = OrbMech::findelev_gs(sv1.R, sv1.V, R_P, MJDguess, 180.0*RAD, sv1.gravref, LmkRange);
		dt2 = OrbMech::findelev_gs(sv1.R, sv1.V, R_P, MJDguess, 145.0*RAD, sv1.gravref, LmkRange);

		pad.T1[i] = dt1 + (MJDguess - opt->GETbase) * 24.0 * 60.0 * 60.0;
		pad.T2[i] = dt2 + (MJDguess - opt->GETbase) * 24.0 * 60.0 * 60.0;

		u = unit(R_P);
		sinl = u.z;

		if (sv1.gravref == hEarth)
		{
			double a, b, r_F;
			a = 6378166;
			b = 6356784;
			gamma = b * b / a / a;
			r_F = sqrt(b*b / (1.0 - (1.0 - b * b / a / a)*(1.0 - sinl * sinl)));
			r_0 = r_F;
		}
		else
		{
			gamma = 1.0;
			r_0 = OrbMech::R_Moon;
		}

		pad.Alt[i] = (length(R_P) - r_0) / 1852.0;
		pad.CRDist[i] = LmkRange / 1852.0;

		pad.Lat[i] = atan2(u.z, gamma*sqrt(u.x*u.x + u.y*u.y))*DEG;
		pad.Lng05[i] = opt->lng[i] / 2.0*DEG;
	}

	pad.entries = opt->entries;
}

MPTSV RTCC::coast(MPTSV sv0, double dt)
{
	MPTSV sv1;
	OBJHANDLE gravout = NULL;

	OrbMech::oneclickcoast(sv0.R, sv0.V, sv0.MJD, dt, sv1.R, sv1.V, sv0.gravref, gravout);
	sv1.gravref = gravout;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}

SV RTCC::coast(SV sv0, double dt)
{
	SV sv1;
	OBJHANDLE gravout = NULL;

	OrbMech::oneclickcoast(sv0.R, sv0.V, sv0.MJD, dt, sv1.R, sv1.V, sv0.gravref, gravout);
	sv1.gravref = gravout;
	sv1.mass = sv0.mass;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}

EphemerisData RTCC::coast(EphemerisData sv1, double dt)
{
	EphemerisData sv2;

	double MJD = OrbMech::MJDfromGET(sv1.GMT, GMTBASE);
	OrbMech::oneclickcoast(sv1.R, sv1.V, MJD, dt, sv2.R, sv2.V, sv1.RBI, sv2.RBI);
	sv2.GMT = sv1.GMT + dt;
	return sv2;
}

MPTSV RTCC::coast_conic(MPTSV sv0, double dt)
{
	MPTSV sv1;
	double mu = GGRAV * oapiGetMass(sv0.gravref);

	OrbMech::rv_from_r0v0(sv0.R, sv0.V, dt, sv1.R, sv1.V, mu);
	sv1.gravref = sv0.gravref;
	sv1.MJD = sv0.MJD + dt / 24.0 / 3600.0;

	return sv1;
}

void RTCC::GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD)
{
	VECTOR3 RIgn, VIgn;
	double GETbase;
	SV sv, sv2;

	GETbase = getGETBase();

	IgnMJD = GETbase + TimeofIgnition / 24.0 / 3600.0;

	sv = StateVectorCalc(calcParams.src);

	sv2 = coast(sv, (IgnMJD - sv.MJD)*24.0*3600.0);

	//RIgn = tmul(Rot, sv2.R);
	//VIgn = tmul(Rot, sv2.V);
	RIgn = sv2.R;
	VIgn = sv2.V;

	RIgn_global = _V(RIgn.x, RIgn.z, RIgn.y);
	VIgn_global = _V(VIgn.x, VIgn.z, VIgn.y);
	dV_LVLH = DeltaV_LVLH;
}

bool RTCC::REFSMMATDecision(VECTOR3 Att)
{
	if (cos(Att.z) > 0.5) //Yaw between 300° and 60°
	{
		return true;
	}

	return false;
}

SevenParameterUpdate RTCC::TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double GETbase, double P30TIG, double TB5, double mu, double T_RG)
{
	//Inputs:
	//
	//R_TLI: TLI cutoff position vector, right-handed, ecliptic coordinate system
	//V_TLI: TLI cutoff velocity vector, right-handed, ecliptic coordinate system

	MATRIX3 Rot;
	VECTOR3 R_TLI2, V_TLI2;
	double T_RP, tb5start;
	SevenParameterUpdate param;
	OELEMENTS coe;

	tb5start = TB5 - 17.0;
	T_RP = P30TIG - tb5start - T_RG;

	Rot = OrbMech::GetRotationMatrix(BODY_EARTH, GETbase - 17.0 / 24.0 / 3600.0);

	R_TLI2 = rhtmul(Rot, R_TLI);
	V_TLI2 = rhtmul(Rot, V_TLI);

	coe = OrbMech::coe_from_PACSS4(R_TLI2, V_TLI2, mu);

	param.alpha_D = coe.w;
	param.C3 = coe.h;
	param.e = coe.e;
	param.f = coe.TA;
	param.Inclination = coe.i;
	param.theta_N = coe.RA;
	param.T_RP = T_RP;

	return param;
}

int RTCC::PMMSPT(PMMSPTInput &in)
{
	EphemerisDataTable ephtab;
	EphemerisData sv_out;
	EMSMISSInputTable emsin;
	double T_TH, alpha_TSS, alpha_TS, beta, T_RP, f, R_N, KP0, KY0, T3_apo, T_ST, tau3R, T2, Vex2, Mdot2, DV_BR, tau2N;
	int J, OrigTLIInd, CurTLIInd;

	if (in.QUEID == 39)
	{
		T_TH = in.GMT;
		goto RTCC_PMMSPT_3_1;
		//goto RTCC_PMMSPT_7_1;
	}
	if (in.QUEID != 38)
	{
		goto RTCC_PMMSPT_3_1;
	}

RTCC_PMMSPT_3_1:
	if (in.QUEID == 32 || in.QUEID == 33)
	{
		//Read in maneuver block
		//J = abs(Original TLI Ind)
		//goto RTCC_PMMSPT_4_1;
	}
	T_RP = in.T_RP;
	if (in.QUEID == 34)
	{
		//TBD: load data from study aid
		goto RTCC_PMMSPT_3_2;
	}
	if (T_RP > 0)
	{
		OrigTLIInd = -in.InjOpp;
	}
	else
	{
	RTCC_PMMSPT_3_2:
		OrigTLIInd = in.InjOpp;
	}

	CurTLIInd = OrigTLIInd;
	J = in.InjOpp;
	int LD = GZGENCSN.DayofLiftoff;
	T_ST = PZSTARGP.T_ST[J - 1];
	alpha_TSS = PZSTARGP.alpha_TS[J - 1];
	beta = PZSTARGP.beta[J - 1];
	f = PZSTARGP.f[J - 1];
	R_N = PZSTARGP.R_N[J - 1];
	KP0 = PZSTARGP.KP0[J - 1];
	KY0 = PZSTARGP.KY0[J - 1];
	T3_apo = PZSTARGP.T3_apo[J - 1];
	tau3R = PZSTARGP.tau3R[J - 1];
	T2 = PZSTARGP.T2[J - 1];
	Vex2 = PZSTARGP.Vex2[J - 1];
	Mdot2 = PZSTARGP.Mdot2[J - 1];
	DV_BR = PZSTARGP.DV_BR[J - 1];
	tau2N = PZSTARGP.tau2N[J - 1];

	if (OrigTLIInd > 0)
	{
		goto RTCC_PMMSPT_6_3;
	}
	else if (OrigTLIInd == 0)
	{
		//goto RTCC_PMMSPT_8_2;
	}

	emsin.MaxIntegTime = T_RP - in.GMT;
	if (emsin.MaxIntegTime < 0)
	{
		emsin.IsForwardIntegration = false;
		emsin.MaxIntegTime = abs(emsin.MaxIntegTime);
		goto RTCC_PMMSPT_6_2;
	}
	else if (emsin.MaxIntegTime == 0.0)
	{
		//Move Time, R, V into Aux Output Table
		goto RTCC_PMMSPT_8_2;
	}
	else
	{
		goto RTCC_PMMSPT_6_1;
	}
RTCC_PMMSPT_6_1:
	emsin.IsForwardIntegration = true;
RTCC_PMMSPT_6_2:
	emsin.EphemerisBuildIndicator = false;
	goto RTCC_PMMSPT_7_2;
RTCC_PMMSPT_6_3:
	T_TH = MDVSTP.T4C + GetGMTLO()*3600.0 + T_ST;
	if (in.QUEID != 37)
	{
		goto RTCC_PMMSPT_7_1;
	}
	if (T_TH < in.PresentGMT)
	{
		T_TH = in.PresentGMT;
	}
	if (in.ReplaceCode != 0)
	{
		goto RTCC_PMMSPT_7_1;
	}
	in.T_RP = T_TH;
	goto RTCC_PMMSPT_19_1;

RTCC_PMMSPT_7_1:
	emsin.EphemerisLeftLimitGMT = T_TH;
	emsin.EphemerRightLimitGMT = T_TH + 1.2*3600.0;
	emsin.MinEphemDT = 0.0;
	emsin.MinNumEphemPoints = 9;
	emsin.ECIEphemerisIndicator = true;
	emsin.ECTEphemerisIndicator = false;
	emsin.MCIEphemerisIndicator = false;
	emsin.MCTEphemerisIndicator = false;
	emsin.EphemerisBuildIndicator = true;
	emsin.EphemTableIndicator = &ephtab;
RTCC_PMMSPT_7_2:
	emsin.AnchorVector.R = in.R;
	emsin.AnchorVector.V = in.V;
	emsin.AnchorVector.GMT = in.GMT;
	emsin.AnchorVector.RBI = BODY_EARTH;
	emsin.ManCutoffIndicator = 2;
	emsin.CutoffIndicator = 0;
	emsin.ManeuverIndicator = false;
	emsin.VehicleCode = in.Table;
	emsin.DensityMultiplier = 1.0;
	emsin.ManTimesIndicator = NULL;
	emsin.AuxTableIndicator = NULL;
	emsin.IgnoreManueverNumber = 0;
	EMSMISS(emsin);
	if (emsin.NIAuxOutputTable.ErrorCode)
	{
		return 72;
	}
RTCC_PMMSPT_8_2:
	double lambda = MCLGRA + GetGMTLO()*3600.0*OrbMech::w_Earth;
	MATRIX3 RMAT = mul(MatrixRH_LH(OrbMech::GetRotationMatrix(BODY_EARTH, GMTBASE)), _M(cos(lambda), -sin(lambda), 0, sin(lambda), cos(lambda), 0, 0, 0, 1));
	MATRIX3 M = mul(_M(1, 0, 0, 0, 0, -1, 0, 1, 0), OrbMech::tmat(RMAT));
	double dt4 = (MDVSTP.T4C - MDVSTP.T4IG) - MDVSTP.DT4N;
	double dt4_apo;
	if (abs(dt4) > MDVSTP.DTLIM)
	{
		dt4_apo = MDVSTP.DTLIM*dt4 / abs(dt4);
	}
	else
	{
		dt4_apo = dt4;
	}
	double t_D = GetGMTLO()*3600.0 - PZSTARGP.T_LO;
	double cos_sigma, C3, e_N, RA, DEC;
	PCMSP2(J, t_D, cos_sigma, C3, e_N, RA, DEC);
	VECTOR3 TargetVector = _V(cos(RA)*cos(DEC), sin(RA)*cos(DEC), sin(DEC));
	double theta_E = PZSTARGP.theta_EO + PZSTARGP.omega_E * t_D;
	MATRIX3 N = mul(RMAT, _M(cos(theta_E), sin(theta_E), 0, -sin(theta_E), cos(theta_E), 0, 0, 0, 1));
	VECTOR3 T_P = mul(N, TargetVector);
	VECTOR3 R = in.R;
	VECTOR3 V = in.V;
	double RR = dotp(R, R);
	double COSB = cos(beta);
	double SINB = sin(beta);
	if (OrigTLIInd < 0)
	{
		//goto  RTCC_PMMSPT_13_2;
	}
	alpha_TS = alpha_TSS + PZSTARGP.K_a1*dt4_apo + PZSTARGP.K_a2*dt4_apo*dt4_apo;
	double COSATS = cos(alpha_TS);
	double t = T_TH;
	VECTOR3 P = V * RR - R * dotp(R, V);
	double p = length(P);
	double r = length(R);
	double vv = dotp(V, V);
	double C1 = COSB / r;
	double C2 = SINB / p;
	VECTOR3 S = R * C1 + P * C2;
	VECTOR3 P_dot = V * dotp(R, V) - R * vv;
	double p_dot = length(P_dot);
	C2 = SINB / p;
	VECTOR3 S_dot = V * C1 + P_dot * C2;
	if (dotp(S_dot, T_P) < 0 && dotp(S, T_P) <= COSATS)
	{
		T_RP = t;
		goto RTCC_PMMSPT_14_1;
	}
	int KX = 1;
	VECTOR3 Hbar, H_apo, W;
	double h, w, du_apo, c, theta, du;
	int err;
	unsigned ORER_out;
RTCC_PMMSPT_12_2:
	RR = dotp(R, R);
	Hbar = crossp(R, V);
	h = length(Hbar);
	H_apo = Hbar / h;
	W = crossp(H_apo, T_P);
	w = length(W);
	du_apo = acos(COSATS / w);
	c = dotp(H_apo, T_P);
	P = T_P - H_apo * c;
	theta = atan2(dotp(crossp(R, P), H_apo), dotp(R, P));
	if (KX == 1 && theta < 0 && abs(theta)>du_apo)
	{
		theta = theta + PI2;
	}
	du = theta + du_apo - beta;
	if (abs(du) > 0.0001 && KX < 8)
	{
		t = t + du * RR / h;
		KX++;
		err = ELVARY(ephtab, 8, t, true, sv_out, ORER_out);
		if (err == 16 || err == 32)
		{
			return 77;
		}
		R = sv_out.R;
		V = sv_out.V;
		goto RTCC_PMMSPT_12_2;
	}
	T_RP = t;
	P = V * RR - R * dotp(R, V);
	p = length(P);
	r = length(R);
	C1 = COSB / r;
	C2 = SINB / p;
	S = R * C1 + P * C2;
RTCC_PMMSPT_14_1:
	double cos_psi = dotp(S, T_P);
	double sin_psi = sqrt(1.0 - cos_psi * cos_psi);
	VECTOR3 Sbar_1 = (S*cos_psi-T_P) / sin_psi;
	VECTOR3 Cbar_1 = crossp(Sbar_1, S);
	VECTOR3 Omega_X = _V(M.m11, M.m12, M.m13);
	VECTOR3 Omega_Y = _V(M.m21, M.m22, M.m23);
	VECTOR3 Omega_Z = _V(M.m31, M.m32, M.m33);
	double i = acos(dotp(Omega_Y, Cbar_1));
	double X1 = dotp(Omega_Z, crossp(Cbar_1, Omega_Y));
	double X2 = dotp(Omega_X, crossp(Cbar_1, Omega_Y));
	double theta_N = atan2(X1, X2);
	if (theta_N < 0)
	{
		theta_N += PI2;
	}
	if (in.QUEID == 34)
	{
		//goto 21_1
	}
	double P_N = (OrbMech::mu_Earth / C3)*(e_N*e_N - 1.0);
	double T_M = P_N / (1.0 - e_N * cos_sigma);
	double alpha_D_apo = acos(cos_psi) + atan2(dotp(Sbar_1, crossp(Cbar_1, Omega_Y)), dotp(S, crossp(Cbar_1, Omega_Y)));
	in.CurMan->dV_inertial.x = i;
	in.CurMan->dV_inertial.y = theta_N;
	in.CurMan->dV_inertial.z = e_N;
	in.CurMan->dV_LVLH.x = C3;
	in.CurMan->dV_LVLH.y = alpha_D_apo;
	in.CurMan->dV_LVLH.z = f;
	in.CurMan->Word67d = R_N;
	in.CurMan->Word69 = T_M;
	goto RTCC_PMMSPT_15_2;
//RTCC_PMMSPT_15_1:
RTCC_PMMSPT_15_2:
	double P_RP = KP0 + MDVSTP.KP1 * dt4_apo + MDVSTP.KP2 * dt4_apo*dt4_apo;
	double Y_RP = KP0 + MDVSTP.KY1 * dt4_apo + MDVSTP.KY2 * dt4_apo*dt4_apo;
	double T3 = T3_apo - dt4_apo;
	double tau3 = tau3R - dt4_apo;
	in.CurMan->Word73 = P_RP;
	in.CurMan->Word74 = Y_RP;
	in.CurMan->Word76 = T3;
	in.CurMan->Word77 = tau3;
	in.CurMan->Word78i[0] = CurTLIInd;
	in.CurMan->Word78i[1] = OrigTLIInd;
	in.CurMan->Word79 = T2;
	in.CurMan->Word80 = Vex2;
	in.CurMan->Word81 = Mdot2;
	in.CurMan->Word82 = DV_BR;
	in.CurMan->Word83 = tau2N;

	double A_Z = 0.0;
	if (t_D < MDVSTP.t_DS1)
	{
		if (t_D < MDVSTP.t_DS0)
		{
			//Some message
		}
		for (int N = 0;N < 4;N++)
		{
			A_Z += MDVSTP.hx[0][N] * pow((t_D - MDVSTP.t_D1) / MDVSTP.t_SD1, N);
		}
	}
	else if (t_D < MDVSTP.t_DS2)
	{
		for (int N = 0;N < 4;N++)
		{
			A_Z += MDVSTP.hx[1][N] * pow((t_D - MDVSTP.t_D2) / MDVSTP.t_SD2, N);
		}
	}
	else
	{
		if (t_D > MDVSTP.t_DS3)
		{
			//Some message
		}
		for (int N = 0;N < 4;N++)
		{
			A_Z += MDVSTP.hx[2][N] * pow((t_D - MDVSTP.t_D3) / MDVSTP.t_SD3, N);
		}
	}
	double Azo = 72.0;
	double Azs = 36.0;
	double i_P = MDVSTP.fx[0] + MDVSTP.fx[1] * (A_Z - Azo) / Azs + MDVSTP.fx[2] * pow((A_Z - Azo) / Azs, 2) + MDVSTP.fx[3] * pow((A_Z - Azo) / Azs, 3)
		+ MDVSTP.fx[4] * pow((A_Z - Azo) / Azs, 4) + MDVSTP.fx[5] * pow((A_Z - Azo) / Azs, 5) + MDVSTP.fx[6] * pow((A_Z - Azo) / Azs, 6);
	double theta_N_P = MDVSTP.gx[0] + MDVSTP.gx[1] * (A_Z - Azo) / Azs + MDVSTP.gx[2] * pow((A_Z - Azo) / Azs, 2) + MDVSTP.gx[3] * pow((A_Z - Azo) / Azs, 3)
		+ MDVSTP.gx[4] * pow((A_Z - Azo) / Azs, 4) + MDVSTP.gx[5] * pow((A_Z - Azo) / Azs, 5) + MDVSTP.gx[6] * pow((A_Z - Azo) / Azs, 6);

	A_Z *= RAD;
	i_P *= RAD;
	theta_N_P *= RAD;
	MATRIX3 A = _M(cos(MDVSTP.PHIL), sin(MDVSTP.PHIL)*sin(A_Z), -sin(MDVSTP.PHIL)*cos(A_Z), -sin(MDVSTP.PHIL), cos(MDVSTP.PHIL)*sin(A_Z), -cos(MDVSTP.PHIL)*cos(A_Z), 0, cos(A_Z), sin(A_Z));
	MATRIX3 EPH = mul(OrbMech::tmat(A), M);
	MATRIX3 BB = _M(cos(theta_N_P), 0, sin(theta_N_P), sin(theta_N_P)*sin(i_P), cos(i_P), -cos(theta_N_P)*sin(i_P), -sin(theta_N_P)*cos(i_P), sin(i_P), cos(theta_N_P)*cos(i_P));
	MATRIX3 GG = mul(BB, A);
	MATRIX3 B = _M(cos(theta_N), 0, sin(theta_N), sin(theta_N)*sin(i), cos(i), -cos(theta_N)*sin(i), -sin(theta_N)*cos(i), sin(i), cos(theta_N)*cos(i));
	MATRIX3 G = mul(B, A);

	in.CurMan->GMTI = T_RP + MDVSTP.DTIG;
	in.CurMan->Word84 = MDVSTP.DTIG - MCTJD1;
//RTCC_PMMSPT_18_1:
	PZTLIMAT.EPH = EPH;
	PZTLIMAT.GG = GG;
	PZTLIMAT.G = G;

	if (in.QUEID != 36 && in.QUEID != 37 && in.QUEID != 39)
	{
		goto RTCC_PMMSPT_20_1;
	}
	if (T_RP <= in.StartTimeLimit || T_RP >= in.EndTimeLimit)
	{
		//Error
		return 79;
	}
	in.mpt->TimeToBeginManeuver[0] = in.mpt->TimeToEndManeuver[0] = T_RP;
	in.CurMan->CommonBlock.ConfigCode = in.CC;
	in.CurMan->CommonBlock.ConfigChangeInd = in.CCI;
	in.CurMan->AttitudeCode = in.AttitudeMode;
	in.CurMan->Thruster = in.ThrusterCode;
	in.CurMan->ConfigCodeBefore = in.CCMI;
	in.CurMan->TVC = in.TVC;
	in.CurMan->FrozenManeuverInd = false;
	in.CurMan->RefBodyInd = BODY_EARTH;
	in.CurMan->CoordSysInd = 0;
	in.CurMan->GMTMAN = T_RP;
	
	if (in.PrevMan)
	{
		in.CurMan->DockingAngle = in.PrevMan->DockingAngle;
	}
	else
	{
		in.CurMan->DockingAngle = in.mpt->DeltaDockingAngle;
	}
	in.CurMan->TrajDet[0] = in.CurMan->TrajDet[1] = in.CurMan->TrajDet[2] = 1;

RTCC_PMMSPT_19_1:
	return 0;
RTCC_PMMSPT_20_1:
	return 0;
}

void RTCC::PCMSP2(int J, double t_D, double &cos_sigma, double &C3, double &e_N, double &RA, double &DEC)
{
	int k = J - 1;
	int i = 0;
	double t_temp;
	bool direct = true;
	do
	{
		t_temp = PZSTARGP.t_D[k][i];
		if (t_D == t_temp)
		{
			break;
		}
		else if (t_D < t_temp)
		{
			direct = false;
			break;
		}
		else
		{
			if (i >= 14)
			{
				break;
			}
			else
			{
				i++;
			}
		}
	} while (i < 15);

	if (i == 0 || direct)
	{
		cos_sigma = PZSTARGP.cos_sigma[k][i];
		C3 = PZSTARGP.C_3[k][i];
		e_N = PZSTARGP.e_N[k][i];
		RA = PZSTARGP.RA[k][i];
		DEC = PZSTARGP.DEC[k][i];
		return;
	}

	double A = (t_D - PZSTARGP.t_D[k][i - 1]) / (PZSTARGP.t_D[k][i] - PZSTARGP.t_D[k][i - 1]);
	cos_sigma = PZSTARGP.cos_sigma[k][i - 1] + A * (PZSTARGP.cos_sigma[k][i] - PZSTARGP.cos_sigma[k][i - 1]);
	C3 = PZSTARGP.C_3[k][i - 1] + A * (PZSTARGP.C_3[k][i] - PZSTARGP.C_3[k][i - 1]);
	e_N = PZSTARGP.e_N[k][i - 1] + A * (PZSTARGP.e_N[k][i] - PZSTARGP.e_N[k][i - 1]);
	RA = PZSTARGP.RA[k][i - 1] + A * (PZSTARGP.RA[k][i] - PZSTARGP.RA[k][i - 1]);
	DEC = PZSTARGP.DEC[k][i - 1] + A * (PZSTARGP.DEC[k][i] - PZSTARGP.DEC[k][i - 1]);
}

void RTCC::LVDCTLIPredict(LVDCTLIparam lvdc, double m0, SV sv_A, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_IG, SV &sv_TLI)
{
	SV sv1, sv2;
	double day, theta_E, MJD_GRR, MJD_TST, dt, cos_psiT, sin_psiT, Inclination, X_1, X_2, theta_N, p_N, T_M, R, e, p, alpha_D;
	double MJD_TIG, R_T, V_T, K_5, G_T, gamma_T, boil, tau3, dL_3, F2, F3, V_ex2, V_ex3, dt_tailoff;
	VECTOR3 PosS, DotS, T_P, N, PosP, Sbar, DotP, Sbardot, Sbar_1, Cbar_1, R_TLI, V_TLI, TargetVector;
	MATRIX3 mat, MX_A, MX_EPH, MX_B, MX_G;
	OELEMENTS coe;

	//Constants
	boil = (1.0 - 0.99998193) / 10.0;
	F2 = 802543.5;
	V_ex2 = 4226.708571;
	V_ex3 = 4188.581306;
	F3 = 906114.066;
	dt_tailoff = 0.25538125;

	//State Vector
	modf(oapiGetSimMJD(), &day);
	if (lvdc.t_clock + lvdc.T_L > 24.0*3600.0)
	{
		day -= 1.0;
	}
	MJD_GRR = day + lvdc.T_L / 24.0 / 3600.0;
	mat = OrbMech::Orbiter2PACSS13(MJD_GRR, lvdc.phi_L, -80.6041140*RAD, lvdc.Azimuth);

	//Find TB6 start
	//Determination of S-bar and S-bar-dot
	theta_E = lvdc.theta_EO + lvdc.omega_E*lvdc.t_D;
	MX_A.m11 = cos(lvdc.phi_L);  MX_A.m12 = sin(lvdc.phi_L)*sin(lvdc.Azimuth); MX_A.m13 = -(sin(lvdc.phi_L)*cos(lvdc.Azimuth));
	MX_A.m21 = -sin(lvdc.phi_L); MX_A.m22 = cos(lvdc.phi_L)*sin(lvdc.Azimuth); MX_A.m23 = -(cos(lvdc.phi_L)*cos(lvdc.Azimuth));
	MX_A.m31 = 0;  MX_A.m32 = cos(lvdc.Azimuth);  MX_A.m33 = sin(lvdc.Azimuth);
	MX_EPH = mul(OrbMech::tmat(MX_A), _M(cos(theta_E), sin(theta_E), 0, 0, 0, -1, -sin(theta_E), cos(theta_E), 0));
	TargetVector = _V(cos(lvdc.RA)*cos(lvdc.DEC), sin(lvdc.RA)*cos(lvdc.DEC), sin(lvdc.DEC));
	T_P = mul(MX_EPH, unit(TargetVector));

	MJD_TST = MJD_GRR + (lvdc.T4C + 17.0 + lvdc.T_ST) / 24.0 / 3600.0;
	sv1 = coast(sv_A, (MJD_TST - sv_A.MJD) * 24.0 * 3600.0);

	dt = 0;

	do
	{
		sv2 = coast(sv1, dt);
		PosS = mul(mat, _V(sv2.R.x, sv2.R.z, sv2.R.y));
		DotS = mul(mat, _V(sv2.V.x, sv2.V.z, sv2.V.y));

		N = unit(crossp(PosS, DotS));
		PosP = crossp(N, unit(PosS));
		Sbar = unit(PosS)*cos(lvdc.beta) + PosP*sin(lvdc.beta);
		DotP = crossp(N, DotS / length(PosS));

		Sbardot = DotS / length(PosS)*cos(lvdc.beta) + DotP*sin(lvdc.beta);
		dt += 1.0;

	} while (!((dotp(Sbardot, T_P) < 0 && dotp(Sbar, T_P) <= cos(lvdc.alpha_TS))));

	//Advance to Ignition State
	sv_IG = coast(sv2, lvdc.T_RG);
	PosS = mul(mat, _V(sv_IG.R.x, sv_IG.R.z, sv_IG.R.y));
	DotS = mul(mat, _V(sv_IG.V.x, sv_IG.V.z, sv_IG.V.y));

	cos_psiT = dotp(Sbar,T_P);
	sin_psiT = sqrt(1.0 - pow(cos_psiT, 2));
	Sbar_1 = (Sbar*cos_psiT - T_P)*(1.0 / sin_psiT);
	Cbar_1 = crossp(Sbar_1, Sbar);
	Inclination = acos(dotp(_V(MX_A.m21, MX_A.m22, MX_A.m23),Cbar_1));
	X_1 = dotp(_V(MX_A.m31, MX_A.m32, MX_A.m33), crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23)));
	X_2 = dotp(_V(MX_A.m11, MX_A.m12, MX_A.m13), crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23)));
	theta_N = atan2(X_1, X_2);
	p_N = lvdc.mu / lvdc.C_3*(pow(lvdc.e_N, 2) - 1.0);
	T_M = p_N / (1.0 - lvdc.e_N*lvdc.cos_sigma);
	R = length(PosS);
	e = R / lvdc.R_N*(lvdc.e_N - 1.0) + 1.0;
	p = lvdc.mu / lvdc.C_3*(pow(e, 2) - 1.0);

	alpha_D = acos(dotp(Sbar, T_P)) - acos((1.0 - p / T_M) / e) + atan2(dotp(Sbar_1, crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23))), dotp(Sbar, crossp(Cbar_1, _V(MX_A.m21, MX_A.m22, MX_A.m23))));

	MJD_TIG = MJD_TST + (dt + lvdc.T_RG) / 24.0 / 3600.0;
	P30TIG = (MJD_TIG - GETbase) * 24.0 * 3600.0;

	MX_B = _M(cos(theta_N), 0, sin(theta_N), sin(theta_N)*sin(Inclination), cos(Inclination), -cos(theta_N)*sin(Inclination),
		-sin(theta_N)*cos(Inclination), sin(Inclination), cos(theta_N)*cos(Inclination));
	MX_G = mul(MX_B, MX_A);
	R_T = p / (1.0 + e*cos(lvdc.f));
	K_5 = sqrt(lvdc.mu / p);
	V_T = K_5*sqrt(1.0 + 2.0 * e*cos(lvdc.f) + pow(e, 2));
	gamma_T = atan((e*sin(lvdc.f)) / (1.0 + cos(lvdc.f)));
	G_T = -lvdc.mu / pow(R_T, 2);

	int UP;
	double mass, a_T, tau2, V, sin_gam, cos_gam, dot_phi_1, dot_phi_T, phi_T, xi_T, dot_zeta_T, dot_xi_T, ddot_zeta_GT, ddot_xi_GT, m1, dt1;
	double dot_dxit, dot_detat, dot_dzetat, dV, f, T_1c, Tt_3, T_2, L_2, L_12, Lt_3, Lt_Y, Tt_T, dT_3, T_3, T_T, dT, Tt_T_old, m2, m3, a_T3, a_T_cut;
	VECTOR3 Pos4, PosXEZ, DotXEZ, ddotG_act, DDotXEZ_G;
	MATRIX3 MX_phi_T, MX_K;

	dT = 1.0;
	Tt_T_old = 0.0;
	UP = 0;
	T_2 = lvdc.T_2R;
	T_1c = T_2;
	Tt_3 = lvdc.Tt_3R;
	Tt_T = T_2 + Tt_3;

	mass = sv_A.mass;
	dt1 = dt + (MJD_TST - sv_A.MJD) * 24.0 * 3600.0;
	m1 = (mass - m0)*exp(-boil*dt1);
	m2 = m0 + m1;

	a_T = F2 / m2;
	tau2 = V_ex2 / a_T;
	m3 = m2 - F2 / V_ex2*T_2;
	a_T3 = F3 / m3;
	tau3 = V_ex3 / a_T3;

	while (abs(dT) > 0.01)
	{
		//IGM
		ddotG_act = -PosS*OrbMech::mu_Earth / OrbMech::power(length(PosS), 3.0);
		Pos4 = mul(MX_G, PosS);

		L_2 = V_ex2 * log(tau2 / (tau2 - T_2));
		L_12 = L_2;
		Lt_3 = V_ex3 * log(tau3 / (tau3 - Tt_3));

		Lt_Y = (L_12 + Lt_3);
gtupdate:
		V = length(DotS);
		R = length(PosS);
		sin_gam = ((PosS.x*DotS.x) + (PosS.y*DotS.y) + (PosS.z*DotS.z)) / (R*V);
		cos_gam = pow(1.0 - pow(sin_gam, 2), 0.5);
		dot_phi_1 = (V*cos_gam) / R;
		dot_phi_T = (V_T*cos(gamma_T)) / R_T;
		phi_T = atan2(Pos4.z, Pos4.x) + (((dot_phi_1 + dot_phi_T) / 2.0)*Tt_T);

		f = phi_T + alpha_D;
		R_T = p / (1 + ((e*(cos(f)))));
		V_T = K_5 * pow(1 + ((2 * e)*(cos(f))) + pow(e, 2), 0.5);
		gamma_T = atan2((e*(sin(f))), (1 + (e*(cos(f)))));
		G_T = -OrbMech::mu_Earth / pow(R_T, 2);

		xi_T = R_T*cos(gamma_T);
		dot_zeta_T = V_T;
		dot_xi_T = 0.0;
		ddot_zeta_GT = G_T*sin(gamma_T);
		ddot_xi_GT = G_T*cos(gamma_T);
		phi_T = phi_T - gamma_T;

		MX_phi_T.m11 = (cos(phi_T));    MX_phi_T.m12 = 0; MX_phi_T.m13 = ((sin(phi_T)));
		MX_phi_T.m21 = 0;               MX_phi_T.m22 = 1; MX_phi_T.m23 = 0;
		MX_phi_T.m31 = (-sin(phi_T)); MX_phi_T.m32 = 0; MX_phi_T.m33 = (cos(phi_T));
		MX_K = mul(MX_phi_T, MX_G);
		PosXEZ = mul(MX_K, PosS);
		DotXEZ = mul(MX_K, DotS);
		VECTOR3 RTT_T1, RTT_T2;
		RTT_T1.x = ddot_xi_GT; RTT_T1.y = 0;        RTT_T1.z = ddot_zeta_GT;
		RTT_T2 = ddotG_act;
		RTT_T2 = mul(MX_K, RTT_T2);
		RTT_T1 = RTT_T1 + RTT_T2;
		DDotXEZ_G = _V(0.5*RTT_T1.x, 0.5*RTT_T1.y, 0.5*RTT_T1.z);

		dot_dxit = dot_xi_T - DotXEZ.x - (DDotXEZ_G.x*Tt_T);
		dot_detat = -DotXEZ.y - (DDotXEZ_G.y * Tt_T);
		dot_dzetat = dot_zeta_T - DotXEZ.z - (DDotXEZ_G.z * Tt_T);

		dV = pow((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)), 0.5);
		dL_3 = (((pow(dot_dxit, 2) + pow(dot_detat, 2) + pow(dot_dzetat, 2)) / Lt_Y) - Lt_Y) / 2;

		dT_3 = (dL_3*(tau3 - Tt_3)) / V_ex3;
		T_3 = Tt_3 + dT_3;
		T_T = Tt_T + dT_3;

		// TARGET PARAMETER UPDATE
		if (!(UP > 0)) {
			UP = 1;
			Tt_3 = T_3;
			Tt_T = T_T;
			Lt_3 = Lt_3 + dL_3;
			Lt_Y = Lt_Y + dL_3;
			goto gtupdate; // Recycle. 
		}

		UP = -1;

		Tt_3 = T_3;
		T_1c = T_2;
		Tt_T = T_1c + Tt_3;

		dT = Tt_T - Tt_T_old;
		Tt_T_old = Tt_T;
	}

	coe.e = e;
	coe.h = lvdc.C_3;
	coe.i = Inclination;
	coe.RA = theta_N;
	coe.TA = f;
	coe.w = alpha_D;

	OrbMech::PACSS13_from_coe(coe, lvdc.phi_L, lvdc.Azimuth, OrbMech::mu_Earth, R_TLI, V_TLI);

	R_TLI = tmul(mat, R_TLI);
	V_TLI = tmul(mat, V_TLI);
	sv_TLI.R = _V(R_TLI.x, R_TLI.z, R_TLI.y);
	sv_TLI.V = _V(V_TLI.x, V_TLI.z, V_TLI.y);
	sv_TLI.MJD = OrbMech::MJDfromGET(P30TIG + Tt_T, GETbase);
	sv_TLI.mass = m3 - F3 / V_ex3 * T_3;
	sv_TLI.gravref = sv_A.gravref;

	a_T_cut = F3 / sv_TLI.mass;

	dV_LVLH = _V(1.0, 0.0, 0.0)*(dV - a_T_cut * dt_tailoff);
}

void RTCC::EngineParametersTable(int enginetype, double &Thrust, double &Isp)
{
	if (enginetype == RTCC_ENGINETYPE_CSMSPS)
	{
		Thrust = SPS_THRUST;
		Isp = SPS_ISP;
	}
	else if (enginetype == RTCC_ENGINETYPE_LMAPS)
	{
		Thrust = APS_THRUST;
		Isp = APS_ISP;
	}
	else if (enginetype == RTCC_ENGINETYPE_LMDPS)
	{
		Thrust = DPS_THRUST;
		Isp = DPS_ISP;
	}
	else if (enginetype == RTCC_ENGINETYPE_CSMRCSPLUS2 || enginetype == RTCC_ENGINETYPE_CSMRCSMINUS2 || enginetype == RTCC_ENGINETYPE_LMRCSPLUS2 || enginetype == RTCC_ENGINETYPE_LMRCSMINUS2)
	{
		Thrust = 441.5*2.0;
		Isp = 2706.64;
	}
	else if (enginetype == RTCC_ENGINETYPE_CSMRCSPLUS4 || enginetype == RTCC_ENGINETYPE_CSMRCSMINUS4 || enginetype == RTCC_ENGINETYPE_LMRCSPLUS4 || enginetype == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		Thrust = 441.5*4.0;
		Isp = 2706.64;
	}
	else if (enginetype == RTCC_ENGINETYPE_SIVB_APS)
	{
		Thrust = 2.0*310.0;
		Isp = 290.0 * G;
	}
	else if (enginetype == RTCC_ENGINETYPE_LOX_DUMP)
	{
		Thrust = 3300.0;
		Isp = 157.0;
	}
}

void RTCC::FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_tig, SV &sv_out, bool agc)
{
	//For CSM, LM or CSM/LM docked maneuvers
	//INPUT:
	//sv: State vector at impulsive TIG
	//attachedMass: mass of the attached vessel, 0 if no vessel present
	//DV: Delta velocity of impulsive thrusting maneuver
	//engine: see defintion of RTCC_ENGINETYPEs
	//
	//OUTPUT:
	//DV_imp: non-impulsive delta velocity
	//t_slip: time in seconds the maneuver has slipped to compensate for finite burntime; should always be negative
	//sv_out: complete state vector at actual cutoff

	VECTOR3 DV_imp_inert;
	double f_T, isp, F_average;

	sv_out.gravref = sv.gravref;

	if (engine == RTCC_ENGINETYPE_LMDPS)
	{
		double bt, bt_var;
		int step;

		EngineParametersTable(engine, f_T, isp);

		LMThrottleProgram(f_T, isp, sv.mass + attachedMass, length(DV), F_average, bt, bt_var, step);
	}
	else
	{
		EngineParametersTable(engine, f_T, isp);
		F_average = f_T;
	}

	OrbMech::impulsive(sv.R, sv.V, sv.MJD, sv.gravref, F_average, isp, sv.mass + attachedMass, sv.R, sv.V + DV, DV_imp_inert, t_slip, sv_out.R, sv_out.V, sv_out.MJD, sv_out.mass);

	sv_tig = coast(sv, t_slip);

	if (agc)
	{
		DV_imp = PIEXDV(sv_tig.R, sv_tig.V, sv.mass + attachedMass, f_T, DV_imp_inert, false);
	}
	else
	{
		DV_imp = PIAEDV(DV_imp_inert, sv_tig.R, sv_tig.V, sv_tig.R, false);
	}

	sv_out.mass -= attachedMass;
}

void RTCC::FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, bool agc)
{
	SV sv_tig, sv_cut;

	FiniteBurntimeCompensation(sv, attachedMass, DV, engine, DV_imp, t_slip, sv_tig, sv_cut, agc);
}

void RTCC::PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc)
{
	SV sv_pre, sv_post;

	PoweredFlightProcessor(sv0, GETbase, GET_TIG_imp, enginetype, attachedMass, DV, DVIsLVLH, GET_TIG, dV_LVLH, sv_pre, sv_post, agc);
}

void RTCC::PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, SV &sv_pre, SV &sv_post, bool agc)
{
	SV sv_imp;
	MATRIX3 Q_Xx;
	VECTOR3 DeltaV;
	double t_slip;

	//If TIG set to 0 use input SV MJD as TIG
	if (GET_TIG_imp == 0.0)
	{
		sv_imp = sv0;
		GET_TIG_imp = OrbMech::GETfromMJD(sv_imp.MJD, GETbase);
	}
	else
	{
		sv_imp = coast(sv0, GET_TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));
	}

	if (DVIsLVLH)
	{
		Q_Xx = OrbMech::LVLH_Matrix(sv_imp.R, sv_imp.V);
		DeltaV = tmul(Q_Xx, DV);
	}
	else
	{
		DeltaV = DV;
	}

	FiniteBurntimeCompensation(sv_imp, attachedMass, DeltaV, enginetype, dV_LVLH, t_slip, sv_pre, sv_post, agc);

	GET_TIG = GET_TIG_imp + t_slip;
}

VECTOR3 RTCC::ConvertDVtoLVLH(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_imp)
{
	MATRIX3 Q_Xx;
	SV sv_tig;

	sv_tig = coast(sv0, TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));
	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);
	return mul(Q_Xx, DV_imp);
}

VECTOR3 RTCC::ConvertDVtoInertial(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_LVLH_imp)
{
	MATRIX3 Q_Xx;
	SV sv_tig;

	sv_tig = coast(sv0, TIG_imp - OrbMech::GETfromMJD(sv0.MJD, GETbase));
	Q_Xx = OrbMech::LVLH_Matrix(sv_tig.R, sv_tig.V);
	return tmul(Q_Xx, DV_LVLH_imp);
}

double RTCC::GetDockedVesselMass(VESSEL *vessel)
{
	//INPUT:
	//vessel: primary vessel
	//OUTPUT:
	//mass of the docked vessel, 0 if no vessel docked

	DOCKHANDLE dock;
	OBJHANDLE hLM;
	VESSEL *lm;
	double LMmass;

	if (vessel->DockingStatus(0) == 1)
	{
		dock = vessel->GetDockHandle(0);
		hLM = vessel->GetDockStatus(dock);
		lm = oapiGetVesselInterface(hLM);

		//Special case: S-IVB, but we want the LM mass
		if (!stricmp(lm->GetClassName(), "ProjectApollo\\sat5stg3") ||
			!stricmp(lm->GetClassName(), "ProjectApollo/sat5stg3"))
		{
			SIVB *sivb = (SIVB *)lm;
			LMmass = sivb->GetPayloadMass();
		}
		else
		{
			LMmass = lm->GetMass();
		}
	}
	else
	{
		LMmass = 0.0;
	}

	return LMmass;
}

double RTCC::PericynthionTime(VESSEL *vessel)
{
	OBJHANDLE gravref;
	VECTOR3 R_A, V_A, R0, V0;
	double SVMJD, mu, dt, GETbase;

	GETbase = getGETBase();
	gravref = AGCGravityRef(vessel);

	mu = GGRAV*oapiGetMass(gravref);
	vessel->GetRelativePos(gravref, R_A);
	vessel->GetRelativeVel(gravref, V_A);
	SVMJD = oapiGetSimMJD();
	R0 = _V(R_A.x, R_A.z, R_A.y);
	V0 = _V(V_A.x, V_A.z, V_A.y);

	dt = OrbMech::timetoperi(R0, V0, mu);
	return (SVMJD-GETbase)*24.0*3600.0 + dt;
}

SV RTCC::FindPericynthion(SV sv0)
{
	OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	double dt = OrbMech::timetoperi_integ(sv0.R, sv0.V, sv0.MJD, sv0.gravref, hMoon);
	return coast(sv0, dt);
}

double RTCC::FindOrbitalMidnight(SV sv, double GETbase, double t_TPI_guess)
{
	SV sv1;
	double GET_SV, dt, ttoMidnight;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, GETbase);
	dt = t_TPI_guess - GET_SV;

	sv1 = coast(sv, dt);

	ttoMidnight = OrbMech::sunrise(sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, 1, 1, false);
	return t_TPI_guess + ttoMidnight;
}

void RTCC::FindRadarAOSLOS(SV sv, double GETbase, double lat, double lng, double &GET_AOS, double &GET_LOS)
{
	VECTOR3 R_P;
	double LmkRange, dt1, dt2;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt1 = OrbMech::findelev_gs(sv.R, sv.V, R_P, sv.MJD, 175.0*RAD, sv.gravref, LmkRange);
	dt2 = OrbMech::findelev_gs(sv.R, sv.V, R_P, sv.MJD, 5.0*RAD, sv.gravref, LmkRange);

	GET_AOS = OrbMech::GETfromMJD(sv.MJD, GETbase) + dt1;
	GET_LOS = OrbMech::GETfromMJD(sv.MJD, GETbase) + dt2;
}

void RTCC::FindRadarMidPass(SV sv, double GETbase, double lat, double lng, double &GET_Mid)
{
	VECTOR3 R_P;
	double LmkRange, dt;

	R_P = unit(_V(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat)))*oapiGetSize(sv.gravref);

	dt = OrbMech::findelev_gs(sv.R, sv.V, R_P, sv.MJD, 90.0*RAD, sv.gravref, LmkRange);

	GET_Mid = OrbMech::GETfromMJD(sv.MJD, GETbase) + dt;
}

double RTCC::FindOrbitalSunrise(SV sv, double GETbase, double t_sunrise_guess)
{
	SV sv1;
	double GET_SV, dt, ttoSunrise;

	OBJHANDLE hSun = oapiGetObjectByName("Sun");

	GET_SV = OrbMech::GETfromMJD(sv.MJD, GETbase);
	dt = t_sunrise_guess - GET_SV;

	sv1 = coast(sv, dt);

	ttoSunrise = OrbMech::sunrise(sv1.R, sv1.V, sv1.MJD, sv1.gravref, hSun, true, false, false);
	return t_sunrise_guess + ttoSunrise;
}

VECTOR3 RTCC::PointAOTWithCSM(MATRIX3 REFSMMAT, SV sv, int AOTdetent, int star, double dockingangle)
{
	MATRIX3 Rot, C_LSM, R_ML, C_MSM;
	VECTOR3 s_REF, U_D, U_OAN, U_A, U_DL, U_AL, U_R, GA;
	double EL, AZ, A;

	s_REF = navstars[star - 1];
	U_D = mul(REFSMMAT, s_REF);

	EL = 45.0*RAD;
	AZ = 60.0*RAD*((double)AOTdetent) - 120.0*RAD;

	U_OAN = _V(sin(EL), cos(EL)*sin(AZ), cos(EL)*cos(AZ));
	Rot = OrbMech::CSMBodyToLMBody(dockingangle);
	U_A = tmul(Rot, U_OAN);

	C_LSM = OrbMech::tmat(OrbMech::CALCSMSC(_V(0, 0, 0)));
	U_DL = tmul(C_LSM, U_D);
	U_AL = U_A;
	U_R = -unit(crossp(U_DL, U_AL));
	A = acos(dotp(U_AL, U_DL));
	R_ML = OrbMech::ROTCOMP(U_R, A);
	C_MSM = mul(C_LSM, R_ML);
	GA = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), OrbMech::tmat(C_MSM));

	return GA;
}

bool RTCC::NC1NC2Program(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh_F, double n_H1, int s, double dh, double n_C, VECTOR3 &dV_NC1_LVLH, double &dh_NC2, double &dv_NC2, double &t_NC2, VECTOR3 &dV_NC2_LVLH, double &dv_NCC, double &t_NCC, double &t_NSR, VECTOR3 &dV_NSR, bool NPC)
{
	int s_F;
	double t_N, dv_C, dv_H1, dv_H2, c, theta, mu, eps1, pi, t, t_H1, t_H2, e_H1, eps2, c_I, t_S, e_H2, e_P, c_F, dvo_C, eo_P;
	double d, MJD, p_H1, p_H2, p_C, e_H1o, e_H2o, dv_H1o, dv_H2o, tt, t_WC;
	VECTOR3 R_WS, V_WS, R_CC, V_CC, R_WC, V_WC, am, ur_WD, R_WF, V_WF, R_WJ, V_WJ, R_CFD, R_CH1, V_CH1, R_CH2, V_CH2, V_CCF, V_CH1F;
	VECTOR3 R_WH2, V_WH2, V_CH2F, R_CS, V_CS, R_CF, V_CF, R_CSF, V_CSF,R_WAF, V_WAF, X, Y, Z, dV_LVLH, dV_NSR_LVLH;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);
	eps1 = 0.000001;
	eps2 = 0.01;
	p_C = 1;
	MJD = GETbase + t_C / 24.0 / 3600.0;

	s_F = 0;
	p_H1 = p_H2 = c_F = c_I = 0.0;
	t_N = t_C;
	R_WS = sv_W.R;
	V_WS = sv_W.V;
	pi = PI;
	dv_C = 225.0*0.3048;
	dv_H1 = 165.0*0.3048;
	dv_H2 = 25.0*0.3048;

	if (s == 2)
	{
		n_C = n_H1;
		dv_C = dv_H1;
	}

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CC, V_CC, gravref, gravref);
	OrbMech::rv_from_r0v0(sv_W.R, sv_W.V, (GETbase - sv_W.MJD)*24.0*3600.0 + t_C, R_WC, V_WC, mu);
	t_WC = t_C;
	do
	{
		am = unit(crossp(R_WC, V_WC));
		ur_WD = unit(crossp(crossp(crossp(R_CC, V_CC), R_CC), am));
		d = OrbMech::sign(n_C - 18.0);
		c = OrbMech::sign(dotp(crossp(ur_WD, R_WC), am));
		theta = pi - c*(pi - acos(dotp(ur_WD, unit(R_WC)))) + (d + 1.0)*(c + 1.0)*pi*0.5;

		if (abs(theta) >= eps1)
		{
			if (c_I == 10)
			{
				return false;
			}
			t = OrbMech::time_theta(R_WC, V_WC, theta, mu);
			if (pi != 0.0)
			{
				pi = 0.0;
				R_WC = R_WS;
				V_WC = V_WS;
				t_WC = (sv_W.MJD - GETbase)*24.0*3600.0;
			}
			c_I++;
			t_N -= t;
			OrbMech::oneclickcoast(R_WC, V_WC, MJD, t_N - t_WC, R_WC, V_WC, gravref, gravref);
			t_WC = t_N;
		}
	} while (abs(theta) >= eps1);

	R_CC = unit(R_CC - am*dotp(R_CC, am))*length(R_CC);
	V_CC = unit(V_CC - am*dotp(V_CC, am))*length(V_CC);

	OrbMech::rv_from_r0v0(R_WC, V_WC, t_F - t_N, R_WF, V_WF, mu);

	if (!OrbMech::QDRTPI(R_WF, V_WF, MJD, gravref, mu, dh_F, E_L, 0, R_WJ, V_WJ))
	{
		return false;
	}

	R_CFD = R_WJ - unit(R_WJ)*dh_F;

	do
	{
		V_CCF = V_CC + unit(crossp(am, R_CC))*dv_C;
		OrbMech::REVUP(R_CC, V_CCF, n_C, mu, R_CH1, V_CH1, tt);
		t_H1 = t_C + tt;

		if (s == 1)
		{
			if (c_F == 0)
			{
				c_I = 0;
			}
			else
			{
				c_I = 0.5;
			}
			do
			{
				V_CH1F = V_CH1 + unit(crossp(am, R_CH1))*dv_H1;
				OrbMech::REVUP(R_CH1, V_CH1F, n_H1, mu, R_CH2, V_CH2, tt);
				t_H2 = t_H1 + tt;
				OrbMech::RADUP(R_WC, V_WC, R_CH2, mu, R_WH2, V_WH2);
				e_H1 = length(R_WH2) - length(R_CH2) - dh;
				if (p_H1 == 0 || abs(e_H1) >= eps2)
				{
					OrbMech::ITER(c_I, s_F, e_H1, p_H1, dv_H1, e_H1o, dv_H1o);
					if (s_F == 1)
					{
						return false;
					}
				}
			} while (abs(e_H1) >= eps2);
		}
		else
		{
			t_H2 = t_H1;
			R_CH2 = R_CH1;
			V_CH2 = V_CH1;
		}
		if (c_F == 0)
		{
			c_I = 0;
		}
		else
		{
			c_I = 0.5;
		}
		t_S = t_H2 + dt;
		do
		{
			V_CH2F = V_CH2 + unit(crossp(am, R_CH2))*dv_H2;
			OrbMech::rv_from_r0v0(R_CH2, V_CH2F, dt, R_CS, V_CS, mu);
			OrbMech::RADUP(R_WC, V_WC, R_CS, mu, R_WS, V_WS);
			OrbMech::COE(R_WS, V_WS, length(R_WS) - length(R_CS), mu, R_CSF, V_CSF);
			OrbMech::rv_from_r0v0(R_CS, V_CSF, t_F - t_S, R_CF, V_CF, mu);
			OrbMech::RADUP(R_WC, V_WC, R_CF, mu, R_WAF, V_WAF);
			e_H2 = length(R_WAF) - length(R_CF) - dh_F;

			if (p_H2 == 0 || abs(e_H2) >= eps2)
			{
				OrbMech::ITER(c_I, s_F, e_H2, p_H2, dv_H2, e_H2o, dv_H2o);
				if (s_F == 1)
				{
					return false;
				}
			}
		} while (abs(e_H2) >= eps2);
		e_P = OrbMech::sign(dotp(crossp(R_CF, R_CFD), am))*acos(dotp(R_CFD / length(R_CFD), R_CF / length(R_CF)));
		if (abs(e_P) >= 4.0*eps1)
		{
			OrbMech::ITER(c_F, s_F, e_P, p_C, dv_C, eo_P, dvo_C);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(e_P) >= 4.0*eps1);

	if (NPC)
	{
		double dt_PC, dv_PC, Y_C;
		VECTOR3 u2, R_WCC, V_WCC, R_CCC, V_CCC, dV_NC, R_WPC, V_WPC, R_CPC, V_CPC, XX, YY, ZZ, V_CCC_apo;
		double c_PC, p_PC, eo_PC, dvo_PC;
		int s_PC;

		c_PC = 0.0;
		p_PC = 0.0;

		OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CCC, V_CCC, gravref, gravref);
		OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_C, R_WCC, V_WCC, gravref, gravref);

		u2 = unit(crossp(V_WCC, R_WCC));
		dv_PC = -dotp(V_CCC, u2);

		ZZ = -unit(R_CCC);
		YY = unit(crossp(V_CCC, R_CCC));
		XX = crossp(YY, ZZ);

		do
		{
			dV_LVLH = _V(dv_C, dv_PC, 0);
			dV_NC = tmul(_M(XX.x, XX.y, XX.z, YY.x, YY.y, YY.z, ZZ.x, ZZ.y, ZZ.z), dV_LVLH);
			V_CCC_apo = V_CCC + dV_NC;

			dt_PC = OrbMech::time_theta(R_CCC, V_CCC_apo, PI05, mu);

			OrbMech::oneclickcoast(R_CCC, V_CCC_apo, MJD, dt_PC, R_CPC, V_CPC, gravref, gravref);
			OrbMech::oneclickcoast(R_WCC, V_WCC, MJD, dt_PC, R_WPC, V_WPC, gravref, gravref);
			u2 = unit(crossp(V_WPC, R_WPC));

			Y_C = dotp(R_CPC, u2);
			if (p_PC == 0 || abs(Y_C) >= eps2)
			{
				OrbMech::ITER(c_PC, s_PC, Y_C, p_PC, dv_PC, eo_PC, dvo_PC);
				if (s_PC == 1)
				{
					return false;
				}
			}
		} while (abs(Y_C) >= eps2);
	}
	else
	{
		dV_LVLH = _V(dv_C, 0, 0);
	}
	if (s == 1)
	{
		OrbMech::RADUP(R_WC, V_WC, R_CH1, mu, R_WF, V_WF);
		dh_NC2 = length(R_WF) - length(R_CH1);
		dv_NC2 = dv_H1;
		dV_NC1_LVLH = dV_LVLH;
		t_NC2 = t_H1;
	}
	else
	{
		dV_NC2_LVLH = dV_LVLH;
	}
	dv_NCC = dv_H2;
	dV_NSR = V_CSF - V_CS;
	t_NCC = t_H2;
	t_NSR = t_S;
	Z = -unit(R_CS);
	Y = -am;
	X = crossp(Y, Z);
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NSR);

	return true;
}

void RTCC::NCCProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh, VECTOR3 &dV_NCC_LVLH, double &t_NSR, VECTOR3 &dV_NSR_LVLH)
{
	VECTOR3 R_CC, V_CC, R_WF, V_WF, R_WJ, V_WJ, R_CF, V_CF, R_CT, V_CT, V_CCF, R_CTF, V_CTF, X, Y, Z, dV_NCC, dV_NSR;
	double MJD, mu;
	OBJHANDLE gravref;
	int body;

	gravref = sv_C.gravref;
	if (gravref == hEarth)
	{
		body = BODY_EARTH;
	}
	else
	{
		body = BODY_MOON;
	}
	mu = GGRAV*oapiGetMass(gravref);
	MJD = GETbase + t_C / 24.0 / 3600.0;

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t_C, R_CC, V_CC, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_F, R_WF, V_WF, gravref, gravref);
	OrbMech::QDRTPI(R_WF, V_WF, MJD, gravref, mu, dh, E_L, 1, R_WJ, V_WJ);
	OrbMech::COE(R_WJ, V_WJ, dh, mu, R_CF, V_CF);
	OrbMech::oneclickcoast(R_CF, V_CF, MJD, t_C + dt - t_F, R_CT, V_CT, gravref, gravref);
	V_CCF = OrbMech::Vinti(R_CC, V_CC, R_CT, MJD, dt, 0, true, body, body, body, _V(0, 0, 0));
	OrbMech::oneclickcoast(R_CC, V_CCF, sv_C.MJD, dt, R_CTF, V_CTF, gravref, gravref);

	dV_NCC = V_CCF - V_CC;
	dV_NSR = V_CT - V_CTF;

	Z = -unit(R_CTF);
	Y = unit(crossp(V_CTF, R_CTF));
	X = crossp(Y, Z);
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NSR);

	Z = -unit(R_CC);
	Y = unit(crossp(V_CC, R_CC));
	X = crossp(Y, Z);
	dV_NCC_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_NCC);
}

void RTCC::NSRProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t2, double t3, VECTOR3 &dV_NSR_LVLH)
{
	VECTOR3 R_C2, V_C2, R_W2, V_W2, u1, u2, u, R_WC, V_WC, dV_2, X, Y, Z, V_C2_apo;
	double Y_C, Y_C_dot, Y_W_dot, cos_phi, sin_phi, SW, phi, dh_NSR, v_WV, a_W, a_C, v_CV, v_CH, mu, dt;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t2, R_C2, V_C2, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t2, R_W2, V_W2, gravref, gravref);

	u1 = unit(crossp(V_C2, R_C2));
	u2 = unit(crossp(V_W2, R_W2));
	Y_C = dotp(R_C2, u2);
	Y_C_dot = dotp(V_C2, u2);
	Y_W_dot = dotp(V_W2, u1);
	
	u = unit(crossp(R_W2, V_W2));
	R_C2 = unit(R_C2 - u*dotp(R_C2, u))*length(R_C2);
	V_C2 = unit(V_C2 - u*dotp(V_C2, u))*length(V_C2);

	cos_phi = dotp(unit(R_C2), unit(R_W2));
	sin_phi = sqrt(1.0 - cos_phi*cos_phi);
	SW = dotp(u, crossp(R_W2, R_C2));

	phi = atan2(OrbMech::sign(SW)*sin_phi, cos_phi);
	dt = OrbMech::time_theta(R_W2, V_W2, phi, mu);
	OrbMech::rv_from_r0v0(R_W2, V_W2, dt, R_WC, V_WC, mu);

	dh_NSR = length(R_WC) - length(R_C2);
	v_WV = dotp(V_WC, unit(R_C2));
	a_W =  1.0 / (2.0 / length(R_W2) - dotp(V_W2, V_W2) / mu);
	a_C = a_W - dh_NSR;
	v_CV = v_WV*OrbMech::power(a_W / a_C, 1.5);
	v_CH = sqrt(mu*(2.0 / length(R_C2) - 1.0 / a_C) - v_CV*v_CV);
	V_C2_apo = unit(crossp(u, R_C2))*v_CH + unit(R_C2)*v_CV;
	dV_2 = V_C2_apo - V_C2;
	Z = unit(-R_C2);
	Y = -u;
	X = unit(crossp(Y, Z));
	
	dV_NSR_LVLH = mul(_M(X.x, X.y, X.z, Y.x, Y.y, Y.z, Z.x, Z.y, Z.z), dV_2) + _V(0.0, -Y_C_dot, 0.0);
}

void RTCC::NPCProgram(SV sv_C, SV sv_W, double GETbase, double t, double &t_NPC, VECTOR3 &dV_NPC_LVLH)
{
	VECTOR3 R_C2, V_C2, R_C3, V_C3, R_W3, V_W3, u1, u2;
	double mu, dt, Y_C, Y_C_dot, Y_W_dot, T_P;
	OBJHANDLE gravref;

	gravref = sv_C.gravref;
	mu = GGRAV*oapiGetMass(gravref);

	OrbMech::oneclickcoast(sv_C.R, sv_C.V, sv_C.MJD, (GETbase - sv_C.MJD)*24.0*3600.0 + t, R_C2, V_C2, gravref, gravref);
	dt = OrbMech::time_theta(R_C2, V_C2, PI05*3.0, mu);
	T_P = OrbMech::period(R_C2, V_C2, mu);

	if (dt < 0)
	{
		dt = T_P + dt;
	}

	t_NPC = t + dt;

	OrbMech::oneclickcoast(R_C2, V_C2, sv_C.MJD, dt, R_C3, V_C3, gravref, gravref);
	OrbMech::oneclickcoast(sv_W.R, sv_W.V, sv_W.MJD, (GETbase - sv_W.MJD)*24.0*3600.0 + t_NPC, R_W3, V_W3, gravref, gravref);

	u1 = unit(crossp(V_C3, R_C3));
	u2 = unit(crossp(V_W3, R_W3));
	Y_C = dotp(R_C3, u2);
	Y_C_dot = dotp(V_C3, u2);
	Y_W_dot = dotp(V_W3, u1);

	dV_NPC_LVLH = _V(0, -Y_C_dot, abs(Y_C_dot) / tan(70.0*RAD));
}

bool RTCC::SkylabRendezvous(SkyRendOpt *opt, SkylabRendezvousResults *res)
{
	SV sv_C, sv_W, sv_tig;
	VECTOR3 dV_NC1, dV_LVLH, dV_NC2, dV_NCC, dV_NSR, dV_NPC;
	double LMmass, P30TIG, t_NPC;
	OBJHANDLE gravref;
	bool SolutionGood = true;	//True = Calculation successful, False = Calculation failed

	if (opt->useSV)
	{
		sv_C = opt->RV_MCC;
	}
	else
	{
		sv_C = StateVectorCalc(opt->vessel);
	}

	sv_W = StateVectorCalc(opt->target);
	gravref = sv_C.gravref;

	if (opt->csmlmdocked)
	{
		LMmass = GetDockedVesselMass(opt->vessel);
	}
	else
	{
		LMmass = 0;
	}

	if (opt->man == 0)
	{
		SV sv_W1;
		double ttoMidnight;
		OBJHANDLE hSun;

		hSun = oapiGetObjectByName("Sun");

		sv_W1 = coast(sv_W, (opt->GETbase - sv_W.MJD)*24.0*3600.0 + opt->TPIGuess);

		ttoMidnight = OrbMech::sunrise(sv_W1.R, sv_W1.V, sv_W1.MJD, gravref, hSun, 1, 1, false);
		res->t_TPI = opt->TPIGuess + ttoMidnight;

		return true;
	}
	else if (opt->man == 1)	//NC1
	{
		SolutionGood = NC1NC2Program(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, 0.5, 1, opt->DH1, opt->n_C, dV_NC1, res->dH_NC2, res->dv_NC2, res->t_NC2, dV_NC2, res->dv_NCC, res->t_NCC, res->t_NSR, res->dV_NSR, opt->NPCOption);
		
		if (SolutionGood)
		{
			dV_LVLH = dV_NC1;
			P30TIG = opt->t_C;
		}
	}
	else if (opt->man == 2)	//NC2
	{
		SolutionGood = NC1NC2Program(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, 0.5, 2, opt->DH1, opt->n_C, dV_NC1, res->dH_NC2, res->dv_NC2, res->t_NC2, dV_NC2, res->dv_NCC, res->t_NCC, res->t_NSR, res->dV_NSR, opt->NPCOption);
		if (SolutionGood)
		{
			dV_LVLH = dV_NC2;
			P30TIG = opt->t_C;
		}
	}
	else if (opt->man == 3)	//NCC
	{
		NCCProgram(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, 37.0*60.0, opt->t_TPI, opt->DH2, dV_NCC, res->t_NSR, res->dV_NSR);
		
		dV_LVLH = dV_NCC;
		P30TIG = opt->t_C;
	}
	else if (opt->man == 4)	//NSR
	{
		NSRProgram(sv_C, sv_W, opt->GETbase, opt->E_L, opt->t_C, opt->t_TPI, dV_NSR);

		dV_LVLH = dV_NSR;
		P30TIG = opt->t_C;
	}
	else if (opt->man == 5)	//TPI
	{
		LambertMan lambert;
		TwoImpulseResuls lamres;
		double dt;

		dt = OrbMech::time_theta(sv_W.R, sv_W.V, 140.0*RAD, GGRAV*oapiGetMass(gravref));

		lambert.mode = 0;
		lambert.GETbase = opt->GETbase;
		lambert.T1 = opt->t_C;
		lambert.T2 = opt->t_C + dt;
		lambert.N = 0;
		lambert.axis = RTCC_LAMBERT_MULTIAXIS;
		lambert.Perturbation = RTCC_LAMBERT_PERTURBED;
		lambert.Offset = _V(0, 0, 0);
		lambert.sv_A = sv_C;
		lambert.sv_P = sv_W;

		LambertTargeting(&lambert, lamres);

		res->dV_LVLH = dV_LVLH = lamres.dV_LVLH;
		res->P30TIG = opt->t_C;

		return true;
	}
	else if (opt->man == 6)	//TPM
	{
		LambertMan lambert;
		TwoImpulseResuls lamres;
		double dt;

		dt = OrbMech::time_theta(sv_W.R, sv_W.V, 140.0*RAD, GGRAV*oapiGetMass(gravref));

		lambert.mode = 0;
		lambert.GETbase = opt->GETbase;
		lambert.T1 = opt->t_C;
		lambert.T2 = opt->t_TPI + dt;
		lambert.N = 0;
		lambert.axis = RTCC_LAMBERT_MULTIAXIS;
		lambert.Perturbation = RTCC_LAMBERT_PERTURBED;
		lambert.Offset = _V(0, 0, 0);
		lambert.sv_A = sv_C;
		lambert.sv_P = sv_W;

		LambertTargeting(&lambert, lamres);

		res->dV_LVLH = dV_LVLH = lamres.dV_LVLH;
		res->P30TIG = opt->t_C;

		return true;
	}
	else if (opt->man == 7)	//NPC
	{
		NPCProgram(sv_C, sv_W, opt->GETbase, opt->t_NC, t_NPC, dV_NPC);

		dV_LVLH = dV_NPC;
		P30TIG = t_NPC;
	}

	if (SolutionGood == false)
	{
		return false;
	}

	sv_tig = coast(sv_C, (opt->GETbase - sv_C.MJD)*24.0*3600.0 + P30TIG);

	PoweredFlightProcessor(sv_tig, opt->GETbase, 0.0, RTCC_ENGINETYPE_CSMSPS, LMmass, dV_LVLH, true, res->P30TIG, res->dV_LVLH);

	return true;
}

VECTOR3 RTCC::TLMCEmpiricalFirstGuess(double r, double dt)
{
	double lambda, psi, V, R_E;

	R_E = 6378.137e3;

	lambda = PI - 0.025*dt*3600.0;
	psi = 270.0*RAD;
	V = sqrt(0.184 + 0.553 / (r / R_E))*R_E / 3600.0 - 0.0022*dt*3600.0;

	return _V(V, psi, lambda);
}

bool RTCC::TLIFlyby(SV sv_TLI, double lat_EMP, double h_peri, SV sv_peri_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_TLI_apo, sv_p, sv_r;
	MATRIX3 M_EMP;
	VECTOR3 R_EMP, V_EMP;
	double dt, ddt, VacPeri, R_E, e_H, e_Ho, c_I, p_H, eps2, dto, r_peri, lngtest, lattest, fpatest, rtest, v_peri, lng_EMP, azi_peri;
	int s_F;
	OBJHANDLE hMoon, hEarth;
	OELEMENTS coe;

	c_I = p_H = dto = 0.0;
	eps2 = 0.1;
	s_F = 0;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");
	R_E = OrbMech::R_Earth;

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	r_peri = OrbMech::R_Moon + h_peri;
	dt = (sv_peri_guess.MJD - sv_TLI.MJD)*24.0*3600.0;

	M_EMP = OrbMech::EMPMatrix(sv_peri_guess.MJD);
	R_EMP = mul(M_EMP, sv_peri_guess.R);
	V_EMP = mul(M_EMP, sv_peri_guess.V);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lng_EMP, lattest, fpatest, azi_peri);

	do
	{
		sv_p.MJD = sv_TLI.MJD + dt / 24.0 / 3600.0;

		do
		{
			//Position vector in EMP Coordinates
			OrbMech::adbar_from_rv(r_peri, v_peri, lng_EMP, lat_EMP, PI05, azi_peri, R_EMP, V_EMP);

			//EMP Matrix
			M_EMP = OrbMech::EMPMatrix(sv_p.MJD);

			//Convert EMP position to ecliptic
			sv_p.R = tmul(M_EMP, R_EMP);
			sv_p.V = tmul(M_EMP, V_EMP);

			//Calculate pericynthion velocity
			sv_p.V = OrbMech::Vinti(sv_p.R, _V(0.0, 0.0, 0.0), sv_TLI.R, sv_p.MJD, -dt, 0, false, BODY_MOON, BODY_MOON, BODY_EARTH, sv_p.V);

			//save azi and vmag as new initial guess
			V_EMP = mul(M_EMP, sv_p.V);
			OrbMech::rv_from_adbar(R_EMP, V_EMP, rtest, v_peri, lngtest, lattest, fpatest, azi_peri);

			coe = OrbMech::coe_from_sv(sv_p.R, sv_p.V, OrbMech::mu_Moon);
			ddt = OrbMech::timetoperi(sv_p.R, sv_p.V, OrbMech::mu_Moon);

			if (coe.TA > PI)
			{
				lng_EMP += coe.TA - PI2;
			}
			else
			{
				lng_EMP += coe.TA;
			}
		} while (abs(ddt) > 0.01);

		OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);
		VacPeri = length(sv_r.R);

		//20NM vacuum perigee
		e_H = VacPeri - R_E - 20.0*1852.0;

		if (p_H == 0 || abs(dt - dto) >= eps2)
		{
			OrbMech::ITER(c_I, s_F, e_H, p_H, dt, e_Ho, dto);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(dt - dto) >= eps2);

	sv_TLI_apo = coast(sv_p, -dt);

	DV = sv_TLI_apo.V - sv_TLI.V;
	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCFlyby(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_p, sv_r;
	OBJHANDLE hMoon, hEarth;
	VECTOR3 step, target;
	double dt1;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	target = _V(h_peri, lat_EMP, 20.0*1852.0);
	step = _V(10e-3, 10e-3, 10e-3);

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	DV = OrbMech::TLMCIntegratedFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;
	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCConic_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor)
{
	SV sv_p, sv_n, sv_r;
	LOIMan loiopt;
	VECTOR3 target, DV_LOI, DV_MCC, step;
	double TIG_LOI, dV_T, lat_iter;
	double latarr[3], dvarr[3];
	int s_F, c_I;
	OBJHANDLE hEarth, hMoon;
	double dt1, dt2;
	VECTOR3 R_patch, V_patch;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	loiopt.R_LLS = opt->R_LLS;
	loiopt.azi = opt->azi;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.impulsive = 1;
	loiopt.type = opt->type;

	sv_p.gravref = hMoon;
	sv_p.mass = sv_mcc.mass;

	DV_MCC = DV_guess;
	lat_iter = lat_EMP;

	step = _V(10e-3, 10e-3, 10e-3);
	dvarr[0] = dvarr[1] = dvarr[2] = 0.0;
	c_I = s_F = 0;

	do
	{
		target = _V(h_peri, lat_iter, 20.0*1852.0);
		DV_MCC = OrbMech::TLMCConicFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_MCC, target, step);

		if (sv_mcc.gravref == hEarth)
		{
			dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_MCC, sv_mcc.MJD, OrbMech::mu_Earth, OrbMech::mu_Moon, R_patch, V_patch);
		}
		else
		{
			R_patch = sv_mcc.R;
			V_patch = sv_mcc.V + DV_MCC;
			dt1 = 0.0;
		}

		dt2 = OrbMech::timetoperi(R_patch, V_patch, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, OrbMech::mu_Moon);
		sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

		loiopt.RV_MCC = sv_p;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n);

		dV_T = length(DV_LOI) + length(DV_MCC);
		OrbMech::QuadraticIterator(c_I, s_F, lat_iter, latarr, dvarr, dV_T, 0.02*RAD, 0.2*RAD);
		if (s_F == 1)
		{
			return false;
		}

	} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMC_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor)
{
	SV sv_p, sv_r, sv_n;
	LOIMan loiopt;
	VECTOR3 target, DV_LOI, DV_MCC, step;
	double TIG_LOI, dV_T, lat_iter;
	double latarr[3], dvarr[3];
	int s_F, c_I;

	loiopt.R_LLS = opt->R_LLS;
	loiopt.azi = opt->azi;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.impulsive = 1;
	loiopt.type = opt->type;

	loiopt.RV_MCC.gravref = sv_mcc.gravref;
	loiopt.RV_MCC.R = sv_mcc.R;
	loiopt.RV_MCC.mass = sv_mcc.mass;
	loiopt.RV_MCC.MJD = sv_mcc.MJD;

	DV_MCC = DV_guess;
	lat_iter = lat_EMP;

	step = _V(10e-3, 10e-3, 10e-3);
	dvarr[0] = dvarr[1] = dvarr[2] = 0.0;
	c_I = s_F = 0;

	do
	{
		target = _V(h_peri, lat_iter, 20.0*1852.0);
		DV_MCC = OrbMech::TLMCIntegratedFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_MCC, target, step);

		loiopt.RV_MCC.V = sv_mcc.V + DV_MCC;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n);

		dV_T = length(DV_LOI) + length(DV_MCC);
		OrbMech::QuadraticIterator(c_I, s_F, lat_iter, latarr, dvarr, dV_T, 0.02*RAD, 0.2*RAD);
		if (s_F == 1)
		{
			return false;
		}

	} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);

	double dt1;

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV_MCC, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;
	sv_p.gravref = hMoon;

	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);
	sv_p.gravref = hEarth;

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_reentry = sv_r;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMCConic_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor)
{
	SV sv_p, sv_mcc_apo, sv_n, sv_preLOI, sv_postLOI;
	VECTOR3 R_EMP, V_EMP, DV_LOI, var_converged, RP_M, VP_M, var_guess, DV_MCC, step, DV_DOI;
	double lng_peri_guess, dt, dt1, TIG_LOI, dV_T, r_peri, v_peri_guess, azi_peri_guess, v_peri_conv, azi_peri_conv, lng_peri_conv, TIG_DOI;
	double rtemp, lattemp, fpatemp, lat_iter;
	double dLat, dV_To;
	int c_I;
	LOIMan loiopt;
	DOIMan doiopt;

	loiopt.R_LLS = opt->R_LLS;
	loiopt.azi = opt->azi;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.impulsive = 1;
	loiopt.type = opt->type;
	loiopt.EllipseRotation = opt->LOIEllipseRotation;

	doiopt.R_LLS = opt->R_LLS;
	doiopt.GETbase = opt->GETbase;
	doiopt.lat = opt->LSlat;
	doiopt.lng = opt->LSlng;
	doiopt.N = opt->N;
	doiopt.opt = opt->DOIType;
	doiopt.PeriAng = opt->DOIPeriAng;
	doiopt.PeriAlt = opt->DOIPeriAlt;

	c_I = 0;
	dLat = 0.0;

	sv_p.mass = sv_mcc.mass;
	sv_p.gravref = hMoon;

	sv_mcc_apo = sv_mcc;
	sv_mcc_apo.V = sv_mcc.V + DV_guess;

	r_peri = OrbMech::R_Moon + h_peri;
	lat_iter = lat_EMP;
	step = _V(10e-3, 10e-3, 10e-3);

	dt = (MJD_peri - sv_mcc.MJD)*24.0*3600.0;

	dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_guess, sv_mcc.MJD, OrbMech::mu_Earth, OrbMech::mu_Moon, RP_M, VP_M);
	OrbMech::rv_from_r0v0(RP_M, VP_M, dt - dt1, sv_p.R, sv_p.V, OrbMech::mu_Moon);
	sv_p.MJD = sv_mcc.MJD + dt / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtemp, v_peri_guess, lng_peri_guess, lattemp, fpatemp, azi_peri_guess);

	var_guess = _V(v_peri_guess, azi_peri_guess, lng_peri_guess);

	do
	{
		var_converged = OrbMech::TLMCConicFirstGuessIterator(r_peri, lat_iter, 0.0, var_guess, sv_mcc.R, step, MJD_peri, -dt, hMoon, sv_mcc.gravref);

		v_peri_conv = var_converged.x;
		azi_peri_conv = var_converged.y;
		lng_peri_conv = var_converged.z;

		var_guess = var_converged;

		//Position and velocity vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat_iter, PI05, azi_peri_conv, R_EMP, V_EMP);

		//Convert EMP position to ecliptic
		OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);

		OrbMech::rv_from_r0v0_tb(sv_p.R, sv_p.V, sv_p.MJD, hMoon, sv_mcc.gravref, -dt, sv_mcc_apo.R, sv_mcc_apo.V);
		DV_MCC = sv_mcc_apo.V - sv_mcc.V;

		loiopt.RV_MCC = sv_p;
		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n, sv_preLOI, sv_postLOI);

		if (doiopt.opt == 1)
		{
			doiopt.EarliestGET = TIG_LOI + 3.5*3600.0;
			doiopt.sv0 = sv_postLOI;
			DOITargeting(&doiopt, DV_DOI, TIG_DOI);
		}
		else
		{
			DV_DOI = _V(0, 0, 0);
		}

		dV_T = length(DV_DOI) + length(DV_LOI) + length(DV_MCC);

		if (c_I == 0)
		{
			dLat = -1.0*RAD;
		}
		else
		{
			if (dV_T > dV_To)
			{
				dLat = -dLat / 2.0;
			}
		}

		lat_iter += dLat;
		dV_To = dV_T;
		c_I++;

		//dvar = OrbMech::QuadraticIterator(c_I, s_F, lat_EMPcor, latarr, dvarr, dV_T, 0.1*RAD, 1.0*RAD);
		//if (s_F == 1)
		//{
		//	return false;
		//}
		//} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);
	} while (abs(dLat) > 0.01*RAD);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

bool RTCC::TLMC_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor)
{
	SV sv_p, sv_mcc_apo, sv_n;
	VECTOR3 R_EMP, V_EMP, DV_LOI, var_converged, var_guess, DV_MCC, step;
	double lng_peri_guess, dt, TIG_LOI, dV_T, r_peri, v_peri_guess, azi_peri_guess, v_peri_conv, azi_peri_conv, lng_peri_conv;
	double rtemp, lattemp, fpatemp, lat_iter;
	double dLat, dV_To;
	int c_I;
	LOIMan loiopt;

	loiopt.R_LLS = opt->R_LLS;
	loiopt.azi = opt->azi;
	loiopt.GETbase = opt->GETbase;
	loiopt.h_apo = opt->LOIh_apo;
	loiopt.h_peri = opt->LOIh_peri;
	loiopt.lat = opt->LSlat;
	loiopt.lng = opt->LSlng;
	loiopt.t_land = opt->t_land;
	loiopt.impulsive = 1;
	loiopt.type = opt->type;

	c_I = 0;
	dLat = 0.0;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	loiopt.RV_MCC.mass = sv_mcc.mass;
	loiopt.RV_MCC.gravref = hMoon;

	sv_mcc_apo = sv_mcc;
	sv_mcc_apo.V = sv_mcc.V + DV_guess;

	r_peri = OrbMech::R_Moon + h_peri;
	lat_iter = lat_EMP;
	step = _V(10e-3, 10e-3, 10e-3);

	dt = (MJD_peri - sv_mcc.MJD)*24.0*3600.0;

	sv_p = coast(sv_mcc_apo, dt);

	//dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV_guess, sv_mcc.MJD, mu_E, mu_M, RP_M, VP_M);
	//OrbMech::rv_from_r0v0(RP_M, VP_M, dt - dt1, sv_p.R, sv_p.V, mu_M);
	//sv_p.MJD = sv_mcc.MJD + dt / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::rv_from_adbar(R_EMP, V_EMP, rtemp, v_peri_guess, lng_peri_guess, lattemp, fpatemp, azi_peri_guess);

	var_guess = _V(v_peri_guess, azi_peri_guess, lng_peri_guess);

	do
	{
		var_converged = OrbMech::IntegratedTLMCIterator(r_peri, lat_iter, PI05, var_guess, sv_mcc.R, step, MJD_peri, -dt, hMoon, sv_mcc.gravref);

		v_peri_conv = var_converged.x;
		azi_peri_conv = var_converged.y;
		lng_peri_conv = var_converged.z;

		var_guess = var_converged;

		//Position and velocity vector in EMP Coordinates
		OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat_iter, PI05, azi_peri_conv, R_EMP, V_EMP);

		//Convert EMP position to ecliptic
		OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);

		sv_mcc_apo = coast(sv_p, -dt);
		DV_MCC = sv_mcc_apo.V - sv_mcc.V;

		loiopt.RV_MCC = sv_p;

		LOITargeting(&loiopt, DV_LOI, TIG_LOI, sv_n);

		dV_T = length(DV_LOI) + length(DV_MCC);

		if (c_I == 0)
		{
			dLat = -1.0*RAD;
		}
		else
		{
			if (dV_T > dV_To)
			{
				dLat = -dLat / 2.0;
			}
		}

		lat_EMPcor += dLat;
		dV_To = dV_T;
		c_I++;

		//dvar = OrbMech::QuadraticIterator(c_I, s_F, lat_EMPcor, latarr, dvarr, dV_T, 0.1*RAD, 1.0*RAD);
		//if (s_F == 1)
		//{
		//	return false;
		//}
		//} while (c_I < 3 || abs(dvarr[2] - dvarr[1]) > 0.01);
	} while (abs(dLat) > 0.01*RAD);

	DV = DV_MCC;
	lat_EMPcor = lat_iter;
	sv_peri = sv_p;
	sv_node = sv_n;

	return true;
}

void RTCC::TLMCFirstGuess(SV sv_mcc, double lat_EMP, double h_peri, double MJD_P, VECTOR3 &DV, SV &sv_peri)
{
	VECTOR3 var_guess, var_converged;
	double r_peri;

	r_peri = OrbMech::R_Moon + h_peri;
	var_guess = TLMCEmpiricalFirstGuess(r_peri, 0.0);

	IntegratedTLMC(sv_mcc, lat_EMP, h_peri, 0.0, MJD_P, var_guess, DV, var_converged, sv_peri);
}

void RTCC::IntegratedTLMC(SV sv_mcc, double lat, double h, double gamma, double MJD, VECTOR3 var_guess, VECTOR3 &DV, VECTOR3 &var_converged, SV &sv_node)
{
	SV sv_n, sv_mcc_apo;
	OBJHANDLE hMoon;
	VECTOR3 R_EMP, V_EMP, step;
	double r_peri, dt, v_peri_conv, azi_peri_conv, lng_peri_conv;

	hMoon = oapiGetObjectByName("Moon");
	r_peri = OrbMech::R_Moon + h;
	sv_n.MJD = MJD;
	sv_n.gravref = hMoon;
	dt = (sv_n.MJD - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);//_V(pow(2.0, -19.0)*1769.7, pow(2.0, -19.0)*RAD, pow(2.0, -19.0));

	var_converged = OrbMech::IntegratedTLMCIterator(r_peri, lat, gamma, var_guess, sv_mcc.R, step, MJD, -dt, hMoon, sv_mcc.gravref);

	v_peri_conv = var_converged.x;
	azi_peri_conv = var_converged.y;
	lng_peri_conv = var_converged.z;

	OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat, gamma + PI05, azi_peri_conv, R_EMP, V_EMP);
	OrbMech::EMPToEcl(R_EMP, V_EMP, sv_n.MJD, sv_n.R, sv_n.V);
	sv_mcc_apo = coast(sv_n, -dt);

	DV = sv_mcc_apo.V - sv_mcc.V;

	sv_node = sv_n;
}

void RTCC::TLMCIntegratedXYZT(SV sv_mcc, double lat_node, double lng_node, double h_node, double MJD_node, VECTOR3 DV_guess, VECTOR3 &DV)
{
	SV sv_p;
	VECTOR3 step;
	double dt;

	dt = (MJD_node - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCIntegratedXYZTIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, _V(h_node, lat_node, lng_node), step, dt);
}

void RTCC::TLMCFirstGuessConic(SV sv_mcc, double lat, double h, double gamma, double MJD_P, VECTOR3 &DV, VECTOR3 &var_converged)
{
	SV sv_p;
	VECTOR3 R_EMP, V_EMP, R_MCC, V_MCC_apo, var_guess, step;
	double r_peri, lng_peri_conv, azi_peri_conv, v_peri_conv, dt;
	OBJHANDLE hMoon, hEarth;

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	r_peri = OrbMech::R_Moon + h;
	sv_p.MJD = MJD_P;
	dt = (MJD_P - sv_mcc.MJD)*24.0*3600.0;
	step = _V(10e-3, 10e-3, 10e-3);

	//Initial guess
	var_guess = TLMCEmpiricalFirstGuess(r_peri, 0.0);

	var_converged = OrbMech::TLMCConicFirstGuessIterator(r_peri, lat, gamma, var_guess, sv_mcc.R, step, MJD_P, -dt, hMoon, sv_mcc.gravref);

	v_peri_conv = var_converged.x;
	azi_peri_conv = var_converged.y;
	lng_peri_conv = var_converged.z;

	OrbMech::adbar_from_rv(r_peri, v_peri_conv, lng_peri_conv, lat, gamma + PI05, azi_peri_conv, R_EMP, V_EMP);
	OrbMech::EMPToEcl(R_EMP, V_EMP, sv_p.MJD, sv_p.R, sv_p.V);
	OrbMech::rv_from_r0v0_tb(sv_p.R, sv_p.V, sv_p.MJD, hMoon, sv_mcc.gravref, -dt, R_MCC, V_MCC_apo);

	DV = V_MCC_apo - sv_mcc.V;
}

bool RTCC::TLMCFlybyConic(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_patch, V_patch, step;
	double dt1, dt2;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	target = _V(h_peri, lat_EMP, 20.0*1852.0);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCConicFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	if (sv_mcc.gravref == hEarth)
	{
		dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, OrbMech::mu_Earth, OrbMech::mu_Moon, R_patch, V_patch);
	}
	else
	{
		R_patch = sv_mcc.R;
		V_patch = sv_mcc.V + DV;
		dt1 = 0.0;
	}

	dt2 = OrbMech::timetoperi(R_patch, V_patch, OrbMech::mu_Moon);
	OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, OrbMech::mu_Moon);
	sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

	OrbMech::ReturnPerigeeConic(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCConicFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_patch, V_patch, R_EMP, V_EMP, step;
	double dt1, dt2, lng_EMP;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	target = _V(h_peri, 20.0*1852.0, inc_fr_des);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCConicSPSLunarFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	if (sv_mcc.gravref == hEarth)
	{
		dt1 = OrbMech::findpatchpoint(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, OrbMech::mu_Earth, OrbMech::mu_Moon, R_patch, V_patch);
	}
	else
	{
		R_patch = sv_mcc.R;
		V_patch = sv_mcc.V + DV;
		dt1 = 0.0;
	}

	dt2 = OrbMech::timetoperi(R_patch, V_patch, OrbMech::mu_Moon);
	OrbMech::rv_from_r0v0(R_patch, V_patch, dt2, sv_p.R, sv_p.V, OrbMech::mu_Moon);

	sv_p.MJD = sv_mcc.MJD + (dt1 + dt2) / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	OrbMech::ReturnPerigeeConic(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

bool RTCC::TLMCIntegratedFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP)
{
	SV sv_p, sv_r;
	VECTOR3 target, R_EMP, V_EMP, step;
	double dt1, lng_EMP;
	OBJHANDLE hEarth, hMoon;

	hEarth = oapiGetObjectByName("Earth");
	hMoon = oapiGetObjectByName("Moon");

	sv_p.gravref = hMoon;
	sv_r.gravref = hEarth;

	target = _V(h_peri, 20.0*1852.0, inc_fr_des);
	step = _V(10e-3, 10e-3, 10e-3);

	DV = OrbMech::TLMCIntegratedSPSLunarFlybyIterator(sv_mcc.R, sv_mcc.V, sv_mcc.MJD, sv_mcc.gravref, DV_guess, target, step);

	dt1 = OrbMech::timetoperi_integ(sv_mcc.R, sv_mcc.V + DV, sv_mcc.MJD, sv_mcc.gravref, hMoon, sv_p.R, sv_p.V);
	sv_p.MJD = sv_mcc.MJD + dt1 / 24.0 / 3600.0;

	OrbMech::EclToEMP(sv_p.R, sv_p.V, sv_p.MJD, R_EMP, V_EMP);
	OrbMech::latlong_from_r(R_EMP, lat_EMP, lng_EMP);

	OrbMech::ReturnPerigee(sv_p.R, sv_p.V, sv_p.MJD, hMoon, hEarth, 1.0, sv_r.MJD, sv_r.R, sv_r.V);

	sv_peri = sv_p;
	sv_reentry = sv_r;

	return true;
}

double LLWP_CURVE(double DV1, double DV2, double DV3, double DH1, double DH2, double DH3, double DVMAX)
{
	double K1, K2, K3, A, B, C, R1, R2, DH_MAX;

	K1 = DV1 / ((DH1 - DH2)*(DH1 - DH3));
	K2 = DV2 / ((DH2 - DH1)*(DH2 - DH3));
	K3 = DV3 / ((DH3 - DH1)*(DH3 - DH2));
	A = K1 + K2 + K3;
	B = -(K1*(DH3 + DH2) + K2 * (DH3 + DH1) + K3 * (DH2 + DH1));
	C = K1 * DH2*DH3 + K2 * DH1*DH3 + K3 * DH1*DH2 - DVMAX;
	R1 = (-B + sqrt(B*B - 4.0*A*C)) / (2.0*A);
	R2 = (-B - sqrt(B*B - 4.0*A*C)) / (2.0*A);
	if (R1 - R2 > 0)
	{
		DH_MAX = R1;
	}
	else
	{
		DH_MAX = R1;
	}
	return DH_MAX;
}

void RTCC::LLWP_PERHAP(AEGHeader Header, AEGDataBlock sv, double &RAP, double &RPE)
{
	AEGDataBlock sv_temp;
	double t0 = sv.TE;
	double dt = 20.0*60.0;
	double R[3], U[3];

	sv.TIMA = 0;
	R[0] = sv.R;
	U[0] = sv.U;
	sv.TE += 20.0*60.0;
	pmmlaeg.CALL(Header, sv, sv_temp);
	R[1] = sv_temp.R;
	U[1] = sv_temp.U;
	sv.TE += 20.0*60.0;
	pmmlaeg.CALL(Header, sv, sv_temp);
	R[2] = sv_temp.R;
	U[2] = sv_temp.U;
	PCHAPE(R[0], R[1], R[2], U[0], U[1], U[2], RAP, RPE);
}

void RTCC::LLWP_HMALIT(AEGHeader Header, AEGDataBlock *sv, AEGDataBlock *sv_temp, int M, int P, int I_CDH, double DH, double &dv_CSI, double &dv_CDH, double &t_CDH)
{
	VECTOR3 H, J, K, L, R_m, V_m, V_m_apo;
	double Pgamma, u_apo, P_v, dgamma, DV_K_apo, t_CSI, theta_k_apo, mu, R_apo, R_dot_apo, gamma_m, p_p, V_H, PP, V, R_p_dot, G_s, G_c, theta_k, g_apo, R_TK, R_R, DV_K;
	double R_CDH, U_CSI, DH_G, du;
	int K_orp, m, p;

	mu = OrbMech::mu_Moon;
	t_CSI = sv[0].TE;
	m = M - 1;
	p = P - 1;

	Pgamma = 0.0;
	u_apo = sv_temp[m].U;
	P_v = 0.0;
	dgamma = 0.0;
	DV_K_apo = 0.0;
	K_orp = 0;

	if (I_CDH > 0)
	{
		t_CDH = t_CSI + PI2 / sv[m].l_dot / 2.0*(double)(I_CDH);
	}
	else
	{
		if (sv[m].coe_osc.l - PI < 0)
		{

		}
		else
		{

		}
	}
	sv[m].TIMA = 0;
	sv[m].TE = t_CDH;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	theta_k_apo = OrbMech::THETR(sv_temp[m].U, sv_temp[p].U, sv_temp[m].coe_osc.i, sv_temp[p].coe_osc.i, sv_temp[m].coe_osc.h, sv_temp[p].coe_osc.h);
	sv[m].TIMA = 0;
	sv[m].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_m, V_m);
	V_m_apo = V_m;
	K = unit(R_m);
	L = unit(V_m);
	H = unit(crossp(K, L));
	J = unit(crossp(K, H));
	R_apo = length(R_m);
	R_dot_apo = dotp(R_m, V_m) / R_apo;
	gamma_m = atan((R_dot_apo / length(V_m)) / sqrt(1.0 - pow(R_dot_apo / length(V_m), 2)));
	p_p = sv_temp[p].coe_osc.a*(1.0 - sv_temp[p].coe_osc.e*sv_temp[p].coe_osc.e);
	R_p_dot = sqrt(mu / p_p)*(sv_temp[p].coe_osc.e*cos(sv_temp[p].coe_osc.g)*sin(sv_temp[p].U) - sv_temp[p].coe_osc.e*sin(sv_temp[p].coe_osc.g)*cos(sv_temp[p].U));
	V_H = length(V_m)*cos(gamma_m);
	//RTCC Requirements had e_m instead of e_p
	PP = sv_temp[p].coe_osc.a*(1.0 - sv_temp[p].coe_osc.e*sv_temp[p].coe_osc.e);
	V = sqrt(mu*(2.0 / sv_temp[p].R - 1.0 / sv_temp[p].coe_osc.a));
	G_s = R_p_dot / V;
	G_c = sqrt(1.0 - G_s * G_s);
	theta_k = atan2(PP*G_s / G_c, P - sv_temp[p].R);
	g_apo = sv_temp[m].coe_osc.g;
	do
	{
		R_TK = PP / (1.0 + sv_temp[p].coe_osc.e*cos(theta_k_apo + theta_k + dgamma));
		R_R = R_TK - DH;
		DV_K = sqrt(2.0*mu / R_apo *(1.0 / (1.0 + R_apo / R_R))) - V_H;
		V_m = V_m_apo - J * DV_K;
		//Initialize
		sv[m].coe_osc = OrbMech::GIMIKC(R_m, V_m, mu);
		sv[m].ENTRY = 0;
		sv[m].TS = sv[m].TE = t_CSI;
		pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
		if (I_CDH > 0 || abs(DV_K_apo - DV_K) <= 1.0*0.3048)
		{
			break;
		}
		DV_K_apo = DV_K;
	} while (abs(DV_K_apo - DV_K) > 1.0*0.3048);
	R_CDH = R_R;
	dv_CSI = DV_K;
RTCC_LLWP_HMALIT_5_2:
	V_m = V_m_apo - J * dv_CSI;
	//Initialize
	sv[m].coe_osc = OrbMech::GIMIKC(R_m, V_m, mu);
	sv[m].ENTRY = 0;
	sv[m].TIMA = 0;
	sv[m].TS = sv[m].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	U_CSI = sv_temp[m].U;
	if (I_CDH > 0)
	{
		t_CDH = t_CSI + PI2 / sv[m].l_dot / 2.0*(double)(I_CDH);
	}
	else
	{
		sv[m].TIMA = 1;
		if (sv[m].coe_osc.l > PI)
		{
			sv[m].Item8 = 0.0;
			sv[m].Item10 = 1.0;
		}
		else
		{
			sv[m].Item8 = PI;
			sv[m].Item10 = 0.0;
		}
		
		pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
		du = sv_temp[m].U - U_CSI;
		if (K_orp <= 0 && du <= PI/6.0)
		{
			if (sv[m].coe_osc.l > PI)
			{
				sv[m].Item8 = PI;
				sv[m].Item10 = 1.0;
			}
			else
			{
				sv[m].Item8 = 0.0;
				sv[m].Item10 = 1.0;
			}
			K_orp = 1;
			pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
		}
		sv[m].Item10 = 0.0;
		t_CDH = sv_temp[m].TE;
	}
	sv[m].ENTRY = 0;
	sv[m].TIMA = 0;
	sv[m].TE = t_CDH;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	//Bring passive vehicle to phase match
	sv[p].TIMA = 6;
	pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
	DH_G = sv_temp[p].R - sv_temp[m].R;
	R_CDH = R_CDH + DH_G - DH;
	if (abs(DH_G - DH) <= 1.0)
	{
		goto RTCC_LLWP_HMALIT_8_1;
	}
	dv_CSI = sqrt(2.0*mu / (R_apo*(1.0 + R_apo / R_CDH))) - V_H;
	goto RTCC_LLWP_HMALIT_5_2;
RTCC_LLWP_HMALIT_8_1:

	VECTOR3 R_m_b, V_m_b, R_p, V_p;
	OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_m_b, V_m_b);
	OrbMech::GIMKIC(sv_temp[p].coe_osc, mu, R_p, V_p);

	V_m_apo = OrbMech::CoellipticDV(R_m_b, R_p, V_p, mu);
	dv_CDH = length(V_m_apo - V_m_b);

	/*double a_a, V_a, l_dot_a, V_R_a, gamma_a, V_H_a, DV_R, DV_H, Pitch, Yaw, V_H_b;
	a_a = sv_temp[p].coe_osc.a - DH;
	V_a = sqrt(mu*(2.0 / sv_temp[m].R - 1.0 / a_a));
	l_dot_a = sqrt(mu / pow(a_a, 3));
	p_p = sv_temp[p].coe_osc.a*(1.0 - sv_temp[p].coe_osc.e*sv_temp[p].coe_osc.e);
	R_p_dot = sqrt(mu / p_p)*(sv_temp[p].coe_osc.e*cos(sv_temp[p].coe_osc.g)*sin(sv_temp[p].U) - sv_temp[p].coe_osc.e*sin(sv_temp[p].coe_osc.g)*cos(sv_temp[p].U));
	V_R_a = R_p_dot * l_dot_a / sv_temp[p].l_dot;
	gamma_a = asin(V_R_a / V_a);
	gamma_b = atan((V_R_b / length(V_m)) / sqrt(1.0 - pow(R_dot_apo / length(V_m), 2)));
	V_H_b = V_b * cos(gamma_b);
	V_H_a = V_a * cos(gamma_a);
	DV_H = V_H_a - V_H_b;
	DV_R = R_dot_apo - V_R_a;
	dv_CDH = sqrt(DV_H*DV_H + DV_R * DV_R);
	OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_m, V_m);
	PCMVMR(R_m, V_m, R_m, V_m_apo, DV_H, DV_R, 0.0, -1, V_m_apo, Pitch, Yaw);*/
	//Initialize
	sv[m].coe_osc = OrbMech::GIMIKC(R_m_b, V_m_apo, mu);
	sv[m].ENTRY = 0;
	sv[m].TIMA = 0;
	sv[m].TS = sv[m].TE = t_CDH;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
}

void RTCC::PMMLTR(AEGBlock sv_CSM, double T_LO, double V_H, double V_R, double h_BO, double t_PF, double P_FA, double Y_S, double r_LS, double lat_LS, double lng_LS, double &deltaw0, double &DR, double &deltaw, double &Yd, double &AZP)
{
	AEGDataBlock sv_temp, sv_L;
	VECTOR3 R_LS_sg, R_LSV, R_C, V_C, H_C, r_C, v_C, h_C, r_LSV, Q, c, R_LSP, r_LSP, R_BO, V_BO;
	double YSS, MJD_BO, DH, CXX, CYY, TRS, TS, phi_star, theta, DN, h_CA, h_CA_dot;
	int N, M;

	N = 1;
	M = 0;
	YSS = Y_S;
	Y_S = 0.0;

	sv_CSM.Data.TIMA = 0;
	sv_CSM.Data.TE = T_LO;
	pmmlaeg.CALL(sv_CSM.Header, sv_CSM.Data, sv_temp);
	if (N > 0)
	{
		goto RTCC_PMMLTR_3;
	}
	R_LS_sg = OrbMech::r_from_latlong(lat_LS, lng_LS, r_LS);
RTCC_PMMLTR_1:
	R_LSV = rhmul(OrbMech::GetRotationMatrix(BODY_MOON, GMTBASE + T_LO / 24.0 / 3600.0), R_LS_sg);
	OrbMech::GIMKIC(sv_temp.coe_osc, OrbMech::mu_Moon, R_C, V_C);
	r_C = unit(R_C);
	v_C = unit(V_C);
	H_C = crossp(r_C, v_C);
	h_C = unit(H_C);
RTCC_PMMLTR_2:
	r_LSV = unit(R_LSV);
	Q = crossp(r_LSV, h_C);
	c = unit(Q);
	R_LSP = crossp(h_C, c);
	r_LSP = unit(R_LSP);
	//Compute angle between landing site vector and its projection in CSM orbit plane
	theta = acos(dotp(r_LSP, r_LSV));
	if (M > 0)
	{
		Yd = r_LS * tan(theta);
		goto RTCC_PMMLTR_4;
	}
	DR = r_LS * tan(theta);
	M = 1;
RTCC_PMMLTR_3:
	sv_CSM.Data.TE = T_LO + t_PF;
	pmmlaeg.CALL(sv_CSM.Header, sv_CSM.Data, sv_temp);
	//Work on this
	h_CA = sv_temp.coe_mean.h;
	h_CA_dot = 0.0;
	OrbMech::ENSERT(R_C, V_C, t_PF, Y_S, P_FA, h_BO, V_H, V_R, GMTBASE + T_LO / 24 / 3600.0, R_LS_sg, R_BO, V_BO, MJD_BO);
	sv_L.coe_osc = OrbMech::GIMIKC(R_BO, V_BO, OrbMech::mu_Moon);
	sv_L.Item7 = GMTBASE;
	sv_L.TS = sv_L.TE = OrbMech::GETfromMJD(MJD_BO, GMTBASE);
	pmmlaeg.CALL(sv_CSM.Header, sv_L, sv_temp);
	if (N <= 0)
	{
		R_LSV = R_BO;
		goto RTCC_PMMLTR_2;
	}
RTCC_PMMLTR_4:
	DH = sv_L.coe_mean.h - sv_CSM.Data.coe_mean.h;
	CXX = cos(sv_L.coe_mean.i)*cos(sv_CSM.Data.coe_mean.i) + sin(sv_L.coe_mean.i)*sin(sv_CSM.Data.coe_mean.i)*cos(DH);
	CYY = sqrt(1.0 - CXX * CXX);
	if (N > 0)
	{
		Y_S = YSS;
		N = 0;
		goto RTCC_PMMLTR_1;
	}
	deltaw = atan2(CXX, CYY);
	DN = h_CA;
	if (DN <= 0)
	{
		DN = DN + PI2;
	}
	DN = DN - PI;
	if (DN < 0)
	{
		DN = DN + PI;
	}
	TRS = DN / (OrbMech::w_Moon + h_CA_dot) + T_LO + t_PF;
	//TBD: TRS is in days and gets rounded to full second?
	TS = T_LO - TRS;
	phi_star = lng_LS - OrbMech::w_Moon*TS;
	AZP = atan2(-cos(sv_L.coe_mean.i)*cos(lat_LS) + sin(sv_CSM.Data.coe_mean.i)*sin(phi_star)*sin(lat_LS), sin(sv_CSM.Data.coe_mean.i)*cos(phi_star));
	if (AZP < 0)
	{
		AZP += PI2;
	}
}

void RTCC::LunarLaunchWindowProcessor(const LunarLiftoffTimeOpt &opt)
{
	//0 = CSM, 1 = LM
	AEGHeader Header;
	AEGDataBlock sv[2], sv_temp[2], sv_xx, elem_TPI;
	VECTOR3 R_LS, R_LS_sg, r_LS, R1, V1, r1, Q, h1, H1, v1, C, R_P, r_P, H_D, h_D, R_BO, V_BO, R_TPI, V_TPI, R_XX, V_XX, R_TI_C, V_TI_C, R_TI_T, V_TI_T;
	double mu, T_TPI, T_hole, t_ca, theta_CA, theta, dt, S1, S2, t_xx, MJD_BO, T_INS, dt_B, t_CSI, t_D, U_R, t_R, r_apo, r_peri, H_S_apo, DH_MIN, DH_CRIT;
	double C1, C2, C3, C4, C5, C6, C7, C8, C9, C10, C11, C12, C13, C14, C15, P_mm, DH_u, theta_w, t_LOR, t_H, dv_CSI, dv_CDH, dv_TPI, dv_TPF, t_CDH, theta_TPI, dt_NSR, P_O, P_Q;
	double dt_max, B1, B2, B3, B4, DP, P_mm_apo, r_TPM, theta_s, f_TPI, C_R[3], C_V[3], DH_MAX, ddH_c;
	int K, m, p, I_SRCH, L_DH, I, J, K_T, i, I_STOP2, M_stop, IPHAS;
	int N_arr[10];
	double T_TPI_arr[10], T_R_arr[10], DV_CSI_arr[10], DV_CDH_arr[10], T_CDH_arr[10], DV_TPI_arr[10], DV_TPF_arr[10], DV_T_arr[10], T_LO_arr[10], T_CSI_arr[10], T_INS_arr[10];
	TwoImpulseOpt tiopt;
	TwoImpulseResuls tires;

	//Reset number of solutions
	PZLRPT.plans = 0;

	tiopt.mode = 5;
	tiopt.sv_A.RBI = BODY_MOON;
	tiopt.sv_P.RBI = BODY_MOON;

	mu = OrbMech::mu_Moon;
	T_hole = opt.t_hole;
	m = opt.M - 1;
	p = opt.P - 1;
	dt_B = opt.DT_B;
	I_SRCH = opt.I_SRCH;
	L_DH = opt.L_DH;
	double *DH = new double[L_DH];
	double *DH_apo = new double[L_DH];
	for (int ii = 0;ii < L_DH;ii++)
	{
		DH[ii] = opt.DH[ii];
	}

	Header.AEGInd = 1;
	Header.ErrorInd = 0;
	Header.NumBlocks = 1;
	sv[0].coe_osc = OrbMech::GIMIKC(opt.sv_CSM.R, opt.sv_CSM.V, mu);
	sv[0].Item7 = GMTBASE;
	sv[0].TE = sv[0].TS = OrbMech::GETfromMJD(opt.sv_CSM.MJD, GMTBASE);
	//Initialize
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);

	if (opt.I_TPI > 1)
	{
		T_TPI = opt.t_hole;
		T_hole = T_TPI - 1.5*PI2 / sv[0].l_dot;
		if (opt.I_CDH > 0)
		{
			T_hole = T_hole - PI2 / sv[0].l_dot*(double)(opt.I_CDH - 1);
		}
		if (opt.I_BURN == 1)
		{
			T_hole = T_hole - opt.DT_B;
		}
	}
	//Move to T_hole
	sv[0].TE = T_hole;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	//Initialize at T_hole
	sv[0] = sv_temp[0];
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	//Move to LS longitude
	PMMTLC(Header, sv[0], sv_temp[0], opt.lng, K, 0);
	t_xx = sv_temp[0].TE;
	sv_xx = sv_temp[0];
	//Initialize
	pmmlaeg.CALL(Header, sv_xx, sv_temp[0]);
	OrbMech::GIMKIC(sv_xx.coe_osc, mu, R_XX, V_XX);
	t_ca = t_xx;
	R_LS_sg = OrbMech::r_from_latlong(opt.lat, opt.lng, opt.R_LLS);
RTCC_PMMLWP_2:
	ELVCNV(R_LS_sg, t_ca, 3, 2, R_LS);
	r_LS = unit(R_LS);
	OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R1, V1);
	r1 = unit(R1);
	v1 = unit(V1);
	H1 = crossp(r1, v1);
	h1 = unit(H1);
	Q = crossp(r_LS, h1);
	C = unit(Q);
	R_P = crossp(h1, C);
	r_P = unit(R_P);
	theta_CA = acos(dotp(r_P, r1));
	H_D = crossp(r1, r_P);
	h_D = unit(H_D);
	S1 = h_D.z / abs(h_D.z);
	S2 = h1.z / abs(h1.z);
	theta = theta_CA * S1 / S2;
	dt = theta / sv[0].l_dot;
	if (abs(dt) > 0.1)
	{
		t_ca = t_ca + dt;
		sv[0].TE = t_ca;
		pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
		goto RTCC_PMMLWP_2;
	}
	//sv[0].TE = t_xx;
	//pmmlaeg.CALL(Header, sv[0], sv_temp);
	OrbMech::ENSERT(R_XX, V_XX, opt.dt_1, opt.Y_S, opt.theta_1, opt.h_BO, opt.v_LH, opt.v_LV, OrbMech::MJDfromGET(t_xx, GMTBASE), R_LS_sg, R_BO, V_BO, MJD_BO);
	T_INS = OrbMech::GETfromMJD(MJD_BO, GMTBASE);
	
	sv[1].coe_osc = OrbMech::GIMIKC(R_BO, V_BO, mu);
	sv[1].Item7 = GMTBASE;
	sv[1].TE = sv[1].TS = T_INS;
	//Initialize
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
	if (opt.I_BURN <= 1)
	{
		if (opt.I_BURN < 1)
		{
			dt_B = 1.0 / sv[m].l_dot*acos(sv[m].coe_mean.e - sv[m].coe_mean.e*sqrt(1.0 - pow(sv[m].coe_mean.e, 2)));
		}
		t_CSI = T_INS + dt_B;
	}
	else
	{
		sv[1].TIMA = 1;
		sv[1].Item8 = PI;
		pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
		t_CSI = sv_temp[1].TE;
		if (opt.M <= 1)
		{
			t_CSI = t_CSI + opt.t_BASE;
		}
		dt_B = t_CSI - T_INS;
	}
	sv[0].TIMA = sv[1].TIMA = 0;
	sv[0].TE = sv[1].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
	int MM, I_LOOP, I_SELECT, I_STOP1, J_MOV, I_LAST, I_SAVE, I_CURVE;

	MM = 2;
	I_LOOP = 1;
	I_SELECT = 1;
	I_STOP1 = 1;
	J_MOV = 0;
	I_LAST = 0;
	I_SAVE = 1;
	I_CURVE = 1;

	if (opt.M < 2)
	{
		OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R_TPI, V_TPI);
		elem_TPI = sv_temp[0];
		I_SRCH = 0;
		goto RTCC_PMMLLWP_6_2;
	}
	t_D = t_CSI + PI2 / sv[1].l_dot / 2.0;
	if (opt.I_CDH > 0)
	{
		t_D = t_D + PI2 / sv[1].l_dot / 2.0*(double)(opt.I_CDH - 1);
	}
	sv[0].TE = t_D;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	sv[0] = sv_temp[0];
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	if (opt.I_TPI > 1)
	{
		sv[0].TE = T_TPI;
		pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	}
	else
	{
		PMMTLC(Header, sv[0], sv_temp[0], opt.lng_TPI, K, 0);
		T_TPI = sv_temp[0].TE;
	}
	OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R_TPI, V_TPI);
	elem_TPI = sv_temp[0];
	U_R = sv_temp[0].U + opt.theta_F;
	sv[0].Item10 = 0.0;
	if (U_R >= PI2)
	{
		U_R -= PI2;
		sv[0].Item10 += 1.0;
	}
	sv[0].Item10 += trunc((sv_temp[0].TE - sv[0].TS) / (PI2 / sv[0].l_dot));
	if (sv[0].U > sv_temp[0].U)
	{
		sv[0].Item10 += 1.0;
	}
	sv[0].TIMA = 2;
	sv[0].Item8 = U_R;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	t_R = sv_temp[0].TE;
	LLWP_PERHAP(Header, sv[p], r_apo, r_peri);
	H_S_apo = r_peri - opt.R_LLS;
	DH_MIN = H_S_apo - opt.H_S;
	DH_CRIT = sv_temp[0].R * (1.0 - 1.0 / cos(opt.E));
	K_T = 0;
	I = 1;
	J = 1;
	if (I_SRCH > 0)
	{
		DH_apo[J - 1] = DH_MIN;
		J = 2;
	}

RTCC_PMMLLWP_4:
	if (opt.DH[I - 1] >= DH_MIN || opt.DH[I-1]<=DH_CRIT)
	{
		if (I < L_DH)
		{
			goto RTCC_PMMLLWP_4;
		}
		if (K_T <= 0)
		{
			if (I_SRCH <= 0)
			{
				//error
				return;
			}
			L_DH = J;
			DH_apo[J - 1] = opt.DH_SRCH;
		}
		else
		{
			if (I_SRCH <= 0)
			{
				L_DH = J;
			}
			else
			{
				L_DH = J + 1;
				DH_apo[J - 1] = opt.DH_SRCH;
			}
		}
	}
	else
	{
		DH_apo[J - 1] = opt.DH[I - 1];
		if (I < L_DH)
		{
			K_T = 1;
			I++;
			J++;
			goto RTCC_PMMLLWP_4;
		}
		if (I_SRCH <= 0)
		{
			L_DH = J;
		}
		else
		{
			L_DH = J + 1;
			DH_apo[J - 1] = opt.DH_SRCH;
		}
	}
	i = 1;
	I_STOP2 = L_DH;
	while (i < L_DH)
	{
		DH[i - 1] = DH_apo[i - 1];
		i++;
	}
	P_mm = PI2 / sv[m].l_dot;
RTCC_PMMLLWP_6_2:
	C1 = sv_temp[p].R;
	C2 = sv_temp[m].R;
	C3 = PI2 / sv[p].l_dot;
	C4 = sv_temp[p].coe_osc.a;
	C5 = (opt.dt_1*sv[p].l_dot - opt.theta_1) / sv[p].l_dot;
	C6 = (C3 - PI2 / sv[m].l_dot)*dt_B / C3;
	DH_u = DH[I_SELECT - 1];
RTCC_PMMLLWP_7_7:
	IPHAS = 0;
	theta_w = PI05 - opt.E - asin((elem_TPI.R - DH_u)*cos(opt.E) / elem_TPI.R);
	do
	{
		theta_s = theta_w;
		IPHAS++;
		f_TPI = elem_TPI.U - elem_TPI.coe_osc.g - theta_w;
		r_TPM = elem_TPI.coe_osc.a*(1.0 - elem_TPI.coe_osc.e*elem_TPI.coe_osc.e) / (1.0 + elem_TPI.coe_osc.e*cos(f_TPI));
		theta_w = PI05 - opt.E - asin((elem_TPI.R - DH_u)*cos(opt.E) / elem_TPI.R);
	} while (abs(theta_w - theta_s) > 0.01*RAD && IPHAS < 10);

	C7 = (C2 + C1 - DH_u) / 2.0;
	C8 = 1.0 / C7 * sqrt(mu / C7);
	C9 = PI / C8;
	C10 = (C3 - 2.0*C9)*C9 / C3;
	C11 = theta_w / sv[p].l_dot;
	C12 = C4 - DH_u;
	C13 = 1.0 / C12 * sqrt(mu / C12);
	C14 = PI2 / C13;
	C15 = (C3 - C14)*C14 / (4.0*C3);
	t_LOR = t_xx - C5 + abs(C6) + abs(C10) + C15 - C11;
RTCC_PMMLLWP_8_8:
	sv[0] = sv_xx;
	sv[0].TE = t_LOR;
	sv[0].TIMA = 0;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	OrbMech::GIMKIC(sv_temp[0].coe_osc, mu, R1, V1);
	OrbMech::ENSERT(R1, V1, opt.dt_1, opt.Y_S, opt.theta_1, opt.h_BO, opt.v_LH, opt.v_LV, GMTBASE + t_LOR / 24.0 / 3600.0, R_LS_sg, R_BO, V_BO, MJD_BO);
	T_INS = OrbMech::GETfromMJD(MJD_BO, GMTBASE);

	sv[1].coe_osc = OrbMech::GIMIKC(R_BO, V_BO, mu);
	sv[1].TE = sv[1].TS = T_INS;
	sv[1].ENTRY = 0;
	sv[1].TIMA = 0;
	//Initialize
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);

	if (opt.M <= 1)
	{
		t_H = t_CSI + PI2 / sv[m].l_dot / 2.0;
		if (opt.I_CDH > 0)
		{
			t_H = t_H + PI2 / sv[m].l_dot / 2.0*(double)(opt.I_CDH - 1);
		}
		if (opt.I_TPI > 1)
		{
			sv[p].TE = T_TPI;
			pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
		}
		else
		{
			//Advance to t_H
			sv[p].TE = t_H;
			pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
			//Initialize at t_H
			sv[p] = sv_temp[p];
			pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
			//Find time of arrival at TPI
			PMMTLC(Header, sv[p], sv_temp[p], opt.lng_TPI, K, 0);
			T_TPI = sv_temp[p].TE;
		}
		OrbMech::GIMKIC(sv_temp[p].coe_osc, mu, R_TPI, V_TPI);
		elem_TPI = sv_temp[p];
		//Find LM time of arrival at U_R
		U_R = sv_temp[p].U + opt.theta_F;
		sv[p].Item10 = 0.0;
		if (U_R >= PI2)
		{
			U_R -= PI2;
			sv[p].Item10 += 1.0;
		}
		sv[p].Item10 += trunc((sv_temp[p].TE - sv[p].TS) / (PI2 / sv[p].l_dot));
		if (sv[p].U > sv_temp[p].U)
		{
			sv[p].Item10 += 1.0;
		}
		sv[p].TIMA = 2;
		sv[p].Item8 = U_R;
		pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
		t_R = sv_temp[p].TE;

		IPHAS = 0;
		theta_w = PI05 - opt.E - asin((length(R_TPI) - DH_u)*cos(opt.E) / length(R_TPI));
		do
		{
			theta_s = theta_w;
			IPHAS++;
			f_TPI = elem_TPI.U - elem_TPI.coe_osc.g - theta_w;
			r_TPM = elem_TPI.coe_osc.a*(1.0 - elem_TPI.coe_osc.e*elem_TPI.coe_osc.e) / (1.0 + elem_TPI.coe_osc.e*cos(f_TPI));
			theta_w = PI05 - opt.E - asin((elem_TPI.R - DH_u)*cos(opt.E) / elem_TPI.R);
		} while (abs(theta_w - theta_s) > 0.01*RAD && IPHAS < 10);

	}
	if (DH_u == 0)
	{
		theta_w += opt.dTheta_OFF;
	}
	//Save output quantities
	N_arr[I_LOOP - 1] = 2;
	T_TPI_arr[I_LOOP - 1] = T_TPI;
	T_R_arr[I_LOOP - 1] = t_R;
	//Move both vehicles to time of insertion
	sv[0].TIMA = sv[1].TIMA = 0;
	sv[0].TE = sv[1].TE  = T_INS;
	pmmlaeg.CALL(Header, sv[0], sv_temp[0]);
	pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
	theta = OrbMech::THETR(sv[p].U, sv[m].U, sv[p].coe_osc.i, sv[m].coe_osc.i, sv[p].coe_osc.h, sv[m].coe_osc.h);
	//TBD: Adjust mean anomaly
	if (opt.I_BURN <= 1)
	{
		if (opt.I_BURN < 1)
		{
			dt_B = 1.0 / sv[m].l_dot*acos(sv[m].coe_mean.e - sv[m].coe_mean.e*sqrt(1.0 - pow(sv[m].coe_mean.e, 2)));
		}
		t_CSI = T_INS + dt_B;
	}
	else
	{
		sv[1].TIMA = 1;
		sv[1].Item8 = PI;
		sv[1].Item10 = 0.0;
		pmmlaeg.CALL(Header, sv[1], sv_temp[1]);
		t_CSI = sv_temp[1].TE;
		if (opt.M <= 1)
		{
			t_CSI = t_CSI + opt.t_BASE;
		}
	}
	//Advance both to CSI
	sv[m].TIMA = sv[p].TIMA = 0;
	sv[m].TE = sv[p].TE = t_CSI;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	pmmlaeg.CALL(Header, sv[p], sv_temp[p]);

	LLWP_HMALIT(Header, sv, sv_temp, opt.M, opt.P, opt.I_CDH, DH_u, dv_CSI, dv_CDH, t_CDH);
	//Advance both vehicles to TPI maneuver point
	sv[m].TE = T_TPI;
	sv[m].TIMA = 0;
	pmmlaeg.CALL(Header, sv[m], sv_temp[m]);
	sv[p].TE = T_TPI;
	sv[p].TIMA = 0;
	pmmlaeg.CALL(Header, sv[p], sv_temp[p]);
	//Phase angle at TPI
	theta_TPI = OrbMech::THETR(sv_temp[p].U, sv_temp[m].U, sv_temp[p].coe_osc.i, sv_temp[m].coe_osc.i, sv_temp[p].coe_osc.h, sv_temp[m].coe_osc.h);
	if (theta_TPI > PI)
	{
		theta_TPI -= PI2;
	}
	else if (theta_TPI < -PI)
	{
		theta_TPI += PI2;
	}

	dt_NSR = (theta_TPI - theta_w) / sv[m].l_dot;
	if (abs(dt_NSR) > 0.1)
	{
		if (opt.M > 1)
		{
			dt_NSR = -dt_NSR;
		}
		t_LOR = t_LOR + dt_NSR;
		goto RTCC_PMMLLWP_8_8;
	}
	P_O = PI2 / sv[p].l_dot;
	P_Q = PI2 / sv[m].l_dot;
	DV_CSI_arr[I_SELECT - 1] = dv_CSI;
	DV_CDH_arr[I_SELECT - 1] = dv_CDH;
	T_CDH_arr[I_LOOP - 1] = t_CDH;
	dv_TPI = 0.0;
	dv_TPF = 0.0;
	if (abs(DH_u) != 0.0)
	{
		//Call two impulse
		tiopt.T1 = T_TPI;
		tiopt.T2 = t_R;
		OrbMech::GIMKIC(sv_temp[m].coe_osc, mu, R_TI_C, V_TI_C);
		OrbMech::GIMKIC(sv_temp[p].coe_osc, mu, R_TI_T, V_TI_T);
		tiopt.sv_A.R = R_TI_C;
		tiopt.sv_A.V = V_TI_C;
		tiopt.sv_A.GMT = sv_temp[m].TE;
		tiopt.sv_P.R = R_TI_T;
		tiopt.sv_P.V = V_TI_T;
		tiopt.sv_P.GMT = sv_temp[p].TE;
		PMSTICN(tiopt, tires);
		dv_TPI = length(tires.dV);
		dv_TPF = length(tires.dV2);
	}
	DV_TPI_arr[I_SELECT - 1] = dv_TPI;
	DV_TPF_arr[I_SELECT - 1] = dv_TPF;
	DV_T_arr[I_SELECT - 1] = abs(dv_CSI) + dv_CDH + dv_TPI + dv_TPF;
	if (J_MOV > 0)
	{
		goto RTCC_PMMLLWP_20_16;
	}
	T_LO_arr[I_LOOP - 1] = t_LOR;
	T_INS_arr[I_LOOP - 1] = T_INS;
	T_CSI_arr[I_LOOP - 1] = t_CSI;
	dt_max = t_R - t_LOR;
	B1 = T_CSI_arr[I_LOOP - 1];
	B2 = T_CDH_arr[I_LOOP - 1];
	B3 = T_TPI_arr[I_LOOP - 1];
	B4 = T_R_arr[I_LOOP - 1];

	if (I_LAST > 0)
	{
		goto RTCC_PMMLLWP_18_17;
	}
	if (I_SELECT <= I_STOP1)
	{
		goto RTCC_PMMLLWP_17_18;
	}
	if (I_SELECT < I_STOP2 - 1)
	{
		goto RTCC_PMMLLWP_19_20;
	}
	else if (I_SELECT > I_STOP2 - 1)
	{
		goto RTCC_PMMLLWP_22_19;
	}
	if (I_SRCH > 0)
	{
		goto RTCC_PMMLLWP_20_22;
	}
	else
	{
		goto RTCC_PMMLLWP_19_20;
	}
RTCC_PMMLLWP_17_18:
	if (I_SRCH <= 0)
	{
		goto RTCC_PMMLLWP_19_20;
	}
	DP = P_O - P_mm;
	P_mm_apo = P_mm;
	M_stop = MM + (int)((opt.t_max - dt_max) / P_mm_apo);
RTCC_PMMLLWP_17_24:
	T_CSI_arr[I_LOOP - 1] = B1 + P_mm_apo * (double)(M_stop - 2);
	T_CDH_arr[I_LOOP - 1] = B2 + P_mm_apo * (double)(M_stop - 2);
	goto RTCC_PMMLLWP_18_25;
RTCC_PMMLLWP_18_17:
	if (DH_u > 0)
	{
		goto RTCC_PMMLLWP_22_19;
	}
	P_mm_apo = P_Q;
	DP = -(P_O - P_mm_apo);
	M_stop = MM + (int)((opt.t_max - dt_max) / P_mm_apo);
RTCC_PMMLLWP_18_26:
	T_CSI_arr[I_LOOP - 1] = B1 + DP * (double)(M_stop - 2);
	T_CDH_arr[I_LOOP - 1] = B2 + DP * (double)(M_stop - 2);
RTCC_PMMLLWP_18_25:
	T_LO_arr[I_LOOP - 1] = t_LOR + DP * (double)(M_stop - 2);
	T_TPI_arr[I_LOOP - 1] = B1 + P_mm_apo * (double)(M_stop - 2);
	T_R_arr[I_LOOP - 1] = B4 + P_mm_apo * (double)(M_stop - 2);
	N_arr[I_LOOP - 1] = M_stop;
	if (MM - M_stop != 0)
	{
		M_stop = 2;
		I_LOOP++;
		if (I_SELECT <= 1)
		{
			goto RTCC_PMMLLWP_17_24;
		}
		goto RTCC_PMMLLWP_18_26;
	}
RTCC_PMMLLWP_19_20:
	MM = 2;
	if (I_LAST > 0)
	{
		goto RTCC_PMMLLWP_22_19;
	}
	I_LOOP++;
	I_SELECT++;
	DH_u = DH[I_SELECT - 1];
	goto RTCC_PMMLLWP_7_7;
RTCC_PMMLLWP_20_22:
	J_MOV = 1;
	I_SELECT++;
	I_LOOP++;
	DH_u = DH[I_SELECT - 1];
	goto RTCC_PMMLLWP_7_7;
RTCC_PMMLLWP_20_16:
	C_R[I_CURVE - 1] = DH_u;
	C_V[I_CURVE - 1] = DV_T_arr[I_SELECT - 1];
	if (I_CURVE < opt.N_CURV)
	{
		I_CURVE++;
		DH_u = DH_u - opt.DH_SRCH;
		goto RTCC_PMMLLWP_7_7;
	}
	DH_MAX = LLWP_CURVE(C_V[0], C_V[1], C_V[2], C_R[0], C_R[1], C_R[2], opt.DV_MAX[m]);
	ddH_c = DH_MAX - DH_CRIT;
	DH[I_SELECT - 1] = DH_MAX;
	if (ddH_c < 0)
	{
		DH[I_SELECT - 1] = DH_CRIT;
	}
	DH_u = DH[I_SELECT - 1];
	I_LAST = 1;
	J_MOV = 0;
	goto RTCC_PMMLLWP_7_7;
RTCC_PMMLLWP_22_19:
	//Set up RET and RPT
	PZLRPT.plans = I_LOOP;
	PZLRPT.CSMSTAID = "";
	PZLRPT.CSM_GMTV = OrbMech::GETfromMJD(opt.sv_CSM.MJD, GMTBASE);
	PZLRPT.CSM_GETV = GETfromGMT(PZLRPT.CSM_GMTV);
	if (opt.M == 2)
	{
		PZLRPT.ManVeh = "LM";
	}
	else
	{
		PZLRPT.ManVeh = "CSM";
	}
	PZLRPT.LSLat = opt.lat*DEG;
	PZLRPT.LSLng = opt.lng*DEG;
	PZLRPT.THT = GETfromGMT(opt.t_hole);
	PZLRPT.LMSTAID = "";
	PZLRPT.LM_GMTV = 0.0;
	PZLRPT.LM_GETV = 0.0;
	PZLRPT.DT_CSI = dt_B;
	PZLRPT.DV_MAX = opt.DV_MAX[m] / 0.3048;
	PZLRPT.WT = opt.theta_F*DEG;
	for (int i = 0;i < I_LOOP;i++)
	{
		PZLRPT.data[i].ID = i + 1;
		PZLRPT.data[i].N = N_arr[i];
		PZLRPT.data[i].DH = DH[i] / 1852.0;
		PZLRPT.data[i].GETLO = GETfromGMT(T_LO_arr[i]);
		PZLRPT.data[i].T_INS = GETfromGMT(T_INS_arr[i]);
		PZLRPT.data[i].T_CSI = GETfromGMT(T_CSI_arr[i]);
		PZLRPT.data[i].T_CDH = GETfromGMT(T_CDH_arr[i]);
		PZLRPT.data[i].T_TPI = GETfromGMT(T_TPI_arr[i]);
		PZLRPT.data[i].T_TPF = GETfromGMT(T_R_arr[i]);
		PZLRPT.data[i].DVCSI = DV_CSI_arr[i] / 0.3048;
		PZLRPT.data[i].DVCDH = DV_CDH_arr[i] / 0.3048;
		PZLRPT.data[i].DVTPI = DV_TPI_arr[i] / 0.3048;
		PZLRPT.data[i].DVTPF = DV_TPF_arr[i] / 0.3048;
		PZLRPT.data[i].DVT = DV_T_arr[i] / 0.3048;
	}

	delete[] DH_apo;
}

void RTCC::LaunchTimePredictionProcessor(const LunarLiftoffTimeOpt &opt, LunarLiftoffResults &res)
{
	VECTOR3 R_LS;
	double h_1, theta_Ins, MJD_guess, t_L_guess;
	SV sv_P, sv_Ins;
	OBJHANDLE hMoon;

	hMoon = oapiGetObjectByName("Moon");
	h_1 = 60000.0*0.3048;
	theta_Ins = 18.2*RAD;

	R_LS = OrbMech::r_from_latlong(opt.lat, opt.lng, opt.R_LLS);

	if (opt.I_TPI > 1)
	{
		//TPI time fixed for concentric profile
		res.t_TPI = opt.t_hole;
		t_L_guess = res.t_TPI - 1.5*7200.0;
		sv_P = coast(opt.sv_CSM, t_L_guess - OrbMech::GETfromMJD(opt.sv_CSM.MJD, opt.GETbase));
	}
	else
	{
		//Initial guess for launch is CSM flying over landing site longitude
		sv_P = coast(opt.sv_CSM, opt.t_hole - OrbMech::GETfromMJD(opt.sv_CSM.MJD, opt.GETbase));
		MJD_guess = OrbMech::P29TimeOfLongitude(sv_P.R, sv_P.V, sv_P.MJD, sv_P.gravref, opt.lng);
		t_L_guess = OrbMech::GETfromMJD(MJD_guess, opt.GETbase);
	}

	if (opt.I_TPI <= 1)
	{
		SV sv_TPI_guess;
		double t_TPI_guess, ttoMidnight;
		OBJHANDLE hSun;

		hSun = oapiGetObjectByName("Sun");

		//About 2.5 hours between liftoff and TPI
		t_TPI_guess = t_L_guess + 2.5*3600.0;
		sv_TPI_guess = coast(sv_P, t_TPI_guess - OrbMech::GETfromMJD(sv_P.MJD, opt.GETbase));

		ttoMidnight = OrbMech::sunrise(sv_TPI_guess.R, sv_TPI_guess.V, sv_TPI_guess.MJD, hMoon, hSun, 1, 1, false);
		res.t_TPI = t_TPI_guess + ttoMidnight;
	}

	LunarLiftoffTimePredictionCFP(opt, R_LS, sv_P, hMoon, h_1, theta_Ins, t_L_guess, res.t_TPI, res);
}

void RTCC::EntryUpdateCalc(SV sv0, double GETbase, double entryrange, bool highspeed, EntryResults *res)
{
	OBJHANDLE hEarth;
	VECTOR3 REI, VEI, UREI, R3, V3, R05G, V05G;
	double EntryInterface, RCON, dt2, MJD_EI, lambda, phi, MJD_l, vEI, t32, dt22, EMSAlt, t2;

	hEarth = oapiGetObjectByName("Earth");

	EntryInterface = 400000.0 * 0.3048;
	RCON = OrbMech::R_Earth + EntryInterface;
	if (highspeed)
	{
		EMSAlt = 297431.0*0.3048;
	}
	else
	{
		EMSAlt = 284643.0*0.3048;
	}

	dt2 = OrbMech::time_radius_integ(sv0.R, sv0.V, sv0.MJD, RCON, -1, sv0.gravref, hEarth, REI, VEI);
	MJD_EI = sv0.MJD + dt2 / 24.0 / 3600.0;
	t2 = (sv0.MJD - GETbase) * 24.0 * 3600.0 + dt2;	//EI time in seconds from launch

	UREI = unit(REI);
	vEI = length(VEI);
	res->ReA = asin(dotp(UREI, VEI) / vEI);

	t32 = OrbMech::time_radius(REI, VEI, RCON - 30480.0, -1, OrbMech::mu_Earth);
	OrbMech::rv_from_r0v0(REI, VEI, t32, R3, V3, OrbMech::mu_Earth);
	dt22 = OrbMech::time_radius(R3, V3, length(R3) - (300000.0 * 0.3048 - EMSAlt), -1, OrbMech::mu_Earth);
	OrbMech::rv_from_r0v0(R3, V3, dt22, R05G, V05G, OrbMech::mu_Earth);

	res->RTGO = EntryCalculations::LNDING(REI, VEI, MJD_EI, 0.3, 2, entryrange, lambda, phi, MJD_l);

	res->latitude = phi;
	res->longitude = lambda;

	res->VIO = length(V05G);
	res->GET400K = t2;
	res->GET05G = t2 + t32 + dt22;
}

double RTCC::CalculateTPITimes(SV sv0, int tpimode, double t_TPI_guess, double dt_TPI_sunrise)
{
	//tpimode: 0 = TPI on time, 1 = TPI at orbital midnight, 2 = TPI at X minutes before sunrise
	SV sv_TPI_guess;
	OBJHANDLE hSun = oapiGetObjectByName("Sun");
	double t_TPI;

	
	if (tpimode == 0)
	{
		t_TPI = t_TPI_guess;
	}
	else if (tpimode == 1)
	{
		sv_TPI_guess = coast(sv0, t_TPI_guess - OrbMech::GETfromMJD(sv0.MJD, CalcGETBase()));
		double ttoMidnight;
		ttoMidnight = OrbMech::sunrise(sv_TPI_guess.R, sv_TPI_guess.V, sv_TPI_guess.MJD, sv_TPI_guess.gravref, hSun, 1, 1, false);
		t_TPI = t_TPI_guess + ttoMidnight;
	}
	else
	{
		SV sv_sunrise_guess;
		double ttoSunrise;

		sv_TPI_guess = coast(sv0, t_TPI_guess - OrbMech::GETfromMJD(sv0.MJD, CalcGETBase()));
		sv_sunrise_guess = coast(sv_TPI_guess, dt_TPI_sunrise);
		ttoSunrise = OrbMech::sunrise(sv_sunrise_guess.R, sv_sunrise_guess.V, sv_sunrise_guess.MJD, sv_sunrise_guess.gravref, hSun, 1, 0, false);
		t_TPI = t_TPI_guess + ttoSunrise;
	}

	return t_TPI;
}

bool RTCC::DockingInitiationProcessor(DKIOpt opt, DKIResults &res)
{
	// NOMENCLATURE:
	// R = position vector, V = velocity vector
	// A = active vehicle, P = passive vehicle
	// P = phasing maneuver, B = boost, HAM = height adjustment, H = height maneuver, C = coelliptic maneuver, F = final maneuver (TPI)

	//Check on N
	if (opt.plan == 0 || opt.plan == 1)
	{
		if (opt.N_HC % 2 == 0) return false;
		if (opt.N_PB % 2 == 0) return false;
	}
	else if (opt.plan == 2)
	{
		if (opt.N_HC % 2 != 0) return false;
		if (opt.N_PB % 2 == 0) return false;
	}
	else if (opt.plan == 4)
	{
		if (opt.N_HC % 2 == 0) return false;
		if (opt.N_PB % 2 != 0) return false;
	}

	SV sv_AP, sv_TPI_guess, sv_TPI, sv_PC;
	VECTOR3 u, R_AP, V_AP, V_APF, R_AH, V_AH, V_AHF, R_AC, V_AC, R_PC, V_PC, V_ACF, R_PJ, V_PJ, R_AFD, R_AF, V_AF;
	double mu, dv_P, p_P, dt_PH, c_P, eps_P, dv_H, dt_HC, t_H, t_C, dv_Po, e_Po, e_P, dv_rad_const;
	int s_P;

	eps_P = 0.000004;	//radians
	s_P = 0;
	p_P = c_P = 0.0;
	if (opt.plan == 4)
	{
		dv_P = 300.0*0.3048;
	}
	else
	{
		dv_P = 100.0*0.3048;
	}
	dv_H = 0.0;
	mu = GGRAV * oapiGetMass(opt.sv_A.gravref);

	if (opt.radial_dv == false)
	{
		dv_rad_const = 0.0;
	}
	else
	{
		dv_rad_const = -50.0*0.3048;
	}

	sv_TPI_guess = coast(opt.sv_P, opt.t_TPI_guess - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));

	if (opt.tpimode == 0)
	{
		sv_TPI = sv_TPI_guess;
		res.t_TPI = opt.t_TPI_guess;
	}
	else
	{
		OBJHANDLE hSun = oapiGetObjectByName("Sun");

		if (opt.tpimode == 1)
		{

			double ttoMidnight;
			ttoMidnight = OrbMech::sunrise(sv_TPI_guess.R, sv_TPI_guess.V, sv_TPI_guess.MJD, sv_TPI_guess.gravref, hSun, 1, 1, false);
			res.t_TPI = opt.t_TPI_guess + ttoMidnight;
			sv_TPI = coast(sv_TPI_guess, res.t_TPI - opt.t_TPI_guess);
		}
		else
		{
			SV sv_sunrise_guess;
			double ttoSunrise;

			sv_sunrise_guess = coast(sv_TPI_guess, opt.dt_TPI_sunrise);
			ttoSunrise = OrbMech::sunrise(sv_sunrise_guess.R, sv_sunrise_guess.V, sv_sunrise_guess.MJD, sv_sunrise_guess.gravref, hSun, 1, 0, false);
			res.t_TPI = opt.t_TPI_guess + ttoSunrise;
		}

		sv_TPI = coast(sv_TPI_guess, res.t_TPI - opt.t_TPI_guess);
	}

	//If we just needed the TPI time, return here
	if (opt.plan == 3) return true;

	sv_AP = coast(opt.sv_A, opt.t_TIG - OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase));

	OrbMech::QDRTPI(sv_TPI.R, sv_TPI.V, sv_TPI.MJD, sv_TPI.gravref, mu, opt.DH, opt.E, 0, R_PJ, V_PJ);
	R_AFD = R_PJ - unit(R_PJ)*opt.DH;

	u = unit(crossp(sv_TPI.R, sv_TPI.V));

	R_AP = unit(sv_AP.R - u * dotp(sv_AP.R, u))*length(sv_AP.R);
	V_AP = unit(sv_AP.V - u * dotp(sv_AP.V, u))*length(sv_AP.V);

	if (opt.plan == 2)
	{
		OrbMech::CSIToDH(R_AP, V_AP, sv_TPI.R, sv_TPI.V, opt.DH, mu, dv_P);
		V_APF = V_AP + tmul(OrbMech::LVLH_Matrix(R_AP, V_AP), _V(dv_P, 0.0, dv_rad_const));

		OrbMech::REVUP(R_AP, V_APF, 0.5, mu, R_AH, V_AH, dt_PH);
		t_H = opt.t_TIG + dt_PH;

		do
		{
			V_AHF = V_AH + unit(crossp(u, R_AH))*dv_H;
			OrbMech::REVUP(R_AH, V_AHF, 0.5*(double)opt.N_HC, mu, R_AC, V_AC, dt_HC);
			t_C = t_H + dt_HC;

			OrbMech::RADUP(sv_TPI.R, sv_TPI.V, R_AC, mu, R_PC, V_PC);
			V_ACF = OrbMech::CoellipticDV(R_AC, R_PC, V_PC, mu);

			OrbMech::rv_from_r0v0(R_AC, V_ACF, res.t_TPI - t_C, R_AF, V_AF, mu);
			e_P = OrbMech::sign(dotp(crossp(R_AF, R_AFD), u))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));

			if (p_P == 0 || abs(e_P) >= eps_P)
			{
				OrbMech::ITER(c_P, s_P, e_P, p_P, dv_H, e_Po, dv_Po);
			}
		} while (abs(e_P) >= eps_P);
	}
	else
	{
		do
		{
			V_APF = V_AP + tmul(OrbMech::LVLH_Matrix(R_AP, V_AP), _V(dv_P, 0.0, dv_rad_const));

			if (opt.plan == 1)
			{
				VECTOR3 R_AB, V_AB, V_ABF, R_AHAM, V_AHAM;
				double dt_PB, t_B, dv_B, t_HAM, dt_BHAM, dt_HAMH;

				dv_B = 10.0*0.3048;

				if (opt.maneuverline)
				{
					dt_PB = OrbMech::timetoapo(R_AP, V_APF, mu, 1);
				}
				else
				{
					dt_PB = opt.DeltaT_PBH;
				}

				t_B = opt.t_TIG + dt_PB;
				OrbMech::rv_from_r0v0(R_AP, V_APF, dt_PB, R_AB, V_AB, mu);

				V_ABF = V_AB + unit(crossp(u, R_AB))*dv_B;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AB, V_ABF, 0.5*(double)opt.N_PB, mu, R_AHAM, V_AHAM, dt_BHAM);
				}
				else
				{
					dt_BHAM = opt.DeltaT_BHAM;
					OrbMech::rv_from_r0v0(R_AB, V_ABF, dt_BHAM, R_AHAM, V_AHAM, mu);
				}

				t_HAM = t_B + dt_BHAM;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AHAM, V_AHAM, 0.5, mu, R_AH, V_AH, dt_HAMH);
				}
				else
				{
					dt_HAMH = opt.Delta_HAMH;
					OrbMech::rv_from_r0v0(R_AHAM, V_AHAM, dt_HAMH, R_AH, V_AH, mu);
				}

				t_H = t_HAM + dt_HAMH;

				res.t_Boost = t_B;
				res.dv_Boost = dv_B;
				res.t_HAM = t_HAM;
			}
			else if (opt.plan == 4)
			{
				VECTOR3 R_AB, V_AB, V_ABF;
				double dt_PB, dv_B, dt_BH, t_B;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AP, V_APF, 0.5*(double)opt.N_PB, mu, R_AB, V_AB, dt_PB);
				}
				else
				{
					dt_PB = opt.DeltaT_PBH;
					OrbMech::rv_from_r0v0(R_AP, V_APF, dt_PB, R_AB, V_AB, mu);
				}

				t_B = opt.t_TIG + dt_PB;

				OrbMech::CSIToDH(R_AB, V_AB, sv_TPI.R, sv_TPI.V, opt.DH, mu, dv_B);
				V_ABF = V_AB + unit(crossp(u, R_AB))*dv_B;

				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AB, V_ABF, 0.5, mu, R_AH, V_AH, dt_BH);
				}
				else
				{
					dt_BH = opt.DeltaT_BHAM;
					OrbMech::rv_from_r0v0(R_AB, V_ABF, dt_BH, R_AH, V_AH, mu);
				}

				t_H = t_B + dt_BH;

				res.t_Boost = t_B;
				res.dv_Boost = dv_B;
			}
			else
			{
				if (opt.maneuverline)
				{
					OrbMech::REVUP(R_AP, V_APF, 0.5*(double)opt.N_PB, mu, R_AH, V_AH, dt_PH);
				}
				else
				{
					dt_PH = opt.DeltaT_PBH;
					OrbMech::rv_from_r0v0(R_AP, V_APF, dt_PH, R_AH, V_AH, mu);
				}

				t_H = opt.t_TIG + dt_PH;
			}

			OrbMech::CSIToDH(R_AH, V_AH, sv_TPI.R, sv_TPI.V, opt.DH, mu, dv_H);
			V_AHF = V_AH + unit(crossp(u, R_AH))*dv_H;
			OrbMech::REVUP(R_AH, V_AHF, 0.5*(double)opt.N_HC, mu, R_AC, V_AC, dt_HC);
			t_C = t_H + dt_HC;
			OrbMech::RADUP(sv_TPI.R, sv_TPI.V, R_AC, mu, R_PC, V_PC);

			V_ACF = OrbMech::CoellipticDV(R_AC, R_PC, V_PC, mu);
			OrbMech::rv_from_r0v0(R_AC, V_ACF, res.t_TPI - t_C, R_AF, V_AF, mu);
			e_P = OrbMech::sign(dotp(crossp(R_AF, R_AFD), u))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));

			if (p_P == 0 || abs(e_P) >= eps_P)
			{
				OrbMech::ITER(c_P, s_P, e_P, p_P, dv_P, e_Po, dv_Po);
			}
		} while (abs(e_P) >= eps_P);
	}

	res.DV_Phasing = _V(dv_P, 0.0, dv_rad_const);
	res.t_CSI = t_H;
	res.dv_CSI = dv_H;
	res.t_CDH = t_C;
	res.DV_CDH = V_ACF - V_AC;

	PZDKIELM.Block[0].SV_before[0].R = sv_AP.R;
	PZDKIELM.Block[0].SV_before[0].V = sv_AP.V;
	PZDKIELM.Block[0].SV_before[0].GMT = OrbMech::GETfromMJD(sv_AP.MJD, GMTBASE);
	if (sv_AP.gravref == hEarth)
	{
		PZDKIELM.Block[0].SV_before[0].RBI = BODY_EARTH;
	}
	else
	{
		PZDKIELM.Block[0].SV_before[0].RBI = BODY_MOON;
	}
	
	PZDKIELM.Block[0].V_after[0] = sv_AP.V + (V_APF - V_AP);
	PZDKIT.Block[0].Display[0].ManGMT = GMTfromGET(opt.t_TIG);
	PZDKIT.Block[0].Display[0].VEH = opt.ChaserID;
	PZDKIT.Block[0].Display[0].Man_ID = "NC";

	PZDKIT.Block[0].NumMan = 1;
	PZDKIT.NumSolutions = 1;

	return true;
}

void RTCC::PMMDKI(SPQOpt &opt, SPQResults &res)
{
	double t_CSI0, e_TPI, t_CSI, eps_TPI, c_TPI, e_TPIo, t_CSIo, p_TPI;
	int s_TPI, err;

	PZDKIT.UpdatingIndicator = true;

	p_TPI = c_TPI = 0.0;
	eps_TPI = 1.0;
	t_CSI = t_CSI0 = opt.t_CSI;

	do
	{
		opt.t_CSI = t_CSI;
		err = ConcentricRendezvousProcessor(opt, res);
		if (err)
		{
			//Time violation
			if (err == 3)
			{
				RTCCONLINEMON.TextBuffer[0] = "TIME";
				PMXSPT("PMMDKI", 92);
			}
			return;
		}

		if (!(opt.OptimumCSI && opt.K_CDH == 1 && opt.t_CSI > 0))
		{
			break;
		}

		e_TPI = res.t_TPI - opt.t_TPI;

		if (p_TPI == 0 || abs(e_TPI) >= eps_TPI)
		{
			OrbMech::ITER(c_TPI, s_TPI, e_TPI, p_TPI, t_CSI, e_TPIo, t_CSIo, 60.0);
		}
		//Iteration checking
		if (t_CSI > opt.t_CSI + 15.0*60.0)
		{
			//Warning message
			PMXSPT("PMMDKI", 101);
			t_CSI = opt.t_CSI + 15.0*60.0;
			break;
		}
		if (t_CSI < opt.t_CSI - 15.0*60.0)
		{
			//Warning message
			PMXSPT("PMMDKI", 101);
			t_CSI = opt.t_CSI - 15.0*60.0;
			break;
		}
		if (s_TPI == 1)
		{
			//Warning message
			PMXSPT("PMMDKI", 101);
			break;
		}
	} while (abs(e_TPI) >= eps_TPI);

	//Reset input CSI time
	opt.t_CSI = t_CSI0;

	//Plan is a SPQ
	PZDKIT.Block[0].PlanStatus = 2;

	//Save state vector and display data
	if (opt.t_CSI <= 0)
	{
		//Save CDH data
		PZDKIT.NumSolutions = 1;
		PZDKIT.Block[0].NumMan = 1;
		PZDKIELM.Block[0].SV_before[0] = ConvertSVtoEphemData(res.sv_C[0]);
		PZDKIELM.Block[0].V_after[0] = res.sv_C_apo[0].V;
		PZDKIT.Block[0].Display[0].Man_ID = "CDH";
		PZDKIT.Block[0].Display[0].VEH = opt.ChaserID;
	}
	else
	{
		//Save CSI and CDH data
		PZDKIT.NumSolutions = 1;
		PZDKIT.Block[0].NumMan = 2;

		PZDKIELM.Block[0].SV_before[0] = ConvertSVtoEphemData(res.sv_C[0]);
		PZDKIELM.Block[0].V_after[0] = res.sv_C_apo[0].V;
		PZDKIT.Block[0].Display[0].Man_ID = "CSI";
		PZDKIT.Block[0].Display[0].VEH = opt.ChaserID;

		PZDKIELM.Block[0].SV_before[1] = ConvertSVtoEphemData(res.sv_C[1]);
		PZDKIELM.Block[0].V_after[1] = res.sv_C_apo[1].V;
		PZDKIT.Block[0].Display[1].Man_ID = "CDH";
		PZDKIT.Block[0].Display[1].VEH = opt.ChaserID;

		//Calculate TPI and TPF maneuvers
		double t_TPI, t_TPF;
		err = PCTETR(res.sv_C_apo[1], res.sv_T[1], opt.GETbase, opt.WT, opt.E, t_TPI, t_TPF);

		if (err == 0)
		{
			//Coast to TPI
			res.sv_C[2] = coast(res.sv_C_apo[1], t_TPI - OrbMech::GETfromMJD(res.sv_C_apo[1].MJD, opt.GETbase));
			res.sv_T[2] = coast(res.sv_T[1], t_TPI - OrbMech::GETfromMJD(res.sv_T[1].MJD, opt.GETbase));

			//Call two-impulse processor
			LambertMan lam;
			TwoImpulseResuls lamres;

			lam.mode = 2;
			lam.GETbase = opt.GETbase;
			lam.T1 = t_TPI;
			lam.T2 = -1;
			lam.N = 0;
			lam.axis = RTCC_LAMBERT_MULTIAXIS;
			lam.Perturbation = RTCC_LAMBERT_PERTURBED;
			lam.sv_A = res.sv_C[2];
			lam.sv_P = res.sv_T[2];
			lam.TravelAngle = opt.WT;

			LambertTargeting(&lam, lamres);

			//Save post TPI state vector
			res.sv_C_apo[2] = res.sv_C[2];
			res.sv_C_apo[2].V = res.sv_C[2].V + lamres.dV;

			//Save TPI data
			PZDKIELM.Block[0].SV_before[2] = ConvertSVtoEphemData(res.sv_C[2]);
			PZDKIELM.Block[0].V_after[2] = res.sv_C_apo[2].V;
			PZDKIT.Block[0].Display[2].Man_ID = "TPI";
			PZDKIT.Block[0].Display[2].VEH = opt.ChaserID;
			PZDKIT.Block[0].NumMan++;

			//Coast to TPF
			res.sv_C[3] = coast(res.sv_C_apo[2], lamres.T2 - OrbMech::GETfromMJD(res.sv_C_apo[2].MJD, opt.GETbase));
			res.sv_T[3] = coast(res.sv_T[2], lamres.T2 - OrbMech::GETfromMJD(res.sv_T[2].MJD, opt.GETbase));

			//Save post TPF state vector
			res.sv_C_apo[3] = res.sv_C[3];
			res.sv_C_apo[3].V = res.sv_C[3].V + lamres.dV2;

			//Save TPF data
			PZDKIELM.Block[0].SV_before[3] = ConvertSVtoEphemData(res.sv_C[3]);
			PZDKIELM.Block[0].V_after[3] = res.sv_C_apo[3].V;
			PZDKIT.Block[0].Display[3].Man_ID = "TPF";
			PZDKIT.Block[0].Display[3].VEH = opt.ChaserID;
			PZDKIT.Block[0].NumMan++;
		}
	}

	//Calculate more display data
	SV sv_C, sv_T;
	MATRIX3 Q_Xx;
	double DH, Phase, HA, HP;
	for (int i = 0;i < PZDKIT.Block[0].NumMan;i++)
	{
		sv_C = ConvertEphemDatatoSV(PZDKIELM.Block[0].SV_before[i]);
		sv_C.V = PZDKIELM.Block[0].V_after[i];
		sv_T = res.sv_T[i];

		PCPICK(sv_C, sv_T, DH, Phase, HA, HP);

		PZDKIT.Block[0].Display[i].DH = DH;
		PZDKIT.Block[0].Display[i].PhaseAngle = Phase;
		PZDKIT.Block[0].Display[i].HA = HA;
		PZDKIT.Block[0].Display[i].HP = HP;
		PZDKIT.Block[0].Display[i].ManGMT = OrbMech::GETfromMJD(sv_C.MJD, GMTBASE);
		Q_Xx = OrbMech::LVLH_Matrix(PZDKIELM.Block[0].SV_before[i].R, PZDKIELM.Block[0].SV_before[i].V);
		PZDKIT.Block[0].Display[i].DV_LVLH = mul(Q_Xx, PZDKIELM.Block[0].V_after[i] - PZDKIELM.Block[0].SV_before[i].V);
		PZDKIT.Block[0].Display[i].dv = length(PZDKIT.Block[0].Display[i].DV_LVLH);
		PZDKIT.Block[0].Display[i].Yaw = atan2(PZDKIT.Block[0].Display[i].DV_LVLH.y, PZDKIT.Block[0].Display[i].DV_LVLH.x);
		PZDKIT.Block[0].Display[i].Pitch = atan2(-PZDKIT.Block[0].Display[i].DV_LVLH.z, sqrt(pow(PZDKIT.Block[0].Display[i].DV_LVLH.x, 2) + pow(PZDKIT.Block[0].Display[i].DV_LVLH.y, 2)));
	}

	//We are done updating
	PZDKIT.UpdatingIndicator = false;

	//Recalculate display
	PMDRET();
}

void RTCC::PCPICK(SV sv_C, SV sv_T, double &DH, double &Phase, double &HA, double &HP)
{
	SV sv_TC;
	double mu, RA, RP, R_E, DT;
	Phase = OrbMech::PHSANG(sv_T.R, sv_T.V, sv_C.R);
	if (sv_C.gravref == hEarth)
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
		DT = 15.0*60.0;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = MCSMLR;
		DT = 20.0*60.0;
	}
	sv_TC = OrbMech::PositionMatch(sv_T, sv_C, mu);
	DH = length(sv_TC.R) - length(sv_C.R);
	SV sv_CC[3];
	sv_CC[0] = sv_C;
	sv_CC[1] = coast(sv_CC[0], DT);
	sv_CC[2] = coast(sv_CC[1], DT);
	VECTOR3 R_equ, V_equ;
	CELEMENTS elem;
	double R[3], U[3];
	for (int i = 0;i < 3;i++)
	{
		R[i] = length(sv_CC[i].R);
		if (sv_C.gravref == hEarth)
		{
			OrbMech::EclipticToECI(sv_CC[i].R, sv_CC[i].V, sv_CC[i].MJD, R_equ, V_equ);
		}
		else
		{
			OrbMech::EclipticToMCI(sv_CC[i].R, sv_CC[i].V, sv_CC[i].MJD, R_equ, V_equ);
		}
		elem = OrbMech::GIMIKC(R_equ, V_equ, mu);
		U[i] = OrbMech::MeanToTrueAnomaly(elem.l, elem.e) + elem.g;
		U[i] = fmod(U[i], PI2);
		if (U[i] < 0)
			U[i] += PI2;
	}
	PCHAPE(R[0], R[1], R[2], U[0], U[1], U[2], RA, RP);
	HA = RA - R_E;
	HP = RP - R_E;
}

void RTCC::PCHAPE(double R1, double R2, double R3, double U1, double U2, double U3, double &RAP, double &RPE)
{
	double alpha, beta, gamma, delta, arg1, arg2, U0, XR, RR;

	alpha = R1 - R3;
	delta = R1 - R2;
	arg1 = sin(U1) - sin(U2) - delta / alpha * (sin(U1) - sin(U3));
	arg2 = delta / alpha * (cos(U3) - cos(U1)) - (cos(U2) - cos(U1));
	U0 = atan2(arg1, arg2);
	beta = sin(U1 - U0);
	gamma = sin(U3 - U0);
	XR = alpha / (beta - gamma);
	RR = (R1 + R3 - XR * (beta + gamma)) / 2.0;
	XR = abs(XR);
	RAP = RR + XR;
	RPE = RR - XR;
}

void RTCC::PMMPNE(AEGBlock sv_C, AEGBlock sv_T, double TREF, double FNPC, int KPC, int IPC, AEGBlock &SAVE, double &DI1, double &DH1)
{
	/*AEGBlock *sv_PC, *sv_NOPC;
	double TA, TB, U_L, U_U;
	int ICT;

	if (abs(KPC) == 2)
	{
		goto RTCC_PMMPNE_2_1;
	}

	ICT = 0;

	//SPQ
	if (KPC < 1)
	{
		TB = TREF + (FNPC - 1.0)*PI / sv_C.Data.l_dot;
		sv_C.Data.TIMA = 0;
		sv_C.Data.TE = TB;
		PMMAEGS(sv_C.Header, sv_C.Data, sv_C.Data);
		sv_T.Data.TIMA = 6;
		PMMAEGS(sv_T.Header, sv_T.Data, sv_T.Data);
		TA = sv_T.Data.TE - sv_C.Data.TE;
	}
	//DKI
	else
	{
		//TBD
	}
	//Target does PC
	if (IPC < 0)
	{
		SAVE = sv_C;
		U_L = sv_C.Data.U;
		sv_PC = &sv_T;
		sv_NOPC = &sv_C;
	}
	else
	{
		SAVE = sv_T;
		U_L = sv_T.Data.U;
		sv_PC = &sv_C;
		sv_NOPC = &sv_T;
	}
	U_U = U_L + PI;
	if (U_U >= PI2)
	{
		U_U -= PI2;
	}
RTCC_PMMPNE_2_1:
	if (abs(KPC) == 3)
	{
		goto RTCC_PMMPNE_3_1;
	}
	double DP, DI, DH;
	DP = PI / sv_NOPC->Data.l_dot - PI2 / sv_PC->Data.l_dot;
	DI = sv_NOPC->Data.coe_mean.i - sv_PC->Data.coe_mean.i;
	DH = sv_NOPC->Data.coe_mean.h - sv_PC->Data.coe_mean.h;
	if (abs(DP) >= 1.0)
	{
		DH += PI2 / sv_NOPC->Data.l_dot*TA*(sv_NOPC->Data.h_dot - sv_PC->Data.h_dot) / DP;
	}
	if (abs(KPC) == 2)
	{
		DI1 += DI;
		DH1 += DH;
		return;
	}
	DI1 = DI;
	DH1 = DH;
RTCC_PMMPNE_3_1:
	double i_apo, h_apo, g_apo;
	if (IPC < 0)
	{
		i_apo = sv_T.Data.coe_osc.i;
		h_apo = sv_T.Data.coe_osc.h;
		g_apo = sv_T.Data.coe_osc.g;
		sv_C = sv_T;
	}
	else
	{
		i_apo = sv_C.Data.coe_osc.i;
		h_apo = sv_C.Data.coe_osc.h;
		g_apo = sv_C.Data.coe_osc.g;
		sv_T = sv_C;
	}
	double i_PH, g_PH, h_PH;
	i_PH = i_apo + DI;
	h_PH = h_apo + DH;
	if (h_PH >= PI2)
	{
		h_PH -= PI2;
	}
	g_PH = g_apo - 2.0*atan(tan(0.5*DH*sin((PI - i_PH + i_apo) / 2.0)) / sin(PI - i_PH + i_apo) / 2.0);
	if (g_PH >= PI2)
	{
		g_PH -= PI2;
	}
	else if (g_PH < 0)
	{
		g_PH += PI2;
	}
	double cos_dw, DEN, U_CN, U_CN_apo, dw, DV_PC;
RTCC_PMMPNE_3_2:
	cos_dw = cos(i_PH)*cos(sv_PC->Data.coe_osc.i) + sin(i_PH)*sin(sv_PC->Data.coe_osc.i)*cos(sv_PC->Data.coe_osc.h - h_PH);
	DEN = cos_dw * cos(sv_PC->Data.coe_osc.i) - cos(i_PH);
	if (h_PH < sv_PC->Data.coe_osc.h)
	{
		DEN = -DEN;
	}
	U_CN = atan2(sin(i_PH)*sin(sv_PC->Data.coe_osc.i)*sin(abs(sv_PC->Data.coe_osc.h - h_PH)), DEN);
	if ((U_L > PI && U_U <= U_CN) || (U_L <= PI && U_U > U_CN))
	{
		U_CN += PI;
	}
	if (ICT != 0)
	{
		goto RTCC_PMMPNE_5_2;
	}
RTCC_PMMPNE_4_2:
	ICT++;
	if (ICT > 3)
	{
		goto RTCC_PMMPNE_6_1;
	}
	U_CN_apo = U_CN;
	sv_PC->Data.TIMA = 2;
	sv_PC->Data.Item8 = U_CN;
	PMMAEGS(sv_PC->Header, sv_PC->Data, sv_PC->Data);
	//TBD
	goto RTCC_PMMPNE_3_2;
RTCC_PMMPNE_5_1:
	if (abs(U_CN - U_CN_apo) > PI05)
	{
		if (abs(U_CN - U_CN_apo) <= PI)
		{
			if (abs(U_CN - U_C) <= PI)
			{
				if (abs(U_CN - U_C) >= PI05)
				{
					U_CN += PI;
					if (U_CN >= PI2)
					{
						U_CN -= PI2;
					}
				}
			}
		}
	}
	goto RTCC_PMMPNE_4_2;
RTCC_PMMPNE_5_2:
	dw = acos(cos_dw);
	DV_PC = 2.0*sqrt(mu)*sin(dw / 2.0)*sqrt(2.0/sv_C.Data.R-1.0/ sv_C.Data.coe_osc.a);
	if (ICT != 1)
	{
		goto RTCC_PMMPNE_5_1;
	}
	if (DV_PC > 5.0*0.3048)
	{
		goto RTCC_PMMPNE_5_1;
	}
RTCC_PMMPNE_6_1:
	if (KPC > 0)
	{
		goto RTCC_PMMPNE_8_1;
	}
	double T_NPC, S, DV_Z, DV_H;
	T_NPC = sv_C.Data.TE;
	VECTOR3 R_C, V_C, R_P, V_P, H_P, H_C, K;
	OrbMech::GIMKIC(sv_C.Data.coe_osc, mu, R_C, V_C);
	OrbMech::GIMKIC(sv_PH.Data.coe_osc, mu, R_P, V_P);
	H_P = unit(crossp(R_P, V_P));
	H_C = unit(crossp(R_C, V_C));
	K = unit(crossp(H_P, R_C));
	S = dotp(H_C, K);
	DV_Z = S * DV_PC*cos(0.5*dw) / abs(S);
	DV_H = -DV_PC * sin(0.5*dw);
	//TBD: Output
	sv_T = SAVE;
	return;
RTCC_PMMPNE_8_1:
	//TBD
	return;*/
}

void RTCC::PCMVMR(VECTOR3 R_C, VECTOR3 V_C, VECTOR3 R_T, VECTOR3 V_T, double DELVX, double DELVY, double DELVZ, int I, VECTOR3 &V_C_apo, double &Pitch, double &Yaw)
{
	//I = 0: Pitch and yaw parallel to target, -1 = apply DV in-plane, 1 = apply DV parallel
	VECTOR3 H1, H2, J2, K1, K2;
	double DV_H, DV_R, DV_Z;

	DV_H = DELVX;
	DV_R = DELVY;
	DV_Z = DELVZ;

	H2 = unit(crossp(R_C, V_C));
	H1 = unit(crossp(R_T, V_T));
	K2 = unit(crossp(H2, R_C));
	K1 = unit(crossp(H1, R_C));
	J2 = unit(crossp(K2, H2));
	if (I >= 0)
	{
		double S = dotp(H2, K1);
		double ABP = S / abs(S)*acos(dotp(unit(K1), unit(K2)));
		DV_Z = DV_H * tan(ABP);
		Yaw = -ABP;
		Pitch = -atan2(DV_R, DV_H / cos(ABP));
		if (I <= 0)
		{
			return;
		}
	}
	V_C_apo = V_C + K2 * DV_H - J2 * DV_R + H2 * DV_Z;
}

int RTCC::PCTETR(SV sv_C, SV sv_T, double GETBase, double WT, double ESP, double &TESP, double &TR)
{
	SV sv_C1, sv_T1;
	double QL, DU, mu, C1, C2, C3, C4, r_T, r_C, QLI, DT, eps_dt, L_C_dot, L_T_dot;
	int I;
	bool err;

	eps_dt = 0.1;
	I = 1;
	sv_C1 = sv_C;
	sv_T1 = sv_T;
	if (sv_C1.gravref == hEarth)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}
	do
	{
		sv_T1 = OrbMech::PMMAEGS(sv_T1, 0, sv_C1.MJD, err);
		if (err)
		{
			return -1;
		}
		QL = OrbMech::PHSANG(sv_T1.R, sv_T1.V, sv_C1.R);
		if (I == 1)
		{
			L_C_dot = OrbMech::GetMeanMotion(sv_C1.R, sv_C1.V, mu);
			L_T_dot = OrbMech::GetMeanMotion(sv_T1.R, sv_T1.V, mu);
			DU = L_C_dot - L_T_dot;
			if (DU*QL < 0)
			{
				QL += PI2;
			}
		}
		r_C = length(sv_C1.R) / OrbMech::R_Earth;
		r_T = length(sv_T1.R) / OrbMech::R_Earth;
		C1 = -pow(r_T / cos(ESP), 2);
		C2 = 2.0*r_T*r_C;
		C3 = -C1 - r_T * r_T - r_C * r_C;
		C4 = C2 * C2 - 4.0*C1*C3;
		if (C4 < 0)
		{
			return -1;
		}
		QLI = acos((-C2 - sqrt(C4)) / (2.0*C1));
		if (r_C > r_T)
		{
			QLI = -QLI;
		}
		DT = (QL - QLI) / DU;
		if (abs(DT) < eps_dt)
		{
			TESP = OrbMech::GETfromMJD(sv_C1.MJD, GETBase);
			TR = TESP + WT / L_T_dot;
			return 0;
		}
		sv_C1 = coast(sv_C1, DT);
		I++;
	} while (I <= 10);
	return -1;
}

//Module PMMSPQ
int RTCC::ConcentricRendezvousProcessor(const SPQOpt &opt, SPQResults &res)
{
	SV sv_C_CSI, sv_T_CSI, sv_C_CDH, sv_C_TPI, sv_T_CDH, sv_C_CSI_apo, sv_T_TPI, sv_C_CDH_apo, sv_PC;
	CELEMENTS elem;
	MATRIX3 Q_Xx;
	VECTOR3 R_equ, V_equ, R_TJ, V_TJ, R_AF, R_AFD;
	double dv_CSI, mu, t_CDH, t_TPI, DH, p_C, c_C, e_C, e_Co, dv_CSIo, V_Cb, V_CRb, gamma_C, V_CHb, a_T, a_C, r_T_dot, r_C_dot, T_TPF;
	double V_C_apo, gamma_C_apo, V_CHa, DV_H, DV_R, Pitch, Yaw;
	int s_C, PCMVMR_IND;
	bool err;

	//Set up variables for the CSI iteration
	p_C = c_C = 0.0;
	s_C = 0;

	//In which orbit are we?
	if (opt.sv_A.gravref == hEarth)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	//Calculate the DV parallel to the target orbit if desired, and only if no plane change maneuver is scheduled
	if (opt.N_PC == false && opt.ParallelDVInd)
	{
		PCMVMR_IND = 1;
	}
	else
	{
		PCMVMR_IND = -1;
	}

	//TPI Definition
	if (opt.K_CDH == 0)
	{
		//TBD: Environment change and longitude crossing logic
		t_TPI = opt.t_TPI;
	}

	//CSI scheduled?
	if (opt.t_CSI > 0)
	{
		sv_C_CSI = coast(opt.sv_A, opt.t_CSI - OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase));
		sv_T_CSI = coast(opt.sv_P, opt.t_CSI - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));

		//Initial guess for CSI DV from GSOP
		VECTOR3 H1 = unit(crossp(crossp(sv_C_CSI.R, sv_C_CSI.V), sv_C_CSI.R));
		dv_CSI = sqrt(2.0*mu*length(sv_T_CSI.R)*1.0 / (length(sv_C_CSI.R)*(length(sv_T_CSI.R) + length(sv_C_CSI.R)))) - dotp(sv_C_CSI.V, H1);

		//Propagate target to TPI
		if (opt.K_CDH == 0)
		{
			sv_T_TPI = coast(sv_T_CSI, t_TPI - OrbMech::GETfromMJD(sv_T_CSI.MJD, opt.GETbase));
		}
	}
	else
	{
		sv_C_CDH = coast(opt.sv_A, opt.t_CDH - OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase));
		sv_T_CDH = coast(opt.sv_P, opt.t_CDH - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));
	}

	//Here we return to the beginning of the loop
RTCC_PMMSPQ_A:
	if (opt.t_CSI > 0)
	{
		//CSI only calculations
		sv_C_CSI_apo = sv_C_CSI;
		//Apply maneuver
		PCMVMR(sv_C_CSI.R, sv_C_CSI.V, sv_T_CSI.R, sv_T_CSI.V, dv_CSI, 0.0, 0.0, PCMVMR_IND, sv_C_CSI_apo.V, Pitch, Yaw);

		//CDH on time
		if (opt.I_CDH == 2)
		{
			t_CDH = opt.t_CDH;
			sv_C_CDH = coast(sv_C_CSI_apo, t_CDH - OrbMech::GETfromMJD(sv_C_CSI_apo.MJD, opt.GETbase));
		}
		else if (opt.I_CDH == 3)
		{
			double u_CSI, u_CDH, DN = 0;
			//Calculate argument of latitude at CSI
			if (opt.sv_A.gravref == hEarth)
			{
				OrbMech::EclipticToECI(sv_C_CSI_apo.R, sv_C_CSI_apo.V, sv_C_CSI_apo.MJD, R_equ, V_equ);
			}
			else
			{
				OrbMech::EclipticToMCI(sv_C_CSI_apo.R, sv_C_CSI_apo.V, sv_C_CSI_apo.MJD, R_equ, V_equ);
			}
			elem = OrbMech::GIMIKC(R_equ, V_equ, mu);
			u_CSI = OrbMech::MeanToTrueAnomaly(elem.l, elem.e) + elem.g;
			u_CSI = fmod(u_CSI, PI2);
			if (u_CSI < 0)
				u_CSI += PI2;
			u_CDH = u_CSI + opt.DU_D;
			while (u_CDH > PI2)
			{
				u_CDH -= PI2;
				DN += 1.0;
			}
			//Propagate to CDH
			sv_C_CDH = OrbMech::PMMAEGS(sv_C_CSI_apo, 2, u_CDH, err, DN);
			t_CDH = OrbMech::GETfromMJD(sv_C_CDH.MJD, opt.GETbase);
		}
		//CDH at next apsis
		else
		{
			//TBD
		}
		sv_T_CDH = coast(sv_T_CSI, t_CDH - OrbMech::GETfromMJD(sv_T_CSI.MJD, opt.GETbase));
	}

	//Target to position (phase) match
	sv_PC = OrbMech::PositionMatch(sv_T_CDH, sv_C_CDH, mu);
	//Calculate delta height
	DH = length(sv_PC.R) - length(sv_C_CDH.R);

	//For iteration on DH
	if (opt.t_CSI > 0 && opt.K_CDH == 1)
	{
		e_C = DH - opt.DH;
		if (abs(e_C) > 1.0)
		{
			OrbMech::ITER(c_C, s_C, e_C, p_C, dv_CSI, e_Co, dv_CSIo);
			goto RTCC_PMMSPQ_A;
		}
	}

	//CDH calculation
	V_Cb = length(sv_C_CDH.V);
	V_CRb = dotp(sv_C_CDH.R, sv_C_CDH.V) / length(sv_C_CDH.R);
	gamma_C = asin(V_CRb / V_Cb);
	V_CHb = V_Cb * cos(gamma_C);
	a_T = 1.0 / (2.0 / length(sv_PC.R) - dotp(sv_PC.V, sv_PC.V) / mu);
	a_C = a_T - DH;
	r_T_dot = dotp(sv_PC.R, sv_PC.V) / length(sv_PC.R);
	r_C_dot = r_T_dot * pow(a_T / a_C, 1.5);
	//Velocity after the maneuver
	V_C_apo = sqrt(mu*(2.0 / length(sv_C_CDH.R) - 1.0 / a_C));
	//Flight path angle after maneuver
	gamma_C_apo = asin(r_C_dot / V_C_apo);
	//Horizontal velocity after maneuver
	V_CHa = V_C_apo * cos(gamma_C_apo);
	DV_H = V_CHa - V_CHb;
	DV_R = V_CRb - r_C_dot;

	sv_C_CDH_apo = sv_C_CDH;
	PCMVMR(sv_C_CDH.R, sv_C_CDH.V, sv_T_CDH.R, sv_T_CDH.V, DV_H, DV_R, 0.0, PCMVMR_IND, sv_C_CDH_apo.V, Pitch, Yaw);

	if (opt.t_CSI <= 0)
	{
		//Final CDH comps
		res.DH = DH;
		Q_Xx = OrbMech::LVLH_Matrix(sv_C_CDH.R, sv_C_CDH.V);
		res.dV_CDH = mul(Q_Xx, sv_C_CDH_apo.V - sv_C_CDH.V);
		res.t_CDH = opt.t_CDH;
		res.t_TPI = 0.0;

		res.sv_C[0] = sv_C_CDH;
		res.sv_C_apo[0] = sv_C_CDH_apo;
		res.sv_T[0] = sv_T_CDH;
		
		return 0;
	}

	//Iteration on TPI time
	if (opt.K_CDH == 0)
	{
		sv_C_TPI = coast(sv_C_CDH_apo, t_TPI - t_CDH);
		OrbMech::QDRTPI(sv_T_TPI.R, sv_T_TPI.V, sv_T_TPI.MJD, sv_T_TPI.gravref, mu, DH, opt.E, 1, R_TJ, V_TJ);
		R_AFD = R_TJ - unit(R_TJ)*DH;
		R_AF = OrbMech::PROJCT(R_TJ, V_TJ, sv_C_TPI.R);

		e_C = OrbMech::sign(dotp(crossp(R_AF, R_AFD), unit(crossp(R_TJ, V_TJ))))*acos(dotp(R_AFD / length(R_AFD), R_AF / length(R_AF)));
		if (abs(e_C) > 0.000004)
		{
			OrbMech::ITER(c_C, s_C, e_C, p_C, dv_CSI, e_Co, dv_CSIo);
			goto RTCC_PMMSPQ_A;
		}
	}
	else
	{
		//If iteration on DH was used, get TPI time now
		if (PCTETR(sv_C_CDH_apo, sv_T_CDH, opt.GETbase, opt.WT, opt.E, t_TPI, T_TPF))
		{
			return 94;
		}
	}

	if (t_CDH - opt.t_CSI < 10.0*60.0)
	{
		return 3;
	}
	if (t_TPI - t_CDH < 10.0*60.0)
	{
		return 3;
	}

	res.t_CSI = opt.t_CSI;
	res.dV_CSI = _V(dv_CSI, 0, 0);
	res.t_CDH = t_CDH;
	Q_Xx = OrbMech::LVLH_Matrix(sv_C_CDH.R, sv_C_CDH.V);
	res.dV_CDH = mul(Q_Xx, sv_C_CDH_apo.V - sv_C_CDH.V);
	res.t_TPI = t_TPI;
	res.DH = DH;

	//Save state vectors
	res.sv_C[0] = sv_C_CSI;
	res.sv_C_apo[0] = sv_C_CSI_apo;
	res.sv_T[0] = sv_T_CSI;
	res.sv_C[1] = sv_C_CDH;
	res.sv_C_apo[1] = sv_C_CDH_apo;
	res.sv_T[1] = sv_T_CDH;

	return 0;
}

VECTOR3 RTCC::HatchOpenThermalControl(VESSEL *v, MATRIX3 REFSMMAT)
{
	MATRIX3 SMNB, MY, MX;
	VECTOR3 R_SC, UX, UY, UZ;
	OBJHANDLE hSun;

	hSun = oapiGetObjectByName("Sun");
	v->GetRelativePos(hSun, R_SC);
	R_SC = _V(R_SC.x, R_SC.z, R_SC.y);
	UZ = unit(R_SC);
	UY = unit(crossp(UZ, _V(0.0, 0.0, -1.0)));
	UX = crossp(UY, UZ);

	SMNB = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	MY = OrbMech::_MRy(-15.0*RAD);
	MX = OrbMech::_MRx(-80.0*RAD);
	SMNB = mul(MX, mul(MY, SMNB));
	return OrbMech::CALCGAR(REFSMMAT, SMNB);
}

void RTCC::DockingAlignmentProcessor(DockAlignOpt &opt)
{
	//Option 1: LM REFSMMAT from CSM REFSMMAT, CSM attitude, docking angle and LM gimbal angles
	//Option 2: LM gimbal angles from CSM REFSMMAT, LM REFSMMAT, CSM gimbal angles and docking angle
	//Option 3: CSM gimbal angles from CSM REFSMMAT, LM REFSMMAT, LM gimbal angles and docking angle
	//Coordinate Systems:
	//Navigation Base (NB)
	//Stable Member (SM)
	//Basic Reference Coordinate System (BRCS)
	//REFSMMAT is BRCS to SM

	MATRIX3 M_NBCSM_NBLM;

	M_NBCSM_NBLM = OrbMech::CSMBodyToLMBody(opt.DockingAngle);

	if (opt.type == 1)
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_NBLM, M_BRCS_NBCSM, M_BRCS_NBLM, M_BRCS_SMLM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(opt.CSMAngles);
		M_SMLM_NBLM = OrbMech::CALCSMSC(opt.LMAngles);
		M_BRCS_NBCSM = mul(M_SMCSM_NBCSM, opt.CSM_REFSMMAT);
		M_BRCS_NBLM = mul(M_NBCSM_NBLM, M_BRCS_NBCSM);
		M_BRCS_SMLM = mul(OrbMech::tmat(M_SMLM_NBLM), M_BRCS_NBLM);

		opt.LM_REFSMMAT = M_BRCS_SMLM;
	}
	else if (opt.type == 2)
	{
		MATRIX3 M_SMCSM_NBCSM, M_SMLM_SMCSM, M_SMCSM_NBLM, M_SMLM_NBLM;

		M_SMCSM_NBCSM = OrbMech::CALCSMSC(opt.CSMAngles);
		M_SMLM_SMCSM = mul(opt.CSM_REFSMMAT, OrbMech::tmat(opt.LM_REFSMMAT));
		M_SMCSM_NBLM = mul(M_NBCSM_NBLM, M_SMCSM_NBCSM);
		M_SMLM_NBLM = mul(M_SMCSM_NBLM, M_SMLM_SMCSM);

		opt.LMAngles = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), M_SMLM_NBLM);
	}
	else if (opt.type == 3)
	{
		MATRIX3 M_SMLM_NBLM, M_SMCSM_NBCSM, M_SMCSM_SMLM, M_SMCSM_NBLM;

		M_SMLM_NBLM = OrbMech::CALCSMSC(opt.LMAngles);
		M_SMCSM_SMLM = mul(opt.LM_REFSMMAT, OrbMech::tmat(opt.CSM_REFSMMAT));
		M_SMCSM_NBLM = mul(M_SMLM_NBLM, M_SMCSM_SMLM);
		M_SMCSM_NBCSM = mul(OrbMech::tmat(M_NBCSM_NBLM), M_SMCSM_NBLM);

		opt.CSMAngles = OrbMech::CALCGAR(_M(1, 0, 0, 0, 1, 0, 0, 0, 1), M_SMCSM_NBCSM);
	}
}

bool RTCC::PMMAPD(SV sv0, SV &sv_a, SV &sv_p)
{
	OELEMENTS coe;
	double mu;
	bool lowecclogic, error2;
	
	mu = GGRAV * oapiGetMass(sv0.gravref);
	coe = OrbMech::coe_from_sv(sv0.R, sv0.V, mu);

	if (coe.e > 0.005)
	{
		lowecclogic = false;
	}
	else
	{
		lowecclogic = true;
	}

	if (lowecclogic == false)
	{
		sv_p = OrbMech::PMMAEGS(sv0, 1, 0.0, error2);
		if (error2)
		{
			return true;
		}
		sv_a = OrbMech::PMMAEGS(sv0, 1, PI, error2);
		if (error2)
		{
			return true;
		}
	}
	else
	{
		SV sv1, sv2;
		double u_x, u_y;

		//First guess
		ApsidesArgumentofLatitudeDetermination(sv0, u_x, u_y);

		sv1 = OrbMech::PMMAEGS(sv0, 2, u_x, error2);
		if (error2)
		{
			return true;
		}
		sv2 = OrbMech::PMMAEGS(sv0, 2, u_y, error2);
		if (error2)
		{
			return true;
		}

		if (length(sv1.R) > length(sv2.R))
		{
			sv_a = sv1;
			sv_p = sv2;
		}
		else
		{
			sv_p = sv1;
			sv_a = sv2;
		}
	}
	return false;
}

VECTOR3 RTCC::HeightManeuverInteg(SV sv0, double dh)
{
	OELEMENTS coe;
	SV sv0_apo, sv1;
	MATRIX3 Rot;
	VECTOR3 R1_equ, V1_equ, am;
	double r_D, dv_H, e_H, eps, p_H, c_I, e_Ho, dv_Ho, u_b, mu, u_d;
	int s_F, body;
	bool error2;

	sv0_apo = sv0;

	if (sv0.gravref == oapiGetObjectByName("Earth"))
	{
		mu = OrbMech::mu_Earth;
		body = BODY_EARTH;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		body = BODY_MOON;
	}

	Rot = OrbMech::GetObliquityMatrix(body, sv0.MJD);
	R1_equ = rhtmul(Rot, sv0.R);
	V1_equ = rhtmul(Rot, sv0.V);
	coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
	u_b = fmod(coe.TA + coe.w, PI2);
	u_d = fmod(u_b + PI, PI2);
	sv1 = OrbMech::PMMAEGS(sv0, 2, u_d, error2);

	r_D = length(sv1.R) + dh;
	am = unit(crossp(sv0.R, sv0.V));
	dv_H = 0.0;
	eps = 1.0;
	p_H = c_I = 0.0;
	s_F = 0;

	do
	{
		sv0_apo.V = sv0.V + unit(crossp(am, sv0.R))*dv_H;

		V1_equ = rhtmul(Rot, sv0_apo.V);
		coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
		u_b = fmod(coe.TA + coe.w, PI2);
		u_d = fmod(u_b + PI, PI2);
		sv1 = OrbMech::PMMAEGS(sv0_apo, 2, u_d, error2);
		e_H = length(sv1.R) - r_D;

		if (p_H == 0 || abs(e_H) >= eps)
		{
			OrbMech::ITER(c_I, s_F, e_H, p_H, dv_H, e_Ho, dv_Ho);
			if (s_F == 1)
			{
				return _V(0, 0, 0);
			}
		}
	} while (abs(e_H) >= eps);

	return sv0_apo.V - sv0.V;
}

VECTOR3 RTCC::ApoapsisPeriapsisChangeInteg(SV sv0, double r_AD, double r_PD)
{
	OELEMENTS coe_bef, coe_aft, coe_p, coe_a;
	SV sv0_apo, sv_a, sv_p;
	double r_a_b, r_p_b, a_ap, a_p, dr_a_max, dr_p_max, a_D, e_D, mu, r_b, cos_theta_A, theta_A, r_a, r_p, a_a, cos_E_a, E_a, u_b, r_a_a, r_p_a;
	double dr_ap0, dr_p0, dr_ap_c, dr_p_c, dr_ap1, dr_p1, ddr_ap, ddr_p, eps;
	int n, nmax;

	mu = GGRAV * oapiGetMass(sv0.gravref);
	sv0_apo = sv0;
	n = 0;
	nmax = 100;
	eps = 1.0;

	coe_bef = OrbMech::coe_from_sv(sv0.R, sv0.V, mu);
	u_b = fmod(coe_bef.TA + coe_bef.w, PI2);
	PMMAPD(sv0, sv_a, sv_p);

	if (r_AD == r_PD)
	{
		r_PD = r_PD - 0.1*1852.0;
	}

	r_b = length(sv0.R);

	if (r_AD < r_b)
	{
		r_AD = r_b;
	}
	if (r_PD > r_b)
	{
		r_PD = r_b;
	}

	r_a_b = length(sv_a.R);
	r_p_b = length(sv_p.R);
	coe_p = OrbMech::coe_from_sv(sv_p.R, sv_p.V, mu);
	a_p = coe_p.h*coe_p.h / (mu*(1.0 - coe_p.e * coe_p.e));
	coe_a = OrbMech::coe_from_sv(sv_a.R, sv_a.V, mu);
	a_ap = coe_a.h*coe_a.h / (mu*(1.0 - coe_a.e * coe_a.e));

	dr_a_max = (a_p*(1.0 + coe_p.e) - r_a_b)*(r_AD / r_a_b);
	dr_p_max = (a_ap*(1.0 - coe_a.e) - r_p_b)*(r_PD / r_p_b);
	a_D = (r_AD + r_PD) / 2.0;
	e_D = abs((a_D - r_PD) / a_D);
	cos_theta_A = (a_D*(1.0 - e_D * e_D) - r_b) / (e_D*r_b);
	if (abs(cos_theta_A) > 1.0)
	{
		if (cos_theta_A < 0)
		{
			cos_theta_A = -1.0;
		}
		else
		{
			cos_theta_A = 1.0;
		}
	}
	theta_A = acos(cos_theta_A);
	r_a = r_AD + (PI - theta_A) / PI * dr_a_max;
	r_p = r_PD + theta_A / PI * dr_p_max;

	do
	{
		a_a = (r_a + r_p) / 2.0;
		coe_aft.e = abs((a_a - r_p) / a_a);
		cos_E_a = (a_a - r_b) / (a_a*coe_aft.e);

		if (abs(cos_E_a) > 1.0)
		{
			if (cos_E_a < 0)
			{
				cos_E_a = -1.0;
			}
			else
			{
				cos_E_a = 1.0;
			}
			a_a = r_b / (1.0 - coe_aft.e * cos_E_a);
		}

		E_a = acos(cos_E_a);
		coe_aft.TA = acos2((a_a*(1.0 - coe_aft.e * coe_aft.e) - r_b) / (coe_aft.e*r_b));
		if (coe_bef.TA > PI)
		{
			E_a = PI2 - E_a;
			coe_aft.TA = PI2 - coe_aft.TA;
		}
		//l_a = E_a - coe_aft.e * sin(E_a);
		coe_aft.w = u_b - coe_aft.TA;
		coe_aft.i = coe_bef.i;
		coe_aft.RA = coe_bef.RA;
		coe_aft.h = sqrt(a_a*mu*(1.0 - coe_aft.e*coe_aft.e));
		if (coe_aft.w < 0)
		{
			coe_aft.w += PI2;
		}
		OrbMech::sv_from_coe(coe_aft, mu, sv0_apo.R, sv0_apo.V);
		PMMAPD(sv0_apo, sv_a, sv_p);
		r_a_a = length(sv_a.R);
		r_p_a = length(sv_p.R);

		if (n == 0)
		{
			dr_ap0 = r_AD - r_a_a;
			dr_p0 = r_PD - r_p_a;
			dr_ap_c = dr_ap0;
			dr_p_c = dr_p0;
		}
		else
		{
			dr_ap1 = r_AD - r_a_a;
			dr_p1 = r_PD - r_p_a;
			ddr_ap = dr_ap0 - dr_ap1;
			ddr_p = dr_p0 - dr_p1;
			dr_ap_c = dr_ap1 * dr_ap_c / ddr_ap;
			dr_p_c = dr_p1 * dr_p_c / ddr_p;
			dr_ap0 = dr_ap1;
			dr_p0 = dr_p1;
		}

		r_a = r_a + dr_ap_c;
		r_p = r_p + dr_p_c;

		n++;
		
	} while (n < nmax && (abs(r_AD - r_a_a) > eps || abs(r_PD - r_p_a) > eps));

	return sv0_apo.V - sv0.V;
}

void RTCC::ApsidesArgumentofLatitudeDetermination(SV sv0, double &u_x, double &u_y)
{
	OELEMENTS coe;
	SV sv[3];
	MATRIX3 obl;
	VECTOR3 R1_equ, V1_equ;
	double u[3], r[3], gamma, u_0, mu;
	int body;

	if (sv0.gravref == oapiGetObjectByName("Earth"))
	{
		mu = OrbMech::mu_Earth;
		body = BODY_EARTH;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		body = BODY_MOON;
	}

	sv[0] = sv0;
	sv[1] = coast(sv[0], 15.0*60.0);
	sv[2] = coast(sv[1], 15.0*60.0);

	for (int i = 0;i < 3;i++)
	{
		obl = OrbMech::GetObliquityMatrix(body, sv[i].MJD);
		R1_equ = rhtmul(obl, sv[i].R);
		V1_equ = rhtmul(obl, sv[i].V);
		coe = OrbMech::coe_from_sv(R1_equ, V1_equ, mu);
		u[i] = fmod(coe.TA + coe.w, PI2);
		r[i] = length(R1_equ);
	}

	gamma = (r[0] - r[1]) / (r[0] - r[2]);
	u_0 = atan2(sin(u[0]) - sin(u[1]) - gamma * (sin(u[0]) - sin(u[2])), gamma*(cos(u[2]) - cos(u[0])) - cos(u[1]) + cos(u[0]));
	if (u_0 < 0)
	{
		u_0 += PI2;
	}

	u_x = u_0 + PI05;
	u_y = u_0 - PI05;

	if (u_x < 0)
	{
		u_x += PI2;
	}
	if (u_y < 0)
	{
		u_y += PI2;
	}

}

VECTOR3 RTCC::CircularizationManeuverInteg(SV sv0)
{
	SV sv0_apo, sv1;
	OELEMENTS coe;
	MATRIX3 Rot, Q_Xx;
	VECTOR3 R0_equ, V0_equ, V_LVLH, DV1, DV2;
	double u_b, u_d, dh, mu;
	int body;
	bool error2;

	if (sv0.gravref == oapiGetObjectByName("Earth"))
	{
		mu = OrbMech::mu_Earth;
		body = BODY_EARTH;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		body = BODY_MOON;
	}

	sv0_apo = sv0;

	Q_Xx = OrbMech::LVLH_Matrix(sv0_apo.R, sv0_apo.V);
	V_LVLH = mul(Q_Xx, sv0_apo.V);
	V_LVLH.z = 0.0;
	sv0_apo.V = tmul(Q_Xx, V_LVLH);
	DV1 = sv0_apo.V - sv0.V;

	Rot = OrbMech::GetObliquityMatrix(body, sv0_apo.MJD);
	R0_equ = rhtmul(Rot, sv0_apo.R);
	V0_equ = rhtmul(Rot, sv0_apo.V);
	coe = OrbMech::coe_from_sv(R0_equ, V0_equ, mu);
	u_b = fmod(coe.TA + coe.w, PI2);
	u_d = fmod(u_b + PI, PI2);
	sv1 = OrbMech::PMMAEGS(sv0_apo, 2, u_d, error2);
	dh = length(sv0_apo.R) - length(sv1.R);
	DV2 = HeightManeuverInteg(sv0_apo, dh);

	return DV1 + DV2;
}

bool RTCC::GETEval(double get)
{
	if (OrbMech::GETfromMJD(oapiGetSimMJD(), calcParams.TEPHEM) > get)
	{
		return true;
	}

	return false;
}

void RTCC::AGOPCislunarNavigation(SV sv, MATRIX3 REFSMMAT, int star, double yaw, VECTOR3 &IMUAngles, double &TA, double &SA)
{
	MATRIX3 M, SBNB, CBNB;
	VECTOR3 U_S, R_ZC, U_Z, u0, u1, u2, R_H, U_SH, t0, t1, R_L, R_CH;
	double r_Z, x_H, y_H, A, alpha, beta, A0, A1, a;
	bool H, k;

	U_S = navstars[star - 1];
	R_ZC = sv.R;
	//Near Horizon
	H = false;
	U_Z = _V(0, 0, 1);
	r_Z = oapiGetSize(sv.gravref);
	a = -0.5676353234;
	SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));

	u2 = unit(crossp(U_S, R_ZC));
	u0 = unit(crossp(U_Z, u2));
	u1 = crossp(u2, u0);
	M = _M(u0.x, u0.y, u0.z, u1.x, u1.y, u1.z, u2.x, u2.y, u2.z);

	R_H = mul(M, R_ZC);
	U_SH = mul(M, U_S);

	x_H = R_H.x;
	y_H = R_H.z;

	A = (x_H*x_H + y_H * y_H) / (r_Z*r_Z);
	alpha = y_H * sqrt(A - 1.0);
	beta = x_H * sqrt(A - 1.0);

	t0 = _V(x_H + alpha, y_H - beta, 0)*1.0 / A;
	t1 = _V(x_H - alpha, y_H + beta, 0)*1.0 / A;
	A0 = dotp(U_SH, unit(t0 - R_H));
	A1 = dotp(U_SH, unit(t1 - R_H));

	if (A1 > A0)
		k = true;
	else
		k = false;

	if (H) k = !k;

	if (k == false)
		R_L = tmul(M, t0);
	else
		R_L = tmul(M, t1);

	//Vector pointing from CSM to horizon
	R_CH = R_L - R_ZC;

	MATRIX3 CBSB;
	VECTOR3 X_SB, Y_SB, Z_SB;

	Z_SB = unit(R_CH);
	Y_SB = unit(crossp(Z_SB, U_S));
	X_SB = crossp(Y_SB, Z_SB);

	CBSB = _M(X_SB.x, X_SB.y, X_SB.z, Y_SB.x, Y_SB.y, Y_SB.z, Z_SB.x, Z_SB.y, Z_SB.z);

	CBNB = mul(SBNB, CBSB);
	IMUAngles = OrbMech::CALCGAR(REFSMMAT, CBNB);

	//Calculate sextant angles
	OrbMech::CALCSXA(CBNB, U_S, TA, SA);
}

VECTOR3 RTCC::LOICrewChartUpdateProcessor(SV sv0, double GETbase, MATRIX3 REFSMMAT, double p_EMP, double LOI_TIG, VECTOR3 dV_LVLH_LOI, double p_T, double y_T)
{
	SV sv_tig;
	MATRIX3 M_EMP, M_R, M, M_RTM;
	VECTOR3 X_B, UX, UY, UZ, IMUangles;
	double dt, headsswitch;

	headsswitch = -1.0;

	dt = LOI_TIG - OrbMech::GETfromMJD(sv0.MJD, GETbase);
	sv_tig = coast(sv0, dt);
	M_EMP = OrbMech::EMPMatrix(sv_tig.MJD);
	X_B = tmul(M_EMP, _V(-sin(p_EMP), cos(p_EMP), 0));

	UX = X_B;
	UY = unit(crossp(X_B, sv_tig.R*headsswitch));
	UZ = unit(crossp(X_B, crossp(X_B, sv_tig.R*headsswitch)));

	M_R = _M(UX.x, UX.y, UX.z, UY.x, UY.y, UY.z, UZ.x, UZ.y, UZ.z);
	M = _M(cos(y_T)*cos(p_T), sin(y_T), -cos(y_T)*sin(p_T), -sin(y_T)*cos(p_T), cos(y_T), sin(y_T)*sin(p_T), sin(p_T), 0.0, cos(p_T));
	M_RTM = mul(OrbMech::tmat(M_R), M);
	IMUangles = OrbMech::CALCGAR(REFSMMAT, mul(OrbMech::tmat(M), M_R));

	return IMUangles;
}

bool RTCC::PoweredDescentProcessor(VECTOR3 R_LS, double TLAND, SV sv, double GETbase, RTCCNIAuxOutputTable &aux, EphemerisDataTable *E, SV &sv_PDI, SV &sv_land, double &dv)
{
	MATRIX3 Rot, REFSMMAT;
	DescentGuidance descguid;
	AscDescIntegrator integ;
	SV sv_IG, sv_D;
	VECTOR3 U_FDP, WI, W, R_LSP, U_FDP_abort, U_M;
	double t_go, CR, t_PDI, t_UL, t_D, W_TD, T_DPS, isp, t_D_old;
	bool stop;

	bool LandFlag = false;
	t_UL = 7.9;
	dv = 0.0;

	if (!PDIIgnitionAlgorithm(sv, GETbase, R_LS, TLAND, sv_IG, t_go, CR, U_FDP, REFSMMAT))
	{
		return false;
	}
	t_go = -t_go;
	t_PDI = OrbMech::GETfromMJD(sv_IG.MJD, GETbase);
	t_D = t_PDI - t_UL;
	t_D_old = t_D;
	sv_D = coast(sv_IG, -t_UL);
	sv_PDI = sv_IG;

	Rot = OrbMech::GetRotationMatrix(BODY_MOON, sv_IG.MJD);
	WI = rhmul(Rot, _V(0, 0, 1));
	W = mul(REFSMMAT, WI)*OrbMech::w_Moon;
	R_LSP = mul(REFSMMAT, rhmul(Rot, R_LS));
	descguid.Init(sv_IG.R, sv_IG.V, sv_IG.mass, t_PDI, REFSMMAT, R_LSP, t_PDI, W, t_go, &RTCCDescentTargets);
	W_TD = sv_IG.mass;
	U_FDP_abort = tmul(REFSMMAT, unit(U_FDP));

	stop = false;

	integ.Init(U_FDP_abort);

	do
	{
		descguid.Guidance(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
		LandFlag = integ.Integration(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
		dv += T_DPS / W_TD * (t_D - t_D_old);
		t_D_old = t_D;
	} while (LandFlag == false);

	sv_land.gravref = sv.gravref;
	sv_land.MJD = OrbMech::MJDfromGET(t_D, GETbase);
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, sv_land.MJD);
	
	sv_land.mass = W_TD;
	sv_land.R = rhmul(Rot, R_LS);
	U_M = rhmul(Rot, _V(0, 0, 1));
	sv_land.V = crossp(U_M, sv_land.R)*OrbMech::w_Moon;

	return true;
}

void RTCC::LunarAscentProcessor(VECTOR3 R_LS, double m0, SV sv_CSM, double GETbase, double t_liftoff, double v_LH, double v_LV, double &theta, double &dt_asc, double &dv, SV &sv_IG, SV &sv_Ins)
{
	//Test
	AscentGuidance asc;
	AscDescIntegrator integ;
	SV sv_CSM_TIG, sv_ins;
	MATRIX3 Rot;
	VECTOR3 R0, V0, R, V, U_FDP, U_M, u, R0_proj;
	double t_go, Thrust, w_M, dt, t_total, isp, m1, t_total_old;
	bool stop = false;

	dv = 0.0;
	t_total = 0.0;
	dt = t_liftoff - OrbMech::GETfromMJD(sv_CSM.MJD, GETbase);
	sv_CSM_TIG = coast(sv_CSM, dt);

	asc.Init(sv_CSM_TIG.R, sv_CSM_TIG.V, m0, length(R_LS), v_LH, v_LV);
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, sv_CSM_TIG.MJD);
	R0 = rhmul(Rot, R_LS);
	w_M = OrbMech::w_Moon;
	U_M = rhmul(Rot, _V(0, 0, 1));
	V0 = crossp(U_M, R0)*w_M;

	sv_IG.gravref = sv_CSM.gravref;
	sv_IG.mass = m0;
	sv_IG.MJD = sv_CSM_TIG.MJD;
	sv_IG.R = R0;
	sv_IG.V = V0;

	R = R0;
	V = V0;
	m1 = m0;
	t_total_old = t_total;
	integ.Init(unit(R));

	while (stop == false)
	{
		asc.Guidance(R, V, m1, t_total, U_FDP, t_go, Thrust, isp);
		stop = integ.Integration(R, V, m1, t_total, U_FDP, t_go, Thrust, isp);
		dv += Thrust / m1 * (t_total - t_total_old);
		t_total_old = t_total;
	}

	u = unit(crossp(R, V));
	R0_proj = unit(R0 - u * dotp(R0, u))*length(R0);
	theta = acos(dotp(unit(R0_proj), unit(R)));
	dt_asc = t_total;

	sv_ins.R = R;
	sv_ins.V = V;
	sv_ins.mass = m1;
	sv_ins.gravref = sv_CSM_TIG.gravref;
	sv_ins.MJD = sv_CSM_TIG.MJD + dt_asc / 24.0 / 3600.0;
	sv_Ins = sv_ins;
}

bool RTCC::PDIIgnitionAlgorithm(SV sv, double GETbase, VECTOR3 R_LS, double TLAND, SV &sv_IG, double &t_go, double &CR, VECTOR3 &U_IG, MATRIX3 &REFSMMAT)
{
	SV sv_I;
	MATRIX3 C_GP, Rot;
	VECTOR3 U_FDP, dV_TrP, R_TG, V_TG, A_TG, R_LSI, R_LSP, W_I, W_P, R_P, G_P, V_P, R_G, V_SURFP, V_G, A_G, A_FDP;
	VECTOR3 C_XGP, C_YGP, C_ZGP, U_XSM, U_YSM, U_ZSM;
	double GUIDDURN, AF_TRIM, DELTTRIM, TTT, t_pip, dt_I, FRAC;
	double v_IGG, r_IGXG, r_IGZG, K_X, K_Y, K_V;
	double J_TZG, A_TZG, V_TZG, R_TZG;
	double LEADTIME, w_M, t_2, t_I, PIPTIME, t_pipold, eps, dTTT, TTT_P, TEM, q;
	int n1, n2, COUNT_TTT;

	GUIDDURN = 664.4;
	AF_TRIM = 0.350133;
	DELTTRIM = 26.0;
	n1 = 40;
	n2 = 2;
	COUNT_TTT = 0;
	U_FDP = dV_TrP = _V(0, 0, 0);
	C_GP = _M(1.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0);
	TTT = 0.0;
	t_pip = TLAND;
	dt_I = 1.0;
	FRAC = 43455.0;

	v_IGG = RTCCPDIIgnitionTargets.v_IGG;
	r_IGXG = RTCCPDIIgnitionTargets.r_IGXG;
	r_IGZG = RTCCPDIIgnitionTargets.r_IGZG;
	K_X = RTCCPDIIgnitionTargets.K_X;
	K_Y = RTCCPDIIgnitionTargets.K_Y;
	K_V = RTCCPDIIgnitionTargets.K_V;
	J_TZG = RTCCDescentTargets.JBRFGZ;
	A_TZG = RTCCDescentTargets.ABRFG.z;
	V_TZG = RTCCDescentTargets.VBRFG.z;
	R_TG = RTCCDescentTargets.RBRFG;
	R_TZG = RTCCDescentTargets.RBRFG.z;
	V_TG = RTCCDescentTargets.VBRFG;
	A_TG = RTCCDescentTargets.ABRFG;
	LEADTIME = 2.2;
	w_M = 2.66169948e-6;

	t_2 = OrbMech::GETfromMJD(sv.MJD, GETbase);
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, GETbase + TLAND / 24.0 / 3600.0);
	R_LSI = rhmul(Rot, R_LS);

	U_XSM = unit(R_LSI);
	U_ZSM = unit(crossp(crossp(sv.R, sv.V), U_XSM));
	U_YSM = crossp(U_ZSM, U_XSM);
	REFSMMAT = _M(U_XSM.x, U_XSM.y, U_XSM.z, U_YSM.x, U_YSM.y, U_YSM.z, U_ZSM.x, U_ZSM.y, U_ZSM.z);

	R_LSP = mul(REFSMMAT, R_LSI);
	t_I = TLAND - GUIDDURN;
	W_I = rhmul(Rot, _V(0.0, 0.0, 1.0));
	W_P = mul(REFSMMAT, W_I)*w_M;
	sv_I = coast(sv, t_I - t_2);

	while (abs(dt_I) > 0.08 && n1>0)
	{
		PIPTIME = t_I;
		R_P = mul(REFSMMAT, sv_I.R);
		G_P = -R_P / pow(length(R_P), 3.0)*OrbMech::mu_Moon;

		n2 = 2;

		t_pipold = t_pip;
		t_pip = PIPTIME;
		t_2 = PIPTIME;
		R_LSP = unit(R_LSP + crossp(W_P, R_LSP)*(t_pip - t_pipold))*length(R_LS);
		TTT = TTT + t_pip - t_pipold;

		while (n2 > 0)
		{
			V_P = mul(REFSMMAT, sv_I.V) + dV_TrP;

			R_G = mul(C_GP, R_P - R_LSP);
			V_SURFP = V_P - crossp(W_P, R_P);
			V_G = mul(C_GP, V_SURFP);

			COUNT_TTT = 0;
			eps = abs(TTT / 128.0);
			do
			{
				dTTT = -(J_TZG*pow(TTT, 3.0) + 6.0*A_TZG*TTT*TTT + (18.0*V_TZG + 6.0*V_G.z)*TTT + 24.0*(R_TZG - R_G.z)) / (3.0*J_TZG*TTT*TTT + 12.0*A_TZG*TTT + 18.0*V_TZG + 6.0*V_G.z);
				TTT += dTTT;
				COUNT_TTT++;
			} while (abs(dTTT) > eps && COUNT_TTT < 8);

			if (COUNT_TTT == 8)
			{
				return false;
			}

			TTT_P = TTT + LEADTIME;
			A_G = (R_TG - R_G)*(-24.0*TTT_P / pow(TTT, 3.0) + 36.0*TTT_P*TTT_P / pow(TTT, 4.0));
			A_G += V_TG * (-18.0*TTT_P / TTT / TTT + 24.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += V_G * (-6.0*TTT_P / TTT / TTT + 12.0*TTT_P*TTT_P / pow(TTT, 3.0));
			A_G += A_TG * (6.0*TTT_P*TTT_P / TTT / TTT - 6.0*TTT_P / TTT + 1.0);

			A_FDP = tmul(C_GP, A_G) - G_P;
			//s = length(A_FDP);
			TEM = FRAC * FRAC / sv.mass / sv.mass - A_FDP.x*A_FDP.x - A_FDP.y*A_FDP.y;
			if (TEM < 0.0)
			{
				TEM = 0.0;
			}
			if (sqrt(TEM) + A_FDP.z < 0.0)
			{
				A_FDP.z = -sqrt(TEM);
			}
			U_FDP = A_FDP;

			C_XGP = unit(R_LSP);
			C_YGP = unit(crossp(unit(V_SURFP*TTT / 4.0 + R_LSP - R_P), R_LSP));
			C_ZGP = crossp(C_XGP, C_YGP);
			C_GP = _M(C_XGP.x, C_YGP.x, C_ZGP.x, C_XGP.y, C_YGP.y, C_ZGP.y, C_XGP.z, C_YGP.z, C_ZGP.z);

			dV_TrP = unit(U_FDP)*DELTTRIM*AF_TRIM;

			n2--;
		}

		n1--;
		q = K_X * (R_G.x - r_IGXG) + K_Y * R_G.y*R_G.y + R_G.z - r_IGZG + K_V * (length(V_G) - v_IGG);
		dt_I = -q / (V_G.z + K_X * V_G.x);
		t_I += dt_I;

		if (abs(dt_I) > 0.08)
		{
			sv_I = coast(sv_I, dt_I);
		}
	}

	if (n1 == 0)
	{
		return false;
	}

	sv_IG = coast(sv_I, -DELTTRIM);
	t_go = TTT;
	U_IG = U_FDP;
	CR = dotp(unit(crossp(V_P, R_P)), R_LSP);

	return true;
}

//Based on NTRS 19740072723
bool RTCC::PoweredDescentAbortProgram(PDAPOpt opt, PDAPResults &res)
{
	SPQOpt conopt;
	SPQResults conres;
	DescentGuidance descguid;
	AscentGuidance ascguid;
	AscDescIntegrator integ;
	SV sv_I_guess, sv_IG, sv_D, sv_CSM_Ins, sv_LM_Ins, sv_Abort, sv_CAN, sv_CAN_apo, sv_CSM_Abort;
	MATRIX3 Rot, Q_Xx, REFSMMAT;
	VECTOR3 U_FDP, WM, WI, W, R_LSP, U_FDP_abort;
	double t_go, CR, t_PDI, t_D, t_UL, t_stage, W_TD, T_DPS, dt_abort, Z_D_dot, R_D_dot, W_TA, t, T, isp, t_Ins, TS, theta, r_Ins, A_Ins, H_a, t_CSI, DH_D;
	double SLOPE, dV_Inc, dh_apo, w_M, V_H_min, t_CAN, dt_CSI, R_a, R_a_apo, dt_CAN, theta_D, theta_apo, t_go_abort;
	int K_loop;
	bool K_stage;
	//false = CSI/CDH, true = Boost + CSI/CDH
	bool K3;
	bool LandFlag = false;
	bool InsertionFlag = false;
	bool stop = false;
	std::vector<double> t_Abort_Table;
	std::vector<double> dV_Abort_Table;
	std::vector<double> Phase_Table;
	std::vector<double> A_ins_Table;
	int i = 0;

	K3 = false;
	w_M = 2.66169948e-6;
	t_UL = 7.9;
	Z_D_dot = 5650.0*0.3048;
	R_D_dot = 19.5*0.3048;
	DH_D = 15.0*1852.0;
	V_H_min = 5515.0*0.3048;
	dt_CSI = opt.dt_CSI;
	dt_CAN = opt.dt_CAN;
	conopt.E = 26.6*RAD;
	conopt.GETbase = opt.GETbase;
	conopt.K_CDH = 0;
	conopt.t_TPI = opt.t_TPI;

	res.R_amin = length(opt.R_LS) + opt.h_amin;

	dt_abort = opt.dt_step;
	sv_I_guess = coast(opt.sv_A, opt.TLAND - OrbMech::GETfromMJD(opt.sv_A.MJD, opt.GETbase));
	if (!PDIIgnitionAlgorithm(sv_I_guess, opt.GETbase, opt.R_LS, opt.TLAND, sv_IG, t_go, CR, U_FDP, REFSMMAT))
	{
		return false;
	}
	t_go = -t_go;
	t_go_abort = t_go;
	t_PDI = OrbMech::GETfromMJD(sv_IG.MJD, opt.GETbase);
	t_D = t_PDI - t_UL;
	sv_Abort = coast(sv_IG, -t_UL);
	t_stage = t_PDI + opt.dt_stage;

	Rot = OrbMech::GetRotationMatrix(BODY_MOON, sv_IG.MJD);
	WI = rhmul(Rot, _V(0, 0, 1));
	W = mul(opt.REFSMMAT, WI)*w_M;
	R_LSP = mul(opt.REFSMMAT, rhmul(Rot, opt.R_LS));
	descguid.Init(sv_IG.R, sv_IG.V, opt.sv_A.mass, t_PDI, opt.REFSMMAT, R_LSP, t_PDI, W, t_go, &RTCCDescentTargets);
	W_TD = opt.sv_A.mass;
	U_FDP_abort = tmul(opt.REFSMMAT, unit(U_FDP));

	res.DEDA225 = (length(R_LSP) + 60000.0*0.3048 + length(R_LSP) + opt.h_amin) / 2.0;
	res.DEDA226 = 7031200.0*0.3048;

	do
	{
		stop = false;

		integ.Init(U_FDP_abort);
		sv_D = sv_Abort;
		t_go = t_go_abort;
		K_loop = 0;
		SLOPE = 2.0 / 1852.0*0.3048;

		do
		{
			if (LandFlag)
			{
				t_D = t_PDI + dt_abort;
				t_stage = 0.;
				break;
			}
			else
			{
				descguid.Guidance(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
				LandFlag = integ.Integration(sv_D.R, sv_D.V, W_TD, t_D, U_FDP, t_go, T_DPS, isp);
				if (LandFlag)
				{
					t_D = t_PDI + dt_abort;
					t_stage = 0.;
					break;
				}
			}
		} while (t_D <= t_PDI + dt_abort);

		T = 43192.23;
		sv_Abort = sv_D;
		U_FDP_abort = U_FDP;
		t_go_abort = t_go;

		sv_CSM_Abort = coast(opt.sv_P, t_D - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));
		WM = unit(crossp(sv_CSM_Abort.V, sv_CSM_Abort.R));
		TS = dotp(crossp(unit(sv_CSM_Abort.R), unit(sv_Abort.R)), WM);
		theta_D = OrbMech::sign(TS)*acos(dotp(unit(sv_CSM_Abort.R), unit(sv_Abort.R)));

		do
		{
			sv_D = sv_Abort;
			ascguid.Init(opt.sv_P.R, opt.sv_P.V, W_TD, length(opt.R_LS), Z_D_dot, R_D_dot, false);
			t = t_D;
			t_go = dt_abort;
			W_TA = W_TD;
			if (t >= t_stage || W_TA <= opt.W_TDRY)
			{
				t_go *= 2.0;
			}
			ascguid.SetTGO(t_go);
			K_stage = false;

			do
			{
				if (K_stage == false)
				{
					if (t >= t_stage || W_TA <= opt.W_TDRY)
					{
						K_stage = true;
						W_TA = opt.W_TAPS;
						ascguid.SetThrustParams(true);
					}
				}

				ascguid.Guidance(sv_D.R, sv_D.V, W_TA, t, U_FDP, t_go, T, isp);
				if (dotp(U_FDP, integ.GetCurrentTD()) < 0)
				{
					if (acos(dotp(integ.GetCurrentTD(), unit(sv_D.R))) > 30.0*RAD)
					{
						U_FDP = unit(sv_D.R);
					}
				}
				InsertionFlag = integ.Integration(sv_D.R, sv_D.V, W_TA, t, U_FDP, t_go, T, isp);
			} while (InsertionFlag == false);
			t_Ins = t;
			sv_LM_Ins = sv_D;
			sv_LM_Ins.mass = W_TA;
			sv_LM_Ins.MJD = OrbMech::MJDfromGET(t_Ins, opt.GETbase);

			//Overwrite actual insertion velocity with desired one; gives more consistent results
			Q_Xx = OrbMech::LVLH_Matrix(sv_LM_Ins.R, sv_LM_Ins.V);
			sv_LM_Ins.V = tmul(Q_Xx, _V(Z_D_dot, 0, -R_D_dot));

			sv_CSM_Ins = coast(opt.sv_P, t_Ins - OrbMech::GETfromMJD(opt.sv_P.MJD, opt.GETbase));
			WM = unit(crossp(sv_CSM_Ins.V, sv_CSM_Ins.R));
			TS = dotp(crossp(unit(sv_CSM_Ins.R), unit(sv_LM_Ins.R)), WM);
			theta = OrbMech::sign(TS)*acos(dotp(unit(sv_CSM_Ins.R), unit(sv_LM_Ins.R)));
			
			r_Ins = length(sv_D.R);
			A_Ins = OrbMech::mu_Moon * r_Ins / (2.0*OrbMech::mu_Moon - r_Ins * Z_D_dot*Z_D_dot);
			H_a = 2.0*A_Ins - r_Ins - length(opt.R_LS);
			R_a = 2.0*A_Ins - r_Ins;
			t_CSI = t_Ins + dt_CSI;

			if (K3 == false || dt_CAN <= 0.0 || opt.dv_CAN <= 0.0)
			{
				conopt.sv_A = sv_LM_Ins;
			}
			else
			{
				t_CAN = t_Ins + dt_CAN;
				sv_CAN = coast(sv_LM_Ins, t_CAN - t_Ins);
				sv_CAN_apo = sv_CAN;
				sv_CAN_apo.V += tmul(OrbMech::LVLH_Matrix(sv_CAN.R, sv_CAN.V), _V(opt.dv_CAN, 0.0, 0.0));
				conopt.sv_A = sv_CAN_apo;
			}

			conopt.sv_P = sv_CSM_Ins;
			conopt.t_CSI = t_CSI;

			ConcentricRendezvousProcessor(conopt, conres);
			K_loop++;
			if (K_loop > 1)
			{
				SLOPE = dV_Inc / (conres.DH - dh_apo);
			}

			dV_Inc = SLOPE * (DH_D - conres.DH);
			Z_D_dot += dV_Inc;
			dh_apo = conres.DH;

		} while (abs(dV_Inc) > 0.1*0.3048 && K_loop < 10);

		if (opt.IsTwoSegment && R_a < res.R_amin)
		{
			if (K3 == false)
			{
				K_loop = 0;
				dt_CSI += opt.dt_2CSI;
				conopt.t_TPI += opt.dt_2TPI;
				res.Theta_LIM = theta_apo + (theta_D - theta_apo) / (R_a - R_a_apo)*(res.R_amin - R_a_apo);
				if (dt_CAN >= dt_CSI)
				{
					dt_CAN = 0.0;
				}

				OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.K1, res.J1);
				res.DEDA227 = res.K1;
				res.DEDA224 = res.J1;
				Phase_Table.clear();
				A_ins_Table.clear();
				K3 = true;
			}
			else
			{
				OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.K2, res.J2);
				stop = true;
			}
		}
		else
		{
			t_Abort_Table.push_back(dt_abort);
			dV_Abort_Table.push_back(Z_D_dot);
			Phase_Table.push_back(theta);
			A_ins_Table.push_back(A_Ins);
			theta_apo = theta_D;
			dt_abort += opt.dt_step;
			R_a_apo = R_a;
		}

		if (!opt.IsTwoSegment && Z_D_dot <= V_H_min)
		{
			stop = true;
		}

	} while (stop == false);

	//If we use the two segment logic, then everything has already been calculated
	if (opt.IsTwoSegment) return true;

	//Apollo 11: First 4 solutions from T vs. V_H table for cubic function
	if (dV_Abort_Table.size() < 4)
	{
		return false;
	}

	double t_Abort[4] = { t_Abort_Table[0], t_Abort_Table[1] , t_Abort_Table[2] , t_Abort_Table[3] };
	double dV_Abort[4] = { dV_Abort_Table[0], dV_Abort_Table[1] , dV_Abort_Table[2] , dV_Abort_Table[3] };
	double coeff[4];
	OrbMech::CubicInterpolation(t_Abort, dV_Abort, coeff);

	res.ABTCOF1 = coeff[0];
	res.ABTCOF2 = coeff[1];
	res.ABTCOF3 = coeff[2];
	res.ABTCOF4 = coeff[3];

	OrbMech::LinearLeastSquares(Phase_Table, A_ins_Table, res.DEDA227, res.DEDA224);

	return true;
}

bool RTCC::LunarLiftoffTimePredictionCFP(const LunarLiftoffTimeOpt &opt, VECTOR3 R_LS, SV sv_P, OBJHANDLE hMoon, double h_1, double theta_Ins, double t_L_guess, double t_TPI, LunarLiftoffResults &res)
{
	// NOMENCLATURE:
	// R = position vector, V = velocity vector
	// A = active vehicle, P = passive vehicle
	// 1 = Insertion, 2 = CSI, 3 = CDH, 4 = TPI (actual), 5 = TPI (LM, desired), 6 = TPI (CSM, desired), 7 = TPF (CSM and LM)
	SV sv_6;
	MATRIX3 Rot;
	VECTOR3 u, R_1, V_1, R_2, V_2, R_5, V_2F, R_3, V_3, R_L, U_L, R_3F, V_3F, R_PJ, V_PJ, R_P3S, V_P3S, R_4, V_4, V_4F, R_7, V_7, R_7F, V_7F;
	int n;
	double r_M, r_A, dt_3, MJD_TPI, dt, MJD_L, t_3, r_Ins, dV_CSI;
	double dt_2, t_L, t_1, t_2, dt_4, e_P, eps, to_L, eo_P, c_F, p_C;
	int s_F;

	r_M = length(R_LS);
	MJD_TPI = opt.GETbase + t_TPI / 24.0 / 3600.0;

	s_F = 0;
	c_F = p_C = 0.0;
	eps = 0.002*RAD;
	n = 0;
	dt = 100.0;
	t_L = t_L_guess;
	r_Ins = r_M + h_1;
	dV_CSI = 10.0*0.3048;

	sv_6 = coast(sv_P, (MJD_TPI - sv_P.MJD)*24.0*3600.0);
	u = unit(crossp(sv_6.R, sv_6.V));

	OrbMech::QDRTPI(sv_6.R, sv_6.V, MJD_TPI, hMoon, OrbMech::mu_Moon, opt.DH_SRCH, opt.E, 0, R_PJ, V_PJ);
	R_5 = R_PJ - unit(R_PJ)*opt.DH_SRCH;
	r_A = length(R_5);

	if (opt.IsInsVelInput)
	{
		res.v_LH = opt.v_LH;
		res.v_LV = opt.v_LV;
	}
	else
	{
		double e_Ins, h_Ins;
		e_Ins = (r_A - r_Ins) / (r_A + cos(theta_Ins)*r_Ins);
		h_Ins = sqrt(r_A* OrbMech::mu_Moon*(1.0 - e_Ins));
		res.v_LV = OrbMech::mu_Moon / h_Ins * e_Ins*sin(theta_Ins);
		res.v_LH = OrbMech::mu_Moon / h_Ins * (1.0 + e_Ins * cos(theta_Ins));
	}

	do
	{
		//Launch to Insertion
		MJD_L = opt.GETbase + t_L / 24.0 / 3600.0;
		Rot = OrbMech::GetRotationMatrix(BODY_MOON, MJD_L);
		R_L = rhmul(Rot, R_LS);
		U_L = unit(R_L - u * dotp(u, R_L));
		R_1 = (U_L*cos(opt.theta_1) + crossp(u, U_L)*sin(opt.theta_1))*r_Ins;
		V_1 = unit(crossp(u, unit(R_1)))*res.v_LH + unit(R_1)*res.v_LV;
		t_1 = t_L + opt.dt_1;
		//Insertion to CSI
		dt_2 = OrbMech::timetoapo(R_1, V_1, OrbMech::mu_Moon);
		OrbMech::rv_from_r0v0(R_1, V_1, dt_2, R_2, V_2, OrbMech::mu_Moon);
		t_2 = t_1 + dt_2;
		OrbMech::CSIToDH(R_2, V_2, sv_6.R, sv_6.V, opt.DH_SRCH, OrbMech::mu_Moon, dV_CSI);
		V_2F = V_2 + unit(crossp(u, R_2))*dV_CSI;
		//CSI to CDH
		OrbMech::REVUP(R_2, V_2F, 0.5, OrbMech::mu_Moon, R_3, V_3, dt_3);
		t_3 = t_2 + dt_3;
		OrbMech::RADUP(sv_6.R, sv_6.V, R_3, OrbMech::mu_Moon, R_P3S, V_P3S);
		OrbMech::COE(R_P3S, V_P3S, length(R_P3S) - length(R_3), OrbMech::mu_Moon, R_3F, V_3F);

		//CDH to TPI
		dt_4 = t_TPI - t_3;
		OrbMech::rv_from_r0v0(R_3, V_3F, dt_4, R_4, V_4, OrbMech::mu_Moon);

		e_P = OrbMech::sign(dotp(crossp(R_4, R_5), u))*acos(dotp(unit(R_5), unit(R_4)));
		if (abs(e_P) >= eps)
		{
			OrbMech::ITER(c_F, s_F, e_P, p_C, t_L, eo_P, to_L);
			if (s_F == 1)
			{
				return false;
			}
		}
	} while (abs(e_P) >= eps);

	double dt_F;

	res.t_L = t_L;
	res.t_Ins = t_1;
	res.t_CSI = t_2;
	res.t_CDH = t_3;

	dt_F = OrbMech::time_theta(sv_6.R, sv_6.V, opt.theta_F, OrbMech::mu_Moon);
	OrbMech::rv_from_r0v0(sv_6.R, sv_6.V, dt_F, R_7, V_7, OrbMech::mu_Moon);
	V_4F = OrbMech::elegant_lambert(R_4, V_4, R_7, dt_F, 0, false, OrbMech::mu_Moon);
	OrbMech::rv_from_r0v0(R_4, V_4F, dt_F, R_7F, V_7F, OrbMech::mu_Moon);

	res.t_TPF = t_TPI + dt_F;
	res.DV_CSI = length(V_2F - V_2);
	res.DV_CDH = length(V_3F - V_3);
	res.DV_TPI = length(V_4F - V_4);
	res.DV_TPF = length(V_7 - V_7F);
	res.DV_T = res.DV_CSI + res.DV_CDH + res.DV_TPI + res.DV_TPF;

	return true;
}

bool RTCC::LunarLiftoffTimePredictionDT(const LLTPOpt &opt, LunarLaunchTargetingTable &res)
{
	if (opt.sv_CSM.RBI != BODY_MOON) return false;

	TwoImpulseOpt lamman;
	TwoImpulseResuls lamres;
	EphemerisData sv_TH, sv_LS, sv_INS, sv_TPI, sv_TPF;
	VECTOR3 R_BO, V_BO, R_LS;
	double T_LO, T_LO_0, MJD_L, MJD_BO, S, DV_Z0, DV_Z, MJD_LS, T_TPI, T_INS, V_H;
	int i, k, I_max;

	lamman.mode = 5;
	sv_INS.RBI = sv_LS.RBI = BODY_MOON;

	sv_TH = coast(opt.sv_CSM, opt.T_TH - opt.sv_CSM.GMT);
	MJD_LS = OrbMech::P29TimeOfLongitude(sv_TH.R, sv_TH.V, GMTBASE + sv_TH.GMT / 24.0 / 3600.0, hMoon, opt.lng_LS);
	//Bias by 1 minute from CSM over LS
	T_LO = OrbMech::GETfromMJD(MJD_LS, GMTBASE) - 60.0;
	sv_LS = coast(sv_TH, T_LO - sv_TH.GMT);

	i = 0;
	k = 0;
	V_H = 5540.0*0.3048;
	I_max = 10;
	R_LS = OrbMech::r_from_latlong(opt.lat_LS, opt.lng_LS, opt.R_LS);

	T_TPI = T_LO + opt.dt_PF + opt.dt_INS_TPI;

	do
	{
		MJD_L = GMTBASE + T_LO / 24.0 / 3600.0;
		OrbMech::ENSERT(sv_LS.R, sv_LS.V, opt.dt_PF, opt.Y_S, opt.alpha_PF, opt.h_INS, V_H, opt.V_Z_NOM, MJD_L, R_LS, R_BO, V_BO, MJD_BO);
		T_INS = T_LO + opt.dt_PF;
		sv_INS.R = R_BO;
		sv_INS.V = V_BO;
		sv_INS.GMT = T_INS;
		if (k < 1)
		{
			lamman.T1 = T_INS;
			lamman.T2 = T_TPI;
			lamman.PhaseAngle = opt.dTheta_TPI;
			lamman.DH = opt.DH_TPI;
		}
		else
		{
			lamman.T1 = T_TPI;
			lamman.T2 = -1.0;
			lamman.WT = opt.WT;
			lamman.PhaseAngle = 0.0;
			lamman.DH = 0.0;
		}
		
		lamman.sv_A = coast(sv_INS, lamman.T1 - sv_INS.GMT);
		lamman.sv_P = coast(sv_LS, lamman.T1 - sv_LS.GMT);

		PMSTICN(lamman, lamres);
		if (lamres.SolutionFound == false)
		{
			return false;
		}
		DV_Z = lamres.dV_LVLH.z;

		if (k >= 1) break;
		if (abs(DV_Z) <= 0.1*0.3048)
		{
			k = 1;
			V_H = V_H + lamres.dV_LVLH.x;
			continue;
		}
		if (i > I_max)
		{
			return false;
		}
		i++;
		if (i == 1)
		{
			S = 0.455 / 0.3048;
		}
		else
		{
			S = (T_LO - T_LO_0) / (DV_Z - DV_Z0);
		}
		DV_Z0 = DV_Z;
		T_LO_0 = T_LO;
		T_LO = T_LO - S * DV_Z;
		T_TPI = T_TPI - S * DV_Z;
	} while (i <= I_max);

	res.GETLOR = GETfromGMT(T_LO);
	res.GET_TPI = GETfromGMT(T_TPI);
	res.GET_TPF = lamres.T2;
	res.DV_TPI = length(lamres.dV);
	res.DV_TPF = length(lamres.dV2);
	res.DV_TPI_LVLH = lamres.dV_LVLH;
	res.DV_TPF_LVLH = lamres.dV_LVLH2;
	res.VH = V_H;

	sv_TPI = coast(sv_INS, T_TPI - sv_INS.GMT);
	sv_TPI.V = sv_TPI.V + lamres.dV;
	sv_TPF = coast(sv_TPI, GMTfromGET(lamres.T2) - sv_TPI.GMT);
	sv_TPF.V = sv_TPF.V + lamres.dV2;
	
	double RA_TPI, RP_TPI, RA_TPF, RP_TPF, RA_T, RP_T;
	OrbMech::periapo(sv_TPI.R, sv_TPI.V, OrbMech::mu_Moon, RA_TPI, RP_TPI);
	OrbMech::periapo(sv_TPF.R, sv_TPF.V, OrbMech::mu_Moon, RA_TPF, RP_TPF);
	OrbMech::periapo(sv_LS.R, sv_LS.V, OrbMech::mu_Moon, RA_T, RP_T);

	res.HA_TPI = RA_TPI - MCSMLR;
	res.HP_TPI = RP_TPI - MCSMLR;
	res.HA_TPF = RA_TPF - MCSMLR;
	res.HP_TPF = RP_TPF - MCSMLR;
	res.HA_T = RA_T - MCSMLR;
	res.HP_T = RP_T - MCSMLR;

	return true;
}

double RTCC::GetSemiMajorAxis(SV sv)
{
	double mu = GGRAV * oapiGetMass(sv.gravref);
	double eps = length(sv.V)*length(sv.V) / 2.0 - mu / (length(sv.R));
	return -mu / (2.0*eps);
}

void RTCC::papiWriteScenario_SV(FILEHANDLE scn, char *item, SV sv)
{

	char buffer[256], name[16];

	if (sv.gravref)
	{
		oapiGetObjectName(sv.gravref, name, 16);
	}
	else
	{
		sprintf(name, "None");
	}

	sprintf(buffer, "  %s %s %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf %.12lf", item, name, sv.mass, sv.MJD, sv.R.x, sv.R.y, sv.R.z, sv.V.x, sv.V.y, sv.V.z);
	oapiWriteLine(scn, buffer);
}

bool RTCC::papiReadScenario_SV(char *line, char *item, SV &sv)
{

	char buffer[256], name[16];
	SV v;

	if (sscanf(line, "%s", buffer) == 1) {
		if (!strcmp(buffer, item)) {
			if (sscanf(line, "%s %s %lf %lf %lf %lf %lf %lf %lf %lf", buffer, name, &v.mass, &v.MJD, &v.R.x, &v.R.y, &v.R.z, &v.V.x, &v.V.y, &v.V.z) == 10) {
				v.gravref = oapiGetObjectByName(name);
				sv = v;
				return true;
			}
		}
	}
	return false;
}

VECTOR3 RTCC::RLS_from_latlng(double lat, double lng, double alt)
{
	return OrbMech::r_from_latlong(lat, lng, OrbMech::R_Moon + alt);
}

void RTCC::PMXSPT(std::string source, int n)
{
	std::vector<std::string> message;
	std::string temp1, temp2, temp3;
	char Buffer[128];

	switch (n)
	{
	case 1:
		message.push_back("MANEUVER PRIOR TO PRESENT TIME.");
		break;
	case 2:
		message.push_back("MANEUVER TO BE REPLACED NOT IN THE MPT.");
		break;
	case 3:
		message.push_back("MANEUVER TO BE REPLACED OVERLAPS");
		message.push_back("ANOTHER MANEUVER.");
		break;
	case 4:
		message.push_back("MANEUVER PRIOR TO FROZEN MANEUVER.");
		break;
	case 5:
		message.push_back("MPT IS FULL - REQUESTED MANEUVER TRANSFER REJECTED.");
		break;
	case 6:
		message.push_back("INVALID CONFIGURATION CODE OR THRUSTER CODE - MPT UNCHANGED.");
		break;
	case 10:
		message.push_back("MANEUVER EXECUTED - CANNOT FREEZE");
		break;
	case 23:
		message.push_back("TIME ON M50 MED PRIOR TO END OF LAST EXECUTED");
		message.push_back("MANEUVER - MPT UNCHANGED");
		break;
	case 29:
		message.push_back("REQUESTED TWO-IMPULSE SOLUTION");
		message.push_back("NUMBER NOT AVAILABLE");
		break;
	case 30:
		message.push_back("FAILED TO CONVERGE ON ELEVATION ANGLE -");
		message.push_back("NO PLAN COMPUTED.");
		break;
	case 36:
		message.push_back("EXECUTION VECTOR FOR MANEUVER");
		OnlinePrintTime(RTCCONLINEMON.DoubleBuffer[0], temp1);
		message.push_back(RTCCONLINEMON.TextBuffer[0] + ", BEGIN TIME = " + temp1);
		OnlinePrintTime(RTCCONLINEMON.DoubleBuffer[1], temp1);
		sprintf_s(Buffer, "%.1lf", RTCCONLINEMON.DoubleBuffer[2]);
		temp2.assign(Buffer);
		sprintf_s(Buffer, "%.1lf", RTCCONLINEMON.DoubleBuffer[3]);
		temp3.assign(Buffer);
		message.push_back("T = " + temp1 + " AREA = " + temp2 + " WEIGHT = " + temp3);
		sprintf_s(Buffer, "R = %.8lf %.8lf %.8lf", RTCCONLINEMON.VectorBuffer[0].x, RTCCONLINEMON.VectorBuffer[0].y, RTCCONLINEMON.VectorBuffer[0].z);
		temp1.assign(Buffer);
		message.push_back(temp1);
		sprintf_s(Buffer, "V = %.8lf %.8lf %.8lf", RTCCONLINEMON.VectorBuffer[1].x, RTCCONLINEMON.VectorBuffer[1].y, RTCCONLINEMON.VectorBuffer[1].z);
		temp1.assign(Buffer);
		message.push_back(temp1);
		break;
	case 37:
		message.push_back("DELETION TIME IS PRIOR TO A FROZEN MANEUVER - MPT UNCHANGED.");
		break;
	case 38:
		message.push_back("UNUSABLE MANEUVER FOR TRANSFER");
		break;
	case 39:
		message.push_back("UNABLE TO FETCH VECTOR FOR " + RTCCONLINEMON.TextBuffer[0]);
		message.push_back("MPT UNCHANGED");
		break;
	case 40:
		message.push_back("MANEUVER DOES NOT EXIST");
		break;
	case 41:
		message.push_back("INVALID VECTOR ID - CANNOT FREEZE");
		break;
	case 42:
		message.push_back("MANEUVER NOT EXECUTED - CANNOT DELETE AS HISTORY");
		break;
	case 43:
		message.push_back("MANEUVER EXECUTED - CANNOT DELETE AS FUTURE");
		break;
	case 44:
		message.push_back("MANEUVER PRIOR TO FROZEN VECTOR TIME - CANNOT FREEZE ON THIS VECTOR");
		break;
	case 45:
		message.push_back("MANEUVER NOT FROZEN - CANNOT UNFREEZE");
		break;
	case 46:
		message.push_back("MANEUVER EXECUTED - CANNOT UNFREEZE");
		break;
	case 47:
		message.push_back("MANEUVER FROZEN - CANNOT DELETE");
		break;
	case 48:
		message.push_back("ILLEGAL ENTRY");
		break;
	case 49:
		message.push_back("UNABLE TO FETCH VECTOR FOR RENDEZVOUS PLANNING REQUEST");
		break;
	case 50:
		sprintf_s(Buffer, "ERROR CODE %d FROM POWERED FLIGHT ITERATOR", RTCCONLINEMON.IntBuffer[0]);
		message.push_back(Buffer);
		message.push_back("MNVR TRANSFERRED USING BEST PARAMETERS AVAILABLE");
		break;
	case 51:
		message.push_back("UNRECOVERABLE ERROR FROM EMMENI - MPT UNCHANGED");
		break;
	case 52:
		message.push_back("FINITE BURN OF MNVR IS PRIOR TO PRESENT TIME OR");
		message.push_back("OVERLAPS ANOTHER MNVR - MPT UNCHANGED");
		break;
	case 53:
		message.push_back("REQUESTED MANEUVER IS NOT IN MPT - M58 MED IGNORE");
		break;
	case 54:
		message.push_back("REQUESTED MANEUVER IS EXECUTED - M58 MED IGNORE");
		break;
	case 55:
		message.push_back("REQUEST IS FOR TLI/ASC MANEUVER - M58 MED IGNORE");
		break;
	case 56:
		message.push_back("REQUESTED MANEUVER IS FROZEN - M58 MED IGNORE");
		break;
	case 70:
		message.push_back("UNABLE TO CONVERT VECTORS FROM MEAN TO TRUE");
		break;
	case 72:
		message.push_back("DELETE MPT MANEUVERS PRIOR TO ENTERING M55 MED");
		break;
	case 92:
		message.push_back("CONSTRAINT " + RTCCONLINEMON.TextBuffer[0] + " VIOLATED IN");
		message.push_back("COELLIPTIC SEQUENCE.");
		break;
	case 101:
		message.push_back("SPQ PLAN FAILED TO CONVERGE ON OPTIMUM");
		message.push_back("CSI - PLAN RETAINED");
		break;
	case 102:
		message.push_back("NOMINAL TIME OF LIFTOFF FOR SPECIFIED LAUNCH DAY");
		message.push_back("UNAVAILABLE - PROCESSING TERMINATED.");
		break;
	case 108:
		message.push_back("ERROR RETURNED FROM PMMAPD - INVALID APOFOCUS / PERIFOCUS");
		break;
	case 120:
		message.push_back("UNABLE TO CONVERT SPHERICAL ELEMENTS TO R AND V VECTORS.");
		break;
	case 121:
		message.push_back("DIFFERENCE BETWEEN ACTUAL AND NOMINAL");
		message.push_back("LIFTOFF TIME OUTSIDE LIMITS OF LAUNCH AZIMUTH POLYNOMIAL");
		break;
	case 124:
		message.push_back("COAST INTEGRATOR UNABLE TO OBTAIN PERICYNTHION POINT");
		message.push_back("FOR LOI REQUEST - LOI SOLUTIONS ARE UNOBTAINABLE");
		break;
	case 125:
		message.push_back("LUNAR APPROACH HYPERBOLA HAS");
		message.push_back("IMPACTING TRAJECTORY - LOI SOLUTIONS ARE UNOBTAINABLE");
		break;
	case 126:
		message.push_back("LUNAR APPROACH HYPERBOLIC PERICYNTHION GREATER THAN");
		message.push_back("REQUESTED LPO APOLUNE - LOI SOLUTIONS ARE UNOBTAINABLE");
		break;
	case 200:
		message.push_back("ITERATION FAILURE, MVR TRANSFERRED");
		message.push_back("USING BEST PARAMETERS AVAILABLE");
		break;
	case 201:
		message.push_back(RTCCONLINEMON.TextBuffer[0]);
		break;
	default:
		return;
	}
	PMXSPT(source, message);
}

void RTCC::PMXSPT(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::OnlinePrint(const std::string &source, const std::vector<std::string> &message)
{
	if (message.size() == 0)
	{
		return;
	}

	OnlineMonitorMessage data;
	std::string temp, temp2;

	double gmt;
	if (GMTBASE == 0.0)
	{
		gmt = 0.0;
	}
	else
	{
		gmt = RTCCPresentTimeGMT();
	}
	OnlinePrintTime(gmt, temp);
	data.message.push_back(temp + " )" + source + "( " + message[0]);
	for (unsigned i = 1;i < message.size();i++)
	{
		data.message.push_back(message[i]);
	}

	//Also write to text file
	if (RTCCONLINEMON.data.size() == 0)
	{
		rtccdebug.open("RTCCDebug.txt", std::ofstream::trunc);
	}
	else
	{
		rtccdebug.open("RTCCDebug.txt", std::ofstream::app);
	}

	RTCCONLINEMON.data.push_front(data);
	
	for (unsigned i = 0;i < message.size();i++)
	{
		rtccdebug << message[i] << endl;
	}
	rtccdebug.close();

	if (RTCCONLINEMON.data.size() >= 9)
	{
		RTCCONLINEMON.data.pop_back();
	}
}

void RTCC::GMSPRINT(std::string source, int n)
{
	std::vector<std::string> message;

	switch (n)
	{
	case 1:
		message.push_back("RECYCLE TO ORBIT L.S.");
		break;
	case 23:
		message.push_back("P32, ADD...STA ALREADY IN TABLE");
		break;
	case 24:
		message.push_back("P32, ADD...SORRY TABLE FULL");
		break;
	case 25:
		message.push_back("P32, MOD/DEL...STA NOT IN TABLE");
		break;
	case 26:
		message.push_back("LATE Q FROM HIGH-SPEED");
		break;
	case 27:
		message.push_back("INVALID P60 - IGNORED");
		break;
	case 28:
		message.push_back("INVALID ID - IGNORED");
		break;
	case 29:
		message.push_back("INVALID P81 - IGNORED");
		break;
	case 30:
		message.push_back("NO/GO GMSMED - S RTCHNGE");
		break;
	case 31:
		message.push_back("P13 CANNOT BE ROTATED TO ECI -- IGNORE");
		break;
	case 32:
		message.push_back("P32 E/M CONFLICT, RELEASE TABLE AND RE-ENTER MED");
		break;
	case 51:
		message.push_back("MED " + RTCCONLINEMON.TextBuffer[0]);
		break;
	default:
		return;
	}
	GMSPRINT(source, message);
}

void RTCC::GMSPRINT(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::OnlinePrintTime(double TIME_SEC, std::string &time)
{
	char Buffer[128];
	double days, hours, minutes, seconds;
	if (TIME_SEC < 0)
	{
		TIME_SEC = 0.0;
	}
	days = trunc(TIME_SEC / 86400.0);
	hours = trunc((TIME_SEC - 86400.0*days) / 3600.0);
	minutes = trunc((TIME_SEC - 86400.0 * days - 3600.0 * hours) / 60.0);
	seconds = TIME_SEC - 86400.0*days - 3600.0*hours - 60.0*minutes;

	sprintf(Buffer, "%02.0lf/%02.0lf/%02.0lf/%05.2lf", days, hours, minutes, seconds);
	time.assign(Buffer);
}

void RTCC::EMGGPCHR(double lat, double lng, double alt, int body, Station *stat)
{
	stat->lat = lat;
	stat->lng = lng;
	stat->alt = alt;
	if (body == BODY_EARTH)
	{
		stat->rad = alt + OrbMech::R_Earth;
	}
	else
	{
		stat->rad = alt + OrbMech::R_Moon;
	}
}

void RTCC::EMMDYNEL(EphemerisData sv, TimeConstraintsTable &tab)
{
	VECTOR3 H, E, N, K;
	double mu, eps, v, r, lng, lat, fpa, azi, h;

	if (sv.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	OrbMech::rv_from_adbar(sv.R, sv.V, r, v, lng, lat, fpa, azi);
	eps = v * v / 2.0 - mu / r;
	H = crossp(sv.R, sv.V);
	E = crossp(sv.V, H) / mu - sv.R / r;
	K = _V(0, 0, 1);
	N = crossp(K, H);
	if (sv.RBI == BODY_EARTH)
	{
		h = r - OrbMech::R_Earth;
	}
	else
	{
		h = r - MCSMLR;
	}

	tab.a = -mu / (2.0*eps);
	tab.AoP = acos2(dotp(unit(N), unit(E)));
	if (E.z < 0)
	{
		tab.AoP = PI2 - tab.AoP;
	}
	tab.azi = azi;
	tab.e = length(E);
	tab.gamma = PI05 - fpa;
	tab.h = h;
	tab.i = acos(H.z / length(H));
	tab.l = pow(length(H), 2) / mu;
	tab.lat = lat;
	tab.lng = lng;
	tab.RA = acos2(N.z / length(N));
	if (N.y < 0)
	{
		tab.RA = PI2 - tab.RA;
	}
	tab.sv_present = sv;
	tab.T0 = OrbMech::period(sv.R, sv.V, mu);
	tab.TA = acos2(dotp(unit(E), unit(sv.R)));
	if (dotp(sv.R, sv.V) < 0)
	{
		tab.TA = PI2 - tab.TA;
	}	
	tab.V = v;
}

void RTCC::EMMRMD(int Veh1, int Veh2, double get, double dt, int refs, int axis, int mode, VECTOR3 Att, double PYRGET)
{
	double gmt_cur = RTCCPresentTimeGMT();
	double get_cur = GETfromGMT(gmt_cur);
	EZRMDT.error = "";
	if (get < get_cur)
	{
		EZRMDT.error = "MED OUTDATED";
		return;
	}
	double gmt = GMTfromGET(get);
	OrbitEphemerisTable *ChaTab, *TgtTab;
	MissionPlanTable *ChaMPT, *TgtMPT, *CSMMPT, *LMMPT;
	REFSMMATData refsdata;
	VECTOR3 X_P, Y_P, Z_P, X_B, Y_B, Z_B, X_B_selen, Y_B_selen, Z_B_selen;

	CSMMPT = &PZMPTCSM;
	LMMPT = &PZMPTLEM;
	if (Veh1 == 1)
	{
		ChaTab = &EZEPH1;
		ChaMPT = &PZMPTCSM;
		refsdata = EZJGMTX1.data[refs - 1];
	}
	else
	{
		ChaTab = &EZEPH2;
		ChaMPT = &PZMPTLEM;
		refsdata = EZJGMTX3.data[refs - 1];
	}
	if (Veh2 == 1)
	{
		TgtTab = &EZEPH1;
		TgtMPT = &PZMPTCSM;
	}
	else
	{
		TgtTab = &EZEPH2;
		TgtMPT = &PZMPTLEM;
	}
	if (ChaTab->EPHEM.Header.TUP == 0 || TgtTab->EPHEM.Header.TUP == 0)
	{
		EZRMDT.error = "TIME NOT IN EPHEM";
		return;
	}
	if (gmt < ChaTab->EPHEM.Header.TL && gmt < TgtTab->EPHEM.Header.TL)
	{
		EZRMDT.error = "TIME NOT IN EPHEM";
		return;
	}
	if (ChaTab->EPHEM.Header.TUP < 0 || TgtTab->EPHEM.Header.TUP < 0)
	{
		EZRMDT.error = "UPDATE IN PROCESS";
		return;
	}
	if (ChaTab->EPHEM.Header.TUP != ChaMPT->CommonBlock.TUP)
	{
		EZRMDT.error = "INCONSISTENT TABLES";
		return;
	}
	if (TgtTab->EPHEM.Header.TUP != TgtMPT->CommonBlock.TUP)
	{
		EZRMDT.error = "INCONSISTENT TABLES";
		return;
	}
	if (mode == 2)
	{
		EphemerisDataTable EPHEM;
		ManeuverTimesTable MANTIMES;
		LunarStayTimesTable LUNSTAY;
		ELFECH(gmt, 1, 0, RTCC_MPT_LM, EPHEM, MANTIMES, LUNSTAY);

		if (gmt < LUNSTAY.LunarStayBeginGMT || gmt >LUNSTAY.LunarStayEndGMT)
		{
			EZRMDT.error = "TIME NOT WITHIN LUNAR STAY PERIOD";
			return;
		}
	}
	EZRMDT.CSMSTAID = CSMMPT->StationID;
	EZRMDT.LMSTAID = LMMPT->StationID;
	EZRMDT.CSMGMTV = CSMMPT->GMTAV;
	EZRMDT.CSMGETV = GETfromGMT(EZRMDT.CSMGMTV);
	EZRMDT.LMGMTV = LMMPT->GMTAV;
	EZRMDT.LMGETV = GETfromGMT(EZRMDT.LMGMTV);
	if (mode == 1)
	{
		EZRMDT.Mode = '1';
		EZRMDT.PETorSH = "/PET";
		EZRMDT.YDotorT = "/Y";
	}
	else
	{
		EZRMDT.Mode = '2';
		EZRMDT.PETorSH = "/SH";
		EZRMDT.YDotorT = "/T";

		MATRIX3 M_B;
		double PYRGMT;

		PYRGMT = GMTfromGET(PYRGET);
		M_B = GLMRTM(refsdata.REFSMMAT, Att.x, 2, Att.z, 3, Att.y, 1);
		ELVCNV(_V(M_B.m11, M_B.m12, M_B.m13), PYRGMT, 2, 3, X_B_selen);
		ELVCNV(_V(M_B.m21, M_B.m22, M_B.m23), PYRGMT, 2, 3, Y_B_selen);
		ELVCNV(_V(M_B.m31, M_B.m32, M_B.m33), PYRGMT, 2, 3, Z_B_selen);
	}
	char Buffer[7];

	FormatREFSMMATCode(refs, refsdata.ID, Buffer);
	EZRMDT.REFSMMAT.assign(Buffer);

	if (axis == 1)
	{
		EZRMDT.AXIS = 'X';
	}
	else
	{
		EZRMDT.AXIS = 'Z';
	}
	if (Veh1 == 1)
	{
		EZRMDT.Pitch = 'I';
		EZRMDT.Yaw = 'M';
		EZRMDT.Roll = 'O';
		EZRMDT.VEH = "CSM";
	}
	else
	{
		EZRMDT.Pitch = 'I';
		EZRMDT.Yaw = 'O';
		EZRMDT.Roll = 'M';
		EZRMDT.VEH = "LM";
	}
	EZRMDT.GETR = MCGREF * 3600.0;
	X_P = _V(refsdata.REFSMMAT.m11, refsdata.REFSMMAT.m12, refsdata.REFSMMAT.m13);
	Y_P = _V(refsdata.REFSMMAT.m21, refsdata.REFSMMAT.m22, refsdata.REFSMMAT.m23);
	Z_P = _V(refsdata.REFSMMAT.m31, refsdata.REFSMMAT.m32, refsdata.REFSMMAT.m33);
	EZRMDT.solns = 0;
	ELVCTRInputTable intab;
	ELVCTROutputTable outtab;
	EphemerisData sv_cha, sv_tgt;
	double r_c, r_t, theta1, x, y, z, sin_phi, A, rho, rho_dot, E, P, Y, R, P_apo, Y_apo, R_apo;
	VECTOR3 R_c_u, R_t_u, H_c_u, H_t_u, Rho, Rho_dot;
	RelativeMotionDigitalsTableEntry empty;
	for (int i = 0;i < 11;i++)
	{
		EZRMDT.data[i] = empty;
	}
	for (int i = 0;i < 11;i++)
	{
		intab.GMT = gmt;
		intab.L = Veh1;
		ELVCTR(intab, outtab);
		if (outtab.ErrorCode)
		{
			gmt += dt;
			continue;
		}
		sv_cha = outtab.SV;
		intab.GMT = gmt;
		intab.L = Veh2;
		ELVCTR(intab, outtab);
		if (outtab.ErrorCode)
		{
			gmt += dt;
			continue;
		}
		sv_tgt = outtab.SV;

		EZRMDT.data[EZRMDT.solns].GET = GETfromGMT(gmt);
		
		r_c = length(sv_cha.R);
		R_c_u = unit(sv_cha.R);
		H_c_u = unit(crossp(sv_cha.R, sv_cha.V));
		r_t = length(sv_tgt.R);
		R_t_u = unit(sv_tgt.R);
		H_t_u = unit(crossp(sv_tgt.R, sv_tgt.V));
		theta1 = atan2(dotp(H_t_u, crossp(R_t_u, R_c_u)), dotp(R_c_u, R_t_u));
		sin_phi = dotp(H_t_u, R_c_u);
		x = abs(r_t*theta1);
		y = abs(r_c*sin_phi);
		A = atan2(dotp(-H_c_u, R_t_u), dotp(H_c_u, crossp(R_c_u, R_t_u)));
		if (A < 0)
		{
			A += PI2;
		}
		Rho = sv_tgt.R - sv_cha.R;
		rho = length(Rho);
		if (rho < 1852.0*1e-10)
		{
			gmt += dt;
			EZRMDT.solns++;
			continue;
		}
		Rho_dot = sv_tgt.V - sv_cha.V;
		rho_dot = dotp(Rho, Rho_dot) / rho;
		E = asin(dotp(Rho, sv_cha.R) / rho / r_c);
		if (E < 0)
		{
			E += PI2;
		}
		z = abs(r_t - r_c);

		if (mode == 1)
		{
			EZRMDT.data[EZRMDT.solns].PETorShaft = abs(EZRMDT.data[EZRMDT.solns].GET - EZRMDT.GETR);
			EZRMDT.data[EZRMDT.solns].YdotorTrun = dotp(sv_cha.V, unit(crossp(sv_tgt.V, sv_tgt.R))) / 0.3048;
		}
		else
		{
			ELVCNV(X_B_selen, gmt, 3, 2, X_B);
			ELVCNV(Y_B_selen, gmt, 3, 2, Y_B);
			ELVCNV(Z_B_selen, gmt, 3, 2, Z_B);

			VECTOR3 Rho_u;
			double Trun, Shaft;

			Rho_u = unit(Rho);
			Trun = asin(dotp(-Rho_u, Y_B));
			if (Trun < 0)
			{
				Trun += PI2;
			}
			Shaft = atan2(dotp(Rho_u, X_B), dotp(Rho_u, Z_B));
			if (Shaft < 0)
			{
				Shaft += PI2;
			}

			EZRMDT.data[EZRMDT.solns].PETorShaft = Shaft*DEG;
			EZRMDT.data[EZRMDT.solns].YdotorTrun = Trun*DEG;
		}

		EZRMDT.data[EZRMDT.solns].R = rho / 1852.0;
		EZRMDT.data[EZRMDT.solns].RDOT = rho_dot / 0.3048;
		EZRMDT.data[EZRMDT.solns].AZH = A * DEG;
		if (EZRMDT.data[EZRMDT.solns].AZH >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].AZH = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].EL = E * DEG;
		EZRMDT.data[EZRMDT.solns].X = x / 1852.0;
		EZRMDT.data[EZRMDT.solns].Y = y / 1852.0;
		EZRMDT.data[EZRMDT.solns].Z = z / 1852.0;

		if (theta1 >= 0)
		{
			EZRMDT.data[EZRMDT.solns].XInd = 'L';
		}
		else
		{
			EZRMDT.data[EZRMDT.solns].XInd = 'T';
		}
		if (sin_phi >= 0)
		{
			EZRMDT.data[EZRMDT.solns].YInd = 'L';
		}
		else
		{
			EZRMDT.data[EZRMDT.solns].YInd = 'R';
		}
		if (r_t - r_c >= 0)
		{
			EZRMDT.data[EZRMDT.solns].ZInd = 'B';
		}
		else
		{
			EZRMDT.data[EZRMDT.solns].ZInd = 'A';
		}
		if (Veh1 == 1)
		{
			X_B = unit(Rho);
			//Y_B = unit(crossp(X_B, R_c_u));
			Y_B = unit(crossp(R_t_u, R_c_u));
			Z_B = crossp(X_B, Y_B);
			RLMPYR(X_P, Y_P, Z_P, X_B, Y_B, Z_B, P, Y, R);
			P_apo = P;
			Y_apo = Y;
			R_apo = R;
		}
		else
		{
			if (axis == 1)
			{
				X_B = unit(Rho);
				//Y_B = unit(crossp(X_B, R_c_u));
				Y_B = unit(crossp(R_t_u, R_c_u));
				Z_B = crossp(X_B, Y_B);
			}
			else
			{
				Z_B = unit(Rho);
				//Y_B = unit(crossp(Z_B, R_c_u));
				Y_B = unit(crossp(R_t_u,R_c_u));
				X_B = crossp(Y_B, Z_B);
			}
			EMGLMRAT(X_P, Y_P, Z_P, X_B, Y_B, Z_B, P, Y, R, P_apo, Y_apo, R_apo);
		}
		EZRMDT.data[EZRMDT.solns].Pitch = P * DEG;
		if (EZRMDT.data[EZRMDT.solns].Pitch >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].Pitch = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].Yaw = Y * DEG;
		if (EZRMDT.data[EZRMDT.solns].Yaw >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].Yaw = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].Roll = R * DEG;
		if (EZRMDT.data[EZRMDT.solns].Roll >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].Roll = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].PB = P_apo * DEG;
		if (EZRMDT.data[EZRMDT.solns].PB >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].PB = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].YB = Y_apo * DEG;
		if (EZRMDT.data[EZRMDT.solns].YB >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].YB = 0.0;
		}
		EZRMDT.data[EZRMDT.solns].RB = R_apo * DEG;
		if (EZRMDT.data[EZRMDT.solns].RB >= 359.95)
		{
			EZRMDT.data[EZRMDT.solns].RB = 0.0;
		}
		gmt += dt;
		EZRMDT.solns++;
	}
}

void RTCC::EMSTIME(int L, int ID)
{
	//Twelve-Second Time trap
	if (ID == 1)
	{
		ELVCTRInputTable in;
		ELVCTROutputTable out;
		TimeConstraintsTable *tab;

		if (L == 1)
		{
			tab = &EZTSCNS1;
		}
		else
		{
			tab = &EZTSCNS3;
		}

		in.GMT = RTCCPresentTimeGMT();
		in.L = L;

		ELVCTR(in, out);

		if (out.ErrorCode)
		{
			tab->TUP = 0;
			return;
		}

		int coor1, coor2;
		if (out.SV.RBI == BODY_EARTH)
		{
			coor1 = 0;
			coor2 = 1;
		}
		else
		{
			coor1 = 2;
			coor2 = 3;
		}

		EphemerisData sv_true;
		int err = ELVCNV(out.SV, coor1, coor2, sv_true);
		if (err)
		{
			tab->TUP = 0;
			return;
		}

		EMMDYNEL(sv_true, *tab);

		tab->RevNum = CapeCrossingRev(L, sv_true.GMT);
		PLAWDT(L, sv_true.GMT, tab->WT);
		tab->TUP = out.TUP;
	}
}

void RTCC::EMSNAP(int L, int ID)
{
	//Trajectory Update
	if (ID == 1)
	{
		EMMDYNMC(L, 2);
	}
	//MPT update without trajectory update
	else if (ID == 2)
	{
		PMDMPT();
	}
	//Update MPT and DMT displays as a result of a fuel change or maneuver execution
	else if (ID == 7)
	{
		PMDMPT();
		PMDDMT(EZETVMED.DMT1Vehicle, EZETVMED.DMT1Number, EZETVMED.DMT1REFSMMATCode, EZETVMED.DMT1HeadsUpDownIndicator, DMTBuffer[0]);
		PMDDMT(EZETVMED.DMT2Vehicle, EZETVMED.DMT2Number, EZETVMED.DMT2REFSMMATCode, EZETVMED.DMT2HeadsUpDownIndicator, DMTBuffer[1]);
	}
	//Two-Impulse Multiple Solution Display
	else if (ID == 63)
	{
		PMDTIMP();
	}
}

void RTCC::EMMDYNMC(int L, int queid, int ind, double param)
{
	//Queue IDs:
	//1 = 12 second update
	//2 = Trajectory Update
	//3 = MED U12
	//4 = MED U13
	//5 = MED U14

	FIDOOrbitDigitals *tab;
	TimeConstraintsTable *tcontab;
	HistoryAnchorVectorTable *anctab;
	MissionPlanTable *mpt;
	OrbitEphemerisTable *ephtab;
	if (L == 1)
	{
		tab = &EZSAVCSM;
		tcontab = &EZTSCNS1;
		anctab = &EZANCHR1;
		mpt = &PZMPTCSM;
		ephtab = &EZEPH1;
	}
	else
	{
		tab = &EZSAVLEM;
		tcontab = &EZTSCNS3;
		anctab = &EZANCHR3;
		mpt = &PZMPTLEM;
		ephtab = &EZEPH2;
	}

	double CurGET, CurGMT, R_B;

	CurGMT = RTCCPresentTimeGMT();
	CurGET = GETfromGMT(CurGMT);

	if (queid == 1 || queid == 2)
	{
		//TBD: move this
		EMSTIME(L, 1);
	}

	//For now don't even allow processing if time constraints table is invalid
	if (tcontab->TUP <= 0)
	{
		return;
	}

	if (tcontab->sv_present.RBI == BODY_EARTH)
	{
		R_B = OrbMech::R_Earth;
	}
	else
	{
		R_B = MCSMLR;
	}

	if (queid == 1)
	{
		tab->REV = tcontab->RevNum;
		tab->GET = GETfromGMT(tcontab->sv_present.GMT);
		tab->TO = tcontab->T0;
		if (tcontab->sv_present.RBI == BODY_EARTH)
		{
			sprintf_s(tab->REF, "EARTH");
		}
		else
		{
			sprintf_s(tab->REF, "LUNAR");
		}
		tab->ORBWT = tcontab->WT*1000.0*LBS;
	}

	if (queid == 2)
	{
		sprintf_s(tab->STAID, tcontab->StationID.c_str());
		tab->GMTID = anctab->AnchorVectors[9].GMT;
		tab->GETID = GETfromGMT(anctab->AnchorVectors[9].GMT);
		if (L == 1)
		{
			sprintf_s(tab->VEHID, "CSM");
		}
		else
		{
			sprintf_s(tab->VEHID, "LEM");
		}
	}

	if (queid == 2 || (tcontab->lat > 0 && tab->PPP < 0))
	{
		double lng;
		int err = RMMASCND(L, CurGMT, lng);
		if (err)
		{
			EMGPRINT("EMMDYNMC", 29);
			tab->LNPP = 0.0;
		}
		else
		{
			tab->LNPP = lng * DEG;
		}
	}

	if (queid == 1 || queid == 2)
	{
		tab->LPP = tcontab->lng*DEG;
		tab->PPP = tcontab->lat*DEG;
		tab->GETCC = GETfromGMT(CapeCrossingGMT(L, tcontab->RevNum + 1));
		tab->TAPP = tcontab->TA*DEG;
		tab->H = tcontab->h / 1852.0;
		tab->V = tcontab->V / 0.3048;
		tab->GAM = tcontab->gamma*DEG;
		tab->A = tcontab->a / 1852.0;
		tab->E = tcontab->e;
		tab->I = tcontab->i*DEG;

		if (queid == 2 || (CurGET > tab->GETA))
		{
			if (tcontab->e > 0.85)
			{
				tab->HA = 0.0;
				tab->PA = 0.0;
				tab->LA = 0.0;
				tab->GETA = 0.0;
			}
			else
			{
				SV sv_A, sv_a, sv_p;

				sv_A.R = tcontab->sv_present.R;
				sv_A.V = tcontab->sv_present.V;
				sv_A.MJD = OrbMech::MJDfromGET(tcontab->sv_present.GMT, GMTBASE);
				sv_A.gravref = GetGravref(tcontab->sv_present.RBI);
				PMMAPD(sv_A, sv_a, sv_p);

				EphemerisData sv_apo, sv_true;
				double lat, lng;

				sv_apo.R = sv_a.R;
				sv_apo.V = sv_a.V;
				sv_apo.GMT = OrbMech::GETfromMJD(sv_a.MJD, GMTBASE);
				sv_apo.RBI = tcontab->sv_present.RBI;

				int in, out;
				if (sv_apo.RBI == BODY_EARTH)
				{
					in = 0;
					out = 1;
				}
				else
				{
					in = 2;
					out = 3;
				}

				ELVCNV(sv_apo, in, out, sv_true);

				OrbMech::latlong_from_r(sv_true.R, lat, lng);
				tab->HA = (length(sv_true.R) - R_B) / 1852.0;
				tab->PA = lat * DEG;
				tab->LA = lng * DEG;
				tab->GETA = GETfromGMT(sv_true.GMT);
			}
		}

		if (queid == 2 || (CurGET > tab->GETP))
		{
			if (tcontab->e > 0.85)
			{
				tab->HP = 0.0;
				tab->PP = 0.0;
				tab->LP = 0.0;
				tab->GETP = 0.0;
			}
			else
			{
				SV sv_A, sv_a, sv_p;

				sv_A.R = tcontab->sv_present.R;
				sv_A.V = tcontab->sv_present.V;
				sv_A.MJD = OrbMech::MJDfromGET(tcontab->sv_present.GMT, GMTBASE);
				sv_A.gravref = GetGravref(tcontab->sv_present.RBI);
				PMMAPD(sv_A, sv_a, sv_p);

				EphemerisData sv_peri, sv_true;
				double lat, lng;

				sv_peri.R = sv_p.R;
				sv_peri.V = sv_p.V;
				sv_peri.GMT = OrbMech::GETfromMJD(sv_p.MJD, GMTBASE);
				sv_peri.RBI = tcontab->sv_present.RBI;

				int in, out;
				if (sv_peri.RBI == BODY_EARTH)
				{
					in = 0;
					out = 1;
				}
				else
				{
					in = 2;
					out = 3;
				}

				ELVCNV(sv_peri, in, out, sv_true);

				OrbMech::latlong_from_r(sv_true.R, lat, lng);
				tab->HP = (length(sv_true.R) - R_B) / 1852.0;
				tab->PP = lat * DEG;
				tab->LP = lng * DEG;
				tab->GETP = GETfromGMT(sv_true.GMT);
			}
		}
	}

	//Clear manual request data upon trajectory update
	if (queid == 2)
	{
		tab->HPR = 0.0;
		tab->PPR = 0.0;
		tab->LPR = 0.0;
		tab->GETPR = 0.0;
		tab->HAR = 0.0;
		tab->PAR = 0.0;
		tab->LAR = 0.0;
		tab->GETAR = 0.0;
		tab->REVL = 0;
		tab->GETL = 0.0;
		tab->L = 0.0;
	}

	//U12: Predicted apogee and perigee
	if (queid == 3)
	{
		EphemerisData sv_pred;
		if (ind == 1 || ind == 2)
		{
			double GET;
			if (ind == 1)
			{
				int rev = (int)param;
				GET = GETfromGMT(CapeCrossingGMT(L, rev));
				if (GET < 0)
				{
					return;
				}
			}
			else
			{
				GET = param;
			}
			ELVCTRInputTable intab;
			ELVCTROutputTable outtab;
			intab.GMT = GMTfromGET(GET);
			intab.L = L;
			ELVCTR(intab, outtab);
			if (outtab.ErrorCode)
			{
				return;
			}
			sv_pred = outtab.SV;
			tab->NV2 = outtab.ORER + 1;
		}
		else
		{
			unsigned man = (unsigned)param;
			if (man > mpt->mantable.size())
			{
				return;
			}
			sv_pred.R = mpt->mantable[man - 1].R_BO;
			sv_pred.V = mpt->mantable[man - 1].V_BO;
			sv_pred.GMT = mpt->mantable[man - 1].GMT_BO;
			sv_pred.RBI = mpt->mantable[man - 1].RefBodyInd;
			tab->NV2 = 0;
		}

		if (sv_pred.RBI == BODY_EARTH)
		{
			sprintf_s(tab->REFR, "EARTH");
		}
		else
		{
			sprintf_s(tab->REFR, "LUNAR");
		}

		tab->REVR = CapeCrossingRev(L, sv_pred.GMT);
		tab->GETBV = GETfromGMT(sv_pred.GMT);

		SV sv_A, sv_a, sv_p;

		sv_A.R = sv_pred.R;
		sv_A.V = sv_pred.V;
		sv_A.MJD = OrbMech::MJDfromGET(sv_pred.GMT, GMTBASE);
		sv_A.gravref = GetGravref(sv_pred.RBI);
		PMMAPD(sv_A, sv_a, sv_p);

		EphemerisData sv_apo, sv_peri, sv_true;
		double lat, lng;

		sv_apo.R = sv_a.R;
		sv_apo.V = sv_a.V;
		sv_apo.GMT = OrbMech::GETfromMJD(sv_a.MJD, GMTBASE);
		sv_apo.RBI = tcontab->sv_present.RBI;

		int in, out;
		if (sv_pred.RBI == BODY_EARTH)
		{
			in = 0;
			out = 1;
		}
		else
		{
			in = 2;
			out = 3;
		}

		ELVCNV(sv_apo, in, out, sv_true);

		OrbMech::latlong_from_r(sv_true.R, lat, lng);
		tab->HAR = (length(sv_true.R) - R_B) / 1852.0;
		tab->PAR = lat * DEG;
		tab->LAR = lng * DEG;
		tab->GETAR = GETfromGMT(sv_true.GMT);

		sv_peri.R = sv_p.R;
		sv_peri.V = sv_p.V;
		sv_peri.GMT = OrbMech::GETfromMJD(sv_p.MJD, GMTBASE);
		sv_peri.RBI = tcontab->sv_present.RBI;

		ELVCNV(sv_peri, in, out, sv_true);

		OrbMech::latlong_from_r(sv_true.R, lat, lng);
		tab->HPR = (length(sv_true.R) - R_B) / 1852.0;
		tab->PPR = lat * DEG;
		tab->LPR = lng * DEG;
		tab->GETPR = GETfromGMT(sv_true.GMT);
	}
	//U13: Longitude crossing time
	else if (queid == 4)
	{
		int rev = ind;
		double lng = param;
		double GET = GETfromGMT(CapeCrossingGMT(L, rev));
		if (GET < 0)
		{
			GET = CurGET;
		}

		double GMT_cross;
		RLMTLC(ephtab->EPHEM, ephtab->MANTIMES, lng, GMTfromGET(GET), GMT_cross);
		tab->GETL = GETfromGMT(GMT_cross);
		tab->REVL = rev;
		tab->L = lng * DEG;
	}
	//U14: Compute longitude at given time
	else if (queid == 5)
	{
		double gmt = GMTfromGET(param);

		ELVCTRInputTable intab;
		ELVCTROutputTable outtab;

		intab.GMT = gmt;
		intab.L = L;

		ELVCTR(intab, outtab);
		if (outtab.ErrorCode)
		{
			return;
		}
		int in, out;
		if (outtab.SV.RBI == BODY_EARTH)
		{
			in = 0;
			out = 1;
		}
		else
		{
			in = 2;
			out = 3;
		}
		EphemerisData sv_true;
		ELVCNV(outtab.SV, in, out, sv_true);
		double lat, lng;
		OrbMech::latlong_from_r(sv_true.R, lat, lng);
		tab->GETL = param;
		tab->L = lng * DEG;
	}
}

//Space Digitals
int RTCC::EMDSPACE(int queid, int option, double val, double incl, double ascnode)
{
	//queid:
	//1 = Time cylce (12 sec)
	//2 = Trajectory Update
	//3 = MED Column 1
	//4 = MED Column 2
	//5 = MED Column 3
	//6 = MSK request
	//7 = Reinitialization

	EZSPACE.errormessage = "";

	if (EZETVMED.SpaceDigVehID < 0)
	{
		//Error: The display has not been initialized via MED U00
		EZSPACE.errormessage = "Error 1";
		return 1;
	}

	OrbitEphemerisTable *ephtab;
	MissionPlanTable *mpt;
	TimeConstraintsTable *tctab;

	if (EZETVMED.SpaceDigVehID == RTCC_MPT_CSM)
	{
		ephtab = &EZEPH1;
		mpt = &PZMPTCSM;
		tctab = &EZTSCNS1;
	}
	else
	{
		ephtab = &EZEPH2;
		mpt = &PZMPTLEM;
		tctab = &EZTSCNS3;
	}

	if (ephtab->EPHEM.Header.TUP == 0)
	{
		//Error: The trajectory for subject vehicle is not generated
		EZSPACE.errormessage = "Error 2";
		return 2;
	}
	if (ephtab->EPHEM.Header.TUP < 0)
	{
		//Error: The main ephemeris is in an update status
		EZSPACE.errormessage = "Error 5";
		return 5;
	}
	if (mpt->CommonBlock.TUP < 0 || mpt->CommonBlock.TUP != ephtab->EPHEM.Header.TUP)
	{
		//Error: The MPT is either in an update state or is inconsistent with the trajectory update number
		EZSPACE.errormessage = "Error 6";
		return 6;
	}

	if (queid == 1)
	{
		//TBD: Move this
		EMSTIME(EZETVMED.SpaceDigVehID, 1);

		if (tctab->TUP > 0)
		{
			EZSPACE.GET = GETfromGMT(tctab->sv_present.GMT);
		}
		else
		{
			EZSPACE.GET = 0.0;
		}
	}
	else
	{
		if (EZETVMED.SpaceDigVehID == 1)
		{
			sprintf_s(EZSPACE.VEHID, "CSM");
		}
		else
		{
			sprintf_s(EZSPACE.VEHID, "LEM");
		}
	}

	if (queid == 1 || queid == 2 || queid == 6 || queid == 7)
	{
		double cfg_weight;

		EZSPACE.TUP = ephtab->EPHEM.Header.TUP;
		sprintf_s(EZSPACE.VecID, mpt->StationID.c_str());
		if (tctab->TUP > 0)
		{
			cfg_weight = tctab->WT;
		}
		else
		{
			cfg_weight = 0.0;
		}
		EZSPACE.WEIGHT = cfg_weight * LBS*1000.0;
		EZSPACE.GMTV = mpt->GMTAV;
		EZSPACE.GETV = EZSPACE.GETAxis = GETfromGMT(mpt->GMTAV);

		if (tctab->TUP > 0)
		{
			if (tctab->sv_present.RBI == BODY_EARTH)
			{
				sprintf(EZSPACE.REF, "EARTH");
			}
			else
			{
				sprintf(EZSPACE.REF, "MOON");
			}
			EZSPACE.V = tctab->V / 0.3048;
			EZSPACE.GAM = tctab->gamma*DEG;
			EZSPACE.H = tctab->h / 1852.0;
			EZSPACE.PHI = tctab->lat*DEG;
			EZSPACE.LAM = tctab->lng*DEG;
			EZSPACE.PSI = tctab->azi*DEG;
			EZSPACE.ADA = tctab->TA*DEG;
		}
		else
		{
			EZSPACE.V = 0.0;
			EZSPACE.GAM = 0.0;
			EZSPACE.H = 0.0;
			EZSPACE.PHI = 0.0;
			EZSPACE.LAM = 0.0;
			EZSPACE.PSI = 0.0;
			EZSPACE.ADA = 0.0;
		}

		EZSPACE.GETR = EZSPACE.GET - MCGREF * 3600.0;
	}
	else
	{
		EphemerisData sv;
		//GET
		if (option == 0)
		{
			double GMT = GMTfromGET(val);
			if (ELFECH(GMT, EZETVMED.SpaceDigVehID, sv))
			{
				if (GMT > ephtab->EPHEM.Header.TR)
				{
					//Error: The time on the U01 exceeds the start of the ephemeris
					EZSPACE.errormessage = "Error 3";
					return 3;
				}
				else
				{
					//Error: The time on the U01 precedes the start of the ephemeris
					EZSPACE.errormessage = "Error 4";
					return 4;
				}
			}
			if (queid == 3)
			{
				EMSMISSInputTable emsin;
				sv = EMMENI(emsin, sv, GMT - sv.GMT);
			}
		}
		//MNV
		else
		{
			unsigned mnv = (unsigned)val;
			if (mnv > mpt->mantable.size())
			{
				//Error: The maneuver number requested is greater than the nmber of maneuvers in the Mission Plan Table
				return 8;
			}
			sv.R = mpt->mantable[mnv - 1].R_BO;
			sv.V = mpt->mantable[mnv - 1].V_BO;
			sv.GMT = mpt->mantable[mnv - 1].GMT_BO;
			sv.RBI = mpt->mantable[mnv - 1].RefBodyInd;
		}

		//Column 1
		if (queid == 3)
		{
			EphemerisData sv_EMP, sv_true;
			TimeConstraintsTable newtab;
			double cfg_weight, R_E, mu;

			EZSPACE.GETVector1 = GETfromGMT(sv.GMT);
			if (sv.RBI == BODY_EARTH)
			{
				sprintf(EZSPACE.REF1, "E");
				mu = OrbMech::mu_Earth;
			}
			else
			{
				sprintf(EZSPACE.REF1, "M");
				mu = OrbMech::mu_Moon;
			}
			
			PLAWDT(EZETVMED.SpaceDigVehID, sv.GMT, cfg_weight);
			EZSPACE.WT = cfg_weight * LBS*1000.0;

			int csi_in, csi_out;
			if (sv.RBI == BODY_EARTH)
			{
				csi_in = 0;
				csi_out = 1;
				R_E = OrbMech::R_Earth;
			}
			else
			{
				csi_in = 2;
				csi_out = 3;
				R_E = MCSMLR;
			}
			
			EZSPACE.H1 = EZSPACE.HS = EZSPACE.HO = (length(sv.R) - R_E) / 1852.0;
			ELVCNV(sv, csi_in, 4, sv_EMP);
			ELVCNV(sv, csi_in, csi_out, sv_true);
			
			EMMDYNEL(sv_EMP, newtab);
			EZSPACE.IEMP = newtab.i*DEG;

			EMMDYNEL(sv_true, newtab);
			EZSPACE.V1 = newtab.V / 0.3048;
			EZSPACE.PHI1 = EZSPACE.PHIO = newtab.lat * DEG;
			EZSPACE.LAM1 = newtab.lng*DEG;
			EZSPACE.GAM1 = newtab.gamma*DEG;
			EZSPACE.PSI1 = newtab.azi*DEG;
			EZSPACE.A1 = newtab.a / 1852.0;
			EZSPACE.L1 = (newtab.TA + newtab.AoP)*DEG;
			if (EZSPACE.L1 > 360.0)
			{
				EZSPACE.L1 -= 360.0;
			}
			EZSPACE.E1 = newtab.e;
			EZSPACE.I1 = newtab.i*DEG;

			if (newtab.e < 0.85)
			{
				SV sv0, sv_a, sv_p;
				sv0.R = sv.R;
				sv0.V = sv.V;
				sv0.MJD = OrbMech::MJDfromGET(sv.GMT, GMTBASE);
				sv0.gravref = GetGravref(sv.RBI);
				PMMAPD(sv0, sv_a, sv_p);

				EZSPACE.GETA = OrbMech::GETfromMJD(sv_a.MJD, CalcGETBase());
				EZSPACE.HA = (length(sv_a.R) - R_E) / 1852.0;
				EZSPACE.HP = (length(sv_p.R) - R_E) / 1852.0;
			}
			else
			{
				double peri, apo;
				OrbMech::periapo(sv.R, sv.V, mu, apo, peri);
				EZSPACE.HA = (apo - R_E) / 1852.0;
				if (EZSPACE.HA > 999999.9)
				{
					EZSPACE.HA = 999999.9;
				}
				EZSPACE.HP = (peri - R_E) / 1852.0;
				EZSPACE.GETA = 0.0;
			}
		}
		//Column 2
		else if (queid == 4)
		{
			//Null all values
			EZSPACE.GETSI = 0.0;
			EZSPACE.GETCA = 0.0;
			EZSPACE.VCA = 0.0;
			EZSPACE.HCA = 0.0;
			EZSPACE.PCA = 0.0;
			EZSPACE.LCA = 0.0;
			EZSPACE.PSICA = 0.0;
			EZSPACE.GETMN = 0.0;
			EZSPACE.HMN = 0.0;
			EZSPACE.PMN = 0.0;
			EZSPACE.LMN = 0.0;
			EZSPACE.DMN = 0.0;

			EZSPACE.GETVector2 = GETfromGMT(sv.GMT);

			EMSMISSInputTable emsin;

			if (sv.RBI == BODY_EARTH)
			{
				//Try to find lunar sphere entry
				emsin.CutoffIndicator = 1;
				emsin.StopParamRefFrame = 1;
				emsin.MoonRelStopParam = 9.0*OrbMech::R_Earth;

				EphemerisData sv_SI = EMMENI(emsin, sv, 10.0*24.0*3600.0);
				if (emsin.NIAuxOutputTable.TerminationCode == 1)
				{
					EZSPACE.GETSI = GETfromGMT(sv_SI.GMT);
				}
			}


			emsin.CutoffIndicator = 4;
			emsin.StopParamRefFrame = 1;
			emsin.MoonRelStopParam = 0.0;
			EphemerisData sv1 = EMMENI(emsin, sv, 10.0*24.0*3600.0);
			if (emsin.NIAuxOutputTable.TerminationCode != 4)
			{
				return 0;
			}

			EphemerisData svtempout;
			TimeConstraintsTable newtab;

			ELVCNV(sv1, 0, 1, svtempout);
			EMMDYNEL(svtempout, newtab);

			EZSPACE.GETCA = GETfromGMT(sv1.GMT);
			EZSPACE.VCA = newtab.V / 0.3048;
			EZSPACE.HCA = newtab.h / 1852.0;
			EZSPACE.PCA = newtab.lat * DEG;
			EZSPACE.LCA = newtab.lng * DEG;
			EZSPACE.PSICA = newtab.azi*DEG;
		}
		//Column 3
		else
		{
			//Null all values
			EZSPACE.GETSE = 0.0;
			EZSPACE.GETEI = 0.0;
			EZSPACE.VEI = 0.0;
			EZSPACE.GEI = 0.0;
			EZSPACE.PEI = 0.0;
			EZSPACE.LEI = 0.0;
			EZSPACE.PSIEI = 0.0;
			EZSPACE.GETVP = 0.0;
			EZSPACE.VVP = 0.0;
			EZSPACE.HVP = 0.0;
			EZSPACE.PVP = 0.0;
			EZSPACE.LVP = 0.0;
			EZSPACE.PSIVP = 0.0;

			double mu;
			bool HighOrbit;

			EZSPACE.GETVector3 = GETfromGMT(sv.GMT);
			if (sv.RBI == BODY_EARTH)
			{
				mu = OrbMech::mu_Earth;
			}
			else
			{
				mu = OrbMech::mu_Moon;
			}

			OELEMENTS coe = OrbMech::coe_from_sv(sv.R, sv.V, mu);
			double SMA = OrbMech::GetSemiMajorAxis(sv.R, sv.V, mu);

			if (coe.e > MCGECC)
			{
				HighOrbit = true;
			}
			else
			{
				if (SMA < 0 || SMA > MCGSMA*OrbMech::R_Earth)
				{
					HighOrbit = true;
				}
				else
				{
					HighOrbit = false;
				}
			}

			if (HighOrbit == false)
			{
				return 0;
			}

			//sv1: "now"
			//sv2: pericynthion
			//sv3: lunar sphere exit
			//sv4: vacuum perigee
			//sv5: entry interface
			EphemerisData sv2, sv3, sv4, sv5;
			PMMCEN_VNI vni;
			PMMCEN_INI ini;
			int ITS;

			vni.GMTBASE = GMTBASE;

			if (sv.RBI == BODY_EARTH)
			{
				//Integrate to next periapsis
				vni.R = sv.R;
				vni.V = sv.V;
				vni.T = sv.GMT;
				ini.body = sv.RBI;
				vni.end_cond = 0.0;
				ini.stop_ind = 2;
				vni.dt_min = 0.0;
				OrbMech::PMMCEN(vni, ini, sv4.R, sv4.V, sv4.GMT, ITS, sv4.RBI);

				if (sv4.RBI == BODY_EARTH)
				{
					if (length(sv4.R) > OrbMech::GetSemiMajorAxis(sv4.R, sv4.V, OrbMech::mu_Earth))
					{
						//We are at apogee, try again
						vni.dt_min = 30.0*60.0;
						vni.R = sv4.R;
						vni.V = sv4.V;
						vni.T = sv4.GMT;
						ini.body = sv4.RBI;
						OrbMech::PMMCEN(vni, ini, sv4.R, sv4.V, sv4.GMT, ITS, sv4.RBI);
					}
					//Found a vacuum perigee
				}
				else
				{
					//Found a pericynthion
					sv2 = sv4;
					vni.R = sv2.R;
					vni.V = sv2.V;
					vni.T = sv2.GMT;
					ini.body = sv2.RBI;
					vni.end_cond = 9.0*OrbMech::R_Earth;
					vni.dt_min = 0.0;
					ini.stop_ind = 3;
					OrbMech::PMMCEN(vni, ini, sv3.R, sv3.V, sv3.GMT, ITS, sv3.RBI);

					EZSPACE.GETSE = GETfromGMT(sv3.GMT);

					//Find vacuum perigee
					vni.R = sv3.R;
					vni.V = sv3.V;
					vni.T = sv3.GMT;
					ini.body = sv3.RBI;
					vni.end_cond = 0.0;
					vni.dt_min = 0.0;
					ini.stop_ind = 2;
					OrbMech::PMMCEN(vni, ini, sv4.R, sv4.V, sv4.GMT, ITS, sv4.RBI);

					if (ITS == 1 || sv4.RBI == BODY_MOON)
					{
						//Not found
						return 0;
					}
				}
			}
			else
			{
				//Try to find lunar sphere exit
				vni.R = sv.R;
				vni.V = sv.V;
				vni.T = sv.GMT;
				ini.body = sv.RBI;
				vni.end_cond = 9.0*OrbMech::R_Earth;
				vni.dt_min = 0.0;
				ini.stop_ind = 3;
				OrbMech::PMMCEN(vni, ini, sv3.R, sv3.V, sv3.GMT, ITS, sv3.RBI);

				if (ITS == 1)
				{
					//Not found
					return 0;
				}
				else
				{
					EZSPACE.GETSE = GETfromGMT(sv3.GMT);
				}
				//Find vacuum perigee
				vni.R = sv3.R;
				vni.V = sv3.V;
				vni.T = sv3.GMT;
				ini.body = sv3.RBI;
				vni.end_cond = 0.0;
				vni.dt_min = 0.0;
				ini.stop_ind = 2;
				OrbMech::PMMCEN(vni, ini, sv4.R, sv4.V, sv4.GMT, ITS, sv4.RBI);

				if (ITS == 1 || sv4.RBI == BODY_MOON)
				{
					//Not found
					return 0;
				}
			}
			//Calc VP parameters
			TimeConstraintsTable newtab;
			EphemerisData svtempout;

			EZSPACE.GETVP = GETfromGMT(sv4.GMT);

			ELVCNV(sv4, 0, 1, svtempout);
			EMMDYNEL(svtempout, newtab);

			EZSPACE.VVP = newtab.V / 0.3048;
			EZSPACE.HVP = newtab.h / 1852.0;
			EZSPACE.PVP = newtab.lat * DEG;
			EZSPACE.LVP = newtab.lng * DEG;
			EZSPACE.PSIVP = newtab.azi*DEG;
			//Do we have a reentry
			if (length(sv4.R) < OrbMech::R_Earth + 400000.0*0.3048)
			{
				//Calc EI parameters
				vni.R = sv4.R;
				vni.V = sv4.V;
				vni.T = sv4.GMT;
				ini.body = sv4.RBI;
				vni.end_cond = OrbMech::R_Earth + 400000.0*0.3048;
				vni.dt_min = 0.0;
				vni.dt_max = 24.0*3600.0;
				vni.dir = -1.0;
				ini.stop_ind = 3;
				OrbMech::PMMCEN(vni, ini, sv5.R, sv5.V, sv5.GMT, ITS, sv5.RBI);

				EZSPACE.GETEI = GETfromGMT(sv5.GMT);

				ELVCNV(sv5, 0, 1, svtempout);
				EMMDYNEL(svtempout, newtab);

				EZSPACE.VEI = newtab.V / 0.3048;
				EZSPACE.GEI = newtab.gamma*DEG;
				EZSPACE.PEI = newtab.lat * DEG;
				EZSPACE.LEI = newtab.lng * DEG;
				EZSPACE.PSIEI = newtab.azi*DEG;
			}
		}
	}

	return 0;
}

void RTCC::EMSTAGEN(int L)
{
	EphemerisDataTable EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;

	ELFECH(RTCCPresentTimeGMT(), 300, 1, L, EPHEM, MANTIMES, LUNSTAY);

	Station station;
	StationTable contact;

	for (int i = 0;i < NUMBEROFGROUNDSTATIONS;i++)
	{
		if (MGRTAG == 0 && !groundstationslunar[i]) continue;
		station.alt = 0.0;
		station.code = gsabbreviations[i];
		station.lat = groundstations[i][0];
		station.lng = groundstations[i][1];
		contact.table.push_back(station);
	}

	OrbitStationContactsTable *tab;
	if (L == 1)
	{
		tab = &EZSTACT1;
	}
	else
	{
		tab = &EZSTACT3;
	}

	double GMT_start, GMT_end;
	GMT_start = RTCCPresentTimeGMT();
	GMT_end = GMT_start + 24.0*3600.0;
	if (GMT_start < EPHEM.Header.TL)
	{
		GMT_start = EPHEM.Header.TL;
		if (L == 1)
		{
			RTCCONLINEMON.TextBuffer[0] = "CSM";
			EMGPRINT("EMSTAGEN", 36);
		}
		else
		{
			RTCCONLINEMON.TextBuffer[0] = "LEM";
			EMGPRINT("EMSTAGEN", 36);
		}
	}

	EMGENGEN(EPHEM, MANTIMES, contact, BODY_EARTH, *tab);

	//Reset this, so next station contacts display updates immedately
	NextStationContactsBuffer.GET = -1;

	CapeCrossingTable *table;

	if (L == 1)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	if (table->NumRev == 0) return;

	for (int i = 0;i < 45;i++)
	{
		if (tab->Stations[i].GMTAOS <= 0.0) break;

		tab->Stations[i].REV = CapeCrossingRev(L, tab->Stations[i].GMTAOS);
	}
}

int RTCC::PMMMCD(PMMMCDInput in, MPTManeuver &man)
{
	EphemerisData sv_GMTI, sv_GMTI_other;
	VECTOR3 X_P, Y_P, Z_P, ExtDV, DV_A;
	double GMT_begin, P_G, Y_G, AL, BE, a1, a2, a3, b1, b2, b3, c1, c2, c3, SINP, SINY, SINR, COSP, COSY, COSR, d1, d2, d3, d4, Thrust;
	int TAIND, J;
	bool EXDVIND, Ind, TargetParamsInput;

	TargetParamsInput = true;
	ExtDV = _V(in.BurnParm75, in.BurnParm76, in.BurnParm77);

	J = 0;
	man.GMTI = in.GMTI;
	man.Thruster = in.Thruster;
	if (in.Thruster == RTCC_ENGINETYPE_CSMSPS || in.Thruster == RTCC_ENGINETYPE_LMAPS || in.Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		double dt_ullage_overlap;

		if (in.dt_ullage == 0.0)
		{
			dt_ullage_overlap = 0.0;
		}
		else
		{
			if (in.Thruster == RTCC_ENGINETYPE_CSMSPS)
			{
				dt_ullage_overlap = MCTSD9;
			}
			else if (in.Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				dt_ullage_overlap = MCTAD9;
			}
			else
			{
				dt_ullage_overlap = MCTDD9;
			}
		}

		GMT_begin = in.GMTI - in.dt_ullage + dt_ullage_overlap;
		man.dt_ullage = in.dt_ullage;
	}
	else
	{
		GMT_begin = in.GMTI;
	}
	man.GMTMAN = GMT_begin;

	//TBD: Confirmation
	TAIND = 0;
	man.TrajDet[0] = 1;
	man.TrajDet[1] = 1;
	man.TrajDet[2] = 1;

	man.AttitudeCode = in.Attitude;
	man.CommonBlock.ConfigChangeInd = in.ConfigChangeInd;
	man.CommonBlock.ConfigCode = in.ConfigCodeAfter;
	man.TVC = in.TVC;
	man.DockingAngle = in.DockingAngle;
	man.ConfigCodeBefore = in.CCMI;

	if (in.BPIND == 5)
	{
		/*if (input)
		{

		}*/
		goto RTCC_PMMMCD_12_1;
	}
	else if (in.BPIND == 6 || in.BPIND == 3)
	{
		goto RTCC_PMMMCD_10_1;
	}
	else if (in.BPIND == 4)
	{
		goto RTCC_PMMMCD_14_2;
	}
	else if (in.BPIND == 1)
	{
		goto RTCC_PMMMCD_12_1;
	}
	else
	{
		if (in.FrozenManeuver)
		{
			goto RTCC_PMMMCD_5_1;
		}
		else
		{
			goto RTCC_PMMMCD_5_2;
		}
	}

RTCC_PMMMCD_5_1:
	J = 1;
	if (TargetParamsInput)
	{
		man.dV_LVLH = _V(in.BurnParm75, in.BurnParm76, in.BurnParm77);
	}
	else
	{

	}
	man.Word67i[0] = 0;
	if (in.Attitude == 4)
	{
		EXDVIND = true;
		goto RTCC_PMMMCD_6_1;
	}
	else
	{

	}
RTCC_PMMMCD_5_2:
	if (TargetParamsInput == false)
	{
		goto RTCC_PMMMCD_12_1;
	}
	man.Word67i[0] = 1;
	//TBD: Fix this
	//if (in.ID != 32)
	{
		goto RTCC_PMMMCD_7_2;
	}
RTCC_PMMMCD_5_3:
	if (in.Attitude > 4)
	{
		goto RTCC_PMMMCD_6_2;
	}
	PMMMCDCallEMSMISS(in.sv_anchor, in.GMTI, sv_GMTI);
RTCC_PMMMCD_B:
	EXDVIND = false;
RTCC_PMMMCD_6_1:
	Thrust = GetOnboardComputerThrust(in.Thruster);
	Ind = 0;
	ExtDV = PIEXDV(sv_GMTI.R, sv_GMTI.V, in.WTMI, Thrust, _V(in.BurnParm75, in.BurnParm76, in.BurnParm77), EXDVIND);
	goto RTCC_PMMMCD_7_3;
RTCC_PMMMCD_6_2:
	PMMMCDCallEMSMISS(in.sv_anchor, GMT_begin, sv_GMTI);
RTCC_PMMMCD_D:
	//Are CSM and LM docked?
	if (in.CCMI.to_ulong() == 13 || in.CCMI.to_ulong() == 5)
	{
		goto RTCC_PMMMCD_C;
	}
	PMMMCDCallEMSMISS(in.sv_other, GMT_begin, sv_GMTI_other);
RTCC_PMMMCD_C:
	Ind = J;
	ExtDV = PIAEDV(_V(in.BurnParm75, in.BurnParm76, in.BurnParm77), sv_GMTI_other.R, sv_GMTI_other.V, sv_GMTI.R, Ind);
RTCC_PMMMCD_7_3:
	if (J != 0)
	{
		man.dV_inertial = ExtDV;
	}
	goto RTCC_PMMMCD_12_1;
RTCC_PMMMCD_7_2:
	man.dV_LVLH = ExtDV;
	goto RTCC_PMMMCD_12_1;
RTCC_PMMMCD_10_1:
	if (in.ID == 32)
	{
		//Get REFSMMAT from inputs
		goto RTCC_PMMMCD_11_1;
	}
	MATRIX3 REFSMMAT;
	if (in.Thruster != RTCC_ENGINETYPE_LOX_DUMP)
	{
		if (in.TVC == 3)
		{
			//Get LM REFSMMAT
			REFSMMAT = EZJGMTX3.data[in.RefMatCode - 1].REFSMMAT;
		}
		else
		{
			//Get CSM REFSMMAT
			REFSMMAT = EZJGMTX1.data[in.RefMatCode - 1].REFSMMAT;
		}
	}
	else
	{
		///Get IU REFSMMAT
		REFSMMAT = GZLTRA.IU1_REFSMMAT;
	}
	X_P = _V(REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13);
	Y_P = _V(REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23);
	Z_P = _V(REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);
	//Obtain REFSMMAT
	if (in.Thruster != RTCC_ENGINETYPE_LOX_DUMP)
	{
		//REFSMMAT error
	}
RTCC_PMMMCD_11_1:
	if (in.BPIND == 5)
	{
		goto RTCC_PMMMCD_15_2;
	}
	if (in.BPIND > 5)
	{
		goto RTCC_PMMMCD_11_3;
	}
	if (in.BPIND < 3)
	{
		goto RTCC_PMMMCD_15_2;
	}
RTCC_PMMMCD_11_2:
	DV_A = X_P * in.BurnParm75 + Y_P * in.BurnParm76 + Z_P * in.BurnParm77;
	goto RTCC_PMMMCD_11_4;
RTCC_PMMMCD_11_3:
	//Residuals
	in.CoordinateInd = -1;
RTCC_PMMMCD_11_4:
	if (in.Attitude >= 4)
	{
		ExtDV = DV_A;
		if (in.FrozenManeuver == false)
		{
			man.Word67i[0] = 1;
			goto RTCC_PMMMCD_5_3;
		}

		man.dV_inertial = DV_A;
		man.Word67i[0] = 0;
		if (in.Attitude > 4)
		{
			goto RTCC_PMMMCD_D;
		}
		goto RTCC_PMMMCD_B;
	}
	double dv = length(DV_A);
	if (dv > 1e-10)
	{
		man.A_T = unit(DV_A);
	}
	else
	{
		man.A_T = _V(1, 0, 0);
	}
RTCC_PMMMCD_12_1:
	if (in.Thruster < 33)
	{
		if (in.BPIND == 1)
		{
			in.BurnParm73 = 0;
		}
	}
	else
	{
		man.UllageThrusterOpt = in.UllageThrusterOpt;
		//APS
		if (in.Thruster == 34)
		{
			TAIND = 1;
		}
		else
		{
			//DPS
			if (in.Thruster > 34)
			{
				man.DT_10PCT = in.DT_10PCT;
				man.DPSScaleFactor = in.DPSScaleFactor;
			}
			man.TrimAngleInd = TAIND - 1;
		}
	}
	if (in.BPIND < 2)
	{
		//DT input
		if (in.BurnParm72 < 0)
		{
			man.dt = in.BurnParm74;
		}
		else
		{
			if (in.BurnParm73 > 1)
			{
				//XBT
				if (TAIND != 0)
				{
					if (in.Thruster == RTCC_ENGINETYPE_CSMSPS)
					{
						P_G = MCTSPG;
						Y_G = MCTSYG;
					}
					else if (in.Thruster == RTCC_ENGINETYPE_LMAPS)
					{
						P_G = MCTAPG;
						Y_G = MCTARG;
					}
					else if (in.Thruster == RTCC_ENGINETYPE_LMDPS)
					{
						P_G = MCTDPG;
						Y_G = MCTDRG;
					}
					else
					{
						P_G = 0.0;
						Y_G = 0.0;
					}
				}
				else
				{
					int IA = 1, IJ;
					double WTCXX, WTLXX, Thr, WDOT;
					if (in.CCMI[RTCC_CONFIG_D])
					{
						IJ = 0;
						if (in.mpt->mantable.size() == 0)
						{
							WTLXX = in.mpt->CommonBlock.LMAscentMass + in.mpt->CommonBlock.LMDescentMass;
						}
						else
						{
							WTLXX = in.mpt->mantable.back().CommonBlock.LMAscentMass + in.mpt->mantable.back().CommonBlock.LMDescentMass;
						}
					}
					else
					{
						IJ = 1;
						if (in.mpt->mantable.size() == 0)
						{
							WTLXX = in.mpt->CommonBlock.LMAscentMass;
						}
						else
						{
							WTLXX = in.mpt->mantable.back().CommonBlock.LMAscentMass;
						}
					}
					if (in.mpt->mantable.size() == 0)
					{
						WTCXX = in.mpt->CommonBlock.CSMMass;
					}
					else
					{
						WTCXX = in.mpt->mantable.back().CommonBlock.CSMMass;
					}
					unsigned IC = in.CCMI.to_ulong();
					GIMGBL(WTCXX, WTLXX, P_G, Y_G, Thr, WDOT, in.Thruster, IC, IA, IJ, in.DockingAngle);
				}
				man.dv = in.BurnParm72 / (cos(P_G)*cos(Y_G));
			}
			else
			{
				if (in.BurnParm73 == 1)
				{
					//DVC
					man.dv = -in.BurnParm72;
				}
				else
				{
					//MAG
					man.dv = in.BurnParm72;
				}
			}
		}
	}
	else
	{
		if (in.Attitude < 3)
		{
			man.dv = in.BurnParm72;
		}
	}

	if (in.CoordinateInd < 0)
	{
		man.HeadsUpDownInd = in.HeadsUpDownInd;
		goto RTCC_PMMMCD_17_1;
	}

	man.AttitudesInput = true;
	SINP = sin(in.Pitch);
	SINY = sin(in.Yaw);
	SINR = sin(in.Roll);
	COSP = cos(in.Pitch);
	COSY = cos(in.Yaw);
	COSR = cos(in.Roll);
	if (in.CoordinateInd > 0)
	{
		goto RTCC_PMMMCD_10_1;
	}
RTCC_PMMMCD_14_2:
	PMMMCDCallEMSMISS(in.sv_anchor, in.GMTI, sv_GMTI);
	Z_P = unit(-sv_GMTI.R);
	Y_P = -unit(crossp(sv_GMTI.R, sv_GMTI.V));
	X_P = crossp(Y_P, Z_P);
	if (in.BPIND == 4)
	{
		goto RTCC_PMMMCD_11_2;
	}
	AL = SINP * SINR;
	BE = SINP * COSR;
	a1 = COSY * COSP;
	a2 = SINY * COSP;
	a3 = -SINP;
	b1 = AL * COSY - SINY * COSR;
	b2 = AL * SINY + COSY * COSR;
	b3 = COSP * SINP;
	c1 = BE * COSY + SINY * SINR;
	c2 = BE * SINY - COSY * SINR;
	c3 = COSP * COSR;
	goto RTCC_PMMMCD_16_3;
RTCC_PMMMCD_15_2:
	if (in.TVC != 3)
	{
		d1 = SINY;
		d2 = SINR;
		d3 = COSY;
		d4 = COSR;
		goto RTCC_PMMMCD_16_4;
	}
	if (in.CoordinateInd == 1)
	{
		d1 = SINR;
		d2 = SINY;
		d3 = COSR;
		d4 = COSY;
		goto RTCC_PMMMCD_16_4;
	}
	AL = SINR * SINY;
	BE = COSR * SINY;
	a1 = COSR * COSP - AL * SINP;
	a2 = SINR * COSP;
	a3 = -COSR * SINP - AL * COSP;
	b1 = -SINR * COSP - AL * COSP;
	b2 = COSR * COSY;
	b3 = SINR * SINP - BE * COSP;
	c1 = COSY * SINP;
	c2 = SINY;
	c3 = COSY * COSP;
	goto RTCC_PMMMCD_16_3;
RTCC_PMMMCD_16_4:
	AL = COSP * d1;
	BE = SINP * d1;
	a1 = COSP * d3;
	a2 = d1;
	a3 = -SINP * d3;
	b1 = SINP * d2 - AL * d4;
	b2 = d3 * d4;
	b3 = COSP * d2 + BE * d4;
	c1 = SINP * d4 + AL * d2;
	c2 = -d3 * d2;
	c3 = COSP * d4 - BE * d2;
RTCC_PMMMCD_16_3:
	man.X_B = X_P * a1 + Y_P * a2 + Z_P * a3;
	man.Y_B = X_P * b1 + Y_P * b2 + Z_P * b3;
	man.Z_B = X_P * c1 + Y_P * c2 + Z_P * c3;

RTCC_PMMMCD_17_1:
	if (GMT_begin <= in.StartTimeLimit)
	{
		//Error
	}
	else if (GMT_begin + man.dt >= in.EndTimeLimit)
	{
		//Error
	}
	in.mpt->TimeToBeginManeuver[in.CurrentManeuver - 1U] = GMT_begin;
	in.mpt->TimeToEndManeuver[in.CurrentManeuver - 1U] = GMT_begin;
//RTCC_PMMMCD_17_2:
	return 0;
}

int RTCC::PMMMPT(PMMMPTInput in, MPTManeuver &man)
{
	EMSMISSInputTable emsin;
	CELEMENTS ELA;
	EphemerisData sv_gmti, sv_gmti_other;
	VECTOR3 DV_LVLH;
	double WDI[6], TH[6], DELT[6], TIDPS, MASS, DELVB, T, GMTBB, GMTI, WAITA, DT, mu;
	int err = 0;

	int NPHASE = 1;

	if (MPTIsRCSThruster(in.Thruster))
	{
		in.DETU = 0.0;
		in.UT = false;
	}

	double TIMP = in.sv_before.GMT;

	man.HeadsUpDownInd = true;
	man.TrajDet[0] = 1;
	man.TrajDet[1] = 1;
	man.TrajDet[2] = 1;
	man.DockingAngle = in.DockingAngle;
	man.Thruster = in.Thruster;
	man.AttitudeCode = in.Attitude;
	man.UllageThrusterOpt = in.UT;
	man.dt_ullage = in.DETU;
	man.ConfigCodeBefore = man.CommonBlock.ConfigCode = in.CONFIG;
	man.TVC = in.VC;
	man.IMPT = TIMP;
	if (in.Thruster == RTCC_ENGINETYPE_LMAPS)
	{
		man.TrimAngleInd = 1;
	}
	else
	{
		man.TrimAngleInd = -1;
	}

	if (in.sv_before.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}
	ELA = OrbMech::GIMIKC(in.sv_before.R, in.V_aft, mu);
	ELA.a /= OrbMech::R_Earth;
	VECTOR3 DV = in.V_aft - in.sv_before.V;
	double dv = length(DV);
	if (in.Attitude < 3)
	{
		man.dv = dv;
	}
	if (dv < 0.01*0.3048)
	{
		DV = _V(0, 0, 0);
		dv = 0.0;
		man.dt_ullage = 0.0;
		WAITA = in.VehicleWeight;
		GMTBB = TIMP;
		GMTI = TIMP;
		goto RTCC_PMMMPT_9_A;
	}

	if (in.Thruster <= 20)
	{
		if (in.Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2)
		{
			WDI[0] = MCTCW1;
			TH[0] = MCTCT1;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
		{
			WDI[0] = MCTCW2;
			TH[0] = MCTCT2;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2)
		{
			WDI[0] = MCTCW3;
			TH[0] = MCTCT3;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
		{
			WDI[0] = MCTCW4;
			TH[0] = MCTCT4;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSPLUS2)
		{
			WDI[0] = MCTLW1;
			TH[0] = MCTLT1;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSPLUS4)
		{
			WDI[0] = MCTLW2;
			TH[0] = MCTLT2;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSMINUS2)
		{
			WDI[0] = MCTLW3;
			TH[0] = MCTLT3;
		}
		else if (in.Thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
		{
			WDI[0] = MCTLW4;
			TH[0] = MCTLT4;
		}
		goto RTCC_PMMMPT_8_A;
	}
	if (in.Thruster != RTCC_ENGINETYPE_CSMSPS)
	{
		goto RTCC_PMMMPT_5_A;
	}
	NPHASE = 4;
	if (in.UT)
	{
		WDI[0] = MCTCW2;
		TH[0] = MCTCT2;
	}
	else
	{
		WDI[0] = MCTCW1;
		TH[0] = MCTCT1;
	}
	DELT[0] = in.DETU - MCTSD9;
	if (DELT[0] < 0)
	{
		DELT[0] = 0.0;
	}
	TH[1] = MCTST2;
	TH[2] = MCTST4;
	TH[3] = MCTST4;
	WDI[1] = MCTSW2;
	WDI[2] = MCTSW4;
	WDI[3] = MCTSW4;
	DELT[1] = MCTSD2;
	DELT[2] = MCTSD3;
	goto RTCC_PMMMPT_7_B;
RTCC_PMMMPT_5_A:
	if (in.UT)
	{
		WDI[0] = MCTLW2;
		TH[0] = MCTLT2;
	}
	else
	{
		WDI[0] = MCTLW1;
		TH[0] = MCTLT1;
	}
	if (in.Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		goto RTCC_PMMMPT_6_A;
	}
	NPHASE = 4;
	DELT[0] = in.DETU - MCTAD9;
	if (DELT[0] < 0)
	{
		DELT[0] = 0.0;
	}
	TH[1] = MCTAT2;
	TH[2] = MCTAT4;
	TH[3] = MCTAT4;
	WDI[1] = MCTAW2;
	WDI[2] = MCTAW4;
	WDI[3] = MCTAW4;
	DELT[1] = MCTAD2;
	DELT[2] = MCTAD3;
	goto RTCC_PMMMPT_7_B;
RTCC_PMMMPT_6_A:
	man.DPSScaleFactor = in.DPSScaleFactor;
	man.DT_10PCT = in.DT_10PCT;
	TH[1] = MCTDT2;
	TH[2] = MCTDT3;
	TH[3] = MCTDT4;
	WDI[1] = MCTDW2;
	WDI[2] = MCTDW3;
	WDI[3] = MCTDW4;
	DELT[0] = in.DETU - MCTDD9;
	if (DELT[0] < 0)
	{
		DELT[0] = 0.0;
	}
	DELT[1] = MCTDD2;
	DELT[2] = MCTDD3;
	TIDPS = in.DT_10PCT;
	if (TIDPS < 0)
	{
		goto RTCC_PMMMPT_7_A;
	}
	else if (TIDPS == 0.0)
	{
		TIDPS = MCTDD4;
	}
	MASS = in.VehicleWeight;
	DELVB = dv - MCTAK4 / MASS;
	double TG = (DELVB / MCTLT2)*MASS;
	if (TG <= MCTDD6)
	{
		NPHASE = 4;
		goto RTCC_PMMMPT_7_B;
	}
RTCC_PMMMPT_7_A:
	DELT[3] = abs(TIDPS) - DELT[1] - DELT[2];
	NPHASE = 6;
	TH[4] = MCTDT5 * in.DPSScaleFactor;
	TH[5] = MCTDT6 * in.DPSScaleFactor;
	WDI[4] = MCTDW5 * in.DPSScaleFactor;
	WDI[5] = MCTDW6 * in.DPSScaleFactor;
	DELT[4] = MCTDD5;
RTCC_PMMMPT_7_B:
	TH[1] = TH[0] + TH[1];
	WDI[1] = WDI[0] + WDI[1];
	if (in.Thruster != RTCC_ENGINETYPE_LMDPS)
	{
		TH[2] = TH[0] + TH[2];
		WDI[2] = WDI[0] + WDI[2];
	}
RTCC_PMMMPT_8_A:
	PCBBT(DELT, WDI, TH, in.VehicleWeight, TIMP, dv, NPHASE, T, GMTBB, GMTI, WAITA);
	DT = GMTI - GMTBB;
	if (in.IgnitionTimeOption)
	{
		GMTI = TIMP;
		GMTBB = GMTI - DT;
		if (in.Attitude != 3)
		{
			//Rotate DV through 1/2 the burn arc
			VECTOR3 X = unit(crossp(crossp(in.sv_before.R, in.sv_before.V), in.sv_before.R));
			VECTOR3 Y = unit(crossp(in.sv_before.V, in.sv_before.R));
			VECTOR3 Z = -unit(in.sv_before.R);
			double DV_X = dotp(DV, X);
			double DV_Y = dotp(DV, Y);
			double DV_Z = dotp(DV, Z);
			VECTOR3 DV_P = X * DV_X + Z * DV_Z;
			double theta_T = length(crossp(in.sv_before.R, in.sv_before.V))*length(DV)*in.VehicleWeight / (pow(length(in.sv_before.R), 2)*GetOnboardComputerThrust(in.Thruster));
			VECTOR3 DV_C = (unit(DV_P)*cos(theta_T / 2.0) + unit(crossp(DV_P, Y))*sin(theta_T / 2.0))*length(DV_P);
			DV = DV_C + Y * DV_Y;
		}
	}
	if (in.Attitude < 4)
	{
		man.A_T = DV / dv;
	}
RTCC_PMMMPT_9_A:
	if (in.Attitude == 3)
	{
		//TBD: Lambert
	}
	if (in.Attitude > 3)
	{
		man.FrozenManeuverInd = true;
	}
	if (in.IterationFlag)
	{
		goto RTCC_PMMMPT_12_A;
	}
RTCC_PMMMPT_10_A:
	man.FrozenManeuverInd = false;
	if (GMTBB < in.LowerTimeLimit)
	{
		return true;
	}
	if (in.Attitude <= 3)
	{
		goto RTCC_PMMMPT_11_B;
	}
	if (in.Attitude > 4)
	{
		goto RTCC_PMMMPT_11_A;
	}
	sv_gmti = EMMENI(emsin, in.sv_before, GMTI - TIMP);
	DV_LVLH = PIEXDV(sv_gmti.R, sv_gmti.V, in.VehicleWeight, GetOnboardComputerThrust(in.Thruster), DV, false);
	man.dV_LVLH = DV_LVLH;
	man.Word67i[0] = 1;
	goto RTCC_PMMMPT_11_B;
RTCC_PMMMPT_11_A:
	sv_gmti_other = EMMENI(emsin, in.sv_other, GMTBB - TIMP);
	sv_gmti = EMMENI(emsin, in.sv_before, GMTBB - TIMP);
	DV_LVLH = PIAEDV(DV, sv_gmti_other.R, sv_gmti_other.V, sv_gmti.R, false);
	man.dV_LVLH = DV_LVLH;
	man.Word67i[0] = 1;
RTCC_PMMMPT_11_B:
	in.mpt->TimeToBeginManeuver[in.CurrentManeuver - 1] = in.mpt->TimeToEndManeuver[in.CurrentManeuver - 1] = GMTBB;
	man.GMTMAN = GMTBB;
	man.GMTI = GMTI;
	//WEIGHT = WAITA;
	if (in.VC == 1)
	{
		//Update CSM weight
	}
	else
	{
		//Update LM weight
	}
	return false;
RTCC_PMMMPT_12_A:
	PMMRKJInputArray integin;

	integin.A = 0.0;
	integin.DENSMULT = 1.0;
	integin.DOCKANG = man.DockingAngle;
	integin.DPSScale = man.DPSScaleFactor;
	integin.DTPS10 = man.DT_10PCT;
	integin.DTU = man.dt_ullage;
	integin.HeadsUpDownInd = man.HeadsUpDownInd;
	integin.IC = man.CommonBlock.ConfigCode.to_ulong();
	integin.KAUXOP = 1;
	integin.KEPHOP = 0;
	integin.KTRIMOP = man.TrimAngleInd;
	integin.LMDESCJETT = 1e70;
	integin.MANOP = 4;
	integin.ThrusterCode = man.Thruster;
	integin.TVC = man.TVC;
	integin.UllageOption = man.UllageThrusterOpt;
	integin.ExtDVCoordInd = false;
	integin.WDMULT = 1.0;
	integin.XB = man.X_B;
	integin.YB = man.Y_B;
	integin.ZB = man.Z_B;
	integin.CAPWT = in.VehicleWeight;
	integin.CSMWT = 0.0;
	integin.LMAWT = 0.0;
	integin.LMDWT = 0.0;
	integin.SIVBWT = 0.0;
	if (man.TVC == RTCC_MANVEHICLE_CSM)
	{
		integin.CSMWT = in.VehicleWeight;
	}
	else if (man.TVC == RTCC_MANVEHICLE_LM)
	{
		integin.LMDWT = in.VehicleWeight;
	}
	else
	{
		integin.SIVBWT = in.VehicleWeight;
	}

	int Ierr;
	RTCCNIAuxOutputTable aux;

	CSMLMPoweredFlightIntegration numin(this, integin, Ierr, NULL, &aux);

	double SUM;
	int ITCNT;

	SUM = 1.0e75;
	ITCNT = 0;
	VECTOR3 ATV;
	if (man.AttitudeCode >= 4)
	{
		ATV = DV / length(DV);
	}
	else
	{
		ATV = man.A_T;
	}
	EphemerisData sv_BO, sv_impt;
	VECTOR3 H, YT;
	double P, Y, PARM[6], SUMN, DA[6], DAS[6], X, LAMI, LAMP, WX[6], WY[6], EPS[6], SAVEX, DP[6], TIG, YV[6], DELP[6], LAML;
	int ICNT, i, ITMAX, J, IPI, ii, NIP, IPT;
	bool IP[6], IFLAG, DFLAG, IC[6];
	CELEMENTS ELPRES, OSCELB;

	double **A, **XP;
	A = new double*[6];
	XP = new double*[6];
	for (i = 0;i < 6;i++)
	{
		A[i] = new double[6];
		XP[i] = new double[6];
		for (ii = 0;ii < 6;ii++)
		{
			A[i][ii] = 0.0;
			XP[i][ii] = 0.0;
		}
	}

	//Constants
	double WNULL[6] = { 0,0,0,0,0,0 };
	LAMI = pow(2, -28);
	LAML = 1.0;
	EPS[0] = 1.0 / OrbMech::R_Earth;
	EPS[1] = 0.0001;
	EPS[2] = 0.00001;
	EPS[3] = 0.00001;
	EPS[4] = 0.00001;
	WX[0] = 1.0;
	WX[1] = 1.0;
	WX[2] = 0.01;
	WX[3] = 0.01;
	WX[4] = 1.0;
	WX[5] = 1.0;
	for (i = 0;i < 5;i++)
	{
		WY[i] = pow(2, -40) / pow(EPS[i], 2);
	}
	ITMAX = 5;
	DP[0] = 0.001*RAD;
	DP[1] = 0.001*RAD;
	DP[2] = 0.1*0.3048;
	DP[3] = 0.01;
	IC[0] = IC[1] = IC[2] = IC[3] = IC[4] = true;
	IC[5] = false;

	PARM[3] = GMTBB;
	IP[3] = true;
	TIG = GMTBB;
	sv_gmti = EMMENI(emsin, in.sv_before, TIG - TIMP);
	LAMP = LAMI * 10.0;
	NIP = 6;
	if (man.AttitudeCode == 3)
	{
		goto RTCC_PMMMPT_14_A;
	}
	IP[0] = true;
	IP[1] = true;
	IP[2] = true;
	IP[4] = false;
	IP[5] = false;
	PARM[0] = P = asin(ATV.z);
	if (abs(P - PI05) >= 0.0017)
	{
		PARM[1] = Y = atan2(ATV.y, ATV.x);
	}
	else
	{
		H = crossp(ATV, sv_gmti.R / OrbMech::R_Earth);
		if (length(H) >= 0.001)
		{
			YT = unit(H);
		}
		else
		{
			YT = unit(crossp(sv_gmti.V, sv_gmti.R));
		}
		Y = PARM[1] = atan2(YT.y, YT.x);
	}
	PARM[2] = dv;
	NIP = 4;
	goto RTCC_PMMMPT_14_B;
RTCC_PMMMPT_14_A:
	IP[0] = false;
	IP[1] = false;
	IP[2] = false;
	IP[4] = true;
	IP[5] = true;
	//TBD: Lambert
	goto RTCC_PMMMPT_14_B;
RTCC_PMMMPT_14_C:
	if (DFLAG)
	{
		for (i = 0;i < 6;i++)
		{
			DAS[i] = DA[i];
		}
	}
RTCC_PMMMPT_14_B:
	//Input parameters
	integin.sv0 = sv_gmti;
	ATV = _V(cos(PARM[1])*cos(PARM[0]), sin(PARM[1])*cos(PARM[0]), sin(PARM[0]));
	integin.VG = ATV * PARM[2];

	numin.PMMRKJ();
	sv_BO.R = aux.R_BO;
	sv_BO.V = aux.V_BO;
	sv_BO.GMT = aux.GMT_BO;
	sv_BO.RBI = aux.RBI;
	sv_impt = EMMENI(emsin, sv_BO, TIMP - sv_BO.GMT);
	ELPRES = OrbMech::GIMIKC(sv_impt.R, sv_impt.V, mu);
	ELPRES.a /= OrbMech::R_Earth;
	SUMN = 0.0;
	IFLAG = false;
	ICNT = 0;
	for (i = 0;i < 6;i++)
	{
		if (IC[i])
		{
			DA[ICNT] = X = ELA.data[i] - ELPRES.data[i];
			ICNT++;
			SUMN = SUMN + WY[i] * X*X;
			if (abs(X) > EPS[i])
			{
				IFLAG = true;
			}
		}
	}
	if (IFLAG)
	{
		goto RTCC_PMMMPT_16_B;
	}
	if (ITCNT != 0)
	{
		//Move burn parameters from integrator input table to PMMMPT output area
		DV = integin.VG;
		GMTBB = integin.sv0.GMT;
		GMTI = GMTBB + DT;
	}
	goto RTCC_PMMMPT_10_A;
RTCC_PMMMPT_16_B:
	if (ITCNT != 0)
	{
		//Did fit improve?
		if (SUMN >= SUM)
		{
			//No
			goto RTCC_PMMMPT_20_A;
		}
	}
	if (LAMP > LAMI)
	{
		LAMP = LAMP / 10.0;
	}
	//Save burn parameters
	DFLAG = true;
	SUM = SUMN;
	ITCNT++;
	if (ITCNT > ITMAX)
	{
		//Put best burn parameters in PMMMPT output area - print message
		DV = integin.VG;
		GMTBB = integin.sv0.GMT;
		GMTI = GMTBB + DT;
		PMXSPT("PMMMPT", 200);
		goto RTCC_PMMMPT_10_A;
	}
	J = 0;
RTCC_PMMMPT_17_B:
	if (IP[J] == false)
	{
		goto RTCC_PMMMPT_19_B;
	}
	SAVEX = PARM[J];
	PARM[J] = PARM[J] + DP[J];
	if (TIG != PARM[3])
	{
		TIG = PARM[3];
		sv_gmti = EMMENI(emsin, in.sv_before, TIG - in.sv_before.GMT);
	}
	//Update PMMRKJ input table
	integin.sv0 = sv_gmti;
	ATV = _V(cos(PARM[1])*cos(PARM[0]), sin(PARM[1])*cos(PARM[0]), sin(PARM[0]));
	integin.VG = ATV * PARM[2];
	numin.PMMRKJ();
	sv_BO.R = aux.R_BO;
	sv_BO.V = aux.V_BO;
	sv_BO.GMT = aux.GMT_BO;
	sv_BO.RBI = aux.RBI;
	sv_impt = EMMENI(emsin, sv_BO, TIMP - sv_BO.GMT);
	OSCELB = OrbMech::GIMIKC(sv_impt.R, sv_impt.V, mu);
	OSCELB.a /= OrbMech::R_Earth;
	IPI = 0;
	for (ii = 0;ii < 6;ii++)
	{
		if (IC[ii])
		{
			XP[IPI][J] = (OSCELB.data[ii] - ELPRES.data[ii]) / DP[J];
			IPI++;
		}
	}
	PARM[J] = SAVEX;
RTCC_PMMMPT_19_B:
	J++;
	if (J <= 5)
	{
		goto RTCC_PMMMPT_17_B;
	}	
	PCMATO(A, YV, XP, DA, 5, NIP, WY, 0.0, WNULL);
	goto RTCC_PMMMPT_20_B;
RTCC_PMMMPT_20_A:
	//Reset independent variables
	LAMP = LAMP * 10.0;
	DFLAG = false;
	for (i = 0;i < 6;i++)
	{
		DA[i] = DAS[i];
	}
	if (LAMP > LAML)
	{
		//Put best burn parameters in PMMMPT output area - print message
		PMXSPT("PMMMPT", 200);
		goto RTCC_PMMMPT_10_A;
	}
RTCC_PMMMPT_20_B:
	for (i = 0;i < NIP;i++)
	{
		A[i][i] = A[i][i] + WX[i] * LAMP;
	}
	PCGAUS(A, YV, DELP, NIP, 0.0);
	IPT = 0;
	for (i = 0;i < 6;i++)
	{
		if (IP[i])
		{
			PARM[i] = PARM[i] + DELP[IPT];
			IPT++;
		}
	}
	TIG = PARM[3];
	sv_gmti = EMMENI(emsin, in.sv_before, TIG - in.sv_before.GMT);
	goto RTCC_PMMMPT_14_C;

	delete[] A;
	delete[] XP;

	/*double MJD = OrbMech::MJDfromGET(in.sv_before.GMT, GMTBASE);
	OBJHANDLE gravref = GetGravref(in.sv_before.RBI);
	VECTOR3 R_cut, V_cut;
	double m_cut, MJD_cut, t_slip;

	OrbMech::impulsive2(in.sv_before.R, in.sv_before.V, MJD, gravref, GetOnboardComputerThrust(in.Thruster), TH[NPHASE - 1], TH[NPHASE - 1] / WDI[NPHASE - 1], in.VehicleWeight, in.sv_before.R, in.V_aft, DV, t_slip, R_cut, V_cut, MJD_cut, m_cut);
	GMTI = TIMP + t_slip;
	GMTBB = GMTI - DT;
	goto RTCC_PMMMPT_10_A;*/
}

int RTCC::PMMLAI(PMMLAIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable *E)
{
	EphemerisData data;
	VECTOR3 R_LS = OrbMech::r_from_latlong(BZLSDISP.lat[RTCC_LMPOS_BEST], BZLSDISP.lng[RTCC_LMPOS_BEST], MCSMLR);
	double theta, dt_asc, dv;
	SV sv_IG, sv_Ins;
	LunarAscentProcessor(R_LS, in.m0, in.sv_CSM, GMTBASE, in.t_liftoff, in.v_LH, in.v_LV, theta, dt_asc, dv, sv_IG, sv_Ins);

	aux.A_T = unit(sv_IG.R);
	aux.DT_B = (sv_Ins.MJD - sv_IG.MJD)*24.0*3600.0;
	aux.DT_TO = 0.0;
	aux.DV = dv;
	aux.DV_C = dv;
	aux.DV_cTO = 0.0;
	aux.DV_TO = 0.0;
	aux.DV_U = 0.0;
	aux.MainFuelUsed = sv_IG.mass - sv_Ins.mass;
	aux.P_G = 0.0;
	aux.RCSFuelUsed = 0.0;
	aux.CSI = 2;
	aux.RBI = BODY_MOON;
	aux.R_1 = sv_IG.R;
	aux.V_1 = sv_IG.V;
	aux.GMT_1 = OrbMech::GETfromMJD(sv_IG.MJD, GMTBASE);
	aux.R_BI = sv_IG.R;
	aux.V_BI = sv_IG.V;
	aux.GMT_BI = OrbMech::GETfromMJD(sv_IG.MJD, GMTBASE);
	aux.R_BO = sv_Ins.R;
	aux.V_BO = sv_Ins.V;
	aux.GMT_BO = OrbMech::GETfromMJD(sv_Ins.MJD, GMTBASE);
	aux.sv_FF.R = sv_IG.R;
	aux.sv_FF.V = sv_IG.V;
	aux.sv_FF.GMT = OrbMech::GETfromMJD(sv_IG.MJD, GMTBASE);
	if (sv_IG.gravref == hEarth)
	{
		aux.sv_FF.RBI = BODY_EARTH;
	}
	else
	{
		aux.sv_FF.RBI = BODY_MOON;
	}
	aux.V_G = aux.A_T*dv;
	aux.WTEND = sv_Ins.mass;
	aux.WTENGON = sv_IG.mass;
	aux.W_CSM = 0.0;
	aux.W_LMA = sv_Ins.mass;
	aux.W_LMD = 0.0;
	aux.W_SIVB = 0.0;
	aux.X_B = _V(1, 0, 0);
	aux.Y_B = _V(0, 1, 0);
	aux.Y_G = 0.0;
	aux.Z_B = _V(0, 0, 1);

	if (E)
	{
		data.R = sv_Ins.R;
		data.V = sv_Ins.V;
		data.GMT = OrbMech::GETfromMJD(sv_Ins.MJD, GMTBASE);
		data.RBI = BODY_MOON;
		E->table.push_back(data);
	}

	return 0;
}

int RTCC::PMMLDI(PMMLDIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable *E)
{
	EphemerisData data;
	SV sv_PDI, sv_land;
	double dv;
	VECTOR3 R_LS = OrbMech::r_from_latlong(BZLSDISP.lat[RTCC_LMPOS_BEST], BZLSDISP.lng[RTCC_LMPOS_BEST], MCSMLR);

	PoweredDescentProcessor(R_LS, in.TLAND, in.sv, CalcGETBase(), aux, E, sv_PDI, sv_land, dv);

	aux.A_T = _V(1, 0, 0);
	aux.DT_B = (sv_land.MJD - sv_PDI.MJD)*24.0*3600.0;
	aux.DT_TO = 0.0;
	aux.DV = dv;
	aux.DV_C = dv;
	aux.DV_cTO = 0.0;
	aux.DV_TO = 0.0;
	aux.DV_U = 0.0;
	aux.MainFuelUsed = sv_PDI.mass - sv_land.mass;
	aux.P_G = 0.0;
	aux.RCSFuelUsed = 0.0;
	aux.CSI = 2;
	aux.RBI = BODY_MOON;
	aux.R_1 = sv_PDI.R;
	aux.V_1 = sv_PDI.V;
	aux.GMT_1 = OrbMech::GETfromMJD(sv_PDI.MJD, GMTBASE);
	aux.R_BI = sv_PDI.R;
	aux.V_BI = sv_PDI.V;
	aux.GMT_BI = OrbMech::GETfromMJD(sv_PDI.MJD, GMTBASE);
	aux.R_BO = sv_land.R;
	aux.V_BO = sv_land.V;
	aux.GMT_BO = OrbMech::GETfromMJD(sv_land.MJD, GMTBASE);
	aux.sv_FF.R = sv_PDI.R;
	aux.sv_FF.V = sv_PDI.V;
	aux.sv_FF.GMT = OrbMech::GETfromMJD(sv_PDI.MJD, GMTBASE);
	if (sv_PDI.gravref == hEarth)
	{
		aux.sv_FF.RBI = BODY_EARTH;
	}
	else
	{
		aux.sv_FF.RBI = BODY_MOON;
	}
	aux.V_G = _V(1, 0, 0)*dv;
	aux.WTEND = sv_land.mass;
	aux.WTENGON = sv_PDI.mass;
	aux.W_CSM = 0.0;
	aux.W_LMA = in.W_LMA;
	aux.W_LMD = sv_land.mass - in.W_LMA;
	aux.W_SIVB = 0.0;
	aux.X_B = _V(1, 0, 0);
	aux.Y_B = _V(0, 1, 0);
	aux.Y_G = 0.0;
	aux.Z_B = _V(0, 0, 1);

	if (E)
	{
		data.R = sv_land.R;
		data.V = sv_land.V;
		data.GMT = OrbMech::GETfromMJD(sv_land.MJD, GMTBASE);
		data.RBI = BODY_MOON;
		E->table.push_back(data);
	}

	return 0;
}

int RTCC::PMMLDP(PMMLDPInput in, MPTManeuver &man)
{
	SV sv_IG;
	MATRIX3 REFSMMAT;
	VECTOR3 U_IG;
	double GMTBB, CR, t_go;

	VECTOR3 R_LS = OrbMech::r_from_latlong(BZLSDISP.lat[RTCC_LMPOS_BEST], BZLSDISP.lng[RTCC_LMPOS_BEST], MCSMLR);

	if (!PDIIgnitionAlgorithm(in.sv, CalcGETBase(), R_LS, in.TLAND, sv_IG, t_go, CR, U_IG, REFSMMAT))
	{
		return 1;
	}

	GMTBB = OrbMech::GETfromMJD(sv_IG.MJD, GMTBASE);

	man.CommonBlock.ConfigCode[RTCC_CONFIG_C] = false;
	man.CommonBlock.ConfigCode[RTCC_CONFIG_S] = false;
	man.CommonBlock.ConfigCode[RTCC_CONFIG_A] = true;
	man.CommonBlock.ConfigCode[RTCC_CONFIG_D] = true;
	man.AttitudeCode = RTCC_ATTITUDE_PGNS_DESCENT;
	man.Thruster = RTCC_ENGINETYPE_LMDPS;
	man.ConfigCodeBefore = man.CommonBlock.ConfigCode;
	man.TVC = RTCC_MANVEHICLE_LM;
	man.TrimAngleInd = in.TrimAngleInd - 1;
	man.HeadsUpDownInd = in.HeadsUpDownInd;
	if (in.CurrentManeuver > 1U)
	{
		man.DockingAngle = in.mpt->mantable[in.CurrentManeuver - 2U].DockingAngle;
	}
	else
	{
		man.DockingAngle = in.mpt->DeltaDockingAngle;
	}
	man.GMTMAN = GMTBB;// -7.5;
	man.GMTI = GMTBB;
	man.GMT_BO = GMTfromGET(in.TLAND);

	return 0;
}

void RTCC::PMMFUD(int veh, unsigned man, int action)
{
	bool tupind = false;
	MissionPlanTable *mpt = GetMPTPointer(veh);

	if (man > mpt->ManeuverNum)
	{
		//Error 40
		PMXSPT("PMMFUD", 40);
		return;
	}

	//Future Delete
	if (action == 0)
	{
		//Maneuver executed?
		if (man <= mpt->LastExecutedManeuver)
		{
			//Error 43
			PMXSPT("PMMFUD", 43);
			return;
		}
		//Maneuver frozen?
		if (mpt->mantable[man - 1].FrozenManeuverInd)
		{
			//Error 47
			PMXSPT("PMMFUD", 47);
			return;
		}

		while (mpt->mantable.size() >= man)
		{
			mpt->mantable.pop_back();
		}

		mpt->ManeuverNum = mpt->mantable.size();
		//TBD: Is TLI maneuver being deleted?
		//TBD: Is deorbit maneuver being deleted?
		//Is the next upcoming maneuver being deleted?
		if (man == mpt->LastExecutedManeuver + 1)
		{
			mpt->UpcomingManeuverGMT = 1e70;
		}
		mpt->CommonBlock.TUP = -mpt->CommonBlock.TUP;
		PMSVCT(8, veh);
	}
	//History Delete
	else if (action == 1)
	{
		//Are all maneuvers to be deleted executed?
		if (man > mpt->LastExecutedManeuver)
		{
			//Error 42
			PMXSPT("PMMFUD", 42);
			return;
		}

		mpt->LastExecutedManeuver -= man;
		mpt->LastFrozenManeuver -= man;

		for (unsigned i = man;i < mpt->ManeuverNum;i++)
		{
			mpt->TimeToBeginManeuver[i - man] = mpt->TimeToBeginManeuver[i];
			mpt->TimeToEndManeuver[i - man] = mpt->TimeToEndManeuver[i];
			mpt->AreaAfterManeuver[i - man] = mpt->AreaAfterManeuver[i];
			mpt->WeightAfterManeuver[i - man] = mpt->WeightAfterManeuver[i];
		}

		mpt->ManeuverNum -= man;
		mpt->CommonBlock.ConfigCode = mpt->mantable[man - 1].CommonBlock.ConfigCode;
		mpt->DeltaDockingAngle = mpt->mantable[man - 1].DockingAngle;
		mpt->ConfigurationArea = mpt->mantable[man - 1].TotalAreaAfter;
		mpt->CommonBlock = mpt->mantable[man - 1].CommonBlock;
		//TBD: Compute and store new GET to begin venting

		for (unsigned i = 0;i < man;i++)
		{
			mpt->mantable.pop_front();
		}

		//Any remaining maneuvers?
		if (mpt->ManeuverNum > 0)
		{
			mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);
		}
		//TBD: Move maneuvers up in the table here
		if (mpt->CommonBlock.TUP < 0)
		{
			mpt->CommonBlock.TUP = abs(mpt->CommonBlock.TUP);
		}
		EMSNAP(veh, 2);
	}
	//Freeze
	else if (action == 2)
	{
		//Maneuver executed?
		if (man <= mpt->LastExecutedManeuver)
		{
			//Error 10
			PMXSPT("PMMFUD", 10);
			return;
		}
		//Determine 1st maneuver to be frozen
		unsigned i = mpt->LastExecutedManeuver + 1;
		//Set trajectory number negative
		mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);

		do
		{
			if (mpt->mantable[i - 1].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
			{
				//Is this a re-freeze?
				if (mpt->mantable[i - 1].FrozenManeuverInd)
				{

				}
			}
			else
			{
				mpt->mantable[i - 1].FrozenManeuverVector.GMT = mpt->mantable[i - 1].GMTMAN;
				mpt->mantable[i - 1].FrozenManeuverVector.R = mpt->mantable[i - 1].R_1;
				mpt->mantable[i - 1].FrozenManeuverVector.V = mpt->mantable[i - 1].V_1;
				mpt->mantable[i - 1].FrozenManeuverVector.RBI = mpt->mantable[i - 1].RefBodyInd;
			}
			//External DV
			if (mpt->mantable[i - 1].AttitudeCode >= 4 && mpt->mantable[i - 1].AttitudeCode <= 5)
			{
				mpt->mantable[i - 1].Word67i[0] = 0;
			}
			mpt->mantable[i - 1].FrozenManeuverInd = true;
			i++;
		} while (i <= man);

		PMSVCT(8, veh);
	}
	//Unfreeze
	else
	{
		//Maneuver executed?
		if (man <= mpt->LastExecutedManeuver)
		{
			//Error 46
			PMXSPT("PMMFUD", 46);
			return;
		}
		//Maneuver frozen?
		if (mpt->mantable[man - 1].FrozenManeuverInd == false)
		{
			//Error 45
			PMXSPT("PMMFUD", 45);
			return;
		}
		//Decrement input number by 1 and store as number of last frozen maneuver in MPT
		mpt->LastFrozenManeuver = man - 1;
		
		mpt->CommonBlock.TUP = -abs(mpt->CommonBlock.TUP);
		unsigned curman = man;
		while (curman <= mpt->ManeuverNum)
		{
			//Is this the TLI maneuver?
			if (mpt->mantable[curman - 1].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
			{
				mpt->mantable[curman - 1].Word78i[0] = mpt->mantable[curman - 1].Word78i[1];
				tupind = true;
			}
			else if (mpt->mantable[curman - 1].AttitudeCode == RTCC_ATTITUDE_LAMBERT)
			{
				tupind = true;
			}
			//External DV
			else if (mpt->mantable[curman - 1].AttitudeCode > 3 && mpt->mantable[curman - 1].AttitudeCode < 6)
			{
				mpt->mantable[curman - 1].Word67i[0] = 1;
				tupind = true;
			}
			//IGM again??
			else if (mpt->mantable[curman - 1].AttitudeCode >= 6)
			{
				tupind = true;
			}
			mpt->mantable[curman - 1].FrozenManeuverInd = false;
			curman++;
		}
		if (tupind)
		{
			PMSVCT(8, veh);
		}
		else
		{
			mpt->CommonBlock.TUP = abs(mpt->CommonBlock.TUP);
		}
	}
}

void RTCC::PMMUDT(int L, unsigned man, int headsup, int trim)
{
	MissionPlanTable *tab;
	if (L == RTCC_MPT_CSM)
	{
		tab = &PZMPTCSM;
	}
	else
	{
		tab = &PZMPTLEM;
	}
	//Is requested maneuver in MPT?
	if (tab->mantable.size() < man)
	{
		//Error 53
		PMXSPT("PMMUDT", 53);
		return;
	}
	MPTManeuver *pMan = &tab->mantable[man - 1];
	//Has requested maneuver been executed?
	if (man <= tab->LastExecutedManeuver)
	{
		//Error 54
		PMXSPT("PMMUDT", 54);
		return;
	}
	//Is requested maneuver frozen?
	if (pMan->FrozenManeuverInd)
	{
		//Error 56
		PMXSPT("PMMUDT", 56);
		return;
	}
	if (pMan->AttitudeCode == RTCC_ATTITUDE_SIVB_IGM || pMan->AttitudeCode == RTCC_ATTITUDE_AGS_ASCENT || pMan->AttitudeCode == RTCC_ATTITUDE_PGNS_ASCENT)
	{
		//Error 55
		PMXSPT("PMMUDT", 55);
		return;
	}
	if (headsup >= 0)
	{
		if (headsup == 1)
		{
			pMan->HeadsUpDownInd = true;
		}
		else
		{
			pMan->HeadsUpDownInd = false;
		}
		pMan->AttitudesInput = false;
	}
	if (trim >= 0)
	{
		pMan->TrimAngleInd = trim - 1;
	}
	PMSVCT(8, L);
}

void RTCC::EMSTRAJ(EphemerisData sv, int L, bool landed)
{
	MissionPlanTable *table;
	OrbitEphemerisTable *maineph;
	HistoryAnchorVectorTable *anchor;
	CapeCrossingTable *cctab;

	double gmt;

	if (L == RTCC_MPT_CSM)
	{
		table = &PZMPTCSM;
		maineph = &EZEPH1;
		anchor = &EZANCHR1;
		cctab = &EZCCSM;
	}
	else
	{
		table = &PZMPTLEM;
		maineph = &EZEPH2;
		anchor = &EZANCHR3;
		cctab = &EZCLEM;
	}

	gmt = RTCCPresentTimeGMT();

	if (!landed)
	{
		//TBD: Maybe store a landing site vector
		EMGVECSTInput(L, sv);
	}

	//Generate main ephemeris
	EMSEPH(2, sv, L, gmt, landed);
	if (landed)
	{
		cctab->NumRev = 0;
	}
	else
	{
		//Generate cape crossing
		RMMEACC(L, sv.RBI, 0, CapeCrossingRev(L, gmt));
	}
	//Generate station contacts
	EMSTAGEN(L);
	//Update displays
	EMSNAP(L, 1);
}

EphemerisData RTCC::EMSEPH(int QUEID, EphemerisData sv0, int L, double PresentGMT, bool landed)
{
	EMSMISSInputTable InTable;

	OrbitEphemerisTable *table;
	
	if (L == RTCC_MPT_CSM)
	{
		table = &EZEPH1;
	}
	else
	{
		table = &EZEPH2;
	}

	if (QUEID == 2)
	{
		//Increment
		table->EPHEM.Header.TUP++;
		//Lock
		table->EPHEM.Header.TUP = -table->EPHEM.Header.TUP;
	}

	InTable.AnchorVector = sv0;
	InTable.EarthRelStopParam = 400000.0*0.3048;
	InTable.MoonRelStopParam = 0.0;
	InTable.CutoffIndicator = 3;
	InTable.VehicleCode = L;
	InTable.IgnoreManueverNumber = 0;
	InTable.landed = landed;
	InTable.ManeuverIndicator = true;

	if (QUEID == 2)
	{
		InTable.MaxIntegTime = 10.0*24.0*3600.0;
		InTable.EphemerisBuildIndicator = true;

		bool HighOrbit;

		if (landed)
		{
			HighOrbit = false;
		}
		else
		{

			double mu;

			if (sv0.RBI == BODY_EARTH)
			{
				mu = OrbMech::mu_Earth;
			}
			else
			{
				mu = OrbMech::mu_Moon;
			}

			OELEMENTS coe = OrbMech::coe_from_sv(sv0.R, sv0.V, mu);
			double SMA = OrbMech::GetSemiMajorAxis(sv0.R, sv0.V, mu);

			if (coe.e > MCGECC)
			{
				HighOrbit = true;
			}
			else
			{
				if (SMA < 0 || SMA > MCGSMA*OrbMech::R_Earth)
				{
					HighOrbit = true;
				}
				else
				{
					HighOrbit = false;
				}
			}
		}

		double DTMAX;

		if (HighOrbit)
		{
			DTMAX = 10.0*24.0*3600.0;
		}
		else
		{
			DTMAX = 48.0*3600.0;
		}

		InTable.EphemerisLeftLimitGMT = PresentGMT;
		InTable.EphemerRightLimitGMT = PresentGMT + DTMAX;
		InTable.ManCutoffIndicator = 1;
		InTable.EphemTableIndicator = &table->EPHEM;
		InTable.StopParamRefFrame = 2;
	}
	else
	{
		InTable.MaxIntegTime = PresentGMT - sv0.GMT;
		InTable.EphemerisBuildIndicator = false;
		InTable.ManCutoffIndicator = 2;
	}

	MissionPlanTable *mpt = GetMPTPointer(L);

	if (QUEID == 2)
	{
		table->EPHEM.table.clear();
		table->MANTIMES.Table.clear();
	}
	else
	{
		EMSMISS(InTable);
		return InTable.NIAuxOutputTable.sv_cutoff;
	}

	RTCCNIAuxOutputTable aux;
	InTable.AuxTableIndicator = &aux;
	InTable.ManTimesIndicator = &table->MANTIMES;

	do
	{
		EMSMISS(InTable);
		if (InTable.NIAuxOutputTable.TerminationCode == 1)
		{
			MANTIMESData data;

			if (InTable.NIAuxOutputTable.LunarStayEndGMT > 0)
			{
				table->LUNRSTAY.LunarStayEndGMT = InTable.NIAuxOutputTable.LunarStayEndGMT;
				InTable.landed = false;
			}
			if (InTable.NIAuxOutputTable.LunarStayBeginGMT > 0)
			{
				table->LUNRSTAY.LunarStayBeginGMT = InTable.NIAuxOutputTable.LunarStayBeginGMT;
				InTable.landed = true;
			}

			PMMDMT(L, InTable.NIAuxOutputTable.ManeuverNumber, InTable.AuxTableIndicator);
			InTable.AnchorVector = table->EPHEM.table.back();
			InTable.EphemerisLeftLimitGMT = InTable.AnchorVector.GMT;
			InTable.IgnoreManueverNumber = InTable.NIAuxOutputTable.ManeuverNumber;
		}
		else
		{
			if (InTable.NIAuxOutputTable.LunarStayEndGMT > 0)
			{
				table->LUNRSTAY.LunarStayEndGMT = InTable.NIAuxOutputTable.LunarStayEndGMT;
			}
			if (InTable.NIAuxOutputTable.LunarStayBeginGMT > 0)
			{
				table->LUNRSTAY.LunarStayBeginGMT = InTable.NIAuxOutputTable.LunarStayBeginGMT;
			}
		}
	} while (InTable.NIAuxOutputTable.TerminationCode == 1);

	//Unlock
	if (QUEID == 2)
	{
		table->EPHEM.Header.TUP = -table->EPHEM.Header.TUP;
	}
	table->EPHEM.Header.NumVec = table->EPHEM.table.size();
	table->EPHEM.Header.Offset = 0;
	table->EPHEM.Header.TL = table->EPHEM.table.front().GMT;
	table->EPHEM.Header.TR = table->EPHEM.table.back().GMT;

	mpt->CommonBlock.TUP = table->EPHEM.Header.TUP;

	if (L == RTCC_MPT_CSM)
	{
		RTCCONLINEMON.TextBuffer[0] = "CSM";
	}
	else
	{
		RTCCONLINEMON.TextBuffer[0] = "LEM";
	}
	EMGPRINT("EMSEPH", 12);

	//Update mission plan table display
	PMDMPT();
	return table->EPHEM.table.back();
}

void RTCC::EMSMISS(EMSMISSInputTable &in)
{
	MANTIMESData mantimes;
	EphemerisData sv1;
	double dt, dt2, t_right, StopCondParam, R_E;

	MissionPlanTable *mpt = NULL;
	
	if (in.ManeuverIndicator)
	{
		mpt = GetMPTPointer(in.VehicleCode);
	}

	unsigned i = 0, NextMan;
	bool manflag = false, surfaceflag = false;

	dt = in.MaxIntegTime;

	in.NIAuxOutputTable.LunarStayBeginGMT = -1;
	in.NIAuxOutputTable.LunarStayEndGMT = -1;

	if (in.EphemerisBuildIndicator)
	{
		if (in.landed)
		{
			sv1.GMT = in.EphemerisLeftLimitGMT;
		}
		else
		{
			sv1 = coast(in.AnchorVector, in.EphemerisLeftLimitGMT - in.AnchorVector.GMT);
		}

		if (!in.landed && in.EphemTableIndicator->table.size() == 0)
		{
			in.EphemTableIndicator->table.push_back(sv1);
		}
		if (in.landed)
		{
			dt2 = in.EphemerRightLimitGMT - in.EphemerisLeftLimitGMT;
		}
		else
		{
			dt2 = in.EphemerRightLimitGMT - in.EphemTableIndicator->table.back().GMT;
		}
		
		if (dt2 < dt)
		{
			dt = dt2;
		}
	}
	else
	{
		sv1 = in.AnchorVector;
	}

	if (in.ManeuverIndicator && in.ManCutoffIndicator != 2)
	{
		dt2 = 1e70;
		for (unsigned i = 0;i < mpt->mantable.size();i++)
		{
			if (i < in.IgnoreManueverNumber)
			{
				continue;
			}

			if (mpt->mantable[i].FrozenManeuverInd == false)
			{
				if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_PGNS_DESCENT)
				{
					//Call PMMLDP

					PMMLDPInput tigin;

					tigin.CurrentManeuver = i;
					tigin.HeadsUpDownInd = mpt->mantable[i].HeadsUpDownInd;
					tigin.mpt = mpt;

					tigin.sv.R = sv1.R;
					tigin.sv.V = sv1.V;
					tigin.sv.MJD = OrbMech::MJDfromGET(sv1.GMT, GMTBASE);
					tigin.sv.gravref = GetGravref(sv1.RBI);

					if (i == 0)
					{
						tigin.sv.mass = mpt->TotalInitMass;
					}
					else
					{
						tigin.sv.mass = mpt->mantable[i - 1].TotalMassAfter;
					}
					tigin.TLAND = GETfromGMT(mpt->mantable[i].GMT_BO);
					tigin.TrimAngleInd = mpt->mantable[i].TrimAngleInd;

					PMMLDP(tigin, mpt->mantable[i]);
				}
			}

			t_right = mpt->mantable[i].GMTMAN;
			//Is the maneuver upcoming and also not beyond the integration limit?
			if (t_right > sv1.GMT && t_right - sv1.GMT < dt)
			{
				dt2 = t_right - sv1.GMT;
				if (in.ManCutoffIndicator == 1)
				{
					NextMan = i;
					manflag = true;
				}
				break;
			}
		}
		if (dt2 < dt)
		{
			dt = dt2;
		}
	}

	if (in.landed || length(in.AnchorVector.R) - 1.0 < MCSMLR)
	{
		surfaceflag = true;
	}

	EphemerisData svtemp;

	if (surfaceflag)
	{
		double GMT;
		
		if (in.EphemerisBuildIndicator && in.EphemTableIndicator->table.size() == 0)
		{
			GMT = sv1.GMT;
		}
		else
		{
			GMT = sv1.GMT + in.LunarEphemDT;
		}

		double GMT2 = sv1.GMT + dt;
		MATRIX3 Rot;
		VECTOR3 R, V, U_M;
		OBJHANDLE hMoon = oapiGetObjectByName("Moon");
		bool stop = false;
		double lat = BZLSDISP.lat[RTCC_LMPOS_BEST];
		double lng = BZLSDISP.lng[RTCC_LMPOS_BEST];
		double r = MCSMLR;

		while (stop == false)
		{
			if (in.EphemerisBuildIndicator == false)
			{
				GMT = GMT2;
			}

			if (GMT + in.LunarEphemDT >= GMT2)
			{
				GMT = GMT2;
				stop = true;
				if (manflag == false)
				{
					in.NIAuxOutputTable.LunarStayEndGMT = GMT;
				}
			}

			Rot = OrbMech::GetRotationMatrix(BODY_MOON, OrbMech::MJDfromGET(GMT, GMTBASE));
			R = OrbMech::r_from_latlong(lat, lng, r);
			R = rhmul(Rot, R);
			U_M = rhmul(Rot, _V(0, 0, 1));
			V = crossp(U_M, R)*OrbMech::w_Moon;
			V = rhmul(Rot, V);

			svtemp.R = R;
			svtemp.V = V;
			svtemp.RBI = BODY_MOON;
			svtemp.GMT = GMT;

			if (in.EphemerisBuildIndicator)
			{
				in.EphemTableIndicator->table.push_back(svtemp);
			}

			if (stop == false)
			{
				GMT += in.LunarEphemDT;
			}
		}
	}
	else
	{
		CoastIntegrator coast(sv1.R, sv1.V, OrbMech::MJDfromGET(sv1.GMT, GMTBASE), dt, sv1.RBI, -1);
		bool stop = false;

		while (stop == false)
		{
			stop = coast.iteration();

			svtemp.RBI = coast.GetGravRef();
			svtemp.GMT = OrbMech::GETfromMJD(coast.GetMJD(), GMTBASE);
			svtemp.R = coast.GetPosition();
			svtemp.V = coast.GetVelocity();

			//Additional stop conditions
			if (in.StopParamRefFrame == 2 || svtemp.RBI == in.StopParamRefFrame)
			{
				if (in.CutoffIndicator == 1)
				{
					if (svtemp.RBI == BODY_EARTH)
					{
						StopCondParam = in.EarthRelStopParam;
					}
					else
					{
						StopCondParam = in.MoonRelStopParam;
					}
					if (length(svtemp.R) < StopCondParam)
					{
						stop = true;
						manflag = false;
					}
				}
				else if (in.CutoffIndicator == 2)
				{
					//TBD: Reference switch
				}
				else if (in.CutoffIndicator == 3)
				{
					if (svtemp.RBI == BODY_EARTH)
					{
						StopCondParam = in.EarthRelStopParam;
						R_E = OrbMech::R_Earth;
					}
					else
					{
						StopCondParam = in.MoonRelStopParam;
						R_E = MCSMLR;
					}
					if (length(svtemp.R) - R_E < StopCondParam)
					{
						stop = true;
						manflag = false;
					}
				}
			}

			if (in.EphemerisBuildIndicator)
			{
				in.EphemTableIndicator->table.push_back(svtemp);
			}
		}
	}

	if (manflag == false)
	{
		in.NIAuxOutputTable.sv_cutoff = svtemp;
		in.NIAuxOutputTable.TerminationCode = 0;
		in.NIAuxOutputTable.ErrorCode = 0;
		in.NIAuxOutputTable.ManeuverNumber = 0;

		//Write header
		if (in.EphemerisBuildIndicator)
		{
			in.EphemTableIndicator->Header.NumVec = in.EphemTableIndicator->table.size();
			in.EphemTableIndicator->Header.Offset = 0;
			in.EphemTableIndicator->Header.TL = in.EphemTableIndicator->table.front().GMT;
			in.EphemTableIndicator->Header.TR = in.EphemTableIndicator->table.back().GMT;
			in.EphemTableIndicator->Header.VEH = in.VehicleCode;
		}
		return;
	}

	i = NextMan;

	if (in.ManTimesIndicator)
	{
		mantimes.ManData[0] = svtemp.GMT;
	}

	//PMMRKJ
	if (mpt->mantable[i].AttitudeCode <= 5)
	{
		PMMRKJInputArray integin;

		integin.A = 0.0;
		integin.AT = mpt->mantable[i].A_T;
		integin.DENSMULT = 1.0;
		integin.DOCKANG = mpt->mantable[i].DockingAngle;
		integin.DPSScale = mpt->mantable[i].DPSScaleFactor;
		integin.DTMAN = mpt->mantable[i].dt;
		integin.DTOUT = 10.0;
		integin.DTPS10 = mpt->mantable[i].DT_10PCT;
		integin.DTU = mpt->mantable[i].dt_ullage;
		integin.DVMAN = mpt->mantable[i].dv;
		integin.HeadsUpDownInd = mpt->mantable[i].HeadsUpDownInd;
		integin.IC = mpt->mantable[i].CommonBlock.ConfigCode.to_ulong();
		integin.KAUXOP = 1;
		if (in.EphemerisBuildIndicator)
		{
			integin.KEPHOP = 2;
		}
		else
		{
			integin.KEPHOP = 0;
		}
		integin.KTRIMOP = mpt->mantable[i].TrimAngleInd;
		integin.LMDESCJETT = 1e70;
		if (mpt->mantable[i].AttitudesInput)
		{
			integin.MANOP = -mpt->mantable[i].AttitudeCode;
		}
		else
		{
			integin.MANOP = mpt->mantable[i].AttitudeCode;
		}

		integin.sv0 = svtemp;
		integin.ThrusterCode = mpt->mantable[i].Thruster;
		integin.TVC = mpt->mantable[i].TVC;
		integin.UllageOption = mpt->mantable[i].UllageThrusterOpt;
		integin.ExtDVCoordInd = mpt->mantable[i].Word67i[0];
		if (mpt->mantable[i].Word67i[0])
		{
			integin.VG = mpt->mantable[i].dV_LVLH;
		}
		else
		{
			integin.VG = mpt->mantable[i].dV_inertial;
		}

		integin.WDMULT = 1.0;
		integin.XB = mpt->mantable[i].X_B;
		integin.YB = mpt->mantable[i].Y_B;
		integin.ZB = mpt->mantable[i].Z_B;
		integin.CAPWT = 0.0;

		if (mpt->mantable[i].ConfigCodeBefore[RTCC_CONFIG_C])
		{
			if (i == 0)
			{
				integin.CSMWT = mpt->CommonBlock.CSMMass;
			}
			else
			{
				integin.CSMWT = mpt->mantable[i - 1].CommonBlock.CSMMass;
			}
			integin.CAPWT += integin.CSMWT;
		}

		if (mpt->mantable[i].ConfigCodeBefore[RTCC_CONFIG_S])
		{
			if (i == 0)
			{
				integin.SIVBWT = mpt->CommonBlock.SIVBMass;
			}
			else
			{
				integin.SIVBWT = mpt->mantable[i - 1].CommonBlock.SIVBMass;
			}
			integin.CAPWT += integin.SIVBWT;
		}

		if (mpt->mantable[i].ConfigCodeBefore[RTCC_CONFIG_A])
		{
			if (i == 0)
			{
				integin.LMAWT = mpt->CommonBlock.LMAscentMass;
			}
			else
			{
				integin.LMAWT = mpt->mantable[i - 1].CommonBlock.LMAscentMass;
			}
			integin.CAPWT += integin.LMAWT;
		}

		if (mpt->mantable[i].ConfigCodeBefore[RTCC_CONFIG_D])
		{
			if (i == 0)
			{
				integin.LMDWT = mpt->CommonBlock.LMDescentMass;
			}
			else
			{
				integin.LMDWT = mpt->mantable[i - 1].CommonBlock.LMDescentMass;
			}
			integin.CAPWT += integin.LMDWT;
		}

		int Ierr;

		CSMLMPoweredFlightIntegration numin(this, integin, Ierr, in.EphemTableIndicator, in.AuxTableIndicator);
		numin.PMMRKJ();
	}
	else if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
	{
		//TLI simulation

		EphemerisDataTable *E;
		RTCCNIInputTable integin;

		integin.R = svtemp.R;
		integin.V = svtemp.V;
		integin.DTOUT = in.ManEphemDT;
		integin.WDMULT = 1.0;
		integin.DENSMULT = in.DensityMultiplier;
		integin.IEPHOP = 1;
		integin.KAUXOP = true;
		integin.MAXSTO = 1000;

		if (i == 0)
		{
			integin.Area = mpt->ConfigurationArea;
			integin.CSMWT = mpt->CommonBlock.CSMMass;
			integin.LMAWT = mpt->CommonBlock.LMAscentMass;
			integin.LMDWT = mpt->CommonBlock.LMDescentMass;
			integin.SIVBWT = mpt->CommonBlock.SIVBMass;
			integin.CAPWT = mpt->TotalInitMass;
		}
		else
		{
			integin.Area = mpt->mantable[i - 1].TotalAreaAfter;
			integin.CSMWT = mpt->mantable[i - 1].CommonBlock.CSMMass;
			integin.LMAWT = mpt->mantable[i - 1].CommonBlock.LMAscentMass;
			integin.LMDWT = mpt->mantable[i - 1].CommonBlock.LMDescentMass;
			integin.SIVBWT = mpt->mantable[i - 1].CommonBlock.SIVBMass;
			integin.CAPWT = mpt->mantable[i - 1].TotalMassAfter;
		}
		integin.MANOP = mpt->mantable[i].AttitudeCode;
		integin.ThrusterCode = mpt->mantable[i].Thruster;
		integin.IC = mpt->mantable[i].ConfigCodeBefore.to_ulong();
		integin.MVC = mpt->mantable[i].TVC;
		integin.IFROZN = mpt->mantable[i].FrozenManeuverInd;
		integin.IREF = mpt->mantable[i].RefBodyInd;
		integin.ICOORD = mpt->mantable[i].CoordSysInd;
		integin.GMTBASE = GMTBASE;
		integin.GMTI = mpt->mantable[i].GMTMAN;
		integin.DTINP = mpt->mantable[i].dt;
		integin.R_frozen = mpt->mantable[i].FrozenManeuverVector.R;
		integin.V_frozen = mpt->mantable[i].FrozenManeuverVector.V;
		integin.GMTI_frozen = mpt->mantable[i].FrozenManeuverVector.GMT;
		//e_N
		integin.Params[0] = mpt->mantable[i].dV_inertial.z;
		//C3
		integin.Params[1] = mpt->mantable[i].dV_LVLH.x;
		//alpha_D
		integin.Params[2] = mpt->mantable[i].dV_LVLH.y;
		//f
		integin.Params[3] = mpt->mantable[i].dV_LVLH.z;
		//P
		integin.Params[4] = mpt->mantable[i].Word67d;
		//K5
		integin.Params[5] = mpt->mantable[i].Word68;
		//R_T or T_M
		integin.Params[6] = mpt->mantable[i].Word69;
		//V_T
		integin.Params[7] = mpt->mantable[i].Word70;
		//gamma_T
		integin.Params[8] = mpt->mantable[i].Word71;
		//G_T
		integin.Params[9] = mpt->mantable[i].Word72;
		integin.Params[10] = mpt->mantable[i].Word73;
		integin.Params[11] = mpt->mantable[i].Word74;
		integin.Params[13] = mpt->mantable[i].Word76;
		integin.Params[14] = mpt->mantable[i].Word77;
		integin.Word68i[0] = mpt->mantable[i].Word78i[0];
		integin.Word68i[1] = mpt->mantable[i].Word78i[1];
		integin.Params2[0] = mpt->mantable[i].Word79;
		integin.Params2[1] = mpt->mantable[i].Word80;
		integin.Params2[2] = mpt->mantable[i].Word81;
		integin.Params2[3] = mpt->mantable[i].Word82;
		integin.Params2[4] = mpt->mantable[i].Word83;
		integin.Params2[5] = mpt->mantable[i].Word84;

		if (in.EphemerisBuildIndicator)
		{
			E = in.EphemTableIndicator;
		}
		else
		{
			E = NULL;
		}

		int err;
		TLIGuidanceSim numin(this, integin, err, E, in.AuxTableIndicator);
		numin.PCMTRL();
	}
	else if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_PGNS_DESCENT)
	{
		//Descent simulation

		PMMLDIInput integin;
		EphemerisDataTable *E;

		integin.sv.R = svtemp.R;
		integin.sv.V = svtemp.V;
		integin.sv.MJD = OrbMech::MJDfromGET(svtemp.GMT, GMTBASE);
		integin.sv.gravref = GetGravref(svtemp.RBI);

		integin.TLAND = GETfromGMT(mpt->mantable[i].GMT_BO);

		if (i == 0)
		{
			integin.sv.mass = mpt->CommonBlock.LMAscentMass + mpt->CommonBlock.LMDescentMass;
			integin.W_LMA = mpt->CommonBlock.LMAscentMass;
			integin.W_LMD = mpt->CommonBlock.LMDescentMass;
		}
		else
		{
			integin.sv.mass = mpt->mantable[i - 1].CommonBlock.LMAscentMass + mpt->mantable[i - 1].CommonBlock.LMDescentMass;
			integin.W_LMA = mpt->mantable[i - 1].CommonBlock.LMAscentMass;
			integin.W_LMD = mpt->mantable[i - 1].CommonBlock.LMDescentMass;
		}

		if (in.EphemerisBuildIndicator)
		{
			E = in.EphemTableIndicator;
		}
		else
		{
			E = NULL;
		}

		PMMLDI(integin, *in.AuxTableIndicator, E);

		svtemp.R = in.AuxTableIndicator->R_BO;
		svtemp.V = in.AuxTableIndicator->V_BO;
		svtemp.GMT = in.AuxTableIndicator->GMT_BO;
		svtemp.GMT += 0.1;

		if (E)
		{
			E->table.push_back(svtemp);
		}

		in.NIAuxOutputTable.LunarStayBeginGMT = svtemp.GMT;
		in.NIAuxOutputTable.LunarStayEndGMT = 1e71;
	}
	else
	{
		//Ascent simulation

		PMMLAIInput integin;
		EphemerisDataTable *E;

		if (i == 0)
		{
			integin.m0 = mpt->CommonBlock.LMAscentMass;
		}
		else
		{
			integin.m0 = mpt->mantable[i - 1].CommonBlock.LMAscentMass;
		}

		if (mpt->mantable[i].FrozenManeuverInd)
		{
			integin.sv_CSM.R.x = mpt->mantable[i].Word73;
			integin.sv_CSM.R.y = mpt->mantable[i].Word74;
			integin.sv_CSM.R.z = mpt->mantable[i].Word75;
			integin.sv_CSM.V.x = mpt->mantable[i].Word76;
			integin.sv_CSM.V.y = mpt->mantable[i].Word77;
			integin.sv_CSM.V.z = mpt->mantable[i].Word78d;
			integin.sv_CSM.MJD = OrbMech::MJDfromGET(mpt->mantable[i].Word79, GMTBASE);
			integin.sv_CSM.gravref = oapiGetObjectByName("Moon");
		}
		else
		{
			EphemerisData SV;
			ELFECH(svtemp.GMT, RTCC_MPT_CSM, SV);
			integin.sv_CSM.R = SV.R;
			integin.sv_CSM.V = SV.V;
			integin.sv_CSM.MJD = OrbMech::MJDfromGET(SV.GMT, GMTBASE);
			integin.sv_CSM.gravref = GetGravref(SV.RBI);
		}
		
		integin.t_liftoff = mpt->mantable[i].GMTMAN;
		integin.v_LH = mpt->mantable[i].dV_LVLH.z;
		integin.v_LV = mpt->mantable[i].dV_LVLH.x;

		if (in.EphemerisBuildIndicator)
		{
			E = in.EphemTableIndicator;
		}
		else
		{
			E = NULL;
		}
		
		PMMLAI(integin, *in.AuxTableIndicator, E);

		in.NIAuxOutputTable.LunarStayEndGMT = integin.t_liftoff;
	}

	if (in.AuxTableIndicator)
	{
		in.NIAuxOutputTable.sv_cutoff.R = in.AuxTableIndicator->R_BO;
		in.NIAuxOutputTable.sv_cutoff.V = in.AuxTableIndicator->V_BO;
		in.NIAuxOutputTable.sv_cutoff.GMT = in.AuxTableIndicator->GMT_BO;
		in.NIAuxOutputTable.sv_cutoff.RBI = in.AuxTableIndicator->RBI;
	}
	in.NIAuxOutputTable.TerminationCode = 1;
	in.NIAuxOutputTable.ErrorCode = 0;
	in.NIAuxOutputTable.ManeuverNumber = i + 1;

	if (in.ManTimesIndicator)
	{
		mantimes.ManData[1] = in.AuxTableIndicator->GMT_BO;
		in.ManTimesIndicator->Table.push_back(mantimes);
	}
}

void RTCC::EMGVECSTInput(int L, EphemerisData sv)
{
	HistoryAnchorVectorTable *tab;
	if (L == RTCC_MPT_CSM)
	{
		tab = &EZANCHR1;
	}
	else
	{
		tab = &EZANCHR3;
	}

	//Move all SVs up by one place
	for (int i = 0;i < 9;i++)
	{
		tab->AnchorVectors[i] = tab->AnchorVectors[i + 1];
	}
	//Adjust number of anchor vectors
	if (tab->num < 10)
	{
		tab->num++;
	}
	//Make the last SV in table the current
	tab->AnchorVectors[9] = sv;
}

int RTCC::EMGVECSTOutput(int L, EphemerisData &sv)
{
	HistoryAnchorVectorTable *tab;
	if (L == RTCC_MPT_CSM)
	{
		tab = &EZANCHR1;
	}
	else
	{
		tab = &EZANCHR3;
	}

	if (tab->num < 1)
	{
		return 1;
	}

	sv = tab->AnchorVectors[9];

	return 0;
}

EphemerisData RTCC::EMMENI(EMSMISSInputTable &in, EphemerisData sv, double dt)
{
	EphemerisData svtemp;
	double funct, RCALC, RES1, TIME, TIME_old, t_new;
	int INITE = 0;
	int gravref = sv.RBI;
	bool allow_stop, ephstore;
	int stopcondition = in.CutoffIndicator;

	CoastIntegrator coast(sv.R, sv.V, OrbMech::MJDfromGET(sv.GMT, GMTBASE), dt, sv.RBI, -1);
	bool stop = false;

	if (in.CutoffIndicator > 0)
	{
		allow_stop = false;
	}
	else
	{
		allow_stop = true;
	}
	ephstore = in.EphemerisBuildIndicator;

	while (stop == false)
	{
		if (abs(coast.GetTime() - dt) < 1e-6)
		{
			allow_stop = true;
			stopcondition = 0;
		}

		stop = coast.iteration(allow_stop);

		svtemp.RBI = coast.GetGravRef();
		svtemp.GMT = OrbMech::GETfromMJD(coast.GetMJD(), GMTBASE);
		svtemp.R = coast.GetPosition();
		svtemp.V = coast.GetVelocity();

		//Additional stop conditions
		if (stop == false && in.CutoffIndicator > 0 && (in.StopParamRefFrame == 2 || svtemp.RBI == in.StopParamRefFrame))
		{
			if (in.CutoffIndicator != 2 && gravref != svtemp.RBI)
			{
				gravref = svtemp.RBI;
				INITE = 0;
			}

			TIME = coast.GetTime();

			//Calculate stop condition value
			if (in.CutoffIndicator == 1)
			{
				funct = length(svtemp.R);
			}
			else if (in.CutoffIndicator == 2)
			{
				if (svtemp.RBI == BODY_EARTH)
				{
					funct = 1.0;
				}
				else
				{
					funct = -1.0;
				}
			}
			else if (in.CutoffIndicator == 3)
			{
				if (svtemp.RBI == BODY_EARTH)
				{
					funct = length(svtemp.R) - OrbMech::R_Earth;
				}
				else
				{
					funct = length(svtemp.R) - MCSMLR;
				}
			}
			else if (in.CutoffIndicator == 4)
			{
				funct = dotp(unit(svtemp.R), unit(svtemp.V));
			}

			if (svtemp.RBI == BODY_EARTH)
			{
				RCALC = funct - in.EarthRelStopParam;
			}
			else
			{
				RCALC = funct - in.MoonRelStopParam;
			}
		
			//1st pass
			if (INITE == 0)
			{
				INITE = -1;
			}
			//Not bounded, not 1st pass
			else if (INITE < 0)
			{
				if (RCALC*RES1 < 0)
				{
					//Turn of ephemeris storage
					ephstore = false;

					if (in.CutoffIndicator == 2)
					{
						INITE = 2;

						coast.AdjustTF(TIME);
						allow_stop = true;
					}
					else
					{
						INITE = 1;

						t_new = OrbMech::LinearInterpolation(RES1, TIME_old, RCALC, TIME, 0.0);
						coast.AdjustTF(t_new);
					}
				}
			}
			//Bounded
			else if (INITE == 1)
			{
				t_new = OrbMech::LinearInterpolation(RES1, TIME_old, RCALC, TIME, 0.0);
				coast.AdjustTF(t_new);
				if (abs(TIME - t_new) < 1e-6)
				{
					allow_stop = true;
				}
			}

			RES1 = RCALC;
			TIME_old = TIME;
		}

		//Store when allowed, or when desired + iteration stopped
		if (in.EphemTableIndicator && (ephstore || (stop && in.EphemerisBuildIndicator)))
		{
			in.EphemTableIndicator->table.push_back(svtemp);
		}
	}

	in.NIAuxOutputTable.TerminationCode = stopcondition;

	return svtemp;
}

int RTCC::EMMXTR(double GMT, double rmag, double vmag, double rtasc, double decl, double fpav, double az, VECTOR3 &R, VECTOR3 &V)
{
	MATRIX3 Rot;
	OrbMech::adbar_from_rv(rmag, vmag, rtasc, decl, fpav, az, R, V);
	Rot = OrbMech::GetRotationMatrix(BODY_EARTH, OrbMech::MJDfromGET(GMT, GMTBASE));
	R = rhmul(Rot, R);
	V = rhmul(Rot, V);

	return 0;
}

bool RTCC::MPTHasManeuvers(int L)
{
	if (L == RTCC_MPT_LM)
	{
		if (PZMPTLEM.mantable.size() > 0)
		{
			return true;
		}
	}
	else
	{
		if (PZMPTCSM.mantable.size() > 0)
		{
			return true;
		}
	}

	return false;
}

int RTCC::PMMWTC(int med)
{
	MPTVehicleDataBlock CommonBlock;
	MissionPlanTable *table;
	double W, WDOT[6], TL[6], DVREM, FuelR[6];
	unsigned IBLK;
	int Thruster;

	WDOT[0] = MCTCW1;
	WDOT[1] = MCTSW1;
	WDOT[2] = MCTWAV;
	WDOT[3] = MCTLW1;
	WDOT[4] = MCTAW1;
	WDOT[5] = MCTDW1;
	TL[0] = MCTCT1;
	TL[1] = MCTST1;
	TL[2] = MCTSAV;
	TL[3] = MCTLT1;
	TL[4] = MCTAT1;
	TL[5] = MCTDT1;

	//M49: Thruster Fuel Remaining
	if (med == 49)
	{
		double RCSFuelUsed, MainEngineFuelUsed, TotalMassAfter;

		table = GetMPTPointer(med_m49.Table);
		IBLK = table->LastExecutedManeuver + 1;
		double FuelC[6];

		FuelC[0] = med_m49.CSMRCSFuelRemaining;
		FuelC[1] = med_m49.SPSFuelRemaining;
		FuelC[2] = med_m49.SIVBFuelRemaining;
		FuelC[3] = med_m49.LMRCSFuelRemaining;
		FuelC[4] = med_m49.LMAPSFuelRemaining;
		FuelC[5] = med_m49.LMDPSFuelRemaining;
		int N = 0;
		int i;
	RTCC_PMMWTC_2:
		//Read IBLK of MPT
		if (IBLK == 1)
		{
			CommonBlock = table->CommonBlock;
		}
		else
		{
			CommonBlock = table->mantable[IBLK - 2].CommonBlock;
			Thruster = table->mantable[IBLK - 2].Thruster;
			RCSFuelUsed = table->mantable[IBLK - 2].RCSFuelUsed;
			MainEngineFuelUsed = table->mantable[IBLK - 2].MainEngineFuelUsed;
			TotalMassAfter = table->mantable[IBLK - 2].TotalMassAfter;
			DVREM = table->mantable[IBLK - 2].DVREM;
		}
		FuelR[0] = CommonBlock.CSMRCSFuelRemaining;
		FuelR[1] = CommonBlock.SPSFuelRemaining;
		FuelR[2] = CommonBlock.SIVBFuelRemaining;
		FuelR[3] = CommonBlock.LMRCSFuelRemaining;
		FuelR[4] = CommonBlock.LMAPSFuelRemaining;
		FuelR[5] = CommonBlock.LMDPSFuelRemaining;
		if (N == 0)
		{
			goto RTCC_PMMWTC_3;
		}
		if (CommonBlock.ConfigChangeInd == 0)
		{
			goto RTCC_PMMWTC_5;
		}
	RTCC_PMMWTC_3:
		if (CommonBlock.ConfigCode[RTCC_CONFIG_C] == false)
		{
			FuelC[0] = -1;
			FuelC[1] = -1;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_S] == false)
		{
			FuelC[2] = -1;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_A] == false)
		{
			FuelC[3] = -1;
			FuelC[4] = -1;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_D] == false)
		{
			FuelC[5] = -1;
		}
	RTCC_PMMWTC_5:
		i = 0;
	RTCC_PMMWTC_6:
		if (FuelC[i] < 0)
		{
			goto RTCC_PMMWTC_11;
		}
		FuelR[i] = FuelC[i];
		if (IBLK <= 1)
		{
			goto RTCC_PMMWTC_11;
		}
		if (N > 0)
		{
			goto RTCC_PMMWTC_7;
		}
		if (MPTIsPrimaryThruster(Thruster, i) == false)
		{
			goto RTCC_PMMWTC_11;
		}
		goto RTCC_PMMWTC_8;
	RTCC_PMMWTC_7:
		if (MPTIsPrimaryThruster(Thruster, i) == false)
		{
			goto RTCC_PMMWTC_9;
		}
		if (MPTIsRCSThruster(Thruster))
		{
			FuelR[i] = FuelR[i] - RCSFuelUsed;
		}
		else
		{
			FuelR[i] = FuelR[i] - MainEngineFuelUsed;
		}
	RTCC_PMMWTC_8:
		DVREM = TL[i] / WDOT[i] * log(TotalMassAfter/(TotalMassAfter - FuelR[i]));
		goto RTCC_PMMWTC_10;
	RTCC_PMMWTC_9:
		if (MPTIsUllageThruster(Thruster, i))
		{
			FuelR[i] = FuelR[i] - RCSFuelUsed;
		}
	RTCC_PMMWTC_10:
		FuelC[i] = FuelR[i];
	RTCC_PMMWTC_11:
		if (i < 5)
		{
			i++;
			goto RTCC_PMMWTC_6;
		}
		CommonBlock.CSMRCSFuelRemaining = FuelR[0];
		CommonBlock.SPSFuelRemaining = FuelR[1];
		CommonBlock.SIVBFuelRemaining = FuelR[2];
		CommonBlock.LMRCSFuelRemaining = FuelR[3];
		CommonBlock.LMAPSFuelRemaining = FuelR[4];
		CommonBlock.LMDPSFuelRemaining = FuelR[5];
		if (IBLK == 1)
		{
			table->CommonBlock = CommonBlock;
		}
		else
		{
			table->mantable[IBLK - 2].CommonBlock = CommonBlock;
			table->mantable[IBLK - 2].DVREM = DVREM;
		}
		if (IBLK - 1 < table->ManeuverNum)
		{
			N = 1;
			IBLK++;
			goto RTCC_PMMWTC_2;
		}
		//Update MPT and DMT displays
		EMSNAP(0, 7);
	}
	//M50: Vehicle Weight
	if (med == 50)
	{
		double WeightGMT, W, WTV[4];

		table = GetMPTPointer(med_m50.Table);
		IBLK = table->LastExecutedManeuver + 1;
		WeightGMT = GMTfromGET(med_m50.WeightGET);

		if (IBLK == 1)
		{
			CommonBlock = table->CommonBlock;
		}
		else
		{
			if (WeightGMT < table->mantable[IBLK - 2].GMT_BO)
			{
				PMXSPT("PMMWTC", 23);
				return 23;
			}
			CommonBlock = table->mantable[IBLK - 2].CommonBlock;
		}
		WTV[0] = CommonBlock.CSMMass;
		WTV[1] = CommonBlock.SIVBMass;
		WTV[2] = CommonBlock.LMAscentMass;
		WTV[3] = CommonBlock.LMDescentMass;

		W = 0.0;

		if (CommonBlock.ConfigCode[RTCC_CONFIG_C])
		{
			if (med_m50.CSMWT >= 0)
			{
				WTV[0] = med_m50.CSMWT;
			}
			W = W + WTV[0];
		}

		if (CommonBlock.ConfigCode[RTCC_CONFIG_S])
		{
			if (med_m50.SIVBWT >= 0)
			{
				WTV[1] = med_m50.SIVBWT;
			}
			W = W + WTV[1];
		}

		//Do we have an ascent stage?
		if (CommonBlock.ConfigCode[RTCC_CONFIG_A])
		{
			goto RTCC_PMMWTC_15;
		}
		if (CommonBlock.ConfigCode[RTCC_CONFIG_D])
		{
			if (med_m50.LMWT >= 0)
			{
				WTV[3] = med_m50.LMWT;
			}
			W = W + WTV[3];
		}
		goto RTCC_PMMWTC_17;
	RTCC_PMMWTC_15:
		//Ascent stage weight input?
		if (med_m50.LMASCWT >= 0)
		{
			//Set initial ascent stage weight
			WTV[2] = med_m50.LMASCWT;
		}
		//Total stage weight input?
		if (med_m50.LMWT < 0)
		{
			//Do we have also have a descent stage?
			if (CommonBlock.ConfigCode[RTCC_CONFIG_D])
			{
				goto RTCC_PMMWTC_15A;
			}
			else
			{
				goto RTCC_PMMWTC_16;
			}
		}
		//Do we have also have a descent stage?
		if (CommonBlock.ConfigCode[RTCC_CONFIG_D])
		{
			//Yes, calculate descent stage weight from total weight, minus ascent stage weight
			WTV[3] = med_m50.LMWT - WTV[2];
		RTCC_PMMWTC_15A:
			W = W + WTV[3];
		}
		else
		{
			//No, total weight equals ascent stage weight (can override input from LMASCWT earlier)
			WTV[3] = med_m50.LMWT;
		}
	RTCC_PMMWTC_16:
		W = W + WTV[2];
	RTCC_PMMWTC_17:
		CommonBlock.CSMMass = WTV[0];
		CommonBlock.SIVBMass = WTV[1];
		CommonBlock.LMAscentMass = WTV[2];
		CommonBlock.LMDescentMass = WTV[3];
		if (IBLK > 1)
		{
		RTCC_PMMWTC_17A:
			goto RTCC_PMMWTC_17B;
		}
		double t_aw = table->SIVBVentingBeginGET;
		if (t_aw >= med_m50.WeightGET)
		{
			goto RTCC_PMMWTC_19;
		}
		else
		{
			goto RTCC_PMMWTC_17A;
		}
	RTCC_PMMWTC_17B:
		//TBD: Call PLAWDT
		//TBD: Adjust total and vehicle weights for expendables
		if (IBLK <= 1)
		{
			goto RTCC_PMMWTC_19;
		}
		int I = MPTGetPrimaryThruster(table->mantable[IBLK - 2].Thruster);
		FuelR[0] = CommonBlock.CSMRCSFuelRemaining;
		FuelR[1] = CommonBlock.SPSFuelRemaining;
		FuelR[2] = CommonBlock.SIVBFuelRemaining;
		FuelR[3] = CommonBlock.LMRCSFuelRemaining;
		FuelR[4] = CommonBlock.LMAPSFuelRemaining;
		FuelR[5] = CommonBlock.LMDPSFuelRemaining;
		DVREM = TL[I] / WDOT[I] * log(table->mantable[IBLK - 2].TotalMassAfter/(table->mantable[IBLK - 2].TotalMassAfter- FuelR[I]));
		//DTWRITE: IBLK of MPT
		table->mantable[IBLK - 2].CommonBlock = CommonBlock;
		table->mantable[IBLK - 2].DVREM = DVREM;
		//DTREAD: MPT Header
		CommonBlock = table->CommonBlock;
	RTCC_PMMWTC_19:
		if (IBLK == 1)
		{
			table->TotalInitMass = W;
		}
		else
		{
			table->WeightAfterManeuver[IBLK - 2] = W;
		}
	RTCC_PMMWTC_20:
		CommonBlock.TUP = -CommonBlock.TUP;
		//DTWRITE: MPT Header
		table->CommonBlock = CommonBlock;

		//Set up input for PMSVCT
		PMSVCT(8, med_m50.Table);
	}
	//Change Vehicle Area or K-Factor
	if (med == 51)
	{
		double Area, ARI[4], ARV[4];
		int I;

		table = GetMPTPointer(med_m51.Table);
		ARI[0] = med_m51.CSMArea;
		ARI[1] = med_m51.SIVBArea;
		ARI[2] = med_m51.LMAscentArea;
		ARI[3] = med_m51.LMDescentArea;
		IBLK = table->LastExecutedManeuver + 1;
		if (IBLK == 1)
		{
			CommonBlock = table->CommonBlock;
		}
		else
		{
			CommonBlock = table->mantable[IBLK - 2].CommonBlock;
		}
		ARV[0] = CommonBlock.CSMArea;
		ARV[1] = CommonBlock.SIVBArea;
		ARV[2] = CommonBlock.LMAscentArea;
		ARV[3] = CommonBlock.LMDescentArea;
		Area = 0;
		I = 1;
	RTCC_PMMWTC_22:
		if (CommonBlock.ConfigCode[I - 1])
		{
			if (ARI[I - 1] >= 0)
			{
				ARV[I - 1] = ARI[I - 1];
			}
			if (Area < ARV[I - 1])
			{
				Area = ARV[I - 1];
			}
		}
		if (I < 4)
		{
			I++;
			goto RTCC_PMMWTC_22;
		}
		CommonBlock.CSMArea = ARV[0];
		CommonBlock.SIVBArea = ARV[1];
		CommonBlock.LMAscentArea = ARV[2];
		CommonBlock.LMDescentArea = ARV[3];
		if (IBLK > 1)
		{
			table->mantable[IBLK - 2].CommonBlock = CommonBlock;
			CommonBlock = table->CommonBlock;
		}
		if (med_m51.KFactor >= -30)
		{
			table->KFactor = med_m51.KFactor;
		}
		if (IBLK > 1)
		{
			table->AreaAfterManeuver[IBLK - 2] = Area;
		}
		else
		{
			table->ConfigurationArea = Area;
		}
		goto RTCC_PMMWTC_20;
	}
	//Input initialization parameters
	if (med == 55)
	{
		table = GetMPTPointer(med_m55.Table);

		if (table->mantable.size() > 0)
		{
			PMXSPT("PMMWTC", 72);
			return 2;
		}

		if (med_m55.DeltaDockingAngle >= -360.0)
		{
			table->DeltaDockingAngle = med_m55.DeltaDockingAngle;
		}

		std::bitset<4> cfg;
		MPTGetConfigFromString(med_m55.ConfigCode, cfg);

		if (cfg.to_ulong() > 0)
		{
			double arv[4], wtv[4], AREA;
			arv[0] = table->CommonBlock.CSMArea;
			arv[1] = table->CommonBlock.SIVBArea;
			arv[2] = table->CommonBlock.LMAscentArea;
			arv[3] = table->CommonBlock.LMDescentArea;
			wtv[0] = table->CommonBlock.CSMMass;
			wtv[1] = table->CommonBlock.SIVBMass;
			wtv[2] = table->CommonBlock.LMAscentMass;
			wtv[3] = table->CommonBlock.LMDescentMass;

			double arvs[4], wtvs[4];
			
			for (int i = 0;i < 4;i++)
			{
				arvs[i] = wtvs[i] = 0.0;
				if (CommonBlock.ConfigCode[i])
				{
					wtvs[i] = wtv[i];
					arvs[i] = arv[i];
				}
			}

			table->CommonBlock.ConfigCode = cfg;
			AREA = 0.0;
			W = 0.0;
			for (int i = 0;i < 4;i++)
			{
				if (CommonBlock.ConfigCode[i])
				{
					if (AREA < arvs[i])
					{
						AREA = arvs[i];
					}
					W = W + wtvs[i];
				}
				else
				{
					arvs[i] = 0.0;
					wtvs[i] = 0.0;
				}
			}
			table->CommonBlock.CSMArea = arvs[0];
			table->CommonBlock.CSMMass = wtvs[0];
			table->CommonBlock.SIVBArea = arvs[1];
			table->CommonBlock.SIVBMass = wtvs[1];
			table->CommonBlock.LMAscentArea = arvs[2];
			table->CommonBlock.LMAscentMass = wtvs[2];
			table->CommonBlock.LMDescentArea = arvs[3];
			table->CommonBlock.LMDescentMass = wtvs[3];
			table->TotalInitMass = W;
			table->ConfigurationArea = AREA;

			table->CommonBlock.TUP = -table->CommonBlock.TUP;

			//Set up input for PMSVCT
			PMSVCT(8, med_m55.Table);
		}
	}

	return 0;
}

void RTCC::PMSVCT(int QUEID, int L, EphemerisData *sv0, bool landed)
{
	EphemerisDataTable EPHEM;
	OrbitEphemerisTable *maineph;
	MissionPlanTable *mpt;

	if (L == RTCC_MPT_CSM)
	{
		maineph = &EZEPH1;
		mpt = &PZMPTCSM;
	}
	else
	{
		maineph = &EZEPH2;
		mpt = &PZMPTLEM;
	}

	//New anchor vector
	if (QUEID == 4)
	{
		if (landed)
		{
			mpt->GMTAV = RTCCPresentTimeGMT();
		}
		else
		{
			mpt->GMTAV = sv0->GMT;
		}
		
		//TBD: Station ID to MPT
		bool tli = false;
		for (unsigned i = 0;i < mpt->mantable.size();i++)
		{
			if (mpt->mantable[i].AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
			{
				if (mpt->mantable[i].GMTMAN > sv0->GMT)
				{
					if (mpt->mantable[i].FrozenManeuverInd == false)
					{
						//EMSMISS and PMMSPT for TLI
					}
				}
			}
		}
		
		EphemerisData sv1;
		
		if (landed == false)
		{
			sv1 = EMSEPH(1, *sv0, L, RTCCPresentTimeGMT());
		}
		EMSTRAJ(sv1, L, landed);
	}
	//Trajectory update
	if (QUEID == 8)
	{
		double T_P;
		EphemerisData sv;

		T_P = RTCCPresentTimeGMT();

		if (maineph->LUNRSTAY.LunarStayBeginGMT < T_P && T_P < maineph->LUNRSTAY.LunarStayEndGMT)
		{
			landed = true;
		}
		else
		{
			landed = false;
		}

		if (landed)
		{
			
		}
		else
		{
			double T_F, T_Left;

			if (maineph->EPHEM.table.size() > 0)
			{
				T_Left = maineph->EPHEM.table.front().GMT;
			}
			else
			{
				T_Left = 0.0;
			}


			if (T_P < T_Left)
			{
				T_F = T_Left;
			}
			else
			{
				T_F = T_P;
			}
			
			if (PMSVCTAuxVectorFetch(L, T_F, sv))
			{
				if (mpt->CommonBlock.TUP < 0)
				{
					mpt->CommonBlock.TUP = -mpt->CommonBlock.TUP;
				}

				//Error
				return;
			}
		}

		//TUP should be negative already
		if (mpt->CommonBlock.TUP > 0)
		{
			mpt->CommonBlock.TUP = -mpt->CommonBlock.TUP;
		}
		mpt->CommonBlock.TUP--;

		EMSTRAJ(sv, L, landed);
	}
}

int RTCC::PMSVEC(int L, double GMT, CELEMENTS &elem, double &KFactor, double &Area, double &Weight, std::string &StaID, int &RBI)
{
	double mu;
	MissionPlanTable *mpt = GetMPTPointer(L);
	EphemerisData sv;

	if (mpt->mantable.size() == 0)
	{
		Area = mpt->ConfigurationArea;
		Weight = mpt->TotalInitMass;
	}
	else
	{
		unsigned mancounter = 0;

		do
		{
			if (GMT < mpt->mantable[mancounter].GMTMAN)
			{
				break;
			}
			if (mancounter >= mpt->mantable.size() - 1)
			{
				break;
			}
		} while (mancounter < mpt->mantable.size());

		if (mancounter == 0U)
		{
			Area = mpt->ConfigurationArea;
			Weight = mpt->TotalInitMass;
		}
		else
		{
			Area = mpt->mantable[mancounter].TotalAreaAfter;
			Weight = mpt->mantable[mancounter].TotalMassAfter;
		}

		if (mancounter > 0U && GMT < mpt->mantable[mancounter - 1].GMT_BO + 3.0*60.0)
		{
			sv.R = mpt->mantable[mancounter - 1].R_BO;
			sv.V = mpt->mantable[mancounter - 1].V_BO;
			sv.GMT = mpt->mantable[mancounter - 1].GMT_BO;
			sv.RBI = mpt->mantable[mancounter - 1].RefBodyInd;

			goto PMSVEC_2_2;
		}
	}

	if (ELFECH(GMT, L, sv))
	{
		PMXSPT("PMSVEC", 49);
		return 1;
	}

PMSVEC_2_2:

	if (sv.RBI == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	VECTOR3 R, V;
	if (sv.RBI == BODY_EARTH)
	{
		MATRIX3 Rot = OrbMech::GetRotationMatrix(sv.RBI, GMTBASE);
		R = rhtmul(Rot, sv.R);
		V = rhtmul(Rot, sv.V);
	}
	else
	{
		R = sv.R;
		V = sv.V;
	}

	elem = OrbMech::GIMIKC(R, V, mu);
	KFactor = mpt->KFactor;
	StaID = mpt->StationID;

	return 0;
}

int RTCC::PMSVCTAuxVectorFetch(int L, double T_F, EphemerisData &sv)
{
	unsigned nbr_left;

	MissionPlanTable * mpt;
	EphemerisDataTable *maineph;
	if (L == RTCC_MPT_CSM)
	{
		maineph = &EZEPH1.EPHEM;
		mpt = &PZMPTCSM;
	}
	else
	{
		maineph = &EZEPH2.EPHEM;
		mpt = &PZMPTLEM;
	}

	if (mpt->mantable.size() == 0)
	{
		nbr_left = 1;
	}
	else
	{
		unsigned K = 0;
		do
		{
			if (T_F == mpt->TimeToEndManeuver[K])
			{
				nbr_left = 0;
				break;
			}
			else if (T_F > mpt->TimeToEndManeuver[K])
			{
				K++;
				if (K > mpt->mantable.size())
				{
					nbr_left = 1;
					break;
				}
			}
			else
			{
				if (T_F < mpt->TimeToBeginManeuver[K])
				{
					nbr_left = 1;
					break;
				}
				else
				{
					T_F = mpt->TimeToBeginManeuver[K];
					break;
				}
			}
		} while (K < mpt->mantable.size());
	}

	double T_right;
	if (maineph->table.size() > 0)
	{
		T_right = maineph->table.back().GMT;
	}
	else
	{
		T_right = 0.0;
	}

	if (T_F > T_right)
	{
		return 1;
	}

	EphemerisDataTable EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	int err = ELFECH(T_F, 1, nbr_left, L, EPHEM, MANTIMES, LUNSTAY);
	if (err)
	{
		return err;
	}
	sv = EPHEM.table[0];

	return 0;
}

void RTCC::PMSEXE(int L, double gmt)
{
	double TMNVR, TMANVR;
	bool print = false;
	TMNVR = gmt;
	MissionPlanTable *tab = GetMPTPointer(L);
	unsigned i, j, k;
	i = tab->ManeuverNum;
	if (i == 0)
	{
		tab->LastExecutedManeuver = 0;
		tab->LastFrozenManeuver = 0;
		goto RTCC_PMSEXE_2_3;
	}
	j = 1;
RTCC_PMSEXE_A:
	if (TMNVR < tab->TimeToBeginManeuver[j - 1])
	{
		goto RTCC_PMSEXE_B;
	}
	j++;
	if (j <= i)
	{
		goto RTCC_PMSEXE_A;
	}
RTCC_PMSEXE_B:
	k = j - 1;
	if (k > tab->LastExecutedManeuver)
	{
		tab->LastExecutedManeuver = k;
		print = true;
	}
	if (k + 1 > i)
	{
	RTCC_PMSEXE_2_3:
		TMANVR = 1e70;
	}
	else
	{
		TMANVR = tab->TimeToBeginManeuver[k];
	}
	tab->UpcomingManeuverGMT = TMANVR;
	if (print == false)
	{
		return;
	}
	RTCCONLINEMON.TextBuffer[0] = tab->mantable[k - 1].code;
	RTCCONLINEMON.DoubleBuffer[0] = tab->mantable[k - 1].GMTMAN;
	RTCCONLINEMON.DoubleBuffer[1] = tab->mantable[k - 1].GMT_BO;
	RTCCONLINEMON.DoubleBuffer[2] = tab->mantable[k - 1].TotalAreaAfter;
	RTCCONLINEMON.DoubleBuffer[3] = tab->mantable[k - 1].TotalMassAfter*LBS*1000.0;
	RTCCONLINEMON.VectorBuffer[0] = tab->mantable[k - 1].R_BO / OrbMech::R_Earth;
	RTCCONLINEMON.VectorBuffer[1] = tab->mantable[k - 1].V_BO*3600.0 / OrbMech::R_Earth;
	PMXSPT("PMSEXE", 36);
	EMSNAP(L, 7);
}

bool RTCC::MEDTimeInputHHMMSS(std::string vec, double &hours)
{
	int hh, mm;
	double ss;
	if (sscanf_s(vec.c_str(), "%d:%d:%lf", &hh, &mm, &ss) != 3)
	{
		return true;
	}
	hours = (double)hh + (double)mm / 60.0 + ss / 3600.0;
	return false;
}

int RTCC::PLAWDT(int L, double gmt, double &cfg_weight)
{
	double csm_weight, lma_weight, lmd_weight, sivb_weight;
	std::bitset<4> cfg;

	return PLAWDT(L, gmt, cfg, cfg_weight, csm_weight, lma_weight, lmd_weight, sivb_weight);
}

int RTCC::PLAWDT(int L, double gmt, std::bitset<4> &cfg, double &cfg_weight, double &csm_weight, double &lm_asc_weight, double &lm_dsc_weight, double &sivb_weight)
{
	MissionPlanTable *table = GetMPTPointer(L);
	unsigned i = 0;

	//No maneuver in MPT or time is before first maneuver. Use initial values
	if (table->mantable.size() == 0 || gmt <= table->mantable[0].GMT_BI)
	{
		cfg = table->CommonBlock.ConfigCode;
		cfg_weight = table->TotalInitMass;
		if (cfg[RTCC_CONFIG_C])
		{
			csm_weight = table->CommonBlock.CSMMass;
		}
		else
		{
			csm_weight = 0.0;
		}
		if (cfg[RTCC_CONFIG_A])
		{
			lm_asc_weight = table->CommonBlock.LMAscentMass;
		}
		else
		{
			lm_asc_weight = 0.0;
		}
		if (cfg[RTCC_CONFIG_D])
		{
			lm_dsc_weight = table->CommonBlock.LMDescentMass;
		}
		else
		{
			lm_dsc_weight = 0.0;
		}
		if (cfg[RTCC_CONFIG_S])
		{
			sivb_weight = table->CommonBlock.SIVBMass;
		}
		else
		{
			sivb_weight = 0.0;
		}
		return 0;
	}

	//Iterate to find maneuver
	while ((i < table->mantable.size() - 1) && (gmt >= table->mantable[i + 1].GMT_BO))
	{
		i++;
	}

	cfg = table->mantable[i].CommonBlock.ConfigCode;
	cfg_weight = table->mantable[i].TotalMassAfter;
	csm_weight = table->mantable[i].CommonBlock.CSMMass;
	lm_asc_weight = table->mantable[i].CommonBlock.LMAscentMass;
	lm_dsc_weight = table->mantable[i].CommonBlock.LMDescentMass;
	sivb_weight = table->mantable[i].CommonBlock.SIVBMass;

	return 0;
}

void RTCC::PMMIEV(double T_L)
{
	double T_D, A_Z, dt_EOI, lat_EOI, lng_EOI, azi_EOI, vel_EOI, fpa_EOI, rad_EOI;
	int day;
	
	day = GZGENCSN.RefDayOfYear;
	T_D = T_L - PZSTARGP.T_LO;
	if (T_D < MDVSTP.t_D0 || T_D > MDVSTP.t_D3)
	{
		PMXSPT("PMMIEV", 102);
		return;
	}
	A_Z = 0.0;
	if (T_D < MDVSTP.t_DS1)
	{
		for (int N = 0;N < 4;N++)
		{
			A_Z += MDVSTP.hx[0][N] * pow((T_D - MDVSTP.t_D1) / MDVSTP.t_SD1, N);
		}
	}
	else if (T_D < MDVSTP.t_DS2)
	{
		for (int N = 0;N < 4;N++)
		{
			A_Z += MDVSTP.hx[1][N] * pow((T_D - MDVSTP.t_D2) / MDVSTP.t_SD2, N);
		}
	}
	else
	{
		for (int N = 0;N < 4;N++)
		{
			A_Z += MDVSTP.hx[2][N] * pow((T_D - MDVSTP.t_D3) / MDVSTP.t_SD3, N);
		}
	}
	GZLTRA.Azimuth = A_Z;
	A_Z *= DEG;
	dt_EOI = MDLIEV[0] + MDLIEV[1] * A_Z + MDLIEV[2] * pow(A_Z, 2) + MDLIEV[3] * pow(A_Z, 3);
	lat_EOI = MDLIEV[4] + MDLIEV[5] * A_Z + MDLIEV[6] * pow(A_Z, 2) + MDLIEV[7] * pow(A_Z, 3);
	lng_EOI = MDLIEV[8] + MDLIEV[9] * A_Z + MDLIEV[10] * pow(A_Z, 2) + MDLIEV[11] * pow(A_Z, 3);
	azi_EOI = MDLIEV[12] + MDLIEV[13] * A_Z + MDLIEV[14] * pow(A_Z, 2) + MDLIEV[15] * pow(A_Z, 3);
	rad_EOI = MDLEIC[0];
	fpa_EOI = MDLEIC[1];
	vel_EOI = MDLEIC[2];

	lat_EOI /= DEG;
	lng_EOI /= DEG;
	azi_EOI /= DEG;
	rad_EOI *= 1852.0;
	fpa_EOI /= DEG;
	vel_EOI /= 0.3048;

	VECTOR3 R, V;
	double GMT_EOI = T_L + dt_EOI;
	if (EMMXTR(GMT_EOI, rad_EOI, vel_EOI, lng_EOI, lat_EOI, fpa_EOI, azi_EOI, R, V))
	{
		PMXSPT("PMMIEV", 121);
		return;
	}
	//TBD: Print insertion vector
	GZLTRA.GMT_T = GMT_EOI;
	GZLTRA.R_T = R;
	GZLTRA.V_T = V;
	//TBD: ETMSCTRL for trajectory update
}

void RTCC::EMGENGEN(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, const StationTable &stationlist, int body, OrbitStationContactsTable &res)
{
	std::vector<StationContact> acquisitions;
	StationContact current;
	StationContact empty;

	if (stationlist.table.size() == 0) return;

	for (unsigned i = 0;i < stationlist.table.size();i++)
	{
		EMXING(ephemeris, MANTIMES, stationlist.table[i], body, acquisitions);
	}

	//Sort
	std::sort(acquisitions.begin(), acquisitions.end());

	//Put first 45 in table
	for (unsigned i = 0;i < 45;i++)
	{
		if (i < acquisitions.size())
		{
			res.Stations[i] = acquisitions[i];
		}
		else
		{
			res.Stations[i] = empty;
		}
	}
}

bool RTCC::EMXING(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, const Station & station, int body, std::vector<StationContact> &acquisitions)
{
	if (ephemeris.table.size() == 0) return false;

	ELVCTRInputTable interin;
	ELVCTROutputTable interout;
	EphemerisData svtemp, sv_AOS;
	StationContact current;
	VECTOR3 R_S_equ, R, rho, N, Ntemp, rhotemp, V;
	double GMT, sinang, GMT_AOS, LastGMT, LastSinang, GMT0, f, last_f, GMT_EMAX, sinangtemp, EMAX, GMT_LOS;
	unsigned iter = 0;
	int n, nmax;
	bool BestAvailableAOS, BestAvailableLOS, BestAvailableEMAX;

	GMT0 = ephemeris.table[0].GMT;
	R_S_equ = OrbMech::r_from_latlong(station.lat, station.lng, OrbMech::R_Earth + station.alt);

EMXING_LOOP:

	BestAvailableAOS = false, BestAvailableLOS = false, BestAvailableEMAX = false;
	n = 0;
	nmax = 10;
	f = last_f = 0.0;

	//Find AOS
	while (ephemeris.table.size() > iter)
	{
		R = ephemeris.table[iter].R;
		V = ephemeris.table[iter].V;
		GMT = ephemeris.table[iter].GMT;

		//For now
		if (ephemeris.table[iter].RBI != body)
		{
			return false;
		}

		OrbMech::EMXINGElev(R, R_S_equ, GMTBASE, GMT, body, N, rho, sinang);
		f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, GMTBASE, GMT, body);

		//Elevation angle above 0, there is an AOS
		if (sinang >= 0) break;
		//Slope of the elevation curve has changed sign, there might be an AOS
		if (iter > 0 && f*last_f < 0)
		{
			GMT_EMAX = OrbMech::LinearInterpolation(f, GMT, last_f, LastGMT, 0.0);
			interin.GMT = GMT_EMAX;
			ELVCTR(interin, interout, ephemeris, MANTIMES);
			if (interout.ErrorCode)
			{
				return false;
			}
			svtemp = interout.SV;

			OrbMech::EMXINGElev(svtemp.R, R_S_equ, GMTBASE, svtemp.GMT, body, Ntemp, rhotemp, sinangtemp);
			//Elevation angle above 0, there is an AOS
			if (sinangtemp >= 0) break;
		}

		LastGMT = GMT;
		LastSinang = sinang;
		last_f = f;
		iter++;
	}

	if (iter == ephemeris.table.size())
	{
		//Out of ephemeris
		return false;
	}

	//Already in AOS
	if (iter == 0)
	{
		GMT_AOS = GMT;
		svtemp = ephemeris.table.front();
		BestAvailableAOS = true;
	}
	else
	{
		GMT_AOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
		interin.GMT = GMT_AOS;
		ELVCTR(interin, interout, ephemeris, MANTIMES);
		if (interout.ErrorCode)
		{
			return false;
		}
		svtemp = interout.SV;

		n = 0;

		while (abs(LastGMT - GMT_AOS) >= 3.0 && nmax > n)
		{
			R = svtemp.R;
			V = svtemp.V;
			GMT = svtemp.GMT;

			OrbMech::EMXINGElev(R, R_S_equ, GMTBASE, GMT, body, N, rho, sinang);

			GMT_AOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
			interin.GMT = GMT_AOS;
			ELVCTR(interin, interout, ephemeris, MANTIMES);
			if (interout.ErrorCode)
			{
				return false;
			}
			svtemp = interout.SV;

			LastGMT = GMT;
			LastSinang = sinang;
			n++;
		}
	}

	sv_AOS = svtemp;

	//Find maximum elevation angle
	if (iter > 0) iter--;

	while (ephemeris.table.size() > iter)
	{
		R = ephemeris.table[iter].R;
		V = ephemeris.table[iter].V;
		GMT = ephemeris.table[iter].GMT;

		f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, GMTBASE, GMT, body);

		//EMAX before first SV in ephemeris
		if (iter == 0 && f < 0)
		{
			svtemp = ephemeris.table.front();
			GMT_EMAX = ephemeris.table.front().GMT;
			BestAvailableEMAX = true;
			break;
		}
		//Sign is changing, EMAX was passed
		if (f*last_f < 0)
		{
			GMT_EMAX = OrbMech::LinearInterpolation(f, GMT, last_f, LastGMT, 0.0);
			interin.GMT = GMT_EMAX;
			ELVCTR(interin, interout, ephemeris, MANTIMES);
			if (interout.ErrorCode)
			{
				return false;
			}
			svtemp = interout.SV;

			LastGMT = GMT;
			last_f = f;
			break;
		}

		LastGMT = GMT;
		last_f = f;
		iter++;
	}

	if (BestAvailableEMAX == false)
	{
		//Out of SVs in the ephemeris, EMAX must be after end of ephemeris
		if (iter == ephemeris.table.size())
		{
			svtemp = ephemeris.table.back();
			GMT_EMAX = ephemeris.table.back().GMT;
			BestAvailableEMAX = true;
		}
		else
		{
			n = 0;

			while (abs(LastGMT - GMT_EMAX) >= 3.0 && nmax > n)
			{
				R = svtemp.R;
				V = svtemp.V;
				GMT = svtemp.GMT;

				f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, GMTBASE, GMT, body);

				GMT_EMAX = OrbMech::LinearInterpolation(f, GMT, last_f, LastGMT, 0.0);
				interin.GMT = GMT_EMAX;
				ELVCTR(interin, interout, ephemeris, MANTIMES);
				if (interout.ErrorCode)
				{
					return false;
				}
				svtemp = interout.SV;

				LastGMT = GMT;
				last_f = f;
				n++;
			}
		}
	}

	OrbMech::EMXINGElev(svtemp.R, R_S_equ, GMTBASE, svtemp.GMT, body, N, rho, sinang);
	EMAX = asin(sinang);

	//Find LOS
	if (iter > 0) iter--;

	while (ephemeris.table.size() > iter)
	{
		R = ephemeris.table[iter].R;
		V = ephemeris.table[iter].V;
		GMT = ephemeris.table[iter].GMT;

		OrbMech::EMXINGElev(R, R_S_equ, GMTBASE, GMT, body, N, rho, sinang);
		f = OrbMech::EMXINGElevSlope(R, V, R_S_equ, GMTBASE, GMT, body);

		//Elevation angle below 0, there is an LOS
		if (sinang < 0 && f < 0) break;

		LastGMT = GMT;
		LastSinang = sinang;
		iter++;
	}

	if (iter == ephemeris.table.size())
	{
		GMT_LOS = ephemeris.table.back().GMT;
		BestAvailableLOS = true;
	}
	else
	{
		GMT_LOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
		interin.GMT = GMT_LOS;
		ELVCTR(interin, interout, ephemeris, MANTIMES);
		if (interout.ErrorCode)
		{
			return false;
		}
		svtemp = interout.SV;

		n = 0;

		while (abs(LastGMT - GMT_LOS) >= 3.0 && nmax > n)
		{
			R = svtemp.R;
			V = svtemp.V;
			GMT = svtemp.GMT;

			OrbMech::EMXINGElev(R, R_S_equ, GMTBASE, GMT, body, N, rho, sinang);

			GMT_LOS = OrbMech::LinearInterpolation(sinang, GMT, LastSinang, LastGMT, 0.0);
			interin.GMT = GMT_LOS;
			ELVCTR(interin, interout, ephemeris, MANTIMES);
			if (interout.ErrorCode)
			{
				return false;
			}
			svtemp = interout.SV;

			LastGMT = GMT;
			LastSinang = sinang;
			n++;
		}
	}

	//If in lunar SOI check if Moon occults AOS
	if (sv_AOS.RBI == BODY_MOON)
	{
		//TBD: Implement this
	}

	current.GMTAOS = GMT_AOS;
	current.GMTLOS = GMT_LOS;
	current.GMTEMAX = GMT_EMAX;
	current.MAXELEV = EMAX;
	current.BestAvailableAOS = BestAvailableAOS;
	current.BestAvailableLOS = BestAvailableLOS;
	current.BestAvailableEMAX = BestAvailableEMAX;
	current.StationID = station.code;

	acquisitions.push_back(current);

	if (iter < ephemeris.table.size() && acquisitions.size() <= 45)
	{
		goto EMXING_LOOP;
	}

	return true;
}

int RTCC::ELVARY(EphemerisDataTable &EPH, unsigned ORER, double GMT, bool EXTRAP, EphemerisData &sv_out, unsigned &ORER_out)
{
	EphemerisData RES;
	VECTOR3 TERM1, TERM2;
	double TERM3;
	unsigned DESLEF, DESRI;
	unsigned i = EPH.Header.Offset;
	int ERR = 0;
	//Ephemeris too small
	if (EPH.Header.NumVec < 2)
	{
		return 128;
	}
	//Requested order too high
	if (ORER > 8)
	{
		return 64;
	}
	if (EPH.Header.NumVec > ORER)
	{
		//Store Order(?)
	}
	else
	{
		ERR += 2;
		ORER = EPH.Header.NumVec - 1;
	}

	if (GMT < EPH.Header.TL)
	{
		if (EXTRAP == false) return 32;
		if (GMT < EPH.Header.TL - 4.0) { return 8; }
		else { ERR += 1; }
	}
	if (GMT > EPH.Header.TR)
	{
		if (EXTRAP == false) return 16;
		if (GMT > EPH.Header.TR + 4.0) { return 4; }
		else { ERR += 1; }
	}

	while (GMT > EPH.table[i].GMT)
	{
		i++;
	} 

	//Direct hit
	if (GMT == EPH.table[i].GMT)
	{
		sv_out = EPH.table[i];
		return ERR;
	}

	if (ORER % 2)
	{
		DESLEF = DESRI = (ORER + 1) / 2;
	}
	else
	{
		DESLEF = ORER / 2 + 1;
		DESRI = ORER / 2;
	}

	if (i < DESLEF + EPH.Header.Offset)
	{
		i = EPH.Header.Offset;
	}
	else if (i > EPH.Header.Offset + EPH.Header.NumVec - DESRI)
	{
		i = EPH.Header.Offset + EPH.Header.NumVec - ORER - 1;
	}
	else
	{
		i = i - DESLEF;
	}

	//Reference body inconsistency
	if (EPH.table[i].RBI != EPH.table[i + ORER].RBI)
	{
		unsigned l;
		unsigned RBI_counter = 0;
		for (l = 0;l < ORER + 1;l++)
		{
			if (EPH.table[i + l].RBI == EPH.table[i].RBI)
			{
				RBI_counter++;
			}
		}
		if (RBI_counter > ORER + 1 - RBI_counter)
		{
			//Most SVs have the same reference as i
			RES.RBI = EPH.table[i].RBI;
			l = ORER;
			while (EPH.table[i + l].RBI != EPH.table[i].RBI)
			{
				ORER--;
				l--;
			}
		}
		else
		{
			//Most SVs have the same reference as i+ORER
			RES.RBI = EPH.table[i + ORER].RBI;
			l = 0;
			while (EPH.table[i + l].RBI != EPH.table[i + ORER].RBI)
			{
				ORER--;
				i++;
				l++;
			}
		}
	}
	else
	{
		RES.RBI = EPH.table[i].RBI;
	}

	for (unsigned j = 0; j < ORER + 1; j++)
	{
		TERM1 = EPH.table[i + j].R;
		TERM2 = EPH.table[i + j].V;
		TERM3 = 1.0;
		for (unsigned k = 0;k < ORER + 1;k++)
		{
			if (k != j)
			{
				TERM3 *= (GMT - EPH.table[i + k].GMT) / (EPH.table[i + j].GMT - EPH.table[i + k].GMT);
			}
		}
		RES.R += TERM1 * TERM3;
		RES.V += TERM2 * TERM3;
	}

	RES.GMT = GMT;

	sv_out = RES;
	ORER_out = ORER;

	return ERR;
}

void RTCC::EMDSTAC()
{
	double GMT = RTCCPresentTimeGMT();
	double GET = GETfromGMT(GMT);

	//Clear table
	for (unsigned l = 0;l < 2;l++)
	{
		for (unsigned k = 0;k < 6;k++)
		{
			NextStationContactsBuffer.STA[l][k] = "";
			NextStationContactsBuffer.GETHCA[l][k] = false;
			NextStationContactsBuffer.DTKLOS[l][k] = false;
			NextStationContactsBuffer.EMAX[l][k] = false;
			NextStationContactsBuffer.DTPASS[l][k] = 0.0;
			NextStationContactsBuffer.DTKH[l][k] = 0.0;
		}
	}

	OrbitStationContactsTable *tab;
	unsigned j;
	//Search through all station contacts
	for (unsigned l = 0;l < 2;l++)
	{
		if (l == 0)
		{
			tab = &EZSTACT1;
		}
		else
		{
			tab = &EZSTACT3;
		}

		j = 0;
		for (unsigned i = 0;i < 45;i++)
		{
			//If we got 6 contacts we are done
			if (j >= 6) break;
			//Skip over contacts from the past
			if (GMT > tab->Stations[i].GMTLOS)
			{
				continue;
			}
			//Found one
			NextStationContactsBuffer.STA[l][j] = tab->Stations[i].StationID;
			NextStationContactsBuffer.GETHCA[l][j] = GETfromGMT(tab->Stations[i].GMTAOS);
			NextStationContactsBuffer.DTKLOS[l][j] = 0.0;
			NextStationContactsBuffer.EMAX[l][j] = tab->Stations[i].MAXELEV*DEG;
			NextStationContactsBuffer.DTPASS[l][j] = tab->Stations[i].GMTLOS - tab->Stations[i].GMTAOS;
			NextStationContactsBuffer.DTKH[l][j] = 0.0;
			NextStationContactsBuffer.BestAvailableAOS[l][j] = tab->Stations[i].BestAvailableAOS;
			NextStationContactsBuffer.BestAvailableEMAX[l][j] = tab->Stations[i].BestAvailableEMAX;

			//If we are in AOS, modify some numbers accordingly
			if (GMT > tab->Stations[i].GMTAOS)
			{
				NextStationContactsBuffer.GETHCA[l][j] = GET;
				NextStationContactsBuffer.DTPASS[l][j] = tab->Stations[i].GMTLOS - GMT;
				NextStationContactsBuffer.BestAvailableAOS[l][j] = true;
			}
			if (GMT > tab->Stations[i].GMTEMAX)
			{
				NextStationContactsBuffer.BestAvailableEMAX[l][j] = true;
			}
			j++;
		}
	}
	//Save current GET for the 12 second update cycle
	NextStationContactsBuffer.GET = GET;
}

void RTCC::EMDLANDM(int L, double get, double dt, int ref)
{
	double GMT_Begin, GMT_End;
	unsigned i = 0, j = 0;
	int stat_body;

	GMT_Begin = GMTfromGET(get);
	GMT_End = GMTfromGET(get + dt);

	Station station;
	StationTable contact;

	stat_body = EZLASITE.REF;
	if (stat_body < 0)
	{
		return;
	}
	for (int i = 0;i < 12;i++)
	{
		if (EZLASITE.Data[i].code == "") continue;
		station.alt = EZLASITE.Data[i].alt;
		station.code = EZLASITE.Data[i].code;
		station.lat = EZLASITE.Data[i].lat;
		station.lng = EZLASITE.Data[i].lng;
		contact.table.push_back(station);
	}

	int TUP;
	unsigned NumVec;
	OrbitEphemerisTable ephem;

	ELNMVC(GMT_Begin, GMT_End, RTCC_MPT_CSM, NumVec, TUP);
	ELFECH(GMT_Begin, NumVec, 1, RTCC_MPT_CSM, ephem.EPHEM, ephem.MANTIMES, ephem.LUNRSTAY);

	OrbitStationContactsTable res;
	EMGENGEN(ephem.EPHEM, ephem.MANTIMES, contact, stat_body, res);
}

void RTCC::ECMEXP(EphemerisData sv, Station *stat, int statbody, double &range, double &alt)
{
	EphemerisData sv_out;
	VECTOR3 Q, U_R;
	double R_stat;
	int in, out;

	U_R = unit(_V(cos(stat->lng)*cos(stat->lat), sin(stat->lng)*cos(stat->lat), sin(stat->lat)));
	if (sv.RBI == BODY_EARTH)
	{
		in = 0;
	}
	else
	{
		in = 2;
	}
	if (statbody == BODY_EARTH)
	{
		out = 1;
	}
	else
	{
		out = 3;
	}
	ELVCNV(sv, in, out, sv_out);
	if (statbody == BODY_EARTH)
	{
		alt = length(sv_out.R) - OrbMech::R_Earth;
		R_stat = stat->alt + OrbMech::R_Earth;
	}
	else
	{
		alt = length(sv_out.R) - MCSMLR;
		R_stat = stat->alt + MCSMLR;
	}
	
	Q = unit(crossp(sv_out.V, sv_out.R));
	range = abs(R_stat * asin(dotp(U_R, Q)));
}

int RTCC::GLSSAT(EphemerisData sv, double &lat, double &lng, double &alt)
{
	EphemerisData sv_out;
	VECTOR3 u;
	int in, out;
	if (sv.RBI == BODY_EARTH)
	{
		in = 0;
		out = 1;
	}
	else
	{
		in = 2;
		out = 3;
	}

	if (ELVCNV(sv, in, out, sv_out))
	{
		return 1;
	}
	u = unit(sv.R);
	lat = atan2(u.z, sqrt(u.x*u.x + u.y*u.y));
	lng = atan2(u.y, u.x);
	if (sv.RBI == BODY_EARTH)
	{
		alt = length(sv.R) - OrbMech::R_Earth;
	}
	else
	{
		alt = length(sv.R) - MCSMLR;
	}
	return 0;
}

void RTCC::EMDPESAD(int num, int veh, int ind, double vala, double valb, int body)
{
	StationContact empty;
	double GMT_Begin, GMT_End;
	unsigned i = 0, j = 0;
	int stat_body;

	if (ind == 1)
	{
		GMT_Begin = GMTfromGET(vala);
		GMT_End = GMTfromGET(valb);
	}
	else
	{
		GMT_Begin = CapeCrossingGMT(veh, (int)vala);
		GMT_End = CapeCrossingGMT(veh, 1 + (int)valb);
	}

	if (GMT_End < 0)
	{
		return;
	}
	if (GMT_Begin < RTCCPresentTimeGMT())
	{
		GMT_Begin = RTCCPresentTimeGMT();
	}

	Station station;
	StationTable contact;

	//Experimental site
	if (num == 3)
	{
		//TBD
		stat_body = EZEXSITE.REF;
		if (stat_body < 0)
		{
			return;
		}
		for (int i = 0;i < 12;i++)
		{
			if (EZEXSITE.Data[i].code == "") continue;
			station.alt = EZEXSITE.Data[i].alt;
			station.code = EZEXSITE.Data[i].code;
			station.lat = EZEXSITE.Data[i].lat;
			station.lng = EZEXSITE.Data[i].lng;
			contact.table.push_back(station);
		}
	}
	//Predicted site
	else
	{
		stat_body = BODY_EARTH;
		for (int i = 0;i < NUMBEROFGROUNDSTATIONS;i++)
		{
			if (MGRTAG == 0 && !groundstationslunar[i]) continue;
			station.alt = 0.0;
			station.code = gsabbreviations[i];
			station.lat = groundstations[i][0];
			station.lng = groundstations[i][1];
			contact.table.push_back(station);
		}
	}

	int TUP;
	unsigned NumVec;
	OrbitEphemerisTable ephem;

	ELNMVC(GMT_Begin, GMT_End, veh, NumVec, TUP);
	ELFECH(GMT_Begin, NumVec, 1, veh, ephem.EPHEM, ephem.MANTIMES, ephem.LUNRSTAY);

	OrbitStationContactsTable res;
	EMGENGEN(ephem.EPHEM, ephem.MANTIMES, contact, stat_body, res);

	//Experimental site
	if (num == 3)
	{
		ExperimentalSiteAcquisitionTable *tab = &EZDPSAD2;

		ExperimentalSiteAcquisitionTable empty;
		*tab = empty;

		ELVCTRInputTable intab;
		ELVCTROutputTable outtab;
		Station tempstat;
		double range, alt;

		intab.L = 1;

		int numcontacts = 0;
		for (int i = 0;i < 42;i++)
		{
			if (res.Stations[i].GMTAOS < GMT_Begin || res.Stations[i].GMTAOS > GMT_End)
			{
				continue;
			}

			intab.GMT = res.Stations[i].GMTEMAX;
			ELVCTR(intab, outtab);

			for (unsigned j = 0;j < contact.table.size();j++)
			{
				if (contact.table[j].code == res.Stations[i].StationID)
				{
					tempstat = contact.table[j];
					break;
				}
			}
			ECMEXP(outtab.SV, &tempstat, stat_body, range, alt);

			tab->REV[tab->pages - 1][numcontacts] = CapeCrossingRev(veh, res.Stations[i].GMTAOS);
			tab->STA[tab->pages - 1][numcontacts] = res.Stations[i].StationID;
			tab->GETAOS[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTAOS);
			tab->GNDRNG[tab->pages - 1][numcontacts] = range / 1852.0;
			tab->ALT[tab->pages - 1][numcontacts] = alt / 1852.0;
			tab->ELMAX[tab->pages - 1][numcontacts] = res.Stations[i].MAXELEV*DEG;
			tab->GETCA[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTEMAX);
			tab->GETLOS[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTLOS);
			tab->BestAvailableAOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableAOS;
			tab->BestAvailableLOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableLOS;
			tab->BestAvailableEMAX[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableEMAX;
			numcontacts++;
			if (numcontacts >= 20)
			{
				tab->pages = 2;
				numcontacts = 0;
			}
		}
		if (tab->pages == 1)
		{
			tab->numcontacts[0] = numcontacts;
			tab->numcontacts[1] = 0;
		}
		else if (tab->pages == 2)
		{
			tab->numcontacts[0] = 20;
			tab->numcontacts[1] = numcontacts;
		}
	}
	//Predicted site
	else
	{
		PredictedSiteAcquisitionTable *tab;

		if (veh == 1)
		{
			if (num == 1)
			{
				tab = &EZACQ1;
			}
			else
			{
				tab = &EZDPSAD1;
			}
		}
		else
		{
			if (num == 1)
			{
				tab = &EZACQ3;
			}
			else
			{
				tab = &EZDPSAD3;
			}
		}

		PredictedSiteAcquisitionTable empty;
		*tab = empty;

		if (veh == 1)
		{
			tab->VEHICLE = "CSM";
		}
		else
		{
			tab->VEHICLE = "LEM";
		}

		int numcontacts = 0;
		for (int i = 0;i < 42;i++)
		{
			if (res.Stations[i].GMTAOS < GMT_Begin || res.Stations[i].GMTAOS > GMT_End)
			{
				continue;
			}
			tab->STA[tab->pages - 1][numcontacts] = res.Stations[i].StationID;
			tab->GETHCA[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTAOS);
			tab->GETHCD[tab->pages - 1][numcontacts] = GETfromGMT(res.Stations[i].GMTLOS);
			tab->ELMAX[tab->pages - 1][numcontacts] = res.Stations[i].MAXELEV*DEG;
			tab->REV[tab->pages - 1][numcontacts] = CapeCrossingRev(veh, res.Stations[i].GMTAOS);
			tab->BestAvailableAOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableAOS;
			tab->BestAvailableLOS[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableLOS;
			tab->BestAvailableEMAX[tab->pages - 1][numcontacts] = res.Stations[i].BestAvailableEMAX;
			numcontacts++;
			if (numcontacts >= 21)
			{
				tab->pages = 2;
				numcontacts = 0;
			}
		}
		if (tab->pages == 1)
		{
			tab->numcontacts[0] = numcontacts;
			tab->numcontacts[1] = 0;
		}
		else if (tab->pages == 2)
		{
			tab->numcontacts[0] = 21;
			tab->numcontacts[1] = numcontacts;
		}
	}
}

void RTCC::PMMAEGS(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out)
{
	if (header.AEGInd == BODY_EARTH)
	{
		pmmaeg.CALL(header, in, out);
	}
	else
	{
		pmmlaeg.CALL(header, in, out);
	}
}

void RTCC::PMMTLC(AEGHeader HEADER, AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double DESLAM, int &K, int INDVEC)
{
	MATRIX3 Rot;
	double i_CB, g_CB, h_CB, u_CB, g_dot, h_dot, Z, DELTA, lambda_V, lambda_apo, w_CB, dlambda, DELTADOT, dt, u_CB_dot, mu_CB;

	if (HEADER.AEGInd == BODY_EARTH)
	{
		w_CB = OrbMech::w_Earth;
		mu_CB = OrbMech::mu_Earth;
	}
	else
	{
		w_CB = OrbMech::w_Moon;
		mu_CB = OrbMech::mu_Moon;
	}
	lambda_apo = 0.0;
	K = 1;
	AEGOUT = AEGIN;
	do
	{
		if (HEADER.AEGInd == BODY_EARTH)
		{
			if (INDVEC < 0)
			{
				lambda_apo = 0.0;
			}
			else
			{
				if (K == 1)
				{
					lambda_apo = 0.0;//MCEGMB;
				}
			}
		}
		else
		{
			VECTOR3 P, W, P_apo, W_apo;
			double i_SG, g_SG, h_SG;

			Rot = OrbMech::GetRotationMatrix(BODY_MOON, AEGOUT.Item7 + AEGOUT.TS / 24.0 / 3600.0);
			OrbMech::PIVECT(AEGOUT.coe_osc.i, AEGOUT.coe_osc.g, AEGOUT.coe_osc.h, P, W);
			P_apo = rhtmul(Rot, P);
			W_apo = rhtmul(Rot, W);
			OrbMech::PIVECT(P_apo, W_apo, i_SG, g_SG, h_SG);
			i_CB = i_SG;
			g_CB = g_SG;
			h_CB = h_SG;
			u_CB = AEGOUT.f + g_CB;
			if (u_CB >= PI2)
			{
				u_CB -= PI2;
			}
			g_dot = AEGIN.g_dot;
			h_dot = AEGIN.h_dot;
			Z = 0.0;
		}
		DELTA = atan2(sin(u_CB)*cos(i_CB), cos(u_CB));
		if (DELTA < 0)
		{
			DELTA += PI2;
		}
		lambda_V = h_CB + DELTA - Z * (lambda_apo + w_CB * AEGOUT.TE);
		lambda_V = fmod(lambda_V, PI2);
		if (lambda_V < 0)
		{
			lambda_V += PI2;
		}
		dlambda = DESLAM - lambda_V;
		if (K <= 1)
		{
			DELTADOT = AEGIN.l_dot + g_dot;
			if (i_CB > PI05)
			{
				DELTADOT = -DELTADOT;
				if (dlambda > 0)
				{
					dlambda = dlambda - PI2;
				}
			}
			else
			{
				if (dlambda < 0)
				{
					dlambda = dlambda + PI2;
				}
			}
		}
		else
		{
			u_CB_dot = sqrt(mu_CB*AEGOUT.coe_osc.a*(1.0 - pow(AEGOUT.coe_osc.e, 2))) / pow(AEGOUT.R, 2) + g_dot;
			DELTADOT = cos(AEGOUT.coe_osc.i)*u_CB_dot / (pow(cos(u_CB), 2) + pow(sin(u_CB), 2)*pow(cos(AEGOUT.coe_osc.i), 2));
			if (abs(dlambda) > PI)
			{
				if (dlambda > 0)
				{
					dlambda = dlambda - PI2;
				}
				else
				{
					dlambda = dlambda + PI2;
				}
			}
		}
		dt = dlambda / (h_dot + DELTADOT - w_CB);
		if (abs(dt) <= 0.01)
		{
			K = 0;
			return;
		}
		if (K > 5)
		{
			return;
		}
		K++;
		AEGIN.TE = AEGOUT.TE + dt;
		AEGIN.TIMA = 0;
		PMMAEGS(HEADER, AEGIN, AEGOUT);
		if (HEADER.ErrorInd)
		{
			K = -1;
			return;
		}
	} while (K < 6);
}

void RTCC::PMMDAN(AEGBlock aeg, int IND, int &ERR, double &T_c, double &T_c_apo)
{
	AEGDataBlock init, out;
	VECTOR3 R_EM, V_EM, R_ES, R_S, R, V, H, N, N_apo;
	double MJD, r_S, mu, cos_theta, R_e, r, phi1, phi2, phi3, n, cos_phi1, sin_alpha, h, cos_eta, sin_eta, F, dt, S_T;
	int J, I_c;
	bool daylight;

	J = 0;
	MJD = GMTBASE + aeg.Data.TS / 24.0 / 3600.0;
	OrbMech::PLEFEM(pzefem, MJD, R_EM, V_EM, R_ES);

	if (aeg.Header.AEGInd == BODY_EARTH)
	{
		R_S = R_ES;
		mu = OrbMech::mu_Earth;
		R_e = OrbMech::R_Earth;
	}
	else
	{
		R_S = R_ES - R_EM;
		mu = OrbMech::mu_Moon;
		R_e = MCSMLR;
	}
	aeg.Data.TIMA = 0;
	aeg.Data.TE = aeg.Data.TS;
	PMMAEGS(aeg.Header, aeg.Data, init);
	if (aeg.Header.ErrorInd != 0)
	{
		goto RTCC_PMMDAN_4_3;
	}
	
	OrbMech::GIMKIC(aeg.Data.coe_osc, mu, R, V);
	r = length(R);
	r_S = length(R_ES);
	cos_theta = dotp(R, R_S) / r / r_S;
	if (cos_theta >= 0)
	{
		//Vehicle is in daylight
		daylight = true;
	}
	else if (r*sqrt(1.0 - cos_theta * cos_theta) >= R_e)
	{
		//Vehicle is in daylight
		daylight = true;
	}
	else
	{
		//Vehicle is in darkness
		daylight = false;
	}
	I_c = 0;
	T_c = init.TE;
	out = init;
RTCC_PMMDAN_2_2:
	if (I_c > 0)
	{
		OrbMech::GIMKIC(out.coe_osc, mu, R, V);
	}
	H = crossp(R, V);
	N = crossp(R_S, H);
	n = length(N);
	if (daylight)
	{
		n = -n;
	}
	N_apo = N / n;
	cos_phi1 = dotp(R, N_apo) / r;
	phi1 = acos(cos_phi1);
	sin_alpha = sqrt(1.0 - pow(R_e / r, 2));
	h = length(H);
	cos_eta = dotp(H, R_S) / h / r_S;
	sin_eta = sqrt(1.0 - cos_eta * cos_eta);
	if (sin_eta <= sin_alpha)
	{
		ERR = 1;
		goto RTCC_PMMDAN_4_3;
	}
	phi2 = asin(sin_alpha / sin_eta);
	F = R.x*N_apo.y - R.y*N_apo.x;
	if (aeg.Header.AEGInd == BODY_MOON)
	{
		F = -F;
	}
	if (daylight)
	{
		if (F >= 0)
		{
			phi3 = phi1 + phi2;
		}
		else
		{
			if (cos_phi1 > 0)
			{
				phi3 = phi2 - phi1;
			}
			else
			{
				phi3 = PI2 - phi1 + phi2;
			}
		}
	}
	else
	{
		if (F > 0)
		{
			phi3 = phi1 - phi2;
		}
		else
		{
			phi3 = -phi1 - phi2;
		}
	}
	dt = phi3 / (out.l_dot + out.g_dot);
	T_c = T_c + dt;
	if (abs(dt) > 0.00055*3600.0)
	{
		if (I_c >= 4)
		{
			ERR = 2;
			goto RTCC_PMMDAN_4_3;
		}
		I_c++;
	RTCC_PMMDAN_2_4:
		if (aeg.Header.AEGInd == BODY_EARTH)
		{
			init.TE = T_c;
			PMMAEGS(aeg.Header, init, out);
		}
		else
		{
			aeg.Data.TE = T_c;
			PMMAEGS(aeg.Header, aeg.Data, out);
		}
		if (aeg.Header.ErrorInd != 0)
		{
			goto RTCC_PMMDAN_4_3;
		}
		goto RTCC_PMMDAN_2_2;
	}
	if (daylight)
	{
		T_c = -T_c;
	}
	if (IND == 1)
	{
		T_c_apo = 0.0;
		return;
	}
	if (J <= 0)
	{
		S_T = T_c;
		daylight = !daylight;
		if (daylight == false)
		{
			phi2 = -phi2;
		}
		phi3 = PI + 2.0*phi2;
		dt = phi3 / (out.l_dot + out.g_dot);
		T_c = abs(T_c) + dt;
		J = 1;
		I_c = 1;
		goto RTCC_PMMDAN_2_4;
	}
	else
	{
		T_c_apo = T_c;
		T_c = S_T;
		return;
	}

RTCC_PMMDAN_4_3:
	T_c = 0.0;
	T_c_apo = 0.0;
	return;
}

VECTOR3 RTCC::PIAEDV(VECTOR3 DV, VECTOR3 R_CSM, VECTOR3 V_CSM, VECTOR3 R_LM, bool i)
{
	//INPUTS:
	// i = 0: inertial to LVLH, 1: LVLH to inertial 

	VECTOR3 H, Z_PA, P, X_PA, Y_PA, DV_out;

	H = crossp(R_CSM, V_CSM);
	Z_PA = -unit(R_LM);
	P = crossp(Z_PA, H);
	X_PA = unit(P);
	Y_PA = crossp(Z_PA, X_PA);
	if (i)
	{
		DV_out = X_PA * DV.x + Y_PA * DV.y + Z_PA * DV.z;
	}
	else
	{
		DV_out = _V(dotp(DV, X_PA), dotp(DV, Y_PA), dotp(DV, Z_PA));
	}

	return DV_out;
}

VECTOR3 RTCC::PIEXDV(VECTOR3 R_ig, VECTOR3 V_ig, double WT, double T, VECTOR3 DV, bool i)
{
	//INPUTS:
	// i = 0: inertial to LVLH, 1: LVLH to inertial 
	VECTOR3 H, Y_PH, Z_PH, X_PH, DV_out;
	double h, rr, r, dv, theta, V_F, V_D;

	H = crossp(R_ig, V_ig);
	h = length(H);
	rr = dotp(R_ig, R_ig);
	r = sqrt(rr);
	Y_PH = -H / h;
	Z_PH = -R_ig / r;
	X_PH = crossp(Y_PH, Z_PH);
	dv = length(DV);
	theta = h * dv*WT / (2.0*rr*T);
	if (i)
	{
		double V_S;
		V_F = DV.x*cos(theta) - DV.z*sin(theta);
		V_S = DV.y;
		V_D = DV.x*sin(theta) + DV.z*cos(theta);
		DV_out = X_PH * V_F + Y_PH * V_S + Z_PH * V_D;
	}
	else
	{
		V_F = dotp(DV, X_PH);
		DV_out.y = dotp(DV, Y_PH);
		V_D = dotp(DV, Z_PH);
		DV_out.x = V_F * cos(theta) + V_D * sin(theta);
		DV_out.z = -V_F * sin(theta) + V_D * cos(theta);
	}

	return DV_out;
}

int RTCC::ELVCNV(VECTOR3 vec, double GMT, int in, int out, VECTOR3 &vec_out)
{
	EphemerisData eph, sv_out;
	int err = 0;

	eph.R = vec;
	eph.V = _V(1, 0, 0);
	eph.GMT = GMT;

	err = ELVCNV(eph, in, out, sv_out);
	vec_out = sv_out.R;
	return err;
}

int RTCC::ELVCNV(EphemerisDataTable &svtab, int in, int out, EphemerisDataTable &svtab_out)
{
	EphemerisData sv, sv_out;
	svtab_out.table.clear();
	int err = 0;
	for (unsigned i = 0;i < svtab.table.size();i++)
	{
		sv = svtab.table[i];
		err = ELVCNV(sv, in, out, sv_out);
		if (err)
		{
			break;
		}
		svtab_out.table.push_back(sv_out);
	}
	//Store some ephemeris header data
	svtab_out.Header.CSI = out;
	svtab_out.Header.NumVec = svtab_out.table.size();
	svtab_out.Header.TL = svtab_out.table.front().GMT;
	svtab_out.Header.TR = svtab_out.table.back().GMT;
	svtab_out.Header.TUP = svtab.Header.TUP;
	svtab_out.Header.VEH = svtab.Header.VEH;
	return err;
}

int RTCC::ELVCNV(EphemerisData &sv, int in, int out, EphemerisData &sv_out)
{
	int err;

	sv_out = sv;

	//0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT, 4 = EMP

	//ECI to/from ECT
	if ((in == 0 && out == 1) || (in == 1 && out == 0))
	{
		MATRIX3 Rot = OrbMech::GetRotationMatrix(BODY_EARTH, OrbMech::MJDfromGET(sv.GMT, GMTBASE));

		if (in == 0)
		{
			sv_out.R = rhtmul(Rot, sv.R);
			sv_out.V = rhtmul(Rot, sv.V);
		}
		else
		{
			sv_out.R = rhmul(Rot, sv.R);
			sv_out.V = rhmul(Rot, sv.V);
		}
	}
	//ECI to/from MCI
	else if ((in == 0 && out == 2) || (in == 2 && out == 0))
	{
		VECTOR3 R_EM, V_EM, R_ES;

		if (!OrbMech::PLEFEM(pzefem, OrbMech::MJDfromGET(sv.GMT, GMTBASE), R_EM, V_EM, R_ES)) return 1;

		if (in == 0)
		{
			sv_out.R = sv.R - R_EM;
			sv_out.V = sv.V - V_EM;
		}
		else
		{
			sv_out.R = sv.R + R_EM;
			sv_out.V = sv.V + V_EM;
		}
	}
	//MCI to/from MCT
	else if ((in == 2 && out == 3) || (in == 3 && out == 2))
	{
		MATRIX3 Rot = OrbMech::GetRotationMatrix(BODY_MOON, OrbMech::MJDfromGET(sv.GMT, GMTBASE));

		if (in == 2)
		{
			sv_out.R = rhtmul(Rot, sv.R);
			sv_out.V = rhtmul(Rot, sv.V);
		}
		else
		{
			sv_out.R = rhmul(Rot, sv.R);
			sv_out.V = rhmul(Rot, sv.V);
		}
	}
	//MCI to/from EMP
	else if ((in == 2 && out == 4) || (in == 4 && out == 2))
	{
		MATRIX3 Rot;
		VECTOR3 R_EM, V_EM, R_ES;
		VECTOR3 X_EMP, Y_EMP, Z_EMP;

		if (!OrbMech::PLEFEM(pzefem, OrbMech::MJDfromGET(sv.GMT, GMTBASE), R_EM, V_EM, R_ES)) return 1;

		X_EMP = -unit(R_EM);
		Z_EMP = unit(crossp(R_EM, V_EM));
		Y_EMP = crossp(Z_EMP, X_EMP);
		Rot = _M(X_EMP.x, X_EMP.y, X_EMP.z, Y_EMP.x, Y_EMP.y, Y_EMP.z, Z_EMP.x, Z_EMP.y, Z_EMP.z);

		if (in == 2)
		{
			sv_out.R = rhmul(Rot, sv.R);
			sv_out.V = rhmul(Rot, sv.V);
		}
		else
		{
			sv_out.R = rhtmul(Rot, sv.R);
			sv_out.V = rhtmul(Rot, sv.V);
		}
	}
	//ECI to MCT
	else if (in == 0 && out == 3)
	{
		EphemerisData sv1;

		err = ELVCNV(sv, 0, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 3, sv_out);
		if (err) return err;
	}
	//MCT to ECI
	else if (in == 3 && out == 0)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 3, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 0, sv_out);
		if (err) return err;
	}
	//ECI to EMP
	else if (in == 0 && out == 4)
	{
		EphemerisData sv1;

		err = ELVCNV(sv, 0, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 4, sv_out);
		if (err) return err;
	}
	//EMP to ECI
	else if (in == 4 && out == 0)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 4, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 0, sv_out);
		if (err) return err;
	}
	//ECT to EMP
	else if (in == 1 && out == 4)
	{
		EphemerisData sv1, sv2;

		err = ELVCNV(sv, 1, 0, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 0, 2, sv2);
		if (err) return err;
		err = ELVCNV(sv2, 2, 4, sv_out);
		if (err) return err;
	}
	//EMP to ECT
	else if (in == 4 && out == 1)
	{
		EphemerisData sv1, sv2;

		err = ELVCNV(sv, 4, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 0, sv2);
		if (err) return err;
		err = ELVCNV(sv2, 0, 1, sv_out);
		if (err) return err;
	}
	//MCT to EMP
	else if (in == 3 && out == 4)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 3, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 4, sv_out);
		if (err) return err;
	}
	//EMP to MCT
	else if (in == 4 && out == 3)
	{
		EphemerisData sv1;
		err = ELVCNV(sv, 4, 2, sv1);
		if (err) return err;
		err = ELVCNV(sv1, 2, 3, sv_out);
		if (err) return err;
	}

	return 0;
}

void RTCC::EMDCHECK(int veh, int opt, double param, double THTime, int ref, bool feet)
{
	EphemerisData sv_out, sv_conv, sv_inert;
	MissionPlanTable *table;
	OrbitEphemerisTable *ephem;

	EZCHECKDIS.ErrorMessage = "";

	if (veh == RTCC_MPT_LM)
	{
		table = &PZMPTLEM;
		ephem = &EZEPH2;
	}
	else
	{
		table = &PZMPTCSM;
		ephem = &EZEPH1;
	}

	if (table->CommonBlock.TUP == 0)
	{
		EZCHECKDIS.ErrorMessage = "Error 8";
		return;
	}
	else if (table->CommonBlock.TUP < 0)
	{
		EZCHECKDIS.ErrorMessage = "Error 2";
		return;
	}

	if (table->CommonBlock.TUP != ephem->EPHEM.Header.TUP)
	{
		EZCHECKDIS.ErrorMessage = "Error 3";
		return;
	}

	EZCHECKDIS.U_T = _V(-2, 0, 0);
	EZCHECKDIS.NV = 0;
	if (THTime > 0)
	{
		EZCHECKDIS.THT = THTime;
	}
	else
	{
		EZCHECKDIS.THT = 0.0;
	}

	//GMT and GET
	if (opt == 1 || opt == 2)
	{
		double GMT;

		if (opt == 1)
		{
			GMT = param;
			sprintf(EZCHECKDIS.Option, "GMT");
		}
		else
		{
			GMT = GMTfromGET(param);
			sprintf(EZCHECKDIS.Option, "GET");
		}

		if (THTime > 0)
		{
			double THGMT;
			if (opt == 1)
			{
				THGMT = THTime;
			}
			else
			{
				THGMT = GMTfromGET(THTime);
			}
			EphemerisData svtemp;
			if (ELFECH(THGMT, veh, svtemp))
			{
				EZCHECKDIS.ErrorMessage = "Error 4";
				return;
			}
			EMSMISSInputTable emsin;

			emsin.MaxIntegTime = GMT - svtemp.GMT;
			sv_out = EMMENI(emsin, svtemp, emsin.MaxIntegTime);
		}
		else
		{
			ELVCTRInputTable interin;
			ELVCTROutputTable interout;
			interin.GMT = GMT;
			interin.L = veh;
			ELVCTR(interin, interout);
			if (interout.ErrorCode > 2)
			{
				//Error: Input time is outside of ephemeris range
				EZCHECKDIS.ErrorMessage = "Error 4";
				return;
			}
			sv_out = interout.SV;

			EZCHECKDIS.NV = interout.ORER + 1;
		}
	}
	//Maneuver Initiation
	else if (opt == 3)
	{
		unsigned man = (unsigned)param;
		if (man <= 0 || table->mantable.size() < man)
		{
			EZCHECKDIS.ErrorMessage = "Error 7";
			return;
		}
		sv_out.R = table->mantable[man - 1].R_BI;
		sv_out.V = table->mantable[man - 1].V_BI;
		sv_out.GMT = table->mantable[man - 1].GMT_BI;
		sv_out.RBI = table->mantable[man - 1].RefBodyInd;
		sprintf(EZCHECKDIS.Option, "MVI");

		EZCHECKDIS.U_T = table->mantable[man - 1].A_T;
	}
	//Maneuver cutoff
	else if (opt == 4)
	{
		unsigned man = (unsigned)param;

		if (man <= 0 || table->mantable.size() < man)
		{
			EZCHECKDIS.ErrorMessage = "Error 7";
			return;
		}
		sv_out.R = table->mantable[man - 1].R_BO;
		sv_out.V = table->mantable[man - 1].V_BO;
		sv_out.GMT = table->mantable[man - 1].GMT_BO;
		sv_out.RBI = table->mantable[man - 1].RefBodyInd;
		sprintf(EZCHECKDIS.Option, "MVE");
	}
	else
	{
		EphemerisData svtemp;
		if (ELFECH(GMTfromGET(THTime), veh, svtemp))
		{
			EZCHECKDIS.ErrorMessage = "Error 4";
			return;
		}

		EMSMISSInputTable emsin;

		if (opt == 5)
		{
			emsin.CutoffIndicator = 1;
		}
		else if (opt == 6)
		{
			emsin.CutoffIndicator = 3;
		}
		else
		{
			emsin.CutoffIndicator = 4;
		}
		if (ref <= 1)
		{
			emsin.StopParamRefFrame = 0;
			emsin.EarthRelStopParam = param;
		}
		else
		{
			emsin.StopParamRefFrame = 1;
			emsin.MoonRelStopParam = param;
		}

		sv_out = EMMENI(emsin, svtemp, 10.0*24.0*3600.0);

		if (emsin.CutoffIndicator != emsin.NIAuxOutputTable.TerminationCode)
		{
			EZCHECKDIS.ErrorMessage = "Error 10";
			return;
		}
	}

	if (veh == RTCC_MPT_LM)
	{
		sprintf_s(EZCHECKDIS.VEH, "LM");
	}
	else
	{
		sprintf_s(EZCHECKDIS.VEH, "CSM");
	}
	EZCHECKDIS.GET = GETfromGMT(sv_out.GMT);
	EZCHECKDIS.GMT = sv_out.GMT;

	sv_inert = sv_out;

	int inref;
	//Convert vector to desired coordinate system
	if (sv_out.RBI == BODY_EARTH)
	{
		inref = 0;
	}
	else
	{
		inref = 2;
	}

	ELVCNV(sv_inert, inref, ref, sv_conv);

	if (ref == 0)
	{
		sprintf(EZCHECKDIS.RF, "ECI");
	}
	else if (ref == 1)
	{
		sprintf(EZCHECKDIS.RF, "ECT");
	}
	else if (ref == 2)
	{
		sprintf(EZCHECKDIS.RF, "MCI");
	}
	else
	{
		sprintf(EZCHECKDIS.RF, "MCT");
	}

	//Display unit
	double CONVFACR, CONVFACV, R_E, mu;

	EZCHECKDIS.unit = feet;
	if (feet == 1)
	{
		CONVFACR = CONVFACV = 0.3048;
	}
	else
	{
		CONVFACR = 6.371e6;
		CONVFACV = 6.371e6 / 3600.0;
	}

	if (ref < 2)
	{
		R_E = OrbMech::R_Earth;
		mu = OrbMech::mu_Earth;
	}
	else
	{
		R_E = MCSMLR;
		mu = OrbMech::mu_Moon;
	}

	EZCHECKDIS.Pos = sv_conv.R / CONVFACR;
	EZCHECKDIS.Vel = sv_conv.V / CONVFACV;

	double rmag, vmag, rtasc, decl, fpav, az;
	OrbMech::rv_from_adbar(sv_conv.R, sv_conv.V, rmag, vmag, rtasc, decl, fpav, az);

	EZCHECKDIS.V_i = vmag / 0.3048;
	EZCHECKDIS.gamma_i = 90.0 - fpav * DEG;
	EZCHECKDIS.psi = az * DEG;
	EZCHECKDIS.phi_c = 0.0;
	EZCHECKDIS.lambda = 0.0;
	EZCHECKDIS.h_s = (rmag - R_E) / 1852.0;
	EZCHECKDIS.R = rmag / 1852.0;

	if (ref < 2)
	{
		EZCHECKDIS.HOBlank = false;
		EZCHECKDIS.h_o_ft = (rmag - OrbMech::R_Earth) / 0.3048;
		if (EZCHECKDIS.h_o_ft > 9999999.0) EZCHECKDIS.h_o_ft = 9999999.0;
		EZCHECKDIS.h_o_NM = (rmag - OrbMech::R_Earth) / 1852.0;
		if (EZCHECKDIS.h_o_NM > 9999.99) EZCHECKDIS.h_o_NM = 9999.99;
	}
	else
	{
		EZCHECKDIS.HOBlank = true;
		EZCHECKDIS.h_o_ft = EZCHECKDIS.h_o_NM = 0.0;
	}

	OELEMENTS coe;
	coe = OrbMech::coe_from_sv(sv_conv.R, sv_conv.V, mu);

	if (coe.e >= 1)
	{
		EZCHECKDIS.a = 99999.99;
	}
	else
	{
		EZCHECKDIS.a = coe.h*coe.h / mu / (1.0 - coe.e*coe.e) / 1852.0;
	}
	if (EZCHECKDIS.a > 99999.99) EZCHECKDIS.a = 99999.99;
	EZCHECKDIS.e = coe.e;
	EZCHECKDIS.i = coe.i*DEG;
	EZCHECKDIS.omega_p = coe.w*DEG;
	EZCHECKDIS.Omega = coe.RA*DEG;
	EZCHECKDIS.nu = coe.TA*DEG;
	if (coe.e < 0.00001)
	{
		EZCHECKDIS.TABlank = true;
	}
	else
	{
		EZCHECKDIS.TABlank = false;
	}
	if (coe.e < 1.0)
	{
		EZCHECKDIS.MABlank = false;
		EZCHECKDIS.m = OrbMech::TrueToMeanAnomaly(coe.TA, coe.e)*DEG;
	}
	else
	{
		EZCHECKDIS.m = 0.0;
		EZCHECKDIS.MABlank = true;
	}

	if (coe.e < 0.85)
	{
		SV sv_a, sv_p, sv_in;

		//Make this better
		sv_in.R = sv_inert.R;
		sv_in.V = sv_inert.V;
		sv_in.MJD = OrbMech::MJDfromGET(sv_inert.GMT, GMTBASE);
		sv_in.gravref = GetGravref(sv_inert.RBI);

		PMMAPD(sv_in, sv_a, sv_p);

		EZCHECKDIS.h_a = (length(sv_a.R) - R_E) / 1852.0;
		EZCHECKDIS.h_p = (length(sv_p.R) - R_E) / 1852.0;
	}
	else
	{
		EZCHECKDIS.h_p = pow(length(crossp(sv_inert.R, sv_inert.V)), 2) / (mu*(1.0 + coe.e)) - R_E;
		EZCHECKDIS.h_p = EZCHECKDIS.h_p / 1852.0;

		if (coe.e >= 1)
		{
			EZCHECKDIS.h_a = 9999.99;
		}
		else
		{
			EZCHECKDIS.h_a = pow(length(crossp(sv_inert.R, sv_inert.V)), 2) / (mu*(1.0 - coe.e)) - R_E;
			EZCHECKDIS.h_a = EZCHECKDIS.h_a / 1852.0;
		}
	}

	if (EZCHECKDIS.h_p > 9999.99) EZCHECKDIS.h_p = 9999.99;
	if (EZCHECKDIS.h_a > 9999.99) EZCHECKDIS.h_a = 9999.99;

	double lat, lng;
	OrbMech::latlong_from_r(sv_conv.R, lat, lng);
	EZCHECKDIS.phi_c = EZCHECKDIS.phi_D = lat * DEG;
	EZCHECKDIS.lambda = EZCHECKDIS.lambda_D = lng * DEG;

	double LunarRightAscension, LunarDeclination, LunarDistance;
	OrbMech::GetLunarEquatorialCoordinates(OrbMech::MJDfromGET(sv_inert.GMT, GMTBASE), LunarRightAscension, LunarDeclination, LunarDistance);
	EZCHECKDIS.deltaL = LunarDeclination * DEG;

	EZCHECKDIS.UpdateNo = table->CommonBlock.TUP;
	if (ephem->EPHEM.table.size() > 0)
	{
		EZCHECKDIS.EPHB = ephem->EPHEM.table.front().GMT;
		EZCHECKDIS.EPHE = ephem->EPHEM.table.back().GMT;
	}
	else
	{
		EZCHECKDIS.EPHB = 0.0;
		EZCHECKDIS.EPHE = 0.0;
	}
	EZCHECKDIS.LOC = OrbMech::GETfromMJD(CalcGETBase(), GetGMTBase());
	EZCHECKDIS.GRRC = OrbMech::GETfromMJD(CalcGETBase(), GetGMTBase());
	EZCHECKDIS.ZSC = EZCHECKDIS.ZSL = OrbMech::GETfromMJD(CalcGETBase(), GetGMTBase());
	EZCHECKDIS.GRRS = OrbMech::GETfromMJD(CalcGETBase(), GetGMTBase()) - 17.0;
	EZCHECKDIS.R_Day[0] = GZGENCSN.DayofLiftoff;
	EZCHECKDIS.R_Day[1] = GZGENCSN.MonthofLiftoff;
	EZCHECKDIS.R_Day[2] = GZGENCSN.Year;

	if (veh == RTCC_MPT_LM)
	{
		if (ephem->LUNRSTAY.LunarStayBeginGMT < 0)
		{
			EZCHECKDIS.LAL = 0.0;
		}
		else
		{
			EZCHECKDIS.LAL = ephem->LUNRSTAY.LunarStayBeginGMT;
		}

		if (ephem->LUNRSTAY.LunarStayEndGMT > 0 && ephem->LUNRSTAY.LunarStayEndGMT < 10e10)
		{
			EZCHECKDIS.LOL = ephem->LUNRSTAY.LunarStayEndGMT;
		}
		else
		{
			EZCHECKDIS.LOL = 0.0;
		}

		EZCHECKDIS.LSTBlank = false;
	}
	else
	{
		EZCHECKDIS.LAL = 0.0;
		EZCHECKDIS.LOL = 0.0;
		EZCHECKDIS.LSTBlank = true;
	}

	double cfg_weight, csm_weight, lma_weight, lmd_weight, sivb_weight;
	std::bitset<4> cfg;

	PLAWDT(veh, sv_conv.GMT, cfg, cfg_weight, csm_weight, lma_weight, lmd_weight, sivb_weight);
	unsigned cfgint = cfg.to_ulong();

	switch (cfgint)
	{
	case 1:
		sprintf(EZCHECKDIS.CFG, "C");
		break;
	case 2:
		sprintf(EZCHECKDIS.CFG, "S");
		break;
	case 3:
		sprintf(EZCHECKDIS.CFG, "CS");
		break;
	case 4:
		sprintf(EZCHECKDIS.CFG, "A");
		break;
	case 5:
		sprintf(EZCHECKDIS.CFG, "CA");
		break;
	case 6:
		sprintf(EZCHECKDIS.CFG, "SA");
		break;
	case 7:
		sprintf(EZCHECKDIS.CFG, "CSA");
		break;
	case 8:
		sprintf(EZCHECKDIS.CFG, "D");
		break;
	case 9:
		sprintf(EZCHECKDIS.CFG, "CD");
		break;
	case 10:
		sprintf(EZCHECKDIS.CFG, "SD");
		break;
	case 11:
		sprintf(EZCHECKDIS.CFG, "CSD");
		break;
	case 12:
		sprintf(EZCHECKDIS.CFG, "L");
		break;
	case 13:
		sprintf(EZCHECKDIS.CFG, "CL");
		break;
	case 14:
		sprintf(EZCHECKDIS.CFG, "SL");
		break;
	case 15:
		sprintf(EZCHECKDIS.CFG, "CSL");
		break;
	default:
		sprintf(EZCHECKDIS.CFG, " ");
		break;
	}

	EZCHECKDIS.WT = cfg_weight / 0.45359237;
	EZCHECKDIS.WC = csm_weight / 0.45359237;
	EZCHECKDIS.WL = (lma_weight + lmd_weight) / 0.45359237;

	unsigned ii = 0;

	while (ii < table->ManeuverNum && sv_inert.GMT > table->mantable[ii].GMT_BO)
	{
		ii++;
	}

	MPTVehicleDataBlock CommonBlock;
	if (ii == 0)
	{
		CommonBlock = table->CommonBlock;
	}
	else
	{
		CommonBlock = table->mantable[ii - 1].CommonBlock;
	}

	EZCHECKDIS.SPS = CommonBlock.SPSFuelRemaining / 0.45359237;
	EZCHECKDIS.DPS = CommonBlock.LMDPSFuelRemaining / 0.45359237;
	EZCHECKDIS.APS = CommonBlock.LMAPSFuelRemaining / 0.45359237;
	EZCHECKDIS.RCS_C = CommonBlock.CSMRCSFuelRemaining / 0.45359237;
	EZCHECKDIS.RCS_L = CommonBlock.LMRCSFuelRemaining / 0.45359237;
	EZCHECKDIS.J2 = CommonBlock.SIVBFuelRemaining / 0.45359237;

	return;
}

bool RTCC::MPTConfigSubset(const std::bitset<4> &CfgOld, const std::bitset<4> &CfgNew)
{
	//Can't be identical
	if (CfgOld == CfgNew)
	{
		return false;
	}
	//Can't be empty
	if (CfgNew.to_ulong() == 0)
	{
		return false;
	}
	for (unsigned i = 0;i < 4;i++)
	{
		//Does CfgNew have something that CfgOld didn't?
		if (CfgNew[i] == true && CfgOld[i] == false)
		{
			//Can't have that
			return false;
		}
	}

	//Otherwise ok
	return true;
}

void RTCC::MPTGetConfigFromString(const std::string &str, std::bitset<4> &cfg)
{
	cfg.reset();
	for (unsigned i = 0;i < str.length();i++)
	{
		if (str[i] == 'C')
		{
			cfg[RTCC_CONFIG_C] = true;
		}
		else if (str[i] == 'S')
		{
			cfg[RTCC_CONFIG_S] = true;
		}
		else if (str[i] == 'A')
		{
			cfg[RTCC_CONFIG_A] = true;
		}
		else if (str[i] == 'D')
		{
			cfg[RTCC_CONFIG_D] = true;
		}
		else if (str[i] == 'L')
		{
			cfg[RTCC_CONFIG_A] = true;
			cfg[RTCC_CONFIG_D] = true;
		}
	}
}

bool RTCC::MPTIsRCSThruster(int thruster)
{
	if (thruster == RTCC_ENGINETYPE_CSMRCSMINUS2 || thruster == RTCC_ENGINETYPE_CSMRCSMINUS4 || thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
	{
		return true;
	}
	if (thruster == RTCC_ENGINETYPE_LMRCSMINUS2 || thruster == RTCC_ENGINETYPE_LMRCSMINUS4 || thruster == RTCC_ENGINETYPE_LMRCSPLUS2 || thruster == RTCC_ENGINETYPE_LMRCSPLUS4)
	{
		return true;
	}

	return false;
}

bool RTCC::MPTIsUllageThruster(int thruster, int i)
{
	switch (thruster)
	{
	case RTCC_ENGINETYPE_CSMSPS:
		if (i == 0)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMAPS:
		if (i == 3)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMDPS:
		if (i == 3)
		{
			return true;
		}
		break;
	}

	return false;
}

int RTCC::MPTGetPrimaryThruster(int thruster)
{
	switch (thruster)
	{
	case RTCC_ENGINETYPE_CSMSPS:
		return 1;
	case RTCC_ENGINETYPE_SIVB_MAIN:
		return 2;
	case RTCC_ENGINETYPE_LMRCSPLUS2:
	case RTCC_ENGINETYPE_LMRCSPLUS4:
	case RTCC_ENGINETYPE_LMRCSMINUS2:
	case RTCC_ENGINETYPE_LMRCSMINUS4:
		return 3;
	case RTCC_ENGINETYPE_LMAPS:
		return 4;
	case RTCC_ENGINETYPE_LMDPS:
		return 5;
	}

	return 0;
}

bool RTCC::MPTIsPrimaryThruster(int thruster, int i)
{
	switch (thruster)
	{
	case RTCC_ENGINETYPE_CSMRCSPLUS2:
	case RTCC_ENGINETYPE_CSMRCSPLUS4:
	case RTCC_ENGINETYPE_CSMRCSMINUS2:
	case RTCC_ENGINETYPE_CSMRCSMINUS4:
		if (i == 0)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_CSMSPS:
		if (i == 1)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_SIVB_MAIN:
		if (i == 2)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMRCSPLUS2:
	case RTCC_ENGINETYPE_LMRCSPLUS4:
	case RTCC_ENGINETYPE_LMRCSMINUS2:
	case RTCC_ENGINETYPE_LMRCSMINUS4:
		if (i == 3)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMAPS:
		if (i == 4)
		{
			return true;
		}
		break;
	case RTCC_ENGINETYPE_LMDPS:
		if (i == 5)
		{
			return true;
		}
		break;
	}

	return false;
}

MissionPlanTable* RTCC::GetMPTPointer(int L)
{
	if (L == RTCC_MPT_CSM)
	{
		return &PZMPTCSM;
	}
	else
	{
		return &PZMPTLEM;
	}
}

OBJHANDLE RTCC::GetGravref(int body)
{
	if (body == BODY_EARTH)
	{
		return hEarth;
	}

	return hMoon;
}

MPTSV RTCC::SVfromRVGMT(VECTOR3 R, VECTOR3 V, double GMT, int body)
{
	MPTSV sv;

	sv.R = R;
	sv.V = V;
	sv.MJD = OrbMech::MJDfromGET(GMT, GMTBASE);
	sv.gravref = GetGravref(body);
	return sv;
}

int RTCC::PMMXFR(int id, void *data)
{
	MissionPlanTable *mpt;
	//MPTManeuver *maneuver;
	double LowerLimit, UpperLimit, VectorFetchTime;
	int err, working_man, TVC;
	std::bitset<4> CCP, CC, CCMI;
	unsigned CurMan;
	bool LastManReplaceFlag;
	bool replace, dodelete;
	std::string code;

	//Save Logic
	//Setup input table for PMSVCT
	//Move MED inputs to proper location
	//Initialize flags
	//Direct inputs (RTE, MPT, TLI)
	if (id == 32 || id == 33 || id == 37)
	{
		PMMXFRDirectInput *inp = static_cast<PMMXFRDirectInput*>(data);
		std::string purpose;
		double BurnParm72, BurnParm74, BurnParm75, BurnParm76, BurnParm77;
		int BurnParm73;
		int BPIND;

		//RTE
		if (id == 32)
		{
			if (inp->BurnParameterNumber == 1)
			{
				purpose = "TTF";
				inp->GMTI = RZRFTT.GMTI_Primary;
				inp->ThrusterCode = RTCC_ENGINETYPE_CSMSPS;
				
				BurnParm75 = RZRFTT.DeltaV_Primary.x;
				BurnParm76 = RZRFTT.DeltaV_Primary.y;
				BurnParm77 = RZRFTT.DeltaV_Primary.z;
			}
			else if (inp->BurnParameterNumber == 3)
			{
				purpose = "RTE";
				inp->GMTI = PZREAP.RTEPrimaryData.IgnitionGMT;
				inp->ThrusterCode = PZREAP.RTEPrimaryData.ThrusterCode;

				BurnParm75 = PZREAP.RTEPrimaryData.UplinkDV.x;
				BurnParm76 = PZREAP.RTEPrimaryData.UplinkDV.y;
				BurnParm77 = PZREAP.RTEPrimaryData.UplinkDV.z;
			}
			BPIND = 2;
		}
		//Direct Input
		else if (id == 33)
		{
			purpose = "MAN";
			//Move in burn parameters from M40 MED
			if (inp->BurnParameterNumber == 1)
			{
				BurnParm72 = PZBURN.P1_DV;
				BurnParm73 = PZBURN.P1_DVIND;
				BurnParm74 = PZBURN.P1_DT;
			}
			else if (inp->BurnParameterNumber == 2)
			{
				BurnParm75 = PZBURN.P2_DV.x;
				BurnParm76 = PZBURN.P2_DV.y;
				BurnParm77 = PZBURN.P2_DV.z;
			}
			BPIND = inp->BurnParameterNumber;
		}
		//TLI
		else if (id == 37)
		{
			purpose = "TLI";

			PMMSPTInput tliin;

			tliin.QUEID = id;
			tliin.PresentGMT = RTCCPresentTimeGMT();
			tliin.ReplaceCode = 0;
			tliin.InjOpp = inp->BurnParameterNumber;

			PMMSPT(tliin);
			inp->GMTI = tliin.T_RP;
			inp->ThrusterCode = RTCC_ENGINETYPE_SIVB_MAIN;
			inp->AttitudeCode = RTCC_ATTITUDE_SIVB_IGM;
			inp->ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_NONE;
		}

		EphemerisData sv;

		if (inp->ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}

		//MPT header
		mpt = GetMPTPointer(inp->TableCode);
		//Check ground rules
		VectorFetchTime = inp->GMTI;
		err = PMMXFRGroundRules(mpt, inp->GMTI, inp->ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		if (err)
		{
			return 1;
		}
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(inp->TableCode, inp->ThrusterCode, inp->AttitudeCode, CurMan, purpose, TVC, code);
		if (err)
		{
			return 1;
		}
		//Check configuration and thrust
		if (CurMan == 1)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}
		CC = inp->EndConfiguration;
		err = PMMXFRCheckConfigThruster(true, inp->ConfigurationChangeIndicator, CCP, TVC, inp->ThrusterCode, CC, CCMI);
		if (err)
		{
			return 1;
		}
		if (replace && LastManReplaceFlag == false)
		{
			//TBD: Are config codes the same?
		}
		//TBD:  TLI maneuver with targets input
		err = PMMXFRFetchVector(VectorFetchTime, inp->TableCode, sv);
		if (err)
		{
			return 1;
		}

		MPTManeuver man;
		man.code = code;

		//TBD: Is this a TLI maneuver?

		if (inp->AttitudeCode == RTCC_ATTITUDE_SIVB_IGM)
		{
			PMMSPTInput in;
			
			in.AttitudeMode = RTCC_ATTITUDE_SIVB_IGM;
			in.CC = CC.to_ulong();
			in.CCI = RTCC_CONFIGCHANGE_NONE;
			in.CCMI = CCMI.to_ulong();
			in.CurMan = &man;
			in.EndTimeLimit = UpperLimit;
			in.GMT = sv.GMT;
			in.InjOpp = inp->BurnParameterNumber;
			in.mpt = mpt;
			in.PresentGMT = RTCCPresentTimeGMT();
			if (mpt->mantable.size() == 0)
			{
				in.PrevMan = NULL;
			}
			else
			{
				in.PrevMan = &mpt->mantable.back();
			}
			in.QUEID = 39;
			in.R = sv.R;
			in.ReplaceCode = inp->ReplaceCode;
			in.StartTimeLimit = LowerLimit;
			in.Table = RTCC_MPT_CSM;
			in.ThrusterCode = RTCC_ENGINETYPE_SIVB_MAIN;
			in.TVC = RTCC_MANVEHICLE_SIVB;
			in.T_RP = 0.0;
			in.V = sv.V;

			PMMSPT(in);
		}
		else
		{
			PMMMCDInput in;

			in.GMTI = inp->GMTI;
			in.Thruster = inp->ThrusterCode;
			in.dt_ullage = inp->dt_ullage;
			in.Attitude = inp->AttitudeCode;
			in.ConfigChangeInd = inp->ConfigurationChangeIndicator;
			in.ConfigCodeAfter = CC.to_ulong();
			in.TVC = TVC;
			in.DockingAngle = inp->DockingAngle;
			in.CCMI = CCMI;
			in.ID = id;
			in.UllageThrusterOpt = inp->UllageThrusterOption;
			in.DT_10PCT = inp->DT10P;
			in.DPSScaleFactor = inp->DPSScaleFactor;
			in.HeadsUpDownInd = inp->HeadsUpDownIndicator;
			in.CoordinateInd = inp->CoordinateIndicator;
			in.StartTimeLimit = LowerLimit;
			in.EndTimeLimit = UpperLimit;
			in.sv_anchor = sv;
			in.FrozenManeuver = false;
			in.CurrentManeuver = CurMan;
			in.mpt = mpt;
			in.BPIND = BPIND;
			in.BurnParm72 = BurnParm72;
			in.BurnParm73 = BurnParm73;
			in.BurnParm74 = BurnParm74;
			in.BurnParm75 = BurnParm75;
			in.BurnParm76 = BurnParm76;
			in.BurnParm77 = BurnParm77;
			in.Pitch = inp->Pitch;
			in.Yaw = inp->Yaw;
			in.Roll = inp->Roll;

			err = PMMMCD(in, man);
		}
		
		//Either add a new maneuver or replace one
		if (CurMan > mpt->mantable.size())
		{
			mpt->mantable.push_back(man);
		}
		else
		{
			mpt->mantable[CurMan - 1] = man;
		}

		EphemerisData sv_anchor;
		bool landed;
		if (PMMXFRFetchAnchorVector(inp->TableCode, sv_anchor, landed))
		{
			if (inp->TableCode == RTCC_MPT_CSM)
			{
				RTCCONLINEMON.TextBuffer[0] = "CSM";
			}
			else
			{
				RTCCONLINEMON.TextBuffer[0] = "LEM";
			}
			PMXSPT("PMMXFR", 39);
			return 39;
		}
		PMSVCT(8, inp->TableCode, &sv_anchor, landed);
	}
	//LOI/MCC, GPM, DKI, SPQ, Two-Impulse
	else if (id == 39 || id == 40 || id == 41 || id == 42)
	{
		std::string purpose;
		PMMMPTInput in;
		PMMXFR_Impulsive_Input *inp = static_cast<PMMXFR_Impulsive_Input*>(data);
		double GMTI;
		int num_man, plan;
		bool update_csm_eph = false, update_lm_eph = false;

		//Logic in MED decoder should prevent that both of these are true
		if (inp->ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}
		if (inp->DeleteGMT > 0)
		{
			dodelete = true;
		}
		else
		{
			dodelete = false;
		}

		//DKI, SPQ and LDPP
		if (id == 41)
		{
			if (inp->Plan < 0)
			{
				num_man = 1;//PZLDPELM.num_man;
			}
			else if (inp->Plan == 0)
			{
				num_man = 1;//PZDKIT.Block[0].NumMan;
			}
			else
			{
				num_man = 1;//PZDKIT.Block[inp->Plan - 1].NumMan;
			}
		}
		else if (id == 42)
		{
			num_man = 2;
		}
		else
		{
			num_man = 1;
		}

		if (num_man < 1)
		{
			PMXSPT("PMMXFR", 38);
			return 38;
		}

		working_man = 1;
	RTCC_PMMXFR_1:
		if (id == 39)
		{
			if (inp->Type == 0)
			{
				GMTI = PZMCCXFR.sv_man_bef[inp->Plan - 1].GMT;
				purpose = "MCC";
				plan = inp->Table;
			}
			else
			{
				GMTI = PZLRBELM.sv_man_bef[inp->Plan - 1].GMT;
				purpose = "LOI";
				plan = inp->Table;
			}
		}
		else if (id == 40)
		{
			GMTI = PZGPMELM.SV_before.GMT;
			purpose = PZGPMELM.code;
			plan = inp->Plan;
		}
		else if (id == 41)
		{
			if (inp->Plan < 0)
			{
				GMTI = PZLDPELM.sv_man_bef[0].GMT;
				purpose = PZLDPELM.code[0];
				plan = PZLDPELM.plan[0];
			}
			else
			{
				GMTI = PZDKIELM.Block[0].SV_before[0].GMT;
				purpose = PZDKIT.Block[0].Display[0].Man_ID;
				plan = PZDKIT.Block[0].Display[0].VEH;
			}
		}
		else if (id == 42)
		{
			GMTI = PZMYSAVE.SV_before[working_man - 1].GMT;
			purpose = PZMYSAVE.code[working_man - 1];
			plan = PZMYSAVE.plan[working_man - 1];
		}

		mpt = GetMPTPointer(plan);

		if (inp->DeleteGMT > 0 && working_man == 1)
		{
			//Delete all unfrozen maneuvers in this table after input time
			PMMXFRDeleteOption(plan, GMTI);
		}
		//Check ground rules
		err = PMMXFRGroundRules(mpt, GMTI, inp->ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		if (err)
		{
			return 1;
		}
		if (working_man == 1)
		{
			in.DockingAngle = mpt->DeltaDockingAngle;
			if (mpt->mantable.size() > 0)
			{
				in.CSMWeight = 0.0;
				in.LMWeight = 0.0;
				in.VehicleWeight = mpt->mantable.back().TotalMassAfter;
			}
			else
			{
				in.CSMWeight = 0.0;
				in.LMWeight = 0.0;
				in.VehicleWeight = mpt->TotalInitMass;
			}
			if (replace)
			{
				//TBD
			}
		}
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(plan, inp->Thruster[working_man - 1], inp->Attitude[working_man - 1], CurMan, purpose, TVC, code);
		//Check thruster
		if (mpt->mantable.size() == 0)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}
		CC = CCMI = CCP;
		err = PMMXFRCheckConfigThruster(false, 0, CCP, TVC, inp->Thruster[working_man - 1], CC, CCMI);
		//Is attitude mode an AGS?
		if (inp->Attitude[working_man - 1] == 5)
		{
			if (ELFECH(GMTI, 4 - plan, in.sv_other))
			{
				//TBD: Error message?
				return 1;
			}
		}

		in.Attitude = inp->Attitude[working_man - 1];
		in.CONFIG = CC.to_ulong();
		in.CurrentManeuver = CurMan;
		in.DETU = inp->dt_ullage[working_man - 1];
		in.DPSScaleFactor = inp->DPSScaleFactor[working_man - 1];
		in.DT_10PCT = inp->DT10P[working_man - 1];
		in.IgnitionTimeOption = inp->TimeFlag[working_man - 1];
		in.IterationFlag = inp->IterationFlag[working_man - 1];
		in.LowerTimeLimit = LowerLimit;
		in.mpt = mpt;
		if (id == 39)
		{
			if (inp->Type == 0)
			{
				in.sv_before = PZMCCXFR.sv_man_bef[inp->Plan - 1];
				in.V_aft = PZMCCXFR.V_man_after[inp->Plan - 1];
			}
			else
			{
				in.sv_before = PZLRBELM.sv_man_bef[inp->Plan - 1];
				in.V_aft = PZLRBELM.V_man_after[inp->Plan - 1];
			}
		}
		else if (id == 40)
		{
			in.sv_before = PZGPMELM.SV_before;
			in.V_aft = PZGPMELM.V_after;
		}
		else if (id == 41)
		{
			if (inp->Plan < 0)
			{
				in.sv_before = PZLDPELM.sv_man_bef[0];
				in.V_aft = PZLDPELM.V_man_after[0];
			}
			else
			{
				in.sv_before = PZDKIELM.Block[0].SV_before[0];
				in.V_aft = PZDKIELM.Block[0].V_after[0];
			}
		}
		else
		{
			in.sv_before = PZMYSAVE.SV_before[working_man - 1];
			in.V_aft = PZMYSAVE.V_after[working_man - 1];
		}
		in.Thruster = inp->Thruster[working_man - 1];
		in.UpperTimeLimit = UpperLimit;
		in.UT = inp->UllageThrusterOption[working_man - 1];
		in.VC = TVC;

		MPTManeuver man;

		man.code = code;
		if (PMMMPT(in, man))
		{
			return 1;
		}
		if (working_man == 1)
		{
			if (GMTI < mpt->UpcomingManeuverGMT)
			{
				mpt->UpcomingManeuverGMT = GMTI;
			}
		}
		//Either add a new maneuver or replace one
		if (CurMan > mpt->mantable.size())
		{
			mpt->mantable.push_back(man);
		}
		else
		{
			mpt->mantable[CurMan - 1] = man;
		}

		if (plan == RTCC_MPT_CSM)
		{
			update_csm_eph = true;
		}
		else
		{
			update_lm_eph = true;
		}

		if (num_man > working_man)
		{
			working_man++;
			goto RTCC_PMMXFR_1;
		}

		if (update_csm_eph)
		{
			PMSVCT(8, RTCC_MPT_CSM);
		}
		if (update_lm_eph)
		{
			PMSVCT(8, RTCC_MPT_LM);
		}
	}
	//PDI
	else if (id == 43)
	{
		if (med_m86.ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}
		//Store purpose code
		std::string purpose = "DSC";
		//MPT header
		mpt = GetMPTPointer(med_m86.Veh);

		PMMLDPInput in;
		MPTManeuver man;

		if (mpt->mantable.size() == 0)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}
		//Do we have a full LM?
		if (!(CCP[RTCC_CONFIG_A] && CCP[RTCC_CONFIG_D]))
		{
			return 1;
		}

		in.CurrentManeuver = mpt->mantable.size();
		in.HeadsUpDownInd = med_m86.HeadsUp;
		in.mpt = mpt;
		if (mpt->mantable.size() == 0)
		{
			in.sv.R = EZANCHR3.AnchorVectors[9].R;
			in.sv.V = EZANCHR3.AnchorVectors[9].V;
			in.sv.MJD = OrbMech::MJDfromGET(EZANCHR3.AnchorVectors[9].GMT, GMTBASE);
			in.sv.gravref = GetGravref(EZANCHR3.AnchorVectors[9].RBI);
			in.sv.mass = mpt->TotalInitMass;
		}
		else
		{
			in.sv.R = mpt->mantable.back().R_BO;
			in.sv.V = mpt->mantable.back().V_BO;
			in.sv.MJD = OrbMech::MJDfromGET(mpt->mantable.back().GMT_BO, GMTBASE);
			in.sv.gravref = GetGravref(mpt->mantable.back().RefBodyInd);
			in.sv.mass = mpt->mantable.back().TotalMassAfter;
		}
		in.TLAND = med_m86.Time;
		in.TrimAngleInd = med_m86.TrimAngleIndicator;

		//Determine time
		PMMLDP(in, man);

		//Check ground rules
		err = PMMXFRGroundRules(mpt, man.GMTI, med_m86.ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(med_m86.Veh, RTCC_ENGINETYPE_LMDPS, RTCC_ATTITUDE_PGNS_DESCENT, CurMan, purpose, TVC, code);
		//Check configuration and thrust
		CC = CCP;
		err = PMMXFRCheckConfigThruster(true, 0, CCP, TVC, RTCC_ENGINETYPE_LMDPS, CC, CCMI);

		man.code = code;
		mpt->mantable.push_back(man);
		mpt->TimeToBeginManeuver[CurMan - 1] = mpt->TimeToEndManeuver[CurMan - 1] = man.GMTMAN;
		EMSTRAJ(EZANCHR3.AnchorVectors[9], RTCC_MPT_LM);
	}
	//Ascent
	else if (id == 44)
	{
		if (med_m85.ReplaceCode > 0)
		{
			replace = true;
		}
		else
		{
			replace = false;
		}
		//Store purpose code
		std::string purpose = "ASC";
		//MPT header
		mpt = GetMPTPointer(med_m85.VEH);

		MPTManeuver man;

		if (mpt->mantable.size() == 0)
		{
			CCP = mpt->CommonBlock.ConfigCode;
		}
		else
		{
			CCP = mpt->mantable.back().CommonBlock.ConfigCode;
		}

		man.GMTI = GMTfromGET(JZLAI.t_launch);

		//Check ground rules
		err = PMMXFRGroundRules(mpt, man.GMTI, med_m85.ReplaceCode, LastManReplaceFlag, LowerLimit, UpperLimit, CurMan, VectorFetchTime);
		//Format maneuver code
		err = PMMXFRFormatManeuverCode(med_m85.VEH, RTCC_ENGINETYPE_LMAPS, RTCC_ATTITUDE_PGNS_ASCENT, CurMan, purpose, TVC, code);
		//Check configuration and thrust
		CC[RTCC_CONFIG_A] = true;
		err = PMMXFRCheckConfigThruster(true, RTCC_CONFIGCHANGE_UNDOCKING, CCP, TVC, RTCC_ENGINETYPE_LMAPS, CC, CCMI);

		man.code = code;
		man.GMTMAN = man.GMTI;
		man.dV_inertial.x = JZLAI.R_D;
		man.dV_inertial.y = JZLAI.Y_D;
		man.dV_LVLH.x = JZLAI.R_D_dot;
		man.dV_LVLH.y = JZLAI.Y_D_dot;
		man.dV_LVLH.z = JZLAI.Z_D_dot;
		man.CommonBlock.ConfigChangeInd = RTCC_CONFIGCHANGE_UNDOCKING;
		man.ConfigCodeBefore[RTCC_CONFIG_A] = true;
		man.ConfigCodeBefore[RTCC_CONFIG_D] = true;
		man.CommonBlock.ConfigCode = CC;
		man.Thruster = RTCC_ENGINETYPE_LMAPS;
		man.AttitudeCode = RTCC_ATTITUDE_PGNS_ASCENT;
		mpt->mantable.push_back(man);
		mpt->TimeToBeginManeuver[CurMan - 1] = mpt->TimeToEndManeuver[CurMan - 1] = man.GMTMAN;
		PMSVCT(8, RTCC_MPT_LM);
	}
	return 0;
}

int RTCC::PMMXFRGroundRules(MissionPlanTable * mpt, double GMTI, unsigned ReplaceMan, bool &LastManReplaceFlag, double &LowerLimit, double &UpperLimit, unsigned &CurMan, double &VectorFetchTime)
{
	double CurrentGMT;
	bool DeleteFlag = false;
	LastManReplaceFlag = false;

	CurrentGMT = RTCCPresentTimeGMT();

	if (ReplaceMan > 0)
	{
		CurMan = ReplaceMan;
		LastManReplaceFlag = true;

		//Is maneuver prior to present time?
		if (GMTI < CurrentGMT)
		{
			PMXSPT("PMMXFR", 1);
			return 1;
		}
		UpperLimit = 1e70;
		//Does maneuver to be replaced exist?
		if (ReplaceMan > mpt->mantable.size())
		{
			RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
			PMXSPT("PMMXFR", 2);
			return 2;
		}
		//Is this the last maneuver in MPT?
		if (ReplaceMan != mpt->mantable.size())
		{
			LastManReplaceFlag = false;
			if (GMTI > mpt->mantable[ReplaceMan].GMTMAN)
			{
				RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
				PMXSPT("PMMXFR", 3);
				return 3;
			}
		}
		//Set lower limit to end time of previous maneuver
		if (ReplaceMan > 1)
		{
			LowerLimit = mpt->mantable[ReplaceMan - 2].GMT_BO;
		}
		//Is this first maneuver in MPT?
		if (ReplaceMan == 1)
		{
			LowerLimit = CurrentGMT;
		}
		if (GMTI < LowerLimit)
		{
			RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
			PMXSPT("PMMXFR", 3);
			return 3;
		}
	}
	else
	{
		//Is maneuver prior to present time?
		if (GMTI < CurrentGMT)
		{
			PMXSPT("PMMXFR", 1);
			return 1;
		}
		if (mpt->mantable.size() == 0)
		{
			//Set lower limit to present time
			LowerLimit = CurrentGMT;
		}
		else
		{
			do
			{
				LowerLimit = mpt->mantable.back().GMT_BO;
				if (GMTI > LowerLimit)
				{
					break;
				}
				mpt->mantable.pop_back();
				DeleteFlag = true;
				if (mpt->mantable.size() == 0)
				{
					LowerLimit = CurrentGMT;
				}
			} while (mpt->mantable.size() > 0);
		}
		CurMan = mpt->mantable.size() + 1;
		if (CurMan >= mpt->MaxManeuverNum)
		{
			PMXSPT("PMMXFR", 5);
			return 5;
		}
		if (CurMan == 1 && DeleteFlag)
		{
			RTCCONLINEMON.IntBuffer[0] = (int)ReplaceMan;
			PMXSPT("PMMXFR", 3);
			return 3;
		}
		mpt->ManeuverNum = CurMan;
		UpperLimit = 1e70;
	}

	//Store pointers and compute pointers to previous descriptive block
	if (ReplaceMan > 0)
	{
		if (mpt->mantable[ReplaceMan - 1].GMTMAN < GMTI)
		{
			//Set vector fetch time to time of original maneuver
			VectorFetchTime = mpt->mantable[ReplaceMan - 1].GMTMAN;
		}
	}

	//TBD: Set upper and lower limit times
	if (CurMan == 1)
	{
		return 0;
	}
	//TBD: Store descriptive block for PMMMCD, also store area and weight

	return 0;
}

int RTCC::PMMXFRFormatManeuverCode(int Table, int Thruster, int Attitude, unsigned Maneuver, std::string ID, int &TVC, std::string &code)
{
	char Veh, Thr, Guid, Tab;
	char Buffer[20];

	//Format all but purpose of maneuver code

	//Set MPT
	if (Table == RTCC_MPT_CSM)
	{
		Tab = 'C';
	}
	else
	{
		Tab = 'L';
	}

	//Set thrusting vehicle ID
	if (Thruster == RTCC_ENGINETYPE_LOX_DUMP || Thruster == RTCC_ENGINETYPE_SIVB_APS || Thruster == RTCC_ENGINETYPE_SIVB_MAIN)
	{
		TVC = 2;
		Veh = 'S';
	}
	else if (Thruster == RTCC_ENGINETYPE_CSMSPS || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4 || Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4)
	{
		TVC = 1;
		Veh = 'C';
	}
	else
	{
		TVC = 3;
		Veh = 'L';
	}

	if (Thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		Thr = 'S';
	}
	else if (Thruster == RTCC_ENGINETYPE_LMDPS)
	{
		Thr = 'D';
	}
	else if (Thruster == RTCC_ENGINETYPE_LMAPS)
	{
		Thr = 'A';
	}
	else if (Thruster == RTCC_ENGINETYPE_LOX_DUMP || Thruster == RTCC_ENGINETYPE_SIVB_MAIN)
	{
		Thr = 'J';
	}
	else
	{
		Thr = 'R';
	}

	if (Attitude == RTCC_ATTITUDE_INERTIAL)
	{
		Guid = 'I';
	}
	else if (Attitude == RTCC_ATTITUDE_MANUAL)
	{
		Guid = 'M';
	}
	else if (Attitude == RTCC_ATTITUDE_LAMBERT)
	{
		Guid = 'L';
	}
	else if (Attitude == RTCC_ATTITUDE_PGNS_EXDV || Attitude == RTCC_ATTITUDE_PGNS_DESCENT || Attitude == RTCC_ATTITUDE_PGNS_ASCENT)
	{
		Guid = 'X';
	}
	else if (Attitude == RTCC_ATTITUDE_AGS_EXDV || Attitude == RTCC_ATTITUDE_AGS_ASCENT)
	{
		Guid = 'A';
	}
	else if (Attitude == RTCC_ATTITUDE_SIVB_IGM)
	{
		Guid = 'G';
	}
	else
	{
		Guid = 'U';
	}

	sprintf(Buffer, "%c%c%c%c%02d%s", Tab, Veh, Thr, Guid, Maneuver, ID.c_str());
	code.assign(Buffer);
	
	return 0;
}

int RTCC::PMMXFRCheckConfigThruster(bool CheckConfig, int CCI, const std::bitset<4> &CCP, int TVC, int Thruster, std::bitset<4> &CC, std::bitset<4> &CCMI)
{
	if (CheckConfig)
	{
		if (CCI > 1)
		{
			CCMI = CCP;
		}
		else
		{
			if (CCI < 1)
			{
				CC = CCP;
			}

			CCMI = CC;
			//Obtain docking angle from previous block
		}
	}
	if (TVC == 2)
	{
		if (!(CC[RTCC_CONFIG_S] && CCP[RTCC_CONFIG_S]))
		{
			PMXSPT("PMMXFR", 6);
			return 6;
		}
	}
	else
	{
		if (CCMI[RTCC_CONFIG_S] && (Thruster == RTCC_ENGINETYPE_LMAPS || Thruster == RTCC_ENGINETYPE_LMDPS || Thruster == RTCC_ENGINETYPE_CSMSPS))
		{
			return 6;
		}
		if (TVC > 2)
		{
			if (Thruster == RTCC_ENGINETYPE_LMAPS && !(CC[RTCC_CONFIG_A] && CCP[RTCC_CONFIG_A]))
			{
				PMXSPT("PMMXFR", 6);
				return 6;
			}
			else if (Thruster == RTCC_ENGINETYPE_LMDPS && !(CC[RTCC_CONFIG_D] && CCP[RTCC_CONFIG_D]))
			{
				PMXSPT("PMMXFR", 6);
				return 6;
			}
		}
		else
		{
			if (!(CC[RTCC_CONFIG_C] && CCP[RTCC_CONFIG_C]))
			{
				PMXSPT("PMMXFR", 6);
				return 6;
			}
		}
	}

	//No configuration change
	if (CCI < 1)
	{
		return 0;
	}
	else if (CCI == 1)
	{
		if (CCP == CC)
		{
			PMXSPT("PMMXFR", 6);
			return 6;
		}
		else if (!MPTConfigSubset(CCP, CC))
		{
			PMXSPT("PMMXFR", 6);
			return 6;
		}
		else
		{
			return 0;
		}
	}

	//TBD: Docking

	return 0;
}

int RTCC::PMMXFRFetchVector(double GMTI, int L, EphemerisData &sv)
{
	int err2 = ELFECH(GMTI, L, sv);
	if (err2)
	{
		return 9;
	}

	return 0;
}

int RTCC::PMMXFRFetchAnchorVector(int L, EphemerisData &sv, bool &landed)
{
	landed = false;
	if (L == RTCC_MPT_LM)
	{
		double gmt = RTCCPresentTimeGMT();

		if (gmt >= EZEPH2.LUNRSTAY.LunarStayBeginGMT && gmt <= EZEPH2.LUNRSTAY.LunarStayEndGMT)
		{
			landed = true;
			return 0;
		}
	}

	return EMGVECSTOutput(L, sv);
}

void RTCC::PMMXFRWeightAtInitiation(int CCI, int CCMI, double &weight)
{
	if (CCI != 1)
	{
		return;
	}

	//If undocking, determine weight
}

bool RTCC::PMMXFRDeleteOption(int L, double GMTI)
{
	MissionPlanTable *mpt = GetMPTPointer(L);

	if (mpt->ManeuverNum == 0)
	{
		return false;
	}
	do
	{
		if (GMTI <= mpt->TimeToEndManeuver[mpt->ManeuverNum - 1])
		{
			if (mpt->mantable[mpt->ManeuverNum - 1].FrozenManeuverInd)
			{
				PMXSPT("PMMXFR", 37);
				return true;
			}

			mpt->mantable.pop_back();
			mpt->ManeuverNum--;
		}
		else
		{
			break;
		}
	} while (mpt->ManeuverNum > 0);

	return false;
}

int RTCC::PMMMCDCallEMSMISS(EphemerisData sv0, double GMTI, EphemerisData &sv1)
{
	double dt = GMTI - sv0.GMT;
	sv1 = coast(sv0, dt);
	return 0;
}

double RTCC::RTCCPresentTimeGMT()
{
	return (oapiGetSimMJD() - GMTBASE)*24.0*3600.0;
}

double RTCC::GetOnboardComputerThrust(int thruster)
{
	if (thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		return MCTST9;
	}
	else if (thruster == RTCC_ENGINETYPE_LMAPS)
	{
		return MCTAT9;
	}
	else if (thruster == RTCC_ENGINETYPE_LMDPS)
	{
		return MCTDT9;
	}
	else if (thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || thruster == RTCC_ENGINETYPE_CSMRCSMINUS2)
	{
		return MCTCT5;
	}
	else if (thruster == RTCC_ENGINETYPE_CSMRCSPLUS4 || thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
	{
		return MCTCT6;
	}
	else if (thruster == RTCC_ENGINETYPE_LMRCSPLUS2 || thruster == RTCC_ENGINETYPE_LMRCSMINUS2)
	{
		return MCTLT5;
	}
	else if (thruster == RTCC_ENGINETYPE_LMRCSPLUS4 || thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
	{
		return MCTLT6;
	}

	return 1e70;
}

void RTCC::GetSystemGimbalAngles(int thruster, double &P_G, double &Y_G) const
{
	if (thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		P_G = MCTSPG;
		Y_G = MCTSYG;
	}
	else if (thruster == RTCC_ENGINETYPE_LMAPS)
	{
		P_G = MCTAPG;
		Y_G = MCTARG;
	}
	else if (thruster == RTCC_ENGINETYPE_LMDPS)
	{
		P_G = MCTDPG;
		Y_G = MCTDRG;
	}
	else
	{
		P_G = 0.0;
		Y_G = 0.0;
	}
}

void RTCC::PMDDMT(int MPT_ID, unsigned ManNo, int REFSMMAT_ID, bool HeadsUp, DetailedManeuverTable &res)
{
	if (MPT_ID == 0) return;

	res.error = "";

	MissionPlanTable *table, *othertable;
	MPTManeuver *man;
	int otherID;

	if (MPT_ID == RTCC_MPT_LM)
	{
		table = &PZMPTLEM;
		othertable = &PZMPTCSM;
		otherID = RTCC_MPT_CSM;
	}
	else
	{
		table = &PZMPTCSM;
		othertable = &PZMPTLEM;
		otherID = RTCC_MPT_LM;
	}

	if (ManNo > table->mantable.size())
	{
		res.error = "MANEUVER NOT AVAILABLE";
		return;
	}

	double mu;

	man = &table->mantable[ManNo - 1];

	sprintf_s(res.CODE, man->code.c_str());
	res.GETI = GETfromGMT(man->GMTI);
	res.AGS_GETI = res.CFP_GETI = res.Lam_GETI = res.PGNS_GETI = res.GETI;
	res.GETR = MCGREF * 3600.0;
	res.PETI = res.GETI - res.GETR;
	res.DVM = man->DV_M / 0.3048;
	res.DVREM = man->DVREM / 0.3048;
	res.DVC = man->DVC / 0.3048;

	res.DT_B = man->dt_BD - man->dt_TO;
	res.DT_U = man->dt_ullage;
	res.DT_TO = man->dt_TO;
	res.DV_TO = man->dv_TO / 0.3048;
	if (man->Thruster == RTCC_ENGINETYPE_CSMSPS)
	{
		res.DEL_P = man->P_G*DEG + 2.15;
		res.DEL_Y = man->Y_G*DEG - 0.95;
	}
	else
	{
		res.DEL_P = man->P_G*DEG;
		res.DEL_Y = man->Y_G*DEG;
	}
	
	res.VF = man->V_F / 0.3048;
	res.VS = man->V_S / 0.3048;
	res.VD = man->V_D / 0.3048;
	res.H_BI = man->h_BI / 1852.0;
	res.P_BI = man->lat_BI*DEG;
	res.L_BI = man->lng_BI*DEG;
	res.F_BI = man->eta_BI*DEG;
	res.E = man->e_BO;
	res.I = man->i_BO*DEG;
	res.WP = man->g_BO*DEG;
	res.HA = man->h_a / 1852.0;
	if (res.HA > 99999.999)
	{
		res.HA = 99999.999;
	}
	res.HP = man->h_p / 1852.0;
	res.L_AN = man->lng_AN*DEG;

	res.P_LLS = 0.0;
	res.L_LLS = 0.0;
	res.R_LLS = MCSMLR / 1852.0;

	res.PHASE = 0.0;
	res.PHASE_DOT = 0.0;
	res.WEDGE_ANG = 0.0;
	res.DH = 0.0;

	if (man->AttitudeCode == 5)
	{
		res.PGNS_DV = _V(0, 0, 0);
		res.AGS_DV = man->dV_LVLH / 0.3048;
	}
	else if (man->AttitudeCode == 4)
	{
		res.PGNS_DV = man->dV_LVLH / 0.3048;
		if (man->TVC == 3)
		{
			//Calc AGS DV, done further down
		}
	}
	else
	{
		res.PGNS_DV = _V(0, 0, 0);
		res.AGS_DV = _V(0, 0, 0);
	}

	if (MPT_ID == RTCC_MPT_CSM)
	{
		sprintf_s(res.C_STA_ID, table->StationID.c_str());
		res.C_GMTV = table->GMTAV;
		res.C_GETV = GETfromGMT(table->GMTAV);
	}
	else
	{
		sprintf_s(res.L_STA_ID, table->StationID.c_str());
		res.L_GMTV = table->GMTAV;
		res.L_GETV = GETfromGMT(table->GMTAV);
	}

	if (man->FrozenManeuverInd)
	{
		//TBD: frozen maneuver
	}
	else
	{
		res.X_GMTV = 0.0;
		sprintf(res.X_STA_ID, "");
	}

	if (man->RefBodyInd == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		sprintf_s(res.REF, "EARTH");
	}
	else
	{
		mu = OrbMech::mu_Moon;
		sprintf_s(res.REF, "LUNAR");
	}

	bool UseOtherMPT;

	if (othertable->CommonBlock.TUP > 0)
	{
		UseOtherMPT = true;
	}
	else
	{
		UseOtherMPT = false;
	}

	VECTOR3 X_B, Y_B, Z_B, X_P, Y_P, Z_P;

	X_B = man->X_B;
	Y_B = man->Y_B;
	Z_B = man->Z_B;

	MATRIX3 REFSMMAT;
	bool RefsAvailable = true;

	if (REFSMMAT_ID == 100)
	{
		REFSMMAT = PIDREF(man->A_T, man->R_BI, man->V_BI, man->P_G, man->Y_G, HeadsUp);
		sprintf_s(res.REFSMMAT_Code, "DES");
	}
	else
	{
		REFSMMATData refdata;

		if (man->TVC == 3)
		{
			refdata = EZJGMTX3.data[REFSMMAT_ID - 1];
		}
		else
		{
			refdata = EZJGMTX1.data[REFSMMAT_ID - 1];
		}

		if (refdata.ID == 0)
		{
			RefsAvailable = false;
			res.error = "REFSMMAT NOT AVAILABLE";
			sprintf(res.REFSMMAT_Code, "");
		}
		else
		{
			REFSMMAT = refdata.REFSMMAT;
			FormatREFSMMATCode(REFSMMAT_ID, refdata.ID, res.REFSMMAT_Code);
		}
	}

	if (man->TVC == 3)
	{
		res.isCSMTV = false;
	}
	else
	{
		res.isCSMTV = true;
	}

	if (RefsAvailable)
	{
		X_P = _V(REFSMMAT.m11, REFSMMAT.m12, REFSMMAT.m13);
		Y_P = _V(REFSMMAT.m21, REFSMMAT.m22, REFSMMAT.m23);
		Z_P = _V(REFSMMAT.m31, REFSMMAT.m32, REFSMMAT.m33);

		double MG, OG, IG, C;

		MG = asin(dotp(Y_P, X_B));
		C = abs(MG);

		if (abs(C - PI05) < 0.0017)
		{
			OG = 0.0;
			IG = atan2(dotp(X_P, Z_B), dotp(Z_P, Z_B));
		}
		else
		{
			OG = atan2(-dotp(Z_B, Y_P), dotp(Y_B, Y_P));
			IG = atan2(-dotp(X_B, Z_P), dotp(X_B, X_P));
		}

		if (man->TVC == 3)
		{
			double Y, P, R;
			Y = asin(-cos(MG)*sin(OG));
			if (abs(sin(Y)) != 1.0)
			{
				R = atan2(sin(MG), cos(OG)*cos(MG));
				P = atan2(sin(IG)*cos(OG) + sin(MG)*sin(OG)*cos(IG), cos(OG)*cos(IG) - sin(MG)*sin(OG)*sin(IG));
			}
			else
			{
				P = 0.0;
				R = 0.0;
			}
			res.FDAIAtt = _V(R, P, Y);
		}
		else
		{
			res.FDAIAtt = _V(OG, IG, MG);
		}

		for (int i = 0;i < 3;i++)
		{
			res.FDAIAtt.data[i] *= DEG;
			if (res.FDAIAtt.data[i] < 0)
			{
				res.FDAIAtt.data[i] += 360.0;
			}
			if (res.FDAIAtt.data[i] >= 359.95)
			{
				res.FDAIAtt.data[i] = 0.0;
			}
		}
		res.IMUAtt = _V(OG, IG, MG);
		for (int i = 0;i < 3;i++)
		{
			res.IMUAtt.data[i] *= DEG;
			if (res.IMUAtt.data[i] < 0)
			{
				res.IMUAtt.data[i] += 360.0;
			}
			if (res.IMUAtt.data[i] >= 359.95)
			{
				res.IMUAtt.data[i] = 0.0;
			}
		}
	}
	else
	{
		res.FDAIAtt = _V(0, 0, 0);
		res.IMUAtt = _V(0, 0, 0);
	}

	VECTOR3 DV, V_G;

	if (man->AttitudeCode < 3 || man->AttitudeCode > 5)
	{
		DV = man->A_T*man->DV_M;
	}
	else
	{
		DV = man->dV_inertial;
	}

	if (RefsAvailable)
	{
		V_G.x = dotp(DV, X_P);
		V_G.y = dotp(DV, Y_P);
		V_G.z = dotp(DV, Z_P);

		res.VG = V_G / 0.3048;
	}
	else
	{
		res.VG = _V(0, 0, 0);
	}

	res.LVLHAtt.x = man->Y_H * DEG;
	res.LVLHAtt.y = man->P_H * DEG;
	res.LVLHAtt.z = man->R_H * DEG;

	if (UseOtherMPT)
	{
		ELVCTRInputTable interin;
		ELVCTROutputTable interout;
		EphemerisData sv_other;

		interin.L = 4 - MPT_ID;
		interin.GMT = man->GMT_BO;

		ELVCTR(interin, interout);

		if (interout.ErrorCode == 0)
		{
			sv_other = interout.SV;

			VECTOR3 R_m = man->R_BO;
			VECTOR3 V_m = man->V_BO;
			VECTOR3 H_m = crossp(R_m, V_m);

			VECTOR3 R_a = sv_other.R;
			VECTOR3 V_a = sv_other.V;
			VECTOR3 H_a = crossp(R_a, V_a);

			VECTOR3 N = crossp(H_a, H_m);
			double h_m = length(H_m);
			double h_a = length(H_a);
			double v_a_sq = dotp(V_a, V_a);
			double v_m_sq = dotp(V_m, V_m);
			double r_a = length(R_a);
			double r_m = length(R_m);
			double a_a = r_a / (2.0 - r_a * v_a_sq / mu);
			double a_m = r_m / (2.0 - r_m * v_m_sq / mu);

			if (man->AttitudeCode == 4 && man->TVC == 3)
			{
				res.AGS_DV = PIAEDV(DV, R_m, V_m, man->R_BI, false) / 0.3048;
			}

			if (a_a > 0 && a_m > 0)
			{
				double theta_dot = PI2 * (sqrt(pow(a_a, 3) / pow(a_m, 3)) - 1.0);
				double delta = asin(length(N) / h_m / h_a);

				res.PHASE_DOT = theta_dot * DEG;
				res.WEDGE_ANG = delta * DEG;

				EphemerisData sv_dh;
				CELEMENTS elem_a, elem_m;
				double h1, h2, i1, i2, u1, u2, gamma, Cg, Tg, alpha1, alpha2, beta, theta_R, n, dt, t_R;
				int i = 0;
				dt = 1.0;
				t_R = man->GMT_BO;

				sv_dh.RBI = man->RefBodyInd;

				while (i<10 && abs(dt)>0.01)
				{
					elem_a = OrbMech::GIMIKC(R_a, V_a, mu);
					elem_m = OrbMech::GIMIKC(R_m, V_m, mu);

					h1 = elem_m.h;
					h2 = elem_a.h;
					i1 = elem_m.i;
					i2 = elem_a.i;
					u1 = OrbMech::MeanToTrueAnomaly(elem_m.l, elem_m.e) + elem_m.g;
					if (u1 > PI2)
					{
						u1 -= PI2;
					}
					u2 = OrbMech::MeanToTrueAnomaly(elem_a.l, elem_a.e) + elem_a.g;
					if (u2 > PI2)
					{
						u2 -= PI2;
					}

					gamma = (h2 - h1) / 2.0;
					Cg = cos(gamma);
					Tg = sin(gamma) / Cg;
					alpha2 = (i2 + i1) / 2.0;
					alpha1 = alpha2 - i1;
					beta = 2.0*atan(cos(alpha2) / cos(alpha1)*Tg);
					theta_R = u1 - u2 + beta;
					if (theta_R > PI)
					{
						theta_R -= PI2;
					}
					if (theta_R < -PI)
					{
						theta_R += PI2;
					}
					if (i == 0)
					{
						res.PHASE = theta_R * DEG;
					}

					n = OrbMech::GetMeanMotion(R_a, V_a, mu);
					dt = theta_R / n;

					sv_dh.R = R_a;
					sv_dh.V = V_a;
					sv_dh.GMT = t_R;

					sv_dh = coast(sv_dh, dt);
					R_a = sv_dh.R;
					V_a = sv_dh.V;
					t_R = sv_dh.GMT;

					i++;
				}
				res.DH = (length(R_a) - length(R_m)) / 1852.0;
			}
		}
	}

	if (ManNo == 1)
	{
		res.WT = table->TotalInitMass / 0.45359237;
		res.WC = table->CommonBlock.CSMMass / 0.45359237;
		res.WL = (table->CommonBlock.LMAscentMass + table->CommonBlock.LMDescentMass) / 0.45359237;

		switch (man->Thruster)
		{
		case RTCC_ENGINETYPE_CSMSPS:
			res.WF = table->CommonBlock.SPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMDPS:
			res.WF = table->CommonBlock.LMDPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMAPS:
			res.WF = table->CommonBlock.LMAPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_SIVB_MAIN:
			res.WF = table->CommonBlock.SIVBFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_CSMRCSMINUS2:
		case RTCC_ENGINETYPE_CSMRCSMINUS4:
		case RTCC_ENGINETYPE_CSMRCSPLUS2:
		case RTCC_ENGINETYPE_CSMRCSPLUS4:
			res.WF = table->CommonBlock.CSMRCSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMRCSMINUS2:
		case RTCC_ENGINETYPE_LMRCSMINUS4:
		case RTCC_ENGINETYPE_LMRCSPLUS2:
		case RTCC_ENGINETYPE_LMRCSPLUS4:
			res.WF = table->CommonBlock.LMRCSFuelRemaining / 0.45359237;
			break;
		default:
			res.WF = 0.0;
			break;
		}
	}
	else
	{
		res.WT = table->mantable[ManNo - 2].TotalMassAfter / 0.45359237;
		res.WC = table->mantable[ManNo - 2].CommonBlock.CSMMass / 0.45359237;
		res.WL = (table->mantable[ManNo - 2].CommonBlock.LMAscentMass + table->mantable[ManNo - 2].CommonBlock.LMDescentMass) / 0.45359237;
		switch (man->Thruster)
		{
		case RTCC_ENGINETYPE_CSMSPS:
			res.WF = table->mantable[ManNo - 2].CommonBlock.SPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMDPS:
			res.WF = table->mantable[ManNo - 2].CommonBlock.LMDPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMAPS:
			res.WF = table->mantable[ManNo - 2].CommonBlock.LMAPSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_SIVB_MAIN:
			res.WF = table->mantable[ManNo - 2].CommonBlock.SIVBFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_CSMRCSMINUS2:
		case RTCC_ENGINETYPE_CSMRCSMINUS4:
		case RTCC_ENGINETYPE_CSMRCSPLUS2:
		case RTCC_ENGINETYPE_CSMRCSPLUS4:
			res.WF = table->mantable[ManNo - 2].CommonBlock.CSMRCSFuelRemaining / 0.45359237;
			break;
		case RTCC_ENGINETYPE_LMRCSMINUS2:
		case RTCC_ENGINETYPE_LMRCSMINUS4:
		case RTCC_ENGINETYPE_LMRCSPLUS2:
		case RTCC_ENGINETYPE_LMRCSPLUS4:
			res.WF = table->mantable[ManNo - 2].CommonBlock.LMRCSFuelRemaining / 0.45359237;
			break;
		default:
			res.WF = 0.0;
			break;
		}
	}
}

void RTCC::PMDLDPP(const LDPPOptions &opt, const LDPPResults &res, LunarDescentPlanningTable &table)
{
	double lat, lng, GMTbase;

	for (int i = 0;i < 4;i++)
	{
		table.DV[i] = length(res.DeltaV_LVLH[i]) / 0.3048;
		table.DVVector[i] = res.DeltaV_LVLH[i] / 0.3048;
		table.GETIG[i] = res.T_M[i];
		table.GETTH[i] = opt.TH[i];
		table.MVR[i] = "";
		table.AC[i] = 0.0;
		table.HPC[i] = 0.0;
		table.LIG[i] = 0.0;
	}

	table.LAT_LLS = opt.Lat_LS*DEG;
	table.LONG_LLS = opt.Lng_LS*DEG;
	table.MODE = opt.MODE;

	GMTbase = floor(opt.GETbase);
	table.GMTV = OrbMech::GETfromMJD(opt.sv0.MJD, GMTbase);
	table.GETV = OrbMech::GETfromMJD(opt.sv0.MJD, opt.GETbase);

	if (opt.MODE == 1)
	{
		if (opt.IDO == -1)
		{
			table.MVR[0] = "PC";
			table.MVR[1] = "DOI";
		}
		else if (opt.IDO == 1)
		{
			table.MVR[0] = "ASP";
			table.MVR[1] = "CIA";
			table.MVR[2] = "DOI";
		}
		else
		{
			table.MVR[0] = "PCC";
			table.MVR[1] = "DOI";
		}
	}
	if (opt.MODE == 2)
	{
		if (opt.IDO == 0)
		{
			table.MVR[0] = "ASH";
			table.MVR[1] = "DOI";
		}
		else
		{
			table.MVR[0] = "CIR";
			table.MVR[1] = "DOI";
		}
	}
	else if (opt.MODE == 3)
	{
		table.MVR[0] = "ASH";
		table.MVR[1] = "CIA";
		table.MVR[2] = "DOI";
	}
	else if (opt.MODE == 4)
	{
		table.MVR[0] = "DOI";
	}
	else if (opt.MODE == 5)
	{
		if (opt.IDO == -1)
		{
			table.MVR[0] = "PC";
			table.MVR[1] = "HO1";
			table.MVR[2] = "HO2";
		}
		else if (opt.IDO == 0)
		{
			table.MVR[0] = "HO1";
			table.MVR[1] = "PC";
			table.MVR[2] = "HO2";
		}
		else
		{
			table.MVR[0] = "HO1";
			table.MVR[1] = "HO2";
			table.MVR[2] = "PC";
		}
		table.MVR[3] = "DOI";
	}
	else if (opt.MODE == 7)
	{
		table.MVR[0] = "PPC";
	}

	SV sv_ig, sv_a, sv_p;
	sv_ig = opt.sv0;

	for (int i = 0;i < res.i;i++)
	{
		sv_ig = coast(sv_ig, res.T_M[i] - OrbMech::GETfromMJD(sv_ig.MJD, opt.GETbase));
		OrbMech::latlong_from_J2000(sv_ig.R, sv_ig.MJD, sv_ig.gravref, lat, lng);
		table.LIG[i] = lng * DEG;
		sv_ig.V += tmul(OrbMech::LVLH_Matrix(sv_ig.R, sv_ig.V), res.DeltaV_LVLH[i]);
		PMMAPD(sv_ig, sv_a, sv_p);
		table.AC[i] = (length(sv_a.R) - opt.R_LS) / 1852.0;
		table.HPC[i] = (length(sv_p.R) - opt.R_LS) / 1852.0;
	}

	table.PD_GETTH = opt.T_PD;
	table.PD_GETIG = res.t_PDI;
	table.PD_GETTD = res.t_Land;

	if (opt.I_AZ == 0)
	{
		sprintf(table.DescAzMode, "OPT");
	}
	else
	{
		sprintf(table.DescAzMode, "DES");
	}
	table.DescAsc = res.azi*DEG;
	table.PD_ThetaIgn = opt.theta_D*DEG;
}

void RTCC::RLMPYR(VECTOR3 X_P, VECTOR3 Y_P, VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll)
{
	double dot;
	dot = dotp(X_B, Y_P);
	if (abs(dot) > 1.0 - 0.0017)
	{
		Roll = 0.0;
		Pitch = atan2(dotp(Z_B, X_P),dotp(Z_B, Z_P));
	}
	else
	{
		Roll = atan2(dotp(-Z_B, Y_P), dotp(Y_B, Z_P));
		Pitch = atan2(dotp(-X_B, Z_P), dotp(X_B, X_P));
	}
	Yaw = asin(dot);
	if (Roll < 0)
	{
		Roll += PI2;
	}
	if (Pitch < 0)
	{
		Pitch += PI2;
	}
	if (Yaw < 0)
	{
		Yaw += PI2;
	}
}

void RTCC::EMGLMRAT(VECTOR3 X_P, VECTOR3 Y_P, VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll, double &PB, double &YB, double &RB)
{
	double dot, sin_YB;
	dot = dotp(X_B, Y_P);
	if (abs(dot) > 1.0 - 0.0017)
	{
		Yaw = 0.0;
		Pitch = atan2(dotp(Z_B, X_P), dotp(Z_B, Z_P));
	}
	else
	{
		Yaw = atan2(dotp(-Z_B, Y_P), dotp(Y_B, Y_P));
		Pitch = atan2(dotp(-X_B, Z_P), dotp(X_B, X_P));
	}
	Roll = asin(dot);
	PB = atan2(sin(Yaw)*sin(Roll)*cos(Pitch) + cos(Yaw)*sin(Pitch), -sin(Yaw)*sin(Roll)*sin(Pitch) + cos(Yaw)*cos(Pitch));
	sin_YB = -sin(Yaw)*cos(Roll);
	YB = asin(sin_YB);
	RB = atan2(tan(Roll), cos(Yaw));

	if (Roll < 0)
	{
		Roll += PI2;
	}
	if (Pitch < 0)
	{
		Pitch += PI2;
	}
	if (Yaw < 0)
	{
		Yaw += PI2;
	}
	if (RB < 0)
	{
		RB += PI2;
	}
	if (PB < 0)
	{
		PB += PI2;
	}
	if (YB < 0)
	{
		YB += PI2;
	}
}

VECTOR3 RTCC::GLMRTV(VECTOR3 A, double THET1, int K1, double THET2, int K2, double THET3, int K3)
{
	MATRIX3 Rot;
	VECTOR3 temp;
	double cos_thet, sin_thet;
	int Karr[3] = { K1,K2,K3 };
	double THETarr[3] = {THET1, THET2, THET3};
	int i = 0;
	temp = A;
RTCC_GLMRTV_1:
	if (Karr[i] == 0)
	{
		return temp;
	}
	cos_thet = cos(THETarr[i]);
	sin_thet = sin(THETarr[i]);
	if (Karr[i] == 1)
	{
		Rot = _M(1, 0, 0, 0, cos_thet, sin_thet, 0, -sin_thet, cos_thet);
	}
	else if (Karr[i] == 2)
	{
		Rot = _M(cos_thet, 0, -sin_thet, 0, 1, 0, sin_thet, 0, cos_thet);
	}
	else
	{
		Rot = _M(cos_thet, sin_thet, 0, -sin_thet, cos_thet, 0, 0, 0, 1);
	}
	temp = mul(Rot, temp);
	i++;
	if (i < 3)
	{
		goto RTCC_GLMRTV_1;
	}
	return temp;
}

MATRIX3 RTCC::GLMRTM(MATRIX3 A, double THET1, int K1, double THET2, int K2, double THET3, int K3)
{
	MATRIX3 Rot, temp;
	double cos_thet, sin_thet;
	int Karr[3] = { K1,K2,K3 };
	double THETarr[3] = { THET1, THET2, THET3 };
	int i = 0;
	temp = A;
RTCC_GLMRTM_1:
	if (Karr[i] == 0)
	{
		return temp;
	}
	cos_thet = cos(THETarr[i]);
	sin_thet = sin(THETarr[i]);
	if (Karr[i] == 1)
	{
		Rot = _M(1, 0, 0, 0, cos_thet, sin_thet, 0, -sin_thet, cos_thet);
	}
	else if (Karr[i] == 2)
	{
		Rot = _M(cos_thet, 0, -sin_thet, 0, 1, 0, sin_thet, 0, cos_thet);
	}
	else
	{
		Rot = _M(cos_thet, sin_thet, 0, -sin_thet, cos_thet, 0, 0, 0, 1);
	}
	temp = mul(Rot, temp);
	i++;
	if (i < 3)
	{
		goto RTCC_GLMRTM_1;
	}
	return temp;
}

double RTCC::RLMTLC(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, double long_des, double GMT_min, double &GMT_cross, LunarStayTimesTable *LUNRSTAY)
{
	if (ephemeris.Header.TUP <= 0) return -1.0;
	if (ephemeris.table.size() == 0) return -1.0;

	ELVCTRInputTable interin;
	ELVCTROutputTable interout;
	EphemerisData sv_cur;
	double T1, lat, lng, dlng1, T2, dlng2, Tx, dlngx;
	int RBI_search;
	unsigned i = 0;

	if (GMT_min > ephemeris.table.front().GMT)
	{
		if (GMT_min > ephemeris.table.back().GMT)
		{
			return -1.0;
		}
		T1 = GMT_min;
	}
	else
	{
		T1 = ephemeris.table.front().GMT;
	}

	while (T1 > ephemeris.table[i + 1].GMT)
	{
		i++;
	}

	interin.GMT = T1;
	ELVCTR(interin, interout, ephemeris, MANTIMES, LUNRSTAY);

	if (interout.ErrorCode)
	{
		return -1.0;
	}

	RBI_search = interout.SV.RBI;
	sv_cur = interout.SV;
	OrbMech::latlong_from_J2000(sv_cur.R, OrbMech::MJDfromGET(sv_cur.GMT, GMTBASE), sv_cur.RBI, lat, lng);
	dlng1 = lng - long_des;
	if (dlng1 > PI) { dlng1 -= PI2; }
	else if (dlng1 < -PI) { dlng1 += PI2; }

	if (abs(dlng1) < 0.0001)
	{
		GMT_cross = T1;
		return 0;
	}

	if (i >= ephemeris.table.size() - 1)
	{
		return -1.0;
	}

	while (i < ephemeris.table.size() - 1)
	{
		i++;
		sv_cur = ephemeris.table[i];
		//Reference body inconsistency, abort
		if (sv_cur.RBI != RBI_search)
		{
			return -1.0;
		}
		T2 = sv_cur.GMT;

		OrbMech::latlong_from_J2000(sv_cur.R, OrbMech::MJDfromGET(sv_cur.GMT, GMTBASE), sv_cur.RBI, lat, lng);
		dlng2 = lng - long_des;
		if (dlng2 > PI) { dlng2 -= PI2; }
		else if (dlng2 < -PI) { dlng2 += PI2; }

		if (abs(dlng2) < 0.0001)
		{
			GMT_cross = T2;
			return 0;
		}

		if (dlng1*dlng2 < 0 && abs(dlng1 - dlng2) < PI)
		{
			break;
		}
		if (i == ephemeris.table.size() - 1)
		{
			return -1.0;
		}
		T1 = T2;
		dlng1 = dlng2;
	}

	int iter = 0;

	do
	{
		Tx = T1 - (T2 - T1) / (dlng2 - dlng1)*dlng1;

		interin.GMT = Tx;
		ELVCTR(interin, interout, ephemeris, MANTIMES, LUNRSTAY);

		if (interout.ErrorCode)
		{
			return -1.0;
		}
		//Reference body inconsistency, abort
		if (interout.SV.RBI != RBI_search)
		{
			return -1.0;
		}

		sv_cur = interout.SV;
		OrbMech::latlong_from_J2000(sv_cur.R, OrbMech::MJDfromGET(sv_cur.GMT, GMTBASE), sv_cur.RBI, lat, lng);
		dlngx = lng - long_des;
		if (dlngx > PI) { dlngx -= PI2; }
		else if (dlngx < -PI) { dlngx += PI2; }

		if (abs(dlngx) < 0.0001)
		{
			GMT_cross = Tx;
			return 0;
		}
		if (abs(dlng2) > abs(dlng1))
		{
			T2 = Tx;
			dlng2 = dlngx;
		}
		else
		{
			T1 = Tx;
			dlng1 = dlngx;
		}
		iter++;

	} while (iter < 30);

	GMT_cross = Tx;
	return abs(dlngx);
}

void RTCC::ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable &out)
{
	EphemerisDataTable EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	unsigned vec_tot = in.ORER * 2;
	unsigned vec_bef = in.ORER;

	if (ELFECH(in.GMT, vec_tot, vec_bef, in.L, EPHEM, MANTIMES, LUNSTAY))
	{
		out.ErrorCode = 128;
		return;
	}

	ELVCTR(in, out, EPHEM, MANTIMES, &LUNSTAY);
}

void RTCC::ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable &out, EphemerisDataTable &EPH, ManeuverTimesTable &mantimes, LunarStayTimesTable *LUNRSTAY)
{
	//Is order of interpolation correct?
	if (in.ORER == 0 || in.ORER > 8)
	{
		out.ErrorCode = 64;
		return;
	}

	unsigned nvec = 0;
	double TS_stored = 0, TE_stored = 0;
	bool restore = false;
	unsigned ORER = in.ORER;
	int I;
	out.VPI = 0;
	out.ErrorCode = 0;

	if (in.GMT < EPH.table[0].GMT)
	{
		out.ErrorCode = 8;
		return;
	}
	if (in.GMT > EPH.table.back().GMT)
	{
		out.ErrorCode = 16;
		return;
	}
	out.TUP = EPH.Header.TUP;
	if (LUNRSTAY != NULL)
	{
		if (in.GMT >= LUNRSTAY->LunarStayBeginGMT && in.GMT <= LUNRSTAY->LunarStayEndGMT)
		{
			out.VPI = -1;
		}
	}
	I = 0;
	if (mantimes.Table.size() > 0)
	{
		double TS = EPH.Header.TL;
		double TE = EPH.Header.TR;
		unsigned J = 0;
		for (J = 0;J < mantimes.Table.size();J++)
		{
			//Equal to maneuver initiate time, go directly to 5A
			if (mantimes.Table[J].ManData[1] == in.GMT)
			{
				goto RTCC_ELVCTR_5A;
			}
			else if (mantimes.Table[J].ManData[1] > in.GMT)
			{
				//Equal to maneuver end time, go directly to 5A
				if (mantimes.Table[J].ManData[0] == in.GMT)
				{
					goto RTCC_ELVCTR_5A;
				}
				//Inside burn
				if (mantimes.Table[J].ManData[0] < in.GMT)
				{
					out.VPI = 1;
					goto RTCC_ELVCTR_3;
				}
				if (J == 0)
				{
					//Maneuver outside ephemeris range, let ELVARY handle the error
					if (mantimes.Table[J].ManData[0] > TE)
					{
						goto RTCC_ELVCTR_5A;
					}
					//Constrain ephemeris end to begin of first maneuver
					TE = mantimes.Table[J].ManData[0];
					goto RTCC_ELVCTR_H;
				}
				else
				{
					if (mantimes.Table[J].ManData[0] < TE)
					{
						TE = mantimes.Table[J].ManData[0];
					}
					break;
				}
			}
		}
		//Constrain ephemeris start to end of previous maneuver
		if (mantimes.Table[J - 1].ManData[1] > TS)
		{
			TS = mantimes.Table[J - 1].ManData[1];
		}
		goto RTCC_ELVCTR_H;
	RTCC_ELVCTR_3:
		ORER = 1;
		unsigned E = 0;
		while (EPH.table[E].GMT <= in.GMT)
		{
			//Direct hit
			if (EPH.table[E].GMT == in.GMT)
			{
				goto RTCC_ELVCTR_5A;
			}
			E++;
		}
		TE = EPH.table[E].GMT;
		TS = EPH.table[E - 1].GMT;
	RTCC_ELVCTR_H:
		unsigned V = 0;
		while (TS>=EPH.table[V].GMT)
		{
			if (TS == EPH.table[V].GMT)
			{
				goto RTCC_ELVCTR_4;
			}
			V++;
		}
		out.ErrorCode = 255;
		return;
	RTCC_ELVCTR_4:
		//Save stuff
		restore = true;
		nvec = EPH.Header.NumVec;
		TS_stored = EPH.Header.TL;
		TE_stored = EPH.Header.TR;
		unsigned NV = 1;
		while (TE != EPH.table[NV - 1].GMT)
		{
			if (TE < EPH.table[NV - 1].GMT)
			{
				out.ErrorCode = 255;
				return;
			}
			NV++;
		}
		EPH.Header.NumVec = NV - V;
		EPH.Header.Offset = V;
		EPH.Header.TL = TS;
		EPH.Header.TR = TE;
	}

RTCC_ELVCTR_5A:
	out.ErrorCode = ELVARY(EPH, ORER, in.GMT, false, out.SV, out.ORER);
	if (restore)
	{
		EPH.Header.NumVec = nvec;
		EPH.Header.Offset = 0;
		EPH.Header.TL = TS_stored;
		EPH.Header.TR = TE_stored;
	}
	return;
}

int RTCC::RMMEACC(int L, int ref_frame, int ephem_type, int rev0)
{
	int i, rev, rev_max = 24;
	double lng_des, GMT_min, GMT_cross, GMT_cur;

	OrbitEphemerisTable *ephemeris;
	CapeCrossingTable *cctab;

	if (ephem_type == 0)
	{
		if (L == RTCC_MPT_CSM)
		{
			ephemeris = &EZEPH1;
			cctab = &EZCCSM;
		}
		else
		{
			ephemeris = &EZEPH2;
			cctab = &EZCLEM;
		}
	}
	else
	{
		//TBD
		return 1;
	}

	GMT_cur = RTCCPresentTimeGMT();

	//Find current rev
	for (i = 0;i < cctab->NumRev;i++)
	{
		if (GMT_cur < cctab->GMTCross[i])
		{
			rev0 = i + 1;
			break;
		}
	}

	for (i = 0;i < 30;i++)
	{
		cctab->GMTCross[i] = 0.0;
	}

	if (ephemeris->EPHEM.table.front().RBI == BODY_EARTH)
	{
		lng_des = -80.0*RAD;
	}
	else
	{
		lng_des = -180.0*RAD;
	}

	cctab->TUP = ephemeris->EPHEM.Header.TUP;
	cctab->GMTEphemFirst = ephemeris->EPHEM.table.front().GMT;
	cctab->GMTEphemLast = ephemeris->EPHEM.table.back().GMT;

	GMT_min = ephemeris->EPHEM.table.front().GMT;

	for (rev = 0;rev < rev_max;rev++)
	{
		if (RLMTLC(ephemeris->EPHEM, ephemeris->MANTIMES, lng_des, GMT_min, GMT_cross))
		{
			break;
		}

		cctab->GMTCross[rev] = GMT_cross;

		GMT_min = GMT_cross + 3600.0;
	}

	cctab->NumRevFirst = rev0 + 1;
	cctab->NumRevLast = rev0 + rev;
	cctab->NumRev = rev;

	return 0;
}

int RTCC::RMMASCND(int L, double GMT_min, double &lng_asc)
{
	double GMT;
	OrbitEphemerisTable *tab;
	if (L == 1)
	{
		tab = &EZEPH1;
	}
	else
	{
		tab = &EZEPH2;
	}

	ELVCTRInputTable intab;
	ELVCTROutputTable outtab;
	EphemerisData sv_true;

	GMT = GMT_min;
	intab.GMT = GMT;
	intab.L = L;
	ELVCTR(intab, outtab);
	if (outtab.ErrorCode)
	{
		return 1;
	}

	double mu, u, du, a, n, dt;
	int in, out;
	OELEMENTS coe;

	if (outtab.SV.RBI == BODY_EARTH)
	{
		in = 0;
		out = 1;
		mu = OrbMech::mu_Earth;
	}
	else
	{
		in = 2;
		out = 3;
		mu = OrbMech::mu_Moon;
	}

	if (ELVCNV(outtab.SV, in, out, sv_true))
	{
		return 2;
	}
	coe = OrbMech::coe_from_sv(sv_true.R, sv_true.V, mu);
	if (coe.e >= 1.0)
	{
		return 4;
	}
	u = coe.TA + coe.w;
	if (u > PI2)
	{
		u = u - PI2;
	}
	du = PI2 - u;
	a = OrbMech::GetSemiMajorAxis(sv_true.R, sv_true.V, mu);
	n = sqrt(mu / pow(a, 3));
	dt = du / n;
	GMT += dt;

	int i, imax;
	i = 0;
	imax = 10;

	while (abs(dt) >= 2.0 && i < imax)
	{
		intab.GMT = GMT;
		ELVCTR(intab, outtab);
		if (outtab.ErrorCode)
		{
			return 1;
		}
		if (ELVCNV(outtab.SV, in, out, sv_true))
		{
			return 2;
		}
		coe = OrbMech::coe_from_sv(sv_true.R, sv_true.V, mu);
		if (coe.e >= 1.0)
		{
			return 4;
		}
		u = coe.TA + coe.w;
		if (u >= PI2)
		{
			u = u - PI2;
		}
		if (abs(u) > PI)
		{
			if (u > 0)
			{
				du = u - PI2;
			}
			else
			{
				du = u + PI2;
			}
		}
		else
		{
			du = u;
		}
		n = coe.h / pow(length(sv_true.R), 2);
		dt = -du / n;
		GMT += dt;
		i++;
	}

	if (i >= imax)
	{
		lng_asc = u;
		return 3;
	}

	double lat;
	OrbMech::latlong_from_r(sv_true.R, lat, lng_asc);
	return 0;
}

int RTCC::EMMENV(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, double GMT_begin, bool sun, SunriseSunsetTable &table)
{
	ELVCTRInputTable interin;
	ELVCTROutputTable interout;
	SunriseSunsetData data;
	EphemerisData sv_cur;
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");
	VECTOR3 R_EM, V_EM, R_ES, R_MS, s;
	double R_E, cos_theta, cos_theta_old, GMT_old, GMT;
	unsigned iter = 0, iter_start;
	bool sight, sight_old, rise;
	int dataset = 0;
	int counter = 0;
	int iter2 = 0;

	if (EZEPH1.EPHEM.table.size() == 0) return 1;

	if (sun == false && ephemeris.table.front().RBI != BODY_EARTH)
	{
		return 1;
	}

	while (ephemeris.table[iter].GMT < GMT_begin)
	{
		iter++;
	}

	iter_start = iter;
	table.num = 0;

	//Find environment change
	while (iter < ephemeris.table.size())
	{
		sv_cur = ephemeris.table[iter];
		OrbMech::PLEFEM(pzefem, OrbMech::MJDfromGET(sv_cur.GMT, GMTBASE), R_EM, V_EM, R_ES);

		if (sun)
		{
			if (sv_cur.RBI == BODY_EARTH)
			{
				R_MS = R_ES;
				R_E = OrbMech::R_Earth;
			}
			else
			{
				R_MS = R_ES - R_EM;
				R_E = OrbMech::R_Moon;
			}
		}
		else
		{
			R_MS = R_EM;
			R_E = OrbMech::R_Earth;
		}

		sight = OrbMech::sight(sv_cur.R, R_MS, R_E);
		s = unit(R_MS);

		cos_theta = length(sv_cur.R - s * dotp(sv_cur.R, s)) - R_E;
		if (iter == iter_start)
		{
			sight_old = sight;
		}
		if (sight != sight_old)
		{
			if (sight)
			{
				//sunrise
				rise = true;
			}
			else
			{
				//sunset
				rise = false;
			}

			//Environment change found

			iter2 = 0;

			while (abs(sv_cur.GMT - GMT_old)*24.0*3600.0 > 1.5)
			{
				GMT = sv_cur.GMT - cos_theta * (sv_cur.GMT - GMT_old) / (cos_theta - cos_theta_old);
				cos_theta_old = cos_theta;
				GMT_old = sv_cur.GMT;

				interin.GMT = GMT;
				ELVCTR(interin, interout, ephemeris, MANTIMES);
				if (interout.ErrorCode)
				{
					return 4;
				}
				sv_cur = interout.SV;

				cos_theta = length(sv_cur.R - s * dotp(sv_cur.R, s)) - R_E;
				iter2++;
				if (iter2 > 20)
				{
					return 2;
				}
			}

			if (rise)
			{
				data.GETSR = GETfromGMT(GMT);
			}
			else
			{
				data.GETSS = GETfromGMT(GMT);
			}

			VECTOR3 R_scal = unit(sv_cur.R)*R_E;
			cos_theta_old = length(R_scal - s * dotp(R_scal, s)) - R_E;
			GMT_old = sv_cur.GMT;
			GMT = GMT_old + 10.0;

			interin.GMT = GMT;
			ELVCTR(interin, interout, ephemeris, MANTIMES);
			if (interout.ErrorCode)
			{
				return 4;
			}
			sv_cur = interout.SV;

			R_scal = unit(sv_cur.R)*R_E;
			cos_theta = length(R_scal - s * dotp(R_scal, s)) - R_E;
			iter2 = 0;

			while (abs(sv_cur.GMT - GMT_old) > 1.5)
			{
				GMT = sv_cur.GMT - cos_theta * (sv_cur.GMT - GMT_old) / (cos_theta - cos_theta_old);
				cos_theta_old = cos_theta;
				GMT_old = sv_cur.GMT;

				interin.GMT = GMT;
				ELVCTR(interin, interout, ephemeris, MANTIMES);
				if (interout.ErrorCode)
				{
					return 4;
				}
				sv_cur = interout.SV;
				R_scal = unit(sv_cur.R)*R_E;
				cos_theta = length(R_scal - s * dotp(R_scal, s)) - R_E;
				iter2++;
				if (iter2 > 20)
				{
					return 2;
				}
			}

			if (rise)
			{
				data.GETTR = GETfromGMT(GMT);
			}
			else
			{
				data.GETTS = GETfromGMT(GMT);
			}
			dataset++;
			if (dataset >= 2)
			{
				dataset = 0;
				table.data[counter] = data;
				counter++;
			}
			if (counter >= 8)
			{
				break;
			}
		}

		sight_old = sight;
		cos_theta_old = cos_theta;
		GMT_old = sv_cur.GMT;
		iter++;
	}

	table.num = counter;

	return 0;
}

void RTCC::EMDSSEMD(int ind, double param)
{
	double GMT_begin, get, Pitch, Yaw;

	if (ind == 1)
	{
		GMT_begin = GMTfromGET(param);
	}
	else
	{
		int rev = (int)param;
		if (EZCCSM.NumRevFirst < rev || rev > EZCCSM.NumRevLast)
		{
			return;
		}
		GMT_begin = CapeCrossingGMT(1, rev);
		if (GMT_begin < RTCCPresentTimeGMT())
		{
			GMT_begin = RTCCPresentTimeGMT();
		}
	}

	for (int i = 0;i < 8;i++)
	{
		EZSSTAB.data[i].REV = 0;
		EZSSTAB.data[i].GETTR = 0.0;
		EZSSTAB.data[i].GETSR = 0.0;
		EZSSTAB.data[i].theta_SR = 0.0;
		EZSSTAB.data[i].psi_SR = 0.0;
		EZSSTAB.data[i].GETTS = 0.0;
		EZSSTAB.data[i].GETSS = 0.0;
		EZSSTAB.data[i].theta_SS = 0.0;
		EZSSTAB.data[i].psi_SS = 0.0;
	}

	if (EMMENV(EZEPH1.EPHEM, EZEPH1.MANTIMES, GMT_begin, true, EZSSTAB))
	{
		return;
	}

	if (EZSSTAB.num == 0) return;

	for (int i = 0;i < EZSSTAB.num;i++)
	{
		if (EZSSTAB.data[i].GETSR > EZSSTAB.data[i].GETSS)
		{
			get = EZSSTAB.data[i].GETSS;
		}
		else
		{
			get = EZSSTAB.data[i].GETSR;
		}

		EZSSTAB.data[i].REV = CapeCrossingRev(1, GMTfromGET(get));
		ECMPAY(EZEPH1.EPHEM, EZEPH1.MANTIMES, GMTfromGET(EZSSTAB.data[i].GETSR), true, Pitch, Yaw);
		EZSSTAB.data[i].theta_SR = Pitch * DEG;
		EZSSTAB.data[i].psi_SR = Yaw * DEG;
		ECMPAY(EZEPH1.EPHEM, EZEPH1.MANTIMES, GMTfromGET(EZSSTAB.data[i].GETSS), true, Pitch, Yaw);
		EZSSTAB.data[i].theta_SS = Pitch * DEG;
		EZSSTAB.data[i].psi_SS = Yaw * DEG;
	}
}

void RTCC::EMDSSMMD(int ind, double param)
{
	double GMT_begin, get, Pitch, Yaw;

	if (ind == 1)
	{
		GMT_begin = GMTfromGET(param);
	}
	else
	{
		int rev = (int)param;
		if (EZCCSM.NumRevFirst < rev || rev > EZCCSM.NumRevLast)
		{
			return;
		}
		GMT_begin = CapeCrossingGMT(1, rev);
		if (GMT_begin < RTCCPresentTimeGMT())
		{
			GMT_begin = RTCCPresentTimeGMT();
		}
	}

	for (int i = 0;i < 8;i++)
	{
		EZMMTAB.data[i].REV = 0;
		EZMMTAB.data[i].GETTR = 0.0;
		EZMMTAB.data[i].GETSR = 0.0;
		EZMMTAB.data[i].theta_SR = 0.0;
		EZMMTAB.data[i].psi_SR = 0.0;
		EZMMTAB.data[i].GETTS = 0.0;
		EZMMTAB.data[i].GETSS = 0.0;
		EZMMTAB.data[i].theta_SS = 0.0;
		EZMMTAB.data[i].psi_SS = 0.0;
	}

	if (EMMENV(EZEPH1.EPHEM, EZEPH1.MANTIMES, GMT_begin, false, EZMMTAB))
	{
		return;
	}

	if (EZMMTAB.num == 0) return;

	for (int i = 0;i < EZMMTAB.num;i++)
	{
		if (EZMMTAB.data[i].GETSR > EZMMTAB.data[i].GETSS)
		{
			get = EZMMTAB.data[i].GETSS;
		}
		else
		{
			get = EZMMTAB.data[i].GETSR;
		}

		EZMMTAB.data[i].REV = CapeCrossingRev(1, GMTfromGET(get));
		ECMPAY(EZEPH1.EPHEM, EZEPH1.MANTIMES, GMTfromGET(EZMMTAB.data[i].GETSR), false, Pitch, Yaw);
		EZMMTAB.data[i].theta_SR = Pitch * DEG;
		EZMMTAB.data[i].psi_SR = Yaw * DEG;
		ECMPAY(EZEPH1.EPHEM, EZEPH1.MANTIMES, GMTfromGET(EZMMTAB.data[i].GETSS), false, Pitch, Yaw);
		EZMMTAB.data[i].theta_SS = Pitch * DEG;
		EZMMTAB.data[i].psi_SS = Yaw * DEG;
	}
}

int RTCC::CapeCrossingRev(int L, double GMT)
{
	CapeCrossingTable *table;
	if (L == RTCC_MPT_LM)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	if (table->NumRev == 0)
	{
		return 1;
	}

	if (GMT < table->GMTCross[0])
	{
		return table->NumRevFirst - 1;
	}
	else if (GMT > table->GMTCross[table->NumRev - 1])
	{
		return table->NumRevLast;
	}

	for (int i = 0;i < table->NumRev;i++)
	{
		if (table->GMTCross[i] > GMT)
		{
			return i + table->NumRevFirst - 1;
		}
	}

	return 1;
}

double RTCC::CapeCrossingGMT(int L, int rev)
{
	CapeCrossingTable *table;
	if (L == RTCC_MPT_LM)
	{
		table = &EZCLEM;
	}
	else
	{
		table = &EZCCSM;
	}

	if (table->NumRev == 0)
	{
		return -1.0;
	}

	if (rev < table->NumRevFirst || rev >table->NumRevLast)
	{
		return -1.0;
	}

	return table->GMTCross[rev - table->NumRevFirst];
}

void RTCC::ECMPAY(EphemerisDataTable &EPH, ManeuverTimesTable &MANTIMES, double GMT, bool sun, double &Pitch, double &Yaw)
{
	ELVCTRInputTable interin;
	ELVCTROutputTable interout;
	EphemerisData sv_out;
	VECTOR3 R_EM, V_EM, R_ES, R_VS, X_B, X_L, Y_L, Z_L;

	interin.GMT = GMT;
	ELVCTR(interin, interout, EPH, MANTIMES);
	if (interout.ErrorCode)
	{
		return;
	}
	sv_out = interout.SV;
	OrbMech::PLEFEM(pzefem, OrbMech::MJDfromGET(GMT, GMTBASE), R_EM, V_EM, R_ES);
	if (sun)
	{
		if (sv_out.RBI == BODY_EARTH)
		{
			R_VS = R_ES - sv_out.R;
		}
		else
		{
			R_VS = (R_ES - R_EM) - sv_out.R;
		}
	}
	else
	{
		if (sv_out.RBI == BODY_EARTH)
		{
			R_VS = R_EM - sv_out.R;
		}
		else
		{
			R_VS = -sv_out.R;
		}
	}

	X_B = unit(R_VS);
	Z_L = unit(-sv_out.R);
	Y_L = unit(crossp(sv_out.V, sv_out.R));
	X_L = crossp(Y_L, Z_L);

	Pitch = asin(dotp(-X_B, Z_L));
	Yaw = atan2(dotp(X_B, Y_L), dotp(X_B, X_L));
}

int RTCC::EMMXTR(double vel, double fpa, double azi, double lat, double lng, double h, VECTOR3 &R, VECTOR3 &V)
{
	double r = h + OrbMech::R_Earth;

	R = _V(cos(lat)*cos(lng), cos(lat)*sin(lng), sin(lat))*r;
	V = mul(_M(cos(lat)*cos(lng), -sin(lng), -sin(lat)*cos(lng), cos(lat)*sin(lng), cos(lng), -sin(lat)*sin(lng), sin(lat), 0, cos(lat)), _V(sin(fpa), cos(fpa)*sin(azi), cos(fpa)*cos(azi))*vel);

	return 0;
}

int RTCC::NewMPTTrajectory(int L, SV &sv0)
{
	if (MPTHasManeuvers(L))
	{
		MissionPlanTable *tab;
		if (L == RTCC_MPT_CSM)
		{
			tab = &PZMPTCSM;
		}
		else
		{
			tab = &PZMPTLEM;
		}

		sv0.R = tab->mantable.back().R_BO;
		sv0.V = tab->mantable.back().V_BO;
		sv0.MJD = OrbMech::MJDfromGET(tab->mantable.back().GMT_BO, GMTBASE);
		if (tab->mantable.back().RefBodyInd == BODY_EARTH)
		{
			sv0.gravref = oapiGetObjectByName("Earth");
		}
		else
		{
			sv0.gravref = oapiGetObjectByName("Moon");
		}
		sv0.mass = tab->mantable.back().TotalMassAfter;
	}
	else
	{
		//SV at specified time
		double GMT = RTCCPresentTimeGMT();
		EphemerisData sv;
		if (ELFECH(GMT, L, sv))
		{
			return 1;
		}

		sv0.R = sv.R;
		sv0.V = sv.V;
		sv0.MJD = OrbMech::MJDfromGET(sv.GMT, GMTBASE);
		sv0.gravref = GetGravref(sv.RBI);

		if (PLAWDT(L, GMT, sv0.mass))
		{
			return 2;
		}
	}

	return 0;
}

int RTCC::ELNMVC(double TL, double TR, int L, unsigned &NumVec, int &TUP)
{
	int err;
	OrbitEphemerisTable *tab;
	if (L == 1)
	{
		tab = &EZEPH1;
	}
	else
	{
		tab = &EZEPH2;
	}

	if (tab->EPHEM.table.size() == 0)
	{
		err = 32;
		goto RTCC_ELNMVC_2B;
	}
	TUP = abs(tab->EPHEM.Header.TUP);
	if (TUP == 0)
	{
		err = 64;
		goto RTCC_ELNMVC_2B;
	}

	tab->EPHEM.Header.TUP = -tab->EPHEM.Header.TUP;

	if (TL < tab->EPHEM.Header.TL)
	{
		if (TR < tab->EPHEM.Header.TL)
		{
			err = 128;
			goto RTCC_ELNMVC_2B;
		}
		TL = tab->EPHEM.Header.TL;
		err = 8;
	}
	if (TL > tab->EPHEM.Header.TR)
	{
		err = 128;
		goto RTCC_ELNMVC_2B;
	}
	double TREQ, T_Mid;
	unsigned LB, UB, Mid;
	bool firstpass = true;
	LB = 0;
	UB = tab->EPHEM.table.size() - 1;
	TREQ = TL;
RTCC_ELNMVC_1B:
	if (UB - LB <= 1)
	{
		goto RTCC_ELNMVC_1D;
	}
	Mid = (LB + UB) / 2;
	T_Mid = tab->EPHEM.table[Mid].GMT;
	if (TREQ < T_Mid)
	{
		UB = Mid;
		goto RTCC_ELNMVC_1B;
	}
	if (TREQ > T_Mid)
	{
		LB = Mid;
		goto RTCC_ELNMVC_1B;
	}
	LB = Mid;
	if (firstpass)
	{
		LB++;
	}
	goto RTCC_ELNMVC_2A;
RTCC_ELNMVC_1D:
	LB++;
RTCC_ELNMVC_2A:
	if (firstpass)
	{
		NumVec = LB;
		LB = NumVec;
		UB = tab->EPHEM.table.size() - 1;
		TREQ = TR;
		firstpass = false;
		goto RTCC_ELNMVC_1B;
	}
	NumVec = LB - NumVec + 2;
RTCC_ELNMVC_2B:
	tab->EPHEM.Header.TUP = -tab->EPHEM.Header.TUP;
	return err;
}

int RTCC::ELFECH(double GMT, int L, EphemerisData &SV)
{
	EphemerisDataTable EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;
	int err = ELFECH(GMT, 1, 1, L, EPHEM, MANTIMES, LUNSTAY);
	if (err == 0)
	{
		SV = EPHEM.table[0];
	}
	return err;
}

int RTCC::ELFECH(double GMT, unsigned vec_tot, unsigned vec_bef, int L, EphemerisDataTable &EPHEM, ManeuverTimesTable &MANTIMES, LunarStayTimesTable &LUNSTAY)
{
	OrbitEphemerisTable *maintable;
	unsigned LO, HI, temp;
	double GMTtemp;

	if (L == RTCC_MPT_CSM)
	{
		maintable = &EZEPH1;
	}
	else
	{
		maintable = &EZEPH2;
	}

	if (maintable->EPHEM.table.size() == 0)
	{
		return 1;
	}

	MANTIMES = maintable->MANTIMES;
	LUNSTAY = maintable->LUNRSTAY;

	//GMT is before first SV in ephemeris, error
	if (GMT < maintable->EPHEM.table.front().GMT)
	{
		return 1;
	}
	//GMT is beyond last SV in ephemeris, error return if more than one SV is requested. Otherwise the last SV will be returned
	if (vec_tot > 1 && GMT > maintable->EPHEM.table.back().GMT)
	{
		return 1;
	}

	//Clear output ephemeris table
	EPHEM.table.clear();

	//Special code for data beyond the ephemeris range, only is used for 1 SV to be returned
	if (GMT > maintable->EPHEM.table.back().GMT)
	{
		EPHEM.table.push_back(maintable->EPHEM.table.back());
	}
	//All other cases
	else
	{
		LO = 0;
		HI = maintable->EPHEM.table.size() - 1;

		while (HI - LO > 1)
		{
			temp = (LO + HI) / 2;
			GMTtemp = maintable->EPHEM.table[temp].GMT;
			if (GMT > GMTtemp)
			{
				LO = temp;
			}
			else
			{
				HI = temp;
			}
		}

		if (maintable->EPHEM.table[LO].GMT < GMT)
		{
			LO++;
		}

		if (LO < vec_bef)
		{
			LO = 0;
		}
		else
		{
			LO = LO - vec_bef;
		}

		HI = LO + vec_tot;

		if (HI >= maintable->EPHEM.table.size())
		{
			HI = maintable->EPHEM.table.size() - 1;
		}

		auto first = maintable->EPHEM.table.cbegin() + LO;
		auto last = maintable->EPHEM.table.cbegin() + HI;

		EPHEM.table.assign(first, last);
	}

	MANTIMES = maintable->MANTIMES;
	EPHEM.Header.TUP = maintable->EPHEM.Header.TUP;
	EPHEM.Header.NumVec = EPHEM.table.size();
	EPHEM.Header.Offset = 0;
	EPHEM.Header.TL = EPHEM.table.front().GMT;
	EPHEM.Header.TR = EPHEM.table.back().GMT;

	return 0;
}

int RTCC::PMQREAP(const std::vector<TradeoffData> &TOdata)
{
	TradeoffData empty;
	unsigned i, j;

	//Clear tradeoff data
	RTETradeoffTable.curves = 0;
	for (i = 0;i < 10;i++)
	{
		RTETradeoffTable.NumInCurve[i] = 0;
		for (j = 0;j < 44;j++)
		{
			RTETradeoffTable.data[i][j] = empty;
		}
	}

	if (TOdata.size() == 0) return 1;

	double TZData[10];
	bool found;

	for (i = 0;i < TOdata.size();i++)
	{
		//Check if the data belongs to an already existing curve
		found = false;
		for (j = 0;j < RTETradeoffTable.curves;j++)
		{
			//Is data within an hour of already existing curve?
			if (abs(TZData[j] - TOdata[i].T_Z) < 1.0)
			{
				//Save for a closer estimate next time around
				TZData[j] = TOdata[i].T_Z;

				if (RTETradeoffTable.NumInCurve[j] < 44)
				{
					//Store data
					RTETradeoffTable.data[j][RTETradeoffTable.NumInCurve[j]] = TOdata[i];
					RTETradeoffTable.NumInCurve[j]++;
				}
				found = true;
				break;
			}
		}
		//Didn't find one, it's a new set of data
		if (found == false && RTETradeoffTable.curves < 10)
		{
			TZData[RTETradeoffTable.curves] = TOdata[i].T_Z;
			RTETradeoffTable.NumInCurve[RTETradeoffTable.curves] = 1;

			//Store data
			RTETradeoffTable.data[RTETradeoffTable.curves][0] = TOdata[i];

			RTETradeoffTable.curves++;
		}
	}

	return 0;
}

void RTCC::PMMREAP(int med)
{
	//Near Earth Tradeoff 
	if (med == 70 || med == 71)
	{
		EphemerisData sv0;

		if (ELFECH(GMTfromGET(PZREAP.RTEVectorTime*3600.0), RTCC_MPT_CSM, sv0))
		{
			return;
		}

		EphemerisDataTable EPHEM;

		double dt = PZREAP.RTET0Min*3600.0 - GETfromGMT(sv0.GMT);
		sv0 = coast(sv0, dt);

		double mu;

		if (sv0.RBI == BODY_EARTH)
		{
			mu = OrbMech::mu_Earth;
		}
		else
		{
			mu = OrbMech::mu_Moon;
		}

		//Calculate maximum time difference
		double T_P = OrbMech::period(sv0.R, sv0.V, mu);
		dt = (PZREAP.RTET0Max - PZREAP.RTET0Min)*3600.0;
		if (T_P < dt)
		{
			dt = T_P;
		}
		if (dt > 24.0*3600.0)
		{
			dt = 24.0*3600.0;
		}

		EMSMISSInputTable in;

		in.AnchorVector = sv0;
		in.CutoffIndicator = 3;
		in.EarthRelStopParam = 400000.0*0.3048;
		in.EphemerisBuildIndicator = true;
		in.EphemerisLeftLimitGMT = sv0.GMT;
		in.EphemerRightLimitGMT = sv0.GMT + dt;
		in.EphemTableIndicator = &EPHEM;
		in.ManCutoffIndicator = 2;
		in.ManeuverIndicator = false;
		in.MoonRelStopParam = 0.0;
		in.StopParamRefFrame = 2;
		in.VehicleCode = RTCC_MPT_CSM;

		EMSMISS(in);

		int mode;
		if (PZREAP.RTEIsPTPSite)
		{
			mode = 2;
		}
		else
		{
			mode = 4;
		}

		std::vector<TradeoffData>todata;

		ConicRTEEarthNew rteproc(EPHEM.table, pzefem);
		rteproc.READ(mode, GMTBASE, PZREAP.TZMIN, PZREAP.TZMAX);
		rteproc.Init(PZREAP.DVMAX, PZREAP.EntryProfile, PZREAP.IRMAX, PZREAP.VRMAX, PZREAP.RRBIAS, PZREAP.TGTLN);

		std::vector<ATPData> line;
		ATPData data;

		double lat;

		if (mode == 4)
		{
			for (int i = 0;i < 5;i++)
			{
				lat = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i * 2];

				if (lat > 1e8)
				{
					break;
				}

				data.lat = lat;
				data.lng = PZREAP.ATPCoordinates[PZREAP.RTESiteNum][i * 2 + 1];

				line.push_back(data);
			}
		}

		if (line.size() == 0)
		{
			return;
		}

		rteproc.ATP(line);
		rteproc.MAIN();
		todata = rteproc.TOData;
		PMQREAP(todata);
		PMDTRDFF(med, PZREAP.RTETradeoffRemotePage);
	}
	//Generation of Abort Scan Table for unspecified area	
	else if (med == 75 || med == 76 || med == 77)
	{
		PMMREAST();
	}
}

void RTCC::PMMREAST()
{
	//TBD: Check if any AST slots are available

	//Convert MED, fetch state vector, propagate to TIG
	EphemerisData sv0;
	double GMTV, GMT0, dt;
	GMTV = GMTfromGET(med_f75.T_V);
	GMT0 = GMTfromGET(med_f75.T_0);
	if (ELFECH(GMTV, RTCC_MPT_CSM, sv0))
	{
		return;
	}
	dt = GMT0 - sv0.GMT;
	PMMCEN_INI ini;
	PMMCEN_VNI vni;
	int ITS;
	EphemerisData sv_abort;
	ini.body = sv0.RBI;
	ini.stop_ind = 1;
	vni.dir = 1.0;
	vni.dt_max = dt;
	vni.dt_min = 0.0;
	vni.GMTBASE = GetGMTBase();
	vni.R = sv0.R;
	vni.V = sv0.V;
	vni.T = sv0.GMT;
	OrbMech::PMMCEN(vni, ini, sv_abort.R, sv_abort.V, sv_abort.GMT, ITS, sv_abort.RBI);

	//Conic solutions
	VECTOR3 DV;
	double T_r;
	if (sv_abort.RBI == BODY_EARTH)
	{
		std::vector<EphemerisData> SVArray;
		SVArray.push_back(sv_abort);
		ConicRTEEarthNew rteproc(SVArray, pzefem);
		rteproc.READ(med_f75.Type, GMTBASE, PZREAP.TZMIN, PZREAP.TZMAX);
		rteproc.Init(med_f75.DVMAX, med_f75.EntryProfile, PZREAP.IRMAX, PZREAP.VRMAX, PZREAP.RRBIAS, PZREAP.TGTLN);
		rteproc.MAIN();

		DV = rteproc.SolData.DV;
		T_r = rteproc.SolData.T_r;
	}
	else
	{

	}

	double r, v1, v2, lat, lng, gamma1, gamma2, azi1, azi2;
	PICSSC(true, sv_abort.R, sv_abort.V, r, v1, lat, lng, gamma1, azi1);
	PICSSC(true, sv_abort.R, sv_abort.V + DV, r, v2, lat, lng, gamma2, azi2);

	ASTInput ARIN;
	ARIN.dgamma = gamma2 - gamma1;
	ARIN.dpsi = azi2 - azi1;
	ARIN.dv = v2 - v1;
	ARIN.T_a = sv_abort.GMT;
	ARIN.T_r = T_r;
	ARIN.Area = 0.0;

	//ASTSettings IRIN;
	//ASTData AST;

	//int IER, IPRT;
	//PMMDAB(sv_abort.R, sv_abort.V, sv_abort.GMT, ARIN, IRIN, AST, IER, IPRT);
}

void RTCC::PMMLRBTI(EphemerisData sv)
{
	EMSMISSInputTable in;
	in.StopParamRefFrame = 1;
	in.MoonRelStopParam = 0.0;
	in.CutoffIndicator = 4;

	EphemerisData sv2 = EMMENI(in, sv, 5.0*24.0*3600.0);

	if (in.NIAuxOutputTable.TerminationCode != 4)
	{
		PMXSPT("PMMLRBTI", 124);
		return;
	}

	rtcc::LOIOptions opt;
	opt.SPH = sv2;
	opt.dh_bias = med_k40.dh_bias*1852.0;
	opt.DV_maxm = med_k18.DVMAXm*0.3048;
	opt.DV_maxp = med_k18.DVMAXp*0.3048;
	opt.DW = med_k40.DW*RAD;
	opt.eta1 = med_k40.eta_1*RAD;
	opt.GMTBASE = GMTBASE;
	opt.HA_LLS = med_k40.HA_LLS*1852.0;
	opt.HP_LLS = med_k40.HP_LLS*1852.0;
	opt.HA_LPO = med_k18.HALOI1*1852.0;
	opt.HP_LPO = med_k18.HPLOI1*1852.0;
	opt.lat_LLS= BZLSDISP.lat[0];
	opt.lng_LLS = BZLSDISP.lng[0];
	opt.psi_DS = med_k18.psi_DS*RAD;
	opt.psi_mn = med_k18.psi_MN*RAD;
	opt.psi_mx = med_k18.psi_MX*RAD;
	opt.REVS1 = med_k40.REVS1;
	opt.REVS2 = med_k40.REVS2;
	opt.R_LLS = MCSMLR;
	opt.usePlaneSolnForInterSoln = med_k40.PlaneSolnForInterSoln;

	double h_pc = length(opt.SPH.R) - opt.R_LLS;
	if (h_pc > opt.HA_LPO)
	{
		PMXSPT("PMMLRBTI", 126);
		return;
	}
	if (h_pc < 0.0)
	{
		PMXSPT("PMMLRBTI", 125);
		return;
	}

	rtcc::LOITargeting loi(opt);
	loi.MAIN();

	//Write message with time at LLS
	char Buffer[100], Buffer2[100];
	OrbMech::format_time_HHMMSS(Buffer, GETfromGMT(loi.out.T_LLS));
	sprintf(Buffer2, "Time above the landing site is %s", Buffer);
	RTCCONLINEMON.TextBuffer[0].assign(Buffer2);
	PMXSPT("PMMLRBTI", 201);

	PMDLRBTI(opt, loi.out);

	for (int i = 0;i < 8;i++)
	{
		PZLRBELM.sv_man_bef[i].R = loi.out.data[i].R_LOI;
		PZLRBELM.sv_man_bef[i].V = loi.out.data[i].V_LOI;
		PZLRBELM.sv_man_bef[i].GMT = loi.out.data[i].GMT_LOI;
		PZLRBELM.sv_man_bef[i].RBI = BODY_MOON;
		PZLRBELM.V_man_after[i] = loi.out.data[i].V_LOI_apo;
	}
}

void RTCC::PMDLRBTI(const rtcc::LOIOptions &opt, const rtcc::LOIOutputData &out)
{
	PZLRBTI.VectorGET = med_k18.VectorTime;
	PZLRBTI.lat_lls = BZLSDISP.lat[0] * DEG;
	PZLRBTI.lng_lls = BZLSDISP.lng[0] * DEG;
	PZLRBTI.R_lls = MCSMLR / 1852.0;
	PZLRBTI.REVS1 = opt.REVS1;
	PZLRBTI.REVS2 = opt.REVS2;
	PZLRBTI.DHBIAS = opt.dh_bias / 1852.0;
	PZLRBTI.AZ_LLS = opt.psi_DS*DEG;
	PZLRBTI.f_LLS = opt.DW*DEG;
	if (PZLRBTI.f_LLS < 0)
	{
		PZLRBTI.f_LLS += 360.0;
	}
	PZLRBTI.HALOI1 = opt.HA_LPO / 1852.0;
	PZLRBTI.HPLOI1 = opt.HP_LPO / 1852.0;
	PZLRBTI.HALOI2 = opt.HA_LLS / 1852.0;
	PZLRBTI.HPLOI2 = opt.HP_LLS / 1852.0;
	PZLRBTI.DVMAXp = opt.DV_maxp / 0.3048;
	PZLRBTI.DVMAXm = opt.DV_maxm / 0.3048;
	PZLRBTI.planesoln = opt.usePlaneSolnForInterSoln;
	PZLRBTI.RARPGT = opt.RARPGT / 1852.0;
	PZLRBTI.AZMN_f_ND = out.eta_MN*DEG;
	if (PZLRBTI.AZMN_f_ND < 0)
	{
		PZLRBTI.AZMN_f_ND += 360.0;
	}
	PZLRBTI.AZMX_f_ND = out.eta_MX*DEG;
	if (PZLRBTI.AZMX_f_ND < 0)
	{
		PZLRBTI.AZMX_f_ND += 360.0;
	}
	for (int i = 0;i < 8;i++)
	{
		if (out.data[i].GMT_LOI == 0.0)
		{
			PZLRBTI.sol[i].GETLOI = 0.0;
			PZLRBTI.sol[i].DVLOI1 = 0.0;
			PZLRBTI.sol[i].DVLOI2 = 0.0;
			PZLRBTI.sol[i].H_ND = 0.0;
			PZLRBTI.sol[i].f_ND_H = 0.0;
			PZLRBTI.sol[i].H_PC = 0.0;
			PZLRBTI.sol[i].Theta = 0.0;
			PZLRBTI.sol[i].f_ND_E = 0.0;
		}
		else
		{
			PZLRBTI.sol[i].GETLOI = GETfromGMT(out.data[i].GMT_LOI);
			PZLRBTI.sol[i].DVLOI1 = length(out.data[i].V_LOI_apo - out.data[i].V_LOI) / 0.3048;
			PZLRBTI.sol[i].DVLOI2 = out.data[i].display.dv_LOI2 / 0.3048;
			PZLRBTI.sol[i].H_ND = out.data[i].display.H_ND / 1852.0;
			PZLRBTI.sol[i].f_ND_H = out.data[i].display.eta_N*DEG;
			if (PZLRBTI.sol[i].f_ND_H < 0)
			{
				PZLRBTI.sol[i].f_ND_H += 360.0;
			}
			PZLRBTI.sol[i].H_PC = out.data[i].display.h_P / 1852.0;
			PZLRBTI.sol[i].Theta = out.data[i].display.theta*DEG;
			PZLRBTI.sol[i].f_ND_E = out.data[i].display.W_P*DEG;
			if (PZLRBTI.sol[i].f_ND_E < 0)
			{
				PZLRBTI.sol[i].f_ND_E += 360.0;
			}
		}
	}
}

bool RTCC::GMGMED(char *str)
{
	int i = 0;

	if (str[i] == '\0')
	{
		return false;
	}

	char medtype;
	medtype = str[i];
	i++;

	std::string code;

	do
	{
		code.push_back(str[i]);
		i++;
	} while (str[i] != ',');

	if (code.size() == 0)
	{
		return false;
	}

	i++;

	std::string word;
	std::vector<std::string> MEDSequence;

	do
	{
		if (str[i] == ',')
		{
			MEDSequence.push_back(word);
			word.clear();
		}
		else
		{
			word.push_back(str[i]);
		}
		i++;
		if (str[i] == '\0' || str[i] == ';')
		{
			MEDSequence.push_back(word);
		}
	} while (str[i] != '\0' && str[i] != ';');

	RTCCONLINEMON.TextBuffer[0].assign(str);
	GMSPRINT("GMGMED", 51);

	int err;
	if (medtype == 'A')
	{
		err = EMGABMED(1, code, MEDSequence);
	}
	else if (medtype == 'B')
	{
		err = EMGABMED(2, code, MEDSequence);
	}
	else if (medtype == 'C')
	{
		err = CMRMEDIN(code, MEDSequence);
	}
	else if (medtype == 'G')
	{
		err = EMGABMED(3, code, MEDSequence);
	}
	else if (medtype == 'F')
	{
		err = PMQAFMED(code, MEDSequence);
	}
	else if (medtype == 'M')
	{
		err = PMMMED(code, MEDSequence);
	}
	else if (medtype == 'P')
	{
		err = GMSMED(code, MEDSequence);
	}
	else if (medtype == 'U')
	{
		err = EMGTVMED(code, MEDSequence);
	}
	else if (medtype == 'X')
	{
		err = GMSREMED(code, MEDSequence);
	}
	
	char Buffer[128];
	if (err == 0)
	{
		sprintf_s(Buffer, "%c%s OK", medtype, code.c_str());
		RTCCONLINEMON.TextBuffer[0].assign(Buffer);
		GMSPRINT("GMGMED", 51);
	}
	else
	{
		sprintf_s(Buffer, "%c%s ERR %d", medtype, code.c_str(), err);
		RTCCONLINEMON.TextBuffer[0].assign(Buffer);
		GMSPRINT("GMGMED", 51);
	}

	return true;
}

int RTCC::EMGABMED(int type, std::string med, std::vector<std::string> data)
{
	//A MEDs
	if (type == 1)
	{

	}
	//B MEDs
	else if (type == 2)
	{
		//Generate Station Contacts
		if (med == "03")
		{
			if (data.size() < 1)
			{
				return 1;
			}
			int veh;
			if (data[0] == "CSM")
			{
				veh = 1;
			}
			else if (data[0] == "LEM")
			{
				veh = 3;
			}
			else
			{
				return 1;
			}
			EMSTAGEN(veh);
		}
		//Suppress/unsuppress C-band station contacts
		else if (med == "04")
		{
			if (data.size() != 1)
			{
				return 1;
			}
			if (data[0] == "START")
			{
				MGRTAG = 0;
				RTCCONLINEMON.TextBuffer[0] = "SUPPRESSED";
			}
			else if (data[0] == "STOP")
			{
				MGRTAG = 1;
				RTCCONLINEMON.TextBuffer[0] = "UNSUPPRESSED";
			}
			else
			{
				return 2;
			}
			EMGPRINT("EMGABMED", 35);
		}
	}
	//G MEDs
	else if (type == 3)
	{
		//CSM/LEM REFSMMAT locker movement
		if (med == "00")
		{
			if (data.size() < 4)
			{
				return 1;
			}
			int L1, ID1, L2, ID2;
			if (data[0] == "CSM")
			{
				L1 = 1;
			}
			else if (data[0] == "LEM")
			{
				L1 = 3;
			}
			else
			{
				return 1;
			}
			ID1 = EMGSTGENCode(data[1].c_str());
			if (ID1 < 0)
			{
				return 1;
			}
			if (data[2] == "CSM")
			{
				L2 = 1;
			}
			else if (data[2] == "LEM")
			{
				L2 = 3;
			}
			else
			{
				return 1;
			}
			ID2 = EMGSTGENCode(data[3].c_str());
			if (ID2 < 0)
			{
				return 1;
			}
			double gmt, hh, mm, ss;
			if (data.size() < 5 || data[4] == "")
			{
				gmt = RTCCPresentTimeGMT();
			}
			else if (sscanf(data[4].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
			{
				gmt = GMTfromGET(hh * 3600.0 + mm * 60.0 + ss);
			}
			else
			{
				return 1;
			}
			EMGSTGEN(2, L1, ID1, L2, ID2, gmt);
		}
		//COMPUTE AND SAVE LOCAL VERTICAL CSM/LM PLATFORM ALIGNMENT
		else if (med == "03")
		{
			if (data.size() < 2)
			{
				return 1;
			}
			int L;
			if (data[0] == "CSM")
			{
				L = 1;
			}
			else if (data[0] == "LEM")
			{
				L = 3;
			}
			else
			{
				return 1;
			}
			double gmt, hh, mm, ss;
			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
			{
				gmt = GMTfromGET(hh * 3600.0 + mm * 60.0 + ss);
			}
			//TBD: Earth or Moon
			EMMGLCVP(L, gmt);
		}
		//Generate Guidance Optics Support Table
		else if (med == "10")
		{
			if (data.size() < 4)
			{
				return 1;
			}
			if (data[0] != "CSM")
			{
				return 2;
			}
			
			double get;
			if (data[1] == "")
			{
				get = 0.0;
			}
			else if (MEDTimeInputHHMMSS(data[1], get))
			{
				return 2;
			}
			unsigned star;
			if (data[2] == "")
			{
				star = 1;
			}
			else
			{
				if (sscanf(data[2].c_str(), "%o", &star) != 1)
				{
					return 2;
				}
			}
			int mtx1, mtx2, mtx3;
			if (data[3] == "")
			{
				mtx1 = 0;
			}
			else
			{
				mtx1 = EMGSTGENCode(data[3].c_str());
				if (mtx1 <= 0)
				{
					return 2;
				}
			}
			if (data.size() < 5 || data[4] == "")
			{
				mtx2 = 0;
			}
			else
			{
				mtx2 = EMGSTGENCode(data[4].c_str());
				if (mtx2 <= 0)
				{
					return 2;
				}
			}
			if (data.size() < 6 || data[5] == "")
			{
				mtx3 = 0;
			}
			else
			{
				mtx3 = EMGSTGENCode(data[5].c_str());
				if (mtx3 <= 0)
				{
					return 2;
				}
			}
			EZGSTMED.GMT = GMTfromGET(get*3600.0);
			EZGSTMED.StartingStar = star;
			EZGSTMED.MTX1 = mtx1;
			EZGSTMED.MTX2 = mtx2;
			EZGSTMED.MTX3 = mtx3;
			EMMGSTMP();
		}
		//Acquire and save CSM IMU matrix/optics
		else if (med == "11")
		{
			if (data.size() < 2)
			{
				return 1;
			}
			if (data[0] != "CSM")
			{
				return 2;
			}
			if (data[1] == "DMT")
			{
				if (data.size() < 5)
				{
					return 1;
				}
				unsigned man;
				if (sscanf(data[2].c_str(), "%d", &man) != 1)
				{
					return 2;
				}
				if (man < 1 || man > 15)
				{
					return 2;
				}
				int refs;

				if (data[3] == "DES")
				{
					refs = 100;
				}
				else
				{
					refs = EMGSTGENCode(data[3].c_str());
					if (refs < 0 || refs > 10)
					{
						return 2;
					}
				}
				bool headsup;
				if (data[4] == "U")
				{
					headsup = true;
				}
				else if (data[4] == "D")
				{
					headsup = false;
				}
				else
				{
					return 2;
				}
				EMSGSUPP(1, 5, refs, man, headsup);
			}
		}
		//Enter CSM sextant optics and IMU attitudes
		else if (med == "12")
		{
			if (data.size() < 1)
			{
				return 1;
			}
			if (data[0] != "CSM")
			{
				return 2;
			}
			unsigned star, i, j;
			double shaft, trun;
			for (i = 0;i < 2;i++)
			{
				if (data.size() <= i * 3 + 1)
				{
					EMDGSUPP(0);
					return 0;
				}
				if (data[i * 3 + 1] != "")
				{
					if (sscanf(data[i * 3 + 1].c_str(), "%o", &star) == 1)
					{
						EZJGSTTB.SXT_STAR[i] = star;
					}
				}
				if (data.size() <= i * 3 + 2)
				{
					EMDGSUPP(0);
					return 0;
				}
				if (data[i * 3 + 2] != "")
				{
					if (sscanf(data[i * 3 + 2].c_str(), "%lf", &shaft) == 1)
					{
						shaft *= RAD;
						if (shaft >= 0.0 && shaft <= PI2)
						{
							EZJGSTTB.SXT_SFT_INP[i] = shaft;
						}
						else
						{
							return 2;
						}
					}
				}
				if (data.size() <= i * 3 + 3)
				{
					EMDGSUPP(0);
					return 0;
				}
				if (data[i * 3 + 3] != "")
				{
					if (sscanf(data[i * 3 + 3].c_str(), "%lf", &trun) == 1)
					{
						trun *= RAD;
						if (trun >= 0.0 && trun <= PI2)
						{
							EZJGSTTB.SXT_TRN_INP[i] = trun;
						}
						else
						{
							return 2;
						}
					}
				}
			}
			double att;
			for (i = 0;i < 2;i++)
			{
				for (j = 0;j < 3;j++)
				{
					if (data.size() <= i * 3 + j + 7)
					{
						EMDGSUPP(0);
						return 0;
					}
					if (data[i * 3 + j + 7] != "")
					{
						if (sscanf(data[i * 3 + j + 7].c_str(), "%lf", &att) == 1)
						{
							att *= RAD;
							if (att >= 0.0 && att <= PI2)
							{
								EZJGSTTB.Att[i].data[j] = att;
							}
							else
							{
								return 2;
							}
						}
					}
				}
			}
			EMDGSUPP(0);
		}
		//Initialize MSK 229 display of special targets
		else if (med == "14")
		{
			if (data.size() < 2)
			{
				return 1;
			}
			int veh;
			if (data[0] == "CSM")
			{
				veh = 1;
			}
			else if (data[0] == "LEM")
			{
				veh = 3;
			}
			else
			{
				return 2;
			}
			unsigned star;
			if (data[1] == "")
			{
				star = 0;
			}
			else
			{
				if (sscanf(data[1].c_str(), "%o", &star) != 1)
				{
					return 2;
				}
				if (star < 1 || star > EZJGSTAR.size())
				{
					return 2;
				}
			}
			int rb = 0;
			double lat = 0.0, lng = 0.0, height = 0.0, GMT = 0.0;
			if (star == 0)
			{
				if (data.size() < 4)
				{
					return 2;
				}
				double hours;
				if (MEDTimeInputHHMMSS(data[2], hours))
				{
					return 2;
				}
				GMT = GMTfromGET(hours*3600.0);
				if (data[3] == "S")
				{
					rb = 1;
				}
				else if (data[3] == "M")
				{
					rb = 2;
				}
				else if (data[3] == "E")
				{
					rb = 3;
				}
				else
				{
					return 2;
				}
				if (rb != 1)
				{
					if (data.size() < 7)
					{
						return 2;
					}
					if (sscanf(data[4].c_str(), "%lf", &lat) != 1)
					{
						return 2;
					}
					lat *= RAD;
					if (lat < -PI05 || lat > PI05)
					{
						return 2;
					}
					if (sscanf(data[5].c_str(), "%lf", &lng) != 1)
					{
						return 2;
					}
					lng *= RAD;
					if (lng < 0 || lng > PI2)
					{
						return 2;
					}
					if (sscanf(data[6].c_str(), "%lf", &height) != 1)
					{
						return 2;
					}
					height /= 0.3048;
				}
			}

			EZGSTMED.G14_Star = star;
			EZGSTMED.G14_Vehicle = veh;
			EZGSTMED.G14_height = height;
			EZGSTMED.G14_lat = lat;
			EZGSTMED.G14_lng = lng;
			EZGSTMED.G14_RB = rb;
			EZGSTMED.G14_GMT = GMT;
			EZGSTMED.MTX1 = 0;
			EZGSTMED.MTX2 = 0;
			EZGSTMED.MTX3 = 0;

			EMMGSTMP();
		}
		//Acquire and save LEM IMU matrix/optics
		else if (med == "21")
		{
			if (data.size() < 2)
			{
				return 1;
			}
			if (data[0] != "LEM")
			{
				return 2;
			}
			if (data[1] == "LLD")
			{
				EMSLSUPP(1, 15);
			}
			else if (data[1] == "DMT")
			{
				if (data.size() < 5)
				{
					return 1;
				}
				unsigned man;
				if (sscanf(data[2].c_str(), "%d", &man) != 1)
				{
					return 2;
				}
				if (man < 1 || man > 15)
				{
					return 2;
				}
				int refs;
				
				if (data[3] == "DES")
				{
					refs = 100;
				}
				else
				{
					refs = EMGSTGENCode(data[3].c_str());
					if (refs < 0 || refs > 10)
					{
						return 2;
					}
				}
				bool headsup;
				if (data[4] == "U")
				{
					headsup = true;
				}
				else if (data[4] == "D")
				{
					headsup = false;
				}
				else
				{
					return 2;
				}
				EMSLSUPP(1, 5, refs, man, headsup);
			}
		}
	}
	
	
	return 0;
}

int RTCC::CMRMEDIN(std::string med, std::vector<std::string> data)
{
	//Initiate a CMC/LGC external delta-V update
	if (med == "10")
	{
		if (data.size() != 3)
		{
			return 1;
		}
		int VehicleType;
		if (data[0] == "CMC")
		{
			VehicleType = 1;
		}
		else if (data[0] == "LGC")
		{
			VehicleType = 2;
		}
		else
		{
			return 2;
		}
		unsigned ManeuverNum;
		if (sscanf(data[1].c_str(), "%d", &ManeuverNum) != 1)
		{
			return 2;
		}
		if (ManeuverNum < 1 || ManeuverNum > 15)
		{
			return 2;
		}
		int L;
		if (data[2] == "CSM")
		{
			L = 1;
		}
		else if (data[2] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}
		MissionPlanTable *tab = GetMPTPointer(L);
		if (tab->mantable.size() < ManeuverNum)
		{
			return 2;
		}
		//TBD: This should use CMC liftoff time, not RTCC
		double TIG = GETfromGMT(tab->mantable[ManeuverNum - 1].GMTI);
		VECTOR3 DV = tab->mantable[ManeuverNum - 1].dV_LVLH;
		if (VehicleType == 1)
		{
			CMMAXTDV(TIG, DV);
		}
		else
		{
			CMMLXTDV(TIG, DV);
		}
	}
	//Initiate a CMC/LGC REFSMMAT update
	else if (med == "12")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int Veh;
		if (data[0] == "CMC")
		{
			Veh = 1;
		}
		else if (data[0] == "LGC")
		{
			Veh = 3;
		}
		else
		{
			return 2;
		}
		int id = EMGSTGENCode(data[1].c_str());
		if (id < 0)
		{
			return 2;
		}
		int type;
		if (data[2] == "1")
		{
			type = 1;
		}
		else if (data[2] == "2")
		{
			type = 2;
		}
		else
		{
			return 2;
		}
		CMMRFMAT(Veh, id, type);
	}

	return 0;
}

int RTCC::PMQAFMED(std::string med)
{
	std::vector<std::string> data;
	return PMQAFMED(med, data);
}

int RTCC::PMQAFMED(std::string med, std::vector<std::string> data)
{
	//Initialize Azimuth Constraints for Midcourse Correction Planning
	if (med == "22")
	{
		double azmin, azmax;
		if (data.size() < 1 || data[0]== "")
		{
			azmin = -110.0*RAD;
		}
		else
		{
			if (sscanf(data[0].c_str(), "%lf", &azmin) != 1)
			{
				return 1;
			}
			if (azmin < -110.0)
			{
				return 1;
			}
			azmin *= RAD;
		}
		PZMCCPLN.AZ_min = azmin;
		if (data.size() < 2 || data[1] == "")
		{
			azmax = -70.0*RAD;
		}
		else
		{
			if (sscanf(data[1].c_str(), "%lf", &azmax) != 1)
			{
				return 1;
			}
			if (azmax > -70.0)
			{
				return 1;
			}
			azmax *= RAD;
		}
		PZMCCPLN.AZ_max = azmax;
	}
	//Initialization of translunar time (minimum and maximum) for midcourse correction planning
	else if (med == "23")
	{
		double hh, mm, ss, get;
		if (data.size() < 1 || data[0] == "")
		{
			PZMCCPLN.TLMIN = 0.0;
		}
		else
		{
			if (sscanf(data[0].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 1;
			}
			PZMCCPLN.TLMIN = hh + mm / 60.0 + ss / 3600.0;
		}
		if (data.size() < 2 || data[1] == "")
		{
			PZMCCPLN.TLMAX = 0.0;
		}
		else
		{
			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 1;
			}
			get = hh + mm / 60.0 + ss / 3600.0;
			if (get > PZMCCPLN.TLMIN)
			{
				PZMCCPLN.TLMAX = get;
			}
		}
	}
	//Initalize gamma and reentry range for midcourse correction plans
	else if (med == "24")
	{
		if (data.size() < 1 || data[0] == "")
		{
			PZMCCPLN.gamma_reentry = -6.52*RAD;
		}
		else
		{
			double gamma;
			if (sscanf(data[0].c_str(), "%lf", &gamma) != 1)
			{
				return 1;
			}
			PZMCCPLN.gamma_reentry = gamma * RAD;
		}
		if (data.size() < 2 || data[1] == "")
		{
			PZMCCPLN.Reentry_range = -6.52*RAD;
		}
		else
		{
			double range;
			if (sscanf(data[1].c_str(), "%lf", &range) != 1)
			{
				return 1;
			}
			PZMCCPLN.Reentry_range = 1350.0;
		}
	}
	//Delete column of midcourse correction display
	else if (med == "26")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int column;
		if (sscanf(data[0].c_str(), "%d", &column) != 1)
		{
			return 1;
		}
		if (column < 0 || column>4)
		{
			return 1;
		}
		if (column == 0)
		{
			for (int i = 0;i < 4;i++)
			{
				PZMCCDIS.data[i].Mode = 0;
			}
		}
		else
		{
			PZMCCDIS.data[column - 1].Mode = 0;
		}
	}
	//Specify pericynthion height limits for optimized MCC
	else if (med == "29")
	{
	if (data.size() < 1 || data[0] == "")
	{
		PZMCCPLN.H_PCYN_MIN = 40.0*1852.0;
	}
	else
	{
		double ht;
		if (sscanf(data[0].c_str(), "%lf", &ht) != 1)
		{
			return 1;
		}
		PZMCCPLN.H_PCYN_MIN = ht * 1852.0;;
	}
	if (data.size() < 2 || data[1] == "")
	{
		PZMCCPLN.H_PCYN_MAX = 5000.0*1852.0;
	}
	else
	{
		double ht;
		if (sscanf(data[1].c_str(), "%lf", &ht) != 1)
		{
			return 1;
		}
		PZMCCPLN.H_PCYN_MAX = ht * 1852.0;
	}
	}
	//Transfer midcourse correction plan to skeleton flight plan table
	else if (med == "30")
	{
	if (data.size() < 1)
	{
		return 1;
	}
	int column;

	if (sscanf(data[0].c_str(), "%d", &column) != 1)
	{
		return 1;
	}
	if (column < 1 || column > 4)
	{
		return 1;
	}
	if (PZMCCSFP.blocks[column - 1].mode <= 1 || PZMCCSFP.blocks[column - 1].mode >= 6)
	{
		return 1;
	}
	PZSFPTAB.blocks[1] = PZMCCSFP.blocks[column - 1];
	}
	//Alteration of Skeleton Flight Plan Table
	else if (med == "32")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int tab;
		if (sscanf(data[0].c_str(), "%d", &tab) != 1)
		{
			return 1;
		}
		if (tab < 1 || tab > 2)
		{
			return 1;
		}
		int item;
		if (sscanf(data[1].c_str(), "%d", &item) != 1)
		{
			return 1;
		}
		if (item < 1 || item > 26)
		{
			return 1;
		}
		TLMCCDataTable *table = &PZSFPTAB.blocks[tab - 1];
		double hh, mm, ss, val;
		int mode;
		switch (item)
		{
		case 1:
		case 3:
		case 7:
		case 11:
		case 12:
		case 18:
		case 19:
		case 26:
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 1;
			}
			val = hh * 3600.0 + mm * 60.0 + ss;
			switch (item)
			{
			case 1:
				table->GMTTimeFlag = val;
				break;
			case 3:
				table->GMT_pc1 = val;
				break;
			case 7:
				table->GMT_pc2 = val;
				break;
			case 11:
				table->GET_TLI = val;
				break;
			case 12:
				table->GMT_nd = val;
				break;
			case 18:
				table->T_lo = val;
				break;
			case 19:
				table->dt_lls = val;
				break;
			case 26:
				table->T_te = val;
				break;
			}
			break;
		case 2:
			if (sscanf(data[2].c_str(), "%d", &mode) != 1)
			{
				return 1;
			}
			table->mode = mode;
			break;
		case 4:
		case 5:
		case 8:
		case 9:
		case 10:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 20:
		case 21:
		case 22:
		case 23:
		case 24:
		case 25:
			if (sscanf(data[2].c_str(), "%lf", &val) != 1)
			{
				return 1;
			}
			switch (item)
			{
			case 4:
				table->lat_pc1 = val * RAD;
				break;
			case 5:
				table->lng_pc1 = val * RAD;
				break;
			case 6:
				table->h_pc1 = val * 1852.0;
				break;
			case 8:
				table->lat_pc2 = val * RAD;
				break;
			case 9:
				table->lng_pc2 = val * RAD;
				break;
			case 10:
				table->h_pc2 = val * 1852.0;
				break;
			case 13:
				table->lat_nd = val * RAD;
				break;
			case 14:
				table->lng_nd = val * RAD;
				break;
			case 15:
				table->h_nd = val * 1852.0;
				break;
			case 16:
				table->dpsi_loi = val * RAD;
				break;
			case 17:
				table->gamma_loi = val * RAD;
				break;
			case 20:
				table->psi_lls = val * RAD;
				break;
			case 21:
				table->lat_lls = val * RAD;
				break;
			case 22:
				table->lng_lls = val * RAD;
				break;
			case 23:
				table->rad_lls = val * 1852.0;
				break;
			case 24:
				table->dpsi_tei = val * RAD;
				break;
			case 25:
				table->dv_tei = val * 0.3048;
				break;
			}
			break;
		}
	}
	//Generation of near Earth tradeoff
	else if (med == "70")
	{
		bool found = false;
		//Check and find site
		for (int i = 0;i < 5;i++)
		{
			if (med_f70.Site == PZREAP.ATPSite[i])
			{
				PZREAP.RTESite = med_f70.Site;
				PZREAP.RTESiteNum = i;
				PZREAP.RTEIsPTPSite = false;
				found = true;
				break;
			}
			if (med_f70.Site == PZREAP.PTPSite[i])
			{
				PZREAP.RTESite = med_f70.Site;
				PZREAP.RTESiteNum = i;
				PZREAP.RTEIsPTPSite = true;
				found = true;
				break;
			}
		}

		if (found == false)
		{
			return 1;
		}
		//Check vector time
		//TBD: T_V greater than present time
		PZREAP.RTEVectorTime = med_f70.T_V;

		//Check min abort time
		if (med_f70.T_omin < PZREAP.RTEVectorTime)
		{
			PZREAP.RTET0Min = PZREAP.RTEVectorTime;
		}
		else
		{
			PZREAP.RTET0Min = med_f70.T_omin;
		}

		//Check max abort time
		if (med_f70.T_omax == -1.0)
		{
			med_f70.T_omax = med_f70.T_omin + 24.0;
		}
		if (med_f70.T_omax < PZREAP.RTET0Min)
		{
			PZREAP.RTET0Max = PZREAP.RTET0Min;
		}
		else
		{
			PZREAP.RTET0Max = med_f70.T_omax;
		}

		//Check entry profile
		if (med_f70.EntryProfile == 2 && PZREAP.TGTLN == 1)
		{
			return 4;
		}
		PZREAP.EntryProfile = med_f70.EntryProfile;

		PMMREAP(70);
	}
	//Generation of remote Earth tradeoff
	else if (med == "71")
	{
		if (med_f71.Page < 1 || med_f71.Page > 5)
		{
			return 5;
		}
		PZREAP.RTETradeoffRemotePage = med_f71.Page;

		bool found = false;
		//Check and find site
		for (int i = 0;i < 5;i++)
		{
			if (med_f71.Site == PZREAP.ATPSite[i])
			{
				PZREAP.RTESite = med_f71.Site;
				PZREAP.RTESiteNum = i;
				PZREAP.RTEIsPTPSite = false;
				found = true;
				break;
			}
			if (med_f71.Site == PZREAP.PTPSite[i])
			{
				PZREAP.RTESite = med_f71.Site;
				PZREAP.RTESiteNum = i;
				PZREAP.RTEIsPTPSite = true;
				found = true;
				break;
			}
		}

		if (found == false)
		{
			return 1;
		}
		//Check vector time
		//TBD: T_V greater than present time
		PZREAP.RTEVectorTime = med_f71.T_V;

		//Check min abort time
		if (med_f71.T_omin < PZREAP.RTEVectorTime)
		{
			PZREAP.RTET0Min = PZREAP.RTEVectorTime;
		}
		else
		{
			PZREAP.RTET0Min = med_f71.T_omin;
		}

		//Check max abort time
		if (med_f71.T_omax == -1.0)
		{
			med_f71.T_omax = med_f71.T_omin + 24.0;
		}
		if (med_f71.T_omax < PZREAP.RTET0Min)
		{
			PZREAP.RTET0Max = PZREAP.RTET0Min;
		}
		else
		{
			PZREAP.RTET0Max = med_f71.T_omax;
		}

		//Check entry profile
		if (med_f71.EntryProfile == 2 && PZREAP.TGTLN == 1)
		{
			return 4;
		}
		PZREAP.EntryProfile = med_f71.EntryProfile;

		PMMREAP(71);
	}
	//Update the target table for return to Earth
	else if (med == "85")
	{

	}
	//Update return to Earth constraints
	else if (med == "86")
	{
		if (med_f86.Constraint == "DVMAX")
		{
			PZREAP.DVMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "TZMIN")
		{
			PZREAP.TZMIN = med_f86.Value;
		}
		else if (med_f86.Constraint == "TZMAX")
		{
			PZREAP.TZMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "GMAX")
		{
			PZREAP.GMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "HMINMC")
		{
			PZREAP.HMINMC = med_f86.Value;
		}
		else if (med_f86.Constraint == "IRMAX")
		{
			PZREAP.IRMAX = med_f86.Value;
		}
		else if (med_f86.Constraint == "RRBIAS")
		{
			PZREAP.RRBIAS = med_f86.Value;
		}
		else if (med_f86.Constraint == "VRMAX")
		{
			PZREAP.VRMAX = med_f86.Value;
		}
	}
	//Update return to Earth constraints
	else if (med == "87")
	{
		if (med_f87.Constraint == "TGTLN")
		{
			if (med_f87.Value == "SHALLOW")
			{
				PZREAP.TGTLN = 0;
			}
			else if (med_f87.Value == "SHALLOW")
			{
				PZREAP.TGTLN = 1;
			}
		}
		else if (med_f87.Constraint == "MOTION")
		{
			if (med_f87.Value == "EITHER")
			{
				PZREAP.MOTION = 0;
			}
			else if (med_f87.Value == "DIRECT")
			{
				PZREAP.MOTION = 1;
			}
			else if (med_f87.Value == "CIRCUM")
			{
				PZREAP.MOTION = 2;
			}
		}
	}

	return 0;
}

void RTCC::PMKMED(std::string med)
{

}

int RTCC::PMMMED(std::string med, std::vector<std::string> data)
{
	if (med == "40")
	{
		if (data.size() < 1)
		{
			return 1;
		}

		if (data[0] == "")
		{
			return 2;
		}
		if (data[0] == "P1")
		{
			double dv;
			if (data.size() < 2 || data[1] == "")
			{
				dv = -1;
			}
			else if (sscanf(data[1].c_str(), "%lf", &dv) != 1)
			{
				return 2;
			}
			int dvind;
			if (data.size() < 3 || data[2] == "" || data[2] == "MAG")
			{
				dvind = 0;
			}
			else if (data[2] == "DVC")
			{
				dvind = 1;
			}
			else if (data[2] == "XBT")
			{
				dvind = 2;
			}
			else
			{
				return 2;
			}

			double dt;
			if (data.size() < 4 || data[3] == "")
			{
				dt = 0.0;
			}
			else if (sscanf(data[3].c_str(), "%lf", &dt) != 1)
			{
				return 2;
			}
			if (dt < 0)
			{
				return 2;
			}

			PZBURN.P1_DV = dv * 0.3048;
			PZBURN.P1_DVIND = dvind;
			PZBURN.P1_DT = dt;
		}
		else if (data[0] == "P2")
		{
			if (data.size() < 4)
			{
				return 1;
			}

			VECTOR3 dv;
			if (sscanf(data[1].c_str(), "%lf", &dv.x) != 1)
			{
				return 2;
			}
			if (sscanf(data[2].c_str(), "%lf", &dv.y) != 1)
			{
				return 2;
			}
			if (sscanf(data[3].c_str(), "%lf", &dv.z) != 1)
			{
				return 2;
			}
			PZBURN.P2_DV = dv * 0.3048;
		}
		else if (data[0] == "P3")
		{
			VECTOR3 dv;
			if (data[1] == "")
			{
				return 2;
			}
			else if (sscanf(data[1].c_str(), "%lf", &dv.x) != 1)
			{
				return 2;
			}
			if (data[2] == "")
			{
				return 2;
			}
			else if (sscanf(data[2].c_str(), "%lf", &dv.y) != 1)
			{
				return 2;
			}
			if (data[3] == "")
			{
				return 2;
			}
			else if (sscanf(data[3].c_str(), "%lf", &dv.z) != 1)
			{
				return 2;
			}
			PZBURN.P3_DV = dv * 0.3048;
		}
		else if (data[0] == "P4")
		{
			VECTOR3 dv;
			if (data[1] == "")
			{
				return 2;
			}
			else if (sscanf(data[1].c_str(), "%lf", &dv.x) != 1)
			{
				return 2;
			}
			if (data[2] == "")
			{
				return 2;
			}
			else if (sscanf(data[2].c_str(), "%lf", &dv.y) != 1)
			{
				return 2;
			}
			if (data[3] == "")
			{
				return 2;
			}
			else if (sscanf(data[3].c_str(), "%lf", &dv.z) != 1)
			{
				return 2;
			}
			PZBURN.P4_DV = dv * 0.3048;
		}
	}
	//Change fuel remaining for specified thruster
	else if (med == "49")
	{
		if (data.size() < 1)
		{
			return 1;
		}

		if (data[0] == "CSM")
		{
			med_m49.Table = RTCC_MPT_CSM;
		}
		else if (data[0] == "LEM")
		{
			med_m49.Table = RTCC_MPT_LM;
		}
		else
		{
			return 2;
		}

		double temp;
		med_m49.SPSFuelRemaining = -1;
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf_s(data[1].c_str(), "%lf", &temp) == 1)
			{
				if (temp > 0)
				{
					med_m49.SPSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.CSMRCSFuelRemaining = -1;
		if (data.size() > 2 && data[2] != "")
		{
			if (sscanf_s(data[2].c_str(), "%lf", &temp) == 1)
			{
				if (temp > 0)
				{
					med_m49.CSMRCSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.SIVBFuelRemaining = -1;
		if (data.size() > 3 && data[3] != "")
		{
			if (sscanf_s(data[3].c_str(), "%lf", &temp) == 1)
			{
				if (temp > 0)
				{
					med_m49.SIVBFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.LMAPSFuelRemaining = -1;
		if (data.size() > 4 && data[4] != "")
		{
			if (sscanf_s(data[4].c_str(), "%lf", &temp) == 1)
			{
				if (temp > 0)
				{
					med_m49.LMAPSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.LMRCSFuelRemaining = -1;
		if (data.size() > 5 && data[5] != "")
		{
			if (sscanf_s(data[5].c_str(), "%lf", &temp) == 1)
			{
				if (temp > 0)
				{
					med_m49.LMRCSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		med_m49.LMDPSFuelRemaining = -1;
		if (data.size() > 6 && data[6] != "")
		{
			if (sscanf_s(data[6].c_str(), "%lf", &temp) == 1)
			{
				if (temp > 0)
				{
					med_m49.LMDPSFuelRemaining = temp * 0.45359237;
				}
			}
		}
		PMMWTC(49);
	}
	//Change vehicle body orientation and trim angles for MPT maneuver
	else if (med == "58")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		unsigned man;
		if (sscanf(data[1].c_str(), "%d", &man) != 1)
		{
			return 2;
		}
		if (man < 1 || man>15)
		{
			return 2;
		}
		int headsup;
		if (data.size() < 3)
		{
			headsup = -1;
		}
		else if (data[2] == "U")
		{
			headsup = 1;
		}
		else if (data[2] == "D")
		{
			headsup = 0;
		}
		else
		{
			return 2;
		}
		int trim;
		if (data.size() < 4)
		{
			trim = -1;
		}
		else if (data[3] == "S")
		{
			trim = 2;
		}
		else if (data[3] == "C")
		{
			trim = 0;
		}
		else
		{
			return 2;
		}
		PMMUDT(veh, man, headsup, trim);
	}
	else if (med == "62")
	{
		int mpt, man, act;

		if (data.size() < 3)
		{
			return 1;
		}

		if (data[0] == "")
		{
			return 2;
		}
		else if (data[0] == "CSM")
		{
			mpt = 1;
		}
		else if (data[0] == "LEM")
		{
			mpt = 3;
		}
		else
		{
			return 2;
		}

		if (data[1] == "")
		{
			return 2;
		}
		if (sscanf(data[1].c_str(), "%d", &man) != 1)
		{
			return 2;
		}
		if (man < 1 || man > 15)
		{
			return 2;
		}

		if (data[2] == "")
		{
			return 2;
		}
		else if (data[2] == "F")
		{
			act = 2;
		}
		else if (data[2] == "U")
		{
			act = 3;
		}
		else if (data[2] == "D")
		{
			act = 0;
		}
		else if (data[2] == "DH")
		{
			act = 1;
		}
		else
		{
			return 2;
		}

		PMMFUD(mpt, man, act);
	}
	//Transfer a GPM to the MPT
	else if (med == "65")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Attitude[0] = med_m65.Attitude;
		inp.DeleteGMT = 0.0;
		inp.DPSScaleFactor[0] = med_m65.DPSThrustFactor;
		inp.DT10P[0] = med_m65.TenPercentDT;

		if (med_m65.UllageDT > 0 && med_m65.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m65.UllageDT < 0)
		{
			if (med_m65.Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				inp.dt_ullage[0] = 4.0;
			}
			else if (med_m65.Thruster == RTCC_ENGINETYPE_LMDPS)
			{
				inp.dt_ullage[0] = 8.0;
			}
			else
			{
				inp.dt_ullage[0] = 0.0;
			}
		}

		inp.IterationFlag[0] = med_m65.Iteration;
		inp.Plan = med_m65.Table;
		inp.ReplaceCode = med_m65.ReplaceCode;
		inp.Thruster[0] = med_m65.Thruster;
		inp.TimeFlag[0] = med_m65.TimeFlag;
		inp.UllageThrusterOption[0] = med_m65.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(40, vPtr);
	}
	//Direct Input to MPT
	else if (med == "66")
	{
		PMMXFRDirectInput inp;

		if (med_m66.Table != 1 && med_m66.Table != 3)
		{
			return 2;
		}
		inp.TableCode = med_m66.Table;

		if (med_m66.ReplaceCode < 0 || med_m66.ReplaceCode > 15)
		{
			return 2;
		}
		inp.ReplaceCode = med_m66.ReplaceCode;
		double GMT = GMTfromGET(med_m66.GETBI);
		if (GMT < RTCCPresentTimeGMT())
		{
			return 2;
		}
		inp.GMTI = GMT;
		inp.ThrusterCode = med_m66.Thruster;

		if (med_m66.Thruster == RTCC_ENGINETYPE_LOX_DUMP && med_m66.AttitudeOpt != RTCC_ATTITUDE_INERTIAL)
		{
			return 2;
		}
		if ((med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_ASCENT || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_ASCENT) && med_m66.Thruster != RTCC_ENGINETYPE_LMAPS)
		{
			return 2;
		}

		inp.AttitudeCode = med_m66.AttitudeOpt;

		if (med_m66.BurnParamNo == 1 && !(med_m66.AttitudeOpt == RTCC_ATTITUDE_INERTIAL || med_m66.AttitudeOpt == RTCC_ATTITUDE_MANUAL))
		{
			return 2;
		}
		if (med_m66.BurnParamNo == 2 && !(med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_EXDV || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_EXDV))
		{
			return 2;
		}
		if (med_m66.BurnParamNo == 3 || med_m66.BurnParamNo == 4)
		{
			if (!(med_m66.AttitudeOpt == RTCC_ATTITUDE_INERTIAL || med_m66.AttitudeOpt == RTCC_ATTITUDE_MANUAL || med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_EXDV || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_EXDV))
			{
				return 2;
			}
		}
		if (med_m66.BurnParamNo == 6 && !(med_m66.AttitudeOpt == RTCC_ATTITUDE_PGNS_ASCENT || med_m66.AttitudeOpt == RTCC_ATTITUDE_AGS_ASCENT))
		{
			return 2;
		}

		inp.BurnParameterNumber = med_m66.BurnParamNo;
		if (med_m66.BurnParamNo == 1)
		{
			inp.CoordinateIndicator = med_m66.CoordInd;
		}
		else
		{
			inp.CoordinateIndicator = -1;
		}
		inp.Pitch = med_m66.Att.x;
		inp.Yaw = med_m66.Att.y;
		inp.Roll = med_m66.Att.z;

		if (med_m66.UllageDT > 0 && med_m66.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m66.UllageDT < 0)
		{
			if (med_m66.Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				inp.dt_ullage = 4.0;
			}
			else if (med_m66.Thruster == RTCC_ENGINETYPE_LMDPS)
			{
				inp.dt_ullage = 8.0;
			}
			else
			{
				inp.dt_ullage = 0.0;
			}
		}
		inp.UllageThrusterOption = med_m66.UllageQuads;
		inp.HeadsUpDownIndicator = med_m66.HeadsUp;

		if (med_m66.TenPercentDT == -1)
		{
			inp.DT10P = MCTDD4;
		}
		else if (med_m66.TenPercentDT == 0)
		{
			inp.DT10P = -(MCTDD2 + MCTDD3);
		}
		else if (abs(med_m66.TenPercentDT) < (MCTDD2 + MCTDD3))
		{
			return 2;
		}
		else
		{
			inp.DT10P = med_m66.TenPercentDT;
		}

		if (med_m66.REFSMMATInd < 1 || med_m66.REFSMMATInd > 10)
		{
			inp.REFSMMATIndicator = RTCC_REFSMMAT_TYPE_CUR;
		}
		else
		{
			inp.REFSMMATIndicator = med_m66.REFSMMATInd;
		}

		if (med_m66.ConfigChangeInd < 0 || med_m66.ConfigChangeInd > 2)
		{
			inp.ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_NONE;
		}
		else
		{
			inp.ConfigurationChangeIndicator = med_m66.ConfigChangeInd;
		}

		MPTGetConfigFromString(med_m66.FinalConfig, inp.EndConfiguration);
		if (inp.EndConfiguration.to_ulong() == 0)
		{
			return 2;
		}
		inp.DockingAngle = med_m66.DeltaDA*MCCRPD;
		if (med_m66.DPSThrustFactor == -1)
		{
			inp.DPSScaleFactor = MCTDTF;
		}
		else if (med_m66.DPSThrustFactor <= 0 || med_m66.DPSThrustFactor > 1)
		{
			return 2;
		}

		if (med_m66.TrimAngleIndicator != 0 && med_m66.TrimAngleIndicator != 2)
		{
			inp.TrimAngleIndicator = 0;
		}
		else
		{
			inp.TrimAngleIndicator = med_m66.TrimAngleIndicator;
		}

		void *vPtr = &inp;

		return PMMXFR(33, vPtr);
	}
	//TLI Direct Input
	else if (med == "68")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int opp;
		if (sscanf(data[1].c_str(), "%d", &opp) != 1)
		{
			return 2;
		}
		if (opp < 1 || opp > 2)
		{
			return 2;
		}

		PMMXFRDirectInput inp;

		inp.ReplaceCode = 0;
		inp.TableCode = veh;
		inp.BurnParameterNumber = opp;

		void *vPtr = &inp;
		return PMMXFR(37, vPtr);
	}
	//Transfer Descent Plan, DKI or SPQ
	else if (med == "70")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Attitude[0] = med_m70.Attitude;
		if (med_m70.DeleteGET <= 0)
		{
			inp.DeleteGMT = 0.0;
		}
		else
		{
			inp.DeleteGMT = GMTfromGET(med_m70.DeleteGET);
		}
		inp.DPSScaleFactor[0] = med_m70.DPSThrustFactor;
		inp.DT10P[0] = med_m70.TenPercentDT;

		if (med_m70.UllageDT > 0 && med_m70.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m70.UllageDT < 0)
		{
			if (med_m70.Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				inp.dt_ullage[0] = 4.0;
			}
			else if (med_m70.Thruster == RTCC_ENGINETYPE_LMDPS)
			{
				inp.dt_ullage[0] = 8.0;
			}
			else
			{
				inp.dt_ullage[0] = 0.0;
			}
		}

		inp.IterationFlag[0] = med_m70.Iteration;
		inp.Plan = med_m70.Plan;
		inp.ReplaceCode = 0;
		inp.Thruster[0] = med_m70.Thruster;
		inp.TimeFlag[0] = med_m70.TimeFlag;
		inp.UllageThrusterOption[0] = med_m70.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(41, vPtr);
	}
	//Transfer two-impulse plan to MPT
	else if (med == "72")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Attitude[0] = inp.Attitude[1] = med_m72.Attitude;
		if (med_m72.DeleteGET <= 0)
		{
			inp.DeleteGMT = 0.0;
		}
		else
		{
			inp.DeleteGMT = GMTfromGET(med_m72.DeleteGET);
		}
		inp.DPSScaleFactor[0] = inp.DPSScaleFactor[1] = med_m72.DPSThrustFactor;
		inp.DT10P[0] = inp.DT10P[1] = med_m72.TenPercentDT;

		if (med_m72.UllageDT > 0 && med_m72.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m72.UllageDT < 0)
		{
			if (med_m72.Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				inp.dt_ullage[0] = 4.0;
			}
			else if (med_m72.Thruster == RTCC_ENGINETYPE_LMDPS)
			{
				inp.dt_ullage[0] = 8.0;
			}
			else
			{
				inp.dt_ullage[0] = 0.0;
			}
		}
		else
		{
			inp.dt_ullage[0] = med_m72.UllageDT;
		}
		inp.dt_ullage[1] = inp.dt_ullage[0];

		inp.IterationFlag[0] = inp.IterationFlag[1] = med_m72.Iteration;
		inp.Plan = med_m72.Plan;
		inp.ReplaceCode = 0;
		inp.Thruster[0] = inp.Thruster[1] = med_m72.Thruster;
		inp.TimeFlag[0] = inp.TimeFlag[1] = med_m72.TimeFlag;
		inp.UllageThrusterOption[0] = inp.UllageThrusterOption[1] = med_m72.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(42, vPtr);
	}
	//Transfer of planned Earth entry maneuver
	else if (med == "74")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int ReplaceCode;
		if (data[1] == "")
		{
			ReplaceCode = 0;
		}
		else
		{
			if (sscanf(data[1].c_str(), "%d", &ReplaceCode) != 1)
			{
				return 2;
			}
			if (ReplaceCode < 1 || ReplaceCode > 15)
			{
				return 2;
			}
		}
		int type;
		if (data[2] == "SCS")
		{
			//TBD
			return 2;
		}
		else if (data[2] == "TTFP")
		{
			type = 1;
		}
		else if (data[2] == "TTFM")
		{
			//TBD
			return 2;
		}
		else if (data[2] == "RTEP")
		{
			type = 3;
		}
		else if (data[2] == "RTEM")
		{
			//TBD
			return 2;
		}
		else
		{
			return 2;
		}

		PMMXFRDirectInput inp;

		inp.AttitudeCode = RTCC_ATTITUDE_PGNS_EXDV;
		inp.BurnParameterNumber = type;
		inp.ConfigurationChangeIndicator = RTCC_CONFIGCHANGE_NONE; //TBD
		inp.CoordinateIndicator = -1;
		inp.DPSScaleFactor = MCTDTF;
		inp.ReplaceCode = ReplaceCode;
		inp.TableCode = veh;
		inp.EndConfiguration = 0; //TBD
		inp.dt_ullage = 0.0;

		void *vPtr = &inp;
		return PMMXFR(32, vPtr);
	}
	//LOI and MCC Transfer
	else if (med == "78")
	{
		PMMXFR_Impulsive_Input inp;

		inp.Table = med_m78.Table;
		inp.Plan = med_m78.ManeuverNumber;
		inp.Attitude[0] = med_m78.Attitude;
		inp.DeleteGMT = 0.0;
		inp.DPSScaleFactor[0] = med_m78.DPSThrustFactor;
		inp.DT10P[0] = med_m78.TenPercentDT;

		if (med_m78.UllageDT > 0 && med_m78.UllageDT <= 1)
		{
			return 2;
		}
		if (med_m78.UllageDT < 0)
		{
			if (med_m78.Thruster == RTCC_ENGINETYPE_LMAPS)
			{
				inp.dt_ullage[0] = 4.0;
			}
			else if (med_m78.Thruster == RTCC_ENGINETYPE_LMDPS)
			{
				inp.dt_ullage[0] = 8.0;
			}
			else
			{
				inp.dt_ullage[0] = 0.0;
			}
		}
		else
		{
			inp.dt_ullage[0] = med_m78.UllageDT;
		}

		inp.IterationFlag[0] = med_m78.Iteration;
		inp.ReplaceCode = med_m78.ReplaceCode;
		if (med_m78.Type)
		{
			inp.Type = 1;
			if (inp.Plan < 1 || inp.Plan > 8)
			{
				return 2;
			}
		}
		else
		{
			inp.Type = 0;
			if (inp.Plan < 1 || inp.Plan > 4)
			{
				return 2;
			}
		}
		inp.Thruster[0] = med_m78.Thruster;
		inp.TimeFlag[0] = med_m78.TimeFlag;
		inp.UllageThrusterOption[0] = med_m78.UllageQuads;

		void *vPtr = &inp;
		return PMMXFR(39, vPtr);
	}
	//Transfer ascent maneuver to MPT from lunar targeting
	else if (med == "85")
	{
		void *vPtr = NULL;
		return PMMXFR(44, vPtr);
	}
	//Direct input of lunar descent maneuver
	else if (med == "86")
	{
		void *vPtr = NULL;
		return PMMXFR(43, vPtr);
	}
	return 0;
}

int RTCC::GMSMED(std::string med, std::vector<std::string> data)
{
	//Enter planned or actual liftoff time
	if (med == "10")
	{
		if (data.size() < 2 || data.size() > 3)
		{
			return 1;
		}
		int Veh;
		if (data[0] == "CSM")
		{
			Veh = 1;
		}
		else if (data[0] == "LEM")
		{
			Veh = 2;
		}
		else
		{
			return 2;
		}
		double hours;
		if (MEDTimeInputHHMMSS(data[1], hours))
		{
			return 2;
		}
		if (hours < 0)
		{
			return 2;
		}

		double gmtlocs = GLHTCS(hours);
		if (Veh == 1)
		{
			MGLGMT = gmtlocs;
			MCGMTL = hours;
		}
		else
		{
			MGGGMT = gmtlocs;
			MCGMTS = hours;
		}
		if (data.size() == 3)
		{
			if (data[2] == "" || data[2] == "NOTRAJ")
			{
				//Nothing
			}
			else if (data[2] == "TRAJ")
			{
				if (Veh == 1)
				{
					PMMIEV(hours*3600.0);
				}
			}
			else
			{
				return 2;
			}
		}
	}
	//Enter planned or actual guidance reference release time and launch azimuth
	else if (med == "12")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 0;
		}
		else if (data[0] == "IU1")
		{
			veh = 1;
		}
		else if (data[0] == "IU2")
		{
			veh = 2;
		}

		double GMTGRR;
		if (MEDTimeInputHHMMSS(data[1], GMTGRR))
		{
			return 2;
		}
		if (GMTGRR < 0)
		{
			return 2;
		}
		double Azi;
		if (sscanf_s(data[2].c_str(), "%lf", &Azi) != 1)
		{
			return 2;
		}
		if (Azi<70.0 || Azi>110.0)
		{
			return 2;
		}

		MCLABN = Azi *RAD;
		MCLSBN = sin(MCLABN);
		MCLCBN = cos(MCLABN);
		if (veh == 0)
		{
			MCGRAG = GMTGRR;
			if (EZJGMTX1.data[0].ID == 0)
			{
				LMMGRP(GMTGRR*3600.0, MCLABN);
			}
		}
		else if (veh == 1)
		{
			MCGRIC = GMTGRR;
		}
		else
		{
			MCGRIL = GMTGRR;
		}
	}
	//Modify A & E used to determine integration limits
	else if (med == "30")
	{
		double val;
		if (data.size() > 0 && data[0] != "")
		{
			if (sscanf_s(data[0].c_str(), "%lf", &val) != 1)
			{
				return 2;
			}
			if (val < 0)
			{
				return 2;
			}
			MCGSMA = val / MCCNMC;
		}
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf_s(data[1].c_str(), "%lf", &val) != 1)
			{
				return 2;
			}
			if (val < 0)
			{
				return 2;
			}
			MCGECC = val;
		}
	}
	//Initialize phase reference time - GET
	else if (med == "31")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		int hh, mm;
		double ss;
		if (sscanf_s(data[0].c_str(), "%d:%d:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		double get = (double)hh + (double)mm / 60.0 + ss / 3600.0;
		if (get < 0)
		{
			return 2;
		}
		MCGREF = get;
	}
	//Update scaling factor used by venting model
	else if (med == "33")
	{
		if (data.size() != 1)
		{	
			return 1;
		}
		double val;
		if (sscanf_s(data[0].c_str(), "%lf", &val) != 1)
		{
			return 2;
		}
		MCTVEN = val;
	}
	//Update coefficients of lift and drag for new center of gravity
	else if (med == "07")
	{
		return 1;
	}
	//Update GMTZS for specified vehicle
	else if (med == "15")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int Veh;
		if (data[0] == "AGC")
		{
			Veh = 1;
		}
		if (data[0] == "LGC")
		{
			Veh = 2;
		}
		if (data[0] == "AGS")
		{
			Veh = 3;
		}
		else
		{
			return 2;
		}
		double hours;
		if (Veh == 2)
		{
			if (MEDTimeInputHHMMSS(data[1].c_str(), hours))
			{
				return 2;
			}
			if (data.size() > 1)
			{
				MCGZSL = hours;
			}
		}
		else if (Veh == 1)
		{
			if (MEDTimeInputHHMMSS(data[1].c_str(), hours))
			{
				return 2;
			}
			if (data.size() > 1)
			{
				MCGZSA = hours;
			}
		}
		else
		{
			if (data.size() > 1 && data[1] != "")
			{
				if (MEDTimeInputHHMMSS(data[1].c_str(), hours))
				{
					return 2;
				}
			}
			else
			{
				hours = MCGZSL;
			}
			double dt = 0.0;
			if (data.size() > 2 && data[2] != "")
			{
				if (MEDTimeInputHHMMSS(data[2].c_str(), dt))
				{
					return 2;
				}
			}
			MCGZSS = hours + dt;
		}
	}
	//Phase initialization, transition, and recycle
	else if (med == "81")
	{
		//TBD
	}
	//Manually EOF and rewind log tape
	else if (med == "82")
	{
		//TBD
	}
	//Generate an ephemeris for one vehicle using a vector from the other vehicle
	else if (med == "16")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int veh1, veh2;
		if (data[0] == "CSM")
		{
			veh1 = 1;
		}
		else if (data[0] == "LEM")
		{
			veh1 = 3;
		}
		else
		{
			return 2;
		}
		if (data[1] == "CSM")
		{
			veh2 = 1;
		}
		else if (data[1] == "LEM")
		{
			veh2 = 3;
		}
		else
		{
			return 2;
		}
		double gmt;
		unsigned mnv;
		bool use_mnv;

		//Error: both GMT and maneuver used
		if (data.size() > 3 && data[2] != "" && data[3] != "")
		{
			return 2;
		}
		
		//Use maneuver
		if (data.size() > 3 && data[3] != "")
		{
			use_mnv = true;
			if (sscanf(data[3].c_str(), "%d", &mnv) != 1)
			{
				return 2;
			}
			if (mnv <= 0)
			{
				return 2;
			}
		}
		else
		{
			use_mnv = false;
			if (data[2] == "")
			{
				gmt = 0.0;
			}
			else
			{
				double hh, mm, ss;
				if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
				{
					return 2;
				}
				gmt = hh * 3600.0 + mm * 60.0 + ss;
			}
		}
		EphemerisData sv0;
		if (use_mnv)
		{
			MissionPlanTable *mpt;
			if (veh1 == RTCC_MPT_CSM)
			{
				mpt = &PZMPTCSM;
			}
			else
			{
				mpt = &PZMPTLEM;
			}
			if (mpt->mantable.size() < mnv)
			{
				return 2;
			}
			sv0.R = mpt->mantable[mnv - 1].R_BO;
			sv0.V = mpt->mantable[mnv - 1].V_BO;
			sv0.GMT = mpt->mantable[mnv - 1].GMT_BO;
			sv0.RBI = mpt->mantable[mnv - 1].RefBodyInd;
		}
		else
		{
			if (gmt == 0)
			{
				gmt = RTCCPresentTimeGMT();
			}

			if (ELFECH(gmt, veh1, sv0))
			{
				return 2;
			}
		}
		PMSVCT(4, veh2, &sv0);
	}
	//Condition for Launch (Simulation)
	else if (med == "92")
	{
		//TBD
	}
	//Cape crossing table update and limit change
	else if (med == "17")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int body;
		if (data[1] == "E")
		{
			body = BODY_EARTH;
		}
		else if (data[1] == "M")
		{
			body = BODY_MOON;
		}
		else
		{
			return 2;
		}
		int rev;
		if (data.size() < 3 || data[2] == "")
		{
			rev = 0;
		}
		else
		{
			if (sscanf(data[2].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			if (rev <= 0)
			{
				return 2;
			}
		}
		double min_get, max_get, hh, mm, ss;
		if (data.size() < 4 || data[3] == "")
		{
			min_get = -1.0;
		}
		else
		{
			if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			min_get = hh * 3600.0 + mm * 60.0 + ss;
			if (min_get < 0)
			{
				return 2;
			}
		}
		if (data.size() < 5 || data[4] == "")
		{
			max_get = -1.0;
		}
		else
		{
			if (sscanf(data[4].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			max_get = hh * 3600.0 + mm * 60.0 + ss;
			if (max_get < 0)
			{
				return 2;
			}
		}
		
		CapeCrossingTable *table;

		if (veh == 1)
		{
			table = &EZCCSM;
		}
		else
		{
			table = &EZCLEM;
		}

		if (table->NumRev == 0) return 2;

		int delta = rev + 1 - table->NumRevFirst;

		table->NumRevFirst += delta;
		table->NumRevLast += delta;
	}
	//Update Fixed Ground Point Table and Landmark Acquisition Ground Point Table
	else if (med == "32")
	{
		if (data.size() < 4)
		{
			return 1;
		}
		int ActionCode;
		if (data[0] == "ADD")
		{
			ActionCode = 0;
		}
		else if (data[0] == "MOD")
		{
			ActionCode = 1;
		}
		else if (data[0] == "DEL")
		{
			ActionCode = 2;
		}
		else
		{
			return 2;
		}
		int BodyInd;
		if (data[1] == "E")
		{
			BodyInd = BODY_EARTH;
		}
		else if (data[1] == "M")
		{
			BodyInd = BODY_MOON;
		}
		int DataTableInd;
		if (data[2] == "EXST")
		{
			DataTableInd = 0;
		}
		else if (data[2] == "LDMK")
		{
			DataTableInd = 1;
		}
		else
		{
			return 2;
		}
		std::string STAID = data[3];
		if (STAID == "" || STAID.size() > 4)
		{
			return 2;
		}
		if (ActionCode == 2)
		{
			if (data.size() != 4)
			{
				return 1;
			}
		}
		else
		{
			if (data.size() != 8)
			{
				return 1;
			}
		}

		//Read rest of values
		double lat, lng, height;
		int HeightUnits;
		if (ActionCode != 2)
		{
			if (sscanf(data[4].c_str(), "%lf", &lat) != 1)
			{
				return 2;
			}
			lat *= RAD;
			if (sscanf(data[5].c_str(), "%lf", &lng) != 1)
			{
				return 2;
			}
			lng *= RAD;
			if (data[6] == "NM")
			{
				HeightUnits = 0;
			}
			else if (data[6] == "METR")
			{
				HeightUnits = 1;
			}
			else
			{
				return 2;
			}
			if (sscanf(data[7].c_str(), "%lf", &height) != 1)
			{
				return 2;
			}
			if (HeightUnits == 0)
			{
				height /= 1852.0;
			}
		}

		//Delete all logic
		if (ActionCode == 2 && STAID == "ALL")
		{
			if (DataTableInd == 0)
			{
				EZEXSITE.REF = -1;
			}
			else
			{
				EZLASITE.REF = -1;
			}

			for (int i = 0;i < 12;i++)
			{
				if (DataTableInd == 0)
				{
					EZEXSITE.Data[i].code = "";
				}
				else
				{
					EZLASITE.Data[i].code = "";
				}
			}
		}
		else
		{
			//Add
			if (ActionCode == 0)
			{
				if (DataTableInd == 0)
				{
					if (EZEXSITE.REF >= 0 && BodyInd != EZEXSITE.REF)
					{
						GMSPRINT("GMSMED", 32);
						return 2;
					}
				}
				else
				{
					if (EZLASITE.REF >= 0 && BodyInd != EZLASITE.REF)
					{
						GMSPRINT("GMSMED", 32);
						return 2;
					}
				}

				bool tablefull = true;
				int freeslot = -1;
				std::string tempname;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}

					if (STAID == tempname)
					{
						GMSPRINT("GMSMED", 23);
						return 2;
					}
					if (tempname == "")
					{
						tablefull = false;
						if (freeslot == -1)
						{
							freeslot = i;
						}
					}
				}
				if (tablefull)
				{
					GMSPRINT("GMSMED", 24);
					return 2;
				}
				if (DataTableInd == 0)
				{
					if (freeslot == 0)
					{
						EZEXSITE.REF = BodyInd;
					}
					EZEXSITE.Data[freeslot].code = STAID;
					EMGGPCHR(lat, lng, height, BodyInd, &EZEXSITE.Data[freeslot]);
				}
				else
				{
					if (freeslot == 0)
					{
						EZLASITE.REF = BodyInd;
					}
					EZLASITE.Data[freeslot].code = STAID;
					EMGGPCHR(lat, lng, height, BodyInd, &EZLASITE.Data[freeslot]);
				}
			}
			//MOD
			else if (ActionCode == 1)
			{
				std::string tempname;
				int found = -1;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}
					if (STAID == tempname)
					{
						found = i;
						break;
					}
				}
				if (found == -1)
				{
					GMSPRINT("GMSMED", 25);
					return 2;
				}
				if (DataTableInd == 0)
				{
					EMGGPCHR(lat, lng, height, BodyInd, &EZEXSITE.Data[found]);
				}
				else
				{
					EMGGPCHR(lat, lng, height, BodyInd, &EZLASITE.Data[found]);
				}
			}
			//DEL
			else
			{
				std::string tempname;
				int found = -1;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}
					if (STAID == tempname)
					{
						found = i;
						break;
					}
				}
				if (found == -1)
				{
					GMSPRINT("GMSMED", 25);
					return 2;
				}
				if (DataTableInd == 0)
				{
					EZEXSITE.Data[found].code = "";
				}
				else
				{
					EZLASITE.Data[found].code = "";
				}
				//Is table now empty?
				bool empty = true;
				for (int i = 0;i < 12;i++)
				{
					if (DataTableInd == 0)
					{
						tempname = EZEXSITE.Data[i].code;
					}
					else
					{
						tempname = EZLASITE.Data[i].code;
					}
					if (tempname != "")
					{
						empty = false;
						break;
					}
				}
				if (empty)
				{
					if (DataTableInd == 0)
					{
						EZEXSITE.REF = -1;
					}
					else
					{
						EZLASITE.REF = -1;
					}
				}
			}
		}
	}
	//Enter vector
	else if (med == "13" || med == "14")
	{
		//TBD
	}
	//High speed processing control
	else if (med == "60")
	{
		//TBD
	}
	//Offsets and elevation angle for two-impulse solution
	else if (med == "51")
	{
		double val;
		if (data.size() > 0 && data[0] != "")
		{
			if (sscanf(data[0].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TIDeltaH = val * 1852.0;
			}
		}
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf(data[1].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TIPhaseAngle = val * RAD;
			}
		}
		if (data.size() > 2 && data[2] != "")
		{
			if (sscanf(data[2].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TIElevationAngle = val * RAD;
			}
		}
		if (data.size() > 3 && data[3] != "")
		{
			if (sscanf(data[3].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TITravelAngle = val * RAD;
			}
		}
	}
	//Two-impulse corrective combination nominals
	else if (med == "52")
	{
		double val;
		if (data.size() > 0 && data[0] != "")
		{
			double hh, mm, ss;
			if (sscanf(data[0].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) == 3)
			{
				val = hh * 3600.0 + mm * 60.0 + ss;
				GZGENCSN.TINSRNominalTime = val;
			}
		}
		if (data.size() > 1 && data[1] != "")
		{
			if (sscanf(data[1].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TINSRNominalDeltaH = val * 1852.0;
			}
		}
		if (data.size() > 2 && data[2] != "")
		{
			if (sscanf(data[2].c_str(), "%lf", &val) == 1)
			{
				GZGENCSN.TINSRNominalPhaseAngle = val * RAD;
			}
		}
	}
	//Update pitch angle from horizon
	else if (med == "08")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		double val;
		if (sscanf_s(data[0].c_str(), "%lf", &val) != 1)
		{
			return 2;
		}
		MCGHZA = val * RAD;
	}
	//Initialize with mission type and day of year
	else if (med == "80")
	{
		if (data.size() < 5)
		{
			return 1;
		}
		int Num;
		if (sscanf_s(data[0].c_str(), "%d", &Num) != 1)
		{
			return 2;
		}
		if (Num != 1)
		{
			return 2;
		}

		bool found = false;
		for (unsigned i = 0;i < MHGVNM.tab.size();i++)
		{
			if (data[1] == MHGVNM.tab[i])
			{
				MCGPRM = data[1];
				found = true;
			}
		}
		if (found == false)
		{
			return 2;
		}
		int Month, Day, Year;
		if (sscanf_s(data[2].c_str(), "%d", &Month) != 1)
		{
			return 2;
		}
		if (sscanf_s(data[3].c_str(), "%d", &Day) != 1)
		{
			return 2;
		}
		if (sscanf_s(data[4].c_str(), "%d", &Year) != 1)
		{
			return 2;
		}

		if (1960 <= Year && Year <= 1980)
		{
			GZGENCSN.Year = Year;
		}
		else
		{
			sprintf(oapiDebugString(), "GMGPMED: P80 HAS INVALID DATE");
			return 2;
		}
		if (1 <= Month && Month <= 12)
		{
			GZGENCSN.MonthofLiftoff = Month;
		}
		else
		{
			sprintf(oapiDebugString(), "GMGPMED: P80 HAS INVALID DATE");
			return 2;
		}
		if (1 <= Day && Day <= 31)
		{
			GZGENCSN.DayofLiftoff = Day;
		}
		else
		{
			sprintf(oapiDebugString(), "GMGPMED: P80 HAS INVALID DATE");
			return 2;
		}

		int m[] = { 31,28,31,30,31,30,31,31,30,31,30,31 };
		double Y_J = (double)GZGENCSN.Year + 0.001;
		double J = Y_J - 1960.0;
		int K = (int)(J / 4);
		J = floor(J);
		if (((int)J) == 4 * K)
		{
			m[1] = 29;
			GZGENCSN.DaysInYear = 366;
		}
		else
		{
			GZGENCSN.DaysInYear = 365;
		}
		double T = (double)GZGENCSN.DayofLiftoff + 0.5;
		J = floor(T);
		double T_apo = (double)GZGENCSN.MonthofLiftoff - 0.5;
		K = (int)T_apo;
		GZGENCSN.DaysinMonthofLiftoff = m[K];
		GZGENCSN.RefDayOfYear = (int)J;

		if (K != 0)
		{
			for (int i = 0;i < K;i++)
			{
				GZGENCSN.RefDayOfYear += m[i];
			}
		}

		double J_D = TJUDAT(Year, Month, Day);
		GMTBASE = J_D - 2400000.5;

		//PIGBHA();

		EMSGSUPP(0, 0);
	}
	return 0;
}

int RTCC::GMSREMED(std::string med, std::vector<std::string> data)
{
	//Restart Tape
	if (med == "XX")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		int type;
		if (data[0] == "B")
		{
			type = 1;
		}
		else if (data[0] == "R")
		{
			type = 2;
		}
		else
		{
			return 2;
		}

		GMLRESRT(type);
	}
	return 0;
}

void RTCC::GMLRESRT(int type)
{
	//RTCC saving
	if (type == 1)
	{

	}
	//RTCC loading
	else if (type == 2)
	{

	}
}

int RTCC::EMGTVMED(std::string med, std::vector<std::string> data)
{
	//Space Digitals Initialization
	if (med == "00")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0]=="LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int body;
		if (data.size() < 2 || data[1] == "" || data[1] == "E")
		{
			body = BODY_EARTH;
		}
		else if (data[1] == "M")
		{
			body = BODY_MOON;
		}
		else
		{
			return 2;
		}
		EZETVMED.SpaceDigVehID = veh;
		EZETVMED.SpaceDigCentralBody = body;

		EMDSPACE(7);
	}
	//Space Digitals
	else if (med == "01")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int column;
		if (sscanf(data[0].c_str(), "%d", &column) != 1)
		{
			return 2;
		}
		if (column < 1 || column > 3)
		{
			return 2;
		}
		int opt;
		if (data[1] == "GET")
		{
			opt = 0;
		}
		else if (data[1] == "MNV")
		{
			opt = 1;
		}
		else
		{
			return 2;
		}
		double val;
		if (opt == 0)
		{
			double hh, mm, ss;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			val = hh * 3600.0 + mm * 60.0 + ss;
		}
		else
		{
			int man;
			if (sscanf(data[2].c_str(), "%d", &man) != 1)
			{
				return 2;
			}
			val = (double)man;
		}

		double incl = 0, ascnode = 0;
		if (column == 2)
		{
			if (data.size() < 5)
			{
				return 1;
			}
			if (sscanf(data[3].c_str(), "%lf", &incl) != 1)
			{
				return 2;
			}
			incl *= RAD;
			if (incl < 0 || incl > PI)
			{
				return 2;
			}
			if (sscanf(data[4].c_str(), "%lf", &ascnode) != 1)
			{
				return 2;
			}
			ascnode *= RAD;
		}
		int queid;
		if (column == 1)
		{
			queid = 3;
		}
		else if (column == 2)
		{
			queid = 4;
		}
		else
		{
			queid = 5;
		}

		EMDSPACE(queid, opt, val, incl, ascnode);
	}
	//Initiate Checkout Monitor Display
	else if (med == "02")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int tab;
		if (data[0] == "CSM")
		{
			tab = 1;
		}
		else if (data[0] == "LEM")
		{
			tab = 3;
		}
		else
		{
			return 2;
		}
		int opt;
		if (data[1] == "GMT")
		{
			opt = 1;
		}
		else if (data[1] == "GET")
		{
			opt = 2;
		}
		else if (data[1] == "MVI")
		{
			opt = 3;
		}
		else if (data[1] == "MVE")
		{
			opt = 4;
		}
		else if (data[1] == "RAD")
		{
			opt = 5;
		}
		else if (data[1] == "ALT")
		{
			opt = 6;
		}
		else if (data[1] == "FPA")
		{
			opt = 7;
		}
		else
		{
			return 2;
		}
		double param;
		if (opt == 1 || opt == 2)
		{
			double hh, mm, ss;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			param = hh * 3600.0 + mm * 60.0 + ss;
		}
		else
		{
			if (sscanf(data[2].c_str(), "%lf", &param) != 1)
			{
				return 2;
			}
			if (opt == 5 || opt == 6)
			{
				param *= 1852.0;
			}
			else if (opt == 7)
			{
				param = sin(param*RAD);
			}
		}
		bool hasTHT;
		double THTime = -1.0;
		if (data.size() < 4 || data[3] == "")
		{
			hasTHT = false;
		}
		else
		{
			hasTHT = true;
		}
		//Illegal for MVI and MVE
		if (hasTHT && (opt == 3 || opt == 4))
		{
			return 2;
		}
		if (opt >= 5)
		{
			//Mandatory for RAD, ALT, FPA
			if (hasTHT == false)
			{
				return 2;
			}
			double hh, mm, ss;
			if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			THTime = hh * 3600.0 + mm * 60.0 + ss;
		}
		else if (opt <= 2)
		{
			if (hasTHT)
			{
				double hh, mm, ss;
				if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
				{
					return 2;
				}
				THTime = hh * 3600.0 + mm * 60.0 + ss;
			}
			else
			{
				THTime = -1.0;
			}
		}
		int ref;
		if (data.size() < 5 || data[4] == "" || data[4] == "ECI")
		{
			ref = 0;
		}
		else if (data[4] == "ECT")
		{
			ref = 1;
		}
		else if (data[4] == "MCI")
		{
			ref = 2;
		}
		else if (data[4] == "MCT")
		{
			ref = 3;
		}
		else
		{
			return 2;
		}
		bool feet;
		if (data.size() < 6 || data[5] == "")
		{
			feet = false;
		}
		else if (data[5] == "FT")
		{
			feet = true;
		}
		else
		{
			return 2;
		}
		EMDCHECK(tab, opt, param, THTime, ref, feet);
	}
	//Update Relative Motion Digital Display
	else if (med == "03")
	{
		if (data.size() < 6)
		{
			return 1;
		}
		int Veh1, Veh2;
		if (data[0] == "CSM")
		{
			Veh1 = 1;
		}
		else if (data[0] == "LEM")
		{
			Veh1 = 3;
		}
		else
		{
			return 2;
		}
		if (data[1] == "CSM")
		{
			Veh2 = 1;
		}
		else if (data[1] == "LEM")
		{
			Veh2 = 3;
		}
		else
		{
			return 2;
		}
		if (Veh1 == Veh2)
		{
			return 2;
		}
		double get;
		if (MEDTimeInputHHMMSS(data[2], get))
		{
			return 2;
		}
		if (get < 0)
		{
			return 2;
		}
		double dt;
		if (sscanf_s(data[3].c_str(), "%lf", &dt) != 1)
		{
			return 2;
		}
		if (dt<1.0 || dt>1800.0)
		{
			return 2;
		}
		int refs;
		if (data[4] == "")
		{
			refs = 1;
		}
		else
		{
			refs = EMGSTGENCode(data[4].c_str());
			if (refs < 0)
			{
				return 2;
			}
		}
		int axis;
		if (Veh1 == 1)
		{
			if (data[5] != "CX")
			{
				return 2;
			}
			axis = 1;
		}
		else
		{
			if (data[5] == "LX")
			{
				axis = 1;
			}
			else if (data[5] == "LZ")
			{
				axis = 2;
			}
			else
			{
				return 2;
			}
		}
		int mode = 1;
		if (data.size() > 6)
		{
			if (data[6] == "" || data[6] == "1")
			{
				mode = 1;
			}
			else if (data[6] == "2")
			{
				mode = 2;
			}
			//In Mode 2 the LM needs to be the chaser
			if (mode == 2 && Veh1 == 1)
			{
				return 2;
			}
			if (mode == 2 && data.size() != 11)
			{
				return 1;
			}
		}
		EMMRMD(Veh1, Veh2, get*3600.0, dt, refs, axis, mode);
	}
	//Generate Rendezvous Evaluation Display
	else if (med == "06")
	{
		if (data.size() != 1)
		{
			return 1;
		}
		int plan;
		if (sscanf(data[0].c_str(), "%d", &plan) != 1)
		{
			return 2;
		}
		if (plan < 1 || plan > 7)
		{
			return 2;
		}
		EZETVMED.RETPlan = plan;
		PMDRET();
	}
	//Moonrise/Moonset Display
	else if (med == "07")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int ind;

		if (data[0] == "GET")
		{
			ind = 1;
		}
		else if (data[0] == "REV")
		{
			ind = 2;
		}
		else
		{
			return 2;
		}
		double param;
		if (ind == 1)
		{
			if (data.size() < 2)
			{
				param = 0.0;
			}
			double hh, mm, ss;
			if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			param = hh * 3600.0 + mm * 60.0 + ss;
		}
		else
		{
			if (data.size() < 2)
			{
				return 1;
			}
			int rev;
			if (sscanf(data[1].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			if (rev > EZCCSM.NumRevLast)
			{
				return 2;
			}
			param = (double)rev;
		}
		EMDSSMMD(ind, param);
	}
	//Sunrise/Sunset Display
	else if (med == "08")
	{
		if (data.size() < 1)
		{
			return 1;
		}
		int ind;

		if (data[0] == "GET")
		{
			ind = 1;
		}
		else if (data[0] == "REV")
		{
			ind = 2;
		}
		else
		{
			return 2;
		}
		double param;
		if (ind == 1)
		{
			if (data.size() < 2)
			{
				param = 0.0;
			}
			else
			{
				double hh, mm, ss;
				if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
				{
					return 2;
				}
				param = hh * 3600.0 + mm * 60.0 + ss;
			}
		}
		else
		{
			if (data.size() < 2)
			{
				return 2;
			}
			int rev;
			if (sscanf(data[1].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			if (rev > EZCCSM.NumRevLast)
			{
				return 2;
			}
			param = (double)rev;
		}
		EMDSSEMD(ind, param);
	}
	//FDO Orbit Digitals: Predict apogee/perigee
	else if (med == "12")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int L;
		if (data[0] == "CSM")
		{
			L = 1;
		}
		else if (data[0] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}

		int ind;
		if (data[1] == "REV")
		{
			ind = 1;
		}
		else if (data[1] == "GET")
		{
			ind = 2;
		}
		else if (data[1] == "MNV")
		{
			ind = 3;
		}
		else
		{
			return 2;
		}
		double param;
		if (ind == 1 || ind == 3)
		{
			int rev;
			if (sscanf(data[2].c_str(), "%d", &rev) != 1)
			{
				return 2;
			}
			param = (double)rev;
		}
		else
		{
			double hh, mm, ss;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			param = hh * 3600.0 + mm * 60.0 + ss;
		}

		EMMDYNMC(L, 3, ind, param);
	}
	//FDO Orbit Digitals: Longitude crossing time
	else if (med == "13")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		int L;
		if (data[0] == "CSM")
		{
			L = 1;
		}
		else if (data[0] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}

		int ind;
		if (sscanf(data[1].c_str(), "%d", &ind) != 1)
		{
			return 2;
		}

		double lng;
		if (sscanf(data[2].c_str(), "%lf", &lng) != 1)
		{
			return 2;
		}
		EMMDYNMC(L, 4, ind, lng*RAD);
	}
	//FDO Orbit Digitals: Compute longitude at given time
	else if (med == "14")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int L;
		if (data[0] == "CSM")
		{
			L = 1;
		}
		else if (data[0] == "LEM")
		{
			L = 3;
		}
		else
		{
			return 2;
		}

		double hh, mm, ss;
		if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		double param = hh * 3600.0 + mm * 60.0 + ss;
		EMMDYNMC(L, 5, 0, param);
	}
	//Predicted and experimental site acquisition
	else if (med == "15" || med == "16" || med == "55")
	{
		if (data.size() < 4)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int ind;
		if (data[1] == "REV")
		{
			ind = 0;
		}
		else if (data[1] == "GET")
		{
			ind = 1;
		}
		else
		{
			return 2;
		}
		double vala, valb;
		if (ind == 0)
		{
			int revb, reve;

			if (sscanf(data[2].c_str(), "%d", &revb) != 1)
			{
				return 2;
			}
			if (revb <= 0)
			{
				return 2;
			}
			if (sscanf(data[3].c_str(), "%d", &reve) != 1)
			{
				return 2;
			}
			if (reve <= 0)
			{
				return 2;
			}
			vala = (double)revb;
			valb = (double)reve;
		}
		else
		{
			double hh, mm, ss;
			double stime, dt;
			if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			stime = hh * 3600.0 + mm * 60.0 + ss;
			if (sscanf(data[3].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
			{
				return 2;
			}
			dt = hh * 3600.0 + mm * 60.0 + ss;
			if (dt < 0 || dt > 24.0*3600.0)
			{
				return 2;
			}
			vala = stime;
			valb = stime + dt;
		}
		int body = 0;
		if (data.size() >= 5)
		{
			if (data[4] == "E")
			{
				body = 0;
			}
			else if (data[4] == "M")
			{
				body = 2;
			}
			else
			{
				return 2;
			}
		}
		
		int num;
		if (med == "15")
		{
			num = 1;
		}
		else if (med == "16")
		{
			num = 3;
		}
		else
		{
			num = 2;
		}

		//TBD: Store in EZETVMED

		EMDPESAD(num, veh, ind, vala, valb, body);
	}
	//Landmark Acquisition Display
	else if (med == "17")
	{
		if (data.size() < 3)
		{
			return 1;
		}
		if (data[0] != "CSM")
		{
			return 2;
		}
		int veh = 1;
		double hh, mm, ss, get, dt;
		if (sscanf(data[1].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		get = hh * 3600.0 + mm * 60.0 + ss;
		if (sscanf(data[2].c_str(), "%lf:%lf:%lf", &hh, &mm, &ss) != 3)
		{
			return 2;
		}
		dt = hh * 3600.0 + mm * 60.0 + ss;
		int ref;
		if (data.size() < 4 || data[3] == "" || data[3] == "E")
		{
			ref = 0;
		}
		else if (data[3] == "M")
		{
			ref = 2;
		}
		else
		{
			return 2;
		}
		double gmt = GMTfromGET(get);
		EZETVMED.LandmarkVehID = veh;
		EZETVMED.LandmarkGMT = gmt;
		EZETVMED.LandmarkDT = dt;
		EZETVMED.LandmarkRef = ref;

		EMDLANDM(veh, gmt, dt, ref);
	}
	//Detailed Maneuver Table
	else if (med == "20")
	{
		if (data.size() < 2)
		{
			return 1;
		}
		int veh;
		if (data[0] == "CSM")
		{
			veh = 1;
		}
		else if (data[0] == "LEM")
		{
			veh = 3;
		}
		else
		{
			return 2;
		}
		int num;
		if (sscanf(data[1].c_str(), "%d", &num) != 1)
		{
			return 2;
		}
		if (num < 1 || num>15)
		{
			return 2;
		}

		DetailedManeuverTable *dmt;
		int dmtnum;
		if (data.size() < 3 || data[2] == "" || data[2] == "54")
		{
			dmt = &DMTBuffer[0];
			dmtnum = 1;
		}
		else if (data[2] == "69")
		{
			dmt = &DMTBuffer[1];
			dmtnum = 2;
		}
		else
		{
			return 2;
		}

		int refscode;
		if (data.size() < 4 || data[3] == "")
		{
			refscode = 1;
		}
		else if (data[3] == "DES")
		{
			refscode = 100;
		}
		else
		{
			refscode = EMGSTGENCode(data[3].c_str());
			if (refscode < 0)
			{
				return 2;
			}
		}

		bool headsupdownind;
		if (refscode == 100)
		{
			if (data.size() < 5 || data[4] == "" || data[4] == "U")
			{
				headsupdownind = true;
			}
			else if (data[4] == "D")
			{
				headsupdownind = false;
			}
			else
			{
				return 2;
			}
		}

		//Save inputs
		if (dmtnum == 1)
		{
			EZETVMED.DMT1Vehicle = veh;
			EZETVMED.DMT1Number = num;
			EZETVMED.DMT1REFSMMATCode = refscode;
			EZETVMED.DMT1HeadsUpDownIndicator = headsupdownind;
		}
		else
		{
			EZETVMED.DMT2Vehicle = veh;
			EZETVMED.DMT2Number = num;
			EZETVMED.DMT2REFSMMATCode = refscode;
			EZETVMED.DMT2HeadsUpDownIndicator = headsupdownind;
		}

		PMDDMT(veh, num, refscode, headsupdownind, *dmt);
	}
	return 0;
}

double RTCC::PIGMHA(int E, int Y, int D)
{
	int XN;
	static const double A = 0.0929;
	static const double B = 8640184.542;
	static const double W1 = 1.720217954160054e-2;
	double C, T, DE, BHA, DI, DELTA;

	XN = (E - 1901) / 4;
	C = -86400.0*(double)(E - 1900) - 74.164;
	T = 2 * C / (-B - sqrt(B*B - 4 * A*C));
	DE = 36525.0*T - 365.0*(double)(E - 1900) + 0.5 - (double)XN;
	if (Y == E)
	{
		DI = D;
	}
	else
	{
		int X = Y % 4;
		if (X == 0)
		{
			DI = D - 366.0;
		}
		else
		{
			DI = D - 365.0;
		}
	}
	DELTA = DI - DE;
	BHA = 2.0 / 3.6 + W1 * DELTA;
	return BHA;
}

EphemerisData RTCC::ConvertSVtoEphemData(SV sv)
{
	EphemerisData svnew;

	svnew.R = sv.R;
	svnew.V = sv.V;
	svnew.GMT = OrbMech::GETfromMJD(sv.MJD, GMTBASE);
	if (sv.gravref == hEarth)
	{
		svnew.RBI = BODY_EARTH;
	}
	else
	{
		svnew.RBI = BODY_MOON;
	}
	return svnew;
}

SV RTCC::ConvertEphemDatatoSV(EphemerisData sv)
{
	SV svnew;

	svnew.R = sv.R;
	svnew.V = sv.V;
	svnew.MJD = OrbMech::MJDfromGET(sv.GMT, GMTBASE);
	if (sv.RBI == BODY_EARTH)
	{
		svnew.gravref = hEarth;
	}
	else
	{
		svnew.gravref = hMoon;
	}

	return svnew;
}

double RTCC::TJUDAT(int Y, int M, int D)
{
	int Y_apo = Y - 1900;
	int TMM[] = { 0,31,59,90,120,151,181,212,243,273,304,334 };

	int Z = Y_apo / 4;
	if (Y_apo % 4 == 0)
	{
		Z = Z - 1;
		for (int i = 2;i < 12;i++)
		{
			TMM[i] += 1;
		}
	}
	return 2415020.5 + (double)(365 * Y_apo + Z + TMM[M - 1] + D - 1);
}

double RTCC::PCDETA(double beta1, double beta2, double r1, double r2)
{
	double ctn_eta122 = (OrbMech::cot(beta1) + r1 / r2 * OrbMech::cot(beta2)) / (1 - r1 / r2);
	double cos_eta12 = (pow(ctn_eta122, 2) - 1.0) / (pow(ctn_eta122, 2) + 1.0);
	double eta12 = atan2((1.0 - cos_eta12)*ctn_eta122, cos_eta12);
	if (eta12 < 0)
	{
		eta12 += PI2;
	}
	return eta12;
}

MATRIX3 RTCC::PIDREF(VECTOR3 AT, VECTOR3 R, VECTOR3 V, double PG, double YG, bool K)
{
	//K: 0 for heads-down, 1 for heads-up
	VECTOR3 X_T, Y_T, Z_T, X_P, Y_P, Z_P;
	double r, y_T;

	X_T = AT;
	r = length(R);
	Y_T = crossp(R, X_T);
	y_T = length(Y_T);

	if (K)
	{
		Y_T = -Y_T;
	}
	if (y_T / r < 0.0017)
	{
		Y_T = crossp(R, V);
		y_T = length(Y_T);
	}
	Y_T = Y_T / y_T;
	Z_T = crossp(X_T, Y_T);

	X_P = X_T * cos(PG)*cos(YG) - Y_T * cos(PG)*sin(YG) + Z_T * sin(PG);
	Y_P = X_T * sin(YG) + Y_T * cos(YG);
	Z_P = -X_T * sin(PG)*cos(YG) + Y_T * sin(PG)*sin(YG) + Z_T * cos(PG);
	return _M(X_P.x, X_P.y, X_P.z, Y_P.x, Y_P.y, Y_P.z, Z_P.x, Z_P.y, Z_P.z);
}

void RTCC::PIMCKC(VECTOR3 R, VECTOR3 V, int body, double &a, double &e, double &i, double &l, double &g, double &h)
{
	double mu;

	if (body == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
	}

	double v = length(V);
	double r = length(R);
	double eps = v * v / 2.0 - mu / r;
	VECTOR3 H = crossp(R, V);

	a = -mu / (2.0*eps);
	VECTOR3 E = crossp(V, H) / mu - R / r;
	e = length(E);
	i = acos(H.z / length(H));
	double theta = acos2(dotp(unit(E), unit(R)));
	if (dotp(R, V) < 0)
	{
		theta = PI2 - theta;
	}
	double EE = atan2(sqrt(1.0 - e * e)*sin(theta), e + cos(theta));
	if (EE < 0)
	{
		EE += PI2;
	}
	l = EE - e * sin(EE);
	VECTOR3 K = _V(0, 0, 1);
	VECTOR3 N = crossp(K, H);
	g = acos2(dotp(unit(N), unit(E)));
	if (E.z < 0)
	{
		g = PI2 - g;
	}
	h = acos2(N.z/length(N));
	if (N.y < 0)
	{
		h = PI2 - h;
	}
}

void RTCC::PIBURN(VECTOR3 R, VECTOR3 V, double T, double *B, VECTOR3 &ROUT, VECTOR3 &VOUT, double &TOUT)
{
	//B[0]: DT
	//B[1]: DR
	//B[2]: DV
	//B[3]: DS (arc length)
	//B[4]: dgamma
	//B[5]: dpsi
	//B[6]: isp
	//B[7]: characteristic DV
	//B[8]: mass ratio

	VECTOR3 H, R1, V1, R2, V2, R3, V3;
	double D;

	TOUT = T + B[0];
	H = crossp(R, V);
	D = dotp(R, V);
	if (B[3] == 0.0)
	{
		R1 = R;
		V1 = V;
	}
	else
	{
		R1 = R * cos(B[3]) + (V*pow(length(R), 2) - R * D)*sin(B[3]) / length(H);
		V1 = V * cos(B[3]) + (V*D - R * pow(length(R), 2))*sin(B[3]) / length(H);
	}
	if (B[4] == 0.0)
	{
		R2 = R1;
		V2 = V1;
	}
	else
	{
		R2 = R1;
		V2 = V1 * cos(-B[4]) + (V1*D - R1 * pow(length(V), 2))*sin(-B[4]) / length(H);
	}
	if (B[5] == 0.0)
	{
		R3 = R2;
		V3 = V2;
	}
	else
	{
		R3 = R2;
		V3 = R * 2.0*(dotp(R2, V)*pow(sin(-B[5] / 2.0), 2)) / pow(length(R), 2) + V2 * cos(-B[5]) + crossp(R2, V2)*sin(-B[5]) / length(R);
	}
	if (B[1] == 0.0)
	{
		ROUT = R3;
	}
	else
	{
		ROUT = R3 * (length(R) + B[1]) / length(R);
	}
	if (B[2] == 0.0)
	{
		VOUT = V3;
	}
	else
	{
		VOUT = V3 * (length(V) + B[2]) / length(V);
	}
	B[8] = sqrt(B[2] * B[2] + 4.0*length(V)*(length(V) + B[2])*pow(sin(B[4] / 2.0), 2) + cos(asin(dotp(unit(R), unit(V))))*cos(asin(dotp(unit(R), unit(V))) + B[4])*pow(sin(B[5] / 2.0), 2));
	B[7] = exp(-abs(B[8] / B[6]));
}

void RTCC::PICSSC(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &lat, double &lng, double &gamma, double &azi)
{
	if (vecinp)
	{
		r = length(R);
		lat = asin(R.z / r);
		lng = atan2(R.y, R.x);
		if (lng < 0)
		{
			lng += PI2;
		}
		VECTOR3 TEMP = mul(_M(cos(lng)*cos(lat), sin(lng)*cos(lat), sin(lat), -sin(lng), cos(lng), 0, -sin(lat)*cos(lng), -sin(lat)*sin(lng), cos(lat)), V);
		v = length(TEMP);
		gamma = asin(TEMP.x/ v);
		azi = atan2(TEMP.y, TEMP.z);
		if (azi < 0)
		{
			azi += PI2;
		}
	}
	else
	{
		R = _V(cos(lat)*cos(lng), cos(lat)*sin(lng), sin(lat))*r;
		V = mul(_M(cos(lat)*cos(lng), -sin(lng), -sin(lat)*cos(lng), cos(lat)*sin(lng), cos(lng), -sin(lat)*sin(lng), sin(lat), 0, cos(lat)), _V(sin(gamma), cos(gamma)*sin(azi), cos(gamma)*cos(azi))*v);
	}
}

void RTCC::PIFAAP(double a, double e, double i, double f, double u, double r, double &r_apo, double &r_peri)
{
	double a_ref, e_ref, p_ref, p, K1, K2, df, r1, r2;

	a_ref = r + OrbMech::J2_Earth / OrbMech::R_Earth*(1.0 - 3.0 / 2.0*pow(sin(i), 2) + 5.0 / 6.0*pow(sin(i), 2)*cos(2.0*u));
	e_ref = 1.0 - r / a_ref;
	p_ref = a_ref * (1.0 - e_ref * e_ref);
	p = a * (1.0 - e * e);
	K1 = e / sqrt(p);
	K2 = e_ref / sqrt(p_ref);
	df = atan2(K1*sin(f), K2 - K1 * cos(f));
	r1 = p / (1.0 + e * cos(f + df)) - p_ref / (1.0 + e_ref * cos(df)) + r;
	r2 = p / (1.0 - e * cos(f + df)) - p_ref / (1.0 - e_ref * cos(df)) + r;
	if (r1 >= r2)
	{
		r_apo = r1;
		r_peri = r2;
	}
	else
	{
		r_peri = r1;
		r_apo = r2;
	}
}

int RTCC::PIATSU(AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double &isg, double &gsg, double &hsg)
{
	PMMLAEG aeg;
	AEGHeader header;
	MATRIX3 Rot;
	VECTOR3 P, W, P_apo, W_apo;
	double eps_i, eps_t, usg, du, theta_dot, dt;
	int KE, K;

	eps_i = 1e-4;
	eps_t = 0.01;
	header.AEGInd = 1;
	header.ErrorInd = 0;
	AEGIN.TIMA = 0;

	AEGOUT = AEGIN;
	KE = 0;
	K = 1;
RTCC_PIATSU_1A:
	Rot = OrbMech::GetRotationMatrix(BODY_MOON, GMTBASE + AEGOUT.TS / 24.0 / 3600.0);
	OrbMech::PIVECT(AEGOUT.coe_osc.i, AEGOUT.coe_osc.g, AEGOUT.coe_osc.h, P, W);
	P_apo = rhtmul(Rot, P);
	W_apo = rhtmul(Rot, W);
	OrbMech::PIVECT(P_apo, W_apo, isg, gsg, hsg);
	if (isg < eps_i || isg > PI - eps_i)
	{
		KE = 2;
		return KE;
	}
	usg = AEGOUT.f + gsg;
	if (usg >= PI2)
	{
		usg = usg - PI2;
	}
	if (K > 1)
	{
		du = AEGIN.Item8 - usg;
		theta_dot = sqrt(OrbMech::mu_Moon*AEGOUT.coe_osc.a*(1.0 - pow(AEGOUT.coe_osc.e, 2))) / pow(AEGOUT.R, 2) + AEGOUT.g_dot;
		if (abs(du) > PI)
		{
			if (du > 0)
			{
				du = du - PI2;
			}
			else if (du <= 0)
			{
				du = du + PI2;
			}
		}
	}
	else
	{
		du = AEGIN.Item8 - usg + AEGIN.Item10*PI2;
		theta_dot = sqrt(OrbMech::mu_Moon / pow(AEGIN.coe_osc.a, 3));
	}
	dt = du / theta_dot;
	if (abs(dt) <= eps_t)
	{
		return KE;
	}
	if (K >= 7)
	{
		KE = 1;
		return KE;
	}
	AEGIN.TE = AEGOUT.TS + dt;
	K++;
	aeg.CALL(header, AEGIN, AEGOUT);
	if (header.ErrorInd)
	{
		KE = -2;
		return KE;
	}
	goto RTCC_PIATSU_1A;
}

void RTCC::EMGSTGEN(int QUEID, int L1, int ID1, int L2, int ID2, double gmt, MATRIX3 *refs)
{
	//CSM/LEM REFSMMAT locker movement
	if (QUEID == 2)
	{
		REFSMMATLocker *tab1, *tab2;

		if (L1 == RTCC_MANVEHICLE_CSM)
		{
			tab1 = &EZJGMTX1;
		}
		else
		{
			tab1 = &EZJGMTX3;
		}
		if (L2 == RTCC_MANVEHICLE_CSM)
		{
			tab2 = &EZJGMTX1;
		}
		else
		{
			tab2 = &EZJGMTX3;
		}

		if (L1 == RTCC_MANVEHICLE_CSM)
		{
			if (ID1 >= RTCC_REFSMMAT_TYPE_AGS)
			{
				//Error
				return;
			}
			
		}
		else
		{
			
		}
		if (L2 == RTCC_MANVEHICLE_CSM)
		{
			if (!(ID2 == RTCC_REFSMMAT_TYPE_CUR || ID2 == RTCC_REFSMMAT_TYPE_PCR || ID2 == RTCC_REFSMMAT_TYPE_TLM || ID2 == RTCC_REFSMMAT_TYPE_MED || ID2 == RTCC_REFSMMAT_TYPE_LCV))
			{
				//Error
				return;
			}
		}
		else
		{
			if (ID2 == RTCC_REFSMMAT_TYPE_OST || ID2 == RTCC_REFSMMAT_TYPE_DMT || ID2 == RTCC_REFSMMAT_TYPE_DOK || ID2 == RTCC_REFSMMAT_TYPE_DOD || ID2 == RTCC_REFSMMAT_TYPE_LLD)
			{
				//Error
				return;
			}
		}

		if (tab1->data[ID1 - 1].ID == 0)
		{
			//ERROR: REFSMMAT NOT AVAILABLE
			EMGPRINT("EMGSTGEN", 1);
			return;
		}

		if (ID2 == RTCC_REFSMMAT_TYPE_CUR)
		{
			EMGSTSTM(L2, tab2->data[0].REFSMMAT, RTCC_REFSMMAT_TYPE_PCR, gmt);
		}
		EMGSTSTM(L2, tab1->data[ID1 - 1].REFSMMAT, ID2, gmt);
	}
}

void RTCC::EMGSTSTM(int L, MATRIX3 REFS, int id, double gmt)
{
	REFSMMATLocker *tab;
	if (L == RTCC_MANVEHICLE_CSM)
	{
		tab = &EZJGMTX1;
	}
	else
	{
		tab = &EZJGMTX3;
	}

	tab->data[id - 1].ID++;
	tab->data[id - 1].REFSMMAT = REFS;
	tab->data[id - 1].GMT = gmt;

	char Buffer[128];
	char Buff1[4];
	char Buff2[32];
	char Buff3[4];

	EMGSTGENName(id, Buff1);
	OrbMech::format_time_HHMMSS(Buff2, GETfromGMT(gmt));
	if (L == 1)
	{
		sprintf_s(Buff3, "CSM");
	}
	else
	{
		sprintf_s(Buff3, "LEM");
	}

	sprintf_s(Buffer, "NEW IMU MATRIX %s%03d %s", Buff1, tab->data[id - 1].ID, Buff3);
	RTCCONLINEMON.TextBuffer[0].assign(Buffer);
	sprintf_s(Buffer, "GET = %s", Buff2);
	RTCCONLINEMON.TextBuffer[1].assign(Buffer);
	RTCCONLINEMON.MatrixBuffer = tab->data[id - 1].REFSMMAT;
	EMGPRINT("EMGSTSTM", 19);
}

void RTCC::FormatREFSMMATCode(int ID, int num, char *buff)
{
	char buff2[4];
	EMGSTGENName(ID, buff2);
	sprintf_s(buff, 7, "%s%03d", buff2, num);
}

void RTCC::EMGSTGENName(int ID, char *Buffer)
{
	switch (ID)
	{
	case RTCC_REFSMMAT_TYPE_CUR:
		sprintf_s(Buffer, 4, "CUR");
		break;
	case RTCC_REFSMMAT_TYPE_PCR:
		sprintf_s(Buffer, 4, "PCR");
		break;
	case RTCC_REFSMMAT_TYPE_TLM:
		sprintf_s(Buffer, 4, "TLM");
		break;
	case RTCC_REFSMMAT_TYPE_OST:
		sprintf_s(Buffer, 4, "OST");
		break;
	case RTCC_REFSMMAT_TYPE_MED:
		sprintf_s(Buffer, 4, "MED");
		break;
	case RTCC_REFSMMAT_TYPE_DMT:
		sprintf_s(Buffer, 4, "DMT");
		break;
	case RTCC_REFSMMAT_TYPE_DOD:
		sprintf_s(Buffer, 4, "DOD");
		break;
	case RTCC_REFSMMAT_TYPE_LCV:
		sprintf_s(Buffer, 4, "LCV");
		break;
	case RTCC_REFSMMAT_TYPE_AGS:
		sprintf_s(Buffer, 4, "AGS");
		break;
	case RTCC_REFSMMAT_TYPE_DOK:
		sprintf_s(Buffer, 4, "DOK");
		break;
	case RTCC_REFSMMAT_TYPE_LLA:
		sprintf_s(Buffer, 4, "LLA");
		break;
	case RTCC_REFSMMAT_TYPE_LLD:
		sprintf_s(Buffer, 4, "LLD");
		break;
	default:
		sprintf_s(Buffer, 4, "");
		break;
	}
}

int RTCC::EMGSTGENCode(const char *Buffer)
{
	if (!strcmp(Buffer, "CUR"))
	{
		return RTCC_REFSMMAT_TYPE_CUR;
	}
	else if (!strcmp(Buffer, "PCR"))
	{
		return RTCC_REFSMMAT_TYPE_PCR;
	}
	else if (!strcmp(Buffer, "TLM"))
	{
		return RTCC_REFSMMAT_TYPE_TLM;
	}
	else if (!strcmp(Buffer, "OST"))
	{
		return RTCC_REFSMMAT_TYPE_OST;
	}
	else if (!strcmp(Buffer, "MED"))
	{
		return RTCC_REFSMMAT_TYPE_MED;
	}
	else if (!strcmp(Buffer, "DMT"))
	{
		return RTCC_REFSMMAT_TYPE_DMT;
	}
	else if (!strcmp(Buffer, "DOD"))
	{
		return RTCC_REFSMMAT_TYPE_DOD;
	}
	else if (!strcmp(Buffer, "LCV"))
	{
		return RTCC_REFSMMAT_TYPE_LCV;
	}
	else if (!strcmp(Buffer, "AGS"))
	{
		return RTCC_REFSMMAT_TYPE_AGS;
	}
	else if (!strcmp(Buffer, "DOK"))
	{
		return RTCC_REFSMMAT_TYPE_DOK;
	}
	else if (!strcmp(Buffer, "LLA"))
	{
		return RTCC_REFSMMAT_TYPE_LLA;
	}
	else if (!strcmp(Buffer, "LLD"))
	{
		return RTCC_REFSMMAT_TYPE_LLD;
	}

	return -1;
}

void RTCC::EMGPRINT(std::string source, int i)
{
	std::vector<std::string> message;
	switch (i)
	{
	case 1:
		message.push_back("REFSMMAT NOT AVAILABLE");
		break;
	case 12:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " EPHEMERIS UPDATE COMPLETED");
		break;
	case 13:
		message.push_back(RTCCONLINEMON.TextBuffer[0] + " MINIMUM EPHEMERIS WAS NOT GENERATED");
		break;
	case 19:
		char Buffer[128];
		message.push_back(RTCCONLINEMON.TextBuffer[0]);
		message.push_back(RTCCONLINEMON.TextBuffer[1]);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m11, RTCCONLINEMON.MatrixBuffer.m12, RTCCONLINEMON.MatrixBuffer.m13);
		message.push_back(Buffer);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m21, RTCCONLINEMON.MatrixBuffer.m22, RTCCONLINEMON.MatrixBuffer.m23);
		message.push_back(Buffer);
		sprintf_s(Buffer, "%+.7lf %+.7lf %+.7lf", RTCCONLINEMON.MatrixBuffer.m31, RTCCONLINEMON.MatrixBuffer.m32, RTCCONLINEMON.MatrixBuffer.m33);
		message.push_back(Buffer);
		break;
	case 21:
		message.push_back("MANEUVER NOT IN PLAN");
		break;
	case 26:
		message.push_back("EPHEMERIS NOT AVAILABLE");
		break;
	case 25:
		message.push_back("RADIAL FLIGHT CONDITIONS");
		break;
	case 29:
		message.push_back("RMMASCND CONVERGENCE LIMIT");
		break;
	case 35:
		message.push_back("C-BAND CONTACT GENERATION " + RTCCONLINEMON.TextBuffer[0]);
		break;
	case 36:
		message.push_back("ANCHOR TIME ADJUSTED FOR" + RTCCONLINEMON.TextBuffer[0] + "STATION CONTACTS");
		break;
	default:
		return;
	}
	EMGPRINT(source, message);
}

void RTCC::EMGPRINT(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::EMMGLCVP(int L, double gmt)
{
	EphemerisDataTable EPHEM;
	ManeuverTimesTable MANTIMES;
	LunarStayTimesTable LUNSTAY;

	if (ELFECH(gmt, 16, 8, L, EPHEM, MANTIMES, LUNSTAY))
	{
		EMGPRINT("EMMGLCVP", 25);
		return;
	}
	ELVCTRInputTable in;
	ELVCTROutputTable out;

	in.GMT = gmt;
	in.L = L;
	in.ORER = 8;

	ELVCTR(in, out);

	if (out.ErrorCode)
	{
		EMGPRINT("EMMGLCVP", 25);
		return;
	}

	if (abs(dotp(unit(out.SV.R), unit(out.SV.V))) > 1.0 - 0.0017)
	{
		EMGPRINT("EMMGLCVP", 26);
		return;
	}

	MATRIX3 REFSMMAT;
	VECTOR3 X_SM, Y_SM, Z_SM;
	if (L == 1)
	{
		Z_SM = unit(-out.SV.R);
		Y_SM = unit(crossp(out.SV.V, out.SV.R));
		X_SM = crossp(Y_SM, Z_SM);
	}
	else
	{
		X_SM = unit(out.SV.R);
		Y_SM = unit(crossp(out.SV.V, out.SV.R));
		Z_SM = crossp(X_SM, Y_SM);
	}
	REFSMMAT = _M(X_SM.x, X_SM.y, X_SM.z, Y_SM.x, Y_SM.y, Y_SM.z, Z_SM.x, Z_SM.y, Z_SM.z);

	EMGSTSTM(L, REFSMMAT, RTCC_REFSMMAT_TYPE_LCV, gmt);
}

void RTCC::PCBBT(double *DELT, double *WDI, double *TU, double W, double TIMP, double DELV, int NPHASE, double &T, double &GMTBB, double &GMTI, double &WA)
{
	//INPUTS:
	//DELT: Array of phase times
	//WDI: Array of weight loss rates
	//TU: Array of thrust levels
	//W: Initial mass
	//TIMP: Impulsive maneuver time
	//DELV: Delta V of maneuver
	//NPHASE: Number of phases

	//OUTPUTS:
	//T: Total time of maneuver
	//GMTBB: Ullage on time
	//GMTI: Main engine on time
	//WA: Weight after maneuver

	double C, D, WD, TT, TH, WDOT, S, WF, A, B, DELVW, DETU;
	int I = 1;
	T = 0.0;
	C = 0.0;
	D = 0.0;
	WD = W;
	DELVW = DELV;
	DETU = DELT[0];
	if (NPHASE == 1)
	{
		DETU = 0.0;
		goto RTCC_PCBBT_2_B;
	}
RTCC_PCBBT_1_A:
	TT = DELT[I - 1];
	TH = TU[I - 1];
	WDOT = WDI[I - 1];
RTCC_PCBBT_1_B:
	S = TH / WDOT;
	WF = WD - WDOT * TT;
	A = S * log(WD / WF);
	B = (WD / WDOT + T)*A - S * TT;
	if (I >= NPHASE)
	{
		goto RTCC_PCBBT_2_C;
	}
	if (DELVW > A)
	{
		goto RTCC_PCBBT_2_C;
	}
RTCC_PCBBT_2_B:
	TH = TU[I - 1];
	WDOT = WDI[I - 1];
	TT = WD / WDOT * (1.0 - exp(-WDOT * DELVW / TH));
	I = NPHASE;
	goto RTCC_PCBBT_1_B;
RTCC_PCBBT_2_C:
	C = C + A;
	D = D + B;
	T = T + TT;
	DELVW = DELVW - A;
	WD = WF;
	I = I + 1;
	if (I < NPHASE)
	{
		goto RTCC_PCBBT_1_A;
	}
	else if (I == NPHASE)
	{
		goto RTCC_PCBBT_2_B;
	}
	GMTBB = TIMP - D / C;
	GMTI = GMTBB + DETU;
	WA = WD;
}

void RTCC::PCMATO(double **A, double *Y, double **B, double *X, int M, int N, double *W1, double lambda, double *W2)
{
	double SUM;
	int I, J, K;

	I = 0;
	do
	{
		J = 0;
		do
		{
			SUM = 0.0;
			K = 0;
			do
			{
				SUM = SUM + B[K][I] * B[K][J] * W1[K];
				K++;
			} while (K < M);
			A[I][J] = SUM;
			J++;
		} while (J <= I);
		I++;
	} while (I < N);
	I = 0;
	do
	{
		J = I + 1;
		do
		{
			A[I][J] = A[J][I];
			J++;
		} while (J < N);
		I++;
	} while (I < N - 1);
	I = 0;
	do
	{
		SUM = 0.0;
		K = 0;
		do
		{
			SUM = SUM + B[K][I] * X[K] * W1[K];
			K++;
		} while (K < M);
		Y[I] = SUM;
		I++;
	} while (I < N);
	I = 0;
	do
	{
		A[I][I] = A[I][I] + lambda * W2[I];
		I++;
	} while (I < N);
}

bool RTCC::PCGAUS(double **A, double *Y, double *X, int N, double eps)
{
	int *PP = new int[N + 1];

	if (OrbMech::LUPDecompose(A, N, 0.0, PP) == 0)
	{
		return true;
	}
	std::vector<double> dx;
	dx.resize(N);
	OrbMech::LUPSolve(A, PP, Y, N, dx);
	delete[] PP;

	for (int i = 0;i < N;i++)
	{
		X[i] = dx[i];
	}

	/*double AMAX, DUMI;
	int I, J, K, *IP, IMAX, JMAX, IDUM;
	IP = new int[N];

	for (I = 0;I < N;I++)
	{
		IP[I] = I;
	}
	for (K = 0;K < N;K++)
	{
		if (K != N - 1)
		{
			IMAX = K;
			AMAX = abs(A[K][K]);
			JMAX = K;
			I = 0;
			do
			{
				//Find maximum element
				J = 0;
				do
				{
					DUMI = abs(A[I][J]);
					if (AMAX < DUMI)
					{
						AMAX = DUMI;
						IMAX = I;
						JMAX = J;
					}
					J++;
				} while (J < N);
				I++;
			} while (I < N);
			if (IMAX != K)
			{
				//Interchange rows
				I = K;
				do
				{
					DUMI = A[IMAX][I];
					A[IMAX][I] = A[K][I];
					A[K][I] = DUMI;
					I++;
				} while (I < N);
				DUMI = Y[IMAX];
				Y[IMAX] = Y[K];
				Y[K] = DUMI;
			}
			if (JMAX != K)
			{
				//Interchange columns
				I = 0;
				do
				{
					DUMI = A[I][K];
					A[I][K] = A[I][JMAX];
					A[I][JMAX] = DUMI;
					I++;
				} while (I < N);
				IDUM = IP[K];
				IP[K] = IP[JMAX];
				IP[JMAX] = IDUM;
			}
		}
		if (abs(A[K][K]) <= eps)
		{
			//error
			return true;
		}

		for (I = K + 1;I < N;I++)
		{
			DUMI = A[I][K] / A[K][K];
			for (J = K + 1;J < N;J++)
			{
				A[I][J] = A[I][J] - DUMI * A[K][J];
			}
			Y[I] = Y[I] - DUMI * Y[K];
		}

		
		//Normalize row
		for (I = K + 1;I < N;I++)
		{
			A[K][I] = A[K][I] / A[K][K];
		}
		Y[K] = Y[K] / A[K][K];
		A[K][K] = 1.0;
		//Reduce matrix
		for (I = 0;I < N;I++)
		{
			if (I != K)
			{
				for (J = K + 1;J < N;J++)
				{
					A[I][J] = A[I][J] - A[I][K] * A[K][J];
				}
				Y[I] = Y[I] - A[I][K] * Y[K];
			}
		}
		
		K++;
	}

	I = 0;
	do
	{
		X[IP[I]] = Y[I];
		I++;
	} while (I < N);

	delete[] IP;
	*/
	return false;
}

void RTCC::PMMDAB(VECTOR3 R, VECTOR3 V, double GMT, ASTInput ARIN, ASTSettings IRIN, ASTData &AST, int &IER, int IPRT)
{
	//Set up inputs for forward iterator
	bool err;
	void *constPtr;
	PCMATCArray outarray;
	outarray.R0 = R;
	outarray.V0 = V;
	outarray.T0 = GMT;
	outarray.REF = IRIN.Ref;
	outarray.IASD = 0;
	outarray.ISTP = IRIN.ReentryStop;
	outarray.gamma_stop = ARIN.gamma_stop;
	if (IRIN.Ref == BODY_EARTH)
	{
		outarray.h_pc_on = false;
	}
	else
	{
		outarray.h_pc_on = true;
	}
	
	constPtr = &outarray;

	bool PCMATCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	bool(*fptr)(void *, std::vector<double>&, void*, std::vector<double>&, bool) = &PCMATCPointer;

	GenIterator::GeneralizedIteratorBlock block;

	block.IndVarSwitch[0] = true;
	block.IndVarSwitch[1] = true;
	block.IndVarSwitch[2] = true;
	block.IndVarSwitch[4] = true;
	block.IndVarGuess[0] = ARIN.dgamma;
	block.IndVarGuess[1] = ARIN.dpsi;
	block.IndVarGuess[2] = ARIN.dv*3600.0 / OrbMech::R_Earth;
	block.IndVarGuess[3] = ARIN.T_a / 3600.0;
	block.IndVarGuess[4] = ARIN.T_r / 3600.0;
	block.IndVarStep[0] = pow(10, -6);
	block.IndVarStep[1] = pow(10, -6);
	block.IndVarStep[2] = 2.0*pow(10, -7);
	block.IndVarWeight[0] = 1.0;
	block.IndVarWeight[1] = 1.0;
	block.IndVarWeight[2] = 1.0;

	block.DepVarSwitch[0] = true;
	block.DepVarSwitch[1] = true;
	block.DepVarSwitch[2] = true;
	block.DepVarSwitch[3] = true;
	block.DepVarSwitch[4] = true;
	if (IRIN.ReentryStop == -1)
	{
		block.DepVarSwitch[5] = true;
	}
	block.DepVarLowerLimit[0] = (ARIN.h_r_des - 0.3*1852.0) / OrbMech::R_Earth;
	block.DepVarLowerLimit[1] = ARIN.lat_r_des - 0.1*RAD;
	block.DepVarLowerLimit[2] = ARIN.lng_r_des - 0.01*RAD;
	block.DepVarLowerLimit[3] = ARIN.azi_r_des - 2.0*RAD;
	block.DepVarLowerLimit[4] = (ARIN.T_r - ARIN.T_a) / 3600.0 - 12.0;
	block.DepVarLowerLimit[5] = ARIN.gamma_des - 0.001*RAD;
	block.DepVarUpperLimit[0] = (ARIN.h_r_des + 0.3*1852.0) / OrbMech::R_Earth;
	block.DepVarUpperLimit[1] = ARIN.lat_r_des + 0.1*RAD;
	block.DepVarUpperLimit[2] = ARIN.lng_r_des + 0.01*RAD;
	block.DepVarUpperLimit[3] = ARIN.azi_r_des + 2.0*RAD;
	block.DepVarUpperLimit[4] = (ARIN.T_r - ARIN.T_a) / 3600.0 + 12.0;
	block.DepVarUpperLimit[5] = ARIN.gamma_des + 0.001*RAD;
	block.DepVarClass[0] = 1;
	block.DepVarClass[1] = 1;
	block.DepVarClass[2] = 1;
	block.DepVarClass[3] = 1;
	block.DepVarClass[4] = 2;
	block.DepVarClass[5] = 1;
	block.DepVarWeight[4] = 1.0;

	std::vector<double> result;
	std::vector<double> y_vals;
	err = GenIterator::GeneralizedIterator(fptr, block, constPtr, (void*)this, result, y_vals);
	if (err)
	{

	}

}

bool PCMATCPointer(void *data, std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	return ((RTCC*)data)->PCMATC(var, varPtr, arr, mode);
}

bool RTCC::PCMATC(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode)
{
	//Independent variables:
	//0: dgamma_a, R_tx or DVX
	//1: dpsi_a, R_ty or DVY
	//2: dv_a, R_tz or DVZ
	//3: T of abort
	//4: T of reentry
	//Dependent variables:
	//0: Height at reentry
	//1: Geodetic latitude at reentry
	//2: Longitude at reentry
	//3: Azimuth at reentry
	//4: Time from abort to reentry
	//5: Gamma at reentry
	//6: Height at pericynthion
	//7: Sigma

	OELEMENTS coe;
	double gamma_p, T_min, h_p = 0.0;

	PCMATCArray *vars = static_cast<PCMATCArray*>(varPtr);

	PMMCEN_VNI vni;
	vni.dt_max = var[3] * 3600.0 - vars->T0;
	vni.GMTBASE = vars->GMTBASE;
	vni.R = vars->R0;
	vni.V = vars->V0;
	vni.T = vars->T0;
	PMMCEN_INI ini;
	ini.body = vars->REF;
	ini.stop_ind = 1;

	VECTOR3 R1, V1;
	double T1;
	int stop_ind, body1;

	OrbMech::PMMCEN(vni, ini, R1, V1, T1, stop_ind, body1);
	if (vars->IASD == 1)
	{
		goto PCMATC_5A;
	}
	double B[9];
	B[0] = 0.0;
	B[1] = 0.0;
	B[2] = var[2] * OrbMech::R_Earth / 3600.0;
	B[3] = 0.0;
	B[4] = var[0];
	B[5] = var[1];
	B[6] = 1.0;

	VECTOR3 R2, V2;
	double T_a;
	PIBURN(R1, V1, T1, B, R2, V2, T_a);
PCMATC_1A:
	if (body1 == BODY_MOON)
	{
		goto PCMATC_2B;
	}
	double dt_ar;
	if (vars->ISTP == -1)
	{
		dt_ar = var[4]*3600.0 - T_a;
		goto PCMATC_3A;
	}
	coe = OrbMech::coe_from_sv(R2, V2, OrbMech::mu_Earth);
	double a = OrbMech::GetSemiMajorAxis(R2, V2, OrbMech::mu_Moon);
	double gamma_min;
	if (coe.e > 1.0)
	{
		gamma_min = -PI05;
	}
	else
	{
		gamma_min = atan2(-coe.e, sqrt(1.0 - coe.e * coe.e));
	}
	if (gamma_min > vars->gamma_stop)
	{
		goto PCMATC_4C;
	}
	gamma_p = atan2(coe.e*sin(coe.TA), 1.0 + coe.e*cos(coe.TA));
	if (coe.e >= 1.0 && gamma_p >= 0.0)
	{
		goto PCMATC_4C;
	}
	double E = OrbMech::TrueToEccentricAnomaly(coe.TA, coe.e);
	if (E >= 3.0*PI05)
	{
		if (gamma_p > vars->gamma_stop)
		{
			goto PCMATC_4C;
		}
		T_min = 0.0;
	}
	else
	{
		T_min = 0.0;
		if (coe.e < 1.0)
		{
			T_min = sqrt(pow(a, 3) / OrbMech::mu_Earth)*((3.0*PI05 - E) + coe.e*(1.0 + sin(E)));
		}
	}
	goto PCMATC_3A;
PCMATC_2B:
	//Is H_PC switched on as a dependant variable?
	if (vars->h_pc_on == true)
	{
		VECTOR3 R_p, V_p;
		double T_p;
		int IRS, ITS;

		gamma_p = asin(dotp(unit(R2), unit(V2)));
		if (gamma_p >= 0.0)
		{
			vni.dir = -1.0;
		}
		else
		{
			vni.dir = 1.0;
		}
		vni.dt_max = 10.0*24.0*3600.0;
		vni.dt_min = 0.0;
		vni.end_cond = 0.0;
		vni.R = R2;
		vni.V = V2;
		vni.T = T_a;
		ini.body = body1;
		ini.stop_ind = 2;
		OrbMech::PMMCEN(vni, ini, R_p, V_p, T_p, ITS, IRS);
		h_p = length(R_p) - MCSMLR;
	}
	T_min = 0.0;
PCMATC_3A:
	vni.dir = 1.0;
	vni.dt_max = 10.0*24.0*3600.0;
	if (vars->ISTP == -1)
	{
		vni.end_cond = 0.0;
		vni.dt_min = 0.0;
		vni.dt_max = T_a;
		ini.stop_ind = 1;
	}
	else
	{
		vni.dt_min = T_min;
		vni.dt_max = 10.0*24.0*3600.0;
		vni.end_cond = vars->gamma_stop;
		ini.stop_ind = 2;
	}
	vni.R = R2;
	vni.V = V2;
	vni.T = T_a;
	ini.body = body1;
	VECTOR3 R_r, V_r;
	double T_r;
	int ref_r;
	OrbMech::PMMCEN(vni, ini, R_r, V_r, T_r, stop_ind, ref_r);

	if (ref_r == BODY_MOON || (vars->ISTP == 1 && stop_ind != 2))
	{
		goto PCMATC_4C;
	}
	MATRIX3 Rot = OrbMech::GetRotationMatrix(BODY_EARTH, OrbMech::MJDfromGET(T_r, vars->GMTBASE));
	VECTOR3 R_r_equ = rhtmul(Rot, R_r);
	VECTOR3 V_r_equ = rhtmul(Rot, V_r);
	double h_r = length(R_r) - OrbMech::R_Earth;
	double r_r, v_r, lat_r, lng_r, gamma_r, azi_r;
	PICSSC(true, R_r, V_r, r_r, v_r, lat_r, lng_r, gamma_r, azi_r);
	double T_ar = T_r - T_a;

	arr[0] = h_r / OrbMech::R_Earth;
	arr[1] = lat_r;
	arr[2] = lng_r;
	arr[3] = azi_r;
	arr[4] = T_ar / 3600.0;
	arr[5] = gamma_r;
	arr[6] = h_p / OrbMech::R_Earth;

	vars->R_r = R_r;
	vars->V_r = V_r;
	vars->T_r = T_r;
	return true;
PCMATC_4C:
	return false;
PCMATC_5A:
	PMMRKJInputArray integin;

	integin.A = 0.0;
	integin.DENSMULT = 1.0;
	integin.DOCKANG = vars->DockingAngle;
	integin.DPSScale = MCTDTF;
	integin.DTOUT = 10.0;
	integin.DTPS10 = vars->DT_10PCT;
	integin.DTU = vars->dt_ullage;
	integin.HeadsUpDownInd = vars->HeadsUpDownInd;
	integin.IC = vars->ConfigCode;
	integin.KAUXOP = 1;
	integin.KEPHOP = 0;
	integin.KTRIMOP = vars->TrimAngleInd;
	integin.LMDESCJETT = 1e70;
	integin.MANOP = vars->AttitudeCode;

	integin.sv0.R = R2;
	integin.sv0.V = V2;
	integin.sv0.GMT = T_a;
	integin.sv0.RBI = body1;
	integin.ThrusterCode = vars->ThrusterCode;
	integin.TVC = vars->TVC;
	integin.UllageOption = vars->UllageCode;
	integin.ExtDVCoordInd = true;
	integin.VG = _V(var[0], var[1], var[2])*OrbMech::R_Earth / 3600.0;

	integin.WDMULT = 1.0;
	integin.CAPWT = vars->CSMWeight + vars->LMWeight;
	integin.CSMWT = vars->CSMWeight;
	integin.LMAWT = 0.0;
	integin.LMDWT = vars->LMWeight;

	int Ierr;

	RTCCNIAuxOutputTable AuxTable;
	CSMLMPoweredFlightIntegration numin(this, integin, Ierr, NULL, &AuxTable);
	numin.PMMRKJ();
	R2 = AuxTable.R_BO;
	V2 = AuxTable.V_BO;
	T_a = AuxTable.GMT_BO;
	goto PCMATC_1A;
}

void RTCC::FDOLaunchAnalog1(MPTSV sv)
{
	fdolaunchanalog1tab.LastUpdateTime = oapiGetSimTime();

	double xval = 0.0;
	double yval = 0.0;

	if (sv.gravref != oapiGetObjectByName("Earth"))
	{
		return;
	}

	double r_apo, r_peri;
	OrbMech::periapo(sv.R, sv.V, OrbMech::mu_Earth, r_apo, r_peri);

	if (r_peri > OrbMech::R_Earth + 50.0*1852.0 || length(sv.R) < OrbMech::R_Earth + 100.0)
	{
		return;
	}

	double v_fps = length(sv.V) / 0.3048;
	double gamma_deg = asin(dotp(unit(sv.R), unit(sv.V)))*DEG;

	if (v_fps < 5500.0)
	{
		if (v_fps < 0)
		{
			v_fps = 0.0;
		}
		xval = v_fps / 6000.0;
		
		if (gamma_deg < 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg > 40.0)
		{
			gamma_deg = 40.0;
		}
		yval = gamma_deg / 40.0;
	}
	else if (v_fps < 22000.0)
	{
		xval = (v_fps - 5000.0) / (23000.0 - 5000.0);

		if (gamma_deg > 35.0)
		{
			gamma_deg = 35.0;
		}
		else if (gamma_deg < -5.0)
		{
			gamma_deg = -5.0;
		}
		yval = (gamma_deg + 5.0) / (35.0 + 5.0);
	}
	else if (v_fps < 27000.0)
	{
		xval = (v_fps - 21000.0) / (27000.0 - 21000.0);

		if (gamma_deg > 2.0)
		{
			gamma_deg = 2.0;
		}
		else if (gamma_deg < -2.0)
		{
			gamma_deg = -2.0;
		}
		yval = (gamma_deg + 2.0) / (2.0 + 2.0);
	}
	else
	{
		return;
	}

	yval = 1.0 - yval;

	double xscalmin = 0.15;
	double xscalmax = 0.95;
	double yscalmin = 0.15;
	double yscalmax = 0.85;

	xval = xval * (xscalmax - xscalmin) + xscalmin;
	yval = yval * (yscalmax - yscalmin) + yscalmin;

	fdolaunchanalog1tab.XVal.push_back(xval);
	fdolaunchanalog1tab.YVal.push_back(yval);
}

void RTCC::FDOLaunchAnalog2(MPTSV sv)
{
	fdolaunchanalog2tab.LastUpdateTime = oapiGetSimTime();

	double r_EI = OrbMech::R_Earth + 400000.0*0.3048;
	double xval = 0.0;
	double yval = 0.0;

	if (sv.gravref != oapiGetObjectByName("Earth"))
	{
		return;
	}

	double r_apo, r_peri;
	OrbMech::periapo(sv.R, sv.V, OrbMech::mu_Earth, r_apo, r_peri);

	if (r_peri > r_EI || r_apo < r_EI)
	{
		return;
	}

	double dt = OrbMech::time_radius(sv.R, sv.V, r_EI, -1.0, OrbMech::mu_Earth);
	VECTOR3 R_EI, V_EI;
	OrbMech::rv_from_r0v0(sv.R, sv.V, dt, R_EI, V_EI, OrbMech::mu_Earth);

	double v_fps = length(V_EI) / 0.3048;
	double gamma_deg = asin(dotp(unit(R_EI), unit(V_EI)))*DEG;

	if (v_fps < 22750.0)
	{
		if (v_fps < 6000.0)
		{
			v_fps = 6000.0;
		}
		xval = (v_fps - 6000.0) / (23000.0 - 6000.0);

		if (gamma_deg > 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg < -36.0)
		{
			gamma_deg = -36.0;
		}
		yval = -gamma_deg / 36.0;
	}
	else if (v_fps < 26250.0)
	{
		xval = (v_fps - 22500.0) / (26750.0 - 22500.0);

		if (gamma_deg > 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg < -12.0)
		{
			gamma_deg = -12.0;
		}
		yval = -gamma_deg / 12.0;
	}
	else if (v_fps < 33500.0)
	{
		xval = (v_fps - 25000.0) / (33500.0 - 25000.0);

		if (gamma_deg > 0.0)
		{
			gamma_deg = 0.0;
		}
		else if (gamma_deg < -12.0)
		{
			gamma_deg = -12.0;
		}
		yval = -gamma_deg / 12.0;
	}
	else
	{
		return;
	}

	double xscalmin = 0.15;
	double xscalmax = 0.95;
	double yscalmin = 0.15;
	double yscalmax = 0.85;

	xval = xval * (xscalmax - xscalmin) + xscalmin;
	yval = yval * (yscalmax - yscalmin) + yscalmin;

	fdolaunchanalog2tab.XVal.push_back(xval);
	fdolaunchanalog2tab.YVal.push_back(yval);
}

void RTCC::PMDTRDFF(int med, unsigned page)
{
	//To figure out how to format the display, search for the largest DV and the maximum time difference

	unsigned i, j, p;
	double maxdv = 0;
	double MinTime = (RTETradeoffTable.data[0][0].T0 - MCGMTL);
	double MaxTime = 0;

	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			if (RTETradeoffTable.data[i][j].DV > maxdv)
			{
				maxdv = RTETradeoffTable.data[i][j].DV;
			}
			if ((RTETradeoffTable.data[i][j].T0 - MCGMTL) > MaxTime)
			{
				MaxTime = (RTETradeoffTable.data[i][j].T0 - MCGMTL);
			}
		}
	}

	//Origin of x-axis should be at least below the minimum time
	double MinTimeLabel = floor(MinTime);
	//If the minimum time was very close to the previous hour, go back even one more hour
	if (MinTime - MinTimeLabel < 0.1)
	{
		MinTimeLabel -= 1.0;
	}
	//End of x-axis should be at least above the maximum time
	double MaxTimeLabel = ceil(MaxTime);
	//Difference between minimum and maximum time
	int DeltaTimeLabel = (int)(MaxTimeLabel - MinTimeLabel);
	//Enforce even number of hours on scale, so that the middle value is also a full hour
	if (DeltaTimeLabel % 2 != 0)
	{
		MaxTimeLabel += 1.0;
	}
	//If the maximum time was very close to the next hour, go up even one more hour
	else if (MaxTimeLabel - MaxTime < 0.1)
	{
		MaxTimeLabel += 1.0;
	}

	double MidTimeLabel = (MaxTimeLabel + MinTimeLabel) / 2.0;

	double MinDVLabel = 0;
	double MaxDVLabel = ceil(maxdv / 1000.0)*1000.0;
	int DeltaDVLabel = (int)((MaxDVLabel - MinDVLabel) / 1000.0);
	if (DeltaDVLabel % 2 != 0)
	{
		MaxDVLabel += 1000.0;
	}

	double MidDVLabel = (MaxDVLabel + MinDVLabel) / 2.0;

	double xscalmin = 0.1;
	double xscalmax = 0.95;
	double yscalmin = 0.1;
	double yscalmax = 0.85;
	double xval, yval;

	if (med == 70)
	{
		p = 0;
	}
	else
	{
		p = page - 1;
	}

	RTETradeoffTableBuffer[p].curves = RTETradeoffTable.curves;
	RTETradeoffTableBuffer[p].XLabels[0] = (int)MinTimeLabel;
	RTETradeoffTableBuffer[p].XLabels[1] = (int)MidTimeLabel;
	RTETradeoffTableBuffer[p].XLabels[2] = (int)MaxTimeLabel;
	RTETradeoffTableBuffer[p].YLabels[0] = (int)MinDVLabel / 1000;
	RTETradeoffTableBuffer[p].YLabels[1] = (int)MidDVLabel / 1000;
	RTETradeoffTableBuffer[p].YLabels[2] = (int)MaxDVLabel / 1000;

	//Set to -1, will not display the unused curves
	for (i = 0;i < 10;i++)
	{
		RTETradeoffTableBuffer[p].TZDisplay[i] = -1;
	}
	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		RTETradeoffTableBuffer[p].NumInCurve[i] = RTETradeoffTable.NumInCurve[i];

		//Rounded average of landing time
		RTETradeoffTableBuffer[p].TZDisplay[i] = (int)round(((RTETradeoffTable.data[i][0].T_Z - MCGMTL) + (RTETradeoffTable.data[i][RTETradeoffTable.NumInCurve[i] - 1].T_Z - MCGMTL)) / 2.0);

		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			xval = ((RTETradeoffTable.data[i][j].T0 - MCGMTL) - MinTimeLabel) / (MaxTimeLabel - MinTimeLabel);
			yval = (RTETradeoffTable.data[i][j].DV - MinDVLabel) / (MaxDVLabel - MinDVLabel);

			xval = xval * (xscalmax - xscalmin) + xscalmin;
			yval = yval * (yscalmax - yscalmin) + yscalmin;
			yval = 1.0 - yval;

			RTETradeoffTableBuffer[p].xval[i][j] = xval;
			RTETradeoffTableBuffer[p].yval[i][j] = yval;
		}

		RTETradeoffTableBuffer[p].TZxval[i] = min(xscalmax, xval + 0.01);
		RTETradeoffTableBuffer[p].TZyval[i] = yval;
	}

	RTETradeoffTableBuffer[p].XAxisName = "Time of abort, hr";
	RTETradeoffTableBuffer[p].YAxisName = "Abort velocity, kfps";
	RTETradeoffTableBuffer[p].Site = PZREAP.RTESite;

	//If remote Earth tradeoff then we are done
	if (med == 71)
	{
		return;
	}

	//Now latitude and time of landing for near-Earth tradeoff

	//Unused for time of landing, same for latitude
	for (i = 0;i < 10;i++)
	{
		RTETradeoffTableBuffer[1].TZDisplay[i] = -1;
		RTETradeoffTableBuffer[2].TZDisplay[i] = RTETradeoffTableBuffer[0].TZDisplay[i];
	}

	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			//X-values are the same
			RTETradeoffTableBuffer[2].xval[i][j] = RTETradeoffTableBuffer[1].xval[i][j] = RTETradeoffTableBuffer[0].xval[i][j];
		}
	}

	double mintz = 10e10;
	double maxtz = 0;
	double minlat = 10e10;
	double maxlat = 0;

	//Find min and max values
	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			if ((RTETradeoffTable.data[i][j].T_Z - MCGMTL) > maxtz)
			{
				maxtz = (RTETradeoffTable.data[i][j].T_Z - MCGMTL);
			}
			if ((RTETradeoffTable.data[i][j].T_Z - MCGMTL) < mintz)
			{
				mintz = (RTETradeoffTable.data[i][j].T_Z - MCGMTL);
			}

			if (RTETradeoffTable.data[i][j].lat > maxlat)
			{
				maxlat = RTETradeoffTable.data[i][j].lat;
			}
			if (RTETradeoffTable.data[i][j].lat < minlat)
			{
				minlat = RTETradeoffTable.data[i][j].lat;
			}
		}
	}

	double MinTZLabel = floor(mintz);
	if (mintz - MinTZLabel < 0.1)
	{
		MinTZLabel -= 1.0;
	}
	double MaxTZLabel = ceil(maxtz);
	if (MaxTZLabel - maxtz < 0.1)
	{
		MaxTZLabel += 1.0;
	}

	int DeltaTZLabel = (int)(MaxTZLabel - MinTZLabel);
	if (DeltaTZLabel % 2 != 0)
	{
		MaxTZLabel += 1.0;
	}

	double MidTZLabel = (MaxTZLabel + MinTZLabel) / 2.0;

	RTETradeoffTableBuffer[1].curves = RTETradeoffTable.curves;
	RTETradeoffTableBuffer[1].XLabels[0] = (int)MinTimeLabel;
	RTETradeoffTableBuffer[1].XLabels[1] = (int)MidTimeLabel;
	RTETradeoffTableBuffer[1].XLabels[2] = (int)MaxTimeLabel;
	RTETradeoffTableBuffer[1].YLabels[0] = (int)MinTZLabel;
	RTETradeoffTableBuffer[1].YLabels[1] = (int)MidTZLabel;
	RTETradeoffTableBuffer[1].YLabels[2] = (int)MaxTZLabel;

	double MinlatLabel = floor(minlat);
	if (minlat - MinlatLabel < 0.1)
	{
		MinlatLabel -= 1.0;
	}
	double MaxlatLabel = ceil(maxlat);
	if (MaxlatLabel - maxlat < 0.1)
	{
		MaxlatLabel += 1.0;
	}

	int DeltalatLabel = (int)(MaxlatLabel - MinlatLabel);
	if (DeltalatLabel % 2 != 0)
	{
		MaxlatLabel += 1.0;
	}

	double MidlatLabel = (MaxlatLabel + MinlatLabel) / 2.0;

	RTETradeoffTableBuffer[2].curves = RTETradeoffTable.curves;
	RTETradeoffTableBuffer[2].XLabels[0] = (int)MinTimeLabel;
	RTETradeoffTableBuffer[2].XLabels[1] = (int)MidTimeLabel;
	RTETradeoffTableBuffer[2].XLabels[2] = (int)MaxTimeLabel;
	RTETradeoffTableBuffer[2].YLabels[0] = (int)MinlatLabel;
	RTETradeoffTableBuffer[2].YLabels[1] = (int)MidlatLabel;
	RTETradeoffTableBuffer[2].YLabels[2] = (int)MaxlatLabel;

	for (i = 0;i < RTETradeoffTable.curves;i++)
	{
		RTETradeoffTableBuffer[1].NumInCurve[i] = RTETradeoffTable.NumInCurve[i];
		RTETradeoffTableBuffer[2].NumInCurve[i] = RTETradeoffTable.NumInCurve[i];

		for (j = 0;j < RTETradeoffTable.NumInCurve[i];j++)
		{
			//Landing time
			yval = ((RTETradeoffTable.data[i][j].T_Z - MCGMTL) - MinTZLabel) / (MaxTZLabel - MinTZLabel);

			yval = yval * (yscalmax - yscalmin) + yscalmin;
			yval = 1.0 - yval;

			RTETradeoffTableBuffer[1].yval[i][j] = yval;

			//Latitude
			yval = (RTETradeoffTable.data[i][j].lat - MinlatLabel) / (MaxlatLabel - MinlatLabel);

			yval = yval * (yscalmax - yscalmin) + yscalmin;
			yval = 1.0 - yval;

			RTETradeoffTableBuffer[2].yval[i][j] = yval;
		}

		RTETradeoffTableBuffer[2].TZxval[i] = RTETradeoffTableBuffer[0].TZxval[i];
		RTETradeoffTableBuffer[2].TZyval[i] = yval;
	}

	RTETradeoffTableBuffer[1].XAxisName = "Time of abort, hr";
	RTETradeoffTableBuffer[1].YAxisName = "Time of landing, hr";
	RTETradeoffTableBuffer[1].Site = PZREAP.RTESite;

	RTETradeoffTableBuffer[2].XAxisName = "Time of abort, hr";
	RTETradeoffTableBuffer[2].YAxisName = "Latitude of landing, deg";
	RTETradeoffTableBuffer[2].Site = PZREAP.RTESite;
}

//Mission Plan Table Display
void RTCC::PMDMPT()
{
	MPTDISPLAY.man.clear();

	char Buffer[100];
	double temp;
	unsigned csmnum = PZMPTCSM.mantable.size();
	unsigned lemnum = PZMPTLEM.mantable.size();
	unsigned i = 0, j = 0;

	MPTDISPLAY.CSMSTAID = PZMPTCSM.StationID;
	MPTDISPLAY.LEMSTAID = PZMPTLEM.StationID;

	if (EZANCHR1.AnchorVectors[9].GMT == 0.0)
	{
		MPTDISPLAY.CSMGETAV = "";
	}
	else
	{
		temp = GETfromGMT(EZANCHR1.AnchorVectors[9].GMT);
		OrbMech::format_time_HHMMSS(Buffer, temp);
		std::string strtemp(Buffer);
		MPTDISPLAY.CSMGETAV = strtemp;
	}

	if (EZANCHR3.AnchorVectors[9].GMT == 0.0)
	{
		MPTDISPLAY.LEMGETAV = "";
	}
	else
	{
		temp = GETfromGMT(EZANCHR3.AnchorVectors[9].GMT);
		OrbMech::format_time_HHMMSS(Buffer, temp);
		std::string strtemp(Buffer);
		MPTDISPLAY.LEMGETAV = strtemp;
	}

	if (csmnum == 0 && lemnum == 0) return;

	MPTManDisplay man;
	MPTManeuver *mptman;
	double mu, r, dt;
	bool isManeuverExecuted;

	while (i < csmnum || j < lemnum)
	{
		if (j >= lemnum || (i < csmnum && PZMPTCSM.mantable[i].GMT_BI < PZMPTLEM.mantable[j].GMT_BI))
		{
			mptman = &PZMPTCSM.mantable[i];
			isManeuverExecuted = (i + 1 <= PZMPTCSM.LastExecutedManeuver);
			i++;
		}
		else
		{
			mptman = &PZMPTLEM.mantable[j];
			isManeuverExecuted = (j + 1 <= PZMPTLEM.LastExecutedManeuver);
			j++;
		}

		man.DELTAV = mptman->DV_M / 0.3048;

		if (mptman->RefBodyInd == BODY_EARTH)
		{
			mu = OrbMech::mu_Earth;
			r = OrbMech::R_Earth;
		}
		else
		{
			mu = OrbMech::mu_Moon;
			r = MCSMLR;
		}

		man.HA = mptman->h_a / 1852.0;
		if (man.HA > 9999.9)
		{
			man.HA = 9999.9;
		}
		man.HP = mptman->h_p / 1852.0;
		if (man.HP > 9999.9)
		{
			man.HP = 9999.9;
		}

		if (MPTDISPLAY.man.size() > 0)
		{
			dt = mptman->GMT_BI - temp;
			OrbMech::format_time_HHMMSS(Buffer, round(dt));
			std::string strtemp3(Buffer);
			man.DT = strtemp3;
		}

		temp = mptman->GMT_BO;

		OrbMech::format_time_HHMMSS(Buffer, round(GETfromGMT(mptman->GMT_BI)));
		std::string strtemp4(Buffer);
		man.GETBI = strtemp4;
		//Maneuver executed?
		if (isManeuverExecuted)
		{
			man.code = "E" + mptman->code;
		}
		//Maneuver frozen?
		else if (mptman->FrozenManeuverInd)
		{
			man.code = "F" + mptman->code;
		}
		else
		{
			man.code = mptman->code;
		}
		man.DVREM = mptman->DVREM / 0.3048;

		MPTDISPLAY.man.push_back(man);
	}
}

void RTCC::PMDTIMP()
{
	TwoImpMultDispBuffer = TwoImpulseMultipleSolutionDisplay();

	if (PZTIPREG.Solutions == 0)
	{
		if (PZTIPREG.Updating)
		{
			TwoImpMultDispBuffer.ErrorMessage = "TABLE BEING UPDATED";
			return;
		}
		else
		{
			TwoImpMultDispBuffer.ErrorMessage = "NO TWO IMPULSE PLANS AVAILABLE";
			return;
		}
	}

	TwoImpMultDispBuffer.CSMSTAID = PZTIPREG.CSMSTAID;
	TwoImpMultDispBuffer.LMSTAID = PZTIPREG.LMSTAID;
	if (PZTIPREG.MAN_VEH == 1)
	{
		TwoImpMultDispBuffer.MAN_VEH = "CSM";
	}
	else
	{
		TwoImpMultDispBuffer.MAN_VEH = "LM";
	}

	if (PZTIPREG.IVFLAG == 2)
	{
		TwoImpMultDispBuffer.GETFRZ = "2";
		TwoImpMultDispBuffer.GMTFRZ = "2";
		TwoImpMultDispBuffer.GETVAR = "1";
		TwoImpMultDispBuffer.GET1 = GETfromGMT(PZTIPREG.data[0].Time2);
		TwoImpMultDispBuffer.GMT1 = PZTIPREG.data[0].Time2;
	}
	else
	{
		TwoImpMultDispBuffer.GETFRZ = "1";
		TwoImpMultDispBuffer.GMTFRZ = "1";
		TwoImpMultDispBuffer.GETVAR = "2";
		TwoImpMultDispBuffer.GET1 = GETfromGMT(PZTIPREG.data[0].Time1);
		TwoImpMultDispBuffer.GMT1 = PZTIPREG.data[0].Time1;
	}

	if (PZTIPREG.IVFLAG == 0)
	{
		TwoImpMultDispBuffer.OPTION = "BOTH FIXED";
	}
	else if (PZTIPREG.IVFLAG == 1)
	{
		TwoImpMultDispBuffer.OPTION = "FIRST FIXED";
	}
	else
	{
		TwoImpMultDispBuffer.OPTION = "SECOND FIXED";
	}

	TwoImpMultDispBuffer.WT = GZGENCSN.TITravelAngle*DEG;
	TwoImpMultDispBuffer.PHASE = GZGENCSN.TIPhaseAngle*DEG;
	TwoImpMultDispBuffer.DH = GZGENCSN.TIDeltaH / 1852.0;

	TwoImpMultDispBuffer.Solutions = PZTIPREG.Solutions;
	TwoImpMultDispBuffer.showTPI = PZTIPREG.showTPI;
	for (int i = 0;i < PZTIPREG.Solutions;i++)
	{
		TwoImpMultDispBuffer.data[i].DELV1 = PZTIPREG.data[i].DELV1 / 0.3048;
		TwoImpMultDispBuffer.data[i].YAW1 = PZTIPREG.data[i].YAW1*DEG;
		TwoImpMultDispBuffer.data[i].PITCH1 = PZTIPREG.data[i].PITCH1*DEG;
		if (PZTIPREG.IVFLAG == 2)
		{
			TwoImpMultDispBuffer.data[i].Time2 = GETfromGMT(PZTIPREG.data[i].Time1);
		}
		else
		{
			TwoImpMultDispBuffer.data[i].Time2 = GETfromGMT(PZTIPREG.data[i].Time2);
		}
		TwoImpMultDispBuffer.data[i].DELV2 = PZTIPREG.data[i].DELV2 / 0.3048;
		TwoImpMultDispBuffer.data[i].YAW2 = PZTIPREG.data[i].YAW2*DEG;
		TwoImpMultDispBuffer.data[i].PITCH2 = PZTIPREG.data[i].PITCH2*DEG;
		if (TwoImpMultDispBuffer.showTPI)
		{
			TwoImpMultDispBuffer.data[i].T_TPI = GETfromGMT(PZTIPREG.data[i].T_TPI);
		}
		TwoImpMultDispBuffer.data[i].L = 'N';
		TwoImpMultDispBuffer.data[i].C = i + 1;
	}
}

void RTCC::PMDRET()
{
	PZREDT = RendezvousEvaluationDisplay();

	bool solns = false;
	for (int i = 0;i < 7;i++)
	{
		if (PZDKIT.Block[i].PlanStatus != 0)
		{
			solns = true;
			break;
		}
	}

	if (solns == false)
	{
		//"No Plans" message
		PZREDT.ErrorMessage = "No Plans";
		return;
	}

	int plan = EZETVMED.RETPlan;

	if (PZDKIT.Block[plan - 1].PlanStatus <= 0)
	{
		// "No Plans" message
		PZREDT.ErrorMessage = "No Plans";
		return;
	}
	DKIDataBlock *block = &PZDKIT.Block[plan - 1];
	PZREDT.ID = plan;
	if (block->PlanStatus == 1)
	{
		PZREDT.isDKI = true;
		PZREDT.M = block->Plan_M;
	}
	else
	{
		PZREDT.isDKI = false;
	}

	PZREDT.NumMans = block->NumMan;
	for (int i = 0;i < PZREDT.NumMans;i++)
	{
		PZREDT.GET[i] = GETfromGMT(block->Display[i].ManGMT);
		if (i > 0)
		{
			PZREDT.DT[i] = block->Display[i].ManGMT - block->Display[i-1].ManGMT;
		}
		PZREDT.DV[i] = block->Display[i].dv / 0.3048;
		if (block->Display[i].VEH == 1)
		{
			PZREDT.VEH[i] = "CSM";
		}
		else
		{
			PZREDT.VEH[i] = "LEM";
		}
		PZREDT.PURP[i] = block->Display[i].Man_ID;
		PZREDT.CODE[i] = 0.0;
		PZREDT.PHASE[i] = block->Display[i].PhaseAngle*DEG;
		PZREDT.HEIGHT[i] = block->Display[i].DH / 1852.0;
		PZREDT.HA[i] = block->Display[i].HA / 1852.0;
		PZREDT.HP[i] = block->Display[i].HP / 1852.0;
		PZREDT.Pitch[i] = block->Display[i].Pitch*DEG;
		PZREDT.Yaw[i] = block->Display[i].Yaw*DEG;
		PZREDT.DVVector[i] = block->Display[i].DV_LVLH / 0.3048;
	}
}

void RTCC::PMDRPT()
{

}

int RTCC::ThrusterNameToCode(std::string thruster)
{
	if (thruster == "C1") return 1;
	else if (thruster == "C2") return 2;
	else if (thruster == "C3") return 3;
	else if (thruster == "C4") return 4;
	else if (thruster == "BV") return 16;
	else if (thruster == "L1") return 17;
	else if (thruster == "L2") return 18;
	else if (thruster == "L3") return 19;
	else if (thruster == "L4") return 20;
	else if (thruster == "S") return 33;
	else if (thruster == "A") return 34;
	else if (thruster == "D") return 35;
	else if (thruster == "J") return 36;
	else if (thruster == "AU") return 37;

	return 0;
}

int RTCC::AttitudeNameToCode(std::string attitude)
{
	if (attitude == "I") return 1;
	else if (attitude == "M") return 2;
	else if (attitude == "L") return 3;
	else if (attitude == "EP") return 4;
	else if (attitude == "EA") return 5;
	else if (attitude == "AP") return 6;
	else if (attitude == "AA") return 7;

	return 0;
}

bool RTCC::RTEManeuverCodeLogic(char *code, double csmmass, double lmascmass, double lmdscmass, int &thruster, double &manmass)
{
	if (code[0] == 'C')
	{
		if (code[1] == 'S')
		{
			thruster = RTCC_ENGINETYPE_CSMSPS;
		}
		else if (code[1] == 'R')
		{
			thruster = RTCC_ENGINETYPE_CSMRCSPLUS4;
		}
		else
		{
			return true;
		}
		if (code[2] == 'D')
		{
			manmass = csmmass + lmascmass + lmdscmass;
		}
		else if (code[2] == 'A')
		{
			manmass = csmmass + lmascmass;
		}
		else if (code[2] == 'U')
		{
			manmass = csmmass;
		}
		else
		{
			return true;
		}
	}
	else if (code[0] == 'L')
	{
		if (code[1] == 'D')
		{
			thruster = RTCC_ENGINETYPE_LMDPS;
			if (code[2] == 'D')
			{
				manmass = csmmass + lmascmass + lmdscmass;
			}
			else
			{
				return true;
			}
		}
		else if (code[1] == 'R')
		{
			thruster = RTCC_ENGINETYPE_LMRCSPLUS4;
			if (code[2] == 'D')
			{
				manmass = csmmass + lmascmass + lmdscmass;
			}
			else if (code[2] == 'A')
			{
				manmass = csmmass + lmascmass;
			}
			else
			{
				return true;
			}
		}
		else
		{
			return true;
		}
	}
	else
	{
		return true;
	}

	return false;
}

void RTCC::GIMGBL(double CSMWT, double LMWT, double &RY, double &RZ, double &T, double &WDOT, int ITC, unsigned &IC, int IA, int IJ, double D)
{
	//INPUTS:
	//CSMWT and LMWT, CSM and LM weights
	//ITC: 33 = SPS, 34 = APS, 35 = DPS, 36 = J2
	//IC: 1 = CSM, 12 = LM, 13 = CSM and LM (docked)
	//IA: -1: trim angles, thrust and weight loss rate desired outputs, 0: thrust and weight loss rate desired outputs, 1: trim angles desired outputs
	//IJ: LM descent stage included in configuration at beginning of this maneuver (only applicable if ITC=33 and IC=2). 0 = included, 1 = not included

	if (ITC < 33)
	{
		RY = 0.0;
		RZ = 0.0;
		return;
	}

	//DPS engine gimbal plane
	static const double K1 = 154.0*0.0254;
	//SPS engine gimbal plane
	static const double K2 = 833.2*0.0254;
	//Distance between SPS and DPS gimbal planes
	static const double K3 = 13.2;//435.55*0.0254;
	//Yaw
	static const double K4 = -0.0829031395;
	//Pitch
	static const double K5 = 0.1876228946;

	/*x1 = LMmass / (CSMmass + LMmass)*6.2;
	p_T = atan2(-2.15 * RAD * 5.0, 5.0 + x1);
	y_T = atan2(0.95 * RAD * 5.0, 5.0 + x1);*/

	int IND;
	double R, W[3];
	VECTOR3 XCG[2], XI, K;

	//CSM only
	if (IC == 1)
	{
		W[0] = CSMWT;
		W[1] = 0.0;
	}
	//LM only
	else if (IC == 12)
	{
		W[0] = 0.0;
		W[1] = LMWT;
	}
	//Both
	else
	{
		W[0] = CSMWT;
		W[1] = LMWT;
	}
	W[2] = W[0] + W[1];

	if (ITC == 34)
	{
		goto RTCC_GIMGBL_LABEL_3_3;
	}
	else if (ITC > 34)
	{
		if (IA == 0)
		{
			goto RTCC_GIMGBL_LABEL_3_5;
		}
		IND = 2;
		K = _V(K1, 0, 0);
		//Use LM DSC CG Table
		XI = GIMGB2(LMDSCCGTAB.Weight, LMDSCCGTAB.CG, LMDSCCGTAB.N, W[1]);		
	}
	else
	{
		if (IA == 0)
		{
			goto RTCC_GIMGBL_LABEL_3_4;
		}
		if (IC == 1)
		{
			IND = 2;
		}
		else
		{
			IND = 1;
		}
		K = _V(K2, K4, K5);
		//Use CSM CG Table
		//GIMGB2();
		XI = _V(26.16328, 0.0, 0.0);
	}

	XCG[IND - 1] = XI - K;

	//CSM or LM, but not docked?
	if (IC <= 12)
	{
		goto RTCC_GIMGBL_LABEL_3_2;
	}
	if (IND > 1)
	{
		//Use CSM CG Table
		//GIMGB2();
		IND = 1;
		K = _V(K2 + K3, 0, 0);
		XI = _V(26.16328, 0.0, 0.0);
	}
	else
	{
		if (IJ != 0)
		{
			//Use LM w/o descent CG table
			//GIMGB2();
			XI = _V(7.6616, 0, 0);
		}
		else
		{
			//Use LM w/ descent CG table
			XI = GIMGB2(LMDSCCGTAB.Weight, LMDSCCGTAB.CG, LMDSCCGTAB.N, W[1]);
		}
		IND = 2;
		K = _V(K1 + K3, 0, 0);
	}

	XI = XI - K;

	XCG[IND - 1] = mul(_M(-1.0, 0.0, 0.0, 0.0, -cos(240.0*RAD - D), -sin(240.0*RAD - D), 0.0, -sin(240.0*RAD - D), cos(240.0*RAD - D)), XI);
	//Remove this when SPS is centered again
	if (ITC == 33)
	{
		XCG[IND - 1] -= _V(0, K4, K5);
	}
	XCG[1] = (XCG[0] * W[0] + XCG[1] * W[1]) / W[2];

RTCC_GIMGBL_LABEL_3_2:
	R = length(XCG[1]);
	if (R > 10e-6)
	{
		RZ = asin(XCG[1].y / R);
		if (XCG[1].x > 10e-6)
		{
			RY = atan(XCG[1].z / XCG[1].x);
		}
		else
		{
			RY = 0.0;
		}
	}
	else
	{
		RZ = 0.0;
		RY = 0.0;
	}
	if (IA == 1)
	{
		return;
	}
	if (ITC == 34)
	{
	RTCC_GIMGBL_LABEL_3_3:;
		//GIMGB2();
	}
	else if (ITC < 34)
	{
	RTCC_GIMGBL_LABEL_3_4:;
		//GIMGB2();
	}
	else
	{
	RTCC_GIMGBL_LABEL_3_5:;
		//GIMGB2();
	}
	//T = XI.x;
	//WDOT = XI.y;
}

VECTOR3 RTCC::GIMGB2(const double *WArr, const VECTOR3 *VecArr, int N, double W)
{
	VECTOR3 XI;
	int I;
	if (W <= WArr[0])
	{
		I = 1;
	RTCC_GIMGB2_4:
		XI = VecArr[I - 1];
		goto RTCC_GIMGB2_2;
	}
	if (W >= WArr[N - 1])
	{
		goto RTCC_GIMGB2_1;
	}
	I = 2;
RTCC_GIMGB2_3:
	if (W < WArr[I - 1])
	{
		//Linearly interpolate
		XI = VecArr[I - 2] + (VecArr[I - 1] - VecArr[I - 2]) / (WArr[I - 1] - WArr[I - 2])*(W - WArr[I - 2]);
		goto RTCC_GIMGB2_2;
	}
	if (W == WArr[I - 1])
	{
		goto RTCC_GIMGB2_4;
	}
	if (I < N)
	{
		I++;
		goto RTCC_GIMGB2_3;
	}
RTCC_GIMGB2_1:
	I = N;
	goto RTCC_GIMGB2_4;
RTCC_GIMGB2_2:
	return XI;
}

void RTCC::PMMDMT(int L, unsigned man, RTCCNIAuxOutputTable *aux)
{
	MissionPlanTable *mpt = GetMPTPointer(L);
	MPTManeuver *mptman;
	EphemerisData sv_FF;
	VECTOR3 V_G;
	
	if (mpt->mantable.size() < man)
	{
		//Error
		return;
	}
	mptman = &mpt->mantable[man - 1];

	mpt->TimeToEndManeuver[man - 1] = aux->GMT_BO;

	mptman->A_T = aux->A_T;
	mptman->R_BI = aux->R_BI;
	mptman->V_BI = aux->V_BI;
	mptman->GMT_BI = aux->GMT_BI;
	mptman->R_BO = aux->R_BO;
	mptman->V_BO = aux->V_BO;
	mptman->GMT_BO = aux->GMT_BO;
	mptman->R_1 = aux->R_1;
	mptman->V_1 = aux->V_1;
	mptman->GMT_1 = aux->GMT_1;
	mptman->CoordSysInd = aux->CSI;
	mptman->RefBodyInd = aux->RBI;
	mptman->DV_M = aux->DV;
	mptman->dt_BD = aux->DT_B;
	mptman->P_G = aux->P_G;
	mptman->Y_G = aux->Y_G;
	mptman->X_B = aux->X_B;
	mptman->Y_B = aux->Y_B;
	mptman->Z_B = aux->Z_B;
	mptman->dt_TO = aux->DT_TO;
	mptman->dv_TO = aux->DV_TO;
	mptman->CommonBlock.CSMMass = aux->W_CSM;
	mptman->CommonBlock.LMAscentMass = aux->W_LMA;
	mptman->CommonBlock.LMDescentMass = aux->W_LMD;
	mptman->CommonBlock.SIVBMass = aux->W_SIVB;
	mptman->MainEngineFuelUsed = aux->MainFuelUsed;
	mptman->RCSFuelUsed = aux->RCSFuelUsed;
	mptman->lng_AN = 0.0;

	double W_S_Prior, S_Fuel, WDOT, T, F, isp;

	if (man == 1)
	{
		W_S_Prior = mpt->CommonBlock.SIVBMass;
		S_Fuel = mpt->CommonBlock.SIVBFuelRemaining;
	}
	else
	{
		W_S_Prior = mpt->mantable[man - 2].CommonBlock.SIVBMass;
		S_Fuel = mpt->mantable[man - 2].CommonBlock.SIVBFuelRemaining;
	} 

	if (mptman->ConfigCodeBefore[RTCC_CONFIG_S])
	{
		double DW = W_S_Prior - S_Fuel;
		S_Fuel = S_Fuel - DW;
		if (mptman->TVC == 2)
		{
			mptman->CommonBlock.SIVBFuelRemaining = S_Fuel - aux->MainFuelUsed;

			T = MCTSAV;
			WDOT = MCTWAV;
			F = mptman->CommonBlock.SIVBFuelRemaining;
			if (man == 1)
			{
				mptman->CommonBlock.SPSFuelRemaining = mpt->CommonBlock.SPSFuelRemaining;
				mptman->CommonBlock.CSMRCSFuelRemaining = mpt->CommonBlock.CSMRCSFuelRemaining;
				mptman->CommonBlock.LMRCSFuelRemaining = mpt->CommonBlock.LMRCSFuelRemaining;
				mptman->CommonBlock.LMAPSFuelRemaining = mpt->CommonBlock.LMAPSFuelRemaining;
				mptman->CommonBlock.LMDPSFuelRemaining = mpt->CommonBlock.LMDPSFuelRemaining;
			}
			else
			{
				mptman->CommonBlock.SPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.SPSFuelRemaining;
				mptman->CommonBlock.CSMRCSFuelRemaining = mpt->mantable[man - 2].CommonBlock.CSMRCSFuelRemaining;
				mptman->CommonBlock.LMRCSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMRCSFuelRemaining;
				mptman->CommonBlock.LMAPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMAPSFuelRemaining;
				mptman->CommonBlock.LMDPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMDPSFuelRemaining;
			}
		}
	}

	if (mptman->TVC != 2)
	{
		if (man == 1)
		{
			if (mptman->TVC == 1)
			{
				mptman->CommonBlock.SPSFuelRemaining = mpt->CommonBlock.SPSFuelRemaining - aux->MainFuelUsed;
				mptman->CommonBlock.CSMRCSFuelRemaining = mpt->CommonBlock.CSMRCSFuelRemaining - aux->RCSFuelUsed;
				mptman->CommonBlock.LMRCSFuelRemaining = mpt->CommonBlock.LMRCSFuelRemaining;
				mptman->CommonBlock.LMAPSFuelRemaining = mpt->CommonBlock.LMAPSFuelRemaining;
				mptman->CommonBlock.LMDPSFuelRemaining = mpt->CommonBlock.LMDPSFuelRemaining;
			}
			else
			{
				mptman->CommonBlock.SPSFuelRemaining = mpt->CommonBlock.SPSFuelRemaining;
				mptman->CommonBlock.CSMRCSFuelRemaining = mpt->CommonBlock.CSMRCSFuelRemaining;
				mptman->CommonBlock.LMRCSFuelRemaining = mpt->CommonBlock.LMRCSFuelRemaining - aux->RCSFuelUsed;
				if (mptman->Thruster == RTCC_ENGINETYPE_LMAPS)
				{
					mptman->CommonBlock.LMAPSFuelRemaining = mpt->CommonBlock.LMAPSFuelRemaining - aux->MainFuelUsed;
					mptman->CommonBlock.LMDPSFuelRemaining = 0.0;
				}
				else if (mptman->Thruster == RTCC_ENGINETYPE_LMDPS)
				{
					mptman->CommonBlock.LMDPSFuelRemaining = mpt->CommonBlock.LMDPSFuelRemaining - aux->MainFuelUsed;
					mptman->CommonBlock.LMAPSFuelRemaining = mpt->CommonBlock.LMAPSFuelRemaining;
				}
				else
				{
					mptman->CommonBlock.LMDPSFuelRemaining = mpt->CommonBlock.LMDPSFuelRemaining;
					mptman->CommonBlock.LMAPSFuelRemaining = mpt->CommonBlock.LMAPSFuelRemaining;
				}
			}
		}
		else
		{
			if (mptman->TVC == 1)
			{
				mptman->CommonBlock.SPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.SPSFuelRemaining - aux->MainFuelUsed;
				mptman->CommonBlock.CSMRCSFuelRemaining = mpt->mantable[man - 2].CommonBlock.CSMRCSFuelRemaining - aux->RCSFuelUsed;
				mptman->CommonBlock.LMRCSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMRCSFuelRemaining;
				mptman->CommonBlock.LMAPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMAPSFuelRemaining;
				mptman->CommonBlock.LMDPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMDPSFuelRemaining;
			}
			else
			{
				mptman->CommonBlock.SPSFuelRemaining = mpt->CommonBlock.SPSFuelRemaining;
				mptman->CommonBlock.CSMRCSFuelRemaining = mpt->CommonBlock.CSMRCSFuelRemaining;
				if (mptman->Thruster == RTCC_ENGINETYPE_LMAPS)
				{
					mptman->CommonBlock.LMAPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMAPSFuelRemaining - aux->MainFuelUsed;
					mptman->CommonBlock.LMDPSFuelRemaining = 0.0;
				}
				else if (mptman->Thruster == RTCC_ENGINETYPE_LMDPS)
				{
					mptman->CommonBlock.LMDPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMDPSFuelRemaining - aux->MainFuelUsed;
					mptman->CommonBlock.LMAPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMAPSFuelRemaining;
				}
				else
				{
					mptman->CommonBlock.LMDPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMDPSFuelRemaining;
					mptman->CommonBlock.LMAPSFuelRemaining = mpt->mantable[man - 2].CommonBlock.LMAPSFuelRemaining;
				}
			}
		}

		EngineParametersTable(mptman->Thruster, T, isp);
		WDOT = T / isp;

		if (mptman->Thruster == RTCC_ENGINETYPE_CSMRCSPLUS2 || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSMINUS2)
		{
			T = MCTCT1;
			WDOT = MCTCW1;
			F = mptman->CommonBlock.CSMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_CSMRCSPLUS4 || mptman->Thruster == RTCC_ENGINETYPE_CSMRCSMINUS4)
		{
			T = 2.0*MCTCT1;
			WDOT = 2.0*MCTCW1;
			F = mptman->CommonBlock.CSMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_LMRCSPLUS4 || mptman->Thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
		{
			T = MCTLT1;
			WDOT = MCTLW1;
			F = mptman->CommonBlock.LMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_LMRCSPLUS4 || mptman->Thruster == RTCC_ENGINETYPE_LMRCSMINUS4)
		{
			T = 2.0*MCTLT1;
			WDOT = 2.0*MCTLW1;
			F = mptman->CommonBlock.LMRCSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_CSMSPS)
		{
			T = MCTST1;
			WDOT = MCTSW1;
			F = mptman->CommonBlock.SPSFuelRemaining;
		}
		else if (mptman->Thruster == RTCC_ENGINETYPE_LMAPS)
		{
			T = MCTAT1;
			WDOT = MCTAW1;
			F = mptman->CommonBlock.LMAPSFuelRemaining;
		}
		else
		{
			T = MCTDT1;
			WDOT = MCTDW1;
			F = mptman->CommonBlock.LMDPSFuelRemaining;
		}
	}

	//TBD: Docking maneuver

	mptman->TotalMassAfter = 0.0;
	if (mptman->CommonBlock.ConfigCode[RTCC_CONFIG_C])
	{
		mptman->TotalMassAfter += aux->W_CSM;
	}
	if(mptman->CommonBlock.ConfigCode[RTCC_CONFIG_A])
	{
		mptman->TotalMassAfter += aux->W_LMA;
	}
	if (mptman->CommonBlock.ConfigCode[RTCC_CONFIG_D])
	{
		mptman->TotalMassAfter += aux->W_LMD;
	}
	if (mptman->CommonBlock.ConfigCode[RTCC_CONFIG_S])
	{
		mptman->TotalMassAfter += aux->W_SIVB;
	}

	mptman->DVREM = T / WDOT * log(mptman->TotalMassAfter / (mptman->TotalMassAfter - F));
	if (mptman->Thruster == RTCC_ENGINETYPE_LOX_DUMP)
	{
		mptman->DVC = mptman->DVXBT = aux->DV*cos(0.0);
	}
	else
	{
		mptman->DVC = aux->DV_C;
		mptman->DVXBT = aux->DV_C + aux->DV_cTO;
	}
	//Inertial, manual
	if (mptman->AttitudeCode < 3)
	{
		V_G = aux->A_T*aux->DV;
		sv_FF = aux->sv_FF;
		mptman->dV_inertial = V_G;
	}
	//Lambert, External DV, AGS
	else if (mptman->AttitudeCode != 6)
	{
		V_G = aux->V_G;
		sv_FF = aux->sv_FF;
		mptman->dV_inertial = V_G;
	}
	//IGM
	else
	{
		EMSMISSInputTable intab;
		intab.AnchorVector.R = aux->R_1;
		intab.AnchorVector.V = aux->V_1;
		intab.AnchorVector.GMT = aux->GMT_1;
		intab.AnchorVector.RBI = aux->RBI;
		intab.AuxTableIndicator = NULL;
		intab.CutoffIndicator = 0;
		intab.EphemerisBuildIndicator = false;
		intab.ManCutoffIndicator = 2;
		intab.ManeuverIndicator = false;
		intab.MaxIntegTime = aux->GMT_BI - aux->GMT_1;

		EMSMISS(intab);
		sv_FF = intab.NIAuxOutputTable.sv_cutoff;

		if (mptman->Word78i[0] != 0)
		{
			mptman->dV_inertial.z = aux->Word60;
			mptman->dV_LVLH.x = aux->Word61;
			mptman->dV_LVLH.y = aux->Word62;
			mptman->dV_LVLH.z = aux->Word63;
			mptman->Word67d = aux->Word64;
			mptman->Word68 = aux->Word65;
			mptman->Word69 = aux->Word66;
			mptman->Word70 = aux->Word67;
			mptman->Word71 = aux->Word68;
			mptman->Word72 = aux->Word69;
			mptman->Word78i[0] = 0;
		}
		V_G = aux->A_T*aux->DV;
	}
	VECTOR3 Z_PHV = -unit(sv_FF.R);
	VECTOR3 Y_PHV = unit(crossp(sv_FF.V, sv_FF.R));
	VECTOR3 X_PHV = crossp(Y_PHV, Z_PHV);
	mptman->P_H = asin(dotp(-Z_PHV, aux->X_B));
	double P_H_apo = abs(mptman->P_H);
	if (abs(P_H_apo - mptman->P_H) < 0.0017)
	{
		mptman->R_H = 0.0;
		mptman->Y_H = atan2(dotp(-X_PHV,aux->Y_B), dotp(Y_PHV,aux->Y_B));
	}
	else
	{
		mptman->Y_H = atan2(dotp(Y_PHV, aux->X_B), dotp(X_PHV, aux->X_B));
		mptman->R_H = atan2(dotp(Z_PHV, aux->Y_B), dotp(Z_PHV, aux->Z_B));
	}
	if (mptman->Y_H < 0)
	{
		mptman->Y_H += PI2;
	}
	if (mptman->R_H < 0)
	{
		mptman->R_H += PI2;
	}
	mptman->V_F = dotp(X_PHV, V_G);
	mptman->V_S = dotp(Y_PHV, V_G);
	mptman->V_D = dotp(Z_PHV, V_G);

	double mu, R_E;
	int in, out, body;

	if (aux->RBI == BODY_EARTH)
	{
		in = 0;
		out = 1;
		mu = OrbMech::mu_Earth;
		body = BODY_EARTH;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		in = 2;
		out = 3;
		mu = OrbMech::mu_Moon;
		body = BODY_MOON;
		R_E = MCSMLR;
	}

	EphemerisData sv_true;
	EphemerisData sv_BI;
	EphemerisData sv_BO;
	AEGDataBlock aeg;

	sv_BI.R = aux->R_BI;
	sv_BI.V = aux->V_BI;
	sv_BI.GMT = aux->GMT_BI;
	sv_BI.RBI = aux->RBI;

	int err = ELVCNV(sv_BI, in, out, sv_true);
	if (err)
	{
		PMXSPT("PMMDMT", 70);
		sv_true = sv_BI;
		goto RTCC_PMMDMT_PB2_6;
	}

	if (body == BODY_EARTH)
	{
		double R_EE = length(sv_true.R) / sqrt((sv_true.R.x*sv_true.R.x + sv_true.R.y*sv_true.R.y) / MCEASQ + sv_true.R.z*sv_true.R.z / MCEBSQ);
		mptman->h_BI = length(sv_true.R) - R_EE;
		mptman->lat_BI = atan(MCEBAS*sv_true.R.z / sqrt(sv_true.R.x*sv_true.R.x + sv_true.R.y*sv_true.R.y));
	}
	else
	{
		mptman->h_BI = length(sv_true.R) - MCSMLR;
		mptman->lat_BI = asin(sv_true.R.z / sv_true.R.x);
	}
	mptman->lng_BI = atan2(sv_true.R.y, sv_true.R.x);
	double h = length(crossp(sv_true.R, sv_true.V));
	mptman->eta_BI = atan2(h*dotp(sv_true.R, sv_true.V), h*h - mu * length(sv_true.R));
	if (mptman->eta_BI < 0)
	{
		mptman->eta_BI += PI2;
	}

	sv_BO.R = aux->R_BO;
	sv_BO.V = aux->V_BO;
	sv_BO.GMT = aux->GMT_BO;
	sv_BO.RBI = aux->RBI;

	err = ELVCNV(sv_BO, in, out, sv_true);
	if (err)
	{
		goto RTCC_PMMDMT_PB2_6;
	}

	PIMCKC(sv_true.R, sv_true.V, body, aeg.coe_osc.a, aeg.coe_osc.e, aeg.coe_osc.i, aeg.coe_osc.l, aeg.coe_osc.g, aeg.coe_osc.h);
	mptman->e_BO = aeg.coe_osc.e;
	mptman->i_BO = aeg.coe_osc.i;
	mptman->g_BO = aeg.coe_osc.g;

	if (aeg.coe_osc.e < 0.85)
	{
		SV sv_BO2, sv_a, sv_p;

		sv_BO2.R = aux->R_BO;
		sv_BO2.V = aux->V_BO;
		sv_BO2.MJD = OrbMech::MJDfromGET(aux->GMT_BO, GMTBASE);
		sv_BO2.gravref = GetGravref(aux->RBI);

		if (PMMAPD(sv_BO2, sv_a, sv_p))
		{
			PMXSPT("PMMDMT", 108);
		}

		mptman->h_a = length(sv_a.R) - R_E;
		mptman->GMT_a = OrbMech::GETfromMJD(sv_a.MJD, GMTBASE);

		EphemerisData sv_a2;
		sv_a2.R = sv_a.R;
		sv_a2.V = sv_a.V;
		sv_a2.GMT = OrbMech::GETfromMJD(sv_a.MJD, GMTBASE);
		if (sv_a.gravref == hEarth)
		{
			sv_a2.RBI = BODY_EARTH;
		}
		else
		{
			sv_a2.RBI = BODY_MOON;
		}
		
		err = ELVCNV(sv_a2, in, out, sv_true);
		if (err)
		{
			goto RTCC_PMMDMT_PB2_6;
		}

		OrbMech::latlong_from_r(sv_true.R, mptman->lat_a, mptman->lng_p);

		mptman->h_p = length(sv_p.R) - R_E;
		mptman->GMT_p = OrbMech::GETfromMJD(sv_p.MJD, GMTBASE);

		EphemerisData sv_p2;
		sv_p2.R = sv_p.R;
		sv_p2.V = sv_p.V;
		sv_p2.GMT = OrbMech::GETfromMJD(sv_p.MJD, GMTBASE);
		if (sv_p.gravref == hEarth)
		{
			sv_p2.RBI = BODY_EARTH;
		}
		else
		{
			sv_p2.RBI = BODY_MOON;
		}

		err = ELVCNV(sv_p2, in, out, sv_true);
		if (err)
		{
			goto RTCC_PMMDMT_PB2_6;
		}

		OrbMech::latlong_from_r(sv_true.R, mptman->lat_p, mptman->lng_p);

		bool err2;
		SV sv_an = OrbMech::PMMAEGS(sv_BO2, 2, 0.0, err2);
		if (err2 == false)
		{
			mptman->GMT_AN = OrbMech::GETfromMJD(sv_an.MJD, GMTBASE);

			EphemerisData sv_an2;
			sv_an2.R = sv_an.R;
			sv_an2.V = sv_an.V;
			sv_an2.GMT = OrbMech::GETfromMJD(sv_an.MJD, GMTBASE);
			if (sv_an.gravref == hEarth)
			{
				sv_an2.RBI = BODY_EARTH;
			}
			else
			{
				sv_an2.RBI = BODY_MOON;
			}


			int err = ELVCNV(sv_an2, in, out, sv_true);
			if (err)
			{
				goto RTCC_PMMDMT_PB2_6;
			}
			double lattemp;
			OrbMech::latlong_from_r(sv_true.R, lattemp, mptman->lng_AN);
		}
	}
	else
	{
		double r_apo, r_peri;
		OrbMech::periapo(aux->R_BO, aux->V_BO, mu, r_apo, r_peri);
		mptman->h_a = r_apo - R_E;
		mptman->h_p = r_peri - R_E;
	}
	//TBD: Day and night

RTCC_PMMDMT_PB2_6:;

}

//CMC External Delta-V Update Generator
void RTCC::CMMAXTDV(double GETIG, VECTOR3 DV_EXDV)
{
	CZAXTRDV.Octals[0] = 12;
	CZAXTRDV.Octals[1] = MCCCEX;
	CZAXTRDV.Octals[2] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 1);
	CZAXTRDV.Octals[3] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 0);
	CZAXTRDV.Octals[4] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 1);
	CZAXTRDV.Octals[5] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 0);
	CZAXTRDV.Octals[6] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 1);
	CZAXTRDV.Octals[7] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 0);
	CZAXTRDV.Octals[8] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 1);
	CZAXTRDV.Octals[9] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 0);

	CZAXTRDV.GET = GETIG;
	CZAXTRDV.DV = DV_EXDV / 0.3048;
}
//LGC External Delta-V Update Generator
void RTCC::CMMLXTDV(double GETIG, VECTOR3 DV_EXDV)
{
	CZLXTRDV.Octals[0] = 12;
	CZLXTRDV.Octals[1] = MCCLEX;
	CZLXTRDV.Octals[2] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 1);
	CZLXTRDV.Octals[3] = OrbMech::DoubleToBuffer(DV_EXDV.x / 100.0, 7, 0);
	CZLXTRDV.Octals[4] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 1);
	CZLXTRDV.Octals[5] = OrbMech::DoubleToBuffer(DV_EXDV.y / 100.0, 7, 0);
	CZLXTRDV.Octals[6] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 1);
	CZLXTRDV.Octals[7] = OrbMech::DoubleToBuffer(DV_EXDV.z / 100.0, 7, 0);
	CZLXTRDV.Octals[8] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 1);
	CZLXTRDV.Octals[9] = OrbMech::DoubleToBuffer(GETIG*100.0, 28, 0);

	CZLXTRDV.GET = GETIG;
	CZLXTRDV.DV = DV_EXDV / 0.3048;
}

void RTCC::CMMRFMAT(int L, int id, int addr)
{
	REFSMMATUpdateMakeupTableBlock *block;
	if (L == 1)
	{
		block = &CZREFMAT.Block[0];
	}
	else
	{
		block = &CZREFMAT.Block[1];
	}

	block->error = "";

	REFSMMATData refs;
	if (L == 1)
	{
		refs = EZJGMTX1.data[id - 1];
	}
	else
	{
		refs = EZJGMTX3.data[id - 1];
	}
	if (refs.ID <= 0)
	{
		block->error = "REFSMMAT NOT AVAILABLE";
		return;
	}

	char buff[7];
	FormatREFSMMATCode(id, refs.ID, buff);
	block->MatrixID.assign(buff);

	MATRIX3 a = mul(refs.REFSMMAT, OrbMech::tmat(OrbMech::J2000EclToBRCS(AGCEpoch)));
	block->REFSMMAT = a;

	//sprintf(oapiDebugString(), "%f, %f, %f, %f, %f, %f, %f, %f, %f", a.m11, a.m12, a.m13, a.m21, a.m22, a.m23, a.m31, a.m32, a.m33);

	block->Octals[0] = 24;
	if (addr == 1)
	{
		block->MatrixType = 1;
		if (L == 1)
		{
			block->Octals[1] = MCCCRF;
		}
		else
		{
			block->Octals[1] = MCCLRF;
		}
	}
	else
	{
		block->MatrixType = 2;
		if (L == 1)
		{
			block->Octals[1] = MCCCXS;
		}
		else
		{
			block->Octals[1] = MCCLXS;
		}
	}
	block->Octals[2] = OrbMech::DoubleToBuffer(a.m11, 1, 1);
	block->Octals[3] = OrbMech::DoubleToBuffer(a.m11, 1, 0);
	block->Octals[4] = OrbMech::DoubleToBuffer(a.m12, 1, 1);
	block->Octals[5] = OrbMech::DoubleToBuffer(a.m12, 1, 0);
	block->Octals[6] = OrbMech::DoubleToBuffer(a.m13, 1, 1);
	block->Octals[7] = OrbMech::DoubleToBuffer(a.m13, 1, 0);
	block->Octals[8] = OrbMech::DoubleToBuffer(a.m21, 1, 1);
	block->Octals[9] = OrbMech::DoubleToBuffer(a.m21, 1, 0);
	block->Octals[10] = OrbMech::DoubleToBuffer(a.m22, 1, 1);
	block->Octals[11] = OrbMech::DoubleToBuffer(a.m22, 1, 0);
	block->Octals[12] = OrbMech::DoubleToBuffer(a.m23, 1, 1);
	block->Octals[13] = OrbMech::DoubleToBuffer(a.m23, 1, 0);
	block->Octals[14] = OrbMech::DoubleToBuffer(a.m31, 1, 1);
	block->Octals[15] = OrbMech::DoubleToBuffer(a.m31, 1, 0);
	block->Octals[16] = OrbMech::DoubleToBuffer(a.m32, 1, 1);
	block->Octals[17] = OrbMech::DoubleToBuffer(a.m32, 1, 0);
	block->Octals[18] = OrbMech::DoubleToBuffer(a.m33, 1, 1);
	block->Octals[19] = OrbMech::DoubleToBuffer(a.m33, 1, 0);

	block->UpdateNo++;
}

void RTCC::BMSVEC()
{
	//If time is missing the time of the first vector is the assumed GMT and the first vector ID cannot be "EPHO" or "EPHR"
	if (med_s80.time == 0.0 && (med_s80.VID[0] == "EPHO" || med_s80.VID[0] == "EPHR"))
	{
		return;
	}

	OELEMENTS coe;
	double mu, r_apo, r_peri, R_E, r, v, lat, lng, gamma, azi, gmt;
	int numvec = 0, lemoffset;

	EphemerisData sv_comp[4];
	EphemerisData sv_final[4];
	EphemerisData sv_w;
	MATRIX3 Rot;

	//Reset
	BZCCANOE = VectorCompareTable();
	//Get transformation matrix
	Rot = OrbMech::J2000EclToBRCS(AGCEpoch);

	ELVCTRInputTable intab;
	ELVCTROutputTable outtab;

	//Offset in the evaluation vector table
	if (med_s80.VEH == 1)
	{
		lemoffset = 0;
	}
	else
	{
		lemoffset = 6;
	}

	//Get GMT for comparison
	if (med_s80.time > 0)
	{
		gmt = med_s80.time;
	}
	else if (med_s80.time < 0)
	{
		gmt = GMTfromGET(-med_s80.time);
	}
	else
	{
		//gmt gets set later
		gmt = 0.0;
	}

	for (int i = 0;i < 4;i++)
	{
		//Are we requesting an ephemeris vector? 
		if (med_s80.VID[i] == "EPHO")
		{
			intab.EphemerisType = 0;
			intab.GMT = gmt;
			intab.L = med_s80.VEH;

			ELVCTR(intab, outtab);

			if (outtab.ErrorCode)
			{
				BMGPRIME("BMSVEC", 10);
				BZCCANOE.error = true;
				goto RTCC_BMSVEC_1;
			}
			sv_comp[i] = outtab.SV;
			//J2000 to BRCS
			sv_comp[i].R = mul(Rot, sv_comp[i].R);
			sv_comp[i].V = mul(Rot, sv_comp[i].V);
			numvec++;
		}
		else if (med_s80.VID[i] == "CMC")
		{
			if (BZEVLVEC.ID[lemoffset + 0] < 0)
			{
				RTCCONLINEMON.TextBuffer[0] = "CMC";
				BMGPRIME("BMSVEC", 43);
				BZCCANOE.error = true;
				goto RTCC_BMSVEC_1;
			}
			sv_comp[i] = BZEVLVEC.Vectors[lemoffset + 0];
			numvec++;
		}
		else if (med_s80.VID[i] == "LGC")
		{
			if (BZEVLVEC.ID[lemoffset + 1] < 0)
			{
				RTCCONLINEMON.TextBuffer[0] = "LGC";
				BMGPRIME("BMSVEC", 43);
				BZCCANOE.error = true;
				goto RTCC_BMSVEC_1;
			}
			sv_comp[i] = BZEVLVEC.Vectors[lemoffset + 1];
			numvec++;
		}
		else if (med_s80.VID[i] == "HSR")
		{
			if (BZEVLVEC.ID[lemoffset + 4] < 0)
			{
				RTCCONLINEMON.TextBuffer[0] = "HSR";
				BMGPRIME("BMSVEC", 43);
				BZCCANOE.error = true;
				goto RTCC_BMSVEC_1;
			}
			sv_comp[i] = BZEVLVEC.Vectors[lemoffset + 4];
			sv_comp[i].R = mul(Rot, sv_comp[i].R);
			sv_comp[i].V = mul(Rot, sv_comp[i].V);
			numvec++;
		}
		else if (med_s80.VID[i] == "")
		{
			break;
		}
		else
		{
			BZCCANOE.error = true;
			goto RTCC_BMSVEC_1;
		}
		if (i == 0)
		{
			gmt = sv_comp[i].GMT;
		}
	}

	BZCCANOE.NumVec = numvec;

	if (med_s80.REF == BODY_EARTH)
	{
		mu = OrbMech::mu_Earth;
		R_E = OrbMech::R_Earth;
	}
	else
	{
		mu = OrbMech::mu_Moon;
		R_E = MCSMLR;
	}

	VECTOR3 U, V, W;

	for (int i = 0;i < numvec;i++)
	{
		//BRCS to ecliptic
		sv_comp[i].R = tmul(Rot, sv_comp[i].R);
		sv_comp[i].V = tmul(Rot, sv_comp[i].V);
		//Coast
		OrbMech::oneclickcoast(sv_comp[i].R, sv_comp[i].V, GMTBASE + sv_comp[i].GMT / 24.0 / 3600.0, gmt - sv_comp[i].GMT, sv_final[i].R, sv_final[i].V, sv_comp[i].RBI, med_s80.REF);
		sv_final[i].GMT = gmt;
		sv_final[i].RBI = med_s80.REF;

		//ecliptic to BRCS
		sv_final[i].R = mul(Rot, sv_final[i].R);
		sv_final[i].V = mul(Rot, sv_final[i].V);

		if (i == 0)
		{
			U = unit(sv_final[i].R);
			W = unit(crossp(sv_final[i].R, sv_final[i].V));
			V = unit(crossp(-U, W));
		}

		BZCCANOE.data[i].GMT = sv_final[i].GMT;
		OrbMech::periapo(sv_final[i].R, sv_final[i].V, mu, r_apo, r_peri);
		coe = OrbMech::coe_from_sv(sv_final[i].R, sv_final[i].V, mu);
		PICSSC(true, sv_final[i].R, sv_final[i].V, r, v, lat, lng, gamma, azi);
		if (lng > PI)
		{
			lng -= PI2;
		}

		BZCCANOE.data[i].HA = (r_apo - R_E);
		BZCCANOE.data[i].HP = (r_peri - R_E);
		BZCCANOE.data[i].v = v;
		BZCCANOE.data[i].gamma = gamma;
		BZCCANOE.data[i].psi = azi;
		BZCCANOE.data[i].phi = lat;
		BZCCANOE.data[i].lambda = lng;
		BZCCANOE.data[i].h = (r - R_E);
		BZCCANOE.data[i].a = 1.0 / (2.0 / r - v * v / mu);
		BZCCANOE.data[i].e = coe.e;
		BZCCANOE.data[i].i = coe.i;
		BZCCANOE.data[i].theta_p = coe.w;
		BZCCANOE.data[i].Omega = coe.RA;
		BZCCANOE.data[i].nu = coe.TA;
		BZCCANOE.data[i].U = dotp(sv_final[i].R, U);
		BZCCANOE.data[i].V = dotp(sv_final[i].R, V);
		BZCCANOE.data[i].W = dotp(sv_final[i].R, W);
		BZCCANOE.data[i].U_dot = dotp(sv_final[i].V, U);
		BZCCANOE.data[i].V_dot = dotp(sv_final[i].V, V);
		BZCCANOE.data[i].W_dot = dotp(sv_final[i].V, W);
	}

RTCC_BMSVEC_1:
	BMDVEC();
}

void RTCC::BMDVEC()
{
	VectorCompareDisplayBuffer = VectorCompareDisplay();
	VectorCompareDisplayBuffer.error = "";

	if (BZCCANOE.error)
	{
		VectorCompareDisplayBuffer.error = "ERROR - REFER TO ONLINE";
		return;
	}

	VectorCompareDisplayBuffer.GMT = BZCCANOE.data[0].GMT;
	VectorCompareDisplayBuffer.GMTR = GMTfromGET(MCGREF);
	VectorCompareDisplayBuffer.PET = VectorCompareDisplayBuffer.GMT - VectorCompareDisplayBuffer.GMTR;
	VectorCompareDisplayBuffer.NumVec = BZCCANOE.NumVec;

	for (int i = 0;i < BZCCANOE.NumVec;i++)
	{
		VectorCompareDisplayBuffer.data[i].GMT = BZCCANOE.data[i].GMT;

		if (BZCCANOE.data[i].e < 1.0)
		{
			VectorCompareDisplayBuffer.data[i].HA = BZCCANOE.data[i].HA / 1852.0;
			VectorCompareDisplayBuffer.showHA[i] = true;
		}
		else
		{
			VectorCompareDisplayBuffer.showHA[i] = false;
		}
		VectorCompareDisplayBuffer.data[i].HP = BZCCANOE.data[i].HP / 1852.0;
		VectorCompareDisplayBuffer.data[i].v = BZCCANOE.data[i].v / 0.3048;
		VectorCompareDisplayBuffer.data[i].gamma = BZCCANOE.data[i].gamma*DEG;
		VectorCompareDisplayBuffer.data[i].psi = BZCCANOE.data[i].psi*DEG;
		VectorCompareDisplayBuffer.data[i].phi = BZCCANOE.data[i].phi*DEG;
		VectorCompareDisplayBuffer.data[i].lambda = BZCCANOE.data[i].lambda*DEG;
		VectorCompareDisplayBuffer.data[i].h = BZCCANOE.data[i].h / 1852.0;
		VectorCompareDisplayBuffer.data[i].a = BZCCANOE.data[i].a / 1852.0;
		VectorCompareDisplayBuffer.data[i].e = BZCCANOE.data[i].e;
		VectorCompareDisplayBuffer.data[i].i = BZCCANOE.data[i].i*DEG;
		if (BZCCANOE.data[i].e >= 0.001)
		{
			VectorCompareDisplayBuffer.showWPAndTA[i] = true;
			VectorCompareDisplayBuffer.data[i].theta_p = BZCCANOE.data[i].theta_p*DEG;
			VectorCompareDisplayBuffer.data[i].nu = BZCCANOE.data[i].nu*DEG;
		}
		else
		{
			VectorCompareDisplayBuffer.showWPAndTA[i] = false;
		}
		VectorCompareDisplayBuffer.data[i].Omega = BZCCANOE.data[i].Omega*DEG;
		VectorCompareDisplayBuffer.data[i].U = BZCCANOE.data[i].U / 1852.0;
		VectorCompareDisplayBuffer.data[i].V = BZCCANOE.data[i].V / 1852.0;
		VectorCompareDisplayBuffer.data[i].W = BZCCANOE.data[i].W / 1852.0;
		VectorCompareDisplayBuffer.data[i].U_dot = BZCCANOE.data[i].U_dot / 0.3048;
		VectorCompareDisplayBuffer.data[i].V_dot = BZCCANOE.data[i].V_dot / 0.3048;
		VectorCompareDisplayBuffer.data[i].W_dot = BZCCANOE.data[i].W_dot / 0.3048;

		//Relative values
		if (i > 0)
		{
			if (VectorCompareDisplayBuffer.showHA[i])
			{
				VectorCompareDisplayBuffer.data[i].HA -= VectorCompareDisplayBuffer.data[0].HA;
			}
			VectorCompareDisplayBuffer.data[i].HP -= VectorCompareDisplayBuffer.data[0].HP;
			VectorCompareDisplayBuffer.data[i].v -= VectorCompareDisplayBuffer.data[0].v;
			VectorCompareDisplayBuffer.data[i].gamma -= VectorCompareDisplayBuffer.data[0].gamma;
			VectorCompareDisplayBuffer.data[i].psi -= VectorCompareDisplayBuffer.data[0].psi;
			VectorCompareDisplayBuffer.data[i].phi -= VectorCompareDisplayBuffer.data[0].phi;
			VectorCompareDisplayBuffer.data[i].lambda -= VectorCompareDisplayBuffer.data[0].lambda;
			VectorCompareDisplayBuffer.data[i].h -= VectorCompareDisplayBuffer.data[0].h;
			VectorCompareDisplayBuffer.data[i].a -= VectorCompareDisplayBuffer.data[0].a;
			VectorCompareDisplayBuffer.data[i].e -= VectorCompareDisplayBuffer.data[0].e;
			VectorCompareDisplayBuffer.data[i].i -= VectorCompareDisplayBuffer.data[0].i;
			if (VectorCompareDisplayBuffer.showWPAndTA[i])
			{
				VectorCompareDisplayBuffer.data[i].theta_p -= VectorCompareDisplayBuffer.data[0].theta_p;
				VectorCompareDisplayBuffer.data[i].nu -= VectorCompareDisplayBuffer.data[0].nu;
			}
			VectorCompareDisplayBuffer.data[i].Omega -= VectorCompareDisplayBuffer.data[0].Omega;
			VectorCompareDisplayBuffer.data[i].U -= VectorCompareDisplayBuffer.data[0].U;
			VectorCompareDisplayBuffer.data[i].V -= VectorCompareDisplayBuffer.data[0].V;
			VectorCompareDisplayBuffer.data[i].W -= VectorCompareDisplayBuffer.data[0].W;
			VectorCompareDisplayBuffer.data[i].U_dot -= VectorCompareDisplayBuffer.data[0].U_dot;
			VectorCompareDisplayBuffer.data[i].V_dot -= VectorCompareDisplayBuffer.data[0].V_dot;
			VectorCompareDisplayBuffer.data[i].W_dot -= VectorCompareDisplayBuffer.data[0].W_dot;
		}
		//Limits
	}
}

void RTCC::BMGPRIME(std::string source, int n)
{
	std::vector<std::string> message;

	switch (n)
	{
	case 1:
		message.push_back("SPACECRAFT BELOW EI");
		break;
	case 10:
		message.push_back("INTERPOLATION ERROR");
		break;
	case 43:
		message.push_back("CANNOT FIND VECTOR " + RTCCONLINEMON.TextBuffer[0]);
		break;
	}
	PMXSPT(source, message);
}

void RTCC::BMGPRIME(std::string source, std::vector<std::string> message)
{
	OnlinePrint(source, message);
}

void RTCC::LMMGRP(double gmt, double A_Z)
{
	double mjd = OrbMech::MJDfromGET(gmt, GMTBASE);
	MATRIX3 R = OrbMech::LaunchREFSMMAT(atan2(MCLSDA, MCLCDA), MCLGRA, mjd, A_Z);
	EMGSTSTM(1, R, RTCC_REFSMMAT_TYPE_CUR, gmt);
}

void RTCC::EMSGSUPP(int QUEID, int refs, int refs2, unsigned man, bool headsup)
{
	//Initialize star table
	if (QUEID == 0)
	{
		if (EZJGSTAR.size() == 0)
		{
			ifstream startable(".\\Config\\ProjectApollo\\RTCC Star Table.txt");
			std::string line;
			VECTOR3 temp;

			while (getline(startable, line))
			{
				sscanf(line.c_str(), "%lf %lf %lf", &temp.x, &temp.y, &temp.z);
				EZJGSTAR.push_back(temp);
			}
			startable.close();
		}
	}
	//Acquire and save CSM IMU matrix/optics
	if (QUEID == 1)
	{
		//Telemetry (high-speed)
		if (refs == 1)
		{
			if (BZSTLM.CMCRefsPresent == false)
			{
				EMGPRINT("EMSGSUPP", 1);
				return;
			}
			EMGSTSTM(1, BZSTLM.CMC_REFSMMAT, RTCC_REFSMMAT_TYPE_TLM, RTCCPresentTimeGMT());
		}
		//DMT
		else if (refs == 5)
		{
			if (refs2 < 0)
			{
				EMGPRINT("EMSGSUPP", 1);
				return;
			}
			MATRIX3 REFSMMAT;
			if (refs2 == 100)
			{
				//Desired REFSMMAT
				if (man < 0 || PZMPTCSM.ManeuverNum < man)
				{
					EMGPRINT("EMSGSUPP", 21);
					return;
				}

				MPTManeuver *mpt = &PZMPTCSM.mantable[man - 1];

				REFSMMAT = PIDREF(mpt->A_T, mpt->R_BI, mpt->V_BI, mpt->P_G, mpt->Y_G, headsup);
			}
			else
			{
				REFSMMATData data;
				data = EZJGMTX1.data[refs2 - 1];
				if (data.ID <= 0)
				{
					EMGPRINT("EMSGSUPP", 1);
					return;
				}
				REFSMMAT = data.REFSMMAT;
			}
			EMGSTSTM(1, REFSMMAT, RTCC_REFSMMAT_TYPE_DMT, RTCCPresentTimeGMT());
		}
	}
}

void RTCC::EMSLSUPP(int QUEID, int refs, int refs2, unsigned man, bool headsup)
{
	//Acquire and save LEM IMU matrix/optics
	if (QUEID == 1)
	{
		//Telemetry (high-speed)
		if (refs == 1)
		{
			if (BZSTLM.LGCRefsPresent == false)
			{
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			EMGSTSTM(3, BZSTLM.LGC_REFSMMAT, RTCC_REFSMMAT_TYPE_TLM, RTCCPresentTimeGMT());
		}
		//DMT
		else if (refs == 5)
		{
			if (refs2 < 0)
			{
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			MATRIX3 REFSMMAT;
			if (refs2 == 100)
			{
				//Desired REFSMMAT
				if (man < 0 || PZMPTLEM.ManeuverNum < man)
				{
					EMGPRINT("EMSLSUPP", 21);
					return;
				}

				MPTManeuver *mpt = &PZMPTLEM.mantable[man - 1];

				REFSMMAT = PIDREF(mpt->A_T, mpt->R_BI, mpt->V_BI, mpt->P_G, mpt->Y_G, headsup);
			}
			else
			{
				REFSMMATData data;
				data = EZJGMTX3.data[refs2 - 1];
				if (data.ID <= 0)
				{
					EMGPRINT("EMSLSUPP", 1);
					return;
				}
				REFSMMAT = data.REFSMMAT;
			}
			EMGSTSTM(3, REFSMMAT, RTCC_REFSMMAT_TYPE_DMT, RTCCPresentTimeGMT());
		}
		//Lunar Launch Desired
		else if (refs == 15)
		{
			if (EZJGLSAD.LLDRefsPresent == false)
			{
				EMGPRINT("EMSLSUPP", 1);
				return;
			}
			EMGSTSTM(3, EZJGLSAD.LLD_REFSMMAT, RTCC_REFSMMAT_TYPE_LLD, RTCCPresentTimeGMT());
		}
	}
}

void RTCC::EMMGSTMP()
{
	int err = 0;

	//Boresight/Scanning Telescope Data
	if (EZGSTMED.MTX1 > 0)
	{
		//Stars not initialized, error?
		if (EZJGSTAR.size() == 0) return;

		REFSMMATData refs = EZJGMTX1.data[EZGSTMED.MTX1 - 1];
		if (refs.ID <= 0)
		{
			err = 4;
			EZJGSTTB.IRA = "ZZZZZZ";
		}
		else
		{
			char Buff[7];

			FormatREFSMMATCode(EZGSTMED.MTX1, refs.ID, Buff);
			EZJGSTTB.IRA.assign(Buff);

			//Null stars
			unsigned i;

			for (i = 0;i < 2;i++)
			{
				EZJGSTTB.BS_S[i] = 0;
				EZJGSTTB.BS_DEC[i] = 0.0;
				EZJGSTTB.BS_RTASC[i] = 0.0;
				EZJGSTTB.BS_SPA[i] = 0.0;
				EZJGSTTB.BS_SXP[i] = 0.0;
			}

			MATRIX3 M_SMNB, RM;
			VECTOR3 u_SM, u_NB;
			double XSC, YSC, ZSC, SPA, SPX;

			RM = refs.REFSMMAT;
			M_SMNB = OrbMech::CALCSMSC(_V(EZJGSTTB.Att[0].z, EZJGSTTB.Att[0].x, EZJGSTTB.Att[0].y));

			VECTOR3 u;
			int num = 0;
			unsigned endstar;

			if (EZGSTMED.StartingStar <= EZJGSTAR.size())
			{
				i = EZGSTMED.StartingStar - 1;
			}
			else
			{
				i = 0;
			}
			endstar = i;

			do
			{
				u = EZJGSTAR[i];

				//TBD: Occultation check
				u_SM = mul(RM, u);
				u_NB = mul(M_SMNB, u_SM);

				XSC = u_NB.x;
				YSC = u_NB.y;
				ZSC = u_NB.z;

				SPA = atan2(ZSC, XSC);
				SPX = asin(YSC);

				if (abs(SPX) <= 5.0*RAD)
				{
					if (SPA <= 31.5*RAD && SPA >= -10.0*RAD)
					{
						//Found one
						EZJGSTTB.BS_S[num] = i + 1;
						EZJGSTTB.BS_DEC[num] = acos(u.z);
						EZJGSTTB.BS_RTASC[num] = atan2(u.y, u.x);
						if (EZJGSTTB.BS_RTASC[num] < 0)
						{
							EZJGSTTB.BS_RTASC[num] += PI2;
						}
						EZJGSTTB.BS_SPA[num] = SPA;
						EZJGSTTB.BS_SXP[num] = SPX;
						num++;
						if (num >= 2)
						{
							break;
						}
					}
				}
				i++;
				if (i >= EZJGSTAR.size())
				{
					i = 0;
				}
			} while (i != endstar);

			if (num < 2)
			{
				err = 3;
			}
		}
	}
	//Sextant Data
	if (EZGSTMED.MTX2 > 0)
	{
		//Stars not initialized, error?
		if (EZJGSTAR.size() == 0) return;

		REFSMMATData refs = EZJGMTX1.data[EZGSTMED.MTX2 - 1];
		if (refs.ID <= 0)
		{
			err = 4;
		}
		else
		{
			//Null stars
			unsigned i;

			for (i = 0;i < 2;i++)
			{
				EZJGSTTB.SXT_STAR[0] = 0;
				EZJGSTTB.SXT_SFT_INP[i] = 0.0;
				EZJGSTTB.SXT_SFT_RTCC[i] = 0.0;
				EZJGSTTB.SXT_TRN_INP[i] = 0.0;
				EZJGSTTB.SXT_TRN_RTCC[i] = 0.0;
			}

			MATRIX3 SMNB, RM, SBNB;
			VECTOR3 S_SM;
			double TA, SA, a;

			RM = refs.REFSMMAT;
			SMNB = OrbMech::CALCSMSC(_V(EZJGSTTB.Att[0].z, EZJGSTTB.Att[0].x, EZJGSTTB.Att[0].y));
			a = -0.5676353234;
			SBNB = _M(cos(a), 0, -sin(a), 0, 1, 0, sin(a), 0, cos(a));

			VECTOR3 u;
			int num = 0;
			unsigned endstar;

			if (EZGSTMED.StartingStar <= EZJGSTAR.size())
			{
				i = EZGSTMED.StartingStar - 1;
			}
			else
			{
				i = 0;
			}
			endstar = i;

			do
			{
				u = EZJGSTAR[i];

				//TBD: Occultation check
				S_SM = mul(RM, u);
				OrbMech::CALCSXA(SMNB, S_SM, TA, SA);

				if (TA <= 50.0*RAD)
				{
					//Found one
					EZJGSTTB.SXT_STAR[num] = i + 1;
					EZJGSTTB.SXT_SFT_RTCC[num] = SA;
					EZJGSTTB.SXT_TRN_RTCC[num] = TA;
					num++;
					if (num >= 2)
					{
						break;
					}
				}
				i++;
				if (i >= EZJGSTAR.size())
				{
					i = 0;
				}
			} while (i != endstar);

			if (num < 2)
			{
				err = 3;
			}
		}
	}
	//Matrix display
	if (EZGSTMED.MTX3 > 0)
	{
		REFSMMATData refs = EZJGMTX1.data[EZGSTMED.MTX3 - 1];
		if (refs.ID <= 0)
		{
			err = 4;
			EZJGSTTB.REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
			EZJGSTTB.MAT = "";
		}
		else
		{
			char Buff[7];

			FormatREFSMMATCode(EZGSTMED.MTX3, refs.ID, Buff);
			EZJGSTTB.MAT.assign(Buff);
			EZJGSTTB.REFSMMAT = refs.REFSMMAT;
		}
	}
	//Unit vector
	if (EZGSTMED.G14_Star > 0)
	{
		EZJGSTTB.Landmark_GET = 0.0;
		VECTOR3 u = EZJGSTAR[EZGSTMED.G14_Star - 1];
		MATRIX3 Rot = OrbMech::J2000EclToBRCS(AGCEpoch);
		u = mul(Rot, u);
		char buff[4];
		sprintf_s(buff, "%03o", EZGSTMED.G14_Star);
		EZJGSTTB.Landmark_SC.assign(buff);
		EZJGSTTB.Landmark_LOS = u;
		EZJGSTTB.Landmark_DEC = acos(u.z);
		EZJGSTTB.Landmark_RA = atan2(u.y, u.x);
		if (EZJGSTTB.Landmark_RA < 0)
		{
			EZJGSTTB.Landmark_RA += PI2;
		}
	}
	//Landmark unit vector
	if (EZGSTMED.G14_RB > 0)
	{
		EZJGSTTB.Landmark_SC = "";
		EZJGSTTB.Landmark_LOS = _V(0, 0, 0);
		EZJGSTTB.Landmark_DEC = 0.0;
		EZJGSTTB.Landmark_RA = 0.0;

		VECTOR3 R_EM, V_EM, R_ES, R_BL, R_BV, R_VL, R_BL_equ;
		if (OrbMech::PLEFEM(pzefem, OrbMech::MJDfromGET(EZGSTMED.G14_GMT, GMTBASE), R_EM, V_EM, R_ES) == false)
		{
			err = 2;
		}
		else
		{
			if (EZGSTMED.G14_RB == 1)
			{
				//Point to center of sun
				R_BL = _V(0, 0, 0);
			}
			else
			{
				double r;
				int in, out;
				if (EZGSTMED.G14_RB == 2)
				{
					r = EZGSTMED.G14_height + MCSMLR;
					in = 3;
					out = 2;
				}
				else
				{
					r = EZGSTMED.G14_height + OrbMech::R_Earth;
					in = 1;
					out = 0;
				}
				R_BL_equ = OrbMech::r_from_latlong(EZGSTMED.G14_lat, EZGSTMED.G14_lng, r);
				if (ELVCNV(R_BL_equ, EZGSTMED.G14_GMT, in, out, R_BL))
				{
					err = 2;
				}
			}
			ELVCTRInputTable intab;
			ELVCTROutputTable outtab;

			intab.GMT = EZGSTMED.G14_GMT;
			intab.L = EZGSTMED.G14_Vehicle;

			ELVCTR(intab, outtab);
			if (outtab.ErrorCode)
			{
				err = 2;
			}
			else
			{
				if (outtab.SV.RBI == BODY_EARTH)
				{
					//Sun
					if (EZGSTMED.G14_RB == 1)
					{
						R_BV = outtab.SV.R - R_ES;
					}
					//Moon
					else if (EZGSTMED.G14_RB == 2)
					{
						R_BV = (outtab.SV.R) - R_EM;
					}
					//Earth
					else
					{
						R_BV = outtab.SV.R;
					}
				}
				else
				{
					//Sun
					if (EZGSTMED.G14_RB == 1)
					{
						R_BV = (outtab.SV.R + R_EM) - R_ES;
					}
					//Moon
					else if (EZGSTMED.G14_RB == 2)
					{
						R_BV = outtab.SV.R;
					}
					//Earth
					else
					{
						R_BV = outtab.SV.R + R_EM;
					}
				}
				R_VL = R_BL - R_BV;

				MATRIX3 Rot = OrbMech::J2000EclToBRCS(AGCEpoch);
				VECTOR3 u = mul(Rot, unit(R_VL));
				EZJGSTTB.Landmark_SC = "";
				if (EZGSTMED.G14_Vehicle == 1)
				{
					EZJGSTTB.Landmark_SC += "C";
				}
				else
				{
					EZJGSTTB.Landmark_SC += "L";
				}
				//TBD
				EZJGSTTB.Landmark_SC += "1";
				if (EZGSTMED.G14_RB == 1)
				{
					EZJGSTTB.Landmark_SC += "S";
				}
				else if (EZGSTMED.G14_RB == 2)
				{
					EZJGSTTB.Landmark_SC += "M";
				}
				else
				{
					EZJGSTTB.Landmark_SC += "E";
				}
				EZJGSTTB.Landmark_GET = GETfromGMT(EZGSTMED.G14_GMT);
				EZJGSTTB.Landmark_LOS = u;
				EZJGSTTB.Landmark_DEC = acos(u.z);
				EZJGSTTB.Landmark_RA = atan2(u.y, u.x);
				if (EZJGSTTB.Landmark_RA < 0)
				{
					EZJGSTTB.Landmark_RA += PI2;
				}
			}
		}
	}

	EMDGSUPP(err);
}

void RTCC::EMDGSUPP(int err)
{
	GOSTDisplayBuffer.err = "";

	if (err)
	{
		switch (err)
		{
		case 1:
			GOSTDisplayBuffer.err = "NO OPTICS THIS VEHICLE";
			break;
		case 2:
			GOSTDisplayBuffer.err = "EPHEMERIS NOT AVAILABLE";
			break;
		case 3:
			GOSTDisplayBuffer.err = "TWO STARS NOT AVAILABLE";
			break;
		case 4:
			GOSTDisplayBuffer.err = "MATRIX NOT AVAILABLE";
			break;
		case 5:
			GOSTDisplayBuffer.err = "MED INPUT REQUIRED";
			break;
		case 6:
			GOSTDisplayBuffer.err = "ATTITUDES NOT AVAILABLE";
			break;
		case 7:
			GOSTDisplayBuffer.err = "OPTICS NOT AVAILABLE";
			break;
		}
	}

	GOSTDisplayBuffer.data.MAT = EZJGSTTB.MAT;
	GOSTDisplayBuffer.data.REFSMMAT = EZJGSTTB.REFSMMAT;
	GOSTDisplayBuffer.data.Landmark_DEC = EZJGSTTB.Landmark_DEC * DEG*3600.0;
	GOSTDisplayBuffer.data.Landmark_RA = EZJGSTTB.Landmark_RA * DEG*3600.0;
	GOSTDisplayBuffer.data.Landmark_SC = EZJGSTTB.Landmark_SC;
	GOSTDisplayBuffer.data.Landmark_GET = EZJGSTTB.Landmark_GET;
	GOSTDisplayBuffer.data.Landmark_LOS = EZJGSTTB.Landmark_LOS;

	for (int i = 0;i < 2;i++)
	{
		GOSTDisplayBuffer.data.Att[i] = EZJGSTTB.Att[i] * DEG;
		GOSTDisplayBuffer.data.BS_S[i] = EZJGSTTB.BS_S[i];
		GOSTDisplayBuffer.data.BS_DEC[i] = EZJGSTTB.BS_DEC[i] * DEG*3600.0;
		GOSTDisplayBuffer.data.BS_RTASC[i] = EZJGSTTB.BS_RTASC[i] * DEG*3600.0;
		GOSTDisplayBuffer.data.BS_SPA[i] = EZJGSTTB.BS_SPA[i] * DEG;
		GOSTDisplayBuffer.data.BS_SXP[i] = EZJGSTTB.BS_SXP[i] * DEG;
		GOSTDisplayBuffer.data.SXT_STAR[i] = EZJGSTTB.SXT_STAR[i];
		GOSTDisplayBuffer.data.SXT_SFT_INP[i] = EZJGSTTB.SXT_SFT_INP[i] * DEG;
		GOSTDisplayBuffer.data.SXT_SFT_RTCC[i] = EZJGSTTB.SXT_SFT_RTCC[i] * DEG;
		GOSTDisplayBuffer.data.SXT_TRN_INP[i] = EZJGSTTB.SXT_TRN_INP[i] * DEG;
		GOSTDisplayBuffer.data.SXT_TRN_RTCC[i] = EZJGSTTB.SXT_TRN_RTCC[i] * DEG;
	}
}

void RTCC::CMMSLVNAV(VECTOR3 R_ecl, VECTOR3 V_ecl, double GMT)
{
	double lambda = MCLGRA + MCGRIC *3600.0*OrbMech::w_Earth;
	//Converts from equatorial coordinates (with launchpad longitude as 0) to RTCC ecliptic
	MATRIX3 RMAT = mul(MatrixRH_LH(OrbMech::GetRotationMatrix(BODY_EARTH, GMTBASE)), _M(cos(lambda), -sin(lambda), 0, sin(lambda), cos(lambda), 0, 0, 0, 1));
	//Converts from RTCC ecliptic to S-IVB ephemeral
	MATRIX3 MRG = mul(_M(1, 0, 0, 0, 0, -1, 0, 1, 0), OrbMech::tmat(RMAT));
	double phi_L = MDVSTP.PHIL;
	MATRIX3 MSG = _M(cos(phi_L), sin(phi_L)*MCLSBN, -sin(phi_L)*MCLCBN, -sin(phi_L), cos(phi_L)*MCLSBN, -cos(phi_L)*MCLCBN, 0, MCLCBN, MCLSBN);
	MATRIX3 MRS = mul(OrbMech::tmat(MSG), MRG);

	CZNAVSLV.PosS = mul(MRS, R_ecl);
	CZNAVSLV.DotS = mul(MRS, V_ecl);
	CZNAVSLV.NUPTIM = GMT - MCGRIC * 3600.0;
}