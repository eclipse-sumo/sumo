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
    double maxReached = 0;
    double totalPrio = 0;
    std::vector<StopEdgeInfo> bestStops = stops;
    std::shared_ptr<StopPathNode> bestNode = nullptr;
    double bestCost = myRouter.recomputeCostsPos(edges, myVehicle, sourcePos, 0, myT);
    std::vector<int> skipped;

    // each upcoming stop is part of a graph where we can start to search for a
    // better solution. We evaluate followers lazily and prune based on the best
    // solution already found
    std::priority_queue<std::shared_ptr<StopPathNode>, std::vector<std::shared_ptr<StopPathNode> >, spnCompare> queue;

    auto prev = std::make_shared<StopPathNode>(*this, StopEdgeInfo(source, -1, SIMSTEP, sourcePos));
    prev->stopIndex = -1;
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
            maxReached += stop.priority;
            auto ptr = std::make_shared<StopPathNode>(*this, stop);
            ptr->skippedPrio = minSkipped;
            ptr->reachedPrio = prev->reachedPrio + stop.priority;
            ptr->prev = prev;
            ptr->stopIndex = i;
            prev = ptr;
            queue.push(ptr);
        }
        totalPrio += stop.priority;
        //std::cout << " i=" << i << " e=" << ptr->edge->getID() << " sp=" << ptr->skippedPrio << " rp=" << ptr->reachedPrio << "\n";
    }
    bestNode = prev;
#ifdef DEBUG_OPTIMIZE_SKIPPED
    std::cout << SIMTIME << " optimizeSkipped veh=" << myVehicle->getID()
        << " source=" << source->getID() << " sourcePos=" << sourcePos
        << " nStops=" << stops.size() << " skipped=" << toString(skipped) << " qSize=" << queue.size()
        << " minSkipped=" << minSkipped << " maxReached=" << maxReached << "\n";
#endif

    while (!queue.empty()) {
        auto ptr = queue.top();
        queue.pop();
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "  qSize=" << queue.size() << " topNode edge=" << ptr->edge->getID()
            << " name='" << ptr->nameTag.first << "' skippedPrio=" << ptr->skippedPrio
            << " trackChanges=" << ptr->trackChanges
            << " cost=" << ptr->cost
            << " reachedPrio=" << ptr->reachedPrio
            << " stopIndex=" << ptr->stopIndex << "\n";
#endif
        auto succ = ptr->getSuccessor(stops, minSkipped);
        if (ptr->skippedPrio < minSkipped && ptr->reachedPrio > maxReached) {
            minSkipped = MIN2(minSkipped, totalPrio - ptr->reachedPrio);
            maxReached = ptr->reachedPrio;
            bestNode = ptr;
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "   newBest minSkipped=" << minSkipped << " maxReached=" << maxReached << "\n";
#endif
        }
        if (succ != nullptr) {
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
    while (bestNode != nullptr && bestNode->stopIndex >= 0) {
        stops[bestNode->stopIndex].skipped = false;
        stops[bestNode->stopIndex].backtracked = false;
        stops[bestNode->stopIndex].origEdge = bestNode->origEdge;
        if (!bestEdges.empty() && !bestNode->edges.empty()) {
            bestNode->edges.pop_back();
        }
        bestEdges.insert(bestEdges.begin(), bestNode->edges.begin(), bestNode->edges.end());
        bestNode = bestNode->prev;
    }
    //std::cout << "oldEdges=" << toString(edges) << "\nnewEdges=" << toString(bestEdges) << "\n";
    return bestEdges;
}


std::shared_ptr<MSStopOptimizer::StopPathNode>
MSStopOptimizer::StopPathNode::getSuccessor(const std::vector<StopEdgeInfo>& stops, double minSkipped) {
    if (!checked) {
        checked = true;
        // @todo caching, bulk-routing
        if (!so.reachableInTime(prev->edge, prev->pos, edge, pos, arrival, edges)) {
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    prevIndex=" << prev->stopIndex << " prevEdge=" << prev->edge->getID() << " i=" << stopIndex << " edge=" << edge->getID() << " unreachable\n";
#endif
            return nullptr;
        } else {
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    prevIndex=" << prev->stopIndex << " prevEdge=" << prev->edge->getID() << " i=" << stopIndex << " edge=" << edge->getID() << " reached with edges=" << toString(edges) << "\n";
#endif
        }
        reachedPrio += priority;
    }
    int nextIndex = stopIndex + numSkipped + 1;
    while (nextIndex < (int)stops.size()) {
        const StopEdgeInfo& next = stops[nextIndex];
        if (next.priority < 0) {
            // next stop can neither be skipped nor changed
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    i=" << stopIndex << " next=" << nextIndex << " unskipped\n";
#endif
            return nullptr;
        }
        const std::vector<MSStoppingPlace*>& alternatives = MSNet::getInstance()->getStoppingPlaceAlternatives(next.nameTag.first, next.nameTag.second);
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "    i=" << stopIndex << " next=" << nextIndex << " numSkipped=" << numSkipped << " altIndex=" << altIndex << " nAlt=" << alternatives.size() << "\n";
#endif
        if (alternatives.empty() && altIndex == 0 && numSkipped > 0) {
            altIndex++;
            auto succ = std::make_shared<StopPathNode>(so, next);
            succ->skippedPrio = skippedPrio;
            succ->reachedPrio = reachedPrio;
            succ->trackChanges = trackChanges;
            succ->prev = shared_from_this();
            succ->stopIndex = nextIndex;
            return succ;
        }
        while (altIndex < (int)alternatives.size()) {
            MSStoppingPlace* alt = alternatives[altIndex];
            const MSEdge* altEdge = &alt->getLane().getEdge();
            altIndex++;
            if ((altEdge == edge && numSkipped == 0) || !alt->getLane().allowsVehicleClass(so.myVehicle->getVClass())) {
                continue;
            }
            auto succ = std::make_shared<StopPathNode>(so, next);
            if (altEdge != succ->edge) {
                succ->origEdge = succ->edge;
                succ->edge = altEdge;
            }
            succ->pos = alt->getEndLanePosition();
            succ->skippedPrio = skippedPrio;
            succ->reachedPrio = reachedPrio;
            succ->trackChanges = trackChanges + 1;
            succ->prev = shared_from_this();
            succ->stopIndex = nextIndex;
            return succ;
        }
        skippedPrio =+ next.priority;
        if (skippedPrio >= minSkipped) {
            // cannot improve on current best solution
#ifdef DEBUG_OPTIMIZE_SKIPPED
            std::cout << "    i=" << stopIndex << " next=" << nextIndex << " skippedPrio=" << skippedPrio << " minSkipped=" << minSkipped << " pruned\n";
#endif
            return nullptr;
        }
        nextIndex++;
        altIndex = 0;
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
        ConstMSEdgeVector& into) const {
    myRouter.compute(from, fromPos, to, toPos, myVehicle, myT, into, true);
    if (into.size() > 0) {
        SUMOTime cost = TIME2STEPS(myRouter.recomputeCostsPos(into, myVehicle, fromPos, toPos, myT));
        //std::cout << " from=" << from->getID() << " fromPos=" << fromPos << " to=" << to->getID() << " toPos=" << toPos << " t=" << myT << " cost=" << cost << " arrival=" << arrival << " maxDelay=" << myMaxDelay << "\n";
        if (myT + cost <= arrival + myMaxDelay) {
            return true;
        }
    }
    return false;
}


/****************************************************************************/
