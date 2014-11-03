/****************************************************************************/
/// @file    TraCIServerAPI_Vehicle.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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
#ifndef TraCIServerAPI_Vehicle_h
#define TraCIServerAPI_Vehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#ifndef NO_TRACI

#include "TraCIException.h"
#include <microsim/MSEdgeWeightsStorage.h>
#include "TraCIServer.h"
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_Vehicle
 * @brief APIs for getting/setting vehicle values via TraCI
 */
class TraCIServerAPI_Vehicle {
public:
    /** @brief Processes a get value command (Command 0xa4: Get Vehicle Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Processes a set value command (Command 0xc4: Change Vehicle State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Returns the named vehicle's position
     * @param[in] id The id of the searched vehicle
     * @param[out] p The position, if the vehicle is on the network
     * @return Whether the vehicle is known (and on road)
     */
    static bool getPosition(const std::string& id, Position& p);


private:
    static bool commandDistanceRequest(TraCIServer& server, tcpip::Storage& inputStorage,
                                       tcpip::Storage& outputStorage, const MSVehicle* v);

    static MSVehicleType& getSingularType(SUMOVehicle* const veh);


    static const std::map<std::string, std::vector<MSLane*> >& getOrBuildVTDMap();
    static bool vtdMap_matchingEdgeLane(const Position& pos, const std::string& origID, MSVehicle& v, bool report,
                                        SUMOReal& bestDistance, MSLane** lane, SUMOReal& lanePos, int& routeOffset, MSEdgeVector& edges);
    static bool vtdMap_matchingRoutePosition(const Position& pos, const std::string& origID, MSVehicle& v, bool report,
            SUMOReal& bestDistance, MSLane** lane, SUMOReal& lanePos, int& routeOffset, MSEdgeVector& edges);
    static bool vtdMap_matchingNearest(const Position& pos, const std::string& origID, MSVehicle& v, TraCIServer& server, bool report,
                                       SUMOReal& bestDistance, MSLane** lane, SUMOReal& lanePos, int& routeOffset, MSEdgeVector& edges);

    static std::map<std::string, std::vector<MSLane*> > gVTDMap;


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Vehicle(const TraCIServerAPI_Vehicle& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Vehicle& operator=(const TraCIServerAPI_Vehicle& s);


};


#endif

#endif

/****************************************************************************/
