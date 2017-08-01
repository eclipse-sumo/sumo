/****************************************************************************/
/// @file    TraCI_Junction.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
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
#ifndef TraCI_Junction_h
#define TraCI_Junction_h


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
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_Junction
 * @brief C++ TraCI client API implementation
 */
class TraCI_Junction {
public:


    static std::vector<std::string> getIDList();
    static int getIDCount();
    static TraCIPosition getPosition(const std::string& junctionID);
    static TraCIPositionVector getShape(const std::string& junctionID);


    static std::string getType(const std::string& poiID);
    static TraCIColor getColor(const std::string& poiID);


    static void subscribe(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, const std::vector<int>& vars);
    static void subscribeContext(const std::string& objID, SUMOTime beginTime, SUMOTime endTime, int domain, double range, const std::vector<int>& vars);
    static MSJunction* getJunction(const std::string& id);

private:
    /// @brief invalidated standard constructor
    TraCI_Junction();

    /// @brief invalidated copy constructor
    TraCI_Junction(const TraCI_Junction& src);

    /// @brief invalidated assignment operator
    TraCI_Junction& operator=(const TraCI_Junction& src);
};


#endif

/****************************************************************************/

