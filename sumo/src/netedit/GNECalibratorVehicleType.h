/****************************************************************************/
/// @file    GNECalibratorVehicleType.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
/// VehicleType used by GNECalibrators
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
#ifndef GNECalibratorVehicleType_h
#define GNECalibratorVehicleType_h


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
#include <utils/common/SUMOVehicleClass.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNECalibrator;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorVehicleType
 * vehicleType vehicleType used by GNECalibrators
 */
class GNECalibratorVehicleType {
public:
    /// @brief constructor
    GNECalibratorVehicleType(GNECalibrator* calibratorParent);

    /// @brief parameter constructor
    GNECalibratorVehicleType(GNECalibrator* calibratorParent, std::string vehicleTypeID,
                             double accel, double decel, double sigma, double tau, double length, double minGap,
                             double maxSpeed, double speedFactor, double speedDev, const std::string& color,
                             SUMOVehicleClass vClass, const std::string& emissionClass, SUMOVehicleShape shape,
                             double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
                             const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
                             double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat);

    /// @brief destructor
    ~GNECalibratorVehicleType();

    /// @brief get pointer to calibrator parent
    GNECalibrator* getCalibratorParent() const;

    /// @brief get tag
    SumoXMLTag getTag() const;

    /// @brief get VehicleTypeID
    std::string getVehicleTypeID() const;

    /// @brief get accel
    double getAccel() const;

    /// @brief get decel
    double getDecel() const;

    /// @brief get sigma
    double getSigma() const;

    /// @brief get tau
    double getTau() const;

    /// @brief get length
    double getLength() const;

    /// @brief get min gap
    double getMinGap() const;

    /// @brief get max speed
    double getMaxSpeed() const;

    /// @brief get speed factor
    double getSpeedFactor() const;

    /// @brief get speed dev
    double getSpeedDev() const;

    /// @brief get color
    std::string getColor() const;

    /// @brief get VClass
    SUMOVehicleClass getVClass() const;

    /// @brief get emission class
    std::string getEmissionClass() const;

    /// @brief get shape
    SUMOVehicleShape getShape() const;

    /// @brief get width
    double getWidth() const;

    /// @brief get filename
    std::string getFilename() const;

    /// @brief get impatience
    double getImpatience() const;

    /// @brief get lane change model
    std::string getLaneChangeModel() const;

    /// @brief get car follow model
    std::string getCarFollowModel() const;

    /// @brief get person capacity
    int getPersonCapacity() const;

    /// @brief get container capacity
    int getContainerCapacity() const;

    /// @brief get boarding duration
    double getBoardingDuration() const;

    /// @brief get loading duration
    double getLoadingDuration() const;

    /// @brief get lateral lat
    std::string getLatAlignment() const;

    /// @brief get min gap lat
    double getMinGapLat() const;

    /// @brief get max speed lat
    double getMaxSpeedLat() const;

    /**@brief set vehicleType ID
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setVehicleTypeID(std::string vehicleTypeID);

    /**@brief set accel
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setAccel(double accel = 2.6);

    /**@brief set accel (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setAccel(std::string accel);

    /**@brief set decel
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setDecel(double decel = 4.5);

    /**@brief set decel (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setDecel(std::string decel);

    /**@brief set sigma
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setSigma(double sigma = 0.5);

    /**@brief set sigma (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setSigma(std::string sigma);

    /**@brief set tau
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setTau(double tau = 1.0);

    /**@brief set tau (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setTau(std::string tau);

    /**@brief set length
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setLength(double length = 5.0);

    /**@brief set length (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setLength(std::string length);

    /**@brief set min gap
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMinGap(double minGap = 2.5);

    /**@brief set min gap (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMinGap(std::string minGap);

    /**@brief set max speed
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMaxSpeed(double maxSpeed = 70.0);

    /**@brief set max speed (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMaxSpeed(std::string maxSpeed);

    /**@brief set speed factor
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setSpeedFactor(double speedFactor = 1.0);

    /**@brief set speed factor (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setSpeedFactor(std::string speedFactor);

    /**@brief set speed dev
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setSpeedDev(double speedDev = 0.0);

    /**@brief set speed dev (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setSpeedDev(std::string speedDev);

    /**@brief set color
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setColor(std::string color = "1,1,0");

    /**@brief set VClass
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setVClass(SUMOVehicleClass vClass = SVC_PRIVATE);

    /**@brief set VClass (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setVClass(std::string vClass);

    /**@brief set emission class
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setEmissionClass(std::string emissionClass = "P_7_7");

    /**@brief set shape (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setShape(std::string shape);

    /**@brief set shape
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setShape(SUMOVehicleShape shape = SVS_PASSENGER);

    /**@brief set width
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setWidth(double width = 2.0);

    /**@brief set width (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setWidth(std::string width);

    /**@brief set filename
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setFilename(std::string filename = "");

    /**@brief set impatience
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setImpatience(double impatience = 0.0);

    /**@brief set impatience (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setImpatience(std::string impatience);

    /**@brief set lane change model
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setLaneChangeModel(std::string laneChangeModel = "LC2013");

    /**@brief set car follow model
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setCarFollowModel(std::string carFollowModel = "Krauss");

    /**@brief set person capacity
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setPersonCapacity(int personCapacity = 4);

    /**@brief set person capacity (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setPersonCapacity(std::string personCapacity);

    /**@brief set container capacity
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setContainerCapacity(int containerCapacity = 0);

    /**@brief set container capacity (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setContainerCapacity(std::string containerCapacity);

    /**@brief set boarding duration
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setBoardingDuration(double boardingDuration = 0.5);

    /**@brief set boarding duration (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setBoardingDuration(std::string boardingDuration);

    /**@brief set loading duration
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setLoadingDuration(double loadingDuration = 90.0);

    /**@brief set loading duration (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setLoadingDuration(std::string loadingDuration);

    /**@brief set lateral lat
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setLatAlignment(std::string latAlignment = "center");

    /**@brief set min gap lat
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMinGapLat(double minGapLat = 0.12);

    /**@brief set min gap lat (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMinGapLat(std::string minGapLat);

    /**@brief set max speed lat
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMaxSpeedLat(double maxSpeedLat = 1.0);

    /**@brief set max speed lat (string version)
     * @return true if was sucesfully set, or false if value isn't valid
     */
    bool setMaxSpeedLat(std::string maxSpeedLat);

    /// @brief overload operator ==
    bool operator==(const GNECalibratorVehicleType& calibratorVehicleType) const;

private:
    /// @brief pointer to calibrator parent
    GNECalibrator* myCalibratorParent;

    /// @brief vehicleType ID
    std::string myVehicleTypeID;

    /// @brief The acceleration ability of vehicles of this type (in m/s^2)
    double myAccel;

    /// @brief The deceleration ability of vehicles of this type (in m/s^2)
    double myDecel;

    /// @brief Car-following model parameter
    double mySigma;

    /// @brief Car-following model parameter
    double myTau;

    /// @brief The vehicle's netto-length (length) (in m)
    double myLength;

    /// @brief Empty space after leader [m]
    double myMinGap;

    /// @brief The vehicle's maximum velocity (in m/s)
    double myMaxSpeed;

    /// @brief The vehicles expected multiplicator for lane speed limits
    double mySpeedFactor;

    /// @brief The deviation of the speedFactor; see below for details
    double mySpeedDev;

    /// @brief This vehicle type's color
    std::string myColor;

    /// @brief An abstract vehicle class
    SUMOVehicleClass myVClass;

    /// @brief An abstract emission class
    std::string myEmissionClass;

    /// @brief How this vehicle is rendered
    SUMOVehicleShape myShape;

    /// @brief The vehicle's width [m] (only used for drawing)
    double myWidth;

    /// @brief Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)
    std::string myFilename;

    /// @brief Willingess of drivers to impede vehicles with higher priority. See below for semantics.
    double myImpatience;

    /// @brief The model used for changing lanes
    std::string myLaneChangeModel;

    /// @brief The model used for car following
    std::string myCarFollowModel;

    /// @brief The number of persons (excluding an autonomous driver) the vehicle can transport.
    int myPersonCapacity;

    /// @brief The number of containers the vehicle can transport.
    int myContainerCapacity;

    /// @brief The time required by a person to board the vehicle.
    double myBoardingDuration;

    /// @brief The time required to load a container onto the vehicle.
    double myLoadingDuration;

    /// @brief The preferred lateral alignment when using the sublane-model. One of (left, right, center, compact, nice, arbitrary).
    std::string myLatAlignment;

    /// @brief The minimum lateral gap at a speed difference of 100km/h when using the sublane-model
    double myMinGapLat;

    /// @brief The maximum lateral speed when using the sublane-model
    double myMaxSpeedLat;
};

#endif
/****************************************************************************/
