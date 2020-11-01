/***************************************************************************
This file is part of Project Apollo - NASSP
Copyright 2017

Digital Command System

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
#include "papi.h"
#include "iu.h"
#include "dcs.h"

DCS::DCS(IU *i)
{
	iu = i;
}

void DCS::LoadState(FILEHANDLE scn)
{

}

void DCS::SaveState(FILEHANDLE scn)
{

}

bool DCS::Uplink(int type, void *upl)
{
	if (IsCommandSystemEnabled())
	{
		if (type == DCSUPLINK_SWITCH_SELECTOR)
		{
			DCSSWITSEL *switsel = static_cast<DCSSWITSEL*>(upl);

			return iu->GetLVDA()->GeneralizedSwitchSelector(switsel->stage, switsel->channel);
		}
		else if (type == DCSUPLINK_TIMEBASE_UPDATE)
		{
			DCSTBUPDATE *tbupdate = static_cast<DCSTBUPDATE*>(upl);

			return iu->GetLVDA()->TimebaseUpdate(tbupdate->dt);
		}
		else if (type == DCSUPLINK_LM_ABORT)
		{
			return iu->GetLVDA()->LMAbort();
		}
		else if (type == DCSUPLINK_RESTART_MANEUVER_ENABLE)
		{
			return iu->GetLVDA()->RestartManeuverEnable();
		}
		else if (type == DCSUPLINK_INHIBIT_MANEUVER)
		{
			return iu->GetLVDA()->InhibitAttitudeManeuver();
		}
		else if (type == DCSUPLINK_TIMEBASE_8_ENABLE)
		{
			return iu->GetLVDA()->Timebase8Enable();
		}
		else if (type == DCSUPLINK_EVASIVE_MANEUVER_ENABLE)
		{
			return iu->GetLVDA()->EvasiveManeuverEnable();
		}
		else if (type == DCSUPLINK_EXECUTE_COMM_MANEUVER)
		{
			return iu->GetLVDA()->ExecuteCommManeuver();
		}
		else if (type == DCSUPLINK_SIVBIU_LUNAR_IMPACT)
		{
			DCSLUNARIMPACT *lunarimpact = static_cast<DCSLUNARIMPACT*>(upl);

			return iu->GetLVDA()->SIVBIULunarImpact(lunarimpact->tig, lunarimpact->dt, lunarimpact->pitch, lunarimpact->yaw);
		}
		else if (type == DCSUPLINK_SATURNIB_LAUNCH_TARGETING)
		{
			DCSLAUNCHTARGET *targeting = static_cast<DCSLAUNCHTARGET*>(upl);

			return iu->GetLVDA()->LaunchTargetingUpdate(targeting->V_T, targeting->R_T, targeting->theta_T, targeting->i, targeting->lambda_0, targeting->lambda_dot, targeting->T_GRR0);
		}
		else if (type == DCSUPLINK_SLV_NAVIGATION_UPDATE)
		{
			DCSSLVNAVUPDATE *targeting = static_cast<DCSSLVNAVUPDATE*>(upl);

			return iu->GetLVDA()->NavigationUpdate(targeting->PosS, targeting->DotS, targeting->NUPTIM);
		}
	}

	return false;
}

bool DCS::IsCommandSystemEnabled()
{ 
	return iu->GetControlDistributor()->GetIUCommandSystemEnable();
}