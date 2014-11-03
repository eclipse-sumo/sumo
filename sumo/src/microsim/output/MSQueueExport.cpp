/****************************************************************************/
/// @file    MSQueueExport.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Mario Krumnow
/// @author  Michael Behrisch
/// @date    2012-04-26
/// @version $Id$
///
// Export the queueing length in front of a junction (very experimental!)
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2014 DLR (http://www.dlr.de/) and contributors
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

#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSQueueExport.h"
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSQueueExport::write(OutputDevice& of, SUMOTime timestep) {
    of.openTag("data").writeAttr("timestep", time2string(timestep));
    writeEdge(of);
    of.closeTag();
}


void
MSQueueExport::writeEdge(OutputDevice& of) {
    of.openTag("lanes");
    MSEdgeControl& ec = MSNet::getInstance()->getEdgeControl();
    const std::vector<MSEdge*>& edges = ec.getEdges();
    for (std::vector<MSEdge*>::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        MSEdge& edge = **e;
        const std::vector<MSLane*>& lanes = edge.getLanes();
        for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
            writeLane(of, **lane);
        }
    }
    of.closeTag();
}


void
MSQueueExport::writeLane(OutputDevice& of, const MSLane& lane) {
    // maximum of all vehicle waiting times
    SUMOReal queueing_time = 0.0;
    // back of last stopped vehicle (XXX does not check for continuous queue)
    SUMOReal queueing_length = 0.0;
    // back of last slow vehicle (XXX does not check for continuous queue)
    SUMOReal queueing_length2 = 0.0;
    const SUMOReal threshold_velocity = 5 / 3.6; // slow

    if (!lane.empty()) {
        for (MSLane::VehCont::const_iterator it_veh = lane.myVehicles.begin(); it_veh != lane.myVehicles.end(); ++it_veh) {
            const MSVehicle& veh = **it_veh;
            if (!veh.isOnRoad()) {
                continue;
            }

            if (veh.getWaitingSeconds() > 0) {
                queueing_time = MAX2(veh.getWaitingSeconds(), queueing_time);
                const SUMOReal veh_back_to_lane_end = (lane.getLength() - veh.getPositionOnLane()) + veh.getVehicleType().getLength();
                queueing_length = MAX2(veh_back_to_lane_end, queueing_length);
            }

            //Experimental
            if (veh.getSpeed() < (threshold_velocity) && (veh.getPositionOnLane() > (veh.getLane()->getLength()) * 0.25)) {
                const SUMOReal veh_back_to_lane_end = (lane.getLength() - veh.getPositionOnLane()) + veh.getVehicleType().getLength();
                queueing_length2 = MAX2(veh_back_to_lane_end, queueing_length2);
            }
        }
    }

    //Output
    if (queueing_length > 1 || queueing_length2 > 1) {
        of.openTag("lane").writeAttr("id", lane.getID()).writeAttr("queueing_time", queueing_time).writeAttr("queueing_length", queueing_length);
        of.writeAttr("queueing_length_experimental", queueing_length2).closeTag();
    }
}


/****************************************************************************/
