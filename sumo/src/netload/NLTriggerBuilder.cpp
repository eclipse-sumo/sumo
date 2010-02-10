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
    std::string type = attrs.getString(SUMO_ATTR_OBJECTTYPE);
    std::string attr = attrs.getStringSecure(SUMO_ATTR_ATTR, "");
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
    SUMOTime begin = attrs.getIntReporting(SUMO_ATTR_BEGIN, "vaporizer", 0, ok);
    SUMOTime end = attrs.getIntReporting(SUMO_ATTR_END, "vaporizer", 0, ok);
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
    if (end>=OptionsCont::getOptions().getInt("begin")) {
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
    if (attrs.hasAttribute(SUMO_ATTR_LANES)) {
        objectid = attrs.getString(SUMO_ATTR_LANES);
    } else {
        if (attrs.hasAttribute(SUMO_ATTR_OBJECTID)) {
            objectid = attrs.getString(SUMO_ATTR_OBJECTID);
            MsgHandler::getWarningInstance()->inform("Defining the lanes using 'objectid' within a variable speed sign is deprecated, use 'lanes' instead.");
        }
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
    SUMOReal frompos, topos;
    try {
        frompos = attrs.getFloat(SUMO_ATTR_FROM);
        topos = attrs.getFloat(SUMO_ATTR_TO);
    } catch (EmptyData&) {
        throw InvalidArgument("Either the begin or the end position of busstop '" + id + "' is not given.");
    } catch (NumberFormatException&) {
        throw InvalidArgument("Either the begin or the end position of busstop '" + id + "' is not numeric.");
    }
    if (frompos<0) {
        frompos = lane->getLength() + frompos;
    }
    if (topos<0) {
        topos = lane->getLength() + topos;
    }
    // check positions
    if (topos<0 || topos>lane->getLength()) {
        if (attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false)) {
            throw InvalidArgument("Bus stop '" + id + "' ends after the lane's end.");
        } else {
            MsgHandler::getWarningInstance()->inform("Bus stop '" + id + "' ends after the lane's end (moving to the end).");
            topos = lane->getLength() - (SUMOReal) .1;
        }
    }
    if (frompos<0 || frompos>lane->getLength()) {
        if (attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false)) {
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
    SUMOSAXAttributes::parseStringVector(attrs.getStringSecure(SUMO_ATTR_LINES, ""), lines);
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
    SUMOTime freq = attrs.GET_XML_SUMO_TIME_SECURE(SUMO_ATTR_FREQUENCY, 1);
#ifdef HAVE_MESOSIM
    if (MSGlobals::gUseMesoSim) {
        std::string file = getFileName(attrs, base, true);
        std::string outfile = attrs.getStringSecure(SUMO_ATTR_OUTPUT, "");
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
    if (attrs.hasAttribute(SUMO_ATTR_EDGES)) {
        objectid = attrs.getString(SUMO_ATTR_EDGES);
    } else {
        if (attrs.hasAttribute(SUMO_ATTR_OBJECTID)) {
            objectid = attrs.getString(SUMO_ATTR_OBJECTID);
            MsgHandler::getWarningInstance()->inform("Defining the edges using 'objectid' within a rerouter is deprecated, use 'edges' instead.");
        }
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

    SUMOReal prob;
    try {
        prob = attrs.getFloatSecure(SUMO_ATTR_PROB, 1);
    } catch (NumberFormatException &) {
        throw InvalidArgument("Invalid probability in definition of MSTriggeredRerouter '" + id + "'.");
    }
    buildRerouter(net, id, edges, prob, file, attrs.getBoolSecure(SUMO_ATTR_OFF, false));
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
    std::string file = attrs.getStringSecure(SUMO_ATTR_FILE, "");
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
    std::string objectid;
    if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
        objectid = attrs.getString(SUMO_ATTR_LANE);
    } else {
        if (attrs.hasAttribute(SUMO_ATTR_OBJECTID)) {
            objectid = attrs.getString(SUMO_ATTR_OBJECTID);
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

    SUMOReal pos = attrs.getFloat(SUMO_ATTR_POSITION);
    bool friendly_pos = attrs.getBoolSecure(SUMO_ATTR_FRIENDLY_POS, false);
    if (pos<0) {
        pos = lane->getLength() + pos;
    }
    if (pos>lane->getLength()) {
        if (friendly_pos) {
            pos = lane->getLength() - (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane->getID() + "' length.");
        }
    }
    return pos;
}



/****************************************************************************/
