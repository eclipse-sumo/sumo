/****************************************************************************/
/// @file    MSSOTLE2Sensors.h
/// @author  Gianfilippo Slager
/// @author  Federico Caselli
/// @date    Feb 2010
/// @version $Id$
///
// The class for SOTL sensors of "E2" type
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2010-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSSOTLE2Sensors_h
#define MSSOTLE2Sensors_h


#define SWARM_DEBUG
#include <utils/common/SwarmDebug.h>
#include "MSSOTLSensors.h"
#include "MSSOTLDefinitions.h"
#include <math.h>
#include <map>
#include <utility>
#include <microsim/output/MSE2Collector.h>

class MSSOTLE2Sensors :	public MSSOTLSensors {
protected :
    void buildSensorForLane(MSLane* lane, NLDetectorBuilder& nb);
    void buildSensorForLane(MSLane* lane, NLDetectorBuilder& nb, SUMOReal sensorLength);
    void buildContinueSensior(MSLane* lane, NLDetectorBuilder& nb, SUMOReal sensorLength, MSLane* continueOnLane, SUMOReal usedLength);
    void buildSensorForOutLane(MSLane* lane, NLDetectorBuilder& nb);
    void buildSensorForOutLane(MSLane* lane, NLDetectorBuilder& nb, SUMOReal sensorLength);

    void buildCountSensorForLane(MSLane* lane, NLDetectorBuilder& nb);
    void buildCountSensorForOutLane(MSLane* lane, NLDetectorBuilder& nb);

public:
    /*
    * @brief This sensor logic contructor
    */
    MSSOTLE2Sensors(std::string tlLogicID, const MSTrafficLightLogic::Phases* phases);

    /*
    * @brief This sensor logic destructor
    */
    ~MSSOTLE2Sensors(void);

    void buildSensors(MSTrafficLightLogic::LaneVectorVector controlledLanes, NLDetectorBuilder& nb);
    void buildSensors(MSTrafficLightLogic::LaneVectorVector controlledLanes, NLDetectorBuilder& nb, SUMOReal sensorLength);
    void buildOutSensors(MSTrafficLightLogic::LaneVectorVector controlledLanes, NLDetectorBuilder& nb);
    void buildOutSensors(MSTrafficLightLogic::LaneVectorVector controlledLanes, NLDetectorBuilder& nb, SUMOReal sensorLength);
    void buildCountSensors(MSTrafficLightLogic::LaneVectorVector controlledLanes, NLDetectorBuilder& nb);
    void buildCountOutSensors(MSTrafficLightLogic::LaneVectorVector controlledLanes, NLDetectorBuilder& nb);

    /*
     * Returns the number of vehicles that have passed the sensor.
     * Vehicles are effectively counted or guessed in the space from the sensor to the junction.
     * @param[in] laneId The ID of the lane of which you want to count vehicles.
     * @param[in] out boolean that indicate if the lane is an incoming or an outgoing one (@todo review)
     */
    int getPassedVeh(std::string laneId, bool out);

    /*
     * Sets the number of vehicles that have passed the sensor.
     * @param[in] laneId The ID of the lane of which you want to set vehicles.
     * @param[in] passed int indicating the number of vehicles to subtract.
     */
    void subtractPassedVeh(std::string laneId, int passed);

    void setSpeedThresholdParam(SUMOReal newThreshold) {
        speedThresholdParam = newThreshold;
    }

    /*
     * Returns the number of vehicles currently approaching the
     * junction for the given lane.
     * Vehicles are effectively counted or guessed in the space from the sensor to the junction.
     * @param[in] lane The lane to count vehicles
     */
    int countVehicles(MSLane* lane);

    /*
     * Returns the number of vehicles currently approaching the
     * junction for the given lane.
     * Vehicles are effectively counted or guessed in the space from the sensor to the junction.
     * @param[in] lane The lane to count vehicles given by ID
     */
    int countVehicles(std::string laneId);

    int estimateVehicles(std::string laneId);

    SUMOReal getEstimateQueueLength(std::string laneId);
    /*
    * @param[in] The lane given by Id
    * @return The maximum speed allowed for the given laneId
    */
    virtual SUMOReal getMaxSpeed(std::string laneId);

    /*
     * Returns the average speed of vehicles currently approaching or leaving the
     * junction for the given lane.
     * Vehicles speed is effectively sensed or guessed in the space from the sensor.
     * @param[in] lane The lane to count vehicles
     */
    virtual SUMOReal meanVehiclesSpeed(MSLane* lane);

    /*
     * Returns the average speed of vehicles currently approaching or leaving the
     * junction for the given lane.
     * Vehicles speed is effectively sensed or guessed in the space from the sensor.
     * @param[in] laneID The lane to count vehicles by ID
     */
    virtual SUMOReal meanVehiclesSpeed(std::string laneId);

    /*
     * Set the weight of the vehicle types to be used by countVehicles
     * @param[in] the value of the param VEHICLE_TYPES_WEIGHTS. Format type1=value1; type2=value2 etc..
     */
    virtual void setVehicleWeigths(const std::string& weightString);

protected:
    int count(MSE2Collector* sensor);
    template<typename Method, typename ValueType>
    bool getVelueFromSensor(std::string laneId, Method function, ValueType& value) {
        if (m_sensorMap.find(laneId) != m_sensorMap.end()) {
            value = (m_sensorMap[laneId]->*function)();
            return true;
        }
        return false;
    }

//	MSLane_MSE2CollectorMap m_sensorMap;
    MSLaneID_MSE2CollectorMap m_sensorMap;
    MSLaneID_MaxSpeedMap m_maxSpeedMap;

//	MSLane_MSE2CollectorMap mySensorsMap_OutLanes;
//	MSLaneID_MSE2CollectorMap mySensorsIDMap_OutLanes;
//	MSLaneID_MaxSpeedMap myMaxSpeedMap_OutLanes;

    SUMOReal speedThresholdParam;
    std::map<std::string, std::vector<std::string> > m_continueSensorOnLanes;
    std::map<const std::string, int> m_typeWeightMap;

};

#endif
/****************************************************************************/
