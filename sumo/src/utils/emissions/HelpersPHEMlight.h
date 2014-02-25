/****************************************************************************/
/// @file    HelpersPHEMlight.h
/// @author  Daniel Krajzewicz
/// @date    Sat, 20.04.2013
/// @version $Id$
///
// Helper methods for PHEMlight-based emission computation
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
#ifndef HelpersPHEMlight_h
#define HelpersPHEMlight_h


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
#include "PHEMCEPHandler.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersPHEMlight
 * @brief Helper methods for PHEMlight-based emission computation
 */
class HelpersPHEMlight {
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


};


#endif

/****************************************************************************/

