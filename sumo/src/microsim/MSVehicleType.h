/****************************************************************************/
/// @file    MSVehicleType.h
/// @author  Christian Roessel
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// The car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2012 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSVehicleType_h
#define MSVehicleType_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <map>
#include <string>
#include "MSCFModel.h"
#include <utils/common/SUMOTime.h>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SUMOVTypeParameter.h>
#include <utils/common/RGBColor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class BinaryInputDevice;
class MSCFModel;
class SUMOVTypeParameter;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSVehicleType
 * @brief The car-following model and parameter
 *
 * MSVehicleType stores the parameter of a single vehicle type and methods
 *  that use these for computing the vehicle's car-following behavior
 *
 * It is assumed that within the simulation many vehicles are using the same
 *  vehicle type, quite common is using only one vehicle type for all vehicles.
 *
 * You can think of it like of having a vehicle type for each VW Golf or
 *  Ford Mustang in your simulation while the car instances just refer to it.
 */
class MSVehicleType {
public:
    /** @brief Constructor.
     *
     * @param[in] id The vehicle type's id
     * @param[in] length The length of vehicles that of this type
     * @param[in] minGap The free space in front of the vehicles of this class
     * @param[in] maxSpeed The maximum velocity vehicles of this type may drive with
     * @param[in] prob The probability of this vehicle type
     * @param[in] speedFactor The speed factor to scale maximum speed with
     * @param[in] speedDev The speed deviation
     * @param[in] vclass The class vehicles of this type belong to
     * @param[in] emissionClass The emission class vehicles of this type belong to
     * @param[in] width The width of the vehicles when being drawn
     * @param[in] height The height of the vehicles when being drawn
     * @param[in] shape How vehicles of this class shall be drawn
     * @param[in] osgFile Model file of this class
     * @param[in] lcModel Name of the lane-change model to use
     * @param[in] c Color of this vehicle type
     */
    MSVehicleType(const std::string& id, const SUMOReal lengthWithGap,
                  const SUMOReal minGap, const SUMOReal maxSpeed,
                  const SUMOReal prob, const SUMOReal speedFactor,
                  const SUMOReal speedDev, const SUMOVehicleClass vclass,
                  const SUMOEmissionClass emissionClass,
                  const SUMOReal width, const SUMOReal height,
                  const SUMOVehicleShape shape, const std::string osgFile,
                  const std::string& lcModel,
                  const RGBColor& c) ;


    /// @brief Destructor
    virtual ~MSVehicleType() ;



    /// @name Atomar getter for simulation
    /// @{

    /** @brief Returns the name of the vehicle type
     * @return This type's id
     */
    const std::string& getID() const {
        return myID;
    }


    /** @brief Get vehicle's length [m]
     * @return The length vehicles of this type have in m
     */
    SUMOReal getLength() const {
        return myLength;
    }


    /** @brief Get vehicle's length including the minimum gap [m]
     * @return The length vehicles of this type have (including the minimum gap in m
     */
    SUMOReal getLengthWithGap() const {
        return myLength + myMinGap;
    }


    /** @brief Get the free space in front of vehicles of this class
     * @return The place before the vehicle
     */
    SUMOReal getMinGap() const {
        return myMinGap;
    }


    /** @brief Returns the vehicle type's car following model definition (const version)
     * @return The vehicle type's car following model definition
     */
    inline const MSCFModel& getCarFollowModel() const {
        return *myCarFollowModel;
    }


    /** @brief Returns the vehicle type's car following model definition (non-const version)
     * @return The vehicle type's car following model definition
     */
    inline MSCFModel& getCarFollowModel() {
        return *myCarFollowModel;
    }


    /** @brief Get vehicle's maximum speed [m/s].
     * @return The maximum speed (in m/s) of vehicles of this class
     */
    SUMOReal getMaxSpeed() const {
        return myMaxSpeed;
    }

    /** @brief Get vehicle's maximum speed [m/s].
     * @return The maximum speed (in m/s) of vehicles of this class
     */
    SUMOReal hasSpeedDeviation() const {
        return mySpeedDev != 0.0 || mySpeedFactor != 1.0;
    }


    /** @brief Get vehicle's maximum speed [m/s] drawn from a normal distribution.
     *
     * The speed is calculated relative to the reference speed (which is usually
     *  the maximum allowed speed on a lane or edge).
     * @return The maximum speed (in m/s) of vehicles of this class
     */
    SUMOReal getMaxSpeedWithDeviation(SUMOReal referenceSpeed) const {
        SUMOReal meanSpeed = mySpeedFactor * referenceSpeed;
        SUMOReal speedDev = mySpeedDev * meanSpeed;
        SUMOReal speed = MIN3(RandHelper::randNorm(meanSpeed, speedDev), meanSpeed + 2 * speedDev, myMaxSpeed);
        return MAX3((SUMOReal)0.0, speed, meanSpeed - 2 * speedDev);
    }


    /** @brief Get the default probability of this vehicle type
     * @return The probability to use this type
     */
    SUMOReal getDefaultProbability() const {
        return myDefaultProbability;
    }


    /** @brief Get this vehicle type's vehicle class
     * @return The class of this vehicle type
     * @see SUMOVehicleClass
     */
    SUMOVehicleClass getVehicleClass() const {
        return myVehicleClass;
    }


    /** @brief Get this vehicle type's emission class
     * @return The emission class of this vehicle type
     * @see SUMOEmissionClass
     */
    SUMOEmissionClass getEmissionClass() const {
        return myEmissionClass;
    }


    /** @brief Returns this type's color
     * @return The color of this type
     */
    const RGBColor& getColor() const {
        return myColor;
    }


    /** @brief Returns this type's speed factor
     * @return The speed factor of this type
     */
    SUMOReal getSpeedFactor() const {
        return mySpeedFactor;
    }


    /** @brief Returns this type's speed deviation
     * @return The speed deviation of this type
     */
    SUMOReal getSpeedDeviation() const {
        return mySpeedDev;
    }
    /// @}



    /// @name Atomar getter for visualization
    /// @{

    /** @brief Get the width which vehicles of this class shall have when being drawn
     * @return The width of this type's vehicles
     */
    SUMOReal getWidth() const {
        return myWidth;
    }

    /** @brief Get the height which vehicles of this class shall have when being drawn
     * @return The height of this type's vehicles
     */
    SUMOReal getHeight() const {
        return myHeight;
    }

    /** @brief Get this vehicle type's shape
     * @return The shape of this vehicle type
     * @see SUMOVehicleShape
     */
    SUMOVehicleShape getGuiShape() const {
        return myShape;
    }

    /** @brief Get this vehicle type's 3D model file name
     * @return The model file name of this vehicle type
     */
    std::string getOSGFile() const {
        return myOSGFile;
    }

    /// @}





    /// Saves the states of a vehicle
    void saveState(std::ostream& os);



    /// @name Setter methods
    /// @{

    /** @brief Set a new value for this type's length
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] length The new length of this type
     */
    void setLength(const SUMOReal& length) ;


    /** @brief Set a new value for this type's minimum gap
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] offset The new minimum gap of this type
     */
    void setMinGap(const SUMOReal& minGap) ;


    /** @brief Set a new value for this type's maximum speed
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] maxSpeed The new maximum speed of this type
     */
    void setMaxSpeed(const SUMOReal& maxSpeed) ;


    /** @brief Set a new value for this type's vehicle class
     * @param[in] vclass The new vehicle class of this type
     */
    void setVClass(SUMOVehicleClass vclass) ;


    /** @brief Set a new value for this type's default probability
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] prob The new default probability of this type
     */
    void setDefaultProbability(const SUMOReal& prob) ;


    /** @brief Set a new value for this type's speed factor
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] factor The new speed factor of this type
     */
    void setSpeedFactor(const SUMOReal& factor) ;


    /** @brief Set a new value for this type's speed deviation
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] dev The new speed deviation of this type
     */
    void setSpeedDeviation(const SUMOReal& dev) ;


    /** @brief Set a new value for this type's emission class
     * @param[in] eclass The new emission class of this type
     */
    void setEmissionClass(SUMOEmissionClass eclass) ;


    /** @brief Set a new value for this type's color
     * @param[in] color The new color of this type
     */
    void setColor(const RGBColor& color) ;


    /** @brief Set a new value for this type's width
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] width The new width of this type
     */
    void setWidth(const SUMOReal& width) ;


    /** @brief Set a new value for this type's shape
     * @param[in] shape The new shape of this type
     */
    void setShape(SUMOVehicleShape shape) ;
    /// @}



    /// @name Static methods for building vehicle types
    /// @{

    /** @brief Builds the microsim vehicle type described by the given parameter
     * @param[in] from The vehicle type description
     * @return The built vehicle type
     * @exception ProcessError on false values (not et used)
     */
    static MSVehicleType* build(SUMOVTypeParameter& from) ;


    /** @brief Duplicates the microsim vehicle type giving it a the given id
     * @param[in] id The new id of the type
     * @param[in] from The vehicle type
     * @return The built vehicle type
     */
    static MSVehicleType* build(const std::string& id, const MSVehicleType* from) ;
    /// @}


    /** @brief Returns whether this type belongs to a single vehicle only (was modified)
     * @return Whether this vehicle type is based on a differen one, and belongs to one vehicle only
     */
    bool amVehicleSpecific() const {
        return myOriginalType != 0;
    }


private:
    /// @brief Unique ID
    std::string myID;

    /// @brief Vehicles' length [m]
    SUMOReal myLength;

    /// @brief This class' free space in front of the vehicle itself
    SUMOReal myMinGap;

    /// @brief Vehicles' maximum speed [m/s]
    SUMOReal myMaxSpeed;

    /// @brief The probability when being added to a distribution without an explicit probability
    SUMOReal myDefaultProbability;

    /// @brief The factor by which the maximum speed may deviate from the allowed max speed on the street
    SUMOReal mySpeedFactor;

    /// @brief The standard deviation for speed variations
    SUMOReal mySpeedDev;

    /// @brief ID of the car following model.
    MSCFModel* myCarFollowModel;

    /// @brief ID of the lane change model.
    std::string myLaneChangeModel;

    /// @brief The emission class of such vehicles
    SUMOEmissionClass myEmissionClass;

    /// @brief The color
    RGBColor myColor;

    /// @brief The vehicles' class
    SUMOVehicleClass myVehicleClass;


    /// @name Values for drawing this class' vehicles
    /// @{

    /// @brief This class' width
    SUMOReal myWidth;

    /// @brief This class' height
    SUMOReal myHeight;

    /// @brief This class' shape
    SUMOVehicleShape myShape;

    /// @brief This class' model file
    std::string myOSGFile;
    /// @}


    /// @brief The original type
    const MSVehicleType* myOriginalType;


private:
    /// @brief Invalidated copy constructor
    MSVehicleType(const MSVehicleType&);

    /// @brief Invalidated assignment operator
    MSVehicleType& operator=(const MSVehicleType&);

};


#endif

/****************************************************************************/

