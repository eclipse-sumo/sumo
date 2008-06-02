/****************************************************************************/
/// @file    MSLaneMeanDataValues.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Data structure for mean (aggregated) edge/lane values
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSLaneMeanDataValues_h
#define MSLaneMeanDataValues_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @struct MSLaneMeanDataValues
 * @brief Data structure for mean (aggregated) edge/lane values
 *
 * Structure holding values that describe the flow and other physical
 *  properties aggregated over some seconds and normalised by the
 *  aggregation period.
 *
 * @todo Check whether the haltings-information is used and how
 */
struct MSLaneMeanDataValues {
    /** @brief Constructor */
    MSLaneMeanDataValues() throw()
            : sampleSeconds(0), nVehLeftLane(0), nVehEnteredLane(0),
            speedSum(0), haltSum(0), vehLengthSum(0),
            emitted(0)
#ifdef HAVE_MESOSIM
    , nVehEntireLane(0)
#endif
    {}


    /** @brief Resets values so they may be used for the next interval
     */
    void reset() throw() {
        sampleSeconds = 0.;
        nVehLeftLane = 0;
        nVehEnteredLane = 0;
        speedSum = 0;
        haltSum = 0;
        vehLengthSum = 0;
        emitted = 0;
#ifdef HAVE_MESOSIM
        nVehEntireLane = 0;
#endif
    }


    /// @brief The number of sampled vehicle movements (in s)
    SUMOReal sampleSeconds;

    /// @brief The number of vehicles that left this lane within the sample intervall
    unsigned nVehLeftLane;

    /// @brief The number of vehicles that entered this lane within the sample intervall
    unsigned nVehEnteredLane;

    /// @brief The sum of the speeds the vehicles had
    SUMOReal speedSum;

    /// @brief The number of vehicle probes with v<0.1
    unsigned haltSum;

    /// @brief The sum of the lengths the vehicles had
    SUMOReal vehLengthSum;

    /// @brief The number of vehicles that were emitted on the lane
    unsigned emitted;

#ifdef HAVE_MESOSIM
    /// @brief The number of vehicles that passed the entire lane
    unsigned nVehEntireLane;
#endif

};


#endif

/****************************************************************************/

