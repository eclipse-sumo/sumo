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
/// @file    AGActivities.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Main class that manages activities taken in account and generates the
// inhabitants' trip list.
/****************************************************************************/
#ifndef AGACTIVITIES_H
#define AGACTIVITIES_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <list>
#include "AGTrip.h"
#include "../city/AGCity.h"
#include "../city/AGBusLine.h"
#include "../city/AGHousehold.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGActivities {
public:
    AGActivities(AGCity* city, int days) :
        myCity(city),
        nbrDays(days) {};
    void addTrip(AGTrip t, std::list<AGTrip>* tripSet);
    void addTrips(std::list<AGTrip> t, std::list<AGTrip>* tripSet);
    void generateActivityTrips();

    /**
     * trips contains trips as well for one day as for every day,
     * these trips will be regenerated with small variations
     * by ActivityGen at the end of the simulation
     * before generating the trip file
     */
    std::list<AGTrip> trips;

private:
    bool generateTrips(AGHousehold& hh);
    bool generateBusTraffic(AGBusLine bl);
    bool generateInOutTraffic();
    bool generateRandomTraffic();

    /**
     * generates car names, given the unique (number, prefix)
     */
    std::string generateName(int i, std::string prefix);

    AGCity* myCity;

    int nbrDays;

};

#endif

/****************************************************************************/
