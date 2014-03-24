/****************************************************************************/
/// @file    RORoute.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Yun-Pang Floetteroed
/// @date    Sept 2002
/// @version $Id$
///
// A complete router's route
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef RORoute_h
#define RORoute_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <utils/common/Named.h>
#include <utils/common/RGBColor.h>
#include <utils/common/SUMOAbstractRouter.h>
#include <utils/common/SUMOVehicleParameter.h>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
class ROVehicle;
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class RORoute
 * @brief A complete router's route
 *
 * This class represents a single and complete vehicle route after being
 *  computed/imported.
 */
class RORoute : public Named {
public:
    /** @brief Constructor
     *
     * @param[in] id The route's id
     * @param[in] costs The route's costs
     * @param[in] prob The route's probability
     * @param[in] route The list of edges the route is made of
     * @param[in] color The (optional) color of this route
     *
     * @todo Are costs/prob really mandatory?
     */
    RORoute(const std::string& id, SUMOReal costs, SUMOReal prob,
            const std::vector<const ROEdge*>& route, const RGBColor* const color,
            const std::vector<SUMOVehicleParameter::Stop>& stops);


    /** @brief Copy constructor
     *
     * @param[in] src The route to copy
     */
    RORoute(const RORoute& src);


    /// @brief Destructor
    ~RORoute();


    /** @brief Returns the first edge in the route
     *
     * @return The route's first edge
     */
    const ROEdge* getFirst() const {
        return myRoute[0];
    }


    /** @brief Returns the last edge in the route
     *
     * @return The route's last edge
     */
    const ROEdge* getLast() const {
        return myRoute.back();
    }


    /** @brief Returns the costs of the route
     *
     * @return The route's costs (normally the time needed to pass it)
     * @todo Recheck why the costs are stored in a route
     */
    SUMOReal getCosts() const {
        return myCosts;
    }


    /** @brief Returns the probability the driver will take this route with
     *
     * @return The probability to choose the route
     * @todo Recheck why the probability is stored in a route
     */
    SUMOReal getProbability() const {
        return myProbability;
    }


    /** @brief Sets the costs of the route
     *
     * @todo Recheck why the costs are stored in a route
     */
    void setCosts(SUMOReal costs);


    /** @brief Sets the probability of the route
     *
     * @todo Recheck why the probability is stored in a route
     */
    void setProbability(SUMOReal prob);


    /** @brief Returns the number of edges in this route
     *
     * @return The number of edges the route is made of
     */
    unsigned int size() const {
        return (unsigned int) myRoute.size();
    }


    /** @brief Returns the list of edges this route consists of
     *
     * @return The edges this route consists of
     */
    const std::vector<const ROEdge*>& getEdgeVector() const {
        return myRoute;
    }

    /** @brief Returns this route's color
     *
     * @return This route's color
     */
    const RGBColor* getColor() const {
        return myColor;
    }


    /** @brief Checks whether this route contains loops and removes such
     */
    void recheckForLoops();

    OutputDevice&
    writeXMLDefinition(OutputDevice& dev, const ROVehicle* const veh,
                       const bool withCosts, const bool withExitTimes) const;

    /** @brief add additional vehicles/probability
     */
    void addProbability(SUMOReal prob);

    /** @brief Returns the list of stops this route contains
     *
     * @return list of stops
     */
    const std::vector<SUMOVehicleParameter::Stop>& getStops() const {
        return myStops;
    }

private:
    /// @brief The costs of the route
    SUMOReal myCosts;

    /// @brief The probability the driver will take this route with
    SUMOReal myProbability;

    /// @brief The edges the route consists of
    std::vector<const ROEdge*> myRoute;

    /// @brief The color of the route
    const RGBColor* myColor;

    /// @brief List of the stops on the parsed route
    std::vector<SUMOVehicleParameter::Stop> myStops;


private:
    /// @brief Invalidated assignment operator
    RORoute& operator=(const RORoute& src);

};


#endif

/****************************************************************************/

