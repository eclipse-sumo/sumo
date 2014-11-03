/****************************************************************************/
/// @file    sumo_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Thimor Bohn
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Main for SUMO
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
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

#ifdef HAVE_VERSION_H
#include <version.h>
#endif

#include <ctime>
#include <string>
#include <iostream>
#include <microsim/MSGlobals.h>
#include <microsim/MSNet.h>
#include <microsim/MSRoute.h>
#include <microsim/MSVehicleControl.h>
#include <netload/NLBuilder.h>
#include <netload/NLHandler.h>
#include <netload/NLTriggerBuilder.h>
#include <netload/NLEdgeControlBuilder.h>
#include <netload/NLJunctionControlBuilder.h>
#include <netload/NLDetectorBuilder.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/MSFrame.h>
#include <microsim/output/MSDetectorControl.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef HAVE_INTERNAL
#include <mesosim/MEVehicleControl.h>
#endif

#ifndef NO_TRACI
#include <traci-server/TraCIServer.h>
#endif


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif

// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * loads the net, additional routes and the detectors
 */
MSNet*
load(OptionsCont& oc) {
    MSFrame::setMSGlobals(oc);
    MSVehicleControl* vc = 0;
#ifdef HAVE_INTERNAL
    if (MSGlobals::gUseMesoSim) {
        vc = new MEVehicleControl();
    } else {
#endif
        vc = new MSVehicleControl();
#ifdef HAVE_INTERNAL
    }
#endif
    MSNet* net = new MSNet(vc, new MSEventControl(),
                           new MSEventControl(), new MSEventControl());
#ifndef NO_TRACI
    // need to init TraCI-Server before loading routes to catch VEHICLE_STATE_BUILT
    TraCIServer::openSocket(std::map<int, TraCIServer::CmdExecutor>());
#endif

    NLEdgeControlBuilder eb;
    NLDetectorBuilder db(*net);
    NLJunctionControlBuilder jb(*net, db);
    NLTriggerBuilder tb;
    NLHandler handler("", *net, db, tb, eb, jb);
    tb.setHandler(&handler);
    NLBuilder builder(oc, *net, eb, jb, db, handler);
    if (!builder.build()) {
        delete net;
        throw ProcessError();
    }
    return net;
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("A microscopic road traffic simulation.");
    oc.setApplicationName("sumo", "SUMO Version " + getBuildName(VERSION_STRING));
    int ret = 0;
    MSNet* net = 0;
    try {
        // initialise subsystems
        XMLSubSys::init();
        MSFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
        MsgHandler::initOutputOptions();
        if (!MSFrame::checkOptions()) {
            throw ProcessError();
        }
        RandHelper::initRandGlobal();
        RandHelper::initRandGlobal(MSRouteHandler::getParsingRNG());
        // load the net
        net = load(oc);
        if (net != 0) {
            ret = net->simulate(string2time(oc.getString("begin")), string2time(oc.getString("end")));
        }
    } catch (const ProcessError& e) {
        if (std::string(e.what()) != std::string("Process Error") && std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (const std::exception& e) {
        if (std::string(e.what()) != std::string("")) {
            WRITE_ERROR(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    delete net;
    SystemFrame::close();
    return ret;
}


/****************************************************************************/
