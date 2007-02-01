/****************************************************************************/
/// @file    NIVissimClosures.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// -------------------
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
#ifndef NIVissimClosures_h
#define NIVissimClosures_h
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


#include <string>
#include <map>
#include <utils/common/IntVector.h>

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

