/****************************************************************************/
/// @file    RODFDetFlowLoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Eric Nicolay
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A loader for detector flows
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <fstream>
#include <sstream>
#include <utils/importio/LineReader.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/TplConvert.h>
#include <utils/common/UtilExceptions.h>
#include "RODFDetFlowLoader.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
RODFDetFlowLoader::RODFDetFlowLoader(const RODFDetectorCon& dets,
                                     RODFDetectorFlows& into,
                                     SUMOTime startTime, SUMOTime endTime,
                                     SUMOTime timeOffset, SUMOTime timeScale)
    : myStorage(into), myTimeOffset(timeOffset), myTimeScale(timeScale),
      myStartTime(startTime), myEndTime(endTime), myDetectorContainer(dets),
      myHaveWarnedAboutOverridingBoundaries(false), myHaveWarnedAboutPartialDefs(false) {}



RODFDetFlowLoader::~RODFDetFlowLoader() {}


void
RODFDetFlowLoader::read(const std::string& file) {
    LineReader lr(file);
    // parse first line
    myLineHandler.reinit(lr.readLine(), ";", ";", true, true);
    // parse values
    while (lr.hasMore()) {
        std::string line = lr.readLine();
        if (line.find(';') == std::string::npos) {
            continue;
        }
        myLineHandler.parseLine(line);
        try {
            std::string detName = myLineHandler.get("detector");
            if (!myDetectorContainer.knows(detName)) {
                continue;
            }
            const SUMOReal parsedTime = TplConvert::_2SUMOReal((myLineHandler.get("time").c_str())) * myTimeScale - myTimeOffset;
            // parsing as float to handle values which would cause int overflow
            if (parsedTime < myStartTime || parsedTime >= myEndTime) {
                if (!myHaveWarnedAboutOverridingBoundaries) {
                    myHaveWarnedAboutOverridingBoundaries = true;
                    WRITE_WARNING("At least one value lies beyond given time boundaries.");
                }
                continue;
            }
            const SUMOTime time = (SUMOTime)(parsedTime + .5);
            FlowDef fd;
            fd.isLKW = 0;
            fd.qPKW = TplConvert::_2SUMOReal(myLineHandler.get("qpkw").c_str());
            fd.vPKW = 0;
            if (myLineHandler.know("vPKW")) {
                fd.vPKW = TplConvert::_2SUMOReal(myLineHandler.get("vpkw").c_str());
            }
            fd.qLKW = 0;
            if (myLineHandler.know("qLKW")) {
                fd.qLKW = TplConvert::_2SUMOReal(myLineHandler.get("qlkw").c_str());
            }
            fd.vLKW = 0;
            if (myLineHandler.know("vLKW")) {
                fd.vLKW = TplConvert::_2SUMOReal(myLineHandler.get("vlkw").c_str());
            }
            if (fd.qLKW < 0) {
                fd.qLKW = 0;
            }
            if (fd.qPKW < 0) {
                fd.qPKW = 0;
            }
            myStorage.addFlow(detName, time, fd);
            if (!myHaveWarnedAboutPartialDefs && !myLineHandler.hasFullDefinition()) {
                myHaveWarnedAboutPartialDefs = true;
                WRITE_WARNING("At least one line does not contain the correct number of columns.");
            }
            continue;
        } catch (UnknownElement&) {} catch (OutOfBoundsException&) {} catch (NumberFormatException&) {}
        throw ProcessError("The detector-flow-file '" + lr.getFileName() + "' is corrupt;\n"
                           + " The following values must be supplied : 'Detector', 'Time', 'qPKW'\n"
                           + " The according column names must be given in the first line of the file.");
    }
}


/****************************************************************************/

