/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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
/// @file    GUIRunThread.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2002
///
// The thread that runs the simulation
/****************************************************************************/
#include <config.h>

#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>

#include <guisim/GUINet.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include "GUIEvent_SimulationEnded.h"
#include "GUIApplicationWindow.h"
#include "GUIRunThread.h"
#include "GUIGlobals.h"
#include <microsim/MSVehicleControl.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/SysUtils.h>
#include <utils/common/MsgRetrievingFunction.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/iodevices/OutputDevice.h>
#include <traci-server/TraCIServer.h>
#include <libsumo/Simulation.h>


// ===========================================================================
// member method definitions
// ===========================================================================
GUIRunThread::GUIRunThread(FXApp* app, MFXInterThreadEventClient* parent, double& simDelay,
                           MFXSynchQue<GUIEvent*>& eq, FXEX::MFXThreadEvent& ev) :
    MFXSingleEventThread(app, parent),
    myNet(nullptr),
    myHalting(true),
    myQuit(false),
    mySimulationInProgress(false),
    myOk(true),
    myHaveSignaledEnd(false),
    mySimDelay(simDelay),
    myEventQue(eq),
    myEventThrow(ev),
    myLastEndMillis(-1),
    myLastBreakMillis(0),
    myAmLibsumo(false) {
    myErrorRetriever = new MsgRetrievingFunction<GUIRunThread>(this, &GUIRunThread::retrieveMessage, MsgHandler::MsgType::MT_ERROR);
    myMessageRetriever = new MsgRetrievingFunction<GUIRunThread>(this, &GUIRunThread::retrieveMessage, MsgHandler::MsgType::MT_MESSAGE);
    myWarningRetriever = new MsgRetrievingFunction<GUIRunThread>(this, &GUIRunThread::retrieveMessage, MsgHandler::MsgType::MT_WARNING);
}


GUIRunThread::~GUIRunThread() {
    // the thread shall stop
    myQuit = true;
    deleteSim();
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetriever;
    // wait for the thread
    while (mySimulationInProgress || myNet != nullptr);
}


bool
GUIRunThread::init(GUINet* net, SUMOTime start, SUMOTime end) {
    assert(net != 0);
    // assign new values
    myOk = true;
    myNet = net;
    mySimStartTime = start;
    mySimEndTime = end;
    myHaveSignaledEnd = false;
    // register message callbacks
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    if (!OptionsCont::getOptions().getBool("no-warnings")) {
        MsgHandler::getWarningInstance()->addRetriever(myWarningRetriever);
    }
    // preload the routes especially for TraCI
    mySimulationLock.lock();
    try {
        net->setCurrentTimeStep(start);
        net->loadRoutes();
    } catch (ProcessError& e2) {
        if (std::string(e2.what()) != std::string("Process Error") && std::string(e2.what()) != std::string("")) {
            WRITE_ERROR(e2.what());
        }
        MsgHandler::getErrorInstance()->inform(TL("Quitting (on error)."), false);
        myHalting = true;
        myOk = false;
        mySimulationInProgress = false;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform(TL("Quitting (on error)."), false);
        myHalting = true;
        myOk = false;
        mySimulationInProgress = false;
#endif
    }
    mySimulationLock.unlock();
    return myOk;
}


FXint
GUIRunThread::run() {
    // perform an endless loop
    while (!myQuit) {
        if (myAmLibsumo) {
            myApp->run();
        } else {
            // if the simulation shall be performed, do it
            tryStep();
        }
    }
    // delete a maybe existing simulation at the end
    deleteSim();
    return 0;
}


void
GUIRunThread::tryStep() {
    if (!myHalting && myNet != nullptr && myOk) {
        const long beg = SysUtils::getCurrentMillis();
        if (myLastEndMillis != -1) {
            getNet().setIdleDuration((int)(beg - myLastEndMillis));
        }
        // check whether we shall stop at this step
        myBreakpointLock.lock();
        const bool haltAfter = std::find(myBreakpoints.begin(), myBreakpoints.end(), myNet->getCurrentTimeStep()) != myBreakpoints.end();
        myBreakpointLock.unlock();
        // stop after this step if wished
        if (haltAfter) {
            stop();
        }
        // stop the execution when only a single step should have been performed
        if (mySingle) {
            myHalting = true;
        }
        // do the step
        makeStep();
        waitForSnapshots(myNet->getCurrentTimeStep() - DELTA_T);
        // wait if wanted (delay is per simulated second)
        long wait = (long)(mySimDelay * TS);
        myLastEndMillis = SysUtils::getCurrentMillis();
        getNet().setSimDuration((int)(myLastEndMillis - beg));
        wait -= (myLastEndMillis - beg);
        if (wait > 0) {
            myLastBreakMillis = myLastEndMillis;
            sleep(wait);
#ifndef WIN32
        } else if (myLastEndMillis - myLastBreakMillis > 1000) {
            // ensure redraw event is successful at least once per second (#9028)
            sleep(100);
            myLastBreakMillis = myLastEndMillis;
#endif
        }
    } else {
        // sleep if the simulation is not running
        sleep(50);
    }
}


void
GUIRunThread::makeStep() {
    GUIEvent* e = nullptr;
    // simulation is being performed
    mySimulationInProgress = true;
    // execute a single step
    try {
        mySimulationLock.lock();
        myNet->simulationStep();
        myNet->guiSimulationStep();
        mySimulationLock.unlock();

        // inform parent that a step has been performed
        e = new GUIEvent_SimulationStep();
        myEventQue.push_back(e);
        myEventThrow.signal();

        e = nullptr;
        MSNet::SimulationState state = myNet->adaptToState(myNet->simulationState(mySimEndTime), myAmLibsumo);
        switch (state) {
            case MSNet::SIMSTATE_LOADING:
            case MSNet::SIMSTATE_END_STEP_REACHED:
            case MSNet::SIMSTATE_NO_FURTHER_VEHICLES:
            case MSNet::SIMSTATE_CONNECTION_CLOSED:
            case MSNet::SIMSTATE_TOO_MANY_TELEPORTS:
                if (!myHaveSignaledEnd || state != MSNet::SIMSTATE_END_STEP_REACHED) {
                    e = new GUIEvent_SimulationEnded(state, myNet->getCurrentTimeStep() - DELTA_T);
                    // ensure that files are closed (deleteSim is called a bit later by the gui thread)
                    // MSNet destructor may trigger MsgHandler (via routing device cleanup). Closing output devices here is not safe
                    // OutputDevice::closeAll();
                    myHaveSignaledEnd = true;
                }
                break;
            default:
                break;
        }
        if (e != nullptr) {
            myEventQue.push_back(e);
            myEventThrow.signal();
            myHalting = true;
        }
        // simulation step is over
        mySimulationInProgress = false;
    } catch (ProcessError& e2) {
        if (std::string(e2.what()) != std::string("Process Error") && std::string(e2.what()) != std::string("")) {
            WRITE_ERROR(e2.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        mySimulationLock.unlock();
        mySimulationInProgress = false;
        e = new GUIEvent_SimulationEnded(MSNet::SIMSTATE_ERROR_IN_SIM, myNet->getCurrentTimeStep());
        myEventQue.push_back(e);
        myEventThrow.signal();
        myHalting = true;
        myOk = false;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        mySimulationLock.unlock();
        mySimulationInProgress = false;
        e = new GUIEvent_SimulationEnded(MSNet::SIMSTATE_ERROR_IN_SIM, myNet->getCurrentTimeStep());
        myEventQue.push_back(e);
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
    // report the begin when wished
    WRITE_MESSAGEF(TL("Simulation started with time: %."), time2string(mySimStartTime));
    myOk = true;
}


void
GUIRunThread::stop() {
    mySingle = false;
    myHalting = true;
}


bool
GUIRunThread::networkAvailable() const {
    return myNet != nullptr;
}


void
GUIRunThread::deleteSim() {
    myHalting = true;
    // flush aggregated warnings
    MsgHandler::getWarningInstance()->clear();
    // remove message callbacks
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetriever);
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    //
    mySimulationLock.lock();
    if (myNet != nullptr) {
        myNet->closeSimulation(mySimStartTime, MSNet::getStateMessage(myNet->simulationState(mySimEndTime)));
    }
    while (mySimulationInProgress) {
        sleep(50);
    }
    delete myNet;
    GUIGlObjectStorage::gIDStorage.clear();
    myNet = nullptr;
    OutputDevice::closeAll();
    mySimulationLock.unlock();
    MsgHandler::cleanupOnEnd();
}


GUINet&
GUIRunThread::getNet() const {
    return *myNet;
}


void
GUIRunThread::prepareDestruction() {
    myHalting = true;
    myQuit = true;
}


void
GUIRunThread::retrieveMessage(const MsgHandler::MsgType type, const std::string& msg) {
    GUIEvent* e = new GUIEvent_Message(type, msg);
    myEventQue.push_back(e);
    myEventThrow.signal();
}


bool
GUIRunThread::simulationIsStartable() const {
    return myNet != nullptr && myHalting;
}


bool
GUIRunThread::simulationIsStopable() const {
    return myNet != nullptr && (!myHalting);
}


bool
GUIRunThread::simulationIsStepable() const {
    return myNet != nullptr && myHalting;
}


void
GUIRunThread::waitForSnapshots(const SUMOTime snapshotTime) {
    GUIMainWindow* const mw = GUIMainWindow::getInstance();
    if (mw != nullptr) {
        for (GUIGlChildWindow* const window : mw->getViews()) {
            window->getView()->waitForSnapshots(snapshotTime);
        }
    }
}


/****************************************************************************/
