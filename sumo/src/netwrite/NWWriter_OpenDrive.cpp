/****************************************************************************/
/// @file    NWWriter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the openDRIVE format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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

#include <ctime>
#include "NWWriter_OpenDrive.h"
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>

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
NWWriter_OpenDrive::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether an opendrive-file shall be generated
    if (!oc.isSet("opendrive-output")) {
        return;
    }
    // some internal mapping containers
    int edgeID = 0;
    int nodeID = 0;
    StringBijection<int> edgeMap;
    StringBijection<int> nodeMap;
    //
    OutputDevice& device = OutputDevice::getDevice(oc.getString("opendrive-output"));
    device << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    device << "<OpenDRIVE>\n";
    time_t now = time(0);
    std::string dstr(ctime(&now));
    const NBNodeCont& nc = nb.getNodeCont();
    const NBEdgeCont& ec = nb.getEdgeCont();
    const Boundary& b = GeoConvHelper::getFinal().getConvBoundary();
    device << "    <header revMajor=\"1\" revMinor=\"3\" name=\"\" version=\"1.00\" date=\"" << dstr.substr(0, dstr.length() - 1)
           << "\" north=\"" << b.ymax() << "\" south=\"" << b.ymin() << "\" east=\"" << b.xmax() << "\" west=\"" << b.xmin()
           << "\" maxRoad=\"" << ec.size() << "\" maxJunc=\"" << nc.size() << "\" maxPrg=\"0\"/>\n";
    // write normal edges (road)
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        const NBEdge* e = (*i).second;
        device << "    <road name=\"" << StringUtils::escapeXML(e->getStreetName()) << "\" length=\"" << e->getLength() << "\" id=\"" << getID(e->getID(), edgeMap, edgeID) << "\" junction=\"-1\">\n";
        device << "        <link>\n";
        device << "            <predecessor elementType=\"junction\" elementId=\"" << getID(e->getFromNode()->getID(), nodeMap, nodeID) << "\"/>\n";
        device << "            <successor elementType=\"junction\" elementId=\"" << getID(e->getToNode()->getID(), nodeMap, nodeID) << "\"/>\n";
        device << "        </link>\n";
        device << "        <type s=\"0\" type=\"town\"/>\n";
        const std::vector<NBEdge::Lane>& lanes = e->getLanes();
        unsigned int li = (unsigned int)lanes.size() - 1;
        PositionVector ls = e->getLaneShape(li);
        try {
            ls.move2side(-e->getLaneWidth(li) / 2.);
        } catch (InvalidArgument&) {
            // we do not write anything, as this should have been reported, already
        }
        writePlanView(ls, device);
        device << "        <elevationProfile><elevation s=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/></elevationProfile>\n";
        device << "        <lateralProfile/>\n";
        device << "        <lanes>\n";
        device << "            <laneSection s=\"0\">\n";
        writeEmptyCenterLane(device, "solid", 0.13);
        device << "                <right>\n";
        for (int j = e->getNumLanes(); --j >= 0;) {
            device << "                    <lane id=\"-" << e->getNumLanes() - j << "\" type=\"driving\" level=\"0\">\n";
            device << "                        <link>\n";
            device << "                            <predecessor id=\"-1\"/>\n"; // internal roads have this
            device << "                            <successor id=\"-1\"/>\n"; // internal roads have this
            device << "                        </link>\n";
            device << "                        <width sOffset=\"0\" a=\"" << e->getLaneWidth(j) << "\" b=\"0\" c=\"0\" d=\"0\"/>\n";
            std::string markType = "broken";
            if (j == 0) {
                markType = "solid";
            }
            device << "                        <roadMark sOffset=\"0\" type=\"" << markType << "\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
            device << "                    </lane>\n";
        }
        device << "                 </right>\n";
        device << "            </laneSection>\n";
        device << "        </lanes>\n";
        device << "        <objects/>\n";
        device << "        <signals/>\n";
        device << "    </road>\n";
    }
    device << "\n";
    // write junction-internal edges (road)
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        const std::vector<NBEdge*>& incoming = (*i).second->getIncomingEdges();
        for (std::vector<NBEdge*>::const_iterator j = incoming.begin(); j != incoming.end(); ++j) {
            const std::vector<NBEdge::Connection>& elv = (*j)->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                if ((*k).toEdge == 0) {
                    continue;
                }
                const NBEdge::Connection& c = *k;
                PositionVector shape = c.shape;
                if (c.haveVia) {
                    shape.append(c.viaShape);
                }
                const SUMOReal width = SUMO_const_laneWidth;
                // @todo: this if-clause is a hack which assures that the code also works with connections of zero length, what may be possible
                // probably, it would make sense to mark such connections and connect the incoming/outgoing streets directly in such cases.
                try {
                    shape.move2side(-width / 2.);
                } catch (InvalidArgument&) {
                    // we do not write anything, maybe we should
                }
                device << "    <road name=\"" << c.id << "\" length=\"" << shape.length() << "\" id=\"" << getID(c.id, edgeMap, edgeID) << "\" junction=\"" << getID(n->getID(), nodeMap, nodeID) << "\">\n";
                device << "        <link>\n";
                device << "            <predecessor elementType=\"road\" elementId=\"" << getID((*j)->getID(), edgeMap, edgeID) << "\"/>\n";
                device << "            <successor elementType=\"road\" elementId=\"" << getID((*k).toEdge->getID(), edgeMap, edgeID) << "\"/>\n";
                device << "        </link>\n";
                device << "        <type s=\"0\" type=\"town\"/>\n";
                writePlanView(shape, device);
                device << "        <elevationProfile><elevation s=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/></elevationProfile>\n";
                device << "        <lateralProfile/>\n";
                device << "        <lanes>\n";
                device << "            <laneSection s=\"0\">\n";
                writeEmptyCenterLane(device, "none", 0);
                device << "                <right>\n";
                device << "                    <lane id=\"-1\" type=\"driving\" level=\"0\">\n";
                device << "                        <link>\n";
                device << "                            <predecessor id=\"-" << (*j)->getNumLanes() - c.fromLane << "\"/>\n";
                device << "                            <successor id=\"-" << c.toEdge->getNumLanes() - c.toLane << "\"/>\n";
                device << "                        </link>\n";
                device << "                        <width sOffset=\"0\" a=\"" << width << "\" b=\"0\" c=\"0\" d=\"0\"/>\n";
                device << "                        <roadMark sOffset=\"0\" type=\"none\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
                device << "                    </lane>\n";
                device << "                 </right>\n";
                device << "            </laneSection>\n";
                device << "        </lanes>\n";
                device << "        <objects/>\n";
                device << "        <signals/>\n";
                device << "    </road>\n";
            }
        }
    }

    // write junctions (junction)
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        device << "    <junction name=\"" << n->getID() << "\" id=\"" << getID(n->getID(), nodeMap, nodeID) << "\">\n";
        unsigned int index = 0;
        const std::vector<NBEdge*>& incoming = n->getIncomingEdges();
        for (std::vector<NBEdge*>::const_iterator j = incoming.begin(); j != incoming.end(); ++j) {
            const std::vector<NBEdge::Connection>& elv = (*j)->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                if ((*k).toEdge == 0) {
                    continue;
                }
                device << "    <connection id=\"" << index << "\" incomingRoad=\"" << getID((*j)->getID(), edgeMap, edgeID)
                       << "\" connectingRoad=\"" << getID((*k).id, edgeMap, edgeID) << "\" contactPoint=\"start\"/>\n";
                ++index;
            }
        }
        device << "    </junction>\n";
    }

    device << "</OpenDRIVE>\n";
    device.close();
}


void
NWWriter_OpenDrive::writePlanView(const PositionVector& shape, OutputDevice& device) {
    device << "        <planView>\n";
    SUMOReal offset = 0;
    for (unsigned int j = 0; j < shape.size() - 1; ++j) {
        const Position& p = shape[j];
        Line l = shape.lineAt(j);
        SUMOReal hdg = atan2(l.p2().y() - l.p1().y(), l.p2().x() - l.p1().x());
        device << "            <geometry s=\"" << offset << "\" x=\"" << p.x() << "\" y=\"" << p.y() << "\" hdg=\"" << hdg << "\" length=\"" << l.length() << "\"><line/></geometry>\n";
        offset += l.length();
    }
    device << "        </planView>\n";
}


void
NWWriter_OpenDrive::writeEmptyCenterLane(OutputDevice& device, const std::string& mark, SUMOReal markWidth) {
    device << "                <center>\n";
    device << "                    <lane id=\"0\" type=\"none\" level= \"0\">\n";
    device << "                        <link/>\n";
    device << "                        <roadMark sOffset=\"0\" type=\"" << mark << "\" weight=\"standard\" color=\"standard\" width=\"" << markWidth << "\"/>\n";
    device << "                        <width sOffset=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/>\n";
    device << "                    </lane>\n";
    device << "                </center>\n";
}


int
NWWriter_OpenDrive::getID(const std::string& origID, StringBijection<int>& map, int& lastID) {
    if (map.hasString(origID)) {
        return map.get(origID);
    }
    map.insert(origID, lastID++);
    return lastID - 1;
}


/****************************************************************************/

