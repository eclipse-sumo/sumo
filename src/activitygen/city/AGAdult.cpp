/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGAdult.cpp
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Person in working age: can be linked to a work position.
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "AGAdult.h"
#include "AGWorkPosition.h"
#include <utils/common/RandHelper.h>
#include <iostream>


// ===========================================================================
// method definitions
// ===========================================================================
AGWorkPosition*
AGAdult::randomFreeWorkPosition(std::vector<AGWorkPosition>* wps) {
    std::vector<AGWorkPosition*> freePos;
    for (std::vector<AGWorkPosition>::iterator i = wps->begin(); i != wps->end(); ++i) {
        if (!i->isTaken()) {
            freePos.push_back(&*i);
        }
    }
    if (freePos.empty()) {
        return nullptr;
    }
    return RandHelper::getRandomFrom(freePos);
}


AGAdult::AGAdult(int age)
    : AGPerson(age), work(nullptr) {}


void
AGAdult::print() const {
    std::cout << "- AGAdult: Age=" << age << " Work=" << work << std::endl;
}


void
AGAdult::tryToWork(double rate, std::vector<AGWorkPosition>* wps) {
    if (decide(rate)) {
        // Select the new work position before giving up the current one.
        // This avoids that the current one is the same as the new one.
        AGWorkPosition* newWork = randomFreeWorkPosition(wps);

        if (work != nullptr) {
            work->let();
        }
        work = newWork;
        work->take(this);
    } else {
        if (work != nullptr) {
            // Also sets work = 0 with the call back lostWorkPosition
            work->let();
        }
    }
}


bool
AGAdult::isWorking() const {
    return (work != nullptr);
}


void
AGAdult::lostWorkPosition() {
    work = nullptr;
}


void
AGAdult::resignFromWorkPosition() {
    if (work != nullptr) {
        work->let();
    }
}


const AGWorkPosition&
AGAdult::getWorkPosition() const {
    if (work != nullptr) {
        return *work;
    }
    throw std::runtime_error("AGAdult::getWorkPosition: Adult is unemployed.");
}

/****************************************************************************/
