/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSStageMoving.h
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    Wed, 1 Jun 2022
///
// The common superclass for modelling walking and tranship
/****************************************************************************/
#pragma once
#include <config.h>

#include <microsim/transportables/MSStage.h>


// ===========================================================================
// class declarations
// ===========================================================================

/**
* An abstract stage providing additional interface for the movement models
*/
class MSStageMoving : public MSStage {
public:
    /// constructor
    MSStageMoving(const std::vector<const MSEdge*>& route, const std::string& routeID, MSStoppingPlace* toStop, const double speed,
                  const double departPos, const double arrivalPos, const double departPosLat, const int departLane, MSStageType type) :
        MSStage(route.back(), toStop, arrivalPos, type),
        myState(nullptr), myRoute(route), myRouteID(routeID), myRouteStep(myRoute.begin()),
        mySpeed(speed), myDepartPos(departPos),
        myDepartPosLat(departPosLat), myDepartLane(departLane) {}

    /// destructor
    virtual ~MSStageMoving();

    virtual const MSEdge* getNextRouteEdge() const = 0;

    virtual MSTransportableStateAdapter* getState() const {
        return myState;
    }

    /// Returns the current edge
    const MSEdge* getEdge() const;

    /// Returns the current lane
    const MSLane* getLane() const;

    /// Returns first edge of the containers route
    const MSEdge* getFromEdge() const;

    /// @brief the edges of the current stage
    ConstMSEdgeVector getEdges() const;

    /// Returns the offset from the start of the current edge measured in its natural direction
    double getEdgePos(SUMOTime now) const;

    /// @brief Return the movement directon on the edge
    int getDirection() const;

    /// Returns the position of the container
    Position getPosition(SUMOTime now) const;

    /// Returns the angle of the container
    double getAngle(SUMOTime now) const;

    /// Returns the time the container spent waiting
    SUMOTime getWaitingTime(SUMOTime now) const;

    /// Returns the speed of the container
    double getSpeed() const;

    /// Returns the configured speed in this stage
    double getConfiguredSpeed() const {
        return mySpeed;
    }

    /// @brief the maximum speed of the transportable
    virtual double getMaxSpeed(const MSTransportable* const transportable = nullptr) const = 0;

    /// @brief move forward and return whether the transportable arrived
    virtual bool moveToNextEdge(MSTransportable* transportable, SUMOTime currentTime, int prevDir, MSEdge* nextInternal = 0) = 0;

    virtual void activateEntryReminders(MSTransportable* /*person*/) { }

    /// @brief place transportable on a previously passed edge
    virtual void setRouteIndex(MSTransportable* const transportable, int routeOffset);

    virtual void replaceRoute(MSTransportable* const transportable, const ConstMSEdgeVector& edges, int routeOffset);

    inline const std::vector<const MSEdge*>& getRoute() const {
        return myRoute;
    }

    inline const std::vector<const MSEdge*>::iterator getRouteStep() const {
        return myRouteStep;
    }

    inline double getDepartPos() const {
        return myDepartPos;
    }

    inline double getDepartPosLat() const {
        return myDepartPosLat;
    }

    inline int getDepartLane() const {
        return myDepartLane;
    }

    /// @brief interpret custom depart lane
    static const MSLane* checkDepartLane(const MSEdge* edge, SUMOVehicleClass svc, int laneIndex, const std::string& id);

protected:
    /// @brief state that is to be manipulated by MSPModel
    MSTransportableStateAdapter* myState;

    /// @brief The route of the container
    std::vector<const MSEdge*> myRoute;

    /// @brief The original route id
    std::string myRouteID;

    /// @brief current step
    std::vector<const MSEdge*>::iterator myRouteStep;

    /// @brief The current internal edge this transportable is on or nullptr
    MSEdge* myCurrentInternalEdge = nullptr;

    /// @brief the speed of the transportable
    double mySpeed;

    /// @brief the depart position
    double myDepartPos;

    /// @brief the lateral depart position
    double myDepartPosLat;

    /// @brief the depart lane or -1
    int myDepartLane;
};
