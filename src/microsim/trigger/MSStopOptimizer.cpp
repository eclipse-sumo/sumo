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
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSStoppingPlace.h>
#include "MSStopOptimizer.h"

//#define DEBUG_OPTIMIZE_SKIPPED

// ===========================================================================
// method definitions
// ===========================================================================

ConstMSEdgeVector
MSStopOptimizer::optimizeSkipped(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router, const MSEdge* source, double sourcePos,
                               std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges, SUMOTime maxDelay) const {
    double skippedPrio = 0;
    double minPrio = std::numeric_limits<double>::max();
    std::vector<int> skipped;
    for (int i = 0; i < (int)stops.size(); i++) {
        if (stops[i].skipped) {
            skipped.push_back(i);
            skippedPrio += stops[i].priority;
        }
        minPrio = MIN2(minPrio, stops[i].priority);
    }
#ifdef DEBUG_OPTIMIZE_SKIPPED
    std::cout << SIMTIME << " veh=" << getID() << " optimizeSkipped=" << toString(skipped) << " source=" << source->getID() << "\n";
    for (int i = 0; i < (int)stops.size(); i++) {
        const auto& stop = stops[i];
        std::cout << "  " << i << " edge=" << stop.edge->getID() << " routeIndex=" << stop.routeIndex << " prio=" << stop.priority << " wasSkipped=" << stop.skipped << " arrival=" << stop.arrival << "\n";
    }
#endif
    // check reachability of skipped stops
    std::vector<int> skippedReachable;
    std::map<int, std::vector<StopEdgeInfo> > alternativesInfo;
    for (int si : skipped) {
        bool reachable = false;
        ConstMSEdgeVector into;
        router.compute(source, sourcePos, stops[si].edge, stops[si].pos, myVehicle, t, into, true);
        if (into.size() > 0) {
            SUMOTime arrival = t + TIME2STEPS(router.recomputeCostsPos(into, myVehicle, sourcePos, stops[si].pos, t));
            if (arrival - stops[si].arrival <= maxDelay) {
                reachable = true;
                skippedReachable.push_back(si);
            }
        }
        if (!reachable) {
            const std::vector<MSStoppingPlace*>& alternatives = MSNet::getInstance()->getStoppingPlaceAlternatives(stops[si].nameTag.first, stops[si].nameTag.second);
            for (MSStoppingPlace* alt : alternatives) {
                const MSEdge* altEdge = &alt->getLane().getEdge();
                if (altEdge == stops[si].edge
                        || !alt->getLane().allowsVehicleClass(myVehicle->getVClass())) {
                    continue;
                }
                const double altPos = alt->getEndLanePosition();
                into.clear();
                router.compute(source, sourcePos, altEdge, altPos, myVehicle, t, into, true);
                if (into.size() > 0) {
                    SUMOTime arrival = t + TIME2STEPS(router.recomputeCostsPos(into, myVehicle, sourcePos, altPos, t));
                    if (arrival - stops[si].arrival <= maxDelay) {
                        alternativesInfo[si].push_back(StopEdgeInfo(altEdge, stops[si].priority, stops[si].arrival, altPos));
                        if (!reachable) {
                            skippedReachable.push_back(si);
                        }
                        reachable = true;
                    }
                }
            }
        }
    }
    if (skippedReachable.size() == 0) {
        // case B: skipped stops are not reachable with backtracking
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "  noneReachable\n";
#endif
        return edges;
    }
    if (skippedPrio == minPrio && alternativesInfo.empty()) {
        // case A: only one stop was skipped and it had the lowest priority (or multiple stops with prio 0 were skipped): this is already optimal
#ifdef DEBUG_OPTIMIZE_SKIPPED
        std::cout << "  skippedPrio=" << skippedPrio << " minPrio=" << minPrio << "\n";
#endif
        return edges;
    }
    // collect stops that were not skipped but that could be skipped
    // (backtracking) to reach more or higher-priority stops
    std::set<int> unskippedBefore;
    for (int i = 0; i < (int)stops.size(); i++) {
        if (i < skipped.back()) {
            unskippedBefore.insert(i);
        }
    }
    for (int i : skipped) {
        unskippedBefore.erase(i);
    }
#ifdef DEBUG_OPTIMIZE_SKIPPED
    std::cout << "  skippedIndices=" << toString(skipped) << " unskippedBefore=" << joinToString(unskippedBefore, " ") << "\n";
#endif
    // otherwise, skippedReachable should have been empty
    assert(unskippedBefore.size() > 0);

    // the unskipped stops may form several non contiguous sequences. We care about the last element of each sequence
    std::vector<int> unskippedEnds;
    std::vector<int> skippedStarts;
    for (int i : unskippedBefore) {
        if (unskippedBefore.count(i + 1) == 0) {
            for (int i2 : skippedReachable) {
                if (i2 > i) {
                    // stop i was not skipped, stop + 1 was skipped and each i2 is potentially reachable when skipping i
                    unskippedEnds.push_back(i);
                    skippedStarts.push_back(i2);
                    break;
                }
            }
        }
    }
    std::sort(unskippedEnds.begin(), unskippedEnds.end()); // ascending
    std::set<int> skippedSet(skipped.begin(), skipped.end());
#ifdef DEBUG_OPTIMIZE_SKIPPED
    std::cout << "  unskippedEnds=" << toString(unskippedEnds) << " skippedStarts=" << toString(skippedStarts) << "\n";
#endif

    ConstMSEdgeVector bestEdges = edges;
    double altSkippedPrio = 0;
    if (skippedStarts.size() > 0) {
        const MSEdge* firstSkipped = stops[skippedStarts.back()].edge;
        for (int i = unskippedEnds.back(); i >= 0; i--) {
            double prio = stops[i].priority;
            altSkippedPrio += prio;
            if (skippedSet.count(i)  // found start of another skip sequence
                    || prio < 0 // cannot backtrack past unskippable stop
                    || altSkippedPrio >= skippedPrio // backtracking past this stop cannot improve result
               ) {
                // i is not a candidate for skipping
                unskippedEnds.pop_back();
                skippedStarts.pop_back();
                if (unskippedEnds.empty()) {
                    return edges;
                }
                // try to optimize earlier sequence of skips
                i = unskippedEnds.back();
                firstSkipped = stops[skippedStarts.back()].edge;
                altSkippedPrio = 0;
                continue;
            }
            // try skipping i and check whether it improves reachable prio
            const MSEdge* prev = i > 0 ? stops[i - 1].edge : source;
            const double prevPos = i > 0 ? stops[i - 1].pos : sourcePos;
            ConstMSEdgeVector into;
            SUMOTime start = stops[i - 1].arrival;
            router.computeLooped(prev, firstSkipped, myVehicle, start, into, true);
            if (into.size() == 0) {
                // check whether an alternative exists and is reachable
                if (alternativesInfo.count(i) != 0) {
                    for (const StopEdgeInfo& altInfo : alternativesInfo[i]) {
                        router.computeLooped(prev, altInfo.edge, myVehicle, start, into, true);
                        if (into.size() != 0) {
                            SUMOTime arrival = start + TIME2STEPS(router.recomputeCostsPos(into, myVehicle, prevPos, stops[skippedStarts.back()].pos, start));
                            if (arrival - stops[i].arrival > maxDelay) {
                                // cannot reach alternative in time
                                continue;
                            }
                            // found viable alternative
                            stops[i].origEdge = stops[i].edge->getID();
                            break;
                        }
                    }
                }
            }
            if (into.size() == 0) {
                // cannot reach firstSkipped and need to backtrack further
                continue;
            }
            start += TIME2STEPS(router.recomputeCostsPos(into, myVehicle, prevPos, stops[skippedStarts.back()].pos, start));
            //if (start - stops[i].arrival > maxDelay) {
            //    // cannot reach firstSkipped in time and need to backtrack further
            //    continue;
            //}
            // initialize skipped priority with stops skipped during backtracking and any skipped before that
            std::vector<StopEdgeInfo> stops2 = stops;
            double skippedPrio2 = altSkippedPrio;
            for (int i2 = 0; i2 < i - 1; i2++) {
                if (stops[i2].skipped) {
                    skippedPrio2 += stops[i2].priority;
                }
            }
            for (int i2 = i; i2 <= unskippedEnds.back(); i2++) {
                stops2[i2].skipped = true;
                stops2[i2].backtracked = true;
            }
            int prevRouteIndex = i > 0 ? stops[i - 1].routeIndex : myVehicle->getDepartEdge();
            assert(prevRouteIndex >= 0 && prevRouteIndex < (int)edges.size());
            ConstMSEdgeVector edges2(edges.begin(), edges.begin() + prevRouteIndex);
            stops2[skippedStarts.back()].skipped = false;
            edges2.insert(edges2.begin(), into.begin(), into.end());
            edges2 = routeAlongStops(start, router, stops2, edges2, skippedStarts.back(), maxDelay, skippedPrio2);
            if (skippedPrio2 < skippedPrio) {
#ifdef DEBUG_OPTIMIZE_SKIPPED
                std::cout << " skippedPrio=" << skippedPrio << " skippedPrio2=" << skippedPrio2 << "\n";
#endif
                bestEdges = edges2;
                skippedPrio = skippedPrio2;
                stops = stops2;
            }
        }
    }
    return bestEdges;
}


ConstMSEdgeVector
MSStopOptimizer::routeAlongStops(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
                               std::vector<StopEdgeInfo>& stops, ConstMSEdgeVector edges,
                               int originStop, SUMOTime maxDelay, double& skippedPrio2) const {
    // originStop was already reached and the edges appended
    for (int i = originStop + 1; i < (int)stops.size(); i++) {
        ConstMSEdgeVector into;
        if (!reachableInTime(t, router, edges.back(), stops[i - 1].pos, stops[i].edge, stops[i].pos, stops[i].arrival + maxDelay - t, into)) {
            if (stops[i].priority < 0) {
                // failure: cannot reach required stop
                skippedPrio2 = std::numeric_limits<double>::max();
                return edges;
            }
            skippedPrio2 += stops[i].priority;
            stops[i].skipped = true;
        } else {
            edges.pop_back();
            edges.insert(edges.end(), into.begin(), into.end());
        }
    }
    return edges;
}


bool
MSStopOptimizer::reachableInTime(SUMOTime t, SUMOAbstractRouter<MSEdge, SUMOVehicle>& router,
        const MSEdge* from, double fromPos,
        const MSEdge* to, double toPos,
        SUMOTime maxCost,
        ConstMSEdgeVector& into) const {
    router.compute(from, fromPos, to, toPos, myVehicle, t, into, true);
    if (into.size() > 0) {
        SUMOTime cost = TIME2STEPS(router.recomputeCostsPos(into, myVehicle, fromPos, toPos, t));
        if (cost <= maxCost) {
            return true;
        }
    }
    return false;
}



/****************************************************************************/
