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
// Revision 1.5  2004/04/02 11:14:36  dkrajzew
// extended traffic lights are no longer template classes
//
// Revision 1.4  2004/03/19 12:56:48  dkrajzew
// porting to FOX
//
// Revision 1.3  2004/01/26 06:49:06  dkrajzew
// work on detectors: e3-detectors loading and visualisation; variable offsets and lengths for lsa-detectors; coupling of detectors to tl-logics
//
// Revision 1.2  2004/01/12 14:44:30  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.1  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <microsim/MSNet.h>
#include <netload/NLDetectorBuilder.h>


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
    GUIDetectorBuilder();

    /// Destructor
    ~GUIDetectorBuilder();

protected:
    /// Creates the instance of an induct loop (gui-version)
    virtual MSInductLoop *createInductLoop(const std::string &id,
        MSLane *lane, double pos);

    /// Creates the instance of a single-lane-e2-detector (gui-version)
    virtual MSE2Collector *createSingleLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, float pos, float length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);

    /// Creates the instance of a multi-lane-e2-detector (gui-version)
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, DetectorUsage usage, MSLane *lane, float pos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);

    /// Creates the instance of an e3-detector (gui version)
    virtual MSE3Collector *createE3Detector(const std::string &id,
        const Detector::CrossSections &entries,
        const Detector::CrossSections &exits,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);


};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/

#endif

// Local Variables:
// mode:C++
//
