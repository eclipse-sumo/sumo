/***************************************************************************
                          PCElmar.cpp
    A storage for type mappings
                             -------------------
    project              : SUMO
    subproject           : PolyConvert
    begin                : Mon, 05 Dec 2005
    copyright            : (C) 2005 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2006/08/01 07:52:47  dkrajzew
// polyconvert added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include "PCTypeMap.h"

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H


/* =========================================================================
 * method definitions
 * ======================================================================= */
PCTypeMap::PCTypeMap()
{
}


PCTypeMap::~PCTypeMap()
{
}


bool
PCTypeMap::add(const std::string &id, const std::string &newid,
               const std::string &color, const std::string &prefix,
               int layer, bool discard, bool allowFill)
{
    if(has(id)) {
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


