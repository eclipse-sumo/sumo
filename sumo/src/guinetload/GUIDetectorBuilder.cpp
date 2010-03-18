/****************************************************************************/
/// @file    GUIDetectorBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, 22 Jul 2003
/// @version $Id$
///
// Builds detectors for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <microsim/MSNet.h>
#include <microsim/output/MSInductLoop.h>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include <guisim/GUIE3Collector.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include "GUIDetectorBuilder.h"

#ifdef HAVE_MESOSIM
#include <mesogui/GUIMEInductLoop.h>
#include <mesosim/MELoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIDetectorBuilder::GUIDetectorBuilder(MSNet &net) throw()
        : NLDetectorBuilder(net) {}


GUIDetectorBuilder::~GUIDetectorBuilder() throw() {}


MSInductLoop *
GUIDetectorBuilder::createInductLoop(const std::string &id,
                                     MSLane *lane, SUMOReal pos) throw() {
    return new GUIInductLoop(id, lane, pos);
}


#ifdef HAVE_MESOSIM
MEInductLoop *
GUIDetectorBuilder::createMEInductLoop(const std::string &id,
                                       MESegment *s, SUMOReal pos) throw() {
    return new GUIMEInductLoop(id, s, pos);
}
#endif


MSE2Collector *
GUIDetectorBuilder::createSingleLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, SUMOReal pos, SUMOReal length,
        SUMOTime haltingTimeThreshold,
        MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw() {
    return new GUI_E2_ZS_Collector(id, usage, lane, pos, length,
                                   haltingTimeThreshold, haltingSpeedThreshold,
                                   jamDistThreshold);

}


MS_E2_ZS_CollectorOverLanes *
GUIDetectorBuilder::createMultiLaneE2Detector(const std::string &id,
        DetectorUsage usage, MSLane *lane, SUMOReal pos,
        SUMOTime haltingTimeThreshold,
        MetersPerSecond haltingSpeedThreshold,
        SUMOReal jamDistThreshold) throw() {
    return new GUI_E2_ZS_CollectorOverLanes(id, usage, lane, pos,
                                            haltingTimeThreshold, haltingSpeedThreshold,
                                            jamDistThreshold);
}


MSE3Collector *
GUIDetectorBuilder::createE3Detector(const std::string &id,
                                     const CrossSectionVector &entries,
                                     const CrossSectionVector &exits,
                                     MetersPerSecond haltingSpeedThreshold,
                                     SUMOTime haltingTimeThreshold) throw() {
    return new GUIE3Collector(id, entries, exits, haltingSpeedThreshold, haltingTimeThreshold);
}



/****************************************************************************/

