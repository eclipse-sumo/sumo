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
        oc.set("keep-edges.explicit", oss.str());
    }
    // apply options to type control
    myTypeCont.setDefaults(oc.getInt("default.lanenumber"), oc.getFloat("default.speed"), oc.getInt("default.priority"));
    // apply options to edge control
    myEdgeCont.applyOptions(oc);
    // apply options to traffic light logics control
    myTLLCont.applyOptions(oc);
}


void
NBNetBuilder::compute(OptionsCont &oc) {
    int step = 1;
    //
    if (oc.getBool("junctions.join")) {
        myNodeCont.joinJunctions(oc.getFloat("junctions.join-dist"), myDistrictCont, myEdgeCont, myTLLCont);
    }
    // Removes edges that are connecting the same node
    inform(step, "Removing dummy edges.");
    myNodeCont.removeDummyEdges(myDistrictCont, myEdgeCont, myTLLCont);
    //
    inform(step, "Joining double connections.");
    myJoinedEdges.init(myEdgeCont);
    myNodeCont.recheckEdges(myDistrictCont, myTLLCont, myEdgeCont);
    //
    if (oc.exists("remove-edges.isolated") && oc.getBool("remove-edges.isolated")) {
        inform(step, "Finding isolated roads.");
        myNodeCont.removeIsolatedRoads(myDistrictCont, myEdgeCont, myTLLCont);
    }
    //
    if (oc.exists("geometry.remove")&&oc.getBool("geometry.remove")) {
        inform(step, "Removing empty nodes and geometry nodes.");
        myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myJoinedEdges, myTLLCont, oc.exists("geometry.remove")&&oc.getBool("geometry.remove"));
    }
    //
    if (oc.exists("keep-edges.postload") && oc.getBool("keep-edges.postload")) {
        if (oc.isSet("keep-edges.explicit")) {
            inform(step, "Removing unwished edges.");
            myEdgeCont.removeUnwishedEdges(myDistrictCont);
        }
    }
    if (oc.isSet("keep-edges.explicit") || oc.isSet("remove-edges.explicit") 
        || (oc.exists("keep-edges.postload") && oc.getBool("keep-edges.postload") )
        || (oc.exists("keep-edges.by-vclass") && oc.isSet("keep-edges.by-vclass") )
        || (oc.exists("remove-edges.by-vclass") && oc.isSet("remove-edges.by-vclass") )
        || oc.isSet("keep-edges.input-file")) {
        inform(step, "Rechecking nodes after edge removal.");
        myNodeCont.removeUnwishedNodes(myDistrictCont, myEdgeCont, myJoinedEdges, myTLLCont, oc.exists("geometry.remove")&&oc.getBool("geometry.remove"));
    }
    //
    if (oc.exists("geometry.split") && oc.getBool("geometry.split")) {
        inform(step, "Splitting geometry edges.");
        myEdgeCont.splitGeometry(myNodeCont);
    }
    //
    if (!oc.getBool("offset.disable-normalization") && oc.isDefault("offset.x") && oc.isDefault("offset.y")) {
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
    if ((oc.exists("ramps.guess")&&oc.getBool("ramps.guess"))||(oc.exists("ramps.set")&&oc.isSet("ramps.set"))) {
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
    if (oc.getBool("roundabouts.guess")) {
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
        myEdgeCont.appendTurnarounds(oc.getBool("no-turnarounds.tls"));
    }
    //
    inform(step, "Rechecking of lane endings.");
    myEdgeCont.recheckLanes();
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


/****************************************************************************/
