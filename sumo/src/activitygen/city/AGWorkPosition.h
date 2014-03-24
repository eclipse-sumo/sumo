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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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
#ifndef AGWORKPOSITION_H
#define AGWORKPOSITION_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
    AGWorkPosition(AGDataAndStatistics* ds, const AGStreet& inStreet, SUMOReal pos);
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
