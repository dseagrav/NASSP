/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Mission File Handling

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

#include "Mission.h"
#include "OrbiterAPI.h"
#include <map>

std::map<std::string, mission::Mission*> pa_mission_hashmap;

DLLCLBK mission::Mission* paGetDefaultMission()
{
	return paGetMission("");
}

DLLCLBK mission::Mission* paGetMission(const std::string& mission_name)
{
	if (pa_mission_hashmap.find(mission_name) != pa_mission_hashmap.end())
	{
		oapiWriteLog("(Mission File Management) Found loaded mission file.");
		return pa_mission_hashmap[mission_name];
	}
	else {
		oapiWriteLog("(Mission File Management) Load mission file into shared memory.");
		mission::Mission* new_mission = new mission::Mission(mission_name);
		pa_mission_hashmap.insert(std::make_pair(mission_name, new_mission));
		return new_mission;
	}
}

void InitMissionManagementMemory()
{
	pa_mission_hashmap.clear();
}

void ClearMissionManagementMemory()
{
	std::map<std::string, mission::Mission*>::iterator iter
		= pa_mission_hashmap.begin();
	while (iter != pa_mission_hashmap.end())
	{
		if (iter->second)
		{
			delete iter->second;
			iter->second = NULL;
		}
		iter++;
	}
}

namespace mission {

	Mission::Mission(const std::string& strMission)
	{
		SetDefaultValues();
		if (!strMission.empty())
		{
			if (!LoadMission(strMission))
			{
				oapiWriteLog("(Mission) failed creating mission from loaded file.");
			}
		}
	}

	Mission::~Mission()
	{
	}

	void Mission::SetDefaultValues()
	{
		iSMJCVersion = 1;
		bJMission = false;
		iPanel277Version = 0;
		iPanel278Version = 1;
		iLMDSKYVersion = 2;
		bHasLMProgramer = false;
		bHasAEA = true;
		bHasAscEngArmAssy = false;
		bLMHasLegs = true;
		bCSMHasHGA = true;
		bCSMHasVHFRanging = true;
		strCMCVersion = "Artemis072";
		strLGCVersion = "Luminary210";
		strAEAVersion = "FP8";
		bInvertLMStageBit = false;
	}

	bool Mission::LoadMission(const int iMission)
	{
		return LoadMission("Apollo " + std::to_string(iMission));
	}

	bool Mission::LoadMission(const std::string& strMission)
	{
		char buffer[256];
		std::string filename;
		filename = "Missions\\ProjectApollo\\" + strMission + ".cfg";
		strFileName = strMission;

		sprintf_s(buffer, 255, "(Mission) Loading mission %s from file %s",
			strMission.c_str(), filename.c_str());
		oapiWriteLog(buffer);
		FILEHANDLE hFile = oapiOpenFile(filename.c_str(), FILE_IN);

		if (hFile == NULL)
		{
			oapiWriteLog("(Mission) ERROR: Can't open file.");
			return false;
		}

		if (oapiReadItem_string(hFile, "Name", buffer))
		{
			strMissionName = buffer;
		}
		oapiReadItem_int(hFile, "SMJCVersion", iSMJCVersion);
		oapiReadItem_bool(hFile, "JMission", bJMission);
		oapiReadItem_int(hFile, "Panel277Version", iPanel277Version);
		oapiReadItem_int(hFile, "Panel278Version", iPanel278Version);
		oapiReadItem_int(hFile, "LMDSKYVersion", iLMDSKYVersion);
		oapiReadItem_bool(hFile, "HasLMProgramer", bHasLMProgramer);
		oapiReadItem_bool(hFile, "HasAEA", bHasAEA);
		oapiReadItem_bool(hFile, "HasAscEngArmAssy", bHasAscEngArmAssy);
		oapiReadItem_bool(hFile, "LMHasLegs", bLMHasLegs);
		oapiReadItem_bool(hFile, "CSMHasHGA", bCSMHasHGA);
		oapiReadItem_bool(hFile, "CSMHasVHFRanging", bCSMHasVHFRanging);
		if (oapiReadItem_string(hFile, "CMCVersion", buffer))
		{
			strCMCVersion = buffer;
		}
		if (oapiReadItem_string(hFile, "LGCVersion", buffer))
		{
			strLGCVersion = buffer;
		}
		if (oapiReadItem_string(hFile, "AEAVersion", buffer))
		{
			strAEAVersion = buffer;
		}
		oapiReadItem_bool(hFile, "InvertLMStageBit", bInvertLMStageBit);

		oapiCloseFile(hFile, FILE_IN);
		return true;
	}

	int Mission::GetSMJCVersion() const
	{
		return iSMJCVersion;
	}

	bool Mission::IsJMission() const
	{
		return bJMission;
	}

	int Mission::GetPanel277Version() const
	{
		return iPanel277Version;
	}

	int Mission::GetPanel278Version() const
	{
		return iPanel278Version;
	}

	int Mission::GetLMDSKYVersion() const
	{
		return iLMDSKYVersion;
	}

	bool Mission::HasLMProgramer() const
	{
		return bHasLMProgramer;
	}

	bool Mission::HasAEA() const
	{
		return bHasAEA;
	}

	bool Mission::HasAscEngArmAssy() const
	{
		return bHasAscEngArmAssy;
	}

	bool Mission::LMHasLegs() const
	{
		return bLMHasLegs;
	}

	bool Mission::CSMHasHGA() const
	{
		return bCSMHasHGA;
	}

	bool Mission::CSMHasVHFRanging() const
	{
		return bCSMHasVHFRanging;
	}

	const std::string& Mission::GetCMCVersion() const
	{
		return strCMCVersion;
	}

	const std::string& Mission::GetLGCVersion() const
	{
		return strLGCVersion;
	}

	const std::string& Mission::GetAEAVersion() const
	{
		return strAEAVersion;
	}

	bool Mission::IsLMStageBitInverted() const
	{
		return bInvertLMStageBit;
	}
}