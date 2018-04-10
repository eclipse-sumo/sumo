/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    TraCIServerAPI_Simulation.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting edge values via TraCI
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

#include <string>
#include <map>
#include <vector>
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
    static std::pair<MSLane*, double> convertCartesianToRoadMap(Position pos);

    /**
     * Converts a road map position to a cartesian position
     *
     * @param pos road map position that is to be convertes
     * @return closest 2D position
     */

    static bool commandPositionConversion(TraCIServer& server, tcpip::Storage& inputStorage,
                                          tcpip::Storage& outputStorage, int commandId);

    static bool commandDistanceRequest(TraCIServer& server, tcpip::Storage& inputStorage,
                                       tcpip::Storage& outputStorage, int commandId);

private:
    static void writeVehicleStateNumber(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state);
    static void writeVehicleStateIDs(TraCIServer& server, tcpip::Storage& outputStorage, MSNet::VehicleState state);
    static void writeStage(tcpip::Storage& outputStorage, const libsumo::TraCIStage& stage);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Simulation(const TraCIServerAPI_Simulation& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Simulation& operator=(const TraCIServerAPI_Simulation& s);


};


#endif

/****************************************************************************/

