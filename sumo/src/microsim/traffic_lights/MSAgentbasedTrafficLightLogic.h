/****************************************************************************/
/// @file    MSAgentbasedTrafficLightLogic.h
/// @author  Julia Ringel
/// @date    Wed, 01. Oct 2003
/// @version $Id$
///
// The basic traffic light logic
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
#include "MSActuatedPhaseDefinition.h"
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
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
class MSAgentbasedTrafficLightLogic :
            public MSSimpleTrafficLightLogic {
public:
    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, MS_E2_ZS_CollectorOverLanes*> E2DetectorMap;

    ///stores the detector values of one single phase
    typedef std::deque<SUMOReal> ValueType;

    ///stores the step of the greenphases and their detector values
    typedef std::map<size_t, ValueType> PhaseValueMap;

    /// stores the mean data of several (numberOfValues) cycles
    typedef std::map<size_t, SUMOReal> MeanDataMap;

public:
    /// constructor
    MSAgentbasedTrafficLightLogic(MSNet &net, MSTLLogicControl &tlcontrol,
                                  const std::string &id, const std::string &subid,
                                  const MSSimpleTrafficLightLogic::Phases &phases,
                                  unsigned int step, SUMOTime delay, int learnHorizon, int decHorizon,
                                  SUMOReal minDiff, int tcycle);

    /// Initialises the tls with information about incoming lanes
    void init(NLDetectorBuilder &nb,
              const MSEdgeContinuations &edgeContinuations);

    /// destructor
    ~MSAgentbasedTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    SUMOTime trySwitch(bool isActive);

protected:
    /// Returns the index of the phase next to the given phase
    /// and stores the duration of the phase, which was just sent
    /// or stores the activation-time in myLastphase of the phase next
    size_t nextStep();

    /// Collects the trafficdata
    void collectData();

    /// Aggregates the data of one phase, collected during different cycles
    void aggregateRawData();

    /// Calculates the duration for all real phases except intergreen phases
    void calculateDuration();

    /// lenghtend the actual cycle by an given value
    void lengthenCycleTime(size_t toLenghten);

    /// cuts the actual cycle by an given value
    void cutCycleTime(size_t toCut);

    /// returns the step of the phase with the longest Queue_Lengt_Ahead_Of_Traffic_Lights
    size_t findStepOfMaxValue();

    /// returns the step of the phase with the shortest Queue_Lengt_Ahead_Of_Traffic_Lights
    size_t findStepOfMinValue();

    /// Returns the definition of the current phase
    MSActuatedPhaseDefinition * currentPhaseDef() const;
    /*
        /// Returns the value of the detector defined by the given lane and type
        SUMOReal currentForLane(E2::DetType what,
                                MSLane *lane) const;
    */
protected:
    /// A map from lanes to E2-detectors lying on them
    E2DetectorMap myE2Detectors;

    /// A map of the step of the greenphases and their detectorvalues for several (mumberofValues) cycles
    PhaseValueMap myRawDetectorData;

    /// A map of the step of the greenphases and their aggregated detectordata
    MeanDataMap myMeanDetectorData;

    /** @brief the interval in which the trafficlight can make a decision
        the interval is given in integer numbers of cycles */
    size_t tDecide;

    /// the number of cycles, before the last decision was made
    size_t tSinceLastDecision;

    /// stores the step of the phase, when the last decision was made
    size_t stepOfLastDecision;

    /// the number of detector values whivh is considered to make a decision
    /// it's only possible to get one value per cycle per greenphase
    size_t numberOfValues;

    /// the cycletime of the trafficlight
    size_t tCycle;

    /* the minimum difference between the shortest and the longest
    Queue_Lengt_Ahead_Of_Traffic_Lights of a phase before greentime is given
    from the phase with the shortest Queue_Lengt_Ahead_Of_Traffic_Lights to the phase with
    the longest Queue_Lengt_Ahead_Of_Traffic_Lights*/
    SUMOReal deltaLimit;

};


#endif

/****************************************************************************/

