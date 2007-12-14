/****************************************************************************/
/// @file    MSDetector2File.cpp
/// @author  Christian Roessel
/// @date    Wed Aug 6 16:16:26 2003
/// @version $Id$
///
//  missingDescription
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

#include "MSDetector2File.h"
#include <microsim/MSEventControl.h>
#include <microsim/MSNet.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <string>
#include <cassert>
#include <utility>
#include <iostream>
#include <utils/common/OneArgumentCommand.h>
#include <sstream>

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
MSDetector2File::MSDetector2File()
{}


MSDetector2File::~MSDetector2File(void)
{}


void
MSDetector2File::close(SUMOTime step)
{
    // flush the last values
    Intervals::iterator it;
    for (it = myIntervals.begin(); it != myIntervals.end(); ++it) {
        writeOutput(step, true);
    }
    // [...] files are closed on another place [...]
    //
    myIntervals.clear();
}


void
MSDetector2File::addDetectorAndInterval(MSDetectorFileOutput* det,
                                        OutputDevice *device,
                                        SUMOTime interval,
                                        bool reinsert)
{
    IntervalsKey key = interval;
    Intervals::iterator it = myIntervals.find(key);
    // Add command for given key only once to MSEventControl...
    if (it == myIntervals.end()) {
        // set the 
        DetectorFileVec detAndFileVec;
        detAndFileVec.push_back(make_pair(det, device));
        myIntervals.insert(make_pair(key, detAndFileVec));
        myLastCalls[interval] = OptionsCont::getOptions().getInt("begin");
    } else {
        DetectorFileVec& detAndFileVec = it->second;
        if (find_if(detAndFileVec.begin(), detAndFileVec.end(), bind2nd(detectorEquals(), det)) == detAndFileVec.end()) {
            detAndFileVec.push_back(make_pair(det, device));
        } else {
            // detector already in container. Don't add several times
            WRITE_WARNING("MSDetector2File::addDetectorAndInterval: detector already in container. Ignoring.");
            return;
        }
    }
    if (!reinsert) {
        det->writeXMLDetectorProlog(*device);
    }
}


void 
MSDetector2File::writeOutput(SUMOTime step, bool closing)
{
    for(Intervals::iterator i=myIntervals.begin(); i!=myIntervals.end(); ++i) {
        MSUnit::IntSteps interval = (*i).first;
        if(myLastCalls[interval]+interval-1<=step || (closing && myLastCalls[interval]<step) ) {
            DetectorFileVec dfVec = (*i).second;
            SUMOTime stopTime = step;
            SUMOTime startTime = myLastCalls[interval];
            // check whether at the end the output was already generated
            for (DetectorFileVec::iterator it = dfVec.begin(); it!=dfVec.end(); ++it) {
                MSDetectorFileOutput* det = it->first;
                det->writeXMLOutput(*(it->second), startTime, stopTime);
            }
            myLastCalls[interval] = stopTime + 1;
        }
    }
}


void
MSDetector2File::resetInterval(MSDetectorFileOutput* det,
                               SUMOTime newinterval)
{
    for (Intervals::iterator i=myIntervals.begin(); i!=myIntervals.end(); ++i) {
        DetectorFileVec &dets = (*i).second;
        for (DetectorFileVec::iterator j=dets.begin(); j!=dets.end(); ++j) {
            DetectorFilePair &dvp = *j;
            if (dvp.first==det) {
                DetectorFilePair dvpu = *j;
                dets.erase(j);
                addDetectorAndInterval(dvpu.first, dvpu.second, newinterval, true);
                return;
            }
        }
    }
}



/****************************************************************************/

