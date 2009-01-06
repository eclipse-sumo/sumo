/****************************************************************************/
/// @file    duarouter_main.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 06 Jun 2002
/// @version $Id$
///
// Main for DUAROUTER
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
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROEdge.h>
#include <router/ROVehicleType_Krauss.h>
#include <utils/common/SUMODijkstraRouter.h>
#include <routing_dua/RODUAEdgeBuilder.h>
#include <router/ROFrame.h>
#include <utils/common/MsgHandler.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/options/OptionsIO.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SystemFrame.h>
#include <utils/common/RandHelper.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <routing_dua/RODUAFrame.h>
#include <utils/iodevices/OutputDevice.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


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
initNet(RONet &net, ROLoader &loader, OptionsCont &oc)
{
    // load the net
    RODUAEdgeBuilder builder(oc.getBool("expand-weights"));
    loader.loadNet(net, builder);
    // load the weights when wished/available
    if (oc.isSet("weights")) {
        loader.loadWeights(net, oc.getString("weights"), false);
    }
    if (oc.isSet("lane-weights")) {
        loader.loadWeights(net, oc.getString("lane-weights"), true);
    }
    if (oc.isSet("S")) {
        loader.loadSupplementaryWeights(net);
    }
}



/**
 * Computes the routes saving them
 */
void
computeRoutes(RONet &net, ROLoader &loader, OptionsCont &oc)
{
    // initialise the loader
    loader.openRoutes(net);
    // prepare the output
    try {
        net.openOutput(oc.getString("output"), true);
    } catch (IOError &e) {
        throw e;
    }
    // build the router
    SUMOAbstractRouter<ROEdge, ROVehicle> *router;
    if (net.hasRestrictions()) {
        router = new SUMODijkstraRouter_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
            net.getEdgeNo(), oc.getBool("continue-on-unbuild"), &ROEdge::getEffort);
    } else {
        router = new SUMODijkstraRouter_Direct<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
            net.getEdgeNo(), oc.getBool("continue-on-unbuild"), &ROEdge::getEffort);
    }
    // process route definitions
    try {
        // the routes are sorted - process stepwise
        if (!oc.getBool("unsorted")) {
            loader.processRoutesStepWise(oc.getInt("begin"), oc.getInt("end"), net, *router);
        }
        // the routes are not sorted: load all and process
        else {
            loader.processAllRoutes(oc.getInt("begin"), oc.getInt("end"), net, *router);
        }
        // end the processing
        net.closeOutput();
        delete router;
    } catch (ProcessError &) {
        net.closeOutput();
        delete router;
        throw;
    }
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char **argv)
{
    OptionsCont &oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Shortest path router and DUE computer for the microscopic road traffic simulation SUMO.");
#ifdef WIN32
    oc.setApplicationName("duarouter.exe", "SUMO duarouter Version " + (string)VERSION_STRING);
#else
    oc.setApplicationName("sumo-duarouter", "SUMO duarouter Version " + (string)VERSION_STRING);
#endif
    int ret = 0;
    RONet *net = 0;
    try {
        XMLSubSys::init(false);
        RODUAFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        MsgHandler::initOutputOptions();
        if (!RODUAFrame::checkOptions()) throw ProcessError();
        RandHelper::initRandGlobal();
        // load data
        ROLoader loader(oc, false);
        net = new RONet();
        initNet(*net, loader, oc);
        // build routes
        try {
            computeRoutes(*net, loader, oc);
        } catch (SAXParseException &e) {
            MsgHandler::getErrorInstance()->inform(toString<int>(e.getLineNumber()));
            ret = 1;
        } catch (SAXException &e) {
            MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(e.getMessage()));
            ret = 1;
        }
    } catch (ProcessError &e) {
        if (string(e.what())!=string("Process Error") && string(e.what())!=string("")) {
            MsgHandler::getErrorInstance()->inform(e.what());
        }
        MsgHandler::getErrorInstance()->inform("Quitting (on error).", false);
        ret = 1;
#ifndef _DEBUG
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("Quitting (on unknown error).", false);
        ret = 1;
#endif
    }
    delete net;
    OutputDevice::closeAll();
    SystemFrame::close();
    if (ret==0) {
        cout << "Success." << endl;
    }
    return ret;
}



/****************************************************************************/

