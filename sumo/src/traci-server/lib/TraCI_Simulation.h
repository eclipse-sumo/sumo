/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2017 German Aerospace Center (DLR) and others.
/****************************************************************************/
//
//   This program and the accompanying materials
//   are made available under the terms of the Eclipse Public License v2.0
//   which accompanies this distribution, and is available at
//   http://www.eclipse.org/legal/epl-v20.html
//
/****************************************************************************/
/// @file    TraCI_Simulation.h
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
#ifndef TraCI_Simulation_h
#define TraCI_Simulation_h


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


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCI_Simulation
 * @brief C++ TraCI client API implementation
 */
class TraCI_Simulation {
public:
    static SUMOTime getCurrentTime();

    static SUMOTime getDeltaT();

    static TraCIBoundary getNetBoundary();

    static int getMinExpectedNumber();

    static std::string getParameter(const std::string& objectID, const std::string& key);

private:
    /// @brief invalidated standard constructor
    TraCI_Simulation();

    /// @brief invalidated copy constructor
    TraCI_Simulation(const TraCI_Simulation& src);

    /// @brief invalidated assignment operator
    TraCI_Simulation& operator=(const TraCI_Simulation& src);
};
#endif
