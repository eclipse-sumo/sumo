/****************************************************************************/
/// @file    NWWriter_XML.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 11.05.2011
/// @version $Id$
///
// Exporter writing networks using XML (native input) format
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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
#include <algorithm>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>
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
NWWriter_XML::writeNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether a matsim-file shall be generated
    if (!oc.isSet("plain-output-prefix")) {
        return;
    }
    // write nodes
	OutputDevice& device = OutputDevice::getDevice(oc.getString("plain-output-prefix") + ".nod.xml");
    device.writeXMLHeader("nodes", " encoding=\"iso-8859-1\""); // street names may contain non-ascii chars
	NBNodeCont &nc = nb.getNodeCont();
	for(std::map<std::string, NBNode*>::const_iterator i=nc.begin(); i!=nc.end(); ++i) {
        NBNode *n = (*i).second;
        device << "   <node id=\"" << n->getID() << "\" ";
        if (GeoConvHelper::usingInverseGeoProjection()) {
            device.setPrecision(GEO_OUTPUT_ACCURACY);
            device << "x=\"" << n->getPosition().x() << "\" y=\"" << n->getPosition().y() << "\"";
            device.setPrecision();
        } else {
            device << "x=\"" << n->getPosition().x() << "\" y=\"" << n->getPosition().y() << "\"";
        }
        device << " type=\"" << toString(n->getType())<< "\"";
        if (n->isTLControlled()) {
            device << " tl=\"";
            const std::set<NBTrafficLightDefinition*> &tlss = n->getControllingTLS();
            // set may contain multiple programs for the same id. 
            // make sure ids are unique and sorted
            std::set<std::string> tlsIDs;
            for (std::set<NBTrafficLightDefinition*>::const_iterator it_tl =tlss.begin(); it_tl!=tlss.end(); it_tl++) {
                tlsIDs.insert((*it_tl)->getID());
            }
            std::vector<std::string> sortedIDs(tlsIDs.begin(), tlsIDs.end());
            sort(sortedIDs.begin(), sortedIDs.end());
            for (std::vector<std::string>::iterator it_tlid=sortedIDs.begin(); it_tlid!=sortedIDs.end(); it_tlid++) {
                if (it_tlid!=sortedIDs.begin()) {
                    device << ",";
                }
                device << (*it_tlid);
            }
            device << "\"";
        }
        device << "/>\n";
    }
    device.close();
    // write edges / connections
    OutputDevice& edevice = OutputDevice::getDevice(oc.getString("plain-output-prefix") + ".edg.xml");
    edevice.writeXMLHeader("edges", " encoding=\"iso-8859-1\""); // street names may contain non-ascii chars
    OutputDevice& cdevice = OutputDevice::getDevice(oc.getString("plain-output-prefix") + ".con.xml");
    cdevice.writeXMLHeader("connections", " encoding=\"iso-8859-1\""); // street names may contain non-ascii chars
	NBEdgeCont &ec = nb.getEdgeCont();
    bool noNames = oc.getBool("output.no-names");
	for(std::map<std::string, NBEdge*>::const_iterator i=ec.begin(); i!=ec.end(); ++i) {
        // write the edge itself to the edges-files
        NBEdge *e = (*i).second;
        edevice << "   <edge id=\"" << e->getID()
        << "\" from=\"" << e->getFromNode()->getID()
        << "\" to=\"" << e->getToNode()->getID();
        if (!noNames && e->getStreetName() != "") {
            edevice << "\" " << toString(SUMO_ATTR_NAME) << "=\"" << e->getStreetName();
        }
        edevice << "\" priority=\"" << e->getPriority();
        // write the type if given
        if (e->getTypeID() != "") {
            edevice << "\" type=\"" << e->getTypeID();
        }
        edevice << "\" numLanes=\"" << e->getNoLanes()
        << "\" speed=\"" << e->getSpeed() << "\"";
        // write inner geometry (if any)
        if (!e->hasDefaultGeometry()) {
            edevice << " shape=\"" << e->getGeometry() << "\"";
        }
        // write the spread type if not default ("right")
        if (e->getLaneSpreadFunction()!=LANESPREAD_RIGHT) {
            edevice << " spreadType=\"" << toString(e->getLaneSpreadFunction()) << "\"";
        }
        // write the length if it was specified
        if (e->hasLoadedLength()) {
            edevice << " " << toString(SUMO_ATTR_LENGTH) << "=\"" << e->getLoadedLength() << "\"";
        }
        // write the vehicles class if restrictions exist
        if (!e->hasRestrictions()) {
            edevice << "/>\n";
        } else {
            edevice << ">\n";
            for (unsigned int i=0; i<e->getLanes().size(); ++i) {
                edevice << "      <lane id=\"" << i << "\"";
                const NBEdge::Lane &lane = e->getLanes()[i];
                // write allowed lanes
                if (lane.allowed.size()!=0) {
                    edevice << " allow=\"" << getVehicleClassNames(lane.allowed) << '\"';
                }
                if (lane.notAllowed.size()!=0) {
                    edevice << " disallow=\"" << getVehicleClassNames(lane.notAllowed) << '\"';
                }
                if (lane.preferred.size()!=0) {
                    edevice << " prefer=\"" << getVehicleClassNames(lane.preferred) << '\"';
                }
                edevice << "/>\n";
            }
            edevice << "   </edge>\n";
        }
        // write this edge's connections to the connections-files
        unsigned int noLanes = e->getNoLanes();
        unsigned int noWritten = 0;
        for (unsigned int lane=0; lane<noLanes; ++lane) {
            std::vector<NBEdge::Connection> connections = e->getConnectionsFromLane(lane);
            for (std::vector<NBEdge::Connection>::iterator c=connections.begin(); c!=connections.end(); ++c) {
                if ((*c).toEdge!=0) {
                    cdevice << "	<connection from=\"" << e->getID()
                    << "\" to=\"" << (*c).toEdge->getID()
                    << "\" lane=\"" << (*c).fromLane << ":" << (*c).toLane;
                    cdevice << "\"/>\n";
                    ++noWritten;
                }
            }
        }
        if (noWritten>0) {
            cdevice << "\n";
        }
    }
    edevice.close();
    cdevice.close();
}


/****************************************************************************/

