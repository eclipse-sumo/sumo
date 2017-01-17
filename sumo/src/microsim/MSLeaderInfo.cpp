/****************************************************************************/
/// @file    MSLeaderInfo.cpp
/// @author  Jakob Erdmann
/// @date    Oct 2015
/// @version $Id$
///
// Information about vehicles ahead (may be multiple vehicles if
// lateral-resolution is active)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <math.h>
#include <utils/common/ToString.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSNet.h>
#include "MSLeaderInfo.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================


// ===========================================================================
// MSLeaderInfo member method definitions
// ===========================================================================
MSLeaderInfo::MSLeaderInfo(const MSLane* lane, const MSVehicle* ego, SUMOReal latOffset) :
    myWidth(lane->getWidth()),
    myVehicles(MAX2(1, int(ceil(myWidth / MSGlobals::gLateralResolution))), (MSVehicle*)0),
    myFreeSublanes((int)myVehicles.size()),
    egoRightMost(-1),
    egoLeftMost(-1),
    myHasVehicles(false) {
    if (ego != 0) {
        getSubLanes(ego, latOffset, egoRightMost, egoLeftMost);
        // filter out sublanes not of interest to ego
        myFreeSublanes -= egoRightMost;
        myFreeSublanes -= (int)myVehicles.size() - 1 - egoLeftMost;
    }
}


MSLeaderInfo::~MSLeaderInfo() { }


int
MSLeaderInfo::addLeader(const MSVehicle* veh, bool beyond, SUMOReal latOffset) {
    if (veh == 0) {
        return myFreeSublanes;
    }
    if (myVehicles.size() == 1) {
        // speedup for the simple case
        if (!beyond || myVehicles[0] == 0) {
            myVehicles[0] = veh;
            myFreeSublanes = 0;
            myHasVehicles = true;
        }
        return myFreeSublanes;
    }
    // map center-line based coordinates into [0, myWidth] coordinates
    int rightmost, leftmost;
    getSubLanes(veh, latOffset, rightmost, leftmost);
    for (int sublane = rightmost; sublane <= leftmost; ++sublane) {
        if ((egoRightMost < 0 || (egoRightMost <= sublane && sublane <= egoLeftMost))
                && (!beyond || myVehicles[sublane] == 0)) {
            if (myVehicles[sublane] == 0) {
                myFreeSublanes--;
            }
            myVehicles[sublane] = veh;
            myHasVehicles = true;
        }
    }
    return myFreeSublanes;
}


void
MSLeaderInfo::clear() {
    myVehicles.assign(myVehicles.size(), (MSVehicle*)0);
    myFreeSublanes = (int)myVehicles.size();
    if (egoRightMost >= 0) {
        myFreeSublanes -= egoRightMost;
        myFreeSublanes -= (int)myVehicles.size() - 1 - egoLeftMost;
    }
}


void
MSLeaderInfo::getSubLanes(const MSVehicle* veh, SUMOReal latOffset, int& rightmost, int& leftmost) const {
    if (myVehicles.size() == 1) {
        // speedup for the simple case
        rightmost = 0;
        leftmost = 0;
        return;
    }
    // map center-line based coordinates into [0, myWidth] coordinates
    const SUMOReal vehCenter = veh->getLateralPositionOnLane() + 0.5 * myWidth + latOffset;
    const SUMOReal vehHalfWidth = 0.5 * veh->getVehicleType().getWidth();
    const SUMOReal rightVehSide = MAX2((SUMOReal)0,  vehCenter - vehHalfWidth);
    const SUMOReal leftVehSide = MIN2(myWidth, vehCenter + vehHalfWidth);
    rightmost = (int)floor((rightVehSide + NUMERICAL_EPS) / MSGlobals::gLateralResolution);
    leftmost = MIN2((int)myVehicles.size() - 1, (int)floor(leftVehSide / MSGlobals::gLateralResolution));
    //if (veh->getID() == "car2") std::cout << SIMTIME << " veh=" << veh->getID()
    //    << std::setprecision(10)
    //    << " posLat=" << veh->getLateralPositionOnLane()
    //    << " rightVehSide=" << rightVehSide
    //    << " leftVehSide=" << leftVehSide
    //    << " rightmost=" << rightmost
    //    << " leftmost=" << leftmost
    //    << std::setprecision(2)
    //    << "\n";
}


const MSVehicle*
MSLeaderInfo::operator[](int sublane) const {
    assert(sublane >= 0);
    assert(sublane < (int)myVehicles.size());
    return myVehicles[sublane];
}


std::string
MSLeaderInfo::toString() const {
    std::ostringstream oss;
    oss.setf(std::ios::fixed , std::ios::floatfield);
    oss << std::setprecision(2);
    for (int i = 0; i < (int)myVehicles.size(); ++i) {
        oss << Named::getIDSecure(myVehicles[i]);
        if (i < (int)myVehicles.size() - 1) {
            oss << ", ";
        }
    }
    oss << " free=" << myFreeSublanes;
    return oss.str();
}


bool
MSLeaderInfo::hasStoppedVehicle() const {
    if (!myHasVehicles) {
        return false;
    }
    for (int i = 0; i < (int)myVehicles.size(); ++i) {
        if (myVehicles[0] != 0 && myVehicles[0]->isStopped()) {
            return true;
        }
    }
    return false;
}

// ===========================================================================
// MSLeaderDistanceInfo member method definitions
// ===========================================================================


MSLeaderDistanceInfo::MSLeaderDistanceInfo(const MSLane* lane, const MSVehicle* ego, SUMOReal latOffset) :
    MSLeaderInfo(lane, ego, latOffset),
    myDistances(myVehicles.size(), std::numeric_limits<SUMOReal>::max()) {
}


MSLeaderDistanceInfo::~MSLeaderDistanceInfo() { }


int
MSLeaderDistanceInfo::addLeader(const MSVehicle* veh, SUMOReal gap, SUMOReal latOffset, int sublane) {
    //if (SIMTIME == 31 && gDebugFlag1 && veh != 0 && veh->getID() == "cars.8") {
    //    std::cout << " BREAKPOINT\n";
    //}
    if (veh == 0) {
        return myFreeSublanes;
    }
    if (myVehicles.size() == 1) {
        // speedup for the simple case
        sublane = 0;
    }
    if (sublane >= 0 && sublane < (int)myVehicles.size()) {
        // sublane is already given
        if (gap < myDistances[sublane]) {
            if (myVehicles[sublane] == 0) {
                myFreeSublanes--;
            }
            myVehicles[sublane] = veh;
            myDistances[sublane] = gap;
            myHasVehicles = true;
        }
        return myFreeSublanes;
    }
    int rightmost, leftmost;
    getSubLanes(veh, latOffset, rightmost, leftmost);
    for (int sublane = rightmost; sublane <= leftmost; ++sublane) {
        if ((egoRightMost < 0 || (egoRightMost <= sublane && sublane <= egoLeftMost))
                && gap < myDistances[sublane]) {
            if (myVehicles[sublane] == 0) {
                myFreeSublanes--;
            }
            myVehicles[sublane] = veh;
            myDistances[sublane] = gap;
            myHasVehicles = true;
        }
    }
    return myFreeSublanes;
}


void
MSLeaderDistanceInfo::clear() {
    MSLeaderInfo::clear();
    myDistances.assign(myVehicles.size(), std::numeric_limits<SUMOReal>::max());
}


CLeaderDist
MSLeaderDistanceInfo::operator[](int sublane) const {
    assert(sublane >= 0);
    assert(sublane < (int)myVehicles.size());
    return std::make_pair(myVehicles[sublane], myDistances[sublane]);
}


std::string
MSLeaderDistanceInfo::toString() const {
    std::ostringstream oss;
    oss.setf(std::ios::fixed , std::ios::floatfield);
    oss << std::setprecision(2);
    for (int i = 0; i < (int)myVehicles.size(); ++i) {
        oss << Named::getIDSecure(myVehicles[i]) << ":";
        if (myVehicles[i] == 0) {
            oss << "inf";
        } else {
            oss << myDistances[i];
        }
        if (i < (int)myVehicles.size() - 1) {
            oss << ", ";
        }
    }
    oss << " free=" << myFreeSublanes;
    return oss.str();
}


// ===========================================================================
// MSCriticalFollowerDistanceInfo member method definitions
// ===========================================================================


MSCriticalFollowerDistanceInfo::MSCriticalFollowerDistanceInfo(const MSLane* lane, const MSVehicle* ego, SUMOReal latOffset) :
    MSLeaderDistanceInfo(lane, ego, latOffset),
    myMissingGaps(myVehicles.size(), -std::numeric_limits<SUMOReal>::max()) {
}


MSCriticalFollowerDistanceInfo::~MSCriticalFollowerDistanceInfo() { }


int
MSCriticalFollowerDistanceInfo::addFollower(const MSVehicle* veh, const MSVehicle* ego, SUMOReal gap, SUMOReal latOffset, int sublane) {
    if (veh == 0) {
        return myFreeSublanes;
    }
    const SUMOReal requiredGap = veh->getCarFollowModel().getSecureGap(veh->getSpeed(), ego->getSpeed(), ego->getCarFollowModel().getMaxDecel());
    const SUMOReal missingGap = requiredGap - gap;
    //if (gDebugFlag1) {
    //    std::cout << "   addFollower veh=" << veh->getID()
    //        << " ego=" << ego->getID()
    //        << " gap=" << gap
    //        << " reqGap=" << requiredGap
    //        << " missingGap=" << missingGap
    //        << " latOffset=" << latOffset
    //        << " sublane=" << sublane
    //        << "\n";
    //    if (sublane > 0) {
    //        std::cout
    //            << "        dists[s]=" << myDistances[sublane]
    //            << " gaps[s]=" << myMissingGaps[sublane]
    //            << "\n";
    //    }
    //}
    if (myVehicles.size() == 1) {
        // speedup for the simple case
        sublane = 0;
    }
    if (sublane >= 0 && sublane < (int)myVehicles.size()) {
        // sublane is already given
        if (missingGap > myMissingGaps[sublane]) {
            if (myVehicles[sublane] == 0) {
                myFreeSublanes--;
            }
            myVehicles[sublane] = veh;
            myDistances[sublane] = gap;
            myMissingGaps[sublane] = missingGap;
            myHasVehicles = true;
        }
        return myFreeSublanes;
    }
    int rightmost, leftmost;
    getSubLanes(veh, latOffset, rightmost, leftmost);
    for (int sublane = rightmost; sublane <= leftmost; ++sublane) {
        if ((egoRightMost < 0 || (egoRightMost <= sublane && sublane <= egoLeftMost))
                && missingGap > myMissingGaps[sublane]) {
            if (myVehicles[sublane] == 0) {
                myFreeSublanes--;
            }
            myVehicles[sublane] = veh;
            myDistances[sublane] = gap;
            myMissingGaps[sublane] = missingGap;
            myHasVehicles = true;
        }
    }
    return myFreeSublanes;
}


void
MSCriticalFollowerDistanceInfo::clear() {
    MSLeaderDistanceInfo::clear();
    myMissingGaps.assign(myVehicles.size(), -std::numeric_limits<SUMOReal>::max());
}


std::string
MSCriticalFollowerDistanceInfo::toString() const {
    std::ostringstream oss;
    oss.setf(std::ios::fixed , std::ios::floatfield);
    oss << std::setprecision(2);
    for (int i = 0; i < (int)myVehicles.size(); ++i) {
        oss << Named::getIDSecure(myVehicles[i]) << ":";
        if (myVehicles[i] == 0) {
            oss << "inf:-inf";
        } else {
            oss << myDistances[i] << ":" << myMissingGaps[i];
        }
        if (i < (int)myVehicles.size() - 1) {
            oss << ", ";
        }
    }
    oss << " free=" << myFreeSublanes;
    return oss.str();
}
/****************************************************************************/

