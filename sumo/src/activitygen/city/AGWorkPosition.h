/****************************************************************************/
/// @file    AGWorkPosition.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Location and schedules of a work position: linked with one adult
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
    // TODO: Change order: ds, inStreet [, pos]
    AGWorkPosition(const AGStreet& inStreet, AGDataAndStatistics* ds) throw();
    AGWorkPosition(const AGStreet& inStreet, SUMOReal pos, AGDataAndStatistics* ds) throw();
    ~AGWorkPosition() throw();

    void take(AGAdult* ad) throw(std::runtime_error);
    void let() throw();
    bool isTaken() const throw();

    AGPosition getPosition() const throw();
    int getOpening() const throw();
    int getClosing() const throw();

    void print() const throw();

private:
    AGDataAndStatistics* ds;
    AGAdult* adult;
    AGPosition location;
    int openingTime;
    int closingTime;

    static int generateOpeningTime(const AGDataAndStatistics& ds) throw();
    static int generateClosingTime(const AGDataAndStatistics& ds) throw();
};

#endif

/****************************************************************************/
