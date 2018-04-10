/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Route.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef Route_h
#define Route_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <libsumo/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSRoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Route
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class Route {
public:

    static std::vector<std::string> getIDList();
    static int getIDCount();
    static std::vector<std::string> getEdges(const std::string& routeID);
    static std::string getParameter(const std::string& routeID, const std::string& param);

    static void add(const std::string& routeID, const std::vector<std::string>& edgeIDs);
    static void setParameter(const std::string& routeID, const std::string& key, const std::string& value); // not needed so far

    static void subscribe(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars);
    static void subscribeContext(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars);

    static const MSRoute* getRoute(const std::string& id);

private:
    /// @brief invalidated standard constructor
    Route();

    /// @brief invalidated copy constructor
    Route(const Route& src);

    /// @brief invalidated assignment operator
    Route& operator=(const Route& src);
};
}


#endif

/****************************************************************************/
