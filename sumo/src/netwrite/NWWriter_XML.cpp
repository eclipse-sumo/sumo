/****************************************************************************/
/// @file    NWWriter_XML.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 11.05.2011
/// @version $Id$
///
// Exporter writing networks using XML (native input) format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>
#include "NWFrame.h"
#include "NWWriter_SUMO.h"
#include "NWWriter_XML.h"

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
NWWriter_XML::writeNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether plain-output files shall be generated
    if (oc.isSet("plain-output-prefix")) {
        writeNodes(oc, nb.getNodeCont());
        writeEdgesAndConnections(oc, nb.getNodeCont(), nb.getEdgeCont());
        writeTrafficLights(oc, nb.getTLLogicCont(), nb.getEdgeCont());
    }
    if (oc.isSet("junctions.join-output")) {
        writeJoinedJunctions(oc, nb.getNodeCont());
    }
    if (oc.isSet("street-sign-output")) {
        writeStreetSigns(oc, nb.getEdgeCont());
    }
}


void
NWWriter_XML::writeNodes(const OptionsCont& oc, NBNodeCont& nc) {
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    bool useGeo = oc.exists("proj.plain-geo") && oc.getBool("proj.plain-geo");
    if (useGeo && !gch.usingGeoProjection()) {
        WRITE_WARNING("Ignoring option \"proj.plain-geo\" because no geo-conversion has been defined");
        useGeo = false;
    }
    const bool geoAccuracy = useGeo || gch.usingInverseGeoProjection();

    OutputDevice& device = OutputDevice::getDevice(oc.getString("plain-output-prefix") + ".nod.xml");
    device.writeXMLHeader("nodes", NWFrame::MAJOR_VERSION + " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/nodes_file.xsd\"");

    // write network offsets and projection to allow reconstruction of original coordinates
    if (!useGeo) {
        NWWriter_SUMO::writeLocation(device);
    }

    // write nodes
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NBNode* n = (*i).second;
        device.openTag(SUMO_TAG_NODE);
        device.writeAttr(SUMO_ATTR_ID, n->getID());
        // write position
        Position pos = n->getPosition();
        if (useGeo) {
            gch.cartesian2geo(pos);
        }
        if (geoAccuracy) {
            device.setPrecision(GEO_OUTPUT_ACCURACY);
        }
        NWFrame::writePositionLong(pos, device);
        if (geoAccuracy) {
            device.setPrecision();
        }

        device.writeAttr(SUMO_ATTR_TYPE, toString(n->getType()));
        if (n->isTLControlled()) {
            const std::set<NBTrafficLightDefinition*>& tlss = n->getControllingTLS();
            // set may contain multiple programs for the same id.
            // make sure ids are unique and sorted
            std::set<std::string> tlsIDs;
            for (std::set<NBTrafficLightDefinition*>::const_iterator it_tl = tlss.begin(); it_tl != tlss.end(); it_tl++) {
                tlsIDs.insert((*it_tl)->getID());
            }
            std::vector<std::string> sortedIDs(tlsIDs.begin(), tlsIDs.end());
            sort(sortedIDs.begin(), sortedIDs.end());
            device.writeAttr(SUMO_ATTR_TLID, sortedIDs);
        }
        device.closeTag();
    }
    device.close();
}


void
NWWriter_XML::writeEdgesAndConnections(const OptionsCont& oc, NBNodeCont& nc, NBEdgeCont& ec) {
    const GeoConvHelper& gch = GeoConvHelper::getFinal();
    bool useGeo = oc.exists("proj.plain-geo") && oc.getBool("proj.plain-geo");
    const bool geoAccuracy = useGeo || gch.usingInverseGeoProjection();

    OutputDevice& edevice = OutputDevice::getDevice(oc.getString("plain-output-prefix") + ".edg.xml");
    edevice.writeXMLHeader("edges", NWFrame::MAJOR_VERSION + " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/edges_file.xsd\"");
    OutputDevice& cdevice = OutputDevice::getDevice(oc.getString("plain-output-prefix") + ".con.xml");
    cdevice.writeXMLHeader("connections", NWFrame::MAJOR_VERSION + " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/connections_file.xsd\"");
    bool noNames = !oc.getBool("output.street-names");
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        // write the edge itself to the edges-files
        NBEdge* e = (*i).second;
        edevice.openTag(SUMO_TAG_EDGE);
        edevice.writeAttr(SUMO_ATTR_ID, e->getID());
        edevice.writeAttr(SUMO_ATTR_FROM, e->getFromNode()->getID());
        edevice.writeAttr(SUMO_ATTR_TO, e->getToNode()->getID());
        if (!noNames && e->getStreetName() != "") {
            edevice.writeAttr(SUMO_ATTR_NAME, StringUtils::escapeXML(e->getStreetName()));
        }
        edevice.writeAttr(SUMO_ATTR_PRIORITY, e->getPriority());
        // write the type if given
        if (e->getTypeID() != "") {
            edevice.writeAttr(SUMO_ATTR_TYPE, e->getTypeID());
        }
        edevice.writeAttr(SUMO_ATTR_NUMLANES, e->getNumLanes());
        if (!e->hasLaneSpecificSpeed()) {
            edevice.writeAttr(SUMO_ATTR_SPEED, e->getSpeed());
        }
        // write non-default geometry
        if (!e->hasDefaultGeometry()) {
            PositionVector geom = e->getGeometry();
            if (useGeo) {
                for (int i = 0; i < (int) geom.size(); i++) {
                    gch.cartesian2geo(geom[i]);
                }
            }
            if (geoAccuracy) {
                edevice.setPrecision(GEO_OUTPUT_ACCURACY);
            }
            edevice.writeAttr(SUMO_ATTR_SHAPE, geom);
            if (geoAccuracy) {
                edevice.setPrecision();
            }
        }
        // write the spread type if not default ("right")
        if (e->getLaneSpreadFunction() != LANESPREAD_RIGHT) {
            edevice.writeAttr(SUMO_ATTR_SPREADTYPE, toString(e->getLaneSpreadFunction()));
        }
        // write the length if it was specified
        if (e->hasLoadedLength()) {
            edevice.writeAttr(SUMO_ATTR_LENGTH, e->getLoadedLength());
        }
        // some attributes can be set by edge default or per lane. Write as default if possible (efficiency)
        if (e->getLaneWidth() != NBEdge::UNSPECIFIED_WIDTH && !e->hasLaneSpecificWidth()) {
            edevice.writeAttr(SUMO_ATTR_WIDTH, e->getLaneWidth());
        }
        if (e->getOffset() != NBEdge::UNSPECIFIED_OFFSET && !e->hasLaneSpecificOffset()) {
            edevice.writeAttr(SUMO_ATTR_ENDOFFSET, e->getOffset());
        }
        if (!e->needsLaneSpecificOutput()) {
            edevice.closeTag();
        } else {
            for (unsigned int i = 0; i < e->getLanes().size(); ++i) {
                const NBEdge::Lane& lane = e->getLanes()[i];
                edevice.openTag(SUMO_TAG_LANE);
                edevice.writeAttr(SUMO_ATTR_INDEX, i);
                // write allowed lanes
                NWWriter_SUMO::writePermissions(edevice, lane.permissions);
                NWWriter_SUMO::writePreferences(edevice, lane.preferred);
                // write other attributes
                if (lane.width != NBEdge::UNSPECIFIED_WIDTH && e->hasLaneSpecificWidth()) {
                    edevice.writeAttr(SUMO_ATTR_WIDTH, lane.width);
                }
                if (lane.offset != NBEdge::UNSPECIFIED_OFFSET && e->hasLaneSpecificOffset()) {
                    edevice.writeAttr(SUMO_ATTR_ENDOFFSET, lane.offset);
                }
                if (e->hasLaneSpecificSpeed()) {
                    edevice.writeAttr(SUMO_ATTR_SPEED, lane.speed);
                }
                edevice.closeTag();
            }
            edevice.closeTag();
        }
        // write this edge's connections to the connections-files
        e->sortOutgoingConnectionsByIndex();
        const std::vector<NBEdge::Connection> connections = e->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator c = connections.begin(); c != connections.end(); ++c) {
            NWWriter_SUMO::writeConnection(cdevice, *e, *c, false, NWWriter_SUMO::PLAIN);
        }
        if (connections.size() > 0) {
            cdevice << "\n";
        }
    }

    // write loaded prohibitions to the connections-file
    for (std::map<std::string, NBNode*>::const_iterator i = nc.begin(); i != nc.end(); ++i) {
        NWWriter_SUMO::writeProhibitions(cdevice, i->second->getProhibitions());
    }
    edevice.close();
    cdevice.close();
}


void
NWWriter_XML::writeTrafficLights(const OptionsCont& oc, NBTrafficLightLogicCont& tc, NBEdgeCont& ec) {
    OutputDevice& device = OutputDevice::getDevice(oc.getString("plain-output-prefix") + ".tll.xml");
    device.writeXMLHeader("tlLogics", NWFrame::MAJOR_VERSION + " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/tllogic_file.xsd\"");
    NWWriter_SUMO::writeTrafficLights(device, tc);
    // we also need to remember the associations between tlLogics and connections
    // since the information in con.xml is insufficient
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        NBEdge* e = (*i).second;
        // write this edge's tl-controlled connections
        const std::vector<NBEdge::Connection> connections = e->getConnections();
        for (std::vector<NBEdge::Connection>::const_iterator c = connections.begin(); c != connections.end(); ++c) {
            if (c->tlID != "") {
                NWWriter_SUMO::writeConnection(device, *e, *c, false, NWWriter_SUMO::TLL);
            }
        }
    }
    device.close();
}


void
NWWriter_XML::writeJoinedJunctions(const OptionsCont& oc, NBNodeCont& nc) {
    OutputDevice& device = OutputDevice::getDevice(oc.getString("junctions.join-output"));
    device.writeXMLHeader("nodes", NWFrame::MAJOR_VERSION + " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/nodes_file.xsd\"");
    const std::vector<std::set<std::string> >& clusters = nc.getJoinedClusters();
    for (std::vector<std::set<std::string> >::const_iterator it = clusters.begin(); it != clusters.end(); it++) {
        assert((*it).size() > 0);
        device.openTag(SUMO_TAG_JOIN);
        // prepare string
        std::ostringstream oss;
        for (std::set<std::string>::const_iterator it_id = it->begin(); it_id != it->end(); it_id++) {
            oss << *it_id << " ";
        }
        // remove final space
        std::string ids = oss.str();
        device.writeAttr(SUMO_ATTR_NODES, ids.substr(0, ids.size() - 1));
        device.closeTag();
    }
    device.close();
}


void
NWWriter_XML::writeStreetSigns(const OptionsCont& oc, NBEdgeCont& ec) {
    OutputDevice& device = OutputDevice::getDevice(oc.getString("street-sign-output"));
    device.writeXMLHeader("pois", NWFrame::MAJOR_VERSION + " xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xsi:noNamespaceSchemaLocation=\"http://sumo-sim.org/xsd/poi_file.xsd\"");
    for (std::map<std::string, NBEdge*>::const_iterator i = ec.begin(); i != ec.end(); ++i) {
        NBEdge* e = (*i).second;
        const std::vector<NBSign>& signs =  e->getSigns();
        for (std::vector<NBSign>::const_iterator it = signs.begin(); it != signs.end(); ++it) {
            it->writeAsPOI(device, e);
        }
    }
    device.close();
}
/****************************************************************************/

