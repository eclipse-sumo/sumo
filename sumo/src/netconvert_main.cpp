/****************************************************************************/
/// @file    netconvert_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Nov 2001
/// @version $Id$
///
// Main for NETCONVERT
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

#include <iostream>
#include <string>
#include <netimport/NIFrame.h>
#include <netimport/NILoader.h>
#include <netbuild/NBFrame.h>
#include <netbuild/NBNetBuilder.h>
#include <netbuild/NBDistribution.h>
#include <netwrite/NWFrame.h>
#include <utils/options/OptionsIO.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/RandHelper.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/geom/GeoConvHelper.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
void
fillOptions() {
    OptionsCont& oc = OptionsCont::getOptions();
    oc.addCallExample("-c <CONFIGURATION>", "generate net with options read from file");
    oc.addCallExample("-n ./nodes.xml -e ./edges.xml -v -t ./owntypes.xml",
                      "generate net with given nodes, edges, and edge types doing verbose output");

    // insert options sub-topics
    SystemFrame::addConfigurationOptions(oc); // this subtopic is filled here, too
    oc.addOptionSubTopic("Input");
    oc.addOptionSubTopic("Output");
    GeoConvHelper::addProjectionOptions(oc);
    oc.addOptionSubTopic("TLS Building");
    oc.addOptionSubTopic("Ramp Guessing");
    oc.addOptionSubTopic("Edge Removal");
    oc.addOptionSubTopic("Unregulated Nodes");
    oc.addOptionSubTopic("Processing");
    oc.addOptionSubTopic("Building Defaults");
    SystemFrame::addReportOptions(oc); // this subtopic is filled here, too

    NIFrame::fillOptions();
    NBFrame::fillOptions(false);
    NWFrame::fillOptions(false);
    RandHelper::insertRandOptions();
}


bool
checkOptions() {
    bool ok = NIFrame::checkOptions();
    ok &= NBFrame::checkOptions();
    ok &= NWFrame::checkOptions();
    return ok;
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Road network importer / builder for the road traffic simulation SUMO.");
    oc.setApplicationName("netconvert", "SUMO netconvert Version " + getBuildName(VERSION_STRING));
    int ret = 0;
    try {
        XMLSubSys::init();
        fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
        MsgHandler::initOutputOptions();
        if (!checkOptions()) {
            throw ProcessError();
        }
        RandHelper::initRandGlobal();
        // build the projection
        if (!GeoConvHelper::init(oc)) {
            throw ProcessError("Could not build projection!");
        }
        NBNetBuilder nb;
        nb.applyOptions(oc);
        // load data
        NILoader nl(nb);
        nl.load(oc);
        if (oc.getBool("ignore-errors")) {
            MsgHandler::getErrorInstance()->clear();
        }
        // check whether any errors occured
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
        }
        nb.compute(oc);
        NWFrame::writeNetwork(oc, nb);
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
    NBDistribution::clear();
    SystemFrame::close();
    // report about ending
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/

