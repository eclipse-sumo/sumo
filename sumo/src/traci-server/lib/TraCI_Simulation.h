/****************************************************************************/
/// @file    TraCI_Simulation.h
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id: TraCI_POI.h 23594 2017-03-22 07:37:54Z namdre $
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

private:
    /// @brief invalidated standard constructor
    TraCI_Simulation();

    /// @brief invalidated copy constructor
    TraCI_Simulation(const TraCI_Simulation& src);

    /// @brief invalidated assignment operator
    TraCI_Simulation& operator=(const TraCI_Simulation& src);
};
#endif
