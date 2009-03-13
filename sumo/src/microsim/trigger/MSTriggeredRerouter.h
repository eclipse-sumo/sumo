/****************************************************************************/
/// @file    MSTriggeredRerouter.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// Reroutes vehicles passing an edge
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
#ifndef MSTriggeredRerouter_h
#define MSTriggeredRerouter_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/common/Command.h>
#include <microsim/MSMoveReminder.h>
#include "MSTrigger.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/RandomDistributor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTriggeredRerouter
 * @brief Reroutes vehicles passing an edge
 *
 * A rerouter can be positioned on a list of edges and gives vehicles which
 *  arrive one of these edges a new route.
 *
 * The new route may be either chosen from a set of routes where each is
 *  chosen with a certain probability, or newly computed, either by keeping
 *  the old destination or by choosing a new one from a set of existing ones.
 */
class MSTriggeredRerouter :
        public MSTrigger,
            public SUMOSAXHandler {
public:
    /** @brief Constructor
     *
     * @param[in] id The id of the rerouter
     * @param[in] edges The edges the rerouter is placed at
     * @param[in] prob The probability the rerouter reoutes vehicles with
     * @param[in] file The file to read the reroute definitions from
     */
    MSTriggeredRerouter(const std::string &id,
                        const std::vector<MSEdge*> &edges,
                        SUMOReal prob, const std::string &file, bool off);


    /** @brief Destructor */
    virtual ~MSTriggeredRerouter() throw();


    /**
     * @class Setter
     * @brief Responsible for setting a new route to a vehicle which arrives at a single lane
     */
    class Setter : public MSMoveReminder {
    public:
        /** @brief Constructor
         *
         * @param[in] parent The rerouter responsible for this actor
         * @param[in] lane The lane this actor is placed at
         */
        Setter(MSTriggeredRerouter * const parent, MSLane * const lane) throw();


        /// @brief Destructor
        ~Setter() throw();


        /// @name Methods inherited from MSMoveReminder
        /// @{

        /** @brief Tries to reroute the vehicle
         *
         * Calls parent's "reroute"-method with the vehicle as one of the
         *  parameters. Returns false - the vehicle will not be rerouted again.
         *
         * @param[in] veh The regarded vehicle
         * @param[in] oldPos Position before the move-micro-timestep.
         * @param[in] newPos Position after the move-micro-timestep.
         * @param[in] newSpeed The vehicle's current speed
         * @return always false (the vehicle will not be rerouted again)
         * @see MSMoveReminder
         * @see MSMoveReminder::isStillActive
         * @see MSTriggeredRerouter::reroute
         */
        bool isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos,
                           SUMOReal newSpeed) throw();


        /** @brief Nothing is done here
         *
         * @param[in] veh The leaving vehicle.
         * @see MSMoveReminder::dismissByLaneChange
         */
        void dismissByLaneChange(MSVehicle& veh) throw();


        /** @brief Tries to reroute the vehicle
         *
         * Calls parent's "reroute"-method with the vehicle as one of the
         *  parameters. Returns false - the vehicle will not be rerouted again.
         *
         * @param[in] veh The entering vehicle.
         * @param[in] isEmit true means emit, false: lane change
         * @return always false (the vehicle will not be rerouted again)
         * @see MSMoveReminder
         * @see MSMoveReminder::isActivatedByEmitOrLaneChange
         * @see MSTriggeredRerouter::reroute
         */
        bool isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw();
        /// @}


    private:
        /// @brief The rerouter used for rerouting the vehicle
        MSTriggeredRerouter * const myParent;

    private:
        /// @brief Invalidated copy constructor.
        Setter(const Setter&);

        /// @brief Invalidated assignment operator.
        Setter& operator=(const Setter&);


    };

    /**
     * @struct RerouteInterval
     * Describes the rerouting definitions valid for an interval
     */
    struct RerouteInterval {
        /// The begin time these definitions are valid
        SUMOTime begin;
        /// The end time these definitions are valid
        SUMOTime end;
        /// The list of closed edges
        std::vector<MSEdge*> closed;
        /// The distributions of new destinations to use
        RandomDistributor<MSEdge*> edgeProbs;
        /// The distributions of new routes to use
        RandomDistributor<const MSRoute*> routeProbs;
    };

    /// Reroutes a vehicle
    void reroute(SUMOVehicle &veh, const MSEdge *src);

    /// Returns whether a rerouting definition is valid for the given time and vehicle
    bool hasCurrentReroute(SUMOTime time, SUMOVehicle &veh) const;

    /// Returns the rerouting definition valid for the given time and vehicle
    const RerouteInterval &getCurrentReroute(SUMOTime time, SUMOVehicle &veh) const;

    /// Returns whether a rerouting definition is valid for the given time
    bool hasCurrentReroute(SUMOTime time) const;

    /// Returns the rerouting definition valid for the given time and vehicle
    const RerouteInterval &getCurrentReroute(SUMOTime time) const;

    /// Sets whether the process is currently steered by the user
    void setUserMode(bool val);

    /// Sets the probability with which a vehicle is rerouted given by the user
    void setUserUsageProbability(SUMOReal prob);

    /// Returns whether the user is setting the rerouting probability
    bool inUserMode() const;

    /// Returns the rerouting probability
    SUMOReal getProbability() const;

    /// Returns the rerouting probability given by the user
    SUMOReal getUserProbability() const;

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
    virtual void myStartElement(SumoXMLTag element,
                                const SUMOSAXAttributes &attrs) throw(ProcessError);


    /** @brief Called when a closing tag occures
     *
     * @param[in] element ID of the currently opened element
     * @exception ProcessError If something fails
     * @see GenericSAXHandler::myEndElement
     */
    void myEndElement(SumoXMLTag element) throw(ProcessError);
    //@}

protected:
    /// List of lane-based vehicle informing children
    std::vector<Setter*> mySetter;

    /// List of rerouting definition intervals
    std::vector<RerouteInterval> myIntervals;

    /// The probability and the user-given probability
    SUMOReal myProbability, myUserProbability;

    /// Information whether the current rerouting probability is the user-given
    bool myAmInUserMode;

    /// @name members used during loading
    //@{

    /// The first and the last time steps of the interval
    SUMOTime myCurrentIntervalBegin, myCurrentIntervalEnd;
    /// List of closed edges
    std::vector<MSEdge*> myCurrentClosed;
    /// new destinations with probabilities
    RandomDistributor<MSEdge*> myCurrentEdgeProb;
    /// new routes with probabilities
    RandomDistributor<const MSRoute*> myCurrentRouteProb;
    //@}

private:
    /// @brief Invalidated copy constructor.
    MSTriggeredRerouter(const MSTriggeredRerouter&);

    /// @brief Invalidated assignment operator.
    MSTriggeredRerouter& operator=(const MSTriggeredRerouter&);


};


#endif

/****************************************************************************/

