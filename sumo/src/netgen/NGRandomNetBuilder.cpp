/****************************************************************************/
/// @file    NGRandomNetBuilder.cpp
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// Additional structures for building random nets
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

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "NGRandomNetBuilder.h"
#include <utils/geom/GeomHelper.h>
#include <utils/common/RandHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// TNeighbourDistribution-definitions
// ---------------------------------------------------------------------------
void
TNeighbourDistribution::add(int NumNeighbours, SUMOReal ratio) throw() {
    myNeighbours[NumNeighbours] = ratio;
}


int
TNeighbourDistribution::num() throw() {
    SUMOReal sum=0, RandValue;
    std::map<int, SUMOReal>::iterator i;
    // total sum of ratios
    for (i=myNeighbours.begin(); i!=myNeighbours.end(); ++i) {
        sum += (*i).second;
    }
    // RandValue = [0,sum]
    RandValue = RandHelper::rand(sum);
    // find selected item
    i = myNeighbours.begin();
    sum = (*i).second;
    while ((i != myNeighbours.end()) && (sum < RandValue)) {
        i++;
        sum += (*i).second;
    }
    return (*i).first;
}


// ---------------------------------------------------------------------------
// NGRandomNetBuilder-definitions
// ---------------------------------------------------------------------------
NGRandomNetBuilder::NGRandomNetBuilder(NGNet &net, SUMOReal minAngle, SUMOReal minDistance,
                                       SUMOReal maxDistance, SUMOReal connectivity,
                                       int numTries, const TNeighbourDistribution &neighborDist) throw()
        : myNet(net), myMinLinkAngle(minAngle), myMinDistance(minDistance),
        myMaxDistance(maxDistance), myConnectivity(connectivity), myNumTries(numTries),
        myNeighbourDistribution(neighborDist) {
}


void
NGRandomNetBuilder::removeOuterNode(NGNode *node) throw() {
    for (NGNodeList::iterator ni=myOuterNodes.begin(); ni!=myOuterNodes.end(); ++ni) {
        if (*ni==node) {
            myOuterNodes.erase(ni);
            return;
        }
    }
}


bool
NGRandomNetBuilder::checkAngles(NGNode *node) throw() {
    bool check = true;

    if (node->LinkList.size() >  1) {
        // loop over all links
        NGEdgeList::iterator li;
        NGNode *ni;
        for (li = node->LinkList.begin(); li != node->LinkList.end(); ++li) {
            // calc vector of currentnode
            if ((*li)->getStartNode() == node) {
                ni = (*li)->getEndNode();
            } else {
                ni = (*li)->getStartNode();
            }
            Position2D v1(
                ni->getPosition().x() - node->getPosition().x(),
                ni->getPosition().y() - node->getPosition().y());
            // loop over all links
            NGEdgeList::iterator lj;
            for (lj = node->LinkList.begin(); lj != node->LinkList.end(); ++lj) {
                if (li != lj) {
                    if ((*lj)->getStartNode() == node)
                        ni = (*lj)->getEndNode();
                    else
                        ni = (*lj)->getStartNode();
                    Position2D v2(
                        ni->getPosition().x() - node->getPosition().x(),
                        ni->getPosition().y() - node->getPosition().y());
                    SUMOReal angle = GeomHelper::Angle2D(v1.x(), v1.y(), v2.x(), v2.y());
                    if (fabs((SUMOReal) angle) < myMinLinkAngle)
                        check = false;
                }
            }
        }
    }
    return check;
}


bool
NGRandomNetBuilder::canConnect(NGNode *baseNode, NGNode *newNode) throw() {
    bool connectable=true;
    Position2D n1(baseNode->getPosition());
    Position2D n2(newNode->getPosition());

    // check for range between Basenode and Newnode
    if (connectable) {
        SUMOReal dist = n1.distanceTo(n2);
        if ((dist < myMinDistance) || (dist > myMaxDistance)) {
            connectable = false;
        }
    }

    // check for angle restrictions
    if (connectable) connectable = checkAngles(baseNode);
    if (connectable) connectable = checkAngles(newNode);

    // check for intersections and range restrictions with outer links
    if (connectable) {
        NGEdgeList::iterator li;
        li = myOuterLinks.begin();
        while ((connectable == true) && (li != myOuterLinks.end())) {
            // check intersection only if links don't share a node
            Position2D p1((*li)->getStartNode()->getPosition());
            Position2D p2((*li)->getEndNode()->getPosition());
            if ((baseNode != (*li)->getStartNode()) && (baseNode!= (*li)->getEndNode())
                    && (newNode != (*li)->getStartNode()) && (newNode!= (*li)->getEndNode())) {
                connectable = !GeomHelper::intersects(n1, n2, p1, p2);

            }
            // check NewNode-To-Links distance only, if NewNode isn't part of link
            if ((connectable) &&
                    (newNode != (*li)->getStartNode()) && (newNode != (*li)->getEndNode())) {
                SUMOReal dist = GeomHelper::distancePointLine(n2, p1, p2);
                if ((dist < myMinDistance) && (dist > -1))
                    connectable = false;
            }
            li++;
        }
    }
    return connectable;
}


void
NGRandomNetBuilder::findPossibleOuterNodes(NGNode *node) throw() {
    myConNodes.clear();
    NGNodeList::iterator ni;
    for (ni = myOuterNodes.begin(); ni != myOuterNodes.end(); ++ni) {
        NGNode *on=*ni;
        if (!node->connected(on)) {
            if ((node->getMaxNeighbours() > node->LinkList.size()) &&
                    ((on)->getMaxNeighbours() > (on)->LinkList.size())) {
                if (canConnect(node, on)) {
                    myConNodes.push_back(on);
                }
            }
        }
    }
}


bool
NGRandomNetBuilder::createNewNode(NGNode *baseNode) throw() {
    // calculate position of new node based on BaseNode
    SUMOReal dist = RandHelper::rand(myMinDistance, myMaxDistance);
    SUMOReal angle = RandHelper::rand((SUMOReal)(2*PI));
    SUMOReal x = baseNode->getPosition().x() + dist * cos(angle);
    SUMOReal y = baseNode->getPosition().y() + dist * sin(angle);
    NGNode *newNode = new NGNode(myNet.getNextFreeID());
    newNode->setX(x);
    newNode->setY(y);
    newNode->setMaxNeighbours((SUMOReal) myNeighbourDistribution.num());
    NGEdge *newLink = new NGEdge(myNet.getNextFreeID(), baseNode, newNode);
    if (canConnect(baseNode, newNode)) {
        // add node
        myNet.add(newNode);
        myOuterNodes.push_front(newNode);
        // add link
        myNet.add(newLink);
        myOuterLinks.push_back(newLink);
        // check basenode for being outer node
        if (baseNode->LinkList.size() >= baseNode->getMaxNeighbours()) {
            removeOuterNode(baseNode);
        }
        return true;
    } else {
        delete newNode;
        return false;
    }
}


void
NGRandomNetBuilder::createNet(int numNodes) throw() {
    myNumNodes = numNodes;

    NGNode *outerNode = new NGNode(myNet.getNextFreeID());
    outerNode->setX(0);
    outerNode->setY(0);
    outerNode->setMaxNeighbours(4);

    myNet.add(outerNode);
    myOuterNodes.push_back(outerNode);

    bool created = true;
    while (((int) myNet.nodeNo() < numNodes) && (myOuterNodes.size() > 0)) {
        // brings last element to front
        if (!created) {
            myOuterNodes.push_front(myOuterNodes.back());
            myOuterNodes.pop_back();
        }
        outerNode = myOuterNodes.back();
        findPossibleOuterNodes(outerNode);
        created = false;
        if ((myConNodes.size() > 0) && (RandHelper::rand() < myConnectivity)) {
            // create link
            NGEdge *newLink = new NGEdge(myNet.getNextFreeID(), outerNode, myConNodes.back());
            if (canConnect(outerNode, myConNodes.back())) {
                // add link
                myNet.add(newLink);
                myOuterLinks.push_back(newLink);
                // check nodes for being outer node
                if (outerNode->LinkList.size() >= outerNode->getMaxNeighbours()) {
                    removeOuterNode(outerNode);
                }
                if (myConNodes.back()->LinkList.size() >= myConNodes.back()->getMaxNeighbours()) {
                    removeOuterNode(myConNodes.back());
                }
                created = true;
            } else {
                delete newLink;
            }
        } else {
            int count=0;
            do {
                created = createNewNode(outerNode);
                count++;
            } while ((count <= myNumTries) && !created);
            if (!created) {
                outerNode->setMaxNeighbours((SUMOReal) outerNode->LinkList.size());
                myOuterNodes.remove(outerNode);
            }
        }
    }
}


/****************************************************************************/

