/****************************************************************************/
/// @file    AGMain.cpp
/// @author  Piotr Woznica
/// @author  Walter Bamberger
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Jul 2010
/// @version $Id$
///
// Main object of the ActivityGen application
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
// activitygen module
// Copyright 2010 TUM (Technische Universitaet Muenchen, http://www.tum.de/)
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

#include <iostream>
#include <exception>
#include <typeinfo>
#include <router/RONet.h>
#include <router/ROLoader.h>
#include <router/RONetHandler.h>
#include <duarouter/RODUAEdgeBuilder.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>
#include <utils/options/OptionsCont.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/iodevices/OutputDevice.h>
//ActivityGen
#include "AGFrame.h"
#include "AGActivityGen.h"
#include "city/AGTime.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================

/// Loads the network
void
loadNet(RONet& toFill, ROAbstractEdgeBuilder& eb) {
    OptionsCont& oc = OptionsCont::getOptions();
    std::string file = oc.getString("net-file");
    if (file == "") {
        throw ProcessError("Missing definition of network to load!");
    }
    if (!FileHelpers::exists(file)) {
        throw ProcessError("The network file '" + file + "' could not be found.");
    }
    PROGRESS_BEGIN_MESSAGE("Loading net");
    RONetHandler handler(toFill, eb);
    handler.setFileName(file);
    if (!XMLSubSys::runParser(handler, file)) {
        PROGRESS_FAILED_MESSAGE();
        throw ProcessError();
    } else {
        PROGRESS_DONE_MESSAGE();
    }
}


int
main(int argc, char* argv[]) {
    int ret = 0;
    OptionsCont& oc = OptionsCont::getOptions();
    RONet* net = 0;
    try {
        // Initialise subsystems and process options
        XMLSubSys::init();
        AGFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
        MsgHandler::initOutputOptions();
        RandHelper::initRandGlobal();

        // Load network
        net = new RONet();
        RODUAEdgeBuilder builder(oc.getBool("weights.expand"), oc.getBool("weights.interpolate"));
        loadNet(*net, builder);
        WRITE_MESSAGE("Loaded " + toString(net->getEdgeNoWithoutInternal()) + " edges.");
        if (oc.getBool("debug")) {
            WRITE_MESSAGE("\n\t ---- begin AcitivtyGen ----\n");
        }

        std::string statFile = oc.getString("stat-file");
        OutputDevice::createDeviceByOption("output-file", "routes");
        AGTime duration(1, 0, 0);
        AGTime begin(0);
        AGTime end(0);
        if (oc.isSet("duration-d")) {
            duration.setDay(oc.getInt("duration-d"));
        }
        if (oc.isSet("begin")) {
            begin.addSeconds(oc.getInt("begin") % 86400);
        }
        if (oc.isSet("end")) {
            end.addSeconds(oc.getInt("end") % 86400);
        }
        AGActivityGen actiGen(statFile, OutputDevice::getDevice(oc.getString("output-file")), net);
        actiGen.importInfoCity();
        actiGen.makeActivityTrips(duration.getDay(), begin.getTime(), end.getTime());

        if (oc.getBool("debug")) {
            WRITE_MESSAGE("\n\t ---- end of ActivityGen ----\n");
        }
        ret = 0;
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
    SystemFrame::close();
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}

/****************************************************************************/

