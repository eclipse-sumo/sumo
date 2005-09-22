#ifndef GUIDetectorBuilder_h
#define GUIDetectorBuilder_h
/***************************************************************************
                          GUIDetectorBuilder.h
                          A building helper for the detectors
                             -------------------
    begin                : Tue, 22 Jul 2003
    copyright            : (C) 2003 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
// $Log$
// Revision 1.10  2005/09/22 13:39:19  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.9  2005/09/15 11:06:03  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.8  2005/05/04 07:56:00  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.7  2004/12/16 12:14:52  dkrajzew
// got rid of an unnecessary detector parameter/debugging
//
// Revision 1.6  2004/11/23 10:12:27  dkrajzew
// new detectors usage applied
//
// Revision 1.5  2004/04/02 11:14:36  dkrajzew
// extended traffic lights are no longer template classes
//
// Revision 1.4  2004/03/19 12:56:48  dkrajzew
// porting to FOX
//
// Revision 1.3  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation;
//  variable offsets and lengths for lsa-detectors;
//  coupling of detectors to tl-logics
//
// Revision 1.2  2004/01/12 14:44:30  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.1  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <string>
#include <microsim/MSNet.h>
#include <netload/NLDetectorBuilder.h>


/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSDetectorControl;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class GUIDetectorBuilder
 * This class builds gui-versions of detectors from their descriptions
 */
class GUIDetectorBuilder : public NLDetectorBuilder {
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
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// Creates the instance of a multi-lane-e2-detector (gui-version)
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, DetectorUsage usage, MSLane *lane, SUMOReal pos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        SUMOTime deleteDataAfterSeconds);

    /// Creates the instance of an e3-detector (gui version)
    virtual MSE3Collector *createE3Detector(const std::string &id,
        const Detector::CrossSections &entries,
        const Detector::CrossSections &exits,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        SUMOTime deleteDataAfterSeconds);


};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
