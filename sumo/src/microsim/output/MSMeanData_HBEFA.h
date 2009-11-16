/****************************************************************************/
/// @file    MSMeanData_HBEFA.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Emission data collector for edges/lanes
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
#include <cassert>
#include <microsim/output/MSDetectorFileOutput.h>
#include <microsim/MSMoveReminder.h>
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
class MSMeanData_HBEFA : public MSDetectorFileOutput {
public:
    /**
     * @class MSLaneMeanDataValues
     * @brief Data structure for mean (aggregated) edge/lane values
     *
     * Structure holding values that describe the emissions aggregated
     *  over some seconds.
     */
    class MSLaneMeanDataValues : public MSMoveReminder {
    public:
        /** @brief Constructor */
        MSLaneMeanDataValues(MSLane * const lane,
                             const std::set<std::string>* const vTypes=0) throw();

        /** @brief Destructor */
        virtual ~MSLaneMeanDataValues() throw();


        /** @brief Resets values so they may be used for the next interval
         */
        void reset() throw();

        /** @brief Add the values to this meanData
         */
        void add(MSLaneMeanDataValues& val) throw();

        /// @name Methods inherited from MSMoveReminder.
        /// @{

        /** @brief Computes current emission values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current emission values
         *  which are computed using the current velocity and acceleration.
         *
         * @param[in] veh The regarded vehicle
         * @param[in] oldPos Position before the move-micro-timestep.
         * @param[in] newPos Position after the move-micro-timestep.
         * @param[in] newSpeed The vehicle's current speed
         * @return false, if the vehicle is beyond the lane, true otherwise
         * @see MSMoveReminder
         * @see MSMoveReminder::isStillActive
         * @see HelpersHBEFA
         */
        bool isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();
        //@}



        /// @name Collected values
        /// @{

        /// @brief The number of sampled vehicle movements (in s)
        SUMOReal sampleSeconds;
        /// @brief The sum of the distances the vehicles travelled
        SUMOReal travelledDistance;
        /// @brief Sum of CO2 emissions
        SUMOReal CO2;
        /// @brief Sum of CO emissions
        SUMOReal CO;
        /// @brief Sum of HC emissions
        SUMOReal HC;
        /// @brief Sum of NOx emissions
        SUMOReal NOx;
        /// @brief Sum of PMx emissions
        SUMOReal PMx;
        /// @brief  Sum of consumed fuel
        SUMOReal fuel;
        //@}

    private:
        /// @brief The vehicle types to look for (0 or empty means all)
        const std::set<std::string>* const myVehicleTypes;

    };


public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] ec Control containing the edges to use
     * @param[in] dumpBegin Begin time of dump
     * @param[in] dumpEnd End time of dump
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     * @param[in] withInternal Information whether internal lanes/edges shall be written
     * @param[in] maxTravelTime the maximum travel time to use when calculating per vehicle output
     * @param[in] minSamples the minimum number of sample seconds before the values are valid
     * @param[in] vTypes the set of vehicle types to consider
     */
    MSMeanData_HBEFA(const std::string &id, const MSEdgeControl &ec,
                     const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                     const bool useLanes, const bool withEmpty, const bool withInternal,
                     const SUMOReal minSamples, const SUMOReal maxTravelTime,
                     const std::set<std::string> vTypes) throw();


    /// @brief Destructor
    virtual ~MSMeanData_HBEFA() throw();


    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Writes collected values into the given stream
     *
     * At first, it is checked whether the values for the current interval shall be written.
     *  If not, a reset is performed, only, using "resetOnly". Otherwise,
     *  both the list of single-lane edges and the list of multi-lane edges
     *  are gone through and each edge is written using "writeEdge".
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @see MSDetectorFileOutput::writeXMLOutput
     * @see write
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    virtual void writeXMLOutput(OutputDevice &dev, SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Opens the XML-output using "netstats" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);
    /// @}


protected:
    /** @brief Writes edge values into the given stream
     *
     * microsim: It is checked whether the dump shall be generated edge-
     *  or lane-wise. In the first case, the lane-data are collected
     *  and aggregated and written directly. In the second case, "writeLane"
     *  is used to write each lane's state.
     *
     * @param[in] dev The output device to write the data into
     * @param[in] edgeValues List of this edge's value collectors
     * @param[in] edge The edge to write the dump of
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    virtual void writeEdge(OutputDevice &dev, const std::vector<MSLaneMeanDataValues*> &edgeValues,
                           MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Writes output values into the given stream
     *
     * @param[in] dev The output device to write the data into
     * @param[in] prefix The xml prefix to write (mostly the lane / edge id)
     * @param[in] values This lane's / edge's value collectors
     * @param[in] period Length of the period the data were gathered
     * @param[in] numLanes The total number of lanes for which the data was collected
     * @param[in] length The length of the object for which the data was collected
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeValues(OutputDevice &dev, const std::string prefix,
                     const MSLaneMeanDataValues &values, const SUMOReal period,
                     const SUMOReal numLanes, const SUMOReal length) throw(IOError);


    /** @brief Resets network value in order to allow processing of the next interval
     *
     * Goes through the lists of edges and starts "resetOnly" for each edge.
     * @param [in] edge The last time step that is reported
     */
    void resetOnly(SUMOTime stopTime) throw();


protected:
    /// @brief The id of the detector
    const std::string myID;

    /// @brief Information whether the output shall be edge-based (not lane-based)
    const bool myAmEdgeBased;

    /// @brief The first and the last time step to write information (-1 indicates always)
    const SUMOTime myDumpBegin, myDumpEnd;

    /// @brief Whether empty lanes/edges shall be written
    const bool myDumpEmpty;

    /// @brief Value collectors; sorted by edge, then by lane
    std::vector<std::vector<MSLaneMeanDataValues*> > myMeasures;

    /// @brief The corresponding first edges
    std::vector<MSEdge*> myEdges;

    /// @brief the maximum travel time to write
    const SUMOReal myMaxTravelTime;

    /// @brief the minimum sample seconds
    const SUMOReal myMinSamples;

    /// @brief The vehicle types to look for (empty means all)
    const std::set<std::string> myVehicleTypes;

private:
    /// @brief Invalidated copy constructor.
    MSMeanData_HBEFA(const MSMeanData_HBEFA&);

    /// @brief Invalidated assignment operator.
    MSMeanData_HBEFA& operator=(const MSMeanData_HBEFA&);

};


#endif

/****************************************************************************/

