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
   #define _CRTDBG_MAP_ALLOC // include Microsoft memory leak detection procedures
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
/* =========================================================================
 * NBNode::ApproachingDivider-methods
 * ======================================================================= */
NBNode::ApproachingDivider::ApproachingDivider(std::vector<NBEdge*> *approaching,
                                               NBEdge *currentOutgoing)
    : _approaching(approaching), _currentOutgoing(currentOutgoing)
{
}

NBNode::ApproachingDivider::~ApproachingDivider()
{
}

void
NBNode::ApproachingDivider::execute(double src, double dest)
{
    assert(_approaching->size()>src);
    NBEdge *incomingEdge = (*_approaching)[src];
    vector<size_t> approachingLanes = incomingEdge->getConnectionLanes(_currentOutgoing);
    assert(approachingLanes.size()!=0);
    deque<size_t> *approachedLanes = spread(approachingLanes, dest);
    assert(approachedLanes->size()<=_currentOutgoing->getNoLanes());
    // set lanes
    for(size_t i=0; i<approachedLanes->size(); i++) {
        size_t approached = (*approachedLanes)[i];
	assert(approachedLanes->size()>i);
	assert(approachingLanes.size()>i);
        incomingEdge->setConnection(approachingLanes[i], _currentOutgoing, approached);
    }
    delete approachedLanes;
}

deque<size_t> *
NBNode::ApproachingDivider::spread(const vector<size_t> &approachingLanes, double dest) const
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



/* =========================================================================
 * NBNode-methods
 * ======================================================================= */
NBNode::NBNode(const string &id, double x, double y)
    : _id(id), _x(x), _y(y), _type(-1)
{
    _incomingEdges = new EdgeCont();
    _outgoingEdges = new EdgeCont();
}

NBNode::NBNode(const string &id, double x, double y, 
               const std::string &type)
    : _id(id), _x(x), _y(y), _type(-1)
{
    _incomingEdges = new EdgeCont();
    _outgoingEdges = new EdgeCont();
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
    _incomingEdges = new EdgeCont();
    _outgoingEdges = new EdgeCont();
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
    _incomingEdges->push_back(edge);
}


void
NBNode::addOutgoingEdge(NBEdge *edge)
{
    _outgoingEdges->push_back(edge);
}

string
NBNode::getID()
{
    return _id;
}


EdgeCont *
NBNode::getIncomingEdges()
{
    return _incomingEdges;// !!!
}


EdgeCont *
NBNode::getOutgoingEdges()
{
    return _outgoingEdges;// !!!
}

const EdgeCont *
NBNode::getEdges()
{
    return &_allEdges;
}

void
NBNode::buildList()
{
    EdgeCont::iterator i;
    for(i=_incomingEdges->begin(); i!=_incomingEdges->end(); i++) {
        NBEdge *edge = *i;
        double junctionAngle = edge->getAngle();
        if(junctionAngle<180)
            junctionAngle = junctionAngle + 180;
        else
            junctionAngle = junctionAngle - 180;
        if(junctionAngle>=360)
            junctionAngle = junctionAngle - 360;
        edge->setJunctionAngle(this, junctionAngle);
        _allEdges.push_back(edge);
    }
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
    for(vector<NBEdge*>::iterator i=_allEdges.begin(); i!=_allEdges.end()-1; i++) {
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
    // the type may already be set from the data
    if(_type>0) {
        return _type;
    }
    // check whether the junction is not a real junction
    if(_incomingEdges->size()==1&&_outgoingEdges->size()==1) {
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
    for(vector<NBEdge*>::const_iterator i=_allEdges.begin(); i!=_allEdges.end(); i++) {
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
    NBEdge *best1, *best2;
    best1 = best2 = 0;
    vector<NBEdge*> incoming(*_incomingEdges);
    int noIncomingPrios = NBContHelper::countPriorities(incoming);
    // !!! Attention!
    // there is no case that fits into junctions with no incoming edges
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
        sort(incoming.begin(), incoming.end(), NBContHelper::edge_by_priority_sorter());
        best1 = extractAndMarkFirst(incoming);
        // now check whether to use another high priorised edge or choose one of
        // the lower priorised edges; it is also possible that there are more than one
        // higher priorised edges
        noIncomingPrios = NBContHelper::countPriorities(incoming);
        if(noIncomingPrios==1) {
            // all other incoming edges have the same priority; choose the one in the
            // opposite direction
            if(incoming.size()>0) {
                sort(incoming.begin(), incoming.end(),
                    NBContHelper::edge_opposite_direction_sorter(best1));
                best2 = extractAndMarkFirst(incoming);
            }
        } else {
            // choose one of the highest priorised edges that is in the most opposite
            // direction to the first chosen
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
}


int
NBNode::getHighestPriority(const vector<NBEdge*> &s) {
    if(s.size()==0)
        return 0;
    vector<int> knownPrios;
    for(vector<NBEdge*>::const_iterator i=s.begin(); i!=s.end(); i++) {
        knownPrios.push_back((*i)->getPriority());
    }
    sort(knownPrios.begin(), knownPrios.end());
    return knownPrios[0];
}

NBEdge*
NBNode::extractAndMarkFirst(vector<NBEdge*> &s) {
    if(s.size()==0)
        return 0;
    NBEdge *ret = s.front();
    s.erase(s.begin());
    ret->setJunctionPriority(this, 1);
    return ret;
}

vector<NBEdge*>
NBNode::getMostPriorised(vector<NBEdge*> &s) {
    if(s.size()==0)
        return vector<NBEdge*>();
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
    if(_incomingEdges->size()==0) return;
    while(norot>0) {
        NBEdge *e = (*_incomingEdges)[0];
        unsigned int i;
        for(i=0; i<_incomingEdges->size()-1; i++) {
            (*_incomingEdges)[i] = (*_incomingEdges)[i+1];
        }
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
    EdgeCont::iterator i;
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
NBNode::computeLogic(long maxSize) {
    if(_incomingEdges->size()==0||_outgoingEdges->size()==0)
        return;
    // build the request
    NBRequest *request = new NBRequest(this,
        static_cast<const EdgeCont * const>(&_allEdges),
        static_cast<const EdgeCont * const>(_incomingEdges),
        static_cast<const EdgeCont * const>(_outgoingEdges));
    // compute the logic if necessary or split the junction
    if(_type!=TYPE_NOJUNCTION) {
        computeLogic(request, maxSize);
    } else {
        computeLogic(request, maxSize); // !!! do something else
    }
    // build the lights when needed
    if(_type==TYPE_TRAFFIC_LIGHT) {
        request->buildTrafficLight(_id); // _key
    }
    // close node computation
    delete request;
}

void
NBNode::setType(int type) {
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
NBNode::reportBuild() {
    cout << "No Junctions (converted)    : " << _noNoJunctions << endl;
    cout << "Traffic Light Junctions     : " << _noTrafficLightJunctions
        << endl;
    cout << "Priority Junctions          : " << _noPriorityJunctions << endl;
    cout << "Right Before Left Junctions : " << _noRightBeforeLeftJunctions
        << endl;
}

void
NBNode::sortNodesEdges() {
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
NBNode::computeLogic(NBRequest *request, long maxSize) {
/*    string key = NBLogicKeyBuilder::buildKey(this, &_allEdges);
    int norot = NBJunctionLogicCont::try2convert(key);
    if(norot>=0) {
        rotateIncomingEdges(norot);
        key = NBLogicKeyBuilder::rotateKey(key, norot);
    } //else {
    cout << key << endl;*/
        request->buildBitfieldLogic(_id);
    //}
    //setKey(key);
}

void
NBNode::computeEdges2Lanes() {
    if(_id=="I880_2") {
        int bla = 0;
    }
    // go through this node's outgoing edges
    //  for every outgoing edge, compute the distribution of the node's
    //  incoming edges on this edge when approaching this edge
    for(vector<NBEdge*>::reverse_iterator i=_outgoingEdges->rbegin(); i!=_outgoingEdges->rend(); i++) {
        NBEdge *currentOutgoing = *i;
        // get the information about edges that do approach this edge
        vector<NBEdge*> *approaching = getApproaching(currentOutgoing);
        if(approaching->size()!=0) {
            ApproachingDivider *divider = new ApproachingDivider(approaching, currentOutgoing);
            Bresenham::compute(divider, approaching->size(), currentOutgoing->getNoLanes());
            delete divider;
        }
        delete approaching;
    }
}

vector<NBEdge*> *
NBNode::getApproaching(NBEdge *currentOutgoing)
{
    // get the position of the node to get the approaching nodes of
    vector<NBEdge*>::const_iterator i = find(_allEdges.begin(), _allEdges.end(), currentOutgoing);
    // get the first possible approaching edge
    i = NBContHelper::nextCW(&_allEdges, i);
    // go through the list of edges clockwise and add the edges
    vector<NBEdge*> *approaching = new vector<NBEdge*>();
    for(; *i!=currentOutgoing; ) {
        // check only incoming edges
        if((*i)->getToNode()==this) {
            vector<size_t> connLanes = (*i)->getConnectionLanes(currentOutgoing);
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
    EdgeCont::iterator i;
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NBNode.icc"
//#endif

// Local Variables:
// mode:C++
// End:
