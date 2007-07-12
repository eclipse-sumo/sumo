/****************************************************************************/
/// @file    ROVehicleCont.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A container for vehicles
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROVehicleCont_h
#define ROVehicleCont_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <queue>
#include <vector>
#include "ROVehicle.h"
#include <utils/helpers/NamedObjectCont.h>
#include "ROHelper.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicleCont
 * A map of vehicle-ids to the vehicles themselves. Additionally supplies
 * a list of vehicles where these are sorted by their departure times.
 */
class ROVehicleCont
            : public NamedObjectCont<ROVehicle*>
{
public:
    /// Constructor
    ROVehicleCont();

    /// Destructor
    ~ROVehicleCont();

    /// Returns the list of sorted vehicles
    std::priority_queue<ROVehicle*,
    std::vector<ROVehicle*>,
    ROHelper::VehicleByDepartureComperator> &sort();

private:
    /// The sorted vehicle list
    std::priority_queue<ROVehicle*,
    std::vector<ROVehicle*>,
    ROHelper::VehicleByDepartureComperator> mySorted;

private:
    /// we made the copy constructor invalid
    ROVehicleCont(const ROVehicleCont &src);

    /// we made the assignment operator invalid
    ROVehicleCont &operator=(const ROVehicleCont &src);

};


#endif

/****************************************************************************/

