/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2022 German Aerospace Center (DLR) and others.
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
// The pedestrian following model connected to the external JuPedSim simulation
/****************************************************************************/
#pragma once
#include <config.h>

#include <jupedsim/jupedsim.h>
#include "microsim/MSNet.h"
#include "MSPModel.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel_Remote
 * @brief The pedestrian following model connected to the external JuPedSim simulation
 */
class MSPModel_Remote : public MSPModel {
public:
    MSPModel_Remote(const OptionsCont& oc, MSNet* net);
    ~MSPModel_Remote();

    MSTransportableStateAdapter* add(MSTransportable* person, MSStageMoving* stage, SUMOTime now) override;
    void remove(MSTransportableStateAdapter* state) override;
    SUMOTime execute(SUMOTime time);

    bool usingInternalLanes();
    void registerArrived();
    int getActiveNumber();
    void clearState();

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
    * @brief Holds pedestrian state and performs updates
    */
    class PState : public MSTransportableStateAdapter {
    public:
        PState(MSPerson* person, MSStageMoving* stage, JPS_Journey journey, Position destination, JPS_AgentId agentId);
        ~PState() override;

        Position getPosition(const MSStageMoving& stage, SUMOTime now) const;
        void setPosition(double x, double y);

        double getAngle(const MSStageMoving& stage, SUMOTime now) const;
        void setAngle(double angle);

        MSStageMoving* getStage();
        MSPerson* getPerson();

        double getEdgePos(const MSStageMoving& stage, SUMOTime now) const;
        int getDirection(const MSStageMoving& stage, SUMOTime now) const;
        SUMOTime getWaitingTime(const MSStageMoving& stage, SUMOTime now) const;
        double getSpeed(const MSStageMoving& stage) const;
        const MSEdge* getNextEdge(const MSStageMoving& stage) const;

        Position getDestination(void) const;
        JPS_AgentId getAgentId(void) const;

    private:
        Position myPosition;
        Position myDestination;
        double myAngle;
        MSStageMoving* myStage;
        MSPerson* myPerson;
        JPS_Journey myJourney;
        JPS_AgentId myAgentId;
    };

    MSNet* myNet;
    int myNumActivePedestrians = 0;
    std::vector<PState*> myPedestrianStates;
    JPS_GeometryBuilder myGeometryBuilder;
    JPS_Geometry myGeometry;
    JPS_AreasBuilder myAreasBuilder;
    JPS_Areas myAreas;
    JPS_OperationalModel myModel;
    JPS_ModelParameterProfileId myParameterProfileId;
    JPS_Simulation mySimulation;

#ifdef DEBUG
    std::ofstream myTrajectoryDumpFile;
#endif

    static const SUMOTime JPS_DELTA_T;
    static const double JPS_EXIT_TOLERANCE;

    void initialize();
    MSLane* getNextPedestrianLane(const MSLane* const currentLane) const;
};
