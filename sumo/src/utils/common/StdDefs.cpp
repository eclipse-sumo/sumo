/****************************************************************************/
/// @file    StdDefs.cpp
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2014-01-07
/// @version $Id$
///
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
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
#include "StdDefs.h"


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

// set by option --precision (see SystemFrame.cpp)
int gPrecision = 2;
int gPrecisionGeo = 6;

bool gDebugFlag1 = false;
bool gDebugFlag2 = false;
bool gDebugFlag3 = false;
bool gDebugFlag4 = false;
std::string gDebugSelectedVehicle = "";

/****************************************************************************/

