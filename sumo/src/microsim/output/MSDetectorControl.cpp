/****************************************************************************/
/// @file    MSDetectorControl.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Clemens Honomichl
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    2005-09-15
/// @version $Id$
///
// Detectors container; responsible for string and output generation
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

#include <iostream>
#include "MSDetectorControl.h"
#include "MSMeanData_Net.h"
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/common/MsgHandler.h>

#ifdef HAVE_INTERNAL
#include <mesosim/MEInductLoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
MSDetectorControl::MSDetectorControl() {
}


MSDetectorControl::~MSDetectorControl() {
    for (std::map<SumoXMLTag, NamedObjectCont<MSDetectorFileOutput*> >::iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        (*i).second.clear();
    }
    for (std::vector<MSMeanData*>::const_iterator i = myMeanData.begin(); i != myMeanData.end(); ++i) {
        delete *i;
    }
}


void
MSDetectorControl::close(SUMOTime step) {
    // flush the last values
    writeOutput(step, true);
    // [...] files are closed on another place [...]
    myIntervals.clear();
}


void
MSDetectorControl::add(SumoXMLTag type, MSDetectorFileOutput* d, const std::string& device, int splInterval, SUMOTime begin) {
    if (!myDetectors[type].add(d->getID(), d)) {
        throw ProcessError(toString(type) + " detector '" + d->getID() + "' could not be build (declared twice?).");
    }
    addDetectorAndInterval(d, &OutputDevice::getDevice(device), splInterval, begin);
}



void
MSDetectorControl::add(SumoXMLTag type, MSDetectorFileOutput* d) {
    if (!myDetectors[type].add(d->getID(), d)) {
        throw ProcessError(toString(type) + " detector '" + d->getID() + "' could not be build (declared twice?).");
    }
}



void
MSDetectorControl::add(MSMeanData* mn, const std::string& device,
                       SUMOTime frequency, SUMOTime begin) {
    myMeanData.push_back(mn);
    addDetectorAndInterval(mn, &OutputDevice::getDevice(device), frequency, begin);
    if (begin == string2time(OptionsCont::getOptions().getString("begin"))) {
        mn->init();
    }
}


const std::vector<SumoXMLTag>
MSDetectorControl::getAvailableTypes() const {
    std::vector<SumoXMLTag> result;
    for (std::map<SumoXMLTag, NamedObjectCont<MSDetectorFileOutput*> >::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        result.push_back(i->first);
    }
    return result;
}


const NamedObjectCont<MSDetectorFileOutput*>&
MSDetectorControl::getTypedDetectors(SumoXMLTag type) const {
    if (myDetectors.find(type) == myDetectors.end()) {
        return myEmptyContainer;
    }
    return myDetectors.find(type)->second;
}


void
MSDetectorControl::updateDetectors(const SUMOTime step) {
    for (std::map<SumoXMLTag, NamedObjectCont<MSDetectorFileOutput*> >::const_iterator i = myDetectors.begin(); i != myDetectors.end(); ++i) {
        const std::map<std::string, MSDetectorFileOutput*>& dets = getTypedDetectors((*i).first).getMyMap();
        for (std::map<std::string, MSDetectorFileOutput*>::const_iterator j = dets.begin(); j != dets.end(); ++j) {
            (*j).second->detectorUpdate(step);
        }
    }
    for (std::vector<MSMeanData*>::const_iterator i = myMeanData.begin(); i != myMeanData.end(); ++i) {
        (*i)->detectorUpdate(step);
    }
}


void
MSDetectorControl::writeOutput(SUMOTime step, bool closing) {
    for (Intervals::iterator i = myIntervals.begin(); i != myIntervals.end(); ++i) {
        IntervalsKey interval = (*i).first;
        if (myLastCalls[interval] + interval.first <= step || (closing && myLastCalls[interval] < step)) {
            DetectorFileVec dfVec = (*i).second;
            SUMOTime startTime = myLastCalls[interval];
            // check whether at the end the output was already generated
            for (DetectorFileVec::iterator it = dfVec.begin(); it != dfVec.end(); ++it) {
                MSDetectorFileOutput* det = it->first;
                det->writeXMLOutput(*(it->second), startTime, step);
            }
            myLastCalls[interval] = step;
        }
    }
}


void
MSDetectorControl::addDetectorAndInterval(MSDetectorFileOutput* det,
        OutputDevice* device,
        SUMOTime interval,
        SUMOTime begin) {
    if (begin == -1) {
        begin = string2time(OptionsCont::getOptions().getString("begin"));
    }
    IntervalsKey key = std::make_pair(interval, begin);
    Intervals::iterator it = myIntervals.find(key);
    // Add command for given key only once to MSEventControl...
    if (it == myIntervals.end()) {
        DetectorFileVec detAndFileVec;
        detAndFileVec.push_back(std::make_pair(det, device));
        myIntervals.insert(std::make_pair(key, detAndFileVec));
        myLastCalls[key] = begin;
    } else {
        DetectorFileVec& detAndFileVec = it->second;
        if (find_if(detAndFileVec.begin(), detAndFileVec.end(), bind2nd(detectorEquals(), det)) == detAndFileVec.end()) {
            detAndFileVec.push_back(std::make_pair(det, device));
        } else {
            // detector already in container. Don't add several times
            WRITE_WARNING("MSDetectorControl::addDetectorAndInterval: detector already in container. Ignoring.");
            return;
        }
    }
    det->writeXMLDetectorProlog(*device);
}



/****************************************************************************/

