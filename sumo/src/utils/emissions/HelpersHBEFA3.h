/****************************************************************************/
/// @file    HelpersHBEFA3.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for HBEFA3-based emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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


    /** @brief Returns the emission class described by the given parameters.
     * @param[in] base the base class giving the default
     * @param[in] vClass the vehicle class as described in the Amitran interface (Passenger, ...)
     * @param[in] fuel the fuel type as described in the Amitran interface (Gasoline, Diesel, ...)
     * @param[in] eClass the emission class as described in the Amitran interface (Euro0, ...)
     * @param[in] weight the vehicle weight in kg as described in the Amitran interface
     * @return the class described by the parameters
     */
    SUMOEmissionClass getClass(const SUMOEmissionClass base, const std::string& vClass, const std::string& fuel, const std::string& eClass, const double weight) const;

    /** @brief Returns the vehicle class described by this emission class as described in the Amitran interface (Passenger, ...)
     * @param[in] c the emission class
     * @return the name of the vehicle class
     */
    std::string getAmitranVehicleClass(const SUMOEmissionClass c) const;

    /** @brief Returns the fuel type described by this emission class as described in the Amitran interface (Gasoline, Diesel, ...)
     * @param[in] c the emission class
     * @return the fuel type
     */
    std::string getFuel(const SUMOEmissionClass c) const;

    /** @brief Returns the Euro emission class described by this emission class as described in the Amitran interface (0, ..., 6)
     * @param[in] c the emission class
     * @return the Euro class
     */
    int getEuroClass(const SUMOEmissionClass c) const;


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
        if (c == HBEFA3_BASE || a < 0.) {
            return 0.;
        }
        const int index = (c & ~PollutantsInterface::HEAVY_BIT) - HBEFA3_BASE - 1;
        SUMOReal scale = 3.6;
        if (e == PollutantsInterface::FUEL) {
            if (getFuel(c) == "Diesel") {
                scale *= 836.;
            } else {
                scale *= 742.;
            }
        }
        const double* f = myFunctionParameter[index][e];
        return (SUMOReal) MAX2((f[0] + f[1] * a * v + f[2] * a * a * v + f[3] * v + f[4] * v * v + f[5] * v * v * v) / scale, 0.);
    }


private:
    /// @brief The function parameter
    static double myFunctionParameter[45][6][6];

};


#endif

/****************************************************************************/

