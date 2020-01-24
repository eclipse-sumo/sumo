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
///
// A super class for objects with additional parameters
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================

#include <config.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/iodevices/OutputDevice.h>

#include "Parameterised.h"


// ===========================================================================
// method definitions
// ===========================================================================
Parameterised::Parameterised() {}


Parameterised::Parameterised(const std::map<std::string, std::string>& mapArg)
    : myMap(mapArg) {
}


Parameterised::~Parameterised() {}


void
Parameterised::setParameter(const std::string& key, const std::string& value) {
    myMap[key] = value;
}


void
Parameterised::unsetParameter(const std::string& key) {
    myMap.erase(key);
}


void
Parameterised::updateParameters(const std::map<std::string, std::string>& mapArg) {
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


std::string
Parameterised::getParametersStr(const std::string& kvsep, const std::string& sep) const {
    std::string result;
    // Generate an string using configurable seperatrs, default: "key1=value1|key2=value2|...|keyN=valueN"
    bool addSep = false;
    for (auto kv : myMap) {
        if (addSep) {
            result += sep;
        }
        result += kv.first + kvsep + kv.second;
        addSep = true;
    }
    return result;
}


void
Parameterised::setParameters(const Parameterised& params) {
    myMap = params.getParametersMap();
}


void
Parameterised::setParametersMap(const std::map<std::string, std::string>& paramsMap) {
    myMap = paramsMap;
}


void
Parameterised::setParametersStr(const std::string& paramsString, const std::string& kvsep, const std::string& sep) {
    // clear parameters
    myMap.clear();
    // separate value in a vector of string using | as separator
    std::vector<std::string> parameters = StringTokenizer(paramsString, sep).getVector();
    // iterate over all values
    for (const auto& i : parameters) {
        // obtain key and value and save it in myParameters
        std::vector<std::string> keyValue = StringTokenizer(i, kvsep).getVector();
        myMap[keyValue.front()] = keyValue.back();
    }
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


bool
Parameterised::areParametersValid(const std::string& value, bool report, const std::string& kvsep, const std::string& sep) {
    std::vector<std::string> parameters = StringTokenizer(value, sep).getVector();
    // first check if parsed parameters are valid
    for (const auto& i : parameters) {
        // check if parameter is valid
        if (!isParameterValid(i, report, kvsep, sep)) {
            // report depending of flag
            if (report) {
                WRITE_WARNING("Invalid format of parameter (" + i + ")");
            }
            return false;
        }
    }
    // all ok, then return true
    return true;
}

// ===========================================================================
// private
// ===========================================================================

bool
Parameterised::isParameterValid(const std::string& value, bool /* report */, const std::string& kvsep, const std::string& sep) {
    if (value.find(sep) != std::string::npos || value.find(kvsep) == std::string::npos) {
        return false;
    }
    // separate key and value
    std::vector<std::string> keyValue = StringTokenizer(value, kvsep).getVector();
    // Check that keyValue size is exactly 2 (key, value)
    if (keyValue.size() == 2) {
        // check if key and value contains valid characters
        if (SUMOXMLDefinitions::isValidParameterKey(keyValue.front()) == false) {
            return false;
        } else if (SUMOXMLDefinitions::isValidParameterValue(keyValue.back()) == false) {
            return false;
        } else {
            // key=value valid, then return true
            return true;
        }
    } else {
        // invalid format
        return false;
    }
}

/****************************************************************************/

