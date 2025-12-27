/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
/// @file    MSStopOptimizer.cpp
/// @author  Jakob Erdmann
/// @date    Nov 2025
///
// Optimizes prioritized stops
/****************************************************************************/
#include <config.h>
#include <queue>
#include "MSLane.h"
#include "MSEdge.h"
#include "MSStoppingPlace.h"
#include "MSStopOptimizer.h"

//#define DEBUG_OPTIMIZE_SKIPPED

// ===========================================================================
// method definitions
// ===========================================================================

ConstMSEdgeVector
MSStopOptimizer::optimizeSkipped(const MSEdge* source, double sourcePos, std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges) const {
    // intial solution is an upper bound on the acceptle total skipped priority
    double minSkipped = 0;
    double totalPrio = 0;
    std::vector<StopEdgeInfo> bestStops = stops;
    double bestCost = myRouter.recomputeCostsPos(edges, myVehicle, sourcePos, 0, myT);
    std::vector<int> skipped;

    // each upcoming stop is part of a graph where we can start to search for a
    // better solution. We evaluate followers lazily and prune based on the best
    // solution already found
    std::priority_queue<std::shared_ptr<StopPathNode>, std::vector<std::shared_ptr<StopPathNode> >, spnCompare> queue;

    auto prev = std::make_shared<StopPathNode>(*this, StopEdgeInfo(source, -1, SIMSTEP, sourcePos));
    std::shared_ptr<StopPathNode> bestNode = prev;
    prev->stopIndex = -1;
    prev->routeIndex = 0;
    prev->cost = bestCost;
    prev->checked = true;
    //std::cout << " i=-1 e=" << prev->edge->getID() << " sp=" << prev->skippedPrio << " rp=" << prev->reachedPrio << "\n";
    queue.push(prev);

    for (int i = 0; i < (int)stops.size(); i++) {
        const StopEdgeInfo& stop = stops[i];
        if (stop.skipped) {
            skipped.push_back(i);
            minSkipped += stops[i].priority;
        } else {
            auto ptr = std::make_shared<StopPathNode>(*this, stop);
            ptr->skippedPrio = minSkipped;
            ptr->reachedPrio = prev->reachedPrio;
            ptr->reachedMandatory = prev->reachedMandatory;
            if (stop.priority >= 0) {
                ptr->reachedPrio += stop.priority;
            } else {
                ptr->reachedMandatory += 1;
            }
            ptr->checked = true;
            ptr->prev = prev;
            ptr->stopIndex = i;
            ConstMSEdgeVector subEdges(edges.begin(), edges.begin() + stop.routeIndex);
            ptr->cost = myRouter.recomputeCostsPos(subEdges, myVehicle, sourcePos, 0, myT);
            ptr->edges.insert(ptr->edges.begin(), edges.begin() + prev->routeIndex, edges.begin() + stop.routeIndex + 1);
            prev = ptr;
            bestNode = ptr;
            queue.push(ptr);
        }
        totalPrio += MAX2(0.0, stop.priority);
        //std::cout << " i=" << i << " e=" << ptr->edge->getID() << " sp=" << ptr->skippedPrio << " rp=" << ptr->reachedPrio << "\n";
    }
#ifdef DEBUG_OPTIMIZE_SKIPPED
    std::cout << SIMTIME << " optimizeSkipped veh=" << myVehicle->getID()
        << " source=" << source->getID() << " sourcePos=" << sourcePos
        << " nStops=" << stops.size() << " skipped=" << toString(skipped) << " qSize=" << queue.size()
        << " minSkipped=" << minSkipped << " totalPrio=" << totalPrio << "\n";
    for (const StopEdgeInfo& stop : stops) {
        std::cout << "   edge=" << stop.edge->getID() << " name=" << stop.nameTag.first << " wasSkipped=" << stop.skipped << " prio=" << stop.priority << "\n";
    }
    std::cout << " bestNode edge=" << bestNode->edge->getID() << " rPrio=" << bestNode->reachedPrio << " cost=" << bestNode->cost << "\n";
#endif

    std::map<const MSEdge*, std::shared_ptr<StopPathNode> > bestIntermediate;
    while (!queue.empty()) {
        auto ptr = queue.top();
        queue.pop();
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "  qSize=" << queue.size() << " topNode edge=" << ptr->edge->getID()
            << " name='" << ptr->nameTag.first << "' skippedPrio=" << ptr->skippedPrio
            << " trackChanges=" << ptr->trackChanges
            << " cost=" << ptr->cost
            << " reachedPrio=" << ptr->reachedPrio
            << " mandatory=" << ptr->reachedMandatory
            << " stopIndex=" << ptr->stopIndex << "\n";
#endif
        auto succ = ptr->getSuccessor(stops, minSkipped);
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "      cost2=" << ptr->cost << " reachedPrio2=" << ptr->reachedPrio << " mandatory2=" << ptr->reachedMandatory << " succ=" << (succ == nullptr ? "NULL" : succ->edge->getID()) << "\n";
#endif
        if (*bestNode < *ptr) {
            minSkipped = MIN2(minSkipped, totalPrio - ptr->reachedPrio);
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "   newBestNode edge=" << bestNode->edge->getID() << " minSkipped=" << minSkipped
                        << " orm=" << bestNode->reachedMandatory << " rm=" << ptr->reachedMandatory
                        << " orp=" << bestNode->reachedPrio << " rp=" << ptr->reachedPrio
                        << " otc=" << bestNode->trackChanges << " tc=" << ptr->trackChanges
                        << " oc=" << bestNode->cost << " c=" << ptr->cost
                        << "\n";
#endif
            bestNode = ptr;
        }
        if (succ != nullptr) {
            if (minSkipped == 0 && ptr->trackChanges > bestNode->trackChanges) {
#ifdef DEBUG_OPTIMIZE_SKIPPED
                std::cout << "pruned by minTC=" << bestNode->trackChanges << "\n";
#endif
                continue;
            }
            auto it = bestIntermediate.find(ptr->edge);
            if (it == bestIntermediate.end()) {
                bestIntermediate[ptr->edge] = ptr;
#ifdef DEBUG_OPTIMIZE_SKIPPED
                std::cout << "firstBest " << ptr->edge->getID() << "\n";
#endif
            } else {
                auto best = it->second;
                assert(best->checked);
                assert(ptr->checked);
                if (best == ptr) {
#ifdef DEBUG_OPTIMIZE_SKIPPED
                    std::cout << "alreadyBest " << ptr->edge->getID() << " rp=" << ptr->reachedPrio << " tc=" << ptr->trackChanges << " c=" << ptr->cost << "\n";
#endif
                } else if (!(*best < *ptr)) {
#ifdef DEBUG_OPTIMIZE_SKIPPED
                    std::cout << "skip " << ptr->edge->getID()
                        << " orp=" << best->reachedPrio << " rp=" << ptr->reachedPrio
                        << " otc=" << best->trackChanges << " tc=" << ptr->trackChanges
                        << " oc=" << best->cost << " c=" << ptr->cost
                        << "\n";
#endif
                    continue;
                } else {
                    bestIntermediate[ptr->edge] = ptr;
#ifdef DEBUG_OPTIMIZE_SKIPPED
                    std::cout << "newBest " << ptr->edge->getID()
                        << " orp=" << best->reachedPrio << " rp=" << ptr->reachedPrio
                        << " otc=" << best->trackChanges << " tc=" << ptr->trackChanges
                        << " oc=" << best->cost << " c=" << ptr->cost
                        << "\n";
#endif
                }
            }
            queue.push(ptr);
            queue.push(succ);
        }
    }
    for (auto& stop : stops) {
        // init true and later set false for all used stops
        if (!stop.skipped) {
            stop.backtracked = true;
            stop.skipped = true;
        }
    }
    ConstMSEdgeVector bestEdges;
    if (bestNode->stopIndex < 0) {
        // all stops were skipped. End route on current edge
        bestEdges.push_back(source);
    }
    while (bestNode != nullptr && bestNode->stopIndex >= 0) {
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "  revBestNode index=" << bestNode->stopIndex << " edge=" << bestNode->edge->getID() << " name=" << bestNode->nameTag.first << " tc=" << bestNode->trackChanges << " c=" << bestNode->cost  << "\n";
#endif
        stops[bestNode->stopIndex].skipped = false;
        stops[bestNode->stopIndex].backtracked = false;
        stops[bestNode->stopIndex].origEdge = bestNode->origEdge;
        if (!bestEdges.empty() && !bestNode->edges.empty()) {
            bestNode->edges.pop_back();
        }
        bestEdges.insert(bestEdges.begin(), bestNode->edges.begin(), bestNode->edges.end());
        bestNode = bestNode->prev;
    }
#ifdef DEBUG_OPTIMIZE_SKIPPED
    std::cout << "oldEdges=" << toString(edges) << "\nnewEdges=" << toString(bestEdges) << "\n";
#endif
    return bestEdges;
}


std::shared_ptr<MSStopOptimizer::StopPathNode>
MSStopOptimizer::StopPathNode::getSuccessor(const std::vector<StopEdgeInfo>& stops, double minSkipped) {
    if (!checked) {
        checked = true;
        // @todo caching, bulk-routing
        double newCost = 0;
        if (!so.reachableInTime(prev->edge, prev->pos, edge, pos, arrival, edges, newCost)) {
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    prevIndex=" << prev->stopIndex << " prevEdge=" << prev->edge->getID() << " i=" << stopIndex << " edge=" << edge->getID() << " unreachable\n";
#endif
            // indicate failure
            reachedMandatory = -1;
            return nullptr;
        } else {
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    prevIndex=" << prev->stopIndex << " prevEdge=" << prev->edge->getID() << " i=" << stopIndex << " edge=" << edge->getID() << " reached with edges=" << toString(edges) << " cost=" << cost << " newCost=" << newCost << "\n";
#endif
        }
        reachedPrio += MAX2(0.0, priority);
        cost += newCost;
        if (priority < 0) {
            reachedMandatory++;
        }
    }
    int nextIndex = stopIndex + numSkipped + 1;
    while (nextIndex < (int)stops.size()) {
        const StopEdgeInfo& next = stops[nextIndex];
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "    i=" << stopIndex << " next=" << nextIndex << " numSkipped=" << numSkipped << " altIndex=" << altIndex << "\n";
#endif
        // always try the default track first
        if (altIndex == -1) {
            altIndex++;
            auto succ = std::make_shared<StopPathNode>(so, next);
            succ->skippedPrio = skippedPrio;
            succ->reachedPrio = reachedPrio;
            succ->reachedMandatory = reachedMandatory;
            succ->trackChanges = trackChanges;
            succ->cost = cost;
            succ->prev = shared_from_this();
            succ->stopIndex = nextIndex;
            return succ;
        }
        const std::vector<MSStoppingPlace*>& alternatives = MSNet::getInstance()->getStoppingPlaceAlternatives(next.nameTag.first, next.nameTag.second);
        if (next.priority < 0) {
            // next stop can neither be skipped nor changed
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    i=" << stopIndex << " next=" << nextIndex << " isMandatory\n";
#endif
            return nullptr;
        }
        while (altIndex < (int)alternatives.size()) {
            MSStoppingPlace* alt = alternatives[altIndex];
            const MSEdge* altEdge = &alt->getLane().getEdge();
            altIndex++;
            if (altEdge == next.edge || !alt->getLane().allowsVehicleClass(so.myVehicle->getVClass())) {
                continue;
            }
            auto succ = std::make_shared<StopPathNode>(so, next);
            succ->trackChanges = trackChanges;
            succ->pos = alt->getEndLanePosition();
            succ->skippedPrio = skippedPrio;
            succ->reachedPrio = reachedPrio;
            succ->reachedMandatory = reachedMandatory;
            succ->cost = cost;
            succ->prev = shared_from_this();
            succ->stopIndex = nextIndex;
            succ->origEdge = succ->edge;
            succ->edge = altEdge;
            succ->trackChanges += 1;
            return succ;
        }
        skippedPrio += next.priority;
        if (skippedPrio >= minSkipped) {
            // cannot improve on current best solution
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    i=" << stopIndex << " next=" << nextIndex << " skippedPrio=" << skippedPrio << " minSkipped=" << minSkipped << " pruned\n";
#endif
            return nullptr;
        }
        nextIndex++;
        altIndex = -1;
        numSkipped++;
    }
#ifdef DEBUG_OPTIMIZE_SKIPPED
    std::cout << "    i=" << stopIndex << " noSuccessors\n";
#endif
    return nullptr;
}


bool
MSStopOptimizer::reachableInTime(const MSEdge* from, double fromPos,
        const MSEdge* to, double toPos,
        SUMOTime arrival,
        ConstMSEdgeVector& into, double &cost) const {
    myRouter.compute(from, fromPos, to, toPos, myVehicle, myT, into, true);
    if (into.size() > 0) {
        cost = myRouter.recomputeCostsPos(into, myVehicle, fromPos, toPos, myT);
        //std::cout << " from=" << from->getID() << " fromPos=" << fromPos << " to=" << to->getID() << " toPos=" << toPos << " t=" << myT << " cost=" << cost << " arrival=" << arrival << " maxDelay=" << myMaxDelay << "\n";
        if (myT + TIME2STEPS(cost) <= arrival + myMaxDelay) {
            return true;
        }
    }
    return false;
}


/****************************************************************************/
