/****************************************************************************/
/// @file    NIVissimVehTypeClass.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2002-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <utils/common/RGBColor.h>
#include <utils/common/VectorHelper.h>
#include "NIVissimVehTypeClass.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


NIVissimVehTypeClass::DictType NIVissimVehTypeClass::myDict;

NIVissimVehTypeClass::NIVissimVehTypeClass(int id,
        const std::string& name,
        const RGBColor& color,
        std::vector<int>& types)
    : myID(id), myName(name), myColor(color), myTypes(types) {}

NIVissimVehTypeClass::~NIVissimVehTypeClass() {}


bool
NIVissimVehTypeClass::dictionary(int id, const std::string& name,
                                 const RGBColor& color,
                                 std::vector<int>& types) {
    NIVissimVehTypeClass* o = new NIVissimVehTypeClass(id, name, color, types);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}




bool
NIVissimVehTypeClass::dictionary(int name, NIVissimVehTypeClass* o) {
    DictType::iterator i = myDict.find(name);
    if (i == myDict.end()) {
        myDict[name] = o;
        return true;
    }
    return false;
}


NIVissimVehTypeClass*
NIVissimVehTypeClass::dictionary(int name) {
    DictType::iterator i = myDict.find(name);
    if (i == myDict.end()) {
        return 0;
    }
    return (*i).second;
}


void
NIVissimVehTypeClass::clearDict() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}



/****************************************************************************/

