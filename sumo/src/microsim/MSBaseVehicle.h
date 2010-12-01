/****************************************************************************/
/// @file    MSBaseVehicle.h
/// @author  Michael Behrisch
/// @date    Mon, 8 Nov 2010
/// @version $Id$
///
// A base class for vehicle implementations
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
#ifndef MSBaseVehicle_h
#define MSBaseVehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <cassert>
#include <vector>
#include <utils/common/SUMOVehicle.h>
#include <utils/common/StdDefs.h>
#include "MSRoute.h"


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVehicleParameter;
class MSVehicleType;
class MSMoveReminder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBaseVehicle
 * @brief The base class for microscopic and mesoscopic vehicles
 */
class MSBaseVehicle : public SUMOVehicle {
public:
    /** @brief Constructor
     */
    MSBaseVehicle(SUMOVehicleParameter* pars, const MSRoute* route, const MSVehicleType* type) throw(ProcessError);


    /// @brief Destructor
    virtual ~MSBaseVehicle() throw();


    /// Returns the name of the vehicle
    const std::string &getID() const throw();

    /** @brief Returns the vehicle's parameter (including departure definition)
     *
     * @return The vehicle's parameter
     */
    const SUMOVehicleParameter &getParameter() const throw();


    /** @brief Returns the current route
     * @return The route the vehicle uses
     */
    const MSRoute &getRoute() const throw();


    /** @brief Returns the vehicle's type definition
     * @return The vehicle's type definition
     */
    const MSVehicleType &getVehicleType() const throw();


    /** @brief Returns the current maximum speed
     * @return The vehicle's maximum speed
     */
    SUMOReal getMaxSpeed() const;


    /** @brief Adapts the current maximum speed using the distribution given in the type
     * @return The vehicle's new maximum speed
     */
    SUMOReal adaptMaxSpeed(SUMOReal referenceSpeed);


    /** @brief Returns the nSuccs'th successor of edge the vehicle is currently at
     *
     * If the rest of the route (counted from the current edge) has less than nSuccs edges,
     *  0 is returned.
     * @param[in] nSuccs The number of edge to look forward
     * @return The nSuccs'th following edge in the vehicle's route
     */
    const MSEdge* succEdge(unsigned int nSuccs) const throw();

    /** @brief Returns the edge the vehicle is currently at
     *
     * @return The current edge in the vehicle's route
     */
    const MSEdge * const getEdge() const;


    /** @brief Performs a rerouting using the given router
     *
     * Tries to find a new route between the current edge and the destination edge, first.
     * Tries to replace the current route by the new one using replaceRoute.
     *
     * @param[in] t The time for which the route is computed
     * @param[in] router The router to use
     * @see replaceRoute
     */
    void reroute(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle> &router, bool withTaz=false) throw();


    /** @brief Replaces the current route by the given edges
     *
     * It is possible that the new route is not accepted, if a) it does not
     *  contain the vehicle's current edge, or b) something fails on insertion
     *  into the routes container (see in-line comments).
     *
     * @param[in] edges The new list of edges to pass
     * @param[in] simTime The time at which the route was replaced
     * @return Whether the new route was accepted
     */
    bool replaceRouteEdges(const MSEdgeVector &edges, bool onInit=false) throw();


    /** @brief Returns the vehicle's acceleration before dawdling
     *
     * This default implementation returns always 0.
     *
     * @return The acceleration before dawdling
     */
    virtual SUMOReal getPreDawdleAcceleration() const throw();

    /** @brief Called when the vehicle is inserted into the network
     *
     * Sets optional information about departure time, informs the vehicle
     *  control about a further running vehicle.
     */
    void onDepart() throw();

    /** @brief Returns this vehicle's real departure time
     * @return This vehicle's real departure time
     */
    SUMOTime getDeparture() const throw();

    /** @brief Returns the number of new routes this vehicle got
     * @return the number of new routes this vehicle got
     */
    unsigned int getNumberReroutes() const throw();

    /** @brief Returns this vehicle's devices
     * @return This vehicle's devices
     */
    const std::vector<MSDevice*> &getDevices() const {
        return myDevices;
    }

    /** @brief Adds a person to this vehicle
     *
     * The default implementation does nothing since persons are not supported by default
     *
     * @param[in] person The person to add
     */
    virtual void addPerson(MSPerson* person) throw();

    /** @brief Returns whether the vehicle is at a stop
     * @return Whether it has stopped
     */
    virtual bool isStopped() const;

    /** @brief Adds a MoveReminder dynamically
     *
     * @see MSMoveReminder
     */
    void addReminder(MSMoveReminder* rem) throw();

protected:
    /// @brief This Vehicle's parameter.
    const SUMOVehicleParameter* myParameter;

    /// @brief This Vehicle's route.
    const MSRoute* myRoute;

    /// @brief This Vehicle's type.
    const MSVehicleType *myType;

    /// @brief Iterator to current route-edge
    MSRouteIterator myCurrEdge;

    /// An individual speed for an vehicle that is used (iff set) instead of
    /// the maximal speed of the vehicle class.
    /// NOTE: This is just a little workaround for allowing an external
    ///       influence on the actual speed
    SUMOReal myIndividualMaxSpeed;

    /// is true if there has an individual speed been set
    bool myHasIndividualMaxSpeed;

    /// the speed which served as reference when calculating the individual maxspeed
    SUMOReal myReferenceSpeed;

    /// @name Move reminder structures
    /// @{

    /// @brief Definition of a move reminder container
    typedef std::vector< std::pair<MSMoveReminder*, SUMOReal> > MoveReminderCont;

    /// @brief Current lane's move reminder
    MoveReminderCont myMoveReminders;
    /// @}

    /// @brief The devices this vehicle has
    std::vector<MSDevice*> myDevices;

    /// @brief The real departure time
    SUMOTime myDeparture;

    /// the position on the destination lane where the vehicle stops
    SUMOReal myArrivalPos;

    /// @brief The number of reroutings
    unsigned int myNumberReroutes;

};

#endif

/****************************************************************************/
