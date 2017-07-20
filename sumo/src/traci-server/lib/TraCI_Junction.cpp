/****************************************************************************/
/// @file    TraCI_Junction.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Robert Hilbrich
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/shapes/PointOfInterest.h>
#include <utils/shapes/ShapeContainer.h>
#include <microsim/MSNet.h>
#include <microsim/MSJunctionControl.h>
#include "TraCI_Junction.h"
#include "TraCI.h"


// ===========================================================================
// member definitions
// ===========================================================================
std::vector<std::string>
TraCI_Junction::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getJunctionControl().insertIDs(ids);
    return ids;
}

int
TraCI_Junction::getIDCount() {
    return (int) getIDList().size();
}

TraCIPosition
TraCI_Junction::getPosition(const std::string& junctionID) {
    return TraCI::makeTraCIPosition(getJunction(junctionID)->getPosition());
}

TraCIPositionVector
TraCI_Junction::getShape(const std::string& junctionID) {
    return TraCI::makeTraCIPositionVector(getJunction(junctionID)->getShape());
}

MSJunction*
TraCI_Junction::getJunction(const std::string& id) {
    MSJunction* j = MSNet::getInstance()->getJunctionControl().get(id);
    if (j == 0) {
        throw TraCIException("Junction '" + id + "' is not known");
    }
    return j;
}


/****************************************************************************/
