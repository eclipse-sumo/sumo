/****************************************************************************/
/// @file    MSEdgeControl.cpp
/// @author  Christian Roessel
/// @date    Mon, 09 Apr 2001
/// @version $Id$
///
// Stores edges and lanes, performs moving of vehicle
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
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSEdgeControl.h"
#include "MSEdge.h"
#include "MSLane.h"
#include <iostream>
#include <vector>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSEdgeControl::MSEdgeControl(EdgeCont* singleLane, EdgeCont *multiLane) throw()
        :
        mySingleLaneEdges(singleLane),
        myMultiLaneEdges(multiLane),
        myLanes(MSLane::dictSize()),
        myLastLaneChange(MSEdge::dictSize()) {
    // build the usage definitions for lanes
    // for lanes with no neighbors
    EdgeCont::iterator i;
    for (i=singleLane->begin(); i!=singleLane->end(); ++i) {
        // build lane information
        const MSEdge::LaneCont * const lanes = (*i)->getLanes();
        size_t pos = (*lanes->begin())->getNumericalID();
        myLanes[pos].lane = *(lanes->begin());
        myLanes[pos].firstNeigh = lanes->end();
        myLanes[pos].lastNeigh = lanes->end();
        myLanes[pos].amActive = false;
        myLanes[pos].haveNeighbors = false;
        // build edge information
        pos = (*i)->getNumericalID();
        myLastLaneChange[pos] = -1;
    }
    // for lanes with neighbors
    for (i=multiLane->begin(); i!=multiLane->end(); ++i) {
        // build lane information
        const MSEdge::LaneCont * const lanes = (*i)->getLanes();
        for (MSEdge::LaneCont::const_iterator j=lanes->begin(); j!=lanes->end(); j++) {
            size_t pos = (*j)->getNumericalID();
            myLanes[pos].lane = *j;
            myLanes[pos].firstNeigh = (j+1);
            myLanes[pos].lastNeigh = lanes->end();
            myLanes[pos].amActive = false;
            myLanes[pos].haveNeighbors = true;
        }
        // build edge information
        size_t pos = (*i)->getNumericalID();
        myLastLaneChange[pos] = -1;
    }
    // assign lane usage definitions to lanes
    for (size_t j=0; j<myLanes.size(); j++) {
        myLanes[j].lane->init(*this, myLanes[j].firstNeigh, myLanes[j].lastNeigh);
    }
}


MSEdgeControl::~MSEdgeControl() throw() {
    delete mySingleLaneEdges;
    delete myMultiLaneEdges;
}


void
MSEdgeControl::patchActiveLanes() throw() {
    for (set<MSLane*>::iterator i=myChangedStateLanes.begin(); i!=myChangedStateLanes.end(); ++i) {
        LaneUsage &lu = myLanes[(*i)->getNumericalID()];
        // if the lane was inactive but is now...
        if (!lu.amActive && (*i)->getVehicleNumber()>0) {
            // ... add to active lanes and mark as such
            if (lu.haveNeighbors) {
                myActiveLanes.push_front(*i);
            } else {
                myActiveLanes.push_back(*i);
            }
            lu.amActive = true;
        }
    }
    myChangedStateLanes.clear();
}


void
MSEdgeControl::moveNonCritical() throw() {
    // move non-critical vehicles
    for (list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->moveNonCritical()) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
}


void
MSEdgeControl::moveCritical() throw() {
    for (list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->moveCritical()) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
}


void
MSEdgeControl::moveFirst() throw() {
    myWithVehicles2Integrate.clear();
    for (list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->setCritical(myWithVehicles2Integrate)) {
            myLanes[(*i)->getNumericalID()].amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
    for (vector<MSLane*>::iterator i=myWithVehicles2Integrate.begin(); i!=myWithVehicles2Integrate.end(); ++i) {
        if ((*i)->integrateNewVehicle()) {
            LaneUsage &lu = myLanes[(*i)->getNumericalID()];
            if (!lu.amActive) {
                if (lu.haveNeighbors) {
                    myActiveLanes.push_front(*i);
                } else {
                    myActiveLanes.push_back(*i);
                }
                lu.amActive = true;
            }
        }
    }
}


void
MSEdgeControl::changeLanes() throw() {
    SUMOTime step = MSNet::getInstance()->getCurrentTimeStep();
    std::vector<MSLane*> toAdd;
    for (list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        LaneUsage &lu = myLanes[(*i)->getNumericalID()];
        /*
        if((*i)->getEdge()->getPurpose()==MSEdge::EDGEFUNCTION_INNERJUNCTION) {
            ++i;
            continue;
        }
        */
        if (lu.haveNeighbors) {
            const MSEdge * const edge = (*i)->getEdge();
            if (myLastLaneChange[edge->getNumericalID()]!=step) {
                myLastLaneChange[edge->getNumericalID()] = step;
                ((MSEdge*) edge)->changeLanes();
                const MSEdge::LaneCont *lanes = edge->getLanes();
                for (MSEdge::LaneCont::const_iterator i=lanes->begin(); i!=lanes->end(); ++i) {
                    LaneUsage &lu = myLanes[(*i)->getNumericalID()];
                    if ((*i)->getVehicleNumber()>0 && !lu.amActive) {
                        toAdd.push_back(*i);
                        lu.amActive = true;
                    }
                }
            }
            ++i;
        } else {
            i = myActiveLanes.end();
        }
    }
    for (vector<MSLane*>::iterator i=toAdd.begin(); i!=toAdd.end(); ++i) {
        myActiveLanes.push_front(*i);
    }
}


void
MSEdgeControl::detectCollisions(SUMOTime timestep) throw() {
    // Detections is made by the edge's lanes, therefore hand over.
    for (list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end(); ++i) {
        (*i)->detectCollisions(timestep);
    }
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getSingleLaneEdges() const throw() {
    return *mySingleLaneEdges;
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getMultiLaneEdges() const throw() {
    return *myMultiLaneEdges;
}


std::vector<std::string>
MSEdgeControl::getEdgeNames() const throw() {
    std::vector<std::string> ret;
    EdgeCont::const_iterator i;
    for (i=mySingleLaneEdges->begin(); i!=mySingleLaneEdges->end(); ++i) {
        ret.push_back((*i)->getID());
    }
    for (i=myMultiLaneEdges->begin(); i!=myMultiLaneEdges->end(); ++i) {
        ret.push_back((*i)->getID());
    }
    return ret;
}


void
MSEdgeControl::gotActive(MSLane *l) throw() {
    myChangedStateLanes.insert(l);
}


/****************************************************************************/

