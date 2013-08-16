/****************************************************************************/
/// @file    MSMeanData_HBEFA.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Emission data collector for edges/lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSMeanData_HBEFA_h
#define MSMeanData_HBEFA_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <set>
#include "MSMeanData.h"
#include <limits>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMeanData_HBEFA
 * @brief Emission data collector for edges/lanes
 *
 * This structure does not contain the data itself, it is stored within
 *  MSLaneMeanDataValues-MoveReminder objects.
 * This class is used to build the output, optionally, in the case
 *  of edge-based dump, aggregated over the edge's lanes.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSMeanData_HBEFA : public MSMeanData {
public:
    /**
     * @class MSLaneMeanDataValues
     * @brief Data structure for mean (aggregated) edge/lane values
     *
     * Structure holding values that describe the emissions aggregated
     *  over some seconds.
     */
    class MSLaneMeanDataValues : public MSMeanData::MeanDataValues {
    public:
        /** @brief Constructor */
        MSLaneMeanDataValues(MSLane* const lane, const SUMOReal length, const bool doAdd,
                             const std::set<std::string>* const vTypes = 0,
                             const MSMeanData_HBEFA* parent = 0);

        /** @brief Destructor */
        virtual ~MSLaneMeanDataValues();


        /** @brief Resets values so they may be used for the next interval
         */
        void reset(bool afterWrite = false);

        /** @brief Add the values of this to the given one and store them there
         *
         * @param[in] val The meandata to add to
         */
        void addTo(MSMeanData::MeanDataValues& val) const;

        /** @brief Writes output values into the given stream
         *
         * @param[in] dev The output device to write the data into
         * @param[in] prefix The xml prefix to write (mostly the lane / edge id)
         * @param[in] numLanes The total number of lanes for which the data was collected
         * @param[in] length The length of the object for which the data was collected
         * @exception IOError If an error on writing occurs (!!! not yet implemented)
         */
        void write(OutputDevice& dev, const SUMOTime period,
                   const SUMOReal numLanes, const SUMOReal defaultTravelTime,
                   const int numVehicles = -1) const;

    protected:
        /** @brief Internal notification about the vehicle moves
         *
         * Indicator if the reminders is still active for the passed
         * vehicle/parameters. If false, the vehicle will erase this reminder
         * from it's reminder-container.
         *
         * @param[in] veh Vehicle that asks this reminder.
         * @param[in] timeOnLane time the vehicle spent on the lane.
         * @param[in] speed Moving speed.
         */
        void notifyMoveInternal(SUMOVehicle& veh, SUMOReal timeOnLane,
                                SUMOReal speed);

    private:
        /// @name Collected values
        /// @{
        /// @brief Sum of CO2 emissions in mg
        SUMOReal CO2;
        /// @brief Sum of CO emissions in mg
        SUMOReal CO;
        /// @brief Sum of HC emissions in mg
        SUMOReal HC;
        /// @brief Sum of NOx emissions in mg
        SUMOReal NOx;
        /// @brief Sum of PMx emissions in mg
        SUMOReal PMx;
        /// @brief  Sum of consumed fuel in ml
        SUMOReal fuel;
        //@}
        /// @brief The meandata parent
        const MSMeanData_HBEFA* myParent;

    };


public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] dumpBegin Begin time of dump
     * @param[in] dumpEnd End time of dump
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     * @param[in] printDefaults Information whether defaults for empty lanes/edges shall be written
     * @param[in] withInternal Information whether internal lanes/edges shall be written
     * @param[in] trackVehicles Information whether vehicles shall be tracked
     * @param[in] maxTravelTime the maximum travel time to use when calculating per vehicle output
     * @param[in] minSamples the minimum number of sample seconds before the values are valid
     * @param[in] vTypes the set of vehicle types to consider
     */
    MSMeanData_HBEFA(const std::string& id,
                     const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                     const bool useLanes, const bool withEmpty,
                     const bool printDefaults, const bool withInternal,
                     const bool trackVehicles,
                     const SUMOReal minSamples, const SUMOReal maxTravelTime,
                     const std::set<std::string> vTypes);


    /// @brief Destructor
    virtual ~MSMeanData_HBEFA();

protected:
    /** @brief Create an instance of MeanDataValues
     *
     * @param[in] lane The lane to create for
     * @param[in] doAdd whether to add the values as reminder to the lane
     */
    MSMeanData::MeanDataValues* createValues(MSLane* const lane, const SUMOReal length, const bool doAdd) const;

private:
    /// @brief Invalidated copy constructor.
    MSMeanData_HBEFA(const MSMeanData_HBEFA&);

    /// @brief Invalidated assignment operator.
    MSMeanData_HBEFA& operator=(const MSMeanData_HBEFA&);

};


#endif

/****************************************************************************/

