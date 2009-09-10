/****************************************************************************/
/// @file    MSVehicleType.h
/// @author  Christian Roessel
/// @date    Mon, 12 Mar 2001
/// @version $Id$
///
// The car-following model and parameter
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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
#include <utils/common/RGBColor.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class BinaryInputDevice;
class MSCFModel;

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
     * @param[in] length The length of vehicles that are of this type
     * @param[in] maxSpeed The maximum velocity vehicles of this type may drive with
     * @param[in] accel The maximum acceleration of vehicles of this type
     * @param[in] decel The maximum deceleration of vehicles of this type
     * @param[in] dawdle The driver imperfection used by this vehicle type
     * @param[in] tau The driver's reaction time used by this vehicle type
     * @param[in] vclass The class vehicles of this type belong to
     * @param[in] emissionClass The emission class vehicles of this type belong to
     * @param[in] shape How vehicles of this class shall be drawn
     * @param[in] guiWidth The width of the vehicles when being drawn
     * @param[in] guiOffset The free space in front of the vehicles of this class
     * @param[in] cfModel Name of the car-following model to use
     * @param[in] lcModel Name of the lane-change model to use
     * @param[in] c Color of this vehicle type
     */
    MSVehicleType(const std::string &id, SUMOReal length, SUMOReal maxSpeed,
                  SUMOReal accel, SUMOReal decel, SUMOReal dawdle,
                  SUMOReal tau, SUMOReal prob, SUMOReal speedFactor,
                  SUMOReal speedDev, SUMOVehicleClass vclass,
                  SUMOEmissionClass emissionClass, SUMOVehicleShape shape,
                  SUMOReal guiWidth, SUMOReal guiOffset,
                  const std::string &cfModel, const std::string &lcModel,
                  const RGBColor &c) throw();


    /// @brief Destructor
    virtual ~MSVehicleType() throw();



    /// @name Atomar getter for simulation
    /// @{

    /** @brief Returns the name of the vehicle type
     * @return This type's id
     */
    const std::string &getID() const throw() {
        return myID;
    }


    /** @brief Get vehicle's length [m]
     * @return The length vehicles of this type have in m
     */
    SUMOReal getLength() const throw() {
        return myLength;
    }


    /** @brief Returns the vehicle type's car following model definition
     * @return The vehicle type's car following model definition
     */
    inline const MSCFModel &getCarFollowModel() const throw() {
        return *myCarFollowModel;
    }


    /** @brief Get vehicle's maximum speed [m/s].
     * @return The maximum speed (in m/s) of vehicles of this class
     */
    SUMOReal getMaxSpeed() const throw() {
        return myMaxSpeed;
    }

    SUMOReal getMaxAccel() const throw() {
        return myAccel;
    }


    /** @brief Get vehicle's maximum speed [m/s].
     * @return The maximum speed (in m/s) of vehicles of this class
     */
    SUMOReal hasSpeedDeviation() const throw() {
        return mySpeedDev != 0.0 || mySpeedFactor != 1.0;
    }


    /** @brief Get vehicle's maximum speed [m/s] drawn from a normal distribution.
     *
     * The speed is calculated relative to the reference speed (which is usually
     *  the maximum allowed speed on a lane or edge).
     * @return The maximum speed (in m/s) of vehicles of this class
     */
    SUMOReal getMaxSpeedWithDeviation(SUMOReal referenceSpeed) const throw() {
        SUMOReal meanSpeed = mySpeedFactor * referenceSpeed;
        SUMOReal speedDev = mySpeedDev * meanSpeed;
        SUMOReal speed = MIN3(RandHelper::randNorm(meanSpeed, speedDev), meanSpeed + 2*speedDev, myMaxSpeed);
        return MAX3((SUMOReal)0.0, speed, meanSpeed - 2*speedDev);
    }


    /** @brief Get the vehicle's maximum deceleration [m/s^2]
     * @return The maximum deceleration (in m/s^2) of vehicles of this class
     */
    SUMOReal getMaxDecel() const throw() {
        return myDecel;
    }


    /** @brief Get the driver's reaction time [s]
     * @return The reaction time of this class' drivers in s
     */
    SUMOReal getTau() const throw() {
        return myTau;
    }


    /** @brief Get the default probability of this vehicle type
     * @return The probability to use this type
     */
    SUMOReal getDefaultProbability() const throw() {
        return myDefaultProbability;
    }


    /** @brief Get this vehicle type's vehicle class
     * @return The class of this vehicle type
     * @see SUMOVehicleClass
     */
    SUMOVehicleClass getVehicleClass() const throw() {
        return myVehicleClass;
    }


    /** @brief Get this vehicle type's emission class
     * @return The emission class of this vehicle type
     * @see SUMOEmissionClass
     */
    SUMOEmissionClass getEmissionClass() const throw() {
        return myEmissionClass;
    }


    /** @brief Returns this type's color
     * @return The color of this type
     */
    const RGBColor &getColor() const throw() {
        return myColor;
    }
    /// @}



    /// @name Atomar getter for visualization
    /// @{

    /** @brief Get this vehicle type's shape
     * @return The shape of this vehicle type
     * @see SUMOVehicleShape
     */
    SUMOVehicleShape getGuiShape() const throw() {
        return myShape;
    }


    /** @brief Get the width which vehicles of this class shall have when being drawn
     * @return The width of this type's vehicles
     */
    SUMOReal getGuiWidth() const throw() {
        return myWidth;
    }


    /** @brief Get the free space (not drawn) in front of vehicles of this class
     * @return The place before the vehicle
     */
    SUMOReal getGuiOffset() const throw() {
        return myOffset;
    }
    /// @}


    SUMOReal getSpeedAfterMaxDecel(SUMOReal v) const {
        return MAX2((SUMOReal) 0, v - (SUMOReal) ACCEL2SPEED(myDecel));
    }




    /// Get the vehicle's maximum acceleration [m/s^2]
    inline SUMOReal getMaxAccel(SUMOReal v) const {
        return (SUMOReal)(myAccel *(1.0 - (v/myMaxSpeed)));
    }

    /// Saves the states of a vehicle
    void saveState(std::ostream &os);


protected:

    /** Returns the SK-vsafe. */
    SUMOReal _vsafe(SUMOReal gap2pred, SUMOReal predSpeed) const {
        if (predSpeed==0&&gap2pred<0.01) {
            return 0;
        }
        assert(gap2pred  >= SUMOReal(0));
        assert(predSpeed >= SUMOReal(0));
        SUMOReal vsafe = (SUMOReal)(-1. * myTauDecel
                                    + sqrt(
                                        myTauDecel*myTauDecel
                                        + (predSpeed*predSpeed)
                                        + (2. * myDecel * gap2pred)
                                    ));
        assert(vsafe >= 0);
        return vsafe;
    }




private:
    /// @brief Unique ID.
    std::string myID;

    /// @brief Vehicle's length [m].
    SUMOReal myLength;

    /// @brief Vehicle's maximum speed [m/s].
    SUMOReal myMaxSpeed;

    /// @brief The vehicle's maximum acceleration [m/s^2]
    SUMOReal myAccel;

    /// @brief The vehicle's maximum deceleration [m/s^2]
    SUMOReal myDecel;

    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// @brief The driver's reaction time [s]
    SUMOReal myTau;

    /// @brief The vehicle's class
    SUMOVehicleClass myVehicleClass;

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

    /// @brief The emission class of this vehicle
    SUMOEmissionClass myEmissionClass;

    /// @brief The color
    RGBColor myColor;


    /// @name Values for drawing this class' vehicles
    /// @{

    /// @brief This class' width
    SUMOReal myWidth;

    /// @brief This class' free space in front of the vehicle itself
    SUMOReal myOffset;

    /// @brief This class' shape
    SUMOVehicleShape myShape;
    /// @}



    /// @name some precomputed values for faster computation
    /// @{

    /// The precomputed value for 1/(2*d)
    SUMOReal myInverseTwoDecel;

    /// The precomputed value for myDecel*myTau
    SUMOReal myTauDecel;
    /// @}


private:
    /// Helper function to bulid CFModel
    void consCfModel(std::string cfModel);

    /// Invalidated copy constructor
    MSVehicleType(const MSVehicleType&);

    /// Invalidated assignment operator
    MSVehicleType& operator=(const MSVehicleType&);

};


#endif

/****************************************************************************/

