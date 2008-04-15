/****************************************************************************/
/// @file    NBEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Methods for the representation of a single edge
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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
#include <iomanip>
#include "NBTypeCont.h"
#include <utils/geom/GeomHelper.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StdDefs.h>
#include "NBEdge.h"
#include <utils/options/OptionsCont.h>
#include <utils/geom/GeoConvHelper.h>
#include <utils/iodevices/OutputDevice.h>

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
NBEdge::ToEdgeConnectionsAdder::execute(SUMOReal lane, SUMOReal virtEdge) throw()
{
    // check
    assert(virtEdge>=0);
    assert(myTransitions.size()>(size_t) virtEdge);
    assert(lane>=0&&lane<10);
    // get the approached edge
    NBEdge *succEdge = myTransitions[(int) virtEdge];
    vector<size_t> lanes;

    // check whether the currently regarded, approached edge has already
    //  a connection starting at the edge which is currently being build
    map<NBEdge*, vector<size_t> >::iterator i=myConnections->find(succEdge);
    if (i!=myConnections->end()) {
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
    (*myConnections)[succEdge] = lanes;
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
        myDirs.push_back(MainDirections::DIR_RIGHTMOST);
    }
    // check whether the left turn has a higher priority
    if (outgoing[outgoing.size()-1]->getJunctionPriority(to)==1) {
        // ok, the left turn belongs to the higher priorised edges on the junction
        //  let's check, whether it has also a higher priority (lane number/speed)
        //  than the current
        vector<NBEdge*> tmp(outgoing);
        sort(tmp.begin(), tmp.end(), NBContHelper::edge_similar_direction_sorter(parent));
        if (outgoing[outgoing.size()-1]->getPriority()>tmp[0]->getPriority()) {
            myDirs.push_back(MainDirections::DIR_LEFTMOST);
        } else {
            if (outgoing[outgoing.size()-1]->getNoLanes()>tmp[0]->getNoLanes()) {
                myDirs.push_back(MainDirections::DIR_LEFTMOST);
            }
        }
    }
    // check whether the forward direction has a higher priority
    //  try to get the forward direction
    vector<NBEdge*> tmp(outgoing);
    sort(tmp.begin(), tmp.end(),
         NBContHelper::edge_similar_direction_sorter(parent));
    NBEdge *edge = *(tmp.begin());
    // check whether it has a higher priority and is going straight
    if (edge->getJunctionPriority(to)==1 && to->getMMLDirection(parent, edge)==MMLDIR_STRAIGHT) {
        myDirs.push_back(MainDirections::DIR_FORWARD);
    }
}


NBEdge::MainDirections::~MainDirections()
{}


bool
NBEdge::MainDirections::empty() const
{
    return myDirs.empty();
}


bool
NBEdge::MainDirections::includes(Direction d) const
{
    return find(myDirs.begin(), myDirs.end(), d)!=myDirs.end();
}



/* -------------------------------------------------------------------------
 * NBEdge-methods
 * ----------------------------------------------------------------------- */
NBEdge::NBEdge(const string &id, NBNode *from, NBNode *to,
               string type, SUMOReal speed, size_t nolanes,
               int priority, LaneSpreadFunction spread,
               EdgeBasicFunction basic) throw(ProcessError) :
        myStep(INIT), myID(StringUtils::convertUmlaute(id)),
        myType(StringUtils::convertUmlaute(type)),
        myNolanes(nolanes), myFrom(from), myTo(to), myAngle(0),
        myPriority(priority), mySpeed(speed),
        myTurnDestination(0),
        myFromJunctionPriority(-1), myToJunctionPriority(-1),
        myBasicType(basic), myLaneSpreadFunction(spread),
        myAllowedOnLanes(nolanes), myNotAllowedOnLanes(nolanes),
        myLoadedLength(-1), myAmTurningWithAngle(0), myAmTurningOf(0),
        myAmInnerEdge(false)
{
	init();
}


NBEdge::NBEdge(const string &id, NBNode *from, NBNode *to,
               string type, SUMOReal speed, size_t nolanes,
               int priority,
               Position2DVector geom, LaneSpreadFunction spread,
               EdgeBasicFunction basic) throw(ProcessError) :
        myStep(INIT), myID(StringUtils::convertUmlaute(id)),
        myType(StringUtils::convertUmlaute(type)),
        myNolanes(nolanes), myFrom(from), myTo(to), myAngle(0),
        myPriority(priority), mySpeed(speed),
        myTurnDestination(0),
        myFromJunctionPriority(-1), myToJunctionPriority(-1),
        myBasicType(basic), myGeom(geom), myLaneSpreadFunction(spread),
        myAllowedOnLanes(nolanes), myNotAllowedOnLanes(nolanes),
        myLoadedLength(-1), myAmTurningWithAngle(0), myAmTurningOf(0),
        myAmInnerEdge(false)
{
	init();
}


void
NBEdge::init() throw(ProcessError)
{
    if (myNolanes==0) {
        throw ProcessError("Edge '" + myID + "' needs at least one lane.");
    }
    if (myFrom==0||myTo==0) {
        throw ProcessError("At least one of edge's '" + myID + "' nodes is not known.");
    }
    if (myFrom->getPosition().almostSame(myTo->getPosition())) {
        throw ProcessError("Edge '" + myID + "' starts at the same position it ends on.");
    }
    myGeom.push_back_noDoublePos(myTo->getPosition());
    myGeom.push_front_noDoublePos(myFrom->getPosition());
    myAngle = NBHelpers::angle(
                  myFrom->getPosition().x(), myFrom->getPosition().y(),
                  myTo->getPosition().x(), myTo->getPosition().y()
              );
    myFrom->addOutgoingEdge(this);
    myTo->addIncomingEdge(this);
    // prepare container
    myReachable.resize(myNolanes, EdgeLaneVector());
    myLength = GeomHelper::distance(
                   myFrom->getPosition(), myTo->getPosition());
    assert(myGeom.size()>=2);
	try {
		computeLaneShapes();
	} catch (InvalidArgument &ia) {
		throw ProcessError(string(ia.what()) + " in edge " + myID + ".");
	}
    for (size_t i=0; i<myNolanes; i++) {
        myLaneSpeeds.push_back(mySpeed);
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in constructor\n";
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
    return myAngle;
}


size_t
NBEdge::getNoLanes() const
{
    return myNolanes;
}


int
NBEdge::getPriority() const
{
    return myPriority;
}


int
NBEdge::getJunctionPriority(NBNode *node)
{
    if (node==myFrom) {
        return myFromJunctionPriority;
    } else {
        return myToJunctionPriority;
    }
}


void
NBEdge::setJunctionPriority(NBNode *node, int prio)
{
    if (node==myFrom) {
        myFromJunctionPriority = prio;
    } else {
        myToJunctionPriority = prio;
    }
}


const string &
NBEdge::getID() const
{
    return myID;
}


NBNode *
NBEdge::getFromNode()
{
    return myFrom;
}


NBNode *
NBEdge::getToNode()
{
    return myTo;
}


string
NBEdge::getType()
{
    return myType;
}


SUMOReal
NBEdge::getLength()
{
    return myLength;
}


const EdgeLaneVector &
NBEdge::getEdgeLanesFromLane(size_t lane) const
{
    assert(lane<myReachable.size());
    return myReachable[lane];
}


void
NBEdge::computeTurningDirections()
{
    myTurnDestination = 0;
    EdgeVector outgoing = myTo->getOutgoingEdges();
    for (EdgeVector::iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if (outedge->getBasicType()==EDGEFUNCTION_SINK||myBasicType==EDGEFUNCTION_SOURCE) {
            continue;
        }
        SUMOReal relAngle =
            NBHelpers::relAngle(getAngle(*myTo), outedge->getAngle(*myTo));
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
        DEBUG_OUT << getID() << "in getAngle\n";
        throw 1;
    }
#endif
#endif
    if (&atNode==myFrom) {
        return myGeom.getBegLine().atan2DegreeAngle();
    } else {
        assert(&atNode==myTo);
        return myGeom.getEndLine().atan2DegreeAngle();
    }
}


void
NBEdge::setTurningDestination(NBEdge *e)
{
    SUMOReal cur = fabs(NBHelpers::relAngle(getAngle(), e->getAngle()));
    SUMOReal old =
        myTurnDestination==0
        ? 0
        : fabs(NBHelpers::relAngle(getAngle(), myTurnDestination->getAngle()));
    if (cur>old
            &&
            e->acceptBeingTurning(this)) {

        myTurnDestination = e;
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
NBEdge::writeXMLStep1(OutputDevice &into)
{
    // write the edge's begin
    into << "   <edge id=\"" << myID <<
    "\" from=\"" << myFrom->getID() <<
    "\" to=\"" << myTo->getID() <<
    "\" priority=\"" << myPriority <<
    "\" type=\"" << myType <<
    "\" function=\"";
    switch (myBasicType) {
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
    if (myAmInnerEdge) {
        into << "\" inner=\"x";
    }
    into << "\">\n";
    // write the lanes
    into << "      <lanes>\n";
    for (size_t i=0; i<myNolanes; i++) {
        writeLane(into, i);
    }
    into << "      </lanes>\n";
    // write the list of connected edges
    const std::vector<NBEdge*> *tmp = getConnectedSorted();
    std::vector<NBEdge*> sortedConnected = *tmp;
    if (getTurnDestination()!=0) {
        sortedConnected.push_back(getTurnDestination());
    }
    delete tmp;
    for (std::vector<NBEdge*>::iterator l=sortedConnected.begin(); l!=sortedConnected.end(); l++) {
        LanesThatSucceedEdgeCont::iterator m = mySucceedinglanes.find(*l);
        if (m!=mySucceedinglanes.end()) {
            writeConnected(into, (*m).first, (*m).second);
        }
    }
    // close the edge
    into << "   </edge>\n\n";
}


void
NBEdge::writeXMLStep2(OutputDevice &into, bool includeInternal)
{
    for (size_t i=0; i<myNolanes; i++) {
        writeSucceeding(into, i, includeInternal);
    }
}


void
NBEdge::writeLane(OutputDevice &into, size_t lane)
{
    // output the lane's attributes
    into << "         <lane id=\"" << myID << '_' << lane << "\"";
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
        WRITE_WARNING("Lane #" + toString<size_t>(lane) + " of edge '" + myID + "' has a maximum velocity of 0.");
    } else if (myLaneSpeeds[lane]<0) {
        throw ProcessError("Negative velocity (" + toString(myLaneSpeeds[lane]) + " on edge '" + myID + "' lane#" + toString(lane) + ".");

    }
    SUMOReal length = myLength;
    if (myLoadedLength>0) {
        length = myLoadedLength;
    }
    into << " maxspeed=\"" << myLaneSpeeds[lane] << "\" length=\"" << length << "\">";
    // the lane's shape
    into << myLaneGeoms[lane];
    // close
    into << "</lane>\n";
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
NBEdge::computeLaneShapes() throw(InvalidArgument)
{
    // vissim needs this
    if (myFrom==myTo) {
        return;
    }
    // build the shape of each lane
    myLaneGeoms.clear();
    for (size_t i=0; i<myNolanes; i++) {
        myLaneGeoms.push_back(computeLaneShape(i));
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[myLaneGeoms.size()-1].assertNonEqual()) {
            DEBUG_OUT << getID() << "in computeLaneShapes\n";
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
        DEBUG_OUT << getID() << "in getLaneShape\n";
        throw 1;
    }
#endif
#endif
    return myLaneGeoms[i];
}


Position2DVector
NBEdge::computeLaneShape(size_t lane) throw(InvalidArgument)
{
    Position2DVector shape;

    for (size_t i=0; i<myGeom.size(); i++) {
        if (/*i==myGeom.size()-2||*/i==0) {
            Position2D from = myGeom[i];
            Position2D to = myGeom[i+1];
            std::pair<SUMOReal, SUMOReal> offsets =
                laneOffset(from, to, SUMO_const_laneWidthAndOffset, myNolanes-1-lane);
            shape.push_back_noDoublePos(//.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position2D(from.x()-offsets.first, from.y()-offsets.second));
        } else if (i==myGeom.size()-1) {
            Position2D from = myGeom[i-1];
            Position2D to = myGeom[i];
            std::pair<SUMOReal, SUMOReal> offsets =
                laneOffset(from, to, SUMO_const_laneWidthAndOffset, myNolanes-1-lane);
            shape.push_back_noDoublePos(//.push_back(
                // (methode umbenennen; was heisst hier "-")
                Position2D(to.x()-offsets.first, to.y()-offsets.second));
        } else {
            Position2D from = myGeom[i-1];
            Position2D me = myGeom[i];
            Position2D to = myGeom[i+1];
            std::pair<SUMOReal, SUMOReal> offsets =
                laneOffset(from, me, SUMO_const_laneWidthAndOffset, myNolanes-1-lane);
            std::pair<SUMOReal, SUMOReal> offsets2 =
                laneOffset(me, to, SUMO_const_laneWidthAndOffset, myNolanes-1-lane);
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
        DEBUG_OUT << getID() << "computeLaneShape\n";
    }
#endif
#endif
    return shape;
}


std::pair<SUMOReal, SUMOReal>
NBEdge::laneOffset(const Position2D &from, const Position2D &to,
                   SUMOReal lanewidth, size_t lane) throw(InvalidArgument)
{
    SUMOReal x1 = from.x();
    SUMOReal y1 = from.y();
    SUMOReal x2 = to.x();
    SUMOReal y2 = to.y();
    SUMOReal length = sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
    std::pair<SUMOReal, SUMOReal> offsets =
        GeomHelper::getNormal90D_CW(x1, y1, x2, y2, length, lanewidth);
    SUMOReal xoff = offsets.first / (SUMOReal) 2.0;
    SUMOReal yoff = offsets.second / (SUMOReal) 2.0;
    if (myLaneSpreadFunction==LANESPREAD_RIGHT) {
        xoff += (offsets.first * (SUMOReal) lane);
        yoff += (offsets.second * (SUMOReal) lane);
    } else {
        xoff += (offsets.first * (SUMOReal) lane) - (offsets.first * (SUMOReal) myNolanes / (SUMOReal) 2.0);
        yoff += (offsets.second * (SUMOReal) lane) - (offsets.second * (SUMOReal) myNolanes / (SUMOReal) 2.0);
    }
    return std::pair<SUMOReal, SUMOReal>(xoff, yoff);
}


void
NBEdge::writeConnected(OutputDevice &into, NBEdge *edge, LaneVector &lanes)
{
    if (edge==0) {
        return;
    }
    into << "      <cedge id=\"" << edge->getID() << "\">";
    size_t noApproachers = lanes.size();
    for (size_t i=0; i<noApproachers; i++) {
        assert(i<lanes.size());
        into << myID << '_' << lanes[i];
        if (i<noApproachers-1) {
            into << ' ';
        }
    }
    into << "</cedge>\n";
}


void
NBEdge::writeSucceeding(OutputDevice &into, size_t lane,
                        bool includeInternal)
{
    into << "   <succ edge=\"" << myID << "\" lane=\"" << myID << "_"
    << lane << "\" junction=\"" << myTo->getID() << "\">\n";
    // the lane may be unconnented; output information about being invalid
    assert(lane<myReachable.size());
    size_t noApproached = myReachable[lane].size();
    if (noApproached==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\"/>\n";
    }
    // output list of connected lanes
    // go through each connected edge
    for (size_t j=0; j<noApproached; j++) {
        writeSingleSucceeding(into, lane, j, includeInternal);
    }
    into << "   </succ>\n\n";
}


void
NBEdge::writeSingleSucceeding(OutputDevice &into, size_t fromlane, size_t destidx,
                              bool includeInternal)
{
    // check whether the connected lane is invalid
    //  (should not happen; this is an artefact left from previous versions)
    assert(fromlane<myReachable.size());
    assert(destidx<myReachable[fromlane].size());
    if (myReachable[fromlane][destidx].edge==0) {
        into << "      <succlane lane=\"SUMO_NO_DESTINATION\" yield=\"1\" "
        << "dir=\"s\" state=\"O\"/>\n"; // !!! check dummy values
        return;
    }
    // write the id
    into << "      <succlane lane=\""
    << myReachable[fromlane][destidx].edge->getID() << '_'
    << myReachable[fromlane][destidx].lane << '\"'; // !!! classe LaneEdge mit getLaneID
    if (includeInternal) {
        into << " via=\""
        << myTo->getInternalLaneID(this, fromlane, myReachable[fromlane][destidx].edge, myReachable[fromlane][destidx].lane)
        << "_0\"";
    }
    // set information about the controlling tl if any
    if (myReachable[fromlane][destidx].tlID!="") {
        into << " tl=\"" << myReachable[fromlane][destidx].tlID << "\"";
        into << " linkno=\"" << myReachable[fromlane][destidx].tlLinkNo << "\"";
    }
    // write information whether the connection yields
    if (myAmInnerEdge||!myTo->mustBrake(this,
                                        myReachable[fromlane][destidx].edge,
                                        myReachable[fromlane][destidx].lane)) {
        into << " yield=\"0\"";
    } else {
        into << " yield=\"1\"";
    }
    // write the direction information
    NBMMLDirection dir = myTo->getMMLDirection(this, myReachable[fromlane][destidx].edge);
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
    if (myReachable[fromlane][destidx].tlID!="") {
        into << "state=\"t";
    } else {
        into << "state=\""
        << myTo->stateCode(this, myReachable[fromlane][destidx].edge, myReachable[fromlane][destidx].lane);
    }
    if (myTo->getCrossingPosition(this, fromlane, myReachable[fromlane][destidx].edge, myReachable[fromlane][destidx].lane).first>=0) {
        into << "\" int_end=\"x";
    }

    // close
    into << "\"/>\n";
}


bool
NBEdge::hasRestrictions() const
{
    for (size_t i=0; i<myNolanes; ++i) {
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
NBEdge::writeLanesPlain(OutputDevice &into)
{
    for (size_t lane=0; lane<myNolanes; ++lane) {
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
        into << "/>\n";
    }
}


bool
NBEdge::addEdge2EdgeConnection(NBEdge *dest)
{
    if (myStep==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (myTo!=dest->myFrom) {
        return false;
    }
    if (find(myConnectedEdges.begin(), myConnectedEdges.end(), dest)==myConnectedEdges.end()) {
        myConnectedEdges.push_back(dest);
    }
    if (myStep<EDGE2EDGES) {
        myStep = EDGE2EDGES;
    }
    // !!! recheck the next - something too much is deleted in invalidateConnections
    if (myToEdges.find(dest)==myToEdges.end()) {
        myToEdges[dest] = vector<size_t>();
    }
    return true;
}


bool
NBEdge::addLane2LaneConnection(size_t from, NBEdge *dest,
                               size_t toLane, bool markAs2Recheck,
                               bool mayUseSameDestination)
{
    if (myStep==INIT_REJECT_CONNECTIONS) {
        return true;
    }
    // check whether the node was merged and now a connection between
    //  not matching edges is tried to be added
    //  This happens f.e. within the ptv VISSIM-example "Beijing"
    if (myTo!=dest->myFrom) {
        return false;
    }
    bool ok = addEdge2EdgeConnection(dest);
    if (ok) {
        setConnection(from, dest, toLane, markAs2Recheck, mayUseSameDestination);
        vector<size_t> &lanes = (myToEdges.find(dest))->second;
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
    if (myStep>=EDGE2EDGES) {
        return true;
    }
    if (myConnectedEdges.size()==0) {
        myConnectedEdges = myTo->getOutgoingEdges();
    }
    myStep = EDGE2EDGES;
    return true;
}


bool
NBEdge::computeLanes2Edges()
{
    // return if this relationship has been build in previous steps or
    //  during the import
    if (myStep>=LANES2EDGES) {
        return true;
    }
    assert(myStep==EDGE2EDGES);
    // get list of possible outgoing edges sorted by direction clockwise
    //  the edge in the backward direction (turnaround) is not in the list
    const vector<NBEdge*> *edges = getConnectedSorted();
    // divide the lanes on reachable edges
    divideOnEdges(edges);
    delete edges;
    myStep = LANES2EDGES;
    return true;
}


bool
NBEdge::recheckLanes()
{
    if (myStep!=LANES2LANES) {
        // check #1:
        // If there is a lane with no connections and any neighbour lane has
        //  more than one connections, try to move one of them.
        // This check is only done for edges which connections were assigned
        //  using the standard algorithm.
        for (size_t i=0; i<myNolanes; i++) {
            assert(i<myReachable.size());
            if (myReachable[i].size()==0) {
                if (i>0&&myReachable[i-1].size()>1) {
                    moveConnectionToLeft(i-1);
                } else if (i<myReachable.size()-1&&myReachable[i+1].size()>1) {
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
    assert(myNolanes==myReachable.size());
    for (size_t i=0; i<myNolanes; i++) {
        if (myReachable[i].size()==0) {
            setConnection(i, 0, 0, false);
        }
    }
    return true;
}


void
NBEdge::moveConnectionToLeft(size_t lane)
{
    assert(lane<myReachable.size() && myReachable[lane].size()>0);
    pair<NBEdge*, size_t> dest = getConnectionRemoving(lane, myReachable[lane].size()-1);
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
    assert(myReachable.size()>srcLane);
    assert(myReachable[srcLane].size()>pos);
    // get the destination edge and remove from the container
    EdgeLane edgelane = myReachable[srcLane][pos];
    myReachable[srcLane].erase(myReachable[srcLane].begin()+pos);
    // remove the information from the map of how to reach edges
    NBEdge::LanesThatSucceedEdgeCont::iterator i = mySucceedinglanes.find(edgelane.edge);
    LaneVector lanes = (*i).second;
    LaneVector::iterator j = find(lanes.begin(), lanes.end(), srcLane);
    lanes.erase(j);
    mySucceedinglanes[edgelane.edge] = lanes;
    // return the information
    return pair<NBEdge*, size_t>(edgelane.edge, edgelane.lane);
}


vector<size_t>
NBEdge::getConnectionLanes(NBEdge *currentOutgoing)
{
    if (currentOutgoing==myTurnDestination) {
        return vector<size_t>();
    }
    map<NBEdge*, vector<size_t> >::iterator i=myToEdges.find(currentOutgoing);
    assert(i==myToEdges.end()||(*i).second.size()<=myNolanes);
    if (i==myToEdges.end()) {
        return vector<size_t>();
    }
    return (*i).second;
}


const std::vector<NBEdge*> *
NBEdge::getConnectedSorted()
{
    // check whether connections exist and if not, use edges from the node
    EdgeVector outgoing = myConnectedEdges;
    if (outgoing.size()==0) {
        outgoing = myTo->getOutgoingEdges();
    }
    // allocate the sorted container
    size_t size = outgoing.size();
    vector<NBEdge*> *edges = new vector<NBEdge*>();
    edges->reserve(size);
    for (EdgeVector::const_iterator i=outgoing.begin(); i!=outgoing.end(); i++) {
        NBEdge *outedge = *i;
        if (outedge!=myTurnDestination) {// !!!
            edges->push_back(outedge);
        }
    }
    sort(edges->begin(), edges->end(),
         NBContHelper::relative_edge_sorter(this, myTo));
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
            (SUMOReal) myNolanes / (SUMOReal) prioSum;
        // do not let this number be greater than the number of available lanes
        if (res>myNolanes) {
            res = (SUMOReal) myNolanes;
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
    sumResulting += minResulting / (SUMOReal) 2.;
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
    ToEdgeConnectionsAdder adder(&myToEdges, transition);
    Bresenham::compute(&adder, (SUMOReal) myNolanes, (SUMOReal) noVirtual);
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
        int prio = (*i)->getJunctionPriority(myTo);
        assert((prio+1)*2>0);
        prio = (prio+1) * 2;
        priorities->push_back(prio);
    }
    // when the right turning direction has not a higher priority, divide
    //  the importance by 2 due to the possibility to leave the junction
    //  faster from this lane
    MainDirections mainDirections(*outgoing, this, myTo);
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
    if (mainDirections.includes(MainDirections::DIR_FORWARD)&&myNolanes>2) {
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
    if (myTurnDestination!=0) {
        setConnection(myNolanes-1, myTurnDestination, myTurnDestination->getNoLanes()-1, false);
    }
}


void
NBEdge::sortOutgoingLanesConnections()
{
    for (size_t i=0; i<myReachable.size(); i++) {
        sort(myReachable[i].begin(), myReachable[i].end(),
             NBContHelper::relative_edgelane_sorter(this, myTo));
    }
}



void
NBEdge::setConnection(size_t src_lane, NBEdge *dest_edge,
                      size_t dest_lane, bool markAs2Recheck,
                      bool mayUseSameDestination)
{
    if (myStep==INIT_REJECT_CONNECTIONS) {
        return;
    }
    assert(dest_lane<=10);
    assert(src_lane<=10);
    // some kind of a misbehaviour which may occure when the junction's outgoing
    //  edge priorities were not properly computed, what may happen due to
    //  an incomplete or not proper input
    // what happens is that under some circumstances a single lane may set to
    //  be approached more than once by the one of our lanes.
    //  This must not be!
    // we test whether it is the case and do nothing if so - the connection
    //  will be refused
    //
    EdgeLane el;
    el.edge = dest_edge;
    el.lane = dest_lane;
    if (!mayUseSameDestination) {
        for (size_t j=0; dest_edge!=0&&j<myReachable.size(); j++) {
            // for any other lane: check whether a connection to the same
            //  lane as the one to be added exists
            EdgeLaneVector &tmp = myReachable[j];
            if (find(tmp.begin(), tmp.end(), el)!=tmp.end()) {
                return;
            }
        }
    }
    EdgeLaneVector &tmp = myReachable[src_lane];
    if (find(tmp.begin(), tmp.end(), el)!=tmp.end()) {
        return;
    }

    // this connection has not yet been set
    bool known = false;
    // find the entry which holds the information which lanes may
    //  be used to reach the current destination edge
    LanesThatSucceedEdgeCont::iterator i = mySucceedinglanes.find(dest_edge);
    // if there is no such connection yet, build it
    if (i==mySucceedinglanes.end()) {
        LaneVector lanes;
        lanes.push_back(src_lane);
        mySucceedinglanes[dest_edge] = lanes;
        // else
    } else {
        LaneVector lanes = (*i).second;
        // check whether one of the known connections to the current destination
        //  comes from the same lane as the current to add
        LaneVector::iterator i = find(lanes.begin(), lanes.end(), src_lane);
        // if not, append
        if (i==lanes.end())
            lanes.push_back(src_lane);
        mySucceedinglanes[dest_edge] = lanes;
    }
    // append current connection only if no equal is already known
    if (!known) {
        if (myReachable.size()<=src_lane) {
            MsgHandler::getErrorInstance()->inform("Could not set connection from '" + myID + "_" + toString(src_lane) + "' to '" + dest_edge->getID() + "_" + toString(dest_lane) + "'.");
            return;
        }
        assert(myReachable.size()>src_lane);
        myReachable[src_lane].push_back(el);
    }
    // check whether we have to take another look at it later
    if (markAs2Recheck) {
        // yes, the connection was set using an algorithm which requires a recheck
        myStep = LANES2LANES_RECHECK;
    } else {
        // ok, let's only not recheck it if we did no add something that has to be recheked
        if (myStep!=LANES2LANES_RECHECK) {
            myStep = LANES2LANES;
        }
    }
}


bool
NBEdge::isTurningDirectionAt(const NBNode *n, NBEdge *edge) const
{
    // maybe it was already set as the turning direction
    if (edge == myTurnDestination) {
        return true;
    } else if (myTurnDestination!=0) {
        // otherwise - it's not if a turning direction exists
        return false;
    }
    // if the same nodes are connected
    if (myFrom==edge->myTo &&myTo==edge->myFrom) {
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
    if (myTo==edge->myTo && myFrom==edge->myFrom) {
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
        return myFrom;
    }
    // return the to-node when the position is at the end of the edge
    if (pos>myLength-tolerance) {
        return myTo;
    }
    return 0;
}


NBEdge::EdgeBasicFunction
NBEdge::getBasicType() const
{
    return myBasicType;
}


SUMOReal
NBEdge::getSpeed() const
{
    return mySpeed;
}


void
NBEdge::replaceInConnections(NBEdge *which, NBEdge *by, size_t laneOff)
{
    // replace in "_connectedEdges"
    EdgeVector::iterator k = find(
                                 myConnectedEdges.begin(), myConnectedEdges.end(), by);
    if (k!=myConnectedEdges.end()) {
        for (k=myConnectedEdges.begin(); k!=myConnectedEdges.end();) {
            if ((*k)==which) {
                k = myConnectedEdges.erase(k);
            } else {
                k++;
            }
        }
    } else {
        for (size_t i=0; i<myConnectedEdges.size(); i++) {
            if (myConnectedEdges[i]==which) {
                myConnectedEdges[i] = by;
            }
        }
    }
    // check whether it was the turn destination
    if (myTurnDestination==which) {
        myTurnDestination = by;
    }
    // replace in myToEdges
    {
        // check if the edge to replace by was already connected
        bool have = myToEdges.find(by)!=myToEdges.end();
        // find the edge to replace
        std::map<NBEdge*, std::vector<size_t> >::iterator j = myToEdges.find(which);
        if (j!=myToEdges.end()) {
            // if the edge to replace by already had a connection
            if (have) {
                // add further connections
                for (std::vector<size_t>::iterator k=myToEdges[which].begin(); k!=myToEdges[which].end(); k++) {
                    if (find(myToEdges[which].begin(), myToEdges[which].end(), (*k)+laneOff)!=myToEdges[which].end()) {
                        myToEdges[by].push_back((*k)+laneOff);
                    }
                }
            } else {
                // set connections
                have = true;
                myToEdges[by] = (*j).second;
            }
        }
    }
    // replace in myReachable
    {
        for (ReachableFromLaneVector::iterator k=myReachable.begin(); k!=myReachable.end(); k++) {
            for (EdgeLaneVector::iterator l=(*k).begin(); l!=(*k).end(); l++) {
                if ((*l).edge==which) {
                    (*l).edge = by;
                    (*l).lane = (*l).lane + laneOff;
                }
            }
        }
    }
    // replace in mySucceedinglanes
    {
        LanesThatSucceedEdgeCont::iterator l=mySucceedinglanes.find(which);
        if (l!=mySucceedinglanes.end()) {
            LanesThatSucceedEdgeCont::iterator l2=mySucceedinglanes.find(by);
            if (l2!=mySucceedinglanes.end()) {
                copy(mySucceedinglanes[which].begin(), mySucceedinglanes[which].end(),
                     back_inserter(mySucceedinglanes[by]));
            } else {
                mySucceedinglanes[by] = (*l).second;
            }
            mySucceedinglanes.erase(l);
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
        find(myConnectedEdges.begin(), myConnectedEdges.end(), e)
        !=
        myConnectedEdges.end();

}


void
NBEdge::remapConnections(const EdgeVector &incoming)
{
    for (EdgeVector::const_iterator i=incoming.begin(); i!=incoming.end(); i++) {
        NBEdge *inc = *i;
        // We have to do this
        inc->myStep = EDGE2EDGES;
        // add all connections
        for (EdgeVector::iterator j=myConnectedEdges.begin(); j!=myConnectedEdges.end(); j++) {
            inc->addEdge2EdgeConnection(*j);
        }
        inc->removeFromConnections(this);
    }
}


void
NBEdge::removeFromConnections(NBEdge *which, int lane)
{
    // remove from "myConnectedEdges"
    if (lane<0) {
        for (int i=0; i<(int) myConnectedEdges.size(); i++) {
            if (myConnectedEdges[i]==which) {
                myConnectedEdges.erase(myConnectedEdges.begin()+i);
                i--;
            }
        }
    }
    // check whether it was the turn destination
    if (myTurnDestination==which&&lane<0) {
        myTurnDestination = 0;
    }
    // remove in myToEdges
    {
        std::map<NBEdge*, std::vector<size_t> >::iterator j = myToEdges.find(which);
        if (j!=myToEdges.end()) {
            if (lane<0) {
                myToEdges.erase(which);
            } else {
                std::vector<size_t> &lanes = (*j).second;
                if (find(lanes.begin(), lanes.end(), lane)!=lanes.end()) {
                    lanes.erase(find(lanes.begin(), lanes.end(), lane));
                }
            }
        }
    }
    // remove in myReachable
    {
        if (lane<0) {
            for (ReachableFromLaneVector::iterator k=myReachable.begin(); k!=myReachable.end(); k++) {
                EdgeLaneVector::iterator l=(*k).begin();
                while (l!=(*k).end()) {
                    if ((*l).edge==which/* && ((*l).lane==lane||lane<0) */) {
                        (*k).erase(l);
                        l = (*k).begin();
                    } else {
                        l++;
                    }
                }
            }
        }
    }
    // remove in mySucceedinglanes
    {
        LanesThatSucceedEdgeCont::iterator l=mySucceedinglanes.find(which);
        if (l!=mySucceedinglanes.end()) {
            if (lane<0) {
                mySucceedinglanes.erase(l);
            } else {
                LaneVector &lanes = (*l).second;
                if (find(lanes.begin(), lanes.end(), lane)!=lanes.end()) {
                    lanes.erase(find(lanes.begin(), lanes.end(), lane));
                }
            }
        }
    }
}


void
NBEdge::invalidateConnections(bool reallowSetting)
{
    myTurnDestination = 0;
    myToEdges.clear();
    myReachable.clear();
    myReachable.resize(myNolanes, EdgeLaneVector());
    mySucceedinglanes.clear();
    if (reallowSetting) {
        myStep = INIT;
    } else {
        myStep = INIT_REJECT_CONNECTIONS;
    }
}


const EdgeVector &
NBEdge::getConnected() const
{
    return myConnectedEdges;
}



bool
NBEdge::lanesWereAssigned() const
{
    return myStep==LANES2LANES;
}


EdgeVector
NBEdge::getEdgesFromLane(size_t lane) const
{
    assert(lane<myReachable.size());
    EdgeVector ret;
    for (EdgeLaneVector::const_iterator i=myReachable[lane].begin(); i!=myReachable[lane].end(); i++) {
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
        DEBUG_OUT << getID() << "in getGeometry\n";
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
        DEBUG_OUT << getID() << "in setGeometry\n";
        throw 1;
    }
#endif
#endif
    computeLaneShapes();
}


SUMOReal
NBEdge::getMaxLaneOffset()
{
    return (SUMOReal) SUMO_const_laneWidthAndOffset * myNolanes;
}


Position2D
NBEdge::getMinLaneOffsetPositionAt(NBNode *node, SUMOReal width)
{
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myLaneGeoms[0].assertNonEqual()) {
        DEBUG_OUT << getID() << "in minlaneoffset\n";
        throw 1;
    }
#endif
#endif
    width = width < myLaneGeoms[0].length()/(SUMOReal) 2.0
            ? width
            : myLaneGeoms[0].length()/(SUMOReal) 2.0;
    if (node==myFrom) {
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
        DEBUG_OUT << getID() << "in getmaxlane\n";
        throw 1;
    }
#endif
#endif
    width = width < myLaneGeoms[0].length()/(SUMOReal) 2.0
            ? width
            : myLaneGeoms[0].length()/(SUMOReal) 2.0;
    if (node==myFrom) {
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


bool
NBEdge::setControllingTLInformation(int fromLane, NBEdge *toEdge, int toLane,
                                    const std::string &tlID, size_t tlPos)
{
    // check whether the connection was not set as not to be controled previously
    {
        TLSDisabledConnection tpl;
        tpl.fromLane = fromLane;
        tpl.to = toEdge;
        tpl.toLane = toLane;
        std::vector<TLSDisabledConnection>::iterator i;
        i = find_if(myTLSDisabledConnections.begin(), myTLSDisabledConnections.end(),
                    tls_disable_finder(tpl));
        if (i!=myTLSDisabledConnections.end()) {
            return false;
        }
    }

    assert(fromLane<0||fromLane<(int) myNolanes);
    // try to use information about the connections if given
    if (fromLane>=0&&toLane>=0) {
        // get the connections outgoing from this lane
        EdgeLaneVector &connections = myReachable[fromLane];
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
            return true;
        }
    }
    // if the original connection was not found, set the information for all
    //  connections
    size_t no = 0;
    bool hadError = false;
    for (size_t j=0; j<myNolanes; j++) {
        EdgeLaneVector &connections = myReachable[j];
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
    return true;
}


void
NBEdge::disableConnection4TLS(int fromLane, NBEdge *toEdge, int toLane)
{
    TLSDisabledConnection c;
    c.fromLane = fromLane;
    c.to = toEdge;
    c.toLane = toLane;
    myTLSDisabledConnections.push_back(c);
}


void
NBEdge::normalisePosition()
{
    myGeom.resetBy(GeoConvHelper::getOffset());
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!myGeom.assertNonEqual()) {
        DEBUG_OUT << getID() << "in normalise2\n";
        throw 1;
    }
#endif
#endif
    for (size_t i=0; i<myNolanes; i++) {
        myLaneGeoms[i].resetBy(GeoConvHelper::getOffset());

#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[i].assertNonEqual()) {
            DEBUG_OUT << getID() << "in normalise\n";
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
        DEBUG_OUT << getID() << "in reshift\n";
        throw 1;
    }
#endif
#endif
    for (size_t i=0; i<myNolanes; i++) {
        myLaneGeoms[i].reshiftRotate(xoff, yoff, rot);
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[i].assertNonEqual()) {
            DEBUG_OUT << getID() << "in reshift2\n";
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
    for (size_t bla=0; bla<myNolanes; bla++) {
        if (!myLaneGeoms[bla].assertNonEqual()) {
            DEBUG_OUT << getID() << endl;
            DEBUG_OUT << myLaneGeoms[bla] << endl;
            throw 1;
        }
    }
#endif
#endif
    Position2DVector ret;
    if (myFrom==(&n)) {
        // outgoing
        ret = myLaneGeoms[0];
    } else {
        // incoming
        ret = myLaneGeoms[getNoLanes()-1].reverse();
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!ret.assertNonEqual()) {
        DEBUG_OUT << getID() << "in cwBoundary\n";
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
    if (myFrom==(&n)) {
        // outgoing
        ret = myLaneGeoms[getNoLanes()-1];
    } else {
        // incoming
        ret = myLaneGeoms[0].reverse();
    }
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
    if (!ret.assertNonEqual()) {
        DEBUG_OUT << getID() << "in ccwboundary\n";
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
    return (SUMOReal) myNolanes * SUMO_const_laneWidth + (SUMOReal)(myNolanes-1) * SUMO_const_laneOffset;
}


bool
NBEdge::expandableBy(NBEdge *possContinuation) const
{
    // ok, the number of lanes must match
    if (myNolanes!=possContinuation->myNolanes) {
        return false;
    }
    // the priority, too (?)
    if (getPriority()!=possContinuation->getPriority()) {
        return false;
    }
    // the speed allowed
    if (mySpeed!=possContinuation->mySpeed) {
        return false;
    }
    // the vehicle class constraints, too
    if (myAllowedOnLanes!=possContinuation->myAllowedOnLanes
            ||
            myNotAllowedOnLanes!=possContinuation->myNotAllowedOnLanes) {
        return false;
    }
    // the next is quite too conservative here, but seems to work
    if (myBasicType!=EDGEFUNCTION_NORMAL
            &&
            possContinuation->myBasicType!=EDGEFUNCTION_NORMAL) {

        return false;
    }
    // also, check whether the connections - if any exit do allow to join
    //  both edges
    // This edge must have a one-to-one connection to the following lanes
    switch (myStep) {
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
        if (myToEdges.find(possContinuation)==myToEdges.end()) {
            return false;
        }
        // all lanes must go to the possible continuation
        const std::vector<size_t> &lanes =
            (*(myToEdges.find(possContinuation))).second;
        if (lanes.size()!=myNolanes) {
            return false;
        }
    }
    break;
    case LANES2LANES: {
        for (size_t i=0; i<myNolanes; i++) {
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
        DEBUG_OUT << getID() << "in append\n";
        throw 1;
    }
#endif
#endif
    for (size_t i=0; i<myNolanes; i++) {
        myLaneGeoms[i].appendWithCrossingPoint(e->myLaneGeoms[i]);
#ifdef _DEBUG
#ifdef CHECK_UNIQUE_POINTS_GEOMETRY
        if (!myLaneGeoms[i].assertNonEqual()) {
            DEBUG_OUT << getID() << "in append2\n";
            throw 1;
        }
#endif
#endif
    }
    // recompute length
    myLength += e->myLength;
    // copy the connections and the building step if given
    myStep = e->myStep;
    myConnectedEdges = e->myConnectedEdges;
    myReachable = (e->myReachable);
    mySucceedinglanes = e->mySucceedinglanes;
    myToEdges = e->myToEdges;
    myTurnDestination = e->myTurnDestination;
    // set the node
    myTo = e->myTo;
}


void
NBEdge::computeEdgeShape()
{
    size_t i;
    for (i=0; i<myNolanes; i++) {
        // get lane begin and end
        Line2D lb = Line2D(myLaneGeoms[i][0], myLaneGeoms[i][1]);
        Line2D le = Line2D(myLaneGeoms[i][-1], myLaneGeoms[i][-2]);
        lb.extrapolateBy(100.0);
        le.extrapolateBy(100.0);
        //
        Position2DVector old = myLaneGeoms[i];
        Position2D nb, ne;
        // lane begin
        if (myFrom->getShape().intersects(myLaneGeoms[i])) {
            // get the intersection position with the junction
            DoubleVector pbv = myLaneGeoms[i].intersectsAtLengths(myFrom->getShape());
            if (pbv.size()>0) {
                SUMOReal pb = VectorHelper<SUMOReal>::maxValue(pbv);
                if (pb>=0&&pb<=myLaneGeoms[i].length()) {
                    myLaneGeoms[i] = myLaneGeoms[i].getSubpart(pb, myLaneGeoms[i].length());
                }
            }
        } else if (myFrom->getShape().intersects(lb.p1(), lb.p2())) {
            DoubleVector pbv = lb.intersectsAtLengths(myFrom->getShape());
            if (pbv.size()>0) {
                SUMOReal pb = VectorHelper<SUMOReal>::maxValue(pbv);
                if (pb>=0) {
                    myLaneGeoms[i].eraseAt(0);
                    myLaneGeoms[i].push_front_noDoublePos(lb.getPositionAtDistance(pb));
                }
            }
        }
        // lane end
        if (myTo->getShape().intersects(myLaneGeoms[i])) {
            // get the intersection position with the junction
            DoubleVector pev = myLaneGeoms[i].intersectsAtLengths(myTo->getShape());
            if (pev.size()>0) {
                SUMOReal pe = VectorHelper<SUMOReal>::minValue(pev);
                if (pe>=0&&pe<=myLaneGeoms[i].length()) {
                    myLaneGeoms[i] = myLaneGeoms[i].getSubpart(0, pe);
                }
            }
        } else if (myTo->getShape().intersects(le.p1(), le.p2())) {
            DoubleVector pev = le.intersectsAtLengths(myTo->getShape());
            if (pev.size()>0) {
                SUMOReal pe = VectorHelper<SUMOReal>::maxValue(pev);
                if (pe>=0) {
                    myLaneGeoms[i].eraseAt(myLaneGeoms[i].size()-1);
                    myLaneGeoms[i].push_back_noDoublePos(le.getPositionAtDistance(pe));
                }
            }
        }
        if (((int) myLaneGeoms[i].length())==0) {
            myLaneGeoms[i] = old;
        } else {
            Line2D lc(myLaneGeoms[i][0], myLaneGeoms[i][-1]);
            Line2D lo(old[0], old[-1]);
            if (135<MIN2(GeomHelper::getCCWAngleDiff(lc.atan2DegreeAngle(), lo.atan2DegreeAngle()), GeomHelper::getCWAngleDiff(lc.atan2DegreeAngle(), lo.atan2DegreeAngle()))) {
                myLaneGeoms[i] = myLaneGeoms[i].reverse();
            }
        }
    }
    // recompute edge's length
    if (myBasicType==EDGEFUNCTION_SOURCE||myBasicType==EDGEFUNCTION_SINK) {
        return;
    }
    SUMOReal length = 0;
    for (i=0; i<myNolanes; i++) {
        assert(myLaneGeoms[i].length()>0);
        length += myLaneGeoms[i].length();
    }
    myLength = length / (SUMOReal) myNolanes;
}


bool
NBEdge::hasSignalisedConnectionTo(NBEdge *e) const
{
    for (size_t i=0; i<myNolanes; i++) {
        // get the connections outgoing from this lane
        const EdgeLaneVector &connections = myReachable[i];
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
    return myTurnDestination;
}


std::string
NBEdge::getLaneID(size_t lane)
{
    assert(lane<myNolanes);
    return myID + "_" + toString<size_t>(lane);
}


bool
NBEdge::isNearEnough2BeJoined2(NBEdge *e)
{
    DoubleVector distances = myGeom.distances(e->getGeometry());
    if (distances.size()==0) {
        distances = e->getGeometry().distancesExt(myGeom);
    }
    SUMOReal max = VectorHelper<SUMOReal>::maxValue(distances);
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
    SUMOReal angle = myAngle;
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
    myNolanes += by;
    myReachable.clear();
    myReachable.resize(myNolanes, EdgeLaneVector());
    while (myLaneSpeeds.size()<myNolanes) {
        myLaneSpeeds.push_back(mySpeed);
        myAllowedOnLanes.push_back(std::vector<SUMOVehicleClass>());
        myNotAllowedOnLanes.push_back(std::vector<SUMOVehicleClass>());
    }
    computeLaneShapes();
    const EdgeVector &incs = myFrom->getIncomingEdges();
    for (EdgeVector::const_iterator i=incs.begin(); i!=incs.end(); ++i) {
        (*i)->invalidateConnections(true);
    }
    invalidateConnections(true);
}


void
NBEdge::decLaneNo(int by, int dir)
{
    myNolanes -= by;
    if (dir==0) {
        myReachable.clear();
        myReachable.resize(myNolanes, EdgeLaneVector());
    } else {
        if (dir<0) {
            myReachable.erase(myReachable.begin());
        } else {
            myReachable.pop_back();
        }
    }
    while (myLaneSpeeds.size()>myNolanes) {
        myLaneSpeeds.pop_back();
        myAllowedOnLanes.pop_back();
        myNotAllowedOnLanes.pop_back();
    }
    computeLaneShapes();
    const EdgeVector &incs = myFrom->getIncomingEdges();
    for (EdgeVector::const_iterator i=incs.begin(); i!=incs.end(); ++i) {
        (*i)->invalidateConnections(true);
    }
    if (dir==0) {
        invalidateConnections(true);
    } else {
        const EdgeVector &outs = myTo->getOutgoingEdges();
        assert(outs.size()==1);
        NBEdge *out = outs[0];
        if (dir<0) {
            removeFromConnections(out, 0);
        } else {
            removeFromConnections(out, myNolanes);
        }
    }
}


void
NBEdge::copyConnectionsFrom(NBEdge *src)
{
    myStep = src->myStep;
    myConnectedEdges = src->myConnectedEdges;
    myToEdges = src->myToEdges;
    for (size_t i=0; i<src->myReachable.size(); i++) {
        myReachable[i] = src->myReachable[i];
    }
    mySucceedinglanes = src->mySucceedinglanes;
    assert(myReachable.size()==myNolanes);
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
    assert(myTo->getOutgoingEdges().size()==0);
    myStep = LANES2LANES;
}


bool
NBEdge::splitGeometry(NBEdgeCont &ec, NBNodeCont &nc)
{
    // check whether there any splits to perform
    if (myGeom.size()<3) {
        return false;
    }
    // ok, split
    NBNode *newFrom = myFrom;
    NBNode *myLastNode = myTo;
    NBNode *newTo = 0;
    NBEdge *currentEdge = this;
    for (size_t i=1; i<myGeom.size()-1; i++) {
        // build the node first
        if (i!=myGeom.size()-2) {
            string nodename = myID + "_in_between#" + toString(i);
            if (!nc.insert(nodename, myGeom[i])) {
                throw ProcessError("Error on adding in-between node '" + nodename + "'.");

            }
            newTo = nc.retrieve(nodename);
        } else {
            newTo = myLastNode;
        }
        if (i==1) {
            currentEdge->myTo->removeIncoming(this);
            currentEdge->myTo = newTo;
            newTo->addIncomingEdge(currentEdge);
        } else {
            string edgename = myID + "[" + toString(i-1) + "]";
            currentEdge = new NBEdge(edgename, newFrom, newTo, myType, mySpeed, myNolanes,
                                     myPriority, myLaneSpreadFunction, myBasicType);
            if (!ec.insert(currentEdge)) {
                throw ProcessError("Error on adding splitted edge '" + edgename + "'.");

            }
        }
        newFrom = newTo;
    }
    //currentEdge->copyConnectionsFrom(this);
    myGeom.clear();
    myGeom.push_back(myFrom->getPosition());
    myGeom.push_back(myTo->getPosition());
    //invalidateConnections();
    myStep = INIT;
    return true;
}


void
NBEdge::allowVehicleClass(int lane, SUMOVehicleClass vclass)
{
    if (lane<0) {
        // if all lanes are meant...
        for (size_t i=0; i<myNolanes; i++) {
            // ... do it for each lane
            allowVehicleClass((int) i, vclass);
        }
        return;
    }
    assert(lane<(int) myNolanes);
    assert(lane<(int) myAllowedOnLanes.size());
    // add it only if not already done
    if (find(myAllowedOnLanes[lane].begin(), myAllowedOnLanes[lane].end(), vclass)==myAllowedOnLanes[lane].end()) {
        myAllowedOnLanes[lane].push_back(vclass);
    }
}


void
NBEdge::disallowVehicleClass(int lane, SUMOVehicleClass vclass)
{
    if (OptionsCont::getOptions().getBool("dismiss-vclasses")) {
        return;
    }
    if (lane<0) {
        // if all lanes are meant...
        for (size_t i=0; i<myNolanes; i++) {
            // ... do it for each lane
            disallowVehicleClass((int) i, vclass);
        }
        return;
    }
    assert(lane<(int) myNolanes);
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
    if (myStep==LANES2LANES) {
        return;
    }
    // go through the lanes
    for (size_t i=0; i<myNolanes; i++) {
        // get the connections from the current lane
        EdgeLaneVector reachableFromLane = myReachable[i];
        // go through these connections
        for (size_t j=0; j<reachableFromLane.size(); ++j) {
            // get the current connection
            const EdgeLane &el = reachableFromLane[j];
            // get all connections that approach the current destination edge
            const std::vector<size_t> &other = myToEdges[el.edge];
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
            if (i+1<myNolanes && el.lane+1<el.edge->getNoLanes() && find(other.begin(), other.end(), i+1)==other.end()) {
                mayAddLeft = true;
            }
            if (i+1==myNolanes && el.lane+1<el.edge->getNoLanes()) {
                mayAddLeft2 = true;
            }
            // add the connections if possible
            if (mayAddLeft) {
                addLane2LaneConnection(i+1, el.edge, el.lane+1, false);
            }
            if (mayAddLeft2) {
                addLane2LaneConnection(i, el.edge, el.lane+1, false);
            }
            reachableFromLane = myReachable[i];
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


int
NBEdge::getMinConnectedLane(NBEdge *of) const
{
    int ret = -1;
    for (ReachableFromLaneVector::const_iterator i=myReachable.begin(); i!=myReachable.end(); ++i) {
        const EdgeLaneVector &elv = (*i);
        for (EdgeLaneVector::const_iterator j=elv.begin(); j!=elv.end(); ++j) {
            if ((*j).edge==of) {
                if (ret==-1 || (int)(*j).lane<ret) {
                    ret = (*j).lane;
                }
            }
        }
    }
    return ret;
}


int
NBEdge::getMaxConnectedLane(NBEdge *of) const
{
    int ret = -1;
    for (ReachableFromLaneVector::const_iterator i=myReachable.begin(); i!=myReachable.end(); ++i) {
        const EdgeLaneVector &elv = (*i);
        for (EdgeLaneVector::const_iterator j=elv.begin(); j!=elv.end(); ++j) {
            if ((*j).edge==of) {
                if (ret==-1 || (int)(*j).lane>ret) {
                    ret = (*j).lane;
                }
            }
        }
    }
    return ret;
}


void
NBEdge::setLoadedLength(SUMOReal val)
{
    myLoadedLength = val;
}


void
NBEdge::dismissVehicleClassInformation()
{
    myAllowedOnLanes = std::vector<std::vector<SUMOVehicleClass> >(myNolanes);
    myNotAllowedOnLanes = std::vector<std::vector<SUMOVehicleClass> >(myNolanes);
}


/****************************************************************************/
