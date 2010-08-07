/****************************************************************************/
/// @file    AGAdult.h
/// @author  Piotr Woznica
/// @date    July 2010
/// @version $Id$
///
// Person in working age: can be liked to a work position.
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
#ifndef AGADULT_H
#define AGADULT_H

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
#include <string>
#include "AGPerson.h"
#include "AGWorkPosition.h"


// ===========================================================================
// class declarations
// ===========================================================================
class AGWorkPosition;


// ===========================================================================
// class definitions
// ===========================================================================
class AGAdult : public AGPerson
{
public:
	AGAdult(int age) : AGPerson(age, true), employed(false) {};
	void print();
	/**
	 * @param[in]: rate is the employment rate (1-unemployment)
	 * @param[in]: wps is the list of workpositions (available or not) from City object.
	 * @param[in]: hasStillWork is the number of work positions not yet allocated (from DataAndStatistics object)
	 */
	bool assocWork(float rate, std::vector<AGWorkPosition> *wps, int hasStillWork);
	bool assocWork(AGWorkPosition *wp);
	bool isWorking();
	/**
	 * in case of workposition suppression (fired)
	 * called from the workposition when the person leaves the job
	 */
	void loseHisJob();
	/**
	 * when the adult gives up with his job, this function is used
	 * (not the previous one)
	 */
	bool quiteHisJob();
	AGWorkPosition* pickWork(std::vector<AGWorkPosition>* wps);

	/**
	 * returns the location of his work (when employed)
	 */
	AGPosition getWorkLocation();
	/**
	 * returns the work beginning hour and closing hour
	 */
	int getWorkOpening();
	int getWorkClosing();

private:
	AGWorkPosition *work;
	bool employed;
};

#endif

/****************************************************************************/
