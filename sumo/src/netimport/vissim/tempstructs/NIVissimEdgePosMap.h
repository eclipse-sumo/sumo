/****************************************************************************/
/// @file    NIVissimEdgePosMap.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef NIVissimEdgePosMap_h
#define NIVissimEdgePosMap_h


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
// class definitions
// ===========================================================================
/**
 *
 */
class NIVissimEdgePosMap {
public:
    NIVissimEdgePosMap();
    ~NIVissimEdgePosMap();
    void add(int edgeid, SUMOReal pos);
    void add(int edgeid, SUMOReal from, SUMOReal to);
    void join(NIVissimEdgePosMap &with);
private:
    typedef std::pair<SUMOReal, SUMOReal> Range;
    typedef std::map<int, Range> ContType;
    ContType myCont;
};


#endif

/****************************************************************************/

