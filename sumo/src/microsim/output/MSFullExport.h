/****************************************************************************/
/// @file    MSFullExport.h
/// @author  Mario Krumnow
///
// Dumping a hugh List of Parameters available in the Simulation
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
#ifndef MSFullExport_h
#define MSFullExport_h


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
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSFullExport
 * @brief Dumping a hugh List of Parameters available in the Simulation
 *
 *  The class offers a static method, which writes a hugh List of Parameters
 *  available in the Simulation into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSFullExport {
public:
    /**@brief Dumping a hugh List of Parameters available in the Simulation
    *
    *  The class offers a static method, which writes a hugh List of Parameters
    *  available in the Simulation into the given OutputDevice.
    *
    * @param[in] of The output device to use
    * @param[in] timestep The current time step
    * @exception IOError If an error on writing occurs (!!! not yet implemented)
    */
    static void write(OutputDevice& of, SUMOTime timestep);


private:
    /// @brief Invalidated copy constructor.
    MSFullExport(const MSFullExport&);

    /// @brief Invalidated assignment operator.
    MSFullExport& operator=(const MSFullExport&);

    /// @brief Writes the XML Nodes for the vehicles (e.g. speed, position, emissions)
    static void writeVehicles(OutputDevice& of);

    /// @brief Writes the XML Nodes for the edges (e.g. traveltime)
    static void writeEdge(OutputDevice& of);

    /// @brief Writes the XML Nodes for the lanes (e.g. emissions, occupancy)
    static void writeLane(OutputDevice& of, const MSLane& lane);

    /// @brief Writes the XML Nodes for the traffic lights (e.g. actual state)
    static void writeTLS(OutputDevice& of, SUMOTime timestep);

};



#endif

/****************************************************************************/

