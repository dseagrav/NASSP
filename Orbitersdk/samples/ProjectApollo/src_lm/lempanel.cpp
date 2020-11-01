/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Jean-Luc Rocca-Serra, Mark Grant, Matthias M?Eler

  ORBITER vessel module: LEM panel code

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
#include "stdio.h"
#include "math.h"
#include "lmresource.h"

#include "nasspdefs.h"
#include "nasspsound.h"

#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "lm_channels.h"
#include "LEMcomputer.h"
#include "dsky.h"
#include "Mission.h"

#include "LEM.h"
 
#define LOADBMP(id) (LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (id)))

static GDIParams g_Param;

void InitGParam(HINSTANCE hModule)

{
	g_Param.hDLL = hModule;

	// allocate GDI resources
	g_Param.font[0]  = CreateFont (-13, 0, 0, 0, 700, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
	g_Param.font[1]  = CreateFont (-10, 0, 0, 0, 400, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");

	g_Param.brush[0] = CreateSolidBrush (RGB(0,255,0));    // green
	g_Param.brush[1] = CreateSolidBrush (RGB(255,0,0));    // red
	g_Param.brush[2] = CreateSolidBrush (RGB(154,154,154));  // Grey
	g_Param.brush[3] = CreateSolidBrush (RGB(3,3,3));  // Black

	g_Param.pen[0] = CreatePen (PS_SOLID, 1, RGB(224,224,224));
	g_Param.pen[1] = CreatePen (PS_SOLID, 3, RGB(164,164,164));
	g_Param.pen[2] = CreatePen (PS_SOLID, 1, RGB(255,0,0));
	g_Param.pen[3] = CreatePen (PS_SOLID, 3, RGB(255,0,0));
	g_Param.pen[4] = CreatePen (PS_SOLID, 3, RGB(0,0,0));

	g_Param.col[2] = oapiGetColour(154,154,154);
	g_Param.col[3] = oapiGetColour(3,3,3);
	g_Param.col[4] = oapiGetColour(255,0,255);
}

void FreeGParam()

{
	int i;
	// deallocate GDI resources
	for (i = 0; i < 2; i++) DeleteObject (g_Param.font[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.brush[i]);
	for (i = 0; i < 4; i++) DeleteObject (g_Param.pen[i]);
}

#define RETICLE_X_CENTER 525
#define RETICLE_Y_CENTER 525
#define RETICLE_RADIUS   525
#define RETICLE_SPLIT_ANGLE 0.05 // about 2.25 degrees
#define RETICLE_SCREW_NPTS 360

void DrawReticle (HDC hDC, double angle, int dimmer)
{
	HGDIOBJ oldObj;
	int xend,yend;
	// Set up Dimmer Pen
	HPEN pen = CreatePen(PS_SOLID,1,RGB(dimmer,64,64));
	oldObj = SelectObject (hDC, pen);
	// Draw crosshair vertical member
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle+PI));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle+PI));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	// Draw crosshair horizontal member
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * cos(angle));
	yend = RETICLE_Y_CENTER + (int)(RETICLE_RADIUS * sin(angle));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * cos(angle+PI));
	yend = RETICLE_Y_CENTER + (int)(RETICLE_RADIUS * sin(angle+PI));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	// Draw radial line #1
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle + RETICLE_SPLIT_ANGLE));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle + RETICLE_SPLIT_ANGLE));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	// Draw radial line #2
	xend = RETICLE_X_CENTER - (int)(RETICLE_RADIUS * sin(angle - RETICLE_SPLIT_ANGLE));
	yend = RETICLE_Y_CENTER - (int)(RETICLE_RADIUS * cos(angle - RETICLE_SPLIT_ANGLE));
	MoveToEx (hDC, RETICLE_X_CENTER, RETICLE_Y_CENTER, 0); LineTo(hDC, xend, yend);
	int i;
	double theta,b, r;
	b = -RETICLE_RADIUS / tan(PI2*30.0 / 360.0);
	POINT ScrewPt[RETICLE_SCREW_NPTS];
	// Draw Archemedes screw #1
	for (i = 0; i < RETICLE_SCREW_NPTS; i++){
		theta = 2*PI / RETICLE_SCREW_NPTS * i;
		r = b * tan(theta*30.0 / 360.0);
		ScrewPt[i].x = RETICLE_X_CENTER - (int)(r*sin(theta+angle+RETICLE_SPLIT_ANGLE+PI));
		ScrewPt[i].y = RETICLE_Y_CENTER - (int)(r*cos(theta+angle+RETICLE_SPLIT_ANGLE+PI));
	}
	Polyline (hDC, ScrewPt, RETICLE_SCREW_NPTS);
	// Draw Archemedes screw #2
	for (i = 0; i < RETICLE_SCREW_NPTS; i++){
		theta = 2*PI / RETICLE_SCREW_NPTS * i;
		r = b * tan(theta*30.0 / 360.0);
		ScrewPt[i].x = RETICLE_X_CENTER - (int)(r*sin(theta+angle-RETICLE_SPLIT_ANGLE+PI));
		ScrewPt[i].y = RETICLE_Y_CENTER - (int)(r*cos(theta+angle-RETICLE_SPLIT_ANGLE+PI));
	}
	Polyline (hDC, ScrewPt, RETICLE_SCREW_NPTS);

	SelectObject(hDC, oldObj);
	DeleteObject(pen);
}

void LEM::RedrawPanel_AOTReticle(SURFHANDLE surf)
{
	HDC hDC = oapiGetDC (surf);
	DrawReticle (hDC, optics.OpticsReticle, optics.RetDimmer);
	oapiReleaseDC (surf,hDC);

}


//
// Initialise panel to default state.
//

void LEM::InitSwitches() {

	AbortSwitch.Register     (PSH, "AbortSwitch", true);
	AbortStageSwitch.Register(PSH, "AbortStageSwitch", true, false);
	AbortStageSwitch.SetGuardResetsState(false);

	EngineArmSwitch.Register(PSH, "EngineArmSwitch", THREEPOSSWITCH_CENTER);
	EngineDescentCommandOverrideSwitch.Register(PSH, "EngineDescentCommandOverrideSwitch", TOGGLESWITCH_DOWN);
	ModeControlPGNSSwitch.Register(PSH,"ModeControlPGNSSwitch", THREEPOSSWITCH_DOWN);
	ModeControlAGSSwitch.Register(PSH,"ModeControlAGSSwitch", THREEPOSSWITCH_DOWN);
	IMUCageSwitch.Register(PSH,"IMUCageSwitch", TOGGLESWITCH_DOWN);
	LeftXPointerSwitch.Register(PSH, "LeftXPointerSwitch", true);
	GuidContSwitch.Register(PSH, "GuidContSwitch", true);
	ModeSelSwitch.Register(PSH, "ModeSelSwitch", THREEPOSSWITCH_UP);
	AltRngMonSwitch.Register(PSH, "AltRngMonSwitch", TOGGLESWITCH_DOWN);
	RateErrorMonSwitch.Register(PSH, "RateErrorMonSwitch", TOGGLESWITCH_DOWN);
	AttitudeMonSwitch.Register(PSH, "AttitudeMonSwitch", true);
	ASCHeReg1TB.Register(PSH,"ASCHeReg1TB", true);
	ASCHeReg2TB.Register(PSH,"ASCHeReg2TB", true);
	DESHeReg1TB.Register(PSH,"DESHeReg1TB", true);
	DESHeReg2TB.Register(PSH,"DESHeReg2TB", false);
	ASCHeReg1Switch.Register(PSH,"ASCHeReg1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCHeReg1Switch.SetCallback(new PanelSwitchCallback<APSValve>(APSPropellant.GetHeliumValve1(), &APSValve::SwitchToggled));
	ASCHeReg2Switch.Register(PSH,"ASCHeReg2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCHeReg2Switch.SetCallback(new PanelSwitchCallback<APSValve>(APSPropellant.GetHeliumValve2(), &APSValve::SwitchToggled));
	DESHeReg1Switch.Register(PSH,"DESHeReg1Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DESHeReg1Switch.SetCallback(new PanelSwitchCallback<DPSValve>(DPSPropellant.GetHeliumValve1(), &DPSValve::SwitchToggled));
	DESHeReg2Switch.Register(PSH,"DESHeReg2Switch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DESHeReg2Switch.SetCallback(new PanelSwitchCallback<DPSValve>(DPSPropellant.GetHeliumValve2(), &DPSValve::SwitchToggled));
	ShiftTruSwitch.Register(PSH, "ShiftTruSwitch", true);
	RateScaleSwitch.Register(PSH, "RateScaleSwitch", TOGGLESWITCH_UP);
	ACAPropSwitch.Register(PSH, "ACAPropSwitch", true);
	RCSAscFeed1ATB.Register(PSH, "RCSAscFeed1ATB", false);
	RCSAscFeed2ATB.Register(PSH, "RCSAscFeed2ATB", false);
	RCSAscFeed1BTB.Register(PSH, "RCSAscFeed1BTB", false);
	RCSAscFeed2BTB.Register(PSH, "RCSAscFeed2BTB", false);
	RCSAscFeed1ASwitch.Register(PSH, "RCSAscFeed1ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSAscFeed1ASwitch.SetCallback(new PanelSwitchCallback<RCSPropellantSource>(&RCSA, &RCSPropellantSource::PrimInterconnectToggled));
	RCSAscFeed2ASwitch.Register(PSH, "RCSAscFeed2ASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSAscFeed2ASwitch.SetCallback(new PanelSwitchCallback<RCSPropellantSource>(&RCSA, &RCSPropellantSource::SecInterconnectToggled));
	RCSAscFeed1BSwitch.Register(PSH, "RCSAscFeed1BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSAscFeed1BSwitch.SetCallback(new PanelSwitchCallback<RCSPropellantSource>(&RCSB, &RCSPropellantSource::PrimInterconnectToggled));
	RCSAscFeed2BSwitch.Register(PSH, "RCSAscFeed2BSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSAscFeed2BSwitch.SetCallback(new PanelSwitchCallback<RCSPropellantSource>(&RCSB, &RCSPropellantSource::SecInterconnectToggled));
	RCSQuad1ACmdEnableTB.Register(PSH, "RCSQuad1ACmdEnableTB", true);
	RCSQuad4ACmdEnableTB.Register(PSH, "RCSQuad4ACmdEnableTB", true);
	RCSQuad1BCmdEnableTB.Register(PSH, "RCSQuad1BCmdEnableTB", true);
	RCSQuad4BCmdEnableTB.Register(PSH, "RCSQuad4BCmdEnableTB", true);
	RCSQuad1ACmdEnableSwitch.Register(PSH, "RCSQuad1ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad1ACmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSA.GetQuad1IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSQuad4ACmdEnableSwitch.Register(PSH, "RCSQuad4ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad4ACmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSA.GetQuad4IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSQuad1BCmdEnableSwitch.Register(PSH, "RCSQuad1BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad1BCmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSB.GetQuad1IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSQuad4BCmdEnableSwitch.Register(PSH, "RCSQuad4BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad4BCmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSB.GetQuad4IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSQuad2ACmdEnableTB.Register(PSH, "RCSQuad2ACmdEnableTB", true);
	RCSQuad3ACmdEnableTB.Register(PSH, "RCSQuad3ACmdEnableTB", true);
	RCSQuad2BCmdEnableTB.Register(PSH, "RCSQuad2BCmdEnableTB", true);
	RCSQuad3BCmdEnableTB.Register(PSH, "RCSQuad3BCmdEnableTB", true);
	RCSQuad2ACmdEnableSwitch.Register(PSH, "RCSQuad2ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad2ACmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSA.GetQuad2IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSQuad3ACmdEnableSwitch.Register(PSH, "RCSQuad3ACmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad3ACmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSA.GetQuad3IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSQuad2BCmdEnableSwitch.Register(PSH, "RCSQuad2BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad2BCmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSB.GetQuad2IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSQuad3BCmdEnableSwitch.Register(PSH, "RCSQuad3BCmdEnableSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSQuad3BCmdEnableSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSB.GetQuad3IsolationValve(), &LEMRCSValve::SwitchToggled));
	RCSXFeedTB.Register(PSH, "RCSXFeedTB", false);
	RCSXFeedSwitch.Register(PSH, "RCSXFeedSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSXFeedSwitch.SetCallback(new PanelSwitchCallback<RCSPropellantSource>(&RCSB, &RCSPropellantSource::CrossfeedToggled));
	RCSMainSovATB.Register(PSH, "RCSMainSovATB", true);
	RCSMainSovBTB.Register(PSH, "RCSMainSovBTB", true);
	RCSMainSovASwitch.Register(PSH, "RCSMainSOVASwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSMainSovASwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSA.GetMainShutoffValve(), &LEMRCSValve::SwitchToggled));
	RCSMainSovBSwitch.Register(PSH, "RCSMainSOVBSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RCSMainSovBSwitch.SetCallback(new PanelSwitchCallback<LEMRCSValve>(RCSB.GetMainShutoffValve(), &LEMRCSValve::SwitchToggled));
	THRContSwitch.Register(PSH, "THRContSwitch", true);
	MANThrotSwitch.Register(PSH, "MANThrotSwitch", true);
	ATTTranslSwitch.Register(PSH, "ATTTranslSwitch", TOGGLESWITCH_DOWN);
	BALCPLSwitch.Register(PSH, "BALCPLSwitch", true);
	QTYMonSwitch.Register(PSH, "QTYMonSwitch", THREEPOSSWITCH_DOWN);
	TempPressMonSwitch.Register(PSH, "TempPressMonSwitch", THREEPOSSWITCH_UP);
	RightRateErrorMonSwitch.Register(PSH, "RightRateErrorMonSwitch", TOGGLESWITCH_DOWN);
	RightAttitudeMonSwitch.Register(PSH, "RightAttitudeMonSwitch", TOGGLESWITCH_DOWN);
	RightACAPropSwitch.Register(PSH, "RightACAPropSwitch", TOGGLESWITCH_UP);
	LandingAntSwitch.Register(PSH, "LandingAntSwitch", THREEPOSSWITCH_UP);
	EngGimbalEnableSwitch.Register(PSH, "EngGimbalEnableSwitch", TOGGLESWITCH_UP);
	RadarTestSwitch.Register(PSH, "RadarTestSwitch",  THREEPOSSWITCH_CENTER);
	RadarSignalStrengthMeter.Register(PSH, "RadarSignalStrengthMeter", 0.0, 5.0, 3);
	SlewRateSwitch.Register(PSH, "SlewRateSwitch", true);
	DeadBandSwitch.Register(PSH, "DeadBandSwitch", false);
	LMSuitTempMeter.Register(PSH,"LMSuitTempMeter",40,100,2);
	LMCabinTempMeter.Register(PSH,"LMCabinTempMeter",40,100,2);
	LMSuitPressMeter.Register(PSH,"LMSuitPressMeter",0,10,2);
	LMCabinPressMeter.Register(PSH,"LMCabinPressMeter",0,10,2);
	LMCO2Meter.Register(PSH,"LMCO2Meter",0,30,2);
	LMGlycolTempMeter.Register(PSH,"LMGlycolTempMeter",0,80,2);
	LMGlycolPressMeter.Register(PSH,"LMGlycolPressMeter",0,80,2);
	LMOxygenQtyMeter.Register(PSH,"LMOxygenQtyMeter",0,100,2);
	LMWaterQtyMeter.Register(PSH,"LMWaterQtyMeter",0,100,2);
	LMRCSATempInd.Register(PSH,"LMRCSATempInd",20,120,2);
	LMRCSBTempInd.Register(PSH,"LMRCSBTempInd",20,120,2);
	LMRCSAPressInd.Register(PSH,"LMRCSAPressInd",0,400,2);
	LMRCSBPressInd.Register(PSH,"LMRCSBPressInd",0,400,2);
	LMRCSAQtyInd.Register(PSH,"LMRCSAQtyInd",0,100,2);
	LMRCSBQtyInd.Register(PSH,"LMRCSBQtyInd",0,100,2);
	TempMonitorInd.Register(PSH,"TempMonitorInd",-100.0,200,2);
	EngineThrustInd.Register(PSH,"EngineThrustInd",0,100,2);
	CommandedThrustInd.Register(PSH,"CommandedThrustInd",0,100,2);
	ThrustWeightInd.Register(PSH, "ThrustWeightInd", 0, 6, 2);
	MainFuelTempInd.Register(PSH,"MainFuelTempInd",40,100,2);
	MainFuelPressInd.Register(PSH,"MainFuelPressInd",0,300,2);
	MainOxidizerTempInd.Register(PSH,"MainOxidizerTempInd",40,100,2);
	MainOxidizerPressInd.Register(PSH,"MainOxidizerPressInd",0,300,2);
	DPSOxidPercentMeter.Register(PSH, "DPSOxidPercentMeter", 0, 0.99, 0, 0);
	DPSFuelPercentMeter.Register(PSH, "DPSFuelPercentMeter", 0, 0.99, 0, 0);
	MainHeliumPressureMeter.Register(PSH, "MainHeliumPressureMeter", 0, 9999, 0, 0);

	GyroTestLeftSwitch.Register(PSH, "GyroTestLeftSwitch",  THREEPOSSWITCH_UP);
	GyroTestRightSwitch.Register(PSH, "GyroTestRightSwitch",  THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	RollSwitch.Register(PSH, "RollSwitch",  THREEPOSSWITCH_UP);
	PitchSwitch.Register(PSH, "PitchSwitch",  THREEPOSSWITCH_UP);
	YawSwitch.Register(PSH, "YawSwitch",  THREEPOSSWITCH_UP);
	RCSSysQuad1Switch.Register(PSH, "RCSSysQuad1Switch",  THREEPOSSWITCH_CENTER);
	RCSSysQuad2Switch.Register(PSH, "RCSSysQuad2Switch",  THREEPOSSWITCH_CENTER);
	RCSSysQuad3Switch.Register(PSH, "RCSSysQuad3Switch",  THREEPOSSWITCH_CENTER);
	RCSSysQuad4Switch.Register(PSH, "RCSSysQuad4Switch",  THREEPOSSWITCH_CENTER);
	SidePanelsSwitch.Register(PSH, "SidePanelsSwitch", false);
	FloodSwitch.Register(PSH, "FloodSwitch", THREEPOSSWITCH_CENTER);
	RightXPointerSwitch.Register(PSH, "RightXPointerSwitch", true);
	ExteriorLTGSwitch.Register(PSH, "ExteriorLTGSwitch", THREEPOSSWITCH_UP);
	LeftACA4JetSwitch.Register(PSH, "LeftACA4JetSwitch", TOGGLESWITCH_UP);
	LeftTTCATranslSwitch.Register(PSH, "LeftTTCATranslSwitch", TOGGLESWITCH_UP);
	RightACA4JetSwitch.Register(PSH, "RightACA4JetSwitch", TOGGLESWITCH_UP);
	RightTTCATranslSwitch.Register(PSH, "RightTTCATranslSwitch", TOGGLESWITCH_UP);
	RadarSlewSwitch.Register(PSH,"RadarSlewSwitch",FIVEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EventTimerCtlSwitch.Register(PSH,"EventTimerCtlSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER_SPRINGUP);
	EventTimerStartSwitch.Register(PSH,"EventTimerStartSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EventTimerMinuteSwitch.Register(PSH,"EventTimerMinuteSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EventTimerSecondSwitch.Register(PSH,"EventTimerSecondSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);

	HeliumMonRotary.AddPosition(0, 290);
	HeliumMonRotary.AddPosition(1, 315);
	HeliumMonRotary.AddPosition(2, 340);
	HeliumMonRotary.AddPosition(3,  20);
	HeliumMonRotary.AddPosition(4,  45);
	HeliumMonRotary.AddPosition(5,  70);
	HeliumMonRotary.AddPosition(6, 110);
	HeliumMonRotary.Register(PSH, "HeliumMonRotary", 0);

	TempPressMonRotary.AddPosition(0, 340);
	TempPressMonRotary.AddPosition(1,  20);
	TempPressMonRotary.AddPosition(2,  45);
	TempPressMonRotary.AddPosition(3,  70);
	TempPressMonRotary.Register(PSH, "TempPressMonRotary", 0);

	GlycolRotary.AddPosition(0,  45);
	GlycolRotary.AddPosition(1,  70);
	GlycolRotary.AddPosition(2, 110);
	GlycolRotary.Register(PSH, "GlycolRotary", 2);
	
	SuitFanRotary.AddPosition(0,  45);
	SuitFanRotary.AddPosition(1,  70);
	SuitFanRotary.AddPosition(2, 110);
	SuitFanRotary.Register(PSH, "SuitFanRotary", 1);

	QtyMonRotary.AddPosition(0, 340);
	QtyMonRotary.AddPosition(1,  20);
	QtyMonRotary.AddPosition(2,  45);
	QtyMonRotary.AddPosition(3,  70);
	QtyMonRotary.Register(PSH, "QtyMonRotary", 3);

	TestMonitorRotary.AddPosition(0, 315);
	TestMonitorRotary.AddPosition(1, 340);
	TestMonitorRotary.AddPosition(2,  20);
	TestMonitorRotary.AddPosition(3,  45);
	TestMonitorRotary.AddPosition(4,  70);
	TestMonitorRotary.AddPosition(5, 110);
	TestMonitorRotary.Register(PSH, "TestMonitorRotary", 0);

	RendezvousRadarRotary.AddPosition(0, 330);
	RendezvousRadarRotary.AddPosition(1,   0);
	RendezvousRadarRotary.AddPosition(2,  30);
	RendezvousRadarRotary.Register(PSH, "RendezvousRadarRotary", 1);

	TempMonitorRotary.AddPosition(0, 315);
	TempMonitorRotary.AddPosition(1, 340);
	TempMonitorRotary.AddPosition(2,  20);
	TempMonitorRotary.AddPosition(3,  45);
	TempMonitorRotary.AddPosition(4,  70);
	TempMonitorRotary.AddPosition(5, 110);
	TempMonitorRotary.AddPosition(6, 135);
	TempMonitorRotary.Register(PSH, "TempMonitorRotary", 1);

	FloodRotary.AddPosition(0, 240);
	FloodRotary.AddPosition(1, 270);
	FloodRotary.AddPosition(2, 300);
	FloodRotary.AddPosition(3, 330);
	FloodRotary.AddPosition(4, 360);
	FloodRotary.AddPosition(5,  30);
	FloodRotary.AddPosition(6,  60);
	FloodRotary.AddPosition(7,  90);
	FloodRotary.AddPosition(8, 120);
	FloodRotary.Register(PSH, "FloodRotary", 8);

	LampToneTestRotary.AddPosition(0, 250);
	LampToneTestRotary.AddPosition(1, 290);
	LampToneTestRotary.AddPosition(2, 315);
	LampToneTestRotary.AddPosition(3, 340);
	LampToneTestRotary.AddPosition(4,  20);
	LampToneTestRotary.AddPosition(5,  45);
	LampToneTestRotary.AddPosition(6,  70);
	LampToneTestRotary.AddPosition(7, 110);
	LampToneTestRotary.Register(PSH, "LampToneTestRotary", 0);

	EPSMonitorSelectRotary.AddPosition(0,210);
	EPSMonitorSelectRotary.AddPosition(1,240);
	EPSMonitorSelectRotary.AddPosition(2,270);
	EPSMonitorSelectRotary.AddPosition(3,300);
	EPSMonitorSelectRotary.AddPosition(4,330);
	EPSMonitorSelectRotary.AddPosition(5,0);
	EPSMonitorSelectRotary.AddPosition(6,30);
	EPSMonitorSelectRotary.AddPosition(7,60);
	EPSMonitorSelectRotary.AddPosition(8,90);
	EPSMonitorSelectRotary.AddPosition(9,120);
	EPSMonitorSelectRotary.Register(PSH, "EPSMonitorSelectRotary", 0);

	EPSEDVoltSelect.Register(PSH, "EPSEDVoltSelect", THREEPOSSWITCH_CENTER);
	EPSEDVoltSelect.SetSource(0, EDBatteryA);
	EPSEDVoltSelect.SetSource(2, EDBatteryB);

	EPSDCVoltMeter.Register(PSH,"EPSDCVoltMeter", 19, 42, 3);
	EPSDCAmMeter.Register(PSH,"EPSDCAmMeter", 0, 120, 3);
	DSCBattery1TB.Register(PSH, "DSCBattery1TB", 2);
	DSCBattery2TB.Register(PSH, "DSCBattery2TB", 0);
	DSCBattery3TB.Register(PSH, "DSCBattery3TB", 0);
	DSCBattery4TB.Register(PSH, "DSCBattery4TB", 2);
	ASCBattery5ATB.Register(PSH, "ASCBattery5ATB", 0);
	ASCBattery5BTB.Register(PSH, "ASCBattery5BTB", 0);
	ASCBattery6ATB.Register(PSH, "ASCBattery6ATB", 0);
	ASCBattery6BTB.Register(PSH, "ASCBattery6BTB", 0);
	DSCBattFeedTB.Register(PSH, "DSCBattFeedTB", 1);
	EPSInverterSwitch.Register(PSH,"EPSInverterSwitch",THREEPOSSWITCH_DOWN);
	DSCSEBat1HVSwitch.Register(PSH, "DSCSEBat1HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCSEBat2HVSwitch.Register(PSH, "DSCSEBat2HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat3HVSwitch.Register(PSH, "DSCCDRBat3HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat4HVSwitch.Register(PSH, "DSCCDRBat4HVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCSEBat1LVSwitch.Register(PSH, "DSCSEBat1LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCSEBat2LVSwitch.Register(PSH, "DSCSEBat2LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat3LVSwitch.Register(PSH, "DSCCDRBat3LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCCDRBat4LVSwitch.Register(PSH, "DSCCDRBat4LVSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	DSCBattFeedSwitch.Register(PSH, "DSCBattFeedSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat5SESwitch.Register(PSH, "ASCBat5SESwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat5CDRSwitch.Register(PSH, "ASCBat5CDRSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat6CDRSwitch.Register(PSH, "ASCBat6CDRSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	ASCBat6SESwitch.Register(PSH, "ASCBat6SESwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	UpDataSquelchSwitch.Register(PSH,"UpDataSquelchSwitch",TOGGLESWITCH_UP);
	Panel12UpdataLinkSwitch.Register(PSH,"Panel12UpdataLinkSwitch",THREEPOSSWITCH_CENTER);
	Panel12AudioCtlSwitch.Register(PSH,"Panel12AudioCtlSwitch",TOGGLESWITCH_UP);
	Panel12AntTrackModeSwitch.Register(PSH,"Panel12AntTrackModeSwitch",THREEPOSSWITCH_CENTER);

	TimerContSwitch.Register(PSH,"TimerContSwitch",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN);
	TimerSlewHours.Register(PSH,"TimerSlewHours",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	TimerSlewMinutes.Register(PSH,"TimerSlewMinutes",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	TimerSlewSeconds.Register(PSH,"TimerSlewSeconds",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	LtgORideAnunSwitch.Register(PSH,"LtgORideAnunSwitch",TOGGLESWITCH_DOWN);
	LtgORideNumSwitch.Register(PSH,"LtgORideNumSwitch",TOGGLESWITCH_DOWN);
	LtgORideIntegralSwitch.Register(PSH,"LtgORideIntegralSwitch",TOGGLESWITCH_DOWN);
	LtgSidePanelsSwitch.Register(PSH,"LtgSidePanelsSwitch",TOGGLESWITCH_DOWN);

	LtgFloodOhdFwdKnob.AddPosition(0, 240);
	LtgFloodOhdFwdKnob.AddPosition(1, 270);
	LtgFloodOhdFwdKnob.AddPosition(2, 300);
	LtgFloodOhdFwdKnob.AddPosition(3, 330);
	LtgFloodOhdFwdKnob.AddPosition(4, 360);
	LtgFloodOhdFwdKnob.AddPosition(5,  30);
	LtgFloodOhdFwdKnob.AddPosition(6,  60);
	LtgFloodOhdFwdKnob.AddPosition(7,  90);
	LtgFloodOhdFwdKnob.AddPosition(8, 120);
	LtgFloodOhdFwdKnob.Register(PSH, "LtgFloodOhdFwdKnob", 8);

	LtgAnunNumKnob.AddPosition(0, 240);
	LtgAnunNumKnob.AddPosition(1, 270);
	LtgAnunNumKnob.AddPosition(2, 300);
	LtgAnunNumKnob.AddPosition(3, 330);
	LtgAnunNumKnob.AddPosition(4, 360);
	LtgAnunNumKnob.AddPosition(5,  30);
	LtgAnunNumKnob.AddPosition(6,  60);
	LtgAnunNumKnob.AddPosition(7,  90);
	LtgAnunNumKnob.AddPosition(8, 120);
	LtgAnunNumKnob.Register(PSH, "LtgAnunNumKnob", 0);

	LtgIntegralKnob.AddPosition(0, 240);
	LtgIntegralKnob.AddPosition(1, 270);
	LtgIntegralKnob.AddPosition(2, 300);
	LtgIntegralKnob.AddPosition(3, 330);
	LtgIntegralKnob.AddPosition(4, 360);
	LtgIntegralKnob.AddPosition(5,  30);
	LtgIntegralKnob.AddPosition(6,  60);
	LtgIntegralKnob.AddPosition(7,  90);
	LtgIntegralKnob.AddPosition(8, 120);
	LtgIntegralKnob.Register(PSH, "LtgIntegralKnob", 0);

	ManualEngineStart.Register(PSH, "ManualEngineStart", 0);
	CDRManualEngineStop.Register(PSH, "CDRManualEngineStop", 0);
	LMPManualEngineStop.Register(PSH, "LMPManualEngineStop", 0);
	PlusXTranslationButton.Register(PSH, "PlusXTranslationButton", TOGGLESWITCH_DOWN);

	EDMasterArm.Register(PSH,"EDMasterArm",TOGGLESWITCH_DOWN);
	EDDesVent.Register(PSH,"EDDesVent",TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);
	EDASCHeSel.Register(PSH,"EDASCHeSel",THREEPOSSWITCH_CENTER);
	EDDesPrpIsol.Register(PSH,"EDDesPrpIsol",TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);
	EDLGDeploy.Register(PSH,"EDLGDeploy",TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);
	EDHePressRCS.Register(PSH,"EDHePressRCS",TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);
	EDHePressDesStart.Register(PSH,"EDHePressDesStart",TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);
	EDHePressASC.Register(PSH,"EDHePressASC",TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);
	EDStage.Register(PSH,"EDStage", TOGGLESWITCH_DOWN, false, SPRINGLOADEDSWITCH_DOWN);
	EDStageRelay.Register(PSH,"EDStageRelay",TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);
	EDDesFuelVent.Register(PSH,"EDDesFuelVent",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EDDesFuelVent.SetCallback(new PanelSwitchCallback<DPSValve>(DPSPropellant.GetFuelVentValve2(), &DPSValve::SwitchToggled));
	EDDesOxidVent.Register(PSH,"EDDesOxidVent",THREEPOSSWITCH_CENTER,SPRINGLOADEDSWITCH_CENTER);
	EDDesOxidVent.SetCallback(new PanelSwitchCallback<DPSValve>(DPSPropellant.GetOxidVentValve2(), &DPSValve::SwitchToggled));
	EDLGTB.Register(PSH, "EDLGTB", true);
	EDDesFuelVentTB.Register(PSH, "EDDesFuelVentTB", true);
	EDDesOxidVentTB.Register(PSH, "EDDesOxidVentTB", true);

	CDRAudSBandSwitch.Register(PSH,"CDRAudSBandSwitch",THREEPOSSWITCH_CENTER);
	CDRAudICSSwitch.Register(PSH,"CDRAudICSSwitch",THREEPOSSWITCH_CENTER);
	CDRAudRelaySwitch.Register(PSH,"CDRAudRelaySwitch",TOGGLESWITCH_DOWN);
	CDRAudVOXSwitch.Register(PSH,"CDRAudVOXSwitch",THREEPOSSWITCH_CENTER);
	CDRAudioControlSwitch.Register(PSH,"CDRAudioControlSwitch",TOGGLESWITCH_UP);
	CDRAudVHFASwitch.Register(PSH,"CDRAudVHFASwitch",THREEPOSSWITCH_CENTER);
	CDRAudVHFBSwitch.Register(PSH,"CDRAudVHFBSwitch",THREEPOSSWITCH_CENTER);
	CDRAudSBandVol.Register(PSH,"CDRAudSBandVol",6,9);
	CDRAudVHFAVol.Register(PSH,"CDRAudVHFAVol",6,9);
	CDRAudVHFBVol.Register(PSH,"CDRAudVHFBVol",6,9);
	CDRAudICSVol.Register(PSH,"CDRAudICSVol",6,9);
	CDRAudMasterVol.Register(PSH,"CDRAudMasterVol",6,9);
	CDRAudVOXSens.Register(PSH,"CDRAudVOXSens",9,9);
	CDRCOASSwitch.Register(PSH,"CDRCOASSwitch",THREEPOSSWITCH_CENTER);

	LMPAudSBandSwitch.Register(PSH,"LMPAudSBandSwitch",THREEPOSSWITCH_CENTER);
	LMPAudICSSwitch.Register(PSH,"LMPAudICSSwitch",THREEPOSSWITCH_CENTER);
	LMPAudRelaySwitch.Register(PSH,"LMPAudRelaySwitch",TOGGLESWITCH_DOWN);
	LMPAudVOXSwitch.Register(PSH,"LMPAudVOXSwitch",THREEPOSSWITCH_CENTER);
	LMPAudVHFASwitch.Register(PSH,"LMPAudVHFASwitch",THREEPOSSWITCH_CENTER);
	LMPAudVHFBSwitch.Register(PSH,"LMPAudVHFBSwitch",THREEPOSSWITCH_CENTER);
	LMPAudSBandVol.Register(PSH,"LMPAudSBandVol",6,9);
	LMPAudVHFAVol.Register(PSH,"LMPAudVHFAVol",6,9);
	LMPAudVHFBVol.Register(PSH,"LMPAudVHFBVol",6,9);
	LMPAudICSVol.Register(PSH,"LMPAudICSVol",6,9);
	LMPAudMasterVol.Register(PSH,"LMPAudMasterVol",6,9);
	LMPAudVOXSens.Register(PSH,"LMPAudVOXSens",9,9);
	SBandModulateSwitch.Register(PSH,"SBandModulateSwitch",TOGGLESWITCH_UP);
	SBandXCvrSelSwitch.Register(PSH,"SBandXCvrSelSwitch",THREEPOSSWITCH_CENTER);
	SBandPASelSwitch.Register(PSH,"SBandPASelSwitch",THREEPOSSWITCH_CENTER);
	SBandVoiceSwitch.Register(PSH,"SBandVoiceSwitch",THREEPOSSWITCH_CENTER);
	SBandPCMSwitch.Register(PSH,"SBandPCMSwitch",THREEPOSSWITCH_CENTER);
	SBandRangeSwitch.Register(PSH,"SBandRangeSwitch",THREEPOSSWITCH_CENTER);
	VHFAVoiceSwitch.Register(PSH,"VHFAVoiceSwitch",THREEPOSSWITCH_CENTER);
	VHFARcvrSwtich.Register(PSH,"VHFARcvrSwtich",TOGGLESWITCH_DOWN);
	VHFBVoiceSwitch.Register(PSH,"VHFBVoiceSwitch",THREEPOSSWITCH_CENTER);
	VHFBRcvrSwtich.Register(PSH,"VHFBRcvrSwtich",TOGGLESWITCH_DOWN);
	TLMBiomedSwtich.Register(PSH,"TLMBiomedSwtich",THREEPOSSWITCH_CENTER);
	TLMBitrateSwitch.Register(PSH,"TLMBitrateSwitch",TOGGLESWITCH_UP);
	VHFASquelch.Register(PSH,"VHFASquelch",3,9);
	VHFBSquelch.Register(PSH,"VHFBSquelch",3,9);
	TapeRecorderTB.Register(PSH, "TapeRecorderTB", false);
	TapeRecorderSwitch.Register(PSH,"TapeRecorderSwitch",TOGGLESWITCH_DOWN);
	AGSOperateSwitch.Register(PSH,"AGSOperateSwitch",THREEPOSSWITCH_DOWN);

	Panel12VHFAntSelKnob.AddPosition(0, 330);
	Panel12VHFAntSelKnob.AddPosition(1, 0);
	Panel12VHFAntSelKnob.AddPosition(2, 30);
	Panel12VHFAntSelKnob.Register(PSH, "Panel12VHFAntSelKnob", 0);
	Panel12SBandAntSelKnob.AddPosition(0, 300);
	Panel12SBandAntSelKnob.AddPosition(1, 330);
	Panel12SBandAntSelKnob.AddPosition(2, 0);
	Panel12SBandAntSelKnob.AddPosition(3, 30);
	Panel12SBandAntSelKnob.Register(PSH, "Panel12SBandAntSelKnob", 1);
	Panel12AntPitchKnob.AddPosition(0, 195);
	Panel12AntPitchKnob.AddPosition(1, 210);
	Panel12AntPitchKnob.AddPosition(2, 225);
	Panel12AntPitchKnob.AddPosition(3, 240);
	Panel12AntPitchKnob.AddPosition(4, 255);
	Panel12AntPitchKnob.AddPosition(5, 270);
	Panel12AntPitchKnob.AddPosition(6, 285);
	Panel12AntPitchKnob.AddPosition(7, 300);
	Panel12AntPitchKnob.AddPosition(8, 315);
	Panel12AntPitchKnob.AddPosition(9, 330);
	Panel12AntPitchKnob.AddPosition(10,345);
	Panel12AntPitchKnob.AddPosition(11,  0);
	Panel12AntPitchKnob.AddPosition(12, 15);
	Panel12AntPitchKnob.AddPosition(13, 30);
	Panel12AntPitchKnob.AddPosition(14, 45);
	Panel12AntPitchKnob.AddPosition(15, 60);
	Panel12AntPitchKnob.AddPosition(16, 75);
	Panel12AntPitchKnob.AddPosition(17, 90);
	Panel12AntPitchKnob.AddPosition(18,105);
	Panel12AntPitchKnob.AddPosition(19,120);
	Panel12AntPitchKnob.AddPosition(20,135);
	Panel12AntPitchKnob.AddPosition(21,150);
	Panel12AntPitchKnob.AddPosition(22,165);
	Panel12AntPitchKnob.Register(PSH, "Panel12AntPitchKnob", 0);
	Panel12AntYawKnob.AddPosition(0,270);
	Panel12AntYawKnob.AddPosition(1,285);
	Panel12AntYawKnob.AddPosition(2,300);
	Panel12AntYawKnob.AddPosition(3,315);
	Panel12AntYawKnob.AddPosition(4,330);
	Panel12AntYawKnob.AddPosition(5,345);
	Panel12AntYawKnob.AddPosition(6,  0);
	Panel12AntYawKnob.AddPosition(7, 15);
	Panel12AntYawKnob.AddPosition(8, 30);
	Panel12AntYawKnob.AddPosition(9, 45);
	Panel12AntYawKnob.AddPosition(10,60);
	Panel12AntYawKnob.AddPosition(11,75);
	Panel12AntYawKnob.AddPosition(12,90);
	Panel12AntYawKnob.Register(PSH, "Panel12AntYawKnob", 5);

	ComPitchMeter.Register(PSH, "ComPitchMeter", -75, 255, 5, -75);
	Panel12SignalStrengthMeter.Register(PSH, "Panel12SignalStrengthMeter", 0, 100, 5);
	ComYawMeter.Register(PSH, "ComYawMeter", -75, 75, 5, -75);

	// CIRCUIT BREAKERS
	// Panel 11, Row 1
	SE_WND_HTR_AC_CB.Register(PSH,"SE_WND_HTR_AC_CB",0);
	HE_PQGS_PROP_DISP_AC_CB.Register(PSH,"HE_PQGS_PROP_DISP_AC_CB",0);
	SBD_ANT_AC_CB.Register(PSH,"SBD_ANT_AC_CB",0);
	ORDEAL_AC_CB.Register(PSH,"ORDEAL_AC_CB",0);
	AGS_AC_CB.Register(PSH,"AGS_AC_CB",0);
	AOT_LAMP_ACB_CB.Register(PSH,"AOT_LAMP_ACB_CB",0);
	LMP_FDAI_AC_CB.Register(PSH,"LMP_FDAI_AC_CB",0);
	NUM_LTG_AC_CB.Register(PSH,"NUM_LTG_AC_CB",0);
	AC_B_INV_2_FEED_CB.Register(PSH,"AC_B_INV_2_FEED_CB",0);
	AC_B_INV_1_FEED_CB.Register(PSH,"AC_B_INV_1_FEED_CB",0);
	AC_A_INV_2_FEED_CB.Register(PSH,"AC_A_INV_2_FEED_CB",0);
	AC_A_INV_1_FEED_CB.Register(PSH,"AC_A_INV_1_FEED_CB",0);
	AC_A_BUS_VOLT_CB.Register(PSH,"AC_A_BUS_VOLT_CB",0);
	CDR_WND_HTR_AC_CB.Register(PSH,"CDR_WND_HTR_AC_CB",0);
	TAPE_RCDR_AC_CB.Register(PSH,"TAPE_RCDR_AC_CB",0);
	AOT_LAMP_ACA_CB.Register(PSH,"AOT_LAMP_ACA_CB",0);
	RDZ_RDR_AC_CB.Register(PSH,"RDZ_RDR_AC_CB",0);
	DECA_GMBL_AC_CB.Register(PSH,"DECA_GMBL_AC_CB",0);
	INTGL_LTG_AC_CB.Register(PSH,"INTGL_LTG_AC_CB",0);

	// Panel 11, Row 2
	RCS_A_MAIN_SOV_CB.Register(PSH,"RCS_A_MAIN_SOV_CB",0);
	RCS_A_QUAD4_TCA_CB.Register(PSH,"RCS_A_QUAD4_TCA_CB",0);
	RCS_A_QUAD3_TCA_CB.Register(PSH,"RCS_A_QUAD3_TCA_CB",0);
	RCS_A_QUAD2_TCA_CB.Register(PSH,"RCS_A_QUAD2_TCA_CB",0);
	RCS_A_QUAD1_TCA_CB.Register(PSH,"RCS_A_QUAD1_TCA_CB",0);
	RCS_A_ISOL_VLV_CB.Register(PSH,"RCS_A_ISOL_VLV_CB",0);
	RCS_A_ASC_FEED_2_CB.Register(PSH,"RCS_A_ASC_FEED_2_CB",0);
	RCS_A_ASC_FEED_1_CB.Register(PSH,"RCS_A_ASC_FEED_1_CB",0);
	THRUST_DISP_CB.Register(PSH,"THRUST_DISP_CB",0);
	MISSION_TIMER_CB.Register(PSH,"MISSION_TIMER_CB",0);
	CDR_XPTR_CB.Register(PSH,"CDR_XPTR_CB",0);
	RNG_RT_ALT_RT_DC_CB.Register(PSH,"RNG_RT_ALT_RT_DC_CB",0);
	GASTA_DC_CB.Register(PSH,"GASTA_DC_CB",0);	
	CDR_FDAI_DC_CB.Register(PSH,"CDR_FDAI_DC_CB",0);
	COAS_DC_CB.Register(PSH,"COAS_DC_CB",0);
	ORDEAL_DC_CB.Register(PSH,"ORDEAL_DC_CB",0);
	RNG_RT_ALT_RT_AC_CB.Register(PSH,"RNG_RT_ALT_RT_AC_CB",0);
	GASTA_AC_CB.Register(PSH,"GASTA_AC_CB",0); 
	CDR_FDAI_AC_CB.Register(PSH,"CDR_FDAI_AC_CB",0);

	// Panel 11, Row 3
	PROP_DES_HE_REG_VENT_CB.Register(PSH,"PROP_DES_HE_REG_VENT_CB",0);
	HTR_RR_STBY_CB.Register(PSH,"HTR_RR_STBY_CB",1);
	HTR_RR_OPR_CB.Register(PSH,"HTR_RR_OPR_CB",0);
	HTR_LR_CB.Register(PSH,"HTR_LR_CB",1);
	HTR_DOCK_WINDOW_CB.Register(PSH,"HTR_DOCK_WINDOW_CB",0);
	HTR_AOT_CB.Register(PSH,"HTR_AOT_CB",0);
	INST_SIG_CONDR_1_CB.Register(PSH,"INST_SIG_CONDR_1_CB",0);
	CDR_SCS_AEA_CB.Register(PSH,"CDR_SCS_AEA_CB",0);
	CDR_SCS_ABORT_STAGE_CB.Register(PSH,"CDR_SCS_ABORT_STAGE_CB",0);
	CDR_SCS_ATCA_CB.Register(PSH,"CDR_SCS_ATCA_CB",0);
	CDR_SCS_AELD_CB.Register(PSH,"CDR_SCS_AELD_CB",0);
	SCS_ENG_CONT_CB.Register(PSH,"SCS_ENG_CONT_CB",0);
	SCS_ATT_DIR_CONT_CB.Register(PSH,"SCS_ATT_DIR_CONT_CB",0);
	SCS_ENG_START_OVRD_CB.Register(PSH,"SCS_ENG_START_OVRD_CB",0);
	SCS_DECA_PWR_CB.Register(PSH,"SCS_DECA_PWR_CB",0);
	EDS_CB_LG_FLAG.Register(PSH,"EDS_CB_LG_FLAG",0);
	EDS_CB_LOGIC_A.Register(PSH,"EDS_CB_LOGIC_A",0);
	CDR_LTG_UTIL_CB.Register(PSH,"CDR_LTG_UTIL_CB",1);
	CDR_LTG_ANUN_DOCK_COMPNT_CB.Register(PSH,"CDR_LTG_ANUN_DOCK_COMPNT_CB",1);

	// Panel 11, Row 4
	RCS_QUAD_4_CDR_HTR_CB.Register(PSH,"RCS_QUAD_4_CDR_HTR_CB",0);
	RCS_QUAD_3_CDR_HTR_CB.Register(PSH,"RCS_QUAD_3_CDR_HTR_CB",0);
	RCS_QUAD_2_CDR_HTR_CB.Register(PSH,"RCS_QUAD_2_CDR_HTR_CB",0);
	RCS_QUAD_1_CDR_HTR_CB.Register(PSH,"RCS_QUAD_1_CDR_HTR_CB",0);
	ECS_SUIT_FAN_1_CB.Register(PSH,"ECS_SUIT_FAN_1_CB",0);
	ECS_CABIN_FAN_1_CB.Register(PSH,"ECS_CABIN_FAN_1_CB",0);
	ECS_GLYCOL_PUMP_2_CB.Register(PSH,"ECS_GLYCOL_PUMP_2_CB",0);
	ECS_GLYCOL_PUMP_1_CB.Register(PSH,"ECS_GLYCOL_PUMP_1_CB",0);
	ECS_GLYCOL_PUMP_AUTO_XFER_CB.Register(PSH,"ECS_GLYCOL_PUMP_AUTO_XFER_CB",0);
	COMM_UP_DATA_LINK_CB.Register(PSH,"COMM_UP_DATA_LINK_CB",0);
	COMM_SEC_SBAND_XCVR_CB.Register(PSH,"COMM_SEC_SBAND_XCVR_CB",0);
	COMM_SEC_SBAND_PA_CB.Register(PSH,"COMM_SEC_SBAND_PA_CB",0);
	COMM_VHF_XMTR_B_CB.Register(PSH,"COMM_VHF_XMTR_B_CB",0);
	COMM_VHF_RCVR_A_CB.Register(PSH,"COMM_VHF_RCVR_A_CB",0);
	COMM_CDR_AUDIO_CB.Register(PSH,"COMM_CDR_AUDIO_CB",0);
	PGNS_SIG_STR_DISP_CB.Register(PSH,"PGNS_SIG_STR_DISP_CB",0);
	PGNS_LDG_RDR_CB.Register(PSH,"PGNS_LDG_RDR_CB",0);
	PGNS_RNDZ_RDR_CB.Register(PSH,"PGNS_RNDZ_RDR_CB",0);
	LGC_DSKY_CB.Register(PSH, "LGC_DSKY_CB", 0);
	IMU_SBY_CB.Register(PSH, "IMU_SBY_CB", 1);
	IMU_OPR_CB.Register(PSH, "IMU_OPR_CB", 0);

	// Panel 11, Row 5
	CDRBatteryFeedTieCB1.Register(PSH, "CDRBatteryFeedTieCB1", 1);
	CDRBatteryFeedTieCB2.Register(PSH, "CDRBatteryFeedTieCB2", 1);
	CDRCrossTieBalCB.Register(PSH, "CDRCrossTieBalCB", 1);
	CDRCrossTieBusCB.Register(PSH, "CDRCrossTieBusCB", 0);
	CDRXLunarBusTieCB.Register(PSH, "CDRXLunarBusTieCB", 0);
	CDRDesECAContCB.Register(PSH, "CDRDesECAContCB", 0);
	CDRDesECAMainCB.Register(PSH, "CDRDesECAMainCB", 1);
	CDRAscECAContCB.Register(PSH, "CDRAscECAContCB", 0);
	CDRAscECAMainCB.Register(PSH, "CDRAscECAMainCB", 0);
	CDRInverter1CB.Register(PSH,"CDRInverter1CB",0);
	CDRDCBusVoltCB.Register(PSH, "CDRDCBusVoltCB", 1);

	// Panel 16, Row 1
	LMP_EVT_TMR_FDAI_DC_CB.Register(PSH,"LMP_EVT_TMR_FDAI_DC_CB",0);
	SE_XPTR_DC_CB.Register(PSH,"SE_XPTR_DC_CB",0);
	RCS_B_ASC_FEED_1_CB.Register(PSH,"RCS_B_ASC_FEED_1_CB",0);
	RCS_B_ASC_FEED_2_CB.Register(PSH,"RCS_B_ASC_FEED_2_CB",0);
	RCS_B_ISOL_VLV_CB.Register(PSH,"RCS_B_ISOL_VLV_CB",0);
	RCS_B_QUAD1_TCA_CB.Register(PSH,"RCS_B_QUAD1_TCA_CB",0);
	RCS_B_QUAD2_TCA_CB.Register(PSH,"RCS_B_QUAD2_TCA_CB",0);
	RCS_B_QUAD3_TCA_CB.Register(PSH,"RCS_B_QUAD3_TCA_CB",0);
	RCS_B_QUAD4_TCA_CB.Register(PSH,"RCS_B_QUAD4_TCA_CB",0);
	RCS_B_CRSFD_CB.Register(PSH,"RCS_B_CRSFD_CB",0);
	RCS_B_TEMP_PRESS_DISP_FLAGS_CB.Register(PSH,"RCS_B_TEMP_PRESS_DISP_FLAGS_CB",0);
	RCS_B_PQGS_DISP_CB.Register(PSH, "RCS_B_PQGS_DISP_CB", 0);
	RCS_B_MAIN_SOV_CB.Register(PSH,"RCS_B_MAIN_SOV_CB",0);
	PROP_DISP_ENG_OVRD_LOGIC_CB.Register(PSH,"PROP_DISP_ENG_OVRD_LOGIC_CB",0);
	PROP_PQGS_CB.Register(PSH,"PROP_PQGS_CB",0);
	PROP_ASC_HE_REG_CB.Register(PSH,"PROP_ASC_HE_REG_CB",0);

	// Panel 16, Row 2
	LTG_FLOOD_CB.Register(PSH,"LTG_FLOOD_CB",1);
	LTG_TRACK_CB.Register(PSH,"LTG_TRACK_CB",0);
	LTG_ANUN_DOCK_COMPNT_CB.Register(PSH,"LTG_ANUN_DOCK_COMPNT_CB",0);
	LTG_MASTER_ALARM_CB.Register(PSH,"LTG_MASTER_ALARM_CB",0);
	EDS_CB_LOGIC_B.Register(PSH,"EDS_CB_LOGIC_B",0);
	SCS_AEA_CB.Register(PSH,"SCS_AEA_CB",0);
	SCS_ENG_ARM_CB.Register(PSH,"SCS_ENG_ARM_CB",0);
	SCS_ASA_CB.Register(PSH,"SCS_ASA_CB",1);
	SCS_AELD_CB.Register(PSH,"SCS_AELD_CB",0);
	SCS_ATCA_CB.Register(PSH,"SCS_ATCA_CB",0);
	SCS_ABORT_STAGE_CB.Register(PSH,"SCS_ABORT_STAGE_CB",0);
	SCS_ATCA_AGS_CB.Register(PSH,"SCS_ATCA_AGS_CB",0);
	SCS_DES_ENG_OVRD_CB.Register(PSH,"SCS_DES_ENG_OVRD_CB",0);
	INST_CWEA_CB.Register(PSH,"INST_CWEA_CB",0);
	INST_SIG_SENSOR_CB.Register(PSH,"INST_SIG_SENSOR_CB",0);
	INST_PCMTEA_CB.Register(PSH,"INST_PCMTEA_CB",0);
	INST_SIG_CONDR_2_CB.Register(PSH,"INST_SIG_CONDR_2_CB",0);
	ECS_SUIT_FLOW_CONT_CB.Register(PSH,"ECS_SUIT_FLOW_CONT_CB",0);

	// Panel 16, Row 3
	COMM_DISP_CB.Register(PSH,"COMM_DISP_CB",0);
	COMM_SE_AUDIO_CB.Register(PSH,"COMM_SE_AUDIO_CB",0);
	COMM_VHF_XMTR_A_CB.Register(PSH,"COMM_VHF_XMTR_A_CB",0);
	COMM_VHF_RCVR_B_CB.Register(PSH,"COMM_VHF_RCVR_B_CB",0);
	COMM_PRIM_SBAND_PA_CB.Register(PSH,"COMM_PRIM_SBAND_PA_CB",0);
	COMM_PRIM_SBAND_XCVR_CB.Register(PSH,"COMM_PRIM_SBAND_XCVR_CB",0);
	COMM_SBAND_ANT_CB.Register(PSH,"COMM_SBAND_ANT_CB",0);
	COMM_PMP_CB.Register(PSH,"COMM_PMP_CB",0);
	COMM_TV_CB.Register(PSH,"COMM_TV_CB",0);
	ECS_DISP_CB.Register(PSH,"ECS_DISP_CB",0);
	ECS_GLYCOL_PUMP_SEC_CB.Register(PSH,"ECS_GLYCOL_PUMP_SEC_CB",0);
	ECS_LGC_PUMP_CB.Register(PSH,"ECS_LGC_PUMP_CB",0);
	ECS_CABIN_FAN_CONT_CB.Register(PSH,"ECS_CABIN_FAN_CONT_CB",0);
	ECS_CABIN_REPRESS_CB.Register(PSH,"ECS_CABIN_REPRESS_CB",1);
	ECS_SUIT_FAN_2_CB.Register(PSH,"ECS_SUIT_FAN_2_CB",0);
	ECS_SUIT_FAN_DP_CB.Register(PSH,"ECS_SUIT_FAN_DP_CB",0);
	ECS_DIVERT_VLV_CB.Register(PSH,"ECS_DIVERT_VLV_CB",0);
	ECS_CO2_SENSOR_CB.Register(PSH,"ECS_CO2_SENSOR_CB",0);

	// Panel 16, Row 4
	RCS_QUAD_1_LMP_HTR_CB.Register(PSH,"RCS_QUAD_1_LMP_HTR_CB",0);
	RCS_QUAD_2_LMP_HTR_CB.Register(PSH,"RCS_QUAD_2_LMP_HTR_CB",0);
	RCS_QUAD_3_LMP_HTR_CB.Register(PSH,"RCS_QUAD_3_LMP_HTR_CB",0);
	RCS_QUAD_4_LMP_HTR_CB.Register(PSH,"RCS_QUAD_4_LMP_HTR_CB",0);
	HTR_DISP_CB.Register(PSH,"HTR_DISP_CB",0);
	HTR_SBD_ANT_CB.Register(PSH, "HTR_SBD_ANT_CB", 1);
	CAMR_SEQ_CB.Register(PSH,"CAMR_SEQ_CB",0);
	EPS_DISP_CB.Register(PSH,"EPS_DISP_CB",0);
	LMPDCBusVoltCB.Register(PSH, "LMPDCBusVoltCB", 1);
	LMPInverter2CB.Register(PSH,"LMPInverter2CB",0);
	LMPAscECAContCB.Register(PSH, "LMPAscECAContCB", 0);
	LMPAscECAMainCB.Register(PSH, "LMPAscECAMainCB", 0);
	LMPDesECAContCB.Register(PSH, "LMPDesECAContCB", 0);
	LMPDesECAMainCB.Register(PSH, "LMPDesECAMainCB", 1);
	LMPXLunarBusTieCB.Register(PSH, "LMPXLunarBusTieCB", 0);
	LMPCrossTieBusCB.Register(PSH, "LMPCrossTieBusCB", 0);
	LMPCrossTieBalCB.Register(PSH, "LMPCrossTieBalCB", 1);
	LMPBatteryFeedTieCB1.Register(PSH, "LMPBatteryFeedTieCB1", 1);
	LMPBatteryFeedTieCB2.Register(PSH, "LMPBatteryFeedTieCB2", 1);

	LEMCoas1Enabled = false;
	LEMCoas2Enabled = true;
	ordealEnabled = false;

	RRGyroSelSwitch.Register(PSH,"RRGyroSelSwitch",THREEPOSSWITCH_UP);

	DskySwitchVerb.Register(PSH, "DskySwitchVerb", false);
	DskySwitchNoun.Register(PSH, "DskySwitchNoun", false);
	DskySwitchPlus.Register(PSH, "DskySwitchPlus", false);
	DskySwitchMinus.Register(PSH, "DskySwitchMinus", false);
	DskySwitchZero.Register(PSH, "DskySwitchZero", false);
	DskySwitchOne.Register(PSH, "DskySwitchOne", false);
	DskySwitchTwo.Register(PSH, "DskySwitchTwo", false);
	DskySwitchThree.Register(PSH, "DskySwitchThree", false);
	DskySwitchFour.Register(PSH, "DskySwitchFour", false);
	DskySwitchFive.Register(PSH, "DskySwitchFive", false);
	DskySwitchSix.Register(PSH, "DskySwitchSix", false);
	DskySwitchSeven.Register(PSH, "DskySwitchSeven", false);
	DskySwitchEight.Register(PSH, "DskySwitchEight", false);
	DskySwitchNine.Register(PSH, "DskySwitchNine", false);
	DskySwitchClear.Register(PSH, "DskySwitchClear", false);
	DskySwitchProg.Register(PSH, "DskySwitchProg", false);
	DskySwitchKeyRel.Register(PSH, "DskySwitchKeyRel", false);
	DskySwitchEnter.Register(PSH, "DskySwitchEnter", false);
	DskySwitchReset.Register(PSH, "DskySwitchReset", false);

	DskySwitchVerb.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::VerbCallback));
	DskySwitchNoun.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::NounCallback));
	DskySwitchPlus.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::PlusCallback));
	DskySwitchMinus.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::MinusCallback));
	DskySwitchZero.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::zeroCallback));
	DskySwitchOne.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::oneCallback));
	DskySwitchTwo.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::twoCallback));
	DskySwitchThree.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::threeCallback));
	DskySwitchFour.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::fourCallback));
	DskySwitchFive.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::fiveCallback));
	DskySwitchSix.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::sixCallback));
	DskySwitchSeven.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::sevenCallback));
	DskySwitchEight.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::eightCallback));
	DskySwitchNine.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::nineCallback));
	DskySwitchProg.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::ProgCallback));
	DskySwitchClear.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::ClearCallback));
	DskySwitchKeyRel.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::KeyRelCallback));
	DskySwitchEnter.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::EnterCallback));
	DskySwitchReset.SetCallback(new PanelSwitchCallback<DSKY>(&dsky, &DSKY::ResetCallback));

	DskySwitchProg.SetDelayTime(1.5);

	DedaSwitchPlus.Register(PSH, "DedaSwitchPlus", false);
	DedaSwitchMinus.Register(PSH, "DedaSwitchMinus", false);
	DedaSwitchZero.Register(PSH, "DedaSwitchZero", false);
	DedaSwitchOne.Register(PSH, "DedaSwitchOne", false);
	DedaSwitchTwo.Register(PSH, "DedaSwitchTwo", false);
	DedaSwitchThree.Register(PSH, "DedaSwitchThree", false);
	DedaSwitchFour.Register(PSH, "DedaSwitchFour", false);
	DedaSwitchFive.Register(PSH, "DedaSwitchFive", false);
	DedaSwitchSix.Register(PSH, "DedaSwitchSix", false);
	DedaSwitchSeven.Register(PSH, "DedaSwitchSeven", false);
	DedaSwitchEight.Register(PSH, "DedaSwitchEight", false);
	DedaSwitchNine.Register(PSH, "DedaSwitchNine", false);
	DedaSwitchClear.Register(PSH, "DedaSwitchClear", false);
	DedaSwitchReadOut.Register(PSH, "DedaSwitchReadOut", false);
	DedaSwitchEnter.Register(PSH, "DedaSwitchEnter", false);
	DedaSwitchHold.Register(PSH, "DedaSwitchHold", false);

	DedaSwitchPlus.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::PlusCallback));
	DedaSwitchMinus.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::MinusCallback));
	DedaSwitchZero.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::zeroCallback));
	DedaSwitchOne.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::oneCallback));
	DedaSwitchTwo.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::twoCallback));
	DedaSwitchThree.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::threeCallback));
	DedaSwitchFour.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::fourCallback));
	DedaSwitchFive.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::fiveCallback));
	DedaSwitchSix.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::sixCallback));
	DedaSwitchSeven.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::sevenCallback));
	DedaSwitchEight.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::eightCallback));
	DedaSwitchNine.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::nineCallback));
	DedaSwitchClear.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::ClearCallback));
	DedaSwitchReadOut.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::ReadOutCallback));
	DedaSwitchEnter.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::EnterCallback));
	DedaSwitchHold.SetCallback(new PanelSwitchCallback<LEM_DEDA>(&deda, &LEM_DEDA::HoldCallback));

	DedaSwitchClear.SetDelayTime(0.5);
	DedaSwitchReadOut.SetDelayTime(0.5);
	DedaSwitchEnter.SetDelayTime(0.5);

	ORDEALAltSetRotary.AddPosition(0, 240);
	ORDEALAltSetRotary.AddPosition(1, 270);
	ORDEALAltSetRotary.AddPosition(2, 330);
	ORDEALAltSetRotary.AddPosition(3, 0);
	ORDEALAltSetRotary.AddPosition(4, 60);
	ORDEALAltSetRotary.AddPosition(5, 90);
	ORDEALAltSetRotary.AddPosition(6, 150);
	ORDEALAltSetRotary.Register(PSH, "ORDEALAltSetRotary", 2);

	ORDEALFDAI1Switch.Register(PSH, "ORDEALFDAI1Switch", false);
	ORDEALFDAI2Switch.Register(PSH, "ORDEALFDAI2Switch", false);
	ORDEALEarthSwitch.Register(PSH, "ORDEALEarthSwitch", THREEPOSSWITCH_CENTER);
	ORDEALLightingSwitch.Register(PSH, "ORDEALLightingSwitch", THREEPOSSWITCH_CENTER);
	ORDEALModeSwitch.Register(PSH, "ORDEALModeSwitch", false);
	ORDEALSlewSwitch.Register(PSH, "ORDEALSlewSwitch", THREEPOSSWITCH_CENTER, SPRINGLOADEDSWITCH_CENTER);
	PanelOrdeal.Register(PSH, "PanelOrdeal", 0, 0, 0, 0);	// dummy switch/display for checklist controller

	// ECS Panel
	SuitGasDiverterSwitch.Register(PSH, "SuitGasDiverterSwitch", 0);

	CabinRepressValveSwitch.AddPosition(0, 0);
	CabinRepressValveSwitch.AddPosition(1, 90);
	CabinRepressValveSwitch.AddPosition(2, 180);
	CabinRepressValveSwitch.Register(PSH, "CabinRepressValveSwitch", 2);

	PLSSFillValve.AddPosition(0, 0);
	PLSSFillValve.AddPosition(1, 210);
	PLSSFillValve.Register(PSH, "PLSSFillValve", 1);

	PressRegAValve.AddPosition(0, 180);
	PressRegAValve.AddPosition(1, 270);
	PressRegAValve.AddPosition(2, 0);
	PressRegAValve.AddPosition(3, 90);
	PressRegAValve.Register(PSH, "PressRegAValve", 3);

	PressRegBValve.AddPosition(0, 180);
	PressRegBValve.AddPosition(1, 270);
	PressRegBValve.AddPosition(2, 0);
	PressRegBValve.AddPosition(3, 90);
	PressRegBValve.Register(PSH, "PressRegBValve", 3);

	DESO2Valve.AddPosition(0, 0);
	DESO2Valve.AddPosition(1, 210);
	DESO2Valve.Register(PSH, "DESO2Valve", 1);

	ASCO2Valve1.AddPosition(0, 0);
	ASCO2Valve1.AddPosition(1, 210);
	ASCO2Valve1.Register(PSH, "ASCO2Valve1", 1);

	ASCO2Valve2.AddPosition(0, 0);
	ASCO2Valve2.AddPosition(1, 210);
	ASCO2Valve2.Register(PSH, "ASCO2Valve2", 1);

	CDRSuitIsolValve.AddPosition(0, 0);
	CDRSuitIsolValve.AddPosition(1, 90);
	CDRSuitIsolValve.Register(PSH, "CDRSuitIsolValve", 1);

	IntlkOvrd.Register(PSH, "InterlockOvrd", 0);

    CDRActuatorOvrd.Register(PSH, "CDRActuatorOvrd", TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);

	LMPSuitIsolValve.AddPosition(0, 0);
	LMPSuitIsolValve.AddPosition(1, 90);
	LMPSuitIsolValve.Register(PSH, "LMPSuitIsolValve", 1);

    LMPActuatorOvrd.Register(PSH, "LMPActuatorOvrd", TOGGLESWITCH_DOWN, SPRINGLOADEDSWITCH_DOWN);

	SecEvapFlowValve.AddPosition(0, 0);
	SecEvapFlowValve.AddPosition(1, 270);
	SecEvapFlowValve.Register(PSH, "SecEvapFlowValve", 1);

	PrimEvap1FlowValve.AddPosition(0, 15);
	PrimEvap1FlowValve.AddPosition(1, 300);
	PrimEvap1FlowValve.Register(PSH, "PrimEvap1FlowValve", 1);

	PrimEvap2FlowValve.AddPosition(0, 0);
	PrimEvap2FlowValve.AddPosition(1, 270);
	PrimEvap2FlowValve.Register(PSH, "PrimEvap2FlowValve", 1);

	DESH2OValve.AddPosition(0, 30);
	DESH2OValve.AddPosition(1, 270);
	DESH2OValve.Register(PSH, "DESH2OValve", 1);

	WaterTankSelectValve.AddPosition(0, 150);
	WaterTankSelectValve.AddPosition(1, 240);
	WaterTankSelectValve.AddPosition(2, 330);
	WaterTankSelectValve.Register(PSH, "WaterTankSelectValve", 0);

	SuitTempValve.AddPosition(0, 0);
	SuitTempValve.AddPosition(1, 22);
	SuitTempValve.AddPosition(2, 45);
	SuitTempValve.AddPosition(3, 67);
	SuitTempValve.AddPosition(4, 90);
	SuitTempValve.Register(PSH, "SuitTempValve", 0);

	ASCH2OValve.AddPosition(0, 100);
	ASCH2OValve.AddPosition(1, 340);
	ASCH2OValve.Register(PSH, "ASCH2OValve", 1);

	LiquidGarmentCoolingValve.AddPosition(0, 0);
	LiquidGarmentCoolingValve.AddPosition(1, 22);
	LiquidGarmentCoolingValve.AddPosition(2, 45);
	LiquidGarmentCoolingValve.AddPosition(3, 67);
	LiquidGarmentCoolingValve.AddPosition(4, 90);
    LiquidGarmentCoolingValve.AddPosition(5, 120);
	LiquidGarmentCoolingValve.Register(PSH, "LiquidGarmentCoolingValve", 0);

	SuitCircuitReliefValveSwitch.AddPosition(0, 0);
	SuitCircuitReliefValveSwitch.AddPosition(1, 90);
	SuitCircuitReliefValveSwitch.AddPosition(2, 180);
	SuitCircuitReliefValveSwitch.Register(PSH, "SuitCircuitReliefValveSwitch", 1);

	CabinGasReturnValveSwitch.AddPosition(0, 0);
	CabinGasReturnValveSwitch.AddPosition(1, 90);
	CabinGasReturnValveSwitch.AddPosition(2, 180);
	CabinGasReturnValveSwitch.Register(PSH, "CabinGasReturnValveSwitch", 1);

	CO2CanisterSelectSwitch.Register(PSH, "CO2CanisterSelectSwitch", TOGGLESWITCH_UP);
	CO2CanisterSelectSwitch.SetSideways(1);

	CO2CanisterPrimValve.AddPosition(0, 300);
	CO2CanisterPrimValve.AddPosition(1, 0);
	CO2CanisterPrimValve.Register(PSH, "CO2CanisterPrimValve", 1);

	CO2CanisterSecValve.AddPosition(0, 300);
	CO2CanisterSecValve.AddPosition(1, 0);
	CO2CanisterSecValve.Register(PSH, "CO2CanisterSecValve", 1);

	CO2CanisterPrimVent.Register(PSH, "CO2CanisterPrimVent", 0);
	CO2CanisterSecVent.Register(PSH, "CO2CanisterSecVent", 0);

	WaterSepSelectSwitch.Register(PSH, "WaterSepSelectSwitch", 0);

	// Upper Hatch
	UpperHatchReliefValve.Register(PSH, "UpperReliefValve", 0);
	UpperHatchHandle.Register(PSH, "UpperHandle", TOGGLESWITCH_DOWN);
	UpperHatchHandle.SetSideways(1);

	UtilityLightSwitchCDR.Register(PSH, "UtilityLightSwitchCDR", THREEPOSSWITCH_UP);
	UtilityLightSwitchLMP.Register(PSH, "UtilityLightSwitchLMP", THREEPOSSWITCH_UP);

	// Forward Hatch
	ForwardHatchHandle.Register(PSH, "ForwardHandle", TOGGLESWITCH_DOWN);
	ForwardHatchReliefValve.Register(PSH, "ForwardReliefValve", THREEPOSSWITCH_CENTER);
	ForwardHatchReliefValve.SetSideways(1);

	//
	// Old stuff.
	//

	COASswitch=true;

	for (int i = 0; i < nsurf; i++)
		srf[i] = 0;
}

void LEM::RedrawPanel_Horizon (SURFHANDLE surf)
{
POINT pt[4];
	static double prange = RAD*30.0;
	static int size = 48, size2 = size*2;
	static int extent = (int)(size*prange);
	double bank = GetBank();
	double pitch = GetPitch();
	double pfrac = pitch/prange;
	double sinb = sin(bank), cosb = cos(bank);
	double a = tan(bank);
	double yl, yr, xb, xt, xlr, xll, ylr, yll;
	int i, iphi, n = 0;
	bool bl, br, bb, bt, bblue;

	if (cosb) { // horizon not vertical
		double b = pfrac/cosb;
		bl = (fabs(yl = -a+b) < 1.0); // left edge
		br = (fabs(yr =  a+b) < 1.0);  // right edge
		if (a) { // horizon not horizontal
			bb = (fabs(xb = ( 1.0-b)/a) < 1.0); // bottom edge
			bt = (fabs(xt = (-1.0-b)/a) < 1.0); // top edge
		} else { // horizon horizontal
			bb = bt = false;
		}
	} else { // horizon vertical
		bl = br = false;
		bb = bt = (fabs(xb = xt = pfrac) < 1.0);
	}
	if (bl) {
		pt[0].x = 0;
		pt[0].y = (int)(yl*size)+size;
		if (bt) {
			pt[1].x = (int)(xt*size)+size;
			pt[1].y = 0;
			pt[2].x = 0;
			pt[2].y = 0;
			n = 3;
			bblue = (cosb > 0.0);
		} else if (br) {
			pt[1].x = size2;
			pt[1].y = (int)(yr*size)+size;
			pt[2].x = size2;
			pt[2].y = 0;
			pt[3].x = 0;
			pt[3].y = 0;
			n = 4;
			bblue = (cosb > 0.0);
		} else if (bb) {
			pt[1].x = (int)(xb*size)+size;
			pt[1].y = size2;
			pt[2].x = 0;
			pt[2].y = size2;
			n = 3;
			bblue = (cosb < 0.0);
		}
	} else if (br) {
		pt[0].x = size2;
		pt[0].y = (int)(yr*size)+size;
		if (bt) {
			pt[1].x = (int)(xt*size)+size;
			pt[1].y = 0;
			pt[2].x = size2;
			pt[2].y = 0;
			n = 3;
			bblue = (cosb > 0.0);
		} else if (bb) {
			pt[1].x = (int)(xb*size)+size;
			pt[1].y = size2;
			pt[2].x = size2;
			pt[2].y = size2;
			n = 3;
			bblue = (cosb < 0.0);
		}
	} else if (bt && bb) {
		pt[0].x = (int)(xt*size)+size;
		pt[0].y = 0;
		pt[1].x = (int)(xb*size)+size;
		pt[1].y = size2;
		pt[2].x = 0;
		pt[2].y = size2;
		pt[3].x = 0;
		pt[3].y = 0;
		n = 4;
		bblue = ((xt-xb)*cosb > 0.0);
	}
	if (!n) bblue = (pitch < 0.0);
	oapiClearSurface (surf, bblue ? g_Param.col[3]:g_Param.col[2]);
	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, GetStockObject (BLACK_PEN));
	if (n >= 3) {
		SelectObject (hDC, g_Param.brush[bblue ? 2:3]);
		Polygon (hDC, pt, n);
		SelectObject (hDC, g_Param.pen[0]);
		MoveToEx (hDC, pt[0].x, pt[0].y, NULL); LineTo (hDC, pt[1].x, pt[1].y);
	}
	// bank indicator
	SelectObject (hDC, g_Param.pen[0]);
	SelectObject (hDC, GetStockObject (NULL_BRUSH));
	static double r1 = 40, r2 = 35;
	double sinb1 = sin(bank-0.1), cosb1 = cos(bank-0.1);
	double sinb2 = sin(bank+0.1), cosb2 = cos(bank+0.1);
	pt[0].x = (int)(r2*sinb1+0.5)+size; pt[0].y = -(int)(r2*cosb1+0.5)+size;
	pt[1].x = (int)(r1*sinb+0.5)+size;  pt[1].y = -(int)(r1*cosb+0.5)+size;
	pt[2].x = (int)(r2*sinb2+0.5)+size; pt[2].y = -(int)(r2*cosb2+0.5)+size;
	Polygon (hDC, pt, 3);

	// pitch ladder
	static double d = size*(10.0*RAD)/prange;
	static double ladderw = 14.0;
	double lwcosa = ladderw*cosb, lwsina = ladderw*sinb;
	double dsinb = d*sinb, dcosb = d*cosb;
	double phi0 = floor(pitch*DEG*0.1);
	double d0 = (pitch*DEG*0.1-phi0) * d, d1 = d0-4*d;
	// ladder
	xlr = lwcosa-d1*sinb, xll = -lwcosa-d1*sinb;
	ylr = lwsina+d1*cosb, yll = -lwsina+d1*cosb;
	for (iphi = (int)phi0+4, i = 0; i < 8; i++, iphi--) {
		if (iphi) {
			MoveToEx (hDC, size+(int)xll, size+(int)yll, NULL);
			LineTo   (hDC, size+(int)xlr, size+(int)ylr);
		}
		xlr -= dsinb, ylr += dcosb;
		xll -= dsinb, yll += dcosb;
	}
	oapiReleaseDC (surf, hDC);
	// labels
	lwcosa *= 1.6, lwsina *= 1.6;
	xlr = lwcosa-d1*sinb, xll = -lwcosa-d1*sinb;
	ylr = lwsina+d1*cosb, yll = -lwsina+d1*cosb;
	for (iphi = (int)phi0+4, i = 0; i < 8; i++, iphi--) {
		if (iphi) {
			int lb = abs(iphi)-1; if (lb >= 9) lb = 16-lb;
			oapiBlt (surf, srf[SRF_DIGITAL], size-5+(int)xlr, size-3+(int)ylr, 9*lb, 96, 9, 7, SURF_PREDEF_CK);
			oapiBlt (surf, srf[SRF_DIGITAL], size-5+(int)xll, size-3+(int)yll, 9*lb, 96, 9, 7, SURF_PREDEF_CK);
		}
		xlr -= dsinb, ylr += dcosb;
		xll -= dsinb, yll += dcosb;
	}

	// now overlay markings with transparent blt
	oapiBlt (surf, srf[SRF_DIGITAL], 0, 0, 0, 0, 96, 96, SURF_PREDEF_CK);
	// oapiBlt (surf, srf[5], 0, 0, 0, 0, 96, 96, SURF_PREDEF_CK);
}

void DrawNeedle (HDC hDC, int x, int y, double rad, double angle, HPEN pen0, HPEN pen1)

{
	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;
	oldObj = SelectObject (hDC, pen1);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, oldObj);
	oldObj = SelectObject (hDC, pen0);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
	SelectObject (hDC, oldObj);
}

void LEM::RedrawPanel_Thrust (SURFHANDLE surf)

{
	double DispValue;

	DispValue = GetEngineLevel(ENGINE_HOVER);

	oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(67-(DispValue)*67),0,0,7,7, SURF_PREDEF_CK);//
	oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(67-(DispValue)*67),8,0,7,7, SURF_PREDEF_CK);//
}

void LEM::RedrawPanel_XPointer (CrossPointer *cp, SURFHANDLE surf) {

	int ix, iy;
	double vx, vy;
	HDC hDC;

	//draw the crosspointers
	cp->GetVelocities(vx, vy);

	ix = (int)(-3.0 * vx);
	if(ix < -60) ix = -60;
	if(ix > 60) ix = 60;
	iy = (int)(3.0 * vy);
	if(iy < -60) iy = -60;
	if(iy > 60 ) iy = 60;
	hDC = oapiGetDC(surf);
	HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 0, 0));
	SelectObject(hDC, pen);
	MoveToEx(hDC, 0, 65 + ix, NULL);
	LineTo(hDC, 134, 65 + ix);
	MoveToEx(hDC, 67 + iy, 0, NULL);
	LineTo(hDC, 67 + iy, 131);
	DeleteObject(pen);
	oapiReleaseDC(surf, hDC);
}

void LEM::RedrawPanel_XPointerVC(CrossPointer *cp, UINT animx, UINT animy) {

	double vx, vy;

	cp->GetVelocities(vx, vy);

	SetAnimation(animx, (vx / 40) + 0.5);
	SetAnimation(animy, (vy / 40) + 0.5);
}

void LEM::RedrawPanel_MFDButton(SURFHANDLE surf, int mfd, int side, int xoffset, int yoffset) {

	HDC hDC = oapiGetDC (surf);
	SelectObject (hDC, g_Param.font[1]);
	SetTextColor (hDC, RGB(255, 255, 255));
	SetTextAlign (hDC, TA_CENTER);
	SetBkMode (hDC, TRANSPARENT);
	const char *label;
	for (int bt = 0; bt < 6; bt++) {
		if (label = oapiMFDButtonLabel (mfd, bt+side*6))
			TextOut (hDC, xoffset, 44 * bt + yoffset, label, strlen(label));
		else break;
	}
	oapiReleaseDC (surf, hDC);
}

void LEM::clbkMFDMode (int mfd, int mode) {

	switch (mfd) {
	case MFD_LEFT:		
		TriggerPanelRedrawArea(LMPANEL_MAIN, AID_MFDLEFT);
		break;
	case MFD_RIGHT:		
		TriggerPanelRedrawArea(LMPANEL_MAIN, AID_MFDRIGHT);
		break;
	}
}

void LEM::ReleaseSurfaces ()
{
	for (int i = 0; i < nsurf; i++)
		if (srf[i]) {
			oapiDestroySurface (srf[i]);
			srf[i] = 0;
		}
}

void LEM::ReleaseSurfacesVC()
{
	for (int i = 0; i < nsurfvc; i++)
		if (srf[i]) {
			oapiDestroySurface(srf[i]);
			srf[i] = 0;
		}
}

void LEM::InitPanel (int panel)

{
    // LEM Main Panel
		srf[0]						= oapiCreateSurface (LOADBMP (IDB_ECSG));
		srf[SRF_INDICATOR]			= oapiCreateSurface (LOADBMP (IDB_INDICATOR));
		srf[SRF_NEEDLE]				= oapiCreateSurface (LOADBMP (IDB_NEEDLE1));
		srf[SRF_DIGITAL]			= oapiCreateSurface (LOADBMP (IDB_DIGITAL));
		srf[SRF_SWITCHUP]			= oapiCreateSurface (LOADBMP (IDB_SWITCHUP));
		// Unused surface 5 was
		// srf[5]						= oapiCreateSurface (LOADBMP (IDB_FDAI));
		srf[SRF_LIGHTS2]			= oapiCreateSurface (LOADBMP (IDB_LIGHTS2));
		srf[SRF_LEMSWITCH1]			= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH1));
		srf[SRF_LEMSWTICH3]			= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH3));
		// Unused surface 7 was
		// srf[7]						= oapiCreateSurface (LOADBMP (IDB_SWLEVER));
		srf[SRF_SECSWITCH]			= oapiCreateSurface (LOADBMP (IDB_SECSWITCH));
		// srf[9]						= oapiCreateSurface (LOADBMP (IDB_ABORT));
		// srf[10]						= oapiCreateSurface (LOADBMP (IDB_ANNUN));
		// srf[11]						= oapiCreateSurface (LOADBMP (IDB_LAUNCH));		
		srf[SRF_LMTWOPOSLEVER]		= oapiCreateSurface (LOADBMP (IDB_LEMSWITCH2));
		// srf[12]						= oapiCreateSurface (LOADBMP (IDB_LV_ENG));
		// There was a conflict here between hardcoded index 13 and SRF_DSKY
		// Hardcoded index 13 was moved to SRF_LIGHTS2 (index 5)
		srf[SRF_DSKY]				= oapiCreateSurface (LOADBMP (IDB_DSKY_LIGHTS));
		// srf[14]						= oapiCreateSurface (LOADBMP (IDB_ALTIMETER));
		// srf[15]						= oapiCreateSurface (LOADBMP (IDB_ANLG_GMETER));
		// srf[16]						= oapiCreateSurface (LOADBMP (IDB_THRUST));
		// srf[17]					= oapiCreateSurface (LOADBMP (IDB_HEADING));
		srf[SRF_CONTACTLIGHT]		= oapiCreateSurface (LOADBMP (IDB_CONTACT));
		// srf[19] (SRF_THREEPOSSWITCH305) was hardcoded in several places but never actually loaded?
		// srf[SRF_THREEPOSSWITCH305]	= oapiCreateSurface (LOADBMP (IDB_CONTACT));
		// There was a conflict here between hardcoded index 20 and SRF_LMABORTBUTTON
		// Hardcoded index 20 was moved to SRF_LEMSWITCH3 (index 7)		
		srf[SRF_LMABORTBUTTON]		= oapiCreateSurface (LOADBMP (IDB_LMABORTBUTTON));
		srf[SRF_LMMFDFRAME]			= oapiCreateSurface (LOADBMP (IDB_LMMFDFRAME));
		srf[SRF_LMTHREEPOSLEVER]	= oapiCreateSurface (LOADBMP (IDB_LMTHREEPOSLEVER));
		srf[SRF_LMTHREEPOSSWITCH]	= oapiCreateSurface (LOADBMP (IDB_LMTHREEPOSSWITCH));
		srf[SRF_DSKYDISP]			= oapiCreateSurface (LOADBMP (IDB_DSKY_DISP));		
		//srf[SRF_FDAI]	        	= oapiCreateSurface (LOADBMP (IDB_FDAI));		//The LM FDAI texture doesn't need this
		srf[SRF_FDAIROLL]			= oapiCreateSurface (LOADBMP (IDB_LEM_FDAI_ROLL));
		srf[SRF_CWSLIGHTS]			= oapiCreateSurface (LOADBMP (IDB_CWS_LIGHTS));
		srf[SRF_DSKYKEY]			= oapiCreateSurface (LOADBMP (IDB_DSKY_KEY));
		srf[SRF_LEMROTARY]			= oapiCreateSurface (LOADBMP (IDB_LEMROTARY));
		srf[SRF_FDAIOFFFLAG]		= oapiCreateSurface (LOADBMP (IDB_FDAIOFFFLAG));
		srf[SRF_FDAINEEDLES]		= oapiCreateSurface (LOADBMP (IDB_LEM_FDAI_NEEDLES));
		srf[SRF_CIRCUITBRAKER]		= oapiCreateSurface (LOADBMP (IDB_CIRCUITBRAKER));
		srf[SRF_LEM_COAS1]			= oapiCreateSurface (LOADBMP (IDB_LEM_COAS1));
		srf[SRF_LEM_COAS2]			= oapiCreateSurface (LOADBMP (IDB_LEM_COAS2));
		srf[SRF_DCVOLTS]			= oapiCreateSurface (LOADBMP (IDB_LMDCVOLTS));
		srf[SRF_DCAMPS]				= oapiCreateSurface (LOADBMP (IDB_LMDCAMPS));
		srf[SRF_LMYAWDEGS]			= oapiCreateSurface (LOADBMP (IDB_LMYAWDEGS));
		srf[SRF_LMPITCHDEGS]		= oapiCreateSurface (LOADBMP (IDB_LMPITCHDEGS));
		srf[SRF_LMSIGNALSTRENGTH]	= oapiCreateSurface (LOADBMP (IDB_LMSIGNALSTRENGTH));
		srf[SRF_AOTRETICLEKNOB]     = oapiCreateSurface (LOADBMP (IDB_AOT_RETICLE_KNOB));
		srf[SRF_AOTSHAFTKNOB]       = oapiCreateSurface (LOADBMP (IDB_AOT_SHAFT_KNOB));
		srf[SRF_AOT_FONT]           = oapiCreateSurface (LOADBMP (IDB_AOT_FONT));
		srf[SRF_THUMBWHEEL_LARGEFONTS] = oapiCreateSurface (LOADBMP (IDB_THUMBWHEEL_LARGEFONTS));
		srf[SRF_FIVE_POS_SWITCH]	= oapiCreateSurface (LOADBMP (IDB_FIVE_POS_SWITCH));
		srf[SRF_RR_NOTRACK]         = oapiCreateSurface (LOADBMP (IDB_RR_NOTRACK));
		srf[SRF_LEM_STAGESWITCH]	= oapiCreateSurface (LOADBMP (IDB_LEM_STAGESWITCH));
		srf[SRF_DIGITALDISP2]		= oapiCreateSurface (LOADBMP (IDB_DIGITALDISP2));
		srf[SRF_RADAR_TAPE]         = oapiCreateSurface (LOADBMP (IDB_RADAR_TAPE));
		srf[SRF_RADAR_TAPE2]        = oapiCreateSurface(LOADBMP(IDB_RADAR_TAPE2));
		srf[SRF_SEQ_LIGHT]			= oapiCreateSurface (LOADBMP (IDB_SEQ_LIGHT));
		srf[SRF_LMENGINE_START_STOP_BUTTONS] = oapiCreateSurface (LOADBMP (IDB_LMENGINESTARTSTOPBUTTONS));
		srf[SRF_LMTRANSLBUTTON]		= oapiCreateSurface (LOADBMP (IDB_LMTRANSLBUTTON));
		srf[SRF_LEMVENT]			= oapiCreateSurface (LOADBMP (IDB_LEMVENT));
		srf[SRF_LEM_ACT_OVRD]		= oapiCreateSurface (LOADBMP (IDB_LEM_ACT_OVRD));
		srf[SRF_LEM_CAN_SEL]		= oapiCreateSurface (LOADBMP (IDB_LEM_CAN_SEL));
		srf[SRF_LEM_ECS_ROTARY]		= oapiCreateSurface (LOADBMP (IDB_LEM_ECS_ROTARY));
		srf[SRF_LEM_H20_SEL]		= oapiCreateSurface (LOADBMP (IDB_LEM_H20_SEL));
		srf[SRF_LEM_H20_SEP]		= oapiCreateSurface (LOADBMP (IDB_LEM_H20_SEP));
		srf[SRF_LEM_ISOL_ROTARY]	= oapiCreateSurface (LOADBMP (IDB_LEM_ISOL_ROTARY));
		srf[SRF_LEM_PRIM_C02]		= oapiCreateSurface (LOADBMP (IDB_LEM_PRIM_C02));
		srf[SRF_LEM_SEC_C02]		= oapiCreateSurface (LOADBMP (IDB_LEM_SEC_C02));
		srf[SRF_LEM_SGD_LEVER]		= oapiCreateSurface (LOADBMP (IDB_LEM_SGD_LEVER));
		srf[SRF_LEM_U_HATCH_REL_VLV] = oapiCreateSurface(LOADBMP(IDB_LEM_UPPER_REL_VLV));
		srf[SRF_LEM_U_HATCH_HNDL]   = oapiCreateSurface(LOADBMP(IDB_LEM_UPPER_HANDLE));
		srf[SRF_LEM_F_HATCH_HNDL]   = oapiCreateSurface(LOADBMP(IDB_LEM_FWD_HANDLE));
		srf[SRF_LEM_F_HATCH_REL_VLV] = oapiCreateSurface(LOADBMP(IDB_LEM_FWD_REL_VLV));
		srf[SRF_LEM_INTLK_OVRD]     = oapiCreateSurface(LOADBMP(IDB_LEM_INTLK_OVRD));
		srf[SRF_RED_INDICATOR]		= oapiCreateSurface(LOADBMP(IDB_RED_INDICATOR));
		srf[SRF_LEM_MASTERALARM]	 = oapiCreateSurface(LOADBMP(IDB_LEM_MASTERALARM));
		srf[SRF_DEDA_KEY]			= oapiCreateSurface(LOADBMP(IDB_DEDA_KEY));
		srf[SRF_DEDA_LIGHTS]		= oapiCreateSurface(LOADBMP(IDB_DEDA_LIGHTS));
		srf[SRF_ORDEAL_ROTARY]		= oapiCreateSurface(LOADBMP(IDB_ORDEAL_ROTARY));
		srf[SRF_ORDEAL_PANEL]		= oapiCreateSurface(LOADBMP(IDB_ORDEAL_PANEL));
		srf[SRF_TW_NEEDLE]			= oapiCreateSurface(LOADBMP(IDB_TW_NEEDLE));
		srf[SRF_PWRFAIL_LIGHT]      = oapiCreateSurface(LOADBMP(IDB_LEM_PWRFAIL_LIGHT));
		
		//
		// Flashing borders.
		//

		srf[SRF_BORDER_34x29]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x29));
		srf[SRF_BORDER_34x61]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x61));
		srf[SRF_BORDER_55x111]		= oapiCreateSurface (LOADBMP (IDB_BORDER_55x111));
		srf[SRF_BORDER_46x75]		= oapiCreateSurface (LOADBMP (IDB_BORDER_46x75));
		srf[SRF_BORDER_39x38]		= oapiCreateSurface (LOADBMP (IDB_BORDER_39x38));
		srf[SRF_BORDER_92x40]		= oapiCreateSurface (LOADBMP (IDB_BORDER_92x40));
		srf[SRF_BORDER_34x33]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x33));
		srf[SRF_BORDER_29x29]		= oapiCreateSurface (LOADBMP (IDB_BORDER_29x29));
		srf[SRF_BORDER_34x31]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x31));
		srf[SRF_BORDER_47x43]		= oapiCreateSurface (LOADBMP(IDB_BORDER_47x43));
		srf[SRF_BORDER_50x158]		= oapiCreateSurface (LOADBMP (IDB_BORDER_50x158));
		srf[SRF_BORDER_38x52]		= oapiCreateSurface (LOADBMP (IDB_BORDER_38x52));
		srf[SRF_BORDER_34x34]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x34));
		srf[SRF_BORDER_90x90]		= oapiCreateSurface (LOADBMP (IDB_BORDER_90x90));
		srf[SRF_BORDER_84x84]		= oapiCreateSurface (LOADBMP (IDB_BORDER_84x84));
		srf[SRF_BORDER_70x70]		= oapiCreateSurface (LOADBMP (IDB_BORDER_70x70));
		srf[SRF_BORDER_23x20]		= oapiCreateSurface (LOADBMP (IDB_BORDER_23x20));
		srf[SRF_BORDER_78x78]		= oapiCreateSurface (LOADBMP (IDB_BORDER_78x78));
		srf[SRF_BORDER_32x160]		= oapiCreateSurface (LOADBMP (IDB_BORDER_32x160));
		srf[SRF_BORDER_72x72]		= oapiCreateSurface (LOADBMP (IDB_BORDER_72x72));
		srf[SRF_BORDER_75x64]		= oapiCreateSurface (LOADBMP (IDB_BORDER_75x64));
		srf[SRF_BORDER_34x39]		= oapiCreateSurface (LOADBMP (IDB_BORDER_34x39));
		srf[SRF_BORDER_38x38]		= oapiCreateSurface (LOADBMP (IDB_BORDER_38x38));
		srf[SRF_BORDER_40x40]		= oapiCreateSurface (LOADBMP (IDB_BORDER_40x40));
		srf[SRF_BORDER_126x131]     = oapiCreateSurface (LOADBMP (IDB_BORDER_126x131));
		srf[SRF_BORDER_115x115]     = oapiCreateSurface (LOADBMP (IDB_BORDER_115x115));
		srf[SRF_BORDER_68x68]       = oapiCreateSurface (LOADBMP (IDB_BORDER_68x68));
		srf[SRF_BORDER_169x168]     = oapiCreateSurface (LOADBMP (IDB_BORDER_169x168));
		srf[SRF_BORDER_67x64]       = oapiCreateSurface (LOADBMP (IDB_BORDER_67x64));
		srf[SRF_BORDER_201x205]     = oapiCreateSurface (LOADBMP (IDB_BORDER_201x205));
		srf[SRF_BORDER_122x265]     = oapiCreateSurface (LOADBMP (IDB_BORDER_122x265));
		srf[SRF_BORDER_225x224]     = oapiCreateSurface (LOADBMP (IDB_BORDER_225x224));
		srf[SRF_BORDER_51x54]       = oapiCreateSurface (LOADBMP (IDB_BORDER_51x54));
		srf[SRF_BORDER_205x205]     = oapiCreateSurface (LOADBMP (IDB_BORDER_205x205));
		srf[SRF_BORDER_30x144]      = oapiCreateSurface (LOADBMP (IDB_BORDER_30x144));
		srf[SRF_BORDER_400x400]     = oapiCreateSurface (LOADBMP (IDB_BORDER_400x400));
		srf[SRF_BORDER_1001x240]    = oapiCreateSurface (LOADBMP (IDB_BORDER_1001x240));
		srf[SRF_BORDER_360x316]     = oapiCreateSurface (LOADBMP (IDB_BORDER_360x316));
		srf[SRF_BORDER_178x187]     = oapiCreateSurface (LOADBMP (IDB_BORDER_178x187));
		srf[SRF_BORDER_55x55]       = oapiCreateSurface (LOADBMP (IDB_BORDER_55x55));
		srf[SRF_BORDER_109x119]     = oapiCreateSurface (LOADBMP (IDB_BORDER_109x119));
		srf[SRF_BORDER_68x69]       = oapiCreateSurface (LOADBMP (IDB_BORDER_68x69));
		srf[SRF_BORDER_210x200]     = oapiCreateSurface (LOADBMP (IDB_BORDER_210x200));
		srf[SRF_BORDER_104x106]     = oapiCreateSurface (LOADBMP (IDB_BORDER_104x106));
		srf[SRF_BORDER_286x197]     = oapiCreateSurface (LOADBMP (IDB_BORDER_286x197));

		//
		// Set color keys where appropriate.
		//

		oapiSetSurfaceColourKey (srf[0], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_NEEDLE], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_DIGITAL], 0);
		// oapiSetSurfaceColourKey (srf[5], g_Param.col[4]);
		// oapiSetSurfaceColourKey (srf[14], g_Param.col[4]);
		// oapiSetSurfaceColourKey (srf[15], g_Param.col[4]);
		// oapiSetSurfaceColourKey (srf[16], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_CONTACTLIGHT], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMABORTBUTTON],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMTHREEPOSLEVER],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMTWOPOSLEVER],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LMTHREEPOSSWITCH],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_DSKYDISP],				g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_SWITCHUP],				g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_LEMROTARY],			g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_CIRCUITBRAKER],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_FDAI],					g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_FDAIROLL],				g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_FDAIOFFFLAG],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_FDAINEEDLES],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LEM_COAS1],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LEM_COAS2],			g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_DCVOLTS],				g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_DCAMPS],				g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LMYAWDEGS],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LMPITCHDEGS],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LMSIGNALSTRENGTH],		g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_THUMBWHEEL_LARGEFONTS],g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_FIVE_POS_SWITCH],		g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_RR_NOTRACK],	     	g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_RADAR_TAPE],	     	g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_ORDEAL_ROTARY],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_ORDEAL_PANEL],			g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_TW_NEEDLE],			g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_LEM_STAGESWITCH],		g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_SEQ_LIGHT],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LMENGINE_START_STOP_BUTTONS], g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LMTRANSLBUTTON],		g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEMVENT],				g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_ACT_OVRD],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_CAN_SEL],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_ECS_ROTARY],		g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_H20_SEL],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_H20_SEP],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_ISOL_ROTARY],		g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_PRIM_C02],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_SEC_C02],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_SGD_LEVER],			g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_U_HATCH_REL_VLV],   g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_U_HATCH_HNDL],      g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_F_HATCH_HNDL],      g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_F_HATCH_REL_VLV],   g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_INTLK_OVRD],        g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_LEM_MASTERALARM],		g_Param.col[4]);

		//
		// Borders need to set the center color to transparent so only the outline
		// is visible.
		//

		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x29], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x61], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_55x111], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_46x75], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_39x38], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_92x40], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x33], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_29x29], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x31], g_Param.col[4]);
		oapiSetSurfaceColourKey(srf[SRF_BORDER_47x43],  g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_50x158], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_38x52], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_34x34], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_90x90], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_84x84], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_70x70], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_23x20], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_78x78], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_32x160], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_72x72], g_Param.col[4]);
		oapiSetSurfaceColourKey	(srf[SRF_BORDER_75x64], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_34x39], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_38x38], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_40x40], g_Param.col[4]);
        oapiSetSurfaceColourKey (srf[SRF_BORDER_126x131], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_115x115], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_68x68], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_169x168], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_67x64], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_201x205], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_122x265], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_225x224], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_51x54], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_205x205], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_30x144], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_400x400], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_1001x240], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_360x316], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_178x187], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_55x55], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_109x119], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_68x69], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_210x200], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_104x106], g_Param.col[4]);
		oapiSetSurfaceColourKey (srf[SRF_BORDER_286x197], g_Param.col[4]);

    SetSwitches(panel);
}

void LEM::InitPanelVC() {

	// LM VC surfaces

	srf[SRF_VC_DIGITALDISP] = oapiLoadTexture("ProjectApollo/VC/digitaldisp.dds");
	srf[SRF_VC_DIGITALDISP2] = oapiLoadTexture("ProjectApollo/VC/digitaldisp_2.dds");
	srf[SRF_VC_DSKYDISP] = oapiLoadTexture("ProjectApollo/VC/dsky_disp.dds");
	srf[SRF_VC_DSKY_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/dsky_lights.dds");
	srf[SRF_VC_RADAR_TAPEA] = oapiLoadTexture("ProjectApollo/VC/lm_range_rate_indicator_scales_a.dds");
	srf[SRF_VC_RADAR_TAPEB] = oapiLoadTexture("ProjectApollo/VC/lm_range_rate_indicator_scales_b.dds");
	srf[SRF_VC_RADAR_TAPE2] = oapiLoadTexture("ProjectApollo/VC/lm_range_rate_indicator_scales2.dds");
	srf[SFR_VC_CW_LIGHTS] = oapiLoadTexture("ProjectApollo/VC/lem_cw_lights.dds");
	srf[SRF_INDICATORVC] = oapiLoadTexture("ProjectApollo/VC/Indicator.dds");
	srf[SRF_INDICATORREDVC] = oapiLoadTexture("ProjectApollo/VC/IndicatorRed.dds");
	srf[SRF_LEM_MASTERALARMVC] = oapiLoadTexture("ProjectApollo/VC/lem_master_alarm.dds");
	srf[SRF_DEDA_LIGHTSVC] = oapiLoadTexture("ProjectApollo/VC/ags_lights.dds");

	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP], g_Param.col[4]);
	oapiSetSurfaceColourKey(srf[SRF_VC_DIGITALDISP2], g_Param.col[4]);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKYDISP], g_Param.col[4]);
	oapiSetSurfaceColourKey(srf[SRF_VC_DSKY_LIGHTS], g_Param.col[4]);
	oapiSetSurfaceColourKey(srf[SRF_VC_RADAR_TAPEA], g_Param.col[4]);
	oapiSetSurfaceColourKey(srf[SRF_VC_RADAR_TAPEB], g_Param.col[4]);
	oapiSetSurfaceColourKey(srf[SRF_VC_RADAR_TAPE2], g_Param.col[4]);
	oapiSetSurfaceColourKey(srf[SFR_VC_CW_LIGHTS], g_Param.col[4]);
}

bool LEM::clbkLoadPanel (int id) {

	//
	// Release all surfaces
	//
 	ReleaseSurfaces();

	//
	// Load panel background image
	//
	HBITMAP hBmp;

	switch(id) {
    case LMPANEL_MAIN:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_MAIN_PANEL));
		oapiSetPanelNeighbours(LMPANEL_LEFTWINDOW, LMPANEL_RIGHTWINDOW, LMPANEL_RNDZWINDOW, LMPANEL_FWDHATCH);
		break;

	case LMPANEL_RIGHTWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_RIGHT_WINDOW));
		oapiSetPanelNeighbours(LMPANEL_MAIN, LMPANEL_RIGHTPANEL, -1, -1);
		break;

    case LMPANEL_LEFTWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_LEFT_WINDOW));
		oapiSetPanelNeighbours(LMPANEL_LEFTPANEL, LMPANEL_MAIN, LMPANEL_LEFTZOOM, LMPANEL_LPDWINDOW);
		break;

    case LMPANEL_LPDWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_LPD_WINDOW));
		oapiSetPanelNeighbours(-1, LMPANEL_MAIN, LMPANEL_LEFTWINDOW, -1);
		break;

	case LMPANEL_RNDZWINDOW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_RENDEZVOUS_WINDOW));
		oapiSetPanelNeighbours(-1, LMPANEL_AOTVIEW, LMPANEL_DOCKVIEW, LMPANEL_MAIN);
		break;

	case LMPANEL_LEFTPANEL:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_LEFT_PANEL));
		oapiSetPanelNeighbours(-1, LMPANEL_LEFTWINDOW, -1, -1);
		break;

	case LMPANEL_AOTVIEW:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_AOT_VIEW));
		oapiSetPanelNeighbours(LMPANEL_RNDZWINDOW, -1, LMPANEL_AOTZOOM, LMPANEL_MAIN);
		break;

	case LMPANEL_RIGHTPANEL:
		hBmp = LoadBitmap (g_Param.hDLL, MAKEINTRESOURCE (IDB_LEM_RIGHT_PANEL));
		oapiSetPanelNeighbours(LMPANEL_RIGHTWINDOW, LMPANEL_ECSPANEL, -1, -1);
		break;

	case LMPANEL_ECSPANEL:
		hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_ECS_PANEL));
		oapiSetPanelNeighbours(LMPANEL_RIGHTPANEL, -1, LMPANEL_UPPERHATCH, -1);
		break;

	case LMPANEL_DOCKVIEW:
		hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_DOCK_VIEW));
		oapiSetPanelNeighbours(-1, -1, -1, LMPANEL_RNDZWINDOW);
		break;

	case LMPANEL_AOTZOOM:
		hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_AOT_ZOOM));
		oapiSetPanelNeighbours(-1, -1, -1, LMPANEL_AOTVIEW);
		break;
	
	case LMPANEL_LEFTZOOM:
		hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_LEFT_ZOOM));
		oapiSetPanelNeighbours(-1, -1, -1, LMPANEL_LEFTWINDOW);
		break;

	case LMPANEL_UPPERHATCH:
		if (OverheadHatch.IsOpen())
		{
			hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_UPPER_HATCH_OPEN));
		}
		else
		{
			hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_UPPER_HATCH));
		}
		oapiSetPanelNeighbours(-1, -1, -1, LMPANEL_ECSPANEL);
		break;

	case LMPANEL_FWDHATCH:
		if (ForwardHatch.IsOpen())
		{
			hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_FWD_HATCH_OPEN));
		}
		else
		{
			hBmp = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_LEM_FWD_HATCH));
		}
		oapiSetPanelNeighbours(-1, -1, LMPANEL_MAIN, -1);
		break;
	}

	MFDSPEC mfds_left = { { 693, 1577,  1003, 1886 }, 6, 6, 55, 44 };
	MFDSPEC mfds_right = { { 1698, 1577, 2008, 1886 }, 6, 6, 55, 44 };

	switch (id) {
	case LMPANEL_MAIN: // LEM Main panel
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiRegisterMFD (MFD_LEFT,  mfds_left);
		oapiRegisterMFD (MFD_RIGHT, mfds_right);
		
		fdaiLeft.RegisterMe(AID_FDAI_LEFT, 744, 625);
		fdaiLeft.SetLMmode();
		fdaiRight.RegisterMe(AID_FDAI_RIGHT, 1714, 625);
		fdaiRight.SetLMmode();
		hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_FDAI_ROLLINDICATOR));

		oapiRegisterPanelArea (AID_MFDLEFT,                          _R(635, 1564, 1060, 1918),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MFDRIGHT,                         _R(1640, 1564, 2065, 1918), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,              PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_LM_RCSIND,					    _R(1408,  245, 1657,  370), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_ECSIND_UPPER,				    _R(1712,  245, 1988,  370), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_ECSIND_LOWER,				    _R(1709,  439, 1867,  564), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_PROP_AND_ENGINE_IND,	    _R(1045,  428, 1294,  553), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_THRUST_WEIGHT_INDICATOR,			_R(1199,  632, 1230,  812), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_CWS_LEFT,					    _R( 859,   54, 1180,  180), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_CWS_RIGHT,				    _R(1694,   54, 1994,  180), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_ABORT,							_R(1161,  855, 1330,  975), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
        // 3 pos Engine Arm Lever
	    oapiRegisterPanelArea (AID_ENG_ARM,							_R( 773,  1078, 807, 1117), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		// 2 pos Descent Engine Command Override Lever
		oapiRegisterPanelArea (AID_DESCENT_ENGINE_SWITCH,			_R( 698,  1321, 740, 1361), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
        // 3 pos Mode control switches
	    oapiRegisterPanelArea (AID_MODECONTROL,						_R( 1137, 1425, 1385, 1470), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		// DSKY		
		oapiRegisterPanelArea (AID_DSKY_DISPLAY,					_R(1370, 1560, 1475, 1736), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_LIGHTS,						_R(1226, 1565, 1328, 1734), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSKY_KEY,						_R(1208, 1756, 1494, 1876), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_MISSION_CLOCK,					_R( 681,  286,  823,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EVENT_TIMER,						_R( 897,  286,  978,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_OXID_QUANTITY_INDICATOR,		_R(1065,  245, 1104,  266), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_FUEL_QUANTITY_INDICATOR,		_R(1065,  297, 1104,  319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_HELIUM_PRESS_INDICATOR,		_R(1197,  286, 1278,  308), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_CONTACTLIGHT1,					_R( 930,  426,  978,  474), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CONTACTLIGHT2,					_R(1962, 1221, 2010, 1269), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTXPOINTERSWITCH,				_R( 938,  515,  972,  544), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GUIDCONTSWITCHROW,				_R(1269,  627, 1304,  823), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_MA_LEFT,						_R( 651, 620, 698,  663), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEFTMONITORSWITCHES,				_R( 659,  712,  693,  824), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FDAILOWERSWITCHROW,				_R( 746,  920,  922,  959), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_REG_CONTROLS_LEFT,			_R( 962,  918,  998, 1125), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MPS_REG_CONTROLS_RIGHT,			_R(1036,  918, 1072, 1125), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ENGINETHRUSTCONTSWITCHES,		_R( 819, 1006,  928, 1117), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PROPELLANTSWITCHES,				_R(1092,  991, 1154, 1112), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_HELIUMMONROTARY,					_R(1192,  992, 1276, 1076), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_MA_RIGHT,						_R(2005, 620, 2052, 663), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTMONITORSWITCHES,			_R(2008,  712, 2042,  824), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPPRESSMONROTARY,				_R(1396, 1002, 1480, 1086), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ACAPROPSWITCH,					_R(1620, 1012, 1654, 1051), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_ASC_FEED_TALKBACKS,			_R(1415,  440, 1652,  463), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_ASC_FEED_SWITCHES,		    _R(1410,  493, 1657,  532), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_14_TALKBACKS,	_R(1415,  589, 1652,  612), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_14_SWITCHES,	_R(1410,  642, 1658,  682), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_23_TALKBACKS,	_R(1415,  715, 1652,  738), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_LGC_CMD_ENABLE_23_SWITCHES,	_R(1410,  768, 1657,  807), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_XFEED_TALKBACK,				_R(1416,  871, 1439,  894), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCS_XFEED_SWITCH,				_R(1410,  921, 1444,  960), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_SOV_TALKBACKS,				_R(1555,  871, 1648,  894), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_MAIN_SOV_SWITCHES,			    _R(1549,  922, 1654,  961), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GLYCOLSUITFANROTARIES,			_R(1706,  926, 1790, 1130), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_QTYMONROTARY,					_R(1892,  986, 1976, 1070), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ENGGIMBALENABLESWITCH,			_R( 709, 1227,  743, 1268), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RADARANTTESTSWITCHES,			_R( 779, 1227,  814, 1361), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TESTMONITORROTARY,				_R( 724, 1410,  808, 1494), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SLEWRATESWITCH,				    _R( 880, 1364,  914, 1393), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RENDEZVOUSRADARROTARY,		    _R( 976, 1387, 1060, 1471), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SATBCONTSWITCHES,				_R(1137, 1221, 1362, 1260), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ATTITUDECONTROLSWITCHES,			_R(1137, 1322, 1362, 1351), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPMONITORROTARY,				_R(1416, 1387, 1500, 1471), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_TEMPMONITORIND,					_R(1438, 1219, 1486, 1344), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RCSSYSQUADSWITCHES,				_R(1579, 1331, 1671, 1455), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LIGHTINGSWITCHES,				_R(1758, 1222, 1792, 1319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_FLOODROTARY,						_R(1848, 1263, 1932, 1347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LAMPTONETESTROTARY,				_R(1780, 1405, 1864, 1489), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RIGHTXPOINTERSWITCH,				_R(1982, 1318, 2016, 1347), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_EXTERIORLTGSWITCH,				_R(1961, 1432, 1995, 1461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL4LEFTSWITCHROW,				_R(1094, 1614, 1128, 1794), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_PANEL4RIGHTSWITCHROW,			_R(1572, 1614, 1606, 1794), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_XPOINTERCDR,						_R( 675,  426,  812,  559), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_XPOINTERLMP,						_R(1914, 426,  2051,  559), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMRADARSIGNALSTRENGTH,			_R( 852, 1229,  943, 1319), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,	          PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMRADARSLEWSWITCH,			    _R( 877, 1433,  918, 1472), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EVENT_TIMER_SWITCHES,			_R(1523, 1233, 1724, 1264), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RR_NOTRACK,    					_R(1002,  1303, 1036, 1337), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RANGE_TAPE,    					_R(1052,  660, 1096,  823), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_RATE_TAPE,    					_R(1103,  660, 1138,  823), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_PANEL_5,                      _R(  33, 1548,  593, 1889), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_CO2_LIGHT,						_R(1899, 1097, 1933, 1131), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SUITFAN_LIGHT,					_R(1823, 1074, 1857, 1108), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_H2OSEP_LIGHT,					_R(1945, 1097, 1979, 1131), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_GLYCOL_LIGHT,					_R(1823, 953, 1857, 987), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea (AID_LMP_MANUAL_ENGINE_STOP_SWITCH,	_R(2160, 1609, 2228, 1678), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);

		// DEDA
		oapiRegisterPanelArea (AID_LM_AGS_OPERATE_SWITCH,            _R(2249, 1835, 2286, 1875), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_DISP,                     _R(2434, 1672, 2568, 1694), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_ADR,                      _R(2458, 1627, 2516, 1649), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_KEYS,                     _R(2363, 1710, 2587, 1887), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_DEDA_LIGHTS,                   _R(2371, 1670, 2418, 1696), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,              PANEL_MAP_BACKGROUND);

		// ORDEAL
		oapiRegisterPanelArea(AID_ORDEALSWITCHES,					_R(1199, 10, 1676, 212),    PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_LBPRESSED | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);

		// Power Failure Lights
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_THRUST,               _R(1072, 404, 1085, 416), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_DPSPRESS,             _R(1254, 404, 1267, 416), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_RCSPRESS,             _R(1526, 221, 1539, 233), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_ECSPRESS,             _R(1832, 221, 1845, 233), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_RCSQUAN,              _R(1617, 221, 1630, 233), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_ECSQUAN,              _R(1829, 415, 1842, 427), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_GLYCOL,               _R(1738, 415, 1751, 427), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_XPTRCDR,              _R(725,  395, 738,  407), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_XPTRLMP,              _R(1961, 383, 1974, 395), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;	
		
	case LMPANEL_RIGHTWINDOW: // LEM Right Window
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		fdaiRight.RegisterMe(AID_FDAI_RIGHT, 36, 445);
		fdaiRight.SetLMmode();
		hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_FDAI_ROLLINDICATOR));

		oapiRegisterPanelArea(AID_XPOINTERLMP,						_R( 237,  246,  374,  379), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_MA_RIGHT, _R(328, 440, 375, 483), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_RIGHTMONITORSWITCHES,				_R (333,  532,  368,  645), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_GLYCOLSUITFANROTARIES,			_R(  31,  746,  116,  951), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_QTYMONROTARY,						_R( 215,  806,  300,  891), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_CO2_LIGHT,						 _R( 222, 917, 256, 951), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_SUITFAN_LIGHT,					_R( 146, 894, 180, 928), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_H2OSEP_LIGHT,						 _R( 268, 917, 302, 951), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_GLYCOL_LIGHT,						 _R( 146, 773, 180, 807), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_DC_BUS_LIGHT,						_R( 1560, 867, 1594, 901), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_BAT_FAULT_LIGHT,					_R( 1832, 778, 1866, 812), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LM_ECSIND_UPPER,				    _R(  35,   65,  311, 190), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,               PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LM_ECSIND_LOWER,				    _R(  32,  259,  190, 384), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,               PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LM_EPS_DC_VOLTMETER,              _R( 1623,  706, 1722,  804), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LM_EPS_DC_AMMETER,                _R( 1623,  818, 1722,  916), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);

		// Power Failure Lights
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_ECSPRESS,             _R(155, 41, 168, 53), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_ECSQUAN,              _R(152, 235, 165, 247), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_GLYCOL,               _R(61, 235, 74, 247), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_XPTRLMP,              _R(284, 202, 297, 214), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_LEFTWINDOW: // LEM Left Window
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		fdaiLeft.RegisterMe(AID_FDAI_LEFT, 1517, 445); // Was 135,625
		fdaiLeft.SetLMmode();
		hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_FDAI_ROLLINDICATOR));

		oapiRegisterPanelArea(AID_LEM_COAS2,						_R( 675,    0, 1215,  540), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MISSION_CLOCK,					_R(1455,  106, 1597,  130), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_EVENT_TIMER,						_R(1671,  106, 1752,  128), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_CONTACTLIGHT1,					_R(1704,  246, 1753,  295), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEFTXPOINTERSWITCH,				_R(1712,  335, 1747,  365), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_MA_LEFT, _R(1425, 440, 1472, 483), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEFTMONITORSWITCHES,				_R(1433,  532, 1467,  645), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_XPOINTERCDR,						_R(1449,  246, 1586,  379), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_FDAILOWERSWITCHROW,				_R(1520,  740, 1696,  780), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_ENGINETHRUSTCONTSWITCHES,			_R(1592,  826, 1702,  938), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_REG_CONTROLS_LEFT,			_R(1736,  738, 1772,  945), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_REG_CONTROLS_RIGHT,			_R(1810,  738, 1846, 945), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_RANGE_TAPE,						_R(1826,  480, 1870,  643), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_RATE_TAPE,						_R(1877,  480, 1915,  643), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_OXID_QUANTITY_INDICATOR,		_R(1839,   65, 1878,   86), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_FUEL_QUANTITY_INDICATOR,		_R(1839,  117, 1878,  139), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);
		// 3 pos Engine Arm Lever
		oapiRegisterPanelArea(AID_ENG_ARM,							_R(1547,  898, 1581,  937), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				  PANEL_MAP_BACKGROUND);

		// Power Failure Lights
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_THRUST,               _R(1846, 224, 1859, 236), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_XPTRCDR,              _R(1499, 215, 1512, 227), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0, 0);
		break;

	case LMPANEL_LPDWINDOW: // LPD Window
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		fdaiLeft.RegisterMe(AID_FDAI_LEFT, 1320, 243); // Was 135,625
		fdaiLeft.SetLMmode();
		hBmpFDAIRollIndicator = LoadBitmap(g_Param.hDLL, MAKEINTRESOURCE(IDB_FDAI_ROLLINDICATOR));

		oapiRegisterPanelArea(AID_MAIN_PROP_AND_ENGINE_IND,         _R(1622, 46, 1871, 171),    PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_THRUST_WEIGHT_INDICATOR,          _R(1776, 250, 1807, 430),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_XPOINTERCDR,						_R(1241,   44, 1378,  177), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			    PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_CONTACTLIGHT1,					_R(1507,   44, 1556,   93), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			    PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_RANGE_TAPE,						_R(1629,  278, 1673,  441), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			    PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_RATE_TAPE,						_R(1680,  278, 1718,  441), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			    PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_OXID_QUANTITY_INDICATOR,		_R(1434,  132, 1473,  153), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_FUEL_QUANTITY_INDICATOR,		_R(1434,  184, 1473,  206), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			    PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_ABORT,                            _R(1738, 473,  1907, 593),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		// 3 pos Engine Arm Lever
		oapiRegisterPanelArea(AID_ENG_ARM,                          _R(1350, 696, 1384, 736),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		// 2 pos Descent Engine Command Override Lever
		oapiRegisterPanelArea(AID_DESCENT_ENGINE_SWITCH,            _R(1578, 865, 1620, 905),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		// 3 pos Mode control switches
		oapiRegisterPanelArea(AID_MODECONTROL,                      _R(1669, 878, 1917, 923),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		// DSKY
		oapiRegisterPanelArea(AID_DSKY_KEY,                         _R(1592, 947, 1878, 1067),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_DSKY_DISPLAY,                     _R(1410, 888, 1515, 1065),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_DSKY_LIGHTS,                      _R(1266, 893, 1368, 1062),  PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);

		oapiRegisterPanelArea(AID_LEFTXPOINTERSWITCH,               _R(1540, 133, 1574, 162),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_GUIDCONTSWITCHROW,                _R(1846, 245, 1881, 441),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_MA_LEFT, _R(1228, 238, 1275, 281), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEFTMONITORSWITCHES,              _R(1236, 330, 1270, 442),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_FDAILOWERSWITCHROW,               _R(1323, 538, 1499, 577),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_REG_CONTROLS_LEFT,            _R(1539, 536, 1575, 743),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_MPS_REG_CONTROLS_RIGHT,           _R(1613, 536, 1649, 743),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_ENGINETHRUSTCONTSWITCHES,         _R(1396, 624, 1505, 735),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_PROPELLANTSWITCHES,               _R(1669, 609, 1731, 730),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_HELIUMMONROTARY,                  _R(1769, 610, 1853, 694),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                  PANEL_MAP_BACKGROUND);

		// Power Failure Lights
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_THRUST,               _R(1649, 22, 1662, 34),     PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_DPSPRESS,             _R(1831, 22, 1844, 34),     PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_LEM_PWRFAIL_XPTRCDR,              _R(1291, 13, 1304, 25),     PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, -sin(VIEWANGLE * RAD), cos(VIEWANGLE * RAD)));
		oapiCameraSetCockpitDir(0, 0);
		
		break;

	case LMPANEL_RNDZWINDOW: // LEM Rendezvous Window
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiRegisterPanelArea(AID_LEM_COAS1, _R(833, 0, 1664, 831), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_LBDOWN, PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 1.0, 0.0));
		oapiCameraSetCockpitDir(0, 0);
		break;

	case LMPANEL_LEFTPANEL: // LEM Left Panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW1,					_R( 264,  85,  1513,  115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW2,					_R( 264,  258, 1513,  288), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW3,					_R( 264,  431, 1513,  461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW4,					_R( 264,  604, 1637,  634), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P11_CB_ROW5,					_R( 264,  777,  996,  807), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,										PANEL_MAP_BACKGROUND);		
		oapiRegisterPanelArea (AID_LEM_PANEL_8,					    _R( 511,  916, 1654, 1258), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,						PANEL_MAP_BACKGROUND);				
		oapiRegisterPanelArea (AID_SEQ_LIGHT1,						_R( 941, 1187,  974, 1217), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP,                     PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_SEQ_LIGHT2,						_R(1015, 1187, 1048, 1217), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN | PANEL_MOUSE_UP,                     PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ORDEALSWITCHES,					_R(   1, 1093,  478, 1295),   PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_LBPRESSED|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);


		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_RIGHTPANEL: // LEM Right Panel
		oapiRegisterPanelBackground (hBmp,PANEL_ATTACH_TOP|PANEL_ATTACH_BOTTOM|PANEL_ATTACH_LEFT|PANEL_MOVEOUT_RIGHT,  g_Param.col[4]);	

		oapiRegisterPanelArea (AID_LM_EPS_DC_VOLTMETER,             _R( 110,  706, 209,  804), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EPS_DC_AMMETER,               _R( 110,  818, 209,  916), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_EPS_LEFT_CONTROLS,            _R( 314,  728, 542,  913), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW1,					_R( 174,  85,  1223, 115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW2,					_R( 174,  258, 1351, 288), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW3,					_R( 174,  431, 1415, 461), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LEM_P16_CB_ROW4,					_R( 174,  604, 1415, 634), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_BATTERY_TALKBACKS,	        _R( 573,  742, 888,  765), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_HIGH_VOLTAGE_SWITCHES,	    _R( 568,  791, 895,  834), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_DSC_LOW_VOLTAGE_SWITCHES,	    _R( 568,  867, 824,  900), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP, PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ASC_BATTERY_TALKBACKS,		    _R( 955,  767, 1199,  790), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_ASC_BATTERY_SWITCHES,		    _R( 949,  825, 1280,  858), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_AUDIO_CONT_SWITCH,		_R(  99, 1054,  133, 1083), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_UPDATA_LINK_SWITCH,		_R(  71, 1199,  105, 1229), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_AUDIO_CONTROLS,			_R( 175,  968,  438, 1272), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_SWITCHES_ROW1,		_R( 452,  994,  923, 1024), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_SWITCHES_ROW2,		_R( 568, 1097,  928, 1128), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_SWITCHES_ROW3,		_R( 636, 1189,  926, 1271), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMYAWDEGS,						_R( 1104, 962, 1196, 1053), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMPITCHDEGS,						_R( 984,  962, 1076, 1053), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LMSIGNALSTRENGTH,				_R( 1053, 1079, 1145, 1170), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_PITCH_KNOB,		_R( 974,  1171, 1058, 1255), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_YAW_KNOB,		_R( 1128, 1180, 1213, 1265), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_TRACK_MODE_SWITCH,_R( 976, 1092, 1010, 1123), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_VHF_SEL_KNOB,	_R( 1252,  994, 1337, 1079), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea (AID_LM_P12_COMM_ANT_SBD_SEL_KNOB,	_R( 1252, 1158, 1337, 1243), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				 PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_DC_BUS_LIGHT,						_R( 47, 867, 81, 901), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,					PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_BAT_FAULT_LIGHT,					 _R( 319, 778, 353, 812), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,				 PANEL_MAP_BACKGROUND);
	
		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0,0);
		break;

	case LMPANEL_AOTVIEW: // LEM Alignment Optical Telescope View
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiRegisterPanelArea(AID_AOT_RETICLE_KNOB,				_R(1427,  694, 1502, 1021), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_PRESSED|PANEL_MOUSE_UP,  PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_AOT_SHAFT_KNOB,				_R(1433,    0, 1496,  156), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,				      PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_RR_GYRO_SEL_SWITCH,			_R( 300,   66,  335,   96), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                    PANEL_MAP_BACKGROUND);
		oapiRegisterPanelArea(AID_AOT_RETICLEDISPLAY,           _R( 341,  824,  461,  860), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,                  PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0, 0);
		break;

	case LMPANEL_ECSPANEL: // LEM ECS Panel 
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiRegisterPanelArea(IDB_LEM_SGD_LEVER,         _R( 204,  129,  204+126,  129+131), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                 PANEL_MAP_BACKGROUND);
        oapiRegisterPanelArea(AID_LEM_ECS_OCM,           _R( 640,  160,     1290,      520), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,  PANEL_MAP_BACKGROUND);
        oapiRegisterPanelArea(AID_LEM_ISOL_ROTARY,       _R( 820,  630,     1372,      870), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,  PANEL_MAP_BACKGROUND);
        oapiRegisterPanelArea(AID_LEM_ECS_WCM,           _R(  40,  410,      440,     1296), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                 PANEL_MAP_BACKGROUND);
        oapiRegisterPanelArea(AID_LEM_ASC_H2O,           _R( 597,  634,      712,      750), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                 PANEL_MAP_BACKGROUND);
        oapiRegisterPanelArea(AID_LEM_GARMENT_COOL,      _R( 604, 1078,  604+115, 1078+115), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN,                 PANEL_MAP_BACKGROUND);
        oapiRegisterPanelArea(AID_LEM_SUIT_CIRCUIT_ASSY, _R(1400,  280,     2200,     1160), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN|PANEL_MOUSE_UP,  PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0, 0);
		break;

	case LMPANEL_DOCKVIEW: // LEM Docking View
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		SetCameraDefaultDirection(_V(0.0, 1.0, 0.0));
		oapiCameraSetCockpitDir(0, 0);
		break;

	case LMPANEL_AOTZOOM: // LEM Alignment Optical Telescope Zoom
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiRegisterPanelArea(AID_AOT_RETICLE,						_R( 408,  0, 1458,  1050), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_IGNORE,			  PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(cos(45.0*RAD)*sin(optics.OpticsShaft*PI / 3.0), sin(45.0*RAD), cos(45.0*RAD)*cos(optics.OpticsShaft*PI / 3.0)), optics.OpticsShaft*PI / 3.0);
		oapiCameraSetCockpitDir(0, 0);
		break;

	case LMPANEL_LEFTZOOM: // LEM Left Window COAS View
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0, 0);
		break;

	case LMPANEL_UPPERHATCH: // LEM Upper Hatch
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiRegisterPanelArea(AID_LEM_UPPER_HATCH, _R(637, 407, 1279, 962), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);

		if (!OverheadHatch.IsOpen())
		{
			oapiRegisterPanelArea(AID_LEM_UPPER_HATCH_HANDLE, _R(784, 52, 1070, 249), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea(AID_LEM_UPPER_HATCH_VALVE, _R(654, 300, 758, 406), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);
		}

		oapiRegisterPanelArea(AID_LEM_UTILITY_LT, _R(151, 112, 335, 141), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);

		SetCameraDefaultDirection(_V(0.0, -1.0, 0.0));
		oapiCameraSetCockpitDir(180 * RAD, 0);
		break;

	case LMPANEL_FWDHATCH: // LEM Forward Hatch
		oapiRegisterPanelBackground(hBmp, PANEL_ATTACH_TOP | PANEL_ATTACH_BOTTOM | PANEL_ATTACH_LEFT | PANEL_MOVEOUT_RIGHT, g_Param.col[4]);

		oapiRegisterPanelArea(AID_LEM_FWD_HATCH, _R(966, 401, 1734, 852), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);

		if (!ForwardHatch.IsOpen())
		{
			oapiRegisterPanelArea(AID_LEM_FWD_HATCH_HANDLE, _R(605, 401, 965, 852), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);
			oapiRegisterPanelArea(AID_LEM_FWD_HATCH_VALVE, _R(1735, 401, 1913, 852), PANEL_REDRAW_ALWAYS, PANEL_MOUSE_DOWN, PANEL_MAP_BACKGROUND);
		}

		SetCameraDefaultDirection(_V(0.0, 0.0, 1.0));
		oapiCameraSetCockpitDir(0, 0);
		break;
	}

	//
	// Initialize surfaces and switches
	//
	InitPanel(id);

	//
	// Change to desired panel next timestep.
	//
    if (!InPanel && id != PanelId) {	 
		CheckPanelIdInTimestep = true;
	} else {
	    PanelId = id;  
	}
	InPanel = true; 
	InVC = false;

	//
	// Set view parameter
	//
	SetCameraRotationRange(0.0, 0.0, 0.0, 0.0);
	SetView();
	
	//Set visbility flags for LM meshes
	SetLMMeshVis();

	return hBmp != NULL;
}

void LEM::SetSwitches(int panel) {

	MainPanel.Init(0, this, &soundlib, this);

	MainPropOxidPercentRow.Init(AID_MPS_OXID_QUANTITY_INDICATOR, MainPanel);
	DPSOxidPercentMeter.Init(srf[SRF_DIGITALDISP2], MainPropOxidPercentRow, this);

	MainPropFuelPercentRow.Init(AID_MPS_FUEL_QUANTITY_INDICATOR, MainPanel);
	DPSFuelPercentMeter.Init(srf[SRF_DIGITALDISP2], MainPropFuelPercentRow, this);

	MainPropHeliumPressRow.Init(AID_MPS_HELIUM_PRESS_INDICATOR, MainPanel);
	MainHeliumPressureMeter.Init(srf[SRF_DIGITALDISP2], MainPropHeliumPressRow, &HeliumMonRotary, this);

	MainPropAndEngineIndRow.Init(AID_MAIN_PROP_AND_ENGINE_IND, MainPanel);
	EngineThrustInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);
	CommandedThrustInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this);
	MainFuelTempInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this, &TempPressMonSwitch);
	MainFuelPressInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this, &TempPressMonSwitch);
	MainOxidizerTempInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this, &TempPressMonSwitch);
	MainOxidizerPressInd.Init(srf[SRF_NEEDLE], MainPropAndEngineIndRow, this, &TempPressMonSwitch);

	ThrustWeightIndRow.Init(AID_THRUST_WEIGHT_INDICATOR, MainPanel);
	ThrustWeightInd.Init(srf[SRF_TW_NEEDLE], ThrustWeightIndRow, this);

	RCSIndicatorRow.Init(AID_LM_RCSIND, MainPanel);
	LMRCSATempInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
	LMRCSBTempInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
	LMRCSAPressInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
	LMRCSBPressInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
	LMRCSAQtyInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);
	LMRCSBQtyInd.Init(srf[SRF_NEEDLE], RCSIndicatorRow, this);

	ECSUpperIndicatorRow.Init(AID_LM_ECSIND_UPPER, MainPanel);
	LMSuitTempMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
	LMCabinTempMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
	LMSuitPressMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
	LMCabinPressMeter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);
	LMCO2Meter.Init(srf[SRF_NEEDLE], ECSUpperIndicatorRow, this);

	ECSLowerIndicatorRow.Init(AID_LM_ECSIND_LOWER, MainPanel);
	LMGlycolTempMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);
	LMGlycolPressMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);
	LMOxygenQtyMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);
	LMWaterQtyMeter.Init(srf[SRF_NEEDLE], ECSLowerIndicatorRow, this);

	AbortSwitchesRow.Init(AID_ABORT, MainPanel);
	AbortSwitch.Init(0, 26, 55, 55, srf[SRF_LMABORTBUTTON], srf[SRF_BORDER_55x55], AbortSwitchesRow, 0, 119, this);
	AbortStageSwitch.Init(59, 0, 109, 119, srf[SRF_LMABORTBUTTON], srf[SRF_BORDER_109x119], AbortSwitchesRow, 109, 0, this);
	AbortStageSwitch.InitGuard(59, 0, 109, 119, srf[SRF_LMABORTBUTTON], srf[SRF_BORDER_109x119]);

	EngineArmSwitchesRow.Init(AID_ENG_ARM, MainPanel);
	EngineArmSwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], EngineArmSwitchesRow);

	LeftXPointerSwitchRow.Init(AID_LEFTXPOINTERSWITCH, MainPanel);
	LeftXPointerSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LeftXPointerSwitchRow);

	GuidContSwitchRow.Init(AID_GUIDCONTSWITCHROW, MainPanel);
	GuidContSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], GuidContSwitchRow);
	ModeSelSwitch.Init(0, 83, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], GuidContSwitchRow, &agc);
	AltRngMonSwitch.Init(0, 167, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], GuidContSwitchRow);

	LeftMonitorSwitchRow.Init(AID_LEFTMONITORSWITCHES, MainPanel);
	RateErrorMonSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LeftMonitorSwitchRow);
	AttitudeMonSwitch.Init(0, 83, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LeftMonitorSwitchRow);

	// 354,  918,  463, 1124
	MPSRegControlLeftSwitchRow.Init(AID_MPS_REG_CONTROLS_LEFT, MainPanel);
	ASCHeReg1TB.Init(6, 0, 23, 23, srf[SRF_INDICATOR], MPSRegControlLeftSwitchRow, scera1.GetSwitch(12, 6), true);
	DESHeReg1TB.Init(7, 134, 23, 23, srf[SRF_INDICATOR], MPSRegControlLeftSwitchRow, DPSPropellant.GetHeliumValve1(), true);
	ASCHeReg1Switch.Init(0, 43, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlLeftSwitchRow);
	ASCHeReg1Switch.SetDelayTime(1);
	DESHeReg1Switch.Init(0, 177, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlLeftSwitchRow);
	DESHeReg1Switch.SetDelayTime(1);

	MPSRegControlRightSwitchRow.Init(AID_MPS_REG_CONTROLS_RIGHT, MainPanel);
	ASCHeReg2TB.Init(6, 0, 23, 23, srf[SRF_INDICATOR], MPSRegControlRightSwitchRow, scera1.GetSwitch(12, 7), true);
	DESHeReg2TB.Init(7, 134, 23, 23, srf[SRF_INDICATOR], MPSRegControlRightSwitchRow, DPSPropellant.GetHeliumValve2(), false);
	ASCHeReg2Switch.Init(0, 43, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlRightSwitchRow);
	ASCHeReg2Switch.SetDelayTime(1);
	DESHeReg2Switch.Init(0, 177, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], MPSRegControlRightSwitchRow);
	DESHeReg2Switch.SetDelayTime(1);

	FDAILowerSwitchRow.Init(AID_FDAILOWERSWITCHROW, MainPanel);
	ShiftTruSwitch.Init(0, 5, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FDAILowerSwitchRow);
	RateScaleSwitch.Init(73, 5, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], FDAILowerSwitchRow);
	ACAPropSwitch.Init(142, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], FDAILowerSwitchRow);

	EngineThrustContSwitchRow.Init(AID_ENGINETHRUSTCONTSWITCHES, MainPanel);
	THRContSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineThrustContSwitchRow, &agc);
	THRContSwitch.SetChannelData(030, AutoThrottle, true);
	MANThrotSwitch.Init(69, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineThrustContSwitchRow);
	ATTTranslSwitch.Init(20, 77, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], EngineThrustContSwitchRow);
	BALCPLSwitch.Init(75, 72, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineThrustContSwitchRow);

	PropellantSwitchRow.Init(AID_PROPELLANTSWITCHES, MainPanel);
	QTYMonSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], PropellantSwitchRow);
	TempPressMonSwitch.Init(28, 92, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], PropellantSwitchRow);

	HeliumMonRotaryRow.Init(AID_HELIUMMONROTARY, MainPanel);
	HeliumMonRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], HeliumMonRotaryRow);

	RightMonitorSwitchRow.Init(AID_RIGHTMONITORSWITCHES, MainPanel);
	RightRateErrorMonSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RightMonitorSwitchRow);
	RightAttitudeMonSwitch.Init(0, 83, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RightMonitorSwitchRow);

	TempPressMonRotaryRow.Init(AID_TEMPPRESSMONROTARY, MainPanel);
	TempPressMonRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], TempPressMonRotaryRow);

	RightACAPropSwitchRow.Init(AID_ACAPROPSWITCH, MainPanel);
	RightACAPropSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], RightACAPropSwitchRow);

	// DS20090901 STILL AT IT...
	RCSAscFeedTBSwitchRow.Init(AID_RCS_ASC_FEED_TALKBACKS, MainPanel);
	RCSAscFeed1ATB.Init(0, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow, scera1.GetSwitch(13, 10), false);
	RCSAscFeed2ATB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow, scera1.GetSwitch(12, 3), false);
	RCSAscFeed1BTB.Init(144, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow, scera1.GetSwitch(13, 11), false);
	RCSAscFeed2BTB.Init(214, 0, 23, 23, srf[SRF_INDICATOR], RCSAscFeedTBSwitchRow, scera1.GetSwitch(12, 4), false);

	RCSAscFeedSwitchRow.Init(AID_RCS_ASC_FEED_SWITCHES, MainPanel);
	RCSAscFeed1ASwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
	RCSAscFeed1ASwitch.SetDelayTime(1);
	RCSAscFeed2ASwitch.Init(69, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
	RCSAscFeed2ASwitch.SetDelayTime(1);
	RCSAscFeed1BSwitch.Init(144, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
	RCSAscFeed1BSwitch.SetDelayTime(1);
	RCSAscFeed2BSwitch.Init(213, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSAscFeedSwitchRow);
	RCSAscFeed2BSwitch.SetDelayTime(1);

	RCSQuad14TBSwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_14_TALKBACKS, MainPanel);
	RCSQuad1ACmdEnableTB.Init(0, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad14TBSwitchRow, scera1.GetSwitch(13, 7), tca1A.GetTCAFailureFlipFlop());
	RCSQuad4ACmdEnableTB.Init(70, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad14TBSwitchRow, scera1.GetSwitch(13, 1), tca4A.GetTCAFailureFlipFlop());
	RCSQuad1BCmdEnableTB.Init(144, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad14TBSwitchRow, scera1.GetSwitch(13, 8), tca1B.GetTCAFailureFlipFlop());
	RCSQuad4BCmdEnableTB.Init(214, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad14TBSwitchRow, scera1.GetSwitch(13, 2), tca4B.GetTCAFailureFlipFlop());

	RCSQuad14SwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_14_SWITCHES, MainPanel);
	RCSQuad1ACmdEnableSwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow, this, 5);
	RCSQuad1ACmdEnableSwitch.SetDelayTime(1);
	RCSQuad4ACmdEnableSwitch.Init(69, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow, this, 0);
	RCSQuad4ACmdEnableSwitch.SetDelayTime(1);
	RCSQuad1BCmdEnableSwitch.Init(144, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow, this, 4);
	RCSQuad1BCmdEnableSwitch.SetDelayTime(1);
	RCSQuad4BCmdEnableSwitch.Init(213, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad14SwitchRow, this, 2);
	RCSQuad4BCmdEnableSwitch.SetDelayTime(1);

	RCSQuad23TBSwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_23_TALKBACKS, MainPanel);
	RCSQuad2ACmdEnableTB.Init(0, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad23TBSwitchRow, scera1.GetSwitch(13, 5), tca2A.GetTCAFailureFlipFlop());
	RCSQuad3ACmdEnableTB.Init(70, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad23TBSwitchRow, scera1.GetSwitch(13, 3), tca3A.GetTCAFailureFlipFlop());
	RCSQuad2BCmdEnableTB.Init(144, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad23TBSwitchRow, scera1.GetSwitch(13, 6), tca2B.GetTCAFailureFlipFlop());
	RCSQuad3BCmdEnableTB.Init(214, 0, 23, 23, srf[SRF_RED_INDICATOR], RCSQuad23TBSwitchRow, scera1.GetSwitch(13, 4), tca3B.GetTCAFailureFlipFlop());

	RCSQuad23SwitchRow.Init(AID_RCS_LGC_CMD_ENABLE_23_SWITCHES, MainPanel);
	RCSQuad2ACmdEnableSwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow, this, 7);
	RCSQuad2ACmdEnableSwitch.SetDelayTime(1);
	RCSQuad3ACmdEnableSwitch.Init(69, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow, this, 1);
	RCSQuad3ACmdEnableSwitch.SetDelayTime(1);
	RCSQuad2BCmdEnableSwitch.Init(144, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow, this, 6);
	RCSQuad2BCmdEnableSwitch.SetDelayTime(1);
	RCSQuad3BCmdEnableSwitch.Init(213, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSQuad23SwitchRow, this, 3);
	RCSQuad3BCmdEnableSwitch.SetDelayTime(1);

	RCSXfeedTBSwitchRow.Init(AID_RCS_XFEED_TALKBACK, MainPanel);
	RCSXFeedTB.Init(0, 0, 23, 23, srf[SRF_INDICATOR], RCSXfeedTBSwitchRow, scera1.GetSwitch(13, 12), false);

	RCSXfeedSwitchRow.Init(AID_RCS_XFEED_SWITCH, MainPanel);
	RCSXFeedSwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSXfeedSwitchRow);
	RCSXFeedSwitch.SetDelayTime(1);

	RCSMainSOVTBRow.Init(AID_MAIN_SOV_TALKBACKS, MainPanel);
	RCSMainSovATB.Init(0, 0, 23, 23, srf[SRF_INDICATOR], RCSMainSOVTBRow, scera1.GetSwitch(12, 8), true);
	RCSMainSovBTB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], RCSMainSOVTBRow, scera1.GetSwitch(12, 10), true);

	RCSMainSOVSwitchRow.Init(AID_MAIN_SOV_SWITCHES, MainPanel);
	RCSMainSovASwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSMainSOVSwitchRow);
	RCSMainSovASwitch.SetDelayTime(1);
	RCSMainSovBSwitch.Init(71, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RCSMainSOVSwitchRow);
	RCSMainSovBSwitch.SetDelayTime(1);

	GlycolSuitFanRotaryRow.Init(AID_GLYCOLSUITFANROTARIES, MainPanel);
	GlycolRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], GlycolSuitFanRotaryRow);
	SuitFanRotary.Init(0, 120, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], GlycolSuitFanRotaryRow);

	QtyMonRotaryRow.Init(AID_QTYMONROTARY, MainPanel);
	QtyMonRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], QtyMonRotaryRow);

	EngineDescentCommandOverrideSwitchesRow.Init(AID_DESCENT_ENGINE_SWITCH, MainPanel);
	EngineDescentCommandOverrideSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngineDescentCommandOverrideSwitchesRow);

	ModeControlSwitchesRow.Init(AID_MODECONTROL, MainPanel);

	ModeControlPGNSSwitch.Init(0, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ModeControlSwitchesRow, &agc);
	ModeControlAGSSwitch.Init(93, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ModeControlSwitchesRow);

	IMUCageSwitch.Init(191, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], ModeControlSwitchesRow, &imu);

	EngGimbalEnableSwitchRow.Init(AID_ENGGIMBALENABLESWITCH, MainPanel);
	EngGimbalEnableSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], EngGimbalEnableSwitchRow, &agc);
	EngGimbalEnableSwitch.SetChannelData(032, DescentEngineGimbalsDisabled, false);

	RadarAntTestSwitchesRow.Init(AID_RADARANTTESTSWITCHES, MainPanel);
	LandingAntSwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RadarAntTestSwitchesRow);
	RadarTestSwitch.Init(0, 95, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], RadarAntTestSwitchesRow);

	TestMonitorRotaryRow.Init(AID_TESTMONITORROTARY, MainPanel);
	TestMonitorRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], TestMonitorRotaryRow);

	SlewRateSwitchRow.Init(AID_SLEWRATESWITCH, MainPanel);
	SlewRateSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], SlewRateSwitchRow);

	RendezvousRadarRotaryRow.Init(AID_RENDEZVOUSRADARROTARY, MainPanel);
	RendezvousRadarRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], RendezvousRadarRotaryRow);

	StabContSwitchesRow.Init(AID_SATBCONTSWITCHES, MainPanel);
	DeadBandSwitch.Init(0, 5, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], StabContSwitchesRow);
	GyroTestLeftSwitch.Init(93, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], StabContSwitchesRow);
	GyroTestRightSwitch.Init(191, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], StabContSwitchesRow);
	GyroTestRightSwitch.SetDelayTime(1);
	AttitudeControlSwitchesRow.Init(AID_ATTITUDECONTROLSWITCHES, MainPanel);
	RollSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlSwitchesRow);
	PitchSwitch.Init(93, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlSwitchesRow);
	YawSwitch.Init(191, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], AttitudeControlSwitchesRow);

	TempMonitorIndRow.Init(AID_TEMPMONITORIND, MainPanel);
	TempMonitorInd.Init(srf[SRF_NEEDLE], TempMonitorIndRow, this);

	TempMonitorRotaryRow.Init(AID_TEMPMONITORROTARY, MainPanel);
	TempMonitorRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], TempMonitorRotaryRow);

	RCSSysQuadSwitchesRow.Init(AID_RCSSYSQUADSWITCHES, MainPanel);
	RCSSysQuad1Switch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);
	RCSSysQuad2Switch.Init(0, 95, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);
	RCSSysQuad3Switch.Init(58, 95, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);
	RCSSysQuad4Switch.Init(58, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RCSSysQuadSwitchesRow);

	LightingSwitchesRow.Init(AID_LIGHTINGSWITCHES, MainPanel);
	SidePanelsSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], LightingSwitchesRow);
	FloodSwitch.Init(0, 68, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], LightingSwitchesRow);

	FloodRotaryRow.Init(AID_FLOODROTARY, MainPanel);
	FloodRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], FloodRotaryRow);

	LampToneTestRotaryRow.Init(AID_LAMPTONETESTROTARY, MainPanel);
	LampToneTestRotary.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], LampToneTestRotaryRow);

	RightXPointerSwitchRow.Init(AID_RIGHTXPOINTERSWITCH, MainPanel);
	RightXPointerSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], RightXPointerSwitchRow);

	ExteriorLTGSwitchRow.Init(AID_EXTERIORLTGSWITCH, MainPanel);
	ExteriorLTGSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ExteriorLTGSwitchRow);

	Panel4LeftSwitchRow.Init(AID_PANEL4LEFTSWITCHROW, MainPanel);
	LeftACA4JetSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4LeftSwitchRow);
	LeftTTCATranslSwitch.Init(0, 141, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4LeftSwitchRow);

	Panel4RightSwitchRow.Init(AID_PANEL4RIGHTSWITCHROW, MainPanel);
	RightACA4JetSwitch.Init(0, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4RightSwitchRow);
	RightTTCATranslSwitch.Init(0, 141, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel4RightSwitchRow);

	RadarSignalStrengthAttenuator.Init(this, &TestMonitorRotary, &PGNS_SIG_STR_DISP_CB);

	RaderSignalStrengthMeterRow.Init(AID_LMRADARSIGNALSTRENGTH, MainPanel);
	RadarSignalStrengthMeter.Init(g_Param.pen[4], g_Param.pen[4], RaderSignalStrengthMeterRow, &RadarSignalStrengthAttenuator);
	RadarSignalStrengthMeter.SetSurface(srf[SRF_LMSIGNALSTRENGTH], 91, 90);

	RadarSlewSwitchRow.Init(AID_LMRADARSLEWSWITCH, MainPanel);
	RadarSlewSwitch.Init(0, 0, 39, 39, srf[SRF_FIVE_POS_SWITCH], NULL, RadarSlewSwitchRow);

	EventTimerSwitchRow.Init(AID_LM_EVENT_TIMER_SWITCHES, MainPanel);
	EventTimerCtlSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow, &EventTimerDisplay);
	EventTimerCtlSwitch.SetDelayTime(1);
	EventTimerStartSwitch.Init(55, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow, &EventTimerDisplay);
	EventTimerStartSwitch.SetDelayTime(1);
	EventTimerMinuteSwitch.Init(111, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow, TIME_UPDATE_MINUTES, &EventTimerDisplay);
	EventTimerSecondSwitch.Init(167, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EventTimerSwitchRow, TIME_UPDATE_SECONDS, &EventTimerDisplay);

	//
	// DSKY
	//

	DskySwitchRow.Init(AID_DSKY_KEY, MainPanel);
	DskySwitchVerb.Init(0, 20, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 0, 20);
	DskySwitchNoun.Init(0, 60, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 0, 60);
	DskySwitchPlus.Init(41, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 41, 0);
	DskySwitchMinus.Init(41, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 41, 40);
	DskySwitchZero.Init(41, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 41, 80);
	DskySwitchSeven.Init(82, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 82, 0);
	DskySwitchFour.Init(82, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 82, 40);
	DskySwitchOne.Init(82, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 82, 80);
	DskySwitchEight.Init(123, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 123, 0);
	DskySwitchFive.Init(123, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 123, 40);
	DskySwitchTwo.Init(123, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 123, 80);
	DskySwitchNine.Init(164, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 164, 0);
	DskySwitchSix.Init(164, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 164, 40);
	DskySwitchThree.Init(164, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 164, 80);
	DskySwitchClear.Init(205, 0, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 205, 0);
	DskySwitchProg.Init(205, 40, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 205, 40);
	DskySwitchKeyRel.Init(205, 80, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 205, 80);
	DskySwitchEnter.Init(246, 20, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 246, 20);
	DskySwitchReset.Init(246, 60, 38, 38, srf[SRF_DSKYKEY], srf[SRF_BORDER_38x38], DskySwitchRow, 246, 60);

	//
	// DEDA
	//

	DedaSwitchRow.Init(AID_LM_DEDA_KEYS, MainPanel);
	DedaSwitchPlus.Init(1 + 44 * 0, 1, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 0, 0);
	DedaSwitchMinus.Init(1 + 44 * 0, 45, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 0, 44);
	DedaSwitchZero.Init(1 + 44 * 0, 90, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 0, 88);
	DedaSwitchSeven.Init(1 + 44 * 1, 1, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 1, 0);
	DedaSwitchFour.Init(1 + 44 * 1, 45, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 1, 44);
	DedaSwitchOne.Init(1 + 44 * 1, 90, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 1, 88);
	DedaSwitchEight.Init(1 + 44 * 2, 0, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 2, 0);
	DedaSwitchFive.Init(1 + 44 * 2, 45, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 2, 44);
	DedaSwitchTwo.Init(1 + 44 * 2, 90, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 2, 88);
	DedaSwitchNine.Init(1 + 44 * 3, 1, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 3, 0);
	DedaSwitchSix.Init(1 + 44 * 3, 45, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 3, 44);
	DedaSwitchThree.Init(1 + 44 * 3, 90, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 3, 88);
	DedaSwitchClear.Init(1 + 44 * 4, 1, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 4, 0);
	DedaSwitchReadOut.Init(1 + 44 * 4, 45, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 4, 44);
	DedaSwitchEnter.Init(1 + 44 * 4, 90, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 4, 88);
	DedaSwitchHold.Init(1 + 44 * 2, 134, 40, 40, srf[SRF_DEDA_KEY], srf[SRF_BORDER_40x40], DedaSwitchRow, 44 * 2, 132);

	//		break;

	//	case LMPANEL_RIGHTPANEL: // LEM Right Panel
	Panel16CB1SwitchRow.Init(AID_LEM_P16_CB_ROW1, MainPanel);
	LMP_EVT_TMR_FDAI_DC_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
	SE_XPTR_DC_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
	RCS_B_ASC_FEED_1_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
	RCS_B_ASC_FEED_2_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
	RCS_B_ISOL_VLV_CB.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
	RCS_B_QUAD1_TCA_CB.Init(320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
	RCS_B_QUAD2_TCA_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
	RCS_B_QUAD3_TCA_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
	RCS_B_QUAD4_TCA_CB.Init(512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 10.0);
	RCS_B_CRSFD_CB.Init(576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
	RCS_B_TEMP_PRESS_DISP_FLAGS_CB.Init(640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
	RCS_B_PQGS_DISP_CB.Init(704, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
	RCS_B_MAIN_SOV_CB.Init(768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);
	PROP_DISP_ENG_OVRD_LOGIC_CB.Init(832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
	PROP_PQGS_CB.Init(896, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 2.0);
	PROP_ASC_HE_REG_CB.Init(1019, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB1SwitchRow, &LMPs28VBus, 5.0);

	Panel16CB2SwitchRow.Init(AID_LEM_P16_CB_ROW2, MainPanel);
	LTG_FLOOD_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 5.0);
	LTG_TRACK_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 20.0);
	LTG_ANUN_DOCK_COMPNT_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	LTG_MASTER_ALARM_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	EDS_CB_LOGIC_B.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	SCS_AEA_CB.Init(320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 10.0);
	SCS_ENG_ARM_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	SCS_ASA_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 20.0);
	SCS_AELD_CB.Init(512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 7.5);
	SCS_ATCA_CB.Init(576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 7.5);
	SCS_ABORT_STAGE_CB.Init(640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	SCS_ATCA_AGS_CB.Init(704, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 3.0);
	SCS_DES_ENG_OVRD_CB.Init(768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 10.0);
	INST_CWEA_CB.Init(832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	INST_SIG_SENSOR_CB.Init(896, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	INST_PCMTEA_CB.Init(1019, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	INST_SIG_CONDR_2_CB.Init(1083, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 2.0);
	ECS_SUIT_FLOW_CONT_CB.Init(1147, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB2SwitchRow, &LMPs28VBus, 5.0);

	Panel16CB3SwitchRow.Init(AID_LEM_P16_CB_ROW3, MainPanel);
	COMM_DISP_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	COMM_SE_AUDIO_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	COMM_VHF_XMTR_A_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
	COMM_VHF_RCVR_B_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	COMM_PRIM_SBAND_PA_CB.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 7.5);
	COMM_PRIM_SBAND_XCVR_CB.Init(320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
	COMM_SBAND_ANT_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	COMM_PMP_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	COMM_TV_CB.Init(512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	ECS_DISP_CB.Init(576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	ECS_GLYCOL_PUMP_SEC_CB.Init(640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
	ECS_LGC_PUMP_CB.Init(768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 5.0);
	ECS_CABIN_FAN_CONT_CB.Init(832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	ECS_CABIN_REPRESS_CB.Init(896, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	ECS_SUIT_FAN_2_CB.Init(1019, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 20.0);
	ECS_SUIT_FAN_DP_CB.Init(1083, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	ECS_DIVERT_VLV_CB.Init(1147, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);
	ECS_CO2_SENSOR_CB.Init(1211, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB3SwitchRow, &LMPs28VBus, 2.0);

	Panel16CB4SwitchRow.Init(AID_LEM_P16_CB_ROW4, MainPanel);
	RCS_QUAD_1_LMP_HTR_CB.Init(0, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
	RCS_QUAD_2_LMP_HTR_CB.Init(64, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
	RCS_QUAD_3_LMP_HTR_CB.Init(128, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
	RCS_QUAD_4_LMP_HTR_CB.Init(192, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 7.5);
	HTR_DISP_CB.Init(256, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
	HTR_SBD_ANT_CB.Init(320, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 5.0);
	CAMR_SEQ_CB.Init(384, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
	EPS_DISP_CB.Init(448, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
	LMPDCBusVoltCB.Init(512, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
	LMPInverter2CB.Init(576, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 30.0);
	LMPAscECAMainCB.Init(640, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 2.0);
	LMPAscECAContCB.Init(704, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 3.0);
	LMPDesECAMainCB.Init(768, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 3.0);
	LMPDesECAContCB.Init(832, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 5.0);
	LMPXLunarBusTieCB.Init(896, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 50.0);
	LMPCrossTieBusCB.Init(1019, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 100.0);
	LMPCrossTieBalCB.Init(1083, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &LMPs28VBus, 30.0);
	LMPBatteryFeedTieCB1.Init(1147, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &BTB_LMP_B, 100.0);
	LMPBatteryFeedTieCB2.Init(1211, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel16CB4SwitchRow, &BTB_LMP_C, 100.0);

	EPSP14VoltMeterSwitchRow.Init(AID_LM_EPS_DC_VOLTMETER, MainPanel);
	EPSDCVoltMeter.Init(g_Param.pen[4], g_Param.pen[4], EPSP14VoltMeterSwitchRow, this, srf[SRF_DCVOLTS]);

	EPSP14AmMeterSwitchRow.Init(AID_LM_EPS_DC_AMMETER, MainPanel);
	EPSDCAmMeter.Init(g_Param.pen[4], g_Param.pen[4], EPSP14AmMeterSwitchRow, this, srf[SRF_DCAMPS]);

	EPSLeftControlArea.Init(AID_LM_EPS_LEFT_CONTROLS, MainPanel);
	EPSInverterSwitch.Init(142, 135, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], EPSLeftControlArea, this, &INV_1, &INV_2);
	EPSEDVoltSelect.Init(319 - 314, 868 - 728, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], EPSLeftControlArea);
	EPSMonitorSelectRotary.Init(117, 17, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], EPSLeftControlArea);

	DSCHiVoltageSwitchRow.Init(AID_DSC_HIGH_VOLTAGE_SWITCHES, MainPanel);
	DSCSEBat1HVSwitch.Init(0, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this, &ECA_1a, 1, 0);
	DSCSEBat1HVSwitch.SetDelayTime(1);
	DSCSEBat2HVSwitch.Init(69, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this, &ECA_1b, 1, 0);
	DSCSEBat2HVSwitch.SetDelayTime(1);
	DSCCDRBat3HVSwitch.Init(151, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this, &ECA_2a, 1, 0);
	DSCCDRBat3HVSwitch.SetDelayTime(1);
	DSCCDRBat4HVSwitch.Init(220, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCHiVoltageSwitchRow, this, &ECA_2b, 1, 0);
	DSCCDRBat4HVSwitch.SetDelayTime(1);
	DSCBattFeedSwitch.Init(291, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], DSCHiVoltageSwitchRow, this);
	DSCBattFeedSwitch.SetDelayTime(1);

	DSCLoVoltageSwitchRow.Init(AID_DSC_LOW_VOLTAGE_SWITCHES, MainPanel);
	DSCSEBat1LVSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCLoVoltageSwitchRow, this, &ECA_1a, 2, 0);
	DSCSEBat1LVSwitch.SetDelayTime(1);
	DSCSEBat2LVSwitch.Init(69, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCLoVoltageSwitchRow, this, &ECA_1b, 2, 0);
	DSCSEBat2LVSwitch.SetDelayTime(1);
	DSCCDRBat3LVSwitch.Init(151, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCLoVoltageSwitchRow, this, &ECA_2a, 2, 0);
	DSCCDRBat3LVSwitch.SetDelayTime(1);
	DSCCDRBat4LVSwitch.Init(220, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], DSCLoVoltageSwitchRow, this, &ECA_2b, 2, 0);
	DSCCDRBat4LVSwitch.SetDelayTime(1);

	DSCBatteryTBSwitchRow.Init(AID_DSC_BATTERY_TALKBACKS, MainPanel);
	DSCBattery1TB.Init(0, 0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow, scera2.GetSwitch(4, 1), scera2.GetSwitch(4, 2));
	DSCBattery2TB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow, scera2.GetSwitch(4, 3), scera2.GetSwitch(4, 4));
	DSCBattery3TB.Init(152, 0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow, scera2.GetSwitch(4, 5), scera2.GetSwitch(4, 6));
	DSCBattery4TB.Init(222, 0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow, scera2.GetSwitch(4, 7), scera2.GetSwitch(4, 8));
	DSCBattFeedTB.Init(292, 0, 23, 23, srf[SRF_INDICATOR], DSCBatteryTBSwitchRow);

	ASCBatteryTBSwitchRow.Init(AID_ASC_BATTERY_TALKBACKS, MainPanel);
	ASCBattery5ATB.Init(0, 0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow, scera2.GetSwitch(4, 11), false);
	ASCBattery5BTB.Init(70, 0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow, scera2.GetSwitch(4, 9), false);
	ASCBattery6ATB.Init(138, 0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow, scera2.GetSwitch(4, 10), false);
	ASCBattery6BTB.Init(221, 0, 23, 23, srf[SRF_INDICATOR], ASCBatteryTBSwitchRow, scera2.GetSwitch(4, 12), false);

	ASCBatterySwitchRow.Init(AID_ASC_BATTERY_SWITCHES, MainPanel);
	ASCBat5SESwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this, &ECA_3a, 1, 1);
	ASCBat5SESwitch.SetDelayTime(1);
	ASCBat5CDRSwitch.Init(69, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this, &ECA_3b, 1, 1);
	ASCBat5CDRSwitch.SetDelayTime(1);
	ASCBat6CDRSwitch.Init(138, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this, &ECA_4a, 1, 1);
	ASCBat6CDRSwitch.SetDelayTime(1);
	ASCBat6SESwitch.Init(221, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ASCBatterySwitchRow, this, &ECA_4b, 1, 1);
	ASCBat6SESwitch.SetDelayTime(1);
	UpDataSquelchSwitch.Init(295, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ASCBatterySwitchRow);

	Panel12AudioCtlSwitchRow.Init(AID_LM_P12_AUDIO_CONT_SWITCH, MainPanel);
	Panel12AudioCtlSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12AudioCtlSwitchRow);
	Panel12UpdataLinkSwitchRow.Init(AID_LM_P12_UPDATA_LINK_SWITCH, MainPanel);
	Panel12UpdataLinkSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12UpdataLinkSwitchRow);

	Panel12AudioControlSwitchRow.Init(AID_LM_P12_AUDIO_CONTROLS, MainPanel);
	LMPAudSBandSwitch.Init(2, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
	LMPAudICSSwitch.Init(106, 5, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
	LMPAudRelaySwitch.Init(206, 0, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel12AudioControlSwitchRow);
	LMPAudSBandVol.Init(4, 61, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow); // FIXME: Need flashing border for this
	LMPAudICSVol.Init(108, 61, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
	LMPAudVOXSwitch.Init(206, 70, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
	LMPAudVHFASwitch.Init(0, 165, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
	LMPAudVHFBSwitch.Init(104, 165, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AudioControlSwitchRow);
	LMPAudVOXSens.Init(210, 125, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
	LMPAudVHFAVol.Init(5, 225, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
	LMPAudVHFBVol.Init(108, 225, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);
	LMPAudMasterVol.Init(210, 225, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12AudioControlSwitchRow);

	Panel12CommSwitchRow1.Init(AID_LM_P12_COMM_SWITCHES_ROW1, MainPanel);
	SBandModulateSwitch.Init(0, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
	SBandXCvrSelSwitch.Init(546 - 452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
	SBandPASelSwitch.Init(631 - 452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
	SBandVoiceSwitch.Init(721 - 452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
	SBandPCMSwitch.Init(813 - 452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);
	SBandRangeSwitch.Init(889 - 452, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow1);

	Panel12CommSwitchRow2.Init(AID_LM_P12_COMM_SWITCHES_ROW2, MainPanel);
	VHFAVoiceSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
	VHFARcvrSwtich.Init(634 - 568, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
	VHFBVoiceSwitch.Init(698 - 568, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
	VHFBRcvrSwtich.Init(764 - 568, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
	TLMBiomedSwtich.Init(828 - 568, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);
	TLMBitrateSwitch.Init(894 - 568, 0, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow2);

	Panel12CommSwitchRow3.Init(AID_LM_P12_COMM_SWITCHES_ROW3, MainPanel);
	VHFASquelch.Init(0, 4, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12CommSwitchRow3);
	VHFBSquelch.Init(136, 4, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel12CommSwitchRow3);
	TapeRecorderTB.Init(841 - 636, 1214 - 1189, 23, 23, srf[SRF_INDICATOR], Panel12CommSwitchRow3);
	TapeRecorderSwitch.Init(892 - 636, 1212 - 1189, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel12CommSwitchRow3);

	ComPitchMeterRow.Init(AID_LMPITCHDEGS, MainPanel, &COMM_DISP_CB);
	ComPitchMeter.Init(g_Param.pen[4], g_Param.pen[4], ComPitchMeterRow, this, srf[SRF_LMPITCHDEGS]);
	ComYawMeterRow.Init(AID_LMYAWDEGS, MainPanel, &COMM_DISP_CB);
	ComYawMeter.Init(g_Param.pen[4], g_Param.pen[4], ComYawMeterRow, this, srf[SRF_LMYAWDEGS]);
	Panel12SignalStrengthMeterRow.Init(AID_LMSIGNALSTRENGTH, MainPanel, &COMM_DISP_CB);
	Panel12SignalStrengthMeter.Init(g_Param.pen[4], g_Param.pen[4], Panel12SignalStrengthMeterRow, this, srf[SRF_LMSIGNALSTRENGTH]);

	Panel12AntTrackModeSwitchRow.Init(AID_LM_P12_COMM_ANT_TRACK_MODE_SWITCH, MainPanel);
	Panel12AntTrackModeSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel12AntTrackModeSwitchRow);

	Panel12VHFAntSelSwitchRow.Init(AID_LM_P12_COMM_ANT_VHF_SEL_KNOB, MainPanel);
	Panel12VHFAntSelKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12VHFAntSelSwitchRow);
	Panel12SBandAntSelSwitchRow.Init(AID_LM_P12_COMM_ANT_SBD_SEL_KNOB, MainPanel);
	Panel12SBandAntSelKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12SBandAntSelSwitchRow);
	Panel12AntPitchSwitchRow.Init(AID_LM_P12_COMM_ANT_PITCH_KNOB, MainPanel);
	Panel12AntPitchKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12AntPitchSwitchRow);
	Panel12AntYawSwitchRow.Init(AID_LM_P12_COMM_ANT_YAW_KNOB, MainPanel);
	Panel12AntYawKnob.Init(0, 0, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel12AntYawSwitchRow);

	LMPManualEngineStopSwitchRow.Init(AID_LMP_MANUAL_ENGINE_STOP_SWITCH, MainPanel);
	LMPManualEngineStop.Init(0, 0, 68, 69, srf[SRF_LMENGINE_START_STOP_BUTTONS], srf[SRF_BORDER_68x69], LMPManualEngineStopSwitchRow, 0, 0, NULL, this);

	AGSOperateSwitchRow.Init(AID_LM_AGS_OPERATE_SWITCH, MainPanel);
	AGSOperateSwitch.Init(0, 0, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], AGSOperateSwitchRow);

	//		break;

	//	case LMPANEL_LEFTPANEL:
	Panel11CB1SwitchRow.Init(AID_LEM_P11_CB_ROW1, MainPanel);
	SE_WND_HTR_AC_CB.Init(3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	HE_PQGS_PROP_DISP_AC_CB.Init(67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	SBD_ANT_AC_CB.Init(131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	ORDEAL_AC_CB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	AGS_AC_CB.Init(318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	AOT_LAMP_ACB_CB.Init(382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	LMP_FDAI_AC_CB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	NUM_LTG_AC_CB.Init(510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusB, 2.0);
	AC_B_INV_2_FEED_CB.Init(574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_2, 5.0);
	AC_B_INV_1_FEED_CB.Init(638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_1, 5.0);
	AC_A_INV_2_FEED_CB.Init(702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_2, 5.0);
	AC_A_INV_1_FEED_CB.Init(766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &INV_1, 5.0);
	AC_A_BUS_VOLT_CB.Init(830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
	CDR_WND_HTR_AC_CB.Init(894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
	TAPE_RCDR_AC_CB.Init(958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
	AOT_LAMP_ACA_CB.Init(1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
	RDZ_RDR_AC_CB.Init(1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
	DECA_GMBL_AC_CB.Init(1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);
	INTGL_LTG_AC_CB.Init(1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB1SwitchRow, &ACBusA, 2.0);

	Panel11CB2SwitchRow.Init(AID_LEM_P11_CB_ROW2, MainPanel);
	RCS_A_MAIN_SOV_CB.Init(3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0);
	RCS_A_QUAD4_TCA_CB.Init(67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0);
	RCS_A_QUAD3_TCA_CB.Init(131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0);
	RCS_A_QUAD2_TCA_CB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0);
	RCS_A_QUAD1_TCA_CB.Init(318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 10.0);
	RCS_A_ISOL_VLV_CB.Init(382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0);
	RCS_A_ASC_FEED_2_CB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0);
	RCS_A_ASC_FEED_1_CB.Init(510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 5.0);
	THRUST_DISP_CB.Init(574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	MISSION_TIMER_CB.Init(638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	CDR_XPTR_CB.Init(702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	RNG_RT_ALT_RT_DC_CB.Init(766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	GASTA_DC_CB.Init(830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	CDR_FDAI_DC_CB.Init(894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	COAS_DC_CB.Init(958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	ORDEAL_DC_CB.Init(1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &CDRs28VBus, 2.0);
	RNG_RT_ALT_RT_AC_CB.Init(1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &ACBusA, 2.0);
	GASTA_AC_CB.Init(1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &ACBusA, 2.0);
	CDR_FDAI_AC_CB.Init(1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB2SwitchRow, &ACBusA, 2.0);

	Panel11CB3SwitchRow.Init(AID_LEM_P11_CB_ROW3, MainPanel); // 184,431 to 1433,459
	PROP_DES_HE_REG_VENT_CB.Init(3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 5.0);
	HTR_RR_STBY_CB.Init(67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 7.5);
	HTR_RR_OPR_CB.Init(131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 7.5);
	HTR_LR_CB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 5.0);
	HTR_DOCK_WINDOW_CB.Init(318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	HTR_AOT_CB.Init(382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	INST_SIG_CONDR_1_CB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	CDR_SCS_AEA_CB.Init(510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 10.0);
	CDR_SCS_ABORT_STAGE_CB.Init(574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	CDR_SCS_ATCA_CB.Init(638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	CDR_SCS_AELD_CB.Init(702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 7.5);
	SCS_ENG_CONT_CB.Init(766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	SCS_ATT_DIR_CONT_CB.Init(830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 10.0);
	SCS_ENG_START_OVRD_CB.Init(894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	SCS_DECA_PWR_CB.Init(958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 20.0);
	EDS_CB_LG_FLAG.Init(1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	EDS_CB_LOGIC_A.Init(1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	CDR_LTG_UTIL_CB.Init(1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);
	CDR_LTG_ANUN_DOCK_COMPNT_CB.Init(1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB3SwitchRow, &CDRs28VBus, 2.0);

	Panel11CB4SwitchRow.Init(AID_LEM_P11_CB_ROW4, MainPanel);
	RCS_QUAD_4_CDR_HTR_CB.Init(3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
	RCS_QUAD_3_CDR_HTR_CB.Init(67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
	RCS_QUAD_2_CDR_HTR_CB.Init(131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
	RCS_QUAD_1_CDR_HTR_CB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
	ECS_SUIT_FAN_1_CB.Init(318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 20.0);
	ECS_CABIN_FAN_1_CB.Init(382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
	ECS_GLYCOL_PUMP_2_CB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
	ECS_GLYCOL_PUMP_1_CB.Init(510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
	ECS_GLYCOL_PUMP_AUTO_XFER_CB.Init(574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
	COMM_UP_DATA_LINK_CB.Init(638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
	COMM_SEC_SBAND_XCVR_CB.Init(702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
	COMM_SEC_SBAND_PA_CB.Init(766, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
	COMM_VHF_XMTR_B_CB.Init(830, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
	COMM_VHF_RCVR_A_CB.Init(894, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
	COMM_CDR_AUDIO_CB.Init(958, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
	PGNS_SIG_STR_DISP_CB.Init(1022, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 2.0);
	PGNS_LDG_RDR_CB.Init(1086, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 10.0);
	PGNS_RNDZ_RDR_CB.Init(1150, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 15.0);
	LGC_DSKY_CB.Init(1214, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 7.5);
	IMU_SBY_CB.Init(1278, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 5.0);
	IMU_OPR_CB.Init(1342, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB4SwitchRow, &CDRs28VBus, 20.0);

	Panel11CB5SwitchRow.Init(AID_LEM_P11_CB_ROW5, MainPanel);
	CDRBatteryFeedTieCB1.Init(3, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &BTB_CDR_B, 100.0);
	CDRBatteryFeedTieCB2.Init(67, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &BTB_CDR_C, 100.0);
	CDRCrossTieBalCB.Init(131, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 30.0);
	CDRCrossTieBusCB.Init(195, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 100.0);
	CDRXLunarBusTieCB.Init(318, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 50.0);
	CDRDesECAContCB.Init(382, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 5.0);
	CDRDesECAMainCB.Init(446, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 3.0);
	CDRAscECAContCB.Init(510, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 3.0);
	CDRAscECAMainCB.Init(574, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 2.0);
	CDRInverter1CB.Init(638, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 30.0);
	CDRDCBusVoltCB.Init(702, 0, 29, 29, srf[SRF_CIRCUITBRAKER], srf[SRF_BORDER_29x29], Panel11CB5SwitchRow, &CDRs28VBus, 2.0);

	// Panel 5 is 1000,1300 to 1560, 1620
	Panel5SwitchRow.Init(AID_LEM_PANEL_5, MainPanel);
	TimerContSwitch.Init(233, 64, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 0);
	TimerContSwitch.SetDelayTime(1);
	TimerSlewHours.Init(333, 64, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 1);
	TimerSlewMinutes.Init(405, 64, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 2);
	TimerSlewSeconds.Init(477, 64, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel5SwitchRow, this, 3);
	LtgORideAnunSwitch.Init(323, 168, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
	LtgORideNumSwitch.Init(380, 168, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
	LtgORideIntegralSwitch.Init(437, 168, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
	LtgSidePanelsSwitch.Init(494, 168, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel5SwitchRow);
	LtgFloodOhdFwdKnob.Init(173, 243, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel5SwitchRow);
	LtgAnunNumKnob.Init(333, 243, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel5SwitchRow);
	LtgIntegralKnob.Init(457, 243, 84, 84, srf[SRF_LEMROTARY], srf[SRF_BORDER_84x84], Panel5SwitchRow);
	PlusXTranslationButton.Init(46, 256, 79, 68, srf[SRF_LMTRANSLBUTTON], srf[SRF_BORDER_84x84], Panel5SwitchRow);
	ManualEngineStart.Init(32, 114, 68, 69, srf[SRF_LMENGINE_START_STOP_BUTTONS], srf[SRF_BORDER_68x69], Panel5SwitchRow, 0, 138, &CDRManualEngineStop, this);
	CDRManualEngineStop.Init(32, 0, 68, 69, srf[SRF_LMENGINE_START_STOP_BUTTONS], srf[SRF_BORDER_68x69], Panel5SwitchRow, 0, 0, &ManualEngineStart, this);

	// Panel 8 is  431,916 to 1574,1258
	Panel8SwitchRow.Init(AID_LEM_PANEL_8, MainPanel);
	EDMasterArm.Init(861 - 431, 978 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDDesVent.Init(935 - 431, 978 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDDesVent.SetDelayTime(2);
	EDASCHeSel.Init(571, 62, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDDesPrpIsol.Init(643 - 431, 978 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDDesPrpIsol.SetDelayTime(2);
	EDLGDeploy.Init(784 - 431, 1078 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDDesPrpIsol.SetDelayTime(2);
	EDHePressRCS.Init(861 - 431, 1078 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDHePressRCS.SetDelayTime(2);
	EDHePressDesStart.Init(935 - 431, 1078 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDHePressDesStart.SetDelayTime(2);
	EDHePressASC.Init(1002 - 431, 1078 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDHePressASC.SetDelayTime(2);
	EDStage.Init(783 - 431, 1175 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDStage.InitGuard(304, 252, 102, 59, srf[SRF_LEM_STAGESWITCH], srf[SRF_BORDER_34x39]);
	EDStageRelay.Init(1002 - 431, 1182 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	EDDesFuelVent.Init(36, 100, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	EDDesOxidVent.Init(109, 100, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	EDLGTB.Init(790 - 431, 1033 - 916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow, scera1.GetSwitch(13, 9));
	EDDesFuelVentTB.Init(472 - 431, 960 - 916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow, DPSPropellant.GetFuelVentValve2(), false);
	EDDesOxidVentTB.Init(545 - 431, 960 - 916, 23, 23, srf[SRF_INDICATOR], Panel8SwitchRow, DPSPropellant.GetOxidVentValve2(), false);
	// Audio stuff
	CDRAudSBandSwitch.Init(1189 - 431, 949 - 916, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	CDRAudICSSwitch.Init(1288 - 431, 949 - 916, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	CDRAudRelaySwitch.Init(1394 - 431, 938 - 916, 34, 39, srf[SRF_LMTWOPOSLEVER], srf[SRF_BORDER_34x39], Panel8SwitchRow);
	CDRAudVOXSwitch.Init(1393 - 431, 1017 - 916, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	CDRAudioControlSwitch.Init(1046, 101, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	CDRAudVHFASwitch.Init(1189 - 431, 1116 - 916, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	CDRAudVHFBSwitch.Init(1288 - 431, 1116 - 916, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], Panel8SwitchRow);
	CDRAudSBandVol.Init(759, 91, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow); // FIXME: Need flashing border for this
	CDRAudVHFAVol.Init(759, 258, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
	CDRAudVHFBVol.Init(858, 258, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
	CDRAudICSVol.Init(858, 91, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
	CDRAudMasterVol.Init(963, 258, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
	CDRAudVOXSens.Init(963, 158, 25, 78, srf[SRF_THUMBWHEEL_LARGEFONTS], NULL, Panel8SwitchRow);
	CDRCOASSwitch.Init(1063, 266, 34, 39, srf[SRF_LMTHREEPOSLEVER], srf[SRF_BORDER_34x29], Panel8SwitchRow);

	ORDEALSwitchesRow.Init(AID_ORDEALSWITCHES, MainPanel);
	ORDEALFDAI1Switch.Init(55, 43, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALFDAI2Switch.Init(168, 43, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALEarthSwitch.Init(264, 43, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALLightingSwitch.Init(55, 132, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALModeSwitch.Init(215, 132, 34, 29, srf[SRF_SWITCHUP], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALSlewSwitch.Init(264, 132, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], ORDEALSwitchesRow);
	ORDEALAltSetRotary.Init(346, 63, 84, 84, srf[SRF_ORDEAL_ROTARY], srf[SRF_BORDER_84x84], ORDEALSwitchesRow);

	PanelOrdeal.Init(ORDEALSwitchesRow, this); 	// dummy switch/display for checklist controller

	// LM Panel AOTVIEW
	RRGyroSelSwitchRow.Init(AID_RR_GYRO_SEL_SWITCH, MainPanel);
	RRGyroSelSwitch.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], RRGyroSelSwitchRow);

	// ECS Panel
	ECSSuitGasDiverterSwitchRow.Init(IDB_LEM_SGD_LEVER, MainPanel);
	SuitGasDiverterSwitch.Init(0, 0, 126, 131, srf[SRF_LEM_SGD_LEVER], srf[SRF_BORDER_126x131], ECSSuitGasDiverterSwitchRow);

    OxygenControlSwitchRow.Init(AID_LEM_ECS_OCM, MainPanel);
    CabinRepressValveSwitch.Init(88, 4, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], OxygenControlSwitchRow);
    PLSSFillValve.Init(340, 7, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], OxygenControlSwitchRow);
    PressRegAValve.Init(532, 53, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], OxygenControlSwitchRow);
    PressRegBValve.Init(531, 239, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], OxygenControlSwitchRow);
    DESO2Valve.Init(4, 163, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], OxygenControlSwitchRow);
    ASCO2Valve1.Init(171, 161, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], OxygenControlSwitchRow, &IntlkOvrd, &DESO2Valve);
    ASCO2Valve2.Init(342, 162, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], OxygenControlSwitchRow, &IntlkOvrd, &DESO2Valve);
    IntlkOvrd.Init(234, 73, 68, 68, srf[SRF_LEM_INTLK_OVRD], srf[SRF_BORDER_68x68], OxygenControlSwitchRow);

    SuitIsolSwitchRow.Init(AID_LEM_ISOL_ROTARY, MainPanel);
    CDRSuitIsolValve.Init(22, 59, 169, 168, srf[SRF_LEM_ISOL_ROTARY], srf[SRF_BORDER_169x168], SuitIsolSwitchRow);
    CDRActuatorOvrd.Init(204, 60, 67, 64, srf[SRF_LEM_ACT_OVRD], srf[SRF_BORDER_67x64], SuitIsolSwitchRow);
	CDRActuatorOvrd.SetDelayTime(1);
    LMPSuitIsolValve.Init(22+281 , 59, 169, 168, srf[SRF_LEM_ISOL_ROTARY], srf[SRF_BORDER_169x168], SuitIsolSwitchRow);
    LMPActuatorOvrd.Init(485, 60, 67, 64, srf[SRF_LEM_ACT_OVRD], srf[SRF_BORDER_67x64], SuitIsolSwitchRow);
	LMPActuatorOvrd.SetDelayTime(1);

    WaterControlSwitchRow.Init(AID_LEM_ECS_WCM, MainPanel);
    SecEvapFlowValve.Init(74, 30, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], WaterControlSwitchRow);
    PrimEvap2FlowValve.Init(240, 43, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], WaterControlSwitchRow);
    DESH2OValve.Init(279, 185, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], WaterControlSwitchRow);
    PrimEvap1FlowValve.Init(256, 346, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], WaterControlSwitchRow);
    WaterTankSelectValve.Init(33, 398, 210, 200, srf[SRF_LEM_H20_SEL], srf[SRF_BORDER_210x200], WaterControlSwitchRow);
    SuitTempValve.Init(258, 721, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], WaterControlSwitchRow, (h_Pipe *) Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROL"), (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:HXFLOWCONTROLBYPASS"));

    ASCH2OSwitchRow.Init(AID_LEM_ASC_H2O, MainPanel);
    ASCH2OValve.Init(0, 0, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], ASCH2OSwitchRow);

    GarmentCoolingSwitchRow.Init(AID_LEM_GARMENT_COOL, MainPanel);
    LiquidGarmentCoolingValve.Init(0, 0, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], GarmentCoolingSwitchRow, (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROL"), (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:LCGFLOWCONTROLBYPASS"));

    SuitCircuitAssySwitchRow.Init(AID_LEM_SUIT_CIRCUIT_ASSY, MainPanel);
    SuitCircuitReliefValveSwitch.Init(67, 6, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], SuitCircuitAssySwitchRow);
    CabinGasReturnValveSwitch.Init(652, 11, 115, 115, srf[SRF_LEM_ECS_ROTARY], srf[SRF_BORDER_115x115], SuitCircuitAssySwitchRow);
	CO2CanisterSelectSwitch.Init(376, 249, 122, 265, srf[SRF_LEM_CAN_SEL], srf[SRF_BORDER_122x265], SuitCircuitAssySwitchRow);
    CO2CanisterPrimValve.Init(101, 623, 225, 224, srf[SRF_LEM_PRIM_C02], srf[SRF_BORDER_225x224], SuitCircuitAssySwitchRow);
    CO2CanisterPrimVent.Init(13, 711, 51, 54, srf[SRF_LEMVENT], srf[SRF_BORDER_51x54], SuitCircuitAssySwitchRow);
    CO2CanisterSecValve.Init(433, 617, 205, 205, srf[SRF_LEM_SEC_C02], srf[SRF_BORDER_205x205], SuitCircuitAssySwitchRow);
    CO2CanisterSecVent.Init(641, 599, 51, 54, srf[SRF_LEMVENT], srf[SRF_BORDER_51x54], SuitCircuitAssySwitchRow);
	WaterSepSelectSwitch.Init(720, 678, 30, 144, srf[SRF_LEM_H20_SEP], srf[SRF_BORDER_30x144], SuitCircuitAssySwitchRow);

    // Upper Hatch
    UpperHatchHandleSwitchRow.Init(AID_LEM_UPPER_HATCH_HANDLE, MainPanel);
    UpperHatchHandle.Init(0, 0, 286, 197, srf[SRF_LEM_U_HATCH_HNDL], srf[SRF_BORDER_286x197], UpperHatchHandleSwitchRow, (h_Pipe *)Panelsdk.GetPointerByString("HYDRAULIC:CABINOVHDHATCHVALVE"), &OverheadHatch);

	UpperHatchValveSwitchRow.Init(AID_LEM_UPPER_HATCH_VALVE, MainPanel);
	UpperHatchReliefValve.Init(0, 0, 104, 106, srf[SRF_LEM_U_HATCH_REL_VLV], srf[SRF_BORDER_104x106], UpperHatchValveSwitchRow);

	UilityLightSwitchRow.Init(AID_LEM_UTILITY_LT, MainPanel);
	UtilityLightSwitchCDR.Init(0, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], UilityLightSwitchRow);
	UtilityLightSwitchLMP.Init(150, 0, 34, 29, srf[SRF_LMTHREEPOSSWITCH], srf[SRF_BORDER_34x29], UilityLightSwitchRow);

    // Forward Hatch
	ForwardHatchHandleSwitchRow.Init(AID_LEM_FWD_HATCH_HANDLE, MainPanel);
	ForwardHatchHandle.Init(0, 135, 360, 316, srf[SRF_LEM_F_HATCH_HNDL], srf[SRF_BORDER_360x316], ForwardHatchHandleSwitchRow, (h_Tank *)Panelsdk.GetPointerByString("HYDRAULIC:CABIN"), &ForwardHatch);

	ForwardHatchValveSwitchRow.Init(AID_LEM_FWD_HATCH_VALVE, MainPanel);
	ForwardHatchReliefValve.Init(0, 0, 178, 187, srf[SRF_LEM_F_HATCH_REL_VLV], srf[SRF_BORDER_178x187], ForwardHatchValveSwitchRow);

	// Radar Tape
	// Used to be in lemsystems, but placed here for proper loading of both tape bitmaps
	RadarTape.Init(this, &RNG_RT_ALT_RT_DC_CB, &RNG_RT_ALT_RT_AC_CB, srf[SRF_RADAR_TAPE], srf[SRF_RADAR_TAPE2]);
}

void LEM::RCSHeaterSwitchToggled(ToggleSwitch *s, int *pump) {

	if (s->IsUp())
		*pump = SP_PUMP_AUTO;
	else if (s->IsCenter())
		*pump = SP_PUMP_OFF;
	else if (s->IsDown())
		*pump = SP_PUMP_ON;
}

void LEM::PanelSwitchToggled(ToggleSwitch *s) {

	//RCS Switchable Heaters//

	//RCS Heater 1
	if (s == &RCSSysQuad1Switch) {
		RCSHeaterSwitchToggled(s,
			(int*)Panelsdk.GetPointerByString("ELECTRIC:QUAD1HTRSYS2:PUMP"));

	}
	//RCS Heater 2
	else if (s == &RCSSysQuad2Switch) {
		RCSHeaterSwitchToggled(s,
			(int*)Panelsdk.GetPointerByString("ELECTRIC:QUAD2HTRSYS2:PUMP"));

	}
	//RCS Heater 3
	else if (s == &RCSSysQuad3Switch) {
		RCSHeaterSwitchToggled(s,
			(int*)Panelsdk.GetPointerByString("ELECTRIC:QUAD3HTRSYS2:PUMP"));

	}
	//RCS Heater 4
	else if (s == &RCSSysQuad4Switch) {
		RCSHeaterSwitchToggled(s,
			(int*)Panelsdk.GetPointerByString("ELECTRIC:QUAD4HTRSYS2:PUMP"));

	}
}

void LEM::PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) {

}

void LEM::PanelRotationalSwitchChanged(RotationalSwitch *s) {

	//Oxygen Control//

		//DES O2 Tank Valve
		if (s == &DESO2Valve) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:DESO2MANIFOLD:OUT");
				if (DESO2Valve.GetState() == 0) {
					vlv->Open();
				}
				else {
					vlv->Close();
				}
			}

		//ASC O2 Tank 1 Valve
		else if (s == &ASCO2Valve1) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK1:OUT");
			if (ASCO2Valve1.GetState() == 0) {
				vlv->Open();
			}
			else {
				vlv->Close();
			}
		}

		//ASC O2 Tank 2 Valve
		else if (s == &ASCO2Valve2) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:ASCO2TANK2:OUT");
			if (ASCO2Valve2.GetState() == 0) {
				vlv->Open();
			}
			else {
				vlv->Close();
			}
		}

		//PLSS Fill Valve
		else if (s == &PLSSFillValve) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:O2MANIFOLD:OUT2");
			if (PLSSFillValve.GetState() == 0) {
				vlv->Open();
			}
			else {
				vlv->Close();
			}
		}
		
		//CDR Suit Isol Valve
		else if (s == &CDRSuitIsolValve) {
			h_Valve * sfvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:OUT");
			h_Valve * scinvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:LEAK");
			h_Valve * scoutvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:OUT");
			h_Valve * dcvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:CDRSUIT:OUT2");
			//Suit Disconnect
			if (CDRSuitIsolValve.GetState() == 1) {
				sfvlv->Close();
				scinvlv->Open();
				scoutvlv->Close();
				dcvlv->Close();
				//dcvlv->Open();   //Use this for when it can connect to a PLSS
			}
			//Suit Flow
			else {
				sfvlv->Open();
				scinvlv->Close();
				scoutvlv->Open();
				dcvlv->Close();
			}
		}
		
		//LMP Suit Isol Valve
		else if (s == &LMPSuitIsolValve) {
			h_Valve * sfvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:SUITCIRCUITHEATEXCHANGERHEATING:OUT2");
			h_Valve * scinvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUITDISCONNECTVALVE");
			h_Valve * scoutvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:OUT");
			h_Valve * dcvlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:LMPSUIT:OUT2");
			//Suit Disconnect
			if (LMPSuitIsolValve.GetState() == 1) {
				sfvlv->Close();
				scinvlv->Open();
				scoutvlv->Close();
				dcvlv->Close();
				//dcvlv->Open();   //Use this for when it can connect to a PLSS
			}
			//Suit Flow
			else {
				sfvlv->Open();
				scinvlv->Close();
				scoutvlv->Open();
				dcvlv->Close();
			}
		}

		//Suit Fan Control
		else if (s == &SuitFanRotary) {
			//OFF
			if (SuitFanRotary.GetState() == 0) {
				SuitFan1->SetPumpOff();
				SuitFan2->SetPumpOff();
			}
			//SUIT FAN 1
			else if (SuitFanRotary.GetState() == 1) {
				SuitFan1->SetPumpOn();
				SuitFan2->SetPumpOff();
			}
			//SUIT FAN 2
			else {
				SuitFan1->SetPumpOff();
				SuitFan2->SetPumpOn();
			}
		}

	//Water Control//

		//DES H2O Valve
		else if (s == &DESH2OValve) {
			if (stage < 2)
			{
				h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:DESH2OTANK:OUT2");
				if (DESH2OValve.GetState() == 0) {
					vlv->Open();
				}
				else {
					vlv->Close();
				}
			}
		}

		//ASC H2O Valve
		else if (s == &ASCH2OValve) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:ASCH2OTANK1:OUT");
			if (ASCH2OValve.GetState() == 0) {
				vlv->Open();
			}
			else {
				vlv->Close();
			}
		}


	//Glycol Loop Control//

		//Prim Evap Flow 1 Valve
		else if (s == &PrimEvap1FlowValve) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:PRIMREG:OUT");
			if (PrimEvap1FlowValve.GetState() == 0) {
				vlv->Open();
			}
			else {
				vlv->Close();
			}
		}

		//Prim Evap Flow 2 Valve
		else if (s == &PrimEvap2FlowValve) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:SECREG1MANIFOLD:OUT");
			if (PrimEvap2FlowValve.GetState() == 0) {
				vlv->Open();
			}
			else {
				vlv->Close();
			}
		}

		//Sec Evap Flow Valve
		else if (s == &SecEvapFlowValve) {
			h_Valve * vlv = (h_Valve*)Panelsdk.GetPointerByString("HYDRAULIC:SECREG1MANIFOLD:OUT2");
			if (SecEvapFlowValve.GetState() == 0) {
				vlv->Open();
			}
			else {
				vlv->Close();
			}
		}
}

void LEM::PanelThumbwheelSwitchChanged(ThumbwheelSwitch *s) {

}

void LEM::MousePanel_MFDButton(int mfd, int event, int mx, int my) {

	if (oapiGetMFDMode(mfd) != MFD_NONE) {
		if (my > 330 && my < 352) {
			if (event & PANEL_MOUSE_LBDOWN) {
				if (mx > 67 && mx < 96) {
					ButtonClick();
					oapiToggleMFD_on(mfd);
				}
				else if (mx > 295 && mx < 324) {
					ButtonClick();
					oapiSendMFDKey(mfd, OAPI_KEY_F1);
				}
				else if (mx > 329 && mx < 356) {
					ButtonClick();
					oapiSendMFDKey(mfd, OAPI_KEY_GRAVE);
				}
			}
		}
		else if (mx > 10 && mx < 38 && my > 53 && my < 294) {
			if ((my - 53) % 44 < 21) {
				int bt = (my - 53) / 44 + 0;
				if (event & PANEL_MOUSE_LBDOWN)
					ButtonClick();
				oapiProcessMFDButton(mfd, bt, event);
			}
		}
		else if (mx > 386 && mx < 416 && my > 53 && my < 294) {
			if ((my - 53) % 44 < 21) {
				int bt = (my - 53) / 44 + 6;
				if (event & PANEL_MOUSE_LBDOWN)
					ButtonClick();
				oapiProcessMFDButton(mfd, bt, event);
			}
		}
	}
	else if (event & PANEL_MOUSE_LBDOWN) {
		ButtonClick();
		oapiToggleMFD_on(mfd);
	}

}

void LEM::SwitchClick()

{
	Sclick.play();
}

void LEM::ButtonClick()

{
	Bclick.play(NOLOOP,255);
}

void LEM::GuardClick()

{
	Gclick.play(NOLOOP,255);
}

void LEM::VoxSound()

{
	Vox.play(LOOP,255);
}

void LEM::AbortFire()

{
	Afire.play(NOLOOP,255);
}

bool LEM::clbkPanelMouseEvent (int id, int event, int mx, int my)

{
	static int ctrl = 0;

	//
	// Special handling ORDEAL
	//

	if (id == AID_ORDEALSWITCHES && PanelId == LMPANEL_MAIN) {
		if (event & PANEL_MOUSE_LBDOWN) {
			if (ordealEnabled == 0) {
				ordealEnabled = 1;
				SwitchClick();
				return true;
			}
			else if (mx <= 50) {
				ordealEnabled = -1;
				SwitchClick();
				return true;
			}
		}
		if (ordealEnabled == 0)
			return false;
	}


	if (MainPanel.CheckMouseClick(id, event, mx, my))
		return true;


	switch (id) {
	// panel 0 events:
	/*case AID_DSKY_KEY:
		if (event & PANEL_MOUSE_LBDOWN) {
			dsky.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			dsky.ProcessKeyRelease(mx, my);
		}
		return true;*/

	/*case AID_LM_DEDA_KEYS:
		if (event & PANEL_MOUSE_LBDOWN) {
			deda.ProcessKeyPress(mx, my);
		} else if (event & PANEL_MOUSE_LBUP) {
			deda.ProcessKeyRelease(mx, my);
		}
		return true;*/

	case AID_MFDLEFT:
		MousePanel_MFDButton(MFD_LEFT, event, mx, my);
		return true;

	case AID_MFDRIGHT:
		MousePanel_MFDButton(MFD_RIGHT, event, mx, my);
		return true;

	case AID_LEM_COAS1:
		if (LEMCoas1Enabled)
			LEMCoas1Enabled = false;
		else
			LEMCoas1Enabled = true;
		SwitchClick();
		return true;

	case AID_LEM_COAS2:
		if (LEMCoas2Enabled)
			LEMCoas2Enabled = false;
		else
			LEMCoas2Enabled = true;
		SwitchClick();
		return true;

	case AID_LEM_FWD_HATCH:
		ForwardHatch.Toggle();

		return true;

	case AID_LEM_UPPER_HATCH:
		OverheadHatch.Toggle();

		return true;

	case AID_COAS:
		if (my >=0 && my <=149 ){
			if (mx > 0 && mx < 302 && !COASswitch){
				GuardClick();
				COASswitch=true;
			}
		}else if (my >=150 && my <=298 ){
			if (mx > 0 && mx < 302 && COASswitch){
				GuardClick();
				COASswitch=false;
			}

		}
		return true;

	case AID_AOT_RETICLE_KNOB:
		optics.ReticleMoved = 0;
		if (my >=0 && my <= 163 ){
			optics.ReticleMoved = 0.001 * pow(163 - my,1.25);
			optics.KnobTurning++;
		} else if (my >= 164 && my <= 326){
			optics.ReticleMoved = -0.001 * pow(my-164,1.25);
			optics.KnobTurning++;
		}
		if (optics.KnobTurning == 2) optics.KnobTurning = 0;

		if (event & PANEL_MOUSE_UP) optics.ReticleMoved = 0;

		return true;

	case AID_AOT_SHAFT_KNOB:
		if (my >= 90 && my <= 155) {
			optics.OpticsShaft++;
		}
		else if (my >= 0 && my <= 90) {
			optics.OpticsShaft--;
		}
		optics.OpticsShaft = (optics.OpticsShaft + 6) % 6;
		ButtonClick();
		//Load panel to trigger change of the default camera direction
		if (PanelId == LMPANEL_AOTZOOM)
		{
			oapiSetPanel(LMPANEL_AOTZOOM);
		}
		return true;

	case AID_LEM_MA_LEFT:
	case AID_LEM_MA_RIGHT:
		return CWEA.CheckMasterAlarmMouseClick(event);

		// panel 1 events:
	}
	return false;
}

bool LEM::clbkPanelRedrawEvent (int id, int event, SURFHANDLE surf) 

{
	//
	// Special handling ORDEAL
	//

	if (id == AID_ORDEALSWITCHES && PanelId == LMPANEL_MAIN) {
		if (ordealEnabled == -1) {
			ordealEnabled = 0;
			return true;
		}
		if (ordealEnabled == 0) {
			if (PanelOrdeal.IsFlashing()) {
				if (PanelFlashOn) {
					oapiBlt(surf, srf[SRF_BORDER_50x158], 0, 22, 0, 0, 50, 158, SURF_PREDEF_CK);
				}
				return true;
			}
			return false;
		}
		oapiBlt(surf, srf[SRF_ORDEAL_PANEL], 0, 0, 0, 0, 477, 202, SURF_PREDEF_CK);
		if (PanelOrdeal.IsFlashing()) {
			if (PanelFlashOn) {
				oapiBlt(surf, srf[SRF_BORDER_50x158], 0, 22, 0, 0, 50, 158, SURF_PREDEF_CK);
			}
		}
	}

	//
	// Process all the generic switches.
	//

	if (MainPanel.DrawRow(id, surf, PanelFlashOn))
		return true;

	//
	// Now special case the rest.
	//
	switch (id) {
	// panel 0 events:
	case AID_LM_CWS_LEFT:
		CWEA.RedrawLeft(surf,srf[SRF_CWSLIGHTS]);
		return true;

	case AID_LM_CWS_RIGHT:
		CWEA.RedrawRight(surf,srf[SRF_CWSLIGHTS]);
		return true;

	case AID_DSKY_LIGHTS:
		if (pMission->GetLMDSKYVersion() == 3)
		{
			dsky.RenderLights(surf, srf[SRF_DSKY], 0, 0, true, true);
		}
		else if (pMission->GetLMDSKYVersion() == 2)
		{
			dsky.RenderLights(surf, srf[SRF_DSKY]);
		}
		else
		{
			dsky.RenderLights(surf, srf[SRF_DSKY], 0, 0, false);
		}
		return true;

	case AID_DSKY_DISPLAY:
		dsky.RenderData(surf, srf[SRF_DIGITAL], srf[SRF_DSKYDISP]);
		return true;

	case AID_LM_DEDA_LIGHTS:
		deda.RenderOprErr(surf, srf[SRF_DEDA_LIGHTS]);
		return true;

	case AID_LM_DEDA_DISP:
		deda.RenderData(surf, srf[SRF_DIGITALDISP2]);
		return true;

	case AID_LM_DEDA_ADR:
		deda.RenderAdr(surf, srf[SRF_DIGITALDISP2]);
		return true;

	/*case AID_LM_DEDA_KEYS:
		deda.RenderKeys(surf, srf[SRF_DEDA_KEY]);
		return true;*/

	case AID_MFDLEFT:
		if (oapiGetMFDMode(MFD_LEFT) != MFD_NONE) {	
			oapiBlt(surf, srf[SRF_LMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton (surf, MFD_LEFT, 0, 24, 58);	
			RedrawPanel_MFDButton (surf, MFD_LEFT, 1, 402, 58);	
		}
		return true;

	case AID_MFDRIGHT:
		if (oapiGetMFDMode(MFD_RIGHT) != MFD_NONE) {	
			oapiBlt(surf, srf[SRF_LMMFDFRAME], 0, 0, 0, 0, 425, 354);

			RedrawPanel_MFDButton (surf, MFD_RIGHT, 0, 24, 58);	
			RedrawPanel_MFDButton (surf, MFD_RIGHT, 1, 402, 58);	
		}
		return true;

	case AID_LEM_COAS1:
		if (LEMCoas1Enabled) {
			oapiBlt(surf, srf[SRF_LEM_COAS1], 0, 0, 0, 0, 830, 830, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_LEM_COAS1], 0, 0, 0, 830, 830, 830, SURF_PREDEF_CK);
		}
		return true;

	case AID_LEM_COAS2:
		if (LEMCoas2Enabled) {
			oapiBlt(surf, srf[SRF_LEM_COAS2], 0, 0, 0, 0, 540, 540, SURF_PREDEF_CK);
		} else {
			oapiBlt(surf, srf[SRF_LEM_COAS2], 0, 0, 0, 540, 540, 540, SURF_PREDEF_CK);
		}
		return true;

	case AID_XPOINTERCDR:
		RedrawPanel_XPointer( &crossPointerLeft, surf);
		return true;

	case AID_XPOINTERLMP:
		RedrawPanel_XPointer(&crossPointerRight, surf);
		return true;

	case AID_CONTACTLIGHT1:
		if (SCS_ENG_CONT_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)){
			oapiBlt(surf,srf[SRF_CONTACTLIGHT],0,0,0,0,48,48, SURF_PREDEF_CK);//
		}
		return true;

	case AID_CONTACTLIGHT2:
		if (SCS_ATCA_CB.IsPowered() && (scca3.GetContactLightLogic() || LampToneTestRotary.GetState() == 6)) {
			oapiBlt(surf, srf[SRF_CONTACTLIGHT], 0, 0, 0, 0, 48, 48, SURF_PREDEF_CK);//
		}
		return true;

	case AID_RR_NOTRACK:
		if(lca.GetAnnunVoltage() > 2.25 && (RR.GetNoTrackSignal() || LampToneTestRotary.GetState() == 6)){ // The AC side is only needed for the transmitter
			oapiBlt(surf,srf[SRF_RR_NOTRACK],0,0,0,34,34,34, SURF_PREDEF_CK); // Light On
		}else{
			oapiBlt(surf,srf[SRF_RR_NOTRACK],0,0,0,0,34,34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_CO2_LIGHT:
		if (lca.GetAnnunVoltage() > 2.25) {
			if (INST_CWEA_CB.IsPowered() && ECS_CO2_SENSOR_CB.IsPowered() && (scera1.GetVoltage(5, 2) >= (7.6 / 6))) {
				oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 34, 34, 34, SURF_PREDEF_CK); // Light On
			}
			else if (CO2CanisterSelectSwitch.GetState() == 0 || LampToneTestRotary.GetState() == 6) {
				oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 34, 34, 34, SURF_PREDEF_CK); // Light On
			}
		}
		else {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 0, 34, 34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_SUITFAN_LIGHT:
		if (lca.GetAnnunVoltage() > 2.25 && (SuitFanDPSensor.GetSuitFanFail() == true || LampToneTestRotary.GetState() == 6)) {
				oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 34, 34, 34, SURF_PREDEF_CK); // Light On
		}
		else {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 0, 34, 34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_H2OSEP_LIGHT:
		if (lca.GetAnnunVoltage() > 2.25 && INST_CWEA_CB.IsPowered() && (scera1.GetVoltage(5, 3) < (792.5 / 720.0) || LampToneTestRotary.GetState() == 6)) {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 34, 34, 34, SURF_PREDEF_CK); // Light On
		}
		else {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 0, 34, 34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_GLYCOL_LIGHT:
		if (lca.GetAnnunVoltage() > 2.25 && (scera2.GetSwitch(12, 2)->IsClosed() || PrimGlycolPumpController.GetPressureSwitch() == true || LampToneTestRotary.GetState() == 6)) {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 34, 34, 34, SURF_PREDEF_CK); // Light On
		}
		else {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 0, 34, 34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_DC_BUS_LIGHT:
		if (lca.GetCompDockVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {								//Needs control logic to indicate DC bus or main feeder grounded
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 34, 34, 34, SURF_PREDEF_CK); // Light On
		}
		else {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 0, 34, 34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_BAT_FAULT_LIGHT:
		if (lca.GetAnnunVoltage() > 2.25 && (LampToneTestRotary.GetState() == 6)) {								//Needs control logic to indicate overcurrent, reverse current, or over temp of selected battery
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 34, 34, 34, SURF_PREDEF_CK); // Light On
		}
		else {
			oapiBlt(surf, srf[SRF_RR_NOTRACK], 0, 0, 0, 0, 34, 34, SURF_PREDEF_CK); // Light Off
		}
		return true;

	case AID_SEQ_LIGHT1:
		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 11) > 2.5 && stage < 2 || LampToneTestRotary.GetState() == 6)) {
			oapiBlt(surf, srf[SRF_SEQ_LIGHT], 0, 0, 0, 0, 33, 30);
		}
		else {
			oapiBlt(surf, srf[SRF_SEQ_LIGHT], 0, 0, 33, 0, 33, 30);
		}
		return true;

	case AID_SEQ_LIGHT2:
		if (lca.GetCompDockVoltage() > 2.25 && (scera1.GetVoltage(12, 12) > 2.5 || LampToneTestRotary.GetState() == 6)) {
			oapiBlt(surf, srf[SRF_SEQ_LIGHT], 0, 0, 0, 0, 33, 30);
		}
		else {
			oapiBlt(surf, srf[SRF_SEQ_LIGHT], 0, 0, 33, 0, 33, 30);
		}
		return true;

	case AID_FDAI:
		RedrawPanel_Horizon (surf);
		return true;

	case AID_FDAI_LEFT:
		if (!fdaiDisabled) {
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (AttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

			if (RateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (AttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI1PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			if (errors.x > 41) { errors.x = 41; }
			else { if (errors.x < -41) { errors.x = -41; } }
			if (errors.y > 41) { errors.y = 41; }
			else { if (errors.y < -41) { errors.y = -41; } }
			if (errors.z > 41) { errors.z = 41; }
			else { if (errors.z < -41) { errors.z = -41; } }
			fdaiLeft.PaintMe(attitude, no_att, rates, errors, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);
		}
		return true;

	case AID_FDAI_RIGHT:
		if (!fdaiDisabled){
			VECTOR3 attitude;
			VECTOR3 errors;
			VECTOR3 rates;
			int no_att = 0;

			if (RightAttitudeMonSwitch.IsUp())	//PGNS
			{
				attitude = gasta.GetTotalAttitude();
			}
			else							//AGS
			{
				attitude = aea.GetTotalAttitude();
			}

			if (RightRateErrorMonSwitch.GetState() == 1)
			{
				if (RR.IsPowered()) {
					if (ShiftTruSwitch.IsUp())
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (50 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (50 * RAD);
						errors.x = 0.0;
					}
					else
					{
						errors.z = RR.GetRadarTrunnionPos() * 41 / (5 * RAD);
						errors.y = RR.GetRadarShaftPos() * 41 / (5 * RAD);
						errors.x = 0.0;
					}
				}
				else
				{
					errors = _V(0, 0, 0);
				}
			}
			else
			{
				if (RightAttitudeMonSwitch.IsUp())	//PGNS
				{
					errors = _V(atca.lgc_err_x, atca.lgc_err_y, atca.lgc_err_z);
				}
				else							//AGS
				{
					VECTOR3 aeaerror = aea.GetAttitudeError();
					errors = _V(aeaerror.x, -aeaerror.y, -aeaerror.z)*DEG*41.0 / 15.0;

					if (DeadBandSwitch.IsUp())
					{
						errors *= 15.0 / 14.4;
					}
					else
					{
						errors *= 15.0 / 1.7;
					}
				}
			}

			// ORDEAL
			if (!no_att) {
				attitude.y += ordeal.GetFDAI2PitchAngle();
				if (attitude.y >= TWO_PI) attitude.y -= TWO_PI;
			}

			if (RateScaleSwitch.IsUp())
			{
				rates = rga.GetRates() / (25.0*RAD);
			}
			else
			{
				rates = rga.GetRates() / (5.0*RAD);
			}

			// ERRORS IN PIXELS -- ENFORCE LIMITS HERE
			if (errors.x > 41) { errors.x = 41; }
			else { if (errors.x < -41) { errors.x = -41; } }
			if (errors.y > 41) { errors.y = 41; }
			else { if (errors.y < -41) { errors.y = -41; } }
			if (errors.z > 41) { errors.z = 41; }
			else { if (errors.z < -41) { errors.z = -41; } }
			fdaiRight.PaintMe(attitude, no_att, rates, errors, surf, srf[SRF_FDAI], srf[SRF_FDAIROLL], srf[SRF_FDAIOFFFLAG], srf[SRF_FDAINEEDLES], hBmpFDAIRollIndicator, fdaiSmooth);
		}
		return true;

	case AID_MISSION_CLOCK:
		MissionTimerDisplay.Render(surf, srf[SRF_DIGITALDISP2]);
		return true;

	case AID_EVENT_TIMER:
		EventTimerDisplay.Render(surf, srf[SRF_DIGITALDISP2]);
		return true;

	case AID_ENG_THRUST:
		RedrawPanel_Thrust(surf);
		return true;

	case AID_ENG_FUEL_TEMP:
		double DispValue;

		if(ph_Dsc){
			DispValue = GetPropellantMass(ph_Dsc)/8165;
		}else{
				DispValue =0;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(47-(DispValue)*47),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_Asc){
		DispValue = GetPropellantMass(ph_Asc)/GetPropellantMaxMass(ph_Asc);
		}else{
			DispValue =1;
		}

		oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(47-(DispValue)*47),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_ENG_FUEL_PRESS:
		if(ph_Dsc){
			DispValue = GetPropellantMass(ph_Dsc)/8165;
		}else{
				DispValue =0;
		}
		oapiBlt(surf,srf[SRF_NEEDLE],1,(int)(62-(DispValue)*62),0,0,7,7, SURF_PREDEF_CK);//
		if(ph_Asc){
		DispValue = GetPropellantMass(ph_Asc)/GetPropellantMaxMass(ph_Asc);
		}else{
			DispValue =1;
		}

		oapiBlt(surf,srf[SRF_NEEDLE],29,(int)(62-(DispValue)*62),8,0,7,7, SURF_PREDEF_CK);//
		return true;

	case AID_AOT_RETICLE:
		RedrawPanel_AOTReticle(surf);
		return true;

	case AID_AOT_RETICLE_KNOB:
		oapiBlt(surf,srf[SRF_AOTRETICLEKNOB],0,0,optics.KnobTurning*74,0,74,326);
		return true;
	
	case AID_AOT_SHAFT_KNOB:
		oapiBlt(surf,srf[SRF_AOTSHAFTKNOB],0,0,optics.OpticsShaft*62,0,62,155);
		return true;

	case AID_AOT_RETICLEDISPLAY:
		optics.PaintReticleAngle(surf, srf[SRF_AOT_FONT]);
		return true;

	case AID_COAS:
		if (COASswitch) {
			oapiBlt(surf, srf[0], 0, 0, 146, 0, 301, 298);
		}
		else {
			oapiBlt(surf, srf[0], 0, 0, 448, 0, 301, 298);
		}
		return true;

	case AID_RANGE_TAPE:
		RadarTape.RenderRange(surf);
		return true;
	case AID_RATE_TAPE:
		RadarTape.RenderRate(surf);
		return true;

	case AID_LEM_MA_LEFT:
	case AID_LEM_MA_RIGHT:
		CWEA.RenderMasterAlarm(surf, srf[SRF_LEM_MASTERALARM], NULL);
		return true;

	// Power Failure Lights
	case AID_LEM_PWRFAIL_THRUST:
		if (!pfira.GetThrustIndRelay() && lca.GetAnnunVoltage() > 2.25 ) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_DPSPRESS:
		if (!pfira.GetPropPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_RCSPRESS:
		if (!pfira.GetRCSPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_ECSPRESS:
		if (!pfira.GetSuitCabinPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_RCSQUAN:
		if (!pfira.GetRCSQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_ECSQUAN:
		if (!pfira.GetO2H2OQtyIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_GLYCOL:
		if (!pfira.GetGlyTempPressIndRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_XPTRCDR:
		if (!pfira.GetCDRXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;

	case AID_LEM_PWRFAIL_XPTRLMP:
		if (!pfira.GetLMPXPointerRelay() && lca.GetAnnunVoltage() > 2.25) {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 0, 0, 13, 12);
		}
		else {
			oapiBlt(surf, srf[SRF_PWRFAIL_LIGHT], 0, 0, 13, 0, 13, 12);
		}
		return true;
	}
	return false;
}


void LEM::PanelRefreshForwardHatch() {

	if (InPanel && PanelId == LMPANEL_FWDHATCH) {
		if (oapiCameraInternal()) {
			oapiSetPanel(LMPANEL_FWDHATCH);
		}
		else {
			RefreshPanelIdInTimestep = true;
		}
	}
}

void LEM::PanelRefreshOverheadHatch() {

	if (InPanel && PanelId == LMPANEL_UPPERHATCH) {
		if (oapiCameraInternal()) {
			oapiSetPanel(LMPANEL_UPPERHATCH);
		}
		else {
			RefreshPanelIdInTimestep = true;
		}
	}
}

//
// The switch functions just pack the different switch areas from the control panel
// into 32-bit integers and unpack them from those integers. This provides a much more
// compact means of storing the switch states in the scenario file than saving them as
// individual values for each switch.
//

typedef union {
	struct {
		unsigned Cswitch1:1;
		unsigned Cswitch2:1;
		unsigned Cswitch4:1;
		unsigned Cswitch5:1;
		unsigned Cswitch6:1;
		unsigned Cswitch7:1;
		unsigned Cswitch8:1;
		unsigned Cswitch9:1;
		unsigned DESHE1switch:1;
		unsigned DESHE2switch:1;
		unsigned ASCHE1switch:1;
		unsigned ASCHE2switch:1;
		unsigned CRSFDswitch:1;
		unsigned CABFswitch:1;
		unsigned PTTswitch:1;
		unsigned RCSS1switch:1;
		unsigned RCSS2switch:1;
		unsigned RCSS3switch:1;
		unsigned RCSS4switch:1;
		unsigned P41switch:1;
		unsigned P42switch:1;
		unsigned P43switch:1;
		unsigned P44switch:1;
		unsigned AUDswitch:1;
		unsigned RELswitch:1;
		unsigned CPswitch:1;
		unsigned HATCHswitch:1;
		unsigned EVAswitch:1;
		unsigned COASswitch:1;

	} u;
	unsigned long word;
} CSwitchState;

int LEM::GetCSwitchState()

{
	CSwitchState state;

	state.word = 0;
	state.u.COASswitch = COASswitch;

	return state.word;
}

void LEM::SetCSwitchState(int s)

{
	CSwitchState state;

	state.word = s;
	COASswitch = state.u.COASswitch;
}