/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2010-2019 German Aerospace Center (DLR) and others.
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    AGStreet.h
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// Represents a SUMO edge and contains people and work densities
/****************************************************************************/
#ifndef AGSTREET_H
#define AGSTREET_H


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <utils/common/SUMOVehicleClass.h>
#include <router/ROAbstractEdgeBuilder.h>
#include <router/ROEdge.h>


// ===========================================================================
// class declarations
// ===========================================================================
class AGPosition;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AGStreet
 * @brief A model of the street in the city.
 *
 * AGStreet represents a street in the city. It contains all model relevant
 * properties and is associated with a ROEdge of the routing network.
 */
class AGStreet : public ROEdge {
public:
    class Builder : public ROAbstractEdgeBuilder {
    public:
        /** @brief Builds an edge with the given name
         *
         * @param[in] name The name of the edge
         * @param[in] from The node the edge begins at
         * @param[in] to The node the edge ends at
         * @param[in] priority The edge priority (road class)
         * @return A proper instance of the named edge
         */
        ROEdge* buildEdge(const std::string& name, RONode* from, RONode* to, const int priority) {
            return new AGStreet(name, from, to, getNextIndex(), priority);
        }
    };

    AGStreet(const std::string& id, RONode* from, RONode* to, int index, const int priority);

    /** @brief Provides the number of persons living in this street.
     *
     * @return the number of inhabitants
     */
    double getPopulation() const;

    /** @brief Modifies the number of persons living in this street.
     *
     * @param[in] pop the new number of inhabitants
     */
    void setPopulation(const double pop);

    /** @brief Provides the number of work places in this street.
     *
     * @return the number of work places
     */
    double getWorkplaceNumber() const;

    /** @brief Modifies the number of work places in this street.
     *
     * @param[in] work the new number of work places
     */
    void setWorkplaceNumber(const double work);

    /** @brief Prints a summary of the properties of this street to standard
     * output.
     */
    void print() const;

    /** @brief Returns whether the given vehicle class is allowed on this street.
     *
     * @param[in] vclass the class (passenger or bus) in question
     * @return whether it is allowed on any of the lanes
     */
    bool allows(const SUMOVehicleClass vclass) const;

private:
    double myPopulation;
    double myNumWorkplaces;
};

#endif

/****************************************************************************/
