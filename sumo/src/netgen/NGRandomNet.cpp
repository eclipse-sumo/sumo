/***************************************************************************
                          NGRandomNet.cpp
                             -------------------
    project              : SUMO
    begin                : Mar, 2003
    copyright            : (C) 2003 by DLR/IVF http://ivf.dlr.de/
    author               : Markus Hartinger
    email                : Markus.Hartinger@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.9  2005/09/15 12:03:17  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/04/27 11:48:51  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.7  2004/11/23 10:22:03  dkrajzew
// debugging
//
// Revision 1.6  2004/08/02 12:41:40  dkrajzew
// using Position2D instead of two doubles
//
// Revision 1.5  2004/02/06 08:39:13  dkrajzew
// false inclusion of old header files removed
//
// Revision 1.4  2003/10/31 08:01:49  dkrajzew
// hope to have patched false usage of RAND_MAX when using gcc
//
// Revision 1.3  2003/10/28 08:32:55  dkrajzew
// random number specification option added
//
// Revision 1.2  2003/07/21 11:05:31  dkrajzew
// patched some bugs found in first real-life execution
//
// Revision 1.1  2003/07/16 15:33:08  dkrajzew
// files needed to generate networks added
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <iostream>
#include <math.h>
#include <stdlib.h>
#include "NGRandomNet.h"
#include <utils/geom/GeomHelper.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */
//------------------------------ TNeighbourDistribution ----------------
void
TNeighbourDistribution::Add(int NumNeighbours, float ratio)
{
    Neighbours[NumNeighbours] = ratio;
}


int
TNeighbourDistribution::Num()
{
    double sum=0, RandValue;
    TNeighbourList::iterator i;
    // total sum of ratios
    for (i=Neighbours.begin(); i!=Neighbours.end(); ++i)
        sum += (*i).second;
    // RandValue = [0,sum]
    RandValue = sum * static_cast<double>(rand()) /
        ( static_cast<double>(RAND_MAX) + 1);
    // find selected item
    i = Neighbours.begin();
    sum = (*i).second;
    while ((i != Neighbours.end()) && (sum < RandValue)) {
        i++;
        sum += (*i).second;
    }
//  if ((*i).first != 4)
//      sum=0;
    return (*i).first;
}


//------------------------------ TNGRandomNet ----------------------------
TNGRandomNet::TNGRandomNet(TNGNet *Net)
{
    myNet = Net;
    Nodes = &Net->NodeList;
    Links = &Net->LinkList;
}

void
TNGRandomNet::RemoveOuterNode(TNode *Node)
{
    TNodeList::iterator ni;
    ni = OuterNodes.begin();
    while ((*ni != Node) && (ni != OuterNodes.end())) {
        ni++;
    }
    if (*ni == Node)
        OuterNodes.erase(ni);
}


void
TNGRandomNet::RemoveOuterLink(TLink *Link)
{
    TLinkList::iterator li;
    li = OuterLinks.begin();
    while ((*li != Link) && (li != OuterLinks.end())) {
        li++;
    }
    if (*li == Link)
        OuterLinks.erase(li);
}

/*
bool
TNGRandomNet::NodesConnected(TNode *Node1, TNode *Node2)
{
    bool Connected = false;
    TLinkList::iterator li;
    for (li = Node1->LinkList.begin(); li != Node1->LinkList.end(); ++li) {
        if (( ((*li)->StartNode() == Node1) && ((*li)->EndNode() == Node2) ) ||
            ( ((*li)->StartNode() == Node2) && ((*li)->EndNode() == Node1) )) {
            return true;
        };
    };
    return Connected;
}
*/

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
            if ((*li)->StartNode() == Node)
                ni = (*li)->EndNode();
            else
                ni = (*li)->StartNode();
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
                    double angle = GeomHelper::Angle2D(v1.x(), v1.y(), v2.x(), v2.y());
                    if (fabs((float) angle) < myMinLinkAngle)
                        check = false;
                };
            };
        };
    };
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
        double dist = GeomHelper::distance(n1, n2);
        if ((dist < myMinDistance) || (dist > myMaxDistance))
            Connectable = false;
    };

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

            };
            // check NewNode-To-Links distance only, if NewNode isn't part of link
            if ((Connectable) &&
                (NewNode != (*li)->StartNode()) && (NewNode != (*li)->EndNode())) {
                double dist = GeomHelper::DistancePointLine(n2, p1, p2);
                if (( dist < myMinDistance) && (dist > -1))
                    Connectable = false;
            };
            li++;
        };
    };

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


double
TNGRandomNet::GetAngle()
{
    return 2*PI*rand() /
        ( static_cast<double>(RAND_MAX) + 1);
}


double
TNGRandomNet::GetDistance()
{
    return (myMaxDistance - myMinDistance)*
        static_cast<double>(rand()) / static_cast<double>(RAND_MAX)
        + myMinDistance;
}


bool
TNGRandomNet::UseOuterNode()
{
    double value = rand();
    double max = static_cast<float>(RAND_MAX) + 1;
    value = value/max;
    if ((value) < myConnectivity)
        return true;
    else
        return false;
}


bool
TNGRandomNet::CreateNewNode(TNode *BaseNode)
{
    TNode *NewNode;
    TLink *NewLink;

    // calculate position of new node based on BaseNode
    double x, y, dist, angle;
    dist = GetDistance();
    angle = GetAngle();
    x = BaseNode->getPosition().x() + dist * cos(angle);
    y = BaseNode->getPosition().y() + dist * sin(angle);
    NewNode = new TNode(myNet->GetID());
    NewNode->SetX(x);
    NewNode->SetY(y);
    NewNode->SetMaxNeighbours((float) NeighbourDistribution.Num());
    NewLink = new TLink(myNet->GetID(), BaseNode, NewNode);
    if (CanConnect(BaseNode, NewNode)) {
        // add node
        Nodes->push_back(NewNode);
        OuterNodes.push_front(NewNode);
        // add link
        Links->push_back(NewLink);
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

    Nodes->push_back(OuterNode);
    OuterNodes.push_back(OuterNode);

    bool created = true;
    while (((int) Nodes->size() < NumNodes) && (OuterNodes.size() > 0)) {
        // brings last element to front
        if (!created) {
            OuterNodes.push_front(OuterNodes.back());
            OuterNodes.pop_back();
        }
        OuterNode = OuterNodes.back();
        FindPossibleOuterNodes(OuterNode);
        created = false;
        if ((ConNodes.size() > 0) && UseOuterNode()){
            TLink *NewLink;
            // create link
            NewLink = new TLink(myNet->GetID(), OuterNode, ConNodes.back());
            if (CanConnect(OuterNode, ConNodes.back())) {
                // add link
                Links->push_back(NewLink);
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
                OuterNode->SetMaxNeighbours((float) OuterNode->LinkList.size());
                OuterNodes.remove(OuterNode);
            };
        }
    };
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/


// Local Variables:
// mode:C++
// End:

