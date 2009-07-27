/****************************************************************************/
/// @file    NBNetBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    20 Nov 2001
/// @version $Id$
///
// Instance responsible for building networks
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NBNetBuilder::NBNetBuilder() throw()
        : myEdgeCont(*this) {}


NBNetBuilder::~NBNetBuilder() throw() {}


void
NBNetBuilder::applyOptions(OptionsCont &oc) throw(ProcessError) {
    // we possibly have to load the edges to keep
    if (oc.isSet("keep-edges.input-file")) {
        ifstream strm(oc.getString("keep-edges.input-file").c_str());
        if (!strm.good()) {
            throw ProcessError("Could not load names of edges too keep from '" + oc.getString("keep-edges.input-file") + "'.");
        }
        std::ostringstream oss;
        bool first = true;
        while (strm.good()) {
            if (!first) {
                oss << ';';
            }
            string name;
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
NBNetBuilder::buildLoaded() throw(IOError) {
    // perform the computation
    OptionsCont &oc = OptionsCont::getOptions();
    compute(oc);
    // save network
    OutputDevice& device = OutputDevice::getDevice(oc.getString("output"));
    save(device, oc);
    // save the mapping information when wished
    if (oc.isSet("map-output")) {
        OutputDevice& mdevice = OutputDevice::getDevice(oc.getString("map-output"));
        mdevice << gJoinedEdges;
    }
    // save the tls positions as a list of pois
    if (oc.isSet("tls-poi-output")) {
        OutputDevice& mdevice = OutputDevice::getDevice(oc.getString("tls-poi-output"));
        myNodeCont.writeTLSasPOIs(mdevice);
    }
}


void
NBNetBuilder::compute(OptionsCont &oc) throw(ProcessError) {
    int step = 1;
    //
    // Removes edges that are connecting the same node
    inform(step, "Removing dummy edges");
    myNodeCont.removeDummyEdges(myDistrictCont, myEdgeCont, myTLLCont);
    //
    inform(step, "Joining double connections");
    gJoinedEdges.init(myEdgeCont);
    myNodeCont.recheckEdges(myDistrictCont, myTLLCont, myEdgeCont);
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
    if (!oc.getBool("disable-normalize-node-positions")) {
        inform(step, "Normalising node positions");
        myNodeCont.normaliseNodePositions();
        myEdgeCont.normaliseEdgePositions();
        myDistrictCont.normaliseDistrictPositions();
    }
    //
    myEdgeCont.recomputeLaneShapes();
    //
    if (oc.getBool("guess-ramps")||oc.getBool("guess-obscure-ramps")) {
        inform(step, "Guessing and setting on-/off-ramps");
        myNodeCont.guessRamps(oc, myEdgeCont, myDistrictCont);
    }
    //
    inform(step, "Guessing and setting TLs");
    if (oc.isSet("explicite-tls")) {
        vector<string> tlControlledNodes = oc.getStringVector("explicite-tls");
        for (vector<string>::const_iterator i=tlControlledNodes.begin(); i!=tlControlledNodes.end(); ++i) {
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
    if (oc.getBool("try-join-tls")) {
        myNodeCont.joinTLS(myTLLCont);
    }
    //
    inform(step, "Computing turning directions");
    myEdgeCont.computeTurningDirections();
    //
    OutputDevice::createDeviceByOption("node-type-output", "pois");
    inform(step, "Sorting nodes' edges");
    myNodeCont.sortNodesEdges(myTypeCont);
    //
    if (oc.getBool("guess-roundabouts")) {
        inform(step, "Guessing and setting roundabouts");
        myEdgeCont.guessRoundabouts(myRoundabouts);
    }
    //
    inform(step, "Computing Approached Edges");
    myEdgeCont.computeEdge2Edges();
    //
    inform(step, "Computing Approaching Lanes");
    myEdgeCont.computeLanes2Edges();
    //
    inform(step, "Dividing of Lanes on Approached Lanes");
    myNodeCont.computeLanes2Lanes();
    myEdgeCont.sortOutgoingLanesConnections();
    //
    if (!oc.getBool("no-turnarounds")) {
        inform(step, "Appending Turnarounds");
        myEdgeCont.appendTurnarounds(oc.getBool("no-tls-turnarounds"));
    }
    //
    inform(step, "Rechecking of lane endings");
    myEdgeCont.recheckLanes();
    // save plain nodes/edges/connections
    if (oc.isSet("plain-output")) {
        myNodeCont.savePlain(oc.getString("plain-output") + ".nod.xml");
        myEdgeCont.savePlain(oc.getString("plain-output") + ".edg.xml", oc.getString("plain-output") + ".con.xml");
    }
    //
    inform(step, "Computing node shapes");
    myNodeCont.computeNodeShapes();
    //
    inform(step, "Computing edge shapes");
    myEdgeCont.computeEdgeShapes();
    //
    inform(step, "Computing tls logics");
    myTLLCont.setTLControllingInformation(myEdgeCont);
    //
    inform(step, "Computing node logics");
    myNodeCont.computeLogics(myEdgeCont, myJunctionLogicCont, oc);
    //
    inform(step, "Computing traffic light logics");
    myTLLCont.computeLogics(myEdgeCont, oc);
    //
    if (!oc.isDefault("x-offset-to-apply")) {
        inform(step, "Transposing network");
        SUMOReal xoff = oc.getFloat("x-offset-to-apply");
        SUMOReal yoff = oc.getFloat("y-offset-to-apply");
        SUMOReal rot = oc.getFloat("rotation-to-apply");
        inform(step, "Normalising node positions");
        myNodeCont.reshiftNodePositions(xoff, yoff, rot);
        myEdgeCont.reshiftEdgePositions(xoff, yoff, rot);
    }
    // report
    WRITE_MESSAGE("-----------------------------------------------------");
    WRITE_MESSAGE("Summary:");
    if (!gSuppressMessages) {
        myNodeCont.printBuiltNodesStatistics();
    }
    WRITE_MESSAGE(" Network boundaries:");
    WRITE_MESSAGE("  Original boundary  : " + toString(GeoConvHelper::getOrigBoundary()));
    WRITE_MESSAGE("  Applied offset     : " + toString(GeoConvHelper::getOffset()));
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
    device << "   <location netOffset=\"" << GeoConvHelper::getOffset() << "\""
    << " convBoundary=\"" << GeoConvHelper::getConvBoundary() << "\"";
    if (oc.getBool("use-projection")) {
        device.setPrecision(6);
        device << " origBoundary=\"" << GeoConvHelper::getOrigBoundary() << "\"";
        device.setPrecision();
        if (oc.getBool("proj.simple")) {
            device << " projParameter=\"-\"";
        } else {
            device << " projParameter=\"" << oc.getString("proj") << "\"";
        }
    } else {
        device << " origBoundary=\"" << GeoConvHelper::getOrigBoundary() << "\"";
        device << " projParameter=\"!\"";
    }
    device << "/>\n\n";

    // write the numbers of some elements
    // edges
    if (!oc.getBool("no-internal-links")) {
        myNodeCont.writeXMLInternalLinks(device);
    }

    // write the districts
    myDistrictCont.writeXML(device);
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
        for (set<NBEdge*>::iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            NBNode *n = (*j)->getToNode();
            if (find(nodes.begin(), nodes.end(), n)==nodes.end()) {
                nodes.push_back(n);
            }
        }
        sort(nodes.begin(), nodes.end(), by_id_sorter());
        device << "   <roundabout nodes=\"";
        int k = 0;
        for (vector<NBNode*>::iterator j=nodes.begin(); j!=nodes.end(); ++j, ++k) {
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
    device.close();
}



// ----- static methods
void
NBNetBuilder::insertNetBuildOptions(OptionsCont &oc) {
    // register additional output options
    oc.doRegister("output-file", 'o', new Option_FileName("net.net.xml"));
    oc.addSynonyme("output-file", "output");
    oc.addDescription("output-file", "Output", "The generated net will be written to FILE");

    oc.doRegister("plain-output", new Option_FileName());
    oc.addDescription("plain-output", "Output", "Prefix of files to write nodes and edges to");

    oc.doRegister("node-geometry-dump", new Option_FileName());
    oc.addDescription("node-geometry-dump", "Output", "Writes node corner positions to FILE");

    oc.doRegister("map-output", 'M', new Option_FileName());
    oc.addDescription("map-output", "Output", "Writes joined edges information to FILE");

    oc.doRegister("tls-poi-output", new Option_FileName());
    oc.addDescription("tls-poi-output", "Output", "Writes pois of tls positions to FILE");

    oc.doRegister("node-type-output", new Option_FileName());
    oc.addDescription("node-type-output", "Output", "Writes pois of node types to FILE"); // !!! describe, rename



    // register building defaults
    oc.doRegister("type", 'T', new Option_String("Unknown"));
    oc.addDescription("type", "Building Defaults", "The default name for an edges type");

    oc.doRegister("lanenumber", 'L', new Option_Integer(1));
    oc.addDescription("lanenumber", "Building Defaults", "The default number of lanes in an edge");

    oc.doRegister("speed", 'S', new Option_Float((SUMOReal) 13.9));
    oc.addDescription("speed", "Building Defaults", "The default speed on an edge (in m/s)");

    oc.doRegister("priority", 'P', new Option_Integer(-1));
    oc.addDescription("priority", "Building Defaults", "The default priority of an edge");


    // projection options
    oc.doRegister("use-projection", new Option_Bool(false));
    oc.addDescription("use-projection", "Projection", "Enables reprojection from geo to cartesian");

    oc.doRegister("proj.simple", new Option_Bool(false));
    oc.addDescription("proj.simple", "Projection", "Uses a simple method for projection");

    oc.doRegister("proj", new Option_String("+proj=utm +ellps=bessel +units=m"));
    oc.addDescription("proj", "Projection", "Uses STR as proj.4 definition for projection");

    oc.doRegister("proj.inverse", new Option_Bool(false));
    oc.addDescription("proj.inverse", "Projection", "Inverses projection");


    // register the data processing options
    oc.doRegister("flip-y", new Option_Bool(false));
    oc.addDescription("flip-y", "Processing", "Flips the y-coordinate along zero");

    oc.doRegister("dismiss-vclasses", new Option_Bool(false));
    oc.addDescription("dismiss-vclasses", "Processing", "");

    oc.doRegister("remove-geometry", 'R', new Option_Bool(false));
    oc.addDescription("remove-geometry", "Processing", "Removes geometry information from edges");

    oc.doRegister("add-node-positions", new Option_Bool(false));
    oc.addDescription("add-node-positions", "Processing", "Adds node positions to edge geometries");

    oc.doRegister("no-turnarounds", new Option_Bool(false));
    oc.addDescription("no-turnarounds", "Processing", "Disables building turnarounds");
    oc.doRegister("no-tls-turnarounds", new Option_Bool(false));
    oc.addDescription("no-tls-turnarounds", "Processing", "Disables building turnarounds at tls-controlled junctions");

    oc.doRegister("no-internal-links", new Option_Bool(false)); // !!! not described
    oc.addDescription("no-internal-links", "Processing", "Omits internal links");

    oc.doRegister("split-geometry", new Option_Bool(false)); // !!!not described
    oc.addDescription("split-geometry", "Processing", "Splits edges across geometry nodes");

    oc.doRegister("disable-normalize-node-positions", new Option_Bool(false));
    oc.addDescription("disable-normalize-node-positions", "Processing", "Turn off normalizing node positions");

    oc.doRegister("x-offset-to-apply", new Option_Float(0));
    oc.addDescription("x-offset-to-apply", "Processing", "Adds FLOAT to net x-positions");

    oc.doRegister("y-offset-to-apply", new Option_Float(0));
    oc.addDescription("y-offset-to-apply", "Processing", "Adds FLOAT to net y-positions");

    oc.doRegister("rotation-to-apply", new Option_Float(0));
    oc.addDescription("rotation-to-apply", "Processing", "Rotates net around FLOAT degrees");

    oc.doRegister("guess-roundabouts", new Option_Bool(false));
    oc.addDescription("guess-roundabouts", "Processing", "Enable roundabout-guessing");


    // tls setting options
    // explicite tls
    oc.doRegister("explicite-tls", new Option_String());
    oc.addDescription("explicite-tls", "TLS Building", "Interprets STR as list of junctions to be controlled by TLS");

    oc.doRegister("explicite-no-tls", new Option_String());
    oc.addDescription("explicite-no-tls", "TLS Building", "Interprets STR as list of junctions to be not controlled by TLS");

    // tls-guessing
    oc.doRegister("guess-tls", new Option_Bool(false));
    oc.addDescription("guess-tls", "TLS Building", "Turns on TLS guessing");

    oc.doRegister("tls-guess.district-nodes", new Option_Bool(false));
    oc.addDescription("tls-guess.district-nodes", "TLS Building", "Sets district nodes as tls-controlled"); // !!! describe

    oc.doRegister("tls-guess.joining", new Option_Bool(false));
    oc.addDescription("tls-guess.joining", "TLS Building", "Includes node clusters into guess"); // !!! describe

    oc.doRegister("try-join-tls", new Option_Bool(false));
    oc.addDescription("try-join-tls", "TLS Building", "Tries to cluster tls-controlled nodes"); // !!! describe

    // computational
    oc.doRegister("min-decel", 'D', new Option_Float(3.0));
    oc.addDescription("min-decel", "TLS Building", "Defines smallest vehicle deceleration");

    oc.doRegister("patch-small-tyellow", new Option_Bool(false));
    oc.addDescription("patch-small-tyellow", "TLS Building", "Given yellow times are patched even if being too short");

    oc.doRegister("traffic-light-green", new Option_Integer());
    oc.addDescription("traffic-light-green", "TLS Building", "Use INT as green phase duration");

    oc.doRegister("traffic-light-yellow", new Option_Integer());
    oc.addDescription("traffic-light-yellow", "TLS Building", "Set INT as fixed time for yellow phase durations");

    // tls-shifts
    oc.doRegister("tl-logics.half-offset", new Option_String());
    oc.addDescription("tl-logics.half-offset", "TLS Building", "TLSs in STR will be shifted by half-phase");

    oc.doRegister("tl-logics.quarter-offset", new Option_String());
    oc.addDescription("tl-logics.quarter-offset", "TLS Building", "TLSs in STR will be shifted by quarter-phase");


    // edge constraints
    oc.doRegister("edges-min-speed", new Option_Float());
    oc.addDescription("edges-min-speed", "Edge Removal", "Remove edges with speed < FLOAT");

    oc.doRegister("remove-edges", new Option_String());
    oc.addDescription("remove-edges", "Edge Removal", "Remove edges in STR");

    oc.doRegister("keep-edges", new Option_String());
    oc.addDescription("keep-edges", "Edge Removal", "Remove edges not in STR");

    oc.doRegister("keep-edges.input-file", new Option_FileName());
    oc.addDescription("keep-edges.input-file", "Edge Removal", "Removed edges not in FILE");

    oc.doRegister("keep-edges.postload", new Option_Bool(false));
    oc.addDescription("keep-edges.postload", "Edge Removal", "Remove edges after joining");

    oc.doRegister("keep-edges.in-boundary", new Option_String());
    oc.addDescription("keep-edges.in-boundary", "Edge Removal", "Keeps edges which are located within the given boundary");

    oc.doRegister("remove-edges.by-vclass", new Option_String());
    oc.addDescription("remove-edges.by-vclass", "Edge Removal", "Remove edges where vclass def is not in STR");


    // unregulated nodes options
    oc.doRegister("keep-unregulated", new Option_Bool(false));
    oc.addDescription("keep-unregulated", "Unregulated Nodes", "All nodes will be not regulated");

    oc.doRegister("keep-unregulated.nodes", new Option_String());
    oc.addDescription("keep-unregulated.nodes", "Unregulated Nodes", "Do not regulate nodes in STR");

    oc.doRegister("keep-unregulated.district-nodes", new Option_Bool(false));
    oc.addDescription("keep-unregulated.district-nodes", "Unregulated Nodes", "Do not regulate district nodes");


    // ramp guessing options
    oc.doRegister("guess-ramps", new Option_Bool(false));
    oc.addDescription("guess-ramps", "Ramp Guessing", "Enable ramp-guessing");

    oc.doRegister("ramp-guess.max-ramp-speed", new Option_Float(-1));
    oc.addDescription("ramp-guess.max-ramp-speed", "Ramp Guessing", "Treat edges with speed > FLOAT as no ramps");

    oc.doRegister("ramp-guess.min-highway-speed", new Option_Float((SUMOReal)(79/3.6)));
    oc.addDescription("ramp-guess.min-highway-speed", "Ramp Guessing", "Treat edges with speed < FLOAT as no highways");

    oc.doRegister("ramp-guess.ramp-length", new Option_Float(100));
    oc.addDescription("ramp-guess.ramp-length", "Ramp Guessing", "Use FLOAT as ramp-length");

    oc.doRegister("guess-obscure-ramps", new Option_Bool(false)); // !!! not described
    oc.addDescription("guess-obscure-ramps", "Ramp Guessing", "");

    oc.doRegister("obscure-ramps.add-ramp", new Option_Bool(false)); // !!! not described
    oc.addDescription("obscure-ramps.add-ramp", "Ramp Guessing", "");

    oc.doRegister("obscure-ramps.min-highway-speed", new Option_Float((SUMOReal)(100/3.6)));  // !!! not described
    oc.addDescription("obscure-ramps.min-highway-speed", "Ramp Guessing", "");


}

/****************************************************************************/
