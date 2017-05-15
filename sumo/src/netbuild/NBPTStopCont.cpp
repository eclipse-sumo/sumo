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

bool NBPTStopCont::insert(NBPTStop* ptStop) {
    std::string id = ptStop->getID();
    PTStopsCont::iterator i = myPTStops.find(id);
    if (i != myPTStops.end()) {
        return false;
    }
    myPTStops[id] = ptStop;
    return true;
}
void NBPTStopCont::process(NBEdgeCont& cont) {

    for (PTStopsCont::iterator i = myPTStops.begin(); i != myPTStops.end();) {
        std::string edgeId = i->second->getEdgeId();
        NBEdge* edge = cont.getByID(edgeId);

        if (edge != 0) {

            int laneNr = -1;

            for (std::vector<NBEdge::Lane>::const_iterator it = edge->getLanes().begin(); it != edge->getLanes().end(); it++) {
                if ((it->permissions & i->second->getPermissions()) > 0){
                    ++laneNr;
                    break;
                }
                laneNr++;
            }

            if (laneNr != -1) {
                const std::string& lane = edge->getLaneID(laneNr);
                i->second->setLaneID(lane);
                const PositionVector& shape = edge->getLaneShape(laneNr);
                double offset = shape.nearest_offset_to_point2D(i->second->getPosition(), true);
                i->second->computExtent(offset, edge->getLength());
                i++;
            } else{
                WRITE_WARNING("There is no compatible lane for pt stop: " + i->second->getName() + " on edge: " + edge->getID() +". Thus, pt stop will be removed!");
                EdgeVector edgeVector = cont.getGeneratedFrom((*i).second->getOrigEdgeId());
                myPTStops.erase(i++);
            }
        } else {
            WRITE_WARNING("Could not find corresponding edge for pt stop: " + i->second->getName() + ". Thus, it will be removed!");
            EdgeVector edgeVector = cont.getGeneratedFrom((*i).second->getOrigEdgeId());
            //std::cout << edgeVector.size() << std::endl;
            myPTStops.erase(i++);
        }

    }

}
