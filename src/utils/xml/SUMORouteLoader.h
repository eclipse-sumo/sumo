/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMORouteLoader.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Wed, 6 Nov 2002
/// @version $Id$
///
// A class that performs the loading of routes
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

    /// returns the first departure time that was ever read
    SUMOTime getFirstDepart() const;

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

