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
#include "MSTrainHelper.h"

// ===========================================================================
// method definitions
// ===========================================================================
std::vector<PositionVector> 
MSTrainHelper::getCarriageShapes(void) {
    std::vector<PositionVector> carriageShapes;
    for (const Carriage* carriage: myCarriages) {
        Position direction = carriage->front - carriage->back;
        Position perp = Position(-direction.y(), direction.x());
        PositionVector shape;
        shape.push_back(carriage->front + perp*myHalfWidth);
        shape.push_back(carriage->front - perp*myHalfWidth); 
        shape.push_back(carriage->back - perp*myHalfWidth); 
        shape.push_back(carriage->back + perp*myHalfWidth);
        carriageShapes.push_back(shape);
    }
    return carriageShapes;
}


void 
MSTrainHelper::computeTrainDimensions(double exaggeration) {
    const MSVehicleType& vtype = myTrain->getVehicleType();
    const double totalLength = vtype.getLength();
    myUpscaleLength = exaggeration;
    if (exaggeration > 1 && totalLength > 5) {
        // Reduce the length/width ratio because this is not useful at high zoom.
        const double widthLengthFactor = totalLength / 5;
        const double shrinkFactor = MIN2(widthLengthFactor, sqrt(myUpscaleLength));
        myUpscaleLength /= shrinkFactor;
    }
    myLocomotiveLength = vtype.getParameter().locomotiveLength * myUpscaleLength;
    myDefaultLength = vtype.getParameter().carriageLength * myUpscaleLength;
    myCarriageGap = vtype.getParameter().carriageGap * myUpscaleLength;
    myLength = totalLength * myUpscaleLength;
    myHalfWidth = 0.5 * vtype.getWidth() * myUpscaleLength;
    myNumCarriages = MAX2(1, 1 + (int)((myLength - myLocomotiveLength) / (myDefaultLength + myCarriageGap) + 0.5)); // Round to closest integer.
    assert(myNumCarriages > 0);
    myCarriageLengthWithGap = myLength / myNumCarriages;
    myCarriageLength = myCarriageLengthWithGap - myCarriageGap;
    myFirstCarriageLength = myCarriageLength;
    if (myDefaultLength != myLocomotiveLength && myNumCarriages > 1) {
        myFirstCarriageLength = myLocomotiveLength;
        myCarriageLengthWithGap = (myLength - myLocomotiveLength) / (myNumCarriages - 1);
        myCarriageLength = myCarriageLengthWithGap - myCarriageGap;
    }
}
    

void 
MSTrainHelper::computeCarriages(bool secondaryShape, bool reversed) {
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
    double carriageBackOffset = carriageOffset - myFirstCarriageLength;

    double curCLength = myFirstCarriageLength;
    myFirstCarriageNo = 0;  // default case - we're going forwards
    myIsReversed = (myTrain->isReversed() && reversed) || myTrain->getLaneChangeModel().isOpposite();
    if (myIsReversed) {
        myFirstCarriageNo = myNumCarriages - 1;
        if (myNumCarriages > 1) {
            carriageBackOffset = carriageOffset - myCarriageLength;
        }
    }

    const double lateralOffset = (myTrain->isParking() && myTrain->getNextStopParameter()->posLat == INVALID_DOUBLE
                                  ? (myTrain->getLane()->getWidth() * (MSGlobals::gLefthand ? -1 : 1))
                                  : -myTrain->getLateralPositionOnLane());

    for (int i = 0; i < myNumCarriages; ++i) {
        Carriage* carriage = new Carriage();
        if (i == myFirstCarriageNo) {
            curCLength = myFirstCarriageLength;
            if (myFirstCarriageNo > 0) {
                // Previous loop iteration has adjusted backpos for a normal carriage so have to correct
                carriageBackOffset += myCarriageLengthWithGap;
                carriageBackOffset -= myFirstCarriageLength + myCarriageGap;
            }
        } else {
            curCLength = myCarriageLength;
        }
        while (carriageOffset < 0) {
            const MSLane* prev = myTrain->getPreviousLane(lane, furtherIndex);
            if (prev != lane) {
                carriageOffset += prev->getLength();
            } else {
                // No lane available.
                carriageOffset = 0;
            }
            lane = prev;
        }
        while (carriageBackOffset < 0) {
            const MSLane* prev = myTrain->getPreviousLane(backLane, backFurtherIndex);
            if (prev != backLane) {
                carriageBackOffset += prev->getLength();
            } else {
                // No lane available.
                carriageBackOffset = 0;
            }
            backLane = prev;
        }
        
        carriage->front = lane->getShape(secondaryShape).positionAtOffset(carriageOffset * lane->getLengthGeometryFactor(secondaryShape), lateralOffset);
        carriage->back = backLane->getShape(secondaryShape).positionAtOffset(carriageBackOffset * lane->getLengthGeometryFactor(secondaryShape), lateralOffset);
        // TODO: compute the doors.
        myCarriages.push_back(carriage);

        carriageOffset -= (curCLength + myCarriageGap);
        carriageBackOffset -= myCarriageLengthWithGap;
    }
}