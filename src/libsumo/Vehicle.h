/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
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
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <libsumo/TraCIDefs.h>
#include <traci-server/TraCIConstants.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;
class MSVehicleType;


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
    static double getSpeedWithoutTraCI(const std::string& vehicleID);
    static TraCIPosition getPosition(const std::string& vehicleID);
    static TraCIPosition getPosition3D(const std::string& vehicleID);
    static double getAngle(const std::string& vehicleID);
    static double getSlope(const std::string& vehicleID);
    static std::string getRoadID(const std::string& vehicleID);
    static std::string getLaneID(const std::string& vehicleID);
    static int getLaneIndex(const std::string& vehicleID);
    static std::string getTypeID(const std::string& vehicleID);
    static std::string getRouteID(const std::string& vehicleID);
    static int getRouteIndex(const std::string& vehicleID);
    static TraCIColor getColor(const std::string& vehicleID);
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
    static std::pair<std::string, double> getLeader(const std::string& vehicleID, double dist);
    static double getWaitingTime(const std::string& vehicleID);
    static double getAccumulatedWaitingTime(const std::string& vehicleID);
    static double getAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID, int time);
    static double getEffort(const std::string& vehicleID, const std::string& edgeID, int time);
    static bool isRouteValid(const std::string& vehicleID);
    static std::vector<std::string> getEdges(const std::string& vehicleID);
    static int getSignalStates(const std::string& vehicleID);
    static std::vector<TraCIBestLanesData> getBestLanes(const std::string& vehicleID);
    static std::vector<TraCINextTLSData> getNextTLS(const std::string& vehicleID);
    static int getStopState(const std::string& vehicleID);
    static double getDistance(const std::string& vehicleID);
    static double getDrivingDistance(const std::string& vehicleID, const std::string& edgeID, double position, int laneIndex);
    static double getDrivingDistance2D(const std::string& vehicleID, double x, double y);
    static double getAllowedSpeed(const std::string& vehicleID);
    static double getSpeedFactor(const std::string& vehicleID);
    static int getSpeedMode(const std::string& vehicleID);
    static int getLanechangeMode(const std::string& vehicleID);
    static int getRoutingMode(const std::string& vehicleID);
    static std::string getLine(const std::string& vehicleID);
    static std::vector<std::string> getVia(const std::string& vehicleID);
    static std::pair<int, int> getLaneChangeState(const std::string& vehicleID, int direction);
    static std::string getParameter(const std::string& vehicleID, const std::string& key);
    static const MSVehicleType& getVehicleType(const std::string& vehicleID);
    /// @}

    /// @name vehicle type value retrieval shortcuts
    /// @{
    static std::string getEmissionClass(const std::string& vehicleID);
    static std::string getShapeClass(const std::string& vehicleID);
    static std::string getVClass(const std::string& vehicleID);
    static double getLength(const std::string& vehicleID);
    static double getAccel(const std::string& vehicleID);
    static double getDecel(const std::string& vehicleID);
    static double getEmergencyDecel(const std::string& vehicleID);
    static double getApparentDecel(const std::string& vehicleID);
    static double getActionStepLength(const std::string& vehicleID);
    static double getLastActionTime(const std::string& vehicleID);
    static double getTau(const std::string& vehicleID);
    static double getImperfection(const std::string& vehicleID);
    static double getSpeedDeviation(const std::string& vehicleID);
    static double getMinGap(const std::string& vehicleID);
    static double getMaxSpeed(const std::string& vehicleID);
    static double getWidth(const std::string& vehicleID);
    /// @}

    /// @name State Changing
    /// @{

    static void setStop(const std::string& vehicleID,
                        const std::string& edgeID,
                        double endPos = 1.,
                        int laneIndex = 0,
                        SUMOTime duration = 4294967295u, // 2^32-1
                        int flags = STOP_DEFAULT,
                        double startPos = INVALID_DOUBLE_VALUE,
                        SUMOTime until = -1);

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
    static void changeLane(const std::string& vehicleID, int laneIndex, SUMOTime duration);
    static void changeSublane(const std::string& vehicleID, double latDist);

    static void slowDown(const std::string& vehicleID, double speed, SUMOTime duration);
    static void setSpeed(const std::string& vehicleID, double speed);
    static void setSpeedMode(const std::string& vehicleID, int speedMode);
    static void setLaneChangeMode(const std::string& vehicleID, int laneChangeMode);
    static void setRoutingMode(const std::string& vehicleID, int routingMode);
    static void setType(const std::string& vehicleID, const std::string& typeID);
    static void setRouteID(const std::string& vehicleID, const std::string& routeID);
    static void setRoute(const std::string& vehicleID, const std::vector<std::string>& edgeIDs);
    static void setAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID,
                                     double time = INVALID_DOUBLE_VALUE, double begSeconds = 0, double endSeconds = std::numeric_limits<double>::max());
    static void setEffort(const std::string& vehicleID, const std::string& edgeID,
                          double effort = INVALID_DOUBLE_VALUE, double begSeconds = 0, double endSeconds = std::numeric_limits<double>::max());
    static void rerouteTraveltime(const std::string& vehicleID);
    static void rerouteEffort(const std::string& vehicleID);
    static void setSignals(const std::string& vehicleID, int signals);
    static void moveTo(const std::string& vehicleID, const std::string& laneID, double position);
    static void moveToXY(const std::string& vehicleID, const std::string& edgeID, const int laneIndex, const double x, const double y, double angle, const int keepRouteFlag);
    static void setMaxSpeed(const std::string& vehicleID, double speed);
    static void setActionStepLength(const std::string& vehicleID, double actionStepLength, bool resetActionOffset = true);
    static void remove(const std::string& vehicleID, char reason = REMOVE_VAPORIZED);
    static void setColor(const std::string& vehicleID, const TraCIColor& col);
    static void setSpeedFactor(const std::string& vehicleID, double factor);
    static void setLine(const std::string& vehicleID, const std::string& line);
    static void setVia(const std::string& vehicleID, const std::vector<std::string>& via);
    static void setShapeClass(const std::string& vehicleID, const std::string& clazz);
    static void setEmissionClass(const std::string& vehicleID, const std::string& clazz);
    static void setParameter(const std::string& vehicleID, const std::string& key, const std::string& value);
    /// @}

private:
    static MSVehicle* getVehicle(const std::string& id);

    static bool isVisible(const MSVehicle* veh);

    static bool isOnInit(const std::string& vehicleID);

    /// @brief invalidated standard constructor
    Vehicle();

    /// @brief invalidated copy constructor
    Vehicle(const Vehicle& src);

    /// @brief invalidated assignment operator
    Vehicle& operator=(const Vehicle& src);
};
}


#endif

/****************************************************************************/
