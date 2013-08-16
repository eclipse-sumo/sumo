/****************************************************************************/
/// @file    MSCFModel_IDM.h
/// @author  Tobias Mayer
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Thu, 03 Sep 2009
/// @version $Id$
///
// The Intelligent Driver Model (IDM) car-following model
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCFMODEL_IDM_H
#define	MSCFMODEL_IDM_H

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"
#include <microsim/MSLane.h>
#include <microsim/MSVehicle.h>
#include <microsim/MSVehicleType.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_IDM
 * @brief The Intelligent Driver Model (IDM) car-following model
 * @see MSCFModel
 */
class MSCFModel_IDM : public MSCFModel {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] headwayTime the headway gap
     * @param[in] delta a model constant
     * @param[in] internalStepping internal time step size
     */
    MSCFModel_IDM(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                  SUMOReal headwayTime, SUMOReal delta, SUMOReal internalStepping);


    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] headwayTime the headway gap
     * @param[in] adaptationFactor a model constant
     * @param[in] adaptationTime a model constant
     * @param[in] internalStepping internal time step size
     */
    MSCFModel_IDM(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel,
                  SUMOReal headwayTime, SUMOReal adaptationFactor, SUMOReal adaptationTime,
                  SUMOReal internalStepping);


    /// @brief Destructor
    ~MSCFModel_IDM();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    SUMOReal moveHelper(MSVehicle* const veh, SUMOReal vPos) const;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     */
    SUMOReal followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    SUMOReal stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap2pred) const;


    /** @brief Returns the maximum gap at which an interaction between both vehicles occurs
     *
     * "interaction" means that the LEADER influences EGO's speed.
     * @param[in] veh The EGO vehicle
     * @param[in] vL LEADER's speed
     * @return The interaction gap
     * @todo evaluate signature
     * @see MSCFModel::interactionGap
     */
    SUMOReal interactionGap(const MSVehicle* const , SUMOReal vL) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    int getModelID() const {
        return myExpFactor > 0. ? SUMO_TAG_CF_IDMM : SUMO_TAG_CF_IDM;
    }
    /// @}



    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    MSCFModel* duplicate(const MSVehicleType* vtype) const;


    VehicleVariables* createVehicleVariables() const {
        if (myExpFactor > 0.) {
            return new VehicleVariables();
        }
        return 0;
    }


private:
    class VehicleVariables : public MSCFModel::VehicleVariables {
    public:
        VehicleVariables() : levelOfService(1.) {}
        /// @brief state variable for remembering speed deviation history (lambda)
        SUMOReal levelOfService;
    };


private:
    SUMOReal _v(const MSVehicle* const veh, SUMOReal gap2pred, SUMOReal mySpeed, SUMOReal predSpeed, SUMOReal desSpeed) const;

    SUMOReal desiredSpeed(const MSVehicle* const veh) const {
        return MIN2(myType->getMaxSpeed(), veh->getLane()->getVehicleMaxSpeed(veh));
    }


private:
    /// @brief The IDM delta exponent
    const SUMOReal myDelta;

    /// @brief The IDMM adaptation factor beta
    const SUMOReal myAdaptationFactor;

    /// @brief The IDMM adaptation time tau
    const SUMOReal myAdaptationTime;

    /// @brief A computational shortcut for IDMM
    const SUMOReal myExpFactor;

    /// @brief The number of iterations in speed calculations
    const int myIterations;

    /// @brief A computational shortcut
    const SUMOReal myTwoSqrtAccelDecel;

private:
    /// @brief Invalidated assignment operator
    MSCFModel_IDM& operator=(const MSCFModel_IDM& s);
};

#endif	/* MSCFMODEL_IDM_H */
