/****************************************************************************/
/// @file    GUIAbstractLoadThread.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class describing the thread that performs the loading of a simulation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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

#include <iostream>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <utils/foxtools/MFXEventQue.h>
#include <utils/common/MsgRetrievingFunction.h>
#include "GUIAbstractLoadThread.h"
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>

#include <ctime>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;
using namespace FXEX;


// ===========================================================================
// member method definitions
// ===========================================================================
GUIAbstractLoadThread::GUIAbstractLoadThread(MFXInterThreadEventClient *mw,
        MFXEventQue &eq, FXEX::FXThreadEvent &ev)
        : FXSingleEventThread(gFXApp, mw), myParent(mw), myEventQue(eq),
        myEventThrow(ev)
{
    myErrorRetriever = new MsgRetrievingFunction<GUIAbstractLoadThread>(this,
            &GUIAbstractLoadThread::retrieveMessage, MsgHandler::MT_ERROR);
    myMessageRetriever = new MsgRetrievingFunction<GUIAbstractLoadThread>(this,
            &GUIAbstractLoadThread::retrieveMessage, MsgHandler::MT_MESSAGE);
    myWarningRetreiver = new MsgRetrievingFunction<GUIAbstractLoadThread>(this,
            &GUIAbstractLoadThread::retrieveMessage, MsgHandler::MT_WARNING);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
}


GUIAbstractLoadThread::~GUIAbstractLoadThread()
{
    delete myErrorRetriever;
    delete myMessageRetriever;
    delete myWarningRetreiver;
}


void
GUIAbstractLoadThread::load(const std::string &file,
                            bool isNet)
{
    myFile = file;
    myLoadNet = isNet;
    start();
}


void
GUIAbstractLoadThread::retrieveMessage(const MsgHandler::MsgType type, const std::string &msg)
{
    GUIEvent *e = new GUIEvent_Message(type, msg);
    myEventQue.add(e);
    myEventThrow.signal();
}


const std::string &
GUIAbstractLoadThread::getFileName() const
{
    return myFile;
}



/****************************************************************************/

