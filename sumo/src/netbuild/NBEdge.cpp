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
// Revision 1.54  2005/10/07 11:38:18  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.53  2005/09/23 06:01:05  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.52  2005/09/15 12:02:45  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.51  2005/07/12 12:32:46  dkrajzew
// code style adapted; guessing of ramps and unregulated near districts implemented; debugging
//
// Revision 1.50  2005/04/27 11:48:25  dkrajzew
// level3 warnings removed; made containers non-static
//
// Revision 1.49  2005/01/27 14:26:08  dkrajzew
// patched several problems on determination of the turning direction; code beautifying
//
// Revision 1.48  2004/12/20 14:00:24  dkrajzew
// debugging
//
// Revision 1.47  2004/11/23 10:21:40  dkrajzew
// debugging
//
// Revision 1.46  2004/08/02 13:11:39  dkrajzew
// made some deprovements or so
//
// Revision 1.45  2004/07/02 09:30:10  dkrajzew
// some work on geometry
//
// Revision 1.44  2004/03/19 13:06:09  dkrajzew
// some further work on vissim-import and geometry computation
//
// Revision 1.43  2004/02/16 13:58:22  dkrajzew
// some further work on edge geometry
//
// Revision 1.42  2004/01/28 12:40:23  dkrajzew
// added to possibility to give each lane a speed
//
// Revision 1.41  2004/01/26 09:53:28  dkrajzew
// cedges are now written in counter-clockwise order
//
// Revision 1.40  2004/01/26 07:05:08  dkrajzew
// problems on occurence of two backward edges patched
//
// Revision 1.39  2004/01/12 15:25:09  dkrajzew
// node-building classes are now lying in an own folder
//
// Revision 1.38  2004/01/12 15:10:27  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.37  2003/12/05 14:58:03  dkrajzew
// loaded connections are now saved on edge joining
//
// Revision 1.36  2003/12/04 13:06:45  dkrajzew
// work on internal lanes
//
// Revision 1.35  2003/11/20 13:13:43  dkrajzew
// obsolete saving of a lanes geometry twice removed
//
// Revision 1.34  2003/11/18 14:25:24  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.33  2003/11/17 07:26:01  dkrajzew
// computations needed for collecting e2-values over multiple lanes added
//
// Revision 1.32  2003/11/11 08:33:54  dkrajzew
// consequent position2D instead of two SUMOReals added
//
// Revision 1.31  2003/10/28 09:47:28  dkrajzew
// lane2lane connections are now kept when edges are joined
//
// Revision 1.30  2003/10/17 06:48:06  dkrajzew
// length of sources and sinks set to 200m
//
// Revision 1.29  2003/10/15 11:45:38  dkrajzew
// geometry computation corrigued partially
//
// Revision 1.28  2003/10/06 07:46:12  dkrajzew
// further work on vissim import (unsignalised vs. signalised streams modality
//  cleared & lane2lane instead of edge2edge-prohibitions implemented
//
// Revision 1.27  2003/09/22 12:40:11  dkrajzew
// further work on vissim-import
//
// Revision 1.26  2003/09/05 15:16:57  dkrajzew
// umlaute conversion; node geometry computation; internal links computation
//
// Revision 1.25  2003/08/21 12:55:20  dkrajzew
// directional information patched
//
// Revision 1.24  2003/08/20 11:53:17  dkrajzew
// further work on node geometry
//
// Revision 1.23  2003/08/14 13:50:15  dkrajzew
// new junction shape computation implemented
//
// Revision 1.22  2003/07/30 09:21:11  dkrajzew
// added the generation about link directions and priority
//
// Revision 1.21  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.20  2003/07/17 12:16:25  dkrajzew
// gcc seems to be unable to temporary convert a value to a reference; patched
//
// Revision 1.19  2003/07/16 15:32:02  dkrajzew
// some work on the geometry of nodes
//
// Revision 1.18  2003/07/07 08:22:42  dkrajzew
// some further refinements due to the new 1:N traffic lights and usage of
//  geometry information
//
// Revision 1.17  2003/06/18 11:13:13  dkrajzew
// new message and error processing: output to user may be a message, warning
//  or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
// changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.16  2003/06/16 08:02:44  dkrajzew
// further work on Vissim-import
//
// Revision 1.15  2003/06/05 11:43:34  dkrajzew
// class templates applied; documentation added
//
// Revision 1.14  2003/05/21 15:18:19  dkrajzew
// yellow traffic lights implemented
//
// Revision 1.13  2003/05/20 09:33:47  dkrajzew
// false computation of yielding on lane ends debugged; some debugging on
//  tl-import; further work on vissim-import
//
// Revision 1.12  2003/04/07 12:15:38  dkrajzew
// first steps towards a junctions geometry; tyellow removed again, traffic
//  lights have yellow times given explicitely, now
//
// Revision 1.11  2003/04/04 07:43:03  dkrajzew
// Yellow phases must be now explicetely given; comments added; order of edge
//  sorting (false lane connections) debugged
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
// possibility to add connections between lanes added; adding of connections
//  between edges revalidated
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
// Lane division bug (forgetting the turning direction; cause of segmentation
//  violations under Linux) fixed
//
// Revision 1.7  2002/06/18 05:05:08  dkrajzew
// Lane division bug accessing after the end of an array removed
//
// Revision 1.6  2002/06/17 15:19:29  dkrajzew
// unreferenced variable declarations removed
//
// Revision 1.5  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the
//  EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.4  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed;
//  Comments improved
//
// Revision 1.3  2002/05/14 04:42:54  dkrajzew
// new computation flow
//
// Revision 1.2  2002/04/26 10:07:10  dkrajzew
// Windows eol removed; minor SUMOReal to int conversions removed;
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
#include "NBEdge.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC       // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
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
    if(i!=_connections->end()) {
        // if there were already lanes assigned, get them
        lanes = (*i).second;
    }

    // check whether the current lane was already used to connect the currently
    //  regarded approached edge
    vector<size_t>::iterator j=find(lanes.begin(), lanes.end(), (size_t) lane);
    if(j==lanes.end()) {
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
               string type, SUMOReal speed, size_t nolanes,
               SUMOReal length, int priority, LaneSpreadFunction spread,
               EdgeBasicFunction basic) :
    _step(INIT), _id(StringUtils::convertUmlaute(id)),
    _type(StringUtils::convertUmlaute(type)),
    _nolanes(nolanes), _from(from), _to(to), _length(length), _angle(0),
    _priority(priority), _speed(speed),
    _name(StringUtils::convertUmlaute(name)), //_ToEdges(0),
    _turnDestination(0), //_reachable(0),
    /*_linkIsPriorised(0),*/ //_succeedinglanes(0),
    _fromJunctionPriority(-1), _toJunctionPriority(-1),
    _basicType(basic), myLaneSpreadFunction(spread),
    myAmTurningWithAngle(0), myAmTurningOf(0)
{
    assert(_nolanes!=0);
    if(_from==0||_to==0) {
        throw std::exception();
    }
    _angle = NBHelpers::angle(
        _from->getPosition().x(), _from->getPosition().y(),
        _to->getPosition().x(), _to->getPosition().y()
        );
    _from->addOutgoingEdge(this);
    _to->addIncomingEdge(this);
    // prepare container
//    _reachable = new NBEdge::ReachableFromLaneVector();
    _reachable.resize(_nolanes, EdgeLaneVector());
//    _linkIsPriorised = new NBEdge::ReachablePrioritiesFromLaneVector();
//    _linkIsPriorised->resize(_nolanes, BoolVector());
//    _succeedinglanes = new NBEdge::LanesThatSucceedEdgeCont();
//    _ToEdges = new map<NBEdge*, vector<size_t> >();
    if(_length<=0) {
        _length = GeomHelper::distance(
            _from->getPosition(), _to->getPosition());
    }
    if(basic==EDGEFUNCTION_SOURCE&&_length<200) {
        _length = 200;
    }
    if(basic==EDGEFUNCTION_SINK&&_length<200) {
        _length = 200;
    }
    assert(_length>0);
    assert(_from->getPosition()!=_to->getPosition());
    myGeom.push_back(_from->getPosition());
    myGeom.push_back(_to->getPosition());
    if(_priority<0) {
        _priority = 0;
    }
    computeLaneShapes();
    for(size_t i=0; i<_nolanes; i++) {
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
    _name(StringUtils::convertUmlaute(name)),
    //_ToEdges(0),
    _turnDestination(0),
    //_reachable(0),
/*    _linkIsPriorised(0), *///_succeedinglanes(0),
    _fromJunctionPriority(-1), _toJunctionPriority(-1),
    _basicType(basic), myGeom(geom), myLaneSpreadFunction(spread),
    myAmTurningWithAngle(0), myAmTurningOf(0)
{
    assert(_nolanes!=0);
    if(_from==0||_to==0) {
        throw std::exception();
    }
    _angle = NBHelpers::angle(
        _from->getPosition().x(), _from->getPosition().y(),
        _to->getPosition().x(), _to->getPosition().y()
        );
    _from->addOutgoingEdge(this);
    _to->addIncomingEdge(this);
    // prepare container
//    _reachable = new NBEdge::ReachableFromLaneVector();
    _reachable.resize(_nolanes, EdgeLaneVector());
//    _linkIsPriorised = new NBEdge::ReachablePrioritiesFromLaneVector();
//    _linkIsPriorised->resize(_nolanes, BoolVector());
//    _succeedinglanes = new NBEdge::LanesThatSucceedEdgeCont();
//    _ToEdges = new map<NBEdge*, vector<size_t> >();
    if(_length<=0) {
        _length = GeomHelper::distance(
            _from->getPosition(), _to->getPosition());
    }
    if(basic==EDGEFUNCTION_SOURCE&&_length<200) {
        _length = 200;
    }
    if(basic==EDGEFUNCTION_SINK&&_length<200) {
        _length = 200;
    }
    assert(_length>0);
    assert(myGeom.size()>=2);
    if(_priority<0) {
        _priority = 0;
    }
    computeLaneShapes();
    for(size_t i=0; i<_nolanes; i++) {
        myLaneSpeeds.push_back(speed);
    }
}


NBEdge::~NBEdge()
{
}


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
NBEdge::getPriority()
{
    return _priority;
}


int
NBEdge::getJunctionPriority(NBNode *node)
{
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


string
NBEdge::getID()
{
    return _id;
}


string
NBEdge::getName()
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


const EdgeLaneVector * const
NBEdge::getEdgeLanesFromLane(size_t lane) const
{
    assert(lane<_reachable.size());
    return &_reachable[lane]; // !!! unschöne Rückgabe
}


void
NBEdge::computeTurningDirections()
{
    _turnDestination = 0;
    EdgeVector outgoing = _to->getOutgoingEdges();
    for(EdgeVector::iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if(outedge->getBasicType()==EDGEFUNCTION_SINK||_basicType==EDGEFUNCTION_SOURCE) {
            continue;
        }
        SUMOReal relAngle =
            NBHelpers::relAngle(getAngle(*_to), outedge->getAngle(*_to));
        // do not append the turnaround
        if(fabs(relAngle)>160) {
            setTurningDestination(outedge);
        }
    }
}


SUMOReal
NBEdge::getAngle(const NBNode &atNode) const
{
    if(&atNode==_from) {
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
    if( cur>old
        &&
        e->acceptBeingTurning(this)) {

        _turnDestination = e;
    }
}


bool
NBEdge::acceptBeingTurning(NBEdge *e)
{
    if(e==myAmTurningOf) {
        return true;
    }
    SUMOReal angle = fabs(NBHelpers::relAngle(getAngle(), e->getAngle()));
    if(myAmTurningWithAngle>angle) {
        return false;
    }
    if(myAmTurningWithAngle==angle) {
        return false; // !!! ok, this happens only within a cell-network (backgrnd), we have to take a further look sometime
    }
    NBEdge *previous = myAmTurningOf;
    myAmTurningWithAngle = angle;
    myAmTurningOf = e;
    if(previous!=0) {
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
        "\" From=\"" << _from->getID() <<
        "\" To=\"" << _to->getID() <<
        "\" Priority=\"" << _priority <<
        "\" Angle=\"" << getAngle() <<
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
    const std::vector<NBEdge*> *tmp = getConnectedSorted();
    std::vector<NBEdge*> sortedConnected = *tmp;
    if(getTurnDestination()!=0) {
        sortedConnected.push_back(getTurnDestination());
    }
    delete tmp;
    for(std::vector<NBEdge*>::iterator l=sortedConnected.begin(); l!=sortedConnected.end(); l++) {
        LanesThatSucceedEdgeCont::iterator m = _succeedinglanes.find(*l);
        if(m!=_succeedinglanes.end()) {
            writeConnected(into, (*m).first, (*m).second);
        }
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
        into << "\"/>" << endl;
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
    if(myLaneSpeeds[lane]==0) {
        WRITE_WARNING(string("Lane #") + toString<size_t>(lane) + string(" of edge '") + _id + string("' has a maximum velocity of 0."));
    } else if(myLaneSpeeds[lane]<0) {
        MsgHandler::getErrorInstance()->inform(
            string("Negative velocity (") + toString<SUMOReal>(myLaneSpeeds[lane])
            + string(" on edge '") + _id + string("' lane#")
            + toString<size_t>(lane) + string("."));
        throw ProcessError();
    }
    into << " maxspeed=\"" << myLaneSpeeds[lane] << "\" length=\"" << _length <<
        "\" changeurge=\"0\">";
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
    if(_from==_to) {
        return;
    }
    // build the shape of each edge
    myLaneGeoms.clear();
    for(size_t i=0; i<_nolanes; i++) {
        myLaneGeoms.push_back(computeLaneShape(i));
    }
}


const Position2DVector &
NBEdge::getLaneShape(size_t i) const
{
    return myLaneGeoms[i];
}


Position2DVector
NBEdge::computeLaneShape(size_t lane)
{
    Position2DVector shape;
    // extrapolate first
    // (needed for a better junction shape computation, they will be prunned later)
    Position2D from = GeomHelper::extrapolate_first(
        myGeom.at(0), myGeom.at(1), 100);

    size_t i = 1;
    std::pair<SUMOReal, SUMOReal> offsets;
    // go thorugh the list of segments
    for(; i<myGeom.size(); i++) {
        Position2D to =
            i==myGeom.size()-1
                ? GeomHelper::extrapolate_second(
                    myGeom.at(myGeom.size()-2), myGeom.at(myGeom.size()-1), 100)
                : myGeom.at(i);
        // compute the lane position in dependence to the lane
        offsets =
            laneOffset(from, to, 3.5, _nolanes-1-lane);
        shape.push_back(
            Position2D(from.x()-offsets.first, from.y()-offsets.second)); // (methode umbenennen; was heisst hier "-")
        from = to;
    }
    // end on last segment
    shape.push_back(
        Position2D(from.x()-offsets.first, from.y()-offsets.second)); // (methode umbenennen; was heisst hier "-")
    // prune the geometry to the begin and the end node
    //  begin node first
    shape.pruneFromBeginAt(_from->getPosition());
    //  then end node
    shape.pruneFromEndAt(_to->getPosition());
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
    SUMOReal length = sqrt( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) );
    std::pair<SUMOReal, SUMOReal> offsets =
        GeomHelper::getNormal90D_CW(x1, y1, x2, y2, length, lanewidth);
    SUMOReal xoff = offsets.first / (SUMOReal) 2.0;
    SUMOReal yoff = offsets.second / (SUMOReal) 2.0;
    if(myLaneSpreadFunction==LANESPREAD_RIGHT) {
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
    if(edge==0) {
        return;
    }
    into << "      <cedge id=\"" << edge->getID()
        << "\" via=\":" << _to->getID() << "\">";
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
    assert(lane<_reachable.size());
    size_t noApproached = _reachable[lane].size();
    if(noApproached==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\"/>"
            << endl;
    }
    // output list of connected lanes
        // go through each connected edge
    for(size_t j=0; j<noApproached; j++) {
        writeSingleSucceeding(into, lane, j);
    }
    into << "   </succ>" << endl << endl;
}


void
NBEdge::writeSingleSucceeding(std::ostream &into, size_t fromlane, size_t destidx)
{
    // check whether the connected lane is invalid
    //  (should not happen; this is an artefact left from previous versions)
    assert(fromlane<_reachable.size());
    assert(destidx<_reachable[fromlane].size());
    if(_reachable[fromlane][destidx].edge==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\" "
            << "dir=\"s\" state=\"O\"/>" // !!! check dummy values
            << endl;
        return;
    }
    // write the id
    into << "      <succlane lane=\""
//        << _to->getInternalLaneID(this, fromlane, _reachable[fromlane][destidx].edge)
//        <<
        << _reachable[fromlane][destidx].edge->getID() << '_'
        << _reachable[fromlane][destidx].lane << '\"'; // !!! classe LaneEdge mit getLaneID
    into << " via=\""
        << _to->getInternalLaneID(this, fromlane, _reachable[fromlane][destidx].edge)
        << "_0\"";
    // set information about the controlling tl if any
    if(_reachable[fromlane][destidx].tlID!="") {
        into << " tl=\"" << _reachable[fromlane][destidx].tlID << "\"";
        into << " linkno=\"" << _reachable[fromlane][destidx].tlLinkNo << "\"";
    }
    // write information whether the connection yields
    if(!_to->mustBrake(this,
            _reachable[fromlane][destidx].edge,
            _reachable[fromlane][destidx].lane)) {
        into << " yield=\"0\"";
    } else {
        into << " yield=\"1\"";
    }
    // write the direction information
    NBMMLDirection dir =
        _to->getMMLDirection(this, _reachable[fromlane][destidx].edge);
    into << " dir=\"";
    switch(dir) {
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
        throw 1;
    }
    into << "\" ";
    // write the state information
    if(_reachable[fromlane][destidx].tlID!="") {
        into << "state=\"t";
    } else {
        into << "state=\""
            << _to->stateCode(this,
                _reachable[fromlane][destidx].edge,
                _reachable[fromlane][destidx].lane);
    }
    // close
    into << "\"/>" << endl;
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
    if( find(_connectedEdges.begin(), _connectedEdges.end(), dest)
          ==_connectedEdges.end()) {
        _connectedEdges.push_back(dest);
    }
    if(_step<EDGE2EDGES) {
        _step = EDGE2EDGES;
    }
    // !!! recheck the next - something too much is deleted in invalidateConnections
    if(_ToEdges.find(dest)==_ToEdges.end()) {
        _ToEdges[dest] = vector<size_t>();
    }
    return true;
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
    if(ok) {
        setConnection(from, dest, toLane);
        vector<size_t> &lanes = (_ToEdges.find(dest))->second;
        vector<size_t>::iterator i = find(lanes.begin(), lanes.end(), from);
        if(i==lanes.end()) {
            lanes.push_back(from);
        }
        _step = LANES2LANES;
    }
    return ok;
}


bool
NBEdge::addLane2LaneConnections(size_t fromLane,
                                NBEdge *dest, size_t toLane,
                                size_t no,
                                bool invalidatePrevious)
{
    if(invalidatePrevious) {
        invalidateConnections(true);
    }
    bool ok = true;
    for(size_t i=0; i<no&&ok; i++) {
        ok &=
            addLane2LaneConnection(fromLane+i, dest, toLane+i);
    }
    return ok;
}


bool
NBEdge::computeEdge2Edges()
{
    // return if this relationship has been build in previous steps or
    //  during the import
    if(_step>=EDGE2EDGES) {
        return true;
    }
    if(_connectedEdges.size()==0) {
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
    if(_step>=LANES2EDGES) {
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
    size_t i;
    // check:
    //  if there is a lane with no connections and any neighbour lane has
    //  more than one connections, try to move one of them
    for(i=0; i<_nolanes; i++) {
        assert(i<_reachable.size());
        if(_reachable[i].size()==0) {
            if(i>0&&_reachable[i-1].size()>1) {
                moveConnectionToLeft(i-1);
            } else if(i<_reachable.size()-1&&_reachable[i+1].size()>1) {
                moveConnectionToRight(i+1);
            }
        }
    }
    // check:
    //  go through all lanes and add an empty connection if no connection
    //  is yet set
    assert(_nolanes==_reachable.size());
    for(i=0; i<_nolanes; i++) {
        if(_reachable[i].size()==0) {
            setConnection(i, 0, 0);
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
    setConnection(lane+1, dest.first, dest.second);
}


void
NBEdge::moveConnectionToRight(size_t lane)
{
    pair<NBEdge*, size_t> dest = getConnectionRemoving(lane, 0);
    setConnection(lane-1, dest.first, dest.second);
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
    NBEdge::LanesThatSucceedEdgeCont::iterator i =
        _succeedinglanes.find(edgelane.edge);
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
    if(currentOutgoing==_turnDestination) {
        return vector<size_t>();
    }
    map<NBEdge*, vector<size_t> >::iterator i=_ToEdges.find(currentOutgoing);
    assert(i==_ToEdges.end()||(*i).second.size()<=_nolanes);
    if(i==_ToEdges.end()) {
        return vector<size_t>();
    }
    return (*i).second;
}


const std::vector<NBEdge*> *
NBEdge::getConnectedSorted()
{
    // check whether connections exist and if not, use edges from the node
    EdgeVector outgoing = _connectedEdges;
    if(outgoing.size()==0) {
        outgoing = _to->getOutgoingEdges();
    }
    // allocate the sorted container
    size_t size = outgoing.size();
    vector<NBEdge*> *edges = new vector<NBEdge*>();
    edges->reserve(size);
    for(EdgeVector::const_iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if(outedge!=_turnDestination) {// !!!
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
    for(i=0; i<size; i++) {
        // res will be the number of lanes which are meant to reach the
        //  current outgoing edge
        SUMOReal res =
            (SUMOReal) (*priorities)[i] *
            (SUMOReal) _nolanes / (SUMOReal) prioSum;
        // do not let this number be greater than the number of available lanes
        if(res>_nolanes) {
            res = (SUMOReal) _nolanes;
        }
        // add it to the list
        resultingLanes.push_back(res);
        sumResulting += res;
        if(minResulting>res) {
            minResulting = res;
        }
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
    SUMOReal g4t = (sumResulting / minResulting);
    int rer = int(g4t);
    SUMOReal f1 = floor(g4t);
    SUMOReal f2 = ceil(g4t);
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
        SUMOReal tmpNo = (SUMOReal) resultingLanes[i] / (SUMOReal) minResulting;
        for(SUMOReal j=0; j<tmpNo; j++) {
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
    if(outgoing->size()==0) {
        return priorities;
    }
        // patch only
/*    if(outgoing->size()==_nolanes) {
        for(size_t i=0; i<_nolanes; i++) {
            priorities->push_back(4);
        }
        return priorities;
    }*/
    priorities->reserve(outgoing->size());
    vector<NBEdge*>::const_iterator i;
    for(i=outgoing->begin(); i!=outgoing->end(); i++) {
        int prio = (*i)->getJunctionPriority(_to);
        if(((*i)->getJunctionPriority(_to)+1)*2<=0) {
            prio = (*i)->getJunctionPriority(_to);
        }
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

/*
void
NBEdge::computeLinkPriorities()
{
    // reset all links to unprioritised
    size_t i;
    for(i=0; i<_nolanes; i++) {
        assert(_reachable!=0&&i<_reachable->size());
        size_t size = _reachable[i].size();
        (*_linkIsPriorised)[i].resize(size, false);
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
                    if((*_reachable)[i][j].edge!=_turnDestination) {
                        (*_linkIsPriorised)[i][j] = true;
                    }
                }
                // otherwise, check whether the direction is on the right side
                // of the other side of the priorised connection
                else {
                    bool found = false;
                    EdgeVector::const_iterator me = find(nodeEdges->begin(), nodeEdges->end(), this);
                    me = NBContHelper::nextCCW(nodeEdges, me);
                    while(!found&&(*me)->getJunctionPriority(_to)!=1) {
                        if(*me==(*_reachable)[i][j].edge) {
                            (*_linkIsPriorised)[i][j] = false;
                            found = true;
                        }
                        me = NBContHelper::nextCCW(nodeEdges, me);
                    }
                }
            }
        }
    }
}
*/

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
    for(size_t i=0; i<_reachable.size(); i++) {
        sort(_reachable[i].begin(), _reachable[i].end(),
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
    // this is somekind of a misbehaviour which should only occure when the
    //  junction's outgoing edge priorities were not properly computed, what
    //  still may happen due to an incomplete or not proper input
    // what happens is that under some circumstances a single lane may set to
    //  be approached by more than a lane of this junction. This must not be!
    // we test whether it is the case and do nothing if so - the connection
    //  will be refused
    EdgeLane el;
    el.edge = dest_edge;
    el.lane = dest_lane; // !!! EdgeLane as class
    for(size_t j=0; dest_edge!=0&&j<_reachable.size(); j++) {
        // skip current lane
        if(j==src_lane) {
            continue;
        }
        // for any other lane: check whether a connection to the same
        //  lane as the one to be added exists
        EdgeLaneVector &tmp = _reachable[j];
        if(find(tmp.begin(), tmp.end(), el)!=tmp.end()) {
            return;
        }
    }

    // this connection has not yet been set
    bool known = false;
    // find the entry which holds the information which lanes may
    //  be used to reach the current destination edge
    LanesThatSucceedEdgeCont::iterator i = _succeedinglanes.find(dest_edge);
    // if there is no such connection yet, build it
    if(i==_succeedinglanes.end()) {
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
        if(i==lanes.end())
            lanes.push_back(src_lane);
        // otherwise, mark as known
        else
            known = true;
        _succeedinglanes[dest_edge] = lanes;
    }
    // append current connection only if no equal is already known
    if(!known) {
        assert(_reachable.size()>src_lane);
        _reachable[src_lane].push_back(el);
    }
}


bool
NBEdge::isTurningDirectionAt(const NBNode *n, NBEdge *edge) const
{
    // maybe it was already set as the turning direction
    if(edge == _turnDestination) {
        return true;
    } else if(_turnDestination!=0) {
        // otherwise - it's not if a turning direction exists
        return false;
    }
    // if the same nodes are connected
    if(_from==edge->_to && _to==edge->_from) {
        return true;
    }
    // we have to checke whether the connection between the nodes is
    //  geometrically similar
    SUMOReal thisFromAngle2 = getAngle(*n);
    SUMOReal otherToAngle2 = edge->getAngle(*n);
    if(thisFromAngle2<otherToAngle2) {
        swap(thisFromAngle2, otherToAngle2);
    }
    if(thisFromAngle2-otherToAngle2>170&&thisFromAngle2-otherToAngle2<190) {
        return true;
    }
    return false;
}



bool
NBEdge::isAlmostSameDirectionAt(const NBNode *n, NBEdge *edge) const
{
    // if the same nodes are connected
    if(_to==edge->_to && _from==edge->_from) {
        return true;
    }
    // we have to checke whether the connection between the nodes is
    //  geometrically similar
    SUMOReal thisFromAngle2 = getAngle(*n);
    SUMOReal otherToAngle2 = edge->getAngle(*n);
    if(thisFromAngle2<otherToAngle2) {
        swap(thisFromAngle2, otherToAngle2);
    }
    if(thisFromAngle2-otherToAngle2<20&&thisFromAngle2-otherToAngle2>-20) {
        return true;
    }
    return false;
}



NBNode *
NBEdge::tryGetNodeAtPosition(SUMOReal pos, SUMOReal tolerance) const
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
    if(k!=_connectedEdges.end()) {
        for(k=_connectedEdges.begin(); k!=_connectedEdges.end();) {
            if((*k)==which) {
                k = _connectedEdges.erase(k);
            } else {
                k++;
            }
        }
    } else {
        for(size_t i=0; i<_connectedEdges.size(); i++) {
            if(_connectedEdges[i]==which) {
                _connectedEdges[i] = by;
            }
        }
    }
    // check whether it was the turn destination
    if(_turnDestination==which) {
        _turnDestination = by;
    }
    // replace in _ToEdges
    {
        bool found = true;
        // check if the edge to replace by was already connected
        bool have = _ToEdges.find(by)!=_ToEdges.end();
        // find the edge to replace
        std::map<NBEdge*, std::vector<size_t> >::iterator j = _ToEdges.find(which);
        if(j!=_ToEdges.end()) {
            // if the edge to replace by already had a connection
            if(have) {
                // add further connections
                for(std::vector<size_t>::iterator k=_ToEdges[which].begin(); k!=_ToEdges[which].end(); k++) {
                    if(find(_ToEdges[which].begin(), _ToEdges[which].end(), (*k)+laneOff)!=_ToEdges[which].end()) {
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
        for(ReachableFromLaneVector::iterator k=_reachable.begin(); k!=_reachable.end(); k++) {
            for(EdgeLaneVector::iterator l=(*k).begin(); l!=(*k).end(); l++) {
                if((*l).edge==which) {
                    (*l).edge = by;
                    (*l).lane = (*l).lane + laneOff;
                }
            }
        }
    }
    // replace in _succeedinglanes
    {
       LanesThatSucceedEdgeCont::iterator l=_succeedinglanes.find(which);
        if(l!=_succeedinglanes.end()) {
            LanesThatSucceedEdgeCont::iterator l2=_succeedinglanes.find(by);
            if(l2!=_succeedinglanes.end()) {
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
NBEdge::moveOutgoingConnectionsFrom(NBEdge *e, size_t laneOff)
{
    size_t lanes = e->getNoLanes();
    for(size_t i=0; i<lanes; i++) {
        const EdgeLaneVector *elv = e->getEdgeLanesFromLane(i);
        for(EdgeLaneVector::const_iterator j=elv->begin(); j!=elv->end(); j++) {
            EdgeLane el = (*j);
            assert(el.tlID=="");
            bool ok = addLane2LaneConnection(i+laneOff, el.edge, el.lane);
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
    for(EdgeVector::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        NBEdge *inc = *i;
        // We have to do this
        inc->_step = EDGE2EDGES;
        // add all connections
        for(EdgeVector::iterator j=_connectedEdges.begin(); j!=_connectedEdges.end(); j++) {
            inc->addEdge2EdgeConnection(*j);
        }
        inc->removeFromConnections(this);
    }
}


void
NBEdge::removeFromConnections(NBEdge *which)
{
    // remove from "_connectedEdges"
    for(size_t i=0; i<_connectedEdges.size(); i++) {
        if(_connectedEdges[i]==which) {
            _connectedEdges.erase(_connectedEdges.begin()+i);
            i--;
        }
    }
    // check whether it was the turn destination
    if(_turnDestination==which) {
        _turnDestination = 0;
    }
    // remove in _ToEdges
    {
        bool found = true;
        std::map<NBEdge*, std::vector<size_t> >::iterator j = _ToEdges.find(which);
        if(j!=_ToEdges.end()) {
            _ToEdges.erase(which);
        }
    }
    // remove in _reachable
    {
        for(ReachableFromLaneVector::iterator k=_reachable.begin(); k!=_reachable.end(); k++) {
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
    {
        LanesThatSucceedEdgeCont::iterator l=_succeedinglanes.find(which);
        if(l!=_succeedinglanes.end()) {
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
//    _linkIsPriorised->clear();
//    _linkIsPriorised->resize(_nolanes, BoolVector());
    _succeedinglanes.clear();
    if(reallowSetting) {
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
    for(EdgeLaneVector::const_iterator i=_reachable[lane].begin(); i!=_reachable[lane].end(); i++) {
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
    computeLaneShapes();
}


SUMOReal
NBEdge::getMaxLaneOffset()
{
    return (SUMOReal) 3.5 * _nolanes;
}


Position2D
NBEdge::getMinLaneOffsetPositionAt(NBNode *node, SUMOReal width)
{
    width = width < myLaneGeoms[0].length()/(SUMOReal) 2.0
        ? width
        : myLaneGeoms[0].length()/(SUMOReal) 2.0;
    if(node==_from) {
        Position2D pos =
            myLaneGeoms[myLaneGeoms.size()-1].positionAtLengthPosition(width);
            GeomHelper::transfer_to_side(pos,
                myLaneGeoms[myLaneGeoms.size()-1].at(0), myLaneGeoms[myLaneGeoms.size()-1].at(myLaneGeoms[0].size()-1),
                3.5 / 2.0);
        return pos;
    } else {
        Position2D pos =
            myLaneGeoms[0].positionAtLengthPosition(myLaneGeoms[0].length() - width);
            GeomHelper::transfer_to_side(pos,
                myLaneGeoms[0].at(myLaneGeoms[0].size()-1), myLaneGeoms[0].at(0),
                3.5 / 2.0);
        return pos;
    }
}


Position2D
NBEdge::getMaxLaneOffsetPositionAt(NBNode *node, SUMOReal width)
{
    width = width < myLaneGeoms[0].length()/(SUMOReal) 2.0
        ? width
        : myLaneGeoms[0].length()/(SUMOReal) 2.0;
    if(node==_from) {
        Position2D pos = myLaneGeoms[0].positionAtLengthPosition(width);
            GeomHelper::transfer_to_side(pos,
                myLaneGeoms[0].at(0), myLaneGeoms[0].at(myLaneGeoms[0].size()-1),
                -3.5 / 2.0);
        return pos;
    } else {
        Position2D pos = myLaneGeoms[myLaneGeoms.size()-1].positionAtLengthPosition(myLaneGeoms[myLaneGeoms.size()-1].length() - width);
            GeomHelper::transfer_to_side(pos,
                myLaneGeoms[myLaneGeoms.size()-1].at(myLaneGeoms[myLaneGeoms.size()-1].size()-1), myLaneGeoms[myLaneGeoms.size()-1].at(0),
                -3.5 / 2.0);
        return pos;
    }
}


void
NBEdge::setControllingTLInformation(int fromLane, NBEdge *toEdge, int toLane,
                                    const std::string &tlID, size_t tlPos)
{
    assert(fromLane<0||fromLane<(int) _nolanes);
    // try to use information about the connections if given
    if(fromLane>=0&&toLane>=0) {
        // get the connections outgoing from this lane
        EdgeLaneVector &connections = _reachable[fromLane];
        // find the specified connection
        EdgeLaneVector::iterator i =
            find_if(connections.begin(), connections.end(),
                NBContHelper::edgelane_finder(toEdge, toLane));
        // ok, we have to test this as on the removal of dummy edges some connections
        //  will be reassigned
        if(i!=connections.end()) {
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
    for(size_t j=0; j<_nolanes; j++) {
        EdgeLaneVector &connections = _reachable[j];
        EdgeLaneVector::iterator i =
            find_if(connections.begin(), connections.end(),
                NBContHelper::edgelane_finder(toEdge, toLane));
        while(i!=connections.end()) {
            // get the connection
            EdgeLane &connection = *i;
            // set the information about the tl
            //  but check first, if it was not set before
            if(connection.tlID=="") {
                connection.tlID = tlID;
                connection.tlLinkNo = tlPos;
                no++;
            } else {
                if(connection.tlID!=tlID&&connection.tlLinkNo==tlPos) {
                    WRITE_WARNING(string("The lane ") + toString<int>(connection.lane)+ string(" on edge ") + connection.edge->getID()+ string(" already had a traffic light signal."));
                    hadError = true;
                }
            }
            i = find_if(i+1, connections.end(),
                NBContHelper::edgelane_finder(toEdge, toLane));
        }
    }
    if(hadError&&no==0) {
        WRITE_WARNING(string("Could not set any signal of the traffic light '")+ tlID + string("' (unknown group)"));
    }
}


void
NBEdge::normalisePosition(const NBNodeCont &nc)
{
    myGeom.resetBy(nc.getNetworkOffset());
    for(size_t i=0; i<_nolanes; i++) {
        myLaneGeoms[i].resetBy(nc.getNetworkOffset());
    }
}


void
NBEdge::reshiftPosition(SUMOReal xoff, SUMOReal yoff, SUMOReal rot)
{
    myGeom.reshiftRotate(xoff, yoff, rot);
    for(size_t i=0; i<_nolanes; i++) {
        myLaneGeoms[i].reshiftRotate(xoff, yoff, rot);
    }
}


Position2DVector
NBEdge::getCWBoundaryLine(const NBNode &n, SUMOReal offset) const
{
    Position2DVector ret;
    if(_from==(&n)) {
        // outgoing
        ret = myLaneGeoms[0];
    } else {
        // incoming
        ret = myLaneGeoms[getNoLanes()-1].reverse();
    }
    ret.move2side(-offset);
    return ret;
}


Position2DVector
NBEdge::getCCWBoundaryLine(const NBNode &n, SUMOReal offset) const
{
    Position2DVector ret;
    if(_from==(&n)) {
        // outgoing
        ret = myLaneGeoms[getNoLanes()-1];
    } else {
        // incoming
        ret = myLaneGeoms[0].reverse();
    }
    ret.move2side(offset);
    return ret;
}

/*
Line2D
NBEdge::getNECWBoundaryLine(NBNode *n, SUMOReal offset)
{
    Line2D ret;
    if(_from==n) {
        // outgoing
        Position2DVector laneGeom = myLaneGeoms[0];
        assert(laneGeom.size()>1);
        ret = Line2D(laneGeom.at(0), laneGeom.at(1));
    } else {
        // incoming
        Position2DVector laneGeom = myLaneGeoms[getNoLanes()-1];
        assert(laneGeom.size()>1);
        ret = Line2D(laneGeom.at(laneGeom.size()-1), laneGeom.at(laneGeom.size()-2));
    }
    ret.move2side(-offset);
    return ret;
}


Line2D
NBEdge::getNECCWBoundaryLine(NBNode *n, SUMOReal offset)
{
    Line2D ret;
    if(_from==n) {
        // outgoing
        Position2DVector laneGeom = myLaneGeoms[getNoLanes()-1];
        ret = Line2D(laneGeom.at(0), laneGeom.at(1));
    } else {
        // incoming
        Position2DVector laneGeom = myLaneGeoms[0];
        ret = Line2D(laneGeom.at(laneGeom.size()-1), laneGeom.at(laneGeom.size()-2));
    }
    ret.move2side(offset);
    return ret;
}
*/

SUMOReal
NBEdge::width() const
{
    return (SUMOReal) _nolanes * (SUMOReal) 3.0;
}


bool
NBEdge::expandableBy(NBEdge *possContinuation) const
{
    // ok, the number of lanes must match
    if(_nolanes!=possContinuation->_nolanes) {
        return false;
    }
    // the priority, too (?)
    if(_priority!=possContinuation->_priority) {
        return false;
    }
    // the speed allowed
    if(_speed!=possContinuation->_speed) {
        return false;
    }
    // the next is quite too conservative here, but seems to work
    if( _basicType!=EDGEFUNCTION_NORMAL
        &&
        possContinuation->_basicType!=EDGEFUNCTION_NORMAL) {

        return false;
    }
    // also, check whether the connections - if any exit do allow to join
    //  both edges
    // This edge must have a one-to-one connection to the following lanes
    switch(_step) {
    case INIT_REJECT_CONNECTIONS:
        break;
    case INIT:
        break;
    case EDGE2EDGES:
        {
            // the following edge must be connected
            const EdgeVector &conn = getConnected();
            if( find(conn.begin(), conn.end(), possContinuation)
                ==conn.end()) {

                return false;
            }
        }
        break;
    case LANES2EDGES:
        {
            // the possible continuation must be connected
            if(_ToEdges.find(possContinuation)==_ToEdges.end()) {
                return false;
            }
            // all lanes must go to the possible continuation
            const std::vector<size_t> &lanes =
                (*(_ToEdges.find(possContinuation))).second;
            if(lanes.size()!=_nolanes) {
                return false;
            }
        }
        break;
    case LANES2LANES:
        {
            for(size_t i=0; i<_nolanes; i++) {
                const EdgeLaneVector *elv = getEdgeLanesFromLane(i);
                bool found = false;
                for(EdgeLaneVector::const_iterator j=elv->begin(); j!=elv->end(); j++) {
                    if((*j).edge==possContinuation&&(*j).lane==i) {
                        found = true;
                        j = elv->end()-1;
                    }
                }
                if(!found) {
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
    for(size_t i=0; i<_nolanes; i++) {
        myLaneGeoms[i].appendWithCrossingPoint(e->myLaneGeoms[i]);
    }
    // recompute length
    _length += e->_length;
    // copy the connections and the building step if given
    _step = e->_step;
    _connectedEdges = e->_connectedEdges;
//    delete _reachable;
    _reachable = (e->_reachable);
//    delete _succeedinglanes;
    _succeedinglanes = e->_succeedinglanes;
//    delete _ToEdges;
    _ToEdges = e->_ToEdges;
    _turnDestination = e->_turnDestination;
    // set the node
    _to = e->_to;
}


void
NBEdge::computeEdgeShape()
{
    size_t i;
    for(i=0; i<_nolanes; i++) {
        // get lane begin and end
        Line2D lb = Line2D(
            myLaneGeoms[i].at(0),
            myLaneGeoms[i].at(1));
        Line2D le = Line2D(
            myLaneGeoms[i].at(myLaneGeoms[i].size()-1),
            myLaneGeoms[i].at(myLaneGeoms[i].size()-2));
        lb.extrapolateBy(100.0);
        le.extrapolateBy(100.0);
        //
        Position2DVector old = myLaneGeoms[i];
        Position2D nb, ne;
        // lane begin
        if(_from->getShape().intersects(myLaneGeoms[i])) {
            // get the intersection position with the junction
            DoubleVector pbv = myLaneGeoms[i].intersectsAtLengths(_from->getShape());
            if(pbv.size()>0) {
                SUMOReal pb = DoubleVectorHelper::maxValue(pbv);
                if(pb>=0&&pb<=myLaneGeoms[i].length()) {
                    myLaneGeoms[i] = myLaneGeoms[i].getSubpart(pb, myLaneGeoms[i].length());
                }
            }
        } else if (_from->getShape().intersects(lb.p1(), lb.p2())) {
            DoubleVector pbv = lb.intersectsAtLengths(_from->getShape());
            if(pbv.size()>0) {
                SUMOReal pb = DoubleVectorHelper::maxValue(pbv);
                if(pb>=0) {
                    myLaneGeoms[i].eraseAt(0);
                    myLaneGeoms[i].push_front(lb.getPositionAtDistance(pb));
                }
            }
        }
        // lane end
        if(_to->getShape().intersects(myLaneGeoms[i])) {
            // get the intersection position with the junction
            DoubleVector pev = myLaneGeoms[i].intersectsAtLengths(_to->getShape());
            if(pev.size()>0) {
                SUMOReal pe = DoubleVectorHelper::minValue(pev);
                if(pe>=0&&pe<=myLaneGeoms[i].length()) {
                    myLaneGeoms[i] = myLaneGeoms[i].getSubpart(0, pe);
                }
            }
        } else if (_to->getShape().intersects(le.p1(), le.p2())) {
            DoubleVector pev = le.intersectsAtLengths(_to->getShape());
            if(pev.size()>0) {
                SUMOReal pe = DoubleVectorHelper::maxValue(pev);
                if(pe>=0) {
                    myLaneGeoms[i].eraseAt(myLaneGeoms[i].size()-1);
                    myLaneGeoms[i].push_back(le.getPositionAtDistance(pe));
                }
            }
        }
        if(((int) myLaneGeoms[i].length())==0) {
            myLaneGeoms[i] = old;
    }
    }
    // recompute edge's length
    if(_basicType==EDGEFUNCTION_SOURCE||_basicType==EDGEFUNCTION_SINK) {
        return;
    }
    SUMOReal length = 0;
    for(i=0; i<_nolanes; i++) {
        assert(myLaneGeoms[i].length()>0);
        length += myLaneGeoms[i].length();
    }
    _length = length / (SUMOReal) _nolanes;
}


bool
NBEdge::hasSignalisedConnectionTo(NBEdge *e) const
{
    for(size_t i=0; i<_nolanes; i++) {
        // get the connections outgoing from this lane
        const EdgeLaneVector &connections = _reachable[i];
        // find the specified connection
        for(EdgeLaneVector::const_iterator j=connections.begin(); j!=connections.end(); j++) {
            // get the connection
            const EdgeLane &connection = *j;
            if(connection.edge==e&&connection.tlID!="") {
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


void
NBEdge::writeLaneContinuation(std::ostream &into, size_t lane,
                              SUMOReal distance)
{
    // collect continuations
    StringContMap continuations;
    std::set<std::string> visited;
}

std::string
NBEdge::getLaneID(size_t lane)
{
    assert(lane<_nolanes);
    return _id + string("_") + toString<size_t>(lane);
}


bool
NBEdge::isNearEnough2BeJoined2(NBEdge *e)
{
    DoubleVector distances = myGeom.distances(e->getGeometry());
    if(distances.size()==0) {
        distances = e->getGeometry().distancesExt(myGeom);
/*        Position2D p1 = myGeom.center();
        Position2D p2 = e->getGeometry().center();
        SUMOReal dist = GeomHelper::distance(p1, p2);*/
    }
    SUMOReal max = DoubleVectorHelper::maxValue(distances);
    return max<7;
}


SUMOReal
NBEdge::getNormedAngle(const NBNode &atNode) const
{
    SUMOReal angle = getAngle(atNode);
    if(angle<0) {
        angle = 360 + angle;
    }
    assert(angle>=0&&angle<360);
    return angle;
}


SUMOReal
NBEdge::getNormedAngle() const
{
    SUMOReal angle = _angle;
    if(angle<0) {
        angle = 360 + angle;
    }
    assert(angle>=0&&angle<360);
    return angle;
}


void
NBEdge::addGeometryPoint(int index, const Position2D &p)
{
    myGeom.insertAt(index, p);
    computeLaneShapes();
}


void
NBEdge::incLaneNo(int by)
{
    _nolanes += by;
    _reachable.resize(_nolanes, EdgeLaneVector());
    while(myLaneSpeeds.size()<_nolanes) {
        myLaneSpeeds.push_back(_speed);
    }
    computeLaneShapes();
    const EdgeVector &incs = _from->getIncomingEdges();
    for(EdgeVector::const_iterator i=incs.begin(); i!=incs.end(); ++i) {
        (*i)->invalidateConnections(true);
    }
}


void
NBEdge::copyConnectionsFrom(NBEdge *src)
{
    _step = src->_step;
    _connectedEdges = src->_connectedEdges;
    _ToEdges = src->_ToEdges;
    _reachable = src->_reachable;
    _succeedinglanes = src->_succeedinglanes;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
