/***************************************************************************
                          NBNode.cpp
			  The representation of a single node
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
// Revision 1.9  2003/03/12 16:47:54  dkrajzew
// extension for artemis-import
//
// Revision 1.8  2003/03/06 17:18:41  dkrajzew
// debugging during vissim implementation
//
// Revision 1.7  2003/03/03 14:59:06  dkrajzew
// debugging; handling of imported traffic light definitions
//
// Revision 1.6  2003/02/13 15:51:54  dkrajzew
// functions for merging edges with the same origin and destination added
//
// Revision 1.5  2003/02/07 10:43:44  dkrajzew
// updated
//
// Revision 1.4  2002/10/22 10:08:28  dkrajzew
// unsupported return value patched
//
// Revision 1.3  2002/10/18 11:47:55  dkrajzew
// edges must only be sorted, when at least 2 are there...
//
// Revision 1.2  2002/10/17 13:40:10  dkrajzew
// typing of nodes during loading allwoed
//
// Revision 1.1  2002/10/16 15:48:13  dkrajzew
// initial commit for net building classes
//
// Revision 1.11  2002/07/25 08:38:27  dkrajzew
// problems on determination of a nodes type patched
//
// Revision 1.10  2002/07/02 09:01:41  dkrajzew
// Output of outgoing edges needed for the router added
//
// Revision 1.9  2002/06/21 13:17:51  dkrajzew
// Lane division bug (forgetting the turning direction; cause of segmentation violations under Linux) fixed
//
// Revision 1.8  2002/06/11 16:00:41  dkrajzew
// windows eol removed; template class definition inclusion depends now on the EXTERNAL_TEMPLATE_DEFINITION-definition
//
// Revision 1.7  2002/06/07 14:58:45  dkrajzew
// Bugs on dead ends and junctions with too few outgoing roads fixed; Comments improved
//
// Revision 1.6  2002/05/14 04:42:55  dkrajzew
// new computation flow
//
// Revision 1.5  2002/04/26 10:07:11  dkrajzew
// Windows eol removed; minor double to int conversions removed;
//
// Revision 1.4  2002/04/25 14:16:57  dkrajzew
// The unneeded iterator definition was again removed
//
// Revision 1.3  2002/04/25 14:15:07  dkrajzew
// The assignement of priorities of incoming edges improved; now, if having equal priorities, the opposite edges are chosen as higher priorised
//
// Revision 1.1.1.1  2002/04/09 14:18:27  dkrajzew
// new version-free project name (try2)
//
// Revision 1.3  2002/03/22 10:50:03  dkrajzew
// Memory leaks debugging added (MSVC++)
//
// Revision 1.2  2002/03/15 12:39:40  dkrajzew
// Handling of map logics removed
//
// Revision 1.1.1.1  2002/02/19 15:33:04  traffic
// Initial import as a separate application.
//
// Revision 1.1  2001/12/06 13:37:59  traffic
// files for the netbuilder
//
//
/* =========================================================================
 * debugging definitions (MSVC++ only)
 * ======================================================================= */
#ifdef _DEBUG
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection
   #define _INC_MALLOC	     // exclude standard memory alloc procedures
#endif


/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <cassert>
#include <algorithm>
#include <vector>
#include <deque>
#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <iomanip>
#include "NBNode.h"
#include "NBNodeCont.h"
#include "NBEdgeCont.h"
#include "NBTypeCont.h"
#include "NBJunctionLogicCont.h"
#include "NBHelpers.h"
#include "NBContHelper.h"
#include "NBLogicKeyBuilder.h"
#include "NBRequest.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


int NBNode::debug = 0;

/* =========================================================================
 * static variable definitions
 * ======================================================================= */
const int NBNode::TYPE_NOJUNCTION = 0;
const int NBNode::TYPE_TRAFFIC_LIGHT = 1;
const int NBNode::TYPE_PRIORITY_JUNCTION = 2;
const int NBNode::TYPE_RIGHT_BEFORE_LEFT = 3;
const int NBNode::TYPE_DEAD_END = -1;

int NBNode::_noNoJunctions = 0;
int NBNode::_noPriorityJunctions = 0;
int NBNode::_noTrafficLightJunctions = 0;
int NBNode::_noRightBeforeLeftJunctions = 0;


/* =========================================================================
 * method definitions
 * ======================================================================= */
/* -------------------------------------------------------------------------
 * NBNode::ApproachingDivider-methods
 * ----------------------------------------------------------------------- */
NBNode::ApproachingDivider::ApproachingDivider(
    std::vector<NBEdge*> *approaching, NBEdge *currentOutgoing)
    : _approaching(approaching), _currentOutgoing(currentOutgoing)
{
}


NBNode::ApproachingDivider::~ApproachingDivider()
{
}


void
NBNode::ApproachingDivider::execute(double src, double dest)
{
    assert(_approaching!=0&&_approaching->size()>src);
    NBEdge *incomingEdge = (*_approaching)[src];
    vector<size_t> approachingLanes =
        incomingEdge->getConnectionLanes(_currentOutgoing);
    assert(approachingLanes.size()!=0);
    deque<size_t> *approachedLanes = spread(approachingLanes, dest);
    assert(approachedLanes->size()<=_currentOutgoing->getNoLanes());
    // set lanes
    for(size_t i=0; i<approachedLanes->size(); i++) {
        size_t approached = (*approachedLanes)[i];
	    assert(approachedLanes->size()>i);
	    assert(approachingLanes.size()>i);
        incomingEdge->setConnection(approachingLanes[i], _currentOutgoing,
            approached);
    }
    delete approachedLanes;
}


deque<size_t> *
NBNode::ApproachingDivider::spread(const vector<size_t> &approachingLanes,
                                   double dest) const
{
    deque<size_t> *ret = new deque<size_t>();
    size_t noLanes = approachingLanes.size();
    // when only one lane is approached, we check, whether the double-value
    //  is assigned more to the left or right lane
    if(noLanes==1) {
        if((size_t) (dest+0.5)>(size_t) dest) {
            ret->push_back((size_t) dest+1);
        } else {
            ret->push_back((size_t) dest);
        }
        return ret;
    }

    size_t noOutgoingLanes = _currentOutgoing->getNoLanes();
    //
    ret->push_back((size_t) dest);
    size_t noSet = 1;
    double roffset = 1;
    double loffset = 1;
    while(noSet<noLanes) {
        // It may be possible, that there are not enough lanes the source
        //  lanes may be divided on
        //  In this case, they remain unset
        //  !!! this is only a hack. It is possible, that this yields in
        //   uncommon divisions
        if(noOutgoingLanes==noSet)
            return ret;

        // as due to the conversion of double->uint the numbers will be lower
        //  than they should be, we try to append to the left side first
        //
        // check whether the left boundary of the approached street has
        //  been overridden; if so, move all lanes to the right
        if(((size_t) dest+loffset)>=noOutgoingLanes) {
            loffset -= 1;
            roffset += 1;
            for(size_t i=0; i<ret->size(); i++) {
                (*ret)[i] = (*ret)[i] - 1;
            }
        }
        // append the next lane to the left of all edges
        //  increase the position (destination edge)
        ret->push_back((size_t) (dest+loffset));
        noSet++;
        loffset += 1;

        // as above
        if(noOutgoingLanes==noSet)
            return ret;

        // now we try to append the next lane to the right side, when needed
        if(noSet<noLanes) {
            // check whether the right boundary of the approached street has
            //  been overridden; if so, move all lanes to the right
            if(((size_t) dest-roffset)<0) {
                loffset += 1;
                roffset -= 1;
                for(size_t i=0; i<ret->size(); i++) {
                    (*ret)[i] = (*ret)[i] + 1;
                }
            }
            ret->push_front((size_t) (dest-roffset));
            noSet++;
            roffset += 1;
        }
    }
    return ret;
}



/* -------------------------------------------------------------------------
 * NBNode::SignalGroup-methods
 * ----------------------------------------------------------------------- */
NBNode::SignalGroup::SignalGroup(const std::string &id)
    : Named(id), myNoLinks(0)
{
}

NBNode::SignalGroup::~SignalGroup()
{
}

void 
NBNode::SignalGroup::addConnection(const Connection &c)
{
    myConnections.push_back(c);
    myNoLinks++;
}


void 
NBNode::SignalGroup::addPhaseBegin(double time, TLColor color) 
{
    myPhases.push_back(PhaseDef(time, color));
}


void 
NBNode::SignalGroup::setYellowTimes(double tRedYellow, 
                                    double tYellow)
{
    myTRedYellow = tRedYellow;
    myTYellow = tYellow;
}


void
NBNode::SignalGroup::sortPhases()
{
    sort(myPhases.begin(), myPhases.end(), 
        phase_by_time_sorter());
}


DoubleVector 
NBNode::SignalGroup::getTimes() const
{
    DoubleVector ret;
    for(GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        ret.push_back((*i).myTime);
    }
    return ret;
}


size_t 
NBNode::SignalGroup::getLinkNo() const
{
    return myNoLinks;
}


bool 
NBNode::SignalGroup::mayDrive(double time) const
{
    for(GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        double nextTime = (*i).myTime;
        if(nextTime>time) {
            if(i==myPhases.begin()) {
                return (*(myPhases.end()-1)).myColor==TLCOLOR_GREEN;
            } else {
                return (*(i-1)).myColor==TLCOLOR_GREEN;
            }
        }
    }
    return (*(myPhases.end()-1)).myColor==TLCOLOR_GREEN;
}


bool 
NBNode::SignalGroup::mustBrake(double time) const
{
    for(GroupsPhases::const_iterator i=myPhases.begin(); i!=myPhases.end(); i++) {
        double nextTime = (*i).myTime;
        if(nextTime>time) {
            if(i==myPhases.begin()) {
                return (*(myPhases.end()-1)).myColor==TLCOLOR_RED;
            } else {
                return (*(i-1)).myColor==TLCOLOR_RED;
            }
        }
    }
    return (*(myPhases.end()-1)).myColor==TLCOLOR_RED;
}


bool 
NBNode::SignalGroup::containsConnection(NBEdge *from, NBEdge *to) const
{
    for(ConnectionVector::const_iterator i=myConnections.begin(); i!=myConnections.end(); i++) {
        if((*i).first==from&&(*i).second==to) {
            return true;
        }
    }
    return false;
    
}


/* -------------------------------------------------------------------------
 * NBNode::Phase-methods
 * ----------------------------------------------------------------------- */
NBNode::Phase::Phase(const std::string &id, size_t begin, size_t end)
    : Named(id), myBegin(begin), myEnd(end)
{
}


NBNode::Phase::~Phase()
{
}

/*
void 
NBNode::Phase::addSignalGroupColor(const std::string &signalgroup, TLColor color)
{
    assert(_groupColors.find(signalgroup)==_groupColors.end());
    _groupColors[signalgroup] = color;
}
*/



/* -------------------------------------------------------------------------
 * NBNode-methods
 * ----------------------------------------------------------------------- */
NBNode::NBNode(const string &id, double x, double y)
    : _id(id), _x(x), _y(y), _type(-1)
{
    _incomingEdges = new EdgeVector();
    _outgoingEdges = new EdgeVector();
}


NBNode::NBNode(const string &id, double x, double y,
               const std::string &type)
    : _id(id), _x(x), _y(y), _type(-1)
{
    _incomingEdges = new EdgeVector();
    _outgoingEdges = new EdgeVector();
    if(type=="traffic_light") {
        _type = TYPE_TRAFFIC_LIGHT;
    } else if(type=="priority") {
        _type = TYPE_PRIORITY_JUNCTION;
    } else {
        _type = -1;
    }
}


NBNode::NBNode(const std::string &id, double x, double y, int type,
               const std::string &key)
    : _id(id), _x(x), _y(y), _key(key), _type(type)
{
    _incomingEdges = new EdgeVector();
    _outgoingEdges = new EdgeVector();
}


NBNode::~NBNode()
{
    delete _incomingEdges;
    delete _outgoingEdges;
}


double NBNode::getXCoordinate() {
    return _x;
}


double NBNode::getYCoordinate() {
    return _y;
}


void
NBNode::addIncomingEdge(NBEdge *edge)
{
    assert(edge!=0);
    if( find(_incomingEdges->begin(), _incomingEdges->end(), edge)
        ==_incomingEdges->end()) {

        _incomingEdges->push_back(edge);
    }
}


void
NBNode::addOutgoingEdge(NBEdge *edge)
{
    assert(edge!=0);
    if( find(_outgoingEdges->begin(), _outgoingEdges->end(), edge)
        ==_outgoingEdges->end()) {

        _outgoingEdges->push_back(edge);
    }
}


string
NBNode::getID()
{
    return _id;
}


EdgeVector *
NBNode::getIncomingEdges()
{
    return _incomingEdges;// !!!
}


EdgeVector *
NBNode::getOutgoingEdges()
{
    return _outgoingEdges;// !!!
}


const EdgeVector *
NBNode::getEdges()
{
    return &_allEdges;
}


void
NBNode::buildList()
{
    // compute angles of participating edges in relation to this junction
    EdgeVector::iterator i;
    // flip angles of incoming junctions by 180°
    for(i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
        NBEdge *edge = *i;
        double junctionAngle = edge->getAngle();
        if(junctionAngle<180) {
            junctionAngle = junctionAngle + 180;
        } else {
            junctionAngle = junctionAngle - 180;
        }
        if(junctionAngle>=360) {
            junctionAngle = junctionAngle - 360;
        }
        edge->setJunctionAngle(this, junctionAngle);
        _allEdges.push_back(edge);
    }
    // keep anges for outgoing angles
    for(i=_outgoingEdges->begin(); i!=_outgoingEdges->end(); i++) {
        NBEdge *edge = *i;
        edge->setJunctionAngle(this,edge->getAngle());
        _allEdges.push_back(edge);
    }
}


void
NBNode::sortSmall()
{
    if(_allEdges.size()==0) {
        return;
    }
    for( vector<NBEdge*>::iterator i=_allEdges.begin();
         i!=_allEdges.end()-1; i++) {
        swapWhenReversed(i, i+1);
    }
    if(_allEdges.size()>1) {
        swapWhenReversed(_allEdges.begin(), _allEdges.end()-1);
    }
}

void
NBNode::swapWhenReversed(const vector<NBEdge*>::iterator &i1,
                         const vector<NBEdge*>::iterator &i2) {
    NBEdge *e1 = *i1;
    NBEdge *e2 = *i2;
    if(e2->isTurningDirection(e1) && e2->getToNode()==this) {
        swap(*i1, *i2);
    }
}

void
NBNode::setPriorities()
{
    // reset all priorities
    vector<NBEdge*>::iterator i;
    // check if the junction is not a real junction
    if(_incomingEdges->size()==1&&_outgoingEdges->size()==1) {
        for(i=_allEdges.begin(); i!=_allEdges.end(); i++) {
            (*i)->setJunctionPriority(this, 1);
        }
    }
    // preset all junction's edge priorities to zero
    for(i=_allEdges.begin(); i!=_allEdges.end(); i++) {
        (*i)->setJunctionPriority(this, 0);
    }
    // compute the priorities on junction when needed
    if(_type==TYPE_PRIORITY_JUNCTION||_type==TYPE_TRAFFIC_LIGHT) {
        setPriorityJunctionPriorities();
    }
}


int
NBNode::computeType() const
{
// !!!
    if(_incomingEdges->size()==1&&_outgoingEdges->size()>0) {
//        cout << "HIer" << endl;
    }
// !!!

    // the type may already be set from the data
    if(_type>0) {
        return _type;
    }
    // check whether the junction is not a real junction
    if(_incomingEdges->size()==1/*&&_outgoingEdges->size()==1*/) {
        return TYPE_PRIORITY_JUNCTION; // !!! no junction?
    }
    // check whether the junction is a district and has no
    //  special meaning
    if(isDistrictCenter()) {
        return TYPE_NOJUNCTION;
    }
    // choose the uppermost type as default
    int type = TYPE_RIGHT_BEFORE_LEFT;
    // determine the type
    for( vector<NBEdge*>::const_iterator i=_allEdges.begin();
         i!=_allEdges.end(); i++) {
        for(vector<NBEdge*>::const_iterator j=i+1; j!=_allEdges.end(); j++) {
            // !!!
            // This usage of defaults is not very well, still we do not have any
            //  methods for the conversion of foreign, sometimes not supplied
            //  road types ino an own format
            // As default, TYPE_TRAFFIC_LIGHT is used, this should be valid for
            //  most coarse networks
            // !!!
            int tmptype = TYPE_TRAFFIC_LIGHT;
            try {
                tmptype = NBTypeCont::getJunctionType(
                    (*i)->getPriority(),
                    (*j)->getPriority());
            } catch (OutOfBoundsException) {
            }
            if(tmptype<type) {
                type = tmptype;
            }
        }
    }
    return type;
}


bool
NBNode::isDistrictCenter() const
{
    return _id.substr(0, 14)=="DistrictCenter";
}


void
NBNode::setPriorityJunctionPriorities() {
    if(_incomingEdges->size()==0) {
        return; // !!! what happens with outgoing edges
                //  (which priority should be assigned here)?
    }
    NBEdge *best1, *best2;
    best1 = best2 = 0;
    best1 = 0;
    vector<NBEdge*> incoming(*_incomingEdges);
    int noIncomingPrios = NBContHelper::countPriorities(incoming);
    // !!! Attention!
    // there is no case that fits into junctions with no incoming edges
        // extract the edge with the highest priority
    if(incoming.size()>0) {
        sort(incoming.begin(), incoming.end(),
            NBContHelper::edge_by_priority_sorter());
        best1 = extractAndMarkFirst(incoming);
    }
        // check whether a second main road exists
    if(incoming.size()>0) {
        noIncomingPrios = NBContHelper::countPriorities(incoming);
        if(noIncomingPrios>1) {
            sort(incoming.begin(), incoming.end(),
                NBContHelper::edge_by_priority_sorter());
            best2 = extractAndMarkFirst(incoming);
        }
    }
    // get the best continuations (outgoing edges)
    NBEdge *bestBack1, *bestBack2;
    bestBack1 = bestBack2 = 0;
    vector<NBEdge*> outgoing(*_outgoingEdges);
    // for best1
    if(outgoing.size()>0) {
        sort(outgoing.begin(), outgoing.end(),
            NBContHelper::edge_similar_direction_sorter(best1));
        bestBack1 = extractAndMarkFirst(outgoing);
    }
    // for best2
    if(best2!=0&&outgoing.size()>0) {
        sort(outgoing.begin(), outgoing.end(),
            NBContHelper::edge_similar_direction_sorter(best1));
        bestBack1 = extractAndMarkFirst(outgoing);
    }


    /*
    if(noIncomingPrios==1) {
        // in this case, all incoming edges are equal in their priority
        // --> choose any as the main
        best1 = extractAndMarkFirst(incoming);
        // try to get the incoming edge in the opposite geometrical direction
        if(incoming.size()>0) {
            sort(incoming.begin(), incoming.end(),
                NBContHelper::edge_opposite_direction_sorter(best1));
            best2 = extractAndMarkFirst(incoming);
        }
    } else {
        // in this case, there is at least one higher priorised edge
        // --> choose this as main
        sort(incoming.begin(), incoming.end(),
            NBContHelper::edge_by_priority_sorter());
        best1 = extractAndMarkFirst(incoming);
        // now check whether to use another high priorised edge or choose one
        // of the lower priorised edges; it is also possible that there are
        // more than one higher priorised edges
        noIncomingPrios = NBContHelper::countPriorities(incoming);
        if(noIncomingPrios==1) {
            // all other incoming edges have the same priority; choose the one
            // in the opposite direction
            if(incoming.size()>0) {
                sort(incoming.begin(), incoming.end(),
                    NBContHelper::edge_opposite_direction_sorter(best1));
                best2 = extractAndMarkFirst(incoming);
            }
        } else {
            // choose one of the highest priorised edges that is in the most
            // opposite direction to the first chosen
            if(incoming.size()>0) {
                vector<NBEdge*> best = getMostPriorised(incoming);
                sort(best.begin(), best.end(),
                    NBContHelper::edge_opposite_direction_sorter(best1));
                best2 = extractAndMarkFirst(best);
            }
        }
    }
    // now process the outgoing edges
    vector<NBEdge*> outgoing(*_outgoingEdges);
    NBEdge *bestBack1, *bestBack2;
    bestBack1 = bestBack2 = 0;
    // for both edges, try to find the edges in the other direction
    if(outgoing.size()>1) {
        sort(outgoing.begin(), outgoing.end(),
            NBContHelper::edge_opposite_direction_sorter(best1));
        bestBack1 = extractAndMarkFirst(outgoing);
    }
    if(outgoing.size()>0&&best2!=0) {
        sort(outgoing.begin(), outgoing.end(),
            NBContHelper::edge_opposite_direction_sorter(best2));
        bestBack2 = extractAndMarkFirst(outgoing);
    }
    */
}


int
NBNode::getHighestPriority(const vector<NBEdge*> &s) {
    if(s.size()==0) {
        return 0;
    }
    vector<int> knownPrios;
    for(vector<NBEdge*>::const_iterator i=s.begin(); i!=s.end(); i++) {
        knownPrios.push_back((*i)->getPriority());
    }
    sort(knownPrios.begin(), knownPrios.end());
    return knownPrios[0];
}


NBEdge*
NBNode::extractAndMarkFirst(vector<NBEdge*> &s) {
    if(s.size()==0) {
        return 0;
    }
    NBEdge *ret = s.front();
    s.erase(s.begin());
    ret->setJunctionPriority(this, 1);
    return ret;
}


vector<NBEdge*>
NBNode::getMostPriorised(vector<NBEdge*> &s)
{
    if(s.size()==0) {
        return vector<NBEdge*>();
    }
    sort(s.begin(), s.end(), NBContHelper::edge_by_priority_sorter());
    vector<NBEdge*> ret;
    int highestP = (*(s.begin()))->getPriority();
    for(vector<NBEdge*>::iterator i=s.begin(); i!=s.end(); i++) {
        if(highestP==(*i)->getPriority())
            ret.push_back(*i);
    }
    return ret;
}


void
NBNode::rotateIncomingEdges(int norot)
{
    if(_incomingEdges->size()==0) {
        return;
    }
    while(norot>0) {
        NBEdge *e = (*_incomingEdges)[0];
        unsigned int i;
        for(i=0; i<_incomingEdges->size()-1; i++) {
            assert(_incomingEdges!=0&&_incomingEdges->size()>i+1);
            (*_incomingEdges)[i] = (*_incomingEdges)[i+1];
        }
        assert(_incomingEdges!=0&&i<_incomingEdges->size());
        (*_incomingEdges)[i] = e;
        norot--;
    }
}


void
NBNode::writeXML(ostream &into)
{
    // write the attributes
    into << "   <junction id=\"" << _id << '\"';
    if(_incomingEdges->size()!=0&&_outgoingEdges->size()!=0) {
        //into << " key=\"" << _key << '\"';
        switch(_type) {
        case TYPE_NOJUNCTION:
            into << " type=\"" << "none\"";
            break;
        case TYPE_TRAFFIC_LIGHT:
            into << " type=\"" << "traffic_light\"";
            break;
        case TYPE_PRIORITY_JUNCTION:
            into << " type=\"" << "priority\"";
            break;
        case TYPE_RIGHT_BEFORE_LEFT:
            into << " type=\"" << "right_before_left\"";
            break;
        default:
            throw exception();
            break;
        }
    } else {
        into << " type=\"DEAD_END\"";
    }
    into << " x=\"" << setprecision( 2 ) << _x
        << "\" y=\"" << setprecision( 2 ) << _y << "\"";
    into <<  ">" << endl;
    // write additional information about the traffic light presettings
    //  when the junction is a traffic light
    if(_type==TYPE_TRAFFIC_LIGHT) {
        into << "      <offset>" << 0 << "</offset>" << endl;
        into << "      <initstep>" << 0 << "</initstep>" << endl;
    }
    // write the inlanes
    EdgeVector::iterator i;
    into << "      <inlanes>";
    for(i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
        size_t noLanes = (*i)->getNoLanes();
        string id = (*i)->getID();
        for(size_t j=0; j<noLanes; j++) {
            into << id << '_' << j;
            if(i!=_incomingEdges->end()-1 || j<noLanes-1)
                into << ' ';
        }
    }
    into << "</inlanes>" << endl;
    // close writing
    into << "   </junction>" << endl << endl;
}


void
NBNode::setKey(string key)
{
    _key = key;
}


void
NBNode::computeLogic(long maxSize)
{
    if(_incomingEdges->size()==0||_outgoingEdges->size()==0) {
        return;
    }
    // build the request
    NBRequest *request = new NBRequest(this,
        static_cast<const EdgeVector * const>(&_allEdges),
        static_cast<const EdgeVector * const>(_incomingEdges),
        static_cast<const EdgeVector * const>(_outgoingEdges),
        _blockedConnections);
    // compute the logic if necessary or split the junction
    if(_type!=TYPE_NOJUNCTION) {
        computeLogic(request, maxSize);
    } else {
        computeLogic(request, maxSize); // !!! do something else
    }
    // build the lights when needed
    if(_type==TYPE_TRAFFIC_LIGHT) {
        int build = request->buildTrafficLight(_id, mySignalGroups, myCycleDuration); // _key
        if(build==0) {
            _type==TYPE_PRIORITY_JUNCTION;
        }
    }
    // close node computation
    delete request;
}


void
NBNode::setType(int type)
{
    // !!! TRAFFIC_LIGHT and NO_JUNCTION are not supprted by now
    switch(type) {
    case TYPE_NOJUNCTION:
        _noNoJunctions++;
        break;
    case TYPE_TRAFFIC_LIGHT:
        _noTrafficLightJunctions++;
        break;
    case TYPE_PRIORITY_JUNCTION:
        _noPriorityJunctions++;
        break;
    case TYPE_RIGHT_BEFORE_LEFT:
        _noRightBeforeLeftJunctions++;
        break;
    default:
        throw exception();
    }
    // some types are planned but not yet supported;
    //  convert these types into known types
    if(/*type==TYPE_TRAFFIC_LIGHT||*/type==TYPE_NOJUNCTION) {
        type = TYPE_PRIORITY_JUNCTION;
    }
    _type = type;
}


void
NBNode::reportBuild()
{
    cout << "No Junctions (converted)    : " 
        << _noNoJunctions << endl;
    cout << "Traffic Light Junctions     : " 
        << _noTrafficLightJunctions << endl;
    cout << "Priority Junctions          : " 
        << _noPriorityJunctions << endl;
    cout << "Right Before Left Junctions : " 
        << _noRightBeforeLeftJunctions << endl;
}


void
NBNode::sortNodesEdges()
{
    buildList();
    //prepareForBuilding();
    sort(_allEdges.begin(), _allEdges.end(), NBContHelper::edge_by_junction_angle_sorter(this));
    sort(_incomingEdges->begin(), _incomingEdges->end(), NBContHelper::edge_by_junction_angle_sorter(this));
    sort(_outgoingEdges->begin(), _outgoingEdges->end(), NBContHelper::edge_by_junction_angle_sorter(this));
    sortSmall();
    setType(computeType());
    setPriorities();
}


void
NBNode::computeLogic(NBRequest *request, long maxSize)
{
/*    if(_id=="318") {
        EdgeVector::iterator i;
        for(i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
            if(i!=_incomingEdges->begin()) {
                cout << ", ";
            }
            cout << (*i)->getID();
        }
        cout << endl;
        for(i=_outgoingEdges->begin(); i!=_outgoingEdges->end(); i++) {
            if(i!=_outgoingEdges->begin()) {
                cout << ", ";
            }
            cout << (*i)->getID();
        }
        cout << endl;
    }
*/
/*    string key = NBLogicKeyBuilder::buildKey(this, &_allEdges);
    int norot = NBJunctionLogicCont::try2convert(key);
    if(norot>=0) {
        rotateIncomingEdges(norot);
        key = NBLogicKeyBuilder::rotateKey(key, norot);
    } //else {
    cout << key << endl;*/
    if(_id=="10") {
        int bla = 0;
    }
        request->buildBitfieldLogic(_id);
    //}
    //setKey(key);
}


void
NBNode::computeEdges2Lanes()
{
    // go through this node's outgoing edges
    //  for every outgoing edge, compute the distribution of the node's
    //  incoming edges on this edge when approaching this edge
    for( vector<NBEdge*>::reverse_iterator i=_outgoingEdges->rbegin();
         i!=_outgoingEdges->rend(); i++) {
        NBEdge *currentOutgoing = *i;
        // get the information about edges that do approach this edge
        vector<NBEdge*> *approaching = getApproaching(currentOutgoing);
        if(approaching->size()!=0) {
            ApproachingDivider *divider =
                new ApproachingDivider(approaching, currentOutgoing);
            Bresenham::compute(divider, approaching->size(),
                currentOutgoing->getNoLanes());
            delete divider;
        }
        delete approaching;
    }
}


vector<NBEdge*> *
NBNode::getApproaching(NBEdge *currentOutgoing)
{
    // get the position of the node to get the approaching nodes of
    vector<NBEdge*>::const_iterator i = find(_allEdges.begin(),
        _allEdges.end(), currentOutgoing);
    // get the first possible approaching edge
    i = NBContHelper::nextCW(&_allEdges, i);
    // go through the list of edges clockwise and add the edges
    vector<NBEdge*> *approaching = new vector<NBEdge*>();
    for(; *i!=currentOutgoing; ) {
        // check only incoming edges
        if((*i)->getToNode()==this) {
            vector<size_t> connLanes =
                (*i)->getConnectionLanes(currentOutgoing);
            if(connLanes.size()!=0) {
                approaching->push_back(*i);
            }
        }
        i = NBContHelper::nextCW(&_allEdges, i);
    }
    return approaching;
}


std::string
NBNode::setTurningDefinition(NBNode *from, NBNode *to)
{
    EdgeVector::iterator i;
    // get the source edge
    NBEdge *src = 0;
    for(i=_incomingEdges->begin(); src==0 && i!=_incomingEdges->end(); i++) {
        if((*i)->getFromNode()==from) {
            src = (*i);
        }
    }
    if(src==0) {
        return string("There is no edge from node '") + from->getID()
            + string("' to node '") + getID() + string("'.");
    }
    // get the destination edge
    NBEdge *dest = 0;
    for(i=_outgoingEdges->begin(); dest==0 && i!=_outgoingEdges->end(); i++) {
        if((*i)->getToNode()==to) {
            dest = (*i);
        }
    }
    if(dest==0) {
        return string("There is no edge from node '") + getID()
            + string("' to node '") + to->getID() + string("'.");
    }
    // both edges found
    //  set them into the edge
    src->addEdge2EdgeConnection(dest);
    // no error occured
    return "";
}


void
NBNode::resetby(double xoffset, double yoffset)
{
    _x += xoffset;
    _y += yoffset;
}


void
NBNode::replaceOutgoing(NBEdge *which, NBEdge *by)
{
    size_t i;
    // replace the edge in the list of outgoing nodes
    for(i=0; i<_outgoingEdges->size(); i++) {
        if((*_outgoingEdges)[i]==which) {
            (*_outgoingEdges)[i] = by;
        }
    }
    // replace the edge in connections of incoming edges
    for(i=0; i<_incomingEdges->size(); i++) {
        (*_incomingEdges)[i]->replaceInConnections(which, by);
    }
    // replace within the connetion prohibition dependencies
    replaceInConnectionProhibitions(which, by);   
}


void
NBNode::replaceIncoming(NBEdge *which, NBEdge *by)
{
    // replace the edge in the list of incoming nodes
    for(size_t i=0; i<_incomingEdges->size(); i++) {
        if((*_incomingEdges)[i]==which) {
            (*_incomingEdges)[i] = by;
        }
    }
    // add connections of the old edge to the new one
    EdgeVector connected = which->getConnected();
    for(EdgeVector::const_iterator j=connected.begin(); j!=connected.end(); j++) {
        by->addEdge2EdgeConnection(*j);
    }
    // replace within the connetion prohibition dependencies
    replaceInConnectionProhibitions(which, by);   
}


void
NBNode::replaceInConnectionProhibitions(NBEdge *which, NBEdge *by)
{
    // replace in keys
    ConnectionProhibits::iterator j = _blockedConnections.begin();
    while(j!=_blockedConnections.end()) {
        bool changed = false;
        Connection c((*j).first);
        if(c.first==which) {
            changed = true;
            c.first = by;
        }
        if(c.second==which) {
            changed = true;
            c.second = by;
        }
        if(changed) {
            _blockedConnections[c] = (*j).second;
            _blockedConnections.erase(j);
            j = _blockedConnections.begin();
        } else {
            j++;
        }
    }
    // replace in values
    for(j=_blockedConnections.begin(); j!=_blockedConnections.end(); j++) {
        const Connection &prohibited = (*j).first;
        ConnectionVector &prohibiting = (*j).second;
        for(ConnectionVector::iterator k=prohibiting.begin(); k!=prohibiting.end(); k++) {
            Connection &sprohibiting = *k;
            if(sprohibiting.first==which) {
                sprohibiting.first = by;
            }
            if(sprohibiting.second==which) {
                sprohibiting.second = by;
            }
        }
    }
}



void
NBNode::removeDoubleEdges()
{
  //  if(_id=="1483") {
/*        cout << "Remove Double:" << endl;
        for(size_t j=0; j<_outgoingEdges->size(); j++) {
            if(j!=0) {
                cout << ", ";
            }
            cout << (*_outgoingEdges)[j]->getID();
        }
        cout << endl;*/
//    }
    EdgeVector::iterator i;
    // check incoming
    size_t pos = 0;
    for(i=_incomingEdges->begin(); _incomingEdges->size()!=0&&i!=_incomingEdges->end()-1;) {
        EdgeVector::iterator j = find(i+1, _incomingEdges->end(), *i);
        if(j!=_incomingEdges->end()) {
            _incomingEdges->erase(j);
            i = _incomingEdges->begin() + pos;
        } else {
            pos++;
            i++;
        }
    }
    // check outgoing
    pos = 0;
    for(i=_outgoingEdges->begin(); _outgoingEdges->size()!=0&&i!=_outgoingEdges->end()-1; ) {
        EdgeVector::iterator j = find(i+1, _outgoingEdges->end(), *i);
        if(j!=_outgoingEdges->end()) {
            _outgoingEdges->erase(j);
            i = _outgoingEdges->begin() + pos;
        } else {
            pos++;
            i++;
        }
    }
//    if(_id=="1483") {
/*        for(j=0; j<_outgoingEdges->size(); j++) {
            if(j!=0) {
                cout << ", ";
            }
            cout << (*_outgoingEdges)[j]->getID();
        }
        cout << endl;*/
//    }
}


bool
NBNode::hasOutgoing(NBEdge *e) const
{
    return find(_outgoingEdges->begin(), _outgoingEdges->end(), e)
        !=
        _outgoingEdges->end();
}


bool
NBNode::hasIncoming(NBEdge *e) const
{
    return find(_incomingEdges->begin(), _incomingEdges->end(), e)
        !=
        _incomingEdges->end();
}

/*
bool
NBNode::hasSingleIncoming() const
{
    for(EdgeVector::iterator i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
        NBEdge *e = *i;
    }
    return _incomingEdges->size()==1;
}


bool
NBNode::hasSingleOutgoing() const
{
    return _outgoingEdges->size()==1;
}


NBEdge *
NBNode::getSingleIncoming() const
{
    return (*_incomingEdges)[0];
}


NBEdge *
NBNode::getSingleOutgoing() const
{
    return (*_outgoingEdges)[0];
}

*/


NBEdge *
NBNode::getOppositeIncoming(NBEdge *e) const
{
    EdgeVector edges(*_incomingEdges);
    sort(edges.begin(), edges.end(),
        NBContHelper::edge_similar_direction_sorter(e));
    return edges[0];
}


NBEdge *
NBNode::getOppositeOutgoing(NBEdge *e) const
{
    EdgeVector edges(*_outgoingEdges);
    sort(edges.begin(), edges.end(),
        NBContHelper::edge_similar_direction_sorter(e));
    return edges[0];
}


void 
NBNode::addSortedLinkFoes(const std::pair<NBEdge*, NBEdge*> &mayDrive,
                          const std::pair<NBEdge*, NBEdge*> &mustStop)
{
    if(mayDrive.first==0||mayDrive.second==0||mustStop.first==0||mustStop.second==0) {
        cout << "Something went wrong during the building of a connection..." << endl;
        cout << "  Continuing..." << endl;
        return; // !!! mark to recompute connections
    }
    assert(mayDrive.first!=0);
    assert(mayDrive.second!=0);
    assert(mustStop.first!=0);
    assert(mustStop.second!=0);
    ConnectionVector conn = _blockedConnections[mustStop];
    conn.push_back(mayDrive);
    _blockedConnections[mustStop] = conn;
}


NBEdge *
NBNode::getPossiblySplittedIncoming(const std::string &edgeid)
{
    size_t size = edgeid.length();
    for(EdgeVector::iterator i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
        string id = (*i)->getID();
        if(id.substr(0, size)==edgeid) {
            return *i;
        }
    }
    return 0;
}


NBEdge *
NBNode::getPossiblySplittedOutgoing(const std::string &edgeid)
{
    size_t size = edgeid.length();
    for(EdgeVector::iterator i=_outgoingEdges->begin(); i!=_outgoingEdges->end(); i++) {
        string id = (*i)->getID();
        if(id.substr(0, size)==edgeid) {
            return *i;
        }
    }
    return 0;
}


void
NBNode::eraseDummies()
{
    if(_outgoingEdges==0||_incomingEdges==0) {
        return;
    }
    size_t pos = 0;
    EdgeVector::const_iterator j=_incomingEdges->begin();
    while(j!=_incomingEdges->end()) {
        // skip edges which are only incoming and not outgoing
        if(find(_outgoingEdges->begin(), _outgoingEdges->end(), *j)==_outgoingEdges->end()) {
            j++;
            pos++;
            continue;
        }
        // an edge with both its origin and destination being the current 
        //  node should be removed
        NBEdge *dummy = *j;
        // get the list of incoming edges connected to the dummy
        EdgeVector incomingConnected;
        for(EdgeVector::const_iterator i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
            if((*i)->isConnectedTo(dummy)) {
                incomingConnected.push_back(*i);
            }
        }
        // let the dummy remap its connections
        dummy->remapConnections(incomingConnected);
//        dummy->removeFromProceeding();
        // delete the dummy
        NBEdgeCont::erase(dummy);
        j = _incomingEdges->begin() + pos;
    }
}


void 
NBNode::removeOutgoing(NBEdge *edge)
{
    EdgeVector::iterator i =
        find(_outgoingEdges->begin(), _outgoingEdges->end(), edge);
    if(i!=_outgoingEdges->end()) {
        _outgoingEdges->erase(i);
    }
}


void 
NBNode::removeIncoming(NBEdge *edge)
{
    EdgeVector::iterator i =
        find(_incomingEdges->begin(), _incomingEdges->end(), edge);
    if(i!=_incomingEdges->end()) {
        _incomingEdges->erase(i);
    }
}




void 
NBNode::addToSignalGroup(const std::string &groupid, 
                         const Connection &connection)
{
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->addConnection(connection);
}


void 
NBNode::addToSignalGroup(const std::string &groupid, 
                         const ConnectionVector &connections)
{
    for(ConnectionVector::const_iterator i=connections.begin(); i!=connections.end(); i++) {
        addToSignalGroup(groupid, *i);
    }
}


void 
NBNode::addSignalGroup(const std::string &id)
{
    assert(mySignalGroups.find(id)==mySignalGroups.end());
    mySignalGroups[id] = new SignalGroup(id);
}


void 
NBNode::addSignalGroupPhaseBegin(const std::string &groupid, double time, 
                                 TLColor color)
{
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->addPhaseBegin(time, color);
}

void
NBNode::setSignalYellowTimes(const std::string &groupid, 
                             double myTRedYellow, double myTYellow)
{
    assert(mySignalGroups.find(groupid)!=mySignalGroups.end());
    mySignalGroups[groupid]->setYellowTimes(myTRedYellow, myTYellow);
}


void 
NBNode::setCycleDuration(size_t cycleDur)
{
    myCycleDuration = cycleDur;
}


NBNode::SignalGroup *
NBNode::findGroup(const NBNode::SignalGroupCont &defs, 
                  NBEdge *from, NBEdge *to)
{
    for(SignalGroupCont::const_iterator i=defs.begin(); i!=defs.end(); i++) {
        if((*i).second->containsConnection(from, to)) {
            return (*i).second;
        }
    }
    return 0;
}


Position2D 
NBNode::getEmptyDir() const
{
    Position2D pos(0, 0);
    EdgeVector::const_iterator i;
    for(i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
        NBNode *conn = (*i)->getFromNode();
        Position2D toAdd = 
            Position2D(conn->getXCoordinate()-_x, 
                conn->getYCoordinate()-_y);
        toAdd.mul(1.0/sqrt(toAdd.x()*toAdd.x()+toAdd.y()*toAdd.y()));
        pos.add(toAdd);
    }
    for(i=_outgoingEdges->begin(); i!=_outgoingEdges->end(); i++) {
        NBNode *conn = (*i)->getToNode();
        Position2D toAdd = 
            Position2D(conn->getXCoordinate()-_x, 
                conn->getYCoordinate()-_y);
        toAdd.mul(1.0/sqrt(toAdd.x()*toAdd.x()+toAdd.y()*toAdd.y()));
        pos.add(toAdd);
    }
    pos.mul(-1.0/(_incomingEdges->size()+_outgoingEdges->size()));
    return pos;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBNode.icc"
//#endif

// Local Variables:
// mode:C++
// End:
