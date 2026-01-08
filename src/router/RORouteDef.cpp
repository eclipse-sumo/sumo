/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2025 German Aerospace Center (DLR) and others.
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
/// @file    RORouteDef.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Sept 2002
///
// Base class for a vehicle's route definition
/****************************************************************************/
#include <config.h>

#include <string>
#include <iterator>
#include <algorithm>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/Named.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include "ROEdge.h"
#include "RORoute.h"
#include <utils/router/SUMOAbstractRouter.h>
#include <utils/router/RouteCostCalculator.h>
#include "RORouteDef.h"
#include "ROVehicle.h"

// ===========================================================================
// static members
// ===========================================================================
bool RORouteDef::myUsingJTRR(false);
bool RORouteDef::mySkipNewRoutes(false);

// ===========================================================================
// method definitions
// ===========================================================================
RORouteDef::RORouteDef(const std::string& id, const int lastUsed,
                       const bool tryRepair, const bool mayBeDisconnected) :
    Named(StringUtils::convertUmlaute(id)),
    myPrecomputed(nullptr), myLastUsed(lastUsed), myTryRepair(tryRepair),
    myMayBeDisconnected(mayBeDisconnected),
    myDiscardSilent(false) {
}


RORouteDef::~RORouteDef() {
    for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        if (myRouteRefs.count(*i) == 0) {
            delete *i;
        }
    }
}


void
RORouteDef::addLoadedAlternative(RORoute* alt) {
    myAlternatives.push_back(alt);
}


void
RORouteDef::addAlternativeDef(const RORouteDef* alt) {
    std::copy(alt->myAlternatives.begin(), alt->myAlternatives.end(),
              back_inserter(myAlternatives));
    std::copy(alt->myAlternatives.begin(), alt->myAlternatives.end(),
              std::inserter(myRouteRefs, myRouteRefs.end()));
}


RORoute*
RORouteDef::buildCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                              SUMOTime begin, const ROVehicle& veh) const {
    if (myPrecomputed == nullptr) {
        preComputeCurrentRoute(router, begin, veh);
    }
    return myPrecomputed;
}


void
RORouteDef::validateAlternatives(const ROVehicle* veh, MsgHandler* errorHandler) {
    for (int i = 0; i < (int)myAlternatives.size();) {
        if (i != myLastUsed || mySkipNewRoutes) {
            if (myAlternatives[i]->isPermitted(veh, errorHandler)) {
                i++;
            } else {
                myAlternatives.erase(myAlternatives.begin() + i); 
                if (myLastUsed > i) {
                    myLastUsed--;
                }
            }
        } else {
            i++;
        }
    }
}


void
RORouteDef::preComputeCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                                   SUMOTime begin, const ROVehicle& veh) const {
    myNewRoute = false;
    const OptionsCont& oc = OptionsCont::getOptions();
    const bool ignoreErrors = oc.getBool("ignore-errors");
    const bool hasRestrictions = RONet::getInstance()->hasParamRestrictions();
    assert(myAlternatives[0]->getEdgeVector().size() > 0);
    MsgHandler* mh = ignoreErrors ? MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance();
    if (myAlternatives[0]->getFirst()->prohibits(&veh, hasRestrictions) && (!oc.getBool("repair.from")
            // do not try to reassign starting edge for trip input
            || myMayBeDisconnected || myAlternatives[0]->getEdgeVector().size() < 2)) {
        mh->inform("Vehicle '" + veh.getID() + "' is not allowed to depart on edge '" +
                   myAlternatives[0]->getFirst()->getID() + "'.");
        return;
    } else if (myAlternatives[0]->getLast()->prohibits(&veh, hasRestrictions) && (!oc.getBool("repair.to")
               // do not try to reassign destination edge for trip input
               || myMayBeDisconnected || myAlternatives[0]->getEdgeVector().size() < 2)) {
        // this check is not strictly necessary unless myTryRepair is set.
        // However, the error message is more helpful than "no connection found"
        mh->inform("Vehicle '" + veh.getID() + "' is not allowed to arrive on edge '" +
                   myAlternatives[0]->getLast()->getID() + "'.");
        return;
    }
    const bool skipTripRouting = (oc.exists("write-trips") && oc.getBool("write-trips")
                                  && RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().skipRouteCalculation());
    if ((myTryRepair && !skipTripRouting) || myUsingJTRR) {
        ConstROEdgeVector newEdges;
        if (repairCurrentRoute(router, begin, veh, myAlternatives[0]->getEdgeVector(), newEdges)) {
            if (myAlternatives[0]->getEdgeVector() != newEdges) {
                if (!myMayBeDisconnected) {
                    WRITE_WARNINGF(TL("Repaired route of vehicle '%'."), veh.getID());
                }
                myNewRoute = true;
                RGBColor* col = myAlternatives[0]->getColor() != nullptr ? new RGBColor(*myAlternatives[0]->getColor()) : nullptr;
                myPrecomputed = new RORoute(myID, 0, myAlternatives[0]->getProbability(), newEdges, col, myAlternatives[0]->getStops());
            } else {
                myPrecomputed = myAlternatives[0];
            }
        }
        return;
    }
    if ((RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().skipRouteCalculation()
            || OptionsCont::getOptions().getBool("remove-loops"))
            && (skipTripRouting || myAlternatives[myLastUsed]->isValid(veh, ignoreErrors))) {
        myPrecomputed = myAlternatives[myLastUsed];
    } else {
        // build a new route to test whether it is better
        ConstROEdgeVector oldEdges({getOrigin(), getDestination()});
        ConstROEdgeVector edges;
        if (repairCurrentRoute(router, begin, veh, oldEdges, edges, true)) {
            if (edges.front()->isTazConnector()) {
                edges.erase(edges.begin());
            }
            if (edges.back()->isTazConnector()) {
                edges.pop_back();
            }
            // check whether the same route was already used
            int existing = -1;
            for (int i = 0; i < (int)myAlternatives.size(); i++) {
                if (edges == myAlternatives[i]->getEdgeVector()) {
                    existing = i;
                    break;
                }
            }
            if (existing >= 0) {
                myPrecomputed = myAlternatives[existing];
            } else {
                RGBColor* col = myAlternatives[0]->getColor() != nullptr ? new RGBColor(*myAlternatives[0]->getColor()) : nullptr;
                myPrecomputed = new RORoute(myID, 0, 1, edges, col, myAlternatives[0]->getStops());
                myNewRoute = true;
            }
        }
    }
}


bool
RORouteDef::repairCurrentRoute(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                               SUMOTime begin, const ROVehicle& veh,
                               ConstROEdgeVector oldEdges, ConstROEdgeVector& newEdges,
                               bool isTrip) const {
    MsgHandler* mh = (OptionsCont::getOptions().getBool("ignore-errors") ?
                      MsgHandler::getWarningInstance() : MsgHandler::getErrorInstance());
    RONet* net = RONet::getInstance();
    const int initialSize = (int)oldEdges.size();
    const bool hasRestrictions = RONet::getInstance()->hasParamRestrictions();
    if (net->getProhibitions().size() > 0 && !router.hasProhibitions()) {
        router.prohibit(net->getProhibitions());
    }
    net->updateLaneProhibitions(begin);
    if (initialSize == 1) {
        if (myUsingJTRR) {
            /// only ROJTRRouter is supposed to handle this type of input
            bool ok = router.compute(oldEdges.front(), nullptr, &veh, begin, newEdges);
            myDiscardSilent = ok && newEdges.size() == 0;
        } else {
            newEdges = oldEdges;
        }
    } else {
        if (oldEdges.front()->prohibits(&veh, hasRestrictions)) {
            // option repair.from is in effect
            const std::string& frontID = oldEdges.front()->getID();
            for (ConstROEdgeVector::iterator i = oldEdges.begin(); i != oldEdges.end();) {
                if ((*i)->prohibits(&veh, hasRestrictions) || (*i)->isInternal()) {
                    i = oldEdges.erase(i);
                } else {
                    WRITE_MESSAGE("Changing invalid starting edge '" + frontID
                                  + "' to '" + (*i)->getID() + "' for vehicle '" + veh.getID() + "'.");
                    break;
                }
            }
        }
        if (oldEdges.size() == 0) {
            mh->inform("Could not find new starting edge for vehicle '" + veh.getID() + "'.");
            return false;
        }
        if (oldEdges.back()->prohibits(&veh, hasRestrictions)) {
            // option repair.to is in effect
            const std::string& backID = oldEdges.back()->getID();
            // oldEdges cannot get empty here, otherwise we would have left the stage when checking "from"
            while (oldEdges.back()->prohibits(&veh, hasRestrictions) || oldEdges.back()->isInternal()) {
                oldEdges.pop_back();
            }
            WRITE_MESSAGE("Changing invalid destination edge '" + backID
                          + "' to edge '" + oldEdges.back()->getID() + "' for vehicle '" + veh.getID() + "'.");
        }
        std::vector<ROVehicle::Mandatory> mandatory = veh.getMandatoryEdges(oldEdges.front(), oldEdges.back());
        assert(mandatory.size() >= 2);
        // removed prohibited
        for (ConstROEdgeVector::iterator i = oldEdges.begin(); i != oldEdges.end();) {
            if ((*i)->prohibits(&veh, hasRestrictions) || (*i)->isInternal()) {
                // no need to check the mandatories here, this was done before
                WRITE_MESSAGEF(TL("Removing invalid edge '%' from route for vehicle '%'."), (*i)->getID(), veh.getID());
                i = oldEdges.erase(i);
            } else {
                ++i;
            }
        }
        // reconnect remaining edges
        if (mandatory.size() > oldEdges.size() && initialSize > 2) {
            WRITE_MESSAGEF(TL("There are stop edges which were not part of the original route for vehicle '%'."), veh.getID());
        }
        ConstROEdgeVector targets;
        bool checkPositions = false;
        if (mandatory.size() >= oldEdges.size()) {
            for (auto m : mandatory) {
                targets.push_back(m.edge);
                checkPositions |= m.pos >= 0;
            }
        } else {
            targets = oldEdges;
        }
        newEdges.push_back(targets.front());
        auto nextMandatory = mandatory.begin() + 1;
        int lastMandatory = 0;
        for (ConstROEdgeVector::const_iterator i = targets.begin() + 1;
                i != targets.end() && nextMandatory != mandatory.end(); ++i) {
            const ROEdge* prev = *(i - 1);
            const ROEdge* cur = *i;
            if (prev->isConnectedTo(*cur, veh.getVClass()) && (!isRailway(veh.getVClass()) || prev->getBidiEdge() != cur)) {
                newEdges.push_back(cur);
            } else {
                if (initialSize > 2) {
                    // only inform if the input is (probably) not a trip
                    WRITE_MESSAGEF(TL("Edge '%' not connected to edge '%' for vehicle '%'."), (*(i - 1))->getID(), (*i)->getID(), veh.getID());
                }
                const ROEdge* last = newEdges.back();
                newEdges.pop_back();
                if (last->isTazConnector() && newEdges.size() > 1) {
                    // assume this was a viaTaz
                    last = newEdges.back();
                    newEdges.pop_back();
                }
                if (veh.hasJumps() && (nextMandatory - 1)->isJump) {
                    while (*i != nextMandatory->edge) {
                        ++i;
                    }
                    newEdges.push_back(last);
                    newEdges.push_back(*i);
                    //std::cout << " skipJump mIndex=" << (nextMandatory - 1 - mandatory.begin()) << " last=" << last->getID() << " next=" << (*i)->getID() << " newEdges=" << toString(newEdges) << "\n";
                } else {

                    int numEdgesBefore = (int)newEdges.size();
                    //                router.setHint(targets.begin(), i, &veh, begin);
                    if (myTryRepair && lastMandatory < (int)newEdges.size() && last != newEdges[lastMandatory]) {
                        router.setMsgHandler(MsgHandler::getWarningInstance());
                    }
                    bool ok;
                    if (checkPositions
                            && mandatory[i - targets.begin() - 1].pos >= 0
                            && mandatory[i - targets.begin()].pos >= 0) {
                        ok = router.compute(
                                last, mandatory[i - targets.begin() - 1].pos,
                                *i, mandatory[i - targets.begin()].pos,
                                &veh, begin, newEdges);
                    } else {
                        ok = router.compute(last, *i, &veh, begin, newEdges);
                    }
                    router.setMsgHandler(mh);
                    if (!ok) {
                        // backtrack: try to route from last mandatory edge to next mandatory edge
                        // XXX add option for backtracking in smaller increments
                        // (i.e. previous edge to edge after *i)
                        // we would then need to decide whether we have found a good
                        // tradeoff between faithfulness to the input data and detour-length
                        if (lastMandatory >= (int)newEdges.size() || last == newEdges[lastMandatory] || !backTrack(router, i, lastMandatory, nextMandatory->edge, newEdges, veh, begin)) {
                            mh->inform("Mandatory edge '" + (*i)->getID() + "' not reachable by vehicle '" + veh.getID() + "'.");
                            return false;
                        }
                    } else if (!myMayBeDisconnected && !isTrip && last != (*i)) {
                        double airDist = last->getToJunction()->getPosition().distanceTo(
                                             (*i)->getFromJunction()->getPosition());
                        double repairDist = 0;
                        for (auto it2 = (newEdges.begin() + numEdgesBefore + 1); it2 != newEdges.end() && it2 != newEdges.end() - 1; it2++) {
                            repairDist += (*it2)->getLength();
                        }
                        const double detourFactor = repairDist / MAX2(airDist, 1.0);
                        const double detour = MAX2(0.0, repairDist - airDist);
                        const double maxDetourFactor = OptionsCont::getOptions().getFloat("repair.max-detour-factor");
                        if (detourFactor > maxDetourFactor) {
                            WRITE_MESSAGEF("    Backtracking to avoid detour of %m for gap of %m)", detour, airDist);
                            backTrack(router, i, lastMandatory, nextMandatory->edge, newEdges, veh, begin);
                        } else if (detourFactor > 1.1) {
                            WRITE_MESSAGEF("    Taking detour of %m to avoid gap of %m)", detour, airDist);
                        }
                    }
                }
            }
            if (*i == nextMandatory->edge) {
                nextMandatory++;
                lastMandatory = (int)newEdges.size() - 1;
            }
        }
        if (veh.getParameter().via.size() > 0 && veh.getParameter().stops.size() > 0) {
            // check consistency of stops and vias
           auto it = newEdges.begin(); 
           for (const auto& stop : veh.getParameter().stops) {
               const ROEdge* e = net->getEdge(stop.edge);
               it = std::find(it, newEdges.end(), e);
               if (it == newEdges.end()) {
                   mh->inform("Stop edge '" + e->getID() + "' is inconsistent with via edges for vehicle '" + veh.getID() + "'.");
                   return false;
               }
           }
        }
    }
    return true;
}


bool
RORouteDef::backTrack(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                      ConstROEdgeVector::const_iterator& i, int lastMandatory, const ROEdge* nextMandatory,
                      ConstROEdgeVector& newEdges, const ROVehicle& veh, SUMOTime begin) {
    ConstROEdgeVector edges;
    bool ok = router.compute(newEdges[lastMandatory], nextMandatory, &veh, begin, edges, true);
    if (!ok) {
        return false;
    }

    while (*i != nextMandatory) {
        ++i;
    }
    newEdges.erase(newEdges.begin() + lastMandatory + 1, newEdges.end());
    std::copy(edges.begin() + 1, edges.end(), back_inserter(newEdges));
    return true;
}


void
RORouteDef::addAlternative(SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                           const ROVehicle* const veh, RORoute* current, SUMOTime begin,
                           MsgHandler* errorHandler) {
    if (myTryRepair || myUsingJTRR) {
        if (myNewRoute) {
            delete myAlternatives[0];
            myAlternatives[0] = current;
        }
        if (!router.isValid(current->getEdgeVector(), veh, STEPS2TIME(begin))) {
            throw ProcessError("Route '" + getID() + "' (vehicle '" + veh->getID() + "') is not valid.");
        }
        double costs = router.recomputeCosts(current->getEdgeVector(), veh, begin);
        if (veh->hasJumps()) {
            // @todo: jumpTime should be applied in recomputeCost to ensure the
            // correctness of time-dependent traveltimes
            costs += STEPS2TIME(veh->getJumpTime());
        }
        current->setCosts(costs);
        return;
    }
    // add the route when it's new
    if (myAlternatives.back()->getProbability() < 0 || !myAlternatives.back()->isPermitted(veh, errorHandler)) {
        if (myAlternatives.back()->getProbability() >= 0 && errorHandler == MsgHandler::getErrorInstance()) {
            throw ProcessError("Route '" + current->getID() + "' (vehicle '" + veh->getID() + "') is not valid.");
        }
        delete myAlternatives.back();
        myAlternatives.pop_back();
    }
    if (myNewRoute) {
        myAlternatives.push_back(current);
    }
    // recompute the costs and (when a new route was added) scale the probabilities
    const double scale = double(myAlternatives.size() - 1) / double(myAlternatives.size());
    for (RORoute* const alt : myAlternatives) {
        if (!router.isValid(alt->getEdgeVector(), veh, STEPS2TIME(begin))) {
            throw ProcessError("Route '" + current->getID() + "' (vehicle '" + veh->getID() + "') is not valid.");
        }
        // recompute the costs for all routes
        const double newCosts = router.recomputeCosts(alt->getEdgeVector(), veh, begin);
        assert(myAlternatives.size() != 0);
        if (myNewRoute) {
            if (alt == current) {
                // set initial probability and costs
                alt->setProbability(1. / (double) myAlternatives.size());
                alt->setCosts(newCosts);
            } else {
                // rescale probs for all others
                alt->setProbability(alt->getProbability() * scale);
            }
        }
        RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().setCosts(alt, newCosts, alt == myAlternatives[myLastUsed]);
    }
    assert(myAlternatives.size() != 0);
    RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().calculateProbabilities(myAlternatives, veh, veh->getDepartureTime());
    const bool keepRoute = RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().keepRoute();
    if (!RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().keepAllRoutes() && !keepRoute) {
        // remove with probability of 0 (not mentioned in Gawron)
        for (std::vector<RORoute*>::iterator i = myAlternatives.begin(); i != myAlternatives.end();) {
            if ((*i)->getProbability() == 0) {
                delete *i;
                i = myAlternatives.erase(i);
            } else {
                i++;
            }
        }
    }
    int maxNumber = RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator().getMaxRouteNumber();
    if ((int)myAlternatives.size() > maxNumber) {
        const RORoute* last = myAlternatives[myLastUsed];
        // only keep the routes with highest probability
        sort(myAlternatives.begin(), myAlternatives.end(), [](const RORoute * const a, const RORoute * const b) {
            return a->getProbability() > b->getProbability();
        });
        if (keepRoute) {
            for (int i = 0; i < (int)myAlternatives.size(); i++) {
                if (myAlternatives[i] == last) {
                    myLastUsed = i;
                    break;
                }
            }
            if (myLastUsed >= maxNumber) {
                std::swap(myAlternatives[maxNumber - 1], myAlternatives[myLastUsed]);
                myLastUsed = maxNumber - 1;
            }
        }
        for (std::vector<RORoute*>::iterator i = myAlternatives.begin() + maxNumber; i != myAlternatives.end(); i++) {
            delete *i;
        }
        myAlternatives.erase(myAlternatives.begin() + maxNumber, myAlternatives.end());
    }
    // rescale probabilities
    double newSum = 0.;
    for (const RORoute* const alt : myAlternatives) {
        newSum += alt->getProbability();
    }
    assert(newSum > 0);
    // @note newSum may be larger than 1 for numerical reasons
    for (RORoute* const alt : myAlternatives) {
        alt->setProbability(alt->getProbability() / newSum);
    }

    // find the route to use
    if (!keepRoute) {
        double chosen = RandHelper::rand();
        myLastUsed = 0;
        for (const RORoute* const alt : myAlternatives) {
            chosen -= alt->getProbability();
            if (chosen <= 0) {
                return;
            }
            myLastUsed++;
        }
    }
}

const ROEdge*
RORouteDef::getOrigin() const {
    return myAlternatives.back()->getFirst();
}


const ROEdge*
RORouteDef::getDestination() const {
    return myAlternatives.back()->getLast();
}


OutputDevice&
RORouteDef::writeXMLDefinition(OutputDevice& dev, const ROVehicle* const veh,
                               bool asAlternatives, bool withExitTimes, bool withCost, bool withLength) const {
    if (asAlternatives) {
        dev.openTag(SUMO_TAG_ROUTE_DISTRIBUTION).writeAttr(SUMO_ATTR_LAST, myLastUsed);
        for (int i = 0; i != (int)myAlternatives.size(); i++) {
            myAlternatives[i]->writeXMLDefinition(dev, veh, true, true, withExitTimes, withLength);
        }
        dev.closeTag();
        return dev;
    } else {
        return myAlternatives[myLastUsed]->writeXMLDefinition(dev, veh, withCost, false, withExitTimes, withLength);
    }
}


RORouteDef*
RORouteDef::copy(const std::string& id, const SUMOTime stopOffset) const {
    RORouteDef* result = new RORouteDef(id, 0, myTryRepair, myMayBeDisconnected);
    for (const RORoute* const route : myAlternatives) {
        RGBColor* col = route->getColor() != nullptr ? new RGBColor(*route->getColor()) : nullptr;
        RORoute* newRoute = new RORoute(id, route->getCosts(), route->getProbability(), route->getEdgeVector(), col, route->getStops());
        newRoute->addStopOffset(stopOffset);
        result->addLoadedAlternative(newRoute);
    }
    return result;
}


double
RORouteDef::getOverallProb() const {
    double sum = 0.;
    for (std::vector<RORoute*>::const_iterator i = myAlternatives.begin(); i != myAlternatives.end(); i++) {
        sum += (*i)->getProbability();
    }
    return sum;
}


/****************************************************************************/
