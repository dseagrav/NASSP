/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Saturn-specific switches

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
#include "csmcomputer.h"
#include "ioChannels.h"

#include "saturn.h"


void SaturnToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s, int xoffset, int yoffset)
{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset);
	sat = s;
}

bool XLunarSwitch::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState,dontspring)) {
		// Do nothing, handling is done in the IU
		return true;
	}
	return false;
}

void SaturnThreePosSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s)
{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	sat = s;
}


SaturnRCSValveTalkback::SaturnRCSValveTalkback()
{
	valve = 0;
}


void SaturnRCSValveTalkback::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, RCSValve *v, bool failopen)
{
	IndicatorSwitch::Init(xp, yp, w, h, surf, row, failopen);
	valve = v;	
}

int SaturnRCSValveTalkback::GetState()
{
	if (valve && SRC && (SRC->Voltage() > SP_MIN_DCVOLTAGE))
		state = valve->IsOpen() ? 1 : 0;
	else 
		// SM RCS helium and prim. propellant talkbacks fail open
		state = (failOpen ? 1 : 0);

	return state;
}


void SaturnH2PressureMeter::Init(int i, SURFHANDLE surf, SwitchRow &row, Saturn *s)
{
	MeterSwitch::Init(row);
	Index = i;
	NeedleSurface = surf;
	Sat = s;
}

double SaturnH2PressureMeter::QueryValue()
{
	TankPressures press;
	Sat->GetTankPressures(press);

	if (Index == 1) 
		return press.H2Tank1PressurePSI;
	else
		return press.H2Tank2PressurePSI;
}

void SaturnH2PressureMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (Index == 1) 
		oapiBlt(drawSurface, NeedleSurface,  0, (130 - (int)(v / 400.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 53, (130 - (int)(v / 400.0 * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnO2PressureMeter::Init(int i, SURFHANDLE surf, SwitchRow &row, Saturn *s, ToggleSwitch *o2PressIndSwitch)
{
	MeterSwitch::Init(row);
	Index = i;
	NeedleSurface = surf;
	Sat = s;
	O2PressIndSwitch = o2PressIndSwitch;
}

double SaturnO2PressureMeter::QueryValue()
{
	TankPressures press;
	Sat->GetTankPressures(press);

	if (Index == 1)
		if (O2PressIndSwitch->IsUp())
			return press.O2Tank1PressurePSI;
		else
			return Sat->O2SurgeTankPressSensor.Voltage()*200.0 + 50.0;
	else
		return press.O2Tank2PressurePSI;
}

void SaturnO2PressureMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (Index == 1) 
		DoDrawSwitch(drawSurface, NeedleSurface, v, 86, 0);
	else
		DoDrawSwitch(drawSurface, NeedleSurface, v, 139, 10);
}

void SaturnO2PressureMeter::DoDrawSwitch(SURFHANDLE surf, SURFHANDLE needle, double value, int xOffset, int xNeedle)
{
	if (value < 100.0)
		oapiBlt(surf, needle, xOffset, 130, xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 500.0) 
		oapiBlt(surf, needle, xOffset, 130 - (int)((value - 100.0) * 0.065), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 850.0)
		oapiBlt(surf, needle, xOffset, 104 - (int)((value - 500.0) * 0.07714), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 900.0)
		oapiBlt(surf, needle, xOffset, 77 - (int)((value - 850.0) * 0.38), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 950.0)
		oapiBlt(surf, needle, xOffset, 58 - (int)((value - 900.0) * 0.42), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else if (value <= 1050.0)
		oapiBlt(surf, needle, xOffset, 37 - (int)((value - 950.0) * 0.13), xNeedle, 0, 10, 10, SURF_PREDEF_CK);

	else
		oapiBlt(surf, needle, xOffset, 24, xNeedle, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnCryoQuantityMeter::Init(char *sub, int i, SURFHANDLE surf, SwitchRow &row, Saturn *s)
{
	MeterSwitch::Init(row);
	Substance = sub;
	Index = i;
	NeedleSurface = surf;
	Sat = s;
}

double SaturnCryoQuantityMeter::QueryValue()
{
	TankQuantities q;
	Sat->GetTankQuantities(q);

	if (!strcmp("H2", Substance)) {
		if (Index == 1) 
			return q.H2Tank1Quantity;
		else
			return q.H2Tank2Quantity;
	} else {
		if (Index == 1) 
			return q.O2Tank1Quantity;
		else
			return q.O2Tank2Quantity;
	}
}

void SaturnCryoQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (!strcmp("H2", Substance)) {
		if (Index == 1) 
			oapiBlt(drawSurface, NeedleSurface,  172, (130 - (int)(v * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
		else
			oapiBlt(drawSurface, NeedleSurface,  225, (130 - (int)(v * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	} else {
		if (Index == 1) 
			oapiBlt(drawSurface, NeedleSurface,  258, (130 - (int)(v * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
		else {
			//
			// Apollo 13 O2 tank 2 quantity display failed offscale high around 46:45.
			//

			#define O2FAILURETIME	(46.0 * 3600.0 + 45.0 * 60.0)

			if (Sat->GetApolloNo() == 1301) {
				if (Sat->GetMissionTime() >= (O2FAILURETIME + 5.0)) {
					v = 1.05;
				}
				else if (Sat->GetMissionTime() >= O2FAILURETIME) {
					v += (1.05 - value) * ((Sat->GetMissionTime() - O2FAILURETIME) / 5.0);
				}
			}
			oapiBlt(drawSurface, NeedleSurface,  311, (130 - (int)(v * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
		}
	}
}


RCSQuantityMeter::RCSQuantityMeter()
{
	source = 0;
	SMRCSIndSwitch = 0;
	NeedleSurface = 0;
}

void RCSQuantityMeter::Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s, ToggleSwitch *indswitch)
{
	MeterSwitch::Init(row);
	source = s;
	SMRCSIndSwitch = indswitch;
	NeedleSurface = surf;
}

double RCSQuantityMeter::QueryValue()
{
	if (!source) return 0.0;

	// only SM data are available/displayed
	SMRCSPropellantSource *ps = source->GetSMSource();
	if (!ps) return 0;

	if (SMRCSIndSwitch->IsDown()) {		
		return ps->GetPropellantQuantityToDisplay();
	}
	return ps->GetHeliumTempF() / 100.;
}

void RCSQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface,  150, (108 - (int)(v * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


RCSFuelPressMeter::RCSFuelPressMeter()
{
	source = 0;
	NeedleSurface = 0;
}

void RCSFuelPressMeter::Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s)
{
	MeterSwitch::Init(row);
	source = s;
	NeedleSurface = surf;
}

double RCSFuelPressMeter::QueryValue()
{
	if (!source) return 0;

	SMRCSPropellantSource *ps = source->GetSMSource();
	if (ps) return ps->GetPropellantPressurePSI();

	CMRCSPropellantSource *cmps = source->GetCMSource();
	if (cmps) return cmps->GetPropellantPressurePSI();

	return 0;
}

void RCSFuelPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface,  95, (108 - (int)(v / 400.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


RCSHeliumPressMeter::RCSHeliumPressMeter()
{
	source = 0;
	NeedleSurface = 0;
}

void RCSHeliumPressMeter::Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s)
{
	MeterSwitch::Init(row);
	source = s;
	NeedleSurface = surf;
}

double RCSHeliumPressMeter::QueryValue()
{
	if (!source) return 0;

	SMRCSPropellantSource *ps = source->GetSMSource();
	if (ps) return ps->GetHeliumPressurePSI();

	CMRCSPropellantSource *cmps = source->GetCMSource();
	if (cmps) return cmps->GetHeliumPressurePSI();

	return 0;
}

void RCSHeliumPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface,  59, (108 - (int)(v / 5000.0 * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


RCSTempMeter::RCSTempMeter()
{
	source = 0;
	NeedleSurface = 0;
}

void RCSTempMeter::Init(SURFHANDLE surf, SwitchRow &row, PropellantRotationalSwitch *s)
{
	MeterSwitch::Init(row);
	source = s;
	NeedleSurface = surf;
}

double RCSTempMeter::QueryValue()
{
	if (!source) return 0;

	SMRCSPropellantSource *ps = source->GetSMSource();
	if (ps) return ps->GetPackageTempF();

	CMRCSPropellantSource *cmps = source->GetCMSource();
	if (cmps) return cmps->GetHeliumTempF();

	return 0;
}

void RCSTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface,  4, (108 - (int)(v / 300.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


PropellantSource::PropellantSource(PROPELLANT_HANDLE &h) : source_prop(h)
{
	our_vessel = 0;
}

PROPELLANT_HANDLE PropellantSource::Handle()
{
	return source_prop;
}

double PropellantSource::Quantity()
{
	if (source_prop && our_vessel) {
		return our_vessel->GetPropellantMass(source_prop) / our_vessel->GetPropellantMaxMass(source_prop);
	}

	return 0.0;
}


PropellantRotationalSwitch::PropellantRotationalSwitch()
{
	int i;

	for (i = 0; i < 7; i++) {
		CMSources[i] = 0;
		SMSources[i] = 0;
	}
}

void PropellantRotationalSwitch::SetCMSource(int num, CMRCSPropellantSource *s)
{
	if (num >= 0 && num < 7)
		CMSources[num] = s; 
}

void PropellantRotationalSwitch::SetSMSource(int num, SMRCSPropellantSource *s)
{
	if (num >= 0 && num < 7)
		SMSources[num] = s; 
}

CMRCSPropellantSource *PropellantRotationalSwitch::GetCMSource()
{
	return CMSources[GetState()];
}

SMRCSPropellantSource *PropellantRotationalSwitch::GetSMSource()
{
	return SMSources[GetState()];
}


void SaturnFuelCellMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, RotationalSwitch *fuelCellIndicatorsSwitch)
{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
	FuelCellIndicatorsSwitch = fuelCellIndicatorsSwitch;
}


double SaturnFuelCellH2FlowMeter::QueryValue()
{
	FuelCellStatus fc;
	Sat->GetFuelCellStatus(FuelCellIndicatorsSwitch->GetState() + 1, fc);

	return fc.H2FlowLBH; 
}

void SaturnFuelCellH2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (v < 0.05)
		oapiBlt(drawSurface, NeedleSurface, 0, (111 - (int)(v / 0.05 * 21.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 0.15)
		oapiBlt(drawSurface, NeedleSurface, 0, (90 - (int)((v - 0.05) / 0.1 * 65.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 0, (25 - (int)((v - 0.15) / 0.05 * 21.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnFuelCellO2FlowMeter::QueryValue()
{
	FuelCellStatus fc;
	Sat->GetFuelCellStatus(FuelCellIndicatorsSwitch->GetState() + 1, fc);

	return fc.O2FlowLBH; 
}

void SaturnFuelCellO2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (v < 0.4)
		oapiBlt(drawSurface, NeedleSurface, 53, (111 - (int)(v / 0.4 * 21.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 1.2)
		oapiBlt(drawSurface, NeedleSurface, 53, (90 - (int)((v - 0.4) / 0.8 * 65.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 53, (25 - (int)((v - 1.2) / 0.4 * 21.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnFuelCellTempMeter::QueryValue()
{
	return (Sat->GetSCE()->GetVoltage(2, FuelCellIndicatorsSwitch->GetState() + 6)*94.0 + 80.0);
}

void SaturnFuelCellTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (v < 400.0)
		oapiBlt(drawSurface, NeedleSurface, 86, (109 - (int)((v - 100.0) / 300.0 * 53.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 500.0)
		oapiBlt(drawSurface, NeedleSurface, 86, (56 - (int)((v - 400.0) / 100.0 * 40.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 86, (16 - (int)((v - 500.0) / 50.0 * 12.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnFuelCellCondenserTempMeter::QueryValue()
{
	return (Sat->GetSCE()->GetVoltage(2, FuelCellIndicatorsSwitch->GetState() + 3)*21.0 + 145.0);
}

void SaturnFuelCellCondenserTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface, 139, (109 - (int)((v - 150.0) / 100.0 * 103.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnCabinMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s)
{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
}


double SaturnSuitTempMeter::QueryValue()
{
	return Sat->SuitTempSensor.Voltage()*15.0 + 20.0;
}

void SaturnSuitTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface,  1, (110 - (int)((v - 20.0) / 75.0 * 104.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnCabinTempMeter::QueryValue()
{
	return Sat->CabinTempSensor.Voltage()*17.0 + 40.0;
}

void SaturnCabinTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface,  53, (110 - (int)((v - 40.0) / 80.0 * 104.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnSuitPressMeter::QueryValue()
{
	return Sat->SuitPressSensor.Voltage()*3.4;
}

void SaturnSuitPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (v < 6.0)
		oapiBlt(drawSurface, NeedleSurface,  101, (108 - (int)(v / 6.0 * 55.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface,  101, (53 - (int)((v - 6.0) / 10.0 * 45.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnCabinPressMeter::QueryValue()
{
	return Sat->CabinPressSensor.Voltage()*3.4;
}

void SaturnCabinPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (v < 6.0)
		oapiBlt(drawSurface, NeedleSurface,  153, (108 - (int)(v / 6.0 * 55.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface,  153, (53 - (int)((v - 6.0) / 10.0 * 45.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


double SaturnPartPressCO2Meter::QueryValue()
{
	return pow(Sat->CO2PartPressSensor.Voltage(), 2)*30.0 / 25.0;
}

void SaturnPartPressCO2Meter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (v < 10.0)
		oapiBlt(drawSurface, NeedleSurface, 215, (109 - (int)(v / 10.0 * 55.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 15.0)
		oapiBlt(drawSurface, NeedleSurface, 215, (54 - (int)((v - 10.0) / 5.0 * 19.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else if (v < 20.0)
		oapiBlt(drawSurface, NeedleSurface, 215, (35 - (int)((v - 15.0) / 5.0 * 15.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	else
		oapiBlt(drawSurface, NeedleSurface, 215, (20 - (int)((v - 20.0) / 10.0 * 14.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}

void SaturnRoundMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s)
{
	RoundMeter::Init(p0, p1, row);
	Sat = s;
}

double SaturnSuitComprDeltaPMeter::QueryValue()
{
	return Sat->SuitCompressorDeltaPSensor.Voltage()*1.0 / 5.0;
}

void SaturnSuitComprDeltaPMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - .5) / .5 * 60.0;
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


double SaturnLeftO2FlowMeter::QueryValue()
{
	return Sat->ECSO2FlowO2SupplyManifoldSensor.Voltage()*0.16 + 0.2;
}

void SaturnLeftO2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - .6) / .4 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


double SaturnSuitCabinDeltaPMeter::QueryValue()
{
	return Sat->SuitCabinDeltaPressSensor.Voltage()*2.0 - 5.0;
}

void SaturnSuitCabinDeltaPMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v / 5.0) * 60.0;
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 13, 0, 0, 46, 18, SURF_PREDEF_CK);
}


double SaturnRightO2FlowMeter::QueryValue()
{
	return Sat->ECSO2FlowO2SupplyManifoldSensor.Voltage()*0.16 + 0.2;
}

void SaturnRightO2FlowMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - .6) / .4 * 60.0;
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 13, 0, 0, 46, 18, SURF_PREDEF_CK);
}


void SaturnEcsRadTempInletMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch)
{
	SaturnRoundMeter::Init(p0, p1, row, s);
	ECSIndicatorsSwitch = ecsindicatorsswitch;
}

double SaturnEcsRadTempInletMeter::QueryValue()
{
	if (ECSIndicatorsSwitch->GetState() == 0) {
		return Sat->PriRadInTempSensor.Voltage()*13.0 + 55.0;
	} else {
		return Sat->SecRadInTempSensor.Voltage()*13.0 + 55.0;
	}
}

void SaturnEcsRadTempInletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 90.0) / 30.0 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


double SaturnEcsRadTempPrimOutletMeter::QueryValue()
{
	return Sat->ECSRadOutTempSensor.Voltage()*30.0 - 50.0;
}

void SaturnEcsRadTempPrimOutletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 25.0) / 75.0 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


double SaturnEcsRadTempSecOutletMeter::QueryValue()
{
	return Sat->SecRadOutTempSensor.Voltage()*8.0 + 30.0;
}

void SaturnEcsRadTempSecOutletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 50.0) / 20.0 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


void SaturnGlyEvapTempOutletMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch)
{
	SaturnRoundMeter::Init(p0, p1, row, s);
	ECSIndicatorsSwitch = ecsindicatorsswitch;
}

double SaturnGlyEvapTempOutletMeter::QueryValue()
{
	if (ECSIndicatorsSwitch->GetState() == 0) {
		return Sat->GlyEvapOutTempSensor.Voltage()*10.0 + 25.0;
	} else {
		return Sat->SecEvapOutLiqTempSensor.Voltage()*10.0 + 25.0;
	}
}

void SaturnGlyEvapTempOutletMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 50.0) / 20.0 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


void SaturnGlyEvapSteamPressMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch)
{
	SaturnRoundMeter::Init(p0, p1, row, s);
	ECSIndicatorsSwitch = ecsindicatorsswitch;
}

double SaturnGlyEvapSteamPressMeter::QueryValue()
{
	if (ECSIndicatorsSwitch->GetState() == 0) {
		return Sat->GlyEvapBackPressSensor.Voltage()*0.04 + 0.05; 
	} else {
		return Sat->SecEvapOutSteamPressSensor.Voltage()*0.04 + 0.05;
	}
}

void SaturnGlyEvapSteamPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 0.15) / 0.1 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


void SaturnGlycolDischPressMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch)
{
	SaturnRoundMeter::Init(p0, p1, row, s);
	ECSIndicatorsSwitch = ecsindicatorsswitch;
}

double SaturnGlycolDischPressMeter::QueryValue()
{
	if (ECSIndicatorsSwitch->GetState() == 0) {
		return Sat->GlycolPumpOutPressSensor.Voltage()*12.0;
	} else {
		return Sat->SecGlyPumpOutPressSensor.Voltage()*12.0;
	}
}

void SaturnGlycolDischPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 30.0) / 30.0 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


void SaturnAccumQuantityMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, RotationalSwitch *ecsindicatorsswitch)
{
	SaturnRoundMeter::Init(p0, p1, row, s);
	ECSIndicatorsSwitch = ecsindicatorsswitch;
}

double SaturnAccumQuantityMeter::QueryValue()
{
	if (ECSIndicatorsSwitch->GetState() == 0) {
		return Sat->GlycolAccumQtySensor.Voltage()*1.0 / 5.0;
	} else {
		return Sat->SecGlycolAccumQtySensor.Voltage()*1.0 / 5.0;
	}
}

void SaturnAccumQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 0.5) / 0.5 * 60.0;	
	DrawNeedle(drawSurface, 0, 22, 20.0, v * RAD);
}


void SaturnH2oQuantityMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, ToggleSwitch *h2oqtyindswitch, PowerSource *pwr)
{
	SaturnRoundMeter::Init(p0, p1, row, s);
	H2oQtyIndSwitch = h2oqtyindswitch;
	WireTo(pwr);
}

double SaturnH2oQuantityMeter::QueryValue()
{
	if (H2oQtyIndSwitch->IsUp())
		return Sat->PotH2OQtySensor.Voltage()*1.0 / 5.0;
	else
		return Sat->WasteH2OQtySensor.Voltage()*1.0 / 5.0;
}

void SaturnH2oQuantityMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (v - 0.5) / 0.5 * 60.0;	
	DrawNeedle(drawSurface, 45, 22, 20.0, (180.0 - v) * RAD);
}


double SaturnAccelGMeter::QueryValue()
{
	return Sat->GetAccelG();
}

void SaturnAccelGMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (-v * 180.0 / 12.0) + 180.0;
	DrawNeedle (drawSurface, 40, 40, 35.0, v * RAD);
}


void DirectO2RotationalSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, h_Pipe *p)
{
	RotationalSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	Pipe = p;
}

/*bool DirectO2RotationalSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (RotationalSwitch::CheckMouseClick(event, mx, my)) {
		CheckValve();
		return true;
	}
	return false;
}*/

bool DirectO2RotationalSwitch::SwitchTo(int newValue)
{
	if (RotationalSwitch::SwitchTo(newValue)) {
		CheckValve();
		return true;
	}
	return false;
}
/*
void DirectO2RotationalSwitch::CheckValve()
{
	if (GetState() == 6) {
		Pipe->in->h_open = SP_VALVE_CLOSE;
		Pipe->flowMax = 0;
	
	} else if (GetState() == 5) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 6.0 / LBH;		//0.1 lb/min

	} else if (GetState() == 4) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 12.0 / LBH;		//0.2 lb/min

	} else if (GetState() == 3) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 18.6 / LBH;		//0.31 lb/min

	} else if (GetState() == 2) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 24.6 / LBH;		//0.41 lb/min

	} else if (GetState() == 1) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 31.8 / LBH;		//0.53 lb/min

	} else if (GetState() == 0) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 40.2 / LBH;		//0.67 lb/min
	}
}
*/

void DirectO2RotationalSwitch::CheckValve()

{
	if (GetState() == 6) {
		Pipe->in->h_open = SP_VALVE_CLOSE;
		Pipe->flowMax = 0;

	}
	else if (GetState() == 5) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 0.42 / LBH;  //0.007 lb/min

	}
	else if (GetState() == 4) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 0.78 / LBH;  //0.013 lb/min

	}
	else if (GetState() == 3) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 1.56 / LBH;  //0.026 lb/min

	}
	else if (GetState() == 2) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 24.6 / LBH;		//0.41 lb/min

	}
	else if (GetState() == 1) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 31.8 / LBH;		//0.53 lb/min

	}
	else if (GetState() == 0) {
		Pipe->in->h_open = SP_VALVE_OPEN;
		Pipe->flowMax = 40.2 / LBH;		//0.67 lb/min
	}
}


void SaturnEcsGlycolPumpsSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Pump *p,
									  CircuitBrakerSwitch* ac1a, CircuitBrakerSwitch* ac1b, CircuitBrakerSwitch* ac1c,
									  CircuitBrakerSwitch* ac2a, CircuitBrakerSwitch* ac2b, CircuitBrakerSwitch* ac2c)
{
	RotationalSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	GlycolPump = p;
	ACBus1.WireToBuses(ac1a, ac1b, ac1c);
	ACBus2.WireToBuses(ac2a, ac2b, ac2c);
	CheckPump();
}

/*bool SaturnEcsGlycolPumpsSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (RotationalSwitch::CheckMouseClick(event, mx, my)) {
		CheckPump();
		return true;
	}
	return false;
}*/

bool SaturnEcsGlycolPumpsSwitch::SwitchTo(int newValue)
{
	if (RotationalSwitch::SwitchTo(newValue)) {
		CheckPump();
		return true;
	}
	return false;
}

void SaturnEcsGlycolPumpsSwitch::LoadState(char *line)
{
	RotationalSwitch::LoadState(line);
	CheckPump();
}

void SaturnEcsGlycolPumpsSwitch::CheckPump()
{
	if (GetState() == 2) 
		GlycolPump->WireTo(NULL);
	else if (GetState() == 1 || GetState() == 3) 
		GlycolPump->WireTo(&ACBus1);
	else
		GlycolPump->WireTo(&ACBus2);
}


void SaturnSuitCompressorSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,
									  CircuitBrakerSwitch* ac1a, CircuitBrakerSwitch* ac1b, CircuitBrakerSwitch* ac1c,
									  CircuitBrakerSwitch* ac2a, CircuitBrakerSwitch* ac2b, CircuitBrakerSwitch* ac2c)
{
	ACBus1.WireToBuses(ac1a, ac1b, ac1c);
	ACBus2.WireToBuses(ac2a, ac2b, ac2c);
	ThreeSourceSwitch::Init(xp, yp, w, h, surf, bsurf, row, &ACBus1, NULL, &ACBus2);
}

void DCBusIndicatorSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, DCBusController *d, int fc)
{
	IndicatorSwitch::Init(xp, yp, w, h, surf, row);
	dcbus = d;
	fuelcell = fc;
}

int DCBusIndicatorSwitch::GetState()
{
	if (dcbus->IsFuelCellConnected(fuelcell) || !dcbus->IsBusContPowered(fuelcell))
		return 1;
	else
		return 0;
}


SaturnDCAmpMeter::SaturnDCAmpMeter(double minVal, double maxVal, double vMin, double vMax) :
	ElectricMeter(minVal, maxVal, vMin, vMax)
{
}

void SaturnDCAmpMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, PowerStateRotationalSwitch *dcindicatorswitch)
{
	ElectricMeter::Init(p0, p1, row, dcindicatorswitch);
	DCIndicatorSwitch = dcindicatorswitch;
}

double SaturnDCAmpMeter::QueryValue()
{
	// Battery Charger
	if (DCIndicatorSwitch->GetState() == 7)
		return DCIndicatorSwitch->Current() * 20.0;

	return DCIndicatorSwitch->Current();
}

void BMAGPowerRotationalSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, BMAG *Unit)
{
	RotationalSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	bmag = Unit;
	
	CheckBMAGPowerState();
}

void BMAGPowerRotationalSwitch::CheckBMAGPowerState()
{
	switch (GetState()) {
	case 0: // OFF
		bmag->SetPower(false, false);
		break;
	case 1: // WARM UP
		bmag->SetPower(true, false);
		break;
	case 2: // ON
		bmag->SetPower(true, true);
		break;
	}
}

/*bool BMAGPowerRotationalSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (RotationalSwitch::CheckMouseClick(event, mx, my)) {		
		CheckBMAGPowerState();
		return true;
	}

	return false;
}*/

bool BMAGPowerRotationalSwitch::SwitchTo(int newValue)
{
	if (RotationalSwitch::SwitchTo(newValue)) {
		CheckBMAGPowerState();
		return true;
	}

	return false;
}

void BMAGPowerRotationalSwitch::LoadState(char *line)
{
	RotationalSwitch::LoadState(line);
	CheckBMAGPowerState();
}


void SaturnSPSPercentMeter::Init(SURFHANDLE blackFontSurf, SURFHANDLE whiteFontSurf, SwitchRow &row, Saturn *s)
{
	MeterSwitch::Init(row);
	BlackFontSurface = blackFontSurf;
	WhiteFontSurface = whiteFontSurf;
	Sat = s;
}

void SaturnSPSPercentMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	int percent = (int) (v * 1000.0);

	// What should the panel display with full tanks? Looks like 99.9 is the maximum.
	if (percent > 999) {
		percent = 999;
	}

	int digit1 = percent / 100;
	percent -= (digit1 * 100);

	int digit2 = percent / 10;
	int digit3 = percent - (digit2 * 10);

	oapiBlt(drawSurface, BlackFontSurface, 0, 0, 10 * digit1, 0, 10, 12);
	oapiBlt(drawSurface, BlackFontSurface, 13, 0, 10 * digit2, 0, 10, 12);
	oapiBlt(drawSurface, WhiteFontSurface, 26, 0, 11 * digit3, 0, 11, 12);
}


double SaturnSPSOxidPercentMeter::QueryValue()
{
	return Sat->GetSPSPropellant()->GetOxidPercent(); 
}


double SaturnSPSFuelPercentMeter::QueryValue()
{
	return Sat->GetSPSPropellant()->GetFuelPercent(); 
}


double SaturnSPSOxidUnbalMeter::QueryValue()
{
	return Sat->GetSPSPropellant()->GetOxidUnbalanceLB();
}

void SaturnSPSOxidUnbalMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = v / 450.0 * 90.0;
	DrawNeedle(drawSurface, 30, 31, 28.0, (180.0 - v) * RAD);
}


void SaturnSPSPropellantPressMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, bool fuel, e_object *p)
{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
	Fuel = fuel;
	WireTo(p);
}

double SaturnSPSPropellantPressMeter::QueryValue()
{
	// Fuel and oxidizer have the same pressure for now.
	return Sat->GetSPSPropellant()->GetPropellantPressurePSI();
}

void SaturnSPSPropellantPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (Fuel) {
		oapiBlt(drawSurface, NeedleSurface, 86, (109 - (int)(v / 250.0 * 103.0)), 0, 0, 10, 10, SURF_PREDEF_CK);	
	} else {
		oapiBlt(drawSurface, NeedleSurface, 139, (109 - (int)(v / 250.0 * 103.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
	}
}


void SaturnSPSTempMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, e_object *p)
{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
	WireTo(p);
}

double SaturnSPSTempMeter::QueryValue()
{
	return Sat->GetSPSPropellant()->GetPropellantLineTempF();
}

void SaturnSPSTempMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface, 0, (109 - (int)(v / 200.0 * 103.0)), 0, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnSPSHeliumNitrogenPressMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, ThreePosSwitch *spspressindswitch)
{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
	SPSPressIndSwitch = spspressindswitch;
}

double SaturnSPSHeliumNitrogenPressMeter::QueryValue()
{
    if (SPSPressIndSwitch->IsUp()) {
        WireTo(&Sat->InstrumentationPowerFeeder); // InstrumentationPowerFeeder until proper source found
        return Sat->GetSPSPropellant()->GetHeliumPressurePSI();
    } else if (SPSPressIndSwitch->IsCenter()) {
        WireTo(&Sat->Panel276CB3);
        return Sat->GetSPSEngine()->GetNitrogenPressureAPSI();
    } else {
        WireTo(&Sat->Panel276CB4);
        return Sat->GetSPSEngine()->GetNitrogenPressureBPSI();
    }
}

void SaturnSPSHeliumNitrogenPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface, 53, (109 - (int)(v / 5000.0 * 103.0)), 10, 0, 10, 10, SURF_PREDEF_CK);
}


void SaturnLVSPSPcMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, Saturn *s, ToggleSwitch *lvspspcindicatorswitch, SURFHANDLE frameSurface)
{
	SaturnRoundMeter::Init(p0, p1, row, s);
	LVSPSPcIndicatorSwitch = lvspspcindicatorswitch;
	FrameSurface = frameSurface;
}

double SaturnLVSPSPcMeter::QueryValue()
{
	//
	// Note: on a real Saturn, it shows a percentage of pressure difference measured by 
	// the Q-ball of the LES. For now it shows AoA x 10
	//

	if (LVSPSPcIndicatorSwitch->IsDown()) {
		return Sat->GetSPSEngine()->GetChamberPressurePSI();

	} else {
		if (Sat->stage < CSM_LEM_STAGE) {
			return fabs((10.0 / RAD) * Sat->qball.GetAOA());
		} else {
			return 0;
		}
	}
}

void SaturnLVSPSPcMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = (155.0 - v) / 160.0 * 270.0;	
	DrawNeedle(drawSurface, 48, 45, 20.0, (v - 45.0) * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 0, 0, 0, 95, 91, SURF_PREDEF_CK);
}

SaturnSystemTestAttenuator::SaturnSystemTestAttenuator(char *i_name, double minIn, double maxIn, double minOut, double maxOut):
	VoltageAttenuator(i_name, minIn, maxIn, minOut, maxOut)
{
}

void SaturnSystemTestAttenuator::Init(Saturn* s, RotationalSwitch *leftsystemtestrotaryswitch, RotationalSwitch *rightsystemtestrotaryswitch, e_object *Instrum)
{
	Sat = s;
	LeftSystemTestRotarySwitch = leftsystemtestrotaryswitch;
	RightSystemTestRotarySwitch = rightsystemtestrotaryswitch;

	WireTo(Instrum);
}

double SaturnSystemTestAttenuator::GetValue()
{
	unsigned char val = NULL;
	int left = LeftSystemTestRotarySwitch->GetState(); //0 = Off, 1 = 1 and so on
	int right = RightSystemTestRotarySwitch->GetState();// 0 = A, 1 = B, 2 = C, 3 = D

	switch(left)
	{
	case 1:
		switch (right)
		{
		case 0:	//FC1 N2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 29);
			break;
		case 1:	//FC2 N2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 30);
			break;
		case 2:	//FC3 N2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 35);
			break;
		case 3:	//FC1 O2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 67);
			break;
		}
		break;
	case 2:
		switch (right)
		{
		case 0:	//FC2 O2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 68);
			break;
		case 1: //FC3 O2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 69);
			break;
		case 2:	//FC1 H2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 70);
			break;
		case 3:	//FC2 H2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 71);
			break;
		}
		break;
	case 3:
		switch (right)
		{
		case 0:	//FC3 H2 REG PRESS
			val = Sat->pcm.measure(11, TLM_A, 72);
			break;
		case 1: //FC 1 RAD OUT TEMP
			val = Sat->pcm.measure(10, TLM_A, 126);
			break;
		case 2: // FC 2 RAD OUT TEMP
			val = Sat->pcm.measure(10, TLM_A, 129);
			break;
		case 3: // FC 3 RAD OUT TEMP
			val = Sat->pcm.measure(10, TLM_A, 132);
			break;
		}
		break;
	case 4:
		switch (right)
		{
		case 1:	//BAT RLY BUS VOLT
			return Sat->sce.GetVoltage(0, 4)*256.0 / 5.0;	//Temporary scaling
		case 3:	//CSM TO LM CURRENT
			return Sat->sce.GetVoltage(1, 7)*256.0 / 5.0;	//Temporary scaling
		}
		break;
	case 5:
		switch (right)
		{
		case 0:	//SPS OX LINE TEMP
			val = Sat->pcm.measure(10, TLM_A, 120);
			break;
		case 2:	//TEMP -P ENG INJECTOR SYS B
			val = Sat->pcm.scale_data(30, -50, 50);
			break;
		case 3:	//TEMP +Y ENG INJECTOR SYS B
			val = Sat->pcm.scale_data(30, -50, 50);
			break;
		}
		break;
	case 6:
		switch (right)
		{
		case 0:	//TEMP CCW ENG INJECTOR SYS A
			val = Sat->pcm.scale_data(30, -50, 50);
			break;
		case 1:	//TEMP -P ENG INJECTOR SYS A
			val = Sat->pcm.scale_data(30, -50, 50);
			break;
		case 2:	//TEMP -Y ENG INJECTOR SYS A
			val = Sat->pcm.scale_data(30, -50, 50);
			break;
		case 3:	//TEMP CW ENG INJECTOR SYS B
			val = Sat->pcm.scale_data(30, -50, 50);
			break;
		}
		break;
	case 8:
		switch (right)
		{
		case 0:	
			val = Sat->RRTsystem.GetScaledRFPower();
			break;
		case 1:	
			val = Sat->RRTsystem.GetScaledAGCPower();
			break;
		case 2:	
			val = Sat->RRTsystem.GetScaledFreqLock();
			break;
		}
	}

	return (double)val;
}

void SaturnGPFPIMeter::Init(SURFHANDLE surf, SwitchRow &row, Saturn *s, int sys, int xoffset)
{
	MeterSwitch::Init(row);
	NeedleSurface = surf;
	Sat = s;
	system = sys;
	xOffset = xoffset;
}

double SaturnGPFPIMeter::AdjustForPower(double val) 
{ 
	if (ACSource && DCSource) {
		if (ACSource->Voltage() > SP_MIN_ACVOLTAGE && DCSource->Voltage() > SP_MIN_DCVOLTAGE)
			return val;
	}
	return 0; 
}

void SaturnGPFPIMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, NeedleSurface, xOffset,      93 - (int)v, 10, 1, 7, 8, SURF_PREDEF_CK);
	oapiBlt(drawSurface, NeedleSurface, xOffset + 12, 93 - (int)v,  3, 1, 7, 8, SURF_PREDEF_CK);
}


double SaturnGPFPIPitchMeter::QueryValue()
{
	return Sat->GetEDA()->GetGPFPIPitch(system);
}


double SaturnGPFPIYawMeter::QueryValue()
{
	return Sat->GetEDA()->GetGPFPIYaw(system);
}


void FDAIPowerRotationalSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, FDAI *F1, FDAI *F2,
    					 		     e_object *dc1, e_object *dc2, e_object *ac1, e_object *ac2,
									 SaturnGPFPIMeter *gpfpiPitch1, SaturnGPFPIMeter *gpfpiPitch2, SaturnGPFPIMeter *gpfpiYaw1, SaturnGPFPIMeter *gpfpiYaw2, EDA *ed)
{
	RotationalSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	FDAI1 = F1;
	FDAI2 = F2;

	DCSource1 = dc1;
	DCSource2 = dc2;
	ACSource1 = ac1;
	ACSource2 = ac2;

	GPFPIPitch1 =gpfpiPitch1;
	GPFPIPitch2 =gpfpiPitch2;
	GPFPIYaw1 =gpfpiYaw1;
	GPFPIYaw2 =gpfpiYaw2;

	eda = ed;

	CheckFDAIPowerState();
}

//
// Wire up the FDAIs to the appropriate power source based on switch position. We wire them to this
// switch, and higher level code can wire the switch to a suitable power source.
//

void FDAIPowerRotationalSwitch::CheckFDAIPowerState()
{
	switch (GetState()) {
	case 0:
		FDAI1->WireTo(NULL, NULL);
		FDAI2->WireTo(NULL, NULL);

		GPFPIPitch1->WireTo(NULL, NULL);
		GPFPIPitch2->WireTo(NULL, NULL);
		GPFPIYaw1->WireTo(NULL, NULL);
		GPFPIYaw2->WireTo(NULL, NULL);

		eda->WireTo(NULL, NULL, NULL, NULL);

		break;

	case 1:
		FDAI1->WireTo(DCSource1, ACSource1);
		FDAI2->WireTo(NULL, NULL);

		GPFPIPitch1->WireTo(DCSource1, ACSource1);
		GPFPIPitch2->WireTo(NULL, NULL);
		GPFPIYaw1->WireTo(DCSource1, ACSource1);
		GPFPIYaw2->WireTo(NULL, NULL);

		eda->WireTo(ACSource1, NULL, DCSource1, NULL);
		break;

	case 2:
		FDAI1->WireTo(NULL, NULL);
		FDAI2->WireTo(DCSource2, ACSource2);

		GPFPIPitch1->WireTo(NULL, NULL);
		GPFPIPitch2->WireTo(DCSource2, ACSource2);
		GPFPIYaw1->WireTo(NULL, NULL);
		GPFPIYaw2->WireTo(DCSource2, ACSource2);

		eda->WireTo(NULL, ACSource2, NULL, DCSource2);
		break;

	case 3:
		FDAI1->WireTo(DCSource1, ACSource1);
		FDAI2->WireTo(DCSource2, ACSource2);

		GPFPIPitch1->WireTo(DCSource1, ACSource1);
		GPFPIPitch2->WireTo(DCSource2, ACSource2);
		GPFPIYaw1->WireTo(DCSource1, ACSource1);
		GPFPIYaw2->WireTo(DCSource2, ACSource2);

		eda->WireTo(ACSource1, ACSource2, DCSource1, DCSource2);
		break;
	}
}

/*bool FDAIPowerRotationalSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (RotationalSwitch::CheckMouseClick(event, mx, my)) {
		CheckFDAIPowerState();
		return true;
	}

	return false;
}*/

bool FDAIPowerRotationalSwitch::SwitchTo(int newValue)
{
	if (RotationalSwitch::SwitchTo(newValue)) {
		CheckFDAIPowerState();
		return true;
	}

	return false;
}

void FDAIPowerRotationalSwitch::LoadState(char *line)
{
	RotationalSwitch::LoadState(line);
	CheckFDAIPowerState();
}


//
// CMACInverterSwitch allows you to connect the CM AC inverters to the CM AC busses.
//

void CMACInverterSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,int bus,int inv, Saturn *ship)
{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	acbus = bus;
	acinv = inv;
	sat = ship;
	UpdateSourceState();
}

/*bool CMACInverterSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (ToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}*/

bool CMACInverterSwitch::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState,dontspring)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

void CMACInverterSwitch::UpdateSourceState()
{	 
	if (IsUp()) {
		switch(acbus){
		case 1: // AC1
			switch(acinv){
			case 1:
				// Inverter 3 beats inverter 1
				if(sat->AcBus1Switch3.GetState() == TOGGLESWITCH_UP){
					return; // LOCKOUT
				}
				sat->ACBus1PhaseA.WireTo(&sat->Inverter1->PhaseA);
				sat->ACBus1PhaseB.WireTo(&sat->Inverter1->PhaseB);
				sat->ACBus1PhaseC.WireTo(&sat->Inverter1->PhaseC);
				break;
			case 2:
				// Inverter 1 beats inverter 2
				if(sat->AcBus1Switch1.GetState() == TOGGLESWITCH_UP){
					return; // LOCKOUT
				}
				sat->ACBus1PhaseA.WireTo(&sat->Inverter2->PhaseA);
				sat->ACBus1PhaseB.WireTo(&sat->Inverter2->PhaseB);
				sat->ACBus1PhaseC.WireTo(&sat->Inverter2->PhaseC);
				break;
			case 3:
				// Inverter 2 beats 3
				if(sat->AcBus1Switch2.GetState() == TOGGLESWITCH_UP){
					return; // LOCKOUT
				}
				sat->ACBus1PhaseA.WireTo(&sat->Inverter3->PhaseA);
				sat->ACBus1PhaseB.WireTo(&sat->Inverter3->PhaseB);
				sat->ACBus1PhaseC.WireTo(&sat->Inverter3->PhaseC);
				break;
			}
			break;
		case 2: // AC2
			switch(acinv){
			case 1:
				if(sat->AcBus2Switch3.GetState() == TOGGLESWITCH_UP){
					return; // LOCKOUT
				}
				sat->ACBus2PhaseA.WireTo(&sat->Inverter1->PhaseA);
				sat->ACBus2PhaseB.WireTo(&sat->Inverter1->PhaseB);
				sat->ACBus2PhaseC.WireTo(&sat->Inverter1->PhaseC);
				break;
			case 2:
				if(sat->AcBus2Switch1.GetState() == TOGGLESWITCH_UP){
					return; // LOCKOUT
				}
				sat->ACBus2PhaseA.WireTo(&sat->Inverter2->PhaseA);
				sat->ACBus2PhaseB.WireTo(&sat->Inverter2->PhaseB);
				sat->ACBus2PhaseC.WireTo(&sat->Inverter2->PhaseC);
				break;
			case 3:
				if(sat->AcBus2Switch2.GetState() == TOGGLESWITCH_UP){
					return; // LOCKOUT
				}
				sat->ACBus2PhaseA.WireTo(&sat->Inverter3->PhaseA);
				sat->ACBus2PhaseB.WireTo(&sat->Inverter3->PhaseB);
				sat->ACBus2PhaseC.WireTo(&sat->Inverter3->PhaseC);
				break;
			}
			break;
		}
	}
	else if (IsDown()) {
		// SHUTTING DOWN CASE
		switch(acbus){
		case 1: // AC1
			switch(acinv){
			case 1:
				if(sat->ACBus1PhaseA.SRC != &sat->Inverter1->PhaseA){ return; } // LOCKOUT
				sat->ACBus1PhaseA.WireTo(NULL);
				sat->ACBus1PhaseB.WireTo(NULL);
				sat->ACBus1PhaseC.WireTo(NULL);
				// Check inv 2
				if(sat->AcBus1Switch2.GetState() == TOGGLESWITCH_UP){ sat->AcBus1Switch2.UpdateSourceState(); }
				break;
			case 2:
				if(sat->ACBus1PhaseA.SRC != &sat->Inverter2->PhaseA){ return; } // LOCKOUT
				sat->ACBus1PhaseA.WireTo(NULL);
				sat->ACBus1PhaseB.WireTo(NULL);
				sat->ACBus1PhaseC.WireTo(NULL);
				if(sat->AcBus1Switch3.GetState() == TOGGLESWITCH_UP){ sat->AcBus1Switch3.UpdateSourceState(); }
				break;
			case 3:
				if(sat->ACBus1PhaseA.SRC != &sat->Inverter3->PhaseA){ return; } // LOCKOUT
				sat->ACBus1PhaseA.WireTo(NULL);
				sat->ACBus1PhaseB.WireTo(NULL);
				sat->ACBus1PhaseC.WireTo(NULL);
				if(sat->AcBus1Switch1.GetState() == TOGGLESWITCH_UP){ sat->AcBus1Switch1.UpdateSourceState(); }
				break;
			}
			break;
		case 2: // AC2
			switch(acinv){
			case 1:
				if(sat->ACBus2PhaseA.SRC != &sat->Inverter1->PhaseA){ return; } // LOCKOUT
				sat->ACBus2PhaseA.WireTo(NULL);
				sat->ACBus2PhaseB.WireTo(NULL);
				sat->ACBus2PhaseC.WireTo(NULL);
				if(sat->AcBus2Switch2.GetState() == TOGGLESWITCH_UP){ sat->AcBus2Switch2.UpdateSourceState(); }
				break;
			case 2:
				if(sat->ACBus2PhaseA.SRC != &sat->Inverter2->PhaseA){ return; } // LOCKOUT
				sat->ACBus2PhaseA.WireTo(NULL);
				sat->ACBus2PhaseB.WireTo(NULL);
				sat->ACBus2PhaseC.WireTo(NULL);
				if(sat->AcBus2Switch3.GetState() == TOGGLESWITCH_UP){ sat->AcBus2Switch3.UpdateSourceState(); }
				break;
			case 3:
				if(sat->ACBus2PhaseA.SRC != &sat->Inverter3->PhaseA){ return; } // LOCKOUT
				sat->ACBus2PhaseA.WireTo(NULL);
				sat->ACBus2PhaseB.WireTo(NULL);
				sat->ACBus2PhaseC.WireTo(NULL);
				if(sat->AcBus2Switch1.GetState() == TOGGLESWITCH_UP){ sat->AcBus2Switch1.UpdateSourceState(); }
				break;
			}
			break;
		}		
	}
}

void CMACInverterSwitch::LoadState(char *line)
{
	ToggleSwitch::LoadState(line);
	UpdateSourceState();
}


/*bool SaturnSCContSwitch::CheckMouseClick(int event, int mx, int my) 
{
	if (SaturnToggleSwitch::CheckMouseClick(event, mx, my)) {
		SetSCControl(sat);
		return true;
	}
	return false;
}*/

bool SaturnSCContSwitch::SwitchTo(int newState, bool dontspring)
{
	if (SaturnToggleSwitch::SwitchTo(newState,dontspring)) {
		SetSCControl(sat);
		return true;
	}
	return false;
}


void THCRotarySwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s)
{
	RotationalSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	sat = s;
}

/*bool THCRotarySwitch::CheckMouseClick(int event, int mx, int my) 
{
	if (RotationalSwitch::CheckMouseClick(event, mx, my)) {
		SetSCControl(sat);
		return true;
	}
	return false;
}*/

bool THCRotarySwitch::SwitchTo(int newState)
{
	if (RotationalSwitch::SwitchTo(newState)) {
		SetSCControl(sat);
		return true;
	}
	return false;
}

void SaturnSCControlSetter::SetSCControl(Saturn *sat)
{
	if (sat->SCContSwitch.IsUp() && !sat->THCRotary.IsClockwise()) {
		sat->agc.SetInputChannelBit(031, GNAutopilotControl, true);
	} else {
		sat->agc.SetInputChannelBit(031, GNAutopilotControl, false);
	}
}


SaturnEMSDvSetSwitch::SaturnEMSDvSetSwitch(Sound &clicksound) : ClickSound(clicksound)
{
	position = 0;
}

void SaturnEMSDvDisplay::Init(SURFHANDLE digits, SwitchRow &row, Saturn *s)
{
	MeterSwitch::Init(row);
	Digits = digits;
	Sat = s;
}

void SaturnEMSDvDisplay::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	if (Voltage() < SP_MIN_DCVOLTAGE || Sat->ems.IsOff() || !Sat->ems.IsDisplayPowered()) return;

	if (v < 0) {
		oapiBlt(drawSurface, Digits, 0, 0, 161, 0, 10, 19);
	}

	int i, Curdigit;
	char buffer[100];
	sprintf(buffer, "%7.1f", fabs(v));
	for (i = 0; i < 7; i++) {
		if (buffer[i] >= '0' && buffer[i] <= '9') {
			Curdigit = buffer[i] - '0';
			oapiBlt(drawSurface, Digits, (i == 6 ? 0 : 10) + 16 * i, 0, 16 * Curdigit, 0, 16, 19);
		} else if (buffer[i] == '.') {
			if (!Sat->ems.IsDecimalPointBlanked())
			{
				oapiBlt(drawSurface, Digits, 10 + 16 * i, 0, 200, 0, 4, 19);
			}
		}
	}
}

double SaturnEMSDvDisplay::QueryValue() {
	return Sat->ems.GetdVRangeCounter();
}

int SaturnEMSDvDisplay::GetState() {
	return (int) (Sat->ems.GetdVRangeCounter() * 10.);
}

void SaturnEMSDvDisplay::SetState(int value) {
	Sat->ems.dVRangeCounter = value / 10.;
}


void SaturnEventTimer::Init(SwitchRow &row, Saturn *s) {
	MeterSwitch::Init(row);
	Sat = s;
}

int SaturnEventTimer::GetState() {
	return (int) (Sat->EventTimerDisplay.GetTime());
}

void SaturnEventTimer::SetState(int value) {
	Sat->EventTimerDisplay.SetTime(value);
}


void SaturnEMSScrollDisplay::Init(SwitchRow &row, Saturn *s) {
	MeterSwitch::Init(row);
	Sat = s;
}

int SaturnEMSScrollDisplay::GetState() {
	return (int) (Sat->ems.ScrollPosition);
}

void SaturnEMSScrollDisplay::SetState(int value) {
	Sat->ems.ScrollPosition = value;
}


void SaturnPanel382Cover::Init(SwitchRow &row, Saturn *s) {
	MeterSwitch::Init(row);
	Sat = s;
}

int SaturnPanel382Cover::GetState() {
	return Sat->panel382Enabled;
}

void SaturnPanel382Cover::SetState(int value) {
	Sat->panel382Enabled = value;
}


void SaturnPanel600::Init(SwitchRow &row, Saturn *s) {
	MeterSwitch::Init(row);
	Sat = s;
}

int SaturnPanel600::GetState() {
	return Sat->hatchPanel600EnabledLeft;
}

void SaturnPanel600::SetState(int value)
{
	if (value == 0) value = -1;

	Sat->hatchPanel600EnabledLeft = value;
	Sat->hatchPanel600EnabledRight = value;
}


void SaturnPanelOrdeal::Init(SwitchRow &row, Saturn *s) {
	MeterSwitch::Init(row);
	Sat = s;
}

int SaturnPanelOrdeal::GetState() {
	return Sat->ordealEnabled;
}

void SaturnPanelOrdeal::SetState(int value)
{
	if (value == 0) value = -1;

	Sat->ordealEnabled = value;
}


void SaturnASCPSwitch::Init(SwitchRow &row, Saturn *s, int axis) {
	MeterSwitch::Init(row);
	Sat = s;
	Axis = axis;
}

int SaturnASCPSwitch::GetState() {
	return (int) (Sat->ascp.output.data[Axis] * 10.);
}

void SaturnASCPSwitch::SetState(int value) {
	Sat->ascp.output.data[Axis] = value / 10.;
}

bool SaturnEMSDvSetSwitch::CheckMouseClick(int event, int mx, int my)
{
	int oldPos = position;
	switch(event) {
		case PANEL_MOUSE_LBPRESSED:
			if (my < 44)
				position = 1;
			else
				position = 3;			
			break;

		case PANEL_MOUSE_RBPRESSED:
			if (my < 44)
				position = 2;
			else
				position = 4;
			break;

		case PANEL_MOUSE_LBUP:
		case PANEL_MOUSE_RBUP:
			position = 0;
			break;
	}
	if (position && position != oldPos)
		ClickSound.play();
	return true;
}


void SaturnCabinPressureReliefLever::InitGuard(SURFHANDLE surf, SoundLib *soundlib)
{	
	guardSurface = surf;

	if (!guardClick.isValid())
		soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void SaturnCabinPressureReliefLever::DrawSwitch(SURFHANDLE drawSurface)
{
	oapiBlt(drawSurface, guardSurface, 0, 0, guardState * 152, 0, 152, 79, SURF_PREDEF_CK);
	ThumbwheelSwitch::DrawSwitch(drawSurface);
}

bool SaturnCabinPressureReliefLever::CheckMouseClick(int event, int mx, int my)
{
	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx <= 152 &&  my <= 79) {			
			if (guardState) {
				guardState = 0;
			} else {
				guardState = 1;
			}
			guardClick.play();
			return true;

		} else {
			return false;
		}
	} else {
		bool r = ThumbwheelSwitch::CheckMouseClick(event, mx, my);
		if (state == 3 && guardState == 0) {
			state = 2;
			return false;
		} else {
			return r;
		}
	}
}

bool SaturnCabinPressureReliefLever::SwitchTo(int newState)
{
	if (ThumbwheelSwitch::SwitchTo(newState))
	{
		if(state == 3 && guardState == 0)
		{
			state = 2;
			return false;
		}
		else
			return true;
	}
	return false;
}

void SaturnCabinPressureReliefLever::SetState(int value)
{
	if (ThumbwheelSwitch::SwitchTo(value))
	{
		if (state == 3 && guardState == 0)
		{
			guardState = 1;
		}
	}
}

void SaturnCabinPressureReliefLever::Guard()
{
	if (guardState) {
		guardState = 0;

		if (state == 3)
		{
			state = 2;
		}
	}
}

void SaturnCabinPressureReliefLever::SaveState(FILEHANDLE scn)
{
	char buffer[100];

	sprintf(buffer, "%i %i", state, guardState); 
	oapiWriteScenario_string(scn, name, buffer);
}

void SaturnCabinPressureReliefLever::LoadState(char *line)
{
	char buffer[100];
	int st, gst;

	sscanf(line, "%s %i %i", buffer, &st, &gst); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
	}
}


void OpticsHandcontrollerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s)
{
	sat = s;
	HandcontrollerSwitch::Init(xp, yp, w, h, surf, bsurf, row);
}

bool OpticsHandcontrollerSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (HandcontrollerSwitch::CheckMouseClick(event, mx, my)) {
		sat->optics.OpticsManualMovement &= 0xF0; 
		if (state == 1) {
			// Optics Right
			sat->optics.OpticsManualMovement |= 0x08; 
		} else if (state == 2) {
			// Optics Left
			sat->optics.OpticsManualMovement |= 0x04;
		} else if (state == 3) {
			// Optics Down
			sat->optics.OpticsManualMovement |= 0x02; 
		} else if (state == 4) {
			// Optics Up
			sat->optics.OpticsManualMovement |= 0x01; 
		}
		return true;
	}
	return false;
}


void MinImpulseHandcontrollerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s)
{
	sat = s;
	HandcontrollerSwitch::Init(xp, yp, w, h, surf, bsurf, row);
}

bool MinImpulseHandcontrollerSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (HandcontrollerSwitch::CheckMouseClick(event, mx, my)) {
		unsigned int c = sat->agc.GetInputChannel(032);
		c &= 077700;
		if (state == 1) {
			// roll right
			c |= 020;			
		} else if (state == 2) {
			// roll left
			c |= 040;			
		} else if (state == 3) {
			// pitch down
			c |= 02;
		} else if (state == 4) {
			// pitch up
			c |= 01;
		} else if (state == 5) {
			// yaw left
			c |= 010;			
		} else if (state == 6) {
			// yaw right
			c |= 04;			
		}		
		sat->agc.SetInputChannel(032, c);
		return true;
	}
	return false;
}

void DSEIndicatorSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, DSE *d, bool failopen)
{
	dse = d;
	IndicatorSwitch::Init(xp, yp, w, h, surf, row, failopen);
}

int DSEIndicatorSwitch::GetState()
{
	return dse->TapeMotion() ? 1 : 0;
}


//
// "Special offset" of 130 px to avoid overlapping with the O2DemandRegulatorRotary
//

void SuitTestSwitch::DrawSwitch(SURFHANDLE drawSurface)
{
	if (position) {
		// Find best bitmap for position angle
		int index = -1;
		for (int i = 0; i < RotationalSwitchBitmapCount; i++) {
			if (index == -1) {
				index = i;
			} else if (AngleDiff(bitmaps[i].angle, position->GetAngle()) < AngleDiff(bitmaps[index].angle, position->GetAngle())) {
				index = i;
			}
		}
		oapiBlt(drawSurface, switchSurface, x, y, (bitmaps[index].xOffset * width) + 130, bitmaps[index].yOffset * height, width - 130, height, SURF_PREDEF_CK);
	}
}

void SuitTestSwitch::DrawFlash(SURFHANDLE DrawSurface)
{
	if (!visible)
		return;

	if (switchBorder)
		oapiBlt(DrawSurface, switchBorder, x, y, 0, 0, width - 130, height, SURF_PREDEF_CK);
}

bool SuitTestSwitch::CheckMouseClick(int event, int mx, int my) 
{	
	return RotationalSwitch::CheckMouseClick(event, mx + 130, my);
}


double SaturnOxygenRepressPressMeter::QueryValue()
{
	AtmosStatus atm;
	Sat->GetAtmosStatus(atm);

	return atm.O2RepressPressurePSI;
}

void SaturnOxygenRepressPressMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface)
{
	v = 115.0 - v / 1200.0 * 50.0 ;	
	DrawNeedle(drawSurface, 55, 440, 60.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 413, 0, 110, 90, 30, SURF_PREDEF_CK);
}

// LM power switch
void CSMLMPowerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, Saturn *s){
	sat = s;
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
}

void CSMLMPowerSwitch::LoadState(char *line)
{
	ToggleSwitch::LoadState(line);

	switch (state)
	{
	case THREEPOSSWITCH_UP:
		sat->CSMToLEMPowerDrain.Enable();
		sat->CSMToLEMPowerDrain.WireTo(&sat->LMUmbilicalFeeder);
		break;
	default:
		sat->CSMToLEMPowerDrain.Disable();
		sat->CSMToLEMPowerDrain.WireTo(NULL);
		break;
	}

}

bool CSMLMPowerSwitch::SwitchTo(int newState, bool dontspring)
{
	if (SaturnThreePosSwitch::SwitchTo(newState, dontspring)) {
		// First -- Are we docked?
		if (sat->dockingprobe.IsHardDocked() == false) { return true; }
		// Umbilical connected? (Supposed to be after LM Pressurization and hatch removal)
		if (sat->CSMToLEMConnector.connectedTo == NULL) { return true; }
		ConnectorMessage msg;
		ConnectorMessageValue mval;
		msg.destination = LEM_CSM_POWER;
		msg.messageType = 42;
		switch (state) {
		case THREEPOSSWITCH_UP:
			// Connect the bus
			sat->CSMToLEMPowerDrain.Enable();
			sat->CSMToLEMPowerDrain.WireTo(&sat->LMUmbilicalFeeder);
			// Turn on LEM
			mval.iValue = 1; // Relay State 1 = Deny Descent ECA operation
			msg.val1 = mval;
			sat->CSMToLEMConnector.SendMessage(msg);
			break;
		case THREEPOSSWITCH_CENTER:
			// Turn off LEM (but don't disconnect it)
			sat->CSMToLEMPowerDrain.Disable();
			sat->CSMToLEMPowerDrain.WireTo(NULL);
			break;
		case THREEPOSSWITCH_DOWN:
			// Reset LEM
			mval.iValue = 0; // Relay State 0 = Permit Descent ECA operation
			msg.val1 = mval;
			sat->CSMToLEMConnector.SendMessage(msg);
			// Ensure disconnected
			sat->CSMToLEMPowerDrain.Disable();
			sat->CSMToLEMPowerDrain.WireTo(NULL);
			break;
		}
		return true;
	}
	return false;
}

double SaturnHighGainAntennaPitchMeter::QueryValue(){
	return Sat->hga.GetResolvedPitch();
}

void SaturnHighGainAntennaPitchMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface){
	// Gauge runs from -110 to 110 degrees. Sign matches.
	v = v * 1.222222;
	DrawNeedle(drawSurface, 30, 25, 20.0, v * RAD);
}

double SaturnHighGainAntennaStrengthMeter::QueryValue(){
	return Sat->usb.rcvr_agc_voltage;
}

void SaturnHighGainAntennaStrengthMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface){
	// Gauge runs from 180 to 0. Sign reversed.
	v = 180-(v*1.8);
	DrawNeedle(drawSurface, 107, 25, 20.0, v * RAD);
}

double SaturnHighGainAntennaYawMeter::QueryValue(){
	return Sat->hga.GetResolvedYaw();
}

void SaturnHighGainAntennaYawMeter::DoDrawSwitch(double v, SURFHANDLE drawSurface){
	// Gauge runs from 220 to -40. Sign reversed.
	v = 220-(v*0.722222);
	DrawNeedle(drawSurface, 185, 25, 20.0, v * RAD);
}

double SaturnLMDPGauge::QueryValue(){
	if (Sat->LMTunnelVentValve.GetState() == 2)
	{
		AtmosStatus atm;
		Sat->GetAtmosStatus(atm);

		return atm.CabinPressurePSI - atm.TunnelPressurePSI;
	}

	return -1.0;
}

void SaturnLMDPGauge::DoDrawSwitch(double v, SURFHANDLE drawSurface){
	// Gauge runs from 122.5 to 57.5, sign reversed.
	v = 122.5-((v+1)*13);	
	DrawNeedle(drawSurface, 43, 77, 20.0, v * RAD);
}

void SaturnLMDPGauge::DrawNeedle (SURFHANDLE surf, int x, int y, double rad, double angle){
	// Needle function by Rob Conley from Mercury code
	// This one needs a longer and offset needle

	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;

	HDC hDC = oapiGetDC (surf);
	oldObj = SelectObject (hDC, Pen1);
	MoveToEx (hDC, x + (int)(2*dx+0.5), y - (int)(2*dy+0.5), 0); 
	LineTo (hDC, x + (int)(3*dx+0.5), y - (int)(3*dy+0.5));
	SelectObject (hDC, oldObj);
	oapiReleaseDC (surf, hDC);
}

// Right Docking Target Switch
bool DockingTargetSwitch::SwitchTo(int newState, bool dontspring)
{
	if (SaturnThreePosSwitch::SwitchTo(newState, dontspring)) {
		switch (state) {
		case THREEPOSSWITCH_UP:  // BRIGHT
			sat->CMdocktgt = true;
			break;
		case THREEPOSSWITCH_CENTER:  // DIM
			sat->CMdocktgt = true;
			break;
		case THREEPOSSWITCH_DOWN:  // OFF
			sat->CMdocktgt = false;
			break;
		}
		sat->SetCMdocktgtMesh();
		//sprintf(oapiDebugString(), "Flag %d, Index number %d", sat->CMdocktgt, sat->cmdocktgtidx);
		return true;
	}
	return false;
}

SaturnLiftoffNoAutoAbortSwitch::SaturnLiftoffNoAutoAbortSwitch()
{
	secs = NULL;
}

void SaturnLiftoffNoAutoAbortSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, SECS *s,
	int xoffset, int yoffset, int lxoffset, int lyoffset)
{
	GuardedPushSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset, lxoffset, lyoffset);

	secs = s;
}

void SaturnLiftoffNoAutoAbortSwitch::DoDrawSwitch(SURFHANDLE drawSurface)
{
	if (secs->LiftoffLightPower()) {
		if (!secs->NoAutoAbortLightPower())
			SetOffset(78, 81);
		else
			SetOffset(234, 81);
	}
	else {
		SetOffset(0, 81);
	}

	GuardedPushSwitch::DoDrawSwitch(drawSurface);
}

void SaturnPanel181::Register(PanelSwitchScenarioHandler *PSH)
{
	SMSector1Cryo3ACPowerSwitch.Register(*PSH, "SMSector1Cryo3ACPowerSwitch", TOGGLESWITCH_UP);
	SMSector1SMACPowerSwitch.Register(*PSH, "SMSector1SMACPowerSwitch", TOGGLESWITCH_UP);
	SMSector1AC2ASystemBraker.Register(*PSH, "SMSector1AC2ASystemBraker", 1);
	SMSector1AC2BSystemBraker.Register(*PSH, "SMSector1AC2BSystemBraker", 1);
	SMSector1AC2CSystemBraker.Register(*PSH, "SMSector1AC2CSystemBraker", 1);
	SMSector1DoorJettisonSwitch.Register(*PSH, "SMSector1DoorJettisonSwitch", TOGGLESWITCH_DOWN, false, SPRINGLOADEDSWITCH_DOWN);
	SMSector1LogicPower1Switch.Register(*PSH, "SMSector1LogicPower1Switch", THREEPOSSWITCH_CENTER);
	SMSector1LogicPower2Switch.Register(*PSH, "SMSector1LogicPower2Switch", THREEPOSSWITCH_CENTER);
	SMSector1LogicPowerMNABraker.Register(*PSH, "SMSector1LogicPowerMNABraker", 0);
	SMSector1LogicPowerMNBBraker.Register(*PSH, "SMSector1LogicPowerMNBBraker", 0);
}

void SaturnPanel277::Register(PanelSwitchScenarioHandler *PSH)
{
	SPSPressIndFuelSwitch.Register(*PSH, "SPSPressIndFuelSwitch", TOGGLESWITCH_UP);
	SPSPressIndFuelSwitch.SetSideways(1);
	SPSPressIndOxidSwitch.Register(*PSH, "SPSPressIndOxidSwitch", TOGGLESWITCH_UP);
	SPSPressIndOxidSwitch.SetSideways(1);
	CSMLMFinalSepABatABraker.Register(*PSH, "CSMLMFinalSepABatABraker", 1);
	CSMLMFinalSepBBatBBraker.Register(*PSH, "CSMLMFinalSepBBatBBraker", 1);
}

void SaturnPanel278J::Register(PanelSwitchScenarioHandler *PSH)
{
	ExperimentCovers1Switch.Register(*PSH, "ExperimentCovers1Switch", THREEPOSSWITCH_CENTER);
	ExperimentCovers1Switch.SetSideways(2);
	ExperimentCovers2Switch.Register(*PSH, "ExperimentCovers2Switch", THREEPOSSWITCH_CENTER);
	ExperimentCovers2Switch.SetSideways(2);
	SMPowerSourceSwitch.Register(*PSH, "SMPowerSourceSwitch", THREEPOSSWITCH_DOWN, false);
	SMPowerSourceSwitch.SetSideways(2);
	SMPowerSourceSwitch.SetGuardResetsState(false);
	O2Tank3IsolSwitch.Register(*PSH, "O2Tank3IsolSwitch", THREEPOSSWITCH_CENTER);
	O2Tank3IsolSwitch.SetSideways(2);
	ExperimentCovers1Indicator.Register(*PSH, "ExperimentCovers1Indicator", false);
	ExperimentCovers2Indicator.Register(*PSH, "ExperimentCovers2Indicator", false);
	O2Tank3IsolIndicator.Register(*PSH, "O2Tank3IsolIndicator", false);
	ExperimentCoversDeployBraker.Register(*PSH, "ExperimentCoversDeployBraker", 1);
}