/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER sound library.
  This code caches sounds for the XRSound library so that
  you don't have to keep track of IDs.

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
#include <string.h>

#include "resource.h"

#include "soundlib.h"
#include "tracer.h"
#include "nasspdefs.h"

SoundData::SoundData()

{
	refcount = 0;
	valid = false;
	id = (-1);
	filename[0] = 0;
	Soundlib = NULL;
}

SoundData::~SoundData()

{
	//
	// Nothing to do.
	//
}

bool SoundData::isValid()

{
	if (id < 0)
		return false;

	//
	// If the sound is playing then we must still claim to
	// be valid. Note that for this to work we must ensure
	// that the sound id won't be reused until the sound has
	// gone invalid.
	//

	if (isPlaying())
		return true;

	return valid;
}

bool SoundData::play(int flags, int libflags, int volume, int playvolume, int frequency /*= NULL*/)

{
	if (valid) {
		if (!Soundlib->PlayWav(id, (bool) flags, playvolume/255.0f))
		{
			return false;
		}

		PlayVolume = playvolume;
		PlayFlags = flags;
		LibFlags = libflags;
		BaseVolume = volume;

		return true;
	}

	return false;
}

void SoundData::done()

{
	refcount--;
	if (refcount <= 0) {
		valid = false;
	}
}

void SoundData::stop()

{
	if (!isPlaying())
		return;

	Soundlib->StopWav(id);
}

bool SoundData::isPlaying()

{
	if (id < 0)
		return false;

	return (Soundlib->IsWavPlaying(id) != 0);
}

bool SoundData::matches(char *s)

{
	if (!valid)
		return false;

	return (strcmp(s, filename) == 0);
}

void SoundData::setFileName(char *s)

{
	strncpy(filename, s, 255);
}

//
// Soundlib files.
//

SoundLib::SoundLib()

{
	int	i;

	for (i = 0; i <= MAX_SOUNDS; i++) {
		sounds[i].MakeInvalid();
	}

	Soundlib = NULL;
	XRSoundActive = 0;
	missionpath[0] = 0;
	basepath[0] = 0;
	strcpy(languagepath, "English");
	NextSlot = 1;

	for (i = 0; i < N_VOLUMES; i++) {
		MasterVolume[i] = 100;
	}
}

SoundLib::~SoundLib()

{
	//
	// Nothing for now.
	//
}

void SoundLib::InitSoundLib(VESSEL *v, char *soundclass)

{
	_snprintf(basepath, 255, "Sound/%s", soundclass);

	Soundlib = XRSound::CreateInstance(v);
	XRSoundActive = Soundlib->IsPresent();
}

void SoundLib::SetSoundLibMissionPath(char *mission)

{
	strncpy(missionpath, mission, 255);
}

//
// access() doesn't seem to exist on Windows, so we have to do this
// manky thing instead, of opening and then closing.
//

static bool IsFile(char *path)

{
	FILE	*fp;

	fp = fopen(path, "rb");
	if (!fp)
		return false;

	fclose(fp);
	return true;
}

SoundData *SoundLib::CheckForMatch(char *s)

{
	int	i;

	for (i = 1; i <= MAX_SOUNDS; i++) {
		if (sounds[i].matches(s))
			return sounds + i;
	}

	return 0;
}

int SoundLib::FindSlot()

{
	int	i;

	//
	// XRSound doesn't seem to like reusing slots much, so we won't do that until we run out
	// of slots. That won't happen too often unless you try to fly an entire mission without
	// quitting Orbiter and reloading.
	//

	if (NextSlot <= MAX_SOUNDS) {
		return NextSlot++;
	}

	//
	// If we've used all slots, look for an empty one.
	//

	for (i = 1; i <= MAX_SOUNDS; i++) {
		if (!(sounds[i].isValid()))
			return i;
	}

	//
	// Uh-oh, we're doomed.
	//

	return -1;
}

SoundData *SoundLib::DoLoadSound(char *SoundPath, EXTENDEDPLAY extended)

{
	SoundData *s;
	XRSound::PlaybackType t;

	switch (extended) {
	case INTERNAL_ONLY:
		t = XRSound::InternalOnly;
		break;
	case BOTHVIEW_FADED_CLOSE:
		t = XRSound::BothViewClose;
		break;
	case BOTHVIEW_FADED_MEDIUM:
		t = XRSound::BothViewMedium;
		break;
	case BOTHVIEW_FADED_FAR:
		t = XRSound::BothViewFar;
		break;
	case EXTERNAL_ONLY_FADED_CLOSE:
	case EXTERNAL_ONLY_FADED_MEDIUM:
	case EXTERNAL_ONLY_FADED_FAR:
		// Will hope these all will work with Wind
		t = XRSound::Wind;
		break;
	case DEFAULT:
	default:
		t = XRSound::Global;
	}

	//
	// If the sound already exists, return it.
	//

	s = CheckForMatch(SoundPath);
	if (s) {
		s->AddRef();
		return s;
	}

	int id = FindSlot();
	if (id < 0)
		return 0;

	s = sounds + id;

	s->setFileName(SoundPath);

	//
	// So the file exists and we have a free slot. Try to load it.
	//

	if (Soundlib->LoadWav(id, s->GetFilename(), t) == 0)
		return 0;


	s->setSoundlib(Soundlib);
	s->setID(id);
	s->MakeValid();
	s->AddRef();

	return s;
}

//
// This is used to load generic NCPP sound files. If they're not over-ridden by a file in the language-specific
// directory, they'll use the file from the NCPP directory. If that fails, they'll finally look in the Vessel
// directory.
//

void SoundLib::LoadSound(Sound &s, char *soundname, EXTENDEDPLAY extended)

{

	if (!XRSoundActive) {
		s.SetSoundData(0);
		return;
	}

	s.SetSoundLib(this);

	char SoundPath[256];

	//
	// First check for a language-specific sound.
	//

	_snprintf(SoundPath, 255, "%s/%s/%s", basepath, languagepath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Then check for the generic file.
	//

	_snprintf(SoundPath, 255, "%s/%s", basepath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Finally check the Vessel directory.
	//

	_snprintf(SoundPath, 255, "Sound/Vessel/%s", soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	s.SetSoundData(0);
}

//
// LoadMissionSound() looks for a mission-specific sound file. If it's not in the
// language-specific mission directory and not in the generic NCPP mission directory,
// then it will instead try to load the generic sound file if it's passed in.
//

void SoundLib::LoadMissionSound(Sound &s, char *soundname, char *genericname, EXTENDEDPLAY extended)

{
	char	SoundPath[256];

	if (!XRSoundActive) {
		s.SetSoundData(0);
		return;
	}

	s.SetSoundLib(this);

	//
	// First check for a language-specific sound.
	//

	_snprintf(SoundPath, 255, "%s/%s/%s/%s", basepath, languagepath, missionpath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// Then check for the generic file.
	//

	_snprintf(SoundPath, 255, "%s/%s/%s", basepath, missionpath, soundname);
	if (IsFile(SoundPath)) {
		s.SetSoundData(DoLoadSound(SoundPath, extended));
		return;
	}

	//
	// If none of the files exist and we're passed a generic alternative, try that.
	//

	if (genericname) {
		LoadSound(s, genericname, extended);
		return;
	}

	//
	// Nothing. Return a dummy.
	//

	s.SetSoundData(0);
}

//
// Load a sound from the generic Vessel directory. You shouldn't call this without a good reason.
//

void SoundLib::LoadVesselSound(Sound &s, char *soundname, EXTENDEDPLAY extended)

{
	char	SoundPath[256];

	if (!XRSoundActive) {
		s.SetSoundData(0);
		return;
	}

	s.SetSoundLib(this);

	_snprintf(SoundPath, 255, "Vessel/%s", soundname);
	s.SetSoundData(DoLoadSound(SoundPath, extended));
}

void SoundLib::SoundOptionOnOff(int option,BOOL status)

{
	// The right way to do this is to go through every vessel and modify
	// the option to be the enum, but that would mean including XRSound.h
	// to every vessel. I'll instead do a lookup here.
	switch (option) {
	case PLAYCOUNTDOWNWHENTAKEOFF:
		Soundlib->SetDefaultSoundEnabled(XRSound::Liftoff, status);
		break;
	case PLAYWHENATTITUDEMODECHANGE:
		Soundlib->SetDefaultSoundEnabled(XRSound::Rotation, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::Translation, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::Off, status);
		break;
	case PLAYDOCKINGSOUND:
		Soundlib->SetDefaultSoundEnabled(XRSound::Docking, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::DockingCallout, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::Undocking, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::UndockingCallout, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::DockingDistanceCalloutsGroup, status);
		break;
	case PLAYRADARBIP:
		Soundlib->SetDefaultSoundEnabled(XRSound::DockingRadarBeep, status);
		break;
	case PLAYLANDINGANDGROUNDSOUND:
		Soundlib->SetDefaultSoundEnabled(XRSound::Crash, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::MetalCrunch, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::WheelChirp, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::Touchdown, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::WheelStop, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::TiresRolling, status);
		Soundlib->SetDefaultSoundEnabled(XRSound::Wheekbrakes, status);
		break;
	case PLAYCABINAIRCONDITIONING:
		Soundlib->SetDefaultSoundEnabled(XRSound::AirConditioning, status);
		break;
	case PLAYCABINRANDOMAMBIANCE:
		Soundlib->SetDefaultSoundEnabled(XRSound::CabinAmbienceGroup, status);
		break;
	case PLAYRADIOATC:
		Soundlib->SetDefaultSoundEnabled(XRSound::RadioATCGroup, status);
		break;
	case DISPLAYTIMER:
		// XRSound doesn't have this
		break;
	}

	// Disable XRSound things while we're at it
	Soundlib->SetDefaultSoundEnabled(XRSound::AudioGreeting, false);
	Soundlib->SetDefaultSoundEnabled(XRSound::SubsonicCallout, false);
	Soundlib->SetDefaultSoundEnabled(XRSound::MachCalloutsGroup, false);
	Soundlib->SetDefaultSoundEnabled(XRSound::OneHundredKnots, false);
}

void SoundLib::SetLanguage(char *language)

{
	strncpy(languagepath, language, 255);
}

//
// Adjust the volume passed in based on the master volume controls.
//

int SoundLib::GetSoundVolume(int flags, int volume)

{
	if (flags & SOUNDFLAG_COMMS) {
		//
		// There are two master volume controls, so use the
		// highest volume set.
		//
		int mv = MasterVolume[VOLUME_COMMS];
		if (MasterVolume[VOLUME_COMMS2] > mv)
			mv = MasterVolume[VOLUME_COMMS2];

		volume = (volume * mv) / 100;
	}

	return volume;
}

//
// Update volume.
//

void SoundLib::SetVolume(int type, int percent)

{
	//
	// We'll be nice if the volume is invalid.
	//

	if (percent > 100)
		percent = 100;
	if (percent < 0)
		percent = 0;

	//
	// Return if the type is invalid.
	//

	if (type < 0)
		return;
	if (type >= N_VOLUMES)
		return;

	//
	// Store volume as a percentage of max.
	//

	MasterVolume[type] = percent;

#if 0

	//
	// Try to update any sounds that are currently playing.
	//

	//
	// Unfortunately this doesn't work here, even though it does work for
	// the cabin fans... maybe only looped sounds let you change the volume?
	//

	int match_flags = 0;

	switch (type) {
	case VOLUME_COMMS:
		match_flags = SOUNDFLAG_COMMS;
		break;

	default:
		return;
	}

	for (int i = 1; i <= MAX_SOUNDS; i++) {
		if (sounds[i].isValid() && sounds[i].isPlaying()) {
			int libflags = sounds[i].GetLibFlags();
			if (libflags & match_flags) {
				int base_vol = sounds[i].GetBaseVolume();
				int playflags = sounds[i].GetPlayFlags();
				int vol = GetSoundVolume(libflags, base_vol);

				sounds[i].play(playflags, libflags, base_vol, vol);
			}
		}
	}
#endif
}

Sound::Sound()

{
	sd = 0;
	sl = 0;
	valid = false;
	soundflags = 0;
}

Sound::Sound(Sound &s)

{
	sd = s.sd;
	sl = s.sl;

	valid = s.valid;

	if (sd)
		sd->AddRef();
}

Sound::~Sound()

{
	if (sd && valid && sd->isValid()) {
		sd->done();
	}
}

Sound &Sound::operator=(const Sound &s)

{
	if (this != &s)
	{
		if (sd && valid && sd->isValid())
		{
			sd->done();
		}

		soundflags = s.soundflags;
		valid = s.valid;
		sd = s.sd;

		sd->AddRef();
		sl = s.sl;
	}
	return *this;
}

void Sound::setFlags(int fl)

{
	soundflags |= fl;
}

void Sound::clearFlags(int fl)

{
	soundflags &= ~fl;
}

bool Sound::isValid()

{
	bool	v;

	v = valid;
	if (sd) {
		if (!sd->isValid())
			v = false;
	}

	return v;
}

bool Sound::isPlaying()

{
	if (!sd || !valid) {
		return false;
	}

	return sd->isPlaying();
}

bool Sound::play(int flags, int volume, int frequency /*= NULL*/)

{
	if (valid && sd && sd->isValid()) {
		if (soundflags & SOUNDFLAG_1XORLESS) {
			if (oapiGetTimeAcceleration() > 1.0)
				return false;
		}
		if (soundflags & SOUNDFLAG_1XONLY) {
			double acc = oapiGetTimeAcceleration();
			if (acc > 1.01 || acc < 0.99)
				return false;
		}

		int vol = volume;

		if (sl) {
			vol = sl->GetSoundVolume(soundflags, volume);
		}

		return sd->play(flags, soundflags, volume, vol, frequency);
	}

	return false;
}

void Sound::stop()

{
	if (valid && sd && sd->isValid())
		sd->stop();
}

void Sound::done()

{
	if (valid && sd && sd->isValid())
		sd->done();

	valid = false;
	sd = 0;
}

void Sound::SetSoundData(SoundData *s)

{
	if (valid && sd && sd->isValid())
		sd->done();

	sd = s;

	if (s) {
		valid = s->isValid();
	}
	else {
		valid = false;
	}
}

bool FadeInOutSound::play(int volume /*= 255*/)
{
	double dt; // [s]
	int freq;  // [Hz] placed here for debug sprintf() to work

	if (currentVolume == -1) { // initial call (first after scenario load) ?
		currentVolume = volume;
	}

	if (currentVolume < volume)
	{
		dt = oapiGetSimStep();
		currentVolume += int(round(double(riseSlope) * dt));
		if (currentVolume > volume) { currentVolume = volume; } // limit (upper)
	}
	else if (currentVolume > volume)
	{
		dt = oapiGetSimStep();
		currentVolume -= int(round(double(fadeSlope) * dt));
		if (currentVolume < 0) { currentVolume = 0; } // limit (lower)
	}

	if (currentVolume)
	{
		freq = hasFrequencyShift()
			? fMin + (currentVolume * (fMax - fMin) / 255)
			: NULL;
		Sound::play(LOOP, currentVolume, freq);
	}
	else
	{
		Sound::stop();
	}

	// sprintf(oapiDebugString(), "vol: %d/%d [%d Hz]", currentVolume, volume, freq);
	return currentVolume == volume;
}

void FadeInOutSound::stop()
{
	// We stop the sound only once (will do no harm and it will save a bit of processor time)
	if (currentVolume) {
		play(0);
	}
}

TimedSound::TimedSound()

{
	priority = 5;
	triggerTime = 0.0;
	soundname[0] = 0;

	next = 0;
	prev = 0;
}

TimedSound::~TimedSound()

{
}

void TimedSound::AddToSoundQueue(TimedSound *previous)

{
	if (previous)
	{
		if (previous->next) {
			next = previous->next;
			next->prev = this;
		}
		previous->next = this;
	}

	prev = previous;
}

void TimedSound::RemoveFromQueue()

{
	if (prev)
	{
		prev->next = next;
	}

	if (next)
	{
		next->prev = prev;
	}

	prev = 0;
	next = 0;
}

void TimedSound::SetFilename(char *s)

{
	strncpy (soundname, s, 255);
}

void TimedSound::SetPriority(int n)

{
	if (n < 0)
		n = 0;

	if (n > 9)
		n = 9;

	priority = n;
}

TimedSoundManager::TimedSoundManager(SoundLib &s) : soundlib(s)

{
	launchRelativeList = 0;
	reentryRelativeList = 0;

	LaunchSoundsLoaded = false;
	SoundToPlay = false;

	TimeToPlay = MINUS_INFINITY;
}

//
// When deleted, delete all the timed sounds too.
//
TimedSoundManager::~TimedSoundManager()

{
	TimedSound *s = launchRelativeList;
	while (s)
	{
		TimedSound *n = s->GetNext();

		delete s;

		s = n;
	}
}

void TimedSoundManager::Timestep(double simt, double simdt, bool autoslow)

{
	// Is XRSound available?
	if (!soundlib.IsXRSoundActive()) return;

	double timeaccel = oapiGetTimeAcceleration();
	if (LaunchSoundsLoaded && simt >= TimeToPlay)
	{
		if (SoundToPlay)
		{	
			if (timeaccel > 1.0) 
			{
				if (SoundIsInformational || (!SoundIsMandatory && !autoslow))
				{
					SoundToPlay = false;
				}
				else
				{
					oapiSetTimeAcceleration(1.0);
				}
			}

			//
			// Stop previous sound if need be.
			//

			if (currentSound.isPlaying())
				currentSound.stop();

			currentSound.done();

			//
			// Save away the new sound so we can stop it if time acceleration changes.
			//
			currentSound = nextSound;

			if (SoundToPlay)
			{
				currentSound.play();
			}

			SoundToPlay = false;
		}

		//
		// Now load the next sound.
		//

		if (launchRelativeList)
		{
			nextSound.done();

			soundlib.LoadMissionSound(nextSound, launchRelativeList->GetFilename(), launchRelativeList->GetFilename());

			SoundToPlay = true;
			TimeToPlay = launchRelativeList->GetPlayTime();
			SoundIsMandatory = launchRelativeList->IsMandatory();
			SoundIsInformational = launchRelativeList->IsInformational();

			TimedSound *n = launchRelativeList->GetNext();
			launchRelativeList->RemoveFromQueue();

			delete launchRelativeList;

			launchRelativeList = n;
		}
		else
		{
			//
			// Reached the end of the list.
			//

			LaunchSoundsLoaded = false;
			currentSound.done();
		}
	}
	else if (timeaccel > 1.0 && currentSound.isPlaying())
	{
		currentSound.stop();
		currentSound.done();
	}
}

static int CharToInt(char c)

{
	if (c >= '0' && c <= '9')
		return c - '0';

	return 0;
}

void TimedSoundManager::LoadFromFile(char *dataFile, double MissionTime)

{
	char filePath[256];

	_snprintf(filePath, 255, "%s/%s/%s", soundlib.basepath,
		                                    soundlib.missionpath, dataFile);

	FILE *fp = fopen(filePath, "rt");

	if (!fp)
	{
		return;
	}

	char line[256];
	bool copying;
	int i, c;
	TimedSound *lastLRSound = 0;
	TimedSound *lastRRSound = 0;

	while (!feof(fp))
	{
		copying = true;

		//
		// Read in a line.
		//

		i = 0;
		while ((c = fgetc(fp)) != '\n' && c != EOF) {
			if (c == '#')
				copying = false;

			if (copying)
			{
				line[i++] = c;
			}
		}

		line[i] = 0;

		if (i > 0)
		{
			int hours = 0, minutes = 0, seconds = 0;
			double simt = 0.0;

			bool timeNegative = false;

			i = 0;

			//
			// Get hours.
			//

			if (line[i] == '-')
			{
				timeNegative = true;
				i++;
			}

			while (line[i] != ':' && line[i])
			{
				hours = (hours * 10) + CharToInt(line[i]);
				i++;
			}

			if (!line[i++])
				continue;

			//
			// Get minutes.
			//

			while (line[i] != ':' && line[i])
			{
				minutes = (minutes * 10) + CharToInt(line[i]);
				i++;
			}

			if (!line[i++])
				continue;

			//
			// Get seconds.
			//

			while (line[i] != ';' && line[i])
			{
				seconds = (seconds * 10) + CharToInt(line[i]);
				i++;
			}

			simt = (double) seconds + ((double) minutes) * 60.0 + ((double) hours * 3600.0);

			if (timeNegative)
			{
				simt = (-simt);
			}

			if (simt >= MissionTime)
			{
				if (!line[i++])
					continue;

				bool launchRelative = (line[i] == 'l' || line[i] == 'L');

				if (!line[i++])
					continue;

				if (line[i++] != ';')
					continue;

				int priority = CharToInt(line[i]);

				if (!line[i++])
					continue;

				if (line[i++] != ';')
					continue;

				char filename[256];

				strncpy(filename, line + i, 250);
				strcat(filename, ".wav");

				TimedSound *t = new TimedSound;

				t->SetFilename(filename);
				t->SetPriority(priority);
				t->SetPlayTime(simt);

				if (launchRelative)
				{
					t->AddToSoundQueue(lastLRSound);
					lastLRSound = t;

					if (!launchRelativeList)
					{
						launchRelativeList = t;
					}
				}
				else
				{
					t->AddToSoundQueue(lastRRSound);
					lastRRSound = t;

					if (!reentryRelativeList)
					{
						reentryRelativeList = t;
					}
				}
			}
		}
	}

	if (launchRelativeList || reentryRelativeList)
		LaunchSoundsLoaded = true;

	fclose(fp);
}
