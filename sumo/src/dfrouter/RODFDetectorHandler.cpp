/****************************************************************************/
/// @file    RODFDetectorHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A handler for loading detector descriptions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2016 DLR (http://www.dlr.de/) and contributors
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
#include <utils/options/OptionsCont.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/xml/SUMOSAXHandler.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include "RODFDetectorHandler.h"
#include "RODFNet.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODFDetectorHandler::RODFDetectorHandler(RODFNet* optNet, bool ignoreErrors, RODFDetectorCon& con,
        const std::string& file)
    : SUMOSAXHandler(file),
      myNet(optNet), myIgnoreErrors(ignoreErrors), myContainer(con) {}


RODFDetectorHandler::~RODFDetectorHandler() {}


void
RODFDetectorHandler::myStartElement(int element,
                                    const SUMOSAXAttributes& attrs) {
    if (element == SUMO_TAG_DETECTOR_DEFINITION || element == SUMO_TAG_E1DETECTOR || element == SUMO_TAG_INDUCTION_LOOP) {
        try {
            bool ok = true;
            // get the id, report an error if not given or empty...
            std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
            if (!ok) {
                throw ProcessError();
            }
            std::string lane = attrs.get<std::string>(SUMO_ATTR_LANE, id.c_str(), ok);
            if (!ok) {
                throw ProcessError();
            }
            ROEdge* edge = myNet->getEdge(lane.substr(0, lane.rfind('_')));
            int laneIndex = TplConvert::_2intSec(lane.substr(lane.rfind('_') + 1).c_str(), INT_MAX);
            if (edge == 0 || laneIndex >= edge->getLaneNo()) {
                throw ProcessError("Unknown lane '" + lane + "' for detector '" + id + "' in '" + getFileName() + "'.");
            }
            SUMOReal pos = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, id.c_str(), ok);
            std::string mml_type = attrs.getOpt<std::string>(SUMO_ATTR_TYPE, id.c_str(), ok, "");
            if (!ok) {
                throw ProcessError();
            }
            RODFDetectorType type = TYPE_NOT_DEFINED;
            if (mml_type == "between") {
                type = BETWEEN_DETECTOR;
            } else if (mml_type == "source" || mml_type == "highway_source") { // !!! highway-source is legacy (removed accoring output on 06.08.2007)
                type = SOURCE_DETECTOR;
            } else if (mml_type == "sink") {
                type = SINK_DETECTOR;
            }
            RODFDetector* detector = new RODFDetector(id, lane, pos, type);
            if (!myContainer.addDetector(detector)) {
                delete detector;
                throw ProcessError("Could not add detector '" + id + "' (probably the id is already used).");
            }
        } catch (ProcessError& e) {
            if (myIgnoreErrors) {
                WRITE_WARNING(e.what());
            } else {
                throw e;
            }
        }
    }
}



/****************************************************************************/

