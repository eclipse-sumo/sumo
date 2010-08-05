/****************************************************************************/
/// @file    TraCIServerAPI_Vehicle.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting vehicle values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
    static bool processGet(traci::TraCIServer &server, tcpip::Storage &inputStorage, 
		tcpip::Storage &outputStorage);


    /** @brief Processes a set value command (Command 0xc4: Change Vehicle State)
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(traci::TraCIServer &server, tcpip::Storage &inputStorage, 
		tcpip::Storage &outputStorage);


	/// @name "old" API functions
	/// @{

    /** @brief processes command setMaximumSpeed
	 *
     * This command causes the node given by nodeId to limit its speed to a maximum speed (float).
     * If maximum speed is set to a negative value, the individual speed limit for that node gets annihilated.
     *
     * @param[in] server The TraCI-server-instance which schedules this request
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
	 */
	static bool commandSetMaximumSpeed(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    static bool commandStopNode(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    static bool commandChangeLane(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    static bool commandChangeRoute(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    static bool commandChangeTarget(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

	static bool commandSlowDown(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);


    static bool commandSubscribeLifecycles(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    static bool commandUnsubscribeLifecycles(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    static bool commandSubscribeDomain(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);

    static bool commandUnsubscribeDomain(traci::TraCIServer &server, tcpip::Storage &inputStorage, tcpip::Storage &outputStorage);
	static void checkReroute(MSVehicle *veh) throw();
	/// @}




private:
	static std::set<MSVehicle*> myVehiclesToReroute;
    // holds all Domain Ids to whose objects' lifecycle the client subscribed
    static std::set<int> myLifecycleSubscriptions;

    // holds all Domain Ids to whose objects the client subscribed, along with the variable/type pairs the client is subscribed to
    static std::map<int, std::list<std::pair<int, int> > > myDomainSubscriptions;




private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Vehicle(const TraCIServerAPI_Vehicle &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Vehicle &operator=(const TraCIServerAPI_Vehicle &s);


};


#endif

/****************************************************************************/

