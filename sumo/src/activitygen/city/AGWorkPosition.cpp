/****************************************************************************/
/// @file    AGWorkPosition.cpp
/// @author  Piotr Woznica & Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Location and schedules of a work position: linked with one adult
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

#include "AGWorkPosition.h"
#include "AGStreet.h"
#include "AGPosition.h"
#include "AGDataAndStatistics.h"
#include "AGAdult.h"
#include <utils/common/RandHelper.h>
#include <iostream>


// ===========================================================================
// method definitions
// ===========================================================================
AGWorkPosition::AGWorkPosition(const AGStreet& inStreet, AGDataAndStatistics* ds) throw() :
        location(inStreet),
        openingTime(generateOpeningTime(*ds)),
        closingTime(generateClosingTime(*ds)),
        ds(ds),
        adult(0) {
    ds->workPositions++;
}

/****************************************************************************/

AGWorkPosition::AGWorkPosition(const AGStreet& inStreet, SUMOReal pos, AGDataAndStatistics* ds) throw() :
        location(inStreet, pos),
        openingTime(generateOpeningTime(*ds)),
        closingTime(generateClosingTime(*ds)),
        ds(ds),
        adult(0) {
    ds->workPositions++;
}

AGWorkPosition::~AGWorkPosition() throw() {
//    let();
}

/****************************************************************************/

void
AGWorkPosition::print() const throw() {
    std::cout << "- AGWorkPosition: open=" << openingTime << " closingTime=" << closingTime << " taken=" << isTaken() << std::endl;
    std::cout << "\t";
    location.print();
}

/****************************************************************************/

int
AGWorkPosition::generateOpeningTime(const AGDataAndStatistics& ds) throw() {
    SUMOReal choice = RandHelper::rand();
    SUMOReal cumul = 0;

    for (std::map<int,SUMOReal>::const_iterator it=ds.beginWorkHours.begin();
            it!=ds.beginWorkHours.end(); ++it) {
        cumul += it->second;
        if (cumul >= choice)
            return it->first;
    }
    std::cout << "-- WARNING: work time distribution not complete (Sum(proportions) != 1): AUTODEFINED at 9.00am --" << std::endl;
    return 900;
}

/****************************************************************************/

int
AGWorkPosition::generateClosingTime(const AGDataAndStatistics& ds) throw() {
    SUMOReal choice = RandHelper::rand();
    SUMOReal cumul = 0;
    for (std::map<int,SUMOReal>::const_iterator it=ds.endWorkHours.begin();
            it!=ds.endWorkHours.end(); ++it) {
        cumul += it->second;
        if (cumul >= choice)
            return it->first;
    }
    std::cout << "-- WARNING: work time distribution not complete (Sum(proportions) != 1): AUTODEFINED at 5.00pm --" << std::endl;
    return 1700;
}

/****************************************************************************/

bool
AGWorkPosition::isTaken() const throw() {
    return (adult != 0);
}

/****************************************************************************/

void
AGWorkPosition::let() throw() {
    if (adult != 0) {
        ds->workPositions++;
        adult->lostWorkPosition();
        adult = 0;
    }
}

/****************************************************************************/

void
AGWorkPosition::take(AGAdult* worker) throw(std::runtime_error) {
    if (adult == 0) {
        ds->workPositions--;
        adult = worker;
    } else {
        throw(std::runtime_error("Work position already occupied. Cannot give it to another adult."));
    }
}

/****************************************************************************/

AGPosition
AGWorkPosition::getPosition() const throw() {
    return location;
}

/****************************************************************************/

int
AGWorkPosition::getClosing() const throw() {
    return closingTime;
}

/****************************************************************************/

int
AGWorkPosition::getOpening() const throw() {
    return openingTime;
}

/****************************************************************************/
