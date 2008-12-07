/****************************************************************************/
/// @file    MSLaneMeanDataValues.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: MSLaneMeanDataValues.h 5922 2008-08-05 08:15:53Z dkrajzew $
///
// Data structure for mean (aggregated) edge/lane values
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

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSLaneMeanDataValues.h"
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <utils/common/SUMOTime.h>

#ifdef HAVE_MESOSIM
#include <map>
#endif


// ===========================================================================
// method definitions
// ===========================================================================
MSLaneMeanDataValues::MSLaneMeanDataValues(MSLane* lane) throw()
        : MSMoveReminder(lane), sampleSeconds(0), nVehLeftLane(0), nVehEnteredLane(0),
        speedSum(0), haltSum(0), vehLengthSum(0),
        emitted(0) {}


void
MSLaneMeanDataValues::reset() throw()
{
    sampleSeconds = 0.;
    nVehLeftLane = 0;
    nVehEnteredLane = 0;
    speedSum = 0;
    haltSum = 0;
    vehLengthSum = 0;
    emitted = 0;
}


bool
MSLaneMeanDataValues::isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw()
{
    bool ret = true;
    SUMOReal l = veh.getLength();
    SUMOReal fraction = 1.;
    if (oldPos<0&&newSpeed!=0) {
        fraction = (oldPos+SPEED2DIST(newSpeed)) / newSpeed;
        ++nVehEnteredLane;
    }
    if (oldPos+SPEED2DIST(newSpeed)>getLane()->length()&&newSpeed!=0) {
        fraction -= (oldPos+SPEED2DIST(newSpeed) - getLane()->length()) / newSpeed;
        ++nVehLeftLane;
        ret = false;
    }
    sampleSeconds += fraction;
    speedSum += newSpeed * fraction;
    vehLengthSum += l * fraction;
    if (newSpeed<0.1) { // !!! swell
        haltSum++;
    }
    return ret;
}


void
MSLaneMeanDataValues::dismissByLaneChange(MSVehicle& veh) throw()
{
}


bool
MSLaneMeanDataValues::isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw()
{
    ++emitted;
    SUMOReal l = veh.getLength();
    SUMOReal fraction = 1.;
    if (veh.getPositionOnLane()+l>getLane()->length()) {
        fraction = l - (getLane()->length()-veh.getPositionOnLane());
    }
    sampleSeconds += fraction;
    speedSum += veh.getSpeed() * fraction;
    vehLengthSum += l * fraction;
    if (veh.getSpeed()<0.1) { // !!! swell
        haltSum++;
    }
    return true;
}


/****************************************************************************/

