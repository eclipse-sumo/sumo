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


int
TNGNet::GetID()
{
	return ++myLastID;
}


void
TNGNet::SaveNet(char* FileName)
{
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
		fprintf(File, "%s%d%s", "   <node id=\"", (*ni)->GetID(), "\" ");
		fprintf(File, "%s%f%s%f%s%", "x=\"", (*ni)->x(), "\" y=\"", (*ni)->y(), "\" ");
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
		fprintf(File, "%s%d%s%d%s%", "fromnode=\"", (*li)->StartNode()->GetID(), "\" tonode=\"", (*li)->EndNode()->GetID(), "\" ");
		fprintf(File, "%s\n", "type=\"25\"/>");
	}
	// end
	fprintf(File, "</edges>\n\n");
	fclose(File);
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
			Node = new TNode(GetID(), ix, iy);
			Node->SetX(ix * SpaceX);
			Node->SetY(iy * SpaceY);
			NodeList.push_back(Node);
			// create Links
			if (ix > 0) {
				Link = new TLink(GetID(), Node, FindNode(ix-1, iy));
				LinkList.push_back(Link);
			}
			if (iy > 0) {
				Link = new TLink(GetID(), Node, FindNode(ix, iy-1));
				LinkList.push_back(Link);
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
			Node = new TNode(GetID(), ir, ic);
			Node->SetX(RadialToX((ic) * SpaceRad, (ir-1) * angle));
			Node->SetY(RadialToY((ic) * SpaceRad, (ir-1) * angle));
			NodeList.push_back(Node);
			// create Links
			if (ir > 1) {
				Link = new TLink(GetID(), Node, FindNode(ir-1, ic));
				LinkList.push_back(Link);
			}
			if (ic > 1) {
				Link = new TLink(GetID(), Node, FindNode(ir, ic-1));
				LinkList.push_back(Link);
			}
			if (ir == NumRadDiv) {
				Link = new TLink(GetID(), Node, FindNode(1, ic));
				LinkList.push_back(Link);
			}
		}
	}
	// create center
	// node
	Node = new TNode(GetID(), 0, 0);
	Node->SetX(0);
	Node->SetY(0);
	NodeList.push_back(Node);
	// links
	for (ir=1; ir<NumRadDiv+1; ir++) {
		Link = new TLink(GetID(), Node, FindNode(ir, 1));
		LinkList.push_back(Link);
	}
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "NGNet.icc"
//#endif

// Local Variables:
// mode:C++
// End:

