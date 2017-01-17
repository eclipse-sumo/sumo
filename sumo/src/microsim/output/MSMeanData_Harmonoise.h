/****************************************************************************/
/// @file    MSMeanData_Harmonoise.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Noise data collector for edges/lanes
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
#ifndef MSMeanData_Harmonoise_h
#define MSMeanData_Harmonoise_h


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
#include "MSMeanData.h"


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMeanData_Harmonoise
 * @brief Noise data collector for edges/lanes
 *
 * This structure does not contain the data itself, it is stored within
 *  MSLaneMeanDataValues-MoveReminder objects.
 * This class is used to build the output, optionally, in the case
 *  of edge-based dump, aggregated over the edge's lanes.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSMeanData_Harmonoise : public MSMeanData {
public:
    /**
     * @class MSLaneMeanDataValues
     * @brief Data structure for mean (aggregated) edge/lane values
     *
     * Structure holding values that describe the noise aggregated over
     *  some seconds.
     */
    class MSLaneMeanDataValues : public MSMeanData::MeanDataValues {
    public:
        /** @brief Constructor */
        MSLaneMeanDataValues(MSLane* const lane, const SUMOReal length, const bool doAdd,
                             const MSMeanData_Harmonoise* parent);

        /** @brief Destructor */
        virtual ~MSLaneMeanDataValues();


        /** @brief Resets values so they may be used for the next interval
         */
        void reset(bool afterWrite = false);

        /** @brief Add the values to this meanData
         */
        void addTo(MSMeanData::MeanDataValues& val) const;


        /** @brief Computes the noise in the last time step
         *
         * The sum of noises collected so far (in the last seen step)
         *  is built, and added to meanNTemp; currentTimeN is resetted.
         */
        void update();

        /** @brief Writes output values into the given stream
         *
         * @param[in] dev The output device to write the data into
         * @param[in] period Length of the period the data were gathered
         * @param[in] numLanes The total number of lanes for which the data was collected
         * @param[in] length The length of the object for which the data was collected
         * @exception IOError If an error on writing occurs (!!! not yet implemented)
         */
        void write(OutputDevice& dev, const SUMOTime period,
                   const SUMOReal numLanes, const SUMOReal defaultTravelTime,
                   const int numVehicles = -1) const;


    protected:
        /** @brief Internal notification about the vehicle moves
         *  @see MSMoveReminder::notifyMoveInternal()
         */
        void notifyMoveInternal(const SUMOVehicle& veh, const SUMOReal /* frontOnLane */, const SUMOReal timeOnLane, const SUMOReal meanSpeedFrontOnLane, const SUMOReal meanSpeedVehicleOnLane, const SUMOReal travelledDistanceFrontOnLane, const SUMOReal travelledDistanceVehicleOnLane);

    private:
        /// @name Collected values
        /// @{

        /// @brief Sum of produced noise at this time step(pow(10, (<NOISE>/10.)))
        SUMOReal currentTimeN;

        /// @brief Sum of produced noise over time (pow(10, (<NOISE>/10.)))
        SUMOReal meanNTemp;
        //@}


        /// @brief The meandata parent
        const MSMeanData_Harmonoise* myParent;
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
    MSMeanData_Harmonoise(const std::string& id,
                          const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                          const bool useLanes, const bool withEmpty,
                          const bool printDefaults, const bool withInternal,
                          const bool trackVehicles,
                          const SUMOReal minSamples, const SUMOReal maxTravelTime,
                          const std::string& vTypes);


    /// @brief Destructor
    virtual ~MSMeanData_Harmonoise();

    /** @brief Updates the detector
     */
    virtual void detectorUpdate(const SUMOTime step);


protected:
    /** @brief Create an instance of MeanDataValues
     *
     * @param[in] lane The lane to create for
     * @param[in] doAdd whether to add the values as reminder to the lane
     */
    MSMeanData::MeanDataValues* createValues(MSLane* const lane, const SUMOReal length, const bool doAdd) const;

private:
    /// @brief Invalidated copy constructor.
    MSMeanData_Harmonoise(const MSMeanData_Harmonoise&);

    /// @brief Invalidated assignment operator.
    MSMeanData_Harmonoise& operator=(const MSMeanData_Harmonoise&);

};


#endif

/****************************************************************************/

