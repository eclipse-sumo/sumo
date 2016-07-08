/****************************************************************************/
/// @file    NWWriter_OpenDrive.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the openDRIVE format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2011-2016 DLR (http://www.dlr.de/) and contributors
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

#define MIN_TURN_DIAMETER 2.0


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
    const bool origNames = oc.getBool("output.original-names");
    // some internal mapping containers
    int edgeID = 1;
    int nodeID = 1;
    StringBijection<int> edgeMap;
    StringBijection<int> nodeMap;
    //
    OutputDevice& device = OutputDevice::getDevice(oc.getString("opendrive-output"));
    device << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    device.openTag("OpenDRIVE");
    time_t now = time(0);
    std::string dstr(ctime(&now));
    const NBNodeCont& nc = nb.getNodeCont();
    const NBEdgeCont& ec = nb.getEdgeCont();
    const Boundary& b = GeoConvHelper::getFinal().getConvBoundary();
    // write header
    device.openTag("header");
    device.writeAttr("revMajor", "1");
    device.writeAttr("revMinor", "3");
    device.writeAttr("name", "");
    device.writeAttr("version", "1.00");
    device.writeAttr("date", dstr.substr(0, dstr.length() - 1));
    device.writeAttr("north", b.ymax());
    device.writeAttr("south", b.ymin());
    device.writeAttr("east", b.xmax());
    device.writeAttr("west", b.xmin());
    device.writeAttr("maxRoad", ec.size());
    device.writeAttr("maxJunc", nc.size());
    device.writeAttr("maxPrg", 0);
    device.closeTag();

    // write normal edges (road)
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        const NBEdge* e = (*i).second;
        device << "    <road name=\"" << StringUtils::escapeXML(e->getStreetName()) << "\" length=\"" << e->getLength() << "\" id=\"" << getID(e->getID(), edgeMap, edgeID) << "\" junction=\"-1\">\n";
        device << "        <link>\n";
        device << "            <predecessor elementType=\"junction\" elementId=\"" << getID(e->getFromNode()->getID(), nodeMap, nodeID) << "\"/>\n";
        device << "            <successor elementType=\"junction\" elementId=\"" << getID(e->getToNode()->getID(), nodeMap, nodeID) << "\"/>\n";
        device << "        </link>\n";
        device << "        <type s=\"0\" type=\"town\"/>\n";
        // for the shape we need to use the leftmost border of the leftmost lane
        const std::vector<NBEdge::Lane>& lanes = e->getLanes();
        PositionVector ls = getLeftLaneBorder(e);
        writePlanView(ls, device);
        device << "        <elevationProfile><elevation s=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/></elevationProfile>\n";
        device << "        <lateralProfile/>\n";
        device << "        <lanes>\n";
        device << "            <laneSection s=\"0\">\n";
        writeEmptyCenterLane(device, "solid", 0.13);
        device << "                <right>\n";
        for (int j = e->getNumLanes(); --j >= 0;) {
            device << "                    <lane id=\"-" << e->getNumLanes() - j << "\" type=\"" << getLaneType(e->getPermissions(j)) << "\" level=\"0\">\n";
            device << "                        <link/>\n";
            // this could be used for geometry-link junctions without u-turn,
            // predecessor and sucessors would be lane indices,
            // road predecessor / succesfors would be of type 'road' rather than
            // 'junction'
            //device << "                            <predecessor id=\"-1\"/>\n";
            //device << "                            <successor id=\"-1\"/>\n";
            //device << "                        </link>\n";
            device << "                        <width sOffset=\"0\" a=\"" << e->getLaneWidth(j) << "\" b=\"0\" c=\"0\" d=\"0\"/>\n";
            std::string markType = "broken";
            if (j == 0) {
                markType = "solid";
            }
            device << "                        <roadMark sOffset=\"0\" type=\"" << markType << "\" weight=\"standard\" color=\"standard\" width=\"0.13\"/>\n";
            device << "                        <speed sOffset=\"0\" max=\"" << lanes[j].speed << "\"/>\n";
            device << "                    </lane>\n";
        }
        device << "                 </right>\n";
        device << "            </laneSection>\n";
        device << "        </lanes>\n";
        device << "        <objects/>\n";
        device << "        <signals/>\n";
        if (origNames) {
            device << "        <userData sumoId=\"" << e->getID() << "\"/>\n";
        }
        device << "    </road>\n";
    }
    device << "\n";
    // write junction-internal edges (road)
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        const std::vector<NBEdge*>& incoming = (*i).second->getIncomingEdges();
        for (std::vector<NBEdge*>::const_iterator j = incoming.begin(); j != incoming.end(); ++j) {
            const NBEdge* inEdge = *j;
            const std::vector<NBEdge::Connection>& elv = inEdge->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                const NBEdge::Connection& c = *k;
                const NBEdge* outEdge = c.toEdge;
                if (outEdge == 0) {
                    continue;
                }
                PositionVector shape = c.shape;
                if (c.haveVia) {
                    shape.append(c.viaShape);
                }
                const SUMOReal width = c.toEdge->getLaneWidth(c.toLane);
                // @todo: this if-clause is a hack which assures that the code also works with connections of zero length, what may be possible
                // probably, it would make sense to mark such connections and connect the incoming/outgoing streets directly in such cases.
                if (shape.length() > POSITION_EPS) {
                    try {
                        shape.move2side(-width / 2.);
                    } catch (InvalidArgument&) {
                        shape.clear();
                    }
                } else {
                    shape.clear();
                }
                if (inEdge->isTurningDirectionAt(outEdge)
                        && getLeftLaneBorder(inEdge).back().distanceTo2D(getLeftLaneBorder(outEdge).front()) < MIN_TURN_DIAMETER) {
                    shape.clear(); // simplified geometry for sharp turn-arounds
                }
                // we need to fix start and endpoints in case the start and
                // end segments were not in line with the incoming and outgoing lanes
                if (shape.size() > 1) {
                    shape[0] = getLeftLaneBorder(inEdge, c.fromLane).back();
                    shape[-1] = getLeftLaneBorder(outEdge, c.toLane).front();
                } else {
                    shape.clear();
                    shape.push_back(getLeftLaneBorder(inEdge, c.fromLane).back());
                    shape.push_back(getLeftLaneBorder(outEdge, c.toLane).front());
                }

                device << "    <road name=\"" << c.getInternalLaneID() << "\" length=\"" << shape.length() << "\" id=\"" << getID(c.getInternalLaneID(), edgeMap, edgeID) << "\" junction=\"" << getID(n->getID(), nodeMap, nodeID) << "\">\n";
                device << "        <link>\n";
                device << "            <predecessor elementType=\"road\" elementId=\"" << getID(inEdge->getID(), edgeMap, edgeID) << "\" contactPoint=\"end\"/>\n";
                device << "            <successor elementType=\"road\" elementId=\"" << getID(outEdge->getID(), edgeMap, edgeID) << "\" contactPoint=\"start\"/>\n";
                device << "        </link>\n";
                device << "        <type s=\"0\" type=\"town\"/>\n";
                writePlanView(shape, device);
                device << "        <elevationProfile><elevation s=\"0\" a=\"0\" b=\"0\" c=\"0\" d=\"0\"/></elevationProfile>\n";
                device << "        <lateralProfile/>\n";
                device << "        <lanes>\n";
                device << "            <laneSection s=\"0\">\n";
                writeEmptyCenterLane(device, "none", 0);
                device << "                <right>\n";
                device << "                    <lane id=\"-1\" type=\"" << getLaneType(outEdge->getPermissions(c.toLane)) << "\" level=\"0\">\n";
                device << "                        <link>\n";
                device << "                            <predecessor id=\"-" << inEdge->getNumLanes() - c.fromLane << "\"/>\n";
                device << "                            <successor id=\"-" << outEdge->getNumLanes() - c.toLane << "\"/>\n";
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
            const NBEdge* inEdge = *j;
            const std::vector<NBEdge::Connection>& elv = inEdge->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                const NBEdge::Connection& c = *k;
                const NBEdge* outEdge = c.toEdge;
                if (outEdge == 0) {
                    continue;
                }
                device << "    <connection id=\""
                       << index << "\" incomingRoad=\"" << getID(inEdge->getID(), edgeMap, edgeID)
                       << "\" connectingRoad=\""
                       << getID(c.getInternalLaneID(), edgeMap, edgeID)
                       << "\" contactPoint=\"start\">\n";
                device << "        <laneLink from=\"-" << inEdge->getNumLanes() - c.fromLane
                       << "\" to=\"-1"  // every connection has its own edge
                       << "\"/>\n";
                device << "    </connection>\n";
                ++index;
            }
        }
        device << "    </junction>\n";
    }

    device.closeTag();
    device.close();
}


void
NWWriter_OpenDrive::writePlanView(const PositionVector& shape, OutputDevice& device) {
    device << "        <planView>\n";
    SUMOReal offset = 0;
    for (unsigned int j = 0; j < shape.size() - 1; ++j) {
        const Position& p = shape[j];
        const SUMOReal hdg = shape.angleAt2D(j);
        const SUMOReal length = p.distanceTo(shape[j + 1]);
        device << std::setprecision(8); // hdg requires higher precision
        device << "            <geometry s=\"" << offset << "\" x=\"" << p.x() << "\" y=\"" << p.y() << "\" hdg=\"" << hdg << "\" length=\"" << length << "\"><line/></geometry>\n";
        device << std::setprecision(OUTPUT_ACCURACY);
        offset += length;
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


std::string
NWWriter_OpenDrive::getLaneType(SVCPermissions permissions) {
    switch (permissions) {
        case SVC_PEDESTRIAN:
            return "sidewalk";
        //case (SVC_BICYCLE | SVC_PEDESTRIAN):
        //    WRITE_WARNING("Ambiguous lane type (biking+driving) for road '" + roadID + "'");
        //    return "sidewalk";
        case SVC_BICYCLE:
            return "biking";
        case 0:
            // ambiguous
            return "none";
        case SVC_RAIL:
        case SVC_RAIL_URBAN:
        case SVC_RAIL_ELECTRIC:
            return "rail";
        case SVC_TRAM:
            return "tram";
        default: {
            // complex permissions
            if (permissions == SVCAll) {
                return "driving";
            } else if (isRailway(permissions)) {
                return "rail";
            } else if ((permissions & SVC_PASSENGER) != 0) {
                return "driving";
            } else {
                return "restricted";
            }
        }
    }
}


PositionVector
NWWriter_OpenDrive::getLeftLaneBorder(const NBEdge* edge, int laneIndex) {
    if (laneIndex == -1) {
        // leftmost lane
        laneIndex = (int)edge->getNumLanes() - 1;
    }
    const int leftmost = (int)edge->getNumLanes() - 1;
    SUMOReal widthOffset = -(edge->getLaneWidth(leftmost) / 2);

    // collect lane widths from left border of edge to left border of lane to connect to
    for (int i = leftmost; i > laneIndex; i--) {
        widthOffset += edge->getLaneWidth(i);
    }

    PositionVector result = edge->getLaneShape(leftmost);
    try {
        result.move2side(widthOffset);
    } catch (InvalidArgument&) { }
    return result;
}


/****************************************************************************/

