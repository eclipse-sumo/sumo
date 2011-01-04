/****************************************************************************/
/// @file    AGActivity.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Parent object for all activities. Derived classes generate trips for each
// household.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
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

#include <utils/common/RandHelper.h>
#include "AGActivity.h"
#include "../city/AGTime.h"


// ===========================================================================
// method definitions
// ===========================================================================
bool
AGActivity::isGenerated() {
    return genDone;
}

bool
AGActivity::generateTrips() {
    return true;
}

int
AGActivity::possibleTranspMean(AGPosition destination) {
    int FOOT = 1;
    int BUS = 2;
    int CAR = 4;

    int transp = 0;

    if (destination.distanceTo(hh->getPosition()) <= ds->maxFootDistance) {
        transp = FOOT;
        if (hh->getCarNbr() != 0)
            transp += CAR;
        if (destination.minDistanceTo(ds->busStations) <= ds->maxFootDistance
                && hh->getPosition().minDistanceTo(ds->busStations) <= ds->maxFootDistance)
            transp += BUS;
    } else if (hh->getCarNbr() == 0) {
        SUMOReal d1 = destination.distanceTo(hh->getPosition());
        SUMOReal d2 = destination.minDistanceTo(ds->busStations) + hh->getPosition().minDistanceTo(ds->busStations);

        if (d1 > d2) {
            transp = BUS;
        } else {
            transp = FOOT;
        }
    } else if (hh->getCarNbr() != 0) { //all other cases
        if (destination.minDistanceTo(ds->busStations) > ds->maxFootDistance
                || hh->getPosition().minDistanceTo(ds->busStations) > ds->maxFootDistance) {
            transp = CAR;
        } else {
            transp = CAR + BUS;
        }
    }
    return transp;
}

int
AGActivity::availableTranspMeans(AGPosition from, AGPosition to) {
    int FOOT = 1;
    int BUS = 2;

    int available = 0;

    if (from.distanceTo(to) <= ds->maxFootDistance) {
        available += FOOT;
    }
    if (from.minDistanceTo(ds->busStations) <= ds->maxFootDistance
            && to.minDistanceTo(ds->busStations) <= ds->maxFootDistance) {
        available += BUS;
    }
    return available;
}

int
AGActivity::timeToDrive(AGPosition from, AGPosition to) {
    SUMOReal dist = from.distanceTo(to);
    return (int)(timePerKm * dist / 1000.0);
}

int
AGActivity::depHour(AGPosition from, AGPosition to, int arrival) {
    // ?? departure.addDays(1); // in case of negative time: arrival < timeToDrive
    //departure.setDay(0); // days are set to 0 because we want the time in the current day
    return (arrival - timeToDrive(from, to));
}

int
AGActivity::arrHour(AGPosition from, AGPosition to, int departure) {
    return (departure + timeToDrive(from, to));
}

int
AGActivity::randomTimeBetween(int begin, int end) {
    if (0 > begin || begin > end)
        return -1;
    if (begin == end)
        return begin;
    int tAlea = RandHelper::rand(end - begin);
    return (begin + tAlea);
}

/****************************************************************************/






