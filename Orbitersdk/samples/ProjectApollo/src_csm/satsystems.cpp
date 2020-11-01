/****************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant

  ORBITER vessel module: generic Saturn base class
  Code for simulation of Saturn hardware systems.

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
#pragma include_alias( <fstream.h>, <fstream> )
#include "Orbitersdk.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"
#include "resource.h"
#include "nasspdefs.h"
#include "nasspsound.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "csmcomputer.h"
#include "iu.h"
#include "saturn.h"
#include "ioChannels.h"
#include "tracer.h"
#include "Mission.h"

//FILE *PanelsdkLogFile;


void Saturn::SystemsInit() {

	// default state
	systemsState = SATSYSTEMS_NONE;
	lastSystemsMissionTime = MINUS_INFINITY;
	firstSystemsTimeStepDone = false;

	// initialize SPSDK
	Panelsdk.RegisterVessel(this);
	Panelsdk.InitFromFile("ProjectApollo\\SaturnSystems");

	//PanelsdkLogFile = fopen("ProjectApollo Saturn Systems.log", "w");

	//
	// Electrical systems.
	// First wire up buses to the Panel SDK.
	//

	e_object *eo;

	//
	// Inverters
	//
	Inverter1 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_1");
	Inverter2 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_2");
	Inverter3 = (ACInverter *) Panelsdk.GetPointerByString("ELECTRIC:INV_3");

	Inverter1->WireTo(&InverterControl1CircuitBraker);
	Inverter2->WireTo(&InverterControl2CircuitBraker);
	Inverter3->WireTo(&InverterControl3CircuitBraker);	

	//
	// AC Bus 1
	//

	ACBus1.WireToBuses(&ACBus1PhaseA, &ACBus1PhaseB, &ACBus1PhaseC);

	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:AC_1");
	eo->WireTo(&ACBus1);

	//
	// AC Bus 2
	//

	ACBus2.WireToBuses(&ACBus2PhaseA, &ACBus2PhaseB, &ACBus2PhaseC);
	
	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:AC_2");
	eo->WireTo(&ACBus2);

	//
	// For now we register these systems so that the panel SDK will update
	// them each timestep. Otherwise we get infinitely increasing current
	// if we're not careful.
	//

	Panelsdk.AddElectrical(&ACBus1PhaseA, false);
	Panelsdk.AddElectrical(&ACBus1PhaseB, false);
	Panelsdk.AddElectrical(&ACBus1PhaseC, false);
	Panelsdk.AddElectrical(&ACBus2PhaseA, false);
	Panelsdk.AddElectrical(&ACBus2PhaseB, false);
	Panelsdk.AddElectrical(&ACBus2PhaseC, false);

	//
	// Fuel cells.
	//

	FuelCells[0] = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1");
	FuelCells[1] = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2");
	FuelCells[2] = (FCell *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3");

	FuelCellCooling[0] = (Cooling *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING");
	FuelCellCooling[0]->WireTo(&FuelCell1PumpsACCB);
	FuelCellCooling[1] = (Cooling *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2COOLING");
	FuelCellCooling[1]->WireTo(&FuelCell2PumpsACCB);
	FuelCellCooling[2] = (Cooling *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3COOLING");
	FuelCellCooling[2]->WireTo(&FuelCell3PumpsACCB);

	FuelCellHeaters[0] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1HEATER");
	FuelCellHeaters[1] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2HEATER");
	FuelCellHeaters[2] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3HEATER");

	FuelCellO2Manifold[0] = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD");
	FuelCellO2Manifold[1] = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2MANIFOLD");
	FuelCellO2Manifold[2] = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD");

	FuelCellH2Manifold[0] = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1MANIFOLD");
	FuelCellH2Manifold[1] = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2MANIFOLD");
	FuelCellH2Manifold[2] = (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3MANIFOLD");

	//
	// O2 tanks.
	//

	O2Tanks[0] = (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1");
	O2Tanks[1] = (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2");

	O2TanksHeaters[0] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1HEATER");
	O2TanksHeaters[1] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2HEATER");
	O2TanksFans[0] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:O2TANK1FAN");
	O2TanksFans[1] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:O2TANK2FAN");

	//
	// H2 tanks.
	//

	H2Tanks[0] = (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1");
	H2Tanks[1] = (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2");

	H2TanksHeaters[0] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1HEATER");
	H2TanksHeaters[1] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2HEATER");
	H2TanksFans[0] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:H2TANK1FAN");
	H2TanksFans[1] = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:H2TANK2FAN");

	//
	// Entry and landing batteries.
	//

	EntryBatteryA = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_A");
	EntryBatteryB = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_B");
	EntryBatteryC = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_C");

	//
	// Wire battery buses to batteries.
	//

	BatteryBusA.WireToBuses(&BatAPWRCircuitBraker, &BatCtoBatBusACircuitBraker);
	BatteryBusB.WireToBuses(&BatBPWRCircuitBraker, &BatCtoBatBusBCircuitBraker);

	//
	// Pyro devices.
	//

	PyroBatteryA = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_PYRO_A");
	PyroBatteryB = (Battery *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_PYRO_B");

	// Pyro buses and its feeders
	PyroBusAFeeder.WireToBuses(&PyroASeqACircuitBraker, &BatBusAToPyroBusTieCircuitBraker);
	PyroBusBFeeder.WireToBuses(&PyroBSeqBCircuitBraker, &BatBusBToPyroBusTieCircuitBraker);	
	Panelsdk.AddElectrical(&PyroBusA, false);
	Panelsdk.AddElectrical(&PyroBusB, false);

	// Pyros
	CMSMPyros.WireTo(&CMSMPyrosFeeder);  
	CMDockingRingPyros.WireTo(&CMDockingRingPyrosFeeder);
	CSMLVPyros.WireTo(&CSMLVPyrosFeeder);
	ApexCoverPyros.WireTo(&ApexCoverPyrosFeeder);
	DrogueChutesDeployPyros.WireTo(&DrogueChutesDeployPyrosFeeder);
	MainChutesDeployPyros.WireTo(&MainChutesDeployPyrosFeeder);
	MainChutesReleasePyros.WireTo(&MainChutesReleasePyrosFeeder);

	//
	// SECS Logic buses
	//

	Panelsdk.AddElectrical(&SECSLogicBusA, false);
	Panelsdk.AddElectrical(&SECSLogicBusB, false);

	//
	// Battery relay bus
	//

	BatteryRelayBus.WireToBuses(&BATRLYBusBatACircuitBraker, &BATRLYBusBatBCircuitBraker);

	//
	// Main Buses
	//

	MainBusA = (DCbus *) Panelsdk.GetPointerByString("ELECTRIC:DC_A");
	MainBusB = (DCbus *) Panelsdk.GetPointerByString("ELECTRIC:DC_B");
	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:BATTERY_GSE");

	MainBusA->WireTo(MainBusAController.GetBusSource());
	MainBusB->WireTo(MainBusBController.GetBusSource());

	MainBusAController.Init(FuelCells[0], FuelCells[1], FuelCells[2],
		                    &MainABatBusACircuitBraker, &MainABatCCircuitBraker, eo,
							&FuelCell1BusContCB, &FuelCell2BusContCB, &FuelCell3BusContCB);	

	MainBusBController.Init(FuelCells[0], FuelCells[1], FuelCells[2],
		                    &MainBBatBusBCircuitBraker, &MainBBatCCircuitBraker, eo,
							&FuelCell1BusContCB, &FuelCell2BusContCB, &FuelCell3BusContCB);
	
	MainBusAController.ConnectFuelCell(2, true);	// Default state of MainBusASwitch2

	//
	// Flight Bus and its feeder
	//

	FlightBusFeeder.WireToBuses(&FLTBusMNACB, &FLTBusMNBCB);
	FlightBus.WireTo(&FlightBusFeeder);
	Panelsdk.AddElectrical(&FlightBus, false);


	// Feeder for LM umbilical
	LMUmbilicalFeeder.WireToBuses(&MnbLMPWR1CircuitBraker,&MnbLMPWR2CircuitBraker);

	//
	// Flight/Post Landing Bus and its feeder
	//

	FlightPostLandingBusFeeder.WireToBus(1, &FlightPostLandingMainACircuitBraker);
	FlightPostLandingBusFeeder.WireToBus(2, &FlightPostLandingMainBCircuitBraker);
	FlightPostLandingBusFeeder.WireToBus(3, &FlightPostLandingBatBusACircuitBraker);
	FlightPostLandingBusFeeder.WireToBus(4, &FlightPostLandingBatBusBCircuitBraker);
	FlightPostLandingBusFeeder.WireToBus(5, &FlightPostLandingBatCCircuitBraker);
	FlightPostLandingBus.WireTo(&FlightPostLandingBusFeeder);
	Panelsdk.AddElectrical(&FlightPostLandingBus, false);

	//
	// Battery Charger
	//

	BatteryCharger.Init(EntryBatteryA, EntryBatteryB, EntryBatteryC,
		                &BatteryChargerBatACircuitBraker, &BatteryChargerBatBCircuitBraker, &BatCCHRGCircuitBraker,
						&BatteryChargerMnACircuitBraker, &BatteryChargerMnBCircuitBraker, &BatteryChargerAcPwrCircuitBraker,
						&BatCPWRCircuitBraker);

	EntryBatteryA->WireTo(&BatteryChargerBatACircuitBraker);
	EntryBatteryB->WireTo(&BatteryChargerBatBCircuitBraker);

	//
	// SCS Logic Buses
	//

	SCSLogicBus1Feeder.WireToBuses(&LogicBus12MnACircuitBraker, &LogicBus14MnBCircuitBraker);
	SCSLogicBus2Feeder.WireToBuses(&LogicBus23MnBCircuitBraker, &LogicBus12MnACircuitBraker);
	SCSLogicBus3Feeder.WireToBuses(&LogicBus23MnBCircuitBraker, &LogicBus34MnACircuitBraker);
	SCSLogicBus4Feeder.WireToBuses(&LogicBus14MnBCircuitBraker, &LogicBus34MnACircuitBraker);
	SCSLogicBus1.WireTo(&SCSLogicBus1Feeder);
	SCSLogicBus4.WireTo(&SCSLogicBus4Feeder);
	Panelsdk.AddElectrical(&SCSLogicBus1, false);
	Panelsdk.AddElectrical(&SCSLogicBus2, false);
	Panelsdk.AddElectrical(&SCSLogicBus3, false);
	Panelsdk.AddElectrical(&SCSLogicBus4, false);

	LogicPowerSwitch.WireSourcesToBuses(1, &SCSLogicBus2Feeder, &SCSLogicBus2);
	LogicPowerSwitch.WireSourcesToBuses(2, &SCSLogicBus3Feeder, &SCSLogicBus3);

	//
	// Generic power source for switches, tied to both Bus A and
	// Bus B.
	//

	SwitchPower.WireToBuses(MainBusA, MainBusB);
	GaugePower.WireToBuses(MainBusA, MainBusB);

	//
	// ECS devices
	//

	PrimCabinHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMCABINHEATEXCHANGER");
	PrimSuitHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITHEATEXCHANGER");
	PrimSuitCircuitHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITCIRCUITHEATEXCHANGER");
	SecCabinHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:SECCABINHEATEXCHANGER");
	SecSuitHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:SECSUITHEATEXCHANGER");
	SecSuitCircuitHeatExchanger = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:SECSUITCIRCUITHEATEXCHANGER");

	PrimEcsRadiatorExchanger1 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER1");
	PrimEcsRadiatorExchanger2 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER2");
	SecEcsRadiatorExchanger1 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:SECECSRADIATOREXCHANGER1");
	SecEcsRadiatorExchanger2 = (h_HeatExchanger *) Panelsdk.GetPointerByString("HYDRAULIC:SECECSRADIATOREXCHANGER2");
	
	CabinHeater = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:CABINHEATER");
	
	PrimECSTestHeater = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:PRIMECSTESTHEATER");
	SecECSTestHeater = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:SECECSTESTHEATER");
	
	Crew = (h_crew *) Panelsdk.GetPointerByString("HYDRAULIC:CREW");	 

	SuitCompressor1 = (AtmRegen *) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER1");
	SuitCompressor1->WireTo(&SuitCompressor1Switch);
	SuitCompressor2 = (AtmRegen *) Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER2");
	SuitCompressor2->WireTo(&SuitCompressor2Switch);

	eo = (e_object *) Panelsdk.GetPointerByString("ELECTRIC:SECGLYCOLPUMP");
	eo->WireTo(&SecCoolantLoopPumpSwitch);

	CabinPressureRegulator.Init((h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR"), 
								(h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESSVALVE"), 
								(h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:EMERGENCYCABINPRESSUREREGULATOR"), 
								&CabinRepressValveRotary, &EmergencyCabinPressureRotary, &EmergencyCabinPressureTestSwitch);

	O2DemandRegulator.Init((h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR"), 
		                   (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:SUITRELIEFVALVE"), 
						   (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:SUITTESTVALVE"), 
						   &O2DemandRegulatorRotary, &SuitTestRotary);
	
	CabinPressureReliefValve1.Init((h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFVALVE1"), 
		                           (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFINLET1"), 
								   this, &CabinPressureReliefLever1, &PostLDGVentValveLever, &PostLandingVentSwitch, 
								   &FLTPLCircuitBraker, &SideHatch);
	
	CabinPressureReliefValve2.Init((h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFVALVE2"), 
		                           (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFINLET2"), 
								   this, &CabinPressureReliefLever2, &PostLDGVentValveLever, &PostLandingVentSwitch, 
								   &FLTPLCircuitBraker, &SideHatch);
	
	SuitCircuitReturnValve.Init((h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNINLET"), &SuitCircuitReturnValveLever);
	
	O2SMSupply.Init((h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY"), (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR"), 
		            (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK"),(h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGE"), 
					(h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGEOUTLET"), (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGEOUTLETPIPE3"),
					&OxygenSMSupplyRotary, &OxygenSurgeTankRotary, &OxygenRepressPackageRotary, &O2MainRegulatorASwitch, &O2MainRegulatorBSwitch,
					&HatchEmergencyO2ValveSwitch, &HatchRepressO2ValveSwitch);

	CMTunnel = (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CSMTUNNELUNDOCKED");

	SetPipeMaxFlow("HYDRAULIC:CSMTUNNELUNDOCKED", 1000.0 / LBH);

	SetPipeMaxFlow("HYDRAULIC:O2SMSUPPLYINLET1", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2SMSUPPLYINLET2", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2SURGETANKINLET1", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2SURGETANKINLET2", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2SURGETANKOUTLET", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2REPRESSPACKAGEINLET1", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2REPRESSPACKAGEINLET2", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2REPRESSPACKAGEINLET3", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2REPRESSPACKAGEOUTLETPIPE1", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:O2REPRESSPACKAGEOUTLETPIPE2", 500./ LBH);

	SetPipeMaxFlow("HYDRAULIC:POTABLEH2OINLETPIPE", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:POTABLEH2OINLETTOWASTEPIPE", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:POTABLEH2OTOWASTEPIPE", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:WASTEH2OINLETPIPE", 100./ LBH);
	SetPipeMaxFlow("HYDRAULIC:WASTEH2OVENTPIPE", 150./ LBH);
	SetPipeMaxFlow("HYDRAULIC:WASTEH2OINLETVENTPIPE", 100./ LBH);

	CrewStatus.Init(this);

	//
	// Wire up internal systems.
	//

	cws.WireTo(&CWMnaCircuitBraker, &CWMnbCircuitBraker);
	agc.WirePower(&GNComputerMnACircuitBraker, &GNComputerMnBCircuitBraker);
	agc.SetDSKY2(&dsky2);
	imu.WireToBuses(&GNIMUMnACircuitBraker, &GNIMUMnBCircuitBraker, &GNPowerIMUSwitch);
	imu.WireHeaterToBuses((Boiler *) Panelsdk.GetPointerByString("ELECTRIC:IMUHEATER"), &GNIMUHTRMnACircuitBraker, &GNIMUHTRMnBCircuitBraker);
	dockingprobe.WireTo(&DockProbeMnACircuitBraker, &DockProbeMnBCircuitBraker);   

	// SCS initialization
	bmag1.Init(1, this, &SystemMnACircuitBraker, &StabContSystemAc1CircuitBraker, (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:BMAGHEATER1"));
	bmag2.Init(2, this, &SystemMnBCircuitBraker, &StabContSystemAc2CircuitBraker, (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:BMAGHEATER2"));
	gdc.Init(this);
	ascp.Init(this);
	eda.Init(this);
	rjec.Init(this);
	eca.Init(this);
	tvsa.Init(this);
	ems.Init(this, &EMSMnACircuitBraker, &EMSMnBCircuitBraker, &NumericRotarySwitch, &LightingNumIntLMDCCB);
	ordeal.Init(&ORDEALEarthSwitch, &OrdealAc2CircuitBraker, &OrdealMnBCircuitBraker, &ORDEALAltSetRotary, &ORDEALModeSwitch, &ORDEALSlewSwitch, &ORDEALFDAI1Switch, &ORDEALFDAI2Switch);
	mechanicalAccelerometer.Init(this);

	qball.Init(this);

	// Telecom initialization
	pmp.Init(this);
	usb.Init(this);
	hga.Init(this);
	omnia.Init(this);
	omnib.Init(this);
	omnic.Init(this);
	omnid.Init(this);
	dataRecorder.Init(this);
	pcm.Init(this);
	vhfranging.Init(this, &VHFStationAudioRCB, &VHFRangingSwitch, &VHFRNGSwitch, &vhftransceiver);
	vhftransceiver.Init(&VHFAMASwitch, &VHFAMBSwitch, &RCVOnlySwitch, &VHFStationAudioCTRCB);
	RRTsystem.Init(this, &RNDZXPNDRFLTBusCB, &RNDZXPDRSwitch, &Panel100RNDZXPDRSwitch, &LeftSystemTestRotarySwitch, &RightSystemTestRotarySwitch);

	//Instrumentation
	sce.Init(this);

	InstrumentationPowerFeeder.WireToBuses(&InstrumentLightingESSMnACircuitBraker, &InstrumentLightingESSMnBCircuitBraker);
	ECSPressGroups1Feeder.WireToBuses(&ECSTransducerPressGroup1MnACircuitBraker, &ECSTransducerPressGroup1MnBCircuitBraker);
	ECSPressGroups2Feeder.WireToBuses(&ECSTransducerPressGroup2MnACircuitBraker, &ECSTransducerPressGroup2MnBCircuitBraker);
	ECSTempTransducerFeeder.WireToBuses(&ECSTransducerTempMnACircuitBraker, &ECSTransducerTempMnBCircuitBraker);
	ECSWastePotTransducerFeeder.WireToBuses(&ECSTransducerWastePOTH2OMnACircuitBraker, &ECSTransducerWastePOTH2OMnBCircuitBraker);
	ECSSecTransducersFeeder.WireToBuses(&ECSSecCoolLoopXducersMnACircuitBraker, &ECSSecCoolLoopXducersMnBCircuitBraker);

	H2Tank1TempSensor.Init(&CryogenicQTYAmpl1CB, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1"));
	H2Tank2TempSensor.Init(&CryogenicQTYAmpl2CB, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2"));
	O2Tank1TempSensor.Init(&CryogenicQTYAmpl1CB, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1"));
	O2Tank2TempSensor.Init(&CryogenicQTYAmpl2CB, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2"));

	CabinPressSensor.Init(&ECSPressGroups2Feeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"));
	CabinTempSensor.Init(&ECSTempTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"));
	SuitCabinDeltaPressSensor.Init(&Panel276CB2, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE"), (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"));
	CO2PartPressSensor.Init(&ECSPressGroups2Feeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUIT"));
	O2SurgeTankPressSensor.Init(&Panel276CB2, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK"));
	SuitTempSensor.Init(&ECSTempTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUIT"));
	WasteH2OQtySensor.Init(&ECSWastePotTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK"));
	PotH2OQtySensor.Init(&ECSWastePotTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK"));
	SuitPressSensor.Init(&ECSPressGroups1Feeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUIT"));
	SuitCompressorDeltaPSensor.Init(&ECSPressGroups1Feeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUIT"), (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE"));
	GlycolPumpOutPressSensor.Init(&ECSPressGroups1Feeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET"));
	GlyEvapOutSteamTempSensor.Init(&ECSTempTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET")); //Should be steam, not glycol temperature
	GlyEvapOutTempSensor.Init(&ECSTempTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET"));
	GlycolAccumQtySensor.Init(&ECSPressGroups1Feeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR"));
	ECSRadOutTempSensor.Init(&ECSTempTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET"));
	GlyEvapBackPressSensor.Init(&ECSPressGroups2Feeder, (h_Evaporator *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR"));
	ECSO2FlowO2SupplyManifoldSensor.Init(&ECSPressGroups2Feeder, (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOROUTLET"));
	O2SupplyManifPressSensor.Init(&ECSPressGroups2Feeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:O2SUPPLYMANIFOLD"));
	SecGlyPumpOutPressSensor.Init(&ECSSecTransducersFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATORINLET"));
	SecEvapOutLiqTempSensor.Init(&ECSTempTransducerFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOROUTLET"));
	SecGlycolAccumQtySensor.Init(&ECSSecTransducersFeeder, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR"));
	SecEvapOutSteamPressSensor.Init(&ECSSecTransducersFeeder, (h_Evaporator *)Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR"));
	PriEvapInletTempSensor.Init(&Panel276CB2, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATORINLET"));
	PriRadInTempSensor.Init(&CONTHTRSMnBCircuitBraker, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET"));
	SecRadInTempSensor.Init(&ECSSecCoolLoopRADHTRMnACircuitBraker, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATORINLET"));
	SecRadOutTempSensor.Init(&ECSSecCoolLoopRADHTRMnACircuitBraker, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATOROUTLET"));

	// Optics initialization
	optics.Init(this);

	// SPS initialization
	SPSPropellant.Init(&GaugingMnACircuitBraker, &GaugingMnBCircuitBraker, &SPSGaugingSwitch, 
		               (h_Radiator *) Panelsdk.GetPointerByString("HYDRAULIC:SPSPROPELLANTLINE"));
	SPSEngine.Init(this);
	SPSEngine.pitchGimbalActuator.Init(this, tvsa.GetPitchServoAmp(), &Pitch1Switch, &Pitch2Switch,
		                               MainBusA, &PitchBatACircuitBraker, MainBusB, &PitchBatBCircuitBraker);
	SPSEngine.yawGimbalActuator.Init(this, tvsa.GetYawServoAmp(), &Yaw1Switch, &Yaw2Switch,
		                             MainBusA, &YawBatACircuitBraker, MainBusB, &YawBatBCircuitBraker);

	SPSPropellantLineHeaterA = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:SPSPROPELLANTLINEHEATERA");
	SPSPropellantLineHeaterB = (Boiler *) Panelsdk.GetPointerByString("ELECTRIC:SPSPROPELLANTLINEHEATERB");


	// SM RCS initialization
	SMQuadARCS.Init(th_rcs_a, (h_Radiator *) Panelsdk.GetPointerByString("HYDRAULIC:SMRCSQUADA"));
	SMQuadBRCS.Init(th_rcs_b, (h_Radiator *) Panelsdk.GetPointerByString("HYDRAULIC:SMRCSQUADB"));
	SMQuadCRCS.Init(th_rcs_c, (h_Radiator *) Panelsdk.GetPointerByString("HYDRAULIC:SMRCSQUADC"));
	SMQuadDRCS.Init(th_rcs_d, (h_Radiator *) Panelsdk.GetPointerByString("HYDRAULIC:SMRCSQUADD"));

	SMRCSHelium1ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium1BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSHelium1CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium1DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSHelium2ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium2BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSHelium2CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSHelium2DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSProp1ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp1BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSProp1CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp1DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSProp2ASwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp2BSwitch.WireTo(&PrplntIsolMnACircuitBraker);
	SMRCSProp2CSwitch.WireTo(&PrplntIsolMnBCircuitBraker);
	SMRCSProp2DSwitch.WireTo(&PrplntIsolMnACircuitBraker);

	SMRCSHelium1ATalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSHelium1BTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSHelium1CTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSHelium1DTalkback.WireTo(&SMHeatersBMnACircuitBraker);

	SMRCSHelium2ATalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSHelium2BTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSHelium2CTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSHelium2DTalkback.WireTo(&SMHeatersBMnACircuitBraker);

	SMRCSProp1ATalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSProp1BTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSProp1CTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSProp1DTalkback.WireTo(&SMHeatersBMnACircuitBraker);

	SMRCSProp2ATalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSProp2BTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSProp2CTalkback.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSProp2DTalkback.WireTo(&SMHeatersBMnACircuitBraker);
	
	SMRCSHeaterASwitch.WireTo(&SMHeatersAMnBCircuitBraker);
	SMRCSHeaterBSwitch.WireTo(&SMHeatersBMnACircuitBraker);
	SMRCSHeaterCSwitch.WireTo(&SMHeatersCMnBCircuitBraker);
	SMRCSHeaterDSwitch.WireTo(&SMHeatersDMnACircuitBraker);

	// CM RCS initialization
	CMRCS1.Init(th_att_cm_sys1, (h_Radiator *) Panelsdk.GetPointerByString("HYDRAULIC:CMRCSHELIUM1"), &CMRCS2, &RCSLogicMnACircuitBraker, &PyroBusA, &SMHeatersBMnACircuitBraker);
	CMRCS2.Init(th_att_cm_sys2, (h_Radiator *) Panelsdk.GetPointerByString("HYDRAULIC:CMRCSHELIUM2"), NULL,    &RCSLogicMnBCircuitBraker, &PyroBusB, &SMHeatersAMnBCircuitBraker);

	CMRCSProp1Switch.WireTo(&PrplntIsolMnACircuitBraker);
	CMRCSProp2Switch.WireTo(&PrplntIsolMnBCircuitBraker);

	CMRCSProp1Talkback.WireTo(&SMHeatersBMnACircuitBraker);
	CMRCSProp2Talkback.WireTo(&SMHeatersAMnBCircuitBraker);

	SideHatch.Init(this, &HatchGearBoxSelector, &HatchActuatorHandleSelector, &HatchActuatorHandleSelectorOpen, &HatchVentValveRotary);
	ForwardHatch.Init(this, (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:FORWARDHATCHPIPE"));

	WaterController.Init(this, (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK"),
		                 (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK"),
		                 (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OINLET"),
						 (h_Tank *) Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OINLET"),
						 (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OVENTPIPE"),
						 (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OINLETVENTPIPE"));
	
	GlycolCoolingController.Init(this);
	LMTunnelVent.Init((h_Valve *)Panelsdk.GetPointerByString("HYDRAULIC:CSMTUNNEL:OUT2"),
					  (h_Valve *)Panelsdk.GetPointerByString("HYDRAULIC:LMTUNNELPRESSURIZATIONVALVE"),
					  &LMTunnelVentValve);
	PressureEqualizationValve.Init((h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:FORWARDHATCHPIPE"),
					  &PressEqualValve, &ForwardHatch);

	// Initialize joystick
	RHCNormalPower.WireToBuses(&ContrAutoMnACircuitBraker, &ContrAutoMnBCircuitBraker);
	RHCDirect1Power.WireToBuses(&ContrDirectMnA1CircuitBraker, &ContrDirectMnB1CircuitBraker);
	RHCDirect2Power.WireToBuses(&ContrDirectMnA2CircuitBraker, &ContrDirectMnB2CircuitBraker);
	
	js_enabled = 0;  // Disabled
	rhc_id = -1;     // Disabled
	rhc_rot_id = -1; // Disabled
	rhc_sld_id = -1; // Disabled
	rhc_rzx_id = -1; // Disabled
	rhc_pov_id = -1; // Disabled
	thc_id = -1;     // Disabled
	thc_rot_id = -1; // Disabled
	thc_sld_id = -1; // Disabled
	thc_rzx_id = -1; // Disabled
	thc_pov_id = -1; // Disabled
	thc_debug = -1;
	rhc_debug = -1;
	rhc_thctoggle = false;
	rhc_thctoggle_id = -1;
	rhc_auto = false;
	thc_auto = false;
	rhc_thctoggle_pressed = false;
}

void Saturn::SetPipeMaxFlow(char *pipe, double flow) {

	h_Pipe *p = (h_Pipe *) Panelsdk.GetPointerByString(pipe);
	p->flowMax = flow; 
}

void Saturn::SystemsTimestep(double simt, double simdt, double mjd) {

	//
	// Don't clock the computer and the internal systems unless we're actually at pre-launch.
	//

	if (stage == ONPAD_STAGE && MissionTime >= -10800) {	// 3h 00min before launch
		// Slow down time acceleration
		if (oapiGetTimeAcceleration() > 1.0)
			oapiSetTimeAcceleration(1.0);

		stage = PRELAUNCH_STAGE;
	}
	else if (stage >= PRELAUNCH_STAGE) {

		//
		// Timestep the internal systems, there can be multiple systems timesteps in one Orbiter timestep
		//

		SystemsInternalTimestep(simdt);

		//
		// Do the "normal" Orbiter timestep, some devices are done in clbkPostStep
		//

		dsky.Timestep(MissionTime);
		dsky2.Timestep(MissionTime);
		agc.Timestep(MissionTime, simdt);
		optics.TimeStep(simdt);

		//
		// If we've seperated from the SIVb, the IU is history.
		//
		if (stage < CSM_LEM_STAGE)
		{
			iu->Timestep(MissionTime, simt, simdt, mjd);
		}
		if (stage == LAUNCH_STAGE_SIVB || stage == STAGE_ORBIT_SIVB)
		{
			sivb->Timestep(simdt);
		}

		bmag1.Timestep(simdt);
		bmag2.Timestep(simdt);
		ascp.TimeStep(simdt);
		gdc.Timestep(simdt);
		eca.TimeStep(simdt);
		rjec.TimeStep(simdt);
		tvsa.TimeStep(simdt);
		eda.Timestep(simdt);
		cws.TimeStep(MissionTime);
		dockingprobe.TimeStep(MissionTime, simdt);
		secs.Timestep(MissionTime, simdt);
		els.Timestep(MissionTime, simdt);
		ordeal.Timestep(simdt);
		mechanicalAccelerometer.Timestep(simdt);
		MissionTimerDisplay.Timestep(simt, simdt, false);
		MissionTimer306Display.Timestep(simt, simdt, false);
		EventTimerDisplay.Timestep(simt, simdt, true);
		EventTimer306Display.Timestep(simt, simdt, true);
		fdaiLeft.Timestep(MissionTime, simdt);
		fdaiRight.Timestep(MissionTime, simdt);
		SPSPropellant.Timestep(MissionTime, simdt);
		JoystickTimestep();
		EPSTimestep();
		SMQuadARCS.Timestep(MissionTime, simdt);
		SMQuadBRCS.Timestep(MissionTime, simdt);
		SMQuadCRCS.Timestep(MissionTime, simdt);
		SMQuadDRCS.Timestep(MissionTime, simdt);
		CMRCS1.Timestep(MissionTime, simdt);	// Must be after JoystickTimestep
		CMRCS2.Timestep(MissionTime, simdt);
		SideHatch.Timestep(simdt);
		ForwardHatch.Timestep(simdt);

		//Telecom update is last so telemetry reflects the current state
		pmp.TimeStep(MissionTime);
		usb.TimeStep(MissionTime);
		hga.TimeStep(MissionTime, simdt);
		omnia.TimeStep();
		omnib.TimeStep();
		omnic.TimeStep();
		omnid.TimeStep();
		if (pMission->CSMHasVHFRanging()) vhfranging.TimeStep(simdt);
		vhftransceiver.Timestep();
		sce.Timestep();
		dataRecorder.TimeStep( MissionTime, simdt );
		RRTsystem.TimeStep(simdt);

		//
		// Systems state handling
		//
		if (!firstSystemsTimeStepDone) {
			firstSystemsTimeStepDone = true;
		}
		else {
			double scdp;
			AtmosStatus atm;
			GetAtmosStatus(atm);

			//
			// Prime Crew Prelaunch 
			//

			if (eventControl.PRIME_CREW_PRELAUNCH == MINUS_INFINITY) {
				if (systemsState > SATSYSTEMS_NONE && MissionTime >= -9600) {	 // T-2h 40min before launch 
					
					// Crew ingress
					
					if (Crewed) {
						SetCrewNumber(3);
					}

					// According to the AOH the SPS Oxidizer Flow valve is in max. position during backup crew prelaunch, but 
					// normal during prime crew prelaunch. It's unknown when and how the valve is changed to normal 
					// (presumably by GSE), so it's done here for now
					
					SPSPropellant.ResetOxidFlowValve();

					//
					// Event handling.
					//
					eventControl.PRIME_CREW_PRELAUNCH = MissionTime;
				}
			}


			switch (systemsState) {
			case SATSYSTEMS_NONE:				
				// No crew 
				SetCrewNumber(0);

				// No leak
				CabinPressureReliefValve1.SetLeakSize(0);
				CabinPressureReliefValve2.SetLeakSize(0);

				// Reset cabin pressure regulator and set to 14.7 psi
				CabinPressureRegulator.Reset(); 
				CabinPressureRegulator.ResetMaxFlow(); 
				CabinPressureRegulator.SetPressurePSI(14.7);
				
				// ECS radiators in prelaunch configuration
				PrimEcsRadiatorExchanger1->SetLength(8.0);
				PrimEcsRadiatorExchanger2->SetLength(8.0);
				SecEcsRadiatorExchanger1->SetLength(0);
				SecEcsRadiatorExchanger2->SetLength(0);

				// GSE provides electrical power
				MainBusAController.SetGSEState(1);
				MainBusBController.SetGSEState(1);

				// Enable GSE SM RCS heaters
				*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADAHEATER:PUMP") = SP_PUMP_AUTO;
				*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADBHEATER:PUMP") = SP_PUMP_AUTO;
				*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADCHEATER:PUMP") = SP_PUMP_AUTO;
				*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADDHEATER:PUMP") = SP_PUMP_AUTO;

				// Reduce fuel cell cooling power because of low fuel cell load
				*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR1:RAD") = 2.0;
				*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR2:RAD") = 2.0;
				*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR3:RAD") = 2.0;
				*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR4:RAD") = 2.0;

				// 
				// Event handling.
				//
				eventControl.BACKUP_CREW_PRELAUNCH = MissionTime;

				// Next state
				systemsState = SATSYSTEMS_PRELAUNCH;
				lastSystemsMissionTime = MissionTime; 
				break;

			case SATSYSTEMS_PRELAUNCH:
				//	Should be triggered by the suit compressor, the Mission Time condition is just in case 
				// the suit compressor isn't turned on until 15 min before launch
				if ((SuitCompressor1->pumping || SuitCompressor2->pumping) || MissionTime >= -900) {
			
					// Close cabin pressure regulator 
					CabinPressureRegulator.Close();

					// Suit compressors to prelaunch configuration
					SuitCompressor1->fan_cap = 110000.0;
					SuitCompressor2->fan_cap = 110000.0;

					// Open suit relief valve and close O2 demand regulator in order to 
					// equalize suit cabin pressure difference
					O2DemandRegulator.Close();
					O2DemandRegulator.OpenSuitReliefValve();

					// Next state
					systemsState = SATSYSTEMS_CREWINGRESS_1;
					lastSystemsMissionTime = MissionTime; 
				}	
				break;

			case SATSYSTEMS_CREWINGRESS_1:
				scdp = (atm.SuitReturnPressurePSI - atm.CabinPressurePSI) * (INH2O / PSI);
				if ((scdp > 0.0 && MissionTime - lastSystemsMissionTime >= 50) || MissionTime >= -900) {	// Suit Cabin delta p is equalized

					// Reset (i.e. close) suit relief valve again
					O2DemandRegulator.ResetSuitReliefValve();

					// Next state
					systemsState = SATSYSTEMS_CREWINGRESS_2;
					lastSystemsMissionTime = MissionTime; 
				}
				break;

			case SATSYSTEMS_CREWINGRESS_2:
				scdp = (atm.SuitReturnPressurePSI - atm.CabinPressurePSI) * (INH2O / PSI);
				if ((scdp > 1.3 && MissionTime - lastSystemsMissionTime >= 10) || MissionTime >= -900) {	// Suit Cabin delta p is established

					// Reset (i.e. open) cabin pressure regulator again, max flow to 0.25 lb/h  
					CabinPressureRegulator.SetMaxFlowLBH(0.25);
					CabinPressureRegulator.Reset(); 

					// Cabin leak
					CabinPressureReliefValve1.SetLeakSize(0.0002);

					// Next state
					systemsState = SATSYSTEMS_CABINCLOSEOUT;
					lastSystemsMissionTime = MissionTime; 
				}	
				break;

			case SATSYSTEMS_CABINCLOSEOUT:
				if (MissionTime >= -6000) {	// 1h 40min before launch

					if (SaturnType == SAT_SATURNV) {
						// Play cabin closeout sound
						CabincloseoutS.play();
						CabincloseoutS.done();
					}

					// Next state
					systemsState = SATSYSTEMS_GSECONNECTED_1;
					lastSystemsMissionTime = MissionTime; 
				}
				break;	

			case SATSYSTEMS_GSECONNECTED_1:
				if (MissionTime >= -900) {	// 15min before launch

					// Disable GSE electrical power 
					// Reference: Apollo 15 Flight Journal (http://history.nasa.gov/ap15fj/01launch_to_earth_orbit.htm)
					MainBusAController.SetGSEState(0);
					MainBusBController.SetGSEState(0);

					// Disable GSE SM RCS heaters
					*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADAHEATER:PUMP") = SP_PUMP_OFF;
					*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADBHEATER:PUMP") = SP_PUMP_OFF;
					*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADCHEATER:PUMP") = SP_PUMP_OFF;
					*(int*) Panelsdk.GetPointerByString("ELECTRIC:GSESMRCSQUADDHEATER:PUMP") = SP_PUMP_OFF;

					// Set fuel cell cooling power to normal
					*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR1:RAD") = 6.8;
					*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR2:RAD") = 6.8;
					*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR3:RAD") = 6.8;
					*(double *) Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR4:RAD") = 6.8;

					// Next state
					systemsState = SATSYSTEMS_GSECONNECTED_2;
					lastSystemsMissionTime = MissionTime; 
				}
				break;

			case SATSYSTEMS_GSECONNECTED_2:
				if (MissionTime >= -135) {	// 2min 15sec before launch
					// Disable GSE devices
					*(int*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMGSEHEATEXCHANGER:PUMP") = SP_PUMP_OFF;
					*(int*) Panelsdk.GetPointerByString("HYDRAULIC:SECGSEHEATEXCHANGER:PUMP") = SP_PUMP_OFF;

					// Next state
					systemsState = SATSYSTEMS_READYTOLAUNCH;
					lastSystemsMissionTime = MissionTime; 
				}
				break;

			case SATSYSTEMS_READYTOLAUNCH:
				if (GetAtmPressure() <= 8.5 / PSI) {					
					// Cabin pressure regulator and relief pressure to boost configuration, disable max. flow
					CabinPressureReliefValve1.SetReliefPressurePSI(4.82); // 5 psi - 5 inH2O
					CabinPressureRegulator.SetPressurePSI(4.7);
					CabinPressureRegulator.ResetMaxFlow();

					// Cabin leak
					CabinPressureReliefValve1.SetLeakSize(0.001);

					// Suit compressors to flight configuration
					SuitCompressor1->fan_cap = 65000.0;
					SuitCompressor2->fan_cap = 65000.0;

					// ECS radiators now working normally
					PrimEcsRadiatorExchanger1->SetLength(10.0);
					PrimEcsRadiatorExchanger2->SetLength(10.0);
					SecEcsRadiatorExchanger1->SetLength(10.0);
					SecEcsRadiatorExchanger2->SetLength(10.0);

					// Next state
					systemsState = SATSYSTEMS_CABINVENTING;
					lastSystemsMissionTime = MissionTime; 
				}		
				break;

			case SATSYSTEMS_CABINVENTING:

				if (SuitCircuitReturnValve.IsOpen() && GetAtmPressure() < 3. / PSI) {
					// Reset (i.e. open) O2 demand regulator
					O2DemandRegulator.Reset();

					// Cabin regulator and relief pressure to flight configuration
					CabinPressureRegulator.SetPressurePSI(5.0);
					CabinPressureReliefValve1.SetReliefPressurePSI(6.0); 

					// Next state
					systemsState = SATSYSTEMS_FLIGHT;
					lastSystemsMissionTime = MissionTime; 
				}
				break;

			case SATSYSTEMS_FLIGHT:
				if (GetAtmPressure() > 4. / PSI) {					
					// Suit compressors to landing configuration
					SuitCompressor1->fan_cap = 110000.0;
					SuitCompressor2->fan_cap = 110000.0;

					// Next state
					systemsState = SATSYSTEMS_LANDING;
					lastSystemsMissionTime = MissionTime; 
				}
				break;
			}
		}
	}


//------------------------------------------------------------------------------------
// Various debug prints
//------------------------------------------------------------------------------------

//CSM Connector Debug Lines

//h_Pipe* csmtunnelpipe = (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:CSMTUNNELUNDOCKED");
//double *pressequalFlow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:FORWARDHATCHPIPE:FLOW");
//sprintf(oapiDebugString(), "CSM Tunnel: %lf LM Tunnel: %lf TunnelFlow %lf EqFlow: %lf", (csmtunnelpipe->in->parent->space.Press)*PSI, (csmtunnelpipe->out->parent->space.Press)*PSI, (csmtunnelpipe->flow)*LBH, *pressequalFlow*LBH);

#ifdef _DEBUG

/*		sprintf(oapiDebugString(), "Bus A %3.1fA/%3.1fV, Bus B %3.1fA/%3.1fV, Batt A %3.1fV/%3.1fA/%.3f, Batt B %3.1fV/%.3f Batt C %3.1fV/%.3f Charg %2.1fV/%3.1fA FC1 %3.1fV/%3.1fA", 
			MainBusA->Current(), MainBusA->Voltage(), MainBusB->Current(), MainBusB->Voltage(),
			EntryBatteryA->Voltage(), EntryBatteryA->Current(), EntryBatteryA->Capacity() / 5508000.0, EntryBatteryB->Voltage(), EntryBatteryB->Capacity() / 5508000.0, EntryBatteryC->Voltage(), EntryBatteryC->Capacity() / 5508000.0,
			BatteryCharger.Voltage(), BatteryCharger.Current(),
			FuelCells[0]->Voltage(), FuelCells[0]->Current());
*/
/*		sprintf(oapiDebugString(), "FC1 %3.3fV/%3.3fA/%3.3fW FC2 %3.3fV/%3.3fA/%3.3fW FC3 %3.3fV/%3.3fA/%3.3fW",
			FuelCells[0]->Voltage(), FuelCells[0]->Current(), FuelCells[0]->PowerLoad(),
			FuelCells[1]->Voltage(), FuelCells[1]->Current(), FuelCells[1]->PowerLoad(),
			FuelCells[2]->Voltage(), FuelCells[2]->Current(), FuelCells[2]->PowerLoad());
*/

/*	sprintf(oapiDebugString(), "PyroBus A %3.1fA/%3.1fV, PyroBus B %3.1fA/%3.1fV",
		PyroBusA.Current(), PyroBusA.Voltage(), PyroBusB.Current(), PyroBusB.Voltage());
*/
	
	double *massCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:MASS");
	double *tempCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	double *pressCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	double *pressCabinCO2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	double *co2removalrate=(double*)Panelsdk.GetPointerByString("ELECTRIC:SUITCOMPRESSORCO2ABSORBER1:CO2REMOVALRATE");

	double *massEarth=(double*)Panelsdk.GetPointerByString("HYDRAULIC:EARTH:MASS");
	double *tempEarth=(double*)Panelsdk.GetPointerByString("HYDRAULIC:EARTH:TEMP");
	double *pressEarth=(double*)Panelsdk.GetPointerByString("HYDRAULIC:EARTH:PRESS");

	double *O2flowCabin=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOW");
	double *O2flowCabinEmer=(double*)Panelsdk.GetPointerByString("HYDRAULIC:EMERGENCYCABINPRESSUREREGULATOR:FLOW");
	double *O2flowDemand = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOW");
	double *O2flowDirect=(double*)Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE:FLOW");
	double *suitreliefflow=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITRELIEFVALVE:FLOW");
	double *suittestflow=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITTESTVALVE:FLOW");
	double *cabinreliefflow1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFVALVE1:FLOW");
	double *cabinreliefflow2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFVALVE2:FLOW");
	double *cabinreliefinletflow1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFINLET1:FLOW");
	double *cabinreliefinletflow2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSURERELIEFINLET2:FLOW");
	double *suitcircuitreturnflow=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNINLET:FLOW");
	double *o2tank1smsupplyflow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLYINLET1:FLOW");

	double *o2smsupplyinsize = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY:IN:SIZE");
	double *o2mainreginsize = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:IN:SIZE");
	double *O2Tank1Outsize = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:OUT:SIZE");

	//sprintf(oapiDebugString(), "O2SMINSize %f O2tank1smsupplyflow %f O2T1Outsize %f", *o2smsupplyinsize, *o2tank1smsupplyflow, *O2Tank1Outsize);
	//sprintf(oapiDebugString(), "EMERG FLOW %lf ", *O2flowCabinEmer*LBH);

	double *pressSuit=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:PRESS");
	double *tempSuit=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:TEMP");
	double *pressSuitCO2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:CO2_PPRESS");
	double *massSuitH2O=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUIT:H2O_MASS");
	double *pressSuitCRV=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS");
	double *tempSuitCRV=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:TEMP");

	//double *tempCabinRad1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINRADIATOR1:TEMP");
	//double *tempCabinRad2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:CABINRADIATOR2:TEMP");

	double *voltageA=(double*)Panelsdk.GetPointerByString("ELECTRIC:DC_A:VOLTS");
	double *amperageA=(double*)Panelsdk.GetPointerByString("ELECTRIC:DC_A:AMPS");


/*	sprintf(oapiDebugString(), "Cabin - Mass [g] %.2f Temp [K] %.2f Press [psi] %.2f CO2 PPress [mmHg] %.2f  CO2ABSORBER - Co2Rate %.2f  DC-A - Volt %.2f Amp %.2f", 
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064,
		*co2removalrate,
		*voltageA, *amperageA); 
*/

	double *massO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:MASS");
	double *tempO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:TEMP");
	double *pressO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:PRESS");
	double *vapormassO2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:O2_VAPORMASS");
	double *energyO2Tank1 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:ENERGY");
	double *isonO2Tank1Heater = (double*)Panelsdk.GetPointerByString("ELECTRIC:O2TANK1HEATER:ISON");
	double *isonO2Tank1Fan = (double*)Panelsdk.GetPointerByString("ELECTRIC:O2TANK1FAN:ISON");

	double *massO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:MASS");
	double *tempO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:TEMP");
	double *pressO2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:PRESS");
	double *vapormassO2Tank2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:O2_VAPORMASS");
	double *energyO2Tank2 = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:ENERGY");
	double *isonO2Tank2Heater = (double*)Panelsdk.GetPointerByString("ELECTRIC:O2TANK2HEATER:ISON");
	double *isonO2Tank2Fan = (double*)Panelsdk.GetPointerByString("ELECTRIC:O2TANK2FAN:ISON");

	double *massO2SurgeTank=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:MASS");
	double *tempO2SurgeTank=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:TEMP");
	double *pressO2SurgeTank=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:PRESS");

	double *massO2RepressPkg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGE:MASS");
	double *tempO2RepressPkg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGE:TEMP");
	double *pressO2RepressPkg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGE:PRESS");

	double *massO2RepressPkgOut=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGEOUTLET:MASS");
	double *tempO2RepressPkgOut=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGEOUTLET:TEMP");
	double *pressO2RepressPkgOut=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGEOUTLET:PRESS");
	double *flowO2RepressPkgOut=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGEOUTLETPIPE3:FLOW");

	double *massO2SMSupply=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY:MASS");
	double *tempO2SMSupply=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY:TEMP");
	double *pressO2SMSupply=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLY:PRESS");

	double *massO2MainReg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:MASS");
	double *tempO2MainReg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:TEMP");
	double *pressO2MainReg=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOR:PRESS");

	double *tempRad1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR1:TEMP");
	double *tempRad2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR2:TEMP");
	double *tempRad3=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR3:TEMP");
	double *tempRad4=(double*)Panelsdk.GetPointerByString("HYDRAULIC:FUELCELLRADIATOR4:TEMP");
	double tempRad = (*tempRad1 + *tempRad2 + *tempRad3 + *tempRad4) / 4.0;

	double *tempFCC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING:TEMP");
	double *tempFCC2=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2COOLING:TEMP");
	double *tempFCC3=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3COOLING:TEMP");

	double *massH2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:MASS");
	double *tempH2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:TEMP");
	double *pressH2Tank1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:PRESS");
	double *isonH2Tank1Heater=(double*)Panelsdk.GetPointerByString("ELECTRIC:H2TANK1HEATER:ISON");
	double *isonH2Tank1Fan=(double*)Panelsdk.GetPointerByString("ELECTRIC:H2TANK1FAN:ISON");

	double *massH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:MASS");
	double *tempH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:TEMP");
	double *pressH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:PRESS");
	double *isonH2Tank2Heater = (double*)Panelsdk.GetPointerByString("ELECTRIC:H2TANK2HEATER:ISON");
	double *isonH2Tank2Fan = (double*)Panelsdk.GetPointerByString("ELECTRIC:H2TANK2FAN:ISON");
	double *vapormassH2Tank2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:H2_VAPORMASS");

	double *massH2FC1M=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1MANIFOLD:MASS");
	double *tempH2FC1M=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1MANIFOLD:TEMP");
	double *pressH2FC1M=(double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1MANIFOLD:PRESS");

	double *massH2FC2M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2MANIFOLD:MASS");
	double *tempH2FC2M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2MANIFOLD:TEMP");
	double *pressH2FC2M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2MANIFOLD:PRESS");

	double *massH2FC3M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3MANIFOLD:MASS");
	double *tempH2FC3M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3MANIFOLD:TEMP");
	double *pressH2FC3M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3MANIFOLD:PRESS");

	double *massO2FC1M=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD:MASS");
	double *tempO2FC1M=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD:TEMP");
	double *pressO2FC1M=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD:PRESS");

	double *massO2FC2M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2MANIFOLD:MASS");
	double *tempO2FC2M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2MANIFOLD:TEMP");
	double *pressO2FC2M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2MANIFOLD:PRESS");

	double *massO2FC3M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD:MASS");
	double *tempO2FC3M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD:TEMP");
	double *pressO2FC3M = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD:PRESS");

	double *massWasteInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OINLET:MASS");
	double *tempWasteInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OINLET:TEMP");
	double *pressWasteInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OINLET:PRESS");
	double *massWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:MASS");
	double *tempWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:TEMP");
	double *pressWaste=(double*)Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:PRESS");

	double *massPotableInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OINLET:MASS");
	double *tempPotableInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OINLET:TEMP");
	double *pressPotableInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OINLET:PRESS");
	double *massPotable=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:MASS");
	double *tempPotable=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:TEMP");
	double *pressPotable=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:PRESS");

	double *flowPotableInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTOWASTEPIPE:FLOW");
	static double maxflow;
	if (*flowPotableInlet > maxflow)
		maxflow = *flowPotableInlet;

	double *voltFC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:VOLTS");
	double *ampFC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:AMPS");
	double *tempFC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:TEMP");
	double *dphFC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:DPH");
	double *voltFC2 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:VOLTS");
	double *ampFC2 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:AMPS");
	double *tempFC2 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:TEMP");
	double *dphFC2 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:DPH");
	double *voltFC3 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:VOLTS");
	double *ampFC3 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:AMPS");
	double *tempFC3 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:TEMP");
	double *dphFC3 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:DPH");
	double *h2flowFC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:H2FLOW");
	double *o2flowFC1=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:O2FLOW");
	double *h2flowFC2 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:H2FLOW");
	double *o2flowFC2 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:O2FLOW");
	double *h2flowFC3 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:H2FLOW");
	double *o2flowFC3 = (double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:O2FLOW");
	//double *ison=(double*)Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1COOLING:ISON");


	double *massAccu=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:MASS");
	double *tempAccu=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:TEMP");
	double *pressAccu=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:PRESS");

	double *massInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:MASS");
	double *tempInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:TEMP");
	double *pressInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:PRESS");

	double *massOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:MASS");
	double *tempOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:TEMP");
	double *pressOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:PRESS");

	double *tempEvapInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATORINLET:TEMP");
	double *pressEvapInlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATORINLET:PRESS");

	double *tempEvapOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET:TEMP");
	double *pressEvapOutlet=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET:PRESS");

	double *tempECSRad1=(double*)Panelsdk.GetPointerByString("HYDRAULIC:ECSRADIATOR1:TEMP");
	double *tempECSRad2=(double*)Panelsdk.GetPointerByString("HYDRAULIC:ECSRADIATOR2:TEMP");
	double *mixerRatio=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLEVAPINLETTEMPVALVE:RATIO");
	double *evapThrottle=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:THROTTLE");
	double *exchanger1Power=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER1:POWER");
	double *exchanger2Power=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMECSRADIATOREXCHANGER2:POWER");
	//double *suitExchangerPower=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITHEATEXCHANGER:POWER");
	double *suitCRVExchangerPower=(double*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMSUITCIRCUITHEATEXCHANGER:POWER");

	double *rcsTemp=(double*)Panelsdk.GetPointerByString("HYDRAULIC:SMRCSQUADA:TEMP");
	//sprintf(oapiDebugString(), "RCS-T %.1f", KelvinToFahrenheit(*rcsTemp)); 

/*	sprintf(oapiDebugString(), "RCS A PCK-T %.1f He-T %.1f p %.1f Prp-p %.1f qty %.1f", SMQuadARCS.GetPackageTempF(), SMQuadARCS.GetHeliumTempF(), 
		SMQuadARCS.GetHeliumPressurePSI(), SMQuadARCS.GetPropellantPressurePSI(), SMQuadARCS.GetPropellantQuantityToDisplay() * 100.); 
*/
/*	sprintf(oapiDebugString(), "RCS C PCK-T %.1f He-T %.1f p %.1f Prp-p %.1f qty %.1f", SMQuadCRCS.GetPackageTempF(), SMQuadCRCS.GetHeliumTempF(), 
		SMQuadCRCS.GetHeliumPressurePSI(), SMQuadCRCS.GetPropellantPressurePSI(), SMQuadCRCS.GetPropellantQuantityToDisplay() * 100.); 
*/
/*	sprintf(oapiDebugString(), "PIn-m %.1f T %.1f p %05.1f Pot-m %.1f T %.1f p %05.1f fl %05.1f %.1f WIn-m %.1f T %.1f p %05.1f Waste-m %.1f T %.1f p %05.1f deltaP %.1f Evap %.2f", 
		*massPotableInlet, *tempPotableInlet, *pressPotableInlet * 0.000145038,
		*massPotable, *tempPotable, *pressPotable * 0.000145038, *flowPotableInlet * LBH, maxflow * LBH,
		*massWasteInlet, *tempWasteInlet, *pressWasteInlet * 0.000145038,
		*massWaste, *tempWaste, *pressWaste * 0.000145038,
		(*pressPotable - *pressWaste) * 0.000145038, *evapThrottle);
*/

/*	sprintf(oapiDebugString(), "Acc-m %.0f T %.1f In-T %.1f p %5.1f Out-T %.1f Mix %.2f EIn-T %.1f EOut-T %.1f Th %.2f H2o %.1f Rd-T1 %.1f T2 %.1f C %.1f S %.1f H2o %.1f P %.1f", 
		*massAccu, *tempAccu,
		*tempInlet, *pressInlet * 0.000145038,
		*tempOutlet, *mixerRatio, 
		*tempEvapInlet, *tempEvapOutlet, *evapThrottle, *massWaste,
		*tempECSRad1, *tempECSRad2,
		*tempCabin, *tempSuit, *massSuitH2O, *suitCRVExchangerPower);
*/
/*	sprintf(oapiDebugString(), "Acc-m %8.2f T %.1f p %3.1f In-T %.1f p %5.1f Out-T %.1f p %5.1f EvIn-T %.1f p %5.1f HSrc-m %4.2f T %.1f p %5.1f Mixer-r %.4f Rad-T1 %.1f T2 %.1f", 
		*massAccu, *tempAccu, *pressAccu * 0.000145038,
		*tempInlet, *pressInlet * 0.000145038,
		*tempOutlet, *pressOutlet * 0.000145038,
		*tempEvapInlet, *pressEvapInlet * 0.000145038,
		*massHeatsource, *tempHeatsource, *pressHeatsource * 0.000145038,
		*mixerRatio, *tempECSRad1, *tempECSRad2);
*/
/*	sprintf(oapiDebugString(), "Acc-m %8.2f T %.1f p %3.1f In-T %.1f p %5.1f Out-T %.1f p %5.1f EvOut-T %.1f p %5.1f HSrc-m %4.2f T %.1f p %5.1f H2O-m %.2f T %.1f p %.1f Rad-T1 %.1f T2 %.1f", 
		*massAccu, *tempAccu, *pressAccu * 0.000145038,
		*tempInlet, *pressInlet * 0.000145038,
		*tempOutlet, *pressOutlet * 0.000145038,
		*tempEvapOutlet, *pressEvapOutlet * 0.000145038,
		*massHeatsource, *tempHeatsource, *pressHeatsource * 0.000145038,
		*massWaste, *tempWaste, *pressWaste * 0.000145038,
		*tempECSRad1, *tempECSRad2);
*/

	double *o2mrFlow=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATORINLET:FLOW");
	double *o2smFlow=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SMSUPPLYINLET1:FLOW");
	double *o2fc1inlet1Flow=(double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1INLET1:FLOW");
	double *o2fc2inlet1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2INLET1:FLOW");
	double *o2fc3inlet1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3INLET1:FLOW");
	double *h2fc1inlet1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1INLET1:FLOW");
	double *h2fc2inlet1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2INLET1:FLOW");
	double *h2fc3inlet1Flow = (double*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3INLET1:FLOW");

	// ECS Pressures

/*sprintf(oapiDebugString(), "MR %.2f SCRV %.2f SR %.2f CR %.2f CRI %.2f CompDp %.2f SuitCabDp %.2f, CabO2 %.2f, DemO2 %.2f DirO2 %.2f EMER %.2f  Cab-p %.2f T %.1f Suit-p %.2f T %.1f co2pp %.2f SCRV-p %.2f T %.1f STV %.2f MR %.2f", 
		*pressO2MainReg * 0.000145038, *suitcircuitreturnflow * LBH, *suitreliefflow * LBH, *cabinreliefflow2 * LBH, *cabinreliefinletflow2 * LBH,
		(*pressSuit - *pressSuitCRV) * 0.000145038, (*pressSuitCRV - *pressCabin) * INH2O,
		*O2flowCabin * LBH, *O2flowDemand * LBH, *O2flowDirect * LBH, *O2flowCabinEmer * LBH,
		*pressCabin * 0.000145038, *tempCabin,
		*pressSuit * 0.000145038, *tempSuit, *pressSuitCO2 * 0.00750064,
		*pressSuitCRV * 0.000145038, *tempSuitCRV, *suittestflow * LBH, *o2mrFlow * LBH);


	sprintf(oapiDebugString(), "SCRVF %.2f SRF %.2f CRF %.2f CRIF %.2f CabO2F %.2f, DemO2F %.2f DirO2F %.2f EMERF %.2f STVF %.2f MRF %.2f",
		*suitcircuitreturnflow * LBH, *suitreliefflow * LBH, *cabinreliefflow2 * LBH, *cabinreliefinletflow2 * LBH, *O2flowCabin * LBH, *O2flowDemand * LBH, *O2flowDirect * LBH, *O2flowCabinEmer * LBH, *suittestflow * LBH, *o2mrFlow * LBH);


		sprintf(oapiDebugString(), "Earth-m %.2f p %.2f T %.1f", *massEarth / 100., *pressEarth * PSI, *tempEarth);

*/
	 //Cabin O2 supply
/*	sprintf(oapiDebugString(), "O2T1-m %.1f T %.1f p %.1f O2T2-m %.1f T %.1f p %.1f O2SM-m %.1f T %.1f p %4.1f O2M-m %.1f T %.1f p %5.1f CAB-m %.1f T %.1f p %.1f CO2PP %.2f", 
		*massO2Tank1 / 1000.0, *tempO2Tank1, *pressO2Tank1 * 0.000145038,
		*massO2Tank2 / 1000.0, *tempO2Tank2, *pressO2Tank2 * 0.000145038,
		*massO2SMSupply / 1000.0, *tempO2SMSupply, *pressO2SMSupply * 0.000145038,
		*massO2MainReg / 1000.0, *tempO2MainReg, *pressO2MainReg * 0.000145038,
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064);
*/
	int *o2fc1reacvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL1MANIFOLD:IN:ISOPEN");
	int *h2fc1reacvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL1MANIFOLD:IN:ISOPEN");
	int *o2fc2reacvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL2MANIFOLD:IN:ISOPEN");
	int *h2fc2reacvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL2MANIFOLD:IN:ISOPEN");
	int *o2fc3reacvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:O2FUELCELL3MANIFOLD:IN:ISOPEN");
	int *h2fc3reacvlv = (int*)Panelsdk.GetPointerByString("HYDRAULIC:H2FUELCELL3MANIFOLD:IN:ISOPEN");

/*
	//O2 Tanks & Fans/Heaters
	sprintf(oapiDebugString(), "O2T1-m %.1f T %.1f Q %.1f p %.1f Fan %lf Heater %lf O2T2-m %.1f T %.1f Q %.1f p %.1f Fan %lf, Heater %lf",
		*massO2Tank1*LBS, KelvinToFahrenheit(*tempO2Tank1), *energyO2Tank1, *pressO2Tank1*PSI, *isonO2Tank1Fan, *isonO2Tank1Heater,
		*massO2Tank2*LBS, KelvinToFahrenheit(*tempO2Tank2), *energyO2Tank2, *pressO2Tank2*PSI, *isonO2Tank2Fan, *isonO2Tank2Heater);
*/

	// Fuel Cell H2
/*	
	sprintf(oapiDebugString(), "H2T1-m %.2f T %.1f p %.1f Fan %lf Heater %lf H2T2-m %.2f T %.1f p %.1f Fan %lf Heater %lf",
		*massH2Tank1*LBS, KelvinToFahrenheit(*tempH2Tank1), *pressH2Tank1*PSI, *isonH2Tank1Fan, *isonH2Tank1Heater,
		*massH2Tank2*LBS, KelvinToFahrenheit(*tempH2Tank2), *pressH2Tank2*PSI, *isonH2Tank2Fan, *isonH2Tank2Heater);
*/


/*
	//O2 Tanks & reaction valves
	sprintf(oapiDebugString(), "O2T1-m %.1f T %.1f Q %.1f p %.1f O2T2-m %.1f T %.1f Q %.1f p %.1f H2vlv1 %d O2vlv1 %d H2vlv2 %d O2vlv2 %d H2vlv3 %d O2vlv3 %d O2FC1 M %.1f T %.1f p %6.1f",
		*massO2Tank1*LBS, KelvinToFahrenheit(*tempO2Tank1), *energyO2Tank1, *pressO2Tank1*PSI,
		*massO2Tank2*LBS, KelvinToFahrenheit(*tempO2Tank2), *energyO2Tank2, *pressO2Tank2*PSI,
		*h2fc1reacvlv, *o2fc1reacvlv, *h2fc2reacvlv, *o2fc2reacvlv, *h2fc3reacvlv, *o2fc3reacvlv,
		*massO2FC1M*LBS, KelvinToFahrenheit(*tempO2FC1M), *pressO2FC1M*PSI);
*/
/*
	// FC Manifold & Pipes
	sprintf(oapiDebugString(), "H2FC1 M %.1f T %.1f p %6.1f O2FC1 M %.1f T %.1f p %6.1f",
	*massH2FC1M, *tempH2FC1M, *pressH2FC1M * 0.000145038,
	*massO2FC1M, *tempO2FC1M, *pressO2FC1M * 0.000145038);
/*

	// Suit O2 supply
/*	sprintf(oapiDebugString(), "O2T1-m %.1f T %.1f p %.1f O2T2-m %.1f T %.1f p %.1f O2SM-m %.1f T %.1f p %4.1f O2M-m %.1f T %.1f p %5.1f SUIT-T %.1f p %.1f Flow-SM %.2f MR %.2f ST %.2f", 
		*massO2Tank1 / 1000.0, *tempO2Tank1, *pressO2Tank1 * 0.000145038,
		*massO2Tank2 / 1000.0, *tempO2Tank2, *pressO2Tank2 * 0.000145038,
		*massO2SMSupply / 1000.0, *tempO2SMSupply, *pressO2SMSupply * 0.000145038,
		*massO2MainReg / 1000.0, *tempO2MainReg, *pressO2MainReg * 0.000145038,		
		*tempSuit, *pressSuit * 0.000145038, *o2smFlow * LBH, *o2mrFlow * LBH, *suittestflow * LBH); 
*/

/*	sprintf(oapiDebugString(), "O2ST-m %.1f T %.1f p %.1f O2RP-m %.1f T %.1f p %.1f RPOUT-m %.1f T %.1f p %.1f f %.1f O2SM-m %.1f T %.1f p %4.1f O2M-m %.1f T %.1f p %5.1f CAB-m %.1f T %.1f p %.1f CO2PP %.2f", 
		*massO2SurgeTank, *tempO2SurgeTank, *pressO2SurgeTank * 0.000145038,
		*massO2RepressPkg, *tempO2RepressPkg, *pressO2RepressPkg * 0.000145038,
		*massO2RepressPkgOut, *tempO2RepressPkgOut, *pressO2RepressPkgOut * 0.000145038, *flowO2RepressPkgOut * LBH,
		*massO2SMSupply, *tempO2SMSupply, *pressO2SMSupply * 0.000145038,
		*massO2MainReg, *tempO2MainReg, *pressO2MainReg * 0.000145038,
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064); 
*/
	// Fuel Cell, flow in lb/h
/*	sprintf(oapiDebugString(), "FC2-T %.1f FC3-T %.1f FC1-T %.1f V %.2f A %.2f H2Flow %.3f O2Flow %.3f Rad1-T %.1f Rad2-T %.1f Rad3-T %.1f Rad4-T %.1f", 
		*tempFC2, *tempFC3, *tempFC, *voltFC, *ampFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665,
		*tempRad1, *tempRad2, *tempRad3, *tempRad4);
*/		

	// Fuel Cell with tanks, flow in lb/h
/*
	sprintf(oapiDebugString(), "O21m %.2f T %.1f p %.1f H21m %.2f T %.1f p %.1f FC3-V %.2f A %.2f T %.1f H2Flow %.3f(%.3f) O2Flow %.3f(%.3f) H2FCM %.5f T %.1f p %.1f H2IN1 %.2f", 
		*massO2Tank1*LBS, KelvinToFahrenheit(*tempO2Tank1), *pressO2Tank1*PSI,
		*massH2Tank1*LBS, KelvinToFahrenheit(*tempH2Tank1), *pressH2Tank1*PSI,
		*voltFC3, *ampFC3, KelvinToFahrenheit(*tempFC3), *h2flowFC3*LBH, 0.00257*(*ampFC3), *o2flowFC3*LBH, 0.0204*(*ampFC3),
		*massH2FC1M*LBS, KelvinToFahrenheit(*tempH2FC1M), *pressH2FC3M*PSI,
		*h2fc3inlet1Flow*LBH);
*/

	// Fuel Cell 1 with manifolds, flow in lb/h
/*	
		sprintf(oapiDebugString(), "FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f H2FCM %.1f T %.1f p %6.1f O2FCM %.1f T %.1f p %6.1f", 
		*voltFC1, *ampFC1, *tempFC1, *h2flowFC1 * 7.93665, *o2flowFC1 * 7.93665, 
		*massH2FC1M, *tempH2FC1M, *pressH2FC1M * 0.000145038,
		*massO2FC1M, *tempO2FC1M, *pressO2FC1M * 0.000145038);
*/

	// Fuel Cell 2 with manifolds, flow in lb/h
/*	
		sprintf(oapiDebugString(), "FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f H2FCM %.1f T %.1f p %6.1f O2FCM %.1f T %.1f p %6.1f",
			*voltFC2, *ampFC2, *tempFC2, *h2flowFC2 * 7.93665, *o2flowFC2 * 7.93665,
			*massH2FC2M, *tempH2FC2M, *pressH2FC2M * 0.000145038,
			*massO2FC2M, *tempO2FC2M, *pressO2FC2M * 0.000145038);
*/

	// Fuel Cell Comparison
/*
		sprintf(oapiDebugString(), "FC1-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f FC2-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f",
			*voltFC1, *ampFC1, KelvinToFahrenheit(*tempFC1), *h2flowFC1*LBH, *o2flowFC1*LBH, *voltFC2, *ampFC2, KelvinToFahrenheit(*tempFC2), *h2flowFC2*LBH, *o2flowFC2*LBH);
*/
	// Fuel Cell H2
/*	sprintf(oapiDebugString(), "H2T1-m %.2f T %.1f p %.1f H %.1f F %.1f H2T2-m %.2f T %.1f p %.1f H2FCM-m %.2f T %.1f p %.1f FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f dpH %f Waste-m %.1f T %.1f p %4.1f", 
		*massH2Tank1, *tempH2Tank1, *pressH2Tank1 * 0.000145038, *isonH2Tank1Heater, *isonH2Tank1Fan, 
		*massH2Tank2, *tempH2Tank2, *pressH2Tank2 * 0.000145038,
		*massH2FC1M, *tempH2FC1M, *pressH2FC1M * 0.000145038,
		*voltFC, *ampFC, *tempFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665, *dphFC, 
		*massWaste, *tempWaste, *pressWaste * 0.000145038); 
*/

	// Fuel Cell H2 Tank2
/*	sprintf(oapiDebugString(), "H2T2-m %.2f vm %.2f T %.1f p %.1f FC-V %.2f A %.2f T %.1f H2Flow %.3f O2Flow %.3f H2FCM-m %.2f T %.1f p %.1f", 
		*massH2Tank2, *vapormassH2Tank2, *tempH2Tank2, *pressH2Tank2 * 0.000145038,
		*voltFC, *ampFC, *tempFC, *h2flowFC * 7.93665, *o2flowFC * 7.93665,
		*massH2FC1M, *tempH2FC1M, *pressH2FC1M * 0.000145038);
*/

/*	fprintf(PanelsdkLogFile, "%f H2T1-m %.2f T %.1f p %.1f H2T2-m %.2f T %.1f p %.1f H2FCM-m %.2f T %.1f p %.1f O2T1-m %.2f T %.2f p %.2f O2T2-m %.2f T %.2f p %.2f O2SM-m %.2f T %.2f p %5.2f O2MAIN-m %.2f T %.2f p %5.2f Cabin-m %.2f T %.2f p %.2f CO2 PP %.2f Co2Rate %f Rad-T %.2f\n", 
		simt, 
		*massH2Tank1 / 1000.0, *tempH2Tank1, *pressH2Tank1 * 0.000145038,
		*massH2Tank2 / 1000.0, *tempH2Tank2, *pressH2Tank2 * 0.000145038,
		*massH2FC1M,           *tempH2FC1M,  *pressH2FC1M  * 0.000145038,
		*massO2Tank1 / 1000.0, *tempO2Tank1, *pressO2Tank1 * 0.000145038,
		*massO2Tank2 / 1000.0, *tempO2Tank2, *pressO2Tank2 * 0.000145038,
		*massO2SMSupply / 1000.0, *tempO2SMSupply, *pressO2SMSupply * 0.000145038,
		*massO2MainReg / 1000.0, *tempO2MainReg, *pressO2MainReg * 0.000145038,
		*massCabin, *tempCabin, *pressCabin * 0.000145038, *pressCabinCO2 * 0.00750064, *co2removalrate,
		*tempRad);
	fflush(PanelsdkLogFile);
*/

#endif
}

void Saturn::SystemsInternalTimestep(double simdt) 

{
	TRACESETUP("Saturn::SystemsInternalTimestep");

	double mintFactor = __max(simdt / 100.0, 0.5);
	double tFactor = __min(mintFactor, simdt);
	while (simdt > 0) {

		// Each timestep is passed to the SPSDK
		// to perform internal computations on the 
		// systems.

		Panelsdk.SimpleTimestep(tFactor);

		//
		// Do all updates after the SDK has updated, so that power use
		// will feed back to it.
		//

		fdaiLeft.SystemTimestep(tFactor);
		fdaiRight.SystemTimestep(tFactor);
		agc.SystemTimestep(tFactor);
		dsky.SystemTimestep(tFactor);
		cws.SystemTimestep(tFactor);
		dockingprobe.SystemTimestep(tFactor);
		imu.SystemTimestep(tFactor);
		bmag1.SystemTimestep(tFactor);
		bmag2.SystemTimestep(tFactor);
		gdc.SystemTimestep(tFactor);
		eca.SystemTimestep(tFactor);
		rjec.SystemTimestep(tFactor);
		tvsa.SystemTimestep(tFactor);
		optics.SystemTimestep(tFactor);
		pcm.SystemTimestep(tFactor);
		pmp.SystemTimestep(tFactor);
		usb.SystemTimestep(tFactor);
		hga.SystemTimestep(tFactor);
		vhfranging.SystemTimestep(tFactor);
		RRTsystem.SystemTimestep(tFactor);
		sce.SystemTimestep();
		ems.SystemTimestep(tFactor);
		els.SystemTimestep(tFactor);
		ordeal.SystemTimestep(tFactor);
		SPSPropellant.SystemTimestep(tFactor);
		SPSEngine.SystemTimestep(tFactor);
		CabinPressureRegulator.SystemTimestep(tFactor);
		O2DemandRegulator.SystemTimestep(tFactor);
		CabinPressureReliefValve1.SystemTimestep(tFactor);
		CabinPressureReliefValve2.SystemTimestep(tFactor);
		SuitCircuitReturnValve.SystemTimestep(tFactor);
		O2SMSupply.SystemTimestep(tFactor);
		WaterController.SystemTimestep(tFactor);
		GlycolCoolingController.SystemTimestep(tFactor);
		LMTunnelVent.SystemTimestep(tFactor);
		PressureEqualizationValve.SystemTimestep(tFactor);
		CabinFansSystemTimestep();
		MissionTimerDisplay.SystemTimestep(tFactor);
		MissionTimer306Display.SystemTimestep(tFactor);
		EventTimerDisplay.SystemTimestep(tFactor);
		EventTimer306Display.SystemTimestep(tFactor);

		simdt -= tFactor;
		tFactor = __min(mintFactor, simdt);
		TRACE("Internal timestep done");
	}

	//Fuel Cell Reactant Heating  TBD heaters and regulators to feed reactant

	//FuelCellO2Manifold[0]->BoilAllAndSetTemp(315);		//Needs to be done using heat exchanger, heated to above 100F
	//FuelCellO2Manifold[1]->BoilAllAndSetTemp(315);		//Needs to be done using heat exchanger, heated to above 100F
	//FuelCellO2Manifold[2]->BoilAllAndSetTemp(315);		//Needs to be done using heat exchanger, heated to above 100F
	
	//FuelCellH2Manifold[0]->BoilAllAndSetTemp(315);		//Needs to be done using heat exchanger, heated to above 100F
	//FuelCellH2Manifold[1]->BoilAllAndSetTemp(315);		//Needs to be done using heat exchanger, heated to above 100F
	//FuelCellH2Manifold[2]->BoilAllAndSetTemp(315);		//Needs to be done using heat exchanger, heated to above 100F
}

void Saturn::JoystickTimestep()

{
	// Read joysticks and feed data to the computer
	// Do not do this if we aren't the active vessel.
	if (oapiGetFocusInterface() == this) {

		// Invert joystick configuration according to navmode in case of one joystick
		int tmp_id, tmp_rot_id, tmp_sld_id, tmp_rzx_id, tmp_pov_id, tmp_debug;
		if (rhc_thctoggle && ((rhc_id != -1 && thc_id == -1 && GetAttitudeMode() == RCS_LIN) ||
			                  (rhc_id == -1 && thc_id != -1 && GetAttitudeMode() == RCS_ROT))) {

			tmp_id = rhc_id;
			tmp_rot_id = rhc_rot_id;
			tmp_sld_id = rhc_sld_id;
			tmp_rzx_id = rhc_rzx_id;
			tmp_pov_id = rhc_pov_id;
			tmp_debug = rhc_debug;

			rhc_id = thc_id;
			rhc_rot_id = thc_rot_id;
			rhc_sld_id = thc_sld_id;
			rhc_rzx_id = thc_rzx_id;
			rhc_pov_id = thc_pov_id;
			rhc_debug = thc_debug;

			thc_id = tmp_id;
			thc_rot_id = tmp_rot_id;
			thc_sld_id = tmp_sld_id;
			thc_rzx_id = tmp_rzx_id;
			thc_pov_id = tmp_pov_id;
			thc_debug = tmp_debug;
		}

		// Issue warnings for bad configuration
		if (thc_id != -1 && !(thc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as THC does not exist.");
		}
		if (rhc_id != -1 && !(rhc_id < js_enabled)){
			sprintf(oapiDebugString(),"DX8JS: Joystick selected as RHC does not exist.");
		}

		HRESULT hr;
		ChannelValue val31;
		e_object *direct_power1, *direct_power2;
		val31 = agc.GetInputChannel(031); // Get current data
		// Mask off joystick bits
		val31  = val31.to_ulong() & 070000;

		// We'll do this with a RHC first. 						
		double rhc_voltage1 = 0, rhc_voltage2 = 0;
		double rhc_acvoltage1 = 0, rhc_acvoltage2 = 0;
		double rhc_directv1 = 0, rhc_directv2 = 0;
		// Since we are feeding the AGC, the RHC NORMAL power must be on.
		// There's more than one RHC in the real ship, but ours is "both" - having the power on for either one will work.

		// DC Power
		switch(RotPowerNormal1Switch.GetState()) {
			case THREEPOSSWITCH_UP:       
				rhc_voltage1 = RHCNormalPower.Voltage();
				break;
		}
		switch(RotPowerNormal2Switch.GetState()){
			case THREEPOSSWITCH_UP:       
				rhc_voltage2 = RHCNormalPower.Voltage();
				break;
		}

		// AC Power		
		if (eca.IsAC1Powered()) {	// ECA needs to be powered (AOH Display & Controls)
			switch (RotPowerNormal1Switch.GetState()) {
				case THREEPOSSWITCH_UP:
				case THREEPOSSWITCH_DOWN:
					rhc_acvoltage1 = StabContSystemAc1CircuitBraker.Voltage();
					break;
			}
		}
		if (eca.IsAC2Powered()) {
			switch(RotPowerNormal2Switch.GetState()){
				case THREEPOSSWITCH_UP:       
				case THREEPOSSWITCH_DOWN:     
					rhc_acvoltage2 = ECATVCAc2CircuitBraker.Voltage();
					break;
			}
		}

		// Direct power
		switch(RotPowerDirect1Switch.GetState()){
			case THREEPOSSWITCH_UP:       // MNA/MNB
				rhc_directv1 = RHCDirect1Power.Voltage();
				direct_power1 = &RHCDirect1Power;
				break;
			case THREEPOSSWITCH_DOWN:     // MNA
				rhc_directv1 = ContrDirectMnA1CircuitBraker.Voltage();
				direct_power1 = &ContrDirectMnA1CircuitBraker;
				break;
		}
		switch(RotPowerDirect2Switch.GetState()){
			case THREEPOSSWITCH_UP:       // MNA/MNB
				rhc_directv2 = RHCDirect2Power.Voltage();
				direct_power2 = &RHCDirect2Power;
				break;
			case THREEPOSSWITCH_DOWN:     // MNB
				rhc_directv2 = ContrDirectMnB2CircuitBraker.Voltage();
				direct_power2 = &ContrDirectMnB2CircuitBraker;
				break;
		}

		// Initialize to centered
		int rhc_x_pos = 32768; 
		int rhc_y_pos = 32768; 
		int rhc_rot_pos = 32768; 

		if (rhc_id != -1 && rhc_id < js_enabled) {	
			hr = dx8_joystick[rhc_id]->Poll();
			if (FAILED(hr)) { // Did that work?
				// Attempt to acquire the device
				hr = dx8_joystick[rhc_id]->Acquire();
				if (FAILED(hr)) {
					sprintf(oapiDebugString(),"DX8JS: Cannot aquire RHC");
				} else {
					hr = dx8_joystick[rhc_id]->Poll();
				}
			}		
			// Read data
			dx8_joystick[rhc_id]->GetDeviceState(sizeof(dx8_jstate[rhc_id]), &dx8_jstate[rhc_id]);
			rhc_x_pos = dx8_jstate[rhc_id].lX;
			rhc_y_pos = dx8_jstate[rhc_id].lY;

			// Z-axis read.
			if (rhc_rzx_id != -1 && rhc_rot_id == -1) { // Native Z-axis first unless rot is also set
				rhc_rot_pos = dx8_jstate[rhc_id].lZ;
			} else if (rhc_rot_id != -1) { // Then if this is a rotator-type axis
				switch(rhc_rot_id) {
					case 0:
						rhc_rot_pos = dx8_jstate[rhc_id].lRx; break;
					case 1:
						rhc_rot_pos = dx8_jstate[rhc_id].lRy; break;
					case 2:
						rhc_rot_pos = dx8_jstate[rhc_id].lRz; break;
				}
			} else if (rhc_sld_id != -1) { // Finally if this is a slider
				rhc_rot_pos = dx8_jstate[rhc_id].rglSlider[rhc_sld_id];
			}

			// RCS mode toggle
			if (rhc_thctoggle && thc_id == -1 && rhc_thctoggle_id != -1) {
				if (dx8_jstate[rhc_id].rgbButtons[rhc_thctoggle_id]) {
					if (!rhc_thctoggle_pressed) {
						SetAttitudeMode(RCS_LIN);
					}
					rhc_thctoggle_pressed = true;
				} else {
					rhc_thctoggle_pressed = false;
				}
			}
		// Use Orbiter's attitude control as RHC
		} else {
			// Roll
			if (GetManualControlLevel(THGROUP_ATT_BANKLEFT) > 0) {
				rhc_x_pos = (int) ((1. - GetManualControlLevel(THGROUP_ATT_BANKLEFT)) * 32768.);
			} else if (GetManualControlLevel(THGROUP_ATT_BANKRIGHT) > 0) {
				rhc_x_pos = (int) (32768. + GetManualControlLevel(THGROUP_ATT_BANKRIGHT) * 32768.);
			}
			// Pitch
			if (GetManualControlLevel(THGROUP_ATT_PITCHDOWN) > 0) {
				rhc_y_pos = (int) ((1. - GetManualControlLevel(THGROUP_ATT_PITCHDOWN)) * 32768.);
			} else if (GetManualControlLevel(THGROUP_ATT_PITCHUP) > 0) {
				rhc_y_pos = (int) (32768. + GetManualControlLevel(THGROUP_ATT_PITCHUP) * 32768.);
			}
			// Yaw
			if (GetManualControlLevel(THGROUP_ATT_YAWLEFT) > 0) {
				rhc_rot_pos = (int) ((1. - GetManualControlLevel(THGROUP_ATT_YAWLEFT)) * 32768.);
			} else if (GetManualControlLevel(THGROUP_ATT_YAWRIGHT) > 0) {
				rhc_rot_pos = (int) (32768. + GetManualControlLevel(THGROUP_ATT_YAWRIGHT) * 32768.);
			}
		}

		int rhc_pos[3] = { rhc_x_pos, rhc_y_pos, rhc_rot_pos };

		rhc1.Timestep(rhc_pos, rhc_voltage1 > SP_MIN_DCVOLTAGE, eca.IsAC1Powered(), rhc_directv1 > SP_MIN_DCVOLTAGE, rhc_directv1 > SP_MIN_DCVOLTAGE);

		// X and Y are well-duh kinda things. X=0 for full-left, Y = 0 for full-down
		// Set bits according to joystick state. 32768 is center, so 16384 is the left half.
		// The real RHC had a 12 degree travel. Our joystick travels 32768 points to full deflection.
		// This means 2730 points per degree travel. The RHC breakout switches trigger at 1.5 degrees deflection and
		// stop at 11. So from 36863 to 62798, we trigger plus, and from 28673 to 2738 we trigger minus.
		// The last degree of travel is reserved for the DIRECT control switches.
		if (rhc_voltage1 > SP_MIN_DCVOLTAGE || rhc_voltage2 > SP_MIN_DCVOLTAGE) { // NORMAL
			// CMC
			if (rhc1.GetMinusRollBreakoutSwitch()) {
				val31[MinusRollManualRotation] = 1;
			}					
			if (rhc1.GetMinusPitchBreakoutSwitch()) {
				val31[MinusPitchManualRotation] = 1;
			}
			if (rhc1.GetPlusRollBreakoutSwitch()) {
				val31[PlusRollManualRotation] = 1;
			}
			if (rhc1.GetPlusPitchBreakoutSwitch()) {
				val31[PlusPitchManualRotation] = 1;
			}
			if (rhc1.GetMinusYawBreakoutSwitch()) {
				val31[MinusYawManualRotation] = 1;
			}
			if (rhc1.GetPlusYawBreakoutSwitch()) {
				val31[PlusYawManualRotation] = 1;
			}
		}

		//
		// DIRECT
		//

		int rflag = 0, pflag = 0, yflag = 0; // Direct Fire Untriggers

		// CM/SM transfer, either motor transfers all thruster, see AOH Figure 2.5-4
		bool sm_sep = false;
		if (secs.rcsc.GetCMTransferMotor1() || secs.rcsc.GetCMTransferMotor2()) sm_sep = true;

		if ((rhc_directv1 > SP_MIN_DCVOLTAGE || rhc_directv2 > SP_MIN_DCVOLTAGE)) {
			if (rhc_x_pos < 2738) {
				// MINUS ROLL
				if (!sm_sep) {						
					SetRCSState(RCS_SM_QUAD_A, 2, 1);
					SetRCSState(RCS_SM_QUAD_B, 2, 1); 
					SetRCSState(RCS_SM_QUAD_C, 2, 1);
					SetRCSState(RCS_SM_QUAD_D, 2, 1);
					SetRCSState(RCS_SM_QUAD_A, 1, 0);
					SetRCSState(RCS_SM_QUAD_B, 1, 0); 
					SetRCSState(RCS_SM_QUAD_C, 1, 0);
					SetRCSState(RCS_SM_QUAD_D, 1, 0); 

					// Disable CM thrusters
					SetCMRCSState(8,0); 
					SetCMRCSState(9,0);
					SetCMRCSState(11,0);
					SetCMRCSState(10,0);

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(200); // Four thrusters worth
					} else {
						direct_power2->DrawPower(200);
					}
				} else {
					SetCMRCSState(8,0); 
					SetCMRCSState(9,0);
					SetCMRCSState(11,1);
					SetCMRCSState(10,1);

					// Disable SM thrusters
					SetRCSState(RCS_SM_QUAD_A, 2, 0);
					SetRCSState(RCS_SM_QUAD_B, 2, 0); 
					SetRCSState(RCS_SM_QUAD_C, 2, 0);
					SetRCSState(RCS_SM_QUAD_D, 2, 0);
					SetRCSState(RCS_SM_QUAD_A, 1, 0);
					SetRCSState(RCS_SM_QUAD_B, 1, 0); 
					SetRCSState(RCS_SM_QUAD_C, 1, 0);
					SetRCSState(RCS_SM_QUAD_D, 1, 0); 

					if (rhc_directv1 > 12) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				}
				rjec.SetDirectRollActive(true); 
				rflag = 1;
			}
			if (rhc_x_pos > 62798) {
				// PLUS ROLL
				if (!sm_sep) {
					SetRCSState(RCS_SM_QUAD_A, 2, 0); 
					SetRCSState(RCS_SM_QUAD_B, 2, 0); 
					SetRCSState(RCS_SM_QUAD_C, 2, 0);
					SetRCSState(RCS_SM_QUAD_D, 2, 0);
					SetRCSState(RCS_SM_QUAD_A, 1, 1);
					SetRCSState(RCS_SM_QUAD_B, 1, 1); 
					SetRCSState(RCS_SM_QUAD_C, 1, 1);
					SetRCSState(RCS_SM_QUAD_D, 1, 1); 

					// Disable CM thrusters
					SetCMRCSState(8, 0); 
					SetCMRCSState(9, 0);
					SetCMRCSState(11, 0);
					SetCMRCSState(10, 0);
					
					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(200);
					} else {
						direct_power2->DrawPower(200);
					}
				} else {
					SetCMRCSState(8, 1); 
					SetCMRCSState(9, 1);
					SetCMRCSState(11, 0);
					SetCMRCSState(10, 0);

					// Disable SM thrusters
					SetRCSState(RCS_SM_QUAD_A, 2, 0); 
					SetRCSState(RCS_SM_QUAD_B, 2, 0); 
					SetRCSState(RCS_SM_QUAD_C, 2, 0);
					SetRCSState(RCS_SM_QUAD_D, 2, 0);
					SetRCSState(RCS_SM_QUAD_A, 1, 0);
					SetRCSState(RCS_SM_QUAD_B, 1, 0); 
					SetRCSState(RCS_SM_QUAD_C, 1, 0);
					SetRCSState(RCS_SM_QUAD_D, 1, 0); 

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				}
				rjec.SetDirectRollActive(true); 
				rflag = 1;
			}
			if (rhc_y_pos < 2738) {
				// MINUS PITCH
				if (!sm_sep) {
					SetRCSState(RCS_SM_QUAD_C, 4, 1);
					SetRCSState(RCS_SM_QUAD_A, 4, 1); 
					SetRCSState(RCS_SM_QUAD_C, 3, 0);
					SetRCSState(RCS_SM_QUAD_A, 3, 0); 
				
					// Disable CM thrusters
					SetCMRCSState(0, 0);
					SetCMRCSState(1, 0);
					SetCMRCSState(2, 0);
					SetCMRCSState(3, 0);

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				} else {
					SetCMRCSState(0, 0);
					SetCMRCSState(1, 0);
					SetCMRCSState(2, 1);
					SetCMRCSState(3, 1);
					
					// Disable SM thrusters
					SetRCSState(RCS_SM_QUAD_C, 4, 0);
					SetRCSState(RCS_SM_QUAD_A, 4, 0); 
					SetRCSState(RCS_SM_QUAD_C, 3, 0);
					SetRCSState(RCS_SM_QUAD_A, 3, 0); 

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				}
				rjec.SetDirectPitchActive(true); 
				pflag = 1;
			}
			if (rhc_y_pos > 62798) {
				// PLUS PITCH
				if (!sm_sep) {
					SetRCSState(RCS_SM_QUAD_C, 4, 0);
					SetRCSState(RCS_SM_QUAD_A, 4, 0); 
					SetRCSState(RCS_SM_QUAD_C, 3, 1);
					SetRCSState(RCS_SM_QUAD_A, 3, 1); 

					// Disable CM thrusters
					SetCMRCSState(0, 0);
					SetCMRCSState(1, 0);
					SetCMRCSState(2, 0);
					SetCMRCSState(3, 0);

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				} else {
					SetCMRCSState(0, 1);
					SetCMRCSState(1, 1);
					SetCMRCSState(2, 0);
					SetCMRCSState(3, 0);
					
					// Disable SM thrusters
					SetRCSState(RCS_SM_QUAD_C, 4, 0);
					SetRCSState(RCS_SM_QUAD_A, 4, 0); 
					SetRCSState(RCS_SM_QUAD_C, 3, 0);
					SetRCSState(RCS_SM_QUAD_A, 3, 0); 
					
					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				}
				rjec.SetDirectPitchActive(true); 
				pflag = 1;
			}
			if (rhc_rot_pos < 2738) {
				// MINUS YAW
				if (!sm_sep) {
					SetRCSState(RCS_SM_QUAD_B, 4, 1);
					SetRCSState(RCS_SM_QUAD_D, 4, 1); 
					SetRCSState(RCS_SM_QUAD_D, 3, 0);
					SetRCSState(RCS_SM_QUAD_B, 3, 0); 

					// Disable CM thrusters
					SetCMRCSState(4, 0);
					SetCMRCSState(5, 0);
					SetCMRCSState(6, 0);
					SetCMRCSState(7, 0);

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				} else {
					SetCMRCSState(4, 0);
					SetCMRCSState(5, 0);
					SetCMRCSState(6, 1);
					SetCMRCSState(7, 1);

					// Disable SM thrusters
					SetRCSState(RCS_SM_QUAD_B, 4, 0);
					SetRCSState(RCS_SM_QUAD_D, 4, 0); 
					SetRCSState(RCS_SM_QUAD_D, 3, 0);
					SetRCSState(RCS_SM_QUAD_B, 3, 0); 

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				}
				rjec.SetDirectYawActive(true);
				yflag = 1;
			}
			if (rhc_rot_pos > 62798) {
				// PLUS YAW
				if (!sm_sep) {
					SetRCSState(RCS_SM_QUAD_D, 3, 1);
					SetRCSState(RCS_SM_QUAD_B, 3, 1); 
					SetRCSState(RCS_SM_QUAD_B, 4, 0);
					SetRCSState(RCS_SM_QUAD_D, 4, 0); 
					
					// Disable CM thrusters
					SetCMRCSState(4, 0);
					SetCMRCSState(5, 0);
					SetCMRCSState(6, 0);
					SetCMRCSState(7, 0);

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				} else {
					SetCMRCSState(4, 1);
					SetCMRCSState(5, 1);
					SetCMRCSState(6, 0);
					SetCMRCSState(7, 0);
					
					// Disable SM thrusters
					SetRCSState(RCS_SM_QUAD_D, 3, 0);
					SetRCSState(RCS_SM_QUAD_B, 3, 0); 
					SetRCSState(RCS_SM_QUAD_B, 4, 0);
					SetRCSState(RCS_SM_QUAD_D, 4, 0); 

					if (rhc_directv1 > SP_MIN_DCVOLTAGE) {
						direct_power1->DrawPower(100);
					} else {
						direct_power2->DrawPower(100);
					}
				}
				rjec.SetDirectYawActive(true);
				yflag = 1;
			}
		}
		if (rjec.GetDirectRollActive() == true && rflag == 0) { // Turn off direct roll
			if (!sm_sep) {
				SetRCSState(RCS_SM_QUAD_A, 1, 0);
				SetRCSState(RCS_SM_QUAD_B, 1, 0); 
				SetRCSState(RCS_SM_QUAD_C, 1, 0);
				SetRCSState(RCS_SM_QUAD_D, 1, 0); 
				SetRCSState(RCS_SM_QUAD_A, 2, 0);
				SetRCSState(RCS_SM_QUAD_B, 2, 0); 
				SetRCSState(RCS_SM_QUAD_C, 2, 0);
				SetRCSState(RCS_SM_QUAD_D, 2, 0); 
			} else {
				SetCMRCSState(8,0);
				SetCMRCSState(9,0);
				SetCMRCSState(10,0);
				SetCMRCSState(11,0);
			}
			rjec.SetDirectRollActive(false);
		}
		if (rjec.GetDirectPitchActive() == true && pflag == 0) { // Turn off direct pitch
			if (!sm_sep) {
				SetRCSState(RCS_SM_QUAD_C, 3, 0);
				SetRCSState(RCS_SM_QUAD_A, 3, 0); 
				SetRCSState(RCS_SM_QUAD_C, 4, 0);
				SetRCSState(RCS_SM_QUAD_A, 4, 0); 
			} else {
				SetCMRCSState(0,0);
				SetCMRCSState(1,0);
				SetCMRCSState(2,0);
				SetCMRCSState(3,0);
			}
			rjec.SetDirectPitchActive(false);
		}
		if (rjec.GetDirectYawActive() == true && yflag == 0) { // Turn off direct yaw
			if (!sm_sep) {
				SetRCSState(RCS_SM_QUAD_D, 3, 0);
				SetRCSState(RCS_SM_QUAD_B, 3, 0); 
				SetRCSState(RCS_SM_QUAD_B, 4, 0);
				SetRCSState(RCS_SM_QUAD_D, 4, 0); 
			} else {
				SetCMRCSState(4,0);
				SetCMRCSState(5,0);
				SetCMRCSState(6,0);
				SetCMRCSState(7,0);
			}
			rjec.SetDirectYawActive(false);
		}
		
		if (rhc_debug != -1) { 
			sprintf(oapiDebugString(),"RHC: X/Y/Z = %d / %d / %d | rzx_id %d rot_id %d", rhc_x_pos, rhc_y_pos, rhc_rot_pos, rhc_rzx_id, rhc_rot_id); 
		}

		//
		// And now the THC...
		//

		double thc_voltage = 0; 
		switch (TransContrSwitch.GetState()) {
			case TOGGLESWITCH_UP: // The THC is powered from MNA or MNB automatically.
				thc_voltage = ContrAutoMnACircuitBraker.Voltage();
				if (thc_voltage < SP_MIN_DCVOLTAGE) {
					thc_voltage = ContrAutoMnBCircuitBraker.Voltage();
				}
				break;
			case TOGGLESWITCH_DOWN:
				break;			
		}

		// Initialize to centered
		int thc_x_pos = 32768; 
		int thc_y_pos = 32768; 
		int thc_rot_pos = 32768; 

		if (thc_id != -1 && thc_id < js_enabled){
			hr = dx8_joystick[thc_id]->Poll();
			if (FAILED(hr)) { // Did that work?
				// Attempt to acquire the device
				hr = dx8_joystick[thc_id]->Acquire();
				if (FAILED(hr)) {
					sprintf(oapiDebugString(),"DX8JS: Cannot aquire THC");
				} else {
					hr = dx8_joystick[thc_id]->Poll();
				}
			}		
			// Read data
			dx8_joystick[thc_id]->GetDeviceState(sizeof(dx8_jstate[thc_id]), &dx8_jstate[thc_id]);
			thc_x_pos = dx8_jstate[thc_id].lX;
			thc_y_pos = dx8_jstate[thc_id].lY;

			// Z-axis read.
			if (thc_rzx_id != -1 && thc_rot_id == -1) { // Native Z-axis first unless rot is also set
				thc_rot_pos = dx8_jstate[thc_id].lZ;
			} else if (thc_rot_id != -1){ // Then if this is a rotator-type axis
				switch(thc_rot_id){
					case 0:
						thc_rot_pos = dx8_jstate[thc_id].lRx; break;
					case 1:
						thc_rot_pos = dx8_jstate[thc_id].lRy; break;
					case 2:
						thc_rot_pos = dx8_jstate[thc_id].lRz; break;
				}
			} else if(thc_sld_id != -1){ // Finally if this is a slider
				thc_rot_pos = dx8_jstate[thc_id].rglSlider[thc_sld_id];
			}

			if (thc_pov_id != -1) {
				DWORD dwPOV = dx8_jstate[thc_id].rgdwPOV[thc_pov_id];
				if (LOWORD(dwPOV) != 0xFFFF) {
					if (dwPOV > 31500 || dwPOV < 4500) {
						thc_rot_pos = 65536;
					} else if (dwPOV > 13500 && dwPOV < 21500) {
						thc_rot_pos = 0;
					}
				}
				//sprintf(oapiDebugString(),"THC: %d", dx8_jstate[thc_id].rgdwPOV[thc_pov_id]);
			}
			
			// RCS mode toggle
			if (rhc_thctoggle && rhc_id == -1 && rhc_thctoggle_id != -1) {
				if (dx8_jstate[thc_id].rgbButtons[rhc_thctoggle_id]) {
					if (!rhc_thctoggle_pressed) {
						SetAttitudeMode(RCS_ROT);
					}
					rhc_thctoggle_pressed = true;
				} else {
					rhc_thctoggle_pressed = false;
				}
			}

		// Use Orbiter's attitude control as THC
		} else {
			// Up/down
			if (GetManualControlLevel(THGROUP_ATT_DOWN) > 0) {
				thc_y_pos = (int) ((1. - GetManualControlLevel(THGROUP_ATT_DOWN)) * 32768.);
			} else if (GetManualControlLevel(THGROUP_ATT_UP) > 0) {
				thc_y_pos = (int) (32768. + GetManualControlLevel(THGROUP_ATT_UP) * 32768.);
			}
			// Left/right
			if (GetManualControlLevel(THGROUP_ATT_LEFT) > 0) {
				thc_x_pos = (int) ((1. - GetManualControlLevel(THGROUP_ATT_LEFT)) * 32768.);
			} else if (GetManualControlLevel(THGROUP_ATT_RIGHT) > 0) {
				thc_x_pos = (int) (32768. + GetManualControlLevel(THGROUP_ATT_RIGHT) * 32768.);
			}
			// Forward/Back
			if (GetManualControlLevel(THGROUP_ATT_BACK) > 0) {
				thc_rot_pos = (int) ((1. - GetManualControlLevel(THGROUP_ATT_BACK)) * 32768.);
			} else if (GetManualControlLevel(THGROUP_ATT_FORWARD) > 0) {
				thc_rot_pos = (int) (32768. + GetManualControlLevel(THGROUP_ATT_FORWARD) * 32768.);
			}
		}

		if (thc_voltage > SP_MIN_DCVOLTAGE) {
			if (SCContSwitch.IsUp() && !THCRotary.IsClockwise()) {	// CMC
				if (thc_x_pos < 16384) {							
					val31[MinusYTranslation] = 1;
				}
				if (thc_y_pos < 16384) {
					val31[PlusZTranslation] = 1;
				}
				if (thc_x_pos > 49152) {
					val31[PlusYTranslation] = 1;
				}
				if (thc_y_pos > 49152) {
					val31[MinusZTranslation] = 1;
				}
				if (thc_rot_pos < 16384) { 
					val31[MinusXTranslation] = 1;
				}
				if (thc_rot_pos > 49152) {
					val31[PlusXTranslation] = 1;
				}
			}
			if (thc_debug != -1) { 
				sprintf(oapiDebugString(),"THC: X/Y/Z = %d / %d / %d", thc_x_pos, thc_y_pos, thc_rot_pos);
			}

			// Update ECA
			eca.thc_x = thc_x_pos;
			eca.thc_y = thc_rot_pos;
			eca.thc_z = thc_y_pos;
		} else {
			// Power off
			eca.thc_x = 32768;
			eca.thc_y = 32768;
			eca.thc_z = 32768;
		}
		// Submit data to the AGC
		agc.SetInputChannel(031, val31);
	}

	//
	// IMPORTANT: The following must be the last SetRCSState calls within a time step, especially after the RJ/EC
	// The thrusters are turned off by the RJ/EC or the direct coil handling above.
	//

	//
	// Direct ullage thrust
	//

	if (DirectUllageButton.GetState() == 1) {
		if (DirectUllMnACircuitBraker.IsPowered()) {   
			SetRCSState(RCS_SM_QUAD_B, 4, true);
			SetRCSState(RCS_SM_QUAD_D, 3, true);
		}
		if (DirectUllMnBCircuitBraker.IsPowered()) {   
			SetRCSState(RCS_SM_QUAD_A, 4, true); 
			SetRCSState(RCS_SM_QUAD_C, 3, true);
		}
		if (SCSLogicBus1.Voltage() > SP_MIN_DCVOLTAGE)
		{
			rjec.SetDirectPitchActive(true);
			rjec.SetDirectYawActive(true);
		}
	}

	//SPS Abort Ullage
	if ((secs.MESCA.FireUllage() && RCSLogicMnACircuitBraker.IsPowered()) || (secs.MESCB.FireUllage() && RCSLogicMnBCircuitBraker.IsPowered()))
	{
		SetRCSState(RCS_SM_QUAD_B, 4, true);
		SetRCSState(RCS_SM_QUAD_D, 3, true);
		SetRCSState(RCS_SM_QUAD_A, 4, true);
		SetRCSState(RCS_SM_QUAD_C, 3, true);

		rjec.SetDirectPitchActive(true);
		rjec.SetDirectYawActive(true);
	}

	//SM Jettison Controller
	bool fire = false;
	if (secs.SMJCA && secs.SMJCA->GetFireMinusXTranslation())
		fire = true;

	if (secs.SMJCB && secs.SMJCB->GetFireMinusXTranslation())
		fire = true;

	if (fire)
	{
		SetRCSState(RCS_SM_QUAD_A, 3, true);
		SetRCSState(RCS_SM_QUAD_B, 3, true);
		SetRCSState(RCS_SM_QUAD_C, 4, true);
		SetRCSState(RCS_SM_QUAD_D, 4, true);
	}

	fire = false;

	if (secs.SMJCA && secs.SMJCA->GetFirePositiveRoll())
		fire = true;

	if (secs.SMJCB && secs.SMJCB->GetFirePositiveRoll())
		fire = true;

	if (fire)
	{
		SetRCSState(RCS_SM_QUAD_A, 1, true);
		SetRCSState(RCS_SM_QUAD_B, 1, true);
		SetRCSState(RCS_SM_QUAD_C, 1, true);
		SetRCSState(RCS_SM_QUAD_D, 1, true);
	}

	//
	// CM RCS propellant dump 
	//
	
	// Manual control
	if (secs.rcsc.GetInterconnectAndPropellantBurnRelayA() && secs.rcsc.GetPropellantDumpInhibitA() && CMRCSLogicSwitch.IsUp() && RCSLogicMnACircuitBraker.IsPowered()) {
		SetCMRCSState(2, true);	
		SetCMRCSState(4, true);	
		SetCMRCSState(7, true);	
		SetCMRCSState(8, true);	
		SetCMRCSState(11, true);	
	}
		
	// Manual control
	if (secs.rcsc.GetInterconnectAndPropellantBurnRelayB() && secs.rcsc.GetPropellantDumpInhibitB() && CMRCSLogicSwitch.IsUp() && RCSLogicMnBCircuitBraker.IsPowered()) {
		SetCMRCSState(3, true);	
		SetCMRCSState(5, true);	
		SetCMRCSState(6, true);	
		SetCMRCSState(9, true);	
		SetCMRCSState(10, true);	
	}
}

void Saturn::CabinFansSystemTimestep()

{
	// Both cabin fans blow through both heat exchangers,
	// so one fan is enough to turn them on both

	if (CabinFansActive()) {
		if (CabinFan1Active()) {
			ACBus1.DrawPower(19.3);
		}

		if (CabinFan2Active()) {
			ACBus2.DrawPower(19.3);
		}

		PrimCabinHeatExchanger->SetPumpAuto();
		SecCabinHeatExchanger->SetPumpAuto();
		CabinHeater->SetPumpAuto(); 

		CabinFanSound();
	} 
	else {
		PrimCabinHeatExchanger->SetPumpOff();
		SecCabinHeatExchanger->SetPumpOff();
		CabinHeater->SetPumpOff(); 

		StopCabinFanSound();
	}

	//
	// Suit Compressor sound
	//

	int vol = 0;
	if (SuitCompressor1->IsOn()) vol += 32;
	if (SuitCompressor2->IsOn()) vol += 32;

	if (vol > 0)
		SuitCompressorSound.play(vol + 191);
	else
		SuitCompressorSound.stop();
}

void Saturn::CheckSaturnSystemsState()
{
	if (stage >= CSM_LEM_STAGE)
	{
		if (sivb)
		{
			delete sivb;
			sivb = 0;
		}
		if (iu && !DontDeleteIU)
		{
			delete iu;
			iu = 0;
		}
	}
}

void Saturn::CheckSMSystemsState()

{
	//
	// Disconnect SM devices after CM/SM separation
	//
	
	if (stage >= CM_STAGE) {

		// Disconnect SM RCS electrical devices
		SMRCSHelium1ASwitch.WireTo(NULL);
		SMRCSHelium1BSwitch.WireTo(NULL);
		SMRCSHelium1CSwitch.WireTo(NULL);
		SMRCSHelium1DSwitch.WireTo(NULL);

		SMRCSHelium2ASwitch.WireTo(NULL);
		SMRCSHelium2BSwitch.WireTo(NULL);
		SMRCSHelium2CSwitch.WireTo(NULL);
		SMRCSHelium2DSwitch.WireTo(NULL);

		SMRCSProp1ASwitch.WireTo(NULL);
		SMRCSProp1BSwitch.WireTo(NULL);
		SMRCSProp1CSwitch.WireTo(NULL);
		SMRCSProp1DSwitch.WireTo(NULL);

		SMRCSProp2ASwitch.WireTo(NULL);
		SMRCSProp2BSwitch.WireTo(NULL);
		SMRCSProp2CSwitch.WireTo(NULL);
		SMRCSProp2DSwitch.WireTo(NULL);

		SMRCSHelium1ATalkback.WireTo(NULL);
		SMRCSHelium1BTalkback.WireTo(NULL);
		SMRCSHelium1CTalkback.WireTo(NULL);
		SMRCSHelium1DTalkback.WireTo(NULL);

		SMRCSHelium2ATalkback.WireTo(NULL);
		SMRCSHelium2BTalkback.WireTo(NULL);
		SMRCSHelium2CTalkback.WireTo(NULL);
		SMRCSHelium2DTalkback.WireTo(NULL);

		SMRCSProp1ATalkback.WireTo(NULL);
		SMRCSProp1BTalkback.WireTo(NULL);
		SMRCSProp1CTalkback.WireTo(NULL);
		SMRCSProp1DTalkback.WireTo(NULL);

		SMRCSProp2ATalkback.WireTo(NULL);
		SMRCSProp2BTalkback.WireTo(NULL);
		SMRCSProp2CTalkback.WireTo(NULL);
		SMRCSProp2DTalkback.WireTo(NULL);
		
		SMRCSHeaterASwitch.WireTo(NULL);
		SMRCSHeaterBSwitch.WireTo(NULL);
		SMRCSHeaterCSwitch.WireTo(NULL);
		SMRCSHeaterDSwitch.WireTo(NULL);
	
		// Disable the fuel cells so they appear to have been disconnected.
		int i;
		for (i = 0; i < 3; i++) {
			if (FuelCells[i])
				FuelCells[i]->Disable();			
		}

		FuelCellCooling[0]->WireTo(NULL);
		FuelCellCooling[1]->WireTo(NULL);
		FuelCellCooling[2]->WireTo(NULL);

		FuelCellHeaters[0]->WireTo(NULL);
		FuelCellHeaters[1]->WireTo(NULL);
		FuelCellHeaters[2]->WireTo(NULL);

		// Cryo heaters/fans
		O2TanksHeaters[0]->WireTo(NULL);
		O2TanksHeaters[1]->WireTo(NULL);
		H2TanksHeaters[0]->WireTo(NULL);
		H2TanksHeaters[1]->WireTo(NULL);
		O2TanksFans[0]->WireTo(NULL);
		O2TanksFans[1]->WireTo(NULL);
		H2TanksFans[0]->WireTo(NULL);
		H2TanksFans[1]->WireTo(NULL);
		
		// SPS
		SPSPropellantLineHeaterA->WireTo(NULL);
		SPSPropellantLineHeaterB->WireTo(NULL);

		HeValveMnACircuitBraker.WireTo(NULL);
		HeValveMnBCircuitBraker.WireTo(NULL);

		// ECS Coolant loops are disconnected, we set them to zero length, which disables them
		PrimEcsRadiatorExchanger1->SetLength(0);
		PrimEcsRadiatorExchanger2->SetLength(0);
		SecEcsRadiatorExchanger1->SetLength(0);
		SecEcsRadiatorExchanger2->SetLength(0);

		// Close O2 SM supply
		O2SMSupply.Close();

		//SM sensors
		H2Tank1TempSensor.WireTo(NULL);
		H2Tank2TempSensor.WireTo(NULL);
		O2Tank1TempSensor.WireTo(NULL);
		O2Tank2TempSensor.WireTo(NULL);
		PriRadInTempSensor.WireTo(NULL);
		SecRadInTempSensor.WireTo(NULL);
		SecRadOutTempSensor.WireTo(NULL);

		if (secs.SMJCA)
		{
			delete secs.SMJCA;
			secs.SMJCA = NULL;
		}
		if (secs.SMJCB)
		{
			delete secs.SMJCB;
			secs.SMJCB = NULL;
		}
	}
}

void Saturn::CreateMissionSpecificSystems()
{
	if (pMission->IsJMission())
	{
		pgPanels100.AddPanel(Panel181 = new SaturnPanel181, &PSH);

		Panel181->SMSector1LogicPowerMNABraker.WireTo(MainBusA);
		Panel181->SMSector1LogicPowerMNBBraker.WireTo(MainBusB);
		Panel181->SMSector1AC2ASystemBraker.WireTo(&ACBus2PhaseA);
		Panel181->SMSector1AC2BSystemBraker.WireTo(&ACBus2PhaseB);
		Panel181->SMSector1AC2CSystemBraker.WireTo(&ACBus2PhaseC);

		secs.InitSIMJett(&Panel181->SMSector1LogicPowerMNABraker, &Panel181->SMSector1LogicPowerMNBBraker);
	}

	if (pMission->GetPanel277Version() == 1)
	{
		pgPanels200.AddPanel(Panel277 = new SaturnPanel277, &PSH);
	}

	if (pMission->GetPanel278Version() == 2 || pMission->GetPanel278Version() == 3)
	{
		pgPanels200.AddPanel(Panel278J = new SaturnPanel278J, &PSH);
	}
	if (stage < CM_STAGE)
	{
		if (pMission->GetSMJCVersion() == 1)
		{
			secs.SMJCA = new SMJC();
			secs.SMJCB = new SMJC();
		}
		else
		{
			secs.SMJCA = new SMJC_MOD1();
			secs.SMJCB = new SMJC_MOD1();
		}
	}
}

bool Saturn::CabinFansActive()

{
	return CabinFan1Active() || CabinFan2Active();
}


bool Saturn::CabinFan1Active()

{
	//
	// For now, if any power breaker is enabled, then run the fans.
	//

	bool PowerFan1 = (ECSCabinFanAC1ACircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) || (ECSCabinFanAC1BCircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) || (ECSCabinFanAC1CCircuitBraker.Voltage() > SP_MIN_ACVOLTAGE);

	return (CabinFan1Switch.IsUp() && PowerFan1);
}

bool Saturn::CabinFan2Active()

{
	//
	// For now, if any power breaker is enabled, then run the fans.
	//

	bool PowerFan2 = (ECSCabinFanAC2ACircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) || (ECSCabinFanAC2BCircuitBraker.Voltage() > SP_MIN_ACVOLTAGE) || (ECSCabinFanAC2CCircuitBraker.Voltage() > SP_MIN_ACVOLTAGE);

	return (CabinFan2Switch.IsUp() && PowerFan2);
}

//
// Forcibly activate the SIVB RCS for unmanned control.
//

void Saturn::ActivateS4RCS()

{
}

void Saturn::DeactivateS4RCS()

{
}

//
// And CSM
//

void Saturn::ActivateCSMRCS()

{
	/// \todo This is now handled by automatic checklists and - as in reality - activated throughout the stage. 
	/// To restore the old functionality, a special flag would need to be introduced in SMRCSPropellantSource.

}

void Saturn::DeactivateCSMRCS()

{
	/// \todo This is now handled by automatic checklists and - as in reality - activated throughout the stage. 
	/// To restore the old functionality, a special flag would need to be introduced in SMRCSPropellantSource.
}

//
// And CM
//

void Saturn::ActivateCMRCS()

{
	/// \todo This is now handled by automatic checklists and - as in reality - activated throughout the stage. 
	/// To restore the old functionality, a special flag would need to be introduced in CMRCSPropellantSource.
}

void Saturn::DeactivateCMRCS()	

{
	/// \todo This is now handled by automatic checklists and - as in reality - activated throughout the stage. 
	/// To restore the old functionality, a special flag would need to be introduced in CMRCSPropellantSource.
}

void Saturn::SetEngineIndicators()

{
	for (int i = 1; i <= 8; i++) 
	{
		SetEngineIndicator(i);
	}
}

void Saturn::ClearEngineIndicators()

{
	for (int i = 1; i <= 8; i++) 
	{
		ClearEngineIndicator(i);
	}
}

void Saturn::SetLVGuidLight()

{
	LVGuidLight = true;
}

void Saturn::ClearLVGuidLight()

{
	LVGuidLight = false;
}

void Saturn::SetLVRateLight()

{
	LVRateLight = true;
}

void Saturn::ClearLVRateLight()

{
	LVRateLight = false;
}

bool Saturn::GetEngineIndicator(int i)

{
	if (i < 1 || i > 8)
		return false;

	return ENGIND[i - 1];
}

void Saturn::SetEngineIndicator(int i)

{
	if (i < 1 || i > 8)
		return;

	ENGIND[i - 1] = true;
}

void Saturn::ClearEngineIndicator(int i)

{
	if (i < 1 || i > 8)
		return;

	ENGIND[i - 1] = false;
}

void Saturn::FuelCellCoolingBypass(int fuelcell, bool bypassed) {

	// Bypass Radiator 2 and 4
	char buffer[100];

	sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:2:BYPASSED", fuelcell);
	bool *bp = (bool *) Panelsdk.GetPointerByString(buffer);
	*bp = bypassed;

	sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:4:BYPASSED", fuelcell);
	bp = (bool *) Panelsdk.GetPointerByString(buffer);
	*bp = bypassed;
}

bool Saturn::FuelCellCoolingBypassed(int fuelcell) {

	// It's bypassed when Radiator 2 is bypassed
	char buffer[100];

	sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:2:BYPASSED", fuelcell);
	bool *bypassed = (bool *) Panelsdk.GetPointerByString(buffer);
	return *bypassed;
}

//
// Set all the saved Panel SDK pointers to 0.
//

void Saturn::ClearPanelSDKPointers()

{
	int	i;

	pCO2Level = 0;
	pCabinCO2Level = 0;
	pCabinPressure = 0;
	pSuitPressure = 0;
	pSuitReturnPressure = 0;
	pO2RepressPressure = 0;
	pCabinTemp = 0;
	pSuitTemp = 0;
	pCabinRegulatorFlow = 0;
	pO2DemandFlow = 0;
	pDirectO2Flow = 0;
	pSuitTestFlow = 0;
	pCabinRepressFlow = 0;
	pEmergencyCabinRegulatorFlow = 0;
	pO2FlowXducer = 0;
	pO2Tank1Press = 0;
	pO2Tank2Press = 0;
	pH2Tank1Press = 0;
	pH2Tank2Press = 0;
	pO2Tank1Quantity = 0;
	pO2Tank2Quantity = 0;
	pH2Tank1Quantity = 0;
	pH2Tank2Quantity = 0;
	pO2SurgeTankPress = 0;
	for (i = 0; i <= 3; i++) {
		pFCH2Flow[i] = 0;
		pFCO2Flow[i] = 0;
		pFCTemp[i] = 0;
		pFCCondenserTemp[i] = 0;
		pFCCoolingTemp[i] = 0;
	}
	pPrimECSRadiatorInletPressure = 0;
	pPrimECSRadiatorInletTemp = 0;
	pPrimECSRadiatorOutletTemp = 0;
	pPrimECSEvaporatorOutletTemp = 0;
	pPrimECSEvaporatorSteamPressure = 0;
	pPrimECSAccumulatorQuantity = 0;
	pSecECSRadiatorInletPressure = 0;
	pSecECSRadiatorInletTemp = 0;
	pSecECSRadiatorOutletTemp = 0;
	pSecECSEvaporatorOutletTemp = 0;
	pSecECSEvaporatorSteamPressure = 0;
	pSecECSAccumulatorQuantity = 0;
	pPotableH2oTankQuantity = 0;
	pWasteH2oTankQuantity = 0;
	pCSMTunnelPressure = 0;
}

//
// Functions that provide structured access to Saturn systems state.
//

//
// Get SPS status
//
void Saturn::GetSPSStatus( SPSStatus &ss )
{
	ss.chamberPressurePSI = SPSEngine.GetChamberPressurePSI();
	ss.PropellantLineTempF = SPSPropellant.GetPropellantLineTempF();
	ss.OxidizerLineTempF = SPSPropellant.GetPropellantLineTempF();
}

//
// Get atmosphere status for CM.
//

void Saturn::GetAtmosStatus(AtmosStatus &atm)

{
	atm.CabinCO2MMHG = 0.0;
	atm.SuitCO2MMHG = 0.0;
	atm.CabinPressureMMHG = 0.0;
	atm.CabinPressurePSI = 0.0;
	atm.CabinTempK = 0.0;
	atm.CabinTempF = 0.0;
	atm.SuitTempK = 0.0;
	atm.SuitTempF = 0.0;
	atm.SuitPressureMMHG = 0.0;
	atm.SuitPressurePSI = 0;
	atm.CabinRegulatorFlowLBH = 0.0;
	atm.DirectO2FlowLBH = 0.0;
	atm.SuitTestFlowLBH = 0.0;
	atm.O2DemandFlowLBH = 0.0;
	atm.SuitReturnPressureMMHG = 0.0;
	atm.SuitReturnPressurePSI = 0.0;
	atm.CabinRepressFlowLBH = 0.0;
	atm.EmergencyCabinRegulatorFlowLBH = 0.0;
	atm.O2RepressPressurePSI = 0.0;
	atm.TunnelPressurePSI = 0.0;

	if (!pCO2Level) {
		pCO2Level = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUIT:CO2_PPRESS");
	}
	if (pCO2Level) {
		atm.SuitCO2MMHG = (*pCO2Level) * MMHG;
	}

	if (!pCabinCO2Level) {
		pCabinCO2Level = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:CO2_PPRESS");
	}
	if (pCabinCO2Level) {
		atm.CabinCO2MMHG = (*pCO2Level) * MMHG;
	}

	if (!pCabinPressure) {
		pCabinPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:PRESS");
	}
	if (pCabinPressure) {
		atm.CabinPressureMMHG = (*pCabinPressure) * MMHG;
		atm.CabinPressurePSI = (*pCabinPressure) * PSI;
	}

	if (!pSuitPressure) {
		pSuitPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUIT:PRESS");
	}
	if (pSuitPressure) {
		atm.SuitPressureMMHG = (*pSuitPressure) * MMHG;
		atm.SuitPressurePSI = (*pSuitPressure) * PSI;
	}

	if (!pSuitReturnPressure) {
		pSuitReturnPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITRETURNVALVE:PRESS");
	}
	if (pSuitReturnPressure) {
		atm.SuitReturnPressureMMHG = (*pSuitReturnPressure) * MMHG;
		atm.SuitReturnPressurePSI = (*pSuitReturnPressure) * PSI;
	}

	if (!pO2RepressPressure) {
		pO2RepressPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2REPRESSPACKAGE:PRESS");
	}
	if (pO2RepressPressure) {
		atm.O2RepressPressurePSI = (*pO2RepressPressure) * PSI;
	}

	if (!pSuitTemp) {
		pSuitTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUIT:TEMP");
	}
	if (pSuitTemp) {
		atm.SuitTempK = (*pSuitTemp);
		atm.SuitTempF = KelvinToFahrenheit( atm.SuitTempK );
	}

	if (!pCabinTemp) {
		pCabinTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABIN:TEMP");
	}
	if (pCabinTemp) {
		atm.CabinTempK = (*pCabinTemp);
		atm.CabinTempF = KelvinToFahrenheit( atm.CabinTempK );
	}

	if (!pCabinRegulatorFlow) {
		pCabinRegulatorFlow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINPRESSUREREGULATOR:FLOW");
	}
	if (pCabinRegulatorFlow) {
		atm.CabinRegulatorFlowLBH = (*pCabinRegulatorFlow) * LBH;
	}

	if (!pO2DemandFlow) {
		pO2DemandFlow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2DEMANDREGULATOR:FLOW");
	}
	if (pO2DemandFlow) {
		atm.O2DemandFlowLBH = (*pO2DemandFlow) * LBH;
	}

	if (!pDirectO2Flow) {
		pDirectO2Flow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:DIRECTO2VALVE:FLOW");
	}
	if (pDirectO2Flow) {
		atm.DirectO2FlowLBH = (*pDirectO2Flow) * LBH;
	}

	if (!pSuitTestFlow) {
		pSuitTestFlow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SUITTESTVALVE:FLOW");
	}
	if (pSuitTestFlow) {
		atm.SuitTestFlowLBH = (*pSuitTestFlow) * LBH;
	}

	if (!pCabinRepressFlow) {
		pCabinRepressFlow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:CABINREPRESSVALVE:FLOW");
	}
	if (pCabinRepressFlow) {
		atm.CabinRepressFlowLBH = (*pCabinRepressFlow) * LBH;
	}

	if (!pEmergencyCabinRegulatorFlow) {
		pEmergencyCabinRegulatorFlow = (double*) Panelsdk.GetPointerByString("HYDRAULIC:EMERGENCYCABINPRESSUREREGULATOR:FLOW");
	}
	if (pEmergencyCabinRegulatorFlow) {
		atm.EmergencyCabinRegulatorFlowLBH = (*pEmergencyCabinRegulatorFlow) * LBH;
	}

	if (!pO2FlowXducer)
	{
		pO2FlowXducer = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2MAINREGULATOROUTLET:FLOW");
	}
	if (pO2FlowXducer) 
	{
		atm.O2FlowXducerLBH = (*pO2FlowXducer) * LBH;
	}

	if (!pCSMTunnelPressure)
	{
		pCSMTunnelPressure = (double*)Panelsdk.GetPointerByString("HYDRAULIC:CSMTUNNEL:PRESS");
	}
	if (pCSMTunnelPressure)
	{
		atm.TunnelPressurePSI = (*pCSMTunnelPressure)*PSI;
	}

	//double *o2supplypress = (double*)Panelsdk.GetPointerByString("HYDRAULIC:O2SUPPLYMANIFOLD:PRESS");
	//sprintf(oapiDebugString(), "CabinReg %lf O2Demand %lf Direct %lf SuitTest %lf CabRepress %lf Emerg %lf FlowXDR %lf SupplyPress %lf", atm.CabinRegulatorFlowLBH, atm.O2DemandFlowLBH, atm.DirectO2FlowLBH, atm.SuitTestFlowLBH, atm.CabinRepressFlowLBH, atm.EmergencyCabinRegulatorFlowLBH, atm.O2FlowXducerLBH, *o2supplypress*PSI);
}


//
// Get displayed atmosphere status for caution & warning system
//

void Saturn::GetDisplayedAtmosStatus(DisplayedAtmosStatus &atm) 

{
	atm.DisplayedO2FlowLBH = RightO2FlowMeter.GetDisplayValue();
	atm.DisplayedSuitComprDeltaPressurePSI = SuitComprDeltaPMeter.GetDisplayValue();
	atm.DisplayedEcsRadTempPrimOutletMeterTemperatureF = EcsRadTempPrimOutletMeter.GetDisplayValue();
}

void Saturn::GetRCSStatus(int index, RCSStatus &rs)
{
	rs.HeliumPressurePSI = 0.0;
	rs.HeliumTempF = 0.0;
	rs.PackageTempF = 0.0;
	rs.PropellantPressurePSI = 0.0;

	//
	// No tanks if we've seperated from the SM
	//
	if (( index >= RCS_SM_QUAD_A && index <= RCS_SM_QUAD_D ) 
		&& stage >= CM_STAGE) {
		return;
	}

	// SM tanks.
	if ( index >= RCS_SM_QUAD_A && index <= RCS_SM_QUAD_D )
	{
		SMRCSPropellantSource *pRcs = 0;
		switch ( index )
		{
		case RCS_SM_QUAD_A:
			pRcs = &SMQuadARCS;
			break;

		case RCS_SM_QUAD_B:
			pRcs = &SMQuadBRCS;
			break;

		case RCS_SM_QUAD_C:
			pRcs = &SMQuadCRCS;
			break;

		case RCS_SM_QUAD_D:
			pRcs = &SMQuadDRCS;
			break;
		}

		if ( pRcs )
		{
			rs.HeliumPressurePSI = pRcs->GetHeliumPressurePSI();
			rs.HeliumTempF = pRcs->GetHeliumTempF();
			rs.PackageTempF = pRcs->GetPackageTempF();
			rs.PropellantPressurePSI = pRcs->GetPropellantPressurePSI();
		}

		return;
	}

	CMRCSPropellantSource *pRcs = 0;
	switch ( index )
	{
	case RCS_CM_RING_1:
		pRcs = &CMRCS1;
		break;

	case RCS_CM_RING_2:
		pRcs = &CMRCS2;
		break;
	}

	if ( pRcs )
	{
		rs.HeliumPressurePSI = pRcs->GetHeliumPressurePSI();
		rs.HeliumTempF = pRcs->GetHeliumTempF();
		rs.PropellantPressurePSI = pRcs->GetPropellantPressurePSI();
	}
}

void Saturn::GetSECSStatus( SECSStatus &ss )
{
	ss.BusAVoltage = SECSLogicBusA.Voltage();
	ss.BusBVoltage = SECSLogicBusB.Voltage();
	ss.CMRCSPressureSignalA = secs.MESCA.CMRCSPressureSignal;
	ss.CMSMSepRelayCloseA = secs.MESCA.CMSMSepRelayClose;
	ss.EDSAbortLogicOutputA = secs.MESCA.EDSAbortLogicOutput;
	ss.RCSActivateSignalA = secs.MESCA.RCSActivateSignal;
	ss.SLASepRelayA = secs.MESCA.SLASepRelay;
	ss.FwdHeatshieldJettA = secs.MESCA.FwdHeatshieldJett;
	ss.CMRCSPressureSignalB = secs.MESCB.CMRCSPressureSignal;
	ss.CMSMSepRelayCloseB = secs.MESCB.CMSMSepRelayClose;
	ss.EDSAbortLogicOutputB = secs.MESCB.EDSAbortLogicOutput;
	ss.RCSActivateSignalB = secs.MESCB.RCSActivateSignal;
	ss.SLASepRelayB = secs.MESCB.SLASepRelay;
	ss.FwdHeatshieldJettB = secs.MESCB.FwdHeatshieldJett;
	ss.DrogueSepRelayA = els.ELSCA.GetDrogueParachuteDeployRelay() && els.pcvb.GetDrogueChuteDeployA();
	ss.DrogueSepRelayB = els.ELSCB.GetDrogueParachuteDeployRelay() && els.pcvb.GetDrogueChuteDeployB();
	ss.MainChuteDiscRelayA = els.pcvb.GetMainChuteReleaseA();
	ss.MainChuteDiscRelayB = els.pcvb.GetMainChuteReleaseB();
	ss.MainDeployRelayA = els.ELSCA.GetMainParachuteDeployRelay() && els.pcvb.GetMainChuteDeployA();
	ss.MainDeployRelayB = els.ELSCB.GetMainParachuteDeployRelay() && els.pcvb.GetMainChuteDeployB();
	ss.EDSAbortLogicInput1 = iuCommandConnector.GetEDSAbort(1);
	ss.EDSAbortLogicInput2 = iuCommandConnector.GetEDSAbort(2);
	ss.EDSAbortLogicInput3 = iuCommandConnector.GetEDSAbort(3);
	ss.CrewAbortA = secs.MESCA.CrewAbortSignal;
	ss.CrewAbortB = secs.MESCB.CrewAbortSignal;
	ss.CSMLEMLockRingSepRelaySignalA = secs.LDECA.CSM_LEM_LockRingSepRelaySignal;
	ss.CSMLEMLockRingSepRelaySignalB = secs.LDECB.CSM_LEM_LockRingSepRelaySignal;
}

void Saturn::GetPyroStatus( PyroStatus &ps )
{
	ps.BusAVoltage = PyroBusA.Voltage();
	ps.BusBVoltage = PyroBusB.Voltage();
}

//
// Get H2/O2 tank pressures.
//

void Saturn::GetTankPressures(TankPressures &press)

{
	//
	// Clear to defaults.
	//

	press.H2Tank1PressurePSI = 0.0;
	press.H2Tank2PressurePSI = 0.0;
	press.O2Tank1PressurePSI = 0.0;
	press.O2Tank2PressurePSI = 0.0;
	press.O2SurgeTankPressurePSI = 0.0;

	//
	// Oxygen surge tank.
	//

	if (!pO2SurgeTankPress) {
		pO2SurgeTankPress = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2SURGETANK:PRESS");
	}
	if (pO2SurgeTankPress) {
		press.O2SurgeTankPressurePSI = (*pO2SurgeTankPress) * PSI;
	}


	//
	// No tanks if we've seperated from the SM
	//

	if (stage >= CM_STAGE) {
		return;
	}

	//
	// Hydrogen tanks.
	//

	if (!pH2Tank1Press) {
		pH2Tank1Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:PRESS");
	}
	if (pH2Tank1Press) {
		press.H2Tank1PressurePSI = (*pH2Tank1Press) * PSI;
	}

	if (!pH2Tank2Press) {
		pH2Tank2Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:PRESS");
	}
	if (pH2Tank2Press) {
		press.H2Tank2PressurePSI = (*pH2Tank2Press) * PSI;
	}

	//
	// Oxygen tanks.
	//

	if (!pO2Tank1Press) {
		pO2Tank1Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:PRESS");
	}
	if (pO2Tank1Press) {
		press.O2Tank1PressurePSI = (*pO2Tank1Press) * PSI;
	}

	if (!pO2Tank2Press) {
		pO2Tank2Press = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:PRESS");
	}
	if (pO2Tank2Press) {
		press.O2Tank2PressurePSI = (*pO2Tank2Press) * PSI;
	}
}


//
// Get H2/O2 tank quantities.
//

void Saturn::GetTankQuantities(TankQuantities &q)

{
	//
	// Clear to defaults.
	//

	q.H2Tank1Quantity = 0.0;
	q.H2Tank2Quantity = 0.0;
	q.O2Tank1QuantityKg = 0.0;
	q.O2Tank2QuantityKg = 0.0;
	q.O2Tank1Quantity = 0.0;
	q.O2Tank2Quantity = 0.0;

	//
	// No tanks if we've seperated from the SM
	//

	if (stage >= CM_STAGE) {
		return;
	}

	//
	// Hydrogen tanks.
	//

	if (!pH2Tank1Quantity) {
		pH2Tank1Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK1:MASS");
	}
	if (pH2Tank1Quantity) {
		q.H2Tank1Quantity = (*pH2Tank1Quantity) / CSM_H2TANK_CAPACITY;
	}

	if (!pH2Tank2Quantity) {
		pH2Tank2Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:H2TANK2:MASS");
	}
	if (pH2Tank2Quantity) {
		q.H2Tank2Quantity = (*pH2Tank2Quantity) / CSM_H2TANK_CAPACITY;
	}

	//
	// Oxygen tanks.
	//

	if (!pO2Tank1Quantity) {
		pO2Tank1Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK1:MASS");
	}

	if (pO2Tank1Quantity) {
		q.O2Tank1Quantity = (*pO2Tank1Quantity) / CSM_O2TANK_CAPACITY;
		q.O2Tank1QuantityKg = (*pO2Tank1Quantity) / 1000.0;
	}

	if (!pO2Tank2Quantity) {
		pO2Tank2Quantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:O2TANK2:MASS");
	}
	if (pO2Tank2Quantity) {
		q.O2Tank2Quantity = (*pO2Tank2Quantity) / CSM_O2TANK_CAPACITY;
		q.O2Tank2QuantityKg = (*pO2Tank2Quantity) / 1000.0;
	}
}


//
// Get fuel cell status. 
//

void Saturn::GetFuelCellStatus(int index, FuelCellStatus &fc)

{
	//
	// Set defaults.
	//

	fc.H2PressurePSI = 0.0;
	fc.H2FlowLBH  = 0.0;
	fc.O2PressurePSI = 0.0;
	fc.O2FlowLBH = 0.0;
	fc.TempF = 0.0;
	fc.CondenserTempF = 0.0;
	fc.CoolingTempF = 0.0;
	fc.RadiatorTempInF = 0.0;
	fc.RadiatorTempOutF = 0.0;
	fc.Voltage = 0.0;
	fc.Current = 0.0;
	fc.PowerOutput = 0.0;

	//
	// No fuel cells if we've seperated from the SM.
	//

	if (stage >= CM_STAGE) {
		return;
	}

	FCell *f = FuelCells[index - 1];

	//
	// Fuel cell.
	//

	char buffer[1000];

	fc.H2FlowLBH = f->H2_flowPerSecond * LBH;
	if ( f->H2_SRC )
	{
		fc.H2PressurePSI = f->H2_SRC->GetPress() * PSI;
	}
	fc.O2FlowLBH = f->O2_flowPerSecond * LBH;
	if ( f->O2_SRC )
	{
		fc.O2PressurePSI = f->O2_SRC->GetPress() * PSI;
	}
	fc.TempF = KelvinToFahrenheit(f->Temp);
	fc.CondenserTempF = KelvinToFahrenheit(f->condenserTemp);

	if (!pFCCoolingTemp[index]) {
		sprintf(buffer, "ELECTRIC:FUELCELL%iCOOLING:TEMP", index);
		pFCCoolingTemp[index] = (double*) Panelsdk.GetPointerByString(buffer);
	}
	if (pFCCoolingTemp[index]) {
		fc.CoolingTempF = KelvinToFahrenheit(*pFCCoolingTemp[index]);
	}

	fc.Voltage = FuelCells[index - 1]->Voltage();
	fc.Current = FuelCells[index - 1]->Current();
	fc.PowerOutput = FuelCells[index - 1]->PowerLoad();

	// For now.
	fc.RadiatorTempInF = fc.CoolingTempF;
	fc.RadiatorTempOutF = fc.CoolingTempF;
}


//
// Get primary ECS water-glycol cooling status. 
//

void Saturn::GetPrimECSCoolingStatus(PrimECSCoolingStatus &pcs)

{
	//
	// Set defaults.
	//

	pcs.RadiatorInletPressurePSI = 0.0;
	pcs.RadiatorInletTempF = 0.0;
	pcs.RadiatorOutletTempF = 0.0;
	pcs.EvaporatorOutletTempF = 0.0;
	pcs.EvaporatorSteamPressurePSI = 0.0;

	//
	// Get values
	//

	if (!pPrimECSRadiatorInletPressure) {
		pPrimECSRadiatorInletPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:PRESS");
	}
	if (pPrimECSRadiatorInletPressure) {
		pcs.RadiatorInletPressurePSI = (*pPrimECSRadiatorInletPressure) * PSI;
	}

	if (!pPrimECSRadiatorInletTemp) {
		pPrimECSRadiatorInletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATORINLET:TEMP");
	}
	if (pPrimECSRadiatorInletTemp) {
		pcs.RadiatorInletTempF = KelvinToFahrenheit(*pPrimECSRadiatorInletTemp);
	}

	if (!pPrimECSRadiatorOutletTemp) {
		pPrimECSRadiatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMRADIATOROUTLET:TEMP");
	}
	if (pPrimECSRadiatorOutletTemp) {
		pcs.RadiatorOutletTempF = KelvinToFahrenheit(*pPrimECSRadiatorOutletTemp);
	}

	if (!pPrimECSEvaporatorOutletTemp) {
		pPrimECSEvaporatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOROUTLET:TEMP");
	}
	if (pPrimECSEvaporatorOutletTemp) {
		pcs.EvaporatorOutletTempF = KelvinToFahrenheit(*pPrimECSEvaporatorOutletTemp);
	}

	if (!pPrimECSEvaporatorSteamPressure) {
		pPrimECSEvaporatorSteamPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMEVAPORATOR:STEAMPRESSURE");
	}
	if (pPrimECSEvaporatorSteamPressure) {
		pcs.EvaporatorSteamPressurePSI = (*pPrimECSEvaporatorSteamPressure) * PSI;
	}

	if (!pPrimECSAccumulatorQuantity) {
		pPrimECSAccumulatorQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:PRIMGLYCOLACCUMULATOR:MASS");
	}
	if (pPrimECSAccumulatorQuantity) {
		pcs.AccumulatorQuantityPercent = (*pPrimECSAccumulatorQuantity) / 10000.0;
	}
}


//
// Get secondary ECS water-glycol cooling status. 
//

void Saturn::GetSecECSCoolingStatus(SecECSCoolingStatus &scs)

{
	//
	// Set defaults.
	//

	scs.RadiatorInletPressurePSI = 0.0;
	scs.RadiatorInletTempF = 0.0;
	scs.RadiatorOutletTempF = 0.0;
	scs.EvaporatorOutletTempF = 0.0;
	scs.EvaporatorSteamPressurePSI = 0.0;

	//
	// Get values
	//

	if (!pSecECSRadiatorInletPressure) {
		pSecECSRadiatorInletPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATORINLET:PRESS");
	}
	if (pSecECSRadiatorInletPressure) {
		scs.RadiatorInletPressurePSI = (*pSecECSRadiatorInletPressure) * PSI;
	}

	if (!pSecECSRadiatorInletTemp) {
		pSecECSRadiatorInletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATORINLET:TEMP");
	}
	if (pSecECSRadiatorInletTemp) {
		scs.RadiatorInletTempF = KelvinToFahrenheit(*pSecECSRadiatorInletTemp);
	}

	if (!pSecECSRadiatorOutletTemp) {
		pSecECSRadiatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECRADIATOROUTLET:TEMP");
	}
	if (pSecECSRadiatorOutletTemp) {
		scs.RadiatorOutletTempF = KelvinToFahrenheit(*pSecECSRadiatorOutletTemp);
	}

	if (!pSecECSEvaporatorOutletTemp) {
		pSecECSEvaporatorOutletTemp = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOROUTLET:TEMP");
	}
	if (pSecECSEvaporatorOutletTemp) {
		scs.EvaporatorOutletTempF = KelvinToFahrenheit(*pSecECSEvaporatorOutletTemp);
	}

	if (!pSecECSEvaporatorSteamPressure) {
		pSecECSEvaporatorSteamPressure = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECEVAPORATOR:STEAMPRESSURE");
	}
	if (pSecECSEvaporatorSteamPressure) {
		scs.EvaporatorSteamPressurePSI = (*pSecECSEvaporatorSteamPressure) * PSI;
	}

	if (!pSecECSAccumulatorQuantity) {
		pSecECSAccumulatorQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:SECGLYCOLACCUMULATOR:MASS");
	}
	if (pSecECSAccumulatorQuantity) {
		scs.AccumulatorQuantityPercent = (*pSecECSAccumulatorQuantity) / 10000.0;
	}
}


//
// Get ECS water status. 
//

void Saturn::GetECSWaterStatus(ECSWaterStatus &ws)

{
	//
	// Set defaults.
	//

	ws.PotableH2oTankQuantityPercent = 0;
	ws.WasteH2oTankQuantityPercent = 0;

	//
	// Get values
	//

	if (!pPotableH2oTankQuantity) {
		pPotableH2oTankQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:POTABLEH2OTANK:MASS");
	}
	if (pPotableH2oTankQuantity) {
		ws.PotableH2oTankQuantityPercent = (*pPotableH2oTankQuantity) / 16300.0;
	}

	if (!pWasteH2oTankQuantity) {
		pWasteH2oTankQuantity = (double*) Panelsdk.GetPointerByString("HYDRAULIC:WASTEH2OTANK:MASS");
	}
	if (pWasteH2oTankQuantity) {
		ws.WasteH2oTankQuantityPercent = (*pWasteH2oTankQuantity) / 25400.0;
	}
}

//
// ECS state for the ProjectApolloMFD.
//

void Saturn::GetECSStatus(ECSStatus &ecs)
 
{
	// Crew
	ecs.crewNumber = Crew->number;
	ecs.crewStatus = CrewStatus.GetStatus();

	// Primary coolant loop
	ecs.PrimECSHeating = - PrimCabinHeatExchanger->power - PrimSuitHeatExchanger->power - PrimSuitCircuitHeatExchanger->power;
	if (imu.GetHeater()->pumping) 
		ecs.PrimECSHeating += imu.GetHeater()->boiler_power;
	if (bmag1.GetHeater()->pumping)
		ecs.PrimECSHeating += bmag1.GetHeater()->boiler_power;
	if (bmag2.GetHeater()->pumping)
		ecs.PrimECSHeating += bmag2.GetHeater()->boiler_power;

	ecs.PrimECSTestHeating = 0;
	if (PrimECSTestHeater->pumping)
		ecs.PrimECSTestHeating += PrimECSTestHeater->boiler_power;

	// Secondary coolant loop
	ecs.SecECSHeating = - SecCabinHeatExchanger->power - SecSuitHeatExchanger->power - SecSuitCircuitHeatExchanger->power;

	ecs.SecECSTestHeating = 0;
	if (SecECSTestHeater->pumping)
		ecs.SecECSTestHeating += SecECSTestHeater->boiler_power;
}

void Saturn::SetCrewNumber(int number) {

	Crew->number = number;
	SetCrewMesh();
}

void Saturn::SetPrimECSTestHeaterPowerW(double power) {

	PrimECSTestHeater->boiler_power = power;
}

void Saturn::SetSecECSTestHeaterPowerW(double power) {

	SecECSTestHeater->boiler_power = power;
}

//
// Main bus state.
//

void Saturn::GetMainBusStatus(MainBusStatus &ms)

{
	ms.MainBusAVoltage = 0.0;
	ms.MainBusBVoltage = 0.0;
	
	ms.Enabled_DC_A_CWS = (MainBusAResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
	ms.Enabled_DC_B_CWS = (MainBusBResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
	ms.Reset_DC_A_CWS = MainBusAResetSwitch.IsUp();
	ms.Reset_DC_B_CWS = MainBusBResetSwitch.IsUp();
	
	if (MainBusA) {
		ms.MainBusAVoltage = MainBusA->Voltage();
	}

	if (MainBusB) {
		ms.MainBusBVoltage = MainBusB->Voltage();
	}

	ms.Fc_Disconnected = MainBusAController.IsFuelCellDisconnectAlarm() || 
		                 MainBusBController.IsFuelCellDisconnectAlarm();
}

//
// Battery bus status.
//

void Saturn::GetBatteryBusStatus( BatteryBusStatus &bs )
{
	bs.BatBusAVoltage = BatteryBusA.Voltage();
	bs.BatBusACurrent = BatteryBusA.Current();
	bs.BatBusBVoltage = BatteryBusB.Voltage();
	bs.BatBusBCurrent = BatteryBusB.Current();
	bs.BatteryRelayBusVoltage = BatteryRelayBus.Voltage();
}

//
// Battery status.
//

void Saturn::GetBatteryStatus( BatteryStatus &bs )
{
	bs.BatteryAVoltage = 0.0;
	bs.BatteryACurrent = 0.0;
	bs.BatteryBVoltage = 0.0;
	bs.BatteryBCurrent = 0.0;
	bs.BatteryCVoltage = 0.0;
	bs.BatteryCCurrent = 0.0;
	bs.BatteryChargerCurrent = BatteryCharger.Current();

	if ( EntryBatteryA ) 
	{
		bs.BatteryAVoltage = EntryBatteryA->Voltage();
		bs.BatteryACurrent = EntryBatteryA->Current();
	}
	
	if ( EntryBatteryB ) 
	{
		bs.BatteryBVoltage = EntryBatteryB->Voltage();
		bs.BatteryBCurrent = EntryBatteryB->Current();
	}
	
	if ( EntryBatteryC ) 
	{
		bs.BatteryCVoltage = EntryBatteryC->Voltage();
		bs.BatteryCCurrent = EntryBatteryC->Current();
	}
}

//
// AC bus state.
//

void Saturn::GetACBusStatus(ACBusStatus &as, int busno)

{
	as.ACBusCurrent = 0.0;
	as.ACBusVoltage = 0.0;
	as.Phase1Current = 0.0;
	as.Phase2Current = 0.0;
	as.Phase3Current = 0.0;
	as.Phase1Voltage = 0.0;
	as.Phase2Voltage = 0.0;
	as.Phase3Voltage = 0.0;
	as.Enabled_AC_CWS = true;
	as.Reset_AC_CWS = false;

	switch (busno) {
	case 1:
		as.ACBusCurrent = ACBus1.Current();
		as.ACBusVoltage = ACBus1.Voltage();
		as.Phase1Current = ACBus1PhaseA.Current();
		as.Phase2Current = ACBus1PhaseB.Current();
		as.Phase3Current = ACBus1PhaseC.Current();
		as.Phase1Voltage = ACBus1PhaseA.Voltage();
		as.Phase2Voltage = ACBus1PhaseB.Voltage();
		as.Phase3Voltage = ACBus1PhaseC.Voltage();
		as.Enabled_AC_CWS = (AcBus1ResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
		as.Reset_AC_CWS = AcBus1ResetSwitch.IsUp();
		break;

	case 2:
		as.ACBusCurrent = ACBus2.Current();
		as.ACBusVoltage = ACBus2.Voltage();
		as.Phase1Current = ACBus2PhaseA.Current();
		as.Phase2Current = ACBus2PhaseB.Current();
		as.Phase3Current = ACBus2PhaseC.Current();
		as.Phase1Voltage = ACBus2PhaseA.Voltage();
		as.Phase2Voltage = ACBus2PhaseB.Voltage();
		as.Phase3Voltage = ACBus2PhaseC.Voltage();
		as.Enabled_AC_CWS = (AcBus2ResetSwitch.Voltage() > SP_MIN_DCVOLTAGE);
		as.Reset_AC_CWS = AcBus2ResetSwitch.IsUp();
		break;
	}
}

void Saturn::EPSTimestep() {

	// FuelCell Bus Connect Switches

	if (FuelCell1BusContCB.IsPowered()) {
		if (MainBusASwitch1.IsUp() && !MainBusAController.IsFuelCellConnected(1)) {
			MainBusAController.ConnectFuelCell(1, true);
		}
		if (MainBusASwitch1.IsDown()) {
			MainBusAController.ConnectFuelCell(1, false);
		}
		if (MainBusBSwitch1.IsUp() && !MainBusBController.IsFuelCellConnected(1)) {
			MainBusBController.ConnectFuelCell(1, true);
		}
		if (MainBusBSwitch1.IsDown()) {
			MainBusBController.ConnectFuelCell(1, false);
		}
	}
	if (FuelCell2BusContCB.IsPowered()) {
		if (MainBusASwitch2.IsUp() && !MainBusAController.IsFuelCellConnected(2)) {
			MainBusAController.ConnectFuelCell(2, true);
		}
		if (MainBusASwitch2.IsDown()) {
			MainBusAController.ConnectFuelCell(2, false);
		}
		if (MainBusBSwitch2.IsUp() && !MainBusBController.IsFuelCellConnected(2)) {
			MainBusBController.ConnectFuelCell(2, true);
		}
		if (MainBusBSwitch2.IsDown()) {
			MainBusBController.ConnectFuelCell(2, false);
		}
	}
	if (FuelCell3BusContCB.IsPowered()) {
		if (MainBusASwitch3.IsUp() && !MainBusAController.IsFuelCellConnected(3)) {
			MainBusAController.ConnectFuelCell(3, true);
		}
		if (MainBusASwitch3.IsDown()) {
			MainBusAController.ConnectFuelCell(3, false);
		}
		if (MainBusBSwitch3.IsUp() && !MainBusBController.IsFuelCellConnected(3)) {
			MainBusBController.ConnectFuelCell(3, true);
		}
		if (MainBusBSwitch3.IsDown()) {
			MainBusBController.ConnectFuelCell(3, false);
		}
	}

	// FuelCell Purge Switches
	int *start = (int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL1:PURGE");	
	if (FuelCellPurge1Switch.IsDown() && FuelCell1PurgeCB.IsPowered()) {
		*start = SP_FUELCELL_O2PURGE;
	} else if (FuelCellPurge1Switch.IsUp() && FuelCell1PurgeCB.IsPowered() && H2PurgeLineSwitch.IsUp()) {
		*start = SP_FUELCELL_H2PURGE;
	} else {
		*start = SP_FUELCELL_NOPURGE;
	}

	start = (int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL2:PURGE");	
	if (FuelCellPurge2Switch.IsDown() && FuelCell2PurgeCB.IsPowered()) {
		*start = SP_FUELCELL_O2PURGE;
	} else if (FuelCellPurge2Switch.IsUp() && FuelCell2PurgeCB.IsPowered() && H2PurgeLineSwitch.IsUp()) {
		*start = SP_FUELCELL_H2PURGE;
	} else {
		*start = SP_FUELCELL_NOPURGE;
	}

	start = (int*) Panelsdk.GetPointerByString("ELECTRIC:FUELCELL3:PURGE");	
	if (FuelCellPurge3Switch.IsDown() && FuelCell3PurgeCB.IsPowered()) {
		*start = SP_FUELCELL_O2PURGE;
	} else if (FuelCellPurge3Switch.IsUp() && FuelCell3PurgeCB.IsPowered() && H2PurgeLineSwitch.IsUp()) {
		*start = SP_FUELCELL_H2PURGE;
	} else {
		*start = SP_FUELCELL_NOPURGE;
	}
}


void Saturn::DisconnectInverter(bool disc, int busno)

{
	if (disc) {
		if (busno == 1){
			ACBus1PhaseA.WireTo(NULL);
			ACBus1PhaseB.WireTo(NULL);
			ACBus1PhaseC.WireTo(NULL);
		}else{
			ACBus2PhaseA.WireTo(NULL);
			ACBus2PhaseB.WireTo(NULL);
			ACBus2PhaseC.WireTo(NULL);
		}
	} else {
		if (busno == 1){
			if(AcBus1Switch1.GetState() == TOGGLESWITCH_UP){ AcBus1Switch1.UpdateSourceState(); }
			if(AcBus1Switch2.GetState() == TOGGLESWITCH_UP){ AcBus1Switch2.UpdateSourceState(); }
			if(AcBus1Switch3.GetState() == TOGGLESWITCH_UP){ AcBus1Switch3.UpdateSourceState(); }
		}else{
			if(AcBus2Switch1.GetState() == TOGGLESWITCH_UP){ AcBus2Switch1.UpdateSourceState(); }
			if(AcBus2Switch2.GetState() == TOGGLESWITCH_UP){ AcBus2Switch2.UpdateSourceState(); }
			if(AcBus2Switch3.GetState() == TOGGLESWITCH_UP){ AcBus2Switch3.UpdateSourceState(); }
		}
	}
}

//
// ISS warning state.
//

void Saturn::GetAGCWarningStatus(AGCWarningStatus &aws)

{
	ChannelValue val11;
	ChannelValue val13;
	ChannelValue val33;
	ChannelValue val163;

	val11 = agc.GetOutputChannel(011);
	if (val11[ISSWarning]) 
		aws.ISSWarning = true;
	else
		aws.ISSWarning = false;

	val163 = agc.GetOutputChannel(0163);
	if (val163[Ch163DSKYWarn])
		aws.DSKYWarn = true;
	else
		aws.DSKYWarn = false;
		
	aws.PGNSWarning = false;
	// Restart alarm
	if (agc.vagc.VoltageAlarm != 0) 
		aws.PGNSWarning = true;
	// Gimbal Lock 
	if (agc.GetGimbalLockAlarm()) 
		aws.PGNSWarning = true;
	// Prog alarm
	if (agc.GetProgAlarm()) 
		aws.PGNSWarning = true;
	// Temp alarm
	if (val11[LightTempCaution])
		aws.PGNSWarning = true;
}

bool Saturn::LETAttached()

{
	return LESAttached;
}

void Saturn::CutLESLegs()

{
	LESLegsCut = true;
}

//
// Engine control functions for AGC.
//

void Saturn::SetRCSState(int Quad, int Thruster, bool Active)

{
	//
	// Sanity check.
	//

	if (stage > CSM_LEM_STAGE || stage < PRELAUNCH_STAGE)
		return;

	THRUSTER_HANDLE th = 0;

	switch (Quad) {
	case RCS_SM_QUAD_A:
		th = th_rcs_a[Thruster];
		break;

	case RCS_SM_QUAD_B:
		th = th_rcs_b[Thruster];
		break;

	case RCS_SM_QUAD_C:
		th = th_rcs_c[Thruster];
		break;

	case RCS_SM_QUAD_D:
		th = th_rcs_d[Thruster];
		break;
	}

	double Level = Active ? 1.0 : 0.0;

	if (th)
		SetThrusterLevel(th, Level);
}

void Saturn::SetCMRCSState(int Thruster, bool Active)

{
	double Level = Active ? 1.0 : 0.0;
	if (th_att_cm[Thruster] == NULL) return;  // Sanity check
	SetThrusterLevel(th_att_cm[Thruster], Level);
	th_att_cm_commanded[Thruster] = Active;  
}

bool Saturn::GetCMRCSStateCommanded(THRUSTER_HANDLE th) {

	for (int i = 0; i < 12; i++) {
		if (th == th_att_cm[i]) {
			return th_att_cm_commanded[i];
		}
	}
	return false;
}


void Saturn::RCSSoundTimestep() {

	// In case of disabled Orbiter attitude thruster groups OrbiterSound plays no
	// engine sound, so this needs to be done manually

	int i;
	bool on = false;
	// CSM RCS
	if (stage >= PRELAUNCH_STAGE && stage <= CSM_LEM_STAGE) {
		for (i = 1; i < 5; i++) {
			if (th_rcs_a[i]) {
				if (GetThrusterLevel(th_rcs_a[i])) on = true;
			}
			if (th_rcs_b[i]) {
				if (GetThrusterLevel(th_rcs_b[i])) on = true;
			}
			if (th_rcs_c[i]) {
				if (GetThrusterLevel(th_rcs_c[i])) on = true;
			}
			if (th_rcs_d[i]) {
				if (GetThrusterLevel(th_rcs_d[i])) on = true;
			}
		}
	}
	// CM RCS
	if (stage >= PRELAUNCH_STAGE) {
		for (i = 0; i < 12; i++) {
			if (th_att_cm[i]) {
				if (GetThrusterLevel(th_att_cm[i])) on = true;
			}
		}		
	}
	// Play/stop sounds
	if (on) {
		if (RCSFireSound.isPlaying()) {
			RCSSustainSound.play(LOOP);
		} else if (!RCSSustainSound.isPlaying()) {
			RCSFireSound.play();
		}				
	} else {
		RCSSustainSound.stop();
	}
}
