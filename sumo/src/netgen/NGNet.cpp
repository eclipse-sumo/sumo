/****************************************************************************/
/// @file    NGNet.cpp
/// @author  Markus Hartinger
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mar, 2003
/// @version $Id$
///
// The class storing the generated network
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

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/options/OptionsCont.h>
#include "NGNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NGNet::NGNet(NBNetBuilder& nb)
    : myNetBuilder(nb) {
    myLastID = 0;
}


NGNet::~NGNet() {
    for (NGEdgeList::iterator ni = myEdgeList.begin(); ni != myEdgeList.end(); ++ni) {
        delete *ni;
    }
    for (NGNodeList::iterator ni = myNodeList.begin(); ni != myNodeList.end(); ++ni) {
        delete *ni;
    }
}


std::string
NGNet::getNextFreeID() {
    return toString<int>(++myLastID);
}


NGNode*
NGNet::findNode(int xID, int yID) {
    for (NGNodeList::iterator ni = myNodeList.begin(); ni != myNodeList.end(); ++ni) {
        if ((*ni)->samePos(xID, yID)) {
            return *ni;
        }
    }
    return 0;
}


void
NGNet::createChequerBoard(int numX, int numY, SUMOReal spaceX, SUMOReal spaceY, SUMOReal attachLength) {
    for (int ix = 0; ix < numX; ix++) {
        for (int iy = 0; iy < numY; iy++) {
            // create Node
            std::string nodeID = toString<int>(ix) + "/" + toString<int>(iy);
            NGNode* node = new NGNode(nodeID, ix, iy);
            node->setX(ix * spaceX + attachLength);
            node->setY(iy * spaceY + attachLength);
            myNodeList.push_back(node);
            // create Links
            if (ix > 0) {
                connect(node, findNode(ix - 1, iy));
            }
            if (iy > 0) {
                connect(node, findNode(ix, iy - 1));
            }
        }
    }
    if (attachLength > 0.0) {
        for (int ix = 0; ix < numX; ix++) {
            // create nodes
            NGNode* topNode = new NGNode("top" + toString<int>(ix), ix, numY);
            NGNode* bottomNode = new NGNode("bottom" + toString<int>(ix), ix, numY + 1);
            topNode->setX(ix * spaceX + attachLength);
            bottomNode->setX(ix * spaceX + attachLength);
            topNode->setY((numY - 1) * spaceY + 2 * attachLength);
            bottomNode->setY(0);
            myNodeList.push_back(topNode);
            myNodeList.push_back(bottomNode);
            // create links
            connect(topNode, findNode(ix, numY - 1));
            connect(bottomNode, findNode(ix, 0));
        }
        for (int iy = 0; iy < numY; iy++) {
            // create nodes
            NGNode* leftNode = new NGNode("left" + toString<int>(iy), numX, iy);
            NGNode* rightNode = new NGNode("right" + toString<int>(iy), numX + 1, iy);
            leftNode->setX(0);
            rightNode->setX((numX - 1) * spaceX + 2 * attachLength);
            leftNode->setY(iy * spaceY + attachLength);
            rightNode->setY(iy * spaceY + attachLength);
            myNodeList.push_back(leftNode);
            myNodeList.push_back(rightNode);
            // create links
            connect(leftNode, findNode(0, iy));
            connect(rightNode, findNode(numX - 1, iy));
        }
    }
}


SUMOReal
NGNet::radialToX(SUMOReal radius, SUMOReal phi) {
    return cos(phi) * radius;
}


SUMOReal
NGNet::radialToY(SUMOReal radius, SUMOReal phi) {
    return sin(phi) * radius;
}


void
NGNet::createSpiderWeb(int numRadDiv, int numCircles, SUMOReal spaceRad, bool hasCenter) {
    if (numRadDiv < 3) {
        numRadDiv = 3;
    }
    if (numCircles < 1) {
        numCircles = 1;
    }

    int ir, ic;
    SUMOReal angle = (SUMOReal)(2 * M_PI / numRadDiv); // angle between radial divisions
    NGNode* Node;
    for (ir = 1; ir < numRadDiv + 1; ir++) {
        for (ic = 1; ic < numCircles + 1; ic++) {
            // create Node
            Node = new NGNode(
                toString<int>(ir) + "/" + toString<int>(ic), ir, ic);
            Node->setX(radialToX((ic) * spaceRad, (ir - 1) * angle));
            Node->setY(radialToY((ic) * spaceRad, (ir - 1) * angle));
            myNodeList.push_back(Node);
            // create Links
            if (ir > 1) {
                connect(Node, findNode(ir - 1, ic));
            }
            if (ic > 1) {
                connect(Node, findNode(ir, ic - 1));
            }
            if (ir == numRadDiv) {
                connect(Node, findNode(1, ic));
            }
        }
    }
    if (hasCenter) {
        // node
        Node = new NGNode(getNextFreeID(), 0, 0, true);
        Node->setX(0);
        Node->setY(0);
        myNodeList.push_back(Node);
        // links
        for (ir = 1; ir < numRadDiv + 1; ir++) {
            connect(Node, findNode(ir, 1));
        }
    }
}


void
NGNet::connect(NGNode* node1, NGNode* node2) {
    std::string id1 = node1->getID() + "to" + node2->getID();
    std::string id2 = node2->getID() + "to" + node1->getID();
    NGEdge* link1 = new NGEdge(id1, node1, node2);
    NGEdge* link2 = new NGEdge(id2, node2, node1);
    myEdgeList.push_back(link1);
    myEdgeList.push_back(link2);
}


void
NGNet::toNB() const {
    std::vector<NBNode*> nodes;
    for (NGNodeList::const_iterator i1 = myNodeList.begin(); i1 != myNodeList.end(); i1++) {
        NBNode* node = (*i1)->buildNBNode(myNetBuilder);
        nodes.push_back(node);
        myNetBuilder.getNodeCont().insert(node);
    }
    for (NGEdgeList::const_iterator i2 = myEdgeList.begin(); i2 != myEdgeList.end(); i2++) {
        NBEdge* edge = (*i2)->buildNBEdge(myNetBuilder);
        myNetBuilder.getEdgeCont().insert(edge);
    }
    // now, let's append the reverse directions...
    SUMOReal bidiProb = OptionsCont::getOptions().getFloat("rand.bidi-probability");
    for (std::vector<NBNode*>::const_iterator i = nodes.begin(); i != nodes.end(); ++i) {
        NBNode* node = *i;
        EdgeVector incoming = node->getIncomingEdges();
        for (EdgeVector::const_iterator j = incoming.begin(); j != incoming.end(); ++j) {
            if (node->getConnectionTo((*j)->getFromNode()) == 0 && RandHelper::rand() <= bidiProb) {
                NBEdge* back = new NBEdge("-" + (*j)->getID(), node, (*j)->getFromNode(),
                                          "", myNetBuilder.getTypeCont().getSpeed(""), myNetBuilder.getTypeCont().getNumLanes(""),
                                          myNetBuilder.getTypeCont().getPriority(""),
                                          myNetBuilder.getTypeCont().getWidth(""), NBEdge::UNSPECIFIED_OFFSET);
                myNetBuilder.getEdgeCont().insert(back);
            }
        }
    }
}


void
NGNet::add(NGNode* node) {
    myNodeList.push_back(node);
}


void
NGNet::add(NGEdge* edge) {
    myEdgeList.push_back(edge);
}


size_t
NGNet::nodeNo() const {
    return myNodeList.size();
}


/****************************************************************************/

