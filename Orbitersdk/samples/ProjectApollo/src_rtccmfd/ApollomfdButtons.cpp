#include "MFDButtonPage.hpp"
#include "ApollomfdButtons.h"
#include "ApolloRTCCMFD.h" 

ApolloRTCCMFDButtons::ApolloRTCCMFDButtons()
{
	static const MFDBUTTONMENU mnu0[] =
	{
		{ "Maneuver Targeting", 0, 'T' },
		{ "Pre-Advisory Data", 0, 'P' },
		{ "Utility", 0, 'U' },
		{ "MCC Displays", 0, 'M' },
		{ "Mission Planning", 0, 'B' },
		{ "Configuration", 0, 'S' },

		{ "Uplinks", 0, 'U' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
	};

	RegisterPage(mnu0, sizeof(mnu0) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAR", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTargetingMenu);
	RegisterFunction("PAD", OAPI_KEY_P, &ApolloRTCCMFD::menuSetPADMenu);
	RegisterFunction("UTI", OAPI_KEY_U, &ApolloRTCCMFD::menuSetUtilityMenu);
	RegisterFunction("MCC", OAPI_KEY_M, &ApolloRTCCMFD::menuSetMCCDisplaysPage);
	RegisterFunction("PLN", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);
	RegisterFunction("CFG", OAPI_KEY_C, &ApolloRTCCMFD::menuSetConfigPage);

	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuSetUplinkMenu);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);

	static const MFDBUTTONMENU mnu1[] =
	{
		{ "Mode", 0, 'T' },
		{ "Vector times", 0, 'P' },
		{ "Maneuver Time", 0, 'M' },
		{ "Arrival Time", 0, 'A' },
		{ "Number of revolutions", 0, 'N' },
		{ "Gravity sources", 0, 'G' },

		{ "Target Vessel", 0, 'V' },
		{ "Calculate burn", 0, 'C' },
		{ "Set Offset", 0, 'O' },
		{ "LVLH maneuver axis", 0, 'U' },
		{ "Transfer maneuver", 0, 'L' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu1, sizeof(mnu1) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_T, &ApolloRTCCMFD::menuCycleTwoImpulseOption);
	RegisterFunction("TGT", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleK30Vehicle);
	RegisterFunction("CVT", OAPI_KEY_M, &ApolloRTCCMFD::menuTIChaserVectorTime);
	RegisterFunction("TVT", OAPI_KEY_A, &ApolloRTCCMFD::menuTITargetVectorTime);
	RegisterFunction("T1", OAPI_KEY_N, &ApolloRTCCMFD::t1dialogue);
	RegisterFunction("T2", OAPI_KEY_G, &ApolloRTCCMFD::t2dialogue);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("OFF", OAPI_KEY_O, &ApolloRTCCMFD::TwoImpulseOffset);
	RegisterFunction("DIS", OAPI_KEY_C, &ApolloRTCCMFD::menuSetTIMultipleSolutionPage);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);

	static const MFDBUTTONMENU mnu2[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Last page", 0, 'L' },
	};

	RegisterPage(mnu2, sizeof(mnu2) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_X, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Y, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Z, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_B, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::lambertcalc);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTITransferPage);
	RegisterFunction("BCK", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLambertPage);

	static const MFDBUTTONMENU mnu3[] =
	{
		{ "SPQ Init", 0, 'I' },
		{ "Chaser/Target", 0, 'V' },
		{ "Chaser threshold", 0, 'F' },
		{ "Target threshold", 0, 'G' },
		{ "CSI or CDH", 0, 'M' },
		{ "Time Mode", 0, 'T' },

		{ "Maneuver Time", 0, 'M' },
		{ "Target vehicle", 0, 'O' },
		{ "Calculate burn", 0, 'C' },
		{ "Rendezvous Display", 0, 'D'},
		{ "Transfer to MPT", 0, 'L' },
		{ "Previous page", 0, 'B' },
	};

	RegisterPage(mnu3, sizeof(mnu3) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetSPQInitializationPage);
	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::menuCycleSPQChaser);
	RegisterFunction("CTH", OAPI_KEY_F, &ApolloRTCCMFD::menuSetSPQChaserThresholdTime);
	RegisterFunction("TTH", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSPQTargetThresholdTime);
	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuCycleSPQMode);
	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::set_CDHtimemode);

	RegisterFunction("TIG", OAPI_KEY_N, &ApolloRTCCMFD::SPQtimedialogue);
	RegisterFunction("TGT", OAPI_KEY_O, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::SPQcalc);
	RegisterFunction("DIS", OAPI_KEY_D, &ApolloRTCCMFD::menuSetRendezvousEvaluationDisplayPage);
	RegisterFunction("MPT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetSPQorDKIRTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu4[] =
	{
		{ "Set input", 0, 'S' },
		{ "Previous Item", 0, 'P' },
		{ "Next Item", 0, 'N' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Transfer to MPT", 0, 'T' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu4, sizeof(mnu4) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SET", OAPI_KEY_S, &ApolloRTCCMFD::menuSetGMPInput);
	RegisterFunction("<<", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleGMPMarkerDown);
	RegisterFunction(">>", OAPI_KEY_N, &ApolloRTCCMFD::menuCycleGMPMarkerUp);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::GPMPCalc);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_T, &ApolloRTCCMFD::menuSetGPMTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu5[] =
	{
		{ "Time of Alignment", 0, 'G' },
		{ "", 0, ' ' },
		{ "Heads up/down for P30", 0, 'H' },
		{ "", 0, ' ' },
		{ "REFSMMAT from AGC", 0, 'D' },
		{ "", 0, ' ' },

		{ "Option", 0, 'O' },
		{ "Calculate", 0, 'C' },
		{ "Upload", 0, 'U' },
		{ "Move REFSMMAT", 0, 'U' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu5, sizeof(mnu5) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::REFSMMATTimeDialogue);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::cycleREFSMMATHeadsUp);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DWN", OAPI_KEY_D, &ApolloRTCCMFD::GetREFSMMATfromAGC);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::CycleREFSMMATopt);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::calcREFSMMAT);
	RegisterFunction("G00", OAPI_KEY_U, &ApolloRTCCMFD::menuREFSMMATLockerMovement);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);

	static const MFDBUTTONMENU mnu6[] =
	{
		{ "Deorbit Maneuver", 0, 'D' },
		{ "Return to Earth (Earth-centered)", 0, 'E' },
		{ "Return to Earth (Moon-centered)", 0, 'M' },
		{ "Splashdown Update", 0, 'S' },
		{ "RTE Constraints", 0, 'C' },
		{ "RTE Tradeoff", 0, 'T' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu6, sizeof(mnu6) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DEO", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDeorbitPage);
	RegisterFunction("RTE", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEarthEntryPage);
	RegisterFunction("RTM", OAPI_KEY_M, &ApolloRTCCMFD::menuSetMoonEntryPage);
	RegisterFunction("SPL", OAPI_KEY_S, &ApolloRTCCMFD::menuSetEntryUpdatePage);
	RegisterFunction("CON", OAPI_KEY_C, &ApolloRTCCMFD::menuSetRTEConstraintsPage);
	RegisterFunction("TRD", OAPI_KEY_T, &ApolloRTCCMFD::menuSetRTETradeoffDisplayPage);

	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu7[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Set Target", 0, 'T' },
		{ "State Vector Slot", 0, 'D' },
		{ "AGS K Factor", 0, 'A' },

		{ "Calculate State Vector", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu7, sizeof(mnu7) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("SLT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchSVSlot);
	RegisterFunction("AGS", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAGSKFactor);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuAGSSVCalc);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);

	static const MFDBUTTONMENU mnu8[] =
	{
		{ "Mission (GET base)", 0, 'G' },
		{ "Vessel type", 0, 'V' },
		{ "LM stage", 0, 'T' },
		{ "", 0, ' ' },
		{ "Sextant Star Time", 0, 'S' },
		{ "Uplinks in LOS", 0, 'U' },

		{ "Set launch day", 0, 'M' },
		{ "Set launch time", 0, 'K' },
		{ "Set AGC Epoch", 0, 'E' },
		{ "Update liftoff time", 0, 'T' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu8, sizeof(mnu8) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MIS", OAPI_KEY_G, &ApolloRTCCMFD::menuMissionNumberInput);
	RegisterFunction("TYP", OAPI_KEY_V, &ApolloRTCCMFD::menuChangeVesselType);
	RegisterFunction("STA", OAPI_KEY_T, &ApolloRTCCMFD::menuCycleLMStage);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("SXT", OAPI_KEY_S, &ApolloRTCCMFD::menusextantstartime);
	RegisterFunction("UPL", OAPI_KEY_A, &ApolloRTCCMFD::menuSwitchUplinkInhibit);

	RegisterFunction("DAT", OAPI_KEY_M, &ApolloRTCCMFD::menuSetLaunchDate);
	RegisterFunction("TIM", OAPI_KEY_K, &ApolloRTCCMFD::menuSetLaunchTime);
	RegisterFunction("EPO", OAPI_KEY_E, &ApolloRTCCMFD::menuSetAGCEpoch);
	RegisterFunction("UPD", OAPI_KEY_U, &ApolloRTCCMFD::menuUpdateLiftoffTime);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu9[] =
	{
		{ "Maneuver Vehicle", 0, 'V' },
		{ "Maneuver Engine", 0, 'E' },
		{ "Heads-up/down", 0, 'H' },
		{ "", 0, ' ' },
		{ "Manual TIG", 0, 'T' },
		{ "Manual DV", 0, 'D' },

		{ "Calculate PAD", 0, 'C' },
		{ "Switch Maneuver option", 0, 'O' },
		{ "", 0, ' ' },
		{ "Request burn data", 0, 'R' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu9, sizeof(mnu9) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchManPADEngine);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::menuSwitchHeadsUp);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcManPAD);
	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuSwitchManPADopt);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("REQ", OAPI_KEY_R, &ApolloRTCCMFD::menuRequestLTMFD);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu10[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Downlink Entry Target", 0, 'W' },
		{ "", 0, ' ' },

		{ "Calculate Entry PAD", 0, 'C' },
		{ "Lunar/Earth Entry", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu10, sizeof(mnu10) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DWN", OAPI_KEY_W, &ApolloRTCCMFD::GetEntryTargetfromAGC);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcEntryPAD);
	RegisterFunction("OPT", OAPI_KEY_V, &ApolloRTCCMFD::menuSwitchEntryPADOpt);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu11[] =
	{
		{ "GET for anchor vector", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calc Map Update", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu11, sizeof(mnu11) / sizeof(MFDBUTTONMENU));

	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetMapUpdateGET);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcMapUpdate);
	RegisterFunction("MOD", OAPI_KEY_V, &ApolloRTCCMFD::menuSwitchMapUpdate);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu12[] =
	{
		{ "LOI init page", 0, 'I' },
		{ "Vector time", 0, 'V' },
		{ "LOI1 apolune", 0, 'A' },
		{ "LOI1 perilune", 0, 'P' },
		{ "Max DV for pos soln", 0, 'D' },
		{ "Max DV for neg soln", 0, 'E' },

		{ "Calculate maneuver", 0, 'C' },
		{ "Minimum azimuth", 0, 'M' },
		{ "Desired azimuth", 0, 'N' },
		{ "Maximum azimuth", 0, 'O' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu12, sizeof(mnu12) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetLOIInitPage);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuSetLOIVectorTime);
	RegisterFunction("APO", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLOIApo);
	RegisterFunction("PER", OAPI_KEY_P, &ApolloRTCCMFD::menuSetLOIPeri);
	RegisterFunction("DVP", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLOIMaxDVPos);
	RegisterFunction("DVN", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLOIMaxDVNeg);

	RegisterFunction("DIS", OAPI_KEY_C, &ApolloRTCCMFD::menuSetLOIDisplayPage);
	RegisterFunction("AMN", OAPI_KEY_M, &ApolloRTCCMFD::menuSetLOIMinAzi);
	RegisterFunction("ADS", OAPI_KEY_N, &ApolloRTCCMFD::menuSetLOIDesiredAzi);
	RegisterFunction("AMX", OAPI_KEY_O, &ApolloRTCCMFD::menuSetLOIMaxAzi);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu13[] =
	{
		{ "T1 guess", 0, 'T' },
		{ "Landmark Latitude", 0, 'A' },
		{ "Landmark Longitude", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu13, sizeof(mnu13) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_T, &ApolloRTCCMFD::menuSetLmkTime);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLmkLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuSetLmkLng);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLmkPADCalc);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu14[] =
	{
		{ "Rendezvous Targeting", 0, 'R' },
		{ "Orbit Adjustment", 0, 'O' },
		{ "TLI Planning", 0, 'T' },
		{ "Midcourse", 0, 'M' },
		{ "Lunar Orbit", 0, 'S' },
		{ "Entry", 0, 'E' },

		{ "DOI Targeting", 0, 'D' },
		{ "Lunar Launch Winodw", 0, 'W' },
		{ "Lunar Launch Targeting", 0, 'L' },
		{ "Lunar Ascent", 0, 'A' },
		{ "", 0, ' ' },
		{ "Back to main menu", 0, 'B' },
	};

	RegisterPage(mnu14, sizeof(mnu14) / sizeof(MFDBUTTONMENU));

	RegisterFunction("REN", OAPI_KEY_R, &ApolloRTCCMFD::menuSetRendezvousPage);
	RegisterFunction("ORB", OAPI_KEY_O, &ApolloRTCCMFD::menuSetOrbAdjPage);
	RegisterFunction("TLI", OAPI_KEY_T, &ApolloRTCCMFD::menuTLIPlanningPage);
	RegisterFunction("MCC", OAPI_KEY_M, &ApolloRTCCMFD::menuMidcoursePage);
	RegisterFunction("LOI", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLOIPage);
	RegisterFunction("ENT", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEntryPage);

	RegisterFunction("LDP", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDescPlanCalcPage);
	RegisterFunction("LLW", OAPI_KEY_W, &ApolloRTCCMFD::menuSetLunarLiftoffPage);
	RegisterFunction("LLT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLunarLaunchTargetingPage);
	RegisterFunction("ASC", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLunarAscentPage);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu15[] =
	{
		{ "Pointing body", 0, 'P' },
		{ "Vessel axis", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate attitude", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu15, sizeof(mnu15) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_G, &ApolloRTCCMFD::cycleVECPOINTOpt);
	RegisterFunction("BOD", OAPI_KEY_P, &ApolloRTCCMFD::vecbodydialogue);
	RegisterFunction("DIR", OAPI_KEY_D, &ApolloRTCCMFD::cycleVECDirOpt);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuVECPOINTCalc);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu16[] =
	{
		{ "Init parameters", 0, 'I' },
		{ "MPT vehicle", 0, 'V' },
		{ "MPT vector time", 0, 'T' },
		{ "LDPP mode", 0, 'M' },
		{ "LDPP sequence", 0, 'S' },
		{ "Desired height", 0, 'H' },

		{ "Threshold time 1", 0, 'A' },
		{ "Threshold time 2", 0, 'C' },
		{ "Threshold time 3", 0, 'D' },
		{ "Threshold time 4", 0, 'E' },
		{ "LDPP display", 0, 'P' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu16, sizeof(mnu16) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_G, &ApolloRTCCMFD::menuSetDescPlanInitPage);
	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::cycleLDPPVehicle);
	RegisterFunction("VTI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLDPPVectorTime);
	RegisterFunction("MOD", OAPI_KEY_M, &ApolloRTCCMFD::menuSetLDPPMode);
	RegisterFunction("SEQ", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLDPPSequence);
	RegisterFunction("HEI", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLDPPDesiredHeight);

	RegisterFunction("TH1", OAPI_KEY_A, &ApolloRTCCMFD::menuLDPPThresholdTime1);
	RegisterFunction("TH2", OAPI_KEY_B, &ApolloRTCCMFD::menuLDPPThresholdTime2);
	RegisterFunction("TH3", OAPI_KEY_D, &ApolloRTCCMFD::menuLDPPThresholdTime3);
	RegisterFunction("TH4", OAPI_KEY_E, &ApolloRTCCMFD::menuLDPPThresholdTime4);
	RegisterFunction("DPT", OAPI_KEY_C, &ApolloRTCCMFD::menuSetDescPlanTablePage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu17[] =
	{
		{ "Maneuver Option", 0, 'M' },
		{ "Ground Elapsed Time", 0, 'G' },
		{ "Revs between NC1 and NC2", 0, 'N' },
		{ "DH at NCC", 0, 'H' },
		{ "DH at NSR", 0, 'I' },
		{ "Elevation angle at TPI", 0, 'E' },

		{ "Target Vessel", 0, 'V' },
		{ "Calculate maneuver", 0, 'C' },
		{ "Cycle PC Option for NCC1/2", 0, 'P' },
		{ "NC1 or NC2 Time for NPC", 0, 'T' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu17, sizeof(mnu17) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSwitchSkylabManeuver);
	RegisterFunction("TIG", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSkylabGET);
	RegisterFunction("NC", OAPI_KEY_N, &ApolloRTCCMFD::menuSetSkylabNC);
	RegisterFunction("DH1", OAPI_KEY_H, &ApolloRTCCMFD::menuSetSkylabDH1);
	RegisterFunction("DH2", OAPI_KEY_I, &ApolloRTCCMFD::menuSetSkylabDH2);
	RegisterFunction("EL", OAPI_KEY_E, &ApolloRTCCMFD::menuSetSkylabEL);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSkylabCalc);
	RegisterFunction("NPC", OAPI_KEY_P, &ApolloRTCCMFD::menuCyclePlaneChange);
	RegisterFunction("PCM", OAPI_KEY_T, &ApolloRTCCMFD::menuCyclePCManeuver);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu18[] =
	{
		{ "Approach azimuth", 0, 'A' },
		{ "Altitude at PDI", 0, 'H' },
		{ "Powered descent sim flag", 0, 'P' },
		{ "PDI time", 0, 'F' },
		{ "Orbits between DOI and PDI", 0, 'N' },
		{ "", 0, ' ' },

		{ "Descent Flight Time", 0, 'T' },
		{ "Descent Flight Arc", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu18, sizeof(mnu18) / sizeof(MFDBUTTONMENU));

	RegisterFunction("AZI", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLDPPAzimuth);
	RegisterFunction("HDP", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLDPPDescIgnHeight);
	RegisterFunction("PDS", OAPI_KEY_P, &ApolloRTCCMFD::cycleLDPPPoweredDescSimFlag);
	RegisterFunction("PDI", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLDPPPoweredDescTime);
	RegisterFunction("N", OAPI_KEY_N, &ApolloRTCCMFD::menuSetLDPPDwellOrbits);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("DFT", OAPI_KEY_T, &ApolloRTCCMFD::menuSetLDPPDescentFlightTime);
	RegisterFunction("DFA", OAPI_KEY_C, &ApolloRTCCMFD::menuSetLDPPDescentFlightArc);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDescPlanCalcPage);


	static const MFDBUTTONMENU mnu19[] =
	{
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Approach azimuth", 0, 'L' },
		{ "Distance", 0, ' ' },
		{ "Step Size", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Generate terrain model", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu19, sizeof(mnu19) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuTMLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuTMLng);
	RegisterFunction("AZI", OAPI_KEY_L, &ApolloRTCCMFD::menuTMAzi);
	RegisterFunction("DIS", OAPI_KEY_B, &ApolloRTCCMFD::menuTMDistance);
	RegisterFunction("STE", OAPI_KEY_D, &ApolloRTCCMFD::menuTMStepSize);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTerrainModelCalc);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);
	

	static const MFDBUTTONMENU mnu20[] =
	{

		{ "Maneuver PAD", 0, 'M' },
		{ "Entry PAD", 0, 'E' },
		{ "Landmark Tracking", 0, 'L' },
		{ "Map Update", 0, 'U' },
		{ "Nav Check PAD", 0, 'N' },
		{ "P37 PAD", 0, 'P' },

		{ "DAP PAD", 0, 'A' },
		{ "LM Ascent PAD", 0, 'C' },
		{ "AGS SV PAD", 0, 'F' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu20, sizeof(mnu20) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSetManPADPage);
	RegisterFunction("ENP", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEntryPADPage);
	RegisterFunction("LMK", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLandmarkTrkPage);
	RegisterFunction("MAP", OAPI_KEY_U, &ApolloRTCCMFD::menuSetMapUpdatePage);
	RegisterFunction("NAV", OAPI_KEY_N, &ApolloRTCCMFD::menuSetNavCheckPADPage);
	RegisterFunction("P37", OAPI_KEY_P, &ApolloRTCCMFD::menuSetP37PADPage);

	RegisterFunction("DAP", OAPI_KEY_A, &ApolloRTCCMFD::menuSetDAPPADPage);
	RegisterFunction("ASC", OAPI_KEY_C, &ApolloRTCCMFD::menuSetLMAscentPADPage);
	RegisterFunction("AGS", OAPI_KEY_F, &ApolloRTCCMFD::menuSetAGSSVPage);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu21[] =
	{
		{ "Landing Site Update", 0, 'V' },
		{ "REFSMMAT", 0, 'R' },
		{ "Vector Pointing", 0, 'P' },
		{ "Erasable Memory Programs", 0, 'E' },
		{ "Convert nodal target", 0, 'N' },
		{ "Descent abort", 0, 'A' },

		{ "LVDC options", 0, 'L' },
		{ "Terrain Model Generation", 0, 'T' },
		{ "AGC Ephemeris Generator", 0, 'A' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to main menu", 0, 'B' }
	};

	RegisterPage(mnu21, sizeof(mnu21) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LS", OAPI_KEY_V, &ApolloRTCCMFD::menuSetLSUpdateMenu);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuSetREFSMMATPage);
	RegisterFunction("VEC", OAPI_KEY_P, &ApolloRTCCMFD::menuSetVECPOINTPage);
	RegisterFunction("EMP", OAPI_KEY_E, &ApolloRTCCMFD::menuSetEMPPage);
	RegisterFunction("NOD", OAPI_KEY_N, &ApolloRTCCMFD::menuSetNodalTargetConversionPage);
	RegisterFunction("ABO", OAPI_KEY_A, &ApolloRTCCMFD::menuSetPDAPPage);

	RegisterFunction("IU", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLVDCPage);
	RegisterFunction("TER", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTerrainModelPage);
	RegisterFunction("EPH", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAGCEphemerisPage);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu22[] =
	{
		{ "Maneuver type", 0, 'M' },
		{ "Vector time", 0, 'V' },
		{ "Ignition time", 0, 'T' },
		{ "Column for results", 0, 'C' },
		{ "Docked or undocked", 0, 'D' },
		{ "Cycle DOI options", 0, 'S' },

		{ "Calculate maneuver", 0, 'O' },
		{ "Pericynthion height", 0, 'H' },
		{ "Return inclination", 0, 'I' },
		{ "PC height for mode 5", 0, 'E' },
		{ "Additional Constraints", 0, 'A' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu22, sizeof(mnu22) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_M, &ApolloRTCCMFD::menuSwitchTLCCManeuver);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuTLCCVectorTime);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTLCCGET);
	RegisterFunction("COL", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleTLCCColumnNumber);
	RegisterFunction("CFG", OAPI_KEY_D, &ApolloRTCCMFD::menuCycleTLCCConfiguration);
	RegisterFunction("SFP", OAPI_KEY_S, &ApolloRTCCMFD::menuCycleTLCCCSFPBlockNumber);

	RegisterFunction("MID", OAPI_KEY_O, &ApolloRTCCMFD::menuMidcourseTradeoffPage);
	RegisterFunction("HPC", OAPI_KEY_H, &ApolloRTCCMFD::menuSetTLCCAlt);
	RegisterFunction("INC", OAPI_KEY_I, &ApolloRTCCMFD::menuSetTLCCDesiredInclination);
	RegisterFunction("PC5", OAPI_KEY_E, &ApolloRTCCMFD::menuSetTLCCAltMode5);
	RegisterFunction("CON", OAPI_KEY_A, &ApolloRTCCMFD::menuSetMidcourseConstraintsPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu23[] =
	{
		{ "LLWP Init", 0, 'I' },
		{ "Chaser vehicle", 0, 'C' },
		{ "Vector time", 0, 'V' },
		{ "Threshold time", 0, 'H' },
		{ "CSI option", 0, 'S' },
		{ "CDH option", 0, 'D' },

		{ "Target Vessel", 0, 'T' },
		{ "TPI option", 0, 'P' },
		{ "DH option", 0, 'A' },
		{ "Delta heights", 0, 'E' },
		{ "LLWP Display", 0, 'F' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu23, sizeof(mnu23) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_I, &ApolloRTCCMFD::menuSetLLWPInitPage);
	RegisterFunction("CHA", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleLLWPChaserOption);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuLLWPVectorTime);
	RegisterFunction("THT", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLiftoffguess);
	RegisterFunction("CSI", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLLWPCSIFlag);
	RegisterFunction("CDH", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLLWPCDHFlag);

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_target);
	RegisterFunction("TPI", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("LLW", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("DH", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLLWPDeltaHeights);
	RegisterFunction("DIS", OAPI_KEY_F, &ApolloRTCCMFD::menuSetLLWPDisplayPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu24[] =
	{
		{ "P99", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Uplink Load Number", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink EMP", 0, 'U' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu24, sizeof(mnu24) / sizeof(MFDBUTTONMENU));

	RegisterFunction("P99", OAPI_KEY_T, &ApolloRTCCMFD::menuSetEMPUplinkP99);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("NUM", OAPI_KEY_O, &ApolloRTCCMFD::menuSetEMPUplinkNumber);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEMPUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu25[] =
	{
		{ "GET of Nav Check", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' }
	};

	RegisterPage(mnu25, sizeof(mnu25) / sizeof(MFDBUTTONMENU));

	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetNavCheckGET);
	RegisterFunction("", OAPI_KEY_J, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuNavCheckPADCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu26[] =
	{
		{ "Est. Time of Ignition", 0, 'T' },
		{ "Manual longitude or zone", 0, 'L' },
		{ "Des. Landing Long", 0, 'O' },
		{ "Entry Angle", 0, 'A' },
		{ "Maneuver Type", 0, 'D' },
		{ "", 0, ' ' },

		{ "Calculate Entry", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Add to MPT", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu26, sizeof(mnu26) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::EntryTimeDialogue);
	RegisterFunction("LMO", OAPI_KEY_L, &ApolloRTCCMFD::EntryLongitudeModeDialogue);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::EntryLngDialogue);
	RegisterFunction("ANG", OAPI_KEY_A, &ApolloRTCCMFD::EntryAngDialogue);
	RegisterFunction("OPT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchEntryNominal);
	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchDeorbitEngineOption);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDeorbitCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuTransferRTEToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu27[] =
	{
		{ "Est. Time of Ignition", 0, 'T' },
		{ "Manual longitude or zone", 0, 'L' },
		{ "Des. Landing Long", 0, 'O' },
		{ "Entry Angle", 0, 'A' },
		{ "", 0, ' ' },
		{ "Maneuver Type", 0, 'E' },

		{ "Calculate Entry", 0, 'C' },
		{ "Maneuver code", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Add to MPT", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu27, sizeof(mnu27) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::EntryTimeDialogue);
	RegisterFunction("LMO", OAPI_KEY_L, &ApolloRTCCMFD::EntryLongitudeModeDialogue);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::EntryLngDialogue);
	RegisterFunction("ANG", OAPI_KEY_A, &ApolloRTCCMFD::EntryAngDialogue);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TYP", OAPI_KEY_E, &ApolloRTCCMFD::menuSwitchCritical);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryCalc);
	RegisterFunction("THR", OAPI_KEY_V, &ApolloRTCCMFD::menuSetRTEManeuverCode);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuTransferRTEToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu28[] =
	{
		{ "Return-to-earth mode", 0, 'N' },
		{ "Est. Time of Ignition", 0, 'T' },
		{ "Manual longitude or zone", 0, 'L' },
		{ "Des. Landing Long", 0, 'O' },
		{ "Est. time of reentry", 0, 'S' },
		{ "FR Inclination", 0, 'I' },

		{ "Calculate Entry", 0, 'C' },
		{ "Maneuver code", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Add to MPT", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu28, sizeof(mnu28) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_N, &ApolloRTCCMFD::CycleRTECalcMode);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::EntryTimeDialogue);
	RegisterFunction("LMO", OAPI_KEY_L, &ApolloRTCCMFD::EntryLongitudeModeDialogue);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::EntryLngDialogue);
	RegisterFunction("EIT", OAPI_KEY_S, &ApolloRTCCMFD::menuSetRTEReentryTime);
	RegisterFunction("INC", OAPI_KEY_I, &ApolloRTCCMFD::menuSetEntryDesiredInclination);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuMoonRTECalc);
	RegisterFunction("THR", OAPI_KEY_E, &ApolloRTCCMFD::menuSetRTEManeuverCode);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuTransferRTEToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu29[] =
	{
		{ "Constraints with number", 0, 'V' },
		{ "Constraints with text", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu29, sizeof(mnu29) / sizeof(MFDBUTTONMENU));

	RegisterFunction("CO1", OAPI_KEY_V, &ApolloRTCCMFD::menuSetRTEConstraintF86);
	RegisterFunction("CO2", OAPI_KEY_T, &ApolloRTCCMFD::menuSetRTEConstraintF87);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_I, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu30[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Update", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Entry Range", 0, 'R' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu30, sizeof(mnu30) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuEntryUpdateCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("RAN", OAPI_KEY_R, &ApolloRTCCMFD::EntryRangeDialogue);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuEntryUpdateUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu31[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu31, sizeof(mnu31) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu32[] =
	{
		{ "Lambert targeting", 0, 'L' },
		{ "CDH/NSR maneuver", 0, 'C' },
		{ "Docking initate", 0, 'D' },
		{ "Skylab Rendezvous", 0, 'S' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to main menu", 0, 'B' },
	};

	RegisterPage(mnu32, sizeof(mnu32) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TI", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLambertPage);
	RegisterFunction("SPQ", OAPI_KEY_C, &ApolloRTCCMFD::menuSetSPQPage);
	RegisterFunction("DKI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetDKIPage);
	RegisterFunction("SKY", OAPI_KEY_S, &ApolloRTCCMFD::menuSetSkylabPage);
	RegisterFunction("TPI", OAPI_KEY_A, &ApolloRTCCMFD::menuSetTPITimesPage);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu33[] =
	{
		{ "DKI Init", 0, 'I' },
		{ "Rendezvous profile", 0, 'P' },
		{ "Time of phasing maneuver", 0, 'T' },
		{ "Time of TPI maneuver", 0, 'L' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
	

		{ "Target name", 0, 'V' },
		{ "Calculate maneuver", 0, 'C' },
		{ "DKI options page", 0, 'Q' },
		{ "", 0, ' ' },
		{ "Transfer to MPT", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu33, sizeof(mnu33) / sizeof(MFDBUTTONMENU));

	RegisterFunction("INI", OAPI_KEY_A, &ApolloRTCCMFD::menuSetDKIInitializationPage);
	RegisterFunction("PRO", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleDKIProfile);
	RegisterFunction("CHA", OAPI_KEY_D, &ApolloRTCCMFD::menuCycleDKIChaser);
	RegisterFunction("VTI", OAPI_KEY_E, &ApolloRTCCMFD::menuSetDKIThresholdTime);
	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::DKITIGDialogue);
	RegisterFunction("TPI", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTPIguess);

	RegisterFunction("TGT", OAPI_KEY_V, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDKICalc);
	RegisterFunction("OPT", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetDKIOptionsPage);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuSetSPQorDKIRTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu34[] =
	{
		{ "Maneuver line or times", 0, 'L' },
		{ "Fixed radial component", 0, 'R' },
		{ "TPI mode", 0, 'O' },
		{ "TPI time before sunrise", 0, 'S' },
		{ "Half-revs between CSI and CDH", 0, 'N' },
		{ "Half-revs between Phasing and Boost", 0, 'M' },

		{ "Time between abort and Boost/CSI", 0, 'E' },
		{ "Time between Boost and HAM", 0, 'V' },
		{ "Time between HAM and CSI", 0, 'Q' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to DKI page", 0, 'B' },
	};

	RegisterPage(mnu34, sizeof(mnu34) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAN", OAPI_KEY_L, &ApolloRTCCMFD::menuCycleDKIManeuverLine);
	RegisterFunction("RAD", OAPI_KEY_R, &ApolloRTCCMFD::menuCycleDKIRadialComponent);
	RegisterFunction("TPI", OAPI_KEY_O, &ApolloRTCCMFD::menuCycleDKITPIMode);
	RegisterFunction("SUN", OAPI_KEY_S, &ApolloRTCCMFD::DKITPIDTDialogue);
	RegisterFunction("NHC", OAPI_KEY_N, &ApolloRTCCMFD::DKINHCDialogue);
	RegisterFunction("NPB", OAPI_KEY_M, &ApolloRTCCMFD::DKINPBDialogue);

	RegisterFunction("DT1", OAPI_KEY_E, &ApolloRTCCMFD::menuDKIDeltaT1);
	RegisterFunction("DT2", OAPI_KEY_V, &ApolloRTCCMFD::menuDKIDeltaT2);
	RegisterFunction("DT3", OAPI_KEY_Q, &ApolloRTCCMFD::menuDKIDeltaT3);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDKIPage);


	static const MFDBUTTONMENU mnu35[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate DAP PAD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu35, sizeof(mnu35) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuDAPPADCalc);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu36[] =
	{
		{ "Launch Azimuth", 0, 'A' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu36, sizeof(mnu36) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LAU", OAPI_KEY_A, &ApolloRTCCMFD::menuLaunchAzimuthCalc);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu37[] =
	{
		{ "Epoch of BRCS", 0, 'E' },
		{ "MJD of July 1st", 0, 'Z' },
		{ "MJD of launch", 0, 'L' },
		{ "MJD of mid-mission", 0, 'M' },
		{ "Time of landing", 0, 'T' },
		{ "Apollo mission", 0, 'N' },

		{ "Cycle options", 0, 'O' },
		{ "Generate file", 0, 'C' },
		{ "CMC or LGC", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu37, sizeof(mnu37) / sizeof(MFDBUTTONMENU));

	RegisterFunction("EPO", OAPI_KEY_A, &ApolloRTCCMFD::menuSetAGCEphemBRCSEpoch);
	RegisterFunction("ZER", OAPI_KEY_L, &ApolloRTCCMFD::menuSetAGCEphemTEphemZero);
	RegisterFunction("LAU", OAPI_KEY_L, &ApolloRTCCMFD::menuSetAGCEphemTEPHEM);
	RegisterFunction("MID", OAPI_KEY_M, &ApolloRTCCMFD::menuSetAGCEphemTIMEM0);
	RegisterFunction("LAN", OAPI_KEY_T, &ApolloRTCCMFD::menuSetAGCEphemTLAND);
	RegisterFunction("MIS", OAPI_KEY_N, &ApolloRTCCMFD::menuSetAGCEphemMission);

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuCycleAGCEphemOpt);
	RegisterFunction("GEN", OAPI_KEY_C, &ApolloRTCCMFD::menuGenerateAGCEphemeris);
	RegisterFunction("AGC", OAPI_KEY_U, &ApolloRTCCMFD::menuCycleAGCEphemAGCType);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu38[] =
	{
		{ "Liftoff time", 0, 'L' },
		{ "Horizontal velocity", 0, 'H' },
		{ "Vertical velocity", 0, 'V' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Choose target", 0, 'A' },
		{ "Calculate lunar ascent", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu38, sizeof(mnu38) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LTO", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLAPLiftoffTime);
	RegisterFunction("HOR", OAPI_KEY_H, &ApolloRTCCMFD::menuLunarLiftoffVHorInput);
	RegisterFunction("VER", OAPI_KEY_V, &ApolloRTCCMFD::menuLunarLiftoffVVertInput);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_A, &ApolloRTCCMFD::set_target);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLAPCalc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuTransferPoweredAscentToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu39[] =
	{
		{ "Liftoff time", 0, 'L' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Ascent PAD", 0, 'C' },
		{ "Set target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu39, sizeof(mnu39) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LTO", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLAPLiftoffTime);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuAscentPADCalc);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_target);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetPADMenu);


	static const MFDBUTTONMENU mnu40[] =
	{
		{ "Abort engine", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Descent Abort", 0, 'C' },
		{ "Set target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to AGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu40, sizeof(mnu40) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ENG", OAPI_KEY_E, &ApolloRTCCMFD::menuCyclePDAPEngine);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuPDAPCalc);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_target);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuAP11AbortCoefUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu41[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Calculate longitude from GET", 0, 'G' },
		{ "Calculate GET from longitude", 0, 'L' },
		{ "Calc apo/peri values from GET", 0, 'R' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu41, sizeof(mnu41) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GETL", OAPI_KEY_G, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETL);
	RegisterFunction("L", OAPI_KEY_L, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsL);
	RegisterFunction("GETR", OAPI_KEY_R, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETBV);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu42[] =
	{
		{ "Select display", 0, 'O' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu42, sizeof(mnu42) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MSK", OAPI_KEY_O, &ApolloRTCCMFD::menuMSKRequest);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu43[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Update state vector", 0, 'U' },
		{ "GET for Vector 1", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu43, sizeof(mnu43) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("UPD", OAPI_KEY_U, &ApolloRTCCMFD::menuSpaceDigitalsInit);
	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateSpaceDigitals);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu44[] =
	{
		{ "Planning mode active", 0, 'A' },
		{ "MPT Init", 0, 'Q' },
		{ "Orientation change", 0, 'O' },
		{ "Delete maneuver", 0, 'D' },
		{ "Copy ephemeris", 0, 'H' },
		{ "Cape Crossing Init", 0, 'C' },

		{ "Direct Input", 0, 'I' },
		{ "Add TLI to MPT", 0, 'T' },
		{ "Add PDI to MPT", 0, 'P' },
		{ "", 0, ' ' },
		{ "Fuel remaining", 0, 'F' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu44, sizeof(mnu44) / sizeof(MFDBUTTONMENU));

	RegisterFunction("ACT", OAPI_KEY_A, &ApolloRTCCMFD::menuMPTCycleActive);
	RegisterFunction("INI", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetMPTInitPage);
	RegisterFunction("UDT", OAPI_KEY_O, &ApolloRTCCMFD::menuMPTVehicleOrientationChange);
	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTDeleteManeuver);
	RegisterFunction("P16", OAPI_KEY_H, &ApolloRTCCMFD::menuMPTCopyEphemeris);
	RegisterFunction("P17", OAPI_KEY_C, &ApolloRTCCMFD::menuCapeCrossingInit);

	RegisterFunction("INP", OAPI_KEY_I, &ApolloRTCCMFD::menuSetMPTDirectInputPage);
	RegisterFunction("TLI", OAPI_KEY_T, &ApolloRTCCMFD::menuMPTTLIDirectInput);
	RegisterFunction("PDI", OAPI_KEY_P, &ApolloRTCCMFD::menuTransferPoweredDescentToMPT);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("M49", OAPI_KEY_F, &ApolloRTCCMFD::menuMPTMEDM49);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu45[] =
	{
		{ "All/lunar stations", 0, 'L' },
		{ "Generate contacts", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu45, sizeof(mnu45) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LUN", OAPI_KEY_L, &ApolloRTCCMFD::menuNextStationContactLunar);
	RegisterFunction("GEN", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateStationContacts);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu46[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu46, sizeof(mnu46) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqCSM1Calc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu47[] =
	{
		{ "AGC State Vectors", 0, 'G' },
		{ "Landing Site Vector", 0, 'L' },
		{ "External DV Update", 0, 'E' },
		{ "Retrofire EXDV Update", 0, 'R' },
		{ "REFSMMAT Update", 0, 'H' },
		{ "SLV Navigation Update", 0, 'S' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu47, sizeof(mnu47) / sizeof(MFDBUTTONMENU));

	RegisterFunction("SV", OAPI_KEY_G, &ApolloRTCCMFD::menuSetStateVectorMenu);
	RegisterFunction("RLS", OAPI_KEY_L, &ApolloRTCCMFD::menuSetLSUplinkPage);
	RegisterFunction("P30", OAPI_KEY_E, &ApolloRTCCMFD::menuSetP30UplinkPage);
	RegisterFunction("RET", OAPI_KEY_R, &ApolloRTCCMFD::menuSetRetrofireEXDVUplinkPage);
	RegisterFunction("REF", OAPI_KEY_H, &ApolloRTCCMFD::menuSetREFSMMATUplinkPage);
	RegisterFunction("SLV", OAPI_KEY_S, &ApolloRTCCMFD::menuSetSLVNavigationUpdatePage);

	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMenu);


	static const MFDBUTTONMENU mnu48[] =
	{
		{ "Set Time", 0, 'G' },
		{ "State Vector Slot", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Set Target", 0, 'T' },
		{ "Calculate State Vector", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Upload State Vector", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu48, sizeof(mnu48) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSVTime);
	RegisterFunction("SLT", OAPI_KEY_D, &ApolloRTCCMFD::menuSwitchSVSlot);
	RegisterFunction("", OAPI_KEY_N, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_M, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSVCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_K, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_L, &ApolloRTCCMFD::menuSVUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu49[] =
	{
		{ "Selen. latitude", 0, 'A' },
		{ "Selen. longitude", 0, 'O' },
		{ "Selen. radius", 0, 'L' },
		{ "Set Target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate LS Coordinates", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu49, sizeof(mnu49) / sizeof(MFDBUTTONMENU));

	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuLSLat);
	RegisterFunction("LNG", OAPI_KEY_O, &ApolloRTCCMFD::menuLSLng);
	RegisterFunction("RAD", OAPI_KEY_L, &ApolloRTCCMFD::menuLSRadius);
	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLSCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu50[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate LS Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink RLS", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu50, sizeof(mnu50) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLSUplinkCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuLSUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu51[] =
	{
		{ "Manual TIG", 0, 'T' },
		{ "Manual DV", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate P30 Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink Ext DV", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu51, sizeof(mnu51) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuP30UplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuP30Uplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu52[] =
	{
		{ "Manual TIG", 0, 'T' },
		{ "Manual DV", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate P30 Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink Ext DV", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu52, sizeof(mnu52) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIG", OAPI_KEY_T, &ApolloRTCCMFD::menuManPADTIG);
	RegisterFunction("DV", OAPI_KEY_D, &ApolloRTCCMFD::menuManPADDV);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuRetrofireEXDVUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuRetrofireEXDVUplink);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu53[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink option", 0, 'K' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu53, sizeof(mnu53) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuREFSMMATUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::UploadREFSMMAT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu54[] =
	{
		{ "Maneuver table", 0, 'E' },
		{ "Plan number", 0, 'N' },
		{ "GET for deletion", 0, 'D' },
		{ "Select Thruster", 0, 'T' },
		{ "Attitude mode", 0, 'A' },
		{ "Ullage options", 0, 'U' },

		{ "Calc maneuver", 0, 'C' },
		{ "Iteration flag", 0, 'I' },
		{ "DPS 10% time", 0, 'P' },
		{ "DPS scale factor", 0, 'S' },
		{ "Time flag", 0, 'F' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu54, sizeof(mnu54) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_E, &ApolloRTCCMFD::menuCycleTITable);
	RegisterFunction("PLN", OAPI_KEY_N, &ApolloRTCCMFD::menuSetTIPlanNumber);
	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuTIDeleteGET);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseTIThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleTIAttitude);
	RegisterFunction("ULL", OAPI_KEY_U, &ApolloRTCCMFD::menuTIUllageOption);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferTIToMPT);
	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleTIIterationFlag);
	RegisterFunction("10P", OAPI_KEY_P, &ApolloRTCCMFD::menuTIDPSTenPercentTime);
	RegisterFunction("DPS", OAPI_KEY_S, &ApolloRTCCMFD::menuTIDPSScaleFactor);
	RegisterFunction("TIM", OAPI_KEY_F, &ApolloRTCCMFD::menuCycleTITimeFlag);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLambertPage);


	static const MFDBUTTONMENU mnu55[] =
	{
		{ "GET for deletion", 0, 'D' },
		{ "Select Thruster", 0, 'T' },
		{ "Attitude option", 0, 'A' },
		{ "", 0, ' ' },
		{ "Iteration flag", 0, 'I' },
		{ "Time flag", 0, 'G' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Transfer to MPT", 0, 'C' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu55, sizeof(mnu55) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DEL", OAPI_KEY_D, &ApolloRTCCMFD::menuM70DeleteGET);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseSPQDKIThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuM70CycleAttitude);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuM70CycleIterationFlag);
	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuM70CycleTimeFlag);
	
	RegisterFunction("10P", OAPI_KEY_F, &ApolloRTCCMFD::menuM70DPSTenPercentTime);
	RegisterFunction("DPS", OAPI_KEY_P, &ApolloRTCCMFD::menuM70DPSScaleFactor);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferSPQorDKIToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuBackToSPQorDKIPage);


	static const MFDBUTTONMENU mnu56[] =
	{
		{ "CSM or LEM", 0, 'V' },
		{ "Input option", 0, 'R' },
		{ "Time of ignition", 0, 'I' },
		{ "Thruster", 0, 'T' },
		{ "Attitude mode", 0, 'G' },
		{ "Input attitude", 0, 'A' },

		{ "Choose thruster", 0, 'C' },
		{ "Heads up/down", 0, 'H' },
		{ "DPS DT at 10%", 0, 'P' },
		{ "DPS thrust", 0, 'F' },
		{ "Move to MPT", 0, 'M' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu56, sizeof(mnu56) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MPT", OAPI_KEY_V, &ApolloRTCCMFD::menuMPTDirectInputMPTCode);
	RegisterFunction("REP", OAPI_KEY_R, &ApolloRTCCMFD::menuMPTDirectInputReplaceCode);
	RegisterFunction("TIG", OAPI_KEY_I, &ApolloRTCCMFD::menuMPTDirectInputTIG);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseMPTDirectInputThruster);
	RegisterFunction("ATT", OAPI_KEY_G, &ApolloRTCCMFD::menuMPTDirectInputAttitude);
	RegisterFunction("BPA", OAPI_KEY_A, &ApolloRTCCMFD::menuMPTDirectInputBurnParameters);

	RegisterFunction("COO", OAPI_KEY_C, &ApolloRTCCMFD::menuMPTDirectInputCoord);
	RegisterFunction("HEA", OAPI_KEY_H, &ApolloRTCCMFD::menuMPTDirectInputHeadsUpDown);
	RegisterFunction("10P", OAPI_KEY_P, &ApolloRTCCMFD::menuMPTDirectInputDPSTenPercentTime);
	RegisterFunction("THR", OAPI_KEY_F, &ApolloRTCCMFD::menuMPTDirectInputDPSScaleFactor);
	RegisterFunction("PAG", OAPI_KEY_M, &ApolloRTCCMFD::menuMPTDirectInputSecondPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);


	static const MFDBUTTONMENU mnu57[] =
	{
		{ "Table code", 0, 'V' },
		{ "Replace code", 0, 'R' },
		{ "Select thruster", 0, 'T' },
		{ "Attitude mode", 0, 'A' },
		{ "Ullage DT", 0, 'U' },
		{ "Ullage thrusters", 0, 'W' },

		{ "Iteration flag", 0, 'I' },
		{ "DPS 10% time", 0, 'P' },
		{ "DPS thrust scale", 0, 'S' },
		{ "Time flag", 0, 'F' },
		{ "Transfer to MPT", 0, 'C' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu57, sizeof(mnu57) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_V, &ApolloRTCCMFD::menuCycleGPMTable);
	RegisterFunction("REP", OAPI_KEY_R, &ApolloRTCCMFD::menuGPMReplaceCode);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseGPMThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleGPMAttitude);
	RegisterFunction("UDT", OAPI_KEY_U, &ApolloRTCCMFD::menuGPMUllageDT);
	RegisterFunction("UTH", OAPI_KEY_W, &ApolloRTCCMFD::menuGPMUllageThrusters);
	
	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleGPMIterationFlag);
	RegisterFunction("DT", OAPI_KEY_P, &ApolloRTCCMFD::menuGPMDPSTenPercentDeltaT);
	RegisterFunction("PCT", OAPI_KEY_S, &ApolloRTCCMFD::menuGPMDPSThrustScaling);
	RegisterFunction("TIM", OAPI_KEY_F, &ApolloRTCCMFD::menuCycleGPMTimeFlag);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferGPMToMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetOrbAdjPage);


	static const MFDBUTTONMENU mnu58[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu58, sizeof(mnu58) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_R, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::CheckoutMonitorCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu59[] =
	{
		{ "CSM or LEM MPT", 0, 'T' },
		{ "Choose vessel", 0, 'V' },
		{ "CSM weight", 0, 'C' },
		{ "S-IVB weight", 0, 'S' },
		{ "LM total weight", 0, 'L' },
		{ "LM ascent weight", 0, 'F' },

		{ "Choose config", 0, 'D' },
		{ "Auto update of parameters", 0, 'U' },
		{ "Config update", 0, 'M' },
		{ "Mass update", 0, 'N' },
		{ "Trajectory update", 0, 'P' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu59, sizeof(mnu59) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_T, &ApolloRTCCMFD::menuMPTInitM50M55Table);
	RegisterFunction("VEH", OAPI_KEY_V, &ApolloRTCCMFD::menuMPTInitM50M55Vehicle);
	RegisterFunction("CSM", OAPI_KEY_C, &ApolloRTCCMFD::menuMPTInitM50CSMWT);
	RegisterFunction("S4B", OAPI_KEY_S, &ApolloRTCCMFD::menuMPTInitM50SIVBWT);
	RegisterFunction("LM", OAPI_KEY_L, &ApolloRTCCMFD::menuMPTInitM50LMWT);
	RegisterFunction("LMA", OAPI_KEY_F, &ApolloRTCCMFD::menuMPTInitM50LMAscentWT);

	RegisterFunction("CFG", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTInitM55Config);
	RegisterFunction("AUT", OAPI_KEY_U, &ApolloRTCCMFD::menuMPTInitAutoUpdate);
	RegisterFunction("M55", OAPI_KEY_M, &ApolloRTCCMFD::menuMPTM55Update);
	RegisterFunction("M50", OAPI_KEY_N, &ApolloRTCCMFD::menuMPTM50Update);
	RegisterFunction("TUP", OAPI_KEY_P, &ApolloRTCCMFD::menuMPTTrajectoryUpdate);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);


	static const MFDBUTTONMENU mnu60[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate maneuver", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Transfer to MPT", 0, 'U' },
		{ "Back to calc page", 0, 'B' },
	};

	RegisterPage(mnu60, sizeof(mnu60) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLDPPCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_U, &ApolloRTCCMFD::menuSetSPQorDKIRTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDescPlanCalcPage);


	static const MFDBUTTONMENU mnu61[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink to LGC", 0, 'U' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu61, sizeof(mnu61) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTLANDUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::menuTLANDUpload);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu62[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate times", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu62, sizeof(mnu62) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuSunriseSunsetTimesCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu63[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate times", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu63, sizeof(mnu63) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuMoonriseMoonsetTimesCalc);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu64[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu64, sizeof(mnu64) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu65[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu65, sizeof(mnu65) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu66[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Cycle through pages", 0, 'P' },
		{ "Calc tradeoff display", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu66, sizeof(mnu66) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MOD", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetRTETradeoffMode);
	RegisterFunction("REM", OAPI_KEY_H, &ApolloRTCCMFD::menuSetRTETradeoffRemoteEarthPage);
	RegisterFunction("SIT", OAPI_KEY_G, &ApolloRTCCMFD::menuSetRTETradeoffSite);
	RegisterFunction("TV", OAPI_KEY_U, &ApolloRTCCMFD::menuSetRTETradeoffVectorTime);
	RegisterFunction("MIN", OAPI_KEY_V, &ApolloRTCCMFD::menuSetRTETradeoffT0MinTime);
	RegisterFunction("MAX", OAPI_KEY_L, &ApolloRTCCMFD::menuSetRTETradeoffT0MaxTime);

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::menuCycleRTETradeoffPage);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalcRTETradeoff);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("ENT", OAPI_KEY_E, &ApolloRTCCMFD::menuSetRTETradeoffEntryProfile);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetEntryPage);


	static const MFDBUTTONMENU mnu67[] =
	{
		{ "Generate DMT", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu67, sizeof(mnu67) / sizeof(MFDBUTTONMENU));

	RegisterFunction("U20", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateDMT);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu68[] =
	{
		{ "Generate DMT", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu68, sizeof(mnu68) / sizeof(MFDBUTTONMENU));

	RegisterFunction("U20", OAPI_KEY_G, &ApolloRTCCMFD::menuGenerateDMT);
	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu69[] =
	{
		{ "Coelliptic DH", 0, 'D' },
		{ "Elevation angle at TPI", 0, 'E' },
		{ "Angle between TPI and TPF", 0, 'W' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "TPI time", 0, 'Q' },

		{ "CDH option", 0, 'C' },
		{ "CDH value", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu69, sizeof(mnu69) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DH", OAPI_KEY_D, &ApolloRTCCMFD::SPQDHdialogue);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetSPQElevation);
	RegisterFunction("WT", OAPI_KEY_W, &ApolloRTCCMFD::menuSetSPQTerminalPhaseAngle);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("TPI", OAPI_KEY_Q, &ApolloRTCCMFD::menuSetSPQTPIDefinitionValue);

	RegisterFunction("CDH", OAPI_KEY_C, &ApolloRTCCMFD::menuCycleSPQCDHPoint);
	RegisterFunction("VAL", OAPI_KEY_D, &ApolloRTCCMFD::menuSPQCDHValue);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetSPQPage);


	static const MFDBUTTONMENU mnu70[] =
	{
		{ "Coelliptic DH", 0, 'D' },
		{ "Elevation angle at TPI", 0, 'E' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu70, sizeof(mnu70) / sizeof(MFDBUTTONMENU));

	RegisterFunction("DH", OAPI_KEY_D, &ApolloRTCCMFD::DKIDHdialogue);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetDKIElevation);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetDKIPage);


	static const MFDBUTTONMENU mnu71[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "Calculate longitude from GET", 0, 'G' },
		{ "Calculate GET from longitude", 0, 'L' },
		{ "Calc apo/peri values from GET", 0, 'R' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu71, sizeof(mnu71) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_U, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("GETL", OAPI_KEY_G, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETL);
	RegisterFunction("L", OAPI_KEY_L, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsL);
	RegisterFunction("GETR", OAPI_KEY_R, &ApolloRTCCMFD::menuSetFIDOOrbitDigitalsGETBV);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu72[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu72, sizeof(mnu72) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqLM1Calc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu73[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu73, sizeof(mnu73) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqCSM2Calc);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu74[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu74, sizeof(mnu74) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CyclePredSiteAcqPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::PredSiteAcqLM2Calc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu75[] =
	{
		{ "General MED Input", 0, 'G' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu75, sizeof(mnu75) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MED", OAPI_KEY_G, &ApolloRTCCMFD::menuGeneralMEDRequest);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu76[] =
	{
		{ "Table code", 0, 'V' },
		{ "Replace code", 0, 'R' },
		{ "Maneuver number", 0, 'N' },
		{ "Select thruster", 0, 'T' },
		{ "Attitude mode", 0, 'A' },
		{ "Ullage thrusters and DT", 0, 'U' },

		{ "Iteration flag", 0, 'I' },
		{ "DPS 10% time", 0, 'P' },
		{ "DPS thrust scale", 0, 'S' },
		{ "Time flag", 0, 'F' },
		{ "Transfer to MPT", 0, 'C' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu76, sizeof(mnu76) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAB", OAPI_KEY_V, &ApolloRTCCMFD::menuCycleLOIMCCTable);
	RegisterFunction("REP", OAPI_KEY_R, &ApolloRTCCMFD::menuLOIMCCReplaceCode);
	RegisterFunction("NUM", OAPI_KEY_N, &ApolloRTCCMFD::menuLOIMCCManeuverNumber);
	RegisterFunction("THR", OAPI_KEY_T, &ApolloRTCCMFD::menuChooseLOIMCCThruster);
	RegisterFunction("ATT", OAPI_KEY_A, &ApolloRTCCMFD::menuCycleLOIMCCAttitude);
	RegisterFunction("UDT", OAPI_KEY_U, &ApolloRTCCMFD::menuLOIMCCUllageThrustersDT);

	RegisterFunction("ITE", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleLOIMCCIterationFlag);
	RegisterFunction("DT", OAPI_KEY_P, &ApolloRTCCMFD::menuLOIMCCDPSTenPercentDeltaT);
	RegisterFunction("PCT", OAPI_KEY_S, &ApolloRTCCMFD::menuLOIMCCDPSThrustScaling);
	RegisterFunction("TIM", OAPI_KEY_F, &ApolloRTCCMFD::menuCycleLOIMCCTimeFlag);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTransferLOIMCCtoMPT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuBacktoLOIorMCCPage);


	static const MFDBUTTONMENU mnu77[] =
	{
		{ "Cycle between SFP blocks", 0, 'D' },
		{ "Change entry in SFP", 0, 'S' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu77, sizeof(mnu77) / sizeof(MFDBUTTONMENU));

	RegisterFunction("F31", OAPI_KEY_D, &ApolloRTCCMFD::menuCycleSFPDisplay);
	RegisterFunction("F32", OAPI_KEY_S, &ApolloRTCCMFD::menuAlterationSFPData);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu78[] =
	{
		{ "Transfer plan to SFP table", 0, 'T' },
		{ "Delete column", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate maneuver", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu78, sizeof(mnu78) / sizeof(MFDBUTTONMENU));

	RegisterFunction("F30", OAPI_KEY_T, &ApolloRTCCMFD::menuTransferMCCPlanToSFP);
	RegisterFunction("F26", OAPI_KEY_D, &ApolloRTCCMFD::menuDeleteMidcourseColumn);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuTLCCCalc);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_E, &ApolloRTCCMFD::menuMCCTransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuMidcoursePage);


	static const MFDBUTTONMENU mnu79[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu79, sizeof(mnu79) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu80[] =
	{
		{ "Azimuth constraints", 0, 'A' },
		{ "TLC times constraints", 0, 'T' },
		{ "Reentry constraints", 0, 'R' },
		{ "Height limits", 0, 'H' },
		{ "Latitude bias", 0, 'L' },
		{ "Max powered incl", 0, 'C' },

		{ "LOI ellipse heights", 0, 'K' },
		{ "DOI ellipse heights", 0, 'D' },
		{ "Revs in LOI/DOI ellipse", 0, 'O' },
		{ "True anomaly at PDI", 0, 'S' },
		{ "Revs for LOPC calc", 0, 'P' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu80, sizeof(mnu80) / sizeof(MFDBUTTONMENU));

	RegisterFunction("F22", OAPI_KEY_A, &ApolloRTCCMFD::menuSetTLMCCAzimuthConstraints);
	RegisterFunction("F23", OAPI_KEY_T, &ApolloRTCCMFD::menuSetTLMCCTLCTimesConstraints);
	RegisterFunction("F24", OAPI_KEY_R, &ApolloRTCCMFD::menuSetTLMCCReentryContraints);
	RegisterFunction("F29", OAPI_KEY_H, &ApolloRTCCMFD::menuSetTLMCCPericynthionHeightLimits);
	RegisterFunction("LAT", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTLMCCLatitudeBias);
	RegisterFunction("INC", OAPI_KEY_C, &ApolloRTCCMFD::menuSetTLMCCMaxInclination);

	RegisterFunction("LOI", OAPI_KEY_K, &ApolloRTCCMFD::menuSetTLMCCLOIEllipseHeights);
	RegisterFunction("DOI", OAPI_KEY_D, &ApolloRTCCMFD::menuSetTLMCCDOIEllipseHeights);
	RegisterFunction("REV", OAPI_KEY_O, &ApolloRTCCMFD::menuSetTLMCCLOIDOIRevs);
	RegisterFunction("ROT", OAPI_KEY_S, &ApolloRTCCMFD::menuSetTLMCCLSRotation);
	RegisterFunction("PC", OAPI_KEY_P, &ApolloRTCCMFD::menuSetTLMCCLOPCRevs);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuMidcoursePage);


	static const MFDBUTTONMENU mnu81[] =
	{
		{ "Node conv. option", 0, 'O' },
		{ "Node conv.n GET", 0, 'G' },
		{ "Node conv. latitude", 0, 'A' },
		{ "Node conv. longitude", 0, 'L' },
		{ "Nodal height", 0, 'H' },
		{ "", 0, ' ' },

		{ "Convert", 0, 'C' },
		{ "Send node to SFP 2", 0, 'S' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu81, sizeof(mnu81) / sizeof(MFDBUTTONMENU));

	RegisterFunction("OPT", OAPI_KEY_O, &ApolloRTCCMFD::menuCycleNodeConvOption);
	RegisterFunction("GET", OAPI_KEY_G, &ApolloRTCCMFD::menuSetNodeConvGET);
	RegisterFunction("LAT", OAPI_KEY_A, &ApolloRTCCMFD::menuSetNodeConvLat);
	RegisterFunction("LNG", OAPI_KEY_L, &ApolloRTCCMFD::menuSetNodeConvLng);
	RegisterFunction("HE", OAPI_KEY_H, &ApolloRTCCMFD::menuSetNodeConvHeight);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuNodeConvCalc);
	RegisterFunction("SFP", OAPI_KEY_S, &ApolloRTCCMFD::menuSendNodeToSFP);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUtilityMenu);


	static const MFDBUTTONMENU mnu82[] =
	{
		{ "Apolune at LLS", 0, 'A' },
		{ "Perilune at LLS", 0, 'P' },
		{ "Angle of perilune from LLS", 0, 'D' },
		{ "Revs in LPO1", 0, 'R' },
		{ "Revs in LPO2", 0, 'S' },
		{ "True anomaly at LOI", 0, 'E' },

		{ "Height bias at DOI", 0, 'H' },
		{ "Plane for inters. soln", 0, 'I' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu82, sizeof(mnu82) / sizeof(MFDBUTTONMENU));

	RegisterFunction("HA", OAPI_KEY_A, &ApolloRTCCMFD::menuSetLOI_HALLS);
	RegisterFunction("HP", OAPI_KEY_P, &ApolloRTCCMFD::menuSetLOI_HPLLS);
	RegisterFunction("DW", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLOIDW);
	RegisterFunction("R1", OAPI_KEY_R, &ApolloRTCCMFD::menuSetLOIRevs1);
	RegisterFunction("R2", OAPI_KEY_S, &ApolloRTCCMFD::menuSetLOIRevs2);
	RegisterFunction("ETA", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLOIEta1);

	RegisterFunction("DHB", OAPI_KEY_H, &ApolloRTCCMFD::menuSetLOIDHBias);
	RegisterFunction("PLA", OAPI_KEY_I, &ApolloRTCCMFD::menuCycleLOIInterSolnFlag);
	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLOIPage);


	static const MFDBUTTONMENU mnu83[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu83, sizeof(mnu83) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_O, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLOICalc);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("MPT", OAPI_KEY_E, &ApolloRTCCMFD::menuLOITransferPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLOIPage);


	static const MFDBUTTONMENU mnu84[] =
	{
		{ "Choose ullage DT", 0, 'U' },
		{ "Choose ullage thrusters", 0, 'T' },
		{ "REFSMMAT", 0, 'R' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu84, sizeof(mnu84) / sizeof(MFDBUTTONMENU));

	RegisterFunction("UDT", OAPI_KEY_U, &ApolloRTCCMFD::menuMPTDirectInputUllageDT);
	RegisterFunction("UTH", OAPI_KEY_T, &ApolloRTCCMFD::menuMPTDirectInputUllageThrusters);
	RegisterFunction("REF", OAPI_KEY_R, &ApolloRTCCMFD::menuMPTDirectInputREFSMMAT);
	RegisterFunction("DOC", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTDirectInputDock);
	RegisterFunction("CFG", OAPI_KEY_C, &ApolloRTCCMFD::menuMPTDirectInputFinalConfig);
	RegisterFunction("DDA", OAPI_KEY_D, &ApolloRTCCMFD::menuMPTDirectInputDeltaDockingAngle);

	RegisterFunction("TRM", OAPI_KEY_S, &ApolloRTCCMFD::menuMPTDirectInputTrimAngleInd);
	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuMPTDirectInputTransfer);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("PAG", OAPI_KEY_E, &ApolloRTCCMFD::menuSetMPTDirectInputPage);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMPTPage);


	static const MFDBUTTONMENU mnu85[] =
	{
		{ "Cycle pages", 0, 'P' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate contacts", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu85, sizeof(mnu85) / sizeof(MFDBUTTONMENU));

	RegisterFunction("PAG", OAPI_KEY_P, &ApolloRTCCMFD::CycleExpSiteAcqPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::ExpSiteAcqLMCalc);
	RegisterFunction("ADD", OAPI_KEY_A, &ApolloRTCCMFD::GroundPointTableUpdate);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu86[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate RMD", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu86, sizeof(mnu86) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::RelativeMotionDigitalsCalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu87[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Choose plan", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu87, sizeof(mnu87) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("U06", OAPI_KEY_C, &ApolloRTCCMFD::menuChooseRETPlan);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetSPQPage);


	static const MFDBUTTONMENU mnu88[] =
	{
		{ "Powered flight arc", 0, ' ' },
		{ "Powered flight time", 0, ' ' },
		{ "Insertion height", 0, ' ' },
		{ "Insertion Zdot", 0, 'L' },
		{ "Insertion Rdot", 0, 'h' },
		{ "Yaw steer capability", 0, ' ' },

		{ "Max asc stage life", 0, ' ' },
		{ "Min safe height", 0, ' ' },
		{ "LM Max DV", 0, ' ' },
		{ "CSM Max DV", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu88, sizeof(mnu88) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("VLH", OAPI_KEY_L, &ApolloRTCCMFD::menuLunarLiftoffVHorInput);
	RegisterFunction("VLV", OAPI_KEY_H, &ApolloRTCCMFD::menuLunarLiftoffVVertInput);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLLWPInitPage);


	static const MFDBUTTONMENU mnu89[] =
	{
		{ "Targeting parameters", 0, 'P' },
		{ "CSM CSI time bias", 0, ' ' },
		{ "Elevation Angle", 0, 'E' },
		{ "TP transfer angle", 0, ' ' },
		{ "TPF height offset", 0, ' ' },
		{ "TPF phase offset", 0, ' ' },

		{ "LW scan begin", 0, ' ' },
		{ "LW scan increment", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu89, sizeof(mnu89) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TAR", OAPI_KEY_P, &ApolloRTCCMFD::menuSetLaunchTargetingInitPage);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("E", OAPI_KEY_E, &ApolloRTCCMFD::menuSetLLWPElevation);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLunarLiftoffPage);


	static const MFDBUTTONMENU mnu90[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu90, sizeof(mnu90) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLunarLiftoffCalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetLunarLiftoffPage);


	static const MFDBUTTONMENU mnu91[] =
	{
		{ "Threshold time", 0, 'H' },
		{ "Vector time", 0, 'V' },
		{ "Radial velocity", 0, 'R' },
		{ "DT Ins. to TPI", 0, 'D' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate solution", 0, 'C' },
		{ "Select target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu91, sizeof(mnu91) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TTH", OAPI_KEY_H, &ApolloRTCCMFD::menuLLTPThresholdTime);
	RegisterFunction("VTI", OAPI_KEY_V, &ApolloRTCCMFD::menuLLTPVectorTime);
	RegisterFunction("RDO", OAPI_KEY_R, &ApolloRTCCMFD::menuLunarLiftoffVVertInput);
	RegisterFunction("DT", OAPI_KEY_D, &ApolloRTCCMFD::menuSetLiftoffDT);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuLLTPCalc);
	RegisterFunction("TGT", OAPI_KEY_A, &ApolloRTCCMFD::set_target);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetTargetingMenu);


	static const MFDBUTTONMENU mnu92[] =
	{
		{ "Set Target", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu92, sizeof(mnu92) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_target);
	RegisterFunction("MOD", OAPI_KEY_Q, &ApolloRTCCMFD::menuCycleDKITPIMode);
	RegisterFunction("DT", OAPI_KEY_V, &ApolloRTCCMFD::DKITPIDTDialogue);
	RegisterFunction("TPI", OAPI_KEY_L, &ApolloRTCCMFD::menuSetTPIguess);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuCalculateTPITime);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetRendezvousPage);


	static const MFDBUTTONMENU mnu93[] =
	{
		{ "Column 1", 0, ' ' },
		{ "Column 2", 0, ' ' },
		{ "Column 3", 0, ' ' },
		{ "Column 4", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu93, sizeof(mnu93) / sizeof(MFDBUTTONMENU));

	RegisterFunction("VEH", OAPI_KEY_H, &ApolloRTCCMFD::menuVectorCompareVehicle);
	RegisterFunction("TIM", OAPI_KEY_Q, &ApolloRTCCMFD::menuVectorCompareTime);
	RegisterFunction("V1", OAPI_KEY_T, &ApolloRTCCMFD::menuVectorCompareColumn1);
	RegisterFunction("V2", OAPI_KEY_Q, &ApolloRTCCMFD::menuVectorCompareColumn2);
	RegisterFunction("V3", OAPI_KEY_V, &ApolloRTCCMFD::menuVectorCompareColumn3);
	RegisterFunction("V4", OAPI_KEY_L, &ApolloRTCCMFD::menuVectorCompareColumn4);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuVectorCompareDisplayCalc);
	RegisterFunction("REF", OAPI_KEY_A, &ApolloRTCCMFD::menuVectorCompareReference);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu94[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "Calculate Uplink", 0, 'C' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Uplink option", 0, 'K' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu94, sizeof(mnu94) / sizeof(MFDBUTTONMENU));

	RegisterFunction("", OAPI_KEY_T, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_D, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_G, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("CLC", OAPI_KEY_C, &ApolloRTCCMFD::menuREFSMMATUplinkCalc);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UPL", OAPI_KEY_U, &ApolloRTCCMFD::UploadREFSMMAT);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);


	static const MFDBUTTONMENU mnu95[] =
	{
		{ "Display REFSMMAT", 0, 'T' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu95, sizeof(mnu95) / sizeof(MFDBUTTONMENU));

	RegisterFunction("MAT", OAPI_KEY_T, &ApolloRTCCMFD::menuGOSTDisplayREFSMMAT);
	RegisterFunction("ATT", OAPI_KEY_D, &ApolloRTCCMFD::menuGOSTEnterAttitude);
	RegisterFunction("ANG", OAPI_KEY_G, &ApolloRTCCMFD::menuGOSTEnterSXTData);
	RegisterFunction("BST", OAPI_KEY_E, &ApolloRTCCMFD::menuGOSTBoresightSCTCalc);
	RegisterFunction("SXT", OAPI_KEY_V, &ApolloRTCCMFD::menuGOSTSXTCalc);
	RegisterFunction("", OAPI_KEY_A, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("", OAPI_KEY_C, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_F, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_P, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("UNI", OAPI_KEY_S, &ApolloRTCCMFD::menuGOSTShowStarVector);
	RegisterFunction("LMK", OAPI_KEY_U, &ApolloRTCCMFD::menuGOSTShowLandmarkVector);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetMCCDisplaysPage);


	static const MFDBUTTONMENU mnu96[] =
	{
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },

		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "", 0, ' ' },
		{ "Back to menu", 0, 'B' },
	};

	RegisterPage(mnu96, sizeof(mnu96) / sizeof(MFDBUTTONMENU));

	RegisterFunction("TIM", OAPI_KEY_G, &ApolloRTCCMFD::menuSetSVTime);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_V, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_L, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_H, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_Q, &ApolloRTCCMFD::menuVoid);

	RegisterFunction("TGT", OAPI_KEY_T, &ApolloRTCCMFD::set_svtarget);
	RegisterFunction("CLC", OAPI_KEY_A, &ApolloRTCCMFD::menuSLVNavigationUpdateCalc);
	RegisterFunction("UPL", OAPI_KEY_G, &ApolloRTCCMFD::menuSLVNavigationUpdateUplink);
	RegisterFunction("", OAPI_KEY_S, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("", OAPI_KEY_E, &ApolloRTCCMFD::menuVoid);
	RegisterFunction("BCK", OAPI_KEY_B, &ApolloRTCCMFD::menuSetUplinkMenu);
}

bool ApolloRTCCMFDButtons::SearchForKeysInOtherPages() const
{
	return false;
}