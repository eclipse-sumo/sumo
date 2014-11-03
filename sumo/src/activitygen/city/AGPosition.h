/****************************************************************************/
/// @file    AGPosition.h
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    July 2010
/// @version $Id$
///
// References a street of the city and defines a position in this street
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#ifndef AGPOSITION_H
#define AGPOSITION_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "../../utils/geom/Position.h"
#include <list>
#include <map>


// ===========================================================================
// class declarations
// ===========================================================================
class AGStreet;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AGPosition
 * @brief A location in the 2D plane freely positioned on a street.
 *
 * This class restricts the Position class in the way that it must be
 * a position on a street. As a consequence, this position can be described
 * either by x and y coordinates or by a street and its distance to the
 * beginning of the street (the relative position).
 *
 * @TODO Should this class be derived from Position?
 */
class AGPosition {
public:
    /** @brief Constructs an AGPosition at a certain point on a street.
     *
     * An AGPosition is determined by a street and the relative position
     * on the street. This relative position is the distance from the
     * from node of the street.
     *
     * param[in] str the street on which the AGPosition is located
     * param[in] pos the distance from the from node of the street
     */
    AGPosition(const AGStreet& str, SUMOReal pos);
    /** @brief Constructs an AGPosition at a random point on a street.
     *
     * This constructor determines the distance from the from node with
     * a random number based on a uniform density.
     *
     * param[in] str the street on which the AGPosition is located
     */
    AGPosition(const AGStreet& str);

    /** @brief Provides the street this AGPosition is located on.
     *
     * @return the street
     */
    const AGStreet& getStreet() const;

    /** @brief Provides the relative position of this AGPosition on the street.
     *
     * This relative position is the distance from the from node
     * of the associated street.
     *
     * @return the relative position
     */
    SUMOReal getPosition() const;

    /** @brief Tests whether two positions are at the same place.
     *
     * Compares the x and y coordinates with a threshold
     * (see Position::almostSame)
     *
     * @param[in] pos the position with which the comparison is done
     * @return true if both AGPositions are (almost) at the same place
     */
    bool operator==(const AGPosition& pos) const;

    /** @brief Computes the distance between two AGPosition objects.
     *
     * @param[in] the other position the distance in computed to
     * @return the distance
     */
    SUMOReal distanceTo(const AGPosition& otherPos) const;

    /** @brief Computes the distance to the closest position in a list.
     *
     * minDistanceTo computes the distance to all positions in the given list
     * and returns the minimal distance.
     *
     * @param[in] positions the list of positions the distances are computed to
     * @return the minimal distance
     */
    SUMOReal minDistanceTo(const std::list<AGPosition>& positions) const;

    /** @brief Computes the distance to the closest position in a map.
     *
     * minDistanceTo computes the distance to all positions given as the second
     * elements of a map and returns the minimal distance.
     *
     * @param[in] positions the map of positions the distances are computed to
     * @return the minimal distance
     */
    SUMOReal minDistanceTo(const std::map<int, AGPosition>& positions) const;

    /** @brief Prints out a summary of the properties of this class
     * on standard output.
     */
    void print() const;

private:
    const AGStreet* street;
    SUMOReal position;
    Position pos2d;

    /** @brief Determines a random relative position on a street.
     *
     * @return the random relative position
     */
    static SUMOReal randomPositionInStreet(const AGStreet& street);

    /** Creates a Position object to the street and position attribute of
     * this class.
     *
     * This method may only be called when street and position are initialised!
     *
     * @return the Position object
     */
    Position compute2dPosition() const;
};

#endif /* AGPOSITION_H */

/****************************************************************************/
