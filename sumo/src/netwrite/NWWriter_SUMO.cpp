/****************************************************************************/
/// @file    NWWriter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the SUMO format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
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
#include <cmath>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBTrafficLightLogic.h>
#include <netbuild/NBDistrict.h>
#include "NWFrame.h"
#include "NWWriter_SUMO.h"

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
NWWriter_SUMO::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether a matsim-file shall be generated
    if (!oc.isSet("output-file")) {
        return;
    }
    OutputDevice& device = OutputDevice::getDevice(oc.getString("output-file"));
    device.writeXMLHeader("net", SUMOSAXAttributes::ENCODING, NWFrame::MAJOR_VERSION + " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo.sf.net/xsd/net_file.xsd\""); // street names may contain non-ascii chars
    device << "\n";
    // get involved container
    const NBNodeCont& nc = nb.getNodeCont();
    const NBEdgeCont& ec = nb.getEdgeCont();
    const NBDistrictCont& dc = nb.getDistrictCont();

    // write network offsets and projection
    writeLocation(device);

    // write inner lanes
    if (!oc.getBool("no-internal-links")) {
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            hadAny |= writeInternalEdges(device, *(*i).second);
        }
        if (hadAny) {
            device << "\n";
        }
    }

    // write edges with lanes and connected edges
    bool noNames = !oc.getBool("output.street-names");
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        writeEdge(device, *(*i).second, noNames);
    }
    device << "\n";

    // write tls logics
    writeTrafficLights(device, nb.getTLLogicCont());

    // write the nodes (junctions)
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        writeJunction(device, *(*i).second);
    }
    device << "\n";
    const bool includeInternal = !oc.getBool("no-internal-links");
    if (includeInternal) {
        // ... internal nodes if not unwanted
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            hadAny |= writeInternalNodes(device, *(*i).second);
        }
        if (hadAny) {
            device << "\n";
        }
    }

    // write the successors of lanes
    unsigned int numConnections = 0;
    for (std::map<std::string, NBEdge*>::const_iterator it_edge = ec.begin(); it_edge != ec.end(); it_edge++) {
        NBEdge* from = it_edge->second;
        from->sortOutgoingConnectionsByIndex();
        const std::vector<NBEdge::Connection> connections = from->getConnections();
        numConnections += (unsigned int)connections.size();
        for (std::vector<NBEdge::Connection>::const_iterator it_c = connections.begin(); it_c != connections.end(); it_c++) {
            writeConnection(device, *from, *it_c, includeInternal);
        }
    }
    if (numConnections > 0) {
        device << "\n";
    }
    if (includeInternal) {
        // ... internal successors if not unwanted
        bool hadAny = false;
        for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
            hadAny |= writeInternalConnections(device, *(*i).second);
        }
        if (hadAny) {
            device << "\n";
        }
    }
    // write loaded prohibitions
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        writeProhibitions(device, i->second->getProhibitions());
    }

    // write roundabout information
    const std::vector<std::set<NBEdge*> > &roundabouts = nb.getRoundabouts();
    for (std::vector<std::set<NBEdge*> >::const_iterator i = roundabouts.begin(); i != roundabouts.end(); ++i) {
        writeRoundabout(device, *i);
    }
    if (roundabouts.size() != 0) {
        device << "\n";
    }

    // write the districts
    for (std::map<std::string, NBDistrict*>::const_iterator i = dc.begin(); i != dc.end(); i++) {
        writeDistrict(device, *(*i).second);
    }
    if (dc.size() != 0) {
        device << "\n";
    }
    device.close();
}


bool
NWWriter_SUMO::writeInternalEdges(OutputDevice& into, const NBNode& n) {
    bool ret = false;
    const EdgeVector& incoming = n.getIncomingEdges();
    for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        const std::vector<NBEdge::Connection> &elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge == 0) {
                continue;
            }
            writeInternalEdge(into, (*k).id, (*k).vmax, (*k).shape);
            if ((*k).haveVia) {
                writeInternalEdge(into, (*k).viaID, (*k).viaVmax, (*k).viaShape);
            }
            ret = true;
        }
    }
    return ret;
}


void
NWWriter_SUMO::writeInternalEdge(OutputDevice& into, const std::string& id, SUMOReal vmax, const PositionVector& shape) {
    SUMOReal length = MAX2(shape.length(), (SUMOReal)POSITION_EPS); // microsim needs positive length
    into.openTag(SUMO_TAG_EDGE);
    into.writeAttr(SUMO_ATTR_ID, id);
    into.writeAttr(SUMO_ATTR_FUNCTION, toString(EDGEFUNC_INTERNAL));
    into << ">\n";
    into.openTag(SUMO_TAG_LANE);
    into.writeAttr(SUMO_ATTR_ID, id + "_0");
    into.writeAttr(SUMO_ATTR_INDEX, 0);
    into.writeAttr(SUMO_ATTR_SPEED, vmax);
    into.writeAttr(SUMO_ATTR_LENGTH, length);
    into.writeAttr(SUMO_ATTR_SHAPE, shape);
    into.closeTag(true);
    into.closeTag();
}


void
NWWriter_SUMO::writeEdge(OutputDevice& into, const NBEdge& e, bool noNames) {
    // write the edge's begin
    into.openTag(SUMO_TAG_EDGE).writeAttr(SUMO_ATTR_ID, e.getID());
    into.writeAttr(SUMO_ATTR_FROM, e.getFromNode()->getID());
    into.writeAttr(SUMO_ATTR_TO, e.getToNode()->getID());
    if (!noNames && e.getStreetName() != "") {
        into.writeAttr(SUMO_ATTR_NAME, e.getStreetName());
    }
    into.writeAttr(SUMO_ATTR_PRIORITY, e.getPriority());
    if (e.getTypeName() != "") {
        into.writeAttr(SUMO_ATTR_TYPE, e.getTypeName());
    }
    if (e.isMacroscopicConnector()) {
        into.writeAttr(SUMO_ATTR_FUNCTION, "connector");
    }
    // write the spread type if not default ("right")
    if (e.getLaneSpreadFunction() != LANESPREAD_RIGHT) {
        into.writeAttr(SUMO_ATTR_SPREADTYPE, e.getLaneSpreadFunction());
    }
    if (e.hasLoadedLength()) {
        into.writeAttr(SUMO_ATTR_LENGTH, e.getLoadedLength());
    }
    if (!e.hasDefaultGeometry()) {
        into.writeAttr(SUMO_ATTR_SHAPE, e.getGeometry());
    }
    into << ">\n";
    // write the lanes
    const std::vector<NBEdge::Lane> &lanes = e.getLanes();
    SUMOReal length = e.getLoadedLength();
    if (length <= 0) {
        length = (SUMOReal) .1;
    }
    for (unsigned int i = 0; i < (unsigned int) lanes.size(); i++) {
        writeLane(into, e.getID(), e.getLaneID(i), lanes[i], length, i);
    }
    // close the edge
    into.closeTag();
}


void
NWWriter_SUMO::writeLane(OutputDevice& into, const std::string& eID, const std::string& lID, const NBEdge::Lane& lane, SUMOReal length, unsigned int index) {
    // output the lane's attributes
    into.openTag(SUMO_TAG_LANE) << " id=\"" << lID << "\"";
    // the first lane of an edge will be the depart lane
    into << " index=\"" << index << "\"";
    // write the list of allowed/disallowed vehicle classes
    if (lane.allowed.size() > 0) {
        into << " allow=\"" << getVehicleClassNames(lane.allowed) << '\"';
    }
    if (lane.notAllowed.size() > 0) {
        into << " disallow=\"" << getVehicleClassNames(lane.notAllowed) << '\"';
    }
    if (lane.preferred.size() > 0) {
        into << " prefer=\"" << getVehicleClassNames(lane.preferred) << '\"';
    }
    // some further information
    if (lane.speed == 0) {
        WRITE_WARNING("Lane #" + toString(index) + " of edge '" + eID + "' has a maximum velocity of 0.");
    } else if (lane.speed < 0) {
        throw ProcessError("Negative velocity (" + toString(lane.speed) + " on edge '" + eID + "' lane#" + toString(index) + ".");
    }
    if (lane.offset > 0) {
        length = length - lane.offset;
    }
    into.writeAttr(SUMO_ATTR_SPEED, lane.speed);
    into.writeAttr(SUMO_ATTR_LENGTH, length);
    if (lane.offset > 0) {
        into << " endOffset=\"" << lane.offset << '\"';
    }
    if (lane.width != NBEdge::UNSPECIFIED_WIDTH) {
        into << " width=\"" << lane.width << '\"';
    }
    PositionVector shape = lane.shape;
    if (lane.offset > 0) {
        shape = shape.getSubpart(0, shape.length() - lane.offset);
    }
    into << " shape=\"" << shape << "\"";
    into.closeTag(true);
}


void
NWWriter_SUMO::writeJunction(OutputDevice& into, const NBNode& n) {
    // write the attributes
    into.openTag(SUMO_TAG_JUNCTION) << " id=\"" << n.getID() << '\"';
    into.writeAttr(SUMO_ATTR_TYPE, n.getType());
    NWFrame::writePositionLong(n.getPosition(), into);
    into << " incLanes=\"";
    // write the incoming lanes
    const std::vector<NBEdge*> &incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); ++i) {
        unsigned int noLanes = (*i)->getNumLanes();
        for (unsigned int j = 0; j < noLanes; j++) {
            into << (*i)->getLaneID(j);
            if (i != incoming.end() - 1 || j < noLanes - 1) {
                into << ' ';
            }
        }
    }
    into << "\"";
    // write the internal lanes
    into << " intLanes=\"";
    if (!OptionsCont::getOptions().getBool("no-internal-links")) {
        unsigned int l = 0;
        for (EdgeVector::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
            const std::vector<NBEdge::Connection> &elv = (*i)->getConnections();
            for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
                if ((*k).toEdge == 0) {
                    continue;
                }
                if (l != 0) {
                    into << ' ';
                }
                if (!(*k).haveVia) {
                    into << (*k).id << "_0";
                } else {
                    into << (*k).viaID << "_0";
                }
                l++;
            }
        }
    }
    into << "\"";
    // close writing
    into << " shape=\"" << n.getShape() << "\"";
    if (n.getType() == NODETYPE_DEAD_END) {
        into.closeTag(true);
    } else {
        into <<  ">\n";
        // write right-of-way logics
        n.writeLogic(into);
        into.closeTag();
    }
}


bool
NWWriter_SUMO::writeInternalNodes(OutputDevice& into, const NBNode& n) {
    bool ret = false;
    const std::vector<NBEdge*> &incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); i++) {
        const std::vector<NBEdge::Connection> &elv = (*i)->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator k = elv.begin(); k != elv.end(); ++k) {
            if ((*k).toEdge == 0 || !(*k).haveVia) {
                continue;
            }
            Position pos = (*k).shape[-1];
            into.openTag(SUMO_TAG_JUNCTION) << " id=\"" << (*k).viaID << "_0\"";
            into << " type=\"" << toString(NODETYPE_INTERNAL) << "\"";
            NWFrame::writePositionLong(pos, into);
            into << " incLanes=\"";
            std::string furtherIncoming = (*k).sourceNames;
            into << (*k).id << "_0";
            if (furtherIncoming.length() != 0) {
                into << " " << furtherIncoming;
            }
            into << "\"";
            into << " intLanes=\"" << (*k).crossingNames << "\"";
            into.closeTag(true);
            ret = true;
        }
    }
    return ret;
}


void
NWWriter_SUMO::writeConnection(OutputDevice& into, const NBEdge& from, const NBEdge::Connection& c,
                               bool includeInternal, ConnectionStyle style) {
    assert(c.toEdge != 0);
    into.openTag(SUMO_TAG_CONNECTION);
    into.writeAttr(SUMO_ATTR_FROM, from.getID());
    into.writeAttr(SUMO_ATTR_TO, c.toEdge->getID());
    into.writeAttr(SUMO_ATTR_FROM_LANE, c.fromLane);
    into.writeAttr(SUMO_ATTR_TO_LANE, c.toLane);

    if (style != PLAIN) {
        if (includeInternal) {
            into.writeAttr(SUMO_ATTR_VIA, c.id + "_0");
        }
        // set information about the controlling tl if any
        if (c.tlID != "") {
            into.writeAttr(SUMO_ATTR_TLID, c.tlID);
            into.writeAttr(SUMO_ATTR_TLLINKINDEX, c.tlLinkNo);
        }
        if (style == SUMONET) {
            // write the direction information
            LinkDirection dir = from.getToNode()->getDirection(&from, c.toEdge);
            assert(dir != LINKDIR_NODIR);
            into.writeAttr(SUMO_ATTR_DIR, toString(dir));
            // write the state information
            std::string stateCode;
            if (c.tlID != "") {
                stateCode = toString(LINKSTATE_TL_OFF_BLINKING);
            } else {
                stateCode = from.getToNode()->stateCode(&from, c.toEdge, c.toLane, c.mayDefinitelyPass);
            }
            into.writeAttr(SUMO_ATTR_STATE, stateCode);
        }
    }
    into.closeTag(true);
}


bool
NWWriter_SUMO::writeInternalConnections(OutputDevice& into, const NBNode& n) {
    bool ret = false;
    const std::vector<NBEdge*> &incoming = n.getIncomingEdges();
    for (std::vector<NBEdge*>::const_iterator i = incoming.begin(); i != incoming.end(); ++i) {
        NBEdge* from = *i;
        const std::vector<NBEdge::Connection> &connections = from->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator j = connections.begin(); j != connections.end(); ++j) {
            const NBEdge::Connection& c = *j;
            assert(c.toEdge != 0);
            if (c.haveVia) {
                // internal split
                writeInternalConnection(into, c.id, c.toEdge->getID(), c.toLane, c.viaID + "_0");
                writeInternalConnection(into, c.viaID, c.toEdge->getID(), c.toLane, "");
            } else {
                // no internal split
                writeInternalConnection(into, c.id, c.toEdge->getID(), c.toLane, "");
            }
            ret = true;
        }
    }
    return ret;
}


void
NWWriter_SUMO::writeInternalConnection(OutputDevice& into,
                                       const std::string& from, const std::string& to, int toLane, const std::string& via) {
    into.openTag(SUMO_TAG_CONNECTION);
    into.writeAttr(SUMO_ATTR_FROM, from);
    into.writeAttr(SUMO_ATTR_TO, to);
    into.writeAttr(SUMO_ATTR_FROM_LANE, 0);
    into.writeAttr(SUMO_ATTR_TO_LANE, toLane);
    if (via != "") {
        into.writeAttr(SUMO_ATTR_VIA, via);
    }
    into.writeAttr(SUMO_ATTR_DIR, "s");
    into.writeAttr(SUMO_ATTR_STATE, "M");
    into.closeTag(true);
}


void
NWWriter_SUMO::writeRoundabout(OutputDevice& into, const std::set<NBEdge*> &r) {
    std::vector<NBNode*> nodes;
    for (std::set<NBEdge*>::const_iterator j = r.begin(); j != r.end(); ++j) {
        NBNode* n = (*j)->getToNode();
        if (find(nodes.begin(), nodes.end(), n) == nodes.end()) {
            nodes.push_back(n);
        }
    }
    sort(nodes.begin(), nodes.end(), NBNode::nodes_by_id_sorter());
    into.openTag(SUMO_TAG_ROUNDABOUT) << " nodes=\"";
    int k = 0;
    for (std::vector<NBNode*>::iterator j = nodes.begin(); j != nodes.end(); ++j, ++k) {
        if (k != 0) {
            into << ' ';
        }
        into << (*j)->getID();
    }
    into << "\"";
    into.closeTag(true);
}


void
NWWriter_SUMO::writeDistrict(OutputDevice& into, const NBDistrict& d) {
    std::vector<SUMOReal> sourceW = d.getSourceWeights();
    VectorHelper<SUMOReal>::normaliseSum(sourceW, 1.0);
    std::vector<SUMOReal> sinkW = d.getSinkWeights();
    VectorHelper<SUMOReal>::normaliseSum(sinkW, 1.0);
    // write the head and the id of the district
    into.openTag(SUMO_TAG_TAZ) << " id=\"" << d.getID() << "\"";
    if (d.getShape().size() > 0) {
        into << " shape=\"" << d.getShape() << "\"";
    }
    into << ">\n";
    size_t i;
    // write all sources
    const std::vector<NBEdge*> &sources = d.getSourceEdges();
    for (i = 0; i < sources.size(); i++) {
        // write the head and the id of the source
        into.openTag(SUMO_TAG_TAZSOURCE) << " id=\"" << sources[i]->getID() << "\" weight=\"" << sourceW[i] << "\"";
        into.closeTag(true);
    }
    // write all sinks
    const std::vector<NBEdge*> &sinks = d.getSinkEdges();
    for (i = 0; i < sinks.size(); i++) {
        // write the head and the id of the sink
        into.openTag(SUMO_TAG_TAZSINK) << " id=\"" << sinks[i]->getID() << "\" weight=\"" << sinkW[i] << "\"";
        into.closeTag(true);
    }
    // write the tail
    into.closeTag();
}


std::string
NWWriter_SUMO::writeSUMOTime(SUMOTime steps) {
    SUMOReal time = STEPS2TIME(steps);
    if (time == std::floor(time)) {
        return toString(int(time));
    } else {
        return toString(time);
    }
}


void
NWWriter_SUMO::writeProhibitions(OutputDevice& into, const NBConnectionProhibits& prohibitions) {
    for (NBConnectionProhibits::const_iterator j = prohibitions.begin(); j != prohibitions.end(); j++) {
        NBConnection prohibited = (*j).first;
        const NBConnectionVector& prohibiting = (*j).second;
        for (NBConnectionVector::const_iterator k = prohibiting.begin(); k != prohibiting.end(); k++) {
            NBConnection prohibitor = *k;
            into.openTag(SUMO_TAG_PROHIBITION);
            into.writeAttr(SUMO_ATTR_PROHIBITOR, prohibitionConnection(prohibitor));
            into.writeAttr(SUMO_ATTR_PROHIBITED, prohibitionConnection(prohibited));
            into.closeTag(true);
        }
    }
}


std::string
NWWriter_SUMO::prohibitionConnection(const NBConnection& c) {
    return c.getFrom()->getID() + "->" + c.getTo()->getID();
}


void
NWWriter_SUMO::writeTrafficLights(OutputDevice& into, const NBTrafficLightLogicCont& tllCont) {
    std::vector<NBTrafficLightLogic*> logics = tllCont.getComputed();
    for (std::vector<NBTrafficLightLogic*>::iterator it = logics.begin(); it != logics.end(); it++) {
        into.openTag(SUMO_TAG_TLLOGIC);
        into.writeAttr(SUMO_ATTR_ID, (*it)->getID());
        into.writeAttr(SUMO_ATTR_TYPE, toString(TLTYPE_STATIC));
        into.writeAttr(SUMO_ATTR_PROGRAMID, (*it)->getProgramID());
        into.writeAttr(SUMO_ATTR_OFFSET, writeSUMOTime((*it)->getOffset()));
        into << ">\n";
        // write the phases
        const std::vector<NBTrafficLightLogic::PhaseDefinition> &phases = (*it)->getPhases();
        for (std::vector<NBTrafficLightLogic::PhaseDefinition>::const_iterator j = phases.begin(); j != phases.end(); ++j) {
            into.openTag(SUMO_TAG_PHASE);
            into.writeAttr(SUMO_ATTR_DURATION, writeSUMOTime(j->duration));
            into.writeAttr(SUMO_ATTR_STATE, j->state);
            into.closeTag(true);
        }
        into.closeTag();
    }
    if (logics.size() > 0) {
        into << "\n";
    }
}


void
NWWriter_SUMO::writeLocation(OutputDevice& into) {
    const GeoConvHelper& geoConvHelper = GeoConvHelper::getFinal();
    into.openTag(SUMO_TAG_LOCATION);
    into.writeAttr(SUMO_ATTR_NET_OFFSET, geoConvHelper.getOffsetBase());
    into.writeAttr(SUMO_ATTR_CONV_BOUNDARY, geoConvHelper.getConvBoundary());
    if (geoConvHelper.usingGeoProjection()) {
        into.setPrecision(GEO_OUTPUT_ACCURACY);
    } 
    into.writeAttr(SUMO_ATTR_ORIG_BOUNDARY, geoConvHelper.getOrigBoundary());
    if (geoConvHelper.usingGeoProjection()) {
        into.setPrecision();
    } 
    into.writeAttr(SUMO_ATTR_ORIG_PROJ, geoConvHelper.getProjString());
    into.closeTag(true);
    into << "\n";
}

/****************************************************************************/

