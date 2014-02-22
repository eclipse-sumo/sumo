/****************************************************************************/
/// @file    NIImporter_RobocupRescue.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in robocup rescue league format
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
#include <string>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/BinaryInputDevice.h>
#include "NILoader.h"
#include "NIImporter_RobocupRescue.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_RobocupRescue::loadNetwork(const OptionsCont& oc, NBNetBuilder& nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("robocup-dir")) {
        return;
    }
    // build the handler
    NIImporter_RobocupRescue handler(nb.getNodeCont(), nb.getEdgeCont());
    // parse file(s)
    std::vector<std::string> files = oc.getStringVector("robocup-dir");
    for (std::vector<std::string>::const_iterator file = files.begin(); file != files.end(); ++file) {
        // nodes
        std::string nodesName = (*file) + "/node.bin";
        if (!FileHelpers::exists(nodesName)) {
            WRITE_ERROR("Could not open robocup-node-file '" + nodesName + "'.");
            return;
        }
        PROGRESS_BEGIN_MESSAGE("Parsing robocup-nodes from '" + nodesName + "'");
        handler.loadNodes(nodesName);
        PROGRESS_DONE_MESSAGE();
        // edges
        std::string edgesName = (*file) + "/road.bin";
        if (!FileHelpers::exists(edgesName)) {
            WRITE_ERROR("Could not open robocup-road-file '" + edgesName + "'.");
            return;
        }
        PROGRESS_BEGIN_MESSAGE("Parsing robocup-roads from '" + edgesName + "'");
        handler.loadEdges(edgesName);
        PROGRESS_DONE_MESSAGE();
    }
}



// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_RobocupRescue::NIImporter_RobocupRescue(NBNodeCont& nc, NBEdgeCont& ec)
    : myNodeCont(nc), myEdgeCont(ec) {}


NIImporter_RobocupRescue::~NIImporter_RobocupRescue() {
}


void
NIImporter_RobocupRescue::loadNodes(const std::string& file) {
    BinaryInputDevice dev(file);
    unsigned int skip;
    dev >> skip; // the number in 19_s
    dev >> skip; // x-offset in 19_s
    dev >> skip; // y-offset in 19_s
    //
    unsigned int noNodes;
    dev >> noNodes;
    WRITE_MESSAGE("Expected node number: " + toString(noNodes));
    do {
        //cout << "  left " << (noNodes) << endl;
        unsigned int entrySize, id, posX, posY, numEdges;
        dev >> entrySize;
        entrySize /= 4;
        dev >> id;
        dev >> posX;
        dev >> posY;
        dev >> numEdges;

        std::vector<int> edges;
        for (unsigned int j = 0; j < numEdges; ++j) {
            unsigned int edge;
            dev >> edge;
            edges.push_back(edge);
        }

        unsigned int signal;
        dev >> signal;

        std::vector<int> turns;
        for (unsigned int j = 0; j < numEdges; ++j) {
            unsigned int turn;
            dev >> turn;
            turns.push_back(turn);
        }

        std::vector<std::pair<int, int> > conns;
        for (unsigned int j = 0; j < numEdges; ++j) {
            unsigned int connF, connT;
            dev >> connF;
            dev >> connT;
            conns.push_back(std::pair<int, int>(connF, connT));
        }

        std::vector<std::vector<int> > times;
        for (unsigned int j = 0; j < numEdges; ++j) {
            unsigned int t1, t2, t3;
            dev >> t1;
            dev >> t2;
            dev >> t3;
            std::vector<int> time;
            time.push_back(t1);
            time.push_back(t2);
            time.push_back(t3);
            times.push_back(time);
        }

        Position pos((SUMOReal)(posX / 1000.), -(SUMOReal)(posY / 1000.));
        NBNetBuilder::transformCoordinates(pos);
        NBNode* node = new NBNode(toString(id), pos);
        myNodeCont.insert(node);
        --noNodes;
    } while (noNodes != 0);
}


void
NIImporter_RobocupRescue::loadEdges(const std::string& file) {
    BinaryInputDevice dev(file);
    unsigned int skip;
    dev >> skip; // the number in 19_s
    dev >> skip; // x-offset in 19_s
    dev >> skip; // y-offset in 19_s
    //
    unsigned int noEdges;
    dev >> noEdges;
    std::cout << "Expected edge number: " << noEdges << std::endl;
    do {
        std::cout << "  left " << (noEdges) << std::endl;
        unsigned int entrySize, id, begNode, endNode, length, roadKind, carsToHead,
                 carsToTail, humansToHead, humansToTail, width, block, repairCost, median,
                 linesToHead, linesToTail, widthForWalkers;
        dev >> entrySize >> id >> begNode >> endNode >> length >> roadKind >> carsToHead
            >> carsToTail >> humansToHead >> humansToTail >> width >> block >> repairCost
            >> median >> linesToHead >> linesToTail >> widthForWalkers;
        NBNode* fromNode = myNodeCont.retrieve(toString(begNode));
        NBNode* toNode = myNodeCont.retrieve(toString(endNode));
        SUMOReal speed = (SUMOReal)(50. / 3.6);
        int priority = -1;
        LaneSpreadFunction spread = linesToHead > 0 && linesToTail > 0 ? LANESPREAD_RIGHT : LANESPREAD_CENTER;
        if (linesToHead > 0) {
            NBEdge* edge = new NBEdge(toString(id), fromNode, toNode, "", speed, linesToHead, priority, NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET, "", spread);
            if (!myEdgeCont.insert(edge)) {
                WRITE_ERROR("Could not insert edge '" + toString(id) + "'");
            }
        }
        if (linesToTail > 0) {
            NBEdge* edge = new NBEdge("-" + toString(id), toNode, fromNode, "", speed, linesToTail, priority, NBEdge::UNSPECIFIED_WIDTH, NBEdge::UNSPECIFIED_OFFSET, "", spread);
            if (!myEdgeCont.insert(edge)) {
                WRITE_ERROR("Could not insert edge '-" + toString(id) + "'");
            }
        }
        --noEdges;
    } while (noEdges != 0);
}


/****************************************************************************/

