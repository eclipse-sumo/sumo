/****************************************************************************/
/// @file    GUIDetectorBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 22 Jul 2003
/// @version $Id$
///
// Builds detectors for guisim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
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
#include <guisim/GUIInductLoop.h>
#include <guisim/GUI_E2_ZS_Collector.h>
#include <guisim/GUI_E2_ZS_CollectorOverLanes.h>
#include <guisim/GUIE3Collector.h>
#include <guisim/GUIInstantInductLoop.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSInductLoop.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include "GUIDetectorBuilder.h"

#ifdef HAVE_INTERNAL
#include <mesogui/GUIMEInductLoop.h>
#include <mesosim/MELoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
GUIDetectorBuilder::GUIDetectorBuilder(MSNet& net)
    : NLDetectorBuilder(net) {}


GUIDetectorBuilder::~GUIDetectorBuilder() {}


MSDetectorFileOutput*
GUIDetectorBuilder::createInductLoop(const std::string& id,
                                     MSLane* lane, SUMOReal pos, bool splitByType) {
    return new GUIInductLoop(id, lane, pos, splitByType);
}


MSDetectorFileOutput*
GUIDetectorBuilder::createInstantInductLoop(const std::string& id,
        MSLane* lane, SUMOReal pos, const std::string& od) {
    return new GUIInstantInductLoop(id, OutputDevice::getDevice(od), lane, pos);
}


#ifdef HAVE_INTERNAL
MEInductLoop*
GUIDetectorBuilder::createMEInductLoop(const std::string& id,
                                       MESegment* s, SUMOReal pos) {
    return new GUIMEInductLoop(id, s, pos);
}
#endif


MSDetectorFileOutput*
GUIDetectorBuilder::createSingleLaneE2Detector(const std::string& id,
        DetectorUsage usage, MSLane* lane, SUMOReal pos, SUMOReal length,
        SUMOTime haltingTimeThreshold,
        SUMOReal haltingSpeedThreshold,
        SUMOReal jamDistThreshold) {
    return new GUI_E2_ZS_Collector(id, usage, lane, pos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold);
}


MSDetectorFileOutput*
GUIDetectorBuilder::createMultiLaneE2Detector(const std::string& id,
        DetectorUsage usage, MSLane* lane, SUMOReal pos,
        SUMOTime haltingTimeThreshold,
        SUMOReal haltingSpeedThreshold,
        SUMOReal jamDistThreshold) {
    return new GUI_E2_ZS_CollectorOverLanes(id, usage, lane, pos, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold);
}


MSDetectorFileOutput*
GUIDetectorBuilder::createE3Detector(const std::string& id,
                                     const CrossSectionVector& entries,
                                     const CrossSectionVector& exits,
                                     SUMOReal haltingSpeedThreshold,
                                     SUMOTime haltingTimeThreshold) {
    return new GUIE3Collector(id, entries, exits, haltingSpeedThreshold, haltingTimeThreshold);
}



/****************************************************************************/

