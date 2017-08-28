/****************************************************************************/
/// @file    NBPTLineCont.cpp
/// @author  Gregor Laemmel
/// @author  Nikita Cherednychek
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// Container for NBPTLine during netbuild
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include <iostream>
#include <utils/common/MsgHandler.h>
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

//			//Laemmel
//            if (waysIdsIt == waysIds.end()){
//                for (auto& edgeCand : stop->getMyAdditionalEdgeCandidates() ){
//                    bool found = false;
//                    waysIdsIt =  waysIds.begin();
//                    for (; waysIdsIt != waysIds.end(); waysIdsIt++) {
//                        if ((*waysIdsIt) == edgeCand.first) {
//                            stop->setEdgeId(edgeCand.second);
//                            stop->setMyOrigEdgeId(edgeCand.first);
//                            origId = edgeCand.first;
//                            found = true;
//                            break;
//                        }
//                    }
//                    if (found){
//                        break;
//                    }
//                }
//            }


            if (waysIdsIt == waysIds.end()) {
                WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                      + ". Ignoring!");
                continue;
            }

            std::vector<long long int>* way = myPTLine->getWaysNodes(origId);
            if (way == 0) {
                WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                      + ". Ignoring!");
                continue;
            }

            int dir = FWD;
            if (waysIdsIt == (waysIds.end() - 1)) {
                std::string adjId = *(waysIdsIt - 1);
                std::vector<long long int>* way2 = myPTLine->getWaysNodes(adjId);
                if (way2 == 0) {
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                          + ". Ignoring!");
                    continue;
                }
                long long int wayEnds = *(way->end() - 1);
                long long int wayBegins = *(way->begin());
                long long int way2Ends = *(way2->end() - 1);
                long long int way2Begins = *(way2->begin());
                if (wayBegins == way2Ends || wayBegins == way2Begins) {
                    dir = FWD;
                } else if (wayEnds == way2Begins || wayEnds == way2Ends) {
                    dir = BWD;
                } else {
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                          + ". Ignoring!");
                    continue;
                }

            } else {
                std::string adjId = *(waysIdsIt + 1);
                std::vector<long long int>* way2 = myPTLine->getWaysNodes(adjId);
                if (way2 == 0) {
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                          + ". Ignoring!");
                    continue;
                }
                long long int wayEnds = *(way->end() - 1);
                long long int wayBegins = *(way->begin());
                long long int way2Ends = *(way2->end() - 1);
                long long int way2Begins = *(way2->begin());
                if (wayBegins == way2Ends || wayBegins == way2Begins) {
                    dir = BWD;
                } else if (wayEnds == way2Begins || wayEnds == way2Ends) {
                    dir = FWD;
                } else {
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                          + ". Ignoring!");
                    continue;
                }
            }


            std::string edgeId = stop->getEdgeId();
            NBEdge* current = cont.getByID(edgeId);
            int assingedTo = edgeId.at(0) == '-' ? BWD : FWD;

            if (dir != assingedTo) {
                NBEdge* reverse = NBPTStopCont::getReverseEdge(current);
                if (reverse == nullptr) {
                    WRITE_WARNING("Could not re-assign PT stop: " + stop->getID() + " probably broken osm file");
                    continue;
                }
                stop->setEdgeId(reverse->getID());
                NBPTStopCont::findLaneAndComputeBusStopExtend(stop, cont);
                WRITE_WARNING("PT stop: " + stop->getID() + " has been moved to edge: " + reverse->getID());
            }

        }

        //
        NBPTLine* pTLine = myPTLine;

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
            if (currentWayEdges.empty())
                continue;
            if (last == currentWayEdges.front()->getFromNode() && last != nullptr) {
                if (!prevWayEdges.empty()) {
                    pTLine->addEdgeVector(prevWayEdges);
                    prevWayEdges.clear();
                    prevWayMinusEdges.clear();
                }
                pTLine->addEdgeVector(currentWayEdges);
                last = currentWayEdges.back()->getToNode();
            } else if (last == currentWayEdges.back()->getToNode() && last != nullptr) {
                if (!prevWayEdges.empty()) {
                    pTLine->addEdgeVector(prevWayEdges);
                    prevWayEdges.clear();
                    prevWayMinusEdges.clear();
                }
                if (currentWayMinusEdges.empty()) {
                    currentWayEdges.clear();
                    last = nullptr;
                    continue;
                } else {
                    pTLine->addEdgeVector(currentWayMinusEdges);
                    last = currentWayMinusEdges.back()->getToNode();
                }
            } else if (first == currentWayEdges.front()->getFromNode() && first != nullptr) {
                pTLine->addEdgeVector(prevWayMinusEdges);
                pTLine->addEdgeVector(currentWayEdges);
                last = currentWayEdges.back()->getToNode();
                prevWayEdges.clear();
                prevWayMinusEdges.clear();
            } else if (first == currentWayEdges.back()->getToNode() && first != nullptr) {
                pTLine->addEdgeVector(prevWayMinusEdges);
                if (currentWayMinusEdges.empty()) {
                    currentWayEdges.clear();
                    last = nullptr;
                    prevWayEdges.clear();
                    prevWayMinusEdges.clear();
                    continue;
                } else {
                    pTLine->addEdgeVector(currentWayMinusEdges);
                    last = currentWayMinusEdges.back()->getToNode();
                    prevWayEdges.clear();
                    prevWayMinusEdges.clear();
                }
            } else {
                if (it3 != pTLine->getMyWays().begin())
                    std::cout << "Warning: incomplete route for " << pTLine->getName() << std::endl;
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
    }
}

