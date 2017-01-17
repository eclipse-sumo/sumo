/****************************************************************************/
/// @file    NLBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// The main interface for loading a microsim
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

#include "NLBuilder.h"
#include <microsim/MSNet.h>
#include <microsim/MSEdgeControl.h>
#include <microsim/MSGlobals.h>
#include <iostream>
#include <vector>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/SAX2XMLReader.hpp>
#include <string>
#include <map>
#include "NLHandler.h"
#include "NLEdgeControlBuilder.h"
#include "NLJunctionControlBuilder.h"
#include "NLDetectorBuilder.h"
#include "NLTriggerBuilder.h"
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleTransfer.h>
#include <utils/xml/SUMORouteLoaderControl.h>
#include <utils/xml/SUMORouteLoader.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/options/Option.h>
#include <utils/options/OptionsCont.h>
#include <utils/common/TplConvert.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/SysUtils.h>
#include <utils/common/ToString.h>
#include <utils/xml/XMLSubSys.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/MSFrame.h>
#include <microsim/MSEdgeWeightsStorage.h>
#include <microsim/MSStateHandler.h>
#include <utils/iodevices/BinaryInputDevice.h>
#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
// ---------------------------------------------------------------------------
// NLBuilder::EdgeFloatTimeLineRetriever_EdgeWeight - methods
// ---------------------------------------------------------------------------
void
NLBuilder::EdgeFloatTimeLineRetriever_EdgeEffort::addEdgeWeight(const std::string& id,
        SUMOReal value, SUMOReal begTime, SUMOReal endTime) const {
    MSEdge* edge = MSEdge::dictionary(id);
    if (edge != 0) {
        myNet.getWeightsStorage().addEffort(edge, begTime, endTime, value);
    } else {
        WRITE_ERROR("Trying to set the effort for the unknown edge '" + id + "'.");
    }
}


// ---------------------------------------------------------------------------
// NLBuilder::EdgeFloatTimeLineRetriever_EdgeTravelTime - methods
// ---------------------------------------------------------------------------
void
NLBuilder::EdgeFloatTimeLineRetriever_EdgeTravelTime::addEdgeWeight(const std::string& id,
        SUMOReal value, SUMOReal begTime, SUMOReal endTime) const {
    MSEdge* edge = MSEdge::dictionary(id);
    if (edge != 0) {
        myNet.getWeightsStorage().addTravelTime(edge, begTime, endTime, value);
    } else {
        WRITE_ERROR("Trying to set the travel time for the unknown edge '" + id + "'.");
    }
}


// ---------------------------------------------------------------------------
// NLBuilder - methods
// ---------------------------------------------------------------------------
NLBuilder::NLBuilder(OptionsCont& oc,
                     MSNet& net,
                     NLEdgeControlBuilder& eb,
                     NLJunctionControlBuilder& jb,
                     NLDetectorBuilder& db,
                     NLHandler& xmlHandler)
    : myOptions(oc), myEdgeBuilder(eb), myJunctionBuilder(jb),
      myDetectorBuilder(db),
      myNet(net), myXMLHandler(xmlHandler) {}


NLBuilder::~NLBuilder() {}


bool
NLBuilder::build() {
    // try to build the net
    if (!load("net-file", true)) {
        return false;
    }
    // check whether the loaded net agrees with the simulation options
#ifdef HAVE_INTERNAL_LANES
    if (myOptions.getBool("no-internal-links") && myXMLHandler.haveSeenInternalEdge()) {
        WRITE_WARNING("Network contains internal links but option --no-internal-links is set. Vehicles will 'jump' across junctions and thus underestimate route lengths and travel times.");
    }
#endif
    if (myOptions.getString("lanechange.duration") != "0" && myXMLHandler.haveSeenNeighs()) {
        throw ProcessError("Network contains explicit neigh lanes which do not work together with option --lanechange.duration.");
    }
    buildNet();
    // load the previous state if wished
    if (myOptions.isSet("load-state")) {
        long before = SysUtils::getCurrentMillis();
        const std::string& f = myOptions.getString("load-state");
        PROGRESS_BEGIN_MESSAGE("Loading state from '" + f + "'");
        MSStateHandler h(f, string2time(OptionsCont::getOptions().getString("load-state.offset")));
        XMLSubSys::runParser(h, f);
        if (myOptions.isDefault("begin")) {
            myOptions.set("begin", time2string(h.getTime()));
        }
        if (MsgHandler::getErrorInstance()->wasInformed()) {
            return false;
        }
        if (h.getTime() != string2time(myOptions.getString("begin"))) {
            WRITE_WARNING("State was written at a different time " + time2string(h.getTime()) + " than the begin time " + myOptions.getString("begin") + "!");
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    // load weights if wished
    if (myOptions.isSet("weight-files")) {
        if (!myOptions.isUsableFileList("weight-files")) {
            return false;
        }
        // build and prepare the weights handler
        std::vector<SAXWeightsHandler::ToRetrieveDefinition*> retrieverDefs;
        //  travel time, first (always used)
        EdgeFloatTimeLineRetriever_EdgeTravelTime ttRetriever(myNet);
        retrieverDefs.push_back(new SAXWeightsHandler::ToRetrieveDefinition("traveltime", true, ttRetriever));
        //  the measure to use, then
        EdgeFloatTimeLineRetriever_EdgeEffort eRetriever(myNet);
        std::string measure = myOptions.getString("weight-attribute");
        if (!myOptions.isDefault("weight-attribute")) {
            if (measure == "CO" || measure == "CO2" || measure == "HC" || measure == "PMx" || measure == "NOx" || measure == "fuel" || measure == "electricity") {
                measure += "_perVeh";
            }
            retrieverDefs.push_back(new SAXWeightsHandler::ToRetrieveDefinition(measure, true, eRetriever));
        }
        //  set up handler
        SAXWeightsHandler handler(retrieverDefs, "");
        // start parsing; for each file in the list
        std::vector<std::string> files = myOptions.getStringVector("weight-files");
        for (std::vector<std::string>::iterator i = files.begin(); i != files.end(); ++i) {
            // report about loading when wished
            WRITE_MESSAGE("Loading weights from '" + *i + "'...");
            // parse the file
            if (!XMLSubSys::runParser(handler, *i)) {
                return false;
            }
        }
    }
    // load routes
    if (myOptions.isSet("route-files") && string2time(myOptions.getString("route-steps")) <= 0) {
        if (!load("route-files")) {
            return false;
        }
    }
    // load additional net elements (sources, detectors, ...)
    if (myOptions.isSet("additional-files")) {
        if (!load("additional-files")) {
            return false;
        }
        // load shapes with separate handler
        NLShapeHandler sh("", myNet.getShapeContainer());
        if (!ShapeHandler::loadFiles(myOptions.getStringVector("additional-files"), sh)) {
            return false;
        }
    }
    // optionally switch off traffic lights
    if (myOptions.getBool("tls.all-off")) {
        myNet.getTLSControl().switchOffAll();
    }
    WRITE_MESSAGE("Loading done.");
    return true;
}


void
NLBuilder::buildNet() {
    MSEdgeControl* edges = 0;
    MSJunctionControl* junctions = 0;
    SUMORouteLoaderControl* routeLoaders = 0;
    MSTLLogicControl* tlc = 0;
    try {
        edges = myEdgeBuilder.build();
        junctions = myJunctionBuilder.build();
        routeLoaders = buildRouteLoaderControl(myOptions);
        tlc = myJunctionBuilder.buildTLLogics();
        MSFrame::buildStreams();
        std::vector<SUMOTime> stateDumpTimes;
        std::vector<std::string> stateDumpFiles;
        const std::vector<int> times = myOptions.getIntVector("save-state.times");
        for (std::vector<int>::const_iterator i = times.begin(); i != times.end(); ++i) {
            stateDumpTimes.push_back(TIME2STEPS(*i));
        }
        if (myOptions.isSet("save-state.files")) {
            stateDumpFiles = myOptions.getStringVector("save-state.files");
            if (stateDumpFiles.size() != stateDumpTimes.size()) {
                WRITE_ERROR("Wrong number of state file names!");
            }
        } else {
            const std::string prefix = myOptions.getString("save-state.prefix");
            const std::string suffix = myOptions.getString("save-state.suffix");
            for (std::vector<SUMOTime>::iterator i = stateDumpTimes.begin(); i != stateDumpTimes.end(); ++i) {
                stateDumpFiles.push_back(prefix + "_" + time2string(*i) + suffix);
            }
        }
        myNet.closeBuilding(myOptions, edges, junctions, routeLoaders, tlc, stateDumpTimes, stateDumpFiles,
                            myXMLHandler.haveSeenInternalEdge(),
                            myXMLHandler.haveSeenNeighs(),
                            myXMLHandler.lefthand(),
                            myXMLHandler.networkVersion());
    } catch (IOError& e) {
        delete edges;
        delete junctions;
        delete routeLoaders;
        delete tlc;
        throw ProcessError(e.what());
    } catch (ProcessError&) {
        delete edges;
        delete junctions;
        delete routeLoaders;
        delete tlc;
        throw;
    }
}


bool
NLBuilder::load(const std::string& mmlWhat, const bool isNet) {
    if (!OptionsCont::getOptions().isUsableFileList(mmlWhat)) {
        return false;
    }
    std::vector<std::string> files = OptionsCont::getOptions().getStringVector(mmlWhat);
    for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
        PROGRESS_BEGIN_MESSAGE("Loading " + mmlWhat + " from '" + *fileIt + "'");
        long before = SysUtils::getCurrentMillis();
        if (!XMLSubSys::runParser(myXMLHandler, *fileIt, isNet)) {
            WRITE_MESSAGE("Loading of " + mmlWhat + " failed.");
            return false;
        }
        PROGRESS_TIME_MESSAGE(before);
    }
    return true;
}


SUMORouteLoaderControl*
NLBuilder::buildRouteLoaderControl(const OptionsCont& oc) {
    // build the loaders
    SUMORouteLoaderControl* loaders =  new SUMORouteLoaderControl(string2time(oc.getString("route-steps")));
    // check whether a list is existing
    if (oc.isSet("route-files") && string2time(oc.getString("route-steps")) > 0) {
        std::vector<std::string> files = oc.getStringVector("route-files");
        for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
            if (!FileHelpers::isReadable(*fileIt)) {
                throw ProcessError("The route file '" + *fileIt + "' is not accessible.");
            }
        }
        // open files for reading
        for (std::vector<std::string>::const_iterator fileIt = files.begin(); fileIt != files.end(); ++fileIt) {
            loaders->add(new SUMORouteLoader(new MSRouteHandler(*fileIt, false)));
        }
    }
    return loaders;
}


/****************************************************************************/
