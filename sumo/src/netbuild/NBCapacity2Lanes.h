/****************************************************************************/
/// @file    NBCapacity2Lanes.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 19 Jul 2002
/// @version $Id$
///
// A helper class for the computation of the number of lanes an
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
 * A helper class for the computation of the number of lanes an edge has
 * in dependence to this edge's capacity
 */
class NBCapacity2Lanes
{
public:
    /// constructor
    NBCapacity2Lanes(SUMOReal divider) : myDivider(divider)
    { }

    /// destructor
    ~NBCapacity2Lanes()
    { }

    /// returns the number of lanes computed from the given capacity
    int get(SUMOReal capacity)
    {
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
    /// the norming factor
    SUMOReal myDivider;
};


#endif

/****************************************************************************/

