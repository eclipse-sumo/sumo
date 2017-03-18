/****************************************************************************/
/// @file    Distribution_Points.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// The description of a distribution by a curve
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

#include <cassert>
#include <sstream>
#include <iomanip>
#include <utils/common/StdDefs.h>
#include "Distribution_Points.h"


// ===========================================================================
// method definitions
// ===========================================================================
Distribution_Points::Distribution_Points(const std::string& id)
    : Distribution(id) {}


Distribution_Points::~Distribution_Points() {}


double
Distribution_Points::getMax() const {
    assert(getVals().size() > 0);
    return getVals().back();
}


std::string
Distribution_Points::toStr(std::streamsize accuracy) const {
    std::stringstream oss;
    oss << std::setprecision(accuracy);
    const std::vector<double> vals = getVals();
    for (int i = 0; i < (int)vals.size(); i++) {
        if (i > 0) {
            oss << ",";
        }
        oss << vals[i] << ":" << getProbs()[i];
    }
    return "points(" + oss.str() + ")";
}


/****************************************************************************/
