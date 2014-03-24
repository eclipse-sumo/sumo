/****************************************************************************/
/// @file    SUMORouteLoaderControl.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 06 Nov 2002
/// @version $Id$
///
// Class responsible for loading of routes from some files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMORouteLoaderControl_h
#define SUMORouteLoaderControl_h


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
class SUMORouteLoader;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMORouteLoaderControl
 *
 * SUMORouteLoaderControl
 * This controls is initialised with the list of route loaders and uses them
 * to load routes step wise.
 * The parameter myInAdvanceStepNo holds the number of time steps to read the
 * routes in forward. If it is 0 (default), all routes will be read at once.
 */
class SUMORouteLoaderControl {
public:
    /// constructor
    SUMORouteLoaderControl(SUMOTime inAdvanceStepNo);

    /// destructor
    ~SUMORouteLoaderControl();

    /// add another loader
    void add(SUMORouteLoader* loader);

    /// loads the next routes
    void loadNext(SUMOTime step);

    /// returns the timestamp of the first loaded vehicle or flow
    SUMOTime getFirstLoadTime() const {
        return myFirstLoadTime;
    }

private:
    /// the first time step for which vehicles were loaded
    SUMOTime myFirstLoadTime;

    /// the number of routes to read in forward
    SUMOTime myInAdvanceStepNo;

    /// the list of route loaders
    std::vector<SUMORouteLoader*> myRouteLoaders;

    /** information whether all routes shall be loaded and whether
        they were loaded */
    bool myLoadAll, myAllLoaded;
};


#endif

/****************************************************************************/

