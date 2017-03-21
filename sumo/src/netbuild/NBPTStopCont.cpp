/****************************************************************************/
/// @file    NBPTStopCont.cpp
/// @author  Gregor Laemmel
/// @date    Tue, 20 Mar 2017
/// @version $Id: NBPTStopCont.cpp 23150 2017-02-27 12:08:30Z behrisch $
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

    for (PTStopsCont::const_iterator i = myPTStops.begin(); i != myPTStops.end(); ++i) {
        std::string edgeId = i->second->getEdgeId();
        NBEdge* edge = cont.getByID(edgeId);
        if (edge != 0) {
            const std::string& lane = edge->getLaneID(0);
            i->second->setLaneID(lane);
            const PositionVector& shape = edge->getLaneShape(0);
            double offset2 = shape.nearest_offset_to_point2D(i->second->getPosition(), true);
            int idx = shape.indexOfClosest(i->second->getPosition());
            double offset = shape.offsetAtIndex2D(idx);
            i->second->computExtent(offset, edge->getLength());
        } else {
            //todo
            //        EdgeVector edgeVector = cont.getGeneratedFrom((*i).second->getEdgeId());
            //        std::cout << edgeVector.size() << std::endl;
        }

    }

}
