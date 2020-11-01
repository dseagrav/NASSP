/****************************************************************************
  This file is part of Project Apollo - NASSP

  Mission Control Center / Ground Support Network

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
#include "soundlib.h"
#include "apolloguidance.h"
#include "csmcomputer.h"
#include "LEMcomputer.h"
#include "papi.h"
#include "saturn.h"
#include "saturnv.h"
#include "LEM.h"
#include "sivb.h"
#include "../src_rtccmfd/OrbMech.h"
#include "mcc.h"
#include "MCC_Mission_B.h"
#include "MCC_Mission_C.h"
#include "MCC_Mission_C_PRIME.h"
#include "MCC_Mission_D.h"
#include "MCC_Mission_F.h"
#include "MCC_Mission_G.h"
#include "rtcc.h"
#include "LVDC.h"
#include "iu.h"

// This is a threadenwerfer. It werfs threaden.
static DWORD WINAPI MCC_Trampoline(LPVOID ptr){	
	MCC *mcc = (MCC *)ptr;
	return(mcc->subThread());
}

// SCENARIO FILE MACROLOGY
#define SAVE_BOOL(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_INT(KEY,VALUE) oapiWriteScenario_int(scn, KEY, VALUE)
#define SAVE_DOUBLE(KEY,VALUE) papiWriteScenario_double(scn, KEY, VALUE)
#define SAVE_V3(KEY,VALUE) papiWriteScenario_vec(scn, KEY, VALUE)
#define SAVE_M3(KEY,VALUE) papiWriteScenario_mx(scn, KEY, VALUE)
#define SAVE_STRING(KEY,VALUE) oapiWriteScenario_string(scn, KEY, VALUE)
#define LOAD_BOOL(KEY,VALUE) if(strnicmp(line, KEY, strlen(KEY)) == 0){ sscanf(line + strlen(KEY), "%i", &tmp); if (tmp == 1) { VALUE = true; } else { VALUE = false; } }
#define LOAD_INT(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%i",&VALUE); }
#define LOAD_DOUBLE(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf",&VALUE); }
#define LOAD_V3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf",&VALUE.x,&VALUE.y,&VALUE.z); }
#define LOAD_M3(KEY,VALUE) if(strnicmp(line,KEY,strlen(KEY))==0){ sscanf(line+strlen(KEY),"%lf %lf %lf %lf %lf %lf %lf %lf %lf",&VALUE.m11,&VALUE.m12,&VALUE.m13,&VALUE.m21,&VALUE.m22,&VALUE.m23,&VALUE.m31,&VALUE.m32,&VALUE.m33); }
#define LOAD_STRING(KEY,VALUE,LEN) if(strnicmp(line,KEY,strlen(KEY))==0){ strncpy(VALUE, line + (strlen(KEY)+1), LEN); }

#define ORBITER_MODULE

void MCC::clbkSaveState(FILEHANDLE scn)
{
	VESSEL4::clbkSaveState(scn);

	if (CSMName[0])
		oapiWriteScenario_string(scn, "CSMNAME", CSMName);

	if (LVName[0])
		oapiWriteScenario_string(scn, "LVNAME", LVName);

	if (LEMName[0])
		oapiWriteScenario_string(scn, "LEMNAME", LEMName);

	SaveState(scn);
	rtcc->SaveState(scn);
}

void MCC::clbkLoadStateEx(FILEHANDLE scn, void *status)
{
	char *line;

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, "MISSIONTRACKING", 15)) {
			int i;
			sscanf(line + 15, "%d", &i);
			if (i)
				enableMissionTracking();
		}
		else if (!strnicmp(line, "CSMNAME", 7))
		{
			strncpy(CSMName, line + 8, 64);
		}
		else if (!strnicmp(line, "LVNAME", 6))
		{
			strncpy(LVName, line + 7, 64);
		}
		else if (!strnicmp(line, "LEMNAME", 7))
		{
			strncpy(LEMName, line + 8, 64);
		}
		else if (!strnicmp(line, MCC_START_STRING, sizeof(MCC_START_STRING))) {
			LoadState(scn);
		}
		else if (!strnicmp(line, RTCC_START_STRING, sizeof(RTCC_START_STRING))) {
			rtcc->LoadState(scn);
		}
		else ParseScenarioLineEx(line, status);
	}
}

void MCC::clbkPreStep(double simt, double simdt, double mjd)
{
	// Update Ground Data
	TimeStep(simdt);
}

void MCC::clbkPostCreation()
{
	VESSEL *v;
	OBJHANDLE hVessel;

	//CSM
	if (CSMName[0])
	{
		hVessel = oapiGetObjectByName(CSMName);
		if (hVessel != NULL)
		{
			v = oapiGetVesselInterface(hVessel);

			if (!stricmp(v->GetClassName(), "ProjectApollo\\Saturn5") ||
				!stricmp(v->GetClassName(), "ProjectApollo/Saturn5") ||
				!stricmp(v->GetClassName(), "ProjectApollo\\Saturn1b") ||
				!stricmp(v->GetClassName(), "ProjectApollo/Saturn1b")) {
				cm = (Saturn *)v;
				rtcc->calcParams.src = cm;
			}
		}
	}

	//S-IVB
	if (LVName[0])
	{
		hVessel = oapiGetObjectByName(LVName);
		if (hVessel != NULL)
		{
			v = oapiGetVesselInterface(hVessel);

			if (!stricmp(v->GetClassName(), "ProjectApollo\\sat5stg3") ||
				!stricmp(v->GetClassName(), "ProjectApollo/sat5stg3") ||
				!stricmp(v->GetClassName(), "ProjectApollo\\nsat1stg2") ||
				!stricmp(v->GetClassName(), "ProjectApollo/nsat1stg2")) {
				sivb = (SIVB *)v;
			}
		}
	}

	//LEM
	if (LEMName[0])
	{
		hVessel = oapiGetObjectByName(LEMName);
		if (hVessel != NULL)
		{
			v = oapiGetVesselInterface(hVessel);

			if (!stricmp(v->GetClassName(), "ProjectApollo\\LEM") ||
				!stricmp(v->GetClassName(), "ProjectApollo/LEM")) {
				lm = (LEM *)v;
				rtcc->calcParams.tgt = lm;
			}
		}
	}
}

DLLCLBK void InitModule(HINSTANCE hDLL)
{
}

DLLCLBK void ExitModule(HINSTANCE hDLL)
{
}

DLLCLBK VESSEL* ovcInit(OBJHANDLE hVessel, int iFlightModel)
{
	return new MCC(hVessel, iFlightModel);
}

DLLCLBK void ovcExit(VESSEL* pVessel)
{
	delete static_cast<MCC*>(pVessel);
}

// CONS
MCC::MCC(OBJHANDLE hVessel, int flightmodel)
	: VESSEL4(hVessel, flightmodel)
{
	//Vessel data
	CSMName[0] = 0;
	LEMName[0] = 0;
	LVName[0] = 0;
	
	// Reset data
	rtcc = NULL;
	cm = NULL;
	lm = NULL;
	sivb = NULL;
	Earth = NULL;
	Moon = NULL;
	CM_DeepSpace = false;
	GT_Enabled = false;
	MT_Enabled = false;
	AbortMode = 0;
	LastAOSUpdate=0;
	CM_MoonPosition[0] = 0;
	CM_MoonPosition[1] = 0;
	CM_MoonPosition[2] = 0;
	CM_Prev_MoonPosition[0] = 0;
	CM_Prev_MoonPosition[1] = 0;
	CM_Prev_MoonPosition[2] = 0;
	// Reset ground stations
	int x=0;
	while(x<MAX_GROUND_STATION){
		GroundStations[x].Active = false;
		GroundStations[x].Position[0] = 0;
		GroundStations[x].Position[1] = 0;
		GroundStations[x].DownTlmCaps = 0;
		GroundStations[x].CommCaps = 0;
		GroundStations[x].HasAcqAid = false;
		GroundStations[x].HasRadar = false;
		GroundStations[x].SBandAntenna = 0;
		GroundStations[x].StationType = 0;
		GroundStations[x].TelemetryCaps = 0;
		GroundStations[x].TrackingCaps = 0;
		GroundStations[x].UpTlmCaps = 0;
		GroundStations[x].USBCaps = 0;
		GroundStations[x].StationPurpose = 0;
		GroundStations[x].AOS = 0;
		sprintf(GroundStations[x].Name,"INVALID");
		sprintf(GroundStations[x].Code,"XXX");
		x++;
	}	
	// Reset capcom interface
	menuState = 0;
	padState = -1;
	padNumber = 0;
	padForm = NULL;
	padAutoShow = true;
	NHmenu = 0;
	NHmessages = 0;
	NHpad = 0;
	StateTime = 0;
	SubStateTime = 0;
	MoonRevTime = 0;
	PCOption_Enabled = false;
	PCOption_Text[0] = 0;
	NCOption_Enabled = false;
	NCOption_Text[0] = 0;
	scrubbed = false;
	upString[0] = 0;
	upDescr[0] = 0;
	upMessage[0] = 0;
	upType = 0;
	subThreadStatus = 0;

	// Ground Systems Init
	Init();
}

void MCC::Init(){
	
	// Make a new RTCC if we don't have one already
	if (rtcc == NULL) { rtcc = new RTCC; rtcc->Init(this); }

	// Obtain Earth and Moon pointers
	Earth = oapiGetGbodyByName("Earth");
	Moon = oapiGetGbodyByName("Moon");
	
	// GROUND TRACKING INITIALIZATION
	LastAOSUpdate=0;

	// Load ground station information.
	// Later this can be made dynamic, but this will work for now.

	// This particular ground station list is the stations that were on-net for Apollo 8.
	// The index numbers represent status as of 1968.	
	sprintf(GroundStations[1].Name,"ANTIGUA"); sprintf(GroundStations[1].Code,"ANG");
	// GroundStations[1].Position[0] = 28.40433; GroundStations[1].Position[1] = -80.60192; // This is the Cape?
	GroundStations[1].Position[0] = 17.137222; GroundStations[1].Position[1] = -61.775833;
	GroundStations[1].SBandAntenna = GSSA_9METER;
	GroundStations[1].HasAcqAid = true;
	GroundStations[1].DownTlmCaps = GSDT_USB;
	GroundStations[1].UpTlmCaps = GSCC_USB;
	GroundStations[1].StationPurpose = GSPT_LV_CUTOFF|GSPT_NEAR_SPACE;
	GroundStations[1].CommCaps = 0;
	GroundStations[1].USBCaps = 0;
	GroundStations[1].TrackingCaps = 0;
	GroundStations[1].Active = true;

	sprintf(GroundStations[2].Name,"ASCENSION"); sprintf(GroundStations[2].Code,"ASC");
	GroundStations[2].Position[0] = -7.94354; GroundStations[2].Position[1] = -14.37105;
	GroundStations[2].SBandAntenna = GSSA_9METER;
	GroundStations[2].HasAcqAid = true;
	GroundStations[2].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[2].UpTlmCaps = GSCC_USB;
	GroundStations[2].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[2].CommCaps = 0;
	GroundStations[2].USBCaps = 0;
	GroundStations[2].TrackingCaps = GSTK_CBAND_LOWSPEED;
	GroundStations[2].Active = true;

	sprintf(GroundStations[3].Name,"BERMUDA"); sprintf(GroundStations[3].Code,"BDA");
	GroundStations[3].Position[0] = 32.36864; GroundStations[3].Position[1] = -64.68563;
	GroundStations[3].SBandAntenna = GSSA_9METER;
	GroundStations[3].HasRadar = true;
	GroundStations[3].HasAcqAid = true;
	GroundStations[3].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[3].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[3].StationPurpose = GSPT_LV_CUTOFF|GSPT_NEAR_SPACE;
	GroundStations[3].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[3].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[3].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[3].Active = true;

	sprintf(GroundStations[4].Name,"GRAND CANARY"); sprintf(GroundStations[4].Code,"CYI");
	GroundStations[4].Position[0] = 27.74055; GroundStations[4].Position[1] = -15.60077;
	GroundStations[4].SBandAntenna = GSSA_9METER;
	GroundStations[4].HasRadar = true;
	GroundStations[4].HasAcqAid = true;
	GroundStations[4].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[4].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[4].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[4].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_USB;
	GroundStations[4].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[4].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[4].Active = true;

	sprintf(GroundStations[5].Name,"HONEYSUCKLE"); sprintf(GroundStations[5].Code,"HSK");
	GroundStations[5].Position[0] = -35.40282; GroundStations[5].Position[1] = 148.98144;
	GroundStations[5].SBandAntenna = GSSA_26METER;
	GroundStations[5].DownTlmCaps = GSDT_USB;
	GroundStations[5].UpTlmCaps = GSCC_USB;
	GroundStations[5].StationPurpose = GSPT_LUNAR;
	GroundStations[5].TrackingCaps = GSTK_USB;
	GroundStations[5].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[5].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VIDEO;
	GroundStations[5].Active = true;

	sprintf(GroundStations[6].Name,"CARNARVON"); sprintf(GroundStations[6].Code,"CRO");
	GroundStations[6].Position[0] = -24.90619; GroundStations[6].Position[1] = 113.72595;
	GroundStations[6].SBandAntenna = GSSA_9METER;
	GroundStations[6].HasRadar = true;
	GroundStations[6].HasAcqAid = true;
	GroundStations[6].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[6].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[6].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[6].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[6].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[6].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[6].Active = true;

	sprintf(GroundStations[7].Name,"CORPUS CHRISTI"); sprintf(GroundStations[7].Code,"TEX");
	GroundStations[7].Position[0] = 27.65273; GroundStations[7].Position[1] = -97.37588;
	GroundStations[7].SBandAntenna = GSSA_9METER;
	GroundStations[7].HasAcqAid = true;
	GroundStations[7].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[7].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[7].StationPurpose = GSPT_ORBITAL;
	GroundStations[7].TrackingCaps = GSTK_USB;
	GroundStations[7].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[7].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[7].Active = true;

	sprintf(GroundStations[8].Name,"GOLDSTONE"); sprintf(GroundStations[8].Code,"GDS");
	GroundStations[8].Position[0] = 35.33820; GroundStations[8].Position[1] = -116.87421;
	GroundStations[8].SBandAntenna = GSSA_26METER;
	GroundStations[8].DownTlmCaps = GSDT_USB;
	GroundStations[8].UpTlmCaps = GSCC_USB;
	GroundStations[8].StationPurpose = GSPT_LUNAR;
	GroundStations[8].TrackingCaps = GSTK_USB;
	GroundStations[8].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[8].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VIDEO;
	GroundStations[8].Active = true;

	sprintf(GroundStations[9].Name,"GRAND BAHAMA"); sprintf(GroundStations[9].Code,"GBM");
	GroundStations[9].Position[0] = 26.62022; GroundStations[9].Position[1] = -78.35825;
	GroundStations[9].SBandAntenna = GSSA_9METER;
	GroundStations[9].HasAcqAid = true;
	GroundStations[9].DownTlmCaps = GSDT_USB;
	GroundStations[9].UpTlmCaps = GSCC_USB;
	GroundStations[9].StationPurpose = GSPT_LAUNCH|GSPT_ORBITAL;
	GroundStations[9].TrackingCaps = GSTK_USB;
	GroundStations[9].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[9].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[9].Active = true;

	sprintf(GroundStations[10].Name,"GUAM"); sprintf(GroundStations[10].Code,"GWM");
	GroundStations[10].Position[0] = 13.30929; GroundStations[10].Position[1] = 144.73694;
	GroundStations[10].SBandAntenna = GSSA_9METER;
	GroundStations[10].HasAcqAid = true;
	GroundStations[10].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[10].UpTlmCaps = GSCC_USB;
	GroundStations[10].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[10].TrackingCaps = GSTK_USB;
	GroundStations[10].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[10].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[10].Active = true;

	sprintf(GroundStations[11].Name,"GUAYMAS"); sprintf(GroundStations[11].Code,"GYM");
	GroundStations[11].Position[0] = 27.95029; GroundStations[11].Position[1] = -110.90846;
	GroundStations[11].SBandAntenna = GSSA_9METER;
	GroundStations[11].HasAcqAid = true;
	GroundStations[11].DownTlmCaps = GSDT_USB;
	GroundStations[11].UpTlmCaps = GSCC_USB;
	GroundStations[11].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[11].TrackingCaps = GSTK_USB;
	GroundStations[11].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[11].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[11].Active = true;

	sprintf(GroundStations[12].Name,"HAWAII"); sprintf(GroundStations[12].Code,"HAW");
	GroundStations[12].Position[0] = 21.44719; GroundStations[12].Position[1] = -157.76307;
	GroundStations[12].SBandAntenna = GSSA_9METER;
	GroundStations[12].HasAcqAid = true;
	GroundStations[12].HasRadar = true;
	GroundStations[12].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[12].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[12].StationPurpose = GSPT_NEAR_SPACE;
	GroundStations[12].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_USB;
	GroundStations[12].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[12].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[12].Active = true;

	sprintf(GroundStations[13].Name,"MADRID"); sprintf(GroundStations[13].Code,"MAD");
	GroundStations[13].Position[0] = 40.45443; GroundStations[13].Position[1] = -4.16990;
	GroundStations[13].SBandAntenna = GSSA_26METER;
	GroundStations[13].DownTlmCaps = GSDT_USB;
	GroundStations[13].UpTlmCaps = GSCC_USB;
	GroundStations[13].StationPurpose = GSPT_LUNAR;
	GroundStations[13].TrackingCaps = GSTK_USB;
	GroundStations[13].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[13].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VIDEO;
	GroundStations[13].Active = true;

	sprintf(GroundStations[14].Name,"MERRIT"); sprintf(GroundStations[14].Code,"MIL");
	GroundStations[14].Position[0] = 28.40433; GroundStations[14].Position[1] = -80.60192;
	GroundStations[14].SBandAntenna = GSSA_3PT7METER;
	GroundStations[14].HasRadar = true;
	GroundStations[14].HasAcqAid = true;
	GroundStations[14].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[14].StationPurpose = GSPT_PRELAUNCH|GSPT_LAUNCH|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[14].TrackingCaps = GSTK_USB;
	GroundStations[14].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[14].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[14].Active = true;

	sprintf(GroundStations[15].Name,"USNS HUNTSVILLE"); sprintf(GroundStations[15].Code,"HTV");
	GroundStations[15].SBandAntenna = GSSA_3PT7METER;
	GroundStations[15].HasRadar = true;
	GroundStations[15].HasAcqAid = true;
	GroundStations[15].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[15].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_USB;
	GroundStations[15].USBCaps = GSSC_VOICE|GSSC_TELEMETRY;
	GroundStations[15].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE;
	GroundStations[15].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;

	sprintf(GroundStations[16].Name,"USNS MERCURY"); sprintf(GroundStations[16].Code,"MER");
	GroundStations[16].SBandAntenna = GSSA_9METER;
	GroundStations[16].HasRadar = true;
	GroundStations[16].HasAcqAid = true;
	GroundStations[16].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[16].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[16].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[16].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[16].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[16].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	sprintf(GroundStations[17].Name,"USNS REDSTONE"); sprintf(GroundStations[17].Code,"RED");
	GroundStations[17].SBandAntenna = GSSA_9METER;
	GroundStations[17].HasRadar = true;
	GroundStations[17].HasAcqAid = true;
	GroundStations[17].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[17].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[17].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[17].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[17].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[17].CommCaps = GSGC_DATAHISPEED|GSGC_DATAWIDEBAND|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	sprintf(GroundStations[18].Name,"USNS VANGUARD"); sprintf(GroundStations[18].Code,"VAN");
	GroundStations[18].SBandAntenna = GSSA_9METER;
	GroundStations[18].HasRadar = true;
	GroundStations[18].HasAcqAid = true;
	GroundStations[18].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[18].UpTlmCaps = GSCC_USB|GSCC_UHF;
	GroundStations[18].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	GroundStations[18].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[18].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[18].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	sprintf(GroundStations[19].Name,"USNS WATERTOWN"); sprintf(GroundStations[19].Code,"WTN");
	GroundStations[19].SBandAntenna = GSSA_3PT7METER;
	GroundStations[19].HasRadar = true;
	GroundStations[19].HasAcqAid = true;
	GroundStations[19].DownTlmCaps = GSDT_USB|GSDT_VHF;
	GroundStations[19].StationPurpose = GSPT_ENTRY|GSPT_ORBITAL|GSPT_NEAR_SPACE;
	// WATERTOWN was not listed in the capabilites table, so I assumed it was outfitted with the usual equipment.
	// This is because it was released to the DoD in 1968, between the Apollo 7 and Apollo 8 flights.
	GroundStations[19].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED|GSTK_USB;
	GroundStations[19].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[19].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;

	// 20 = ARIA-1
	// 21 = ARIA-2
	// 22 = ARIA-3
	// 23 = ARIA-4
	// 24 = ARIA-5
	// 25 = ARIA-6
	// 26 = ARIA-7
	// 27 = ARIA-8
	// 28 = MSFNOC (? THE OPERATIONS CENTER ?)

	// 29 = HSK WING
	sprintf(GroundStations[29].Name,"HONEYSUCKLE"); sprintf(GroundStations[29].Code,"HSKX");
	GroundStations[29].Position[0] = -35.40282; GroundStations[29].Position[1] = 148.98144;
	GroundStations[29].SBandAntenna = GSSA_26METER;
	GroundStations[29].StationPurpose = GSPT_LUNAR;
	GroundStations[29].TrackingCaps = GSTK_USB;
	GroundStations[29].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[29].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[29].Active = true;

	// 30 = GDS WING
	sprintf(GroundStations[30].Name,"GOLDSTONE"); sprintf(GroundStations[30].Code,"GDSX");
	GroundStations[30].Position[0] = 35.33820; GroundStations[30].Position[1] = -116.87421;
	GroundStations[30].SBandAntenna = GSSA_26METER;
	GroundStations[30].StationPurpose = GSPT_LUNAR;
	GroundStations[30].TrackingCaps = GSTK_USB;
	GroundStations[30].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[30].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[30].Active = true;

	// 31 = MAD WING
	sprintf(GroundStations[31].Name,"MADRID"); sprintf(GroundStations[31].Code,"MADX");
	GroundStations[31].Position[0] = 40.45443; GroundStations[31].Position[1] = -4.16990;
	GroundStations[31].SBandAntenna = GSSA_26METER;
	GroundStations[31].StationPurpose = GSPT_LUNAR;
	GroundStations[31].TrackingCaps = GSTK_USB;
	GroundStations[31].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[31].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[31].Active = true;

	// 32 = NTTF (PRELAUNCH?)

	sprintf(GroundStations[33].Name,"TANANARIVE"); sprintf(GroundStations[33].Code,"TAN");
	GroundStations[33].Position[0] = -19.00000; GroundStations[33].Position[1] = 47.27556;	
	GroundStations[33].HasRadar = true;
	GroundStations[33].HasAcqAid = true;
	GroundStations[33].StationPurpose = GSPT_ORBITAL;
	GroundStations[33].TrackingCaps = GSTK_CBAND_LOWSPEED;
	GroundStations[33].USBCaps = 0;
	GroundStations[33].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE;
	GroundStations[33].Active = true;

	// ALL SUBSEQUENT ARE DOD STATIONS
	sprintf(GroundStations[34].Name,"ANTIGUA"); sprintf(GroundStations[34].Code,"ANT");
	GroundStations[34].Position[0] = 17.137222; GroundStations[34].Position[1] = -61.775833;
	GroundStations[34].HasRadar = true;
	GroundStations[34].HasAcqAid = true;
	GroundStations[34].UpTlmCaps = GSCC_UHF;
	GroundStations[34].StationPurpose = GSPT_ORBITAL;
	GroundStations[34].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[34].USBCaps = 0;
	GroundStations[34].CommCaps = 0;
	GroundStations[34].Active = true;

	sprintf(GroundStations[35].Name,"ASCENSION"); sprintf(GroundStations[35].Code,"ASC");
	GroundStations[35].Position[0] = -7.969444; GroundStations[35].Position[1] = -14.393889;
	GroundStations[35].HasRadar = true;
	GroundStations[35].HasAcqAid = true;
	GroundStations[35].StationPurpose = GSPT_ORBITAL;
	GroundStations[35].TrackingCaps = GSTK_USB;
	GroundStations[35].USBCaps = GSSC_VOICE|GSSC_COMMAND|GSSC_TELEMETRY;
	GroundStations[35].CommCaps = GSGC_DATAHISPEED|GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE|GSGC_VIDEO;
	GroundStations[35].Active = true;

	// CAPE KENNEDY: These numbers are guesses. Since it's a DOD site, it's probably at the AFB.
	sprintf(GroundStations[36].Name,"CAPE KENNEDY"); sprintf(GroundStations[36].Code,"CNV");
	GroundStations[36].Position[0] = 28.488889; GroundStations[36].Position[1] = -80.577778;
	GroundStations[36].SBandAntenna = GSSA_9METER;
	GroundStations[36].StationPurpose = GSPT_LAUNCH;
	GroundStations[36].TrackingCaps = GSTK_OPTICAL|GSTK_ODOP|GSTK_CBAND_HIGHSPEED;
	GroundStations[36].USBCaps = 0;
	GroundStations[36].CommCaps = GSGC_VHFAG_VOICE;
	GroundStations[36].Active = true;	

	sprintf(GroundStations[37].Name,"GRAND BAHAMA"); sprintf(GroundStations[37].Code,"GBI");
	GroundStations[37].Position[0] = 26.574167; GroundStations[37].Position[1] = -78.664722;
	GroundStations[37].HasRadar = true;
	GroundStations[37].HasAcqAid = true;
	GroundStations[37].DownTlmCaps = GSDT_VHF;
	GroundStations[37].UpTlmCaps = GSCC_UHF;
	GroundStations[37].StationPurpose = GSPT_LAUNCH;
	GroundStations[37].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[37].USBCaps = 0;
	GroundStations[37].CommCaps = 0;
	GroundStations[37].Active = true;

	sprintf(GroundStations[38].Name,"MERRIT ISLAND"); sprintf(GroundStations[38].Code,"MLA");
	GroundStations[38].Position[0] = 28.488889; GroundStations[38].Position[1] = -80.577778; // Generic KSC coordinates.
	GroundStations[38].HasRadar = true;
	GroundStations[38].HasAcqAid = true;
	GroundStations[38].StationPurpose = GSPT_LAUNCH;
	GroundStations[38].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[38].USBCaps = 0;
	GroundStations[38].CommCaps = 0;
	GroundStations[38].Active = true;

	sprintf(GroundStations[39].Name,"PATRICK AFB"); sprintf(GroundStations[39].Code,"PAT");	
	GroundStations[39].Position[0] = 28.235; GroundStations[39].Position[1] = -80.61;
	GroundStations[39].HasRadar = true;
	GroundStations[39].StationPurpose = GSPT_LAUNCH;
	GroundStations[39].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[39].USBCaps = 0;
	GroundStations[39].CommCaps = 0;
	GroundStations[39].Active = true;

	sprintf(GroundStations[40].Name,"PRETORIA"); sprintf(GroundStations[40].Code,"PRE");
	GroundStations[40].Position[0] = -30.78330; GroundStations[40].Position[1] = 28.58330;	
	GroundStations[40].HasRadar = true;
	GroundStations[40].StationPurpose = GSPT_ORBITAL;
	GroundStations[40].TrackingCaps = GSTK_CBAND_LOWSPEED;
	GroundStations[40].USBCaps = 0;
	GroundStations[40].CommCaps = 0;
	GroundStations[40].Active = true;

	sprintf(GroundStations[41].Name,"VANDENBERG"); sprintf(GroundStations[41].Code,"CAL");
	GroundStations[41].Position[0] = 34.74007; GroundStations[41].Position[1] = -120.61909;	
	GroundStations[41].HasRadar = true;
	GroundStations[41].HasAcqAid = true;
	GroundStations[41].StationPurpose = GSPT_ORBITAL;
	GroundStations[41].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[41].USBCaps = 0;
	GroundStations[41].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE|GSGC_VHFAG_VOICE;
	GroundStations[41].Active = true;

	sprintf(GroundStations[42].Name,"WHITE SANDS"); sprintf(GroundStations[42].Code,"WHS");
	GroundStations[42].Position[0] = 32.35637; GroundStations[42].Position[1] = -106.37826;
	GroundStations[42].HasRadar = true;
	GroundStations[42].HasAcqAid = true;
	GroundStations[42].StationPurpose = GSPT_ORBITAL;
	GroundStations[42].TrackingCaps = GSTK_CBAND_LOWSPEED|GSTK_CBAND_HIGHSPEED;
	GroundStations[42].USBCaps = 0;
	GroundStations[42].CommCaps = GSGC_TELETYPE|GSGC_SCAMA_VOICE;
	GroundStations[42].Active = true;

	// MISSION STATE
	MissionPhase = 0;
	setState(MMST_PRELAUNCH);
	// Earth Revolutions count up from 1, 1st Moon Revolution starts at -180° longitude around the time of LOI.
	EarthRev = 1;
	MoonRev = 0;

	// CAPCOM INTERFACE INITIALIZATION
	// Get handles to annotations.
	// The menu lives in the top left, the message box to the right of that
	NHmenu = oapiCreateAnnotation(false,0.65,_V(1,1,0));
	oapiAnnotationSetPos(NHmenu,0,0,0.15,0.2);
	NHmessages = oapiCreateAnnotation(false,0.65,_V(1,1,0));
	oapiAnnotationSetPos(NHmessages,0.18,0,0.87,0.2);
	NHpad = oapiCreateAnnotation(false,0.65,_V(1,1,0));
	oapiAnnotationSetPos(NHpad,0,0.2,0.33,1);
	// Clobber message output buffer
	msgOutputBuf[0] = 0;
	// Clobber the message ring buffer, then set the index back to zero
	currentMessage = 0;
	while(currentMessage < MAX_MESSAGES){
		messages[currentMessage][0] = 0;
		msgtime[currentMessage] = -1;
		currentMessage++;
	}
	currentMessage = 0;
	// Default PC/NC
	PCOption_Enabled = false;
	sprintf(PCOption_Text, "Roger");
	NCOption_Enabled = false;
	sprintf(NCOption_Text, "Negative");
	// Uplink items
	uplink_size = 0;
}

void MCC::setState(int newState){
	MissionState = newState;
	SubState = 0;
	StateTime = 0;
	SubStateTime = 0;
	NCOption_Enabled = false;
	PCOption_Enabled = false;
}

void MCC::setSubState(int newState){
	SubState = newState;
	SubStateTime = 0;
}

void MCC::TimeStep(double simdt){
	int x=0,y=0,z=0;				// Scratch
	char buf[MAX_MSGSIZE];			// More Scratch

	/* AOS DETERMINATION */
	
	if(GT_Enabled == true){
		LastAOSUpdate += simdt;
		if(LastAOSUpdate > 1){
			double Moonrelang;
			double LOSRange;
			VECTOR3 CMGlobalPos = _V(0,0,0);
			VECTOR3 MoonGlobalPos = _V(0, 0, 0);
			VECTOR3 CM_Vector = _V(0, 0, 0);
			VECTOR3 GSGlobalVector = _V(0, 0, 0);
			VECTOR3 GSVector = _V(0, 0, 0);
			double R_E, R_M;
			bool MoonInTheWay;

			LastAOSUpdate = 0;
			// Bail out if we failed to find either major body
			if(Earth == NULL){ addMessage("Can't find Earth"); GT_Enabled = false; return; }
			if(Moon == NULL){ addMessage("Can't find Moon"); GT_Enabled = false; return; }
			//Or the CSM
			if (cm == NULL) { return; }

			R_E = oapiGetSize(Earth);
			R_M = oapiGetSize(Moon);
				
			// Update previous position data
			CM_Prev_Position[0] = CM_Position[0];
			CM_Prev_Position[1] = CM_Position[1];
			CM_Prev_Position[2] = CM_Position[2];
			CM_Prev_MoonPosition[0] = CM_MoonPosition[0];
			CM_Prev_MoonPosition[1] = CM_MoonPosition[1];
			CM_Prev_MoonPosition[2] = CM_MoonPosition[2];
			// Obtain global positions
			cm->GetGlobalPos(CMGlobalPos);
			oapiGetGlobalPos(Moon, &MoonGlobalPos);

			// Convert to Earth equatorial
			oapiGlobalToEqu(Earth,CMGlobalPos,&CM_Position[1],&CM_Position[0],&CM_Position[2]);
			// Convert to Earth equatorial
			oapiGlobalToLocal(Earth, &CMGlobalPos, &CM_Vector);
			// Convert to Moon equatorial
			oapiGlobalToEqu(Moon, CMGlobalPos, &CM_MoonPosition[1], &CM_MoonPosition[0], &CM_MoonPosition[2]);
			// Convert from radians
			CM_Position[0] *= DEG; 
			CM_Position[1] *= DEG; 
			// Convert from radial distance
			CM_Position[2] -= 6373338; // Launch pad radius should be good enough

			//Within Lunar SOI
			if (length(MoonGlobalPos - CMGlobalPos) < 0.0661e9)
			{
				// If we just crossed the rev line, count it (from -180 it jumps to 180)
				if (CM_Prev_MoonPosition[1] < 0 && CM_MoonPosition[1] >= 0 && cm->stage >= STAGE_ORBIT_SIVB) {
					MoonRev++;
					MoonRevTime = 0.0;
					sprintf(buf, "Rev %d", MoonRev);
					addMessage(buf);
				}
			}
			//Within Earth SOI
			else
			{
				// If we just crossed the rev line, count it
				if (CM_Prev_Position[1] < -80 && CM_Position[1] >= -80 && cm->stage >= STAGE_ORBIT_SIVB) {
					EarthRev++;
					sprintf(buf, "Rev %d", EarthRev);
					addMessage(buf);
				}
			}

			y = 0;

			while (x < MAX_GROUND_STATION) {
				if (GroundStations[x].Active == true) {
					GSVector = _V(cos(GroundStations[x].Position[1] * RAD)*cos(GroundStations[x].Position[0] * RAD), sin(GroundStations[x].Position[0] * RAD), sin(GroundStations[x].Position[1] * RAD)*cos(GroundStations[x].Position[0] * RAD))*R_E;
					oapiLocalToGlobal(Earth, &GSVector, &GSGlobalVector);
					MoonInTheWay = false;
					if (GroundStations[x].StationPurpose&GSPT_LUNAR)
					{
						LOSRange = 5e8;
					}
					else
					{
						LOSRange = 2e7;
					}
					//Moon in the way
					Moonrelang = dotp(unit(MoonGlobalPos - CMGlobalPos),  unit(GSGlobalVector - CMGlobalPos));
					if (Moonrelang > cos(asin(R_M / length(MoonGlobalPos - CMGlobalPos))))
					{
						MoonInTheWay = true;
					}
					if (OrbMech::sight(CM_Vector, GSVector, R_E) && GroundStations[x].AOS == 0 && ((GroundStations[x].USBCaps&GSSC_VOICE) || (GroundStations[x].CommCaps&GSGC_VHFAG_VOICE))) {
						if (length(CM_Vector - GSVector) < LOSRange && !MoonInTheWay)
						{
							GroundStations[x].AOS = 1;
							sprintf(buf, "AOS %s", GroundStations[x].Name);
							addMessage(buf);
						}
					}
					if ((!OrbMech::sight(CM_Vector, GSVector, R_E) || length(CM_Vector - GSVector) > LOSRange || MoonInTheWay) && GroundStations[x].AOS == 1) {
						GroundStations[x].AOS = 0;
						sprintf(buf, "LOS %s", GroundStations[x].Name);
						addMessage(buf);
					}
					if (GroundStations[x].AOS) { y++; }
				}
				x++;
			}
		}
	}

	// MISSION STATE EVALUATOR
	if(MT_Enabled == true){
		// Make sure ground tracking is also on
		if(GT_Enabled == false){ addMessage("Mission tracking requires ground tracking"); MT_Enabled = false; return; }
		// Clock timers
		StateTime += simdt;
		SubStateTime += simdt;
		MoonRevTime += simdt;
		// Handle global mission states
		switch(MissionState){			
		case MST_INIT:
			// INITIALIZATION STATE
			AbortMode = 0;
			// Determine mission type.

			if (cm)
			{
				switch (cm->ApolloNo) {
				case 7:
					MissionType = MTP_C;
					setState(MST_1B_PRELAUNCH);
					break;
				case 8:
					MissionType = MTP_C_PRIME;
					setState(MST_SV_PRELAUNCH);
					break;
				case 9:
					MissionType = MTP_D;
					setState(MST_SV_PRELAUNCH);
					break;
				case 10:
					MissionType = MTP_F;
					setState(MST_SV_PRELAUNCH);
					break;
				case 11:
					MissionType = MTP_G;
					setState(MST_SV_PRELAUNCH);
					break;
				case 12:
				case 13:
				case 14:
					MissionType = MTP_H;
					break;
				case 15:
				case 16:
				case 17:
					MissionType = MTP_J;
					break;
				default:
					// If the ApolloNo is not on this list, you are expected to provide a mission type in the scenario file, which will override the default.
					if (cm->SaturnType == SAT_SATURNV) {
						MissionType = MTP_H;
					}
					if (cm->SaturnType == SAT_SATURN1B) {
						MissionType = MTP_C;
					}
					break;

				}
			}
			else if (lm)
			{
				switch (lm->ApolloNo) {
				case 5:
					MissionType = MTP_B;
					setState(MST_B_PRELAUNCH);
					break;
				}
			}
			if(MissionType == 0){
				sprintf(buf,"Unsupported Mission %d",cm->ApolloNo);
				addMessage(buf);
				MT_Enabled = false;
			}
			break;
		case MST_1B_PRELAUNCH:
			// Await launch
			if(cm->stage >= CM_STAGE){
				// Pad Abort
				addMessage("PAD ABORT");
				setState(MST_ABORT_PL);					
			}else{
				if(cm->stage == LAUNCH_STAGE_ONE){
					// Normal Liftoff
					addMessage("LIFTOFF!");
					MissionPhase = MMST_BOOST;
					setState(MST_1B_LAUNCH);
				}
			}
			break;
		case MST_SV_PRELAUNCH:
			// Await launch
			if(cm->stage >= CM_STAGE){
				// Pad Abort
				addMessage("PAD ABORT");
				setState(MST_ABORT_PL);
			}else{
				if(cm->stage == LAUNCH_STAGE_ONE){
					// Normal Liftoff
					addMessage("LIFTOFF!");
					MissionPhase = MMST_BOOST;
					setState(MST_SV_LAUNCH);
				}
			}
			break;
		case MST_1B_LAUNCH:
			// CALLOUTS TO MAKE:
			// TOWER CLEAR
			if(SubState == 0 && CM_Position[2] > 100){
				addMessage("CLEAR OF THE TOWER");
				setSubState(1);
			}
			// ABORT MODES (1B, 1C)
			// These callouts can safely be made based purely on time; They are early enough in the mission that
			// large deviations from the mission plan will likely cause an abort anyway. Later abort callouts
			// must be made on criteria.
			if(SubState == 1 && StateTime > 61){
				addMessage("MODE 1B");
				setSubState(2);
			}
			if(SubState == 2 && StateTime > 110){
				addMessage("MODE 1C");
				setSubState(3);
			}
			// GO FOR STAGING
			// (What criteria?)

			// TRAJECTORY/GUIDANCE GO
			// (What criteria?)

			// ABORT MODE 4
			// (What criteria?)

			// Abort?

			if (cm->stage == CM_STAGE)
			{
				addMessage("ABORT MODE 1");
				setState(MST_LAUNCH_ABORT);
				AbortMode = 1;
			}
			else if (cm->stage == CSM_LEM_STAGE)
			{
				// AP7 Abort Summary says:
				// Mode 2: 2:46 - 9:30
				// Mode 2 is selected if we are within the time range and must land immediately/no other options available.
				// Mode 3: 9:30 - Insertion
				// Mode 3 is selected if we are within the time range and must land immediately.
				// Mode 4: 9:21 - Insertion
				// Mode 4 is selected if we are within the time range and can land in the Pacific.
				addMessage("ABORT MODE 2/3/4");
				setState(MST_LAUNCH_ABORT);
				AbortMode = 2;
			}
			else
			{
				// Await insertion
				if (cm->stage >= STAGE_ORBIT_SIVB) {
					switch (MissionType) {
					case MTP_C:
						addMessage("INSERTION");
						MissionPhase = MMST_EARTH_ORBIT;
						setState(MST_C_INSERTION);
						break;
					}
				}
			}
			break;
		case MST_SV_LAUNCH:
			// Abort?

			if (cm->stage == CM_STAGE)
			{
				// ABORT MODE 1
				addMessage("ABORT MODE 1");
				setState(MST_LAUNCH_ABORT);
				AbortMode = 1;
			}
			else if (cm->stage == CSM_LEM_STAGE)
			{
				// ABORT MODE 2/3/4
				addMessage("ABORT MODE 2/3/4");
				setState(MST_LAUNCH_ABORT);
				AbortMode = 2;
			}
			else
			{
				// Await insertion
				if (cm->stage >= STAGE_ORBIT_SIVB) {
					addMessage("INSERTION");
					MissionPhase = MMST_EARTH_ORBIT;
					switch (MissionType) {
					case MTP_C_PRIME:
						setState(MST_CP_INSERTION);
						break;
					case MTP_D:
						setState(MST_D_INSERTION);
						break;
					case MTP_F:
						setState(MST_F_INSERTION);
						break;
					case MTP_G:
						setState(MST_G_INSERTION);
						break;
					}
				}
			}
			break;
		}
		// Now handle mission-specific states
		switch (MissionType) {
		case MTP_B:
			/* *********************
			* MISSION B: APOLLO 5 *
			********************* */
			MissionSequence_B();
			break;
		case MTP_C:
			/* *********************
			 * MISSION C: APOLLO 7 *
			 ********************* */
			MissionSequence_C();
			break;
		case MTP_C_PRIME:
			/* **************************
			* MISSION C PRIME: APOLLO 8 *
			*************************** */
			MissionSequence_C_Prime();
			break;
		case MTP_D:
			/* ********************
			* MISSION D: APOLLO 9 *
			********************* */
			MissionSequence_D();
			break;
		case MTP_F:
			/* *********************
			* MISSION F: APOLLO 10 *
			********************** */
			MissionSequence_F();
			break;
		case MTP_G:
			/* *********************
			* MISSION G: APOLLO 11 *
			********************** */
			MissionSequence_G();
			break;
		}
	}

	// MESSAGE LIST MAINTENANCE should come last that way if any of the above prints anything,
	// it gets printed in this timestep rather than the next.
	x = currentMessage+1;					// Index, Point beyond tail
	y = 0;									// Tail of output buffer
	z = 0;									// Updation flag
	msgOutputBuf[0] = 0;					// Clobber buffer
	if(x == MAX_MESSAGES){ x = 0; }			// Wrap index if necessary
	while(x != currentMessage){				// we'll wait till it comes around on the guitar here, and sing it when it does
		if(msgtime[x] != -1){				// with feeling!
			if(msgtime[x] == 0){ z = 1; }	// If message is brand new, set updation flag			
			msgtime[x] += simdt;			// Increment lifetime
			if(msgtime[x] > MSG_DISPLAY_TIME){
				// messages[x][0] = 0;			// Message too old, kill it
				msgtime[x] = -1;			// Mark not-in-use
				z = 1;						// Set updation flag
			}else{				
				sprintf(msgOutputBuf+y,"%s\n",messages[x]);	// Otherwise push to output buffer
				y += strlen(messages[x])+1; // Add message length plus NL
			}
		}
		x++;								// Next message
		if(x == MAX_MESSAGES){ x = 0; }		// Wrap index if necessary
	}
	if(z == 1){								// If we set the updation flag
		oapiAnnotationSetText(NHmessages,msgOutputBuf);	// update the annotation
	}
}

// Add message to ring buffer
void MCC::addMessage(char *msg){
	strncpy(messages[currentMessage],msg,MAX_MSGSIZE);			// Copy string
	msgtime[currentMessage] = 0;								// Mark new
	currentMessage++;											// Advance tail index
	if(currentMessage == MAX_MESSAGES){ currentMessage = 0; }	// Wrap index if necessary
}

// Redisplay ring buffer
void MCC::redisplayMessages(){
	int x=0;
	while(x < MAX_MESSAGES){
		if(messages[x][0] != 0){
			msgtime[x] = 0;
		}
		x++;
	}
	// The ring buffer pointer will walk from the current message forward, so anything that was displayed
	// should be redisplayed in the right order.
}

// Push CM uplink sequence
void MCC::pushCMCUplinkString(const char *str) {
	if (str == NULL) { return; } // Bail
	int len = strlen(str);
	int x = 0;
	while (x < len) {
		if (str[x] == '\n' || str[x] == '\r') { x++; continue; } // Ignore CR and LF if they are present (they should not be)
		this->pushAGCUplinkKey(str[x], true);
		x++;
	}
}

// Push CM uplink sequence
void MCC::pushLGCUplinkString(const char *str) {
	if (str == NULL) { return; } // Bail
	int len = strlen(str);
	int x = 0;
	while (x < len) {
		if (str[x] == '\n' || str[x] == '\r') { x++; continue; } // Ignore CR and LF if they are present (they should not be)
		this->pushAGCUplinkKey(str[x], false);
		x++;
	}
}

// Push CM uplink keystroke
void MCC::pushAGCUplinkKey(char key, bool cm) {
	if (cm)
	{
		this->pushUplinkData(043); // VA, SA for CMC
	}
	else
	{
		this->pushUplinkData(031); // VA, SA for LGC
	}
	switch (key) {
	case 'V': // VERB
		// 11-000-101 11-010-001
		this->pushUplinkData(0305);
		this->pushUplinkData(0321);
		break;
	case 'N': // NOUN
		// 11-111-100 00-011-111
		this->pushUplinkData(0374);
		this->pushUplinkData(0037);
		break;
	case 'E': // ENTER
		// 11-110-000 01-111-100
		this->pushUplinkData(0360);
		this->pushUplinkData(0174);
		break;
	case 'R': // RESET
		// 11-001-001 10-110-010
		this->pushUplinkData(0311);
		this->pushUplinkData(0262);
		break;
	case 'C': // CLEAR
		// 11-111-000 00-111-110
		this->pushUplinkData(0370);
		this->pushUplinkData(0076);
		break;
	case 'K': // KEY RELEASE
		// 11-100-100 11-011-001
		this->pushUplinkData(0344);
		this->pushUplinkData(0331);
		break;
	case '+': 
		// 11-101-000 10-111-010
		this->pushUplinkData(0350);
		this->pushUplinkData(0372);
		break;
	case '-': 
		// 11-101-100 10-011-011
		this->pushUplinkData(0354);
		this->pushUplinkData(0233);
		break;
	case '1': 
		// 10-000-111 11-000-001
		this->pushUplinkData(0207);
		this->pushUplinkData(0301);
		break;
	case '2': 
		// 10-001-011 10-100-010
		this->pushUplinkData(0213);
		this->pushUplinkData(0242);
		break;
	case '3': 
		// 10-001-111 10-000-011
		this->pushUplinkData(0217);
		this->pushUplinkData(0203);
		break;
	case '4': 
		// 10-010-011 01-100-100
		this->pushUplinkData(0223);
		this->pushUplinkData(0144);
		break;
	case '5': 
		// 10-010-111 01-000-101
		this->pushUplinkData(0227);
		this->pushUplinkData(0105);
		break;
	case '6': 
		// 10-011-011 00-100-110
		this->pushUplinkData(0233);
		this->pushUplinkData(0046);
		break;
	case '7': 
		// 10-011-111 00-000-111
		this->pushUplinkData(0237);
		this->pushUplinkData(0007);
		break;
	case '8': 
		// 10-100-010 11-101-000
		this->pushUplinkData(0242);
		this->pushUplinkData(0350);
		break;
	case '9': 
		// 10-100-110 11-001-001
		this->pushUplinkData(0246);
		this->pushUplinkData(0311);
		break;
	case '0': 
		// 11-000-001 11-110-000
		this->pushUplinkData(0301);
		this->pushUplinkData(0360);
		break;
	default:
		// Unknown keystroke!
		return;
	}
}

// Push uplink data word on stack
void MCC::pushUplinkData(unsigned char data) {
	uplink_data[uplink_size] = data;
	uplink_size++;
}

// Uplink string to CM
int MCC::CM_uplink(const unsigned char *data, int len) {
	int remsize = 2048;
	remsize -= cm->pcm.mcc_size;
	// if (cm->pcm.mcc_size > 0) { return -1; } // If busy, bail
	if (len > remsize) { return -2; } // Too long!
	memcpy((cm->pcm.mcc_data+cm->pcm.mcc_size), data, len);
	cm->pcm.mcc_size += len;
	return len;
}

// Uplink string to LM
int MCC::LM_uplink(const unsigned char *data, int len) {
	int remsize = 2048;
	remsize -= lm->VHF.mcc_size;
	// if (lm->pcm.mcc_size > 0) { return -1; } // If busy, bail
	if (len > remsize) { return -2; } // Too long!
	memcpy((lm->VHF.mcc_data + lm->VHF.mcc_size), data, len);
	lm->VHF.mcc_size += len;
	return len;
}

// Send uplink buffer to CMC
int MCC::CM_uplink_buffer() {
	int rv = this->CM_uplink(uplink_data, uplink_size);
	if (rv > 0) {
		uplink_size = 0; // Reset
	}
	return(rv);
}

// Send uplink buffer to LGC
int MCC::LM_uplink_buffer() {
	int rv = this->LM_uplink(uplink_data, uplink_size);
	if (rv > 0) {
		uplink_size = 0; // Reset
	}
	return(rv);
}

// Subthread Entry Point
int MCC::subThread(){
	int Result = 0;
	subThreadStatus = 2; // Running
	
	if (subThreadMode == 0)
	{
		Sleep(5000); // Waste 5 seconds
		Result = 0;  // Success (negative = error)
	}
	else if (MissionType == MTP_B)
	{
		subThreadMacro(subThreadType, subThreadMode);
		Result = 0;
	}
	else if (MissionType == MTP_D)
	{
		subThreadMacro(subThreadType, subThreadMode);
		Result = 0;
	}
	else if (MissionType == MTP_F || MissionType == MTP_G)
	{
		//Try to find LEM
		if (rtcc->calcParams.tgt == NULL)
		{
			OBJHANDLE ves = oapiGetVesselByName(LEMName);
			if (ves != NULL)
			{
				rtcc->calcParams.tgt = oapiGetVesselInterface(ves);
			}
		}

		subThreadMacro(subThreadType, subThreadMode);
		Result = 0;
	}
	else if (MissionType == MTP_C_PRIME)
	{
		subThreadMacro(subThreadType, subThreadMode);
		Result = 0;
	}
	else if (MissionType == MTP_C)
	{
		OBJHANDLE ves = oapiGetVesselByName(LVName);
		if (ves != NULL)
		{
			rtcc->calcParams.tgt = oapiGetVesselInterface(ves); // Should be user-programmable later
		}
		subThreadMacro(subThreadType, subThreadMode);
		Result = 0; // Done
	}
	subThreadStatus = Result;
	// Printing messages from the subthread is not safe, but this is just for testing.
	addMessage("Thread Completed");
	return(0);
}

// Subthread initiation
int MCC::startSubthread(int fcn, int type){
	if(subThreadStatus < 1){
		// Punt thread
		subThreadMode = fcn;
		subThreadType = type;
		subThreadStatus = 1; // Busy
		DWORD id = 0;
		HANDLE h = CreateThread(NULL, 0, MCC_Trampoline, this, 0, &id);
		if(h != NULL){ CloseHandle(h); }
		addMessage("Thread Started");
	}else{
		addMessage("Thread Busy");
		return(-1);
	}
	return(0);
}

// Save State
void MCC::SaveState(FILEHANDLE scn) {
	oapiWriteLine(scn, MCC_START_STRING);
	// Booleans
	SAVE_BOOL("MCC_GT_Enabled", GT_Enabled);	
	SAVE_BOOL("MCC_MT_Enabled", MT_Enabled);
	SAVE_BOOL("MCC_padAutoShow", padAutoShow);
	SAVE_BOOL("MCC_PCOption_Enabled", PCOption_Enabled);
	SAVE_BOOL("MCC_NCOption_Enabled", NCOption_Enabled);
	SAVE_BOOL("MCC_scrubbed", scrubbed);
	// Integers
	SAVE_INT("MCC_MissionType", MissionType);
	SAVE_INT("MCC_MissionPhase", MissionPhase);
	SAVE_INT("MCC_MissionState", MissionState);
	SAVE_INT("MCC_SubState", SubState);
	SAVE_INT("MCC_EarthRev", EarthRev);
	SAVE_INT("MCC_MoonRev", MoonRev);
	SAVE_INT("MCC_AbortMode", AbortMode);
	// Floats
	SAVE_DOUBLE("MCC_StateTime", StateTime);
	SAVE_DOUBLE("MCC_SubStateTime", SubStateTime);
	SAVE_DOUBLE("MCC_MoonRevTime", MoonRevTime);
	// Strings
	if (PCOption_Enabled == true) { SAVE_STRING("MCC_PCOption_Text", PCOption_Text); }
	if (NCOption_Enabled == true) { SAVE_STRING("MCC_NCOption_Text", NCOption_Text); }
	// Write PAD here!
	//SAVE_INT("MCC_padState", padState);
	if (padNumber != 0) { SAVE_INT("MCC_padNumber", padNumber); }
	if (padState >= 0 && padForm != NULL)
	{
		if (padNumber == 1)
		{
			char tmpbuf[36];
			AP7BLK * form = (AP7BLK *)padForm;

			for (int i = 0;i < 8;i++)
			{
				sprintf(tmpbuf, "MCC_AP7BLK_Area[%d]", i);
				SAVE_STRING(tmpbuf, form->Area[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_dVC[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->dVC[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_GETI[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->GETI[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_Lat[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->Lat[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_Lng[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->Lng[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_Wx[%d]", i);
				SAVE_STRING(tmpbuf, form->Wx[i]);
			}
		}
		else if (padNumber == 2)
		{
			char tmpbuf[36];
			P27PAD * form = (P27PAD *)padForm;

			SAVE_DOUBLE("MCC_P27PAD_alt", form->alt);
			for (int i = 0;i < 16;i++)
			{
				sprintf(tmpbuf, "MCC_P27PAD_Data[%d]", i);
				SAVE_INT(tmpbuf, form->Data[0][i]);
			}
			SAVE_DOUBLE("MCC_P27PAD_GET", form->GET[0]);
			SAVE_INT("MCC_P27PAD_Index", form->Index[0]);
			SAVE_DOUBLE("MCC_P27PAD_lat", form->lat);
			SAVE_DOUBLE("MCC_P27PAD_lng", form->lng);
			SAVE_DOUBLE("MCC_P27PAD_NavChk", form->NavChk);
			SAVE_STRING("MCC_P27PAD_Purpose", form->Purpose[0]);
			SAVE_INT("MCC_P27PAD_Verb", form->Verb[0]);
		}
		else if (padNumber == 3)
		{
			AP7NAV * form = (AP7NAV *)padForm;

			SAVE_DOUBLE("MCC_AP7NAV_alt", form->alt[0]);
			SAVE_DOUBLE("MCC_AP7NAV_lat", form->lat[0]);
			SAVE_DOUBLE("MCC_AP7NAV_lng", form->lng[0]);
			SAVE_DOUBLE("MCC_AP7NAV_NavChk", form->NavChk[0]);
		}
		else if (padNumber == 4)
		{
			AP7MNV * form = (AP7MNV *)padForm;

			SAVE_DOUBLE("MCC_AP7MNV_alt", form->alt);
			SAVE_V3("MCC_AP7MNV_Att", form->Att);
			SAVE_DOUBLE("MCC_AP7MNV_burntime", form->burntime);
			SAVE_V3("MCC_AP7MNV_dV", form->dV);
			SAVE_DOUBLE("MCC_AP7MNV_GETI", form->GETI);
			SAVE_DOUBLE("MCC_AP7MNV_HA", form->HA);
			SAVE_DOUBLE("MCC_AP7MNV_HP", form->HP);
			SAVE_DOUBLE("MCC_AP7MNV_lat", form->lat);
			SAVE_DOUBLE("MCC_AP7MNV_lng", form->lng);
			SAVE_DOUBLE("MCC_AP7MNV_NavChk", form->NavChk);
			SAVE_DOUBLE("MCC_AP7MNV_pTrim", form->pTrim);
			SAVE_STRING("MCC_AP7MNV_purpose", form->purpose);
			SAVE_STRING("MCC_AP7MNV_remarks", form->remarks);
			SAVE_DOUBLE("MCC_AP7MNV_Shaft", form->Shaft);
			SAVE_INT("MCC_AP7MNV_Star", form->Star);
			SAVE_DOUBLE("MCC_AP7MNV_Trun", form->Trun);
			SAVE_DOUBLE("MCC_AP7MNV_Vc", form->Vc);
			SAVE_DOUBLE("MCC_AP7MNV_Weight", form->Weight);
			SAVE_DOUBLE("MCC_AP7MNV_yTrim", form->yTrim);
		}
		else if (padNumber == 5)
		{
			AP7TPI * form = (AP7TPI *)padForm;

			SAVE_DOUBLE("MCC_AP7TPI_AZ", form->AZ);
			SAVE_V3("MCC_AP7TPI_Backup_bT", form->Backup_bT);
			SAVE_V3("MCC_AP7TPI_Backup_dV", form->Backup_dV);
			SAVE_DOUBLE("MCC_AP7TPI_dH_Max", form->dH_Max);
			SAVE_DOUBLE("MCC_AP7TPI_dH_Min", form->dH_Min);
			SAVE_DOUBLE("MCC_AP7TPI_dH_TPI", form->dH_TPI);
			SAVE_DOUBLE("MCC_AP7TPI_dTT", form->dTT);
			SAVE_DOUBLE("MCC_AP7TPI_E", form->E);
			SAVE_DOUBLE("MCC_AP7TPI_EL", form->EL);
			SAVE_DOUBLE("MCC_AP7TPI_GET", form->GET);
			SAVE_DOUBLE("MCC_AP7TPI_GETI", form->GETI);
			SAVE_DOUBLE("MCC_AP7TPI_R", form->R);
			SAVE_DOUBLE("MCC_AP7TPI_Rdot", form->Rdot);
			SAVE_V3("MCC_AP7TPI_Vg", form->Vg);
		}
		else if (padNumber == 6)
		{
			AP7ENT * form = (AP7ENT *)padForm;

			SAVE_STRING("MCC_AP7ENT_Area", form->Area[0]);
			SAVE_V3("MCC_AP7ENT_Att400K", form->Att400K[0]);
			SAVE_DOUBLE("MCC_AP7ENT_BankAN", form->BankAN[0]);
			SAVE_DOUBLE("MCC_AP7ENT_DRE", form->DRE[0]);
			SAVE_DOUBLE("MCC_AP7ENT_dVTO", form->dVTO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_Lat", form->Lat[0]);
			SAVE_DOUBLE("MCC_AP7ENT_Lng", form->Lng[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_BankAN", form->PB_BankAN[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_DRE", form->PB_DRE[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_R400K", form->PB_R400K[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_Ret05", form->PB_Ret05[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_Ret2", form->PB_Ret2[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_RetBBO", form->PB_RetBBO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_RetDrog", form->PB_RetDrog[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_RetEBO", form->PB_RetEBO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_RetRB", form->PB_RetRB[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_RTGO", form->PB_RTGO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_PB_VIO", form->PB_VIO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_Ret05", form->Ret05[0]);
			SAVE_DOUBLE("MCC_AP7ENT_Ret2", form->Ret2[0]);
			SAVE_DOUBLE("MCC_AP7ENT_RetBBO", form->RetBBO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_RetDrog", form->RetDrog[0]);
			SAVE_DOUBLE("MCC_AP7ENT_RetEBO", form->RetEBO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_RetRB", form->RetRB[0]);
			SAVE_DOUBLE("MCC_AP7ENT_RTGO", form->RTGO[0]);
			SAVE_DOUBLE("MCC_AP7ENT_VIO", form->VIO[0]);
		}
		else if (padNumber == PT_P37PAD)
		{
			char tmpbuf[36];
			P37PAD * form = (P37PAD *)padForm;

			for (int i = 0;i < 4;i++)
			{
				sprintf(tmpbuf, "MCC_P37PAD_dVT[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->dVT[i]);
				sprintf(tmpbuf, "MCC_P37PAD_GET400K[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->GET400K[i]);
				sprintf(tmpbuf, "MCC_P37PAD_GETI[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->GETI[i]);
				sprintf(tmpbuf, "MCC_P37PAD_lng[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->lng[i]);
			}
		}
		else if (padNumber == 8)
		{
			AP11MNV * form = (AP11MNV *)padForm;

			SAVE_V3("MCC_AP11MNV_Att", form->Att);
			SAVE_INT("MCC_AP11MNV_BSSStar", form->BSSStar);
			SAVE_DOUBLE("MCC_AP11MNV_burntime", form->burntime);
			SAVE_V3("MCC_AP11MNV_dV", form->dV);
			SAVE_V3("MCC_AP11MNV_GDCangles", form->GDCangles);
			SAVE_DOUBLE("MCC_AP11MNV_GET05G", form->GET05G);
			SAVE_DOUBLE("MCC_AP11MNV_GETI", form->GETI);
			SAVE_DOUBLE("MCC_AP11MNV_HA", form->HA);
			SAVE_DOUBLE("MCC_AP11MNV_HP", form->HP);
			SAVE_DOUBLE("MCC_AP11MNV_lat", form->lat);
			SAVE_DOUBLE("MCC_AP11MNV_lng", form->lng);
			SAVE_STRING("MCC_AP11MNV_PropGuid", form->PropGuid);
			SAVE_DOUBLE("MCC_AP11MNV_pTrim", form->pTrim);
			SAVE_STRING("MCC_AP11MNV_purpose", form->purpose);
			SAVE_STRING("MCC_AP11MNV_remarks", form->remarks);
			SAVE_DOUBLE("MCC_AP11MNV_RTGO", form->RTGO);
			SAVE_STRING("MCC_AP11MNV_SetStars", form->SetStars);
			SAVE_DOUBLE("MCC_AP11MNV_Shaft", form->Shaft);
			SAVE_DOUBLE("MCC_AP11MNV_SPA", form->SPA);
			SAVE_INT("MCC_AP11MNV_Star", form->Star);
			SAVE_DOUBLE("MCC_AP11MNV_SXP", form->SXP);
			SAVE_DOUBLE("MCC_AP11MNV_Trun", form->Trun);
			SAVE_DOUBLE("MCC_AP11MNV_Vc", form->Vc);
			SAVE_DOUBLE("MCC_AP11MNV_VI0", form->VI0);
			SAVE_DOUBLE("MCC_AP11MNV_Vt", form->Vt);
			SAVE_DOUBLE("MCC_AP11MNV_Weight", form->Weight);
			SAVE_DOUBLE("MCC_AP11MNV_yTrim", form->yTrim);
		}
		else if (padNumber == 9)
		{
			AP11ENT * form = (AP11ENT *)padForm;

			SAVE_STRING("MCC_AP11ENT_Area", form->Area[0]);
			SAVE_V3("MCC_AP11ENT_Att05", form->Att05[0]);
			SAVE_INT("MCC_AP11ENT_BSS", form->BSS[0]);
			SAVE_DOUBLE("MCC_AP11ENT_DLMax", form->DLMax[0]);
			SAVE_DOUBLE("MCC_AP11ENT_DLMin", form->DLMin[0]);
			SAVE_DOUBLE("MCC_AP11ENT_DO", form->DO[0]);
			SAVE_DOUBLE("MCC_AP11ENT_Gamma400K", form->Gamma400K[0]);
			SAVE_DOUBLE("MCC_AP11ENT_GETHorCheck", form->GETHorCheck[0]);
			SAVE_DOUBLE("MCC_AP11ENT_Lat", form->Lat[0]);
			SAVE_STRING("MCC_AP11ENT_LiftVector", form->LiftVector[0]);
			SAVE_DOUBLE("MCC_AP11ENT_Lng", form->Lng[0]);
			SAVE_DOUBLE("MCC_AP11ENT_MaxG", form->MaxG[0]);
			SAVE_DOUBLE("MCC_AP11ENT_PitchHorCheck", form->PitchHorCheck[0]);
			SAVE_STRING("MCC_AP11ENT_remarks", form->remarks[0]);
			SAVE_DOUBLE("MCC_AP11ENT_RET05", form->RET05[0]);
			SAVE_DOUBLE("MCC_AP11ENT_RETBBO", form->RETBBO[0]);
			SAVE_DOUBLE("MCC_AP11ENT_RETDRO", form->RETDRO[0]);
			SAVE_DOUBLE("MCC_AP11ENT_RETEBO", form->RETEBO[0]);
			SAVE_DOUBLE("MCC_AP11ENT_RETVCirc", form->RETVCirc[0]);
			SAVE_DOUBLE("MCC_AP11ENT_RRT", form->RRT[0]);
			SAVE_DOUBLE("MCC_AP11ENT_RTGO", form->RTGO[0]);
			SAVE_DOUBLE("MCC_AP11ENT_SFT", form->SFT[0]);
			SAVE_DOUBLE("MCC_AP11ENT_SPA", form->SPA[0]);
			SAVE_DOUBLE("MCC_AP11ENT_SXP", form->SXP[0]);
			SAVE_INT("MCC_AP11ENT_SXTS", form->SXTS[0]);
			SAVE_DOUBLE("MCC_AP11ENT_TRN", form->TRN[0]);
			SAVE_DOUBLE("MCC_AP11ENT_V400K", form->V400K[0]);
			SAVE_DOUBLE("MCC_AP11ENT_VIO", form->VIO[0]);
			SAVE_DOUBLE("MCC_AP11ENT_VLMax", form->VLMax[0]);
			SAVE_DOUBLE("MCC_AP11ENT_VLMin", form->VLMin[0]);
		}
		else if (padNumber == PT_TLIPAD)
		{
			TLIPAD * form = (TLIPAD *)padForm;

			SAVE_DOUBLE("MCC_TLIPAD_BurnTime", form->BurnTime);
			SAVE_DOUBLE("MCC_TLIPAD_dVC", form->dVC);
			SAVE_V3("MCC_TLIPAD_ExtATT", form->ExtATT);
			SAVE_V3("MCC_TLIPAD_IgnATT", form->IgnATT);
			SAVE_V3("MCC_TLIPAD_SepATT", form->SepATT);
			SAVE_DOUBLE("MCC_TLIPAD_TB6P", form->TB6P);
			SAVE_DOUBLE("MCC_TLIPAD_VI", form->VI);
			SAVE_STRING("MCC_TLIPAD_remarks", form->remarks);
		}
		else if (padNumber == 11)
		{
			STARCHKPAD * form = (STARCHKPAD *)padForm;

			SAVE_DOUBLE("MCC_STARCHKPAD_GET", form->GET[0]);
			SAVE_V3("MCC_STARCHKPAD_Att", form->Att[0]);
			SAVE_DOUBLE("MCC_STARCHKPAD_TAlign", form->TAlign[0]);
		}
		else if (padNumber == 12)
		{
			AP10MAPUPDATE * form = (AP10MAPUPDATE *)padForm;

			SAVE_INT("MCC_AP10MAPUPDATE_REV", form->Rev);
			SAVE_INT("MCC_AP10MAPUPDATE_type", form->type);
			SAVE_DOUBLE("MCC_AP10MAPUPDATE_AOSGET", form->AOSGET);
			SAVE_DOUBLE("MCC_AP10MAPUPDATE_LOSGET", form->LOSGET);
			SAVE_DOUBLE("MCC_AP10MAPUPDATE_PMGET", form->PMGET);
			SAVE_DOUBLE("MCC_AP10MAPUPDATE_SRGET", form->SRGET);
			SAVE_DOUBLE("MCC_AP10MAPUPDATE_SSGET", form->SSGET);
			if (form->type == 3)
			{
				SAVE_DOUBLE("MCC_AP10MAPUPDATE_AOSGET2", form->AOSGET2);
				SAVE_DOUBLE("MCC_AP10MAPUPDATE_LOSGET2", form->LOSGET2);
				SAVE_DOUBLE("MCC_AP10MAPUPDATE_PMGET2", form->PMGET2);
				SAVE_DOUBLE("MCC_AP10MAPUPDATE_SSGET2", form->SSGET2);
			}
		}
		else if (padNumber == 13)
		{
			char tmpbuf[64];
			AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)padForm;

			SAVE_INT("MCC_AP11LMARKTRKPAD_entries", form->entries);

			for (int i = 0;i < form->entries;i++)
			{
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_LmkID[%d]", i);
				SAVE_STRING(tmpbuf, form->LmkID[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_Alt[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->Alt[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_CRDist[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->CRDist[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_Lat[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->Lat[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_Lng05[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->Lng05[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_T1[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->T1[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_T2[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->T2[i]);
			}
		}
		else if (padNumber == PT_AP10DAPDATA)
		{
			AP10DAPDATA * form = (AP10DAPDATA *)padForm;

			SAVE_DOUBLE("MCC_AP10DAPDATA_OtherVehicleWeight", form->OtherVehicleWeight);
			SAVE_DOUBLE("MCC_AP10DAPDATA_PitchTrim", form->PitchTrim);
			SAVE_DOUBLE("MCC_AP10DAPDATA_ThisVehicleWeight", form->ThisVehicleWeight);
			SAVE_DOUBLE("MCC_AP10DAPDATA_YawTrim", form->YawTrim);
		}
		else if (padNumber == PT_AP11LMMNV)
		{
			AP11LMMNV * form = (AP11LMMNV *)padForm;

			SAVE_V3("MCC_AP11LMMNV_Att", form->Att);
			SAVE_DOUBLE("MCC_AP11LMMNV_burntime", form->burntime);
			SAVE_V3("MCC_AP11LMMNV_dV", form->dV);
			SAVE_DOUBLE("MCC_AP11LMMNV_dVR", form->dVR);
			SAVE_V3("MCC_AP11LMMNV_dV_AGS", form->dV_AGS);
			SAVE_DOUBLE("MCC_AP11LMMNV_GETI", form->GETI);
			SAVE_DOUBLE("MCC_AP11LMMNV_HA", form->HA);
			SAVE_DOUBLE("MCC_AP11LMMNV_HP", form->HP);
			SAVE_DOUBLE("MCC_AP11LMMNV_LMWeight", form->LMWeight);
			SAVE_STRING("MCC_AP11LMMNV_purpose", form->purpose);
			SAVE_STRING("MCC_AP11LMMNV_remarks", form->remarks);
			SAVE_INT("MCC_AP11LMMNV_type", form->type);

			if (form->type == 1)
			{
				SAVE_DOUBLE("MCC_AP11LMMNV_t_CSI", form->t_CSI);
				SAVE_DOUBLE("MCC_AP11LMMNV_t_TPI", form->t_TPI);
			}
			else
			{
				SAVE_INT("MCC_AP11LMMNV_BSSStar", form->BSSStar);
				SAVE_DOUBLE("MCC_AP11LMMNV_SPA", form->SPA);
				SAVE_DOUBLE("MCC_AP11LMMNV_SXP", form->SXP);
			}
		}
		else if (padNumber == PT_AP10CSI)
		{
			AP10CSI * form = (AP10CSI *)padForm;

			SAVE_DOUBLE("MCC_AP10CSI_t_CSI", form->t_CSI);
			SAVE_DOUBLE("MCC_AP10CSI_t_TPI", form->t_TPI);
			SAVE_V3("MCC_AP10CSI_dV_LVLH", form->dV_LVLH);
			SAVE_DOUBLE("MCC_AP10CSI_PLM_FDAI", form->PLM_FDAI);
			SAVE_V3("MCC_AP10CSI_dV_AGS", form->dV_AGS);
			SAVE_INT("MCC_AP10CSI_type", form->type);
		}
		else if (padNumber == PT_GENERIC)
		{
			GENERICPAD * form = (GENERICPAD *)padForm;

			SAVE_STRING("MCC_GENERICPAD_paddata", form->paddata);
		}
		else if (padNumber == PT_AP9AOTSTARPAD)
		{
			AP9AOTSTARPAD *form = (AP9AOTSTARPAD*)padForm;

			SAVE_DOUBLE("MCC_AP9AOTSTARPAD_GET", form->GET);
			SAVE_INT("MCC_AP9AOTSTARPAD_Detent", form->Detent);
			SAVE_INT("MCC_AP9AOTSTARPAD_Star", form->Star);
			SAVE_V3("MCC_AP9AOTSTARPAD_CSMAtt", form->CSMAtt);
		}
		else if (padNumber == PT_TORQANG)
		{
			TORQANG *form = (TORQANG*)padForm;

			SAVE_V3("MCC_TORQANG_V42Angles", form->V42Angles);
		}
		else if (padNumber == PT_AP9LMTPI)
		{
			AP9LMTPI * form = (AP9LMTPI *)padForm;

			SAVE_DOUBLE("MCC_AP9LMTPI_dVR", form->dVR);
			SAVE_DOUBLE("MCC_AP9LMTPI_GETI", form->GETI);
			SAVE_DOUBLE("MCC_AP9LMTPI_R", form->R);
			SAVE_DOUBLE("MCC_AP9LMTPI_Rdot", form->Rdot);
			SAVE_V3("MCC_AP9LMTPI_Att", form->Att);
			SAVE_V3("MCC_AP9LMTPI_Backup_dV", form->Backup_dV);
			SAVE_V3("MCC_AP9LMTPI_Vg", form->Vg);
		}
		else if (padNumber == PT_AP9LMCDH)
		{
			AP9LMCDH * form = (AP9LMCDH *)padForm;

			SAVE_DOUBLE("MCC_AP9LMCDH_GETI", form->GETI);
			SAVE_DOUBLE("MCC_AP9LMCDH_Pitch", form->Pitch);
			SAVE_V3("MCC_AP9LMCDH_Vg", form->Vg);
			SAVE_V3("MCC_AP9LMCDH_Vg_AGS", form->Vg_AGS);
		}
		else if (padNumber == PT_S065UPDATE)
		{
			char tmpbuf[64];
			S065UPDATE *form = (S065UPDATE*)padForm;

			for (int i = 0;i < 4;i++)
			{
				sprintf(tmpbuf, "MCC_S065UPDATE_Area[%d]", i);
				SAVE_STRING(tmpbuf, form->Area[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_ExposureInterval[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->ExposureInterval[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_ExposureNum[%d]", i);
				SAVE_INT(tmpbuf, form->ExposureNum[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_FDAIAngles[%d]", i);
				SAVE_V3(tmpbuf, form->FDAIAngles[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_GETStart[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->GETStart[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_OrbRate[%d]", i);
				SAVE_BOOL(tmpbuf, form->OrbRate[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_TAlign[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->TAlign[i]);
			}
		}
		else if (padNumber == PT_AP11AGSACT)
		{
			AP11AGSACT *form = (AP11AGSACT*)padForm;

			SAVE_DOUBLE("MCC_AP11AGSACT_KFactor", form->KFactor);
			SAVE_INT("MCC_AP11AGSACT_DEDA224", form->DEDA224);
			SAVE_INT("MCC_AP11AGSACT_DEDA225", form->DEDA225);
			SAVE_INT("MCC_AP11AGSACT_DEDA226", form->DEDA226);
			SAVE_INT("MCC_AP11AGSACT_DEDA227", form->DEDA227);
		}
		else if (padNumber == PT_AP11PDIPAD)
		{
			AP11PDIPAD *form = (AP11PDIPAD*)padForm;

			SAVE_V3("MCC_AP11PDIPAD_Att", form->Att);
			SAVE_DOUBLE("MCC_AP11PDIPAD_CR", form->CR);
			SAVE_DOUBLE("MCC_AP11PDIPAD_DEDA231", form->DEDA231);
			SAVE_DOUBLE("MCC_AP11PDIPAD_GETI", form->GETI);
			SAVE_DOUBLE("MCC_AP11PDIPAD_t_go", form->t_go);
		}
		else if (padNumber == PT_PDIABORTPAD)
		{
			PDIABORTPAD *form = (PDIABORTPAD*)padForm;

			SAVE_DOUBLE("MCC_PDIABORTPAD_T_Phasing", form->T_Phasing);
			SAVE_DOUBLE("MCC_PDIABORTPAD_T_TPI_Post10Min", form->T_TPI_Post10Min);
			SAVE_DOUBLE("MCC_PDIABORTPAD_T_TPI_Pre10Min", form->T_TPI_Pre10Min);
			SAVE_INT("MCC_PDIABORTPAD_type", form->type);
		}
		else if (padNumber == PT_AP11LUNSURFPAD)
		{
			AP11LunarSurfaceDataCard *form = (AP11LunarSurfaceDataCard*)padForm;

			SAVE_DOUBLE("MCC_AP11T2ABORTPAD_TIG", form->T2_TIG);
			SAVE_DOUBLE("MCC_AP11T2ABORTPAD_t_CSI1", form->T2_t_CSI1);
			SAVE_DOUBLE("MCC_AP11T2ABORTPAD_t_Phasing", form->T2_t_Phasing);
			SAVE_DOUBLE("MCC_AP11T2ABORTPAD_t_TPI", form->T3_t_TPI);
			SAVE_DOUBLE("MCC_AP11T3ABORTPAD_TIG", form->T3_TIG);
			SAVE_DOUBLE("MCC_AP11T3ABORTPAD_t_CSI", form->T3_t_CSI);
			SAVE_DOUBLE("MCC_AP11T3ABORTPAD_t_Period", form->T3_t_Period);
			SAVE_DOUBLE("MCC_AP11T3ABORTPAD_t_PPlusDT", form->T3_t_PPlusDT);
			SAVE_DOUBLE("MCC_AP11T3ABORTPAD_t_TPI", form->T3_t_TPI);
		}
		else if (padNumber == PT_AP11P76PAD)
		{
			AP11P76PAD *form = (AP11P76PAD*)padForm;

			char tmpbuf[64];

			SAVE_INT("MCC_AP11P76PAD_entries", form->entries);

			for (int i = 0;i < form->entries;i++)
			{
				sprintf(tmpbuf, "MCC_AP11P76PAD_DV[%d]", i);
				SAVE_V3(tmpbuf, form->DV[i]);
				sprintf(tmpbuf, "MCC_AP11P76PAD_purpose[%d]", i);
				SAVE_STRING(tmpbuf, form->purpose[i]);
				sprintf(tmpbuf, "MCC_AP11P76PAD_TIG[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->TIG[i]);
			}
		}
		else if (padNumber == PT_AP11LMASCPAD)
		{
			AP11LMASCPAD *form = (AP11LMASCPAD*)padForm;

			SAVE_DOUBLE("MCC_AP11LMASCPAD_CR", form->CR);
			SAVE_INT("MCC_AP11LMASCPAD_DEDA047", form->DEDA047);
			SAVE_INT("MCC_AP11LMASCPAD_DEDA053", form->DEDA053);
			SAVE_DOUBLE("MCC_AP11LMASCPAD_DEDA225_226", form->DEDA225_226);
			SAVE_DOUBLE("MCC_AP11LMASCPAD_DEDA231", form->DEDA231);
			SAVE_DOUBLE("MCC_AP11LMASCPAD_TIG", form->TIG);
			SAVE_DOUBLE("MCC_AP11LMASCPAD_V_hor", form->V_hor);
			SAVE_DOUBLE("MCC_AP11LMASCPAD_V_vert", form->V_vert);
			SAVE_STRING("MCC_AP11LMASCPAD_remarks", form->remarks);
		}
		else if (padNumber == PT_LIFTOFFTIMES)
		{
			LIFTOFFTIMES *form = (LIFTOFFTIMES*)padForm;

			char tmpbuf[64];

			SAVE_INT("MCC_LIFTOFFTIMES_entries", form->entries);
			SAVE_INT("MCC_LIFTOFFTIMES_startdigit", form->startdigit);

			for (int i = 0;i < form->entries;i++)
			{
				sprintf(tmpbuf, "MCC_LIFTOFFTIMES_TIG[%d]", i);
				SAVE_DOUBLE(tmpbuf, form->TIG[i]);
			}
		}
		else if (padNumber == PT_LMACTDATA)
		{
			LMACTDATA *form = (LMACTDATA*)padForm;

			SAVE_DOUBLE("MCC_LMACTDATA_CSMWeight", form->CSMWeight);
			SAVE_DOUBLE("MCC_LMACTDATA_LMWeight", form->LMWeight);
			SAVE_DOUBLE("MCC_LMACTDATA_PitchTrim", form->PitchTrim);
			SAVE_DOUBLE("MCC_LMACTDATA_RollTrim", form->RollTrim);
			SAVE_V3("MCC_LMACTDATA_V42Angles", form->V42Angles);
		}
	}
	// Write uplink buffer here!
	if (upString[0] != 0 && uplink_size > 0) { SAVE_STRING("MCC_upString", upString); }
	if (upString[0] != 0 && uplink_size > 0) { SAVE_INT("MCC_upType", upType); }
	if (upDescr[0]) { SAVE_STRING("MCC_upDescr", upDescr); }
	// Done
	oapiWriteLine(scn, MCC_END_STRING);
}

// Load State
void MCC::LoadState(FILEHANDLE scn) {
	char *line;
	int tmp = 0; // Used in boolean type loader
	bool padisallocated = false;

	char tmpbuf[64];

	while (oapiReadScenario_nextline(scn, line)) {
		if (!strnicmp(line, MCC_END_STRING, sizeof(MCC_END_STRING))) {
			break;
		}
		LOAD_BOOL("MCC_GT_Enabled", GT_Enabled);
		LOAD_BOOL("MCC_MT_Enabled", MT_Enabled);
		LOAD_BOOL("MCC_padAutoShow", padAutoShow);
		LOAD_BOOL("MCC_PCOption_Enabled", PCOption_Enabled);
		LOAD_BOOL("MCC_NCOption_Enabled", NCOption_Enabled);
		LOAD_BOOL("MCC_scrubbed", scrubbed);
		LOAD_INT("MCC_MissionType", MissionType);
		LOAD_INT("MCC_MissionPhase", MissionPhase);
		LOAD_INT("MCC_MissionState", MissionState);
		LOAD_INT("MCC_SubState", SubState);
		LOAD_INT("MCC_EarthRev", EarthRev);
		LOAD_INT("MCC_MoonRev", MoonRev);
		LOAD_INT("MCC_AbortMode", AbortMode);
		LOAD_DOUBLE("MCC_StateTime", StateTime);
		LOAD_DOUBLE("MCC_SubStateTime", SubStateTime);
		LOAD_DOUBLE("MCC_MoonRevTime", MoonRevTime);
		LOAD_STRING("MCC_PCOption_Text", PCOption_Text, 32);
		LOAD_STRING("MCC_NCOption_Text", NCOption_Text, 32);
		LOAD_INT("MCC_padNumber", padNumber);
		//LOAD_INT("MCC_padState", padState);
		if (padNumber > 0)
		{
			if (!padisallocated)
			{
				allocPad(padNumber);
				padState = 0;
				padisallocated = true;
			}
		}
		if (padNumber == 1)
		{
			AP7BLK * form = (AP7BLK *)padForm;

			for (int i = 0;i < 8;i++)
			{
				sprintf(tmpbuf, "MCC_AP7BLK_Area[%d]", i);
				LOAD_STRING(tmpbuf, form->Area[i], 10);
				sprintf(tmpbuf, "MCC_AP7BLK_dVC[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->dVC[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_GETI[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->GETI[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_Lat[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->Lat[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_Lng[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->Lng[i]);
				sprintf(tmpbuf, "MCC_AP7BLK_Wx[%d]", i);
				LOAD_STRING(tmpbuf, form->Wx[i], 10);
			}
		}
		else if (padNumber == 2)
		{
			P27PAD * form = (P27PAD *)padForm;

			LOAD_DOUBLE("MCC_P27PAD_alt", form->alt);
			for (int i = 0;i < 16;i++)
			{
				sprintf(tmpbuf, "MCC_P27PAD_Data[%d]", i);
				LOAD_INT(tmpbuf, form->Data[0][i]);
			}
			LOAD_DOUBLE("MCC_P27PAD_GET", form->GET[0]);
			LOAD_INT("MCC_P27PAD_Index", form->Index[0]);
			LOAD_DOUBLE("MCC_P27PAD_lat", form->lat);
			LOAD_DOUBLE("MCC_P27PAD_lng", form->lng);
			LOAD_DOUBLE("MCC_P27PAD_NavChk", form->NavChk);
			LOAD_STRING("MCC_P27PAD_Purpose", form->Purpose[0], 64);
			LOAD_INT("MCC_P27PAD_Verb", form->Verb[0]);
		}
		else if (padNumber == 3)
		{
			AP7NAV * form = (AP7NAV *)padForm;

			LOAD_DOUBLE("MCC_AP7NAV_alt", form->alt[0]);
			LOAD_DOUBLE("MCC_AP7NAV_lat", form->lat[0]);
			LOAD_DOUBLE("MCC_AP7NAV_lng", form->lng[0]);
			LOAD_DOUBLE("MCC_AP7NAV_NavChk", form->NavChk[0]);
		}
		else if (padNumber == 4)
		{
			AP7MNV * form = (AP7MNV *)padForm;

			LOAD_DOUBLE("MCC_AP7MNV_alt", form->alt);
			LOAD_V3("MCC_AP7MNV_Att", form->Att);
			LOAD_DOUBLE("MCC_AP7MNV_burntime", form->burntime);
			LOAD_V3("MCC_AP7MNV_dV", form->dV);
			LOAD_DOUBLE("MCC_AP7MNV_GETI", form->GETI);
			LOAD_DOUBLE("MCC_AP7MNV_HA", form->HA);
			LOAD_DOUBLE("MCC_AP7MNV_HP", form->HP);
			LOAD_DOUBLE("MCC_AP7MNV_lat", form->lat);
			LOAD_DOUBLE("MCC_AP7MNV_lng", form->lng);
			LOAD_DOUBLE("MCC_AP7MNV_NavChk", form->NavChk);
			LOAD_DOUBLE("MCC_AP7MNV_pTrim", form->pTrim);
			LOAD_STRING("MCC_AP7MNV_purpose", form->purpose, 64);
			LOAD_STRING("MCC_AP7MNV_remarks", form->remarks, 128);
			LOAD_DOUBLE("MCC_AP7MNV_Shaft", form->Shaft);
			LOAD_INT("MCC_AP7MNV_Star", form->Star);
			LOAD_DOUBLE("MCC_AP7MNV_Trun", form->Trun);
			LOAD_DOUBLE("MCC_AP7MNV_Vc", form->Vc);
			LOAD_DOUBLE("MCC_AP7MNV_Weight", form->Weight);
			LOAD_DOUBLE("MCC_AP7MNV_yTrim", form->yTrim);
		}
		else if (padNumber == 5)
		{
			AP7TPI * form = (AP7TPI *)padForm;

			LOAD_DOUBLE("MCC_AP7TPI_AZ", form->AZ);
			LOAD_V3("MCC_AP7TPI_Backup_bT", form->Backup_bT);
			LOAD_V3("MCC_AP7TPI_Backup_dV", form->Backup_dV);
			LOAD_DOUBLE("MCC_AP7TPI_dH_Max", form->dH_Max);
			LOAD_DOUBLE("MCC_AP7TPI_dH_Min", form->dH_Min);
			LOAD_DOUBLE("MCC_AP7TPI_dH_TPI", form->dH_TPI);
			LOAD_DOUBLE("MCC_AP7TPI_dTT", form->dTT);
			LOAD_DOUBLE("MCC_AP7TPI_E", form->E);
			LOAD_DOUBLE("MCC_AP7TPI_EL", form->EL);
			LOAD_DOUBLE("MCC_AP7TPI_GET", form->GET);
			LOAD_DOUBLE("MCC_AP7TPI_GETI", form->GETI);
			LOAD_DOUBLE("MCC_AP7TPI_R", form->R);
			LOAD_DOUBLE("MCC_AP7TPI_Rdot", form->Rdot);
			LOAD_V3("MCC_AP7TPI_Vg", form->Vg);
		}
		else if (padNumber == 6)
		{
			AP7ENT * form = (AP7ENT *)padForm;

			LOAD_STRING("MCC_AP7ENT_Area", form->Area[0], 10);
			LOAD_V3("MCC_AP7ENT_Att400K", form->Att400K[0]);
			LOAD_DOUBLE("MCC_AP7ENT_BankAN", form->BankAN[0]);
			LOAD_DOUBLE("MCC_AP7ENT_DRE", form->DRE[0]);
			LOAD_DOUBLE("MCC_AP7ENT_dVTO", form->dVTO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_Lat", form->Lat[0]);
			LOAD_DOUBLE("MCC_AP7ENT_Lng", form->Lng[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_BankAN", form->PB_BankAN[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_DRE", form->PB_DRE[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_R400K", form->PB_R400K[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_Ret05", form->PB_Ret05[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_Ret2", form->PB_Ret2[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_RetBBO", form->PB_RetBBO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_RetDrog", form->PB_RetDrog[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_RetEBO", form->PB_RetEBO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_RetRB", form->PB_RetRB[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_RTGO", form->PB_RTGO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_PB_VIO", form->PB_VIO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_Ret05", form->Ret05[0]);
			LOAD_DOUBLE("MCC_AP7ENT_Ret2", form->Ret2[0]);
			LOAD_DOUBLE("MCC_AP7ENT_RetBBO", form->RetBBO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_RetDrog", form->RetDrog[0]);
			LOAD_DOUBLE("MCC_AP7ENT_RetEBO", form->RetEBO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_RetRB", form->RetRB[0]);
			LOAD_DOUBLE("MCC_AP7ENT_RTGO", form->RTGO[0]);
			LOAD_DOUBLE("MCC_AP7ENT_VIO", form->VIO[0]);
		}
		else if (padNumber == PT_P37PAD)
		{
			P37PAD * form = (P37PAD *)padForm;

			for (int i = 0;i < 4;i++)
			{
				sprintf(tmpbuf, "MCC_P37PAD_dVT[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->dVT[i]);
				sprintf(tmpbuf, "MCC_P37PAD_GET400K[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->GET400K[i]);
				sprintf(tmpbuf, "MCC_P37PAD_GETI[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->GETI[i]);
				sprintf(tmpbuf, "MCC_P37PAD_lng[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->lng[i]);
			}
		}
		else if (padNumber == 8)
		{
			AP11MNV * form = (AP11MNV *)padForm;

			LOAD_V3("MCC_AP11MNV_Att", form->Att);
			LOAD_INT("MCC_AP11MNV_BSSStar", form->BSSStar);
			LOAD_DOUBLE("MCC_AP11MNV_burntime", form->burntime);
			LOAD_V3("MCC_AP11MNV_dV", form->dV);
			LOAD_V3("MCC_AP11MNV_GDCangles", form->GDCangles);
			LOAD_DOUBLE("MCC_AP11MNV_GET05G", form->GET05G);
			LOAD_DOUBLE("MCC_AP11MNV_GETI", form->GETI);
			LOAD_DOUBLE("MCC_AP11MNV_HA", form->HA);
			LOAD_DOUBLE("MCC_AP11MNV_HP", form->HP);
			LOAD_DOUBLE("MCC_AP11MNV_lat", form->lat);
			LOAD_DOUBLE("MCC_AP11MNV_lng", form->lng);
			LOAD_STRING("MCC_AP11MNV_PropGuid", form->PropGuid, 64);
			LOAD_DOUBLE("MCC_AP11MNV_pTrim", form->pTrim);
			LOAD_STRING("MCC_AP11MNV_purpose", form->purpose, 64);
			LOAD_STRING("MCC_AP11MNV_remarks", form->remarks, 128);
			LOAD_DOUBLE("MCC_AP11MNV_RTGO", form->RTGO);
			LOAD_STRING("MCC_AP11MNV_SetStars", form->SetStars, 32);
			LOAD_DOUBLE("MCC_AP11MNV_Shaft", form->Shaft);
			LOAD_DOUBLE("MCC_AP11MNV_SPA", form->SPA);
			LOAD_INT("MCC_AP11MNV_Star", form->Star);
			LOAD_DOUBLE("MCC_AP11MNV_SXP", form->SXP);
			LOAD_DOUBLE("MCC_AP11MNV_Trun", form->Trun);
			LOAD_DOUBLE("MCC_AP11MNV_Vc", form->Vc);
			LOAD_DOUBLE("MCC_AP11MNV_VI0", form->VI0);
			LOAD_DOUBLE("MCC_AP11MNV_Vt", form->Vt);
			LOAD_DOUBLE("MCC_AP11MNV_Weight", form->Weight);
			LOAD_DOUBLE("MCC_AP11MNV_yTrim", form->yTrim);
		}
		else if (padNumber == 9)
		{
			AP11ENT * form = (AP11ENT *)padForm;

			LOAD_STRING("MCC_AP11ENT_Area", form->Area[0], 10);
			LOAD_V3("MCC_AP11ENT_Att05", form->Att05[0]);
			LOAD_INT("MCC_AP11ENT_BSS", form->BSS[0]);
			LOAD_DOUBLE("MCC_AP11ENT_DLMax", form->DLMax[0]);
			LOAD_DOUBLE("MCC_AP11ENT_DLMin", form->DLMin[0]);
			LOAD_DOUBLE("MCC_AP11ENT_DO", form->DO[0]);
			LOAD_DOUBLE("MCC_AP11ENT_Gamma400K", form->Gamma400K[0]);
			LOAD_DOUBLE("MCC_AP11ENT_GETHorCheck", form->GETHorCheck[0]);
			LOAD_DOUBLE("MCC_AP11ENT_Lat", form->Lat[0]);
			LOAD_STRING("MCC_AP11ENT_LiftVector", form->LiftVector[0], 4);
			LOAD_DOUBLE("MCC_AP11ENT_Lng", form->Lng[0]);
			LOAD_DOUBLE("MCC_AP11ENT_MaxG", form->MaxG[0]);
			LOAD_DOUBLE("MCC_AP11ENT_PitchHorCheck", form->PitchHorCheck[0]);
			LOAD_STRING("MCC_AP11ENT_remarks", form->remarks[0], 128);
			LOAD_DOUBLE("MCC_AP11ENT_RET05", form->RET05[0]);
			LOAD_DOUBLE("MCC_AP11ENT_RETBBO", form->RETBBO[0]);
			LOAD_DOUBLE("MCC_AP11ENT_RETDRO", form->RETDRO[0]);
			LOAD_DOUBLE("MCC_AP11ENT_RETEBO", form->RETEBO[0]);
			LOAD_DOUBLE("MCC_AP11ENT_RETVCirc", form->RETVCirc[0]);
			LOAD_DOUBLE("MCC_AP11ENT_RRT", form->RRT[0]);
			LOAD_DOUBLE("MCC_AP11ENT_RTGO", form->RTGO[0]);
			LOAD_DOUBLE("MCC_AP11ENT_SFT", form->SFT[0]);
			LOAD_DOUBLE("MCC_AP11ENT_SPA", form->SPA[0]);
			LOAD_DOUBLE("MCC_AP11ENT_SXP", form->SXP[0]);
			LOAD_INT("MCC_AP11ENT_SXTS", form->SXTS[0]);
			LOAD_DOUBLE("MCC_AP11ENT_TRN", form->TRN[0]);
			LOAD_DOUBLE("MCC_AP11ENT_V400K", form->V400K[0]);
			LOAD_DOUBLE("MCC_AP11ENT_VIO", form->VIO[0]);
			LOAD_DOUBLE("MCC_AP11ENT_VLMax", form->VLMax[0]);
			LOAD_DOUBLE("MCC_AP11ENT_VLMin", form->VLMin[0]);
		}
		else if (padNumber == 10)
		{
			TLIPAD * form = (TLIPAD *)padForm;

			LOAD_DOUBLE("MCC_TLIPAD_BurnTime", form->BurnTime);
			LOAD_DOUBLE("MCC_TLIPAD_dVC", form->dVC);
			LOAD_V3("MCC_TLIPAD_ExtATT", form->ExtATT);
			LOAD_V3("MCC_TLIPAD_IgnATT", form->IgnATT);
			LOAD_V3("MCC_TLIPAD_SepATT", form->SepATT);
			LOAD_DOUBLE("MCC_TLIPAD_TB6P", form->TB6P);
			LOAD_DOUBLE("MCC_TLIPAD_VI", form->VI);
			LOAD_STRING("MCC_TLIPAD_remarks", form->remarks, 128);
		}
		else if (padNumber == 11)
		{
			STARCHKPAD * form = (STARCHKPAD *)padForm;

			LOAD_DOUBLE("MCC_STARCHKPAD_GET", form->GET[0]);
			LOAD_V3("MCC_STARCHKPAD_Att", form->Att[0]);
			LOAD_DOUBLE("MCC_STARCHKPAD_TAlign", form->TAlign[0]);
		}
		else if (padNumber == 12)
		{
			AP10MAPUPDATE * form = (AP10MAPUPDATE *)padForm;

			LOAD_INT("MCC_AP10MAPUPDATE_REV", form->Rev);
			LOAD_INT("MCC_AP10MAPUPDATE_type", form->type);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_AOSGET", form->AOSGET);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_LOSGET", form->LOSGET);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_PMGET", form->PMGET);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_SRGET", form->SRGET);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_SSGET", form->SSGET);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_AOSGET2", form->AOSGET2);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_LOSGET2", form->LOSGET2);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_PMGET2", form->PMGET2);
			LOAD_DOUBLE("MCC_AP10MAPUPDATE_SSGET2", form->SSGET2);
		}
		else if (padNumber == 13)
		{
			AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)padForm;

			LOAD_INT("MCC_AP11LMARKTRKPAD_entries", form->entries);

			for (int i = 0;i < form->entries;i++)
			{
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_LmkID[%d]", i);
				LOAD_STRING(tmpbuf, form->LmkID[i], 128);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_Alt[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->Alt[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_CRDist[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->CRDist[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_Lat[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->Lat[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_Lng05[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->Lng05[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_T1[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->T1[i]);
				sprintf(tmpbuf, "MCC_AP11LMARKTRKPAD_T2[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->T2[i]);
			}
		}
		else if (padNumber == PT_AP10DAPDATA)
		{
			AP10DAPDATA * form = (AP10DAPDATA *)padForm;

			LOAD_DOUBLE("MCC_AP10DAPDATA_OtherVehicleWeight", form->OtherVehicleWeight);
			LOAD_DOUBLE("MCC_AP10DAPDATA_PitchTrim", form->PitchTrim);
			LOAD_DOUBLE("MCC_AP10DAPDATA_ThisVehicleWeight", form->ThisVehicleWeight);
			LOAD_DOUBLE("MCC_AP10DAPDATA_YawTrim", form->YawTrim);
		}
		else if (padNumber == PT_AP11LMMNV)
		{
			AP11LMMNV * form = (AP11LMMNV *)padForm;

			LOAD_V3("MCC_AP11LMMNV_Att", form->Att);
			LOAD_INT("MCC_AP11LMMNV_BSSStar", form->BSSStar);
			LOAD_DOUBLE("MCC_AP11LMMNV_burntime", form->burntime);
			LOAD_V3("MCC_AP11LMMNV_dV", form->dV);
			LOAD_DOUBLE("MCC_AP11LMMNV_dVR", form->dVR);
			LOAD_V3("MCC_AP11LMMNV_dV_AGS", form->dV_AGS);
			LOAD_DOUBLE("MCC_AP11LMMNV_GETI", form->GETI);
			LOAD_DOUBLE("MCC_AP11LMMNV_HA", form->HA);
			LOAD_DOUBLE("MCC_AP11LMMNV_HP", form->HP);
			LOAD_DOUBLE("MCC_AP11LMMNV_LMWeight", form->LMWeight);
			LOAD_STRING("MCC_AP11LMMNV_purpose", form->purpose, 64);
			LOAD_STRING("MCC_AP11LMMNV_remarks", form->remarks, 128);
			LOAD_DOUBLE("MCC_AP11LMMNV_SPA", form->SPA);
			LOAD_DOUBLE("MCC_AP11LMMNV_SXP", form->SXP);
			LOAD_INT("MCC_AP11LMMNV_type", form->type);
			LOAD_DOUBLE("MCC_AP11LMMNV_t_CSI", form->t_CSI);
			LOAD_DOUBLE("MCC_AP11LMMNV_t_TPI", form->t_TPI);
		}
		else if (padNumber == PT_AP10CSI)
		{
			AP10CSI * form = (AP10CSI *)padForm;

			LOAD_DOUBLE("MCC_AP10CSI_t_CSI", form->t_CSI);
			LOAD_DOUBLE("MCC_AP10CSI_t_TPI", form->t_TPI);
			LOAD_V3("MCC_AP10CSI_dV_LVLH", form->dV_LVLH);
			LOAD_DOUBLE("MCC_AP10CSI_PLM_FDAI", form->PLM_FDAI);
			LOAD_V3("MCC_AP10CSI_dV_AGS", form->dV_AGS);
			LOAD_INT("MCC_AP10CSI_type", form->type);
		}
		else if (padNumber == PT_GENERIC)
		{
			GENERICPAD * form = (GENERICPAD *)padForm;

			LOAD_STRING("MCC_GENERICPAD_paddata", form->paddata, 512);
		}
		else if (padNumber == PT_AP9AOTSTARPAD)
		{
			AP9AOTSTARPAD *form = (AP9AOTSTARPAD*)padForm;

			LOAD_DOUBLE("MCC_AP9AOTSTARPAD_GET", form->GET);
			LOAD_INT("MCC_AP9AOTSTARPAD_Detent", form->Detent);
			LOAD_INT("MCC_AP9AOTSTARPAD_Star", form->Star);
			LOAD_V3("MCC_AP9AOTSTARPAD_CSMAtt", form->CSMAtt);
		}
		else if (padNumber == PT_TORQANG)
		{
			TORQANG *form = (TORQANG*)padForm;

			LOAD_V3("MCC_TORQANG_V42Angles", form->V42Angles);
		}
		else if (padNumber == PT_AP9LMTPI)
		{
			AP9LMTPI * form = (AP9LMTPI *)padForm;

			LOAD_DOUBLE("MCC_AP9LMTPI_dVR", form->dVR);
			LOAD_DOUBLE("MCC_AP9LMTPI_GETI", form->GETI);
			LOAD_DOUBLE("MCC_AP9LMTPI_R", form->R);
			LOAD_DOUBLE("MCC_AP9LMTPI_Rdot", form->Rdot);
			LOAD_V3("MCC_AP9LMTPI_Att", form->Att);
			LOAD_V3("MCC_AP9LMTPI_Backup_dV", form->Backup_dV);
			LOAD_V3("MCC_AP9LMTPI_Vg", form->Vg);
		}
		else if (padNumber == PT_AP9LMCDH)
		{
			AP9LMCDH * form = (AP9LMCDH *)padForm;

			LOAD_DOUBLE("MCC_AP9LMCDH_GETI", form->GETI);
			LOAD_DOUBLE("MCC_AP9LMCDH_Pitch", form->Pitch);
			LOAD_V3("MCC_AP9LMCDH_Vg", form->Vg);
			LOAD_V3("MCC_AP9LMCDH_Vg_AGS", form->Vg_AGS);
		}
		else if (padNumber == PT_S065UPDATE)
		{
			S065UPDATE *form = (S065UPDATE*)padForm;

			for (int i = 0;i < 4;i++)
			{
				sprintf(tmpbuf, "MCC_S065UPDATE_Area[%d]", i);
				LOAD_STRING(tmpbuf, form->Area[i], 16);
				sprintf(tmpbuf, "MCC_S065UPDATE_ExposureInterval[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->ExposureInterval[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_ExposureNum[%d]", i);
				LOAD_INT(tmpbuf, form->ExposureNum[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_FDAIAngles[%d]", i);
				LOAD_V3(tmpbuf, form->FDAIAngles[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_GETStart[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->GETStart[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_OrbRate[%d]", i);
				LOAD_BOOL(tmpbuf, form->OrbRate[i]);
				sprintf(tmpbuf, "MCC_S065UPDATE_TAlign[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->TAlign[i]);
			}
		}
		else if (padNumber == PT_AP11AGSACT)
		{
			AP11AGSACT *form = (AP11AGSACT*)padForm;

			LOAD_DOUBLE("MCC_AP11AGSACT_KFactor", form->KFactor);
			LOAD_INT("MCC_AP11AGSACT_DEDA224", form->DEDA224);
			LOAD_INT("MCC_AP11AGSACT_DEDA225", form->DEDA225);
			LOAD_INT("MCC_AP11AGSACT_DEDA226", form->DEDA226);
			LOAD_INT("MCC_AP11AGSACT_DEDA227", form->DEDA227);
		}
		else if (padNumber == PT_AP11PDIPAD)
		{
			AP11PDIPAD *form = (AP11PDIPAD*)padForm;

			LOAD_V3("MCC_AP11PDIPAD_Att", form->Att);
			LOAD_DOUBLE("MCC_AP11PDIPAD_CR", form->CR);
			LOAD_DOUBLE("MCC_AP11PDIPAD_DEDA231", form->DEDA231);
			LOAD_DOUBLE("MCC_AP11PDIPAD_GETI", form->GETI);
			LOAD_DOUBLE("MCC_AP11PDIPAD_t_go", form->t_go);
		}
		else if (padNumber == PT_PDIABORTPAD)
		{
			PDIABORTPAD *form = (PDIABORTPAD*)padForm;

			LOAD_DOUBLE("MCC_PDIABORTPAD_T_Phasing", form->T_Phasing);
			LOAD_DOUBLE("MCC_PDIABORTPAD_T_TPI_Post10Min", form->T_TPI_Post10Min);
			LOAD_DOUBLE("MCC_PDIABORTPAD_T_TPI_Pre10Min", form->T_TPI_Pre10Min);
			LOAD_INT("MCC_PDIABORTPAD_type", form->type);
		}
		else if (padNumber == PT_AP11LUNSURFPAD)
		{
		AP11LunarSurfaceDataCard *form = (AP11LunarSurfaceDataCard*)padForm;

			LOAD_DOUBLE("MCC_AP11T2ABORTPAD_TIG", form->T2_TIG);
			LOAD_DOUBLE("MCC_AP11T2ABORTPAD_t_CSI1", form->T2_t_CSI1);
			LOAD_DOUBLE("MCC_AP11T2ABORTPAD_t_Phasing", form->T2_t_Phasing);
			LOAD_DOUBLE("MCC_AP11T2ABORTPAD_t_TPI", form->T2_t_TPI);
			LOAD_DOUBLE("MCC_AP11T3ABORTPAD_TIG", form->T3_TIG);
			LOAD_DOUBLE("MCC_AP11T3ABORTPAD_t_CSI", form->T3_t_CSI);
			LOAD_DOUBLE("MCC_AP11T3ABORTPAD_t_Period", form->T3_t_Period);
			LOAD_DOUBLE("MCC_AP11T3ABORTPAD_t_PPlusDT", form->T3_t_PPlusDT);
			LOAD_DOUBLE("MCC_AP11T3ABORTPAD_t_TPI", form->T3_t_TPI);
		}
		else if (padNumber == PT_AP11P76PAD)
		{
			AP11P76PAD *form = (AP11P76PAD*)padForm;

			LOAD_INT("MCC_AP11P76PAD_entries", form->entries);

			for (int i = 0;i < form->entries;i++)
			{
				sprintf(tmpbuf, "MCC_AP11P76PAD_DV[%d]", i);
				LOAD_V3(tmpbuf, form->DV[i]);
				sprintf(tmpbuf, "MCC_AP11P76PAD_purpose[%d]", i);
				LOAD_STRING(tmpbuf, form->purpose[i], 16);
				sprintf(tmpbuf, "MCC_AP11P76PAD_TIG[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->TIG[i]);
			}
		}
		else if (padNumber == PT_AP11LMASCPAD)
		{
			AP11LMASCPAD *form = (AP11LMASCPAD*)padForm;

			LOAD_DOUBLE("MCC_AP11LMASCPAD_CR", form->CR);
			LOAD_INT("MCC_AP11LMASCPAD_DEDA047", form->DEDA047);
			LOAD_INT("MCC_AP11LMASCPAD_DEDA053", form->DEDA053);
			LOAD_DOUBLE("MCC_AP11LMASCPAD_DEDA225_226", form->DEDA225_226);
			LOAD_DOUBLE("MCC_AP11LMASCPAD_DEDA231", form->DEDA231);
			LOAD_DOUBLE("MCC_AP11LMASCPAD_TIG", form->TIG);
			LOAD_DOUBLE("MCC_AP11LMASCPAD_V_hor", form->V_hor);
			LOAD_DOUBLE("MCC_AP11LMASCPAD_V_vert", form->V_vert);
			LOAD_STRING("MCC_AP11LMASCPAD_remarks", form->remarks, 128);
		}
		else if (padNumber == PT_LIFTOFFTIMES)
		{
			LIFTOFFTIMES *form = (LIFTOFFTIMES*)padForm;

			char tmpbuf[64];

			LOAD_INT("MCC_LIFTOFFTIMES_entries", form->entries);
			LOAD_INT("MCC_LIFTOFFTIMES_startdigit", form->startdigit);

			for (int i = 0;i < form->entries;i++)
			{
				sprintf(tmpbuf, "MCC_LIFTOFFTIMES_TIG[%d]", i);
				LOAD_DOUBLE(tmpbuf, form->TIG[i]);
			}
		}
		else if (padNumber == PT_LMACTDATA)
		{
			LMACTDATA *form = (LMACTDATA*)padForm;

			LOAD_DOUBLE("MCC_LMACTDATA_CSMWeight", form->CSMWeight);
			LOAD_DOUBLE("MCC_LMACTDATA_LMWeight", form->LMWeight);
			LOAD_DOUBLE("MCC_LMACTDATA_PitchTrim", form->PitchTrim);
			LOAD_DOUBLE("MCC_LMACTDATA_RollTrim", form->RollTrim);
			LOAD_V3("MCC_LMACTDATA_V42Angles", form->V42Angles);
		}

		LOAD_STRING("MCC_upString", upString, 3072);
		LOAD_INT("MCC_upType", upType);
		LOAD_STRING("MCC_upDescr", upDescr, 1024);
	}

	if (upString[0] != 0) {

		if (upType == 1)
		{
			this->pushCMCUplinkString(upString);
		}
		else if (upType == 2)
		{
			this->pushLGCUplinkString(upString);
		}
	}

	return;
}

// PAD Utility: Format time.
void format_time(char *buf, double time) {
	buf[0] = 0; // Clobber
	int hours, minutes, seconds;
	if (time < 0) { return;  } // don't do that
	hours = (int)(time / 3600);
	minutes = (int)((time / 60) - (hours * 60));
	seconds = (int)((time - (hours * 3600)) - (minutes * 60));
	sprintf(buf, "%03d:%02d:%02d", hours, minutes, seconds);
}

// PAD Utility: Format precise time.
void format_time_prec(char *buf, double time) {
	buf[0] = 0; // Clobber
	int hours, minutes;
	double seconds;
	if (time < 0) { return; } // don't do that
	hours = (int)(time / 3600);
	minutes = (int)((time / 60) - (hours * 60));
	seconds = ((time - (hours * 3600)) - (minutes * 60));
	sprintf(buf, "HRS XXX%03d\nMIN XXXX%02d\nSEC XX%05.2f", hours, minutes, seconds);
}

void SStoHHMMSS(double time, int &hours, int &minutes, double &seconds)
{
	double mins;
	hours = (int)trunc(time / 3600.0);
	mins = fmod(time / 60.0, 60.0);
	minutes = (int)trunc(mins);
	seconds = (mins - minutes) * 60.0;
}

// Draw PAD display
void MCC::drawPad(){
	char buffer[1024];
	char tmpbuf[36];
	char tmpbuf2[36];
	if(padNumber > 0 && padForm == NULL){
		oapiAnnotationSetText(NHpad,"PAD data lost");
		return;
	}
	switch(padNumber){
	case 0:
		// NO PAD (*knifed*)
		oapiAnnotationSetText(NHpad, "No PAD");
		break;
	case PT_AP7BLK:
		{
			AP7BLK * form = (AP7BLK *)padForm;
			sprintf(buffer, "BLOCK DATA\n");

			for (int i = 0;i < 4;i++)
			{
				format_time(tmpbuf, form->GETI[i]);
				format_time(tmpbuf2, form->GETI[i + 4]);
				sprintf(buffer, "%sXX%s XX%s AREA\nXXX%+05.1f XXX%+05.1f LAT\nXX%+06.1f XX%+06.1f LONG\n%s %s GETI\nXXX%4.1f XXX%4.1f DVC\n%s %s WX    \n", buffer, form->Area[i], form->Area[i + 4], form->Lat[i], form->Lat[i + 4], form->Lng[i], form->Lng[i + 4], tmpbuf, tmpbuf2, form->dVC[i], form->dVC[i + 4], form->Wx[i], form->Wx[i + 4]);
			}
			oapiAnnotationSetText(NHpad, buffer);

			//ofstream myfile;
			//myfile.open("MCCDebugging.txt");
			//myfile << buffer;
			//myfile.close();
		}
		break;
	case PT_P27PAD:
		{
			P27PAD * form = (P27PAD *)padForm;
			format_time(tmpbuf, form->GET[0]);
			sprintf(buffer, "P27 UPDATE\nPURP V%d\nGET %s\n304 01 INDEX %d\n", form->Verb[0], tmpbuf, form->Index[0]);
			for (int i = 0;i < 16;i++)
			{
				sprintf(buffer, "%s %02o %05d\n", buffer,i+2, form->Data[0][i]);
			}
			format_time_prec(tmpbuf, form->NavChk);
			sprintf(buffer, "%sNAV CHECK (N34)\n%s\nLAT %+07.2f\nLONG %+07.2f\nALT %+07.1f\n", buffer, tmpbuf, form->lat, form->lng, form->alt);
			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case PT_AP7NAV:
		{
			AP7NAV * form = (AP7NAV *)padForm;
			format_time_prec(tmpbuf, form->NavChk[0]);
			sprintf(buffer, "NAV CHECK\nGET (N34):\n%s\n %+07.2f LAT\n %+07.2f LNG\n %+07.1f ALT\n", tmpbuf, form->lat[0], form->lng[0], form->alt[0]);
			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case PT_AP7MNV:
		{
			int hh, mm;
			double ss;
			AP7MNV * form = (AP7MNV *)padForm;
			format_time_prec(tmpbuf, form->GETI);
			format_time_prec(tmpbuf2, form->NavChk);
			SStoHHMMSS(form->burntime, hh, mm, ss);
			sprintf(buffer,"MANEUVER PAD\nPURPOSE: %s\nGETI (N33):\n%s\ndV X: %+07.1f\ndV Y: %+07.1f\ndV Z: %+07.1f\nHA: %+07.1f\nHP: %+07.1f\nVC: %+07.1f\nWGT: %+06.0f\nPTRM: %+07.2f\n YTRM: %+07.2f\nBT: XXX%d:%02.0f\nSXTS: %02d\n SFT: %+07.2f\nTRN: %+07.3f\nTLAT,LONG\n%s\n%+07.2f LAT\n%+07.2f LONG\n%+07.1f ALT\nXXX%03.0f R\nXXX%03.0f P\nXXX%03.0f Y\nRemarks:\n%s",form->purpose,tmpbuf,form->dV.x,form->dV.y,form->dV.z, form->HA, form->HP, form->Vc, form->Weight, form->pTrim, form->yTrim, mm, ss, form->Star, form->Shaft, form->Trun, tmpbuf2, form->lat, form->lng, form->alt, form->Att.x, form->Att.y, form->Att.z, form->remarks);			
			oapiAnnotationSetText(NHpad,buffer);
		}
		break;
	case PT_AP7TPI:
		{
			AP7TPI * form = (AP7TPI *)padForm;
			format_time_prec(tmpbuf, form->GETI);
			sprintf(buffer, "TERMINAL PHASE INITIATE\nGETI\n%s\n%+07.1f Vgx\n%+07.1f Vgy\n%+07.1f Vgz\n", tmpbuf, form->Vg.x, form->Vg.y, form->Vg.z);
			if (form->Backup_dV.x > 0)
			{
				sprintf(buffer, "%sF%04.1f/%02.0f DVX LOS/BT\n", buffer, abs(form->Backup_dV.x), form->Backup_bT.x);
			}
			else
			{
				sprintf(buffer, "%sA%04.1f/%02.0f DVX LOS/BT\n", buffer, abs(form->Backup_dV.x), form->Backup_bT.x);
			}
			if (form->Backup_dV.y > 0)
			{
				sprintf(buffer, "%sR%04.1f/%02.0f DVY LOS/BT\n", buffer, abs(form->Backup_dV.y), form->Backup_bT.y);
			}
			else
			{
				sprintf(buffer, "%sL%04.1f/%02.0f DVY LOS/BT\n", buffer, abs(form->Backup_dV.y), form->Backup_bT.y);
			}
			if (form->Backup_dV.z > 0)
			{
				sprintf(buffer, "%sD%04.1f/%02.0f DVZ LOS/BT\n", buffer, abs(form->Backup_dV.z), form->Backup_bT.z);
			}
			else
			{
				sprintf(buffer, "%sU%04.1f/%02.0f DVZ LOS/BT\n", buffer, abs(form->Backup_dV.z), form->Backup_bT.z);
			}
			sprintf(buffer, "%sX%04.1f/%3.1f\nX%06.2f R\n%+06.1f RDOT AT TPI\n%06.2f EL MINUS 5 MIN\n%06.2f AZ", buffer, form->dH_TPI, form->dH_Max, form->R, form->Rdot, form->EL, form->AZ);

			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case PT_AP7ENT:
		{
			AP7ENT * form = (AP7ENT *)padForm;
			int hh, mm, hh2, mm2;
			double ss, ss2;
			SStoHHMMSS(form->Ret05[0], hh, mm, ss);
			SStoHHMMSS(form->PB_Ret05[0], hh2, mm2, ss2);
			sprintf(buffer, "ENTRY UPDATE\nPREBURN\nX%s AREA\nXX%+5.1f DV TO\nXXX%03.0f R400K\nXXX%03.0f P400K\nXXX%03.0f Y400K\n%+07.1f RTGO .05G\n%+06.0f VIO .05G\nXX%0d:%02.0f RET .05G\n%+07.2f LAT\n%+07.2f LONG\nPOSTBURN\nXXX%03.0f R400K\n%+07.1f RTGO .05G\n%+06.0f VIO .05G\nXX%0d:%02.0f RET .05G", form->Area[0], form->dVTO[0], form->Att400K[0].x, form->Att400K[0].y, form->Att400K[0].z, form->RTGO[0], form->VIO[0], mm, ss, form->Lat[0], form->Lng[0], form->PB_R400K[0], form->PB_RTGO[0], form->PB_VIO[0], mm2, ss2);
			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case PT_P37PAD:
		{
			P37PAD * form = (P37PAD *)padForm;
			sprintf(buffer, "P37 BLOCK DATA\n");

			for (int i = 0;i < 4;i++)
			{
				format_time(tmpbuf, form->GETI[i]);
				format_time(tmpbuf2, form->GET400K[i]);
				sprintf(buffer, "%s-------------------------\n%s GETI\nX%+04.0f DVT\nX%+5.1f LONG\n%s GET 400K\n", buffer, tmpbuf, form->dVT[i], form->lng[i], tmpbuf2);
			}
			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case PT_AP11MNV:
		{
			AP11MNV * form = (AP11MNV *)padForm;

			int hh, hh2, mm, mm2;
			double ss, ss2;

			sprintf(buffer, "P30 MANEUVER");
			SStoHHMMSS(form->GETI, hh, mm, ss);
			SStoHHMMSS(form->burntime, hh2, mm2, ss2);

			format_time(tmpbuf, form->GET05G);

			sprintf(buffer, "%s\n%s PURPOSE\n%s PROP/GUID\n%+05.0f WT N47\n%+07.2f PTRIM N48\n%+07.2f YTRIM\n%+06d HRS GETI\n%+06d MIN N33\n%+07.2f SEC\n%+07.1f DVX N81\n%+07.1f DVY\n%+07.1f DVZ\nXXX%03.0f R\nXXX%03.0f P\nXXX%03.0f Y\n%+07.1f HA N44\n%+07.1f HP\n%+07.1f DVT\nXXX%d:%02.0f BT\nX%06.1f DVC\nXXXX%02d SXTS\n%+06.1f0 SFT\n%+05.1f00 TRN\nXXX%03d BSS\nXX%+05.1f SPA\nXXX%+04.1f SXP\n%+07.2f LAT N61\n%+07.2f LONG\n%+07.1f RTGO EMS\n%+06.0f VI0\n%s GET 0.05G\n",\
				buffer, form->purpose, form->PropGuid, form->Weight, form->pTrim, form->yTrim, hh, mm, ss, form->dV.x, form->dV.y, form->dV.z, form->Att.x, form->Att.y, form->Att.z, form->HA, form->HP, form->Vt,\
				mm2, ss2, form->Vc, form->Star, form->Shaft, form->Trun, form->BSSStar, form->SPA, form->SXP, form->lat, form->lng, form->RTGO, form->VI0, tmpbuf);
			sprintf(buffer, "%sSET STARS: %s\nRALIGN %03.0f\nPALIGN %03.0f\nYALIGN %03.0f\nRemarks:\n%s", buffer,form->SetStars, form->GDCangles.x, form->GDCangles.y, form->GDCangles.z, form->remarks);
			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case PT_AP11ENT:
		{
			AP11ENT * form = (AP11ENT *)padForm;

			int hh, mm;
			double ss;

			sprintf(buffer, "LUNAR ENTRY");
			format_time(tmpbuf, form->GETHorCheck[0]);
			format_time(tmpbuf2, form->RRT[0]);
			SStoHHMMSS(form->RET05[0], hh, mm, ss);

			sprintf(buffer, "%s\n%s AREA\nXXX%03.0f R 0.05G\nXXX%03.0f P 0.05G\nXXX%03.0f Y 0.05G\n%s GET HOR CHK\nXXX%03.0f P\n%+07.2f LAT N61\n%+07.2f LONG\nXXX%04.1f MAX G\n%+06.0f V400K N60\n%+07.2f y400K\n%+07.1f RTGO EMS\n%+06.0f VI0\n%s RRT\nXX%02d:%02.0f RET 0.05G\nXXX%04.2f DO\nXXXX%02d SXTS\n%+06.1f0 SFT\n%+05.1f00 TRN\nXXX%03d BSS\nXX%+05.1f SPA\nXXX%+04.1f SXP\nXXXX%s LIFT VECTOR\nRemarks:\n%s", \
				buffer, form->Area[0], form->Att05[0].x, form->Att05[0].y, form->Att05[0].z, tmpbuf, form->PitchHorCheck[0], form->Lat[0], form->Lng[0], form->MaxG[0], form->V400K[0], \
				form->Gamma400K[0], form->RTGO[0], form->VIO[0], tmpbuf2, mm, ss, form->DO[0], form->SXTS[0], form->SFT[0], form->TRN[0], form->BSS[0], form->SPA[0], \
				form->SXP[0], form->LiftVector[0], form->remarks[0]);

			oapiAnnotationSetText(NHpad, buffer);
		}
		break;
	case PT_TLIPAD:
		{
			TLIPAD * form = (TLIPAD *)padForm;

			int hh, mm;
			double ss;

			sprintf(buffer, "TLI");
			format_time(tmpbuf, form->TB6P);
			SStoHHMMSS(form->BurnTime, hh, mm, ss);

			sprintf(buffer, "%s\n%s TB6p\nXXX%03.0f R\nXXX%03.0f P TLI\nXXX%03.0f Y\nXXX%d:%02.0f BT\n%07.1f DVC\n%+05.0f VI\nXXX%03.0f R\nXXX%03.0f P SEP\nXXX%03.0f Y\nXXX%03.0f R\nXXX%03.0f P EXTRACTION\nXXX%03.0f Y\nRemarks: %s", buffer, tmpbuf, form->IgnATT.x, form->IgnATT.y, form->IgnATT.z, mm, ss, form->dVC, form->VI, form->SepATT.x, form->SepATT.y, form->SepATT.z, form->ExtATT.x, form->ExtATT.y, form->ExtATT.z, form->remarks);
			oapiAnnotationSetText(NHpad, buffer);
		}
	break;
	case PT_STARCHKPAD:
	{
		STARCHKPAD * form = (STARCHKPAD *)padForm;

		int hh, mm, hh2, mm2;
		double ss, ss2;

		sprintf(buffer, "CSM STAR CHECK UPDATE");
		SStoHHMMSS(form->GET[0], hh, mm, ss);
		SStoHHMMSS(form->TAlign[0], hh2, mm2, ss2);

		sprintf(buffer, "%s\nXX%03d HR GET\nXXX%02d MIN SR\nX%05.2f SEC\n%+06.1f R FDAI\n%+06.1f P\n%+06.1f Y\nXX%03d HR T ALIGN\nXXX%02d MIN\nX%05.2f SEC\n", buffer, hh, mm, ss, form->Att[0].x, form->Att[0].y, form->Att[0].z, hh2, mm2, ss2);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP10MAPUPDATE:
	{
		AP10MAPUPDATE * form = (AP10MAPUPDATE *)padForm;

		int hh, mm;
		double ss;

		sprintf(buffer, "MAP UPDATE REV %d\n", form->Rev);
		SStoHHMMSS(form->LOSGET, hh, mm, ss);
		sprintf(buffer, "%sLOS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
		if (form->type == 0)
		{
			SStoHHMMSS(form->PMGET, hh, mm, ss);
			sprintf(buffer, "%sPM: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->AOSGET, hh, mm, ss);
			sprintf(buffer, "%sAOS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
		}
		else if (form->type == 1)
		{
			SStoHHMMSS(form->SRGET, hh, mm, ss);
			sprintf(buffer, "%sSR: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->PMGET, hh, mm, ss);
			sprintf(buffer, "%sPM: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->AOSGET, hh, mm, ss);
			sprintf(buffer, "%sAOS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->SSGET, hh, mm, ss);
			sprintf(buffer, "%sSS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
		}
		else if (form->type == 2)
		{
			SStoHHMMSS(form->PMGET, hh, mm, ss);
			sprintf(buffer, "%sAOS WITH LOI1: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->AOSGET, hh, mm, ss);
			sprintf(buffer, "%sAOS W/O LOI1: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
		}
		else if (form->type == 3)
		{
			SStoHHMMSS(form->PMGET, hh, mm, ss);
			sprintf(buffer, "%sPM: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->AOSGET, hh, mm, ss);
			sprintf(buffer, "%sAOS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->SSGET, hh, mm, ss);
			sprintf(buffer, "%sSS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->LOSGET2, hh, mm, ss);
			sprintf(buffer, "%sLOS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->SRGET, hh, mm, ss);
			sprintf(buffer, "%sSR: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->PMGET2, hh, mm, ss);
			sprintf(buffer, "%sPM: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->AOSGET2, hh, mm, ss);
			sprintf(buffer, "%sAOS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->SSGET2, hh, mm, ss);
			sprintf(buffer, "%sSS: %d:%02d:%02.0f\n", buffer, hh, mm, ss);
		}

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP11LMARKTRKPAD:
	{
		AP11LMARKTRKPAD * form = (AP11LMARKTRKPAD *)padForm;

		int hh, mm;
		double ss;

		sprintf(buffer, "P22 AUTO OPTICS\n");

		for (int i = 0;i < form->entries;i++)
		{
			sprintf(buffer, "%sLMK ID %s\n", buffer, form->LmkID[i]);
			SStoHHMMSS(form->T1[i], hh, mm, ss);
			sprintf(buffer, "%sT1 %03d:%02d:%02.f (HOR)\n", buffer, hh, mm, ss);
			SStoHHMMSS(form->T2[i], hh, mm, ss);
			sprintf(buffer, "%sT2 %03d:%02d:%02.f (35°)\n", buffer, hh, mm, ss);

			if (form->CRDist[i] > 0)
			{
				sprintf(buffer, "%s%02.f NM North\n", buffer, form->CRDist[i]);
			}
			else
			{
				sprintf(buffer, "%s%02.f NM South\n", buffer, abs(form->CRDist[i]));
			}

			sprintf(buffer, "%sN 89\nLAT %+07.3f\nLONG/2 %+07.3f\nALTITUDE %+07.2f NM\n", buffer, form->Lat[i], form->Lng05[i], form->Alt[i]);
		}

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP10DAPDATA:
	{
		AP10DAPDATA * form = (AP10DAPDATA *)padForm;

		sprintf(buffer, "DAP PAD\n%+06.0f\n%+06.0f\n%+07.2f\n%+07.2f", form->ThisVehicleWeight, form->OtherVehicleWeight, form->PitchTrim, form->YawTrim);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP11LMMNV:
	{
		AP11LMMNV * form = (AP11LMMNV *)padForm;

		int hh, hh2, mm, mm2;
		double ss, ss2;

		sprintf(buffer, "P30 LM MANEUVER");
		SStoHHMMSS(form->GETI, hh, mm, ss);
		SStoHHMMSS(form->burntime, hh2, mm2, ss2);

		sprintf(buffer, "%s\n%s PURPOSE\n%+06d HRS N33\n%+06d MIN TIG\n%+07.2f SEC\n%+07.1f DVX N81\n%+07.1f DVY LOCAL\n%+07.1f DVZ VERT\n"
			"%+07.1f HA N42\n%+07.1f HP\n%+07.1f DVR\nXXX%d:%02.0f BT\nXXX%03.0f R FDAI\nXXX%03.0f P INER\n%+07.1f DVX AGS N86\n%+07.1f DVY AGS\n%+07.1f DVZ AGS\n",
			buffer, form->purpose, hh, mm, ss, form->dV.x, form->dV.y, form->dV.z, form->HA, form->HP, form->dVR, mm2, ss2, form->Att.x, form->Att.y, 
			form->dV_AGS.x,form->dV_AGS.y,form->dV_AGS.z);
		
		if (form->type == 0)
		{
			sprintf(buffer, "%sXXX%03d BSS\nXX%+05.1f SPA\nXXX%+04.1f SXP\n", buffer, form->BSSStar, form->SPA, form->SXP);
		}
		else
		{
			SStoHHMMSS(form->t_CSI, hh, mm, ss);
			SStoHHMMSS(form->t_TPI, hh2, mm2, ss2);

			sprintf(buffer, "%s%+06d HRS N11\n%+06d MIN CSI\n%+07.2f SEC\n%+06d HRS N37\n%+06d MIN TPI\n%+07.2f SEC\n", buffer, hh, mm, ss, hh2, mm2, ss2);
		}

		sprintf(buffer, "%sRemarks:\n%s", buffer, form->remarks);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP10CSI:
	{
		AP10CSI * form = (AP10CSI *)padForm;

		int hh, hh2, mm, mm2;
		double ss, ss2;
		char buffer1[1000], buffer2[100], buffer3[200];

		SStoHHMMSS(form->t_CSI, hh, mm, ss);
		SStoHHMMSS(form->t_TPI, hh2, mm2, ss2);

		sprintf(buffer1, "P32 CSI UPDATE\n%+06d HR N11\n%+06d MIN TIG\n%+07.2f SEC CSI\n%+06d HR N37\n%+06d MIN TIG\n%+07.2f SEC TPI\n%+07.1f DVX LOCAL N81\n%+07.1f DVY VERT\n"
			"XXX%03.0f PLM FDAI\n", hh, mm, ss, hh2, mm2, ss2, form->dV_LVLH.x, form->dV_LVLH.y, form->PLM_FDAI);

		if (form->type == 1)
		{
			sprintf(buffer2, "373 %+07.1f\n275 %+07.1f\n", form->DEDA373, form->DEDA275);
		}
		else
		{
			sprintf(buffer2, "");
		}

		sprintf(buffer3, "%+07.1f DVX AGS N86\n%+07.1f DVY AGS\n%+07.1f DVZ AGS", form->dV_AGS.x, form->dV_AGS.y, form->dV_AGS.z);
		sprintf(buffer, "%s%s%s", buffer1, buffer2, buffer3);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP9AOTSTARPAD:
	{
		AP9AOTSTARPAD * form = (AP9AOTSTARPAD *)padForm;

		int hh, mm;
		double ss;

		sprintf(buffer, "LM AOT STAR OBSERVATION");
		SStoHHMMSS(form->GET, hh, mm, ss);

		sprintf(buffer, "%s\n%03d HR\n%02d MIN\n%02.0f SEC\n%d AOT DETENT\n%02o NAV STAR\n%03.0f R\n%03.0f P\n%03.0f Y", buffer, hh, mm, ss, form->Detent, form->Star, form->CSMAtt.x, form->CSMAtt.y, form->CSMAtt.z);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_TORQANG:
	{
		TORQANG *form = (TORQANG*)padForm;

		sprintf(buffer, "GYRO TORQUING ANGLES\nX %+07.3f\nY %+07.3f\n Z %+07.3f", form->V42Angles.x, form->V42Angles.y, form->V42Angles.z);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP9LMTPI:
	{
		AP9LMTPI * form = (AP9LMTPI *)padForm;
		int hh, mm;
		double ss;

		SStoHHMMSS(form->GETI, hh, mm, ss);

		sprintf(buffer, "TPI UPDATE (P34)\n%+06d HR N37\n%+06d MIN TIG\n%+07.2f SEC TPI\n%+07.1f DVX N81\n%+07.1f DVY LOCAL\n%+07.1f DVZ VERT\n%+07.1f DVR N42\n"
			"XXX%03.0f RLM FDAI N18\nXXX%03.0f PLM INER\n%+07.2f R TPI N54\n%+07.1f RDOT TPI\n%+07.1f F/A (+/-) N59\n%+07.1f L/R (-/+) DV\n%+07.1f U/D (-/+) LOS", 
			hh, mm, ss, form->Vg.x, form->Vg.y, form->Vg.z, form->dVR, form->Att.x, form->Att.y, form->R, form->Rdot, form->Backup_dV.x, form->Backup_dV.y, form->Backup_dV.z);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP9LMCDH:
	{
		AP9LMCDH * form = (AP9LMCDH *)padForm;
		int hh, mm;
		double ss;

		SStoHHMMSS(form->GETI, hh, mm, ss);

		sprintf(buffer, "CDH UPDATE (P33)\n%+06d HR N31\n%+06d MIN TIG\n%+07.2f SEC CDH\n%+07.1f DVX N81\n%+07.1f DVY LOCAL\n%+07.1f DVZ VERT\n"
			"XXX%03.0f PLM INER\n%+07.1f DVX N86\n%+07.1f DVZ AGS",
			hh, mm, ss, form->Vg.x, form->Vg.y, form->Vg.z, form->Pitch, form->Vg_AGS.x, form->Vg_AGS.z);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_S065UPDATE:
	{
		S065UPDATE * form = (S065UPDATE *)padForm;
		int hh[4], mm[4], hh2[4], mm2[4];
		double ss[4], ss2[4];

		sprintf(buffer, "S065 UPDATE");

		for (int i = 0;i < 2;i++)
		{
			SStoHHMMSS(form->GETStart[2 * i], hh[2 * i], mm[2 * i], ss[2 * i]);
			SStoHHMMSS(form->GETStart[2 * i + 1], hh[2 * i + 1], mm[2 * i + 1], ss[2 * i + 1]);
			SStoHHMMSS(form->TAlign[2 * i], hh2[2 * i], mm2[2 * i], ss2[2 * i]);
			SStoHHMMSS(form->TAlign[2 * i + 1], hh2[2 * i + 1], mm2[2 * i + 1], ss2[2 * i + 1]);

			sprintf(buffer, "%s\n%s %s SITE (OR AREA)\n%06.2f %06.2f R FDAI\n%06.2f %06.2f P\n%06.2f %06.2f Y\nXX%03d XX%03d GET START\n"
				"XXX%02d XXX%02d MIN (5 MIN PRIOR TO)\nXXX%02.0f XXX%02.0f SEC (FIRST EXPOSURE)\nXX%03d XX%03d T ALIGN\nXXX%02d XXX%02d MIN (IF REQ)\n"
				"XXX%02.0f XXX%02.0f SEC\nXXX%02.0f XXX%02.0f EXPOSURE INTER-SEC\nXX%03d XX%03d NUMBER OF EXPOSURES\n",
				buffer, form->Area[2 * i], form->Area[2 * i + 1], form->FDAIAngles[2 * i].x, form->FDAIAngles[2 * i + 1].x, form->FDAIAngles[2 * i].y, 
				form->FDAIAngles[2 * i + 1].y, form->FDAIAngles[2 * i].z, form->FDAIAngles[2 * i + 1].z, hh[2 * i], hh[2 * i + 1], mm[2 * i], mm[2 * i + 1], 
				ss[2 * i], ss[2 * i + 1], hh2[2 * i], hh2[2 * i + 1], mm2[2 * i], mm2[2 * i + 1], ss2[2 * i], ss2[2 * i + 1],
				form->ExposureInterval[2 * i], form->ExposureInterval[2 * i + 1], form->ExposureNum[2 * i], form->ExposureNum[2 * i + 1]);

			if (form->OrbRate[2 * i])
			{
				sprintf(buffer, "%sORB RATE X ATTITUDE CONTROL", buffer);
			}
			else
			{
				sprintf(buffer, "%sINERTIAL X ATTITUDE CONTROL", buffer);
			}
		}

		oapiAnnotationSetText(NHpad, buffer);

		//ofstream myfile;
		//myfile.open("MCCDebugging.txt");
		//myfile << buffer;
		//myfile.close();
	}
	break;
	case PT_AP11AGSACT:
	{
		AP11AGSACT *form = (AP11AGSACT*)padForm;

		int hh, mm;
		double ss;

		SStoHHMMSS(form->KFactor, hh, mm, ss);

		sprintf(buffer, "AGS ACTIVATION\n%d:%02d:%05.2f GET\n224 %+06d\n225 %+06d\n226 %+06d\n227 %+06d", hh, mm, ss, form->DEDA224, form->DEDA225, form->DEDA226, form->DEDA227);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP11PDIPAD:
	{
		AP11PDIPAD *form = (AP11PDIPAD*)padForm;

		int hh[2], mm[2];
		double ss[2];

		SStoHHMMSS(form->GETI, hh[0], mm[0], ss[0]);
		SStoHHMMSS(form->t_go, hh[1], mm[1], ss[1]);

		sprintf(buffer, "PDI PAD\n%+06d HRS TIG\n%+06d MIN PDI\n%+07.2f SEC\nXX%02d:%02.0f TGO N61\n%+07.1f CROSSRANGE\nXXX%03.0f R FDAI\nXXX%03.0f P AT TIG\n"
			"XXX%03.0f Y\n%+06.0f DEDA 231 IF RQD", hh[0], mm[0], ss[0], mm[1], ss[1], form->CR, form->Att.x, form->Att.y, form->Att.z, form->DEDA231);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_PDIABORTPAD:
	{
		PDIABORTPAD *form = (PDIABORTPAD*)padForm;

		int hh[3], mm[3];
		double ss[3];

		SStoHHMMSS(form->T_TPI_Pre10Min, hh[0], mm[0], ss[0]);
		SStoHHMMSS(form->T_Phasing, hh[1], mm[1], ss[1]);
		SStoHHMMSS(form->T_TPI_Post10Min, hh[2], mm[2], ss[2]);

		if (form->type == 0)
		{
			sprintf(buffer, "PDI ABORT <10 MIN\n%+06d HRS N37\n%+06d MIN TPI\n%+07.2f SEC\n%+06d HRS\n%+06d MIN\n%+07.2f SEC PHASING TIG\n"
				"%+06d HRS N37\n%+06d MIN TPI\n%+07.2f SEC", hh[0], mm[0], ss[0], hh[1], mm[1], ss[1], hh[2], mm[2], ss[2]);
		}
		else
		{
			sprintf(buffer, "CSM RESCUE PAD\nPHAS 33 %d:%02d:%05.2f\nTPI (PDI<10) 37 %d:%02d:%05.2f\nTPI (PDI>10) 37 %d:%02d:%05.2f",
				hh[1], mm[1], ss[1], hh[0], mm[0], ss[0], hh[2], mm[2], ss[2]);
		}

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP11LUNSURFPAD:
	{
		AP11LunarSurfaceDataCard *form = (AP11LunarSurfaceDataCard*)padForm;

		int hh[9], mm[9];
		double ss[9];

		SStoHHMMSS(form->T2_TIG, hh[0], mm[0], ss[0]);
		SStoHHMMSS(form->T2_t_Phasing, hh[1], mm[1], ss[1]);
		SStoHHMMSS(form->T2_t_CSI1, hh[2], mm[2], ss[2]);
		SStoHHMMSS(form->T2_t_TPI, hh[3], mm[3], ss[3]);
		SStoHHMMSS(form->T3_TIG, hh[4], mm[4], ss[4]);
		SStoHHMMSS(form->T3_t_Period, hh[5], mm[5], ss[5]);
		SStoHHMMSS(form->T3_t_PPlusDT, hh[6], mm[6], ss[6]);
		SStoHHMMSS(form->T3_t_CSI, hh[7], mm[7], ss[7]);
		SStoHHMMSS(form->T3_t_TPI, hh[8], mm[8], ss[8]);

		sprintf(buffer, "T2 ABORT\n%+06d HRS T2\n%+06d MIN TIG\n%+07.2f SEC\n%+06d HRS N33\n%+06d MIN PHASING\n%+07.2f SEC TIG\n"
			"%+06d HRS N11\n%+06d MIN CSI1\n%+07.2f SEC\n%+06d HRS N37\n%+06d MIN TPI\n%+07.2f SEC\n"
			"T3 ABORT\n%+06d HRS T3\n%+06d MIN TIG\n%+07.2f SEC\n%+06d HRS CSM\n%+06d MIN PERIOD\n%+07.2f SEC\n"
			"%+06d HRS\n%+06d MIN P+DT\n%+07.2f SEC\n%+06d HRS N11\n%+06d MIN CSI TIG\n%+07.2f SEC\n%+06d HRS N37\n%+06d MIN TPI\n%+07.2f SEC",
			hh[0], mm[0], ss[0], hh[1], mm[1], ss[1], hh[2], mm[2], ss[2], hh[3], mm[3], ss[3], hh[4], mm[4], ss[4], hh[5], mm[5], ss[5],
			hh[6], mm[6], ss[6], hh[7], mm[7], ss[7], hh[8], mm[8], ss[8]);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP11P76PAD:
	{
		AP11P76PAD *form = (AP11P76PAD*)padForm;

		int hh, mm;
		double ss;

		sprintf(buffer, "P76 UPDATE PAD\n");

		for (int i = 0;i < form->entries;i++)
		{
			SStoHHMMSS(form->TIG[i], hh, mm, ss);

			sprintf(buffer, "%s%s PURPOSE\n%+06d HRS N33\n%+06d MIN TIG\n%+07.2f SEC\n%+07.1f DVX N84\n%+07.1f DVY\n%+07.1f DVZ\n", 
				buffer, form->purpose, hh, mm, ss, form->DV[i].x, form->DV[i].y, form->DV[i].z);
		}

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_AP11LMASCPAD:
	{
		AP11LMASCPAD *form = (AP11LMASCPAD*)padForm;

		int hh, mm;
		double ss;

		SStoHHMMSS(form->TIG, hh, mm, ss);

		sprintf(buffer, "LM ASCENT PAD\n%+06d HRS\n%+06d MIN TIG\n%+07.2f SEC\n%+07.1f V (HOR)\n%+07.1f V (VERT) N76\n%+07.1f CROSSRANGE\n"
			"%+06d DEDA 047\n%+06d DEDA 053\n%+06.0f DEDA 225/226\n%+06.0f DEDA 231\nRemarks: %s", hh, mm, ss, form->V_hor, form->V_vert, form->CR,
			form->DEDA047, form->DEDA053, form->DEDA225_226, form->DEDA231, form->remarks);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_LIFTOFFTIMES:
	{
		LIFTOFFTIMES *form = (LIFTOFFTIMES*)padForm;

		sprintf(buffer, "LIFTOFF TIMES\n");

		for (int i = 0;i < form->entries;i++)
		{
			format_time(tmpbuf, form->TIG[i]);
			sprintf(buffer, "%sT%d %s\n", buffer, form->startdigit + i, tmpbuf);
		}

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_LMACTDATA:
	{
		LMACTDATA *form = (LMACTDATA*)padForm;

		sprintf(buffer, "DAP PAD\n%+06.0f\n%+06.0f\n%+07.2f\n%+07.2f\nGYRO TORQUING ANGLES\nX %+07.3f\nY %+07.3f\n Z %+07.3f", 
			form->LMWeight, form->CSMWeight, form->PitchTrim, form->RollTrim, form->V42Angles.x, form->V42Angles.y, form->V42Angles.z);

		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	case PT_GENERIC:
	{
		GENERICPAD * form = (GENERICPAD *)padForm;
		sprintf(buffer, form->paddata);
		oapiAnnotationSetText(NHpad, buffer);
	}
	break;
	default:
		sprintf(buffer,"Unknown padNumber %d",padNumber);
		oapiAnnotationSetText(NHpad,buffer);
		break;
	}
	padState = 1;
}

// Allocate PAD
void MCC::allocPad(int Number){
	char msg[256];
	// Ensure PAD display off and disabled
	if(padState == 1){ 
		oapiAnnotationSetText(NHpad,""); // Clear PAD		
	}
	padState = -1;
	padNumber = Number;
	// Ensure pointer freed
	if(padForm != NULL){
		free(padForm);
	}
	switch(padNumber){
	case PT_AP7BLK: // AP7BLK
		padForm = calloc(1,sizeof(AP7BLK));
		break;
	case PT_P27PAD: // P27PAD
		padForm = calloc(1,sizeof(P27PAD));
		break;
	case PT_AP7NAV: // AP7NAV
		padForm = calloc(1,sizeof(AP7NAV));
		break;
	case PT_AP7MNV: // AP7MNV
		padForm = calloc(1,sizeof(AP7MNV));
		break;
	case PT_AP7TPI: // AP7TPI
		padForm = calloc(1,sizeof(AP7TPI));
		break;
	case PT_AP7ENT: // AP7ENT
		padForm = calloc(1,sizeof(AP7ENT));
		break;
	case PT_P37PAD: // P37PAD
		padForm = calloc(1, sizeof(P37PAD));
		break;
	case PT_AP11MNV: // AP11MNV
		padForm = calloc(1, sizeof(AP11MNV));
		break;
	case PT_AP11ENT: // AP11ENT
		padForm = calloc(1, sizeof(AP11ENT));
		break;
	case PT_TLIPAD: // TLIPAD
		padForm = calloc(1, sizeof(TLIPAD));
		break;
	case PT_STARCHKPAD: // STARCHKPAD
		padForm = calloc(1, sizeof(STARCHKPAD));
		break;
	case PT_AP10MAPUPDATE: // AP10MAPUPDATE
		padForm = calloc(1, sizeof(AP10MAPUPDATE));
		break;
	case PT_AP11LMARKTRKPAD: // AP11LMARKTRKPAD
		padForm = calloc(1, sizeof(AP11LMARKTRKPAD));
		break;
	case PT_AP10DAPDATA: // AP10DAPDATA
		padForm = calloc(1, sizeof(AP10DAPDATA));
		break;
	case PT_AP11LMMNV: // AP11LMMNV
		padForm = calloc(1, sizeof(AP11LMMNV));
		break;
	case PT_AP10CSI: // AP10CSI
		padForm = calloc(1, sizeof(AP10CSI));
		break;
	case PT_AP9AOTSTARPAD: // AP9AOTSTARPAD
		padForm = calloc(1, sizeof(AP9AOTSTARPAD));
		break;
	case PT_TORQANG: // TORQANG
		padForm = calloc(1, sizeof(TORQANG));
		break;
	case PT_AP9LMTPI: // AP9LMTPI
		padForm = calloc(1, sizeof(AP9LMTPI));
		break;
	case PT_AP9LMCDH: // AP9LMCDH
		padForm = calloc(1, sizeof(AP9LMCDH));
		break;
	case PT_S065UPDATE: // S065UPDATE
		padForm = calloc(1, sizeof(S065UPDATE));
		break;
	case PT_AP11AGSACT: // AP11AGSACT
		padForm = calloc(1, sizeof(AP11AGSACT));
		break;
	case PT_AP11PDIPAD: // AP11PDIPAD
		padForm = calloc(1, sizeof(AP11PDIPAD));
		break;
	case PT_PDIABORTPAD: // PDIABORTPAD
		padForm = calloc(1, sizeof(PDIABORTPAD));
		break;
	case PT_AP11LUNSURFPAD: // AP11T2ABORTPAD
		padForm = calloc(1, sizeof(AP11LunarSurfaceDataCard));
		break;
	case PT_AP11P76PAD: // AP11P76PAD
		padForm = calloc(1, sizeof(AP11P76PAD));
		break;
	case PT_AP11LMASCPAD: // AP11LMASCPAD
		padForm = calloc(1, sizeof(AP11LMASCPAD));
		break;
	case PT_LIFTOFFTIMES: // LIFTOFFTIMES
		padForm = calloc(1, sizeof(LIFTOFFTIMES));
		break;
	case PT_LMACTDATA: // LMACTDATA
		padForm = calloc(1, sizeof(LMACTDATA));
		break;
	case PT_GENERIC: // GENERICPAD
		padForm = calloc(1, sizeof(GENERICPAD));
		break;

	default:
		padForm = NULL;
		return;
	}
	if(padForm == NULL){
		sprintf(msg,"Could not calloc() PAD: %s",strerror(errno));
		addMessage(msg);
	}
	return;
}

// Free PAD
void MCC::freePad(){
	if(padState == 1){ 
		oapiAnnotationSetText(NHpad,""); // Clear PAD		
	}
	padState = -1;
	padNumber = 0;
	if(padForm == NULL){ return; }
	free(padForm);
	padForm = NULL; // ensure clobber
}

// Keypress handler
void MCC::keyDown(DWORD key){
	char buf[MAX_MSGSIZE];
	char menubuf[300];
	
	switch(key){
		case OAPI_KEY_TAB:
			if(menuState == 0){
				// Build buf
				buf[0] = 0;
				if (PCOption_Enabled == true) { sprintf(buf, "2: %s\n", PCOption_Text); }
				if (NCOption_Enabled == true) { sprintf(buf, "%s3: %s\n", buf, NCOption_Text); }
				sprintf(menubuf, "CAPCOM MENU\n1: Voice Check\n%s4: Toggle Auto PAD\n5: Hide/Show PAD\n6: Redisplay Messages\n8: Request Abort\n9: Debug Options", buf);
				oapiAnnotationSetText(NHmenu,menubuf); // Present menu
				// 2: Toggle Ground Trk\n3: Toggle Mission Trk\n
				menuState = 1;
			}else{
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_1:
			if(menuState == 1){
				sprintf(buf,"Voice Check");
				addMessage(buf);
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				if (GT_Enabled == false) {
					GT_Enabled = true;
					sprintf(buf, "Ground Tracking Enabled");
				}
				else {
					GT_Enabled = false;
					sprintf(buf, "Ground Tracking Disabled");
				}
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 3) {
				sprintf(buf, "Abort confirmed!");
				addMessage(buf);
				initiateAbort();
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_2:
			if (menuState == 1 && PCOption_Enabled == true) {
				// Positive Completion
				SubState += 2;
				SubStateTime = 0;
				addMessage(PCOption_Text);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				if (MT_Enabled == false) {
					MT_Enabled = true; GT_Enabled = true;
					sprintf(buf, "Mission and Ground Tracking Enabled");
				}
				else {
					MT_Enabled = false;
					sprintf(buf, "Mission Tracking Disabled");
				}
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 3) {
				sprintf(buf, "Abort rejected!");
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_3:
			if (menuState == 1 && NCOption_Enabled == true) {
				// Negative Completion
				SubState++;
				SubStateTime = 0;
				addMessage(NCOption_Text);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Report State
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				sprintf(buf, "MissionPhase %d EarthRev %d MoonRev %d MoonRevTime %f", MissionPhase, EarthRev, MoonRev, MoonRevTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_4:
			if(menuState == 1){
				if(padAutoShow == false){
					padAutoShow = true;
					sprintf(buf,"PAD Auto Show Enabled");
				}else{
					padAutoShow = false;
					sprintf(buf,"PAD Auto Show Disabled");
				}
				addMessage(buf);			
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Increment State
				setState(MissionState + 1);
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_5:
			if(menuState == 1){
				if(padState == -1 || padNumber == 0){
					addMessage("No PAD available");
				}else{
					if(padState == 1){
						oapiAnnotationSetText(NHpad,""); // Clear PAD
						padState = 0;
					}else{
						drawPad();						
					}
				}
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Decrement State
				setState(MissionState - 1);
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_6:
			if(menuState == 1){
				redisplayMessages();
				oapiAnnotationSetText(NHmenu,""); // Clear menu
				menuState = 0;
			}
			if (menuState == 2) {
				// Increment SubState				
				setSubState(SubState + 1);
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_7:
			/*
			if (menuState == 1) {
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			*/
			if (menuState == 2) {
				// Decrement SubState				
				setSubState(SubState - 1);
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_8:
			
			if (menuState == 1) {
				oapiAnnotationSetText(NHmenu, "ABORT MENU\n1: Confirm Abort\n2: Reject Abort"); // Abort menu
				menuState = 3;
			}
			if (menuState == 2) {
				// Reset State				
				SubState = 0;
				StateTime = SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			break;
		case OAPI_KEY_9:
			if (menuState == 2) {
				// Reset SubState				
				SubStateTime = 0;
				sprintf(buf, "MissionState %d SubState %d StateTime %f SubStateTime %f", MissionState, SubState, StateTime, SubStateTime);
				addMessage(buf);
				oapiAnnotationSetText(NHmenu, ""); // Clear menu
				menuState = 0;
			}
			if (menuState == 1) {
				oapiAnnotationSetText(NHmenu, "DEBUG MENU\n1: Toggle GT\n2: Toggle MT\n3: Report State\n4: Inc State\n5: Dec State\n6: Inc SubState\n7: Dec SubState\n8: Reset State\n9: Reset SubState"); // Debug menu
				menuState = 2;
			}
			break;

	}
}

void MCC::UpdateMacro(int type, int padtype, bool condition, int updatenumber, int nextupdate, bool altcriterium, bool altcondition, int altnextupdate)
{
	if (type == UTP_PADONLY) //Display PAD without uplink
	{
		switch (SubState) {
		case 0:
			allocPad(padtype); // Allocate PAD
			if (padForm != NULL) {
				// If success
				startSubthread(updatenumber, type); // Start subthread to fill PAD
			}
			else {
				// ERROR STATE
			}
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1 && padState > -1) {
				if (scrubbed)
				{
					if (upMessage[0] != 0)
					{
						addMessage(upMessage);
					}
					freePad();
					scrubbed = false;
					setSubState(2);
				}
				else
				{
					addMessage("You can has PAD");
					if (padAutoShow == true && padState == 0) { drawPad(); }
					setSubState(2);
				}
			}
			break;
		case 2: // Await burn
			if (altcriterium)
			{
				if (altcondition)
				{
					SlowIfDesired();
					setState(altnextupdate);
				}
			}
			else if (condition)
			{
				SlowIfDesired();
				setState(nextupdate);
			}
			break;
		}
	}
	else if (type == UTP_PADWITHCMCUPLINK)//PAD with uplinks
	{
		switch (SubState) {
		case 0:
			allocPad(padtype); // Allocate PAD
			if (padForm != NULL) {
				// If success
				startSubthread(updatenumber, type); // Start subthread to fill PAD
			}
			else {
				// ERROR STATE
			}
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1 && subThreadStatus == 0) {
				if (scrubbed)
				{
					if (upMessage[0] != 0)
					{
						addMessage(upMessage);
					}
					freePad();
					scrubbed = false;
				}
				else
				{
					addMessage("You can has PAD");
					if (padAutoShow == true && padState == 0) { drawPad(); }
				}

				//Do we have an uplink?
				if (upString[0] != 0)
				{
					// We really should test for P00 and proceed since that would be visible to the ground.
					addMessage("CSM: Ready for uplink?");
					sprintf(PCOption_Text, "Ready for uplink");
					PCOption_Enabled = true;
					setSubState(2);
				}
				else
				{
					if (upDescr[0] != 0)
					{
						addMessage(upDescr);
					}
					setSubState(6);
				}
			}
			break;
		case 2: // Awaiting user response
			if (SubStateTime > 60.0)
			{
				addMessage("CSM: Ready for uplink?");
				setSubState(2);
			}
		case 3: // Negative response / not ready for uplink
			break;
		case 4: // Ready for uplink
			if (SubStateTime > 1 && subThreadStatus == 0) {
				// The uplink should also be ready, so flush the uplink buffer to the CMC
				this->CM_uplink_buffer();
				// uplink_size = 0; // Reset
				PCOption_Enabled = false; // No longer needed
				if (upDescr[0] != 0)
				{
					addMessage(upDescr);
				}
				setSubState(5);
			}
			break;
		case 5: // Await uplink completion
			if (cm->pcm.mcc_size == 0) {
				addMessage("Uplink completed!");
				NCOption_Enabled = true;
				sprintf(NCOption_Text, "Repeat uplink");
				setSubState(6);
			}
			break;
		case 6: // Await burn
			if (altcriterium)
			{
				if (altcondition)
				{
					SlowIfDesired();
					setState(altnextupdate);
				}
			}
			else if (condition)
			{
				SlowIfDesired();
				setState(nextupdate);
			}
			break;
		case 7: //Repeat uplink
		{
			NCOption_Enabled = false;
			setSubState(0);
		}
		break;
		}
	}
	else if (type == UTP_CMCUPLINKONLY)//CMC uplink without PAD
	{
		switch (SubState) {
		case 0:
			startSubthread(updatenumber, type); // Start subthread to fill PAD
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1 && subThreadStatus == 0) {
				// Completed. We really should test for P00 and proceed since that would be visible to the ground.
				addMessage("CSM: Ready for uplink?");
				sprintf(PCOption_Text, "Ready for uplink");
				PCOption_Enabled = true;
				setSubState(2);
			}
			break;
		case 2: // Awaiting user response
			if (SubStateTime > 60.0)
			{
				addMessage("CSM: Ready for uplink?");
				setSubState(2);
			}
		case 3: // Negative response / not ready for uplink
			break;
		case 4: // Ready for uplink
			if (SubStateTime > 1) {
				// The uplink should also be ready, so flush the uplink buffer to the CMC
				this->CM_uplink_buffer();
				// uplink_size = 0; // Reset
				PCOption_Enabled = false; // No longer needed
				if (upDescr[0] != 0)
				{
					addMessage(upDescr);
				}
				setSubState(5);
			}
			break;
		case 5: // Await uplink completion
			if (cm->pcm.mcc_size == 0) {
				addMessage("Uplink completed!");
				NCOption_Enabled = true;
				sprintf(NCOption_Text, "Repeat uplink");
				setSubState(6);
			}
			break;
		case 6: // Await burn
			if (altcriterium)
			{
				if (altcondition)
				{
					SlowIfDesired();
					setState(altnextupdate);
				}
			}
			else if (condition)
			{
				SlowIfDesired();
				setState(nextupdate);
			}
			break;
		case 7: //Repeat uplink
		{
			NCOption_Enabled = false;
			setSubState(0);
		}
		break;
		}
	}
	else if (type == UTP_PADWITHLGCUPLINK)//PAD with LGC uplinks
	{
		switch (SubState) {
		case 0:
			allocPad(padtype); // Allocate PAD
			if (padForm != NULL) {
				// If success
				startSubthread(updatenumber, type); // Start subthread to fill PAD
			}
			else {
				// ERROR STATE
			}
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1 && subThreadStatus == 0) {
				if (scrubbed)
				{
					if (upMessage[0] != 0)
					{
						addMessage(upMessage);
					}
					freePad();
					scrubbed = false;
				}
				else
				{
					addMessage("You can has PAD");
					if (padAutoShow == true && padState == 0) { drawPad(); }
				}

				//Do we have an uplink?
				if (upString[0] != 0)
				{
					// We really should test for P00 and proceed since that would be visible to the ground.
					addMessage("LM: Ready for uplink?");
					sprintf(PCOption_Text, "Ready for uplink");
					PCOption_Enabled = true;
					setSubState(2);
				}
				else
				{
					if (upDescr[0] != 0)
					{
						addMessage(upDescr);
					}
					setSubState(6);
				}
			}
			break;
		case 2: // Awaiting user response
			if (SubStateTime > 60.0)
			{
				addMessage("LM: Ready for uplink?");
				setSubState(2);
			}
		case 3: // Negative response / not ready for uplink
			break;
		case 4: // Ready for uplink
			if (SubStateTime > 1 && subThreadStatus == 0) {
				// The uplink should also be ready, so flush the uplink buffer to the CMC
				this->LM_uplink_buffer();
				// uplink_size = 0; // Reset
				PCOption_Enabled = false; // No longer needed
				if (upDescr[0] != 0)
				{
					addMessage(upDescr);
				}
				setSubState(5);
			}
			break;
		case 5: // Await uplink completion
			if (lm->VHF.mcc_size == 0) {
				addMessage("Uplink completed!");
				NCOption_Enabled = true;
				sprintf(NCOption_Text, "Repeat uplink");
				setSubState(6);
			}
			break;
		case 6: // Await burn
			if (altcriterium)
			{
				if (altcondition)
				{
					SlowIfDesired();
					setState(altnextupdate);
				}
			}
			else if (condition)
			{
				SlowIfDesired();
				setState(nextupdate);
			}
			break;
		case 7: //Repeat uplink
		{
			NCOption_Enabled = false;
			setSubState(0);
		}
		break;
		}
	}
	else if (type == UTP_LGCUPLINKONLY)//LGC uplink without PAD
	{
		switch (SubState) {
		case 0:
			startSubthread(updatenumber, type); // Start subthread to fill PAD
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1 && subThreadStatus == 0) {
				// Completed. We really should test for P00 and proceed since that would be visible to the ground.
				addMessage("LM: Ready for uplink?");
				sprintf(PCOption_Text, "Ready for uplink");
				PCOption_Enabled = true;
				setSubState(2);
			}
			break;
		case 2: // Awaiting user response
			if (SubStateTime > 60.0)
			{
				addMessage("LM: Ready for uplink?");
				setSubState(2);
			}
		case 3: // Negative response / not ready for uplink
			break;
		case 4: // Ready for uplink
			if (SubStateTime > 1) {
				// The uplink should also be ready, so flush the uplink buffer to the CMC
				this->LM_uplink_buffer();
				// uplink_size = 0; // Reset
				PCOption_Enabled = false; // No longer needed
				if (upDescr[0] != 0)
				{
					addMessage(upDescr);
				}
				setSubState(5);
			}
			break;
		case 5: // Await uplink completion
			if (lm->VHF.mcc_size == 0) {
				addMessage("Uplink completed!");
				NCOption_Enabled = true;
				sprintf(NCOption_Text, "Repeat uplink");
				setSubState(6);
			}
			break;
		case 6: // Await burn
			if (altcriterium)
			{
				if (altcondition)
				{
					SlowIfDesired();
					setState(altnextupdate);
				}
			}
			else if (condition)
			{
				SlowIfDesired();
				setState(nextupdate);
			}
			break;
		case 7: //Repeat uplink
		{
			NCOption_Enabled = false;
			setSubState(0);
		}
		break;
		}
	}
	else if (type == UTP_LGCUPLINKDIRECT) //Uplink without dialogue
	{
		switch (SubState) {
		case 0:
			startSubthread(updatenumber, type); // Start subthread to fill PAD
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1) {
				// Completed. We really should test for P00 and proceed since that would be visible to the ground.
				setSubState(2);
			}
			break;
		case 2: // Ready for uplink
			if (SubStateTime > 1) {
				// The uplink should also be ready, so flush the uplink buffer to the LGC
				this->LM_uplink_buffer();
				if (upDescr[0] != 0)
				{
					addMessage(upDescr);
				}
				setSubState(3);
			}
			break;
		case 3: // Await uplink completion
			if (lm->VHF.mcc_size == 0) {
				addMessage("Uplink completed!");
				setSubState(4);
			}
			break;
		case 4: // Await next update
			if (condition)
			{
				SlowIfDesired();
				setState(nextupdate);
			}
			break;
		}
	}
	else if (type == UTP_NONE) //calculation with an optional message, without uplink
	{
		switch (SubState) {
		case 0:
			startSubthread(updatenumber, type); // Start subthread
			setSubState(1);
			// FALL INTO
		case 1: // Await pad read-up time (however long it took to compute it and give it to capcom)
			if (SubStateTime > 1 && subThreadStatus == 0) {
				if (upMessage[0] != 0)
				{
					addMessage(upMessage);
				}
				setSubState(2);
			}
			break;
		case 2: // Await burn
			if (altcriterium)
			{
				if (altcondition)
				{
					SlowIfDesired();
					setState(altnextupdate);
				}
			}
			else if (condition)
			{
				SlowIfDesired();
				setState(nextupdate);
			}
			break;
		}
	}
}

void MCC::subThreadMacro(int type, int updatenumber)
{
	// Clobber string
	upString[0] = 0;
	upDescr[0] = 0;
	upMessage[0] = 0;
	uplink_size = 0;

	if (type == UTP_PADONLY)
	{
		// Ask RTCC for numbers
		rtcc->Calculation(MissionType, updatenumber, padForm, upString, upDescr, upMessage);
		// Done filling form, OK to show
		padState = 0;
	}
	else if (type == UTP_PADWITHCMCUPLINK)
	{
		// Ask RTCC for numbers
		// Do math
		scrubbed = rtcc->Calculation(MissionType, updatenumber, padForm, upString, upDescr, upMessage);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
			upType = 1;
		}
		// Done filling form, OK to show
		padState = 0;
	}
	else if (type == UTP_CMCUPLINKONLY)
	{
		// Ask RTCC for numbers
		// Do math
		scrubbed = rtcc->Calculation(MissionType, updatenumber, padForm, upString, upDescr, upMessage);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushCMCUplinkString(upString);
			upType = 1;
		}
	}
	else if (type == UTP_PADWITHLGCUPLINK)
	{
		// Ask RTCC for numbers
		// Do math
		scrubbed = rtcc->Calculation(MissionType, updatenumber, padForm, upString, upDescr, upMessage);
		// Give resulting uplink string to LGC
		if (upString[0] != 0) {
			this->pushLGCUplinkString(upString);
			upType = 2;
		}
		// Done filling form, OK to show
		padState = 0;
	}
	else if (type == UTP_LGCUPLINKONLY)
	{
		// Ask RTCC for numbers
		// Do math
		scrubbed = rtcc->Calculation(MissionType, updatenumber, padForm, upString, upDescr, upMessage);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushLGCUplinkString(upString);
			upType = 2;
		}
	}
	else if (type == UTP_LGCUPLINKDIRECT)
	{
		// Ask RTCC for numbers
		// Do math
		scrubbed = rtcc->Calculation(MissionType, updatenumber, padForm, upString, upDescr, upMessage);
		// Give resulting uplink string to CMC
		if (upString[0] != 0) {
			this->pushLGCUplinkString(upString);
			upType = 2;
		}
	}
	else if (type == UTP_NONE)
	{
		scrubbed = rtcc->Calculation(MissionType, subThreadMode, padForm, upString, upDescr, upMessage);
	}
}

void MCC::initiateAbort()
{
	if (MissionPhase == MMST_EARTH_ORBIT)
	{
		AbortMode = 5;
		if (MissionType == MTP_C)
		{
			setState(MST_C_ABORT);
		}
		else if (MissionType == MTP_C_PRIME)
		{
			setState(MST_CP_ABORT_ORBIT);
		}
		else if (MissionType == MTP_F)
		{
			setState(MST_F_ABORT_ORBIT);
		}
		else if (MissionType == MTP_G)
		{
			setState(MST_G_ABORT_ORBIT);
		}
	}
	else if (MissionPhase == MMST_TL_COAST)
	{
		AbortMode = 6;
		if (MissionType == MTP_C_PRIME)
		{
			setState(MST_CP_ABORT);
		}
	}
	else if (MissionPhase == MMST_LUNAR_ORBIT)
	{
		AbortMode = 7;
		if (MissionType == MTP_C_PRIME)
		{
			setState(MST_CP_ABORT);
		}
	}
	else if (MissionPhase == MMST_TE_COAST)
	{
		AbortMode = 8;
		if (MissionType == MTP_C_PRIME)
		{
			setState(MST_CP_ABORT);
		}
	}
}

void MCC::SlowIfDesired()

{
	if (oapiGetTimeAcceleration() > 1.0) {
		oapiSetTimeAcceleration(1.0);
	}
}