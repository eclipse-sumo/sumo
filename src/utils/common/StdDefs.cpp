/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    StdDefs.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2014-01-07
/// @version $Id$
///
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include "StdDefs.h"


// set by option --precision (see SystemFrame.cpp)
int gPrecision = 2;
int gPrecisionGeo = 6;
bool gHumanReadableTime = false;
bool gSimulation = false;
double gWeightsRandomFactor = 1;

bool gDebugFlag1 = false;
bool gDebugFlag2 = false;
bool gDebugFlag3 = false;
bool gDebugFlag4 = false;

double truncate(double x, int fractionBits) {
    return ceil(x * (1 << fractionBits)) / (1 << fractionBits);
}

double roundBits(double x, int fractionBits) {
    const double x2 = x * (1 << fractionBits);
    const double rounded = x2 < 0 ? ceil(x2 - 0.5) : floor(x2 + 0.5);
    return rounded / (1 << fractionBits);
}

/****************************************************************************/

