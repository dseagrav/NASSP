/**************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005



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

#if !defined(_PA_PAYLOAD_H)
#define _PA_PAYLOAD_H

#include "connector.h"

struct PayloadSettings {

	double AscentFuelKg;
	double DescentFuelKg;
	double AscentEmptyKg;
	double DescentEmptyKg;
	int MissionNo;
	double MissionTime;
	char CSMName[64];
	bool Crewed;
	bool AutoSlow;
	char checklistFile[100];
};

///
/// This is the base class for all SIVB-launched payloads which can be
/// separated from the SIVB.
///
/// \ingroup SepStages
///
class Payload : public ProjectApolloConnectorVessel
{
public:
	///
	/// \brief Constructor.
	///
	Payload(OBJHANDLE hObj, int fmodel);
	
	virtual ~Payload();

	///
	/// \brief Set up the payload based on the payload settings.
	/// \param p Payload settings.
	///
	virtual bool SetupPayload(PayloadSettings &p);
};

#endif // _PA_PAYLOAD_H
