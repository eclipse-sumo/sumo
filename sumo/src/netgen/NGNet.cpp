/***************************************************************************
                          NGNet.cpp
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
// Revision 1.5  2004/01/12 15:27:30  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.4  2003/07/30 09:22:31  dkrajzew
// chequered board generation verified and eased
//
// Revision 1.3  2003/07/22 15:09:50  dkrajzew
// errors on building and warnings removed
//
// Revision 1.2  2003/07/21 11:05:30  dkrajzew
// patched some bugs found in first real-life execution
//
// Revision 1.1  2003/07/16 15:33:08  dkrajzew
// files needed to generate networks added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <utils/convert/ToString.h>
#include "NGNet.h"


/* =========================================================================
 * some definitions
 * ======================================================================= */
#define LENGTH 80


/* =========================================================================
 * used namespace
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
TNGNet::TNGNet()
{
	myLastID = 0;
}


TNGNet::~TNGNet()
{
}


std::string
TNGNet::GetID()
{
	return toString<int>(++myLastID);
}


void
TNGNet::SaveNet(char* FileName)
{/*
	FILE* File;
	// --------- nodes ---------------------------------------------------------
	char NodeName[LENGTH];
	strcpy(NodeName, FileName);
	strcat(NodeName, "_nodes.txt");
	File = fopen(NodeName, "w");
	// header
	fprintf(File,"<nodes>\n");
	// node
	TNodeList::iterator ni;
    for (ni = NodeList.begin(); ni != NodeList.end(); ++ni) {
		fprintf(File, "%s%d%s", "   <node id=\"", (*ni)->GetID().c_str(), "\" ");
		fprintf(File, "%s%f%s%f%s%", (const char*) "x=\"", (*ni)->x(), (const char *)"\" y=\"", (*ni)->y(), "\" ");
		fprintf(File, "%s\n", "type=\"priority\"/>");
	}
	// end
	fprintf(File, "</nodes>\n\n");
    fclose(File);

	// ---------- links -------------------------------------------------------
	char LinkName[LENGTH];
	strcpy(LinkName, FileName);
	strcat(LinkName, "_links.txt");
	File = fopen(LinkName, "w");
	fprintf(File,"<edges>\n");
	// node
	TLinkList::iterator li;
    for (li = LinkList.begin(); li != LinkList.end(); ++li) {
//   <edge id="1i" fromnode="1" tonode="0" type="25"/>
   		fprintf(File, "%s%d%s", "   <edge id=\"", (*li)->GetID(), "\" ");
		fprintf(File, "%s%d%s%d%s", "fromnode=\"", (*li)->StartNode()->GetID(), "\" tonode=\"", (*li)->EndNode()->GetID(), "\" ");
		fprintf(File, "%s\n", "type=\"25\"/>");
	}
	// end
	fprintf(File, "</edges>\n\n");
	fclose(File);*/
}


TNode*
TNGNet::FindNode(int xID, int yID)
{
	TNodeList::iterator ni;
    ni = NodeList.begin();
	while ((((*ni)->xID != xID) || ((*ni)->yID != yID)) && (ni != NodeList.end())) {
		ni++;
	}
	if (((*ni)->xID == xID) && ((*ni)->yID == yID))
		return *ni;
	else
		return 0;
}


void
TNGNet::CreateChequerBoard(int NumX, int NumY, float SpaceX, float SpaceY)
{
	int ix, iy;
	TNode *Node;
    TLink *Link;
	for (ix=0; ix<NumX; ix++) {
		for (iy=0; iy<NumY; iy++) {
			// create Node
            string nodeID = toString<int>(ix) + "/" + toString<int>(iy);
			Node = new TNode(nodeID, ix, iy);
			Node->SetX(ix * SpaceX);
			Node->SetY(iy * SpaceY);
			NodeList.push_back(Node);
			// create Links
			if (ix > 0) {
                connect(Node, FindNode(ix-1, iy));
/*				Link = new TLink(GetID(), Node, FindNode(ix-1, iy));
				LinkList.push_back(Link);*/
			}
			if (iy > 0) {
                connect(Node, FindNode(ix, iy-1));
/*				Link = new TLink(GetID(), Node, FindNode(ix, iy-1));
				LinkList.push_back(Link);*/
			}
		}
	}
}


float
TNGNet::RadialToX(float radius, float phi) {
	return cos(phi) * radius;
}


float
TNGNet::RadialToY(float radius, float phi) {
	return sin(phi) * radius;
}


void
TNGNet::CreateSpiderWeb(int NumRadDiv, int NumCircles, float SpaceRad)
{
	if (NumRadDiv < 3) NumRadDiv = 3;
	if (NumCircles < 1) NumCircles = 1;

	int ir, ic;
	float angle=(2*PI/NumRadDiv);  // angle between radial divisions
	TNode *Node;
    TLink *Link;
	for (ir=1; ir<NumRadDiv+1; ir++) {
		for (ic=1; ic<NumCircles+1; ic++) {
			// create Node
			Node = new TNode(
                toString<int>(ir) + "/" + toString<int>(ic), ir, ic);
			Node->SetX(RadialToX((ic) * SpaceRad, (ir-1) * angle));
			Node->SetY(RadialToY((ic) * SpaceRad, (ir-1) * angle));
			NodeList.push_back(Node);
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
	NodeList.push_back(Node);
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
    LinkList.push_back(link1);
    LinkList.push_back(link2);
}

void
TNGNet::toNB() const
{
    for(TNodeList::const_iterator i1=NodeList.begin(); i1!=NodeList.end(); i1++) {
        NBNode *node = (*i1)->buildNBNode();
        NBNodeCont::insert(node);
    }
    for(TLinkList::const_iterator i2=LinkList.begin(); i2!=LinkList.end(); i2++) {
        NBEdge *edge = (*i2)->buildNBEdge();
        NBEdgeCont::insert(edge);
    }
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "NGNet.icc"
//#endif

// Local Variables:
// mode:C++
// End:

