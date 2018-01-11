/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    NBPTLineCont.cpp
/// @author  Gregor Laemmel
/// @author  Nikita Cherednychek
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// Container for NBPTLine during netbuild
/****************************************************************************/

#include <iostream>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include "NBPTLineCont.h"
#include "NBPTStop.h"
#include "NBEdge.h"
#include "NBNode.h"
#include "NBPTStopCont.h"

// ===========================================================================
// static value definitions
// ===========================================================================
const int NBPTLineCont::FWD(1);
const int NBPTLineCont::BWD(-1);
// ===========================================================================
// method definitions
// ===========================================================================

NBPTLineCont::NBPTLineCont()
    :
    myIdCnt(0) { }


NBPTLineCont::~NBPTLineCont() {
    for (auto& myPTLine : myPTLines) {
        delete myPTLine;
    }
    myPTLines.clear();
}

void
NBPTLineCont::insert(NBPTLine* pLine) {
    pLine->setId(myIdCnt++);
    myPTLines.push_back(pLine);
}
void NBPTLineCont::process(NBEdgeCont& cont) {
    for (auto& myPTLine : myPTLines) {
        reviseStops(myPTLine, cont);

        constructRoute(myPTLine, cont);


        //



    }
}
void NBPTLineCont::reviseStops(NBPTLine* myPTLine, NBEdgeCont& cont) {
    std::vector<NBPTStop*> stops = myPTLine->getStops();
    for (auto& stop : stops) {
        //get the corresponding and one of the two adjacent ways
        std::string origId = stop->getOrigEdgeId();

        std::vector<std::string> waysIds = myPTLine->getMyWays();
        auto waysIdsIt = waysIds.begin();
        if (waysIds.size() <= 1) {
            WRITE_WARNING("Cannot revise pt stop localization for pt line: " + myPTLine->getName()
                          + ", which consist of one way only. Ignoring!");
            continue;
        }
        for (; waysIdsIt != waysIds.end(); waysIdsIt++) {
            if ((*waysIdsIt) == origId) {
                break;
            }
        }

        if (waysIdsIt == waysIds.end()) {
            for (auto& edgeCand : stop->getMyAdditionalEdgeCandidates()) {
                bool found = false;
                waysIdsIt =  waysIds.begin();
                for (; waysIdsIt != waysIds.end(); waysIdsIt++) {
                    if ((*waysIdsIt) == edgeCand.first) {
                        if (stop->setEdgeId(edgeCand.second, cont)) {
                            stop->setMyOrigEdgeId(edgeCand.first);
                            origId = edgeCand.first;
                            found = true;
                            break;
                        }
                    }
                }
                if (found) {
                    break;
                }
            }
        }


        if (waysIdsIt == waysIds.end()) {
            WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                          + ". Ignoring!");
            continue;
        }

        std::vector<long long int>* way = myPTLine->getWaysNodes(origId);
        if (way == nullptr) {
            WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                          + ". Ignoring!");
            continue;
        }


        int dir;
        std::string adjIdPrev;
        std::string adjIdNext;
        if (waysIdsIt != waysIds.begin()) {
            adjIdPrev = *(waysIdsIt - 1);
        }
        if (waysIdsIt != (waysIds.end() - 1)) {
            adjIdNext = *(waysIdsIt + 1);
        }
        std::vector<long long int>* wayPrev = myPTLine->getWaysNodes(adjIdPrev);
        std::vector<long long int>* wayNext = myPTLine->getWaysNodes(adjIdNext);
        if (wayPrev == nullptr && wayNext == nullptr) {
            WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                          + ". Ignoring!");
            continue;
        }
        long long int wayEnds = *(way->end() - 1);
        long long int wayBegins = *(way->begin());
        long long int wayPrevEnds = wayPrev != nullptr ? *(wayPrev->end() - 1) : 0;
        long long int wayPrevBegins = wayPrev != nullptr ? *(wayPrev->begin()) : 0;
        long long int wayNextEnds = wayNext != nullptr ? *(wayNext->end() - 1) : 0;
        long long int wayNextBegins = wayNext != nullptr ? *(wayNext->begin()) : 0;
        if (wayBegins == wayPrevEnds || wayBegins == wayPrevBegins || wayEnds == wayNextBegins
                || wayEnds == wayNextEnds) {
            dir = FWD;
        } else if (wayEnds == wayPrevBegins || wayEnds == wayPrevEnds || wayBegins == wayNextEnds
                   || wayBegins == wayNextBegins) {
            dir = BWD;
        } else {
            WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                          + ". Ignoring!");
            continue;
        }

        std::string edgeId = stop->getEdgeId();
        NBEdge* current = cont.getByID(edgeId);
        int assignedTo = edgeId.at(0) == '-' ? BWD : FWD;

        if (dir != assignedTo) {
            NBEdge* reverse = NBPTStopCont::getReverseEdge(current);
            if (reverse == nullptr) {
                WRITE_WARNING("Could not re-assign PT stop: " + stop->getID() + " probably broken osm file");
                continue;
            }
            stop->setEdgeId(reverse->getID(), cont);
            WRITE_WARNING("PT stop: " + stop->getID() + " has been moved to edge: " + reverse->getID());
        }
        myServedPTStops.insert(stop->getID());
    }
}
void NBPTLineCont::constructRoute(NBPTLine* pTLine, NBEdgeCont& cont) {


    std::vector<NBEdge*> edges;

    NBNode* first = nullptr;
    NBNode* last = nullptr;
    std::vector<NBEdge*> prevWayEdges;
    std::vector<NBEdge*> prevWayMinusEdges;
    prevWayEdges.clear();
    prevWayMinusEdges.clear();
    std::vector<NBEdge*> currentWayEdges;
    std::vector<NBEdge*> currentWayMinusEdges;
    for (auto it3 = pTLine->getMyWays().begin();
            it3 != pTLine->getMyWays().end(); it3++) {

        if (cont.retrieve(*it3, false) != nullptr) {
            currentWayEdges.push_back(cont.retrieve(*it3, false));
        } else {
            int i = 0;
            while (cont.retrieve(*it3 + "#" + std::to_string(i), false) != nullptr) {
                currentWayEdges.push_back(cont.retrieve(*it3 + "#" + std::to_string(i), false));
                i++;
            }
        }

        if (cont.retrieve("-" + *it3, false) != nullptr) {
            currentWayMinusEdges.push_back(cont.retrieve("-" + *it3, false));
        } else {
            int i = 0;
            while (cont.retrieve("-" + *it3 + "#" + std::to_string(i), false) != nullptr) {
                currentWayMinusEdges.insert(currentWayMinusEdges.begin(),
                                            cont.retrieve("-" + *it3 + "#" + std::to_string(i), false));
                i++;
            }
        }
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
                last = currentWayMinusEdges.back()->getToNode();
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
            if (it3 != pTLine->getMyWays().begin()) {
                WRITE_WARNING("Incomplete route for ptline '" + toString(pTLine->getLineID()) + "' (" + pTLine->getName() + ")");
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

    auto fr = edges.begin();
    NBPTStop* frStop = pTLine->getStops()[0];
    for (; fr != edges.end(); fr++) {
        if ((*fr)->getID() == frStop->getEdgeId()) {
            break;
        }
    }
    auto to = fr;
    NBPTStop* toStop = *(pTLine->getStops().end() - 1);
    for (; to != edges.end(); to++) {
        if ((*to)->getID() == toStop->getEdgeId()) {
            to++;
            break;
        }
    }

    pTLine->addEdgeVector(fr, to);
}


void
NBPTLineCont::addEdges2Keep(const OptionsCont& oc, std::set<std::string>& into) {
    if (oc.isSet("ptline-output")) {
        for (auto line : myPTLines) {
            for (auto edge : line->getRoute()) {
                into.insert(edge->getID());
            }
        }
    }
}
std::set<std::string>& NBPTLineCont::getServedPTStops() {
    return myServedPTStops;
}
