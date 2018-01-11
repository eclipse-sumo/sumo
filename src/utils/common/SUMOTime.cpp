/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOTime.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Variables, methods, and tools for internal time representation
/****************************************************************************/
// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <sstream>
#include "SUMOTime.h"
#include "TplConvert.h"
#include "StdDefs.h"


// ===========================================================================
// type definitions
// ===========================================================================
SUMOTime DELTA_T = 1000;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOTime
string2time(const std::string& r) {
    double time;
    std::istringstream buf(r);
    buf >> time;
    if (buf.fail() || time > STEPS2TIME(SUMOTime_MAX)) {
        throw ProcessError("Input string '" + r + "' is not a valid number or exceeds the time value range.");
    } else {
        return TIME2STEPS(time);
    }
}


std::string
time2string(SUMOTime t) {
    // 123456 -> "12.34"
    std::ostringstream oss;
    oss.setf(oss.fixed);
    oss.precision(gPrecision);
    oss << STEPS2TIME(t);
    return oss.str();
}


/****************************************************************************/

