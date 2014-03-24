/****************************************************************************/
/// @file    AGHousehold.h
/// @author  Piotr Woznica
/// @author  Daniel Krajzewicz
/// @author  Walter Bamberger
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// A household contains the people and cars of the city: roughly represents
// families with their address, cars, adults and possibly children
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
class AGHousehold {
public:
    AGHousehold(AGPosition pos, AGCity* city, int idHouseholds) :
        myCity(city),
        myLocation(pos),
        myId(idHouseholds) {};
    AGHousehold(AGStreet* str, AGCity* city, int idHouseholds) :
        myCity(city),
        myLocation(*str),
        myId(idHouseholds) {};
    /**
     * function generating the given number of adults (1 or 2) and possibly children
     */
    void generatePeople(int numAdults, int numChilds, bool firstRetired);
    unsigned int getPeopleNbr();
    unsigned int getAdultNbr();
    const std::list<AGAdult>& getAdults() const;
    const std::list<AGChild>& getChildren() const;
    const std::list<AGCar>& getCars() const;
    /**
     * function returning true if the household is close to the given stations stations
     */
    bool isCloseFromPubTransport(std::list<AGPosition>* pubTransport);
    bool isCloseFromPubTransport(std::map<int, AGPosition>* pubTransport);
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
    void generateCars(SUMOReal rate);
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
     * returns if adults are retired or in working age
     */
    bool retiredHouseholders();
    /**
     * returns the position of the household and other private entities
     */
    AGPosition getPosition();

private:
    AGCity* myCity;
    AGPosition myLocation;
    int myNumberOfCars;
    int myId;

private:
    std::list<AGAdult> myAdults;
    std::list<AGChild> myChildren;
    std::list<AGCar> myCars;
};

#endif

/****************************************************************************/
