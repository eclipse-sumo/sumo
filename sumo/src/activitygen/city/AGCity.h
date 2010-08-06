/****************************************************************************/
/// @file    AGCity.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGCity.h 8236 2010-02-10 11:16:41Z behrisch $
///
// Main object containing all others of the city: in particular
// streets, households, bus lines, work positions and schools
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
#ifndef AGCITY_H
#define AGCITY_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <vector>
#include <list>
#include <router/RONet.h>
#include "AGPosition.h"
#include "AGDataAndStatistics.h"
#include "AGSchool.h"
#include "AGBusLine.h"
#include "AGWorkPosition.h"
#include "AGHousehold.h"


// ===========================================================================
// class declarations
// ===========================================================================
class AGHousehold;


// ===========================================================================
// class definitions
// ===========================================================================
class AGCity
{
public:
	AGCity() :
	  statData()
	  {};

	/**
	 * generates streets: complete the "streets" vector using the DataAndStat's map edges.
	 */
	void completeStreets();
	void generateWorkPositions();
	void completeBusLines();
	//void generateSchools();
	void generatePopulation();
	void schoolAllocation();
	void workAllocation();
	void carAllocation();

	/**
	 * manipulation functions
	 */
	AGStreet* getStreet(std::string edge);
	/**
	 * returns a random street
	 */
	AGStreet* getRandomStreet();

	AGDataAndStatistics statData;
	std::vector<AGStreet> streets;
	std::vector<AGWorkPosition> workPositions;
	std::list<AGSchool> schools;
	std::list<AGBusLine> busLines;
	std::list<AGHousehold> households;
	std::list<AGPosition> cityGates;
	std::list<AGAdult> peopleIncoming;

private:
	AGSchool closestSchoolTo(AGPosition pos);
	/**
	 * generates workpositions on the city's gates (entrances) for the outgoing work traffic.
	 */
	void generateOutgoingWP();
	/**
	 * generates people from outside the city for incoming traffic generation
	 */
	void generateIncomingPopulation();

	int NrStreets;
	int nbrCars;
};

#endif

/****************************************************************************/
