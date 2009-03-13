/****************************************************************************/
/// @file    MSUnboundActuatedTrafficLightLogic.h
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
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
#ifndef MSUnboundActuatedTrafficLightLogic_h
#define MSUnboundActuatedTrafficLightLogic_h


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
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include "MSActuatedPhaseDefinition.h"
#include "MSSimpleTrafficLightLogic.h"
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSLaneState.h>


// ===========================================================================
// class declarations
// ===========================================================================
class NLDetectorBuilder;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSUnboundActuatedTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
class MSUnboundActuatedTrafficLightLogic :
            public MSSimpleTrafficLightLogic {
public:
    /// Definition of a map from lanes to induct loops lying on them
    typedef std::map<MSLane*, MSInductLoop*> InductLoopMap;

    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, MSLaneState*> LaneStateMap;

public:
    /// constructor
    MSUnboundActuatedTrafficLightLogic(const std::string &id,
                                       const MSSimpleTrafficLightLogic::Phases &phases,
                                       size_t step, size_t delay,
                                       SUMOReal maxGap, SUMOReal passingTime, SUMOReal detectorGap);

    /// Initialises the tls with information about incoming lanes
    void init(NLDetectorBuilder &nb,
              const std::vector<MSLane*> &lanes,
              std::map<std::string, std::vector<std::string> > &edgeContinuations,
              SUMOReal det_offset);

    /// destructor
    ~MSUnboundActuatedTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    SUMOTime trySwitch(bool isActive);

    /// Returns the duration of the given step
    SUMOTime duration() const;

    /// Desides, whether a phase should be continued by checking the gaps of vehicles having green
    bool gapControl();

    // Checkes wheter the tested phase is a neither a yellow nor a allred phase
    bool isGreenPhase() const ;

protected:
    /// Returns the definition of the current phase
    MSActuatedPhaseDefinition * currentPhaseDef() const;

protected:
    /// A map from lanes to induct loops lying on them
    InductLoopMap myInductLoops;

    /// A map from lanes to lane states lying on them
    LaneStateMap myLaneStates;

    /// information whether the current phase should be lenghtend
    bool myContinue;

    /// The maximum gap to check
    SUMOReal myMaxGap;

    /// The passing time used
    SUMOReal myPassingTime;

    /// The detector distance
    SUMOReal myDetectorGap;

};


#endif

/****************************************************************************/

