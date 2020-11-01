/***************************************************************************
  This file is part of Project Apollo - NASSP
  Copyright 2004-2005 Mark Grant

  ORBITER vessel module: NASSP pyro simulation.

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

#ifndef __PYRO_H
#define __PYRO_H

#include "PanelSDK/Internals/Esystems.h"

///
/// This class simulates a small explosive charge used to explosively seperate parts of
/// the spacecraft.
/// \ingroup InternalSystems
/// \brief Pyro (explosive) simulation.
///
class Pyro : public e_object {

public:
	///
	/// Constructor. This has to register with the Panel SDK to ensure it's updated
	/// correctly and the state is saved to the scenario.
	/// \brief Pyro constructor.
	/// \param i_name Internal pyro name to pass to the Panel SDK.
	/// \param p The Panel SDK library.
	///
	Pyro(char *i_name, PanelSDK &p);

	///
	/// \brief Has the pyro blown?
	/// \return True if it's blown, false if it's still intact.
	///
	bool Blown() { return blown; };

	///
	/// Force the pyro into a set state, either blown or not blown. Setting it to not
	/// blown will reset the accumulated energy to zero, to ensure it doesn't just blow
	/// again on the next timestep.
	///
	/// \brief Set pyro state.
	/// \param b True to mark the pyro as blown, false to set it to unblown.
	///
	void SetBlown(bool b);

	void DrawPower(double watts);
	void refresh(double dt);
	void UpdateFlow(double dt);

	void Load(char *line);
	void Save(FILEHANDLE scn);

protected:
	///
	/// When the pyro is connected to an electrical source we accumulate
	/// the heat input from that source until the 'ignition wire' has heated
	/// enough to make the explosives ignite.
	///
	/// At that point we mark the pyro as blown and stop drawing any power from
	/// the source. If the pyro is disconnected from the power source before then,
	/// this 'ignition wire' energy will rapidly 'cool down' and need to be heated
	/// up again to ignite the explosives.
	///
	/// \brief Accumulated heat in pyro.
	///
	double energy;

	///
	/// \brief Has the pyro blown?
	///
	bool blown;

	///
	/// \brief The Panel SDK library for the vessel we're connected to.
	///
	PanelSDK &sdk;

};

#endif
