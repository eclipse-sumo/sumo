/****************************************************************************/
/// @file    MSCFModel_SmartSK.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Peter Wagner
/// @date    Tue, 05 Jun 2012
/// @version $Id$
///
// A smarter SK
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2012-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSCFModel_SmartSK_h
#define MSCFModel_SmartSK_h

// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSCFModel.h"
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class definitions
// ===========================================================================
/** @class MSCFModel_SmartSK
 * @brief The original Krauss (1998) car-following model and parameter
 * @see MSCFModel
 */
class MSCFModel_SmartSK : public MSCFModel {
public:
    /** @brief Constructor
     * @param[in] accel The maximum acceleration
     * @param[in] decel The maximum deceleration
     * @param[in] dawdle The driver imperfection
     * @param[in] tau The driver's reaction time
     */
    MSCFModel_SmartSK(const MSVehicleType* vtype, SUMOReal accel, SUMOReal decel, SUMOReal dawdle, SUMOReal headwayTime,
                      SUMOReal tmp1, SUMOReal tmp2, SUMOReal tmp3, SUMOReal tmp4, SUMOReal tmp5);


    /// @brief Destructor
    ~MSCFModel_SmartSK();


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
    virtual SUMOReal followSpeed(const MSVehicle* const veh, SUMOReal speed, SUMOReal gap2pred, SUMOReal predSpeed, SUMOReal predMaxDecel) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    virtual SUMOReal stopSpeed(const MSVehicle* const veh, const SUMOReal speed, SUMOReal gap2pred) const;


    /** @brief Returns the model's name
     * @return The model's name
     * @see MSCFModel::getModelName
     */
    virtual int getModelID() const {
        return SUMO_TAG_CF_SMART_SK;
    }


    /** @brief Get the driver's imperfection
     * @return The imperfection of drivers of this class
     */
    SUMOReal getImperfection() const {
        return myDawdle;
    }
    /// @}



    /// @name Setter methods
    /// @{
    /** @brief Sets a new value for maximum deceleration [m/s^2]
     * @param[in] accel The new deceleration in m/s^2
     */
    void setMaxDecel(SUMOReal decel) {
        myDecel = decel;
        myTauDecel = myDecel * myHeadwayTime;
    }


    /** @brief Sets a new value for driver imperfection
     * @param[in] accel The new driver imperfection
     */
    void setImperfection(SUMOReal imperfection) {
        myDawdle = imperfection;
    }


    /** @brief Sets a new value for driver reaction time [s]
     * @param[in] headwayTime The new driver reaction time (in s)
     */
    void setHeadwayTime(SUMOReal headwayTime) {
        myHeadwayTime = headwayTime;
        myTauDecel = myDecel * headwayTime;
    }
    /// @}


    /** @brief Duplicates the car-following model
     * @param[in] vtype The vehicle type this model belongs to (1:1)
     * @return A duplicate of this car-following model
     */
    virtual MSCFModel* duplicate(const MSVehicleType* vtype) const;

private:
    /** @brief Returns the "safe" velocity
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The LEADER's speed
     * @return the safe velocity
     */
    virtual SUMOReal _vsafe(const MSVehicle* const veh, SUMOReal gap, SUMOReal predSpeed) const;


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] speed The speed with no dawdling
     * @return The speed after dawdling
     */
    virtual SUMOReal dawdle(SUMOReal speed) const;

    virtual void updateMyHeadway(const MSVehicle* const veh) const {
        // this is the point were the preferred headway changes slowly:
        SSKVehicleVariables* vars = (SSKVehicleVariables*)veh->getCarFollowVariables();
        SUMOReal tTau = vars->myHeadway;
        tTau = tTau + (myHeadwayTime - tTau) * myTmp2 + myTmp3 * tTau * RandHelper::rand(SUMOReal(-1.0), SUMOReal(1.0));
        if (tTau < TS) { // this ensures the SK safety condition
            tTau = TS;
        }
        vars->myHeadway = tTau;
    }

    virtual MSCFModel::VehicleVariables* createVehicleVariables() const {
        SSKVehicleVariables* ret = new SSKVehicleVariables();
        ret->gOld = 0.0;
        ret->myHeadway = myHeadwayTime;
        return ret;
    }

#include <map>

private:
    class SSKVehicleVariables : public MSCFModel::VehicleVariables {
    public:
        SUMOReal gOld, myHeadway;
        std::map<int, SUMOReal> ggOld;
    };

protected:
    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    SUMOReal myDawdle;

    /// @brief The precomputed value for myDecel*myTau
    SUMOReal myTauDecel;

    /// @brief temporary (testing) parameter
    SUMOReal myTmp1, myTmp2, myTmp3, myTmp4, myTmp5;

    /** @brief new variables needed in this model; myS2Sspeed is the speed below which the vehicle does not move when stopped
     * @brief maxDeltaGap is the theoretical maximum change in gap that can happen in one time step
    */
    SUMOReal myS2Sspeed, maxDeltaGap;

};

#endif /* MSCFModel_SmartSK_H */

