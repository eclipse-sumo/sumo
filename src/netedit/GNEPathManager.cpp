/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    GNEPathManager.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Feb 2011
///
// Manager for paths in NETEDIT (routes, trips, flows...)
/****************************************************************************/

#include <netbuild/NBEdgeCont.h>
#include <netedit/changes/GNEChange_Attribute.h>

#include "GNEPathManager.h"


// ===========================================================================
// member method definitions
// ===========================================================================

GNEPathManager::GNEPathManager() {
	//
}


GNEPathManager::~GNEPathManager() {
	//
}

GNEPathManager::Segment::Segment(GNEPathManager* pathManager, GNEAttributeCarrier* element) :
    myPathManager(pathManager),
    myElement(element),
    myValid(true) {

}


GNEPathManager::Segment::~Segment() {
    // clear segment from LaneSegments
    myPathManager->clearSegmentFromLaneSegments(this);

}


GNEPathManager::Segment::Segment() :
    myPathManager(nullptr),
    myElement(nullptr),
    myValid(false) {
}


void 
GNEPathManager::clearSegmentFromLaneSegments(Segment* segment) {
    // first declare vector with lanes to clear
    std::vector<GNELane*> lanesToClear;
    // now iterate over laneSegments
    for (auto &laneSegment : myLaneSegments) {
        // remove segment from segment sets
        if (laneSegment.second.find(segment) != laneSegment.second.end()) {
            laneSegment.second.erase(segment);
        }
        // now check if lane doesn't have segments
        if (laneSegment.second.empty()) {
            lanesToClear.push_back(laneSegment.first);
        }
    }
    // finally clear empty lanes from myLaneSegments
    for (const auto &laneToClear : lanesToClear) {
        myLaneSegments.erase(laneToClear);
    }
}


/****************************************************************************/
