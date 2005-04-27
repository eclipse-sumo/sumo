#ifndef NBLogicKeyBuilder_h
#define NBLogicKeyBuilder_h
//---------------------------------------------------------------------------//
//                        NBLogicKeyBuilder.h -
//  The builder of logic keys
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.3  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <string>
#include <sstream>
#include "NBContHelper.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NBNode;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * NBLogicKeyBuilder
 * Builds the key that fits into all instances of the logic
 * !!! deprecated?
 */
class NBLogicKeyBuilder {
private:
    void appendEdgeDescription(std::ostringstream &to,
        NBNode *junction, const EdgeVector * const edges,
        EdgeVector::const_iterator &pos);
    void appendEdgesLaneDescriptions(std::ostringstream &to,
        const EdgeVector * const edges, NBEdge *edge,
        EdgeVector::const_iterator &pos);
    void appendLaneConnectionDescriptions(std::ostringstream &to,
        const EdgeVector * const edges, const EdgeLaneVector *connected,
        EdgeVector::const_iterator &pos);
    void appendDetailedConnectionDescription(std::ostringstream &to,
        const EdgeVector * const edges, const EdgeLane &edgelane,
        EdgeVector::const_iterator &pos);
public:
    /** chars from 'a' to 'z' to get alphanumerical values instead of
        numerical only */
    static char convert[];
    std::string buildKey(NBNode *junction,
        const EdgeVector * const edges);
    /// rotates the key by the given amount
    std::string rotateKey(std::string key, int norot);
    /// permutates the key
    std::string rotateKey(const std::string &run);
};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

