/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2020 German Aerospace Center (DLR) and others.
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
/// @file    Vehicle.h
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    15.03.2017
///
// C++ Vehicle API
/****************************************************************************/
#pragma once
#include <config.h>

#include <limits>
#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/VehicleType.h>
#include <libsumo/TraCIConstants.h>
#ifndef LIBTRACI
#include <utils/vehicle/SUMOVehicleParameter.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
#ifndef LIBTRACI
namespace libsumo {
class VariableWrapper;
}
class SUMOVehicle;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Vehicle
 * @brief C++ TraCI client API implementation
 */
namespace LIBSUMO_NAMESPACE {
class Vehicle {
public:
    /// @name Value retrieval
    /// @{
    static double getSpeed(const std::string& vehicleID);
    static double getLateralSpeed(const std::string& vehicleID);
    static double getAcceleration(const std::string& vehicleID);
    static double getSpeedWithoutTraCI(const std::string& vehicleID);
    static libsumo::TraCIPosition getPosition(const std::string& vehicleID, const bool includeZ = false);
    static libsumo::TraCIPosition getPosition3D(const std::string& vehicleID);
    static double getAngle(const std::string& vehicleID);
    static double getSlope(const std::string& vehicleID);
    static std::string getRoadID(const std::string& vehicleID);
    static std::string getLaneID(const std::string& vehicleID);
    static int getLaneIndex(const std::string& vehicleID);
    static std::string getTypeID(const std::string& vehicleID);
    static std::string getRouteID(const std::string& vehicleID);
    static int getRouteIndex(const std::string& vehicleID);
    static double getLanePosition(const std::string& vehicleID);
    static double getLateralLanePosition(const std::string& vehicleID);
    static double getCO2Emission(const std::string& vehicleID);
    static double getCOEmission(const std::string& vehicleID);
    static double getHCEmission(const std::string& vehicleID);
    static double getPMxEmission(const std::string& vehicleID);
    static double getNOxEmission(const std::string& vehicleID);
    static double getFuelConsumption(const std::string& vehicleID);
    static double getNoiseEmission(const std::string& vehicleID);
    static double getElectricityConsumption(const std::string& vehicleID);
    static int getPersonNumber(const std::string& vehicleID);
    static std::vector<std::string> getPersonIDList(const std::string& vehicleID);
    static std::pair<std::string, double> getLeader(const std::string& vehicleID, double dist = 0.);
    static std::pair<std::string, double> getFollower(const std::string& vehicleID, double dist = 0.);
    static double getWaitingTime(const std::string& vehicleID);
    static double getAccumulatedWaitingTime(const std::string& vehicleID);
    static double getAdaptedTraveltime(const std::string& vehicleID, double time, const std::string& edgeID);
    static double getEffort(const std::string& vehicleID, double time, const std::string& edgeID);
    static bool isRouteValid(const std::string& vehicleID);
    static std::vector<std::string> getRoute(const std::string& vehicleID);
    static int getSignals(const std::string& vehicleID);
    static std::vector<libsumo::TraCIBestLanesData> getBestLanes(const std::string& vehicleID);
    static std::vector<libsumo::TraCINextTLSData> getNextTLS(const std::string& vehicleID);
    static std::vector<libsumo::TraCINextStopData> getNextStops(const std::string& vehicleID);
    static std::vector<libsumo::TraCINextStopData> getStops(const std::string& vehicleID, int limit = 0);
    static int getStopState(const std::string& vehicleID);
    static double getDistance(const std::string& vehicleID);
    static double getDrivingDistance(const std::string& vehicleID, const std::string& edgeID, double position, int laneIndex = 0);
    static double getDrivingDistance2D(const std::string& vehicleID, double x, double y);
    static double getAllowedSpeed(const std::string& vehicleID);
    static int getSpeedMode(const std::string& vehicleID);
    static int getLaneChangeMode(const std::string& vehicleID);
    static int getRoutingMode(const std::string& vehicleID);
    static std::string getLine(const std::string& vehicleID);
    static std::vector<std::string> getVia(const std::string& vehicleID);
    static std::pair<int, int> getLaneChangeState(const std::string& vehicleID, int direction);
    static double getLastActionTime(const std::string& vehicleID);
    static std::vector<std::pair<std::string, double> > getNeighbors(const std::string& vehicleID, const int mode);
    static double getFollowSpeed(const std::string& vehicleID, double speed, double gap, double leaderSpeed, double leaderMaxDecel, const std::string& leaderID = "");
    static double getSecureGap(const std::string& vehicleID, double speed, double leaderSpeed, double leaderMaxDecel, const std::string& leaderID = "");
    static double getStopSpeed(const std::string& vehicleID, double speed, double gap);
    static double getStopDelay(const std::string& vehicleID);
    static double getStopArrivalDelay(const std::string& vehicleID);
    static std::vector<std::string> getTaxiFleet(int taxiState = 0);
    /// @}

    LIBSUMO_ID_PARAMETER_API
    LIBSUMO_VEHICLE_TYPE_GETTER

    /// @name State Changing
    /// @{

    static void setStop(const std::string& vehicleID,
                        const std::string& edgeID,
                        double pos = 1.,
                        int laneIndex = 0,
                        double duration = libsumo::INVALID_DOUBLE_VALUE,
                        int flags = libsumo::STOP_DEFAULT,
                        double startPos = libsumo::INVALID_DOUBLE_VALUE,
                        double until = libsumo::INVALID_DOUBLE_VALUE);

    static void replaceStop(const std::string& vehicleID,
                            int nextStopIndex,
                            const std::string& edgeID,
                            double pos = 1.,
                            int laneIndex = 0,
                            double duration = libsumo::INVALID_DOUBLE_VALUE,
                            int flags = libsumo::STOP_DEFAULT,
                            double startPos = libsumo::INVALID_DOUBLE_VALUE,
                            double until = libsumo::INVALID_DOUBLE_VALUE);

    static void rerouteParkingArea(const std::string& vehicleID,
                                   const std::string& parkingAreaID);

    static void resume(const std::string& vehicleID);

    static void add(const std::string& vehID,
                    const std::string& routeID,
                    const std::string& typeID = "DEFAULT_VEHTYPE",
                    const std::string& depart = "now",
                    const std::string& departLane = "first",
                    const std::string& departPos = "base",
                    const std::string& departSpeed = "0",
                    const std::string& arrivalLane = "current",
                    const std::string& arrivalPos = "max",
                    const std::string& arrivalSpeed = "current",
                    const std::string& fromTaz = "",
                    const std::string& toTaz = "",
                    const std::string& line = "",
                    int personCapacity = 4,
                    int personNumber = 0);

    static void changeTarget(const std::string& vehicleID, const std::string& edgeID);
    static void changeLane(const std::string& vehicleID, int laneIndex, double duration);
    static void changeLaneRelative(const std::string& vehicleID, int indexOffset, double duration);
    static void changeSublane(const std::string& vehicleID, double latDist);

    static void slowDown(const std::string& vehicleID, double speed, double duration);
    static void openGap(const std::string& vehicleID, double newTimeHeadway, double newSpaceHeadway, double duration, double changeRate, double maxDecel = libsumo::INVALID_DOUBLE_VALUE, const std::string& referenceVehID = "");
    static void deactivateGapControl(const std::string& vehicleID);
    static void requestToC(const std::string& vehID, double leadTime);
    static void setSpeed(const std::string& vehicleID, double speed);
    static void setPreviousSpeed(const std::string& vehicleID, double prevspeed);
    static void setSpeedMode(const std::string& vehicleID, int speedMode);
    static void setLaneChangeMode(const std::string& vehicleID, int laneChangeMode);
    static void setRoutingMode(const std::string& vehicleID, int routingMode);
    static void setType(const std::string& vehicleID, const std::string& typeID);
    static void setRouteID(const std::string& vehicleID, const std::string& routeID);
    static void setRoute(const std::string& vehicleID, const std::string& edgeID);
    static void setRoute(const std::string& vehicleID, const std::vector<std::string>& edgeIDs);
    static void updateBestLanes(const std::string& vehicleID);
    static void setAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID,
                                     double time = libsumo::INVALID_DOUBLE_VALUE, double begSeconds = 0, double endSeconds = std::numeric_limits<double>::max());
    static void setEffort(const std::string& vehicleID, const std::string& edgeID,
                          double effort = libsumo::INVALID_DOUBLE_VALUE, double begSeconds = 0, double endSeconds = std::numeric_limits<double>::max());
    static void rerouteTraveltime(const std::string& vehicleID, const bool currentTravelTimes = true);
    static void rerouteEffort(const std::string& vehicleID);
    static void setSignals(const std::string& vehicleID, int signals);
    static void moveTo(const std::string& vehicleID, const std::string& laneID, double position, int reason = libsumo::MOVE_AUTOMATIC);
    static void moveToXY(const std::string& vehicleID, const std::string& edgeID, const int laneIndex, const double x, const double y, double angle = libsumo::INVALID_DOUBLE_VALUE, const int keepRoute = 1);
    static void remove(const std::string& vehicleID, char reason = libsumo::REMOVE_VAPORIZED);
    static void setLine(const std::string& vehicleID, const std::string& line);
    static void setVia(const std::string& vehicleID, const std::vector<std::string>& via);
    static void highlight(const std::string& vehicleID, const libsumo::TraCIColor& col = libsumo::TraCIColor(255, 0, 0, 255), double size = -1, const int alphaMax = -1, const double duration = -1, const int type = 0);
    static void dispatchTaxi(const std::string& vehicleID,  const std::vector<std::string>& reservations);
    /// @}

    LIBSUMO_VEHICLE_TYPE_SETTER

    LIBSUMO_SUBSCRIPTION_API

    static void subscribeLeader(const std::string& vehicleID, double dist = 0., double beginTime = libsumo::INVALID_DOUBLE_VALUE, double endTime = libsumo::INVALID_DOUBLE_VALUE);

    static void addSubscriptionFilterLanes(const std::vector<int>& lanes, bool noOpposite = false, double downstreamDist = libsumo::INVALID_DOUBLE_VALUE, double upstreamDist = libsumo::INVALID_DOUBLE_VALUE);

    static void addSubscriptionFilterNoOpposite();

    static void addSubscriptionFilterDownstreamDistance(double dist);

    static void addSubscriptionFilterUpstreamDistance(double dist);

    static void addSubscriptionFilterCFManeuver(double downstreamDist = libsumo::INVALID_DOUBLE_VALUE, double upstreamDist = libsumo::INVALID_DOUBLE_VALUE);

    static void addSubscriptionFilterLCManeuver(int direction = libsumo::INVALID_INT_VALUE, bool noOpposite = false, double downstreamDist = libsumo::INVALID_DOUBLE_VALUE, double upstreamDist = libsumo::INVALID_DOUBLE_VALUE);

    static void addSubscriptionFilterLeadFollow(const std::vector<int>& lanes);

    static void addSubscriptionFilterTurn(double downstreamDist = libsumo::INVALID_DOUBLE_VALUE, double upstreamDist = libsumo::INVALID_DOUBLE_VALUE);

    static void addSubscriptionFilterVClass(const std::vector<std::string>& vClasses);

    static void addSubscriptionFilterVType(const std::vector<std::string>& vTypes);

    static void addSubscriptionFilterFieldOfVision(double openingAngle);

    static void addSubscriptionFilterLateralDistance(double lateralDist, double downstreamDist = libsumo::INVALID_DOUBLE_VALUE, double upstreamDist = libsumo::INVALID_DOUBLE_VALUE);

#ifndef LIBTRACI
    /** @brief Saves the shape of the requested object in the given container
    *  @param id The id of the poi to retrieve
    *  @param shape The container to fill
    */
    static void storeShape(const std::string& id, PositionVector& shape);

    static std::shared_ptr<VariableWrapper> makeWrapper();

    static bool handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper);


private:
    static bool isVisible(const SUMOVehicle* veh);

    static bool isOnInit(const std::string& vehicleID);

    static SUMOVehicleParameter::Stop buildStopParameters(const std::string& edgeOrStoppingPlaceID,
            double pos, int laneIndex, double startPos, int flags, double duration, double until);

    static TraCINextStopData buildStopData(const SUMOVehicleParameter::Stop& stopPar);

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;
#endif

    /// @brief invalidated standard constructor
    Vehicle() = delete;
};


}
