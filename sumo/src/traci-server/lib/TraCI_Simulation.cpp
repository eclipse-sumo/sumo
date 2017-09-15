/****************************************************************************/
/// @file    TraCI_Simulation.cpp
/// @author  Laura Bieker-Walz
/// @author  Robert Hilbrich
/// @date    15.09.2017
/// @version $Id: TraCI_POI.cpp 25277 2017-07-20 11:15:41Z behrisch $
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017-2017 DLR (http://www.dlr.de/) and contributors
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

#ifndef NO_TRACI

#include <utils/common/StdDefs.h>
#include <utils/geom/GeoConvHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSInsertionControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSStateHandler.h>
#include <microsim/MSStoppingPlace.h>
#include <traci-server/lib/TraCI.h>
#include "TraCI_Simulation.h"
#include <traci-server/TraCIDefs.h>


SUMOTime 
TraCI_Simulation::getCurrentTime(){
    return MSNet::getInstance()->getCurrentTimeStep();
}

/*todo move server methods 
int 
TraCI_Simulation::getLoadedNumber(){
    return 0;
}*/


SUMOTime 
TraCI_Simulation::getDeltaT(){
    return (int)DELTA_T;
}
#endif