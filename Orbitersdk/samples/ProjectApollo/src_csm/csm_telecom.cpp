/****************************************************************************
  This file is part of Project Apollo - NASSP

  CSM Telecommunications
  
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
#include "saturn.h"
#include "LEM.h"
#include "ioChannels.h"
#include "tracer.h"
#include "Mission.h"

// DS20060326 TELECOM OBJECTS

// PREMODULATION PROCESSOR
PMP::PMP(){
	sat = NULL;
}

void PMP::Init(Saturn *vessel){
	sat = vessel;
}

void PMP::SystemTimestep(double simdt) {
	if (sat->PMPSwitch.GetState() == THREEPOSSWITCH_UP) {
		if (sat->PMPPowerPrimCB.Voltage() > 18) {
			sat->PMPPowerPrimCB.DrawPower(8.5);
		}
	}
	else if (sat->PMPSwitch.GetState() == THREEPOSSWITCH_DOWN) {
		if (sat->PMPPowerAuxCB.Voltage() > 18) {
			sat->PMPPowerAuxCB.DrawPower(8.5);
		}
	}
}

void PMP::TimeStep(double simt){

}

double SBandAntenna::dBm2SignalStrength(double dBm)
{
	double SignalStrength;

	if (dBm >= -50)
	{
		SignalStrength = 100.0;
	}
	else if (dBm <= -130)
	{
		SignalStrength = 0.0;
	}
	else
	{
		SignalStrength = (130.0 + dBm)*1.25; //convert from dBm to the 0-100% signal strength used in NASSP
	}

	return SignalStrength;
}

// Unifed S-Band System
USB::USB(){
	sat = NULL;
	ant = NULL;
	fm_ena = 1; pa_ovr_1 = 1; pa_ovr_2 = 1;
	pa_mode_1 = 0; pa_timer_1 = 0;
	pa_mode_2 = 0; pa_timer_2 = 0;
	xpdr_sel = THREEPOSSWITCH_CENTER; // OFF
	rcvr_agc_voltage = 0.0;
}

void USB::Init(Saturn *vessel){
	sat = vessel;
	ant = &sat->omnib;
	fm_ena = 1; pa_ovr_1 = 1; pa_ovr_2 = 1;
	pa_mode_1 = 0; pa_timer_1 = 0;
	pa_mode_2 = 0; pa_timer_2 = 0;
	xpdr_sel = THREEPOSSWITCH_CENTER; // OFF
	rcvr_agc_voltage = 0.0;
}

void USB::SystemTimestep(double simdt) {
	// S-Band Transponder power
	if(sat->SBandNormalXPDRSwitch.GetState() != xpdr_sel){		
		if (sat->SBandPWRAmpl1FLTBusCB.Voltage() > 12.0 && sat->SBandNormalXPDRSwitch.GetState() == THREEPOSSWITCH_UP)
		{
			sat->SBandPWRAmpl1FLTBusCB.DrawPower(1); // Consume switching power
			xpdr_sel = sat->SBandNormalXPDRSwitch.GetState();
		}
		else if (sat->SBandPWRAmpl2FLTBusCB.Voltage() > 12.0 && sat->SBandNormalXPDRSwitch.GetState() == THREEPOSSWITCH_DOWN)
		{
			sat->SBandPWRAmpl2FLTBusCB.DrawPower(1); // Consume switching power
			xpdr_sel = sat->SBandNormalXPDRSwitch.GetState();
		}
	}
	switch(xpdr_sel){
		case THREEPOSSWITCH_CENTER: // OFF
			break; 
		case THREEPOSSWITCH_UP:     // PRIM
			if(sat->SBandPWRAmpl1Group1CB.Voltage() > 100){ sat->SBandPWRAmpl1Group1CB.DrawPower(16.5); } break;
		case THREEPOSSWITCH_DOWN:   // SEC
			if(sat->SBandPWRAmpl2Group1CB.Voltage() > 100){ sat->SBandPWRAmpl2Group1CB.DrawPower(16.5); } break;
	}
	// S-Band FM Transmitter power
	if(fm_ena > 0){
		if(fm_ena == 2){ // Forced on by up tlm
			if(fm_opr == false && sat->SBandFMXMTRFLTBusCB.Voltage() > 12){
				sat->SBandFMXMTRFLTBusCB.DrawPower(1.5); // Consume switching power
				fm_opr = true;
			}
		}else{
			if(sat->SBandAuxSwitch1.GetState() == THREEPOSSWITCH_UP){ // TAPE selected
				if(fm_opr == false && sat->SBandFMXMTRFLTBusCB.Voltage() > 12){
					sat->SBandFMXMTRFLTBusCB.DrawPower(1.5); // Consume switching power
					fm_opr = true;
				}
			}else{
				if(sat->SBandAuxSwitch2.GetState() != THREEPOSSWITCH_CENTER){ // TV or SCI selected
					if(fm_opr == false && sat->SBandFMXMTRFLTBusCB.Voltage() > 12){
						sat->SBandFMXMTRFLTBusCB.DrawPower(1.5); // Consume switching power
						fm_opr = true;
					}
				}else{
					// Both off
					if(fm_opr == true && sat->SBandFMXMTRFLTBusCB.Voltage() > 12){
						sat->SBandFMXMTRFLTBusCB.DrawPower(1.5); // Consume switching power
						fm_opr = false;
					}
				}
			}
		}
	}else{
		// FM disabled by telemetry
		if(fm_opr == true && sat->SBandFMXMTRFLTBusCB.Voltage() > 12){
			sat->SBandFMXMTRFLTBusCB.DrawPower(1.5); // Consume switching power
			fm_opr = false;
		}
	}
	if(fm_opr){
		if(sat->TelcomGroup1Switch.Voltage() > 100){ sat->TelcomGroup1Switch.DrawPower(6.7); }
	}
	// S-Band Power Amplifier #1
	if (pa_ovr_1 > 0) {
		// Enabled
		if (pa_ovr_1 > 1) {
			// TLM forced on
			if (pa_mode_1 == 0 && sat->FlightBus.Voltage() > 24){ pa_mode_1 = 2; } // Start warming up
		} else {
			// Normal
			if (sat->SBandNormalPwrAmpl1Switch.IsUp()) {
				if (!sat->SBandNormalPwrAmpl2Switch.IsCenter()) {
					// Turned on
					if (pa_mode_1 == 0 && sat->FlightBus.Voltage() > 24){ pa_mode_1 = 2; } // Start warming up
				} else {
					// Turned off
					if(pa_mode_1 > 1 && sat->FlightBus.Voltage() > 24){ pa_mode_1 = 1; } // Start shutting down
				}
			}
		}
	} else {
		// TLM commanded off
		if(pa_mode_1 > 1 && sat->FlightBus.Voltage() > 24){ pa_mode_1 = 1; } // Start shutting down
	}
	// S-Band Power Amplifier #2
	if (pa_ovr_2 > 0) { 
		// Enabled
		if (pa_ovr_2 > 1) {
			// TLM forced on
			if (pa_mode_2 == 0 && sat->FlightBus.Voltage() > 24){ pa_mode_2 = 2; } // Start warming up
		} else {
			// Normal
			if (sat->SBandNormalPwrAmpl1Switch.IsDown()) {
				if (!sat->SBandNormalPwrAmpl2Switch.IsCenter()) {
					// Turned on
					if(pa_mode_2 == 0 && sat->FlightBus.Voltage() > 24){ pa_mode_2 = 2; } // Start warming up
				} else {
					// Turned off
					if(pa_mode_2 > 1 && sat->FlightBus.Voltage() > 24){	pa_mode_2 = 1; } // Start shutting down
				}
			}
		}
	} else {
		// TLM commanded off
		if(pa_mode_2 > 1 && sat->FlightBus.Voltage() > 24){	pa_mode_2 = 1; } // Start shutting down
	}

	// Power Amplifier #1 
	switch(pa_mode_1) {
		case 2: // STARTING UP
			// For the remainder of 90 seconds, warm up the tubes
			if (sat->FlightBus.Voltage() > 24 && sat->TelcomGroup1Switch.Voltage() > 100) {
				sat->FlightBus.DrawPower(5);
				sat->TelcomGroup1Switch.DrawPower(15);
			}
			break;
		case 3: // OPERATING AT LOW POWER
			if (sat->FlightBus.Voltage() > 24 && sat->TelcomGroup1Switch.Voltage() > 100) {
				sat->FlightBus.DrawPower(5);
				sat->TelcomGroup1Switch.DrawPower(32);
			}
			break;
		case 4: // OPERATING AT HIGH POWER
			if (sat->FlightBus.Voltage() > 24 && sat->TelcomGroup1Switch.Voltage() > 100) {
				sat->FlightBus.DrawPower(5);
				sat->TelcomGroup1Switch.DrawPower(81);
			}
			break;
	}
	// Power Amplifier #2 
	switch(pa_mode_2) {
		case 2: // STARTING UP
			// For the remainder of 90 seconds, warm up the tubes
			if (sat->FlightBus.Voltage() > 24 && sat->TelcomGroup2Switch.Voltage() > 100) {
				sat->FlightBus.DrawPower(5);
				sat->TelcomGroup2Switch.DrawPower(15);
			}
			break;
		case 3: // OPERATING AT LOW POWER
			if (sat->FlightBus.Voltage() > 24 && sat->TelcomGroup2Switch.Voltage() > 100) {
				sat->FlightBus.DrawPower(5);
				sat->TelcomGroup2Switch.DrawPower(32);
			}
			break;
		case 4: // OPERATING AT HIGH POWER
			if (sat->FlightBus.Voltage() > 24 && sat->TelcomGroup2Switch.Voltage() > 100) {
				sat->FlightBus.DrawPower(5);
				sat->TelcomGroup2Switch.DrawPower(81);
			}
			break;
	}
}

void USB::TimeStep(double simt) {

	/// \todo Move all DrawPower's to SystemTimestep

	//S-Band Antenna switches
	if (sat->SBandAntennaSwitch2.GetState() == THREEPOSSWITCH_UP)
	{
		if (sat->SBandAntennaSwitch1.GetState() == THREEPOSSWITCH_UP)
		{
			ant = &sat->omnia;
		}
		else if (sat->SBandAntennaSwitch1.GetState() == THREEPOSSWITCH_CENTER)
		{
			ant = &sat->omnib;
		}
		else if (sat->SBandAntennaSwitch1.GetState() == THREEPOSSWITCH_DOWN)
		{
			ant = &sat->omnic;
		}
	}
	else if (sat->SBandAntennaSwitch2.GetState() == THREEPOSSWITCH_CENTER)
	{
		ant = &sat->omnid;
	}
	else if (sat->SBandAntennaSwitch2.GetState() == THREEPOSSWITCH_DOWN)
	{
		ant = &sat->hga;
	}
	 
	// Power Amplifier #1 
	switch(pa_mode_1){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(pa_timer_1 == 0){
				pa_timer_1 = simt; // Initialize
			}
			// For the first .3 second of the shutdown, draw switching current.
			if(simt < (pa_timer_1+0.3)){
				if(sat->FlightBus.Voltage() > 24){
					sat->FlightBus.DrawPower(350);					
				}else{
					// Flight bus power failed - Start over
					pa_timer_1 = simt;
				}
				break;
			}
			// Now wait the remainder of one second for the tubes to cool
			if(simt > (pa_timer_1+1)){
				// Tubes are cooled, we can stop the timer.
				pa_timer_1 = 0; pa_mode_1 = 0;
			}
			break;
		case 2: // STARTING UP
			if(pa_timer_1 == 0){
				pa_timer_1 = simt; // Initialize
			}
			// For the first .3 second of the startup, draw switching current and start warming up.
			if(simt < (pa_timer_1+0.3)){
				if(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup1Switch.Voltage() > 100){
					sat->FlightBus.DrawPower(350);
					sat->TelcomGroup1Switch.DrawPower(15);
				}else{
					// Either bus power failed - Start over
					pa_timer_1 = simt;
				}
				break;
			}
			// For the remainder of 90 seconds, warm up the tubes
			if (!(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup1Switch.Voltage() > 100)) {				
				// Either bus power failed - Start over
				pa_timer_1 = simt; break;
			}
			if(simt > (pa_timer_1+90)){
				// Tubes are warm and we're ready to operate.
				if(pa_ovr_1 == 2 || (pa_ovr_1 == 1 && sat->SBandNormalPwrAmpl1Switch.IsUp() && sat->SBandNormalPwrAmpl2Switch.IsDown())){
					// Change to low power
					pa_mode_1 = 3; pa_timer_1 = 0;
				}
				if(pa_ovr_1 == 3 || (pa_ovr_1 == 1 && sat->SBandNormalPwrAmpl1Switch.IsUp() && sat->SBandNormalPwrAmpl2Switch.IsUp())){
					// Change to high power
					pa_mode_1 = 4; pa_timer_1 = 0;
				}
			}
			break;
		case 3: // OPERATING AT LOW POWER
			if (!(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup1Switch.Voltage() > 100)) {
				// Either bus power failed - Start cooling
				if(pa_timer_1 == 0){
					pa_timer_1 = simt;
				}
				if(simt > (pa_timer_1+1)){ // After one second, shut down
					pa_mode_1 = 1; break;
				}
			}
			if(pa_ovr_1 == 3 || (pa_ovr_1 == 1 && sat->SBandNormalPwrAmpl1Switch.IsUp() && sat->SBandNormalPwrAmpl2Switch.IsUp())){
				// Change to high power
				pa_mode_1 = 4;
			}
			if(pa_ovr_1 == 1 && sat->SBandNormalPwrAmpl1Switch.IsUp() && sat->SBandNormalPwrAmpl2Switch.IsCenter()){
				// Change to warm-up
				pa_mode_1 = 2; pa_timer_1 = (simt-95);
			}
			break;
		case 4: // OPERATING AT HIGH POWER
			if (!(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup1Switch.Voltage() > 100)) {
				// Either bus power failed - Start cooling
				if(pa_timer_1 == 0){
					pa_timer_1 = simt;
				}
				if(simt > (pa_timer_1+1)){ // After one second, shut down
					pa_mode_1 = 1; break;
				}
			}
			if(pa_ovr_1 == 2 || (pa_ovr_1 == 1 && sat->SBandNormalPwrAmpl1Switch.IsUp() && sat->SBandNormalPwrAmpl2Switch.IsDown())){
				// Change to low power
				pa_mode_1 = 3;
			}
			if(pa_ovr_1 == 1 && sat->SBandNormalPwrAmpl1Switch.IsUp() && sat->SBandNormalPwrAmpl2Switch.IsCenter()){
				// Change to warm-up
				pa_mode_1 = 2; pa_timer_1 = (simt-95);
			}
			break;
	}
	// Power Amplifier #2 
	switch(pa_mode_2){
		case 0: // OFF
			break;
		case 1: // SHUTTING DOWN
			if(pa_timer_2 == 0){
				pa_timer_2 = simt; // Initialize
			}
			// For the first .3 second of the shutdown, draw switching current.
			if(simt < (pa_timer_2+0.3)){
				if(sat->FlightBus.Voltage() > 24){
					sat->FlightBus.DrawPower(350);					
				}else{
					// Flight bus power failed - Start over
					pa_timer_2 = simt;
				}
				break;
			}
			// Now wait the remainder of one second for the tubes to cool
			if(simt > (pa_timer_2+1)){
				// Tubes are cooled, we can stop the timer.
				pa_timer_2 = 0; pa_mode_2 = 0;
			}
			break;
		case 2: // STARTING UP
			if(pa_timer_2 == 0){
				pa_timer_2 = simt; // Initialize
			}
			// For the first .3 second of the startup, draw switching current and start warming up.
			if(simt < (pa_timer_2+0.3)){
				if(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup2Switch.Voltage() > 100){
					sat->FlightBus.DrawPower(350);
					sat->TelcomGroup2Switch.DrawPower(15);
				}else{
					// Either bus power failed - Start over
					pa_timer_2 = simt;
				}
				break;
			}
			// For the remainder of 90 seconds, warm up the tubes
			if (!(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup2Switch.Voltage() > 100)) {
				// Either bus power failed - Start over
				pa_timer_2 = simt; break;
			}
			if(simt > (pa_timer_2+90)){
				// Tubes are warm and we're ready to operate.
				if(pa_ovr_2 == 2 || (pa_ovr_2 == 1 && sat->SBandNormalPwrAmpl1Switch.IsDown() && sat->SBandNormalPwrAmpl2Switch.IsDown())){
					// Change to low power
					pa_mode_2 = 3; pa_timer_2 = 0;
				}
				if(pa_ovr_2 == 3 || (pa_ovr_2 == 1 && sat->SBandNormalPwrAmpl1Switch.IsDown() && sat->SBandNormalPwrAmpl2Switch.IsUp())){
					// Change to high power
					pa_mode_2 = 4; pa_timer_2 = 0;
				}
			}
			break;
		case 3: // OPERATING AT LOW POWER
			if (!(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup2Switch.Voltage() > 100)) {
				// Either bus power failed - Start cooling
				if(pa_timer_2 == 0){
					pa_timer_2 = simt;
				}
				if(simt > (pa_timer_2+1)){ // After one second, shut down
					pa_mode_2 = 1; break;
				}
			}
			if(pa_ovr_2 == 3 || (pa_ovr_2 == 1 && sat->SBandNormalPwrAmpl1Switch.IsDown() && sat->SBandNormalPwrAmpl2Switch.IsUp())){
				// Change to high power
				pa_mode_2 = 4;
			}
			if(pa_ovr_2 == 1 && sat->SBandNormalPwrAmpl1Switch.IsDown() && sat->SBandNormalPwrAmpl2Switch.IsCenter()){
				// Change to warm-up
				pa_mode_2 = 2; pa_timer_2 = (simt-95);
			}
			break;
		case 4: // OPERATING AT HIGH POWER
			if (!(sat->FlightBus.Voltage() > 24 && sat->TelcomGroup2Switch.Voltage() > 100)) {
				// Either bus power failed - Start cooling
				if(pa_timer_2 == 0){
					pa_timer_2 = simt;
				}
				if(simt > (pa_timer_2+1)){ // After one second, shut down
					pa_mode_2 = 1; break;
				}
			}
			if(pa_ovr_2 == 2 || (pa_ovr_2 == 1 && sat->SBandNormalPwrAmpl1Switch.IsDown() && sat->SBandNormalPwrAmpl2Switch.IsCenter())){
				// Change to low power
				pa_mode_2 = 3;
			}
			if(pa_ovr_2 == 1 && sat->SBandNormalPwrAmpl1Switch.IsDown() && sat->SBandNormalPwrAmpl2Switch.IsCenter()){
				// Change to warm-up
				pa_mode_2 = 2; pa_timer_2 = (simt-95);
			}
			break;
	}

	// Update PA TB
	// CSM-104 HB says that this is on when "either pwr ampl is selected for pm operation".
	// I assume that means it's actually amplifying and not in warmup.
	if (sat->SBandNormalPwrAmpl1Switch.IsUp()) {
		if (pa_mode_1 > 2) {
			sat->PwrAmplTB.SetState(1);
		} else {
			sat->PwrAmplTB.SetState(0);
		}
	} else {
		if (pa_mode_2 > 2) {
			sat->PwrAmplTB.SetState(1);
		} else {
			sat->PwrAmplTB.SetState(0);
		}
	}

	// Receiver AGC Voltage
	if (sat->SBandNormalPwrAmpl1Switch.IsUp()) {
		if (ant && pa_mode_1 > 2) {
			rcvr_agc_voltage = ant->GetSignalStrength();
		}
		else {
			rcvr_agc_voltage = 0.0;
		}
	}
	else {
		if (ant && pa_mode_2 > 2) {
			rcvr_agc_voltage = ant->GetSignalStrength();
		}
		else {
			rcvr_agc_voltage = 0.0;
		}
	}

	//sprintf(oapiDebugString(), "rcvr_agc_voltage %lf", rcvr_agc_voltage);
	// sprintf(oapiDebugString(), "USB - pa_mode_1 %d pa_mode_2 %d", pa_mode_1, pa_mode_2);
}


void USB::LoadState(char *line) {
	int i;

	sscanf(line + 12, "%i %i %i %i %i %lf %lf %i %i", &fm_ena, &xpdr_sel, &i, 
		&pa_mode_1, &pa_mode_2, &pa_timer_1, &pa_timer_2, &pa_ovr_1, &pa_ovr_2);
	fm_opr  = (i != 0);
}


void USB::SaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%i %i %i %i %i %lf %lf %i %i", fm_ena, xpdr_sel, (fm_opr ? 1 : 0), 
		pa_mode_1, pa_mode_2, pa_timer_1, pa_timer_2, pa_ovr_1, pa_ovr_2);

	oapiWriteScenario_string(scn, "UNIFIEDSBAND", buffer);
}


// High Gain Antenna
// Unifed S-Band System
HGA::HGA(){
	sat = NULL;
	Alpha = 0;
	Beta = 0;
	Gamma = 0;
	AAxisCmd = 0;
	BAxisCmd = 0;
	CAxisCmd = 0;
	SignalStrength = 0;
	PitchRes = 0;
	YawRes = 0;
	
	scanlimit = false;
	scanlimitwarn = false;

	for (int i = 0;i < 0;i++)
	{
		HornSignalStrength[i] = 0.0;
	}

	double angdiff = 1.0*RAD; 

	//looking along the antennas pointing vector(x3) with z3 to your left and y3 pointing down
	U_Horn[0] = _V(cos(angdiff), 0.0, -sin(angdiff));//left
	U_Horn[1] = _V(cos(-angdiff), 0.0, -sin(-angdiff));//right
	U_Horn[2] = _V(cos(angdiff), sin(angdiff), 0.0);//up
	U_Horn[3] = _V(cos(-angdiff), sin(-angdiff), 0.0);//down

	anim_HGAalpha = -1;
	anim_HGAbeta = -1;
	anim_HGAgamma = -1;

	hga_proc[0] = hga_proc_last[0] = 0.0;
	hga_proc[1] = hga_proc_last[1] = 0.0;
	hga_proc[2] = hga_proc_last[2] = 0.0;
}

void HGA::Init(Saturn *vessel){
	sat = vessel;
	Alpha = 180.0*RAD;
	Beta = 0;
	Gamma = 90.0*RAD;
	PitchRes = 0;
	YawRes = 180.0*RAD;
	SignalStrength = 0;
	scanlimit = false;
	scanlimitwarn = false;
	ModeSwitchTimer = 0;
	AutoTrackingMode = false;
	RcvBeamWidthSelect = 0; // 0 = none, 1 = Wide, 2 = Med, 3 = Narrow
	XmtBeamWidthSelect = 0; // 0 = none, 1 = Wide, 2 = Med, 3 = Narrow

	HGAFrequency = 2119; //MHz. Should this get set somewhere else?
	HGAWavelength = C0 / (HGAFrequency * 1000000); //meters
	Gain85ft = pow(10,(50 / 10)); //this is the gain, dB converted to ratio of the 85ft antennas on earth
	Power85ft = 20000; //watts
}

bool HGA::IsPowered()
{
	// Do we have a HGA?
	if (!sat->pMission->CSMHasHGA()) return false;

	// Fully deployed antenna boom operates micro switch; separated SM deenergized power switch
	if (sat->GetStage() != CSM_LEM_STAGE) return false;

	// Do we have power?
	if (!sat->GHAPowerSwitch.IsUp()) return false;		// Switched off

	// Ensure AC/DC power
	if (!sat->HGAFLTBus1CB.IsPowered() ||
		!sat->HGAGroup2CB.IsPowered()) return false;

	return true;
}

// Draw power
void HGA::SystemTimestep(double simdt) {	
	
	// Do we have power?
	if (!IsPowered()) return;

	// see CSM Systems Handbook
	if (sat->GHAServoElecSwitch.IsUp())
	{
		sat->HGAFLTBus1CB.DrawPower(16.45);
	}
	else
	{
		sat->HGAFLTBus1CB.DrawPower(22.84);
	}
	sat->HGAGroup2CB.DrawPower(34.5);
}

void HGA::DefineAnimations(UINT idx) {

	// HGA animation definition
	ANIMATIONCOMPONENT_HANDLE	ach_HGAalpha, ach_HGAbeta, ach_HGAgamma;
	const VECTOR3	HGA_PIVOT1 = { -0.460263, -0.596586, -0.062961 }; // Pivot Point
	const VECTOR3	HGA_PIVOT2 = { -0.530745, -0.687882, -0.062966 }; // Pivot Point
	const VECTOR3	HGA_PIVOT3 = { -0.589306, -0.764893, -0.06296 }; // Pivot Point
	const VECTOR3	HGA_AXIS_YAW = { sin(RAD * 37.75),cos(RAD * 37.75), 0.00 }; //Pivot Axis
	const VECTOR3	HGA_AXIS_PITCH = { -sin(RAD * 52.25),cos(RAD * 52.25), 0.00 }; //Pivot Axis

	static UINT meshgroup_Pivot1 = { 2 };
	static UINT meshgroup_Pivot2 = { 3 };
	static UINT meshgroup_Main[2] = { 1, 4 };

	static MGROUP_ROTATE mgt_HGA_Alpha(idx, &meshgroup_Pivot1, 1, HGA_PIVOT1, HGA_AXIS_YAW, (float)(RAD * 360));
	static MGROUP_ROTATE mgt_HGA_Beta(idx, &meshgroup_Pivot2, 1, HGA_PIVOT2, _V(0, 0, 1), (float)(RAD * 360));
	static MGROUP_ROTATE mgt_HGA_Gamma(idx, meshgroup_Main, 2, HGA_PIVOT3, HGA_AXIS_PITCH, (float)(RAD * 360));
	anim_HGAalpha = sat->CreateAnimation(0.0);
	anim_HGAbeta = sat->CreateAnimation(0.0);
	anim_HGAgamma = sat->CreateAnimation(0.0);
	ach_HGAalpha = sat->AddAnimationComponent(anim_HGAalpha, 0.0f, 1.0f, &mgt_HGA_Alpha);
	ach_HGAbeta = sat->AddAnimationComponent(anim_HGAbeta, 0.0f, 1.0f, &mgt_HGA_Beta, ach_HGAalpha);
	ach_HGAgamma = sat->AddAnimationComponent(anim_HGAgamma, 0.0f, 1.0f, &mgt_HGA_Gamma, ach_HGAbeta);
	//Anything but 0.0-1.0 will do
	hga_proc_last[0] = 2.0;
	hga_proc_last[1] = 2.0;
	hga_proc_last[2] = 2.0;
}

void HGA::DeleteAnimations() {

	if (anim_HGAalpha != -1) sat->DelAnimation(anim_HGAalpha);
	anim_HGAalpha = -1;
	if (anim_HGAbeta != -1) sat->DelAnimation(anim_HGAbeta);
	anim_HGAbeta = -1;
	if (anim_HGAgamma != -1) sat->DelAnimation(anim_HGAgamma);
	anim_HGAgamma = -1;
}


// Do work
void HGA::TimeStep(double simt, double simdt)
{
	if (!sat->pMission->CSMHasHGA()) return;
	if (sat->GetStage() != CSM_LEM_STAGE) return;

	// HGA mesh animation
	hga_proc[0] = Alpha / PI2;
	if (hga_proc[0] < 0) hga_proc[0] += 1.0;
	hga_proc[1] = Beta / PI2;
	if (hga_proc[1] < 0) hga_proc[1] += 1.0;
	hga_proc[2] = (Gamma - PI05) / PI2;
	if (hga_proc[2] < 0) hga_proc[2] += 1.0;
	if (hga_proc[0] - hga_proc_last[0] != 0.0) sat->SetAnimation(anim_HGAalpha, hga_proc[0]);
	if (hga_proc[1] - hga_proc_last[1] != 0.0) sat->SetAnimation(anim_HGAbeta, hga_proc[1]);
	if (hga_proc[2] - hga_proc_last[2] != 0.0) sat->SetAnimation(anim_HGAgamma, hga_proc[2]);
	hga_proc_last[0] = hga_proc[0];
	hga_proc_last[1] = hga_proc[1];
	hga_proc_last[2] = hga_proc[2];

	// Do we have power and a SM?
	if (!IsPowered())
	{
		SignalStrength = 0;
		scanlimitwarn = false;
		scanlimit = false;

		return;
	}
	
	double gain;
	double AzimuthErrorSignal, ElevationErrorSignal;
	double AzimuthErrorSignalNorm, ElevationErrorSignalNorm;
	double AzmuthTrackErrorDeg, TrackErrorSumNorm;
	bool WhiparoundIsSet = false;

	//actual Azimuth and Elevation error signals came from phase differences not signal strength
	//both are be a function of tracking error though so this works
	AzimuthErrorSignal = (HornSignalStrength[1] - HornSignalStrength[0])*0.25;
	ElevationErrorSignal = (HornSignalStrength[2] - HornSignalStrength[3])*0.25;

	//normalize Azimuth and Elevation error signals
	if (SignalStrength > 0.0)
	{
		AzimuthErrorSignalNorm = AzimuthErrorSignal / SignalStrength;
		ElevationErrorSignalNorm = ElevationErrorSignal / SignalStrength;
		AzmuthTrackErrorDeg = acos(1 - abs(AzimuthErrorSignalNorm))*DEG;
	}
	else //prevent division by zero
	{
		AzimuthErrorSignalNorm = 0;
		ElevationErrorSignalNorm = 0;
		AzmuthTrackErrorDeg = 90 * DEG;
	}

	TrackErrorSumNorm = abs(AzimuthErrorSignalNorm + ElevationErrorSignalNorm);

	//sprintf(oapiDebugString(), "TrackErrorSumNorm %lf", TrackErrorSumNorm);

	const double TrkngCtrlGain = 2.9; //determined empericially, is actually the combination of many gains that are applied to everything from gear backlash to servo RPM
	const double ServoFeedbackGain = 1.2; //this works too...
	const double BeamSwitchingTrkErThreshhold = 0.001; 


	//There are different behavoirs for recv vs xmit beamwidth, right now this just looks at recv mode, we can add the xmit vs recv modes later

	//this block handels mode selection based on combinations of mode and beam width switches and the signal strength scanlimit
	if(sat->GHATrackSwitch.IsCenter()) //manual control if switch is set to manual or scanlimit has been hit in reacq mode
	{
		AutoTrackingMode = false;
		RcvBeamWidthSelect = 0;
		XmtBeamWidthSelect = 0;
	}
	else if (sat->GHATrackSwitch.IsUp()) // auto mode selected
	{
		AutoTrackingMode = true;
		if (ModeSwitchTimer < simt) //timer prevents getting caught oscilating between narrow/wide or auto/manual which can happen if checked every timestep or with very small[ish] timesteps
		{
			if (SignalStrength > 0)
			{
				if (TrackErrorSumNorm >= BeamSwitchingTrkErThreshhold*392) //acquire mode in auto (392 = PI/8*1000 which is a good place to switch between narrow and wide)
				{
					RcvBeamWidthSelect = 1;
					XmtBeamWidthSelect = 1;
				}

				if ((TrackErrorSumNorm < BeamSwitchingTrkErThreshhold) && (sat->GHABeamSwitch.IsUp())) //tracking modes in auto, limits to wide with switch up
				{
					RcvBeamWidthSelect = 1;
					XmtBeamWidthSelect = 1;
				}
				else if ((TrackErrorSumNorm < BeamSwitchingTrkErThreshhold) && sat->GHABeamSwitch.IsCenter()) //tracking modes in auto, limits to wide with switch up
				{
					RcvBeamWidthSelect = 3;
					XmtBeamWidthSelect = 2;
				}
				else if ((TrackErrorSumNorm < BeamSwitchingTrkErThreshhold) && sat->GHABeamSwitch.IsDown())
				{
					RcvBeamWidthSelect = 3;
					XmtBeamWidthSelect = 3;
				}
			}
			else //switch back to wide beamwidth if signal lost
			{
				RcvBeamWidthSelect = 1;
				XmtBeamWidthSelect = 1;
			}
			ModeSwitchTimer = simt + 1; 
		}
	}
	else
	{
		AutoTrackingMode = true;	//enable the auto track flag. this might not need to be here, but it also might be fixing a rare condition where the state oscimates between manual and auto and won't acquire. 
									//It get's set to manual later if we actually have no signal

		if (ModeSwitchTimer < simt)
		{
			if ((SignalStrength > 0) && (scanlimitwarn == false) && (scanlimit == false)) //
			{
				AutoTrackingMode = true; //if it somehow wasn't on...
				if ((TrackErrorSumNorm >= BeamSwitchingTrkErThreshhold * 392)) //acquire mode in auto
				{
					RcvBeamWidthSelect = 1;
					XmtBeamWidthSelect = 1;
				}

				if ((TrackErrorSumNorm < BeamSwitchingTrkErThreshhold) && (sat->GHABeamSwitch.IsUp())) //tracking modes in auto wide
				{
					RcvBeamWidthSelect = 1;
					XmtBeamWidthSelect = 1;
				}
				else if ((TrackErrorSumNorm < BeamSwitchingTrkErThreshhold) && sat->GHABeamSwitch.IsCenter()) //tracking modes in auto med
				{
					RcvBeamWidthSelect = 3;
					XmtBeamWidthSelect = 2;
				}
				else if ((TrackErrorSumNorm < BeamSwitchingTrkErThreshhold) && sat->GHABeamSwitch.IsDown()) //tracking modes in auto fine
				{
					RcvBeamWidthSelect = 3;
					XmtBeamWidthSelect = 3;
				}
				ModeSwitchTimer = simt + 1; 

			}
			else if ((scanlimitwarn == true) && (scanlimit == false)) //switch to wide mode, but stay in auto tracking if scanlimit warn is set, but not scanlimit
			{
				AutoTrackingMode = true;
				RcvBeamWidthSelect = 1;
				XmtBeamWidthSelect = 1;
				ModeSwitchTimer = simt + 1;
			}
			else // switch to manual mode if loss of signal or scanlimit (this will enable the manual controls and drive the servos to the selecter position)
			{
				AutoTrackingMode = false;
				RcvBeamWidthSelect = 1;
				XmtBeamWidthSelect = 1;
			}
		}
		
	}

	//select control mode for high gain antenna 
	if (AutoTrackingMode == false) //manual control if switch is set to manual or scanlimit has been hit in reacq mode
	{
		double PitchCmd, YawCmd;
		PitchCmd = -(double)sat->HighGainAntennaPitchPositionSwitch.GetState()*15.0 + 90.0;
		YawCmd = (double)sat->HighGainAntennaYawPositionSwitch.GetState()*15.0;

		//Command Resolver
		VECTOR3 U_RB;
		U_RB = PitchYawToBodyVector(PitchCmd*RAD, YawCmd*RAD);
		BodyToAC(U_RB, AAxisCmd, CAxisCmd);
		BAxisCmd = 0.0;

		//sprintf(oapiDebugString(), "PitchCmd: %lf° YawCmd: %lf° AAxisCmd: %lf° CAxisCmd: %lf°", PitchCmd, YawCmd, AAxisCmd*DEG, CAxisCmd*DEG);
	}
	else if (AutoTrackingMode == true) //this auto-tracking is used in both the AUTO and the REAQC modes. Beamwidth switching, LOS/AOS logic and scanlimit(warn) log are handled in a seperate block of code 
	{ 
		//auto control, added by n72.75 204020
		if (Gamma > 45 * RAD)	//mode select A-C servo control
		{
			if (AzmuthTrackErrorDeg > 3.0)
			{
				AAxisCmd = Alpha + (TrkngCtrlGain*AzimuthErrorSignalNorm*simdt);
				BAxisCmd = Beta - (Beta*ServoFeedbackGain*simdt);
				CAxisCmd = Gamma + (TrkngCtrlGain*ElevationErrorSignalNorm*simdt);
			}
			else
			{
				AAxisCmd = Alpha + (TrkngCtrlGain*AzimuthErrorSignalNorm*simdt) - (Beta*ServoFeedbackGain*simdt);
				BAxisCmd = Beta - (Beta*ServoFeedbackGain*simdt);
				CAxisCmd = Gamma + (TrkngCtrlGain*ElevationErrorSignalNorm*simdt);
			}
		}
		else					//mode select B-C servo control
		{
			double GammaLast = Gamma;
			if (Gamma <= -1.0 * RAD)
			{
				if ((GammaLast > -1.0 * RAD)&&(Gamma < -1.0)) //falling edge detection for gamma dropping below -1.0°
				{
					WhiparoundIsSet = true; //set whiparound flag on falling edge below -1.0
				}
				else
				{
					WhiparoundIsSet = false; //clear whiparound flag
				}
				BAxisCmd = Beta - (TrkngCtrlGain*AzimuthErrorSignalNorm*simdt) - (Beta*ServoFeedbackGain*simdt / 10);
				CAxisCmd = Gamma + (TrkngCtrlGain*ElevationErrorSignalNorm*simdt);

				if (WhiparoundIsSet == true)
				{
					if (Alpha > 0.0)
					{
						AAxisCmd = Alpha - 180 * RAD;
					}
					else
					{
						AAxisCmd = Alpha + 180 * RAD;
					}
					WhiparoundIsSet = false; //clear whiparound flag
				}
			}
			else
			{
				if (AzmuthTrackErrorDeg > 3.0)
				{
					AAxisCmd = Alpha + (TrkngCtrlGain*AzimuthErrorSignalNorm*simdt);
					BAxisCmd = Beta - (TrkngCtrlGain*AzimuthErrorSignalNorm*simdt) - (Beta*ServoFeedbackGain*simdt/10);
					CAxisCmd = Gamma + (TrkngCtrlGain*ElevationErrorSignalNorm*simdt);
				}
				else
				{
					AAxisCmd = Alpha + (TrkngCtrlGain*AzimuthErrorSignalNorm*simdt);
					BAxisCmd = Beta - (Beta*ServoFeedbackGain*simdt);
					CAxisCmd = Gamma + (TrkngCtrlGain*ElevationErrorSignalNorm*simdt);
				}
			}
		}
	}
	else
	{
		AAxisCmd = Alpha;
		BAxisCmd = Beta;
		CAxisCmd = Gamma;
	}

	//sprintf(oapiDebugString(), "AzimuthErrorSigNorm: %lf ElevationErrorSigNorm: %lf A_CMD: %lf B_CMD: %lf C_CMD: %lf SignalStrength %lf AzmuthTrackErrorDeg %lf°", AzimuthErrorSignalNorm, ElevationErrorSignalNorm, AAxisCmd, BAxisCmd, CAxisCmd, SignalStrength, AzmuthTrackErrorDeg);

	//SERVO DRIVE

	// 5°/s rate limit as per CSM Data Book (3.7.4.1)
	ServoDrive(Alpha, AAxisCmd, 5.0*RAD, simdt);
	ServoDrive(Beta, BAxisCmd, 5.0*RAD, simdt);
	ServoDrive(Gamma, CAxisCmd, 5.0*RAD, simdt);

	//GIMBAL LIMITS
	if (Alpha > PI2)
	{
		Alpha = PI2;
	}
	else if (Alpha < -PI2)
	{
		Alpha = -PI2;
	}
	if (Beta > 23.5*RAD)
	{
		Beta = 23.5*RAD;
	}
	else if (Beta < -23.5*RAD)
	{
		Beta = -23.5*RAD;
	}
	if (Gamma > 125.0*RAD)
	{
		Gamma = 125.0*RAD;
	}
	else if (Gamma < -4.0*RAD)
	{
		Gamma = -4.0*RAD;
	}

	//sprintf(oapiDebugString(), "AAxisCmd: %lf° CAxisCmd: %lf° Alpha: %lf° Gamma: %lf°", AAxisCmd*DEG, CAxisCmd*DEG, Alpha*DEG, Gamma*DEG);

	//READOUT RESOLVER
	VECTOR3 U_Readout;
	U_Readout = ABCAndVectorToBody(Alpha, 0, Gamma, _V(1.0, 0.0, 0.0));
	BodyVectorToPitchYaw(U_Readout, PitchRes, YawRes);

	//sprintf(oapiDebugString(), "Alpha: %lf° Gamma: %lf° PitchRes: %lf° YawRes: %lf°", Alpha*DEG, Gamma*DEG, PitchRes*DEG, YawRes*DEG);

	VECTOR3 U_RP, pos, R_E, R_M, U_R;
	MATRIX3 Rot;
	double relang, beamwidth, Moonrelang, EarthSignalDist;

	OBJHANDLE hMoon = oapiGetObjectByName("Moon");
	OBJHANDLE hEarth = oapiGetObjectByName("Earth");

	//Global position of Earth, Moon and spacecraft, spacecraft rotation matrix from local to global
	sat->GetGlobalPos(pos);
	oapiGetGlobalPos(hEarth, &R_E);
	oapiGetGlobalPos(hMoon, &R_M);
	sat->GetRotationMatrix(Rot);
	
	double RecvdHGAPower, RecvdHGAPower_dBm, SignalStrengthScaleFactor;
	//gain values from NASA Technical Note TN D-6723
	
	EarthSignalDist = length(pos - R_E) - oapiGetSize(hEarth); //distance from earth's surface in meters
	
	int RcvBeamWidthMode = 1;

	if(RcvBeamWidthSelect == 0)
		if (sat->GHABeamSwitch.IsUp())
		{
			RcvBeamWidthMode = 1;
		}
		else if (sat->GHABeamSwitch.IsCenter())
		{
			RcvBeamWidthMode = 2;
		}
		else
		{
			RcvBeamWidthMode = 3;
		}
	else
	{
		RcvBeamWidthMode = RcvBeamWidthSelect;
	}

	if (RcvBeamWidthMode == 1)		//Wide 3.1 dB
	{
		beamwidth = 40.0*RAD;
		gain = pow(10, (3.1 / 10)); //dB to ratio
	}
	else if (RcvBeamWidthMode == 2)	//Medium 22.5 dB
	{
		beamwidth = 4.5*RAD;
		gain = pow(10, (22.5 / 10)); //dB to ratio
	}
	else						//Narrow 23.0 dB
	{
		beamwidth = 4.5*RAD;
		gain = pow(10, (23.0 / 10)); //dB to ratio
	}

	RecvdHGAPower = Power85ft*Gain85ft*gain*pow(HGAWavelength/(4*PI*EarthSignalDist),2); //maximum recieved power to the HGA on axis in watts
	RecvdHGAPower_dBm = 10*log10(1000*RecvdHGAPower);
	SignalStrengthScaleFactor = SBandAntenna::dBm2SignalStrength(RecvdHGAPower_dBm);

	//sprintf(oapiDebugString(), "Received HGA Power: %lf fW, %lf dBm", RecvdHGAPower*1000000000000000, RecvdHGAPower_dBm); //show theoretical max HGA recieved in Femtowatts and dBm

	double a = acos(sqrt(sqrt(0.5))) / (beamwidth / 2.0); //Scaling for beamwidth... I think; now with actual half-POWER beamwidth

	//Moon in the way
	Moonrelang = dotp(unit(R_M - pos), unit(R_E - pos));

	if (Moonrelang > cos(asin(oapiGetSize(hMoon) / length(R_M - pos))))
	{
		SignalStrength = 0.0;
		for (int i = 0;i < 4;i++)
		{
			HornSignalStrength[i] = 0.0;
		}
	}
	else
	{
		for (int i = 0;i < 4;i++)
		{
			//Unit vector of antenna in vessel's local frame
			U_RP = ABCAndVectorToBody(Alpha, Beta, Gamma, U_Horn[i]);
			//Convert from Apollo CSM coordinate system to left-handed Orbiter coordinate system
			U_RP = _V(U_RP.y, -U_RP.z, U_RP.x);

			//Calculate antenna pointing vector in global frame
			U_R = mul(Rot, U_RP);
			//relative angle between antenna pointing vector and direction of Earth
			relang = acos(dotp(U_R, unit(R_E - pos)));

			if (relang < PI05 / a)
			{
				//HornSignalStrength[i] = cos(a*relang)*cos(a*relang)*gain;
				HornSignalStrength[i] = cos(a*relang)*cos(a*relang)*SignalStrengthScaleFactor;
			}
			else
			{
				HornSignalStrength[i] = 0.0;
			}
		}

		SignalStrength = (HornSignalStrength[0] + HornSignalStrength[1] + HornSignalStrength[2] + HornSignalStrength[3]) / 4.0;

		//sprintf(oapiDebugString(), "%f %f %f %f", HornSignalStrength[0], HornSignalStrength[1], HornSignalStrength[2], HornSignalStrength[3]);
	}

	double scanlim, scanlimwarn;

	//Scan limit function
	scanlim = 11.0*RAD*sin(2.0*Alpha - PI05) + 105.0*RAD;
	//Scan limit warning function
	scanlimwarn = 11.0*RAD*sin(2.0*Alpha - PI05) + PI05;

	if (Gamma > scanlim)
	{
		scanlimit = true;
	}
	else
	{
		scanlimit = false;
	}
	if (Gamma > scanlimwarn)
	{
		scanlimitwarn = true;
	}
	else
	{
		scanlimitwarn = false;
	}

	//sprintf(oapiDebugString(), "A: %lf° B: %lf° C: %lf° PitchRes: %lf° YawRes: %lf° SignalStrength %lf RelAng %lf Warn: %d Limit: %d", Alpha*DEG, Beta*DEG, Gamma*DEG, PitchRes*DEG, YawRes*DEG, SignalStrength, relang*DEG, scanlimitwarn, scanlimit);
}

void HGA::ServoDrive(double &Angle, double AngleCmd, double RateLimit, double simdt)
{
	double dposcmd, dpos;

	dposcmd = AngleCmd - Angle;

	if (abs(dposcmd)>RateLimit*simdt)
	{
		dpos = sign(AngleCmd - Angle)*RateLimit*simdt;
	}
	else
	{
		dpos = dposcmd;
	}
	Angle += dpos;

}

VECTOR3 HGA::PitchYawToBodyVector(double pit, double ya)
{
	return _V(cos(ya)*cos(pit), sin(ya)*cos(pit), -sin(pit));
}

void HGA::BodyVectorToPitchYaw(VECTOR3 U_R, double &pitch, double &yaw)
{
	VECTOR3 RP, U_RP, U_X, U_Y, U_Z;
	double x;

	U_X = _V(1, 0, 0);
	U_Y = _V(0, 1, 0);
	U_Z = _V(0, 0, 1);

	RP = U_R - U_Z * dotp(U_R, U_Z);
	U_RP = unit(RP);
	yaw = acos(dotp(U_RP, U_X));
	x = dotp(U_RP, U_Y);
	if (x < 0)
	{
		yaw = PI2 - yaw;
	}
	pitch = acos(dotp(U_R, U_Z)) - PI05;
}

void HGA::BodyToAC(VECTOR3 U_R, double &alpha, double &gamma)
{
	MATRIX3 MB_apo, MC_apo;
	VECTOR3 U_R3;
	double theta;

	theta = -52.25*RAD;

	MB_apo = _M(cos(theta), 0.0, -sin(theta), 0.0, 1.0, 0.0, sin(theta), 0.0, cos(theta));
	MC_apo = _M(0.0, -1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 1.0);

	U_R3 = mul(MB_apo, mul(MC_apo, U_R));
	alpha = atan2(U_R3.z, U_R3.y);
	gamma = acos(U_R3.x);
}

VECTOR3 HGA::ABCAndVectorToBody(double alpha, double beta, double gamma, VECTOR3 U_R)
{
	MATRIX3 M1, M2, M3, MB, MC;
	double theta;

	theta = -52.25*RAD;

	M3 = _M(cos(gamma), -sin(gamma), 0.0, sin(gamma), cos(gamma), 0.0, 0.0, 0.0, 1.0);
	M2 = _M(cos(beta), 0.0, sin(beta), 0.0, 1.0, 0.0, -sin(beta), 0.0, cos(beta));
	M1 = _M(1.0, 0.0, 0.0, 0.0, cos(alpha), -sin(alpha), 0.0, sin(alpha), cos(alpha));
	MB = _M(cos(theta), 0.0, sin(theta), 0.0, 1.0, 0.0, -sin(theta), 0.0, cos(theta));
MC = _M(0.0, 1.0, 0.0, -1.0, 0.0, 0.0, 0.0, 0.0, 1.0);
return mul(MC, mul(MB, mul(M1, mul(M2, mul(M3, U_R)))));
}

bool HGA::ScanLimitWarning()
{
	return scanlimitwarn;
}

void HGA::clbkPostCreation()
{
	if (!sat->pMission->CSMHasHGA()) return;
	if (sat->GetStage() != CSM_LEM_STAGE) return;

	// Get current HGA state for animation
	hga_proc[0] = Alpha / PI2;
	if (hga_proc[0] < 0) hga_proc[0] += 1.0;
	hga_proc[1] = Beta / PI2;
	if (hga_proc[1] < 0) hga_proc[1] += 1.0;
	hga_proc[2] = (Gamma - PI05) / PI2;
	if (hga_proc[2] < 0) hga_proc[2] += 1.0;
	sat->SetAnimation(anim_HGAalpha, hga_proc[0]);
	sat->SetAnimation(anim_HGAbeta, hga_proc[1]);
	sat->SetAnimation(anim_HGAgamma, hga_proc[2]);
	hga_proc_last[0] = hga_proc[0];
	hga_proc_last[1] = hga_proc[1];
	hga_proc_last[2] = hga_proc[2];
}


// Load
void HGA::LoadState(char *line) {
	sscanf(line + 15, "%lf %lf %lf %lf %lf %lf", &Alpha, &Beta, &Gamma, &AAxisCmd, &BAxisCmd, &CAxisCmd);
}

// Save
void HGA::SaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%lf %lf %lf %lf %lf %lf", Alpha, Beta, Gamma, AAxisCmd, BAxisCmd, CAxisCmd);

	oapiWriteScenario_string(scn, "HIGHGAINANTENNA", buffer);
}

OMNI::OMNI(VECTOR3 dir)
{
	direction = unit(dir);
	hpbw_factor = 0.0;
	hMoon = NULL;
	hEarth = NULL;
}

void OMNI::Init(Saturn *vessel) {
	sat = vessel;

	SignalStrength = 0;

	double beamwidth = 45*RAD;
	OMNI_Gain = pow(10, (-3 / 10));

	hpbw_factor = acos(sqrt(sqrt(0.5))) / (beamwidth / 2.0); //Scaling for beamwidth

	hMoon = oapiGetObjectByName("Moon");
	hEarth = oapiGetObjectByName("Earth");

	OMNIFrequency = 2119; //MHz. Should this get set somewhere else?
	OMNIWavelength = C0 / (OMNIFrequency * 1000000); //meters
	Gain30ft = pow(10, (43 / 10)); //this is the gain, dB converted to ratio of the 30ft antennas on earth
	Power30ft = 20000; //watts

	
}

void OMNI::TimeStep()
{
	VECTOR3 U_RP, pos, R_E, R_M, U_R;
	MATRIX3 Rot;
	double relang, Moonrelang;
	double RecvdOMNIPower, RecvdOMNIPower_dBm, SignalStrengthScaleFactor;

	double EarthSignalDist;

	//Unit vector of antenna in vessel's local frame
	U_RP = _V(direction.y, -direction.z, direction.x);

	//Global position of Earth, Moon and spacecraft, spacecraft rotation matrix from local to global
	sat->GetGlobalPos(pos);
	oapiGetGlobalPos(hEarth, &R_E);
	oapiGetGlobalPos(hMoon, &R_M);
	sat->GetRotationMatrix(Rot);

	//Calculate antenna pointing vector in global frame
	U_R = mul(Rot, U_RP);
	//relative angle between antenna pointing vector and direction of Earth
	relang = acos(dotp(U_R, unit(R_E - pos)));

	EarthSignalDist = length(pos - R_E) - oapiGetSize(hEarth); //distance from earth's surface in meters

	RecvdOMNIPower = Power30ft * Gain30ft * OMNI_Gain * pow(OMNIWavelength / (4 * PI*EarthSignalDist), 2); //maximum recieved power to the HGA on axis in watts
	RecvdOMNIPower_dBm = 10 * log10(1000 * RecvdOMNIPower);
	SignalStrengthScaleFactor = SBandAntenna::dBm2SignalStrength(RecvdOMNIPower_dBm);

	if (relang < 160*RAD)
	{
		//very rough approximation of radiation pattern
		//https://www.wolframalpha.com/input/?i=polar+plot+sin%5E2%28%28acos%28sqrt%28sqrt%280.5%29%29%29+%2F+%2845deg+%2F+2.0%29%29*theta%2F%281.309-e%5E-theta%5E2%29%29+from+-160deg+to+160deg
		SignalStrength = sin(hpbw_factor*relang / ((75 * RAD) - exp(-(relang*relang))))*sin(hpbw_factor*relang / ((75 * RAD) - exp(-(relang*relang))))*SignalStrengthScaleFactor;
	}
	else
	{
		SignalStrength = 0.0;
	}

	//Moon in the way
	Moonrelang = dotp(unit(R_M - pos), unit(R_E - pos));

	if (Moonrelang > cos(asin(oapiGetSize(hMoon) / length(R_M - pos))))
	{
		SignalStrength = 0.0;
	}
}

VHFAntenna::VHFAntenna(VECTOR3 dir)
{

}

VHFAMTransceiver::VHFAMTransceiver()
{
	vhfASwitch = NULL;
	vhfBSwitch = NULL;
	rcvSwitch = NULL;
	ctrPowerCB = NULL;
	K1 = false;
	K2 = false;
	receiveA = false;
	receiveB = false;
	transmitA = false;
	transmitB = false;
}

void VHFAMTransceiver::Init(ThreePosSwitch *vhfASw, ThreePosSwitch *vhfBSw, ThreePosSwitch *rcvSw, CircuitBrakerSwitch *ctrpowcb)
{
	vhfASwitch = vhfASw;
	vhfBSwitch = vhfBSw;
	rcvSwitch = rcvSw;
	ctrPowerCB = ctrpowcb;
}

void VHFAMTransceiver::Timestep()
{
	if (vhfASwitch->GetState() != THREEPOSSWITCH_CENTER && ctrPowerCB->IsPowered())
	{
		K1 = true;
	}
	else
	{
		K1 = false;
	}

	if (vhfBSwitch->GetState() != THREEPOSSWITCH_CENTER && ctrPowerCB->IsPowered())
	{
		K2 = true;
	}
	else
	{
		K2 = false;
	}

	if (K1 && vhfASwitch->GetState() != THREEPOSSWITCH_CENTER && ctrPowerCB->IsPowered())
	{
		transmitA = true;
	}
	else
	{
		transmitA = false;
	}

	if (K2 && vhfBSwitch->GetState() != THREEPOSSWITCH_CENTER && ctrPowerCB->IsPowered())
	{
		transmitB = true;
	}
	else
	{
		transmitB = false;
	}

	if ((vhfASwitch->IsDown() || vhfBSwitch->IsUp() || rcvSwitch->IsDown()) && ctrPowerCB->IsPowered())
	{
		receiveA = true;
	}
	else
	{
		receiveA = false;
	}

	if ((vhfBSwitch->IsDown() || vhfASwitch->IsUp() || rcvSwitch->IsUp()) && ctrPowerCB->IsPowered())
	{
		receiveB = true;
	}
	else
	{
		receiveB = false;
	}

	//sprintf(oapiDebugString(), "%d %d %d %d %d %d", K1, K2, transmitA, transmitB, receiveA, receiveB);
}

// Load
void VHFAMTransceiver::LoadState(char *line) {
	int one, two, three, four, five, six;
	
	sscanf(line + 14, "%d %d %d %d %d %d", &one, &two, &three, &four, &five, &six);
	K1 = (one != 0);
	K2 = (two != 0);
	transmitA = (three != 0);
	transmitB = (four != 0);
	receiveA = (five != 0);
	receiveB = (six != 0);
}

// Save
void VHFAMTransceiver::SaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%d %d %d %d %d %d", K1, K2, transmitA, transmitB, receiveA, receiveB);

	oapiWriteScenario_string(scn, "VHFTRANSCEIVER", buffer);
}

VHFRangingSystem::VHFRangingSystem()
{
	sat = NULL;
	powercb = NULL;
	powerswitch = NULL;
	resetswitch = NULL;
	dataGood = false;
	range = 0.0;
	isRanging = false;
	lem = NULL;
	phaseLockTimer = 0.0;
	hasLock = 0;
}

void VHFRangingSystem::Init(Saturn *vessel, CircuitBrakerSwitch *cb, ToggleSwitch *powersw, ToggleSwitch *resetsw, VHFAMTransceiver *transc)
{
	sat = vessel;
	powercb = cb;
	powerswitch = powersw;
	resetswitch = resetsw;
	transceiver =  transc;
}

void VHFRangingSystem::RangingReturnSignal()
{
	hasLock = 3;
}

void VHFRangingSystem::TimeStep(double simdt)
{
	ChannelValue val33;

	val33 = sat->agc.GetInputChannel(033);
	dataGood = false;
	range = 0.0;

	if (!IsPowered())
	{
		val33[RangeUnitDataGood] = 0;
		sat->agc.SetInputChannel(033, val33);
		hasLock = 0;
		isRanging = false;
		return;
	}

	if (!lem)
	{
		VESSEL *lm = sat->agc.GetLM();
		if (lm) lem = (static_cast<LEM*>(lm));
	}

	if (resetswitch->IsUp())
	{
		isRanging = true;
	}

	if (isRanging && transceiver->IsVHFRangingConfig())
	{
		if (lem)
		{
			VECTOR3 R;
			double newrange;

			oapiGetRelativePos(sat->GetHandle(), lem->GetHandle(), &R);
			newrange = length(R);

			if (abs(internalrange - newrange) < 1800.0*0.3048*simdt)
			{
				//Specification is 200NM range, but during the flights up to 320NM was achieved
				if (newrange > 500.0*0.3048 && newrange < 320.0*1852.0)
				{
					lem->SendVHFRangingSignal(sat, false);
				}
			}

			internalrange = newrange;

			if (hasLock)
			{
				if (phaseLockTimer < 13.0)
				{
					phaseLockTimer += simdt;
				}
			}
			else
			{
				phaseLockTimer = 0.0;
			}

			if (phaseLockTimer > 13.0)
			{
				range = internalrange;
				dataGood = true;
			}
		}
	}

	ChannelValue val13;
	val13 = sat->agc.GetInputChannel(013);

	if (dataGood == 1 && val33[RangeUnitDataGood] == 0) { val33[RangeUnitDataGood] = 1; sat->agc.SetInputChannel(033, val33); }
	if (dataGood == 0 && val33[RangeUnitDataGood] == 1) { val33[RangeUnitDataGood] = 0; sat->agc.SetInputChannel(033, val33); }

	if (val13[RangeUnitActivity] == 1) {
		int radarBits = 0;
		if (val13[RangeUnitSelectA] == 1) { radarBits |= 1; }
		if (val13[RangeUnitSelectB] == 1) { radarBits |= 2; }
		if (val13[RangeUnitSelectC] == 1) { radarBits |= 4; }

		switch (radarBits) {
		case 4:
			// Docs says this should be 0.01 NM/bit, or 18.52 meters/bit
			sat->agc.vagc.Erasable[0][RegRNRAD] = (int16_t)fmod(range / 18.52, 32768.0);
			sat->agc.SetInputChannelBit(013, RangeUnitActivity, 0);
			sat->agc.GenerateRadarupt();
			break;
		default:
			break;
		}
	}

	//sprintf(oapiDebugString(), "%d %d %d %f %f %o", isRanging, hasLock, dataGood, range, phaseLockTimer, sat->agc.vagc.Erasable[0][RegRNRAD]);

	//Reset after the timestep
	if (hasLock) hasLock--;
}

void VHFRangingSystem::SystemTimestep(double simdt)
{
	if (IsPowered())
	{
		powercb->DrawPower(10.0);
	}
}

bool VHFRangingSystem::IsPowered()
{
	// Do we have a VHF Ranging System?
	if (!sat->pMission->CSMHasVHFRanging()) return false;

	if (powerswitch->IsUp() && powercb && powercb->IsPowered())
	{
		return true;
	}

	return false;
}

// Load
void VHFRangingSystem::LoadState(char *line) {
	int one, two;

	sscanf(line + 10, "%d %d %d %lf %lf %lf", &one, &two, &hasLock, &internalrange, &range, &phaseLockTimer);
	dataGood = (one != 0);
	isRanging = (two != 0);
}

// Save
void VHFRangingSystem::SaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%d %d %d %lf %lf %lf", dataGood, isRanging, hasLock, internalrange, range, phaseLockTimer);

	oapiWriteScenario_string(scn, "VHFRANGING", buffer);
}

// Socket registration method (registers sockets to be deinitialized
bool PCM::registerSocket(SOCKET sock)
{
	HMODULE hpac = GetModuleHandle("modules\\startup\\ProjectApolloConfigurator.dll");
	if (hpac) {
		bool (__cdecl *regSocket1)(SOCKET);
		regSocket1 = (bool (__cdecl *)(SOCKET)) GetProcAddress(hpac,"pacDefineSocket");
		if (regSocket1)	{
			if (!regSocket1(sock))
				return false;
		} else {
			return false;
		}
	}
	return true;
}
// PCM SYSTEM
PCM::PCM(){
	sat = NULL;
	conn_state = 0;
	uplink_state = 0; rx_offset = 0; 
	mcc_size = 0; mcc_offset = 0;
	wsk_error = 0;
	last_update = 0;
	last_rx = 0;
	pcm_rate_override = 0;
}

void PCM::Init(Saturn *vessel){
	sat = vessel;
	conn_state = 0;
	uplink_state = 0; rx_offset = 0;
	mcc_size = 0; mcc_offset = 0;
	wsk_error = 0;
	last_update = 0;
	last_rx = MINUS_INFINITY;
	word_addr = 0;
	pcm_rate_override = 0;
	int iResult = WSAStartup( MAKEWORD(2,2), &wsaData );
	if ( iResult != NO_ERROR ){
		sprintf(wsk_emsg,"TELECOM: Error at WSAStartup()");
		wsk_error = 1;
		return;
	}
	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );
	if ( m_socket == INVALID_SOCKET ) {
		sprintf(wsk_emsg,"TELECOM: Error at socket(): %ld", WSAGetLastError());
		WSACleanup();
		wsk_error = 1;
		return;
	}
	// Be nonblocking
	int iMode = 1; // 0 = BLOCKING, 1 = NONBLOCKING
	if(ioctlsocket(m_socket, FIONBIO, (u_long FAR*) &iMode) != 0){
		sprintf(wsk_emsg,"TELECOM: ioctlsocket() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}

	// Set up incoming options
	service.sin_family = AF_INET;
	service.sin_addr.s_addr = htonl(INADDR_ANY);
	service.sin_port = htons( 14242 );

	if ( ::bind( m_socket, (SOCKADDR*) &service, sizeof(service) ) == SOCKET_ERROR ) {
		sprintf(wsk_emsg,"TELECOM: bind() failed: %ld", WSAGetLastError());
		wsk_error = 1;
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	if ( listen( m_socket, 1 ) == SOCKET_ERROR ){
		wsk_error = 1;
		sprintf(wsk_emsg,"TELECOM: listen() failed: %ld", WSAGetLastError());
		closesocket(m_socket);
		WSACleanup();
		return;
	}
	if(!registerSocket(m_socket))
	{
		sprintf(wsk_emsg,"TELECOM: Failed to register socket %i for cleanup",m_socket);
		wsk_error = 1;
	}
	conn_state = 1; // INITIALIZED, LISTENING
	uplink_state = 0; rx_offset = 0;
}

void PCM::SystemTimestep(double simdt) {	
	// Anything on?	
	// Up telemetry power
	if(sat->UPTLMSwitch2.GetState() != THREEPOSSWITCH_DOWN){
		// Command reset?
		if(sat->UPTLMSwitch2.GetState() == THREEPOSSWITCH_UP){
			rx_offset = 0; uplink_state = 0; // Do that and continue
		}
		if(sat->FlightBus.Voltage() > 28){
			sat->FlightBus.DrawPower(8.9);
		}
	}
	// Down telemetry power
	if(sat->TelcomGroup1Switch.Voltage() > 100){
		if(sat->TelcomGroup2Switch.Voltage() > 100){
			// Both
			sat->TelcomGroup1Switch.DrawPower(10.5);
			sat->TelcomGroup2Switch.DrawPower(10.5);
			return;
		}else{
			// Just G1
			sat->TelcomGroup1Switch.DrawPower(21);
			return;
		}
	}else{
		if(sat->TelcomGroup2Switch.Voltage() > 100){
			// Just G2
			sat->TelcomGroup2Switch.DrawPower(21);
			return;
		}
	}
	// No power (has no effect for now)
	return;
}

void PCM::TimeStep(double simt){
	// This stuff has to happen every timestep, regardless of system status.
	if(wsk_error != 0){
		sprintf(oapiDebugString(),"%s",wsk_emsg);
		// return;
	}
	// For now, don't care about voltages and such.

	// Allow IO to check for connections, etc
	// FIXME: Should we maintain the downlink interrupt rate?
	/*
	if(conn_state != 2){
		last_update = simt; // Don't care about rate		
		perform_io(simt);
		return;
	}
	*/

	// Generate PCM datastream
	if(pcm_rate_override == 1 || (pcm_rate_override == 0 && sat->PCMBitRateSwitch.GetState() == TOGGLESWITCH_DOWN)){
		tx_size = (int)((simt - last_update) / 0.005);
		// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
		if(tx_size > 0){
			last_update = simt;
			if(tx_size < 1024){
				tx_offset = 0;
				while(tx_offset < tx_size){
					generate_stream_lbr();
					tx_offset++;
				}
				perform_io(simt);
			}
		}
		return; // Don't waste time checking for HBR
	}
	if(pcm_rate_override == 2 || (pcm_rate_override == 0 && sat->PCMBitRateSwitch.GetState() == TOGGLESWITCH_UP)){
		tx_size = (int)((simt - last_update) / 0.00015625);
		// sprintf(oapiDebugString(),"Need to send %d bytes",tx_size);
		if(tx_size > 0){
			last_update = simt;
			if(tx_size < 1024){
				tx_offset = 0;
				while(tx_offset < tx_size){
					generate_stream_hbr();
					tx_offset++;
				}			
				perform_io(simt);
			}
		}
	}
}

// Scale data to 255 steps for transmission in the PCM datastream.
// This function will be called lots of times inside a timestep, so it should go
// as fast as possible!

unsigned char PCM::scale_data(double data, double low, double high){
	double step = 0;
	
	// First eliminate cases outside of the scales
	if(data >= high){ return 0xFF; }
	if(data <= low){  return 0; }
	
	// Now figure step value
	step = ( ( high - low ) / 256.0);
	// and return result
	return static_cast<unsigned char>( ( ( data - low ) / step ) + 0.5 );
}

// Fetch a telemetry data item from its channel code
unsigned char PCM::measure(int channel, int type, int ccode){
	// Status structures.
	TankPressures smTankPress;
	TankQuantities tankQuantities;
	SPSStatus spsStatus;
	FuelCellStatus fcStatus;
	PyroStatus pyroStatus;
	SECSStatus secsStatus;
	RCSStatus rcsStatus;

	unsigned char data = 0;

	switch(type){
		case TLM_A:  // ANALOG
			switch(channel){
				case 10: // S10A
					switch(ccode){
						case 1:			// UNKNOWN - HBR ONLY
							return(0);
						case 2:			// UNKNOWN - HBR ONLY
							return(0);
						case 3:			// CO2 PARTIAL PRESS
							return(scale_data(sat->CO2PartPressSensor.Voltage(), 0.0, 5.0));
						case 4:			// GLY EVAP BACK PRESS
							return(scale_data(sat->GlyEvapBackPressSensor.Voltage(), 0.0, 5.0));
						case 5:			// UNKNOWN - HBR ONLY
							return(0);
						case 6:			// CABIN PRESS
							return(scale_data(sat->CabinPressSensor.Voltage(), 0.0, 5.0));
						case 7:			// UNKNOWN - HBR ONLY
							return(0);
						case 8:			// SEC EVAP OUT STEAM PRESS
							return(scale_data(sat->SecEvapOutSteamPressSensor.Voltage(), 0.0, 5.0));
						case 9:			// WASTE H20 QTY
							return(scale_data(sat->WasteH2OQtySensor.Voltage(), 0.0, 5.0));
						case 10:		// SPS VLV ACT PRESS PRI
							return(scale_data(0,0,5000));
						case 11:		// SPS VLV ACT PRESS SEC
							return(scale_data(0,0,5000));
						case 12:		// GLY EVAP OUT TEMP
							return(scale_data(sat->GlyEvapOutTempSensor.Voltage(), 0.0, 5.0));
						case 13:		// UNKNOWN - HBR ONLY
							return(0);
						case 14:		// ENG CHAMBER PRESS
							sat->GetSPSStatus( spsStatus );
							return(scale_data(spsStatus.chamberPressurePSI, 0, 150));
						case 15:		// ECS RAD OUT TEMP
							return(scale_data(sat->ECSRadOutTempSensor.Voltage(), 0.0, 5.0));
						case 16:		// HE TK TEMP
							return(scale_data(0,-100,200));
						case 17:		// SM ENG PKG B TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_B, rcsStatus );
							return(scale_data(rcsStatus.PackageTempF, 0, 300));
						case 18:		// CM HE TK A PRESS
							sat->GetRCSStatus( RCS_CM_RING_1, rcsStatus );
							return(scale_data(rcsStatus.HeliumPressurePSI, 0, 5000));
						case 19:		// SM ENG PKG C TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_C, rcsStatus );
							return(scale_data(rcsStatus.PackageTempF, 0, 300));
						case 20:		// SM ENG PKG D TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_D, rcsStatus );
							return(scale_data(rcsStatus.PackageTempF, 0, 300));
						case 21:		// CM HE TK B PRESS
							sat->GetRCSStatus( RCS_CM_RING_2, rcsStatus );
							return(scale_data(rcsStatus.HeliumPressurePSI, 0, 5000));
						case 22:		// DOCKING PROBE TEMP
							return(scale_data(0,-100,300));
						case 23:		// UNKNOWN - HBR ONLY
							return(0);
						case 24:		// SM HE TK A PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_A, rcsStatus );
							return(scale_data(rcsStatus.HeliumPressurePSI, 0, 5000));
						case 25:		// UNKNOWN - HBR ONLY
							return(0);
						case 26:		// OX TK 1 QTY -TOTAL AUX
							return(scale_data(0,0,50));
						case 27:		// SM HE TK B PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_B, rcsStatus );
							return(scale_data(rcsStatus.HeliumPressurePSI, 0, 5000));
						case 28:		// OX TK 2 QTY
							return(scale_data(0,0,60));
						case 29:		// FU TK 1 QTY -TOTAL AUX
							return(scale_data(0,0,50));
						case 30:		// SM HE TK C PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_C, rcsStatus );
							return(scale_data(rcsStatus.HeliumPressurePSI, 0, 5000));
						case 31:		// FU TK 2 QTY
							return(scale_data(0,0,60));
						case 32:		// UNKNOWN - HBR ONLY
							return(0);
						case 33:		// SM HE TK D PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_D, rcsStatus );
							return(scale_data(rcsStatus.HeliumPressurePSI, 0, 5000));
						case 34:		// UNKNOWN - HBR ONLY
							return(0);
						case 35:		// UNKNOWN - HBR ONLY
							return(0);
						case 36:		// H2 TK 1 PRESS
							sat->GetTankPressures( smTankPress );
							return(scale_data(smTankPress.H2Tank1PressurePSI, 0, 350));
						case 37:		// SPS VLV BODY TEMP
							return(scale_data(0,0,200));
						case 38:		// UNKNOWN - HBR ONLY
							return(0);
						case 39:		// H2 TK 2 PRESS
							sat->GetTankPressures( smTankPress );
							return(scale_data(smTankPress.H2Tank2PressurePSI, 0, 350));
						case 40:		// UNKNOWN - HBR ONLY
							return(0);
						case 41:		// UNKNOWN - HBR ONLY
							return(0);
						case 42:		// O2 TK 2 QTY
							sat->GetTankQuantities( tankQuantities );
							return(scale_data(tankQuantities.O2Tank2Quantity * 100.0, 0, 100));
						case 43:		// UNKNOWN - HBR ONLY
							return(0);
						case 44:		// OX LINE 1 TEMP
							return(scale_data(0,0,200));
						case 45:		// SUIT AIR HX OUT TEMP
							return(scale_data(sat->SuitTempSensor.Voltage(), 0.0, 5.0));
						case 46:		// UNKNOWN - HBR ONLY
							return(0);
						case 47:		// SPS INJECTOR FLANGE TEMP 1
							return(scale_data(sat->sce.GetVoltage(2, 9), 0.0, 5.0));
						case 48:		// PRI RAD IN TEMP
							return(scale_data(sat->PriRadInTempSensor.Voltage(), 0.0, 5.0));
						case 49:		// SPS INJECTOR FLANGE TEMP 2
							return(scale_data(sat->sce.GetVoltage(2, 10), 0.0, 5.0));
						case 50:		// UNKNOWN - HBR ONLY
							return(0);
						case 51:		// FC 1 COND EXH TEMP
							return(scale_data(sat->sce.GetVoltage(2, 3), 0.0, 5.0));
						case 52:		// UNKNOWN - HBR ONLY
							return(0);
						case 53:		// UNKNOWN - HBR ONLY
							return(0);
						case 54:		// O2 TK 1 TEMP
							return(scale_data(sat->O2Tank1TempSensor.Voltage(), 0.0, 5.0));
						case 55:		// UNKNOWN - HBR ONLY
							return(0);
						case 56:		// UNKNOWN - HBR ONLY
							return(0);
						case 57:		// O2 TK 2 TEMP
							return(scale_data(sat->O2Tank2TempSensor.Voltage(), 0.0, 5.0));
						case 58:		// UNKNOWN - HBR ONLY
							return(0);
						case 59:		// FU LINE 1 TEMP
							return(scale_data(0,0,200));
						case 60:		// H2 TK 1 TEMP
							return(scale_data(sat->H2Tank1TempSensor.Voltage(), 0.0, 5.0));
						case 61:		// NUCLEAR PARTICLE DETECTOR TEMP
							return(scale_data(0,-109,140));
						case 62:		// NUCLEAR PARTICLE ANALYZER TEMP
							return(scale_data(0,-109,140));
						case 63:		// H2 TK 2 TEMP
							return(scale_data(sat->H2Tank2TempSensor.Voltage(), 0.0, 5.0));
						case 64:		// UNKNOWN - HBR ONLY
							return(0);
						case 65:		// SIDE HS BOND LOC 1 TEMP
							return(scale_data(0,-260,600));
						case 66:		// O2 TK 2 PRESS
							sat->GetTankPressures( smTankPress );
							return(scale_data(smTankPress.O2Tank2PressurePSI, 50, 1050));
						case 67:		// FC 3 RAD IN TEMP
							sat->GetFuelCellStatus(3, fcStatus);
							return(scale_data(fcStatus.RadiatorTempInF, -50, 300));
						case 68:		// UNKNOWN - HBR ONLY
							return(0);
						case 69:		// FC 3 COND EXH TEMP
							return(scale_data(sat->sce.GetVoltage(2, 5), 0.0, 5.0));
						case 70:		// SIDE HS BOND LOC 2 TEMP
							return(scale_data(0,-260,600));
						case 71:		// UNKNOWN - HBR ONLY
							return(0);
						case 72:		// FC 1 SKIN TEMP
							return(scale_data(sat->sce.GetVoltage(2, 6), 0.0, 5.0));
						case 73:		// UNKNOWN - HBR ONLY
							return(0);
						case 74:		// SIDE HS BOND LOC 3 TEMP
							return(scale_data(0,-260,600));
						case 75:		// FC 2 SKIN TEMP
							return(scale_data(sat->sce.GetVoltage(2, 7), 0.0, 5.0));
						case 76:		// UNKNOWN - HBR ONLY
							return(0);
						case 77:		// UNKNOWN - HBR ONLY
							return(0);
						case 78:		// FC 3 SKIN TEMP
							return(scale_data(sat->sce.GetVoltage(2, 8), 0.0, 5.0));
						case 79:		// SIDE HS BOND LOC 4 TEMP
							return(scale_data(0,-260,600));
						case 80:		// UNKNOWN - HBR ONLY
							return(0);
						case 81:		// POTABLE H20 QTY
							return(scale_data(sat->PotH2OQtySensor.Voltage(), 0.0, 5.0));
						case 82:		// UNKNOWN - HBR ONLY
							return(0);
						case 83:		// PIPA +120 VDC
							return(scale_data(0,85,135));
						case 84:		// CABIN TEMP
							return(scale_data(sat->CabinTempSensor.Voltage(), 0.0, 5.0));
						case 85:		// 3.2 KHz 28V SUPPLY
							return(scale_data(0,0,31.1));
						case 86:		// INVERTER 1 TEMP
							return(scale_data(sat->sce.GetVoltage(2, 0), 0.0, 5.0));
						case 87:		// SEC RAD IN TEMP
							return(scale_data(sat->SecRadInTempSensor.Voltage(), 0.0, 5.0));
						case 88:		// INVERTER 2 TEMP
							return(scale_data(sat->sce.GetVoltage(2, 1), 0.0, 5.0));
						case 89:		// INVERTER 3 TEMP
							return(scale_data(sat->sce.GetVoltage(2, 2), 0.0, 5.0));
						case 90:		// SEC RAD OUT TEMP
							return(scale_data(sat->SecRadOutTempSensor.Voltage(), 0.0, 5.0));
						case 91:		// IMU 28 VAC 800Hz
							return(scale_data(0,0,31.1));
						case 92:		// UNKNOWN - HBR ONLY
							return(0);
						case 93:		// SM HE PRESS/TEMP RATIO A
							return(scale_data(0,0,100));
						case 94:		// UNKNOWN - HBR ONLY
							return(0);
						case 95:		// UNKNOWN - HBR ONLY
							return(0);
						case 96:		// PIPA TEMP
							return(scale_data(0,120,140));
						case 97:		// UNKNOWN - HBR ONLY
							return(0);
						case 98:		// UNKNOWN - HBR ONLY
							return(0);
						case 99:		// SM HE PRESS/TEMP RATIO B
							return(scale_data(0,0,100));
						case 100:		// PRI EVAP INLET TEMP
							return(scale_data(sat->PriEvapInletTempSensor.Voltage(), 0.0, 5.0));
						case 101:		// H2O TANK - GLY RES PRESS
							return(scale_data(0,0,50));
						case 102:		// SM HE PRESS/TEMP RATIO C
							return(scale_data(0,0,100));
						case 103:		// UNKNOWN - HBR ONLY
							return(0);
						case 104:		// SCI EXP #3
							return(scale_data(0,0,100));
						case 105:		// SM HE PRESS/TEMP RATIO D
							return(scale_data(0,0,100));
						case 106:		// SCI EXP #4
							return(scale_data(0,0,100));
						case 107:		// SCI EXP #5
							return(scale_data(0,0,100));
						case 108:		// SCI EXP #1
							return(scale_data(0,0,100));
						case 109:		// SCI EXP #6
							return(scale_data(0,0,100));
						case 110:		// SCI EXP #7
							return(scale_data(0,0,100));
						case 111:		// SCI EXP #2
							return(scale_data(0,0,100));
						case 112:		// SCI EXP #8
							return(scale_data(0,0,100));
						case 113:		// SCI EXP #9
							return(scale_data(0,0,100));
						case 114:		// H2O DUMP TEMP
							return(scale_data(0,0,100));
						case 115:		// SCI EXP #10
							return(scale_data(0,0,100));
						case 116:		// SCI EXP #11
							return(scale_data(0,0,100));
						case 117:		// SPS FU FEED LINE TEMP
							sat->GetSPSStatus(spsStatus);
							return(scale_data(spsStatus.PropellantLineTempF,0,200));
						case 118:		// SCI EXP #12
							return(scale_data(0,0,100));
						case 119:		// SCI EXP #13
							return(scale_data(0,0,100));
						case 120:		// SPS OX FEED LINE TEMP
							sat->GetSPSStatus(spsStatus);
							return(scale_data(spsStatus.OxidizerLineTempF,0,200));
						case 121:		// SCI EXP #14
							return(scale_data(0,0,100));
						case 122:		// SCI EXP #15
							return(scale_data(0,0,100));
						case 123:		// FC 2 COND EXH TEMP
							return(scale_data(sat->sce.GetVoltage(2, 4), 0.0, 5.0));
						case 124:		// UNKNOWN - HBR ONLY
							return(0);
						case 125:		// UNKNOWN - HBR ONLY
							return(0);
						case 126:		// FC 1 RAD OUT TEMP
							sat->GetFuelCellStatus( 1, fcStatus );
							return(scale_data(fcStatus.RadiatorTempOutF, -50, 300));
						case 127:		// UNKNOWN - HBR ONLY
							return(0);
						case 128:		// UNKNOWN - HBR ONLY
							return(0);
						case 129:		// FC 2 RAD OUT TEMP
							sat->GetFuelCellStatus( 2, fcStatus );
							return(scale_data(fcStatus.RadiatorTempOutF, -50, 300));
						case 130:		// FC 1 RAD IN TEMP
							sat->GetFuelCellStatus( 1, fcStatus );
							return(scale_data(fcStatus.RadiatorTempInF, -50, 300));
						case 131:		// FC 2 RAD IN TEMP
							sat->GetFuelCellStatus( 2, fcStatus );
							return(scale_data(fcStatus.RadiatorTempInF, -50, 300));
						case 132:		// FC 3 RAD OUT TEMP
							sat->GetFuelCellStatus( 3, fcStatus );
							return(scale_data(fcStatus.RadiatorTempOutF, -50, 300));
						case 133:		// GLY EVAP OUT STEAM TEMP
							return(scale_data(sat->GlyEvapOutSteamTempSensor.Voltage(), 0.0, 5.0));
						case 134:		// UNKNOWN - HBR ONLY
							return(0);
						case 135:		// URINE DUMP NOZZLE TEMP
							return(scale_data(0,0,100));
						case 136:		// SM ENG PKG A TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_A, rcsStatus );
							return(scale_data(rcsStatus.PackageTempF, 0, 300));
						case 137:		// BAY 3 OX TK SURFACE TEMP
							return(scale_data(0,-100,200));
						case 138:		// TM BIAS 2.5 VDC
							return(scale_data(0,0,5));
						case 139:		// BAY 5 FU TK SURFACE TEMP
							return(scale_data(0,-100,200));
						case 140:		// BAY 6 FU TK SURFACE TEMP
							return(scale_data(0,-100,200));
						case 141:		// H2 TK 1 QTY
							sat->GetTankQuantities(tankQuantities);
							return(scale_data(tankQuantities.H2Tank1Quantity * 100.0, 0, 100));
						case 142:		// BAY 2 OX TK SURFACE TEMP
							return(scale_data(0,-100,200));
						case 143:		// OX LINE ENTRY SUMP TK TEMP
							return(scale_data(0,-100,200));
						case 144:		// H2 TK 2 QTY
							sat->GetTankQuantities( tankQuantities );
							return(scale_data(tankQuantities.H2Tank2Quantity * 100.0, 0, 100));
						case 145:		// FU LINE ENTRY SUMP TK TEMP
							return(scale_data(0,-100,200));
						case 146:		// UNKNOWN - HBR ONLY
							return(0);
						case 147:		// O2 TK 1 QTY
							sat->GetTankQuantities( tankQuantities );
							return(scale_data(tankQuantities.O2Tank1Quantity * 100.0, 0, 100));
						case 148:		// UNKNOWN - HBR ONLY
							return(0);
						case 149:		// DOSIMETER RATE
							return(scale_data(0,0,5));
						case 150:		// O2 TK 1 PRESS
							sat->GetTankPressures( smTankPress );
							return(scale_data(smTankPress.O2Tank1PressurePSI, 50, 1050));
						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 10-A-%d",ccode);
							break;
					}
					break;
				case 11: // S11A
					switch(ccode){
						case 1:			// SUIT MANF ABS PRESS
							return(scale_data(sat->SuitPressSensor.Voltage(), 0.0, 5.0));
						case 2:			// SUIT COMP DELTA P
							return(scale_data(sat->SuitComprDeltaPMeter.Voltage(), 0.0, 5.0));
						case 3:			// GLY PUMP OUT PRESS
							return(scale_data(sat->GlycolPumpOutPressSensor.Voltage(), 0.0, 5.0));
						case 4:			// ECS SURGE TANK PRESS
							return(scale_data(sat->O2SurgeTankPressSensor.Voltage(), 0.0, 5.0));
						case 5:			// PYRO BUS B VOLTS
							sat->GetPyroStatus( pyroStatus );
							return(scale_data(pyroStatus.BusBVoltage, 0, 40 ));
						case 6:			// LES LOGIC BUS B VOLTS
							sat->GetSECSStatus( secsStatus );
							return(scale_data( secsStatus.BusBVoltage, 0, 40 ));
						case 7:			// UNKNOWN - HBR ONLY
							return(0);
						case 8:			// LES LOGIC BUS A VOLTS
							sat->GetSECSStatus( secsStatus );
							return(scale_data( secsStatus.BusBVoltage, 0, 40 ));
						case 9:			// PYRO BUS A VOLTS
							sat->GetSECSStatus( secsStatus );
							return(scale_data( secsStatus.BusAVoltage, 0, 40 ));
						case 10:		// SPS HE TK PRESS
							return(scale_data(sat->GetSPSPropellant()->GetHeliumPressurePSI(), 0, 5000));
						case 11:		// SPS OX TK PRESS
							return(scale_data(sat->GetSPSPropellant()->GetPropellantPressurePSI(), 0, 250));
						case 12:		// SPS FU TK PRESS
							return(scale_data(sat->GetSPSPropellant()->GetPropellantPressurePSI(), 0, 250));
						case 13:		// GLY ACCUM QTY
							return(scale_data(sat->GlycolAccumQtySensor.Voltage(), 0.0, 5.0));
						case 14:		// ECS O2 FLOW O2 SUPPLY MANF
							return(scale_data(sat->ECSO2FlowO2SupplyManifoldSensor.Voltage(), 0.0, 5.0));
						case 15:		// UNKNOWN - HBR ONLY
							return(0);
						case 16:		// DOSIMETER 2 RADIATION
							return(scale_data(0,0,1000));
						case 17:		// PROTON CT RATE CHAN 1
							return(scale_data(0,1,100000));
						case 18:		// PROTON CT RATE CHAN 2
							return(scale_data(0,0.1,10000));
						case 19:		// PROTON CT RATE CHAN 3
							return(scale_data(0,0.1,10000));
						case 20:		// PROTON CT RATE CHAN 4
							return(scale_data(0,0.1,10000));
						case 21:		// CM HE MANIF 1 PRESS
							return(scale_data(0,0,400));
						case 22:		// CM HE MANIF 2 PRESS
							return(scale_data(0,0,400));
						case 23:		// SM OX MANF A PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_A, rcsStatus );
							return(scale_data(rcsStatus.PropellantPressurePSI, 0, 300));
						case 24:		// SM OX MANF B PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_B, rcsStatus );
							return(scale_data(rcsStatus.PropellantPressurePSI, 0, 300));
						case 25:		// UNKNOWN - HBR ONLY
							return(0);
						case 26:		// UNKNOWN - HBR ONLY
							return(0);
						case 27:		// SM OX MANF C PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_C, rcsStatus );
							return(scale_data(rcsStatus.PropellantPressurePSI,0,300));
						case 28:		// SM OX MANF D PRESS
							sat->GetRCSStatus( RCS_SM_QUAD_D, rcsStatus );
							return(scale_data(rcsStatus.PropellantPressurePSI, 0, 300));
						case 29:		// FC 1 N2 PRESS
							return(scale_data(0,0,75));
						case 30:		// FC 2 N2 PRESS
							return(scale_data(0,0,75));
						case 31:		// FU/OX VLV 1 POS
							return(scale_data(0,0,90));
						case 32:		// FU/OX VLV 2 POS
							return(scale_data(0,0,90));
						case 33:		// FU/OX VLV 3 POS
							return(scale_data(0,0,90));
						case 34:		// FU/OX VLV 4 POS
							return(scale_data(0,0,90));
						case 35:		// FC 3 N2 PRESS
							return(scale_data(0,0,75));
						case 36:		// UNKNOWN - HBR ONLY
							return(0);
						case 37:		// SUIT-CABIN DELTA PRESS
							return(scale_data(sat->SuitCabinDeltaPressSensor.Voltage(), 0.0, 5.0));
						case 38:		// ALPHA CT RATE CHAN 1
							return(scale_data(0,0.1,10000));
						case 39:		// SM HE MANF A PRESS
							return(scale_data(0,0,400));
						case 40:		// SM HE MANF B PRESS
							return(scale_data(0,0,400));
						case 41:		// ALPHA CT RATE CHAN 2
							return(scale_data(0,0.1,10000));
						case 42:		// ALPHA CT RATE CHAN 3
							return(scale_data(0,0.1,10000));
						case 43:		// PROTON INTEG CT RATE
							return(scale_data(0,1,100000));
						case 44:		// UNKNOWN - HBR ONLY
							return(0);
						case 45:		// UNKNOWN - HBR ONLY
							return(0);
						case 46:		// SM HE MANF C PRESS
							return(scale_data(0,0,400));
						case 47:		// LM HEATER CURRENT
							return(scale_data(sat->sce.GetVoltage(1, 7), 0.0, 5.0));
						case 48:		// PCM HI LEVEL 85 PCT REF
							return(scale_data(0,0,5));
						case 49:		// PCM LO LEVEL 15 PCT REF
							return(scale_data(0,0,1));
						case 50:		// USB RCVR PHASE ERR
							return(scale_data(0,-90000,90000));
						case 51:		// ANGLE OF ATTACK
							return(scale_data(DEG * fabs(sat->GetAOA()), 0, 5));
						case 52:		// SHAFT CDU DAC OUT
							return(scale_data(0,-10,10));
						case 53:		// TRUNNION CDU DAC OUT
							return(scale_data(0,-10,10));
						case 54:		// IG 1X RSVR OUT SIN
							return(scale_data(0,-50,50));
						case 55:		// O2 SUPPLY MANF PRESS
							return(scale_data(sat->O2SupplyManifPressSensor.Voltage(), 0.0, 5.0));
						case 56:		// AC BUS 2 PH A VOLTS
							return(scale_data(sat->sce.GetVoltage(3, 1), 0.0, 5.0));
						case 57:		// MAIN BUS A VOLTS
							return scale_data(sat->sce.GetVoltage(0, 0), 0.0, 5.0);
						case 58:		// MAIN BUS B VOLTS
							return scale_data(sat->sce.GetVoltage(0, 1), 0.0, 5.0);
						case 59:		// IG 1X RSVR OUT COS
							return(scale_data(0,130,50));
						case 60:		// MG 1X RSVR OUT SIN
							return(scale_data(0,-50,50));
						case 61:		// MG 1X RSVR OUT COS
							return(scale_data(0,130,50));
						case 62:		// OG 1X RSVR OUT SIN
							return(scale_data(0,-50,50));
						case 63:		// OG 1X RSVR OUT COS
							return(scale_data(0,130,50));
						case 64:		// UNKNOWN - HBR ONLY
							return(0);
						case 65:		// UNKNOWN - HBR ONLY
							return(0);
						case 66:		// UNKNOWN - HBR ONLY
							return(0);
						case 67:		// FC 1 O2 PRESS
							sat->GetFuelCellStatus( 1, fcStatus );
							return(scale_data(fcStatus.O2PressurePSI, 0, 75));
						case 68:		// FC 2 O2 PRESS
							sat->GetFuelCellStatus( 2, fcStatus );
							return(scale_data(fcStatus.O2PressurePSI, 0, 75));
						case 69:		// FC 3 O2 PRESS
							sat->GetFuelCellStatus( 3, fcStatus );
							return(scale_data(fcStatus.O2PressurePSI, 0, 75));
						case 70:		// FC 1 H2 PRESS
							sat->GetFuelCellStatus( 1, fcStatus );
							return(scale_data(fcStatus.H2PressurePSI, 0, 75));
						case 71:		// FC 2 H2 PRESS
							sat->GetFuelCellStatus( 2, fcStatus );
							return(scale_data(fcStatus.H2PressurePSI, 0, 75));
						case 72:		// FC 3 H2 PRESS
							sat->GetFuelCellStatus( 3, fcStatus );
							return(scale_data(fcStatus.H2PressurePSI, 0, 75));
						case 73:		// BAT CHARGER AMPS
							return scale_data(sat->sce.GetVoltage(1, 0), 0.0, 5.0);
						case 74:		// BAT A CUR
							return scale_data(sat->sce.GetVoltage(1, 1), 0.0, 5.0);
						case 75:		// BAT RELAY BUS VOLTS
							return scale_data(sat->sce.GetVoltage(0, 4), 0.0, 5.0);
						case 76:		// FC 1 CUR
							return scale_data(sat->sce.GetVoltage(1, 4), 0.0, 5.0);
						case 77:		// FC 1 H2 FLOW
							sat->GetFuelCellStatus( 1, fcStatus );
							return(scale_data(fcStatus.H2FlowLBH, 0, 0.2));
						case 78:		// FC 2 H2 FLOW
							sat->GetFuelCellStatus( 2, fcStatus );
							return(scale_data(fcStatus.H2FlowLBH, 0, 0.2));
						case 79:		// FC 3 H2 FLOW
							sat->GetFuelCellStatus( 3, fcStatus );
							return(scale_data(fcStatus.H2FlowLBH, 0, 0.2));
						case 80:		// FC 1 O2 FLOW
							sat->GetFuelCellStatus( 1, fcStatus );
							return(scale_data(fcStatus.O2FlowLBH, 0, 1.6));
						case 81:		// FC 2 O2 FLOW
							sat->GetFuelCellStatus( 2, fcStatus );
							return(scale_data(fcStatus.O2FlowLBH, 0, 1.6));
						case 82:		// FC 3 O2 FLOW
							sat->GetFuelCellStatus( 3, fcStatus );
							return(scale_data(fcStatus.O2FlowLBH, 0, 1.6));
						case 83:		// UNKNOWN - HBR ONLY
							return(0);
						case 84:		// FC 2 CUR
							return scale_data(sat->sce.GetVoltage(1, 5), 0.0, 5.0);
						case 85:		// FC 3 CUR
							return scale_data(sat->sce.GetVoltage(1, 6), 0.0, 5.0);
						case 86:		// UNKNOWN - HBR ONLY
							return(0);
						case 87:		// PRI GLY FLOW RATE
							return(scale_data(0,150,300));
						case 88:		// UNKNOWN - HBR ONLY
							return(0);
						case 89:		// UNKNOWN - HBR ONLY
							return(0);
						case 90:		// UNKNOWN - HBR ONLY
							return(0);
						case 91:		// BAT BUS A VOLTS
							return scale_data(sat->sce.GetVoltage(0, 2), 0.0, 5.0);
						case 92:		// SM FU MANF A PRESS
							return(scale_data(0,0,400));
						case 93:		// BAT BUS B VOLTS
							return scale_data(sat->sce.GetVoltage(0, 3), 0.0, 5.0);
						case 94:		// SM FU MANF B PRESS
							return(scale_data(0,0,400));
						case 95:		// UNKNOWN - HBR ONLY
							return(0);
						case 96:		// UNKNOWN - HBR ONLY
							return(0);
						case 97:		// UNKNOWN - HBR ONLY
							return(0);
						case 98:		// UNKNOWN - HBR ONLY
							return(0);
						case 99:		// UNKNOWN - HBR ONLY
							return(0);
						case 100:		// UNKNOWN - HBR ONLY
							return(0);
						case 101:		// UNKNOWN - HBR ONLY
							return(0);
						case 102:		// UNKNOWN - HBR ONLY
							return(0);
						case 103:		// UNKNOWN - HBR ONLY
							return(0);
						case 104:		// UNKNOWN - HBR ONLY
							return(0);
						case 105:		// UNKNOWN - HBR ONLY
							return(0);
						case 106:		// UNKNOWN - HBR ONLY
							return(0);
						case 107:		// UNKNOWN - HBR ONLY
							return(0);
						case 108:		// UNKNOWN - HBR ONLY
							return(0);
						case 109:		// BAT B CUR
							return scale_data(sat->sce.GetVoltage(1, 2), 0.0, 5.0);
						case 110:		// BAT C CUR
							return scale_data(sat->sce.GetVoltage(1, 3), 0.0, 5.0);
						case 111:		// SM FU MANF C PRESS
							return(scale_data(0,0,400));
						case 112:		// SM FU MANF D PRESS
							return(scale_data(0,0,400));
						case 113:		// UNKNOWN - HBR ONLY
							return(0);
						case 114:		// UNKNOWN - HBR ONLY
							return(0);
						case 115:		// UNKNOWN - HBR ONLY
							return(0);
						case 116:		// UNKNOWN - HBR ONLY
							return(0);
						case 117:		// UNKNOWN - HBR ONLY
							return(0);
						case 118:		// SEC EVAP OUT LIQ TEMP
							return(scale_data(sat->SecEvapOutLiqTempSensor.Voltage(), 0.0, 5.0));
						case 119:		// SENSOR EXCITATION 5V
							return(scale_data(0,0,9));
						case 120:		// SENSOR EXCITATION 10V
							return(scale_data(0,0,15));
						case 121:		// USB RCVR AGC VOLTAGE
							return(scale_data(0,-130,-50));
						case 122:		// UNKNOWN - HBR ONLY
							return(0);
						case 123:		// UNKNOWN - HBR ONLY
							return(0);
						case 124:		// UNKNOWN - HBR ONLY
							return(0);
						case 125:		// UNKNOWN - HBR ONLY
							return(0);
						case 126:		// UNKNOWN - HBR ONLY
							return(0);
						case 127:		// UNKNOWN - HBR ONLY
							return(0);
						case 128:		// UNKNOWN - HBR ONLY
							return(0);
						case 129:		// SEC GLY ACCUM QTY
							return(scale_data(sat->SecGlycolAccumQtySensor.Voltage(), 0.0, 5.0));
						case 130:		// SM HE MANF D PRESS
							return(scale_data(0,0,400));
						case 131:		// UNKNOWN - HBR ONLY
							return(0);
						case 132:		// UNKNOWN - HBR ONLY
							return(0);
						case 133:		// UNKNOWN - HBR ONLY
							return(0);
						case 134:		// UNKNOWN - HBR ONLY
							return(0);
						case 135:		// UNKNOWN - HBR ONLY
							return(0);
						case 136:		// UNKNOWN - HBR ONLY
							return(0);
						case 137:		// UNKNOWN - HBR ONLY
							return(0);
						case 138:		// UNKNOWN - HBR ONLY
							return(0);
						case 139:		// UNKNOWN - HBR ONLY
							return(0);
						case 140:		// UNKNOWN - HBR ONLY
							return(0);
						case 141:		// UNKNOWN - HBR ONLY
							return(0);
						case 142:		// UNKNOWN - HBR ONLY
							return(0);
						case 143:		// UNKNOWN - HBR ONLY
							return(0);
						case 144:		// UNKNOWN - HBR ONLY
							return(0);
						case 145:		// UNKNOWN - HBR ONLY
							return(0);
						case 146:		// UNKNOWN - HBR ONLY
							return(0);
						case 147:		// AC BUS 1 PH A VOLTS
							return(scale_data(sat->sce.GetVoltage(3, 0), 0.0, 5.0));
						case 148:		// SCE POS SUPPLY VOLTS
							return(scale_data(0,0,30));
						case 149:		// UNKNOWN - HBR ONLY
							return(0);
						case 150:		// UNKNOWN - HBR ONLY
							return(0);
						case 151:		// UNKNOWN - HBR ONLY
							return(0);
						case 152:		// FUEL SM/ENG INTERFACE P
							return(scale_data(0,0,300));
						case 153:		// UNKNOWN - HBR ONLY
							return(0);
						case 154:		// SCE NEG SUPPLY VOLTS
							return(scale_data(0, -30, 0));
						case 155:		// CM HE TK A TEMP
							sat->GetRCSStatus( RCS_CM_RING_1, rcsStatus );
							return(scale_data(rcsStatus.HeliumTempF, 0, 300));
						case 156:		// CM HE TK B TEMP
							sat->GetRCSStatus( RCS_CM_RING_2, rcsStatus );
							return(scale_data(rcsStatus.HeliumTempF, 0, 300));
						case 157:		// SEC GLY PUMP OUT PRESS
							return(scale_data(sat->SecGlyPumpOutPressSensor.Voltage(), 0.0, 5.0));
						case 158:		// UNKNOWN - HBR ONLY
							return(0);
						case 159:		// UNKNOWN - HBR ONLY
							return(0);
						case 160:		// UNKNOWN - HBR ONLY
							return(0);
						case 161:		// OX SM/ENG INTERFACE P
							return(scale_data(0,0,300));
						case 162:		// UNKNOWN - HBR ONLY
							return(0);
						case 163:		// SM HE TK A TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_A, rcsStatus );
							return(scale_data(rcsStatus.HeliumTempF, 0, 100));
						case 164:		// SM HE TK B TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_B, rcsStatus );
							return(scale_data(rcsStatus.HeliumTempF, 0, 100));
						case 165:		// SM HE TK C TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_C, rcsStatus );
							return(scale_data(rcsStatus.HeliumTempF, 0, 100));
						case 166:		// SM HE TK D TEMP
							sat->GetRCSStatus( RCS_SM_QUAD_D, rcsStatus );
							return(scale_data(rcsStatus.HeliumTempF, 0, 100));
						case 167:		// UNKNOWN - HBR ONLY
							return(0);
						case 168:		// UNKNOWN - HBR ONLY
							return(0);
						case 169:		// DOSIMETER 1 RADIATION
							return(scale_data(0,0,1000));
						case 170:		// UNKNOWN - HBR ONLY
						case 171:		// UNKNOWN - HBR ONLY
						case 172:		// UNKNOWN - HBR ONLY
						case 173:		// UNKNOWN - HBR ONLY
						case 174:		// UNKNOWN - HBR ONLY
						case 175:		// UNKNOWN - HBR ONLY
						case 176:		// UNKNOWN - HBR ONLY
						case 177:		// UNKNOWN - HBR ONLY
						case 178:		// UNKNOWN - HBR ONLY
						case 179:		// UNKNOWN - HBR ONLY
						case 180:		// UNKNOWN - HBR ONLY
							return(0);

						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 11-A-%d",ccode);
							break;
					}
					break;
				case 12: // S12A
					switch(ccode){
						case 1:			// MGA SERVO ERR IN PHASE
							return(scale_data(0,-2.5,2.5));
						case 2:			// IGA SERVO ERR IN PHASE
							return(scale_data(0,-2.5,2.5));
						case 3:			// OGA SERVO ERR IN PHASE
							return(scale_data(0,-2.5,2.5));
						case 4:			// ROLL ATT ERR
							return(scale_data(sat->eda.GetConditionedRollAttErr(), 0.0, 5.0));
						case 5:			// SCS PITCH BODY RATE
							return(scale_data(sat->eda.GetInstPitchAttRate(), 0.0, 5.0));
						case 6:			// SCS YAW BODY RATE
							return(scale_data(sat->eda.GetInstYawAttRate(), 0.0, 5.0));
						case 7:			// SCS ROLL BODY RATE
							return(scale_data(sat->eda.GetInstRollAttRate(), 0.0, 5.0));
						case 8:			// PITCH GIMBL POS 1 OR 2
							return(scale_data(0,-5,5));
						case 9:			// CM X-AXIS ACCEL
							return(scale_data(0,-2,10));
						case 10:		// YAW GIMBL POS 1 OR 2
							return(scale_data(0,-5,5));
						case 11:		// CM Y-AXIS ACCEL
							return(scale_data(0,-2,2));
						case 12:		// CM Z-AXIS ACCEL
							return(scale_data(0,-2,2));
						case 13:		// SCS TVC YAW AUTO CMD
							return(scale_data(0,-10,10));
						case 14:        // UNKNOWN - HBR ONLY
							return(0);
						case 15:		// SCS TVC PITCH AUTO CMD
							return(scale_data(0,-10,10));
						case 16:		// YAW DIFF CLUTCH CURRENT
							return(scale_data(0,-0.807,0.807));

						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 12-A-%d",ccode);
							break;
					}
					break;
				case 22: // S22A
					switch(ccode){
						case 1:			// ASTRO 1 EKG AXIS 2
							return(scale_data(0,0.1,0.5));
						case 2:			// ASTRO 1 EKG AXIS 3
							return(scale_data(0,0.1,0.5));
						case 3:			// ASTRO 1 EKG AXIS 1
							return(scale_data(0,0.1,0.5));
						case 4:			// PITCH DIFF CLUTCH CURRENT
							return(scale_data(0,-0.807,0.807));
						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 22-A-%d",ccode);
							break;
					}
					break;
				case 51: // S51A
					switch(ccode){
						case 1:			// UNKNOWN - HBR ONLY
						case 2:			// UNKNOWN - HBR ONLY
						case 3:			// UNKNOWN - HBR ONLY
						case 4:			// UNKNOWN - HBR ONLY
							return(0);
						case 5:			// PITCH ATT ERR
							return(scale_data(sat->eda.GetConditionedPitchAttErr(), 0.0, 5.0));
						case 6:			// YAW ATT ERR
							return(scale_data(sat->eda.GetConditionedYawAttErr(), 0.0, 5.0));
						case 7:			// ASTRO 1 RESPIR
							return(scale_data(0,-5,5));
						case 8:			// ASTRO 2 RESPIR
							return(scale_data(0,-5,5));
						case 9:			// ASTRO 3 RESPIR
							return(scale_data(0,-5,5));
						case 10:		// UNKNOWN - HBR ONLY
							return(0);
						case 11:		// MTVC PITCH CMD
							return(scale_data(0,-11.5,11.5));
						case 12:		// MTVC YAW CMD
							return(scale_data(0,-11.5,11.5));
						case 13:		// ROT ROLL CMD
							return(scale_data(0,-11.5,11.5));
						case 14:		// UNKNOWN - HBR ONLY
						case 15:
							return(0);
						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 51-A-%d",ccode);
							break;
					}
					break;
				default:
					sprintf(sat->debugString(),"MEASURE: UNKNOWN %d-A-%d",channel,ccode);
					break;
			}
			break;
		case TLM_DS: // DIGITAL SERIAL
			/* NOT CALLED - ONLY USED AS 51DS1 FOR CMC DOWNTELEMETRY */
			sprintf(sat->debugString(),"MEASURE: UNKNOWN %d-DS-%d",channel,ccode);
			break;
		case TLM_DP: // DIGITAL PARALLEL (SAME THING AS EVENT BITS)
		case TLM_E:  // EVENT BITS
			switch(channel){
				case 10: // S10E
					switch(ccode){
						case 1: // S10DP1 - PCM BIT RATE CHANGE (?)
							return(0);
						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 10-E-%d",ccode);
							break;
					}
					break;
				case 11: // S11E
					switch(ccode){
						case 2:	        // S11DP2 CTE TIME (4 WORDS, HBR ONLY)
							return(0);
						case 3:			// SCI EXP #17
							return(0);
						case 4:
							/* 3 = SM EDS ABORT REQUEST B
							   4 = SPS SOL DRIVER 1
							   5 = SM EDS ABORT REQUEST A
							   6 = MASTER CAUTION WARNING ON
							   8 = RAD FLOW CONT SYS 1 OR 2
								*/
							data |= ((sat->sce.GetVoltage(0, 13) > 2.5) << 2);
							data |= ((sat->sce.GetVoltage(0, 12) > 2.5) << 4);
							data |= ((sat->sce.GetVoltage(0, 14) > 2.5) << 5);
							data |= ((sat->sce.GetVoltage(0, 5) > 2.5) << 7);

							return data;
						case 5:			// SCI EXP #18
							return(0);
						case 6:			// SCI EXP #19
							return(0);
						case 7:			// SCI EXP #20
							return(0);
						case 8:
							/* 2 = ATT DEADBAND MIN
							   4 = HI RATE LIMIT
							   5 = FDAI ERR 5 RATE 5
							   6 = FDAI SCALE ERR 50/15
							   7 = GYRO 1 COMB SMRD
							   8 = GYRO 2 COMB SMRD
								*/
							return(0);
						case 9:
							/* 3 = DV CG LM/CSM
							   4 = SPS SOL DRIVER 2
							   5 = S/C CONT SCS
								*/
							return(0);
						case 10:		// SCI EXP #21
							return(0);
						case 11:		// SCI EXP #22
							return(0);
						case 12:		// SCI EXP #23
							return(0);
						case 13:
							/* 3 = CREW ABORT A
							   4 = EDS ABORT B
							   6 = CREW ABORT B
							   7 = EDS ABORT A
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.CrewAbortA << 2);
							data |= (secsStatus.EDSAbortLogicOutputB << 3);
							data |= (secsStatus.CrewAbortB << 5);
							data |= (secsStatus.EDSAbortLogicOutputA << 6);
							return data;
						case 14:
							/* 1 = EDS ABORT VOTE 1
							   2 = EDS ABORT VOTE 2
							   4 = EDS ABORT VOTE 3
							   5 = DSE TAPE MOTION
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.EDSAbortLogicInput1 << 0);
							data |= (secsStatus.EDSAbortLogicInput2 << 1);
							data |= (secsStatus.EDSAbortLogicInput3 << 3);
							return data;
						case 15:
							/*	2 = IMU HTR +28 VDC
							    3 = CMC OPERATE +28 VDC
								4 = OPTX OPERATE 28 VAC
							    6 = CSM-LM LOCK RING SEP RELAY A
								7 = CSM-LM LOCK RING SEP RELAY B
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.CSMLEMLockRingSepRelaySignalA << 5);
							data |= (secsStatus.CSMLEMLockRingSepRelaySignalB << 6);
							return data;
						case 16:		// SCI EXP #16
							return(0);
						case 17:		// SCI EXP #24
							return(0);
						case 18:		// SCI EXP #25
							return(0);
						case 19:
							/* 1 = CMC WARNING
								*/
							return(0);
						case 20: // ZEROES
						case 21: // ZEROES
							return(0);
						case 22:
							/* 1 = CM-SM SEP RELAY A 
							   3 = SCS CHANNEL ENABLE RCS A
							   4 = TRANS CTL +X CMD
							   5 = SLA SEP RELAY A
							   6 = TRANS CTL -X CMD
							   7 = CM RCS PRESS SIG A
							   8 = TRANS CTL +Y CMD
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.CMSMSepRelayCloseA << 0);
							data |= (secsStatus.RCSActivateSignalA << 2);
							data |= (secsStatus.SLASepRelayA << 4);
							data |= (secsStatus.CMRCSPressureSignalA << 6);
							return data;
						case 23:
							/* 1 = CM-SM SEP RELAY B
							   3 = SCS CHANNEL ENABLE RCS B
							   5 = CM RCS PRESS SIG B
							   6 = TRANS CTL -Y CMD
							   7 = SLA SEP RELAY B
							   8 = TRANS CTL +Z CMD
								*/;
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.CMSMSepRelayCloseB << 0);
							data |= (secsStatus.RCSActivateSignalB << 2);
							data |= (secsStatus.CMRCSPressureSignalB << 4);
							data |= (secsStatus.SLASepRelayB << 6);
							return data;
						case 24:
							/* 1 = FWD HS JET A
							   2 = TRANS CTL -Z CMD
							   3 = DIRECT RCS #1
							   4 = DIRECT RCS #2
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.FwdHeatshieldJettA << 0);
							return data;
						case 25:
							/* 1 = LIMIT CYCLE
							   3 = MANUAL ATT PITCH ACCEL CMD
							   4 = MANUAL ATT PITCH MIN IMP
							   5 = MANUAL ATT YAW ACCEL CMD
							   6 = MANUAL ATT YAW MIN IMP
							   7 = MANUAL ATT ROLL ACCEL CMD
							   8 = MANUAL ATT ROLL MIN IMP
								*/
							return(0);
						case 26:
							/* 5 = FWD HS JET B
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.FwdHeatshieldJettB << 4);
							return data;
						case 27: // ZEROES
							return(0);
						case 28:
							/* 1 = FC 1 PH
							   2 = FC 2 PH
							   3 = FC 3 PH
							   */							   
							return(0);
						case 29:
							/* 1 = DROGUE SEP RELAY A
							   5 = MAIN CHUTE DISC RELAY A
							   8 = MAIN DEPLOY RELAY A
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.DrogueSepRelayA << 0);
							data |= (secsStatus.MainChuteDiscRelayA << 4);
							data |= (secsStatus.MainDeployRelayA << 7);
							return data;
						case 30:
							/* 3 = MAIN DEPLOY RELAY B
							   6 = DROGUE SEP RELAY B
							   8 = MAIN CHUTE DISC RELAY B
								*/
							sat->GetSECSStatus(secsStatus);

							data |= (secsStatus.MainDeployRelayB << 2);
							data |= (secsStatus.DrogueSepRelayB << 5);
							data |= (secsStatus.MainChuteDiscRelayB << 7);
							return data;
						case 31: // ZEROES
							return(0);
						case 32:
							/* 8 = PCM SYNC SRC INT OR EXT
								*/
							return(0);
						case 33:
							/* 1 = BMAG MODE SW-ROLL ATT 1 RT 2
							   2 = BMAG MODE SW-ROLL RATE 2
							   3 = BMAG MODE SW-PITCH ATT 1 RT 2
							   4 = BMAG MODE SW-PITCH RATE 2
							   5 = BMAG MODE SW-YAW ATT 1 RT 2
							   6 = BMAG MODE SW-YAW RATE 2
								*/
							data |= ((sat->sce.GetVoltage(0, 6) > 2.5) << 0);
							data |= ((sat->sce.GetVoltage(0, 7) > 2.5) << 1);
							data |= ((sat->sce.GetVoltage(0, 8) > 2.5) << 2);
							data |= ((sat->sce.GetVoltage(0, 9) > 2.5) << 3);
							data |= ((sat->sce.GetVoltage(0, 10) > 2.5) << 4);
							data |= ((sat->sce.GetVoltage(0, 11) > 2.5) << 5);
							return data;
						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 11-E-%d",ccode);
							break;
					}
					break;
				case 22: // S22E
					switch(ccode){
						case 1:
							/* 1 = +PITCH/+X
								*/
							return(0);
						case 2:
							/* 2 = -PITCH/+X
								*/
							return(0);
						case 3:
							/* 3 = +PITCH/-X
								*/
							return(0);
						case 4:
							/* 1 = -PITCH/-X
								*/
							return(0);
						case 5:
							/* 5 = +YAW/+X
								*/
							return(0);
						case 6:
							/* 6 = -YAW/+X
								*/
							return(0);
						case 7:
							/* 7 = +YAW/-X
								*/
							return(0);
						case 8:
							/* 8 = -YAW/-X
								*/
							return(0);
						case 9:
							/* 1 = +ROLL/+Z
								*/
							return(0);
						case 10:
							/* 2 = -ROLL/-Z
								*/
							return(0);
						case 11:
							/* 3 = -ROLL/+Y
								*/
							return(0);
						case 12:
							/* 4 = +ROLL/-Y
								*/
							return(0);
						case 13:
							/* 5 = -ROLL/+Z
								*/
							return(0);
						case 14:
							/* 6 = +ROLL/-Z
								*/
							return(0);
						case 15:
							/* 7 = +ROLL/+Y
								*/
							return(0);
						case 16:
							/* 8 = -ROLL/-Y
								*/
							return(0);
						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 22-E-%d",ccode);
							break;
					}
					break;
				case 51: // S51E
					switch(ccode){
						case 2: // UDL VALIDITY BITS
							return(0);
						default:
							sprintf(sat->debugString(),"MEASURE: UNKNOWN 51-E-%d",ccode);
							break;
					}
					break;
				default:
					sprintf(sat->debugString(),"MEASURE: UNKNOWN %d-E-%d",channel,ccode);
					break;
			}
			break;
		case TLM_SRC:  // "SRC" (channel is always 0)
			switch(ccode){
				case 0: // SOURCE OF ZEROES
					return(0);
				case 1: // SOURCE OF ONES
					return(0xFF);
				default:
					sprintf(sat->debugString(),"MEASURE: UNKNOWN SRC-%d",ccode);
					break;
			}
			break;
	}
	return (0);
}

void PCM::generate_stream_lbr(){
	unsigned char data=0;
	// 40 words per frame, 5 frames, 1 frame per second
	switch(word_addr){
		case 0: // SYNC 1
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();
			// And continue
			tx_data[tx_offset] = 05;
			break;
		case 1: // SYNC 2
			tx_data[tx_offset] = 0171;
			break;
		case 2: // SYNC 3
			tx_data[tx_offset] = 0267;
			break;
		case 3: // SYNC 4 & FRAME COUNT
			tx_data[tx_offset] = (0300|frame_count);
			break;
		case 4: 
			switch(frame_count){
				case 0: // 11A1 ECS: SUIT MANF ABS PRESS					
					tx_data[tx_offset] = measure(11,TLM_A,1);
					break;
				case 1: // 11A109 EPS: BAT B CURR
					tx_data[tx_offset] = measure(11,TLM_A,109);
					break;
				case 2: // 11A46 RCS: SM HE MANF C PRESS
					tx_data[tx_offset] = measure(11,TLM_A,46);
					break;
				case 3: // 11A154 CMI: SCE NEG SUPPLY VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,154);
					break;
				case 4: // 11A91 EPS: BAT BUS A VOLTS					
					tx_data[tx_offset] = measure(11,TLM_A,91);
					break;
			}
			break;
		case 5:
			switch(frame_count){
				case 0: // 11A2 ECS: SUIT COMP DELTA P
					tx_data[tx_offset] = measure(11,TLM_A,2);
					break;
				case 1: // 11A110 EPS: BAT C CURR
					tx_data[tx_offset] = measure(11,TLM_A,110);
					break;
				case 2: // 11A47 EPS: LM HEATER CURRENT
					tx_data[tx_offset] = measure(11,TLM_A,47);
					break;
				case 3: // 11A155 RCS: CM HE TK A TEMP
					tx_data[tx_offset] = measure(11,TLM_A,155);
					break;
				case 4: // 11A92 RCS: SM FU MANF A PRESS
					tx_data[tx_offset] = measure(11,TLM_A,92);
					break;
			}
			break;
		case 6:
			switch(frame_count){
				case 0: // 11A3 ECS: GLY PUMP OUT PRESS
					tx_data[tx_offset] = measure(11,TLM_A,3);
					break;
				case 1: // 11A111 ECS: SM FU MANF C PRESS
					tx_data[tx_offset] = measure(11,TLM_A,111);
					break;
				case 2: // 11A48 PCM HI LEVEL 85 PCT REF
					tx_data[tx_offset] = measure(11,TLM_A,48);
					break;
				case 3: // 11A156 CM HE TK B TEMP
					tx_data[tx_offset] = measure(11,TLM_A,156);
					break;
				case 4: // 11A93 BAT BUS B VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,93);
					break;
			}
			break;
		case 7:
			switch(frame_count){
				case 0: // 11A4 ECS SURGE TANK PRESS
					tx_data[tx_offset] = measure(11,TLM_A,4);
					break;
				case 1: // 11A112 SM FU MANF D PRESS
					tx_data[tx_offset] = measure(11,TLM_A,112);
					break;
				case 2: // 11A49 PC HI LEVEL 15 PCT REF
					tx_data[tx_offset] = measure(11,TLM_A,49);
					break;
				case 3: // 11A157 SEC GLY PUMP OUT PRESS
					tx_data[tx_offset] = measure(11,TLM_A,157);
					break;
				case 4: // 11A94 SM FU MANF B PRESS
					tx_data[tx_offset] = measure(11,TLM_A,94);
					break;
			}
			break;
		case 8: // 51DS1A COMPUTER DIGITAL DATA (40 BITS) 
		case 28:
		{
			unsigned char data;
			ChannelValue ch13;
			ch13 = sat->agc.GetOutputChannel(013);
			data = (sat->agc.GetOutputChannel(034) & 077400) >> 8;
			if (ch13[DownlinkWordOrderCodeBit]) { data |= 0200; } // WORD ORDER BIT
			/*
			sprintf(oapiDebugString(),"CMC DATA: %o (%lo %lo)",data,sat->agc.GetOutputChannel(034),
				sat->agc.GetOutputChannel(035));
				*/
			tx_data[tx_offset] = data;
			break;
		}
		case 9: // 51DS1B COMPUTER DIGITAL DATA (40 BITS)
		case 29:
			data = (sat->agc.GetOutputChannel(034)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 10: // 51DS1C COMPUTER DIGITAL DATA (40 BITS) 
		case 30:
			// PARITY OF CH 34 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(035)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 11: // 51DS1D COMPUTER DIGITAL DATA (40 BITS) 
		case 31:
			data = (sat->agc.GetOutputChannel(035)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 12: // 51DS1E COMPUTER DIGITAL DATA (40 BITS) 
		case 32:
			// PARITY OF CH 35 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 13: // 51DP2 UP-DATA-LINK VALIDITY BITS (4 BITS)
		case 33:
			tx_data[tx_offset] = 0; 
			break;
		case 14:
			switch(frame_count){
				case 0: // 10A123 FC 2 COND EXH TEMP
					tx_data[tx_offset] = measure(10,TLM_A,123);
					break;
				case 1: // 10A126 FC 1 RAD OUT TEMP
					tx_data[tx_offset] = measure(10,TLM_A,126);
					break;
				case 2: // 10A129 FC 2 RAD OUT TEMP
					tx_data[tx_offset] = measure(10,TLM_A,129);
					break;
				case 3: // 10A132 FC 3 RAD OUT TEMP
					tx_data[tx_offset] = measure(10,TLM_A,132);
					break;
				case 4: // 10A135 URINE DUMP NOZZLE TEMP
					tx_data[tx_offset] = measure(10,TLM_A,135);
					break;
			}
			break;
		case 15:
			switch(frame_count){
				case 0: // 10A138 TM BIAS 2.5 VDC
					tx_data[tx_offset] = measure(10,TLM_A,138);
					break;
				case 1: // 10A141 EPS: H2 TK 1 QTY
					tx_data[tx_offset] = measure(10,TLM_A,141);
					break;
				case 2: // 10A144 H2 TK 2 QTY
					tx_data[tx_offset] = measure(10,TLM_A,144);
					break;
				case 3: // 10A147 O2 TK 1 QTY
					tx_data[tx_offset] = measure(10,TLM_A,147);
					break;
				case 4: // 10A150 O2 TK 1 PRESS
					tx_data[tx_offset] = measure(10,TLM_A,150);
					break;
			}
			break;
		case 16:
			switch(frame_count){
				case 0: // 10A3
					tx_data[tx_offset] = measure(10,TLM_A,3);
					break;
				case 1: // 10A6
					tx_data[tx_offset] = measure(10,TLM_A,6);
					break;
				case 2: // 10A9
					tx_data[tx_offset] = measure(10,TLM_A,9);
					break;
				case 3: // 10A12
					tx_data[tx_offset] = measure(10,TLM_A,12);
					break;
				case 4: // 10A15
					tx_data[tx_offset] = measure(10,TLM_A,15);
					break;
			}
			break;
		case 17:
			switch(frame_count){
				case 0: // 10A18
					tx_data[tx_offset] = measure(10,TLM_A,18);
					break;
				case 1: // 10A21
					tx_data[tx_offset] = measure(10,TLM_A,21);
					break;
				case 2: // 10A24
					tx_data[tx_offset] = measure(10,TLM_A,24);
					break;
				case 3: // 10A27
					tx_data[tx_offset] = measure(10,TLM_A,27);
					break;
				case 4: // 10A30
					tx_data[tx_offset] = measure(10,TLM_A,30);
					break;
			}
			break;
		case 18:
			switch(frame_count){
				case 0: // 10A33 
					tx_data[tx_offset] = measure(10,TLM_A,33);
					break;
				case 1: // 10A36 H2 TK 1 PRESS
					tx_data[tx_offset] = measure(10,TLM_A,36);
					break;
				case 2: // 10A39 H2 TK 2 PRESS
					tx_data[tx_offset] = measure(10,TLM_A,39);
					break;
				case 3: // 10A42 O2 TK 2 QTY
					tx_data[tx_offset] = measure(10,TLM_A,42);
					break;
				case 4: // 10A45
					tx_data[tx_offset] = measure(10,TLM_A,45);
					break;
			}
			break;
		case 19:
			switch(frame_count){
				case 0: // 10A48 
					tx_data[tx_offset] = measure(10,TLM_A,48);
					break;
				case 1: // 10A51
					tx_data[tx_offset] = measure(10,TLM_A,51);
					break;
				case 2: // 10A54 O2 TK 1 TEMP
					tx_data[tx_offset] = measure(10,TLM_A,54);
					break;
				case 3: // 10A57 O2 TK 2 TEMP
					tx_data[tx_offset] = measure(10,TLM_A,57);
					break;
				case 4: // 10A60 H2 TK 1 TEMP
					tx_data[tx_offset] = measure(10,TLM_A,60);
					break;
			}
			break;
		case 20:
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();
			// and continue
			switch(frame_count){
				case 0: // 10DP1 
					tx_data[tx_offset] = measure(10,TLM_DP,1);
					break;
				case 1: // 11DP6
					tx_data[tx_offset] = measure(11,TLM_DP,6); 
					break;
				case 2: // 11DP27
					tx_data[tx_offset] = measure(11,TLM_DP,27); 
					break;
				case 3: // 11DP15
					tx_data[tx_offset] = measure(11,TLM_DP,17); 
					break;
				case 4: // 11DP20
					tx_data[tx_offset] = measure(11,TLM_DP,20); 
					break;
			}
			break;
		case 21:
			switch(frame_count){
				case 0: // SRC 0
					tx_data[tx_offset] = measure(0,TLM_SRC,0); 
					break;
				case 1: // 11DP7
					tx_data[tx_offset] = measure(11,TLM_DP,7); 
					break;
				case 2: // 11DP28
					tx_data[tx_offset] = measure(11,TLM_DP,28); 
					break;
				case 3: // 11DP16
					tx_data[tx_offset] = measure(11,TLM_DP,16); 
					break;
				case 4: // 11DP21
					tx_data[tx_offset] = measure(11,TLM_DP,21); 
					break;
			}
			break;
		case 22:
			switch(frame_count){
				case 0: // 11A39
					tx_data[tx_offset] = measure(11,TLM_A,39); 
					break;
				case 1: // 11A147 AC BUS 1 PH A VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,147); 
					break;
				case 2: // 11A84
					tx_data[tx_offset] = measure(11,TLM_A,84); 
					break;
				case 3: // 11A21
					tx_data[tx_offset] = measure(11,TLM_A,21); 
					break;
				case 4: // 11A129
					tx_data[tx_offset] = measure(11,TLM_A,129); 
					break;
			}
			break;
		case 23:
			switch(frame_count){
				case 0: // 11A40
					tx_data[tx_offset] = measure(11,TLM_A,40); 
					break;
				case 1: // 11A48
					tx_data[tx_offset] = measure(11,TLM_A,48); 
					break;
				case 2: // 11A85
					tx_data[tx_offset] = measure(11,TLM_A,85); 
					break;
				case 3: // 11A22
					tx_data[tx_offset] = measure(11,TLM_A,22); 
					break;
				case 4: // 11A130
					tx_data[tx_offset] = measure(11,TLM_A,130); 
					break;
			}
			break;
		case 24:
			switch(frame_count){
				case 0: // 11A73 BAT CHRGR AMPS
					tx_data[tx_offset] = measure(11,TLM_A,73); 
					break;
				case 1: // 11A10
					tx_data[tx_offset] = measure(11,TLM_A,10); 
					break;
				case 2: // 11A118
					tx_data[tx_offset] = measure(11,TLM_A,118); 
					break;
				case 3: // 11A55
					tx_data[tx_offset] = measure(11,TLM_A,55); 
					break;
				case 4: // 11A163
					tx_data[tx_offset] = measure(11,TLM_A,163); 
					break;
			}
			break;
		case 25:
			switch(frame_count){
				case 0: // 11A74 BAT A CUR
					tx_data[tx_offset] = measure(11,TLM_A,74); 
					break;
				case 1: // 11A11
					tx_data[tx_offset] = measure(11,TLM_A,11); 
					break;
				case 2: // 11A119
					tx_data[tx_offset] = measure(11,TLM_A,119); 
					break;
				case 3: // 11A56 AC BUS 2 PH A VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,56); 
					break;
				case 4: // 11A164
					tx_data[tx_offset] = measure(11,TLM_A,164); 
					break;
			}
			break;
		case 26:
			switch(frame_count){
				case 0: // 11A75
					tx_data[tx_offset] = measure(11,TLM_A,75); 
					break;
				case 1: // 11A12
					tx_data[tx_offset] = measure(11,TLM_A,12); 
					break;
				case 2: // 11A120
					tx_data[tx_offset] = measure(11,TLM_A,120); 
					break;
				case 3: // 11A57
					tx_data[tx_offset] = measure(11,TLM_A,57); 
					break;
				case 4: // 11A165
					tx_data[tx_offset] = measure(11,TLM_A,165); 
					break;
			}
			break;
		case 27:
			switch(frame_count){
				case 0: // 11A76
					tx_data[tx_offset] = measure(11,TLM_A,76); 
					break;
				case 1: // 11A13
					tx_data[tx_offset] = measure(11,TLM_A,13); 
					break;
				case 2: // 11A121
					tx_data[tx_offset] = measure(11,TLM_A,121); 
					break;
				case 3: // 11A58
					tx_data[tx_offset] = measure(11,TLM_A,58); 
					break;
				case 4: // 11A166
					tx_data[tx_offset] = measure(11,TLM_A,166); 
					break;
			}
			break;
		case 34:
			switch(frame_count){
				case 0: // 11DP3
					tx_data[tx_offset] = measure(11,TLM_DP,3); 
					break;
				case 1: // 11DP8
					tx_data[tx_offset] = measure(11,TLM_DP,8); 
					break;
				case 2: // 11DP13
					tx_data[tx_offset] = measure(11,TLM_DP,13); 
					break;
				case 3: // 11DP29
					tx_data[tx_offset] = measure(11,TLM_DP,29); 
					break;
				case 4: // 11DP22
					tx_data[tx_offset] = measure(11,TLM_DP,22); 
					break;
			}
			break;
		case 35:
			switch(frame_count){
				case 0: // SRC 1
					tx_data[tx_offset] = measure(0,TLM_SRC,1); 
					break;
				case 1: // 11DP9
					tx_data[tx_offset] = measure(11,TLM_DP,9); 
					break;
				case 2: // 11DP14
					tx_data[tx_offset] = measure(11,TLM_DP,14); 
					break;
				case 3: // 11DP17
					tx_data[tx_offset] = measure(11,TLM_DP,17); 
					break;
				case 4: // 11DP23
					tx_data[tx_offset] = measure(11,TLM_DP,23); 
					break;
			}
			break;
		case 36:
			switch(frame_count){
				case 0: // 10A63 H2 TK 2 TEMP
					tx_data[tx_offset] = measure(10,TLM_A,63); 
					break;
				case 1: // 10A66 O2 TK 2 PRESS
					tx_data[tx_offset] = measure(10,TLM_A,66); 
					break;
				case 2: // 10A69
					tx_data[tx_offset] = measure(10,TLM_A,69); 
					break;
				case 3: // 10A72
					tx_data[tx_offset] = measure(10,TLM_A,72); 
					break;
				case 4: // 10A75
					tx_data[tx_offset] = measure(10,TLM_A,75); 
					break;
			}
			break;
		case 37:
			switch(frame_count){
				case 0: // 10A78
					tx_data[tx_offset] = measure(10,TLM_A,78); 
					break;
				case 1: // 10A81
					tx_data[tx_offset] = measure(10,TLM_A,81); 
					break;
				case 2: // 10A84
					tx_data[tx_offset] = measure(10,TLM_A,84); 
					break;
				case 3: // 10A87
					tx_data[tx_offset] = measure(10,TLM_A,87); 
					break;
				case 4: // 10A90
					tx_data[tx_offset] = measure(10,TLM_A,90); 
					break;
			}
			break;
		case 38:
			switch(frame_count){
				case 0: // 10A93
					tx_data[tx_offset] = measure(10,TLM_A,93); 
					break;
				case 1: // 10A96
					tx_data[tx_offset] = measure(10,TLM_A,96); 
					break;
				case 2: // 10A99
					tx_data[tx_offset] = measure(10,TLM_A,99); 
					break;
				case 3: // 10A102
					tx_data[tx_offset] = measure(10,TLM_A,102); 
					break;
				case 4: // 10A105
					tx_data[tx_offset] = measure(10,TLM_A,105); 
					break;
			}
			break;
		case 39:
			switch(frame_count){
				case 0: // 10A108
					tx_data[tx_offset] = measure(10,TLM_A,108); 
					break;
				case 1: // 10A111
					tx_data[tx_offset] = measure(10,TLM_A,11);  
					break;
				case 2: // 10A114
					tx_data[tx_offset] = measure(10,TLM_A,114); 
					break;
				case 3: // 10A117
					tx_data[tx_offset] = measure(10,TLM_A,117); 
					break;
				case 4: // 10A120
					tx_data[tx_offset] = measure(10,TLM_A,120); 
					break;
			}
			break;
		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 39){
		word_addr = 0;
		frame_addr++;
		if(frame_addr > 4){
			frame_addr = 0;
		}
		frame_count++;
		if(frame_count > 5){
			frame_count = 0;
		}
	}
}

void PCM::generate_stream_hbr(){
	unsigned char data=0;
	// 128 words per frame, 50 frames pre second
	switch(word_addr){
		case 0: // SYNC 1
			tx_data[tx_offset] = 05;
			break;
		case 1: // SYNC 2
			tx_data[tx_offset] = 0171;
			break;
		case 2: // SYNC 3
			tx_data[tx_offset] = 0267;
			break;
		case 3: // SYNC 4 & FRAME COUNT
			tx_data[tx_offset] = (0300|frame_addr);
			break;
		case 4: // 22A1 ASTRO 1 EKG AXIS 2
		case 36:
		case 68:
		case 100:
			tx_data[tx_offset] = measure(22,TLM_A,1);
			break;
		case 5: // 22A2 ASTRO 1 EKG AXIS 3
		case 37:
		case 69:
		case 101:
			tx_data[tx_offset] = measure(22,TLM_A,2);
			break;
		case 6: // 22A3 ASTRO 1 EKG AXIS 1
		case 38:
		case 70:
		case 102:
			tx_data[tx_offset] = measure(22,TLM_A,3);
			break;
		case 7: // 22A4 PITCH DIFF CLUTCH CURRENT
		case 39:
		case 71:
		case 103:
			tx_data[tx_offset] = measure(22,TLM_A,4);
			break;
		case 8:
			switch(frame_count){
				case 0: // 11A1 SUIT MANF ABS PRESS
					tx_data[tx_offset] = measure(11,TLM_A,1); 
					break;
				case 1: // 11A37 SUIT-CABIN DELTA PRESS
					tx_data[tx_offset] = measure(11,TLM_A,37); 
					break;
				case 2: // 11A73 BAT CHARGER AMPS
					tx_data[tx_offset] = measure(11,TLM_A,73); 
					break;
				case 3: // 11A109 BAT B CUR
					tx_data[tx_offset] = measure(11,TLM_A,109); 
					break;
				case 4: // 11A145
					tx_data[tx_offset] = measure(11,TLM_A,145); 
					break;
			}
			break;
		case 9: 
			switch(frame_count){
				case 0: // 11A2 SUIT COMP DELTA P
					tx_data[tx_offset] = measure(11,TLM_A,2); 
					break;
				case 1: // 11A38 ALPHA CT RATE CHAN 1
					tx_data[tx_offset] = measure(11,TLM_A,38); 
					break;
				case 2: // 11A74 BAT A CUR
					tx_data[tx_offset] = measure(11,TLM_A,74); 
					break;
				case 3: // 11A110 BAT C CUR
					tx_data[tx_offset] = measure(11,TLM_A,110); 
					break;
				case 4: // 11A146
					tx_data[tx_offset] = measure(11,TLM_A,146); 
					break;
			}
			break;
		case 10:
			switch(frame_count){
				case 0: // 11A3 GLY PUMP OUT PRESS
					tx_data[tx_offset] = measure(11,TLM_A,3); 
					break;
				case 1: // 11A39 SM HE MANF A PRESS
					tx_data[tx_offset] = measure(11,TLM_A,39); 
					break;
				case 2: // 11A75 BAT RELAY BUS VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,75); 
					break;
				case 3: // 11A111 SM FU MANF C PRESS
					tx_data[tx_offset] = measure(11,TLM_A,111); 
					break;
				case 4: // 11A147 AC BUS 1 PH A VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,147); 
					break;
			}
			break;
		case 11:
			switch(frame_count){
				case 0: // 11A4 ECS SURGE TANK PRESS
					tx_data[tx_offset] = measure(11,TLM_A,4); 
					break;
				case 1: // 11A40 SM HE MANF B PRESS
					tx_data[tx_offset] = measure(11,TLM_A,40); 
					break;
				case 2: // 11A76 FC 1 CUR
					tx_data[tx_offset] = measure(11,TLM_A,76); 
					break;
				case 3: // 11A112 SM FU MANF D PRESS
					tx_data[tx_offset] = measure(11,TLM_A,112); 
					break;
				case 4: // 11A148 SCE POS SUPPLY VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,148); 
					break;
			}
			break;
		case 12: // 12A1 MGA SERVO ERR IN PHASE
		case 76:
			tx_data[tx_offset] = measure(12,TLM_A,1); 
			break;
		case 13: // 12A2 IGA SERVO ERR IN PHASE
		case 77:
			tx_data[tx_offset] = measure(12,TLM_A,2); 
			break;
		case 14: // 12A3 OGA SERVO ERR IN PHASE
		case 78:
			tx_data[tx_offset] = measure(12,TLM_A,3); 
			break;
		case 15: // 12A4 ROLL ATT ERR
		case 79:
			tx_data[tx_offset] = measure(12,TLM_A,4); 
			break;
		case 16:
			switch(frame_count){
				case 0: // 11A5 PYRO BUS B VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,5); 
					break;
				case 1: // 11A41 ALPHA CT RATE CHAN 2
					tx_data[tx_offset] = measure(11,TLM_A,41); 
					break;
				case 2: // 11A77 FC 1 H2 FLOW
					tx_data[tx_offset] = measure(11,TLM_A,77); 
					break;
				case 3: // 11A113
					tx_data[tx_offset] = measure(11,TLM_A,113); 
					break;
				case 4: // 11A149
					tx_data[tx_offset] = measure(11,TLM_A,149); 
					break;
			}
			break;
		case 17: // 22DP1
			tx_data[tx_offset] = measure(22,TLM_DP,1); 
			break;
		case 18: // 22DP2
			tx_data[tx_offset] = measure(22,TLM_DP,2); 
			break;
		case 19: 
			switch(frame_count){
				case 0: // 10DP1
					tx_data[tx_offset] = measure(10,TLM_DP,1); 
					break;
				case 1: // SRC-0
					tx_data[tx_offset] = measure(0,TLM_SRC,0); 
					break;
				case 2: // SRC-1
					tx_data[tx_offset] = measure(0,TLM_SRC,1); 
					break;
				case 3: // (Zeroes?)
					tx_data[tx_offset] = 0;
					break;
				case 4: // (Zeroes?)
					tx_data[tx_offset] = 0;
					break;
			}
			break;
		case 20: // 12A5 SCS PITCH BODY RATE
		case 84:
			tx_data[tx_offset] = measure(12,TLM_A,5); 
			break;
		case 21: // 12A6 SCS YAW BODY RATE
		case 85:
			tx_data[tx_offset] = measure(12,TLM_A,6); 
			break;
		case 22: // 12A7 SCS ROLL BODY RATE
		case 86:
			tx_data[tx_offset] = measure(12,TLM_A,7); 
			break;
		case 23: // 12A8 PITCH GIMBL POS 1 OR 2
		case 87:
			tx_data[tx_offset] = measure(12,TLM_A,8); 
			break;
		case 24:
			switch(frame_count){
				case 0: // 11A6 LES LOGIC BUS B VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,6); 
					break;
				case 1: // 11A42 ALPHA CT RATE CHAN 3
					tx_data[tx_offset] = measure(11,TLM_A,42); 
					break;
				case 2: // 11A78 FC 2 H2 FLOW
					tx_data[tx_offset] = measure(11,TLM_A,78); 
					break;
				case 3: // 11A114
					tx_data[tx_offset] = measure(11,TLM_A,114); 
					break;
				case 4: // 11A150
					tx_data[tx_offset] = measure(11,TLM_A,150); 
					break;
			}
			break;
		case 25:
			switch(frame_count){
				case 0: // 11A7
					tx_data[tx_offset] = measure(11,TLM_A,7); 
					break;
				case 1: // 11A43 PROTON INTEG CT RATE
					tx_data[tx_offset] = measure(11,TLM_A,43); 
					break;
				case 2: // 11A79 FC 3 H2 FLOW
					tx_data[tx_offset] = measure(11,TLM_A,79); 
					break;
				case 3: // 11A115
					tx_data[tx_offset] = measure(11,TLM_A,115); 
					break;
				case 4: // 11A151
					tx_data[tx_offset] = measure(11,TLM_A,151); 
					break;
			}
			break;
		case 26:
			switch(frame_count){
				case 0: // 11A8 LES LOGIC BUS A VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,8); 
					break;
				case 1: // 11A44
					tx_data[tx_offset] = measure(11,TLM_A,44);  
					break;
				case 2: // 11A80 FC 1 O2 FLOW
					tx_data[tx_offset] = measure(11,TLM_A,80); 
					break;
				case 3: // 11A116
					tx_data[tx_offset] = measure(11,TLM_A,116); 
					break;
				case 4: // 11A152 FUEL SM/ENG INTERFACE P
					tx_data[tx_offset] = measure(11,TLM_A,152); 
					break;
			}
			break;
		case 27:
			switch(frame_count){
				case 0: // 11A9 PYRO BUS A VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,9); 
					break;
				case 1: // 11A45
					tx_data[tx_offset] = measure(11,TLM_A,45); 
					break;
				case 2: // 11A81 FC 2 O2 FLOW
					tx_data[tx_offset] = measure(11,TLM_A,81); 
					break;
				case 3: // 11A117
					tx_data[tx_offset] = measure(11,TLM_A,117); 
					break;
				case 4: // 11A153
					tx_data[tx_offset] = measure(11,TLM_A,153); 
					break;
			}
			break;
		case 28: // 51A1
			tx_data[tx_offset] = measure(51,TLM_A,1); 
			break;
		case 29: // 51A2
			tx_data[tx_offset] = measure(51,TLM_A,2); 
			break;
		case 30: // 51A3
			tx_data[tx_offset] = measure(51,TLM_A,3); 
			break;
		case 31: // 51DS1A COMPUTER DIGITAL DATA (40 BITS)
			// The very first pass through this loop will get garbage data because there was no downrupt.
			// Generating a downrupt at 0 doesn't give the CMC enough time to get data on the busses.
		{
			ChannelValue ch13;
			ch13 = sat->agc.GetOutputChannel(013);
			data = (sat->agc.GetOutputChannel(034) & 077400) >> 8;
			if (ch13[DownlinkWordOrderCodeBit]) { data |= 0200; } // WORD ORDER BIT
			/*
			sprintf(oapiDebugString(),"CMC DATA: %o (%lo %lo)",data,sat->agc.GetOutputChannel(034),
				sat->agc.GetOutputChannel(035));
			*/
			tx_data[tx_offset] = data;
			break;
		}
		case 32: // 51DS1B COMPUTER DIGITAL DATA (40 BITS)
			data = (sat->agc.GetOutputChannel(034)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 33: // 51DS1C COMPUTER DIGITAL DATA (40 BITS)
			// PARITY OF CH 34 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(035)&077400)>>8;
			tx_data[tx_offset] = data; 
			break;
		case 34: // 51DS1C COMPUTER DIGITAL DATA (40 BITS)
			data = (sat->agc.GetOutputChannel(035)&0377);
			tx_data[tx_offset] = data; 
			break;
		case 35: // 51DS1E COMPUTER DIGITAL DATA (40 BITS)
			// PARITY OF CH 35 GOES IN TOP BIT HERE!
			data = (sat->agc.GetOutputChannel(034)&077400)>>8;
			tx_data[tx_offset] = data; 
			// Trigger telemetry END PULSE
			sat->agc.GenerateDownrupt();			
			break;
		case 40:
			switch(frame_count){
				case 0: // 11A10 HE TK PRESS
					tx_data[tx_offset] = measure(11,TLM_A,10); 
					break;
				case 1: // 11A46 SM HE MANF C PRESS
					tx_data[tx_offset] = measure(11,TLM_A,46); 
					break;
				case 2: // 11A82 FC 3 O2 FLOW
					tx_data[tx_offset] = measure(11,TLM_A,82); 
					break;
				case 3: // 11A118 SEC EVAP OUT LIQ TEMP
					tx_data[tx_offset] = measure(11,TLM_A,118); 
					break;
				case 4: // 11A154 SCE NEG SUPPLY VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,154); 
					break;
			}
			break;
		case 41:
			switch(frame_count){
				case 0: // 11A11 OX TK PRESS
					tx_data[tx_offset] = measure(11,TLM_A,11); 
					break;
				case 1: // 11A47 LM HEATER CURRENT
					tx_data[tx_offset] = measure(11,TLM_A,47); 
					break;
				case 2: // 11A83
					tx_data[tx_offset] = measure(11,TLM_A,83); 
					break;
				case 3: // 11A119 SENSOR EXCITATION 5V
					tx_data[tx_offset] = measure(11,TLM_A,119); 
					break;
				case 4: // 11A155 CM HE TK A TEMP
					tx_data[tx_offset] = measure(11,TLM_A,155); 
					break;
			}
			break;
		case 42:
			switch(frame_count){
				case 0: // 11A12 SPS FU TK PRESS
					tx_data[tx_offset] = measure(11,TLM_A,12); 
					break;
				case 1: // 11A48 PCM HI LEVEL 85 PCT REF
					tx_data[tx_offset] = measure(11,TLM_A,48); 
					break;
				case 2: // 11A84 FC 2 CUR
					tx_data[tx_offset] = measure(11,TLM_A,84); 
					break;
				case 3: // 11A120 SENSOR EXCITATION 10V
					tx_data[tx_offset] = measure(11,TLM_A,120); 
					break;
				case 4: // 11A156 CM HE TK B TEMP
					tx_data[tx_offset] = measure(11,TLM_A,156); 
					break;
			}
			break;
		case 43:
			switch(frame_count){
				case 0: // 11A13 GLY ACCUM QTY
					tx_data[tx_offset] = measure(11,TLM_A,13); 
					break;
				case 1: // 11A49 PCM LO LEVEL 15 PCT REF
					tx_data[tx_offset] = measure(11,TLM_A,49); 
					break;
				case 2: // 11A85 FC 3 CUR
					tx_data[tx_offset] = measure(11,TLM_A,85); 
					break;
				case 3: // 11A121 USB RCVR AGC VOLTAGE
					tx_data[tx_offset] = measure(11,TLM_A,121); 
					break;
				case 4: // 11A157 SEC GLY PUMP OUT PRESS
					tx_data[tx_offset] = measure(11,TLM_A,157); 
					break;
			}
			break;
		case 44: // 12A9 CM X-AXIS ACCEL
		case 108:
			tx_data[tx_offset] = measure(12,TLM_A,9); 
			break;
		case 45: // 12A10 YAW GIMBL POS 1 OR 2
		case 109:
			tx_data[tx_offset] = measure(12,TLM_A,10); 
			break;
		case 46: // 12A11 CM Y-AXIS ACCEL
		case 110:
			tx_data[tx_offset] = measure(12,TLM_A,11); 
			break;
		case 47: // 12A12 CM Z-AXIS ACCEL
		case 111:
			tx_data[tx_offset] = measure(12,TLM_A,12); 
			break;
		case 48:
			switch(frame_count){
				case 0: // 11A14 ECS O2 FLOW O2 SUPPLY MANF
					tx_data[tx_offset] = measure(11,TLM_A,14);  
					break;
				case 1: // 11A50 USB RCVR PHASE ERR
					tx_data[tx_offset] = measure(11,TLM_A,50); 
					break;
				case 2: // 11A86
					tx_data[tx_offset] = measure(11,TLM_A,86); 
					break;
				case 3: // 11A122
					tx_data[tx_offset] = measure(11,TLM_A,122); 
					break;
				case 4: // 11A158
					tx_data[tx_offset] = measure(11,TLM_A,158); 
					break;
			}
			break;
		case 49: // 22DP1
			tx_data[tx_offset] = measure(22,TLM_DP,1); 
			break;
		case 50: // 22DP2
			tx_data[tx_offset] = measure(22,TLM_DP,2); 
			break;
		case 51: // MAGICAL WORD 1
			// 10A1
			// 10A4
			// 10A7
			// ...
			// 10A148
			tx_data[tx_offset] = measure(10,TLM_A,1+(frame_addr*3));
			break;
		case 52: // 12A13
		case 116:
			tx_data[tx_offset] = measure(12,TLM_A,13); 
			break;
		case 53: // 12A14
		case 117:
			tx_data[tx_offset] = measure(12,TLM_A,14); 
			break;
		case 54: // 12A15
		case 118:
			tx_data[tx_offset] = measure(12,TLM_A,15); 
			break;
		case 55: // 12A16
		case 119:
			tx_data[tx_offset] = measure(12,TLM_A,16); 
			break;
		case 56:
			switch(frame_count){
				case 0: // 11A15
					tx_data[tx_offset] = measure(11,TLM_A,15); 
					break;
				case 1: // 11A51
					tx_data[tx_offset] = measure(11,TLM_A,51); 
					break;
				case 2: // 11A87
					tx_data[tx_offset] = measure(11,TLM_A,87); 
					break;
				case 3: // 11A123
					tx_data[tx_offset] = measure(11,TLM_A,123); 
					break;
				case 4: // 11A159
					tx_data[tx_offset] = measure(11,TLM_A,159); 
					break;
			}
			break;
		case 57:
			switch(frame_count){
				case 0: // 11A16
					tx_data[tx_offset] = measure(11,TLM_A,16); 
					break;
				case 1: // 11A52
					tx_data[tx_offset] = measure(11,TLM_A,52); 
					break;
				case 2: // 11A88
					tx_data[tx_offset] = measure(11,TLM_A,88); 
					break;
				case 3: // 11A124
					tx_data[tx_offset] = measure(11,TLM_A,124); 
					break;
				case 4: // 11A160
					tx_data[tx_offset] = measure(11,TLM_A,160); 
					break;
			}
			break;
		case 58:
			switch(frame_count){
				case 0: // 11A17
					tx_data[tx_offset] = measure(11,TLM_A,17); 
					break;
				case 1: // 11A53
					tx_data[tx_offset] = measure(11,TLM_A,53); 
					break;
				case 2: // 11A89
					tx_data[tx_offset] = measure(11,TLM_A,89); 
					break;
				case 3: // 11A125
					tx_data[tx_offset] = measure(11,TLM_A,125); 
					break;
				case 4: // 11A161
					tx_data[tx_offset] = measure(11,TLM_A,161); 
					break;
			}
			break;
		case 59:
			switch(frame_count){
				case 0: // 11A18
					tx_data[tx_offset] = measure(11,TLM_A,18); 
					break;
				case 1: // 11A54
					tx_data[tx_offset] = measure(11,TLM_A,54); 
					break;
				case 2: // 11A90
					tx_data[tx_offset] = measure(11,TLM_A,90); 
					break;
				case 3: // 11A126
					tx_data[tx_offset] = measure(11,TLM_A,126); 
					break;
				case 4: // 11A162
					tx_data[tx_offset] = measure(11,TLM_A,162); 
					break;
			}
			break;
		case 60: // 51A4
			tx_data[tx_offset] = measure(51,TLM_A,4); 
			break;
		case 61: // 51A5
			tx_data[tx_offset] = measure(51,TLM_A,5); 
			break;
		case 62: // 51A6
			tx_data[tx_offset] = measure(51,TLM_A,6); 
			break;
		case 63: // 51A7
			tx_data[tx_offset] = measure(51,TLM_A,7); 
			break;
		case 64:
			switch(frame_count){
				case 0: // 11DP2A
					tx_data[tx_offset] = measure(11,TLM_DP,2); 
					break;
				case 1: // 11DP6
					tx_data[tx_offset] = measure(11,TLM_DP,6); 
					break;
				case 2: // 11DP13
					tx_data[tx_offset] = measure(11,TLM_DP,13); 
					break;
				case 3: // 11DP20
					tx_data[tx_offset] = measure(11,TLM_DP,20); 
					break;
				case 4: // 11DP27
					tx_data[tx_offset] = measure(11,TLM_DP,27); 
					break;
			}
			break;
		case 65:
			switch(frame_count){
				case 0: // 11DP2B
					tx_data[tx_offset] = measure(11,TLM_DP,2); 
					break;
				case 1: // 11DP7
					tx_data[tx_offset] = measure(11,TLM_DP,7); 
					break;
				case 2: // 11DP14
					tx_data[tx_offset] = measure(11,TLM_DP,14); 
					break;
				case 3: // 11DP21
					tx_data[tx_offset] = measure(11,TLM_DP,21); 
					break;
				case 4: // 11DP28
					tx_data[tx_offset] = measure(11,TLM_DP,28); 
					break;
			}
			break;
		case 66:
			switch(frame_count){
				case 0: // 11DP2C
					tx_data[tx_offset] = measure(11,TLM_DP,2); 
					break;
				case 1: // 11DP8
					tx_data[tx_offset] = measure(11,TLM_DP,8); 
					break;
				case 2: // 11DP15
					tx_data[tx_offset] = measure(11,TLM_DP,15); 
					break;
				case 3: // 11DP22
					tx_data[tx_offset] = measure(11,TLM_DP,22); 
					break;
				case 4: // 11DP29
					tx_data[tx_offset] = measure(11,TLM_DP,29); 
					break;
			}
			break;
		case 67:
			switch(frame_count){
				case 0: // 11DP2D
					tx_data[tx_offset] = measure(11,TLM_DP,2); 
					break;
				case 1: // 11DP9
					tx_data[tx_offset] = measure(11,TLM_DP,9); 
					break;
				case 2: // 11DP16
					tx_data[tx_offset] = measure(11,TLM_DP,16); 
					break;
				case 3: // 11DP23
					tx_data[tx_offset] = measure(11,TLM_DP,23); 
					break;
				case 4: // 11DP30
					tx_data[tx_offset] = measure(11,TLM_DP,30); 
					break;
			}
			break;
		case 72: 
			switch(frame_count){
				case 0: // 11A19
					tx_data[tx_offset] = measure(11,TLM_A,19); 
					break;
				case 1: // 11A55
					tx_data[tx_offset] = measure(11,TLM_A,55);  
					break;
				case 2: // 11A91
					tx_data[tx_offset] = measure(11,TLM_A,91); 
					break;
				case 3: // 11A127
					tx_data[tx_offset] = measure(11,TLM_A,127); 
					break;
				case 4: // 11A163
					tx_data[tx_offset] = measure(11,TLM_A,163); 
					break;
			}
			break;
		case 73:
			switch(frame_count){
				case 0: // 11A20
					tx_data[tx_offset] = measure(11,TLM_A,20); 
					break;
				case 1: // 11A56 AC BUS 2 PH A VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,56); 
					break;
				case 2: // 11A92
					tx_data[tx_offset] = measure(11,TLM_A,92); 
					break;
				case 3: // 11A128
					tx_data[tx_offset] = measure(11,TLM_A,128); 
					break;
				case 4: // 11A164
					tx_data[tx_offset] = measure(11,TLM_A,164); 
					break;
			}
			break;
		case 74:
			switch(frame_count){
				case 0: // 11A21
					tx_data[tx_offset] = measure(11,TLM_A,21); 
					break;
				case 1: // 11A57 MNA VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,57); 
					break;
				case 2: // 11A93
					tx_data[tx_offset] = measure(11,TLM_A,93); 
					break;
				case 3: // 11A129
					tx_data[tx_offset] = measure(11,TLM_A,129); 
					break;
				case 4: // 11A165
					tx_data[tx_offset] = measure(11,TLM_A,165); 
					break;
			}
			break;
		case 75:
			switch(frame_count){
				case 0: // 11A22
					tx_data[tx_offset] = measure(11,TLM_A,22); 
					break;
				case 1: // 11A58 MNB VOLTS
					tx_data[tx_offset] = measure(11,TLM_A,58); 
					break;
				case 2: // 11A94
					tx_data[tx_offset] = measure(11,TLM_A,94); 
					break;
				case 3: // 11A130
					tx_data[tx_offset] = measure(11,TLM_A,130); 
					break;
				case 4: // 11A166
					tx_data[tx_offset] = measure(11,TLM_A,166); 
					break;
			}
			break;
		case 80:
			switch(frame_count){
				case 0: // 11A23
					tx_data[tx_offset] = measure(11,TLM_A,23); 
					break;
				case 1: // 11A59
					tx_data[tx_offset] = measure(11,TLM_A,59); 
					break;
				case 2: // 11A95
					tx_data[tx_offset] = measure(11,TLM_A,95); 
					break;
				case 3: // 11A131
					tx_data[tx_offset] = measure(11,TLM_A,131); 
					break;
				case 4: // 11A167
					tx_data[tx_offset] = measure(11,TLM_A,167); 
					break;
			}
			break;
		case 81: // 22DP1
			tx_data[tx_offset] = measure(22,TLM_DP,1); 
			break;
		case 82: // 22DP2
			tx_data[tx_offset] = measure(22,TLM_DP,2); 
			break;
		case 83: // MAGICAL WORD 2
			// 10A2
			// 10A5
			// 10A8
			// ...
			// 10A149
			tx_data[tx_offset] = measure(10,TLM_A,2+(frame_addr*3));
			break;
		case 88:
			switch(frame_count){
				case 0: // 11A24
					tx_data[tx_offset] = measure(11,TLM_A,24); 
					break;
				case 1: // 11A60
					tx_data[tx_offset] = measure(11,TLM_A,60); 
					break;
				case 2: // 11A96
					tx_data[tx_offset] = measure(11,TLM_A,96); 
					break;
				case 3: // 11A132
					tx_data[tx_offset] = measure(11,TLM_A,132); 
					break;
				case 4: // 11A168
					tx_data[tx_offset] = measure(11,TLM_A,168); 
					break;
			}
			break;
		case 89:
			switch(frame_count){
				case 0: // 11A25
					tx_data[tx_offset] = measure(11,TLM_A,25); 
					break;
				case 1: // 11A61
					tx_data[tx_offset] = measure(11,TLM_A,61); 
					break;
				case 2: // 11A97
					tx_data[tx_offset] = measure(11,TLM_A,97); 
					break;
				case 3: // 11A133
					tx_data[tx_offset] = measure(11,TLM_A,133); 
					break;
				case 4: // 11A169
					tx_data[tx_offset] = measure(11,TLM_A,169); 
					break;
			}
			break;
		case 90:
			switch(frame_count){
				case 0: // 11A26
					tx_data[tx_offset] = measure(11,TLM_A,26); 
					break;
				case 1: // 11A62
					tx_data[tx_offset] = measure(11,TLM_A,62); 
					break;
				case 2: // 11A98
					tx_data[tx_offset] = measure(11,TLM_A,98); 
					break;
				case 3: // 11A134
					tx_data[tx_offset] = measure(11,TLM_A,134); 
					break;
				case 4: // 11A170
					tx_data[tx_offset] = measure(11,TLM_A,170); 
					break;
			}
			break;
		case 91:
			switch(frame_count){
				case 0: // 11A27
					tx_data[tx_offset] = measure(11,TLM_A,27); 
					break;
				case 1: // 11A63
					tx_data[tx_offset] = measure(11,TLM_A,63); 
					break;
				case 2: // 11A99
					tx_data[tx_offset] = measure(11,TLM_A,99); 
					break;
				case 3: // 11A135
					tx_data[tx_offset] = measure(11,TLM_A,135); 
					break;
				case 4: // 11A171
					tx_data[tx_offset] = measure(11,TLM_A,171); 
					break;
			}
			break;
		case 92: // 51A8
			tx_data[tx_offset] = measure(51,TLM_A,8); 
			break;
		case 93: // 51A9
			tx_data[tx_offset] = measure(51,TLM_A,9); 
			break;
		case 94: // 51A10
			tx_data[tx_offset] = measure(51,TLM_A,10); 
			break;
		case 95: // 51A11
			tx_data[tx_offset] = measure(51,TLM_A,11); 
			break;
		case 96:
			switch(frame_count){
				case 0: // 11DP3
					tx_data[tx_offset] = measure(11,TLM_DP,3); 
					break;
				case 1: // 11DP10
					tx_data[tx_offset] = measure(11,TLM_DP,10); 
					break;
				case 2: // 11DP17
					tx_data[tx_offset] = measure(11,TLM_DP,17); 
					break;
				case 3: // 11DP24
					tx_data[tx_offset] = measure(11,TLM_DP,24); 
					break;
				case 4: // 11DP31
					tx_data[tx_offset] = measure(11,TLM_DP,31); 
					break;
			}
			break;
		case 97:
			switch(frame_count){
				case 0: // 11DP4
					tx_data[tx_offset] = measure(11,TLM_DP,4); 
					break;
				case 1: // 11DP11
					tx_data[tx_offset] = measure(11,TLM_DP,11); 
					break;
				case 2: // 11DP18
					tx_data[tx_offset] = measure(11,TLM_DP,18); 
					break;
				case 3: // 11DP25
					tx_data[tx_offset] = measure(11,TLM_DP,25); 
					break;
				case 4: // 11DP32
					tx_data[tx_offset] = measure(11,TLM_DP,32); 
					break;
			}
			break;
		case 98:
			switch(frame_count){
				case 0: // 11DP5
					tx_data[tx_offset] = measure(11,TLM_DP,5); 
					break;
				case 1: // 11DP12
					tx_data[tx_offset] = measure(11,TLM_DP,12); 
					break;
				case 2: // 11DP19
					tx_data[tx_offset] = measure(11,TLM_DP,19); 
					break;
				case 3: // 11DP26
					tx_data[tx_offset] = measure(11,TLM_DP,26); 
					break;
				case 4: // 11DP33
					tx_data[tx_offset] = measure(11,TLM_DP,33); 
					break;
			}
			break;
		case 99: // 51DP2
			tx_data[tx_offset] = measure(51,TLM_DP,2); 
			break;
		case 104:
			switch(frame_count){
				case 0: // 11A28
					tx_data[tx_offset] = measure(11,TLM_A,28); 
					break;
				case 1: // 11A64
					tx_data[tx_offset] = measure(11,TLM_A,64); 
					break;
				case 2: // 11A100
					tx_data[tx_offset] = measure(11,TLM_A,100); 
					break;
				case 3: // 11A136
					tx_data[tx_offset] = measure(11,TLM_A,136); 
					break;
				case 4: // 11A172
					tx_data[tx_offset] = measure(11,TLM_A,172); 
					break;
			}
			break;
		case 105:
			switch(frame_count){
				case 0: // 11A29 FC1 N2 PRESS
					tx_data[tx_offset] = measure(11,TLM_A,29); 
					break;
				case 1: // 11A65
					tx_data[tx_offset] = measure(11,TLM_A,65); 
					break;
				case 2: // 11A101
					tx_data[tx_offset] = measure(11,TLM_A,101); 
					break;
				case 3: // 11A137
					tx_data[tx_offset] = measure(11,TLM_A,137); 
					break;
				case 4: // 11A173
					tx_data[tx_offset] = measure(11,TLM_A,173); 
					break;
			}
			break;
		case 106:
			switch(frame_count){
				case 0: // 11A30 FC2 N2 PRESS
					tx_data[tx_offset] = measure(11,TLM_A,30); 
					break;
				case 1: // 11A66
					tx_data[tx_offset] = measure(11,TLM_A,66); 
					break;
				case 2: // 11A102
					tx_data[tx_offset] = measure(11,TLM_A,102); 
					break;
				case 3: // 11A138
					tx_data[tx_offset] = measure(11,TLM_A,138); 
					break;
				case 4: // 11A174
					tx_data[tx_offset] = measure(11,TLM_A,174); 
					break;
			}
			break;
		case 107:
			switch(frame_count){
				case 0: // 11A31
					tx_data[tx_offset] = measure(11,TLM_A,31); 
					break;
				case 1: // 11A67 FC1 O2 PRESS
					tx_data[tx_offset] = measure(11,TLM_A,67); 
					break;
				case 2: // 11A103
					tx_data[tx_offset] = measure(11,TLM_A,103); 
					break;
				case 3: // 11A139
					tx_data[tx_offset] = measure(11,TLM_A,139); 
					break;
				case 4: // 11A175
					tx_data[tx_offset] = measure(11,TLM_A,175); 
					break;
			}
			break;
		case 112:
			switch(frame_count){
				case 0: // 11A32 
					tx_data[tx_offset] = measure(11,TLM_A,32); 
					break;
				case 1: // 11A68 FC2 O2 PRESS
					tx_data[tx_offset] = measure(11,TLM_A,68); 
					break;
				case 2: // 11A104
					tx_data[tx_offset] = measure(11,TLM_A,104); 
					break;
				case 3: // 11A140
					tx_data[tx_offset] = measure(11,TLM_A,140); 
					break;
				case 4: // 11A176
					tx_data[tx_offset] = measure(11,TLM_A,176); 
					break;
			}
			break;
		case 113: // 22DP1
			tx_data[tx_offset] = measure(22,TLM_DP,1); 
			break;
		case 114: // 22DP2
			tx_data[tx_offset] = measure(22,TLM_DP,2); 
			break;
		case 115: // MAGICAL WORD 3
			// 10A3
			// 10A6
			// 10A9
			// ...
			// 10A150
			tx_data[tx_offset] = measure(10,TLM_A,3+(frame_addr*3));
			break;
		case 120:
			switch(frame_count){
				case 0: // 11A33 
					tx_data[tx_offset] = measure(11,TLM_A,33); 
					break;
				case 1: // 11A69
					tx_data[tx_offset] = measure(11,TLM_A,69); 
					break;
				case 2: // 11A105
					tx_data[tx_offset] = measure(11,TLM_A,105); 
					break;
				case 3: // 11A141
					tx_data[tx_offset] = measure(11,TLM_A,141); 
					break;
				case 4: // 11A177
					tx_data[tx_offset] = measure(11,TLM_A,177); 
					break;
			}
			break;
		case 121:
			switch(frame_count){
				case 0: // 11A34
					tx_data[tx_offset] = measure(11,TLM_A,34); 
					break;
				case 1: // 11A70
					tx_data[tx_offset] = measure(11,TLM_A,70); 
					break;
				case 2: // 11A106
					tx_data[tx_offset] = measure(11,TLM_A,106); 
					break;
				case 3: // 11A142
					tx_data[tx_offset] = measure(11,TLM_A,142); 
					break;
				case 4: // 11A178
					tx_data[tx_offset] = measure(11,TLM_A,178); 
					break;
			}
			break;
		case 122:
			switch(frame_count){
				case 0: // 11A35 FC3 N2 PRESS
					tx_data[tx_offset] = measure(11,TLM_A,35); 
					break;
				case 1: // 11A71
					tx_data[tx_offset] = measure(11,TLM_A,71); 
					break;
				case 2: // 11A107
					tx_data[tx_offset] = measure(11,TLM_A,107); 
					break;
				case 3: // 11A143
					tx_data[tx_offset] = measure(11,TLM_A,143); 
					break;
				case 4: // 11A179
					tx_data[tx_offset] = measure(11,TLM_A,179); 
					break;
			}
			break;
		case 123:
			switch(frame_count){
				case 0: // 11A36
					tx_data[tx_offset] = measure(11,TLM_A,36); 
					break;
				case 1: // 11A72
					tx_data[tx_offset] = measure(11,TLM_A,72); 
					break;
				case 2: // 11A108
					tx_data[tx_offset] = measure(11,TLM_A,108); 
					break;
				case 3: // 11A143
					tx_data[tx_offset] = measure(11,TLM_A,143); 
					break;
				case 4: // 11A180
					tx_data[tx_offset] = measure(11,TLM_A,180); 
					break;
			}
			break;
		case 124: // 51A12
			tx_data[tx_offset] = measure(51,TLM_A,12); 
			break;
		case 125: // 51A13
			tx_data[tx_offset] = measure(51,TLM_A,13); 
			break;
		case 126: // 51A14
			tx_data[tx_offset] = measure(51,TLM_A,14); 
			break;
		case 127: // 51A15
			tx_data[tx_offset] = measure(51,TLM_A,15); 
			break;

		// JUST IN CASE
		default:
			tx_data[tx_offset] = 0;
			break;
	}
	word_addr++;
	if(word_addr > 127){
		word_addr = 0;
		frame_addr++;
		if(frame_addr > 49){
			frame_addr = 0;
		}
		frame_count++;
		if(frame_count > 4){
			frame_count = 0;
		}
	}
}

void PCM::perform_io(double simt){
	// Do TCP IO	
	switch(conn_state){
		case 0: // UNINITIALIZED
			break;
		case 1: // INITALIZED, LISTENING
			// Do we have data from MCC?
			if (mcc_size > 0) {
				// sprintf(oapiDebugString(), "MCCSIZE %d LRX %f LRXINT %f", mcc_size, last_rx, ((simt - last_rx) / 0.005));
				// Should we recieve?
				if ((fabs(simt - last_rx) / 0.1) < 1 || sat->agc.IsUpruptActive()) {
					return; // No
				}
				last_rx = simt;
				// Yes. Take a byte
				rx_data[rx_offset] = mcc_data[mcc_offset];
				mcc_offset++;
				// If uplink isn't blocked
				if (sat->UPTLMSwitch1.GetState() != TOGGLESWITCH_DOWN) {
					// Handle it
					handle_uplink();
				}
				// Are we done?
				if (mcc_offset >= mcc_size) {
					// We reached the end of the MCC buffer.
					mcc_offset = mcc_size = 0;
				}
			}else{
				// Try to accept
				AcceptSocket = accept( m_socket, NULL, NULL );
				if(AcceptSocket != INVALID_SOCKET){
					conn_state = 2; // Accept this!
					wsk_error = 0; // For now
				}
			}
			// Otherwise loop and try again.
			break;
		case 2: // CONNECTED			
			int bytesSent,bytesRecv;

			bytesSent = send(AcceptSocket, (char *)tx_data, tx_size, 0 );
			if(bytesSent == SOCKET_ERROR){
				long errnumber = WSAGetLastError();
				switch(errnumber){
					// KNOWN CODES that we can ignore
					case 10035: // Operation Would Block
						// We can ignore this entirely. It's not an error.
						break;

					case 10038: // Socket isn't a socket
					case 10053: // Software caused connection abort
					case 10054: // Connection reset by peer
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;

					default:           // If unknown
						wsk_error = 1; // do this
						sprintf(wsk_emsg,"TELECOM: send() failed: %ld",errnumber);
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;					
				}
			}
			// Should we recieve?
			if ((fabs(simt - last_rx) / 0.005) < 1 || sat->agc.IsUpruptActive()) {			
				return; // No
			}
			last_rx = simt;
			bytesRecv = recv( AcceptSocket, (char *)(rx_data+rx_offset), 1, 0 );
			if(bytesRecv == SOCKET_ERROR){
				long errnumber = WSAGetLastError();
				switch(errnumber){
					// KNOWN CODES that we can ignore
					case 10035: // Operation Would Block
						// We can ignore this entirely. It's not an error.
						break;

					case 10053: // Software caused connection abort
					case 10038: // Socket isn't a socket
					case 10054: // Connection reset by peer
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;

					default:           // If unknown
						wsk_error = 1; // do this
						sprintf(wsk_emsg,"TELECOM: recv() failed: %ld",errnumber);
						closesocket(AcceptSocket);
						conn_state = 1; // Accept another
						uplink_state = 0; rx_offset = 0;
						break;					
				}
				// Do we have data from MCC instead?
				if (mcc_size > 0) {
					// Yes. Take a byte
					rx_data[rx_offset] = mcc_data[mcc_offset];
					mcc_offset++;
					// If the telemetry data-path is disconnected, discard the data
					if (sat->UPTLMSwitch1.GetState() != TOGGLESWITCH_DOWN) {
						// otherwise handle it
						handle_uplink();
					}
					// Are we done?
					if (mcc_offset >= mcc_size) {
						// We reached the end of the MCC buffer.
						mcc_offset = mcc_size = 0;
					}
				}
			}else{
				// If the telemetry data-path is disconnected
				if(sat->UPTLMSwitch1.GetState() == TOGGLESWITCH_DOWN){
					return; // Discard the data
				}
				if(bytesRecv > 0){
					handle_uplink();
				} else {
					// Do we have data from MCC instead?
					if (mcc_size > 0) {
						// Yes. Take a byte
						rx_data[rx_offset] = mcc_data[mcc_offset];
						mcc_offset++;
						// Handle it
						handle_uplink();
						// Are we done?
						if (mcc_offset >= mcc_size) {
							// We reached the end of the MCC buffer.
							mcc_offset = mcc_size = 0;
						}
					}
				}
			}
			break;			
	}
}

// Handle data moved to buffer from either the socket or mcc buffer
void PCM::handle_uplink() {
	switch (uplink_state) {
	case 0: // NEW COMMAND START
		int va, sa;
		va = ((rx_data[rx_offset] & 070) >> 3);
		sa = rx_data[rx_offset] & 07;
		// *** VEHICLE ADDRESS HARDCODED HERE *** (NASA DID THIS TOO)
		if (va != 04) { break; }
		switch (sa) {
		case 0: // TEST
			rx_offset++; uplink_state = 10;
			break;
		case 3: // CMC-UPDATA
			rx_offset++; uplink_state = 20;
			break;
		case 4: // CTE-UPDATE
			rx_offset++; uplink_state = 30;
			break;
		case 5: // RTC-SALVO
			rx_offset++; uplink_state = 40;
			break;
		case 6: // RTC-COMMAND
			rx_offset++; uplink_state = 50;
			break;
		default:
			sprintf(sat->debugString(), "UPLINK: UNKNOWN SYSTEM-ADDRESS %o", sa);
			break;
		}
		break;

	case 10: // TEST CMD
		rx_offset = 0; uplink_state = 0; break;

	case 20: // CMC UPLINK CMD
			 // Expect another byte
		rx_offset++; uplink_state++; break;
	case 21: // CMC UPLINK
	{
		int cmc_uplink_wd = rx_data[rx_offset - 1];
		cmc_uplink_wd <<= 8;
		cmc_uplink_wd |= rx_data[rx_offset];
		// Both uplink switches must be in ACCEPT
		if (sat->CMUplinkSwitch.GetState() == TOGGLESWITCH_DOWN ||
			sat->UPTLMSwitch.GetState() == TOGGLESWITCH_DOWN) {
			rx_offset = 0; uplink_state = 0; break;
		}
		// Move to INLINK
		sat->agc.vagc.Erasable[0][045] = cmc_uplink_wd;
		// Cause UPRUPT
		sat->agc.GenerateUprupt();

		//sprintf(oapiDebugString(),"CMC UPLINK DATA %05o",cmc_uplink_wd);
		rx_offset = 0; uplink_state = 0;
	}
	break;

	case 30: // CTE UPDATE CMD
		rx_offset = 0; uplink_state = 0; break;

	case 40: // RTC SALVO CMD
		switch (rx_data[rx_offset]) {
		case 030: // LATCH RELAYS FOR RESET 1
			rx_offset = 0; uplink_state = 0; break;
		case 032: // RESET SALVO 1
				  // SENDS RTC-COMMANDS
				  // 00 04 20 10 14 02,16 06 12 22,26 32,36
			rx_offset = 0; uplink_state = 0; break;

		case 070: // LATCH RELAYS FOR RESET 2
			rx_offset = 0; uplink_state = 0; break;
		case 072: // RESET SALVO 2
				  // SENDS SENDS RTC-COMMANDS
				  // 40,50 44,54,60 64,70 74 42,46 52,56 62,66 72,76
			rx_offset = 0; uplink_state = 0; break;

		default:
			sprintf(sat->debugString(), "UNKNOWN RTC SALVO CMD %o", rx_data[rx_offset]);
			rx_offset = 0; uplink_state = 0;
			break;
		}
		break;

	case 50: // RTC CMD
		switch (rx_data[rx_offset]) {
		case 00: // ABORT LT A OFF
		{
			sat->cws.UplinkTestState &= 012;
			rx_offset = 0; uplink_state = 0;
		}
		break;
		case 01: // ABORT LT A ON
		{
			sat->cws.UplinkTestState |= 001;
			rx_offset = 0; uplink_state = 0;
		}
		break;
		case 04: // CREW ALERT OFF
		{
			sat->cws.UplinkTestState &= 003;

			rx_offset = 0; uplink_state = 0;
		}
		break;
		case 05: // CREW ALERT ON
		{
			sat->cws.UplinkTestState |= 010;
			rx_offset = 0; uplink_state = 0;
		}
		break;
		case 06: // ABORT LT B OFF
		{
			sat->cws.UplinkTestState &= 011;
			rx_offset = 0; uplink_state = 0;
		}
		break;
		case 07: // ABORT LT B ON
		{
			sat->cws.UplinkTestState |= 002;
			rx_offset = 0; uplink_state = 0;
		}
		break;

		case 010: // ACE CMC ZERO DISABLE
		case 011: // ACE CMC ZERO ENABLE
		case 014: // ACE CMC ONE DISABLE
		case 015: // ACE CMC ONE ENABLE
				  // Ignore these
			rx_offset = 0; uplink_state = 0;
			break;

		case 022: // LATCH RELAYS FOR VHF RANGING CONTROL
		case 023: // RANGING DISABLE
		case 026: // RANGING RESET
		case 027: // RANGING ENABLE
			rx_offset = 0; uplink_state = 0; break;

		case 032: // LATCH RELAYS FOR R/T PCM CONTROL
		case 033: // SAME
		case 036: // R/T PCM RESET
		case 037: // R/T PCM 1024 ON (IF 32 LATCHED) or R/T PCM OFF (IF 33 LATCHED)
			rx_offset = 0; uplink_state = 0; break;

		case 040: // LATCH RELAYS FOR UNIFIED S-BAND SYSTEM CONTROL
		case 041: // SAME
		case 042: // SAME
		case 043: // SAME
		case 044: // FM OFF, TAPE OFF
		case 045: // FM ON, TAPE ON
		case 046: // POWER AMP RESET (IF 42 LATCHED), POWER AMP OFF (IF 43 LATCHED)
		case 047: // POWER AMP HIGH (IF 42 LATCHED), POWER AMP LOW (IF 43 LATCHED)
		case 050: // USB MODE RESET 
		case 051: // BACKUP VOICE FM OFF
			rx_offset = 0; uplink_state = 0; break;

		case 052: // LATCH RELAYS FOR DSE PLAYBACK MODE CONTROL
		case 053: // SELECT LM PLAYBACK DATA
		case 056: // PLAYBACK MODE RESET
		case 057: // SELECT CSM PLAYBACK DATA
			rx_offset = 0; uplink_state = 0; break;

		case 062: // LATCH RELAYS FOR DSE CONTROL
		case 063: // SAME
		case 064: // LATCH RELAYS FOR PCM DOWNTELEMETRY RATE CONTROL
			rx_offset = 0; uplink_state = 0; break;

		case 065: // DOWNTLM MODE LBR
			pcm_rate_override = 1;
			rx_offset = 0; uplink_state = 0; break;

		case 066: // DSE RESET (IF 62 LATCHED), DSE OFF (IF 63 LATCHED)
		case 067: // DSE RECORD (IF 62 LATCHED), DSE PLAYBACK (IF 63 LATCHED)
			rx_offset = 0; uplink_state = 0; break;

		case 070: // DOWNTLM MODE RESET
			pcm_rate_override = 0;
			rx_offset = 0; uplink_state = 0; break;

		case 071: // DOWNTLM MODE HBR
			pcm_rate_override = 2;
			rx_offset = 0; uplink_state = 0; break;

		case 072: // LATCH RELAYS FOR DSE TAPE CONTROL
		case 073: // SAME
		case 074: // ANTENNA SELECT RESET
		case 075: // ANTENNA SELECT OMNI "D" 
		case 076: // DSE TAPE RESET (IF 72 LATCHED), DSE TAPE STOP (IF 73 LATCHED)
		case 077: // DSE TAPE FORWARD (IF 72 LATCHED), DSE TAPE REWIND (IF 73 LATCHED)
			rx_offset = 0; uplink_state = 0; break;

		default:
			sprintf(sat->debugString(), "UNKNOWN RTC COMMAND %o", rx_data[rx_offset]);
			rx_offset = 0; uplink_state = 0;
			break;
		}
		break;
	}
}

DSEChunk::DSEChunk() :
	chunkData( 0 ),
	chunkSize( 0 ),
	chunkValidBytes( 0 ),
	chunkType( DSEEMPTY )
{
	// Nothing to do.
}

DSEChunk::~DSEChunk()
{
	deleteData();
}

void DSEChunk::deleteData()
{
	if ( chunkData )
	{
		delete[] chunkData;
	}

	chunkData = 0;
	chunkSize = 0;
	chunkValidBytes = 0;
	chunkType = DSEEMPTY;
}

void DSEChunk::Erase( const DSEChunkType dataType )
{
	unsigned int requiredData = 0;

	switch ( dataType )
	{
	case DSEHBR:
		requiredData = dseChunkSizeHBR;
		break;

	case DSELBR:
		requiredData = dseChunkSizeLBR;
		break;

	default:
		deleteData();
		return;
	}

	if ( chunkSize < requiredData )
	{
		deleteData();

		chunkData = new unsigned char[requiredData];
		chunkSize = requiredData;
	}

	chunkType = dataType;
	chunkValidBytes = 0;
}

DSE::DSE() :
	tapeSpeedInchesPerSecond( 0.0 ),
	desiredTapeSpeed( 0.0 ),
	tapeMotion( 0.0 ),
	state( STOPPED )
{
	lastEventTime = 0;
}

DSE::~DSE()
{
}

void DSE::Init(Saturn *vessel)
{
	sat = vessel;
}

bool DSE::TapeMotion()
{
	switch (state)
	{
	case STOPPED:
	case STARTING_PLAY:
	case STARTING_RECORD:
		return false;

	default:
		return true;
	}
}

const double playSpeed = 120.0;
const double hbrRecord = 15.0;
const double lbrRecord = 3.75;

void DSE::Play()
{
	if ( state != PLAYING || desiredTapeSpeed < playSpeed  )
	{
		desiredTapeSpeed = playSpeed;
		state = STARTING_PLAY;
	} else
		state = PLAYING;
}

void DSE::Stop()
{
	if ( state != STOPPED || desiredTapeSpeed > 0.0  )
	{
		desiredTapeSpeed = 0.0;
		state = STOPPING;
	} else
		state = STOPPED;
}

void DSE::Record( bool hbr )
{
	double tapeSpeed = hbr ? hbrRecord : lbrRecord;
	if ( state != RECORDING || tapeSpeedInchesPerSecond != tapeSpeed )
	{
		desiredTapeSpeed = tapeSpeed;
		
		if ( desiredTapeSpeed > tapeSpeedInchesPerSecond )
		{
			state = STARTING_RECORD;
		}
		else if ( desiredTapeSpeed < tapeSpeedInchesPerSecond )
		{
			state = SLOWING_RECORD;
		}
		else
		{
			state = RECORDING;
		}
	} else
		state = RECORDING;
}

const double tapeAccel = 30.0;

void DSE::TimeStep( double simt, double simdt )
{
	/// \todo forward/backward motion
	/// \todo high/low bitrate

	switch ( state )
	{
	case STOPPED:
		if (!sat->TapeRecorderForwardSwitch.IsCenter()) {
			if (sat->TapeRecorderRecordSwitch.IsUp()) {
				Record(true);
			} else if (sat->TapeRecorderRecordSwitch.IsDown()) {
				Play();
			}
		}
		break;

	case PLAYING:
		if (sat->TapeRecorderForwardSwitch.IsCenter() || sat->TapeRecorderRecordSwitch.IsCenter()) {
			Stop();
		} else if (sat->TapeRecorderRecordSwitch.IsUp()) {
			Record(true);
		}
		break;

	case RECORDING:
		if (sat->TapeRecorderForwardSwitch.IsCenter() || sat->TapeRecorderRecordSwitch.IsCenter()) {
			Stop();
		} else if (sat->TapeRecorderRecordSwitch.IsDown()) {
			Play();
		}
		break;

	case STARTING_PLAY:
		tapeSpeedInchesPerSecond += tapeAccel * simdt;
		if ( tapeSpeedInchesPerSecond >= desiredTapeSpeed )
		{
			tapeSpeedInchesPerSecond = desiredTapeSpeed;
			state = PLAYING;
		}
		break;

	case STARTING_RECORD:
		tapeSpeedInchesPerSecond += tapeAccel * simdt;
		if ( tapeSpeedInchesPerSecond >= desiredTapeSpeed )
		{
			tapeSpeedInchesPerSecond = desiredTapeSpeed;
			state = RECORDING;
		}
		break;

	case SLOWING_RECORD:
		tapeSpeedInchesPerSecond -= tapeAccel * simdt;
		if ( tapeSpeedInchesPerSecond <= desiredTapeSpeed )
		{
			tapeSpeedInchesPerSecond = desiredTapeSpeed;
			state = RECORDING;
		}
		break;

	case STOPPING:
		tapeSpeedInchesPerSecond -= tapeAccel * simdt;
		if ( tapeSpeedInchesPerSecond <= 0.0 )
		{
			tapeSpeedInchesPerSecond = 0.0;
			state = STOPPED;
		}
		break;

	default:
		break;
	}
	lastEventTime = simt;
	//sprintf(oapiDebugString(), "DSE tapeSpeedips %lf desired %lf tapeMotion %lf state %i", tapeSpeedInchesPerSecond, desiredTapeSpeed, tapeMotion, state);
}

void DSE::LoadState(char *line) {
	
	/// \todo DSE Chunks

	sscanf(line + 12, "%lf %lf %lf %i %lf", &tapeSpeedInchesPerSecond, &desiredTapeSpeed, &tapeMotion, &state, &lastEventTime);
}

void DSE::SaveState(FILEHANDLE scn) {
	char buffer[256];

	sprintf(buffer, "%lf %lf %lf %i %lf", tapeSpeedInchesPerSecond, desiredTapeSpeed, tapeMotion, state, lastEventTime); 
	oapiWriteScenario_string(scn, "DATARECORDER", buffer);
}

// Rendezvous Radar Transponder System
// there is a connector, CSM_RRTto_LM_RRConnector, which is a member of the saturn class, that is recieving the radar RF properties from the LEM which is doing the sending.

RNDZXPDRSystem::RNDZXPDRSystem()
{
	sat = NULL;
	lem = NULL;
	TestOperateSwitch = NULL;
	HeaterPowerSwitch = NULL;
	RRT_LeftSystemTestRotarySwitch = NULL;
	RRT_RightSystemTestRotarySwitch = NULL;
	RRT_FLTBusCB = NULL;
}

RNDZXPDRSystem::~RNDZXPDRSystem()
{
	sat->CSM_RRTto_LM_RRConnector.Disconnect();
}

void RNDZXPDRSystem::Init(Saturn *vessel, CircuitBrakerSwitch *PowerCB, ToggleSwitch *RNDZXPDRSwitch, ThreePosSwitch *Panel100RNDZXPDRSwitch, RotationalSwitch *LeftSystemTestRotarySwitch, RotationalSwitch *RightSystemTestRotarySwitch)
{
	sat = vessel;
	if (!lem){
		VESSEL *lm = sat->agc.GetLM();
		if (lm) {
			lem = (static_cast<LEM*>(lm));
		}
	}

	TestOperateSwitch = RNDZXPDRSwitch;
	HeaterPowerSwitch = Panel100RNDZXPDRSwitch;
	RRT_LeftSystemTestRotarySwitch = LeftSystemTestRotarySwitch;
	RRT_RightSystemTestRotarySwitch = LeftSystemTestRotarySwitch;
	RRT_FLTBusCB = PowerCB;

	RCVDfreq = 0.0;
	RCVDpow = 0.0;
	RCVDgain = 0.0;
	RCVDPhase = 0.0;

	XPDRon = false;
	XPDRheaterOn = false;

	RadarDist = 0.0;

	RCVDPowerdB = 0.0;
	XMITpower = 0.240; //watts

	if (!(sat->CSM_RRTto_LM_RRConnector.connectedTo))
	{
		sat->CSM_RRTto_LM_RRConnector.ConnectTo(GetVesselConnector(lem, VIRTUAL_CONNECTOR_PORT, RADAR_RF_SIGNAL));
	}

	if(lem){ RNDZXPDRSystem::SendRF(); } //send inital info to the connector
}

unsigned char RNDZXPDRSystem::GetScaledRFPower()
{
	const double min_value = -122.0;
	const double max_value = -18.0;
	
	if(XPDRon && (haslock == LOCKED))
	{ 
		return static_cast<unsigned char>(((RCVDPowerdB - min_value) / (max_value - min_value) * 148) + 107); //2.1 to 5.0V, scalled to 0x00 to 0xFF range
	}
	else
	{
		return NULL;
	}	
}

unsigned char RNDZXPDRSystem::GetScaledAGCPower()
{
	const double min_value = 18.0;
	const double max_value = 122.0;

	if (XPDRon && (haslock == LOCKED))
	{
		return static_cast<unsigned char>((abs(RCVDPowerdB)-min_value)/(max_value - min_value)*229); //0.0 to 4.5V, scalled to 0x00 to 0xFF range
	}
	else
	{
		return NULL;
	}
}

unsigned char RNDZXPDRSystem::GetScaledFreqLock()
{
	if (XPDRon && (haslock == LOCKED))
	{
		return static_cast<unsigned char>((lockTimer/1.3)*229); //0.0 to 4.5V, scalled to 0x00 to 0xFF range
	}
	else if (XPDRon && (haslock == UNLOCKED))
	{
		return static_cast<unsigned char>(20.0); //Signal Search Mode.
	}
	else
	{
		return NULL;
	}
}

double RNDZXPDRSystem::GetCSMGain(double theta, double phi)
{

	//values from AOH LM volume 2

	const double gainMin = -32.0;
	const double gainMax = 6.0;

	const double ThetaXPDR = 85.0*RAD; //15 deg forward
	const double PhiXPDR = 141.8*RAD; //

	double gain;

	double AngleMap = sqrt(((theta - ThetaXPDR)*(theta - ThetaXPDR)) + ((phi - PhiXPDR)*(phi - PhiXPDR))); 
	
	gain = cos(AngleMap/2*RAD)*cos(AngleMap / 2 * RAD); //close enough

	gain = gain * (gainMax - gainMin) + gainMin;




	return gain;
}

void RNDZXPDRSystem::SendRF()
{
	if (XPDRon && (haslock == LOCKED))//act like a transponder
	{
		sat->CSM_RRTto_LM_RRConnector.SendRF(RCVDfreq*(240.0 / 241.0), XMITpower, RNDZXPDRGain, 0.0);
	}
	else //act like a radar reflector, this is also a function of orientation and skin temperature of the CSM, but this should work.
	{
		sat->CSM_RRTto_LM_RRConnector.SendRF(RCVDfreq, (pow(10.0, RCVDPowerdB / 10.0) / 1000)*0.85*((sin(theta*RAD) + 1) / 2), 12.0, 0.0); //should give a radar cross section of ~5m^2 side on, ~=5kM range
	}
}

void RNDZXPDRSystem::TimeStep(double simdt)
{
	//this block of code checks to see if the LEM has somehow been deleted mid sceneriao, and sets the lem pointer to null
	bool isLem = false;

	for (unsigned int i = 0; i < oapiGetVesselCount(); i++)
	{
		OBJHANDLE hVessel = oapiGetVesselByIndex(i);
		VESSEL* pVessel = oapiGetVesselInterface(hVessel);
		if (!_strnicmp(pVessel->GetClassName(), "ProjectApollo/LEM", 17))
		{
			isLem = true;
		}
	}

	if (!isLem)
	{
		lem = NULL;
		haslock = UNLOCKED;
		sat->CSM_RRTto_LM_RRConnector.Disconnect();
	}
	//

	//get a pointer to the lem
	if (!lem){
		VESSEL *lm = sat->agc.GetLM();
		if (lm) {
			lem = (static_cast<LEM*>(lm));
		}
	}

	///
	/// TODO: make heater heat, should be on for 15min before switching RRT on.
	///

	//make sure the power's on to the heater and the transponder
	if (RRT_FLTBusCB->Voltage() > 25.0) //spec minimum for the RRT system
	{
		if ((HeaterPowerSwitch->GetState() == THREEPOSSWITCH_CENTER))
		{
		XPDRon = false;
		XPDRheaterOn = false;
		XPDRtest = false;
		}
		else if ((HeaterPowerSwitch->GetState() == THREEPOSSWITCH_UP) && (TestOperateSwitch->GetState() == TOGGLESWITCH_DOWN)) 
		{
			XPDRon = true;
			XPDRheaterOn = true;
			XPDRtest = false;
		}
		else if ((HeaterPowerSwitch->GetState() == THREEPOSSWITCH_UP) && (TestOperateSwitch->GetState() == TOGGLESWITCH_UP))
		{
			XPDRon = false;
			XPDRheaterOn = true;
			XPDRtest = true;
		}
		else if (HeaterPowerSwitch->GetState() == THREEPOSSWITCH_DOWN)
		{
			XPDRon = false;
			XPDRheaterOn = true;
			XPDRtest = false;
		}
	}
	else
	{
		XPDRon = false;
		XPDRheaterOn = false;
		XPDRtest = false;
	}

	if (!XPDRon)
	{
		haslock = UNLOCKED;
		lockTimer = 0.0;
	}

	//sprintf(oapiDebugString(), "RRT_FLTBusCB Current = %lf A; Voltage = %lf V", RRT_FLTBusCB->Current(), RRT_FLTBusCB->Voltage());

	if (lem) //do transpondery things
	{
		if (!(sat->CSM_RRTto_LM_RRConnector.connectedTo))
		{
			sat->CSM_RRTto_LM_RRConnector.ConnectTo(GetVesselConnector(lem, VIRTUAL_CONNECTOR_PORT, RADAR_RF_SIGNAL));
		}

		//sprintf(oapiDebugString(),"Frequency Received: %lf MHz", RCVDfreq);
		//sprintf(oapiDebugString(), "LEM RR Gain Received: %lf", RCVDgain);

		sat->GetGlobalPos(csmPos);
		sat->GetRotationMatrix(CSMRot);
		lem->GetGlobalPos(lemPos);

		R = csmPos - lemPos;
		U_R = unit(R);

		U_R_RR = unit(tmul(CSMRot, -U_R)); // calculate the pointing vector from the CSM to the LM in the CSM's local frame
		U_R_RR = _V(U_R_RR.z, U_R_RR.x, -U_R_RR.y); //swap out Orbiter's axes for the Apollo CSM's

		theta = acos(U_R_RR.x); //calculate the azmuth about the csm local frame
		phi = atan2(U_R_RR.y, -U_R_RR.z); //calculate the elevation about the csm local frame

		if (phi < 0)
		{
			phi += RAD * 360;
		}
		//sprintf(oapiDebugString(), "Theta: %lf, Phi: %lf", theta*DEG, phi*DEG);

		RadarDist = length(R);
		//sprintf(oapiDebugString(), "LEM-CSM Distance: %lfm", RadarDist);

		RNDZXPDRGain = RNDZXPDRSystem::GetCSMGain(theta, phi);
		//sprintf(oapiDebugString(), "RNDZXPDRGain = %lf dBi", RNDZXPDRGain);

		RNDZXPDRGain = pow(10, (RNDZXPDRGain / 10)); //convert to ratio from dB

		if (RadarDist > 80.0*0.3048)
		{
			RCVDPowerdB = RCVDgain * RNDZXPDRGain * RCVDpow*pow((C0 / (RCVDfreq * 1000000)) / (4 * PI*RadarDist), 2); //watts
			RCVDPowerdB = 10.0 * log10(1000.0 * RCVDPowerdB); //convert to dBm
		}
		else
		{
			RCVDPowerdB = -130; //technicially dB should decrease linearly with decreasing log(distance) as we enter the Rayleigh Region, but this works maybe simulate this better later
		}

		if ((RCVDPowerdB > -122.0) && XPDRon)
		{
			if (lockTimer < 1.3)
			{
				lockTimer += simdt;
			}
			else
			{
				haslock = LOCKED;
			}
		}
		else
		{
			haslock = UNLOCKED;
			lockTimer = 0.0;
		}

		//sprintf(oapiDebugString(), "Power Receved: %lfdB ,Lock Timer: %lfsec", RCVDPowerdB, lockTimer);

		RNDZXPDRSystem::SendRF();
	}
}

void RNDZXPDRSystem::SystemTimestep(double simdt)
{
	double XPDRpowerDraw = 70.5; //watts
	double heater = 14.0; //watts

	if (RRT_FLTBusCB->Voltage() > 25.0) //spec minimum for the RRT system
	{
		if (HeaterPowerSwitch->GetState() == THREEPOSSWITCH_UP)
		{
			RRT_FLTBusCB->DrawPower(XPDRpowerDraw + heater);
		}
		else if (HeaterPowerSwitch->GetState() == THREEPOSSWITCH_DOWN)
		{
			RRT_FLTBusCB->DrawPower(heater);
		}

		//if (haslock == LOCKED)
		//{
			//send voltages to proper gauges.
		//}
	}
}

void RNDZXPDRSystem::LoadState(char *line)
{
	sscanf(line + 14, "%i %lf", &haslock, &lockTimer);
}

void RNDZXPDRSystem::SaveState(FILEHANDLE scn)
{
	char buffer[256];

	sprintf(buffer, "%i %lf", haslock, lockTimer);

	oapiWriteScenario_string(scn, "RNDZXPDRSystem", buffer);
}