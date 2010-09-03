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

#include <iostream>
#include "AGStreet.h"
#include "AGPosition.h"
#include "AGDataAndStatistics.h"
#include "AGAdult.h"


// ===========================================================================
// class declarations
// ===========================================================================
class AGAdult;


// ===========================================================================
// class definitions
// ===========================================================================
class AGWorkPosition
{
public:
	AGWorkPosition(AGStreet *inStreet, int opening, int closing, AGDataAndStatistics* ds) :
			location(inStreet),
			openingTime(opening),
			closingTime(closing),
			taken(false),
			ds(ds)
		{
			ds->workPositions++;
		};
	AGWorkPosition(AGStreet *inStreet, AGDataAndStatistics* ds) :
			location(inStreet),
			openingTime(generateOpeningTime(ds)),
			closingTime(generateClosingTime(ds)),
			taken(false),
			ds(ds)
		{
			ds->workPositions++;
		};
	AGWorkPosition(AGStreet *inStreet, float pos, AGDataAndStatistics* ds) :
			location(inStreet, pos),
			openingTime(generateOpeningTime(ds)),
			closingTime(generateClosingTime(ds)),
			taken(false),
			ds(ds)
		{
			ds->workPositions++;
		};
	~AGWorkPosition() {
		let();
	}
	void print();
	int generateOpeningTime(AGDataAndStatistics* ds);
	int generateClosingTime(AGDataAndStatistics* ds);
	bool take(AGAdult* ad);
	bool let();
	bool isTaken();
	AGPosition getPosition();
	int getOpening();
	int getClosing();

private:
	AGDataAndStatistics* ds;
	AGAdult* adult;
	AGPosition location;
	int openingTime;
	int closingTime;
	bool taken;
};

#endif

/****************************************************************************/
