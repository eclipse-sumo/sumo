/****************************************************************************/
/// @file    MSMeanData_Net.h
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id:MSMeanData_Net.h 4976 2008-01-30 14:23:39Z dkrajzew $
///
// Redirector for mean data output (net->edgecontrol)
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
 * @brief Redirector for mean data output (net->edgecontrol)
 *
 * This structure does not contain the data itself, it is stored within 
 *  MSLaneMeanDataValues-MoveReminder objects. 
 * This class is used to build the output, optionally, in the case
 *  of edge-based dump, aggregated over the edge's lanes.
 *
 * @todo check where mean data is stored in mesosim.
 * @todo consider error-handling on write (using IOError)
 */
class MSMeanData_Net : public MSDetectorFileOutput
{
public:
/**
 * @class MSLaneMeanDataValues
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
    MSLaneMeanDataValues(MSLane * const lane) throw();


    /** @brief Resets values so they may be used for the next interval
     */
    void reset() throw();


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


    /** @brief Nothing is done here
     *
     * @param[in] veh The leaving vehicle.
     * @see MSMoveReminder::dismissByLaneChange
     */
    virtual void dismissByLaneChange(MSVehicle& veh) throw();


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
    virtual bool isActivatedByEmitOrLaneChange(MSVehicle& veh, bool isEmit) throw();
    //@}



    /// @name Collected values
    /// @{

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
    //@}


#ifdef HAVE_MESOSIM
    std::map<MEVehicle*, std::pair<SUMOReal, SUMOReal> > myLastVehicleUpdateValues;
#endif

};


public:
    /** @brief Constructor
     *
     * @param[in] id The id of the detector
     * @param[in] t The interval in [s]
     * @param[in] edges Control containing the edges to use
     * @param[in] dumpBegins Begin times of dumps
     * @param[in] dumpEnds End times of dumps
     * @param[in] useLanes Information whether lane-based or edge-based dump shall be generated
     * @param[in] withEmpty Information whether empty lanes/edges shall be written
     */
    MSMeanData_Net(const std::string &id, unsigned int t, 
                   MSEdgeControl &edges, const std::vector<int> &dumpBegins,
                   const std::vector<int> &dumpEnds, bool useLanes,
                   bool withEmptyEdges, bool withEmptyLanes) throw();


    /// @brief Destructor
    virtual ~MSMeanData_Net() throw();


    /// @name Methods inherited from MSDetectorFileOutput.
    /// @{

    /** @brief Writes collected values into the given stream
     *
     * This method writes only the interval time into the stream. The interval's
     *  contents are then written (if wanted) using write.
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
    /** @brief Writes network values into the given stream
     *
     * At first, it is checked whether the values for the current interval shall be written.
     *  If not, a reset is performed, only, using "resetOnly". Otherwise,
     *  both the list of single-lane edges and the list of multi-lane edges
     *  are gone through and each edge is written using "writeEdge".
     *
     * @param[in] dev The output device to write the data into
     * @param[in] startTime First time step the data were gathered
     * @param[in] stopTime Last time step the data were gathered
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    virtual void write(OutputDevice &dev, SUMOTime startTime, SUMOTime stopTime) throw(IOError);


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


    /** @brief Inline function for value conversion
     *
     * Uses the given values to compute proper results regarding that
     *  some lanes may be unused (empty). This method is an inline
     *  method, because it is used in several places, and is assumed
     *  to be critical in speed.
     *
     * @param [in] values The edge to reset the value of
     * @param [in] period The edge to reset the value of
     * @param [in] laneLength The edge to reset the value of
     * @param [in] laneVMax The edge to reset the value of
     * @param [out] traveltime The edge to reset the value of
     * @param [out] meanSpeed The edge to reset the value of
     * @param [out] meanDensity The edge to reset the value of
     * @param [out] meanOccupancy The edge to reset the value of
     */
    inline void conv(const MSLaneMeanDataValues &values, SUMOTime period,
                     SUMOReal laneLength, SUMOReal laneVMax,
                     SUMOReal &traveltime, SUMOReal &meanSpeed,
                     SUMOReal &meanDensity, SUMOReal &meanOccupancy) throw() {

        if (values.sampleSeconds==0) {
            assert(laneVMax>=0);
            traveltime = laneLength / laneVMax;
            meanSpeed = laneVMax;
            meanDensity = 0;
            meanOccupancy = 0;
        } else {
            meanSpeed = values.speedSum / values.sampleSeconds;
            if (meanSpeed==0) {
                traveltime = 1000000;//std::numeric_limits<SUMOReal>::max() / (SUMOReal) 100.;
            } else {
                traveltime = laneLength / meanSpeed;
            }
            assert(period!=0);
            assert(laneLength!=0);
            meanDensity = values.sampleSeconds /
                          (SUMOReal) period * (SUMOReal) 1000. / (SUMOReal) laneLength;
            meanOccupancy = (SUMOReal) values.vehLengthSum /
                            (SUMOReal) period / (SUMOReal) laneLength * (SUMOReal) 100.;
        }
    }

protected:
    /// @brief The id of the detector
    std::string myID;

    /// @brief The time interval the data shall be aggregated over (in s)
    unsigned int myInterval;

    /// @brief Information whether the output shall be edge-based (not lane-based)
    bool myAmEdgeBased;

    /// @brief The first and the last time steps to write information (-1 indicates always)
    std::vector<int> myDumpBegins, myDumpEnds;

    /// @brief Whether empty lanes/edges shall be written
    bool myDumpEmptyEdges, myDumpEmptyLanes;

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

