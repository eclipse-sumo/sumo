/***************************************************************************
                          NBEdge.cpp
			  Methods for the representation of a single egde
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
// Revision 1.12  2003/04/07 12:15:38  dkrajzew
// first steps towards a junctions geometry; tyellow removed again, traffic lights have yellow times given explicitely, now
//
// Revision 1.11  2003/04/04 07:43:03  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge sorting (false lane connections) debugged
//
// Revision 1.10  2003/04/01 15:15:50  dkrajzew
// further work on vissim-import
//
// Revision 1.9  2003/03/26 12:00:08  dkrajzew
// debugging for Vissim and Visum-imports
//
// Revision 1.8  2003/03/17 14:22:33  dkrajzew
// further debug and windows eol removed
//
// Revision 1.7  2003/03/12 16:47:52  dkrajzew
// extension for artemis-import
//
// Revision 1.5  2003/03/03 14:59:01  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.4  2003/02/13 15:51:01  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.3  2003/02/07 10:43:43  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 13:32:01  dkrajzew
// possibility to add connections between lanes added; adding of connectionsbetween edges revalidated
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.10  2002/07/25 08:27:54  dkrajzew
// A strange bug within divideOnEdges is now prefixed
//
// Revision 1.9  2002/07/02 09:00:15  dkrajzew
// Bug on computation of the real turnaround edge fixed
//
// Revision 1.8  2002/06/21 13:17:51  dkrajzew
// Lane division bug (forgetting the turning direction; cause of segmentation violations under Linux) fixed
//
// Revision 1.7  2002/06/18 05:05:08  dkrajzew
// Lane division bug accessing after the end of an array removed
//
// Revision 1.6  2002/06/17 15:19:29  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.3  2002/05/14 04:42:54  dkrajzew
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
// Revision 1.5  2002/04/09 12:21:24  dkrajzew
// Windows-Memoryleak detection changed
//
// Revision 1.4  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.3  2002/03/15 09:14:26  traffic
// Changed the number of lanes field into unsigned
//
// Revision 1.2  2002/03/11 15:22:12  traffic
// Priority definition corrected
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include "NBEdgeCont.h"
#include "NBNode.h"
#include "NBNodeCont.h"
#include "NBContHelper.h"
#include "NBHelpers.h"
#include <cmath>
#include "NBTypeCont.h"
#include <iostream>
#include <utils/geom/GeomHelper.h>
#include "NBEdge.h"


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
 * static variables
 * ======================================================================= */
const int NBEdge::LINKTYPE_INVALID = 0;
const int NBEdge::LINKTYPE_UNPRIORISED = 1;
const int NBEdge::LINKTYPE_PRIORISED = 2;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * NBEdge::ToEdgeConnectionsAdder-methods
 * ----------------------------------------------------------------------- */
void NBEdge::ToEdgeConnectionsAdder::execute(double lane, double virtEdge)
{
    assert(_transitions.size()>(int) virtEdge);
    assert(lane>=0&&lane<10);
    NBEdge *succEdge = _transitions[(int) virtEdge];
    vector<size_t> lanes;
    map<NBEdge*, vector<size_t> >::iterator i=_connections->find(succEdge);
    if(i!=_connections->end()) {
        lanes = (*i).second;
    }
    vector<size_t>::iterator j=find(lanes.begin(), lanes.end(), (size_t) lane);
    if(j==lanes.end()) {
        lanes.push_back((size_t) lane);
    }
    (*_connections)[succEdge] = lanes;
}



/* -------------------------------------------------------------------------
 * NBEdge::MainDirections-methods
 * ----------------------------------------------------------------------- */
NBEdge::MainDirections::MainDirections(const std::vector<NBEdge*> &outgoing,
                                       NBEdge *parent, NBNode *to)
{
    if(outgoing.size()==0)
        return;
    // check whether the right turn has a higher priority
    assert(outgoing.size()>0);
    if(outgoing[0]->getJunctionPriority(to)==1) {
        _dirs.push_back(MainDirections::DIR_RIGHTMOST);
    }
    // check whether the left turn has a higher priority
    if(outgoing[outgoing.size()-1]->getJunctionPriority(to)==1) {
        _dirs.push_back(MainDirections::DIR_LEFTMOST);
    }
    // check whether the forward direction has a higher priority
    //  get the forward direction
    vector<NBEdge*> tmp(outgoing);
    sort(tmp.begin(), tmp.end(),
        NBContHelper::edge_similar_direction_sorter(parent));
    NBEdge *edge = *(tmp.begin());
    // check whether it has a higher priority
    if(edge->getJunctionPriority(to)==1) {
        _dirs.push_back(MainDirections::DIR_FORWARD);
    }
}


NBEdge::MainDirections::~MainDirections()
{
}


bool
NBEdge::MainDirections::empty() const
{
    return _dirs.empty();
}

bool
NBEdge::MainDirections::includes(Direction d) const {
    return find(_dirs.begin(), _dirs.end(), d)!=_dirs.end();
}



/* -------------------------------------------------------------------------
 * NBEdge-methods
 * ----------------------------------------------------------------------- */
NBEdge::NBEdge(string id, string name, NBNode *from, NBNode *to,
               string type, double speed, size_t nolanes,
               double length, int priority, EdgeBasicFunction basic) :
    _step(INIT), _id(id), _type(type), _nolanes(nolanes),
    _from(from), _to(to), _length(length), _angle(0),
    _priority(priority), _speed(speed), _name(name),
    _connectedEdges(0),
    _ToEdges(0), _turnDestination(0),
    _reachable(0),
    _reachablePriorities(0), _succeedinglanes(0),
    _fromJunctionPriority(-1), _toJunctionPriority(-1),
    _basicType(basic)
{
    if(_from==0||_to==0) {
        throw std::exception();
    }
    _angle = NBHelpers::angle(
        _from->getXCoordinate(), _from->getYCoordinate(),
        _to->getXCoordinate(), _to->getYCoordinate()
        );
     _from->addOutgoingEdge(this);
    _to->addIncomingEdge(this);
    // prepare container
    _reachable = new NBEdge::ReachableFromLaneVector();
    _reachable->resize(_nolanes, EdgeLaneVector());
    _reachablePriorities = new NBEdge::ReachablePrioritiesFromLaneVector();
    _reachablePriorities->resize(_nolanes, IntVector());
    _succeedinglanes = new NBEdge::LanesThatSucceedEdgeCont();
    _ToEdges = new map<NBEdge*, vector<size_t> >();
    if(_length<=0) {
        _length = GeomHelper::distance(
            Position2D(_from->getXCoordinate(), _from->getYCoordinate()),
            Position2D(_to->getXCoordinate(), _to->getYCoordinate()));
    }
    if(basic==EDGEFUNCTION_SOURCE&&_length<200) {
        _length = 200;
    }
   assert(_length>0);
    myGeom.push_back(
        Position2D(_from->getXCoordinate(), _from->getYCoordinate()));
    myGeom.push_back(
        Position2D(_to->getXCoordinate(), _to->getYCoordinate()));
    if(_priority<0) {
        _priority = 0;
    }
}


NBEdge::NBEdge(string id, string name, NBNode *from, NBNode *to,
               string type, double speed, size_t nolanes,
               double length, int priority,
               const Position2DVector &geom,
               EdgeBasicFunction basic) :
    _step(INIT), _id(id), _type(type), _nolanes(nolanes),
    _from(from), _to(to), _length(length), _angle(0),
    _priority(priority), _speed(speed), _name(name),
    _connectedEdges(0),
    _ToEdges(0), _turnDestination(0),
    _reachable(0),
    _reachablePriorities(0), _succeedinglanes(0),
    _fromJunctionPriority(-1), _toJunctionPriority(-1),
    _basicType(basic), myGeom(geom)
{
    if(_from==0||_to==0) {
        throw std::exception();
    }
    _angle = NBHelpers::angle(
        _from->getXCoordinate(), _from->getYCoordinate(),
        _to->getXCoordinate(), _to->getYCoordinate()
        );
    _from->addOutgoingEdge(this);
    _to->addIncomingEdge(this);
    // prepare container
    _reachable = new NBEdge::ReachableFromLaneVector();
    _reachable->resize(_nolanes, EdgeLaneVector());
    _reachablePriorities = new NBEdge::ReachablePrioritiesFromLaneVector();
    _reachablePriorities->resize(_nolanes, IntVector());
    _succeedinglanes = new NBEdge::LanesThatSucceedEdgeCont();
    _ToEdges = new map<NBEdge*, vector<size_t> >();
    if(_length<=0) {
        _length = GeomHelper::distance(
            Position2D(_from->getXCoordinate(), _from->getYCoordinate()),
            Position2D(_to->getXCoordinate(), _to->getYCoordinate()));
    }
    if(basic==EDGEFUNCTION_SOURCE&&_length<200) {
        _length = 200;
    }
    assert(_length>0);
    assert(myGeom.size()>=2);
    if(_priority<0) {
        _priority = 0;
    }
}


NBEdge::~NBEdge()
{
    delete _connectedEdges;
    delete _reachable;
    delete _reachablePriorities;
    delete _succeedinglanes;
    delete _ToEdges;
}


double
NBEdge::getAngle()
{
    return _angle;
}


size_t
NBEdge::getNoLanes()
{
    return _nolanes;
}


int
NBEdge::getPriority()
{
    return _priority;
}


int
NBEdge::getJunctionPriority(NBNode *node) {
    if(node==_from) {
        return _fromJunctionPriority;
    } else {
        return _toJunctionPriority;
    }
}


void
NBEdge::setJunctionPriority(NBNode *node, int prio)
{
    if(node==_from) {
        _fromJunctionPriority = prio;
    } else {
        _toJunctionPriority = prio;
    }
}

/*
void
NBEdge::setJunctionAngle(NBNode *node, double angle)
{
    if(node==_from) {
        _fromJunctionAngle = angle;
    } else {
        _toJunctionAngle = angle;
    }
}


double
NBEdge::getJunctionAngle(NBNode *node)
{
    if(node==_from) {
        return _fromJunctionAngle;
    } else {
        return _toJunctionAngle;
    }
}
*/

string
NBEdge::getID()
{
    return _id;
}


string
NBEdge::getName() {
    return _name;
}


NBNode *
NBEdge::getFromNode()
{
    return _from;
}


NBNode *
NBEdge::getToNode()
{
    return _to;
}


string
NBEdge::getType() {
    return _type;
}


double
NBEdge::getLength() {
    return _length;
}


const EdgeLaneVector *
NBEdge::getEdgeLanesFromLane(size_t lane) {
    assert(_reachable!=0&&lane<_reachable->size());
    return &(*_reachable)[lane];
}


void
NBEdge::computeTurningDirections()
{
    EdgeVector *outgoing = _connectedEdges;
    if(outgoing==0) {
        outgoing = _to->getOutgoingEdges();
    }
    for(EdgeVector::iterator i=outgoing->begin(); i!=outgoing->end(); i++) {
        NBEdge *outedge = *i;
        double relAngle = NBHelpers::normRelAngle(_angle, outedge->getAngle());
        // do not append the turnaround
        if(outgoing->size()>1 && relAngle==-180) {
            if(_turnDestination==0) {
                _turnDestination = outedge;
            } else {
                if( fabs(NBHelpers::relAngle(_angle, _turnDestination->getAngle()))
                    <
                    fabs(NBHelpers::relAngle(_angle, outedge->getAngle())) ) {
                    _turnDestination = outedge;
                }
            }
        }
    }
#ifdef CROSS_TEST
    assert(
        (_id=="1si"&&_turnDestination->getID()=="1o") ||
        (_id=="2si"&&_turnDestination->getID()=="2o") ||
        (_id=="3si"&&_turnDestination->getID()=="3o") ||
        (_id=="4si"&&_turnDestination->getID()=="4o") ||
        _id=="1o" || _id=="2o" || _id=="3o" || _id=="4o" ||
        _id=="1fi" || _id=="2fi" || _id=="3fi" || _id=="4fi" );
#endif
}


void
NBEdge::writeXMLStep1(std::ostream &into)
{
    // write the edge's begin
    into << "   <edge id=\"" << _id <<
        "\" Length=\"" << _length <<
        "\" Speed=\"" << _speed <<
        "\" Name=\"" << _name <<
        "\" NoLanes=\"" << _nolanes <<
        "\" XFrom=\"" << _from->getXCoordinate() <<
        "\" YFrom=\"" << _from->getYCoordinate() <<
        "\" XTo=\"" << _to->getXCoordinate() <<
        "\" YTo=\"" << _to->getYCoordinate() <<
        "\" From=\"" << _from->getID() <<
        "\" To=\"" << _to->getID() <<
        "\" Priority=\"" << _priority <<
        "\" Angle=\"" << _angle <<
        "\" Type=\"" << _type <<
        "\" function=\"";
    switch(_basicType) {
    case EDGEFUNCTION_NORMAL:
        into << "normal";
        break;
    case EDGEFUNCTION_SOURCE:
        into << "source";
        break;
    case EDGEFUNCTION_SINK:
        into << "sink";
        break;
    default:
        throw 1;
    }
    into << "\">" << endl;
    // write the lanes
    into << "      <lanes>" << endl;
    for(size_t i=0; i<_nolanes; i++) {
        writeLane(into, i);
    }
    into << "      </lanes>" << endl;
    // write the list of connected edges
    for( LanesThatSucceedEdgeCont::iterator l=_succeedinglanes->begin();
         l!=_succeedinglanes->end(); l++) {
        writeConnected(into, (*l).first, (*l).second);
    }
    // close the edge
    into << "   </edge>" << endl << endl;
}


void
NBEdge::writeXMLStep2(std::ostream &into)
{
    for(size_t i=0; i<_nolanes; i++) {
        writeSucceeding(into, i);
    }
}


void
NBEdge::writeXMLStep3(std::ostream &into)
{
    for(size_t i=0; i<_nolanes; i++) {
        into << "   <edgepos id=\"" << _id << "\""
            << " from=\"" << _from->getID() << "\""
            << " to=\"" << _to->getID() << "\""
            << " lane=\"" << i << "\""
            << " function=\"";
        switch(_basicType) {
        case EDGEFUNCTION_NORMAL:
            into << "normal";
            break;
        case EDGEFUNCTION_SOURCE:
            into << "source";
            break;
        case EDGEFUNCTION_SINK:
            into << "sink";
            break;
        default:
            throw 1;
        }
        into << "\">"
            << myGeom << "</edgepos>" << endl;
    }
}


void
NBEdge::writeLane(std::ostream &into, size_t lane)
{
    // output the lane's attributes
    into << "         <lane id=\"" << _id << '_' << lane << "\"";
    // the first lane of an edge will be the depart lane
    if(lane==0) {
        into << " depart=\"1\"";
    } else {
        into << " depart=\"0\"";
    }
    // some further information
    into << " maxspeed=\"" << _speed << "\" length=\"" << _length <<
        "\" changeurge=\"0\"/>" << endl;
}


void
NBEdge::writeConnected(std::ostream &into, NBEdge *edge, LaneVector &lanes)
{
    if(edge==0) {
        return;
    }
    into << "      <cedge id=\"" << edge->getID() << "\">";
    size_t noApproachers = lanes.size();
    for(size_t i=0; i<noApproachers; i++) {
        assert(i<lanes.size());
        into << _id << '_' << lanes[i];
        if(i<noApproachers-1) {
            into << ' ';
        }
    }
    into << "</cedge>" << endl;
}


void
NBEdge::writeSucceeding(std::ostream &into, size_t lane)
{
    into << "   <succ edge=\"" << _id << "\" lane=\"" << _id << "_"
        << lane << "\" junction=\"" << _to->getID() << "\">" << endl;
    // the lane may be unconnented; output information about being invalid
    assert(_reachable!=0&&lane<_reachable->size());
    size_t noApproached = (*_reachable)[lane].size();
    if(noApproached==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\"/>"
            << endl;
    }
    // output list of connected lanes
    for(size_t j=0; j<noApproached; j++) {
        writeSingleSucceeding(into, lane, j);
    }
    into << "   </succ>" << endl << endl;
}


void
NBEdge::writeSingleSucceeding(std::ostream &into, size_t from, size_t dest)
{
    // check whether the connected lane is invalid
    //  (should not happen; this is an artefact left from previous versions)
    assert(_reachable!=0 && from<_reachable->size() && dest<(*_reachable)[from].size());
    if((*_reachable)[from][dest].edge==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\"/>"
            << endl;
        return;
    }
    // output the id
    into << "      <succlane lane=\""
        << (*_reachable)[from][dest].edge->getID() << '_'
        << (*_reachable)[from][dest].lane << '\"'; // !!! classe LaneEdge mit getLaneID
        // output information whether the connection yields
    if((*_reachablePriorities)[from][dest]==LINKTYPE_UNPRIORISED) {
        into << " yield=\"1\"/>" << endl;
    }
    if((*_reachablePriorities)[from][dest]==LINKTYPE_PRIORISED) {
        into << " yield=\"0\"/>" << endl;
    }
}


bool
NBEdge::addEdge2EdgeConnection(NBEdge *dest)
{
    if(_step==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if(_to!=dest->_from) {
        return false;
    }
    if(_connectedEdges==0) {
        _connectedEdges = new std::vector<NBEdge*>();
    }
    if( find(_connectedEdges->begin(), _connectedEdges->end(), dest)
          ==_connectedEdges->end()) {
        _connectedEdges->push_back(dest);
        _step = EDGE2EDGES;
        assert(_ToEdges!=0);
        (*_ToEdges)[dest] = vector<size_t>();
        return true;
    }
    return false;
}


bool
NBEdge::addLane2LaneConnection(size_t from, NBEdge *dest, size_t toLane)
{
    if(_step==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if(_to!=dest->_from) {
        return false;
    }
    bool ok = addEdge2EdgeConnection(dest);
    setConnection(from, dest, toLane);
    vector<size_t> &lanes = (_ToEdges->find(dest))->second;
    vector<size_t>::iterator i = find(lanes.begin(), lanes.end(), from);
    if(i==lanes.end()) {
        lanes.push_back(from);
    }
    _step = EDGE2LANES;
    return true;
}

bool
NBEdge::computeEdge2Edges()
{
    // return if this relationship has been build in previous steps or
    //  during the import
    if(_step>EDGE2EDGES) {
        return true;
    }
    // get list of possible outgoing edges sorted by direction clockwise
    //  the edge in the backward direction (turnaround) is not in the list
    const vector<NBEdge*> *edges = getConnectedSorted();
    // divide the lanes on reachable edges
    divideOnEdges(edges);
    delete edges;
    delete _connectedEdges;
    _connectedEdges = 0;
    _step = EDGE2EDGES;
    return true;
}


bool
NBEdge::recheckLanes(bool verbose) {
    size_t i;
    // check:
    //  if there is a lane with no connections and any neighbour lane has
    //  more than one connections, try to move one of them
    for(i=0; i<_nolanes; i++) {
        assert(_reachable!=0 && i<_reachable->size());
        if((*_reachable)[i].size()==0) {
            if(i>0&&(*_reachable)[i-1].size()>1) {
                moveConnectionToLeft(i-1);
            } else if(i<_reachable->size()-1&&(*_reachable)[i+1].size()>1) {
                moveConnectionToRight(i+1);
            }
        }
    }
    // check:
    //  go through all lanes and add an empty connection if no connection
    //  is yet set
    for(i=0; i<_nolanes; i++) {
        assert(_reachable!=0 && i<_reachable->size());
        if((*_reachable)[i].size()==0) {
            setConnection(i, 0, 0);
        }
    }
    return true;
}


void
NBEdge::moveConnectionToLeft(size_t lane) {
    assert(_reachable!=0 && lane<_reachable->size() && (*_reachable)[lane].size()>0);
    pair<NBEdge*, size_t> dest =
        getConnectionRemoving(lane, (*_reachable)[lane].size()-1);
    setConnection(lane+1, dest.first, dest.second);
}


void
NBEdge::moveConnectionToRight(size_t lane) {
    pair<NBEdge*, size_t> dest = getConnectionRemoving(lane, 0);
    setConnection(lane-1, dest.first, dest.second);
}


pair<NBEdge*, size_t>
NBEdge::getConnectionRemoving(size_t srcLane, size_t pos) {
    assert(_reachable!=0);
    assert(_reachable->size()>srcLane);
    assert((*_reachable)[srcLane].size()>pos);
    // get the destination edge and remove from the container
    EdgeLane edgelane = (*_reachable)[srcLane][pos];
    (*_reachable)[srcLane].erase((*_reachable)[srcLane].end()-1);
    // remove the information from the map of how to reach edges
    NBEdge::LanesThatSucceedEdgeCont::iterator i =
        _succeedinglanes->find(edgelane.edge);
    LaneVector lanes = (*i).second;
    LaneVector::iterator j = find(lanes.begin(), lanes.end(), srcLane);
    lanes.erase(j);
    (*_succeedinglanes)[edgelane.edge] = lanes;
    // return the information
    return pair<NBEdge*, size_t>(edgelane.edge, edgelane.lane);
}


vector<size_t>
NBEdge::getConnectionLanes(NBEdge *currentOutgoing) {
    if(currentOutgoing==_turnDestination) {
        return vector<size_t>();
    }
    map<NBEdge*, vector<size_t> >::iterator i=_ToEdges->find(currentOutgoing);
    assert(i==_ToEdges->end()||(*i).second.size()<=_nolanes);
    if(i==_ToEdges->end()) {
        return vector<size_t>();
    }
    return (*i).second;
}


const std::vector<NBEdge*> *
NBEdge::getConnectedSorted()
{
    // check whether connections exist and if not, use edges from the node
    EdgeVector *outgoing = _connectedEdges;
    if(outgoing==0) {
        outgoing = _to->getOutgoingEdges();
    }
    // allocate the sorted container
    size_t size = outgoing->size();
    vector<NBEdge*> *edges = new vector<NBEdge*>();
    edges->reserve(size);
    for(EdgeVector::iterator i=outgoing->begin(); i!=outgoing->end(); i++) {
        NBEdge *outedge = *i;
        if(outedge!=_turnDestination) {
            edges->push_back(outedge);
        }
    }
    sort(edges->begin(), edges->end(),
        NBContHelper::relative_edge_sorter(this, _to));
    return edges;
}


void
NBEdge::divideOnEdges(const vector<NBEdge*> *outgoing)
{
    if(outgoing->size()==0) {
        return;
    }
    // precompute priorities; needed as some kind of assumptions for
    //  priorities of directions (see preparePriorities)
    vector<size_t> *priorities = preparePriorities(outgoing);
    //
    // compute the sum of priorities (needed for normalisation)
    size_t prioSum = computePrioritySum(priorities);
    // compute the resulting number of lanes that should be used to
    //  reach the following edge
    size_t size = outgoing->size();
    vector<double> resultingLanes;
    resultingLanes.reserve(size);
    double sumResulting = 0; // the sum of resulting lanes
    double minResulting = 10000; // the least number of lanes to reach an edge
    size_t i;
    for(i=0; i<size; i++) {
        // res will be the number of lanes which are meant to reach the
        //  current outgoing edge
        double res =
            (double) (*priorities)[i] *
            (double) _nolanes / (double) prioSum;
        // do not let this number be greater than the number of available lanes
        if(res>_nolanes)
            res = _nolanes;
        // add it to the list
        resultingLanes.push_back(res);
        sumResulting += res;
        if(minResulting>res)
            minResulting = res;
    }
    // compute the number of virtual edges
    //  a virtual edge is used as a replacement for a real edge from now on
    //  it shall ollow to divide the existing lanes on this structure without
    //  regarding the structure of outgoing edges
    // the next line is absolutely unneeded and false;
    //  Still, I encountered a conversion from 7,0000000000001400000000000028 to
    //  an int yielding in 6; This is by far a worse problem and I hope to manage
    //  it by adding a small offset...
    // Sorry, Daniel Krajzewicz
/*
    double g4t = (sumResulting / minResulting);
    int rer = int(g4t);
    double f1 = floor(g4t);
    double f2 = ceil(g4t);
*/
    sumResulting += minResulting / 2;
    size_t noVirtual = (size_t) (sumResulting / minResulting);
    // compute the transition from virtual to real edges
    vector<NBEdge*> transition;
    transition.reserve(size);
    for(i=0; i<size; i++) {
        // tmpNo will be the number of connections from this edge
        //  to the next edge
        assert(i<resultingLanes.size());
        double tmpNo = (double) resultingLanes[i] / (double) minResulting;
        for(double j=0; j<tmpNo; j++) {
    	    assert(outgoing->size()>i);
            transition.push_back((*outgoing)[i]);
        }
    }
    // assign lanes to edges
    //  (conversion from virtual to real edges is done)
    ToEdgeConnectionsAdder *adder
        = new ToEdgeConnectionsAdder(_ToEdges, transition);
    Bresenham::compute(adder, _nolanes, noVirtual);
    delete adder;
    delete priorities;
}


vector<size_t> *
NBEdge::preparePriorities(const vector<NBEdge*> *outgoing)
{
    // copy the priorities first
    vector<size_t> *priorities = new vector<size_t>();
    if(outgoing->size()==0) {
        return priorities;
    }
    priorities->reserve(outgoing->size());
    vector<NBEdge*>::const_iterator i;
    for(i=outgoing->begin(); i!=outgoing->end(); i++) {
        int prio = (*i)->getJunctionPriority(_to);
	    assert(((*i)->getJunctionPriority(_to)+1)*2>0);
        prio = (prio+1) * 2;
        priorities->push_back(prio);
    }
    // when the right turning direction has not a higher priority, divide
    //  the importance by 2 due to the possibility to leave the junction
    //  faster from this lane
    MainDirections mainDirections(*outgoing, this, _to);
    if(!mainDirections.includes(MainDirections::DIR_RIGHTMOST)) {
        assert(priorities->size()>0);
        (*priorities)[0] = (*priorities)[0] / 2;
    }
    // HEURISTIC:
    // when no higher priority exists, let the forward direction be
    //  the main direction
    if(mainDirections.empty()) {
        vector<NBEdge*> tmp(*outgoing);
        sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(this));
        i=find(
            outgoing->begin(),
            outgoing->end(),
            *(tmp.begin()));
        size_t dist = distance(outgoing->begin(), i);
        assert(dist<priorities->size());
        (*priorities)[dist] = (*priorities)[dist] * 2;
    }
    // return
    return priorities;
}


size_t
NBEdge::computePrioritySum(vector<size_t> *priorities)
{
    size_t sum = 0;
    for(vector<size_t>::iterator i=priorities->begin(); i!=priorities->end(); i++) {
        sum += (*i);
    }
    return sum;
}


void
NBEdge::computeLinkPriorities()
{
    // reset all links to unprioritised
    size_t i;
    for(i=0; i<_nolanes; i++) {
        assert(_reachable!=0&&i<_reachable->size());
        size_t size = (*_reachable)[i].size();
        (*_reachablePriorities)[i].resize(size, LINKTYPE_UNPRIORISED);
    }
    // the link stays unpriorised when the edge is not a prioritised edge
    // (the vehicles must then always look out for incoming vehicles)
    if(getJunctionPriority(_to)==0) {
        return;
    }
    // check whether which links of this priorised edge are always priorised
    // (don't have to look out for other vehicles)
    const EdgeVector *nodeEdges = _to->getEdges();
    for(i=0; i<_nolanes; i++) {
        size_t size = (*_reachable)[i].size();
        size_t j;
        for(j=0; j<size; j++) {
            assert(_reachable!=0&&i<_reachable->size()&&j<(*_reachable)[i].size());
            if((*_reachable)[i][j].edge!=0) {
                // when the destination is priorised, the link is priorised
                // but only when not turning
                if((*_reachable)[i][j].edge->getJunctionPriority(_to)==1) {
                    if((*_reachable)[i][j].edge!=_turnDestination)
                        (*_reachablePriorities)[i][j] = LINKTYPE_PRIORISED;
                }
                // otherwise, check whether the direction is on the right side
                // of the other side of the priorised connection
                else {
                    bool found = false;
                    EdgeVector::const_iterator me = find(nodeEdges->begin(), nodeEdges->end(), this);
                    me = NBContHelper::nextCCW(nodeEdges, me);
                    while(!found&&(*me)->getJunctionPriority(_to)!=1) {
                        if(*me==(*_reachable)[i][j].edge) {
                            (*_reachablePriorities)[i][j] = LINKTYPE_PRIORISED;
                            found = true;
                        }
                        me = NBContHelper::nextCCW(nodeEdges, me);
                    }
                }
            }
      // check whether other lanes from this edge may cross and lower the
      // priority if so.
      // optimization needed
/*      bool found = false;
      if((*_reachablePriorities)[i][j] == NBEdge::LINKTYPE_PRIORISED) {
        for(size_t k=0; !found && k<_edge->getNoLanes(); k++) {
          size_t currsize = (*_reachableEdges)[k].size();
          for(size_t l=0; !found && l<currsize; l++) {
            if((*_reachableEdges)[i][j]==(*_reachableEdges)[k][l]) {
              (*_reachablePriorities)[i][j] == NBEdge::LINKTYPE_PRIORISED;
              found = true;
            }
          }
        }
      }*/
      // this code is not needed as no lanes may have the same destination
      // lane
    }
  }
}


void
NBEdge::appendTurnaround()
{
    if(_turnDestination!=0) {
        setConnection(_nolanes-1, _turnDestination,
            _turnDestination->getNoLanes()-1);
    }
}


void
NBEdge::sortOutgoingLanesConnections()
{
    for(size_t i=0; i<_reachable->size(); i++) {
        sort((*_reachable)[i].begin(), (*_reachable)[i].end(),
            NBContHelper::relative_edgelane_sorter(this, _to));
    }
}



void
NBEdge::setConnection(size_t src_lane, NBEdge *dest_edge, size_t dest_lane)
{
    if(_step==INIT_REJECT_CONNECTIONS) {
        return;
    }
    assert(dest_lane>=0&&dest_lane<=10);
    assert(src_lane>=0&&src_lane<=10);
    // this connection has not yet been set
    bool known = false;
    // find the entry which holds the information which lanes may
    //  be used to reach the current destination edge
    LanesThatSucceedEdgeCont::iterator i = _succeedinglanes->find(dest_edge);
    // if there is no such connection yet,
    //  build it
    if(i==_succeedinglanes->end()) {
        LaneVector lanes;
        lanes.push_back(src_lane);
        (*_succeedinglanes)[dest_edge] = lanes;
    // else
    } else {
        // check whether one of the known connections to the current destination
        //  comes from the same lane as the current to add
        LaneVector lanes = (*i).second;
        LaneVector::iterator i = find(lanes.begin(), lanes.end(), src_lane);
        // if not, append
        if(i==lanes.end())
            lanes.push_back(src_lane);
        // otherwise, mark as known
        else
            known = true;
        (*_succeedinglanes)[dest_edge] = lanes;
    }
    // append current connection only if no equal is already known
    if(!known) {
	    assert(_reachable->size()>src_lane);
        EdgeLane el;
        el.edge = dest_edge;
        el.lane = dest_lane; // !!! EdgeLane as class
        assert(_reachable!=0&&src_lane<_reachable->size());
        (*_reachable)[src_lane].push_back(el);
    }
}


bool
NBEdge::isTurningDirection(NBEdge *edge) const
{
    return edge == _turnDestination;
}



NBNode *
NBEdge::tryGetNodeAtPosition(double pos, double tolerance) const
{
    // return the from-node when the position is at the begin of the edge
    if(pos<tolerance) {
        return _from;
    }
    // return the to-node when the position is at the end of the edge
    if(pos>_length-tolerance) {
        return _to;
    }
    return 0;
}


NBEdge *
NBEdge::checkCorrectNode(NBEdge *opposite)
{
    if(_from->hasOutgoing(opposite)) {
        return _from->getOppositeIncoming(this);
    }
    if(_to->hasIncoming(opposite)) {
        return _to->getOppositeOutgoing(this);
    }
    return this;
}


NBEdge::EdgeBasicFunction
NBEdge::getBasicType() const
{
    return _basicType;
}


double
NBEdge::getSpeed() const
{
    return _speed;
}


void
NBEdge::replaceInConnections(NBEdge *which, NBEdge *by)
{
    // replace in "_connectedEdges"
    if(_connectedEdges!=0) {
        for(size_t i=0; i<_connectedEdges->size(); i++) {
            if((*_connectedEdges)[i]==which) {
                (*_connectedEdges)[i] = by;
            }
        }
    }
    // check whether it was the turn destination
    if(_turnDestination==which) {
        _turnDestination = by;
    }
    // replace in _ToEdges
    if(_ToEdges!=0) {
        bool found = true;
        bool have = _ToEdges->find(by)!=_ToEdges->end();
        std::map<NBEdge*, std::vector<size_t> >::iterator j = _ToEdges->find(which);
        if(j!=_ToEdges->end()) {
            if(have) {
                copy((*_ToEdges)[which].begin(), (*_ToEdges)[which].end(),
                    back_inserter((*j).second));
            } else {
                have = true;
                (*_ToEdges)[by] = (*j).second;
            }
        }
    }
    // replace in _reachable
    if(_reachable!=0) {
        for(ReachableFromLaneVector::iterator k=_reachable->begin(); k!=_reachable->end(); k++) {
            for(EdgeLaneVector::iterator l=(*k).begin(); l!=(*k).end(); l++) {
                if((*l).edge==which) {
                    (*l).edge = by;
                }
            }
        }
    }
    // replace in _succeedinglanes
    if(_succeedinglanes!=0) {
        LanesThatSucceedEdgeCont::iterator l=_succeedinglanes->find(which);
        if(l!=_succeedinglanes->end()) {
            (*_succeedinglanes)[by] = (*l).second;
            _succeedinglanes->erase(l);
        }
    }
}


bool
NBEdge::isConnectedTo(NBEdge *e)
{
    //
    if(_connectedEdges==0) {
        return false;
    }
    //
    return
        find(_connectedEdges->begin(), _connectedEdges->end(), e)
        !=
        _connectedEdges->end();

}


void
NBEdge::remapConnections(const EdgeVector &incoming)
{
    for(EdgeVector::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        NBEdge *inc = *i;
        // We have to do this
        inc->_step = EDGE2EDGES;
        // add all connections
        if(_connectedEdges!=0) {
            for(EdgeVector::iterator j=_connectedEdges->begin(); j!=_connectedEdges->end(); j++) {
                inc->addEdge2EdgeConnection(*j);
            }
        }
        inc->removeFromConnections(this);
    }
}


void
NBEdge::removeFromConnections(NBEdge *which)
{
    // remove from "_connectedEdges"
    if(_connectedEdges!=0) {
        for(size_t i=0; i<_connectedEdges->size(); i++) {
            if((*_connectedEdges)[i]==which) {
                _connectedEdges->erase(_connectedEdges->begin()+i);
                i--;
            }
        }
    }
    // check whether it was the turn destination
    if(_turnDestination==which) {
        _turnDestination = 0;
    }
    // remove in _ToEdges
    if(_ToEdges!=0) {
        bool found = true;
        std::map<NBEdge*, std::vector<size_t> >::iterator j = _ToEdges->find(which);
        if(j!=_ToEdges->end()) {
            _ToEdges->erase(which);
        }
    }
    // remove in _reachable
    if(_reachable!=0) {
        for(ReachableFromLaneVector::iterator k=_reachable->begin(); k!=_reachable->end(); k++) {
            EdgeLaneVector::iterator l=(*k).begin();
            while(l!=(*k).end()) {
                if((*l).edge==which) {
                    (*k).erase(l);
                    l = (*k).begin();
                } else {
                    l++;
                }
            }
        }
    }
    // remove in _succeedinglanes
    if(_succeedinglanes!=0) {
        LanesThatSucceedEdgeCont::iterator l=_succeedinglanes->find(which);
        if(l!=_succeedinglanes->end()) {
            _succeedinglanes->erase(l);
        }
    }
}


void
NBEdge::invalidateConnections()
{
    delete _connectedEdges;
    _connectedEdges = 0;
    _turnDestination = 0;
    _ToEdges->clear();
    _reachable->clear();
    _reachable->resize(_nolanes, EdgeLaneVector());
    _reachablePriorities->clear();
    _reachablePriorities->resize(_nolanes, IntVector());
    _succeedinglanes->clear();
    _step = INIT_REJECT_CONNECTIONS;
}


EdgeVector
NBEdge::getConnected() const
{
    if(_connectedEdges==0) {
        return EdgeVector();
    }
    return EdgeVector(*_connectedEdges);
}



bool
NBEdge::lanesWereAssigned() const
{
    return _step==EDGE2LANES;
}


EdgeVector
NBEdge::getEdgesFromLane(size_t lane) const
{
    assert(lane<_reachable->size());
    EdgeVector ret;
    for(EdgeLaneVector::const_iterator i=(*_reachable)[lane].begin(); i!=(*_reachable)[lane].end(); i++) {
        ret.push_back((*i).edge);
    }
    return ret;
}

const Position2DVector &
NBEdge::getGeometry() const
{
    return myGeom;
}


void
NBEdge::setGeometry(const Position2DVector &s)
{
    myGeom = s;
}


double
NBEdge::getMaxLaneOffset()
{
    return 3.5 * _nolanes;
}


Position2D
NBEdge::getMaxLaneOffsetPositionAt(NBNode *node, double width)
{
    Position2D pos = myGeom.positionAtLengthPosition(width);
    if(node==_from) {
        GeomHelper::transfer_to_side(pos,
            myGeom.at(0), myGeom.at(myGeom.size()-1),
            3.5 * _nolanes);
    } else {
        GeomHelper::transfer_to_side(pos,
            myGeom.at(myGeom.size()-1), myGeom.at(0),
            -3.5 * _nolanes);
    }
    return pos;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBEdge.icc"
//#endif

// Local Variables:
// mode:C++
// End:

