/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    GNESegment.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2024
///
// GNESegment used in Path Manager
/****************************************************************************/

#include <netedit/GNENet.h>

#include "GNESegment.h"

// ===========================================================================
// member method definitions
// ===========================================================================

GNESegment::GNESegment(GNEPathManager* pathManager, GNEPathElement* element, const GNELane* lane, std::vector<GNESegment*>& segments) :
    myPathManager(pathManager),
    myPathElement(element),
    myLane(lane),
    myJunction(nullptr),
    myNextSegment(nullptr),
    myPreviousSegment(nullptr),
    myLabelSegment(false),
    myContour(new GNEContour),
    myFromContour(nullptr),
    myToContour(nullptr) {
    // set previous segment
    if (segments.size() > 0) {
        // set previous segment
        myPreviousSegment = segments.back();
        myPreviousSegment->myNextSegment = this;
        // use to contour of previous segment
        myToContour = myPreviousSegment->myToContour;
        myPreviousSegment->myToContour = nullptr;
        // update lane index
        if (myPreviousSegment->myPreviousSegment && myPreviousSegment->myPreviousSegment->myLane) {
            myLaneIndex = myPreviousSegment->myPreviousSegment->myLaneIndex + 1;
        } else if (myPreviousSegment->myLane) {
            myLaneIndex = myPreviousSegment->myLaneIndex + 1;
        }
    } else {
        // this is the first segment, then create both from and to contours
        myFromContour = new GNEContour();
        myToContour = new GNEContour();
    }
    // add this segment in segments
    segments.push_back(this);
    // add segment in laneSegments
    myPathManager->addSegmentInLaneSegments(this, lane);
}


GNESegment::GNESegment(GNEPathManager* pathManager, GNEPathElement* element, const GNEJunction* junction, std::vector<GNESegment*>& segments) :
    myPathManager(pathManager),
    myPathElement(element),
    myLane(nullptr),
    myJunction(junction),
    myNextSegment(nullptr),
    myPreviousSegment(nullptr),
    myLabelSegment(false),
    myContour(new GNEContour),
    myFromContour(nullptr),
    myToContour(nullptr) {
    // set previous segment
    if (segments.size() > 0) {
        // set previous segment
        myPreviousSegment = segments.back();
        myPreviousSegment->myNextSegment = this;
        // use to contour of previous segment
        myToContour = myPreviousSegment->myToContour;
        myPreviousSegment->myToContour = nullptr;
        // update junction index
        if (myPreviousSegment->myPreviousSegment && myPreviousSegment->myPreviousSegment->myJunction) {
            myJunctionIndex = myPreviousSegment->myPreviousSegment->myJunctionIndex + 1;
        } else if (myPreviousSegment->myJunction) {
            myJunctionIndex = myPreviousSegment->myJunctionIndex + 1;
        }
    } else {
        // this is the first segment, then create both from and to contours
        myFromContour = new GNEContour();
        myToContour = new GNEContour();
    }
    // add this segment in segments
    segments.push_back(this);
    // add segment in junctionSegments
    myPathManager->addSegmentInJunctionSegments(this, junction);
}


GNESegment::~GNESegment() {
    // check if we're cleaning all segments
    if (!myPathManager->myCleaningSegments) {
        // clear segment from LaneSegments
        myPathManager->clearSegmentFromJunctionAndLaneSegments(this);
        // remove references in previous and next segment
        if (myPreviousSegment) {
            myPreviousSegment->myNextSegment = nullptr;
        }
        if (myNextSegment) {
            myNextSegment->myPreviousSegment = nullptr;
        }
    }
    // delete contours
    delete myContour;
    if (myFromContour) {
        delete myFromContour;
    }
    if (myToContour) {
        delete myToContour;
    }
}


GNEContour*
GNESegment::getContour() const {
    return myContour;
}


GNEContour*
GNESegment::getFromContour() const {
    return myFromContour;
}


GNEContour*
GNESegment::getToContour() const {
    return myToContour;
}


GNESegment*
GNESegment::getNextSegment() const {
    return myNextSegment;
}


GNESegment*
GNESegment::getPreviousSegment() const {
    return myPreviousSegment;
}


bool
GNESegment::isFirstSegment() const {
    return (myPreviousSegment == nullptr);
}


bool
GNESegment::isLastSegment() const {
    return (myNextSegment == nullptr);
}


GNEPathElement*
GNESegment::getPathElement() const {
    return myPathElement;
}


const GNELane*
GNESegment::getLane() const {
    return myLane;
}


const GNELane*
GNESegment::getPreviousLane() const {
    if (myPreviousSegment) {
        return myPreviousSegment->getLane();
    } else {
        return nullptr;
    }
}


const GNELane*
GNESegment::getNextLane() const {
    if (myNextSegment) {
        return myNextSegment->getLane();
    } else {
        return nullptr;
    }
}


int
GNESegment::getLaneIndex() const {
    return myLaneIndex;
}


const GNEJunction*
GNESegment::getJunction() const {
    return myJunction;
}


int
GNESegment::getJunctionIndex() const {
    return myJunctionIndex;
}


bool
GNESegment::isLabelSegment() const {
    return myLabelSegment;
}


void
GNESegment::markSegmentLabel() {
    myLabelSegment = true;
}


GNESegment::GNESegment() :
    myPathManager(nullptr),
    myPathElement(nullptr),
    myLane(nullptr),
    myJunction(nullptr),
    myNextSegment(nullptr),
    myPreviousSegment(nullptr),
    myLabelSegment(false) {
}

/****************************************************************************/
