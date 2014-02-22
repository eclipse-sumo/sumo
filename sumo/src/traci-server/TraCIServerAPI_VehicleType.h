/****************************************************************************/
/// @file    TraCIServerAPI_VehicleType.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle type values via TraCI
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
#ifndef TraCIServerAPI_VehicleType_h
#define TraCIServerAPI_VehicleType_h


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
#include "TraCIServer.h"
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_VehicleType
 * @brief APIs for getting/setting vehicle type values via TraCI
 */
class TraCIServerAPI_VehicleType {
public:
    /** @brief Processes a get value command (Command 0xa5: Get Vehicle Type Variable)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Processes a value request for the given type
     *
     * @param[in] variable The type variable asked for
     * @param[in] v The vehicle type to get the value from
     * @param[out] tempMsg The storage to write the result to
     */
    static bool getVariable(const int variable, const MSVehicleType& v, tcpip::Storage& tempMsg);


    /** @brief Processes a set value command (Command 0xc5: Change Vehicle Type State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(TraCIServer& server, tcpip::Storage& inputStorage,
                           tcpip::Storage& outputStorage);


    /** @brief Processes a set value for the given type
     */
    static bool setVariable(const int cmd, const int variable,
                            MSVehicleType& v, TraCIServer& server,
                            tcpip::Storage& inputStorage, tcpip::Storage& outputStorage);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_VehicleType(const TraCIServerAPI_VehicleType& s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_VehicleType& operator=(const TraCIServerAPI_VehicleType& s);


};


#endif

#endif

/****************************************************************************/

