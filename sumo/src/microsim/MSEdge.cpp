/****************************************************************************/
/// @file    MSEdge.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// A road/street connecting two junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/StringTokenizer.h>
#include "MSEdgeWeightsStorage.h"

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
        : myID(id), myNumericalID(numericalID), myLanes(0),
        myLaneChanger(0), myVaporizationRequests(0), myLastFailedEmissionTime(-1) {}


MSEdge::~MSEdge() throw() {
    delete myLaneChanger;
    for (AllowedLanesCont::iterator i1=myAllowed.begin(); i1!=myAllowed.end(); i1++) {
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
    delete myLanes;
    // Note: Lanes are delete using MSLane::clear();
}


void
MSEdge::initialize(MSLane* departLane,
                   std::vector<MSLane*>* lanes, EdgeBasicFunction function) throw() {
    assert(lanes!=0);
    myDepartLane = departLane;
    myLanes = lanes;
    myFunction = function;
    if (myLanes->size() > 1 && function!=EDGEFUNCTION_INTERNAL) {
        myLaneChanger = new MSLaneChanger(myLanes);
    }
}

void
MSEdge::closeBuilding() {
    for (std::vector<MSLane*>::iterator i=myLanes->begin(); i!=myLanes->end(); ++i) {
        const MSLinkCont &lc = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            MSLane *toL = (*j)->getLane();
            if (toL!=0) {
                MSEdge &to = toL->getEdge();
                //
                if(std::find(mySuccessors.begin(), mySuccessors.end(), &to)==mySuccessors.end()) {
                    mySuccessors.push_back(&to);
                }
                if(std::find(to.myPredeccesors.begin(), to.myPredeccesors.end(), this)==to.myPredeccesors.end()) {
                    to.myPredeccesors.push_back(this);
                }
                //
                if (myAllowed.find(&to)==myAllowed.end()) {
                    myAllowed[&to] = new std::vector<MSLane*>();
                }
                myAllowed[&to]->push_back(*i);
            }
        }
    }
    std::sort(mySuccessors.begin(), mySuccessors.end(), by_id_sorter());
    rebuildAllowedLanes();
}


void
MSEdge::rebuildAllowedLanes() throw() {
    // build the classed allowed lanes
    myHaveClassConstraints = false;
    // build list of vehicle classes that are constrained
    // ... all others will be not regarded (allowed) ...
    std::vector<SUMOVehicleClass> vclasses;
    std::vector<MSLane*>::const_iterator i2;
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
        for (AllowedLanesCont::iterator i1=myAllowed.begin(); i1!=myAllowed.end(); ++i1) {
            myClassedAllowed[*j][(*i1).first] = new std::vector<MSLane*>();
            for (std::vector<MSLane*>::iterator i2=(*i1).second->begin(); i2!=(*i1).second->end(); ++i2) {
                if ((*i2)->allowsVehicleClass(*j)) {
                    myClassedAllowed[*j][(*i1).first]->push_back(*i2);
                }
            }
        }
        myHaveClassConstraints = true;
    }
}


const std::vector<MSLane*>*
MSEdge::allowedLanes(const MSEdge& destination, SUMOVehicleClass vclass) const throw() {
    if (!myHaveClassConstraints||vclass==SVC_UNKNOWN||myClassedAllowed.find(vclass)==myClassedAllowed.end()) {
        AllowedLanesCont::const_iterator it = myAllowed.find(&destination);
        if (it!=myAllowed.end()) {
            return it->second;
        } else {
            return 0; // Destination-edge not found.
        }
    }
    // ok, we have vehicle class based prohibitions
    AllowedLanesCont::const_iterator it;
    if (myClassedAllowed.find(vclass)!=myClassedAllowed.end()) {
        it = myClassedAllowed.find(vclass)->second.find(&destination);
        if (it==myClassedAllowed.find(vclass)->second.end()) {
            it = myAllowed.find(&destination);
            if (it==myAllowed.end()) {
                return 0;
            }
        }
    }
    // !!! missing: what happens to prohibited classes?
    return it->second;
}


MSLane * const
MSEdge::leftLane(const MSLane * const lane) const throw() {
    std::vector<MSLane*>::iterator laneIt = find(myLanes->begin(), myLanes->end(), lane);
    if (laneIt==myLanes->end()||laneIt==myLanes->end()-1) {
        return 0;
    }
    return *(laneIt+1);
}


MSLane * const
MSEdge::rightLane(const MSLane * const lane) const throw() {
    std::vector<MSLane*>::iterator laneIt = find(myLanes->begin(), myLanes->end(), lane);
    if (laneIt==myLanes->end()||laneIt==myLanes->begin()) {
        return 0;
    }
    return *(laneIt-1);
}


const MSEdge * const
MSEdge::getFollower(unsigned int n) const throw() {
    AllowedLanesCont::const_iterator i = myAllowed.begin();
    while (n!=0) {
        ++i;
        --n;
    }
    return (*i).first;
}


SUMOTime
MSEdge::incVaporization(SUMOTime) throw(ProcessError) {
    ++myVaporizationRequests;
    return 0;
}


SUMOTime
MSEdge::decVaporization(SUMOTime) throw(ProcessError) {
    --myVaporizationRequests;
    return 0;
}


bool
MSEdge::freeLaneEmit(MSVehicle &v, SUMOTime time, bool isReinsertion) const throw() {
    const std::vector<MSLane*> &lanes = v.getDepartLanes();
    // !!! what if the vehicle may not use any lane?
    int minI = 0;
    int ir = 0;
    unsigned int noCars = (unsigned int) lanes[0]->getVehicleNumber();
    for (std::vector<MSLane*>::const_iterator i=lanes.begin(); i!=lanes.end(); ++i, ++ir) {
        if ((*i)->getVehicleNumber()<noCars) {
            minI = ir;
            noCars = (*i)->getVehicleNumber();
        }
    }
    if (isReinsertion) {
        return lanes[minI]->freeEmit(v, MIN2(lanes[minI]->getMaxSpeed(), v.getMaxSpeed()));
    } else {
        return lanes[minI]->emit(v);
    }
}


bool
MSEdge::emit(MSVehicle &v, SUMOTime time) const throw() {
    // when vaporizing, no vehicles are emitted...
    if (isVaporizing()) {
        return false;
    }
    const SUMOVehicleParameter &pars = v.getParameter();
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        SUMOReal pos = 0.0;
        switch (pars.departPosProcedure) {
        case DEPART_POS_GIVEN:
            if (pars.departPos >= 0.) {
                pos = pars.departPos;
            } else {
                pos = pars.departPos + getLanes()[0]->getLength();
            }
            break;
        case DEPART_POS_RANDOM:
        case DEPART_POS_RANDOM_FREE:
            pos = RandHelper::rand(getLanes()[0]->getLength());
            break;
        default:
            break;
        }
        bool result = false;
        bool insertToNet = false;
        MESegment* segment = MSGlobals::gMesoNet->getSegmentForEdge(*this, pos);
        if (pars.departPosProcedure == DEPART_POS_FREE) {
            while (segment != 0 && !result) {
                result = segment->initialise(&v, time, insertToNet);
                segment = segment->getNextSegment();
            }
        } else {
            result = segment->initialise(&v, time, insertToNet);
        }
        if (insertToNet) {
            MSGlobals::gMesoNet->addCar(&v);
        }
        return result;
    }
#endif
    switch (pars.departLaneProcedure) {
    case DEPART_LANE_GIVEN:
        return v.getDepartLanes()[pars.departLane]->emit(v); // !!! unsecure
    case DEPART_LANE_RANDOM: {
        const std::vector<MSLane*> &lanes = v.getDepartLanes();
        return RandHelper::getRandomFrom(lanes)->emit(v);
    }
    case DEPART_LANE_FREE:
        return freeLaneEmit(v, time);
    case DEPART_LANE_DEPARTLANE:
    case DEPART_LANE_DEFAULT:
    default:
        return myDepartLane->emit(v);
    }
}


void
MSEdge::changeLanes() throw() {
    if (myFunction==EDGEFUNCTION_INTERNAL) {
        return;
    }
    assert(myLaneChanger != 0);
    myLaneChanger->laneChange();
}



#ifdef HAVE_INTERNAL_LANES
const MSEdge *
MSEdge::getInternalFollowingEdge(MSEdge *followerAfterInternal) const throw() {
    //@ to be optimized
    for (std::vector<MSLane*>::const_iterator i=myLanes->begin(); i!=myLanes->end(); ++i) {
        MSLane *l = *i;
        const MSLinkCont &lc = l->getLinkCont();
        for (MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            MSLink *link = *j;
            if (&link->getLane()->getEdge()==followerAfterInternal) {
                return &link->getViaLane()->getEdge();
            }
        }
    }
    return 0;
}
#endif


SUMOReal
MSEdge::getCurrentTravelTime() const throw() {
    SUMOReal v = 0;
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge(*this);
        unsigned segments = 0;
        do {
            v += first->getMeanSpeed();
            first = first->getNextSegment();
            segments++;
        } while (first!=0);
        v /= (SUMOReal) segments;
    } else {
#endif
        for (std::vector<MSLane*>::iterator i=myLanes->begin(); i!=myLanes->end(); ++i) {
            v += (*i)->getMeanSpeed();
        }
        v /= (SUMOReal) myLanes->size();
#ifdef HAVE_MESOSIM
    }
#endif
    if (v!=0) {
        return (*myLanes)[0]->getLength() / v;
    } else {
        return 1000000.;
    }
}


bool
MSEdge::prohibits(const SUMOVehicle * const vehicle) const throw() {
    if (!myHaveClassConstraints) {
        return false;
    }
    SUMOVehicleClass vclass = vehicle->getVehicleType().getVehicleClass();
    for (std::vector<MSLane*>::iterator i=myLanes->begin(); i!=myLanes->end(); ++i) {
        if ((*i)->allowsVehicleClass(vclass)) {
            return false;
        }
    }
    return true;
}


bool
MSEdge::dictionary(const string &id, MSEdge* ptr) throw() {
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
MSEdge::dictionary(const string &id) throw() {
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


MSEdge*
MSEdge::dictionary(size_t id) throw() {
    assert(myEdges.size()>id);
    return myEdges[id];
}


size_t
MSEdge::dictSize() throw() {
    return myDict.size();
}


void
MSEdge::clear() throw() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        delete(*i).second;
    }
    myDict.clear();
}


void
MSEdge::insertIDs(std::vector<std::string> &into) throw() {
    for (DictType::iterator i=myDict.begin(); i!=myDict.end(); ++i) {
        into.push_back((*i).first);
    }
}


void
MSEdge::parseEdgesList(const std::string &desc, std::vector<const MSEdge*> &into,
                       const std::string &rid) throw(ProcessError) {
    StringTokenizer st(desc);
    parseEdgesList(st.getVector(), into, rid);
}


void
MSEdge::parseEdgesList(const std::vector<std::string> &desc, std::vector<const MSEdge*> &into,
                       const std::string &rid) throw(ProcessError) {
    for (std::vector<std::string>::const_iterator i=desc.begin(); i!=desc.end(); ++i) {
        const MSEdge *edge = MSEdge::dictionary(*i);
        // check whether the edge exists
        if (edge==0) {
            throw ProcessError("The edge '" + *i + "' within route '" + rid + "' is not known."
                               + "\n The route can not be build.");
        }
        into.push_back(edge);
    }
}


/****************************************************************************/

