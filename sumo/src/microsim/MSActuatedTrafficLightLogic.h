#ifndef MSActuatedTrafficLightLogic_h
#define MSActuatedTrafficLightLogic_h
//---------------------------------------------------------------------------//
//                        MSActuatedTrafficLightLogic.h -
//  The basic traffic light logic
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
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
// Revision 1.2  2003/03/17 14:12:19  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/03 14:56:19  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
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


/* =========================================================================
 * class definitions
 * ======================================================================= */

/**
 * The definition of a single phase.
 * We moved it out of the main class to allow later template
 * parametrisation.
 */
class ActuatedPhaseDefinition {
public:
    /// the duration of the phase
    size_t          duration;

    /// the mask which links are allowed to drive within this phase (green light)
    std::bitset<64>  driveMask;

    /// the mask which vehicles must not drive within this phase (red light)
    std::bitset<64>  breakMask;

    /// The minimum duration of the pahse
    size_t          minDuration;

    /// The maximum duration of the pahse
    size_t          maxDuration;


    /// constructor
    ActuatedPhaseDefinition(size_t durationArg,
        const std::bitset<64> &driveMaskArg, const std::bitset<64> &breakMaskArg,
        size_t minDurationArg, size_t maxDurationArg)
    : duration(durationArg), driveMask(driveMaskArg),
    breakMask(breakMaskArg), minDuration(minDurationArg),
    maxDuration(maxDurationArg)
    { }

    /// destructor
    ~ActuatedPhaseDefinition() { }

private:
    /// invalidated standard constructor
    ActuatedPhaseDefinition();

};


/// definition of a list of phases, being the junction logic
typedef std::vector<ActuatedPhaseDefinition > ActuatedPhases;


/**
 * @class MSActuatedTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
template< class _TInductLoop, class _TLaneState >
class MSActuatedTrafficLightLogic :
        public MSTrafficLightLogic
{
public:
    /// Definition of a map from lanes to induct loops lying on them
    typedef std::map<MSLane*, _TInductLoop*> InductLoopMap;

    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, _TLaneState*> LaneStateMap;

public:
    /// constructor
    MSActuatedTrafficLightLogic(const std::string &id, const ActuatedPhases &phases,
        size_t step, const std::vector<MSLane*> &lanes);

    /// destructor
    ~MSActuatedTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual MSNet::Time nextPhase(MSLogicJunction::InLaneCont &inLanes);

    /// Returns the duration of the given step
    virtual MSNet::Time duration() const;

    /** @brief Applies the right-of-way rules of the phase specified by the second argument to the first argument
        Requests of vehicles which are not allowed to drive as they
        have red light are masked out from the given request */
    virtual void applyPhase(MSLogicJunction::Request &request) const;

    /** Returns the link priorities for the given phase */
    virtual const std::bitset<64> &linkPriorities() const;

    /// Returns the index of the phase next to the given phase
    virtual size_t nextStep();

    /// Returns a vector of build detectors
    MSNet::DetectorCont getDetectorList() const;

protected:
    /// Builds the detectors
    virtual void sproutDetectors(const std::vector<MSLane*> &lanes);

protected:
    /// A map from lanes to induct loops lying on them
    InductLoopMap myInductLoops;

    /// A map from lanes to lane states lying on them
    LaneStateMap myLaneStates;

    /// infomration whether the current phase is the dead-phase
    bool _allRed;

    /// The step within the cycla
    size_t _step;

    /// The phase definitions
    ActuatedPhases _phases;

    /// static container for all lights being set to red
    static std::bitset<64> _allClear;

};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "MSActuatedTrafficLightLogic.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "MSActuatedTrafficLightLogic.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

