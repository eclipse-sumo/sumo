/****************************************************************************/
/// @file    MSDetectorControl.h
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// Detectors container; responsible for string and output generation
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
#ifndef MSDetectorControl_h
#define MSDetectorControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <vector>
#include <utils/common/NamedObjectCont.h>
#include <microsim/output/MSE2Collector.h>
#include <microsim/output/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/output/MSE3Collector.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/output/MSVTypeProbe.h>
#include <microsim/output/MSRouteProbe.h>

#ifdef _MESSAGES
#include <microsim/output/MSMsgInductLoop.h>
#endif

#ifdef HAVE_MESOSIM
#include <mesosim/MEInductLoop.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDetectorControl
 * @brief Detectors container; responsible for string and output generation
 */
class MSDetectorControl
{
public:
    /// @name Definitions of containers for different detector types
    /// @{

#ifdef _MESSAGES
    /// @brief Map of MSMsgInductLoop by ID
    typedef NamedObjectCont< MSMsgInductLoop*> MsgLoopDict;
#endif

    /// @brief Map of MSInductLoop by ID
    typedef NamedObjectCont< MSInductLoop*> LoopDict;

    /// @brief Map of MSE2Collector by ID
    typedef NamedObjectCont< MSE2Collector*> E2Dict;

    /// @brief Map of MSE3Collector by ID
    typedef NamedObjectCont< MSE3Collector*> E3Dict;

    /// @brief Map of MS_E2_ZS_CollectorOverLanes by ID
    typedef NamedObjectCont< MS_E2_ZS_CollectorOverLanes* > E2ZSOLDict;

    /// @brief Map of MSVTypeProbe by ID
    typedef NamedObjectCont< MSVTypeProbe* > VTypeProbeDict;

    /// @brief Map of MSRouteProbe by ID
    typedef NamedObjectCont< MSRouteProbe* > RouteProbeDict;

#ifdef HAVE_MESOSIM
    /// @brief Map of MEInductLoop by ID
    typedef NamedObjectCont< MEInductLoop*> MELoopDict;
#endif


    /// @brief Vector of MSInductLoop
    typedef std::vector< MSInductLoop*> LoopVect;

    /// @brief Vector of MSE2Collector
    typedef std::vector< MSE2Collector*> E2Vect;

    /// @brief Vector of MSE3Collector
    typedef std::vector< MSE3Collector*> E3Vect;

    /// @brief Vector of MS_E2_ZS_CollectorOverLanes
    typedef std::vector< MS_E2_ZS_CollectorOverLanes* > E2ZSOLVect;

#ifdef HAVE_MESOSIM
    /// @brief Vector of MEInductLoop
    typedef std::vector< MEInductLoop*> MELoopVect;
#endif
    /// @}


public:
    /** @brief Constructor
     */
    MSDetectorControl() throw();


    /** @brief Destructor
     *
     * Deletes all stored detectors.
     */
    ~MSDetectorControl() throw();


    /** @brief Closes the detector outputs
     *
     * Goes through the intervals to write and writes the last step using
     *  the current simulation time as end.
     *
     * @param[in] step The time step (the simulation has ended at)
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void close(SUMOTime step) throw(IOError);


    /// @name Methods for adding detectors that are coupled to an own OutputDevice
    /// @{

#ifdef _MESSAGES
    /** @brief Adds a message induct loop into the containers
     *
     *
     *
     */
    void add(MSMsgInductLoop *msgl, OutputDevice& device, int splInterval) throw(ProcessError);
#endif

    /** @brief Adds a induction loop into the containers
     *
     * The detector is tried to be added into "myLoops". If the detector
     *  is already known (the id was already used for a similar detector),
     *  a ProcessError is thrown.
     *
     * Otherwise, the Detector2File-mechanism is instantiated for the detector.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] il The induction loop to add
     * @param[in] device The device the loop uses
     * @param[in] splInterval The sample interval of the loop
     * @exception ProcessError If the detector is already known
     */
    void add(MSInductLoop *il, OutputDevice& device, int splInterval) throw(ProcessError);


    /** @brief Adds a e2-detector into the containers
     *
     * The detector is tried to be added into "myE2Detectors". If the detector
     *  is already known (the id was already used for a similar detector),
     *  a ProcessError is thrown.
     *
     * Otherwise, the Detector2File-mechanism is instantiated for the detector.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] e2 The e2-detector to add
     * @param[in] device The device the e2-detector uses
     * @param[in] splInterval The sample interval of the e2-detector
     * @exception ProcessError If the detector is already known
     */
    void add(MSE2Collector *e2, OutputDevice& device, int splInterval) throw(ProcessError);


    /** @brief Adds a e2ol-detector into the containers
     *
     * The detector is tried to be added into "myE2OverLanesDetectors".
     *  If the detector is already known (the id was already used for a
     *  similar detector), a ProcessError is thrown.
     *
     * Otherwise, the Detector2File-mechanism is instantiated for the detector.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] e2ol The e2ol-detector to add
     * @param[in] device The device the e2ol-detector uses
     * @param[in] splInterval The sample interval of the e2ol-detector
     * @exception ProcessError If the detector is already known
     */
    void add(MS_E2_ZS_CollectorOverLanes *e2ol, OutputDevice& device,
             int splInterval) throw(ProcessError);


    /** @brief Adds a e3-detector into the containers
     *
     * The detector is tried to be added into "myE3Detectors". If the detector
     *  is already known (the id was already used for a similar detector),
     *  a ProcessError is thrown.
     *
     * Otherwise, the Detector2File-mechanism is instantiated for the detector.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] e3 The e3-detector to add
     * @param[in] device The device the e3-detector uses
     * @param[in] splInterval The sample interval of the e3-detector
     * @exception ProcessError If the detector is already known
     */
    void add(MSE3Collector *e3, OutputDevice& device, int splInterval) throw(ProcessError);


    /** @brief Adds a vytpeprobe into the containers
     *
     * The Detector2File-mechanism is instantiated for the detector.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] vp The vytpeprobe to add
     * @param[in] device The device the vytpeprobe uses
     * @param[in] frequency The frequency of calling this vtypeprobe
     * @exception ProcessError If the detector is already known
     */
    void add(MSVTypeProbe *vp, OutputDevice& device, int frequency) throw(ProcessError);


    /** @brief Adds a routeprobe into the containers
     *
     * The Detector2File-mechanism is instantiated for the detector.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] vp The routeprobe to add
     * @param[in] device The device the routeprobe uses
     * @param[in] frequency The frequency of calling this routeprobe
     * @exception ProcessError If the detector is already known
     */
    void add(MSRouteProbe *vp, OutputDevice& device, int frequency) throw(ProcessError);


#ifdef HAVE_MESOSIM
    /** @brief Adds a mesoscopic induction loop into the containers
     *
     * The detector is tried to be added into "myMesoLoops". If the detector
     *  is already known (the id was already used for a similar detector),
     *  a ProcessError is thrown.
     *
     * Otherwise, the Detector2File-mechanism is instantiated for the detector.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] il The induction loop to add
     * @param[in] device The device the loop uses
     * @param[in] splInterval The sample interval of the loop
     * @exception ProcessError If the detector is already known
     */
    void add(MEInductLoop *il, OutputDevice& device, int splInterval) throw(ProcessError);
#endif
    /// @}



    /// @name Methods for adding detectors that are coupled to an own OutputDevice
    /// @{

    /** @brief Adds a e2-detector coupled to an extern output impulse giver
     *
     * The detector is tried to be added into "myE2Detectors". If the detector
     *  is already known (the id was already used for a similar detector),
     *  a ProcessError is thrown.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] e2 The e2-detector to add
     * @exception ProcessError If the detector is already known
     */
    void add(MSE2Collector *e2) throw(ProcessError);


    /** @brief Adds a e2ol-detector coupled to an extern output impulse giver
     *
     * The detector is tried to be added into "myE2OverLanesDetectors". If the detector
     *  is already known (the id was already used for a similar detector),
     *  a ProcessError is thrown.
     *
     * Please note, that the detector control gets responsible for the detector.
     *
     * @param[in] e2ol The e2ol-detector to add
     * @exception ProcessError If the detector is already known
     */
    void add(MS_E2_ZS_CollectorOverLanes *e2ol) throw(ProcessError);
    /// @}


    /** @brief Resets the interval of an output device
     *
     * This method is used within the TrafficTower-extension of SUMO
     *
     * @param[in] il The detector to reset the interval of
     * @param[in] newinterval The new interval to assign
     */
    void resetInterval(MSDetectorFileOutput *il, SUMOTime newinterval) throw();


    /** @brief Adds one of the detectors as a new MSDetectorFileOutput
     *
     * This method is used within the TrafficTower-extension of SUMO
     *
     * @param[in] det The generator to add
     * @param[in] device The device to use
     * @param[in] interval The sample interval to use
     * @param[in] reinsert Used to determine whether the prolog shall be written (if false)
     * @param[in] onStepBegin If true, writeXMLOutput will be called at interval begin (see writeOutput)
     * @tode Recheck whether this method could be made private/protected
     */
    void addDetectorAndInterval(MSDetectorFileOutput* det,
                                OutputDevice *device, SUMOTime interval,
                                bool reinsert=false,
                                bool onStepBegin=false) throw();



    /// @name Methods for retrieving detectors
    /// @{

    /** @brief Returns the MSInductLoop-container
     *
     * @return The container of MSInductLoops
     */
    const LoopDict &getInductLoops() const throw() {
        return myLoops;
    }


    /** @brief Returns the MSE2Detector-container
     *
     * @return The container of MSE2Detector
     */
    const E2Dict &getE2Detectors() const throw() {
        return myE2Detectors;
    }


    /** @brief Returns the MSE3Detector-container
     *
     * @return The container of MSE3Detector
     */
    const E3Dict &getE3Detectors() const throw() {
        return myE3Detectors;
    }


    /** @brief Returns the MS_E2_ZS_CollectorOverLanes-container
     *
     * @return The container of MS_E2_ZS_CollectorOverLanes
     */
    const E2ZSOLDict &getE2OLDetectors() const throw() {
        return myE2OverLanesDetectors;
    }
    /// @}


    /** @brief Computes detector values
     *
     * Some detectors need to be touched each time step in order to compute
     *  values from the vehicles stored in their containers. This method
     *  goes through all of these detectors and forces a recomputation of
     *  the values.
     *
     * The following detector types need an update:
     * @arg MSE2Collector
     * @arg MSE3Collector
     */
    void updateDetectors(SUMOTime step) throw();


    /** @brief Writes the output to be generated within the given time step
     *
     * Goes through the list of intervals. If one interval has ended within the
     *  given step or if the closing-flag is set and the output was not
     *  written in this step already, the writeXMLOutput method is called
     *  for all MSDetectorFileOutputs within this interval.
     *
     * @param[in] step The current time step
     * @param[in] closing Whether the device is closed
     * @exception IOError If an error on writing occures (!!! not yet implemented)
     */
    void writeOutput(SUMOTime step, bool closing) throw(IOError);


protected:
    /// @name Structures needed for assigning detectors to intervals
    /// @{

#ifdef _MESSAGES
    /// @brief MSMsgInductLoop dictionary
    MsgLoopDict myMsgLoops;
#endif

    /// @brief A pair of a Detector with it's associated file-stream.
    typedef std::pair< MSDetectorFileOutput*, OutputDevice* > DetectorFilePair;

    /// @brief Container holding DetectorFilePair (with the same interval).
    typedef std::vector< DetectorFilePair > DetectorFileVec;

    /// @brief Definition of the interval key
    typedef std::pair<SUMOTime, bool> IntervalsKey;

    /// @brief Association of intervals to DetectorFilePair containers.
    typedef std::map< IntervalsKey, DetectorFileVec > Intervals;
    /// @}

    /**
     * @struct detectorEquals
     * @brief Returns true if detectors are equal.
     *
     * Binary predicate that compares the passed DetectorFilePair's
     * detector to a fixed one. Returns true if detectors are
     * equal. (Used to prevent multiple inclusion of a detector for
     * the same interval.)
     *
     * @see addDetectorAndInterval
     */
struct detectorEquals : public std::binary_function< DetectorFilePair, MSDetectorFileOutput*, bool > {
        /** @brief Returns true if detectors are equal. */
        bool operator()(const DetectorFilePair& pair, const MSDetectorFileOutput* det) const throw() {
            return pair.first == det;
        }
    };

protected:
    /// @name Dictionaries of different detector types
    /// @{

    /// @brief MSInductLoop dictionary
    LoopDict myLoops;

    /// @brief MSE2Collector dictionary
    E2Dict myE2Detectors;

    /// @brief MSE3Collector dictionary
    E3Dict myE3Detectors;

    /// @brief MS_E2_ZS_CollectorOverLanes dictionary
    E2ZSOLDict myE2OverLanesDetectors;

    /// @brief MSVTypeProbe dictionary
    VTypeProbeDict myVTypeProbeDetectors;

    /// @brief MSRouteProbe dictionary
    RouteProbeDict myRouteProbeDetectors;

#ifdef HAVE_MESOSIM
    /// @brief MEInductLoop dictionary
    MELoopDict myMesoLoops;
#endif
    /// @}

    /** @brief Map that hold DetectorFileVec for given intervals. */
    Intervals myIntervals;

    /// @brief The map that holds the last call for each sample interval
    std::map<IntervalsKey, SUMOTime> myLastCalls;


private:
    /// @brief Invalidated copy constructor.
    MSDetectorControl(const MSDetectorControl&);

    /// @brief Invalidated assignment operator.
    MSDetectorControl& operator=(const MSDetectorControl&);


};


#endif

/****************************************************************************/

