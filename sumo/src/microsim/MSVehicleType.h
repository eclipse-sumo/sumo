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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <microsim/cfmodels/MSCFModel.h>
#include <utils/common/SUMOTime.h>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/RandHelper.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/common/RGBColor.h>
#include <foreign/mersenne/MersenneTwister.h>


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
     * @param[in] parameter The vehicle type's parameter
     */
    MSVehicleType(const SUMOVTypeParameter& parameter);


    /// @brief Destructor
    virtual ~MSVehicleType();


    /** @brief Returns whether the given parameter was set
     * @param[in] what The parameter which one asks for
     * @return Whether the given parameter was set
     */
    bool wasSet(int what) const {
        return (myParameter.setParameter & what) != 0;
    }


    /// @name Atomar getter for simulation
    /// @{

    /** @brief Returns the name of the vehicle type
     * @return This type's id
     */
    const std::string& getID() const {
        return myParameter.id;
    }


    /** @brief Returns the running index of the vehicle type
     * @return This type's numerical id
     */
    int getNumericalID() const {
        return myIndex;
    }


    /** @brief Get vehicle's length [m]
     * @return The length vehicles of this type have in m
     */
    SUMOReal getLength() const {
        return myParameter.length;
    }


    /** @brief Get vehicle's length including the minimum gap [m]
     * @return The length vehicles of this type have (including the minimum gap in m
     */
    SUMOReal getLengthWithGap() const {
        return myParameter.length + myParameter.minGap;
    }


    /** @brief Get the free space in front of vehicles of this class
     * @return The place before the vehicle
     */
    SUMOReal getMinGap() const {
        return myParameter.minGap;
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


    inline LaneChangeModel getLaneChangeModel() const {
        return myParameter.lcModel;
    }


    /** @brief Get vehicle's maximum speed [m/s].
     * @return The maximum speed (in m/s) of vehicles of this class
     */
    SUMOReal getMaxSpeed() const {
        return myParameter.maxSpeed;
    }


    /** @brief Computes and returns the speed deviation
     * @return A new, random speed deviation
     */
    SUMOReal computeChosenSpeedDeviation(MTRand* rng, const SUMOReal minDevFactor = 0.2) const;


    /** @brief Get the default probability of this vehicle type
     * @return The probability to use this type
     */
    SUMOReal getDefaultProbability() const {
        return myParameter.defaultProbability;
    }


    /** @brief Get this vehicle type's vehicle class
     * @return The class of this vehicle type
     * @see SUMOVehicleClass
     */
    SUMOVehicleClass getVehicleClass() const {
        return myParameter.vehicleClass;
    }


    /** @brief Get this vehicle type's emission class
     * @return The emission class of this vehicle type
     * @see SUMOEmissionClass
     */
    SUMOEmissionClass getEmissionClass() const {
        return myParameter.emissionClass;
    }


    /** @brief Returns this type's color
     * @return The color of this type
     */
    const RGBColor& getColor() const {
        return myParameter.color;
    }


    /** @brief Returns this type's speed factor
     * @return The speed factor of this type
     */
    SUMOReal getSpeedFactor() const {
        return myParameter.speedFactor;
    }


    /** @brief Returns this type's speed deviation
     * @return The speed deviation of this type
     */
    SUMOReal getSpeedDeviation() const {
        return myParameter.speedDev;
    }


    /** @brief Returns this type's impatience
     * @return The impatience of this type
     */
    SUMOReal getImpatience() const {
        return myParameter.impatience;
    }
    /// @}



    /// @name Atomar getter for visualization
    /// @{

    /** @brief Get the width which vehicles of this class shall have when being drawn
     * @return The width of this type's vehicles
     */
    SUMOReal getWidth() const {
        return myParameter.width;
    }

    /** @brief Get the height which vehicles of this class shall have when being drawn
     * @return The height of this type's vehicles
     */
    SUMOReal getHeight() const {
        return myParameter.height;
    }

    /** @brief Get this vehicle type's shape
     * @return The shape of this vehicle type
     * @see SUMOVehicleShape
     */
    SUMOVehicleShape getGuiShape() const {
        return myParameter.shape;
    }

    /** @brief Get this vehicle type's 3D model file name
     * @return The model file name of this vehicle type
     */
    std::string getOSGFile() const {
        return myParameter.osgFile;
    }


    /** @brief Get this vehicle type's raster model file name
     * @return The raster file name of this vehicle type
     */
    std::string getImgFile() const {
        return myParameter.imgFile;
    }

    /// @}


    /// @name Setter methods
    /// @{

    /** @brief Set a new value for this type's length
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] length The new length of this type
     */
    void setLength(const SUMOReal& length);


    /** @brief Set a new value for this type's minimum gap
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] minGap The new minimum gap of this type
     */
    void setMinGap(const SUMOReal& minGap);


    /** @brief Set a new value for this type's maximum speed
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] maxSpeed The new maximum speed of this type
     */
    void setMaxSpeed(const SUMOReal& maxSpeed);


    /** @brief Set a new value for this type's vehicle class
     * @param[in] vclass The new vehicle class of this type
     */
    void setVClass(SUMOVehicleClass vclass);


    /** @brief Set a new value for this type's default probability
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] prob The new default probability of this type
     */
    void setDefaultProbability(const SUMOReal& prob);


    /** @brief Set a new value for this type's speed factor
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] factor The new speed factor of this type
     */
    void setSpeedFactor(const SUMOReal& factor);


    /** @brief Set a new value for this type's speed deviation
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] dev The new speed deviation of this type
     */
    void setSpeedDeviation(const SUMOReal& dev);


    /** @brief Set a new value for this type's emission class
     * @param[in] eclass The new emission class of this type
     */
    void setEmissionClass(SUMOEmissionClass eclass);


    /** @brief Set a new value for this type's color
     * @param[in] color The new color of this type
     */
    void setColor(const RGBColor& color);


    /** @brief Set a new value for this type's width
     *
     * If the given value<0 then the one from the original type will
     *  be used.
     *
     * @param[in] width The new width of this type
     */
    void setWidth(const SUMOReal& width);


    /** @brief Set a new value for this type's shape
     * @param[in] shape The new shape of this type
     */
    void setShape(SUMOVehicleShape shape);


    /** @brief Set a new value for this type's impatience
     * @param[in] impatience The new impatience of this type
     */
    void setImpatience(const SUMOReal impatience);
    /// @}



    /// @name Static methods for building vehicle types
    /// @{

    /** @brief Builds the microsim vehicle type described by the given parameter
     * @param[in] from The vehicle type description
     * @return The built vehicle type
     * @exception ProcessError on false values (not et used)
     */
    static MSVehicleType* build(SUMOVTypeParameter& from);


    /** @brief Duplicates the microsim vehicle type giving it a the given id
     * @param[in] id The new id of the type
     * @param[in] from The vehicle type
     * @return The built vehicle type
     */
    static MSVehicleType* build(const std::string& id, const MSVehicleType* from);
    /// @}


    /** @brief Returns whether this type belongs to a single vehicle only (was modified)
     * @return Whether this vehicle type is based on a differen one, and belongs to one vehicle only
     */
    bool amVehicleSpecific() const {
        return myOriginalType != 0;
    }


    const SUMOVTypeParameter& getParameter() const {
        return myParameter;
    }


private:
    /// @brief the parameter container
    SUMOVTypeParameter myParameter;

    /// @brief the running index
    const int myIndex;

    /// @brief ID of the car following model.
    MSCFModel* myCarFollowModel;

    /// @brief The original type
    const MSVehicleType* myOriginalType;

    /// @brief next value for the running index
    static int myNextIndex;


private:
    /// @brief Invalidated copy constructor
    MSVehicleType(const MSVehicleType&);

    /// @brief Invalidated assignment operator
    MSVehicleType& operator=(const MSVehicleType&);

};


#endif

/****************************************************************************/

