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
/// @file    MSTrainHelper.cpp
/// @author  Benjamin Coueraud
/// @date    Fri, 8 Feb 2024
///
// A class that helps computing positions of a train's carriages.
/****************************************************************************/
#include <config.h>

#include <microsim/MSLane.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSEdge.h>
#include "MSTrainHelper.h"

#define MIN_SCALED_CARRIAGE_LENGTH 5.

const double MSTrainHelper::PEDESTRIAN_RADIUS_EXTRA_TOLERANCE = 0.01;

// ===========================================================================
// method definitions
// ===========================================================================
void
MSTrainHelper::computeTrainDimensions(double exaggeration, bool secondaryShape, double scaledLength, int vehicleQuality) {
    const MSVehicleType& vtype = myTrain->getVehicleType();
    const double laneFactor = (myTrain->getLane() != nullptr
                               ? myTrain->getLane()->getLengthGeometryFactor(secondaryShape)
                               : (myTrain->getEdge()->getLanes().size() > 0 ? myTrain->getEdge()->getLanes()[0]->getLengthGeometryFactor(secondaryShape) : 1));
    double totalLength = vtype.getLength();
    const double geometryScale = scaledLength / totalLength;
    myUpscaleLength = getUpscaleLength(exaggeration, totalLength, vtype.getWidth(), vehicleQuality);
    myLocomotiveLength = vtype.getParameter().locomotiveLength * myUpscaleLength;
    myDefaultLength = vtype.getParameter().carriageLength * myUpscaleLength;
    if (myLocomotiveLength == 0) {
        myLocomotiveLength = myDefaultLength;
    }
    const double minLength = MIN2(myLocomotiveLength, myDefaultLength);
    const double minScaledLength = MIN2(MIN_SCALED_CARRIAGE_LENGTH, minLength);
    myUnscale = geometryScale == 1 && laneFactor != 1;
    if (geometryScale < 1 && minLength * geometryScale < minScaledLength) {
        const double rescaleSmall = minScaledLength / (minLength * geometryScale);
        myLocomotiveLength *= rescaleSmall;
        myDefaultLength *= rescaleSmall;
    }
    myCarriageGap = vtype.getParameter().carriageGap * myUpscaleLength;
    myLength = totalLength * myUpscaleLength;
    myHalfWidth = 0.5 * vtype.getWidth() * exaggeration;
    myNumCarriages = MAX2(1, 1 + (int)((myLength - myLocomotiveLength) / (myDefaultLength + myCarriageGap) + 0.5)); // Round to closest integer.
    if (myUpscaleLength > 1 && vehicleQuality != 4) {
        // at high zoom, it doesn't help to draw many carriages)
        myNumCarriages = MIN2(myNumCarriages, 2);
        myLocomotiveLength = myLength / 2;
    }
    assert(myNumCarriages > 0);
    if (myNumCarriages == 1) {
        myCarriageGap = 0;
    }
    myCarriageLengthWithGap = myLength / myNumCarriages;
    myCarriageLength = myCarriageLengthWithGap - myCarriageGap;
    myFirstCarriageLength = myCarriageLength;
    if (myDefaultLength != myLocomotiveLength && myNumCarriages > 1) {
        myFirstCarriageLength = myLocomotiveLength;
        myCarriageLengthWithGap = (myLength - myLocomotiveLength) / (myNumCarriages - 1);
        myCarriageLength = myCarriageLengthWithGap - myCarriageGap;
    }
    myCarriageDoors = vtype.getParameter().carriageDoors;
}


void
MSTrainHelper::computeCarriages(bool reversed, bool secondaryShape) {
    myCarriages.clear();

    const MSLane* lane = myTrain->getLane(); // Lane on which the carriage's front is situated.
    int furtherIndex = 0;
    const MSLane* backLane = lane; // Lane on which the carriage's back is situated.
    int backFurtherIndex = furtherIndex;
    // Offsets of front and back parts of a carriage.
    double carriageOffset = myTrain->getPositionOnLane();
    if (myTrain->getLaneChangeModel().isOpposite()) {
        // This still produces some artifacts when not fully on the current lane.
        carriageOffset = MIN2(carriageOffset + myTrain->getLength(), lane->getLength());
    }
    const double unscale = myUnscale ? 1. / lane->getLengthGeometryFactor() : 1.;
    double carriageBackOffset = carriageOffset - myFirstCarriageLength * unscale;

    myFirstCarriageNo = 0;  // default case - we're going forwards
    myIsReversed = (myTrain->isReversed() && reversed) || myTrain->getLaneChangeModel().isOpposite();
    if (myIsReversed) {
        myFirstCarriageNo = myNumCarriages - 1;
        if (myNumCarriages > 1) {
            carriageBackOffset = carriageOffset - myCarriageLength;
        }
    }
    if (myTrain->getVehicleType().getParameter().locomotiveLength == 0) {
        myFirstCarriageNo = -1; // don't draw locomotive
    }

    myFirstPassengerCarriage = myDefaultLength == myLocomotiveLength || myNumCarriages == 1
                               || (myTrain->getVClass() & (SVC_RAIL_ELECTRIC | SVC_RAIL_FAST | SVC_RAIL)) == 0 ? 0 : 1;

    const double lateralOffset = (myTrain->isParking() && myTrain->getNextStopParameter()->posLat == INVALID_DOUBLE
                                  ? (myTrain->getLane()->getWidth() * (MSGlobals::gLefthand ? -1 : 1))
                                  : -myTrain->getLateralPositionOnLane());

    for (int i = 0; i < myNumCarriages; ++i) {
        Carriage* carriage = new Carriage();
        if (i == myFirstCarriageNo) {
            if (myFirstCarriageNo > 0) {
                // Previous loop iteration has adjusted backpos for a normal carriage so have to correct
                carriageBackOffset += myCarriageLengthWithGap;
                carriageBackOffset -= myFirstCarriageLength + myCarriageGap;
            }
        }
        while (carriageOffset < 0) {
            const MSLane* prev = myTrain->getPreviousLane(lane, furtherIndex);
            if (prev != lane) {
                carriageOffset += prev->getLength();
            } else {
                break;
            }
            lane = prev;
        }
        while (carriageBackOffset < 0) {
            const MSLane* prev = myTrain->getPreviousLane(backLane, backFurtherIndex);
            if (prev != backLane) {
                if (myUnscale) {
                    carriageBackOffset *= backLane->getLengthGeometryFactor() / prev->getLengthGeometryFactor();
                }
                carriageBackOffset += prev->getLength();
            } else {
                break;
            }
            backLane = prev;
        }
        carriage->front = lane->getShape(secondaryShape).positionAtOffset2D(carriageOffset * lane->getLengthGeometryFactor(secondaryShape), lateralOffset, true);
        carriage->back = backLane->getShape(secondaryShape).positionAtOffset2D(carriageBackOffset * backLane->getLengthGeometryFactor(secondaryShape), lateralOffset, true);
        myCarriages.push_back(carriage);
        lane = backLane;
        furtherIndex = backFurtherIndex;
        carriageOffset = carriageBackOffset - myCarriageGap;
        carriageBackOffset -= myCarriageLengthWithGap * unscale;
    }
}


void
MSTrainHelper::computeDoorPositions() {
    for (Carriage* carriage : myCarriages) {
        Position dir = carriage->front - carriage->back;
        const double carriageLength = dir.length2D();
        if (carriageLength > 0.0) {
            dir.norm2D();
            for (int j = 1; j <= myCarriageDoors; j++) {
                const double doorOffset = j * carriageLength / (myCarriageDoors + 1);
                carriage->doorPositions.push_back(carriage->front - dir * doorOffset);
            }
        }
    }
}


void
MSTrainHelper::computeUnboardingPositions(double passengerRadius, std::vector<Position>& unboardingPositions) {
    passengerRadius += PEDESTRIAN_RADIUS_EXTRA_TOLERANCE;
    for (Carriage* carriage : myCarriages) {
        Position dir = carriage->front - carriage->back;
        const double carriageLength = dir.length2D();
        if (carriageLength > 0.0) {
            dir.norm2D();
            const Position perp = Position(-dir.y(), dir.x());
            double nbrLongitudinalCells, longitudinalOffset;
            longitudinalOffset = std::modf((carriageLength - 2.0 * passengerRadius) / (2.0 * passengerRadius), &nbrLongitudinalCells);
            double nbrLateralCells, lateralOffset;
            lateralOffset = std::modf((myHalfWidth * 2.0 - 2.0 * passengerRadius) / (2.0 * passengerRadius), &nbrLateralCells);
            const Position gridOrigin = carriage->back + dir * (passengerRadius + 0.5 * longitudinalOffset) - perp * (myHalfWidth - passengerRadius - 0.5 * lateralOffset);
            for (unsigned int i = 0; i <= (unsigned int)nbrLongitudinalCells; i++) {
                for (unsigned int j = 0; j <= (unsigned int)nbrLateralCells; j++) {
                    carriage->unboardingPositions.push_back(gridOrigin + dir * i * 2.0 * passengerRadius + perp * j * 2.0 * passengerRadius);
                }
            }
        }
        std::copy(carriage->unboardingPositions.begin(), carriage->unboardingPositions.end(), std::back_inserter(unboardingPositions));
    }
    // Shuffle the positions upstream so that we don't have to sample later on, just pop the last element.
    RandHelper::shuffle(unboardingPositions);
}


double
MSTrainHelper::getUpscaleLength(double upscale, double length, double width, int vehicleQuality) {
    if (upscale > 1 && length > 5 && width < 5 && vehicleQuality != 4) {
        return MAX2(1.0, upscale * 5 / length);
    } else {
        return upscale;
    }
}
