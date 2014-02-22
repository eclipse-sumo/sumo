/****************************************************************************/
/// @file    ROLoader.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Christian Roessel
/// @date    Sept 2002
/// @version $Id$
///
// Loader for networks and route imports
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
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

#include <iostream>
#include <string>
#include <iomanip>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/TransService.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <utils/options/OptionsCont.h>
#include <utils/common/ToString.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/xml/XMLSubSys.h>
#include <utils/xml/SAXWeightsHandler.h>
#include <utils/xml/SUMORouteLoader.h>
#include <utils/xml/SUMORouteLoaderControl.h>
#include "RONet.h"
#include "RONetHandler.h"
#include "ROLoader.h"
#include "ROEdge.h"
#include "RORouteHandler.h"

#ifdef HAVE_INTERNAL // catchall for internal stuff
#include <internal/RouteAggregator.h>
#endif // have HAVE_INTERNAL

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// ROLoader::EdgeFloatTimeLineRetriever_EdgeTravelTime - methods
// ---------------------------------------------------------------------------
void
ROLoader::EdgeFloatTimeLineRetriever_EdgeTravelTime::addEdgeWeight(const std::string& id,
        SUMOReal val, SUMOReal beg, SUMOReal end) const {
    ROEdge* e = myNet.getEdge(id);
    if (e != 0) {
        e->addTravelTime(val, beg, end);
    } else {
        if (id[0] != ':') {
            if (OptionsCont::getOptions().getBool("ignore-errors")) {
                WRITE_WARNING("Trying to set a weight for the unknown edge '" + id + "'.");
            } else {
                WRITE_ERROR("Trying to set a weight for the unknown edge '" + id + "'.");
            }
        }
    }
}


// ---------------------------------------------------------------------------
// ROLoader::EdgeFloatTimeLineRetriever_EdgeWeight - methods
// ---------------------------------------------------------------------------
void
ROLoader::EdgeFloatTimeLineRetriever_EdgeWeight::addEdgeWeight(const std::string& id,
        SUMOReal val, SUMOReal beg, SUMOReal end) const {
    ROEdge* e = myNet.getEdge(id);
    if (e != 0) {
        e->addEffort(val, beg, end);
    } else {
        if (id[0] != ':') {
            if (OptionsCont::getOptions().getBool("ignore-errors")) {
                WRITE_WARNING("Trying to set a weight for the unknown edge '" + id + "'.");
            } else {
                WRITE_ERROR("Trying to set a weight for the unknown edge '" + id + "'.");
            }
        }
    }
}


// ---------------------------------------------------------------------------
// ROLoader - methods
// ---------------------------------------------------------------------------
ROLoader::ROLoader(OptionsCont& oc, const bool emptyDestinationsAllowed, const bool logSteps) :
    myOptions(oc),
    myEmptyDestinationsAllowed(emptyDestinationsAllowed),
    myLogSteps(logSteps),
    myLoaders(oc.exists("unsorted-input") && oc.getBool("unsorted-input") ? 0 : DELTA_T)
{}


ROLoader::~ROLoader() {
}


void
ROLoader::loadNet(RONet& toFill, ROAbstractEdgeBuilder& eb) {
    std::string file = myOptions.getString("net-file");
    if (file == "") {
        throw ProcessError("Missing definition of network to load!");
    }
    if (!FileHelpers::exists(file)) {
        throw ProcessError("The network file '" + file + "' could not be found.");
    }
    PROGRESS_BEGIN_MESSAGE("Loading net");
    RONetHandler handler(toFill, eb);
    handler.setFileName(file);
    if (!XMLSubSys::runParser(handler, file, true)) {
        PROGRESS_FAILED_MESSAGE();
        throw ProcessError();
    } else {
        PROGRESS_DONE_MESSAGE();
    }
    if (myOptions.isSet("additional-files", false)) { // dfrouter does not register this option
        std::vector<std::string> files = myOptions.getStringVector("additional-files");
        for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
            if (!FileHelpers::exists(*fileIt)) {
                throw ProcessError("The additional file '" + *fileIt + "' could not be found.");
            }
            PROGRESS_BEGIN_MESSAGE("Loading additional file '" + *fileIt + "' ");
            handler.setFileName(*fileIt);
            if (!XMLSubSys::runParser(handler, *fileIt)) {
                PROGRESS_FAILED_MESSAGE();
                throw ProcessError();
            } else {
                PROGRESS_DONE_MESSAGE();
            }
        }
    }
}


void
ROLoader::openRoutes(RONet& net) {
    // build loader
    // load sumo-routes when wished
    bool ok = openTypedRoutes("route-files", net);
    // load the XML-trip definitions when wished
    ok &= openTypedRoutes("trip-files", net);
    // load the sumo-alternative file when wished
    ok &= openTypedRoutes("alternative-files", net);
    // load the amount definitions if wished
    ok &= openTypedRoutes("flow-files", net);
    // check
    if (ok) {
        myLoaders.loadNext(string2time(myOptions.getString("begin")));
        if (!MsgHandler::getErrorInstance()->wasInformed() && !net.furtherStored()) {
            throw ProcessError("No route input specified or all routes were invalid.");
        }
        // skip routes prior to the begin time
        if (!myOptions.getBool("unsorted-input")) {
            WRITE_MESSAGE("Skipped until: " + time2string(myLoaders.getFirstLoadTime()));
        }
    }
}


void
ROLoader::processRoutes(SUMOTime start, SUMOTime end,
                        RONet& net, SUMOAbstractRouter<ROEdge, ROVehicle>& router) {
    SUMOTime absNo = end - start;
    // skip routes that begin before the simulation's begin
    // loop till the end
    bool endReached = false;
    bool errorOccured = false;
    const SUMOTime firstStep = myLoaders.getFirstLoadTime();
    SUMOTime lastStep = firstStep;
    for (SUMOTime time = firstStep; time < end && !errorOccured && !endReached; time += DELTA_T) {
        writeStats(time, start, absNo);
        myLoaders.loadNext(time);
        net.saveAndRemoveRoutesUntil(myOptions, router, time);
        endReached = !net.furtherStored();
        lastStep = time;
        errorOccured = MsgHandler::getErrorInstance()->wasInformed() && !myOptions.getBool("ignore-errors");
    }
    if (myLogSteps) {
        WRITE_MESSAGE("Routes found between time steps " + time2string(firstStep) + " and " + time2string(lastStep) + ".");
    }
}


#ifdef HAVE_INTERNAL // catchall for internal stuff
void
ROLoader::processAllRoutesWithBulkRouter(SUMOTime start, SUMOTime end,
        RONet& net, SUMOAbstractRouter<ROEdge, ROVehicle>& router) {
    myLoaders.loadNext(SUMOTime_MAX);
    RouteAggregator::processAllRoutes(net, router);
    net.saveAndRemoveRoutesUntil(myOptions, router, end);
}
#endif


bool
ROLoader::openTypedRoutes(const std::string& optionName,
                          RONet& net) {
    // check whether the current loader is known
    //  (not all routers import all route formats)
    if (!myOptions.exists(optionName)) {
        return true;
    }
    // check whether the current loader is wished
    //  and the file(s) can be used
    if (!myOptions.isUsableFileList(optionName)) {
        return !myOptions.isSet(optionName);
    }
    bool ok = true;
    std::vector<std::string> files = myOptions.getStringVector(optionName);
    for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
        // build the instance when everything's all right
        try {
            myLoaders.add(new SUMORouteLoader(new RORouteHandler(net, *fileIt, myOptions.getBool("repair"), myEmptyDestinationsAllowed, myOptions.getBool("ignore-errors"))));
        } catch (ProcessError& e) {
            std::string msg = "The loader for " + optionName + " from file '" + *fileIt + "' could not be initialised;";
            std::string reason = e.what();
            if (reason != "Process Error" && reason != "") {
                msg = msg + "\n Reason: " + reason + ".";
            } else {
                msg = msg + "\n (unknown reason).";
            }
            WRITE_ERROR(msg);
            ok = false;
        }
    }
    return ok;
}


bool
ROLoader::loadWeights(RONet& net, const std::string& optionName,
                      const std::string& measure, bool useLanes) {
    // check whether the file exists
    if (!myOptions.isUsableFileList(optionName)) {
        return false;
    }
    // build and prepare the weights handler
    std::vector<SAXWeightsHandler::ToRetrieveDefinition*> retrieverDefs;
    //  travel time, first (always used)
    EdgeFloatTimeLineRetriever_EdgeTravelTime ttRetriever(net);
    retrieverDefs.push_back(new SAXWeightsHandler::ToRetrieveDefinition("traveltime", !useLanes, ttRetriever));
    //  the measure to use, then
    EdgeFloatTimeLineRetriever_EdgeWeight eRetriever(net);
    if (measure != "traveltime") {
        std::string umeasure = measure;
        if (measure == "CO" || measure == "CO2" || measure == "HC" || measure == "PMx" || measure == "NOx" || measure == "fuel") {
            umeasure = measure + "_perVeh";
        }
        retrieverDefs.push_back(new SAXWeightsHandler::ToRetrieveDefinition(umeasure, !useLanes, eRetriever));
    }
    //  set up handler
    SAXWeightsHandler handler(retrieverDefs, "");
    // go through files
    std::vector<std::string> files = myOptions.getStringVector(optionName);
    for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
        PROGRESS_BEGIN_MESSAGE("Loading precomputed net weights from '" + *fileIt + "'");
        if (XMLSubSys::runParser(handler, *fileIt)) {
            PROGRESS_DONE_MESSAGE();
        } else {
            WRITE_MESSAGE("failed.");
            return false;
        }
    }
    // build edge-internal time lines
    const std::map<std::string, ROEdge*>& edges = net.getEdgeMap();
    for (std::map<std::string, ROEdge*>::const_iterator i = edges.begin(); i != edges.end(); ++i) {
        (*i).second->buildTimeLines(measure);
    }
    return true;
}


void
ROLoader::writeStats(SUMOTime time, SUMOTime start, int absNo) {
    if (myLogSteps) {
        const SUMOReal perc = (SUMOReal)(time - start) / (SUMOReal) absNo;
        std::cout << "Reading time step: " + time2string(time) + "  (" + time2string(time - start) + "/" + time2string(absNo) + " = " + toString(perc * 100) + "% done)       \r";
    }
}


/****************************************************************************/

