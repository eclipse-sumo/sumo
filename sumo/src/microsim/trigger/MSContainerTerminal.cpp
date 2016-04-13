/****************************************************************************/
/// @file    MSContainerTerminal.cpp
/// @author  Melanie Weber
/// @author  Andreas Kendziorra
/// @date    Mon, 13.12.2005
/// @version $Id$
///
// A collection of stops for container where containers can be transhiped
// from one stop to another within the terminal.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2005-2016 DLR (http://www.dlr.de/) and contributors
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

//#include <cassert>
//#include "MSTrigger.h"
#include "MSContainerTerminal.h"
//#include <utils/common/SUMOVehicle.h>
//#include <microsim/MSVehicleType.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSContainerTerminal::MSContainerTerminal(const std::string& id,
        const std::set<MSContainerStop*> stops)
    : Named(id), myStops(stops) {}


MSContainerTerminal::~MSContainerTerminal() {}

//
//const MSLane&
//MSContainerTerminal::getLane() const {
//    return myLane;
//}
//
//
//SUMOReal
//MSContainerTerminal::getBeginLanePosition() const {
//    return myBegPos;
//}
//
//
//SUMOReal
//MSContainerTerminal::getEndLanePosition() const {
//    return myEndPos;
//}
//
//
//void
//MSContainerTerminal::enter(SUMOVehicle* what, SUMOReal beg, SUMOReal end) {
//    myEndPositions[what] = std::pair<SUMOReal, SUMOReal>(beg, end);
//    computeLastFreePos();
//}
//
//
//SUMOReal
//MSContainerTerminal::getLastFreePos(const SUMOVehicle& forVehicle) const {
//    if (myLastFreePos != myEndPos) {
//        return myLastFreePos - forVehicle.getVehicleType().getMinGap();
//    }
//    return myLastFreePos;
//}
//
//
//void
//MSContainerTerminal::leaveFrom(SUMOVehicle* what) {
//    assert(myEndPositions.find(what) != myEndPositions.end());
//    myEndPositions.erase(myEndPositions.find(what));
//    computeLastFreePos();
//}
//
//
//void
//MSContainerTerminal::computeLastFreePos() {
//    myLastFreePos = myEndPos;
//    std::map<SUMOVehicle*, std::pair<SUMOReal, SUMOReal> >::iterator i;
//    for (i = myEndPositions.begin(); i != myEndPositions.end(); i++) {
//        if (myLastFreePos > (*i).second.second) {
//            myLastFreePos = (*i).second.second;
//        }
//    }
//}



/****************************************************************************/

