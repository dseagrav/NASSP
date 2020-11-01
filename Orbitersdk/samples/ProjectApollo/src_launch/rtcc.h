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

#if !defined(_PA_RTCC_H)
#define _PA_RTCC_H

#include <vector>
#include <deque>
#include "../src_rtccmfd/OrbMech.h"
#include "../src_rtccmfd/LDPP.h"
#include "../src_rtccmfd/EntryCalculations.h"
#include "../src_rtccmfd/RTCCTables.h"
#include "../src_rtccmfd/TLMCC.h"
#include "../src_rtccmfd/LOITargeting.h"
#include "../src_rtccmfd/LMGuidanceSim.h"
#include "MCCPADForms.h"

#define RTCC_START_STRING	"RTCC_BEGIN"
#define RTCC_END_STRING	    "RTCC_END"

#define RTCC_LAMBERT_MULTIAXIS 0
#define RTCC_LAMBERT_XAXIS 1

#define RTCC_LAMBERT_SPHERICAL 0
#define RTCC_LAMBERT_PERTURBED 1

#define RTCC_ENTRY_DEORBIT 0
#define RTCC_ENTRY_MCC 1
#define RTCC_ENTRY_ABORT 2
#define RTCC_ENTRY_CORRIDOR 3

#define RTCC_ENTRY_MINDV 0
#define RTCC_ENTRY_NOMINAL 1

#define RTCC_MPT_CSM 1
#define RTCC_MPT_SIVB 2
#define RTCC_MPT_LM 3

//CSM present
#define RTCC_CONFIG_C 0
//S-IVB present
#define RTCC_CONFIG_S 1
//LM ascent stage present
#define RTCC_CONFIG_A 2
//LM descent stage present
#define RTCC_CONFIG_D 3

#define RTCC_CONFIGCHANGE_NONE 0
#define RTCC_CONFIGCHANGE_UNDOCKING 1
#define RTCC_CONFIGCHANGE_DOCKING 2

#define RTCC_ENGINETYPE_CSMRCSPLUS2 1
#define RTCC_ENGINETYPE_CSMRCSPLUS4 2
#define RTCC_ENGINETYPE_CSMRCSMINUS2 3
#define RTCC_ENGINETYPE_CSMRCSMINUS4 4
#define RTCC_ENGINETYPE_LOX_DUMP 16
#define RTCC_ENGINETYPE_LMRCSPLUS2 17
#define RTCC_ENGINETYPE_LMRCSPLUS4 18
#define RTCC_ENGINETYPE_LMRCSMINUS2 19
#define RTCC_ENGINETYPE_LMRCSMINUS4 20
#define RTCC_ENGINETYPE_CSMSPS 33
#define RTCC_ENGINETYPE_LMAPS 34
#define RTCC_ENGINETYPE_LMDPS 35
#define RTCC_ENGINETYPE_SIVB_MAIN 36
#define RTCC_ENGINETYPE_SIVB_APS 37

#define RTCC_MANVEHICLE_CSM 1
#define RTCC_MANVEHICLE_SIVB 2
#define RTCC_MANVEHICLE_LM 3

#define RTCC_ATTITUDE_INERTIAL 1
#define RTCC_ATTITUDE_MANUAL 2
#define RTCC_ATTITUDE_LAMBERT 3
#define RTCC_ATTITUDE_PGNS_EXDV 4
#define RTCC_ATTITUDE_AGS_EXDV 5
#define RTCC_ATTITUDE_SIVB_IGM 6
#define RTCC_ATTITUDE_PGNS_DESCENT 7
#define RTCC_ATTITUDE_PGNS_ASCENT 8
#define RTCC_ATTITUDE_AGS_ASCENT 9

#define RTCC_REFSMMAT_TYPE_CUR 1
#define RTCC_REFSMMAT_TYPE_PCR 2
#define RTCC_REFSMMAT_TYPE_TLM 3
#define RTCC_REFSMMAT_TYPE_OST 4
#define RTCC_REFSMMAT_TYPE_MED 5
#define RTCC_REFSMMAT_TYPE_DMT 6
#define RTCC_REFSMMAT_TYPE_DOD 7
#define RTCC_REFSMMAT_TYPE_LCV 8
#define RTCC_REFSMMAT_TYPE_AGS 9
#define RTCC_REFSMMAT_TYPE_DOK 10
#define RTCC_REFSMMAT_TYPE_LLA 11
#define RTCC_REFSMMAT_TYPE_LLD 12

#define RTCC_LMPOS_BEST 0
#define RTCC_LMPOS_PGNCS 1
#define RTCC_LMPOS_AGS 2
#define RTCC_LMPOS_MED 3

#define RTCC_GMP_PCE 1
#define RTCC_GMP_PCL 2
#define RTCC_GMP_PCT 3
#define RTCC_GMP_CRL 4
//Circularization at a specified height
#define RTCC_GMP_CRH 5
#define RTCC_GMP_HOL 6
#define RTCC_GMP_HOT 7
#define RTCC_GMP_HAO 8
#define RTCC_GMP_HPO 9
#define RTCC_GMP_NST 10
#define RTCC_GMP_NSO 11
#define RTCC_GMP_HBT 12
#define RTCC_GMP_HBH 13
#define RTCC_GMP_HBO 14
#define RTCC_GMP_FCT 15
#define RTCC_GMP_FCL 16
#define RTCC_GMP_FCH 17
#define RTCC_GMP_FCA 18
#define RTCC_GMP_FCP 19
#define RTCC_GMP_FCE 20
#define RTCC_GMP_NHT 21
//Combination maneuver to change both apogee and perigee and shift the node at a specified longitude
#define RTCC_GMP_NHL 22
//Maneuver to shift line-of-apsides some angle at a specified longitude
#define RTCC_GMP_SAL 23
//Maneuver to shift line-of-apsides to a specified longitude
#define RTCC_GMP_SAA 24
#define RTCC_GMP_PHL 25
#define RTCC_GMP_PHT 26
#define RTCC_GMP_PHA 27
#define RTCC_GMP_PHP 28
#define RTCC_GMP_CPL 29
#define RTCC_GMP_CPH 30
#define RTCC_GMP_SAT 31
//Maneuver to shift line-of-apsides some angle and keep the same apogee and perigee altitudes
#define RTCC_GMP_SAO 32
//Maneuver to change both apogee and perigee at a specified longitude
#define RTCC_GMP_HBL 33
#define RTCC_GMP_CNL 34
#define RTCC_GMP_CNH 35
//Height maneuver and node shift at a specified longitude
#define RTCC_GMP_HNL 36
#define RTCC_GMP_HNT 37
#define RTCC_GMP_HNA 38
#define RTCC_GMP_HNP 39
#define RTCC_GMP_CRT 40
#define RTCC_GMP_CRA 41
#define RTCC_GMP_CRP 42
#define RTCC_GMP_CPT 43
#define RTCC_GMP_CPA 44
#define RTCC_GMP_CPP 45
#define RTCC_GMP_CNT 46
#define RTCC_GMP_CNA 47
#define RTCC_GMP_CNP 48
#define RTCC_GMP_PCH 49
#define RTCC_GMP_NSH 50
//Node shift at a longitude
#define RTCC_GMP_NSL 51
#define RTCC_GMP_HOH 52
//Maneuver to change both apogee and perigee and place perigee at a certain longitude a certain number of revs later
#define RTCC_GMP_HAS 53

const double LaunchMJD[11] = {//Launch MJD of Apollo missions
	40140.62691,
	40211.535417,
	40283.666667,
	40359.700694,
	40418.563889,
	40539.68194,
	40687.80069,
	40982.849306,
	41158.5652869,
	41423.74583,
	41658.120139
};

//MANUAL ENTRY DEVICES

//Computation for Lunar Descent Planning
struct MED_K16
{
	double VectorTime = 0.0;
	int Mode = 1;
	int Sequence = 1;
	double GETTH1 = 0.0;
	double GETTH2 = 0.0;
	double GETTH3 = 0.0;
	double GETTH4 = 0.0;
	double DesiredHeight = 60.0*1852.0;
	int Vehicle = RTCC_MPT_CSM; //1 = CSM, 3 = LEM (Instead of Vector ID)
};

//Initialization for Lunar Descent Planning
struct MED_K17
{
	double Azimuth = 0.0;					//Greater or equal to zero, lower than 360°. If 0, LDPP will compute azimuth
	double DescIgnHeight = 50000.0*0.3048;	//Feet
	bool PoweredDescSimFlag = false;//true = simulate powered descent
	double PoweredDescTime = 0.0;	//Time for powered descent ignition
	int DwellOrbits = 0;			//Number of dwell orbits desired between DOI and PDI
	double DescentFlightTime = 11.0*60.0; //Minutes
	double DescentFlightArc = 15.0*RAD;	//Degrees
};

//Change Vehicle Weight
struct MED_M50
{
	int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
	double CSMWT = -1.0;
	double SIVBWT = -1.0;
	double LMWT = -1.0;
	double LMASCWT = -1.0;
	double WeightGET = 0.0;
};

//Change Vehicle Area or K-Factor
struct MED_M51
{
	int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
	double CSMArea = 0.0;
	double SIVBArea = 0.0;
	double LMAscentArea = 0.0;
	double LMDescentArea = 0.0;
	double KFactor = 0.0;
};

//Input initial configuration for Mission Plan Table
struct MED_M55
{
	int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
	std::string ConfigCode;
	double VentingGET = 0.0;
	double DeltaDockingAngle = 0.0;
};

//TLI Direct Input
struct MED_M68
{
	int Table = 2; //1 = LEM, 2 = CSM
	int Opportunity = 1; //1-2
};

//Transfer a DKI, SPQ, or a Descent Plan to the MPT
struct MED_M70
{
	int Plan = 0; //-1 = Descent Plan, 0 = SPQ, 1 = DKI
	double DeleteGET = 0.0;
	int Thruster = RTCC_ENGINETYPE_CSMRCSPLUS2; //Thruster for the maneuver
	int Attitude = 4;		//Attitude option (1 = Inertial, 2 = Manual, 3 = Lambert, 4 = PGNS External DV, 5 = AGS External DV)
	double UllageDT = -1;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool Iteration = false; //false = do not iterate, true = iterate
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
	bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
};

//Transfer a Two Impulse Maneuver to the MPT
struct MED_M72
{
	int Table = 1; //1 = multiple solution, 2 = corrective solution
	int Plan = 1; // Plan number to be transferred
	double DeleteGET = 0.0; //Deletes all maneuvers in both tables occurring after the input GET (no delete if 0)
	int Thruster = RTCC_ENGINETYPE_CSMRCSPLUS4; //Thruster for the maneuver
	int Attitude = RTCC_ATTITUDE_PGNS_EXDV;		//Attitude option
	double UllageDT = -1;	//Delta T of Ullage
	bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
	bool Iteration = false; //false = do not iterate, true = iterate
	double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
	double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
	bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
};

struct TwoImpulseOpt
{
	int mode;		//1 = Corrective Combination (NCC), 2 = Multiple Solution/Two-Impulse Computation (TPI), 3 = Single Solution, 4 = Transfer Plan, 5 = DKI/SPQ
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	EphemerisData sv_A;		//Chaser state vector
	EphemerisData sv_P;		//Target state vector
	int ChaserVehicle = 1;	//1 = CSM, 3 = LEM
	//0 = Time of both maneuvers fixed, 1 = time of first maneuver fixed, 2 = time of second maneuver fixed
	int IVFLAG = 0;
	double TimeStep = 10.0;
	double TimeRange = 0.0;

	//Corrective combination options
	//Minimum height difference
	double DH_min;
	//Maximum height difference
	double DH_max;
	double T2_min;
	double T2_max;
	//0 = use item 11 as time increment of second maneuver
	//1 = use item 11 as terminal phase slip time increment
	int CCReqInd;
	//Item 11: Time increment of second maneuver
	double dt_inc;
	double TPILimit;
	//Height increment
	double dh_inc;

	//Single solution options
	//1 = Multiple, 2 = Corrective Combination
	int SingSolTable = 1;
	//1 to 13
	int SingSolNum = 1;
	//false = 2 quads, true = 4 quads
	bool UllageQuads = true;
	//false = Target, true = Horizon
	bool LOSMode = false;
	double DeltaPitch = 0.0;
	double RelMoTimeStep = 0.0;

	//External request
	double DH = 0.0;
	double PhaseAngle = 0.0;
	double WT = 0.0;
};

struct LambertMan //Data for Lambert targeting
{
	int mode;		//0 = General, 1 = Corrective Combination (NCC), 2 = Two-Impulse Computation (TPI)
	double GETbase; //usually MJD at launch
	double T1;	//GET of the maneuver
	double T2;	// GET of the arrival
	int N;		//number of revolutions
	int axis;	//Multi-axis or horizontal burn
	int Perturbation; //Spherical or non-spherical gravity
	VECTOR3 Offset = _V(0, 0, 0); //Offset vector
	SV sv_A;		//Chaser state vector
	SV sv_P;		//Target state vector
	int ChaserVehicle = 1;	//1 = CSM, 3 = LEM
	bool storesolns = false;

	//For mode 1 and 2
	double PhaseAngle = 0.0;
	double DH = 0.0;
	double ElevationAngle = 26.6*RAD;
	double TravelAngle = 130.0*RAD;
};

struct AP7ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int enginetype = RTCC_ENGINETYPE_CSMSPS; //Engine type used for the maneuver
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime; //time delay for the sextant star check (in case no star is available during the maneuver)
	double navcheckGET; //Time for the navcheck. 0 = no nav check
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct AP11ManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int enginetype = RTCC_ENGINETYPE_CSMSPS; //Engine type used for the maneuver
	bool HeadsUp; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime = 0; //time delay for the sextant star check (in case no star is available during the maneuver)
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	double R_LLS = OrbMech::R_Moon;	//Landing site radius
};

struct AP11LMManPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int enginetype = RTCC_ENGINETYPE_LMDPS; //Engine type used for the maneuver
	bool HeadsUp = false; //Orientation during the maneuver
	MATRIX3 REFSMMAT;//REFSMMAT during the maneuver
	double sxtstardtime = 0; //time delay for the sextant star check (in case no star is available during the maneuver)
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	double R_LLS = OrbMech::R_Moon;	//Landing site radius
};

struct AP10CSIPADOpt
{
	SV sv0;
	double GETbase;
	double t_CSI;
	double t_TPI;
	VECTOR3 dV_LVLH;
	MATRIX3 REFSMMAT;
	int enginetype = RTCC_ENGINETYPE_CSMRCSPLUS4; //Engine type used for the maneuver
	double KFactor = 0.0;
};

struct AP7TPIPADOpt
{
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	SV sv_A;
	SV sv_P;
};

struct AP9LMTPIPADOpt
{
	SV sv_A; //Chaser state vector
	SV sv_P; //Target state vector
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;	//REFSMMAT
};

struct AP9LMCDHPADOpt
{
	SV sv_A; //Chaser state vector
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;	//REFSMMAT
};

struct EarthEntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double GETbase; //usually MJD at launch
	double TIGguess; //Initial estimate for the TIG
	double ReA = 0; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	bool nominal; //Calculates minimum DV deorbit or nominal 31.7° line deorbit
	int enginetype = RTCC_ENGINETYPE_CSMSPS;		//Engine type used for the maneuver
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool prediction = false;	//0 = Entry maneuver, 1 = Entry prediction
};

struct EntryOpt
{
	VESSEL* vessel; //Reentry vessel
	double GETbase; //usually MJD at launch
	double TIGguess; //Initial estimate for the TIG or baseline TIG for abort and MCC maneuvers
	int type; //Type of reentry maneuver
	double ReA = 0; //Reentry angle at entry interface, 0 starts iteration to find reentry angle
	double lng; //Longitude of the desired splashdown coordinates
	int enginetype;		//Engine type used for the maneuver
	bool entrylongmanual; //Targeting a landing zone or a manual landing longitude
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	// relative range override
	double r_rbias = 1285.0;
	//Maximum DV
	double dv_max = 2804.0;
};

struct EntryResults
{
	VECTOR3 dV_LVLH;
	double P30TIG;
	double latitude, longitude;
	double GET05G, GET400K, RTGO, VIO, ReA, Incl, FlybyAlt;
	int precision;
	SV sv_preburn;
	SV sv_postburn;
	bool solutionfound = false;
};


struct TLMCCResults
{
	VECTOR3 DV;
	double TIG;
	double SplashdownLat, SplashdownLng;
	double PericynthionGET;
	double EntryInterfaceGET;
	double NodeLat;
	double NodeLng;
	double NodeAlt;
	double NodeGET;
	double EMPLatitude;
	double FRInclination;
	double t_Rev2Meridian;
	VECTOR3 dV_LVLH_MCC;
	VECTOR3 dV_LVLH_LOI;
	VECTOR3 dV_LVLH_DOI;
	double h_peri_postDOI, h_apo_postDOI;
};

struct TwoImpulseResuls
{
	VECTOR3 dV;
	VECTOR3 dV2;
	VECTOR3 dV_LVLH;
	VECTOR3 dV_LVLH2;
	double t_TPI;
	double T1;
	double T2;
	bool SolutionFound;
};

struct SPQResults
{
	double t_CSI;
	double t_CDH;
	double t_TPI;
	double DH;
	VECTOR3 dV_CSI;
	VECTOR3 dV_CDH;
	SV sv_C[5];
	SV sv_C_apo[5];
	SV sv_T[5];
};

struct RTEMoonOpt
{
	VESSEL* vessel;			//Reentry vessel
	double GETbase;			//usually MJD at launch
	double TIGguess = 0.0;	//Initial estimate for the TIG
	double EntryLng;		//Entry longitude
	int returnspeed = -1;	//0 = slow return, 1 = normal return, 2 = fast return
	SV RV_MCC;				//State vector as input
	int RevsTillTEI = 0;	//Revolutions until TEI
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool entrylongmanual = true; //Targeting a landing zone or a manual landing longitude
	int enginetype = RTCC_ENGINETYPE_CSMSPS; //Engine type used for the maneuver
	double Inclination = 0.0;	//Specified return inclination (sign is azimuth option)
	// 12: PTP discrete option (not implemented yet)
	// 14: ATP discrete option
	// 16: UA discrete option
	// 22: PTP tradeoff display (not implemented yet)
	// 24: ATP tradeoff display (not implemented yet)
	// 32: PTP search option (not implemented yet)
	// 34: ATP search option
	// 36: UA search option
	int SMODE = 34;
	//Approximate landing time
	double t_zmin = 0;
	int ATPLine = 0;
};

struct TradeoffDataDisplay
{
	TradeoffData data[10][44];
	unsigned curves;
	unsigned NumInCurve[10];
};

struct TradeoffDataDisplayBuffer
{
	TradeoffDataDisplayBuffer();

	double xval[10][44];
	double yval[10][44];
	unsigned curves;
	unsigned NumInCurve[10];
	int XLabels[3];
	int YLabels[3];
	int TZDisplay[10];
	double TZxval[10];
	double TZyval[10];
	std::string XAxisName;
	std::string YAxisName;
	std::string Site;
};

struct REFSMMATOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	int REFSMMATopt; //REFSMMAT options: 0 = P30 Maneuver, 1 = P30 Retro, 2= LVLH, 3= Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = Attitude, 8 = LS during TLC
	double REFSMMATTime; //Time for the REFSMMAT calculation
	double LSLng; //longitude for the landing site REFSMMAT
	double LSLat; //latitude for the landing site REFSMMAT
	double LSAzi; //approach azimuth for the landing site REFSMMAT
	int mission; //Just for the launch REFSMMAT
	bool csmlmdocked = false;	//0 = CSM or LM alone, 1 = CSM/LM docked
	bool HeadsUp = true; //Orientation during the maneuver
	int vesseltype = 0; //0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	MATRIX3 PresentREFSMMAT;	//Present REFSMMAT (for option 9)
	VECTOR3 IMUAngles;			//Desired Attitude (for option 9)
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;				//State vector as input
};

struct CDHOpt
{
	VESSEL* vessel; //Vessel executing the burn
	VESSEL* target; //Target vessel
	double GETbase; //usually MJD at launch
	double DH; //Delta Height
	int CDHtimemode; //0 = Fixed Time, 1 = Find GETI
	double TIG; // (Estimated) Time of Ignition
	int impulsive; //Calculated with nonimpulsive maneuver compensation or without
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int vesseltype = 0;			//0 = CSM, 1 = LM
};

struct AP7BLKOpt
{
	int n; //number of PAD entries
	std::vector<double> lng; //Splashdown longitudes
	std::vector<double> GETI; //Ignition times
	std::vector<std::string> area; //Splashdown areas
};

struct AP11BLKOpt
{
	int n; //number of PAD entries
	std::vector<double> lng; //Splashdown longitudes
	std::vector<double> GETI; //Ignition times
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
};

struct EarthEntryPADOpt
{
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (deorbit maneuver)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (deorbit maneuver)
	MATRIX3 REFSMMAT;
	bool preburn; //
	double lat; //splashdown latitude
	double lng; //splashdown longitude
	SV sv0;
};

struct LunarEntryPADOpt
{
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (MCC)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (MCC)
	MATRIX3 REFSMMAT;
	bool direct = true; //0 = with MCC, 1 = without
	double lat; //splashdown latitude
	double lng; //splashdown longitude
	SV sv0;
};

struct TLIManNode
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TLI_TIG; //Initial guess for TLI TIG
	double lat; //selenographic latitude
	double lng; //selenographic longitude
	double PeriGET; //time of pericynthion, initial guess
	double h_peri;	//flyby altitude
	SV RV_MCC;		//State vector as input
};

struct TLIManFR
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double TLI_TIG; //Initial guess for TLI TIG
	double lat; //Earth-Moon-Plane
	double PeriGET; //time of pericynthion, initial guess
	double h_peri;	//flyby altitude
	SV RV_MCC;		//State vector as input
};

struct MCCNodeMan
{
	double GETbase; //usually MJD at launch
	double MCCGET; //TIG for the MCC
	double lat; //target for MCC, selenographic latitude
	double lng; //selenographic longitude
	double NodeGET; //GET at node
	double h_node;	//node altitude
	SV RV_MCC;		//State vector as input
};

struct MCCFRMan
{
	double GETbase; //usually MJD at launch
	int type; //0 = Fixed LPO, 1 = Free LPO
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	SV RV_MCC;		//State vector as input

	//LOI targets for BAP
	double LSlat;			//landing site latitude
	double LSlng;			//landing site longitude
	double R_LLS;			//landing site radius
	double azi;			//landing site approach azimuth
	double t_land;		//time of landing
	double LOIh_apo;		//apolune altitude
	double LOIh_peri;		//perilune altitude
};

struct MCCNFRMan
{
	int type;		//0 = fixed LPO, 1 = free LPO
	double GETbase; //usually MJD at launch
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	SV RV_MCC;		//State vector as input

	//LOI targets for BAP
	double LSlat;			//landing site latitude
	double LSlng;			//landing site longitude
	double R_LLS;			//landing site radius
	double azi;			//landing site approach azimuth
	double t_land;		//time of landing
	double LOIh_apo;		//apolune altitude
	double LOIh_peri;		//perilune altitude
	int N;				//Revs between DOI and PDI
	int DOIType;		//0 = Normal DOI, 1 = DOI as LOI-2
	double DOIPeriAng;	//Angle between landing site and perilune
	int LOIEllipseRotation = 0;	//0 = Choose the lowest DV solution, 1 = solution 1, 2 = solution 2
	double DOIPeriAlt = 50000.0*0.3048; //perilune altitude above landing site
};

struct MCCFlybyMan
{
	double GETbase; //usually MJD at launch
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	SV RV_MCC;		//State vector as input
};

struct MCCSPSLunarFlybyMan
{
	double GETbase; //usually MJD at launch
	double MCCGET; //GET for the MCC
	double lat; //Earth-Moon-Plane latitude, initial guess
	double PeriGET; //initial guess for the GET at pericynthion
	double h_peri;	//pericynthion altitude
	double FRInclination;
	bool AscendingNode;
	SV RV_MCC;		//State vector as input
};

struct LOIMan
{
	VESSEL* vessel;		//vessel
	int type = 0;		//0 = fixed approach azimuth, 1 = LOI at pericynthion
	double GETbase;		//usually MJD at launch
	double lat;			//landing site latitude
	double lng;			//landing site longitude
	double R_LLS;		//landing site radius
	double azi;			//landing site approach azimuth
	double t_land;		//time of landing
	double h_apo;		//apolune altitude
	double h_peri;		//perilune altitude
	SV RV_MCC;			//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	int enginetype = RTCC_ENGINETYPE_CSMSPS;	//Engine type to use for maneuver
	int impulsive = 0;	//0 = finite burntime, 1 = impulsive
	int EllipseRotation = 0;	//0 = Choose the lowest DV solution, 1 = solution 1, 2 = solution 2
	int plan = 1;		//1 = CSM, 3 = LEM
};

struct DOIMan
{
	int opt;		//0 = DOI from circular orbit, 1 = DOI as LOI-2
	double GETbase; //usually MJD at launch
	double EarliestGET;	//Earliest GET for the DOI maneuver
	double lat; //landing site latitude
	double lng; //landing site longitude
	double R_LLS;	//landing site radius
	SV sv0;		//State vector as input
	int N = 0;	// Revolutions between DOI and PDI
	double PeriAng = 15.0*RAD;	//Angle from landing site to perilune
	double PeriAlt = 50000.0*0.3048; //perilune altitude above landing site
};

struct PCMan
{
	VESSEL* vessel; //vessel
	VESSEL* target; //target
	double GETbase; //usually MJD at launch
	double EarliestGET;	//Earliest GET for the PC maneuver
	double t_A; //time when the orbit is aligned with the landing site
	SV RV_MCC;		//State vector as input
	bool csmlmdocked = false; //0 = CSM/LM alone, 1 = CSM/LM docked
	bool landed; //0 = use lat/lng/alt to calculate landing site, 1 = target vessel on the surface
	double lat; //landing site latitude
	double lng; //landing site longitude
	double alt;	//altitude of the landing site
};

struct GMPOpt
{
	//0 = Fixed TIG, specify inclination, apoapsis and periapsis altitude
	//1 = Fixed TIG, specify apoapsis altitude
	//2 = Fixed TIG, specify periapsis altitude
	//3 = Fixed TIG, circularize orbit
	//4 = Circularize orbit at specified altitude
	//5 = Rotate velocity vector, specify apoapsis altitude
	//6 = Rotate line of apsides, perigee at specific longitude
	//7 = Optimal node shift maneuver
	int ManeuverCode;
	double GETbase; //usually MJD at launch
	SV RV_MCC;		//State vector as inputn or without
	bool AltRef = 0;	//0 = use mean radius, 1 = use launchpad or landing site radius
	double R_LLS;	//Landing site radius

	//maneuver parameters

	double TIG_GET;	//Threshold time or time of ignition
	double dW;		//Desired wedge angle change
	double long_D;	//Desired maneuver longitude
	double H_D;		//Desired maneuver height
	double dH_D;	//Desired change in height
	double dLAN;	//Desired change in the ascending node
	double H_A;		//Desired apogee height after the maneuver
	double H_P;		//Desired perigee height after the maneuver
	double dV;		//Input incremental velocity magnitude of the maneuver
	double Pitch;	//Input pitch of the maneuver
	double Yaw;		//Input yaw of the maneuver
	double dLOA;	//Line-of-apsides shift
	int N;			//Number of revolutions
};

struct TLIPADOpt
{
	VESSEL* vessel; //vessel
	SV sv0; //vessel state vector
	double GETbase; //usually MJD at launch
	double TIG; //Time of Ignition
	double TLI;	//Time of Injection
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates
	MATRIX3 REFSMMAT;
	VECTOR3 SeparationAttitude; //LVLH IMU angles
	VECTOR3 R_TLI;
	VECTOR3 V_TLI;
	bool uselvdc;	//LVDC in use/or not
};

struct P27Opt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double SVGET; //GET of the state vector
	double navcheckGET; //GET of the Nav Check
};

struct AGSSVOpt
{
	SV sv;
	MATRIX3 REFSMMAT;
	bool csm;
	double GETbase;
	double AGSbase;
};

struct SkyRendOpt
{
	VESSEL* vessel;		//vessel
	VESSEL* target;		//Target vessel
	double GETbase;		//usually MJD at launch
	int man;			//0 = Presettings, 1 = NC1, 2 = NC2, 3 = NCC, 4 = NSR, 5 = TPI, 6 = TPM, 7 = NPC
	bool PCManeuver;	//0 = NC1 is setting up NPC, 1 = NC2 is setting up NPC
	bool NPCOption;		//0 = NC1 or NC2 with out-of-plane component, setting up a NPC maneuver 90° later
	double TPIGuess;	//Estimate for the TPI time
	double t_TPI;		//Time of TPI
	double E_L;			//Elevation angle at TPI
	double t_C;			//Time of Ignition
	double DH1;			//Delta Height at NCC
	double DH2;			//Delta Height at NSR
	double n_C;
	double t_NC;		//Reference time for the NPC maneuver
	bool useSV = false;		//true if state vector is to be used
	SV RV_MCC;		//State vector as input
	bool csmlmdocked; //0 = CSM alone, 1 = CSM/DM
};

struct SkylabRendezvousResults
{
	double P30TIG;
	VECTOR3 dV_LVLH;

	double t_NC2;
	double t_NCC;
	double t_NSR;
	double t_TPI;

	double dv_NC2;
	double dv_NCC;

	double dH_NC2;

	VECTOR3 dV_NSR;
};

struct LunarLiftoffTimeOpt
{
	LunarLiftoffTimeOpt();

	//Flag that controls at which time CSI is done
	//0: CSI is done 90° from insertion
	//1: CSI is done at an input elapsed time from insertion
	//2: CSI is done at LM apocynthion
	int I_BURN;
	//Flag that controls at what position TPI is done
	//1: TPI is done over an input longitude
	//2: TPI is at an input time
	int I_TPI;
	//Flag that controls CDH position
	//0: CDH is done at the upcoming apsis after CSI
	//1, 3, 5, etc., CDH is done I_CDH*period/2 after CSI
	int I_CDH;
	//Time base for CSI if CSM is active vehicle and CSI was done on an apsis
	double t_BASE;
	//Flag that controls the search option for the absolute opening and closing of the launch window
	//0: compute the launch window for only the input DH table
	//1: compute the complete window
	int I_SRCH;
	//Flag that controls position for final maneuver
	//0: Compute final maneuver for a rendezvous solution
	//1: compute final maneuver for an offset in height and phase
	int I_OS;
	//Number of maneuvering vehicle
	//1: CSM is maneuvering vehicle
	//2: LM is maneuvering vehicle
	int M;
	//Number of non-maneuvering vehicle
	//1: CSM is non-maneuvering vehicle
	//2: LM is non-maneuvering vehicle
	int P;
	//Number of points used in the curve fit
	int N_CURV;
	//Number of entries in the DH table
	int L_DH;
	//Maximum in-orbit lifetime of the LM ascent stage pwoer systems
	double t_max;
	//Minimum safe height of an orbiting vehicle
	double H_S;
	//Rendezvous DV budget for both vehicles
	double DV_MAX[2];
	//Input table of coelliptic altitude differences
	double  DH[3];
	double theta_1;		//Angle travelled between liftoff and insertion
	double v_LH;
	double dt_1;		//Ascent burn time (liftoff to insertion)
	double v_LV;
	//Height at insertion
	double h_BO;
	//Angle from TPI to the earth-moon line
	double phi_TPI;
	//Yaw steer capability of LM
	double Y_S;
	//Time to travel from insertion to CSI
	double DT_B;
	//Height difference to begin curve fit with
	double DH_SRCH;
	//Height difference to increment curve fit with
	double DH_STEP;
	//Transfer angle for terminal phase
	double theta_F;
	//Elevation angle to initiate terminal phase on
	double E;
	//Offset height difference
	double DH_OFF;
	//Phase angle offset
	double dTheta_OFF;
	//Threshold time to determine which launch window is obtained
	double t_hole;
	//Time of TPI
	double t_TPI;
	double lat;
	double lng;
	double R_LLS;
	//Longitude at which TPI is to be scheduled
	double lng_TPI;

	double GETbase;		//usually MJD at launch
	SV sv_CSM;			//CSM State vector
	
	bool IsInsVelInput;	//0 = calculate insertion velocity, 1 = use input velocity
};

struct LLTPOpt
{
	EphemerisData sv_CSM;
	double Y_S;
	double V_Z_NOM;
	double T_TH;
	double R_LS, lat_LS, lng_LS;
	double alpha_PF, dt_PF;
	double dt_INS_TPI;
	double h_INS;
	double DH_TPI, dTheta_TPI;
	double WT;
};

struct LunarLaunchTargetingTable
{
	double GETLOR = 0.0;
	double VH = 0.0;
	double GET_TPI = 0.0, GET_TPF = 0.0;
	double DV_TPI = 0.0, DV_TPF = 0.0;
	VECTOR3 DV_TPI_LVLH = _V(0, 0, 0), DV_TPF_LVLH = _V(0, 0, 0);
	double HA_TPI = 0.0, HP_TPI = 0.0;
	double HA_TPF = 0.0, HP_TPF = 0.0;
	double HA_T = 0.0, HP_T = 0.0;
};

struct LunarLiftoffResults
{
	double t_L;
	double t_Ins;
	double t_CSI;
	double t_CDH;
	double t_TPI;
	double t_TPF;
	double v_LH;
	double v_LV;
	double DV_CSI;
	double DV_CDH;
	double DV_TPI;
	double DV_TPF;
	double DV_T;
};

struct PDIPADOpt
{
	VESSEL* vessel; //vessel
	double GETbase; //usually MJD at launch
	double P30TIG; //Time of Ignition (MCC)
	VECTOR3 dV_LVLH; //Delta V in LVLH coordinates (MCC)
	MATRIX3 REFSMMAT;
	bool direct; //0 = with MCC, 1 = without
	VECTOR3 R_LS;	//Landing Site Vector
	double t_land;
	bool HeadsUp; //Orientation during the maneuver
	SV sv0;
};

struct ASCPADOpt
{
	double GETbase;
	VECTOR3 R_LS;
	double TIG;
	double v_LH;
	double v_LV;
	SV sv_CSM;
	MATRIX3 Rot_VL;	//Rotation Matrix, vessel to local, left-handed
};

struct LMARKTRKPADOpt
{
	SV sv0; //Input state vector
	double GETbase; //usually MJD at launch
	double LmkTime[4]; //initial guess for time over landmark
	double lat[4];		//landmark latitude
	double lng[4];		//landmark longitude
	double alt[4] = { 0,0,0,0 };	//landmark altitude
	int entries;
};

struct DKIOpt	//Docking Initiation Processor
{
	SV sv_A;
	SV sv_P;
	double GETbase;
	double t_TIG;
	double t_TPI_guess;
	double DH;
	double E;
	int ChaserID;
	int tpimode = 0;	//0 = TPI on time, 1 = TPI at orbital midnight, 2 = TPI at X minutes before sunrise
	//DKI RENDEZVOUS PLANS:
	//0 = Phasing, CSI 0.5 revs later, CDH 0.5 revs later
	//1 = Phasing with +50 ft/s DVZ, at apolune, CDH 0.5 revs later
	//2 = Height, CSI (Phasing) 0.5 revs later, CDH 2xN revs later
	//3 = Just calculate TPI time
	//4 = High Dwell Sequence
	int plan = 0;
	bool maneuverline = true;	//false = use input delta times, true = use 0.5 revolutions
	bool radial_dv = false;		//false = horizontal maneuver, true = 50 ft/s radial component
	int N_HC = 1;				//Number of half revs between CSI and CDH
	int N_PB = 1;				//Number of half revs between Phasing and Boost/Height

	double dt_TPI_sunrise = 16.0*60.0;
	double DeltaT_PBH = 55.0*60.0;	//Delta time between phasing and boost/CSI
	double DeltaT_BHAM = 60.0*60.0;	//Delta time between boost and HAM
	double Delta_HAMH = 60.0*60.0;	//Delta time between HAM and CSI
};

struct DKIResults
{
	VECTOR3 DV_Phasing;
	double t_Boost;
	double dv_Boost;
	double t_HAM;
	double t_CSI;
	double dv_CSI;
	double t_CDH;
	VECTOR3 DV_CDH;
	double t_TPI;
};

struct SPQOpt //Coelliptic Sequence Processor
{
	SV sv_A;
	SV sv_P;
	double GETbase;
	//Time of CSI maneuver (<= 0 if not scheduled)
	double t_CSI;
	//Time of CDH maneuver
	double t_CDH;
	double t_TPI;				// Only for calculation type = 0
	double DH = 15.0*1852.0;	// Only for calculation type = 1
	double E = 26.6*RAD;
	double WT = 130.0*RAD;
	//Height iteration. 0 = fixed TIG at TPI, 1 = fixed DH at CDH
	int K_CDH;
	int K_TPI = 0;		//-1 = Midpoint of darkness, 0 = on time, 1 = on longitude
	int ChaserID = 1;
	//0 = Plane change not requested, 1 = plane change requested
	bool N_PC = false;
	//Plane change threshold
	double T_BNPC = 0.0;
	//Initial phase angle (0 = -180° to 180°, 1 = 0 to 180°, 2 = -180° to 0)
	int I_Theta = 0;
	//0 = CDH not scheduled, 1 = CDH scheduled
	bool CDH = true;
	//1 = CDH at next apsis, 2 = CDH on time, 3 = angle from CSI
	int I_CDH = 3;
	//Number of apsis since CSI (for CDH at next apsis option)
	int N_CDH = 1;
	bool OptimumCSI = false;
	//0 = CSI and CDH in-plane, 1 = CSI and CDH parallel to target
	bool ParallelDVInd = false;
	//Angle from CSI to CDH (for I_CDH = 3)
	double DU_D = PI;
};

struct PDAPOpt //Powered Descent Abort Program
{
	SV sv_A;
	SV sv_P;
	double GETbase;
	double TLAND;
	MATRIX3 REFSMMAT;
	VECTOR3 R_LS;
	double dt_stage;
	//LM vehicle weight immediately after staging
	double W_TAPS;
	//LM weight representative of DPS fuel depletion
	double W_TDRY;
	//DT between successive abort points
	double dt_step;
	//Time from Insertion to CSI
	double dt_CSI = 50.0*60.0;
	//Time of TPI
	double t_TPI;
	//dt added to dt_CSI for generation of the second set of targeting coefficients
	double dt_2CSI = 110.0*60.0;
	//dt added to t_TPI for generation of the second set of targeting coefficients
	double dt_2TPI = 7117.0;
	//One or two segments (Apollo 11 style vs. all later missions)
	bool IsTwoSegment = false;
	//time between orbit insertion and the canned maneuver
	double dt_CAN = 50.0*60.0;
	//DV of the canned maneuver
	double dv_CAN = 10.0*0.3048;
	//Minimum apogee altitude limit for the insertion orbit; reference from the landing site radius
	double h_amin = 30.0*1852.0;
};

struct PDAPResults
{
	//Apollo 11 abort coefficients
	double ABTCOF1;
	double ABTCOF2;
	double ABTCOF3;
	double ABTCOF4;
	//Term in LM desired semi-major axis
	double DEDA224;
	//Lower limit on semi-major axis
	double DEDA225;
	//Upper limit on semi-major axis
	double DEDA226;
	//Factor in LM desired semi-major axis
	double DEDA227;
	//Limiting phase angle
	double Theta_LIM;
	//Minimum apolune radius permitted in the insertion orbit
	double R_amin;
	double K1, K2, J1, J2;
};

struct DockAlignOpt	//Docking Alignment Processor
{
	//Option 1: LM REFSMMAT from CSM REFSMMAT, CSM attitude, docking angle and LM gimbal angles
	//Option 2: LM gimbal angles from CSM REFSMMAT, LM REFSMMAT, CSM gimbal angles and docking angle
	//Option 3: CSM gimbal angles from CSM REFSMMAT, LM REFSMMAT, LM gimbal angles and docking angle
	int type;
	MATRIX3 CSM_REFSMMAT;
	MATRIX3 LM_REFSMMAT;
	VECTOR3 CSMAngles;
	VECTOR3 LMAngles;
	double DockingAngle = 0.0;
};

struct FIDOOrbitDigitals
{
	FIDOOrbitDigitals();
	double GET;		//Ground elapsed time associated with present position data
	char VEHID[64];	//Vehicle name
	int REV;		//Current revolution number associated with subject vehicle and central body
	char REF[64];	//Reference planet
	char STAID[16]; //Last vector used for updating the ephemeris
	double GMTID;	//GMT of the state vector
	double GETID;	//GET of the state vector
	int NV1;		//Number of vectors used for interpolation for present position values
	double H;		//Current height
	double V;		//Current inertial velocity
	double GAM;		//Current inertial flight path angle
	double A;		//Semimajor axis of orbital ellipse
	double E;		//Eccentricity of orbital ellipse
	double I;		//Orbital inclination to Earth or linar equator
	double HA;		//Height of next apogee at GETA
	double PA;		//Latitude of next apogee at GETA
	double LA;		//Longitude of next apogee at GETA
	double GETA;	//Time of arrival at next apogee
	double HP;		//Height of next apogee at GETP
	double PP;		//Latitude of next apogee at GETP
	double LP;		//Longitude of next apogee at GETP
	double GETP;	//Time of arrival at next apogee
	double LPP;		//Present position, longitude
	double PPP;		//Present position, latitude
	double GETCC;	//GET of arrival at next rev crossing
	double TAPP;	//Present position, true anomaly
	double LNPP;	//Longitude of ascending node (Earth-fixed or moon-fixed)
	double GETL;	//Time spacecraft will pass over L
	int REVL;		//Revolution associated with GETL
	double L;		//The longitude associated with GETL
	double TO;		//Orbital period
	double K;		//K-Factor
	double ORBWT;	//Total current weight
	int REVR;		//Revolution of requested apogee/perigee
	char REFR[64];	//Reference planet of requested vector
	double GETBV;	//Time tag of vector from which apogee/perigee values were computed
	int NV2;		//Number of vectors used in interpolating for base vector for predicted apogee/perigee data
	double HAR;		//Height of next apogee at GETA, as requested
	double PAR;		//Latitude of next apogee at GETA, as requested
	double LAR;		//Longitude of next apogee at GETA, as requested
	double GETAR;	//Time of arrival at next apogee, as requested
	double HPR;		//Height of next apogee at GETP, as requested
	double PPR;		//Latitude of next apogee at GETP, as requested
	double LPR;		//Longitude of next apogee at GETP, as requested
	double GETPR;	//Time of arrival at next apogee, as requested
};

struct FIDOOrbitDigitalsOpt
{
	SV sv_A;
	double GETbase;
	double MJD;		//MJD to update the state vector to (only used in continuous update)
};

struct SpaceDigitals
{
	SpaceDigitals();
	int TUP;			//Update number associated with subject vehicle
	char VecID[16];		//Identification of the last vector used to update the ephemeris
	double WEIGHT;		//Total vehicle weight
	double GMTV;		//Greenwich time-tag of the vector
	double GETV;		//Ground elapsed time-tag of the vector
	double GETAxis;		//Ground elapsed time used to define the earth-moon line
	double GETR;		//Ground elapsed time reference (elapsed time of an event)
	double GET;			//Current ground elapsed time for which orb params were computed
	char REF[64];		//Inertial reference body used to compute orb params
	double V;			//Current velocity
	double PHI;			//Current latitude
	double H;			//Current altitude above spherical Earth or above moon assuming landing site radius
	double ADA;			//Current true anomaly
	double GAM;			//Current inertial flightpath angle
	double LAM;			//Current longitude
	double PSI;			//Current heading angle
	char VEHID[4];		//Vehicle for which the space digitals are computed
	double GETVector1;	//Ground elapsed time of the vector used to compute quantities below
	char REF1[64];		//Inertial reference body used to compute quantities for GET Vector 1
	double WT;			//Total vehicle weight at GET vector 1
	double GETA;		//Ground elapsed time of next apogee (referenced from GET Vector 1)
	double HA;			//Height of apogee (referenced from GET Vector 1)
	double HP;			//Height of perigee (referenced from GET Vector 1)
	double H1;			//Altitude above reference radius (referenced from GET Vector 1)
	double V1;			//Inertial velocity (referenced from GET Vector 1)
	double GAM1;		//Flightpath angle (referenced from GET Vector 1)
	double PSI1;		//Heading angle (referenced from GET Vector 1)
	double PHI1;		//Geodetic latitude (referenced from GET Vector 1)
	double LAM1;		//Longitude (referenced from GET Vector 1)
	double HS;			//Altitude above a spherical earth or moon (referenced from GET Vector 1)
	double HO;			//Altitude above a oblate earth or spherical moon (referenced from GET Vector 1)
	double PHIO;		//Geocentric latitude (referenced from GET Vector 1)
	double IEMP;		//Inclination of the trajectory with respect to the earth-moon plane
	double W1;			//Argument of periapsis (referenced from GET Vector 1)
	double OMG;			//Right ascension of the ascending node (inertial)
	double PRA;			//Inertial right ascension of perigee
	double A1;			//Semi-major axis
	double L1;			//Argument of latitude
	double E1;			//Eccentricity
	double I1;			//Inclination
	double GETVector2;
	double GETSI;		//GET of vehicle passing through the lunar SOI with negative lunar altitude rate
	double GETCA;		//GET of closest approach to the moon
	double VCA;			//Inertial velocity at the point of closest approach
	double HCA;			//Altitude at the point of closest approach above landing site radius
	double PCA;			//Latitude of closest approach
	double LCA;			//Longitude of closest approach
	double PSICA;		//Heading angle of closest approach
	double GETMN;		//GET of arrival at the node
	double HMN;			//Height of arrival at the node
	double PMN;			//Latitude of arrival at the node
	double LMN;			//Longitude of arrival at the node
	double DMN;			//Wedge angle between planes of approach hyperbola and lunar parking orbit
	double GETVector3;
	double GETSE;		//GET of vehicle passing through the lunar SOI with positive lunar altitude rate
	double GETEI;		//GET of entry interface
	double VEI;			//Earth centered inertial velocity at entry interface
	double GEI;			//Inertial flightpath angle at entry interface
	double PEI;			//Latitude at entry interface
	double LEI;			//Longitude at entry interface
	double PSIEI;		//Heading angle at entry interface
	double GETVP;		//GET of arrival at vacuum perigee
	double VVP;			//Earth centered velocity at vacuum perigee
	double HVP;			//Altitude at vacuum perigee
	double PVP;			//Latitude at vacuum perigee
	double LVP;			//Longitude at vacuum perigee
	double PSIVP;		//Heading angle at vacuum perigee
	double IE;			//Inclination angle at vacuum perigee
	double LN;			//Geographic longitude of the earth return ascending node
	int TUN1, TUN2, TUN3;
	std::string errormessage;
};

struct SpaceDigitalsOpt
{
	SV sv_A;
	double GETbase;
	double LSLat;
	double LSLng;
	double LSAzi;
	double t_land;
};

struct CheckoutMonitor
{
	CheckoutMonitor();

	char VEH[4];	//Vehicle associated with parameters
	double GET;		//Ground elapsed time tag of vector
	double GMT;		//Greenwich mean time tag of the vector
	char VID[64];	//Identification of the last vector used to update the ephemeris
	VECTOR3 Pos;	//Position vector
	VECTOR3 Vel;	//Velocity vector
	double V_i;		//Inertial velocity
	double gamma_i;	//Inertial flight-path angle
	double psi;		//Inertial azimuth
	double phi_c;	//Geocentric/selenographic latitude
	double lambda;	//Geographic/selenographic longitude
	double h_s;		//Height of vehicle above a spherical Earth or moon
	double h_a;		//Height of apogee above a spherical Earth or moon
	double h_p;		//Height of perigee above a spherical Earth or moon
	double h_o_NM;	//Height above an oblate Earth (NM)
	double h_o_ft;	//Height above an oblate Earth (feet)
	double phi_D;	//Geodetic/selenographic latitude
	double lambda_D;//Mean geographic/true inertial longitude
	double a;		//Semi-major axis of orbital ellipse
	double e;		//Eccentricity of orbital ellipse
	double i;		//Orbital inclination
	double omega_p;	//Argument of perigee
	double Omega;	//Right ascension of the ascending node (inertial)
	double nu;		//True anomaly
	double m;		//Mean anomaly
	double R;		//Vector magnitude radius
	int R_Day[3];	//Calendar day of liftoff (day:month:year)
	double A;		//Vehicle area at GET
	double K_Fac;	//Current atmospheric density multiplier
	char CFG[4];	//Vehicle configuration at GET
	char RF[4];		//Reference frame (ECT, ECI, MCT, MCI)
	double WT;		//Total weight for above configuration
	double WC;		//Total CSM weight
	double WL;		//Total LM weight
	double SPS;		//Total SPS propellant remaining
	double RCS_C;	//Total CSM RCS propellant remaining
	double APS;		//Total APS propellant remaining
	double DPS;		//Total DPS propellant remaining
	double RCS_L;	//Total LM RCS propellant remaining
	double J2;		//Total J2 propellant remaining
	double LOC;		//Lift-off time of the CSM
	double GRRC;	//Guidance reference release time of the spacecraft
	double ZSC;		//Time that S/C register was zeroed
	double GRRS;	//Guidance reference release time of the S-IVB
	double LAL;		//Landing time of the LM (Is this really on the checkout monitor?)
	double LOL;		//Lift-off time of the LM
	double ZSL;		//Time that the LGC register was zeroed
	double ZSA;		//Time that the AGS register was zeroed
	double EPHB;	//The begin time of the ephemeris
	double EPHE;	//The end time of the ephemeris
	int UpdateNo;	//To indicate the number of times that the ephemeris was updated
	unsigned NV;	//Number of vectors used to interpolate for vector displayed
	double THT;		//MED threshold time
	double RTT;		//Reentry threshold time
	double LSB;		//Lunar stay begin time
	double LLS;		//Lunar stay termination
	double deltaL;	//Lunar declination
	double VSF;		//Vent scaling factor
	double EB2;		//Ephemeris begin time (other vehicle)
	double EE2;		//Ephemeris end time (other vehicle)
	VECTOR3 U_T;	//Unit thrust vector associated with the requested maneuver
	char Option[4];	//MED option to which displayed data corresponds (GET, GMT, MVI, MVE, RAD, ALT, FPA)

	int unit; //0 = ER, 1 = FT
	bool TABlank; //Is true anomaly blanked
	bool LSTBlank; //Lunar stay time blank
	bool MABlank; //Is mean anomaly blanked
	bool HOBlank; //Is oblate height blank

	std::string ErrorMessage;
};

struct DetailedManeuverTable
{
	DetailedManeuverTable();
	char C_STA_ID[10];
	double C_GMTV;
	double C_GETV;
	char CODE[10];
	char L_STA_ID[10];
	double L_GMTV;
	double L_GETV;
	char REF[10];
	char X_STA_ID[10];
	double X_GMTV;
	double X_GETV;
	double GETR;
	double WT;
	double WC;
	double WL;
	double WF;
	double GETI;
	double PETI;
	double DVM;
	double DVREM;
	double DVC;
	double DT_B;
	double DT_U;
	double DT_TO;
	double DV_TO;
	char REFSMMAT_Code[7];
	double DEL_P;
	double DEL_Y;
	VECTOR3 VG;
	VECTOR3 IMUAtt;
	VECTOR3 FDAIAtt;
	VECTOR3 LVLHAtt;
	double VF;
	double VS;
	double VD;
	double H_BI;
	double P_BI;
	double L_BI;
	double F_BI;
	double HA;
	double HP;
	double L_AN;
	double E;
	double I;
	double WP;
	double VP;
	double THETA_P;
	double DELTA_P;
	double P_LLS;
	double L_LLS;
	double R_LLS;
	double DH;
	double PHASE;
	double PHASE_DOT;
	double WEDGE_ANG;
	double YD;
	bool UntilDay; //false = until night, true = until day
	double TimeUntil;
	char PGNS_Veh[8];
	char AGS_Veh[8];
	double PGNS_GETI;
	VECTOR3 PGNS_DV;
	double AGS_GETI;
	VECTOR3 AGS_DV;
	double Lam_GETI;
	double Lam_TF;
	VECTOR3 Lam_R;
	int CFP_ID;
	double CFP_GETI;
	int CFP_APSIS;
	double CFP_ELEV;
	double CFP_TPI;
	double CFP_DT;
	char CFP_OPTION[8];
	bool isCSMTV;
	std::string error;
};

struct MPTVehicleDataBlock
{
	//Word 12 (Bytes 1, 2)
	std::bitset<4> ConfigCode;
	//Word 12 (Bytes 3, 4)
	int ConfigChangeInd = 0;
	//Word 12 (Bytes 5, 6)
	int TUP = 0;
	//Word 13
	double CSMArea = 0.0;
	//Word 14
	double SIVBArea = 0.0;
	//Word 15
	double LMAscentArea = 0.0;
	//Word 16
	double LMDescentArea = 0.0;
	//Word 17
	double CSMMass = 0.0;
	//Word 18
	double SIVBMass = 0.0;
	//Word 19
	double LMAscentMass = 0.0;
	//Word 20
	double LMDescentMass = 0.0;
	//Word 21
	double CSMRCSFuelRemaining = 0.0;
	//Word 22
	double SPSFuelRemaining = 0.0;
	//Word 23
	double SIVBFuelRemaining = 0.0;
	//Word 24
	double LMRCSFuelRemaining = 0.0;
	//Word 25
	double LMAPSFuelRemaining = 0.0;
	//Word 26
	double LMDPSFuelRemaining = 0.0;
};

struct MPTManeuver
{
	MPTManeuver();

	//Word 1
	std::string code;
	//Words 12-26
	MPTVehicleDataBlock CommonBlock;
	//Word 27
	std::string StationIDFrozen;
	//Word 28, GMT of frozen anchor vector
	double GMTFrozen;
	//Word 31 (Bytes 1, 2)
	int AttitudeCode;
	//Word 31 (Bytes 3, 4)
	int Thruster;
	//Word 31 (Bytes 5, 6), false = 2 thrusters, true = 4 thrusters
	bool UllageThrusterOpt;
	//Word 31 (Bytes 7,8)
	bool AttitudesInput;
	//Word 32 (Bytes 1, 2)
	std::bitset<4> ConfigCodeBefore;
	//Word 32 (Bytes 3, 4), Maneuvering vehicle: 1 = CSM, 2 = S-IVB, 3 = LM
	int TVC;
	//Word 32 (Bytes 5,6)
	int TrimAngleInd;
	//Word 32 (Bytes 7, 8)
	bool FrozenManeuverInd;
	//Word 33 (Bytes 1,2), 0 = Earth, 1 = Moon
	int RefBodyInd;
	//Word 33 (Bytes 3,4), 0 = ECI, 1 = ECT, 2 = MCI, 3 = MCT
	int CoordSysInd;
	//Word 33 (Bytes 5,6), false = down, true = up
	bool HeadsUpDownInd;
	//Word 34
	double DockingAngle;
	//Word 35, GMT time to begin maneuver (first phase, aka ullage)
	double GMTMAN;
	//Word 36, total time of ullage including overlap
	double dt_ullage;
	//Word 37, time at 10% thrust for DPS (from engine on signal to throttle up)
	double DT_10PCT;
	//Word 38, duration of burn, for inertial and manual
	double dt;
	//Word 39, Delta V of maneuver, for inertial and manual
	double dv;
	//Word 40-42
	VECTOR3 A_T; //Unit thrust vector
	//Word 43-51
	VECTOR3 X_B, Y_B, Z_B; //Body direction matrix
	//Words 52-58
	EphemerisData FrozenManeuverVector;
	//Word 59, DPS scale factor (0 to 1)
	double DPSScaleFactor;
	//Word 61-63
	//For ascent maneuver this is: R_D, Y_D
	VECTOR3 dV_inertial; //In RTCC coordinates
	//Word 64-66, In P30 coordinates
	//For ascent maneuver this is: R_D_dot, Y_D_dot, Z_D_dot
	VECTOR3 dV_LVLH;
	union
	{
		//ExtDVCoordInd and 
		int Word67i[2];
		//R_N or P for TLI
		double Word67d;
	};
	//K5 for TLI
	double Word68;
	//T_M or R_T for TLI
	double Word69;
	double Word70;
	double Word71;
	double Word72;
	//Words 73-84
	//For ascent maneuver this is: R, V, MJD of CSM
	//For TLI this is: P_RP, Y_RP, , T3, tau3, TLI ind,T2,Vex2,Mdot2,DV_BR,tau2N,dtP
	double Word73;
	double Word74;
	double Word75;
	double Word76;
	double Word77;
	union
	{
		double Word78d;
		//For TLI this is current and original TLI computation indicator
		int Word78i[2];
	};
	double Word79;
	double Word80;
	double Word81;
	double Word82;
	double Word83;
	double Word84;
	//Word 106
	double GMTI;
	//Word 110 (Byte 1)
	//X'FF'???
	//Word 148-150
	int TrajDet[3];

	VECTOR3 R_BI;
	VECTOR3 V_BI;
	double GMT_BI;
	VECTOR3 R_BO;
	VECTOR3 V_BO;
	double GMT_BO;
	VECTOR3 R_1;
	VECTOR3 V_1;
	double GMT_1;
	
	double TotalMassAfter;
	double TotalAreaAfter;
	double MainEngineFuelUsed;
	double RCSFuelUsed;
	double DVREM;
	double DVC;
	double DVXBT;
	double DV_M;
	double V_F, V_S, V_D;
	double P_H, Y_H, R_H;
	//Burn duration, GMTCO minus GMTI
	double dt_BD;
	//DT of tailoff
	double dt_TO;
	//DV of tailoff
	double dv_TO;
	//Pitch gimbal at ignition
	double P_G;
	//Yaw or roll gimbal at ignition
	double Y_G;
	//Latitude at burn initiation
	double lat_BI;
	//Longitude at burn initiation
	double lng_BI;
	//Height at burn initiation
	double h_BI;
	//True anomaly at burn initiation
	double eta_BI;
	//Eccentricity of orbit after this maneuver
	double e_BO;
	//Inclination of orbit after this maneuver
	double i_BO;
	//Argument of periapsis after this maneuver
	double g_BO;
	//Altitude at first apogee after the maneuver
	double h_a;
	double lat_a;
	double lng_a;
	double GMT_a;
	double h_p;
	double lat_p;
	double lng_p;
	double GMT_p;
	double GMT_AN;
	double lng_AN;
	//Impulsive maneuver time
	double IMPT;
};

struct MPTManDisplay
{
	MPTManDisplay();
	std::string GETBI;
	std::string DT;
	double DELTAV;
	double DVREM;
	double HA;
	double HP;
	std::string code;
};

struct MissionPlanTable
{
	//Word 1 (Byte 3,4)
	//Number of maneuvers in table
	unsigned ManeuverNum = 0;
	//Word 1 (Byte 5,6)
	//Not needed
	//Word 1 (Byte 7,8)
	unsigned MaxManeuverNum = 15;
	//Word 2
	std::string StationID;
	//Word 3
	//Anchor vector time
	double GMTAV;
	//Word 4
	double KFactor = 0.0;
	//Word 8
	double LMStagingGMT = 0.0;
	//Word 9
	double UpcomingManeuverGMT = 1e70;
	//Word 11
	double SIVBVentingBeginGET = 0.0;
	//Words 12-26
	MPTVehicleDataBlock CommonBlock;
	double TotalInitMass = 0.0;
	double ConfigurationArea = 0.0;
	//Word 34
	double DeltaDockingAngle = 0.0;

	double TimeToBeginManeuver[15];
	double TimeToEndManeuver[15];
	double AreaAfterManeuver[15];
	double WeightAfterManeuver[15];
	unsigned LastFrozenManeuver = 0;
	unsigned LastExecutedManeuver = 0;

	std::deque<MPTManeuver> mantable;
};

struct BurnParameterTable
{
	//P1: DV, DV IND, DT
	double P1_DV = 0.0; //Vel increment or burn duration of the maneuver
	//0 = MAG, Magnitude of maneuver
	//1 = DVC, DV along X-body axis includes ullage, excludes tailoff
	//2 = XBT, Includes both ullage and tailoff
	int P1_DVIND = 0;
	double P1_DT = 0.0; //Time duration of maneuver, excludes ullage and tailoff

	//P2: VX, VY, VZ in command and display system
	VECTOR3 P2_DV = _V(0, 0, 0);

	//P3: VGX, VGY, VGZ DV Vector in IMU coordinates
	VECTOR3 P3_DV = _V(0, 0, 0);

	//P4: VF, VS, VD LVLH DV vector
	VECTOR3 P4_DV = _V(0, 0, 0);

	//P5: X, Y, Z, T Flight, C
	VECTOR3 P5_RT;
	double P5_TFLT; //Time from ignition to arrival at target vector
	double P5_C;	//Cross product steering constant

	//P6: RD, YD, RD DOT, YD DOT, ZD DOT, DT ascent man. targets
	double P6_RD;	//Desired injection radius
	double P6_YD;	//Desired injection cross range distance from the CSM orbital plane
	double P6_RD_DOT;	//Desired injection radial velocity
	double P6_YD_DOT;	//Desired injection crossrange velocity
	double P6_ZD_DOT;	//Desired injection downrange velocity
	double P6_DT;		//Time from descent ignition to descent abort

	//P7: DVX, DVY, DVZ maneuver residuals in RCS control axes
	VECTOR3 P7_DDV;
};

struct PMMXFRDirectInput
{
	int TableCode;
	unsigned ReplaceCode;
	double GMTI;
	int ThrusterCode;
	int AttitudeCode;
	int BurnParameterNumber;
	int CoordinateIndicator;
	double Pitch;
	double Yaw;
	double Roll;
	double dt_ullage;
	bool UllageThrusterOption;
	bool HeadsUpDownIndicator;
	double DT10P;
	int REFSMMATIndicator;
	int ConfigurationChangeIndicator;
	std::bitset<4> EndConfiguration;
	double DockingAngle;
	double DPSScaleFactor;
	int TrimAngleIndicator;
};

struct PMMXFR_Impulsive_Input
{
	int Table;
	int Plan;
	double DeleteGMT;
	int Thruster[4];
	int Attitude[4];
	double dt_ullage[4];
	bool UllageThrusterOption[4];
	bool IterationFlag[4];
	double DT10P[4];
	double DPSScaleFactor[4];
	bool TimeFlag[4];
	unsigned ReplaceCode = 0;
	int Type;
};

struct PMMMCDInput
{
	double GMTI;
	int Thruster;
	int Attitude;
	double dt_ullage;
	int ConfigChangeInd;
	int ConfigCodeAfter;
	int TVC;
	double DockingAngle;
	std::bitset<4> CCMI;
	int BPIND;
	int ID;
	bool UllageThrusterOpt;
	double DT_10PCT;
	double DPSScaleFactor;
	bool HeadsUpDownInd;
	int CoordinateInd;
	double StartTimeLimit;
	double EndTimeLimit;
	MissionPlanTable *mpt;
	MPTManeuver *prevman;
	unsigned CurrentManeuver;
	double Pitch, Yaw, Roll;
	EphemerisData sv_anchor;
	EphemerisData sv_other;
	bool FrozenManeuver;
	double BurnParm72;
	int BurnParm73;
	double BurnParm74;
	double BurnParm75;
	double BurnParm76;
	double BurnParm77;
	double BurnParm78;
	double BurnParm79;
	double WTMI;
	int RefMatCode = 1;
};

struct PMMMPTInput
{
	EphemerisData sv_before;
	VECTOR3 V_aft;
	int Thruster;
	int Attitude;
	double DockingAngle;
	double DETU;
	bool UT;
	int CONFIG;
	int VC;
	double DT_10PCT;
	double DPSScaleFactor;
	double VehicleWeight;
	double CSMWeight;
	double LMWeight;
	bool IgnitionTimeOption;
	bool IterationFlag;
	double LowerTimeLimit;
	double UpperTimeLimit;
	MissionPlanTable *mpt;
	unsigned CurrentManeuver;
	EphemerisData sv_other;
};

struct PMMSPTInput
{
	//Word 1
	int QUEID;
	//Word 2
	double GMT;
	//Word 3-5
	VECTOR3 R;
	//Word 6-8
	VECTOR3 V;
	//Word 10
	int Table;
	//Word 11
	unsigned ReplaceCode;
	int InjOpp;
	//Word 12
	double T_RP = -1;
	//Word 13
	int ThrusterCode;
	int AttitudeMode;
	//Word 14-19
	//Targeting Parameters
	//Word 29
	int CCI;
	//Word 30
	int CC;
	int TVC;
	//Word 32
	int CCMI;
	MPTManeuver *CurMan;
	MPTManeuver *PrevMan = NULL;
	MissionPlanTable *mpt;
	//Word 46
	double PresentGMT;
	//Word 47
	double StartTimeLimit;
	//Word 48
	double EndTimeLimit;
};

struct PMMLDPInput
{
	double TLAND;
	SV sv;
	MissionPlanTable *mpt;
	unsigned CurrentManeuver;
	int TrimAngleInd;
	int HeadsUpDownInd;
};

struct PMMLDIInput
{
	double TLAND = 0.0;
	SV sv;
	double W_LMA;
	double W_LMD;
};

struct PMMLAIInput
{
	double m0;
	SV sv_CSM;
	double t_liftoff;
	double v_LH;
	double v_LV;
};

struct PCMATCArray
{
	//Words 31-44: State and time of preabort
	VECTOR3 R0;
	VECTOR3 V0;
	double T0;
	//Word 45: Solution type indicator (0 = AST, 1 = RTED)
	int IASD;
	//Word 46: Initial reference
	int REF;
	//Word 48: Stopping type (1 = gamma, -1 = time)
	int ISTP;
	//Words 49-50: Stopping gamma (for stopping type = 1)
	double gamma_stop;
	//Words 51-64: State and time of reentry
	VECTOR3 R_r;
	VECTOR3 V_r;
	double T_r;
	//Words 65-70: Unit thrust at MEI (RTED), output abort position vector (AST)
	VECTOR3 Vec1;
	//Words 71-76: Position at MEI (RTED), output abort velocity vector (AST)
	VECTOR3 Vec2;
	//Words 83-84: CSM weight at abort
	double CSMWeight;
	//Words 85-86: LM weight at abort
	double LMWeight;
	//Words 87-88: Docking angle
	double DockingAngle;
	//Words 89-90: DT of ullage
	double dt_ullage;
	//Words 91-92: DT of 10% thrust (both input and output)
	double DT_10PCT;
	//Words 93-94: DT of tailoff
	double DT_TO;
	//Words 95-112: XB, YB, ZB vehicle body axies
	VECTOR3 X_B, Y_B, Z_B;
	//Words 113-116: Ry, Rz trim angles at MEI
	double R_Y, R_Z;
	//Words 117-118: Velocity counter
	double DVC;
	//Word 130: Attitude control (3 Lambert, 4 Ext DV)
	int AttitudeCode;
	//Word 131: Thruster code
	int ThrusterCode;
	//Word 132: Ullage code
	bool UllageCode;
	//Word 133: Configuration code (as defined for MPT)
	int ConfigCode;
	//Word 134: Maneuvering vehicle (1 CSM, 3 LM)
	int TVC;
	//Word 135: Initial attitude (0-heads down, 1-up)
	bool HeadsUpDownInd;
	//Word 136: Trim angles (-1 compute, 1 system parameters)
	int TrimAngleInd;
	//Words 137-138: Time of midnight prior to launch
	double GMTBASE;
	bool h_pc_on;
};

struct REFSMMATData
{
	MATRIX3 REFSMMAT;
	int ID = 0;
	double GMT = 0.0;
};

struct REFSMMATLocker
{
	REFSMMATData data[12];
};

struct StationContact
{
	StationContact();
	double GMTAOS;
	double GMTLOS;
	double GMTEMAX;
	double MAXELEV;
	std::string StationID;
	bool BestAvailableAOS;
	bool BestAvailableLOS;
	bool BestAvailableEMAX;
	int REV;

	//For sorting
	bool operator<(const StationContact& rhs) const;
};

struct OrbitStationContactsTable
{
	StationContact Stations[45];
};

struct NextStationContactsTable
{
	NextStationContactsTable();
	//Station ID
	std::string STA[2][6];
	//GET of horizon ascent
	double GETHCA[2][6];
	//Duration from GETHCA to entrance into keyhole (GETKLOS - GETHCA)
	double DTKLOS[2][6];
	//Maximum elevation angle above station
	double EMAX[2][6];
	//Duration of contact
	double DTPASS[2][6];
	//Duration of time in keyhole
	double DTKH[2][6];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[2][6];
	//If true, maximum elevation occured before current GET
	bool BestAvailableEMAX[2][6];
	//Current GET
	double GET = 0.0;
};

struct PredictedSiteAcquisitionTable
{
	PredictedSiteAcquisitionTable();
	
	//Current page (1 to 2)
	int curpage;
	//Total number of pages (1 or 2)
	int pages;
	//Total number of contacts
	unsigned numcontacts[2];
	//Station ID of anchor vector
	std::string STAID;
	//Vehicle (CSM or LEM)
	std::string VEHICLE;
	//Revolution number
	int REV[2][21];
	//Sites
	std::string STA[2][21];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[2][21];
	//AOS GET
	double GETHCA[2][21];
	//If true, AOS occured before current GET
	bool BestAvailableLOS[2][21];
	//LOS GET
	double GETHCD[2][21];
	//If true, maximum elevation occured before current GET
	bool BestAvailableEMAX[2][21];
	//Max elevation
	double ELMAX[2][21];
};

struct ExperimentalSiteAcquisitionTable
{
	ExperimentalSiteAcquisitionTable();

	//Current page (1 to 2)
	int curpage;
	//Total number of pages (1 or 2)
	int pages;
	//Total number of contacts
	unsigned numcontacts[2];
	//Station ID of anchor vector
	std::string STAID;
	//Revolution number
	int REV[2][20];
	//Sites
	std::string STA[2][20];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[2][20];
	//AOS GET
	double GETAOS[2][20];
	//Ground range
	double GNDRNG[2][20];
	//Altitude
	double ALT[2][20];
	//If true, maximum elevation occured before current GET
	bool BestAvailableEMAX[2][20];
	//Max elevation
	double ELMAX[2][20];
	//Closest approach GET
	double GETCA[2][20];
	//LOS GET
	double GETLOS[2][20];
	//If true, AOS occured before current GET
	bool BestAvailableLOS[2][20];
};

struct LandmarkAcquisitionTable
{
	LandmarkAcquisitionTable();
	//Current page (1 to 3)
	int curpage;
	//Total number of pages (1 or 3)
	int pages;
	//Total number of contacts
	unsigned numcontacts[3];
	//Station ID of anchor vector
	std::string STAID;
	//Trajectory update number of subject vehicle ephemeris
	int TUN;
	//GET of acquisition of landmark site by vehicle
	double GETAOS[3][20];
	//If true, AOS occured before current GET
	bool BestAvailableAOS[3][20];
	//GET of loss of sight of the landmark site by vehicle
	double GETLOS[3][20];
	//If true, LOS occurs after end of ephemeris
	bool BestAvailableLOS[3][20];
	//GET of closest approach of vehicle to the landmark
	double GETCA[3][20];
	//GET of local sunrise
	double GETSR[3][20];
	//GET of local sunset
	double GETSS[3][20];
	//Viewing angle from the vehicle to the landmark site at GETCA
	double Lambda[3][20];
	//Spacecraft altitude at GETCA
	double h[3][20];
};

struct MANTIMESData
{
	MANTIMESData() { ManData[0] = 0.0;ManData[1] = 0.0; }
	double ManData[2];
};

struct ManeuverTimesTable
{
	int TUP = 0;
	std::vector<MANTIMESData> Table;
};

struct LunarStayTimesTable
{
	double LunarStayBeginGMT = -1;
	double LunarStayEndGMT = -1;
};

struct CapeCrossingTable
{
	CapeCrossingTable();
	int TUP;
	int NumRev;
	int NumRevFirst;
	int NumRevLast;
	double GMTEphemFirst;
	double GMTEphemLast;
	//Time of last known cape crossing before the time of the update vector (zero if unknown)
	double GMTCrossPrev;
	double GMTCross[30];
};

struct Station
{
	double lat = 0.0;
	double lng = 0.0;
	double alt = 0.0;
	double rad = 0.0;
	std::string code;
};

struct StationTable
{
	std::vector<Station> table;
};

struct LunarDescentPlanningTable
{
	LunarDescentPlanningTable();
	double LMWT;
	double GMTV;
	double GETV;
	int MODE;
	double LAT_LLS;
	double LONG_LLS;
	double GETTH[4];
	double GETIG[4];
	double LIG[4];
	double DV[4];
	double AC[4];	//Altitude at cutoff?
	double HPC[4];	//Perilune height at cutoff?
	double DEL[4];	//???
	double THPC[4];	//???
	VECTOR3 DVVector[4];
	std::string MVR[4];
	double PD_ThetaIgn;
	double PD_PropRem;
	double PD_GETTH;
	double PD_GETIG;
	double PD_GETTD;
	char DescAzMode[4];
	double DescAsc;
	double SN_LK_A; //???
};

struct SunriseSunsetData
{
	//Revolution number
	int REV = 0;
	//GET of terminator rise from liftoff
	double GETTR = 0.0;
	//GET of sunrise from liftoff
	double GETSR = 0.0;
	//Pitch at sunrise
	double theta_SR = 0.0;
	//Yaw at sunrise
	double psi_SR = 0.0;
	//GET of terminator set from liftoff
	double GETTS = 0.0;
	//GET of sunset from liftoff
	double GETSS = 0.0;
	//Pitch at sunset
	double theta_SS = 0.0;
	//Yaw at sunset
	double psi_SS = 0.0;
};

struct SunriseSunsetTable
{
	int num = 0;
	SunriseSunsetData data[8];
	std::string errormessage;
};

// Parameter block for Calculation(). Expand as needed.
struct calculationParameters {
	Saturn *src;		// Our ship
	VESSEL *tgt;		// Target ship
	double TLI;			// Time of TLI
	VECTOR3 R_TLI;		//TLI cutoff position vector
	VECTOR3 V_TLI;		//TLI cutoff velocity vector
	double LOI;			// Time of LOI/Pericynthion
	double SEP;			// Time of separation
	double DOI;			// Time of DOI
	double PDI;			// Time of PDI
	double TLAND;		// Time of landing
	double LunarLiftoff;// Time of lunar liftoff
	double LSAlt;		// Height of the lunar landing site relative to mean lunar radius
	double LSAzi;		// Approach azimuth to the lunar landing site
	double LSLat;		// Latitude of the lunar landing site
	double LSLng;		// Longitude of the lunar landing site
	double Insertion;	// Time of Insertion
	double Phasing;		// Time of Phasing
	double CSI;			// Time of CSI
	double CDH;			// Time of CDH
	double TPI;			// Time of TPI
	double TEI;			// Time of TEI
	double EI;			// Time of Entry Interface
	double lat_node;
	double lng_node;
	double alt_node;
	double GET_node;
	MATRIX3 StoredREFSMMAT;
	double TEPHEM;	// MJD of CMC liftoff time
	double PericynthionLatitude;	//Latitude of pericynthion in Earth-Moon Plane coordinates
	double TIGSTORE1;		//Temporary TIG storage
	VECTOR3 DVSTORE1;		//Temporary DV storage
	SV SVSTORE1;			//Temporary state vector storage
};

//For LVDC
struct SevenParameterUpdate
{
	double T_RP;	//Time of Restart Preparation (TB6)
	double C3;
	double Inclination;
	double e;
	double alpha_D;
	double f;
	double theta_N;
};

struct LVDCTLIparam
{
	double alpha_TS;
	double Azimuth;
	double beta;
	double C_3;
	double cos_sigma;
	double DEC;
	double e_N;
	double f;
	double mu;
	double omega_E;
	double phi_L;
	double RA;
	double R_N;
	double T_2R;
	double theta_EO;
	//Time of ignition of first S-IVB burn
	double T4IG;
	//Time of cutoff of first S-IVB burn
	double T4C;
	double T_L;
	double T_LO;
	double t_D;
	double T_RG;
	double T_ST;
	double Tt_3R;
	double t_clock;
};

struct GPMPRESULTS
{
	double GET_A;
	double HA;
	double long_A;	//longitude of apoapsis
	double lat_A;	//latitude of apoapsis
	double GET_P;
	double HP;
	double long_P;	//longitude of periapsis
	double lat_P;	//latitude of periapsis
	double A;
	double E;
	double I;
	double Node_Ang;
	double Del_G;
	double Pitch_Man;
	double Yaw_Man;
	double H_Man;
	double long_Man;
	double lat_Man;
};

struct ASTInput
{
	double dgamma;
	double dpsi;
	double dv;
	double T_a;
	double T_r;
	double h_r_des;
	double lat_r_des;
	double lng_r_des;
	double azi_r_des;
	double dt_ar;
	double gamma_des;
	double gamma_stop;
	double R_E_equ;
	double R_E_pole;
	double RRBIAS;
	double w_E;
	double Area;
	double Weight;
	double lat_PLA;
	double lng_PLA;
	double GLevel;
	double BankAngle;
	double GLevelGuidInit;
	int RollDir;
	double GMTBASE;
};

struct ASTSettings
{
	//Mode indicator to RMMYNI (1 = zero lift, 2 = max lift, 3 = G&N, 6 = Constant G, then roll, 10 = Constant G) 
	int ReentryMode;
	int Ref;
	//Type of reentry stop (1 = gamma, -1 = T)
	int ReentryStop;
	//Target line indicator (1 = Primary, 2 = Secondary)
	int TargetLine;
	//Entry profile (1 = G&N, <= 0 Constant G)
	int EntryProfile;
	//Miss distance indicator (0 = do not compute, 1 = compute)
	int MissDistanceInd;
};

struct ASTData
{
	int ASTCode;
	std::string StationID;
	//Time of abort (hr.)
	double AbortGMT;
	//Inclination of reentry (rad.)
	double incl_EI;
	//DV abort (Er/hr.)
	double dv;
	//Height of pericynthion (Er)
	double h_PC;
	//Reentry velocity magnitude (Er/hr.)
	double v_EI;
	//Gamma at reentry (rad.)
	double gamma_EI;
	//Time of reentry (GMT)
	double ReentryGMT;
	//Geodetic latitude of impact (rad.)
	double lat_SPL;
	//Longitude of impact (rad.)
	double lng_SPL;
	//Time of impact (hr.)
	double SplashdownGMT;
	//Time of vector in MPT (hr.)
	double VectorGMT;
	//Abort mode
	int AbortMode;
	//Primary reentry mode
	int ReentryMode;
	//Miss distance (Er)
	double MissDistance;
	//Site ID
	std::string SiteID;
	//G-level sensed (Er/hr.^2)
	double GLevel;
	//Reentry range bias (Er)
	double RRBIAS;
	//State and time at reentry (Er, Er/hr.)
	VECTOR3 R_EI;
	VECTOR3 V_EI;
	double MJD_EI;
	//Year of input
	int Year;
	//Geodetic latitude of target
	double lat_TGT;
	//Longitude of target
	double lng_TGT;
	//Preabort state and time (Er, Er/hr)
	VECTOR3 R0;
	VECTOR3 V0;
	double T0;
};

struct EMSMISSAuxOutputTable
{
	EphemerisData sv_cutoff;
	int ErrorCode;
	//0 = free-flight, 1 = end of maneuver
	int TerminationCode;
	//Maneuver number of last processed maneuver
	unsigned ManeuverNumber;
	double LunarStayBeginGMT;
	double LunarStayEndGMT;
};

struct EMSMISSInputTable
{
	EphemerisData AnchorVector;
	bool landed = false;
	//Desired value of stopping parameter relative to the Earth
	double EarthRelStopParam = 0.0;
	//Desired value of stopping parameter relative to the Moon
	double MoonRelStopParam = 0.0;
	//Maximum time of integration
	double MaxIntegTime = 10e70;
	//Storage interval for maneuver ephemeris
	double ManEphemDT = 10.0;
	//Storage interval for lunar surface ephemeris
	double LunarEphemDT = 3.0*60.0;
	//Density multiplier value
	double DensityMultiplier = 1.0;
	//Left limit of ephemeris (time to begin ephemeris)
	double EphemerisLeftLimitGMT;
	//Right limit of ephemeris (time to end ephemeris)
	double EphemerRightLimitGMT;
	//Minimum time between ephemeris points
	double MinEphemDT;
	//Reference frame of desired stopping parameter (0 = Earth, 1 = Moon, 2 = both)
	int StopParamRefFrame = 2;
	//Minimum number of points desired in ephemeris
	unsigned MinNumEphemPoints;
	bool ECIEphemerisIndicator = true;
	bool ECTEphemerisIndicator = false;
	bool MCIEphemerisIndicator = false;
	bool MCTEphemerisIndicator = false;
	//Ephemeris build indicator
	bool EphemerisBuildIndicator = false;
	//Maneuver cut-off indicator (0 = cut at begin of maneuver, 1 = cut at end of maneuver, 2 = don't cut off)
	int ManCutoffIndicator;
	//Descent burn indicator
	bool DescentBurnIndicator;
	//Cut-off indicator (0 = none, 1 = radial distance, 2 = first reference switch, 3 = altitude above Earth or moon, 4 = flight-path angle)
	int CutoffIndicator = 0;
	//Integration direction indicator (true = forward, false = backward)
	bool IsForwardIntegration = true;
	//Coordinate system indicator (TBD)
	//Maneuver indicator (true = consider maneuvers, false = don't consider maneuvers)
	bool ManeuverIndicator;
	//Vehicle code (1 = LEM, 3 = CSM)
	int VehicleCode;
	//Density multiplication override indicator
	bool DensityMultOverrideIndicator = false;
	//Table of ephemeris addresses indicator
	EphemerisDataTable *EphemTableIndicator = NULL;
	//Reference switch table indicator
	//Maneuver times table indicator
	ManeuverTimesTable *ManTimesIndicator = NULL;
	//Runge-Kutta auxiliary output table indicator
	RTCCNIAuxOutputTable *AuxTableIndicator = NULL;
	//Runge-Kutta dense ephemeris table indicator
	//Update in process override indicator (true = override, false = don't override)
	bool UIPOverrideIndicator = false;
	//Maneuver number of last maneuver to be ignored
	unsigned IgnoreManueverNumber = 10000U;
	EMSMISSAuxOutputTable NIAuxOutputTable;
};

struct ELVCTRInputTable
{
	//Time to interpolate
	double GMT;
	//Order of interpolation
	unsigned ORER = 8;
	//Vehicle code (option 2 only)
	int L;
	//Ephemeris type (0 = Orbit, 8 = Reentry, option 2 only)
	int EphemerisType = 0;
};

struct ELVCTROutputTable
{
	EphemerisData SV;
	int ErrorCode;
	//Order of interpolation performed
	unsigned ORER;
	//Vector property indicator (0 = Free flight, -1 = Lunar Stay, 1 += Maneuver)
	int VPI;
	//Update number
	int TUP;
};

class RTCC {

	friend class MCC;

public:
	RTCC();
	~RTCC();
	void Init(MCC *ptr);
	bool Calculation(int mission, int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);

	void SetManeuverData(double TIG, VECTOR3 DV);
	void GetTLIParameters(VECTOR3 &RIgn_global, VECTOR3 &VIgn_global, VECTOR3 &dV_LVLH, double &IgnMJD);

	void AP7TPIPAD(const AP7TPIPADOpt &opt, AP7TPI &pad);
	void AP9LMTPIPAD(AP9LMTPIPADOpt *opt, AP9LMTPI &pad);
	void AP9LMCDHPAD(AP9LMCDHPADOpt *opt, AP9LMCDH &pad);
	void TLI_PAD(TLIPADOpt* opt, TLIPAD &pad);
	bool PDI_PAD(PDIPADOpt* opt, AP11PDIPAD &pad);
	void LunarAscentPAD(ASCPADOpt opt, AP11LMASCPAD &pad);
	void EarthOrbitEntry(const EarthEntryPADOpt &opt, AP7ENT &pad);
	void LunarEntryPAD(LunarEntryPADOpt *opt, AP11ENT &pad);
	//Conic Fit
	int PCZYCF(double R1, double R2, double PHIT, double DELT, double VXI2, double VYI2, double VXF1, double VYF1, double SQRMU, int NREVS, int body, double &a, double &e, double &f_T, double &t_PT);
	int PMMTIS(EphemerisData sv_A1, EphemerisData sv_P1, double dt, double DH, double theta, EphemerisData &sv_A1_apo, EphemerisData &sv_A2, EphemerisData &sv_A2_apo);
	int PMSTICN_ELEV(EphemerisData sv_A1, EphemerisData sv_P1, double phi_req, double mu, double &T_ELEV);
	void PMSTICN(const TwoImpulseOpt &opt, TwoImpulseResuls &res);
	//Two-Impulse Single Solution
	void PMMTISS();
	void LambertTargeting(LambertMan *lambert, TwoImpulseResuls &res);
	double TPISearch(SV sv_A, SV sv_P, double GETbase, double elev);
	double FindDH(MPTSV sv_A, MPTSV sv_P, double GETbase, double TIGguess, double DH);
	MATRIX3 REFSMMATCalc(REFSMMATOpt *opt);
	void EntryTargeting(EntryOpt *opt, EntryResults *res);//VECTOR3 &dV_LVLH, double &P30TIG, double &latitude, double &longitude, double &GET05G, double &RTGO, double &VIO, double &ReA, int &precision);
	void BlockDataProcessor(EarthEntryOpt *opt, EntryResults *res);
	void TranslunarMidcourseCorrectionProcessor(SV sv0, double CSMmass, double LMmass);
	void TranslunarMidcourseCorrectionTargetingNodal(MCCNodeMan &opt, TLMCCResults &res);
	bool TranslunarMidcourseCorrectionTargetingFreeReturn(MCCFRMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingNonFreeReturn(MCCNFRMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingFlyby(MCCFlybyMan *opt, TLMCCResults *res);
	bool TranslunarMidcourseCorrectionTargetingSPSLunarFlyby(MCCSPSLunarFlybyMan *opt, TLMCCResults *res, int &step);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node);
	void LOITargeting(LOIMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_node, SV &sv_pre, SV &sv_post);
	void DOITargeting(DOIMan *opt, VECTOR3 &DV, double &P30TIG);
	void DOITargeting(DOIMan *opt, VECTOR3 &dv, double &P30TIG, double &t_PDI, double &t_L, double &CR);
	int LunarDescentPlanningProcessor(SV sv, double GETbase, double lat, double lng, double rad, LunarDescentPlanningTable &table);
	void PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG);
	void PlaneChangeTargeting(PCMan *opt, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_pre, SV &sv_post);
	bool GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG);
	bool GeneralManeuverProcessor(GMPOpt *opt, VECTOR3 &dV_i, double &P30TIG, GPMPRESULTS &res);
	OBJHANDLE AGCGravityRef(VESSEL* vessel); // A sun referenced state vector wouldn't be much of a help for the AGC...
	void NavCheckPAD(SV sv, AP7NAV &pad, double GETbase, double GET = 0.0);
	void AGSStateVectorPAD(AGSSVOpt *opt, AP11AGSSVPAD &pad);
	void AP11LMManeuverPAD(AP11LMManPADOpt *opt, AP11LMMNV &pad);
	void AP11ManeuverPAD(AP11ManPADOpt *opt, AP11MNV &pad);
	void AP10CSIPAD(AP10CSIPADOpt *opt, AP10CSI &pad);
	void CSMDAPUpdate(VESSEL *v, AP10DAPDATA &pad);
	void LMDAPUpdate(VESSEL *v, AP10DAPDATA &pad, bool asc = false);
	void RTEMoonTargeting(RTEMoonOpt *opt, EntryResults *res);
	void LunarOrbitMapUpdate(SV sv0, double GETbase, AP10MAPUPDATE &pad, double pm = -150.0*RAD);
	void LandmarkTrackingPAD(LMARKTRKPADOpt *opt, AP11LMARKTRKPAD &pad);
	SevenParameterUpdate TLICutoffToLVDCParameters(VECTOR3 R_TLI, VECTOR3 V_TLI, double GETbase, double P30TIG, double TB5, double mu, double T_RG);
	void LVDCTLIPredict(LVDCTLIparam lvdc, double m0, SV sv_A, double GETbase, VECTOR3 &dV_LVLH, double &P30TIG, SV &sv_IG, SV &sv_TLI);
	int PMMSPT(PMMSPTInput &in);
	void PCMSP2(int J, double t_D, double &cos_sigma, double &C3, double &e_N, double &RA, double &DEC);
	void LMThrottleProgram(double F, double v_e, double mass, double dV_LVLH, double &F_average, double &ManPADBurnTime, double &bt_var, int &step);
	void FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, bool agc = true);
	void FiniteBurntimeCompensation(SV sv, double attachedMass, VECTOR3 DV, int engine, VECTOR3 &DV_imp, double &t_slip, SV &sv_tig, SV &sv_cut, bool agc = true);
	void EngineParametersTable(int enginetype, double &Thrust, double &Isp);
	VECTOR3 ConvertDVtoLVLH(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_imp);
	VECTOR3 ConvertDVtoInertial(SV sv0, double GETbase, double TIG_imp, VECTOR3 DV_LVLH_imp);
	void PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, SV &sv_pre, SV &sv_post, bool agc = true);
	void PoweredFlightProcessor(SV sv0, double GETbase, double GET_TIG_imp, int enginetype, double attachedMass, VECTOR3 DV, bool DVIsLVLH, double &GET_TIG, VECTOR3 &dV_LVLH, bool agc = true);
	double GetDockedVesselMass(VESSEL *vessel);
	SV StateVectorCalc(VESSEL *vessel, double SVMJD = 0.0);
	EphemerisData StateVectorCalcEphem(VESSEL *vessel, double SVGMT = 0.0);
	SV ExecuteManeuver(SV sv, double GETbase, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster);
	SV ExecuteManeuver(SV sv, double GETbase, double P30TIG, VECTOR3 dV_LVLH, double attachedMass, int Thruster, MATRIX3 &Q_Xx, VECTOR3 &V_G);
	void TLMCIntegratedXYZT(SV sv_mcc, double lat_node, double lng_node, double h_node, double MJD_node, VECTOR3 DV_guess, VECTOR3 &DV);
	VECTOR3 TLMCEmpiricalFirstGuess(double r, double dt);
	void IntegratedTLMC(SV sv_mcc, double lat, double h, double gamma, double MJD, VECTOR3 var_guess, VECTOR3 &DV, VECTOR3 &var_converged, SV &sv_node);
	void TLMCFirstGuessConic(SV sv_mcc, double lat, double h, double gamma, double MJD_P, VECTOR3 &DV, VECTOR3 &var_converged);
	void TLMCFirstGuess(SV sv_mcc, double lat_EMP, double h_peri, double MJD_P, VECTOR3 &DV, SV &sv_peri);
	bool TLIFlyby(SV sv_TLI, double lat_EMP, double h_peri, SV sv_peri_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry);
	bool TLMCFlyby(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry);
	bool TLMCFlybyConic(SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry);
	bool TLMCConicFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP);
	bool TLMCIntegratedFlybyToInclinationSubprocessor(SV sv_mcc, double h_peri, double inc_fr_des, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_reentry, double &lat_EMP);
	bool TLMCConic_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor);
	bool TLMC_BAP_FR_LPO(MCCFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, SV &sv_reentry, double &lat_EMPcor);
	bool TLMCConic_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor);
	bool TLMC_BAP_NFR_LPO(MCCNFRMan *opt, SV sv_mcc, double lat_EMP, double h_peri, double MJD_peri, VECTOR3 DV_guess, VECTOR3 &DV, SV &sv_peri, SV &sv_node, double &lat_EMPcor);
	void PMMLTR(AEGBlock sv_CSM, double T_LO, double V_H, double V_R, double h_BO, double t_PF, double P_FA, double Y_S, double r_LS, double lat_LS, double lng_LS, double &deltaw0, double &DR, double &deltaw, double &Yd, double &AZP);
	void LLWP_PERHAP(AEGHeader Header, AEGDataBlock sv, double &RAP, double &RPE);
	void LLWP_HMALIT(AEGHeader Header, AEGDataBlock *sv, AEGDataBlock *sv_temp, int M, int P, int I_CDH, double DH, double &dv_CSI, double &dv_CDH, double &t_CDH);
	void LunarLaunchWindowProcessor(const LunarLiftoffTimeOpt &opt);
	void LaunchTimePredictionProcessor(const LunarLiftoffTimeOpt &opt, LunarLiftoffResults &res);
	bool LunarLiftoffTimePredictionCFP(const LunarLiftoffTimeOpt &opt, VECTOR3 R_LS, SV sv_P, OBJHANDLE hMoon, double h_1, double theta_Ins, double t_L_guess, double t_TPI, LunarLiftoffResults &res);
	bool LunarLiftoffTimePredictionDT(const LLTPOpt &opt, LunarLaunchTargetingTable &res);
	void LunarAscentProcessor(VECTOR3 R_LS, double m0, SV sv_CSM, double GETbase, double t_liftoff, double v_LH, double v_LV, double &theta, double &dt_asc, double &dv, SV &sv_IG, SV &sv_Ins);
	bool PoweredDescentProcessor(VECTOR3 R_LS, double TLAND, SV sv, double GETbase, RTCCNIAuxOutputTable &aux, EphemerisDataTable *E, SV &sv_PDI, SV &sv_land, double &dv);
	void EntryUpdateCalc(SV sv0, double GETbase, double entryrange, bool highspeed, EntryResults *res);
	void PMMDKI(SPQOpt &opt, SPQResults &res);
	//Velocity maneuver performer
	void PCMVMR(VECTOR3 R_C, VECTOR3 V_C, VECTOR3 R_T, VECTOR3 V_T, double DELVX, double DELVY, double DELVZ, int I, VECTOR3 &V_C_apo, double &Pitch, double &Yaw);
	//Elevation angle search subroutine
	int PCTETR(SV sv_C, SV sv_T, double GETBase, double WT, double ESP, double &TESP, double &TR);
	//Apogee, perigee, and offset determination
	void PCPICK(SV sv_C, SV sv_T, double &DH, double &Phase, double &HA, double &HP);
	//Apogee and perigee radius magnitude
	void PCHAPE(double R1, double R2, double R3, double U1, double U2, double U3, double &RAP, double &RPE);
	//Plane change time and velocity increments computations
	void PMMPNE(AEGBlock sv_C, AEGBlock sv_T, double TREF, double FNPC, int KPC, int IPC, AEGBlock &SAVE, double &DI1, double &DH1);
	bool DockingInitiationProcessor(DKIOpt opt, DKIResults &res);
	int ConcentricRendezvousProcessor(const SPQOpt &opt, SPQResults &res);
	double CalculateTPITimes(SV sv0, int tpimode, double t_TPI_guess, double dt_TPI_sunrise);
	void AGOPCislunarNavigation(SV sv, MATRIX3 REFSMMAT, int star, double yaw, VECTOR3 &IMUAngles, double &TA, double &SA);
	VECTOR3 LOICrewChartUpdateProcessor(SV sv0, double GETbase, MATRIX3 REFSMMAT, double p_EMP, double LOI_TIG, VECTOR3 dV_LVLH_LOI, double p_T, double y_T);
	SV coast(SV sv0, double dt);
	MPTSV coast(MPTSV sv0, double dt);
	MPTSV coast_conic(MPTSV sv0, double dt);
	EphemerisData coast(EphemerisData sv1, double dt);
	VECTOR3 HatchOpenThermalControl(VESSEL *v, MATRIX3 REFSMMAT);
	VECTOR3 PointAOTWithCSM(MATRIX3 REFSMMAT, SV sv, int AOTdetent, int star, double dockingangle);
	void DockingAlignmentProcessor(DockAlignOpt &opt);
	//Apsides Determination Subroutine
	bool PMMAPD(SV sv0, SV &sv_a, SV &sv_p);
	VECTOR3 HeightManeuverInteg(SV sv0, double dh);
	VECTOR3 ApoapsisPeriapsisChangeInteg(SV sv0, double r_AD, double r_PD);
	VECTOR3 CircularizationManeuverInteg(SV sv0);
	void ApsidesArgumentofLatitudeDetermination(SV sv0, double &u_x, double &u_y);
	bool GETEval(double get);
	bool PDIIgnitionAlgorithm(SV sv, double GETbase, VECTOR3 R_LS, double TLAND, SV &sv_IG, double &t_go, double &CR, VECTOR3 &U_IG, MATRIX3 &REFSMMAT);
	bool PoweredDescentAbortProgram(PDAPOpt opt, PDAPResults &res);
	VECTOR3 RLS_from_latlng(double lat, double lng, double alt);

	//Mission Operations Control Room Displays

	//FDO Orbit Digitals
	void EMMDYNMC(int L, int queid, int ind = 0, double param = 0.0);
	//FDO Space Digitals
	int EMDSPACE(int queid, int option = 0, double val = 0.0, double incl = 0.0, double ascnode = 0.0);
	//Orbit Station Contact Generation Control
	void EMSTAGEN(int L);
	//Next Station Contact Display
	void EMDSTAC();
	//Predicted Site Acquisition Display
	void EMDPESAD(int num, int veh, int ind, double vala, double valb, int body);
	//Ground Range and Altitude Subprogram
	void ECMEXP(EphemerisData sv, Station *stat, int statbody, double &range, double &alt);
	//Subsatellite Position
	int GLSSAT(EphemerisData sv, double &lat, double &lng, double &alt);
	//Landmark Acquisition Display
	void EMDLANDM(int L, double get, double dt, int ref);
	//Display Updates
	void EMSNAP(int L, int ID);

	//Actual RTCC Subroutines

	//AEG Service Routine
	void PMMAEGS(AEGHeader &header, AEGDataBlock &in, AEGDataBlock &out);
	//Time of Longitude Crossing Determination
	void PMMTLC(AEGHeader HEADER, AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double DESLAM, int &K, int INDVEC);
	//AEG Day/Night Determination
	void PMMDAN(AEGBlock aeg, int IND, int &ERR, double &T1, double &T2);
	//LM AGS External DV Coordinate Transformation Subroutine
	VECTOR3 PIAEDV(VECTOR3 DV, VECTOR3 R_CSM, VECTOR3 V_CSM, VECTOR3 R_LM, bool i);
	//External DV Coordinate Transformation Subroutine
	VECTOR3 PIEXDV(VECTOR3 R_ig, VECTOR3 V_ig, double WT, double T, VECTOR3 DV, bool i);
	//Generalized Coordinate System Conversion Subroutine
	int ELVCNV(EphemerisDataTable &svtab, int in, int out, EphemerisDataTable &svtab_out);
	int ELVCNV(EphemerisData &sv, int in, int out, EphemerisData &sv_out);
	int ELVCNV(VECTOR3 vec, double GMT, int in, int out, VECTOR3 &vec_out);
	//Checkout Monitor Display
	void EMDCHECK(int veh, int opt, double param, double THTime, int ref, bool feet);
	//Detailed Maneuver Table Display
	void PMDDMT(int MPT_ID, unsigned ManNo, int REFSMMAT_ID, bool HeadsUp, DetailedManeuverTable &res);
	//Lunar Descent Planning Table Display
	void PMDLDPP(const LDPPOptions &opt, const LDPPResults &res, LunarDescentPlanningTable &table);
	//Time of Longitude Crossing Subroutine
	double RLMTLC(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, double long_des, double GMT_min, double &GMT_cross, LunarStayTimesTable *LUNRSTAY = NULL);
	//Computes and outputs pitch, yaw, roll
	void RLMPYR(VECTOR3 X_P,VECTOR3 Y_P,VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll);
	//LEM gimbal angle + FDAI angle computation routine
	void EMGLMRAT(VECTOR3 X_P, VECTOR3 Y_P, VECTOR3 Z_P, VECTOR3 X_B, VECTOR3 Y_B, VECTOR3 Z_B, double &Pitch, double &Yaw, double &Roll, double &PB, double &YB, double &RB);
	//Vector rotation routine
	VECTOR3 GLMRTV(VECTOR3 A, double THET1, int K1, double THET2 = 0.0, int K2 = 0.0, double THET3 = 0.0, int K3 = 0);
	//Matrix rotation routine
	MATRIX3 GLMRTM(MATRIX3 A, double THET1, int K1, double THET2 = 0.0, int K2 = 0.0, double THET3 = 0.0, int K3 = 0);
	//Cape Crossing Table Generation
	int RMMEACC(int L, int ref_frame, int ephem_type, int rev0);
	//Ascending Node Computation
	int RMMASCND(int L, double GMT_min, double &lng_asc);
	//Environment Change Calculations
	int EMMENV(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, double GMT_begin, bool sun, SunriseSunsetTable &table);
	//Sunrise/Sunset Display
	void EMDSSEMD(int ind, double param);
	//Moonrise/Moonset Display
	void EMDSSMMD(int ind, double param);
	//Ephemeris Fetch Routine
	int ELFECH(double GMT, int L, EphemerisData &SV);
	int ELFECH(double GMT, unsigned vec_tot, unsigned vec_bef, int L, EphemerisDataTable &EPHEM, ManeuverTimesTable &MANTIMES, LunarStayTimesTable &LUNSTAY);
	//Vector Count Routine
	int ELNMVC(double TL, double TR, int L, unsigned &NumVec, int &TUP);
	int NewMPTTrajectory(int L, SV &sv0);
	//RTE Tradeoff Display Sort and Order Routine
	int PMQREAP(const std::vector<TradeoffData> &TOdata);
	//Return to Earth Abort Planning Supervisor
	void PMMREAP(int med);
	//Return to Earth Abort Planning Supervisor (Abort Scan Table)
	void PMMREAST();
	//RTE Trajectory Computer
	bool PCMATC(std::vector<double> &var, void *varPtr, std::vector<double>& arr, bool mode);
	//Lunar Orbit Insertion Computational Unit
	void PMMLRBTI(EphemerisData sv);
	//Lunar Orbit Insertion Display
	void PMDLRBTI(const rtcc::LOIOptions &opt, const rtcc::LOIOutputData &out);
	//Central Manual Entry Device Decoder
	bool GMGMED(char *str);
	//MED Decoder for G, A and B MEDs
	int EMGABMED(int type, std::string med, std::vector<std::string> data);
	//MED Decoder for C MEDs
	int CMRMEDIN(std::string med, std::vector<std::string> data);
	//'F' MED Module
	int PMQAFMED(std::string med);
	int PMQAFMED(std::string med, std::vector<std::string> data);
	//K-MED Decoder
	void PMKMED(std::string med);
	//'M' MED Module
	int PMMMED(std::string med, std::vector<std::string> data);
	//'P' Code MED Processor
	int GMSMED(std::string med, std::vector<std::string> data);
	//'U' Code MED Processor
	int EMGTVMED(std::string med, std::vector<std::string> data);
	//Restart MED Decoder
	int GMSREMED(std::string med, std::vector<std::string> data);
	//High-Speed Restart MED Processor
	void GMLRESRT(int type);
	//Detailed Maneuver Table Math Module
	void PMMDMT(int L, unsigned man, RTCCNIAuxOutputTable *aux);
	//Time Queue Control Load Module
	void EMSTIME(int L, int ID);
	void FDOLaunchAnalog1(MPTSV sv);
	void FDOLaunchAnalog2(MPTSV sv);
	double GetGMTLO() { return MCGMTL; }
	void SetGMTLO(double gmt) { MCGMTL = gmt; }
	double CalcGETBase();
	double GetGMTBase() { return GMTBASE; }
	void SetGMTBase(double gmt) { GMTBASE = gmt; }
	double GETfromGMT(double GMT);
	double GMTfromGET(double GET);
	//Arrival time at selenographic argument of latitude
	int PIATSU(AEGDataBlock AEGIN, AEGDataBlock &AEGOUT, double &isg, double &gsg, double &hsg);

	//Skylark
	bool SkylabRendezvous(SkyRendOpt *opt, SkylabRendezvousResults *res);
	bool NC1NC2Program(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh_F, double n_H1, int s, double dh, double n_C, VECTOR3 &dV_NC1_LVLH, double &dh_NC2, double &dv_NC2, double &t_NC2, VECTOR3 &dV_NC2_LVLH, double &dv_NCC, double &t_NCC, double &t_NSR, VECTOR3 &dV_NSR, bool NPC = false);
	void NCCProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t_C, double dt, double t_F, double dh, VECTOR3 &dV_NCC_LVLH, double &t_NSR, VECTOR3 &dV_NSR_LVLH);
	void NSRProgram(SV sv_C, SV sv_W, double GETbase, double E_L, double t2, double t3, VECTOR3 &dV_NSR_LVLH);
	void NPCProgram(SV sv_C, SV sv_W, double GETbase, double t, double &t_NPC, VECTOR3 &dV_NPC_LVLH);

	//Mission Planning

	//Maneuver Transfer Module
	int PMMXFR(int id, void* data);
	//Maneuver direct input and confirmation math module
	int PMMMCD(PMMMCDInput in, MPTManeuver &man);
	//Impulsive Maneuver Transfer Math Module
	int PMMMPT(PMMMPTInput in, MPTManeuver &man);
	//Lunar Ascent Integrator
	int PMMLAI(PMMLAIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable *E = NULL);
	//LM Lunar Descent Numerical Integration Module
	int PMMLDI(PMMLDIInput in, RTCCNIAuxOutputTable &aux, EphemerisDataTable *E = NULL);
	//LM Lunar Descent Pre-Thrust Targeting Module
	int PMMLDP(PMMLDPInput in, MPTManeuver &man);
	//Freeze, Unfreeze, Delete Processor
	void PMMFUD(int veh, unsigned man, int action);
	//Vehicle Orientation Change Processor
	void PMMUDT(int L, unsigned man, int headsup, int trim);
	//Vector Routing Load Module
	void PMSVCT(int QUEID, int L, EphemerisData* sv0 = NULL, bool landed = false);
	//Vector Fetch Load Module
	int PMSVEC(int L, double GMT, CELEMENTS &elem, double &KFactor, double &Area, double &Weight, std::string &StaID, int &RBI);
	//Maneuver Execution Program
	void PMSEXE(int L, double gmt);
	//Earth Orbit Insertion Processor
	void PMMIEV(double T_L);
	//Mission Planning Print Load Module
	void PMXSPT(std::string source, int n);
	void PMXSPT(std::string source, std::vector<std::string> message);
	void OnlinePrintTime(double TIME_SEC, std::string &time);
	void OnlinePrint(const std::string &source, const std::vector<std::string> &message);
	//Mission Control Print Program
	void GMSPRINT(std::string source, int n);
	void GMSPRINT(std::string source, std::vector<std::string> message);
	//Trajectory Update Control Module
	void EMSTRAJ(EphemerisData sv, int L, bool landed = false);
	//Ephemeris Storage and Control Module
	EphemerisData EMSEPH(int QUEID, EphemerisData sv0, int L, double PresentGMT, bool landed = false);
	//Miscellaneous Numerical Integration Control Module
	void EMSMISS(EMSMISSInputTable &in);
	//Encke Integrator
	EphemerisData EMMENI(EMSMISSInputTable &in, EphemerisData sv, double dt);
	//Spherical to inertial conversion
	int EMMXTR(double GMT, double rmag, double vmag, double rtasc, double decl, double fpav, double az, VECTOR3 &R, VECTOR3 &V);
	//Anchor Vector Maintenance Module
	void EMGVECSTInput(int L, EphemerisData sv);
	int EMGVECSTOutput(int L, EphemerisData &sv);
	int ThrusterNameToCode(std::string thruster);
	int AttitudeNameToCode(std::string attitude);

	bool MPTHasManeuvers(int L);
	//Weight Change Module
	int PMMWTC(int med);
	//Weight Determination at a Time
	int PLAWDT(int L, double gmt, double &cfg_weight);
	int PLAWDT(int L, double gmt, std::bitset<4> &cfg, double &cfg_weight, double &csm_weight, double &lm_asc_weight, double &lm_dsc_weight, double &sivb_weight);
	//Gimbal, Thrust and Weight Loss Rate Subroutine
	void GIMGBL(double CSMWT, double LMWT, double &RY, double &RZ, double &T, double &WDOT, int ITC, unsigned &IC, int IA, int IJ, double D);
	VECTOR3 GIMGB2(const double *WArr, const VECTOR3 *VecArr, int N, double W);
	double GetOnboardComputerThrust(int thruster);
	void GetSystemGimbalAngles(int thruster, double &P_G, double &Y_G) const;
	double RTCCPresentTimeGMT();
	OBJHANDLE GetGravref(int body);
	bool RTEManeuverCodeLogic(char *code, double csmmass, double lmascmass, double lmdscmass, int &thruster, double &manmass);

	//Uplink Mission Programs (C-Code)

	//CMC External Delta-V Update Generator
	void CMMAXTDV(double GETIG, VECTOR3 DV_EXDV);
	//LGC External Delta-V Update Generator
	void CMMLXTDV(double GETIG, VECTOR3 DV_EXDV);
	//CMC and LGC REFSMMAT Update Generator
	void CMMRFMAT(int L, int id, int addr);
	//SLV Navigation Update
	void CMMSLVNAV(VECTOR3 R_ecl, VECTOR3 V_ecl, double GMT);

	//Trajectory Determination
	//Vector Comparison Control
	void BMSVEC();
	//Vector Comparison Display
	void BMDVEC();
	//Online print of trajectory determination
	void BMGPRIME(std::string source, int n);
	void BMGPRIME(std::string source, std::vector<std::string> message);

	//Launch 
	void LMMGRP(double gmt, double A_Z);

	//Guidance

	//Guidance Optics Display Supervisor
	void EMSGSUPP(int QUEID, int refs, int refs2 = -1, unsigned man = -1, bool headsup = true);
	//LEM Optics Supervisor
	void EMSLSUPP(int QUEID, int refs, int refs2 = -1, unsigned man = -1, bool headsup = true);
	//GOST REFSMMAT Maintenance
	void EMGSTGEN(int QUEID, int L1, int ID1, int L2, int ID2, double gmt, MATRIX3 *refs = NULL);
	//GOST Matrix Storage and Print
	void EMGSTSTM(int L, MATRIX3 REFS, int id, double gmt);
	void EMGSTGENName(int ID, char *Buffer);
	int EMGSTGENCode(const char *Buffer);
	//Guidance Optics Support Table
	void EMMGSTMP();
	//Guidance Optics Display
	void EMDGSUPP(int err);

	void SaveState(FILEHANDLE scn);							// Save state
	void LoadState(FILEHANDLE scn);							// Load state

	MCC *mcc;
	struct calculationParameters calcParams;

	//MEDs

	//Generate the near Earth tradeoff display
	struct MED_F70
	{
		std::string Site;
		//Time of vector (hrs.)
		double T_V = 0.0;
		//Minimum abort time (hrs.)
		double T_omin = 0.0;
		//Maximum abort time (hrs.)
		double T_omax = 0.0;
		//1 = Constant G reentry (HB1), 2 = G&N reentry (HGN)
		int EntryProfile = 1;
	} med_f70;

	//Generate the remote Earth tradeoff display
	struct MED_F71
	{
		int Page = 1;
		std::string Site;
		double T_V = 0.0;
		double T_omin = 0.0;
		double T_omax = 0.0;
		//1 = Constant G reentry (HB1), 2 = G&N reentry (HGN)
		int EntryProfile = 1;
	} med_f71;

	//Generation of Abort Scan Table for unspecified area
	struct MED_F75
	{
		int Type = 0; //0 = TCUA, 1 = FCUA
		double T_V = 0.0; //Vector time
		double T_0 = 0.0; //Time of abort
		double DVMAX = 0.0;
		int EntryProfile = 0;
	} med_f75;

	//Update return to Earth constraints
	struct MED_F86
	{
		std::string Constraint;
		double Value;
	} med_f86;

	//Update return to Earth constraints
	struct MED_F87
	{
		std::string Constraint;
		std::string Value;
	} med_f87;

	//Generate DKI
	struct MED_K00
	{
		int ChaserVehicle = 1; //1 = CSM, 3 = LEM
	} med_k00;

	//Generate SPQ
	struct MED_K01
	{
		int ChaserVehicle = 1; //1 = CSM, 3 = LEM
		double ChaserThresholdGET = -1.0;
		double TargetThresholdGET = -1.0;
		//1 = CDH at next apsis, 2 = CDH on time, 3 = angle from CSI
		int I_CDH = 3;
		//For option 1
		int CDH_Apsis = 1;
		//For option 2
		double CDH_Time = 0.0;
		//For option 3
		double CDH_Angle = PI;
	} med_k01;

	//Maneuver Line Definition Initialization
	struct MED_K10
	{
		int MLDOption = 2; //1 = Angle, 2 = Time, 3 = Delta Time, 4 = Time and Angle
		double MLDTime = -1.0;
		double MLDAngle = 0.0;
	} med_k10;

	//Generate Launch Window
	struct MED_K15
	{
		//1 = CSM, 3 = LEM
		int Chaser = 3;
		double CSMVectorTime = 0.0;
		double ThresholdTime = 0.0;
		//CSI Flag: 0 = CSI done 90° from insertion, negative: CSI at LM apocynthion, positive: CSI done at a delta time from insertion
		double CSI_Flag = -1.0;
		//CDH Flag: 0 = CDH done at upcoming apsis after CSI, positive: CDH is done at N/2 after CSI, must be odd number
		int CDH_Flag = 1;
		//1 = TPI at longitude, 2 = TPI at time
		int TPIDefinition = 2;
		double TPIValue = 0.0;
		//Positive: request for complete launch window and the number of delta hts, negative: request for the input delta hts only
		int DeltaHTFlag = -1;
		double DH1 = 10.0*1852.0;
		double DH2 = 15.0*1852.0;
		double DH3 = 20.0*1852.0;
	} med_k15;

	//LOI Computation
	struct MED_K18
	{
		double HALOI1 = 170.0;
		double HPLOI1 = 60.0;
		double DVMAXp = 10000.0;
		double DVMAXm = 10000.0;
		double psi_DS = 270.0;
		double psi_MX = 271.0;
		double psi_MN = 269.0;
		double VectorTime = 0.0;
	} med_k18;

	//GPM Maneuver Computation
	struct MED_K20
	{
		int Vehicle = 1; //1 = CSM, 3 = LM
		double VectorTime = 0.0;
		double ThresholdTime = 0.0;
	} med_k20;

	//Two Impulse Computation
	struct MED_K30
	{
		int Vehicle = 1; //1 = CSM, 3 = LEM
		int IVFlag = 0; //0 = Time of both maneuvers fixed, 1 = Time of first maneuver fixed, 2 = time of second maneuver fixed
		double ChaserVectorTime = 0.0;
		double TargetVectorTime = 0.0;
		double StartTime = 0.0;
		double EndTime = 0.0;
		double TimeStep = 60.0;
		double TimeRange = 600.0;
	} med_k30;

	//LOI Initialization (Apollo 14 and later, MED code is not from any documentation!)
	struct MED_K40
	{
		double HA_LLS = 60.0;
		double HP_LLS = 8.23;
		double DW = -15.0;
		double REVS1 = 2.0;
		int REVS2 = 11;
		double eta_1 = 0.0;
		double dh_bias = 0.0;
		bool PlaneSolnForInterSoln = true;
	} med_k40;

	//Lunar Launch Targeting Processor (Apollo 14 and later, MED code is not from any documentation!)
	struct MED_K50
	{
		double GETTH = 0.0;
		double GETV = 0.0;
	} med_k50;

	//Transfer a GPM to the MPT
	struct MED_M65
	{
		int Table = RTCC_MPT_CSM; //1 = CSM, 3 = LEM
		int ReplaceCode = 0; //1-15
		int Thruster = RTCC_ENGINETYPE_CSMSPS; //Thruster for the maneuver
		int Attitude = RTCC_ATTITUDE_PGNS_EXDV;		//Attitude option
		double UllageDT = -1;	//Delta T of Ullage
		bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
		bool Iteration = false; //false = do not iterate, true = iterate
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
		bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
	} med_m65;

	//Direct input of a maneuver to the MPT (CSM and LEM)
	struct MED_M66
	{
		int Table = 1; //1 = CSM, 3 = LEM
		unsigned ReplaceCode = 0; //1-15
		double GETBI = 0; //Time of ignition
		int Thruster = RTCC_ENGINETYPE_CSMSPS; //Thruster for maneuver
		int AttitudeOpt = RTCC_ATTITUDE_PGNS_EXDV; //Attitude option
		int BurnParamNo = 2; //1 = P1, 2 = P2 etc.
		int CoordInd = 0; //0 = LVLH, 1 = IMU, 2 = FDAI
		VECTOR3 Att = _V(0, 0, 0);
		double UllageDT = 0.0;	//Delta T of Ullage
		bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
		bool HeadsUp = true; //false = heads down, true = heads up
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		int REFSMMATInd = RTCC_REFSMMAT_TYPE_CUR;	//Used for IMU and inertial coordinate options
		int ConfigChangeInd = 0; //0 = No change, 1 = Undocking, 2 = Docking
		std::string FinalConfig = "C";
		double DeltaDA = 0.0; //Delta docking angle
		double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
		int TrimAngleIndicator = 0; //0 = computed, 2 = system
	} med_m66;

	//LOI and MCC Transfer
	struct MED_M78
	{
		int Table = 1; //1 = CSM, 3 = LEM
		unsigned ReplaceCode = 0; //1-15
		bool Type = false; //false = MCC, true = LOI
		unsigned ManeuverNumber = 1; //Maneuver number in LOI or MCC table
		int Thruster = RTCC_ENGINETYPE_CSMSPS; //Thruster for maneuver
		int Attitude = RTCC_ATTITUDE_PGNS_EXDV; //Attitude option
		double UllageDT = -1;	//Delta T of Ullage
		bool UllageQuads = true;//false = 2 thrusters, true = 4 thrusters
		bool Iteration = false; //false = do not iterate, true = iterate
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		double DPSThrustFactor = 0.925; //Main DPS thrust scaling factor
		bool TimeFlag = false;	//false = use optimum time, true = start at impulsive time
	} med_m78;

	//Transfer ascent maneuver to MPT from Lunar Targeting
	struct MED_M85
	{
		int VEH = 3;
		int ReplaceCode = 0;
	} med_m85;

	//Direct Input of Lunar Descent Maneuver
	struct MED_M86
	{
		int Veh = 3; //3 = LEM
		int ReplaceCode = 0;
		bool TimeFlag = false; //false = Nominal landing time to be input, true = Ignition time (only if replacing descent maneuver)
		double Time = 0.0;
		double dt_ullage = 8.0;
		bool UllageQuads = true; //false = 2 thrusters, true = 4 thrusters
		bool HeadsUp = true; //false = heads down, true = heads up
		double TenPercentDT = 26.0;	//Delta T of 10% thrust for the DPS
		int TrimAngleIndicator = 0; //0 = computed, 2 = system
	} med_m86;

	MED_K16 med_k16;
	MED_K17 med_k17;

	struct MED_M49
	{
		int Table = 1;
		double SPSFuelRemaining = -1;
		double CSMRCSFuelRemaining = -1;
		double SIVBFuelRemaining = -1;
		double LMAPSFuelRemaining = -1;
		double LMRCSFuelRemaining = -1;
		double LMDPSFuelRemaining = -1;
	} med_m49;

	MED_M50 med_m50;
	MED_M51 med_m51;
	MED_M55 med_m55;
	MED_M68 med_m68;
	MED_M70 med_m70;
	MED_M72 med_m72;

	struct MED_S80
	{
		//1 = CSM, 3 = LEM
		int VEH = 1;
		//GMT if positive, GET if negative, time of base vector if 0
		double time = 0.0;
		int REF = BODY_EARTH;
		std::string VID[4];
	} med_s80;

	//Data Tables
	PZEFEM pzefem;
	CapeCrossingTable EZCCSM;
	CapeCrossingTable EZCLEM;
	SunriseSunsetTable EZSSTAB;
	SunriseSunsetTable EZMMTAB;
	TradeoffDataDisplay RTETradeoffTable;
	TradeoffDataDisplayBuffer RTETradeoffTableBuffer[5];
	MissionPlanTable PZMPTCSM, PZMPTLEM;
	DetailedManeuverTable DMTBuffer[2];
	BurnParameterTable PZBURN;

	std::vector<VECTOR3> EZJGSTAR;

	struct LunarSurfaceAlignmentTable
	{
		MATRIX3 LLD_REFSMMAT;
		bool LLDRefsPresent = false;
	} EZJGLSAD;

	struct GuidanceOpticsSupportTable
	{
		std::string CODE = "ZZZZZZZZ";
		VECTOR3 Att_H = _V(0, 0, 0);
		double GETAC = 0.0;
		double IGA = 0.0;
		std::string IRA = "ZZZZZZ";
		unsigned SXT_STAR[2] = { 0,0 };
		double SXT_SFT_INP[2] = { 0.0, 0.0 };
		double SXT_TRN_INP[2] = { 0.0, 0.0 };
		double SXT_SFT_RTCC[2] = { 0.0, 0.0 };
		double SXT_TRN_RTCC[2] = { 0.0, 0.0 };
		unsigned SCT_S[2] = { 0,0 };
		double SCT_SF = 0.0;
		double SCT_TR = 0.0;
		double SCT_M[2] = { 0.0, 0.0 };
		double SCT_R[2] = { 0.0, 0.0 };
		double SCT_RTASC[2] = { 0.0, 0.0 };
		double SCT_DEC[2] = { 0.0, 0.0 };
		unsigned BS_S[2] = { 0,0 };
		double BS_SPA[2] = { 0.0, 0.0 };
		double BS_SXP[2] = { 0.0, 0.0 };
		double BS_RTASC[2] = { 0.0, 0.0 };
		double BS_DEC[2] = { 0.0, 0.0 };
		VECTOR3 Landmark_LOS = _V(0, 0, 0);
		double Landmark_GET = 0.0;
		std::string Landmark_SC = "ZZZ";
		double Landmark_RA = 0.0;
		double Landmark_DEC = 0.0;
		VECTOR3 Att[2] = { _V(0, 0, 0), _V(0, 0, 0) };
		std::string MAT = "ZZZZZZ";
		MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	} EZJGSTTB;

	struct GOSTDisplay
	{
		GuidanceOpticsSupportTable data;
		std::string err;
	} GOSTDisplayBuffer;

	struct LunarOpticsSupportTable
	{
		std::string MAT;
		MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
	} EZJGSTBL;

	struct LOSTDisplay
	{
		std::string MAT;
		MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
		std::string err;
	} LOSTDisplayBuffer;

	REFSMMATLocker EZJGMTX1, EZJGMTX3;
	FIDOOrbitDigitals EZSAVCSM, EZSAVLEM;
	SpaceDigitals EZSPACE;
	OrbitStationContactsTable EZSTACT1, EZSTACT3;
	NextStationContactsTable NextStationContactsBuffer;
	PredictedSiteAcquisitionTable EZACQ1, EZACQ3, EZDPSAD1, EZDPSAD3;
	ExperimentalSiteAcquisitionTable EZDPSAD2;
	LandmarkAcquisitionTable EZLANDU1;
	LunarLaunchTargetingTable PZLLTT;

	struct EvaluationVectorTable
	{
		//0 = CSM, CMC
		//1 = CSM, LGC
		//2 = CSM, AGS
		//3 = CSM, IU
		//4 = CSM, HSR
		//5 = CSM, DC
		//6 = LM, CMC
		//7 = LM, LGC
		//8 = LM, AGS
		//9 = LM, IU
		//10 = LM, HSR
		//11 = LM, DC
		EphemerisData Vectors[12];
		int ID[12] = { -1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1 };
	} BZEVLVEC;

	struct VectorCompareTableData
	{
		double GMT = 0.0;
		double HA = 0.0;
		double HP = 0.0;
		double v = 0.0;
		double gamma = 0.0;
		double psi = 0.0;
		double phi = 0.0;
		double lambda = 0.0;
		double h = 0.0;
		double a = 0.0;
		double e = 0.0;
		double i = 0.0;
		double theta_p = 0.0;
		double Omega = 0.0;
		double nu = 0.0;
		double U = 0.0;
		double V = 0.0;
		double W = 0.0;
		double U_dot = 0.0;
		double V_dot = 0.0;
		double W_dot = 0.0;
	};

	struct VectorCompareTable
	{
		bool error = false;
		int NumVec = 0;
		VectorCompareTableData data[4];
	} BZCCANOE;

	struct VectorCompareDisplay
	{
		bool showWPAndTA[4] = { false,false,false,false };
		bool showHA[4] = { false,false,false,false };
		int NumVec = 0;
		VectorCompareTableData data[4];
		double GMT = 0.0;
		double PET = 0.0;
		double GMTR = 0.0;
		std::string error = "TABLE NOT INITIALIZED";
	} VectorCompareDisplayBuffer;

	struct TelemetryTrajectoryInterfaceTable
	{
		MATRIX3 LGC_REFSMMAT;
		bool LGCRefsPresent = false;
		MATRIX3 CMC_REFSMMAT;
		bool CMCRefsPresent = false;
	} BZSTLM;

	struct RelativeMotionDigitalsTableEntry
	{
		double GET = 0.0;
		double PETorShaft = 0.0;
		double R = 0.0;
		double YdotorTrun = 0.0;
		double RDOT = 0.0;
		double AZH = 0.0;
		double EL = 0.0;
		double X = 0.0;
		double Y = 0.0;
		double Z = 0.0;
		char XInd = ' ';
		char YInd = ' ';
		char ZInd = ' ';
		double Pitch = 0.0;
		double Yaw = 0.0;
		double Roll = 0.0;
		double PB = 0.0;
		double YB = 0.0;
		double RB = 0.0;
	};

	struct RelativeMotionDigitalsTable
	{
		std::string CSMSTAID;
		std::string LMSTAID;
		double GETR = 0.0;
		double CSMGMTV = 0.0;
		double CSMGETV = 0.0;
		double LMGMTV = 0.0;
		double LMGETV = 0.0;
		std::string REFSMMAT;
		std::string VEH;
		char AXIS = ' ';
		char Mode = ' ';
		RelativeMotionDigitalsTableEntry data[11];
		std::string error = "MED OUTDATED";

		int solns = 0;
		char Pitch = ' ';
		char Yaw = ' ';
		char Roll = ' ';
		std::string PETorSH;
		std::string YDotorT;
	} EZRMDT;

	struct ExperimentalSiteGroundPointTable
	{
		//-1 when empty, 0 = Earth, 1 = Moon
		int REF = -1;
		Station Data[12];
	} EZEXSITE;

	struct LandmarkSitesTable
	{
		//-1 when empty, 0 = Earth, 1 = Moon
		int REF = -1;
		Station Data[12];
	} EZLASITE;

	struct OrbitEphemerisTable
	{
		EphemerisDataTable EPHEM;
		ManeuverTimesTable MANTIMES;
		LunarStayTimesTable LUNRSTAY;
	} EZEPH1, EZEPH2;

	struct TLITargetingParametersTable
	{
		int Day;
		int Month;
		int Year;
		double T_LO;
		double theta_EO;
		double omega_E;		
		double K_a1 = 0.0;
		double K_a2 = 0.0;
		double K_T3;

		double T_ST[2];
		double beta[2];
		double alpha_TS[2];
		double f[2];
		double R_N[2];
		double T3_apo[2];
		double tau3R[2];
		double T2[2];
		double Vex2[2];
		double Mdot2[2];
		double DV_BR[2];
		double tau2N[2];
		double KP0[2];
		double KY0[2];

		double cos_sigma[2][15];
		double C_3[2][15];
		double e_N[2][15];
		double RA[2][15];
		double DEC[2][15];
		double t_D[2][15];
	} PZSTARGP;

	struct TLISystemParameters
	{
		//Time of ignition of first S-IVB burn
		double T4IG = 0.0;
		//Time of cutoff of first S-IVB
		double T4C = 0.0;
		//Time interval from time of restart preparations to time of ignition
		double DTIG = 578.0;
		//Nominal time duration of first S-IVB burn
		double DT4N = 0.0;
		//Limited value for difference between actual and nominal burn times for first S-IVB burn
		double DTLIM = 100.0;
		//Second coefficient of pitch polynomial
		double KP1 = 0.0;
		//Third coefficient of pitch polynomial
		double KP2 = 0.0;
		//Second coefficient of yaw polynomial
		double KY1 = 0.0;
		//Third coefficient of yaw polynomial
		double KY2 = 0.0;
		//Geodetic latitude of launch site
		double PHIL = 0.0;
		//Azimuth from time polynomial
		double hx[3][5];
		//Inclination from azimuth polynomial
		double fx[7];
		//Descending Node Angle from azimuth polynomial
		double gx[7];
		//Times of the opening and closing of launch windows
		double t_D0, t_D1, t_D2, t_D3;
		//Times to segment the azimuth calculation polynomial
		double t_DS0, t_DS1, t_DS2, t_DS3;
		//Times used to scale the azimuth polynomial
		double t_SD1, t_SD2, t_SD3;
	} MDVSTP;

	struct SIVBTLIMatrixTable
	{
		//Plumbline coordinate axes in ECI coordinates
		MATRIX3 EPH;
		//Plumbline to parking orbit nodal system transformation matrix
		MATRIX3 GG;
		//Plumbline to target orbit nodal system transformation matrix
		MATRIX3 G;
	} PZTLIMAT;

	struct TLIPlanningOutputTable
	{
		//Target vector
		VECTOR3 T;
		//Unit nodal vector which defines node of desired cutoff plane and parking orbit plane at time of restart preparation
		VECTOR3 S;
		//Position vector at time of restart preparation
		VECTOR3 R;
		//Velocity vector at time of restart preparation
		VECTOR3 V;
		double TB6;
		double TIG;
		double i;
		double theta_N;
		double sigma;
		double C3;
	} PZTTLIPL;

	struct LOIElementsTable
	{
		EphemerisData sv_man_bef[8];
		VECTOR3 V_man_after[8];
	} PZLRBELM;

	struct LOIDisplayTableElement
	{
		double GETLOI = 0.0;
		double DVLOI1 = 0.0;
		double DVLOI2 = 0.0;
		double H_ND = 0.0;
		double f_ND_H = 0.0;
		double H_PC = 0.0;
		double Theta = 0.0;
		double f_ND_E = 0.0;
	};

	struct LOIDisplayTable
	{
		std::string StaID = "STAT000";
		double VectorGET = 0.0;
		double lat_lls = 0.0;
		double lng_lls = 0.0;
		double R_lls = 0.0;
		double AZMN_f_ND = 0.0;
		double AZMX_f_ND = 0.0;
		double REVS1 = 0.0;
		int REVS2 = 0;
		double DHBIAS = 0.0;
		double AZ_LLS = 0.0;
		double f_LLS = 0.0;
		double HALOI1 = 0.0;
		double HPLOI1 = 0.0;
		double HALOI2 = 0.0;
		double HPLOI2 = 0.0;
		double DVMAXp = 0.0;
		double DVMAXm = 0.0;
		double RARPGT = 0.0;
		bool planesoln = true;
		LOIDisplayTableElement sol[8];
	} PZLRBTI;

	struct MCCTransferTable
	{
		EphemerisData sv_man_bef[6];
		VECTOR3 V_man_after[6];
	} PZMCCXFR;

	struct LDPPElementsTable
	{
		EphemerisData sv_man_bef[4];
		VECTOR3 V_man_after[4];
		int num_man;
		int plan[4];
		std::string code[4];
	} PZLDPELM;

	struct LMPositionVectorTable
	{
		double lat[4];
		double lng[4];
	} BZLSDISP;//Is this the right one?

	struct HistoryAnchorVectorTable
	{
		EphemerisData AnchorVectors[10];
		std::string StationIDs[10];
		int num = 0;
	} EZANCHR1, EZANCHR3;

	struct TimeConstraintsTable
	{
		EphemerisData sv_present;
		double a = 0.0;
		double e = 0.0;
		double i = 0.0;
		double gamma = 0.0;
		double lat = 0.0;
		double lng = 0.0;
		double h = 0.0;
		double T0 = 0.0;
		double TA = 0.0;
		double V = 0.0;
		double azi = 0.0;
		double AoP = 0.0;
		double RA = 0.0;
		double l = 0.0;
		double WT = 0.0;
		int OrbitNum = 0;
		int RevNum = 0;
		//EI time?
		double GMTPI = 0.0;
		std::string StationID;
		int TUP = 0;
	} EZTSCNS1, EZTSCNS3;

	struct GeneralConstraintsTable
	{
		//Block 1
		int Year = 0;
		//Block 2
		int RefDayOfYear = 0;
		//Block 3
		int DaysInYear = 0;
		//Block 4
		int MonthofLiftoff = 0;
		int DayofLiftoff = 0;
		int DaysinMonthofLiftoff = 0;
		//Block 5
		double DKIElevationAngle = 26.6*RAD;
		//Block 8
		double DKITerminalPhaseAngle = 130.0*RAD;
		//Block 9
		double TIDeltaH = 0.0;
		//Block 10
		double TIPhaseAngle = 0.0;
		//Block 11
		double TIElevationAngle = 26.6*RAD;
		//Block 12
		double TITravelAngle = 130.0*RAD;
		//Block 13
		double TINSRNominalTime = 0.0;
		//Block 14
		double TINSRNominalDeltaH = 0.0;
		//Block 15
		double TINSRNominalPhaseAngle = 0.0;
		//Block 18 Bytes 5-8
		int TPFDefinition;
		//Block 25
		double DKIDeltaH = 0.0;
		//Block 26
		double TPFDefinitionValue;
		//Block 27
		double DKIMinPerigee = 0.0;
		//Block 28 Bytes 1-4
		double DeltaNSR;
		//Block 29
		double TPIDefinitionValue = 0.0;
		//Block 30
		double OrbitsFromCSItoTPI;
		//Block 31 Bytes 1-4
		int TPIDefinition = 3;
		//Block 31 Bytes 5-8
		int TPICounterNum;
		//Block 32 Bytes 5-8
		int PhaseAngleSetting;
		//Block 34 (something for PMMIEV)
		//Block 36
		double ActualDH;
		//Block 37
		double ActualPhaseAngle;
		//Block 38
		double ActualWedgeAngle;
		//Block 40
		double LDPPAzimuth;
		//Block 41
		double LDPPHeightofPDI;
		//Block 42 1st word
		int LDPPDwellOrbits;
		//Block 42 2nd word
		bool LDPPPoweredDescentSimFlag;
		//Block 43
		double LDPPTimeofPDI;
		//Block 44
		double LDPPDescentFlightTime;
		//Block 45
		double LDPPDescentFlightArc;
		//Block 46
		double SPQDeltaH = 15.0*1852.0;
		//Block 47
		double SPQElevationAngle = 26.6*RAD;
		//Block 48
		double SPQTerminalPhaseAngle = 130.0*RAD;
		//Block 49
		double SPQMinimumPerifocus = 0.0;

	} GZGENCSN;

	struct TwoImpulseMultipleSolutionTableEntry
	{
		double Time1 = 0.0;
		double DELV1 = 0.0;
		double YAW1 = 0.0;
		double PITCH1 = 0.0;
		double Time2 = 0.0;
		double DELV2 = 0.0;
		double YAW2 = 0.0;
		double PITCH2 = 0.0;
		double T_TPI = 0.0;
		char L = ' ';
		int C = 0;
	};

	struct TwoImpulseMultipleSolutionTable
	{
		bool Updating = false;
		int Solutions = 0;
		bool showTPI = false;
		int IVFLAG = 0;
		int MAN_VEH = 0;
		std::string CSMSTAID, LMSTAID;
		TwoImpulseMultipleSolutionTableEntry data[13];
	} PZTIPREG;

	struct TwoImpulseMultipleSolutionDisplay
	{
		std::string ErrorMessage;
		std::string CSMSTAID;
		std::string LMSTAID;
		double GETTH_CSM = 0.0;
		double GETTH_LM = 0.0;
		std::string MAN_VEH;
		std::string GETFRZ;
		std::string GMTFRZ;
		std::string GETVAR;
		std::string OPTION;
		double WT = 0.0;
		double PHASE = 0.0;
		double DH = 0.0;
		double GET1 = 0.0;
		double GMT1 = 0.0;
		std::string MinutesUntil;
		int Solutions = 0;
		bool showTPI = false;
		TwoImpulseMultipleSolutionTableEntry data[13];
	} TwoImpMultDispBuffer;

	struct TwoImpulseSingleSolutionTable
	{
		std::string CSMSTAID, LMSTAID;
	} PZTIPSS;

	struct CorrectiveCombinationSolutionTable
	{
		bool Updating = false;
		int Solutions = 0;
		int MAN_VEH = 0;
	} PZTIPCCD;

	struct LaunchInterfaceTable
	{
		//Block 1-9
		MATRIX3 IU1_REFSMMAT;
		//Block 56-62
		double GMT_T;
		VECTOR3 R_T;
		VECTOR3 V_T;
		//Block 73
		double Azimuth;
	} GZLTRA;

	struct RTEDigitalSolutionTable
	{
		int ASTSolutionCode;
		std::string LandingSiteID;
		int ThrusterCode;
		int SpecifiedREFSMMAT;
		double VehicleWeightTIG;
		double TAAbort;
		int PrimReentryMode;
		VECTOR3 Att_LVLH;
		VECTOR3 Att_FDAI;
		double dV_C;
		double dt_BD;
		double dV;
		bool Quads;
		double dt_ullage;
		double IgnitionPET;
		double IgnitionGET;
		double IgnitionGMT;
		int BackupReentryMode;
		double RollInitiatePET;
		int LVOrientation;
		double EI_PET;
		double V_EI;
		double gamma_EI;
		double lat_EI;
		double lng_EI;
		double lat_splash;
		double lng_splash;
		double lat_tgt;
		double lng_tgt;

		double dt_10PCT;

		VECTOR3 InertialDV; //or Lambert target
		VECTOR3 UplinkDV; //or C and DT for Lambert
	};

	struct RTEConstraintsTable
	{
		RTEConstraintsTable();

		//Block 1 (?)
		RTEDigitalSolutionTable RTEPrimaryData;
		//Block 2 (?)
		RTEDigitalSolutionTable RTEManualData;

		//Block 11

		//Maximum allowable DV for tradeoff display (in ft/s)
		double DVMAX;
		//Minimum time of landing for tradeoff display (in hours GET)
		double TZMIN;
		//Maximum time of landing for tradeoff display (in hours GET)
		double TZMAX;
		//Constant g-level for use in return-to-earth digitals and EFCUA mode of the abort scan table (AST)
		double GMAX;
		//Minimum height of pericynthion (NM)
		double HMINMC;
		//Maximum return inclination (DEG)
		double IRMAX;
		//Relative range override (NM)
		double RRBIAS;
		//Maximum return velocity
		double VRMAX;
		//Vector source (0 = CSM, 1 = LEM)
		int VECID;
		//Vector type (0 = CMC, 1 = LGC, 2 = AGS, 3 = IU, 4 = HSR, 5 = DC, 6 = ANC, 7 = EPH)
		int VECTYPE;
		//Target line (0 = shallow, 1 = steep)
		int TGTLN;
		//Circumlunar motion (0 = either, 1 = direct, 2 = circumlunar)
		int MOTION;

		//Names of PTP sites
		std::string PTPSite[5];
		//Latitude of PTP sites
		double PTPLatitude[5];
		//Longitude of PTP sites
		double PTPLongitude[5];
		//Names of ATP sites
		std::string ATPSite[5];
		//Latitude and longitude of ATP data points
		double ATPCoordinates[5][10];

		//Block 12
		std::string RTESite;
		double RTEVectorTime;
		double RTET0Min;
		double RTET0Max;
		double RTETimeOfLanding;
		double RTEUADVMax;
		double RTEPTPMissDistance;
		double RTEInclination;
		int EntryProfile;
		int RTETradeoffRemotePage;
		int RTESiteNum;
		bool RTEIsPTPSite;
		int RTEDigitalsColumn;
		int RTEDTrimAngleIndicator;
		int RTEDDockingAngle;
		int RTEDManeuverOrientation;
		bool RTEDIterationFlag;
		char RTEManeuverCode[4];

		//Block 13
		int RTETradeoffLabelling[5];
	} PZREAP;

	struct RetrofireTransferTable
	{
		double GMTI_Primary;
		VECTOR3 DeltaV_Primary;
		double GMTI_Manual;
		VECTOR3 DeltaV_Manual;
	} RZRFTT;

	struct LMLaunchTargetTable
	{
		//K13
		double PoweredFlightArc = 10.0*RAD;
		double PoweredFlightTime = 7.0*60.0 + 15.0;
		double InsertionHeight = 60000.0*0.3048;
		double InsertionHorizontalVelocity = 5535.6*0.3048;
		double InsertionRadialVelocity = 32.0*0.3048;
		double YawSteerCap = 0.5*RAD;
		double MaxAscLifetime = 9.0*3600.0;
		double MinSafeHeight = 5.0*1852.0;
		double LMMaxDeltaV = 430.0*0.3048;
		double CSMMaxDeltaV = 430.0*0.3048;
		//K14
		//If CSI scheduled at apsis and CSM is chaser, then CSI will be done at LM apsis plus input time bias
		double dt_bias = 0.0;
		double ElevationAngle = 26.6*RAD;
		double TerminalPhaseTravelAngle = 130.0*RAD;
		double TPF_Height_Offset = 0.0;
		double TPF_Phase_Offset = 0.0;
		double Height_Diff_Begin = 20.0*1852.0;
		double Height_Diff_Incr = 5.0*1852.0;

		//Short profile
		double DT_DH = 15.0*1852.0;
		double DT_Theta_i = 1.69*RAD;
		double DT_Ins_TPI;			//Fixed time from insertion to TPI
		double DT_Ins_TPI_NOM;		//Nominal time from insertion to TPI
	} PZLTRT;

	struct LAIInputOutput
	{
		double t_launch;
		double R_D, Y_D;
		double R_D_dot, Y_D_dot, Z_D_dot;
		MPTSV sv_CSM;
		MPTSV sv_Insertion;
	} JZLAI;

	struct AEGBlockSaveTable
	{
		//Block 1: Multiple Solution
		EphemerisData SV_mult[2];
		//Block 2: Corrective Combination
		EphemerisData SV_CC[2];
		//Block 3: Transfer Data
		EphemerisData SV_before[2];
		VECTOR3 V_after[2];
		int plan[2];
		std::string code[2];
	} PZMYSAVE;

	struct GPMElementsTable
	{
		EphemerisData SV_before;
		VECTOR3 V_after;
		std::string code;
	} PZGPMELM;

	struct DKIElementsBlock
	{
		EphemerisData SV_before[5];
		VECTOR3 V_after[5];
	};

	struct DKIElementsTable
	{
		DKIElementsBlock Block[7];
	} PZDKIELM;

	struct DKIDisplayBlock
	{
		double ManGMT = 0.0;
		double dt = 0.0;
		double dv = 0.0;
		//Vehicle performing the maneuver
		int VEH;
		//Name of the maneuver
		std::string Man_ID;
		double PhaseAngle = 0.0;
		double DH = 0.0;
		double HA = 0.0;
		double HP = 0.0;
		double ManLine = 0.0;
		VECTOR3 DV_LVLH = _V(0, 0, 0);
		double Pitch = 0.0;
		double Yaw = 0.0;
	};

	struct DKIDataBlock
	{
		int Plan_M = 0;
		DKIDisplayBlock Display[5];
		int NumMan = 0;
		//0 = No plan, 1 = DKI, 2 = SPQ
		int PlanStatus = 0;
	};

	struct DKIDataTable
	{
		//Block 1
		bool UpdatingIndicator = false;
		//Number of plans that were generated (SPQ always 1)
		int NumSolutions = 0;
		//Block 2 (Information Block)
		DKIDataBlock Block[7];
	} PZDKIT;

	struct RendezvousEvaluationDisplay
	{
		RendezvousEvaluationDisplay();
		int ID;
		int M;
		int NumMans;
		double GET[5];
		double DT[4];
		double DV[5];
		std::string VEH[5];
		std::string PURP[5];
		double CODE[5];
		double PHASE[5];
		double HEIGHT[5];
		double HA[5];
		double HP[5];
		double Pitch[5], Yaw[5];
		VECTOR3 DVVector[5];
		bool isDKI;
		std::string ErrorMessage;
	} PZREDT;

	struct RendezvousPlanningDisplayData
	{
		int ID = 0;
		int N = 0;
		double GETLO = 0.0;
		double DH = 0.0;
		double T_INS = 0.0;
		double T_CSI = 0.0;
		double T_CDH = 0.0;
		double T_TPI = 0.0;
		double T_TPF = 0.0;
		double DVCSI = 0.0;
		double DVCDH = 0.0;
		double DVTPI = 0.0;
		double DVTPF = 0.0;
		double DVT = 0.0;
	};

	struct RendezvousPlanningDisplay
	{
		std::string CSMSTAID;
		double CSM_GMTV = 0.0;
		double CSM_GETV = 0.0;
		std::string ManVeh;
		double LSLat = 0.0;
		double LSLng = 0.0;
		double THT = 0.0;
		std::string LMSTAID;
		double LM_GMTV = 0.0;
		double LM_GETV = 0.0;
		double DT_CSI = 0.0;
		double DV_MAX = 0.0;
		double MINH = 0.0;
		double WT = 0.0;
		int plans = 0;
		std::string ErrorMessage;
		RendezvousPlanningDisplayData data[7];
	} PZLRPT;

	struct SkeletonFlightPlanTable
	{
		//Blocks
		//1: Preflight
		//2: Nominal nodal targets from options 2-5
		//3-5: TBD
		TLMCCDataTable blocks[2];
		//Block 6
		int DisplayBlockNum = 1;
	} PZSFPTAB;

	struct UpdatedSkeletonFlightPlansTable
	{
		//One for each MCC tradeoff display columns
		TLMCCDataTable blocks[6];
	} PZMCCSFP;

	struct TTLMCCPlanningDisplay
	{
		TLMCCDisplayData data[6];
	} PZMCCDIS;

	struct MidcourseCorrectionPlanTable
	{
		//Block 1
		double VectorGET = 0.0;
		int Column = 1;
		int Mode = 1;
		double MidcourseGET = 0.0;
		bool Config = true; //false = undocked, true = docked
		int SFPBlockNum = 1;
		double h_PC = 60.0*1852.0;
		double h_PC_mode5 = -1.0;
		double incl_fr = 0.0;
		double AZ_min = -110.0*RAD;
		double AZ_max = -70.0*RAD;
		//Minimum and maximum time at pericynthion (GET, hours)
		double TLMIN = 0.0;
		double TLMAX = 0.0;
		double gamma_reentry = -6.52*RAD;
		double Reentry_range = 1350.0;
		double H_PCYN_MIN = 40.0*1852.0;
		double H_PCYN_MAX = 5000.0*1852.0;
		double LATBIAS = 2.0*RAD;
		double H_A_LPO1 = 170.0*1852.0;
		double H_P_LPO1 = 60.0*1852.0;
		double H_A_LPO2 = 60.0*1852.0;
		double H_P_LPO2 = 8.23*1852.0;
		double REVS1 = 2.0;
		int REVS2 = 11;
		double SITEROT = -15.0*RAD;
		double ETA1 = 0.0;
		//Constants (some of these should be RTCC program constants, but we don't know their name)
		int LOPC_M = 3;
		int LOPC_N = 8;
		double TT1_DPS_MIN = 1.0*3600.0;
		double TT1_DPS_MAX = 120.0*3600.0;
		//Max inclination of powered return
		double INCL_PR_MAX = 40.0*RAD;
	} PZMCCPLN;

	struct UMEDSaveTable
	{
		//Block 3
		int SpaceDigVehID = -1;
		int SpaceDigCentralBody = -1;
		//Block 17
		int LandmarkVehID = 0;
		double LandmarkGMT = 0.0;
		double LandmarkDT = 0.0;
		int LandmarkRef = 0;

		//DMT
		int DMT1Vehicle = 0;
		int DMT1Number = 0;
		int DMT1REFSMMATCode = 0;
		bool DMT1HeadsUpDownIndicator = false;
		int DMT2Vehicle = 0;
		int DMT2Number = 0;
		int DMT2REFSMMATCode = 0;
		bool DMT2HeadsUpDownIndicator = false;

		//RET
		int RETPlan = 1;
	} EZETVMED;

	struct GMEDSaveTable
	{
		double GMT = 0.0;
		unsigned StartingStar = 0;
		int MTX1 = -1, MTX2 = -1, MTX3 = -1;
		int G14_Vehicle = 1;
		unsigned G14_Star = 0;
		int G14_RB = 0;
		double G14_lat = 0.0, G14_lng = 0.0, G14_height = 0.0;
		double G14_GMT = 0.0;
	} EZGSTMED;

	struct ExternalDVMakeupBuffer
	{
		int LoadNumber = 0;
		std::string Site1, Site2;
		double GenGET = 0.0;
		int Verb = 71;
		int CMCCoreAddress = 0;
		int Octals[012] = { 0,0,0,0,0,0,0,0,0,0 };
		int VehID = 0;
		double GET = 0.0;
		VECTOR3 DV = _V(0, 0, 0);
		std::string ManeuverCode;
		std::string GMTID;
		std::string StationID;
	} CZAXTRDV, CZLXTRDV;

	struct REFSMMATUpdateMakeupTableBlock
	{
		int UpdateNo = 0;
		int SequenceNo = 0;
		std::string Site1, Site2;
		double GET = 0.0;
		int Verb = 71;
		int Octals[20] = { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
		int VehID = 0;
		MATRIX3 REFSMMAT = _M(0, 0, 0, 0, 0, 0, 0, 0, 0);
		std::string error;
		//1 = Actual REFSMMAT; 2 = Desired REFSMMAT
		int MatrixType = 2;
		std::string MatrixID;
	};

	struct REFSMMATUpdateMakeupTable
	{
		REFSMMATUpdateMakeupTableBlock Block[2];
	} CZREFMAT;

	struct SLVNavigationMakeupTable
	{
		VECTOR3 PosS = _V(0, 0, 0);
		VECTOR3 DotS = _V(0, 0, 0);
		double NUPTIM = 0.0;
	} CZNAVSLV;

	struct FIDOLaunchAnalogNo1DisplayTable
	{
		double LastUpdateTime = -1.0;
		std::vector<double> XVal;
		std::vector<double> YVal;
	} fdolaunchanalog1tab;

	struct FIDOLaunchAnalogNo2DisplayTable
	{
		double LastUpdateTime = -1.0;
		std::vector<double> XVal;
		std::vector<double> YVal;
	} fdolaunchanalog2tab;

	struct MPTManDisplayBuffer
	{
		std::string CSMSTAID;
		std::string LEMSTAID;
		std::string CSMGETAV;
		std::string LEMGETAV;
		std::vector<MPTManDisplay> man;
	} MPTDISPLAY;

	CheckoutMonitor EZCHECKDIS;

	struct VehicleNameTable
	{
		VehicleNameTable() {
			tab.push_back("CSM"); tab.push_back("LEM");
		}

		std::vector<std::string> tab;
	} MHGVNM;

	//System parameters for PDI
	LGCDescentConstants RTCCDescentTargets;
	LGCIgnitionConstants RTCCPDIIgnitionTargets;

	//RTCC MED Buffer
	char RTCCMEDBUFFER[256];
	//RTCC On-line Monitor Buffer
	struct OnlineMonitorMessage
	{
		std::vector<std::string> message;
	};

	struct OnlineMonitorBuffer
	{
		std::deque<OnlineMonitorMessage> data;
		std::string TextBuffer[3];
		double DoubleBuffer[4];
		int IntBuffer[2];
		VECTOR3 VectorBuffer[2];
		MATRIX3 MatrixBuffer;
	} RTCCONLINEMON;

	//AEG
	PMMAEG pmmaeg;
	PMMLAEG pmmlaeg;

private:
	void AP7ManeuverPAD(AP7ManPADOpt *opt, AP7MNV &pad);
	MATRIX3 GetREFSMMATfromAGC(agc_t *agc, double AGCEpoch, int addroff = 0);
	double GetClockTimeFromAGC(agc_t *agc);
	double GetTEPHEMFromAGC(agc_t *agc);
	void navcheck(VECTOR3 R, VECTOR3 V, double MJD, OBJHANDLE gravref, double &lat, double &lng, double &alt);
	double getGETBase();
	void AP7BlockData(AP7BLKOpt *opt, AP7BLK &pad);
	void AP11BlockData(AP11BLKOpt *opt, P37PAD &pad);
	void CMCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH);
	void LGCExternalDeltaVUpdate(char *str, double P30TIG, VECTOR3 dV_LVLH);
	void LandingSiteUplink(char *str, double lat, double lng, double alt, int RLSAddr);
	void AGCStateVectorUpdate(char *str, SV sv, bool csm, double AGCEpoch, double GETbase, bool v66 = false);
	void AGCDesiredREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, bool cmc = true, bool AGCCoordSystem = false);
	void AGCREFSMMATUpdate(char *list, MATRIX3 REFSMMAT, double AGCEpoch, int offset = 0, bool AGCCoordSystem = false);
	void CMCRetrofireExternalDeltaVUpdate(char *list, double LatSPL, double LngSPL, double P30TIG, VECTOR3 dV_LVLH);
	void CMCEntryUpdate(char *list, double LatSPL, double LngSPL);
	void IncrementAGCTime(char *list, double dt);
	void TLANDUpdate(char *list, double t_land, int tlandaddr);
	void V71Update(char *list, int* emem, int n);
	void V72Update(char *list, int *emem, int n);
	void SunburstAttitudeManeuver(char *list, VECTOR3 imuangles);
	void SunburstLMPCommand(char *list, int code);
	void SunburstMassUpdate(char *list, double masskg);
	void P27PADCalc(P27Opt *opt, double AGCEpoch, P27PAD &pad);
	int SPSRCSDecision(double a, VECTOR3 dV_LVLH);	//0 = SPS, 1 = RCS
	bool REFSMMATDecision(VECTOR3 Att); //true = everything ok, false = Preferred REFSMMAT necessary
	double PericynthionTime(VESSEL* vessel);
	SV FindPericynthion(SV sv0);
	double FindOrbitalMidnight(SV sv, double GETbase, double t_TPI_guess);
	double FindOrbitalSunrise(SV sv, double GETbase, double t_sunrise_guess);
	void FindRadarAOSLOS(SV sv, double GETbase, double lat, double lng, double &GET_AOS, double &GET_LOS);
	void FindRadarMidPass(SV sv, double GETbase, double lat, double lng, double &GET_Mid);
	double GetSemiMajorAxis(SV sv);
	void papiWriteScenario_SV(FILEHANDLE scn, char *item, SV sv);
	bool papiReadScenario_SV(char *line, char *item, SV &sv);
	void DMissionRendezvousPlan(SV sv_A0, double GETbase, double &t_TPI0);
	void FMissionRendezvousPlan(VESSEL *chaser, VESSEL *target, SV sv_A0, double GETbase, double t_TIG, double t_TPI, double &t_Ins, double &CSI);

	bool CalculationMTP_B(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_C_PRIME(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_D(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_F(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);
	bool CalculationMTP_G(int fcn, LPVOID &pad, char * upString = NULL, char * upDesc = NULL, char * upMessage = NULL);

	//Generalized Contact Generator
	void EMGENGEN(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, const StationTable &stationlist, int body, OrbitStationContactsTable &res);
	//Horizon Crossing Subprogram
	bool EMXING(EphemerisDataTable &ephemeris, ManeuverTimesTable &MANTIMES, const Station & station, int body, std::vector<StationContact> &acquisitions);
	//Variable Order Interpolation
	int ELVARY(EphemerisDataTable &EPH, unsigned ORER, double GMT, bool EXTRAP, EphemerisData &sv_out, unsigned &ORER_out);
	//Extended Interpolation Routine
	void ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable &out);
	void ELVCTR(const ELVCTRInputTable &in, ELVCTROutputTable &out, EphemerisDataTable &EPH, ManeuverTimesTable &mantimes, LunarStayTimesTable *LUNRSTAY = NULL);
	int CapeCrossingRev(int L, double GMT);
	double CapeCrossingGMT(int L, int rev);
	void ECMPAY(EphemerisDataTable &EPH, ManeuverTimesTable &MANTIMES, double GMT, bool sun, double &Pitch, double &Yaw);
	//Spherical to Inertial Conversion
	int EMMXTR(double vel, double fpa, double azi, double lat, double lng, double h, VECTOR3 &R, VECTOR3 &V);
	//Base hour Angle
	double PIGMHA(int E, int Y, int D);
	//Delta True Anomaly Function
	double PCDETA(double beta1, double beta2, double r1, double r2);
	//Orbit Desired REFSMMAT Computation Subroutine
	MATRIX3 PIDREF(VECTOR3 AT, VECTOR3 R, VECTOR3 V, double PG, double YG, bool K);
	//Universal Cartesian to Kepler Coordinates
	void PIMCKC(VECTOR3 R, VECTOR3 V, int body, double &a, double &e, double &i, double &l, double &g, double &h);
	//Impulse Analytical Burn
	void PIBURN(VECTOR3 R, VECTOR3 V, double T, double *B, VECTOR3 &ROUT, VECTOR3 &VOUT, double &TOUT);
	//Coordinate transformation
	void PICSSC(bool vecinp, VECTOR3 &R, VECTOR3 &V, double &r, double &v, double &lat, double &lng, double &gamma, double &azi);
	//Apogee/perigee magnitude determination
	void PIFAAP(double a, double e, double i, double f, double u, double r, double &r_apo, double &r_peri);
	//PMMMPT Begin Burn Time Computation Subroutine
	void PCBBT(double *DELT, double *WDI, double *TU, double W, double TIMP, double DELV, int NPHASE, double &T, double &GMTBB, double &GMTI, double &WA);
	//PMMMPT Matrix Utility Subroutine
	void PCMATO(double **A, double *Y, double **B, double *X, int M, int N, double *W1, double lambda, double *W2);
	//PMMMPT Gaussian Elimination Subroutine
	bool PCGAUS(double **A, double *Y, double *X, int N, double eps);
	//RTE Abort Scan Table (AST) Subroutine
	void PMMDAB(VECTOR3 R, VECTOR3 V, double GMT, ASTInput ARIN, ASTSettings IRIN, ASTData &AST, int &IER, int IPRT);
	//Return-to-Earth Tradeoff Display
	void PMDTRDFF(int med, unsigned page);
	//Mission Plan Table Display
	void PMDMPT();
	//Rendezvous Evaluation Display Load Module
	void PMDRET();
	//Rendezvous Planning Table
	void PMDRPT();
	//Two-Impulse Multiple Solution Display
	void PMDTIMP();
	//GOST REFSMMAT Maintenance
	void FormatREFSMMATCode(int ID, int num, char *buff);
	//GOST CSM/LM LCV Computation
	void EMMGLCVP(int L, double gmt);
	//Trajectory Update On-line Print
	void EMGPRINT(std::string source, int i);
	void EMGPRINT(std::string source, std::vector<std::string> message);
	//Orbital Elements Computations
	void EMMDYNEL(EphemerisData sv, TimeConstraintsTable &tab);
	//Relative Motion Digital Display
	void EMMRMD(int Veh1, int Veh2, double get, double dt, int refs, int axis, int mode, VECTOR3 Att = _V(0, 0, 0), double PYRGET = 0.0);
	//Ground Point Characteristics Block Routine
	void EMGGPCHR(double lat, double lng, double alt, int body, Station *stat);

	//MPT utility functions
	bool MPTConfigSubset(const std::bitset<4> &CfgOld, const std::bitset<4> &CfgNew);
	bool MPTIsRCSThruster(int thruster);
	bool MPTIsPrimaryThruster(int thruster, int i);
	bool MPTIsUllageThruster(int thruster, int i);
	int MPTGetPrimaryThruster(int thruster);
	void MPTGetConfigFromString(const std::string &str, std::bitset<4> &cfg);

	//Auxiliary subroutines
	MissionPlanTable *GetMPTPointer(int L);
	MPTSV SVfromRVGMT(VECTOR3 R, VECTOR3 V, double GMT, int body);
	int PMMXFRGroundRules(MissionPlanTable * mpt, double GMTI, unsigned ReplaceMan, bool &LastManReplaceFlag, double &LowerLimit, double &UpperLimit, unsigned &CurMan, double &VectorFetchTime);
	int PMMXFRFormatManeuverCode(int Table, int Thruster, int Attitude, unsigned Maneuver, std::string ID, int &TVC, std::string &code);
	int PMMXFRCheckConfigThruster(bool CheckConfig, int CCI, const std::bitset<4> &CCP, int TVC, int Thruster, std::bitset<4> &CC, std::bitset<4> &CCMI);
	int PMMXFRFetchVector(double GMTI, int L, EphemerisData &sv);
	int PMMXFRFetchAnchorVector(int L, EphemerisData &sv, bool &landed);
	void PMMXFRWeightAtInitiation(int CCI, int CCMI, double &weight);
	bool PMMXFRDeleteOption(int L, double GMTI);
	int PMMMCDCallEMSMISS(EphemerisData sv0, double GMTI, EphemerisData &sv1);
	int PMSVCTAuxVectorFetch(int L, double T_F, EphemerisData &sv);
	bool MEDTimeInputHHMMSS(std::string vec, double &hours);

	double GLHTCS(double FLTHRS) { return FLTHRS * 360000.0; }
	double GLCSTH(double FIXCSC) { return FIXCSC / 360000.0; }
	double TJUDAT(int Y, int M, int D);
	EphemerisData ConvertSVtoEphemData(SV sv);
	SV ConvertEphemDatatoSV(EphemerisData sv);

protected:
	double TimeofIgnition;
	double SplashLatitude, SplashLongitude;
	VECTOR3 DeltaV_LVLH;
	int REFSMMATType;
	OBJHANDLE hEarth, hMoon;
	std::ofstream rtccdebug;

	//RTCC System Parameters

	//Radians per degree
	double MCCRPD = RAD;
	//Burnout launch azimuth (rad.)
	double MCLABN;
	//Sine of burnout launch azimuth
	double MCLSBN;
	//Cosine of burnout launch azimuth
	double MCLCBN;
	//Pitch angle from horizon (rad.)
	double MCGHZA;
	//L/O time first vehicle (hrs.)
	double MCGMTL;
	//L/O time second vehicle (hrs.)
	double MCGMTS;
	//L/O time first vehicle (centisec.)
	double MGLGMT;
	//L/O time second vehicle (centisec.)
	double MGGGMT;
	//CSM GMTGRR (hrs.)
	double MCGRAG;
	//IU1 GMTGRR (hrs.)
	double MCGRIC;
	//IU2 GMTGRR (hrs.)
	double MCGRIL;
	//AGC GMTZS (hrs.)
	double MCGZSA;
	//LGC GMTZS (hrs.)
	double MCGZSL;
	//AGS GMTZS (hrs.)
	double MCGZSS;
	//Semimajor axis for integration limit (Er.)
	double MCGSMA;
	//Eccentricity for integration limit
	double MCGECC;
	//Phase reference time - GET (hrs.)
	double MCGREF;
	//Venting scale factor
	double MCTVEN;
	//Lambda Zero
	double MCLAMD;
	//P80 first launch vehicle
	std::string MCGPRM;
	//Geodetic Earth constant B²/A²
	double MCEBAS;
	//Geodetic Earth constant A²
	double MCEASQ;
	//Geodetic Earth constant B²
	double MCEBSQ;

	//MJD of launch day (days)
	double GMTBASE;

	//CONSTANTS
	//Nautical miles per Earth radii
	const double MCCNMC = 3443.93359;

public:
	//MJD of epoch
	double AGCEpoch;
	//Radius of lunar landing site
	double MCSMLR;
	//Sine of the geodetic latitude of the launch pad
	double MCLSDA;
	//Cosine of the geodetic latitude of the launch pad
	double MCLCDA;
	//Longitude of launch pad
	double MCLGRA;
	//Nominal LM cross-product steering constant
	double MCVLMC;
	//Nominal CSM cross-product steering constant
	double MCVCMC;
	//Thrust of CSM RCS+X (2 quads)
	double MCTCT1;
	//Thrust of CSM RCS+X (4 quads)
	double MCTCT2;
	//Thrust of CSM RCS-X (2 quads)
	double MCTCT3;
	//Thrust of CSM RCS-X (4 quads)
	double MCTCT4;
	//On-board computer thrust level for CSM RCS+/-X thruster (2 quads)
	double MCTCT5;
	//On-board computer thrust level for CSM RCS+/-X thruster (4 quads)
	double MCTCT6;
	//Weight loss rate CSM RCS+X (2 quads)
	double MCTCW1;
	//Weight loss rate CSM RCS+X (4 quads)
	double MCTCW2;
	//Weight loss rate CSM RCS-X (2 quads)
	double MCTCW3;
	//Weight loss rate CSM RCS-X (4 quads)
	double MCTCW4;
	//Thrust of LM RCS+X (2 quads)
	double MCTLT1;
	//Thrust of LM RCS+X (4 quads)
	double MCTLT2;
	//Thrust of LM RCS-X (2 quads)
	double MCTLT3;
	//Thrust of LM RCS-X (4 quads)
	double MCTLT4;
	//On-board computer thrust level for LM RCS+/-X thruster (2 quads)
	double MCTLT5;
	//On-board computer thrust level for LM RCS+/-X thruster (4 quads)
	double MCTLT6;
	//Weight loss rate LM RCS+X (2 quads)
	double MCTLW1;
	//Weight loss rate LM RCS+X (4 quads)
	double MCTLW2;
	//Weight loss rate LM RCS-X (2 quads)
	double MCTLW3;
	//Weight loss rate LM RCS-X (4 quads)
	double MCTLW4;
	//APS thrust level
	double MCTAT1;
	//APS buildup thrust level
	double MCTAT2;
	//APS full load thrust level
	double MCTAT4;
	//On-board computer thrust level for LM APS thruster
	double MCTAT9;
	//APS weight loss rate
	double MCTAW1;
	//APS weight loss rate for buildup
	double MCTAW2;
	//APS weight loss rate for full load
	double MCTAW4;
	//APS phase two burn time
	double MCTAD2;
	//APS phase three burn time
	double MCTAD3;
	//Total APS ullage overlap
	double MCTAD9;
	//DPS thrust level
	double MCTDT1;
	//DPS phase 2 thrust level
	double MCTDT2;
	//DPS phase 3 thrust level
	double MCTDT3;
	//DPS 10% thrust level
	double MCTDT4;
	//DPS thrust for buildup from 10% to maximum
	double MCTDT5;
	//DPS full load thrust level
	double MCTDT6;
	//On-board computer thrust level for LM DPS thruster
	double MCTDT9;
	//DPS weight loss rate
	double MCTDW1;
	//DPS phase 2 weight loss rate
	double MCTDW2;
	//DPS phase 3 weight loss rate
	double MCTDW3;
	//DPS 10% weight loss rate
	double MCTDW4;
	//DPS weight loss rate for build from 10% to max.
	double MCTDW5;
	//DPS full load weight loss rate
	double MCTDW6;
	//DPS phase 2 burn time
	double MCTDD2;
	//DPS phase 3 burn time
	double MCTDD3;
	//Nominal dt of 10% DPS thrust
	double MCTDD4;
	//DPS burn time for building from 10% to max
	double MCTDD5;
	//DPS burn time used to determine if 20% thrust level is to be maintained throughout the burn
	double MCTDD6;
	//Total DPS ullage overlap
	double MCTDD9;
	//SPS thrust level
	double MCTST1;
	//SPS thrust level for buildup
	double MCTST2;
	//SPS thrust level for full load
	double MCTST4;
	//On-board computer thrust level for CSM SPS thruster
	double MCTST9;
	//SPS weight loss rate
	double MCTSW1;
	//SPS weight loss rate for buildup
	double MCTSW2;
	//SPS weight loss rate for full load
	double MCTSW4;
	//SPS phase two burn time
	double MCTSD2;
	//SPS phase three burn time
	double MCTSD3;
	//Total SPS ullage overlap
	double MCTSD9;

	//Impulse in one second SPS / APS / DPS burn
	double MCTAK1;
	//Initial value of SPS/APS/DPS minimum impulse curve
	double MCTAK2;
	//Slope of SPS/APS/DPS curve
	double MCTAK3;
	//LM RCS impulse due to 7 second, 2 jet ullage
	double MCTAK4;

	//SPS pitch electronic null parameter
	double MCTSPP;
	//SPS yaw electronic null parameter
	double MCTSYP;
	//SPS pitch gimbal system parameter
	double MCTSPG;
	//SPS yaw gimbal system parameter
	double MCTSYG;
	//APS pitch engine cant system parameter
	double MCTAPG;
	//APS roll engine cant system parameter
	double MCTARG;
	//DPS pitch gimbal system parameter
	double MCTDPG;
	//DPS roll gimbal system parameter
	double MCTDRG;
	//Nominal scaling factor for main DPS thrust level and weight loss rate
	double MCTDTF;

	//Phase 1 burn time for S-IVB TLI maneuver (vent and ullage)
	double MCTJD1;
	//Phase 3 burn time for S-IVB TLI maneuver (buildup)
	double MCTJD3;
	//Phase 4 burn time for S-IVB TLI maneuver (main burn)
	double MCTJD4;
	//Phase 5 burn time for S-IVB TLI maneuver (during MRS)
	double MCTJDS;
	//Phase 6 burn time for S-IVB TLI maneuver (main burn after MRS)
	double MCTJD5;
	//Phase 7 burn time for S-IVB TLI maneuver (tailoff)
	double MCTJD6;
	//Thrust level of first S-IVB thrust phase (vent and ullage)
	double MCTJT1;
	//Thrust level of second S-IVB thrust phase (chilldown)
	double MCTJT2;
	//Thrust level of third S-IVB thrust phase (buildup)
	double MCTJT3;
	//Thrust level of third S-IVB thrust phase (pre MRS)
	double MCTJT4;
	//Thrust level of fifth S-IVB thrust phase (high)
	double MCTJTH;
	//Thrust level of fifth S-IVB thrust phase (low)
	double MCTJTL;
	//Thrust level of sixth S-IVB thrust phase (main burn after MRS)
	double MCTJT5;
	//Thrust level of sevent S-IVB thrust phase (tailoff)
	double MCTJT6;
	//Weight flow rate for first S-IVB thrust phase
	double MCTJW1;
	//Weight flow rate for second S-IVB thrust phase
	double MCTJW2;
	//Weight flow rate for third S-IVB thrust phase
	double MCTJW3;
	//Weight flow rate for fourth S-IVB thrust phase
	double MCTJW4;
	//Weight flow rate for fifth S-IVB thrust phase (high)
	double MCTJWH;
	//Weight flow rate for fifth S-IVB thrust phase (low)
	double MCTJWL;
	//Weight flow rate for sixth S-IVB thrust phase
	double MCTJW5;
	//Weight flow rate for seventh S-IVB thrust phase
	double MCTJW6;
	//Time from ignition to start IGM guidance
	double MCVIGM;
	//Minimum allowable S-IVB weight
	double MCVWMN;
	//Fixed time increment to force MRS to occur
	double MCVKPC;
	//Duration of artificial tau for beginning of burn
	double MCVCPQ;
	//Epsilon to control range angle computation
	double MCVEP1;
	//Epsilon to terminate computation of K1, K2, K3, and K4
	double MCVEP2;
	//Epsilon to terminate recomputation of terminal values
	double MCVEP3;
	//Epsilon to allow cut-off computations to be sensed
	double MCVEP4;
	//Maximum allowable total yaw angle (IGM)
	double MCVYMX;
	//Maximum pitch rate (IGM)
	double MCVPDL;
	//Maximum yaw rate (IGM)
	double MCVYDL;
	//Time-to-go reference velocity increment
	double MCVTGQ;
	//IGM range angle constant
	double MCVRQV;
	//Terminal valus rotation indicator
	double MCVRQT;
	//High or low thrust indicator
	int MCTIND;
	//Second stage exhaust velocity
	double MCVVX3;
	//Second stage flow rate
	double MCVWD3;
	//Transition time for mixture ratio shift (MRS)
	double MCVTB2;
	//S-IVB thrust level
	double MCTSAV;
	//S-IVB weight loss rate
	double MCTWAV;

	//Polynomial coefficients for insertion conditions
	double MDLIEV[16];
	//Earth orbit insertion constants
	double MDLEIC[3];

	//CMC address for external DV uplink
	int MCCCEX;
	//LGC address for external DV uplink
	int MCCLEX;
	//CMC address for REFSMMAT uplink (and downlink)
	int MCCCRF, MCCCRF_DL;
	//CMC address for desired REFSMMAT uplink
	int MCCCXS;
	//LGC address for REFSMMAT uplink (and downlink)
	int MCCLRF, MCCLRF_DL;
	//LGC address for desired REFSMMAT uplink
	int MCCLXS;
	//Suppress C-band station contacts generation (0 = suppressed, 1 = unsuppressed)
	int MGRTAG;

	struct CGTable
	{
		double Weight[40];
		VECTOR3 CG[40];
		int N;
	};

	//CG table of LM with descent stage
	CGTable LMDSCCGTAB;
};

#endif