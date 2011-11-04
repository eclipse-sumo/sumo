/****************************************************************************/
/// @file    AGStreet.cpp
/// @author  Piotr Woznica & Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Represents a SUMO edge and contains people and work densities
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

#include "AGStreet.h"
#include "router/ROEdge.h"
#include <iostream>


// ===========================================================================
// method definitions
// ===========================================================================
AGStreet::AGStreet(const ROEdge* edge, SUMOReal popDensity, SUMOReal workDensity) :
    edge(edge) {
    pop = static_cast<int>(popDensity * edge->getLength());
    work = static_cast<int>(workDensity * edge->getLength());
}

/****************************************************************************/

void
AGStreet::print() const {
    std::cout << "- AGStreet: Name=" << edge->getID() << " Length=" << edge->getLength() << " pop=" << pop << " work=" << work << std::endl;
}

/****************************************************************************/

SUMOReal
AGStreet::getLength() const {
    return edge->getLength();
}

/****************************************************************************/

const std::string&
AGStreet::getName() const {
    return edge->getID();
}

/****************************************************************************/

int
AGStreet::getPopulation() const {
    return pop;
}

/****************************************************************************/

void
AGStreet::setPopulation(const int& population) {
    pop = population;
}

/****************************************************************************/

int
AGStreet::getWorkplaceNumber() const {
    return work;
}

/****************************************************************************/

void
AGStreet::setWorkplaceNumber(const int& workPositions) {
    work = workPositions;
}

/****************************************************************************/
