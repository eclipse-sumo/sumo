/****************************************************************************/
/// @file    MSDetectorControl.cpp
/// @author  Daniel Krajzewicz
/// @date    2005-09-15
/// @version $Id$
///
// Detectors container; responsible for string and output generation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include "MSDetectorControl.h"
#include "MSMeanData_Net.h"
#include <microsim/output/MSE2Collector.h>
#include <microsim/output/MS_E2_ZS_CollectorOverLanes.h>
#include <microsim/output/MSE3Collector.h>
#include <microsim/output/MSInductLoop.h>
#include <microsim/output/MSMeanData_Harmonoise.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>

#ifdef _MESSAGES
#include <microsim/output/MSMsgInductLoop.h>
#endif

#ifdef HAVE_MESOSIM
#include <mesosim/MEInductLoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
MSDetectorControl::MSDetectorControl() throw() {
}


MSDetectorControl::~MSDetectorControl() throw() {
#ifdef HAVE_MESOSIM
    myMesoLoops.clear();
#endif
    myLoops.clear();
#ifdef _MESSAGES
#ifdef _DEBUG
    cout << "MSDetectorControl: clearing myMsgLoops" << endl;
#endif
    myMsgLoops.clear();
#endif
    myE2Detectors.clear();
    myE3Detectors.clear();
    myE2OverLanesDetectors.clear();
    myVTypeProbeDetectors.clear();
    myRouteProbeDetectors.clear();
}


void
MSDetectorControl::close(SUMOTime step) throw(IOError) {
    // flush the last values
    writeOutput(step-DELTA_T, true);
    // [...] files are closed on another place [...]
    myIntervals.clear();
}


#ifdef _MESSAGES
void
MSDetectorControl::add(MSMsgInductLoop *msgl, OutputDevice& device, int splInterval) throw(ProcessError) {
#ifdef _DEBUG
    cout << "adding MSMsgInductLoop..." << endl;
#endif
    if (! myMsgLoops.add(msgl->getID(), msgl)) {
        throw ProcessError("message induct loop '" + msgl->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
    addDetectorAndInterval(msgl, &device, splInterval);
}
#endif


void
MSDetectorControl::add(MSInductLoop *il, OutputDevice& device, int splInterval) throw(ProcessError) {
    // insert object into dictionary
    if (! myLoops.add(il->getID(), il)) {
        throw ProcessError("induct loop '" + il->getID() + "' could not be build;\n (declared twice?)");
    }
    addDetectorAndInterval(il, &device, splInterval);
}


void
MSDetectorControl::add(MSE2Collector *e2, OutputDevice& device, int splInterval) throw(ProcessError) {
    // insert object into dictionary
    if (! myE2Detectors.add(e2->getID(), e2)) {
        throw ProcessError("e2-detector '" + e2->getID() + "' could not be build;\n (declared twice?)");
    }
    addDetectorAndInterval(e2, &device, splInterval);
}


void
MSDetectorControl::add(MS_E2_ZS_CollectorOverLanes *e2ol, OutputDevice& device, int splInterval) throw(ProcessError) {
    // insert object into dictionary
    if (! myE2OverLanesDetectors.add(e2ol->getID(), e2ol)) {
        throw ProcessError("e2-overlanes-detector '" + e2ol->getID() + "' could not be build;\n (declared twice?)");
    }
    addDetectorAndInterval(e2ol, &device, splInterval);
}


void
MSDetectorControl::add(MSE2Collector *e2) throw(ProcessError) {
    // insert object into dictionary
    if (! myE2Detectors.add(e2->getID(), e2)) {
        throw ProcessError("e2-detector '" + e2->getID() + "' could not be build;\n (declared twice?)");
    }
}


void
MSDetectorControl::add(MS_E2_ZS_CollectorOverLanes *e2ol) throw(ProcessError) {
    // insert object into dictionary
    if (! myE2OverLanesDetectors.add(e2ol->getID(), e2ol)) {
        throw ProcessError("e2-overlanes-detector '" + e2ol->getID() + "' could not be build;\n (declared twice?)");
    }
}


void
MSDetectorControl::add(MSMeanData_Harmonoise *mn) throw() {
    myHarmonoiseDetectors.push_back(mn);
}


void
MSDetectorControl::add(MSE3Collector *e3, OutputDevice& device, int splInterval) throw(ProcessError) {
    // insert object into dictionary
    if (! myE3Detectors.add(e3->getID(), e3)) {
        throw ProcessError("e3-detector '" + e3->getID() + "' could not be build;\n (declared twice?)");
    }
    addDetectorAndInterval(e3, &device, splInterval);
}


void
MSDetectorControl::add(MSVTypeProbe *vp, OutputDevice& device,
                       int frequency) throw(ProcessError) {
    // insert object into dictionary
    if (! myVTypeProbeDetectors.add(vp->getID(), vp)) {
        throw ProcessError("vtypeprobe '" + vp->getID() + "' could not be build;\n (declared twice?)");
    }
    addDetectorAndInterval(vp, &device, frequency);
}


void
MSDetectorControl::add(MSRouteProbe *vp, OutputDevice& device,
                       SUMOTime frequency, SUMOTime begin) throw(ProcessError) {
    // insert object into dictionary
    if (! myRouteProbeDetectors.add(vp->getID(), vp)) {
        throw ProcessError("routeprobe '" + vp->getID() + "' could not be build;\n (declared twice?)");
    }
    addDetectorAndInterval(vp, &device, frequency, begin);
}




#ifdef HAVE_MESOSIM
void
MSDetectorControl::add(MEInductLoop *meil, OutputDevice& device, int splInterval) throw(ProcessError) {
    // insert object into dictionary
    if (! myMesoLoops.add(meil->getID(), meil)) {
        throw ProcessError("meso-induct loop '" + meil->getID() + "' could not be build;"
                           + "\n (declared twice?)");
    }
    addDetectorAndInterval(meil, &device, splInterval);
}
#endif


void
MSDetectorControl::updateDetectors(SUMOTime step) throw() {
    // update all detectors with inner containers
    // e2-detectors
    const std::vector<MSE2Collector*> &e2s = myE2Detectors.buildAndGetStaticVector();
    for (E2Vect::const_iterator i=e2s.begin(); i!=e2s.end(); ++i) {
        (*i)->update(step);
    }
    // e3-detectors
    const std::vector<MSE3Collector*> &e3s = myE3Detectors.buildAndGetStaticVector();
    for (E3Vect::const_iterator i=e3s.begin(); i!=e3s.end(); ++i) {
        (*i)->update(step);
    }
    // induct loops do not need to be updated...
    // vtypeprobes do not need to be updated...
    for (vector<MSMeanData_Harmonoise*>::const_iterator i=myHarmonoiseDetectors.begin(); i!=myHarmonoiseDetectors.end(); ++i) {
        (*i)->update();
    }
}


void
MSDetectorControl::writeOutput(SUMOTime step, bool closing) throw(IOError) {
    for (Intervals::iterator i=myIntervals.begin(); i!=myIntervals.end(); ++i) {
        IntervalsKey interval = (*i).first;
        if (myLastCalls[interval]+interval.first-1<=step || (closing && myLastCalls[interval]<step + DELTA_T)) {
            DetectorFileVec dfVec = (*i).second;
            SUMOTime startTime = myLastCalls[interval];
            // check whether at the end the output was already generated
            for (DetectorFileVec::iterator it = dfVec.begin(); it!=dfVec.end(); ++it) {
                MSDetectorFileOutput* det = it->first;
                det->writeXMLOutput(*(it->second), startTime, step+DELTA_T);
            }
            myLastCalls[interval] = step + DELTA_T;
        }
    }
}


void
MSDetectorControl::addDetectorAndInterval(MSDetectorFileOutput* det,
        OutputDevice *device,
        SUMOTime interval,
        SUMOTime begin) throw() {
    if (begin == -1) {
        begin = OptionsCont::getOptions().getInt("begin");
    }
    IntervalsKey key = make_pair(interval, begin);
    Intervals::iterator it = myIntervals.find(key);
    // Add command for given key only once to MSEventControl...
    if (it == myIntervals.end()) {
        DetectorFileVec detAndFileVec;
        detAndFileVec.push_back(make_pair(det, device));
        myIntervals.insert(make_pair(key, detAndFileVec));
        myLastCalls[key] = begin;
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
    det->writeXMLDetectorProlog(*device);
}





/****************************************************************************/

