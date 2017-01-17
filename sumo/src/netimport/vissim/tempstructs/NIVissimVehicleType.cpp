/****************************************************************************/
/// @file    NIVissimVehicleType.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// -------------------
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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


#include "NIVissimVehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

NIVissimVehicleType::DictType NIVissimVehicleType::myDict;

NIVissimVehicleType::NIVissimVehicleType(const std::string& name,
        const std::string& category, const RGBColor& color)
    : myName(name), myCategory(category),
      myColor(color) {}


NIVissimVehicleType::~NIVissimVehicleType() {}



bool
NIVissimVehicleType::dictionary(int id, const std::string& name, const std::string& category,
                                const RGBColor& color) {
    NIVissimVehicleType* o = new NIVissimVehicleType(name, category, color);
    if (!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}


bool
NIVissimVehicleType::dictionary(int id, NIVissimVehicleType* o) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimVehicleType*
NIVissimVehicleType::dictionary(int id) {
    DictType::iterator i = myDict.find(id);
    if (i == myDict.end()) {
        return 0;
    }
    return (*i).second;
}

void
NIVissimVehicleType::clearDict() {
    for (DictType::iterator i = myDict.begin(); i != myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}



/****************************************************************************/

