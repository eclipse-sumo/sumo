/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2021 German Aerospace Center (DLR) and others.
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
/// @file    TrafficLight.cpp
/// @author  Daniel Krajzewicz
/// @author  Mario Krumnow
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    30.05.2012
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <utils/common/StringUtils.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/transportables/MSTransportable.h>
#include <microsim/transportables/MSPerson.h>
#include <microsim/traffic_lights/MSTLLogicControl.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/traffic_lights/MSActuatedTrafficLightLogic.h>
#include <microsim/traffic_lights/MSDelayBasedTrafficLightLogic.h>
#include "microsim/traffic_lights/NEMAController.h"
#include <microsim/traffic_lights/MSRailSignal.h>
#include <microsim/traffic_lights/MSRailSignalConstraint.h>
#include <microsim/traffic_lights/MSRailSignalControl.h>
#include <netload/NLDetectorBuilder.h>
#include <libsumo/TraCIConstants.h>
#include "Helper.h"
#include "TrafficLight.h"

//#define DEBUG_CONSTRAINT_DEADLOCK

namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults TrafficLight::mySubscriptionResults;
ContextSubscriptionResults TrafficLight::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
TrafficLight::getIDList() {
    return MSNet::getInstance()->getTLSControl().getAllTLIds();
}


int
TrafficLight::getIDCount() {
    return (int)getIDList().size();
}


std::string
TrafficLight::getRedYellowGreenState(const std::string& tlsID) {
    return Helper::getTLS(tlsID).getActive()->getCurrentPhaseDef().getState();
}


std::vector<TraCILogic>
TrafficLight::getAllProgramLogics(const std::string& tlsID) {
    std::vector<TraCILogic> result;
    const std::vector<MSTrafficLightLogic*> logics = Helper::getTLS(tlsID).getAllLogics();
    for (MSTrafficLightLogic* logic : logics) {
        TraCILogic l(logic->getProgramID(), (int)logic->getLogicType(), logic->getCurrentPhaseIndex());
        l.subParameter = logic->getParametersMap();
        for (const MSPhaseDefinition* const phase : logic->getPhases()) {
            l.phases.emplace_back(new TraCIPhase(STEPS2TIME(phase->duration), phase->getState(),
                                                 STEPS2TIME(phase->minDuration), STEPS2TIME(phase->maxDuration),
                                                 phase->getNextPhases(), phase->getName()));
        }
        result.emplace_back(l);
    }
    return result;
}


std::vector<std::string>
TrafficLight::getControlledJunctions(const std::string& tlsID) {
    std::set<std::string> junctionIDs;
    const MSTrafficLightLogic::LinkVectorVector& links = Helper::getTLS(tlsID).getActive()->getLinks();
    for (const MSTrafficLightLogic::LinkVector& llinks : links) {
        for (const MSLink* l : llinks) {
            junctionIDs.insert(l->getJunction()->getID());
        }
    }
    return std::vector<std::string>(junctionIDs.begin(), junctionIDs.end());
}


std::vector<std::string>
TrafficLight::getControlledLanes(const std::string& tlsID) {
    std::vector<std::string> laneIDs;
    const MSTrafficLightLogic::LaneVectorVector& lanes = Helper::getTLS(tlsID).getActive()->getLaneVectors();
    for (const MSTrafficLightLogic::LaneVector& llanes : lanes) {
        for (const MSLane* l : llanes) {
            laneIDs.push_back(l->getID());
        }
    }
    return laneIDs;
}


std::vector<std::vector<TraCILink> >
TrafficLight::getControlledLinks(const std::string& tlsID) {
    std::vector<std::vector<TraCILink> > result;
    const MSTrafficLightLogic::LaneVectorVector& lanes = Helper::getTLS(tlsID).getActive()->getLaneVectors();
    const MSTrafficLightLogic::LinkVectorVector& links = Helper::getTLS(tlsID).getActive()->getLinks();
    for (int i = 0; i < (int)lanes.size(); ++i) {
        std::vector<TraCILink> subList;
        const MSTrafficLightLogic::LaneVector& llanes = lanes[i];
        const MSTrafficLightLogic::LinkVector& llinks = links[i];
        // number of links controlled by this signal (signal i)
        for (int j = 0; j < (int)llanes.size(); ++j) {
            MSLink* link = llinks[j];
            // approached non-internal lane (if any)
            const std::string to = link->getLane() != nullptr ? link->getLane()->getID() : "";
            // approached "via", internal lane (if any)
            const std::string via = link->getViaLane() != nullptr ? link->getViaLane()->getID() : "";
            subList.emplace_back(TraCILink(llanes[j]->getID(), via, to));
        }
        result.emplace_back(subList);
    }
    return result;
}


std::string
TrafficLight::getProgram(const std::string& tlsID) {
    return Helper::getTLS(tlsID).getActive()->getProgramID();
}


int
TrafficLight::getPhase(const std::string& tlsID) {
    return Helper::getTLS(tlsID).getActive()->getCurrentPhaseIndex();
}


std::string
TrafficLight::getPhaseName(const std::string& tlsID) {
    return Helper::getTLS(tlsID).getActive()->getCurrentPhaseDef().getName();
}


double
TrafficLight::getPhaseDuration(const std::string& tlsID) {
    return STEPS2TIME(Helper::getTLS(tlsID).getActive()->getCurrentPhaseDef().duration);
}


double
TrafficLight::getNextSwitch(const std::string& tlsID) {
    return STEPS2TIME(Helper::getTLS(tlsID).getActive()->getNextSwitchTime());
}

int
TrafficLight::getServedPersonCount(const std::string& tlsID, int index) {
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getActive();
    if (index < 0 || active->getPhaseNumber() <= index) {
        throw TraCIException("The phase index " + toString(index) + " is not in the allowed range [0,"
                             + toString(active->getPhaseNumber() - 1) + "].");
    }
    // find all crossings which have a green light in that phas
    int result = 0;

    const std::string& state = active->getPhases()[index]->getState();
    for (int i = 0; i < (int)state.size(); i++) {
        for (MSLink* link : active->getLinksAt(i)) {
            if (link->getLane()->getEdge().isCrossing()) {
                // walking forwards across
                for (MSTransportable* person : link->getLaneBefore()->getEdge().getPersons()) {
                    if (static_cast<MSPerson*>(person)->getNextEdge() == link->getLane()->getEdge().getID()) {
                        result += 1;
                    }
                }
                // walking backwards across
                MSLane* walkingAreaAcross = link->getLane()->getLinkCont().front()->getLane();
                for (MSTransportable* person : walkingAreaAcross->getEdge().getPersons()) {
                    if (static_cast<MSPerson*>(person)->getNextEdge() == link->getLane()->getEdge().getID()) {
                        result += 1;
                    }
                }
            } else if (link->getLaneBefore()->getEdge().isCrossing()) {
                // walking backwards across (in case both sides are separately controlled)
                for (MSTransportable* person : link->getLane()->getEdge().getPersons()) {
                    if (static_cast<MSPerson*>(person)->getNextEdge() == link->getLaneBefore()->getEdge().getID()) {
                        result += 1;
                    }
                }
            }
        }
    }
    return result;
}

std::vector<std::string>
TrafficLight::getBlockingVehicles(const std::string& tlsID, int linkIndex) {
    std::vector<std::string> result;
    // for railsignals we cannot use the "online" program
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getDefault();
    if (linkIndex < 0 || linkIndex >= active->getNumLinks()) {
        throw TraCIException("The link index " + toString(linkIndex) + " is not in the allowed range [0,"
                             + toString(active->getNumLinks() - 1) + "].");
    }
    for (const SUMOVehicle* veh : active->getBlockingVehicles(linkIndex)) {
        result.push_back(veh->getID());
    }
    return result;
}

std::vector<std::string>
TrafficLight::getRivalVehicles(const std::string& tlsID, int linkIndex) {
    std::vector<std::string> result;
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getDefault();
    if (linkIndex < 0 || linkIndex >= active->getNumLinks()) {
        throw TraCIException("The link index " + toString(linkIndex) + " is not in the allowed range [0,"
                             + toString(active->getNumLinks() - 1) + "].");
    }
    for (const SUMOVehicle* veh : active->getRivalVehicles(linkIndex)) {
        result.push_back(veh->getID());
    }
    return result;
}

std::vector<std::string>
TrafficLight::getPriorityVehicles(const std::string& tlsID, int linkIndex) {
    std::vector<std::string> result;
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getDefault();
    if (linkIndex < 0 || linkIndex >= active->getNumLinks()) {
        throw TraCIException("The link index " + toString(linkIndex) + " is not in the allowed range [0,"
                             + toString(active->getNumLinks() - 1) + "].");
    }
    for (const SUMOVehicle* veh : active->getPriorityVehicles(linkIndex)) {
        result.push_back(veh->getID());
    }
    return result;
}

std::vector<TraCISignalConstraint>
TrafficLight::getConstraints(const std::string& tlsID, const std::string& tripId) {
    std::vector<TraCISignalConstraint> result;
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getDefault();
    MSRailSignal* s = dynamic_cast<MSRailSignal*>(active);
    if (s == nullptr) {
        throw TraCIException("'" + tlsID + "' is not a rail signal");
    }
    for (auto item : s->getConstraints()) {
        if (tripId != "" && tripId != item.first) {
            continue;
        }
        for (MSRailSignalConstraint* c : item.second) {
            result.push_back(buildConstraint(tlsID, item.first, c, false));
        }
    }
    for (auto item : s->getInsertionConstraints()) {
        if (tripId != "" && tripId != item.first) {
            continue;
        }
        for (MSRailSignalConstraint* c : item.second) {
            result.push_back(buildConstraint(tlsID, item.first, c, true));
        }
    }
    return result;
}

std::vector<TraCISignalConstraint>
TrafficLight::getConstraintsByFoe(const std::string& foeSignal, const std::string& foeId) {
    // retrieve all constraints that have the given foeSignal (optionally filtered by foeId)
    // @note could improve efficiency by storing a map of rail signals in MSRailSignalControl
    std::vector<TraCISignalConstraint> result;
    for (const std::string& tlsID : getIDList()) {
        MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getDefault();
        MSRailSignal* s = dynamic_cast<MSRailSignal*>(active);
        if (s != nullptr) {
            for (auto item : s->getConstraints()) {
                for (MSRailSignalConstraint* cand : item.second) {
                    MSRailSignalConstraint_Predecessor* pc = dynamic_cast<MSRailSignalConstraint_Predecessor*>(cand);
                    if (pc != nullptr && pc->myFoeSignal->getID() == foeSignal
                            && (foeId == "" || pc->myTripId == foeId)) {
                        result.push_back(buildConstraint(s->getID(), item.first, pc, false));
                    }
                }
            }
            for (auto item : s->getInsertionConstraints()) {
                for (MSRailSignalConstraint* cand : item.second) {
                    MSRailSignalConstraint_Predecessor* pc = dynamic_cast<MSRailSignalConstraint_Predecessor*>(cand);
                    if (pc != nullptr && pc->myFoeSignal->getID() == foeSignal
                            && (foeId == "" || pc->myTripId == foeId)) {
                        result.push_back(buildConstraint(s->getID(), item.first, pc, true));
                    }
                }
            }
        }
    }
    return result;
}

std::vector<TraCISignalConstraint>
TrafficLight::swapConstraints(const std::string& tlsID, const std::string& tripId, const std::string& foeSignal, const std::string& foeId) {
#ifdef DEBUG_CONSTRAINT_DEADLOCK
    std::cout << "swapConstraints tlsId=" << tlsID << " tripId=" << tripId << " foeSignal=" << foeSignal << " foeId=" << foeId << "\n";
#endif
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getDefault();
    MSTrafficLightLogic* const active2 = Helper::getTLS(foeSignal).getDefault();
    MSRailSignal* s = dynamic_cast<MSRailSignal*>(active);
    MSRailSignal* s2 = dynamic_cast<MSRailSignal*>(active2);
    if (s == nullptr) {
        throw TraCIException("'" + tlsID + "' is not a rail signal");
    }
    if (s2 == nullptr) {
        throw TraCIException("'" + foeSignal + "' is not a rail signal");
    }
    MSRailSignalConstraint_Predecessor* c = nullptr;
    for (auto item : s->getConstraints()) {
        if (tripId == item.first) {
            for (MSRailSignalConstraint* cand : item.second) {
                MSRailSignalConstraint_Predecessor* pc = dynamic_cast<MSRailSignalConstraint_Predecessor*>(cand);
                if (pc != nullptr && pc->myFoeSignal->getID() == foeSignal && pc->myTripId == foeId) {
                    c = pc;
                    break;
                }
            }
            break;
        }
    }
    if (c != nullptr) {
        const int limit = c->myLimit;
        s->removeConstraint(tripId, c);
        s2->addConstraint(foeId, new MSRailSignalConstraint_Predecessor(s, tripId, limit));
        return findConstraintsDeadLocks(foeId, tripId, foeSignal, tlsID);
    } else {
        throw TraCIException("Rail signal '" + tlsID + "' does not have a contraint for tripId '" + tripId + "' with foeSignal '" + foeSignal + "' and foeId '" + foeId + "'");
    }
}

void
TrafficLight::removeConstraints(const std::string& tlsID, const std::string& tripId, const std::string& foeSignal, const std::string& foeId) {
    // remove all constraints that have the given foeId
    // @note could improve efficiency by storing a map of rail signals in MSRailSignalControl
    for (const std::string& tlsCand : getIDList()) {
        if (tlsID == "" || tlsCand == tlsID) {
            MSTrafficLightLogic* const active = Helper::getTLS(tlsCand).getDefault();
            MSRailSignal* s = dynamic_cast<MSRailSignal*>(active);
            if (s != nullptr) {
                auto cands = s->getConstraints(); // make copy
                for (auto item : cands) {
                    if (tripId == "" || item.first == tripId) {
                        for (MSRailSignalConstraint* cand : item.second) {
                            MSRailSignalConstraint_Predecessor* pc = dynamic_cast<MSRailSignalConstraint_Predecessor*>(cand);
                            if (pc != nullptr
                                    && (foeId == "" || pc->myTripId == foeId)
                                    && (foeSignal == "" || pc->myFoeSignal->getID() == foeSignal)) {
                                s->removeConstraint(item.first, cand);
                            }
                        }
                    }
                }
            }
        }
    }
}


std::vector<TraCISignalConstraint>
TrafficLight::findConstraintsDeadLocks(const std::string& foeId, const std::string& tripId, const std::string& foeSignal, const std::string& tlsID) {
    std::vector<TraCISignalConstraint> result;
    // find circular constraints (deadlock)
    // foeId is now constrainted by tripId and assumed to follow tripId on the
    // same track without possibility of overtaking
    // we look for a third vehicle foeId2 where
    // tripId waits for foeId2 and foeId2 waits on foeId
    std::map<std::string, TraCISignalConstraint> constraintsOnTripId;
    std::map<std::string, TraCISignalConstraint> constrainedByFoeId;
    std::set<std::string> foeId2Cands1;
    std::set<std::string> foeId2Cands2;
    for (MSRailSignal* s : MSRailSignalControl::getInstance().getSignals()) {
        for (auto item : s->getConstraints()) {
            for (MSRailSignalConstraint* cand : item.second) {
                MSRailSignalConstraint_Predecessor* pc = dynamic_cast<MSRailSignalConstraint_Predecessor*>(cand);
                if (pc != nullptr && !pc->cleared()) {
                    if (item.first == tripId) {
                        // tripId waits for foe2
                        // @could there by more than one constraint on tripId by this foe2?
                        constraintsOnTripId[pc->myTripId] = buildConstraint(s->getID(), item.first, pc, false);
                        foeId2Cands1.insert(pc->myTripId);
                    } else if (pc->myTripId == foeId) {
                        // foeId2 waits for foe
                        constrainedByFoeId[item.first] = buildConstraint(s->getID(), item.first, pc, false);
                        foeId2Cands2.insert(item.first);
                    }
                }
            }
        }
    }
    if (foeId2Cands1.size() > 0) {
        // foe2 might be constrained implicitly by foe due to following on the same track
        // in this case foe must be on the route of foe2 between its current position and foeSignal

        // we have to check this first because it also affects foeInsertion
        // constraints if the foe is already inserted but hasn't yet passed the
        // signal (cleared == false).
        SUMOVehicle* foe = getVehicleByTripId(foeId);
        if (foe != nullptr) {
            const MSEdge* foeEdge = foe->getEdge();
            const double foePos = foe->getPositionOnLane();
            for (const std::string& foeId2 : foeId2Cands1) {
                // tripId waits for foeId2
                SUMOVehicle* foe2 = getVehicleByTripId(foeId2);
                if (foe2 != nullptr) {
                    const ConstMSEdgeVector& foe2Route = foe2->getRoute().getEdges();
                    const TraCISignalConstraint& c = constraintsOnTripId[foeId2];
                    bool foeAhead = false;
                    for (int i = foe2->getRoutePosition(); i < (int)foe2Route.size(); i++) {
                        const MSEdge* e = foe2Route[i];
                        if (e == foeEdge &&
                                ((e != foe2->getEdge() || foe2->getPositionOnLane() < foePos)
                                 || (foe->hasDeparted() && !foe2->hasDeparted())
                                 || (!foe->hasDeparted() && !foe2->hasDeparted() &&
                                     (foe->getParameter().depart < foe2->getParameter().depart
                                      || (foe->getParameter().depart == foe2->getParameter().depart && foe->getNumericalID() < foe2->getNumericalID())))
                                )) {
                            foeAhead = true;
#ifdef DEBUG_CONSTRAINT_DEADLOCK
                            std::cout << "findConstraintsDeadLocks foeId=" << foeId << " tripId=" << tripId << " foeSignal=" << foeSignal << "\n";
                            std::cout << "  foeLeaderDeadlock foeEdge=" << foeEdge->getID() << " foe2=" << foe2->getParameter().getParameter("tripId", foe2->getID())
                                      << " routePos=" << foe2->getRoutePosition() << " futureRPos=" << i << " e=" << e->getID()
                                      //<< " foePos=" << foePos << " foe2Pos=" << foe2->getPositionOnLane()
                                      << " " << constraintsOnTripId[foeId2].getString() << "\n";
#endif
                            break;
                        }
                        if (e->getToJunction()->getID() == foeSignal
                                || e->getToJunction()->getID() == c.foeSignal) {
                            break;
                        }
                    }
                    if (foeAhead) {
                        // foe cannot wait for foe2 (since it's behind). Instead foe2 must wait for tripId
                        TraCISignalConstraint nc; // constraint after swap
                        nc.tripId = c.foeId;
                        nc.foeId = c.tripId;
                        nc.signalId = c.foeSignal;
                        nc.foeSignal = c.signalId;
                        nc.limit = c.limit;
                        nc.type = c.type;
                        nc.mustWait = true; // ???
                        result.push_back(nc);
                        // let foe wait for foe2
                        std::vector<TraCISignalConstraint> result2 = swapConstraints(c.signalId, c.tripId, c.foeSignal, c.foeId);
                        result.insert(result.end(), result2.begin(), result2.end());

                        // Other deadlocks might not be valid anymore so we need a fresh recheck for remaining implicit or explicit deadlocks
                        const std::vector<TraCISignalConstraint>& result4 = findConstraintsDeadLocks(foeId, tripId, foeSignal, c.signalId);
                        result.insert(result.end(), result4.begin(), result4.end());
                        return result;
                    }
                }
            }
        }
    }

    if (foeId2Cands2.size() > 0) {
        // tripId might be constrained implicitly by foe2 due to following on the same track
        // in this case foe2 must be on the route of tripId between its current position and tlsID
        // if foe2 then waits for foe, deadlock occurs

        SUMOVehicle* ego = getVehicleByTripId(tripId);
        if (ego != nullptr && (ego->hasDeparted() || !ego->getParameter().wasSet(VEHPARS_FORCE_REROUTE))) {
            std::set<const MSEdge*> egoToSignal;
            const double egoPos = ego->getPositionOnLane();
            const ConstMSEdgeVector& egoRoute = ego->getRoute().getEdges();
            for (int i = ego->getRoutePosition(); i < (int)egoRoute.size(); i++) {
                const MSEdge* e = egoRoute[i];
                egoToSignal.insert(e);
                if (e->getToJunction()->getID() == tlsID) {
                    break;
                }
            }

            for (const std::string& foeId2 : foeId2Cands2) {
                // foeId2 waits for foe
                SUMOVehicle* foe2 = getVehicleByTripId(foeId2);
                //std::cout << "  foe2=" << foe2->getID() << " edge=" << foe2->getEdge()->getID() << " egoToSignal=" << toString(egoToSignal) << "\n";
                if (foe2 != nullptr) {
                    if (egoToSignal.count(foe2->getEdge()) != 0
                            && (foe2->getEdge() != ego->getEdge() || foe2->getPositionOnLane() > egoPos)) {
                        const TraCISignalConstraint& c = constrainedByFoeId[foeId2];
#ifdef DEBUG_CONSTRAINT_DEADLOCK
                        std::cout << "findConstraintsDeadLocks foeId=" << foeId << " tripId=" << tripId << " foeSignal=" << foeSignal << "\n";
                        std::cout << "  egoLeaderDeadlock foe2Edge=" << foe2->getEdge()->getID() << " foe2=" << foe2->getParameter().getParameter("tripId", foe2->getID())
                                  << " " << c.getString() << "\n";
#endif
                        // foe is already waiting for tripId (ego) and should also wait for foeId2
                        TraCISignalConstraint nc; // constraint after swap
                        nc.tripId = c.foeId;
                        nc.foeId = c.tripId;
                        nc.signalId = c.foeSignal;
                        nc.foeSignal = c.signalId;
                        nc.limit = c.limit;
                        nc.type = c.type;
                        nc.mustWait = true; // ???
                        result.push_back(nc);
                        // let foe wait for foe2
                        std::vector<TraCISignalConstraint> result2 = swapConstraints(c.signalId, c.tripId, c.foeSignal, c.foeId);
                        result.insert(result.end(), result2.begin(), result2.end());

                        // Other deadlocks might not be valid anymore so we need a fresh recheck for remaining implicit or explicit deadlocks
                        const std::vector<TraCISignalConstraint>& result4 = findConstraintsDeadLocks(c.foeId, c.tripId, c.foeSignal, c.signalId);
                        result.insert(result.end(), result4.begin(), result4.end());
                        return result;
                    }
                }
            }
        } else if (ego != nullptr) {
            WRITE_WARNING("Cannot check for all deadlocks on swapConstraints because the route for vehicle '" + ego->getID() + "' is not computed yet");
        }
    }

    // find deadlock in explicit constraints
    std::vector<std::string> foeIds2;
    std::set_intersection(
        foeId2Cands1.begin(), foeId2Cands1.end(),
        foeId2Cands2.begin(), foeId2Cands2.end(),
        std::back_inserter(foeIds2));
#ifdef DEBUG_CONSTRAINT_DEADLOCK
    std::cout << "findConstraintsDeadLocks foeId=" << foeId << " tripId=" << tripId << " foeSignal=" << foeSignal << "\n";
    for (const std::string& foeId2 : foeIds2) {
        std::cout << "  deadlockId=" << foeId2 << " " << constraintsOnTripId[foeId2].getString() << " " << constrainedByFoeId[foeId2].getString() << "\n";
    }
#endif
    if (foeIds2.size() > 0) {
        const TraCISignalConstraint& c = constrainedByFoeId[foeIds2.front()];
        TraCISignalConstraint nc; // constraint after swap
        nc.tripId = c.foeId;
        nc.foeId = c.tripId;
        nc.signalId = c.foeSignal;
        nc.foeSignal = c.signalId;
        nc.limit = c.limit;
        nc.type = c.type;
        nc.mustWait = true; // ???
        result.push_back(nc);
        // let foe wait for foe2
        const std::vector<TraCISignalConstraint>& result2 = swapConstraints(c.signalId, c.tripId, c.foeSignal, c.foeId);
        result.insert(result.end(), result2.begin(), result2.end());
        if (foeIds2.size() > 1) {
            // calling swapConstraints once may result in further swaps so we have to recheck for remaining deadlocks anew
            const std::vector<TraCISignalConstraint>& result3 = findConstraintsDeadLocks(foeId, tripId, foeSignal, c.signalId);
            result.insert(result.end(), result3.begin(), result3.end());
        }
    }
    return result;
}


SUMOVehicle*
TrafficLight::getVehicleByTripId(const std::string tripOrVehID) {
    MSVehicleControl& c = MSNet::getInstance()->getVehicleControl();
    for (MSVehicleControl::constVehIt i = c.loadedVehBegin(); i != c.loadedVehEnd(); ++i) {
        SUMOVehicle* veh = i->second;
        if (veh->getParameter().getParameter("tripId", veh->getID()) == tripOrVehID) {
            return veh;
        }
    }
    return nullptr;
}


std::string
TrafficLight::getParameter(const std::string& tlsID, const std::string& paramName) {
    return Helper::getTLS(tlsID).getActive()->getParameter(paramName, "");
}


LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(TrafficLight)


void
TrafficLight::setRedYellowGreenState(const std::string& tlsID, const std::string& state) {
    Helper::getTLS(tlsID).setStateInstantiatingOnline(MSNet::getInstance()->getTLSControl(), state);
}


void
TrafficLight::setPhase(const std::string& tlsID, const int index) {
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getActive();
    if (index < 0 || active->getPhaseNumber() <= index) {
        throw TraCIException("The phase index " + toString(index) + " is not in the allowed range [0,"
                             + toString(active->getPhaseNumber() - 1) + "].");
    }
    const SUMOTime cTime = MSNet::getInstance()->getCurrentTimeStep();
    const SUMOTime duration = active->getPhase(index).duration;
    active->changeStepAndDuration(MSNet::getInstance()->getTLSControl(), cTime, index, duration);
}

void
TrafficLight::setPhaseName(const std::string& tlsID, const std::string& name) {
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getActive();
    const_cast<MSPhaseDefinition&>(active->getCurrentPhaseDef()).setName(name);
}


void
TrafficLight::setProgram(const std::string& tlsID, const std::string& programID) {
    try {
        Helper::getTLS(tlsID).switchTo(MSNet::getInstance()->getTLSControl(), programID);
    } catch (ProcessError& e) {
        throw TraCIException(e.what());
    }
}


void
TrafficLight::setPhaseDuration(const std::string& tlsID, const double phaseDuration) {
    MSTrafficLightLogic* const active = Helper::getTLS(tlsID).getActive();
    const SUMOTime cTime = MSNet::getInstance()->getCurrentTimeStep();
    active->changeStepAndDuration(MSNet::getInstance()->getTLSControl(), cTime, -1, TIME2STEPS(phaseDuration));
}


void
TrafficLight::setProgramLogic(const std::string& tlsID, const TraCILogic& logic) {
    MSTLLogicControl::TLSLogicVariants& vars = Helper::getTLS(tlsID);
    // make sure index and phaseNo are consistent
    if (logic.currentPhaseIndex >= (int)logic.phases.size()) {
        throw TraCIException("set program: parameter index must be less than parameter phase number.");
    }
    std::vector<MSPhaseDefinition*> phases;
    for (TraCIPhase* phase : logic.phases) {
        MSPhaseDefinition* sumoPhase = new MSPhaseDefinition(TIME2STEPS(phase->duration), phase->state, phase->name);
        sumoPhase->minDuration = TIME2STEPS(phase->minDur);
        sumoPhase->maxDuration = TIME2STEPS(phase->maxDur);
        sumoPhase->nextPhases = phase->next;
        phases.push_back(sumoPhase);
    }
    if (vars.getLogic(logic.programID) == nullptr) {
        MSTLLogicControl& tlc = MSNet::getInstance()->getTLSControl();
        int step = logic.currentPhaseIndex;
        const std::string basePath = "";
        MSTrafficLightLogic* tlLogic = nullptr;
        SUMOTime nextSwitch = MSNet::getInstance()->getCurrentTimeStep() + phases[0]->duration;
        switch ((TrafficLightType)logic.type) {
            case TrafficLightType::ACTUATED:
                tlLogic = new MSActuatedTrafficLightLogic(tlc,
                        tlsID, logic.programID,
                        phases, step, nextSwitch,
                        logic.subParameter, basePath);
                break;
            case TrafficLightType::NEMA:
                tlLogic = new NEMALogic(tlc,
                                        tlsID, logic.programID,
                                        phases, step, nextSwitch,
                                        logic.subParameter, basePath);
                break;
            case TrafficLightType::DELAYBASED:
                tlLogic = new MSDelayBasedTrafficLightLogic(tlc,
                        tlsID, logic.programID,
                        phases, step, nextSwitch,
                        logic.subParameter, basePath);
                break;
            case TrafficLightType::STATIC:
                tlLogic = new MSSimpleTrafficLightLogic(tlc,
                                                        tlsID, logic.programID, TrafficLightType::STATIC,
                                                        phases, step, nextSwitch,
                                                        logic.subParameter);
                break;
            default:
                throw TraCIException("Unsupported traffic light type '" + toString(logic.type) + "'");
        }
        vars.addLogic(logic.programID, tlLogic, true, true);
        // XXX pass GUIDetectorBuilder when running with gui
        NLDetectorBuilder db(*MSNet::getInstance());
        tlLogic->init(db);
    } else {
        static_cast<MSSimpleTrafficLightLogic*>(vars.getLogic(logic.programID))->setPhases(phases, logic.currentPhaseIndex);
    }
}


void
TrafficLight::setParameter(const std::string& tlsID, const std::string& paramName, const std::string& value) {
    MSTrafficLightLogic* tll = Helper::getTLS(tlsID).getActive();
    if (StringUtils::startsWith(paramName, "NEMA.") && tll->getLogicType() != TrafficLightType::NEMA) {
        throw TraCIException("'" + tlsID + "' is not a NEMA controller");
    }
    tll->setParameter(paramName, value);
}

LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(TrafficLight, TL)

void
TrafficLight::setNemaSplits(const std::string& tlsID, const std::vector<double>& splits) {
    setParameter(tlsID, "NEMA.splits", toString(splits));
}

void
TrafficLight::setNemaMaxGreens(const std::string& tlsID, const std::vector<double>& maxGreens) {
    setParameter(tlsID, "NEMA.maxGreens", toString(maxGreens));
}

void
TrafficLight::setNemaCycleLength(const std::string& tlsID, double cycleLength) {
    setParameter(tlsID, "NEMA.cycleLength", toString(cycleLength));
}

void
TrafficLight::setNemaOffset(const std::string& tlsID, double offset) {
    setParameter(tlsID, "NEMA.offset", toString(offset));
}


libsumo::TraCISignalConstraint
TrafficLight::buildConstraint(const std::string& tlsID, const std::string& tripId, MSRailSignalConstraint* constraint, bool insertionConstraint) {
    TraCISignalConstraint c;
    c.tripId = tripId;
    MSRailSignalConstraint_Predecessor* pc = dynamic_cast<MSRailSignalConstraint_Predecessor*>(constraint);
    if (pc == nullptr) {
        // unsupported constraint
        c.type = -1;
    } else {
        c.signalId = tlsID;
        c.foeId = pc->myTripId;
        c.foeSignal = pc->myFoeSignal->getID();
        c.limit = pc->myLimit;
        c.type = insertionConstraint ? 1 : 0;
        c.mustWait = !pc->cleared();
    }
    return c;
}


std::shared_ptr<VariableWrapper>
TrafficLight::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
TrafficLight::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case TL_RED_YELLOW_GREEN_STATE:
            return wrapper->wrapString(objID, variable, getRedYellowGreenState(objID));
        case TL_CONTROLLED_LANES:
            return wrapper->wrapStringList(objID, variable, getControlledLanes(objID));
        case TL_CURRENT_PHASE:
            return wrapper->wrapInt(objID, variable, getPhase(objID));
        case VAR_NAME:
            return wrapper->wrapString(objID, variable, getPhaseName(objID));
        case TL_CURRENT_PROGRAM:
            return wrapper->wrapString(objID, variable, getProgram(objID));
        case TL_PHASE_DURATION:
            return wrapper->wrapDouble(objID, variable, getPhaseDuration(objID));
        case TL_NEXT_SWITCH:
            return wrapper->wrapDouble(objID, variable, getNextSwitch(objID));
        case TL_CONTROLLED_JUNCTIONS:
            return wrapper->wrapStringList(objID, variable, getControlledJunctions(objID));
        case libsumo::VAR_PARAMETER:
            paramData->readUnsignedByte();
            return wrapper->wrapString(objID, variable, getParameter(objID, paramData->readString()));
        case libsumo::VAR_PARAMETER_WITH_KEY:
            paramData->readUnsignedByte();
            return wrapper->wrapStringPair(objID, variable, getParameterWithKey(objID, paramData->readString()));
        default:
            return false;
    }
}
}


/****************************************************************************/
