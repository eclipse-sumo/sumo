/****************************************************************************/
/// @file    NIImporter_RobocupRescue.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 14.04.2008
/// @version $Id$
///
// Importer for networks stored in robocup rescue league format
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
#include "NIImporter_RobocupRescue.h"
#include <utils/geom/GeoConvHelper.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/BinaryInputDevice.h>

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
// ---------------------------------------------------------------------------
// static methods (interface in this case)
// ---------------------------------------------------------------------------
void
NIImporter_RobocupRescue::loadNetwork(const OptionsCont &oc, NBNetBuilder &nb) {
    // check whether the option is set (properly)
    if (!oc.isSet("robocup-net")) {
        return;
    }
    // build the handler
    NIImporter_RobocupRescue handler(nb.getNodeCont(), nb.getEdgeCont());
    // parse file(s)
    vector<string> files = oc.getStringVector("robocup-net");
    for (vector<string>::const_iterator file=files.begin(); file!=files.end(); ++file) {
        // nodes
        string nodesName = (*file) + "/node.bin";
        if (!FileHelpers::exists(nodesName)) {
            MsgHandler::getErrorInstance()->inform("Could not open robocup-node-file '" + nodesName + "'.");
            return;
        }
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing robocup-nodes from '" + nodesName + "'...");
        handler.loadNodes(nodesName);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
        // edges
        string edgesName = (*file) + "/road.bin";
        if (!FileHelpers::exists(edgesName)) {
            MsgHandler::getErrorInstance()->inform("Could not open robocup-road-file '" + edgesName + "'.");
            return;
        }
        MsgHandler::getMessageInstance()->beginProcessMsg("Parsing robocup-roads from '" + edgesName + "'...");
        handler.loadEdges(edgesName);
        MsgHandler::getMessageInstance()->endProcessMsg("done.");
    }
    // build edges
}



// ---------------------------------------------------------------------------
// loader methods
// ---------------------------------------------------------------------------
NIImporter_RobocupRescue::NIImporter_RobocupRescue(NBNodeCont &nc, NBEdgeCont &ec)
        : myNodeCont(nc), myEdgeCont(ec) {}


NIImporter_RobocupRescue::~NIImporter_RobocupRescue() throw() {
}


void
NIImporter_RobocupRescue::loadNodes(const std::string &file) {
    BinaryInputDevice dev(file);
    unsigned int skip;
    dev >> skip; // the number in 19_s
    dev >> skip; // x-offset in 19_s
    dev >> skip; // y-offset in 19_s
    //
    unsigned int noNodes;
    dev >> noNodes;
    MsgHandler::getMessageInstance()->inform("Expected node number: " + toString(noNodes));
    do {
        //cout << "  left " << (noNodes) << endl;
        unsigned int entrySize, id, posX, posY, numEdges;
        dev >> entrySize;
        entrySize /= 4;
        dev >> id;
        dev >> posX;
        dev >> posY;
        dev >> numEdges;

        vector<int> edges;
        for (unsigned int j=0; j<numEdges; ++j) {
            unsigned int edge;
            dev >> edge;
            edges.push_back(edge);
        }

        unsigned int signal;
        dev >> signal;

        vector<int> turns;
        for (unsigned int j=0; j<numEdges; ++j) {
            unsigned int turn;
            dev >> turn;
            turns.push_back(turn);
        }

        vector<pair<int, int> > conns;
        for (unsigned int j=0; j<numEdges; ++j) {
            unsigned int connF, connT;
            dev >> connF;
            dev >> connT;
            conns.push_back(pair<int, int>(connF, connT));
        }

        vector<vector<int> > times;
        for (unsigned int j=0; j<numEdges; ++j) {
            unsigned int t1, t2, t3;
            dev >> t1;
            dev >> t2;
            dev >> t3;
            vector<int> time;
            time.push_back(t1);
            time.push_back(t2);
            time.push_back(t3);
            times.push_back(time);
        }

        Position2D pos((SUMOReal)(posX / 1000.), -(SUMOReal)(posY / 1000.));
        GeoConvHelper::x2cartesian(pos);
        NBNode *node = new NBNode(toString(id), pos);
        myNodeCont.insert(node);
        --noNodes;
    } while (noNodes!=0);
}


void
NIImporter_RobocupRescue::loadEdges(const std::string &file) {
    BinaryInputDevice dev(file);
    unsigned int skip;
    dev >> skip; // the number in 19_s
    dev >> skip; // x-offset in 19_s
    dev >> skip; // y-offset in 19_s
    //
    unsigned int noEdges;
    dev >> noEdges;
    cout << "Expected edge number: " << noEdges << endl;
    do {
        cout << "  left " << (noEdges) << endl;
        unsigned int entrySize, id, begNode, endNode, length, roadKind, carsToHead,
        carsToTail, humansToHead, humansToTail, width, block, repairCost, median,
        linesToHead, linesToTail, widthForWalkers;
        dev >> entrySize >> id >> begNode >> endNode >> length >> roadKind >> carsToHead
        >> carsToTail >> humansToHead >> humansToTail >> width >> block >> repairCost
        >> median >> linesToHead >> linesToTail >> widthForWalkers;
        NBNode *fromNode = myNodeCont.retrieve(toString(begNode));
        NBNode *toNode = myNodeCont.retrieve(toString(endNode));
        SUMOReal speed = (SUMOReal)(50. / 3.6);
        int priority = -1;
        NBEdge::LaneSpreadFunction spread = linesToHead>0&&linesToTail>0 ? NBEdge::LANESPREAD_RIGHT : NBEdge::LANESPREAD_CENTER;
        if(linesToHead>0) {
            NBEdge *edge = new NBEdge(toString(id), fromNode, toNode, "",
                                      speed, linesToHead, priority, spread);
            if(!myEdgeCont.insert(edge)) {
                MsgHandler::getErrorInstance()->inform("Could not insert edge '" + toString(id) + "'");
            }
        }
        if(linesToTail>0) {
            NBEdge *edge = new NBEdge("-" + toString(id), toNode, fromNode, "",
                                      speed, linesToTail, priority, spread);
            if(!myEdgeCont.insert(edge)) {
                MsgHandler::getErrorInstance()->inform("Could not insert edge '-" + toString(id) + "'");
            }
        }
        --noEdges;
    } while (noEdges!=0);
}


/****************************************************************************/

