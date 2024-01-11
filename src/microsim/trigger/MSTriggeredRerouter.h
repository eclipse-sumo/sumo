/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSTriggeredRerouter.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Mirco Sturari
/// @date    Mon, 25 July 2005
///
// Reroutes vehicles passing an edge
/****************************************************************************/
#pragma once
#include <config.h>

#include <string>
#include <vector>
#include <utils/common/Command.h>
#include <utils/common/Named.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/distribution/RandomDistributor.h>
#include <microsim/MSMoveReminder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class MSRoute;
class SUMOVehicle;
class MSParkingArea;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTriggeredRerouter
 * @brief Reroutes traffic objects passing an edge
 *
 * A rerouter can be positioned on a list of edges and gives traffic objects which
 *  enters one of these edges a new route.
 *
 * The new route may be either chosen from a set of routes where each is
 *  chosen with a certain probability, or newly computed, either by keeping
 *  the old destination or by choosing a new one from a set of existing ones.
 */
class MSTriggeredRerouter :
    public Named, public MSMoveReminder,
    public SUMOSAXHandler {

    friend class GUIEdge; // dynamic instantiation

public:
    /** @brief Constructor
     *
     * @param[in] id The id of the rerouter
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     */
    MSTriggeredRerouter(const std::string& id, const MSEdgeVector& edges,
                        double prob, bool off, bool optional, SUMOTime timeThreshold,
                        const std::string& vTypes, const Position& pos=Position::INVALID);


    /** @brief Destructor */
    virtual ~MSTriggeredRerouter();

    typedef std::pair<MSParkingArea*, bool> ParkingAreaVisible;

    /**
     * @struct RerouteInterval
     * Describes the rerouting definitions valid for an interval
     */
    struct RerouteInterval {
        /// unique ID for this interval
        long long int id;
        /// The begin time these definitions are valid
        SUMOTime begin;
        /// The end time these definitions are valid
        SUMOTime end;
        /// The list of closed edges
        MSEdgeVector closed;
        /// The list of closed lanes
        std::vector<MSLane*> closedLanes;
        /// The list of edges that are affect by closed lanes
        MSEdgeVector closedLanesAffected;
        /// The distributions of new destinations or vias to use
        RandomDistributor<MSEdge*> edgeProbs;
        /// The distributions of new routes to use
        RandomDistributor<ConstMSRoutePtr> routeProbs;
        /// The permissions to use
        SVCPermissions permissions;
        /// The distributions of new parking areas to use as destinations
        RandomDistributor<ParkingAreaVisible> parkProbs;
        /// The edge probs are vias and not destinations
        bool isVia = false;
    };

    /** @brief Tries to reroute the vehicle
     *
     * It will not try to reroute if it is a
     * lane change because there should be another rerouter on the lane
     * the vehicle is coming from.
     * Returns false - the vehicle will not be rerouted again.
     *
     * @param[in] veh The entering vehicle.
     * @param[in] reason how the vehicle enters the lane
     * @return always false (the vehicle will not be rerouted again)
     * @see MSMoveReminder
     * @see MSMoveReminder::notifyEnter
     * @see MSMoveReminder::Notification
     */
    bool notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);

    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Triggers rerouting (once) for vehicles that are already on the edge when the rerouter activates
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed Moving speed.
     * @return True (always).
     */
    bool notifyMove(SUMOTrafficObject& veh, double oldPos, double newPos, double newSpeed);

    /** @brief Removes the reminder
     *
     * @param[in] veh The leaving vehicle.
     * @param[in] lastPos Position on the lane when leaving.
     * @param[in] isArrival whether the vehicle arrived at its destination
     * @param[in] isLaneChange whether the vehicle changed from the lane
     * @return false if the vehicle left th edge
     */
    bool notifyLeave(SUMOTrafficObject& veh, double lastPos, MSMoveReminder::Notification reason, const MSLane* enteredLane = 0);

    bool triggerRouting(SUMOTrafficObject& veh, MSMoveReminder::Notification reason);

    /// Returns the rerouting definition valid for the given time and object, nullptr if none
    const RerouteInterval* getCurrentReroute(SUMOTime time, SUMOTrafficObject& obj) const;

    /// Sets the edge permission if there are any defined in the closingEdge
    SUMOTime setPermissions(const SUMOTime currentTime);

    /// Returns the rerouting definition valid for the given time, 0 if none
    const RerouteInterval* getCurrentReroute(SUMOTime time) const;

    /// Sets whether the process is currently steered by the user
    void setUserMode(bool val);

    /// Sets the probability with which a vehicle is rerouted given by the user
    void setUserUsageProbability(double prob);

    /// Returns whether the user is setting the rerouting probability
    bool inUserMode() const;

    /// Returns the rerouting probability
    double getProbability() const;

    /// Returns the rerouting probability given by the user
    double getUserProbability() const;

    // @brief return whether this moveReminder triggers parking reroute
    bool isParkingRerouter() const {
        return myHaveParkProbs;
    }

    const Position& getPosition() {
        return myPosition;
    }

    static double getWeight(SUMOVehicle& veh, const std::string param, const double defaultWeight);

    static MSParkingArea* rerouteParkingArea(const MSTriggeredRerouter::RerouteInterval* rerouteDef,
            SUMOVehicle& veh, bool& newDestination, ConstMSEdgeVector& newRoute);

    /// @brief return all rerouter instances
    static const std::map<std::string, MSTriggeredRerouter*>& getInstances() {
        return myInstances;
    }

    /// @brief issues warning for incomplete parkingReroute relationships
    static void checkParkingRerouteConsistency();

protected:
    /// @name inherited from GenericSAXHandler
    //@{

    /** @brief Called on the opening of a tag;
     *
     * @param[in] element ID of the currently opened element
     * @param[in] attrs Attributes within the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myStartElement
     */
    virtual void myStartElement(int element,
                                const SUMOSAXAttributes& attrs);


    /** @brief Called when a closing tag occurs
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    virtual void myEndElement(int element);
    //@}

    /** @brief Checks whether the detector measures objects of the given type.
    *
    * @param[in] obj the traffic object of which the type is checked.
    * @return whether it should be measured
    */
    bool applies(const SUMOTrafficObject& obj) const;

    static bool affected(const std::set<SUMOTrafficObject::NumericalID>& edgeIndices, const MSEdgeVector& closed);

    typedef std::map<std::string, double> ParkingParamMap_t;
    typedef std::map<MSParkingArea*, ParkingParamMap_t, ComparatorIdLess> MSParkingAreaMap_t;

    /// determine attributes of candiate parking area for scoring
    static bool addParkValues(SUMOVehicle& veh, double brakeGap, bool newDestination,
                              MSParkingArea* pa, double paOccupancy, double prob,
                              SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
                              MSParkingAreaMap_t& parkAreas,
                              std::map<MSParkingArea*, ConstMSEdgeVector>& newRoutes,
                              std::map<MSParkingArea*, ConstMSEdgeVector>& parkApproaches,
                              ParkingParamMap_t& maxValues);

protected:
    /// @brief edges where vehicles are notified
    const MSEdgeVector myEdges;

    /// List of rerouting definition intervals
    std::vector<RerouteInterval> myIntervals;

    /// The probability and the user-given probability
    double myProbability, myUserProbability;

    /// Information whether the current rerouting probability is the user-given
    bool myAmInUserMode;

    /// Information whether the rerouting will only take place on request
    bool myAmOptional;

    /// Where are we located in the network
    Position myPosition;

    // @brief waiting time threshold for activation
    SUMOTime myTimeThreshold;

    /// @brief The vehicle types to look for (empty means all)
    std::set<std::string> myVehicleTypes;

    /// used during loading
    RerouteInterval myParsedRerouteInterval;

    /// whether this rerouter has loaded parkingReroute definitions
    bool myHaveParkProbs;

    /// @brief special destination values
    static MSEdge mySpecialDest_keepDestination;
    static MSEdge mySpecialDest_terminateRoute;

    static std::map<std::string, MSTriggeredRerouter*> myInstances;

private:
    /// @brief Invalidated copy constructor.
    MSTriggeredRerouter(const MSTriggeredRerouter&);

    /// @brief Invalidated assignment operator.
    MSTriggeredRerouter& operator=(const MSTriggeredRerouter&);


};
