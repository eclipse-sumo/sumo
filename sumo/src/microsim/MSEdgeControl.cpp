/****************************************************************************/
/// @file    MSEdgeControl.cpp
/// @author  Christian Roessel
/// @date    Mon, 09 Apr 2001
/// @version $Id$
///
// Stores edges and lanes, performs moving of vehicle
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
MSEdgeControl::MSEdgeControl(EdgeCont* singleLane, EdgeCont *multiLane)
        :
        mySingleLaneEdges(singleLane),
        myMultiLaneEdges(multiLane),
        myLanes(MSLane::dictSize())
{
    // build the usage definitions for lanes
        // for lanes with no neighbors
    size_t pos = 0;
    EdgeCont::iterator i;
    for (i=singleLane->begin(); i!=singleLane->end(); ++i) {
        const MSEdge::LaneCont * const lanes = (*i)->getLanes();
        myLanes[pos].lane = *(lanes->begin());
        myLanes[pos].noVehicles = 0;
        myLanes[pos].vehLenSum = 0;
        myLanes[pos].firstNeigh = lanes->end();
        myLanes[pos].lastNeigh = lanes->end();
        myLanes[pos].index = pos;
        myLanes[pos].amActive = false;
        pos++;
    }
        // for lanes with neighbors
    for (i=multiLane->begin(); i!=multiLane->end(); ++i) {
        const MSEdge::LaneCont * const lanes = (*i)->getLanes();
        for (MSEdge::LaneCont::const_iterator j=lanes->begin(); j!=lanes->end(); j++) {
            myLanes[pos].lane = *j;
            myLanes[pos].noVehicles = 0;
            myLanes[pos].vehLenSum = 0;
            myLanes[pos].firstNeigh = (j+1);
            myLanes[pos].lastNeigh = lanes->end();
            myLanes[pos].index = pos;
            myLanes[pos].amActive = false;
            pos++;
        }
    }
    // assign lane usage definitions to lanes
    for (size_t j=0; j<pos; j++) {
        myLanes[j].lane->init(*this, &(myLanes[j]));
    }
}


MSEdgeControl::~MSEdgeControl() throw()
{
    delete mySingleLaneEdges;
    delete myMultiLaneEdges;
}


void
MSEdgeControl::patchActiveLanes() throw()
{
    for (set<MSLane*>::iterator i=myChangedStateLanes.begin(); i!=myChangedStateLanes.end(); ++i) {
        const LaneUsage &lu = (*i)->getLaneUsage();
        if (lu.noVehicles>0 && !lu.amActive) {
            myActiveLanes.push_back(*i);
            ((LaneUsage &)(*i)->getLaneUsage()).amActive = true;
        }
    }
    myChangedStateLanes.clear();
}


void
MSEdgeControl::moveNonCritical() throw()
{
    // move non-critical vehicles
    for (list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->moveNonCritical()) {
            ((LaneUsage &)(*i)->getLaneUsage()).amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
}


void
MSEdgeControl::moveCritical() throw()
{
    for (list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->moveCritical()) {
            ((LaneUsage &)(*i)->getLaneUsage()).amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
}


void
MSEdgeControl::moveFirst() throw()
{
    myWithVehicles2Integrate.clear();
    for (list<MSLane*>::iterator i=myActiveLanes.begin(); i!=myActiveLanes.end();) {
        if ((*i)->getVehicleNumber()==0 || (*i)->setCritical(myWithVehicles2Integrate)) {
            ((LaneUsage &)(*i)->getLaneUsage()).amActive = false;
            i = myActiveLanes.erase(i);
        } else {
            ++i;
        }
    }
    for (vector<MSLane*>::iterator i=myWithVehicles2Integrate.begin(); i!=myWithVehicles2Integrate.end(); ++i) {
        if ((*i)->integrateNewVehicle()) {
            if (!(*i)->getLaneUsage().amActive) {
                myActiveLanes.push_back(*i);
                ((LaneUsage &)(*i)->getLaneUsage()).amActive = true;
            }
        }
    }
}


void
MSEdgeControl::changeLanes() throw()
{
    for (EdgeCont::iterator edge = myMultiLaneEdges->begin(); edge != myMultiLaneEdges->end(); ++edge) {
/* @extension: no lane changing on inner lanes
        if((*edge)->getPurpose()==MSEdge::EDGEFUNCTION_INNERJUNCTION) {
            continue;
        }
*/
        assert((*edge)->getLanes()->size()>1);
        (*edge)->changeLanes();
        const MSEdge::LaneCont *lanes = (*edge)->getLanes();
        for (MSEdge::LaneCont::const_iterator i=lanes->begin(); i!=lanes->end(); ++i) {
            if ((*i)->getVehicleNumber()>0 && !(*i)->getLaneUsage().amActive) {
                myActiveLanes.push_back(*i);
                ((LaneUsage &)(*i)->getLaneUsage()).amActive = true;
            }
        }
    }
}


void
MSEdgeControl::detectCollisions(SUMOTime timestep) throw()
{
    // Detections is made by the edge's lanes, therefore hand over.
    for (list<MSLane*>::iterator i = myActiveLanes.begin(); i != myActiveLanes.end(); ++i) {
        (*i)->detectCollisions(timestep);
    }
}


void
MSEdgeControl::insertMeanData(unsigned int number) throw()
{
    for (LaneUsageVector::iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        (*i).lane->insertMeanData(number);
    }
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getSingleLaneEdges() const throw()
{
    return *mySingleLaneEdges;
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getMultiLaneEdges() const throw()
{
    return *myMultiLaneEdges;
}


std::vector<std::string>
MSEdgeControl::getEdgeNames() const
{
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
MSEdgeControl::gotActive(MSLane *l) throw()
{
    myChangedStateLanes.insert(l);
}


/****************************************************************************/

