/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    Vehicle.h
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    15.03.2017
/// @version $Id$
///
// C++ Vehicle API
/****************************************************************************/
#ifndef Vehicle_h
#define Vehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <limits>
#include <vector>
#include <libsumo/TraCIDefs.h>
#include <libsumo/VehicleType.h>
#include <libsumo/TraCIConstants.h>


// ===========================================================================
// class declarations
// ===========================================================================
namespace libsumo {
class VariableWrapper;
}
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class Vehicle
 * @brief C++ TraCI client API implementation
 */
namespace libsumo {
class Vehicle {
public:
    /// @name Value retrieval
    /// @{
    static std::vector<std::string> getIDList();
    static int getIDCount();
    static double getSpeed(const std::string& vehicleID);
    static double getLateralSpeed(const std::string& vehicleID);
    static double getAcceleration(const std::string& vehicleID);
    static double getSpeedWithoutTraCI(const std::string& vehicleID);
    static TraCIPosition getPosition(const std::string& vehicleID, const bool includeZ = false);
    static TraCIPosition getPosition3D(const std::string& vehicleID);
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
    static double getWaitingTime(const std::string& vehicleID);
    static double getAccumulatedWaitingTime(const std::string& vehicleID);
    static double getAdaptedTraveltime(const std::string& vehicleID, double time, const std::string& edgeID);
    static double getEffort(const std::string& vehicleID, double time, const std::string& edgeID);
    static bool isRouteValid(const std::string& vehicleID);
    static std::vector<std::string> getRoute(const std::string& vehicleID);
    static int getSignals(const std::string& vehicleID);
    static std::vector<TraCIBestLanesData> getBestLanes(const std::string& vehicleID);
    static std::vector<TraCINextTLSData> getNextTLS(const std::string& vehicleID);
    static std::vector<TraCINextStopData> getNextStops(const std::string& vehicleID);
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
    static std::string getParameter(const std::string& vehicleID, const std::string& key);
    static std::vector<std::pair<std::string, double> > getNeighbors(const std::string& vehicleID, const int mode);
    /// @}

    LIBSUMO_VEHICLE_TYPE_GETTER

    /// @name State Changing
    /// @{

    static void setStop(const std::string& vehicleID,
                        const std::string& edgeID,
                        double pos = 1.,
                        int laneIndex = 0,
                        double duration = INVALID_DOUBLE_VALUE,
                        int flags = STOP_DEFAULT,
                        double startPos = INVALID_DOUBLE_VALUE,
                        double until = INVALID_DOUBLE_VALUE);

    static void rerouteParkingArea(const std::string& vehicleID,
                                   const std::string& parkingAreaID);

    static void resume(const std::string& vehicleID);

    static void add(const std::string& vehicleID,
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
    static void openGap(const std::string& vehicleID, double newTimeHeadway, double newSpaceHeadway, double duration, double changeRate, double maxDecel = INVALID_DOUBLE_VALUE, const std::string& referenceVehID = "");
    static void deactivateGapControl(const std::string& vehicleID);
    static void requestToC(const std::string& vehID, double leadTime);
    static void setSpeed(const std::string& vehicleID, double speed);
    static void setSpeedMode(const std::string& vehicleID, int speedMode);
    static void setLaneChangeMode(const std::string& vehicleID, int laneChangeMode);
    static void setRoutingMode(const std::string& vehicleID, int routingMode);
    static void setType(const std::string& vehicleID, const std::string& typeID);
    static void setRouteID(const std::string& vehicleID, const std::string& routeID);
    static void setRoute(const std::string& vehicleID, const std::vector<std::string>& edgeIDs);
    static void updateBestLanes(const std::string& vehicleID);
    static void setAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID,
                                     double time = INVALID_DOUBLE_VALUE, double begSeconds = 0, double endSeconds = std::numeric_limits<double>::max());
    static void setEffort(const std::string& vehicleID, const std::string& edgeID,
                          double effort = INVALID_DOUBLE_VALUE, double begSeconds = 0, double endSeconds = std::numeric_limits<double>::max());
    static void rerouteTraveltime(const std::string& vehicleID, const bool currentTravelTimes = true);
    static void rerouteEffort(const std::string& vehicleID);
    static void setSignals(const std::string& vehicleID, int signals);
    static void moveTo(const std::string& vehicleID, const std::string& laneID, double position);
    static void moveToXY(const std::string& vehicleID, const std::string& edgeID, const int laneIndex, const double x, const double y, double angle = INVALID_DOUBLE_VALUE, const int keepRoute = 1);
    static void remove(const std::string& vehicleID, char reason = REMOVE_VAPORIZED);
    static void setLine(const std::string& vehicleID, const std::string& line);
    static void setVia(const std::string& vehicleID, const std::vector<std::string>& via);
    static void setParameter(const std::string& vehicleID, const std::string& key, const std::string& value);
    static void highlight(const std::string& vehicleID, const TraCIColor& col, double size, const int alphaMax, const double duration, const int type);
    /// @}

    LIBSUMO_VEHICLE_TYPE_SETTER

    LIBSUMO_SUBSCRIPTION_API

    static void subscribeLeader(const std::string& vehicleID, double dist = 0., double beginTime = libsumo::INVALID_DOUBLE_VALUE, double endTime = libsumo::INVALID_DOUBLE_VALUE);

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

private:
    static SubscriptionResults mySubscriptionResults;
    static ContextSubscriptionResults myContextSubscriptionResults;

    /// @brief invalidated standard constructor
    Vehicle() = delete;
};


}


#endif

/****************************************************************************/
