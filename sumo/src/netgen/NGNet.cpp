/****************************************************************************/
/// @file    NGNet.cpp
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id$
///
// The class storing the generated network
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/ToString.h>
#include "NGNet.h"

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
TNGNet::TNGNet(NBNetBuilder &nb)
        : myNetBuilder(nb)
{
    myLastID = 0;
}


TNGNet::~TNGNet()
{
    {
        for (TLinkList::iterator ni=myLinkList.begin(); ni!=myLinkList.end(); ++ni) {
            delete *ni;
        }
    }
    {
        for (TNodeList::iterator ni=myNodeList.begin(); ni!=myNodeList.end(); ++ni) {
            delete *ni;
        }
    }
}


std::string
TNGNet::GetID()
{
    return toString<int>(++myLastID);
}


TNode*
TNGNet::FindNode(int xID, int yID)
{
    TNodeList::iterator ni = myNodeList.begin();
    while ((((*ni)->xID != xID) || ((*ni)->yID != yID)) && (ni != myNodeList.end())) {
        ni++;
    }
    if (((*ni)->xID == xID) && ((*ni)->yID == yID)) {
        return *ni;
    } else {
        return 0;
    }
}


void
TNGNet::CreateChequerBoard(int NumX, int NumY, SUMOReal SpaceX, SUMOReal SpaceY)
{
    int ix, iy;
    TNode *Node;
    for (ix=0; ix<NumX; ix++) {
        for (iy=0; iy<NumY; iy++) {
            // create Node
            string nodeID = toString<int>(ix) + "/" + toString<int>(iy);
            Node = new TNode(nodeID, ix, iy);
            Node->SetX(ix * SpaceX);
            Node->SetY(iy * SpaceY);
            myNodeList.push_back(Node);
            // create Links
            if (ix > 0) {
                connect(Node, FindNode(ix-1, iy));
            }
            if (iy > 0) {
                connect(Node, FindNode(ix, iy-1));
            }
        }
    }
}


SUMOReal
TNGNet::RadialToX(SUMOReal radius, SUMOReal phi)
{
    return cos(phi) * radius;
}


SUMOReal
TNGNet::RadialToY(SUMOReal radius, SUMOReal phi)
{
    return sin(phi) * radius;
}


void
TNGNet::CreateSpiderWeb(int NumRadDiv, int NumCircles, SUMOReal SpaceRad)
{
    if (NumRadDiv < 3) NumRadDiv = 3;
    if (NumCircles < 1) NumCircles = 1;

    int ir, ic;
    SUMOReal angle = (SUMOReal)(2*PI/NumRadDiv);   // angle between radial divisions
    TNode *Node;
    for (ir=1; ir<NumRadDiv+1; ir++) {
        for (ic=1; ic<NumCircles+1; ic++) {
            // create Node
            Node = new TNode(
                       toString<int>(ir) + "/" + toString<int>(ic), ir, ic);
            Node->SetX(RadialToX((ic) * SpaceRad, (ir-1) * angle));
            Node->SetY(RadialToY((ic) * SpaceRad, (ir-1) * angle));
            myNodeList.push_back(Node);
            // create Links
            if (ir > 1) {
                connect(Node, FindNode(ir-1, ic));
            }
            if (ic > 1) {
                connect(Node, FindNode(ir, ic-1));
            }
            if (ir == NumRadDiv) {
                connect(Node, FindNode(1, ic));
            }
        }
    }
    // create center
    // node
    Node = new TNode(GetID(), 0, 0, true);
    Node->SetX(0);
    Node->SetY(0);
    myNodeList.push_back(Node);
    // links
    for (ir=1; ir<NumRadDiv+1; ir++) {
        connect(Node, FindNode(ir, 1));
    }
}


void
TNGNet::connect(TNode *node1, TNode *node2)
{
    string id1 = node1->GetID() + "to" + node2->GetID();
    string id2 = node2->GetID() + "to" + node1->GetID();
    TLink *link1 = new TLink(id1, node1, node2);
    TLink *link2 = new TLink(id2, node2, node1);
    myLinkList.push_back(link1);
    myLinkList.push_back(link2);
}


void
TNGNet::toNB() const
{
    for (TNodeList::const_iterator i1=myNodeList.begin(); i1!=myNodeList.end(); i1++) {
        NBNode *node = (*i1)->buildNBNode(myNetBuilder);
        myNetBuilder.getNodeCont().insert(node);
    }
    for (TLinkList::const_iterator i2=myLinkList.begin(); i2!=myLinkList.end(); i2++) {
        NBEdge *edge = (*i2)->buildNBEdge(myNetBuilder);
        myNetBuilder.getEdgeCont().insert(edge);
    }
}


void
TNGNet::add(TNode *node)
{
    myNodeList.push_back(node);
}


void
TNGNet::add(TLink *edge)
{
    myLinkList.push_back(edge);
}


size_t
TNGNet::nodeNo() const
{
    return myNodeList.size();
}


/****************************************************************************/

