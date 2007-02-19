/****************************************************************************/
/// @file    NBCont.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// Some list definitions
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
#ifndef NBCont_h
#define NBCont_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class NBEdge;


// ===========================================================================
// container definitions
// ===========================================================================
/** structure specifying a certain lane on a certain edge */
class EdgeLane
{
public:
    /// The according edge (semantics may change)
    NBEdge *edge;

    /// The according lane (semantics may change)
    size_t lane;

    /// The id of the traffic light this link is steered by
    std::string tlID;

    /// The position within the traffic light logic of this link's information
    size_t tlLinkNo;

    /// output operator
    friend bool operator==(const EdgeLane &lhs, const EdgeLane &rhs)
    {
        return lhs.edge==rhs.edge && lhs.lane==rhs.lane;
    }

};


/** container for (sorted) lanes of edges */
typedef std::vector<EdgeLane> EdgeLaneVector;


/** container for (sorted) edges */
typedef std::vector<NBEdge*> EdgeVector;


/** container for (sorted) lanes.
    The lanes are sorted from rightmost (id=0) to leftmost (id=nolanes-1) */
typedef std::vector<size_t> LaneVector;


#endif

/****************************************************************************/

