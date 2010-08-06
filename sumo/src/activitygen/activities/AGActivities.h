/****************************************************************************/
/// @file    AGActivities.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGActivities.h 8236 2010-02-10 11:16:41Z behrisch $
///
// Main object managing activities taken in account and generating the city
// inhabitants' trip list.
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
#ifndef AGACTIVITIES_H
#define AGACTIVITIES_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <list>
#include "AGTrip.h"
#include "../city/AGCity.h"
#include "../city/AGBusLine.h"
#include "../city/AGHousehold.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGActivities
{
public:
	AGActivities(AGCity *city, int days) :
		myCity(city),
		nbrDays(days)
	{};
	void addTrip(AGTrip t, std::list<AGTrip> *tripSet);
	void addTrips(std::list<AGTrip> t, std::list<AGTrip> *tripSet);
	void generateActivityTrips();

	/**
	 * trips contains trips as well for one day as for every day,
	 * these trips will be regenerated with small variations
	 * by ActivityGen at the end of the simulation
	 * before generating the trip file
	 */
	std::list<AGTrip> trips;

private:
	bool generateTrips(AGHousehold &hh);
	bool generateBusTraffic(AGBusLine bl);
	bool generateInOutTraffic();

	/**
	 * generates names for incoming cars, given there unique number
	 */
	std::string generateIncomingName(int i);
	
	AGCity *myCity;

	int nbrDays;

};

#endif

/****************************************************************************/
