#ifndef MSAgentbasedTrafficLightLogic_h
#define MSAgentbasedTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSAgentbasedTrafficLightLogic.h -
//  The basic traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 01. Oct 2003
//  copyright            : (C) 2003 by DLR e.V.
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.13  2004/01/26 07:32:46  dkrajzew
// added the possibility to specify the position (actuated-tlls) / length (agentbased-tlls) of used detectors
//
// Revision 1.12  2004/01/13 08:06:55  dkrajzew
// recent changes applied
//
// Revision 1.11  2004/01/12 15:04:16  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.10  2003/12/04 13:27:10  dkrajzew
// jringels wish for min/max duration applied defaults
//
// Revision 1.7  2003/11/24 10:21:20  dkrajzew
// some documentation added and dead code removed
//
// Revision 1.6  2003/11/17 07:18:21  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.5  2003/11/04 08:55:28  jringel
// implemetation of the agentbased trafficlightlogic
//
//
// Revision 1.5 2003/10/28 jringel
// implemetation of the agent-based logic
//
// Revision 1.4  2003/10/08 14:50:28  dkrajzew
//
// Revision 1.1  2003/10/01 11:24:35  dkrajzew
// agent-based traffic lights added
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include <utility>
#include <vector>
#include <bitset>
#include <map>
#include "MSEventControl.h"
#include "MSNet.h"
#include "MSTrafficLightLogic.h"
#include "MSActuatedPhaseDefinition.h"
#include "MSSimpleTrafficLightLogic.h"
#include "MS_E2_ZS_CollectorOverLanes.h"


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSLane;
class MSAgentbasedPhaseDefinition;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSAgentbasedTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
template< class _TE2_ZS_CollectorOverLanes >
class MSAgentbasedTrafficLightLogic :
        public MSExtendedTrafficLightLogic
{
public:
    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, MS_E2_ZS_CollectorOverLanes*> E2DetectorMap;

    ///stores the detector values of one single phase
    typedef std::deque<double> ValueType;

    ///stores the step of the greenphases and their detector values
    typedef std::map<size_t, ValueType> PhaseValueMap;

    /// stores the mean data of several (numberOfValues) cycles
    typedef std::map<size_t, double> MeanDataMap;

public:
    /// constructor
    MSAgentbasedTrafficLightLogic(const std::string &id,
        const MSSimpleTrafficLightLogic::Phases &phases,
        size_t step, size_t delay);

	void init(
		const std::vector<MSLane*> &lanes,
        std::map<std::string, std::vector<std::string> > &edgeContinuations,
        double det_offset);

    /// destructor
    ~MSAgentbasedTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual MSNet::Time nextPhase();

    /// Returns the duration of the given step
    virtual MSNet::Time duration() ;

    /// Returns the index of the phase next to the given phase
    /// and stores the duration of the phase, which was just sent
    /// or stores the activation-time in _lastphase of the phase next
    virtual size_t nextStep();

    /// Collects the trafficdata
    virtual void collectData();

    /// Aggregates the data of one phase, collected during different cycles
    virtual void aggregateRawData();

    /// Calculates the duration for all real phases except intergreen phases
    virtual void calculateDuration();

    /// Checkes wheter the tested phase is a neither a yellow nor a allred phase
    virtual bool isGreenPhase(const size_t testStep) const ;

protected:
    /// Builds the detectors
    virtual void sproutDetectors(const std::vector<MSLane*> &lanes,
        std::map<std::string, std::vector<std::string> > &laneContinuations,
        double det_offset);

    /// initializes the duration of the phases (except the intergeentimes)
    /// so that the time cycletime tCyle is kept
    virtual void initializeDuration();

    /// lenghtend the actual cycle by an given value
    virtual void lengthenCycleTime(size_t toLenghten);

    /// cuts the actual cycle by an given value
    virtual void cutCycleTime(size_t toCut);

    /// returns the step of the phase with the longest Queue_Lengt_Ahead_Of_Traffic_Lights
    virtual size_t findStepOfMaxValue();

    /// returns the step of the phase with the shortest Queue_Lengt_Ahead_Of_Traffic_Lights
    virtual size_t findStepOfMinValue();

    /// Returns the definition of the current phase
    MSActuatedPhaseDefinition * currentPhaseDef() const;

    /// Returns the value of the detector defined by the given lane and type
	double currentForLane(E2::DetType what,
		MSLane *lane) const;

protected:
	/// A map from lanes to E2-detectors lying on them
	E2DetectorMap myE2Detectors;

    /// A map of the step of the greenphases and their detectorvalues for several (mumberofValues) cycles
    PhaseValueMap myRawDetectorData;

    /// A map of the step of the greenphases and theri aggregated detectordata
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
    double deltaLimit;

};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "MSAgentbasedTrafficLightLogic.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSAgentbasedTrafficLightLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

