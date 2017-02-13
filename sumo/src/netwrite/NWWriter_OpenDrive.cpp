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
// Copyright (C) 2011-2017 DLR (http://www.dlr.de/) and contributors
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
#include <utils/iodevices/OutputDevice_String.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/bezier.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StdDefs.h>
#include <utils/common/StringUtils.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

//#define DEBUG_SMOOTH_GEOM
#define DEBUGCOND true

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
    const NBNodeCont& nc = nb.getNodeCont();
    const NBEdgeCont& ec = nb.getEdgeCont();
    const bool origNames = oc.getBool("output.original-names");
    const SUMOReal straightThresh = DEG2RAD(oc.getFloat("opendrive-output.straight-threshold"));
    // some internal mapping containers
    int nodeID = 1;
    int edgeID = nc.size() * 10; // distinct from node ids
    StringBijection<int> edgeMap;
    StringBijection<int> nodeMap;
    //
    OutputDevice& device = OutputDevice::getDevice(oc.getString("opendrive-output"));
    device << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
    device.openTag("OpenDRIVE");
    time_t now = time(0);
    std::string dstr(ctime(&now));
    const Boundary& b = GeoConvHelper::getFinal().getConvBoundary();
    // write header
    device.openTag("header");
    device.writeAttr("revMajor", "1");
    device.writeAttr("revMinor", "4");
    device.writeAttr("name", "");
    device.writeAttr("version", "1.00");
    device.writeAttr("date", dstr.substr(0, dstr.length() - 1));
    device.writeAttr("north", b.ymax());
    device.writeAttr("south", b.ymin());
    device.writeAttr("east", b.xmax());
    device.writeAttr("west", b.xmin());
    /* @note obsolete in 1.4
    device.writeAttr("maxRoad", ec.size());
    device.writeAttr("maxJunc", nc.size());
    device.writeAttr("maxPrg", 0);
    */
    device.closeTag();

    // write normal edges (road)
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        const NBEdge* e = (*i).second;

        // buffer output because some fields are computed out of order
        OutputDevice_String elevationOSS(false, 3);
        elevationOSS.setPrecision(8);
        OutputDevice_String planViewOSS(false, 2);
        planViewOSS.setPrecision(8);
        SUMOReal length = 0;

        planViewOSS.openTag("planView");
        planViewOSS.setPrecision(8); // geometry hdg requires higher precision
        // for the shape we need to use the leftmost border of the leftmost lane
        const std::vector<NBEdge::Lane>& lanes = e->getLanes();
        PositionVector ls = getLeftLaneBorder(e);
#ifdef DEBUG_SMOOTH_GEOM
        if (DEBUGCOND) {
            std::cout << "write planview for edge " << e->getID() << "\n";
        }
#endif

        if (ls.size() == 2 || e->getPermissions() == SVC_PEDESTRIAN) {
            // foot paths may contain sharp angles
            length = writeGeomLines(ls, planViewOSS, elevationOSS);
        } else {
            bool ok = writeGeomSmooth(ls, e->getSpeed(), planViewOSS, elevationOSS, straightThresh, length);
            if (!ok) {
                WRITE_WARNING("Could not compute smooth shape for edge '" + e->getID() + "'.");
            }
        }
        planViewOSS.closeTag();

        device.openTag("road");
        device.writeAttr("name", StringUtils::escapeXML(e->getStreetName()));
        device.setPrecision(8); // length requires higher precision
        device.writeAttr("length", MAX2(POSITION_EPS, length));
        device.setPrecision(gPrecision);
        device.writeAttr("id", getID(e->getID(), edgeMap, edgeID));
        device.writeAttr("junction", -1);
        const bool hasSucc = e->getConnections().size() > 0;
        const bool hasPred = e->getIncomingEdges().size() > 0;
        if (hasPred || hasSucc) {
            device.openTag("link");
            if (hasPred) {
                device.openTag("predecessor");
                device.writeAttr("elementType", "junction");
                device.writeAttr("elementId", getID(e->getFromNode()->getID(), nodeMap, nodeID));
                device.closeTag();
            }
            if (hasSucc) {
                device.openTag("successor");
                device.writeAttr("elementType", "junction");
                device.writeAttr("elementId", getID(e->getToNode()->getID(), nodeMap, nodeID));
                device.closeTag();
            }
            device.closeTag();
        }
        device.openTag("type").writeAttr("s", 0).writeAttr("type", "town").closeTag();
        device << planViewOSS.getString();
        writeElevationProfile(ls, device, elevationOSS);
        device << "        <lateralProfile/>\n";
        device << "        <lanes>\n";
        device << "            <laneSection s=\"0\">\n";
        writeEmptyCenterLane(device, "solid", 0.13);
        device << "                <right>\n";
        for (int j = e->getNumLanes(); --j >= 0;) {
            device << "                    <lane id=\"-" << e->getNumLanes() - j << "\" type=\"" << getLaneType(e->getPermissions(j)) << "\" level=\"true\">\n";
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
            device << "        <userData code=\"sumoId\" value=\"" << e->getID() << "\"/>\n";
        }
        device.closeTag();
        checkLaneGeometries(e);
    }
    device.lf();

    // write junction-internal edges (road). In OpenDRIVE these are called 'paths' or 'connecting roads'
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
                const SUMOReal width = c.toEdge->getLaneWidth(c.toLane);
                const PositionVector begShape = getLeftLaneBorder(inEdge, c.fromLane);
                const PositionVector endShape = getLeftLaneBorder(outEdge, c.toLane);
                //std::cout << "computing reference line for internal lane " << c.getInternalLaneID() << " begLane=" << inEdge->getLaneShape(c.fromLane) << " endLane=" << outEdge->getLaneShape(c.toLane) << "\n";

                SUMOReal length;
                PositionVector fallBackShape;
                fallBackShape.push_back(begShape.back());
                fallBackShape.push_back(endShape.front());
                const bool turnaround = inEdge->isTurningDirectionAt(outEdge);
                bool ok = true;
                PositionVector init = NBNode::bezierControlPoints(begShape, endShape, turnaround, 25, 25, ok, 0, straightThresh);
                if (init.size() == 0) {
                    length = fallBackShape.length2D();
                    // problem with turnarounds is known, method currently returns 'ok' (#2539)
                    if (!ok) {
                        WRITE_WARNING("Could not compute smooth shape from lane '" + inEdge->getLaneID(c.fromLane) + "' to lane '" + outEdge->getLaneID(c.toLane) + "'. Use option 'junctions.scurve-stretch' or increase radius of junction '" + inEdge->getToNode()->getID() + "' to fix this.");
                    }
                } else {
                    length = bezier(init, 12).length2D();
                }

                device.openTag("road");
                device.writeAttr("name", c.getInternalLaneID());
                device.setPrecision(8); // length requires higher precision
                device.writeAttr("length", MAX2(POSITION_EPS, length));
                device.setPrecision(gPrecision);
                device.writeAttr("id", getID(c.getInternalLaneID(), edgeMap, edgeID));
                device.writeAttr("junction", getID(n->getID(), nodeMap, nodeID));
                device.openTag("link");
                device.openTag("predecessor");
                device.writeAttr("elementType", "road");
                device.writeAttr("elementId", getID(inEdge->getID(), edgeMap, edgeID));
                device.writeAttr("contactPoint", "end");
                device.closeTag();
                device.openTag("successor");
                device.writeAttr("elementType", "road");
                device.writeAttr("elementId", getID(outEdge->getID(), edgeMap, edgeID));
                device.writeAttr("contactPoint", "start");
                device.closeTag();
                device.closeTag();
                device.openTag("type").writeAttr("s", 0).writeAttr("type", "town").closeTag();
                device.openTag("planView");
                device.setPrecision(8); // geometry hdg requires higher precision
                OutputDevice_String elevationOSS(false, 3);
#ifdef DEBUG_SMOOTH_GEOM
                if (DEBUGCOND) {
                    std::cout << "write planview for internal edge " << c.getInternalLaneID() << " init=" << init << " fallback=" << fallBackShape << "\n";
                }
#endif
                if (init.size() == 0) {
                    writeGeomLines(fallBackShape, device, elevationOSS);
                } else {
                    writeGeomPP3(device, elevationOSS, init, length);
                }
                device.setPrecision(gPrecision);
                device.closeTag();
                writeElevationProfile(fallBackShape, device, elevationOSS);
                device << "        <lateralProfile/>\n";
                device << "        <lanes>\n";
                device << "            <laneSection s=\"0\">\n";
                writeEmptyCenterLane(device, "none", 0);
                device << "                <right>\n";
                device << "                    <lane id=\"-1\" type=\"" << getLaneType(outEdge->getPermissions(c.toLane)) << "\" level=\"true\">\n";
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
                device.closeTag();
            }
        }
    }

    // write junctions (junction)
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        const std::vector<NBEdge*>& incoming = n->getIncomingEdges();
        // check if any connections must be written
        int numConnections = 0;
        for (std::vector<NBEdge*>::const_iterator j = incoming.begin(); j != incoming.end(); ++j) {
            numConnections += (int)((*j)->getConnections().size());
        }
        if (numConnections == 0) {
            continue;
        }
        device << "    <junction name=\"" << n->getID() << "\" id=\"" << getID(n->getID(), nodeMap, nodeID) << "\">\n";
        int index = 0;
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


SUMOReal
NWWriter_OpenDrive::writeGeomLines(const PositionVector& shape, OutputDevice& device, OutputDevice& elevationDevice, SUMOReal offset) {
    for (int j = 0; j < (int)shape.size() - 1; ++j) {
        const Position& p = shape[j];
        const Position& p2 = shape[j + 1];
        const SUMOReal hdg = shape.angleAt2D(j);
        const SUMOReal length = p.distanceTo2D(p2);
        device.openTag("geometry");
        device.writeAttr("s", offset);
        device.writeAttr("x", p.x());
        device.writeAttr("y", p.y());
        device.writeAttr("hdg", hdg);
        device.writeAttr("length", length);
        device.openTag("line").closeTag();
        device.closeTag();
        elevationDevice << "            <elevation s=\"" << offset << "\" a=\"" << p.z() << "\" b=\"" << (p2.z() - p.z()) / MAX2(POSITION_EPS, length) << "\" c=\"0\" d=\"0\"/>\n";
        offset += length;
    }
    return offset;
}


void
NWWriter_OpenDrive::writeEmptyCenterLane(OutputDevice& device, const std::string& mark, SUMOReal markWidth) {
    device << "                <center>\n";
    device << "                    <lane id=\"0\" type=\"none\" level=\"true\">\n";
    device << "                        <link/>\n";
    device << "                        <roadMark sOffset=\"0\" type=\"" << mark << "\" weight=\"standard\" color=\"standard\" width=\"" << markWidth << "\"/>\n";
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
    /// it would be tempting to use
    // PositionVector result = edge->getLaneShape(laneIndex);
    // (and the moveo2side)
    // However, the lanes in SUMO have a small lateral gap (SUMO_const_laneOffset) to account for markings
    // In OpenDRIVE this gap does not exists so we have to do all lateral
    // computations based on the reference line
    // This assumes that the 'stop line' for all lanes is colinear!
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


SUMOReal
NWWriter_OpenDrive::writeGeomPP3(
    OutputDevice& device,
    OutputDevice& elevationDevice,
    PositionVector init,
    SUMOReal length,
    SUMOReal offset) {
    assert(init.size() == 3 || init.size() == 4);

    // avoid division by 0
    length = MAX2(POSITION_EPS, length);

    const Position p = init.front();
    const SUMOReal hdg = init.angleAt2D(0);

    // backup elevation values
    const PositionVector initZ = init;
    // translate to u,v coordinates
    init.add(-p.x(), -p.y(), -p.z());
    init.rotate2D(-hdg);

    // parametric coefficients
    SUMOReal aU, bU, cU, dU;
    SUMOReal aV, bV, cV, dV;
    SUMOReal aZ, bZ, cZ, dZ;

    // unfactor the Bernstein polynomials of degree 2 (or 3) and collect the coefficients
    if (init.size() == 3) {
        //f(x, a, b ,c) = a + (2*b - 2*a)*x + (a - 2*b + c)*x*x
        aU = init[0].x();
        bU = 2 * init[1].x() - 2 * init[0].x();
        cU = init[0].x() - 2 * init[1].x() + init[2].x();
        dU = 0;

        aV = init[0].y();
        bV = 2 * init[1].y() - 2 * init[0].y();
        cV = init[0].y() - 2 * init[1].y() + init[2].y();
        dV = 0;

        // elevation is not parameteric on [0:1] but on [0:length]
        aZ = initZ[0].z();
        bZ = (2 * initZ[1].z() - 2 * initZ[0].z()) / length;
        cZ = (initZ[0].z() - 2 * initZ[1].z() + initZ[2].z()) / (length * length);
        dZ = 0;

    } else {
        // f(x, a, b, c, d) = a + (x*((3*b) - (3*a))) + ((x*x)*((3*a) + (3*c) - (6*b))) + ((x*x*x)*((3*b) - (3*c) - a + d))
        aU = init[0].x();
        bU = 3 * init[1].x() - 3 * init[0].x();
        cU = 3 * init[0].x() - 6 * init[1].x() + 3 * init[2].x();
        dU = -init[0].x() + 3 * init[1].x() - 3 * init[2].x() + init[3].x();

        aV = init[0].y();
        bV = 3 * init[1].y() - 3 * init[0].y();
        cV = 3 * init[0].y() - 6 * init[1].y() + 3 * init[2].y();
        dV = -init[0].y() + 3 * init[1].y() - 3 * init[2].y() + init[3].y();

        // elevation is not parameteric on [0:1] but on [0:length]
        aZ = initZ[0].z();
        bZ = (3 * initZ[1].z() - 3 * initZ[0].z()) / length;
        cZ = (3 * initZ[0].z() - 6 * initZ[1].z() + 3 * initZ[2].z()) / (length * length);
        dZ = (-initZ[0].z() + 3 * initZ[1].z() - 3 * initZ[2].z() + initZ[3].z()) / (length * length * length);
    }

    device.openTag("geometry");
    device.writeAttr("s", offset);
    device.writeAttr("x", p.x());
    device.writeAttr("y", p.y());
    device.writeAttr("hdg", hdg);
    device.writeAttr("length", length);

    device.openTag("paramPoly3");
    device.writeAttr("aU", aU);
    device.writeAttr("bU", bU);
    device.writeAttr("cU", cU);
    device.writeAttr("dU", dU);
    device.writeAttr("aV", aV);
    device.writeAttr("bV", bV);
    device.writeAttr("cV", cV);
    device.writeAttr("dV", dV);
    device.closeTag();
    device.closeTag();

    // write elevation
    elevationDevice.openTag("elevation");
    elevationDevice.writeAttr("s", offset);
    elevationDevice.writeAttr("a", aZ);
    elevationDevice.writeAttr("b", bZ);
    elevationDevice.writeAttr("c", cZ);
    elevationDevice.writeAttr("d", dZ);
    elevationDevice.closeTag();

    return offset + length;
}


bool
NWWriter_OpenDrive::writeGeomSmooth(const PositionVector& shape, SUMOReal speed, OutputDevice& device, OutputDevice& elevationDevice, SUMOReal straightThresh, SUMOReal& length) {
#ifdef DEBUG_SMOOTH_GEOM
    if (DEBUGCOND) {
        std::cout << "writeGeomSmooth\n  n=" << shape.size() << " shape=" << toString(shape) << "\n";
    }
#endif
    bool ok = true;
    const SUMOReal longThresh = speed; //  16.0; // make user-configurable (should match the sampling rate of the source data)
    const SUMOReal curveCutout = longThresh / 2; // 8.0; // make user-configurable (related to the maximum turning rate)
    // the length of the segment that is added for cutting a corner can be bounded by 2*curveCutout (prevent the segment to be classified as 'long')
    assert(longThresh >= 2 * curveCutout);
    assert(shape.size() > 2);
    // add intermediate points wherever there is a strong angular change between long segments
    // assume the geometry is simplified so as not to contain consecutive colinear points
    PositionVector shape2 = shape;
    SUMOReal maxAngleDiff = 0;
    SUMOReal offset = 0;
    for (int j = 1; j < (int)shape.size() - 1; ++j) {
        //const SUMOReal hdg = shape.angleAt2D(j);
        const Position& p0 = shape[j - 1];
        const Position& p1 = shape[j];
        const Position& p2 = shape[j + 1];
        const SUMOReal dAngle = fabs(GeomHelper::angleDiff(p0.angleTo2D(p1), p1.angleTo2D(p2)));
        const SUMOReal length1 = p0.distanceTo2D(p1);
        const SUMOReal length2 = p1.distanceTo2D(p2);
        maxAngleDiff = MAX2(maxAngleDiff, dAngle);
#ifdef DEBUG_SMOOTH_GEOM
        if (DEBUGCOND) {
            std::cout << "   j=" << j << " dAngle=" << RAD2DEG(dAngle) << " length1=" << length1 << " length2=" << length2 << "\n";
        }
#endif
        if (dAngle > straightThresh
                && (length1 > longThresh || j == 1)
                && (length2 > longThresh || j == (int)shape.size() - 2)) {
            shape2.insertAtClosest(shape.positionAtOffset2D(offset + length1 - MIN2(length1 - POSITION_EPS, curveCutout)));
            shape2.insertAtClosest(shape.positionAtOffset2D(offset + length1 + MIN2(length2 - POSITION_EPS, curveCutout)));
            shape2.removeClosest(p1);
        }
        offset += length1;
    }
    const int numPoints = (int)shape2.size();
#ifdef DEBUG_SMOOTH_GEOM
    if (DEBUGCOND) {
        std::cout << " n=" << numPoints << " shape2=" << toString(shape2) << "\n";
    }
#endif

    if (maxAngleDiff < straightThresh) {
        length = writeGeomLines(shape2, device, elevationDevice, 0);
#ifdef DEBUG_SMOOTH_GEOM
        if (DEBUGCOND) {
            std::cout << "   special case: all lines. maxAngleDiff=" << maxAngleDiff << "\n";
        }
#endif
        return ok;
    }

    // write the long segments as lines, short segments as curves
    offset = 0;
    for (int j = 0; j < numPoints - 1; ++j) {
        const Position& p0 = shape2[j];
        const Position& p1 = shape2[j + 1];
        PositionVector line;
        line.push_back(p0);
        line.push_back(p1);
        const SUMOReal lineLength = line.length2D();
        if (lineLength >= longThresh) {
            offset = writeGeomLines(line, device, elevationDevice, offset);
#ifdef DEBUG_SMOOTH_GEOM
            if (DEBUGCOND) {
                std::cout << "      writeLine=" << toString(line) << "\n";
            }
#endif
        } else {
            // find control points
            PositionVector begShape;
            PositionVector endShape;
            if (j == 0 || j == numPoints - 2) {
                // keep the angle of the first/last segment but end at the front of the shape
                begShape = line;
                begShape.add(p0 - begShape.back());
            } else if (j == 1 || p0.distanceTo2D(shape2[j - 1]) > longThresh) {
                // use the previous segment if it is long or the first one
                begShape.push_back(shape2[j - 1]);
                begShape.push_back(p0);
            } else {
                // end at p0 with mean angle of the previous and current segment
                begShape.push_back(shape2[j - 1]);
                begShape.push_back(p1);
                begShape.add(p0 - begShape.back());
            }

            if (j == 0 || j == numPoints - 2) {
                // keep the angle of the first/last segment but start at the end of the shape
                endShape = line;
                endShape.add(p1 - endShape.front());
            } else if (j == numPoints - 3 || p1.distanceTo2D(shape2[j + 2]) > longThresh) {
                // use the next segment if it is long or the final one
                endShape.push_back(p1);
                endShape.push_back(shape2[j + 2]);
            } else {
                // start at p1 with mean angle of the current and next segment
                endShape.push_back(p0);
                endShape.push_back(shape2[j + 2]);
                endShape.add(p1 - endShape.front());
            }
            const SUMOReal extrapolateLength = MIN2((SUMOReal)25, lineLength / 4);
            PositionVector init = NBNode::bezierControlPoints(begShape, endShape, false, extrapolateLength, extrapolateLength, ok, 0, straightThresh);
            if (init.size() == 0) {
                // could not compute control points, write line
                offset = writeGeomLines(line, device, elevationDevice, offset);
#ifdef DEBUG_SMOOTH_GEOM
                if (DEBUGCOND) {
                    std::cout << "      writeLine lineLength=" << lineLength << " begShape" << j << "=" << toString(begShape) << " endShape" << j << "=" << toString(endShape) << " init" << j << "=" << toString(init) << "\n";
                }
#endif
            } else {
                // write bezier
                const SUMOReal curveLength = bezier(init, 12).length2D();
                offset = writeGeomPP3(device, elevationDevice, init, curveLength, offset);
#ifdef DEBUG_SMOOTH_GEOM
                if (DEBUGCOND) {
                    std::cout << "      writeCurve lineLength=" << lineLength << " curveLength=" << curveLength << " begShape" << j << "=" << toString(begShape) << " endShape" << j << "=" << toString(endShape) << " init" << j << "=" << toString(init) << "\n";
                }
#endif
            }
        }
    }
    length = offset;
    return ok;
}


void
NWWriter_OpenDrive::writeElevationProfile(const PositionVector& shape, OutputDevice& device, const OutputDevice_String& elevationDevice) {
    // check if the shape is flat
    bool flat = true;
    SUMOReal z = shape.size() == 0 ? 0 : shape[0].z();
    for (int i = 1; i < (int)shape.size(); ++i) {
        if (fabs(shape[i].z() - z) > NUMERICAL_EPS) {
            flat = false;
            break;
        }
    }
    device << "        <elevationProfile>\n";
    if (flat) {
        device << "            <elevation s=\"0\" a=\"" << z << "\" b=\"0\" c=\"0\" d=\"0\"/>\n";
    } else {
        device << elevationDevice.getString();
    }
    device << "        </elevationProfile>\n";

}


void
NWWriter_OpenDrive::checkLaneGeometries(const NBEdge* e) {
    if (e->getNumLanes() > 1) {
        // compute 'stop line' of rightmost lane
        const PositionVector shape0 = e->getLaneShape(0);
        assert(shape0.size() >= 2);
        const Position& from = shape0[-2];
        const Position& to = shape0[-1];
        PositionVector stopLine;
        stopLine.push_back(to);
        stopLine.push_back(to - PositionVector::sideOffset(from, to, -1000.0));
        // endpoints of all other lanes should be on the stop line
        for (int lane = 1; lane < e->getNumLanes(); ++lane) {
            const SUMOReal dist = stopLine.distance2D(e->getLaneShape(lane)[-1]);
            if (dist > NUMERICAL_EPS) {
                WRITE_WARNING("Uneven stop line at lane '" + e->getLaneID(lane) + "' (dist=" + toString(dist) + ") cannot be represented in OpenDRIVE.");
            }
        }
    }
}

/****************************************************************************/

