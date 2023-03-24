/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
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
/// @file    HelpersPHEMlight5.h
/// @author  Daniel Krajzewicz
/// @author  Nikolaus Furian
/// @date    Sat, 20.04.2013
///
// Helper methods for PHEMlight-based emission computation
/****************************************************************************/
#pragma once
#include <config.h>

#define INTERNAL_PHEM

#include <vector>
#include <limits>
#include <cmath>
#include <foreign/PHEMlight/V5/cpp/CEP.h>
#include <foreign/PHEMlight/V5/cpp/CEPHandler.h>
#include <foreign/PHEMlight/V5/cpp/Helpers.h>
#include <utils/common/StdDefs.h>
#include "HelpersPHEMlight.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersPHEMlight5
 * @brief Helper methods for PHEMlight-based emission computation
 */
class HelpersPHEMlight5 : public HelpersPHEMlight {
private:
    static const int PHEMLIGHT5_BASE = 6 << 16;

public:
    /** @brief Constructor
     */
    HelpersPHEMlight5();

    /** @brief Destructor
     */
    virtual ~HelpersPHEMlight5();

    /** @brief Checks whether the string describes a known vehicle class
     * @param[in] eClass The string describing the vehicle emission class
     * @return whether it describes a valid emission class
     */
    SUMOEmissionClass getClassByName(const std::string& eClass, const SUMOVehicleClass vc);

    /** @brief Returns the amount of emitted pollutant given the vehicle type and state (in mg/s or in ml/s for fuel)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @return The amount of the pollutant emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    double compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope, const EnergyParams* param) const;

    /** @brief Returns the adapted acceleration value, useful for comparing with external PHEMlight references.
     * @param[in] c the emission class
     * @param[in] v the speed value
     * @param[in] a the acceleration value
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @return the modified acceleration
     */
    double getModifiedAccel(const SUMOEmissionClass c, const double v, const double a, const double slope) const;

    /** @brief Returns the maximum deceleration value (as a negative number), which can still be considered as non-braking.
     * @param[in] c the emission class
     * @param[in] v the speed value
     * @param[in] a the acceleration value
     * @param[in] slope The road's slope at vehicle's position [deg]
     * @param[in] param parameter of the emission model affecting the computation
     * @return the coasting deceleration
     */
    virtual double getCoastingDecel(const SUMOEmissionClass c, const double v, const double a, const double slope, const EnergyParams* param) const;

private:
    /** @brief Returns the amount of emitted pollutant given the vehicle type and state (in mg/s or in ml/s for fuel)
    * @param[in] currCep The vehicle emission class
    * @param[in] e The emission type
    * @param[in] p The vehicle's current power
    * @param[in] v The vehicle's current velocity
    * @return The amount of the pollutant emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
    */
    double getEmission(PHEMlightdllV5::CEP* currCep, const std::string& e, const double p, const double v) const;

    /// @brief the index of the next class
    int myIndex;
    PHEMlightdllV5::CEPHandler myCEPHandler;
    PHEMlightdllV5::Correction* myCorrection = nullptr;
    mutable PHEMlightdllV5::Helpers myHelper;
    std::map<SUMOEmissionClass, PHEMlightdllV5::CEP*> myCEPs;
};
