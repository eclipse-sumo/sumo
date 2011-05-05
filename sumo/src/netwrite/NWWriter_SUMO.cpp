/****************************************************************************/
/// @file    NWWriter_SUMO.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 04.05.2011
/// @version $Id$
///
// Exporter writing networks using the SUMO format
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
#include "NWWriter_SUMO.h"
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
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
NWWriter_SUMO::writeNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether a matsim-file shall be generated
    if (oc.getString("output-format")!="sumo") {
        return;
    }
	OutputDevice& device = OutputDevice::getDevice(oc.getString("output"));
    device.writeXMLHeader("net");
    device << "\n";
    // write network offsets
    device << "   <location netOffset=\"" << GeoConvHelper::getOffsetBase() << "\""
    << " convBoundary=\"" << GeoConvHelper::getConvBoundary() << "\"";
    if (GeoConvHelper::usingGeoProjection()) {
        device.setPrecision(GEO_OUTPUT_ACCURACY);
        device << " origBoundary=\"" << GeoConvHelper::getOrigBoundary() << "\"";
        device.setPrecision();
    } else {
        device << " origBoundary=\"" << GeoConvHelper::getOrigBoundary() << "\"";
    }
    device << " projParameter=\"" << GeoConvHelper::getProjString() << "\"/>\n\n";

    // write the numbers of some elements
    // edges
    if (!oc.getBool("no-internal-links")) {
		nb.getNodeCont().writeXMLInternalLinks(device);
    }

    // write edges with lanes and connected edges
    nb.getEdgeCont().writeXMLStep1(device);
    // write the logics
	nb.getJunctionLogicCont().writeXML(device);
	nb.getTLLogicCont().writeXML(device);
    // write the nodes
    nb.getNodeCont().writeXML(device);
    // write internal nodes
    if (!oc.getBool("no-internal-links")) {
        nb.getNodeCont().writeXMLInternalNodes(device);
    }
    // write the successors of lanes
    nb.getEdgeCont().writeXMLStep2(device, !oc.getBool("no-internal-links"));
    if (!oc.getBool("no-internal-links")) {
        nb.getNodeCont().writeXMLInternalSuccInfos(device);
    }
    // write roundabout information
	const std::vector<std::set<NBEdge*> > &roundabouts = nb.getRoundabouts();
    for (std::vector<std::set<NBEdge*> >::const_iterator i=roundabouts.begin(); i!=roundabouts.end(); ++i) {
        std::vector<NBNode*> nodes;
        for (std::set<NBEdge*>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            NBNode *n = (*j)->getToNode();
            if (find(nodes.begin(), nodes.end(), n)==nodes.end()) {
                nodes.push_back(n);
            }
        }
		sort(nodes.begin(), nodes.end(), NBNode::nodes_by_id_sorter());
        device << "   <roundabout nodes=\"";
        int k = 0;
        for (std::vector<NBNode*>::iterator j=nodes.begin(); j!=nodes.end(); ++j, ++k) {
            if (k!=0) {
                device << ' ';
            }
            device << (*j)->getID();
        }
        device << "\"/>\n";
    }
    if (roundabouts.size()!=0) {
        device << "\n";
    }
    // write the districts
	nb.getDistrictCont().writeXML(device);
    device.close();
}


/****************************************************************************/

