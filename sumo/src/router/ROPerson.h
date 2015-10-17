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

// ===========================================================================
// class declarations
// ===========================================================================
class RORouteDef;
class OutputDevice;
class ROEdge;
class ROVehicle;

typedef std::vector<const ROEdge*> ConstROEdgeVector;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROPerson
 * @brief A person as used by router
 */
class ROPerson
{

public:
    /** @brief Constructor
     *
     * @param[in] pars Parameter of this person
     */
    ROPerson(const SUMOVehicleParameter& pars);

    /// @brief Destructor
    virtual ~ROPerson();

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
        virtual const ROEdge* getDestination() const = 0;
        virtual void saveAsXML(OutputDevice& os) const = 0;
        virtual bool isStop() const {
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

        virtual void saveAsXML(OutputDevice& os) const = 0;
    };

    /**
     * @brief A TripItem is part of a trip, e.g., go from here to here by car
     *
     */
    class Ride : public TripItem {
    public:
        Ride(const ROEdge* const from, const ROEdge* const to, const std::string& lines) : from(from), to(to), lines(lines) {}
        void saveAsXML(OutputDevice& os) const;
    private:
        const ROEdge* const from;
        const ROEdge* const to;
        const std::string lines;
    };

    /**
     * @brief A TripItem is part of a trip, e.g., go from here to here by car
     *
     */
    class Walk : public TripItem {
    public:
        Walk(const SUMOReal duration, const SUMOReal speed, const ConstROEdgeVector& edges,
             const SUMOReal departPos, const SUMOReal arrivalPos, const std::string& busStop)
            : dur(duration), v(speed), edges(edges), dep(departPos), arr(arrivalPos), busStop(busStop) {}
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
        /// @brief Destructor
        virtual ~PersonTrip() {
            for (std::vector<TripItem*>::const_iterator it = myTripItems.begin(); it != myTripItems.end(); ++it) {
                delete *it;
            }
        }

        virtual void addTripItem(TripItem* tripIt) {
            myTripItems.push_back(tripIt);
        }
        const ROEdge* getDestination() const {
            return 0;
        }
        void saveAsXML(OutputDevice& os) const {
            for (std::vector<TripItem*>::const_iterator it = myTripItems.begin(); it != myTripItems.end(); ++it) {
                (*it)->saveAsXML(os);
            }
        }
    private:
        /// @brief the fully specified trips
        std::vector<TripItem*> myTripItems;
        /// @brief the vehicles which may be used for routing
        std::vector<ROVehicle*> myVehicles;
    };

//    /** @brief Returns the definition of the route the vehicle takes
//     *
//     * @return The vehicle's route definition
//     *
//     * @todo Why not return a reference?
//     */
//    RORouteDef* getRouteDefinition() const {
//        return myRoute;
//    }

//    /** @brief Returns the type of the vehicle
//     *
//     * @return The vehicle's type
//     *
//     * @todo Why not return a reference?
//     */
//    const SUMOVTypeParameter* getType() const {
//        return myType;
//    }

    /** @brief Returns the id of the vehicle
     *
     * @return The id of the vehicle
     */
    const std::string& getID() const {
        return myParameter.id;
    }

//    /** @brief Returns the time the vehicle starts at, 0 for triggered vehicles
//     *
//     * @return The vehicle's depart time
//     */
//    SUMOTime getDepartureTime() const {
//        return MAX2(SUMOTime(0), myParameter.depart);
//    }

    /** @brief Returns the time the vehicle starts at, -1 for triggered vehicles
     *
     * @return The vehicle's depart time
     */
    SUMOTime getDepart() const {
        return myParameter.depart;
    }

//    const ConstROEdgeVector& getStopEdges() const {
//        return myStopEdges;
//    }

//    /// @brief Returns the vehicle's maximum speed
//    SUMOReal getMaxSpeed() const;

//    inline SUMOVehicleClass getVClass() const {
//        return getType() != 0 ? getType()->vehicleClass : SVC_IGNORING;
//    }

    /** @brief Saves the complete person description.
     *
     * Saves the person itself including the trips and stops.
     *
     * @param[in] os The routes or alternatives output device to store the person description into
     * @exception IOError If something fails (not yet implemented)
     */
    void saveAsXML(OutputDevice& os) const;

    inline void setRoutingSuccess(const bool val) {
        myRoutingSuccess = val;
    }

    inline bool getRoutingSuccess() const {
        return myRoutingSuccess;
    }

    std::vector<PlanItem*>& getPlan() {
        return myPlan;
    }


protected:
    /// @brief The person's parameter
    SUMOVehicleParameter myParameter;

/// @brief The type of the person
//    const SUMOVTypeParameter* const myType;

//    typedef std::vector<ROVehicle*> ROTrip;

    /**
     * @brief The plan of each person
     */
    std::vector<PlanItem*> myPlan;

/// @brief The edges where the vehicle stops
//    ConstROEdgeVector myStopEdges;

/// @brief Whether the last routing was successful
    bool myRoutingSuccess;


private:
    /// @brief Invalidated copy constructor
    ROPerson (const ROPerson& src);

    /// @brief Invalidated assignment operator
    ROPerson&
    operator= (const ROPerson& src);

};

#endif

/****************************************************************************/

