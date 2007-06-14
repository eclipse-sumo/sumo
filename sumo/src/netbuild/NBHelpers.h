/****************************************************************************/
/// @file    NBHelpers.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Some mathematical helper methods
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
#ifndef NBHelpers_h
#define NBHelpers_h


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
class NBNode;
class Position2D;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class NBHelpers
 * Some mathmatical methods for the computation of angles
 */
class NBHelpers
{
public:
    /** computes the angle of the straight which is described by the two
        coordinates */
    static SUMOReal angle(SUMOReal x1, SUMOReal y1, SUMOReal x2, SUMOReal y2);

    /** computes the angle between the first angle and the straigth which
        is described by her coordinates (parameter 2-5) */
    static SUMOReal relAngle(SUMOReal angle,
                             SUMOReal x1, SUMOReal y1,
                             SUMOReal x2, SUMOReal y2);

    /** computes the relative angle between the two angles */
    static SUMOReal relAngle(SUMOReal angle1, SUMOReal angle2);

    /** normalises angle <-170 and >170 to 180 after the computation with
        "relAngle" */
    static SUMOReal normRelAngle(SUMOReal angle1, SUMOReal angle2);

    /** computes number of a logic's setting possibilities for the given key
        In other words: how many permutation of vehicle wishes are possible on
        this junction */
    static long computeLogicSize(std::string key);

    /** converts the numerical id to its "normal" string representation */
    static std::string normalIDRepresentation(const std::string &id);

    /** returns the distance between both nodes */
    static SUMOReal distance(NBNode *node1, NBNode *node2);

};


#endif

/****************************************************************************/

