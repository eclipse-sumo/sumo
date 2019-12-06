/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    HelpersEnergy.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
///
// Helper methods for HBEFA-based emission computation
/****************************************************************************/
#ifndef HelpersEnergy_h
#define HelpersEnergy_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
 * @class HelpersEnergy
 * @brief Helper methods for energy-based electricity consumption computation based on the battery device
 */
class HelpersEnergy : public PollutantsInterface::Helper {
public:
    static const int ENERGY_BASE = 3 << 16;

    /** @brief Constructor (initializes myEmissionClassStrings)
     */
    HelpersEnergy();


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
    double compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const std::map<int, double>* param) const;

    /** @brief Computes the achievable acceleartion using the given speed and amount of consumed electric power
     *
     * @param[in] c emission class for the function parameters to use
     * @param[in] e the type of emission (CO, CO2, ...), only electricity gives valid results
     * @param[in] v The vehicle's current velocity
     * @param[in] P The vehicle's current power consumption
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @return The amount emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    double acceleration(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double P, const double slope, const std::map<int, double>* param) const;

    double getDefaultParam(int paramKey) const {
        return myDefaultParameter.find(paramKey)->second;
    }


private:
    /// @brief The default parameter
    std::map<int, double> myDefaultParameter;

    /** @brief Solver of quadratic equation ax^2 + bx + c = 0
     *
     * return only real roots
     *
     * @param[in] a The coefficient of the qadratic term x^2
     * @param[in] b The coefficient of the linear term x
     * @param[in] c The coefficient of the constant term 
     * @return the number of real roots and these real roots
     */
    std::tuple<int, double, double> quadraticSolve(double a, double b, double c) const;

    /** @brief Solver of cubic equation ax^3 + bx^2 + cx + d = 0
     *
     * return only real roots
     *
     * @param[in] a The coefficient of the cubic term x^3
     * @param[in] b The coefficient of the qadratic term x^2
     * @param[in] c The coefficient of the linear term x
     * @param[in] d The coefficient of the constant term
     * @return the number of real roots and these real roots
     */
    std::tuple<int, double, double, double> cubicSolve(double a, double b, double c, double d) const;
};


#endif

/****************************************************************************/

