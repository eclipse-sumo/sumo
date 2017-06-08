/****************************************************************************/
/// @file    MSPModel_Remote.h
/// @author  Gregor Laemmel
/// @date    Mon, 13 Jan 2014
/// @version $Id: MSPModel_Remote.h 23150 2017-02-27 12:08:30Z behrisch $
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

#ifndef SUMO_MSPMODEL_REMOTE_H
#define SUMO_MSPMODEL_REMOTE_H


#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/pedestrians/hybridsim.grpc.pb.h>
#include <utils/geom/Boundary.h>
#include "MSPModel.h"
class MSPModel_Remote : public MSPModel{


public:
    MSPModel_Remote(const OptionsCont& oc, MSNet* net);
    PedestrianState* add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now) override;
    void remove(PedestrianState* state) override;
    bool
    blockedAtDist(const MSLane* lane, double distToCrossing, std::vector<const MSPerson*>* collectBlockers) override;

private:
    MSNet* myNet;
    std::unique_ptr<hybridsim::HybridSimulation::Stub> myHybridsimStub;
    Boundary myBoundary;
    void initialize();
    void handleWalkingArea(MSEdge* msEdge, hybridsim::Scenario& scenario);
    void handlePedestrianLane(MSLane* pLane, hybridsim::Scenario& scenario);
    void makeStartOrEndTransition(Position& position, Position& scnd, double width, hybridsim::Scenario& scenario);
    void handleShape(const PositionVector& shape, hybridsim::Scenario& scenario);
};



#endif //SUMO_MSPMODEL_REMOTE_H
