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
// Revision 1.2  2004/01/12 14:44:30  dkrajzew
// handling of e2-detectors within the gui added
//
// Revision 1.1  2003/07/22 14:58:33  dkrajzew
// changes due to new detector handling
//
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
        MSLane *lane, float pos, float length,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);

    /// Creates the instance of a multi-lane-e2-detector (gui-version)
    virtual MS_E2_ZS_CollectorOverLanes *createMultiLaneE2Detector(
        const std::string &id, MSLane *lane, float pos,
        MSUnit::Seconds haltingTimeThreshold,
        MSUnit::MetersPerSecond haltingSpeedThreshold,
        MSUnit::Meters jamDistThreshold,
        MSUnit::Seconds deleteDataAfterSeconds);


};

/**************** DO NOT DECLARE ANYTHING AFTER THE INCLUDE ****************/
//#ifndef DISABLE_INLINE
//#include "GUIDetectorBuilder.icc"
//#endif

#endif

// Local Variables:
// mode:C++
//
