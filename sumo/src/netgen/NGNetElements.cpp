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
// Revision 1.11  2005/09/23 06:01:31  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.10  2005/09/15 12:03:17  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/04/27 11:48:51  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.8  2004/08/02 12:41:40  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.7  2004/03/19 13:03:52  dkrajzew
// removed some warnings
//
// Revision 1.6  2004/01/12 15:27:30  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.5  2003/12/04 12:51:28  dkrajzew
// documentation added; possibility to use actuated and agentbased junctions added; usage of street types patched
//
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

#include <algorithm>
#include <netbuild/nodes/NBNode.h>
#include <netbuild/nodes/NBNodeCont.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBOwnTLDef.h>
#include <netbuild/NBTypeCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBNetBuilder.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include "NGNetElements.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
TNode::TNode()
    : xID(-1), yID(-1), myID(""), myAmCenter(false)
{
}


TNode::TNode(const std::string &id)
    : xID(-1), yID(-1), myID(id), myAmCenter(false)
{
}


TNode::TNode(const std::string &id, int xIDa, int yIDa)
    : xID(xIDa), yID(yIDa), myID(id), myAmCenter(false)
{
}


TNode::TNode(const std::string &id, int xIDa, int yIDa, bool amCenter)
    : xID(xIDa), yID(yIDa), myID(id), myAmCenter(amCenter)
{
}


// destructor TNode
TNode::~TNode()
{
    TLinkList::iterator li;
    //for (li = LinkList.begin(); li != LinkList.end(); ++li) {
    //  delete (*li);
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
TNode::buildNBNode(NBNetBuilder &nb) const
{
    // the center will have no logic!
    if( myAmCenter ) {
        return new NBNode(myID, myPosition,
            NBNode::NODETYPE_NOJUNCTION);
    }
    //
    NBNode *node = new NBNode(myID, myPosition);
    // check whether it is a traffic light junction
    string nodeType = OptionsSubSys::getOptions().getString("default-junction-type");
    if(nodeType=="priority") {
        return node;
    }
    if(nodeType=="traffic-light") {
        nodeType = "static";
    }
    // this traffic light is visited the first time
    NBTrafficLightDefinition *tlDef =
        new NBOwnTLDef(myID, nodeType, node);
    if(!nb.getTLLogicCont().insert(myID, tlDef)) {
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
TLink::buildNBEdge(NBNetBuilder &nb) const
{
    return new NBEdge(
        myID, // id
        myID, // name
        nb.getNodeCont().retrieve(myStartNode->GetID()), // from
        nb.getNodeCont().retrieve(myEndNode->GetID()), // to
        "netgen-default", // type
        nb.getTypeCont().getDefaultSpeed(),
        nb.getTypeCont().getDefaultNoLanes(),
        -1,
        nb.getTypeCont().getDefaultPriority()
        );
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:

