/***************************************************************************
                          NBEdgeCont.cpp
			  A container for all of the nets edges
                             -------------------
    project              : SUMO
    subproject           : netbuilder / netconverter
    begin                : Tue, 20 Nov 2001
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
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
// Revision 1.10  2003/04/14 08:34:57  dkrajzew
// some further bugs removed
//
// Revision 1.9  2003/04/01 15:15:51  dkrajzew
// further work on vissim-import
//
// Revision 1.8  2003/03/19 08:03:40  dkrajzew
// splitting of edges made a little bit more stable
//
// Revision 1.7  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.6  2003/03/12 16:47:53  dkrajzew
// extension for artemis-import
//
// Revision 1.5  2003/03/06 17:18:33  dkrajzew
// debugging during vissim implementation
//
// Revision 1.4  2003/03/03 14:59:04  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.3  2003/02/13 15:51:04  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.2  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.6  2002/07/25 08:31:42  dkrajzew
// Report methods transfered from loader to the containers
//
// Revision 1.5  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.1.1.1  2002/04/09 13:22:00  dkrajzew
// new version-free project name
//
// Revision 1.3  2002/04/09 12:21:24  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.2  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:38:00  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <cassert>
#include <algorithm>
#include <iostream>
#include <utils/geom/GeomHelper.h>
#include "NBEdgeCont.h"
#include "NBNodeCont.h"
#include "NBHelpers.h"
#include "NBCont.h"
#include <cmath>
#include "NBTypeCont.h"
#include <iostream>
//#include <strstream>


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static members
 * ======================================================================= */
NBEdgeCont::EdgeCont NBEdgeCont::_edges;


/* =========================================================================
 * method definitions
 * ======================================================================= */
bool
NBEdgeCont::insert(NBEdge *edge)
{
    EdgeCont::iterator i = _edges.find(edge->getID());
    if(i!=_edges.end()) return false;
    _edges.insert(EdgeCont::value_type(edge->getID(), edge));
    return true;
}


NBEdge *
NBEdgeCont::retrieve(const string &id)
{
    EdgeCont::iterator i = _edges.find(id);
    if(i==_edges.end()) return 0;
    return (*i).second;
}


bool
NBEdgeCont::computeTurningDirections(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeTurningDirections();
    }
    return true;
}


bool
NBEdgeCont::sortOutgoingLanesConnections(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->sortOutgoingLanesConnections();
    }
    return true;
}


bool
NBEdgeCont::computeEdge2Edges()
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeEdge2Edges();
    }
    return true;
}


bool
NBEdgeCont::recheckLanes(bool verbose) {
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->recheckLanes(verbose);
    }
    return true;
}


bool
NBEdgeCont::computeLinkPriorities(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->computeLinkPriorities();
    }
    return true;
}


bool
NBEdgeCont::appendTurnarounds(bool verbose)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->appendTurnaround();
    }
    return true;
}


void
NBEdgeCont::writeXMLEdgeList(ostream &into)
{
    into << "   <edges no=\"" << _edges.size() << "\">";
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        if(i!=_edges.begin()) {
            into << ' ';
        }
        into << (*i).first;
    }
    into << "</edges>" << endl;
}


void
NBEdgeCont::writeXMLStep1(std::ostream &into)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep1(into);
    }
    into << endl;
}


void
NBEdgeCont::writeXMLStep2(std::ostream &into)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep2(into);
    }
    into << endl;
}


void
NBEdgeCont::writeXMLStep3(std::ostream &into)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        (*i).second->writeXMLStep3(into);
    }
    into << endl;
}


int NBEdgeCont::size() {
    return _edges.size();
}


int
NBEdgeCont::getNo() {
    return _edges.size();
}


void
NBEdgeCont::clear() {
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++)
        delete((*i).second);
    _edges.clear();
}


void
NBEdgeCont::report(bool verbose)
{
    if(verbose) {
        cout << "   " << getNo() << " edges loaded." << endl;
    }
}


bool
NBEdgeCont::splitAt(NBEdge *edge, NBNode *node)
{
    return splitAt(edge, node,
        edge->getID() + string("[0]"), edge->getID() + string("[1]"),
        edge->_nolanes, edge->_nolanes);
}


bool
NBEdgeCont::splitAt(NBEdge *edge, NBNode *node,
                    const std::string &firstEdgeName,
                    const std::string &secondEdgeName,
                    size_t noLanesFirstEdge, size_t noLanesSecondEdge)
{
    double pos = GeomHelper::nearest_position_on_line_to_point(
        Position2D(edge->_from->getXCoordinate(), edge->_from->getYCoordinate()),
        Position2D(edge->_to->getXCoordinate(), edge->_to->getYCoordinate()),
        Position2D(node->getXCoordinate(), node->getYCoordinate()));
    if(pos<=0) {
        return false;
    }
    return splitAt(edge, pos, node, firstEdgeName, secondEdgeName,
        noLanesFirstEdge, noLanesSecondEdge);
        //!!! does not regard the real edge geometry
}

bool
NBEdgeCont::splitAt(NBEdge *edge, double pos, NBNode *node,
                    const std::string &firstEdgeName,
                    const std::string &secondEdgeName,
                    size_t noLanesFirstEdge, size_t noLanesSecondEdge)
{
    // compute the position to split the edge at
    assert(pos<edge->getLength());
    // build the new edges' geometries
    std::pair<Position2DVector, Position2DVector> geoms =
        edge->getGeometry().splitAt(pos);
    geoms.first.push_back(
        Position2D(node->getXCoordinate(), node->getYCoordinate()));
    geoms.second.push_front(
        Position2D(node->getXCoordinate(), node->getYCoordinate()));
    // build and insert the edges
    NBEdge *one = new NBEdge(firstEdgeName, firstEdgeName,
        edge->_from, node, edge->_type, edge->_speed, noLanesFirstEdge,
        pos, edge->getPriority(), geoms.first, edge->_basicType);
    NBEdge *two = new NBEdge(secondEdgeName, secondEdgeName,
        node, edge->_to, edge->_type, edge->_speed, noLanesSecondEdge,
        edge->_length-pos, edge->getPriority(), geoms.second,
        edge->_basicType);
    // replace information about this edge within the nodes
    edge->_from->replaceOutgoing(edge, one);
    edge->_to->replaceIncoming(edge, two);
    // the edge is now occuring twice in both nodes...
    //  clean up
    edge->_from->removeDoubleEdges();
    edge->_to->removeDoubleEdges();
    // erase the splitted edge
    erase(edge);
    // add connections from the first to the second edge
    size_t noLanes = one->getNoLanes();
    for(size_t i=0; i<noLanes; i++) {
        one->addLane2LaneConnection(i, two, i);
    }
    insert(one);
    insert(two);
    return true;
}






void
NBEdgeCont::erase(NBEdge *edge)
{
    _edges.erase(edge->getID());
    edge->_from->removeOutgoing(edge);
    edge->_to->removeIncoming(edge);
    delete edge;
}


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id,
                                     const std::string &hint,
                                     bool incoming)
{
    // try to retrieve using the given name (iterative)
    NBEdge *edge = retrieve(id);
    if(edge!=0) {
        return edge;
    }
    // now, we did not find it; we have to look over all possibilities
    EdgeVector hints;
        // check whether at least the hint was not splitted
    NBEdge *hintedge = retrieve(hint);
    if(hintedge==0) {
        hints = getGeneratedFrom(hint);
    } else {
        hints.push_back(hintedge);
    }
    EdgeVector candidates = getGeneratedFrom(id);
    for(EdgeVector::iterator i=hints.begin(); i!=hints.end(); i++) {
        NBEdge *hintedge = (*i);
        for(EdgeVector::iterator j=candidates.begin(); j!=candidates.end(); j++) {
            NBEdge *poss_searched = (*j);
            NBNode *node = incoming
                ? poss_searched->_to : poss_searched->_from;
            const EdgeVector *cont = incoming
                ? node->getOutgoingEdges() : node->getIncomingEdges();
            if(find(cont->begin(), cont->end(), hintedge)!=cont->end()) {
                return poss_searched;
            }
        }
    }
    return 0;
}


EdgeVector
NBEdgeCont::getGeneratedFrom(const std::string &id)
{
    size_t len = id.length();
    EdgeVector ret;
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        string curr = (*i).first;
        // the next check makes it possibly faster - we don not have
        //  to compare the names
        if(curr.length()<=len) {
            continue;
        }
        // the name must be the same as the given id but something
        //  beginning with a '[' must be appended to it
        if(curr.substr(0, len)==id&&curr[len]=='[') {
            ret.push_back((*i).second);
        }
    }
    return ret;
}

std::vector<std::string>
NBEdgeCont::buildPossibilities(const std::vector<std::string> &s)
{
    std::vector<std::string> ret;
    for(std::vector<std::string>::const_iterator i=s.begin(); i!=s.end(); i++) {
        ret.push_back((*i) + string("[0]"));
        ret.push_back((*i) + string("[1]"));
    }
    return ret;
}



void
NBEdgeCont::joinSameNodeConnectingEdges(const EdgeVector &edges)
{
    // !!! Attention!
    //  No merging of the geometry to come is being done
    //  The connections are moved from one edge to another within
    //   the replacement where the edge is a node's incoming edge.

    // count the number of lanes, the speed and the id
    size_t nolanes = 0;
    double speed = 0;
    int priority = 0;
    string id;
    NBEdge::EdgeBasicFunction function =
        NBEdge::EDGEFUNCTION_UNKNOWN;
    // retrieve the connected nodes
    NBEdge *tpledge = *(edges.begin());
    NBNode *from = tpledge->getFromNode();
    NBNode *to = tpledge->getToNode();
    EdgeVector::const_iterator i;
    for(i=edges.begin(); i!=edges.end(); i++) {
        // some assertions
        assert((*i)->getFromNode()==from);
        assert((*i)->getToNode()==to);
        // ad the number of lanes the current edge has
        nolanes += (*i)->getNoLanes();
        // build the id
        if(i!=edges.begin()) {
            id += "+";
        }
        id += (*i)->getID();
        // build the edge type
        if(function==NBEdge::EDGEFUNCTION_UNKNOWN) {
            function = (*i)->getBasicType();
        } else {
            if(function!=NBEdge::EDGEFUNCTION_NORMAL) {
                if(function!=(*i)->getBasicType()) {
                    function = NBEdge::EDGEFUNCTION_NORMAL;
                }
            }
        }
        // compute the speed
        speed += (*i)->getSpeed();
        // build the priority
        if(priority<(*i)->getPriority()) {
            priority = (*i)->getPriority();
        }
        // remove all connections to the joined edges
/*
        for(EdgeVector::const_iterator j=edges.begin(); j!=edges.end(); j++) {
            (*i)->removeFromConnections(*j);
        }
        */
    }
    speed /= edges.size();
    // build the new edge
    NBEdge *newEdge = new NBEdge(id, id, from, to, "", speed,
        nolanes, -1, priority, function);
    insert(newEdge);
    // replace old edge by current within the nodes
    //  and delete the old
    from->replaceOutgoing(edges, newEdge);
    to->replaceIncoming(edges, newEdge);
    for(i=edges.begin(); i!=edges.end(); i++) {
        erase(*i);
    }
}


NBEdge *
NBEdgeCont::retrievePossiblySplitted(const std::string &id, double pos)
{
    // check whether the edge was not split, yet
    NBEdge *edge = retrieve(id);
    if(edge!=0) {
        return edge;
    }
    // find the part of the edge which matches the position
    double seen = 0;
    std::vector<string> names;
    names.push_back(id + "[1]");
    names.push_back(id + "[0]");
    while(true) {
        // retrieve the first subelement (to follow)
        string cid = names[names.size()-1];
        names.pop_back();
        edge = retrieve(cid);
        // The edge was splitted; check its subparts within the
        //  next step
        if(edge==0) {
            names.push_back(cid + "[1]");
            names.push_back(cid + "[0]");
        }
        // an edge with the name was found,
        //  check whether the position lies within it
        else {
            seen += edge->getLength();
            if(seen>=pos) {
                return edge;
            }
        }
    }
}


void
NBEdgeCont::search(NBEdge *e)
{
    for(EdgeCont::iterator i=_edges.begin(); i!=_edges.end(); i++) {
        cout << (*i).second << ", " << (*i).second->getID() << endl;
        if((*i).second==e) {
            int checkdummy = 0;
        }
    }
    cout << "--------------------------------" << endl;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBEdgeCont.icc"
//#endif

// Local Variables:
// mode:C++
// End:







