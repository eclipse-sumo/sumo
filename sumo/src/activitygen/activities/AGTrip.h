/****************************************************************************/
/// @file    AGTrip.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Class containing all information of a given trip (car, bus)
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
#ifndef AGTRIP_H
#define AGTRIP_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include "../city/AGPosition.h"
#include "../city/AGCar.h"
#include "../city/AGBus.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGTrip {
public:
    /*Trip() :
    	atTime(-1)
    {};*/
    AGTrip(AGPosition from, AGPosition to, int at) : //vehicle not specified
            from(from),
            to(to),
            atTime(at),
            type("default"),
            day(0) {};
    AGTrip(AGPosition from, AGPosition to, AGCar c, int at) :
            from(from),
            to(to),
            atTime(at),
            vehicle(c.getName()),
            type("default"),
            day(0) {};
    AGTrip(AGPosition from, AGPosition to, AGBus b, int at) :
            from(from),
            to(to),
            atTime(at),
            vehicle(b.getName()),
            type("bus"),
            day(0) {};
    AGTrip(AGPosition from, AGPosition to, std::string v, int at) :
            from(from),
            to(to),
            atTime(at),
            vehicle(v),
            type("default"),
            day(0) {};
    AGTrip(AGPosition from, AGPosition to, std::string v, int at, int day) :
            from(from),
            to(to),
            atTime(at),
            vehicle(v),
            type("default"),
            day(day) {};
    void print();
    bool operator<(AGTrip & trip);

    void addLayOver(AGPosition by);
    void addLayOver(AGTrip &trip);
    void addLayOverWithoutDestination(AGTrip &trip);

    AGPosition getDep();
    AGPosition getArr();
    int getTime();
    void setDepTime(int time);
    std::string getVehicleName();
    void setVehicleName(std::string name);
    void setArr(AGPosition arrival);
    void setDep(AGPosition departure);
    int getDay();
    void setDay(int day);
    std::string getType();
    void setType(std::string type);
    std::list<AGPosition>* getPassed();

    /**
     * returns the time regarding the departure time
     * going through the different points and coming back to the initial position
     * given the time to make one kilometer
     */
    int getRideBackArrTime(SUMOReal secPerKm);
    /**
     * returns the estimated arrival time
     * given the time to make one kilometer
     */
    int getArrTime(SUMOReal secPerKm);
    /**
     * gives the time in seconds for the trip
     * given a speed in seconds per kilometer (in city, not car speed
     * but time needed to make a distance in the city)
     */
    int getTimeTrip(SUMOReal secPerKm);
    /**
     * estimate the departure time needed for a given arrival time
     * and a speed in seconds per kilometer
     */
    int estimateDepTime(int arrTime, SUMOReal secPerKm);
    /**
     * returns whether this is a daily trip or a one day trip
     */
    bool isDaily();

private:
    int atTime;
    AGPosition from;
    AGPosition to;
    std::string vehicle;
    std::list<AGPosition> passBy;
    /**
     * if everyday : 0
     * else        : number of the day ( != 0 )
     */
    int day;
    /**
     * indicates if it is a bus or a car (or any type)
     * "bus", "default" or "random" (which is a kind of default)
     */
    std::string type;
};

#endif

/****************************************************************************/
