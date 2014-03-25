/****************************************************************************/
/// @file    HelpersHBEFA3.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for HBEFA3-based emission computation
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
#ifndef HelpersHBEFA3_h
#define HelpersHBEFA3_h


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
 * @class HelpersHBEFA3
 * @brief Helper methods for HBEFA3-based emission computation
 *
 * The parameter are stored per vehicle class; 6*6 parameter are used, sorted by
 *  the pollutant (CO2, CO, HC, fuel, NOx, PMx), and the function part
 *  (c0, cav1, cav2, c1, c2, c3).
 */
class HelpersHBEFA3 : public PollutantsInterface::Helper {
public:


    static const int HBEFA3_BASE = 1 << 16;


    /** @brief Constructor (initializes myEmissionClassStrings)
     */
    HelpersHBEFA3();


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
    inline SUMOReal compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope) const {
        if (a < 0.) {
            return 0.;
        }
        const int index = (c & ~PollutantsInterface::HEAVY_BIT) - HBEFA3_BASE - 2;
        const SUMOReal kmh = v * 3.6;
        const SUMOReal scale = (e == PollutantsInterface::FUEL) ? 3.6 * 790. : 3.6;
        const double* f = myFunctionParameter[c][e];
        const double alpha = asin(a / 9.81) * 180. / M_PI;
        return (SUMOReal) MAX2((f[0] + f[1] * alpha * kmh + f[2] * alpha * alpha * kmh + f[3] * kmh + f[4] * kmh * kmh + f[5] * kmh * kmh * kmh) / scale, 0.);
    }


private:
    /// @brief The function parameter
    static double myFunctionParameter[45][6][6];

};


#endif

/****************************************************************************/

