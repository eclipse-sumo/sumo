/****************************************************************************/
/// @file    duarouter_main.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Thu, 06 Jun 2002
/// @version $Id$
///
// Main for DUAROUTER
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
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
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROEdge.h>
#include <utils/vehicle/DijkstraRouterTT.h>
#include <utils/vehicle/DijkstraRouterEffort.h>
#include <utils/vehicle/AStarRouter.h>
#include <utils/vehicle/CHRouter.h>
#include <utils/vehicle/CHRouterWrapper.h>
#include "RODUAEdgeBuilder.h"
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
#include "RODUAFrame.h"
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
initNet(RONet& net, ROLoader& loader, OptionsCont& oc) {
    // load the net
    RODUAEdgeBuilder builder;
    ROEdge::setGlobalOptions(oc.getBool("weights.interpolate"));
    loader.loadNet(net, builder);
    // load the weights when wished/available
    if (oc.isSet("weight-files")) {
        loader.loadWeights(net, "weight-files", oc.getString("weight-attribute"), false, oc.getBool("weights.expand"));
    }
    if (oc.isSet("lane-weight-files")) {
        loader.loadWeights(net, "lane-weight-files", oc.getString("weight-attribute"), true, oc.getBool("weights.expand"));
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
    const std::string& filename = oc.getString("output-file");
    std::string altFilename = filename + ".alt";
    const int len = (int)filename.length();
    if (len > 4 && filename.substr(len - 4) == ".xml") {
        altFilename = filename.substr(0, len - 4) + ".alt.xml";
    } else if (len > 4 && filename.substr(len - 4) == ".sbx") {
        altFilename = filename.substr(0, len - 4) + ".alt.sbx";
    }
    // build the router
    SUMOAbstractRouter<ROEdge, ROVehicle>* router;
    const std::string measure = oc.getString("weight-attribute");
    const std::string routingAlgorithm = oc.getString("routing-algorithm");
    const SUMOTime begin = string2time(oc.getString("begin"));
    const SUMOTime end = string2time(oc.getString("end"));
    if (measure == "traveltime") {
        if (routingAlgorithm == "dijkstra") {
            if (net.hasPermissions()) {
                router = new DijkstraRouterTT<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
            } else {
                router = new DijkstraRouterTT<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
            }
        } else if (routingAlgorithm == "astar") {
            if (net.hasPermissions()) {
                router = new AStarRouter<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
            } else {
                router = new AStarRouter<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic);
            }
        } else if (routingAlgorithm == "CH") {
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());
            if (net.hasPermissions()) {
                router = new CHRouter<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, SVC_IGNORING, weightPeriod, true);
            } else {
                router = new CHRouter<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                    ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic, SVC_IGNORING, weightPeriod, false);
            }
        } else if (routingAlgorithm == "CHWrapper") {
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());
            router = new CHRouterWrapper<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                ROEdge::getAllEdges(), oc.getBool("ignore-errors"), &ROEdge::getTravelTimeStatic,
                begin, end, weightPeriod, oc.getInt("routing-threads"));
        } else {
            throw ProcessError("Unknown routing Algorithm '" + routingAlgorithm + "'!");
        }
    } else {
        DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >::Operation op;
        if (measure == "CO") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::CO>;
        } else if (measure == "CO2") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::CO2>;
        } else if (measure == "PMx") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::PM_X>;
        } else if (measure == "HC") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::HC>;
        } else if (measure == "NOx") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::NO_X>;
        } else if (measure == "fuel") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::FUEL>;
        } else if (measure == "electricity") {
            op = &ROEdge::getEmissionEffort<PollutantsInterface::ELEC>;
        } else if (measure == "noise") {
            op = &ROEdge::getNoiseEffort;
        } else {
            throw ProcessError("Unknown measure (weight attribute '" + measure + "')!");
        }
        if (net.hasPermissions()) {
            router = new DijkstraRouterEffort<ROEdge, ROVehicle, prohibited_withPermissions<ROEdge, ROVehicle> >(
                ROEdge::getAllEdges(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTimeStatic);
        } else {
            router = new DijkstraRouterEffort<ROEdge, ROVehicle, noProhibitions<ROEdge, ROVehicle> >(
                ROEdge::getAllEdges(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTimeStatic);
        }
    }
    RORouterProvider provider(router, new PedestrianRouterDijkstra<ROEdge, ROLane, RONode, ROVehicle>(),
                              new ROIntermodalRouter(RONet::adaptIntermodalRouter));
    // process route definitions
    try {
        net.openOutput(oc, altFilename);
        loader.processRoutes(begin, end, string2time(oc.getString("route-steps")), net, provider);
        // end the processing
        net.cleanup();
    } catch (ProcessError&) {
        net.cleanup();
        throw;
    }
}


/* -------------------------------------------------------------------------
 * main
 * ----------------------------------------------------------------------- */
int
main(int argc, char** argv) {
    OptionsCont& oc = OptionsCont::getOptions();
    // give some application descriptions
    oc.setApplicationDescription("Shortest path router and DUE computer for the microscopic road traffic simulation SUMO.");
    oc.setApplicationName("duarouter", "SUMO duarouter Version " VERSION_STRING);
    int ret = 0;
    RONet* net = 0;
    try {
        XMLSubSys::init();
        RODUAFrame::fillOptions();
        OptionsIO::setArgs(argc, argv);
        OptionsIO::getOptions();
        if (oc.processMetaOptions(argc < 2)) {
            SystemFrame::close();
            return 0;
        }
        XMLSubSys::setValidation(oc.getString("xml-validation"), oc.getString("xml-validation.net"));
        MsgHandler::initOutputOptions();
        if (!RODUAFrame::checkOptions()) {
            throw ProcessError();
        }
        RandHelper::initRandGlobal();
        // load data
        ROLoader loader(oc, false, !oc.getBool("no-step-log"));
        net = new RONet();
        initNet(*net, loader, oc);
        // build routes
        try {
            computeRoutes(*net, loader, oc);
        } catch (XERCES_CPP_NAMESPACE::SAXParseException& e) {
            WRITE_ERROR(toString(e.getLineNumber()));
            ret = 1;
        } catch (XERCES_CPP_NAMESPACE::SAXException& e) {
            WRITE_ERROR(TplConvert::_2str(e.getMessage()));
            ret = 1;
        }
        if (MsgHandler::getErrorInstance()->wasInformed() || ret != 0) {
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

