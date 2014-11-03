/****************************************************************************/
/// @file    AGCity.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// City class that contains all other objects of the city: in particular
// streets, households, bus lines, work positions and schools
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors
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
#ifndef AGCITY_H
#define AGCITY_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <vector>
#include <list>
#include <router/RONet.h>
#include "AGPosition.h"
#include "AGDataAndStatistics.h"
#include "AGSchool.h"
#include "AGBusLine.h"
#include "AGWorkPosition.h"
#include "AGHousehold.h"


// ===========================================================================
// class declarations
// ===========================================================================
class AGHousehold;


// ===========================================================================
// class definitions
// ===========================================================================
class AGCity {
public:
    AGCity(RONet* net) :
        statData(AGDataAndStatistics::getDataAndStatistics()),
        net(net),
        streetsCompleted(false) {};

    /**
     * generates streets: complete the "streets" vector using the DataAndStat's map edges.
     */
    void completeStreets();
    void generateWorkPositions();
    void completeBusLines();
    //void generateSchools();
    void generatePopulation();
    void schoolAllocation();
    void workAllocation();
    void carAllocation();

    /**
     * manipulation functions
     */
    const AGStreet& getStreet(const std::string& edge);
    /**
     * returns a random street
     */
    const AGStreet& getRandomStreet();

    AGDataAndStatistics& statData;
    std::vector<AGStreet> streets;
    std::vector<AGWorkPosition> workPositions;
    std::list<AGSchool> schools;
    std::list<AGBusLine> busLines;
    std::list<AGHousehold> households;
    std::vector<AGPosition> cityGates;
    std::list<AGAdult> peopleIncoming;

private:
    AGSchool closestSchoolTo(AGPosition pos);
    /**
     * generates workpositions on the city's gates (entrances) for the outgoing work traffic.
     */
    void generateOutgoingWP();
    /**
     * generates people from outside the city for incoming traffic generation
     */
    void generateIncomingPopulation();

    // @brief network of the city
    RONet* net;
    /**
     * false until the function completeStreets is called
     * this function completes streets and turn this parameter to true
     */
    bool streetsCompleted;

    int nbrCars;

private:
    /// @brief invalidated assignment operator
    AGCity& operator=(const AGCity&);
};

#endif

/****************************************************************************/
