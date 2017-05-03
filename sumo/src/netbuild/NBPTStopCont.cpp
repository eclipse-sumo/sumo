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
            const std::string& lane = edge->getLaneID(0);
            i->second->setLaneID(lane);
            const PositionVector& shape = edge->getLaneShape(0);
            double offset = shape.nearest_offset_to_point2D(i->second->getPosition(), true);
            i->second->computExtent(offset, edge->getLength());
            i++;
        } else {
            WRITE_WARNING("Could not find corresponding edge for pt stop: " + i->second->getName() + ". Thus, it will be removed!");
            EdgeVector edgeVector = cont.getGeneratedFrom((*i).second->getOrigEdgeId());
            //std::cout << edgeVector.size() << std::endl;
            myPTStops.erase(i++);
        }

    }

}
