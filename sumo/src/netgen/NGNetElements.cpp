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
// Revision 1.4  2003/07/22 15:09:50  dkrajzew
// errors on building and warnings removed
//
// Revision 1.3  2003/07/21 11:05:31  dkrajzew
// patched some bugs found in first real-life execution
//
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
#include <algorithm>
#include <netbuild/NBNode.h>
#include <netbuild/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/convert/ToString.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include "NGNetElements.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
TNode::TNode()
    : myID(""), xID(-1), yID(-1), myAmCenter(false)
{
}


TNode::TNode(const std::string &id)
    : myID(id), xID(-1), yID(-1), myAmCenter(false)
{
}


TNode::TNode(const std::string &id, int xIDa, int yIDa)
    : myID(id), xID(xIDa), yID(yIDa), myAmCenter(false)
{
}


TNode::TNode(const std::string &id, int xIDa, int yIDa, bool amCenter)
    : myID(id), xID(xIDa), yID(yIDa), myAmCenter(amCenter)
{
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


NBNode *
TNode::buildNBNode() const
{
    // the center will have no logic!
    if( myAmCenter ) {
        return new NBNode(myID, (double) myX, (double) myY,
            NBNode::NODETYPE_NOJUNCTION);
    }
    //
    NBNode *node = new NBNode(myID, (double) myX, (double) myY);
    // check whether it is a traffic light junction
    string nodeType = OptionsSubSys::getOptions().getString("j");
    if(nodeType!="traffic_light") {
        return node;
    }
    // this traffic light is visited the first time
    NBTrafficLightDefinition *tlDef =
        new NBOwnTLDef(myID, node);
    if(!NBTrafficLightLogicCont::insert(myID, tlDef)) {
        // actually, nothing should fail here
        delete tlDef;
        throw ProcessError();
    }
    return node;
}


void
TNode::addLink(TLink *link)
{
    LinkList.push_back(link);
}


void
TNode::removeLink(TLink *link)
{
    LinkList.remove(link);
}


bool
TNode::connected(TNode *node) const
{
    for(TLinkList::const_iterator i=LinkList.begin(); i!=LinkList.end(); i++) {
        if(find(node->LinkList.begin(), node->LinkList.end(), *i)!=node->LinkList.end()) {
            return true;
        }
    }
    return false;
}


//----------------------------------------- TLink ------------------------------------
// constructors TLink
TLink::TLink()
    : myID()
{
}


TLink::TLink(const std::string &id)
    : myID(id)
{
}


TLink::TLink(const std::string &id, TNode *StartNode, TNode *EndNode)
    : myID(id), myStartNode(StartNode), myEndNode(EndNode)
{
	myStartNode->addLink(this);
	myEndNode->addLink(this);
}


// destructor TLink
TLink::~TLink()
{
	myStartNode->removeLink(this);
	myEndNode->removeLink(this);
}


NBEdge *
TLink::buildNBEdge() const
{
    return new NBEdge(
        myID, // id
        myID, // name
        NBNodeCont::retrieve(myStartNode->GetID()), // from
        NBNodeCont::retrieve(myEndNode->GetID()), // to
        "netgen-default", // type
        13.9, 1, -1, 0 // speed, lane number, length, priority
        );
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "NGNetElements.icc"
//#endif

// Local Variables:
// mode:C++
// End:

