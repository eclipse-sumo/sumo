/****************************************************************************/
/// @file    ROLane.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A single lane the router may use
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
#ifndef ROLane_h
#define ROLane_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <utils/common/Named.h>
#include <utils/common/SUMOVehicleClass.h>

class ROEdge;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROLane
 * The class for an edge's lane. Just a container for the speed and the
 * length of the lane.
 */
class ROLane : public Named
{
public:
    /// Constructor
    ROLane(const std::string &id, SUMOReal length, SUMOReal maxSpeed,
           const std::vector<SUMOVehicleClass> &allowed,
           const std::vector<SUMOVehicleClass> &disallowed);

    /// Destructor
    ~ROLane();

    /// Returns the length of the lane
    SUMOReal getLength() const;

    /// Returns the maximum speed allowed on this lane
    SUMOReal getSpeed() const;

    /// Return the Edge
    ROEdge *getEdge();

    /// Returns the list of allowed vehicle classes
    const std::vector<SUMOVehicleClass> &getAllowedClasses() const;

    /// Returns the list of not allowed vehicle classes
    const std::vector<SUMOVehicleClass> &getNotAllowedClasses() const;

protected:

    ROEdge *myEdge;
private:
    /// The length of the lane
    SUMOReal myLength;

    /// The maximum speed allowed on the lane
    SUMOReal myMaxSpeed;

    /// The list of allowed vehicle classes
    std::vector<SUMOVehicleClass> myAllowedClasses;

    /// The list of disallowed vehicle classes
    std::vector<SUMOVehicleClass> myNotAllowedClasses;

};


#endif

/****************************************************************************/

