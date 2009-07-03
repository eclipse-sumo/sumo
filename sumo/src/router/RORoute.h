/****************************************************************************/
/// @file    RORoute.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A complete router's route
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


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
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
    RORoute(const std::string &id, SUMOReal costs, SUMOReal prob,
            const std::vector<const ROEdge*> &route, const RGBColor * const color) throw();


    /** @brief Copy constructor
     *
     * @param[in] src The route to copy
     */
    RORoute(const RORoute &src) throw();


    /// @brief Destructor
    ~RORoute() throw();


    /** @brief Adds an edge to the end of the route
     *
     * @param[in] edge The edge to add
     * @todo What for? Isn't the route already complete?
     */
    void add(ROEdge *edge) throw();


    /** @brief Returns the first edge in the route
     *
     * @return The route's first edge
     */
    const ROEdge *getFirst() const throw() {
        return myRoute[0];
    }


    /** @brief Returns the last edge in the route
     *
     * @return The route's last edge
     */
    const ROEdge *getLast() const throw() {
        return myRoute[myRoute.size()-1];
    }


    /** @brief Returns the costs of the route
     *
     * @return The route's costs (normally the time needed to pass it)
     * @todo Recheck why the costs are stored in a route
     */
    SUMOReal getCosts() const throw() {
        return myCosts;
    }


    /** @brief Returns the probability the driver will take this route with
     *
     * @return The probability to choose the route
     * @todo Recheck why the probability is stored in a route
     */
    SUMOReal getProbability() const throw() {
        return myProbability;
    }


    /** @brief Sets the costs of the route
     *
     * @todo Recheck why the costs are stored in a route
     */
    void setCosts(SUMOReal costs) throw();


    /** @brief Sets the probability of the route
     *
     * @todo Recheck why the probability is stored in a route
     */
    void setProbability(SUMOReal prob) throw();


    /** @brief Returns the number of edges in this route
     *
     * @return The number of edges the route is made of
     */
    unsigned int size() const throw() {
        return (unsigned int) myRoute.size();
    }


    /** @brief Returns the list of edges this route consists of
     *
     * @return The edges this route consists of
     */
    const std::vector<const ROEdge*> &getEdgeVector() const throw() {
        return myRoute;
    }

    /** @brief Returns this route's color
     *
     * @return This route's color
     */
    const RGBColor * const getColor() const throw() {
        return myColor;
    }


    /** @brief Checks whether this route contains loops and removes such
     */
    void recheckForLoops() throw();

    OutputDevice &
    writeXMLDefinition(SUMOAbstractRouter<ROEdge,ROVehicle> &router, OutputDevice &dev, const ROVehicle * const veh, bool asAlternatives, bool withExitTimes) const;


private:
    /// @brief The costs of the route
    SUMOReal myCosts;

    /// @brief The probability the driver will take this route with
    SUMOReal myProbability;

    /// @brief The edges the route consists of
    std::vector<const ROEdge*> myRoute;

    /// @brief The color of the route
    const RGBColor *myColor;

private:
    /// @brief Invalidated assignment operator
    RORoute &operator=(const RORoute &src);

};


#endif

/****************************************************************************/

