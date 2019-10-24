/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
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
#include <config.h>

#include <sstream>
#include <iostream>
#include "SUMOTime.h"
#include "StringTokenizer.h"
#include "StringUtils.h"
#include "StdDefs.h"
#include "MsgHandler.h"


// ===========================================================================
// type definitions
// ===========================================================================
SUMOTime DELTA_T = 1000;


// ===========================================================================
// method definitions
// ===========================================================================

SUMOTime
string2time(const std::string& r) {
    if (r.find(":") == std::string::npos) {
        const double time = StringUtils::toDouble(r);
        if (time > STEPS2TIME(SUMOTime_MAX)) {
            throw TimeFormatException("Input string '" + r + "' exceeds the time value range.");
        }
        return TIME2STEPS(time);
    } else {
        // try to parse jj:hh:mm:ss.s
        std::vector<std::string> hrt = StringTokenizer(r, ":").getVector();
        if (hrt.size() == 3) {
            //std::cout << "parsed '" << r << "' as " << (3600 * string2time(hrt[0]) + 60 * string2time(hrt[1]) + string2time(hrt[2])) << "\n";
            return 3600 * string2time(hrt[0]) + 60 * string2time(hrt[1]) + string2time(hrt[2]);
        } else if (hrt.size() == 4) {
            //std::cout << "parsed '" << r << "' as " << (24 * 3600 * string2time(hrt[0]) + 3600 * string2time(hrt[1]) + 60 * string2time(hrt[2]) + string2time(hrt[3])) << "\n";
            return 24 * 3600 * string2time(hrt[0]) + 3600 * string2time(hrt[1]) + 60 * string2time(hrt[2]) + string2time(hrt[3]);
        }
        throw TimeFormatException("Input string '" + r + "' is not a valid time format (jj:HH:MM:SS.S).");
    }
}


std::string
time2string(SUMOTime t) {
    std::ostringstream oss;
    oss.setf(oss.fixed);
    oss.precision(gPrecision);
    const SUMOTime second = TIME2STEPS(1);
    if (t < 0) {
        oss << "-";
    }
    // needed for signed zero errors, see #5926
    t = abs(t);
    if (gHumanReadableTime) {
        const SUMOTime minute = 60 * second;
        const SUMOTime hour = 60 * minute;
        const SUMOTime day = 24 * hour;
        // 123456 -> "00:00:12.34"
        if (t > day) {
            oss << t / day << ":";
            t %= day;
        }
        // hours, pad with zero
        if (t / hour < 10) {
            oss << "0";
        }
        oss << t / hour << ":";
        // minutes, pad with zero
        t %= hour;
        if (t / minute < 10) {
            oss << "0";
        }
        oss << t / minute << ":";
        // seconds, pad with zero
        t %= minute;
        if (t / second < 10) {
            oss << "0";
        }
        if (t % second != 0 || TS != 1.) {
            oss << STEPS2TIME(t);
        } else {
            oss << t / second;
        }
    } else {
        if (t >= TIME2STEPS(10)) {
            oss << t / TIME2STEPS(10);
            t %= TIME2STEPS(10);
        }
        oss << STEPS2TIME(t);
    }
    return oss.str();
}


bool checkStepLengthMultiple(const SUMOTime t, const std::string& error) {
    if (t % DELTA_T != 0) {
        WRITE_WARNING("The given time value " + time2string(t) + " is not a multiple of the step length " + time2string(DELTA_T) + error + ".")
    }
    // next line used to fix build
    return false;
}


/****************************************************************************/
