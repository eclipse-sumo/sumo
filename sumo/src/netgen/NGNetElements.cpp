/***************************************************************************
                          NGNetElements.cpp
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
// Revision 1.2  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.1  2003/07/16 15:33:08  dkrajzew
// files needed to generate networks added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include "NGNetElements.h"


/* =========================================================================
 * method definitions
 * ======================================================================= */
TNode::TNode()
{
	myID = -1;
	xID = -1;
	yID = -1;
}


TNode::TNode(int ID)
{
	myID = ID;
	xID = -1;
	yID = -1;
}


TNode::TNode(int ID, int xID, int yID)
{
	myID = ID;
	this->xID = xID;
	this->yID = yID;
}


// destructor TNode
TNode::~TNode()
{
	TLinkList::iterator li;
    //for (li = LinkList.begin(); li != LinkList.end(); ++li) {
	//	delete (*li);
	//}
	while (LinkList.size() != 0) {
		li = LinkList.begin();
		delete (*li);
	}
}


// Remove Link
void
TNode::RemoveLink(TLink *Link)
{
	TLinkList::iterator li;
	li = LinkList.begin();
	while (li!= LinkList.end()) {
		if ((*li) == Link) {
			LinkList.erase(li);
            return;
		} else
			li++;
	}
}


//----------------------------------------- TLink ------------------------------------
// constructors TLink
TLink::TLink()
{
	myID = 0;
}


TLink::TLink(int ID)
{
	myID = ID;
}


TLink::TLink(int ID, TNode *StartNode, TNode *EndNode)
{
	myID = ID;
	myStartNode = StartNode;
	myStartNode->LinkList.push_back(this);
	myEndNode = EndNode;
	myEndNode->LinkList.push_back(this);
}


// destructor TLink
TLink::~TLink()
{
	myStartNode->LinkList.remove(this);
	myEndNode->LinkList.remove(this);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "NGNetElements.icc"
//#endif

// Local Variables:
// mode:C++
// End:

