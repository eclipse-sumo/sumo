/****************************************************************************/
/// @file    MSPModel_Remote.cpp
/// @author  Gregor Laemmel
/// @date    Mon, 13 Jan 2014
/// @version $Id: MSPModel_Remote.cpp 23150 2017-02-27 12:08:30Z behrisch $
///
// The pedestrian following model for remote controlled pedestrian movement
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2014-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include "MSPModel_Remote.h"
#include <grpc++/grpc++.h>
#include "microsim/MSEdge.h"
#include "microsim/MSLane.h"
#include "microsim/MSEdgeControl.h"
#include "hybridsim.grpc.pb.h"
#include "utils/geom/Position.h"
#include "utils/geom/PositionVector.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

MSPModel_Remote::MSPModel_Remote(const OptionsCont& oc, MSNet* net)
        :myNet(net) {
    const std::string address = oc.getString("pedestrian.remote.address");
    myHybridsimStub = hybridsim::HybridSimulation::NewStub(grpc::CreateChannel(
            address, grpc::InsecureChannelCredentials()));

    initialize();

}
PedestrianState* MSPModel_Remote::add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now) {
    return nullptr;
}
void MSPModel_Remote::remove(PedestrianState* state) {

}
bool MSPModel_Remote::blockedAtDist(const MSLane* lane, double distToCrossing,
                                    std::vector<const MSPerson*>* collectBlockers) {
    return false;
}
void MSPModel_Remote::initialize() {
    hybridsim::Scenario req;




    //1. for all edges
    for (MSEdge* e : myNet->getEdgeControl().getEdges()) {
        if (e->isInternal()) {
            continue;
        }
        if (e->isWalkingArea()) {
            handleWalkingArea(e, req);
            continue;
        }
        for (MSLane* l : e->getLanes()) {
            if (l->getPermissions() == SVC_PEDESTRIAN){
                handlePedestrianLane(l,req);
            }
        }
    }

    //add boundary
    hybridsim::Edge* edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmin());
    edge->mutable_c0()->set_y(myBoundary.ymin());
    edge->mutable_c1()->set_x(myBoundary.xmax());
    edge->mutable_c1()->set_y(myBoundary.ymin());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);

    edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmax());
    edge->mutable_c0()->set_y(myBoundary.ymin());
    edge->mutable_c1()->set_x(myBoundary.xmax());
    edge->mutable_c1()->set_y(myBoundary.ymax());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);

    edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmax());
    edge->mutable_c0()->set_y(myBoundary.ymax());
    edge->mutable_c1()->set_x(myBoundary.xmin());
    edge->mutable_c1()->set_y(myBoundary.ymax());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);

    edge = req.add_edges();
    edge->mutable_c0()->set_x(myBoundary.xmin());
    edge->mutable_c0()->set_y(myBoundary.ymax());
    edge->mutable_c1()->set_x(myBoundary.xmin());
    edge->mutable_c1()->set_y(myBoundary.ymin());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);


    hybridsim::Empty rpl;
    ClientContext context;
    Status st = myHybridsimStub->initScenario(&context, req, &rpl);
    if (!st.ok()) {
        std::cerr << "Remote side could not initialize scenario!" << std::endl;
        exit(-1);
    }

}
void MSPModel_Remote::handleWalkingArea(MSEdge* msEdge, hybridsim::Scenario& scenario) {
    MSLane* l = *(msEdge->getLanes().begin());

    const PositionVector shape = l->getShape();
    if (shape.size() < 2) {//should never happen
        return;
    }

    handleShape(shape,scenario);


    //close walking area
    Position frst = *shape.begin();
    Position scnd = *(shape.end()-1);
    hybridsim::Edge* edge = scenario.add_edges();
    edge->mutable_c0()->set_x(frst.x());
    edge->mutable_c0()->set_y(frst.y());
    edge->mutable_c1()->set_x(scnd.x());
    edge->mutable_c1()->set_y(scnd.y());
    edge->set_type(hybridsim::Edge_Type_OBSTACLE);



}
void MSPModel_Remote::handlePedestrianLane(MSLane* l, hybridsim::Scenario& scenario) {
    double width = l->getWidth();
    PositionVector centerLine = PositionVector(l->getShape());
    if (centerLine.size() < 2) {//should never happen
        return;
    }

    //start and end
    Position frst = *centerLine.begin();
    Position scnd = *(centerLine.begin()+1);
    makeStartOrEndTransition(frst,scnd,width,scenario);
    Position thrd = *(centerLine.end()-1);
    Position frth = *(centerLine.end()-2);
    makeStartOrEndTransition(thrd,frth,width,scenario);

    centerLine.move2side(-width/2.);
    handleShape(centerLine,scenario);
    centerLine.move2side(width);
    handleShape(centerLine,scenario);

}
void MSPModel_Remote::makeStartOrEndTransition(Position& frst, Position& scnd, double width,
                                               hybridsim::Scenario& scenario) {

    double dx = scnd.x() - frst.x();
    double dy = scnd.y() - frst.y();
    double length = sqrt(dx*dx+dy*dy);
    dx /= length;
    dy /= length;
    double x0 = frst.x() - dy*width/2.;
    double y0 = frst.y() + dx*width/2.;
    double x1 = frst.x() + dy*width/2.;
    double y1 = frst.y() - dx*width/2.;
    hybridsim::Edge* edge = scenario.add_edges();
    edge->mutable_c0()->set_x(x0);
    edge->mutable_c0()->set_y(y0);
    edge->mutable_c1()->set_x(x1);
    edge->mutable_c1()->set_y(y1);
    edge->set_type(hybridsim::Edge_Type_TRANSITION);

}
void MSPModel_Remote::handleShape(const PositionVector& shape, hybridsim::Scenario& scenario) {
    PositionVector::const_iterator it = shape.begin();
    Position frst = *it;

    myBoundary.add(frst.x(), frst.y());
    it++;
    for (; it != shape.end(); it++) {
        hybridsim::Edge* edge = scenario.add_edges();
        edge->mutable_c0()->set_x(frst.x());
        edge->mutable_c0()->set_y(frst.y());
        edge->mutable_c1()->set_x((*it).x());
        edge->mutable_c1()->set_y((*it).y());
        edge->set_type(hybridsim::Edge_Type_OBSTACLE);


//        std::cout << "edge: " << edge->c0().x() << "," << edge->c0().y() << "  --> " << edge->c1().x() << ","
//                  << edge->c1().y() << std::endl;
        frst = *it;
        myBoundary.add(frst.x(), frst.y());
    }
}
