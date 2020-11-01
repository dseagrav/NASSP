/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  Toggle switch handling code.
  This file defines a number of useful toggle switches which can
  be set up to avoid huge swathes of code checking for mouse events,
  checking and processing switch state, and displaying switch state.

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

#include "nasspdefs.h"
#include "nasspsound.h"

#include "toggleswitch.h"

#include "IMU.h"
#include "missiontimer.h"
#include "apolloguidance.h"
#include "ioChannels.h"
#include "powersource.h"
#include "fdai.h"
#include "scs.h"
#include "connector.h"
#include "checklistController.h"

#include "tracer.h"

//
// Generic panel switch item.
//

PanelSwitchItem::PanelSwitchItem()

{
	Failed = false;
	FailedState = 0;
	state = 0;

	name = 0;
	next = 0;
	nextForScenario = 0;

	DisplayName = 0;
	flashing = false;
	visible = true;
	doTimeStep = false;

	callback = 0;
}

PanelSwitchItem::~PanelSwitchItem()
{
	if (callback)
		delete callback;
}
void PanelSwitchItem::SetCallback(PanelSwitchCallbackInterface *call)
{
	if (callback)
		delete callback;
	callback = call;
}

char *PanelSwitchItem::GetDisplayName()

{
	if (DisplayName)
		return DisplayName;

	return name;
}

//
// Get the state. If it's failed, return the failed state.
//
// Note that you should always call this to get the state unless you're actually concerned with
// the appearance of the switch rather than the functionality.
//

int PanelSwitchItem::GetState()

{
	if (!Failed)
		return state;

	return FailedState;
}

void PanelSwitchItem::SetState(int value)

{
	state = value;
}


//
// Generic toggle switch.
//

ToggleSwitch::ToggleSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	springLoaded = SPRINGLOADEDSWITCH_NONE;

	next = 0;

	SwitchSurface = 0;
	BorderSurface = 0;

	OurVessel = 0;
	switchRow = 0;

	Active = true;
	Held = false;
	Sideways = 0;
	delayTime = 0;
	resetTime = 0;
}

ToggleSwitch::~ToggleSwitch() {
	Sclick.done();
}

void ToggleSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int springloaded, char *dname) {

	name = n;
	state = defaultState;
	springLoaded = springloaded;
	scnh.RegisterSwitch(this);

	DisplayName = dname;
}

void ToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset)

{
	x = xp;
	y = yp;
	width = w;
	height = h;
	xOffset = xoffset;
	yOffset = yoffset;
	SwitchSurface = surf;
	BorderSurface = bsurf;
	
	SwitchToggled = false;
	
	row.AddSwitch(this);
	switchRow = &row;
	OurVessel = switchRow->panelSwitches->vessel;

	InitSound(switchRow->panelSwitches->soundlib);
}

void ToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s, int xoffset, int yoffset) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	xOffset = xoffset;
	yOffset = yoffset;
	SwitchSurface = surf;

	InitSound(&s);
}

void ToggleSwitch::InitSound(SoundLib *s) {

	if (!Sclick.isValid())
		s->LoadSound(Sclick, CLICK_SOUND);
}

bool ToggleSwitch::SwitchTo(int newState, bool dontspring) {

	if (Active)
	{
		if (state != newState)
		{
			state = newState;
			SwitchToggled = true;
			if (switchRow)
			{
				if (switchRow->panelSwitches->listener) 
					switchRow->panelSwitches->listener->PanelSwitchToggled(this);
			}
			if (callback)
				callback->call(this);

			//
			// Reset the switch if it's spring-loaded and not held.
			//
			if (IsSpringLoaded() && !IsHeld() && !dontspring)
			{
				if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(TOGGLESWITCH_DOWN,true);
				if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(TOGGLESWITCH_UP,true);
			}
			return true;
		}
		if (IsSpringLoaded() && !IsHeld() && !dontspring)
		{
			if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   return SwitchTo(TOGGLESWITCH_DOWN,true);
			if (springLoaded == SPRINGLOADEDSWITCH_UP)     return SwitchTo(TOGGLESWITCH_UP,true);
		}
	}
	return false;
}

unsigned int ToggleSwitch::GetFlags()

{
	ToggleSwitchFlags fs;

	fs.Held = Held ? 1 : 0;

	return fs.flags;
}

void ToggleSwitch::SetFlags(unsigned int f)

{
	ToggleSwitchFlags fs;

	fs.flags = f;

	Held = (fs.Held != 0);
}

bool ToggleSwitch::DoCheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	//
	// Check whether it's actually in our switch region.
	//

	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//

	if (event & PANEL_MOUSE_LBDOWN) {
		if ((!Sideways && my > (y + (height / 2.0))) || (Sideways && mx < (x + (width / 2.0)))) {
			if (state != TOGGLESWITCH_DOWN) {
				SwitchTo(TOGGLESWITCH_DOWN,true);
				Sclick.play();
			}
		}
		else {
			if (state != TOGGLESWITCH_UP) {
				SwitchTo(TOGGLESWITCH_UP,true);
				Sclick.play();
			}
		}
	}
	else if (IsSpringLoaded() && ((event & PANEL_MOUSE_LBUP) != 0) && !IsHeld()) {
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(TOGGLESWITCH_DOWN);
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(TOGGLESWITCH_UP);
	}
	return true;
}

bool ToggleSwitch::CheckMouseClick(int event, int mx, int my) {

	if (visible)
		return DoCheckMouseClick(event, mx, my);
	else
		return false;
}

void ToggleSwitch::DoDrawSwitch(SURFHANDLE DrawSurface)

{
	if (IsUp())
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset, yOffset, width, height, SURF_PREDEF_CK);
	}
	else
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset + width, yOffset, width, height, SURF_PREDEF_CK);
	}
}

void ToggleSwitch::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (visible) 
		DoDrawSwitch(DrawSurface);
}

//
// Generic function to draw a flashing box around the switch. This is only called if the
// flashing is currently active.
//

void ToggleSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (BorderSurface)
		oapiBlt(DrawSurface, BorderSurface, x, y, 0, 0, width, height, SURF_PREDEF_CK);
}

void ToggleSwitch::SetActive(bool s) {
	Active = s;
}

void ToggleSwitch::SaveState(FILEHANDLE scn)

{
	char buffer[1000];

	sprintf(buffer, "%i %u", state, GetFlags()); 
	oapiWriteScenario_string(scn, name, buffer);
}

void ToggleSwitch::LoadState(char *line)
{
	// Load state
	char buffer[100];
	int st = 0;
	unsigned int f = 0;

	sscanf(line, "%s %i %u", buffer, &st, &f);
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		SetFlags(f);
	}
}

void ToggleSwitch::SetState(int value)
{
	if (!delayTime) {
		if (SwitchTo(value))
			Sclick.play();
	} else {
		if (SwitchTo(value, true)) {
			Sclick.play();
			doTimeStep = true;
			resetTime = switchRow->panelSwitches->lastexecutedtime + delayTime;
		}
	}
}

void ToggleSwitch::timestep(double missionTime)
{
	if (missionTime <= resetTime)
		return;
	doTimeStep = false;
	SwitchTo(state);
}

bool ToggleSwitch::ProcessMouseVC(int event, VECTOR3 &p)
{
	int OldState = state;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//

	if (event & PANEL_MOUSE_LBDOWN) {
		if (state != TOGGLESWITCH_DOWN) {
			SwitchTo(TOGGLESWITCH_DOWN, true);
			Sclick.play();
		}

		else {
			if (state != TOGGLESWITCH_UP) {
				SwitchTo(TOGGLESWITCH_UP, true);
				Sclick.play();
			}
		}
	}
	else if (IsSpringLoaded() && ((event & (PANEL_MOUSE_LBUP | PANEL_MOUSE_RBUP)) != 0) && !IsHeld()) {
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(TOGGLESWITCH_DOWN);
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(TOGGLESWITCH_UP);
	}
	return true;
}

void ToggleSwitch::RedrawVC(UINT anim)
{
	if (IsUp()) {
		OurVessel->SetAnimation(anim, 1.0);
	}
	else if (IsCenter())
	{
		OurVessel->SetAnimation(anim, 0.5);
	}
	else
	{
		OurVessel->SetAnimation(anim, 0.0);
	}
}

//
// Three pos switch.
//

bool ThreePosSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	//
	// Check whether it's actually in our switch region.
	//

	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//
	if (event & PANEL_MOUSE_LBDOWN) {
		if ((Sideways == 0 && my > (y + (height / 2.0))) || (Sideways == 1 && mx < (x + (width / 2.0))) || (Sideways == 2 && mx > (x + (width / 2.0)))) {
			if (state > 0) {
				SwitchTo(state - 1, true);
				Sclick.play();
			}
		}
		else {
			if (state < 2) {
				SwitchTo(state + 1, true);
				Sclick.play();
			}
		}

	}
	else if (IsSpringLoaded() && ((event & PANEL_MOUSE_LBUP) != 0) && !IsHeld()) {		
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(THREEPOSSWITCH_DOWN,true);
		if (springLoaded == SPRINGLOADEDSWITCH_CENTER) SwitchTo(THREEPOSSWITCH_CENTER,true);
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(THREEPOSSWITCH_UP,true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)     
			SwitchTo(THREEPOSSWITCH_CENTER,true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)     
			SwitchTo(THREEPOSSWITCH_CENTER);
	}
	return true;
}

bool ThreePosSwitch::ProcessMouseVC(int event, VECTOR3 &p)
{
	int OldState = state;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//
	if (event & PANEL_MOUSE_LBDOWN) {
		if (Sideways == 0 || Sideways == 2 ) {
			if (state < 2) {
				SwitchTo(state + 1, true);
				Sclick.play();
			}
		} else {
			if (state > 0) {
				SwitchTo(state - 1, true);
				Sclick.play();
			}
		}
	} else if (event & PANEL_MOUSE_RBDOWN) {
		if (Sideways == 1) {
			if (state < 2) {
				SwitchTo(state + 1, true);
				Sclick.play();
			}
		} else {
			if (state > 0) {
				SwitchTo(state - 1, true);
				Sclick.play();
			}
		}
	}

	else if (IsSpringLoaded() && ((event & (PANEL_MOUSE_LBUP | PANEL_MOUSE_RBUP)) != 0) && !IsHeld()) {
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(THREEPOSSWITCH_DOWN, true);
		if (springLoaded == SPRINGLOADEDSWITCH_CENTER) SwitchTo(THREEPOSSWITCH_CENTER, true);
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(THREEPOSSWITCH_UP, true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)
			SwitchTo(THREEPOSSWITCH_CENTER, true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)
			SwitchTo(THREEPOSSWITCH_CENTER);
	}
	return true;
}

void ThreePosSwitch::DrawSwitch(SURFHANDLE DrawSurface)

{
	oapiBlt(DrawSurface, SwitchSurface, x, y, (state * width), 0, width, height, SURF_PREDEF_CK);
}

bool ThreePosSwitch::SwitchTo(int newState, bool dontspring)

{
	if (Active)
	{
		if (state != newState)
		{
			state = newState;
			SwitchToggled = true;
			if (switchRow) {
				if (switchRow->panelSwitches->listener) 
					switchRow->panelSwitches->listener->PanelSwitchToggled(this);
			}
			if (callback)
				callback->call(this);

			if (IsSpringLoaded() && !dontspring && !IsHeld()) {
				if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(THREEPOSSWITCH_DOWN,true);
				if (springLoaded == SPRINGLOADEDSWITCH_CENTER) SwitchTo(THREEPOSSWITCH_CENTER,true);
				if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(THREEPOSSWITCH_UP,true);

				if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)     
						SwitchTo(THREEPOSSWITCH_CENTER,true);

				if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)     
						SwitchTo(THREEPOSSWITCH_CENTER,true);
			}
			return true;
		}
		if (IsSpringLoaded() && !dontspring && !IsHeld()) {
			if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   return SwitchTo(THREEPOSSWITCH_DOWN,true);
			if (springLoaded == SPRINGLOADEDSWITCH_CENTER) return SwitchTo(THREEPOSSWITCH_CENTER,true);
			if (springLoaded == SPRINGLOADEDSWITCH_UP)     return SwitchTo(THREEPOSSWITCH_UP,true);

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)     
					return SwitchTo(THREEPOSSWITCH_CENTER,true);

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)     
					return SwitchTo(THREEPOSSWITCH_CENTER,true);
		}
	}
	return false;
}

//
// Five pos switch.
//

bool FivePosSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	//
	// Check whether it's actually in our switch region.
	//

	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//
	// FIXME: This assumes the switch is 39x39px in size. Should be variable.
	if (event == PANEL_MOUSE_LBDOWN) {
		if (my > 24 && (mx < 30 && mx > 7)){
			// Down
			if(state != FIVEPOSSWITCH_DOWN){
				SwitchTo(FIVEPOSSWITCH_DOWN, true);
				Sclick.play();
			}
			return true;
		}
		if(my < 10 && (mx < 30 && mx > 7)){
			// Up
			if (state != FIVEPOSSWITCH_UP) {
				SwitchTo(FIVEPOSSWITCH_UP, true);
				Sclick.play();
			}
			return true;
		}
		if(mx > 30){
			// Right
			if (state != FIVEPOSSWITCH_RIGHT) {
				SwitchTo(FIVEPOSSWITCH_RIGHT, true);
				Sclick.play();
			}
			return true;
		}
		if(mx < 7){
			// Left
			if (state != FIVEPOSSWITCH_LEFT) {
				SwitchTo(FIVEPOSSWITCH_LEFT, true);
				Sclick.play();
			}
			return true;
		}
		// Otherwise
		if (state != FIVEPOSSWITCH_CENTER) {
			SwitchTo(FIVEPOSSWITCH_CENTER, true);
			Sclick.play();
		}
	} else if (IsSpringLoaded() && event == PANEL_MOUSE_LBUP && !IsHeld()) {		
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(FIVEPOSSWITCH_DOWN,true);
		if (springLoaded == SPRINGLOADEDSWITCH_CENTER) SwitchTo(FIVEPOSSWITCH_CENTER,true);
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(FIVEPOSSWITCH_UP,true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == FIVEPOSSWITCH_UP)     
			SwitchTo(FIVEPOSSWITCH_CENTER,true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == FIVEPOSSWITCH_DOWN)     
			SwitchTo(FIVEPOSSWITCH_CENTER);
	}
	return true;
}

bool FivePosSwitch::CheckMouseClickVC(int event, VECTOR3 &p) {

	int OldState = state;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);

	if (IsSpringLoaded())
		SetHeld((ctrlState & 0x8000) != 0);

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//
	if (event == PANEL_MOUSE_LBDOWN) {
		if (p.y > 0.5 && (p.x < 0.66 && p.x > 0.33)) {
			// Down
			if (state != FIVEPOSSWITCH_DOWN) {
				SwitchTo(FIVEPOSSWITCH_DOWN, true);
				Sclick.play();
			}
			return true;
		}
		if (p.y < 0.5 && (p.x < 0.66 && p.x > 0.33)) {
			// Up
			if (state != FIVEPOSSWITCH_UP) {
				SwitchTo(FIVEPOSSWITCH_UP, true);
				Sclick.play();
			}
			return true;
		}
		if (p.x > 0.66) {
			// Right
			if (state != FIVEPOSSWITCH_RIGHT) {
				SwitchTo(FIVEPOSSWITCH_RIGHT, true);
				Sclick.play();
			}
			return true;
		}
		if (p.x < 0.33) {
			// Left
			if (state != FIVEPOSSWITCH_LEFT) {
				SwitchTo(FIVEPOSSWITCH_LEFT, true);
				Sclick.play();
			}
			return true;
		}
		// Otherwise
		if (state != FIVEPOSSWITCH_CENTER) {
			SwitchTo(FIVEPOSSWITCH_CENTER, true);
			Sclick.play();
		}
	}
	else if (IsSpringLoaded() && event == PANEL_MOUSE_LBUP && !IsHeld()) {
		if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(FIVEPOSSWITCH_DOWN, true);
		if (springLoaded == SPRINGLOADEDSWITCH_CENTER) SwitchTo(FIVEPOSSWITCH_CENTER, true);
		if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(FIVEPOSSWITCH_UP, true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == FIVEPOSSWITCH_UP)
			SwitchTo(FIVEPOSSWITCH_CENTER, true);

		if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == FIVEPOSSWITCH_DOWN)
			SwitchTo(FIVEPOSSWITCH_CENTER);
	}
	return true;
}

void FivePosSwitch::DrawSwitch(SURFHANDLE DrawSurface)

{
	oapiBlt(DrawSurface, SwitchSurface, x, y, (state * width), 0, width, height, SURF_PREDEF_CK);
}

void FivePosSwitch::DrawSwitchVC(UINT animx, UINT animy)

{
	if (state == FIVEPOSSWITCH_UP) {
		OurVessel->SetAnimation(animx, 0.5);
		OurVessel->SetAnimation(animy, 1.0);
	}
	else if (state == FIVEPOSSWITCH_RIGHT) {
		OurVessel->SetAnimation(animx, 1.0);
		OurVessel->SetAnimation(animy, 0.5);
	}
	else if (state == FIVEPOSSWITCH_DOWN) {
		OurVessel->SetAnimation(animx, 0.5);
		OurVessel->SetAnimation(animy, 0.0);
	}
	else if (state == FIVEPOSSWITCH_LEFT) {
		OurVessel->SetAnimation(animx, 0.0);
		OurVessel->SetAnimation(animy, 0.5);
	}
	else {
		OurVessel->SetAnimation(animx, 0.5);
		OurVessel->SetAnimation(animy, 0.5);
	}
}

bool FivePosSwitch::SwitchTo(int newState, bool dontspring)

{
	if (Active)
	{
		if (state != newState)
		{
			state = newState;
			SwitchToggled = true;
			if (switchRow) {
				if (switchRow->panelSwitches->listener) 
					switchRow->panelSwitches->listener->PanelSwitchToggled(this);
			}
			if (callback)
				callback->call(this);

			if (IsSpringLoaded() && !dontspring && !IsHeld()) {
				if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(FIVEPOSSWITCH_DOWN,true);
				if (springLoaded == SPRINGLOADEDSWITCH_CENTER) SwitchTo(FIVEPOSSWITCH_CENTER,true);
				if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(FIVEPOSSWITCH_UP,true);

				if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == FIVEPOSSWITCH_UP)     
						SwitchTo(FIVEPOSSWITCH_CENTER,true);

				if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == FIVEPOSSWITCH_DOWN)     
						SwitchTo(FIVEPOSSWITCH_CENTER,true);
			}
			return true;
		}
		if (IsSpringLoaded() && !dontspring && !IsHeld()) {
			if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   return SwitchTo(FIVEPOSSWITCH_DOWN,true);
			if (springLoaded == SPRINGLOADEDSWITCH_CENTER) return SwitchTo(FIVEPOSSWITCH_CENTER,true);
			if (springLoaded == SPRINGLOADEDSWITCH_UP)     return SwitchTo(FIVEPOSSWITCH_UP,true);

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == FIVEPOSSWITCH_UP)     
					return SwitchTo(FIVEPOSSWITCH_CENTER,true);

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == FIVEPOSSWITCH_DOWN)     
					return SwitchTo(FIVEPOSSWITCH_CENTER,true);
		}
	}
	return false;
}

//
// Push button like switch.  Now implemented as a special case of toggle switch (springloaded and special sound)
//
void PushSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, char *dname)
{
	ToggleSwitch::Register(scnh, n, defaultState, 1, dname);
}

// Special case check mouse click, same result top and Bottom.
bool PushSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	if (!visible) return false;
	if (mx < x || my < y) return false;
	if (mx > (x + width) || my > (y + height)) return false;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);
	SetHeld((ctrlState & 0x8000) != 0);

	if (event == PANEL_MOUSE_LBDOWN) {
		SwitchTo(1, true);
		Sclick.play();
	} else if (event == PANEL_MOUSE_LBUP && !IsHeld()) {
		SwitchTo(0, true);
	}
	return true;
}

bool PushSwitch::ProcessMouseVC(int event, VECTOR3 &p) {

	int OldState = state;

	if (!visible) return false;

	///
	/// \todo Get CTRL state properly if and when Orbiter supports it.
	///
	SHORT ctrlState = GetKeyState(VK_SHIFT);
	SetHeld((ctrlState & 0x8000) != 0);

	if (event == PANEL_MOUSE_LBDOWN) {
		SwitchTo(1, true);
		Sclick.play();
	}
	else if (event == PANEL_MOUSE_LBUP && !IsHeld()) {
		SwitchTo(0, true);
	}
	return true;
}

void PushSwitch::InitSound(SoundLib *s) {

	if (!Sclick.isValid())
		s->LoadSound(Sclick, BUTTON_SOUND);
}


//
// Circuit braker switch.  Special case of Toggle Switch
//

bool CircuitBrakerSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	if (!visible) return false;
	if (mx < x || my < y) return false;
	if (mx > (x + width) || my > (y + height)) return false;

	if (event == PANEL_MOUSE_LBDOWN) {
		if (state) {
			SwitchTo(0);
			Sclick.play();
		} else {
			SwitchTo(1);
			Sclick.play();
		}
	}
	return true;
}

double CircuitBrakerSwitch::Voltage()
{
	if ((state != 0) && SRC)
		return SRC->Voltage();

	return 0.0;
}

double CircuitBrakerSwitch::Current()
{	
	if ((state != 0) && SRC && SRC->IsEnabled()) {
		Volts = SRC->Voltage();
		if (Volts > 0.0)
			Amperes = (power_load / Volts);
		else 
			Amperes = 0.0; 
	}
	else {
		return 0.0; //remove the else part of this if it causes problems. the function is only ever called by debug strings and I find this implimentation to be more useful --n7275
	}
	return Amperes;
}

double CircuitBrakerSwitch::Frequency()
{
	if ((state != 0) && SRC)
		return SRC->Frequency();

	return 0.0;
}

void CircuitBrakerSwitch::InitSound(SoundLib *s) {

	if (!Sclick.isValid())
		s->LoadSound(Sclick, CIRCUITBREAKER_SOUND);
}

void CircuitBrakerSwitch::DrawPower(double watts)

{
	/// \todo Note that this will not work properly if more than one source draws from a CB.

	// Default the power load to zero
	power_load = 0; 

	//
	// Do nothing if the breaker is open.
	//

	if (state == 0){
		return;
	}

	//
	// Check the current isn't over the max rating, and if it is then
	// pop the breaker.
	//

	if (SRC) {
		double volts = SRC->Voltage();
		if (volts > 0.0) {
			double amps = watts / volts;
			if (amps > MaxAmps) {				
				state = 0;
				SwitchToggled = true;

				if (switchRow) {
					if (switchRow->panelSwitches->listener) 
						switchRow->panelSwitches->listener->PanelSwitchToggled(this);
				}
				if (callback)
					callback->call(this);
				return;
			}
		}
		
		power_load = watts; // Make this available for reading draw across a CB from later (Used in the LM)
		e_object::DrawPower(watts);
	}
}

void CircuitBrakerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s, double amps)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	if (s) {
		WireTo(s);
	}

	MaxAmps = amps;
}

//
// Attitude mode toggle switch.
//

/*bool AttitudeToggle::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (!visible)
		return false;

	if (!DoCheckMouseClick(event, mx, my))
		return false;

	if (!Active || state == OldState)
		return true;



	return true;
}*/

bool AttitudeToggle::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState,dontspring))
	{
		if (GetState()) {
			OurVessel->SetAttitudeMode(ATTMODE_ROT);
		}
		else {
			OurVessel->SetAttitudeMode(ATTMODE_LIN);
		}
		return true;
	}
	return false;
}

void AttitudeToggle::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;
	
	if (Active) {
		if (OurVessel->GetAttitudeMode() == ATTMODE_ROT) {
			state = true;
		}
		else {
			state = false;
		}
	}

	DoDrawSwitch(DrawSurface);
}

//
// HUD mode toggle switches.
//

void HUDToggle::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int mode, SoundLib &s)

{
	HUDMode = mode;
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	row.AddSwitch(this);
}

void HUDToggle::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (Active) {
		if (oapiGetHUDMode() == HUDMode) {
			state = true;
		}
		else {
			state = false;
		}
	}

	DoDrawSwitch(DrawSurface);
}

/*bool HUDToggle::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (!DoCheckMouseClick(event, mx, my))
		return false;

	if (!Active || state == OldState)
		return true;

	if (GetState()) {
		oapiSetHUDMode(HUDMode);
	}
	else {
		oapiSetHUDMode(HUD_NONE) ;
	}

	return true;
}*/

bool HUDToggle::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState,dontspring))
	{
		if (GetState()) 
		{
			oapiSetHUDMode(HUDMode);
		}
		else 
		{
			oapiSetHUDMode(HUD_NONE) ;
		}
		return true;
	}
	return false;
}

//
// NAV mode toggle switches.
//

void NavModeToggle::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, VESSEL *v, int mode, SoundLib &s)

{
	NAVMode = mode;
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
}

void NavModeToggle::DrawSwitch(SURFHANDLE DrawSurface)

{
	if (Active) {
		if (OurVessel->GetNavmodeState(NAVMode)) {
			state = true;
		}
		else {
			state = false;
		}
	}

	DoDrawSwitch(DrawSurface);
}

/*bool NavModeToggle::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (!DoCheckMouseClick(event, mx, my))
		return false;

	if (!Active || state == OldState)
		return true;

	if (GetState()) {
		OurVessel->ActivateNavmode(NAVMode);
	}
	else {
		OurVessel->DeactivateNavmode(NAVMode) ;
	}

	return true;
}*/

bool NavModeToggle::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState,dontspring))
	{
		if (GetState())
		{
			OurVessel->ActivateNavmode(NAVMode);
		}
		else
		{
			OurVessel->ActivateNavmode(NAVMode);
		}
		return true;
	}
	return false;
}

//
// Switch row support.
//

SwitchRow::SwitchRow() {

	SwitchList = 0;
	RowList = 0;
	PanelArea = (-1);

	RowPower = 0;
}

SwitchRow::~SwitchRow() {
}

bool SwitchRow::CheckMouseClick(int id, int event, int mx, int my) {

	if (id != PanelArea)
		return false;

	PanelSwitchItem *s = SwitchList;
	while (s) {
		if (s->CheckMouseClick(event, mx, my))
			return true;
		s = s->GetNext();
	}
	return false;
}

void SwitchRow::timestep(double missionTime)
{
	PanelSwitchItem *s = SwitchList;
	while (s)
	{
		if (s->doTimeStep)
			s->timestep(missionTime);
		s = s->GetNext();
	}
}

void SwitchRow::AddSwitch(PanelSwitchItem *s)

{
	s->SetNext(SwitchList); 
	SwitchList = s;

	//
	// If we have power, wire it to the switch. Unless someone's already connected it
	// to another power source.
	//
	if (RowPower && !(s->IsWired()))
		s->WireTo(RowPower);
}

PanelSwitchItem *SwitchRow::GetItemByName(const char *n)

{
	if (!n)
		return 0;

	PanelSwitchItem *s = SwitchList;
	while (s) {
		char *nm = s->GetName();
		if (nm && !strcmp(nm, n))
		{
			return s;
		}

		s = s->GetNext();
	}

	return 0;
}

void SwitchRow::Init(int area, PanelSwitches &panel, e_object *p) {

	SwitchList = 0;
	RowList = 0;
	PanelArea = area;
	panelSwitches = &panel;
	RowPower = p;

	panel.AddRow(this);
}

bool SwitchRow::DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn) {

	if (id != PanelArea)
		return false;

	PanelSwitchItem *s = SwitchList;
	while (s) {
		s->DrawSwitch(DrawSurface);
		if (FlashOn && s->IsFlashing())
			s->DrawFlash(DrawSurface);
		s = s->GetNext();
	}
	return true;
}

bool PanelSwitchesVC::VCMouseEvent(int id, int event, VECTOR3 &p)
{
	for (unsigned i = 0;i < SwitchList.size();i++)
	{
		if (id == SwitchArea[i])
		{
			SwitchList[i]->ProcessMouseVC(event, p);
			return true;
		}
	}
	return false;
}

bool PanelSwitchesVC::VCRedrawEvent(int id, int event, SURFHANDLE surf)
{
	for (unsigned i = 0;i < SwitchList.size();i++)
	{
		if (id == SwitchArea[i])
		{
			SwitchList[i]->RedrawVC(*SwitchAnim[i]);
			return true;
		}
	}
	return false;
}

void PanelSwitchesVC::AddSwitch(PanelSwitchItem *s, int area, UINT * anim)
{
	SwitchList.push_back(s);
	SwitchArea.push_back(area);
	SwitchAnim.push_back(anim);
}

void PanelSwitchesVC::ClearSwitches()
{
	SwitchList.clear();
	SwitchArea.clear();
	SwitchAnim.clear();
}

//
// Panel of switches. This code wraps up a whole panel with multiple
// rows of switches, and passes redraw and mouse events to the appropriate
// switches.
//

bool PanelSwitches::CheckMouseClick(int id, int event, int mx, int my) {

	SwitchRow *row = RowList;

	while (row) {
		if (row->CheckMouseClick(id, event, mx, my))
			return true;
		row = row->GetNext();
	}

	return false;
}

void PanelSwitches::timestep(double missionTime)
{
	if (missionTime < (lastexecutedtime + 1))
		return;
	if (missionTime > (lastexecutedtime + 2))
		lastexecutedtime = missionTime;
	else
		lastexecutedtime += 1;
	SwitchRow *row = RowList;

	while (row) {
		row->timestep(missionTime);
		row = row->GetNext();
	}
}

bool PanelSwitches::DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn) {

	SwitchRow *row = RowList;

	while (row) {
		if (row->DrawRow(id, DrawSurface, FlashOn))
			return true;
		row = row->GetNext();
	}

	return false;
}

bool PanelSwitches::SetFlashing(const char *n, bool flash)

{
	PanelSwitchItem *p;
	SwitchRow *row = RowList;

	while (row) {
		p = row->GetItemByName(n);
		if (p)
		{
			p->SetFlashing(flash);
			return true;
		}

		row = row->GetNext();
	}

	return false;
}

bool PanelSwitches::GetFlashing(const char *n) {

	PanelSwitchItem *p;
	SwitchRow *row = RowList;

	while (row) {
		p = row->GetItemByName(n);
		if (p) {
			return p->IsFlashing();
		}
		row = row->GetNext();
	}
	return false;
}

int PanelSwitches::GetState(const char *n)

{
	PanelSwitchItem *p;
	SwitchRow *row = RowList;

	while (row) {
		p = row->GetItemByName(n);
		if (p)
		{
			return p->GetState();
		}

		row = row->GetNext();
	}

	return -1;
}

bool PanelSwitches::GetFailedState(const char *n)

{
	PanelSwitchItem *p;
	SwitchRow *row = RowList;

	while (row) {
		p = row->GetItemByName(n);
		if (p)
		{
			return p->IsFailed();
		}

		row = row->GetNext();
	}

	return false;
}

bool PanelSwitches::SetState(const char *n, int value, bool guard, bool hold)

{
	PanelSwitchItem *p;
	SwitchRow *row = RowList;

	while (row) {
		p = row->GetItemByName(n);
		if (p) {
			p->Unguard();
			p->SetHeld(hold);
			p->SetState(value);
			if (guard)
				p->Guard();			
			return true;
		}
		row = row->GetNext();
	}

	/// \todo When false is returned, the checklist controller loops infinitely, better solution?
	// return false;
	sprintf(oapiDebugString(),"Switch not found: %s", n);
	return true;

}

//
// Guarded toggle switch.
//

GuardedToggleSwitch::GuardedToggleSwitch() {
	
	guardX = 0;
	guardY = 0;
	guardWidth = 0;
	guardHeight = 0;
	guardSurface = 0;
	guardState = 0;
	guardResetsState = true;
}

GuardedToggleSwitch::~GuardedToggleSwitch() {
	guardClick.done();
}

void GuardedToggleSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, int springloaded) 
{
	ToggleSwitch::Register(scnh, n, defaultState, springloaded, 0);
	guardState = defaultGuardState;
}

void GuardedToggleSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf,
									int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;

	guardSurface = surf;
	guardBorder = bsurf;
	
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		switchRow->panelSwitches->soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedToggleSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if (guardResetsState) { 
		if (guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			DoDrawSwitch(DrawSurface);
		} else {
			DoDrawSwitch(DrawSurface);
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		}
	} else {
		if (guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			DoDrawSwitch(DrawSurface);
		} else {
			if (state) {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + 3 * guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			} else {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			}
		}
	}
}

void GuardedToggleSwitch::DrawSwitchVC(UINT anim, UINT animguard) {

	RedrawVC(anim);

	if (guardState) {
		OurVessel->SetAnimation(animguard, 1.0);
	}
	else {
		OurVessel->SetAnimation(animguard, 0.0);
	}
}


void GuardedToggleSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (!guardState && guardBorder)
		oapiBlt(DrawSurface, guardBorder, guardX, guardY, 0, 0, guardWidth, guardHeight, SURF_PREDEF_CK);
	else
		ToggleSwitch::DrawFlash(DrawSurface);
}

void GuardedToggleSwitch::Guard() {

	if (guardState) {
		guardState = 0;

		// reset by guard
		if (guardResetsState) { 
			if (Active && state) {
				SwitchTo(TOGGLESWITCH_DOWN);
			}
		}
		// reset by spring
		if (IsSpringLoaded()) {
			if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(TOGGLESWITCH_DOWN,true);
			if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(TOGGLESWITCH_UP,true);
		}

	} 
}

bool GuardedToggleSwitch::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				Guard();
			} 
			else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	}
	else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return ToggleSwitch::CheckMouseClick(event, mx, my);
		}
	}
	return false;
}

bool GuardedToggleSwitch::CheckMouseClickVC(int event, VECTOR3 &p) {

	if (event & PANEL_MOUSE_RBDOWN) {

		if (guardState) {
			Guard();
		}
		else {
			guardState = 1;
		}
		guardClick.play();
		return true;

	}
	else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return ToggleSwitch::ProcessMouseVC(event, p);
		}
	}
	return false;
}

void GuardedToggleSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i", state, guardState); 
	oapiWriteScenario_string(scn, name, buffer);
}

void GuardedToggleSwitch::LoadState(char *line) {
	
	char buffer[100];
	int st, gst;

	sscanf(line, "%s %i %i", buffer, &st, &gst); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
	}
}


//
// Guarded push switch.
//

GuardedPushSwitch::GuardedPushSwitch() {
	
	guardX = 0;
	guardY = 0;
	guardWidth = 0;
	guardHeight = 0;
	guardSurface = 0;
	guardBorder = 0;
	guardState = 0;

	lit = false;
}

void GuardedPushSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int xoffset, int yoffset, int lxoffset, int lyoffset)

{
	litOffsetX = lxoffset;
	litOffsetY = lyoffset;

	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset);
}

GuardedPushSwitch::~GuardedPushSwitch() {
	guardClick.done();
}

void GuardedPushSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState) {

	PushSwitch::Register(scnh, n, defaultState, 0);
	guardState = defaultGuardState;
}

void GuardedPushSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf,
									int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;

	guardSurface = surf;
	guardBorder = bsurf;
	
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		switchRow->panelSwitches->soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedPushSwitch::DoDrawSwitch(SURFHANDLE DrawSurface)

{
	if (lit)
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, litOffsetX, litOffsetY, width, height, SURF_PREDEF_CK);
	}
	else
		ToggleSwitch::DoDrawSwitch(DrawSurface);
}

void GuardedPushSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if(guardState)
	{
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		DoDrawSwitch(DrawSurface);
	}
	else
	{
		DoDrawSwitch(DrawSurface);
		oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
	}
}

void GuardedPushSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (!guardState && guardBorder)
		oapiBlt(DrawSurface, guardBorder, guardX, guardY, 0, 0, guardWidth, guardHeight, SURF_PREDEF_CK);
	else
		ToggleSwitch::DrawFlash(DrawSurface);
}

void GuardedPushSwitch::Guard() {
			
	if (guardState) {
		guardState = 0;
		if (Active && state) 
			SwitchTo(0,true);
	}
}

bool GuardedPushSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				Guard();
			}
			else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	}
	else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return PushSwitch::CheckMouseClick(event, mx, my);
		}
	}
	return false;
}

void GuardedPushSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i %d", state, guardState, lit ? 1 : 0); 
	oapiWriteScenario_string(scn, name, buffer);
}

void GuardedPushSwitch::LoadState(char *line) {
	
	char buffer[100];
	int st, gst, l = 0;

	//
	// Note we set l to zero before the sscanf call to allow backward
	// compatibility with old scenarios: default to unlit if the lit
	// state isn't saved in the file.
	//
	sscanf(line, "%s %i %i %d", buffer, &st, &gst, &l); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
		lit = (l != 0);
	}
}


//
// Guarded three pos switch.
//

GuardedThreePosSwitch::GuardedThreePosSwitch() {
	
	guardX = 0;
	guardY = 0;
	guardWidth = 0;
	guardHeight = 0;
	guardSurface = 0;
	guardState = 0;
}

GuardedThreePosSwitch::~GuardedThreePosSwitch() {
	guardClick.done();
}

void GuardedThreePosSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, 
									 int springloaded) 
{
	ThreePosSwitch::Register(scnh, n, defaultState, springloaded, 0);
	guardState = defaultGuardState;
}

void GuardedThreePosSwitch::InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, 
									  int xOffset, int yOffset) {
	guardX = xp;
	guardY = yp;
	guardWidth = w;
	guardHeight = h;
	guardSurface = surf;
	guardXOffset = xOffset;
	guardYOffset = yOffset;

	if (!guardClick.isValid())
		switchRow->panelSwitches->soundlib->LoadSound(guardClick, GUARD_SOUND, INTERNAL_ONLY);
}

void GuardedThreePosSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible) return;

	if (guardResetsState) { 
		if(guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			ThreePosSwitch::DrawSwitch(DrawSurface);
		} else {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
		}
	} else {
		if (guardState) {
			oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			ThreePosSwitch::DrawSwitch(DrawSurface);
		} else {
			if (state == THREEPOSSWITCH_DOWN) {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			} else if (state == THREEPOSSWITCH_CENTER) {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + 2 * guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			} else {
				oapiBlt(DrawSurface, guardSurface, guardX, guardY, guardXOffset + 3 * guardWidth, guardYOffset, guardWidth, guardHeight, SURF_PREDEF_CK);
			}
		}
	}
}

void GuardedThreePosSwitch::Guard() {

	if (guardState) {
		guardState = 0;
		// reset by guard
		if (guardResetsState) { 
			if (Active && state) {
				SwitchTo(THREEPOSSWITCH_DOWN,true);
			}
		}
		// reset by spring
		if (IsSpringLoaded()) {
			if (springLoaded == SPRINGLOADEDSWITCH_DOWN)   SwitchTo(THREEPOSSWITCH_DOWN,true);
			if (springLoaded == SPRINGLOADEDSWITCH_CENTER) SwitchTo(THREEPOSSWITCH_CENTER,true);
			if (springLoaded == SPRINGLOADEDSWITCH_UP)     SwitchTo(THREEPOSSWITCH_UP,true);

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGUP && state == THREEPOSSWITCH_UP)     
				SwitchTo(THREEPOSSWITCH_CENTER,true);

			if (springLoaded == SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN && state == THREEPOSSWITCH_DOWN)     
				SwitchTo(THREEPOSSWITCH_CENTER,true);
		}
	}
}

bool GuardedThreePosSwitch::CheckMouseClick(int event, int mx, int my) {

	if (!visible) return false;

	if (event & PANEL_MOUSE_RBDOWN) {
		if (mx >= guardX && mx <= guardX + guardWidth && 
			my >= guardY && my <= guardY + guardHeight) {			
			if (guardState) {
				Guard();
			} else {
				guardState = 1;
			}
			guardClick.play();
			return true;
		}
	} else if (event & (PANEL_MOUSE_LBDOWN | PANEL_MOUSE_LBUP)) {
		if (guardState) {
			return ThreePosSwitch::CheckMouseClick(event, mx, my);
		}
	}
	return false;
}

void GuardedThreePosSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%i %i", state, guardState); 
	oapiWriteScenario_string(scn, name, buffer);
}

void GuardedThreePosSwitch::LoadState(char *line) {
	
	char buffer[100];
	int st, gst;

	sscanf(line, "%s %i %i", buffer, &st, &gst); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		guardState = gst;
	}
}


//
// Rotational Switch
//

RotationalSwitch::RotationalSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	position = 0;
	positionList = 0;
	next = 0;
	soundEnabled = true;
	maxState = -1;
	Wraparound = false;

	switchSurface = 0;
	switchBorder = 0;
	
	switchRow = 0;

	bitmaps[0].angle = 0;
	bitmaps[0].xOffset = 0;
	bitmaps[0].yOffset = 0;

	bitmaps[1].angle = 30;
	bitmaps[1].xOffset = 1;
	bitmaps[1].yOffset = 0;

	bitmaps[2].angle = 60;
	bitmaps[2].xOffset = 2;
	bitmaps[2].yOffset = 0;

	bitmaps[3].angle = 90;
	bitmaps[3].xOffset = 3;
	bitmaps[3].yOffset = 0;

	bitmaps[4].angle = 120;
	bitmaps[4].xOffset = 4;
	bitmaps[4].yOffset = 0;

	bitmaps[5].angle = 150;
	bitmaps[5].xOffset = 5;
	bitmaps[5].yOffset = 0;

	bitmaps[6].angle = 180;
	bitmaps[6].xOffset = 6;
	bitmaps[6].yOffset = 0;

	bitmaps[7].angle = 210;
	bitmaps[7].xOffset = 7;
	bitmaps[7].yOffset = 0;

	bitmaps[8].angle = 240;
	bitmaps[8].xOffset = 8;
	bitmaps[8].yOffset = 0;

	bitmaps[9].angle = 270;
	bitmaps[9].xOffset = 9;
	bitmaps[9].yOffset = 0;

	bitmaps[10].angle = 300;
	bitmaps[10].xOffset = 10;
	bitmaps[10].yOffset = 0;

	bitmaps[11].angle = 330;
	bitmaps[11].xOffset = 11;
	bitmaps[11].yOffset = 0;

	bitmaps[12].angle = 20;
	bitmaps[12].xOffset = 0;
	bitmaps[12].yOffset = 1;

	bitmaps[13].angle = 45;
	bitmaps[13].xOffset = 1;
	bitmaps[13].yOffset = 1;

	bitmaps[14].angle = 70;
	bitmaps[14].xOffset = 2;
	bitmaps[14].yOffset = 1;

	bitmaps[15].angle = 110;
	bitmaps[15].xOffset = 3;
	bitmaps[15].yOffset = 1;

	bitmaps[16].angle = 135;
	bitmaps[16].xOffset = 4;
	bitmaps[16].yOffset = 1;

	bitmaps[17].angle = 160;
	bitmaps[17].xOffset = 5;
	bitmaps[17].yOffset = 1;

	bitmaps[18].angle = 200;
	bitmaps[18].xOffset = 6;
	bitmaps[18].yOffset = 1;

	bitmaps[19].angle = 225;
	bitmaps[19].xOffset = 7;
	bitmaps[19].yOffset = 1;

	bitmaps[20].angle = 250;
	bitmaps[20].xOffset = 8;
	bitmaps[20].yOffset = 1;

	bitmaps[21].angle = 290;
	bitmaps[21].xOffset = 9;
	bitmaps[21].yOffset = 1;

	bitmaps[22].angle = 315;
	bitmaps[22].xOffset = 10;
	bitmaps[22].yOffset = 1;

	bitmaps[23].angle = 340;
	bitmaps[23].xOffset = 11;
	bitmaps[23].yOffset = 1;
}

RotationalSwitch::~RotationalSwitch() {

	DeletePositions();
	sclick.done();
}

void RotationalSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultValue) {
	
	name = n;
	SetValue(defaultValue);
	scnh.RegisterSwitch(this);
}

void RotationalSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;

	switchSurface = surf;
	switchBorder = bsurf;
	
	row.AddSwitch(this);
	switchRow = &row;

	if (!sclick.isValid()) {
		row.panelSwitches->soundlib->LoadSound(sclick, ROTARY_SOUND);
	}

	OurVessel = switchRow->panelSwitches->vessel;
}

void RotationalSwitch::AddPosition(int value, double angle) {

	RotationalSwitchPosition *p = new RotationalSwitchPosition(value, angle);
	p->SetNext(positionList);
	positionList = p;
	maxState++;

	if (position == 0) {
		position = p;
	}
}

void RotationalSwitch::DeletePositions() {

	RotationalSwitchPosition *p1, *p = positionList; 
	while (p) {
		p1 = p->GetNext();
		delete p;
		p = p1;
	}

	position = 0;
	positionList = 0;
}

void RotationalSwitch::DrawSwitch(SURFHANDLE drawSurface) {

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
		oapiBlt(drawSurface, switchSurface, x, y, bitmaps[index].xOffset * width, bitmaps[index].yOffset * height, width, height, SURF_PREDEF_CK);
	}
}

void RotationalSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (switchBorder)
		oapiBlt(DrawSurface, switchBorder, x, y, 0, 0, width, height, SURF_PREDEF_CK);
}

bool RotationalSwitch::CheckMouseClick(int event, int mx, int my) {

	// Check whether it's actually in our switch region.
	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	int state = GetState();

	if (event == PANEL_MOUSE_LBDOWN) {
		if (state > 0) {
			SwitchTo(state - 1);
		}
		else if (state == 0 && Wraparound) {
			SwitchTo(maxState);
		}
	}
	else if (event == PANEL_MOUSE_RBDOWN) {
		if (state < maxState) {
			SwitchTo(state + 1);
		}
		else if (state == maxState && Wraparound) {
			SwitchTo(0);
		}
	}
	return true;
}

bool RotationalSwitch::SwitchTo(int newValue) {

	if (!position || (position->GetValue() != newValue)) {
		SetValue(newValue);
		if (soundEnabled) sclick.play();
		if (switchRow) {
			if (switchRow->panelSwitches->listener) 
				switchRow->panelSwitches->listener->PanelRotationalSwitchChanged(this);
		}
		if (callback)
			callback->call(this);
		return true;
	}
	return false;
}

void RotationalSwitch::SetValue(int newValue) { 
	
	RotationalSwitchPosition *p = positionList; 
	while (p) {
		if (p->GetValue() == newValue) {
			position = p;
		}
		p = p->GetNext();
	}
}

double RotationalSwitch::AngleDiff(double a1, double a2) {

	double diff = fabs(a1 - a2);
	if (diff > 180.0) diff = 360.0 - diff;
	return diff;
}

RotationalSwitch::operator int() {

	if (position) {
		return position->GetValue();
	} else {	
		return 0;
	}
}

int RotationalSwitch::GetState() {

	if (position) {
		return position->GetValue();
	} else {	
		return 0;
	}
}

void RotationalSwitch::SaveState(FILEHANDLE scn) {

	if (position) {
		oapiWriteScenario_int (scn, name, position->GetValue()); 
	}		
}

void RotationalSwitch::LoadState(char *line) {

	char buffer[100];
	int val;

	sscanf(line, "%s %i", buffer, &val); 
	if (!strnicmp(buffer, name, strlen(name))) {
		SetValue(val);
	}
}

void RotationalSwitch::SetState(int value)
{
	SwitchTo(value);
}

void RotationalSwitch::RedrawVC(UINT anim)
{
	double state = 0;

	if (position) state = position->GetAngle();

	OurVessel->SetAnimation(anim, state / 360);
}

bool RotationalSwitch::ProcessMouseVC(int event, VECTOR3 &p) {

	int state = GetState();

	if (event == PANEL_MOUSE_LBDOWN) {
		if (state > 0) {
			SwitchTo(state - 1);
		}
		else if (state == 0 && Wraparound) {
			SwitchTo(maxState);
		}
	}
	else if (event == PANEL_MOUSE_RBDOWN) {
		if (state < maxState) {
			SwitchTo(state + 1);
		}
		else if (state == maxState && Wraparound) {
			SwitchTo(0);
		}
	}
	return true;
}

RotationalSwitchPosition::RotationalSwitchPosition(int v, double a) {

	value = v;
	angle = a;
}

RotationalSwitchPosition::~RotationalSwitchPosition() {
}

PowerStateRotationalSwitch::PowerStateRotationalSwitch()

{
	int i;

	for (i = 0; i < 16; i++)
		sources[i] = 0;
}

void PowerStateRotationalSwitch::CheckPowerState()

{
	WireTo(sources[GetState()]);
}

double PowerStateRotationalSwitch::Current()

{
	e_object *e = sources[GetState()];
	if (e)
	{
		return e->Current();
	}

	return 0.0;
}

double PowerStateRotationalSwitch::Voltage()

{
	e_object *e = sources[GetState()];
	if (e)
	{
		return e->Voltage();
	}

	return 0.0;
}

/*bool PowerStateRotationalSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (RotationalSwitch::CheckMouseClick(event, mx, my)) {
		CheckPowerState();
		return true;
	}

	return false;
}*/

void PowerStateRotationalSwitch::SetSource(int num, e_object *s)

{
	if (num >= 0 && num < 16)
		sources[num] = s; 

	CheckPowerState();
}

bool PowerStateRotationalSwitch::SwitchTo(int newValue)

{
	if (RotationalSwitch::SwitchTo(newValue)) {
		CheckPowerState();
		return true;
	}

	return false;
}

void PowerStateRotationalSwitch::LoadState(char *line)

{
	RotationalSwitch::LoadState(line);
	CheckPowerState();
}

void OrdealRotationalSwitch::DrawSwitch(SURFHANDLE drawSurface) {

	RotationalSwitch::DrawSwitch(drawSurface);

	if (mouseDown) {
		RECT rt;
		char label[100];
		sprintf(label, "%d", value);

		HDC hDC = oapiGetDC(drawSurface);
		HFONT font = CreateFont(22, 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0, "Arial");
		SelectObject(hDC, font);
		SetTextColor(hDC, RGB(255, 255, 255));
		SetTextAlign(hDC, TA_CENTER);
		SetBkMode(hDC, OPAQUE);
		SetBkColor(hDC, RGB(146, 146, 146));

		if (GetState() == 0) {
			rt.left = 29 + x;
			rt.top = 24 + y;
			rt.right = 60 + x;
			rt.bottom = 55 + y;
			ExtTextOut(hDC, 44 + x, 28 + y, ETO_OPAQUE, &rt, label, strlen(label), NULL);

		}
		else if (GetState() == 1) {
			rt.left = 35 + x;
			rt.top = 30 + y;
			rt.right = 59 + x;
			rt.bottom = 52 + y;
			ExtTextOut(hDC, 49 + x, 31 + y, ETO_OPAQUE, &rt, label, strlen(label), NULL);

		}
		else if (GetState() == 2) {
			rt.left = 29 + x;
			rt.top = 29 + y;
			rt.right = 60 + x;
			rt.bottom = 59 + y;
			ExtTextOut(hDC, 44 + x, 34 + y, ETO_OPAQUE, &rt, label, strlen(label), NULL);

		}
		else if (GetState() == 3) {
			TextOut(hDC, 42 + x, 36 + y, label, strlen(label));

		}
		else if (GetState() == 4) {
			rt.left = 28 + x;
			rt.top = 30 + y;
			rt.right = 54 + x;
			rt.bottom = 60 + y;
			ExtTextOut(hDC, 37 + x, 34 + y, ETO_OPAQUE, &rt, label, strlen(label), NULL);

		}
		else if (GetState() == 5) {
			TextOut(hDC, 32 + x, 31 + y, label, strlen(label));

		}
		else if (GetState() == 6) {
			rt.left = 25 + x;
			rt.top = 24 + y;
			rt.right = 55 + x;
			rt.bottom = 54 + y;
			ExtTextOut(hDC, 39 + x, 28 + y, ETO_OPAQUE, &rt, label, strlen(label), NULL);
		}

		DeleteObject(font);
		oapiReleaseDC(drawSurface, hDC);
	}
}

bool OrdealRotationalSwitch::CheckMouseClick(int event, int mx, int my) {

	if (event & PANEL_MOUSE_LBDOWN) {
		// Check whether it's actually in our switch region.
		if (mx < x || my < y)
			return false;

		if (mx >(x + width) || my >(y + height))
			return false;

		lastX = mx;
		mouseDown = true;

	}
	else if (((event & PANEL_MOUSE_LBPRESSED) != 0) && mouseDown) {
		if (abs(mx - lastX) >= 2) {
			value += (int)((mx - lastX) / 2.);
			value = min(max(value, 10), 310);
			lastX = mx;
		}

	}
	else if (event & PANEL_MOUSE_LBUP) {
		mouseDown = false;
		return false;
	}
	SetValue((int)((value / 50.) + 0.5));
	return true;
}

void OrdealRotationalSwitch::SaveState(FILEHANDLE scn) {

	if (position) {
		oapiWriteScenario_int(scn, name, value);
	}
}

void OrdealRotationalSwitch::LoadState(char *line) {

	char buffer[100];
	int val;

	sscanf(line, "%s %i", buffer, &val);
	if (!strnicmp(buffer, name, strlen(name))) {
		value = val;
		SetValue((int)((value / 50.) + 0.5));
	}
}

//
// Thumbwheel Switch
//

ThumbwheelSwitch::ThumbwheelSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	state = 0;
	maxState = 0;
	switchSurface = 0;
	switchRow = 0;
	OurVessel = 0;
}

ThumbwheelSwitch::~ThumbwheelSwitch() {
	sclick.done();
}

void ThumbwheelSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState) {

	Register(scnh, n, defaultState, maximumState, false);
}

void ThumbwheelSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState, bool horizontal) {

	name = n;
	state = defaultState;
	maxState = maximumState;
	isHorizontal = horizontal;
	scnh.RegisterSwitch(this);
}

void ThumbwheelSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	switchSurface = surf;
	switchBorder = bsurf;
	
	row.AddSwitch(this);
	switchRow = &row;

	if (!sclick.isValid()) {
		row.panelSwitches->soundlib->LoadSound(sclick, THUMBWHEEL_SOUND);
	}

	OurVessel = switchRow->panelSwitches->vessel;
}

int ThumbwheelSwitch::GetState() {

	return state;
}

bool ThumbwheelSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	//
	// Check whether it's actually in our switch region.
	//

	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//

	if (event == PANEL_MOUSE_LBDOWN) {
		if (isHorizontal) {
			if (mx < (x + (width / 2.0))) {
				if (state < maxState) {
					SwitchTo(state + 1);
					sclick.play();
				}
			}
			else {
				if (state > 0) {
					SwitchTo(state - 1);
					sclick.play();
				}
			}
		} else {
			if (my < (y + (height / 2.0))) {
				if (state < maxState) {
					SwitchTo(state + 1);
					sclick.play();
				}
			}
			else {
				if (state > 0) {
					SwitchTo(state - 1);
					sclick.play();
				}
			}
		}
	}

	return true;
}

bool ThumbwheelSwitch::ProcessMouseVC(int event, VECTOR3 &p) {

	int OldState = state;

	if (event == PANEL_MOUSE_LBDOWN) {
		if (state < maxState) {
			SwitchTo(state + 1);
			sclick.play();
		}
	}
	else {
		if (state > 0) {
			SwitchTo(state - 1);
			sclick.play();
		}
	}

	return true;
}

bool ThumbwheelSwitch::SwitchTo(int newState) {

	if (newState >= 0 && newState <= maxState && state != newState) {
		state = newState;
		sclick.play();
		if (callback)
			callback->call(this);
		return true;
	}
	return false;
}

void ThumbwheelSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	oapiBlt(DrawSurface, switchSurface, x, y, state * width, 0, width, height, SURF_PREDEF_CK);
}

void ThumbwheelSwitch::RedrawVC(UINT anim) {

	double s = ((double)state) / ((double)maxState);
	OurVessel->SetAnimation(anim, s);
}

void ThumbwheelSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (switchBorder)
		oapiBlt(DrawSurface, switchBorder, x, y, 0, 0, width, height, SURF_PREDEF_CK);
}

void ThumbwheelSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state);
}

void ThumbwheelSwitch::LoadState(char *line) {

	char buffer[100];
	int st;

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
	}
}

void ThumbwheelSwitch::SetState(int value)
{
	SwitchTo(value);
}

ContinuousThumbwheelSwitch::ContinuousThumbwheelSwitch()
{
	numPositions = 0;
	multiplicator = 0;
	position = 0;
}

void ContinuousThumbwheelSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState, bool horizontal, int multPos)
{
	if (horizontal)
		ThumbwheelSwitch::Register(scnh, n, defaultState, maximumState, horizontal);
	else
		ThumbwheelSwitch::Register(scnh, n, defaultState, maximumState);

	multiplicator = multPos;
	numPositions = maximumState * multPos;
	position = StateToPosition(defaultState);
}

bool ContinuousThumbwheelSwitch::CheckMouseClick(int event, int mx, int my) {

	//
	// Check whether it's actually in our switch region.
	//

	if (mx < x || my < y)
		return false;

	if (mx > (x + width) || my > (y + height))
		return false;

	//
	// Yes, so now we just need to check whether it's an on or
	// off click.
	//

	if (event == PANEL_MOUSE_LBDOWN) {
		if (isHorizontal) {
			if (mx < (x + (width / 2.0))) {
				if (position + multiplicator <= numPositions) {
					SwitchTo(position + multiplicator);
					sclick.play();
				}
			}
			else {
				if (position - multiplicator >= 0) {
					SwitchTo(position - multiplicator);
					sclick.play();
				}
			}
		}
		else {
			if (my < (y + (height / 2.0))) {
				if (position + multiplicator <= numPositions) {
					SwitchTo(position + multiplicator);
					sclick.play();
				}
			}
			else {
				if (position - multiplicator >= 0) {
					SwitchTo(position - multiplicator);
					sclick.play();
				}
			}
		}
	}
	else if (event == PANEL_MOUSE_RBDOWN)
	{
		if (isHorizontal) {
			if (mx < (x + (width / 2.0))) {
				if (position < numPositions) {
					SwitchTo(position + 1);
					sclick.play();
				}
			}
			else {
				if (position > 0) {
					SwitchTo(position - 1);
					sclick.play();
				}
			}
		}
		else {
			if (my < (y + (height / 2.0))) {
				if (position < numPositions) {
					SwitchTo(position + 1);
					sclick.play();
				}
			}
			else {
				if (position > 0) {
					SwitchTo(position - 1);
					sclick.play();
				}
			}
		}
	}

	return true;
}

bool ContinuousThumbwheelSwitch::SwitchTo(int newPosition) {

	if (newPosition >= 0 && newPosition <= numPositions && position != newPosition) {
		position = newPosition;
		state = PositionToState(position);
		sclick.play();
		if (callback)
			callback->call(this);
		return true;
	}
	return false;
}

int ContinuousThumbwheelSwitch::StateToPosition(int st)
{
	return st * multiplicator;
}

int ContinuousThumbwheelSwitch::PositionToState(int pos)
{
	double temp = ((double)pos) / ((double)multiplicator);
	return (int)round(temp);
}

void ContinuousThumbwheelSwitch::LoadState(char *line) {

	char buffer[100];
	int st;

	sscanf(line, "%s %i", buffer, &st);
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
	}

	position = StateToPosition(state);
}

int ContinuousThumbwheelSwitch::GetPosition() {

	return position;
}

void ContinuousThumbwheelSwitch::SetState(int value)
{
	SwitchTo(StateToPosition(value));
}

//
// Thumbwheel which adjusts volume levels.
//

VolumeThumbwheelSwitch::VolumeThumbwheelSwitch()

{
	sl = 0;
	volume_class = -1;
}

void VolumeThumbwheelSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int vclass, SoundLib *s)

{
	ThumbwheelSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	sl = s;
	volume_class = vclass;

	if (sl) {
		sl->SetVolume(volume_class, (int) (state * (100.0 / 9.0)));
	}
}

//
// We override CheckMouseClick so we can update the volume on change.
//

bool VolumeThumbwheelSwitch::SwitchTo(int newState)

{
	bool ret = ThumbwheelSwitch::SwitchTo(newState);

	if (ret && sl) {
		sl->SetVolume(volume_class, (int) (state * (100.0 / 9.0)));
	}

	return ret;
}

//
// We override LoadState() so we can update the volume on loading.
//

void VolumeThumbwheelSwitch::LoadState(char *line)

{
	ThumbwheelSwitch::LoadState(line);

	if (sl) {
		sl->SetVolume(volume_class, (int) (state * (100.0 / 9.0)));
	}
}

//
// Indicator Switch
//

IndicatorSwitch::IndicatorSwitch() {

	state = false;
	displayState = 0.0;
	failOpen = false;
	x = 0;
	y = 0;
	width = 0;
	height = 0;
	switchSurface = 0;
	switchRow = 0;
	SRC = NULL;

}

IndicatorSwitch::~IndicatorSwitch() {
}

void IndicatorSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState) {

	name = n;
	state = defaultState;
	if (state) 
		displayState = 3.0;
	else
		displayState = 0.0;

	scnh.RegisterSwitch(this);
}

void IndicatorSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, bool failopen) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	switchSurface = surf;
	failOpen = failopen;
	
	row.AddSwitch(this);
	switchRow = &row;
}

bool IndicatorSwitch::CheckMouseClick(int event, int mx, int my) {

	return false;
}

void IndicatorSwitch::DrawSwitch(SURFHANDLE drawSurface) {

	int drawState=0;
	if (switchRow) {
		if (switchRow->panelSwitches->listener) 
			switchRow->panelSwitches->listener->PanelIndicatorSwitchStateRequested(this);
	}
	if (callback)
		callback->call(this);

	// Require power if wired
	if (SRC != NULL) {
		if (SRC->Voltage() > SP_MIN_DCVOLTAGE) {
			drawState = GetState();
		} else {
			drawState = (failOpen ? 1 : 0);
		}
	} else {
		drawState = GetState();
	}

	if (drawState && displayState < 3.0)
		displayState += oapiGetSimStep() * 4.0;

	if (!drawState && displayState > 0.0) 
		displayState -= oapiGetSimStep() * 4.0;

	if (displayState > 3.0) displayState = 3.0;
	if (displayState < 0.0) displayState = 0.0;

	// Cheating beyond normal saves switch subclasses and associated etcetera
	if (displayState == 3.0 && drawState > 1) {
		displayState += (drawState - 1);
	}

	oapiBlt(drawSurface, switchSurface, x, y, width * (int) displayState, 0, width, height);
}

void IndicatorSwitch::DrawSwitchVC(SURFHANDLE drawSurface, SURFHANDLE switchsurfacevc) {

	int drawState = 0;

	// Require power if wired
	if (SRC != NULL) {
		if (SRC->Voltage() > SP_MIN_DCVOLTAGE) {
			drawState = GetState();
		}
		else {
			drawState = (failOpen ? 1 : 0);
		}
	}
	else {
		drawState = GetState();
	}

	if (drawState && displayState < 3.0)
		displayState += oapiGetSimStep() * 4.0;

	if (!drawState && displayState > 0.0)
		displayState -= oapiGetSimStep() * 4.0;

	if (displayState > 3.0) displayState = 3.0;
	if (displayState < 0.0) displayState = 0.0;

	// Cheating beyond normal saves switch subclasses and associated etcetera
	if (displayState == 3.0 && drawState > 1) {
		displayState += (drawState - 1);
	}

	oapiBlt(drawSurface, switchsurfacevc, x, y, width * (int)displayState, 0, width, height);
}

void IndicatorSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state); 
}

void IndicatorSwitch::LoadState(char *line) {

	char buffer[100];
	int st = FALSE; // Avoids crash bug

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
		if (state != 0){
			displayState = 3.0;
		}else{
			displayState = 0.0;
		}
		// Cheating beyond normal saves switch subclasses and associated etcetera
		if (displayState == 3.0 && state > 1){
			displayState += (state - 1);
		}
	}
}


//
// Meter Switch
//

MeterSwitch::MeterSwitch() {

	value = 0;
	displayValue = 0;
	minValue = 0;
	maxValue = 0;
	switchRow = 0;
	lastDrawTime = -1;
}

MeterSwitch::~MeterSwitch() {
}

void MeterSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time) {

	Register(scnh, n, min, max, time, min);
}

void MeterSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time, double defaultValue) {

	name = n;
	value = defaultValue;
	displayValue = defaultValue;
	minValue = min;
	maxValue = max;
	minMaxTime = time;
	scnh.RegisterSwitch(this);
}

void MeterSwitch::Init(SwitchRow &row) {

	row.AddSwitch(this);
	switchRow = &row;
}

bool MeterSwitch::CheckMouseClick(int event, int mx, int my) {

	return false;
}

void MeterSwitch::DrawSwitch(SURFHANDLE drawSurface) {

	DoDrawSwitch(GetDisplayValue(), drawSurface);
}

double MeterSwitch::GetDisplayValue() {

	// Prevent problems if the simulation is started paused and 
	// a lot of values are zero in that case
	if (oapiGetPause())
		return displayValue;

	if (lastDrawTime == -1) {
		lastDrawTime = oapiGetSysTime(); // oapiGetSimTime();
		return displayValue;
	}

	value = AdjustForPower(QueryValue());
	if (value > maxValue) value = maxValue;
	if (value < minValue) value = minValue;

	if (minMaxTime == 0) {
		displayValue = value;
	} else {
		double dt = oapiGetSysTime() - lastDrawTime; // oapiGetSimTime() - lastDrawTime;
		if (dt > 0) {
			if (fabs(value - displayValue) / dt > (maxValue - minValue) / minMaxTime) {
				displayValue += ((value - displayValue) / fabs(value - displayValue)) * (maxValue - minValue) / minMaxTime * dt;
			} else {
				displayValue = value;
			}
		}
	}
	lastDrawTime = oapiGetSysTime(); // oapiGetSimTime();
	return displayValue;
}

//
// Adjust the input value based on the voltage fed to the meter.
// CWS limit for main bus undervolt is 26.25 V, so we begin at 26 V 
// with the adjustment
//

double MeterSwitch::AdjustForPower(double val)

{
	double volts = Voltage();

	if (volts >= 26.0)
		return val;

	if (volts == 0)      // Hack to make meters with minimums below zero show properly
		return minValue; // when the power is out. DS20100509

	// sprintf(oapiDebugString(), "MeterSwitch::AdjustForPower %s undervolt", name);
	return ((val * (volts - 1.0)) / 25.0);
}

void MeterSwitch::SaveState(FILEHANDLE scn) {

	char buffer[100];

	sprintf(buffer, "%lf %lf", value, displayValue); 
	oapiWriteScenario_string(scn, name, buffer);
}

void MeterSwitch::LoadState(char *line) {

	char buffer[100];
	double v, dv;

	sscanf(line, "%s %lf %lf", buffer, &v, &dv); 
	if (!strnicmp(buffer, name, strlen(name))) {
		value = v;
		displayValue = dv;
	}
}

void RoundMeter::Init(HPEN p0, HPEN p1, SwitchRow &row)

{
	MeterSwitch::Init(row);
	Pen0 = p0;
	Pen1 = p1;
}

void RoundMeter::DrawNeedle (SURFHANDLE surf, int x, int y, double rad, double angle)

{
	// Needle function by Rob Conley from Mercury code
	
	double dx = rad * cos(angle), dy = rad * sin(angle);
	HGDIOBJ oldObj;

	HDC hDC = oapiGetDC (surf);
	oldObj = SelectObject (hDC, Pen1);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(0.85*dx+0.5), y - (int)(0.85*dy+0.5));
	SelectObject (hDC, oldObj);
	oldObj = SelectObject (hDC, Pen0);
	MoveToEx (hDC, x, y, 0); LineTo (hDC, x + (int)(dx+0.5), y - (int)(dy+0.5));
	SelectObject (hDC, oldObj);
	oapiReleaseDC (surf, hDC);
}

ElectricMeter::ElectricMeter(double minVal, double maxVal, double vMin, double vMax)

{
	minValue = minVal;
	maxValue = maxVal;
	minAngle = vMin;
	maxAngle = vMax;

	xSize = 99;
	ySize = 98;

	ScaleFactor = (vMax - vMin) / (maxValue - minValue);
}

void ElectricMeter::SetSurface(SURFHANDLE srf, int x, int y)

{
	xSize = x;
	ySize = y;
	FrameSurface = srf;
}

void ElectricMeter::Init(HPEN p0, HPEN p1, SwitchRow &row, e_object *dcindicatorswitch)

{
	RoundMeter::Init(p0, p1, row);
	WireTo(dcindicatorswitch);
}

void ElectricMeter::DoDrawSwitch(double volts, SURFHANDLE drawSurface)

{
	double v = minAngle + (ScaleFactor * (volts - minValue));
	DrawNeedle(drawSurface, xSize / 2, ySize / 2, 25.0, v * RAD);
	oapiBlt(drawSurface, FrameSurface, 0, 0, 0, 0, xSize, ySize, SURF_PREDEF_CK);
}

DCVoltMeter::DCVoltMeter(double minVal, double maxVal, double vMin, double vMax) :
	ElectricMeter(minVal, maxVal, vMin, vMax)

{
}

double DCVoltMeter::QueryValue()

{
	return Voltage();
}

ACVoltMeter::ACVoltMeter(double minVal, double maxVal, double vMin, double vMax) :
	ElectricMeter(minVal, maxVal, vMin, vMax)

{
}

double ACVoltMeter::QueryValue()

{
	return Voltage();
}

DCAmpMeter::DCAmpMeter(double minVal, double maxVal, double vMin, double vMax) :
	ElectricMeter(minVal, maxVal, vMin, vMax)

{
}

double DCAmpMeter::QueryValue()

{
	if (SRC)
	{
		return SRC->Current();
	}

	return 0.0;
}

//
// Panel Switch Scenario Handler
//

void PanelSwitchScenarioHandler::RegisterSwitch(PanelSwitchItem *s) {

	s->SetNextForScenario(switchList); 
	switchList = s; 
}

void PanelSwitchScenarioHandler::SaveState(FILEHANDLE scn) {

	oapiWriteLine(scn, PANELSWITCH_START_STRING);

 	PanelSwitchItem *s = switchList;
	while (s) {
		s->SaveState(scn);
		s = s->GetNextForScenario();
	}

	oapiWriteLine(scn, PANELSWITCH_END_STRING);
}

void PanelSwitchScenarioHandler::LoadState(FILEHANDLE scn) {

	char * line;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, PANELSWITCH_END_STRING, strlen(PANELSWITCH_END_STRING)))
			return;

		PanelSwitchItem *s = switchList;
		while (s) {
			s->LoadState(line);
			s = s->GetNextForScenario();
		}
	}
}

PanelSwitchItem* PanelSwitchScenarioHandler::GetSwitch(char *name) {

	PanelSwitchItem *s = switchList;
	while (s) {
		if (!stricmp(s->GetName(), name)) 
			return s;
		s = s->GetNextForScenario();
	}
	return 0;
}


void MissionTimerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, MissionTimer *ptimer)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	timer = ptimer;
}

//
// ThreeSourceSwitch allows you to connect the output to one of three inputs based on the position
// of the switch.
//

void ThreeSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
}

void ThreeSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source[0] = s1;
	source[1] = s2;
	source[2] = s3;

	UpdateSourceState();
}

/*bool ThreeSourceSwitch::CheckMouseClick(int event, int mx, int my)
{
	if (ThreePosSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}*/

bool ThreeSourceSwitch::SwitchTo(int newState, bool dontspring)

{
	if (ThreePosSwitch::SwitchTo(newState,dontspring)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

void ThreeSourceSwitch::UpdateSourceState()

{
	if (IsUp()) {
		WireTo(source[0]);
	}
	else if (IsCenter()) {
		WireTo(source[1]);
	}
	else if (IsDown()) {
		WireTo(source[2]);
	}
}

void ThreeSourceSwitch::LoadState(char *line)

{
	ThreePosSwitch::LoadState(line);
	UpdateSourceState();
}

void ThreeSourceSwitch::SetSource(int i, e_object *s)
{
	source[i] = s;

	UpdateSourceState();
}

/*void ThreeSourceSwitch::SetState(int value)
{
	SwitchTo(value);
}*/


//
// TwoSourceSwitch allows you to connect the output to one of two inputs based on the position
// of the switch.
//

void TwoSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source1 = s1;
	source2 = s2;

	UpdateSourceState();
}

/*bool TwoSourceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}*/

bool TwoSourceSwitch::SwitchTo(int newState, bool dontspring)

{
	if (ToggleSwitch::SwitchTo(newState,dontspring)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

void TwoSourceSwitch::UpdateSourceState()

{
	if (IsUp()) {
		WireTo(source1);
	}
	else if (IsDown()) {
		WireTo(source2);
	}
}

void TwoSourceSwitch::LoadState(char *line)

{
	ToggleSwitch::LoadState(line);
	UpdateSourceState();
}

/*void TwoSourceSwitch::SetState(int value)
{
	SwitchTo(value);
}*/

//
// GuardedTwoSourceSwitch allows you to connect the output to one of two inputs based on the position
// of the switch.
//

void GuardedTwoSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2)

{
	GuardedToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source1 = s1;
	source2 = s2;

	UpdateSourceState();
}

/*bool GuardedTwoSourceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (GuardedToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}*/

bool GuardedTwoSourceSwitch::SwitchTo(int newState, bool dontspring)

{
	if (GuardedToggleSwitch::SwitchTo(newState, dontspring)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

void GuardedTwoSourceSwitch::UpdateSourceState()

{
	if (IsUp()) {
		WireTo(source1);
	}
	else if (IsDown()) {
		WireTo(source2);
	}
}

void GuardedTwoSourceSwitch::LoadState(char *line)

{
	GuardedToggleSwitch::LoadState(line);
	UpdateSourceState();
}
//
// TwoOutputSwitch allows you to connect one of the two outputs to the input based on the position
// of the switch.
//

void TwoOutputSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	output1 = o1;
	output2 = o2;

	UpdateSourceState();
}

/*bool TwoOutputSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}*/

bool TwoOutputSwitch::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState,dontspring))
	{
		UpdateSourceState();
		return true;
	}
	return false;
}

void TwoOutputSwitch::UpdateSourceState()

{
	if (IsUp()) {
		if (output1)
			output1->WireTo(this);
		if (output2)
			output2->WireTo(0);
	}
	else if (IsDown()) {
		if (output1)
			output1->WireTo(0);
		if (output2)
			output2->WireTo(this);
	}
}

void TwoOutputSwitch::LoadState(char *line)

{
	ToggleSwitch::LoadState(line);
	UpdateSourceState();
}


NSourceDestSwitch::NSourceDestSwitch(int n)
{
	nSources = n;
	sources = new e_object *[nSources];
	buses = new DCbus *[nSources];

	int i;

	for (i = 0; i < nSources; i++)
	{
		sources[i] = 0;
		buses[i] = 0;
	}
}

NSourceDestSwitch::~NSourceDestSwitch()
{
	if (sources)
	{
		delete[] sources;
		sources = 0;
	}
	if (buses)
	{
		delete[] buses;
		buses = 0;
	}
}

void NSourceDestSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row)
{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);

	UpdateSourceState();
}

bool NSourceDestSwitch::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState, dontspring))
	{
		UpdateSourceState();
		return true;
	}
	return false;
}

void NSourceDestSwitch::UpdateSourceState()
{
	if (IsUp()) {
		for (int i = 0;i < nSources;i++)
		{
			buses[i]->WireTo(sources[i]);
		}
	}
	else if (IsDown()) {
		for (int i = 0;i < nSources;i++)
		{
			buses[i]->Disconnect();
		}
	}
}

void NSourceDestSwitch::WireSourcesToBuses(int bus, e_object* i, DCbus* o)
{
	if (bus > 0 && bus <= nSources)
	{
		sources[bus - 1] = i;
		buses[bus - 1] = o;
	}
}

void NSourceDestSwitch::LoadState(char *line)
{
	ToggleSwitch::LoadState(line);
	UpdateSourceState();
}

//
// ThreeOutputSwitch allows you to connect one of the three outputs to the input based on the position
// of the switch.
//

void ThreeOutputSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2, e_object *o3)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	output1 = o1;
	output2 = o2;
	output3 = o3;

	UpdateSourceState();
}

/*bool ThreeOutputSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ThreePosSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState();
		return true;
	}

	return false;
}*/

void ThreeOutputSwitch::UpdateSourceState()

{
	if (IsUp()) {
		if (output1) output1->WireTo(this);
		if (output2) output2->WireTo(0);
		if (output3) output3->WireTo(0);

	} else if (IsCenter()) {
		if (output1) output1->WireTo(0);
		if (output2) output2->WireTo(this);
		if (output3) output3->WireTo(0);

	} else if (IsDown()) {
		if (output1) output1->WireTo(0);
		if (output2) output2->WireTo(0);
		if (output3) output3->WireTo(this);
	}
}

void ThreeOutputSwitch::LoadState(char *line)

{
	ThreePosSwitch::LoadState(line);
	UpdateSourceState();
}

bool ThreeOutputSwitch::SwitchTo(int newState, bool dontspring)

{
	if (ThreePosSwitch::SwitchTo(newState, dontspring)) {
		UpdateSourceState();
		return true;
	}
	return false;
}

//
// GuardedTwoOutputSwitch allows you to connect one of the two outputs to the input based on the position
// of the switch.
//

void GuardedTwoOutputSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2)

{
	GuardedToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	output1 = o1;
	output2 = o2;

	UpdateSourceState(GetState());
}

/*bool GuardedTwoOutputSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (GuardedToggleSwitch::CheckMouseClick(event, mx, my))
	{
		UpdateSourceState(GetState());
		return true;
	}

	return false;
}*/

bool GuardedTwoOutputSwitch::SwitchTo(int newState, bool dontspring)

{
	if (GuardedToggleSwitch::SwitchTo(newState,dontspring))
	{
		// some of these switches are spring-loaded, 
		// so we have to use newState here
		UpdateSourceState(newState);
		return true;
	}

	return false;
}

void GuardedTwoOutputSwitch::UpdateSourceState(int newState)

{
	if (newState == TOGGLESWITCH_UP) {
		if (output1)
			output1->WireTo(this);
		if (output2)
			output2->WireTo(0);
	}
	else if (newState == TOGGLESWITCH_DOWN) {
		if (output1)
			output1->WireTo(0);
		if (output2)
			output2->WireTo(this);
	}
}

void GuardedTwoOutputSwitch::LoadState(char *line)

{
	GuardedToggleSwitch::LoadState(line);
	UpdateSourceState(GetState());
}

void GuardedTwoOutputSwitch::SetState(int value)
{
	SwitchTo(value);
}

TimerUpdateSwitch::TimerUpdateSwitch()

{
	adjust_type = TIME_UPDATE_SECONDS;
	springLoaded = SPRINGLOADEDSWITCH_CENTER;
}

void ThreeSourceTwoDestSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3, e_object *d1, e_object *d2)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	source[0] = s1;
	source[1] = s2;
	source[2] = s3;
	dest1 = d1;
	dest2 = d2;

	UpdateSourceState();
}

void ThreeSourceTwoDestSwitch::UpdateSourceState()

{
	if (IsUp()) {
		//
		// Source 1 to dest 1, source 3 to dest 2
		//
	}
	else if (IsCenter()) {
		//
		// Disconnect.
		//
	}
	else if (IsDown()) {
		//
		// Source 2 to dest 2, source 3 to dest 1.
		//
	}
}

void TimerUpdateSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int adjuster, MissionTimer *ptimer)

{
	MissionTimerSwitch::Init(xp, yp, w, h, surf, bsurf, row, ptimer);
	adjust_type = adjuster;
}

void TimerUpdateSwitch::AdjustTime(int val)

{
	if (timer) {
		switch (adjust_type) {
		case TIME_UPDATE_HOURS:
			timer->UpdateHours(val);
			break;

		case TIME_UPDATE_MINUTES:
			timer->UpdateMinutes(val);
			break;

		case TIME_UPDATE_SECONDS:
			timer->UpdateSeconds(val);
			break;
		}
	}
}

/*bool TimerControlSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (MissionTimerSwitch::CheckMouseClick(event, mx, my))
	{
		SetTimer();
		return true;
	}

	return false;
}*/

bool TimerControlSwitch::SwitchTo(int newState, bool dontspring) 

{
	if (MissionTimerSwitch::SwitchTo(newState,dontspring)) {
		SetTimer();
		return true;
	}
	return false;
}

void TimerControlSwitch::SetTimer() 

{
	if (timer) {
		if (IsUp()) {
			timer->SetRunning(true);
		}
		else if (IsCenter()) {
			timer->SetRunning(false);
		}
		else if (IsDown()) {
			timer->SetRunning(false);
			timer->Reset();
		}
	}
}


//
// Event timer start/stop switch. Although it's a three-position switch, the center position does
// nothing.
//

bool EventTimerControlSwitch::SwitchTo(int newState, bool dontspring)
{
	if (MissionTimerSwitch::SwitchTo(newState,dontspring))
	{
		if (timer) {
			if (IsUp()) {
				timer->SetRunning(true);
			}
			else if (IsDown()) {
				timer->SetRunning(false);
			}
		}
		return true;
	}
	return false;
}

//
// Event timer up/down/reset switch.
//

bool EventTimerResetSwitch::SwitchTo(int newState, bool dontspring)
{
	if (MissionTimerSwitch::SwitchTo(newState,dontspring))
	{
		if (timer) {
			if (IsUp()) {
				timer->SetCountUp(TIMER_COUNT_NONE);
				timer->Reset();
				timer->SetRunning(false);
			}
			else if (IsCenter()) {
				timer->SetCountUp(TIMER_COUNT_UP);
			}
			else if (IsDown()) {
				timer->SetCountUp(TIMER_COUNT_DOWN);
			}
		}
		return true;
	}
	return false;
}

//
// Generic switch to update the timer time. AdjustTime() knows whether to update the hours, minutes or seconds.
//

/*bool TimerUpdateSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (MissionTimerSwitch::CheckMouseClick(event, mx, my))
	{
		//
		// We need to increase by one if the switch is up, and ten if it's down.
		//
		if (IsUp()) {
			AdjustTime(10);
		}
		else if (IsDown()) {
			AdjustTime(1);
		}
		return true;
	}
	else return false;
}*/

bool TimerUpdateSwitch::SwitchTo(int newstate, bool dontspring)
{
	if (MissionTimerSwitch::SwitchTo(newstate,dontspring))
	{
		//
		// We need to increase by one if the switch is up, and ten if it's down.
		//
		if (IsUp())
		{
			AdjustTime(10);
		}
		else if (IsDown())
		{
			AdjustTime(1);
		}
		return true;
	}
	return false;
}

IMUCageSwitch::IMUCageSwitch()

{
	imu = 0;
}

void IMUCageSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im)

{
	GuardedToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	imu = im;
}

/*bool IMUCageSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (GuardedToggleSwitch::CheckMouseClick(event, mx, my))
	{
		SetIMU();
		return true;
	}
	else return false;
}*/

bool IMUCageSwitch::SwitchTo(int newState, bool dontspring)

{
	if (GuardedToggleSwitch::SwitchTo(newState, dontspring))
	{
		SetIMU();
		return true;
	}
	else return false;
}

void IMUCageSwitch::SetIMU() 

{
	//
	// Cage the IMU if it's up, release if it's down. For now we'll also turn it
	// on if the switch is down.
	//
	if (imu) {
		if (IsUp()) {
			imu->SetCaged(true);
		}
		else if (IsDown()) {
			imu->SetCaged(false);
			imu->TurnOn();
		}
	}
}

//
// Unfortunately we need to duplicate this code for the LEM switch which doesn't have a guard.
//

UnguardedIMUCageSwitch::UnguardedIMUCageSwitch()

{
	imu = 0;
}

void UnguardedIMUCageSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	imu = im;
}

bool UnguardedIMUCageSwitch::SwitchTo(int newState, bool dontspring)

{
	if (ToggleSwitch::SwitchTo(newState,dontspring))
	{
		SetIMU();
		return true;
	}
	else return false;
}

void UnguardedIMUCageSwitch::SetIMU() 

{
	//
	// Cage the IMU if it's up, release if it's down. For now we'll also turn it
	// on if the switch is down.
	//
	if (imu) {
		if (IsUp()) {
			imu->SetCaged(true);
		}
		else if (IsDown()) {
			imu->SetCaged(false);
			imu->TurnOn();
		}
	}
}

//
// Enable light test on caution and warning system.
//

/*bool CWSLightTestSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (CWSThreePosSwitch::CheckMouseClick(event, mx,my)) {
		//
		// Up lights left-hand lights, down lights right-hand lights.
		//

		if (cws) {
			if (IsUp()) {
				cws->LightTest(CWS_TEST_LIGHTS_LEFT);
			}
			else if (IsDown()) {
				cws->LightTest(CWS_TEST_LIGHTS_RIGHT);
			}
			else if (IsCenter()) {
				cws->LightTest(CWS_TEST_LIGHTS_NONE);
			}
		}
		return true;
	}

	return false;
}*/

bool CWSLightTestSwitch::SwitchTo(int newState, bool dontspring)
{
	if (CWSThreePosSwitch::SwitchTo(newState, dontspring))
	{
		if (cws)
		{
			if (IsUp())
			{
				cws->LightTest(CWS_TEST_LIGHTS_LEFT);
			}
			else if (IsDown())
			{
				cws->LightTest(CWS_TEST_LIGHTS_RIGHT);
			}
			else if (IsCenter())
			{
				cws->LightTest(CWS_TEST_LIGHTS_NONE);
			}
		}
		return true;
	}
	return false;
}
//
// Set caution and warning mode state.
//

/*bool CWSModeSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (CWSThreePosSwitch::CheckMouseClick(event, mx,my)) {
		if (cws) {
			if (IsUp()) {
				cws->SetMode(CWS_MODE_NORMAL);
			}
			else if (IsCenter()) {
				cws->SetMode(CWS_MODE_BOOST);
			}
			else if (IsDown()) {
				cws->SetMode(CWS_MODE_ACK);
			}
		}
		return true;
	}

	return false;
} */

bool CWSModeSwitch::SwitchTo(int newState, bool dontspring) 

{
	if (CWSThreePosSwitch::SwitchTo(newState, dontspring)) {
		if (cws) {
			if (IsUp()) {
				cws->SetMode(CWS_MODE_NORMAL);
			}
			else if (IsCenter()) {
				cws->SetMode(CWS_MODE_BOOST);
			}
			else if (IsDown()) {
				cws->SetMode(CWS_MODE_ACK);
			}
		}
		return true;
	}
	return false;
}


//
// Set caution and warning power source.
//

/*bool CWSPowerSwitch::CheckMouseClick(int event, int mx, int my)

{
	int OldState = state;

	if (CWSThreePosSwitch::CheckMouseClick(event, mx, my)) {
		if (Active && (state != OldState)) {
			SetPowerBus();
		}
		return true;
	}
	return false;
}*/

bool CWSPowerSwitch::SwitchTo(int newState, bool dontspring) 

{
	if (CWSThreePosSwitch::SwitchTo(newState, dontspring)) {
		SetPowerBus();
		return true;
	}
	return false;
}

void CWSPowerSwitch::SetPowerBus() 

{
	if (cws) {
		if (IsUp()) {
			cws->SetPowerBus(CWS_POWER_SUPPLY_1);
		}
		else if (IsCenter()) {
			cws->SetPowerBus(CWS_POWER_NONE);
		}
		else if (IsDown()) {
			cws->SetPowerBus(CWS_POWER_SUPPLY_2);
		}
	}
}

/*void CWSPowerSwitch::SetState(int value)
{
	SwitchTo(value);
}*/

void CWSThreePosSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	cws = c;
}

//
// Set caution and warning source (e.g. CSM/LEM or CM)
//

/*bool CWSSourceSwitch::CheckMouseClick(int event, int mx, int my)

{
	if (ToggleSwitch::CheckMouseClick(event, mx, my)) {
		if (cws) {
			if (IsUp()) {
				cws->SetSource(CWS_SOURCE_CSM);
			}
			else if (IsDown()) {
				cws->SetSource(CWS_SOURCE_CM);
			}
		}
		return true;
	}

	return false;
}*/

bool CWSSourceSwitch::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState, dontspring))
	{
		if (cws)
		{
			if (IsUp())
			{
				cws->SetSource(CWS_SOURCE_CSM);
			}
			else if (IsDown())
			{
				cws->SetSource(CWS_SOURCE_CM);
			}
		}
		return true;
	}
	return false;
}

//
// Switch that controls AGC input channels.
//

bool AGCIOSwitch::SwitchTo(int newState, bool dontspring)
{
	if (ToggleSwitch::SwitchTo(newState,dontspring)) {
		if (agc) {
			if (IsUp()) {
				agc->SetInputChannelBit(Channel, Bit, UpValue);
			}
			else if (IsDown()) {
				agc->SetInputChannelBit(Channel, Bit, !UpValue);
			}
		}
		return true;
	}
	return false;
}

//
// CMC Hold/Free/Auto mode.
//

bool CMCModeHoldFreeSwitch::SwitchTo(int newState, bool dontspring)
{
	if (AGCThreePoswitch::SwitchTo(newState,dontspring)) {
		if (agc) {
			bool Hold = false;
			bool Free = false;

			if (IsCenter()) {
				Hold = true;
			}
			else if (IsDown()) {
				Free = true;
			}

			agc->SetInputChannelBit(031, HoldFunction, Hold);
			agc->SetInputChannelBit(031, FreeFunction, Free);
		}
		return true;
	}

	return false;
}

//
// CMC Optics Mode Switch
//

void CMCOpticsZeroSwitch::DoDrawSwitch(SURFHANDLE DrawSurface)
{
	if (IsUp())
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset, yOffset, width, height, SURF_PREDEF_CK);
	}
	else
	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset - width, yOffset, width, height, SURF_PREDEF_CK);
	}
}

//
// LEM PGNS switch.
//

bool PGNSSwitch::SwitchTo(int newState, bool dontspring)
{
	if (AGCThreePoswitch::SwitchTo(newState,dontspring)) {
		if (agc) {
			bool Hold = false;
			bool Auto = false;

			if (IsCenter()) {
				Hold = true;
			}
			else if (IsUp()) {
				Auto = true;
			}

			agc->SetInputChannelBit(031, HoldFunction, Hold);
			agc->SetInputChannelBit(031, FreeFunction, Auto);
		}
		return true;
	}

	return false;
}

bool ModeSelectSwitch::SwitchTo(int newState, bool dontspring)
{
	if (AGCThreePoswitch::SwitchTo(newState, dontspring)) {
		if (agc) {
			bool PGNS = false;

			if (IsCenter()) {
				PGNS = true;
			}

			//Actually Display Inertial Data
			agc->SetInputChannelBit(030, GuidanceReferenceRelease, PGNS);
		}
		return true;
	}

	return false;
}

//
// If we add more caution and warning system switches which use toggle-switch, they could be derived from a new
// class which has the generic init function to set the cws.
//

void CWSSourceSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	cws = c;
}


void AGCSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c, int xoffset, int yoffset)

{
	ToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row, xoffset, yoffset);
	agc = c;
}

void AGCThreePoswitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c)

{
	ThreePosSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	agc = c;
}

void AGCGuardedToggleSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c)

{
	GuardedToggleSwitch::Init(xp, yp, w, h, surf, bsurf, row);
	agc = c;
}

bool AGCIOGuardedToggleSwitch::SwitchTo(int newState, bool dontspring)
{
	if (AGCGuardedToggleSwitch::SwitchTo(newState, dontspring)) {
		if (agc) {
			if (IsUp()) {
				agc->SetInputChannelBit(Channel, Bit, UpValue);
			}
			else if (IsDown()) {
				agc->SetInputChannelBit(Channel, Bit, !UpValue);
			}
		}
		return true;
	}
	return false;
}


//
// Handcontroller Switch
//

HandcontrollerSwitch::HandcontrollerSwitch() {

	x = 0;
	y = 0;
	width = 0;
	height = 0;
	state = 0;
	switchSurface = 0;
	borderSurface = 0;
	switchRow = 0;
}

HandcontrollerSwitch::~HandcontrollerSwitch() {
	sclick.done();
}

void HandcontrollerSwitch::Register(PanelSwitchScenarioHandler &scnh, char *n, bool hasyawaxis) {

	name = n;
	hasYawAxis =hasyawaxis;
	scnh.RegisterSwitch(this);
}

void HandcontrollerSwitch::Init(int xp, int yp, int w, int h, SURFHANDLE surf, 	SURFHANDLE bsurf, SwitchRow &row) {

	x = xp;
	y = yp;
	width = w;
	height = h;
	switchSurface = surf;
	borderSurface = bsurf;
	
	row.AddSwitch(this);
	switchRow = &row;

	if (!sclick.isValid()) {
		row.panelSwitches->soundlib->LoadSound(sclick, THUMBWHEEL_SOUND);
	}
}

int HandcontrollerSwitch::GetState() {

	return state;
}

bool HandcontrollerSwitch::CheckMouseClick(int event, int mx, int my) {

	int OldState = state;

	if (event == PANEL_MOUSE_LBDOWN) {
		//
		// Check whether it's actually in our switch region.
		//

		if (mx < x || my < y)
			return false;

		if (mx > (x + width) || my > (y + height))
			return false;

		//
		// Yes, so now we just need to check whether it's an on or
		// off click.
		//

		if (mx < (x + (width / 3.))) {
			if (my < (y + (height * 2. / 3.))) {
				state = 2;
				sclick.play();
			} else if (hasYawAxis) {
				state = 5;
				sclick.play();
			}
		} else if (mx > (x + (width * 2. / 3.))) {
			if (my < (y + (height * 2. / 3.))) {
				state = 1;
				sclick.play();
			} else if (hasYawAxis) {
				state = 6;
				sclick.play();
			}
		} else {
			if (my < (y + (height / 2.))) {
				state = 4;
				sclick.play();
			} else {
				state = 3;
				sclick.play();
			}
		}
	} else if (event == PANEL_MOUSE_LBUP) {
		state = 0;
		sclick.play();
	}
	return true;
}

void HandcontrollerSwitch::DrawSwitch(SURFHANDLE DrawSurface) {

	if (!visible)
		return;

	oapiBlt(DrawSurface, switchSurface, x, y, state * width, 0, width, height, SURF_PREDEF_CK);
}

void HandcontrollerSwitch::DrawFlash(SURFHANDLE DrawSurface)

{
	if (!visible)
		return;

	if (borderSurface)
		oapiBlt(DrawSurface, borderSurface, x, y, 0, 0, width, height, SURF_PREDEF_CK);
}

void HandcontrollerSwitch::SaveState(FILEHANDLE scn) {

	oapiWriteScenario_int (scn, name, state);
}

void HandcontrollerSwitch::LoadState(char *line) {

	char buffer[100];
	int st;

	sscanf(line, "%s %i", buffer, &st); 
	if (!strnicmp(buffer, name, strlen(name))) {
		state = st;
	}
}

//
// Panel interface connector. This is here as it's primarily concerned
// with handling panel calls.
//

PanelConnector::PanelConnector(PanelSwitches &p, ChecklistController &c) : panel(p), checklist(c)

{
	type = MFD_PANEL_INTERFACE;
}

PanelConnector::~PanelConnector()

{
}

bool PanelConnector::ReceiveMessage(Connector *from, ConnectorMessage &m)

{
	//
	// Sanity check.
	//

	if (m.destination != type)
	{
		return false;
	}

	PanelConnectorMessageType messageType;

	messageType = (PanelConnectorMessageType) m.messageType;

	switch (messageType)
	{
	case MFD_PANEL_FLASH_ITEM:
		m.val1.bValue = panel.SetFlashing(static_cast<char *>(m.val1.pValue), m.val2.bValue);
		return true;

	case MFD_PANEL_GET_ITEM_STATE:
		m.val1.iValue = panel.GetState(static_cast<char *>(m.val1.pValue));
		return true;

	case MFD_PANEL_GET_ITEM_FLASHING:
		m.val1.bValue = panel.GetFlashing(static_cast<char *>(m.val1.pValue));
		return true;

	case MFD_PANEL_SET_ITEM_STATE:
		m.val1.bValue = panel.SetState(static_cast<char *>(m.val1.pValue), m.val2.iValue, m.val3.bValue, m.val4.bValue);
		return true;

	case MFD_PANEL_GET_FAILED_STATE:
		m.val1.bValue = panel.GetFailedState(static_cast<char *>(m.val1.pValue));
		return true;

	case MFD_PANEL_CHECKLIST_AUTOCOMPLETE:
		m.val1.bValue = checklist.autoComplete(m.val1.bValue);
		return true;
	case MFD_PANEL_GET_CHECKLIST_ITEM:
		m.val1.pValue = checklist.getChecklistItem(m.val1.iValue, m.val2.iValue);
		return true;
	case MFD_PANEL_GET_CHECKLIST_LIST:
		m.val1.pValue = checklist.getChecklistList();
		return true;
	case MFD_PANEL_FAIL_ITEM:
		m.val2.bValue = checklist.failChecklistItem(static_cast<ChecklistItem *>(m.val1.pValue));
		return true;
	case MFD_PANEL_COMPLETE_ITEM:
		m.val2.bValue = checklist.completeChecklistItem(static_cast<ChecklistItem *>(m.val1.pValue));
		return true;
	case MFD_PANEL_CHECKLIST_AUTOCOMPLETE_QUERY:
		m.val1.bValue = checklist.autoComplete();
		return true;
	case MFD_PANEL_CHECKLIST_NAME:
		m.val1.pValue = checklist.activeName();
		return true;
	case MFD_PANEL_RETRIEVE_CHECKLIST:
		m.val2.bValue = checklist.retrieveChecklistContainer(static_cast<ChecklistContainer *>(m.val1.pValue));
		return true;
	case MFD_PANEL_CHECKLIST_FLASHING_QUERY:
		m.val1.bValue = checklist.getFlashing();
		return true;
	case MFD_PANEL_CHECKLIST_FLASHING:
		checklist.setFlashing(m.val1.bValue);
		return true;
	}

	return false;
}

void MasterAlarmSwitch::SetState(int value) {

	if (value == TOGGLESWITCH_UP)
		cws->PushMasterAlarm();
}

int MasterAlarmSwitch::GetState() {

	if (cws->GetMasterAlarm()) {
		return TOGGLESWITCH_DOWN;
	}
	return TOGGLESWITCH_UP;
}


void DSKYPushSwitch::DoDrawSwitch(SURFHANDLE DrawSurface) {

	if (IsUp())	{
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset, yOffset, width, height, SURF_PREDEF_CK);
	} else {
		oapiBlt(DrawSurface, SwitchSurface, x, y, xOffset, yOffset + 120, width, height, SURF_PREDEF_CK);
	}
}

PanelGroup::~PanelGroup()
{
	while (!panels.empty()) {
		BasicPanel* panel = panels.back();
		if (panel) delete panel;
		panels.pop_back();
	}
}

bool PanelGroup::AddPanel(BasicPanel* pPanel, PanelSwitchScenarioHandler *PSH)
{
	panels.push_back(pPanel);
	pPanel->Register(PSH);
	return true;
}