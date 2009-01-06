/****************************************************************************/
/// @file    NBCapacity2Lanes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A helper class which computes the lane number from given capacity
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
#ifndef NBCapacity2Lanes_h
#define NBCapacity2Lanes_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBCapacity2Lanes
 * @brief A helper class which computes the lane number from given capacity
 *
 * A helper class for the computation of the number of lanes an edge has
 *  in dependence to this edge's capacity. The computation is done by the
 *  (trivial) assumption, the number of lanes increases linear with the
 *  number of lanes.
 */
class NBCapacity2Lanes
{
public:
    /** @brief Donstructor
     *
     * @param[in] divider Value for the norming divider
     */
    NBCapacity2Lanes(SUMOReal divider) throw() : myDivider(divider) { }


    /// @brief Destructor
    ~NBCapacity2Lanes() throw() { }


    /** @brief Returns the number of lanes computed from the given capacity
     *
     * Returns the esimtaed number of lanes by returning the given capacity
     *  divided by the norming divider given in the constructor.
     *
     * @param[in] capacity The capacity to convert
     * @return The capacity converted to the number of lanes
     */
    int get(SUMOReal capacity) const throw() {
        capacity /= myDivider;
        if (capacity>(int) capacity) {
            capacity += 1;
        }
        // just assure that the number of lanes is not zero
        if (capacity==0) {
            capacity = 1;
        }
        return (int) capacity;
    }

private:
    /// @brief The norming divider
    SUMOReal myDivider;

};


#endif

/****************************************************************************/

