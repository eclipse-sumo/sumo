/****************************************************************************/
/// @file    GUIRunThread.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The thread that runs the simulation
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

#include <cassert>
#include <string>
#include <iostream>
#include <algorithm>

#include <utils/common/MsgRetrievingFunction.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <guisim/GUINet.h>
#include <microsim/MSVehicleControl.h>
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/events/GUIEvent_SimulationStep.h>
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include "GUIApplicationWindow.h"
#include "GUIRunThread.h"
#include "GUIGlobals.h"
#include <utils/options/OptionsCont.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/common/SysUtils.h>
#include <utils/iodevices/OutputDevice.h>

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
        myNet(0), myQuit(false), myOk(true),
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
    while (myNet!=0);
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
            bool haltAfter =
                find(gBreakpoints.begin(), gBreakpoints.end(), myNet->getCurrentTimeStep())!=gBreakpoints.end();
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
    bool hadProblem = true;
    // simulation is being perfomed
    mySimulationLock.lock();
    try {
        myNet->simulationStep();
        myNet->guiSimulationStep();
        hadProblem = false;
    } catch (ProcessError &e) {
        if (string(e.what())!=string("Process Error") && string(e.what())!=string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
#ifndef _DEBUG
    } catch (...) {
#endif
    }
    if (!hadProblem) {
        // inform parent that a step has been performed
        myEventQue.add(new GUIEvent_SimulationStep());
        myEventThrow.signal();
        // stop the simulation when the last step has been reached
        if (myNet->getCurrentTimeStep()>=mySimEndTime) {
            myEventQue.add(new GUIEvent_SimulationEnded(GUIEvent_SimulationEnded::ER_END_STEP_REACHED, myNet->getCurrentTimeStep()-DELTA_T));
            myEventThrow.signal();
            myHalting = true;
        }
        // stop the execution when only a single step should have
        //  been performed
        if (mySingle) {
            myHalting = true;
        }
        // check whether all vehicles loaded have left the simulation
        if (mySimEndTime == INT_MAX && myNet->getVehicleControl().haveAllVehiclesQuit()) {
            myEventQue.add(new GUIEvent_SimulationEnded(GUIEvent_SimulationEnded::ER_NO_VEHICLES, myNet->getCurrentTimeStep()-DELTA_T));
            myEventThrow.signal();
            myHalting = true;
        }
    } else {
        mySimulationLock.unlock();
        myEventQue.add(new GUIEvent_SimulationEnded(GUIEvent_SimulationEnded::ER_ERROR_IN_SIM, myNet->getCurrentTimeStep()));
        myEventThrow.signal();
        myHalting = true;
        myOk = false;
    }
    mySimulationLock.unlock();
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

