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
/// @file    MSInsertionControl.cpp
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @author  Mirko Barthauer
/// @date    Mon, 12 Mar 2001
///
// Inserts vehicles into the network when their departure time is reached
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <algorithm>
#include <cassert>
#include <iterator>
#include <utils/router/IntermodalRouter.h>
#include <microsim/devices/MSDevice_Routing.h>
#include <microsim/devices/MSRoutingEngine.h>
#include "MSGlobals.h"
#include "MSVehicle.h"
#include "MSVehicleControl.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSNet.h"
#include "MSRouteHandler.h"
#include "MSInsertionControl.h"


// ===========================================================================
// member method definitions
// ===========================================================================
MSInsertionControl::MSInsertionControl(MSVehicleControl& vc,
                                       SUMOTime maxDepartDelay,
                                       bool eagerInsertionCheck,
                                       int maxVehicleNumber,
                                       SUMOTime randomDepartOffset) :
    myVehicleControl(vc),
    myMaxDepartDelay(maxDepartDelay),
    myEagerInsertionCheck(eagerInsertionCheck),
    myMaxVehicleNumber(maxVehicleNumber),
    myPendingEmitsUpdateTime(SUMOTime_MIN),
    myFlowRNG("flow") {
    myMaxRandomDepartOffset = randomDepartOffset;
    RandHelper::initRandGlobal(&myFlowRNG);
}


MSInsertionControl::~MSInsertionControl() {
    for (const Flow& f : myFlows) {
        delete (f.pars);
    }
}


void
MSInsertionControl::add(SUMOVehicle* veh) {
    myAllVeh.add(veh);
}


bool
MSInsertionControl::addFlow(SUMOVehicleParameter* const pars, int index) {
    if (myFlowIDs.count(pars->id) > 0) {
        return false;
    }
    const bool loadingFromState = index >= 0;
    Flow flow{pars, loadingFromState ? index : 0, initScale(pars->vtypeid)};
    if (!loadingFromState && pars->repetitionProbability < 0 && pars->repetitionOffset < 0) {
        // init poisson flow (but only the timing)
        flow.pars->incrementFlow(flow.scale, &myFlowRNG);
        flow.pars->repetitionsDone--;
    }
    myFlows.emplace_back(flow);
    myFlowIDs.insert(std::make_pair(pars->id, flow.index));
    return true;
}


double
MSInsertionControl::initScale(const std::string vtypeid) {
    MSVehicleControl& vc = MSNet::getInstance()->getVehicleControl();
    if (vc.hasVTypeDistribution(vtypeid)) {
        double result = -1;
        const RandomDistributor<MSVehicleType*>* dist = vc.getVTypeDistribution(vtypeid);
        for (const MSVehicleType* t : dist->getVals()) {
            if (result == -1) {
                result = t->getParameter().scale;
            } else if (result != t->getParameter().scale) {
                // unequal scales in distribution
                return -1;
            }
        }
        return result;
    } else {
        // rng is not used since vtypeid is not a distribution
        return vc.getVType(vtypeid, nullptr, true)->getParameter().scale;
    }
}


void
MSInsertionControl::updateScale(const std::string vtypeid) {
    for (Flow& f : myFlows) {
        if (f.pars->vtypeid == vtypeid) {
            f.scale = initScale(vtypeid);
        }
    }
}


int
MSInsertionControl::emitVehicles(SUMOTime time) {
    // check whether any vehicles shall be emitted within this time step
    const bool havePreChecked = MSRoutingEngine::isEnabled();
    if (myPendingEmits.empty() || (havePreChecked && myEmitCandidates.empty())) {
        return 0;
    }
    int numEmitted = 0;
    // we use buffering for the refused emits to save time
    //  for this, we have two lists; one contains previously refused emits, the second
    //  will be used to append those vehicles that will not be able to depart in this
    //  time step
    MSVehicleContainer::VehicleVector refusedEmits;

    // go through the list of previously refused vehicles, first
    MSVehicleContainer::VehicleVector::const_iterator veh;
    for (veh = myPendingEmits.begin(); veh != myPendingEmits.end(); veh++) {
        if (havePreChecked && (myEmitCandidates.count(*veh) == 0)) {
            refusedEmits.push_back(*veh);
        } else {
            numEmitted += tryInsert(time, *veh, refusedEmits);
        }
    }
    myEmitCandidates.clear();
    myPendingEmits = refusedEmits;
    return numEmitted;
}


int
MSInsertionControl::tryInsert(SUMOTime time, SUMOVehicle* veh,
                              MSVehicleContainer::VehicleVector& refusedEmits) {
    assert(veh->getParameter().depart <= time);
    const MSEdge& edge = *veh->getEdge();
    if (veh->isOnRoad()) {
        return 1;
    }
    if ((myMaxVehicleNumber < 0 || (int)MSNet::getInstance()->getVehicleControl().getRunningVehicleNo() < myMaxVehicleNumber)
            && edge.insertVehicle(*veh, time, false, myEagerInsertionCheck || veh->getParameter().departProcedure == DepartDefinition::SPLIT)) {
        // Successful insertion
        return 1;
    }
    if (myMaxDepartDelay >= 0 && time - veh->getParameter().depart > myMaxDepartDelay) {
        // remove vehicles waiting too long for departure
        myVehicleControl.deleteVehicle(veh, true);
    } else if (edge.isVaporizing()) {
        // remove vehicles if the edge shall be empty
        myVehicleControl.deleteVehicle(veh, true);
    } else if (myAbortedEmits.count(veh) > 0) {
        // remove vehicles which shall not be inserted for some reason
        myAbortedEmits.erase(veh);
        myVehicleControl.deleteVehicle(veh, true);
    } else if ((veh->getRouteValidity(false) & (
                    MSBaseVehicle::ROUTE_START_INVALID_LANE
                    | MSBaseVehicle::ROUTE_START_INVALID_PERMISSIONS)) != 0) {
        myVehicleControl.deleteVehicle(veh, true);
    } else {
        // let the vehicle wait one step, we'll retry then
        refusedEmits.push_back(veh);
    }
    edge.setLastFailedInsertionTime(time);
    return 0;
}


void
MSInsertionControl::checkCandidates(SUMOTime time, const bool preCheck) {
    while (myAllVeh.anyWaitingBefore(time)) {
        const MSVehicleContainer::VehicleVector& top = myAllVeh.top();
        copy(top.begin(), top.end(), back_inserter(myPendingEmits));
        myAllVeh.pop();
    }
    if (preCheck) {
        MSVehicleContainer::VehicleVector::const_iterator veh;
        for (veh = myPendingEmits.begin(); veh != myPendingEmits.end(); veh++) {
            SUMOVehicle* const v = *veh;
            const MSEdge* const edge = v->getEdge();
            if (edge->insertVehicle(*v, time, true, myEagerInsertionCheck)) {
                myEmitCandidates.insert(v);
            } else {
                MSDevice_Routing* dev = static_cast<MSDevice_Routing*>(v->getDevice(typeid(MSDevice_Routing)));
                if (dev != nullptr) {
                    dev->skipRouting(time);
                }
            }
        }
    }
}


void
MSInsertionControl::determineCandidates(SUMOTime time) {
    MSVehicleControl& vehControl = MSNet::getInstance()->getVehicleControl();
    // for equidistant vehicles, up-scaling is done via repetitionOffset
    for (std::vector<Flow>::iterator i = myFlows.begin(); i != myFlows.end();) {
        MSVehicleType* vtype = nullptr;
        SUMOVehicleParameter* pars = i->pars;
        double typeScale = i->scale;
        if (typeScale < 0) {
            // must sample from distribution to determine scale value
            vtype = vehControl.getVType(pars->vtypeid, MSRouteHandler::getParsingRNG());
            typeScale = vtype->getParameter().scale;
        }
        double scale = vehControl.getScale() * typeScale;
        bool tryEmitByProb = pars->repetitionProbability > 0;
        while (scale > 0 && ((pars->repetitionProbability < 0
                              && pars->repetitionsDone < pars->repetitionNumber * scale
                              && pars->depart + pars->repetitionTotalOffset <= time)
                             || (tryEmitByProb
                                 && pars->depart <= time
                                 && pars->repetitionEnd > time
                                 // only call rand if all other conditions are met
                                 && RandHelper::rand(&myFlowRNG) < (pars->repetitionProbability * TS))
                            )) {
            tryEmitByProb = false; // only emit one per step
            SUMOVehicleParameter* newPars = new SUMOVehicleParameter(*pars);
            newPars->id = pars->id + "." + toString(i->index);
            newPars->depart = pars->repetitionProbability > 0 ? time : pars->depart + pars->repetitionTotalOffset + computeRandomDepartOffset();
            pars->incrementFlow(scale, &myFlowRNG);
            myFlowIDs[pars->id] = i->index;
            //std::cout << SIMTIME << " flow=" << pars->id << " done=" << pars->repetitionsDone << " totalOffset=" << STEPS2TIME(pars->repetitionTotalOffset) << "\n";
            // try to build the vehicle
            if (vehControl.getVehicle(newPars->id) == nullptr) {
                ConstMSRoutePtr const route = MSRoute::dictionary(pars->routeid);
                if (vtype == nullptr) {
                    vtype = vehControl.getVType(pars->vtypeid, MSRouteHandler::getParsingRNG());
                }
                SUMOVehicle* const vehicle = vehControl.buildVehicle(newPars, route, vtype, !MSGlobals::gCheckRoutes);
                // for equidistant vehicles, all scaling is done via repetitionOffset (to avoid artefacts, #11441)
                // for probabilistic vehicles, we use the quota
                int quota = pars->repetitionProbability < 0 ? 1 : vehControl.getQuota(scale);
                if (quota > 0) {
                    vehControl.addVehicle(newPars->id, vehicle);
                    if (pars->departProcedure == DepartDefinition::GIVEN || pars->departProcedure == DepartDefinition::BEGIN) {
                        add(vehicle);
                    }
                    i->index++;
                    while (--quota > 0) {
                        SUMOVehicleParameter* const quotaPars = new SUMOVehicleParameter(*pars);
                        quotaPars->id = pars->id + "." + toString(i->index);
                        quotaPars->depart = pars->repetitionProbability > 0 ? time :
                                            pars->depart + pars->repetitionsDone * pars->repetitionTotalOffset + computeRandomDepartOffset();
                        SUMOVehicle* const quotaVehicle = vehControl.buildVehicle(quotaPars, route, vtype, !MSGlobals::gCheckRoutes);
                        vehControl.addVehicle(quotaPars->id, quotaVehicle);
                        if (pars->departProcedure == DepartDefinition::GIVEN || pars->departProcedure == DepartDefinition::BEGIN) {
                            add(quotaVehicle);
                        }
                        pars->repetitionsDone++;
                        i->index++;
                    }
                } else {
                    vehControl.deleteVehicle(vehicle, true);
                }
            } else {
                if (MSGlobals::gStateLoaded) {
                    /// @note probably obsolete since flows save their state
                    break;
                }
                throw ProcessError(TLF("Another vehicle with the id '%' exists.", newPars->id));
            }
            vtype = nullptr;
        }
        if (time >= pars->repetitionEnd ||
                (pars->repetitionNumber != std::numeric_limits<int>::max()
                 && pars->repetitionsDone >= (int)(pars->repetitionNumber * scale + 0.5))) {
            i = myFlows.erase(i);
            MSRoute::checkDist(pars->routeid);
            delete pars;
        } else {
            ++i;
        }
    }
    checkCandidates(time, MSRoutingEngine::isEnabled());
}


int
MSInsertionControl::getWaitingVehicleNo() const {
    return (int)myPendingEmits.size();
}


int
MSInsertionControl::getPendingFlowCount() const {
    return (int)myFlows.size();
}


void
MSInsertionControl::descheduleDeparture(const SUMOVehicle* veh) {
    myAbortedEmits.insert(veh);
}

void
MSInsertionControl::retractDescheduleDeparture(const SUMOVehicle* veh) {
    myAbortedEmits.erase(veh);
}


void
MSInsertionControl::alreadyDeparted(SUMOVehicle* veh) {
    myPendingEmits.erase(std::remove(myPendingEmits.begin(), myPendingEmits.end(), veh), myPendingEmits.end());
    myAllVeh.remove(veh);
}


void
MSInsertionControl::clearPendingVehicles(const std::string& route) {
    //clear out the refused vehicle list, deleting the vehicles entirely
    MSVehicleContainer::VehicleVector::iterator veh;
    for (veh = myPendingEmits.begin(); veh != myPendingEmits.end();) {
        if ((*veh)->getRoute().getID() == route || route == "") {
            myVehicleControl.deleteVehicle(*veh, true);
            veh = myPendingEmits.erase(veh);
        } else {
            ++veh;
        }
    }
}


int
MSInsertionControl::getPendingEmits(const MSLane* lane) {
    if (MSNet::getInstance()->getCurrentTimeStep() != myPendingEmitsUpdateTime) {
        // updated pending emits (only once per time step)
        myPendingEmitsForLane.clear();
        for (const SUMOVehicle* const veh : myPendingEmits) {
            const MSLane* const vlane = veh->getLane();
            if (vlane != nullptr) {
                myPendingEmitsForLane[vlane]++;
            } else {
                // no (tentative) departLane was set, increase count for all
                // lanes of the depart edge
                for (const MSLane* const l : veh->getEdge()->getLanes()) {
                    myPendingEmitsForLane[l]++;
                }
            }
        }
        myPendingEmitsUpdateTime = MSNet::getInstance()->getCurrentTimeStep();
    }
    return myPendingEmitsForLane[lane];
}


void
MSInsertionControl::adaptIntermodalRouter(MSTransportableRouter& router) const {
    // fill the public transport router with pre-parsed public transport lines
    for (const Flow& f : myFlows) {
        if (f.pars->line != "") {
            ConstMSRoutePtr const route = MSRoute::dictionary(f.pars->routeid);
            router.getNetwork()->addSchedule(*f.pars, route == nullptr ? nullptr : &route->getStops());
        }
    }
}


void
MSInsertionControl::saveState(OutputDevice& out) {
    // save flow states
    for (const Flow& flow : myFlows) {
        flow.pars->write(out, OptionsCont::getOptions(), SUMO_TAG_FLOWSTATE,
                         flow.pars->vtypeid == DEFAULT_VTYPE_ID ? "" : flow.pars->vtypeid);
        if (flow.pars->repetitionEnd == SUMOTime_MAX) {
            out.writeAttr(SUMO_ATTR_NUMBER, flow.pars->repetitionNumber);
        }
        if (flow.pars->repetitionProbability > 0) {
            out.writeAttr(SUMO_ATTR_PROB, flow.pars->repetitionProbability);
        } else if (flow.pars->poissonRate > 0) {
            out.writeAttr(SUMO_ATTR_PERIOD, "exp(" + toString(flow.pars->poissonRate) + ")");
            out.writeAttr(SUMO_ATTR_NEXT, STEPS2TIME(flow.pars->repetitionTotalOffset));
        } else {
            out.writeAttr(SUMO_ATTR_PERIOD, STEPS2TIME(flow.pars->repetitionOffset));
            out.writeAttr(SUMO_ATTR_NEXT, STEPS2TIME(flow.pars->repetitionTotalOffset));
        }
        if (flow.pars->repetitionEnd != SUMOTime_MAX) {
            out.writeAttr(SUMO_ATTR_END, STEPS2TIME(flow.pars->repetitionEnd));
        };
        out.writeAttr(SUMO_ATTR_ROUTE, flow.pars->routeid);
        out.writeAttr(SUMO_ATTR_DONE, flow.pars->repetitionsDone);
        out.writeAttr(SUMO_ATTR_INDEX, flow.index);
        if (flow.pars->wasSet(VEHPARS_FORCE_REROUTE)) {
            out.writeAttr(SUMO_ATTR_REROUTE, true);
        }
        out.closeTag();
    }
}


void
MSInsertionControl::clearState() {
    for (const Flow& f : myFlows) {
        delete (f.pars);
    }
    myFlows.clear();
    myFlowIDs.clear();
    myAllVeh.clearState();
    myPendingEmits.clear();
    myEmitCandidates.clear();
    myAbortedEmits.clear();
    // myPendingEmitsForLane must not be cleared since it updates itself on the next call
}


SUMOTime
MSInsertionControl::computeRandomDepartOffset() const {
    if (myMaxRandomDepartOffset > 0) {
        // round to the closest usable simulation step
        return DELTA_T * ((RandHelper::rand(myMaxRandomDepartOffset, MSRouteHandler::getParsingRNG()) + DELTA_T / 2) / DELTA_T);
    }
    return 0;
}

const SUMOVehicleParameter*
MSInsertionControl::getFlowPars(const std::string& id) const {
    if (hasFlow(id)) {
        for (const Flow& f : myFlows) {
            if (f.pars->id == id) {
                return f.pars;
            }
        }
    }
    return nullptr;
}

SUMOVehicle*
MSInsertionControl::getLastFlowVehicle(const std::string& id) const {
    const auto it = myFlowIDs.find(id);
    if (it != myFlowIDs.end()) {
        const std::string vehID = id + "." + toString(it->second);
        return MSNet::getInstance()->getVehicleControl().getVehicle(vehID);
    }
    return nullptr;
}

/****************************************************************************/
