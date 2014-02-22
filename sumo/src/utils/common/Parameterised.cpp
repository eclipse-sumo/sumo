/****************************************************************************/
/// @file    Parameterised.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A RGB-color definition
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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

#include "Parameterised.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
Parameterised::Parameterised() {}


Parameterised::~Parameterised() {}


Parameterised::Parameterised(const std::map<std::string, std::string>& mapArg)
    : myMap(mapArg)
{ }


void
Parameterised::addParameter(const std::string& key, const std::string& value) {
    myMap[key] = value;
}


void
Parameterised::addParameter(const std::map<std::string, std::string>& mapArg) {
    for (std::map<std::string, std::string>::const_iterator i = mapArg.begin(); i != mapArg.end(); ++i) {
        myMap[(*i).first] = (*i).second;
    }
}


void
Parameterised::addParameter(const Parameterised& p) {
    for (std::map<std::string, std::string>::const_iterator i = p.myMap.begin(); i != p.myMap.end(); ++i) {
        myMap[(*i).first] = (*i).second;
    }
}


bool
Parameterised::knowsParameter(const std::string& key) const {
    return myMap.find(key) != myMap.end();
}


const std::string&
Parameterised::getParameter(const std::string& key, const std::string& defaultValue) const {
    std::map<std::string, std::string>::const_iterator i = myMap.find(key);
    if (i != myMap.end()) {
        return (*i).second;
    }
    return defaultValue;
}


void
Parameterised::clearParameter() {
    myMap.clear();
}


/****************************************************************************/

