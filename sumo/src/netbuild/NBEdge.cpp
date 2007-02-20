/****************************************************************************/
/// @file    NBEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Methods for the representation of a single egde
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

#include <vector>
#include <string>
#include <algorithm>
#include <cassert>
#include "NBEdgeCont.h"
#include "nodes/NBNode.h"
#include "nodes/NBNodeCont.h"
#include "NBContHelper.h"
#include "NBHelpers.h"
#include <cmath>
#include <iomanip>
#include "NBTypeCont.h"
#include <iostream>
#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "NBEdge.h"
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <utils/geoconv/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// debug definitions
// ===========================================================================
#define DEBUG_OUT cout


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
/* -------------------------------------------------------------------------
 * NBEdge::ToEdgeConnectionsAdder-methods
 * ----------------------------------------------------------------------- */
void
NBEdge::ToEdgeConnectionsAdder::execute(SUMOReal lane, SUMOReal virtEdge)
{
    // check
    assert(virtEdge>=0);
    assert(_transitions.size()>(size_t) virtEdge);
    assert(lane>=0&&lane<10);
    // get the approached edge
    NBEdge *succEdge = _transitions[(int) virtEdge];
    vector<size_t> lanes;

    // check whether the currently regarded, approached edge has already
    //  a connection starting at the edge which is currently being build
    map<NBEdge*, vector<size_t> >::iterator i=_connections->find(succEdge);
    if (i!=_connections->end()) {
        // if there were already lanes assigned, get them
        lanes = (*i).second;
    }

    // check whether the current lane was already used to connect the currently
    //  regarded approached edge
    vector<size_t>::iterator j=find(lanes.begin(), lanes.end(), (size_t) lane);
    if (j==lanes.end()) {
        // if not, add it to the list
        lanes.push_back((size_t) lane);
    }
    // set information about connecting lanes
    (*_connections)[succEdge] = lanes;
}



/* -------------------------------------------------------------------------
 * NBEdge::MainDirections-methods
 * ----------------------------------------------------------------------- */
NBEdge::MainDirections::MainDirections(const std::vector<NBEdge*> &outgoing,
                                       NBEdge *parent, NBNode *to)
{
    if (outgoing.size()==0) {
        return;
    }
    // check whether the right turn has a higher priority
    assert(outgoing.size()>0);
    if (outgoing[0]->getJunctionPriority(to)==1) {
        _dirs.push_back(MainDirections::DIR_RIGHTMOST);
    }
    // check whether the left turn has a higher priority
    if (outgoing[outgoing.size()-1]->getJunctionPriority(to)==1) {
        // ok, the left turn belongs to the higher priorised edges on the junction
        //  let's check, whether it has also a higher priority (lane number/speed) 
        //  than the current
        vector<NBEdge*> tmp(outgoing);
        sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(parent));
        if (outgoing[outgoing.size()-1]->getPriority()>tmp[0]->getPriority()) {
            _dirs.push_back(MainDirections::DIR_LEFTMOST);
        } else {
            if (outgoing[outgoing.size()-1]->getNoLanes()>tmp[0]->getNoLanes()) {
                _dirs.push_back(MainDirections::DIR_LEFTMOST);
            }
        }
    }
    // check whether the forward direction has a higher priority
    //  get the forward direction
    vector<NBEdge*> tmp(outgoing);
    sort(tmp.begin(), tmp.end(),
         NBContHelper::edge_similar_direction_sorter(parent));
    NBEdge *edge = *(tmp.begin());
    // check whether it has a higher priority
    if (edge->getJunctionPriority(to)==1) {
        _dirs.push_back(MainDirections::DIR_FORWARD);
    }
}


NBEdge::MainDirections::~MainDirections()
{}


bool
NBEdge::MainDirections::empty() const
{
    return _dirs.empty();
}


bool
NBEdge::MainDirections::includes(Direction d) const
{
    return find(_dirs.begin(), _dirs.end(), d)!=_dirs.end();
}



/* -------------------------------------------------------------------------
 * NBEdge-methods
 * ----------------------------------------------------------------------- */
NBEdge::NBEdge(string id, string name, NBNode *from, NBNode *to,
               string type, SUMOReal speed, size_t nolanes,
               SUMOReal length, int priority, LaneSpreadFunction spread,
               EdgeBasicFunction basic) :
        _step(INIT), _id(StringUtils::convertUmlaute(id)),
        _type(StringUtils::convertUmlaute(type)),
        _nolanes(nolanes), _from(from), _to(to), _length(length), _angle(0),
        _priority(priority), _speed(speed),
        _name(StringUtils::convertUmlaute(name)),
        _turnDestination(0),
        _fromJunctionPriority(-1), _toJunctionPriority(-1),
        _basicType(basic), myLaneSpreadFunction(spread),
        myAllowedOnLanes(nolanes), myNotAllowedOnLanes(nolanes),
        myAmTurningWithAngle(0), myAmTurningOf(0)
{
    assert(_nolanes!=0);
    if (_from==0||_to==0) {
        throw std::exception();
    }
    _angle = NBHelpers::angle(
                 _from->getPosition().x(), _from->getPosition().y(),
                 _to->getPosition().x(), _to->getPosition().y()
             );
    _from->addOutgoingEdge(this);
    _to->addIncomingEdge(this);
    // prepare container
    _reachable.resize(_nolanes, EdgeLaneVector());
    if (_length<=0) {
        _length = GeomHelper::distance(_from->getPosition(), _to->getPosition());
    }
    assert(_from->getPosition()!=_to->getPosition());
    myGeom.push_back(_from->getPosition());
    myGeom.push_back(_to->getPosition());
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in constructor" << endl;
        throw 1;
    }
#endif
#endif
    computeLaneShapes();
    for (size_t i=0; i<_nolanes; i++) {
        myLaneSpeeds.push_back(speed);
    }
}


NBEdge::NBEdge(string id, string name, NBNode *from, NBNode *to,
               string type, SUMOReal speed, size_t nolanes,
               SUMOReal length, int priority,
               Position2DVector geom, LaneSpreadFunction spread,
               EdgeBasicFunction basic) :
        _step(INIT), _id(StringUtils::convertUmlaute(id)),
        _type(StringUtils::convertUmlaute(type)),
        _nolanes(nolanes), _from(from), _to(to), _length(length), _angle(0),
        _priority(priority), _speed(speed),
        _name(StringUtils::convertUmlaute(name)), _turnDestination(0),
        _fromJunctionPriority(-1), _toJunctionPriority(-1),
        _basicType(basic), myGeom(geom), myLaneSpreadFunction(spread),
        myAllowedOnLanes(nolanes), myNotAllowedOnLanes(nolanes),
        myAmTurningWithAngle(0), myAmTurningOf(0)
{
    assert(_nolanes!=0);
    if (_from==0||_to==0) {
        throw std::exception();
    }
    myGeom.push_back_noDoublePos(to->getPosition());
    myGeom.push_front_noDoublePos(from->getPosition());
    _angle = NBHelpers::angle(
                 _from->getPosition().x(), _from->getPosition().y(),
                 _to->getPosition().x(), _to->getPosition().y()
             );
    _from->addOutgoingEdge(this);
    _to->addIncomingEdge(this);
    // prepare container
    _reachable.resize(_nolanes, EdgeLaneVector());
    if (_length<=0) {
        _length = GeomHelper::distance(
                      _from->getPosition(), _to->getPosition());
    }
    assert(myGeom.size()>=2);
    computeLaneShapes();
    for (size_t i=0; i<_nolanes; i++) {
        myLaneSpeeds.push_back(speed);
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in constructor" << endl;
        throw 1;
    }
#endif
#endif
}


NBEdge::~NBEdge()
{}


SUMOReal
NBEdge::getAngle()
{
    return _angle;
}


size_t
NBEdge::getNoLanes() const
{
    return _nolanes;
}


int
NBEdge::getPriority() const
{
    return _priority;
}


int
NBEdge::getJunctionPriority(NBNode *node)
{
    if (node==_from) {
        return _fromJunctionPriority;
    } else {
        return _toJunctionPriority;
    }
}


void
NBEdge::setJunctionPriority(NBNode *node, int prio)
{
    if (node==_from) {
        _fromJunctionPriority = prio;
    } else {
        _toJunctionPriority = prio;
    }
}


const string &
NBEdge::getID() const
{
    return _id;
}


string
NBEdge::getName() const
{
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
NBEdge::getType()
{
    return _type;
}


SUMOReal
NBEdge::getLength()
{
    return _length;
}


const EdgeLaneVector &
NBEdge::getEdgeLanesFromLane(size_t lane) const
{
    assert(lane<_reachable.size());
    return _reachable[lane];
}


void
NBEdge::computeTurningDirections()
{
    _turnDestination = 0;
    EdgeVector outgoing = _to->getOutgoingEdges();
    for (EdgeVector::iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if (outedge->getBasicType()==EDGEFUNCTION_SINK||_basicType==EDGEFUNCTION_SOURCE) {
            continue;
        }
        SUMOReal relAngle =
            NBHelpers::relAngle(getAngle(*_to), outedge->getAngle(*_to));
        // do not append the turnaround
        if (fabs(relAngle)>160) {
            setTurningDestination(outedge);
        }
    }
}


SUMOReal
NBEdge::getAngle(const NBNode &atNode) const
{
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in getAngle" << endl;
        throw 1;
    }
#endif
#endif
    if (&atNode==_from) {
        return myGeom.getBegLine().atan2DegreeAngle();
    } else {
        assert(&atNode==_to);
        return myGeom.getEndLine().atan2DegreeAngle();
    }
}


void
NBEdge::setTurningDestination(NBEdge *e)
{
    SUMOReal cur = fabs(NBHelpers::relAngle(getAngle(), e->getAngle()));
    SUMOReal old =
        _turnDestination==0
        ? 0
        : fabs(NBHelpers::relAngle(getAngle(), _turnDestination->getAngle()));
    if (cur>old
            &&
            e->acceptBeingTurning(this)) {

        _turnDestination = e;
    }
}


bool
NBEdge::acceptBeingTurning(NBEdge *e)
{
    if (e==myAmTurningOf) {
        return true;
    }
    SUMOReal angle = fabs(NBHelpers::relAngle(getAngle(), e->getAngle()));
    if (myAmTurningWithAngle>angle) {
        return false;
    }
    if (myAmTurningWithAngle==angle) {
        return false; // !!! ok, this happens only within a cell-network (backgrnd), we have to take a further look sometime
    }
    NBEdge *previous = myAmTurningOf;
    myAmTurningWithAngle = angle;
    myAmTurningOf = e;
    if (previous!=0) {
        previous->computeTurningDirections();
    }
    return true;
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
    "\" XFrom=\"" << _from->getPosition().x() <<
    "\" YFrom=\"" << _from->getPosition().y() <<
    "\" XTo=\"" << _to->getPosition().x() <<
    "\" YTo=\"" << _to->getPosition().y() <<
    "\" from=\"" << _from->getID() <<
    "\" to=\"" << _to->getID() <<
    "\" Priority=\"" << _priority <<
    "\" Angle=\"" << getAngle() <<
    "\" Type=\"" << _type <<
    "\" function=\"";
    switch (_basicType) {
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
    for (size_t i=0; i<_nolanes; i++) {
        writeLane(into, i);
    }
    into << "      </lanes>" << endl;
    // write the list of connected edges
    const std::vector<NBEdge*> *tmp = getConnectedSorted();
    std::vector<NBEdge*> sortedConnected = *tmp;
    if (getTurnDestination()!=0) {
        sortedConnected.push_back(getTurnDestination());
    }
    delete tmp;
    for (std::vector<NBEdge*>::iterator l=sortedConnected.begin(); l!=sortedConnected.end(); l++) {
        LanesThatSucceedEdgeCont::iterator m = _succeedinglanes.find(*l);
        if (m!=_succeedinglanes.end()) {
            writeConnected(into, (*m).first, (*m).second);
        }
    }
    // close the edge
    into << "   </edge>" << endl << endl;
}


void
NBEdge::writeXMLStep2(std::ostream &into, bool includeInternal)
{
    for (size_t i=0; i<_nolanes; i++) {
        writeSucceeding(into, i, includeInternal);
    }
}


void
NBEdge::writeLane(std::ostream &into, size_t lane)
{
    // output the lane's attributes
    into << "         <lane id=\"" << _id << '_' << lane << "\"";
    // the first lane of an edge will be the depart lane
    if (lane==0) {
        into << " depart=\"1\"";
    } else {
        into << " depart=\"0\"";
    }
    // write the list of allowed/disallowed vehicle classes
    {
        into << " vclasses=\"";
        std::vector<SUMOVehicleClass>::const_iterator i;
        bool hadOne = false;
        for (i=myAllowedOnLanes[lane].begin(); i!=myAllowedOnLanes[lane].end(); ++i) {
            if (hadOne) {
                into << ';';
            }
            into << getVehicleClassName(*i);
            hadOne = true;
        }
        for (i=myNotAllowedOnLanes[lane].begin(); i!=myNotAllowedOnLanes[lane].end(); ++i) {
            if (hadOne) {
                into << ';';
            }
            into << '-' << getVehicleClassName(*i);
            hadOne = true;
        }
        into << '\"';
    }
    // some further information
    if (myLaneSpeeds[lane]==0) {
        WRITE_WARNING("Lane #" + toString<size_t>(lane) + " of edge '" + _id + "' has a maximum velocity of 0.");
    } else if (myLaneSpeeds[lane]<0) {
        MsgHandler::getErrorInstance()->inform(
            "Negative velocity (" + toString<SUMOReal>(myLaneSpeeds[lane]) + " on edge '" + _id + "' lane#" + toString<size_t>(lane) + ".");
        throw ProcessError();
    }
    into << " maxspeed=\"" << myLaneSpeeds[lane] << "\" length=\"" << _length <<
    "\">";
    // the lane's shape
    into << myLaneGeoms[lane];
    // close
    into << "</lane>" << endl;
}


void
NBEdge::setLaneSpeed(int lane, SUMOReal speed)
{
    myLaneSpeeds[lane] = speed;
}


SUMOReal
NBEdge::getLaneSpeed(int lane) const
{
    return myLaneSpeeds[lane];
}


void
NBEdge::computeLaneShapes()
{
    // vissim needs this
    if (_from==_to) {
        return;
    }
    // build the shape of each lane
    myLaneGeoms.clear();
    for (size_t i=0; i<_nolanes; i++) {
        myLaneGeoms.push_back(computeLaneShape(i));
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[myLaneGeoms.size()-1].assertNonEqual()) {
            DEBUG_OUT << getID() << "in computeLaneShapes" << endl;
            throw 1;
        }
#endif
#endif
    }
}


const Position2DVector &
NBEdge::getLaneShape(size_t i) const
{
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myLaneGeoms[i].assertNonEqual()) {
        DEBUG_OUT << getID() << "in getLaneShape" << endl;
        throw 1;
    }
#endif
#endif
    return myLaneGeoms[i];
}


Position2DVector
NBEdge::computeLaneShape(size_t lane)
{
    Position2DVector shape;

    for (size_t i=0; i<myGeom.size(); i++) {
        if (/*i==myGeom.size()-2||*/i==0) {
            Position2D from = myGeom[i];
            Position2D to = myGeom[i+1];
            std::pair<SUMOReal, SUMOReal> offsets =
                laneOffset(from, to, SUMO_const_laneWidthAndOffset, _nolanes-1-lane);
            shape.push_back_noDoublePos(//.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position2D(from.x()-offsets.first, from.y()-offsets.second));
        } else if (i==myGeom.size()-1) {
            Position2D from = myGeom[i-1];
            Position2D to = myGeom[i];
            std::pair<SUMOReal, SUMOReal> offsets =
                laneOffset(from, to, SUMO_const_laneWidthAndOffset, _nolanes-1-lane);
            shape.push_back_noDoublePos(//.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position2D(to.x()-offsets.first, to.y()-offsets.second));
        } else {
            Position2D from = myGeom[i-1];
            Position2D me = myGeom[i];
            Position2D to = myGeom[i+1];
            std::pair<SUMOReal, SUMOReal> offsets =
                laneOffset(from, me, SUMO_const_laneWidthAndOffset, _nolanes-1-lane);
            std::pair<SUMOReal, SUMOReal> offsets2 =
                laneOffset(me, to, SUMO_const_laneWidthAndOffset, _nolanes-1-lane);
            Line2D l1(
                Position2D(from.x()-offsets.first, from.y()-offsets.second),
                Position2D(me.x()-offsets.first, me.y()-offsets.second));
            l1.extrapolateBy(100);
            Line2D l2(
                Position2D(me.x()-offsets2.first, me.y()-offsets2.second),
                Position2D(to.x()-offsets2.first, to.y()-offsets2.second));
            l2.extrapolateBy(100);
            if (l1.intersects(l2)) {
                shape.push_back_noDoublePos(l1.intersectsAt(l2));
            } else {
                // !!! should never happen
                //   throw 1;
            }
        }
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!shape.assertNonEqual()) {
        DEBUG_OUT << getID() << "computeLaneShape" << endl;
    }
#endif
#endif
    return shape;
}


std::pair<SUMOReal, SUMOReal>
NBEdge::laneOffset(const Position2D &from, const Position2D &to,
                   SUMOReal lanewidth, size_t lane)
{
    SUMOReal x1 = from.x();
    SUMOReal y1 = from.y();
    SUMOReal x2 = to.x();
    SUMOReal y2 = to.y();
    assert(x1!=x2||y1!=y2);
    SUMOReal length = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    std::pair<SUMOReal, SUMOReal> offsets =
        GeomHelper::getNormal90D_CW(x1, y1, x2, y2, length, lanewidth);
    SUMOReal xoff = offsets.first / (SUMOReal) 2.0;
    SUMOReal yoff = offsets.second / (SUMOReal) 2.0;
    if (myLaneSpreadFunction==LANESPREAD_RIGHT) {
        xoff += (offsets.first * (SUMOReal) lane);
        yoff += (offsets.second * (SUMOReal) lane);
    } else {
        xoff += (offsets.first * (SUMOReal) lane) - (offsets.first * (SUMOReal) _nolanes / (SUMOReal) 2.0);
        yoff += (offsets.second * (SUMOReal) lane) - (offsets.second * (SUMOReal) _nolanes / (SUMOReal) 2.0);
    }
    return std::pair<SUMOReal, SUMOReal>(xoff, yoff);
}


void
NBEdge::writeConnected(std::ostream &into, NBEdge *edge, LaneVector &lanes)
{
    if (edge==0) {
        return;
    }
    into << "      <cedge id=\"" << edge->getID() << "\">";
    size_t noApproachers = lanes.size();
    for (size_t i=0; i<noApproachers; i++) {
        assert(i<lanes.size());
        into << _id << '_' << lanes[i];
        if (i<noApproachers-1) {
            into << ' ';
        }
    }
    into << "</cedge>" << endl;
}


void
NBEdge::writeSucceeding(std::ostream &into, size_t lane,
                        bool includeInternal)
{
    into << "   <succ edge=\"" << _id << "\" lane=\"" << _id << "_"
    << lane << "\" junction=\"" << _to->getID() << "\">" << endl;
    // the lane may be unconnented; output information about being invalid
    assert(lane<_reachable.size());
    size_t noApproached = _reachable[lane].size();
    if (noApproached==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\"/>"
        << endl;
    }
    // output list of connected lanes
    // go through each connected edge
    for (size_t j=0; j<noApproached; j++) {
        writeSingleSucceeding(into, lane, j, includeInternal);
    }
    into << "   </succ>" << endl << endl;
}


void
NBEdge::writeSingleSucceeding(std::ostream &into, size_t fromlane, size_t destidx,
                              bool includeInternal)
{
    // check whether the connected lane is invalid
    //  (should not happen; this is an artefact left from previous versions)
    assert(fromlane<_reachable.size());
    assert(destidx<_reachable[fromlane].size());
    if (_reachable[fromlane][destidx].edge==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\" "
        << "dir=\"s\" state=\"O\"/>" // !!! check dummy values
        << endl;
        return;
    }
    // write the id
    into << "      <succlane lane=\""
    << _reachable[fromlane][destidx].edge->getID() << '_'
    << _reachable[fromlane][destidx].lane << '\"'; // !!! classe LaneEdge mit getLaneID
    if (includeInternal) {
        into << " via=\""
        << _to->getInternalLaneID(this, fromlane, _reachable[fromlane][destidx].edge, _reachable[fromlane][destidx].lane)
        << "_0\"";
    }
    // set information about the controlling tl if any
    if (_reachable[fromlane][destidx].tlID!="") {
        into << " tl=\"" << _reachable[fromlane][destidx].tlID << "\"";
        into << " linkno=\"" << _reachable[fromlane][destidx].tlLinkNo << "\"";
    }
    // write information whether the connection yields
    if (!_to->mustBrake(this,
                        _reachable[fromlane][destidx].edge,
                        _reachable[fromlane][destidx].lane)) {
        into << " yield=\"0\"";
    } else {
        into << " yield=\"1\"";
    }
    // write the direction information
    NBMMLDirection dir = _to->getMMLDirection(this, _reachable[fromlane][destidx].edge);
    into << " dir=\"";
    switch (dir) {
    case MMLDIR_STRAIGHT:
        into << "s";
        break;
    case MMLDIR_LEFT:
        into << "l";
        break;
    case MMLDIR_RIGHT:
        into << "r";
        break;
    case MMLDIR_TURN:
        into << "t";
        break;
    case MMLDIR_PARTLEFT:
        into << "L";
        break;
    case MMLDIR_PARTRIGHT:
        into << "R";
        break;
    default:
        // should not happen
        assert(false);
        break;
    }
    into << "\" ";
    // write the state information
    if (_reachable[fromlane][destidx].tlID!="") {
        into << "state=\"t";
    } else {
        into << "state=\""
        << _to->stateCode(this, _reachable[fromlane][destidx].edge, _reachable[fromlane][destidx].lane);
    }
    // close
    into << "\"/>" << endl;
}


bool
NBEdge::hasRestrictions() const
{
    for (size_t i=0; i<_nolanes; ++i) {
        if (myAllowedOnLanes[i].size()!=0) {
            return true;
        }
        if (myNotAllowedOnLanes[i].size()!=0) {
            return true;
        }
    }
    return false;
}


void
NBEdge::writeLanesPlain(std::ostream &into)
{
    for (size_t lane=0; lane<_nolanes; ++lane) {
        into << "      <lane id=\"" << lane << "\"";
        // write allowed lanes
        if (myAllowedOnLanes[lane].size()!=0) {
            bool hadOne = false;
            into << " allow=\"";
            std::vector<SUMOVehicleClass>::const_iterator i;
            for (i=myAllowedOnLanes[lane].begin(); i!=myAllowedOnLanes[lane].end(); ++i) {
                if (hadOne) {
                    into << ';';
                }
                into << getVehicleClassName(*i);
                hadOne = true;
            }
            into << "\"";
        }
        if (myNotAllowedOnLanes[lane].size()!=0) {
            bool hadOne = false;
            into << " disallow=\"";
            std::vector<SUMOVehicleClass>::const_iterator i;
            for (i=myNotAllowedOnLanes[lane].begin(); i!=myNotAllowedOnLanes[lane].end(); ++i) {
                if (hadOne) {
                    into << ';';
                }
                into << getVehicleClassName(*i);
                hadOne = true;
            }
            into << "\"";
        }
        into << "/>" << endl;
    }
}


bool
NBEdge::addEdge2EdgeConnection(NBEdge *dest)
{
    if (_step==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (_to!=dest->_from) {
        return false;
    }
    if (find(_connectedEdges.begin(), _connectedEdges.end(), dest)==_connectedEdges.end()) {
        _connectedEdges.push_back(dest);
    }
    if (_step<EDGE2EDGES) {
        _step = EDGE2EDGES;
    }
    // !!! recheck the next - something too much is deleted in invalidateConnections
    if (_ToEdges.find(dest)==_ToEdges.end()) {
        _ToEdges[dest] = vector<size_t>();
    }
    return true;
}


bool
NBEdge::addLane2LaneConnection(size_t from, NBEdge *dest,
                               size_t toLane, bool markAs2Recheck,
                               bool mayUseSameDestination)
{
    if (_step==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (_to!=dest->_from) {
        return false;
    }
    bool ok = addEdge2EdgeConnection(dest);
    if (ok) {
        setConnection(from, dest, toLane, markAs2Recheck, mayUseSameDestination);
        vector<size_t> &lanes = (_ToEdges.find(dest))->second;
        vector<size_t>::iterator i = find(lanes.begin(), lanes.end(), from);
        if (i==lanes.end()) {
            lanes.push_back(from);
        }
    }
    return ok;
}


bool
NBEdge::addLane2LaneConnections(size_t fromLane,
                                NBEdge *dest, size_t toLane,
                                size_t no,
                                bool markAs2Recheck,
                                bool invalidatePrevious)
{
    if (invalidatePrevious) {
        invalidateConnections(true);
    }
    bool ok = true;
    for (size_t i=0; i<no&&ok; i++) {
        ok &= addLane2LaneConnection(fromLane+i, dest, toLane+i, markAs2Recheck);
    }
    return ok;
}


bool
NBEdge::computeEdge2Edges()
{
    // return if this relationship has been build in previous steps or
    //  during the import
    if (_step>=EDGE2EDGES) {
        return true;
    }
    if (_connectedEdges.size()==0) {
        _connectedEdges = _to->getOutgoingEdges();
    }
    _step = EDGE2EDGES;
    return true;
}


bool
NBEdge::computeLanes2Edges()
{
    // return if this relationship has been build in previous steps or
    //  during the import
    if (_step>=LANES2EDGES) {
        return true;
    }
    assert(_step==EDGE2EDGES);
    // get list of possible outgoing edges sorted by direction clockwise
    //  the edge in the backward direction (turnaround) is not in the list
    const vector<NBEdge*> *edges = getConnectedSorted();
    // divide the lanes on reachable edges
    divideOnEdges(edges);
    delete edges;
    _step = LANES2EDGES;
    return true;
}


bool
NBEdge::recheckLanes()
{
    if (_step!=LANES2LANES) {
        // check #1:
        // If there is a lane with no connections and any neighbour lane has
        //  more than one connections, try to move one of them.
        // This check is only done for edges which connections were assigned
        //  using the standard algorithm.
        for (size_t i=0; i<_nolanes; i++) {
            assert(i<_reachable.size());
            if (_reachable[i].size()==0) {
                if (i>0&&_reachable[i-1].size()>1) {
                    moveConnectionToLeft(i-1);
                } else if (i<_reachable.size()-1&&_reachable[i+1].size()>1) {
                    moveConnectionToRight(i+1);
                }
            }
        }
    }
    // check:
    // Go through all lanes and add an empty connection if no connection
    //  is yet set.
    // This check must be done for all lanes to assert that there is at
    //  least a dead end information (needed later for building the
    //  node request
    assert(_nolanes==_reachable.size());
    for (size_t i=0; i<_nolanes; i++) {
        if (_reachable[i].size()==0) {
            setConnection(i, 0, 0, false);
        }
    }
    return true;
}


void
NBEdge::moveConnectionToLeft(size_t lane)
{
    assert(lane<_reachable.size() && _reachable[lane].size()>0);
    pair<NBEdge*, size_t> dest =
        getConnectionRemoving(lane, _reachable[lane].size()-1);
    setConnection(lane+1, dest.first, dest.second, false);
}


void
NBEdge::moveConnectionToRight(size_t lane)
{
    pair<NBEdge*, size_t> dest = getConnectionRemoving(lane, 0);
    setConnection(lane-1, dest.first, dest.second, false);
}


pair<NBEdge*, size_t>
NBEdge::getConnectionRemoving(size_t srcLane, size_t pos)
{
    assert(_reachable.size()>srcLane);
    assert(_reachable[srcLane].size()>pos);
    // get the destination edge and remove from the container
    EdgeLane edgelane = _reachable[srcLane][pos];
    _reachable[srcLane].erase(_reachable[srcLane].begin()+pos);
    // remove the information from the map of how to reach edges
    NBEdge::LanesThatSucceedEdgeCont::iterator i = _succeedinglanes.find(edgelane.edge);
    LaneVector lanes = (*i).second;
    LaneVector::iterator j = find(lanes.begin(), lanes.end(), srcLane);
    lanes.erase(j);
    _succeedinglanes[edgelane.edge] = lanes;
    // return the information
    return pair<NBEdge*, size_t>(edgelane.edge, edgelane.lane);
}


vector<size_t>
NBEdge::getConnectionLanes(NBEdge *currentOutgoing)
{
    if (currentOutgoing==_turnDestination) {
        return vector<size_t>();
    }
    map<NBEdge*, vector<size_t> >::iterator i=_ToEdges.find(currentOutgoing);
    assert(i==_ToEdges.end()||(*i).second.size()<=_nolanes);
    if (i==_ToEdges.end()) {
        return vector<size_t>();
    }
    return (*i).second;
}


const std::vector<NBEdge*> *
NBEdge::getConnectedSorted()
{
    // check whether connections exist and if not, use edges from the node
    EdgeVector outgoing = _connectedEdges;
    if (outgoing.size()==0) {
        outgoing = _to->getOutgoingEdges();
    }
    // allocate the sorted container
    size_t size = outgoing.size();
    vector<NBEdge*> *edges = new vector<NBEdge*>();
    edges->reserve(size);
    for (EdgeVector::const_iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if (outedge!=_turnDestination) {// !!!
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
    if (outgoing->size()==0) {
        return;
    }
    // precompute priorities; needed as some kind of assumptions for
    //  priorities of directions (see preparePriorities)
    vector<size_t> *priorities = preparePriorities(outgoing);

    // compute the sum of priorities (needed for normalisation)
    size_t prioSum = computePrioritySum(priorities);
    // compute the resulting number of lanes that should be used to
    //  reach the following edge
    size_t size = outgoing->size();
    vector<SUMOReal> resultingLanes;
    resultingLanes.reserve(size);
    SUMOReal sumResulting = 0; // the sum of resulting lanes
    SUMOReal minResulting = 10000; // the least number of lanes to reach an edge
    size_t i;
    for (i=0; i<size; i++) {
        // res will be the number of lanes which are meant to reach the
        //  current outgoing edge
        SUMOReal res =
            (SUMOReal)(*priorities)[i] *
            (SUMOReal) _nolanes / (SUMOReal) prioSum;
        // do not let this number be greater than the number of available lanes
        if (res>_nolanes) {
            res = (SUMOReal) _nolanes;
        }
        // add it to the list
        resultingLanes.push_back(res);
        sumResulting += res;
        if (minResulting>res) {
            minResulting = res;
        }
    }
    // compute the number of virtual edges
    //  a virtual edge is used as a replacement for a real edge from now on
    //  it shall ollow to divide the existing lanes on this structure without
    //  regarding the structure of outgoing edges
    sumResulting += minResulting / 2;
    size_t noVirtual = (size_t)(sumResulting / minResulting);
    // compute the transition from virtual to real edges
    vector<NBEdge*> transition;
    transition.reserve(size);
    for (i=0; i<size; i++) {
        // tmpNo will be the number of connections from this edge
        //  to the next edge
        assert(i<resultingLanes.size());
        SUMOReal tmpNo = (SUMOReal) resultingLanes[i] / (SUMOReal) minResulting;
        for (SUMOReal j=0; j<tmpNo; j++) {
            assert(outgoing->size()>i);
            transition.push_back((*outgoing)[i]);
        }
    }

    // assign lanes to edges
    //  (conversion from virtual to real edges is done)
    ToEdgeConnectionsAdder adder(&_ToEdges, transition);
    Bresenham::compute(&adder, (SUMOReal) _nolanes, (SUMOReal) noVirtual);
    delete priorities;
}


vector<size_t> *
NBEdge::preparePriorities(const vector<NBEdge*> *outgoing)
{
    // copy the priorities first
    vector<size_t> *priorities = new vector<size_t>();
    if (outgoing->size()==0) {
        return priorities;
    }
    priorities->reserve(outgoing->size());
    vector<NBEdge*>::const_iterator i;
    for (i=outgoing->begin(); i!=outgoing->end(); i++) {
        int prio = (*i)->getJunctionPriority(_to);
        assert((prio+1)*2>0);
        prio = (prio+1) * 2;
        priorities->push_back(prio);
    }
    // when the right turning direction has not a higher priority, divide
    //  the importance by 2 due to the possibility to leave the junction
    //  faster from this lane
    MainDirections mainDirections(*outgoing, this, _to);
    vector<NBEdge*> tmp(*outgoing);
    sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(this));
    i=find(outgoing->begin(), outgoing->end(), *(tmp.begin()));
    size_t dist = distance(outgoing->begin(), i);
    if (dist!=0&&!mainDirections.includes(MainDirections::DIR_RIGHTMOST)) {
        assert(priorities->size()>0);
        (*priorities)[0] = (*priorities)[0] / 2;
    }
    // HEURISTIC:
    // when no higher priority exists, let the forward direction be
    //  the main direction
    if (mainDirections.empty()) {
        assert(dist<priorities->size());
        (*priorities)[dist] = (*priorities)[dist] * 2;
    }
    if (mainDirections.includes(MainDirections::DIR_FORWARD)&&_nolanes>2) {
        (*priorities)[dist] = (*priorities)[dist] * 2;
    }
    // return
    return priorities;
}


size_t
NBEdge::computePrioritySum(vector<size_t> *priorities)
{
    size_t sum = 0;
    for (vector<size_t>::iterator i=priorities->begin(); i!=priorities->end(); i++) {
        sum += (*i);
    }
    return sum;
}


void
NBEdge::appendTurnaround()
{
    if (_turnDestination!=0) {
        setConnection(_nolanes-1, _turnDestination, _turnDestination->getNoLanes()-1, false);
    }
}


void
NBEdge::sortOutgoingLanesConnections()
{
    for (size_t i=0; i<_reachable.size(); i++) {
        sort(_reachable[i].begin(), _reachable[i].end(),
             NBContHelper::relative_edgelane_sorter(this, _to));
    }
}



void
NBEdge::setConnection(size_t src_lane, NBEdge *dest_edge,
                      size_t dest_lane, bool markAs2Recheck,
                      bool mayUseSameDestination)
{
    if (_step==INIT_REJECT_CONNECTIONS) {
        return;
    }
    assert(dest_lane<=10);
    assert(src_lane<=10);
    // some kind of a misbehaviour which may occure when the junction's outgoing
    //  edge priorities were not properly computed, what may happen due to
    //  an incomplete or not proper input
    // what happens is that under some circumstances a single lane may set to
    //  be approached by more than once by the one of our lanes.
    //  This must not be!
    // we test whether it is the case and do nothing if so - the connection
    //  will be refused
    //
    EdgeLane el;
    el.edge = dest_edge;
    el.lane = dest_lane;
    if(!mayUseSameDestination) {
        for (size_t j=0; dest_edge!=0&&j<_reachable.size(); j++) {
            // for any other lane: check whether a connection to the same
            //  lane as the one to be added exists
            EdgeLaneVector &tmp = _reachable[j];
            if (find(tmp.begin(), tmp.end(), el)!=tmp.end()) {
                return;
            }
        }
    }

    // this connection has not yet been set
    bool known = false;
    // find the entry which holds the information which lanes may
    //  be used to reach the current destination edge
    LanesThatSucceedEdgeCont::iterator i = _succeedinglanes.find(dest_edge);
    // if there is no such connection yet, build it
    if (i==_succeedinglanes.end()) {
        LaneVector lanes;
        lanes.push_back(src_lane);
        _succeedinglanes[dest_edge] = lanes;
        // else
    } else {
        LaneVector lanes = (*i).second;
        // check whether one of the known connections to the current destination
        //  comes from the same lane as the current to add
        LaneVector::iterator i = find(lanes.begin(), lanes.end(), src_lane);
        // if not, append
        if (i==lanes.end())
            lanes.push_back(src_lane);
        _succeedinglanes[dest_edge] = lanes;
    }
    // append current connection only if no equal is already known
    if (!known) {
        if (_reachable.size()<=src_lane) {
            MsgHandler::getErrorInstance()->inform("Could not set connection from '" + _id + "_" + toString(src_lane) + "' to '" + dest_edge->getID() + "_" + toString(dest_lane) + "'.");
            return;
        }
        assert(_reachable.size()>src_lane);
        _reachable[src_lane].push_back(el);
    }
    // check whether we have to take another look at it later
    if (markAs2Recheck) {
        // yes, the connection was set using an algorithm which requires a recheck
        _step = LANES2LANES_RECHECK;
    } else {
        // ok, let's only not recheck it if we did no add something that has to be recheked
        if (_step!=LANES2LANES_RECHECK) {
            _step = LANES2LANES;
        }
    }
}


bool
NBEdge::isTurningDirectionAt(const NBNode *n, NBEdge *edge) const
{
    // maybe it was already set as the turning direction
    if (edge == _turnDestination) {
        return true;
    } else if (_turnDestination!=0) {
        // otherwise - it's not if a turning direction exists
        return false;
    }
    // if the same nodes are connected
    if (_from==edge->_to && _to==edge->_from) {
        return true;
    }
    // we have to checke whether the connection between the nodes is
    //  geometrically similar
    SUMOReal thisFromAngle2 = getAngle(*n);
    SUMOReal otherToAngle2 = edge->getAngle(*n);
    if (thisFromAngle2<otherToAngle2) {
        swap(thisFromAngle2, otherToAngle2);
    }
    if (thisFromAngle2-otherToAngle2>170&&thisFromAngle2-otherToAngle2<190) {
        return true;
    }
    return false;
}



bool
NBEdge::isAlmostSameDirectionAt(const NBNode *n, NBEdge *edge) const
{
    // if the same nodes are connected
    if (_to==edge->_to && _from==edge->_from) {
        return true;
    }
    // we have to checke whether the connection between the nodes is
    //  geometrically similar
    SUMOReal thisFromAngle2 = getAngle(*n);
    SUMOReal otherToAngle2 = edge->getAngle(*n);
    if (thisFromAngle2<otherToAngle2) {
        swap(thisFromAngle2, otherToAngle2);
    }
    if (thisFromAngle2-otherToAngle2<20&&thisFromAngle2-otherToAngle2>-20) {
        return true;
    }
    return false;
}



NBNode *
NBEdge::tryGetNodeAtPosition(SUMOReal pos, SUMOReal tolerance) const
{
    // return the from-node when the position is at the begin of the edge
    if (pos<tolerance) {
        return _from;
    }
    // return the to-node when the position is at the end of the edge
    if (pos>_length-tolerance) {
        return _to;
    }
    return 0;
}


NBEdge::EdgeBasicFunction
NBEdge::getBasicType() const
{
    return _basicType;
}


SUMOReal
NBEdge::getSpeed() const
{
    return _speed;
}


void
NBEdge::replaceInConnections(NBEdge *which, NBEdge *by, size_t laneOff)
{
    // replace in "_connectedEdges"
    EdgeVector::iterator k = find(
                                 _connectedEdges.begin(), _connectedEdges.end(), by);
    if (k!=_connectedEdges.end()) {
        for (k=_connectedEdges.begin(); k!=_connectedEdges.end();) {
            if ((*k)==which) {
                k = _connectedEdges.erase(k);
            } else {
                k++;
            }
        }
    } else {
        for (size_t i=0; i<_connectedEdges.size(); i++) {
            if (_connectedEdges[i]==which) {
                _connectedEdges[i] = by;
            }
        }
    }
    // check whether it was the turn destination
    if (_turnDestination==which) {
        _turnDestination = by;
    }
    // replace in _ToEdges
    {
        // check if the edge to replace by was already connected
        bool have = _ToEdges.find(by)!=_ToEdges.end();
        // find the edge to replace
        std::map<NBEdge*, std::vector<size_t> >::iterator j = _ToEdges.find(which);
        if (j!=_ToEdges.end()) {
            // if the edge to replace by already had a connection
            if (have) {
                // add further connections
                for (std::vector<size_t>::iterator k=_ToEdges[which].begin(); k!=_ToEdges[which].end(); k++) {
                    if (find(_ToEdges[which].begin(), _ToEdges[which].end(), (*k)+laneOff)!=_ToEdges[which].end()) {
                        _ToEdges[by].push_back((*k)+laneOff);
                    }
                }
            } else {
                // set connections
                have = true;
                _ToEdges[by] = (*j).second;
            }
        }
    }
    // replace in _reachable
    {
        for (ReachableFromLaneVector::iterator k=_reachable.begin(); k!=_reachable.end(); k++) {
            for (EdgeLaneVector::iterator l=(*k).begin(); l!=(*k).end(); l++) {
                if ((*l).edge==which) {
                    (*l).edge = by;
                    (*l).lane = (*l).lane + laneOff;
                }
            }
        }
    }
    // replace in _succeedinglanes
    {
        LanesThatSucceedEdgeCont::iterator l=_succeedinglanes.find(which);
        if (l!=_succeedinglanes.end()) {
            LanesThatSucceedEdgeCont::iterator l2=_succeedinglanes.find(by);
            if (l2!=_succeedinglanes.end()) {
                copy(_succeedinglanes[which].begin(), _succeedinglanes[which].end(),
                     back_inserter(_succeedinglanes[by]));
            } else {
                _succeedinglanes[by] = (*l).second;
            }
            _succeedinglanes.erase(l);
        }
    }
}


void
NBEdge::moveOutgoingConnectionsFrom(NBEdge *e, size_t laneOff,
                                    bool markAs2Recheck)
{
    size_t lanes = e->getNoLanes();
    for (size_t i=0; i<lanes; i++) {
        const EdgeLaneVector &elv = e->getEdgeLanesFromLane(i);
        for (EdgeLaneVector::const_iterator j=elv.begin(); j!=elv.end(); j++) {
            EdgeLane el = (*j);
            assert(el.tlID=="");
            bool ok = addLane2LaneConnection(i+laneOff, el.edge, el.lane, markAs2Recheck);
            assert(ok);
        }
    }
}


bool
NBEdge::isConnectedTo(NBEdge *e)
{
    return
        find(_connectedEdges.begin(), _connectedEdges.end(), e)
        !=
        _connectedEdges.end();

}


void
NBEdge::remapConnections(const EdgeVector &incoming)
{
    for (EdgeVector::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        NBEdge *inc = *i;
        // We have to do this
        inc->_step = EDGE2EDGES;
        // add all connections
        for (EdgeVector::iterator j=_connectedEdges.begin(); j!=_connectedEdges.end(); j++) {
            inc->addEdge2EdgeConnection(*j);
        }
        inc->removeFromConnections(this);
    }
}


void
NBEdge::removeFromConnections(NBEdge *which)
{
    // remove from "_connectedEdges"
    for (size_t i=0; i<_connectedEdges.size(); i++) {
        if (_connectedEdges[i]==which) {
            _connectedEdges.erase(_connectedEdges.begin()+i);
            i--;
        }
    }
    // check whether it was the turn destination
    if (_turnDestination==which) {
        _turnDestination = 0;
    }
    // remove in _ToEdges
    {
        std::map<NBEdge*, std::vector<size_t> >::iterator j = _ToEdges.find(which);
        if (j!=_ToEdges.end()) {
            _ToEdges.erase(which);
        }
    }
    // remove in _reachable
    {
        for (ReachableFromLaneVector::iterator k=_reachable.begin(); k!=_reachable.end(); k++) {
            EdgeLaneVector::iterator l=(*k).begin();
            while (l!=(*k).end()) {
                if ((*l).edge==which) {
                    (*k).erase(l);
                    l = (*k).begin();
                } else {
                    l++;
                }
            }
        }
    }
    // remove in _succeedinglanes
    {
        LanesThatSucceedEdgeCont::iterator l=_succeedinglanes.find(which);
        if (l!=_succeedinglanes.end()) {
            _succeedinglanes.erase(l);
        }
    }
}


void
NBEdge::invalidateConnections(bool reallowSetting)
{
    _turnDestination = 0;
    _ToEdges.clear();
    _reachable.clear();
    _reachable.resize(_nolanes, EdgeLaneVector());
    _succeedinglanes.clear();
    if (reallowSetting) {
        _step = INIT;
    } else {
        _step = INIT_REJECT_CONNECTIONS;
    }
}


const EdgeVector &
NBEdge::getConnected() const
{
    return _connectedEdges;
}



bool
NBEdge::lanesWereAssigned() const
{
    return _step==LANES2LANES;
}


EdgeVector
NBEdge::getEdgesFromLane(size_t lane) const
{
    assert(lane<_reachable.size());
    EdgeVector ret;
    for (EdgeLaneVector::const_iterator i=_reachable[lane].begin(); i!=_reachable[lane].end(); i++) {
        ret.push_back((*i).edge);
    }
    return ret;
}

const Position2DVector &
NBEdge::getGeometry() const
{
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in getGeometry" << endl;
        throw 1;
    }
#endif
#endif
    return myGeom;
}


void
NBEdge::setGeometry(const Position2DVector &s)
{
    myGeom = s;
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in setGeometry" << endl;
        throw 1;
    }
#endif
#endif
    computeLaneShapes();
}


SUMOReal
NBEdge::getMaxLaneOffset()
{
    return (SUMOReal) SUMO_const_laneWidthAndOffset * _nolanes;
}


Position2D
NBEdge::getMinLaneOffsetPositionAt(NBNode *node, SUMOReal width)
{
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myLaneGeoms[0].assertNonEqual()) {
        DEBUG_OUT << getID() << "in minlaneoffset" << endl;
        throw 1;
    }
#endif
#endif
    width = width < myLaneGeoms[0].length()/(SUMOReal) 2.0
            ? width
            : myLaneGeoms[0].length()/(SUMOReal) 2.0;
    if (node==_from) {
        Position2D pos =
            myLaneGeoms[myLaneGeoms.size()-1].positionAtLengthPosition(width);
        GeomHelper::transfer_to_side(pos,
                                     myLaneGeoms[myLaneGeoms.size()-1][0], myLaneGeoms[myLaneGeoms.size()-1][myLaneGeoms[0].size()-1], // !!!!
                                     SUMO_const_halfLaneAndOffset);
        return pos;
    } else {
        Position2D pos =
            myLaneGeoms[0].positionAtLengthPosition(myLaneGeoms[0].length() - width);
        GeomHelper::transfer_to_side(pos,
                                     myLaneGeoms[0][myLaneGeoms[0].size()-1], myLaneGeoms[0][0],
                                     SUMO_const_halfLaneAndOffset);
        return pos;
    }
}


Position2D
NBEdge::getMaxLaneOffsetPositionAt(NBNode *node, SUMOReal width)
{
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myLaneGeoms[0].assertNonEqual()) {
        DEBUG_OUT << getID() << "in getmaxlane" << endl;
        throw 1;
    }
#endif
#endif
    width = width < myLaneGeoms[0].length()/(SUMOReal) 2.0
            ? width
            : myLaneGeoms[0].length()/(SUMOReal) 2.0;
    if (node==_from) {
        Position2D pos = myLaneGeoms[0].positionAtLengthPosition(width);
        GeomHelper::transfer_to_side(pos,
                                     myLaneGeoms[0][0], myLaneGeoms[0][myLaneGeoms[0].size()-1],
                                     -SUMO_const_halfLaneAndOffset);
        return pos;
    } else {
        Position2D pos = myLaneGeoms[myLaneGeoms.size()-1].positionAtLengthPosition(myLaneGeoms[myLaneGeoms.size()-1].length() - width);
        GeomHelper::transfer_to_side(pos,
                                     myLaneGeoms[myLaneGeoms.size()-1][myLaneGeoms[myLaneGeoms.size()-1].size()-1], myLaneGeoms[myLaneGeoms.size()-1][0],
                                     -SUMO_const_halfLaneAndOffset);
        return pos;
    }
}


void
NBEdge::setControllingTLInformation(int fromLane, NBEdge *toEdge, int toLane,
                                    const std::string &tlID, size_t tlPos)
{
    assert(fromLane<0||fromLane<(int) _nolanes);
    // try to use information about the connections if given
    if (fromLane>=0&&toLane>=0) {
        // get the connections outgoing from this lane
        EdgeLaneVector &connections = _reachable[fromLane];
        // find the specified connection
        EdgeLaneVector::iterator i =
            find_if(connections.begin(), connections.end(),
                    NBContHelper::edgelane_finder(toEdge, toLane));
        // ok, we have to test this as on the removal of dummy edges some connections
        //  will be reassigned
        if (i!=connections.end()) {
            // get the connection
            EdgeLane &connection = *i;
            // set the information about the tl
            connection.tlID = tlID;
            connection.tlLinkNo = tlPos;
            return;
        }
    }
    // if the original connection was not found, set the information for all
    //  connections
    size_t no = 0;
    bool hadError = false;
    for (size_t j=0; j<_nolanes; j++) {
        EdgeLaneVector &connections = _reachable[j];
        EdgeLaneVector::iterator i =
            find_if(connections.begin(), connections.end(),
                    NBContHelper::edgelane_finder(toEdge, toLane));
        while (i!=connections.end()) {
            // get the connection
            EdgeLane &connection = *i;
            // set the information about the tl
            //  but check first, if it was not set before
            if (connection.tlID=="") {
                connection.tlID = tlID;
                connection.tlLinkNo = tlPos;
                no++;
            } else {
                if (connection.tlID!=tlID&&connection.tlLinkNo==tlPos) {
                    WRITE_WARNING("The lane " + toString<int>(connection.lane)+ " on edge " + connection.edge->getID()+ " already had a traffic light signal.");
                    hadError = true;
                }
            }
            i = find_if(i+1, connections.end(),
                        NBContHelper::edgelane_finder(toEdge, toLane));
        }
    }
    if (hadError&&no==0) {
        WRITE_WARNING("Could not set any signal of the traffic light '" + tlID + "' (unknown group)");
    }
}


void
NBEdge::normalisePosition(const NBNodeCont &nc)
{
    myGeom.resetBy(GeoConvHelper::getOffset());
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in normalise2" << endl;
        throw 1;
    }
#endif
#endif
    for (size_t i=0; i<_nolanes; i++) {
        myLaneGeoms[i].resetBy(GeoConvHelper::getOffset());
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[i].assertNonEqual()) {
            DEBUG_OUT << getID() << "in normalise" << endl;
            throw 1;
        }
#endif
#endif
    }
}


void
NBEdge::reshiftPosition(SUMOReal xoff, SUMOReal yoff, SUMOReal rot)
{
    myGeom.reshiftRotate(xoff, yoff, rot);
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in reshift" << endl;
        throw 1;
    }
#endif
#endif
    for (size_t i=0; i<_nolanes; i++) {
        myLaneGeoms[i].reshiftRotate(xoff, yoff, rot);
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[i].assertNonEqual()) {
            DEBUG_OUT << getID() << "in reshift2" << endl;
            throw 1;
        }
#endif
#endif
    }
}


Position2DVector
NBEdge::getCWBoundaryLine(const NBNode &n, SUMOReal offset) const
{
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    for (size_t bla=0; bla<_nolanes; bla++) {
        if (!myLaneGeoms[bla].assertNonEqual()) {
            DEBUG_OUT << getID() << endl;
            DEBUG_OUT << myLaneGeoms[bla] << endl;
            throw 1;
        }
    }
#endif
#endif
    Position2DVector ret;
    if (_from==(&n)) {
        // outgoing
        ret = myLaneGeoms[0];
    } else {
        // incoming
        ret = myLaneGeoms[getNoLanes()-1].reverse();
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!ret.assertNonEqual()) {
        DEBUG_OUT << getID() << "in cwBoundary" << endl;
        throw 1;
    }
#endif
#endif
    ret.move2side(offset);
    return ret;
}


Position2DVector
NBEdge::getCCWBoundaryLine(const NBNode &n, SUMOReal offset) const
{
    Position2DVector ret;
    if (_from==(&n)) {
        // outgoing
        ret = myLaneGeoms[getNoLanes()-1];
    } else {
        // incoming
        ret = myLaneGeoms[0].reverse();
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!ret.assertNonEqual()) {
        DEBUG_OUT << getID() << "in ccwboundary" << endl;
        throw 1;
    }
#endif
#endif
    ret.move2side(-offset);
    return ret;
}


SUMOReal
NBEdge::width() const
{
    return (SUMOReal) _nolanes * SUMO_const_laneWidth + (SUMOReal)(_nolanes-1) * SUMO_const_laneOffset;
}


bool
NBEdge::expandableBy(NBEdge *possContinuation) const
{
    // ok, the number of lanes must match
    if (_nolanes!=possContinuation->_nolanes) {
        return false;
    }
    // the priority, too (?)
    if (getPriority()!=possContinuation->getPriority()) {
        return false;
    }
    // the speed allowed
    if (_speed!=possContinuation->_speed) {
        return false;
    }
    // the vehicle class constraints, too
    if (myAllowedOnLanes!=possContinuation->myAllowedOnLanes
            ||
            myNotAllowedOnLanes!=possContinuation->myNotAllowedOnLanes) {
        return false;
    }
    // the next is quite too conservative here, but seems to work
    if (_basicType!=EDGEFUNCTION_NORMAL
            &&
            possContinuation->_basicType!=EDGEFUNCTION_NORMAL) {

        return false;
    }
    // also, check whether the connections - if any exit do allow to join
    //  both edges
    // This edge must have a one-to-one connection to the following lanes
    switch (_step) {
    case INIT_REJECT_CONNECTIONS:
        break;
    case INIT:
        break;
    case EDGE2EDGES: {
        // the following edge must be connected
        const EdgeVector &conn = getConnected();
        if (find(conn.begin(), conn.end(), possContinuation)
                ==conn.end()) {

            return false;
        }
    }
    break;
    case LANES2EDGES: {
        // the possible continuation must be connected
        if (_ToEdges.find(possContinuation)==_ToEdges.end()) {
            return false;
        }
        // all lanes must go to the possible continuation
        const std::vector<size_t> &lanes =
            (*(_ToEdges.find(possContinuation))).second;
        if (lanes.size()!=_nolanes) {
            return false;
        }
    }
    break;
    case LANES2LANES: {
        for (size_t i=0; i<_nolanes; i++) {
            const EdgeLaneVector &elv = getEdgeLanesFromLane(i);
            bool found = false;
            for (EdgeLaneVector::const_iterator j=elv.begin(); j!=elv.end(); j++) {
                if ((*j).edge==possContinuation&&(*j).lane==i) {
                    found = true;
                    j = elv.end()-1;
                }
            }
            if (!found) {
                return false;
            }
        }
    }
    break;
    default:
        break;
    }
    return true;
}


void
NBEdge::append(NBEdge *e)
{
    // append geometry
    myGeom.appendWithCrossingPoint(e->myGeom);
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in append" << endl;
        throw 1;
    }
#endif
#endif
    for (size_t i=0; i<_nolanes; i++) {
        myLaneGeoms[i].appendWithCrossingPoint(e->myLaneGeoms[i]);
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[i].assertNonEqual()) {
            DEBUG_OUT << getID() << "in append2" << endl;
            throw 1;
        }
#endif
#endif
    }
    // recompute length
    _length += e->_length;
    // copy the connections and the building step if given
    _step = e->_step;
    _connectedEdges = e->_connectedEdges;
    _reachable = (e->_reachable);
    _succeedinglanes = e->_succeedinglanes;
    _ToEdges = e->_ToEdges;
    _turnDestination = e->_turnDestination;
    // set the node
    _to = e->_to;
}


void
NBEdge::computeEdgeShape()
{
    size_t i;
    for (i=0; i<_nolanes; i++) {
        // get lane begin and end
        Line2D lb = Line2D(myLaneGeoms[i][0], myLaneGeoms[i][1]);
        Line2D le = Line2D(myLaneGeoms[i][-1], myLaneGeoms[i][-2]);
        lb.extrapolateBy(100.0);
        le.extrapolateBy(100.0);
        //
        Position2DVector old = myLaneGeoms[i];
        Position2D nb, ne;
        // lane begin
        if (_from->getShape().intersects(myLaneGeoms[i])) {
            // get the intersection position with the junction
            DoubleVector pbv = myLaneGeoms[i].intersectsAtLengths(_from->getShape());
            if (pbv.size()>0) {
                SUMOReal pb = DoubleVectorHelper::maxValue(pbv);
                if (pb>=0&&pb<=myLaneGeoms[i].length()) {
                    myLaneGeoms[i] = myLaneGeoms[i].getSubpart(pb, myLaneGeoms[i].length());
                }
            }
        } else if (_from->getShape().intersects(lb.p1(), lb.p2())) {
            DoubleVector pbv = lb.intersectsAtLengths(_from->getShape());
            if (pbv.size()>0) {
                SUMOReal pb = DoubleVectorHelper::maxValue(pbv);
                if (pb>=0) {
                    myLaneGeoms[i].eraseAt(0);
                    myLaneGeoms[i].push_front_noDoublePos(lb.getPositionAtDistance(pb));
                }
            }
        }
        // lane end
        if (_to->getShape().intersects(myLaneGeoms[i])) {
            // get the intersection position with the junction
            DoubleVector pev = myLaneGeoms[i].intersectsAtLengths(_to->getShape());
            if (pev.size()>0) {
                SUMOReal pe = DoubleVectorHelper::minValue(pev);
                if (pe>=0&&pe<=myLaneGeoms[i].length()) {
                    myLaneGeoms[i] = myLaneGeoms[i].getSubpart(0, pe);
                }
            }
        } else if (_to->getShape().intersects(le.p1(), le.p2())) {
            DoubleVector pev = le.intersectsAtLengths(_to->getShape());
            if (pev.size()>0) {
                SUMOReal pe = DoubleVectorHelper::maxValue(pev);
                if (pe>=0) {
                    myLaneGeoms[i].eraseAt(myLaneGeoms[i].size()-1);
                    myLaneGeoms[i].push_back_noDoublePos(le.getPositionAtDistance(pe));
                }
            }
        }
        if (((int) myLaneGeoms[i].length())==0) {
            myLaneGeoms[i] = old;
        }
    }
    // recompute edge's length
    if (_basicType==EDGEFUNCTION_SOURCE||_basicType==EDGEFUNCTION_SINK) {
        return;
    }
    SUMOReal length = 0;
    for (i=0; i<_nolanes; i++) {
        assert(myLaneGeoms[i].length()>0);
        length += myLaneGeoms[i].length();
    }
    _length = length / (SUMOReal) _nolanes;
}


bool
NBEdge::hasSignalisedConnectionTo(NBEdge *e) const
{
    for (size_t i=0; i<_nolanes; i++) {
        // get the connections outgoing from this lane
        const EdgeLaneVector &connections = _reachable[i];
        // find the specified connection
        for (EdgeLaneVector::const_iterator j=connections.begin(); j!=connections.end(); j++) {
            // get the connection
            const EdgeLane &connection = *j;
            if (connection.edge==e&&connection.tlID!="") {
                return true;
            }
        }
    }
    return false;
}


NBEdge*
NBEdge::getTurnDestination() const
{
    return _turnDestination;
}


std::string
NBEdge::getLaneID(size_t lane)
{
    assert(lane<_nolanes);
    return _id + "_" + toString<size_t>(lane);
}


bool
NBEdge::isNearEnough2BeJoined2(NBEdge *e)
{
    DoubleVector distances = myGeom.distances(e->getGeometry());
    if (distances.size()==0) {
        distances = e->getGeometry().distancesExt(myGeom);
    }
    SUMOReal max = DoubleVectorHelper::maxValue(distances);
    return max<7;
}


SUMOReal
NBEdge::getNormedAngle(const NBNode &atNode) const
{
    SUMOReal angle = getAngle(atNode);
    if (angle<0) {
        angle = 360 + angle;
    }
    assert(angle>=0&&angle<360);
    return angle;
}


SUMOReal
NBEdge::getNormedAngle() const
{
    SUMOReal angle = _angle;
    if (angle<0) {
        angle = 360 + angle;
    }
    assert(angle>=0&&angle<360);
    return angle;
}


void
NBEdge::addGeometryPoint(int index, const Position2D &p)
{
    myGeom.insertAt(index, p);
}


void
NBEdge::incLaneNo(int by)
{
    _nolanes += by;
    _reachable.clear();
    _reachable.resize(_nolanes, EdgeLaneVector());
    while (myLaneSpeeds.size()<_nolanes) {
        myLaneSpeeds.push_back(_speed);
        myAllowedOnLanes.push_back(std::vector<SUMOVehicleClass>());
        myNotAllowedOnLanes.push_back(std::vector<SUMOVehicleClass>());
    }
    computeLaneShapes();
    const EdgeVector &incs = _from->getIncomingEdges();
    for (EdgeVector::const_iterator i=incs.begin(); i!=incs.end(); ++i) {
        (*i)->invalidateConnections(true);
    }
    invalidateConnections(true);
}


void
NBEdge::decLaneNo(int by)
{
    _nolanes -= by;
    _reachable.clear();
    _reachable.resize(_nolanes, EdgeLaneVector());
    while (myLaneSpeeds.size()>_nolanes) {
        myLaneSpeeds.pop_back();
        myAllowedOnLanes.pop_back();
        myNotAllowedOnLanes.pop_back();
    }
    computeLaneShapes();
    const EdgeVector &incs = _from->getIncomingEdges();
    for (EdgeVector::const_iterator i=incs.begin(); i!=incs.end(); ++i) {
        (*i)->invalidateConnections(true);
    }
    invalidateConnections(true);
}


void
NBEdge::copyConnectionsFrom(NBEdge *src)
{
    _step = src->_step;
    _connectedEdges = src->_connectedEdges;
    _ToEdges = src->_ToEdges;
    for (size_t i=0; i<src->_reachable.size(); i++) {
        _reachable[i] = src->_reachable[i];
    }
    _succeedinglanes = src->_succeedinglanes;
    assert(_reachable.size()==_nolanes);
}


void
NBEdge::setLaneSpreadFunction(LaneSpreadFunction spread)
{
    myLaneSpreadFunction = spread;
}


NBEdge::LaneSpreadFunction
NBEdge::getLaneSpreadFunction() const
{
    return myLaneSpreadFunction;
}


void
NBEdge::markAsInLane2LaneState()
{
    assert(_to->getOutgoingEdges().size()==0);
    _step = LANES2LANES;
}


bool
NBEdge::splitGeometry(NBEdgeCont &ec, NBNodeCont &nc)
{
    // check whether there any splits to perform
    if (myGeom.size()<3) {
        return false;
    }
    // ok, split
    NBNode *newFrom = _from;
    NBNode *myLastNode = _to;
    NBNode *newTo = 0;
    NBEdge *currentEdge = this;
    for (size_t i=1; i<myGeom.size()-1; i++) {
        // build the node first
        if (i!=myGeom.size()-2) {
            string nodename = _id + "_in_between#" + toString(i);
            if (!nc.insert(nodename, myGeom[i])) {
                MsgHandler::getErrorInstance()->inform("Error on adding in-between node '" + nodename + "'.");
                throw ProcessError();
            }
            newTo = nc.retrieve(nodename);
        } else {
            newTo = myLastNode;
        }
        if (i==1) {
            currentEdge->_to->removeIncoming(this);
            currentEdge->_to = newTo;
            newTo->addIncomingEdge(currentEdge);
        } else {
            string edgename = _id + "[" + toString(i-1) + "]";
            currentEdge = new NBEdge(edgename, edgename, newFrom, newTo, _type, _speed, _nolanes,
                                     -1, _priority, myLaneSpreadFunction, _basicType);
            if (!ec.insert(currentEdge)) {
                MsgHandler::getErrorInstance()->inform("Error on adding splitted edge '" + edgename + "'.");
                throw ProcessError();
            }
        }
        newFrom = newTo;
    }
    //currentEdge->copyConnectionsFrom(this);
    myGeom.clear();
    myGeom.push_back(_from->getPosition());
    myGeom.push_back(_to->getPosition());
    //invalidateConnections();
    _step = INIT;
    return true;
}


void
NBEdge::allowVehicleClass(int lane, SUMOVehicleClass vclass)
{
    if (OptionsSubSys::getOptions().getBool("dismiss-vclasses")) {
        return;
    }
    if (lane<0) {
        // if all lanes are meant...
        for (size_t i=0; i<_nolanes; i++) {
            // ... do it for each lane
            allowVehicleClass((int) i, vclass);
        }
        return;
    }
    assert(lane<(int) _nolanes);
    assert(lane<(int) myAllowedOnLanes.size());
    // add it only if not already done
    if (find(myAllowedOnLanes[lane].begin(), myAllowedOnLanes[lane].end(), vclass)==myAllowedOnLanes[lane].end()) {
        myAllowedOnLanes[lane].push_back(vclass);
    }
}


void
NBEdge::disallowVehicleClass(int lane, SUMOVehicleClass vclass)
{
    if (OptionsSubSys::getOptions().getBool("dismiss-vclasses")) {
        return;
    }
    if (lane<0) {
        // if all lanes are meant...
        for (size_t i=0; i<_nolanes; i++) {
            // ... do it for each lane
            disallowVehicleClass((int) i, vclass);
        }
        return;
    }
    assert(lane<(int) _nolanes);
    assert(lane<(int) myAllowedOnLanes.size());
    // add it only if not already done
    if (find(myNotAllowedOnLanes[lane].begin(), myNotAllowedOnLanes[lane].end(), vclass)==myNotAllowedOnLanes[lane].end()) {
        myNotAllowedOnLanes[lane].push_back(vclass);
    }
}


void
NBEdge::recheckEdgeGeomForDoublePositions()
{
    myGeom.removeDoublePoints();
}


void
NBEdge::addAdditionalConnections()
{
    // go through the lanes
    for (size_t i=0; i<_nolanes; i++) {
        // get the connections from the current lane
        EdgeLaneVector reachableFromLane = _reachable[i];
        // go through these connections
        for (size_t j=0; j<reachableFromLane.size(); ++j) {
            // get the current connection
            const EdgeLane &el = reachableFromLane[j];
            // get all connections that approach the current destination edge
            const std::vector<size_t> &other = _ToEdges[el.edge];
            // check whether we may add a connection on the right side
            //  we may not do this directly because the containers will change
            //  during the addition
            bool mayAddRight = false;
            bool mayAddRight2 = false;
            bool mayAddLeft = false;
            bool mayAddLeft2 = false;
            if (i!=0 && el.lane!=0 && find(other.begin(), other.end(), i-1)==other.end()) {
                mayAddRight = true;
            }
            if (i==0 && el.lane!=0) {
                mayAddRight2 = true;
            }
            if (i+1<_nolanes && el.lane+1<el.edge->getNoLanes() && find(other.begin(), other.end(), i+1)==other.end()) {
                mayAddLeft = true;
            }
            if (i+1==_nolanes && el.lane+1<el.edge->getNoLanes()) {
                mayAddLeft2 = true;
            }
            // add the connections if possible
            if (mayAddLeft) {
                addLane2LaneConnection(i+1, el.edge, el.lane+1, false);
            }
            if (mayAddLeft2) {
                addLane2LaneConnection(i, el.edge, el.lane+1, false);
            }
            reachableFromLane = _reachable[i];
        }
    }
}


std::vector<SUMOVehicleClass>
NBEdge::getAllowedVehicleClasses() const
{
    std::vector<SUMOVehicleClass> ret;
    for (std::vector<std::vector<SUMOVehicleClass> >::const_iterator i=myAllowedOnLanes.begin(); i!=myAllowedOnLanes.end(); ++i) {
        for (std::vector<SUMOVehicleClass>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            if (find(ret.begin(), ret.end(), *j)==ret.end()) {
                ret.push_back(*j);
            }
        }
    }
    return ret;
}


std::vector<SUMOVehicleClass>
NBEdge::getNotAllowedVehicleClasses() const
{
    std::vector<SUMOVehicleClass> ret;
    for (std::vector<std::vector<SUMOVehicleClass> >::const_iterator i=myNotAllowedOnLanes.begin(); i!=myNotAllowedOnLanes.end(); ++i) {
        for (std::vector<SUMOVehicleClass>::const_iterator j=(*i).begin(); j!=(*i).end(); ++j) {
            if (find(ret.begin(), ret.end(), *j)==ret.end()) {
                ret.push_back(*j);
            }
        }
    }
    return ret;
}



/****************************************************************************/

