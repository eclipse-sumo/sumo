/****************************************************************************/
/// @file    SUMOTime.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// Variables, methods, and tools for internal time representation
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

