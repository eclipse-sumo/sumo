/****************************************************************************/
/// @file    Distribution_Parameterized.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// A distribution described by parameters such as the mean value and std-dev
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

#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include "Distribution_Parameterized.h"


// ===========================================================================
// method definitions
// ===========================================================================
Distribution_Parameterized::Distribution_Parameterized(const std::string& description)
    : Distribution("") {
    const std::string distName = description.substr(0, description.find('('));
    if (distName == "norm" || distName == "normc") {
        std::vector<std::string> params = StringTokenizer(description.substr(distName.size() + 1, description.size() - distName.size() - 2), ',').getVector();
        myParameter.resize(params.size());
        std::transform(params.begin(), params.end(), myParameter.begin(), TplConvert::_str2double);
        setID(distName);
    } else {
        myParameter.push_back(TplConvert::_str2double(description));
    }
    assert(!myParameter.empty());
    if (myParameter.size() == 1) {
        myParameter.push_back(0.);
    }
}


Distribution_Parameterized::Distribution_Parameterized(const std::string& id,
        double mean, double deviation)
    : Distribution(id) {
    myParameter.push_back(mean);
    myParameter.push_back(deviation);
}


Distribution_Parameterized::Distribution_Parameterized(const std::string& id,
        double mean, double deviation, double min, double max)
    : Distribution(id) {
    myParameter.push_back(mean);
    myParameter.push_back(deviation);
    myParameter.push_back(min);
    myParameter.push_back(max);
}


Distribution_Parameterized::~Distribution_Parameterized() {}


double
Distribution_Parameterized::sample(MTRand* which) const {
    if (myParameter[1] == 0.) {
        return myParameter[0];
    }
    double val = which == 0 ? RandHelper::randNorm(myParameter[0], myParameter[1]) : which->randNorm(myParameter[0], myParameter[1]);
    if (myParameter.size() > 2) {
        const double min = myParameter[2];
        const double max = getMax();
        while (val < min || val > max) {
            val = which == 0 ? RandHelper::randNorm(myParameter[0], myParameter[1]) : which->randNorm(myParameter[0], myParameter[1]);
        }
    }
    return val;
}


double
Distribution_Parameterized::getMax() const {
    if (myParameter[1] == 0.) {
        return myParameter[0];
    }
    return myParameter.size() > 3 ? myParameter[3] : std::numeric_limits<double>::infinity();
}


std::string
Distribution_Parameterized::toStr(std::streamsize accuracy) const {
    return myParameter[1] == 0. ? toString(myParameter[0]) : myID + "(" + joinToString(myParameter, ",", accuracy) + ")";
}


/****************************************************************************/

