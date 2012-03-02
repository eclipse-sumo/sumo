/****************************************************************************/
/// @file    NBAlgorithms.h
/// @author  Daniel Krajzewicz
/// @date    02. March 2012
/// @version $Id$
///
// 
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NBAlgorithms_h
#define NBAlgorithms_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <map>


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;
class NBNodeCont;

// ===========================================================================
// class definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NBTurningDirectionsComputer
// ---------------------------------------------------------------------------
/* @class NBTurningDirectionsComputer
 * @brief Computes turnaround destinations for all edges (if exist)
 */
class NBTurningDirectionsComputer {
public:
    /** @brief Computes turnaround destinations for all edges (if exist)
     * @param[in] nc The container of nodes to loop along
     */
    static void compute(NBNodeCont &nc);
private:
    /** @struct Combination
     * @brief Stores the information about the angle between an incoming ("from") and an outgoing ("to") edge
     *
     * Note that the angle is increased by 360 if the edges connect the same two nodes in
     *  opposite direction.
     */
    struct Combination {
        NBEdge *from;
        NBEdge *to;
        SUMOReal angle;
    };


    /** @class combination_by_angle_sorter
     * @brief Sorts "Combination"s by decreasing angle
     */
    class combination_by_angle_sorter {
    public:
        explicit combination_by_angle_sorter() { }
        int operator()(const Combination& c1, const Combination& c2) const {
            if (c1.angle!=c2.angle) {
                return c1.angle > c2.angle;
            }
            if (c1.from!=c2.from) {
                return c1.from->getID() < c2.from->getID();
            }
            return c1.to->getID() < c2.to->getID();
        }
    };
};


#endif

/****************************************************************************/

