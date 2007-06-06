/****************************************************************************/
/// @file    GUIAbstractRunThread.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// The thread that runs the simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <gui/GUIApplicationWindow.h>
#include "GUIAbstractRunThread.h"
// #include "GUIGlobals.h"
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsSubSys.h>

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
GUIAbstractRunThread::GUIAbstractRunThread(GUIApplicationWindow *parent,
        FXRealSpinDial &simDelay, MFXEventQue &eq,
        FXEX::FXThreadEvent &ev)
        : FXSingleEventThread(parent->getApp(), parent),
        _net(0), _quit(false), _simulationInProgress(false), _ok(true),
        mySimDelay(simDelay), myEventQue(eq), myEventThrow(ev)
{
    myErrorRetriever = new MsgRetrievingFunction<GUIAbstractRunThread>(this,
                       &GUIAbstractRunThread::retrieveError);
    myMessageRetriever = new MsgRetrievingFunction<GUIAbstractRunThread>(this,
                         &GUIAbstractRunThread::retrieveMessage);
    myWarningRetreiver = new MsgRetrievingFunction<GUIAbstractRunThread>(this,
                         &GUIAbstractRunThread::retrieveWarning);
}


GUIAbstractRunThread::~GUIAbstractRunThread()
{
    // the thread shall stop
    _quit = true;
    deleteSim();
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetreiver;
    // wait for the thread
    while (_simulationInProgress||_net!=0);
}


FXint
GUIAbstractRunThread::run()
{
    return 0;
}


void
GUIAbstractRunThread::resume()
{
    if (_step<_simEndTime) {
        _single = false;
        _halting = false;
    }
}


void
GUIAbstractRunThread::singleStep()
{
    _single = true;
    _halting = false;
}


void
GUIAbstractRunThread::begin()
{}


void
GUIAbstractRunThread::stop()
{
    _single = false;
    _halting = true;
}


bool
GUIAbstractRunThread::simulationAvailable() const
{
    return _net!=0;
}


void
GUIAbstractRunThread::deleteSim()
{}



SUMOTime
GUIAbstractRunThread::getCurrentTimeStep() const
{
    return _step;
}



void
GUIAbstractRunThread::prepareDestruction()
{
    _halting = true;
    _quit = true;
}


void
GUIAbstractRunThread::retrieveMessage(const std::string &msg)
{
    if (!_simulationInProgress) {
        return;
    }
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_MESSAGE, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


void
GUIAbstractRunThread::retrieveWarning(const std::string &msg)
{
    if (!_simulationInProgress) {
        return;
    }
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_WARNING, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


void
GUIAbstractRunThread::retrieveError(const std::string &msg)
{
    if (!_simulationInProgress) {
        return;
    }
    GUIEvent *e = new GUIEvent_Message(MsgHandler::MT_ERROR, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


bool
GUIAbstractRunThread::simulationIsStartable() const
{
    return _net!=0&&(_halting||_single);
}


bool
GUIAbstractRunThread::simulationIsStopable() const
{
    return _net!=0&&(!_halting);
}


bool
GUIAbstractRunThread::simulationIsStepable() const
{
    return _net!=0&&(_halting||_single);
}



/****************************************************************************/

