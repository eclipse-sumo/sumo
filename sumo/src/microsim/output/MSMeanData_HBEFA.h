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
        MSLaneMeanDataValues(MSLane * const lane) throw();

        /** @brief Destructor */
        virtual ~MSLaneMeanDataValues() throw();


        /** @brief Resets values so they may be used for the next interval
         */
        void reset() throw();


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


        /** @brief Computes current emission values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current emission values
         *  which are computed using the current velocity and acceleration.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] isEmit true means emit, false: lane change
         * @see MSMoveReminder::isActivatedByEmitOrLaneChange
         * @return Always true
         */
        virtual bool isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw();
        //@}



        /// @name Collected values
        /// @{

        /// @brief The number of sampled vehicle movements (in s)
        SUMOReal sampleSeconds;

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


#ifdef HAVE_MESOSIM
        std::map<MEVehicle*, std::pair<SUMOReal, SUMOReal> > myLastVehicleUpdateValues;
#endif

    };


public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] edges Control containing the edges to use
     * @param[in] dumpBegin Begin time of dump
     * @param[in] dumpEnd End time of dump
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     */
    MSMeanData_HBEFA(const std::string &id,
                     MSEdgeControl &edges, SUMOTime dumpBegin,
                     SUMOTime dumpEnd, bool useLanes,
                     bool withEmptyEdges, bool withEmptyLanes) throw();


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


    /** @brief Writes lane values into the given stream
     *
     * @param[in] dev The output device to write the data into
     * @param[in] laneValues This lane's value collectors
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    virtual void writeLane(OutputDevice &dev,
                           MSLaneMeanDataValues &laneValues,
                           SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Resets network value in order to allow processing of the next interval
     *
     * Goes through the lists of edges and starts "resetOnly" for each edge.
     * @param [in] edge The last time step that is reported
     */
    void resetOnly(SUMOTime stopTime) throw();


    /** @brief Norms the values by the given length/duration
     *
     * @param[in] val The initial value
     * @param[in] dur The aggregation duration in s
     * @param[in] len The length of the edge
     * @todo Check subseconds simulation
     */
    inline SUMOReal norm(SUMOReal val, SUMOReal dur, SUMOReal len) const throw() {
        return SUMOReal(val * 3600. * 1000. / dur / len);
    }

protected:
    /// @brief The id of the detector
    std::string myID;

    /// @brief Information whether the output shall be edge-based (not lane-based)
    bool myAmEdgeBased;

    /// @brief The first and the last time step to write information (-1 indicates always)
    SUMOTime myDumpBegin, myDumpEnd;

    /// @brief Whether empty lanes/edges shall be written
    bool myDumpEmptyEdges, myDumpEmptyLanes;

    /// @brief Value collectors; sorted by edge, then by lane
    std::vector<std::vector<MSLaneMeanDataValues*> > myMeasures;

    /// @brief The corresponding first edges
    std::vector<MSEdge*> myEdges;

private:
    /// @brief Invalidated copy constructor.
    MSMeanData_HBEFA(const MSMeanData_HBEFA&);

    /// @brief Invalidated assignment operator.
    MSMeanData_HBEFA& operator=(const MSMeanData_HBEFA&);

};


#endif

/****************************************************************************/

