/****************************************************************************/
/// @file    NGNetElements.cpp
/// @author  Markus Hartinger
/// @date    Mar, 2003
/// @version $Id: $
///
// -------------------
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


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
TNode::TNode()
        : xID(-1), yID(-1), myID(""), myAmCenter(false)
{}


TNode::TNode(const std::string &id)
        : xID(-1), yID(-1), myID(id), myAmCenter(false)
{}


TNode::TNode(const std::string &id, int xIDa, int yIDa)
        : xID(xIDa), yID(yIDa), myID(id), myAmCenter(false)
{}


TNode::TNode(const std::string &id, int xIDa, int yIDa, bool amCenter)
        : xID(xIDa), yID(yIDa), myID(id), myAmCenter(amCenter)
{}


// destructor TNode
TNode::~TNode()
{
    TLinkList::iterator li;
    while (LinkList.size() != 0) {
        li = LinkList.begin();
        delete(*li);
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
        } else {
            li++;
        }
    }
}


NBNode *
TNode::buildNBNode(NBNetBuilder &nb) const
{
    // the center will have no logic!
    if (myAmCenter) {
        return new NBNode(myID, myPosition,
                          NBNode::NODETYPE_NOJUNCTION);
    }
    //
    NBNode *node = new NBNode(myID, myPosition);
    // check whether it is a traffic light junction
    string nodeType = OptionsSubSys::getOptions().getString("default-junction-type");
    if (nodeType=="priority") {
        return node;
    }
    if (nodeType=="traffic-light") {
        nodeType = "static";
    }
    // this traffic light is visited the first time
    NBTrafficLightDefinition *tlDef =
        new NBOwnTLDef(myID, nodeType, node);
    if (!nb.getTLLogicCont().insert(myID, tlDef)) {
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
    for (TLinkList::const_iterator i=LinkList.begin(); i!=LinkList.end(); i++) {
        if (find(node->LinkList.begin(), node->LinkList.end(), *i)!=node->LinkList.end()) {
            return true;
        }
    }
    return false;
}


//----------------------------------------- TLink ------------------------------------
// constructors TLink
TLink::TLink()
        : myID()
{}


TLink::TLink(const std::string &id)
        : myID(id)
{}


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



/****************************************************************************/

