/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @file    HelpersEnergy.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
///
// Helper methods for HBEFA-based emission computation
/****************************************************************************/
#pragma once
#include <config.h>

#include <vector>
#include <limits>
#include <cmath>
#include <utils/common/StdDefs.h>
#include <utils/geom/GeomHelper.h>
#include <utils/common/SUMOVehicleClass.h>
#include "PollutantsInterface.h"
#include "EnergyParams.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersEnergy
 * @brief Helper methods for energy-based electricity consumption computation based on the battery device
 */
class HelpersEnergy : public PollutantsInterface::Helper {
private:
    static const int ENERGY_BASE = 4 << 16;

public:
    /** @brief Constructor (initializes myEmissionClassStrings)
     */
    HelpersEnergy();

    /** @brief Returns the fuel type described by this emission class as described in the Amitran interface (Gasoline, Diesel, ...)
     * @param[in] c the emission class
     * @return always "Electricity"
     */
    std::string getFuel(const SUMOEmissionClass /* c */) const {
        return "Electricity";
    }

    /** @brief Returns a reference weight in kg described by this emission class
     * This implementation returns the default mass for this model.
     * @param[in] c the emission class
     * @return a reference weight
     */
    double getWeight(const SUMOEmissionClass /* c */) const {
        return myDefaultMass;
    }

    /** @brief Computes the emitted pollutant amount using the given speed and acceleration
     *
     * Returns only valid values for electricity all other types give 0.
     *
     * @param[in] c emission class for the function parameters to use
     * @param[in] e the type of emission (CO, CO2, ...), only electricity gives valid results
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @return The amount emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    double compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const EnergyParams* param) const;

    /** @brief Computes the achievable acceleration using the given speed and amount of consumed electric power
     *
     * @param[in] c emission class for the function parameters to use
     * @param[in] e the type of emission (CO, CO2, ...), only electricity gives valid results
     * @param[in] v The vehicle's current velocity
     * @param[in] P The vehicle's current power consumption
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @return The amount emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    double acceleration(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double P, const double slope, const EnergyParams* param) const;

private:
    // default values from https://sumo.dlr.de/docs/Models/Electric.html#kia_soul_ev_2020
    static constexpr double myDefaultMass = 1830.;
    static constexpr double myDefaultFrontSurfaceArea = 2.6;
    static constexpr double myDefaultAirDragCoefficient = 0.35;
    static constexpr double myDefaultRotatingMass = 40.;
    static constexpr double myDefaultRadialDragCoefficient = 0.1;
    static constexpr double myDefaultRollDragCoefficient = 0.01;
    static constexpr double myDefaultConstantPowerIntake = 100.;
    static constexpr double myDefaultPropulsionEfficiency = 0.98;
    static constexpr double myDefaultRecuperationEfficiency = 0.96;
    static constexpr double myDefaultRecuperationEfficiencyByDeceleration = 0.0;

};
