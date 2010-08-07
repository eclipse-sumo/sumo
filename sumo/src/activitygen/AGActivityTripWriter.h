/****************************************************************************/
/// @file    AGActivityTripWriter.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Object writing Trip objects in a SUMO-route file.
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
#ifndef AGACTIVITYTRIPWRITER_H
#define AGACTIVITYTRIPWRITER_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "activities/AGTrip.h"
#include <iostream>
#include <fstream>
#include <map>
#include <string>


// ===========================================================================
// class definitions
// ===========================================================================
class AGActivityTripWriter
{
public:
	AGActivityTripWriter(std::string file) :
		fileName(file),
		routes(file.c_str())
	{
		initialize();
	}

	void initialize();
	void addTrip(AGTrip trip);
	void writeOutputFile();

private:
	std::string fileName;
	std::ofstream routes;
	std::map<std::string, std::string> colors;

	void vtypes();
};


#endif

/****************************************************************************/
