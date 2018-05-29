/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    GNECalibratorVehicleType.h
/// @author  Pablo Alvarez Lopez
/// @date    Nov 2015
/// @version $Id$
///
// VehicleType used by GNECalibrators
/****************************************************************************/
#ifndef GNECalibratorVehicleType_h
#define GNECalibratorVehicleType_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/UtilExceptions.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/RGBColor.h>

#include <netedit/GNEAttributeCarrier.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNECalibrator;
class GNECalibratorDialog;

// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GNECalibratorVehicleType
 * vehicleType vehicleType used by GNECalibrators
 */
class GNECalibratorVehicleType : public GNEAttributeCarrier {

public:
    /// @brief constructor (Used only in GNECalibratorDialog)
    GNECalibratorVehicleType(GNENet* net, const std::string& id="");

    /// @brief parameter constructor
    GNECalibratorVehicleType(GNENet* net, std::string vehicleTypeID,
                             double accel, double decel, double sigma, double tau, double length, double minGap,
                             double maxSpeed, double speedFactor, double speedDev, const RGBColor& color,
                             SUMOVehicleClass vClass, const std::string& emissionClass, SUMOVehicleShape shape,
                             double width, const std::string& filename, double impatience, const std::string& laneChangeModel,
                             const std::string& carFollowModel, int personCapacity, int containerCapacity, double boardingDuration,
                             double loadingDuration, const std::string& latAlignment, double minGapLat, double maxSpeedLat);

    /// @brief destructor
    ~GNECalibratorVehicleType();

    /// @brief write Flow values into a XML
    void writeVehicleType(OutputDevice& device);

    /// @brief get pointer to calibrator parent
    GNENet* getNet() const;

    /// @brief inherited from GNEAttributeCarrier
    /// @{
    /// @brief select attribute carrier
    void selectAttributeCarrier(bool);

    /// @brief unselect attribute carrier
    void unselectAttributeCarrier(bool);

    /// @brief check if attribute carrier is selected
    bool isAttributeCarrierSelected() const;

    /* @brief method for getting the Attribute of an XML key
    * @param[in] key The attribute key
    * @return string with the value associated to key
    */
    std::string getAttribute(SumoXMLAttr key) const;

    /* @brief method for setting the attribute and letting the object perform additional changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    * @param[in] net optionally the GNENet to inform about gui updates
    */
    void setAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList);

    /* @brief method for setting the attribute and letting the object perform additional changes
    * @param[in] key The attribute key
    * @param[in] value The new value
    * @param[in] undoList The undoList on which to register changes
    */
    bool isValid(SumoXMLAttr key, const std::string& value);
    /// @}

private:
    /// @brief pointer to the network
    GNENet* myNet;

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
    RGBColor myColor;

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

private:
    /// @brief method for setting the attribute and nothing else
    void setAttribute(SumoXMLAttr key, const std::string& value);

    /// @brief Invalidated copy constructor.
    GNECalibratorVehicleType(GNECalibratorVehicleType*) = delete;

    /// @brief Invalidated assignment operator
    GNECalibratorVehicleType* operator=(GNECalibratorVehicleType*) = delete;
};

#endif
/****************************************************************************/
