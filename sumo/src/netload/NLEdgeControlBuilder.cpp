/****************************************************************************/
/// @file    NLEdgeControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Interface for building edges
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <map>
#include <algorithm>
#include <iterator>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include "NLBuilder.h"
#include "NLEdgeControlBuilder.h"
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NLEdgeControlBuilder::NLEdgeControlBuilder()
    : myCurrentNumericalLaneID(0), myCurrentNumericalEdgeID(0), myEdges(0) {
    myActiveEdge = (MSEdge*) 0;
    myLaneStorage = new std::vector<MSLane*>();
}


NLEdgeControlBuilder::~NLEdgeControlBuilder() {
    delete myLaneStorage;
}


void
NLEdgeControlBuilder::beginEdgeParsing(
    const std::string& id, const MSEdge::EdgeBasicFunction function,
    const std::string& streetName,
    const std::string& edgeType,
    int priority) {
    myActiveEdge = buildEdge(id, function, streetName, edgeType, priority);
    if (MSEdge::dictionary(id) != 0) {
        throw InvalidArgument("Another edge with the id '" + id + "' exists.");
    }
    myEdges.push_back(myActiveEdge);
}


MSLane*
NLEdgeControlBuilder::addLane(const std::string& id,
                              SUMOReal maxSpeed, SUMOReal length,
                              const PositionVector& shape, SUMOReal width,
                              SVCPermissions permissions, int index) {
    MSLane* lane = new MSLane(id, maxSpeed, length, myActiveEdge, myCurrentNumericalLaneID++, shape, width, permissions, index);
    myLaneStorage->push_back(lane);
    return lane;
}


void
NLEdgeControlBuilder::addNeigh(const std::string id) {
    myLaneStorage->back()->addNeigh(id);
}


MSEdge*
NLEdgeControlBuilder::closeEdge() {
    std::vector<MSLane*>* lanes = new std::vector<MSLane*>();
    lanes->reserve(myLaneStorage->size());
    copy(myLaneStorage->begin(), myLaneStorage->end(), back_inserter(*lanes));
    myLaneStorage->clear();
    myActiveEdge->initialize(lanes);
    return myActiveEdge;
}


MSEdgeControl*
NLEdgeControlBuilder::build() {
    for (MSEdgeVector::iterator i1 = myEdges.begin(); i1 != myEdges.end(); i1++) {
        (*i1)->closeBuilding();
    }
    for (MSEdgeVector::iterator i1 = myEdges.begin(); i1 != myEdges.end(); i1++) {
        (*i1)->buildLaneChanger();
    }
    // mark internal edges belonging to a roundabout (after all edges are build)
    if (MSGlobals::gUsingInternalLanes) {
        for (MSEdgeVector::iterator i1 = myEdges.begin(); i1 != myEdges.end(); i1++) {
            MSEdge* edge = *i1;
            if (edge->isInternal()) {
                if (edge->getNumSuccessors() != 1 || edge->getNumPredecessors() != 1) {
                    throw ProcessError("Internal edge '" + edge->getID() + "' is not properly connected (probably a manually modified net.xml).");
                }
                if (edge->getSuccessors()[0]->isRoundabout() || edge->getPredecessors()[0]->isRoundabout()) {
                    edge->markAsRoundabout();
                }
            }
        }
    }
    if (!deprecatedVehicleClassesSeen.empty()) {
        WRITE_WARNING("Deprecated vehicle classes '" + toString(deprecatedVehicleClassesSeen) + "' in input network.");
        deprecatedVehicleClassesSeen.clear();
    }
    return new MSEdgeControl(myEdges);
}


MSEdge*
NLEdgeControlBuilder::buildEdge(const std::string& id, const MSEdge::EdgeBasicFunction function,
                                const std::string& streetName, const std::string& edgeType, const int priority) {
    return new MSEdge(id, myCurrentNumericalEdgeID++, function, streetName, edgeType, priority);
}

void NLEdgeControlBuilder::addCrossingEdges(const std::vector<std::string>& crossingEdges) {
    myActiveEdge->setCrossingEdges(crossingEdges);
}

/****************************************************************************/

