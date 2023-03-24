/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NLEdgeControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Leonhard Luecken
/// @date    Mon, 9 Jul 2001
///
// Interface for building edges
/****************************************************************************/
#include <config.h>

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <iterator>
#include <mesosim/MELoop.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include "NLBuilder.h"
#include "NLEdgeControlBuilder.h"
#include <utils/iodevices/OutputDevice.h>


// ===========================================================================
// method definitions
// ===========================================================================
NLEdgeControlBuilder::NLEdgeControlBuilder()
    : myCurrentNumericalLaneID(0), myCurrentNumericalEdgeID(0), myEdges(0), myCurrentLaneIndex(-1) {
    myActiveEdge = (MSEdge*) nullptr;
    myLaneStorage = new std::vector<MSLane*>();
}


NLEdgeControlBuilder::~NLEdgeControlBuilder() {
    delete myLaneStorage;
}


void
NLEdgeControlBuilder::beginEdgeParsing(
    const std::string& id, const SumoXMLEdgeFunc function,
    const std::string& streetName,
    const std::string& edgeType,
    int priority,
    const std::string& bidi,
    double distance) {
    // closeEdge might not have been called because the last edge had an error, so we clear the lane storage
    myLaneStorage->clear();
    myActiveEdge = buildEdge(id, function, streetName, edgeType, priority, distance);
    if (MSEdge::dictionary(id) != nullptr) {
        throw InvalidArgument("Another edge with the id '" + id + "' exists.");
    }
    myEdges.push_back(myActiveEdge);
    if (bidi != "") {
        myBidiEdges[myActiveEdge] = bidi;
    }
}


MSLane*
NLEdgeControlBuilder::addLane(const std::string& id,
                              double maxSpeed, double friction, double length,
                              const PositionVector& shape, double width,
                              SVCPermissions permissions,
                              SVCPermissions changeLeft, SVCPermissions changeRight,
                              int index, bool isRampAccel,
                              const std::string& type) {
    MSLane* lane = new MSLane(id, maxSpeed, friction, length, myActiveEdge, myCurrentNumericalLaneID++, shape, width, permissions, changeLeft, changeRight, index, isRampAccel, type);
    myLaneStorage->push_back(lane);
    myCurrentLaneIndex = index;
    return lane;
}


void
NLEdgeControlBuilder::addStopOffsets(const StopOffset& stopOffset) {
    if (myCurrentLaneIndex == -1) {
        setDefaultStopOffset(stopOffset);
    } else {
        updateCurrentLaneStopOffset(stopOffset);
    }
}


std::string
NLEdgeControlBuilder::reportCurrentEdgeOrLane() const {
    std::stringstream ss;
    if (myCurrentLaneIndex != -1) {
        ss << "lane " << myCurrentLaneIndex << " of ";
    }
    ss << "edge '" << myActiveEdge->getID() << "'";
    return ss.str();
}


void
NLEdgeControlBuilder::updateCurrentLaneStopOffset(const StopOffset& stopOffset) {
    if (myLaneStorage->size() == 0) {
        throw ProcessError("myLaneStorage cannot be empty");
    }
    if (stopOffset.isDefined()) {
        if (myLaneStorage->back()->getLaneStopOffsets().isDefined()) {
            WRITE_WARNING("Duplicate stopOffset definition for lane " + toString(myLaneStorage->back()->getIndex()) +
                          " on edge " + myActiveEdge->getID() + "!")
        } else {
            myLaneStorage->back()->setLaneStopOffset(stopOffset);
        }
    }
}


void
NLEdgeControlBuilder::setDefaultStopOffset(const StopOffset& stopOffsets) {
    if (myCurrentDefaultStopOffset.isDefined()) {
        WRITE_WARNING("Duplicate stopOffset definition for edge " + myActiveEdge->getID() + ". Ignoring duplicate specification.")
    } else {
        myCurrentDefaultStopOffset = stopOffsets;
    }
}


void
NLEdgeControlBuilder::applyDefaultStopOffsetsToLanes() {
    if (myActiveEdge == nullptr) {
        throw ProcessError("myActiveEdge cannot be nullptr");
    }
    if (myCurrentDefaultStopOffset.isDefined()) {
        for (const auto& l : *myLaneStorage) {
            if (!l->getLaneStopOffsets().isDefined()) {
                l->setLaneStopOffset(myCurrentDefaultStopOffset);
            }
        }
    }
}


void
NLEdgeControlBuilder::addNeigh(const std::string id) {
    myOppositeLanes.push_back({myLaneStorage->back(), id});
}


MSEdge*
NLEdgeControlBuilder::closeEdge() {
    applyDefaultStopOffsetsToLanes();
    std::vector<MSLane*>* lanes = new std::vector<MSLane*>();
    lanes->reserve(myLaneStorage->size());
    copy(myLaneStorage->begin(), myLaneStorage->end(), back_inserter(*lanes));
    myLaneStorage->clear();
    myActiveEdge->initialize(lanes);
    myCurrentDefaultStopOffset.reset();
    return myActiveEdge;
}


void
NLEdgeControlBuilder::closeLane() {
    myCurrentLaneIndex = -1;
}


MSEdgeControl*
NLEdgeControlBuilder::build(const MMVersion& networkVersion) {
    if (MSGlobals::gUseMesoSim && !OptionsCont::getOptions().getBool("meso-lane-queue")) {
        MSEdge::setMesoIgnoredVClasses(parseVehicleClasses(OptionsCont::getOptions().getStringVector("meso-ignore-lanes-by-vclass")));
    }
    // connecting opposite lanes must happen before MSEdge::closeBuilding
    for (auto item : myOppositeLanes) {
        MSLane* oppo = MSLane::dictionary(item.second);
        if (oppo == nullptr) {
            WRITE_ERRORF("Unknown neigh lane '%' for lane '%'", item.second, item.first->getID());
        } else {
            item.first->setOpposite(oppo);
        }
    }
    // consistency check
    for (auto item : myOppositeLanes) {
        if (item.first->getOpposite() != nullptr && item.first->getOpposite()->getOpposite() != item.first) {
            WRITE_WARNINGF(TL("Asymmetrical neigh lane '%' for lane '%'"), item.second, item.first->getID());
            item.first->getOpposite()->setOpposite(item.first);
        }
    }
    for (MSEdge* const edge : myEdges) {
        edge->closeBuilding();
    }
    for (MSEdge* const edge : myEdges) {
        edge->rebuildAllowedTargets(false);
        // segment building depends on the finished list of successors (for multi-queue)
        if (MSGlobals::gUseMesoSim && !edge->getLanes().empty()) {
            MSGlobals::gMesoNet->buildSegmentsFor(*edge, OptionsCont::getOptions());
        }
        edge->buildLaneChanger();
    }
    // mark internal edges belonging to a roundabout (after all edges are build)
    if (MSGlobals::gUsingInternalLanes) {
        for (MSEdge* const edge : myEdges) {
            if (edge->isInternal()) {
                if (edge->getNumSuccessors() != 1 || edge->getNumPredecessors() != 1) {
                    throw ProcessError(TLF("Internal edge '%' is not properly connected (probably a manually modified net.xml).", edge->getID()));
                }
                if (edge->getSuccessors()[0]->isRoundabout() || edge->getPredecessors()[0]->isRoundabout()) {
                    edge->markAsRoundabout();
                }
            }
        }
    }
    if (!deprecatedVehicleClassesSeen.empty()) {
        WRITE_WARNINGF(TL("Deprecated vehicle classes '%' in input network."), toString(deprecatedVehicleClassesSeen));
        deprecatedVehicleClassesSeen.clear();
    }
    // check for bi-directional edges (this are edges in opposing direction and superposable/congruent shapes)
    if (myBidiEdges.size() > 0 || networkVersion > MMVersion(1, 0)) {
        for (auto& item : myBidiEdges) {
            item.first->checkAndRegisterBiDirEdge(item.second);
        }
        //WRITE_MESSAGEF(TL("Loaded % bidirectional edges"), toString(myBidiEdges.size()));
    } else {
        // legacy network
        for (MSEdge* e : myEdges) {
            e->checkAndRegisterBiDirEdge();
        }
    }
    return new MSEdgeControl(myEdges);
}


MSEdge*
NLEdgeControlBuilder::buildEdge(const std::string& id, const SumoXMLEdgeFunc function,
                                const std::string& streetName, const std::string& edgeType, const int priority, const double distance) {
    return new MSEdge(id, myCurrentNumericalEdgeID++, function, streetName, edgeType, priority, distance);
}

void NLEdgeControlBuilder::addCrossingEdges(const std::vector<std::string>& crossingEdges) {
    myActiveEdge->setCrossingEdges(crossingEdges);
}


/****************************************************************************/
