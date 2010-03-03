/****************************************************************************/
/// @file    RODFDetFlowLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 16.03.2006
/// @version $Id$
///
// A loader for detector flows
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
#include <fstream>
#include <sstream>
#include <utils/importio/LineReader.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/IDSupplier.h>
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
RODFDetFlowLoader::RODFDetFlowLoader(const RODFDetectorCon &dets,
                                     RODFDetectorFlows &into,
                                     SUMOTime startTime, SUMOTime endTime,
                                     int timeOffset) throw()
        : myStorage(into), myTimeOffset(timeOffset),
        myStartTime(startTime/60), myEndTime((endTime+59)/60), myDetectorContainer(dets),
        myHaveWarnedAboutOverridingBoundaries(false) {}



RODFDetFlowLoader::~RODFDetFlowLoader() throw() {}


void
RODFDetFlowLoader::read(const std::string &file) throw(IOError, ProcessError) {
    LineReader lr(file);
    // parse first line
    myLineHandler.reinit(lr.readLine(), ";", ";", true, true);
    // parse values
    while (lr.hasMore()) {
        string line = lr.readLine();
        if (line.find(';')==string::npos) {
            continue;
        }
        myLineHandler.parseLine(line);
        try {
            string detName = myLineHandler.get("detector");
            if (!myDetectorContainer.knows(detName)) {
                continue;
            }
            int time = TplConvert<char>::_2int((myLineHandler.get("time").c_str()));
            time -= myTimeOffset;
            if (time<myStartTime||time>myEndTime) {
                if (!myHaveWarnedAboutOverridingBoundaries) {
                    myHaveWarnedAboutOverridingBoundaries = true;
                    MsgHandler::getWarningInstance()->inform("At least one value lies beyond given time boundaries.");
                }
                continue;
            }
            FlowDef fd;
            fd.isLKW = 0;
            fd.qPKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("qpkw").c_str());
            fd.vPKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("vpkw").c_str());
            fd.qLKW = 0;
            if (myLineHandler.know("qLKW")) {
                fd.qLKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("qlkw").c_str());
            }
            fd.vLKW = 0;
            if (myLineHandler.know("vLKW")) {
                fd.vLKW = TplConvert<char>::_2SUMOReal(myLineHandler.get("vlkw").c_str());
            }
            if (fd.qLKW<0) {
                fd.qLKW = 0;
            }
            if (fd.qPKW<0) {
                fd.qPKW = 0;
            }
            myStorage.addFlow(detName, time, fd);
            continue;
        } catch (UnknownElement &) {} catch (OutOfBoundsException &) {} catch (NumberFormatException &) {}
        throw ProcessError("The detector-flow-file '" + lr.getFileName() + "' is corrupt;\n"
                           + " The following values must be supplied : 'Detector', 'Time', 'qPKW', 'vPKW'\n"
                           + " The according column names must be given in the first line of the file.");
    }
}


/****************************************************************************/

