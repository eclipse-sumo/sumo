/****************************************************************************/
/// @file    NBPTStopCont.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id$
///
// Container for pt stops during the netbuilding process
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


#include <utils/common/MsgHandler.h>
#include <microsim/MSLane.h>
#include "NBPTStopCont.h"
#include "NBEdgeCont.h"
#include "NBEdge.h"
#include "NBNode.h"
#include <utils/geom/Position.h>


NBPTStopCont::~NBPTStopCont() {
    for (auto& myPTStop : myPTStops) {
        delete myPTStop.second;
    }
    myPTStops.clear();
}

bool NBPTStopCont::insert(NBPTStop* ptStop) {
    std::string id = ptStop->getID();
    auto i = myPTStops.find(id);
    if (i != myPTStops.end()) {
        return false;
    }
    myPTStops[id] = ptStop;
    return true;
}

NBPTStop* NBPTStopCont::get(std::string id) {
    if (myPTStops.find(id) != myPTStops.end()) {
        return myPTStops.find(id)->second;
    }
    return nullptr;
}


void NBPTStopCont::process(NBEdgeCont& cont) {

//    PTStopsCont::iterator end = myPTStops.end();

    std::vector<NBPTStop*> reverseStops;

    //frst pass localize pt stop at correct side of the street; create stop for opposite side if needed
    for (auto& myPTStop : myPTStops) {

        NBPTStop* stop = myPTStop.second;

        bool multipleStopPositions = stop->getIsMultipleStopPositions();
        bool platformsDefined = !stop->getPlatformCands().empty();
        if (!platformsDefined) {
            //create pt stop for reverse edge if edge exist
            NBPTStop* reverseStop = getReverseStop(stop, cont);
            if (reverseStop != 0) {
                reverseStops.push_back(reverseStop);
            }
        } else if (multipleStopPositions) {
            //create pt stop for closest platform at corresponding edge
            assignPTStopToEdgeOfClosestPlatform(stop, cont);

        } else {
            //create pt stop for each side of the street where a platform is defined (create additional pt stop as needed)
            NBPTStop* additionalStop = assignAndCreatNewPTStopAsNeeded(stop, cont);
            if (additionalStop != 0) {
                reverseStops.push_back(additionalStop);
            }
        }
    }

    //insrt new stops if any
    for (auto& reverseStop : reverseStops) {
        insert(reverseStop);
    }


    //scnd pass set correct lane
    for (auto i = myPTStops.begin(); i != myPTStops.end();) {
        NBPTStop* stop = i->second;
        if (!NBPTStopCont::findLaneAndComputeBusStopExtend(stop, cont)) {
            WRITE_WARNING("Could not find corresponding edge or compatible lane for pt stop: " + i->second->getName()
                                  + ". Thus, it will be removed!");
            EdgeVector edgeVector = cont.getGeneratedFrom((*i).second->getOrigEdgeId());
            //std::cout << edgeVector.size() << std::endl;
            myPTStops.erase(i++);
        } else {
            i++;
        };
    }

}


NBPTStop* NBPTStopCont::getReverseStop(NBPTStop* pStop, NBEdgeCont& cont) {
    std::string edgeId = pStop->getEdgeId();
    NBEdge* edge = cont.getByID(edgeId);
    NBEdge* reverse = NBPTStopCont::getReverseEdge(edge);
    if (reverse != nullptr) {

        Position* pos = new Position(pStop->getPosition());
        NBPTStop* ret = new NBPTStop("-1" + pStop->getID(), *pos, reverse->getID(), reverse->getID(),
                                     pStop->getLength(), pStop->getName(), pStop->getPermissions());
        return ret;

    }
    return nullptr;
}

NBPTStop* NBPTStopCont::assignAndCreatNewPTStopAsNeeded(NBPTStop* pStop, NBEdgeCont& cont) {
    std::string edgeId = pStop->getEdgeId();
    NBEdge* edge = cont.getByID(edgeId);


    bool rightOfEdge = false;
    bool leftOfEdge = false;
    NBPTPlatform* left = nullptr;
    for (auto it = pStop->getPlatformCands().begin(); it != pStop->getPlatformCands().end(); it++) {
        NBPTPlatform* platform = &(*it);
        double crossProd = computeCrossProductEdgePosition(edge, platform->getMyPos());

        //TODO consider driving on the left!!! [GL May '17]
        if (crossProd > 0) {
            leftOfEdge = true;
            left = platform;
        } else {
            rightOfEdge = true;
            pStop->setMyPTStopLength(platform->getMyLength());
        }

    }

    if (leftOfEdge && rightOfEdge) {
        NBPTStop* leftStop = getReverseStop(pStop, cont);
        leftStop->setMyPTStopLength(left->getMyLength());
        return leftStop;
    } else if (leftOfEdge) {
        NBEdge* reverse = getReverseEdge(edge);
        if (reverse != nullptr) {
            pStop->setEdgeId(reverse->getID());
            pStop->setMyPTStopLength(left->getMyLength());
        }
    }

    return nullptr;
}


void NBPTStopCont::assignPTStopToEdgeOfClosestPlatform(NBPTStop* pStop, NBEdgeCont& cont) {
    std::string edgeId = pStop->getEdgeId();
    NBEdge* edge = cont.getByID(edgeId);
    NBEdge* reverse = NBPTStopCont::getReverseEdge(edge);
    NBPTPlatform* closestPlatform = getClosestPlatformToPTStopPosition(pStop);
    pStop->setMyPTStopLength(closestPlatform->getMyLength());
    if (reverse != 0) {

        //TODO make isLeft in PositionVector static [GL May '17]
//        if (PositionVector::isLeft(edge->getFromNode()->getPosition(),edge->getToNode()->getPosition(),closestPlatform)){
//
//        }
        double crossProd = computeCrossProductEdgePosition(edge, closestPlatform->getMyPos());

        //TODO consider driving on the left!!! [GL May '17]
        if (crossProd > 0) { //pt stop is on the left of the orig edge
            pStop->setEdgeId(reverse->getID());
        }
    }


}
double NBPTStopCont::computeCrossProductEdgePosition(const NBEdge* edge, const Position* closestPlatform) const {
    PositionVector geom = edge->getGeometry();
    int idxTmp = geom.indexOfClosest(*closestPlatform);
    double offset = geom.nearest_offset_to_point2D(*closestPlatform, true);
    double offset2 = geom.offsetAtIndex2D(idxTmp);
    int idx1, idx2;
    if (offset2 < offset) {
        idx1 = idxTmp;
        idx2 = idx1 + 1;
    } else {
        idx2 = idxTmp;
        idx1 = idxTmp - 1;
    }
    if (idx1 < 0 || idx1 >= (int) geom.size() || idx2 < 0 || idx2 >= (int) geom.size()) {
        WRITE_WARNING("Could not determine cross product");
        return 0;
    }
    Position p1 = geom[idx1];
    Position p2 = geom[idx2];

    double x0 = p1.x();
    double y0 = p1.y();
    double x1 = p2.x();
    double y1 = p2.y();
    double x2 = closestPlatform->x();
    double y2 = closestPlatform->y();
    double crossProd = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);
    return crossProd;
}
NBPTPlatform* NBPTStopCont::getClosestPlatformToPTStopPosition(NBPTStop* pStop) {

    Position stopPosition = pStop->getPosition();

    NBPTPlatform* closest = nullptr;
    double minSqrDist = std::numeric_limits<double>::max();

    for (auto it = pStop->getPlatformCands().begin();
         it != pStop->getPlatformCands().end();
         it++) {
        NBPTPlatform platform = *it;
        double sqrDist = stopPosition.distanceSquaredTo2D(*platform.getMyPos());
        if (sqrDist < minSqrDist) {
            minSqrDist = sqrDist;
            closest = &(*it);
        }
    }
    return closest;
}

//static functions

bool NBPTStopCont::findLaneAndComputeBusStopExtend(NBPTStop* pStop, NBEdgeCont& cont) {
    std::string edgeId = pStop->getEdgeId();
    NBEdge* edge = cont.getByID(edgeId);
    if (edge != nullptr) {

        int laneNr = -1;

        for (const auto& it : edge->getLanes()) {
            if ((it.permissions & pStop->getPermissions()) > 0) {
                ++laneNr;
                break;
            }
            laneNr++;
        }

        if (laneNr != -1) {
            const std::string& lane = edge->getLaneID(laneNr);
            pStop->setLaneID(lane);
            const PositionVector& shape = edge->getLaneShape(laneNr);
            double offset = shape.nearest_offset_to_point2D(pStop->getPosition(), true);
            pStop->computExtent(offset, shape.length());

        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;
}

NBEdge* NBPTStopCont::getReverseEdge(NBEdge* edge) {
    if (edge != nullptr) {
        for (auto it = edge->getToNode()->getOutgoingEdges().begin();
             it != edge->getToNode()->getOutgoingEdges().end();
             it++) {
            if ((*it)->getToNode() == edge->getFromNode()) {
                return (*it);
            }
        }
    }
    return 0;
}
void NBPTStopCont::reviseStops(NBEdgeCont& cont) {
    for (auto i = myPTStops.begin(); i != myPTStops.end();) {
        if (cont.getByID((*i).second->getEdgeId()) == nullptr) {
            WRITE_WARNING("Removing pt stop:" + (*i).first + " on non existing edge: " + (*i).second->getEdgeId());
            myPTStops.erase(i++);
        } else {
            i++;
        }
    }

}
