/****************************************************************************/
/// @file    HelpersHBEFA.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for HBEFA-based emission computation
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
#include "PollutantsInterface.h"


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
class HelpersHBEFA : public PollutantsInterface::Helper {
public:
    /** @brief Constructor (initializes myEmissionClassStrings)
     */
    HelpersHBEFA();


    /** @brief Computes the emitted pollutant amount using the given speed and acceleration
     *
     * As the functions are defining emissions in g/hour, the function's result is normed
     *  by 3.6 (seconds in an hour/1000) yielding mg/s. For fuel ml/s is returned.
     *  Negative acceleration results directly in zero emission.
     *
     * @param[in] c emission class for the function parameters to use
     * @param[in] e the type of emission (CO, CO2, ...)
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    inline SUMOReal compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope) const {
        UNUSED_PARAMETER(slope);
        if (c == PollutantsInterface::ZERO_EMISSIONS) {
            return 0.;
        }
        const int index = (c & ~PollutantsInterface::HEAVY_BIT) - 1;
        const SUMOReal kmh = v * 3.6;
        const SUMOReal scale = (e == PollutantsInterface::FUEL) ? 3.6 * 790. : 3.6;
        if (index >= 42) {
            const double* f = myFunctionParameter[index - 42] + 6 * e;
            return (SUMOReal) MAX2((f[0] + f[3] * kmh + f[4] * kmh * kmh + f[5] * kmh * kmh * kmh) / scale, 0.);
        }
        if (a < 0.) {
            return 0.;
        }
        const double* f = myFunctionParameter[index] + 6 * e;
        const double alpha = asin(a / 9.81) * 180. / M_PI;
        return (SUMOReal) MAX2((f[0] + f[1] * alpha * kmh + f[2] * alpha * alpha * kmh + f[3] * kmh + f[4] * kmh * kmh + f[5] * kmh * kmh * kmh) / scale, 0.);
    }


private:
    /// @brief The function parameter
    static double myFunctionParameter[42][36];

};


#endif

/****************************************************************************/

