/****************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2018

MCC sequencing for Mission B

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
#include "saturn.h"
#include "sivb.h"
#include "LEM.h"
#include "mcc.h"
#include "MCC_Mission_B.h"
#include "iu.h"

void MCC::MissionSequence_B()
{
	switch (MissionState)
	{
	case MST_B_PRELAUNCH:
		switch (SubState) {
		case 0:
			if (lm->GetMissionTime() > -5.0)
			{
				SIVB *lmsat = (SIVB *)sivb;
				lmsat->GetIU()->GetEDS()->EnableCommandSystem();
				setSubState(1);
			}
			break;
		case 1:
			char uplinkdata[1000];
			sprintf(uplinkdata, "V65E");
			strncpy(upString, uplinkdata, 1024 * 3);
			if (upString[0] != 0) {
				this->pushLGCUplinkString(upString);
			}
			this->LM_uplink_buffer();
			setSubState(2);
			break;
		case 2:
			if (lm->VHF.mcc_size == 0)
			{
				setSubState(3);
			}
			break;
		case 3:
			if (lm->GetMissionTime() > 0.0)
			{
				setState(MST_B_COASTING);
			}
			break;
		}
		break;
	case MST_B_COASTING:
		if (lm->GetMissionTime() > 8.0*3600.0 + 52.0*60.0 + 10.0)
		{
			setState(MST_B_RCS_TESTS1);
		}
		break;
	case MST_B_RCS_TESTS1:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 8.0 * 3600.0 + 52.0 * 60.0 + 30.0, 1, MST_B_RCS_TESTS2);
		break;
	case MST_B_RCS_TESTS2:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 8.0 * 3600.0 + 52.0 * 60.0 + 40.0, 2, MST_B_RCS_TESTS3);
		break;
	case MST_B_RCS_TESTS3:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 9.0 * 3600.0, 3, MST_B_RCS_TESTS4);
		break;
	case MST_B_RCS_TESTS4:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 9.0 * 3600.0 + 20.0, 4, MST_B_RCS_TESTS5);
		break;
	case MST_B_RCS_TESTS5:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 9.0 * 3600.0 + 46.0 * 60.0 + 5.0, 5, MST_B_RCS_TESTS6);
		break;
	case MST_B_RCS_TESTS6:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 9.0 * 3600.0 + 46.0 * 60.0 + 35.0, 6, MST_B_RCS_TESTS7);
		break;
	case MST_B_RCS_TESTS7:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 9.0 * 3600.0 + 47.0 * 60.0 + 20.0, 3, MST_B_RCS_TESTS8);
		break;
	case MST_B_RCS_TESTS8:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 11.0 * 3600.0 + 27.0 * 60.0 + 35.0, 4, MST_B_RCS_TESTS9);
		break;
	case MST_B_RCS_TESTS9:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 11.0 * 3600.0 + 28.0 * 60.0 + 20.0, 3, MST_B_RCS_TESTS10);
		break;
	case MST_B_RCS_TESTS10:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 11.0 * 3600.0 + 31.0 * 60.0 + 20.0, 4, MST_B_RCS_TESTS11);
		break;
	case MST_B_RCS_TESTS11:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 11.0 * 3600.0 + 31.0 * 60.0 + 30.0, 3, MST_B_RCS_TESTS12);
		break;
	case MST_B_RCS_TESTS12:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 12.0 * 3600.0 + 51.0 * 60.0, 4, MST_B_RCS_TESTS13);
		break;
	case MST_B_RCS_TESTS13:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 12.0 * 3600.0 + 51.0 * 60.0 + 20.0, 7, MST_B_RCS_TESTS14);
		break;
	case MST_B_RCS_TESTS14:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 12.0 * 3600.0 + 51.0 * 60.0 + 50.0, 8, MST_B_RCS_TESTS15);
		break;
	case MST_B_RCS_TESTS15:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, lm->GetMissionTime() > 12.0 * 3600.0 + 52.0 * 60.0 + 15.0, 9, MST_B_RCS_TESTS16);
		break;
	case MST_B_RCS_TESTS16:
		UpdateMacro(UTP_LGCUPLINKDIRECT, PT_NONE, false, 3, MST_B_RCS_TESTS17);
		break;
	}
}