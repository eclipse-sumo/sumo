/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MSPModel_Remote.h
/// @author  Gregor Laemmel
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model for remote controlled pedestrian movement
/****************************************************************************/

#pragma once


#include <utils/options/OptionsCont.h>
#include <microsim/MSNet.h>
#include <microsim/transportables/hybridsim.grpc.pb.h>
#include <utils/geom/Boundary.h>
#include "MSPModel.h"
class MSPModel_Remote : public MSPModel {


public:
    MSPModel_Remote(const OptionsCont& oc, MSNet* net);

    ~MSPModel_Remote();
    PedestrianState* add(MSPerson* person, MSPerson::MSPersonStage_Walking* stage, SUMOTime now) override;
    void remove(PedestrianState* state) override;
    bool usingInternalLanes();

    SUMOTime execute(SUMOTime time);
    class Event : public Command {
    public:
        explicit Event(MSPModel_Remote* remoteModel)
            : myRemoteModel(remoteModel) { }
        SUMOTime execute(SUMOTime currentTime) override {
            return myRemoteModel->execute(currentTime);
        }
    private:
        MSPModel_Remote* myRemoteModel;
    };

private:
    /**
    * @class PState
    * @brief Container for pedestrian state and individual position update function
    */
    class PState : public PedestrianState {
    public:
        PState(MSPerson* person, MSPerson::MSPersonStage_Walking* stage);
        ~PState() override;
        double getEdgePos(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const override;
        Position getPosition(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const override;
        double getAngle(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const override;
        SUMOTime getWaitingTime(const MSPerson::MSPersonStage_Walking& stage, SUMOTime now) const override;
        double getSpeed(const MSPerson::MSPersonStage_Walking& stage) const override;
        const MSEdge* getNextEdge(const MSPerson::MSPersonStage_Walking& stage) const override;
        MSPerson::MSPersonStage_Walking* getStage();
        MSPerson* getPerson();

        void setPosition(double x, double y);
        void setPhi(double phi);
    private:
        Position myPosition;
        double myPhi;
        MSPerson::MSPersonStage_Walking* myStage;
        MSPerson* myPerson;
    };


    MSNet* myNet;
    std::unique_ptr<hybridsim::HybridSimulation::Stub> myHybridsimStub;
    Boundary myBoundary;
    void initialize();
    void handleWalkingArea(MSEdge* msEdge, hybridsim::Scenario& scenario);
    void handlePedestrianLane(MSLane* pLane, hybridsim::Scenario& scenario);
    void makeStartOrEndTransition(Position position, Position scnd, double width, hybridsim::Scenario& scenario,
                                  hybridsim::Edge_Type type, int i);
    void handleShape(const PositionVector& shape, hybridsim::Scenario& scenario);

    std::map<int, PState*> remoteIdPStateMapping;
    std::map<const MSEdge*, std::tuple<int, int>> edgesTransitionsMapping;
    std::map<int, const MSEdge*> transitionsEdgesMapping;
    int myLastId = 0;
    int myLastTransitionId = 0;


    MSLane* getFirstPedestrianLane(const MSEdge* const& edge);
};


