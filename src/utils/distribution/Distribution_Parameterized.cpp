/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    Distribution_Parameterized.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// A distribution described by parameters such as the mean value and std-dev
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utils/common/RandHelper.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>

#include "Distribution_Parameterized.h"


// ===========================================================================
// method definitions
// ===========================================================================
/// @brief Constructor for any temporary distribution parsed directly from the description
Distribution_Parameterized::Distribution_Parameterized(const std::string& description) :
    Distribution("") {
    myParameter = {0., 0.};
    parse(description, true);
}


Distribution_Parameterized::Distribution_Parameterized(const std::string& id, double mean, double deviation) :
    Distribution(id) {
    myParameter.push_back(mean);
    myParameter.push_back(deviation);
}


Distribution_Parameterized::Distribution_Parameterized(const std::string& id, double mean, double deviation, double min, double max) :
    Distribution(id) {
    myParameter.push_back(mean);
    myParameter.push_back(deviation);
    myParameter.push_back(min);
    myParameter.push_back(max);
}


Distribution_Parameterized::~Distribution_Parameterized() {}


void
Distribution_Parameterized::parse(const std::string& description, const bool hardFail) {
    try {
        const std::string distName = description.substr(0, description.find('('));
        if (distName == "norm" || distName == "normc") {
            const std::vector<std::string> params = StringTokenizer(description.substr(distName.size() + 1, description.size() - distName.size() - 2), ',').getVector();
            myParameter.resize(params.size());
            std::transform(params.begin(), params.end(), myParameter.begin(), StringUtils::toDouble);
            setID(distName);
        } else {
            myParameter[0] = StringUtils::toDouble(description);
        }
        if (myParameter.size() == 1) {
            myParameter.push_back(0.);
        }
    } catch (...) {
        // set default distribution parameterized
        myParameter = {0., 0.};
        if (hardFail) {
            throw ProcessError(TL("Invalid format of distribution parameterized"));
        } else {
            WRITE_ERROR(TL("Invalid format of distribution parameterized"));
        }
    }
}


bool
Distribution_Parameterized::isValidDescription(const std::string& description) {
    try {
        Distribution_Parameterized dummy(description);
        const std::string error = dummy.isValid();
        if (error == "") {
            return true;
        }
        WRITE_ERROR(error);
    } catch (...) {
        WRITE_ERROR(TL("Invalid format of distribution parameterized"));
    }
    return false;
}


double
Distribution_Parameterized::sample(SumoRNG* which) const {
    if (myParameter[1] <= 0.) {
        return myParameter[0];
    }
    double val = RandHelper::randNorm(myParameter[0], myParameter[1], which);
    if (myParameter.size() > 2) {
        const double min = myParameter[2];
        const double max = getMax();
        while (val < min || val > max) {
            val = RandHelper::randNorm(myParameter[0], myParameter[1], which);
        }
    }
    return val;
}


double
Distribution_Parameterized::getMax() const {
    if (myParameter[1] <= 0.) {
        return myParameter[0];
    }
    return myParameter.size() > 3 ? myParameter[3] : std::numeric_limits<double>::infinity();
}


double
Distribution_Parameterized::getMin() const {
    if (myParameter[1] <= 0.) {
        return myParameter[0];
    }
    return myParameter.size() > 2 ? myParameter[2] : -std::numeric_limits<double>::infinity();
}


std::vector<double>&
Distribution_Parameterized::getParameter() {
    return myParameter;
}


const std::vector<double>&
Distribution_Parameterized::getParameter() const {
    return myParameter;
}


std::string
Distribution_Parameterized::toStr(std::streamsize accuracy) const {
    if (myParameter[1] < 0) {
        // only write simple speedFactor
        return toString(myParameter[0]);
    } else {
        return (myParameter[1] == 0.
                ? myID + "(" + toString(myParameter[0], accuracy) + "," + toString(myParameter[1], accuracy) + ")"
                : myID + "(" + joinToString(myParameter, ",", accuracy) + ")");
    }
}


const std::string
Distribution_Parameterized::isValid() const {
    if (myParameter[1] > 0.) {
        if (getMin() > getMax()) {
            return TLF("minimum value % larger than maximum %", getMin(), getMax());
        }
        if (getMin() > myParameter[0] + 3 * myParameter[1]) {
            return TLF("minimum value % too large for distribution with mean % and deviation %", myParameter[2], myParameter[0], myParameter[1]);
        }
        if (getMax() < myParameter[0] - 3 * myParameter[1]) {
            return TLF("maximum value % too small for distribution with mean % and deviation %", myParameter[3], myParameter[0], myParameter[1]);
        }
        if (myParameter.size() > 3 && myParameter[3] - myParameter[2] < NUMERICAL_EPS * myParameter[1]) {
            return TLF("maximum value % and minimum value % too close for distribution with mean % and deviation %", myParameter[3], myParameter[2], myParameter[0], myParameter[1]);
        }
    }
    return "";
}


/****************************************************************************/
