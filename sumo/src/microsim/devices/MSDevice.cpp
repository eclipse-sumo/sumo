/****************************************************************************/
/// @file    MSDevice.cpp
/// @author  Daniel Krajzewicz
/// @date    14.08.2013
/// @version $Id: MSDevice_Example.cpp 13989 2013-05-23 11:40:37Z namdre $
///
// Abstract in-vehicle device
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSVehicle.h>
#include "MSDevice.h"
#include <microsim/devices/MSDevice_Vehroutes.h>
#include <microsim/devices/MSDevice_Tripinfo.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSDevice_Person.h>
#include <microsim/devices/MSDevice_HBEFA.h>
#include <microsim/devices/MSDevice_Example.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice::insertOptions() {
    MSDevice_Routing::insertOptions();
    MSDevice_HBEFA::insertOptions();
    MSDevice_Example::insertOptions();
}


void
MSDevice::buildVehicleDevices(SUMOVehicle& v, std::vector<MSDevice*>& into) {
    MSDevice_Vehroutes::buildVehicleDevices(v, into);
    MSDevice_Tripinfo::buildVehicleDevices(v, into);
    MSDevice_Routing::buildVehicleDevices(v, into);
    MSDevice_HBEFA::buildVehicleDevices(v, into);
    MSDevice_Example::buildVehicleDevices(v, into);
}


/****************************************************************************/

