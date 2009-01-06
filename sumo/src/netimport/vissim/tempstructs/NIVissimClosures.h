/****************************************************************************/
/// @file    NIVissimClosures.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
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
#ifndef NIVissimClosures_h
#define NIVissimClosures_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <map>
#include <utils/common/VectorHelper.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimClosures
{
public:
    NIVissimClosures(const std::string &id,
                     int from_node, int to_node,
                     IntVector &overEdges);
    ~NIVissimClosures();
    static bool dictionary(const std::string &id,
                           int from_node, int to_node, IntVector &overEdges);
    static bool dictionary(const std::string &name, NIVissimClosures *o);
    static NIVissimClosures *dictionary(const std::string &name);
    static void clearDict();
private:
    typedef std::map<std::string, NIVissimClosures*> DictType;
    static DictType myDict;
    const std::string myID;
    int myFromNode, myToNode;
    IntVector myOverEdges;
};


#endif

/****************************************************************************/

