#ifndef __ARCORE_H
#define __ARCORE_H

#include "Orbitersdk.h"
#include "MFDButtonPage.hpp"
#include "ApollomfdButtons.h"
#include "OrbMech.h"
#include "EntryCalculations.h"
#include "soundlib.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "saturn.h"
#include "mcc.h"
#include "rtcc.h"
#include <queue>

struct ApolloRTCCMFDData {  // global data storage
	int connStatus;
	int emem[24];
	int uplinkState;
	int uplinkLEM;
	IMFD_BURN_DATA burnData;
	std::queue<unsigned char> uplinkBuffer;
	double uplinkBufferSimt;
	bool isRequesting;
	Saturn *progVessel;
};

class AR_GCore
{
public:
	AR_GCore(VESSEL* v);
	~AR_GCore();

	void SetMissionSpecificParameters();
	void MPTMassUpdate();
	int MPTTrajectoryUpdate();

	bool MissionPlanningActive;
	int mission;				//0=manual, 7 = Apollo 7, 8 = Apollo 8, 9 = Apollo 9, etc.
	double t_Land;				//Time of landing

	VESSEL *pCSM;
	VESSEL *pLM;

	int pCSMnumber;
	int pLMnumber;
	int mptInitError;

	RTCC* rtcc;

	LunarDescentPlanningTable descplantable;
};

class ARCore {
public:
	ARCore(VESSEL* v, AR_GCore* gcin);
	~ARCore();
	void lambertcalc();
	void SPQcalc();
	void GPMPCalc();
	void REFSMMATCalc();
	void SkylabCalc();
	void LunarLaunchTargetingCalc();
	void LDPPalc();
	void LunarLiftoffCalc();
	void LOICalc();
	void LmkCalc();
	void MoonRTECalc();
	void EntryCalc();
	void DeorbitCalc();
	void TLCCCalc();
	void EntryUpdateCalc();
	void StateVectorCalc();
	void AGSStateVectorCalc();
	void LandingSiteUpdate();
	void LSUplinkCalc();
	void LandingSiteUplink();
	void VecPointCalc();
	void TerrainModelCalc();
	void DKICalc();
	void LAPCalc();
	void DAPPADCalc();
	void AscentPADCalc();
	void PDAPCalc();
	void CycleFIDOOrbitDigitals1();
	void CycleFIDOOrbitDigitals2();
	void CycleSpaceDigitals();
	void SpaceDigitalsMSKRequest();
	void CycleNextStationContactsDisplay();
	void RTETradeoffDisplayCalc();
	void GeneralMEDRequest();
	void TransferTIToMPT();
	void TransferSPQToMPT();
	void TransferDKIToMPT();
	void TransferDescentPlanToMPT();
	void TransferPoweredDescentToMPT();
	void TransferPoweredAscentToMPT();
	void TransferGPMToMPT();
	void MPTDirectInputCalc();
	void MPTTLIDirectInput();
	void TransferLOIorMCCtoMPT();
	void TransferRTEToMPT();
	void SLVNavigationUpdateCalc();
	void SLVNavigationUpdateUplink();
	bool vesselinLOS();
	void MinorCycle(double SimT, double SimDT, double mjd);

	void UplinkData();
	void UplinkData2();
	void send_agc_key(char key);
	void uplink_word(char *data);
	void P30UplinkCalc();
	void P30Uplink();
	void RetrofireEXDVUplinkCalc();
	void RetrofireEXDVUplink();
	void EntryUpdateUplink(void);
	void REFSMMATUplink(void);
	void StateVectorUplink();
	void TLANDUplinkCalc(void);
	void TLANDUplink(void);
	void EMPP99Uplink(int i);
	void ManeuverPAD();
	void EntryPAD();
	void TPIPAD();
	void TLI_PAD();
	void PDI_PAD();
	void MapUpdate();
	void NavCheckPAD();
	void AP11AbortCoefUplink();
	void AP12AbortCoefUplink();
	void DetermineGMPCode();
	void NodeConvCalc();
	void SendNodeToSFP();
	void CalculateTPITime();
	void GetStateVectorFromAGC(bool csm);
	void VectorCompareDisplayCalc();

	int startSubthread(int fcn);
	int subThread();
	void StartIMFDRequest();
	void StopIMFDRequest();

	//EPHEM PROGRAM
	void GenerateAGCEphemeris();
	int agcCelBody_RH(CELBODY *Cel, double mjd, int Flags, VECTOR3 *Pos = NULL, VECTOR3 *Vel = NULL);
	int agcCelBody_LH(CELBODY *Cel, double mjd, int Flags, VECTOR3 *Pos = NULL, VECTOR3 *Vel = NULL);
	void AGCEphemeris(double T0, double Epoch, double TEphem0);
	void AGCCorrectionVectors(double mjd_launch, double t_land, int mission, bool isCMC);
	void GenerateAGCCorrectionVectors();

	// SUBTHREAD MANAGEMENT
	HANDLE hThread;
	int subThreadMode;										// What should the subthread do?
	int subThreadStatus;									// 0 = done/not busy, 1 = busy, negative = done with error

	ApolloRTCCMFDData g_Data;

	//TARGETING VESSELS
	VESSEL* vessel;
	VESSEL* target;
	int targetnumber;		//Vessel index for target

	//GENERAL PARAMETERS
	double P30TIG;				//Maneuver GET
	VECTOR3 dV_LVLH;			//LVLH maneuver vector
	int vesseltype;				//0=CSM, 1=CSM/LM docked, 2 = LM, 3 = LM/CSM docked
	bool lemdescentstage;		//0 = ascent stage, 1 = descent stage
	bool inhibUplLOS;
	bool PADSolGood;
	int manpadenginetype;
	double t_TPI;				// Generally used TPI time
	int RetrofireEXDVOctals[016];

	//DOCKING INITIATION
	double DKI_TIG;		//Impulsive time of ignition
	int DKI_Profile;	//0 = Four-impulse: Phasing/CSI/CDH/TPI, 1 = Six-Impulse: Phasing/Boost/HAM/CSI/CDH/TPI, 2 = Four-impulse rescue: Height/CSI/CDH/TPI, 3 = Calculate TPI time only
	int DKI_TPI_Mode;	//0 = TPI on time, 1 = TPI at orbital midnight, 2 = TPI at X minutes before sunrise
	bool DKI_Maneuver_Line;	//false = define relative times, true = 0.5 revolutions between maneuvers
	bool DKI_Radial_DV;	//false = horizontal maneuver, true = 50 ft/s radial component
	double DKI_dt_TPI_sunrise;
	double DKI_dt_PBH;	//Delta time between phasing and boost/CSI
	double DKI_dt_BHAM;	//Delta time between boost and HAM
	double DKI_dt_HAMH;	//Delta time between HAM and CSI
	int DKI_N_HC;		//Half revolutions between CSI and CDH
	int DKI_N_PB;		//Number of half revs between Phasing and Boost/Height
	double t_TPIguess;
	DKIResults dkiresult;
	VECTOR3 DKI_DV;

	//CONCENTRIC RENDEZVOUS PAGE
	int SPQMode;	//0 = CSI on time, 1 = CDH, 2 = optimum CSI
	double CSItime;	//Time of the CSI maneuver
	double CDHtime;	//Time of the CDH maneuver
	double SPQTIG;	//Time of ignition for concentric rendezvous maneuver
	int CDHtimemode; //CSI: 0 = fixed TIG at TPI, 1 = fixed DH at CDH. CDH: 0=Fixed, 1 = Find GETI
	VECTOR3 SPQDeltaV;

	//ORBIT ADJUSTMENT PAGE
	int GMPManeuverCode; //Maneuver code
	bool OrbAdjAltRef;	//0 = use mean radius, 1 = use launchpad or landing site radius
	double GMPApogeeHeight;		//Desired apoapsis height
	double GMPPerigeeHeight;	//Desired periapsis height
	double GMPWedgeAngle;
	double GMPManeuverHeight;
	double GMPManeuverLongitude;
	double GMPHeightChange;
	double GMPNodeShiftAngle;
	double GMPDeltaVInput;
	double GMPPitch;
	double GMPYaw;
	double GMPApseLineRotAngle;
	int GMPRevs;
	double SPSGET;		//Maneuver GET
	double GPM_TIG;		//Maneuver GET output
	VECTOR3 OrbAdjDVX;	//LVLH maneuver vector
	//0 = Apogee
	//1 = Equatorial crossing
	//2 = Perigee
	//3 = Longitude
	//4 = Height
	//5 = Time
	//6 = Optimum
	int GMPManeuverPoint;
	//0 = Plane Change
	//1 = Circularization
	//2 = Height Change
	//3 = Node Shift
	//4 = Apogee and perigee change
	//5 = Input maneuver
	//6 = Combination apogee/perigee change and node shift
	//7 = Shift line-of-apsides
	//8 = Combination height maneuver and plane change
	//9 = Combination circularization and plane change
	//10 = Combination circularization and node shift
	//11 = Combination height maneuver and node shift
	//12 = Combination apogee/perigee change and line-of-apsides shift
	int GMPManeuverType;
	GPMPRESULTS GMPResults;

	//REFSMMAT PAGE
	double REFSMMATTime;
	int REFSMMATopt; //Displayed REFSMMAT page: 0 = P30 Maneuver, 1 = P30 Retro, 2 = LVLH, 3 = Lunar Entry, 4 = Launch, 5 = Landing Site, 6 = PTC, 7 = Attitude, 8 = LS during TLC
	int REFSMMATcur; //Currently saved REFSMMAT
	bool REFSMMATHeadsUp;

	//ENTY PAGE	
	int entrycritical; //1 = Midcourse, 2 = Abort, 3 = Corridor Control
	bool entrynominal; //0 = minimum DV, 1 = 31.7° line
	double EntryTIG;
	double EntryLat;
	double EntryLng;
	double EntryAng, EntryAngcor;
	double EntryTIGcor;
	double EntryLatcor;
	double EntryLngcor;
	VECTOR3 Entry_DV;
	double entryrange;
	double EntryRET05G; //Time of 0.05g
	double EntryRRT; //Time of entry interface (400k feet altitude)
	bool entrylongmanual; //0 = landing zone, 1 = manual longitude input
	int landingzone; //0 = Mid Pacific, 1 = East Pacific, 2 = Atlantic Ocean, 3 = Indian Ocean, 4 = West Pacific
	int entryprecision; //0 = conic, 1 = precision, 2 = PeA=-30 solution
	double RTEReentryTime; //Desired landing time
	double FlybyPeriAlt;
	double EntryDesiredInclination;
	int RTECalcMode; // 0 = ATP Tradeoff, 1 = ATP Search, 2 = ATP Discrete, 3 = UA Search, 4 = UA Discrete
	double RTEReturnInclination;
	int RTETradeoffMode; //0 = Near-Earth (F70), 1 = Remote-Earth (F71)
	int deorbitenginetype;

	//STATE VECTOR PAGE
	bool SVSlot; //true = CSM, false = LEM
	MPTSV UplinkSV;
	double SVDesiredGET;
	VESSEL* svtarget;
	int svtargetnumber;
	double AGSEpochTime;
	VECTOR3 AGSPositionVector, AGSVelocityVector;
	double AGSKFactor;
	AP11AGSSVPAD agssvpad;
	int SVOctals[021];
	VECTOR3 RLSUplink;
	int RLSOctals[010];
	int TLANDOctals[5];

	//MANEUVER PAD PAGE
	AP11MNV manpad;
	AP11LMMNV lmmanpad;
	char GDCset[64];
	bool HeadsUp;
	VECTOR3 TPIPAD_dV_LOS, TPIPAD_BT;
	double TPIPAD_dH, TPIPAD_R, TPIPAD_Rdot, TPIPAD_ELmin5, TPIPAD_AZ, TPIPAD_ddH;
	int manpadopt; //0 = Maneuver PAD, 1 = TPI PAD, 2 = TLI PAD
	double sxtstardtime;
	TLIPAD tlipad;
	AP11PDIPAD pdipad;

	///ENTRY PAD PAGE
	AP11ENT lunarentrypad;
	AP7ENT earthentrypad;
	int entrypadopt; //0 = Earth Entry Update, 1 = Lunar Entry
	double EntryRTGO;

	//MAP UPDATE PAGE
	AP10MAPUPDATE mapupdate;
	double GSAOSGET, GSLOSGET;
	int mappage, mapgs;
	double mapUpdateGET;

	//TLI PAGE
	//0 = TLI (nodal), 1 = TLI (free return)
	int TLImaneuver;

	//TLCC PAGE

	//1 = XYZ and T (Nodal) Targeting, 2 = FR BAP Fixed LPO, 3 = FR BAP Free LPO, 4 = Non Free BAP Fixed LPO, 5 = Non Free BAP Free LPO
	//6 = Circumlunar free-return flyby, nominal H_PC and phi_PC, 7 = Flyby with specific H_PC, 8 = SPS lunar flyby, 9 = Optimized RCS flyby
	int TLCCmaneuver;
	double TLCC_TIG;
	VECTOR3 TLCC_dV_LVLH;
	VECTOR3 R_TLI, V_TLI;
	int TLCCSolGood;

	//LANDMARK TRACKING PAGE
	AP11LMARKTRKPAD landmarkpad;
	double LmkLat, LmkLng;
	double LmkTime;

	//VECPOINT PAGE
	int VECoption;		//0 = Point SC at body, 1 = Open hatch thermal control, 2 = Point AOT with CSM
	int VECdirection;	//0 = +X, 1 = -X, 2 = +Y,3 = -Y,4 = +Z, 5 = -Z
	OBJHANDLE VECbody;	//handle for the desired body
	VECTOR3 VECangles;	//IMU angles

	//DOI Page
	VECTOR3 DOI_dV_LVLH;				//Integrated DV Vector

	//Skylab Page
	int Skylabmaneuver;					//0 = Presettings, 1 = NC1, 2 = NC2, 3 = NCC, 4 = NSR, 5 = TPI, 6 = TPM, 7 = NPC
	bool Skylab_NPCOption;				//0 = NC1 or NC2 with out-of-plane component, setting up a NPC maneuver 90° later
	bool Skylab_PCManeuver;				//0 = NC1 is setting up NPC, 1 = NC2 is setting up NPC
	double SkylabTPIGuess;
	double Skylab_n_C;
	double SkylabDH1;					//Delta Height at NCC
	double SkylabDH2;					//Delta Height at NSR
	double Skylab_E_L;
	bool SkylabSolGood;
	VECTOR3 Skylab_dV_NSR, Skylab_dV_NCC;//, Skylab_dV_NPC;
	double Skylab_dH_NC2, Skylab_dv_NC2, Skylab_dv_NCC;
	double Skylab_t_NC1, Skylab_t_NC2, Skylab_t_NCC, Skylab_t_NSR, Skylab_dt_TPM; //Skylab_t_NPC

	//Terrain Model
	double TMLat, TMLng, TMAzi, TMDistance, TMStepSize, TMAlt;

	//LM Ascent PAD
	AP11LMASCPAD lmascentpad;
	double t_LunarLiftoff;

	//Powered Descent Abort Program
	int PDAPEngine;	//0 = DPS/APS, 1 = APS
	bool PDAPTwoSegment;	//false = One Segment (Luminary099, FP6), true = Two Segment (Luminary116 and later, FP7 and later)
	double PDAPABTCOF[8];	//Luminary099 abort coefficients
	double DEDA224, DEDA225, DEDA226;
	int DEDA227;
	double PDAP_J1, PDAP_K1, PDAP_J2, PDAP_K2, PDAP_Theta_LIM, PDAP_R_amin;

	//Erasable Memory Programs
	int EMPUplinkType;	// 0 = P99
	int EMPUplinkNumber;

	//NAV CHECK PAGE
	AP7NAV navcheckpad;

	//DAP PAD PAGE
	AP10DAPDATA DAP_PAD;

	//LVDC PAGE
	double LVDCLaunchAzimuth;

	//AGC EPHEMERIS
	int AGCEphemOption;	//0 = AGC ephemerides, 1 = AGC precession/nutation/libration correction vectors
	double AGCEphemBRCSEpoch;
	double AGCEphemTEphemZero;
	double AGCEphemTIMEM0;
	double AGCEphemTEPHEM;
	double AGCEphemTLAND;
	int AGCEphemMission;
	bool AGCEphemIsCMC;

	//NODAL TARGET CONVERSION
	bool NodeConvOpt; //false = EMP to selenographc, true = selenographic to EMP
	double NodeConvLat;
	double NodeConvLng;
	double NodeConvGET;
	double NodeConvHeight;
	double NodeConvResLat;
	double NodeConvResLng;

private:

	AR_GCore* GC;
};




#endif // !__ARCORE_H