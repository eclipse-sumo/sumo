/****************************************************************************/
/// @file    RORoutable.h
/// @author  Michael Behrisch
/// @date    Oct 2015
/// @version $Id$
///
// A routable thing such as a vehicle or person
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
#ifndef RORoutable_h
#define RORoutable_h


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
#include <utils/vehicle/IntermodalRouter.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/vehicle/SUMOVTypeParameter.h>

// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class ROEdge;
class ROLane;
class RONode;
class ROVehicle;

typedef std::vector<const ROEdge*> ConstROEdgeVector;
typedef IntermodalRouter<ROEdge, ROLane, RONode, ROVehicle> ROIntermodalRouter;
typedef RouterProvider<ROEdge, ROLane, RONode, ROVehicle> RORouterProvider;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORoutable
 * @brief A routable thing such as a vehicle or person
 */
class RORoutable {
public:
    /** @brief Constructor
     *
     * @param[in] pars Parameter of this routable
     * @param[in] type The type of the routable
     */
    RORoutable(const SUMOVehicleParameter& pars, const SUMOVTypeParameter* type)
        : myParameter(pars), myType(type), myRoutingSuccess(false) {}


    /// @brief Destructor
    virtual ~RORoutable() {}


    /** @brief Returns the type of the vehicle
     *
     * @return The vehicle's type
     *
     * @todo Why not return a reference?
     */
    inline const SUMOVTypeParameter* getType() const {
        return myType;
    }


    /** @brief Returns the id of the vehicle
     *
     * @return The id of the vehicle
     */
    inline const std::string& getID() const {
        return myParameter.id;
    }


    /** @brief Returns the time the vehicle starts at, -1 for triggered vehicles
     *
     * @return The vehicle's depart time
     */
    inline SUMOTime getDepart() const {
        return myParameter.depart;
    }


    inline SUMOVehicleClass getVClass() const {
        return getType() != 0 ? getType()->vehicleClass : SVC_IGNORING;
    }


    /// @brief Returns the vehicle's maximum speed
    inline SUMOReal getMaxSpeed() const {
        return myType->maxSpeed;
    }


    virtual const ROEdge* getDepartEdge() const = 0;


    virtual void computeRoute(const RORouterProvider& provider,
                              const bool removeLoops, MsgHandler* errorHandler) = 0;


    /** @brief  Saves the routable including the vehicle type (if it was not saved before).
     *
     * @param[in] os The routes - output device to store the vehicle's description into
     * @param[in] altos The route alternatives - output device to store the vehicle's description into
     * @param[in] typeos The types - output device to store the vehicle types into
     * @exception IOError If something fails (not yet implemented)
     */
    void write(OutputDevice& os, OutputDevice* const altos,
               OutputDevice* const typeos, OptionsCont& options) const {
        if (altos == 0 && typeos == 0) {
            saveAsXML(os, &os, false, options);
        } else {
            saveAsXML(os, typeos, false, options);
        }
        if (altos != 0) {
            saveAsXML(*altos, typeos, true, options);
        }
    }


    inline bool getRoutingSuccess() const {
        return myRoutingSuccess;
    }


protected:
    /** @brief Saves the complete routable description.
     *
     * Saves the routable itself including the route and stops.
     *
     * @param[in] os The routes or alternatives output device to store the routable's description into
     * @param[in] typeos The types - output device to store additional types into
     * @param[in] asAlternatives Whether the route shall be saved as route alternatives
     * @param[in] options to find out about defaults and whether exit times for the edges shall be written
     * @exception IOError If something fails (not yet implemented)
     */
    virtual void saveAsXML(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const = 0;


protected:
    /// @brief The vehicle's parameter
    SUMOVehicleParameter myParameter;

    /// @brief The type of the vehicle
    const SUMOVTypeParameter* const myType;

    /// @brief Whether the last routing was successful
    bool myRoutingSuccess;


private:
    /// @brief Invalidated copy constructor
    RORoutable(const RORoutable& src);

    /// @brief Invalidated assignment operator
    RORoutable& operator=(const RORoutable& src);

};


#endif

/****************************************************************************/
