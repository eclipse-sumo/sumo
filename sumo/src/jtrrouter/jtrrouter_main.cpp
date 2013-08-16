/****************************************************************************/
/// @file    jtrrouter_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 20 Jan 2004
/// @version $Id$
///
// Main for JTRROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2013 DLR (http://www.dlr.de/) and contributors
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

#include <xercesc/sax/SAXException.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <utils/common/TplConvert.h>
#include <iostream>
#include <string>
#include <limits.h>
#include <ctime>
#include <set>
#include <router/ROFrame.h>
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/ToString.h>
#include <utils/common/RandHelper.h>
#include <utils/common/StringTokenizer.h>
#include <utils/xml/XMLSubSys.h>
#include "ROJTREdgeBuilder.h"
#include "ROJTRRouter.h"
#include "ROJTREdge.h"
#include "ROJTRTurnDefLoader.h"
#include "ROJTRFrame.h"
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// functions
// ===========================================================================
/* -------------------------------------------------------------------------
 * data processing methods
 * ----------------------------------------------------------------------- */
/**
 * loads the net
 * The net is in this meaning made up by the net itself and the dynamic
 * weights which may be supplied in a separate file
 */
void
initNet(RONet& net, ROLoader& loader,
        const std::vector<SUMOReal>& turnDefs) {
    // load the net
    ROJTREdgeBuilder builder;
    loader.loadNet(net, builder);
    // set the turn defaults
    const std::map<std::string, ROEdge*>& edges = net.getEdgeMap();
    for (std::map<std::string, ROEdge*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        static_cast<ROJTREdge*>((*i).second)->setTurnDefaults(turnDefs);
    }
}

std::vector<SUMOReal>
getTurningDefaults(OptionsCont& oc) {
    std::vector<SUMOReal> ret;
    std::vector<std::string> defs = oc.getStringVector("turn-defaults");
    if (defs.size() < 2) {
        throw ProcessError("The defaults for turnings must be a tuple of at least two numbers divided by ','.");
    }
    for (std::vector<std::string>::const_iterator i = defs.begin(); i != defs.end(); ++i) {
        try {
            SUMOReal val = TplConvert::_2SUMOReal((*i).c_str());
            ret.push_back(val);
        } catch (NumberFormatException&) {
            throw ProcessError("A turn default is not numeric.");
        }
    }
    return ret;
}


void
loadJTRDefinitions(RONet& net, OptionsCont& oc) {
    // load the turning definitions (and possible sink definition)
    if (oc.isSet("turn-ratio-files")) {
        ROJTRTurnDefLoader loader(net);
        std::vector<std::string> ratio_files = oc.getStringVector("turn-ratio-files");
        for (std::vector<std::string>::const_iterator i = ratio_files.begin(); i != ratio_files.end(); ++i) {
            if (!XMLSubSys::runParser(loader, *i)) {
                throw ProcessError();
            }
        }
    }
    if (MsgHandler::getErrorInstance()->wasInformed() && oc.getBool("ignore-errors")) {
        MsgHandler::getErrorInstance()->clear();
    }
    // parse sink edges specified at the input/within the configuration
    if (oc.isSet("sink-edges")) {
        std::vector<std::string> edges = oc.getStringVector("sink-edges");
        for (std::vector<std::string>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
            ROJTREdge* edge = static_cast<ROJTREdge*>(net.getEdge(*i));
            if (edge == 0) {
                throw ProcessError("The edge '" + *i + "' declared as a sink is not known.");
            }
            edge->setType(ROEdge::ET_SINK);
        }
    }
}


/**
 * Computes the routes saving them
 */
void
computeRoutes(RONet& net, ROLoader& loader, OptionsCont& oc) {
    // initialise the loader
    loader.openRoutes(net);
    // prepare the output
    net.openOutput(oc.getString("output-file"), "", oc.getString("vtype-output"));
    // build the router
    ROJTRRouter router(net, oc.getBool("ignore-errors"), oc.getBool("accept-all-destinations"),
                       (int)(((SUMOReal) net.getEdgeNo()) * OptionsCont::getOptions().getFloat("max-edges-factor")),
                       oc.getBool("ignore-vclasses"), oc.getBool("allow-loops"));
    loader.processRoutes(string2time(oc.getString("begin")), string2time(oc.getString("end")), net, router);
    net.closeOutput();
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Router for the microscopic road traffic simulation SUMO based on junction turning ratios.");
    oc.setApplicationName("jtrrouter", "SUMO jtrrouter Version " + (std::string)VERSION_STRING);
    int ret = 0;
    RONet* net = 0;
    try {
        // initialise the application system (messaging, xml, options)
        XMLSubSys::init();
        ROJTRFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getBool("xml-validation"));
        MsgHandler::initOutputOptions();
        if (!ROJTRFrame::checkOptions()) {
            throw ProcessError();
        }
        RandHelper::initRandGlobal();
        std::vector<SUMOReal> defs = getTurningDefaults(oc);
        // load data
        ROLoader loader(oc, true, !oc.getBool("no-step-log"));
        net = new RONet();
        initNet(*net, loader, defs);
        try {
            // parse and set the turn defaults first
            loadJTRDefinitions(*net, oc);
            // build routes
            computeRoutes(*net, loader, oc);
        } catch (XERCES_CPP_NAMESPACE::SAXParseException& e) {
            WRITE_ERROR(toString(e.getLineNumber()));
            ret = 1;
        } catch (XERCES_CPP_NAMESPACE::SAXException& e) {
            WRITE_ERROR(TplConvert::_2str(e.getMessage()));
            ret = 1;
        }
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            throw ProcessError();
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
    if (ret == 0) {
        std::cout << "Success." << std::endl;
    }
    return ret;
}



/****************************************************************************/
