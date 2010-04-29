/****************************************************************************/
/// @file    NLTriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Oct 2002
/// @version $Id$
///
// Builds trigger objects for microsim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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

#include <string>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSGlobals.h>
#include <microsim/trigger/MSLaneSpeedTrigger.h>
#include <microsim/trigger/MSEmitter.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <microsim/trigger/MSBusStop.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/WrappingCommand.h>
#include "NLHandler.h"
#include "NLTriggerBuilder.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <microsim/trigger/MSCalibrator.h>


#ifdef HAVE_MESOSIM
#include <mesosim/METriggeredCalibrator.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NLTriggerBuilder::NLTriggerBuilder() throw()
        : myHaveInformedAboutDeprecatedTriggerDefinition(false),
        myHaveInformedAboutDeprecatedEmitter(false), myHandler(0) {}


NLTriggerBuilder::~NLTriggerBuilder() throw() {}

void
NLTriggerBuilder::setHandler(NLHandler *handler) throw() {
    myHandler = handler;
}


void
NLTriggerBuilder::buildTrigger(MSNet &net,
                               const SUMOSAXAttributes &attrs,
                               const std::string &base) throw(InvalidArgument) {
    bool ok = true;
    std::string type = attrs.getStringReporting(SUMO_ATTR_OBJECTTYPE, 0, 0, ok);
    std::string attr = attrs.getOptStringReporting(SUMO_ATTR_ATTR, 0, 0, ok, "");
    // check which type of a trigger shall be build
    if (type=="lane"&&attr=="speed") {
        parseAndBuildLaneSpeedTrigger(net, attrs, base);
    } else if (type=="emitter") {
        parseAndBuildLaneEmitTrigger(net, attrs, base);
    } else if (type=="rerouter") {
        parseAndBuildRerouter(net, attrs, base);
    } else if (type=="bus_stop") {
        parseAndBuildBusStop(net, attrs);
    } else if (type=="calibrator") {
        parseAndBuildCalibrator(net, attrs, base);
    }
    if (!myHaveInformedAboutDeprecatedTriggerDefinition) {
        MsgHandler::getWarningInstance()->inform("Defining '" + type + "' using a trigger definition is deprecated.");
        myHaveInformedAboutDeprecatedTriggerDefinition = true;
    }
}



void
NLTriggerBuilder::buildVaporizer(const SUMOSAXAttributes &attrs) throw() {
    // get the id, throw if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("vaporizer", id, false)) {
        MsgHandler::getErrorInstance()->inform("Missing or empty id in a vaporizer-object.");
        return;
    }
    MSEdge *e = MSEdge::dictionary(id);
    if (e==0) {
        MsgHandler::getErrorInstance()->inform("Unknown edge ('" + id + "') referenced in a vaporizer.");
        return;
    }
    bool ok = true;
    SUMOTime begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, "vaporizer", 0, ok);
    SUMOTime end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, "vaporizer", 0, ok);
    if (!ok) {
        return;
    }
    if (begin<0) {
        MsgHandler::getErrorInstance()->inform("A vaporization begin time is negative (edge id='" + id + "').");
        return;
    }
    if (begin>=end) {
        MsgHandler::getErrorInstance()->inform("A vaporization ends before it starts (edge id='" + id + "').");
        return;
    }
    if (end>=string2time(OptionsCont::getOptions().getString("begin"))) {
        Command* cb = new WrappingCommand< MSEdge >(e, &MSEdge::incVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(cb, begin, MSEventControl::ADAPT_AFTER_EXECUTION);
        Command* ce = new WrappingCommand< MSEdge >(e, &MSEdge::decVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(ce, end, MSEventControl::ADAPT_AFTER_EXECUTION);
    }
}



void
NLTriggerBuilder::parseAndBuildLaneSpeedTrigger(MSNet &net, const SUMOSAXAttributes &attrs,
        const std::string &base) throw(InvalidArgument) {
    // get the id, throw if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("lane speed trigger", id, false)) {
        throw InvalidArgument("A lane speed trigger definition does not contain an id");
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base, true);
    std::string objectid;
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_LANES)) {
        objectid = attrs.getStringReporting(SUMO_ATTR_LANES, "vss", id.c_str(), ok);
    } else {
        if (attrs.hasAttribute(SUMO_ATTR_OBJECTID)) {
            objectid = attrs.getStringReporting(SUMO_ATTR_OBJECTID, "vss", id.c_str(), ok);
            MsgHandler::getWarningInstance()->inform("Defining the lanes using 'objectid' within a variable speed sign is deprecated, use 'lanes' instead.");
        }
    }
    if (!ok) {
        throw InvalidArgument("The lanes to use within MSLaneSpeedTrigger '" + id + "' are not known.");
    }
    std::vector<MSLane*> lanes;
    std::vector<std::string> laneIDs;
    SUMOSAXAttributes::parseStringVector(objectid, laneIDs);
    for (std::vector<std::string>::iterator i=laneIDs.begin(); i!=laneIDs.end(); ++i) {
        MSLane *lane = MSLane::dictionary(*i);
        if (lane==0) {
            throw InvalidArgument("The lane to use within MSLaneSpeedTrigger '" + id + "' is not known.");
        }
        lanes.push_back(lane);
    }
    if (lanes.size()==0) {
        throw InvalidArgument("No lane defined for MSLaneSpeedTrigger '" + id + "'.");
    }
    try {
        MSLaneSpeedTrigger* trigger = buildLaneSpeedTrigger(net, id, lanes, file);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_VSS, myHandler);
        }
    } catch (ProcessError &e) {
        throw InvalidArgument(e.what());
    }
}


void
NLTriggerBuilder::parseAndBuildLaneEmitTrigger(MSNet &net, const SUMOSAXAttributes &attrs,
        const std::string &base) throw(InvalidArgument) {
    if (!myHaveInformedAboutDeprecatedEmitter) {
        myHaveInformedAboutDeprecatedEmitter = true;
        MsgHandler::getWarningInstance()->inform("Emitter are deprecated; use departpos/departspeed within routes instead.");
    }
    // get the id, throw if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("emitter", id, false)) {
        throw InvalidArgument("An emitter does not contain an id");
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base);
    MSLane *lane = getLane(attrs, "emitter", id);
    SUMOReal pos = getPosition(attrs, lane, "emitter", id);
    buildLaneEmitTrigger(net, id, lane, pos, file);
}


void
NLTriggerBuilder::parseAndBuildBusStop(MSNet &net, const SUMOSAXAttributes &attrs) throw(InvalidArgument) {
    // get the id, throw if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("bus stop", id, false)) {
        throw InvalidArgument("A bus stop does not contain an id");
    }
    // get the lane
    MSLane *lane = getLane(attrs, "bus_stop", id);
    // get the positions
    bool ok = true;
    SUMOReal frompos = attrs.getSUMORealReporting(SUMO_ATTR_FROM, "busstop", id.c_str(), ok);
    SUMOReal topos = attrs.getSUMORealReporting(SUMO_ATTR_TO, "busstop", id.c_str(), ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, "busstop", id.c_str(), ok, false);
    if (!ok) {
        throw InvalidArgument("Error on parsing a bus stop.");
    }
    if (frompos<0) {
        frompos = lane->getLength() + frompos;
    }
    if (topos<0) {
        topos = lane->getLength() + topos;
    }
    // check positions
    if (topos<0 || topos>lane->getLength()) {
        if (!friendlyPos) {
            throw InvalidArgument("Bus stop '" + id + "' ends after the lane's end.");
        } else {
            MsgHandler::getWarningInstance()->inform("Bus stop '" + id + "' ends after the lane's end (moving to the end).");
            topos = lane->getLength() - (SUMOReal) .1;
        }
    }
    if (frompos<0 || frompos>lane->getLength()) {
        if (!friendlyPos) {
            throw InvalidArgument("Bus stop '" + id + "' begins after the lane's end.");
        } else {
            MsgHandler::getWarningInstance()->inform("Bus stop '" + id + "' begins after the lane's end (moving to the begin-10m).");
            frompos = MAX2(SUMOReal(0), SUMOReal(topos-10.));
        }
    }
    if (topos<frompos) {
        throw InvalidArgument("Bus stop's '" + id + "' end is in front of its begin.");
    }
    // get the lines
    std::vector<std::string> lines;
    SUMOSAXAttributes::parseStringVector(attrs.getOptStringReporting(SUMO_ATTR_LINES, "busstop", id.c_str(), ok, ""), lines);
    // build the bus stop
    buildBusStop(net, id, lines, lane, frompos, topos);
}


void
NLTriggerBuilder::parseAndBuildCalibrator(MSNet &net, const SUMOSAXAttributes &attrs,
        const std::string &base) throw(InvalidArgument) {
    // get the id, throw if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("calibrator", id, false)) {
        throw InvalidArgument("A calibrator does not contain an id");
    }
    // get the file name to read further definitions from
    MSLane *lane = getLane(attrs, "calibrator", id);
    SUMOReal pos = getPosition(attrs, lane, "calibrator", id);
    bool ok = true;
    SUMOTime freq = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, "meandata_lane", id.c_str(), ok, DELTA_T); // !!! no error handling
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        std::string file = getFileName(attrs, base, true);
        bool ok = true;
        std::string outfile = attrs.getOptStringReporting(SUMO_ATTR_OUTPUT, 0, 0, ok, "");
        METriggeredCalibrator* trigger = buildCalibrator(net, id, &lane->getEdge(), pos, file, outfile, freq);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
    } else {
#endif
        std::string file = getFileName(attrs, base);
        buildLaneCalibrator(net, id, lane, pos, file);
#ifdef HAVE_MESOSIM
    }
#endif
}


void
NLTriggerBuilder::parseAndBuildRerouter(MSNet &net, const SUMOSAXAttributes &attrs,
                                        const std::string &base) throw(InvalidArgument) {
    // get the id, throw if not given or empty...
    std::string id;
    if (!attrs.setIDFromAttributes("rerouter", id, false)) {
        throw InvalidArgument("A rerouter does not contain an id");
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base);
    std::string objectid;
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        objectid = attrs.getStringReporting(SUMO_ATTR_EDGES, "rerouter", id.c_str(), ok);
    } else {
        if (attrs.hasAttribute(SUMO_ATTR_OBJECTID)) {
            objectid = attrs.getStringReporting(SUMO_ATTR_OBJECTID, "rerouter", id.c_str(), ok);
            MsgHandler::getWarningInstance()->inform("Defining the edges using 'objectid' within a rerouter is deprecated, use 'edges' instead.");
        }
    }
    if (!ok) {
        throw InvalidArgument("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
    }
    std::vector<MSEdge*> edges;
    std::vector<std::string> edgeIDs;
    SUMOSAXAttributes::parseStringVector(objectid, edgeIDs);
    for (std::vector<std::string>::iterator i=edgeIDs.begin(); i!=edgeIDs.end(); ++i) {
        MSEdge *edge = MSEdge::dictionary(*i);
        if (edge==0) {
            throw InvalidArgument("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
        }
        edges.push_back(edge);
    }
    if (edges.size()==0) {
        throw InvalidArgument("No edges found for MSTriggeredRerouter '" + id + "'.");
    }
    SUMOReal prob = attrs.getOptSUMORealReporting(SUMO_ATTR_PROB, "rerouter", id.c_str(), ok, 1);
    bool off = attrs.getOptBoolReporting(SUMO_ATTR_OFF, "rerouter", id.c_str(), ok, false);
    if (!ok) {
        throw InvalidArgument("Could not parse MSTriggeredRerouter '" + id + "'.");
    }
    buildRerouter(net, id, edges, prob, file, off);
}


// -------------------------


MSLaneSpeedTrigger*
NLTriggerBuilder::buildLaneSpeedTrigger(MSNet &net, const std::string &id,
                                        const std::vector<MSLane*> &destLanes,
                                        const std::string &file) throw(ProcessError) {
    return new MSLaneSpeedTrigger(id, destLanes, file);
}


void
NLTriggerBuilder::buildLaneEmitTrigger(MSNet &net, const std::string &id,
                                       MSLane *destLane, SUMOReal pos,
                                       const std::string &file) throw() {
    new MSEmitter(id, net, destLane, pos, file);
}


void
NLTriggerBuilder::buildLaneCalibrator(MSNet &net, const std::string &id,
                                      MSLane *destLane, SUMOReal pos,
                                      const std::string &file) throw() {
    new MSCalibrator(id, net, destLane, pos, file);
}


#ifdef HAVE_MESOSIM
METriggeredCalibrator*
NLTriggerBuilder::buildCalibrator(MSNet &net, const std::string &id,
                                  const MSEdge *edge, SUMOReal pos,
                                  const std::string &file,
                                  const std::string &outfile,
                                  const SUMOTime freq) throw() {
    return new METriggeredCalibrator(id, edge, pos, file, outfile, freq);
}
#endif


void
NLTriggerBuilder::buildRerouter(MSNet &, const std::string &id,
                                std::vector<MSEdge*> &edges,
                                SUMOReal prob, const std::string &file, bool off) throw() {
    new MSTriggeredRerouter(id, edges, prob, file, off);
}


void
NLTriggerBuilder::buildBusStop(MSNet &net, const std::string &id,
                               const std::vector<std::string> &lines,
                               MSLane *lane, SUMOReal frompos, SUMOReal topos) throw() {
    net.addBusStop(new MSBusStop(id, lines, *lane, frompos, topos));
}




std::string
NLTriggerBuilder::getFileName(const SUMOSAXAttributes &attrs,
                              const std::string &base,
                              const bool allowEmpty) throw(InvalidArgument) {
    // get the file name to read further definitions from
    bool ok = true;
    std::string file = attrs.getOptStringReporting(SUMO_ATTR_FILE, 0, 0, ok, "");
    if (file == "") {
        if (allowEmpty) {
            return file;
        }
        throw InvalidArgument("No filename given.");
    }
    // check whether absolute or relative filenames are given
    if (!FileHelpers::isAbsolute(file)) {
        return FileHelpers::getConfigurationRelative(base, file);
    }
    return file;
}


MSLane *
NLTriggerBuilder::getLane(const SUMOSAXAttributes &attrs,
                          const std::string &tt,
                          const std::string &tid) throw(InvalidArgument) {
    bool ok = true;
    std::string objectid;
    if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
        objectid = attrs.getStringReporting(SUMO_ATTR_LANE, tt.c_str(), tid.c_str(), ok);
    } else {
        if (attrs.hasAttribute(SUMO_ATTR_OBJECTID)) {
            objectid = attrs.getStringReporting(SUMO_ATTR_OBJECTID, tt.c_str(), tid.c_str(), ok);
            MsgHandler::getWarningInstance()->inform("Defining the lane using 'objectid' within " + tt + " is deprecated, use 'lane' instead.");
        }
    }
    MSLane *lane = MSLane::dictionary(objectid);
    if (lane==0) {
        throw InvalidArgument("The lane " + objectid + " to use within the " + tt + " '" + tid + "' is not known.");
    }
    return lane;
}


SUMOReal
NLTriggerBuilder::getPosition(const SUMOSAXAttributes &attrs,
                              MSLane *lane,
                              const std::string &tt, const std::string &tid) throw(InvalidArgument) {
    bool ok = true;
    SUMOReal pos = attrs.getSUMORealReporting(SUMO_ATTR_POSITION, 0, 0, ok);
    bool friendlyPos = attrs.getOptBoolReporting(SUMO_ATTR_FRIENDLY_POS, 0, 0, ok, false);
    if (!ok) {
        throw InvalidArgument("Error on parsing a position information.");
    }
    if (pos<0) {
        pos = lane->getLength() + pos;
    }
    if (pos>lane->getLength()) {
        if (friendlyPos) {
            pos = lane->getLength() - (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane->getID() + "' length.");
        }
    }
    return pos;
}



/****************************************************************************/
