/****************************************************************************/
/// @file    NIVissimVehTypeClass.cpp
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
#include <utils/gfx/RGBColor.h>
#include <utils/common/IntVector.h>
#include "NIVissimVehTypeClass.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


NIVissimVehTypeClass::DictType NIVissimVehTypeClass::myDict;

NIVissimVehTypeClass::NIVissimVehTypeClass(int id,
        const std::string &name,
        const RGBColor &color,
        IntVector &types)
        : myID(id), myName(name), myColor(color), myTypes(types)
{}

NIVissimVehTypeClass::~NIVissimVehTypeClass()
{}


bool
NIVissimVehTypeClass::dictionary(int id, const std::string &name,
                                 const RGBColor &color,
                                 IntVector &types)
{
    NIVissimVehTypeClass *o = new NIVissimVehTypeClass(id, name, color, types);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}




bool
NIVissimVehTypeClass::dictionary(int name, NIVissimVehTypeClass *o)
{
    DictType::iterator i=myDict.find(name);
    if (i==myDict.end()) {
        myDict[name] = o;
        return true;
    }
    return false;
}


NIVissimVehTypeClass *
NIVissimVehTypeClass::dictionary(int name)
{
    DictType::iterator i=myDict.find(name);
    if (i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimVehTypeClass::clearDict()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}



/****************************************************************************/

