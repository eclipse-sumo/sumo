/****************************************************************************/
/// @file    PCTypeMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// A storage for type mappings
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
#include "PCTypeMap.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
PCTypeMap::PCTypeMap() throw() {}


PCTypeMap::~PCTypeMap() throw() {}


bool
PCTypeMap::add(const std::string &id, const std::string &newid,
               const std::string &color, const std::string &prefix,
               int layer, bool discard, bool allowFill) throw() {
    if (has(id)) {
        return false;
    }
    TypeDef td;
    td.id = newid;
    td.color = color;
    td.layer = layer;
    td.discard = discard;
    td.allowFill = allowFill;
    td.prefix = prefix;
    myTypes[id] = td;
    return true;
}


const PCTypeMap::TypeDef &
PCTypeMap::get(const std::string &id) throw() {
    return myTypes.find(id)->second;
}


bool
PCTypeMap::has(const std::string &id) throw() {
    return myTypes.find(id)!=myTypes.end();
}



/****************************************************************************/

