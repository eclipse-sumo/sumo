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
class RONet;

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
     * @param[in] pars Parameter of this vehicle
     * @param[in] route The definition of the route the vehicle shall use
     * @param[in] type The type of the vehicle
     */
    ROPerson (const SUMOVehicleParameter& pars, RORouteDef* route,
              const SUMOVTypeParameter* type, const RONet* net);

    /// @brief Destructor
    virtual
    ~ROPerson ();

    // forward declarations
    class PlanItem;
    class Stop;
    class Trip;
    class TripItem;


    /**
     * @brief Every person has a plan comprising of multiple planItems
     *
     */
    class PlanItem
    {
    };

    /**
     * @brief A planItem can be a Stop
     *
     */
    class Stop : public PlanItem
    {
    };

    /**
     * @brief A planItem can be a Trip which contains multiple tripItems
     *
     */
    class Trip : public PlanItem
    {
    public:

        /**
         * @brief A TripItem is part of a trip, e.g., go from here to here by car
         *
         */

        std::vector<TripItem> myTripItems;
        std::vector<ROVehicle> myVehicles;
    };

    class TripItem
    {
    };

    /**
     * @brief The plan of each person
     */
    std::vector<PlanItem> myPlan;

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

//    /** @brief Returns the id of the vehicle
//     *
//     * @return The id of the vehicle
//     */
//    const std::string& getID() const {
//        return myParameter.id;
//    }

//    /** @brief Returns the time the vehicle starts at, 0 for triggered vehicles
//     *
//     * @return The vehicle's depart time
//     */
//    SUMOTime getDepartureTime() const {
//        return MAX2(SUMOTime(0), myParameter.depart);
//    }

//    /** @brief Returns the time the vehicle starts at, -1 for triggered vehicles
//     *
//     * @return The vehicle's depart time
//     */
//    SUMOTime getDepart() const {
//        return myParameter.depart;
//    }

//    const ConstROEdgeVector& getStopEdges() const {
//        return myStopEdges;
//    }

//    /// @brief Returns the vehicle's maximum speed
//    SUMOReal getMaxSpeed() const;

//    inline SUMOVehicleClass getVClass() const {
//        return getType() != 0 ? getType()->vehicleClass : SVC_IGNORING;
//    }

//    /** @brief Returns an upper bound for the speed factor of this vehicle
//     *
//     * @return the maximum speed factor
//     */
//    inline SUMOReal getChosenSpeedFactor() const {
//        return SUMOReal(2. * getType()->speedDev + 1.) * getType()->speedFactor;
//    }

//    /** @brief  Saves the vehicle type if it was not saved before.
//     *
//     * @param[in] os The routes - output device to store the vehicle's description into
//     * @param[in] altos The route alternatives - output device to store the vehicle's description into
//     * @param[in] typeos The types - output device to store the vehicle types into
//     * @exception IOError If something fails (not yet implemented)
//     */
//    void saveTypeAsXML(OutputDevice& os, OutputDevice* const altos,
//                       OutputDevice* const typeos) const;

//    /** @brief Saves the complete vehicle description.
//     *
//     * Saves the vehicle itself including the route and stops.
//     *
//     * @param[in] os The routes or alternatives output device to store the vehicle's description into
//     * @param[in] asAlternatives Whether the route shall be saved as route alternatives
//     * @param[in] withExitTimes whether exit times for the edges shall be written
//     * @exception IOError If something fails (not yet implemented)
//     */
//    void saveAllAsXML(OutputDevice& os, bool asAlternatives, bool withExitTimes) const;

//    inline void setRoutingSuccess(const bool val) {
//        myRoutingSuccess = val;
//    }

//    inline bool getRoutingSuccess() const {
//        return myRoutingSuccess;
//    }
private:
    /** @brief Adds a stop to this vehicle
     *
     * @param[in] stopPar the stop paramters
     * @param[in] net     pointer to the network, used for edge retrieval
     */
//    void addStop(const SUMOVehicleParameter::Stop& stopPar, const RONet* net);
protected:
    /// @brief The person's parameter
//    SUMOVehicleParameter myParameter;

/// @brief The type of the person
//    const SUMOVTypeParameter* const myType;

//    typedef std::vector<ROVehicle*> ROTrip;

/// @brief The person plan
//    std::vector<ROTrip> myPlan;

/// @brief The edges where the vehicle stops
//    ConstROEdgeVector myStopEdges;

/// @brief Whether the last routing was successful
//    bool myRoutingSuccess;

private:
    /// @brief Invalidated copy constructor
    ROPerson (const ROPerson& src);

    /// @brief Invalidated assignment operator
    ROPerson&
    operator= (const ROPerson& src);

};

#endif

/****************************************************************************/

