/****************************************************************************/
/// @file    MSGlobals.cpp
/// @author  Daniel Krajzewicz
/// @date    late summer 2003
/// @version $Id$
///
// Some static variables for faster access
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

size_t MSGlobals::gTimeToGridlock = 300;

bool MSGlobals::gCheck4Accidents;

#ifdef HAVE_MESOSIM
bool MSGlobals::gStateLoaded;
#endif

SUMOReal MSGlobals::gMinLaneChangeSight;
int MSGlobals::gMinLaneChangeSightEdges;

bool MSGlobals::gUsingC2C = false;

SUMOReal MSGlobals::gLANRange = 100;

SUMOReal MSGlobals::gNumberOfSendingPos = 732;

SUMOReal MSGlobals::gInfoPerPaket = 14;

SUMOTime MSGlobals::gLANRefuseOldInfosOffset = 30 * 60;

SUMOReal MSGlobals::gAddInfoFactor = (SUMOReal) 1.2;

#ifdef HAVE_MESOSIM
bool MSGlobals::gUseMesoSim;
MELoop *MSGlobals::gMesoNet;
#endif


/****************************************************************************/

