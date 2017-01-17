/****************************************************************************/
/// @file    MEInductLoop.cpp
/// @author  Daniel Krajzewicz
/// @date    Tue, May 2005
/// @version $Id$
///
// An induction loop for mesoscopic simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MEInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <limits>
#include <utils/common/WrappingCommand.h>
#include <utils/common/ToString.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEventControl.h>
#include <mesosim/MESegment.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/StringUtils.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MEInductLoop::MEInductLoop(const std::string& id,
                           MESegment* s,
                           SUMOReal positionInMeters,
                           const std::string& vTypes) :
    MSDetectorFileOutput(id, vTypes), mySegment(s),
    myPosition(positionInMeters),
    myMeanData(0, mySegment->getLength(), false, 0) {
    myMeanData.setDescription("inductionLoop_" + id);
    s->addDetector(&myMeanData);
}


MEInductLoop::~MEInductLoop() {}


void
MEInductLoop::writeXMLOutput(OutputDevice& dev,
                             SUMOTime startTime, SUMOTime stopTime) {
    mySegment->prepareDetectorForWriting(myMeanData);
    dev.openTag(SUMO_TAG_INTERVAL).writeAttr(SUMO_ATTR_BEGIN, time2string(startTime)).writeAttr(SUMO_ATTR_END, time2string(stopTime));
    dev.writeAttr(SUMO_ATTR_ID, StringUtils::escapeXML(myID)).writeAttr("sampledSeconds", myMeanData.getSamples());
    myMeanData.write(dev, stopTime - startTime, (SUMOReal)mySegment->getEdge().getLanes().size(), -1.0);
    myMeanData.reset();
}


/****************************************************************************/

