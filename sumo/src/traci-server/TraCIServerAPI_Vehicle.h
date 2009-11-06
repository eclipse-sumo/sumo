/****************************************************************************/
/// @file    TraCIServerAPI_Vehicle.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_Vehicle.h 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting vehicle values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) throw(traci::TraCIException);


    /** @brief Processes a set value command (Command 0xc4: Change Vehicle State)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) throw(traci::TraCIException);

private:
    /** @class EdgeWeightsProxi
     * @brief A proxi for edge weights known by a vehicle/known globally
     *
     * Both getter methods try to return the vehicle's knowledge about the edge/time, first. 
     *  If not existing,they try to retrieve it from the global knowledge. If not existing, 
     *  the travel time retrieval method returns the edges' length divided by the maximum speed
     *  (information from the first lane is used). The default value for the effort is 0.
     * @see MSEdgeWeightsStorage
     */
    class EdgeWeightsProxi {
    public:
        /** @brief Constructor
         * @param[in] vehKnowledge The vehicle's edge weights knowledge
         * @param[in] netKnowledge The global edge weights knowledge
         */
        EdgeWeightsProxi(const MSEdgeWeightsStorage &vehKnowledge, 
            const MSEdgeWeightsStorage &netKnowledge) 
            : myVehicleKnowledge(vehKnowledge), myNetKnowledge(netKnowledge) {}


        /// @brief Destructor
        ~EdgeWeightsProxi() {}


        /** @brief Returns the travel time to pass an edge
         * @param[in] e The edge for which the travel time to be passed shall be returned
         * @param[in] v The vehicle that is rerouted
         * @param[in] t The time for which the travel time shall be returned
         * @return The travel time for an edge
         * @see DijkstraRouterTT_ByProxi
         */
        SUMOReal getTravelTime(const MSEdge * const e, const SUMOVehicle * const v, SUMOTime t) const;


        /** @brief Returns the effort to pass an edge
         * @param[in] e The edge for which the effort to be passed shall be returned
         * @param[in] v The vehicle that is rerouted
         * @param[in] t The time for which the effort shall be returned
         * @return The effort (abstract) for an edge
         * @see DijkstraRouterTT_ByProxi
         */
        SUMOReal getEffort(const MSEdge * const e, const SUMOVehicle * const v, SUMOTime t) const;

    private:
        /// @brief The vehicle's knownledge
        const MSEdgeWeightsStorage &myVehicleKnowledge;

        /// @brief The global knownledge
        const MSEdgeWeightsStorage &myNetKnowledge;

    };

private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Vehicle(const TraCIServerAPI_Vehicle &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Vehicle &operator=(const TraCIServerAPI_Vehicle &s);


};


#endif

/****************************************************************************/

