/****************************************************************************/
/// @file    MSMeanData.h
/// @author  Daniel Krajzewicz, Michael Behrisch
/// @date    Tue, 17.11.2009
/// @version $Id$
///
// Data collector for edges/lanes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSMeanData_h
#define MSMeanData_h


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
class MSEdge;
class MSLane;
class SUMOVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMeanData
 * @brief Data collector for edges/lanes
 *
 * This structure does not contain the data itself, it is stored within
 *  MeanDataValues-MoveReminder objects.
 * This class is used to build the output, optionally, in the case
 *  of edge-based dump, aggregated over the edge's lanes.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSMeanData : public MSDetectorFileOutput {
public:
    /**
     * @class MeanDataValues
     * @brief Data structure for mean (aggregated) edge/lane values
     *
     * Structure holding values that describe the emissions aggregated
     *  over some seconds.
     */
    class MeanDataValues : public MSMoveReminder {
    public:
        /** @brief Constructor */
        MeanDataValues(MSLane * const lane, const std::set<std::string>* const vTypes=0) throw();

        /** @brief Destructor */
        virtual ~MeanDataValues() throw();


        /** @brief Resets values so they may be used for the next interval
         */
        virtual void reset() throw() = 0;

        /** @brief Add the values to this meanData
         */
        virtual void add(MeanDataValues& val) throw() = 0;

        /// @name Methods inherited from MSMoveReminder.
        /// @{

        /** @brief Tests whether the vehicles type is to be regarded
         *
         * @param[in] veh The regarded vehicle
         * @return whether the type of the vehicle is in the set of regarded types
         */
        bool vehicleApplies(const SUMOVehicle& veh) const throw();
        //@}


        /** @brief Returns whether any data was collected.
         *
         * @return whether no data was collected
         */
        virtual bool isEmpty() const throw();


        /** @brief Called if a per timestep update is needed. Default does nothing.
         */
        virtual void update() throw();

        /** @brief Writes output values into the given stream
         *
         * @param[in] dev The output device to write the data into
         * @param[in] period Length of the period the data were gathered
         * @param[in] numLanes The total number of lanes for which the data was collected
         * @param[in] length The length of the object for which the data was collected
         * @exception IOError If an error on writing occures (!!! not yet implemented)
         */
        virtual void write(OutputDevice &dev, const SUMOReal period,
                           const SUMOReal numLanes, const SUMOReal length) const throw(IOError) = 0;

    public:
        /// @name Collected values
        /// @{
        /// @brief The number of sampled vehicle movements (in s)
        SUMOReal sampleSeconds;
        /// @brief The sum of the distances the vehicles travelled
        SUMOReal travelledDistance;
        //@}

    private:
        /// @brief The vehicle types to look for (0 or empty means all)
        const std::set<std::string>* const myVehicleTypes;

    };


public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] dumpBegin Begin time of dump
     * @param[in] dumpEnd End time of dump
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     * @param[in] maxTravelTime the maximum travel time to use when calculating per vehicle output
     * @param[in] minSamples the minimum number of sample seconds before the values are valid
     * @param[in] vTypes the set of vehicle types to consider
     */
    MSMeanData(const std::string &id,
               const SUMOTime dumpBegin, const SUMOTime dumpEnd,
               const bool useLanes, const bool withEmpty,
               const SUMOReal minSamples, const SUMOReal maxTravelTime,
               const std::set<std::string> vTypes) throw();


    /// @brief Destructor
    virtual ~MSMeanData() throw();

    /** @brief Adds the value collectors to all relevant edges.
     *
     * @param[in] edges the edges to use
     * @param[in] withInternal Information whether internal lanes/edges shall be written
     */
    void init(const std::vector<MSEdge*> &edges, const bool withInternal) throw();

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
    void writeXMLOutput(OutputDevice &dev, SUMOTime startTime, SUMOTime stopTime) throw(IOError);

    /** @brief Opens the XML-output using "netstats" as root element
     *
     * @param[in] dev The output device to write the root into
     * @see MSDetectorFileOutput::writeXMLDetectorProlog
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeXMLDetectorProlog(OutputDevice &dev) const throw(IOError);
    /// @}

    /** @brief Updates the detector
     */
    void update() throw();


protected:
    /** @brief Create an instance of MeanDataValues
     *
     * @param[in] lane The lane to create for
     */
    virtual MSMeanData::MeanDataValues* createValues(MSLane * const lane) throw(IOError) = 0;

    /** @brief Resets network value in order to allow processing of the next interval
     *
     * Goes through the lists of edges and starts "resetOnly" for each edge.
     * @param [in] edge The last time step that is reported
     */
    void resetOnly(SUMOTime stopTime) throw();

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
    virtual void writeEdge(OutputDevice &dev, const std::vector<MeanDataValues*> &edgeValues,
                           MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError);

    /** @brief Checks for emptiness and writes prefix into the given stream
     *
     * @param[in] dev The output device to write the data into
     * @param[in] values The values to check for emptiness
     * @param[in] prefix The xml prefix to write (mostly the lane / edge id)
     * @return whether further output should be generated
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    bool writePrefix(OutputDevice &dev, const MeanDataValues &values,
                     const std::string prefix) const throw(IOError);

protected:
    /// @brief the minimum sample seconds
    const SUMOReal myMinSamples;

    /// @brief the maximum travel time to write
    const SUMOReal myMaxTravelTime;

    /// @brief The vehicle types to look for (empty means all)
    const std::set<std::string> myVehicleTypes;

private:
    /// @brief The id of the detector
    const std::string myID;

    /// @brief Information whether the output shall be edge-based (not lane-based)
    const bool myAmEdgeBased;

    /// @brief The first and the last time step to write information (-1 indicates always)
    const SUMOTime myDumpBegin, myDumpEnd;

    /// @brief Value collectors; sorted by edge, then by lane
    std::vector<std::vector<MeanDataValues*> > myMeasures;

    /// @brief The corresponding first edges
    std::vector<MSEdge*> myEdges;

    /// @brief Whether empty lanes/edges shall be written
    const bool myDumpEmpty;

private:
    /// @brief Invalidated copy constructor.
    MSMeanData(const MSMeanData&);

    /// @brief Invalidated assignment operator.
    MSMeanData& operator=(const MSMeanData&);

};


#endif

/****************************************************************************/

