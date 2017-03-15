/****************************************************************************/
/// @file    TraCI_Route.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id: TraCI_Route.h 23414 2017-03-15 11:56:48Z rhilbrich $
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCI_Route_h
#define TraCI_Route_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <traci-server/TraCIDefs.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSRoute;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_Route
 * @brief C++ TraCI client API implementation
 */
class TraCI_Route {
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
    TraCI_Route();

    /// @brief invalidated copy constructor
    TraCI_Route(const TraCI_Route& src);

    /// @brief invalidated assignment operator
    TraCI_Route& operator=(const TraCI_Route& src);
};


#endif

/****************************************************************************/

