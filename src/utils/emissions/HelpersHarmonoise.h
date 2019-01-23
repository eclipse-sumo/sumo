/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    HelpersHarmonoise.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Helper methods for Harmonoise-based noise emission computation
/****************************************************************************/
#ifndef HelpersHarmonoise_h
#define HelpersHarmonoise_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

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
    static double computeNoise(SUMOEmissionClass c, double v, double a);


    /** @brief Computes the resulting noise
     *
     * @param[in] val The sum of converted vehicle noises ( pow(10., (<NOISE>/10.)) )
     * @return The resulting sum
     */
    inline static double sum(double val) {
        return double(10. * log10(val));
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

