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

#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNECalibrator;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorFlow
 * flow flow used by GNECalibrators
 */
class GNECalibratorFlow {
public:

    /// @brief type of flow
    enum TypeOfFlow {
        GNE_CALIBRATORFLOW_INVALID,
        GNE_CALIBRATORFLOW_VEHSPERHOUR,
        GNE_CALIBRATORFLOW_PERIOD,
        GNE_CALIBRATORFLOW_PROBABILITY
    };

    /// @brief constructor
    GNECalibratorFlow(GNECalibrator* calibratorParent);

    /// @brief parameter constructor
    GNECalibratorFlow(GNECalibrator* calibratorParent, std::string flowID, std::string vehicleType, std::string route, std::string color, std::string departLane,
                      std::string departPos, std::string departSpeed, std::string arrivalLane, std::string arrivalPos, std::string arrivalSpeed, std::string line,
                      int personNumber, int containerNumber, bool reroute, std::string departPosLat, std::string arrivalPosLat, double begin, double end,
                      double vehsPerHour, double period, double probability, int number);

    /// @brief destructor
    ~GNECalibratorFlow();

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get ID of flow
    const std::string& getFlowID() const;

    /// @brief get vehicle type of flow
    const std::string& getVehicleType() const;

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

    /// @brief get reroute
    bool getReroute() const;

    /// @brief get departPosLat
    std::string getDepartPosLat() const;

    //// @brief set arrivalPosLat
    std::string getArrivalPosLat() const;

    /// @name value specific of
    ///{
    /// @brief get begin time step
    double getBegin() const;

    /// @brief get end time step
    double getEnd() const;

    /// @brief get flows per hour
    double getVehsPerHour() const;

    /// @brief get period of flow
    double getPeriod() const;

    /// @brief get probability of flow
    double getProbability() const;

    /// @brief get number of flows
    int getNumber() const;

    /// @brief get type of flow
    TypeOfFlow getFlowType() const;
    /// @}

    /**@brief set ID of flow
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setFlowID(std::string ID);

    /**@brief set vehicleType of flow
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setVehicleType(std::string vehicleType);

    /**@brief set route in which this flow is used
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setRoute(std::string route);

    /**@brief set color of flow
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setColor(std::string color = "");

    /**@brief set depart lane
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setDepartLane(std::string departLane = "first");

    /**@brief set depart position
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setDepartPos(std::string departPos = "base");

    /**@brief set depart speed
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setDepartSpeed(std::string departSpeed = "0");

    /**@brief set arrival lane
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setArrivalLane(std::string arrivalLane = "current");

    /**@brief set arrival position
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setArrivalPos(std::string arrivalPos = "max");

    /**@brief set arrival speed
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setArrivalSpeed(std::string arrivalSpeed = "current");

    /**@brief set line of busStop/containerStop
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setLine(std::string line = "");

    /**@brief set number of persons
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setPersonNumber(int personNumber = 0);

    /**@brief set number of persons (String)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setPersonNumber(std::string personNumber = "0");

    /**@brief set number of container
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setContainerNumber(int containerNumber = 0);

    /**@brief set number of container (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setContainerNumber(std::string containerNumber = "0");

    /**@brief set reroute
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setReroute(bool value);

    /**@brief set reroute (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setReroute(std::string value);

    /**@brief set departPosLat
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setDepartPosLat(std::string departPosLat = "center");

    /**@brief set arrivalPosLat
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setArrivalPosLat(std::string arrivalPosLat = "");

    /// @name values specific of Flows
    /// @{
    /**@brief set begin step
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setBegin(double begin);

    /**@brief set begin step (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setBegin(std::string begin);

    /**@brief set end step
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setEnd(double end);

    /**@brief set end step (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setEnd(std::string end);

    /**@brief set flows per hour
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setVehsPerHour(double vehsPerHour);

    /**@brief set flows per hour (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setVehsPerHour(std::string vehsPerHour);

    /**@brief set period of flows
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setPeriod(double period);

    /**@brief set period of flows (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setPeriod(std::string period);

    /**@brief set probability of flow
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setProbability(double probability);

    /**@brief set probability of flow (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setProbability(std::string probability);

    /**@brief set number of flows
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setNumber(int number);

    /**@brief set number of flows (string)
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setNumber(std::string number);

    /**@brief set type of flow
    * @return true if was sucesfully set, or false if value isn't valid
    */
    bool setTypeOfFlow(TypeOfFlow type);
    /// @}

    /// @brief overload operator ==
    bool operator==(const GNECalibratorFlow& calibratorFlow) const;

private:
    /// @brief pointer to calibrator parent
    GNECalibrator* myCalibratorParent;

    /// @brief ID of flow
    std::string myFlowID;

    /// @brief type of flow
    std::string myVehicleType;

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

    /// @brief reroute
    bool myReroute;

    /// @brief departPosLat
    std::string myDepartPosLat;

    //// @brief arrivalPosLat
    std::string myArrivalPosLat;

    /// @name specific of flows
    /// @{
    /// @brief time step begin
    double myBegin;

    /// @brief time step end
    double myEnd;

    /// @brief flows per hour
    double myVehsPerHour;

    /// @brief period
    double myPeriod;

    /// @brief probability
    double myProbability;

    /// @brief number of flow
    int myNumber;
    /// @}

    /// @brief type of flow
    TypeOfFlow myTypeOfFlow;
};

#endif
/****************************************************************************/
