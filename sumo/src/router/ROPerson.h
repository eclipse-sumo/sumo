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
// Copyright (C) 2002-2017 DLR (http://www.dlr.de/) and contributors
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
#include "RORouteDef.h"
#include "ROVehicle.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class ROEdge;


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

    void addTrip(const ROEdge* const from, const ROEdge* const to, const SVCPermissions modeSet,
                 const std::string& vTypes, const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop,
                 SUMOReal walkFactor);

    void addRide(const ROEdge* const from, const ROEdge* const to, const std::string& lines, const std::string& destStop);

    void addWalk(const ConstROEdgeVector& edges, const SUMOReal duration, const SUMOReal speed,
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

        virtual void addTripItem(TripItem* /* tripIt */) {
            throw ProcessError();
        }
        virtual const ROEdge* getOrigin() const = 0;
        virtual const ROEdge* getDestination() const = 0;
        virtual void saveVehicles(OutputDevice& /* os */, OutputDevice* const /* typeos */, bool /* asAlternatives */, OptionsCont& /* options */) const {}
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

    private:
        /// @brief Invalidated assignment operator
        Stop& operator=(const Stop& src);

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
        Ride(const ROEdge* const _from, const ROEdge* const _to,
             const std::string& _lines, const std::string& _destStop = "")
            : from(_from), to(_to), lines(_lines), destStop(_destStop) {}

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
        const std::string destStop;

    private:
        /// @brief Invalidated assignment operator
        Ride& operator=(const Ride& src);

    };

    /**
     * @brief A walk is part of a trip, e.g., go from here to here by foot
     *
     */
    class Walk : public TripItem {
    public:
        Walk(const ConstROEdgeVector& _edges, const std::string& _destStop = "")
            : edges(_edges), dur(-1), v(-1), dep(std::numeric_limits<SUMOReal>::infinity()), arr(std::numeric_limits<SUMOReal>::infinity()), destStop(_destStop) {}
        Walk(const ConstROEdgeVector& edges, const SUMOReal duration, const SUMOReal speed,
             const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& _destStop)
            : edges(edges), dur(duration), v(speed), dep(departPos), arr(arrivalPos), destStop(_destStop) {}
        const ROEdge* getOrigin() const {
            return edges.front();
        }
        const ROEdge* getDestination() const {
            return edges.back();
        }
        void saveAsXML(OutputDevice& os) const;

    private:
        const ConstROEdgeVector edges;
        const SUMOReal dur, v, dep, arr;
        const std::string destStop;

    private:
        /// @brief Invalidated assignment operator
        Walk& operator=(const Walk& src);

    };

    /**
     * @brief A planItem can be a Trip which contains multiple tripItems
     *
     */
    class PersonTrip : public PlanItem {
    public:
        PersonTrip()
            : from(0), to(0), modes(SVC_PEDESTRIAN), dep(0), arr(0), busStop(""), walkFactor(1.0) {}
        PersonTrip(const ROEdge* const from, const ROEdge* const to, const SVCPermissions modeSet,
                   const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop, SUMOReal _walkFactor)
            : from(from), to(to), modes(modeSet), dep(departPos), arr(arrivalPos), busStop(busStop), walkFactor(_walkFactor) {}
        /// @brief Destructor
        virtual ~PersonTrip() {
            for (std::vector<TripItem*>::const_iterator it = myTripItems.begin(); it != myTripItems.end(); ++it) {
                delete *it;
            }
            for (std::vector<ROVehicle*>::const_iterator it = myVehicles.begin(); it != myVehicles.end(); ++it) {
                delete(*it)->getRouteDefinition();
                delete *it;
            }
        }

        virtual void addTripItem(TripItem* tripIt) {
            myTripItems.push_back(tripIt);
        }
        void addVehicle(ROVehicle* veh) {
            myVehicles.push_back(veh);
        }
        std::vector<ROVehicle*>& getVehicles() {
            return myVehicles;
        }
        const ROEdge* getOrigin() const {
            return from != 0 ? from : myTripItems.front()->getOrigin();
        }
        const ROEdge* getDestination() const {
            return to != 0 ? to : myTripItems.back()->getDestination();
        }
        SUMOReal getDepartPos() const {
            return dep;
        }
        SUMOReal getArrivalPos() const {
            return arr;
        }
        SVCPermissions getModes() const {
            return modes;
        }
        bool hasBusStopDest() const {
            return busStop != "";
        }
        virtual bool needsRouting() const {
            return myTripItems.empty();
        }
        void saveVehicles(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const;
        void saveAsXML(OutputDevice& os) const {
            for (std::vector<TripItem*>::const_iterator it = myTripItems.begin(); it != myTripItems.end(); ++it) {
                (*it)->saveAsXML(os);
            }
        }
        SUMOReal getWalkFactor() const {
            return walkFactor;
        }

    private:
        const ROEdge* from;
        const ROEdge* to;
        const SVCPermissions modes;
        const SUMOReal dep, arr;
        const std::string busStop;
        /// @brief the fully specified trips
        std::vector<TripItem*> myTripItems;
        /// @brief the vehicles which may be used for routing
        std::vector<ROVehicle*> myVehicles;
        /// @brief walking speed factor
        SUMOReal walkFactor;

    private:
        /// @brief Invalidated assignment operator
        PersonTrip& operator=(const PersonTrip& src);

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
     * @param[in] typeos The types - output device to store additional types into
     * @param[in] asAlternatives Whether the route shall be saved as route alternatives
     * @param[in] options to find out about defaults and whether exit times for the edges shall be written
     * @exception IOError If something fails (not yet implemented)
     */
    void saveAsXML(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const;

    std::vector<PlanItem*>& getPlan() {
        return myPlan;
    }

private:
    bool computeIntermodal(const RORouterProvider& provider, PersonTrip* const trip, const ROVehicle* const veh, MsgHandler* const errorHandler);

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

