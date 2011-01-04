/****************************************************************************/
/// @file    AGAdult.cpp
/// @author  Piotr Woznica & Walter Bamberger
/// @date    July 2010
/// @version $Id$
///
// Person in working age: can be linked to a work position.
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

#include "AGAdult.h"
#include "AGWorkPosition.h"
#include <utils/common/RandHelper.h>
#include <iostream>


// ===========================================================================
// method definitions
// ===========================================================================
AGWorkPosition*
AGAdult::randomFreeWorkPosition(std::vector<AGWorkPosition> *wps) throw() {
    size_t wpsIndex = 0;

    // TODO: Could end up in an endless loop
    do {
        wpsIndex = RandHelper::rand(wps->size());
    } while (wps->at(wpsIndex).isTaken());

    return &wps->at(wpsIndex);
}

/****************************************************************************/

AGAdult::AGAdult(int age) throw()
        : AGPerson(age), work(0) {}

/****************************************************************************/

void
AGAdult::print() const throw() {
    std::cout << "- AGAdult: Age=" << age << " Work=" << work << std::endl;
}

/****************************************************************************/

void
AGAdult::tryToWork(SUMOReal rate, std::vector<AGWorkPosition>* wps) throw() {
    if (decide(rate)) {
        // Select the new work position before giving up the current one.
        // This avoids that the current one is the same as the new one.
        AGWorkPosition* newWork = randomFreeWorkPosition(wps);

        if (work != 0) {
            work->let();
        }
        work = newWork;
        work->take(this);
    } else {
        if (work != 0) {
            // Also sets work = 0 with the call back lostWorkPosition
            work->let();
        }
    }
}

/****************************************************************************/

bool
AGAdult::isWorking() const throw() {
    return (work != 0);
}

/****************************************************************************/

void
AGAdult::lostWorkPosition() throw() {
    work = 0;
}

/****************************************************************************/

void
AGAdult::resignFromWorkPosition() throw() {
    if (work != 0)
        work->let();
}

/****************************************************************************/

const AGWorkPosition&
AGAdult::getWorkPosition() const throw(std::runtime_error) {
    if (work != 0)
        return *work;

    else
        throw(std::runtime_error("AGAdult::getWorkPosition: Adult is unemployed."));
}

/****************************************************************************/
