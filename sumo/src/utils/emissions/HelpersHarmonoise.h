/****************************************************************************/
/// @file    HelpersHarmonoise.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for Harmonoise-based noise emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef HelpersHarmonoise_h
#define HelpersHarmonoise_h


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
#include <utils/common/SUMOVehicleClass.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersHarmonoise
 * @brief Helper methods for Harmonoise-based noise emission computation
 *
 * The stored values compute the recepted noise of either passenger or heavy
 *  duty vehicles for a distance of 10m from the noise source.
 */
class HelpersHarmonoise {
public:
    /** @brief Returns the noise produced by the a vehicle of the given type at the given speed
     *
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     * @param[in] a The vehicle's current acceleration
     * @return The noise produced by the vehicle of the given class running with v and a
     */
    static SUMOReal computeNoise(SUMOEmissionClass c, double v, double a);


    /** @brief Computes the resulting noise
     *
     * @param[in] val The sum of converted vehicle noises ( pow(10., (<NOISE>/10.)) )
     * @return The resulting sum
     */
    inline static SUMOReal sum(SUMOReal val) {
        return SUMOReal(10. * log10(val));
    }


private:
    /// @name vehicle class noise emission coefficients
    /// @{

    /// @brief rolling component, light vehicles, alpha
    static double myR_A_C1_Parameter[27];

    /// @brief rolling component, light vehicles, beta
    static double myR_B_C1_Parameter[27];

    /// @brief rolling component, heavy vehicles, alpha
    static double myR_A_C3_Parameter[27];

    /// @brief rolling component, heavy vehicles, beta
    static double myR_B_C3_Parameter[27];

    /// @brief traction component, light vehicles, alpha
    static double myT_A_C1_Parameter[27];

    /// @brief traction component, light vehicles, beta
    static double myT_B_C1_Parameter[27];

    /// @brief traction component, heavy vehicles, alpha
    static double myT_A_C3_Parameter[27];

    /// @brief traction component, heavy vehicles, beta
    static double myT_B_C3_Parameter[27];
    /// @}


    /// @brief A-weighted correction for octave bands
    static double myAOctaveBandCorrection[27];


};


#endif

/****************************************************************************/

