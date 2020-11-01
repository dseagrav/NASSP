/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Lunar Module Mission Programer

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

#include "Orbitersdk.h"
#include "soundlib.h"
#include "toggleswitch.h"
#include "apolloguidance.h"
#include "LEM.h"
#include "lm_programer.h"
#include "papi.h"
#include "Mission.h"

LEM_Programer::LEM_Programer()
{
	lem = NULL;

	for (int i = 0; i < 128;i++)
	{
		Relays[i] = false;
	}
}

void LEM_Programer::Init(LEM *l)
{
	lem = l;
}

void LEM_Programer::ProcessChannel10(ChannelValue val)
{
	if (!lem->pMission->HasLMProgramer()) return;

	ChannelValue10LMP out_val;

	out_val.Value = val.to_ulong();

	if (out_val.Bits.a == 15)
	{
		switch (out_val.Bits.b)
		{
		case 4: //Pyro Master Arm On
			lem->EDMasterArm.SetState(1);
			break;
		case 5:	//Pyro Master Arm Off
			lem->EDMasterArm.SetState(0);
			break;
		case 6:	//RCS Press Fire
			lem->EDHePressRCS.SetState(1);
			break;
		case 7:	//RCS Press Fire Reset
			lem->EDHePressRCS.SetState(0);
			break;
		case 12: //RCS Thruster Isol Valve 1A Open
			lem->RCSQuad1ACmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 13: //RCS Thruster Isol Valve 1A Open Reset
			lem->RCSQuad1ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 14: //RCS Thruster Isol Valve 3A Open
			lem->RCSQuad3ACmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 15: //RCS Thruster Isol Valve 3A Open Reset
			lem->RCSQuad3ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 22: //Abort Stage Fire
			lem->AbortStageSwitch.SetState(0);
			break;
		case 23: //Abort Stage Fire Reset
			lem->AbortStageSwitch.SetState(1);
			break;
		case 28: //RCS Thruster Isol Valve 1B Open
			lem->RCSQuad1BCmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 29: //RCS Thruster Isol Valve 1B Open Reset
			lem->RCSQuad1BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 30: //RCS Thruster Isol Valve 3B Open
			lem->RCSQuad3BCmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 31: //RCS Thruster Isol Valve 3B Open Reset
			lem->RCSQuad3BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 60: //RCS ASC Feed A Open
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_UP);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 61:	//RCS ASC Feed A Open Reset
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 62:	//RCS ASC Feed B Open
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_UP);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 63: //RCS ASC Feed B Open Reset
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 76: //RCS ASC Feed A Closed
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_DOWN);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 77: //RCS ASC Feed A Closed Reset
			lem->RCSAscFeed1ASwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2ASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 78: //RCS ASC Feed B Closed
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_DOWN);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 79: //RCS ASC Feed B Closed Reset
			lem->RCSAscFeed1BSwitch.SetState(THREEPOSSWITCH_CENTER);
			lem->RCSAscFeed2BSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 90: //LM/SLA Sep Fire
			lem->StartSeparationPyros();
			break;
		case 91: //LM/SLA Sep Fire Reset
			break;
		case 92: //RCS Thruster Isol Valve 1A Close
			lem->RCSQuad1ACmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 93: //RCS Thruster Isol Valve 1A Close Reset
			lem->RCSQuad1ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 94: //RCS Thruster Isol Valve 3A Close
			lem->RCSQuad3ACmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 95: //RCS Thruster Isol Valve 3A Close Reset
			lem->RCSQuad3ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 108: //RCS Thruster Isol Valve 1B Close
			lem->RCSQuad1BCmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 109: //RCS Thruster Isol Valve 1B Close Reset
			lem->RCSQuad1BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 110: //RCS Thruster Isol Valve 3B Close
			lem->RCSQuad3BCmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 111: //RCS Thruster Isol Valve 3B Close Reset
			lem->RCSQuad3BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 120: //RCS Thruster Isol Valve 2B Close
			lem->RCSQuad2BCmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 121: //RCS Thruster Isol Valve 2B Close Reset
			lem->RCSQuad2BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 128: //X-Trans On
			Relays[64] = true;
			break;
		case 129: //X-Trans Off
			Relays[64] = false;
			break;
		case 134: //APS Arm
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 135: //APS Arm Reset
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 136: //RCS Thruster Isol Valve 2B Open
			lem->RCSQuad2BCmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 137: //RCS Thruster Isol Valve 2B Open Reset
			lem->RCSQuad2BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 150: //DPS Arm
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 151: //DPS Reset
			lem->EngineArmSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 152: //RCS Thruster Isol Valve 2A Open
			lem->RCSQuad2ACmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 153: //RCS Thruster Isol Valve 2A Open Reset
			lem->RCSQuad2ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 166: //Engine Stop On
			lem->CDRManualEngineStop.SetState(1);
			break;
		case 167: //Engine Stop Reset
			lem->CDRManualEngineStop.SetState(0);
			break;
		case 168: //RCS Thruster Isol Valve 2A Close
			lem->RCSQuad2ACmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 169: //RCS Thruster Isol Valve 2A Close Reset
			lem->RCSQuad2ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 172: //RCS Main A Closed
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 173: //RCS Main A Closed Reset
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 174: //RCS Main B Closed
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 175: //RCS Main B Closed Reset
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 184: //RCS Thruster Isol Valve 4B Open
			lem->RCSQuad4BCmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 185: //RCS Thruster Isol Valve 4B Open Reset
			lem->RCSQuad4BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 188: //RCS Main A Open
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 189: //RCS Main A Open Reset
			lem->RCSMainSovASwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 190: //RCS Main B Open
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 191: //RCS Main B Open Reset
			lem->RCSMainSovBSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 196: //DPS PQGS 2 Arm
			//lem->QTYMonSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 197: //DPS PQGS 2 Reset
			//lem->QTYMonSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 200: //RCS Thruster Isol Valve 4B Close
			lem->RCSQuad4BCmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 201: //RCS Thruster Isol Valve 4B Close Reset
			lem->RCSQuad4BCmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 212: //DPS PQGS 2 On
			lem->QTYMonSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 213: //DPS PQGS 2 Off
			lem->QTYMonSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 216: //RCS Thruster Isol Valve 4A Open
			lem->RCSQuad4ACmdEnableSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 217: //RCS Thruster Isol Valve 4A Open Reset
			lem->RCSQuad4ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 228: //DPS PQGS 1 Arm
			//lem->QTYMonSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 229: //DPS PQGS 1 Reset
			//lem->QTYMonSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 232: //RCS Thruster Isol Valve 4A Close
			lem->RCSQuad4ACmdEnableSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 233: //RCS Thruster Isol Valve 4A Close Reset
			lem->RCSQuad4ACmdEnableSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 244: //DPS PQGS 1 On
			lem->QTYMonSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 245: //DPS PQGS 1 Off
			lem->QTYMonSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 252: //RCS X-Feed Open
			lem->RCSXFeedSwitch.SetState(THREEPOSSWITCH_UP);
			break;
		case 253: //RCS X-Feed Open Reset
			lem->RCSXFeedSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		case 254: //RCS X-Feed Close
			lem->RCSXFeedSwitch.SetState(THREEPOSSWITCH_DOWN);
			break;
		case 255: //RCS X-Feed Close Reset
			lem->RCSXFeedSwitch.SetState(THREEPOSSWITCH_CENTER);
			break;
		default:
			break;
		}
	}
}