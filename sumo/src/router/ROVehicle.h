/****************************************************************************/
/// @file    ROVehicle.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
/// @version $Id$
///
// A vehicle as used by router
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
#ifndef ROVehicle_h
#define ROVehicle_h


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
class RONet;
class RORouteDef;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ROVehicle
 * @brief A vehicle as used by router
 */
class ROVehicle : public RORoutable {
public:
    /** @brief Constructor
     *
     * @param[in] pars Parameter of this vehicle
     * @param[in] route The definition of the route the vehicle shall use
     * @param[in] type The type of the vehicle
     */
    ROVehicle(const SUMOVehicleParameter& pars,
              RORouteDef* route, const SUMOVTypeParameter* type,
              const RONet* net, MsgHandler* errorHandler = 0);


    /// @brief Destructor
    virtual ~ROVehicle();


    /** @brief Returns the definition of the route the vehicle takes
     *
     * @return The vehicle's route definition
     *
     * @todo Why not return a reference?
     */
    inline RORouteDef* getRouteDefinition() const {
        return myRoute;
    }


    /** @brief Returns the definition of the vehicle parameter
     *
     * @return The vehicle's parameter
     */
    inline const SUMOVehicleParameter& getParameter() const {
        return myParameter;
    }


    /** @brief Returns the first edge the vehicle takes
     *
     * @return The vehicle's departure edge
     */
    const ROEdge* getDepartEdge() const;


    void computeRoute(const RORouterProvider& provider,
                      const bool removeLoops, MsgHandler* errorHandler);

    /** @brief Returns the time the vehicle starts at, 0 for triggered vehicles
     *
     * @return The vehicle's depart time
     */
    inline SUMOTime getDepartureTime() const {
        return MAX2(SUMOTime(0), myParameter.depart);
    }


    inline const ConstROEdgeVector& getStopEdges() const {
        return myStopEdges;
    }

    /** @brief Returns an upper bound for the speed factor of this vehicle
     *
     * @return the maximum speed factor
     */
    inline SUMOReal getChosenSpeedFactor() const {
        return SUMOReal(2. * getType()->speedDev + 1.) * getType()->speedFactor;
    }


    /** @brief Saves the complete vehicle description.
     *
     * Saves the vehicle itself including the route and stops.
     *
     * @param[in] os The routes or alternatives output device to store the vehicle's description into
     * @param[in] typeos The types - output device to store types into
     * @param[in] asAlternatives Whether the route shall be saved as route alternatives
     * @param[in] options to find out about defaults and whether exit times for the edges shall be written
     * @exception IOError If something fails (not yet implemented)
     */
    void saveAsXML(OutputDevice& os, OutputDevice* const typeos, bool asAlternatives, OptionsCont& options) const;


private:
    /** @brief Adds a stop to this vehicle
     *
     * @param[in] stopPar the stop paramters
     * @param[in] net     pointer to the network, used for edge retrieval
     */
    void addStop(const SUMOVehicleParameter::Stop& stopPar,
                 const RONet* net, MsgHandler* errorHandler);

private:
    /// @brief The route the vehicle takes
    RORouteDef* const myRoute;

    /// @brief The edges where the vehicle stops
    ConstROEdgeVector myStopEdges;


private:
    /// @brief Invalidated copy constructor
    ROVehicle(const ROVehicle& src);

    /// @brief Invalidated assignment operator
    ROVehicle& operator=(const ROVehicle& src);

};


#endif

/****************************************************************************/
