/****************************************************************************/
/// @file    MSEdge.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// A road/street connecting two junctions
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
#pragma warning(disable: 4503)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSEdge.h"
#include "MSLane.h"
#include "MSLaneChanger.h"
#include "MSGlobals.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include "MSVehicle.h"

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

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
MSEdge::DictType MSEdge::myDict;
std::vector<MSEdge*> MSEdge::myEdges;


// ===========================================================================
// member method definitions
// ===========================================================================
MSEdge::MSEdge(const std::string &id, unsigned int numericalID) throw()
        : myID(id), myNumericalID(numericalID), myLanes(0), myAllowed(0), 
        myLaneChanger(0), myVaporizationRequests(0), myLastFailedEmissionTime(-1), 
        myHaveLoadedWeights(false), myHaveBuildShortCut(false), 
        myPackedValueLine(0), myUseBoundariesOnOverride(true)//!!!
{}


MSEdge::~MSEdge() throw()
{
    delete myLaneChanger;
    if (myAllowed!=0) {
        for (AllowedLanesCont::iterator i1=myAllowed->begin(); i1!=myAllowed->end(); i1++) {
            delete(*i1).second;
        }
        ClassedAllowedLanesCont::iterator i2;
        for (i2=myClassedAllowed.begin(); i2!=myClassedAllowed.end(); i2++) {
            for (AllowedLanesCont::iterator i1=(*i2).second.begin(); i1!=(*i2).second.end(); i1++) {
                delete(*i1).second;
            }
        }
        for (i2=myClassedNotAllowed.begin(); i2!=myClassedNotAllowed.end(); i2++) {
            for (AllowedLanesCont::iterator i1=(*i2).second.begin(); i1!=(*i2).second.end(); i1++) {
                delete(*i1).second;
            }
        }
    }
    delete myAllowed;
    delete myLanes;
    // Remark: Lanes are delete using MSLane::clear();
}


void
MSEdge::initialize(AllowedLanesCont* allowed, MSLane* departLane,
                   LaneCont* lanes, EdgeBasicFunction function) throw()
{
    assert(allowed!=0);
    assert(lanes!=0);
    myAllowed = allowed;
    myDepartLane = departLane;
    myLanes = lanes;
    myFunction = function;
    if (myLanes->size() > 1 && function!=EDGEFUNCTION_INTERNAL) {
        myLaneChanger = new MSLaneChanger(myLanes);
    }
    // build the classed allowed lanes
    myHaveClassConstraints = false;
    if (myAllowed!=0) {
        // build list of vehicle classes that are constrained
        // ... all others will be not regarded (allowed) ...
        std::vector<SUMOVehicleClass> vclasses;
        LaneCont::const_iterator i2;
        for (i2=myLanes->begin(); i2!=myLanes->end(); ++i2) {
            std::vector<SUMOVehicleClass>::const_iterator j;
            const std::vector<SUMOVehicleClass> &allowed = (*i2)->getAllowedClasses();
            for (j=allowed.begin(); j!=allowed.end(); j++) {
                if (find(vclasses.begin(), vclasses.end(), *j)==vclasses.end()) {
                    vclasses.push_back(*j);
                }
            }
            const std::vector<SUMOVehicleClass> &disallowed = (*i2)->getNotAllowedClasses();
            for (j=disallowed.begin(); j!=disallowed.end(); j++) {
                if (find(vclasses.begin(), vclasses.end(), *j)==vclasses.end()) {
                    vclasses.push_back(*j);
                }
            }
        }
        // go through these classes
        std::vector<SUMOVehicleClass>::const_iterator j;
        for (j=vclasses.begin(); j!=vclasses.end(); ++j) {
            // copy the possibilities first
            // - ok, this is still not the complete truth as a lane may be used
            //   differently for different vehicles classes, should be
            //   revisited one day
            AllowedLanesCont nallowed;
            AllowedLanesCont::iterator i1;
            for (i1=myAllowed->begin(); i1!=myAllowed->end(); ++i1) {
                LaneCont *nc = new LaneCont(*((*i1).second));
                nallowed[(*i1).first] = nc;
            }
            // now go through the lists of lanes
            // remove a lane if
            // a) this vehicle class is disallowed on this lane
            bool isAllowedSomewhere = false;
            bool wasRemoved = false;
            for (i1=nallowed.begin(); i1!=nallowed.end(); ++i1) {
                for (LaneCont::iterator i2=(*i1).second->begin(); i2!=(*i1).second->end();) {
                    // ... for later actions ...
                    const std::vector<SUMOVehicleClass> &allowed = (*i2)->getAllowedClasses();
                    if (find(allowed.begin(), allowed.end(), *j)!=allowed.end()) {
                        isAllowedSomewhere = true;
                    }
                    // remove in to remove
                    const std::vector<SUMOVehicleClass> &disallowed = (*i2)->getNotAllowedClasses();
                    if (find(disallowed.begin(), disallowed.end(), *j)!=disallowed.end()) {
                        wasRemoved = true;
                        assert(find((*i1).second->begin(), (*i1).second->end(), (*i2))!=(*i1).second->end());
                        i2 = (*i1).second->erase(find((*i1).second->begin(), (*i1).second->end(), (*i2)));
                    } else {
                        ++i2;
                    }
                }
            }
            // b) is allowed on all other but not on this
            if (isAllowedSomewhere) {
                for (i1=nallowed.begin(); i1!=nallowed.end(); ++i1) {
                    for (LaneCont::iterator i2=(*i1).second->begin(); i2!=(*i1).second->end();) {
                        // ... for later actions ...
                        const std::vector<SUMOVehicleClass> &allowed = (*i2)->getAllowedClasses();
                        if (find(allowed.begin(), allowed.end(), *j)==allowed.end()) {
                            i2 = (*i1).second->erase(find((*i1).second->begin(), (*i1).second->end(), (*i2)));
                        } else {
                            ++i2;
                        }
                    }
                }
            }
            myClassedAllowed[*j] = nallowed;
            myHaveClassConstraints = true;
        }
    }
}


const MSEdge::LaneCont*
MSEdge::allowedLanes(const MSEdge& destination, SUMOVehicleClass vclass) const throw()
{
    if (!myHaveClassConstraints||vclass==SVC_UNKNOWN||myClassedAllowed.find(vclass)==myClassedAllowed.end()) {
        AllowedLanesCont::const_iterator it = myAllowed->find(&destination);
        if (it!=myAllowed->end()) {
            return it->second;
        } else {
            return 0; // Destination-edge not found.
        }
    }
    // ok, we have vehicle class based prohibitions
    AllowedLanesCont::const_iterator it;
    if (myClassedAllowed.find(vclass)!=myClassedAllowed.end()) {
        it = myClassedAllowed.find(vclass)->second.find(&destination);
        if (it==myAllowed->end()) {
            it = myAllowed->find(&destination);
        }
    }
    if (it==myAllowed->end()) {
        return 0;
    }
    // !!! missing: what happens to prohibited classes?
    return it->second;
}


MSLane * const 
MSEdge::leftLane(const MSLane * const lane) const throw()
{
    LaneCont::iterator laneIt = find(myLanes->begin(), myLanes->end(), lane);
    if (laneIt==myLanes->end()||laneIt==myLanes->end()-1) {
        return 0;
    }
    return *(laneIt+1);
}


MSLane * const 
MSEdge::rightLane(const MSLane * const lane) const throw()
{
    LaneCont::iterator laneIt = find(myLanes->begin(), myLanes->end(), lane);
    if (laneIt==myLanes->end()||laneIt==myLanes->begin()) {
        return 0;
    }
    return *(laneIt-1);
}


const MSEdge * const
MSEdge::getFollower(unsigned int n) const throw()
{
    AllowedLanesCont::const_iterator i = myAllowed->begin();
    while (n!=0) {
        ++i;
        --n;
    }
    return (*i).first;
}


std::vector<MSEdge *>
MSEdge::getFollowingEdges() const throw()
{
    std::vector<MSEdge*> ret;
    for (AllowedLanesCont::iterator i=myAllowed->begin(); i!=myAllowed->end(); ++i) {
        ret.push_back((MSEdge*)(*i).first);
    }
    return ret;
}


std::vector<MSEdge*>
MSEdge::getIncomingEdges() const throw()
{
    std::vector<MSEdge*> ret;
    for (DictType::iterator edge = myDict.begin(); edge != myDict.end();
            ++edge) {

        const MSEdge::LaneCont *allowed = (*edge).second->allowedLanes(*this, SVC_UNKNOWN);
        if (allowed!=0) {
            ret.push_back(edge->second);
        }
    }
    return ret;
}


SUMOTime 
MSEdge::incVaporization(SUMOTime) throw(ProcessError) 
{
    ++myVaporizationRequests;
    return 0;
}


SUMOTime 
MSEdge::decVaporization(SUMOTime) throw(ProcessError) 
{
    --myVaporizationRequests;
    return 0;
}


bool
MSEdge::emit(MSVehicle &v, SUMOTime time) const throw()
{
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        v.update_segment(MSGlobals::gMesoNet->getSegmentForEdge(this));
        v.update_tEvent((SUMOReal) time);
        bool insertToNet = false;
        if (MSGlobals::gMesoNet->getSegmentForEdge(this)->initialise2(&v, 0, time, insertToNet)) {
            if (insertToNet) {//MSGlobals::gMesoNet->getSegmentForEdge(this)->noCars()==1) {
                MSGlobals::gMesoNet->addCar(&v);
                v.inserted = true;
            }
            return true;
        } else {
            return false;
        }
    }
#endif
    // when vaporizing, no vehicles are emitted...
    if(isVaporizing()) {
        return false;
    }
    if (myFunction!=EDGEFUNCTION_SOURCE) {
        return myDepartLane->emit(v);
    } else {
        const LaneCont &lanes =  v.departLanes();
        int minI = 0;
        int ir = 0;
        unsigned int noCars = (unsigned int)(*getLanes())[0]->length();
        {
            for (LaneCont::const_iterator i=lanes.begin(); i!=lanes.end(); ++i, ++ir) {
                if ((*i)->getVehicleNumber()<noCars) {
                    minI = ir;
                    noCars = (*i)->getVehicleNumber();
                }
            }
        }
        if (lanes[minI]->emit(v)) {
            return true;
        } else {
            ir = 0;
            for (LaneCont::const_iterator i=lanes.begin(); i!=lanes.end(); ++i, ++ir) {
                if (ir!=minI&&(*i)->emit(v)) {
                    return true;
                }
            }
        }

        return false;
    }
}


void
MSEdge::changeLanes() throw()
{
    if (myFunction==EDGEFUNCTION_INTERNAL) {
        return;
    }
    assert(myLaneChanger != 0);
    myLaneChanger->laneChange();
}



#ifdef HAVE_INTERNAL_LANES
const MSEdge *
MSEdge::getInternalFollowingEdge(MSEdge *followerAfterInternal) const throw()
{
    //@ to be optimized
    for (LaneCont::const_iterator i=myLanes->begin(); i!=myLanes->end(); ++i) {
        MSLane *l = *i;
        const MSLinkCont &lc = l->getLinkCont();
        for (MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            MSLink *link = *j;
            if (link->getLane()->getEdge()==followerAfterInternal) {
                return link->getViaLane()->getEdge();
            }
        }
    }
    return 0;
}
#endif


bool myHaveWarned; // !!!


SUMOReal
MSEdge::getEffort(SUMOReal forTime) const throw()
{
    if(!myHaveLoadedWeights) {
        return (*myLanes)[0]->length() / (*myLanes)[0]->maxSpeed();
    }
    if (!myHaveBuildShortCut) {
        myPackedValueLine = myOwnValueLine.buildShortCut(myShortCutBegin, myShortCutEnd, myLastPackedIndex, myShortCutInterval);
        myHaveBuildShortCut = true;
    }
    SUMOTime t = (SUMOTime) forTime;
    if (myShortCutBegin>t||myShortCutEnd<t) {
        if (myUseBoundariesOnOverride) {
            if (!myHaveWarned) {
                WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(t)  + " in edge '" + getID() + "'.\n Using first/last entry.");
                myHaveWarned = true;
            }
            if (myShortCutBegin>t) {
                return myPackedValueLine[0];
            } else {
                return myPackedValueLine[myLastPackedIndex];
            }
        } else {
            // value is already set
            //  warn if wished
            if (!myHaveWarned) {
                WRITE_WARNING("No interval matches passed time "+ toString<SUMOTime>(t)  + " in edge '" + getID() + "'.\n Using edge's length / edge's speed.");
                myHaveWarned = true;
            }
        }
    }
    return myPackedValueLine[(t-myShortCutBegin)/myShortCutInterval];
}


SUMOReal
MSEdge::getCurrentEffort() const throw()
{
    SUMOReal v = 0;
    for(LaneCont::iterator i=myLanes->begin(); i!=myLanes->end(); ++i) {
        v += (*i)->getMeanSpeed();
    }
    v /= (SUMOReal) myLanes->size();
    if(v!=0) {
        return (*myLanes)[0]->length() / v;
    } else {
        return 1000000.;
    }
}


void
MSEdge::addWeight(SUMOReal value, SUMOTime timeBegin, SUMOTime timeEnd) throw()
{
    myOwnValueLine.add(timeBegin, timeEnd, value);
    myHaveLoadedWeights = true;
}


SUMOReal
MSEdge::getVehicleEffort(const MSVehicle * const v, SUMOReal t) const throw()
{
    SUMOReal teffort = v->getEffort(this, t);
    if (teffort>=0) {
        return teffort;
    }
    return MIN2((*myLanes)[0]->length()/v->getMaxSpeed(), getEffort(t));
}


bool
MSEdge::dictionary(const string &id, MSEdge* ptr) throw()
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict[id] = ptr;
        while (myEdges.size()<ptr->getNumericalID()+1) {
            myEdges.push_back(0);
        }
        myEdges[ptr->getNumericalID()] = ptr;
        return true;
    }
    return false;
}


MSEdge*
MSEdge::dictionary(const string &id) throw()
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


MSEdge*
MSEdge::dictionary(size_t id) throw()
{
    assert(myEdges.size()>id);
    return myEdges[id];
}


size_t
MSEdge::dictSize() throw()
{
    return myDict.size();
}


void
MSEdge::clear() throw()
{
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
}


/****************************************************************************/

