/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Parameterised.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// A super class for objects with additional parameters
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "utils/iodevices/OutputDevice.h"
#include "utils/common/StringUtils.h"
#include "TplConvert.h"
#include "Parameterised.h"


// ===========================================================================
// method definitions
// ===========================================================================
Parameterised::Parameterised() {}


Parameterised::~Parameterised() {}


Parameterised::Parameterised(const std::map<std::string, std::string>& mapArg)
    : myMap(mapArg) {
}


void
Parameterised::setParameter(const std::string& key, const std::string& value) {
    myMap[key] = value;
}


void
Parameterised::unsetParameter(const std::string& key) {
    myMap.erase(key);
}


void
Parameterised::updateParameter(const std::map<std::string, std::string>& mapArg) {
    for (std::map<std::string, std::string>::const_iterator i = mapArg.begin(); i != mapArg.end(); ++i) {
        myMap[(*i).first] = (*i).second;
    }
}


bool
Parameterised::knowsParameter(const std::string& key) const {
    return myMap.find(key) != myMap.end();
}


const std::string
Parameterised::getParameter(const std::string& key, const std::string& defaultValue) const {
    std::map<std::string, std::string>::const_iterator i = myMap.find(key);
    if (i != myMap.end()) {
        return (*i).second;
    }
    return defaultValue;
}


double
Parameterised::getDouble(const std::string& key, const double defaultValue) const {
    std::map<std::string, std::string>::const_iterator i = myMap.find(key);
    if (i != myMap.end()) {
        return TplConvert::_2double(i->second.c_str());
    }
    return defaultValue;
}


void
Parameterised::clearParameter() {
    myMap.clear();
}

void
Parameterised::writeParams(OutputDevice& out) const {
    for (std::map<std::string, std::string>::const_iterator j = myMap.begin(); j != myMap.end(); ++j) {
        out.openTag(SUMO_TAG_PARAM);
        out.writeAttr(SUMO_ATTR_KEY, StringUtils::escapeXML((*j).first));
        out.writeAttr(SUMO_ATTR_VALUE, StringUtils::escapeXML((*j).second));
        out.closeTag();
    }
}

/****************************************************************************/

