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
/// @file    ROMAAssignments.cpp
/// @author  Yun-Pang Floetteroed
/// @author  Laura Bieker
/// @author  Michael Behrisch
/// @date    Feb 2013
///
// Assignment methods
/****************************************************************************/
#include <config.h>

#include <vector>
#include <algorithm>
#include <utils/common/SUMOTime.h>
#include <utils/distribution/Distribution_Points.h>
#include <utils/router/RouteCostCalculator.h>
#include <utils/router/SUMOAbstractRouter.h>
#include <router/ROEdge.h>
#include <router/RONet.h>
#include <router/RORoute.h>
#include <od/ODMatrix.h>
#include "ROMAEdge.h"
#include "ROMAAssignments.h"


// ===========================================================================
// static member variables
// ===========================================================================
thread_local std::unordered_map<const ROEdge*, double> ROMAAssignments::myPenalties;


// ===========================================================================
// method definitions
// ===========================================================================

ROMAAssignments::ROMAAssignments(const SUMOTime begin, const SUMOTime end, const bool additiveTraffic,
                                 const double adaptionFactor, const int maxAlternatives, const bool defaultCapacities,
                                 RONet& net, ODMatrix& matrix, SUMOAbstractRouter<ROEdge, ROVehicle>& router,
                                 OutputDevice* netloadOutput)
    : myBegin(begin), myEnd(end), myAdditiveTraffic(additiveTraffic), myAdaptionFactor(adaptionFactor),
      myMaxAlternatives(maxAlternatives), myUseDefaultCapacities(defaultCapacities),
      myNet(net), myMatrix(matrix), myRouter(router), myNetloadOutput(netloadOutput) {
    myDefaultVehicle = new ROVehicle(SUMOVehicleParameter(), nullptr, net.getVehicleTypeSecure(DEFAULT_VTYPE_ID), &net);
}


ROMAAssignments::~ROMAAssignments() {
    delete myDefaultVehicle;
}

// based on the definitions in PTV-Validate and in the VISUM-Cologne network
double
ROMAAssignments::getCapacity(const ROEdge* edge) const {
    if (edge->isTazConnector()) {
        return 0;
    }
    const int roadClass = myUseDefaultCapacities ? -1 : -edge->getPriority();
    // TODO: differ road class 1 from the unknown road class 1!!!
    if (edge->getNumLanes() == 0) {
        // TAZ have no cost
        return 0;
    } else if (roadClass == 0 || roadClass == 1)  {
        return edge->getNumLanes() * 2000.; //CR13 in table.py
    } else if (roadClass == 2 && edge->getSpeedLimit() <= 11.) {
        return edge->getNumLanes() * 1333.33; //CR5 in table.py
    } else if (roadClass == 2 && edge->getSpeedLimit() > 11. && edge->getSpeedLimit() <= 16.) {
        return edge->getNumLanes() * 1500.; //CR3 in table.py
    } else if (roadClass == 2 && edge->getSpeedLimit() > 16.) {
        return edge->getNumLanes() * 2000.; //CR13 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() <= 11.) {
        return edge->getNumLanes() * 800.; //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() > 11. && edge->getSpeedLimit() <= 13.) {
        return edge->getNumLanes() * 875.; //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() > 13. && edge->getSpeedLimit() <= 16.) {
        return edge->getNumLanes() * 1500.; //CR4 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() > 16.) {
        return edge->getNumLanes() * 1800.; //CR13 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() <= 5.) {
        return edge->getNumLanes() * 200.; //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 5. && edge->getSpeedLimit() <= 7.) {
        return edge->getNumLanes() * 412.5; //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 7. && edge->getSpeedLimit() <= 9.) {
        return edge->getNumLanes() * 600.; //CR6 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 9. && edge->getSpeedLimit() <= 11.) {
        return edge->getNumLanes() * 800.; //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 11. && edge->getSpeedLimit() <= 13.) {
        return edge->getNumLanes() * 1125.; //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 13. && edge->getSpeedLimit() <= 16.) {
        return edge->getNumLanes() * 1583.; //CR4 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 16. && edge->getSpeedLimit() <= 18.) {
        return edge->getNumLanes() * 1100.; //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 18. && edge->getSpeedLimit() <= 22.) {
        return edge->getNumLanes() * 1200.; //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 22. && edge->getSpeedLimit() <= 26.) {
        return edge->getNumLanes() * 1300.; //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 26.) {
        return edge->getNumLanes() * 1400.; //CR3 in table.py
    }
    return edge->getNumLanes() * 800.; //CR5 in table.py
}


// based on the definitions in PTV-Validate and in the VISUM-Cologne network
double
ROMAAssignments::capacityConstraintFunction(const ROEdge* edge, const double flow) const {
    if (edge->isTazConnector()) {
        return 0;
    }
    const int roadClass = myUseDefaultCapacities ? -1 : -edge->getPriority();
    const double capacity = getCapacity(edge);
    // TODO: differ road class 1 from the unknown road class 1!!!
    if (edge->getNumLanes() == 0) {
        // TAZ have no cost
        return 0;
    } else if (roadClass == 0 || roadClass == 1)  {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.3)) * 2.); //CR13 in table.py
    } else if (roadClass == 2 && edge->getSpeedLimit() <= 11.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 2 && edge->getSpeedLimit() > 11. && edge->getSpeedLimit() <= 16.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if (roadClass == 2 && edge->getSpeedLimit() > 16.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.3)) * 2.); //CR13 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() <= 11.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() > 11. && edge->getSpeedLimit() <= 13.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() > 13. && edge->getSpeedLimit() <= 16.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.7 * (flow / (capacity * 1.)) * 2.); //CR4 in table.py
    } else if (roadClass == 3 && edge->getSpeedLimit() > 16.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.3)) * 2.); //CR13 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() <= 5.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.5)) * 3.); //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 5. && edge->getSpeedLimit() <= 7.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.5)) * 3.); //CR7 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 7. && edge->getSpeedLimit() <= 9.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.8)) * 3.); //CR6 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 9. && edge->getSpeedLimit() <= 11.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 11. && edge->getSpeedLimit() <= 13.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 13. && edge->getSpeedLimit() <= 16.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.7 * (flow / (capacity * 1.)) * 2.); //CR4 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 16. && edge->getSpeedLimit() <= 18.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 18. && edge->getSpeedLimit() <= 22.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 22. && edge->getSpeedLimit() <= 26.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    } else if ((roadClass >= 4 || roadClass == -1) && edge->getSpeedLimit() > 26.) {
        return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 1.)) * 2.); //CR3 in table.py
    }
    return edge->getLength() / edge->getSpeedLimit() * (1. + 1.*(flow / (capacity * 0.9)) * 3.); //CR5 in table.py
}


bool
ROMAAssignments::addRoute(const ConstROEdgeVector& edges, std::vector<std::shared_ptr<RORoute> >& paths, std::string routeId, double prob) {
    std::vector<std::shared_ptr<RORoute> >::iterator p;
    for (p = paths.begin(); p != paths.end(); p++) {
        if (edges == (*p)->getEdgeVector()) {
            break;
        }
    }
    if (p == paths.end()) {
        paths.push_back(std::make_shared<RORoute>(routeId, 0., prob, edges, nullptr, StopParVector()));
        return true;
    }
    (*p)->addProbability(prob);
    std::iter_swap(paths.end() - 1, p);
    return false;
}


const ConstROEdgeVector
ROMAAssignments::computePath(ODCell* cell, const SUMOTime time, const double probability, SUMOAbstractRouter<ROEdge, ROVehicle>* router, bool setBulkMode) {
    const ROEdge* const from = myNet.getEdge(cell->origin + (cell->originIsEdge ? "" : "-source"));
    if (from == nullptr) {
        throw ProcessError(TLF("Unknown origin '%'.", cell->origin));
    }
    const ROEdge* const to = myNet.getEdge(cell->destination + (cell->destinationIsEdge ? "" : "-sink"));
    if (to == nullptr) {
        throw ProcessError(TLF("Unknown destination '%'.", cell->destination));
    }
    ConstROEdgeVector edges;
    if (router == nullptr) {
        router = &myRouter;
    }
    if (myMaxAlternatives > 0 && (int)cell->pathsVector.size() < myMaxAlternatives) {
        router->compute(from, to, myDefaultVehicle, time, edges);
        if (setBulkMode) {
            router->setBulkMode(true);
        }
        if (addRoute(edges, cell->pathsVector, cell->origin + cell->destination + toString(cell->pathsVector.size()), probability)) {
            return edges;
        }
    } else {
        double minCost = std::numeric_limits<double>::max();
        std::shared_ptr<RORoute> minRoute = nullptr;
        for (const std::shared_ptr<RORoute>& p : cell->pathsVector) {
            const double cost = router->recomputeCosts(p->getEdgeVector(), myDefaultVehicle, time);
            if (cost < minCost) {
                minCost = cost;
                minRoute = p;
            }
        }
        if (minRoute != nullptr) {
            minRoute->addProbability(probability);
        }
    }
    return ConstROEdgeVector();
}


bool
ROMAAssignments::computeBulkPaths(const std::map<std::string, std::vector<ODCell*> >& cellsByOrigin) {
#ifdef HAVE_FOX
    if (myNet.getThreadPool().size() > 0) {
        std::atomic<bool> newRoute(false);
        int workerIndex = 0;
        for (const auto& item : cellsByOrigin) {
            myNet.getThreadPool().add(new SUEBulkOriginTask(*this, item.second, newRoute), workerIndex);
            workerIndex++;
            if (workerIndex == (int)myNet.getThreadPool().size()) {
                workerIndex = 0;
            }
        }
        myNet.getThreadPool().waitAll();
        return newRoute.load();
    }
#endif
    bool newRoute = false;
    for (const auto& item : cellsByOrigin) {
        myRouter.setBulkMode(false);
        for (ODCell* const c : item.second) {
            newRoute |= !computePath(c, 0, 0., &myRouter, true).empty();
        }
    }
    myRouter.setBulkMode(false);
    return newRoute;
}


void
ROMAAssignments::getKPaths(const int kPaths, const double penalty) {
    if (kPaths <= 0) {
        return;
    }
    std::map<std::string, std::vector<ODCell*> > cellsByOrigin;
    std::map<std::string, std::vector<size_t> > prevSizesByOrigin;
    for (ODCell* const c : myMatrix.getCells()) {
        cellsByOrigin[c->origin].push_back(c);
        prevSizesByOrigin[c->origin].push_back(c->pathsVector.size());
    }
    myPenalties.clear();
    computeBulkPaths(cellsByOrigin);
    if (kPaths > 1) {
#ifdef HAVE_FOX
        if (myNet.getThreadPool().size() > 0) {
            int workerIndex = 0;
            for (const auto& item : cellsByOrigin) {
                myNet.getThreadPool().add(new KPathsTask(*this, item.second, prevSizesByOrigin[item.first], kPaths, penalty), workerIndex);
                workerIndex++;
                if (workerIndex == (int)myNet.getThreadPool().size()) {
                    workerIndex = 0;
                }
            }
            myNet.getThreadPool().waitAll();
            return;
        }
#endif
        for (const auto& item : cellsByOrigin) {
            const std::vector<ODCell*>& originCells = item.second;
            const std::vector<size_t>& prevSizes = prevSizesByOrigin[item.first];
            for (size_t idx = 0; idx < originCells.size(); idx++) {
                ODCell* const c = originCells[idx];
                myPenalties.clear();
                if (c->pathsVector.size() > prevSizes[idx]) {
                    for (const ROEdge* const e : c->pathsVector.back()->getEdgeVector()) {
                        myPenalties[e] += penalty;
                    }
                }
                for (int k = 1; k < kPaths; k++) {
                    for (const ROEdge* const e : computePath(c, 0, 0., &myRouter, false)) {
                        myPenalties[e] += penalty;
                    }
                }
            }
        }
        myPenalties.clear();
    }
}


void
ROMAAssignments::resetFlows() {
    const double begin = STEPS2TIME(MIN2(myBegin, myMatrix.getCells().front()->begin));
    for (ROEdge* e : ROEdge::getAllEdges()) {
        ROMAEdge* const edge = static_cast<ROMAEdge*>(e);
        edge->setFlow(begin, STEPS2TIME(myEnd), 0.);
        edge->setHelpFlow(begin, STEPS2TIME(myEnd), 0.);
    }
}


void
ROMAAssignments::writeInterval(const SUMOTime begin, const SUMOTime end) {
    if (myNetloadOutput != nullptr) {
        myNetloadOutput->openTag(SUMO_TAG_INTERVAL).writeAttr(SUMO_ATTR_BEGIN, time2string(begin)).writeAttr(SUMO_ATTR_END, time2string(end));
        for (ROEdge* e : ROEdge::getAllEdges()) {
            const ROMAEdge* const edge = static_cast<ROMAEdge*>(e);
            if (edge->getFunction() == SumoXMLEdgeFunc::NORMAL) {
                myNetloadOutput->openTag(SUMO_TAG_EDGE).writeAttr(SUMO_ATTR_ID, edge->getID());
                const double traveltime = edge->getTravelTime(getDefaultVehicle(), STEPS2TIME(begin));
                const double speed = edge->getLength() / traveltime;
                const double flow = edge->getFlow(STEPS2TIME(begin));
                const double timeGap = STEPS2TIME(end - begin) / flow;
                const double spaceGap = timeGap * speed;
                const double density = 1000.0 / spaceGap;
                const double laneDensity = density / edge->getNumLanes();
                myNetloadOutput->writeAttr(SUMO_ATTR_TRAVELTIME, traveltime);
                myNetloadOutput->writeAttr(SUMO_ATTR_SPEED, speed);
                myNetloadOutput->writeAttr(SUMO_ATTR_SPEEDREL, speed / edge->getSpeedLimit());
                myNetloadOutput->writeAttr(SUMO_ATTR_ENTERED, flow);
                myNetloadOutput->writeAttr(SUMO_ATTR_DENSITY, density);
                myNetloadOutput->writeAttr(SUMO_ATTR_LANEDENSITY, laneDensity);
                myNetloadOutput->writeAttr("flowCapacityRatio", 100. * flow / getCapacity(edge));
                myNetloadOutput->closeTag();
            }
        }
        myNetloadOutput->closeTag();
    }
}


void
ROMAAssignments::incremental(const int numIter, const bool verbose) {
    SUMOTime lastBegin = -1;
    std::vector<int> intervals;
    int count = 0;
    for (const ODCell* const c : myMatrix.getCells()) {
        if (c->begin != lastBegin) {
            intervals.push_back(count);
            lastBegin = c->begin;
        }
        count++;
    }
    lastBegin = -1;
    for (std::vector<int>::const_iterator offset = intervals.begin(); offset != intervals.end(); offset++) {
        std::vector<ODCell*>::const_iterator firstCell = myMatrix.getCells().begin() + (*offset);
        std::vector<ODCell*>::const_iterator lastCell = myMatrix.getCells().end();
        if (offset != intervals.end() - 1) {
            lastCell = myMatrix.getCells().begin() + (*(offset + 1));
        }
        const SUMOTime intervalStart = (*firstCell)->begin;
        if (verbose) {
            WRITE_MESSAGE(" starting interval " + time2string(intervalStart));
        }
        std::map<const ROMAEdge*, double> loadedTravelTimes;
        for (ROEdge* e : ROEdge::getAllEdges()) {
            const ROMAEdge* const edge = static_cast<ROMAEdge*>(e);
            if (edge->hasLoadedTravelTime(STEPS2TIME(intervalStart))) {
                loadedTravelTimes[edge] = edge->getTravelTime(myDefaultVehicle, STEPS2TIME(intervalStart));
            }
        }
        if (loadedTravelTimes.empty()) {
            // we don't have loaded travel times for this interval but we may have set ourselves some for the interval before but no need to warn
            ROEdge::disableTimelineWarning();
        }
        for (int t = 0; t < numIter; t++) {
            if (verbose) {
                WRITE_MESSAGE("  starting iteration " + toString(t));
            }
            std::string lastOrigin = "";
            int workerIndex = 0;
            for (std::vector<ODCell*>::const_iterator i = firstCell; i != lastCell; i++) {
                ODCell* const c = *i;
                const double linkFlow = c->vehicleNumber / numIter;
                const SUMOTime begin = myAdditiveTraffic ? myBegin : c->begin;
#ifdef HAVE_FOX
                if (myNet.getThreadPool().size() > 0) {
                    if (lastOrigin != c->origin) {
                        workerIndex++;
                        if (workerIndex == myNet.getThreadPool().size()) {
                            workerIndex = 0;
                        }
                        myNet.getThreadPool().add(new RONet::BulkmodeTask(false), workerIndex);
                        lastOrigin = c->origin;
                        myNet.getThreadPool().add(new RoutingTask(*this, c, begin, linkFlow), workerIndex);
                    } else {
                        myNet.getThreadPool().add(new RoutingTask(*this, c, begin, linkFlow), workerIndex);
                    }
                    continue;
                }
#else
                UNUSED_PARAMETER(workerIndex);
#endif
                if (lastOrigin != c->origin) {
                    myRouter.setBulkMode(false);
                    lastOrigin = c->origin;
                }
                computePath(c, begin, linkFlow, &myRouter, true);
            }
#ifdef HAVE_FOX
            if (myNet.getThreadPool().size() > 0) {
                myNet.getThreadPool().waitAll();
            }
#endif
            for (std::vector<ODCell*>::const_iterator i = firstCell; i != lastCell; i++) {
                ODCell* const c = *i;
                const double linkFlow = c->vehicleNumber / numIter;
                const SUMOTime begin = myAdditiveTraffic ? myBegin : c->begin;
                const SUMOTime end = myAdditiveTraffic ? myEnd : c->end;
                const double intervalLengthInHours = STEPS2TIME(end - begin) / 3600.;
                const ConstROEdgeVector& edges = c->pathsVector.back()->getEdgeVector();
                for (ConstROEdgeVector::const_iterator e = edges.begin(); e != edges.end(); e++) {
                    ROMAEdge* edge = static_cast<ROMAEdge*>(myNet.getEdge((*e)->getID()));
                    const double newFlow = edge->getFlow(STEPS2TIME(begin)) + linkFlow;
                    edge->setFlow(STEPS2TIME(begin), STEPS2TIME(end), newFlow);
                    double travelTime = capacityConstraintFunction(edge, newFlow / intervalLengthInHours);
                    if (lastBegin >= 0 && myAdaptionFactor > 0.) {
                        if (loadedTravelTimes.count(edge) != 0) {
                            travelTime = loadedTravelTimes[edge] * myAdaptionFactor + (1. - myAdaptionFactor) * travelTime;
                        } else {
                            travelTime = edge->getTravelTime(myDefaultVehicle, STEPS2TIME(lastBegin)) * myAdaptionFactor + (1. - myAdaptionFactor) * travelTime;
                        }
                    }
                    edge->addTravelTime(travelTime, STEPS2TIME(begin), STEPS2TIME(end));
                }
            }
            myRouter.reset(myDefaultVehicle);
        }
        writeInterval(intervalStart, (*(lastCell - 1))->end);
        lastBegin = intervalStart;
    }
}


void
ROMAAssignments::sue(const int maxOuterIteration, const int maxInnerIteration, const int kPaths, const double penalty, const double tolerance, const std::string /* routeChoiceMethod */) {
    getKPaths(kPaths, penalty);
    const std::vector<ODCell*>& cells = myMatrix.getCells();
    std::map<std::string, std::vector<ODCell*> > cellsByOrigin;
    std::map<const SUMOTime, SUMOTime> intervals;
    if (myAdditiveTraffic) {
        intervals[myBegin] = myEnd;
    }
    for (ODCell* const c : cells) {
        cellsByOrigin[c->origin].push_back(c);
        if (!myAdditiveTraffic) {
            intervals[c->begin] = c->end;
        }
    }
    const size_t numEdges = ROEdge::getAllEdges().size();
    std::vector<std::pair<SUMOTime, SUMOTime> > intervalVec(intervals.begin(), intervals.end());
    std::map<SUMOTime, size_t> intervalToOffset;
    for (size_t idx = 0; idx < intervalVec.size(); idx++) {
        intervalToOffset[intervalVec[idx].first] = idx * numEdges;
    }
    std::vector<size_t> cellIntervalOffset(cells.size(), 0);
    for (size_t ci = 0; ci < cells.size(); ci++) {
        const SUMOTime begin = myAdditiveTraffic ? myBegin : cells[ci]->begin;
        cellIntervalOffset[ci] = intervalToOffset[begin];
    }
    std::vector<double> helpFlows(intervalVec.size() * numEdges, 0.);
#ifdef HAVE_FOX
    const int numThreads = (int)myNet.getThreadPool().size();
    std::vector<std::unique_ptr<LogitCalculator<RORoute, ROEdge, ROVehicle> > > logitCalcs;
    std::vector<std::unique_ptr<GawronCalculator<RORoute, ROEdge, ROVehicle> > > gawronCalcs;
    std::vector<RouteCostCalculator<RORoute, ROEdge, ROVehicle>*> workerCalcs;
    if (numThreads > 0) {
        OptionsCont& oc = OptionsCont::getOptions();
        const std::string method = oc.getString("route-choice-method");
        workerCalcs.reserve(numThreads);
        if (method == "logit") {
            logitCalcs.reserve(numThreads);
            for (int t = 0; t < numThreads; t++) {
                logitCalcs.emplace_back(new LogitCalculator<RORoute, ROEdge, ROVehicle>(
                                            oc.getFloat("logit.beta"), oc.getFloat("logit.gamma"), oc.getFloat("logit.theta")));
                workerCalcs.push_back(logitCalcs.back().get());
            }
        } else {
            gawronCalcs.reserve(numThreads);
            for (int t = 0; t < numThreads; t++) {
                gawronCalcs.emplace_back(new GawronCalculator<RORoute, ROEdge, ROVehicle>(
                                             oc.getFloat("gawron.beta"), oc.getFloat("gawron.a")));
                workerCalcs.push_back(gawronCalcs.back().get());
            }
        }
    }
#endif
    for (int outer = 0; outer < maxOuterIteration; outer++) {
        for (int inner = 0; inner < maxInnerIteration; inner++) {
#ifdef HAVE_FOX
            if (numThreads > 0 && !cells.empty()) {
                const size_t chunkSize = (cells.size() + numThreads - 1) / numThreads;
                for (int t = 0; t < numThreads; t++) {
                    const size_t beginIdx = (size_t)t * chunkSize;
                    if (beginIdx >= cells.size()) {
                        break;
                    }
                    const size_t endIdx = MIN2(cells.size(), beginIdx + chunkSize);
                    myNet.getThreadPool().add(new SUEInnerTask(*this, cells, beginIdx, endIdx, *workerCalcs[t]), t);
                }
                myNet.getThreadPool().waitAll();
            } else
#endif
            {
                RouteCostCalculator<RORoute, ROEdge, ROVehicle>& calc = RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator();
                for (const ODCell* const c : cells) {
                    if (!c->pathsVector.empty()) {
                        for (const std::shared_ptr<RORoute>& r : c->pathsVector) {
                            r->setCosts(myRouter.recomputeCosts(r->getEdgeVector(), myDefaultVehicle, 0));
                        }
                        calc.calculateProbabilities(c->pathsVector, myDefaultVehicle, 0);
                    }
                }
            }
            std::fill(helpFlows.begin(), helpFlows.end(), 0.);
            for (size_t ci = 0; ci < cells.size(); ci++) {
                const ODCell* const c = cells[ci];
                const size_t baseOffset = cellIntervalOffset[ci];
                for (const std::shared_ptr<RORoute>& r : c->pathsVector) {
                    const double pathFlow = r->getProbability() * c->vehicleNumber;
                    for (const ROEdge* const e : r->getEdgeVector()) {
                        helpFlows[baseOffset + e->getNumericalID()] += pathFlow;
                    }
                }
            }
            // calculate new edge flows and check for stability
            int unstableEdges = 0;
            for (size_t idx = 0; idx < intervalVec.size(); idx++) {
                const auto& it = intervalVec[idx];
                const double intervalLengthInHours = STEPS2TIME(it.second - it.first) / 3600.;
                const double intBegin = STEPS2TIME(it.first);
                const double intEnd = STEPS2TIME(it.second);
                const size_t baseOffset = idx * numEdges;
                for (ROEdge* e : ROEdge::getAllEdges()) {
                    ROMAEdge* const edge = static_cast<ROMAEdge*>(e);
                    const double helpFlow = helpFlows[baseOffset + e->getNumericalID()];
                    const double oldFlow = edge->getFlow(intBegin);
                    double newFlow = oldFlow;
                    if (inner == 0 && outer == 0) {
                        newFlow += helpFlow;
                    } else {
                        newFlow += (helpFlow - oldFlow) / (inner + 1);
                    }
                    //                if not lohse:
                    if (newFlow > 0.) {
                        if (fabs(newFlow - oldFlow) / newFlow > tolerance) {
                            unstableEdges++;
                        }
                    } else if (newFlow == 0.) {
                        if (oldFlow != 0. && (fabs(newFlow - oldFlow) / oldFlow > tolerance)) {
                            unstableEdges++;
                        }
                    } else { // newFlow < 0.
                        unstableEdges++;
                        newFlow = 0.;
                    }
                    edge->setFlow(intBegin, intEnd, newFlow);
                    const double travelTime = capacityConstraintFunction(edge, newFlow / intervalLengthInHours);
                    edge->addTravelTime(travelTime, intBegin, intEnd);
                }
            }
            // if stable break
            if (unstableEdges == 0) {
                break;
            }
            // additional stability check from python script: if notstable < math.ceil(net.geteffEdgeCounts()*0.005) or notstable < 3: stable = True
        }
        // travel times changed since the metric was last built (or since the
        // previous outer iteration): flag it stale so the *first* subsequent
        // router query re-customizes (CCH) / rebuilds the hierarchy (CH)
        // from current edge weights. recomputeCosts() above never consults
        // the cached metric (it reads the live effort function directly), so
        // a single reset() here -- right before the only call that does,
        // computePath()'s router->compute() below -- is exactly as correct
        // as resetting after every inner iteration, but for an
        // expensive-to-rebuild algorithm like CH this cuts the number of
        // rebuilds from O(maxInnerIteration) to one per outer iteration.
        myRouter.reset(myDefaultVehicle);
        // check for a new route, if none available, break
        // several modifications about when a route is new and when to break are in the original script
        if (!computeBulkPaths(cellsByOrigin)) {
            break;
        }
    }
    // final round of assignment
#ifdef HAVE_FOX
    if (numThreads > 0 && !cells.empty()) {
        const size_t chunkSize = (cells.size() + numThreads - 1) / numThreads;
        for (int t = 0; t < numThreads; t++) {
            const size_t beginIdx = (size_t)t * chunkSize;
            if (beginIdx >= cells.size()) {
                break;
            }
            const size_t endIdx = MIN2(cells.size(), beginIdx + chunkSize);
            myNet.getThreadPool().add(new SUEInnerTask(*this, cells, beginIdx, endIdx, *workerCalcs[t]), t);
        }
        myNet.getThreadPool().waitAll();
        for (const ODCell* const c : cells) {
            for (const std::shared_ptr<RORoute>& r : c->pathsVector) {
                r->setProbability(r->getProbability() * c->vehicleNumber);
            }
        }
    } else
#endif
    {
        RouteCostCalculator<RORoute, ROEdge, ROVehicle>& calc = RouteCostCalculator<RORoute, ROEdge, ROVehicle>::getCalculator();
        for (const ODCell* const c : cells) {
            if (!c->pathsVector.empty()) {
                // update path cost
                for (const std::shared_ptr<RORoute>& r : c->pathsVector) {
                    r->setCosts(myRouter.recomputeCosts(r->getEdgeVector(), myDefaultVehicle, 0));
                }
                // calculate route utilities and probabilities
                calc.calculateProbabilities(c->pathsVector, myDefaultVehicle, 0);
                // calculate route flows
                for (const std::shared_ptr<RORoute>& r : c->pathsVector) {
                    r->setProbability(r->getProbability() * c->vehicleNumber);
                }
            }
        }
    }
    for (const auto& it : intervals) {
        writeInterval(it.first, it.second);
    }
}


double
ROMAAssignments::getPenalizedEffort(const ROEdge* const e, const ROVehicle* const v, double t) {
    if (myPenalties.empty()) {
        return e->getEffort(v, t);
    }
    const auto i = myPenalties.find(e);
    return i == myPenalties.end() ? e->getEffort(v, t) : e->getEffort(v, t) + i->second;
}


double
ROMAAssignments::getPenalizedTT(const ROEdge* const e, const ROVehicle* const v, double t) {
    if (myPenalties.empty()) {
        return e->getTravelTime(v, t);
    }
    const auto i = myPenalties.find(e);
    return i == myPenalties.end() ? e->getTravelTime(v, t) : e->getTravelTime(v, t) + i->second;
}


double
ROMAAssignments::getTravelTime(const ROEdge* const e, const ROVehicle* const v, double t) {
    return e->getTravelTime(v, t);
}


#ifdef HAVE_FOX
// ---------------------------------------------------------------------------
// ROMAAssignments::RoutingTask-methods
// ---------------------------------------------------------------------------
void
ROMAAssignments::RoutingTask::run(MFXWorkerThread* context) {
    myAssign.computePath(myCell, myBegin, myLinkFlow, &static_cast<RONet::WorkerThread*>(context)->getVehicleRouter(SVC_IGNORING), mySetBulkMode);
}


void
ROMAAssignments::SUEBulkOriginTask::run(MFXWorkerThread* context) {
    SUMOAbstractRouter<ROEdge, ROVehicle>& router = static_cast<RONet::WorkerThread*>(context)->getVehicleRouter(SVC_IGNORING);
    router.setBulkMode(false);
    bool localNew = false;
    for (ODCell* const c : myCells) {
        if (!myAssign.computePath(c, 0, 0., &router, true).empty()) {
            localNew = true;
        }
    }
    router.setBulkMode(false);
    if (localNew) {
        myNewRoute.store(true, std::memory_order_relaxed);
    }
}


void
ROMAAssignments::KPathsTask::run(MFXWorkerThread* context) {
    SUMOAbstractRouter<ROEdge, ROVehicle>& router = static_cast<RONet::WorkerThread*>(context)->getVehicleRouter(SVC_IGNORING);
    router.setBulkMode(false);
    for (size_t idx = 0; idx < myCells.size(); idx++) {
        ODCell* const c = myCells[idx];
        myPenalties.clear();
        if (c->pathsVector.size() > myPrevSizes[idx]) {
            for (const ROEdge* const e : c->pathsVector.back()->getEdgeVector()) {
                myPenalties[e] += myPenalty;
            }
        }
        for (int k = 1; k < myKPaths; k++) {
            for (const ROEdge* const e : myAssign.computePath(c, 0, 0., &router, false)) {
                myPenalties[e] += myPenalty;
            }
        }
    }
    myPenalties.clear();
}


void
ROMAAssignments::SUEInnerTask::run(MFXWorkerThread* context) {
    SUMOAbstractRouter<ROEdge, ROVehicle>& router = static_cast<RONet::WorkerThread*>(context)->getVehicleRouter(SVC_IGNORING);
    for (size_t ci = myBeginIdx; ci < myEndIdx; ci++) {
        const ODCell* const c = myCells[ci];
        if (!c->pathsVector.empty()) {
            for (const std::shared_ptr<RORoute>& r : c->pathsVector) {
                r->setCosts(router.recomputeCosts(r->getEdgeVector(), myAssign.myDefaultVehicle, 0));
            }
            myCalc.calculateProbabilities(c->pathsVector, myAssign.myDefaultVehicle, 0);
        }
    }
}
#endif
