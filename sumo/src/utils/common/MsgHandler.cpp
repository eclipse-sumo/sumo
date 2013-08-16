/****************************************************************************/
/// @file    MsgHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Retrieves messages about the process and gives them further to output
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

#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#include "MsgHandler.h"
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include "AbstractMutex.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static member variables
// ===========================================================================
MsgHandler* MsgHandler::myErrorInstance = 0;
MsgHandler* MsgHandler::myWarningInstance = 0;
MsgHandler* MsgHandler::myMessageInstance = 0;
bool MsgHandler::myAmProcessingProcess = false;
AbstractMutex* MsgHandler::myLock = 0;


// ===========================================================================
// method definitions
// ===========================================================================
MsgHandler*
MsgHandler::getMessageInstance() {
    if (myMessageInstance == 0) {
        myMessageInstance = new MsgHandler(MT_MESSAGE);
    }
    return myMessageInstance;
}


MsgHandler*
MsgHandler::getWarningInstance() {
    if (myWarningInstance == 0) {
        myWarningInstance = new MsgHandler(MT_WARNING);
    }
    return myWarningInstance;
}


MsgHandler*
MsgHandler::getErrorInstance() {
    if (myErrorInstance == 0) {
        myErrorInstance = new MsgHandler(MT_ERROR);
    }
    return myErrorInstance;
}


void
MsgHandler::inform(std::string msg, bool addType) {
    if (myLock != 0) {
        myLock->lock();
    }
    // beautify progress output
    if (myAmProcessingProcess) {
        myAmProcessingProcess = false;
        MsgHandler::getMessageInstance()->inform("");
    }
    msg = build(msg, addType);
    // inform all receivers
    for (RetrieverVector::iterator i = myRetrievers.begin(); i != myRetrievers.end(); i++) {
        (*i)->inform(msg);
    }
    // set the information that something occured
    myWasInformed = true;
    if (myLock != 0) {
        myLock->unlock();
    }
}


void
MsgHandler::beginProcessMsg(std::string msg, bool addType) {
    if (myLock != 0) {
        myLock->lock();
    }
    msg = build(msg, addType);
    // inform all other receivers
    for (RetrieverVector::iterator i = myRetrievers.begin(); i != myRetrievers.end(); i++) {
        (*i)->inform(msg, ' ');
        myAmProcessingProcess = true;
    }
    // set the information that something occured
    myWasInformed = true;
    if (myLock != 0) {
        myLock->unlock();
    }
}


void
MsgHandler::endProcessMsg(std::string msg) {
    if (myLock != 0) {
        myLock->lock();
    }
    // inform all other receivers
    for (RetrieverVector::iterator i = myRetrievers.begin(); i != myRetrievers.end(); i++) {
        (*i)->inform(msg);
    }
    // set the information that something occured
    myWasInformed = true;
    myAmProcessingProcess = false;
    if (myLock != 0) {
        myLock->unlock();
    }
}


void
MsgHandler::clear() {
    if (myLock != 0) {
        myLock->lock();
    }
    myWasInformed = false;
    if (myLock != 0) {
        myLock->unlock();
    }
}


void
MsgHandler::addRetriever(OutputDevice* retriever) {
    if (myLock != 0) {
        myLock->lock();
    }
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if (i == myRetrievers.end()) {
        myRetrievers.push_back(retriever);
    }
    if (myLock != 0) {
        myLock->unlock();
    }
}


void
MsgHandler::removeRetriever(OutputDevice* retriever) {
    if (myLock != 0) {
        myLock->lock();
    }
    RetrieverVector::iterator i =
        find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if (i != myRetrievers.end()) {
        myRetrievers.erase(i);
    }
    if (myLock != 0) {
        myLock->unlock();
    }
}


void
MsgHandler::initOutputOptions() {
    // initialize console properly
    OutputDevice::getDevice("stdout");
    OutputDevice::getDevice("stderr");
    OptionsCont& oc = OptionsCont::getOptions();
    if (oc.getBool("no-warnings")) {
        getWarningInstance()->removeRetriever(&OutputDevice::getDevice("stderr"));
    }
    // build the logger if possible
    if (oc.isSet("log", false)) {
        try {
            OutputDevice* logFile = &OutputDevice::getDevice(oc.getString("log"));
            getErrorInstance()->addRetriever(logFile);
            if (!oc.getBool("no-warnings")) {
                getWarningInstance()->addRetriever(logFile);
            }
            getMessageInstance()->addRetriever(logFile);
        } catch (IOError&) {
            throw ProcessError("Could not build logging file '" + oc.getString("log") + "'");
        }
    }
    if (oc.isSet("message-log", false)) {
        try {
            OutputDevice* logFile = &OutputDevice::getDevice(oc.getString("message-log"));
            getMessageInstance()->addRetriever(logFile);
        } catch (IOError&) {
            throw ProcessError("Could not build logging file '" + oc.getString("message-log") + "'");
        }
    }
    if (oc.isSet("error-log", false)) {
        try {
            OutputDevice* logFile = &OutputDevice::getDevice(oc.getString("error-log"));
            getErrorInstance()->addRetriever(logFile);
            getWarningInstance()->addRetriever(logFile);
        } catch (IOError&) {
            throw ProcessError("Could not build logging file '" + oc.getString("error-log") + "'");
        }
    }
    if (!oc.getBool("verbose")) {
        getMessageInstance()->removeRetriever(&OutputDevice::getDevice("stdout"));
    }
}


void
MsgHandler::cleanupOnEnd() {
    if (myLock != 0) {
        myLock->lock();
    }
    delete myMessageInstance;
    myMessageInstance = 0;
    delete myWarningInstance;
    myWarningInstance = 0;
    delete myErrorInstance;
    myErrorInstance = 0;
    if (myLock != 0) {
        myLock->unlock();
    }
}


MsgHandler::MsgHandler(MsgType type)
    : myType(type), myWasInformed(false) {
    if (type == MT_MESSAGE) {
        addRetriever(&OutputDevice::getDevice("stdout"));
    } else {
        addRetriever(&OutputDevice::getDevice("stderr"));
    }
}


MsgHandler::~MsgHandler() {
}


bool
MsgHandler::wasInformed() const {
    return myWasInformed;
}


void
MsgHandler::assignLock(AbstractMutex* lock) {
    assert(myLock == 0);
    myLock = lock;
}



/****************************************************************************/

