/****************************************************************************/
/// @file    AGActivityGen.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Main class that handles City, Activities and Trips
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include <sstream>
#include "AGActivityGen.h"
#include "AGActivityGenHandler.h"
#include "city/AGPosition.h"
#include "activities/AGActivities.h"
#include "AGActivityTripWriter.h"
#include "city/AGTime.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGActivityGen::importInfoCity() {
    AGActivityGenHandler handler(city, net);
    handler.setFileName(inputFile);
    XMLSubSys::init(false);
    MsgHandler::initOutputOptions();
    if (!XMLSubSys::runParser(handler, inputFile)) {
        MsgHandler::getMessageInstance()->endProcessMsg("failed.");
        throw ProcessError();
    } else {
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }

    cout << "### read input done." << endl;
    city.statData.consolidateStat(); //some maps are still not
    cout << "### pre-processing done." << endl;
    city.completeStreets();
    cout << "### - streets done..." << endl;
    city.generateWorkPositions();
    cout << "### - work done..." << endl;
    city.completeBusLines();
    cout << "### - bus lines done..." << endl;

    city.generatePopulation();
    cout << "### ...city built." << endl;

    city.schoolAllocation();
    city.workAllocation();
    city.carAllocation();
    cout << "### allocation done." << endl;
}

bool
AGActivityGen::timeTripValidation(AGTrip trip) {
    if (trip.getDay() > durationInDays+1)
        return false;
    if (trip.getDay() == 1) { //first day
        if (trip.getTime() < beginTime)
            return false;
        if (durationInDays == 0 && trip.getTime() > endTime)
            return false;
    }
    if (trip.getDay() == durationInDays+1) { //last day
        if (trip.getTime() > endTime)
            return false;
        if (durationInDays == 0 && trip.getTime() < beginTime)
            return false;
    }
    return true;
}

void
AGActivityGen::varDepTime(AGTrip & trip) {
    if (trip.getType() != "default")
        return;
    //buses are on time and random are already spread
    int variation = (int)RandHelper::randNorm(0, city.statData.departureVariation);
    AGTime depTime(trip.getDay(), 0, 0, trip.getTime());
    depTime += variation;
    if (depTime.getDay() > 0) {
        trip.setDay(depTime.getDay());
        trip.setDepTime(depTime.getSecondsInCurrentDay());
    } else {
        trip.setDay(1);
        trip.setDepTime(0);
    }
}

void
AGActivityGen::generateOutputFile(std::list<AGTrip>& trips) {
    AGActivityTripWriter atw(outputFile);
    list<AGTrip>::iterator it;
    //variables for TESTS:
    int firstTrip = trips.front().getTime() + trips.front().getDay()*86400;
    int lastTrip = trips.front().getTime() + trips.front().getDay()*86400;
    map<int, int> histogram;
    for (int i=0 ; i<100 ; ++i)
        histogram[i] = 0;
    //END var TESTS
    for (it=trips.begin() ; it!=trips.end() ; ++it) {
        atw.addTrip(*it);
        //TEST
        if (it->getTime() + 86400*it->getDay() > lastTrip)
            lastTrip = it->getTime() + 86400*it->getDay();
        if (it->getTime() + 86400*it->getDay() < firstTrip)
            firstTrip = it->getTime() + 86400*it->getDay();
        //++histogram[((it->getDay()-1)*86400 + it->getTime())/3600];
        ++histogram[(it->getTime())/3600];
        //END TEST
    }
    //PRINT TEST
    AGTime first(firstTrip);
    AGTime last(lastTrip);
    cout << "first real trip: " << first.getDay() << ", " << first.getHour() << ":" << first.getMinute() << ":" << first.getSecond() << endl;
    cout << "last real trip: " << last.getDay() << ", " << last.getHour() << ":" << last.getMinute() << ":" << last.getSecond() << endl;
    for (int i=0 ; i<100 ; ++i) {
        if (histogram[i] > 0) {
            cout << "histogram[ hour " << i << " ] = " << histogram[i] << endl;
        }
    }
    //END TEST
    atw.writeOutputFile();
}

void
AGActivityGen::makeActivityTrips(int days, int beginSec, int endSec) {
    durationInDays = days;
    beginTime = beginSec;
    endTime = endSec;
    /**
     * making the activity trips
     */
    AGActivities acts(&city, durationInDays+1);
    acts.generateActivityTrips();

    /**
     * validating the trips with the simulation's time limits
     */
    //list<Trip>* trips = &(acts.trips);
    list<AGTrip> expTrips;
    map<string, int> carUsed;
    list<AGTrip>::iterator it;
    //multiplication of days
    for (it=acts.trips.begin() ; it!=acts.trips.end() ; ++it) {
        if (it->isDaily()) {
            for (int currday=1 ; currday<durationInDays+2 ; ++currday) {
                AGTrip tr(it->getDep(), it->getArr(), it->getVehicleName(), it->getTime(), currday);
                tr.setType(it->getType());
                if (carUsed.find(tr.getVehicleName()) != carUsed.end()) {
                    ++carUsed.find(tr.getVehicleName())->second;
                } else {
                    carUsed[tr.getVehicleName()] = 1;
                }
                std::ostringstream os;
                os << tr.getVehicleName() << ":" << carUsed.find(tr.getVehicleName())->second;
                tr.setVehicleName(os.str());
                tr.addLayOverWithoutDestination(*it); //intermediate destinations are taken in account too
                varDepTime(tr); //slight variation on each "default" car
                if (timeTripValidation(tr))
                    expTrips.push_back(tr);
                //else
                //	cout << "trop tard 1 pour " << tr.getVehicleName() << " " << tr.getTime() << " day: " << tr.getDay() << endl;
            }
        } else {
            AGTrip tr(it->getDep(), it->getArr(), it->getVehicleName(), it->getTime(), it->getDay());
            tr.setType(it->getType());
            if (carUsed.find(tr.getVehicleName()) != carUsed.end()) {
                ++carUsed.find(tr.getVehicleName())->second;
            } else {
                carUsed[tr.getVehicleName()] = 1;
            }
            std::ostringstream os;
            os << tr.getVehicleName() << ":" << carUsed.find(tr.getVehicleName())->second;
            tr.setVehicleName(os.str());
            tr.addLayOverWithoutDestination(*it); //intermediate destinations are taken in account too
            varDepTime(tr); //slight variation on each "default" car
            if (timeTripValidation(tr))
                expTrips.push_back(tr);
            //else
            //	cout << "trop tard 2 pour " << tr.getVehicleName() << " " << tr.getTime() << " day: " << tr.getDay() << endl;
        }
    }

    cout << "total trips generated: " << acts.trips.size() << endl;
    cout << "total trips finally taken: " << expTrips.size() << endl;

    /**
     * re-ordering of trips: SUMO needs routes ordered by departure time.
     */
    expTrips.sort(); //natural order of trips
    cout << "...sorted by departure time.\n" << endl;

    /**
     * trip file generation
     */
    generateOutputFile(expTrips);
}

/****************************************************************************/
