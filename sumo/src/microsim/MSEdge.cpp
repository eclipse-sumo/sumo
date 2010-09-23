/****************************************************************************/
/// @file    MSEdge.cpp
/// @author  Christian Roessel
/// @date    Tue, 06 Mar 2001
/// @version $Id$
///
// A road/street connecting two junctions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include "MSEdge.h"
#include "MSLane.h"
#include "MSLaneChanger.h"
#include "MSGlobals.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include "MSVehicle.h"
#include <utils/common/StringTokenizer.h>
#include <utils/options/OptionsCont.h>
#include "MSEdgeWeightsStorage.h"

#ifdef HAVE_MESOSIM
#include <mesosim/MELoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


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
    for (ClassedAllowedLanesCont::iterator i2=myClassedAllowed.begin(); i2!=myClassedAllowed.end(); i2++) {
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
    assert(function == EDGEFUNCTION_DISTRICT || lanes!=0);
    myDepartLane = departLane;
    myLanes = lanes;
    myFunction = function;
    if (myLanes && myLanes->size() > 1 && function!=EDGEFUNCTION_INTERNAL) {
        myLaneChanger = new MSLaneChanger(myLanes, OptionsCont::getOptions().getBool("lanechange.allow-swap"));
    }
}


void
MSEdge::closeBuilding() {
    myAllowed[0] = new std::vector<MSLane*>();
    for (std::vector<MSLane*>::iterator i=myLanes->begin(); i!=myLanes->end(); ++i) {
        myAllowed[0]->push_back(*i);
        const MSLinkCont &lc = (*i)->getLinkCont();
        for (MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); ++j) {
            MSLane *toL = (*j)->getLane();
            if (toL!=0) {
                MSEdge &to = toL->getEdge();
                //
                if (std::find(mySuccessors.begin(), mySuccessors.end(), &to)==mySuccessors.end()) {
                    mySuccessors.push_back(&to);
                }
                if (std::find(to.myPredeccesors.begin(), to.myPredeccesors.end(), this)==to.myPredeccesors.end()) {
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
    std::set<SUMOVehicleClass> vclasses;
    for (std::vector<MSLane*>::const_iterator i2=myLanes->begin(); i2!=myLanes->end(); ++i2) {
        const std::vector<SUMOVehicleClass> &allowed = (*i2)->getAllowedClasses();
        for (std::vector<SUMOVehicleClass>::const_iterator j=allowed.begin(); j!=allowed.end(); j++) {
            vclasses.insert(*j);
        }
        const std::vector<SUMOVehicleClass> &disallowed = (*i2)->getNotAllowedClasses();
        for (std::vector<SUMOVehicleClass>::const_iterator j=disallowed.begin(); j!=disallowed.end(); j++) {
            vclasses.insert(*j);
        }
    }
    // go through these classes
    for (std::set<SUMOVehicleClass>::const_iterator j=vclasses.begin(); j!=vclasses.end(); ++j) {
        // go through connected edges
        for (AllowedLanesCont::iterator i1=myAllowed.begin(); i1!=myAllowed.end(); ++i1) {
            delete myClassedAllowed[*j][(*i1).first];
            myClassedAllowed[*j][(*i1).first] = new std::vector<MSLane*>();
            // go through lanes approaching current edge
            for (std::vector<MSLane*>::iterator i2=(*i1).second->begin(); i2!=(*i1).second->end(); ++i2) {
                // allows the current vehicle class?
                if ((*i2)->allowsVehicleClass(*j)) {
                    // -> may be used
                    myClassedAllowed[*j][(*i1).first]->push_back(*i2);
                }
            }
            // assert that 0 is returned if no connection is allowed for a class
            if (myClassedAllowed[*j][(*i1).first]->size()==0) {
                delete myClassedAllowed[*j][(*i1).first];
                myClassedAllowed[*j][(*i1).first] = 0;
            }
        }
        myHaveClassConstraints = true;
    }
}


// ------------ Access to the edge's lanes
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


const std::vector<MSLane*>*
MSEdge::allowedLanes(const MSEdge& destination, SUMOVehicleClass vclass) const throw() {
    return allowedLanes(&destination, vclass);
}


const std::vector<MSLane*>*
MSEdge::allowedLanes(SUMOVehicleClass vclass) const throw() {
    return allowedLanes(0, vclass);
}


const std::vector<MSLane*>*
MSEdge::allowedLanes(const MSEdge *destination, SUMOVehicleClass vclass) const throw() {
    if (myHaveClassConstraints&&vclass!=SVC_UNKNOWN) {
        ClassedAllowedLanesCont::const_iterator i = myClassedAllowed.find(vclass);
        if (i!=myClassedAllowed.end()) {
            const AllowedLanesCont &c = (*i).second;
            AllowedLanesCont::const_iterator j = (*i).second.find(destination);
            if (j==c.end()) {
                // Destination-edge not found.
                return 0;
            }
            return (*j).second;
        }
    }
    AllowedLanesCont::const_iterator it = myAllowed.find(destination);
    if (it!=myAllowed.end()) {
        return it->second;
    } else {
        // Destination-edge not found.
        return 0;
    }
}


// ------------
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


MSLane *
MSEdge::getFreeLane(const std::vector<MSLane*>* allowed, const SUMOVehicleClass vclass) const throw() {
    if(allowed==0) {
        allowed = allowedLanes(vclass);
    }
    MSLane* res = 0;
    if (allowed != 0) {
        unsigned int noCars = INT_MAX;
        for (std::vector<MSLane*>::const_iterator i=allowed->begin(); i!=allowed->end(); ++i) {
            if ((*i)->getVehicleNumber()<noCars) {
                res = (*i);
                noCars = (*i)->getVehicleNumber();
            }
        }
    }
    return res;
}


MSLane *
MSEdge::getDepartLane(const MSVehicle &v) const throw() {
    const SUMOVehicleParameter &pars = v.getParameter();
    switch (pars.departLaneProcedure) {
    case DEPART_LANE_GIVEN:
        if ((int) myLanes->size() <= pars.departLane || !(*myLanes)[pars.departLane]->allowsVehicleClass(v.getVehicleType().getVehicleClass())) {
            return 0;
        }
        return (*myLanes)[pars.departLane];
    case DEPART_LANE_RANDOM:
        return RandHelper::getRandomFrom(*allowedLanes(v.getVehicleType().getVehicleClass()));
    case DEPART_LANE_FREE:
        return getFreeLane(0, v.getVehicleType().getVehicleClass());
    case DEPART_LANE_DEPARTLANE:
    case DEPART_LANE_DEFAULT:
    default:
        break;
    }
    if (!myDepartLane->allowsVehicleClass(v.getVehicleType().getVehicleClass())) {
        return 0;
    }
    return myDepartLane;
}


bool
MSEdge::emit(MSVehicle &v, SUMOTime time) const throw(ProcessError) {
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
    MSLane* emitLane = getDepartLane(v);
    return emitLane != 0 && emitLane->emit(v);
}


void
MSEdge::changeLanes(SUMOTime t) throw() {
    if (myFunction==EDGEFUNCTION_INTERNAL) {
        return;
    }
    assert(myLaneChanger != 0);
    myLaneChanger->laneChange(t);
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
    if (myFunction == EDGEFUNCTION_DISTRICT || !myHaveClassConstraints) {
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
MSEdge::dictionary(const std::string &id, MSEdge* ptr) throw() {
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
MSEdge::dictionary(const std::string &id) throw() {
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
            throw ProcessError("The edge '" + *i + "' within the route " + rid + " is not known."
                               + "\n The route can not be build.");
        }
        into.push_back(edge);
    }
}


/****************************************************************************/

