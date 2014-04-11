/****************************************************************************/
/// @file    ODAmitranHandler.cpp
/// @author  Michael Behrisch
/// @date    27.03.2014
/// @version $Id$
///
// An XML-Handler for Amitran OD matrices
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/MsgHandler.h>
#include "ODMatrix.h"
#include "ODAmitranHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
ODAmitranHandler::ODAmitranHandler(ODMatrix& matrix, const std::string& file)
    : SUMOSAXHandler(file), myMatrix(matrix) {}


ODAmitranHandler::~ODAmitranHandler() {}


void
ODAmitranHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    switch (element) {
        case SUMO_TAG_ACTORCONFIG:
            myVehicleType = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
            break;
        case SUMO_TAG_TIMESLICE:
            myBegin = attrs.get<int>(SUMO_ATTR_STARTTIME, myVehicleType.c_str(), ok);
            myEnd = myBegin + attrs.get<int>(SUMO_ATTR_DURATION, myVehicleType.c_str(), ok);
            if (myBegin >= myEnd) {
                WRITE_ERROR("Invalid duration for timeSlice starting " + toString(myBegin) + ".");
            }
            break;
        case SUMO_TAG_OD_PAIR:
            myMatrix.add(attrs.get<SUMOReal>(SUMO_ATTR_AMOUNT, myVehicleType.c_str(), ok),
                         myBegin, myEnd, attrs.get<std::string>(SUMO_ATTR_ORIGIN, myVehicleType.c_str(), ok),
                         attrs.get<std::string>(SUMO_ATTR_DESTINATION, myVehicleType.c_str(), ok), myVehicleType);
            break;
        default:
            break;
    }
}


/****************************************************************************/
