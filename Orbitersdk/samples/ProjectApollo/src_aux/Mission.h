/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2020

  Mission File Handling (Header)

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

#include <OrbiterAPI.h>
#include <string>

namespace mission
{
	class Mission
	{
	public:
		Mission(const std::string& strMission);
		virtual ~Mission();

		virtual bool LoadMission(const int iMission);
		virtual bool LoadMission(const std::string& strMission);

		//1 = Block I and pre Apollo 12, 2 = Apollo 12 and later
		virtual int GetSMJCVersion() const;
		//false = any other CSM, true = J-type mission CSM (for all systems and panels common to CSM-112 to 114)
		virtual bool IsJMission() const;
		//0 = none, 1 = J-type mission, 2 = Skylab
		virtual int GetPanel277Version() const;
		//1 = pre Apollo 15, 2 = Apollo 15-16, 3 = Apollo 17, 4 = Skylab, 5 = ASTP
		virtual int GetPanel278Version() const;
		//1 = No lights in the bottom two rows, 2 = ALT + VEL lights, 3 = ALT, VEL, PRIO DISP, NO DAP lights
		virtual int GetLMDSKYVersion() const;
		//false = LM has no Programer, true = LM has programer
		virtual bool HasLMProgramer() const;
		//false = LM has no abort electronics assembly, true = LM has abort electronics assembly
		virtual bool HasAEA() const;
		//false = LM has no ascent engine arming assembly, = true = LM has ascent engine arming assembly
		virtual bool HasAscEngArmAssy() const;
		//false = LM has no legs, true = LM has legs
		virtual bool LMHasLegs() const;
		//false = CSM has no HGA, true = CSM has a HGA
		virtual bool CSMHasHGA() const;
		//false = CSM has no VHF Ranging System, true = CSM has VHF Ranging System
		virtual bool CSMHasVHFRanging() const;
		//Name of CMC software
		virtual const std::string& GetCMCVersion() const;
		//Name of LGC software
		virtual const std::string& GetLGCVersion() const;
		//Name of AEA software
		virtual const std::string& GetAEAVersion() const;
		//false = LM stage verify bit normal, true = inverted
		bool IsLMStageBitInverted() const;
	protected:
		std::string strFileName;
		std::string strMissionName;
		std::string strCMCVersion;
		std::string strLGCVersion;
		std::string strAEAVersion;

		int iSMJCVersion;
		bool bJMission;
		int iPanel277Version;
		int iPanel278Version;
		int iLMDSKYVersion;
		bool bHasLMProgramer;
		bool bHasAEA;
		bool bHasAscEngArmAssy;
		bool bLMHasLegs;
		bool bCSMHasHGA;
		bool bCSMHasVHFRanging;
		bool bInvertLMStageBit;

		void SetDefaultValues();
	};
}

DLLCLBK mission::Mission* paGetDefaultMission();
DLLCLBK mission::Mission* paGetMission(const std::string& filename);
void ClearMissionManagementMemory();
void InitMissionManagementMemory();