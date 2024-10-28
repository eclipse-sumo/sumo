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
/// @file    NBPTLineCont.cpp
/// @author  Gregor Laemmel
/// @author  Nikita Cherednychek
/// @date    Tue, 20 Mar 2017
///
// Container for NBPTLine during netbuild
/****************************************************************************/
#include <config.h>

#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <utils/router/DijkstraRouter.h>
#include "NBPTLineCont.h"
#include "NBPTStop.h"
#include "NBEdge.h"
#include "NBNode.h"
#include "NBVehicle.h"
#include "NBPTStopCont.h"

//#define DEBUG_FIND_WAY
//#define DEBUG_CONSTRUCT_ROUTE

#define DEBUGLINEID ""
#define DEBUGSTOPID ""

// ===========================================================================
// static value definitions
// ===========================================================================
const int NBPTLineCont::FWD(1);
const int NBPTLineCont::BWD(-1);


// ===========================================================================
// method definitions
// ===========================================================================
NBPTLineCont::~NBPTLineCont() {
    for (auto& myPTLine : myPTLines) {
        delete myPTLine.second;
    }
    myPTLines.clear();
}


bool
NBPTLineCont::insert(NBPTLine* ptLine) {
    if (myPTLines.count(ptLine->getLineID()) == 0) {
        myPTLines[ptLine->getLineID()] = ptLine;
        return true;
    }
    return false;
}


NBPTLine*
NBPTLineCont::retrieve(const std::string& lineID) {
    if (myPTLines.count(lineID) == 0) {
        return nullptr;
    } else {
        return myPTLines[lineID];
    }
}

void
NBPTLineCont::process(NBEdgeCont& ec, NBPTStopCont& sc, bool routeOnly) {
    for (auto& item : myPTLines) {
        NBPTLine* line = item.second;
        if (item.second->getWays().size() > 0) {
            // loaded from OSM rather than ptline input. We can use extra
            // information to reconstruct route and stops
            constructRoute(line, ec);
            if (!routeOnly) {
                // map stops to ways, using the constructed route for loose stops
                reviseStops(line, ec, sc);
            }
        }
        // fix circular line if necessary
        if (line->getStops().size() > 1
                && line->getStops().front() == line->getStops().back()
                && line->getRoute().size() > 1
                && line->getRoute().front() != line->getRoute().back()) {
            // we need to duplicate either the first or the last edge depending on the stop locations
            const std::string firstStopEdge = line->getStops().front()->getEdgeId();
            const std::string lastStopEdge = line->getStops().back()->getEdgeId();
            std::vector<NBEdge*> edges = line->getRoute();
            if (firstStopEdge == edges.back()->getID()) {
                edges.insert(edges.begin(), edges.back());
            } else if (lastStopEdge == edges.front()->getID()) {
                edges.push_back(edges.front());
            }
            line->setEdges(edges);
        }
        line->deleteInvalidStops(ec, sc);
        //line->deleteDuplicateStops();
        for (std::shared_ptr<NBPTStop> stop : line->getStops()) {
            myServedPTStops.insert(stop->getID());
        }
    }
}


void
NBPTLineCont::reviseStops(NBPTLine* line, const NBEdgeCont& ec, NBPTStopCont& sc) {
    const std::vector<std::string>& waysIds = line->getWays();
    if (waysIds.size() == 1 && line->getStops().size() > 1) {
        reviseSingleWayStops(line, ec, sc);
        return;
    }
    if (waysIds.size() <= 1) {
        WRITE_WARNINGF(TL("Cannot revise pt stop localization for pt line '%', which consist of one way only. Ignoring!"), line->getLineID());
        return;
    }
    if (line->getRoute().size() == 0) {
        WRITE_WARNINGF(TL("Cannot revise pt stop localization for pt line '%', which has no route edges. Ignoring!"), line->getLineID());
        return;
    }
    std::vector<std::shared_ptr<NBPTStop> > stops = line->getStops();
    for (std::shared_ptr<NBPTStop> stop : stops) {
        //get the corresponding and one of the two adjacent ways
        stop = findWay(line, stop, ec, sc);
        if (stop == nullptr) {
            // warning already given
            continue;
        }
        auto waysIdsIt = std::find(waysIds.begin(), waysIds.end(), stop->getOrigEdgeId());
        if (waysIdsIt == waysIds.end()) {
            // warning already given
            continue;
        }
        // find directional edge (OSM ways are bidirectional)
        const std::vector<long long int>* const way = line->getWayNodes(stop->getOrigEdgeId());
        if (way == nullptr) {
            WRITE_WARNINGF(TL("Cannot assign stop '%' on edge '%' to pt line '%' (wayNodes not found). Ignoring!"),
                           stop->getID(), stop->getOrigEdgeId(), line->getLineID());
            continue;
        }

        int dir;
        const std::vector<long long int>* wayPrev = nullptr;
        if (waysIdsIt != waysIds.begin()) {
            wayPrev = line->getWayNodes(*(waysIdsIt - 1));
        }
        const std::vector<long long int>* wayNext = nullptr;
        if (waysIdsIt != (waysIds.end() - 1)) {
            wayNext = line->getWayNodes(*(waysIdsIt + 1));
        }
        if (wayPrev == nullptr && wayNext == nullptr) {
            WRITE_WARNINGF(TL("Cannot revise pt stop localization for incomplete pt line '%'. Ignoring!"), line->getLineID());
            continue;
        }
        const long long int wayEnds = way->back();
        const long long int wayBegins = way->front();
        const long long int wayPrevEnds = wayPrev != nullptr ? wayPrev->back() : 0;
        const long long int wayPrevBegins = wayPrev != nullptr ? wayPrev->front() : 0;
        const long long int wayNextEnds = wayNext != nullptr ? wayNext->back() : 0;
        const long long int wayNextBegins = wayNext != nullptr ? wayNext->front() : 0;
        if (wayBegins == wayPrevEnds || wayBegins == wayPrevBegins || wayEnds == wayNextBegins || wayEnds == wayNextEnds) {
            dir = FWD;
        } else if (wayEnds == wayPrevBegins || wayEnds == wayPrevEnds || wayBegins == wayNextEnds || wayBegins == wayNextBegins) {
            dir = BWD;
        } else {
            WRITE_WARNINGF(TL("Cannot revise pt stop localization for incomplete pt line '%'. Ignoring!"), line->getLineID());
            continue;
        }

        std::string edgeId = stop->getEdgeId();
        NBEdge* current = ec.getByID(edgeId);
        int assignedTo = edgeId.at(0) == '-' ? BWD : FWD;

        if (dir != assignedTo) {
            NBEdge* reverse = NBPTStopCont::getReverseEdge(current);
            if (reverse == nullptr) {
                WRITE_WARNINGF(TL("Could not re-assign PT stop '%', probably broken osm file."), stop->getID());
                continue;
            }
            if (stop->getLines().size() > 0) {
                std::shared_ptr<NBPTStop> reverseStop = sc.getReverseStop(stop, ec);
                sc.insert(reverseStop);
                line->replaceStop(stop, reverseStop);
                stop = reverseStop;
            } else {
                WRITE_WARNINGF(TL("PT stop '%' has been moved to edge '%'."), stop->getID(), reverse->getID());
            }
            stop->setEdgeId(reverse->getID(), ec);
        }
        stop->addLine(line->getRef());
    }
}


void NBPTLineCont::reviseSingleWayStops(NBPTLine* line, const NBEdgeCont& ec, NBPTStopCont& sc) {
    const std::vector<std::string>& waysIds = line->getWays();
    for (std::shared_ptr<NBPTStop> stop : line->getStops()) {
        //get the corresponding and one of the two adjacent ways
        stop = findWay(line, stop, ec, sc);
        if (stop == nullptr) {
            // warning already given
            continue;
        }
        auto waysIdsIt = std::find(waysIds.begin(), waysIds.end(), stop->getOrigEdgeId());
        if (waysIdsIt == waysIds.end()) {
            // warning already given
            continue;
        }
        stop->addLine(line->getRef());
    }
}


std::shared_ptr<NBPTStop>
NBPTLineCont::findWay(NBPTLine* line, std::shared_ptr<NBPTStop> stop, const NBEdgeCont& ec, NBPTStopCont& sc) const {
    const std::vector<std::string>& waysIds = line->getWays();
#ifdef DEBUG_FIND_WAY
    if (stop->getID() == DEBUGSTOPID) {
        std::cout << " stop=" << stop->getID() << " line=" << line->getLineID() << " edgeID=" << stop->getEdgeId() << " origID=" << stop->getOrigEdgeId() << "\n";
    }
#endif
    if (stop->isLoose()) {
        // find closest edge in route
        double minDist = std::numeric_limits<double>::max();
        NBEdge* best = nullptr;
        for (NBEdge* edge : line->getRoute()) {
            const double dist = edge->getLaneShape(0).distance2D(stop->getPosition());
            if (dist < minDist) {
                best = edge;
                minDist = dist;
            }
        }
#ifdef DEBUG_FIND_WAY
        if (stop->getID() == DEBUGSTOPID) {
            std::cout << "   best=" << Named::getIDSecure(best) << " minDist=" << minDist << " wayID=" << getWayID(best->getID())
                      << " found=" << (std::find(waysIds.begin(), waysIds.end(), getWayID(best->getID())) != waysIds.end())
                      << " wayIDs=" << toString(waysIds) << "\n";
        }
#endif
        if (minDist < OptionsCont::getOptions().getFloat("ptline.match-dist")) {
            const std::string wayID = getWayID(best->getID());
            if (stop->getEdgeId() == "") {
                stop->setEdgeId(best->getID(), ec);
                stop->setOrigEdgeId(wayID);
            } else if (stop->getEdgeId() != best->getID()) {
                // stop is used by multiple lines and mapped to different edges.
                // check if an alternative stop already exists
                std::shared_ptr<NBPTStop> newStop = sc.findStop(wayID, stop->getPosition());
                if (newStop == nullptr) {
                    newStop = std::make_shared<NBPTStop>(stop->getID() + "@" + line->getLineID(), stop->getPosition(), best->getID(), wayID, stop->getLength(), stop->getName(), stop->getPermissions());
                    newStop->setEdgeId(best->getID(), ec);  // trigger lane assignment
                    sc.insert(newStop);
                }
                line->replaceStop(stop, newStop);
                stop = newStop;
            }
        } else {
            WRITE_WARNINGF(TL("Could not assign stop '%' to pt line '%' (closest edge '%', distance %). Ignoring!"),
                           stop->getID(), line->getLineID(), Named::getIDSecure(best), minDist);
            return nullptr;
        }
    } else {
        // if the stop is part of an edge, find that edge among the line edges
        auto waysIdsIt = waysIds.begin();
        for (; waysIdsIt != waysIds.end(); waysIdsIt++) {
            if ((*waysIdsIt) == stop->getOrigEdgeId()) {
                break;
            }
        }

        if (waysIdsIt == waysIds.end()) {
            // stop edge not found, try additional edges
            for (auto& edgeCand : stop->getAdditionalEdgeCandidates()) {
                bool found = false;
                waysIdsIt =  waysIds.begin();
                for (; waysIdsIt != waysIds.end(); waysIdsIt++) {
                    if ((*waysIdsIt) == edgeCand.first) {
                        if (stop->setEdgeId(edgeCand.second, ec)) {
                            stop->setOrigEdgeId(edgeCand.first);
                            found = true;
                            break;
                        }
                    }
                }
                if (found) {
                    break;
                }
            }
            if (waysIdsIt == waysIds.end()) {
                WRITE_WARNINGF(TL("Cannot assign stop % on edge '%' to pt line '%'. Ignoring!"), stop->getID(), stop->getOrigEdgeId(), line->getLineID());
            }
        }
    }
    return stop;
}


void NBPTLineCont::constructRoute(NBPTLine* pTLine, const NBEdgeCont& cont) {
    std::vector<NBEdge*> edges;

    NBNode* first = nullptr;
    NBNode* last = nullptr;
    std::vector<NBEdge*> prevWayEdges;
    std::vector<NBEdge*> prevWayMinusEdges;
    std::vector<NBEdge*> currentWayEdges;
    std::vector<NBEdge*> currentWayMinusEdges;
    for (auto it3 = pTLine->getWays().begin(); it3 != pTLine->getWays().end(); it3++) {

        int foundForward = 0;
        if (cont.retrieve(*it3, false) != nullptr) {
            currentWayEdges.push_back(cont.retrieve(*it3, false));
            foundForward++;
        } else {
            int i = 0;
            while (cont.retrieve(*it3 + "#" + std::to_string(i), true) != nullptr) {
                if (cont.retrieve(*it3 + "#" + std::to_string(i), false)) {
                    currentWayEdges.push_back(cont.retrieve(*it3 + "#" + std::to_string(i), false));
                    foundForward++;
                }
                i++;
            }
        }

        int foundReverse = 0;
        if (cont.retrieve("-" + *it3, false) != nullptr) {
            currentWayMinusEdges.push_back(cont.retrieve("-" + *it3, false));
            foundReverse++;
        } else {
            int i = 0;
            while (cont.retrieve("-" + *it3 + "#" + std::to_string(i), true) != nullptr) {
                if (cont.retrieve("-" + *it3 + "#" + std::to_string(i), false)) {
                    currentWayMinusEdges.insert(currentWayMinusEdges.end() - foundReverse,
                                                cont.retrieve("-" + *it3 + "#" + std::to_string(i), false));
                    foundReverse++;
                }
                i++;
            }
        }
        bool fakeMinus = false;
        if (foundReverse == 0 && foundForward > 0 && isRailway(pTLine->getVClass())) {
            // rail tracks may be used in both directions and are often not tagged as such.
            // This can be repaired later with option --railway.topology.repair
            currentWayMinusEdges.insert(currentWayMinusEdges.begin(), currentWayEdges.rbegin(), currentWayEdges.rbegin() + foundForward);
            fakeMinus = true;
        }
#ifdef DEBUG_CONSTRUCT_ROUTE
        if (pTLine->getLineID() == DEBUGLINEID) {
            std::cout << " way=" << (*it3)
                      << " done=" << toString(edges)
                      << " first=" << Named::getIDSecure(first)
                      << " last=" << Named::getIDSecure(last)
                      << "\n    +=" << toString(currentWayEdges)
                      << "\n    -=" << toString(currentWayMinusEdges)
                      << "\n   p+=" << toString(prevWayEdges)
                      << "\n   p-=" << toString(prevWayMinusEdges)
                      << "\n";
        }
#endif
        if (currentWayEdges.empty()) {
            continue;
        }
        if (last == currentWayEdges.front()->getFromNode() && last != nullptr) {
            if (!prevWayEdges.empty()) {
                edges.insert(edges.end(), prevWayEdges.begin(), prevWayEdges.end());
                prevWayEdges.clear();
                prevWayMinusEdges.clear();
            }
            edges.insert(edges.end(), currentWayEdges.begin(), currentWayEdges.end());
            last = currentWayEdges.back()->getToNode();
        } else if (last == currentWayEdges.back()->getToNode() && last != nullptr) {
            if (!prevWayEdges.empty()) {
                edges.insert(edges.end(), prevWayEdges.begin(), prevWayEdges.end());
                prevWayEdges.clear();
                prevWayMinusEdges.clear();
            }
            if (currentWayMinusEdges.empty()) {
                currentWayEdges.clear();
                last = nullptr;
                continue;
            } else {
                edges.insert(edges.end(), currentWayMinusEdges.begin(), currentWayMinusEdges.end());
                if (fakeMinus) {
                    last = currentWayMinusEdges.back()->getFromNode();
                } else {
                    last = currentWayMinusEdges.back()->getToNode();
                }
            }
        } else if (first == currentWayEdges.front()->getFromNode() && first != nullptr) {
            edges.insert(edges.end(), prevWayMinusEdges.begin(), prevWayMinusEdges.end());
            edges.insert(edges.end(), currentWayEdges.begin(), currentWayEdges.end());
            last = currentWayEdges.back()->getToNode();
            prevWayEdges.clear();
            prevWayMinusEdges.clear();
        } else if (first == currentWayEdges.back()->getToNode() && first != nullptr) {
            edges.insert(edges.end(), prevWayMinusEdges.begin(), prevWayMinusEdges.end());
            if (currentWayMinusEdges.empty()) {
                currentWayEdges.clear();
                last = nullptr;
                prevWayEdges.clear();
                prevWayMinusEdges.clear();
                continue;
            } else {
                edges.insert(edges.end(), currentWayMinusEdges.begin(), currentWayMinusEdges.end());
                last = currentWayMinusEdges.back()->getToNode();
                prevWayEdges.clear();
                prevWayMinusEdges.clear();
            }
        } else {
            if (it3 != pTLine->getWays().begin()) {
#ifdef DEBUG_CONSTRUCT_ROUTE
                if (pTLine->getLineID() == DEBUGLINEID) {
                    std::cout << " way " << (*it3)
                              << " is not the start of ptline " << pTLine->getLineID()
                              << " (" + pTLine->getName() + ")\n";
                }
#endif
            } else if (pTLine->getWays().size() == 1) {
                if (currentWayEdges.size() > 0) {
                    edges.insert(edges.end(), currentWayEdges.begin(), currentWayEdges.end());
                } else {
                    edges.insert(edges.end(), currentWayMinusEdges.begin(), currentWayMinusEdges.end());
                }
            }
            prevWayEdges = currentWayEdges;
            prevWayMinusEdges = currentWayMinusEdges;
            if (!prevWayEdges.empty()) {
                first = prevWayEdges.front()->getFromNode();
                last = prevWayEdges.back()->getToNode();
            } else {
                first = nullptr;
                last = nullptr;
            }
        }
        currentWayEdges.clear();
        currentWayMinusEdges.clear();
    }
    pTLine->setEdges(edges);
}


void
NBPTLineCont::replaceEdge(const std::string& edgeID, const EdgeVector& replacement) {
    //std::cout << " replaceEdge " << edgeID << " replacement=" << toString(replacement) << "\n";
    if (myPTLines.size() > 0 && myPTLineLookup.size() == 0) {
        // init lookup once
        for (auto& item : myPTLines) {
            for (const NBEdge* e : item.second->getRoute()) {
                myPTLineLookup[e->getID()].insert(item.second);
            }
        }
    }
    for (NBPTLine* line : myPTLineLookup[edgeID]) {
        line->replaceEdge(edgeID, replacement);
        for (const NBEdge* e : replacement) {
            myPTLineLookup[e->getID()].insert(line);
        }
    }
    myPTLineLookup.erase(edgeID);
}


std::set<std::string>&
NBPTLineCont::getServedPTStops() {
    return myServedPTStops;
}


void
NBPTLineCont::fixBidiStops(const NBEdgeCont& ec) {
    std::map<std::string, SUMOVehicleClass> types;
    types["bus"] = SVC_BUS;
    types["minibus"] = SVC_BUS;
    types["trolleybus"] = SVC_BUS;
    types["tram"] = SVC_TRAM;
    types["train"] = SVC_RAIL;
    types["subway"] = SVC_RAIL_URBAN;
    types["light_rail"] = SVC_RAIL_URBAN;
    types["monorail"] = SVC_RAIL_URBAN;
    types["aerialway"] = SVC_RAIL_URBAN;
    types["ferry"] = SVC_SHIP;

    SUMOAbstractRouter<NBRouterEdge, NBVehicle>* const router = new DijkstraRouter<NBRouterEdge, NBVehicle>(
        ec.getAllRouterEdges(), true, &NBRouterEdge::getTravelTimeStatic, nullptr, true);

    for (auto& item : myPTLines) {
        NBPTLine* line = item.second;
        std::vector<std::shared_ptr<NBPTStop> > stops = line->getStops();
        if (stops.size() < 2) {
            continue;
        }
        if (types.count(line->getType()) == 0) {
            WRITE_WARNINGF(TL("Could not determine vehicle class for public transport line of type '%'."), line->getType());
            continue;
        }
        NBVehicle veh(line->getRef(), types[line->getType()]);
        std::vector<std::shared_ptr<NBPTStop> > newStops;
        std::shared_ptr<NBPTStop> from = nullptr;
        for (auto it = stops.begin(); it != stops.end(); ++it) {
            std::shared_ptr<NBPTStop> to = *it;
            std::shared_ptr<NBPTStop> used = *it;
            if (to->getBidiStop() != nullptr) {
                double best = std::numeric_limits<double>::max();
                std::shared_ptr<NBPTStop> to2 = to->getBidiStop();
                if (from == nullptr) {
                    if ((it + 1) != stops.end()) {
                        from = to;
                        std::shared_ptr<NBPTStop> from2 = to2;
                        to = *(it + 1);
                        const double c1 = getCost(ec, *router, from, to, &veh);
                        const double c2 = getCost(ec, *router, from2, to, &veh);
                        //std::cout << " from=" << from->getID() << " to=" << to->getID() << " c1=" << MIN2(10000.0, c1) << "\n";
                        //std::cout << " from2=" << from2->getID() << " to=" << to->getID() << " c2=" << MIN2(10000.0, c2) << "\n";
                        best = c1;
                        if (to->getBidiStop() != nullptr) {
                            to2 = to->getBidiStop();
                            const double c3 = getCost(ec, *router, from, to2, &veh);
                            const double c4 = getCost(ec, *router, from2, to2, &veh);
                            //std::cout << " from=" << from->getID() << " to2=" << to2->getID() << " c3=" << MIN2(10000.0, c3) << "\n";
                            //std::cout << " from2=" << from2->getID() << " to2=" << to2->getID() << " c4=" << MIN2(10000.0, c4) << "\n";
                            if (c2 < best) {
                                used = from2;
                                best = c2;
                            }
                            if (c3 < best) {
                                used = from;
                                best = c3;
                            }
                            if (c4 < best) {
                                used = from2;
                                best = c4;
                            }
                        } else {
                            if (c2 < c1) {
                                used = from2;
                                best = c2;
                            } else {
                                best = c1;
                            }
                        }
                    }
                } else {
                    const double c1 = getCost(ec, *router, from, to, &veh);
                    const double c2 = getCost(ec, *router, from, to2, &veh);
                    //std::cout << " from=" << from->getID() << " to=" << to->getID() << " c1=" << MIN2(10000.0, c1) << "\n";
                    //std::cout << " from=" << from->getID() << " t2o=" << to2->getID() << " c2=" << MIN2(10000.0, c2) << "\n";
                    if (c2 < c1) {
                        used = to2;
                        best = c2;
                    } else {
                        best = c1;
                    }

                }
                if (best < std::numeric_limits<double>::max()) {
                    from = used;
                } else {
                    WRITE_WARNINGF(TL("Could not determine direction for line '%' at stop '%'."), line->getLineID(), used->getID());
                }
            }
            from = used;
            newStops.push_back(used);
        }
        assert(stops.size() == newStops.size());
        line->replaceStops(newStops);
    }
    delete router;
}


void
NBPTLineCont::removeInvalidEdges(const NBEdgeCont& ec) {
    for (auto& item : myPTLines) {
        item.second->removeInvalidEdges(ec);
    }
}


void
NBPTLineCont::fixPermissions() {
    for (auto& item : myPTLines) {
        NBPTLine* line = item.second;
        const std::vector<NBEdge*>& route = line->getRoute();
        const SUMOVehicleClass svc = line->getVClass();
        for (int i = 1; i < (int)route.size(); i++) {
            NBEdge* e1 = route[i - 1];
            NBEdge* e2 = route[i];
            std::vector<NBEdge::Connection> cons = e1->getConnectionsFromLane(-1, e2, -1);
            if (cons.size() == 0) {
                //WRITE_WARNINGF(TL("Disconnected ptline '%' between edge '%' and edge '%'"), line->getLineID(), e1->getID(), e2->getID());
            } else {
                bool ok = false;
                for (const auto& c : cons) {
                    if ((e1->getPermissions(c.fromLane) & svc) == svc) {
                        ok = true;
                        break;
                    }
                }
                if (!ok) {
                    int lane = cons[0].fromLane;
                    e1->setPermissions(e1->getPermissions(lane) | svc, lane);
                }
            }
        }
    }
}


double
NBPTLineCont::getCost(const NBEdgeCont& ec, SUMOAbstractRouter<NBRouterEdge, NBVehicle>& router,
                      const std::shared_ptr<NBPTStop> from, const std::shared_ptr<NBPTStop> to, const NBVehicle* veh) {
    NBEdge* fromEdge = ec.getByID(from->getEdgeId());
    NBEdge* toEdge = ec.getByID(to->getEdgeId());
    if (fromEdge == nullptr || toEdge == nullptr) {
        return std::numeric_limits<double>::max();
    } else if (fromEdge == toEdge) {
        if (from->getEndPos() <= to->getEndPos()) {
            return to->getEndPos() - from->getEndPos();
        } else {
            return std::numeric_limits<double>::max();
        }
    } else if (fromEdge->getBidiEdge() == toEdge) {
        return std::numeric_limits<double>::max();
    }
    std::vector<const NBRouterEdge*> route;
    router.compute(fromEdge, toEdge, veh, 0, route);
    if (route.size() == 0) {
        return std::numeric_limits<double>::max();
    } else {
        return router.recomputeCosts(route, veh, 0);
    }
}


std::string
NBPTLineCont::getWayID(const std::string& edgeID) {
    std::size_t found = edgeID.rfind("#");
    std::string result = edgeID;
    if (found != std::string::npos) {
        result = edgeID.substr(0, found);
    }
    if (result[0] == '-') {
        result = result.substr(1);
    }
    return result;
}


/****************************************************************************/
