/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSOverheadWire.cpp
/// @author  Jakub Sevcik (RICE)
/// @author  Jan Prikryl (RICE)
/// @date    2019-12-15
///
// Member method definitions for MSOverheadWire and MSTractionSubstation.
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <tuple>
#include <mutex>
#include <string.h>

#include <utils/vehicle/SUMOVehicle.h>
#include <utils/common/ToString.h>
#include <microsim/MSVehicleType.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/MSJunction.h>
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSNet.h>
#include <microsim/devices/MSDevice_ElecHybrid.h>

// due to gOverheadWireSolver
#include <microsim/MSGlobals.h>

// due to solving circuit as endEndOfTimestepEvents
#include <utils/common/StaticCommand.h>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSEventControl.h>

#include <utils/traction_wire/Node.h>
#include "MSOverheadWire.h"


Command* MSTractionSubstation::myCommandForSolvingCircuit = nullptr;
static std::mutex ow_mutex;

// ===========================================================================
//                                                              MSOverheadWire
// ===========================================================================

MSOverheadWire::MSOverheadWire(const std::string& overheadWireSegmentID, MSLane& lane, double startPos, double endPos, bool voltageSource) :
    MSStoppingPlace(overheadWireSegmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT, std::vector<std::string>(), lane, startPos, endPos),
    myVoltage(0),
    myChargingVehicle(false),
    myTotalCharge(0),
    myChargingVehicles({}),
                   // RICE_TODO: think about some better structure storing circuit pointers below
                   myTractionSubstation(nullptr),
                   myVoltageSource(voltageSource),
                   myCircuitElementPos(nullptr),
                   myCircuitStartNodePos(nullptr),
myCircuitEndNodePos(nullptr) {
    if (getBeginLanePosition() > getEndLanePosition()) {
        WRITE_WARNING(toString(SUMO_TAG_OVERHEAD_WIRE_SEGMENT) + " with ID = " + getID() + " doesn't have a valid range (" + toString(getBeginLanePosition()) + " < " + toString(getEndLanePosition()) + ").");
    }
}

MSOverheadWire::~MSOverheadWire() {
    if (myTractionSubstation != nullptr) {
        Circuit* circuit = myTractionSubstation->getCircuit();
        if (circuit != nullptr && myCircuitElementPos != nullptr && myCircuitElementPos->getPosNode() == myCircuitStartNodePos && myCircuitElementPos->getNegNode() == myCircuitEndNodePos) {
            circuit->eraseElement(myCircuitElementPos);
            delete myCircuitElementPos;
            if (myCircuitEndNodePos->getElements()->size() == 0) {
                circuit->eraseNode(myCircuitEndNodePos);
                delete myCircuitEndNodePos;
            }
            if (myCircuitStartNodePos->getElements()->size() == 0) {
                circuit->eraseNode(myCircuitStartNodePos);
                delete myCircuitStartNodePos;
            }
        }

        if (myTractionSubstation->numberOfOverheadSegments() <= 1) {
            myTractionSubstation->eraseOverheadWireSegmentFromCircuit(this);
            //RICE_TODO We should "delete myTractionSubstation;" here ...
        } else {
            myTractionSubstation->eraseOverheadWireSegmentFromCircuit(this);
        }
    }
}


void
MSOverheadWire::addVehicle(SUMOVehicle& veh) {
    std::lock_guard<std::mutex> guard(ow_mutex);
    setChargingVehicle(true);
    myChargingVehicles.push_back(&veh);
    sort(myChargingVehicles.begin(), myChargingVehicles.end(), vehicle_position_sorter());
}

void
MSOverheadWire::eraseVehicle(SUMOVehicle& veh) {
    std::lock_guard<std::mutex> guard(ow_mutex);
    myChargingVehicles.erase(std::remove(myChargingVehicles.begin(), myChargingVehicles.end(), &veh), myChargingVehicles.end());
    if (myChargingVehicles.size() == 0) {
        setChargingVehicle(false);
    }
    //sort(myChargingVehicles.begin(), myChargingVehicles.end(), vehicle_position_sorter());
}

void
MSOverheadWire::lock() const {
    ow_mutex.lock();
}

void
MSOverheadWire::unlock() const {
    ow_mutex.unlock();
}

void
MSTractionSubstation::addVehicle(MSDevice_ElecHybrid* elecHybrid) {
    myElecHybrid.push_back(elecHybrid);
}

void
MSTractionSubstation::eraseVehicle(MSDevice_ElecHybrid* veh) {
    myElecHybrid.erase(std::remove(myElecHybrid.begin(), myElecHybrid.end(), veh), myElecHybrid.end());
}

void
MSTractionSubstation::writeOut() {
    std::cout << "substation " << getID() << " constrols segments: \n";
    for (std::vector<MSOverheadWire*>::iterator it = myOverheadWireSegments.begin(); it != myOverheadWireSegments.end(); ++it) {
        std::cout << "        " << (*it)->getOverheadWireSegmentName() << "\n";
    }
}


std::string MSOverheadWire::getOverheadWireSegmentName() {
    return toString(getID());
}

MSTractionSubstation::~MSTractionSubstation() {
}

Circuit*
MSOverheadWire::getCircuit() const {
    if (getTractionSubstation() != nullptr) {
        return getTractionSubstation()->getCircuit();
    }
    return nullptr;
}

double
MSOverheadWire::getVoltage() const {
    return myVoltage;
}

void
MSOverheadWire::setVoltage(double voltage) {
    if (voltage < 0) {
        WRITE_WARNING("New " + toString(SUMO_ATTR_VOLTAGE) + " for " + toString(SUMO_TAG_OVERHEAD_WIRE_SEGMENT) + " with ID = " + getID() + " isn't valid (" + toString(voltage) + ").")
    } else {
        myVoltage = voltage;
    }
}

void
MSOverheadWire::setChargingVehicle(bool value) {
    myChargingVehicle = value;
}


void
MSTractionSubstation::setChargingVehicle(bool value) {
    myChargingVehicle = value;
}

bool
MSOverheadWire::vehicleIsInside(const double position) const {
    if ((position >= getBeginLanePosition()) && (position <= getEndLanePosition())) {
        return true;
    } else {
        return false;
    }
}


bool
MSOverheadWire::isCharging() const {
    return myChargingVehicle;
}


void
MSOverheadWire::addChargeValueForOutput(double WCharged, MSDevice_ElecHybrid* elecHybrid, bool ischarging) {
    std::string status = "charging";
    if (!ischarging) {
        status = "not-charging";
    }

    // update total charge
    myTotalCharge += WCharged;
    // create charge row and insert it in myChargeValues
    const std::string vehID = elecHybrid->getHolder().getID();
    if (myChargeValues.count(vehID) == 0) {
        myChargedVehicles.push_back(vehID);
    }
    Charge C(MSNet::getInstance()->getCurrentTimeStep(), elecHybrid->getHolder().getID(), elecHybrid->getHolder().getVehicleType().getID(),
             status, WCharged, elecHybrid->getActualBatteryCapacity(), elecHybrid->getMaximumBatteryCapacity(),
             elecHybrid->getVoltageOfOverheadWire(), myTotalCharge);
    myChargeValues[vehID].push_back(C);
}


void
MSOverheadWire::writeOverheadWireSegmentOutput(OutputDevice& output) {
    int chargingSteps = 0;
    std::vector<SUMOTime> chargingSteps_list;
    for (const auto& item : myChargeValues) {
        for (auto it : item.second) {
            if (std::find(chargingSteps_list.begin(), chargingSteps_list.end(), it.timeStep) == chargingSteps_list.end()) {
                chargingSteps_list.push_back(it.timeStep);
            }
        }
    }
    chargingSteps = (int) chargingSteps_list.size();
    output.openTag(SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
    output.writeAttr(SUMO_ATTR_ID, myID);
    if (getTractionSubstation() != nullptr) {
        output.writeAttr(SUMO_ATTR_TRACTIONSUBSTATIONID, getTractionSubstation()->getID());
    } else {
        output.writeAttr(SUMO_ATTR_TRACTIONSUBSTATIONID, "");
    }
    output.writeAttr(SUMO_ATTR_TOTALENERGYCHARGED, myTotalCharge);

    // RICE_TODO QUESTION myChargeValues.size() vs. chargingSteps
    // myChargeValues.size() is the number of vehicles charging sometimes from this overheadwire segment during simulation
    // chargingSteps is now the sum of chargingSteps of each vehicle, but takes also into account that at the given
    // step more than one vehicle may be charged from this segment
    output.writeAttr(SUMO_ATTR_CHARGINGSTEPS, chargingSteps);
    // output.writeAttr(SUMO_ATTR_EDGE, getLane().getEdge());
    output.writeAttr(SUMO_ATTR_LANE, getLane().getID());

    // Start writing
    if (myChargeValues.size() > 0) {
        for (const std::string& vehID : myChargedVehicles) {
            int iStart = 0;
            const auto& chargeSteps = myChargeValues[vehID];
            while (iStart < (int)chargeSteps.size()) {
                int iEnd = iStart + 1;
                double charged = chargeSteps[iStart].WCharged;
                while (iEnd < (int)chargeSteps.size() && chargeSteps[iEnd].timeStep == chargeSteps[iEnd - 1].timeStep + DELTA_T) {
                    charged += chargeSteps[iEnd].WCharged;
                    iEnd++;
                }
                writeVehicle(output, chargeSteps, iStart, iEnd, charged);
                iStart = iEnd;
            }
        }
    }
    // close charging station tag
    output.closeTag();
}


void
MSOverheadWire::writeVehicle(OutputDevice& out, const std::vector<Charge>& chargeSteps, int iStart, int iEnd, double charged) {
    const Charge& first = chargeSteps[iStart];
    out.openTag(SUMO_TAG_VEHICLE);
    out.writeAttr(SUMO_ATTR_ID, first.vehicleID);
    out.writeAttr(SUMO_ATTR_TYPE, first.vehicleType);
    out.writeAttr(SUMO_ATTR_TOTALENERGYCHARGED_VEHICLE, charged);
    out.writeAttr(SUMO_ATTR_CHARGINGBEGIN, time2string(first.timeStep));
    out.writeAttr(SUMO_ATTR_CHARGINGEND, time2string(chargeSteps[iEnd - 1].timeStep));
    out.writeAttr(SUMO_ATTR_MAXIMUMBATTERYCAPACITY, first.maxBatteryCapacity);
    for (int i = iStart; i < iEnd; i++) {
        const Charge& c = chargeSteps[i];
        out.openTag(SUMO_TAG_STEP);
        out.writeAttr(SUMO_ATTR_TIME, time2string(c.timeStep));
        // charge values
        out.writeAttr(SUMO_ATTR_CHARGING_STATUS, c.status);
        out.writeAttr(SUMO_ATTR_ENERGYCHARGED, c.WCharged);
        out.writeAttr(SUMO_ATTR_PARTIALCHARGE, c.totalEnergyCharged);
        // charging values of charging station in this timestep
        out.writeAttr(SUMO_ATTR_VOLTAGE, c.voltage);
        // battery status of vehicle
        out.writeAttr(SUMO_ATTR_ACTUALBATTERYCAPACITY, c.actualBatteryCapacity);
        // close tag timestep
        out.closeTag();
    }
    out.closeTag();
}


// ===========================================================================
//                                                        MSTractionSubstation
// ===========================================================================
// RICE_TODO Split MSTractionSubstation and MSOverheadWire?
// Probably no as the traction substation cannot stand alone and is always
// used together with the overhead wire. It is a bit disorganised, though.

MSTractionSubstation::MSTractionSubstation(const std::string& substationId, double voltage, double currentLimit) :
    Named(substationId),
    myChargingVehicle(false),
    myElecHybridCount(0),
    mySubstationVoltage(voltage),
    myCircuit(new Circuit(currentLimit)),
    myTotalEnergy(0)
{}



void
MSTractionSubstation::addOverheadWireSegmentToCircuit(MSOverheadWire* newOverheadWireSegment) {
    MSLane& lane = const_cast<MSLane&>(newOverheadWireSegment->getLane());
    if (lane.isInternal()) {
        return;
    }

    // RICE_TODO: consider the possibility of having more segments that belong to one lane.

    myOverheadWireSegments.push_back(newOverheadWireSegment);
    newOverheadWireSegment->setTractionSubstation(this);

    if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
        Circuit* circuit = newOverheadWireSegment->getCircuit();
        const std::string segmentID = newOverheadWireSegment->getID();

        if (circuit->getNode("negNode_ground") == nullptr) {
            circuit->addNode("negNode_ground");
        }

        // convention: pNode is at the beginning of the wire segment, nNode is at the end of the wire segment
        newOverheadWireSegment->setCircuitStartNodePos(circuit->addNode("pNode_pos_" + segmentID));
        newOverheadWireSegment->setCircuitEndNodePos(circuit->addNode("nNode_pos_" + segmentID));
        // RICE_TODO: to use startPos and endPos of ovhdsegment: set the length of wire here properly
        newOverheadWireSegment->setCircuitElementPos(
            circuit->addElement("pos_" + segmentID,
                                (newOverheadWireSegment->getLane().getLength()) * WIRE_RESISTIVITY,
                                newOverheadWireSegment->getCircuitStartNodePos(),
                                newOverheadWireSegment->getCircuitEndNodePos(),
                                Element::ElementType::RESISTOR_traction_wire));
#else
        WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
    }

    const MSLane* connection = nullptr;
    std::string ovrhdSegmentID = ""; //ID of outgoing or incoming overhead wire segment
    MSOverheadWire* ovrhdSegment = nullptr; //pointer to outgoing or incoming overhead wire segment

    // RICE_TODO: simplify the code, two similar code-blocks below
    // RICE_TODO: to use startPos and endPos of ovhdsegment: if endPos+EPS > newOverheadWireSegment->getLane().getLength(),
    //            and the outgoing lanes will be skipped as there is no wire at the end of the lane

    /* in version before SUMO 1.0.1 the function getOutgoingLanes() returning MSLane* exists,
       in new version of SUMO the funciton getOutgoingViaLanes() returning MSLane* and MSEdge* pair exists */
    // std::vector<const MSLane*> outgoing = lane.getOutgoingLanes();
    const std::vector<std::pair<const MSLane*, const MSEdge*> > outgoingLanesAndEdges = lane.getOutgoingViaLanes();
    std::vector<const MSLane*> neigboringInnerLanes;
    neigboringInnerLanes.reserve(outgoingLanesAndEdges.size());
    for (size_t it = 0; it < outgoingLanesAndEdges.size(); ++it) {
        neigboringInnerLanes.push_back(outgoingLanesAndEdges[it].first);
    }

    // Check if there is an overhead wire segment on the outgoing lane. If not, do nothing, otherwise find connnecting internal lanes and
    // add all lanes (this and inner) to circuit
    for (std::vector<const MSLane*>::iterator it = neigboringInnerLanes.begin(); it != neigboringInnerLanes.end(); ++it) {
        ovrhdSegmentID = MSNet::getInstance()->getStoppingPlaceID(*it, NUMERICAL_EPS, SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
        // If the overhead wire segment is over the outgoing (not internal) lane
        if (ovrhdSegmentID != "" && !(*it)->isInternal()) {
            ovrhdSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(ovrhdSegmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
            // If the outgoing overhead wire segment belongs to the same substation as newOverheadWireSegment
            // RICE_TODO: define what happens if the traction stations are different (overhead wire should continue over inner segments but it is unclear to which traction substation or even circuit it should be connected)
            if (ovrhdSegment->getTractionSubstation() == newOverheadWireSegment->getTractionSubstation()) {
                connection = lane.getInternalFollowingLane(*it);
                if (connection != nullptr) {
                    //is connection a forbidden lane?
                    if (!(ovrhdSegment->getTractionSubstation()->isForbidden(connection) ||
                            ovrhdSegment->getTractionSubstation()->isForbidden(lane.getInternalFollowingLane(connection)) ||
                            ovrhdSegment->getTractionSubstation()->isForbidden(connection->getInternalFollowingLane(*it)))) {
                        addOverheadWireInnerSegmentToCircuit(newOverheadWireSegment, ovrhdSegment, connection, lane.getInternalFollowingLane(connection), connection->getInternalFollowingLane(*it));
                    }

                } else {
                    if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
                        Node* const unusedNode = newOverheadWireSegment->getCircuitEndNodePos();
                        for (MSOverheadWire* const ows : myOverheadWireSegments) {
                            if (ows->getCircuitStartNodePos() == unusedNode) {
                                ows->setCircuitStartNodePos(ovrhdSegment->getCircuitStartNodePos());
                            }
                            if (ows->getCircuitEndNodePos() == unusedNode) {
                                ows->setCircuitEndNodePos(ovrhdSegment->getCircuitStartNodePos());
                            }
                        }
                        newOverheadWireSegment->getCircuit()->replaceAndDeleteNode(unusedNode, ovrhdSegment->getCircuitStartNodePos());
#else
                        WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
                    }
                }
            }
        }
    }

    // RICE_TODO: to use startPos and endPos of ovhdsegment: if startPos-EPS < 0,
    //            and the incoming lanes will be skipped as there is no wire at the beginning of the lane

    // This is the same as above, only this time checking the wires on some incoming lanes. If some of them
    // has an overhead wire segment, find the connnecting internal lanes and add all lanes (the internal
    // and this) to the circuit, otherwise do nothing.
    neigboringInnerLanes = lane.getNormalIncomingLanes();
    for (std::vector<const MSLane*>::iterator it = neigboringInnerLanes.begin(); it != neigboringInnerLanes.end(); ++it) {
        ovrhdSegmentID = MSNet::getInstance()->getStoppingPlaceID(*it, (*it)->getLength() - NUMERICAL_EPS, SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
        // If the overhead wire segment is over the incoming (not internal) lane
        if (ovrhdSegmentID != "" && !(*it)->isInternal()) {
            ovrhdSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(ovrhdSegmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
            // If the incoming overhead wire segment belongs to the same substation as newOverheadWireSegment
            // RICE_TODO: define what happens if the traction stations are different (overhead wire should continue over inner segments but it is unclear to which traction substation or even circuit it should be connected)
            if (ovrhdSegment->getTractionSubstation() == newOverheadWireSegment->getTractionSubstation()) {
                connection = (*it)->getInternalFollowingLane(&lane);
                if (connection != nullptr) {
                    //is connection a forbidden lane?
                    if (!(ovrhdSegment->getTractionSubstation()->isForbidden(connection) ||
                            ovrhdSegment->getTractionSubstation()->isForbidden((*it)->getInternalFollowingLane(connection)) ||
                            ovrhdSegment->getTractionSubstation()->isForbidden(connection->getInternalFollowingLane(&lane)))) {
                        addOverheadWireInnerSegmentToCircuit(ovrhdSegment, newOverheadWireSegment, connection, (*it)->getInternalFollowingLane(connection), connection->getInternalFollowingLane(&lane));
                    }
                } else {
                    if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
                        Node* const unusedNode = newOverheadWireSegment->getCircuitStartNodePos();
                        for (MSOverheadWire* const ows : myOverheadWireSegments) {
                            if (ows->getCircuitStartNodePos() == unusedNode) {
                                ows->setCircuitStartNodePos(ovrhdSegment->getCircuitEndNodePos());
                            }
                            if (ows->getCircuitEndNodePos() == unusedNode) {
                                ows->setCircuitEndNodePos(ovrhdSegment->getCircuitEndNodePos());
                            }
                        }
                        newOverheadWireSegment->getCircuit()->replaceAndDeleteNode(unusedNode, ovrhdSegment->getCircuitEndNodePos());
#else
                        WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
                    }
                }
            }
        }
    }

    if (MSGlobals::gOverheadWireSolver && newOverheadWireSegment->isThereVoltageSource()) {
#ifdef HAVE_EIGEN
        newOverheadWireSegment->getCircuit()->addElement(
            "voltage_source_" + newOverheadWireSegment->getID(),
            mySubstationVoltage,
            newOverheadWireSegment->getCircuitStartNodePos(),
            newOverheadWireSegment->getCircuit()->getNode("negNode_ground"),
            Element::ElementType::VOLTAGE_SOURCE_traction_wire);
#else
        WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
    }
}


void
MSTractionSubstation::addOverheadWireInnerSegmentToCircuit(MSOverheadWire* incomingSegment, MSOverheadWire* outgoingSegment, const MSLane* connection, const MSLane* frontConnection, const MSLane* behindConnection) {
    if (frontConnection == nullptr && behindConnection == nullptr) {
        // addOverheadWire from nNode of newOverheadWireSegment to pNode
        MSOverheadWire* innerSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + connection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        myOverheadWireSegments.push_back(innerSegment);
        innerSegment->setTractionSubstation(incomingSegment->getTractionSubstation());
        if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
            Element* elem = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + connection->getID(), (connection->getLength()) * WIRE_RESISTIVITY, incomingSegment->getCircuitEndNodePos(), outgoingSegment->getCircuitStartNodePos(), Element::ElementType::RESISTOR_traction_wire);
            innerSegment->setCircuitElementPos(elem);
            innerSegment->setCircuitStartNodePos(incomingSegment->getCircuitEndNodePos());
            innerSegment->setCircuitEndNodePos(outgoingSegment->getCircuitStartNodePos());
#else
            UNUSED_PARAMETER(outgoingSegment);
            WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
        }
    } else if (frontConnection != nullptr && behindConnection == nullptr) {
        MSOverheadWire* innerSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + frontConnection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        MSOverheadWire* innerSegment2 = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + connection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));

        innerSegment->setTractionSubstation(incomingSegment->getTractionSubstation());
        myOverheadWireSegments.push_back(innerSegment);
        innerSegment2->setTractionSubstation(incomingSegment->getTractionSubstation());
        myOverheadWireSegments.push_back(innerSegment2);

        if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
            Node* betweenFrontNode_pos = incomingSegment->getCircuit()->addNode("betweenFrontNode_pos_" + connection->getID());
            Element* elem = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + frontConnection->getID(), (frontConnection->getLength()) * WIRE_RESISTIVITY, incomingSegment->getCircuitEndNodePos(), betweenFrontNode_pos, Element::ElementType::RESISTOR_traction_wire);
            Element* elem2 = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + connection->getID(), (connection->getLength()) * WIRE_RESISTIVITY, betweenFrontNode_pos, outgoingSegment->getCircuitStartNodePos(), Element::ElementType::RESISTOR_traction_wire);

            innerSegment->setCircuitElementPos(elem);
            innerSegment->setCircuitStartNodePos(incomingSegment->getCircuitEndNodePos());
            innerSegment->setCircuitEndNodePos(betweenFrontNode_pos);

            innerSegment2->setCircuitElementPos(elem2);
            innerSegment2->setCircuitStartNodePos(betweenFrontNode_pos);
            innerSegment2->setCircuitEndNodePos(outgoingSegment->getCircuitStartNodePos());
#else
            WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
        }
    } else if (frontConnection == nullptr && behindConnection != nullptr) {
        MSOverheadWire* innerSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + connection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        MSOverheadWire* innerSegment2 = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + behindConnection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));

        innerSegment->setTractionSubstation(incomingSegment->getTractionSubstation());
        myOverheadWireSegments.push_back(innerSegment);
        innerSegment2->setTractionSubstation(incomingSegment->getTractionSubstation());
        myOverheadWireSegments.push_back(innerSegment2);

        if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
            Node* betweenBehindNode_pos = incomingSegment->getCircuit()->addNode("betweenBehindNode_pos_" + connection->getID());
            Element* elem = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + connection->getID(), (connection->getLength()) * WIRE_RESISTIVITY, incomingSegment->getCircuitEndNodePos(), betweenBehindNode_pos, Element::ElementType::RESISTOR_traction_wire);
            Element* elem2 = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + behindConnection->getID(), (behindConnection->getLength()) * WIRE_RESISTIVITY, betweenBehindNode_pos, outgoingSegment->getCircuitStartNodePos(), Element::ElementType::RESISTOR_traction_wire);

            innerSegment->setCircuitElementPos(elem);
            innerSegment->setCircuitStartNodePos(incomingSegment->getCircuitEndNodePos());
            innerSegment->setCircuitEndNodePos(betweenBehindNode_pos);

            innerSegment2->setCircuitElementPos(elem2);
            innerSegment2->setCircuitStartNodePos(betweenBehindNode_pos);
            innerSegment2->setCircuitEndNodePos(outgoingSegment->getCircuitStartNodePos());
#else
            WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
        }
    } else if (frontConnection != nullptr && behindConnection != nullptr) {
        MSOverheadWire* innerSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + frontConnection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        MSOverheadWire* innerSegment2 = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + connection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        MSOverheadWire* innerSegment3 = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace("ovrhd_inner_" + behindConnection->getID(), SUMO_TAG_OVERHEAD_WIRE_SEGMENT));

        innerSegment->setTractionSubstation(incomingSegment->getTractionSubstation());
        myOverheadWireSegments.push_back(innerSegment);
        innerSegment2->setTractionSubstation(incomingSegment->getTractionSubstation());
        myOverheadWireSegments.push_back(innerSegment2);
        innerSegment3->setTractionSubstation(incomingSegment->getTractionSubstation());
        myOverheadWireSegments.push_back(innerSegment3);

        if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
            Node* betweenFrontNode_pos = incomingSegment->getCircuit()->addNode("betweenFrontNode_pos_" + connection->getID());
            Node* betweenBehindNode_pos = incomingSegment->getCircuit()->addNode("betweenBehindNode_pos_" + connection->getID());
            Element* elem = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + frontConnection->getID(), (frontConnection->getLength()) * WIRE_RESISTIVITY, incomingSegment->getCircuitEndNodePos(), betweenFrontNode_pos, Element::ElementType::RESISTOR_traction_wire);
            Element* elem2 = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + connection->getID(), (connection->getLength()) * WIRE_RESISTIVITY, betweenFrontNode_pos, betweenBehindNode_pos, Element::ElementType::RESISTOR_traction_wire);
            Element* elem3 = incomingSegment->getCircuit()->addElement("pos_ovrhd_inner_" + behindConnection->getID(), (behindConnection->getLength()) * WIRE_RESISTIVITY, betweenBehindNode_pos, outgoingSegment->getCircuitStartNodePos(), Element::ElementType::RESISTOR_traction_wire);

            innerSegment->setCircuitElementPos(elem);
            innerSegment->setCircuitStartNodePos(incomingSegment->getCircuitEndNodePos());
            innerSegment->setCircuitEndNodePos(betweenFrontNode_pos);

            innerSegment2->setCircuitElementPos(elem2);
            innerSegment2->setCircuitStartNodePos(betweenFrontNode_pos);
            innerSegment2->setCircuitEndNodePos(betweenBehindNode_pos);

            innerSegment3->setCircuitElementPos(elem3);
            innerSegment3->setCircuitStartNodePos(betweenBehindNode_pos);
            innerSegment3->setCircuitEndNodePos(outgoingSegment->getCircuitStartNodePos());
#else
            WRITE_WARNING(TL("Overhead circuit solver requested, but solver support not compiled in."));
#endif
        }
    }
}


void MSTractionSubstation::addOverheadWireClampToCircuit(const std::string id, MSOverheadWire* startSegment, MSOverheadWire* endSegment) {
    PositionVector pos_start = startSegment->getLane().getShape();
    PositionVector pos_end = endSegment->getLane().getShape();
    double distance = pos_start[0].distanceTo2D(pos_end.back());

    if (distance > 10) {
        WRITE_WARNING("The distance between two overhead wires during adding overhead wire clamp '" + id + "' defined for traction substation '" + startSegment->getTractionSubstation()->getID() + "' is " + toString(distance) + " m.")
    }
    getCircuit()->addElement(id, distance * WIRE_RESISTIVITY, startSegment->getCircuitStartNodePos(), endSegment->getCircuitEndNodePos(), Element::ElementType::RESISTOR_traction_wire);
}


void
MSTractionSubstation::eraseOverheadWireSegmentFromCircuit(MSOverheadWire* oldSegment) {
    //myOverheadWireSegments.push_back(static_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(overheadWireSegmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT)));
    myOverheadWireSegments.erase(std::remove(myOverheadWireSegments.begin(), myOverheadWireSegments.end(), oldSegment), myOverheadWireSegments.end());
}


bool
MSTractionSubstation::isCharging() const {
    return myChargingVehicle;
}


void
MSTractionSubstation::increaseElecHybridCount() {
    myElecHybridCount++;
}


void
MSTractionSubstation::decreaseElecHybridCount() {
    myElecHybridCount--;
}


void MSTractionSubstation::addForbiddenLane(MSLane* lane) {
    myForbiddenLanes.push_back(lane);
}


bool MSTractionSubstation::isForbidden(const MSLane* lane) {
    for (std::vector<MSLane*>::iterator it = myForbiddenLanes.begin(); it != myForbiddenLanes.end(); ++it) {
        if (lane == (*it)) {
            return true;
        }
    }
    return false;
}


void
MSTractionSubstation::addClamp(const std::string& id, MSOverheadWire* startPos, MSOverheadWire* endPos) {
    OverheadWireClamp clamp(id, startPos, endPos, false);
    myOverheadWireClamps.push_back(clamp);
}


MSTractionSubstation::OverheadWireClamp*
MSTractionSubstation::findClamp(std::string clampId) {
    for (auto it = myOverheadWireClamps.begin(); it != myOverheadWireClamps.end(); it++) {
        if (it->id == clampId) {
            return &(*it);
        }
    }
    return nullptr;
}


bool
MSTractionSubstation::isAnySectionPreviouslyDefined() {
    if (myOverheadWireSegments.size() > 0 || myForbiddenLanes.size() > 0 || getCircuit()->getLastId() > 0) {
        return true;
    }
    return false;
}


void
MSTractionSubstation::addSolvingCirucitToEndOfTimestepEvents() {
    if (!myChargingVehicle) {
        // myCommandForSolvingCircuit = new StaticCommand<MSTractionSubstation>(&MSTractionSubstation::solveCircuit);
        myCommandForSolvingCircuit = new WrappingCommand<MSTractionSubstation>(this, &MSTractionSubstation::solveCircuit);
        MSNet::getInstance()->getEndOfTimestepEvents()->addEvent(myCommandForSolvingCircuit);
        setChargingVehicle(true);
    }
}


SUMOTime
MSTractionSubstation::solveCircuit(SUMOTime /*currentTime*/) {
    /*Circuit evaluation*/
    setChargingVehicle(false);

#ifdef HAVE_EIGEN

    // RICE_TODO: Allow for updating current limits in each time step if changed e.g. via traci or similar
    // getCircuit()->setCurrentLimit(myCurrentLimit);

    // Solve the electrical circuit
    myCircuit->solve();

    if (myCircuit->getAlphaBest() != 1.0) {
        WRITE_WARNINGF(TL("The requested total power could not be delivered by the overhead wire. Only % of originally requested power was provided."), toString(myCircuit->getAlphaBest()));
    }
#endif

    // RICE_TODO: verify what happens if eigen is not defined?
    // Note: addSolvingCirucitToEndOfTimestepEvents() and thus solveCircuit() should be called from notifyMove only if eigen is defined.
    addChargeValueForOutput(WATT2WATTHR(myCircuit->getTotalPowerOfCircuitSources()), myCircuit->getTotalCurrentOfCircuitSources(), myCircuit->getAlphaBest(), myCircuit->getAlphaReason());

    for (auto* it : myElecHybrid) {

        Element* vehElem = it->getVehElem();
        double voltage = vehElem->getVoltage();
        double current = -vehElem->getCurrent();  // Vehicle is a power source, hence its current (returned by getCurrent()) flows in opposite direction

        it->setCurrentFromOverheadWire(current);
        it->setVoltageOfOverheadWire(voltage);

        // Calulate energy charged
        double energyIn = WATT2WATTHR(voltage * current);  // [Wh]

        // Compute energy charged into/from battery considering recuperation and propulsion efficiency (not considering battery capacity)
        double energyCharged = it->computeChargedEnergy(energyIn);

        // Update energy saved in the battery pack and return trully charged energy considering limits of battery
        double realEnergyCharged = it->storeEnergyToBattery(energyCharged);

        it->setEnergyCharged(realEnergyCharged);

        // Add energy wasted to the total sum
        it->updateTotalEnergyWasted(energyCharged - realEnergyCharged);
        // Add the energy provided by the overhead wire segment to the output of the segment
        it->getActOverheadWireSegment()->addChargeValueForOutput(energyIn, it);
    }

    return 0;
}

void
MSTractionSubstation::addChargeValueForOutput(double energy, double current, double alpha, Circuit::alphaFlag alphaReason) {
    std::string status = "";

    myTotalEnergy += energy; //[Wh]

    std::string vehicleIDs = "";
    for (std::vector<MSDevice_ElecHybrid*>::iterator it = myElecHybrid.begin(); it != myElecHybrid.end(); it++) {
        vehicleIDs += (*it)->getID() + " ";
    }
    //vehicleIDs.erase(vehicleIDs.end());
    // TODO vehicleIDs should not be empty, but in some case, it is (due to teleporting of vehicle?)
    if (!vehicleIDs.empty()) {
        vehicleIDs.pop_back();
    }

    std::string currents = "";
    currents = myCircuit->getCurrentsOfCircuitSource(currents);

    // create charge row and insert it in myChargeValues
    chargeTS C(MSNet::getInstance()->getCurrentTimeStep(), getID(), vehicleIDs, energy, current, currents, mySubstationVoltage, status,
               (int)myElecHybrid.size(), (int)getCircuit()->getNumVoltageSources(), alpha, alphaReason);
    myChargeValues.push_back(C);
}

void
MSTractionSubstation::writeTractionSubstationOutput(OutputDevice& output) {
    output.openTag(SUMO_TAG_TRACTION_SUBSTATION);
    output.writeAttr(SUMO_ATTR_ID, myID);
    output.writeAttr(SUMO_ATTR_TOTALENERGYCHARGED, myTotalEnergy); //[Wh]
    double length = 0;
    for (auto it = myOverheadWireSegments.begin(); it != myOverheadWireSegments.end(); it++) {
        length += (*it)->getEndLanePosition() - (*it)->getBeginLanePosition();
    }
    output.writeAttr(SUMO_ATTR_LENGTH, length);
    output.writeAttr("numVoltageSources", myCircuit->getNumVoltageSources());
    output.writeAttr("numClamps", myOverheadWireClamps.size());
    output.writeAttr(SUMO_ATTR_CHARGINGSTEPS, myChargeValues.size());

    // start writting
    if (myChargeValues.size() > 0) {
        // iterate over charging values
        for (std::vector<MSTractionSubstation::chargeTS>::const_iterator i = myChargeValues.begin(); i != myChargeValues.end(); i++) {
            // open tag for timestep and write all parameters
            output.openTag(SUMO_TAG_STEP);
            output.writeAttr(SUMO_ATTR_TIME, time2string(i->timeStep));
            // charge values
            output.writeAttr("vehicleIDs", i->vehicleIDs);
            output.writeAttr("numVehicles", i->numVehicles);
            // same number of numVoltageSources for all time, parameter is written in the superordinate tag
            //output.writeAttr("numVoltageSources", i->numVoltageSources);
            // charging status is always ""
            //output.writeAttr(SUMO_ATTR_CHARGING_STATUS, i->status);
            output.writeAttr(SUMO_ATTR_ENERGYCHARGED, i->energy);
            output.writeAttr(SUMO_ATTR_CURRENTFROMOVERHEADWIRE, i->current);
            output.writeAttr("currents", i->currentsString);
            // charging values of charging station in this timestep
            output.writeAttr(SUMO_ATTR_VOLTAGE, i->voltage);
            output.writeAttr(SUMO_ATTR_ALPHACIRCUITSOLVER, i->alpha);
            output.writeAttr("alphaFlag", i->alphaReason);
            // close tag timestep
            output.closeTag();
            // update timestep of charge
        }
    }
    // close charging station tag
    output.closeTag();
}

/****************************************************************************/
