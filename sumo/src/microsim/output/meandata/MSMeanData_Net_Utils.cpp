/****************************************************************************/
/// @file    MSMeanData_Net_Utils.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 10.05.2004
/// @version $Id$
///
// Utilities for building mean data output
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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

#include "MSMeanData_Net.h"
#include "MSMeanData_Net_Utils.h"
#include <cassert>
#include <algorithm>
#include <set>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/ToString.h>
#include <microsim/MSEdgeControl.h>
#include <utils/iodevices/OutputDevice.h>
#include <microsim/output/MSDetector2File.h>

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
MSMeanData_Net_Cont
MSMeanData_Net_Utils::buildList(MSDetector2File &det2file,
                                MSEdgeControl &ec,
                                std::vector<int> dumpMeanDataIntervalls,
                                std::string baseNameDumpFiles,
                                std::vector<int> laneDumpMeanDataIntervalls,
                                std::string baseNameLaneDumpFiles,
                                const std::vector<int> &dumpBegins,
                                const std::vector<int> &dumpEnds)
{
    // check constraints
    if (dumpBegins.size()!=dumpEnds.size()) {
        throw ProcessError("The number of entries in dump-begins must be the same as in dump-ends.");
    }
    size_t noConstraints = dumpBegins.size();
    for (size_t i=0; i<noConstraints; i++) {
        if (dumpBegins[i]>=dumpEnds[i]) {
            throw ProcessError("The dump-begin at position " + toString(i+1) + " is not smaller than the according dump-end.");
        }
    }
    // build mean data
    MSMeanData_Net_Cont ret;
    if (dumpMeanDataIntervalls.size() > 0) {
        MSMeanData_Net_Cont tmp =
            buildList(det2file, ec,
                      dumpMeanDataIntervalls, baseNameDumpFiles,
                      dumpBegins, dumpEnds, false);
        copy(tmp.begin(), tmp.end(), back_inserter(ret));
    }
    if (laneDumpMeanDataIntervalls.size() > 0) {
        MSMeanData_Net_Cont tmp =
            buildList(det2file, ec,
                      laneDumpMeanDataIntervalls, baseNameLaneDumpFiles,
                      dumpBegins, dumpEnds, true);
        copy(tmp.begin(), tmp.end(), back_inserter(ret));
    }
    return ret;
}


MSMeanData_Net_Cont
MSMeanData_Net_Utils::buildList(MSDetector2File &det2file,
                                MSEdgeControl &ec,
                                std::vector<int> dumpMeanDataIntervalls,
                                std::string baseNameDumpFiles,
                                const std::vector<int> &dumpBegins,
                                const std::vector<int> &dumpEnds,
                                bool useLanes)
{
    MSMeanData_Net_Cont ret;
    if (dumpMeanDataIntervalls.size() > 0) {
        dumpMeanDataIntervalls = buildUniqueList(dumpMeanDataIntervalls);
        sort(dumpMeanDataIntervalls.begin(),
             dumpMeanDataIntervalls.end());

        // Prepare MeanData container, e.g. assign intervals and open files.
        for (std::vector<int>::iterator it =
                    dumpMeanDataIntervalls.begin();
                it != dumpMeanDataIntervalls.end(); ++it) {

            string fileName   = baseNameDumpFiles + "_" + toString(*it) + ".xml";
            OutputDevice* dev = &OutputDevice::getDevice(fileName);
            MSMeanData_Net *det =
                new MSMeanData_Net(*it, ret.size(), ec,
                                   dumpBegins, dumpEnds, useLanes, true);
            ret.push_back(det);
            det2file.addDetectorAndInterval(det, dev, *it);
        }
    }
    return ret;
}


std::vector<int>
MSMeanData_Net_Utils::buildUniqueList(
    std::vector<int> dumpMeanDataIntervalls)
{
    vector<int> ret;
    set<int> u;
    copy(dumpMeanDataIntervalls.begin(), dumpMeanDataIntervalls.end(),
         inserter(u, u.begin()));
    if (dumpMeanDataIntervalls.size()!=u.size()) {
        WRITE_WARNING("Removed duplicate dump-intervalls");
    }
    copy(u.begin(), u.end(), back_inserter(ret));
    return ret;
}



/****************************************************************************/

