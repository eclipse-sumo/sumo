/****************************************************************************/
/// @file    NBNetBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    20 Nov 2001
/// @version $Id$
///
// Instance responsible for building networks
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

#include <string>
#include <fstream>
#include "NBNetBuilder.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTrafficLightLogicCont.h"
#include "NBJunctionLogicCont.h"
#include "NBDistrictCont.h"
#include "NBDistribution.h"
#include "NBRequest.h"
#include "NBTypeCont.h"
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include "NBJoinedEdgesMap.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NBNetBuilder::NBNetBuilder() throw()
        : myEdgeCont(myTypeCont) {}


NBNetBuilder::~NBNetBuilder() throw() {}


void
NBNetBuilder::applyOptions(OptionsCont &oc) throw(ProcessError) {
    // we possibly have to load the edges to keep
    if (oc.isSet("keep-edges.input-file")) {
        std::ifstream strm(oc.getString("keep-edges.input-file").c_str());
        if (!strm.good()) {
            throw ProcessError("Could not load names of edges too keep from '" + oc.getString("keep-edges.input-file") + "'.");
        }
        std::ostringstream oss;
        bool first = true;
        while (strm.good()) {
            if (!first) {
                oss << ',';
            }
            std::string name;
            strm >> name;
            oss << name;
            first = false;
        }
        oc.set("keep-edges", oss.str());
    }
    // check whether at least one output file is given
    if (!oc.isSet("output")&&!oc.isSet("plain-output")&&!oc.isSet("map-output")) {
        throw ProcessError("No output defined.");
    }
    // apply options to type control
    myTypeCont.setDefaults(oc.getInt("lanenumber"), oc.getFloat("speed"), oc.getInt("priority"));
    // apply options to edge control
    myEdgeCont.applyOptions(oc);
    // apply options to traffic light logics control
    myTLLCont.applyOptions(oc);
}


void
NBNetBuilder::buildLoaded(OptionsCont& oc) {
    // perform the computation
    compute(oc);
    // save the mapping information when wished
    if (oc.isSet("map-output")) {
        OutputDevice& mdevice = OutputDevice::getDevice(oc.getString("map-output"));
        mdevice << gJoinedEdges;
    }
}


void
NBNetBuilder::compute(OptionsCont &oc) throw(ProcessError) {
    int step = 1;
    //
    // Removes edges that are connecting the same node
    inform(step, "Removing dummy edges.");
    myNodeCont.removeDummyEdges(myDistrictCont, myEdgeCont, myTLLCont);
    //
    inform(step, "Joining double connections.");
    gJoinedEdges.init(myEdgeCont);
    myNodeCont.recheckEdges(myDistrictCont, myTLLCont, myEdgeCont);
    //
    if (oc.getBool("remove-isolated")) {
        inform(step, "Finding isolated roads.");
        myNodeCont.removeIsolatedRoads(myDistrictCont, myEdgeCont, myTLLCont);
    }
    //
    if (oc.getBool("remove-geometry")) {
        inform(step, "Removing empty nodes and geometry nodes.");
        myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myTLLCont, oc.getBool("remove-geometry"));
    }
    //
    if (oc.getBool("keep-edges.postload")) {
        if (oc.isSet("keep-edges")) {
            inform(step, "Removing unwished edges.");
            myEdgeCont.removeUnwishedEdges(myDistrictCont);
        }
    }
    if (oc.isSet("keep-edges") || oc.isSet("remove-edges") || oc.getBool("keep-edges.postload") || oc.isSet("keep-edges.by-vclass") || oc.isSet("keep-edges.input-file")) {
        inform(step, "Rechecking nodes after edge removal.");
        myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myTLLCont, oc.getBool("remove-geometry"));
    }
    //
    if (oc.getBool("split-geometry")) {
        inform(step, "Splitting geometry edges.");
        myEdgeCont.splitGeometry(myNodeCont);
    }
    //
    if (!oc.getBool("disable-normalize-node-positions") && oc.isDefault("x-offset-to-apply") && oc.isDefault("y-offset-to-apply")) {
        inform(step, "Normalising node positions.");
        const SUMOReal x = -GeoConvHelper::getConvBoundary().xmin();
        const SUMOReal y = -GeoConvHelper::getConvBoundary().ymin();
        myNodeCont.reshiftNodePositions(x, y);
        myEdgeCont.reshiftEdgePositions(x, y);
        myDistrictCont.reshiftDistrictPositions(x, y);
        GeoConvHelper::moveConvertedBy(x, y);
    }
    //
    myEdgeCont.recomputeLaneShapes();
    //
    if (oc.getBool("ramps.guess")||oc.isSet("ramps.set")) {
        inform(step, "Guessing and setting on-/off-ramps.");
        myNodeCont.guessRamps(oc, myEdgeCont, myDistrictCont);
    }
    //
    inform(step, "Guessing and setting TLs.");
    if (oc.isSet("tls.set")) {
        std::vector<std::string> tlControlledNodes = oc.getStringVector("tls.set");
        for (std::vector<std::string>::const_iterator i=tlControlledNodes.begin(); i!=tlControlledNodes.end(); ++i) {
            NBNode *node = myNodeCont.retrieve(*i);
            if (node==0) {
                WRITE_WARNING("Building a tl-logic for node '" + *i + "' is not possible." + "\n The node '" + *i + "' is not known.");
            } else {
                myNodeCont.setAsTLControlled(node, myTLLCont);
            }
        }
    }
    myNodeCont.guessTLs(oc, myTLLCont);
    //
    if (oc.getBool("tls.join")) {
        myNodeCont.joinTLS(myTLLCont);
    }
    //
    inform(step, "Computing turning directions.");
    myEdgeCont.computeTurningDirections();
    //
    inform(step, "Sorting nodes' edges.");
    myNodeCont.sortNodesEdges(oc.getBool("lefthand"), myTypeCont);
    //
    if (oc.getBool("guess-roundabouts")) {
        inform(step, "Guessing and setting roundabouts.");
        myEdgeCont.guessRoundabouts(myRoundabouts);
    }
    //
    inform(step, "Computing approached edges.");
    myEdgeCont.computeEdge2Edges();
    //
    inform(step, "Computing approaching lanes.");
    myEdgeCont.computeLanes2Edges();
    //
    inform(step, "Dividing of lanes on approached lanes.");
    myNodeCont.computeLanes2Lanes();
    myEdgeCont.sortOutgoingLanesConnections();
    //
    if (!oc.getBool("no-turnarounds")) {
        inform(step, "Appending turnarounds.");
        myEdgeCont.appendTurnarounds(oc.getBool("no-tls-turnarounds"));
    }
    //
    inform(step, "Rechecking of lane endings.");
    myEdgeCont.recheckLanes();
    // save plain nodes/edges/connections
    if (oc.isSet("plain-output")) {
        myNodeCont.savePlain(oc.getString("plain-output") + ".nod.xml");
        myEdgeCont.savePlain(oc.getString("plain-output") + ".edg.xml", oc.getString("plain-output") + ".con.xml");
    }
    //
    inform(step, "Computing node shapes.");
    myNodeCont.computeNodeShapes(oc.getBool("lefthand"));
    //
    inform(step, "Computing edge shapes.");
    myEdgeCont.computeEdgeShapes();
    //
    inform(step, "Computing tls logics.");
    myTLLCont.setTLControllingInformation(myEdgeCont);
    //
    inform(step, "Computing node logics.");
    myNodeCont.computeLogics(myEdgeCont, myJunctionLogicCont, oc);
    //
    inform(step, "Computing traffic light logics.");
    myTLLCont.computeLogics(myEdgeCont, oc);
    // report
    WRITE_MESSAGE("-----------------------------------------------------");
    WRITE_MESSAGE("Summary:");
    if (!gSuppressMessages) {
        myNodeCont.printBuiltNodesStatistics();
    }
    WRITE_MESSAGE(" Network boundaries:");
    WRITE_MESSAGE("  Original boundary  : " + toString(GeoConvHelper::getOrigBoundary()));
    WRITE_MESSAGE("  Applied offset     : " + toString(GeoConvHelper::getOffsetBase()));
    WRITE_MESSAGE("  Converted boundary : " + toString(GeoConvHelper::getConvBoundary()));
    WRITE_MESSAGE("-----------------------------------------------------");
    NBRequest::reportWarnings();
}


void
NBNetBuilder::inform(int &step, const std::string &about) throw() {
    WRITE_MESSAGE("Computing step " + toString<int>(step)+ ": " + about);
    step++;
}



// ----- proetected methods
void
NBNetBuilder::save(OutputDevice &device, OptionsCont &oc) throw(IOError) {
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
        myNodeCont.writeXMLInternalLinks(device);
    }

    // write edges with lanes and connected edges
    myEdgeCont.writeXMLStep1(device);
    // write the logics
    myJunctionLogicCont.writeXML(device);
    myTLLCont.writeXML(device);
    // write the nodes
    myNodeCont.writeXML(device);
    // write internal nodes
    if (!oc.getBool("no-internal-links")) {
        myNodeCont.writeXMLInternalNodes(device);
    }
    // write the successors of lanes
    myEdgeCont.writeXMLStep2(device, !oc.getBool("no-internal-links"));
    if (!oc.getBool("no-internal-links")) {
        myNodeCont.writeXMLInternalSuccInfos(device);
    }
    // write roundabout information
    for (std::vector<std::set<NBEdge*> >::iterator i=myRoundabouts.begin(); i!=myRoundabouts.end(); ++i) {
        std::vector<NBNode*> nodes;
        for (std::set<NBEdge*>::iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            NBNode *n = (*j)->getToNode();
            if (find(nodes.begin(), nodes.end(), n)==nodes.end()) {
                nodes.push_back(n);
            }
        }
        sort(nodes.begin(), nodes.end(), by_id_sorter());
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
    if (myRoundabouts.size()!=0) {
        device << "\n";
    }
    // write the districts
    myDistrictCont.writeXML(device);
    device.close();
}


/****************************************************************************/
