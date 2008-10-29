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

#include <microsim/MSMoveReminder.h>

#ifdef HAVE_MESOSIM
#include <map>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class MEVehicle;
class MSLane;
class MSVehicle;


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
class MSLaneMeanDataValues : public MSMoveReminder
{
public:
    /** @brief Constructor */
    MSLaneMeanDataValues(MSLane* lane) throw();


    /** @brief Resets values so they may be used for the next interval
     */
    void reset() throw();

    bool isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();

    virtual void dismissByLaneChange(MSVehicle& veh) throw();

    virtual bool isActivatedByEmitOrLaneChange(MSVehicle& veh) throw();


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
    std::map<MEVehicle*, std::pair<SUMOReal, SUMOReal> > myLastVehicleUpdateValues;
#endif

};


#endif

/****************************************************************************/

