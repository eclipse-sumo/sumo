/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Junction.h
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    30.05.2012
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef Junction_h
#define Junction_h


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
class MSJunction;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Junction
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class Junction {
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

    /** @brief Returns a tree filled with junction instances
     * @return The rtree of junctions
     */
    static NamedRTree* getTree();

private:
    /// @brief invalidated standard constructor
    Junction();

    /// @brief invalidated copy constructor
    Junction(const Junction& src);

    /// @brief invalidated assignment operator
    Junction& operator=(const Junction& src);
};
}


#endif

/****************************************************************************/
