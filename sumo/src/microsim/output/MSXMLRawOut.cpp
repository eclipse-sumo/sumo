/****************************************************************************/
/// @file    MSXMLRawOut.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Bjoern Hendriks
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Realises dumping the complete network state
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/geom/GeomHelper.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLane.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicle.h>
#include <microsim/pedestrians/MSPModel.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSContainer.h>
#include <utils/iodevices/OutputDevice.h>
#include "MSXMLRawOut.h"

#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
MSXMLRawOut::write(OutputDevice& of, const MSEdgeControl& ec,
                   SUMOTime timestep, int precision) {
    of.openTag("timestep") << " time=\"" << time2string(timestep) << "\"";
    of.setPrecision(precision);
    const MSEdgeVector& edges = ec.getEdges();
    for (MSEdgeVector::const_iterator e = edges.begin(); e != edges.end(); ++e) {
        writeEdge(of, **e, timestep);
    }
    of.setPrecision(gPrecision);
    of.closeTag();
}


void
MSXMLRawOut::writeEdge(OutputDevice& of, const MSEdge& edge, SUMOTime timestep) {
    //en
    bool dump = !MSGlobals::gOmitEmptyEdgesOnDump;
    if (!dump) {
        if (MSGlobals::gUseMesoSim) {
            MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(edge);
            while (seg != 0) {
                if (seg->getCarNumber() != 0) {
                    dump = true;
                    break;
                }
                seg = seg->getNextSegment();
            }
        } else {
            const std::vector<MSLane*>& lanes = edge.getLanes();
            for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
                if (((**lane).getVehicleNumber() != 0)) {
                    dump = true;
                    break;
                }
            }
        }
    }
    //en
    const std::vector<MSTransportable*>& persons = edge.getSortedPersons(timestep);
    const std::vector<MSTransportable*>& containers = edge.getSortedContainers(timestep);
    if (dump || persons.size() > 0 || containers.size() > 0) {
        of.openTag("edge") << " id=\"" << edge.getID() << "\"";
        if (dump) {
            if (MSGlobals::gUseMesoSim) {
                MESegment* seg = MSGlobals::gMesoNet->getSegmentForEdge(edge);
                while (seg != 0) {
                    seg->writeVehicles(of);
                    seg = seg->getNextSegment();
                }
            } else {
                const std::vector<MSLane*>& lanes = edge.getLanes();
                for (std::vector<MSLane*>::const_iterator lane = lanes.begin(); lane != lanes.end(); ++lane) {
                    writeLane(of, **lane);
                }
            }
        }
        // write persons
        for (std::vector<MSTransportable*>::const_iterator it_p = persons.begin(); it_p != persons.end(); ++it_p) {
            writeTransportable(of, *it_p, SUMO_TAG_PERSON);
        }
        // write containers
        for (std::vector<MSTransportable*>::const_iterator it_c = containers.begin(); it_c != containers.end(); ++it_c) {
            writeTransportable(of, *it_c, SUMO_TAG_CONTAINER);
        }
        of.closeTag();
    }
}


void
MSXMLRawOut::writeLane(OutputDevice& of, const MSLane& lane) {
    of.openTag("lane") << " id=\"" << lane.myID << "\"";
    if (lane.getVehicleNumber() != 0) {
        for (std::vector<MSVehicle*>::const_iterator veh = lane.myVehBuffer.begin();
                veh != lane.myVehBuffer.end(); ++veh) {
            writeVehicle(of, **veh);
        }
        for (MSLane::VehCont::const_iterator veh = lane.myVehicles.begin();
                veh != lane.myVehicles.end(); ++veh) {
            writeVehicle(of, **veh);
        }
    }
    of.closeTag();
}


void
MSXMLRawOut::writeVehicle(OutputDevice& of, const MSBaseVehicle& veh) {
    if (veh.isOnRoad()) {
        of.openTag("vehicle");
        of.writeAttr(SUMO_ATTR_ID, veh.getID());
        of.writeAttr(SUMO_ATTR_POSITION, veh.getPositionOnLane());
        of.writeAttr(SUMO_ATTR_SPEED, veh.getSpeed());
        if (!MSGlobals::gUseMesoSim) {
            const MSVehicle& microVeh = static_cast<const MSVehicle&>(veh);
            // microsim-specific stuff
            if (MSGlobals::gLateralResolution > 0) {
                const SUMOReal posLat = microVeh.getLateralPositionOnLane();
                of.writeAttr(SUMO_ATTR_POSITION_LAT, posLat);
            }
            const int personNumber = microVeh.getPersonNumber();
            if (personNumber > 0) {
                of.writeAttr(SUMO_ATTR_PERSON_NUMBER, personNumber);
            }
            const int containerNumber = microVeh.getContainerNumber();
            if (containerNumber > 0) {
                of.writeAttr(SUMO_ATTR_CONTAINER_NUMBER, containerNumber);
            }
            const std::vector<MSTransportable*>& persons = microVeh.getPersons();
            for (std::vector<MSTransportable*>::const_iterator it_p = persons.begin(); it_p != persons.end(); ++it_p) {
                writeTransportable(of, *it_p, SUMO_TAG_PERSON);
            }
            const std::vector<MSTransportable*>& containers = microVeh.getContainers();
            for (std::vector<MSTransportable*>::const_iterator it_c = containers.begin(); it_c != containers.end(); ++it_c) {
                writeTransportable(of, *it_c, SUMO_TAG_CONTAINER);
            }
        }
        of.closeTag();
    }
}


void
MSXMLRawOut::writeTransportable(OutputDevice& of, const MSTransportable* p, SumoXMLTag tag) {
    of.openTag(tag);
    of.writeAttr(SUMO_ATTR_ID, p->getID());
    of.writeAttr(SUMO_ATTR_POSITION, p->getEdgePos());
    of.writeAttr(SUMO_ATTR_ANGLE, GeomHelper::naviDegree(p->getAngle()));
    of.writeAttr("stage", p->getCurrentStageDescription());
    of.closeTag();
}

/****************************************************************************/
