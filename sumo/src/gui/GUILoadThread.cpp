/****************************************************************************/
/// @file    GUILoadThread.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
/// @version $Id$
///
// Class describing the thread that performs the loading of a simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <guisim/GUINet.h>
#include <guisim/GUIEventControl.h>
#include <netload/NLBuilder.h>
#include <netload/NLHandler.h>
#include <netload/NLJunctionControlBuilder.h>
#include <guinetload/GUIEdgeControlBuilder.h>
#include <guinetload/GUIDetectorBuilder.h>
#include <guinetload/GUITriggerBuilder.h>
#include <guisim/GUIVehicleControl.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXEventQue.h>
#include <microsim/MSFrame.h>
#include <utils/common/MsgRetrievingFunction.h>
#include "GUIApplicationWindow.h"
#include "GUILoadThread.h"
#include "GUIGlobals.h"
#include "GUIEvent_SimulationLoaded.h"
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObjectStorage.h>
#include <utils/gui/images/GUITexturesHelper.h>
#include <utils/common/RandHelper.h>
#include <ctime>

#ifdef HAVE_INTERNAL
#include <mesogui/GUIMEVehicleControl.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
GUILoadThread::GUILoadThread(FXApp* app, MFXInterThreadEventClient* mw,
                             MFXEventQue& eq, FXEX::FXThreadEvent& ev)
    : FXSingleEventThread(app, mw), myParent(mw), myEventQue(eq),
      myEventThrow(ev) {
    myErrorRetriever = new MsgRetrievingFunction<GUILoadThread>(this, &GUILoadThread::retrieveMessage, MsgHandler::MT_ERROR);
    myMessageRetriever = new MsgRetrievingFunction<GUILoadThread>(this, &GUILoadThread::retrieveMessage, MsgHandler::MT_MESSAGE);
    myWarningRetriever = new MsgRetrievingFunction<GUILoadThread>(this, &GUILoadThread::retrieveMessage, MsgHandler::MT_WARNING);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
}


GUILoadThread::~GUILoadThread() {
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetriever;
}


FXint
GUILoadThread::run() {
    GUINet* net = 0;
    int simStartTime = 0;
    int simEndTime = 0;
    std::vector<std::string> guiSettingsFiles;
    bool osgView = false;
    OptionsCont& oc = OptionsCont::getOptions();

    // register message callbacks
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetriever);

    // try to load the given configuration
    if (!initOptions()) {
        // the options are not valid but maybe we want to quit
        GUIGlobals::gQuitOnEnd = oc.getBool("quit-on-end");
        submitEndAndCleanup(net, simStartTime, simEndTime);
        return 0;
    }
    // within gui-based applications, nothing is reported to the console
    MsgHandler::getMessageInstance()->removeRetriever(&OutputDevice::getDevice("stdout"));
    MsgHandler::getWarningInstance()->removeRetriever(&OutputDevice::getDevice("stderr"));
    MsgHandler::getErrorInstance()->removeRetriever(&OutputDevice::getDevice("stderr"));
    // do this once again to get parsed options
    MsgHandler::initOutputOptions();
    XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
    GUIGlobals::gRunAfterLoad = oc.getBool("start");
    GUIGlobals::gQuitOnEnd = oc.getBool("quit-on-end");

    if (!MSFrame::checkOptions()) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        submitEndAndCleanup(net, simStartTime, simEndTime);
        return 0;
    }

    // initialise global settings
    RandHelper::initRandGlobal();
    RandHelper::initRandGlobal(&MSVehicleControl::myVehicleParamsRNG);
    MSFrame::setMSGlobals(oc);
    GUITexturesHelper::allowTextures(!oc.getBool("disable-textures"));
    MSVehicleControl* vehControl = 0;
#ifdef HAVE_INTERNAL
    GUIVisualizationSettings::UseMesoSim = MSGlobals::gUseMesoSim;
    if (MSGlobals::gUseMesoSim) {
        vehControl = new GUIMEVehicleControl();
    } else
#endif
        vehControl = new GUIVehicleControl();

    GUIEdgeControlBuilder* eb = 0;
    try {
        net = new GUINet(
            vehControl,
            new GUIEventControl(),
            new GUIEventControl(),
            new GUIEventControl());
        eb = new GUIEdgeControlBuilder();
        GUIDetectorBuilder db(*net);
        NLJunctionControlBuilder jb(*net, db);
        GUITriggerBuilder tb;
        NLHandler handler("", *net, db, tb, *eb, jb);
        tb.setHandler(&handler);
        NLBuilder builder(oc, *net, *eb, jb, db, handler);
        MsgHandler::getErrorInstance()->clear();
        MsgHandler::getWarningInstance()->clear();
        MsgHandler::getMessageInstance()->clear();
        if (!builder.build()) {
            throw ProcessError();
        } else {
            net->initGUIStructures();
            simStartTime = string2time(oc.getString("begin"));
            simEndTime = string2time(oc.getString("end"));
            guiSettingsFiles = oc.getStringVector("gui-settings-file");
#ifdef HAVE_INTERNAL
            osgView = oc.getBool("osg-view");
#endif
        }
    } catch (ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        delete net;
        net = 0;
#ifndef _DEBUG
    } catch (std::exception& e) {
        WRITE_ERROR(e.what());
        delete net;
        net = 0;
#endif
    }
    if (net == 0) {
        MSNet::clearAll();
    }
    delete eb;
    submitEndAndCleanup(net, simStartTime, simEndTime, guiSettingsFiles, osgView);
    return 0;
}



void
GUILoadThread::submitEndAndCleanup(GUINet* net,
                                   const SUMOTime simStartTime,
                                   const SUMOTime simEndTime,
                                   const std::vector<std::string>& guiSettingsFiles,
                                   const bool osgView) {
    // remove message callbacks
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetriever);
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    // inform parent about the process
    GUIEvent* e = new GUIEvent_SimulationLoaded(net, simStartTime, simEndTime, myFile, guiSettingsFiles, osgView);
    myEventQue.add(e);
    myEventThrow.signal();
}


bool
GUILoadThread::initOptions() {
    try {
        OptionsCont& oc = OptionsCont::getOptions();
        oc.clear();
        MSFrame::fillOptions();
        if (myFile != "") {
            if (myLoadNet) {
                oc.set("net-file", myFile);
            } else {
                oc.set("configuration-file", myFile);
            }
            OptionsIO::getOptions(true, 1, 0);
        } else {
            OptionsIO::getOptions(true);
        }
        return true;
    } catch (ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
    }
    return false;
}


void
GUILoadThread::load(const std::string& file, bool isNet) {
    myFile = file;
    myLoadNet = isNet;
    start();
}


void
GUILoadThread::retrieveMessage(const MsgHandler::MsgType type, const std::string& msg) {
    GUIEvent* e = new GUIEvent_Message(type, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


const std::string&
GUILoadThread::getFileName() const {
    return myFile;
}



/****************************************************************************/

