/****************************************************************************/
/// @file    MSMeanData_Net.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
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
#ifndef MSMeanData_Net_h
#define MSMeanData_Net_h


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
#ifdef HAVE_MESOSIM
class MEVehicle;
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSMeanData_Net
 * @brief Network state mean data collector for edges/lanes
 *
 * This structure does not contain the data itself, it is stored within
 *  MSLaneMeanDataValues-MoveReminder objects.
 * This class is used to build the output, optionally, in the case
 *  of edge-based dump, aggregated over the edge's lanes.
 *
 * @todo check where mean data is stored in mesosim.
 * @todo consider error-handling on write (using IOError)
 */
class MSMeanData_Net : public MSDetectorFileOutput {
public:
    /**
     * @class MSLaneMeanDataValues
     * @brief Data structure for mean (aggregated) edge/lane values
     *
     * Structure holding values that describe the flow and other physical
     *  properties aggregated over some seconds.
     *
     * @todo Check whether the haltings-information is used and how
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

        /** @brief Add the values to this meanData
         */
        void add(MSLaneMeanDataValues& val) throw();

        /// @name Methods inherited from MSMoveReminder
        /// @{

        /** @brief Computes current values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current values.
         *
         * Additionally, if the vehicle has entered this lane, "nVehEnteredLane"
         *  is incremented, and if the vehicle has left the lane, "nVehLeftLane".
         *
         * @param[in] veh The regarded vehicle
         * @param[in] oldPos Position before the move-micro-timestep.
         * @param[in] newPos Position after the move-micro-timestep.
         * @param[in] newSpeed The vehicle's current speed
         * @return false, if the vehicle is beyond the lane, true otherwise
         * @see MSMoveReminder
         * @see MSMoveReminder::isStillActive
         */
        bool isStillActive(MSVehicle& veh, SUMOReal oldPos, SUMOReal newPos, SUMOReal newSpeed) throw();


        /** @brief Called if the vehicle leaves the reminder's lane
         *
         * @param veh The leaving vehicle.
         * @see MSMoveReminder
         * @see MSMoveReminder::dismissOnLeavingLane
         */
        void dismissOnLeavingLane(MSVehicle& veh) throw();


        /** @brief Computes current emission values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current values.
         *  The "emitted" field is incremented, additionally.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] isEmit true means emit, false: lane change
         * @see MSMoveReminder::isActivatedByEmitOrLaneChange
         * @return Always true
         */
        bool isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw();
        //@}



        /// @name Collected values
        /// @{

        /// @brief The number of vehicles that were emitted on the lane
        unsigned nVehEmitted;

        /// @brief The number of vehicles that entered this lane within the sample intervall
        unsigned nVehEnteredLane;

        /// @brief The number of vehicles that left this lane within the sample intervall
        unsigned nVehLeftLane;

        /// @brief The number of vehicles that changed to this lane
        unsigned nLaneChanges;

        /// @brief The number of sampled vehicle movements (in s)
        SUMOReal sampleSeconds;

        /// @brief The sum of the distances the vehicles travelled
        SUMOReal travelledDistance;

        /// @brief The number of vehicle probes with v<0.1
        unsigned haltSum;

        /// @brief The sum of the lengths the vehicles had
        SUMOReal vehLengthSum;
        //@}


#ifdef HAVE_MESOSIM
        std::map<MEVehicle*, std::pair<SUMOReal, SUMOReal> > myLastVehicleUpdateValues;
#endif

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
     */
    MSMeanData_Net(const std::string &id,
                   MSEdgeControl &ec, SUMOTime dumpBegin,
                   SUMOTime dumpEnd, bool useLanes,
                   bool withEmpty) throw();


    /// @brief Destructor
    virtual ~MSMeanData_Net() throw();


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
     * @param[in] laneValues This lane's / edge's value collectors
     * @param[in] period Length of the period the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    virtual void writeValues(OutputDevice &dev, std::string prefix,
                             MSLaneMeanDataValues &laneValues, SUMOReal period,
                             SUMOReal length, SUMOReal numLanes, SUMOReal maxSpeed) throw(IOError);


    /** @brief Resets network value in order to allow processing of the next interval
     *
     * Goes through the lists of edges and starts "resetOnly" for each edge.
     * @param [in] edge The last time step that is reported
     */
    void resetOnly(SUMOTime stopTime) throw();

protected:
    /// @brief The id of the detector
    std::string myID;

    /// @brief Information whether the output shall be edge-based (not lane-based)
    bool myAmEdgeBased;

    /// @brief The first and the last time step to write information (-1 indicates always)
    SUMOTime myDumpBegin, myDumpEnd;

    /// @brief Whether empty lanes/edges shall be written
    bool myDumpEmpty;

    /// @brief Value collectors; sorted by edge, then by lane
    std::vector<std::vector<MSLaneMeanDataValues*> > myMeasures;

    /// @brief The corresponding first edges
    std::vector<MSEdge*> myEdges;

private:
    /// @brief Invalidated copy constructor.
    MSMeanData_Net(const MSMeanData_Net&);

    /// @brief Invalidated assignment operator.
    MSMeanData_Net& operator=(const MSMeanData_Net&);

};


#endif

/****************************************************************************/

