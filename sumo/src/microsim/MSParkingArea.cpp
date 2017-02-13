/****************************************************************************/
/// @file    MSParkingArea.h
/// @author  Mirco Sturari
/// @author  Jakob Erdmann
/// @date    Tue, 19.01.2016
/// @version $Id$
///
// A area where vehicles can park next to the road
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2015-2017 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <microsim/MSVehicleType.h>
#include <utils/foxtools/MFXMutex.h>
#include "MSLane.h"
#include "MSTransportable.h"
#include "MSParkingArea.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSParkingArea::MSParkingArea(const std::string& id,
                             const std::vector<std::string>& lines,
                             MSLane& lane,
                             SUMOReal begPos, SUMOReal endPos,
                             unsigned int capacity,
                             SUMOReal width, SUMOReal length, SUMOReal angle) :
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

    myShape = lane.getShape();
    myShape.move2side(lane.getWidth() / 2. + myWidth / 2.);
    myShape = myShape.getSubpart(begPos, endPos);
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
            SUMOReal lot_angle = ((SUMOReal) atan2((s.x() - f.x()), (f.y() - s.y())) * (SUMOReal) 180.0 / (SUMOReal) PI) + myAngle;
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

SUMOReal
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

SUMOReal
MSParkingArea::getVehicleAngle(const SUMOVehicle& forVehicle) {
    std::map<unsigned int, LotSpaceDefinition >::iterator i;
    for (i = mySpaceOccupancies.begin(); i != mySpaceOccupancies.end(); i++) {
        if ((*i).second.vehicle == &forVehicle) {
            return (((*i).second.myRotation - 90.) * (SUMOReal) PI / (SUMOReal) 180.0);
        }
    }
    return 0.;
}


SUMOReal
MSParkingArea::getSpaceDim() const {
    return (myEndPos - myBegPos) / myCapacity;
}


void
MSParkingArea::addLotEntry(SUMOReal x, SUMOReal y, SUMOReal z,
                           SUMOReal width, SUMOReal length, SUMOReal angle) {

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
MSParkingArea::enter(SUMOVehicle* what, SUMOReal beg, SUMOReal end) {
    if (myLastFreeLot >= 1 && myLastFreeLot <= (int)mySpaceOccupancies.size()) {
        mySpaceOccupancies[myLastFreeLot].vehicle = what;
        myEndPositions[what] = std::pair<SUMOReal, SUMOReal>(beg, end);
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


SUMOReal
MSParkingArea::getWidth() const {
    return myWidth;
}


SUMOReal
MSParkingArea::getLength() const {
    return myLength;
}


SUMOReal
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
