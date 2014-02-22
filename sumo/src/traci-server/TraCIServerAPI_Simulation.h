/****************************************************************************/
/// @file    TraCIServerAPI_Simulation.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting edge values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIServerAPI_Simulation_h
#define TraCIServerAPI_Simulation_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include <string>
#include <map>
#include <vector>
#include "TraCIException.h"
#include "TraCIServer.h"
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_Simulation
 * @brief APIs for getting/setting simulation values via TraCI
 */
class TraCIServerAPI_Simulation {
public:
    /** @brief Processes a get value command (Command 0xab: Get Simulation Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);

    /** @brief Processes a set value command (Command 0xcb: Set Simulation Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /**
     * Converts a cartesian position to the closest road map position
     *
     * @param pos cartesian position that is to be converted
     * @return the closest road map position to the cartesian position
     */
    static std::pair<MSLane*, SUMOReal> convertCartesianToRoadMap(Position pos);

    /**
     * Converts a road map position to a cartesian position
     *
     * @param pos road map position that is to be convertes
     * @return closest 2D position
     */
    static const MSLane* getLaneChecking(std::string roadID, int laneIndex, SUMOReal pos);

    static bool commandPositionConversion(TraCIServer& server, tcpip::Storage& inputStorage,
                                          tcpip::Storage& outputStorage, int commandId);

    static bool commandDistanceRequest(TraCIServer& server, tcpip::Storage& inputStorage,
                                       tcpip::Storage& outputStorage, int commandId);

    static void writeVehicleStateNumber(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state);
    static void writeVehicleStateIDs(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state);

private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Simulation(const TraCIServerAPI_Simulation& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Simulation& operator=(const TraCIServerAPI_Simulation& s);


};


#endif

#endif

/****************************************************************************/

