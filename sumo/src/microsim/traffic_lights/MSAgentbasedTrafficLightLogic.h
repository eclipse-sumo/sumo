/****************************************************************************/
/// @file    MSAgentbasedTrafficLightLogic.h
/// @author  Julia Ringel
/// @date    Wed, 01. Oct 2003
/// @version $Id$
///
// An agentbased traffic light logic
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
#ifndef MSAgentbasedTrafficLightLogic_h
#define MSAgentbasedTrafficLightLogic_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utility>
#include <vector>
#include <bitset>
#include <map>
#include <microsim/MSEventControl.h>
#include "MSTrafficLightLogic.h"
#include "MSSimpleTrafficLightLogic.h"
#include <microsim/output/MS_E2_ZS_CollectorOverLanes.h>
#include "MSSimpleTrafficLightLogic.h"


// ===========================================================================
// class declarations
// ===========================================================================
class MSLane;
class MSAgentbasedPhaseDefinition;
class NLDetectorBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSAgentbasedTrafficLightLogic
 * @brief An agentbased traffic light logic
 */
class MSAgentbasedTrafficLightLogic :
            public MSSimpleTrafficLightLogic {
public:
    /// @brief Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, MS_E2_ZS_CollectorOverLanes*> E2DetectorMap;

    /// @brief Definition of a map which stores the detector values of one single phase
    typedef std::deque<SUMOReal> ValueType;

    /// @brief Definition of a map which stores the step of the greenphases and their detector values
    typedef std::map<unsigned int, ValueType> PhaseValueMap;

    /// @brief Definition of a map which stores the mean data of several (numberOfValues) cycles
    typedef std::map<unsigned int, SUMOReal> MeanDataMap;

public:
    /** @brief Constructor
     * @param[in] tlcontrol The tls control responsible for this tls
     * @param[in] id This tls' id
     * @param[in] subid This tls' sub-id (program id)
     * @param[in] phases Definitions of the phases
     * @param[in] step The initial phase index
     * @param[in] delay The time to wait before the first switch
     * @param[in] learnHorizon !!!
     * @param[in] decHorizon !!!
     * @param[in] minDiff !!!
     * @param[in] tcycle !!!
     */
    MSAgentbasedTrafficLightLogic(MSTLLogicControl &tlcontrol,
                                  const std::string &id, const std::string &subid,
                                  const MSSimpleTrafficLightLogic::Phases &phases,
                                  unsigned int step, SUMOTime delay, int learnHorizon, int decHorizon,
                                  SUMOReal minDiff, int tcycle) throw();


    /** @brief Initialises the tls with information about incoming lanes
     * @param[in] nb The detector builder
     * @param[in] edgeContinuations Information about edge predecessors/successors
     * @exception ProcessError If something fails on initialisation
     */
    void init(NLDetectorBuilder &nb) throw(ProcessError);


    /// @brief Destructor
    ~MSAgentbasedTrafficLightLogic() throw();



    /// @name Switching and setting current rows
    /// @{

    /** @brief Switches to the next phase
     * @param[in] isActive Whether this program is the currently used one
     * @return The time of the next switch
     * @see MSTrafficLightLogic::trySwitch
     */
    SUMOTime trySwitch(bool isActive) throw();
    /// @}


protected:
    /// @name "agentbased" algorithm methods
    /// @{

    /** @brief Returns the index of the phase next to the given phase
     *
     * Stores the duration of the phase, which was just sent
     *  or stores the activation-time in myLastphase of the phase next
     * @return The index of the next step
     */
    unsigned int nextStep() throw();


    /** @brief Collects the traffic data
     */
    void collectData() throw();


    /** @brief Aggregates the data of one phase, collected during different cycles
     */
    void aggregateRawData() throw();


    /** @brief Calculates the duration for all real phases except intergreen phases
     */
    void calculateDuration() throw();


    /** @brief lenghtens the actual cycle by an given value
     * @param[in] toLenghten The time increase
     */
    void lengthenCycleTime(unsigned int toLenghten) throw();


    /** @brief cuts the actual cycle by an given value
     * @param[in] toCut The time decrease
     */
    void cutCycleTime(unsigned int toCut) throw();


    /** @brief Returns the step of the phase with the longest Queue_Lengt_Ahead_Of_Traffic_Lights
     * @return Which step has the longest queue
     */
    unsigned int findStepOfMaxValue() const throw();


    /** @brief Returns the step of the phase with the shortest Queue_Lengt_Ahead_Of_Traffic_Lights
     * @return Which step has the shortest queue
     */
    unsigned int findStepOfMinValue() const throw();
    /// @}


protected:
    /// @brief A map from lanes to E2-detectors lying on them
    E2DetectorMap myE2Detectors;

    /// @brief A map of the step of the greenphases and their detectorvalues for several (mumberofValues) cycles
    PhaseValueMap myRawDetectorData;

    /// @brief A map of the step of the greenphases and their aggregated detectordata
    MeanDataMap myMeanDetectorData;

    /** @brief the interval in which the traffic light can make a decision
     *
     * The interval is given in integer numbers of cycles */
    unsigned int tDecide;

    /// @brief The number of cycles, before the last decision was made
    unsigned int tSinceLastDecision;

    /// @brief Stores the step of the phase, when the last decision was made
    unsigned int stepOfLastDecision;

    /** @brief The number of detector values whivh is considered to make a decision
     *
     * it's only possible to get one value per cycle per greenphase */
    unsigned int numberOfValues;

    /// @brief The cycletime of the trafficlight
    unsigned int tCycle;

    /* @brief The minimum difference between the shortest and the longest que
     *
     * Queue_Lengt_Ahead_Of_Traffic_Lights of a phase before greentime is given
     *  from the phase with the shortest Queue_Lengt_Ahead_Of_Traffic_Lights to the phase with
     *  the longest Queue_Lengt_Ahead_Of_Traffic_Lights */
    SUMOReal deltaLimit;

};


#endif

/****************************************************************************/

