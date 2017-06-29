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
    for (std::vector<NBPTLine*>::iterator it = myPTLines.begin(); it != myPTLines.end(); it++) {
        std::vector<NBPTStop*> stops = (*it)->getStops();
        for (std::vector<NBPTStop*>::iterator it2 = stops.begin(); it2 != stops.end(); it2++) {
            //get the corresponding and one of the two adjacent ways
            std::string origId = (*it2)->getOrigEdgeId();
            (*it2)->getEdgeId();
            std::vector<std::string> waysIds = (*it)->getMyWays();
            std::vector<std::string>::iterator waysIdsIt = waysIds.begin();
            if (waysIds.size() <= 1) {
                WRITE_WARNING("Cannot revise pt stop localization for pt line: " + (*it)->getName()
                                      + ", which consist of one way only. Ignoring!");
                continue;
            }
            for (; waysIdsIt != waysIds.end(); waysIdsIt++) {
                if ((*waysIdsIt) == origId) {
                    break;
                }
            }
            if (waysIdsIt == waysIds.end()) {
                WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + (*it)->getName()
                                      + ". Ignoring!");
                continue;
            }

            std::vector<long long int>* way = (*it)->getWaysNodes(origId);
            if (way == 0) {
                WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + (*it)->getName()
                                      + ". Ignoring!");
                continue;
            }

            int dir = FWD;
            if (waysIdsIt == (waysIds.end() - 1)) {
                std::string adjId = *(waysIdsIt - 1);
                std::vector<long long int>* way2 = (*it)->getWaysNodes(adjId);
                if (way2 == 0) {
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + (*it)->getName()
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
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + (*it)->getName()
                                          + ". Ignoring!");
                    continue;
                }

            } else {
                std::string adjId = *(waysIdsIt + 1);
                std::vector<long long int>* way2 = (*it)->getWaysNodes(adjId);
                if (way2 == 0) {
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + (*it)->getName()
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
                    WRITE_WARNING("Cannot revise pt stop localization for incomplete pt line: " + (*it)->getName()
                                          + ". Ignoring!");
                    continue;
                }
            }


            std::string edgeId = (*it2)->getEdgeId();
            NBEdge * current = cont.getByID(edgeId);
            int assingedTo = edgeId.at(0) == '-' ? BWD : FWD;

            if (dir != assingedTo) {
                NBEdge * reverse = NBPTStopCont::getReverseEdge(current);
                (*it2)->setEdgeId(reverse->getID());
                NBPTStopCont::findLaneAndComputeBusStopExtend(*it2,cont);
                WRITE_WARNING("PT stop: " + (*it2)->getID() + " has been moved to edge: " + reverse->getID());
            }

        }


    }

}

