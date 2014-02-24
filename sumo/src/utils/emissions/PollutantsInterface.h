/****************************************************************************/
/// @file    PollutantsInterface.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 19.08.2013
/// @version $Id: PollutantsInterface.h 13107 2012-12-02 13:57:34Z behrisch $
///
// Interface to capsulate different emission models
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
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
#ifndef PollutantsInterface_h
#define PollutantsInterface_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <limits>
#include <cmath>
#include <utils/common/StdDefs.h>
#include <utils/common/SUMOVehicleClass.h>
#include "PHEMCEP.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class PollutantsInterface
 * @brief Helper methods for PHEMlight-based emission computation
 */
class PollutantsInterface {
public:
    /** @brief Returns the maximum possible acceleration
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The maximum possible acceleration
     */
    static SUMOReal getMaxAccel(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted CO given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of CO emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeCO(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted CO2 given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of CO2 emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeCO2(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted HC given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of HC emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeHC(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted NOx given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of NOx emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeNOx(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted PMx given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of PMx emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computePMx(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of consumed fuel given the vehicle type and state (in ml/s)
     *
     * As the general function returns mg/s, this implementation scales with 790 (average density of fuel)
     *
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of fuel consumed by the given vehicle class when moving with the given velocity and acceleration [ml/s]
     */
    static SUMOReal computeFuel(SUMOEmissionClass c, double v, double a, double slope);




    /** @brief Returns the amount of emitted CO given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of CO emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultCO(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted CO2 given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of CO2 emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultCO2(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted HC given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of HC emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultHC(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted NOx given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of NOx emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultNOx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of emitted PMx given the vehicle type and default values for the state (in mg)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of PMx emitted by the given vehicle class [mg]
     */
    static SUMOReal computeDefaultPMx(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


    /** @brief Returns the amount of fuel given the vehicle type and default values for the state (in ml)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's average velocity
     * @param[in] a The vehicle's average acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @param{in] tt the time the vehicle travels
     * @return The amount of fuel emitted by the given vehicle class [ml]
     */
    static SUMOReal computeDefaultFuel(SUMOEmissionClass c, double v, double a, double slope, SUMOReal tt);


};


#endif

/****************************************************************************/

