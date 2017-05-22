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

bool NBPTStopCont::insert(NBPTStop* ptStop) {
    std::string id = ptStop->getID();
    PTStopsCont::iterator i = myPTStops.find(id);
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
    return 0;
}


void NBPTStopCont::process(NBEdgeCont& cont) {

    PTStopsCont::iterator end = myPTStops.end();

    std::vector<NBPTStop*> reverseStops;

    //frst pass localize pt stop at correct side of the street; create stop for opposite side if needed
    for (PTStopsCont::iterator i = myPTStops.begin(); i != end; i++) {

        NBPTStop* stop = i->second;

        bool multipleStopPositions = stop->getIsMultipleStopPositions();
        bool platformsDefined = stop->getPlatformPosCands().size() > 0;
        if (!multipleStopPositions && !platformsDefined) {
            //create pt stop for reverse edge if edge exist
            NBPTStop* reverseStop = getReverseStop(stop, cont);
            if (reverseStop != 0) {
                reverseStops.push_back(reverseStop);
            }
        } else if (multipleStopPositions && platformsDefined) {
            //create pt stop for closest platform at corresponding edge
            assignPTStopToEdgeOfClosestPlatform(stop, cont);

        } else if (platformsDefined) {
            //create pt stop for each side of the street where a platform is defined
            throw new ProcessError("in NBPTStopCont:process() not yet implemented!");
        }
    }

    //insrt new stops if any
    for (std::vector<NBPTStop*>::iterator i = reverseStops.begin(); i != reverseStops.end(); i++) {
        insert(*i);
    }


    //scnd pass set correct lane
    for (PTStopsCont::iterator i = myPTStops.begin(); i != myPTStops.end();) {
        NBPTStop* stop = i->second;
        if (!findLaneAndComputeBusStopExtend(stop, cont)) {
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

bool NBPTStopCont::findLaneAndComputeBusStopExtend(NBPTStop* pStop, NBEdgeCont& cont) {
    std::string edgeId = pStop->getEdgeId();
    NBEdge* edge = cont.getByID(edgeId);
    if (edge != 0) {

        int laneNr = -1;

        for (std::vector<NBEdge::Lane>::const_iterator it = edge->getLanes().begin();
             it != edge->getLanes().end();
             it++) {
            if ((it->permissions & pStop->getPermissions()) > 0) {
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
            pStop->computExtent(offset, edge->getLength());

        } else {
            return false;
        }
    } else {
        return false;
    }
    return true;

}
NBPTStop* NBPTStopCont::getReverseStop(NBPTStop* pStop, NBEdgeCont& cont) {
    std::string edgeId = pStop->getEdgeId();
    NBEdge* edge = cont.getByID(edgeId);
    NBEdge* reverse = getReverseEdge(edge);
    if (reverse != 0) {

        Position* pos = new Position(pStop->getPosition());
        NBPTStop* ret = new NBPTStop("-1" + pStop->getID(), *pos, reverse->getID(), reverse->getID(),
                                     pStop->getLength(), pStop->getName(), pStop->getPermissions());
        return ret;

    }
    return 0;
}

NBEdge* NBPTStopCont::getReverseEdge(NBEdge* edge) {
    if (edge != 0) {
        for (EdgeVector::const_iterator it = edge->getToNode()->getOutgoingEdges().begin();
             it != edge->getToNode()->getOutgoingEdges().end();
             it++) {
            if ((*it)->getToNode() == edge->getFromNode()) {
                return (*it);
            }
        }
    }
    return 0;
}

void NBPTStopCont::assignPTStopToEdgeOfClosestPlatform(NBPTStop* pStop, NBEdgeCont& cont) {
    std::string edgeId = pStop->getEdgeId();
    NBEdge* edge = cont.getByID(edgeId);
    NBEdge* reverse = getReverseEdge(edge);
    if (reverse != 0) {
        Position* closestPlatform = getClosestPlatformToPTStopPosition(pStop);
        //TODO make isLeft in PositionVector static [GL May '17]
//        if (PositionVector::isLeft(edge->getFromNode()->getPosition(),edge->getToNode()->getPosition(),closestPlatform)){
//
//        }
        double x0 = edge->getFromNode()->getPosition().x();
        double y0 = edge->getFromNode()->getPosition().y();
        double x1 = edge->getToNode()->getPosition().x();
        double y1 = edge->getToNode()->getPosition().y();
        double x2 = closestPlatform->x();
        double y2 = closestPlatform->y();
        double crossProd = (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0);

        //TODO consider driving on the left!!! [GL May '17]
        if (crossProd > 0) { //pt stop is on the left of the orig edge
            pStop->setEdgeId(reverse->getID());
        }
    }


}
Position* NBPTStopCont::getClosestPlatformToPTStopPosition(NBPTStop* pStop) {

    Position stopPosition = pStop->getPosition();

    Position* closest;
    double minSqrDist = INFINITY;

    for (std::vector<Position>::iterator it = pStop->getPlatformPosCands().begin();
         it != pStop->getPlatformPosCands().end();
         it++) {
        double sqrDist = stopPosition.distanceSquaredTo(*it);
        if (sqrDist < minSqrDist) {
            minSqrDist = sqrDist;
            closest = &(*it);
        }
    }

    return closest;
}
