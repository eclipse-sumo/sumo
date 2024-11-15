/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
// Segment used in Path Manager
/****************************************************************************/

#include <netbuild/NBNetBuilder.h>
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <netedit/elements/GNEPathElement.h>
#include <netedit/elements/network/GNEConnection.h>
#include <utils/router/DijkstraRouter.h>

#include "GNESegment.h"


// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// Segment - methods
// ---------------------------------------------------------------------------

Segment::Segment(GNEPathManager* pathManager, GNEPathElement* element, const GNELane* lane, std::vector<Segment*>& segments) :
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


Segment::Segment(GNEPathManager* pathManager, GNEPathElement* element, const GNEJunction* junction, std::vector<Segment*>& segments) :
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


Segment::~Segment() {
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
Segment::getContour() const {
    return myContour;
}


GNEContour*
Segment::getFromContour() const {
    return myFromContour;
}


GNEContour*
Segment::getToContour() const {
    return myToContour;
}


Segment*
Segment::getNextSegment() const {
    return myNextSegment;
}


Segment*
Segment::getPreviousSegment() const {
    return myPreviousSegment;
}


bool
Segment::isFirstSegment() const {
    return (myPreviousSegment == nullptr);
}


bool
Segment::isLastSegment() const {
    return (myNextSegment == nullptr);
}


GNEPathElement*
Segment::getPathElement() const {
    return myPathElement;
}


const GNELane*
Segment::getLane() const {
    return myLane;
}


const GNELane*
Segment::getPreviousLane() const {
    if (myPreviousSegment) {
        return myPreviousSegment->getLane();
    } else {
        return nullptr;
    }
}


const GNELane*
Segment::getNextLane() const {
    if (myNextSegment) {
        return myNextSegment->getLane();
    } else {
        return nullptr;
    }
}


int
Segment::getLaneIndex() const {
    return myLaneIndex;
}


const GNEJunction*
Segment::getJunction() const {
    return myJunction;
}


int
Segment::getJunctionIndex() const {
    return myJunctionIndex;
}


bool
Segment::isLabelSegment() const {
    return myLabelSegment;
}


void
Segment::markSegmentLabel() {
    myLabelSegment = true;
}


Segment::Segment() :
    myPathManager(nullptr),
    myPathElement(nullptr),
    myLane(nullptr),
    myJunction(nullptr),
    myNextSegment(nullptr),
    myPreviousSegment(nullptr),
    myLabelSegment(false) {
}

/****************************************************************************/
