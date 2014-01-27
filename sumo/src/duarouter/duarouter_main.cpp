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
#include <router/ROLoader.h>
#include <router/RONet.h>
#include <router/ROEdge.h>
#include <router/ROCostCalculator.h>
#include <utils/common/DijkstraRouterTT.h>
#include <utils/common/DijkstraRouterEffort.h>
#include <utils/common/AStarRouter.h>
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

#ifdef HAVE_INTERNAL // catchall for internal stuff
#include <internal/BulkStarRouter.h>
#include <internal/CHRouter.h>
#include <internal/CHRouterWrapper.h>
#endif // have HAVE_INTERNAL

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
    RODUAEdgeBuilder builder(oc.getBool("weights.expand"), oc.getBool("weights.interpolate"));
    loader.loadNet(net, builder);
    // load the weights when wished/available
    if (oc.isSet("weight-files")) {
        loader.loadWeights(net, "weight-files", oc.getString("weight-attribute"), false);
    }
    if (oc.isSet("lane-weight-files")) {
        loader.loadWeights(net, "lane-weight-files", oc.getString("weight-attribute"), true);
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
    const size_t len = filename.length();
    if (len > 4 && filename.substr(len - 4) == ".xml") {
        altFilename = filename.substr(0, len - 4) + ".alt.xml";
    } else if (len > 4 && filename.substr(len - 4) == ".sbx") {
        altFilename = filename.substr(0, len - 4) + ".alt.sbx";
    }
    // build the router
    SUMOAbstractRouter<ROEdge, ROVehicle>* router;
    const std::string measure = oc.getString("weight-attribute");
    const std::string routingAlgorithm = oc.getString("routing-algorithm");
    if (measure == "traveltime") {
        if (routingAlgorithm == "dijkstra") {
            if (net.hasRestrictions()) {
                router = new DijkstraRouterTT_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime);
            } else {
                router = new DijkstraRouterTT_Direct<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime);
            }
        } else if (routingAlgorithm == "astar") {
            if (net.hasRestrictions()) {
                router = new AStarRouterTT_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime);
            } else {
                router = new AStarRouterTT_Direct<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime);
            }
#ifdef HAVE_INTERNAL // catchall for internal stuff
        } else if (routingAlgorithm == "bulkstar") {
            if (net.hasRestrictions()) {
                router = new BulkStarRouterTT<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime, &ROEdge::getMinimumTravelTime);
            } else {
                router = new BulkStarRouterTT<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime, &ROEdge::getMinimumTravelTime);
            }

        } else if (routingAlgorithm == "CH") {
            // defaultVehicle is only in constructor and may be safely deleted
            // it is mainly needed for its maximum speed. @todo XXX make this configurable
            ROVehicle defaultVehicle(SUMOVehicleParameter(), 0, net.getVehicleTypeSecure(DEFAULT_VTYPE_ID), &net);
            const SUMOTime begin = string2time(oc.getString("begin"));
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());
            if (net.hasRestrictions()) {
                router = new CHRouter<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime, &defaultVehicle, begin, weightPeriod, true);
            } else {
                router = new CHRouter<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                    net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime, &defaultVehicle, begin, weightPeriod, false);
            }

        } else if (routingAlgorithm == "CHWrapper") {
            const SUMOTime begin = string2time(oc.getString("begin"));
            const SUMOTime weightPeriod = (oc.isSet("weight-files") ?
                                           string2time(oc.getString("weight-period")) :
                                           std::numeric_limits<int>::max());

            router = new CHRouterWrapper<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                net.getEdgeNo(), oc.getBool("ignore-errors"), &ROEdge::getTravelTime, begin, weightPeriod);

#endif // have HAVE_INTERNAL
        } else {
            throw ProcessError("Unknown routing Algorithm '" + routingAlgorithm + "'!");
        }

    } else {
        DijkstraRouterEffort_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >::Operation op;
        if (measure == "CO") {
            op = &ROEdge::getCOEffort;
        } else if (measure == "CO2") {
            op = &ROEdge::getCO2Effort;
        } else if (measure == "PMx") {
            op = &ROEdge::getPMxEffort;
        } else if (measure == "HC") {
            op = &ROEdge::getHCEffort;
        } else if (measure == "NOx") {
            op = &ROEdge::getNOxEffort;
        } else if (measure == "fuel") {
            op = &ROEdge::getFuelEffort;
        } else if (measure == "noise") {
            op = &ROEdge::getNoiseEffort;
        } else {
            net.closeOutput();
            throw ProcessError("Unknown measure (weight attribute '" + measure + "')!");
        }
        if (net.hasRestrictions()) {
            router = new DijkstraRouterEffort_Direct<ROEdge, ROVehicle, prohibited_withRestrictions<ROEdge, ROVehicle> >(
                net.getEdgeNo(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTime);
        } else {
            router = new DijkstraRouterEffort_Direct<ROEdge, ROVehicle, prohibited_noRestrictions<ROEdge, ROVehicle> >(
                net.getEdgeNo(), oc.getBool("ignore-errors"), op, &ROEdge::getTravelTime);
        }
    }
    net.openOutput(filename, altFilename, oc.getString("vtype-output"));
    // process route definitions
    try {
        if (routingAlgorithm == "bulkstar") {
#ifdef HAVE_INTERNAL // catchall for internal stuff
            // need to load all routes for spatial aggregation
            loader.processAllRoutesWithBulkRouter(string2time(oc.getString("begin")), string2time(oc.getString("end")), net, *router);
#endif
        } else {
            loader.processRoutes(string2time(oc.getString("begin")), string2time(oc.getString("end")), net, *router);
        }
        // end the processing
        net.closeOutput();
        delete router;
        ROCostCalculator::cleanup();
    } catch (ProcessError&) {
        net.closeOutput();
        delete router;
        ROCostCalculator::cleanup();
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
    oc.setApplicationName("duarouter", "SUMO duarouter Version " + (std::string)VERSION_STRING);
    int ret = 0;
    RONet* net = 0;
    try {
        XMLSubSys::init();
        RODUAFrame::fillOptions();
        OptionsIO::getOptions(true, argc, argv);
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

