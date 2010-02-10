/****************************************************************************/
/// @file    MSCORN.cpp
/// @author  Daniel Krajzewicz
/// @date    2004
/// @version $Id$
///
// A storage for optional things to compute
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include "MSVehicle.h"
#include "MSCORN.h"
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/StringUtils.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
bool MSCORN::myWished[CORN_MAX];
bool MSCORN::myFirstCall[CORN_MAX];


// ===========================================================================
// method definitions
// ===========================================================================
void
MSCORN::init() {
    // TrafficOnline output files & settings
    for (int i=0; i<CORN_MAX; ++i) {
        myWished[i] = false;
        myFirstCall[i] = true;
    }
}


bool
MSCORN::wished(Function f) {
    return myWished[(int) f];
}


void
MSCORN::setWished(Function f) {
    myWished[(int) f] = true;
    switch (f) {
    case CORN_OUT_TRIPDURATIONS:
        setWished(CORN_VEH_DEPART_INFO);
        setWished(CORN_VEH_ARRIVAL_INFO);
        setWished(CORN_VEH_WAITINGTIME);
        setWished(CORN_VEH_VAPORIZED);
        break;
    case CORN_OUT_VEHROUTES:
        setWished(CORN_VEH_DEPART_TIME);
        setWished(CORN_VEH_SAVEREROUTING);
        setWished(CORN_VEH_DEPART_INFO);
        setWished(CORN_VEH_ARRIVAL_INFO);
        break;
    case CORN_OUT_EMISSIONS:
        setWished(CORN_MEAN_VEH_TRAVELTIME);
        setWished(CORN_MEAN_VEH_WAITINGTIME);
        break;
    case CORN_MEAN_VEH_TRAVELTIME:
        setWished(CORN_VEH_DEPART_TIME);
        break;
    case CORN_MEAN_VEH_WAITINGTIME:
        setWished(CORN_VEH_DEPART_TIME);
        break;
    default:
        break;
    }
}



/****************************************************************************/

