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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2010-2014 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AGSTREET_H
#define AGSTREET_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>


// ===========================================================================
// class declarations
// ===========================================================================
class ROEdge;
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
class AGStreet {
public:
    AGStreet(const ROEdge* edge, SUMOReal popD = 0, SUMOReal workD = 0);

    /** @brief Provides the length of this edge.
     *
     * @return the length of this edge
     */
    SUMOReal getLength() const;

    /** @brief Provides the id of this edge.
     *
     * @return the id of this edge
     */
    const std::string& getName() const;

    /** @brief Provides the number of persons living in this street.
     *
     * @return the number of inhabitants
     */
    SUMOReal getPopulation() const;

    /** @brief Modifies the number of persons living in this street.
     *
     * @param[in] pop the new number of inhabitants
     */
    void setPopulation(const SUMOReal pop);

    /** @brief Provides the number of work places in this street.
     *
     * @return the number of work places
     */
    SUMOReal getWorkplaceNumber() const;

    /** @brief Modifies the number of work places in this street.
     *
     * @param[in] work the new number of work places
     */
    void setWorkplaceNumber(const SUMOReal work);

    /** @brief Prints a summary of the properties of this street to standard
     * output.
     */
    void print() const;

private:
    friend class AGPosition;

    const ROEdge* edge;
    SUMOReal pop;
    SUMOReal work;
};

#endif

/****************************************************************************/
