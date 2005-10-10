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
// Revision 1.9  2005/10/10 11:56:09  dkrajzew
// reworking the tls-API: made tls-control non-static; made net an element of traffic lights
//
// Revision 1.8  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.7  2005/09/22 13:45:52  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.6  2005/09/15 11:09:53  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.5  2005/05/04 08:22:18  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.4  2005/02/01 10:10:46  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.3  2005/01/27 14:22:45  dkrajzew
// ability to open the complete phase definition added; code style adapted
//
// Revision 1.2  2004/12/10 11:43:57  dksumo
// parametrisation of actuated traffic lights added
//
// Revision 1.1  2004/10/22 12:49:44  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.19  2004/04/02 11:38:28  dkrajzew
// extended traffic lights are now no longer template classes
//
// Revision 1.18  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.17  2004/01/26 07:32:46  dkrajzew
// added the possibility to specify the position (actuated-tlls) / length
//  (agentbased-tlls) of used detectors
//
// Revision 1.16  2004/01/12 15:04:16  dkrajzew
// more wise definition of lane predeccessors implemented
//
// Revision 1.15  2003/11/24 10:21:20  dkrajzew
// some documentation added and dead code removed
//
// Revision 1.14  2003/11/17 07:18:21  dkrajzew
// e2-detector over lanes merger added
//
// Revision 1.13  2003/10/01 13:59:53  dkrajzew
// logic building completed (Julia Ringel)
//
// Revision 1.12  2003/09/23 14:19:59  dkrajzew
// an easier usage of the current actuated phase definition added
//
// Revision 1.11  2003/09/22 12:31:06  dkrajzew
// actuated traffic lights are now derived from simple traffic lights
//
// Revision 1.10  2003/09/17 06:50:45  dkrajzew
// phase definitions extracted from traffic lights;
//  MSActuatedPhaseDefinition is now derived from MSPhaseDefinition
//
// Revision 1.9  2003/07/21 18:13:05  roessel
// Changes due to new MSInductLoop.
//
// Revision 1.8  2003/06/06 10:39:16  dkrajzew
// new usage of MSEventControl applied
//
// Revision 1.7  2003/06/05 16:01:28  dkrajzew
// MSTLLogicControl added
//
// Revision 1.6  2003/05/21 16:20:44  dkrajzew
// further work detectors
//
// Revision 1.5  2003/05/21 15:15:41  dkrajzew
// yellow lights implemented (vehicle movements debugged
//
// Revision 1.4  2003/05/20 09:31:45  dkrajzew
// emission debugged; movement model reimplemented (seems ok);
//  detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.3  2003/04/02 11:44:03  dkrajzew
// continuation of implementation of actuated traffic lights
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <utility>
#include <vector>
#include <bitset>
#include <map>
#include <microsim/MSEventControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include "MSActuatedPhaseDefinition.h"
#include "MSExtendedTrafficLightLogic.h"
#include <microsim/output/MSInductLoop.h>
#include <microsim/MSLaneState.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class NLDetectorBuilder;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class MSActuatedTrafficLightLogic
 * The implementation of a simple traffic light which only switches between
 * it's phases and sets the lights to red in between.
 * Some functions are called with an information about the current step. This
 * is needed as a single logic may be used by many junctions and so the current
 * step is stored within them, not within the logic.
 */
class MSActuatedTrafficLightLogic :
        public MSExtendedTrafficLightLogic
{
public:
    /// Definition of a map from lanes to induct loops lying on them
    typedef std::map<MSLane*, MSInductLoop*> InductLoopMap;

    /// Definition of a map from lanes to lane state detectors lying on them
    typedef std::map<MSLane*, MSLaneState*> LaneStateMap;

public:
    /// constructor
    MSActuatedTrafficLightLogic(MSNet &net, const std::string &id,
        const MSSimpleTrafficLightLogic::Phases &phases,
        size_t step, size_t delay,
        SUMOReal maxGap, SUMOReal passingTime, SUMOReal detectorGap);

    /// Initialises the tls with information about incoming lanes
	void init(NLDetectorBuilder &nb,
		const std::vector<MSLane*> &lanes,
        const std::map<std::string, std::vector<std::string> > &edgeContinuations,
        SUMOReal det_offset);

    /// destructor
    ~MSActuatedTrafficLightLogic();

    /** @brief Switches to the next phase
        Returns the time of the next switch */
    virtual SUMOTime nextPhase();

    /// Returns the duration of the given step
    virtual SUMOTime duration() const;

    /// Returns the index of the phase next to the given phase
    /// and stores the duration of the phase, which was just sent
    /// or stores the activation-time in _lastphase of the phase next
    virtual size_t nextStep();

    /// Desides, whether a phase should be continued by checking the gaps of vehicles having green
    virtual void gapControl();

    // Checkes wheter the tested phase is a neither a yellow nor a allred phase
    virtual bool isGreenPhase() const ;

protected:
    /// Builds the detectors
    virtual void sproutDetectors(NLDetectorBuilder &nb,
        const std::vector<MSLane*> &lanes,
        const std::map<std::string, std::vector<std::string> > &laneContinuations,
        SUMOReal det_offset);

    /// Returns the definition of the current phase
    MSActuatedPhaseDefinition * currentPhaseDef() const;

protected:
    /// A map from lanes to induct loops lying on them
    InductLoopMap myInductLoops;

    /// A map from lanes to lane states lying on them
    LaneStateMap myLaneStates;

    /// information whether the current phase should be lenghtend
    bool _continue;

    /// The maximum gap to check
    SUMOReal myMaxGap;

    /// The passing time used
    SUMOReal myPassingTime;

    /// The detector distance
    SUMOReal myDetectorGap;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

