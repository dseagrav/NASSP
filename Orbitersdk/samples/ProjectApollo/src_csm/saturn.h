/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2007

  Generic Saturn class for AGC interface. As much code as possible will be shared
  here between the SaturnV and Saturn1B.

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

#if !defined(_PA_SATURN_H)
#define _PA_SATURN_H


//
// I hate nested includes, but this is much easier than adding them to all the files
// which need them.
//

#include "PanelSDK/PanelSDK.h"

#include "connector.h"
#include "csmconnector.h"
#include "cautionwarning.h"
#include "csmcautionwarning.h"
#include "missiontimer.h"
#include "FDAI.h"
#include "dsky.h"
#include "cdu.h"
#include "IMU.h"
#include "satswitches.h"
#include "powersource.h"
#include "dockingprobe.h"
#include "pyro.h"
#include "secs.h"
#include "scs.h"
#include "csm_telecom.h"
#include "sps.h"
#include "ecs.h"
#include "csmrcs.h"
#include "ORDEAL.h"
#include "MechanicalAccelerometer.h"
#include "checklistController.h"
#include "payload.h"
#include "csmcomputer.h"
#include "qball.h"
#include "canard.h"
#include "siisystems.h"
#include "sivbsystems.h"
#include "sce.h"
#include "csmsensors.h"
#include "rhc.h"

#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"


//
// IMFD5 communication support
//

#include "IMFD/IMFD_Client.h"

class IU;
class SICSystems;

namespace mission
{
	class Mission;
};


#define RCS_SM_QUAD_A		0
#define RCS_SM_QUAD_B		1
#define RCS_SM_QUAD_C		2
#define RCS_SM_QUAD_D		3

#define RCS_CM_RING_1		4
#define RCS_CM_RING_2		5

///
/// \brief O2/H2 tank status.
/// \ingroup InternalInterface
///
typedef struct {
	double O2Tank1PressurePSI;
	double O2Tank2PressurePSI;
	double H2Tank1PressurePSI;
	double H2Tank2PressurePSI;
	double O2SurgeTankPressurePSI;
} TankPressures;

///
/// \brief O2/H2 tank quantities.
/// \ingroup InternalInterface
///
typedef struct {
	double O2Tank1Quantity;
	double O2Tank1QuantityKg;
	double O2Tank2Quantity;
	double O2Tank2QuantityKg;
	double H2Tank1Quantity;
	double H2Tank2Quantity;
} TankQuantities;

///
/// \brief Cabin atmosphere status.
/// \ingroup InternalInterface
///
typedef struct {
	double SuitTempK;
	double SuitTempF;
	double CabinTempK;
	double CabinTempF;
	double CabinPressureMMHG;
	double SuitPressureMMHG;
	double SuitReturnPressureMMHG;
	double CabinPressurePSI;
	double SuitPressurePSI;
	double SuitReturnPressurePSI;
	double SuitCO2MMHG;
	double CabinCO2MMHG;
	double CabinRegulatorFlowLBH;
	double O2DemandFlowLBH;
	double DirectO2FlowLBH;
	double SuitTestFlowLBH;
	double CabinRepressFlowLBH;
	double EmergencyCabinRegulatorFlowLBH;
	double O2FlowXducerLBH;
	double O2RepressPressurePSI;
	double TunnelPressurePSI;
} AtmosStatus;

///
/// \brief Displayed atmosphere status.
/// \ingroup InternalInterface
///
typedef struct {
	double DisplayedO2FlowLBH;
	double DisplayedSuitComprDeltaPressurePSI;
	double DisplayedEcsRadTempPrimOutletMeterTemperatureF;
} DisplayedAtmosStatus;

///
/// \brief Primary ECS cooling status.
/// \ingroup InternalInterface
///
typedef struct {
	double RadiatorInletPressurePSI;
	double RadiatorInletTempF;
	double RadiatorOutletTempF;
	double EvaporatorOutletTempF;
	double EvaporatorSteamPressurePSI;
	double AccumulatorQuantityPercent;
} PrimECSCoolingStatus;

///
/// \brief Secondary ECS cooling status.
/// \ingroup InternalInterface
///
typedef struct {
	double RadiatorInletPressurePSI;
	double RadiatorInletTempF;
	double RadiatorOutletTempF;
	double EvaporatorOutletTempF;
	double EvaporatorSteamPressurePSI;
	double AccumulatorQuantityPercent;
} SecECSCoolingStatus;

///
/// \brief ECS water status.
/// \ingroup InternalInterface
///
typedef struct {
	double PotableH2oTankQuantityPercent;
	double WasteH2oTankQuantityPercent;
} ECSWaterStatus;

///
/// \brief General ECS status.
/// \ingroup InternalInterface
///
typedef struct {
	int crewNumber;
	int crewStatus;
	double PrimECSHeating;
	double PrimECSTestHeating;
	double SecECSHeating;
	double SecECSTestHeating;
} ECSStatus;

///
/// \brief Main bus status.
/// \ingroup InternalInterface
///
typedef struct {
	double MainBusAVoltage;
	double MainBusBVoltage;
	bool Enabled_DC_A_CWS;
	bool Enabled_DC_B_CWS;
	bool Reset_DC_A_CWS;
	bool Reset_DC_B_CWS;
	bool Fc_Disconnected;
} MainBusStatus;

///
/// \brief Battery bus status.
/// \ingroup InternalInterface
///
typedef struct {
	double BatBusAVoltage;
	double BatBusACurrent;
	double BatBusBVoltage;
	double BatBusBCurrent;
	double BatteryRelayBusVoltage;
} BatteryBusStatus;

///
/// \brief Battery status.
/// \ingroup InternalInterface
///
typedef struct {
	double BatteryAVoltage;
	double BatteryACurrent;
	double BatteryBVoltage;
	double BatteryBCurrent;
	double BatteryCVoltage;
	double BatteryCCurrent;
	double BatteryChargerCurrent;
} BatteryStatus;

///
/// \brief Apollo Guidance Computer warning status.
/// \ingroup InternalInterface
///
typedef struct {
	bool DSKYWarn;
	bool ISSWarning;
	bool PGNSWarning;
} AGCWarningStatus;

///
/// \brief Pyro status.
/// \ingroup InternalInterface
///
typedef struct {
	double BusAVoltage;
	double BusBVoltage;
} PyroStatus;

///
/// \brief SECS status.
/// \ingroup InternalInterface
///
typedef struct {
	double BusAVoltage;
	double BusBVoltage;
	bool CMRCSPressureSignalA;
	bool CMSMSepRelayCloseA;
	bool SLASepRelayA;
	bool RCSActivateSignalA;
	bool EDSAbortLogicOutputA;
	bool FwdHeatshieldJettA;
	bool CMRCSPressureSignalB;
	bool CMSMSepRelayCloseB;
	bool SLASepRelayB;
	bool RCSActivateSignalB;
	bool EDSAbortLogicOutputB;
	bool FwdHeatshieldJettB;
	bool DrogueSepRelayA;
	bool DrogueSepRelayB;
	bool MainChuteDiscRelayA;
	bool MainChuteDiscRelayB;
	bool MainDeployRelayA;
	bool MainDeployRelayB;
	bool EDSAbortLogicInput1;
	bool EDSAbortLogicInput2;
	bool EDSAbortLogicInput3;
	bool CrewAbortA;
	bool CrewAbortB;
	bool CSMLEMLockRingSepRelaySignalA;
	bool CSMLEMLockRingSepRelaySignalB;
} SECSStatus;

///
/// \brief RCS status
/// \ingroup InternalInterface
///
typedef struct {
	double PackageTempF;
	double HeliumTempF;
	double HeliumPressurePSI;
	double PropellantPressurePSI;
} RCSStatus;

///
/// \brief SPS status.
/// \ingroup InternalInterface
///
typedef struct {
	double chamberPressurePSI;
	double PropellantLineTempF;
	double OxidizerLineTempF;
} SPSStatus;

///
/// \brief Generic Saturn launch vehicle class.
/// \ingroup Saturns
///
class Saturn: public ProjectApolloConnectorVessel, public PanelSwitchListener {

public:

	//
	// First define some structures and values specific to the Saturn class. This ensures we don't
	// have problems with using the same names for different purposes in other classes.
	//
	
	///
	/// This enum gives IDs for the surface bitmaps. We don't use #defines because we want
	/// to automatically calculate the maximum number of bitmaps.
	///
	/// \ingroup Saturns
	///
	enum SurfaceID
	{
		//
		// First value in the enum must be set to one. Entry zero is not
		// used.
		//
		SRF_INDICATOR							=	 1,
		SRF_NEEDLE,
		SRF_DIGITAL,
		SRF_SWITCHUP,
		SRF_SWITCHLEVER,
		SRF_SWITCHGUARDS,
		SRF_ABORT,
		SRF_LV_ENG,
		SRF_ALTIMETER,
		SRF_ALTIMETER2,
		SRF_THRUSTMETER,
		SRF_SEQUENCERSWITCHES,
		SRF_LMTWOPOSLEVER,
		SRF_MASTERALARM_BRIGHT,
		SRF_DSKY,
		SRF_THREEPOSSWITCH,
		SRF_MFDFRAME,
		SRF_MFDPOWER,
		SRF_ROTATIONALSWITCH,
		SRF_SUITCABINDELTAPMETER,
		SRF_THREEPOSSWITCH305,
		SRF_DSKYDISP,
		SRF_FDAI,
		SRF_FDAIROLL,
		SRF_CWSLIGHTS,
		SRF_EVENT_TIMER_DIGITS,
		SRF_DSKYKEY,
		SRF_ECSINDICATOR,
		SRF_SWITCHUPSMALL,
		SRF_CMMFDFRAME,
		SRF_COAS,
		SRF_THUMBWHEEL_SMALLFONTS,
		SRF_CIRCUITBRAKER,
		SRF_CIRCUITBRAKER_YELLOW,
		SRF_THREEPOSSWITCH20,
		SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL,
		SRF_THREEPOSSWITCH30,
		SRF_SWITCH20,
		SRF_SWITCH30,
		SRF_SWITCH20LEFT,
		SRF_THREEPOSSWITCH20LEFT,
		SRF_GUARDEDSWITCH20,
		SRF_SWITCHGUARDPANEL15,
		SRF_THUMBWHEEL_SMALLFONTS_DIAGONAL_LEFT,
		SRF_THREEPOSSWITCH30LEFT,
		SRF_SWITCH30LEFT,
		SRF_THREEPOSSWITCH305LEFT,
		SRF_SWITCH305LEFT,
		SRF_FDAIPOWERROTARY,
		SRF_DIRECTO2ROTARY,
		SRF_ECSGLYCOLPUMPROTARY,
		SRF_GTACOVER,
		SRF_DCVOLTS,
		SRF_DCVOLTS_PANEL101,
		SRF_ACVOLTS,
		SRF_DCAMPS,
		SRF_POSTLDGVENTVLVLEVER,
		SRF_SPSMAXINDICATOR,
		SRF_ECSROTARY,
		SRF_CSM_MNPNL_WDW_LES,
		SRF_CSM_RNDZ_WDW_LES,
		SRF_CSM_RIGHT_WDW_LES,
		SRF_CSM_LEFT_WDW_LES,
		SRF_GLYCOLLEVER,
		SRF_FDAIOFFFLAG,
		SRF_FDAINEEDLES,
		SRF_THUMBWHEEL_LARGEFONTS,
		SRF_SPS_FONT_WHITE,
		SRF_SPS_FONT_BLACK,
		SRF_BORDER_31x31,
		SRF_BORDER_34x29,
		SRF_BORDER_34x61,
		SRF_BORDER_55x111,
		SRF_BORDER_46x75,
		SRF_BORDER_39x38,
		SRF_BORDER_92x40,
		SRF_BORDER_34x33,
		SRF_BORDER_29x29,
		SRF_BORDER_34x31,
		SRF_BORDER_50x158,
		SRF_BORDER_38x52,
		SRF_BORDER_34x34,
		SRF_BORDER_90x90,
		SRF_BORDER_84x84,
		SRF_BORDER_70x70,
		SRF_BORDER_23x20,
		SRF_BORDER_78x78,
		SRF_BORDER_32x160,
		SRF_BORDER_72x72,
		SRF_BORDER_75x64,
		SRF_BORDER_34x39,
		SRF_BORDER_58x58,
		SRF_BORDER_160x32,
		SRF_BORDER_57x57,
		SRF_BORDER_47x47,
		SRF_BORDER_48x48,
		SRF_BORDER_65x65,
		SRF_BORDER_87x111,
		SRF_BORDER_23x23,
		SRF_BORDER_118x118,
		SRF_BORDER_38x38,
		SRF_BORDER_116x116,
		SRF_THUMBWHEEL_SMALL,
		SRF_THUMBWHEEL_LARGEFONTSINV,
		SRF_SWLEVERTHREEPOS,
		SRF_ORDEAL_ROTARY,
		SRF_LV_ENG_S1B,
		SRF_SPSMININDICATOR,
		SRF_SPS_INJ_VLV,
		SRF_SM_RCS_MODE,
		SRF_THUMBWHEEL_GPI_PITCH,
		SRF_THUMBWHEEL_GPI_YAW,
		SRF_THC,
		SRF_EMS_LIGHTS,
		SRF_SUITRETURN_LEVER,
		SRF_CABINRELIEFUPPERLEVER,
		SRF_CABINRELIEFLOWERLEVER,
		SRF_CABINRELIEFGUARDLEVER,
		SRF_OPTICS_HANDCONTROLLER,
		SRF_MARK_BUTTONS,
		SRF_THREEPOSSWITCHSMALL,
		SRF_OPTICS_DSKY,
		SRF_MINIMPULSE_HANDCONTROLLER,
		SRF_EMS_SCROLL_LEO,
		SRF_EMS_SCROLL_BORDER,
		SRF_EMS_RSI_BKGRND,
		SRF_EMSDVSETSWITCH,
		SRF_OXYGEN_SURGE_TANK_VALVE,
		SRF_GLYCOL_TO_RADIATORS_KNOB,
		SRF_ACCUM_ROTARY,
		SRF_GLYCOL_ROTARY,
		SRF_PRESS_RELIEF_VALVE,
		SRF_CABIN_REPRESS_VALVE,
		SRF_SELECTOR_INLET_ROTARY,
		SRF_SELECTOR_OUTLET_ROTARY,
		SRF_EMERGENCY_PRESS_ROTARY,
		SRF_SUIT_FLOW_CONTROL_LEVER,
		SRF_CSM_SEC_CABIN_TEMP_VALVE, 
		SRF_CSM_FOOT_PREP_WATER_LEVER,
		SRF_CSM_LM_TUNNEL_VENT_VALVE,
		SRF_CSM_WASTE_MGMT_ROTARY,
		SRF_BORDER_45x36,
		SRF_BORDER_17x36,                         
		SRF_BORDER_33x43,                        
		SRF_BORDER_36x17,                         
		SRF_BORDER_38x37,                         
		SRF_BORDER_150x80,                      
		SRF_BORDER_200x80,                   
		SRF_BORDER_72x109,        
		SRF_CSM_DEMAND_REG_ROTARY,
		SRF_CSM_SUIT_TEST_LEVER,
		SRF_CSM_GEAR_BOX_ROTARY,
		SRF_CSM_PUMP_HANDLE_ROTARY,
		SRF_CSM_VENT_VALVE_HANDLE,
		SRF_CSM_PUMP_HANDLE_ROTARY_OPEN,
		SRF_BORDER_200x300,
		SRF_BORDER_150x200,
		SRF_BORDER_240x240,
		SRF_CSM_PANEL_351_SWITCH,
		SRF_CSM_PANEL_600,
		SRF_CSM_PANEL_600_SWITCH,
		SRF_CSM_PANEL_382_COVER,
		SRF_CSM_WASTE_DISPOSAL_ROTARY,
		SRF_THREEPOSSWITCH90_LEFT,
		SRF_EMS_SCROLL_BUG,
		SRF_BORDER_55x91,
		SRF_SWITCH90,
		SRF_BORDER_673x369,
		SRF_BORDER_673x80,
		SRF_BORDER_110x29,
		SRF_BORDER_29x30,
		SRF_BORDER_62x129,
		SRF_BORDER_194x324,
		SRF_BORDER_36x69,
		SRF_BORDER_62x31,
		SRF_CSM_CABINPRESSTESTSWITCH,
		SRF_ORDEAL_PANEL,
		SRF_DIGITAL2,
		SRF_CSM_TELESCOPECOVER,
		SRF_CSM_SEXTANTCOVER,
		SRF_CWS_GNLIGHTS,
		SRF_BORDER_45x49,
		SRF_BORDER_28x32,
		SRF_EVENT_TIMER_DIGITS90,
		SRF_DIGITAL90,
		SRF_CSM_PRESS_EQUAL_HANDLE,
		SRF_CSM_PANEL_181,
		SRF_CSM_PANEL_277,
		SRF_CSM_PANEL_278_CSM112,
		SRF_CSM_PANEL_278_CSM114,
		SRF_INDICATOR90,
		SRF_THREEPOSSWITCH90_RIGHT,
		SRF_CRYO_SWITCHES_J,
		SRF_CRYO_IND_J,
		SRF_SWITCHGUARDS90_RIGHT,


		//
		// NSURF MUST BE THE LAST ENTRY HERE. PUT ANY NEW SURFACE IDS ABOVE THIS LINE
		//
		nsurf	///< nsurf gives the count of surfaces for the array size calculation.
	};

	//
	// Random failure flags, copied into unions and extracted as ints (or vice-versa).
	//

	///
	/// \ingroup FailFlags
	/// \brief Landing failure flags.
	///
	union LandingFailures {
		struct {
			unsigned CoverFail:1;	///< Apex cover will fail to deploy automatically.
			unsigned DrogueFail:1;	///< Drogue will fail to deploy automatically.
			unsigned MainFail:1;	///< Main chutes will fail to deploy automatically.
		};
		int word;					///< Word holds the flags from the bitfield in one 32-bit value for scenarios.

		LandingFailures() { word = 0; };
	};

	///
	/// \ingroup FailFlags
	/// \brief Launch failure flags.
	///
	union LaunchFailures {
		struct {
			unsigned LETAutoJetFail:1;			///< The LES auto jettison will fail.
			unsigned LESJetMotorFail:1;			///< The LET jettison motor will fail.
			unsigned SIIAutoSepFail:1;			///< Stage two will fail to seperate automatically from stage one.
			unsigned LiftoffSignalAFail:1;		///< Liftoff signal A will not come through from the IU.
			unsigned LiftoffSignalBFail:1;		///< Liftoff signal B will not come through from the IU.
			unsigned AutoAbortEnableFail:1;		///< IU fails to enable the auto abort relays.
		};
		int word;								///< Word holds the flags from the bitfield in one 32-bit value for scenarios.

		LaunchFailures() { word = 0; };
	};

	///
	/// \ingroup FailFlags
	/// \brief Flags specifying which control panel switches will fail.
	///
	/// \ingroup InternalInterface
	///
	union SwitchFailures {
		struct {
			unsigned TowerJett1Fail:1;		///< TWR JETT switch 1 will fail.
			unsigned TowerJett2Fail:1;		///< TWR JETT switch 2 will fail.
			unsigned SMJett1Fail:1;			///< SM JETT switch 1 will fail.
			unsigned SMJett2Fail:1;			///< SM JETT switch 2 will fail
		};
		int word;							///< Word holds the flags from the bitfield in one 32-bit value for scenarios.

		SwitchFailures() { word = 0; };
	};

	///
	/// \ingroup ScenarioState
	/// \brief CSM light display state.
	///
	 union LightState {
		struct {
			unsigned Engind0:1;
			unsigned Engind1:1;
			unsigned Engind2:1;
			unsigned Engind3:1;
			unsigned Engind4:1;
			unsigned Engind5:1;
			unsigned LVGuidLight:1;
			unsigned LiftoffLight:1;
			unsigned NoAutoAbortLight:1;
			unsigned spare1:1;
			unsigned spare2:1;
			unsigned spare3:1;
			unsigned spare4:1;
			unsigned spare5:1;
			unsigned spare6:1;
			unsigned Engind6:1;
			unsigned Engind7:1;
			unsigned Engind8:1;
			unsigned LVRateLight:1;
		};
		unsigned long word;

		LightState() { word = 0; };
	};

	///
	/// \ingroup ScenarioState
	/// \brief State which is only required for Apollo 13
	///
	/// This structure holds the flags which are used for the Apollo 13 simulation. 
	///
	union A13State {
		struct {
			unsigned ApolloExploded:1;	///< Has the SM exploded yet?
			unsigned CryoStir:1;		///< Have the crew been asked to do a cryo stir?
			unsigned KranzPlayed:1;		///< Has the Kranz audio been played yet?
		};
		unsigned long word;				///< Used to return all the flags as one 32-bit word for the scenario file.

		A13State() { word = 0; };
	};

	///
	/// \ingroup ScenarioState
	/// \brief Which parts are still attached?
	///
	union AttachState {
		struct {
			unsigned InterstageAttached:1;	///< Is the interstage attached?
			unsigned LESAttached:1;			///< Is the LES attached?
			unsigned HasProbe:1;			///< Does the CM have a docking probe?
			unsigned ApexCoverAttached:1;	///< Is the apex cover attached?
			unsigned ChutesAttached:1;		///< Are the chutes attached?
			unsigned CSMAttached:1;			///< Is there a CSM?
			unsigned NosecapAttached:1;		///< Is there an Apollo 5-style nosecap?
			unsigned LESLegsCut:1;			///< Are the LES legs attached?
			unsigned SIMBayPanelJett:1;		///< Has the SIM bay panel been jettisoned?
		};
		unsigned long word;

		AttachState() { word = 0; };
	};

	///
	/// \ingroup ScenarioState
	/// \brief State related to the SLA panels.
	///
	union SLAState {
		struct {
			unsigned SLARotationLimit:8;	///< Maximum rotation limit in degrees when opening.
			unsigned SLAWillSeparate:1;		///< Will the panels separate when they reach the limit?
		};
		unsigned long word;

		SLAState() { word = 0; };
	};

	///
	/// \ingroup ScenarioState
	/// \brief Main state flags.
	///
	union MainState {
		struct {
			unsigned IUSCContPermanentEnabled:1;	///< Can the IU FCC SC control be permanently enabled?
			unsigned SIISepState:1;					///< State of the SII Sep light.
			unsigned TLIBurnDone:1;					///< Have we done our TLI burn?
			unsigned Scorrec:1;						///< Have we played the course correction sound?
			unsigned Burned:1;						///< Has the CM been burned by re-entry heating?
			unsigned FireLEM:1;						///< Launch Escape Motor has been fired.
			unsigned ABORT_IND:1;					///< State of the abort light.
			unsigned FireTJM:1;						///< Tower Jettison Motor has been fired.
			unsigned SplashdownPlayed:1;			///< Have we played the splashdown sound?
			unsigned FirePCM:1;						///< Pitch Control Motor has been fired.
			unsigned PayloadDataTransfer:1;			///< Have we transfered setup data to the SIVB for the payload?
			unsigned PostSplashdownPlayed:1;		///< Have we played the post-splashdown sound?
			unsigned unused:1;						///< Unused bit for backwards compatibility. Can be used for other things.
			unsigned TLISoundsLoaded:1;				///< Have we loaded the TLI sounds?
			unsigned CMdocktgt:1;                   ///< CM docking target on
			unsigned unused4:1;						///< Spare
			unsigned unused5:1;						///< Spare
			unsigned unused6:2;						///< Spare
			unsigned SkylabSM:1;					///< Is this a Skylab Service Module?
			unsigned SkylabCM:1;					///< Is this a Skylab Command Module?
			unsigned S1bPanel:1;					///< Is this a Command Module with a Saturn 1b panel?
			unsigned unused7:1;						///< Spare
			unsigned viewpos:5;						///< Position of the virtual cockpit viewpoint.
		};
		unsigned long word;

		MainState() { word = 0; };
	};

	//
	// Now the actual code and data for the class.
	//

	//This simply equips us with a reference to either the mfd debug string, 
	//or in the extreme case, the orbiter debug string
	char *(__cdecl *debugString)();
	bool debugConnected;
	///
	/// \brief Standard constructor with the usual Orbiter parameters.
	///
	Saturn(OBJHANDLE hObj, int fmodel);

	///
	/// \brief Destructor.
	///
	virtual ~Saturn();

	///
	/// Turn on the SII Seperation light on the control panel.
	/// \brief Set the SII Seperation light.
	///
	void SetSIISep() { SIISepState = true; };

	///
	/// Turn off the SII Seperation light on the control panel.
	/// \brief Clear the SII Seperation light.
	///
	void ClearSIISep() { SIISepState = false; };

	///
	/// Turn on an engine indicator light on the control panel.
	/// \brief Turn on engine light
	/// \param i Specify the engine light number to turn on (1-8).
	///
	void SetEngineIndicator(int i);

	bool GetEngineIndicator(int i);

	///
	/// Turn off an engine indicator light on the control panel.
	/// \brief Turn off engine light
	/// \param i Specify the engine light number to turn off (1-8).
	///
	void ClearEngineIndicator(int i);

	///
	/// Set all engine indicator lights on the control panel.
	/// \brief Set engine indicator lights.
	///
	void SetEngineIndicators();

	///
	/// Clear all engine indicator lights on the control panel.
	/// \brief Clear engine indicator lights.
	///
	void ClearEngineIndicators();

	///
	/// Call this function to jettison the LET and BPC from the Saturn. This should work during any stage,
	/// and will handle any tidying up required. If called with no LET attached, it will just return.
	/// \brief Jettison the LET and BPC.
	/// \param UseMain Specifies whether to use the main abort motor or the jettison motor.
	/// \param AbortJettison If we're jettisoning during an abort, the BPC will take the docking probe with it.
	///
	void JettisonLET(bool AbortJettison = false);
	void DeployCanard();
	void CMLETAirfoilConfig();
	void CMLETCanardAirfoilConfig();

	///
	/// This function can be used during the countdown to update the MissionTime. Since we launch when
	/// MissionTime reaches zero, setting MissionTime to (-t) tells the code when to launch.
	/// \brief Update the launch time.
	/// \param t Specifies the time in seconds to wait before launch.
	///
	void UpdateLaunchTime(double t);	

	///
	/// Set up the default mesh for the virtual cockpit.
	///
	void DefaultCMMeshSetup();

	///
	/// Turn a Service Module RCS thruster on or off.
	/// \brief SM RCS thruster control.
	/// \param Quad SM RCS thruster quad.
	/// \param Thruster Thruster number in quad.
	/// \param Active Thruster on or off?
	///
	virtual void SetRCSState(int Quad, int Thruster, bool Active);

	///
	/// Turn a Command Module RCS thruster on or off.
	/// \brief CM RCS thruster control.
	/// \param Thruster Thruster number to control.
	/// \param Active Thruster on or off?
	///
	virtual void SetCMRCSState(int Thruster, bool Active);

	bool GetCMRCSStateCommanded(THRUSTER_HANDLE th);

	char *getOtherVesselName() { return agc.OtherVesselName;};

	//
	// RHC/THC 
	//

	// DirectInput stuff
	/// Handle to DLL instance
	HINSTANCE dllhandle;
	/// pointer to DirectInput class itself
	LPDIRECTINPUT8 dx8ppv;
	/// Joysticks-Enabled flag / counter - Zero if we aren't using DirectInput, nonzero is the number of joysticks we have.
	int js_enabled;
	/// Pointers to DirectInput joystick devices
	LPDIRECTINPUTDEVICE8 dx8_joystick[2]; ///< One for THC, one for RHC, ignore extras
	DIDEVCAPS			 dx8_jscaps[2];   ///< Joystick capabilities
	DIJOYSTATE2			 dx8_jstate[2];   ///< Joystick state
	HRESULT				 dx8_failure;     ///< DX failure reason
	int rhc_id;							  ///< Joystick # for the RHC
	int rhc_rot_id;						  ///< ID of ROTATOR axis to use for RHC Z-axis
	int rhc_sld_id;                       ///< ID of SLIDER axis to use for RHC Z-axis
	int rhc_rzx_id;                       ///< Flag to use native Z-axis as RHC Z-axis
	int rhc_pov_id;						  ///< ID of the cooliehat a.k.a. POV
	int thc_id;                           ///< Joystick # for the THC
	int thc_rot_id;						  ///< ID of ROTATOR axis to use for THC Z-axis
	int thc_sld_id;                       ///< ID of SLIDER axis to use for THC Z-axis
	int thc_rzx_id;                       ///< Flag to use native Z-axis as THC Z-axis	
	int thc_pov_id;						  ///< ID of the cooliehat a.k.a. POV
	int rhc_debug;						  ///< Flags to print debugging messages.
	int thc_debug;
	bool rhc_auto;						  ///< RHC Z-axis auto detection
	bool thc_auto;						  ///< THC Z-axis auto detection
	bool rhc_thctoggle;					  ///< Enable RHC/THC toggle
	int rhc_thctoggle_id;				  ///< RHC button id for RHC/THC toggle
	bool rhc_thctoggle_pressed;			  ///< Button pressed flag				  
	int js_current;

	// Variables for checklists
	char Checklist_Variable[16][32];

	//
	// General functions that handle calls from Orbiter.
	//

	///
	/// \brief Orbiter dock state function.
	///
	void clbkDockEvent(int dock, OBJHANDLE connected);
	bool clbkLoadGenericCockpit ();
	bool clbkPanelMouseEvent(int id, int event, int mx, int my);
	bool clbkPanelRedrawEvent(int id, int event, SURFHANDLE surf);
	void clbkMFDMode (int mfd, int mode);

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
	void clbkPostStep (double simt, double simdt, double mjd);

	///
	/// \brief Orbiter timestep function.
	/// \param simt Current simulation time, in seconds since Orbiter was started.
	/// \param simdt Time in seconds since last timestep.
	/// \param mjd Current MJD.
	///
	void clbkPreStep(double simt, double simdt, double mjd);
	bool clbkLoadPanel (int id);
	int clbkConsumeDirectKey(char *keystate);
	int clbkConsumeBufferedKey(DWORD key, bool down, char *kstate);
	bool clbkLoadVC (int id);
	bool clbkVCMouseEvent (int id, int event, VECTOR3 &p);
	bool clbkVCRedrawEvent (int id, int event, SURFHANDLE surf);
	void clbkPostCreation();
	void clbkVisualCreated(VISHANDLE vis, int refcount);
	void clbkVisualDestroyed(VISHANDLE vis, int refcount);

	///
	/// This function performs all actions required to update the spacecraft state as time
	/// passes. This is a pure virtual function implemented by the derived classes.
	/// \brief Process time steps from Orbiter.
	/// \param simt Current MET in seconds.
	/// \param simdt Time in seconds since last timestep.
	///
	virtual void Timestep(double simt, double simdt, double mjd) = 0;

	///
	/// \brief Initialise a virtual cockpit view.
	///
	void InitVC (int vc);
	bool RegisterVC ();

	void PanelSwitchToggled(ToggleSwitch *s);
	void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s); 
	void PanelRotationalSwitchChanged(RotationalSwitch *s);
	void PanelRefreshForwardHatch();
	void PanelRefreshSideHatch();

	// Called by Crawler/ML
	virtual void LaunchVehicleRolloutEnd() {};	// after arrival on launch pad
	virtual void LaunchVehicleBuild() {};		// build/unbuild during assembly
	virtual void LaunchVehicleUnbuild() {};
	
	int GetBuildStatus() { return buildstatus; }

	///
	/// \brief Get the current stage number
	/// \return Stage number as specified in nassdefs.h
	///
	int GetStage() { return stage; };

	///
	/// \brief Get the systems state
	/// \return Systems state.
	///
	int GetSystemsState() { return systemsState; };


	///
	/// \brief Get the Apollo mission number
	/// \return Apollo mission number.
	///
	int GetApolloNo() { return ApolloNo; };

	///
	/// \brief Get the current mission elapsed time
	/// \return Current time
	///
	double GetMissionTime() { return MissionTime; };

	virtual double GetFirstStageThrust();
	virtual double GetSIIFuelTankPressurePSI() { return 0.0; }
	double GetSIVBFuelTankPressurePSI();
	double GetSIVBLOXTankPressurePSI();

	virtual void GetSIThrustOK(bool *ok) = 0;
	virtual void GetSIIThrustOK(bool *ok);
	bool GetSIVBThrustOK();
	virtual bool GetSIPropellantDepletionEngineCutoff() = 0;
	virtual bool GetSIIPropellantDepletionEngineCutoff();
	virtual bool GetSIInboardEngineOut() = 0;
	virtual bool GetSIOutboardEngineOut() = 0;
	virtual bool GetSIBLowLevelSensorsDry();
	virtual bool GetSIIEngineOut();
	virtual void SetSIThrusterDir(int n, double yaw, double pitch) = 0;
	virtual void SetSIIThrusterDir(int n, double yaw, double pitch) {};
	void SetSIVBThrusterDir(double yaw, double pitch);
	void SetAPSAttitudeEngine(int n, bool on);
	virtual void SIEDSCutoff(bool cut) = 0;
	virtual void SIIEDSCutoff(bool cut) {};
	void SIVBEDSCutoff(bool cut);
	virtual double GetSIThrustLevel() = 0;
	bool GetQBallPower();
	bool GetQBallSimulateCmd();

	virtual void ActivateStagingVent() {}

	virtual void VHFRangingReturnSignal();
	void StartSeparationPyros();

	//CSM to IU interface functions
	bool GetCMCSIVBTakeover();
	bool GetCMCSIVBIgnitionSequenceStart();
	bool GetCMCSIVBCutoff();
	bool GetSIISIVbDirectStagingSignal();
	bool GetTLIInhibitSignal();
	bool GetIUUPTLMAccept();

	//CSM to LM interface functions
	h_Pipe* GetCMTunnelPipe() { return CMTunnel; }
	void ConnectTunnelToCabinVent();

	///
	/// \brief Triggers Virtual AGC core dump
	///
	virtual void VirtualAGCCoreDump() { agc.VirtualAGCCoreDump("ProjectApollo CMC.core"); }

	///
	/// \brief Triggers EMS scroll saving
	///
	virtual void SaveEMSScroll() { ems.WriteScrollToFile(); }	

	///
	/// \brief Get settings for the Saturn payload.
	///
	void GetPayloadSettings(PayloadSettings &p);

	///
	/// \brief Accessor to get checklistController
	///
	ChecklistController *GetChecklistControl();

	/// 
	/// \brief LVDC "Switch Selector" staging support utility function
	/// 
	virtual void SISwitchSelector(int channel) = 0;
	virtual void SIISwitchSelector(int channel) {}
	void SIVBSwitchSelector(int channel);

	//
	// CWS functions.
	//

	///
	/// Get information on the atmosphere status in the CSM.
	/// \brief Get atmosphere status.
	/// \param atm Atmosphere information structure, updated by the call.
	///
	void GetAtmosStatus(AtmosStatus &atm);
	void GetDisplayedAtmosStatus(DisplayedAtmosStatus &atm);
	void GetTankPressures(TankPressures &press);
	void GetTankQuantities(TankQuantities &q);

	///
	/// Get information on the status of a fuel cell in the CSM.
	/// \brief Get fuel cell status.
	/// \param index Index of fuel cell to query (1-3)
	/// \param fc Fuel cell information structure, updated by the call.
	///
	void GetFuelCellStatus(int index, FuelCellStatus &fc);
	void GetPrimECSCoolingStatus(PrimECSCoolingStatus &pcs);
	void GetSecECSCoolingStatus(SecECSCoolingStatus &scs);
	void GetECSWaterStatus(ECSWaterStatus &ws);
	void GetMainBusStatus(MainBusStatus &ms);
	void GetBatteryBusStatus( BatteryBusStatus &bs );
	void GetACBusStatus(ACBusStatus &as, int busno);
	void GetBatteryStatus( BatteryStatus &bs );
	void GetSPSStatus( SPSStatus &ss );
	void GetSECSStatus( SECSStatus &ss );
	void GetPyroStatus( PyroStatus &ps );
	void DisconnectInverter(bool disc, int busno);
	void EPSTimestep();
	void GetAGCWarningStatus(AGCWarningStatus &aws);
	void GetRCSStatus(int index, RCSStatus &rs);
	double GetAccelG() { return mechanicalAccelerometer.GetXAccel() / G; };
	virtual void GetECSStatus(ECSStatus &ecs);
	virtual void SetCrewNumber(int number);
	virtual void SetPrimECSTestHeaterPowerW(double power);
	virtual void SetSecECSTestHeaterPowerW(double power);

	///
	/// Enable or disable generic Service Module systems based on current state.
	/// \brief Check SM systems state.
	///
	void CheckSMSystemsState();

	///
	/// Enable or disable Saturn systems based on current state.
	/// \brief Check Saturn systems state.
	///
	virtual void CheckSaturnSystemsState();

	virtual void CreateStageSpecificSystems() = 0;

	void CreateMissionSpecificSystems();

	///
	/// If the scenario specified AUTOSLOW and time acceleration is enabled, slow it
	/// down to 1.0.
	/// \brief Disable time acceleration if desired.
	///
	void SlowIfDesired();

	void ActivateS4RCS();
	void DeactivateS4RCS();

	virtual void ActivatePrelaunchVenting() = 0;
	virtual void DeactivatePrelaunchVenting() = 0;

	void SetContrailLevel(double level);

	void SetStage(int s);
	virtual void SeparateStage(int stage) = 0;

	///
	/// Turn on the LV Guidance warning light on the control panel to indicate an autopilot
	/// failure.
	/// \brief Set the LV Guidance light.
	///
	void SetLVGuidLight();

	///
	/// Turn off the LV Guidance warning light on the control panel.
	/// \brief Clear the LV Guidance light.
	///
	void ClearLVGuidLight();

	///
	/// Turn on the LV Rate warning light on the control panel, indicating that the spacecraft
	/// is turning too fast.
	/// \brief Set the LV Rate light.
	///
	void SetLVRateLight();

	///
	/// Turn on the LV Rate warning light on the control panel.
	/// \brief Set the LV Rate light.
	///
	void ClearLVRateLight();

	///
	/// \brief Set propellant mass in the SIVb stage.
	/// \param mass Propellant mass in kg.
	///
	void SetSIVbPropellantMass(double mass);

	int GetLVRateAutoSwitchState();
	int GetTwoEngineOutAutoSwitchState();

	bool GetBECOSignal(bool IsSysA);
	bool IsEDSBusPowered(int eds);
	int GetAGCAttitudeError(int axis);

	void AddRCS_S4B();

	///
	/// \brief Load sounds required for TLI burn.
	///
	void LoadTLISounds();

	///
	/// \brief Clear TLI sounds.
	///
	void ClearTLISounds();

	///
	/// \brief We've hard docked, so check connections.
	///
	void HaveHardDocked(int port);

	///
	/// \brief We've extended the docking probe, so disconnect connections.
	///
	void Undocking(int port);

	///
	/// \brief Set docking probe mesh
	///
	void SetDockingProbeMesh();

	///
	/// \brief Set side hatch mesh
	///
	void SetSideHatchMesh();

	///
	/// \brief Set fwd hatch mesh
	///
	void SetFwdHatchMesh();
	
	///
	/// \brief Set crew mesh
	///
	void SetCrewMesh();

	///
	/// \brief Set optics cover mesh
	///
	void SetOpticsCoverMesh();

	///
	/// \brief Set CM docking target mesh
	///
	
	void SetCMdocktgtMesh();

	///
	/// \brief Set nosecap mesh
	///
	void SetNosecapMesh();

	void SetSIMBayPanelMesh();

	///
	/// \brief Set probe visibility flag
	///
	void ProbeVis();

	///
	/// Check whether the Launch Escape Tower is attached.
	/// \brief Is the LET still attached?
	/// \return True if attached, false if not.
	///
	bool LETAttached();

	void CutLESLegs();

	///
	/// \brief Returns the IMFD communication client for ProjectApolloMFD
	///
	virtual IMFD_Client *GetIMFDClient() { return &IMFD_Client; }; 

	///
	/// \brief TLI event management
	///
	void TLI_Begun();
	void TLI_Ended();

	//
	// Function for configuring 4 touchdown points in Orbiter 2016.
	// \ mass in kg, ro (distance from center of the bottom points), tdph (height of bottom points),
	// \ height (of the top point), x_target (stiffness/damping factor, stable default is -0.5)
	//
	void ConfigTouchdownPoints(double mass, double ro, double tdph, double height, double x_target = -0.5);

	//
	// LUA Interface
	//
	int clbkGeneric(int msgid, int prm, void *context);
	int Lua_InitInterpreter(void *context);
	int Lua_InitInstance(void *context);

	int Lua_GetPanelSwitch(const char *name);
	void Lua_SetPanelSwitch(const char *name, int state);
	bool Lua_GetPanelSwitchFlashing(const char *name);
	void Lua_SetPanelSwitchFlashing(const char *name, bool flash);
	int Lua_GetAGCErasable(int page, int addr);
	int Lua_GetAGCChannel(int ch);
	void Lua_SetAGCErasable(int page, int addr, int value);
	int Lua_GetAGCUplinkStatus();

	//System Access
	SPSPropellantSource *GetSPSPropellant() { return &SPSPropellant; };
	SPSEngine *GetSPSEngine() { return &SPSEngine; };
	SCE *GetSCE() { return &sce; }
	EDA *GetEDA() { return &eda; }
	IU *GetIU() { return iu; };
	virtual SICSystems *GetSIC() { return NULL; }
	SECS *GetSECS() { return &secs; }

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

	// FILE *PanelsdkLogFile;

	void InitSwitches();

	///
	/// Check whether the Saturn vehicle has a CSM. Some, like Apollo 5, flew without a CSM for
	/// LEM testing.
	/// \brief Do we have a CSM?
	/// \return True if CSM, false if not.
	///
	bool SaturnHasCSM();

	void JettisonDockingProbe();

	void JettisonOpticsCover();

	void JettisonNosecap();

	void JettisonSIMBayPanel();

	//
	// State that needs to be saved.
	//

	char StagesString[256];

	///
	/// SII Sep light. True if light should be lit.
	/// \brief SII Sep light.
	///
	bool SIISepState;

	///
	/// Abort light. True if ABORT light is lit.
	/// \brief Abort light.
	///
	bool ABORT_IND;

	///
	/// Interstage attached flag. True if interstage is attached.
	/// \brief Interstage flag.
	///
	bool InterstageAttached;

	///
	/// LESAttached flag. True if the LES is attached.
	/// \brief LES flag.
	///
	bool LESAttached;

	///
	/// LESLegsCut flag. True if the LES legs have been cut.
	/// \brief LES flag.
	///
	bool LESLegsCut;

	///
	/// True if the docking probe is attached.
	/// \brief Docking probe flag.
	///
	bool HasProbe;

	///
	/// True if the parachutes are attached.
	/// \brief Parachutes attached flag.
	///
	bool ChutesAttached;

	///
	/// True if the apex cover is attached.
	/// \brief Apex cover flag.
	///
	bool ApexCoverAttached;

	///
	/// True if there is a CSM.
	/// \brief Nosecap attached flag.
	///
	bool CSMAttached;

	///
	/// True if there is an Apollo 5-style nosecap in place of a CSM.
	/// \brief Nosecap attached flag.
	///
	bool NosecapAttached;

	///
	/// Gives the angle to which the SLA panels will rotate; some of the Skylab missions
	/// planned to retain the SLA panels but rotate them around 150 degrees.
	/// \brief SLA panel rotation angle.
	///
	int SLARotationLimit;

	///
	/// True if the SLA panels will separate from the SIVB.
	/// \brief SLA panels separation flag.
	///
	bool SLAWillSeparate;

	bool SIMBayPanelJett;

	bool DeleteLaunchSite;

	int buildstatus;

	//
	// Current mission time and mission times for stage events.
	//

	///
	/// The current Mission Elapsed Time. This is the main variable used for timing
	/// automated events during the mission, giving the time in seconds from launch
	/// (negative for the pre-launch countdown).
	/// \brief Mission Elapsed Time.
	///
	double MissionTime;

	///
	/// The time in seconds of the next automated event to occur in the mission. This 
	/// is a generic value used by the autopilot and staging code.
	/// \brief Time of next event.
	///
	double NextMissionEventTime;

	///
	/// The time in seconds of the previous automated event that occur in the mission. This 
	/// is a generic value used by the autopilot and staging code.
	/// \brief Time of last event.
	///
	double LastMissionEventTime;


	///
	/// The time in seconds of the next check for destroying old stages. We destroy them when
	/// they get too far away or too close to the ground as there's no point keeping them
	/// hanging around when we'll never see them again.
	/// \brief Time to next check for stage destruction.
	///
	double NextDestroyCheckTime;

	///
	/// Mission Timer display on control panel.
	/// \brief Mission Timer display.
	///
	MissionTimer MissionTimerDisplay;

	///
	/// Mission Timer display on panel 306.
	/// \brief Panel 306 Mission Timer display.
	///
	MissionTimer MissionTimer306Display;

	///
	/// Event Timer display on control panel.
	/// \brief Event Timer display.
	///
	EventTimer EventTimerDisplay;
	SaturnEventTimer SaturnEventTimerDisplay;	//Dummy for checklist controller

	///
	/// Event Timer display on panel 306.
	/// \brief Panel 306 Event Timer display.
	///
	EventTimer EventTimer306Display;
	SaturnEventTimer SaturnEventTimer306Display;	//Dummy for checklist controller

	///
	/// Flag for use of low-res meshes to reduce graphics lag.
	/// \brief Using low-res meshes?
	///
	bool LowRes;

	//
	// Mesh handles.
	//

	MESHHANDLE hStage1Mesh;
	MESHHANDLE hStage2Mesh;
	MESHHANDLE hStage3Mesh;
	MESHHANDLE hInterstageMesh;
	MESHHANDLE hStageSLA1Mesh;
	MESHHANDLE hStageSLA2Mesh;
	MESHHANDLE hStageSLA3Mesh;
	MESHHANDLE hStageSLA4Mesh;

	UINT SPSidx;

	bool LEM_DISPLAY;

	///
	/// The Saturn vehicle number. Saturn 1b will usually be in the 200s, and Saturn V in the 500s.
	/// \brief Saturn number.
	///
	int VehicleNo;

	///
	/// Apollo mission number. Some events are triggered based on the mission, so for fictional
	/// missions this should be set to a value below 7 or above 18.
	/// \brief Apollo mission number.
	///
	int ApolloNo;

	///
	/// Flag to determine whether we leave the OrbiterSound default ATC enabled. Some people like
	/// it, so if this flag is true we allow them to use it.
	/// \brief Orbitersound ATC enabled.
	///
	bool UseATC;

	///
	/// Flag to indicate that we've told the user to make a course correction.
	/// \brief Course correction flag.
	///
	bool Scorrec;

	///
	/// Flag to indicate that the SM has exploded on the Apollo 13 mission.
	/// \brief SM exploded flag.
	///
	bool ApolloExploded;

	///
	/// Flag to indicate that the CM has been 'burnt' by re-entry.
	/// \brief CM burnt flag.
	///
	bool Burned;

	bool CryoStir;
	double TCPO;

	//
	// Failures.
	//

	LandingFailures LandFail;
	LaunchFailures LaunchFail;
	SwitchFailures SwitchFail;

	//
	// Ground Systems
	//
	MCC *pMCC;
	OBJHANDLE hMCC;
	char MCCstring[64];

	//
	// ChecklistController
	//
	ChecklistController checkControl;
	SaturnEvents eventControl;

	//
	// Switches
	//

	int coasEnabled;
	int ordealEnabled;
	int opticsDskyEnabled;
	int hatchPanel600EnabledLeft;
	int hatchPanel600EnabledRight;
	int panel382Enabled;

	///
	/// \brief Right-hand FDAI.
	///
	FDAI fdaiRight;

	///
	/// \brief Left-hand FDAI.
	///
	FDAI fdaiLeft;
	int fdaiDisabled;
	int fdaiSmooth;

	HBITMAP hBmpFDAIRollIndicator;

	//Panels

	PanelGroup pgPanels100;
	PanelGroup pgPanels200;

	SwitchRow MasterAlarmSwitchRow;
	MasterAlarmSwitch MasterAlarmSwitch; 

	// EMS
	SwitchRow EMSFunctionSwitchRow;
	RotationalSwitch EMSFunctionSwitch;

	SwitchRow EMSModeRow;
	ThreePosSwitch EMSModeSwitch;

	SwitchRow GTASwitchRow;
	GuardedToggleSwitch GTASwitch;

	SaturnEMSDvSetSwitch EMSDvSetSwitch;

	SwitchRow EMSDvDisplayRow;
	SaturnEMSDvDisplay EMSDvDisplay;
	SaturnEMSScrollDisplay EMSScrollDisplay;	// dummy switch/display for checklist controller

	SwitchRow IMUCageSwitchRow;
	SwitchRow CautionWarningRow;
	SwitchRow MissionTimerSwitchesRow;
	SwitchRow SPSRow;

	SwitchRow AccelGMeterRow;
	SaturnAccelGMeter AccelGMeter;
	MechanicalAccelerometer mechanicalAccelerometer;

	SwitchRow THCRotaryRow;
	THCRotarySwitch THCRotary;

	SaturnLiftoffNoAutoAbortSwitch LiftoffNoAutoAbortSwitch;
	GuardedPushSwitch LesMotorFireSwitch;
	GuardedPushSwitch CanardDeploySwitch;
	GuardedPushSwitch CsmLvSepSwitch;
	GuardedPushSwitch ApexCoverJettSwitch;
	GuardedPushSwitch DrogueDeploySwitch;
	GuardedPushSwitch MainDeploySwitch;
	GuardedPushSwitch CmRcsHeDumpSwitch;

	ToggleSwitch	    EDSSwitch;				
	GuardedToggleSwitch CsmLmFinalSep1Switch;
	GuardedToggleSwitch CsmLmFinalSep2Switch;
	GuardedToggleSwitch CmSmSep1Switch;
	GuardedToggleSwitch CmSmSep2Switch;
	GuardedToggleSwitch SIVBPayloadSepSwitch;

	ToggleSwitch   CabinFan1Switch;
	ToggleSwitch   CabinFan2Switch;
	ThreePosSwitch H2Heater1Switch;
	ThreePosSwitch H2Heater2Switch;
	ThreePosSwitch O2Heater1Switch;
	ThreePosSwitch O2Heater2Switch;	
	ToggleSwitch   O2PressIndSwitch;	
	ThreePosSwitch H2Fan1Switch; 
	ThreePosSwitch H2Fan2Switch; 
	ThreePosSwitch O2Fan1Switch; 
	ThreePosSwitch O2Fan2Switch; 

	IndicatorSwitch FuelCellPhIndicator;
	IndicatorSwitch FuelCellRadTempIndicator;

	IndicatorSwitch FuelCellRadiators1Indicator;
	IndicatorSwitch FuelCellRadiators2Indicator;
	IndicatorSwitch FuelCellRadiators3Indicator;
	ThreePosSwitch FuelCellRadiators1Switch;
	ThreePosSwitch FuelCellRadiators2Switch;
	ThreePosSwitch FuelCellRadiators3Switch;

	RotationalSwitch FuelCellIndicatorsSwitch;

	ToggleSwitch FuelCellHeater1Switch;
	ToggleSwitch FuelCellHeater2Switch;
	ToggleSwitch FuelCellHeater3Switch;

	ThreePosSwitch FuelCellPurge1Switch;
	ThreePosSwitch FuelCellPurge2Switch;
	ThreePosSwitch FuelCellPurge3Switch;

	IndicatorSwitch FuelCellReactants1Indicator;
	IndicatorSwitch FuelCellReactants2Indicator;
	IndicatorSwitch FuelCellReactants3Indicator;
	ThreePosSwitch FuelCellReactants1Switch;
	ThreePosSwitch FuelCellReactants2Switch;
	ThreePosSwitch FuelCellReactants3Switch;

	ToggleSwitch FCReacsValvesSwitch;
	ToggleSwitch H2PurgeLineSwitch;

	ThreeSourceSwitch FuelCellPumps1Switch;
	ThreeSourceSwitch FuelCellPumps2Switch;
	ThreeSourceSwitch FuelCellPumps3Switch;

	SaturnSuitCompressorSwitch SuitCompressor1Switch;
	SaturnSuitCompressorSwitch SuitCompressor2Switch;

	TimerControlSwitch MissionTimerSwitch;
	CWSModeSwitch CautionWarningModeSwitch;
	CWSSourceSwitch CautionWarningCMCSMSwitch;
	CWSPowerSwitch CautionWarningPowerSwitch;
	CWSLightTestSwitch CautionWarningLightTestSwitch;

	TimerUpdateSwitch MissionTimerHoursSwitch;
	TimerUpdateSwitch MissionTimerMinutesSwitch;
	TimerUpdateSwitch MissionTimerSecondsSwitch;

	IMUCageSwitch IMUGuardedCageSwitch;

	SwitchRow SMRCSHelium1Row;
	ThreePosSwitch SMRCSHelium1ASwitch;
	ThreePosSwitch SMRCSHelium1BSwitch;
	ThreePosSwitch SMRCSHelium1CSwitch;
	ThreePosSwitch SMRCSHelium1DSwitch;

	SwitchRow SMRCSHelium1TalkbackRow;
	SaturnRCSValveTalkback SMRCSHelium1ATalkback;
	SaturnRCSValveTalkback SMRCSHelium1BTalkback;
	SaturnRCSValveTalkback SMRCSHelium1CTalkback;
	SaturnRCSValveTalkback SMRCSHelium1DTalkback;

	SwitchRow SMRCSHelium2Row;
	ThreePosSwitch SMRCSHelium2ASwitch;
	ThreePosSwitch SMRCSHelium2BSwitch;
	ThreePosSwitch SMRCSHelium2CSwitch;
	ThreePosSwitch SMRCSHelium2DSwitch;

	SwitchRow SMRCSHelium2TalkbackRow;
	SaturnRCSValveTalkback SMRCSHelium2ATalkback;
	SaturnRCSValveTalkback SMRCSHelium2BTalkback;
	SaturnRCSValveTalkback SMRCSHelium2CTalkback;
	SaturnRCSValveTalkback SMRCSHelium2DTalkback;

	AGCIOSwitch CMUplinkSwitch;
	ToggleSwitch IUUplinkSwitch;

	GuardedToggleSwitch CMRCSPressSwitch;
	ToggleSwitch SMRCSIndSwitch;

	SwitchRow RCSGaugeRow;
	RCSQuantityMeter RCSQuantityMeter;
	RCSFuelPressMeter RCSFuelPressMeter;
	RCSHeliumPressMeter RCSHeliumPressMeter;
	RCSTempMeter RCSTempMeter;

	SwitchRow SMRCSProp1Row;
	ThreePosSwitch SMRCSProp1ASwitch;
	ThreePosSwitch SMRCSProp1BSwitch;
	ThreePosSwitch SMRCSProp1CSwitch;
	ThreePosSwitch SMRCSProp1DSwitch;

	ThreeOutputSwitch SMRCSHeaterASwitch;
	ThreeOutputSwitch SMRCSHeaterBSwitch;
	ThreeOutputSwitch SMRCSHeaterCSwitch;
	ThreeOutputSwitch SMRCSHeaterDSwitch;

	SwitchRow SMRCSProp1TalkbackRow;
	SaturnRCSValveTalkback SMRCSProp1ATalkback;
	SaturnRCSValveTalkback SMRCSProp1BTalkback;
	SaturnRCSValveTalkback SMRCSProp1CTalkback;
	SaturnRCSValveTalkback SMRCSProp1DTalkback;

	SwitchRow SMRCSProp2Row;
	ThreePosSwitch SMRCSProp2ASwitch;
	ThreePosSwitch SMRCSProp2BSwitch;
	ThreePosSwitch SMRCSProp2CSwitch;
	ThreePosSwitch SMRCSProp2DSwitch;

	ThreePosSwitch RCSCMDSwitch;
	ThreePosSwitch RCSTrnfrSwitch;

	ThreePosSwitch CMRCSProp1Switch;
	ThreePosSwitch CMRCSProp2Switch;

	SwitchRow SMRCSProp2TalkbackRow;
	SaturnRCSValveTalkback CMRCSProp1Talkback;
	SaturnRCSValveTalkback CMRCSProp2Talkback;

	SaturnRCSValveTalkback SMRCSProp2ATalkback;
	SaturnRCSValveTalkback SMRCSProp2BTalkback;
	SaturnRCSValveTalkback SMRCSProp2CTalkback;
	SaturnRCSValveTalkback SMRCSProp2DTalkback;

	SwitchRow RCSIndicatorsSwitchRow;
	PropellantRotationalSwitch RCSIndicatorsSwitch;

	SwitchRow ECSIndicatorsSwitchRow;
	RotationalSwitch ECSIndicatorsSwitch;

	SwitchRow AttitudeControlRow;
	ThreePosSwitch ManualAttRollSwitch;
	ThreePosSwitch ManualAttPitchSwitch;
	ThreePosSwitch ManualAttYawSwitch;
	ToggleSwitch LimitCycleSwitch;
	ToggleSwitch AttDeadbandSwitch;
	ToggleSwitch AttRateSwitch;
	ToggleSwitch TransContrSwitch;

	SwitchRow BMAGRow;
	ThreePosSwitch BMAGRollSwitch;
	ThreePosSwitch BMAGPitchSwitch;
	ThreePosSwitch BMAGYawSwitch;

	SwitchRow SCContCMCModeSwitchesRow;
	SaturnSCContSwitch SCContSwitch;
	CMCModeHoldFreeSwitch CMCModeSwitch;

	SwitchRow SCSTvcSwitchesRow;
	ThreePosSwitch SCSTvcPitchSwitch;
	ThreePosSwitch SCSTvcYawSwitch;

	SwitchRow SPSGimbalMotorsRow;
	ThreePosSwitch Pitch1Switch;
	ThreePosSwitch Pitch2Switch;
	ThreePosSwitch Yaw1Switch;
	ThreePosSwitch Yaw2Switch;

	SwitchRow EntrySwitchesRow;
	ToggleSwitch EMSRollSwitch;
	ToggleSwitch GSwitch;

	SwitchRow LVSPSIndSwitchesRow;
	ToggleSwitch LVSPSPcIndicatorSwitch;
	ToggleSwitch LVFuelTankPressIndicatorSwitch;

	SwitchRow TVCGimbalDriveSwitchesRow;
	ThreePosSwitch TVCGimbalDrivePitchSwitch;
	ThreePosSwitch TVCGimbalDriveYawSwitch;

	SwitchRow CSMLightSwitchesRow;
	ToggleSwitch RunEVALightSwitch;
	ThreePosSwitch RndzLightSwitch;
	ToggleSwitch TunnelLightSwitch;

	SwitchRow LMPowerSwitchRow;
	CSMLMPowerSwitch LMPowerSwitch;

	SwitchRow PostLDGVentValveLeverRow;
	CircuitBrakerSwitch PostLDGVentValveLever;

	SwitchRow GDCAlignButtonRow;
	PushSwitch GDCAlignButton;
	
	//
	// Communication switches (s-band, vhf etc.)
	//

	SwitchRow TelecomTBRow;
	IndicatorSwitch PwrAmplTB;
	DSEIndicatorSwitch DseTapeTB;

	ThreePosSwitch SBandNormalXPDRSwitch;
	ToggleSwitch SBandNormalPwrAmpl1Switch;
	ThreePosSwitch SBandNormalPwrAmpl2Switch;
	ThreePosSwitch SBandNormalMode1Switch;
	ThreePosSwitch SBandNormalMode2Switch;
	ToggleSwitch SBandNormalMode3Switch;

	SwitchRow SBandAuxSwitchesRow;
	ThreePosSwitch SBandAuxSwitch1;
	ThreePosSwitch SBandAuxSwitch2;

	SwitchRow UPTLMSwitchesRow;
	ToggleSwitch   UPTLMSwitch1;
	ThreePosSwitch UPTLMSwitch2;

	SwitchRow SBandAntennaSwitchesRow;
	ThreePosSwitch SBandAntennaSwitch1;
	ThreePosSwitch SBandAntennaSwitch2;

	SwitchRow VHFAmThumbwheelsRow;
	ThumbwheelSwitch SquelchAThumbwheel;
	ThumbwheelSwitch SquelchBThumbwheel;

	SwitchRow VHFSwitchesRow;
	ThreePosSwitch VHFAMASwitch;
	ThreePosSwitch VHFAMBSwitch;
	ThreePosSwitch RCVOnlySwitch;
	ToggleSwitch VHFBeaconSwitch;
	ToggleSwitch VHFRangingSwitch;

	SwitchRow TapeRecorderSwitchesRow;
	ToggleSwitch TapeRecorderPCMSwitch;
	ThreePosSwitch TapeRecorderRecordSwitch;
	ThreePosSwitch TapeRecorderForwardSwitch;

	SwitchRow PowerSwitchesRow;
	ThreePosSwitch SCESwitch;
	ThreePosSwitch PMPSwitch;

	SwitchRow PCMBitRateSwitchRow;
	ToggleSwitch PCMBitRateSwitch;
	
	//
	// VHF antenna rotary
	//
	
	SwitchRow VHFAntennaRotaryRow;
	RotationalSwitch VHFAntennaRotarySwitch;

	//
	// SPS switches (panel 3)
	//

	SwitchRow SPSInjectorValveIndicatorsRow;
	IndicatorSwitch SPSInjectorValve1Indicator;
	IndicatorSwitch SPSInjectorValve2Indicator;
	IndicatorSwitch SPSInjectorValve3Indicator;
	IndicatorSwitch SPSInjectorValve4Indicator;

	SwitchRow SPSTestSwitchRow;
	ThreePosSwitch SPSTestSwitch;

	SwitchRow SPSOxidFlowValveSwitchesRow;
	ThreePosSwitch SPSOxidFlowValveSwitch;
	ToggleSwitch SPSOxidFlowValveSelectorSwitch;

	SwitchRow SPSOxidFlowValveIndicatorsRow;
	IndicatorSwitch SPSOxidFlowValveMaxIndicator;
	IndicatorSwitch SPSOxidFlowValveMinIndicator;

	SwitchRow SPSPugModeSwitchRow;
	ThreePosSwitch SPSPugModeSwitch;

	SwitchRow SPSHeliumValveIndicatorsRow;
	IndicatorSwitch SPSHeliumValveAIndicator;
	IndicatorSwitch SPSHeliumValveBIndicator;

	SwitchRow SPSSwitchesRow;
	ThreePosSwitch SPSHeliumValveASwitch;
	ThreePosSwitch SPSHeliumValveBSwitch;
	ThreePosSwitch SPSLineHTRSSwitch;
	ThreePosSwitch SPSPressIndSwitch;

	//
	// Electricals switches & indicators
	//

	SwitchRow DCIndicatorsRotaryRow;
	PowerStateRotationalSwitch DCIndicatorsRotary;

	SwitchRow BatteryChargeRotaryRow;
	RotationalSwitch BatteryChargeRotary;

	SwitchRow ACIndicatorRotaryRow;
	PowerStateRotationalSwitch ACIndicatorRotary;

	SwitchRow ACInverterSwitchesRow;
	TwoSourceSwitch MnA1Switch;
	TwoSourceSwitch MnB2Switch;
	ThreeSourceSwitch MnA3Switch;
	CMACInverterSwitch AcBus1Switch1;
	CMACInverterSwitch AcBus1Switch2;
	CMACInverterSwitch AcBus1Switch3;
	ThreeSourceSwitch AcBus1ResetSwitch;
	CMACInverterSwitch AcBus2Switch1;
	CMACInverterSwitch AcBus2Switch2;
	CMACInverterSwitch AcBus2Switch3;
	ThreeSourceSwitch AcBus2ResetSwitch;

	DCBusIndicatorSwitch MainBusBIndicator1;
	DCBusIndicatorSwitch MainBusBIndicator2;
	DCBusIndicatorSwitch MainBusBIndicator3;
	DCBusIndicatorSwitch MainBusAIndicator1;
	DCBusIndicatorSwitch MainBusAIndicator2;
	DCBusIndicatorSwitch MainBusAIndicator3;
	ThreePosSwitch MainBusBSwitch1;
	ThreePosSwitch MainBusBSwitch2;
	ThreePosSwitch MainBusBSwitch3;
	ThreeSourceSwitch MainBusBResetSwitch;
	ThreePosSwitch MainBusASwitch1;
	ThreePosSwitch MainBusASwitch2;
	ThreePosSwitch MainBusASwitch3;
	ThreeSourceSwitch MainBusAResetSwitch;

	//
	// Electrical meters
	//

	SwitchRow ACVoltMeterRow;
	ACVoltMeter CSMACVoltMeter;

	SwitchRow DCVoltMeterRow;
	DCVoltMeter CSMDCVoltMeter;

	SwitchRow DCAmpMeterRow;
	SaturnDCAmpMeter DCAmpMeter;

	SwitchRow SystemTestMeterRow;
	DCVoltMeter SystemTestVoltMeter;
	SaturnSystemTestAttenuator  SystemTestAttenuator;

	//
	// FDAI control switches.
	//

	SwitchRow FDAISwitchesRow;
	ThreePosSwitch FDAIScaleSwitch;
	ThreePosSwitch FDAISourceSwitch;
	ThreePosSwitch FDAISelectSwitch;
	ToggleSwitch FDAIAttSetSwitch;

	//
	// CMC Att: IMU is normal state, GDC does nothing.
	//

	SwitchRow CMCAttRow;
	ToggleSwitch CMCAttSwitch;

	//
	// Launch vehicle switches.
	//

	SwitchRow LVRow;
	AGCIOGuardedToggleSwitch LVGuidanceSwitch;
	GuardedToggleSwitch SIISIVBSepSwitch;
	XLunarSwitch TLIEnableSwitch;

	//
	// ELS and CSM propellant switches.
	//

	SwitchRow ELSRow;
	AGCIOSwitch CGSwitch;
	GuardedToggleSwitch ELSLogicSwitch;
	ToggleSwitch ELSAutoSwitch;
	ToggleSwitch CMRCSLogicSwitch;
	GuardedToggleSwitch CMPropDumpSwitch;
	GuardedToggleSwitch CMPropPurgeSwitch;

	//
	// Event Timer switches.
	//

	SwitchRow EventTimerRow;
	// The FSCM switches were removed in the real CSM
	// ToggleSwitch FCSMSPSASwitch;
	// ToggleSwitch FCSMSPSBSwitch;
	EventTimerResetSwitch EventTimerUpDownSwitch;
	EventTimerControlSwitch EventTimerContSwitch;
	TimerUpdateSwitch EventTimerMinutesSwitch;
	TimerUpdateSwitch EventTimerSecondsSwitch;

	//
	// Main chute release switch.
	//

	SwitchRow MainReleaseRow;
	GuardedToggleSwitch MainReleaseSwitch;

	//
	// Abort switches.
	//

	SwitchRow AbortRow;
	ToggleSwitch PropDumpAutoSwitch;
	ToggleSwitch TwoEngineOutAutoSwitch;
	ToggleSwitch LVRateAutoSwitch;
	GuardedThreePosSwitch TowerJett1Switch;
	GuardedThreePosSwitch TowerJett2Switch;

	///
	/// \brief Rotational Controller power switches.
	///
	SwitchRow RotContrPowerRow;
	ThreePosSwitch RotPowerNormal1Switch;
	ThreePosSwitch RotPowerNormal2Switch;
	ThreePosSwitch RotPowerDirect1Switch;
	ThreePosSwitch RotPowerDirect2Switch;

	///
	/// \brief dV Thrust switches.
	///
	SwitchRow dvThrustRow;
	GuardedTwoSourceSwitch dVThrust1Switch;
	GuardedTwoSourceSwitch dVThrust2Switch;

	///
	/// \brief SPS Switch.
	///
	ToggleSwitch SPSswitch;

	SwitchRow SPSGimbalPitchThumbwheelRow;
	ContinuousThumbwheelSwitch SPSGimbalPitchThumbwheel;
	SwitchRow SPSGimbalYawThumbwheelRow;
	ContinuousThumbwheelSwitch SPSGimbalYawThumbwheel;

	SwitchRow DirectUllageThrustOnRow;
	PushSwitch DirectUllageButton;
	PushSwitch ThrustOnButton;

	///
	/// \brief SPS meters.
	///
	SwitchRow SPSOxidPercentRow;
	SaturnSPSOxidPercentMeter SPSOxidPercentMeter;

	SwitchRow SPSFuelPercentRow;
	SaturnSPSFuelPercentMeter SPSFuelPercentMeter;

	SwitchRow SPSOxidUnbalMeterRow;
	SaturnSPSOxidUnbalMeter SPSOxidUnbalMeter;

	SwitchRow SPSMetersRow;
	SaturnSPSTempMeter SPSTempMeter;
	SaturnSPSHeliumNitrogenPressMeter SPSHeliumNitrogenPressMeter;
	SaturnSPSPropellantPressMeter SPSFuelPressMeter;
	SaturnSPSPropellantPressMeter SPSOxidPressMeter;

	SwitchRow LVSPSPcMeterRow;
	SaturnLVSPSPcMeter LVSPSPcMeter;

	SwitchRow GPFPIMeterRow;
	SaturnGPFPIPitchMeter GPFPIPitch1Meter;
	SaturnGPFPIPitchMeter GPFPIPitch2Meter;
	SaturnGPFPIYawMeter GPFPIYaw1Meter;
	SaturnGPFPIYawMeter GPFPIYaw2Meter;

	///
	/// \brief Cryo tank meters
	///
	SwitchRow CryoTankMetersRow;
	SaturnH2PressureMeter H2Pressure1Meter;
	SaturnH2PressureMeter H2Pressure2Meter;
	SaturnO2PressureMeter O2Pressure1Meter;
	SaturnO2PressureMeter O2Pressure2Meter;
	SaturnCryoQuantityMeter H2Quantity1Meter;
	SaturnCryoQuantityMeter H2Quantity2Meter;
	SaturnCryoQuantityMeter O2Quantity1Meter;
	SaturnCryoQuantityMeter O2Quantity2Meter;

	///
	/// \brief Fuel cell meters
	///
	SwitchRow FuelCellMetersRow;
	SaturnFuelCellH2FlowMeter FuelCellH2FlowMeter;
	SaturnFuelCellO2FlowMeter FuelCellO2FlowMeter;
	SaturnFuelCellTempMeter FuelCellTempMeter;
	SaturnFuelCellCondenserTempMeter FuelCellCondenserTempMeter;

	///
	/// \brief Cabin meters
	///
	SwitchRow CabinMetersRow;
	SaturnSuitTempMeter SuitTempMeter;
	SaturnCabinTempMeter CabinTempMeter;
	SaturnSuitPressMeter SuitPressMeter; 
	SaturnCabinPressMeter CabinPressMeter;
	SaturnPartPressCO2Meter PartPressCO2Meter; 

	//
	// Suit compressor delta pressure meter
	//
	SwitchRow SuitComprDeltaPMeterRow;
	SaturnSuitComprDeltaPMeter SuitComprDeltaPMeter;
	SaturnLeftO2FlowMeter LeftO2FlowMeter;

	//
	// Suit cabin delta pressure meter
	//
	SwitchRow SuitCabinDeltaPMeterRow;
	SaturnSuitCabinDeltaPMeter SuitCabinDeltaPMeter;
	SaturnRightO2FlowMeter RightO2FlowMeter;

	//
	// ECS radiator/evaporator temperature/pressure meters
	//
	SwitchRow EcsRadTempMetersRow;
	SaturnEcsRadTempInletMeter EcsRadTempInletMeter;
	SaturnEcsRadTempPrimOutletMeter EcsRadTempPrimOutletMeter;

	SwitchRow EcsEvapTempMetersRow;
	SaturnEcsRadTempSecOutletMeter EcsRadTempSecOutletMeter;
	SaturnGlyEvapTempOutletMeter GlyEvapTempOutletMeter;

	SwitchRow EcsPressMetersRow;
	SaturnGlyEvapSteamPressMeter GlyEvapSteamPressMeter;
	SaturnGlycolDischPressMeter GlycolDischPressMeter;

	SwitchRow EcsQuantityMetersRow;
	SaturnAccumQuantityMeter AccumQuantityMeter;
	SaturnH2oQuantityMeter H2oQuantityMeter;

	//
	// ECS radiator switches
	//
	SwitchRow EcsRadiatorIndicatorRow;
	IndicatorSwitch EcsRadiatorIndicator;

	SwitchRow EcsRadiatorSwitchesRow;
	ThreePosSwitch EcsRadiatorsFlowContAutoSwitch;
	ThreePosSwitch EcsRadiatorsFlowContPwrSwitch;
	ThreePosSwitch EcsRadiatorsManSelSwitch;
	ThreePosSwitch EcsRadiatorsHeaterPrimSwitch;
	ToggleSwitch EcsRadiatorsHeaterSecSwitch;

	//
	// ECS switches
	//
	SwitchRow EcsSwitchesRow;
	ThreePosSwitch PotH2oHtrSwitch;
	ThreePosSwitch SuitCircuitH2oAccumAutoSwitch;
	ThreePosSwitch SuitCircuitH2oAccumOnSwitch;
	ThreePosSwitch SuitCircuitHeatExchSwitch;
	ThreePosSwitch SecCoolantLoopEvapSwitch;
	ThreeSourceSwitch SecCoolantLoopPumpSwitch;
	ToggleSwitch H2oQtyIndSwitch;
	ToggleSwitch GlycolEvapTempInSwitch;
	ToggleSwitch GlycolEvapSteamPressAutoManSwitch;
	ThreePosSwitch GlycolEvapSteamPressIncrDecrSwitch;
	ThreePosSwitch GlycolEvapH2oFlowSwitch;
	ToggleSwitch CabinTempAutoManSwitch;

	SwitchRow CabinTempAutoControlSwitchRow;
	ThumbwheelSwitch CabinTempAutoControlSwitch;

	SwitchRow EcsGlycolPumpsSwitchRow;
	SaturnEcsGlycolPumpsSwitch EcsGlycolPumpsSwitch;

	//
	// High gain antenna
	//
	
	SwitchRow HighGainAntennaUpperSwitchesRow;
	ThreePosSwitch GHATrackSwitch;
	ThreePosSwitch GHABeamSwitch;

	SwitchRow HighGainAntennaLowerSwitchesRow;
	ThreePosSwitch GHAPowerSwitch;
	ToggleSwitch GHAServoElecSwitch;
	
	SwitchRow HighGainAntennaPitchPositionSwitchRow;
	RotationalSwitch HighGainAntennaPitchPositionSwitch;

	SwitchRow HighGainAntennaYawPositionSwitchRow;
	RotationalSwitch HighGainAntennaYawPositionSwitch;

	SwitchRow HighGainAntennaMetersRow;
	SaturnHighGainAntennaPitchMeter HighGainAntennaPitchMeter;
	SaturnHighGainAntennaStrengthMeter HighGainAntennaStrengthMeter;
	SaturnHighGainAntennaYawMeter HighGainAntennaYawMeter;	

	///
	/// \brief Docking probe switches
	///
	SwitchRow DockingProbeSwitchesRow;
	GuardedThreePosSwitch DockingProbeExtdRelSwitch;
	ThreePosSwitch DockingProbeRetractPrimSwitch;
	ThreePosSwitch DockingProbeRetractSecSwitch;

	SwitchRow DockingProbeIndicatorsRow;
	IndicatorSwitch DockingProbeAIndicator;
	IndicatorSwitch DockingProbeBIndicator;

	//
	// Orbiter switches
	//

	SwitchRow OrbiterAttitudeToggleRow;
	AttitudeToggle OrbiterAttitudeToggle;

	/////////////////////////////
	// Panel 5 circuit brakers //
	/////////////////////////////
	
	// EPS sensor signal circuit brakers

	SwitchRow EpsSensorSignalDcCircuitBrakersRow;
	CircuitBrakerSwitch EpsSensorSignalDcMnaCircuitBraker;
	CircuitBrakerSwitch EpsSensorSignalDcMnbCircuitBraker;
	
	SwitchRow EpsSensorSignalAcCircuitBrakersRow;
	CircuitBrakerSwitch EpsSensorSignalAc1CircuitBraker;
	CircuitBrakerSwitch EpsSensorSignalAc2CircuitBraker;

	// C/W circuit brakers

	SwitchRow CWCircuitBrakersRow;
	CircuitBrakerSwitch CWMnaCircuitBraker;
	CircuitBrakerSwitch CWMnbCircuitBraker;

	// LEM PWR circuit brakers
	
	SwitchRow LMPWRCircuitBrakersRow;
	CircuitBrakerSwitch MnbLMPWR1CircuitBraker;
	CircuitBrakerSwitch MnbLMPWR2CircuitBraker;

	// Inverter control circuit brakers
	
	SwitchRow InverterControlCircuitBrakersRow;
	CircuitBrakerSwitch InverterControl1CircuitBraker;
	CircuitBrakerSwitch InverterControl2CircuitBraker;
	CircuitBrakerSwitch InverterControl3CircuitBraker;

	// EPS sensor unit circuit brakers
	
	SwitchRow EPSSensorUnitCircuitBrakersRow;
	CircuitBrakerSwitch EPSSensorUnitDcBusACircuitBraker;
	CircuitBrakerSwitch EPSSensorUnitDcBusBCircuitBraker;
	CircuitBrakerSwitch EPSSensorUnitAcBus1CircuitBraker;
	CircuitBrakerSwitch EPSSensorUnitAcBus2CircuitBraker;

	// Battery relay bus circuit brakers

	SwitchRow BATRLYBusCircuitBrakersRow;
	CircuitBrakerSwitch BATRLYBusBatACircuitBraker;
	CircuitBrakerSwitch BATRLYBusBatBCircuitBraker;

	// ECS radiators circuit brakers

	SwitchRow ECSRadiatorsCircuitBrakersRow;
	CircuitBrakerSwitch ControllerAc1CircuitBraker;
	CircuitBrakerSwitch ControllerAc2CircuitBraker;
	CircuitBrakerSwitch CONTHTRSMnACircuitBraker;
	CircuitBrakerSwitch CONTHTRSMnBCircuitBraker;
	CircuitBrakerSwitch HTRSOVLDBatACircuitBraker;
	CircuitBrakerSwitch HTRSOVLDBatBCircuitBraker;

	// Battery charger circuit brakers

	SwitchRow BatteryChargerCircuitBrakersRow;
	CircuitBrakerSwitch BatteryChargerBatACircuitBraker;
	CircuitBrakerSwitch BatteryChargerBatBCircuitBraker;
	CircuitBrakerSwitch BatteryChargerMnACircuitBraker;
	CircuitBrakerSwitch BatteryChargerMnBCircuitBraker;
	CircuitBrakerSwitch BatteryChargerAcPwrCircuitBraker;

	// Instrument lighting circuit brakers

	SwitchRow InstrumentLightingCircuitBrakersRow;
	CircuitBrakerSwitch InstrumentLightingESSMnACircuitBraker;
	CircuitBrakerSwitch InstrumentLightingESSMnBCircuitBraker;
	CircuitBrakerSwitch InstrumentLightingNonESSCircuitBraker;
	CircuitBrakerSwitch InstrumentLightingSCIEquipSEP1CircuitBraker;
	CircuitBrakerSwitch InstrumentLightingSCIEquipSEP2CircuitBraker;
	CircuitBrakerSwitch InstrumentLightingSCIEquipHatchCircuitBraker;
	
	// ECS circuit brakers

	SwitchRow ECSCircuitBrakersRow;
	CircuitBrakerSwitch ECSPOTH2OHTRMnACircuitBraker;
	CircuitBrakerSwitch ECSPOTH2OHTRMnBCircuitBraker;
	CircuitBrakerSwitch ECSH2OAccumMnACircuitBraker;
	CircuitBrakerSwitch ECSH2OAccumMnBCircuitBraker;
	CircuitBrakerSwitch ECSTransducerWastePOTH2OMnACircuitBraker;
	CircuitBrakerSwitch ECSTransducerWastePOTH2OMnBCircuitBraker;
	CircuitBrakerSwitch ECSTransducerPressGroup1MnACircuitBraker;
	CircuitBrakerSwitch ECSTransducerPressGroup1MnBCircuitBraker;
	CircuitBrakerSwitch ECSTransducerPressGroup2MnACircuitBraker;
	CircuitBrakerSwitch ECSTransducerPressGroup2MnBCircuitBraker;
	CircuitBrakerSwitch ECSTransducerTempMnACircuitBraker;
	CircuitBrakerSwitch ECSTransducerTempMnBCircuitBraker;

	// ECS circuit brakers lower row 

	SwitchRow ECSLowerRowCircuitBrakersRow;
	CircuitBrakerSwitch ECSSecCoolLoopAc1CircuitBraker;
	CircuitBrakerSwitch ECSSecCoolLoopAc2CircuitBraker;
	CircuitBrakerSwitch ECSSecCoolLoopRADHTRMnACircuitBraker;
	CircuitBrakerSwitch ECSSecCoolLoopXducersMnACircuitBraker;
	CircuitBrakerSwitch ECSSecCoolLoopXducersMnBCircuitBraker;
	CircuitBrakerSwitch ECSWasteH2OUrineDumpHTRMnACircuitBraker;
	CircuitBrakerSwitch ECSWasteH2OUrineDumpHTRMnBCircuitBraker;
	CircuitBrakerSwitch ECSCabinFanAC1ACircuitBraker;
	CircuitBrakerSwitch ECSCabinFanAC1BCircuitBraker;
	CircuitBrakerSwitch ECSCabinFanAC1CCircuitBraker;
	CircuitBrakerSwitch ECSCabinFanAC2ACircuitBraker;
	CircuitBrakerSwitch ECSCabinFanAC2BCircuitBraker;
	CircuitBrakerSwitch ECSCabinFanAC2CCircuitBraker;

	// Guidance/navigation circuit brakers 

	SwitchRow GNCircuitBrakersRow;
	CircuitBrakerSwitch GNPowerAc1CircuitBraker;
	CircuitBrakerSwitch GNPowerAc2CircuitBraker;
	CircuitBrakerSwitch GNIMUMnACircuitBraker;
	CircuitBrakerSwitch GNIMUMnBCircuitBraker;
	CircuitBrakerSwitch GNIMUHTRMnACircuitBraker;
	CircuitBrakerSwitch GNIMUHTRMnBCircuitBraker;
	CircuitBrakerSwitch GNComputerMnACircuitBraker;
	CircuitBrakerSwitch GNComputerMnBCircuitBraker;
	CircuitBrakerSwitch GNOpticsMnACircuitBraker;
	CircuitBrakerSwitch GNOpticsMnBCircuitBraker;

	/////////////////////////////
	// Panel 4 circuit brakers //
	/////////////////////////////
	
	// Suit compressors circuit brakers 

	SwitchRow SuitCompressorsAc1ACircuitBrakerRow;
	CircuitBrakerSwitch SuitCompressorsAc1ACircuitBraker;

	SwitchRow SuitCompressorsAc1BCircuitBrakerRow;
	CircuitBrakerSwitch SuitCompressorsAc1BCircuitBraker;

	SwitchRow SuitCompressorsAc1CCircuitBrakerRow;
	CircuitBrakerSwitch SuitCompressorsAc1CCircuitBraker;

	SwitchRow SuitCompressorsAc2ACircuitBrakerRow;
	CircuitBrakerSwitch SuitCompressorsAc2ACircuitBraker;

	SwitchRow SuitCompressorsAc2BCircuitBrakerRow;
	CircuitBrakerSwitch SuitCompressorsAc2BCircuitBraker;

	SwitchRow SuitCompressorsAc2CCircuitBrakerRow;
	CircuitBrakerSwitch SuitCompressorsAc2CCircuitBraker;

	// ECS glycol pumps circuit brakers 

	SwitchRow ECSGlycolPumpsAc1ACircuitBrakerRow;
	CircuitBrakerSwitch ECSGlycolPumpsAc1ACircuitBraker;

	SwitchRow ECSGlycolPumpsAc1BCircuitBrakerRow;
	CircuitBrakerSwitch ECSGlycolPumpsAc1BCircuitBraker;

	SwitchRow ECSGlycolPumpsAc1CCircuitBrakerRow;
	CircuitBrakerSwitch ECSGlycolPumpsAc1CCircuitBraker;

	SwitchRow ECSGlycolPumpsAc2ACircuitBrakerRow;
	CircuitBrakerSwitch ECSGlycolPumpsAc2ACircuitBraker;

	SwitchRow ECSGlycolPumpsAc2BCircuitBrakerRow;
	CircuitBrakerSwitch ECSGlycolPumpsAc2BCircuitBraker;

	SwitchRow ECSGlycolPumpsAc2CCircuitBrakerRow;
	CircuitBrakerSwitch ECSGlycolPumpsAc2CCircuitBraker;

	/////////////////////////
	// Panel 6 thumbwheels //
	/////////////////////////

	SwitchRow ModeIntercomVOXSensThumbwheelSwitchRow;
	ThumbwheelSwitch ModeIntercomVOXSensThumbwheelSwitch;

	SwitchRow PowerMasterVolumeThumbwheelSwitchRow;
	/// \todo was VolumeThumbwheelSwitch and controlled mission sound volume unrealistically
	ThumbwheelSwitch PowerMasterVolumeThumbwheelSwitch;

	SwitchRow PadCommVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch PadCommVolumeThumbwheelSwitch;

	SwitchRow IntercomVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch IntercomVolumeThumbwheelSwitch;

	SwitchRow SBandVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch SBandVolumeThumbwheelSwitch;

	SwitchRow VHFAMVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch VHFAMVolumeThumbwheelSwitch;

	//////////////////////
	// Panel 6 switches //
	//////////////////////

	SwitchRow RightModeIntercomSwitchRow;
	ThreePosSwitch RightModeIntercomSwitch;

	SwitchRow RightAudioPowerSwitchRow;
	ThreePosSwitch RightAudioPowerSwitch;

	SwitchRow RightPadCommSwitchRow;
	ThreePosSwitch RightPadCommSwitch;

	SwitchRow RightIntercomSwitchRow;
	ThreePosSwitch RightIntercomSwitch;

	SwitchRow RightSBandSwitchRow;
	ThreePosSwitch RightSBandSwitch;

	SwitchRow RightVHFAMSwitchRow;
	ThreePosSwitch RightVHFAMSwitch;
	
	SwitchRow AudioControlSwitchRow;
	ToggleSwitch AudioControlSwitch;

	SwitchRow SuitPowerSwitchRow;
	ToggleSwitch SuitPowerSwitch;

	///////////////////////
	// Panel 16 switches //
	///////////////////////

	SwitchRow RightCOASPowerSwitchRow;
	ToggleSwitch RightCOASPowerSwitch;

	SwitchRow RightUtilityPowerSwitchRow;
	ToggleSwitch RightUtilityPowerSwitch;

	SwitchRow RightDockingTargetSwitchRow;
	DockingTargetSwitch RightDockingTargetSwitch;

	//////////////////////
	// Panel 5 switches //
	//////////////////////

	SwitchRow GNPowerSwitchRow;
	ThreePosSwitch GNPowerSwitch;
	
	SwitchRow MainBusTieSwitchesRow;
	ThreePosSwitch MainBusTieBatAcSwitch;
	ThreePosSwitch MainBusTieBatBcSwitch;

	SwitchRow BatCHGRSwitchRow;
	ThreeSourceSwitch BatCHGRSwitch;

	SwitchRow NonessBusSwitchRow;
	ThreeSourceSwitch NonessBusSwitch;

	SwitchRow InteriorLightsFloodSwitchesRow;
	ToggleSwitch InteriorLightsFloodDimSwitch;
	ToggleSwitch InteriorLightsFloodFixedSwitch;

	//////////////////////
	// Panel 5 rotaries //
	//////////////////////
	
	SwitchRow RightInteriorLightRotariesRow;
	RotationalSwitch RightIntegralRotarySwitch;
	RotationalSwitch RightFloodRotarySwitch;

	//////////////////////
	// Panel 4 switches //
	//////////////////////

	SwitchRow SPSGaugingSwitchRow;
	ThreeSourceSwitch SPSGaugingSwitch;

	SwitchRow TelcomSwitchesRow;
	ThreeSourceSwitch TelcomGroup1Switch;
	ThreeSourceSwitch TelcomGroup2Switch;

	///////////////////////////////
	// Panel 275 circuit brakers //
	///////////////////////////////

	SwitchRow Panel275CircuitBrakersRow;
	CircuitBrakerSwitch InverterPower3MainBCircuitBraker;
	CircuitBrakerSwitch InverterPower3MainACircuitBraker;
	CircuitBrakerSwitch InverterPower2MainBCircuitBraker;
	CircuitBrakerSwitch InverterPower1MainACircuitBraker;
	CircuitBrakerSwitch FlightPostLandingMainBCircuitBraker;
	CircuitBrakerSwitch FlightPostLandingMainACircuitBraker;
	CircuitBrakerSwitch FlightPostLandingBatCCircuitBraker;
	CircuitBrakerSwitch FlightPostLandingBatBusBCircuitBraker;
	
	SwitchRow Panel275CircuitBrakersLowerRow;
	CircuitBrakerSwitch FlightPostLandingBatBusACircuitBraker;
	CircuitBrakerSwitch MainBBatBusBCircuitBraker;
	CircuitBrakerSwitch MainBBatCCircuitBraker;
	CircuitBrakerSwitch MainABatCCircuitBraker;
	CircuitBrakerSwitch MainABatBusACircuitBraker;

	////////////////////////
	// Right window cover //
	////////////////////////

	//SwitchRow RightWindowCoverRow;
	//ToggleSwitch RightWindowCoverSwitch;

	/////////////////////////////
	// G&N lower equipment bay //
	/////////////////////////////

	SwitchRow ZeroSwitchRow;
	CMCOpticsZeroSwitch OpticsZeroSwitch;

	SwitchRow TelescopeTrunnionSwitchRow;
	ThreePosSwitch ControllerTelescopeTrunnionSwitch;

	SwitchRow ControllerCouplingSwitchRow;
	ToggleSwitch ControllerCouplingSwitch;

	SwitchRow OpticsModeSpeedSwitchesRow;
	ToggleSwitch OpticsModeSwitch;
	ThreePosSwitch ControllerSpeedSwitch;

	SwitchRow ConditionLampsSwitchRow;
	ThreePosSwitch ConditionLampsSwitch;

	SwitchRow UPTLMSwitchRow;
	ToggleSwitch UPTLMSwitch;

	SwitchRow OpticsHandcontrollerSwitchRow;
	OpticsHandcontrollerSwitch OpticsHandcontrollerSwitch;

	SwitchRow OpticsMarkButtonRow;
	PushSwitch OpticsMarkButton;

	SwitchRow OpticsMarkRejectButtonRow;
	PushSwitch OpticsMarkRejectButton;

	SwitchRow MinImpulseHandcontrollerSwitchRow;
	MinImpulseHandcontrollerSwitch MinImpulseHandcontrollerSwitch;


	///////////////
	// Panel 100 //
	///////////////

	SwitchRow Panel100SwitchesRow;
	ToggleSwitch UtilityPowerSwitch;
	ToggleSwitch Panel100FloodDimSwitch;	
	ToggleSwitch Panel100FloodFixedSwitch;
	ToggleSwitch GNPowerOpticsSwitch;
	GuardedToggleSwitch GNPowerIMUSwitch;
	ThreePosSwitch Panel100RNDZXPDRSwitch;

	SwitchRow Panel100LightingRoatariesRow;
	RotationalSwitch Panel100NumericRotarySwitch;
	RotationalSwitch Panel100FloodRotarySwitch;
	RotationalSwitch Panel100IntegralRotarySwitch;
	
	///////////////
	// Panel 101 //
	///////////////

	SwitchRow SystemTestRotariesRow;
	RotationalSwitch LeftSystemTestRotarySwitch;
	RotationalSwitch RightSystemTestRotarySwitch;

	SwitchRow RNDZXPDRSwitchRow;
	ToggleSwitch RNDZXPDRSwitch;

	SwitchRow Panel101LowerSwitchRow;
	ToggleSwitch CMRCSHTRSSwitch;
	ThreePosSwitch WasteH2ODumpSwitch;
	ThreePosSwitch UrineDumpSwitch;

	///////////////
	// Panel 163 //
	///////////////

	SwitchRow SCIUtilPowerSwitchRow;
	ToggleSwitch SCIUtilPowerSwitch;

	/////////////////////////////////////
	// Panel 181 (Apollo 15 and later) //
	/////////////////////////////////////
	SaturnPanel181 *Panel181;

	///////////////////////
	// Panel 15 switches //
	///////////////////////

	SwitchRow LeftCOASPowerSwitchRow;
	ToggleSwitch LeftCOASPowerSwitch;

	SwitchRow LeftUtilityPowerSwitchRow;
	ToggleSwitch LeftUtilityPowerSwitch;

	SwitchRow PostLandingBCNLTSwitchRow;
	ThreePosSwitch PostLandingBCNLTSwitch;

	SwitchRow PostLandingDYEMarkerSwitchRow;
	GuardedToggleSwitch PostLandingDYEMarkerSwitch;

	SwitchRow PostLandingVentSwitchRow;
	ThreePosSwitch PostLandingVentSwitch;

	/////////////////////////
	// Panel 9 thumbwheels //
	/////////////////////////

	SwitchRow LeftModeIntercomVOXSensThumbwheelSwitchRow;
	ThumbwheelSwitch LeftModeIntercomVOXSensThumbwheelSwitch;

	SwitchRow LeftPowerMasterVolumeThumbwheelSwitchRow;
	/// \todo was VolumeThumbwheelSwitch and controlled mission sound volume unrealistically
	ThumbwheelSwitch LeftPowerMasterVolumeThumbwheelSwitch;

	SwitchRow LeftPadCommVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch LeftPadCommVolumeThumbwheelSwitch;

	SwitchRow LeftIntercomVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch LeftIntercomVolumeThumbwheelSwitch;

	SwitchRow LeftSBandVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch LeftSBandVolumeThumbwheelSwitch;

	SwitchRow LeftVHFAMVolumeThumbwheelSwitchRow;
	ThumbwheelSwitch LeftVHFAMVolumeThumbwheelSwitch;

	//////////////////////
	// Panel 9 switches //
	//////////////////////

	SwitchRow LeftModeIntercomSwitchRow;
	ThreePosSwitch LeftModeIntercomSwitch;

	SwitchRow LeftAudioPowerSwitchRow;
	ThreePosSwitch LeftAudioPowerSwitch;

	SwitchRow LeftPadCommSwitchRow;
	ThreePosSwitch LeftPadCommSwitch;

	SwitchRow LeftIntercomSwitchRow;
	ThreePosSwitch LeftIntercomSwitch;

	SwitchRow LeftSBandSwitchRow;
	ThreePosSwitch LeftSBandSwitch;

	SwitchRow LeftVHFAMSwitchRow;
	ThreePosSwitch LeftVHFAMSwitch;

	SwitchRow LeftAudioControlSwitchRow;
	ThreePosSwitch LeftAudioControlSwitch;

	SwitchRow LeftSuitPowerSwitchRow;
	ToggleSwitch LeftSuitPowerSwitch;

	SwitchRow VHFRNGSwitchRow;
	ToggleSwitch VHFRNGSwitch;

	SwitchRow FloatBagSwitchRow;
	ThreePosSwitch FloatBagSwitch1;
	ThreePosSwitch FloatBagSwitch2;
	ThreePosSwitch FloatBagSwitch3;

	SwitchRow SeqEventsContSystemSwitchesRow;
	ToggleSwitch SECSLogic1Switch;
	ToggleSwitch SECSLogic2Switch;
	ToggleSwitch PyroArmASwitch;
	ToggleSwitch PyroArmBSwitch;

	//////////////////////////////
	// Panel 8 circuit breakers //
	//////////////////////////////

	SwitchRow StabContSystemCircuitBrakerRow;
	CircuitBrakerSwitch StabContSystemTVCAc1CircuitBraker;
	CircuitBrakerSwitch StabContSystemAc1CircuitBraker;
	CircuitBrakerSwitch StabContSystemAc2CircuitBraker;

	SwitchRow StabilizationControlSystemCircuitBrakerRow;
	CircuitBrakerSwitch ECATVCAc2CircuitBraker;
	CircuitBrakerSwitch DirectUllMnACircuitBraker;
	CircuitBrakerSwitch DirectUllMnBCircuitBraker;
	CircuitBrakerSwitch ContrDirectMnA1CircuitBraker;
	CircuitBrakerSwitch ContrDirectMnB1CircuitBraker;
	CircuitBrakerSwitch ContrDirectMnA2CircuitBraker;
	CircuitBrakerSwitch ContrDirectMnB2CircuitBraker;
	CircuitBrakerSwitch ACRollMnACircuitBraker;
	CircuitBrakerSwitch ACRollMnBCircuitBraker;
	CircuitBrakerSwitch BDRollMnACircuitBraker;
	CircuitBrakerSwitch BDRollMnBCircuitBraker;
	CircuitBrakerSwitch PitchMnACircuitBraker;
	CircuitBrakerSwitch PitchMnBCircuitBraker;
	CircuitBrakerSwitch YawMnACircuitBraker;
	CircuitBrakerSwitch YawMnBCircuitBraker;

	SwitchRow StabilizationControlSystem2CircuitBrakerRow;
	CircuitBrakerSwitch OrdealAc2CircuitBraker;
	CircuitBrakerSwitch OrdealMnBCircuitBraker;
	CircuitBrakerSwitch ContrAutoMnACircuitBraker;
	CircuitBrakerSwitch ContrAutoMnBCircuitBraker;
	CircuitBrakerSwitch LogicBus12MnACircuitBraker;
	CircuitBrakerSwitch LogicBus34MnACircuitBraker;
	CircuitBrakerSwitch LogicBus14MnBCircuitBraker;
	CircuitBrakerSwitch LogicBus23MnBCircuitBraker;
	CircuitBrakerSwitch SystemMnACircuitBraker;
	CircuitBrakerSwitch SystemMnBCircuitBraker;

	SwitchRow ReactionControlSystemCircuitBrakerRow;
	CircuitBrakerSwitch CMHeater1MnACircuitBraker;
	CircuitBrakerSwitch CMHeater2MnBCircuitBraker;
	CircuitBrakerSwitch SMHeatersAMnBCircuitBraker;
	CircuitBrakerSwitch SMHeatersCMnBCircuitBraker;
	CircuitBrakerSwitch SMHeatersBMnACircuitBraker;
	CircuitBrakerSwitch SMHeatersDMnACircuitBraker;
	CircuitBrakerSwitch PrplntIsolMnACircuitBraker;
	CircuitBrakerSwitch PrplntIsolMnBCircuitBraker;
	CircuitBrakerSwitch RCSLogicMnACircuitBraker;
	CircuitBrakerSwitch RCSLogicMnBCircuitBraker;
	CircuitBrakerSwitch EMSMnACircuitBraker;
	CircuitBrakerSwitch EMSMnBCircuitBraker;
	CircuitBrakerSwitch DockProbeMnACircuitBraker;
	CircuitBrakerSwitch DockProbeMnBCircuitBraker;

	SwitchRow ServicePropulsionSysCircuitBrakerRow;
	CircuitBrakerSwitch GaugingMnACircuitBraker;
	CircuitBrakerSwitch GaugingMnBCircuitBraker;
	CircuitBrakerSwitch GaugingAc1CircuitBraker;
	CircuitBrakerSwitch GaugingAc2CircuitBraker;
	CircuitBrakerSwitch HeValveMnACircuitBraker;
	CircuitBrakerSwitch HeValveMnBCircuitBraker;
	CircuitBrakerSwitch PitchBatACircuitBraker;
	CircuitBrakerSwitch PitchBatBCircuitBraker;
	CircuitBrakerSwitch YawBatACircuitBraker;
	CircuitBrakerSwitch YawBatBCircuitBraker;
	CircuitBrakerSwitch PilotValveMnACircuitBraker;
	CircuitBrakerSwitch PilotValveMnBCircuitBraker;

	SwitchRow FloatBagCircuitBrakerRow;
	CircuitBrakerSwitch FloatBag1BatACircuitBraker;
	CircuitBrakerSwitch FloatBag2BatBCircuitBraker;
	CircuitBrakerSwitch FloatBag3FLTPLCircuitBraker;

	SwitchRow SeqEventsContSysCircuitBrakerRow;
	CircuitBrakerSwitch SECSLogicBatACircuitBraker;
	CircuitBrakerSwitch SECSLogicBatBCircuitBraker;
	CircuitBrakerSwitch SECSArmBatACircuitBraker;
	CircuitBrakerSwitch SECSArmBatBCircuitBraker;

	SwitchRow EDSCircuitBrakerRow;
	CircuitBrakerSwitch EDS1BatACircuitBraker;
	CircuitBrakerSwitch EDS2BatCCircuitBraker;
	CircuitBrakerSwitch EDS3BatBCircuitBraker;

	SwitchRow ELSCircuitBrakerRow;
	CircuitBrakerSwitch ELSBatACircuitBraker;
	CircuitBrakerSwitch ELSBatBCircuitBraker;

	SwitchRow PLVentCircuitBrakerRow;
	CircuitBrakerSwitch FLTPLCircuitBraker;

	//////////////////////
	// Panel 8 switches //
	//////////////////////
	
	SwitchRow AutoRCSSelectSwitchesRow;
	ThreeSourceSwitch AcRollA1Switch;
	ThreeSourceSwitch AcRollC1Switch;
	ThreeSourceSwitch AcRollA2Switch;
	ThreeSourceSwitch AcRollC2Switch;
	ThreeSourceSwitch BdRollB1Switch;
	ThreeSourceSwitch BdRollD1Switch;
	ThreeSourceSwitch BdRollB2Switch;
	ThreeSourceSwitch BdRollD2Switch;
	ThreeSourceSwitch PitchA3Switch;
	ThreeSourceSwitch PitchC3Switch;
	ThreeSourceSwitch PitchA4Switch;
	ThreeSourceSwitch PitchC4Switch;
	ThreeSourceSwitch YawB3Switch;
	ThreeSourceSwitch YawD3Switch;
	ThreeSourceSwitch YawB4Switch;
	ThreeSourceSwitch YawD4Switch;

	SwitchRow FloodDimSwitchRow;
	ToggleSwitch FloodDimSwitch;

	SwitchRow FloodFixedSwitchRow;
	ThreePosSwitch FloodFixedSwitch;

	//////////////////////
	// Panel 7 switches //
	//////////////////////

	SwitchRow EDSPowerSwitchRow;
	ToggleSwitch EDSPowerSwitch;

	SwitchRow TVCServoPowerSwitchesRow;
	ThreePosSwitch TVCServoPower1Switch;
	ThreePosSwitch TVCServoPower2Switch;

	SwitchRow LogicPowerSwitchRow;
	NSourceDestSwitch LogicPowerSwitch;

	SwitchRow SIGCondDriverBiasPowerSwitchesRow;
	ThreeSourceSwitch SIGCondDriverBiasPower1Switch;
	ThreeSourceSwitch SIGCondDriverBiasPower2Switch;

	//////////////////////
	// Panel 7 rotaries //
	//////////////////////

	SwitchRow LeftInteriorLightRotariesRow;
	RotationalSwitch NumericRotarySwitch;
	RotationalSwitch FloodRotarySwitch;
	RotationalSwitch IntegralRotarySwitch;

	SwitchRow FDAIPowerRotaryRow;
	FDAIPowerRotationalSwitch FDAIPowerRotarySwitch;

	SwitchRow SCSElectronicsPowerRotaryRow;
	RotationalSwitch SCSElectronicsPowerRotarySwitch;

	SwitchRow BMAGPowerRotary1Row;
	BMAGPowerRotationalSwitch BMAGPowerRotary1Switch;

	SwitchRow BMAGPowerRotary2Row;
	BMAGPowerRotationalSwitch BMAGPowerRotary2Switch;

	SwitchRow DirectO2RotaryRow;
	DirectO2RotationalSwitch DirectO2RotarySwitch;

	/////////////////
	// Panel 10/12 //
	/////////////////

	SwitchRow LeftAudioSwitchesRow;
	ThreePosSwitch ModeIntercomSwitch;
	ThreePosSwitch PadComSwitch;
	ThreePosSwitch SBandSwitch;

	SwitchRow LeftAudioThumbwheelsRow;
	ThumbwheelSwitch LeftAudioVOXSensThumbwheel;
	ThumbwheelSwitch LeftAudioPadComVolumeThumbwheel;
	ThumbwheelSwitch LeftAudioSBandVolumeThumbwheel;

	SwitchRow CenterAudioSwitchesRow;
	ToggleSwitch CenterSuitPowerSwitch;
	ToggleSwitch CenterAudioControlSwitch;

	SwitchRow RightAudioThumbwheelsRow;
	ThumbwheelSwitch RightAudioMasterVolumeThumbwheel;
	ThumbwheelSwitch RightAudioIntercomVolumeThumbwheel;
	ThumbwheelSwitch RightAudioVHFAMVolumeThumbwheel;

	SwitchRow RightAudioSwitchesRow;
	ThreePosSwitch PowerAudioSwitch;
	ThreePosSwitch IntercomSwitch;
	ThreePosSwitch VHFAMSwitch;

	SwitchRow LMTunnelVentValveRow;
	RotationalSwitch LMTunnelVentValve;

	SwitchRow LMDPGaugeRow;
	SaturnLMDPGauge LMDPGauge;

	SwitchRow PressEqualValveRow;
	RotationalSwitch PressEqualValve;

	///////////////////
	// Panel 225/226 //
	///////////////////
	
	SwitchRow Panel225CircuitBreakersRow;
	CircuitBrakerSwitch PCMTLMGroup1CB;
	CircuitBrakerSwitch PCMTLMGroup2CB;
	CircuitBrakerSwitch FLTBusMNACB;
	CircuitBrakerSwitch FLTBusMNBCB;
	CircuitBrakerSwitch PMPPowerPrimCB;
	CircuitBrakerSwitch PMPPowerAuxCB;
	CircuitBrakerSwitch VHFStationAudioLCB;
	CircuitBrakerSwitch VHFStationAudioCTRCB;
	CircuitBrakerSwitch VHFStationAudioRCB;
	CircuitBrakerSwitch UDLCB;
	CircuitBrakerSwitch HGAFLTBus1CB;
	CircuitBrakerSwitch HGAGroup2CB;
	CircuitBrakerSwitch SBandFMXMTRFLTBusCB;
	CircuitBrakerSwitch SBandFMXMTRGroup1CB;
	CircuitBrakerSwitch CentralTimingEquipMNACB;
	CircuitBrakerSwitch CentralTimingEquipMNBCB;
	CircuitBrakerSwitch RNDZXPNDRFLTBusCB;
	CircuitBrakerSwitch SIGCondrFLTBusCB;
	CircuitBrakerSwitch SBandPWRAmpl1FLTBusCB;
	CircuitBrakerSwitch SBandPWRAmpl1Group1CB;
	CircuitBrakerSwitch SBandPWRAmpl2FLTBusCB;
	CircuitBrakerSwitch SBandPWRAmpl2Group1CB;

	SwitchRow Panel226CircuitBreakersRow;
	CircuitBrakerSwitch FuelCell1PumpsACCB;
	CircuitBrakerSwitch FuelCell1ReacsCB;
	CircuitBrakerSwitch FuelCell1BusContCB;
	CircuitBrakerSwitch FuelCell1PurgeCB;
	CircuitBrakerSwitch FuelCell1RadCB;
	CircuitBrakerSwitch CryogenicH2HTR1CB;
	CircuitBrakerSwitch CryogenicH2HTR2CB;
	CircuitBrakerSwitch FuelCell2PumpsACCB;
	CircuitBrakerSwitch FuelCell2ReacsCB;
	CircuitBrakerSwitch FuelCell2BusContCB;
	CircuitBrakerSwitch FuelCell2PurgeCB;
	CircuitBrakerSwitch FuelCell2RadCB;
	CircuitBrakerSwitch CryogenicO2HTR1CB;
	CircuitBrakerSwitch CryogenicO2HTR2CB;
	CircuitBrakerSwitch FuelCell3PumpsACCB;
	CircuitBrakerSwitch FuelCell3ReacsCB;
	CircuitBrakerSwitch FuelCell3BusContCB;
	CircuitBrakerSwitch FuelCell3PurgeCB;
	CircuitBrakerSwitch FuelCell3RadCB;
	CircuitBrakerSwitch CryogenicQTYAmpl1CB;
	CircuitBrakerSwitch CryogenicQTYAmpl2CB;
	CircuitBrakerSwitch CryogenicFanMotorsAC1ACB;
	CircuitBrakerSwitch CryogenicFanMotorsAC1BCB;
	CircuitBrakerSwitch CryogenicFanMotorsAC1CCB;
	CircuitBrakerSwitch CryogenicFanMotorsAC2ACB;
	CircuitBrakerSwitch CryogenicFanMotorsAC2BCB;
	CircuitBrakerSwitch CryogenicFanMotorsAC2CCB;
	CircuitBrakerSwitch LightingRndzMNACB;
	CircuitBrakerSwitch LightingRndzMNBCB;
	CircuitBrakerSwitch LightingFloodMNACB;
	CircuitBrakerSwitch LightingFloodMNBCB;
	CircuitBrakerSwitch LightingFloodFLTPLCB;
	CircuitBrakerSwitch LightingNumIntLEBCB;
	CircuitBrakerSwitch LightingNumIntLMDCCB;
	CircuitBrakerSwitch LightingNumIntRMDCCB;
	CircuitBrakerSwitch RunEVATRGTAC1CB;
	CircuitBrakerSwitch RunEVATRGTAC2CB;
	
	///////////////
	// Panel 227 //
	///////////////

	SwitchRow SCIInstSwitchRow;
	ToggleSwitch SCIInstSwitch;

	////////////////////////////////////////
	// Panel 229 - Right Instrument Panel //
	////////////////////////////////////////

	SwitchRow Panel229CircuitBreakersRow;
	CircuitBrakerSwitch TimersMnACircuitBraker;
	CircuitBrakerSwitch TimersMnBCircuitBraker;
	CircuitBrakerSwitch EPSMnAGroup1CircuitBraker;
	CircuitBrakerSwitch EPSMnBGroup1CircuitBraker;
	CircuitBrakerSwitch SPSLineHtrsMnACircuitBraker;
	CircuitBrakerSwitch SPSLineHtrsMnBCircuitBraker;
	CircuitBrakerSwitch EPSMnAGroup2CircuitBraker;
	CircuitBrakerSwitch EPSMnBGroup2CircuitBraker;
	CircuitBrakerSwitch O2VacIonPumpsMnACircuitBraker;
	CircuitBrakerSwitch O2VacIonPumpsMnBCircuitBraker;
	CircuitBrakerSwitch EPSMnAGroup3CircuitBraker;
	CircuitBrakerSwitch EPSMnBGroup3CircuitBraker;
	CircuitBrakerSwitch MainReleasePyroACircuitBraker;
	CircuitBrakerSwitch MainReleasePyroBCircuitBraker;
	CircuitBrakerSwitch EPSMnAGroup4CircuitBraker;
	CircuitBrakerSwitch EPSMnBGroup4CircuitBraker;
	CircuitBrakerSwitch EPSMnAGroup5CircuitBraker;
	CircuitBrakerSwitch EPSMnBGroup5CircuitBraker;
	CircuitBrakerSwitch UtilityCB1;
	CircuitBrakerSwitch UtilityCB2;
	CircuitBrakerSwitch EPSBatBusACircuitBraker;
	CircuitBrakerSwitch EPSBatBusBCircuitBraker;

	///////////////////////
	// Panel 250/251/252 //
	///////////////////////

	SwitchRow Panel250CircuitBreakersRow;
	CircuitBrakerSwitch BatBusAToPyroBusTieCircuitBraker;
	CircuitBrakerSwitch PyroASeqACircuitBraker;
	CircuitBrakerSwitch BatBusBToPyroBusTieCircuitBraker;
	CircuitBrakerSwitch PyroBSeqBCircuitBraker;
	CircuitBrakerSwitch BatAPWRCircuitBraker;
	CircuitBrakerSwitch BatBPWRCircuitBraker;
	CircuitBrakerSwitch BatCPWRCircuitBraker;
	CircuitBrakerSwitch BatCtoBatBusACircuitBraker;
	CircuitBrakerSwitch BatCtoBatBusBCircuitBraker;
	CircuitBrakerSwitch BatCCHRGCircuitBraker;

	SwitchRow WasteMGMTOvbdDrainDumpRotaryRow;
	RotationalSwitch WasteMGMTOvbdDrainDumpRotary;

	SwitchRow WasteMGMTBatteryVentRotaryRow;
	RotationalSwitch WasteMGMTBatteryVentRotary;

	SwitchRow WasteMGMTStoageVentRotaryRow;
	RotationalSwitch WasteMGMTStoageVentRotary;

	SwitchRow WasteDisposalSwitchRow;
	ThreePosSwitch WasteDisposalSwitch;
	
	///////////////////
	// Panel 276/278 //
	///////////////////

	SwitchRow Panel276CBRow;
	CircuitBrakerSwitch Panel276CB1;
	CircuitBrakerSwitch Panel276CB2;
	CircuitBrakerSwitch Panel276CB3;
	CircuitBrakerSwitch Panel276CB4;
	
	SwitchRow Panel278CBRow;
	CircuitBrakerSwitch UprightingSystemCompressor1CircuitBraker;
	CircuitBrakerSwitch UprightingSystemCompressor2CircuitBraker;
	CircuitBrakerSwitch SIVBLMSepPyroACircuitBraker;
	CircuitBrakerSwitch SIVBLMSepPyroBCircuitBraker;

	// Panel 278 Mission-Specific Additions
	SaturnPanel278J *Panel278J;

	/////////////////////////////////////
	// Panel 277 (Apollo 15 and later) //
	/////////////////////////////////////
	SaturnPanel277 *Panel277;
	
	///////////////////////////////
	// Panel 300/301/302/303/305 //
	///////////////////////////////

	SwitchRow SuitCircuitFlow300SwitchRow;
	ThreePosSwitch SuitCircuitFlow300Switch;

	SwitchRow SuitCircuitFlow301SwitchRow;
	ThreePosSwitch SuitCircuitFlow301Switch;

	SwitchRow SuitCircuitFlow302SwitchRow;
	ThreePosSwitch SuitCircuitFlow302Switch;

	SwitchRow SecondaryCabinTempValveRow;
	RotationalSwitch SecondaryCabinTempValve;

	SwitchRow FoodPreparationWaterLeversRow;
	ToggleSwitch FoodPreparationWaterHotLever;
	ToggleSwitch FoodPreparationWaterColdLever;
	
	SwitchRow Panel306Row;
	EventTimerResetSwitch EventTimerUpDown306Switch;
	EventTimerControlSwitch EventTimerControl306Switch;
	TimerUpdateSwitch EventTimer306MinutesSwitch;
	TimerUpdateSwitch EventTimer306SecondsSwitch;
	TimerUpdateSwitch MissionTimer306HoursSwitch;
	TimerUpdateSwitch MissionTimer306MinutesSwitch;
	TimerUpdateSwitch MissionTimer306SecondsSwitch;

	SwitchRow MissionTimer306SwitchRow;
	TimerControlSwitch MissionTimer306Switch;


	////////////////////////
	// Panel 325/326 etc. //
	////////////////////////

	SwitchRow GlycolToRadiatorsLeverRow; 	
	CircuitBrakerSwitch GlycolToRadiatorsLever;

	SwitchRow CabinPressureReliefLever1Row;
	ThumbwheelSwitch CabinPressureReliefLever1;

	SwitchRow CabinPressureReliefLever2Row;
	SaturnCabinPressureReliefLever CabinPressureReliefLever2;

	SwitchRow GlycolReservoirRotariesRow;
	RotationalSwitch GlycolReservoirInletRotary;
	RotationalSwitch GlycolReservoirBypassRotary;
	RotationalSwitch GlycolReservoirOutletRotary;

	SwitchRow OxygenRotariesRow;
	RotationalSwitch OxygenSurgeTankRotary;
	RotationalSwitch OxygenSMSupplyRotary;
	RotationalSwitch OxygenRepressPackageRotary;

	///////////////
	// Panel 380 //
	///////////////


	SwitchRow SuitCircuitReturnValveLeverRow;
	CircuitBrakerSwitch SuitCircuitReturnValveLever;

	SwitchRow O2DemandRegulatorRotaryRow;
	RotationalSwitch O2DemandRegulatorRotary;

	SwitchRow SuitTestRotaryRow;
	SuitTestSwitch SuitTestRotary;


	///////////////////////////
	// Panel 375/377/378/379 //
	///////////////////////////

	SwitchRow OxygenSurgeTankValveRotaryRow;
	RotationalSwitch OxygenSurgeTankValveRotary;

	SwitchRow GlycolToRadiatorsRotaryRow;
	RotationalSwitch GlycolToRadiatorsRotary;

	SwitchRow AccumRotaryRow;
	RotationalSwitch AccumRotary;

	SwitchRow GlycolRotaryRow;
	RotationalSwitch GlycolRotary;

	SwitchRow PLVCSwitchRow;
	ToggleSwitch PLVCSwitch;


	///////////////////////////
	// Panel 382             //
	///////////////////////////

	SwitchRow Panel382Row;
	RotationalSwitch EvapWaterControlPrimaryRotary;
	RotationalSwitch EvapWaterControlSecondaryRotary;
	RotationalSwitch WaterAccumulator1Rotary;
	RotationalSwitch WaterAccumulator2Rotary;
	RotationalSwitch PrimaryGlycolEvapInletTempRotary;
	RotationalSwitch SuitFlowReliefRotary;
	RotationalSwitch SuitHeatExchangerPrimaryGlycolRotary;
	RotationalSwitch SuitHeatExchangerSecondaryGlycolRotary;
	SaturnPanel382Cover Panel382Cover;		// Dummy switch/display for checklist controller

	///////////////
	// Panel 351 //
	///////////////

	SwitchRow O2MainRegulatorSwitchesRow;
	CircuitBrakerSwitch O2MainRegulatorASwitch;
	CircuitBrakerSwitch O2MainRegulatorBSwitch;

	SwitchRow CabinRepressValveRotaryRow;
	RotationalSwitch CabinRepressValveRotary;
	
	SwitchRow WaterGlycolTanksRotariesRow;
	RotationalSwitch SelectorInletValveRotary;
	RotationalSwitch SelectorOutletValveRotary;

	SwitchRow EmergencyCabinPressureRotaryRow;
	RotationalSwitch EmergencyCabinPressureRotary;

	SwitchRow EmergencyCabinPressureTestSwitchRow;
	PushSwitch EmergencyCabinPressureTestSwitch;

	///////////////
	// Panel 352 //
	///////////////

	SwitchRow WaterControlPanelRow;
	RotationalSwitch PressureReliefRotary;
	RotationalSwitch WasteTankInletRotary;
	RotationalSwitch PotableTankInletRotary;
	RotationalSwitch WasteTankServicingRotary;

	///////////////////////
	// Panel 13 switches //
	///////////////////////

	SwitchRow ORDEALSwitchesRow;
	ToggleSwitch ORDEALFDAI1Switch;
	ToggleSwitch ORDEALFDAI2Switch;
	ThreePosSwitch ORDEALEarthSwitch;
	ThreePosSwitch ORDEALLightingSwitch; 
	ToggleSwitch ORDEALModeSwitch;	
	ThreePosSwitch ORDEALSlewSwitch;
	OrdealRotationalSwitch ORDEALAltSetRotary;

	SaturnPanelOrdeal PanelOrdeal;		// Dummy switch/display for checklist controller


	///////////////////////
	// Hatch             //
	///////////////////////

	SwitchRow HatchGearBoxSelectorRow;
	RotationalSwitch HatchGearBoxSelector;

	SwitchRow HatchActuatorHandleSelectorRow;
	RotationalSwitch HatchActuatorHandleSelector;

	SwitchRow HatchVentValveRotaryRow;
	RotationalSwitch HatchVentValveRotary;

	SwitchRow HatchActuatorHandleSelectorOpenRow;
	RotationalSwitch HatchActuatorHandleSelectorOpen;

	SwitchRow HatchToggleRow;
	PushSwitch HatchToggle;

	SwitchRow HatchPanel600LeftRow;
	ToggleSwitch HatchEmergencyO2ValveSwitch;
	SaturnOxygenRepressPressMeter HatchOxygenRepressPressMeter;

	SwitchRow HatchPanel600RightRow;
	ThreePosSwitch HatchRepressO2ValveSwitch;

	SaturnPanel600 Panel600;		// Dummy switch/display for checklist controller


	///////////////////////
	// DSKYs             //
	///////////////////////

	SwitchRow DskySwitchRow;
	DSKYPushSwitch DskySwitchVerb;
	DSKYPushSwitch DskySwitchNoun;
	DSKYPushSwitch DskySwitchPlus;
	DSKYPushSwitch DskySwitchMinus;
	DSKYPushSwitch DskySwitchZero;
	DSKYPushSwitch DskySwitchOne;
	DSKYPushSwitch DskySwitchTwo;
	DSKYPushSwitch DskySwitchThree;
	DSKYPushSwitch DskySwitchFour;
	DSKYPushSwitch DskySwitchFive;
	DSKYPushSwitch DskySwitchSix;
	DSKYPushSwitch DskySwitchSeven;
	DSKYPushSwitch DskySwitchEight;
	DSKYPushSwitch DskySwitchNine;
	DSKYPushSwitch DskySwitchClear;
	DSKYPushSwitch DskySwitchProg;
	DSKYPushSwitch DskySwitchKeyRel;
	DSKYPushSwitch DskySwitchEnter;
	DSKYPushSwitch DskySwitchReset;

	SwitchRow Dsky2SwitchRow;
	DSKYPushSwitch Dsky2SwitchVerb;
	DSKYPushSwitch Dsky2SwitchNoun;
	DSKYPushSwitch Dsky2SwitchPlus;
	DSKYPushSwitch Dsky2SwitchMinus;
	DSKYPushSwitch Dsky2SwitchZero;
	DSKYPushSwitch Dsky2SwitchOne;
	DSKYPushSwitch Dsky2SwitchTwo;
	DSKYPushSwitch Dsky2SwitchThree;
	DSKYPushSwitch Dsky2SwitchFour;
	DSKYPushSwitch Dsky2SwitchFive;
	DSKYPushSwitch Dsky2SwitchSix;
	DSKYPushSwitch Dsky2SwitchSeven;
	DSKYPushSwitch Dsky2SwitchEight;
	DSKYPushSwitch Dsky2SwitchNine;
	DSKYPushSwitch Dsky2SwitchClear;
	DSKYPushSwitch Dsky2SwitchProg;
	DSKYPushSwitch Dsky2SwitchKeyRel;
	DSKYPushSwitch Dsky2SwitchEnter;
	DSKYPushSwitch Dsky2SwitchReset;

	SaturnASCPSwitch ASCPRollSwitch;		// Dummy switch/display for checklist controller
	SaturnASCPSwitch ASCPPitchSwitch;
	SaturnASCPSwitch ASCPYawSwitch;


	///
	/// Stage is the main stage of the flight.
	/// \brief Flight stage.
	///
	int stage;

	///
	/// StageState is the current state within the flight stage.
	/// \brief State within flight stage.
	///
	int StageState;

	///
	/// Which type of Saturn are we simulating? e.g. Saturn 1b, Saturn V, INT-20.
	/// \brief Saturn rocket type.
	///
	int SaturnType;

	#define SATSYSTEMS_NONE				  0
	#define SATSYSTEMS_PRELAUNCH		100
	#define SATSYSTEMS_CREWINGRESS_1	200
	#define SATSYSTEMS_CREWINGRESS_2	210
	#define SATSYSTEMS_CABINCLOSEOUT	300
	#define SATSYSTEMS_GSECONNECTED_1	400
	#define SATSYSTEMS_GSECONNECTED_2	410
	#define SATSYSTEMS_READYTOLAUNCH    500
	#define SATSYSTEMS_CABINVENTING		600
	#define SATSYSTEMS_FLIGHT			700
	#define SATSYSTEMS_LANDING			800

	int systemsState;
	bool firstSystemsTimeStepDone;
	double lastSystemsMissionTime;

	//
	// Stage masses: should really be saved, but probably aren't at the
	// moment.
	//

	///
	/// Command module total mass in kg.
	/// \brief CM total mass.
	///
	double CM_Mass;

	///
	/// Command module empty mass in kg.
	/// \brief CM empty mass.
	///
	double CM_EmptyMass;

	///
	/// Command module fuel mass in kg.
	/// \brief CM fuel mass.
	///
	double CM_FuelMass;

	///
	/// Service module total mass in kg.
	/// \brief SM total mass.
	///
	double SM_Mass;

	///
	/// Service module empty mass in kg.
	/// \brief SM empty mass.
	///
	double SM_EmptyMass;

	///
	/// Service module fuel mass in kg.
	/// \brief SM fuel mass.
	///
	double SM_FuelMass;

	///
	/// SIVb payload mass in kg.
	/// \brief SIVb payload mass.
	///
	double S4PL_Mass;

	double Abort_Mass;
	double Interstage_Mass;
	double S4B_Mass;
	double S4B_EmptyMass;
	double S4B_FuelMass;
	double SII_Mass;
	double SII_EmptyMass;
	double SII_FuelMass;
	double SI_Mass;
	double SI_EmptyMass;
	double SI_FuelMass;

	bool SI_MassLoaded;
	bool SII_MassLoaded;
	bool S4B_MassLoaded;

	bool S1_ThrustLoaded;
	bool S2_ThrustLoaded;
	bool S3_ThrustLoaded;

	//
	// Calculated total stage mass.
	//

	double Stage1Mass;
	double Stage2Mass;
	double Stage3Mass;

	//
	// Engine numbers.
	//

	///
	/// \brief Number of engines on stage one.
	///
	int SI_EngineNum;

	///
	/// \brief Number of engines on stage two.
	///
	int SII_EngineNum;

	///
	/// \brief Number of engines on stage three.
	///
	int SIII_EngineNum;

	///
	/// Engine indicator lights on control panel: true for lit. Five used on Saturn V, eight on
	/// Saturn 1b.
	/// \brief Engine indicator lights.
	///
	bool ENGIND[9];
	bool LVGuidLight;
	bool LVRateLight;

	//
	// And state that doesn't need to be saved.
	//

	double aHAcc;

	///
	/// Mesh offset for BPC and LET.
	/// \brief Tower mesh offset.
	///
	double TowerOffset;

	///
	/// Mesh offset for SIVb.
	/// \brief SIVb mesh offset.
	///
	double S4Offset;

	double actualFUEL;

	#define LASTVELOCITYCOUNT 50
	VECTOR3 LastVelocity[LASTVELOCITYCOUNT];
	double LastSimt[LASTVELOCITYCOUNT];
	int LastVelocityFilled;

	bool KEY1;
	bool KEY2;
	bool KEY3;
	bool KEY4;
	bool KEY5;
	bool KEY6;
	bool KEY7;
	bool KEY8;
	bool KEY9;

	//
	// State variables that must be initialised but not saved.
	//

	///
	/// \brief Flag for first timestep.
	///
	bool FirstTimestep;

	///
	/// \brief Flag for first call to generic timestep function.
	///
	bool GenericFirstTimestep;

	int CurrentTimestep;
	int LongestTimestep;
	double LongestTimestepLength;
	VECTOR3 normal;

	PanelSwitches MainPanel;
	PanelSwitchScenarioHandler PSH;

	SwitchRow SequencerSwitchesRow;
	SwitchRow SeparationSwitchesRow;
	SwitchRow CryoTankSwitchesRow;
	SwitchRow FuelCellPhRadTempIndicatorsRow;
	SwitchRow FuelCellRadiatorsIndicatorsRow;
	SwitchRow FuelCellRadiatorsSwitchesRow;
	SwitchRow FuelCellIndicatorsSwitchRow;
	SwitchRow FuelCellHeatersSwitchesRow;
	SwitchRow FuelCellPurgeSwitchesRow;
	SwitchRow FuelCellReactantsIndicatorsRow;
	SwitchRow FuelCellReactantsSwitchesRow;
	SwitchRow FuelCellLatchSwitchesRow;
	SwitchRow FuelCellPumpsSwitchesRow;
	SwitchRow SuitCompressorSwitchesRow;
	SwitchRow MainBusAIndicatorsRow;

	SwitchRow SBandNormalSwitchesRow;

	///////////////////////////////////////////////////////
	// Internal systems devices.						 //
	///////////////////////////////////////////////////////

	// SCS components
	BMAG bmag1;
	BMAG bmag2;
	GDC  gdc;
	ASCP ascp;
	EDA  eda;
	RJEC rjec;
	ECA  eca;
	TVSA tvsa;
	ORDEAL ordeal;
	EMS ems;
	RHC rhc1;
	RHC rhc2;

	// Telecom equipment
	DSE  dataRecorder;
	PCM  pcm;
	PMP	 pmp;
	USB  usb;
	HGA  hga;
	OMNI omnia;
	OMNI omnib;
	OMNI omnic;
	OMNI omnid;
	VHFAntenna vhfa;
	VHFAntenna vhfb;
	VHFRangingSystem vhfranging;
	VHFAMTransceiver vhftransceiver;
	RNDZXPDRSystem RRTsystem;

	//Instrumentation
	SCE sce;
	PowerMerge ECSPressGroups1Feeder;
	PowerMerge ECSPressGroups2Feeder;
	PowerMerge ECSTempTransducerFeeder;
	PowerMerge ECSWastePotTransducerFeeder;
	PowerMerge InstrumentationPowerFeeder;
	PowerMerge ECSSecTransducersFeeder;
public:
	CSMTankTempTransducer H2Tank1TempSensor;
	CSMTankTempTransducer H2Tank2TempSensor;
	CSMTankTempTransducer O2Tank1TempSensor;
	CSMTankTempTransducer O2Tank2TempSensor;
	CSMTankPressTransducer CabinPressSensor;
	CSMTankTempTransducer CabinTempSensor;
	CSMDeltaPressINH2OTransducer SuitCabinDeltaPressSensor;
	CSMCO2PressTransducer CO2PartPressSensor;
	CSMTankPressTransducer O2SurgeTankPressSensor;
	CSMTankTempTransducer SuitTempSensor;
	CSMTankQuantityTransducer WasteH2OQtySensor;
	CSMTankQuantityTransducer PotH2OQtySensor;
	CSMTankPressTransducer SuitPressSensor;
	CSMDeltaPressPSITransducer SuitCompressorDeltaPSensor;
	CSMTankPressTransducer GlycolPumpOutPressSensor;
	CSMTankTempTransducer GlyEvapOutSteamTempSensor;
	CSMTankTempTransducer GlyEvapOutTempSensor;
	CSMTankQuantityTransducer GlycolAccumQtySensor;
	CSMTankTempTransducer ECSRadOutTempSensor;
	CSMEvaporatorPressTransducer GlyEvapBackPressSensor;
	CSMPipeFlowTransducer ECSO2FlowO2SupplyManifoldSensor;
	CSMTankPressTransducer O2SupplyManifPressSensor;
	CSMTankPressTransducer SecGlyPumpOutPressSensor;
	CSMTankTempTransducer SecEvapOutLiqTempSensor;
	CSMTankQuantityTransducer SecGlycolAccumQtySensor;
	CSMEvaporatorPressTransducer SecEvapOutSteamPressSensor;
	//CSMTankPressTransducer H2OGlyResPressSensor;
	//Primary glycol flow rate needs a pipe implemented with name
	//CSMPipeFlowTransducer PriGlycolFlowRateSensor;
	CSMTankTempTransducer PriEvapInletTempSensor;
	CSMTankTempTransducer PriRadInTempSensor;
	CSMTankTempTransducer SecRadInTempSensor;
	CSMTankTempTransducer SecRadOutTempSensor;
protected:

	// CM Optics
	CMOptics optics;

	// Fuel cells	
	FCell *FuelCells[3];
	Boiler *FuelCellHeaters[3];
	Cooling *FuelCellCooling[3];
	h_Tank *FuelCellO2Manifold[3];
	h_Tank *FuelCellH2Manifold[3];

	// O2 tanks.
	h_Tank *O2Tanks[2];
	Boiler *O2TanksHeaters[2];
	Boiler *O2TanksFans[2];

	// H2 tanks
	h_Tank *H2Tanks[2];
	Boiler *H2TanksHeaters[2];
	Boiler *H2TanksFans[2];

	//Tunnel Pipe
	h_Pipe *CMTunnel;

	// Main bus A and B.
	DCbus *MainBusA;
	DCbus *MainBusB;

	// Non-essential buses... oddly, these are for non-essential systems.
	DCbus NonEssBus1;
	DCbus NonEssBus2;

	DCBusController MainBusAController;
	DCBusController MainBusBController;

	// Flight Bus.
	// This gets fed via diodes from MNA and MNB through 20-amp circuit breakers on panel 225.
	DCbus FlightBus;
	PowerMerge FlightBusFeeder;

	// Flight/Post Landing Bus.
	// This gets fed via various circuit breakers on panel 275.
	DCbus FlightPostLandingBus;
	NWayPowerMerge FlightPostLandingBusFeeder;

	// Fake feeder for twin LM power umbilicals
	PowerMerge LMUmbilicalFeeder;

	//SCS Logic Buses
	DCbus SCSLogicBus1;
	DCbus SCSLogicBus2;
	DCbus SCSLogicBus3;
	DCbus SCSLogicBus4;
	PowerMerge SCSLogicBus1Feeder;
	PowerMerge SCSLogicBus2Feeder;
	PowerMerge SCSLogicBus3Feeder;
	PowerMerge SCSLogicBus4Feeder;

	// Inverters.
	ACInverter *Inverter1;
	ACInverter *Inverter2;
	ACInverter *Inverter3;

	// AC bus 1 and 2, which are three-phase.
	ACbus ACBus1PhaseA;
	ACbus ACBus1PhaseB;
	ACbus ACBus1PhaseC;

	ACbus ACBus2PhaseA;
	ACbus ACBus2PhaseB;
	ACbus ACBus2PhaseC;

	ThreeWayPowerMerge ACBus1;
	ThreeWayPowerMerge ACBus2;

	Battery *EntryBatteryA;
	Battery *EntryBatteryB;
	Battery *EntryBatteryC;

	Battery *PyroBatteryA;
	Battery *PyroBatteryB;

	PowerMerge BatteryBusA;
	PowerMerge BatteryBusB;

	PowerMerge BatteryRelayBus;

	BatteryCharger BatteryCharger;

	PowerMerge PyroBusAFeeder;
	PowerMerge PyroBusBFeeder;
	DCbus PyroBusA;
	DCbus PyroBusB;

	DCbus SECSLogicBusA;
	DCbus SECSLogicBusB;

	PowerMerge SwitchPower;
	PowerMerge GaugePower;

	// ECS
	h_HeatExchanger *PrimCabinHeatExchanger;
	h_HeatExchanger *PrimSuitHeatExchanger;
	h_HeatExchanger *PrimSuitCircuitHeatExchanger;
	h_HeatExchanger *SecCabinHeatExchanger;
	h_HeatExchanger *SecSuitHeatExchanger;
	h_HeatExchanger *SecSuitCircuitHeatExchanger;
	h_HeatExchanger *PrimEcsRadiatorExchanger1;
	h_HeatExchanger *PrimEcsRadiatorExchanger2;
	h_HeatExchanger *SecEcsRadiatorExchanger1;
	h_HeatExchanger *SecEcsRadiatorExchanger2;
	Boiler *CabinHeater;
	Boiler *PrimECSTestHeater;
	Boiler *SecECSTestHeater;
	AtmRegen *SuitCompressor1;
	AtmRegen *SuitCompressor2;
	h_crew *Crew;
	CabinPressureRegulator CabinPressureRegulator;
	O2DemandRegulator O2DemandRegulator;
	CabinPressureReliefValve CabinPressureReliefValve1;
	CabinPressureReliefValve CabinPressureReliefValve2;
	SuitCircuitReturnValve SuitCircuitReturnValve;
	O2SMSupply O2SMSupply;
	CrewStatus CrewStatus;
	SaturnSideHatch SideHatch;
	SaturnWaterController WaterController;
	SaturnGlycolCoolingController GlycolCoolingController;
	SaturnLMTunnelVent LMTunnelVent;
	SaturnForwardHatch ForwardHatch;
	SaturnPressureEqualizationValve PressureEqualizationValve;

	// RHC/THC 
	PowerMerge RHCNormalPower;
	PowerMerge RHCDirect1Power;
	PowerMerge RHCDirect2Power;

	// CSM has two DSKYs: one is in the main panel, the other is below. For true realism we should support
	// both.
	DSKY dsky;
	DSKY dsky2;
	CSMcomputer agc;	
	IMU imu;
	CDU tcdu;
	CDU scdu;
	IU* iu;
	SIVBSystems *sivb;
	CSMCautionWarningSystem cws;

	DockingProbe dockingprobe;
	SECS secs;
	ELS els;

	QBall qball;
	LETCanard canard;

	Pyro CMSMPyros;
	Pyro CMDockingRingPyros;
	Pyro CSMLVPyros;
	Pyro ApexCoverPyros;
	Pyro DrogueChutesDeployPyros;
	Pyro MainChutesDeployPyros;
	Pyro MainChutesReleasePyros;
	PowerMerge CMSMPyrosFeeder;
	PowerMerge CMDockingRingPyrosFeeder;
	PowerMerge CSMLVPyrosFeeder;
	PowerMerge ApexCoverPyrosFeeder;
	PowerMerge DrogueChutesDeployPyrosFeeder;
	PowerMerge MainChutesDeployPyrosFeeder;
	PowerMerge MainChutesReleasePyrosFeeder;


	//
	// LM PAD
	//

	int LMPadCount;
	unsigned int *LMPad;
	int AEAPadCount;
	unsigned int *AEAPad;

	//
	// Do we have a crew, or is this an unmanned flight?
	//

	bool Crewed;

	//
	// Unmanned flight parameters.
	//

	bool AutoSlow;

	int SIVBPayload;

	//
	// VAGC Mode settings
	//

	bool VAGCChecklistAutoSlow;
	bool VAGCChecklistAutoEnabled;

	//
	// Misc. settings
	//

	bool TLICapableBooster;
	bool IUSCContPermanentEnabled;
	bool TLISoundsLoaded;
	bool SkylabSM;
	bool CMdocktgt;
	bool SkylabCM;
	bool S1bPanel;
	bool bRecovery;
	bool DontDeleteIU;

	#define SATVIEW_LEFTSEAT		0
	#define SATVIEW_RIGHTSEAT		1
	#define SATVIEW_CENTERSEAT		2
	#define SATVIEW_LEFTDOCK		3
	#define SATVIEW_RIGHTDOCK		4
	#define SATVIEW_GNPANEL			5
	#define SATVIEW_ENG1			10
	#define SATVIEW_ENG2			11
	#define SATVIEW_ENG3			12
	#define SATVIEW_ENG4			13
	#define SATVIEW_ENG5			14
	#define SATVIEW_ENG6			15

	unsigned int	viewpos;

	// Mesh indexes
	int probeidx;
	int probeextidx;
	int crewidx;
	int cmpidx;
	int sidehatchidx;
	int sidehatchopenidx;
	int sidehatchburnedidx;
	int sidehatchburnedopenidx;
	int fwdhatchidx;
	int opticscoveridx;
	int cmdocktgtidx;
	int nosecapidx;
	int meshLM_1;
	int simbaypanelidx;

	DEVMESHHANDLE probe;

	bool ASTPMission;

	double DockAngle;

	double 	Offset1st;

	bool StopRot;
	bool PayloadDataTransfer;

	mission::Mission* pMission;

	//
	// Panels
	//

#define SATPANEL_MAIN				0 // Both have Orbiter's 
#define SATPANEL_MAIN_LEFT		    0 // default panel id 0
#define SATPANEL_GN					1
#define SATPANEL_GN_CENTER			1 // Should never have both full Lower and Center Lower available.
#define SATPANEL_LEFT				2
#define SATPANEL_RIGHT				3
#define SATPANEL_LEFT_RNDZ_WINDOW	4
#define SATPANEL_RIGHT_RNDZ_WINDOW	5
#define SATPANEL_HATCH_WINDOW		6
#define SATPANEL_CABIN_PRESS_PANEL	7
#define SATPANEL_TELESCOPE			8
#define SATPANEL_SEXTANT			9
#define SATPANEL_MAIN_MIDDLE		10
#define SATPANEL_MAIN_RIGHT			11
#define SATPANEL_GN_LEFT			12
#define SATPANEL_GN_RIGHT			13
#define SATPANEL_LOWER_LEFT			14
#define SATPANEL_LOWER_MAIN			15
#define SATPANEL_RIGHT_CB			16
#define SATPANEL_LEFT_317_WINDOW    17
#define SATPANEL_TUNNEL             18

	int  PanelId;
	int MainPanelSplit;
	int GNSplit;
	bool InVC;
	bool InPanel;
	bool CheckPanelIdInTimestep;
	bool RefreshPanelIdInTimestep;
	bool FovFixed;
	bool FovExternal;
	double FovSave;
	int maxTimeAcceleration;
	bool IsMultiThread;

	//
	// Virtual cockpit
	//

	bool VCRegistered;
	VECTOR3 VCCameraOffset;
	VECTOR3 VCMeshOffset;

	bool KranzPlayed;
	bool PostSplashdownPlayed;
	bool SplashdownPlayed;

	bool FireLEM;
	bool FireTJM;
	bool FirePCM;

	OBJHANDLE hEVA;

	SoundLib soundlib;

	TimedSoundManager timedSounds;

	//
	// Surfaces.
	//

	SURFHANDLE srf[nsurf];  // handles for panel bitmaps.
	SURFHANDLE SMExhaustTex;
	SURFHANDLE CMTex;
	SURFHANDLE J2Tex;
	SURFHANDLE SIVBRCSTex;

	/// \todo SURFHANDLEs for VC

	//
	// Vessel handles.
	//

	OBJHANDLE hLMV;
	OBJHANDLE hstg1;
	OBJHANDLE hstg2;
	OBJHANDLE hintstg;
	OBJHANDLE hesc1;
	OBJHANDLE hPROBE;
	OBJHANDLE hs4bM;
	OBJHANDLE hs4b1;
	OBJHANDLE hs4b2;
	OBJHANDLE hs4b3;
	OBJHANDLE hs4b4;
	OBJHANDLE habort;
	OBJHANDLE hMaster;
	OBJHANDLE hSMJet;
	OBJHANDLE hVAB;
	OBJHANDLE hCrawler;
	OBJHANDLE hML;
	OBJHANDLE hMSS;
	OBJHANDLE hApex;
	OBJHANDLE hDrogueChute;
	OBJHANDLE hMainChute;
	OBJHANDLE hOpticsCover;
	OBJHANDLE hNosecapVessel;
	OBJHANDLE hLC34;
	OBJHANDLE hLC37;
	OBJHANDLE hLCC;

	//
	// ISP and thrust values, which vary depending on vehicle number.
	//

	double ISP_FIRST_SL;
	double ISP_FIRST_VAC;
	double ISP_SECOND_SL;//300*G;
	double ISP_SECOND_VAC;//421*G;
	double ISP_THIRD_VAC;//421*G;
	double ISP_LEM_VAC;
	double ISP_LEM_SL;
	double ISP_TJM_VAC;
	double ISP_TJM_SL;
	double ISP_PCM_VAC;
	double ISP_PCM_SL;

	double THRUST_FIRST_VAC;
	double THRUST_SECOND_VAC;//115200*G;
	double THRUST_THIRD_VAC;
	double THRUST_VAC_LEM;
	double THRUST_VAC_TJM;
	double THRUST_VAC_PCM;

	//
	// Generic functions shared between SaturnV and Saturn1B
	//

	void AddRCSJets(double TRANZ,double MaxThrust);
	void SetRecovery();
	void InitPanel(int panel);
	void SetSwitches(int panel);
	void AddLeftMainPanelAreas();
	void AddLeftMiddleMainPanelAreas(int offset);
	void AddRightMiddleMainPanelAreas(int offset);
	void AddRightMainPanelAreas(int offset);
	void AddLeftLowerPanelAreas();
	void AddLeftCenterLowerPanelAreas(int offset);
	void AddCenterLowerPanelAreas(int offset);
	void AddRightCenterLowerPanelAreas(int offset);
	void AddRightLowerPanelAreas(int offset);
	void ReleaseSurfaces();
	void KillDist(OBJHANDLE &hvessel, double kill_dist = 5000.0);
	void KillAlt(OBJHANDLE &hvessel,double altVS);
	void RedrawPanel_Alt (SURFHANDLE surf);
	void RedrawPanel_Alt2 (SURFHANDLE surf);
	void RedrawPanel_MFDButton (SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset, int ydist);
	void CryoTankHeaterSwitchToggled(ToggleSwitch *s, int *pump);
	void FuelCellHeaterSwitchToggled(ToggleSwitch *s, int *pump);
	void FuelCellReactantsSwitchToggled(ToggleSwitch *s, CircuitBrakerSwitch *cb, CircuitBrakerSwitch *cbLatch, int *h2open, int *o2open);
	void MousePanel_MFDButton(int mfd, int event, int mx, int my);
	void initSaturn();
	void SwitchClick();
	void ProbeSound();
	void CabinFanSound();
	void StopCabinFanSound();
	void CabinFansSystemTimestep();
	void ButtonClick();
	void GuardClick();
	void SetView();
	void SetView(double offset);
	void SetView(bool update_direction);
	void SetView(double offset, bool update_direction);
	int GetRenderViewportIsWideScreen();
	void MasterAlarm();
	void StopMasterAlarm();
	void GenericTimestep(double simt, double simdt, double mjd);
	void SystemsInit();
	void SystemsTimestep(double simt, double simdt, double mjd);
	void SystemsInternalTimestep(double simdt);
	void JoystickTimestep();
	void LimitSetThrusterDir (THRUSTER_HANDLE th, const VECTOR3 &dir);
	void FireLaunchEscapeMotor();
	void FireTowerJettisonMotor();
	void FirePitchControlMotor();
	void MoveTHC(bool dir);

	void RenderS1bEngineLight(bool EngineOn, SURFHANDLE dest, SURFHANDLE src, int xoffs, int yoffs);

	void ClearPropellants();
	void ClearThrusters();
	virtual void ConfigureStageMeshes(int stage_state) = 0;
	virtual void ConfigureStageEngines(int stage_state) = 0;
	virtual void CreateStageOne() = 0;

	void StageSix(double simt);
	void JostleViewpoint(double amount);
	void UpdatePayloadMass();
	void GetPayloadName(char *s);
	void GetApolloName(char *s);
	void AddSM(double offet, bool showSPS);

	//
	// Systems functions.
	//

	bool CabinFansActive();
	bool CabinFan1Active();
	bool CabinFan2Active();
	void ActivateCSMRCS();
	void DeactivateCSMRCS();
	void ActivateCMRCS();
	void DeactivateCMRCS();
	void FuelCellCoolingBypass(int fuelcell, bool bypassed);
	bool FuelCellCoolingBypassed(int fuelcell);
	void SetPipeMaxFlow(char *pipe, double flow);

	//
	// Save/Load support functions.
	//

	int GetMainState();
	void SetMainState(int s);
	int GetAttachState();
	void SetAttachState(int s);
	int GetSLAState();
	void SetSLAState(int s);

	///
	/// Get the Apollo 13 state flags as an int.
	/// \return 32-bit int representing the state flags.
	///
	int GetA13State();

	///
	/// Set the Apollo 13 state flags based on an int value.
	/// \param s This 32-bit int has the packed Apollo 13 flags.
	///
	void SetA13State(int s);
	int GetLightState();
	void SetLightState(int s);
	void GenericLoadStateSetup();

	virtual void SetVehicleStats() = 0;
	virtual void CalculateStageMass () = 0;
	virtual void SaveVehicleStats(FILEHANDLE scn) = 0;
	virtual void LoadIU(FILEHANDLE scn) = 0;
	void SaveIU(FILEHANDLE scn);
	void SaveLVDC(FILEHANDLE scn);
	virtual void LoadLVDC(FILEHANDLE scn) = 0;
	virtual void LoadSIVB(FILEHANDLE scn) = 0;
	virtual void SaveSI(FILEHANDLE scn) = 0;
	virtual void LoadSI(FILEHANDLE scn) = 0;
	virtual void SaveSII(FILEHANDLE scn) {};
	virtual void LoadSII(FILEHANDLE scn) {};
	virtual void SetEngineFailure(int failstage, int faileng, double failtime, bool fail) = 0;
	virtual void GetEngineFailure(int failstage, int faileng, bool &fail, double &failtime) = 0;

	void GetScenarioState (FILEHANDLE scn, void *status);
	bool ProcessConfigFileLine (FILEHANDLE scn, char *line);

	void ClearPanelSDKPointers();

	//
	// Mission stage functions.
	//

	void StageSeven(double simt);
	void StageEight(double simt);
	void SetChuteStage1();
	void SetChuteStage2();
	void SetChuteStage3();
	void SetChuteStage4();
	void SetSplashStage();
	void SetCSMStage();
	void CreateSIVBStage(char *config, VESSELSTATUS &vs1, bool SaturnVStage);
	void SetReentryStage();
	void SetReentryMeshes();
	void AddRCS_CM(double MaxThrust, double offset = 0.0, bool createThrusterGroups = true);
	void GenericTimestepStage(double simt, double simdt);
	void SetGenericStageState();
	void DestroyStages(double simt);
	void LookForSIVb();
	void LookForLEM();
	void FireSeperationThrusters(THRUSTER_HANDLE *pth);
	void LoadDefaultSounds();
	void RCSSoundTimestep();

	//
	// Sounds
	//

	Sound Sclick;
	Sound Bclick;
	Sound Gclick;
	Sound ThumbClick;
	Sound SMasterAlarm;
	Sound LaunchS;
	Sound Scount;
	Sound SplashS;
	Sound Swater;
	Sound PostSplashdownS;
	Sound TowerJS;
	Sound StageS;
	Sound Sctdw;
	Sound SeparationS;
	Sound SMJetS;
	Sound S4CutS;
	Sound SShutS;
	Sound S2ShutS;
	Sound SepS;
	Sound CrashBumpS;
	Sound Psound;
	FadeInOutSound CabinFans;
	FadeInOutSound SuitCompressorSound;
	Sound SwindowS;
	Sound SKranz;
	Sound SExploded;
	Sound SApollo13;
	Sound SSMSepExploded;
	Sound SPUShiftS;
	Sound SDockingCapture;
	Sound SDockingLatch;
	Sound SDockingExtend;
	Sound SUndock;
	Sound CabincloseoutS;
	Sound STLI;
	Sound STLIStart;
	Sound SecoSound;
	Sound PostLandingVentSound;
	Sound CrewDeadSound;
	Sound RCSFireSound;
	Sound RCSSustainSound;
	Sound HatchOpenSound;
	Sound HatchCloseSound;

	///
	/// Drogue deployment message.
	///
	/// \brief 'Drogues' sound.
	///
	Sound DrogueS;

	//
	// General engine resources.
	//

	//
	// Propellant resources.
	//

	//
	// Main fuel for stage 1, 2, and 3.
	//

	PROPELLANT_HANDLE ph_1st, ph_2nd, ph_3rd;

	//
	// SM RCS
	//

	PROPELLANT_HANDLE ph_rcs0, ph_rcs1, ph_rcs2, ph_rcs3;

	//
	// CM RCS
	//

	PROPELLANT_HANDLE ph_rcs_cm_1, ph_rcs_cm_2;

	//
	// Fake propellant for fake thrusters spewing out junk at stage
	// seperation.
	//

	PROPELLANT_HANDLE ph_sep, ph_sep2;

	//
	// SPS and LET.
	//

	PROPELLANT_HANDLE ph_sps, ph_lem, ph_pcm;//, ph_tjm;

	//
	// Ullage rockets for stage 1, 2 and 3.
	//

	PROPELLANT_HANDLE ph_ullage1, ph_ullage2, ph_ullage3;

	///
	/// Fake propellant for O2 vents.
	///
	PROPELLANT_HANDLE ph_o2_vent;

	//S-IVB APS modules
	PROPELLANT_HANDLE ph_aps1, ph_aps2;

	//
	// Thruster group handles. We have a lot of these :).
	//

	THGROUP_HANDLE thg_1st, thg_2nd, thg_3rd, thg_sps;
	THGROUP_HANDLE thg_ull, thg_ver, thg_lem;//, thg_tjm;
	THGROUP_HANDLE thg_retro1, thg_retro2;

	THRUSTER_HANDLE th_1st[8], th_2nd[5], th_3rd[1], th_sps[1];
	THRUSTER_HANDLE th_3rd_lox, th_3rd_lh2;
	THRUSTER_HANDLE th_ull[8], th_ver[3];                       // handles for orbiter main engines
	THRUSTER_HANDLE th_lem[4], th_tjm[2], th_pcm;
	THRUSTER_HANDLE th_att_rot[24], th_att_lin[24];
	THRUSTER_HANDLE	th_aps_rot[6];
	THRUSTER_HANDLE	th_aps_ull[2];
	THRUSTER_HANDLE	th_sep[8], th_sep2[8];
	THRUSTER_HANDLE th_rcs_a[8], th_rcs_b[8], th_rcs_c[8], th_rcs_d[8];		// SM RCS quads. Entry zero is not used, to match Apollo numbering
	THRUSTER_HANDLE th_att_cm[12], th_att_cm_sys1[6], th_att_cm_sys2[6];    // CM RCS  
	THRUSTER_HANDLE th_o2_vent;
	bool th_att_cm_commanded[12];

	PSTREAM_HANDLE dyemarker;
	PSTREAM_HANDLE wastewaterdump;
	PSTREAM_HANDLE urinedump;
	PSTREAM_HANDLE cmrcsdump[12];
	PSTREAM_HANDLE prelaunchvent[3];
	PSTREAM_HANDLE stagingvent[8];
	PSTREAM_HANDLE contrail[8];
	double contrailLevel;

	CMRCSPropellantSource CMRCS1;
	CMRCSPropellantSource CMRCS2;

	SMRCSPropellantSource SMQuadARCS;
	SMRCSPropellantSource SMQuadBRCS;
	SMRCSPropellantSource SMQuadCRCS;
	SMRCSPropellantSource SMQuadDRCS;

	SPSEngine SPSEngine;
	SPSPropellantSource SPSPropellant;
	Boiler *SPSPropellantLineHeaterA;
	Boiler *SPSPropellantLineHeaterB;

	//
	// LEM data.
	//

	double LMDescentFuelMassKg;	///< Mass of fuel in descent stage of LEM.
	double LMAscentFuelMassKg;	///< Mass of fuel in ascent stage of LEM.
	double LMDescentEmptyMassKg;
	double LMAscentEmptyMassKg;

	//
	// Random motion.
	//

	double ViewOffsetx;
	double ViewOffsety;
	double ViewOffsetz;

	//
	// Save the last view offset.
	//

	double CurrentViewOffset;

	///
	/// Time of last timestep call.
	///
	double LastTimestep;

	//
	// Panel flash.
	//

	double NextFlashUpdate;
	bool PanelFlashOn;

	///
	/// Audio language.
	///
	char AudioLanguage[64];

	///
	/// SIVB payload name
	///
	char PayloadName[64];

	///
	/// LEM checklist file
	///
	char LEMCheck[100];

	//
	// Connectors.
	//

	///
	/// \brief MFD to panel connector.
	///
	PanelConnector MFDToPanelConnector;

	///
	/// \brief Connector from CSM to IU.
	///
	CSMToIUConnector iuCommandConnector;
	SaturnToIUCommandConnector sivbCommandConnector;

	MultiConnector CSMToLEMConnector;

	PowerDrainConnectorObject CSMToLEMPowerDrain;
	PowerDrainConnector CSMToLEMPowerConnector;
	CSMToLEMECSConnector lemECSConnector;
	CSMToPayloadConnector payloadCommandConnector;

	// Checklist Controller to CSM connector
	ChecklistDataInterface cdi;

	CSM_RRTto_LM_RRConnector CSM_RRTto_LM_RRConnector;

	//
	// PanelSDK pointers.
	//

	double *pCO2Level;
	double *pCabinCO2Level;
	double *pCabinPressure;
	double *pCabinTemp;
	double *pSuitTemp;
	double *pSuitPressure;
	double *pSuitReturnPressure;
	double *pO2RepressPressure;
	double *pCabinRegulatorFlow;
	double *pO2DemandFlow;
	double *pDirectO2Flow;
	double *pSuitTestFlow;
	double *pCabinRepressFlow;
	double *pEmergencyCabinRegulatorFlow;
	double *pO2FlowXducer;
	double *pO2Tank1Press;
	double *pO2Tank2Press;
	double *pH2Tank1Press;
	double *pH2Tank2Press;
	double *pO2SurgeTankPress;
	double *pO2Tank1Quantity;
	double *pO2Tank2Quantity;
	double *pH2Tank1Quantity;
	double *pH2Tank2Quantity;
	double *pFCH2Flow[4];
	double *pFCO2Flow[4];
	double *pFCTemp[4];
	double *pFCCondenserTemp[4];
	double *pFCCoolingTemp[4];
	double *pPrimECSRadiatorInletPressure;
	double *pPrimECSRadiatorInletTemp;
	double *pPrimECSRadiatorOutletTemp;
	double *pPrimECSEvaporatorOutletTemp;
	double *pPrimECSEvaporatorSteamPressure;
	double *pPrimECSAccumulatorQuantity;
	double *pSecECSRadiatorInletPressure;
	double *pSecECSRadiatorInletTemp;
	double *pSecECSRadiatorOutletTemp;
	double *pSecECSEvaporatorOutletTemp;
	double *pSecECSEvaporatorSteamPressure;
	double *pSecECSAccumulatorQuantity;
	double *pPotableH2oTankQuantity;
	double *pWasteH2oTankQuantity;
	double *pCSMTunnelPressure;

	// InitSaturn is called twice, but some things must run only once
	bool InitSaturnCalled;

	int LMPadLoadCount;
	int LMPadValueCount;
	int AEAPadLoadCount;
	int AEAPadValueCount;

#define SISYSTEMS_START_STRING		"SISYSTEMS_BEGIN"
#define SISYSTEMS_END_STRING		"SISYSTEMS_END"

	//
	// IMFD5 communication support
	//
	IMFD_Client IMFD_Client; 

	//
	// Friend Class List for systems objects 
	//

	friend class LVDC1B;	// If we are a Saturn 1B
	friend class LVDC;		// If we are a Saturn V
	friend class MCC;
	friend class GDC;
	friend class BMAG;
	friend class ASCP;
	friend class EDA;
	friend class RJEC;
	friend class ECA;
	friend class TVSA;
	friend class ServoAmplifierModule;
	friend class CSMcomputer; // I want this to be able to see the GDC	
	friend class LEMcomputer; 
	friend class PCM;         // Otherwise reading telemetry is a pain
	friend class PMP;
	friend class USB;
	friend class HGA;
	friend class DSE;
	friend class EMS;
	friend class SPSPropellantSource;
	friend class SPSEngine;
	friend class SPSGimbalActuator;
	friend class CMOptics;
	friend class CSMCautionWarningSystem;
	friend class CMACInverterSwitch;
	friend class SaturnSCControlSetter;
	friend class SaturnEMSDvDisplay;
	friend class SaturnEMSScrollDisplay;
	friend class SaturnEventTimer;
	friend class SECS;
	friend class MESC;
	friend class RCSC;
	friend class ELS;
	friend class ELSC;
	friend class PCVB;
	friend class LDEC;
	friend class CrewStatus;
	friend class OpticsHandcontrollerSwitch;
	friend class MinImpulseHandcontrollerSwitch;
	friend class CMRCSPropellantSource;
	friend class DockingProbe;
	friend class SaturnWaterController;
	friend class SaturnGlycolCoolingController;
	friend class CSMLMPowerSwitch;
	friend class SaturnPanel382Cover;
	friend class SaturnPanel600;
	friend class SaturnASCPSwitch;
	friend class SaturnPanelOrdeal;
	friend class SaturnHighGainAntennaPitchMeter;
	friend class SaturnHighGainAntennaYawMeter;
	friend class SaturnHighGainAntennaStrengthMeter;
	friend class SaturnSystemTestAttenuator;
	friend class SaturnLVSPSPcMeter;
	friend class SaturnSPSHeliumNitrogenPressMeter;
	friend class SaturnLMDPGauge;
	friend class VHFRangingSystem;
	friend class RNDZXPDRSystem;
	friend class DockingTargetSwitch;
	friend class SCE;
	// Friend class the MFD too so it can steal our data
	friend class ProjectApolloMFD;
	friend class ARCore;
	friend class AR_GCore;
	friend class ApolloRTCCMFD;
	friend class RTCC;
};

extern void BaseInit();
extern void SaturnInitMeshes();
extern void StageTransform(VESSEL *vessel, VESSELSTATUS *vs, VECTOR3 ofs, VECTOR3 vel);

const double STG2O = 8;
const double SMVO = 0.0;
const double CREWO = 0.0;

extern MESHHANDLE hSM;
extern MESHHANDLE hCM;
extern MESHHANDLE hCMnh;
extern MESHHANDLE hCMInt;
extern MESHHANDLE hCMVC;
extern MESHHANDLE hFHC;
extern MESHHANDLE hFHO;
extern MESHHANDLE hFHF;
extern MESHHANDLE hCMP;
extern MESHHANDLE hCREW;
extern MESHHANDLE hSMhga;
extern MESHHANDLE hprobe;
extern MESHHANDLE hprobeext;
extern MESHHANDLE hsat5tower;
extern MESHHANDLE hFHO2;
extern MESHHANDLE hopticscover;

extern void SetupgParam(HINSTANCE hModule);
extern void DeletegParam();

#endif // _PA_SATURN_H
