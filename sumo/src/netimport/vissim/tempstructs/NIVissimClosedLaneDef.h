/****************************************************************************/
/// @file    NIVissimClosedLaneDef.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimClosedLaneDef_h
#define NIVissimClosedLaneDef_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif



// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimClosedLaneDef {
public:
    NIVissimClosedLaneDef(int lane, const std::vector<int>& assignedVehicles);
    ~NIVissimClosedLaneDef();
private:
    int myLaneNo;
    std::vector<int> myAssignedVehicles;
};


#endif

/****************************************************************************/

