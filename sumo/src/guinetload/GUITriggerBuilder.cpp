/****************************************************************************/
/// @file    GUITriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 26.04.2004
/// @version $Id$
///
// Builds trigger objects for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <fstream>
#include <guisim/GUILaneSpeedTrigger.h>
#include <guisim/GUIEmitter.h>
#include <guisim/GUITriggeredRerouter.h>
#include <guisim/GUIE1VehicleActor.h>
#include <guisim/GUIBusStop.h>
#include "GUITriggerBuilder.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUITriggerBuilder::GUITriggerBuilder() throw() {}


GUITriggerBuilder::~GUITriggerBuilder() throw() {}


MSLaneSpeedTrigger*
GUITriggerBuilder::buildLaneSpeedTrigger(MSNet &net,
        const std::string &id, const std::vector<MSLane*> &destLanes,
        const std::string &file) throw(ProcessError) {
    return new GUILaneSpeedTrigger(id, destLanes, file);
}


void
GUITriggerBuilder::buildLaneEmitTrigger(MSNet &net,
                                        const std::string &id,
                                        MSLane *destLane,
                                        SUMOReal pos,
                                        const std::string &file) throw() {
    new GUIEmitter(id, net, destLane, pos, file);
}


void
GUITriggerBuilder::buildRerouter(MSNet &, const std::string &id,
                                 std::vector<MSEdge*> &edges,
                                 SUMOReal prob, const std::string &file, bool off) throw() {
    new GUITriggeredRerouter(id, edges, prob, file, off);
}


void
GUITriggerBuilder::buildVehicleActor(MSNet &, const std::string &id,
                                     MSLane *lane, SUMOReal pos, unsigned int la,
                                     unsigned int cell, unsigned int type) throw() {
    new GUIE1VehicleActor(id, lane, pos, la, cell, type);
}


void
GUITriggerBuilder::buildBusStop(MSNet &net, const std::string &id,
                                const std::vector<std::string> &lines,
                                MSLane *lane,
                                SUMOReal frompos, SUMOReal topos) throw() {
    net.addBusStop(new GUIBusStop(id, net, lines, *lane, frompos, topos));
}


/****************************************************************************/

