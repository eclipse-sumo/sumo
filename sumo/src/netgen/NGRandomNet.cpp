/****************************************************************************/
/// @file    NGRandomNet.cpp
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// Additional structures for building random nets
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include "NGRandomNet.h"
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
TNeighbourDistribution::Add(int NumNeighbours, SUMOReal ratio)
{
    Neighbours[NumNeighbours] = ratio;
}


int
TNeighbourDistribution::Num()
{
    SUMOReal sum=0, RandValue;
    TNeighbourList::iterator i;
    // total sum of ratios
    for (i=Neighbours.begin(); i!=Neighbours.end(); ++i)
        sum += (*i).second;
    // RandValue = [0,sum]
    RandValue = randSUMO(sum);
    // find selected item
    i = Neighbours.begin();
    sum = (*i).second;
    while ((i != Neighbours.end()) && (sum < RandValue)) {
        i++;
        sum += (*i).second;
    }
    return (*i).first;
}


// ---------------------------------------------------------------------------
// TNGRandomNet-definitions
// ---------------------------------------------------------------------------
TNGRandomNet::TNGRandomNet(TNGNet *Net)
{
    myNet = Net;
}


void
TNGRandomNet::RemoveOuterNode(TNode *Node)
{
    TNodeList::iterator ni;
    ni = OuterNodes.begin();
    while ((*ni != Node) && (ni != OuterNodes.end())) {
        ni++;
    }
    if (*ni == Node) {
        OuterNodes.erase(ni);
    }
}


void
TNGRandomNet::RemoveOuterLink(TLink *Link)
{
    TLinkList::iterator li;
    li = OuterLinks.begin();
    while ((*li != Link) && (li != OuterLinks.end())) {
        li++;
    }
    if (*li == Link) {
        OuterLinks.erase(li);
    }
}


bool
TNGRandomNet::CheckAngles(TNode *Node)
{
    bool check = true;

    if (Node->LinkList.size() >  1) {
        // loop over all links
        TLinkList::iterator li;
        TNode *ni;
        for (li = Node->LinkList.begin(); li != Node->LinkList.end(); ++li) {
            // calc vector of currentnode
            if ((*li)->StartNode() == Node) {
                ni = (*li)->EndNode();
            } else {
                ni = (*li)->StartNode();
            }
            Position2D v1(
                ni->getPosition().x() - Node->getPosition().x(),
                ni->getPosition().y() - Node->getPosition().y());
            // loop over all links
            TLinkList::iterator lj;
            for (lj = Node->LinkList.begin(); lj != Node->LinkList.end(); ++lj) {
                if (li != lj) {
                    if ((*lj)->StartNode() == Node)
                        ni = (*lj)->EndNode();
                    else
                        ni = (*lj)->StartNode();
                    Position2D v2(
                        ni->getPosition().x() - Node->getPosition().x(),
                        ni->getPosition().y() - Node->getPosition().y());
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
TNGRandomNet::CanConnect(TNode *BaseNode, TNode *NewNode)
{
    bool Connectable=true;
    Position2D n1(BaseNode->getPosition());
    Position2D n2(NewNode->getPosition());

    // check for range between Basenode and Newnode
    if (Connectable) {
        SUMOReal dist = GeomHelper::distance(n1, n2);
        if ((dist < myMinDistance) || (dist > myMaxDistance)) {
            Connectable = false;
        }
    }

    // check for angle restrictions
    if (Connectable) Connectable = CheckAngles(BaseNode);
    if (Connectable) Connectable = CheckAngles(NewNode);

    // check for intersections and range restrictions with outer links
    if (Connectable) {
        TLinkList::iterator li;
        li = OuterLinks.begin();
        while ((Connectable == true) && (li != OuterLinks.end())) {
            // check intersection only if links don't share a node
            Position2D p1((*li)->StartNode()->getPosition());
            Position2D p2((*li)->EndNode()->getPosition());
            if ((BaseNode != (*li)->StartNode()) && (BaseNode!= (*li)->EndNode())
                    && (NewNode != (*li)->StartNode()) && (NewNode!= (*li)->EndNode())) {
                Connectable = !GeomHelper::intersects(n1, n2, p1, p2);

            }
            // check NewNode-To-Links distance only, if NewNode isn't part of link
            if ((Connectable) &&
                    (NewNode != (*li)->StartNode()) && (NewNode != (*li)->EndNode())) {
                SUMOReal dist = GeomHelper::DistancePointLine(n2, p1, p2);
                if ((dist < myMinDistance) && (dist > -1))
                    Connectable = false;
            }
            li++;
        }
    }
    return Connectable;
}


void
TNGRandomNet::FindPossibleOuterNodes(TNode *Node)
{
    ConNodes.clear();
    TNodeList::iterator ni;
    for (ni = OuterNodes.begin(); ni != OuterNodes.end(); ++ni) {
        TNode *on=*ni;
        if (!Node->connected(on))
            if ((Node->MaxNeighbours() > Node->LinkList.size()) &&
                    ((on)->MaxNeighbours() > (on)->LinkList.size()))
                if (CanConnect(Node, on))
                    ConNodes.push_back(on);
    }
}


bool
TNGRandomNet::CreateNewNode(TNode *BaseNode)
{
    // calculate position of new node based on BaseNode
    SUMOReal dist = randSUMO(myMinDistance, myMaxDistance);
    SUMOReal angle = randSUMO((SUMOReal)(2*PI));
    SUMOReal x = BaseNode->getPosition().x() + dist * cos(angle);
    SUMOReal y = BaseNode->getPosition().y() + dist * sin(angle);
    TNode *NewNode = new TNode(myNet->GetID());
    NewNode->SetX(x);
    NewNode->SetY(y);
    NewNode->SetMaxNeighbours((SUMOReal) NeighbourDistribution.Num());
    TLink *NewLink = new TLink(myNet->GetID(), BaseNode, NewNode);
    if (CanConnect(BaseNode, NewNode)) {
        // add node
        myNet->add(NewNode);
        OuterNodes.push_front(NewNode);
        // add link
        myNet->add(NewLink);
        OuterLinks.push_back(NewLink);
        // check basenode for being outer node
        if (BaseNode->LinkList.size() >= BaseNode->MaxNeighbours())
            RemoveOuterNode(BaseNode);
        return true;
    } else {
        delete NewNode;
        return false;
    }
}


void
TNGRandomNet::CreateNet(int NumNodes)
{
    TNode *OuterNode;

    myNumNodes = NumNodes;

    OuterNode = new TNode(myNet->GetID());
    OuterNode->SetX(0);
    OuterNode->SetY(0);
    OuterNode->SetMaxNeighbours(4);

    myNet->add(OuterNode);
    OuterNodes.push_back(OuterNode);

    bool created = true;
    while (((int) myNet->nodeNo() < NumNodes) && (OuterNodes.size() > 0)) {
        // brings last element to front
        if (!created) {
            OuterNodes.push_front(OuterNodes.back());
            OuterNodes.pop_back();
        }
        OuterNode = OuterNodes.back();
        FindPossibleOuterNodes(OuterNode);
        created = false;
        if ((ConNodes.size() > 0) && (randSUMO() < myConnectivity)) {
            TLink *NewLink;
            // create link
            NewLink = new TLink(myNet->GetID(), OuterNode, ConNodes.back());
            if (CanConnect(OuterNode, ConNodes.back())) {
                // add link
                myNet->add(NewLink);
                OuterLinks.push_back(NewLink);
                // check nodes for being outer node
                if (OuterNode->LinkList.size() >= OuterNode->MaxNeighbours())
                    RemoveOuterNode(OuterNode);
                if (ConNodes.back()->LinkList.size() >= ConNodes.back()->MaxNeighbours())
                    RemoveOuterNode(ConNodes.back());
                created = true;
            } else {
                delete NewLink;
            }
        } else {
            int count=0;
            do {
                created = CreateNewNode(OuterNode);
                count++;
            } while ((count <= myNumTries) && !created);
            if (!created) {
                OuterNode->SetMaxNeighbours((SUMOReal) OuterNode->LinkList.size());
                OuterNodes.remove(OuterNode);
            }
        }
    }
}


/****************************************************************************/

