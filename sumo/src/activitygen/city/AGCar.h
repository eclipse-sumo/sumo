/****************************************************************************/
/// @file    AGCar.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Cars owned by people of the city: included in households.
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
#ifndef AGCAR_H
#define AGCAR_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <string>
#include "AGAdult.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGCar
{
public:
	AGCar(std::string name) :
		idName(name)
	{};
	AGCar(int idHH, int idCar) :
		idName(createName(idHH, idCar))
	{};
	bool associateTo(AGAdult *pers);
	bool isAssociated();
	std::string getName();

private:
	std::string createName(int idHH, int idCar);

	std::string idName;
	AGAdult* currentUser;

};

#endif
 
/****************************************************************************/
