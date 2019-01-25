/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGBusLine.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Bus line of the city: contains all the buses of this line
/****************************************************************************/
#ifndef AGBUSLINE_H
#define AGBUSLINE_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
