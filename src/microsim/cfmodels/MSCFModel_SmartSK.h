/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2012-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
     * @param[in] emergencyDecel The maximum emergency deceleration
     * @param[in] apparentDecel The deceleration as expected by others
     * @param[in] dawdle The driver imperfection
     * @param[in] tau The driver's desired headway
     */
    MSCFModel_SmartSK(const MSVehicleType* vtype, double accel,
                      double decel, double emergencyDecel, double apparentDecel,
                      double dawdle, double headwayTime,
                      double tmp1, double tmp2, double tmp3, double tmp4, double tmp5);


    /// @brief Destructor
    ~MSCFModel_SmartSK();


    /// @name Implementations of the MSCFModel interface
    /// @{

    /** @brief Applies interaction with stops and lane changing model influences
     * @param[in] veh The ego vehicle
     * @param[in] vPos The possible velocity
     * @return The velocity after applying interactions with stops and lane change model influences
     */
    double finalizeSpeed(MSVehicle* const veh, double vPos) const;


    /** @brief Computes the vehicle's safe speed (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] speed The vehicle's speed
     * @param[in] gap2pred The (netto) distance to the LEADER
     * @param[in] predSpeed The speed of LEADER
     * @return EGO's safe speed
     * @see MSCFModel::ffeV
     */
    virtual double followSpeed(const MSVehicle* const veh, double speed, double gap2pred, double predSpeed, double predMaxDecel) const;


    /** @brief Computes the vehicle's safe speed for approaching a non-moving obstacle (no dawdling)
     * @param[in] veh The vehicle (EGO)
     * @param[in] gap2pred The (netto) distance to the the obstacle
     * @return EGO's safe speed for approaching a non-moving obstacle
     * @see MSCFModel::ffeS
     * @todo generic Interface, models can call for the values they need
     */
    virtual double stopSpeed(const MSVehicle* const veh, const double speed, double gap2pred) const;


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
    double getImperfection() const {
        return myDawdle;
    }
    /// @}



    /// @name Setter methods
    /// @{
    /** @brief Sets a new value for maximum deceleration [m/s^2]
     * @param[in] accel The new deceleration in m/s^2
     */
    void setMaxDecel(double decel) {
        myDecel = decel;
        myTauDecel = myDecel * myHeadwayTime;
    }


    /** @brief Sets a new value for driver imperfection
     * @param[in] accel The new driver imperfection
     */
    void setImperfection(double imperfection) {
        myDawdle = imperfection;
    }


    /** @brief Sets a new value for desired headway [s]
     * @param[in] headwayTime The new desired headway (in s)
     */
    void setHeadwayTime(double headwayTime) {
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
    virtual double _vsafe(const MSVehicle* const veh, double gap, double predSpeed) const;


    /** @brief Applies driver imperfection (dawdling / sigma)
     * @param[in] speed The speed with no dawdling
     * @return The speed after dawdling
     */
    virtual double dawdle(double speed) const;

    virtual void updateMyHeadway(const MSVehicle* const veh) const {
        // this is the point were the preferred headway changes slowly:
        SSKVehicleVariables* vars = (SSKVehicleVariables*)veh->getCarFollowVariables();
        double tTau = vars->myHeadway;
        tTau = tTau + (myHeadwayTime - tTau) * myTmp2 + myTmp3 * tTau * RandHelper::rand(double(-1.0), double(1.0));
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
        double gOld, myHeadway;
        std::map<int, double> ggOld;
    };

protected:
    /// @brief The vehicle's dawdle-parameter. 0 for no dawdling, 1 for max.
    double myDawdle;

    /// @brief The precomputed value for myDecel*myTau
    double myTauDecel;

    /// @brief temporary (testing) parameter
    double myTmp1, myTmp2, myTmp3, myTmp4, myTmp5;

    /** @brief new variables needed in this model; myS2Sspeed is the speed below which the vehicle does not move when stopped
     * @brief maxDeltaGap is the theoretical maximum change in gap that can happen in one time step
    */
    double myS2Sspeed, maxDeltaGap;

};

#endif /* MSCFModel_SmartSK_H */

