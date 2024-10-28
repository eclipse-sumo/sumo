/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
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
/// @file    StdDefs.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2014-01-07
///
/****************************************************************************/
#include "StdDefs.h"
#include <sstream>


// set by option --precision (see SystemFrame.cpp)
int gPrecision = 2;
int gPrecisionGeo = 6;
int gPrecisionRandom = 4;
bool gHumanReadableTime = false;
bool gSimulation = false;
bool gIgnoreUnknownVClass = false;
double gWeightsRandomFactor = 1;
double gWeightsWalkOppositeFactor = 1;
std::string gLanguage = "C";
int GUIDesignHeight = 23;

bool gDebugFlag1 = false;
bool gDebugFlag2 = false;
bool gDebugFlag3 = false;
bool gDebugFlag4 = false;
bool gDebugFlag5 = false;
bool gDebugFlag6 = false;

double truncate(double x, int fractionBits) {
    return ceil(x * (1 << fractionBits)) / (1 << fractionBits);
}

double roundBits(double x, int fractionBits) {
    const double x2 = x * (1 << fractionBits);
    const double rounded = x2 < 0 ? ceil(x2 - 0.5) : floor(x2 + 0.5);
    return rounded / (1 << fractionBits);
}

double roundDecimal(double x, int precision) {
    const double p = pow(10, precision);
    const double x2 = x * p;
    return (x2 < 0 ? ceil(x2 - 0.5) : floor(x2 + 0.5)) / p;
}

int
getScalingQuota(double frac, int loaded) {
    if (frac < 0 || frac == 1.) {
        return 1;
    }
    const int base = (int)frac;
    const int resolution = 1000;
    const int intFrac = (int)floor((frac - base) * resolution + 0.5);
    // apply % twice to avoid integer overflow
    if (((loaded % resolution) * intFrac) % resolution < intFrac) {
        return base + 1;
    }
    return base;
}

/****************************************************************************/
