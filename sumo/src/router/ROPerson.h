/****************************************************************************/
/// @file    ROPerson.h
/// @author  Robert Hilbrich
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A person as used by router
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2002-2015 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ROPerson_h
#define ROPerson_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOTime.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include "RORoutable.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class ROEdge;
class ROVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROPerson
 * @brief A person as used by router
 */
class ROPerson : public RORoutable {

public:
    /** @brief Constructor
     *
     * @param[in] pars Parameter of this person
     * @param[in] type The type of the person
     */
    ROPerson(const SUMOVehicleParameter& pars, const SUMOVTypeParameter* type);

    /// @brief Destructor
    virtual ~ROPerson();

    void addTrip(const ROEdge* const from, const ROEdge* const to, const std::string& modes, const std::string& vTypes,
                 const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop);

    void addRide(const ROEdge* const from, const ROEdge* const to, const std::string& lines);

    void addWalk(const SUMOReal duration, const SUMOReal speed, const ConstROEdgeVector& edges,
                 const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop);

    void addStop(const SUMOVehicleParameter::Stop& stopPar, const ROEdge* const stopEdge);

    class TripItem;
    /**
     * @brief Every person has a plan comprising of multiple planItems
     *
     */
    class PlanItem {
    public:
        /// @brief Destructor
        virtual ~PlanItem() {}

        virtual void addTripItem(TripItem* tripIt) {
            throw ProcessError();
        }
        virtual const ROEdge* getOrigin() const = 0;
        virtual const ROEdge* getDestination() const = 0;
        virtual void saveAsXML(OutputDevice& os) const = 0;
        virtual bool isStop() const {
            return false;
        }
        virtual bool needsRouting() const {
            return false;
        }
    };

    /**
     * @brief A planItem can be a Stop
     *
     */
    class Stop : public PlanItem {
    public:
        Stop(const SUMOVehicleParameter::Stop& stop, const ROEdge* const stopEdge)
            : stopDesc(stop), edge(stopEdge) {}
        const ROEdge* getOrigin() const {
            return edge;
        }
        const ROEdge* getDestination() const {
            return edge;
        }
        void saveAsXML(OutputDevice& os) const {
            stopDesc.write(os);
        }
        bool isStop() const {
            return true;
        }
    private:
        SUMOVehicleParameter::Stop stopDesc;
        const ROEdge* const edge;
    };

    /**
     * @brief A TripItem is part of a trip, e.g., go from here to here by car
     *
     */
    class TripItem {
    public:
        /// @brief Destructor
        virtual ~TripItem() {}

        virtual const ROEdge* getOrigin() const = 0;
        virtual const ROEdge* getDestination() const = 0;
        virtual void saveAsXML(OutputDevice& os) const = 0;
    };

    /**
     * @brief A ride is part of a trip, e.g., go from here to here by car or bus
     *
     */
    class Ride : public TripItem {
    public:
        Ride(const ROEdge* const from, const ROEdge* const to, const std::string& lines) : from(from), to(to), lines(lines) {}
        const ROEdge* getOrigin() const {
            return from;
        }
        const ROEdge* getDestination() const {
            return to;
        }
        void saveAsXML(OutputDevice& os) const;
    private:
        const ROEdge* const from;
        const ROEdge* const to;
        const std::string lines;
    };

    /**
     * @brief A walk is part of a trip, e.g., go from here to here by foot
     *
     */
    class Walk : public TripItem {
    public:
        Walk(const ConstROEdgeVector& edges)
            : dur(-1), v(-1), edges(edges), dep(std::numeric_limits<SUMOReal>::infinity()), arr(std::numeric_limits<SUMOReal>::infinity()), busStop("") {}
        Walk(const SUMOReal duration, const SUMOReal speed, const ConstROEdgeVector& edges,
             const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop)
            : dur(duration), v(speed), edges(edges), dep(departPos), arr(arrivalPos), busStop(busStop) {}
        const ROEdge* getOrigin() const {
            return edges.front();
        }
        const ROEdge* getDestination() const {
            return edges.back();
        }
        void saveAsXML(OutputDevice& os) const;
    private:
        const SUMOReal dur, v, dep, arr;
        const ConstROEdgeVector edges;
        const std::string busStop;
    };

    /**
     * @brief A planItem can be a Trip which contains multiple tripItems
     *
     */
    class PersonTrip : public PlanItem {
    public:
        PersonTrip()
            : from(0), to(0), modes("walk"), dep(std::numeric_limits<SUMOReal>::infinity()), arr(std::numeric_limits<SUMOReal>::infinity()), busStop("") {}
        PersonTrip(const ROEdge* const from, const ROEdge* const to, const std::string& modes,
                   const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop)
            : from(from), to(to), modes(modes), dep(departPos), arr(arrivalPos), busStop(busStop) {}
        /// @brief Destructor
        virtual ~PersonTrip() {
            for (std::vector<TripItem*>::const_iterator it = myTripItems.begin(); it != myTripItems.end(); ++it) {
                delete *it;
            }
        }

        virtual void addTripItem(TripItem* tripIt) {
            myTripItems.push_back(tripIt);
        }
        const ROEdge* getOrigin() const {
            return from != 0 ? from : myTripItems.front()->getOrigin();
        }
        const ROEdge* getDestination() const {
            return to != 0 ? to : myTripItems.back()->getDestination();
        }
        virtual bool needsRouting() const {
            return myTripItems.empty();
        }
        void saveAsXML(OutputDevice& os) const {
            for (std::vector<TripItem*>::const_iterator it = myTripItems.begin(); it != myTripItems.end(); ++it) {
                (*it)->saveAsXML(os);
            }
        }
    private:
        const ROEdge* from;
        const ROEdge* to;
        const std::string modes;
        const SUMOReal dep, arr;
        const std::string busStop;
        /// @brief the fully specified trips
        std::vector<TripItem*> myTripItems;
        /// @brief the vehicles which may be used for routing
        std::vector<ROVehicle*> myVehicles;
    };


    /** @brief Returns the first edge the person takes
     *
     * @return The person's departure edge
     */
    const ROEdge* getDepartEdge() const {
        return myPlan.front()->getOrigin();
    }


    void computeRoute(const RORouterProvider& provider,
                      const bool removeLoops, MsgHandler* errorHandler);


    /** @brief Saves the complete person description.
     *
     * Saves the person itself including the trips and stops.
     *
     * @param[in] os The routes or alternatives output device to store the routable's description into
     * @param[in] asAlternatives Whether the route shall be saved as route alternatives
     * @param[in] options to find out about defaults and whether exit times for the edges shall be written
     * @exception IOError If something fails (not yet implemented)
     */
    void saveAsXML(OutputDevice& os, bool asAlternatives, OptionsCont& options) const;

    std::vector<PlanItem*>& getPlan() {
        return myPlan;
    }


private:
    /**
     * @brief The plan of the person
     */
    std::vector<PlanItem*> myPlan;


private:
    /// @brief Invalidated copy constructor
    ROPerson(const ROPerson& src);

    /// @brief Invalidated assignment operator
    ROPerson& operator=(const ROPerson& src);

};

#endif

/****************************************************************************/

