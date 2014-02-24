/****************************************************************************/
/// @file    HelpersHBEFA.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for HBEFA-based emission computation
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
#ifndef HelpersHBEFA_h
#define HelpersHBEFA_h


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
#include <utils/geom/GeomHelper.h>
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersHBEFA
 * @brief Helper methods for HBEFA-based emission computation
 *
 * The parameter are stored per vehicle class; 6*6 parameter are used, sorted by
 *  the pollutant (CO2, CO, HC, fuel, NOx, PMx), and the function part
 *  (c0, cav1, cav2, c1, c2, c3).
 */
class HelpersHBEFA {
public:
    /** @brief Returns the amount of emitted CO given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of CO emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeCO(SUMOEmissionClass c, double v, double a);


    /** @brief Returns the amount of emitted CO2 given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of CO2 emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeCO2(SUMOEmissionClass c, double v, double a);


    /** @brief Returns the amount of emitted HC given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of HC emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeHC(SUMOEmissionClass c, double v, double a);


    /** @brief Returns the amount of emitted NOx given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of NOx emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computeNOx(SUMOEmissionClass c, double v, double a);


    /** @brief Returns the amount of emitted PMx given the vehicle type and state (in mg/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of PMx emitted by the given vehicle class when moving with the given velocity and acceleration [mg/s]
     */
    static SUMOReal computePMx(SUMOEmissionClass c, double v, double a);


    /** @brief Returns the amount of consumed fuel given the vehicle type and state (in ml/s)
     *
     * As the general function returns mg/s, this implementation scales with 790 (average density of fuel)
     *
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of fuel consumed by the given vehicle class when moving with the given velocity and acceleration [ml/s]
     */
    static SUMOReal computeFuel(SUMOEmissionClass c, double v, double a);


private:
    /** @brief Computes the emitted pollutant amount using the given speed and acceleration
     *
     * As the functions are defining emissions in g/hour, the function's result is normed
     *  by 3.6 (seconds in an hour/1000) yielding mg/s. Negative acceleration
     *  results directly in zero emission.
     *
     * @param[in] c emission class for the function parameters to use
     * @param[in] offset the offset in the function parameters for the correct pollutant
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     */
    static inline SUMOReal compute(SUMOEmissionClass c, const int offset, double v, const double a) {
        switch (c) {
            case SVE_ZERO_EMISSIONS:
                return 0.;
            case SVE_UNKNOWN:
                c = SVE_P_LDV_7_7;
                break;
            default:
                break;
        }
        v *= 3.6;
        if (c > 42) {
            const double* f = myFunctionParameter[c - 42] + offset;
            return (SUMOReal) MAX2((f[0] + f[3] * v + f[4] * v * v + f[5] * v * v * v) / 3.6, 0.);
        }
        if (a < 0.) {
            return 0.;
        }
        const double* f = myFunctionParameter[c] + offset;
        const double alpha = asin(a / 9.81) * 180. / M_PI;
        return (SUMOReal) MAX2((f[0] + f[1] * alpha * v + f[2] * alpha * alpha * v + f[3] * v + f[4] * v * v + f[5] * v * v * v) / 3.6, 0.);
    }


private:
    /// @brief The function parameter
    static double myFunctionParameter[42][36];

};


#endif

/****************************************************************************/

