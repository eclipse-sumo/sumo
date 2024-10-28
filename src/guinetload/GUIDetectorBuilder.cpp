/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    GUIDetectorBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 22 Jul 2003
///
// Builds detectors for guisim
/****************************************************************************/
#include <config.h>

#include <string>
#include <iostream>
#include <guisim/GUIInductLoop.h>
#include <guisim/GUIE2Collector.h>
#include <guisim/GUIE3Collector.h>
#include <guisim/GUIInstantInductLoop.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/output/MSInductLoop.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include "GUIDetectorBuilder.h"

#include <mesogui/GUIMEInductLoop.h>
#include <mesosim/MELoop.h>


// ===========================================================================
// method definitions
// ===========================================================================
GUIDetectorBuilder::GUIDetectorBuilder(MSNet& net)
    : NLDetectorBuilder(net) {}


GUIDetectorBuilder::~GUIDetectorBuilder() {}


MSDetectorFileOutput*
GUIDetectorBuilder::createInductLoop(const std::string& id,
                                     MSLane* lane, double pos, double length,
                                     const std::string name, const std::string& vTypes,
                                     const std::string& nextEdges,
                                     int detectPersons, bool show) {
    if (MSGlobals::gUseMesoSim) {
        return new GUIMEInductLoop(id, MSGlobals::gMesoNet->getSegmentForEdge(lane->getEdge(), pos), pos, name, vTypes, nextEdges, detectPersons, show);
    } else {
        return new GUIInductLoop(id, lane, pos, length, name, vTypes, nextEdges, detectPersons, show);
    }
}


MSDetectorFileOutput*
GUIDetectorBuilder::createInstantInductLoop(const std::string& id,
        MSLane* lane, double pos, const std::string& od, const std::string name, const std::string& vTypes, const std::string& nextEdges) {
    return new GUIInstantInductLoop(id, OutputDevice::getDevice(od), lane, pos, name, vTypes, nextEdges);
}


MSE2Collector*
GUIDetectorBuilder::createE2Detector(const std::string& id,
                                     DetectorUsage usage, MSLane* lane, double pos, double endPos, double length,
                                     SUMOTime haltingTimeThreshold, double haltingSpeedThreshold, double jamDistThreshold,
                                     const std::string name, const std::string& vTypes,
                                     const std::string& nextEdges,
                                     int detectPersons, bool showDetector) {
    return new GUIE2Collector(id, usage, lane, pos, endPos, length, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, name, vTypes, nextEdges, detectPersons, showDetector);
}

MSE2Collector*
GUIDetectorBuilder::createE2Detector(const std::string& id,
                                     DetectorUsage usage, std::vector<MSLane*> lanes, double pos, double endPos,
                                     SUMOTime haltingTimeThreshold, double haltingSpeedThreshold, double jamDistThreshold,
                                     const std::string name, const std::string& vTypes,
                                     const std::string& nextEdges,
                                     int detectPersons, bool showDetector) {
    return new GUIE2Collector(id, usage, lanes, pos, endPos, haltingTimeThreshold, haltingSpeedThreshold, jamDistThreshold, name, vTypes, nextEdges, detectPersons, showDetector);
}

MSDetectorFileOutput*
GUIDetectorBuilder::createE3Detector(const std::string& id,
                                     const CrossSectionVector& entries,
                                     const CrossSectionVector& exits,
                                     double haltingSpeedThreshold,
                                     SUMOTime haltingTimeThreshold,
                                     const std::string name, const std::string& vTypes,
                                     const std::string& nextEdges,
                                     int detectPersons, bool openEntry, bool expectArrival) {
    return new GUIE3Collector(id, entries, exits, haltingSpeedThreshold, haltingTimeThreshold, name, vTypes, nextEdges, detectPersons, openEntry, expectArrival);
}


/****************************************************************************/
