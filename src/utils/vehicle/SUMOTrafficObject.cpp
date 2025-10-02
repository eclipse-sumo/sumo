/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    SUMOTrafficObject.cpp
/// @author  Michael Behrisch
/// @date    2024-06-27
///
// Abstract base class for vehicle, person, and container representations
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "SUMOVehicleParameter.h"
#include "SUMOTrafficObject.h"


// ===========================================================================
// method definitions
// ===========================================================================
std::string
SUMOTrafficObject::getStringParam(const std::string& paramName, const bool required, const std::string& deflt) const {
    if (getParameter().hasParameter(paramName)) {
        return getParameter().getParameter(paramName, "");
    } else if (getVTypeParameter().hasParameter(paramName)) {
        return getVTypeParameter().getParameter(paramName, "");
    } else {
        const OptionsCont& oc = OptionsCont::getOptions();
        if (oc.exists(paramName) && oc.isSet(paramName)) {
            return oc.getValueString(paramName);
        } else {
            if (required) {
                const std::string type = isVehicle() ? "vehicle" : (isPerson() ? "person" : "container");
                throw ProcessError(TLF("Missing parameter '%' for % '%'.", paramName, type, getID()));
            } else {
                if (oc.exists(paramName)) {
                    return oc.getValueString(paramName);
                }
                return deflt;
            }
        }
    }
}


double
SUMOTrafficObject::getFloatParam(const std::string& paramName, const bool required, const double deflt, bool checkDist) const {
    const std::string val = getStringParam(paramName, required, toString(deflt, 16));
    if (!checkDist) {
        try {
            return StringUtils::toDouble(val);
        } catch (NumberFormatException& e) {
            const std::string type = isVehicle() ? "vehicle" : (isPerson() ? "person" : "container");
            WRITE_ERRORF(TL("Invalid float value '%' for parameter '%' in % '%' (%)."), val, paramName, type, getID(), e.what());
            return deflt;
        }
    }
    try {
        Distribution_Parameterized dist(val);
        const std::string& error = dist.isValid();
        if (error != "") {
            throw ProcessError(error);
        }
        return dist.sample();
    } catch (const ProcessError& e) {
        const std::string type = isVehicle() ? "vehicle" : (isPerson() ? "person" : "container");
        WRITE_ERRORF(TL("Invalid distribution / float value '%' for parameter '%' in % '%' (%)."), val, paramName, type, getID(), e.what());
        return deflt;
    }
}


bool
SUMOTrafficObject::getBoolParam(const std::string& paramName, const bool required, const bool deflt) const {
    const std::string val = getStringParam(paramName, required, toString(deflt));
    try {
        return StringUtils::toBool(val);
    } catch (const ProcessError&) {
        const std::string type = isVehicle() ? "vehicle" : (isPerson() ? "person" : "container");
        WRITE_ERRORF(TL("Invalid boolean value '%' for parameter '%' in % '%'."), val, paramName, type, getID());
        return deflt;
    }
}


SUMOTime
SUMOTrafficObject::getTimeParam(const std::string& paramName, const bool required, const SUMOTime deflt) const {
    const std::string val = getStringParam(paramName, required, time2string(deflt));
    try {
        return string2time(val);
    } catch (const ProcessError&) {
        const std::string type = isVehicle() ? "vehicle" : (isPerson() ? "person" : "container");
        WRITE_ERRORF(TL("Invalid time value '%' for parameter '%' in % '%'."), val, paramName, type, getID());
        return deflt;
    }
}


/****************************************************************************/
