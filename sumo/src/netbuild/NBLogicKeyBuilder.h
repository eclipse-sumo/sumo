/****************************************************************************/
/// @file    NBLogicKeyBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The builder of logic keys
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
#ifndef NBLogicKeyBuilder_h
#define NBLogicKeyBuilder_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <sstream>
#include "NBContHelper.h"


// ===========================================================================
// class declarations
// ===========================================================================
class NBNode;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * NBLogicKeyBuilder
 * Builds the key that fits into all instances of the logic
 * !!! deprecated?
 */
class NBLogicKeyBuilder
{
private:
    void appendEdgeDescription(std::ostringstream &to,
                               NBNode *junction, const EdgeVector * const edges,
                               EdgeVector::const_iterator &pos);
    void appendEdgesLaneDescriptions(std::ostringstream &to,
                                     const EdgeVector * const edges, NBEdge *edge,
                                     EdgeVector::const_iterator &pos);
    void appendLaneConnectionDescriptions(std::ostringstream &to,
                                          const EdgeVector * const edges, const EdgeLaneVector &connected,
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


#endif

/****************************************************************************/

