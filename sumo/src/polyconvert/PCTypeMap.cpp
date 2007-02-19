/****************************************************************************/
/// @file    PCTypeMap.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 05 Dec 2005
/// @version $Id$
///
// }
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
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#include <string>
#include <map>
#include "PCTypeMap.h"

#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// method definitions
// ===========================================================================
PCTypeMap::PCTypeMap()
{}


PCTypeMap::~PCTypeMap()
{}


bool
PCTypeMap::add(const std::string &id, const std::string &newid,
                   const std::string &color, const std::string &prefix,
                   int layer, bool discard, bool allowFill)
{
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
PCTypeMap::get(const std::string &id)
{
    return myTypes.find(id)->second;
}


bool
PCTypeMap::has(const std::string &id)
{
    return myTypes.find(id)!=myTypes.end();
}



/****************************************************************************/

