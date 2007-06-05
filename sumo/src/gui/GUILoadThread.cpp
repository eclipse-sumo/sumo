/****************************************************************************/
/// @file    GUILoadThread.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// Class describing the thread that performs the loading of a simulation
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <iostream>
#include <guisim/GUINet.h>
#include <netload/NLBuilder.h>
#include <guinetload/GUIEdgeControlBuilder.h>
#include <guinetload/GUIJunctionControlBuilder.h>
#include <guinetload/GUIDetectorBuilder.h>
#include <guinetload/GUITriggerBuilder.h>
#include <guinetload/GUIGeomShapeBuilder.h>
#include <guinetload/GUIHandler.h>
#include <guisim/GUIVehicleControl.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/common/UtilExceptions.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/common/MsgHandler.h>
#include <utils/foxtools/MFXEventQue.h>
#include <sumo_only/SUMOFrame.h>
#include <utils/common/MsgRetrievingFunction.h>
#include "GUIApplicationWindow.h"
#include "GUILoadThread.h"
#include "GUIGlobals.h"
#include "GUIEvent_SimulationLoaded.h"
#include <utils/gui/events/GUIEvent_Message.h>
#include <utils/gui/events/GUIEvent_SimulationEnded.h>
#include <utils/gui/windows/GUIAppEnum.h>
#include <utils/gui/globjects/GUIGlObjectGlobals.h>
#include <utils/gui/windows/GUIAppGlobals.h>
#include <utils/common/RandHelper.h>
#include <ctime>
#include <utils/iodevices/SharedOutputDevices.h>

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
GUILoadThread::GUILoadThread(MFXInterThreadEventClient *mw,
                             MFXEventQue &eq, FXEX::FXThreadEvent &ev)
        : GUIAbstractLoadThread(mw, eq, ev)
{}


GUILoadThread::~GUILoadThread()
{}


FXint
GUILoadThread::run()
{
    GUINet *net = 0;
    int simStartTime = 0;
    int simEndTime = 0;

    // remove old options
    OptionsSubSys::close();

    // try to load the given configuration
    if (!initOptions()) {
        // ok, the options could not be set
        submitEndAndCleanup(net, simStartTime, simEndTime);
        return 0;
    }
    // retrieve the options
    OptionsCont &oc = OptionsSubSys::getOptions();
    if (!SUMOFrame::checkOptions(oc)) {
        // the options are not valid
        submitEndAndCleanup(net, simStartTime, simEndTime);
        return 0;
    }
    // register message callbacks
    MsgHandler::getMessageInstance()->addRetriever(myMessageRetriever);
    MsgHandler::getErrorInstance()->addRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->addRetriever(myWarningRetreiver);

    // try to load
    SUMOFrame::setMSGlobals(oc);
    net =
        new GUINet(oc.getInt("begin"), oc.getInt("end"),
                   buildVehicleControl(),
                   oc.getFloat("too-slow-rtf"), !oc.getBool("no-duration-log"));
    GUIEdgeControlBuilder *eb = buildEdgeBuilder();
    GUIJunctionControlBuilder jb(*net, oc);
    GUIDetectorBuilder db(*net);
    GUIGeomShapeBuilder sb(*net, gIDStorage);
    GUITriggerBuilder tb;
    GUIHandler handler("", *net, db, tb, *eb, jb, sb, oc.getInt("incremental-dua-base"), oc.getInt("incremental-dua-step"));
    NLBuilder builder(oc, *net, *eb, jb, db, tb, sb, handler);
    try {
        MsgHandler::getErrorInstance()->clear();
        MsgHandler::getWarningInstance()->clear();
        MsgHandler::getMessageInstance()->clear();
        initDevices();
        if (!builder.build()) {
            delete net;
            net = 0;
        } else {
            net->initGUIStructures();
            simStartTime = oc.getInt("begin");
            simEndTime = oc.getInt("end");
            closeNetLoadingDependent(oc, *net);
            RandHelper::initRandGlobal(oc);
        }
    } catch (ProcessError &e) {
        if(string(e.what())!=string("Process Error") && string(e.what())!=string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        delete net;
        MSNet::clearAll();
        net = 0;
#ifndef _DEBUG
    } catch (exception &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        delete net;
        MSNet::clearAll();
        net = 0;
#endif
    }
    if (net==0) {
        MSNet::clearAll();
    }
    delete eb;
    submitEndAndCleanup(net, simStartTime, simEndTime);
    return 0;
}



GUIEdgeControlBuilder *
GUILoadThread::buildEdgeBuilder()
{
    return new GUIEdgeControlBuilder(gIDStorage);
}




GUIVehicleControl*
GUILoadThread::buildVehicleControl()
{
    return new GUIVehicleControl();
}


void
GUILoadThread::initDevices()
{
    SharedOutputDevices::setInstance(new SharedOutputDevices());
}


void
GUILoadThread::submitEndAndCleanup(GUINet *net,
                                   int simStartTime,
                                   int simEndTime)
{
    // remove message callbacks
    MsgHandler::getErrorInstance()->removeRetriever(myErrorRetriever);
    MsgHandler::getWarningInstance()->removeRetriever(myWarningRetreiver);
    MsgHandler::getMessageInstance()->removeRetriever(myMessageRetriever);
    // inform parent about the process
    GUIEvent *e = new GUIEvent_SimulationLoaded(net, simStartTime, simEndTime, _file);
    myEventQue.add(e);
    myEventThrow.signal();
}


bool
GUILoadThread::initOptions()
{
    if (myLoadNet) {
        return OptionsSubSys::guiInit(SUMOFrame::fillOptions, "net-file", _file);
    } else {
        return OptionsSubSys::guiInit(SUMOFrame::fillOptions, "configuration-file", _file);
    }
}



/****************************************************************************/

