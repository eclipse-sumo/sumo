/****************************************************************************/
/// @file    HelpersHBEFA.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: HelpersHBEFA.h 7457 2009-07-18 23:25:57 +0200 (Sa, 18 Jul 2009) behrisch $
///
// Helper methods for HBEFA-based emission computation
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
#include <cassert>
#include "StdDefs.h"
#include "SUMOVehicleClass.h"
#include <limits>
#include <cmath>


// ===========================================================================
// definitions
// ===========================================================================
#ifndef PI
#define PI 3.1415926535897932384626433832795
#endif


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
    /** @brief Returns the amount of emitted CO given the vehicle type and state (in g/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of CO emitted by the given vehicle class when moving with the given velocity and acceleration [g/s]
     */
    static SUMOReal computeCO(SUMOEmissionClass c, double v, double a) throw();


    /** @brief Returns the amount of emitted CO2 given the vehicle type and state (in g/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of CO2 emitted by the given vehicle class when moving with the given velocity and acceleration [g/s]
     */
    static SUMOReal computeCO2(SUMOEmissionClass c, double v, double a) throw();


    /** @brief Returns the amount of emitted HC given the vehicle type and state (in g/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of HC emitted by the given vehicle class when moving with the given velocity and acceleration [g/s]
     */
    static SUMOReal computeHC(SUMOEmissionClass c, double v, double a) throw();


    /** @brief Returns the amount of emitted NOx given the vehicle type and state (in g/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of NOx emitted by the given vehicle class when moving with the given velocity and acceleration [g/s]
     */
    static SUMOReal computeNOx(SUMOEmissionClass c, double v, double a) throw();


    /** @brief Returns the amount of emitted PMx given the vehicle type and state (in g/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of PMx emitted by the given vehicle class when moving with the given velocity and acceleration [g/s]
     */
    static SUMOReal computePMx(SUMOEmissionClass c, double v, double a) throw();


    /** @brief Returns the amount of consumed fuel given the vehicle type and state (in l/s)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The amount of fuel consumed by the given vehicle class when moving with the given velocity and acceleration [l/s]
     */
    static SUMOReal computeFuel(SUMOEmissionClass c, double v, double a) throw();


private:
    /** @brief Returns the parameter for the given vehicle emission class
     * @param[in] c The vehicle emission class
     * @return The function parameter (for all pollutants)
     */
    static inline double *getParameterForClass(SUMOEmissionClass c) throw() {
        return myFunctionParameter[c];
    }


    /** @brief Computes the emitted pollutant amount using the given values
     *
     * As the functions are defining emissions/hour, the function's result is normed
     *  by 3600 (seconds in an hour) yielding in <measure>/s.
     *
     * @param[in] f Pointer to the function parameters to use
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     */
    static inline double computeUsing(double *f, double v, double a) throw() {
        if (a<0) {
            return 0.;
        }
        v = v * 3.6;
        double alpha = asin(a/9.81)*180./PI;
        return MAX2(((f[0] + f[1]*alpha*v + f[2]*alpha*alpha*v + f[3]*v + f[4]*v*v + f[5]*v*v*v) / 3600.), 0.);
    }


private:
    /// @brief The function parameter
    static double myFunctionParameter[42][36];

};


#endif

/****************************************************************************/

