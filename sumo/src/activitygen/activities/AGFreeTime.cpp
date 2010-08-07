/****************************************************************************/
/// @file    AGFreeTime.cpp
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Child object of AGActivity: generates Trips corresponding to after-work
// activities like visiting family or party
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

#include "AGFreeTime.h"
#include "../city/AGTime.h"
#include <math.h>
#include <utils/common/RandHelper.h>


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;

const int AGFreeTime::DAY = 1;
const int AGFreeTime::EVENING = 2;
const int AGFreeTime::NIGHT = 4;

const int AGFreeTime::TB_DAY = (new AGTime(0,8,0))->getTime();
const int AGFreeTime::TE_DAY = (new AGTime(0,18,0))->getTime();
const int AGFreeTime::TB_EVENING = (new AGTime(0,19,0))->getTime();
const int AGFreeTime::TE_EVENING = (new AGTime(0,23,59))->getTime();
const int AGFreeTime::TB_NIGHT = (new AGTime(0,23,0))->getTime();
const int AGFreeTime::TE_NIGHT = (new AGTime(1,5,0))->getTime();


// ===========================================================================
// method definitions
// ===========================================================================
int
AGFreeTime::decideTypeOfTrip()
{
	if(hh->adults.front().decision(freqOut))
	{
		int num_poss = 0; //(possibleType % 2) + (possibleType / 4) + ((possibleType / 2) % 2);
		if(possibleType & DAY)
			++num_poss;
		if(possibleType & EVENING)
			++num_poss;
		if(possibleType & NIGHT)
			++num_poss;

		if(num_poss == 0)
			return 0;
		float alea = (float)RandHelper::rand(); //(float)(rand() % 1000) / 1000.0;
		int decision = (int)floor(alea * (float)num_poss);

		if(possibleType & DAY)
		{
			if(decision == 0)
				return DAY;
			else
				--decision;
		}
		if(possibleType & EVENING)
		{
			if(decision == 0)
				return EVENING;
			else
				--decision;
		}
		if(possibleType & NIGHT)
		{
			if(decision == 0)
				return NIGHT;
		}
	}
	return 0;
}

int
AGFreeTime::possibleTypeOfTrip()
{
	int val = 0;
	if(hh->adults.front().getAge() >= ds->limitAgeRetirement && tReady == 0)
		val += DAY+EVENING;
	else
	{
		if(hh->getPeopleNbr() > hh->getAdultNbr())
			val += NIGHT;

		list<AGAdult>::iterator itA;
		bool noBodyWorks = true;
		for(itA=hh->adults.begin() ; itA!=hh->adults.end() ; ++itA)
		{
			if(itA->isWorking())
				noBodyWorks = false;
		}
		if(noBodyWorks)
			val += DAY;

		if(tReady < (*(new AGTime(0,22,0))).getTime())
			val += EVENING;
	}
	return val;
}

bool
AGFreeTime::typeFromHomeDay(int day)
{
	int backHome = whenBackHomeThisDay(day);
	if(hh->cars.empty())
		return true;
	AGPosition destination(hh->getTheCity()->getRandomStreet());
	int depTime = randomTimeBetween(max(backHome, TB_DAY), (TB_DAY+TE_DAY)/2);
	int arrTime = this->arrHour(hh->getPosition(), destination, depTime);
	int retTime = randomTimeBetween(arrTime, TE_DAY);
	if(depTime < 0 || retTime < 0)
		return true; // not enough time during the day
	AGTrip depTrip(hh->getPosition(), destination, hh->cars.front().getName(), depTime, day);
	AGTrip retTrip(destination, hh->getPosition(), hh->cars.front().getName(), retTime, day);

	this->partialActivityTrips.push_back(depTrip);
	this->partialActivityTrips.push_back(retTrip);
	return true;
}

bool
AGFreeTime::typeFromHomeEvening(int day)
{
	int backHome = whenBackHomeThisDay(day);
	if(hh->cars.empty())
		return true;
	AGPosition destination(hh->getTheCity()->getRandomStreet());
	int depTime = randomTimeBetween(max(backHome, TB_EVENING), TE_EVENING);
	int arrTime = this->arrHour(hh->getPosition(), destination, depTime);
	int retTime = randomTimeBetween(arrTime, TE_EVENING);
	if(depTime < 0 || retTime < 0)
		return true; // not enough time during the day
	AGTrip depTrip(hh->getPosition(), destination, hh->cars.front().getName(), depTime, day);
	AGTrip retTrip(destination, hh->getPosition(), hh->cars.front().getName(), retTime, day);

	this->partialActivityTrips.push_back(depTrip);
	this->partialActivityTrips.push_back(retTrip);
	return true;
}

bool
AGFreeTime::typeFromHomeNight(int day)
{
	int backHome = whenBackHomeThisDay(day);
	int ActivitiesNextDay = whenBeginActivityNextDay(day); // is equal to 2 days if there is nothing the next day
	int nextDay = 0;
	if(hh->cars.empty())
		return true;
	AGPosition destination(hh->getTheCity()->getRandomStreet());

	int depTime = randomTimeBetween(max(backHome, TB_NIGHT), TE_NIGHT);
	int arrTime = this->arrHour(hh->getPosition(), destination, depTime);
	//we have to go back home before the beginning of next day activities.
	int lastRetTime = this->depHour(destination, hh->getPosition(), min(TE_NIGHT, ActivitiesNextDay));
	int retTime = randomTimeBetween(arrTime, lastRetTime);
	if(depTime < 0 || retTime < 0)
		return true; // not enough time during the day

	AGTime departureTime(depTime);
	nextDay = departureTime.getDay();
	departureTime.setDay(0);
	AGTrip depTrip(hh->getPosition(), destination, hh->cars.front().getName(), departureTime.getTime(), day+nextDay);

	AGTime returnTime(depTime);
	nextDay = returnTime.getDay();
	returnTime.setDay(0);
	AGTrip retTrip(destination, hh->getPosition(), hh->cars.front().getName(), returnTime.getTime(), day+nextDay);

	this->partialActivityTrips.push_back(depTrip);
	this->partialActivityTrips.push_back(retTrip);
	return true;
}

bool
AGFreeTime::generateTrips()
{
	tReady = whenBackHome();
	possibleType = possibleTypeOfTrip();
	int type;

	for(int day=1 ; day <= nbrDays ; ++day)
	{
		type = decideTypeOfTrip();
		if(type == 0)
			continue;
		else if(type == DAY)
		{
			if(!typeFromHomeDay(day))
				return false;
		}else if(type == EVENING)
		{
			if(!typeFromHomeEvening(day))
				return false;
		}else if(type == NIGHT)
		{
			if(!typeFromHomeNight(day))
				return false;
		}
	}
	genDone = true;
	return genDone;
}

int
AGFreeTime::whenBackHome()
{
	int timeBack = 0;
	if(!this->previousTrips->empty())
	{
		list<AGTrip>::iterator itT;
		for(itT=previousTrips->begin() ; itT!=previousTrips->end() ; ++itT)
		{
			if(timeBack < itT->getArrTime(this->timePerKm) && itT->isDaily())
			{
				timeBack = itT->getArrTime(this->timePerKm);
			}
		}
	}
	return timeBack;
}

int
AGFreeTime::whenBackHomeThisDay(int day)
{
	int timeBack = 0;
	if(!this->previousTrips->empty())
	{
		list<AGTrip>::iterator itT;
		for(itT=previousTrips->begin() ; itT!=previousTrips->end() ; ++itT)
		{
			if(timeBack < itT->getArrTime(this->timePerKm) && (itT->getDay() == day || itT->isDaily()))
			{
				timeBack = itT->getArrTime(this->timePerKm);
			}
		}
	}
	return timeBack;
}

int
AGFreeTime::whenBeginActivityNextDay(int day)
{
	AGTime timeBack(1,0,0);
	if(!this->previousTrips->empty())
	{
		list<AGTrip>::iterator itT;
		for(itT=previousTrips->begin() ; itT!=previousTrips->end() ; ++itT)
		{
			if(timeBack.getTime() > itT->getTime() && (itT->getDay() == (day+1) || itT->isDaily()))
			{
				timeBack.setTime(itT->getTime());
			}
		}
	}
	timeBack.addDays(1); // this the beginning of activities of the next day
	return timeBack.getTime();
}

/****************************************************************************/
