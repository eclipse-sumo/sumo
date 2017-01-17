/****************************************************************************/
/// @file    MSGlobals.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    late summer 2003
/// @version $Id$
///
// Some static variables for faster access
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2003-2017 DLR (http://www.dlr.de/) and contributors
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

#include "MSGlobals.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variable definitions
// ===========================================================================
bool MSGlobals::gOmitEmptyEdgesOnDump;

bool MSGlobals::gUsingInternalLanes;
SUMOTime MSGlobals::gIgnoreJunctionBlocker;

SUMOTime MSGlobals::gTimeToGridlock;
SUMOTime MSGlobals::gTimeToGridlockHighways;

bool MSGlobals::gCheck4Accidents;

bool MSGlobals::gCheckRoutes;

SUMOTime MSGlobals::gLaneChangeDuration;

SUMOReal MSGlobals::gLateralResolution;

bool MSGlobals::gStateLoaded;
bool MSGlobals::gUseMesoSim;
bool MSGlobals::gMesoLimitedJunctionControl;
bool MSGlobals::gMesoOvertaking;
SUMOReal MSGlobals::gMesoTLSPenalty;
SUMOTime MSGlobals::gMesoMinorPenalty;
MELoop* MSGlobals::gMesoNet;

bool MSGlobals::gSemiImplicitEulerUpdate;

SUMOTime MSGlobals::gWaitingTimeMemory;
/****************************************************************************/

