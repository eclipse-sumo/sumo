/****************************************************************************/
/// @file    GUIDetectorBuilder.h
/// @author  Daniel Krajzewicz
/// @date    Tue, 22 Jul 2003
/// @version $Id$
///
// A building helper for the detectors
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef GUIDetectorBuilder_h
#define GUIDetectorBuilder_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <microsim/MSNet.h>
#include <netload/NLDetectorBuilder.h>


// ===========================================================================
// class declarations
// ===========================================================================
class MSDetectorControl;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class GUIDetectorBuilder
 * This class builds gui-versions of detectors from their descriptions
 */
class GUIDetectorBuilder : public NLDetectorBuilder
{
public:
    /// Constructor
    GUIDetectorBuilder(MSNet &net);

    /// Destructor
    ~GUIDetectorBuilder();

protected:
    /// Creates the instance of an induct loop (gui-version)
    virtual MSInductLoop *createInductLoop(const std::string &id,
                                           MSLane *lane, SUMOReal pos, int splInterval);

    /// Creates the instance of a single-lane-e2-detector (gui-version)
    virtual MSE2Collector *createSingleLaneE2Detector(const std::string &id,
            DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
            SUMOReal haltingTimeThreshold,
            MSUnit::MetersPerSecond haltingSpeedThreshold,
            SUMOReal jamDistThreshold,
            SUMOTime deleteDataAfterSeconds);

    /// Creates the instance of a multi-lane-e2-detector (gui-version)
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, DetectorUsage usage, MSLane *lane, SUMOReal pos,
        SUMOReal haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// Creates the instance of an e3-detector (gui version)
    virtual MSE3Collector *createE3Detector(const std::string &id,
                                            const CrossSectionVector &entries,
                                            const CrossSectionVector &exits,
                                            SUMOTime haltingTimeThreshold,
                                            MSUnit::MetersPerSecond haltingSpeedThreshold);


};


#endif

/****************************************************************************/

