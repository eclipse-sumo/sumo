/****************************************************************************/
/// @file    AGPerson.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGPerson.h 8236 2010-02-10 11:16:41Z behrisch $
///
// Parent object of every person, contains age and any natural characteristic
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
#ifndef AGPERSON_H
#define AGPERSON_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>


// ===========================================================================
// class definitions
// ===========================================================================
class AGPerson
{
public:
	AGPerson(int age, bool adult) : age(age), adult(adult) {};
	void print();
	int getAge();

	//decision feature for people in relation to the realisation probability
	bool decision(float probability);
	//returns if an adult
	bool isAdult();

protected:
	int age;
	bool adult;
	//bool driverLicence; //not used
	//char sex; //not used
};

#endif

/****************************************************************************/
