/****************************************************************************/
/// @file    HelpersTNM.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id: HelpersTNM.h 6716 2009-02-03 12:23:15Z dkrajzew $
///
// Helper methods for TNM-based noise emission computation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef HelpersTNM_h
#define HelpersTNM_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <cassert>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/MSMoveReminder.h>
#include <utils/common/StdDefs.h>
#include <limits>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSEdgeControl;
class MSEdge;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class HelpersTNM
 * @brief Helper methods for TNM-based noise emission computation
 *
 * The stored values compute the recepted noise of either passenger or heavy
 *  duty vehicles for a distance of 10m from the noise source.
 */
class HelpersTNM
{
public:
    /** @brief Returns the noise produced by the a vehicle of the given type at the given speed
     *
     * @param[in] c The vehicle emission class
     * @param[in] v The vehicle's current velocity
     */
    static SUMOReal computeNoise(SUMOEmissionClass c, double v) throw();


private:
    /// @brief The function parameter
    static double myFunctionParameter[2][14];

};


#endif

/****************************************************************************/

