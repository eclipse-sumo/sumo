/****************************************************************************/
/// @file    MSLaneSpeedTrigger.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Changes the speed allowed on a set of lanes
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
#include <utils/common/MsgHandler.h>
#include <utils/common/WrappingCommand.h>
#include <microsim/MSLane.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/common/UtilExceptions.h>
#include "MSLaneSpeedTrigger.h"
#include <utils/xml/XMLSubSys.h>
#include <utils/common/TplConvert.h>
#include <microsim/MSEventControl.h>

#ifdef HAVE_MESOSIM
#include <microsim/MSGlobals.h>
#include <mesosim/MELoop.h>
#include <mesosim/MESegment.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
MSLaneSpeedTrigger::MSLaneSpeedTrigger(const std::string &id,
                                       const std::vector<MSLane*> &destLanes,
                                       const std::string &file) throw(ProcessError)
        : MSTrigger(id), SUMOSAXHandler(file),
        myDestLanes(destLanes), myAmOverriding(false), myDidInit(false) {
    myCurrentSpeed = destLanes[0]->getMaxSpeed();
    if (file != "") {
        if (!XMLSubSys::runParser(*this, file)) {
            throw ProcessError();
        }
        if (!myDidInit) {
            init();
        }
    }
}

void
MSLaneSpeedTrigger::init() throw(ProcessError) {
    // set it to the right value
    // assert there is at least one
    if (myLoadedSpeeds.size()==0) {
        myLoadedSpeeds.push_back(std::make_pair(100000, myCurrentSpeed));
    }
    // set the process to the begin
    myCurrentEntry = myLoadedSpeeds.begin();
    // pass previous time steps
    while ((*myCurrentEntry).first<MSNet::getInstance()->getCurrentTimeStep()&&myCurrentEntry!=myLoadedSpeeds.end()) {
        processCommand(true, MSNet::getInstance()->getCurrentTimeStep());
    }

    // add the processing to the event handler
    MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(
        new WrappingCommand<MSLaneSpeedTrigger>(this, &MSLaneSpeedTrigger::execute),
        (*myCurrentEntry).first, MSEventControl::NO_CHANGE);
    myDidInit = true;
}


MSLaneSpeedTrigger::~MSLaneSpeedTrigger() throw() {}


SUMOTime
MSLaneSpeedTrigger::execute(SUMOTime currentTime) throw(ProcessError) {
    return processCommand(true, currentTime);
}


SUMOTime
MSLaneSpeedTrigger::processCommand(bool move2next, SUMOTime currentTime) {
    std::vector<MSLane*>::iterator i;
    const SUMOReal speed = getCurrentSpeed();
    for (i=myDestLanes.begin(); i!=myDestLanes.end(); ++i) {
#ifdef HAVE_MESOSIM
        if (MSGlobals::gUseMesoSim) {
            MESegment *first = MSGlobals::gMesoNet->getSegmentForEdge((*i)->getEdge());
            while (first!=0) {
                MSGlobals::gMesoNet->setSpeed(first, speed, currentTime);
                first = first->getNextSegment();
            }
            continue;
        }
#endif
        (*i)->setMaxSpeed(speed);
    }
    if (!move2next) {
        // changed from the gui
        return 0;
    }
    if (myCurrentEntry!=myLoadedSpeeds.end()) {
        ++myCurrentEntry;
    }
    if (myCurrentEntry!=myLoadedSpeeds.end()) {
        return ((*myCurrentEntry).first)-((*(myCurrentEntry-1)).first);
    } else {
        return 0;
    }
}


void
MSLaneSpeedTrigger::myStartElement(SumoXMLTag element,
                                   const SUMOSAXAttributes &attrs) throw(ProcessError) {
    // check whether the correct tag is read
    if (element!=SUMO_TAG_STEP) {
        return;
    }
    // extract the values
    try {
        int next = attrs.getIntSecure(SUMO_ATTR_TIME, -1);
        SUMOReal speed = attrs.getFloatSecure(SUMO_ATTR_SPEED, -1.0);
        // check the values
        if (next<0) {
            MsgHandler::getErrorInstance()->inform("Wrong time in MSLaneSpeedTrigger in file '" + getFileName() + "'.");
            return;
        }
        if (speed<0) {
            MsgHandler::getErrorInstance()->inform("Wrong speed in MSLaneSpeedTrigger in file '" + getFileName() + "'.");
            return;
        }
        // set the values for the next step as they are valid
        myLoadedSpeeds.push_back(std::make_pair(next, speed));
    } catch (NumberFormatException &) {
        throw ProcessError("Could not initialise vss '" + getID() + "'.");
    }
}


void
MSLaneSpeedTrigger::myEndElement(SumoXMLTag element) throw(ProcessError) {
    if (element==SUMO_TAG_VSS && !myDidInit) {
        init();
    }
}


SUMOReal
MSLaneSpeedTrigger::getDefaultSpeed() const {
    return myDefaultSpeed;
}


void
MSLaneSpeedTrigger::setOverriding(bool val) {
    myAmOverriding = val;
    processCommand(false, MSNet::getInstance()->getCurrentTimeStep());
}


void
MSLaneSpeedTrigger::setOverridingValue(SUMOReal val) {
    mySpeedOverrideValue = val;
    processCommand(false, MSNet::getInstance()->getCurrentTimeStep());
}


SUMOReal
MSLaneSpeedTrigger::getLoadedSpeed() {
    if (myCurrentEntry!=myLoadedSpeeds.begin()) {
        return (*(myCurrentEntry-1)).second;
    } else {
        return (*myCurrentEntry).second;
    }
}


SUMOReal
MSLaneSpeedTrigger::getCurrentSpeed() const {
    if (myAmOverriding) {
        return mySpeedOverrideValue;
    } else {
        // ok, maybe the first shall not yet be the valid one
        if (myCurrentEntry==myLoadedSpeeds.begin()&&(*myCurrentEntry).first>MSNet::getInstance()->getCurrentTimeStep()) {
            return myDefaultSpeed;
        }
        // try the loaded
        if (myCurrentEntry!=myLoadedSpeeds.end()&&(*myCurrentEntry).first<=MSNet::getInstance()->getCurrentTimeStep()) {
            return (*myCurrentEntry).second;
        } else {
            return (*(myCurrentEntry-1)).second;
        }
    }
}


/****************************************************************************/

