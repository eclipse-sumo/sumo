//---------------------------------------------------------------------------//
//                        NBTrafficLightDefinition.cpp -
//  The definition of a traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.13  2004/11/23 10:21:41  dkrajzew
// debugging
//
// Revision 1.12  2003/12/04 13:03:58  dkrajzew
// possibility to pass the tl-type from the netgenerator added
//
// Revision 1.11  2003/10/30 09:09:55  dkrajzew
// tl-building order patched
//
// Revision 1.10  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams modality cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.8  2003/09/30 14:48:52  dkrajzew
// debug work on vissim-junctions
//
// Revision 1.7  2003/07/30 09:21:11  dkrajzew
// added the generation about link directions and priority
//
// Revision 1.6  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of geometry information
//
// Revision 1.5  2003/06/24 14:35:19  dkrajzew
// unneded debug-prints removed
//
// Revision 1.4  2003/06/24 08:21:01  dkrajzew
// some further work on importing traffic lights
//
// Revision 1.3  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning or an error now; it is reported to a Singleton (MsgHandler); this handler puts it further to output instances. changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.2  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.1  2003/06/05 11:43:20  dkrajzew
// definition class for traffic lights added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include <utils/common/MsgHandler.h>
#include <utils/convert/ToString.h>
#include "NBTrafficLightDefinition.h"
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightDefinition.h"
#include "NBTrafficLightPhases.h"
#include "NBLinkPossibilityMatrix.h"
#include "NBTrafficLightLogic.h"
#include "NBContHelper.h"
#include "NBRequestEdgeLinkIterator.h"
#include "NBLinkCliqueContainer.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * NBTrafficLightDefinition
 * ----------------------------------------------------------------------- */
NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string &id,
                                                   const std::set<NBNode*> &junctions)
    : Named(id), myType("static"), _nodes(junctions)
{
    for(NodeCont::const_iterator i=junctions.begin(); i!=junctions.end(); i++) {
        (*i)->addTrafficLight(this);
    }
}


NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string &id,
                                                   NBNode *junction)
    : Named(id), myType("static")
{
    addNode(junction);
    junction->addTrafficLight(this);
}


NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string &id,
                                                   std::string type,
                                                   NBNode *junction)
    : Named(id), myType(type)
{
    addNode(junction);
    junction->addTrafficLight(this);
}


NBTrafficLightDefinition::NBTrafficLightDefinition(const std::string &id)
    : Named(id), myType("static")
{
}


NBTrafficLightDefinition::~NBTrafficLightDefinition()
{
}


NBTrafficLightLogicVector *
NBTrafficLightDefinition::compute(OptionsCont &oc)
{
    // it is not really a traffic light if no incoming edge exists
    if(_incoming.size()==0) {
        WRITE_WARNING(string("The traffic light '") + getID() +string("' has no incoming edges; it will not be build."));
        return 0;
    }
    // compute the time needed to brake
    size_t breakingTime = computeBrakingTime(oc.getFloat("min-decel"));
    // perform the computation depending on whether the traffic light
    //  definition was loaded or shall be computed new completely
    if(OptionsSubSys::getOptions().isSet("traffic-light-green")) {
        breakingTime = OptionsSubSys::getOptions().getInt("traffic-light-yellow");
    }
    return myCompute(breakingTime, myType, oc.getBool("all-logics"));
}


size_t
NBTrafficLightDefinition::computeBrakingTime(double minDecel) const
{
    double vmax = NBContHelper::maxSpeed(_incoming);
    return (size_t) (vmax / minDecel);
}


void
NBTrafficLightDefinition::setParticipantsInformation()
{
    // collect the infomration about participating edges and links
    collectEdges();
    collectLinks();
}


void
NBTrafficLightDefinition::collectEdges()
{
    EdgeVector myOutgoing;
    // collect the edges from the participating nodes
    for(NodeCont::iterator i=_nodes.begin(); i!=_nodes.end(); i++) {
        const EdgeVector &incoming = (*i)->getIncomingEdges();
        copy(incoming.begin(), incoming.end(), back_inserter(_incoming));
        const EdgeVector &outgoing = (*i)->getOutgoingEdges();
        copy(outgoing.begin(), outgoing.end(), back_inserter(myOutgoing));
    }
    // check which of the edges are completely within the junction
    //  remove these edges from the list of incoming edges
    //  add them to the list of edges lying within the node
    size_t pos = 0;
    while(pos<_incoming.size()) {
        NBEdge *edge = *(_incoming.begin() + pos);
        // an edge lies within the logic if it outgoing as well as incoming
        EdgeVector::iterator j = find(myOutgoing.begin(), myOutgoing.end(), edge);
        if(j!=myOutgoing.end()) {
            _within.push_back(edge);
            _incoming.erase(_incoming.begin() + pos);
        } else {
            pos++;
        }
    }
}


void
NBTrafficLightDefinition::collectLinks()
{
    // build the list of links which are controled by the traffic light
    for(EdgeVector::iterator i=_incoming.begin(); i!=_incoming.end(); i++) {
        NBEdge *incoming = *i;
        size_t noLanes = incoming->getNoLanes();
        for(size_t j=0; j<noLanes; j++) {
            const EdgeLaneVector *connected = incoming->getEdgeLanesFromLane(j);
            for(EdgeLaneVector::const_iterator k=connected->begin(); k!=connected->end(); k++) {
                const EdgeLane &el = *k;
                if(el.edge!=0) {
                    _links.push_back(
                            NBConnection(incoming, j, el.edge, el.lane));
                }
            }
        }
    }
    // set the information about the link's positions within the tl into the
    //  edges the links are starting at, respectively
    size_t pos = 0;
    for(NBConnectionVector::iterator j=_links.begin(); j!=_links.end(); j++) {
        const NBConnection &conn = *j;
        NBEdge *edge = conn.getFrom();
        edge->setControllingTLInformation(
            conn.getFromLane(), conn.getTo(), conn.getToLane(),
            getID(), pos++);
    }
}


pair<size_t, size_t>
NBTrafficLightDefinition::getSizes() const
{
    size_t noLanes = 0;
    size_t noLinks = 0;
    for(EdgeVector::const_iterator i=_incoming.begin(); i!=_incoming.end(); i++) {
        size_t noLanesEdge = (*i)->getNoLanes();
        for(size_t j=0; j<noLanesEdge; j++) {
            assert((*i)->getEdgeLanesFromLane(j)->size()!=0);
            noLinks += (*i)->getEdgeLanesFromLane(j)->size();
        }
        noLanes += noLanesEdge;
    }
    return pair<size_t, size_t>(noLanes, noLinks);
}


bool
NBTrafficLightDefinition::isLeftMover(NBEdge *from, NBEdge *to) const
{
    // the destination edge may be unused
    if(to==0) {
        return false;
    }
    // get the node which is holding this connection
    NodeCont::const_iterator i =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_incoming_finder(from));
    assert(i!=_nodes.end());
    NBNode *node = *i;
    return node->isLeftMover(from, to);
}


bool
NBTrafficLightDefinition::mustBrake(NBEdge *from, NBEdge *to) const
{
    NodeCont::const_iterator i =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_incoming_finder(from));
    assert(i!=_nodes.end());
    NBNode *node = *i;
    if(!node->hasOutgoing(to)) {
        return true; // !!!
    }
    return node->mustBrake(from, to, -1);
}


bool
NBTrafficLightDefinition::mustBrake(NBEdge *from1, NBEdge *to1,
                                    NBEdge *from2, NBEdge *to2,
                                    bool regardNonSignalisedLowerPriority) const
{
    return forbids(from1, to1, from2, to2, regardNonSignalisedLowerPriority);
}


bool
NBTrafficLightDefinition::mustBrake(const NBConnection &possProhibited,
                                    const NBConnection &possProhibitor,
                                    bool regardNonSignalisedLowerPriority) const
{
    return forbids(possProhibitor.getFrom(), possProhibitor.getTo(),
        possProhibited.getFrom(), possProhibited.getTo(),
        regardNonSignalisedLowerPriority);
}


bool
NBTrafficLightDefinition::forbids(NBEdge *possProhibitorFrom,
                                  NBEdge *possProhibitorTo,
                                  NBEdge *possProhibitedFrom,
                                  NBEdge *possProhibitedTo,
                                  bool regardNonSignalisedLowerPriority) const
{
    // retrieve both nodes (it is possible that a connection
    NodeCont::const_iterator incoming =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_incoming_finder(possProhibitorFrom));
    NodeCont::const_iterator outgoing =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_outgoing_finder(possProhibitedTo));
    assert(incoming!=_nodes.end());
    NBNode *incnode = *incoming;
    NBNode *outnode = *outgoing;
    if(incnode!=outnode) {
        return false;
    }
    return incnode->forbids(possProhibitorFrom, possProhibitorTo,
        possProhibitedFrom, possProhibitedTo,
        regardNonSignalisedLowerPriority);
}


bool
NBTrafficLightDefinition::foes(NBEdge *from1, NBEdge *to1,
                                    NBEdge *from2, NBEdge *to2) const
{
    // retrieve both nodes (it is possible that a connection
    NodeCont::const_iterator incoming =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_incoming_finder(from1));
    NodeCont::const_iterator outgoing =
        find_if(_nodes.begin(), _nodes.end(),
            NBContHelper::node_with_outgoing_finder(to1));
    assert(incoming!=_nodes.end());
    NBNode *incnode = *incoming;
    NBNode *outnode = *outgoing;
    if(incnode!=outnode) {
        return false;
    }
    return incnode->foes(from1, to1, from2, to2);
}


void
NBTrafficLightDefinition::addNode(NBNode *node)
{
    _nodes.insert(node);
    node->addTrafficLight(this);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBTrafficLightDefinition.icc"
//#endif

// Local Variables:
// mode:C++
// End:


