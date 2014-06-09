/****************************************************************************/
/// @file    HelpersPHEMlight.h
/// @author  Daniel Krajzewicz
/// @author  Nikolaus Furian
/// @date    Sat, 20.04.2013
/// @version $Id$
///
// Helper methods for PHEMlight-based emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2013-2012 DLR (http://www.dlr.de/) and contributors
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
#include "PollutantsInterface.h"
#include "PHEMCEPHandler.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersPHEMlight
 * @brief Helper methods for PHEMlight-based emission computation
 */
class HelpersPHEMlight : public PollutantsInterface::Helper {
public:
    static const int PHEMLIGHT_BASE = 2 << 16;


    /** @brief Constructor
     */
    HelpersPHEMlight();


    /** @brief Checks whether the string describes a known vehicle class
     * @param[in] eClass The string describing the vehicle emission class
     * @return whether it describes a valid emission class
     */
    SUMOEmissionClass getClassByName(const std::string& eClass, const SUMOVehicleClass vc);


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

    /** @brief Returns a reference weight in kg described by this emission class as described in the Amitran interface
     * This implementation returns only meaningful values for Solo_LKW (truck without trailer) and LNF (light duty vehicles).
     * @param[in] c the emission class
     * @return a reference weight
     */
    SUMOReal getWeight(const SUMOEmissionClass c) const;

    /** @brief Returns the maximum possible acceleration
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The maximum possible acceleration
     */
    SUMOReal getMaxAccel(SUMOEmissionClass c, double v, double a, double slope);


    /** @brief Returns the amount of emitted pollutant given the vehicle type and state (in mg/s or in ml/s for fuel)
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @param[in] slope The road's slope at vehicle's position [°]
     * @return The amount of the pollutant emitted by the given emission class when moving with the given velocity and acceleration [mg/s or ml/s]
     */
    SUMOReal compute(const SUMOEmissionClass c, const PollutantsInterface::EmissionType e, const double v, const double a, const double slope) const;

private:
    /// @brief the index of the next class
    int myIndex;
};


#endif

/****************************************************************************/

