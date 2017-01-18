/****************************************************************************/
/// @file    MSSOTLDefinitions.h
/// @author  Gianfilippo Slager
/// @date    Mar 2010
/// @version $Id$
///
// The repository for definitions about SOTL and Swarm-based logics
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

#ifndef MSSOTLDefinitions_h
#define MSSOTLDefinitions_h

///For MSSOTLSensors
//SENSOR_START in meters, counting from the traffic light and moving backward with respect to traffic direction
#define SENSOR_START 0.0f
//INPUT_SENSOR_LENGTH in meters, counting from SENSOR_START and moving backward with respect to traffic direction
#define INPUT_SENSOR_LENGTH 100.0f
#define OUTPUT_SENSOR_LENGTH 80.0f

//TODO Check the distances for the count sensors
#define INPUT_COUNT_SENSOR_LENGTH 15.0f
#define OUTPUT_COUNT_SENSOR_LENGTH 15.0f
#define COUNT_SENSOR_START 10000.0f
////For MSSOTLE2Sensors
//E2 Detector parameter: the time in seconds a vehicle's speed must be below haltingSpeedThreshold to be assigned as jammed
#define HALTING_TIME_THRS 10
//E2 Detector parameter: the speed in meters/second a vehicle's speed must be below to be assigned as jammed
#define HALTING_SPEED_THRS 1
//E2 Detector parameter: the distance in meters between two vehicles in order to not count them to one jam
#define DIST_THRS 20.0

//#define SENSORS_TYPE "e2"
#define SENSORS_TYPE_E1 1
#define SENSORS_TYPE_E2 2
#define SENSORS_TYPE SENSORS_TYPE_E2

#include <stdlib.h>

#include <map>
#include <utility>
#include <microsim/output/MSE2Collector.h>
//Every lane has its own sensors, one at the beginning and one at the end
//Sensors can be retrieved by lanes pointer
typedef std::pair<MSLane*, MSE2Collector*> MSLane_MSE2Collector;
typedef std::map<MSLane*, MSE2Collector*> MSLane_MSE2CollectorMap;

//Sensors can be retrieved by lane Id
typedef std::pair<std::string, MSE2Collector*> MSLaneID_MSE2Collector;
typedef std::map<std::string, MSE2Collector*> MSLaneID_MSE2CollectorMap;

//Every lane has its speed limit, it can be retrieved by lane Id
typedef std::pair<std::string, SUMOReal> MSLaneID_MaxSpeed;
typedef std::map<std::string, SUMOReal> MSLaneID_MaxSpeedMap;

//****************************************************
//Type definitions to implement the pheromone paradigm
typedef std::pair<std::string, SUMOReal> MSLaneId_Pheromone;
/**
* This map type definition identifies a set of lanes, connected to a kind of pheromone.
* Pheromone can be of different kinds to express different stimuli
*/
typedef std::map<std::string, SUMOReal> MSLaneId_PheromoneMap;

//****************************************************

typedef std::set<std::string> MSLaneID_set;

typedef std::map<MSLane*, bool> LaneCheckMap;
typedef std::vector<std::string> LaneIdVector;

#endif
/****************************************************************************/
