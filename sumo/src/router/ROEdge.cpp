/****************************************************************************/
/// @file    ROEdge.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// An edge the router may route through
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

#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <algorithm>
#include <cassert>
#include <iostream>
#include "ROLane.h"
#include "ROEdge.h"
#include "ROVehicle.h"
#include "ROVehicleType.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static member definitions
// ===========================================================================
ROEdge::DictType ROEdge::myDict;
bool myHaveWarned = false; // !!! make this static or so


// ===========================================================================
// method definitions
// ===========================================================================
ROEdge::ROEdge(const std::string &id, int index, bool useBoundariesOnOverride)
        : myID(id), myDist(0), mySpeed(-1),
        mySupplementaryWeightAbsolut(0),
        mySupplementaryWeightAdd(0),
        mySupplementaryWeightMult(0),
        myUsingTimeLine(false),
        myIndex(index), myLength(-1), myHasSupplementaryWeights(false),
        myUseBoundariesOnOverride(useBoundariesOnOverride),
        myHaveBuildShortCut(false), myPackedValueLine(0)
{}


ROEdge::~ROEdge()
{
    for (std::vector<ROLane*>::iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        delete(*i);
    }
    delete mySupplementaryWeightAbsolut;
    delete mySupplementaryWeightAdd;
    delete mySupplementaryWeightMult;
    delete[] myPackedValueLine;
}


size_t
ROEdge::getNumericalID() const
{
    return myIndex;
}


void
ROEdge::addLane(ROLane *lane)
{
    SUMOReal length = lane->getLength();
    assert(myLength==-1||length==myLength);
    myLength = length;
    myDist = length > myDist ? length : myDist;
    SUMOReal speed = lane->getSpeed();
    mySpeed = speed > mySpeed ? speed : mySpeed;
    myDictLane[lane->getID()] = lane;
    myLanes.push_back(lane);

    std::vector<SUMOVehicleClass>::const_iterator i;
    const std::vector<SUMOVehicleClass> &allowed = lane->getAllowedClasses();
    // for allowed classes
    for (i=allowed.begin(); i!=allowed.end(); ++i) {
        SUMOVehicleClass allowedC = *i;
        std::vector<SUMOVehicleClass>::iterator t;
        // add to allowed if not already in there
        t = find(myAllowedClasses.begin(), myAllowedClasses.end(), allowedC);
        if (t==myAllowedClasses.end()) {
            myAllowedClasses.push_back(allowedC);
        }
        // remove from disallowed if allowed on the lane
        t = find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), allowedC);
        if (t!=myNotAllowedClasses.end()) {
            myNotAllowedClasses.erase(t);
        }
    }
    // for disallowed classes
    const std::vector<SUMOVehicleClass> &disallowed = lane->getNotAllowedClasses();
    for (i=disallowed.begin(); i!=disallowed.end(); ++i) {
        SUMOVehicleClass disallowedC = *i;
        std::vector<SUMOVehicleClass>::iterator t;
        // add to disallowed if not already in there
        //  and not within allowed
        t = find(myAllowedClasses.begin(), myAllowedClasses.end(), disallowedC);
        if (t==myAllowedClasses.end()) {
            t = find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), disallowedC);
            if (t==myNotAllowedClasses.end()) {
                myNotAllowedClasses.push_back(disallowedC);
            }
        }
    }
}


void
ROEdge::addWeight(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd)
{
    myOwnValueLine.add(timeBegin, timeEnd, value);
    myUsingTimeLine = true;
}


void
ROEdge::addFollower(ROEdge *s)
{
    myFollowingEdges.push_back(s);
}


SUMOReal
ROEdge::getEffort(const ROVehicle *const, SUMOTime time) const
{
    FloatValueTimeLine::SearchResult searchResult;
    FloatValueTimeLine::SearchResult supplementarySearchResult;
    // check whether an absolute value shalle be used
    if (myHasSupplementaryWeights) {
        searchResult = mySupplementaryWeightAbsolut->getSearchStateAndValue(time);
        if (searchResult.first) {
            // ok, we have an absolute value for this time step, return it
            return searchResult.second;
        }
    }

    // ok, no absolute value was found, use the normal value (without)
    //  weight as default
    SUMOReal value = (SUMOReal)(myDist / mySpeed);
    if (myUsingTimeLine) {
        if (!myHaveBuildShortCut) {
            myPackedValueLine = myOwnValueLine.buildShortCut(myShortCutBegin, myShortCutEnd, myLastPackedIndex, myShortCutInterval);
            myHaveBuildShortCut = true;
        }
        if (myShortCutBegin>time||myShortCutEnd<time) {
            if (myUseBoundariesOnOverride) {
                if (!myHaveWarned) {
                    WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(time)  + " in edge '" + myID + "'.\n Using first/last entry.");
                    myHaveWarned = true;
                }
                if (myShortCutBegin>time) {
                    value = myPackedValueLine[0];
                } else {
                    value = myPackedValueLine[myLastPackedIndex];
                }
            } else {
                // value is already set
                //  warn if wished
                if (!myHaveWarned) {
                    WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(time)  + " in edge '" + myID + "'.\n Using edge's length / edge's speed.");
                    myHaveWarned = true;
                }
            }
        } else {
            value = myPackedValueLine[(time-myShortCutBegin)/myShortCutInterval];
        }
    }

    // check for additional values
    if (myHasSupplementaryWeights == true) {
        // for factors
        supplementarySearchResult = mySupplementaryWeightMult->getSearchStateAndValue(time);
        if (supplementarySearchResult.first) {
            value *= supplementarySearchResult.second;
        }
        // for a value to add
        supplementarySearchResult = mySupplementaryWeightAdd->getSearchStateAndValue(time);
        if (supplementarySearchResult.first) {
            value += supplementarySearchResult.second;
        }
    }
    // return final value
    return value;
}


size_t
ROEdge::getNoFollowing() const
{
    if (getType()==ET_SINK) {
        return 0;
    }
    return myFollowingEdges.size();
}


ROEdge *
ROEdge::getFollower(size_t pos) const
{
    return myFollowingEdges[pos];
}


bool
ROEdge::isConnectedTo(ROEdge *e)
{
    return find(myFollowingEdges.begin(), myFollowingEdges.end(), e)!=myFollowingEdges.end();
}


SUMOReal
ROEdge::getCost(const ROVehicle *const v, SUMOTime time) const
{
    return getEffort(v, time);
}


SUMOReal
ROEdge::getDuration(const ROVehicle *const v, SUMOTime time) const
{
    return getEffort(v, time);
}


const std::string &
ROEdge::getID() const
{
    return myID;
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


SUMOReal
ROEdge::getLength() const
{
    return myLength;
}

void
ROEdge::setSupplementaryWeights(FloatValueTimeLine* absolut,
                                FloatValueTimeLine* add,
                                    FloatValueTimeLine* mult)
{
    mySupplementaryWeightAbsolut = absolut;
    mySupplementaryWeightAdd     = add;
    mySupplementaryWeightMult    = mult;
    assert(mySupplementaryWeightAbsolut != 0 &&
           mySupplementaryWeightAdd     != 0 &&
           mySupplementaryWeightMult    != 0);
    myHasSupplementaryWeights = true;
}


void
ROEdge::clear()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete(*i).second;
    }
    myDict.clear();
}

bool
ROEdge::dictionary(string id, ROEdge* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

ROEdge*
ROEdge::dictionary2(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}




ROLane *
ROEdge::getLane(std::string name)
{
    //(assert(laneNo<myLanes->size()); ??? was ist assert
    //return *((*myLanes)[laneNo]);
    std::map<std::string, ROLane*>::const_iterator i=
        ROEdge::myDictLane.find(name);
    //assert(i!=myEmitterDict.end());
    return (*i).second;
}


ROLane*
ROEdge::getLane(size_t index)
{
    return myLanes[index];
}


SUMOReal
ROEdge::getSpeed() const
{
    return mySpeed;
}


int
ROEdge::getLaneNo() const
{
    return myLanes.size();
}


bool
ROEdge::prohibits(const ROVehicle * const vehicle) const
{
    if (myAllowedClasses.size()==0&&myNotAllowedClasses.size()==0) {
        return false;
    }
    // ok, vehicles with an unknown class may be only prohibited
    //  if the edge is limited to a set of classes
    SUMOVehicleClass vclass = vehicle->getType()!=0 ? vehicle->getType()->getClass() : DEFAULT_VEH_CLASS;
    if (vclass==SVC_UNKNOWN&&myAllowedClasses.size()==0) {
        return false;
    }
    // check whether it is explicitely disallowed
    if (find(myNotAllowedClasses.begin(), myNotAllowedClasses.end(), vclass)!=myNotAllowedClasses.end()) {
        return true;
    }
    // check whether it is within the allowed classes
    if (myAllowedClasses.size()==0||find(myAllowedClasses.begin(), myAllowedClasses.end(), vclass)!=myAllowedClasses.end()) {
        return false;
    }
    // ok, we have a set of allowed vehicle classes, but this vehicle's class
    //  is not among them
    return true;
}


void
ROEdge::setNodes(RONode *from, RONode *to)
{
    myFromNode = from;
    myToNode = to;
}


RONode *
ROEdge::getFromNode() const
{
    return myFromNode;
}


RONode *
ROEdge::getToNode() const
{
    return myToNode;
}



/****************************************************************************/

