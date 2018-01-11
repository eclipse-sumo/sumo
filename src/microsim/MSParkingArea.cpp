/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2015-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSParkingArea.cpp
/// @author  Mirco Sturari
/// @author  Jakob Erdmann
/// @date    Tue, 19.01.2016
/// @version $Id$
///
// A area where vehicles can park next to the road
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include "MSLane.h"
#include "MSTransportable.h"
#include "MSParkingArea.h"


// ===========================================================================
// method definitions
// ===========================================================================
MSParkingArea::MSParkingArea(const std::string& id,
                             const std::vector<std::string>& lines,
                             MSLane& lane,
                             double begPos, double endPos,
                             unsigned int capacity,
                             double width, double length, double angle) :
    MSStoppingPlace(id, lines, lane, begPos, endPos),
    myCapacity(capacity),
    myWidth(width),
    myLength(length),
    myAngle(angle) {
    // initialize unspecified defaults
    if (myWidth == 0) {
        myWidth = SUMO_const_laneWidth;
    }
    if (myLength == 0) {
        myLength = getSpaceDim();
    }

    const double offset = MSNet::getInstance()->lefthand() ? -1 : 1;
    myShape = lane.getShape().getSubpart(
                  lane.interpolateLanePosToGeometryPos(begPos),
                  lane.interpolateLanePosToGeometryPos(endPos));
    myShape.move2side((lane.getWidth() / 2. + myWidth / 2.) * offset);
    // Initialize space occupancies if there is a road-side capacity
    // The overall number of lots is fixed and each lot accepts one vehicle regardless of size
    if (myCapacity > 0) {
        for (int i = 1; i <= myCapacity; ++i) {
            mySpaceOccupancies[i] = LotSpaceDefinition();
            mySpaceOccupancies[i].index = i;
            mySpaceOccupancies[i].vehicle = 0;
            mySpaceOccupancies[i].myWidth = myWidth;
            mySpaceOccupancies[i].myLength = myLength;
            mySpaceOccupancies[i].myEndPos = myBegPos + getSpaceDim() * i;

            const Position& f = myShape.positionAtOffset(getSpaceDim() * (i - 1));
            const Position& s = myShape.positionAtOffset(getSpaceDim() * (i));
            double lot_angle = ((double) atan2((s.x() - f.x()), (f.y() - s.y())) * (double) 180.0 / (double) M_PI) + myAngle;
            mySpaceOccupancies[i].myRotation = lot_angle;
            if (myAngle == 0) {
                // parking parallel to the road
                mySpaceOccupancies[i].myPosition = s;
            } else {
                // angled parking
                mySpaceOccupancies[i].myPosition = (f + s) * 0.5;
            }

        }
    }
    computeLastFreePos();
}

MSParkingArea::~MSParkingArea() {}

double
MSParkingArea::getLastFreePos(const SUMOVehicle& /* forVehicle */) const {
    return myLastFreePos;
}

Position
MSParkingArea::getVehiclePosition(const SUMOVehicle& forVehicle) {
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == &forVehicle) {
            return (*i).second.myPosition;
        }
    }
    return Position::INVALID;
}

double
MSParkingArea::getVehicleAngle(const SUMOVehicle& forVehicle) {
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == &forVehicle) {
            return (((*i).second.myRotation - 90.) * (double) M_PI / (double) 180.0);
        }
    }
    return 0.;
}


double
MSParkingArea::getSpaceDim() const {
    return myLane.interpolateLanePosToGeometryPos((myEndPos - myBegPos) / myCapacity);
}


void
MSParkingArea::addLotEntry(double x, double y, double z,
                           double width, double length, double angle) {

    const int i = (int)mySpaceOccupancies.size() + 1;

    mySpaceOccupancies[i] = LotSpaceDefinition();
    mySpaceOccupancies[i].index = i;
    mySpaceOccupancies[i].vehicle = 0;
    mySpaceOccupancies[i].myPosition = Position(x, y, z);
    mySpaceOccupancies[i].myWidth = width;
    mySpaceOccupancies[i].myLength = length;
    mySpaceOccupancies[i].myRotation = angle;
    mySpaceOccupancies[i].myEndPos = myEndPos;
    myCapacity = (int)mySpaceOccupancies.size();
    computeLastFreePos();
}


void
MSParkingArea::enter(SUMOVehicle* what, double beg, double end) {
    if (myLastFreeLot >= 1 && myLastFreeLot <= (int)mySpaceOccupancies.size()) {
        mySpaceOccupancies[myLastFreeLot].vehicle = what;
        myEndPositions[what] = std::pair<double, double>(beg, end);
        computeLastFreePos();
    }
}


void
MSParkingArea::leaveFrom(SUMOVehicle* what) {
    assert(myEndPositions.find(what) != myEndPositions.end());
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == what) {
            (*i).second.vehicle = 0;
            break;
        }
    }
    myEndPositions.erase(myEndPositions.find(what));
    computeLastFreePos();
}


void
MSParkingArea::computeLastFreePos() {
    myLastFreeLot = 0;
    myLastFreePos = myBegPos;
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == 0) {
            myLastFreeLot = (*i).first;
            myLastFreePos = (*i).second.myEndPos;
            break;
        }
    }
}


double
MSParkingArea::getWidth() const {
    return myWidth;
}


double
MSParkingArea::getLength() const {
    return myLength;
}


double
MSParkingArea::getAngle() const {
    return myAngle;
}


int
MSParkingArea::getCapacity() const {
    return myCapacity;
}


int
MSParkingArea::getOccupancy() const {
    return (int)myEndPositions.size();
}


/****************************************************************************/
