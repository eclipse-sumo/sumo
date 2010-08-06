/****************************************************************************/
/// @file    AGStreet.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id: AGStreet.h 8236 2010-02-10 11:16:41Z behrisch $
///
// represents a SUMO edge and contains people and work densities
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
#ifndef AGSTREET_H
#define AGSTREET_H

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
#include "../../router/RONet.h"


// ===========================================================================
// class definitions
// ===========================================================================
class AGStreet
{
public:
	AGStreet(const std::string edg, RONet* net, int popD = 0, int workD = 0) :
	  edgeName(edg),
		  net(net),
		  popDensity(popD),
		  workDensity(workD)
	  {
		computeLength();
	  };
	void print();
	float getLength();
	std::string getName();
	int getPopDensity();
	int getWorkDensity();
	void setPopulation(int pop);
	void setWorkPositions(int work);

public:
	RONet* net;

private:
	void computeLength();

	std::string edgeName;
	int popDensity;
	int workDensity;
	float length;
};

#endif

/****************************************************************************/
