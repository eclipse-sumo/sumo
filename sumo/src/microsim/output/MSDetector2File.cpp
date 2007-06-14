/****************************************************************************/
/// @file    MSDetector2File.cpp
/// @author  Christian Roessel
/// @date    Wed Aug 6 16:16:26 2003
/// @version $Id$
///
//  »missingDescription«
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
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>
#include <string>
#include <cassert>
#include <utility>
#include <iostream>
#include <utils/helpers/OneArgumentCommand.h>
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
MSDetector2File::close()
{
    // flush the last values
    Intervals::iterator it;
    for (it = intervalsM.begin(); it != intervalsM.end(); ++it) {
        write2file((*it).first);
    }
    // close files and delete the detectors
    for (it = intervalsM.begin(); it != intervalsM.end(); ++it) {
        for (DetectorFileVec::iterator df =
                    it->second.begin(); df != it->second.end(); ++df) {
            MSDetectorFileOutput *det = df->first;
            if (df->second->needsTail()) {
                det->writeXMLDetectorInfoEnd(*(df->second));
                df->second->closeInfo();
                df->second->setNeedsTail(false);
            }
        }
    }
    intervalsM.clear();
}


void
MSDetector2File::addDetectorAndInterval(MSDetectorFileOutput* det,
                                        OutputDevice *device,
                                        SUMOTime interval,
                                        bool reinsert)
{
    IntervalsKey key = interval;
    Intervals::iterator it = intervalsM.find(key);
    if (it == intervalsM.end()) {
        DetectorFileVec detAndFileVec;
        detAndFileVec.push_back(make_pair(det, device));
        intervalsM.insert(make_pair(key, detAndFileVec));
        // Add command for given key only once to MSEventControl
        Command* writeData =
            new OneArgumentCommand< MSDetector2File, IntervalsKey >
            (this, &MSDetector2File::write2file, key);
        MSNet::getInstance()->getEndOfTimestepEvents().addEvent(
            writeData,
            OptionsSubSys::getOptions().getInt("begin") + interval,
            MSEventControl::ADAPT_AFTER_EXECUTION);
        myLastCalls[interval] =
            OptionsSubSys::getOptions().getInt("begin");
    } else {
        DetectorFileVec& detAndFileVec = it->second;
        if (find_if(detAndFileVec.begin(), detAndFileVec.end(),
                    bind2nd(detectorEquals(), det))
                == detAndFileVec.end()) {
            detAndFileVec.push_back(make_pair(det, device));
        } else {
            // detector already in container. Don't add several times
            WRITE_WARNING("MSDetector2File::addDetectorAndInterval: detector already in container. Ignoring.");
            return;
        }
    }
    if (!reinsert&&device->needsHeader()) {
        det->writeXMLHeader(*device);
        det->writeXMLDetectorInfoStart(*device);
        device->closeInfo();
        device->setNeedsHeader(false);
    }
}


MSUnit::IntSteps
MSDetector2File::write2file(IntervalsKey key)
{
    Intervals::iterator iIt = intervalsM.find(key);
    assert(iIt != intervalsM.end());
    DetectorFileVec dfVec = iIt->second;
    MSUnit::IntSteps interval = key;
    SUMOTime stopTime = MSNet::getInstance()->simSeconds();
    SUMOTime startTime = myLastCalls[interval];
    // check whether at the end the output was already generated
    if (stopTime==startTime) {
        return 0; // a dummy value only; this should only be the case
        // when this container is destroyed
    }
    for (DetectorFileVec::iterator it = dfVec.begin();
            it != dfVec.end(); ++it) {
        MSDetectorFileOutput* det = it->first;
        det->writeXMLOutput(*(it->second), startTime, stopTime-1);
        it->second->closeInfo();
    }
    myLastCalls[interval] = MSNet::getInstance()->simSeconds();
    return interval;
}



void
MSDetector2File::resetInterval(MSDetectorFileOutput* det,
                               SUMOTime newinterval)
{
    for (Intervals::iterator i=intervalsM.begin(); i!=intervalsM.end(); ++i) {
        DetectorFileVec &dets = (*i).second;
        for (DetectorFileVec::iterator j=dets.begin(); j!=dets.end(); ++j) {
            DetectorFilePair &dvp = *j;
            if (dvp.first==det) {
                DetectorFilePair dvpu = *j;
                dets.erase(j);
                addDetectorAndInterval(dvpu.first, dvpu.second,
                                       newinterval, true);
                return;
            }
        }
    }
}



/****************************************************************************/

