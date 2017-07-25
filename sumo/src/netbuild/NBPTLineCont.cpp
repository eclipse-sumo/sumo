/****************************************************************************/
/// @file    NBPTLineCont.cpp
/// @author  Gregor Laemmel
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
    for (std::vector<NBPTLine*>::iterator it = myPTLines.begin(); it != myPTLines.end(); it++) {
        delete *it;
    }
    myPTLines.clear();
}

void
NBPTLineCont::insert(NBPTLine* pLine) {
//    std::cout << "new line: " << pLine->getName() << " stops: " << pLine->getStops().size() << std::endl;
    pLine->setId(myIdCnt++);
    myPTLines.push_back(pLine);
}
void NBPTLineCont::process(NBEdgeCont& cont) {
    for (auto& myPTLine : myPTLines) {
        std::vector<NBPTStop*> stops = myPTLine->getStops();
        std::vector<std::string> waysIds = myPTLine->getMyWays();
        auto waysIdsIt = waysIds.begin();

        for (auto& stop : stops) {
            //get the corresponding and one of the two adjacent ways
            std::string origId = stop->getOrigEdgeId();



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
            if (waysIdsIt == waysIds.end()){
                for (auto& edgeCand : stop->getMyAdditionalEdgeCandidates() ){
                    bool found = false;
                    waysIdsIt =  waysIds.begin();
                    for (; waysIdsIt != waysIds.end(); waysIdsIt++) {
                        if ((*waysIdsIt) == edgeCand.first) {
                            stop->setEdgeId(edgeCand.second);
                            stop->setMyOrigEdgeId(edgeCand.first);
                            origId = edgeCand.first;
                            found = true;
                            break;
                        }
                    }
                    if (found){
                        break;
                    }
                }
            }


            if (waysIdsIt == waysIds.end()) {
                WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                      + ". Ignoring!");
                waysIdsIt = waysIds.begin();
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
                long long int wayEnds = *(way->end()-1);
                long long int wayBegins = *(way->begin());
                long long int way2Ends = *(way2->end()-1);
                long long int way2Begins = *(way2->begin());
                if (wayBegins == way2Ends || wayBegins == way2Begins) {
                    dir = FWD;
                } else if (wayEnds == way2Begins || wayEnds == way2Ends){
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
                long long int wayEnds = *(way->end()-1);
                long long int wayBegins = *(way->begin());
                long long int way2Ends = *(way2->end()-1);
                long long int way2Begins = *(way2->begin());
                if (wayBegins == way2Ends || wayBegins == way2Begins) {
                    dir = BWD;
                } else if (wayEnds == way2Begins || wayEnds == way2Ends){
                    dir = FWD;
                } else {
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + myPTLine->getName()
                                          + ". Ignoring!");
                    continue;
                }
            }


            std::string edgeId = stop->getEdgeId();
            NBEdge * current = cont.getByID(edgeId);
            int assingedTo = edgeId.at(0) == '-' ? BWD : FWD;

            if (dir != assingedTo) {
                NBEdge * reverse = NBPTStopCont::getReverseEdge(current);
                if (reverse == nullptr) {
                    WRITE_WARNING("Could not re-assign PT stop: " + stop->getID()  + " probably broken osm file");
                    continue;
                }
                stop->setEdgeId(reverse->getID());
                NBPTStopCont::findLaneAndComputeBusStopExtend(stop,cont);
                WRITE_WARNING("PT stop: " + stop->getID() + " has been moved to edge: " + reverse->getID());
            }

        }


    }

}

