/****************************************************************************/
/// @file    MSDevice_Vehroutes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 30.01.2009
/// @version $Id$
///
// A device which collects info on the vehicle trip
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
#ifndef MSDevice_Vehroutes_h
#define MSDevice_Vehroutes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDevice.h"
#include <microsim/MSNet.h>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSEdge;
class MSRoute;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_Vehroutes
 * @brief A device which collects info on the vehicle trip (mainly on departure and arrival)
 *
 * Each device collects departure time, lane and speed and the same for arrival.
 *
 * @see MSDevice
 */
class MSDevice_Vehroutes : public MSDevice {
public:
    /** @brief Static intialization
     */
    static void init() throw(IOError);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a vehroutes-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[in, filled] into The vector to store the built device in
     */
    static MSDevice_Vehroutes* buildVehicleDevices(SUMOVehicle &v, std::vector<MSDevice*> &into, unsigned int maxRoutes=INT_MAX) throw();


public:
    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Does nothing, returns true only if exit times should be collected
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @see MSMoveReminder::notifyEnter
     *
     * @return True, if exit times are to be collected.
     */
    bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason) throw();

    /** @brief Saves exit times if needed
     *
     * The exit time is collected on all occasions except for lane change.
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] reason how the vehicle leaves the lane
     * @see MSMoveReminder::notifyLeave
     *
     * @return True, if exit times are to be collected.
     */
    bool notifyLeave(SUMOVehicle& veh, SUMOReal lastPos, Notification reason) throw();

    /// @}


    /** @brief Called on writing tripinfo output
     *
     * @param[in] os The stream to write the information into
     * @exception IOError not yet implemented
     * @see MSDevice::tripInfoOutput
     */
    void generateOutput() const throw(IOError);


    /** @brief Called on route retrieval
     *
     * @param[in] index The index of the route to retrieve
     * @return the route at the index
     */
    const MSRoute* getRoute(int index) const;


private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_Vehroutes(SUMOVehicle &holder, const std::string &id, unsigned int maxRoutes) throw();


    /// @brief Destructor.
    ~MSDevice_Vehroutes() throw();


    /** @brief Called on route output
     *
     * @param[in] os the device to write into
     * @param[in] index The index of the route to write (-1 writes the current route)
     */
    void writeXMLRoute(OutputDevice &os, int index=-1) const;


    /** @brief Called on route change
     */
    void addRoute();


private:
    /// @brief A shortcut for the Option "vehroute-output.exit-times"
    static bool mySaveExits;

    /// @brief A shortcut for the Option "vehroute-output.last-route"
    static bool myLastRouteOnly;

    /// @brief A shortcut for the Option "device.routing.with-taz"
    static bool myWithTaz;

    class StateListener : public MSNet::VehicleStateListener {
    public:
        /** @brief Called if a vehicle changes its state
         * @param[in] vehicle The vehicle which changed its state
         * @param[in] to The state the vehicle has changed to
         */
        void vehicleStateChanged(const SUMOVehicle * const vehicle, MSNet::VehicleState to) throw();

        /// @brief A map for internal notification
        std::map<const SUMOVehicle*, MSDevice_Vehroutes*> myDevices;
    };

    static StateListener myStateListener;

    /**
     * @class RouteReplaceInfo
     * @brief Information about a replaced route
     *
     * Generated optionally and stored in a vector within the Pointer-CORN-map
     *  this structure contains information about a replaced route: the edge
     *  the route was replaced at by a new one, the time this was done, and
     *  the previous route.
     */
    class RouteReplaceInfo {
    public:
        /// Constructor
        RouteReplaceInfo(const MSEdge * const edge_, const SUMOTime time_, const MSRoute * const route_)
                : edge(edge_), time(time_), route(route_) {}

        /// Destructor
        ~RouteReplaceInfo() { }

        /// The edge the vehicle was on when the route was replaced
        const MSEdge * edge;

        /// The time the route was replaced
        SUMOTime time;

        /// The prior route
        const MSRoute * route;

    };

    const MSRoute* myCurrentRoute;
    std::vector<RouteReplaceInfo> myReplacedRoutes;
    std::vector<SUMOTime> myExits;
    const unsigned int myMaxRoutes;
    const MSEdge* myLastSavedAt;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_Vehroutes(const MSDevice_Vehroutes&);

    /// @brief Invalidated assignment operator.
    MSDevice_Vehroutes& operator=(const MSDevice_Vehroutes&);


};


#endif

/****************************************************************************/

