/****************************************************************************/
/// @file    MSEdgeControl.cpp
/// @author  Christian Roessel
/// @date    Mon, 09 Apr 2001
/// @version $Id$
///
// operations.
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
MSEdgeControl::MSEdgeControl()
{}


MSEdgeControl::MSEdgeControl(EdgeCont* singleLane, EdgeCont *multiLane)
        :
        mySingleLaneEdges(singleLane),
        myMultiLaneEdges(multiLane),
        myLanes(MSLane::dictSize())
{
    // build the usage defintions for lanes
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
            pos++;
        }
    }
    // assign lane usage definitions to lanes
    for (size_t j=0; j<pos; j++) {
        myLanes[j].lane->init(*this, &(myLanes[j]));
    }
}


MSEdgeControl::~MSEdgeControl()
{
    delete mySingleLaneEdges;
    delete myMultiLaneEdges;
}


void
MSEdgeControl::moveNonCritical()
{
    LaneUsageVector::iterator i;
    // reset the approaching vehicle distance, first
    for (i=myLanes.begin(); i!=myLanes.end(); ++i) {
        (*i).lane->resetApproacherDistance();
    }
    // move non-critical vehicles
    for (i=myLanes.begin(); i!=myLanes.end(); ++i) {
        if ((*i).noVehicles!=0) {
            (*i).lane->moveNonCritical();
        }
    }
}


void
MSEdgeControl::moveCritical()
{
    for (LaneUsageVector::iterator i=myLanes.begin(); i!=myLanes.end(); ++i) {
        if ((*i).noVehicles!=0) {
            (*i).lane->moveCritical();
        }
    }
}


void
MSEdgeControl::moveFirst()
{
    LaneUsageVector::iterator i;
    for (i=myLanes.begin(); i!=myLanes.end(); ++i) {
        if ((*i).noVehicles!=0) {
            (*i).lane->setCritical();
        }
    }
    for (i=myLanes.begin(); i!=myLanes.end(); ++i) {
        (*i).lane->integrateNewVehicle();
    }
}


void
MSEdgeControl::changeLanes()
{
    for (EdgeCont::iterator edge = myMultiLaneEdges->begin();
            edge != myMultiLaneEdges->end(); ++edge) {
        assert((*edge)->getLanes()->size()>1);
        (*edge)->changeLanes();
    }
}


void
MSEdgeControl::detectCollisions(SUMOTime timestep)
{
    LaneUsageVector::iterator i;
    // Detections is made by the edge's lanes, therefore hand over.
    for (i = myLanes.begin(); i != myLanes.end(); ++i) {
        if ((*i).noVehicles>1) {
            (*i).lane->detectCollisions(timestep);
        }
    }
}


void
MSEdgeControl::insertMeanData(unsigned int number)
{
    LaneUsageVector::iterator i;
    for (i=myLanes.begin(); i!=myLanes.end(); ++i) {
        (*i).lane->insertMeanData(number);
    }
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getSingleLaneEdges() const
{
    return *mySingleLaneEdges;
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getMultiLaneEdges() const
{
    return *myMultiLaneEdges;
}


void
MSEdgeControl::saveState(std::ostream &/*os*/, long /*what*/)
{}


void
MSEdgeControl::loadState(BinaryInputDevice &/*bis*/, long /*what*/)
{}


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



/****************************************************************************/

