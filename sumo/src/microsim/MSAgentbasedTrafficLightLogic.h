#ifndef MSAgentbasedTrafficLightLogic_h
#define MSAgentbasedTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSAgentbasedTrafficLightLogic.h -
//  The basic traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Wed, 01. Oct 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.3  2003/10/06 07:40:55  dkrajzew
// lanes are saved for further purposes, now
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
#include "MS_E2_ZS_Collector.h"

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
template< class _TInductLoop, class _TLaneState, class _TE2_ZS_Collector >
class MSAgentbasedTrafficLightLogic :
        public MSSimpleTrafficLightLogic
{
public:
    /// Definition of a map from lanes to induct loops lying on them
    typedef std::map<MSLane*, _TInductLoop*> InductLoopMap;

    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, MSLaneState*> LaneStateMap;

    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, MS_E2_ZS_Collector*> E2DetectorMap;

public:
    /// constructor
    MSAgentbasedTrafficLightLogic(const std::string &id,
        const MSSimpleTrafficLightLogic::Phases &phases,
        size_t step, const std::vector<MSLane*> &lanes, size_t delay);

    /// destructor
    ~MSAgentbasedTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual MSNet::Time nextPhase();

    /// Returns the duration of the given step
    virtual MSNet::Time duration() const;

    /// Returns the index of the phase next to the given phase
    /// and stores the duration of the phase, which was just sent
    /// or stores the activation-time in _lastphase of the phase next
    virtual size_t nextStep();

    /// Desides, whether a phase should be continued by checking the gaps of vehicles having green
    virtual bool gapControl();

protected:
    /// Builds the detectors
    virtual void sproutDetectors(const std::vector<MSLane*> &lanes);

    MSActuatedPhaseDefinition * currentPhaseDef();

	double currentForLane(MS_E2_ZS_Collector::DetType what,
		MSLane *lane) const;

	double currentForLane(MS_E2_ZS_Collector::DetType what,
		MSUnit::Seconds lanstNSeconds, MSLane *lane) const;

protected:
    /// A map from lanes to induct loops lying on them
    InductLoopMap myInductLoops;

    /// A map from lanes to lane states lying on them
    LaneStateMap myLaneStates;

	/// A map from lanes to E2-detectors lying on them
	E2DetectorMap myE2Detectors;

    /// information whether the current phase should be lenghtend
    bool _continue;

    std::vector<MSLane*> _lanes;

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

