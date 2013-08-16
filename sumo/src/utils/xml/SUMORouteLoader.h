/****************************************************************************/
/// @file    SUMORouteLoader.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 6 Nov 2002
/// @version $Id$
///
// A class that performs the loading of routes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SUMORouteLoader_h
#define SUMORouteLoader_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMORouteHandler;
class SUMOSAXReader;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class SUMORouteLoader
 */
class SUMORouteLoader {
public:
    /// constructor
    SUMORouteLoader(SUMORouteHandler* handler);

    /// destructor
    ~SUMORouteLoader();

    /** loads vehicles until a vehicle is read that starts after
        the specified time */
    SUMOTime loadUntil(SUMOTime time);

    /// returns the information whether new data is available
    bool moreAvailable() const;

private:
    /// the used SAXReader
    SUMOSAXReader* myParser;

    /// information whether more vehicles should be available
    bool myMoreAvailable;

    /// the used Handler
    SUMORouteHandler* myHandler;

};


#endif

/****************************************************************************/

