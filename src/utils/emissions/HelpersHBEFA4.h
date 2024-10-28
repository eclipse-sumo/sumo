/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    HelpersHBEFA4.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
///
// Helper methods for HBEFA4-based emission computation
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <limits>
#include <cmath>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include "EnergyParams.h"
#include "PollutantsInterface.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersHBEFA4
 * @brief Helper methods for HBEFA4-based emission computation
 *
 * The parameter are stored per vehicle class; 6*6 parameter are used, sorted by
 *  the pollutant (CO2, CO, HC, fuel, NOx, PMx), and the function part
 *  (c0, cav1, cav2, c1, c2, c3).
 */
class HelpersHBEFA4 : public PollutantsInterface::Helper {
private:
    static const int HBEFA4_BASE = 7 << 16;

public:
    /** @brief Constructor (initializes myEmissionClassStrings)
     */
    HelpersHBEFA4();

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
     * For most emissions the function yields mg/s. For fuel ml/s is returned if volumetric fuel has been requested.
     *  Coasting and an engine which is off by the given param result directly in zero emission.
     *
     * @param[in] c emission class for the function parameters to use
     * @param[in] e the type of emission (CO, CO2, ...)
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @param[in] param parameter of the emission model (only used for the coasting deceleration and to determine whether the engine is off)
     * @return The amount emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    inline double compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const EnergyParams* param) const {
        if (param != nullptr && param->isEngineOff()) {
            return 0.;
        }
        const std::string& fuel = getFuel(c);
        if (fuel != "Electricity" && v > ZERO_SPEED_ACCURACY && a < getCoastingDecel(c, v, a, slope, param)) {
            return 0.;
        }
        const int index = (c & ~PollutantsInterface::HEAVY_BIT) - HBEFA4_BASE;
        double scale = 1.;
        if (e == PollutantsInterface::FUEL && myVolumetricFuel) {
            if (fuel == "Diesel") {
                scale *= 836.;
            } else if (fuel == "Gasoline") {
                scale *= 742.;
            }
        }
        const double* f = myFunctionParameter[index][e];
        double result = (f[0] + f[1] * v + f[2] * a + f[3] * v * v + f[4] * v * v * v + f[5] * a * v + f[6] * a * v * v) / scale;
        if (e != PollutantsInterface::ELEC) {
            // no negative emissions
            result = MAX2(0.0, result);
        }
        return result;
    }


private:
    /// @brief The function parameter
    static double myFunctionParameter[833][7][7];

};
