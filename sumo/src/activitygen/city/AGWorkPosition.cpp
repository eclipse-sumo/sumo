/****************************************************************************/
/// @file    AGWorkPosition.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGWorkPosition.cpp 8236 2010-02-10 11:16:41Z behrisch $
///
// location and schedules of a work position: linked with one adult
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2010 TUM (Technische Universität München, http://www.tum.de/)
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

#include <iostream>
#include <utils/common/RandHelper.h>
#include "AGWorkPosition.h"
#include "AGStreet.h"
#include "AGPosition.h"
#include "AGDataAndStatistics.h"
#include "AGAdult.h"


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
AGWorkPosition::print()
{
	cout << "- AGWorkPosition: open=" << openingTime << " closingTime=" << closingTime << " taken=" << taken << endl;
	cout << "\t";
	location.print();
}

int
AGWorkPosition::generateOpeningTime(AGDataAndStatistics* ds)
{
	float choice = (float)RandHelper::rand(); //((float)(rand()%1000))/1000.0;
	float cumul = 0;
	map<int,float>::iterator it = ds->beginWorkHours.begin();
	while(it!=ds->beginWorkHours.end())
	{
		cumul += it->second;
		if(cumul > choice)
			return it->first;
		++it;
	}
	cout << "-- WARNING: work time distribution not complete (Sum(proportions) != 1): AUTODEFINED at 9.00am --" << endl;
	return 900;
}

int
AGWorkPosition::generateClosingTime(AGDataAndStatistics* ds)
{
	float choice = (float)RandHelper::rand(); //((float)(rand()%1000))/1000.0;
	float cumul = 0;
	map<int,float>::iterator it = ds->endWorkHours.begin();
	while(it!=ds->endWorkHours.end())
	{
		cumul += it->second;
		if(cumul > choice)
			return it->first;
		++it;
	}
	cout << "-- WARNING: work time distribution not complete (Sum(proportions) != 1): AUTODEFINED at 5.00pm --" << endl;
	return 1700;
}

bool
AGWorkPosition::isTaken()
{
	return taken;
}

bool
AGWorkPosition::let()
{
	if(isTaken())
	{
		ds->workPositions++;
		adult->loseHisJob();
		taken = false;
	}
	return true;
}

bool
AGWorkPosition::take(AGAdult* ad)
{
	if(taken)
		return false;
	taken = true;
	ds->workPositions--;
	adult = ad;
	return taken;
}

AGPosition
AGWorkPosition::getPosition()
{
	return location;
}

int
AGWorkPosition::getClosing()
{
	return closingTime;
}

int
AGWorkPosition::getOpening()
{
	return openingTime;
}

/****************************************************************************/
