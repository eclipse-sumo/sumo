/****************************************************************************/
/// @file    MSPersonControl.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Stores all persons in the net and handles their waiting for cars.
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSPersonControl_h
#define MSPersonControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>


// ===========================================================================
// class declarations
// ===========================================================================
class MSPerson;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 * @class MSPersonControl
 * The class is used to handle persons who are not using a transportation
 *  system but are walking or waiting. Both is processed by waiting
 *  for the arrival time / the time the waiting is over.
 */
class MSPersonControl {
public:

    typedef std::vector<MSPerson*> PersonVector;

    /// constructor
    MSPersonControl();

    /// destructor
    ~MSPersonControl();

    /// adds a single person, returns false iff an id clash occured
    bool add(const std::string &id, MSPerson *person);

    /// removes a single person
    void erase(MSPerson *person);

    /// sets the arrival time for a waiting or walking person
    void setArrival(SUMOTime time, MSPerson *person);

    /// returns whether any persons waiting or walking time is over
    bool hasArrivedPersons(SUMOTime time) const;

    /// returns the list of persons which waiting or walking period is over
    const PersonVector popArrivedPersons(SUMOTime time);

    /// adds a person to the list of persons waiting for a vehicle on the specified edge
    void addWaiting(const MSEdge* edge, MSPerson *person) throw();

    /// checkls whether a person waits for the specified vehicle and if so removes it from the list
    void checkWaiting(const MSEdge* edge, MSVehicle *vehicle) throw();


private:
    /// all persons by id
    std::map<std::string, MSPerson*> myPersons;

    /// the lists of walking / stopping persons
    std::map<SUMOTime, PersonVector> myArrivals;

    /// the lists of waiting persons
    std::map<const MSEdge*, PersonVector> myWaiting;

};


#endif

/****************************************************************************/
