/****************************************************************************/
/// @file    AGBusLine.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Bus line of the city: contains all the buses of this line
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
#ifndef AGBUSLINE_H
#define AGBUSLINE_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include <list>
#include "AGBus.h"
#include "AGPosition.h"
#include "AGDataAndStatistics.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGBusLine {
public:
    AGBusLine(std::string lineNr) :
            lineNumber(lineNr) {};
    void setMaxTripTime(int time);
    void setBusNames();
    int nbrBuses();
    void locateStation(AGPosition pos);
    void locateRevStation(AGPosition pos);
    void generateBuses(int start, int stop, int rate);
    void printBuses();

    std::list<AGPosition> stations;
    std::list<AGPosition> revStations;
    std::list<AGBus> buses;
    std::list<AGBus> revBuses;

private:
    /**
     * @return: a name for a new Bus. unique for the city.
     */
    std::string createName();

    /**
     * @param time: time of departure of the bus in one direction (current time)
     * @return: time at which it will be ready for going in the opposite direction
     */
    int getReady(int time);

    std::string lineNumber;
    int maxTripTime;
    int busNbr;
};

#endif

/****************************************************************************/
