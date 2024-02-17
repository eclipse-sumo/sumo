/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2014-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSPModel_JuPedSim.h
/// @author  Gregor Laemmel
/// @author  Benjamin Coueraud
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Mon, 13 Jan 2014
///
// The pedestrian following model that can instantiate different pedestrian models
// that come with the JuPedSim third-party simulation framework.
/****************************************************************************/
#pragma once
#include <config.h>
#include <vector>
#include <map>
#include <geos_c.h>
#include <jupedsim/jupedsim.h>
#include "microsim/MSNet.h"
#include "MSPModel.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSPModel_JuPedSim
 * @brief A pedestrian following model that acts as a proxy for pedestrian models
 * provided by the JuPedSim third-party simulation framework.
 */
class MSPModel_JuPedSim : public MSPModel {
public:
    MSPModel_JuPedSim(const OptionsCont& oc, MSNet* net);
    ~MSPModel_JuPedSim();

    MSTransportableStateAdapter* add(MSTransportable* person, MSStageMoving* stage, SUMOTime now) override;
    void remove(MSTransportableStateAdapter* state) override;
    SUMOTime execute(SUMOTime time);

    bool usingInternalLanes() override;
    bool usingShortcuts() override {
        return myHaveAdditionalWalkableAreas;
    }
    void registerArrived();
    int getActiveNumber() override;
    void clearState() override;

    class Event : public Command {
    public:
        explicit Event(MSPModel_JuPedSim* model)
            : myJPSModel(model) { }
        SUMOTime execute(SUMOTime currentTime) override {
            return myJPSModel->execute(currentTime);
        }

    private:
        MSPModel_JuPedSim* myJPSModel;
    };

private:
    /**
    * @class PState
    * @brief Holds pedestrian state and performs updates
    */
    class PState : public MSTransportableStateAdapter {
    public:
        PState(MSPerson* person, MSStageMoving* stage, JPS_JourneyId journeyId, JPS_StageId stageId, const PositionVector& waypoints);
        ~PState() override;

        void reinit(MSStageMoving* stage, JPS_JourneyId journeyId, JPS_StageId stageId, const PositionVector& waypoints);

        Position getPosition(const MSStageMoving& stage, SUMOTime now) const override;
        void setPosition(double x, double y);

        Position getPreviousPosition() const;
        void setPreviousPosition(Position previousPosition);

        double getAngle(const MSStageMoving& stage, SUMOTime now) const override;
        void setAngle(double angle);

        MSStageMoving* getStage() const;
        void setStage(MSStageMoving* const stage);

        MSPerson* getPerson() const;

        void setLanePosition(double lanePosition);
        double getEdgePos(const MSStageMoving& stage, SUMOTime now) const override;
        int getDirection(const MSStageMoving& stage, SUMOTime now) const override;
        SUMOTime getWaitingTime(const MSStageMoving& stage, SUMOTime now) const override;
        double getSpeed(const MSStageMoving& stage) const override;
        const MSEdge* getNextEdge(const MSStageMoving& stage) const override;
        const Position& getNextWaypoint() const;
        JPS_AgentId getAgentId() const;

        /// @brief whether the transportable has finished walking
        bool isFinished() const override {
            return myWaypoints.empty();
        }

        void setAgentId(JPS_AgentId id) {
            myAgentId = id;
            myWaitingToEnter = false;
        }

        JPS_JourneyId getJourneyId() const {
            return myJourneyId;
        }

        /// @brief first stage of the journey
        JPS_StageId getStageId() const {
            return myStageId;
        }

        bool isWaitingToEnter() const {
            return myWaitingToEnter;
        }

        bool advanceNextWaypoint() {
            myWaypoints.erase(myWaypoints.begin());
            return myWaypoints.empty();
        }

    private:
        MSPerson* myPerson;
        MSStageMoving* myStage;
        /// @brief id of the journey, needed for modifying it
        JPS_JourneyId myJourneyId;
        JPS_StageId myStageId;
        PositionVector myWaypoints;
        JPS_AgentId myAgentId;
        Position myPosition;
        Position myPreviousPosition; // Will be initialized to zero automatically.
        double myAngle;
        double myLanePosition;
        /// @brief whether the pedestrian is waiting to start its walk
        bool myWaitingToEnter;
        int myNumStages;
    };

    MSNet* const myNetwork;
    const SUMOTime myJPSDeltaT;
    const double myExitTolerance;
    int myNumActivePedestrians = 0;
    std::vector<PState*> myPedestrianStates;

    GEOSGeometry* myGEOSPedestrianNetwork;
    bool myHaveAdditionalWalkableAreas;

    JPS_Geometry myJPSGeometry;
    JPS_Geometry myJPSGeometryWithTrainsAndRamps;
    JPS_OperationalModel myJPSModel;
    JPS_Simulation myJPSSimulation;
    struct AreaData {
        const std::string id;
        const std::string areaType;
        const std::vector<JPS_Point> areaBoundary;
        const Parameterised::Map params;
    };
    std::vector<AreaData> myAreas;
    SUMOTime myLastRemovalTime = 0;
    std::vector<SUMOTrafficObject::NumericalID> myAllStoppedTrainIDs;

    static const int GEOS_QUADRANT_SEGMENTS;
    static const double GEOS_MITRE_LIMIT;
    static const double GEOS_MIN_AREA;
    static const double GEOS_BUFFERED_SEGMENT_WIDTH;
    static const double CARRIAGE_RAMP_WIDTH;

    void initialize(const OptionsCont& oc);
    void tryPedestrianInsertion(PState* state, const Position& p);
    bool addWaypoint(JPS_JourneyDescription journey, JPS_StageId& predecessor, const Position& point, const std::string& agentID);
    static MSLane* getNextPedestrianLane(const MSLane* const currentLane);
    static const Position& getAnchor(const MSLane* const lane, const MSEdge* const edge, MSEdgeVector incoming);
    static const MSEdgeVector getAdjacentEdgesOfEdge(const MSEdge* const edge);
    static const MSEdge* getWalkingAreaInbetween(const MSEdge* const edge, const MSEdge* const otherEdge);
    static GEOSGeometry* createGeometryFromCenterLine(PositionVector centerLine, double width, int capStyle);
    static GEOSGeometry* createGeometryFromShape(PositionVector shape, std::string shapeID, bool isInternalShape = false);
    static GEOSGeometry* createGeometryFromAnchors(const Position& anchor, const MSLane* const lane, const Position& otherAnchor, const MSLane* const otherLane);
    GEOSGeometry* buildPedestrianNetwork(MSNet* network);
    static PositionVector getCoordinates(const GEOSGeometry* geometry);
    static std::vector<JPS_Point> convertToJPSPoints(const GEOSGeometry* geometry);
    static double getHoleArea(const GEOSGeometry* hole);
    void preparePolygonForJPS(const GEOSGeometry* polygon, JPS_GeometryBuilder geometryBuilder);
    void preparePolygonForDrawing(const GEOSGeometry* polygon, const std::string& polygonId);
    JPS_Geometry buildJPSGeometryFromGEOSGeometry(const GEOSGeometry* polygon);
    static void dumpGeometry(const GEOSGeometry* polygon, const std::string& filename);
};
