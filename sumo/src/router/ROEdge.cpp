//---------------------------------------------------------------------------//
//                        ROEdge.cpp -
//  An edge the router may route through
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
// Revision 1.5  2003/04/14 13:54:20  roessel
// Removed "EdgeType::" in method ROEdge::getNoFollowing().
//
// Revision 1.4  2003/04/09 15:39:10  dkrajzew
// router debugging & extension: no routing over sources, random routes added
//
// Revision 1.3  2003/02/07 10:45:04  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
#ifdef MSVC
#include <limits>
#else
#define DBL_MAX 10000000000.0 // !!!
#endif
#include <utils/common/SErrorHandler.h>
#include <algorithm>
#include "ROLane.h"
#include "ROEdge.h"

using namespace std;

// !!! wie wärs mit einer effort-Tabelle oder Funktion über die Zeit?

ROEdge::ROEdge(const std::string &id)
    : _id(id), _dist(0), _effort(0), _usingTimeLine(false)
{
}


ROEdge::~ROEdge()
{
    for(LaneUsageCont::iterator i=_laneCont.begin(); i!=_laneCont.end(); i++) {
        delete (*i).first;
        delete (*i).second;
    }
}


void
ROEdge::postloadInit() {
    // !!! only when not lanes but the edge shall be used for routing
    if(_usingTimeLine) {
        ValueTimeLine *tmp = (*(_laneCont.begin())).second;
        size_t noLanes = _laneCont.size();
        for(size_t i=0; i<tmp->noDefinitions(); i++) {
            double currValue = 0;
            ValueTimeLine::TimeRange range(tmp->getRangeAtPosition(i));
            for(LaneUsageCont::iterator j=_laneCont.begin(); j!=_laneCont.end(); j++) {
                currValue += (*j).second->getValue(range.first);
            }
            currValue = currValue/noLanes;
            _ownValueLine.addValue(range, currValue);
        }
    }
}

void ROEdge::setEffort(double effort)
{
    _effort = effort;
}


void ROEdge::addLane(ROLane *lane)
{
    double length = lane->getLength();
    _dist = length > _dist ? length : _dist;
    _laneCont[lane] = new ValueTimeLine();
}


void ROEdge::setLane(long timeBegin, long timeEnd,
                     const std::string &id, float value)
{
    LaneUsageCont::iterator i = _laneCont.begin();
    while(i!=_laneCont.end()) {
        if((*i).first->getID()==id) {
            (*i).second->addValue(timeBegin, timeEnd, value);
            _usingTimeLine = true;
            return;
        }
    }
    SErrorHandler::add(
        string("Un unknown lane '") + id
        + string("' occured at loading weights."));
}

void
ROEdge::addSucceeder(ROEdge *s)
{
    _succeeding.push_back(s);
}

float
ROEdge::getMyEffort(long time) const {
    if(_usingTimeLine) {
        return _ownValueLine.getValue(time);
    } else {
        return _dist;
    }
}

size_t
ROEdge::getNoFollowing()
{
    if(getType()==ET_SINK) {
        return 0;
    }
    return _succeeding.size();
}


ROEdge *
ROEdge::getFollower(size_t pos)
{
    return _succeeding[pos];
}

bool
ROEdge::isConnectedTo(ROEdge *e)
{
    return find(_succeeding.begin(), _succeeding.end(), e)!=_succeeding.end();
}



double
ROEdge::getCost(long time) const
{
    return getMyEffort(time);
}


long
ROEdge::getDuration(long time) const
{
    return long(getMyEffort(time) / 20.9); // !!!
}


void ROEdge::init() {
    _explored = false;
    _effort = DBL_MAX;
    _prevKnot = 0;
    _inFrontList= false;
}

void ROEdge::initRootDistance() {
    _effort = 0;
    _inFrontList = true;
}

float ROEdge::getEffort() const {
    return _effort;
}

float ROEdge::getNextEffort(long time) const {
    return _effort + getMyEffort(time);
}

void ROEdge::setEffort(float effort) {
    _effort = effort;
}

bool ROEdge::isInFrontList() const {
    return _inFrontList;
}

bool ROEdge::isExplored() const {
    return _explored;
}

void ROEdge::setExplored(bool value) {
    _explored = value;
}

ROEdge *ROEdge::getPrevKnot() const {
    return _prevKnot;
}

void ROEdge::setPrevKnot(ROEdge *prev) {
    _prevKnot = prev;
}

std::string ROEdge::getID() const {
    return _id;
}


void
ROEdge::setType(ROEdge::EdgeType type)
{
    myType = type;
}


ROEdge::EdgeType
ROEdge::getType() const
{
    return myType;
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ROEdge.icc"
//#endif

// Local Variables:
// mode:C++
// End:


