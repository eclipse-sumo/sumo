/****************************************************************************/
/// @file    NWWriter_Amitran.cpp
/// @author  Michael Behrisch
/// @date    13.03.2014
/// @version $Id$
///
// Exporter writing networks using the Amitran format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2014-2014 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include "NWWriter_DlrNavteq.h"
#include "NWWriter_Amitran.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS



// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods
// ---------------------------------------------------------------------------
void
NWWriter_Amitran::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether an amitran-file shall be generated
    if (!oc.isSet("amitran-output")) {
        return;
    }
    OutputDevice& device = OutputDevice::getDevice(oc.getString("amitran-output"));
    device << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    device << "<network xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/amitran/network.xsd\">\n";
    // write nodes
    int index = 0;
    NBNodeCont& nc = nb.getNodeCont();
    std::set<NBNode*> singleRoundaboutNodes;
    std::set<NBNode*> multiRoundaboutNodes;
    const std::vector<EdgeVector>& roundabouts = nb.getRoundabouts();
    for (std::vector<EdgeVector>::const_iterator i = roundabouts.begin(); i != roundabouts.end(); ++i) {
        for (EdgeVector::const_iterator j = (*i).begin(); j != (*i).end(); ++j) {
            if ((*j)->getNumLanes() > 1) {
                multiRoundaboutNodes.insert((*j)->getFromNode());
            } else {
                singleRoundaboutNodes.insert((*j)->getFromNode());
            }
        }
    }
    std::map<NBNode*, int> nodeIds;
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        device << "    <node id=\"" << index;
        nodeIds[i->second] = index++;
        if (singleRoundaboutNodes.count(i->second) > 0) {
            device << "\" type=\"roundaboutSingle\"/>\n";
            continue;
        }
        if (multiRoundaboutNodes.count(i->second) > 0) {
            device << "\" type=\"roundaboutMulti\"/>\n";
            continue;
        }
        switch (i->second->getType()) {
            case NODETYPE_TRAFFIC_LIGHT:
            case NODETYPE_TRAFFIC_LIGHT_NOJUNCTION:
                device << "\" type=\"trafficLight";
                break;
            case NODETYPE_PRIORITY:
                device << "\" type=\"priority";
                break;
            case NODETYPE_PRIORITY_STOP:
                device << "\" type=\"priorityStop";
                break;
            case NODETYPE_RIGHT_BEFORE_LEFT:
                device << "\" type=\"rightBeforeLeft";
                break;
            case NODETYPE_ALLWAY_STOP:
                device << "\" type=\"allwayStop";
                break;
            case NODETYPE_DEAD_END:
            case NODETYPE_DEAD_END_DEPRECATED:
                device << "\" type=\"deadEnd";
                break;
            case NODETYPE_DISTRICT:
            case NODETYPE_NOJUNCTION:
            case NODETYPE_INTERNAL:
            case NODETYPE_UNKNOWN:
                break;
        }
        device << "\"/>\n";
    }
    // write edges
    index = 0;
    NBEdgeCont& ec = nb.getEdgeCont();
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        device << "    <link id=\"" << index++
               << "\" from=\"" << nodeIds[i->second->getFromNode()]
               << "\" to=\"" << nodeIds[i->second->getToNode()]
               << "\" roadClass=\"" << NWWriter_DlrNavteq::getRoadClass((*i).second)
               << "\" length=\"" << int(1000 * i->second->getLoadedLength())
               << "\" speedLimitKmh=\"" << int(3.6 * (*i).second->getSpeed() + 0.5)
               << "\" laneNr=\"" << (*i).second->getNumLanes()
               << "\"/>\n";
    }
    device << "</network>\n";
    device.close();
}


/****************************************************************************/

