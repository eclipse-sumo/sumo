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
/// @file    AGWorkPosition.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Location and schedules of a work position: linked with one adult
/****************************************************************************/
#ifndef AGWORKPOSITION_H
#define AGWORKPOSITION_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include "AGPosition.h"
#include <stdexcept>


// ===========================================================================
// class declarations
// ===========================================================================
class AGStreet;
class AGAdult;
class AGDataAndStatistics;


// ===========================================================================
// class definitions
// ===========================================================================
// TODO: Change name to AGWorkPlace?
// TODO: Counter for free work positions should be in City
// TODO: Change name of openingTime to something like startHour or openingHour
class AGWorkPosition {
public:
    AGWorkPosition(AGDataAndStatistics* ds, const AGStreet& inStreet);
    AGWorkPosition(AGDataAndStatistics* ds, const AGStreet& inStreet, double pos);
    ~AGWorkPosition();

    void take(AGAdult* ad);
    void let();
    bool isTaken() const;

    AGPosition getPosition() const;
    int getOpening() const;
    int getClosing() const;

    void print() const;

private:
    static int generateOpeningTime(const AGDataAndStatistics& ds);
    static int generateClosingTime(const AGDataAndStatistics& ds);

private:
    AGDataAndStatistics* myStatData;
    AGPosition myLocation;
    AGAdult* myAdult;
    int myOpeningTime;
    int myClosingTime;
};

#endif

/****************************************************************************/
