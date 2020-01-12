/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005

  ORBITER vessel module: Lunar Rover

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

#define ORBITER_MODULE

// To force orbitersdk.h to use <fstream> in any compiler version
#pragma include_alias( <fstream.h>, <fstream> )
#include "orbitersdk.h"
#include "stdio.h"
#include "math.h"

#include "nasspsound.h"
#include "nasspdefs.h"
#include "soundlib.h"
#include "OrbiterMath.h"

#include "toggleswitch.h"
#include "LEM.h"

#include "lrv.h"
#include "lrv_console.h"
#include "tracer.h"

//
// Set the file name for the tracer code.
//

char trace_file[] = "ProjectApollo LRV.log";

const VECTOR3 OFS_STAGE1 =  { 0, 0, -8.935};
const VECTOR3 OFS_STAGE2 =  { 0, 0, 9.25-12.25};
const VECTOR3 OFS_STAGE21 =  { 1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE22 =  { -1.85,1.85,24.5-12.25};
const VECTOR3 OFS_STAGE23 =  { 1.85,-1.85,24.5-12.25};
const VECTOR3 OFS_STAGE24 =  { -1.85,-1.85,24.5-12.25};

//
// Astronaut and rover movement capabilities
//

#define ROVER_SPEED_M_S 3.6  // max rover speed in m/s (http://en.wikipedia.org/wiki/Lunar_rover; values on the web range from 7 mph to 10.56 mph)
#define ROVER_ACC_M_S2 0.5   // rover acceleration in m/s^2 (roughly based on AP15 mission report: accelleration to 10 km/h in "3 vehicle lengths")
#define ROVER_BRK_M_S2 2.0   // rover braking in m/s^2 (not based on any real data (yet))

//
// Steering geometry
// Sources:
//   (ST1) "Lunar Rover Operations Handbook"; http://www.hq.nasa.gov/alsj/LRV_OpsNAS8-25145.pdf
#define WHEEL_RADIUS_M (16.0 * 0.0254) // (real) wheel radius in [m]; may need adjusting to mesh dimensions
#define WHEEL_SPIN_FUDGE_FACTOR 0.82   // adjust calculated wheel spin (should not be necessary, but is ...)
#define MAX_WHEEL_TURN_OUTER_DEG 22.0  // max turning of outer wheel in [degrees] (ST1)
#define MAX_WHEEL_TURN_INNER_DEG 50.0  // max turning of inner wheel in [degrees] (ST1)
#define WHEEL_BASE_CM (2.54 * 90.0)    // wheelbase (longitudinal) in [cm] (ST1)
#define WHEEL_TRACK_CM (2.54 * 72.0)   // wheel track (width) in [cm] (ST1)
#define WHEEL_LOCK2LOCK_S 5.5          // lock-to-lock time [s] (ST1); lock-to-lock time is given as 5.5s (+/- 0,5s)

//
// Variables that really should be global variables.
//

static 	int refcount = 0;
static MESHHANDLE hLRV;
static MESHHANDLE hLRVConsole;

// some static data four the eight needles of the power/temp gauges
static const double VCC_NEEDLE_MINPOS[8] = {-25.0, -25.0, -24.3, -24.3, -24.1, -24.1, -23.2, -23.2};  // angle of minimum value on scale
static const double VCC_NEEDLE_MAXPOS[8] = {26.5, 26.5, 24.3, 24.3, 25.5, 25.5, 25.3, 25.3};  // angle of maximum value on scale
static const int VCC_NEEDLE_GROUPS[8] = {  // GEOM groups of the eight needles
			GEOM_NEEDLE_A, GEOM_NEEDLE_B, GEOM_NEEDLE_C, GEOM_NEEDLE_D,
			GEOM_NEEDLE_E, GEOM_NEEDLE_F, GEOM_NEEDLE_G, GEOM_NEEDLE_H
		};

// Calculate the correct Ackermann steering angle for outer wheel.
// Arguments:
//  inner_angle -- steering angle of inner wheel (in radians), measured from direction of travel
double AckermannInnerToOuter(double inner_angle_rad)
{
	if (fabs(inner_angle_rad) < 0.0001) return 0.0;

	if (inner_angle_rad > 0.0)
	{
		return (PI05) - atan((WHEEL_TRACK_CM / (0.5 * WHEEL_BASE_CM)) + tan(PI05 - inner_angle_rad));
	} else {
		return - ((PI05) - atan((WHEEL_TRACK_CM / (0.5 * WHEEL_BASE_CM)) + tan(PI05 + inner_angle_rad)));
	}
}

// Calculate the turn radius for the center of the LRV based on the
// inner wheel deflection.
// ATTENTION: not defined for (inner_wheel_deflection_rad = 0.0)!
double get_turn_radius_for_center(double inner_wheel_deflection_rad)
{
	return (0.5 * WHEEL_TRACK_CM) + (0.5 * WHEEL_BASE_CM) * tan(PI05 - inner_wheel_deflection_rad);
}

LRV::LRV(OBJHANDLE hObj, int fmodel) : VESSEL2(hObj, fmodel)

{
	TRACESETUP("baseLRVfunctioncall");
	hMaster = hObj;
	init();
	DefineAnimations();
}

LRV::~LRV()

{
	// delete wheel;  Delete all animation subsets...
	delete frwheel;
	delete flwheel;
	delete rrwheel;
	delete rlwheel;
	delete frtire;
	delete fltire;
	delete rrtire;
	delete rltire;
}

void LRV::init()

{
	TRACESETUP("init");

	//GoDock1=false;
	starthover=false;
	MotherShip=false;
	EVAName[0]=0;
	CSMName[0]=0;
	MSName[0]=0;
	KEY1 = false;
	KEY2 = false;
	KEY3 = false;
	KEY4 = false;
	KEY5 = false;
	KEY6 = false;
	KEY7 = false;
	KEY8 = false;
	KEY9 = false;
	KEYADD = false;
	KEYSUBTRACT = false;
	KEYCTRL = false;

	FirstTimestep = true;
	SLEVAPlayed = false;
	StateSet = false;

	lastLatLongSet = false;
	lastLat = 0;
	lastLong = 0;

	speed = 0.0;
	speedlock = false;

	ApolloNo = 0;

	// power and temperature (currently simplistic and/or faked)
	Bat1Cap = 120;  // remaining capacity of battery 1 [Ah]
	Bat2Cap = 121;  // remaining capacity of battery 2 [Ah]
	Bat1Volt = 36; // voltage of battery 1 [V]
	Bat2Volt = 37; // voltage of battery 2 [V]
	Bat1Temp = 80; // temperature of battery 1 [°F]
	Bat2Temp = 78; // temperature of battery 2 [°F]

	// LRV Console
	vccVis = NULL;
	vccCompAngle = 0.0; // North
	vccBear001Angle = 0.0;
	vccBear010Angle = 0.0;
	vccBear100Angle = 0.0;
	vccDist001Angle = 0.0;
	vccDist010Angle = 0.0;
	vccDist100Angle = 0.0;
	vccRange001Angle = 0.0;
	vccRange010Angle = 0.0;
	vccRange100Angle = 0.0;
	vccSpeedAngle = 0.0;
	for (int i=0; i<8; i++)
		vccNeedleAngle[i] = - Radians(VCC_NEEDLE_MINPOS[i]);
	vccInitialized = false;
	vccInitLat = 0.0;
	vccInitLong = 0.0;
	vccDistance = 0.0;

	// touchdown point test
	// touchdownPointHeight = -0.8;

    proc_tires_left = 0.0;
    proc_tires_right = 0.0;
	proc_frontwheel_inner = 0.0;
	proc_frontwheel_outer = 0.0;
	proc_rearwheel_inner = 0.0;
	proc_rearwheel_outer = 0.0;
	steering = 0.0;
	autocenter = false;
	
	LRVMeshIndex = 1;
}

void LRV::clbkSetClassCaps (FILEHANDLE cfg)
{
	TRACESETUP("clbkSetClassCaps");
	SetRoverStage();
}

void LRV::DefineAnimations ()
{
	TRACESETUP("DefineAnimations");

	static UINT fntrgtfendergrp[1] = {16}; //front right fender groups
	static UINT fntlftfendergrp[1] = {20}; //front left fender groups
	static UINT rearrgtfendergrp[1] = {19}; //rear right fender groups
	static UINT rearlftfendergrp[1] = {21}; //rear left fender groups

	static MGROUP_ROTATE fntrgtfender (LRVMeshIndex, fntrgtfendergrp, 1, _V(0.46,-0.502,0.916), _V(0,0,1), (float)(PI/6));
	static MGROUP_ROTATE fntlftfender (LRVMeshIndex, fntlftfendergrp, 1, _V(-0.46,-0.502,0.916), _V(0,0,1), (float)(PI/6));
	static MGROUP_ROTATE rearrgtfender (LRVMeshIndex, rearrgtfendergrp, 1, _V(0.46,-0.502,-1.385), _V(0,0,1), (float)(PI/6));
	static MGROUP_ROTATE rearlftfender (LRVMeshIndex, rearlftfendergrp, 1, _V(-0.46,-0.502,-1.385), _V(0,0,1), (float)(PI/6));

	static UINT fntrgtwheelgrp[4] = {0,27,33,51}; //front right wheel groups
	static UINT fntlftwheelgrp[4] = {1,26,34,50}; //front left wheel groups
	static UINT rearrgtwheelgrp[2] = {6,52}; //rear right wheel groups
	static UINT rearlftwheelgrp[2] = {2,53}; //rear left wheel groups

	frwheel = new MGROUP_ROTATE (LRVMeshIndex, fntrgtwheelgrp, 4, _V(0.842,-0.621,0.916), _V(0,1,0), (float)(PI * 100/180));
	flwheel = new MGROUP_ROTATE (LRVMeshIndex, fntlftwheelgrp, 4, _V(-0.837,-0.621,0.916), _V(0,1,0), (float)(PI * 100/180));
	rrwheel = new MGROUP_ROTATE (LRVMeshIndex, rearrgtwheelgrp, 2, _V(0.842,-0.623,-1.389), _V(0,1,0), (float)(PI * 100/180));
	rlwheel = new MGROUP_ROTATE (LRVMeshIndex, rearlftwheelgrp, 2, _V(-0.837,-0.623,-1.389), _V(0,1,0), (float)(PI * 100/180));

	static UINT fntrgttiregrp[4] = {35,40,62,67}; //front right tire groups
	static UINT fntlfttiregrp[4] = {37,41,61,66}; //front left tire groups
	static UINT rearrgttiregrp[4] = {36,39,63,68}; //rear right tire groups
	static UINT rearlfttiregrp[4] = {38,42,64,65}; //rear left tire groups

	frtire = new MGROUP_ROTATE (LRVMeshIndex, fntrgttiregrp, 4, _V(-0.976,-0.621,0.916), _V(1,0,0), (float)(2*PI));
	fltire = new MGROUP_ROTATE (LRVMeshIndex, fntlfttiregrp, 4, _V(0.959,-0.621,0.916), _V(1,0,0), (float)(2*PI));
	rrtire = new MGROUP_ROTATE (LRVMeshIndex, rearrgttiregrp, 4, _V(-0.976,-0.623,-1.389), _V(1,0,0), (float)(2*PI));
	rltire = new MGROUP_ROTATE (LRVMeshIndex, rearlfttiregrp, 4, _V(0.959,-0.623,-1.389), _V(1,0,0), (float)(2*PI));


	//SET UP ANIMATIONS

	anim_fntrgtfender = CreateAnimation (0.5);
	fr_fender = AddAnimationComponent (anim_fntrgtfender, 0, 1, &fntrgtfender);
	anim_fntrgtwheel = CreateAnimation (0.5);
	fr_wheel = AddAnimationComponent (anim_fntrgtwheel, 0, 1, frwheel, fr_fender);
	anim_fntrgttire = CreateAnimation (0.0);
	AddAnimationComponent (anim_fntrgttire, 0, 1, frtire, fr_wheel);

	anim_fntlftfender = CreateAnimation (0.5);
	fl_fender = AddAnimationComponent (anim_fntlftfender, 0, 1, &fntlftfender);
	anim_fntlftwheel = CreateAnimation (0.5);
	fl_wheel = AddAnimationComponent (anim_fntlftwheel, 0, 1, flwheel, fl_fender);
	anim_fntlfttire = CreateAnimation (0.0);
	AddAnimationComponent (anim_fntlfttire, 0, 1, fltire, fl_wheel);

	anim_rearrgtfender = CreateAnimation (0.5);
	rr_fender = AddAnimationComponent (anim_rearrgtfender, 0, 1, &rearrgtfender);
	anim_rearrgtwheel = CreateAnimation (0.5);
	rr_wheel = AddAnimationComponent (anim_rearrgtwheel, 0, 1, rrwheel, rr_fender);
	anim_rearrgttire = CreateAnimation (0.0);
	AddAnimationComponent (anim_rearrgttire, 0, 1, rrtire, rr_wheel);

	anim_rearlftfender = CreateAnimation (0.5);
	rl_fender = AddAnimationComponent (anim_rearlftfender, 0, 1, &rearlftfender);
	anim_rearlftwheel = CreateAnimation (0.5);
	rl_wheel = AddAnimationComponent (anim_rearlftwheel, 0, 1, rlwheel, rl_fender);
	anim_rearlfttire = CreateAnimation (0.0);
	AddAnimationComponent (anim_rearlfttire, 0, 1, rltire, rl_wheel);

}

void LRV::SetRoverStage ()
{
	SetEmptyMass(250);
	SetSize(10);
	SetPMI(_V(15,15,15));

	SetSurfaceFrictionCoeff(0.005, 0.5);
	SetRotDrag(_V(0, 0, 0));
	SetCW(0, 0, 0, 0);
	SetPitchMomentScale(0);
	SetYawMomentScale(0);
	SetLiftCoeffFunc(0); 

    ClearMeshes();
    ClearExhaustRefs();
    ClearAttExhaustRefs();
	VECTOR3 mesh_adjust = _V(0.0, 0.15, 0.0);
	vccMeshIdx = AddMesh(hLRVConsole, &mesh_adjust);
	AddMesh(hLRV, &mesh_adjust);
	SetMeshVisibilityMode(LRVMeshIndex, MESHVIS_ALWAYS); 
	SetMeshVisibilityMode(vccMeshIdx, MESHVIS_ALWAYS);
	SetCameraOffset(_V(0.36, 0.54, -0.55));  // roughly at the driver's head

	//////////////////////////////////////////////////////////////////////////
	// With vccMeshIdx, we now have all data to initialize the LRV console
	// transformation matrices:
	//////////////////////////////////////////////////////////////////////////
    // Compass rose rotation
	mgtRotCompass.P.rotparam.ref = LRV_COMPASS_PIVOT;
	mgtRotCompass.P.rotparam.axis = Normalize(LRV_COMPASS_AXIS);
	mgtRotCompass.P.rotparam.angle = 0.0;  // dummy value
	mgtRotCompass.nmesh = vccMeshIdx;
	mgtRotCompass.ngrp = GEOM_COMPASS;
	mgtRotCompass.transform = MESHGROUP_TRANSFORM::ROTATE;
    // Bearing, distance or range drum (shared for all drums)
	mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_UPPER;  // dummy value
	mgtRotDrums.P.rotparam.axis = LRV_DRUM_AXIS;
	mgtRotDrums.P.rotparam.angle = 0.0;  // dummy value
	mgtRotDrums.nmesh = vccMeshIdx;
	mgtRotDrums.ngrp = GEOM_DRUM_BEAR_001;  // dummy value
	mgtRotDrums.transform = MESHGROUP_TRANSFORM::ROTATE;
    // Speed dial rotation
	mgtRotSpeed.P.rotparam.ref = LRV_SPEED_PIVOT;
	mgtRotSpeed.P.rotparam.axis = Normalize(LRV_SPEED_AXIS);
	mgtRotSpeed.P.rotparam.angle = 0.0;  // dummy value
	mgtRotSpeed.nmesh = vccMeshIdx;
	mgtRotSpeed.ngrp = GEOM_SPEEDDIAL;
	mgtRotSpeed.transform = MESHGROUP_TRANSFORM::ROTATE;
    // Power/temp gauge needles
	mgtRotGauges.P.rotparam.ref = LRV_GAUGE_PIVOT;
	mgtRotGauges.P.rotparam.axis = Normalize(LRV_DRUM_AXIS);  // same axis as drums
	mgtRotGauges.P.rotparam.angle = 0.0;  // dummy value
	mgtRotGauges.nmesh = vccMeshIdx;
	mgtRotGauges.ngrp = GEOM_NEEDLE_A;  // dummy value
	mgtRotGauges.transform = MESHGROUP_TRANSFORM::ROTATE;

	double tdph = -0.9;
	SetTouchdownPoints(_V(0, tdph, 1), _V(-1, tdph, -1), _V(1, tdph, -1));

}

void LRV::ScanMotherShip()

{
	double VessCount;
	int i=0;

	VessCount=oapiGetVesselCount();
	for ( i = 0 ; i< VessCount ; i++ ) 
	{
		hMaster=oapiGetVesselByIndex(i);
		strcpy(EVAName,GetName());
		oapiGetObjectName(hMaster,CSMName,256);
		strcpy(MSName,CSMName);
		strcat(CSMName,"-LRV");
		if (strcmp(CSMName, EVAName)==0){
			MotherShip=true;
			i=int(VessCount);
		}
		else{
			strcpy(CSMName,"");
		}
	}
}

void LRV::MoveLRV(double SimDT, VESSELSTATUS *eva, double heading)
{
	//TRACESETUP("MoveLRV");

	double lat;
	double lon;
	double move_spd;
	double ctrl_factor;

	// limit time acceleration
	double timeW = oapiGetTimeAcceleration();
	if (timeW > 100 && speed != 0.0)
		oapiSetTimeAcceleration(100);

	if (eva->status == 1) {
		lon = eva->vdata[0].x;
		lat = eva->vdata[0].y;
	
	} else if (lastLatLongSet) {
		lon = lastLong;
		lat = lastLat;
		eva->vdata[0].z = heading;
	
	} else return;

	// if CTRL is pressed, slow steering and accel/decel by a factor of one tenth
	if (KEYCTRL)
		ctrl_factor = 0.1;
	else
		ctrl_factor = 1.0;

    //modify steering, which varies from -1 (full left) to 1 (full right)
	if (KEY1)  // turn left
	{
		autocenter = false;
		steering = max(-1.0, steering - ctrl_factor * 2.0 * SimDT / WHEEL_LOCK2LOCK_S);
	}
	else if (KEY3)  // turn right
	{
		autocenter = false;
		steering = min(1.0, steering + ctrl_factor * 2.0 * SimDT / WHEEL_LOCK2LOCK_S);
	}
	else if (KEY5 | KEY2)  // center wheels
	{
		autocenter = true;
	}

	if (autocenter)
	{
		if (steering > 0.00001)
		  steering = max(0.0, steering - 2.0 * SimDT / WHEEL_LOCK2LOCK_S);
		else if (steering < -0.00001)
		  steering = min(0.0, steering + 2.0 * SimDT / WHEEL_LOCK2LOCK_S);
		else
		{
		  steering = 0.0;
		  autocenter = false;
		}
	}

	if (speed != 0.0) //LRV
	{
		//
		// calculate turn radius for center of LRV based on inner wheel deflection 
		//
		double turn_angle;
        double inner_angle_rad = Radians(fabs(steering) * MAX_WHEEL_TURN_INNER_DEG);
		// how far along the full circle does the LRV move (angle, not distance)?
		if (inner_angle_rad <= PI/1000.0) // avoid singularity at 0.0 ...
			turn_angle = 0.0;
		else
			turn_angle = 100.0 * speed * SimDT / get_turn_radius_for_center(inner_angle_rad);
		
		//modify vdata to simulate turning motion
		if (steering >= 0.0)
		  eva->vdata[0].z = eva->vdata[0].z + turn_angle;
		else
		  eva->vdata[0].z = eva->vdata[0].z - turn_angle;
		if(eva->vdata[0].z <=-2*PI)
			eva->vdata[0].z = eva->vdata[0].z + 2*PI;
		else if (eva->vdata[0].z >=2*PI)
			eva->vdata[0].z = eva->vdata[0].z - 2*PI;

	}

	if (KEYSUBTRACT)  // decelerate
	{
		if (speed > 0.0) {
			// we are still braking from forward movement to standstill
			speed = __max(0.0, speed - ctrl_factor * SimDT * ROVER_BRK_M_S2); 
			speedlock = (speed == 0.0);  // if wheelstop: don't go into reverse until key is released
		}
		else if (!speedlock)
		{
			speed = __max(-ROVER_SPEED_M_S, speed - ctrl_factor * SimDT * ROVER_ACC_M_S2); 
		}
	}
	else if (KEYADD)  // accelerate
	{
		if (speed < 0.0) {
			// we are still braking from reverse movement to standstill
			speed = __min(0.0, speed + ctrl_factor * SimDT * ROVER_BRK_M_S2); 
			speedlock = (speed == 0.0);  // if wheelstop: don't accellerate until key is released
		}
		else if (!speedlock)
		{
			speed = __min(ROVER_SPEED_M_S, speed + ctrl_factor * SimDT * ROVER_ACC_M_S2); 
		}
	}
	else
	{
		// no speed key pressed: release lock
		speedlock = false;
	}

	// move forward / backward according to current speed
	move_spd =  SimDT * atan2(speed, oapiGetSize(eva->rbody));  // surface speed in radians
	lat = lat + cos(0.5 * (heading + eva->vdata[0].z)) * move_spd;  // direction of travel is (old_heading + new_heading)/2
	lon = lon + sin(0.5 * (heading + eva->vdata[0].z)) * move_spd;
	vccDistance = vccDistance + SimDT * fabs(speed);  // the console distance always counts up!
	// decrease battery power (by about 1.87 Ah/km [http://www.hq.nasa.gov/alsj/a15/a15mr-8.htm])
	Bat1Cap = Bat1Cap - 0.5 * 1.87 * (speed * SimDT / 1000.0);
	Bat2Cap = Bat2Cap - 0.5 * 1.87 * (speed * SimDT / 1000.0);

	// reset all keys
	KEY1 = false;
	KEY2 = false;
	KEY3 = false;
	KEY4 = false;
	KEY5 = false;
	KEY6 = false;
	KEY7 = false;
	KEY8 = false;
	KEY9 = false;
	KEYADD = false;
	KEYSUBTRACT = false;

	// move the vessel
	eva->vdata[0].x = lon;
	eva->vdata[0].y = lat;
	eva->status = 1;
	DefSetState(eva);

	// remember last coordinates
	lastLat = lat;
	lastLong = lon;
	lastLatLongSet = true;
}

// ==============================================================
// API interface
// ==============================================================

int LRV::clbkConsumeDirectKey(char *kstate) {

	TRACESETUP("clbkConsumeDirectKey");

	if (FirstTimestep) return 0;

	if (KEYMOD_SHIFT(kstate)) {
		return 0; 
	}

	KEYCTRL = KEYMOD_CONTROL(kstate);

	if (KEYDOWN(kstate, OAPI_KEY_NUMPAD1)) {
		KEY1 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD1);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD2)) {
		KEY2 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD2);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD3)) {
		KEY3 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD3);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD4)) {			
		KEY4 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD4);			
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD5)) {			
		KEY5 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD5);			
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD6)) {			
		KEY6 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD6);
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD7)) {			
		KEY7 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD7);			
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD8)) {
		KEY8 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD8);						
	}

	if (KEYDOWN (kstate, OAPI_KEY_NUMPAD9)) {
		KEY9 = true;
		RESETKEY(kstate, OAPI_KEY_NUMPAD9);
	}

	if (KEYDOWN (kstate, OAPI_KEY_ADD)) {
		KEYADD = true;
		RESETKEY(kstate, OAPI_KEY_ADD);
	}

	if (KEYDOWN (kstate, OAPI_KEY_SUBTRACT)) {
		KEYSUBTRACT = true;
		RESETKEY(kstate, OAPI_KEY_SUBTRACT);
	}


	// touchdown point test
	/* 
	if (KEYDOWN (kstate, OAPI_KEY_A)) {
		touchdownPointHeight += 0.01;
		RESETKEY(kstate, OAPI_KEY_A);

		SetTouchdownPoints (_V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
		VSSetTouchdownPoints(GetHandle(), _V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
	}

	if (KEYDOWN (kstate, OAPI_KEY_S)) {
		touchdownPointHeight -= 0.01;
		RESETKEY(kstate, OAPI_KEY_S);

		SetTouchdownPoints (_V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
		VSSetTouchdownPoints(GetHandle(), _V(0, touchdownPointHeight, 1), _V(-1, touchdownPointHeight, -1), _V(1, touchdownPointHeight, -1));
	}
	*/

	return 0;
}

int LRV::clbkConsumeBufferedKey(DWORD key, bool down, char *kstate) {

	TRACESETUP("clbkConsumeBufferedKey");

	if (FirstTimestep) return 0;

	if (KEYMOD_SHIFT(kstate) || KEYMOD_CONTROL(kstate)) {
		return 0; 
	}

	return 0;
}

void LRV::SetMissionPath()

{
	char MissionName[24];

	_snprintf(MissionName, 23, "Apollo%d", ApolloNo);
	soundlib.SetSoundLibMissionPath(MissionName);
}

void LRV::SetLRVStats(LRVSettings &lrvs)

{
	ApolloNo = lrvs.MissionNo;
	StateSet = true;
}

void LRV::DoFirstTimestep()

{
	TRACESETUP("FirstTimestep");
	//
	// Load mission-specific sounds _AFTER_ the LEM has called us to set the Apollo mission number.
	//

	if (StateSet) {
		// Probably a better way but I'm not familiar with the API
		soundlib.InitSoundLib(oapiGetVesselInterface(GetHandle()), SOUND_DIRECTORY);
		SetMissionPath();

		//
		// Load sounds if they may be required.
		//

		if (!SLEVAPlayed)
			soundlib.LoadMissionSound(SLEVA, LEVA_SOUND, LEVA_SOUND);

		//
		// Turn off pretty much everything that Orbitersound does by default.
		//

		soundlib.SoundOptionOnOff(PLAYCOUNTDOWNWHENTAKEOFF, FALSE);
		soundlib.SoundOptionOnOff(PLAYCABINAIRCONDITIONING, FALSE);
		soundlib.SoundOptionOnOff(PLAYCABINRANDOMAMBIANCE, FALSE);
		soundlib.SoundOptionOnOff(PLAYRADIOATC, FALSE);
		soundlib.SoundOptionOnOff(PLAYRADARBIP, FALSE);
		soundlib.SoundOptionOnOff(DISPLAYTIMER, FALSE);

		FirstTimestep = false;
	}
}

void LRV::SetNeedleAngle(int idx, double val, double min_val, double max_val)

{
	//
	// Move one of the 8 needles of the 4 power/temp gauges
	//
	double pos = __min(1.0, __max(0.0, (val - min_val) / (max_val - min_val)));  // position in [0 .. 1]
	double needle_rad = Radians(pos * (VCC_NEEDLE_MAXPOS[idx] - VCC_NEEDLE_MINPOS[idx]));

	mgtRotGauges.P.rotparam.angle = float(needle_rad - vccNeedleAngle[idx]);
	mgtRotGauges.ngrp = VCC_NEEDLE_GROUPS[idx];		
	vccNeedleAngle[idx] = needle_rad;
	MeshgroupTransform(vccVis, mgtRotGauges);
}

void LRV::clbkPreStep (double SimT, double SimDT, double mjd)

{
//	VESSELSTATUS csmV;
	VESSELSTATUS evaV;
	VECTOR3 rdist = {0,0,0};
	VECTOR3 posr  = {0,0,0};
	VECTOR3 rvel  = {0,0,0};
	VECTOR3 RelRot  = {0,0,0};
	double dist = 0.0;
	double Vel = 0.0;
	double heading;

	StepCount++;

	if (FirstTimestep) {
		DoFirstTimestep();
		return;
	}
	else if (!SLEVAPlayed && StepCount > 20) {
		//
		// We can't play this immediately on creation, otherwise Orbitersound gets
		// confused by the focus change. Instead we have to wait a few timsteps.
		//
		SLEVA.play();
		SLEVA.done();
		SLEVAPlayed = true;
	}

/*	if (!MotherShip)
		ScanMotherShip();
*/
	
	GetStatus(evaV);
	oapiGetHeading(GetHandle(),&heading);
	//
	// if the VESSELSTATUS is not in the state "landed", force it, using stored
	// values for lat and long that are updated in MoveLRV().
	//
	if ((evaV.status != 1) && lastLatLongSet) {
		evaV.vdata[0].x = lastLong;
		evaV.vdata[0].y = lastLat;
		evaV.vdata[0].z = heading;
		evaV.status = 1;
	}
	//
	// Get reference lat and long for the VC console, as soon as we have "landed" status
	//
	if (!vccInitialized && (evaV.status == 1)) {
		vccInitLat = evaV.vdata[0].y;
		vccInitLong = evaV.vdata[0].x;
		vccInitialized = true;
	}

/*	if (hMaster){
		LEM *lmvessel = (LEM *) oapiGetVesselInterface(hMaster);					
		oapiGetRelativePos (GetHandle() ,hMaster, &posr);
		oapiGetRelativeVel (GetHandle() ,hMaster , &rvel);
		lmvessel->GetStatus(csmV);
		GlobalRot (posr, RelRot);
		dist = sqrt(posr.x * posr.x + posr.y * posr.y + posr.z * posr.z);
		Vel = sqrt(rvel.x * rvel.x + rvel.y * rvel.y + rvel.z * rvel.z);
		/*if (GoDock1) {						
			if (dist <= 6.00550) {
				GoDock1 = false;
				lmvessel->StopEVA();
				oapiSetFocusObject(hMaster);
				oapiDeleteVessel(GetHandle());
				return;
			}
		}*//*
	}
*/
	//
	// update the VC console
	//
	if (vccVis) {
		// rotate the VC compass rose
		mgtRotCompass.P.rotparam.angle = float(heading - vccCompAngle);
		vccCompAngle = heading;
		MeshgroupTransform(vccVis, mgtRotCompass);

		// Rotate the speed dial.
		double abs_speed_kmh = fabs(speed * 3.6);  // absolute speed in km/h
		double speed_dial_rad = -(abs_speed_kmh - 10.0) * Radians(31.25) / 10.0;
		mgtRotSpeed.P.rotparam.angle = float(speed_dial_rad - vccSpeedAngle);
		vccSpeedAngle = speed_dial_rad;
		MeshgroupTransform(vccVis, mgtRotSpeed);

		// Rotate the 8 needles
		SetNeedleAngle(0, Bat1Cap, -20.0, 130.0);
		SetNeedleAngle(1, Bat2Cap, -20.0, 130.0); 
		SetNeedleAngle(2, 2.0 * Bat1Volt, 0.0, 100.0);
		SetNeedleAngle(3, 2.0 * Bat2Volt, 0.0, 100.0); 
		SetNeedleAngle(4, Bat1Temp, 0.0, 180.0); 
		SetNeedleAngle(5, Bat2Temp, 0.0, 180.0); 
		SetNeedleAngle(6, 0.0, 200.0, 500.0);
		SetNeedleAngle(7, 0.0, 200.0, 500.0); 

		// Display distance travelled (in km) since last initialization
		double distance = vccDistance / 1000.0;
		while (distance > 100.0) distance = distance - 100.0;
		double digit = floor(distance/10.0);  // tens
		double drum_rot = (digit * PI / 5.0) - vccDist100Angle;
		vccDist100Angle = digit * PI / 5.0;
		mgtRotDrums.P.rotparam.angle = float(drum_rot);
		mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_LOWER;
		mgtRotDrums.ngrp = GEOM_DRUM_DIST_10_0;
		MeshgroupTransform(vccVis, mgtRotDrums);
		distance = distance - 10.0 * digit;
		digit = floor(distance);  // ones
		drum_rot = (digit * PI / 5.0) - vccDist010Angle;
		vccDist010Angle = digit * PI / 5.0;
		mgtRotDrums.P.rotparam.angle = float(drum_rot);
		mgtRotDrums.ngrp = GEOM_DRUM_DIST_01_0;
		MeshgroupTransform(vccVis, mgtRotDrums);
		distance = distance - digit;
		digit = floor(10.0 * distance);  // tenths
		drum_rot = (digit * PI / 5.0) - vccDist001Angle;
		vccDist001Angle = digit * PI / 5.0;
		mgtRotDrums.P.rotparam.angle = float(drum_rot);
		mgtRotDrums.ngrp = GEOM_DRUM_DIST_00_1;
		MeshgroupTransform(vccVis, mgtRotDrums);

		if (vccInitialized) {  // we need the reference lat and long for this ...
			// Display bearing to last reference point (usually the LM)
			double bearing = Degree(CalcSphericalBearing(_V(vccInitLong, vccInitLat, 0.0), evaV.vdata[0]));
			bearing = bearing - 90.0;  // correct bearing to local North
			while (bearing < 0.0) bearing += 360.0;
			digit = floor(bearing/100.0);  // hundreds
			drum_rot = (digit * PI / 5.0) - vccBear100Angle;
			vccBear100Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_UPPER;
			mgtRotDrums.ngrp = GEOM_DRUM_BEAR_100;
			MeshgroupTransform(vccVis, mgtRotDrums);
			bearing = bearing - 100.0 * digit;
			digit = floor(bearing/10.0);  // tens
			drum_rot = (digit * PI / 5.0) - vccBear010Angle;
			vccBear010Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_BEAR_010;
			MeshgroupTransform(vccVis, mgtRotDrums);
			bearing = bearing - 10.0 * digit;
			digit = floor(bearing);  // ones
			drum_rot = (digit * PI / 5.0) - vccBear001Angle;
			vccBear001Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_BEAR_001;
			MeshgroupTransform(vccVis, mgtRotDrums);

			// Display range (in km) to last reference point (usually the LM)
			double range = CalcSphericalDistance(_V(vccInitLong, vccInitLat, 0.0), evaV.vdata[0], oapiGetSize(evaV.rbody))/1000.0;
			if (range < 0.0) range = -range;
			while (range > 100.0) range = range - 100.0;
			digit = floor(range/10.0);  // tens
			drum_rot = (digit * PI / 5.0) - vccRange100Angle;
			vccRange100Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.P.rotparam.ref = LRV_DRUM_PIVOT_LOWER;
			mgtRotDrums.ngrp = GEOM_DRUM_RNGE_10_0;
			MeshgroupTransform(vccVis, mgtRotDrums);
			range = range - 10.0 * digit;
			digit = floor(range);  // ones
			drum_rot = (digit * PI / 5.0) - vccRange010Angle;
			vccRange010Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_RNGE_01_0;
			MeshgroupTransform(vccVis, mgtRotDrums);
			range = range - digit;
			digit = floor(10.0 * range);  // tenths
			drum_rot = (digit * PI / 5.0) - vccRange001Angle;
			vccRange001Angle = digit * PI / 5.0;
			mgtRotDrums.P.rotparam.angle = float(drum_rot);
			mgtRotDrums.ngrp = GEOM_DRUM_RNGE_00_1;
			MeshgroupTransform(vccVis, mgtRotDrums);
		}
	}


	MoveLRV(SimDT, &evaV, heading);

	UpdateAnimations(SimDT);

	DoAnimations();
	
	// touchdown point test
	// sprintf(oapiDebugString(), "touchdownPointHeight %f", touchdownPointHeight);
}

void LRV::DoAnimations ()
{
	TRACESETUP("DoAnimations");

	SetAnimation(anim_fntrgttire, proc_tires_right);
	SetAnimation(anim_fntlfttire, proc_tires_left);
	SetAnimation(anim_rearrgttire, proc_tires_right);
	SetAnimation(anim_rearlfttire, proc_tires_left);

	if (steering > 0.0)  // right turn
	{
		SetAnimation(anim_fntrgtwheel, proc_frontwheel_inner);
		SetAnimation(anim_fntlftwheel, proc_frontwheel_outer);
		SetAnimation(anim_rearrgtwheel, proc_rearwheel_inner);
		SetAnimation(anim_rearlftwheel, proc_rearwheel_outer);
	} else {  // left turn or straight
		SetAnimation(anim_fntrgtwheel, proc_frontwheel_outer);
		SetAnimation(anim_fntlftwheel, proc_frontwheel_inner);
		SetAnimation(anim_rearrgtwheel, proc_rearwheel_outer);
		SetAnimation(anim_rearlftwheel, proc_rearwheel_inner);
	}
	//SetAnimation(anim_fntrgtfender, proc_fntrgtfender);
	//SetAnimation(anim_fntlftfender, proc_fntlftfender);
	//SetAnimation(anim_rearrgtfender, proc_rearrgtfender);
	//SetAnimation(anim_rearlftfender, proc_rearlftfender);
}

void LRV::UpdateAnimations (double SimDT)
{
	double d_tires;

	TRACESETUP("UpdateAnimations");

	///
	/// \todo Draw random number to see if wheels hit a bump and it's magnitude or determine wheel motion due to vertical accelleration by bouncing
	///
	double outer_steering = AckermannInnerToOuter(steering * PI * (MAX_WHEEL_TURN_INNER_DEG / 180.0)) * 180.0 / (PI * MAX_WHEEL_TURN_INNER_DEG);
	proc_frontwheel_inner = (0.5 + 0.5 * steering); //Right is positive, left is negative
	proc_frontwheel_outer = (0.5 + 0.5 * outer_steering); //Right is positive, left is negative
	proc_rearwheel_inner = (0.5 - 0.5 * steering); //rear wheels steer opposite of front wheels
	proc_rearwheel_outer = (0.5 - 0.5 * outer_steering); //rear wheels steer opposite of front wheels

	// read speed and determine change in omega in wheel rotation in SimDT time
	double inner_angle_rad, outer_angle_rad, turn_radius_center_cm, turn_radius_inner_cm, turn_radius_outer_cm;
	d_tires = WHEEL_SPIN_FUDGE_FACTOR * (speed * SimDT) / (2.0 * PI * WHEEL_RADIUS_M);
    // calculate inner and outer circle distance and vary speed accordingly
	if (fabs(steering) < 0.0001)
	{
		// avoid inner_angle_rad = 0.0, at which point get_turn_radius_for_center() is undefined
		
		// the actual values are not important, they just have to be equal
		turn_radius_inner_cm = 1.0;
		turn_radius_outer_cm = 1.0;
		turn_radius_center_cm = 1.0;
	}
	else
	{
		inner_angle_rad = Radians(fabs(steering) * MAX_WHEEL_TURN_INNER_DEG);
		outer_angle_rad = Radians(fabs(outer_steering) * MAX_WHEEL_TURN_INNER_DEG);
		turn_radius_center_cm = get_turn_radius_for_center(inner_angle_rad);
		turn_radius_inner_cm = 0.5 * WHEEL_BASE_CM / cos(PI05 - inner_angle_rad);
		turn_radius_outer_cm = 0.5 * WHEEL_BASE_CM / cos(PI05 - outer_angle_rad);;
	}
	
	if (steering > 0.0)
	{
		proc_tires_left = proc_tires_left + d_tires * (turn_radius_outer_cm/turn_radius_center_cm);
		proc_tires_right = proc_tires_right + d_tires * (turn_radius_inner_cm/turn_radius_center_cm);
	}
	else
	{
		proc_tires_left = proc_tires_left + d_tires * (turn_radius_inner_cm/turn_radius_center_cm);
		proc_tires_right = proc_tires_right + d_tires * (turn_radius_outer_cm/turn_radius_center_cm);
	}

	// check to see if animations hit limits and adjust accordingly
	if (proc_tires_left > 1){
		proc_tires_left = proc_tires_left - 1;
	} else if (proc_tires_left < 0) {
		proc_tires_left = proc_tires_left + 1;
	}
	if (proc_tires_right > 1){
		proc_tires_right = proc_tires_right - 1;
	} else if (proc_tires_right < 0) {
		proc_tires_right = proc_tires_right + 1;
	}

	//sprintf(oapiDebugString(), "proc_tire %f", proc_tires);

}

void LRV::clbkLoadStateEx(FILEHANDLE scn, void *vs)

{
    char *line;
	
	while (oapiReadScenario_nextline (scn, line)) 
	{
		if (!strnicmp (line, "STATE", 5)) 
		{
			int	s;
			sscanf(line + 5, "%d", &s);
			SetMainState(s);

			SetEngineLevel(ENGINE_HOVER,1);
		}
		else if (!strnicmp (line, "MISSIONNO", 9)) 
		{
			sscanf(line + 9, "%d", &ApolloNo);
		}
		else 
		{
            ParseScenarioLineEx (line, vs);
        }
    }
}


	
	
void LRV::clbkVisualCreated (VISHANDLE vis, int refcount)
{
	vccVis = vis;
}

void LRV::clbkVisualDestroyed (VISHANDLE vis, int refcount)
{
	vccVis = NULL;
	// reset the variables keeping track of console mesh animation
	vccCompAngle = 0.0;
	vccDist100Angle = 0.0;
	vccDist010Angle = 0.0;
	vccDist001Angle = 0.0;
	vccBear100Angle = 0.0;
	vccBear010Angle = 0.0;
	vccBear001Angle = 0.0;
	vccRange100Angle = 0.0;
	vccRange010Angle = 0.0;
	vccRange010Angle = 0.0;
	vccSpeedAngle = 0.0;
	for (int i=0; i<8; i++)
		vccNeedleAngle[i] = - Radians(VCC_NEEDLE_MINPOS[i]);
}

typedef union {
	struct {
		unsigned int StateSet:1;
		unsigned int SLEVAPlayed:1;
	} u;
	unsigned int word;
} MainLEVAState;

int LRV::GetMainState()

{
	MainLEVAState s;

	s.word = 0;
	s.u.StateSet = StateSet;
	s.u.SLEVAPlayed = SLEVAPlayed;

	return s.word;
}

void LRV::SetMainState(int n)

{
	MainLEVAState s;

	s.word = n;
	StateSet = (s.u.StateSet != 0);
	SLEVAPlayed = (s.u.SLEVAPlayed != 0);
}

void LRV::clbkSaveState(FILEHANDLE scn)

{
	SaveDefaultState (scn);

	int s = GetMainState();
	if (s) {
		oapiWriteScenario_int (scn, "STATE", s);
	}

	if (ApolloNo != 0) {
		oapiWriteScenario_int (scn, "MISSIONNO", ApolloNo);
	}
}

DLLCLBK VESSEL *ovcInit (OBJHANDLE hvessel, int flightmodel)
{
	if (!refcount++) {
		hLRV = oapiLoadMeshGlobal ("ProjectApollo/LRV");
		hLRVConsole = oapiLoadMeshGlobal ("ProjectApollo/LRV_console");
	}

	return new LRV (hvessel, flightmodel);
}

DLLCLBK void ovcExit (VESSEL *vessel)
{
	LRV *sv = (LRV *) vessel;

	if (sv)
		delete sv;
}

DLLCLBK void InitModule (HINSTANCE hModule)
{

}

/* just some handy code for debugging purposes ...
char test[200];
sprintf(test, "DBG:%f, %f", steering, outer_deflect);
OutputDebugString(test);
*/
