/****************************************************************************/
/// @file    NLTriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @author  Jakob Erdmann
/// @author  Eric Nicolay
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @date    Thu, 17 Oct 2002
/// @version $Id$
///
// Builds trigger objects for microsim
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

#include <string>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSRouteProbe.h>
#include <microsim/trigger/MSLaneSpeedTrigger.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <microsim/trigger/MSCalibrator.h>
#include <microsim/trigger/MSBusStop.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/WrappingCommand.h>
#include "NLHandler.h"
#include "NLTriggerBuilder.h"
#include <utils/xml/SUMOXMLDefinitions.h>


#ifdef HAVE_INTERNAL
#include <mesosim/MELoop.h>
#include <mesosim/METriggeredCalibrator.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
NLTriggerBuilder::NLTriggerBuilder()
    : myHandler(0) {}


NLTriggerBuilder::~NLTriggerBuilder() {}

void
NLTriggerBuilder::setHandler(NLHandler* handler) {
    myHandler = handler;
}


void
NLTriggerBuilder::buildVaporizer(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    MSEdge* e = MSEdge::dictionary(id);
    if (e == 0) {
        WRITE_ERROR("Unknown edge ('" + id + "') referenced in a vaporizer.");
        return;
    }
    SUMOTime begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, 0, ok);
    SUMOTime end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, 0, ok);
    if (!ok) {
        return;
    }
    if (begin < 0) {
        WRITE_ERROR("A vaporization begin time is negative (edge id='" + id + "').");
        return;
    }
    if (begin >= end) {
        WRITE_ERROR("A vaporization ends before it starts (edge id='" + id + "').");
        return;
    }
    if (end >= string2time(OptionsCont::getOptions().getString("begin"))) {
        Command* cb = new WrappingCommand< MSEdge >(e, &MSEdge::incVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(cb, begin, MSEventControl::ADAPT_AFTER_EXECUTION);
        Command* ce = new WrappingCommand< MSEdge >(e, &MSEdge::decVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents().addEvent(ce, end, MSEventControl::ADAPT_AFTER_EXECUTION);
    }
}



void
NLTriggerBuilder::parseAndBuildLaneSpeedTrigger(MSNet& net, const SUMOSAXAttributes& attrs,
        const std::string& base) {
    // get the id, throw if not given or empty...
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        return;
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base, true);
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_LANES, id.c_str(), ok);
    if (!ok) {
        throw InvalidArgument("The lanes to use within MSLaneSpeedTrigger '" + id + "' are not known.");
    }
    std::vector<MSLane*> lanes;
    std::vector<std::string> laneIDs;
    SUMOSAXAttributes::parseStringVector(objectid, laneIDs);
    for (std::vector<std::string>::iterator i = laneIDs.begin(); i != laneIDs.end(); ++i) {
        MSLane* lane = MSLane::dictionary(*i);
        if (lane == 0) {
            throw InvalidArgument("The lane to use within MSLaneSpeedTrigger '" + id + "' is not known.");
        }
        lanes.push_back(lane);
    }
    if (lanes.size() == 0) {
        throw InvalidArgument("No lane defined for MSLaneSpeedTrigger '" + id + "'.");
    }
    try {
        MSLaneSpeedTrigger* trigger = buildLaneSpeedTrigger(net, id, lanes, file);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_VSS, myHandler);
        }
    } catch (ProcessError& e) {
        throw InvalidArgument(e.what());
    }
}


void
NLTriggerBuilder::parseAndBuildBusStop(MSNet& net, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the lane
    MSLane* lane = getLane(attrs, "busStop", id);
    // get the positions
    SUMOReal frompos = attrs.getOpt<SUMOReal>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    SUMOReal topos = attrs.getOpt<SUMOReal>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    if (!ok || !myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos)) {
        throw InvalidArgument("Invalid position for bus stop '" + id + "'.");
    }
    // get the lines
    std::vector<std::string> lines;
    SUMOSAXAttributes::parseStringVector(attrs.getOpt<std::string>(SUMO_ATTR_LINES, id.c_str(), ok, "", false), lines);
    // build the bus stop
    buildBusStop(net, id, lines, lane, frompos, topos);
}


void
NLTriggerBuilder::parseAndBuildCalibrator(MSNet& net, const SUMOSAXAttributes& attrs,
        const std::string& base) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the file name to read further definitions from
    MSLane* lane = getLane(attrs, "calibrator", id);
    const SUMOReal pos = getPosition(attrs, lane, "calibrator", id);
    const SUMOTime freq = attrs.getOptSUMOTimeReporting(SUMO_ATTR_FREQUENCY, id.c_str(), ok, DELTA_T); // !!! no error handling
    std::string file = getFileName(attrs, base, true);
    std::string outfile = attrs.getOpt<std::string>(SUMO_ATTR_OUTPUT, id.c_str(), ok, "");
    std::string routeProbe = attrs.getOpt<std::string>(SUMO_ATTR_ROUTEPROBE, id.c_str(), ok, "");
    MSRouteProbe* probe = 0;
    if (routeProbe != "") {
        probe = dynamic_cast<MSRouteProbe*>(net.getDetectorControl().getTypedDetectors(SUMO_TAG_ROUTEPROBE).get(routeProbe));
    }
    if (MSGlobals::gUseMesoSim) {
#ifdef HAVE_INTERNAL
        METriggeredCalibrator* trigger = buildMECalibrator(net, id, &lane->getEdge(), pos, file, outfile, freq, probe);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
#endif
    } else {
        MSCalibrator* trigger = buildCalibrator(net, id, &lane->getEdge(), pos, file, outfile, freq, probe);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
    }
}


void
NLTriggerBuilder::parseAndBuildRerouter(MSNet& net, const SUMOSAXAttributes& attrs,
                                        const std::string& base) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base, true);
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_EDGES, id.c_str(), ok);
    if (!ok) {
        throw InvalidArgument("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
    }
    std::vector<MSEdge*> edges;
    std::vector<std::string> edgeIDs;
    SUMOSAXAttributes::parseStringVector(objectid, edgeIDs);
    for (std::vector<std::string>::iterator i = edgeIDs.begin(); i != edgeIDs.end(); ++i) {
        MSEdge* edge = MSEdge::dictionary(*i);
        if (edge == 0) {
            throw InvalidArgument("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
        }
        edges.push_back(edge);
    }
    if (edges.size() == 0) {
        throw InvalidArgument("No edges found for MSTriggeredRerouter '" + id + "'.");
    }
    SUMOReal prob = attrs.getOpt<SUMOReal>(SUMO_ATTR_PROB, id.c_str(), ok, 1);
    bool off = attrs.getOpt<bool>(SUMO_ATTR_OFF, id.c_str(), ok, false);
    if (!ok) {
        throw InvalidArgument("Could not parse MSTriggeredRerouter '" + id + "'.");
    }
    MSTriggeredRerouter* trigger = buildRerouter(net, id, edges, prob, file, off);
    if (file == "") {
        trigger->registerParent(SUMO_TAG_REROUTER, myHandler);
    }
}


// -------------------------


MSLaneSpeedTrigger*
NLTriggerBuilder::buildLaneSpeedTrigger(MSNet& /*net*/, const std::string& id,
                                        const std::vector<MSLane*>& destLanes,
                                        const std::string& file) {
    return new MSLaneSpeedTrigger(id, destLanes, file);
}


#ifdef HAVE_INTERNAL
METriggeredCalibrator*
NLTriggerBuilder::buildMECalibrator(MSNet& /*net*/, const std::string& id,
                                    const MSEdge* edge, SUMOReal pos,
                                    const std::string& file,
                                    const std::string& outfile,
                                    const SUMOTime freq, MSRouteProbe* probe) {
    return new METriggeredCalibrator(id, edge, pos, file, outfile, freq, MSGlobals::gMesoNet->getSegmentForEdge(*edge, pos)->getLength(), probe);
}
#endif


MSCalibrator*
NLTriggerBuilder::buildCalibrator(MSNet& /*net*/, const std::string& id,
                                  MSEdge* edge, SUMOReal pos,
                                  const std::string& file,
                                  const std::string& outfile,
                                  const SUMOTime freq, MSRouteProbe* probe) {
    return new MSCalibrator(id, edge, pos, file, outfile, freq, edge->getLength(), probe);
}


MSTriggeredRerouter*
NLTriggerBuilder::buildRerouter(MSNet&, const std::string& id,
                                std::vector<MSEdge*>& edges,
                                SUMOReal prob, const std::string& file, bool off) {
    return new MSTriggeredRerouter(id, edges, prob, file, off);
}


void
NLTriggerBuilder::buildBusStop(MSNet& net, const std::string& id,
                               const std::vector<std::string>& lines,
                               MSLane* lane, SUMOReal frompos, SUMOReal topos) {
    MSBusStop* stop = new MSBusStop(id, lines, *lane, frompos, topos);
    if (!net.addBusStop(stop)) {
        delete stop;
        throw InvalidArgument("Could not build bus stop '" + id + "'; probably declared twice.");
    }
}




std::string
NLTriggerBuilder::getFileName(const SUMOSAXAttributes& attrs,
                              const std::string& base,
                              const bool allowEmpty) {
    // get the file name to read further definitions from
    bool ok = true;
    std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, 0, ok, "");
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


MSLane*
NLTriggerBuilder::getLane(const SUMOSAXAttributes& attrs,
                          const std::string& tt,
                          const std::string& tid) {
    bool ok = true;
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_LANE, tid.c_str(), ok);
    MSLane* lane = MSLane::dictionary(objectid);
    if (lane == 0) {
        throw InvalidArgument("The lane " + objectid + " to use within the " + tt + " '" + tid + "' is not known.");
    }
    return lane;
}


SUMOReal
NLTriggerBuilder::getPosition(const SUMOSAXAttributes& attrs,
                              MSLane* lane,
                              const std::string& tt, const std::string& tid) {
    bool ok = true;
    SUMOReal pos = attrs.get<SUMOReal>(SUMO_ATTR_POSITION, 0, ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, 0, ok, false);
    if (!ok) {
        throw InvalidArgument("Error on parsing a position information.");
    }
    if (pos < 0) {
        pos = lane->getLength() + pos;
    }
    if (pos > lane->getLength()) {
        if (friendlyPos) {
            pos = lane->getLength() - (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane->getID() + "' length.");
        }
    }
    return pos;
}



/****************************************************************************/
