/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_Bluelight.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Laura Bieker
/// @date    01.06.2017
///
// A device for emergency vehicle. The behaviour of other traffic participants will be triggered with this device.
// For example building a rescue lane.
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/vehicle/SUMOVehicle.h>
#include <microsim/MSNet.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSLink.h>
#include <microsim/MSVehicle.h>
#include <microsim/lcmodels/MSAbstractLaneChangeModel.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleType.h>
#include "MSDevice_Tripinfo.h"
#include "MSDevice_Bluelight.h"

//#define DEBUG_BLUELIGHT
//#define DEBUG_BLUELIGHT_RESCUELANE

#define INFLUENCED_BY "rescueLane"

// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// static initialisation methods
// ---------------------------------------------------------------------------
void
MSDevice_Bluelight::insertOptions(OptionsCont& oc) {
    oc.addOptionSubTopic("Bluelight Device");
    insertDefaultAssignmentOptions("bluelight", "Bluelight Device", oc);

    oc.doRegister("device.bluelight.reactiondist", new Option_Float(25.0));
    oc.addDescription("device.bluelight.reactiondist", "Bluelight Device", TL("Set the distance at which other drivers react to the blue light and siren sound"));
    oc.doRegister("device.bluelight.mingapfactor", new Option_Float(1.));
    oc.addDescription("device.bluelight.mingapfactor", "Bluelight Device", TL("Reduce the minGap for reacting vehicles by the given factor"));
}


void
MSDevice_Bluelight::buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into) {
    OptionsCont& oc = OptionsCont::getOptions();
    if (equippedByDefaultAssignmentOptions(oc, "bluelight", v, false)) {
        if (MSGlobals::gUseMesoSim) {
            WRITE_WARNINGF(TL("bluelight device is not compatible with mesosim (ignored for vehicle '%')"), v.getID());
        } else {
            MSDevice_Bluelight* device = new MSDevice_Bluelight(v, "bluelight_" + v.getID(),
                    v.getFloatParam("device.bluelight.reactiondist"),
                    v.getFloatParam("device.bluelight.mingapfactor"));
            into.push_back(device);
        }
    }
}


// ---------------------------------------------------------------------------
// MSDevice_Bluelight-methods
// ---------------------------------------------------------------------------
MSDevice_Bluelight::MSDevice_Bluelight(SUMOVehicle& holder, const std::string& id,
                                       const double reactionDist, const double minGapFactor) :
    MSVehicleDevice(holder, id),
    myReactionDist(reactionDist),
    myMinGapFactor(minGapFactor) {
#ifdef DEBUG_BLUELIGHT
    std::cout << SIMTIME << " initialized device '" << id << "' with myReactionDist=" << myReactionDist << "\n";
#endif
}


MSDevice_Bluelight::~MSDevice_Bluelight() {
}


bool
MSDevice_Bluelight::notifyMove(SUMOTrafficObject& veh, double /* oldPos */,
                               double /* newPos */, double newSpeed) {
#ifdef DEBUG_BLUELIGHT
    std::cout << SIMTIME  << " device '" << getID() << "' notifyMove: newSpeed=" << newSpeed << "\n";
#else
    UNUSED_PARAMETER(newSpeed);
#endif
    //violate red lights  this only need to be done once so shift it todo
    MSVehicle& ego = dynamic_cast<MSVehicle&>(veh);
    MSVehicle::Influencer& redLight = ego.getInfluencer();
    const double vMax = ego.getLane()->getVehicleMaxSpeed(&ego);
    redLight.setSpeedMode(7);
    if (ego.getSpeed() < 0.5 * vMax) {
        // advance as far as possible (assume vehicles will keep moving out of the way)
        ego.getLaneChangeModel().setParameter(toString(SUMO_ATTR_LCA_STRATEGIC_PARAM), "-1");
        ego.getLaneChangeModel().setParameter(toString(SUMO_ATTR_LCA_SPEEDGAIN_LOOKAHEAD), "0");
        try {
            ego.getLaneChangeModel().setParameter(toString(SUMO_ATTR_MINGAP_LAT), "0");
        } catch (InvalidArgument&) {
            // not supported by the current laneChangeModel
        }
    } else {
        // restore defaults
        ego.getLaneChangeModel().setParameter(toString(SUMO_ATTR_LCA_STRATEGIC_PARAM),
                                              ego.getVehicleType().getParameter().getLCParamString(SUMO_ATTR_LCA_STRATEGIC_PARAM, "1"));
        ego.getLaneChangeModel().setParameter(toString(SUMO_ATTR_LCA_SPEEDGAIN_LOOKAHEAD),
                                              ego.getVehicleType().getParameter().getLCParamString(SUMO_ATTR_LCA_SPEEDGAIN_LOOKAHEAD, "5"));
        try {
            ego.getLaneChangeModel().setParameter(toString(SUMO_ATTR_MINGAP_LAT),
                                                  toString(ego.getVehicleType().getMinGapLat()));
        } catch (InvalidArgument&) {
            // not supported by the current laneChangeModel
        }
    }
    // build a rescue lane for all vehicles on the route of the emergency vehicle within the range of the siren
    MSVehicleType* vt = MSNet::getInstance()->getVehicleControl().getVType(veh.getVehicleType().getID());
    vt->setPreferredLateralAlignment(LatAlignmentDefinition::ARBITRARY);
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    // use edges on the way of the emergency vehicle
    std::vector<const MSEdge*> upcomingEdges;
    std::set<MSVehicle*, ComparatorIdLess> upcomingVehicles;
    std::set<std::string> lastStepInfluencedVehicles = myInfluencedVehicles;
    std::vector<MSLink*> upcomingLinks;
    double affectedJunctionDist = ego.getPositionOnLane() + myReactionDist;
    for (const MSLane* const l : ego.getUpcomingLanesUntil(myReactionDist)) {
        upcomingEdges.push_back(&l->getEdge());

        affectedJunctionDist -= l->getLength();
        if (affectedJunctionDist > 0 && l->isInternal()) {
            upcomingLinks.push_back(l->getIncomingLanes()[0].viaLink);
        }
    }

    for (const MSEdge* const e : upcomingEdges) {
        //inform all vehicles on upcomingEdges
        for (const SUMOVehicle* v : e->getVehicles()) {
            upcomingVehicles.insert(dynamic_cast<MSVehicle*>(const_cast<SUMOVehicle*>(v)));
            if (lastStepInfluencedVehicles.count(v->getID()) > 0) {
                lastStepInfluencedVehicles.erase(v->getID());
            }
        }
    }
    // reset all vehicles that were in myInfluencedVehicles in the previous step but not in the current step todo refactor
    for (std::string vehID : lastStepInfluencedVehicles) {
        myInfluencedVehicles.erase(vehID);
        Parameterised::Map::iterator it = myInfluencedTypes.find(vehID);
        MSVehicle* veh2 = dynamic_cast<MSVehicle*>(vc.getVehicle(vehID));
        if (veh2 != nullptr && it != myInfluencedTypes.end()) {
            // The vehicle gets back its old VehicleType after the emergency vehicle have passed them
            resetVehicle(veh2, it->second);
        }
    }

    for (MSVehicle* veh2 : upcomingVehicles) {
        assert(veh2 != nullptr);
        if (veh2->getLane() == nullptr) {
            continue;
        }
        if (std::find(upcomingEdges.begin(), upcomingEdges.end(), &veh2->getLane()->getEdge()) != upcomingEdges.end()) {
            if (veh2->getDevice(typeid(MSDevice_Bluelight)) != nullptr) {
                // emergency vehicles should not react
                continue;
            }
            const int numLanes = (int)veh2->getLane()->getEdge().getNumLanes();
            // make sure that vehicles are still building the rescue lane as they might have moved to a new edge or changed lanes
            if (myInfluencedVehicles.count(veh2->getID()) > 0) {
                // Vehicle gets a new Vehicletype to change the alignment and the lanechange options
                MSVehicleType& t = veh2->getSingularType();
                // Setting the lateral alignment to build a rescue lane
                LatAlignmentDefinition align = LatAlignmentDefinition::RIGHT;
                if (veh2->getLane()->getIndex() == numLanes - 1) {
                    align = LatAlignmentDefinition::LEFT;
                }
                t.setPreferredLateralAlignment(align);
#ifdef DEBUG_BLUELIGHT_RESCUELANE
                std::cout << "Refresh alignment for vehicle: " << veh2->getID()
                          << " laneIndex=" << veh2->getLane()->getIndex() << " numLanes=" << numLanes
                          << " alignment=" << toString(align) << "\n";
#endif
            }

            double distanceDelta = veh.getPosition().distanceTo(veh2->getPosition());
            //emergency vehicle has to slow down when entering the rescue lane
            if (distanceDelta <= 10 && veh.getID() != veh2->getID() && myInfluencedVehicles.count(veh2->getID()) > 0 && veh2->getSpeed() < 1) {
                // set ev speed to 20 km/h 0 5.56 m/s
                std::vector<std::pair<SUMOTime, double> > speedTimeLine;
                speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep(), veh.getSpeed()));
                speedTimeLine.push_back(std::make_pair(MSNet::getInstance()->getCurrentTimeStep() + TIME2STEPS(2), 5.56));
                redLight.setSpeedTimeLine(speedTimeLine);
            }

            // the perception of the sound of the siren should be around 25 meters
            // todo only vehicles in front of the emergency vehicle should react
            if (distanceDelta <= myReactionDist && veh.getID() != veh2->getID() && myInfluencedVehicles.count(veh2->getID()) == 0) {
                // only a percentage of vehicles should react to the emergency vehicle to make the behaviour more realistic
                double reaction = RandHelper::rand();
                MSVehicle::Influencer& lanechange = veh2->getInfluencer();

                //other vehicle should not use the rescue lane so they should not make any lane changes
                lanechange.setLaneChangeMode(1605);//todo change lane back
                // the vehicles should react according to the distance to the emergency vehicle taken from real world data
                double reactionProb = (
                                          distanceDelta < myHolder.getFloatParam("device.bluelight.near-dist", false, 12.5)
                                          ? myHolder.getFloatParam("device.bluelight.reaction-prob-near", false, 0.577)
                                          : myHolder.getFloatParam("device.bluelight.reaction-prob-far", false, 0.189));
                // todo works only for one second steps
                //std::cout << SIMTIME << " veh2=" << veh2->getID() << " distanceDelta=" << distanceDelta << " reaction=" << reaction << " reactionProb=" << reactionProb << "\n";
                if (veh2->isActionStep(SIMSTEP) && reaction < reactionProb * veh2->getActionStepLengthSecs()) {
                    myInfluencedVehicles.insert(veh2->getID());
                    myInfluencedTypes.insert(std::make_pair(veh2->getID(), veh2->getVehicleType().getID()));
                    if (myMinGapFactor != 1.) {
                        // TODO this is a permanent change to the vtype!
                        MSNet::getInstance()->getVehicleControl().getVType(veh2->getVehicleType().getID())->getCarFollowModel().setCollisionMinGapFactor(myMinGapFactor);
                    }

                    // Vehicle gets a new Vehicletype to change the alignment and the lanechange options
                    MSVehicleType& t = veh2->getSingularType();
                    // Setting the lateral alignment to build a rescue lane
                    LatAlignmentDefinition align = LatAlignmentDefinition::RIGHT;
                    if (veh2->getLane()->getIndex() == numLanes - 1) {
                        align = LatAlignmentDefinition::LEFT;
                    }
                    t.setPreferredLateralAlignment(align);
                    t.setMinGap(t.getMinGap() * myMinGapFactor);
                    const_cast<SUMOVTypeParameter&>(t.getParameter()).jmParameter[SUMO_ATTR_JM_STOPLINE_GAP] = toString(myMinGapFactor);
                    // disable strategic lane-changing
#ifdef DEBUG_BLUELIGHT_RESCUELANE
                    std::cout << SIMTIME << " device=" << getID() << " formingRescueLane=" << veh2->getID()
                              << " laneIndex=" << veh2->getLane()->getIndex() << " numLanes=" << numLanes
                              << " alignment=" << toString(align) << "\n";
#endif
                    std::vector<std::string> influencedBy = StringTokenizer(veh2->getParameter().getParameter(INFLUENCED_BY, "")).getVector();
                    if (std::find(influencedBy.begin(), influencedBy.end(), myHolder.getID()) == influencedBy.end()) {
                        influencedBy.push_back(myHolder.getID());
                        const_cast<SUMOVehicleParameter&>(veh2->getParameter()).setParameter(INFLUENCED_BY, toString(influencedBy));
                    }
                    veh2->getLaneChangeModel().setParameter(toString(SUMO_ATTR_LCA_STRATEGIC_PARAM), "-1");
                }
            }

        } else { //if vehicle is passed all vehicles which had to react should get their state back after they leave the communication range
            if (myInfluencedVehicles.count(veh2->getID()) > 0) {
                double distanceDelta = veh.getPosition().distanceTo(veh2->getPosition());
                if (distanceDelta > myReactionDist && veh.getID() != veh2->getID()) {
                    myInfluencedVehicles.erase(veh2->getID());
                    Parameterised::Map::iterator it = myInfluencedTypes.find(veh2->getID());
                    if (it != myInfluencedTypes.end()) {
                        // The vehicle gets back its old VehicleType after the emergency vehicle have passed them
                        resetVehicle(veh2, it->second);
                    }
                }
            }
        }
    }
    // make upcoming junction foes slow down
    for (MSLink* link : upcomingLinks) {
        auto avi = link->getApproaching(&ego);
        MSLink::BlockingFoes blockingFoes;
        link->opened(avi.arrivalTime, avi.arrivalSpeed, avi.arrivalSpeed, ego.getLength(),
                     0, ego.getCarFollowModel().getMaxDecel(), ego.getWaitingTime(), ego.getLateralPositionOnLane(), &blockingFoes, true, &ego);
        const SUMOTime timeToArrival = avi.arrivalTime - SIMSTEP;
        for (const SUMOTrafficObject* foe : blockingFoes) {
            if (!foe->isVehicle()) {
                continue;
            }
            const double dist = ego.getPosition().distanceTo2D(foe->getPosition());
            if (dist < myReactionDist) {
                MSVehicle* microFoe = dynamic_cast<MSVehicle*>(const_cast<SUMOTrafficObject*>(foe));
                if (microFoe->getDevice(typeid(MSDevice_Bluelight)) != nullptr) {
                    // emergency vehicles should not react
                    continue;
                }
                const double timeToBrake = foe->getSpeed() / 4.5;
                if (timeToArrival < TIME2STEPS(timeToBrake + 1)) {
                    ;
                    std::vector<std::pair<SUMOTime, double> > speedTimeLine;
                    speedTimeLine.push_back(std::make_pair(SIMSTEP, foe->getSpeed()));
                    speedTimeLine.push_back(std::make_pair(avi.arrivalTime, 0));
                    microFoe->getInfluencer().setSpeedTimeLine(speedTimeLine);
                    //std::cout << SIMTIME << " foe=" << foe->getID() << " dist=" << dist << " timeToBrake= " << timeToBrake << " ttA=" << STEPS2TIME(timeToArrival) << "\n";
                }
            }
        }
    }

    // ego is at the end of its current lane and cannot continue
    const double distToEnd = ego.getLane()->getLength() - ego.getPositionOnLane();
    //std::cout << SIMTIME << " " << getID() << " lane=" << ego.getLane()->getID() << " pos=" << ego.getPositionOnLane() << " distToEnd=" << distToEnd << " conts=" << toString(ego.getBestLanesContinuation()) << " furtherEdges=" << upcomingEdges.size() << "\n";
    if (ego.getBestLanesContinuation().size() == 1 && distToEnd <= POSITION_EPS
            // route continues
            && upcomingEdges.size() > 1) {
        const MSEdge* currentEdge = &ego.getLane()->getEdge();
        // move onto the intersection as if there was a connection from the current lane
        const MSEdge* next = currentEdge->getInternalFollowingEdge(upcomingEdges[1], ego.getVClass());
        if (next == nullptr) {
            next = upcomingEdges[1];
        }
        // pick the lane that causes the minimizes lateral jump
        const std::vector<MSLane*>* allowed = next->allowedLanes(ego.getVClass());
        MSLane* nextLane = next->getLanes().front();
        double bestJump = std::numeric_limits<double>::max();
        double newPosLat = 0;
        if (allowed != nullptr) {
            for (MSLane* nextCand : *allowed) {
                for (auto ili : nextCand->getIncomingLanes()) {
                    if (&ili.lane->getEdge() == currentEdge) {
                        double jump = fabs(ego.getLatOffset(ili.lane) + ego.getLateralPositionOnLane());
                        if (jump < bestJump) {
                            //std::cout << SIMTIME << " nextCand=" << nextCand->getID() << " from=" << ili.lane->getID() << " jump=" << jump << "\n";
                            bestJump = jump;
                            nextLane = nextCand;
                            // stay within newLane
                            const double maxVehOffset = MAX2(0.0, nextLane->getWidth() - ego.getVehicleType().getWidth()) * 0.5;
                            newPosLat = ego.getLatOffset(ili.lane) + ego.getLateralPositionOnLane();
                            newPosLat = MAX2(-maxVehOffset, newPosLat);
                            newPosLat = MIN2(maxVehOffset, newPosLat);
                        }
                    }
                }
            }
        }
        ego.leaveLane(NOTIFICATION_JUNCTION, nextLane);
        ego.getLaneChangeModel().cleanupShadowLane();
        ego.getLaneChangeModel().cleanupTargetLane();
        ego.setTentativeLaneAndPosition(nextLane, 0, newPosLat); // update position
        ego.enterLaneAtMove(nextLane);
        // sublane model must adapt state to the new lane
        ego.getLaneChangeModel().prepareStep();
    }
    return true; // keep the device
}


void
MSDevice_Bluelight::resetVehicle(MSVehicle* veh2, const std::string& targetTypeID) {
    MSVehicleType* targetType = MSNet::getInstance()->getVehicleControl().getVType(targetTypeID);
    //targetType is nullptr if the vehicle type has already changed to its old vehicleType
    if (targetType != nullptr) {
#ifdef DEBUG_BLUELIGHT_RESCUELANE
        std::cout << SIMTIME << " device=" << getID() << " reset " << veh2->getID() << "\n";
#endif

        std::vector<std::string> influencedBy = StringTokenizer(veh2->getParameter().getParameter(INFLUENCED_BY, "")).getVector();
        auto it = std::find(influencedBy.begin(), influencedBy.end(), myHolder.getID());
        if (it != influencedBy.end()) {
            influencedBy.erase(it);
            const_cast<SUMOVehicleParameter&>(veh2->getParameter()).setParameter(INFLUENCED_BY, toString(influencedBy));
        }
        if (influencedBy.size() == 0) {
            veh2->replaceVehicleType(targetType);
            veh2->getLaneChangeModel().setParameter(toString(SUMO_ATTR_LCA_STRATEGIC_PARAM),
                                                    targetType->getParameter().getLCParamString(SUMO_ATTR_LCA_STRATEGIC_PARAM, "1"));
        }
    }
}



bool
MSDevice_Bluelight::notifyEnter(SUMOTrafficObject& veh, MSMoveReminder::Notification reason, const MSLane* enteredLane) {
    UNUSED_PARAMETER(veh);
#ifdef DEBUG_BLUELIGHT
    std::cout << SIMTIME << " device '" << getID() << "' notifyEnter: reason=" << toString(reason) << " enteredLane=" << Named::getIDSecure(enteredLane)  << "\n";
#else
    UNUSED_PARAMETER(reason);
    UNUSED_PARAMETER(enteredLane);
#endif
    return true; // keep the device
}


bool
MSDevice_Bluelight::notifyLeave(SUMOTrafficObject& veh, double /*lastPos*/, MSMoveReminder::Notification reason, const MSLane* enteredLane) {
    UNUSED_PARAMETER(veh);
#ifdef DEBUG_BLUELIGHT
    std::cout << SIMTIME << " device '" << getID() << "' notifyLeave: reason=" << toString(reason) << " approachedLane=" << Named::getIDSecure(enteredLane) << "\n";
#else
    UNUSED_PARAMETER(reason);
    UNUSED_PARAMETER(enteredLane);
#endif
    return true; // keep the device
}


void
MSDevice_Bluelight::generateOutput(OutputDevice* tripinfoOut) const {
    if (tripinfoOut != nullptr) {
        tripinfoOut->openTag("bluelight");
        tripinfoOut->closeTag();
    }
}

std::string
MSDevice_Bluelight::getParameter(const std::string& key) const {
    if (key == "reactiondist") {
        return toString(myReactionDist);
    }
    throw InvalidArgument("Parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
}


void
MSDevice_Bluelight::setParameter(const std::string& key, const std::string& value) {
    double doubleValue;
    try {
        doubleValue = StringUtils::toDouble(value);
    } catch (NumberFormatException&) {
        throw InvalidArgument("Setting parameter '" + key + "' requires a number for device of type '" + deviceName() + "'");
    }
    if (key == "reactiondist") {
        myReactionDist = doubleValue;
    } else {
        throw InvalidArgument("Setting parameter '" + key + "' is not supported for device of type '" + deviceName() + "'");
    }
}


/****************************************************************************/
