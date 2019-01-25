/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/iodevices/OutputDevice.h>

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
    for (auto i : mapArg) {
        myMap[i.first] = i.second;
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
        return i->second;
    }
    return defaultValue;
}


double
Parameterised::getDouble(const std::string& key, const double defaultValue) const {
    std::map<std::string, std::string>::const_iterator i = myMap.find(key);
    if (i != myMap.end()) {
        try {
            return StringUtils::toDouble(i->second);
        } catch (NumberFormatException&) {
            WRITE_WARNING("Invalid conversion from string to double (" + i->second + ")");
            return defaultValue;
        } catch (EmptyData&) {
            WRITE_WARNING("Invalid conversion from string to double (empty value)");
            return defaultValue;
        }
    }
    return defaultValue;
}


void
Parameterised::clearParameter() {
    myMap.clear();
}


const std::map<std::string, std::string>&
Parameterised::getParametersMap() const {
    return myMap;
}


void
Parameterised::writeParams(OutputDevice& device) const {
    // iterate over all parameters and write it
    for (auto i : myMap) {
        device.openTag(SUMO_TAG_PARAM);
        device.writeAttr(SUMO_ATTR_KEY, StringUtils::escapeXML(i.first));
        device.writeAttr(SUMO_ATTR_VALUE, StringUtils::escapeXML(i.second));
        device.closeTag();
    }
}

/****************************************************************************/

