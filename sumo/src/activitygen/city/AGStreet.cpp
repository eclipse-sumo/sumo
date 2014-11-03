/****************************************************************************/
/// @file    AGStreet.cpp
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Represents a SUMO edge and contains people and work densities
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


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "AGStreet.h"
#include "router/ROEdge.h"
#include <iostream>


// ===========================================================================
// method definitions
// ===========================================================================
AGStreet::AGStreet(const ROEdge* edge, SUMOReal popDensity, SUMOReal workDensity) :
    edge(edge) {
    pop = popDensity * edge->getLength();
    work = workDensity * edge->getLength();
}


void
AGStreet::print() const {
    std::cout << "- AGStreet: Name=" << edge->getID() << " Length=" << edge->getLength() << " pop=" << pop << " work=" << work << std::endl;
}


SUMOReal
AGStreet::getLength() const {
    return edge->getLength();
}


const std::string&
AGStreet::getName() const {
    return edge->getID();
}


SUMOReal
AGStreet::getPopulation() const {
    return pop;
}


void
AGStreet::setPopulation(const SUMOReal population) {
    pop = population;
}


SUMOReal
AGStreet::getWorkplaceNumber() const {
    return work;
}


void
AGStreet::setWorkplaceNumber(const SUMOReal workPositions) {
    work = workPositions;
}

/****************************************************************************/
