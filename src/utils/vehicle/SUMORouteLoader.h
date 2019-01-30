/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

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
    /// @brief constructor
    SUMORouteLoader(SUMORouteHandler* handler);

    /// @brief destructor
    ~SUMORouteLoader();

    /// @brief loads vehicles until a vehicle is read that starts after the specified time
    SUMOTime loadUntil(SUMOTime time);

    /// @brief returns the information whether new data is available
    bool moreAvailable() const;

    /// @brief returns the first departure time that was ever read
    SUMOTime getFirstDepart() const;

private:
    /// @brief the used SAXReader
    SUMOSAXReader* myParser;

    /// @brief flag with information whether more vehicles should be available
    bool myMoreAvailable;

    /// @brief the used Handler
    SUMORouteHandler* myHandler;
};


#endif

/****************************************************************************/

