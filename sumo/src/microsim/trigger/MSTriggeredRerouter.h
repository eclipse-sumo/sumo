/****************************************************************************/
/// @file    MSTriggeredRerouter.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 25 July 2005
/// @version $Id$
///
// Allows the triggered rerouting of vehicles within the simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <utils/helpers/Command.h>
#include <microsim/MSMoveReminder.h>
#include "MSTrigger.h"
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/helpers/RandomDistributor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSNet;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTriggeredRerouter
 * A rerouter can be positioned on a list of edges and gives vehicles which
 *  arrive one of these edges a new route.
 * The new route may be either chosen from a set of routes where each is
 *  chosen with a certain probability, or newly computed, either by keeping
 *  the old destination or by choosing a new one from a set of existing ones.
 */
class MSTriggeredRerouter :
            public MSTrigger,
            public SUMOSAXHandler
{
public:
    /** constructor */
    MSTriggeredRerouter(const std::string &id,
                        const std::vector<MSEdge*> &edges,
                        SUMOReal prob, const std::string &aXMLFilename);

    /** destructor */
    virtual ~MSTriggeredRerouter();

    /**
     * @class Setter
     * Responsible for setting a new route to a vehicle which arrives a single lane
     */
class Setter : public MSMoveReminder
    {
    public:
        /// Constructor
        Setter(MSTriggeredRerouter *parent, MSLane *lane);

        /// Destructor
        ~Setter();

        /// Returns whether the vehicle has to be aware of this setter in the next step
        bool isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos,
                           SUMOReal newSpeed);

        /// Removes from the list of move reminders to be aware of
        void dismissByLaneChange(MSVehicle& veh);

        /// Returns whether the vehicle shall be aware of this move reminder
        bool isActivatedByEmitOrLaneChange(MSVehicle& veh);

    private:
        /// The rerouter used for rerouting the vehicle
        MSTriggeredRerouter *myParent;

    };

    /**
     * @struct RerouteInterval
     * Describes the rerouting definitions valid for an interval
     */
    struct RerouteInterval
    {
        /// The begin time these definitions are valid
        SUMOTime begin;
        /// The end time these definitions are valid
        SUMOTime end;
        /// The list of closed edges
        std::vector<MSEdge*> closed;
        /// The distributions of new destinations to use
        RandomDistributor<MSEdge*> edgeProbs;
        /// The distributions of new routes to use
        RandomDistributor<MSRoute*> routeProbs;
    };

    /// Reroutes a vehicle
    void reroute(MSVehicle &veh, const MSEdge *src);

    /// Returns whether a rerouting definition is valid for the given time and vehicle
    bool hasCurrentReroute(SUMOTime time, MSVehicle &veh) const;

    /// Returns the rerouting definition valid for the given time and vehicle
    const RerouteInterval &getCurrentReroute(SUMOTime time, MSVehicle &veh) const;

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
    /** the implementation of the SAX-handler interface for reading
        element begins */
    virtual void myStartElement(SumoXMLTag element, const std::string &name,
                                const Attributes &attrs);

    /** the implementation of the SAX-handler interface for reading
        characters */
    void myCharacters(SumoXMLTag element, const std::string &name,
                      const std::string &chars);

    /** the implementation of the SAX-handler interface for reading
        element ends */
    void myEndElement(SumoXMLTag element, const std::string &name);

protected:
    /// List of lane-based vehicle informing children
    std::vector<Setter*> mySetter;

    /// List of rerouting definition intervals
    std::vector<RerouteInterval> myIntervals;

    /// The probability and the user-given probability
    SUMOReal myProbability, myUserProbability;

    /// Information whether the current rerouting probability is the user-given
    bool myAmInUserMode;

    //{ members used during loading
    /// The first and the last time steps of the interval
    SUMOTime myCurrentIntervalBegin, myCurrentIntervalEnd;
    /// List of closed edges
    std::vector<MSEdge*> myCurrentClosed;
    /// new destinations with probabilities
    RandomDistributor<MSEdge*> myCurrentEdgeProb;
    /// new routes with probabilities
    RandomDistributor<MSRoute*> myCurrentRouteProb;
    //}

};


#endif

/****************************************************************************/

