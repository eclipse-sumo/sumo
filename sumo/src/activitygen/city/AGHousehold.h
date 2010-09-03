/****************************************************************************/
/// @file    AGHousehold.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// A household contains the people and cars of the city: roughly represents
// families with their address, cars, adults and possibly children
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
#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <list>
#include "AGPerson.h"
#include "AGAdult.h"
#include "AGChild.h"
#include "AGCar.h"
#include "AGStreet.h"
#include "AGPosition.h"
#include "AGCity.h"


// ===========================================================================
// class declarations
// ===========================================================================
class AGCity;


// ===========================================================================
// class definitions
// ===========================================================================
class AGHousehold
{
public:
	AGHousehold(AGPosition pos, AGCity *city, int idHouseholds) :
	  location(pos),
	  myCity(city),
	  idHH(idHouseholds)
	  {};
	AGHousehold(AGStreet *str, AGCity *city, int idHouseholds) :
	  location(str),
	  myCity(city),
	  idHH(idHouseholds)
	  {};
	/**
	 * - function generating one first Adult.
	 * - a second person is created depending on the mean number of adults per Households
	 *   (and depending on the age of the first person)
	 * - and children are created for households of one or two people in age to work.
	 *   Children are created in relation to the mean number of children and
	 *   they are distributed with the POISSON's lay of probability.
	 */
	void generatePeople();
	int getPeopleNbr();
	int getAdultNbr();
	/**
	 * function returning true if the household is close to the given stations stations
	 */
	bool isCloseFromPubTransport(std::list<AGPosition> *pubTransport);
	bool isCloseFromPubTransport(std::map<int, AGPosition> *pubTransport);
	/**
	 * function regenerating the household:
	 * --> work positions and schools are resigned
	 * --> cars and people are deleted
	 * --> number of people are MAINTAINED
	 * --> work positions, schools and cars are reallocated
	 */
	void regenerate();
	/**
	 * associates a school to each children.
	 * return false if not done (not enough place at school in the city...
	 */
	bool allocateChildrenSchool();
	/**
	 * associates a work position to every working adult
	 * is taken in account the unemployment and the number of work positions
	 */
	bool allocateAdultsWork();
	/**
	 * function allocating cars to this household in relation to the given rate for each adult
	 */
	void generateCars(float rate);
	int getCarNbr();
	/**
	 * generates one (more) car in this household
	 */
	void addACar();
	/**
	 * returns the city pointer in which the household is.
	 */
	AGCity* getTheCity();
	/**
	 * returns the position of the household and other private entities
	 */
	AGPosition getPosition();
	std::list<AGAdult> adults;
	std::list<AGChild> children;
	std::list<AGCar> cars;

private:

	bool decisionProba(float p);

	AGCity *myCity;
	AGPosition location;
	int numberOfPeople;
	int numberOfCars;
	int idHH;
};

#endif

/****************************************************************************/
