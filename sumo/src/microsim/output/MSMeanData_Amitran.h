/****************************************************************************/
/// @file    MSMeanData_Amitran.h
/// @author  Daniel Krajzewicz
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSMeanData_Amitran_h
#define MSMeanData_Amitran_h


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
#include <limits>
#include "MSMeanData.h"


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
 * @class MSMeanData_Amitran
 * @brief Network state mean data collector for edges/lanes
 *
 * This structure does not contain the data itself, it is stored within
 *  MSLaneMeanDataValues-MoveReminder objects.
 * This class is used to build the output, optionally, in the case
 *  of edge-based dump, aggregated over the edge's lanes.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSMeanData_Amitran : public MSMeanData {
public:
    /**
     * @class MSLaneMeanDataValues
     * @brief Data structure for mean (aggregated) edge/lane values
     *
     * Structure holding values that describe the flow and other physical
     *  properties aggregated over some seconds.
     */
    class MSLaneMeanDataValues : public MSMeanData::MeanDataValues {
    public:
        /** @brief Constructor
         * @param[in] length The length of the object for which the data gets collected
         */
        MSLaneMeanDataValues(MSLane* const lane, const SUMOReal length, const bool doAdd,
                             const std::set<std::string>* const vTypes = 0,
                             const MSMeanData_Amitran* parent = 0);

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

        /// @name Methods inherited from MSMoveReminder
        /// @{

        /** @brief Computes current values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current values.
         *  The "emitted" field is incremented, additionally.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] veh The entering vehicle.
         * @param[in] reason how the vehicle enters the lane
         * @return Always true
         * @see MSMoveReminder::notifyEnter
         * @see MSMoveReminder::Notification
         */
        bool notifyEnter(SUMOVehicle& veh, MSMoveReminder::Notification reason);
        //@}

        bool isEmpty() const;

        /** @brief Writes output values into the given stream
         *
         * @param[in] dev The output device to write the data into
         * @param[in] period Length of the period the data were gathered
         * @param[in] numLanes The total number of lanes for which the data was collected
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
        /// @brief The number of vehicles that entered this lane within the sample interval
        unsigned amount;

        /// @brief The number of vehicles that entered this lane within the sample interval by type
        std::map<const MSVehicleType*, unsigned> typedAmount;

        /// @brief The number of sampled vehicle movements by type (in s)
        std::map<const MSVehicleType*, SUMOReal> typedSamples;

        /// @brief The sum of the distances the vehicles travelled by type
        std::map<const MSVehicleType*, SUMOReal> typedTravelDistance;
        //@}

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
     * @param[in] maxTravelTime the maximum travel time to output
     * @param[in] minSamples the minimum number of sample seconds before the values are valid
     * @param[in] haltSpeed the maximum speed to consider a vehicle waiting
     * @param[in] vTypes the set of vehicle types to consider
     */
    MSMeanData_Amitran(const std::string& id,
                       const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                       const bool useLanes, const bool withEmpty, const bool printDefaults,
                       const bool withInternal, const bool trackVehicles,
                       const SUMOReal maxTravelTime, const SUMOReal minSamples,
                       const SUMOReal haltSpeed, const std::set<std::string> vTypes);


    /// @brief Destructor
    virtual ~MSMeanData_Amitran();

    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Opens the XML-output using "netstats" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    virtual void writeXMLDetectorProlog(OutputDevice& dev) const;
    /// @}

    /** @brief Return the relevant edge id
     *
     * @param[in] edge The edge to retrieve the id for
     */
    virtual std::string getEdgeID(const MSEdge* const edge);

    /** @brief Writes the interval opener
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     */
    virtual void openInterval(OutputDevice& dev, const SUMOTime startTime, const SUMOTime stopTime);

    /** @brief Checks for emptiness and writes prefix into the given stream
     *
     * @param[in] dev The output device to write the data into
     * @param[in] values The values to check for emptiness
     * @param[in] tag The xml tag to write (lane / edge)
     * @param[in] id The id for the lane / edge to write
     * @return whether further output should be generated
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    virtual bool writePrefix(OutputDevice& dev, const MeanDataValues& values,
                             const SumoXMLTag tag, const std::string id) const;

protected:
    /** @brief Create an instance of MeanDataValues
     *
     * @param[in] lane The lane to create for
     * @param[in] doAdd whether to add the values as reminder to the lane
     */
    MSMeanData::MeanDataValues* createValues(MSLane* const lane, const SUMOReal length, const bool doAdd) const;

    /** @brief Resets network value in order to allow processing of the next interval
     *
     * Goes through the lists of edges and starts "resetOnly" for each edge.
     * @param [in] edge The last time step that is reported
     */
    void resetOnly(SUMOTime stopTime);

private:
    /// @brief the minimum sample seconds
    const SUMOReal myHaltSpeed;

    /// @brief Invalidated copy constructor.
    MSMeanData_Amitran(const MSMeanData_Amitran&);

    /// @brief Invalidated assignment operator.
    MSMeanData_Amitran& operator=(const MSMeanData_Amitran&);

};


#endif

/****************************************************************************/

