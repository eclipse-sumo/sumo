/****************************************************************************/
/// @file    AGActivity.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Parent object for all activities. Derived classes generate trips for each
// household.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AGACTIVITY_H
#define AGACTIVITY_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "AGTrip.h"
#include "../city/AGHousehold.h"
#include "../city/AGDataAndStatistics.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGActivity {
public:
    AGActivity(AGHousehold* hh, AGDataAndStatistics* das, std::list<AGTrip>* prevTrips, int prio) :
        myHousehold(hh),
        myStatData(das),
        myPreviousTrips(prevTrips),
        activityPriority(prio),
        genDone(false),
        timePerKm(das->speedTimePerKm),
        carPreference(das->carPreference) {};

    /**
     * returns whether the generation could have been well done
     */
    bool isGenerated();

    /**
     * main function called for trip generation
     * this function is overwritten in every child-class (every activity)
     */
    virtual bool generateTrips() = 0;

    /**
     * determine the possible transportation means, what would be chosen:
     * 1 = foot
     * 2 = bus
     * 4 = car
     * any combination is possible by simply addition of these values
     * (ex. 7 means: 4+2+1 <=> foot, bus and car possible / 5 means: 4+1 <=> only foot and car are possible)
     */
    int possibleTranspMean(AGPosition destination);

    /**
     * determine the possible means for a trip from one position to a destination.
     * whether CAR is necessary or not, BUS available or not...
     * 1 = by foot possible.
     * 2 = by bus possible.
     * 0 = by bus or foot NOT possible => only by car.
     * @NOTE: 4 is useless because it is always possible
     * @NOTE: 3 = 2 + 1 = means bus and foot possible.
     */
    int availableTranspMeans(AGPosition from, AGPosition to);

    /**
     * evaluation of the needed time for going from one point to an other using the car
     */
    int timeToDrive(AGPosition from, AGPosition to);

    /**
     * estimates the departure/arrival time given the departure location
     * the arrival location and the wished arrival/departure time
     */
    int depHour(AGPosition from, AGPosition to, int arrival);
    int arrHour(AGPosition from, AGPosition to, int departure);

    /**
     * evaluates a random time between the given two time instants
     */
    int randomTimeBetween(int begin, int end);

    std::list<AGTrip>& getPartialActivityTrips();

protected:
    AGHousehold* myHousehold;

    AGDataAndStatistics* myStatData;

    std::list<AGTrip>* myPreviousTrips;
    std::list<AGTrip> myPartialActivityTrips;
    int activityPriority;
    bool genDone;
    SUMOReal timePerKm;
    /**
     * rate of taking the car instead of the bus because of personal preference
     */
    SUMOReal carPreference;

};

#endif

/****************************************************************************/
