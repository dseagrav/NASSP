/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: Basic Apollo Guidance computer setup

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
#include "inttypes.h"
#include <stdio.h>
#include <math.h>
#include "soundlib.h"

#include "yaAGC/agc_engine.h"
#include "ioChannels.h"

#include "nasspdefs.h"
#include "apolloguidance.h"
#include "dsky.h"
#include "IMU.h"
#include "cdu.h"
#include "powersource.h"
#include "papi.h"

#include "tracer.h"

ApolloGuidance::ApolloGuidance(SoundLib &s, DSKY &display, IMU &im, CDU &sc, CDU &tc, PanelSDK &p) : soundlib(s), dsky(display), imu(im), DCPower(0, p), scdu(sc), tcdu(tc)

{
	Reset = false;
	CurrentTimestep = 0;
	LastTimestep = 0;
	LastCycled = 0;
	AGCHeat = NULL;

	OtherVesselName[0] = 0;

	//
	// Clear channels.
	//

	int i;

	for (i = 0; i <= MAX_OUTPUT_CHANNELS; i++)
		OutputChannel[i] = 0;

	//
	// Dsky interface.
	//

	dsky2 = NULL;

	isFirstTimestep = true;
	PadLoaded = false;

	ProgAlarm = false;
	TrackerAlarm = false;
	GimbalLockAlarm = false;

	//
	// Virtual AGC.
	//
	memset(&vagc, 0, sizeof(vagc));
	vagc.agc_clientdata = this;
	agc_engine_init(&vagc, NULL, NULL, 0);

#ifdef _DEBUG
	out_file = fopen("ProjectApollo AGC.log", "wt");
	vagc.out_file = out_file;
#endif
}

ApolloGuidance::~ApolloGuidance()

{
#ifdef _DEBUG
	fclose(out_file);
#endif
}

void ApolloGuidance::InitVirtualAGC(char *binfile)

{

	(void) agc_load_binfile(&vagc, binfile);

	// Set channels only once, otherwise this code overwrites the channel values in the scenario
	if (!PadLoaded) { 
		ChannelValue val30;
		ChannelValue val31;
		ChannelValue val32;
		ChannelValue val33;

		//
		// Set default state. Note that these are oddities, as zero means
		// true and one means false for the 'Virtual AGC'!
		//

		val30 = 077777;
		// Enable to turn on
		// val30.Bits.IMUOperate = 0;
		val30.reset(TempInLimits);

		//
		// We default to the IMU turned off. If you change this, change the IMU code to
		// match.
		//
		val30.set(IMUOperate);

		vagc.InputChannel[030] = (int16_t)val30.to_ulong();

		val31 = 077777;
		// Default position of the CMC MODE switch is FREE
		val31[FreeFunction] = 0;

		vagc.InputChannel[031] = (int16_t)val31.to_ulong();


		val32 = 077777;
		vagc.InputChannel[032] = (int16_t)val32.to_ulong();

		val33 = 077777;
	//	val33.Bits.RangeUnitDataGood = 0;
	//	val33.Bits.BlockUplinkInput = 0;

		//
		// Setting AGCWarning is needed to avoid the 1107 alarm on startup. It basically forces the AGC to do a
		// hard reset in the RESTART code.
		//
		//	CA	BIT14			# IF AGC WARNING ON (BIT = 0), DO A FRESH
		//	EXTEND				# START ON THE ASSUMPTION THAT WE'RE IN A
		//	RAND	CHAN33		# RESTART LOOP.
		//

		val33[AGCWarning] = 0;
		
		vagc.InputChannel[033] = (int16_t)val33.to_ulong();
	}
}

//
// Force a hardware restart.
//

void ApolloGuidance::ForceRestart()

{
	Reset = false;
}

bool ApolloGuidance::OutOfReset()

{
	return true;
}


// Do a single timestep - Used by CM to maintain sync between telemetry and vAGC.
bool ApolloGuidance::SingleTimestepPrep(double simt, double simdt){
	LastTimestep = CurrentTimestep;
	CurrentTimestep = simt;
	return TRUE;
}

bool ApolloGuidance::SingleTimestep() {

	agc_engine(&vagc);
	return TRUE;
}

void ApolloGuidance::VirtualAGCCoreDump(char *fileName) {

	MakeCoreDump(&vagc, fileName); 
}

bool ApolloGuidance::GenericTimestep(double simt, double simdt)
{
//	TRACESETUP("COMPUTER TIMESTEP");
	int i;

	LastTimestep = CurrentTimestep;
	CurrentTimestep = simt;

	// Physical AGC timing was generated from a master 1024 KHz clock, divided by 12.
	// This resulted in a machine cycle of just over 11.7 microseconds.
	int cycles = (long) ((simdt) * 1024000 / 12);

	for (i = 0; i < cycles; i++) {
		agc_engine(&vagc);
	}

	return true;
}

void ApolloGuidance::SystemTimestep(double simdt) 

{
	if (!IsPowered()) return;

	if (OnStandby()) {
		//DCPower.DrawPower(22.9);
		DCPower.DrawPower(15.0);  //From 1970 AGC Improvement Study
		if (AGCHeat)
		{
			AGCHeat->GenerateHeat(20.0);  //A guess based on heat load vs power in LM 8 systems handbook
		}
	}
	else {
		//DCPower.DrawPower(106.0);
		DCPower.DrawPower(70.0); //From 1970 AGC Improvement Study, LM-8 Handbook has 80W
		if (AGCHeat)
		{
			AGCHeat->GenerateHeat(110.0);  //From LM 8 systems handbook, for LGC/DSKY cb
		}
	}
}

void ApolloGuidance::SetMissionInfo(std::string ProgramName, char *OtherName)
{
	this->ProgramName = ProgramName; 

	if (OtherName != 0)
		strncpy(OtherVesselName, OtherName, 64);
}

void ApolloGuidance::SetOtherVesselName(char *OtherName)
{
	if (OtherName != 0)
		strncpy(OtherVesselName, OtherName, 64);
}

//
// Virtual AGC Erasable memory functions.
//


int ApolloGuidance::GetErasable(int bank, int address)

{
	if (bank < 0 || bank > 8)
		return 0;
	if (address < 0 || address > 0400)
		return 0;

	return vagc.Erasable[bank][address];
}

void ApolloGuidance::SetErasable(int bank, int address, int value)

{
	if (bank < 0 || bank > 8)
		return;
	if (address < 0 || address > 0400)
		return;

	vagc.Erasable[bank][address] = value;
}

void ApolloGuidance::PulsePIPA(int RegPIPA, int pulses) 

{
	int i;
	//
	// No pulsing, Don't lock the thread mutex. Locking the mutex here slows time acceleration, like single thread.
	// There should not be any pulsing during time acceleration anyways. i.e. no thruster firing. Still locking
	// the mutex in case the agc does small maneuvering during time acceleration.
	//
	if (pulses == 0 ) 
		return;

	Lock lock(agcCycleMutex);


	if (pulses >= 0) {
    	for (i = 0; i < pulses; i++) {
			UnprogrammedIncrement(&vagc, RegPIPA, 0);	// PINC

    	}
	} else {
    	for (i = 0; i < -pulses; i++) {
			UnprogrammedIncrement(&vagc, RegPIPA, 2);	// MINC
    	}
	}

}

//
// State save/load routines.
//

//
// Note that this state structure is now full! If you want to add a bit you'll have to add it to
// a new structure.
//

typedef union

{
	struct {
		unsigned Reset:1;
		unsigned isFirstTimestep:1;
		unsigned ExtraCode:1;
		unsigned AllowInterrupt:1;
		unsigned InIsr:1;
		unsigned SubstituteInstruction:1;
		unsigned PendFlag:1;
		unsigned PendDelay:3;
		unsigned ExtraDelay:3;
		unsigned DownruptTimeValid:1;
		unsigned PadLoaded:1;
		unsigned NightWatchman:1;
		unsigned RuptLock:1;
		unsigned NoRupt:1;
		unsigned TCTrap:1;
		unsigned NoTC:1;
		unsigned Standby:1;
		unsigned SbyPressed:1;
		unsigned SbyStillPressed:1;
		unsigned ParityFail:1;
		unsigned Spare1:1;
		unsigned NightWatchmanTripped:1;
		unsigned GeneratedWarning:1;
		unsigned TookBZF:1;
		unsigned TookBZMF:1;
		unsigned Trap31A:1;
		unsigned Trap31B:1;
		unsigned Trap32:1;
	} u;
	unsigned long word;
} AGCState;

void ApolloGuidance::SaveState(FILEHANDLE scn)

{
	char fname[32], str[32], buffer[256];
	int i;
	int val;

	oapiWriteLine(scn, AGC_START_STRING);

	if (OtherVesselName[0])
		oapiWriteScenario_string(scn, "ONAME", OtherVesselName);

	//
	// Copy internal state to the structure.
	//

	AGCState state;

	state.word = 0;
	state.u.Reset = Reset;
	state.u.isFirstTimestep = isFirstTimestep;
	state.u.ExtraCode = vagc.ExtraCode;
	state.u.AllowInterrupt = vagc.AllowInterrupt;
	state.u.InIsr = vagc.InIsr;
	state.u.SubstituteInstruction = vagc.SubstituteInstruction;
	state.u.PendFlag = vagc.PendFlag;
	state.u.PendDelay = vagc.PendDelay;
	state.u.ExtraDelay = vagc.ExtraDelay;
	state.u.DownruptTimeValid = vagc.DownruptTimeValid;
	state.u.PadLoaded = PadLoaded;
	state.u.NightWatchman = vagc.NightWatchman;
	state.u.RuptLock = vagc.RuptLock;
	state.u.NoRupt = vagc.NoRupt;
	state.u.TCTrap = vagc.TCTrap;
	state.u.NoTC = vagc.NoTC;
	state.u.Standby = vagc.Standby;
	state.u.SbyPressed = vagc.SbyPressed;
	state.u.SbyStillPressed = vagc.SbyStillPressed;
	state.u.ParityFail = vagc.ParityFail;
	state.u.NightWatchmanTripped = vagc.NightWatchmanTripped;
	state.u.GeneratedWarning = vagc.GeneratedWarning;
	state.u.TookBZF = vagc.TookBZF;
	state.u.TookBZMF = vagc.TookBZMF;
	state.u.Trap31A = vagc.Trap31A;
	state.u.Trap31B = vagc.Trap31B;
	state.u.Trap32 = vagc.Trap32;

	oapiWriteScenario_int(scn, "STATE", state.word);

	//
	// Write out any non-zero EMEM state.
	//

	for (i = 0; i < EMEM_ENTRIES; i++) {
		// Always save RegZ because it's set in agc_engine_init, so we have to store 0, too
		if (ReadMemory(i, val) && (val != 0 || i == RegZ)) {
			sprintf(fname, "EMEM%04o", i);
			sprintf(str, "%o", val);
			oapiWriteScenario_string (scn, fname, str);
		}
	}

	//
	// And non-zero I/O state.
	//

	for (i = 0; i < MAX_OUTPUT_CHANNELS; i++) {
		val = GetOutputChannel(i);
		if (val != 0) {
			sprintf(fname, "OCHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	for (i = 0; i < NUM_CHANNELS; i++) {
		val = vagc.InputChannel[i];
		// Always save channel 030 - 033 because they're set in agc_engine_init, so we have to store 0, too
		if (val != 0 || (i >= 030 && i <= 033)) {
			sprintf(fname, "VICHAN%03d", i);
			oapiWriteScenario_int (scn, fname, val);
		}
	}

	oapiWriteScenario_int (scn, "VOC7", vagc.OutputChannel7);
	oapiWriteScenario_int (scn, "IDXV", vagc.IndexValue);
	oapiWriteScenario_int (scn, "NEXTZ", vagc.NextZ);
	oapiWriteScenario_int (scn, "SCALERCOUNTER", vagc.ScalerCounter);
	oapiWriteScenario_int(scn, "SCALER", vagc.Scaler);
	oapiWriteScenario_int(scn, "SCALERCHANGED", vagc.ScalerChanged ? 1 : 0);
	oapiWriteScenario_int (scn, "CRCOUNT", vagc.ChannelRoutineCount);
	oapiWriteScenario_int(scn, "DSKYCHANNEL163", vagc.DskyChannel163);
	oapiWriteScenario_int(scn, "WARNINGFILTER", vagc.WarningFilter);

	sprintf(buffer, "  CYCLECOUNTER %I64d", vagc.CycleCounter);
	oapiWriteLine(scn, buffer);
		
	for (i = 0; i < 16; i++) {
		val = vagc.OutputChannel10[i];
		sprintf(fname, "V10CHAN%03d", i);
		oapiWriteScenario_int (scn, fname, val);
	}

	for (i = 0; i < (1 + NUM_INTERRUPT_TYPES); i++) {
		val = vagc.InterruptRequests[i];
		sprintf(fname, "VINT%03d", i);
		oapiWriteScenario_int (scn, fname, val);
	}

	papiWriteScenario_bool(scn, "PROGALARM", ProgAlarm);
	papiWriteScenario_bool(scn, "TRACKERALARM", TrackerAlarm);
	papiWriteScenario_bool(scn, "GIMBALLOCKALARM", GimbalLockAlarm);

	oapiWriteLine(scn, AGC_END_STRING);
}

void ApolloGuidance::LoadState(FILEHANDLE scn)

{
	char	*line;

	//
	// Now load the data.
	//

	boolean  //Flag to keep compatibility with older scenario files.
		scalerLoaded = false 
		;

	while (oapiReadScenario_nextline (scn, line)) {
		if (!strnicmp(line, AGC_END_STRING, sizeof(AGC_END_STRING)))
			break;
			
		if (!strnicmp (line, "EMEM", 4)) {
			int num, val;
			sscanf(line+4, "%o", &num);
			sscanf(line+9, "%o", &val);
			WriteMemory(num, val);
		}
		else if (!strnicmp (line, "VICHAN", 6)) {
			int num;
			unsigned int val;
			sscanf(line+6, "%d", &num);
			sscanf(line+10, "%d", &val);
			vagc.InputChannel[num] = val;
		}
		else if (!strnicmp (line, "V10CHAN", 7)) {
			int num;
			unsigned int val;
			sscanf(line+7, "%d", &num);
			sscanf(line+11, "%d", &val);
			vagc.OutputChannel10[num] = val;
		}
		else if (!strnicmp (line, "OCHAN", 5)) {
			int num;
			unsigned int val;
			sscanf(line+5, "%d", &num);
			sscanf(line+9, "%d", &val);
			OutputChannel[num] = val;
		}
		else if (!strnicmp (line, "VOC7", 4)) {
			sscanf (line+4, "%" SCNd16, &vagc.OutputChannel7);
		}
		else if (!strnicmp (line, "IDXV", 4)) {
			sscanf (line+4, "%" SCNd16, &vagc.IndexValue);
		}
		else if (!strnicmp (line, "NEXTZ", 5)) {
			sscanf (line+5, "%d", &vagc.NextZ);
		}
		else if (!strnicmp (line, "SCALERCOUNTER", 13)) {
			sscanf (line+13, "%d", &vagc.ScalerCounter);
		}
		else if (!strnicmp(line, "SCALER", 6)) {
			sscanf(line + 6, "%d", &vagc.Scaler);
			scalerLoaded = true;
		}
		else if (!strnicmp(line, "SCALERCHANGED", 6)) {
			int tmp;
			sscanf(line + 6, "%d", &tmp);
			vagc.ScalerChanged = tmp;
		}
		else if (!strnicmp (line, "CRCOUNT", 7)) {
			sscanf (line+7, "%d", &vagc.ChannelRoutineCount);
		}
		else if (!strnicmp(line, "DSKYCHANNEL163", 14)) {
			sscanf(line + 14, "%d", &vagc.DskyChannel163);
		}
		else if (!strnicmp(line, "WARNINGFILTER", 13)) {
			sscanf(line + 13, "%" SCNd32, &vagc.WarningFilter);
		}
		/*
		TODO Do NOT load CycleCounter until CduFifos are saved/loaded, too
		else if (!strnicmp (line, "CYCLECOUNTER", 12)) {
			sscanf (line+12, "%I64d", &vagc.CycleCounter);
		}
		*/
		else if (!strnicmp (line, "VINT", 4)) {
			int num;
			unsigned int val;
			sscanf(line+4, "%d", &num);
			sscanf(line+8, "%d", &val);
			vagc.InterruptRequests[num] = val;
		}
		else if (!strnicmp (line, "STATE", 5)) {
			AGCState state;
			sscanf (line+5, "%d", &state.word);

			Reset = state.u.Reset;
			isFirstTimestep = (state.u.isFirstTimestep != 0);
			vagc.ExtraCode = state.u.ExtraCode;
			vagc.AllowInterrupt = state.u.AllowInterrupt;
			vagc.InIsr = state.u.InIsr;
			vagc.SubstituteInstruction = state.u.SubstituteInstruction;
			vagc.PendFlag = state.u.PendFlag;
			vagc.PendDelay = state.u.PendDelay;
			vagc.ExtraDelay = state.u.ExtraDelay;
			vagc.DownruptTimeValid = state.u.DownruptTimeValid;
			PadLoaded = state.u.PadLoaded;
			vagc.NightWatchman = state.u.NightWatchman;
			vagc.RuptLock = state.u.RuptLock;
			vagc.NoRupt = state.u.NoRupt;
			vagc.TCTrap = state.u.TCTrap;
			vagc.NoTC = state.u.NoTC;
			vagc.Standby = state.u.Standby;
			vagc.SbyPressed = state.u.SbyPressed;
			vagc.SbyStillPressed = state.u.SbyStillPressed;
			vagc.ParityFail = state.u.ParityFail;
			vagc.NightWatchmanTripped = state.u.NightWatchmanTripped;
			vagc.GeneratedWarning = state.u.GeneratedWarning;
			vagc.TookBZF = state.u.TookBZF;
			vagc.TookBZMF = state.u.TookBZMF;
			vagc.Trap31A = state.u.Trap31A;
			vagc.Trap31B = state.u.Trap31B;
			vagc.Trap32 = state.u.Trap32;
		}
		else if (!strnicmp (line, "ONAME", 5)) {
			strncpy (OtherVesselName, line + 6, 64);
		}

		papiReadScenario_bool(line, "PROGALARM", ProgAlarm);
		papiReadScenario_bool(line, "TRACKERALARM", TrackerAlarm);
		papiReadScenario_bool(line, "GIMBALLOCKALARM", GimbalLockAlarm);
	}

	if (!scalerLoaded)
		vagc.Scaler = (vagc.InputChannel[ChanSCALER1] << 2) | (vagc.InputChannel[ChanSCALER2] << 16);
}

//
// Power.
//

bool ApolloGuidance::IsPowered()

{
	if (DCPower.Voltage() > SP_MIN_DCVOLTAGE)
		return true;

	return false;
}

//
// I/O channel support code.
//
// Note that the AGC 'bit 1' is actually 'bit 0' in today's terminology, so we have
// to adjust the bit number here to match the real AGC.
//

bool ApolloGuidance::GetOutputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return false;

	int val = OutputChannel[channel];
	return (OutputChannel[channel] & (1 << (bit))) != 0;
}

unsigned int ApolloGuidance::GetOutputChannel(int channel)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return 0;

	return OutputChannel[channel];
}

void ApolloGuidance::SetInputChannel(int channel, ChannelValue val)
{
	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

#ifdef _DEBUG
	//
	// Don't print debug for IMU channels or we get a multi-gigabyte log file!
	//
	if (!(channel & 0x80))
		fprintf(out_file, "Wrote %05o to input channel %04o\n", channel, val);
#endif

	if (channel & 0x80) {
		// In this case we're dealing with a counter increment.
		// So increment the counter.
		UnprogrammedIncrement (&vagc, channel, val.to_ulong());
	}
	else {
		// If this is a keystroke from the DSKY, generate an interrupt req.
		if (channel == 015){
			vagc.InterruptRequests[5] = 1;
		}else{ if (channel == 016){ // Secondary DSKY
			vagc.InterruptRequests[6] = 1;
		}}

		//
		// Channels 030-034 are inverted!
		//

		if (channel >= 030 && channel <= 034){
			val ^= 077777;
		}

		WriteIO(&vagc, channel, val.to_ulong());
	}
}

void ApolloGuidance::SetInputChannelBit(int channel, int bit, bool val)

{
	unsigned int mask = (1 << (bit));
	int	data = vagc.InputChannel[channel];

	//
	// Channels 030-034 are inverted!
	//

	if ((channel >= 030) && (channel <= 034))
		data ^= 077777;

#ifdef _DEBUG
		fprintf(out_file, "Set bit %d of input channel %04o to %d\n", bit, channel, val ? 1 : 0); 
#endif

	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return;

	if (val) {
		data |= mask;
	}
	else {
		data &= ~mask;
	}

	//
	// Do nothing if we have no power.
	//
	if (!IsPowered())
		return;

	//
	// Channels 030-034 are inverted!
	//

	if ((channel >= 030) && (channel <= 034))
		data ^= 077777;

	// If this is a keystroke from the DSKY (Or MARK/MARKREJ), generate an interrupt req.
	if (channel == 015 && val != 0){
		vagc.InterruptRequests[5] = 1;
	}else{ if (channel == 016 && val != 0){ // Secondary DSKY
		vagc.InterruptRequests[6] = 1;
	}}

	WriteIO(&vagc, channel, data);
}

void ApolloGuidance::SetOutputChannel(int channel, ChannelValue val)

{
	if (channel < 0 || channel > MAX_OUTPUT_CHANNELS)
		return;

	OutputChannel[channel] = val.to_ulong();

#ifdef _DEBUG
	switch (channel) {
	case 010:
	case 034:
	case 035:
	case 01:
	case 02:
		break;

	default:
		fprintf(out_file, "AGC write %05o to %04o\n", val, channel);
		break;
	}
#endif

	//
	// Special-case processing.
	//

	switch (channel)
	{
	case 05:
		ProcessChannel5(val);
		break;

	case 06:
		ProcessChannel6(val);
		break;

	case 010:
		ProcessChannel10(val);
		break;

	case 011:
		ProcessChannel11(val);
		break;

	case 013:
		ProcessChannel13(val);
		break;

	case 014:
		{
			// DS20060225 Enable SPS gimbal control
			// TVC Enable does not disconnect the IMU from this channel			
			// (Even though it probably doesn't matter)
			imu.ChannelOutput(channel, val);
			// DS20060829 Allow other stuff too
			ProcessChannel14(val);
		}
		break;

	// Various control bits
	case 012:
		scdu.ChannelOutput(channel, val);
		tcdu.ChannelOutput(channel, val);
	// 174-177 are ficticious channels with the IMU CDU angles.
	case 0174:  // FDAI ROLL CHANNEL
	case 0175:  // FDAI PITCH CHANNEL
	case 0176:  // FDAI YAW CHANNEL
	case 0177:		
		ProcessIMUCDUErrorCount(channel, val);
		imu.ChannelOutput(channel, val);
		break;
	case 0140:
		scdu.ChannelOutput(channel, val);
		break;
	case 0141:
		tcdu.ChannelOutput(channel, val);
		break;
	case 0142:
		ProcessChannel142(val);
		break;
	case 0143:
		ProcessChannel143(val);
		break;
	case 0163:	//Virtual AGC DSKY stuff
		ProcessChannel163(val);
		break;

	case 033: 
		/* No longer needed DS20061226
		{			
			ChannelValue33 val33;
			val33.Value = val;
		} */
		break;
	case 034:
		ProcessChannel34(val);
		break;
	}
}

//
// By default, do nothing for the RCS channels.
//

void ApolloGuidance::ProcessChannel5(ChannelValue val){
}

void ApolloGuidance::ProcessChannel6(ChannelValue val){
}

// DS20060226 Stubs for optics controls and TVC
void ApolloGuidance::ProcessChannel14(ChannelValue val){
}

void ApolloGuidance::ProcessChannel34(ChannelValue val) {
}

// Stub for LGC thrust drive
void ApolloGuidance::ProcessChannel142(ChannelValue val) {
}

// Stub for LGC altitude meter drive
void ApolloGuidance::ProcessChannel143(ChannelValue val) {
}

// DS20060308 Stub for FDAI
void ApolloGuidance::ProcessIMUCDUErrorCount(int channel, ChannelValue val){
}

void ApolloGuidance::ProcessIMUCDUReadCount(int channel, int val) {
}

void ApolloGuidance::GenerateHandrupt() {
	vagc.InterruptRequests[10] = 1;
}

void ApolloGuidance::GenerateDownrupt(){
	vagc.InterruptRequests[8] = 1;
}

void ApolloGuidance::GenerateUprupt(){
	vagc.InterruptRequests[7] = 1;
}

void ApolloGuidance::GenerateRadarupt(){
	vagc.InterruptRequests[9] = 1;
}

bool ApolloGuidance::IsUpruptActive() {
	// UPRUPT waiting to be processed
	if (vagc.InterruptRequests[7] == 1)
		return 1;

	// UPRUPT currently being processed
	if (vagc.InIsr && vagc.InterruptRequests[0] == 7)
		return 1;

	return 0;
}

bool ApolloGuidance::GetInputChannelBit(int channel, int bit)

{
	if (channel < 0 || channel > MAX_INPUT_CHANNELS)
		return false;

	return (GetInputChannel(channel) & (1 << (bit))) != 0;
}

unsigned int ApolloGuidance::GetInputChannel(int channel)

{
	if (channel < 0 || channel >= NUM_CHANNELS)
		return 0;

	//
	// Virtual AGC code stores values in native form. C++ AGC expects to read them out in
	// 0 = false, 1 = true form.
	//

	unsigned int val = vagc.InputChannel[channel];
	
	if ((channel >= 030) && (channel <= 034))
		val ^= 077777;

	return val;
}

bool ApolloGuidance::GenericReadMemory(unsigned int loc, int &val)

{
	int bank, addr;

	bank = (loc / 0400);
	addr = loc - (bank * 0400);

	if (bank >= 0 && bank < 8) {
		val = vagc.Erasable[bank][addr];
		return true;
	}

	val = 0;
	return true;

}

//
// Load a PAD value into the AGC. Used for initialising the LEM when created.
//

void ApolloGuidance::PadLoad(unsigned int address, unsigned int value)

{
	WriteMemory(address, value);
}

void ApolloGuidance::GenericWriteMemory(unsigned int loc, int val)

{
	int bank, addr;

	bank = (loc / 0400);
	addr = loc - (bank * 0400);

	if (bank >= 0 && bank < 8)
		vagc.Erasable[bank][addr] = val;
	return;
}

int16_t ApolloGuidance::ConvertDecimalToAGCOctal(double x, bool highByte) 

{
	int sign = 0, value = 0, i = 0;

	if (x < 0) {
		sign = 1;
		x = -x;
    }
	
	if (x >= 1.0) {
		// Illegal value: Must be <1.0 according to AGC rules
		return -1;
    }

	for (value = 0, i = 0; i < 28; i++) {
		value = value << 1;
		if (x >= 0.5) {
			value++;
			x -= 0.5;
		}
		x *= 2;
    }
  
	if (x >= 0.5)
		value++;

	i = value & 0x00003fff;
	value = (value >> 14) & 0x00003fff;
	if (sign) {
		value = ~value;
		i = ~i;
		i &= 0x00007fff;
		value &= 0x00007fff;
	}

	if (highByte)
		return value;
	else
		return i;
}


//
// Virtual AGC functions.
//


//-----------------------------------------------------------------------------
// Function for broadcasting "output channel" data to all connected clients.

void ChannelOutput (agc_t * State, int Channel, int Value) 

{

  // Some output channels have purposes within the CPU, so we have to
  // account for those separately.
  if (Channel == 7)
    {
      State->InputChannel[7] = State->OutputChannel7 = (Value & 0160);
      return;
    }
  // Most output channels are simply transmitted to clients representing
  // hardware simulations.

  ApolloGuidance *agc;

  agc = (ApolloGuidance *) State->agc_clientdata;
  agc->SetOutputChannel(Channel, Value);
}

void ShiftToDeda (agc_t *State, int Data)

{
	// Nothing for now.
}

//
// Do nothing here. We'll process input seperately.
//

int ChannelInput (agc_t *State)

{
	return 0;
}

void ChannelRoutine (agc_t *State)

{
}

