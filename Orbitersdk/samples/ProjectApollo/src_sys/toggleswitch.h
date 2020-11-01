/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  Toggle switch handling code definitions.

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

#pragma once

#include <vector>
#include "cautionwarning.h"
#include "powersource.h"
#include "nasspdefs.h"

//
// Switch states. Only use positive numbers.
//

#define TOGGLESWITCH_DOWN		0			///< Toggle switch is up.
#define TOGGLESWITCH_UP			1			///< Toggle switch is down.

#define THREEPOSSWITCH_DOWN		0			///< Three-position switch is down.
#define THREEPOSSWITCH_CENTER	1			///< Three-position switch is centered.
#define THREEPOSSWITCH_UP		2			///< Three-position switch is up.

#define FIVEPOSSWITCH_DOWN		0			///< Five-position switch is down.
#define FIVEPOSSWITCH_CENTER	1			///< Five-position switch is centered.
#define FIVEPOSSWITCH_UP		2			///< Five-position switch is up.
#define FIVEPOSSWITCH_LEFT		3			///< Five-position switch is left.
#define FIVEPOSSWITCH_RIGHT		4			///< Five-position switch is right.

#define GUARDEDSWITCH_UNGUARD	101			///<SetState can open a guard from this.
#define GUARDEDSWITCH_GUARD		102			///<SetState can close a guard from this.

#define SPRINGLOADEDSWITCH_NONE					0		///< Switch is not spring-loaded.
#define SPRINGLOADEDSWITCH_DOWN					1		///< Switch is spring-loaded to down.
#define SPRINGLOADEDSWITCH_CENTER				2		///< Switch is spring-loaded to center.
#define SPRINGLOADEDSWITCH_UP					3		///< Switch is spring-loaded to up.
#define SPRINGLOADEDSWITCH_CENTER_SPRINGUP		4
#define SPRINGLOADEDSWITCH_CENTER_SPRINGDOWN	5

#define PANELSWITCH_START_STRING	"PANELSWITCHES_BEGIN"	///< Beginning of saved switch states in scenario file.
#define PANELSWITCH_END_STRING		"PANELSWITCHES_END"		///< End of saved switch states in scenario file.

#define TIME_UPDATE_SECONDS	0
#define TIME_UPDATE_MINUTES 1
#define TIME_UPDATE_HOURS	2

class SwitchRow;
class PanelSwitchScenarioHandler;
class PanelSwitchCallbackInterface;

class PanelSwitchCallbackInterface;
///
/// This is the base class for panel items. Items using this class can be looked up by name,
/// and are automatically saved into scenario files and loaded from them without us having to
/// individually track each one.
/// \brief Panel item base class.
/// \ingroup PanelItems
///
class PanelSwitchItem: public e_object {
friend class SwitchRow;
public:
	PanelSwitchItem();
	virtual ~PanelSwitchItem();

	void SetNext(PanelSwitchItem *s) { next = s; };
	PanelSwitchItem *GetNext() { return next; };
	void SetNextForScenario(PanelSwitchItem *s) { nextForScenario = s; };
	PanelSwitchItem *GetNextForScenario() { return nextForScenario; };

	///
	/// Force an object to fail, or return it to correct operation, and set the
	/// state it was in when it failed.
	/// \brief Force an object to fail and set state.
	/// \param fail True to make the object fail, false to return it to operation.
	/// \param fail_state The state the object will fail in (e.g. switch up or switch down).
	///
	void SetFailed(bool fail, int fail_state = 0) { Failed = fail; FailedState = fail_state; };

	///
	/// \brief Has the object failed?
	/// \return True if the object failed, false if it's operating normally.
	///
	bool IsFailed() { return Failed; };

	///
	/// \brief Get the name of this item.
	/// \return Pointer to a string containing the item name.
	///
	char *GetName() { return name; }

	///
	/// \brief Process a mouse click which may be for this item.
	/// \param event Mouse event type.
	/// \param mx Mouse x position.
	/// \param my Mouse y position.
	/// \return True if we processed it, false if it wasn't for us.
	///
	virtual bool CheckMouseClick(int event, int mx, int my) = 0;

	///
	/// \brief Draw the switch with its current state and position.
	/// \param DrawSurface Surface to draw the switch into.
	///
	virtual void DrawSwitch(SURFHANDLE DrawSurface) = 0;

	///
	/// \brief Save the switch state.
	/// \param scn Scenario file to save state into.
	///
	virtual void SaveState(FILEHANDLE scn) = 0;

	///
	/// \brief Load state from scenario.
	/// \param line A line from the scenario file, which may or may not be for us.
	///
	virtual void LoadState(char *line) = 0;
	virtual void DrawFlash(SURFHANDLE DrawSurface) {};

	///
	/// Each object has a human-readable displayable name. Normally this will be a
	/// pre-initialised string rather than a dynamic name, so we just copy the pointer
	/// passed into us and don't take a copy. If you call this with a temporary string
	/// on the stack our pointer will end up pointing to garbage after you return from
	/// the routine which set up the string.
	/// \brief Set the displayable name.
	///
	void SetDisplayName(char *s) { DisplayName = s; };

	///
	/// \brief Get the displayable name.
	/// \return Pointer to the displayable name, or NULL.
	///
	char *GetDisplayName();

	///
	/// \brief Start flashing the object.
	/// \param flash True to turn flashing on, false to turn it off.
	///
	void SetFlashing(bool flash) { flashing = flash; };

	///
	/// \brief Check whether flashing is enabled.
	/// \return True if the object is flashing.
	///
	bool IsFlashing() { return flashing; };

	///
	/// \brief Set the visibitility state of the item.
	/// \param v True if it's visible, false if it's not.
	///
	void SetVisible(bool v) {visible = v; };

	///
	/// \brief Get current state.
	/// \return Current state value.
	///
	virtual int GetState();

	///
	/// \brief Set current state.
	/// \param value State to set.
	///
	virtual void SetState(int value);

	///
	/// \brief Check the power state.
	/// \return True if the switch is powered.
	///
	virtual bool IsPowered() { return Voltage() > SP_MIN_DCVOLTAGE; };

	///
	/// \brief Timestep function, only called if doTimeStep is true
	/// \param time - Mission time
	///
	virtual void timestep(double missionTime){return;}

	///
	/// \brief Set a callback.  Note, make absolute sure that the old callback is properly disposed of.
	/// \param call - New callback to use.
	///
	virtual void SetCallback(PanelSwitchCallbackInterface* call);

	///
	/// \brief Unguard
	///
	virtual void Unguard() {};

	///
	/// \brief Guard
	///
	virtual void Guard() {};

	virtual	void SetHeld(bool s) {};

	virtual void RedrawVC(UINT anim) {}
	virtual bool ProcessMouseVC(int event, VECTOR3 &p) { return false; }
	
protected:
	///
	/// This can be interpreted in any desired manner by derived classes. Only positive values should be used.
	/// \brief Current state.
	///
	int state;

	///
	/// boolean determining whether to call timestep.
	///
	bool doTimeStep;

	///
	/// If a switch fails, you can no longer change the state even if you can still move
	/// the toggle. If a gauge fails it will no longer read the correct value.
	/// \brief Has the switch failed?
	///
	bool Failed;

	///
	/// \brief State that the switch failed in.
	///
	int FailedState;

	///
	/// \brief Name of the switch.
	///
	char *name;

	///
	/// We can display a flashing rectangle around a switch or gauge to point the user to
	/// it when running a checklist. For example, if they need to click a switch to continue
	/// we can indicate the switch to click until it's in the correct state, or if they should
	/// check a gauge reading we can flash that briefly.
	/// \brief Is the object flashing?
	///
	bool flashing;

	///
	/// A human-readable object name which we can display to them when we want them to use the
	/// object. For example 'The CSM/LV seperation switch', or 'The liquid oxygen quantity gauge.'
	/// \brief Displayable name.
	///
	char *DisplayName;

	///
	/// \brief Is the object visible?
	///
	bool visible;

	PanelSwitchItem *next;
	PanelSwitchItem *nextForScenario;
	PanelSwitchCallbackInterface *callback;
};

///
/// Generic two-position toggle switch.
/// \brief Toggle switch base class.
/// \ingroup PanelItems
///
class ToggleSwitch: public PanelSwitchItem {

public:
	ToggleSwitch();
	virtual ~ToggleSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int springloaded = SPRINGLOADEDSWITCH_NONE, char *dname = 0);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SoundLib &s,
		      int xoffset = 0, int yoffset = 0);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,
		      int xoffset = 0, int yoffset = 0);
	void SetSize(int w, int h) { width = w; height = h; };
	void SetPosition(int xp, int yp) { x = xp; y = yp; };
	void SetOffset(int xo, int yo) {xOffset = xo; yOffset = yo; };
	void SetSpringLoaded(int springloaded) { springLoaded = springloaded; };
	bool IsSpringLoaded() { return (springLoaded != SPRINGLOADEDSWITCH_NONE); };
	void SetHeld(bool s) { Held = s; };
	bool IsHeld() { return Held; };
	void SetActive(bool s);
	void SetSideways(int s) { Sideways = s; }
	void SetDelayTime(double t) { delayTime = t; };

	void RedrawVC(UINT anim);
	bool ProcessMouseVC(int event, VECTOR3 &p);

	bool Toggled() { return SwitchToggled; };
	void ClearToggled() { SwitchToggled = false; };
	
	void DrawFlash(SURFHANDLE DrawSurface);
	void SetBorderSurface(SURFHANDLE border) { BorderSurface = border; };

	virtual bool IsUp() { return (GetState() == TOGGLESWITCH_UP); };
	virtual bool IsDown() { return (GetState() == TOGGLESWITCH_DOWN); };
	virtual bool IsCenter() { return false; };

	virtual bool SwitchTo(int newState, bool dontspring = false);
	virtual void DrawSwitch(SURFHANDLE DrawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
	virtual void SaveState(FILEHANDLE scn);
	virtual void LoadState(char *line);
	virtual void SetState(int value); //Needed to properly process set states from toggle switches.
	virtual void timestep(double missionTime);

protected:
	virtual void InitSound(SoundLib *s);
	virtual void DoDrawSwitch(SURFHANDLE DrawSurface);
	bool DoCheckMouseClick(int event, int mx, int my);

	virtual unsigned int GetFlags();
	virtual void SetFlags(unsigned int f);

	int	x;
	int y;
	int width;
	int height;
	int xOffset;
	int yOffset;

	int springLoaded;
	bool Active;
	bool SwitchToggled;
	bool Held;
	//0 = not sideways, 1 = sideways rotated 90° clockwise, 2 = sideways rotated 90° counterclockwise
	int Sideways;

	double delayTime;
	double resetTime;

	SURFHANDLE SwitchSurface;
	SURFHANDLE BorderSurface;
	VESSEL *OurVessel;
	Sound Sclick;

	SwitchRow *switchRow;

	///
	/// Flags structure for saving state to scenario file.
	///
	struct ToggleSwitchFlags {
		union {
			struct {
				unsigned int Held:1;	///< Is the switch held?
			};
			unsigned int flags;			///< Packed structure value to save.
		};

		///
		/// Constructor. Set flags to zero.
		///
		ToggleSwitchFlags() { flags = 0; };
	};
};

///
/// \brief Orbiter attitude-control toggle switch.
/// \ingroup PanelItems
///
class AttitudeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	virtual bool SwitchTo(int newState, bool dontspring = false);

};

///
/// \brief Orbiter attitude-control toggle switch.
/// \ingroup PanelItems
///
class NavModeToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, VESSEL *v, int mode, SoundLib &s);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	int NAVMode;
};

///
/// \brief Orbiter HUD-mode toggle switch.
/// \ingroup PanelItems
///
class HUDToggle: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int mode, SoundLib &s);
	virtual bool SwitchTo(int newState,bool dontspring = false);

protected:
	int	HUDMode;
};

///
/// Generic three-position toggle switch
/// \brief Three position toggle switch base class.
/// \ingroup PanelItems
///
class ThreePosSwitch: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState, bool dontspring = false);

	bool IsDown() { return (GetState() == THREEPOSSWITCH_DOWN); };
	bool IsCenter() { return (GetState() == THREEPOSSWITCH_CENTER); };
	bool IsUp() { return (GetState() == THREEPOSSWITCH_UP); };

	bool ProcessMouseVC(int event, VECTOR3 &p);
};

///
/// Generic five-position toggle switch
/// \brief Five position toggle switch base class.
/// \ingroup PanelItems
///

class FivePosSwitch: public ToggleSwitch {

public:
	void DrawSwitch(SURFHANDLE DrawSurface);
	void DrawSwitchVC(UINT animx, UINT animy);
	bool CheckMouseClick(int event, int mx, int my);
	bool CheckMouseClickVC(int event, VECTOR3 &p);
	virtual bool SwitchTo(int newState, bool dontspring = false);

	bool IsDown() { return (GetState() == FIVEPOSSWITCH_DOWN); };
	bool IsCenter() { return (GetState() == FIVEPOSSWITCH_CENTER); };
	bool IsUp() { return (GetState() == FIVEPOSSWITCH_UP); };
	bool IsLeft() { return (GetState() == FIVEPOSSWITCH_LEFT); };
	bool IsRight() { return (GetState() == FIVEPOSSWITCH_RIGHT); };

};

///
/// A three-position switch which can switch between three different electrical sources.
/// \brief Three power source switch.
/// \ingroup PanelItems
///
class ThreeSourceSwitch : public ThreePosSwitch {
public:
	ThreeSourceSwitch() { source[0] = source[1] = source[2] = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void SetSource(int i, e_object *s);

protected:
	virtual void UpdateSourceState();

	e_object *source[3];
};

///
/// A two-position switch which can switch between two different electrical sources.
/// \brief Two power source switch.
/// \ingroup PanelItems
///
class TwoSourceSwitch : public ToggleSwitch {
public:
	TwoSourceSwitch() { source1 = source2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void LoadState(char *line);
	//virtual void SetState(int value);

protected:
	virtual void UpdateSourceState();

	e_object *source1;
	e_object *source2;
};

///
/// A two-position switch which can switch its power between two different electrical outputs.
/// \brief Two power output switch.
/// \ingroup PanelItems
///
class TwoOutputSwitch : public ToggleSwitch {
public:
	TwoOutputSwitch() { output1 = output2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	virtual void UpdateSourceState();

	e_object *output1;
	e_object *output2;
};

///
/// A two-position switch which can switch multiple connections between sources and output buses.
/// \brief N-sources to outputs switch.
/// \ingroup PanelItems
///
class NSourceDestSwitch : public ToggleSwitch {
public:
	NSourceDestSwitch(int nSources);
	~NSourceDestSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void WireSourcesToBuses(int bus, e_object* i, DCbus* o);

protected:
	virtual void UpdateSourceState();

	int nSources;
	e_object **sources;
	DCbus **buses;
};

///
/// A three-position switch which can switch its power between three different electrical outputs.
/// \brief Three power output switch.
/// \ingroup PanelItems
///
class ThreeOutputSwitch : public ThreePosSwitch {
public:
	ThreeOutputSwitch() { output1 = output2 = output3 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2, e_object *o3);
	void LoadState(char *line);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	virtual void UpdateSourceState();

	e_object *output1;
	e_object *output2;
	e_object *output3;
};

class ThreeSourceTwoDestSwitch : public ThreeSourceSwitch {
public:
	ThreeSourceTwoDestSwitch() { dest1 = dest2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2, e_object *s3, e_object *d1, e_object *d2);

protected:
	virtual void UpdateSourceState();

	e_object *dest1;
	e_object *dest2;
};

//
// Mission Timer switches.
//

class MissionTimer; // Forward reference for files which include this before missiontimer.h

///
/// A generic three-position switch which can control a mission timer.
/// \brief Mission timer switch base class.
/// \ingroup PanelItems
///
class MissionTimerSwitch: public ThreePosSwitch {

public:
	MissionTimerSwitch() { timer = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, MissionTimer *ptimer);

protected:
	MissionTimer *timer;
};

///
/// A three-position switch which controls a mission timer.
/// \brief Mission timer control switch.
/// \ingroup PanelItems
///
class TimerControlSwitch: public MissionTimerSwitch {

public:
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	void SetTimer();
};

///
/// A three-position switch which updates the time on a mission timer.
/// \brief Mission timer time update switch.
/// \ingroup PanelItems
///
class TimerUpdateSwitch: public MissionTimerSwitch {

public:
	TimerUpdateSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int adjuster, MissionTimer *ptimer);
	virtual bool SwitchTo(int newstate, bool dontspring = false);

protected:
	void AdjustTime(int val);

	int adjust_type;
};


///
/// Switch that starts and stops the event timer.
/// \brief Event timer control switch.
/// \ingroup PanelItems
///
class EventTimerControlSwitch: public MissionTimerSwitch {

public:
	virtual bool SwitchTo(int newState, bool dontspring = false);

};

///
/// Switch that controls count up/count down/reset for event timer.
/// \brief Event timer count/reset switch.
/// \ingroup PanelItems
///
class EventTimerResetSwitch: public MissionTimerSwitch {

public:
	//bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newState, bool dontspring = false);

};

class CautionWarningSystem; // Forward reference for files which include this before cautionwarning.h

///
/// A generic three-position switch which can control a caution and warning system.
/// \brief CWS control switch base class.
/// \ingroup PanelItems
///
class CWSThreePosSwitch: public ThreePosSwitch {

public:
	CWSThreePosSwitch() { cws = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c);

protected:
	///
	/// \brief The caution and warning system that this switch controls.
	///
	CautionWarningSystem *cws;
};

///
/// A three-position switch controlling a caution and warning system light test.
/// \brief CWS light test switch.
/// \ingroup PanelItems
///
class CWSLightTestSwitch: public CWSThreePosSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

///
/// A two-position switch which is pushed in/out rather than toggled up/down.
/// \brief Two-position push switch.
/// \ingroup PanelItems
///
class PushSwitch: public ToggleSwitch {

public:
	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, char *dname = 0);
	bool CheckMouseClick(int event, int mx, int my);
	bool ProcessMouseVC(int event, VECTOR3 &p);

protected:
	virtual void InitSound(SoundLib *s);
};

///
/// A two-position electrical circuit breaker switch which is pushed in and pulled out rather 
/// than toggled up/down, and turns its electrical supply on and off as it does so.
/// \brief Two-position circuit breaker switch.
/// \ingroup PanelItems
///
class CircuitBrakerSwitch: public ToggleSwitch {

public:
	CircuitBrakerSwitch() { MaxAmps = 0.0; };

	bool CheckMouseClick(int event, int mx, int my);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s = 0, double amps = 30.0);

	double Voltage();
	double Current();
	double Frequency();
	void DrawPower(double watts);

	///
	/// Maximum current which can be pulled through the circuit breaker before it automatically
	/// pops out.
	/// \brief Maximum safe current.
	///
	double MaxAmps;
	/// This has to be set early in initialization, otherwise MaxLoad is referred to before assignment
	/// when loading a scenario. See the MaxAmps setting happening in the LM systems init as the systems are wired.

protected:
	virtual void InitSound(SoundLib *s);

};

///
/// A three-position switch controlling a caution and warning system operating mode.
/// \brief CWS mode switch.
/// \ingroup PanelItems
///
class CWSModeSwitch: public CWSThreePosSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

///
/// A three-position switch controlling caution and warning system power.
/// \brief CWS power switch.
/// \ingroup PanelItems
///
class CWSPowerSwitch: public CWSThreePosSwitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
	//virtual void SetState(int value);

protected:
	void SetPowerBus();
};

///
/// A two-position switch controlling caution and warning system source, typically switching
/// between monitoring CSM systems and only monitoring CM systems..
/// \brief CWS source switch.
/// \ingroup PanelItems
///
class CWSSourceSwitch: public ToggleSwitch {
public:
	CWSSourceSwitch() { cws = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, CautionWarningSystem *c);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	///
	/// \brief The caution and warning system that this switch controls.
	///
	CautionWarningSystem *cws;
};

///
/// Dummy switch for checklist control
///
class MasterAlarmSwitch: public PushSwitch {

public:
	void Init(CautionWarningSystem *c) { cws = c; };
	int GetState();
	void SetState(int value);

protected:
	CautionWarningSystem *cws;
};

//
// Switches that talk directly to the AGC.
//

class ApolloGuidance; // Forward reference for files which include this before apolloguidance.h

class AGCSwitch: public ToggleSwitch {

public:
	AGCSwitch() { agc = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c, int xoffset = 0, int yoffset = 0);

protected:
	ApolloGuidance *agc;
};

class AGCThreePoswitch: public ThreePosSwitch {

public:
	AGCThreePoswitch() { agc = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c);

protected:
	ApolloGuidance *agc;
};

//
// This class directly toggles AGC input channel states.
//

class AGCIOSwitch: public AGCSwitch {
public:
	AGCIOSwitch() { Channel = 0; Bit = 0; UpValue = false; };
	void SetChannelData(int chan, int bit, bool value) { Channel = chan; Bit = bit; UpValue = value; };
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	bool UpValue;
	int Channel;
	int Bit;
};

//
// CMC mode switch.
//

class CMCModeHoldFreeSwitch : public AGCThreePoswitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class CMCOpticsZeroSwitch : public ToggleSwitch {
public:
	void DoDrawSwitch(SURFHANDLE DrawSurface);
};

class PGNSSwitch : public AGCThreePoswitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class ModeSelectSwitch : public AGCThreePoswitch {
public:
	virtual bool SwitchTo(int newState, bool dontspring = false);
};

class GuardedToggleSwitch: public ToggleSwitch {

public:
	GuardedToggleSwitch();
	virtual ~GuardedToggleSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, int springloaded = SPRINGLOADEDSWITCH_NONE);
	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf,
				   int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	void DrawSwitchVC(UINT anim, UINT animguard);
	void DrawFlash(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	bool CheckMouseClickVC(int event, VECTOR3 &p);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetGuardState() { return guardState; };
	void SetGuardState(bool s) { guardState = s; };
	void SetGuardResetsState(bool s) { guardResetsState = s; };
	void Unguard() { guardState = 1; };
	void Guard();

protected:
	int	guardX;
	int guardY;
	int guardWidth;
	int guardHeight;
	int guardState;
	bool guardResetsState;

	SURFHANDLE guardSurface;
	SURFHANDLE guardBorder;

	int guardXOffset;
	int guardYOffset;
	Sound guardClick;
};

class AGCGuardedToggleSwitch : public GuardedToggleSwitch {

public:
	AGCGuardedToggleSwitch() { agc = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, ApolloGuidance *c);

protected:
	ApolloGuidance *agc;
};

class AGCIOGuardedToggleSwitch : public AGCGuardedToggleSwitch {
public:
	AGCIOGuardedToggleSwitch() { Channel = 0; Bit = 0; UpValue = false; };
	void SetChannelData(int chan, int bit, bool value) { Channel = chan; Bit = bit; UpValue = value; };
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	bool UpValue;
	int Channel;
	int Bit;
};

class GuardedTwoOutputSwitch : public GuardedToggleSwitch {
public:
	GuardedTwoOutputSwitch() { output1 = output2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *o1, e_object *o2);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void LoadState(char *line);
	virtual void SetState(int value);

protected:
	virtual void UpdateSourceState(int newState);

	e_object *output1;
	e_object *output2;
};

///
/// A guarded two-position switch which can switch between two different electrical sources.
/// \brief Guarded two power source switch.
/// \ingroup PanelItems
///
class GuardedTwoSourceSwitch : public GuardedToggleSwitch {
public:
	GuardedTwoSourceSwitch() { source1 = source2 = 0; };
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, e_object *s1, e_object *s2);
	virtual bool SwitchTo(int newState, bool dontspring = false);
	void LoadState(char *line);

protected:
	virtual void UpdateSourceState();

	e_object *source1;
	e_object *source2;
};


class IMU; // Forward reference for files which include this before IMU.h

class IMUCageSwitch: public GuardedToggleSwitch {

public:
	IMUCageSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	IMU *imu;

	void SetIMU();
};

class UnguardedIMUCageSwitch: public ToggleSwitch {

public:
	UnguardedIMUCageSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, IMU *im);
	virtual bool SwitchTo(int newState, bool dontspring = false);

protected:
	IMU *imu;

	void SetIMU();
};

class GuardedPushSwitch: public PushSwitch {

public:
	GuardedPushSwitch();
	virtual ~GuardedPushSwitch();

	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row,
		int xoffset = 0, int yoffset = 0, int lxoffset = 0, int lyoffset = 0);

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState);
	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE dsurf,
				   int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	void DrawFlash(SURFHANDLE DrawSurface);
	void DoDrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetGuardState() { return guardState; };
	void SetGuardState(bool s) { guardState = s; };
	void Unguard() { guardState = 1; };
	void Guard();

	void SetLit(bool l) { lit = l; };
	bool IsLit() { return lit; };

protected:
	int	guardX;
	int guardY;
	int guardWidth;
	int guardHeight;
	int guardState;

	int litOffsetX;
	int litOffsetY;

	bool lit;

	SURFHANDLE guardSurface;
	SURFHANDLE guardBorder;

	int guardXOffset;
	int guardYOffset;
	Sound guardClick;
};


class GuardedThreePosSwitch: public ThreePosSwitch {

public:
	GuardedThreePosSwitch();
	virtual ~GuardedThreePosSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int defaultGuardState, 
				  int springloaded = SPRINGLOADEDSWITCH_NONE);
	void InitGuard(int xp, int yp, int w, int h, SURFHANDLE surf,
				   int xOffset = 0, int yOffset = 0);
	void DrawSwitch(SURFHANDLE DrawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetGuardState() { return guardState; };
	void SetGuardState(bool s) { guardState = s; };
	void SetGuardResetsState(bool s) { guardResetsState = s; };
	void Unguard() { guardState = 1; };
	void Guard();

protected:
	int	guardX;
	int guardY;
	int guardWidth;
	int guardHeight;
	int guardState;
	bool guardResetsState;
	SURFHANDLE guardSurface;
	int guardXOffset;
	int guardYOffset;
	Sound guardClick;
};


typedef struct {
	double angle;
	int xOffset;
	int yOffset;
} RotationalSwitchBitmap;

const int RotationalSwitchBitmapCount = 24;


class RotationalSwitchPosition {

public:
	RotationalSwitchPosition(int v, double a);
	virtual ~RotationalSwitchPosition();
	void SetNext(RotationalSwitchPosition *p) { next = p; };
	RotationalSwitchPosition *GetNext() { return next; };
	double GetAngle() { return angle; };
	int GetValue() { return value; };

protected:
	int value;
	double angle;
	RotationalSwitchPosition* next;
};


class RotationalSwitch: public PanelSwitchItem {

public:
	RotationalSwitch();
	virtual ~RotationalSwitch();

	virtual void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultValue);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void AddPosition(int value, double angle);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawFlash(SURFHANDLE drawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
	virtual bool SwitchTo(int newValue);
	virtual void SaveState(FILEHANDLE scn);
	virtual void LoadState(char *line);
	int GetState();
	operator int();
	virtual void SetState(int value);
	void SoundEnabled(bool on) { soundEnabled = on; };
	void SetWraparound(bool w) { Wraparound = w; };

	void RedrawVC(UINT anim);
	bool ProcessMouseVC(int event, VECTOR3 &p);

protected:
	int	x;
	int y;
	int width;
	int height;
	int maxState;
	bool Wraparound;
	RotationalSwitchPosition *position;
	RotationalSwitchPosition *positionList;

	SURFHANDLE switchSurface;
	SURFHANDLE switchBorder;

	VESSEL *OurVessel;
	Sound sclick;
	bool soundEnabled;
	RotationalSwitchBitmap bitmaps[RotationalSwitchBitmapCount];
	SwitchRow *switchRow;

	void SetValue(int newValue);
	double AngleDiff(double a1, double a2);
	void DeletePositions();
};

class PowerStateRotationalSwitch: public RotationalSwitch {
public:
	PowerStateRotationalSwitch();

	virtual bool SwitchTo(int newValue);
	void LoadState(char *line);
	void SetSource(int num, e_object *s);
	double Current();
	double Voltage();

protected:
	void CheckPowerState();

	e_object *sources[16];
};

class OrdealRotationalSwitch : public RotationalSwitch {

public:
	OrdealRotationalSwitch() { value = 100; lastX = 0; mouseDown = false; };
	virtual void DrawSwitch(SURFHANDLE drawSurface);
	virtual bool CheckMouseClick(int event, int mx, int my);
	virtual void SaveState(FILEHANDLE scn);
	virtual void LoadState(char *line);
	int GetValue() { return value; }

protected:
	int value;
	int lastX;
	bool mouseDown;
};

class IndicatorSwitch: public PanelSwitchItem {

public:
	IndicatorSwitch();
	virtual ~IndicatorSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SwitchRow &row, bool failopen = false);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawSwitchVC(SURFHANDLE drawSurface, SURFHANDLE switchsurfacevc);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	virtual int GetState() { return state; };
	virtual void SetState(int s) { state = s; };

//	int operator=(const int b) { state = b; return state; };
//	operator int() {return state; };

protected:
	int state; // Changed to INT for extended capabilities hackery
	double displayState;	//0: false, 1: moving, 2: moving, 3: true
	bool failOpen;
	int	x;
	int y;
	int width;
	int height;
	SURFHANDLE switchSurface;
	SwitchRow *switchRow;
};


class MeterSwitch: public PanelSwitchItem {

public:
	MeterSwitch();
	virtual ~MeterSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time, double defaultValue);
	void Register(PanelSwitchScenarioHandler &scnh, char *n, double min, double max, double time);
	void Init(SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	double GetDisplayValue();

	virtual double QueryValue() = 0;
	virtual void DoDrawSwitch(double v, SURFHANDLE drawSurface) = 0;

protected:

	virtual double AdjustForPower(double val);

	double value;
	double displayValue;
	double minValue;
	double maxValue;
	double minMaxTime;
	SwitchRow *switchRow;
	double lastDrawTime;
};


class ThumbwheelSwitch: public PanelSwitchItem {

public:
	ThumbwheelSwitch();
	virtual ~ThumbwheelSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState);
	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState, bool horizontal);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	void RedrawVC(UINT anim);
	void DrawFlash(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	bool ProcessMouseVC(int event, VECTOR3 &p);
	virtual bool SwitchTo(int newState);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetState();
//	int operator=(const int b);
//	operator int();
	virtual void SetState(int value);

protected:
	int	x;
	int y;
	int width;
	int height;
	int state;
	int maxState;
	bool isHorizontal;
	SURFHANDLE switchSurface;
	SURFHANDLE switchBorder;
	Sound sclick;
	SwitchRow *switchRow;
	VESSEL *OurVessel;
};

class ContinuousThumbwheelSwitch : public ThumbwheelSwitch {
public:
	ContinuousThumbwheelSwitch();
	void Register(PanelSwitchScenarioHandler &scnh, char *n, int defaultState, int maximumState, bool horizontal, int multPos);
	bool CheckMouseClick(int event, int mx, int my);
	bool SwitchTo(int newPosition);
	void LoadState(char *line);
	void SetState(int value);
	int GetPosition();
protected:
	int StateToPosition(int st);
	int PositionToState(int pos);

	int multiplicator;
	int numPositions;
	int position;
};


class HandcontrollerSwitch: public PanelSwitchItem {

public:
	HandcontrollerSwitch();
	virtual ~HandcontrollerSwitch();

	void Register(PanelSwitchScenarioHandler &scnh, char *n, bool hasyawaxis = false);
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row);
	void DrawSwitch(SURFHANDLE drawSurface);
	void DrawFlash(SURFHANDLE drawSurface);
	bool CheckMouseClick(int event, int mx, int my);
	void SaveState(FILEHANDLE scn);
	void LoadState(char *line);
	int GetState();

protected:
	bool hasYawAxis;
	int	x;
	int y;
	int width;
	int height;
	int state;
	SURFHANDLE switchSurface;
	SURFHANDLE borderSurface;
	Sound sclick;
	SwitchRow *switchRow;
};

class SoundLib;

class VolumeThumbwheelSwitch: public ThumbwheelSwitch {

public:
	VolumeThumbwheelSwitch();
	void Init(int xp, int yp, int w, int h, SURFHANDLE surf, SURFHANDLE bsurf, SwitchRow &row, int vclass, SoundLib *s);
	virtual bool SwitchTo(int newState);
	void LoadState(char *line);

protected:
	SoundLib *sl;
	int volume_class;
};

class PanelSwitches;

class SwitchRow {

public:
	SwitchRow();
	virtual ~SwitchRow();

	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn);
	void AddSwitch(PanelSwitchItem *s);
	void Init(int area, PanelSwitches &panel, e_object *p = 0);
	SwitchRow *GetNext() { return RowList; };
	void SetNext(SwitchRow *s) { RowList = s; };
	void timestep(double missionTime);

	///
	/// Look up a panel switch item by its name.
	///
	/// \param n String for panel item name.
	/// \return Item if found, NULL if not.
	///
	PanelSwitchItem *GetItemByName(const char *n);

protected:
	PanelSwitchItem *SwitchList;
	SwitchRow *RowList;
	int PanelArea;
	PanelSwitches *panelSwitches;

	e_object *RowPower;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class FivePosSwitch;
	friend class PushSwitch;
	friend class GuardedToggleSwitch;
	friend class GuardedThreePosSwitch;
	friend class GuardedPushSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
	friend class ThumbwheelSwitch;
	friend class CircuitBrakerSwitch;
	friend class HandcontrollerSwitch;
};

class PanelSwitchListener {

public:
	virtual void PanelSwitchToggled(ToggleSwitch *s) = 0;
	virtual void PanelIndicatorSwitchStateRequested(IndicatorSwitch *s) = 0;
	virtual void PanelRotationalSwitchChanged(RotationalSwitch *s) = 0;
};

class PanelSwitchesVC
{
public:
	PanelSwitchesVC() {}
	bool VCMouseEvent(int id, int event, VECTOR3 &p);
	bool VCRedrawEvent(int id, int event, SURFHANDLE surf);
	void AddSwitch(PanelSwitchItem *s, int area, UINT * anim);
	void ClearSwitches();
protected:
	std::vector<PanelSwitchItem*>SwitchList;
	std::vector<int> SwitchArea;
	std::vector<UINT *> SwitchAnim;
};

class PanelSwitches {

public:
	PanelSwitches() { PanelID = 0; RowList = 0; lastexecutedtime=MINUS_INFINITY;};
	bool CheckMouseClick(int id, int event, int mx, int my);
	bool DrawRow(int id, SURFHANDLE DrawSurface, bool FlashOn);
	void AddRow(SwitchRow *s) { s->SetNext(RowList); RowList = s; };
	void Init(int id, VESSEL *v, SoundLib *s, PanelSwitchListener *l) { PanelID = id; RowList = 0; vessel = v; soundlib = s; listener = l; };
	void timestep(double missionTime);

	///
	/// Set an item's flashing state.
	///
	/// \param n Item name.
	/// \param flash True for flashing, false for not.
	/// \return True if we found the item, false if not.
	///
	bool SetFlashing(const char *n, bool flash);

	int GetState(const char *n);
	bool SetState(const char *n, int value, bool guard = false, bool hold = false);
	bool GetFailedState(const char *n);
	bool GetFlashing(const char *n);

protected:
	VESSEL *vessel;
	SoundLib *soundlib;
	PanelSwitchListener *listener;
	int	PanelID;
	SwitchRow *RowList;
	double lastexecutedtime;

	friend class ToggleSwitch;
	friend class ThreePosSwitch;
	friend class FivePosSwitch;
	friend class PushSwitch;
	friend class GuardedToggleSwitch;
	friend class GuardedThreePosSwitch;
	friend class GuardedPushSwitch;
	friend class RotationalSwitch;
	friend class IndicatorSwitch;
	friend class ThumbwheelSwitch;
	friend class CircuitBrakerSwitch;
	friend class HandcontrollerSwitch;
};


class PanelSwitchScenarioHandler {

public:
	PanelSwitchScenarioHandler() { switchList = 0; };
	void RegisterSwitch(PanelSwitchItem *s);
	PanelSwitchItem* GetSwitch(char *name);
	void SaveState(FILEHANDLE scn);
	void LoadState(FILEHANDLE scn);

protected:
	PanelSwitchItem *switchList;
};

///
/// \brief Round meter for control panel.
///
/// This meter displays a value with a rotating dial.
///
/// \ingroup PanelItems
///
class RoundMeter : public MeterSwitch {
public:
	void Init(HPEN p0, HPEN p1, SwitchRow &row);

protected:
	HPEN Pen0;
	HPEN Pen1;

	void DrawNeedle (SURFHANDLE surf, int x, int y, double rad, double angle);
};

///
/// \brief Electric meter for control panel.
///
/// This meter displays the electrical readings of one of the numerous systems in the CSM (e.g. the main buses
/// or batteries).
///
/// \ingroup PanelItems
///
class ElectricMeter: public RoundMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum value to display (meter may show beyond it).
	/// \param maxVal Maximum value to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum value.
	/// \param vMax Angle of meter at maximum value.
	///
	ElectricMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Initialise the meter.
	///
	void Init(HPEN p0, HPEN p1, SwitchRow &row, e_object *dcindicatorswitch);

	///
	/// \brief Actually draw the switch.
	/// \param volts Current voltage.
	/// \param drawSurface The surface to draw to.
	///
	void DoDrawSwitch(double volts, SURFHANDLE drawSurface);

	///
	/// \brief Set the switch bitmap.
	/// \param srf Frame bitmap surface.
	/// \param x Width in pixels.
	/// \param y Height in pixels.
	///
	void SetSurface(SURFHANDLE srf, int x, int y);

protected:
	double minValue;		///< The minimum value to display.
	double maxValue;		///< The maximum value to  display.
	double minAngle;		///< Angle at minimum voltage.
	double maxAngle;		///< Angle at maximum voltage.
	double ScaleFactor;		///< The internal volts to angle scale factor.

	int xSize;				///< X-size of bitmap in pixels.
	int ySize;				///< Y-size of bitmap in pixels.

	///
	/// \brief The surface to use for the meter frame.
	///
	SURFHANDLE FrameSurface;

	double AdjustForPower(double val) { return val; } ///< These are always powered by definition.
};

///
/// \brief DC voltage meter for control panel.
///
/// \image html DCVolts.bmp "DC voltage meter"
///
/// This meter displays the DC voltage of one of the numerous DC systems in the CSM or LEM (e.g. the main buses
/// or batteries).
///
/// \ingroup PanelItems
///
class DCVoltMeter: public ElectricMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum voltage to display (meter may show beyond it).
	/// \param maxVal Maximum voltage to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum voltage.
	/// \param vMax Angle of meter at maximum voltage.
	///
	DCVoltMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Query the voltage.
	/// \return Current voltage.
	///
	double QueryValue();
};

///
/// \brief DC current meter for control panel.
///
/// \image html DCAmps.bmp "DC current meter"
///
/// This meter displays the DC current of one of the numerous DC systems in the CSM or LEM (e.g. the main buses
/// or batteries).
///
/// \ingroup PanelItems
///
class DCAmpMeter: public ElectricMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum current to display (meter may show beyond it).
	/// \param maxVal Maximum current to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum current.
	/// \param vMax Angle of meter at maximum current.
	///
	DCAmpMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Query the voltage.
	/// \return Current voltage.
	///
	double QueryValue();
};

///
/// \brief AC voltage meter for control panel.
///
/// \image html ACVolts.bmp "ACVolts meter"
///
/// This meter displays the AC voltage on one phase of one of the CSM AC buses.
///
/// \ingroup PanelItems
///
class ACVoltMeter: public ElectricMeter {
public:
	///
	/// \brief Constructor.
	/// \param minVal Minimum voltage to display (meter may show beyond it).
	/// \param maxVal Maximum voltage to display (meter may show beyond it).
	/// \param vMin Angle of meter at minimum voltage.
	/// \param vMax Angle of meter at maximum voltage.
	///
	ACVoltMeter(double minVal, double maxVal, double vMin = 202.5, double vMax = (-22.5));

	///
	/// \brief Query the voltage.
	/// \return Current voltage.
	///
	double QueryValue();
};

class DSKYPushSwitch: public PushSwitch {
protected:
	virtual void DoDrawSwitch(SURFHANDLE DrawSurface);
};

///
/// This is going to become the core of the panel callback system.  It follows a standard
/// functor model.  This allows each switch to call into the appropriate system rather than
/// the old style switch/case system that was used on a listener to update systems.
/// \brief Panel Item callback system.
/// \ingroup PanelItems
///
class PanelSwitchCallbackInterface
{
public:
	virtual void call(PanelSwitchItem* s) = 0;
};

///
/// Actual Functor for the Panel Switch Callback system.
/// \brief Panel Item callback system.
/// \ingroup PanelItems
///
template <class T> class PanelSwitchCallback : public PanelSwitchCallbackInterface
{
private:
	T* obj_ptr;
	void (T::*func_ptr)(PanelSwitchItem* s);
public:
	PanelSwitchCallback(T* ptr_to_obj, void (T::*ptr_to_func)(PanelSwitchItem* s))
	{
		obj_ptr = ptr_to_obj;
		func_ptr = ptr_to_func;
	}
	virtual void call(PanelSwitchItem* s)
	{
		(*obj_ptr.*func_ptr)(s);
	}
};

class BasicPanel
{
public:
	BasicPanel() {}
	virtual ~BasicPanel() {}
	virtual void Register(PanelSwitchScenarioHandler *PSH) = 0;
};

class PanelGroup
{
public:
	PanelGroup() {}
	virtual ~PanelGroup();

	virtual bool AddPanel(BasicPanel* pPanel, PanelSwitchScenarioHandler *PSH);
private:
	std::vector<BasicPanel*> panels;
};