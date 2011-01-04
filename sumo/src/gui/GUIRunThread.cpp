/****************************************************************************/
/// @file    GUIRunThread.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The thread that runs the simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>

#include <utils/common/MsgRetrievingFunction.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <guisim/GUINet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSEmitControl.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include "GUIEvent_SimulationEnded.h"
#include "GUIApplicationWindow.h"
#include "GUIRunThread.h"
#include "GUIGlobals.h"
#include <utils/options/OptionsCont.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/common/SysUtils.h>
#include <utils/iodevices/OutputDevice.h>

#ifndef NO_TRACI
#include <traci-server/TraCIServer.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace FXEX;
using namespace std;


// ===========================================================================
// member method definitions
// ===========================================================================
GUIRunThread::GUIRunThread(MFXInterThreadEventClient *parent,
                           FXRealSpinDial &simDelay, MFXEventQue &eq,
                           FXEX::FXThreadEvent &ev)
        : FXSingleEventThread(gFXApp, parent),
        myNet(0), myQuit(false), mySimulationInProgress(false), myOk(true),
        mySimDelay(simDelay), myEventQue(eq), myEventThrow(ev) {
    myErrorRetriever = new MsgRetrievingFunction<GUIRunThread>(this, &GUIRunThread::retrieveMessage, MsgHandler::MT_ERROR);
    myMessageRetriever = new MsgRetrievingFunction<GUIRunThread>(this, &GUIRunThread::retrieveMessage, MsgHandler::MT_MESSAGE);
    myWarningRetriever = new MsgRetrievingFunction<GUIRunThread>(this, &GUIRunThread::retrieveMessage, MsgHandler::MT_WARNING);
}


GUIRunThread::~GUIRunThread() {
    // the thread shall stop
    myQuit = true;
    deleteSim();
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetriever;
    // wait for the thread
    while (mySimulationInProgress||myNet!=0);
}


void
GUIRunThread::init(GUINet *net, SUMOTime start, SUMOTime end) {
    // assign new values
    myNet = net;
    mySimStartTime = start;
    mySimEndTime = end;
    // register message callbacks
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetriever);
}


FXint
GUIRunThread::run() {
    long beg = 0;
    long end = 0;
    long end2 = -1;
    // perform an endless loop
    while (!myQuit) {
        // if the simulation shall be perfomed, do it
        if (!myHalting&&myNet!=0&&myOk) {
            if (getNet().logSimulationDuration()) {
                beg = SysUtils::getCurrentMillis();
                if (end2!=-1) {
                    getNet().setIdleDuration((int)(beg-end2));
                }
            }
            // check whether we shall stop at this step
            bool haltAfter = find(gBreakpoints.begin(), gBreakpoints.end(), myNet->getCurrentTimeStep())!=gBreakpoints.end();
            // do the step
            makeStep();
            // stop if wished
            if (haltAfter) {
                stop();
            }
            // wait if wanted
            SUMOReal val = (SUMOReal) mySimDelay.getValue();
            if (getNet().logSimulationDuration()) {
                end = SysUtils::getCurrentMillis();
                getNet().setSimDuration((int)(end-beg));
                end2 = SysUtils::getCurrentMillis();
            }
            if ((int) val!=0) {
                sleep((int) val);
            }
        } else {
            // sleep if the siulation is not running
            sleep(500);
        }
    }
    // delete a maybe existing simulation at the end
    deleteSim();
    return 0;
}


void
GUIRunThread::makeStep() throw() {
    GUIEvent *e = 0;
    // simulation is being perfomed
    mySimulationInProgress = true;
    // execute a single step
    try {
        mySimulationLock.lock();
        myNet->simulationStep();
        myNet->guiSimulationStep();
        mySimulationLock.unlock();

        // inform parent that a step has been performed
        e = new GUIEvent_SimulationStep();
        myEventQue.add(e);
        myEventThrow.signal();

        e = 0;
        MSNet::SimulationState state = myNet->simulationState(mySimEndTime);
#ifndef NO_TRACI
        if (state!=MSNet::SIMSTATE_RUNNING) {
            if (OptionsCont::getOptions().getInt("remote-port")!=0&&!traci::TraCIServer::wasClosed()) {
                state = MSNet::SIMSTATE_RUNNING;
            }
        }
#endif
        switch (state) {
        case MSNet::SIMSTATE_END_STEP_REACHED:
        case MSNet::SIMSTATE_NO_FURTHER_VEHICLES:
        case MSNet::SIMSTATE_CONNECTION_CLOSED:
        case MSNet::SIMSTATE_TOO_MANY_VEHICLES:
            e = new GUIEvent_SimulationEnded(state, myNet->getCurrentTimeStep()-DELTA_T);
            break;
        default:
            break;
        }
        if (e!=0) {
            myEventQue.add(e);
            myEventThrow.signal();
            myHalting = true;
        }
        // stop the execution when only a single step should have
        //  been performed
        if (mySingle) {
            myHalting = true;
        }
        // simulation step is over
        mySimulationInProgress = false;
    } catch (ProcessError &e2) {
        if (string(e2.what())!=string("Process Error") && std::string(e2.what())!=string("")) {
            MsgHandler::getErrorInstance()->inform(e2.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        mySimulationLock.unlock();
        mySimulationInProgress = false;
        e = new GUIEvent_SimulationEnded(MSNet::SIMSTATE_ERROR_IN_SIM, myNet->getCurrentTimeStep());
        myEventQue.add(e);
        myEventThrow.signal();
        myHalting = true;
        myOk = false;
#ifndef _DEBUG
    } catch (...) {
        mySimulationLock.unlock();
        mySimulationInProgress = false;
        e = new GUIEvent_SimulationEnded(MSNet::SIMSTATE_ERROR_IN_SIM, myNet->getCurrentTimeStep());
        myEventQue.add(e);
        myEventThrow.signal();
        myHalting = true;
        myOk = false;
#endif
    }
}


void
GUIRunThread::resume() {
    mySingle = false;
    myHalting = false;
}


void
GUIRunThread::singleStep() {
    mySingle = true;
    myHalting = false;
}


void
GUIRunThread::begin() {
    myOk = true;
}


void
GUIRunThread::stop() {
    mySingle = false;
    myHalting = true;
}


bool
GUIRunThread::simulationAvailable() const {
    return myNet!=0;
}


void
GUIRunThread::deleteSim() {
    myHalting = true;
    // remove message callbacks
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetriever);
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    //
    mySimulationLock.lock();
    if (myNet!=0) {
        myNet->closeSimulation(mySimStartTime);
    }
    while (mySimulationInProgress);
    delete myNet;
    myNet = 0;
    OutputDevice::closeAll();
    mySimulationLock.unlock();
    MsgHandler::cleanupOnEnd();
}


GUINet &
GUIRunThread::getNet() const {
    return *myNet;
}


void
GUIRunThread::prepareDestruction() {
    myHalting = true;
    myQuit = true;
}


void
GUIRunThread::retrieveMessage(const MsgHandler::MsgType type, const std::string &msg) {
    GUIEvent *e = new GUIEvent_Message(type, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


bool
GUIRunThread::simulationIsStartable() const {
    return myNet!=0&&myHalting;
}


bool
GUIRunThread::simulationIsStopable() const {
    return myNet!=0&&(!myHalting);
}


bool
GUIRunThread::simulationIsStepable() const {
    return myNet!=0&&myHalting;
}



/****************************************************************************/

