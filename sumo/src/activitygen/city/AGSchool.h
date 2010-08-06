/****************************************************************************/
/// @file    AGSchool.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGSchool.h 8236 2010-02-10 11:16:41Z behrisch $
///
// Correspond to given ages and referenced by children. has a precise location.
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
#ifndef AGSCHOOL_H
#define AGSCHOOL_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include "AGPosition.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGSchool
{
public:
	AGSchool(int capacity_, AGPosition pos, int beginAge, int endAge, int open, int close) :
		beginAge(beginAge),
		endAge(endAge),
		  capacity(capacity_),
		  initCapacity(capacity_),
		  location(pos),
		  opening(open),
		  closing(close)
	  {};
	void print();
	int getPlaces();
	bool addNewChild();
	bool removeChild();
	int getBeginAge();
	int getEndAge();
	bool acceptThisAge(int age);
	AGPosition getPosition();
	int getClosingHour();
	int getOpeningHour();

private:
	int beginAge, endAge;
	int capacity;
	int initCapacity;
	AGPosition location;
	int opening, closing;
};

#endif

/****************************************************************************/
