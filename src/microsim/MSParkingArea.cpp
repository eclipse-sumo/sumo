/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2015-2021 German Aerospace Center (DLR) and others.
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
/// @file    MSParkingArea.cpp
/// @author  Mirco Sturari
/// @author  Jakob Erdmann
/// @date    Tue, 19.01.2016
///
// A area where vehicles can park next to the road
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <utils/common/WrappingCommand.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <utils/geom/Position.h>
#include <utils/geom/GeomHelper.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleType.h>
#include "MSLane.h"
#include <microsim/transportables/MSTransportable.h>
#include "MSParkingArea.h"
#include "MSGlobals.h"

//#define DEBUG_RESERVATIONS
//#define DEBUG_COND2(obj) (obj.getID() == "v.3")
#define DEBUG_COND2(obj) (obj.isSelected())


// ===========================================================================
// method definitions
// ===========================================================================
MSParkingArea::MSParkingArea(const std::string& id, const std::vector<std::string>& lines,
                             MSLane& lane, double begPos, double endPos, int capacity, double width, double length,
                             double angle, const std::string& name, bool onRoad,
                             const std::string& departPos) :
    MSStoppingPlace(id, SUMO_TAG_PARKING_AREA, lines, lane, begPos, endPos, name),
    myCapacity(0),
    myOnRoad(onRoad),
    myWidth(width),
    myLength(length),
    myAngle(angle),
    myEgressBlocked(false),
    myReservationTime(-1),
    myReservations(0),
    myReservationMaxLength(0),
    myNumAlternatives(0),
    myLastStepOccupancy(0),
    myDepartPos(-1),
    myDepartPosDefinition(DepartPosDefinition::DEFAULT),
    myUpdateEvent(nullptr) {
    // initialize unspecified defaults
    if (myWidth == 0) {
        myWidth = SUMO_const_laneWidth;
    }
    const double spaceDim = capacity > 0 ? myLane.interpolateLanePosToGeometryPos((myEndPos - myBegPos) / capacity) : 7.5;
    if (myLength == 0) {
        myLength = spaceDim;
    }
    if (departPos != "") {
        std::string error;
        if (!SUMOVehicleParameter::parseDepartPos(departPos, toString(myElement), getID(), myDepartPos, myDepartPosDefinition, error)) {
            throw ProcessError(error);
        }
        if (myDepartPosDefinition != DepartPosDefinition::GIVEN) {
            // maybe allow other methods at a later time
            throw ProcessError("Only a numerical departPos is supported for " + toString(myElement) + " '" + getID() + "'");
        } else if (myDepartPos < 0 || myDepartPos > lane.getLength()) {
            throw ProcessError("Invalid departPos for " + toString(myElement) + " '" + getID() + "'");
        }
    }

    const double offset = MSGlobals::gLefthand ? -1 : 1;
    myShape = lane.getShape().getSubpart(
                  lane.interpolateLanePosToGeometryPos(begPos),
                  lane.interpolateLanePosToGeometryPos(endPos));
    if (!myOnRoad) {
        myShape.move2side((lane.getWidth() / 2. + myWidth / 2.) * offset);
    }
    // Initialize space occupancies if there is a road-side capacity
    // The overall number of lots is fixed and each lot accepts one vehicle regardless of size
    for (int i = 0; i < capacity; ++i) {
        // calculate pos, angle and slope of parking lot space
        const Position pos = GeomHelper::calculateLotSpacePosition(myShape, i, spaceDim, myAngle, myWidth, myLength);
        double spaceAngle = GeomHelper::calculateLotSpaceAngle(myShape, i, spaceDim, myAngle);
        double spaceSlope = GeomHelper::calculateLotSpaceSlope(myShape, i, spaceDim);
        // add lotEntry
        addLotEntry(pos.x(), pos.y(), pos.z(), myWidth, myLength, spaceAngle, spaceSlope);
        // update endPos
        mySpaceOccupancies.back().endPos = MIN2(myEndPos, myBegPos + MAX2(POSITION_EPS, spaceDim * (i + 1)));
    }
    computeLastFreePos();
}


MSParkingArea::~MSParkingArea() {}


void
MSParkingArea::addLotEntry(double x, double y, double z, double width, double length, double angle, double slope) {
    // create LotSpaceDefinition
    LotSpaceDefinition lsd((int)mySpaceOccupancies.size(), nullptr, x, y, z, angle, slope, width, length);
    // If we are modelling parking set the end position to the lot position relative to the lane
    // rather than the end of the parking area - this results in vehicles stopping nearer the space
    // and re-entering the lane nearer the space. (If we are not modelling parking the vehicle will usually
    // enter the space and re-enter at the end of the parking area.)
    if (MSGlobals::gModelParkingManoeuver) {
        const double offset = this->getLane().getShape().nearest_offset_to_point2D(lsd.position);
        if (offset <  getBeginLanePosition()) {
            lsd.endPos =  getBeginLanePosition() + POSITION_EPS;
        } else {
            if (this->getLane().getLength() > offset) {
                lsd.endPos = offset;
            } else {
                lsd.endPos = this->getLane().getLength() - POSITION_EPS;
            }
        }
        // Work out the angle of the lot relative to the lane  (-90 adjusts for the way the bay is drawn )
        double relativeAngle = fmod(lsd.rotation - 90., 360) - fmod(RAD2DEG(this->getLane().getShape().rotationAtOffset(lsd.endPos)), 360) + 0.5;
        if (relativeAngle < 0.) {
            relativeAngle += 360.;
        }
        lsd.manoeuverAngle = relativeAngle;

        // if p2.y is -ve the lot is on LHS of lane relative to lane direction
        // we need to know this because it inverts the complexity of the parking manoeuver
        Position p2 = this->getLane().getShape().transformToVectorCoordinates(lsd.position);
        if (p2.y() < (0. + POSITION_EPS)) {
            lsd.sideIsLHS = true;
        } else {
            lsd.sideIsLHS = false;
        }
    } else {
        lsd.endPos = myEndPos;
        lsd.manoeuverAngle = int(angle); // unused unless gModelParkingManoeuver is true
        lsd.sideIsLHS = true;
    }
    mySpaceOccupancies.push_back(lsd);
    myCapacity++;
    computeLastFreePos();
}

int
MSParkingArea::getLastFreeLotAngle() const {
    assert(myLastFreeLot >= 0);
    assert(myLastFreeLot < (int)mySpaceOccupancies.size());

    const LotSpaceDefinition& lsd = mySpaceOccupancies[myLastFreeLot];
    if (lsd.sideIsLHS) {
        return abs(int(lsd.manoeuverAngle)) % 180;
    } else {
        return abs(abs(int(lsd.manoeuverAngle)) % 180 - 180) % 180;
    }
}

double
MSParkingArea::getLastFreeLotGUIAngle() const {
    assert(myLastFreeLot >= 0);
    assert(myLastFreeLot < (int)mySpaceOccupancies.size());

    const LotSpaceDefinition& lsd = mySpaceOccupancies[myLastFreeLot];
    if (lsd.manoeuverAngle > 180.) {
        return DEG2RAD(lsd.manoeuverAngle - 360.);
    } else {
        return DEG2RAD(lsd.manoeuverAngle);
    }
}


double
MSParkingArea::getLastFreePos(const SUMOVehicle& forVehicle) const {
    if (myCapacity == (int)myEndPositions.size()) {
        // keep enough space so that  parking vehicles can leave
        return myLastFreePos - forVehicle.getVehicleType().getMinGap() - POSITION_EPS;
    } else {
        // XXX if (forVehicle.getLane() == myLane && forVehicle.getPositionOnLane() > myLastFreePos) {
        //        find freePos beyond vehicle position }
        return myLastFreePos;
    }
}

Position
MSParkingArea::getVehiclePosition(const SUMOVehicle& forVehicle) const {
    for (const auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == &forVehicle) {
            return lsd.position;
        }
    }
    return Position::INVALID;
}


double
MSParkingArea::getInsertionPosition(const SUMOVehicle& forVehicle) const {
    if (myDepartPosDefinition == DepartPosDefinition::GIVEN) {
        return myDepartPos;
    }
    for (const auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == &forVehicle) {
            return lsd.endPos;
        }
    }
    return -1;
}


double
MSParkingArea::getVehicleAngle(const SUMOVehicle& forVehicle) const {
    for (const auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == &forVehicle) {
            return (lsd.rotation - 90.) * (double) M_PI / (double) 180.0;
        }
    }
    return 0;
}

double
MSParkingArea::getVehicleSlope(const SUMOVehicle& forVehicle) const {
    for (const auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == &forVehicle) {
            return lsd.slope;
        }
    }
    return 0;
}

double
MSParkingArea::getGUIAngle(const SUMOVehicle& forVehicle) const {
    for (const auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == &forVehicle) {
            if (lsd.manoeuverAngle > 180.) {
                return DEG2RAD(lsd.manoeuverAngle - 360.);
            } else {
                return DEG2RAD(lsd.manoeuverAngle);
            }
        }
    }
    return 0.;
}

int
MSParkingArea::getManoeuverAngle(const SUMOVehicle& forVehicle) const {
    for (const auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == &forVehicle) {
            if (lsd.sideIsLHS) {
                return abs(int(lsd.manoeuverAngle)) % 180;
            } else {
                return abs(abs(int(lsd.manoeuverAngle)) % 180 - 180) % 180;
            }
        }
    }
    return 0;
}


void
MSParkingArea::enter(SUMOVehicle* veh) {
    double beg = veh->getPositionOnLane() + veh->getVehicleType().getMinGap();
    double end = veh->getPositionOnLane() - veh->getVehicleType().getLength();
    assert(myLastFreePos >= 0);
    assert(myLastFreeLot < (int)mySpaceOccupancies.size());
    if (myUpdateEvent == nullptr) {
        myUpdateEvent = new WrappingCommand<MSParkingArea>(this, &MSParkingArea::updateOccupancy);
        MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myUpdateEvent);
    }
    mySpaceOccupancies[myLastFreeLot].vehicle = veh;
    myEndPositions[veh] = std::pair<double, double>(beg, end);
    computeLastFreePos();
    // current search ends here
    veh->setNumberParkingReroutes(0);
}


void
MSParkingArea::leaveFrom(SUMOVehicle* what) {
    assert(myEndPositions.find(what) != myEndPositions.end());
    if (myUpdateEvent == nullptr) {
        myUpdateEvent = new WrappingCommand<MSParkingArea>(this, &MSParkingArea::updateOccupancy);
        MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myUpdateEvent);
    }
    for (auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == what) {
            lsd.vehicle = nullptr;
            break;
        }
    }
    myEndPositions.erase(myEndPositions.find(what));
    computeLastFreePos();
}


SUMOTime
MSParkingArea::updateOccupancy(SUMOTime /* currentTime */) {
    myLastStepOccupancy = getOccupancy();
    myUpdateEvent = nullptr;
    return 0;
}


MSParkingArea::LotSpaceDefinition::LotSpaceDefinition() :
    index(-1),
    vehicle(nullptr),
    rotation(0),
    slope(0),
    width(0),
    length(0),
    endPos(0),
    manoeuverAngle(0),
    sideIsLHS(false) {
}


MSParkingArea::LotSpaceDefinition::LotSpaceDefinition(int index_, SUMOVehicle* vehicle_, double x, double y, double z, double rotation_, double slope_, double width_, double length_) :
    index(index_),
    vehicle(vehicle_),
    position(Position(x, y, z)),
    rotation(rotation_),
    slope(slope_),
    width(width_),
    length(length_),
    endPos(0),
    manoeuverAngle(0),
    sideIsLHS(false) {
}


void
MSParkingArea::computeLastFreePos() {
    myLastFreeLot = -1;
    myLastFreePos = myBegPos;
    myEgressBlocked = false;
    for (auto& lsd : mySpaceOccupancies) {
        if (lsd.vehicle == nullptr
                || (getOccupancy() == getCapacity()
                    && lsd.vehicle->remainingStopDuration() <= 0
                    && !lsd.vehicle->isStoppedTriggered())) {
            if (lsd.vehicle == nullptr) {
                myLastFreeLot = lsd.index;
                myLastFreePos = lsd.endPos;
            } else {
                // vehicle wants to exit the parking area
                myLastFreeLot = lsd.index;
                myLastFreePos = lsd.endPos - lsd.vehicle->getVehicleType().getLength() - POSITION_EPS;
                myEgressBlocked = true;
            }
            break;
        } else {
            myLastFreePos = MIN2(myLastFreePos,
                                 lsd.endPos - lsd.vehicle->getVehicleType().getLength() - NUMERICAL_EPS);
        }
    }
}


double
MSParkingArea::getLastFreePosWithReservation(SUMOTime t, const SUMOVehicle& forVehicle) {
    if (forVehicle.getLane() != &myLane) {
        // for different lanes, do not consider reservations to avoid lane-order
        // dependency in parallel simulation
#ifdef DEBUG_RESERVATIONS
        if (DEBUG_COND2(forVehicle)) {
            std::cout << SIMTIME << " pa=" << getID() << " freePosRes veh=" << forVehicle.getID() << " other lane\n";
        }
#endif
        if (myNumAlternatives > 0 && getOccupancy() == getCapacity()) {
            // ensure that the vehicle reaches the rerouter lane
            return MAX2(myBegPos, MIN2(POSITION_EPS, myEndPos));
        } else {
            return getLastFreePos(forVehicle);
        }
    }
    if (t > myReservationTime) {
#ifdef DEBUG_RESERVATIONS
        if (DEBUG_COND2(forVehicle)) {
            std::cout << SIMTIME << " pa=" << getID() << " freePosRes veh=" << forVehicle.getID() << " first reservation\n";
        }
#endif
        myReservationTime = t;
        myReservations = 1;
        myReservationMaxLength = forVehicle.getVehicleType().getLength();
        for (const auto& lsd : mySpaceOccupancies) {
            if (lsd.vehicle != nullptr) {
                myReservationMaxLength = MAX2(myReservationMaxLength, lsd.vehicle->getVehicleType().getLength());
            }
        }
        return getLastFreePos(forVehicle);
    } else {
        if (myCapacity > getOccupancy() + myReservations) {
#ifdef DEBUG_RESERVATIONS
            if (DEBUG_COND2(forVehicle)) {
                std::cout << SIMTIME << " pa=" << getID() << " freePosRes veh=" << forVehicle.getID() << " res=" << myReservations << " enough space\n";
            }
#endif
            myReservations++;
            myReservationMaxLength = MAX2(myReservationMaxLength, forVehicle.getVehicleType().getLength());
            return getLastFreePos(forVehicle);
        } else {
            if (myCapacity == 0) {
                return getLastFreePos(forVehicle);
            } else {
#ifdef DEBUG_RESERVATIONS
                if (DEBUG_COND2(forVehicle)) std::cout << SIMTIME << " pa=" << getID() << " freePosRes veh=" << forVehicle.getID()
                                                           << " res=" << myReservations << " resTime=" << myReservationTime << " reserved full, maxLen=" << myReservationMaxLength << " endPos=" << mySpaceOccupancies[0].myEndPos << "\n";
#endif
                return (mySpaceOccupancies[0].endPos
                        - myReservationMaxLength
                        - forVehicle.getVehicleType().getMinGap()
                        - NUMERICAL_EPS);
            }
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


bool
MSParkingArea::parkOnRoad() const {
    return myOnRoad;
}


int
MSParkingArea::getOccupancy() const {
    return (int)myEndPositions.size() - (myEgressBlocked ? 1 : 0);
}


int
MSParkingArea::getOccupancyIncludingBlocked() const {
    return (int)myEndPositions.size();
}


int
MSParkingArea::getLastStepOccupancy() const {
    return myLastStepOccupancy;
}

void
MSParkingArea::notifyEgressBlocked() {
    computeLastFreePos();
}


int
MSParkingArea::getNumAlternatives() const {
    return myNumAlternatives;
}


void
MSParkingArea::setNumAlternatives(int alternatives) {
    myNumAlternatives = MAX2(myNumAlternatives, alternatives);
}

/****************************************************************************/
