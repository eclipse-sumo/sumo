/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MsgHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    Tue, 17 Jun 2003
/// @version $Id$
///
// Retrieves messages about the process and gives them further to output
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#ifdef HAVE_FOX
#include <fx.h>
#endif
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include "MsgHandler.h"


// ===========================================================================
// static member variables
// ===========================================================================

MsgHandler::Factory MsgHandler::myFactory = nullptr;
MsgHandler* MsgHandler::myDebugInstance = nullptr;
MsgHandler* MsgHandler::myGLDebugInstance = nullptr;
MsgHandler* MsgHandler::myErrorInstance = nullptr;
MsgHandler* MsgHandler::myWarningInstance = nullptr;
MsgHandler* MsgHandler::myMessageInstance = nullptr;
bool MsgHandler::myAmProcessingProcess = false;
bool MsgHandler::myWriteDebugMessages(false);
bool MsgHandler::myWriteDebugGLMessages(false);


// ===========================================================================
// method definitions
// ===========================================================================

MsgHandler*
MsgHandler::getMessageInstance() {
    if (myMessageInstance == nullptr) {
        if (myFactory == nullptr) {
            myMessageInstance = new MsgHandler(MT_MESSAGE);
        } else {
            myMessageInstance = myFactory(MT_MESSAGE);
        }
    }
    return myMessageInstance;
}


MsgHandler*
MsgHandler::getWarningInstance() {
    if (myWarningInstance == nullptr) {
        myWarningInstance = new MsgHandler(MT_WARNING);
    }
    return myWarningInstance;
}


MsgHandler*
MsgHandler::getErrorInstance() {
    if (myErrorInstance == nullptr) {
        myErrorInstance = new MsgHandler(MT_ERROR);
    }
    return myErrorInstance;
}


MsgHandler*
MsgHandler::getDebugInstance() {
    if (myDebugInstance == nullptr) {
        myDebugInstance = new MsgHandler(MT_DEBUG);
    }
    return myDebugInstance;
}


MsgHandler*
MsgHandler::getGLDebugInstance() {
    if (myGLDebugInstance == nullptr) {
        myGLDebugInstance = new MsgHandler(MT_GLDEBUG);
    }
    return myGLDebugInstance;
}


void
MsgHandler::enableDebugMessages(bool enable) {
    myWriteDebugMessages = enable;
}

void
MsgHandler::enableDebugGLMessages(bool enable) {
    myWriteDebugGLMessages = enable;
}

void
MsgHandler::inform(std::string msg, bool addType) {
    // beautify progress output
    if (myAmProcessingProcess) {
        myAmProcessingProcess = false;
        MsgHandler::getMessageInstance()->inform("");
    }
    msg = build(msg, addType);
    // inform all receivers
    for (auto i : myRetrievers) {
        i->inform(msg);
    }
    // set the information that something occurred
    myWasInformed = true;
}


void
MsgHandler::beginProcessMsg(std::string msg, bool addType) {
    msg = build(msg, addType);
    // inform all other receivers
    for (auto i : myRetrievers) {
        i->inform(msg, ' ');
        myAmProcessingProcess = true;
    }
    // set the information that something occurred
    myWasInformed = true;
}


void
MsgHandler::endProcessMsg(std::string msg) {
    // inform all other receivers
    for (auto i : myRetrievers) {
        i->inform(msg);
    }
    // set the information that something occurred
    myWasInformed = true;
    myAmProcessingProcess = false;
}


void
MsgHandler::clear() {
    myWasInformed = false;
}


void
MsgHandler::addRetriever(OutputDevice* retriever) {
    if (!isRetriever(retriever)) {
        myRetrievers.push_back(retriever);
    }
}


void
MsgHandler::removeRetriever(OutputDevice* retriever) {
    std::vector<OutputDevice*>::iterator i = find(myRetrievers.begin(), myRetrievers.end(), retriever);
    if (i != myRetrievers.end()) {
        myRetrievers.erase(i);
    }
}


bool
MsgHandler::isRetriever(OutputDevice* retriever) const {
    return std::find(myRetrievers.begin(), myRetrievers.end(), retriever) != myRetrievers.end();
}


void
MsgHandler::removeRetrieverFromAllInstances(OutputDevice* out) {
    if (myDebugInstance != nullptr) {
        myDebugInstance->removeRetriever(out);
    }
    if (myGLDebugInstance != nullptr) {
        myGLDebugInstance->removeRetriever(out);
    }
    if (myErrorInstance != nullptr) {
        myErrorInstance->removeRetriever(out);
    }
    if (myWarningInstance != nullptr) {
        myWarningInstance->removeRetriever(out);
    }
    if (myMessageInstance != nullptr) {
        myMessageInstance->removeRetriever(out);
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
        OutputDevice* logFile = &OutputDevice::getDevice(oc.getString("log"));
        getErrorInstance()->addRetriever(logFile);
        if (!oc.getBool("no-warnings")) {
            getWarningInstance()->addRetriever(logFile);
        }
        getMessageInstance()->addRetriever(logFile);
    }
    if (oc.isSet("message-log", false)) {
        OutputDevice* logFile = &OutputDevice::getDevice(oc.getString("message-log"));
        getMessageInstance()->addRetriever(logFile);
    }
    if (oc.isSet("error-log", false)) {
        OutputDevice* logFile = &OutputDevice::getDevice(oc.getString("error-log"));
        getErrorInstance()->addRetriever(logFile);
        getWarningInstance()->addRetriever(logFile);
    }
    if (!oc.getBool("verbose")) {
        getMessageInstance()->removeRetriever(&OutputDevice::getDevice("stdout"));
    }
}


void
MsgHandler::cleanupOnEnd() {
    delete myMessageInstance;
    myMessageInstance = nullptr;
    delete myWarningInstance;
    myWarningInstance = nullptr;
    delete myErrorInstance;
    myErrorInstance = nullptr;
    delete myDebugInstance;
    myDebugInstance = nullptr;
    delete myGLDebugInstance;
    myGLDebugInstance = nullptr;
}


MsgHandler::MsgHandler(MsgType type) :
    myType(type), myWasInformed(false) {
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


/****************************************************************************/
