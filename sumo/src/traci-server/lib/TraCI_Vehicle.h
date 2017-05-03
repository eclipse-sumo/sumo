/****************************************************************************/
/// @file    TraCI_Vehicle.h
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    15.03.2017
/// @version $Id$
///
// C++ Vehicle API
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCI_Vehicle_h
#define TraCI_Vehicle_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <traci-server/TraCIDefs.h>
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
 * @class TraCI_Vehicle
 * @brief C++ TraCI client API implementation
 */
class TraCI_Vehicle {
public:
    struct NextTLSData {
        /* @brief Constructor */
        NextTLSData() {}
        /// @brief The id of the next tls
        std::string id;
        /// @brief The tls index of the controlled link
        int tlIndex;
        /// @brief The distance to the tls
        double dist;
        /// @brief The current state of the tls
        char state;
    };

    struct BestLanesData {
        /* @brief Constructor */
        BestLanesData() {}
        /// @brief The id of the lane
        std::string laneID;
        /// @brief The length than can be driven from that lane without lane change
        double length;
        /// @brief The traffic density along length
        double nextOccupation;
        /// @brief The offset of this lane from the best lane
        int bestLaneOffset;
        /// @brief Whether this lane allows continuing the route
        bool allowsContinuation;
        /// @brief The sequence of lanes that best allows continuing the route without lane change
        std::vector<std::string> continuationLanes;
    };


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
    static double getAdaptedTraveltime(const std::string& vehicleID, const std::string& edgeID, int time);
    static double getEffort(const std::string& vehicleID, const std::string& edgeID, int time);
    static bool isRouteValid(const std::string& vehicleID);
    static std::vector<std::string> getEdges(const std::string& vehicleID);
    static int getSignalStates(const std::string& vehicleID);
    static std::vector<BestLanesData> getBestLanes(const std::string& vehicleID);
    static std::vector<NextTLSData> getNextTLS(const std::string& vehicleID);
    static int getStopState(const std::string& vehicleID);
    static double getDistance(const std::string& vehicleID);
    static double getDrivingDistance(const std::string& vehicleID, const std::string& edgeID, double position, int laneIndex);
    static double getDrivingDistance2D(const std::string& vehicleID, double x, double y);
    static double getAllowedSpeed(const std::string& vehicleID);
    static double getSpeedFactor(const std::string& vehicleID);
    static int getSpeedMode(const std::string& vehicleID);
    static std::string getLine(const std::string& vehicleID);
    static std::vector<std::string> getVia(const std::string& vehicleID);
    static std::pair<int, int> getLaneChangeState(const std::string& vehicleID, int direction);
    static std::string getParameter(const std::string& vehicleID, const std::string& key);

    static const MSVehicleType& getVehicleType(const std::string& vehicleID);
    /* Type requests not yet implemented
    static std::string getEmissionClass(const std::string& vehicleID);
    static std::string getShapeClass(const std::string& vehicleID);
    static std::string getVClass(const std::string& vehicleID);
    static double getLength(const std::string& vehicleID);
    static double getAccel(const std::string& vehicleID);
    static double getDecel(const std::string& vehicleID);
    static double getEmergencyDecel(const std::string& vehicleID);
    static double getApparentDecel(const std::string& vehicleID);
    static double getTau(const std::string& vehicleID);
    static double getImperfection(const std::string& vehicleID);
    static double getSpeedDeviation(const std::string& vehicleID);
    static double getMinGap(const std::string& vehicleID);
    static double getMaxSpeed(const std::string& vehicleID);
    static double getWidth(const std::string& vehicleID);
    */
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
                    std::string depart = "-1",
                    const std::string& departLane = "first",
                    const std::string& departPos = "base",
                    const std::string& departSpeed = "0",
                    const std::string& arrivalLane = "current",
                    const std::string& arrivalPos = "max",
                    const std::string& arrivalSpeed = "current",
                    const std::string& fromTaz = "",
                    const std::string& toTaz = "",
                    const std::string& line = "",
                    int personCapacity = 0,
                    int personNumber = 0);

    static void changeLane(const std::string& vehID, int laneIndex, SUMOTime duration);

    static void moveTo(const std::string& vehicleID, const std::string& laneID, double position);
    static void moveToXY(const std::string& vehicleID, const std::string& edgeID, const int lane, const double x, const double y, const double angle, const int keepRoute);
    static void slowDown(const std::string& vehicleID, double speed, int duration);
    static void setSpeed(const std::string& vehicleID, double speed);
    static void setMaxSpeed(const std::string& vehicleID, double speed);
    static void remove(const std::string& vehicleID, char reason = REMOVE_VAPORIZED);
    static void setColor(const std::string& vehicleID, const TraCIColor& c);
    static void setLine(const std::string& vehicleID, const std::string& line);
    static void setVia(const std::string& vehicleID, const std::vector<std::string>& via);
    static void setShapeClass(const std::string& vehicleID, const std::string& clazz);
    static void setEmissionClass(const std::string& vehicleID, const std::string& clazz);
    static void setParameter(const std::string& vehicleID, const std::string& key, const std::string& value);
    /// @}


private:
    static MSVehicle* getVehicle(const std::string& id);

    static bool isVisible(const MSVehicle* veh);

    static bool onInit(const std::string& vehicleID);

    /// @brief invalidated standard constructor
    TraCI_Vehicle();

    /// @brief invalidated copy constructor
    TraCI_Vehicle(const TraCI_Vehicle& src);

    /// @brief invalidated assignment operator
    TraCI_Vehicle& operator=(const TraCI_Vehicle& src);
};


#endif

/****************************************************************************/

