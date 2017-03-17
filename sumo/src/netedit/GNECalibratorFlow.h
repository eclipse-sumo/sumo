/****************************************************************************/
/// @file    GNECalibratorFlow.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// Flow used by GNECalibrators
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GNECalibratorFlow_h
#define GNECalibratorFlow_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

// ===========================================================================
// class declaration
// ===========================================================================

class GNECalibrator;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorFlow
 * vehicle flow used by GNECalibrators
 */
class GNECalibratorFlow {

public:

    /// @brief constructor
    GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route);

    /// @brief parameter constructor
    GNECalibratorFlow(GNECalibrator* calibratorParent, std::string type, std::string route, std::string color, std::string departLane, std::string departPos,
                        std::string departSpeed, std::string arrivalLane, std::string arrivalPos, std::string arrivalSpeed, std::string line, int personNumber, 
                        int containerNumber, double begin, double end, double vehsPerHour, double period, double probability, int number);

    /// @brief destructor
    ~GNECalibratorFlow();

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief get type of vehicle
    const std::string& getType() const;

    /// @brief get route in which this flow is used
    const std::string& getRoute() const;

    /// @brief get color of flow
    const std::string& getColor() const;

    /// @brief get depart lane
    const std::string& getDepartLane() const;

    /// @brief get depart position
    const std::string& getDepartPos() const;

    /// @brief get arrival speed
    const std::string& getDepartSpeed() const;

    /// @brief get arrival lane
    const std::string& getArrivalLane() const;

    /// @brief get arrival position
    const std::string& getArrivalPos() const;

    /// @brief get arrival speed
    const std::string& getArrivalSpeed() const;

    /// @brief get line of busStop/containerStop
    const std::string& getLine() const;

    /// @brief get number of persons
    int getPersonNumber() const;

    /// @brief get number of containers
    int getContainerNumber() const;

    /// @brief get begin time step
    double getBegin() const;

    /// @brief get end time step
    double getEnd() const;

    /// @brief get vehicles per hour
    double getVehsPerHour() const;

    /// @brief get period of vehicle
    double getPeriod() const;

    /// @brief get probability of vehicle
    double getProbability() const;

    /// @brief get number of vehicles
    int getNumber() const;

    /**@brief set type of vehicle
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setType(std::string type);

    /**@brief set route in which this flow is used
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setRoute(std::string route);

    /**@brief set color of flow
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setColor(std::string color);

    /**@brief set depart lane
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setDepartLane(std::string departLane);

    /**@brief set depart position
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setDepartPos(std::string departPos);

    /**@brief set depart speed
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setDepartSpeed(std::string departSpeed);

    /**@brief set arrival lane
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setArrivalLane(std::string arrivalLane);

    /**@brief set arrival position
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setArrivalPos(std::string arrivalPos);

    /**@brief set arrival speed
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setArrivalSpeed(std::string arrivalSpeed);

    /**@brief set line of busStop/containerStop
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setLine(std::string line);

    /**@brief set number of persons
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setPersonNumber(int personNumber);

    /**@brief set number of container
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setContainerNumber(int containerNumber);

    /**@brief set begin step
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setBegin(double begin);

    /**@brief set end step
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setEnd(double end);

    /**@brief set vehicles per hour
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setVehsPerHour(double vehsPerHour);

    /**@brief set period of vehicles
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setPeriod(double period);

    /**@brief set probability of vehicle
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setProbability(double probability);

    /**@brief set number of vehicles
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setNumber(int number);

private:
    /// @brief pointer to calibrator parent
    GNECalibrator* myCalibratorParent;

    /// @brief type of flow
    std::string myType;

    /// @brief route in which this flow is used
    std::string myRoute;

    /// @brief color of flow
    std::string myColor;

    /// @brief depart lane
    std::string myDepartLane;

    /// @brief depart position
    std::string myDepartPos;

    /// @brief depart speed
    std::string myDepartSpeed;

    /// @brief arrival lane
    std::string myArrivalLane;

    /// @brief arrival pos
    std::string myArrivalPos;

    /// @brief arrival speed
    std::string myArrivalSpeed;

    /// @brief line of bus/container stop
    std::string myLine;

    /// @brief number of person
    int myPersonNumber;

    /// @brief number of container
    int myContainerNumber;

    /// @brief time step begin
    double myBegin;

    /// @brief time step end
    double myEnd;

    /// @brief vehicles per hour
    double myVehsPerHour;

    /// @brief period
    double myPeriod;

    /// @brief probability
    double myProbability;

    /// @brief number of vehicle
    int myNumber;
};

#endif
/****************************************************************************/
