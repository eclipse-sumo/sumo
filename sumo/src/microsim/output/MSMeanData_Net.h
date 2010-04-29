/****************************************************************************/
/// @file    MSMeanData_Net.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Network state mean data collector for edges/lanes
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
#include <set>
#include <cassert>
#include <limits>
#include "MSMeanData.h"


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
class MSMeanData_Net : public MSMeanData {
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
        /** @brief Constructor */
        MSLaneMeanDataValues(MSLane * const lane, const bool doAdd,
                             const std::set<std::string>* const vTypes=0,
                             const MSMeanData_Net *parent=0) throw();

        /** @brief Destructor */
        virtual ~MSLaneMeanDataValues() throw();

        /** @brief Resets values so they may be used for the next interval
         */
        void reset() throw();

        /** @brief Add the values of this to the given one and store them there
         *
         * @param[in] val The meandata to add to
         */
        void addTo(MSMeanData::MeanDataValues& val) const throw();

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
         * @see MSMoveReminder::notifyLeave
         */
        void notifyLeave(MSVehicle& veh, bool isArrival, bool isLaneChange) throw();


        /** @brief Computes current values and adds them to their sums
         *
         * The fraction of time the vehicle is on the lane is computed and
         *  used as a weight for the vehicle's current values.
         *  The "emitted" field is incremented, additionally.
         *
         * @param[in] veh The vehicle that enters the lane
         * @param[in] isEmit whether the vehicle was just emitted into the net
         * @param[in] isLaneChange whether the vehicle changed to the lane
         * @see MSMoveReminder::notifyEnter
         * @return Always true
         */
        bool notifyEnter(MSVehicle& veh, bool isEmit, bool isLaneChange) throw();
        //@}

        bool isEmpty() const throw();

        /** @brief Writes output values into the given stream
         *
         * @param[in] dev The output device to write the data into
         * @param[in] period Length of the period the data were gathered
         * @param[in] numLanes The total number of lanes for which the data was collected
         * @param[in] length The length of the object for which the data was collected
         * @exception IOError If an error on writing occurs (!!! not yet implemented)
         */
        void write(OutputDevice &dev, const SUMOTime period,
                   const SUMOReal numLanes, const SUMOReal length,
                   const int numVehicles=-1) const throw(IOError);

#ifdef HAVE_MESOSIM
        void addData(const MEVehicle& veh, const SUMOReal timeOnLane, const SUMOReal dist) throw();
        void getLastReported(MEVehicle *v, SUMOTime &lastReportedTime, SUMOReal &lastReportedPos) throw();
        void setLastReported(MEVehicle *v, SUMOTime lastReportedTime, SUMOReal lastReportedPos) throw();
#endif

        /// @name Collected values
        /// @{
        /// @brief The number of vehicles that were emitted on the lane
        unsigned nVehDeparted;

        /// @brief The number of vehicles that finished on the lane
        unsigned nVehArrived;

        /// @brief The number of vehicles that entered this lane within the sample intervall
        unsigned nVehEntered;

        /// @brief The number of vehicles that left this lane within the sample intervall
        unsigned nVehLeft;

    private:
        /// @brief The number of vehicles that changed from this lane
        unsigned nVehLaneChangeFrom;

        /// @brief The number of vehicles that changed to this lane
        unsigned nVehLaneChangeTo;

        /// @brief The number of vehicle probes with small speed
        SUMOReal waitSeconds;

        /// @brief The sum of the lengths the vehicles had
        SUMOReal vehLengthSum;
        //@}

        /// @brief The meandata parent
        const MSMeanData_Net* myParent;

#ifdef HAVE_MESOSIM
        std::map<MEVehicle*, std::pair<SUMOTime, SUMOReal> > myLastVehicleUpdateValues;
#endif

    };


public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] dumpBegin Begin time of dump
     * @param[in] dumpEnd End time of dump
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     * @param[in] trackVehicles Information whether vehicles shall be tracked
     * @param[in] maxTravelTime the maximum travel time to output
     * @param[in] minSamples the minimum number of sample seconds before the values are valid
     * @param[in] haltSpeed the maximum speed to consider a vehicle waiting
     * @param[in] vTypes the set of vehicle types to consider
     */
    MSMeanData_Net(const std::string &id,
                   const SUMOTime dumpBegin, const SUMOTime dumpEnd,
                   const bool useLanes, const bool withEmpty,
                   const bool trackVehicles,
                   const SUMOReal maxTravelTime, const SUMOReal minSamples,
                   const SUMOReal haltSpeed, const std::set<std::string> vTypes) throw();


    /// @brief Destructor
    virtual ~MSMeanData_Net() throw();

protected:
    /** @brief Create an instance of MeanDataValues
     *
     * @param[in] lane The lane to create for
     * @param[in] doAdd whether to add the values as reminder to the lane
     */
    MSMeanData::MeanDataValues* createValues(MSLane * const lane, const bool doAdd) const throw(IOError);

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
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    void writeEdge(OutputDevice &dev, const std::vector<MSMeanData::MeanDataValues*> &edgeValues,
                   MSEdge *edge, SUMOTime startTime, SUMOTime stopTime) throw(IOError);


    /** @brief Resets network value in order to allow processing of the next interval
     *
     * Goes through the lists of edges and starts "resetOnly" for each edge.
     * @param [in] edge The last time step that is reported
     */
    void resetOnly(SUMOTime stopTime) throw();

private:
    /// @brief the minimum sample seconds
    const SUMOReal myHaltSpeed;

    /// @brief Invalidated copy constructor.
    MSMeanData_Net(const MSMeanData_Net&);

    /// @brief Invalidated assignment operator.
    MSMeanData_Net& operator=(const MSMeanData_Net&);

};


#endif

/****************************************************************************/

