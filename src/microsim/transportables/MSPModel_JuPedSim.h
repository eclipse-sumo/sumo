/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2023 German Aerospace Center (DLR) and others.
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

#include <memory>
#include <unordered_set>
#define USE_UNSTABLE_GEOS_CPP_API 1
#include <geos/geom/GeometryFactory.h>
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

    bool usingInternalLanes();
    void registerArrived();
    int getActiveNumber();
    void clearState();

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
        PState(MSPerson* person, MSStageMoving* stage, JPS_JourneyDescription journey, JPS_JourneyId journeyId, Position destination);
        ~PState() override;

        Position getPosition(const MSStageMoving& stage, SUMOTime now) const;
        void setPosition(double x, double y);

        Position getPreviousPosition() const;
        void setPreviousPosition(Position previousPosition);

        double getAngle(const MSStageMoving& stage, SUMOTime now) const;
        void setAngle(double angle);

        MSStageMoving* getStage();
        MSPerson* getPerson();

        void setLanePosition(double lanePosition);
        double getEdgePos(const MSStageMoving& stage, SUMOTime now) const;
        int getDirection(const MSStageMoving& stage, SUMOTime now) const;
        SUMOTime getWaitingTime(const MSStageMoving& stage, SUMOTime now) const;
        double getSpeed(const MSStageMoving& stage) const;
        const MSEdge* getNextEdge(const MSStageMoving& stage) const;
        Position getDestination() const;
        JPS_AgentId getAgentId() const;

        void setAgentId(JPS_AgentId id) {
            myAgentId = id;
            myWaitingToEnter = false;
        }

        JPS_JourneyId getJourneyId() const {
            return myJourneyId;
        }

        bool isWaitingToEnter() const {
            return myWaitingToEnter;
        }

    private:
        MSPerson* myPerson;
        MSStageMoving* myStage;
        /// @brief handle to the JPS journey, only needed for freeing the memory later
        JPS_JourneyDescription myJourney;
        /// @brief id of the journey, needed for modifying it
        JPS_JourneyId myJourneyId;
        Position myDestination;
        JPS_AgentId myAgentId;
        Position myPosition;
        Position myPreviousPosition; // Will be initialized to zero automatically.
        double myAngle;
        double myLanePosition;
        MSEdge* myCurrentEdge;
        /// @brief whether the pedestrian is waiting to start its walk
        bool myWaitingToEnter;
    };

    MSNet* const myNetwork;
    const SUMOTime myJPSDeltaT;
    const double myExitTolerance;
    int myNumActivePedestrians = 0;
    std::vector<PState*> myPedestrianStates;

    geos::geom::GeometryFactory::Ptr myGEOSGeometryFactory;
    std::vector<geos::geom::Geometry*> myGEOSLineStringsDump;
    std::vector<geos::geom::Geometry*> myGEOSPointsDump;
    std::vector<geos::geom::Geometry*> myGEOSBufferedGeometriesDump;
    std::vector<geos::geom::Geometry*> myGEOSGeometryCollectionsDump;
    std::vector<geos::geom::Geometry*> myGEOSConvexHullsDump;
    std::vector<geos::geom::Geometry*> myGEOSConvertedLinearRingsDump;
    geos::geom::Geometry* myGEOSPedestrianNetwork;
    bool myIsPedestrianNetworkConnected;

    JPS_GeometryBuilder myJPSGeometryBuilder;
    JPS_Geometry myJPSGeometry;
    JPS_VelocityModelBuilder myJPSModelBuilder;
    JPS_OperationalModel myJPSModel;
    std::map<std::string, JPS_ModelParameterProfileId> myJPSParameterProfileIds;
    JPS_Simulation myJPSSimulation;

    static const int GEOS_QUADRANT_SEGMENTS;
    static const double GEOS_MIN_AREA;

    void initialize();
    void tryInsertion(PState* state);

    static MSLane* getNextPedestrianLane(const MSLane* const currentLane);
    
    static const Position& getAnchor(const MSLane* const lane, const MSJunction* const junction);
    static const Position& getAnchor(const MSLane* const lane, const MSEdge* const edge, MSEdgeVector incoming);
    static const MSEdgeVector getAdjacentEdgesOfEdge(const MSEdge* const edge);
    static bool hasWalkingAreasInbetween(const MSEdge* const edge, const MSEdge* const otherEdge);
    geos::geom::Geometry* createShapeFromCenterLine(PositionVector centerLine, double width, int capStyle);
    geos::geom::Geometry* createShapeFromAnchors(const Position& anchor, const MSLane* const lane, const Position& otherAnchor, const MSLane* const otherLane);
    geos::geom::Geometry* buildPedestrianNetwork(MSNet* network);
    static PositionVector getCoordinates(const geos::geom::Geometry* geometry);
    static std::vector<JPS_Point> convertToJPSPoints(const geos::geom::Geometry* geometry);
    static std::vector<JPS_Point> convertToJPSPoints(const PositionVector& coordinates);
    geos::geom::Polygon* toPolygon(const geos::geom::LinearRing* linearRing);
    void renderPolygon(const geos::geom::Polygon* polygon, const std::string& polygonId);
    void preparePolygonForJPS(const geos::geom::Polygon* polygon, const std::string& polygonId = std::string());
    void prepareAdditionalPolygonsForJPS(void);
};
