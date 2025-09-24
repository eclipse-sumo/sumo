/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MsgHandler.cpp
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @author  Mirko Barthauer
/// @date    Tue, 17 Jun 2003
///
// Retrieves messages about the process and gives them further to output
/****************************************************************************/
#include <config.h>

#include <string>
#include <cassert>
#include <vector>
#include <algorithm>
#include <iostream>
#ifdef WIN32
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
#else
#include <unistd.h>
#endif
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/common/UtilExceptions.h>
#include "MsgHandler.h"


// ===========================================================================
// static member variables
// ===========================================================================
MsgHandler::Factory MsgHandler::myFactory = nullptr;
MsgHandler* MsgHandler::myErrorInstance = nullptr;
MsgHandler* MsgHandler::myWarningInstance = nullptr;
MsgHandler* MsgHandler::myMessageInstance = nullptr;
bool MsgHandler::myAmProcessingProcess = false;
bool MsgHandler::myWriteDebugMessages = false;
bool MsgHandler::myWriteDebugGLMessages = false;
bool MsgHandler::myWriteTimestamps = false;
bool MsgHandler::myWriteProcessId = false;
std::string MsgHandler::myErrorPrefix = "Error: ";
std::string MsgHandler::myWarningPrefix = "Warning: ";


// ===========================================================================
// method definitions
// ===========================================================================

MsgHandler*
MsgHandler::getMessageInstance() {
    if (myMessageInstance == nullptr) {
        if (myFactory == nullptr) {
            myMessageInstance = new MsgHandler(MsgType::MT_MESSAGE);
        } else {
            myMessageInstance = myFactory(MsgType::MT_MESSAGE);
        }
    }
    return myMessageInstance;
}


MsgHandler*
MsgHandler::getWarningInstance() {
    if (myWarningInstance == nullptr) {
        if (myFactory == nullptr) {
            myWarningInstance = new MsgHandler(MsgType::MT_WARNING);
        } else {
            myWarningInstance = myFactory(MsgType::MT_WARNING);
        }
    }
    return myWarningInstance;
}


MsgHandler*
MsgHandler::getErrorInstance() {
    if (myErrorInstance == nullptr) {
        myErrorInstance = new MsgHandler(MsgType::MT_ERROR);
    }
    return myErrorInstance;
}


void
MsgHandler::enableDebugMessages(bool enable) {
    myWriteDebugMessages = enable;
}

void
MsgHandler::enableDebugGLMessages(bool enable) {
    myWriteDebugGLMessages = enable;
}


std::string
MsgHandler::insertLineBreaks(std::string msg, int lineWidth) {
    // TODO: check what FXFont::getTextWidth can do
    //int textWidth = getApp()->getNormalFont()->getTextWidth
    if ((int)msg.size() <= lineWidth) {
        return msg;
    }
    size_t pos = 0;
    size_t nextLineBreak = msg.find('\n');
    size_t spaceAfterLine = msg.find(' ', lineWidth);
    while (spaceAfterLine != std::string::npos) {
        if (nextLineBreak == std::string::npos || nextLineBreak > spaceAfterLine) {
            msg = msg.replace(spaceAfterLine, 1, "\n");
            pos = spaceAfterLine + 1;
        } else {
            pos = nextLineBreak + 1;
        }
        spaceAfterLine = msg.find(' ', pos + lineWidth);
        nextLineBreak = msg.find('\n', pos);
    }
    return msg;
}


void
MsgHandler::inform(std::string msg, bool addType) {
    if (addType && !myInitialMessages.empty() && myInitialMessages.size() < 5) {
        myInitialMessages.push_back(msg);
    }
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
        i->inform(msg, true);
        myAmProcessingProcess = true;
    }
    // set the information that something occurred
    myWasInformed = true;
}


void
MsgHandler::endProcessMsg2(bool success, long duration) {
    if (success) {
        if (duration > -1) {
            endProcessMsg(TLF(" done (%ms).", toString(duration)));
        } else {
            endProcessMsg(TL(" done."));
        }
    } else {
        endProcessMsg(TL(" failed."));
    }
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
MsgHandler::clear(bool resetInformed) {
    if (myAggregationThreshold >= 0) {
        for (const auto& i : myAggregationCount) {
            if (i.second > myAggregationThreshold) {
                inform(toString(i.second) + " total messages of type: " + i.first);
            }
        }
    }
    myAggregationCount.clear();
    if (!resetInformed && myInitialMessages.size() > 1) {
        const bool wasInformed = myWasInformed;
        for (const std::string& msg : myInitialMessages) {
            inform(msg, false);
        }
        myInitialMessages.clear();
        myWasInformed = wasInformed;
    }
    if (resetInformed) {
        myWasInformed = false;
    }
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
MsgHandler::setupI18n(const std::string& locale) {
#ifdef HAVE_INTL
    if (locale != "") {
#ifdef WIN32
        _putenv_s("LANGUAGE", locale.data());
#else
        setenv("LANGUAGE", locale.data(), true);
#endif
    }
    if (!setlocale(LC_MESSAGES, "")) {
        WRITE_WARNINGF(TL("Could not set locale to '%'."), locale);
    }
    const char* sumoPath = getenv("SUMO_HOME");
    if (sumoPath == nullptr) {
        if (!bindtextdomain("sumo", nullptr)) {
            WRITE_WARNING(TL("Environment variable SUMO_HOME is not set, could not find localized messages."));
            return;
        }
    } else {
        const std::string path = sumoPath + std::string("/data/locale/");
        if (!bindtextdomain("sumo", path.data())) {
            WRITE_WARNING(TL("Could not find localized messages."));
            return;
        }
    }
    bind_textdomain_codeset("sumo", "UTF-8");
    textdomain("sumo");
#ifdef WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
#else
    UNUSED_PARAMETER(locale);
#endif
    myWarningPrefix = TL("Warning: ");
    myErrorPrefix = TL("Error: ");
    gLocaleInitialized = true;
}


void
MsgHandler::initOutputOptions() {
    // initialize console properly
    OutputDevice::getDevice("stdout");
    OutputDevice::getDevice("stderr");
    OptionsCont& oc = OptionsCont::getOptions();
    getWarningInstance()->setAggregationThreshold(oc.getInt("aggregate-warnings"));
    getErrorInstance()->setAggregationThreshold(oc.getInt("aggregate-warnings"));
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
        if (oc.getBool("log.timestamps")) {
            myWriteTimestamps = true;
        }
        if (oc.getBool("log.processid")) {
            myWriteProcessId = true;
        }
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
    if (oc.getBool("verbose")) {
        getErrorInstance()->myInitialMessages.push_back("Repeating initial error messages:");
    } else {
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
}


std::string
MsgHandler::buildProcessIdPrefix() const {
    std::stringstream prefix;
    prefix << "[PID: ";
#ifdef WIN32
    prefix << GetCurrentProcessId();
#else
    prefix << getpid();
#endif
    prefix << "] ";
    return prefix.str();
}


MsgHandler::MsgHandler(MsgType type) :
    myType(type), myWasInformed(false), myAggregationThreshold(-1) {
    if (type == MsgType::MT_MESSAGE) {
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
