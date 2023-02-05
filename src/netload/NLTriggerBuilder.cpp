/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    NLTriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @author  Tino Morenz
/// @author  Jakob Erdmann
/// @author  Eric Nicolay
/// @author  Sascha Krieg
/// @author  Michael Behrisch
/// @author  Johannes Rummel
/// @date    Thu, 17 Oct 2002
///
// Builds trigger objects for microsim
/****************************************************************************/
#include <config.h>

#include <string>
#include <microsim/MSEventControl.h>
#include <microsim/MSLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSGlobals.h>
#include <microsim/MSParkingArea.h>
#include <microsim/output/MSDetectorControl.h>
#include <microsim/output/MSRouteProbe.h>
#include <microsim/trigger/MSLaneSpeedTrigger.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <microsim/trigger/MSCalibrator.h>
#include <microsim/MSStoppingPlace.h>
#include <microsim/trigger/MSChargingStation.h>
#include <microsim/trigger/MSOverheadWire.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/WrappingCommand.h>
#include <utils/common/RGBColor.h>
#include <utils/options/OptionsCont.h>
#include "NLHandler.h"
#include "NLTriggerBuilder.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/xml/XMLSubSys.h>


#include <mesosim/MELoop.h>
#include <mesosim/METriggeredCalibrator.h>


// ===========================================================================
// method definitions
// ===========================================================================
NLTriggerBuilder::NLTriggerBuilder()
    : myHandler(nullptr), myParkingArea(nullptr), myCurrentStop(nullptr) {}


NLTriggerBuilder::~NLTriggerBuilder() {}

void
NLTriggerBuilder::setHandler(NLHandler* handler) {
    myHandler = handler;
}


void
NLTriggerBuilder::buildVaporizer(const SUMOSAXAttributes& attrs) {
    WRITE_WARNING(TL("Vaporizers are deprecated. Use rerouters instead."));
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    MSEdge* e = MSEdge::dictionary(id);
    if (e == nullptr) {
        WRITE_ERRORF(TL("Unknown edge ('%') referenced in a vaporizer."), id);
        return;
    }
    SUMOTime begin = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, nullptr, ok);
    SUMOTime end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, nullptr, ok);
    if (!ok) {
        return;
    }
    if (begin < 0) {
        WRITE_ERRORF(TL("A vaporization begin time is negative (edge id='%')."), id);
        return;
    }
    if (begin >= end) {
        WRITE_ERRORF(TL("A vaporization ends before it starts (edge id='%')."), id);
        return;
    }
    if (end >= string2time(OptionsCont::getOptions().getString("begin"))) {
        Command* cb = new WrappingCommand< MSEdge >(e, &MSEdge::incVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(cb, begin);
        Command* ce = new WrappingCommand< MSEdge >(e, &MSEdge::decVaporization);
        MSNet::getInstance()->getBeginOfTimestepEvents()->addEvent(ce, end);
    }
}


void
NLTriggerBuilder::parseAndBuildLaneSpeedTrigger(MSNet& net, const SUMOSAXAttributes& attrs,
        const std::string& base) {
    // get the id, throw if not given or empty...
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        return;
    }
    // get the file name to read further definitions from
    std::string file = getFileName(attrs, base, true);
    std::string objectid = attrs.get<std::string>(SUMO_ATTR_LANES, id.c_str(), ok);
    std::vector<MSLane*> lanes;
    for (const std::string& laneID : attrs.get<std::vector<std::string> >(SUMO_ATTR_LANES, id.c_str(), ok)) {
        MSLane* lane = MSLane::dictionary(laneID);
        if (lane == nullptr) {
            throw InvalidArgument("The lane '" + laneID + "' to use within MSLaneSpeedTrigger '" + id + "' is not known.");
        }
        lanes.push_back(lane);
    }
    if (!ok) {
        throw InvalidArgument("The lanes to use within MSLaneSpeedTrigger '" + id + "' are not known.");
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
NLTriggerBuilder::parseAndBuildChargingStation(MSNet& net, const SUMOSAXAttributes& attrs) {
    bool ok = true;

    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        throw ProcessError();
    }

    MSLane* const lane = getLane(attrs, "chargingStation", id);
    double frompos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    double topos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
    const double chargingPower = attrs.getOpt<double>(SUMO_ATTR_CHARGINGPOWER, id.c_str(), ok, 0);
    const double efficiency = attrs.getOpt<double>(SUMO_ATTR_EFFICIENCY, id.c_str(), ok, 0.95);
    const bool chargeInTransit = attrs.getOpt<bool>(SUMO_ATTR_CHARGEINTRANSIT, id.c_str(), ok, 0);
    const SUMOTime chargeDelay = attrs.getOptSUMOTimeReporting(SUMO_ATTR_CHARGEDELAY, id.c_str(), ok, 0);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, "");

    if (!ok || (myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos) != SUMORouteHandler::StopPos::STOPPOS_VALID)) {
        throw InvalidArgument("Invalid position for charging station '" + id + "'.");
    }

    buildChargingStation(net, id, lane, frompos, topos, name, chargingPower, efficiency, chargeInTransit, chargeDelay);
}


void
NLTriggerBuilder::parseAndBuildOverheadWireSegment(MSNet& net, const SUMOSAXAttributes& attrs) {
    bool ok = true;

    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }

    /* The following call may either throw InvalidArgument exeption or return NULL:
        NULL is returned in case when the overhead wire segment should be built over an already
        ignored internal lane of an intersection, the exeption is thrown in case that
        the overhead wire segment references a non-existent lane. */
    MSLane* const lane = getLane(attrs, "overheadWireSegment", id);
    if (lane == nullptr) {
        WRITE_MESSAGEF(TL("The overheadWireSegment '%' was not created as it is attached to internal lane. It will be build automatically."), id);
        return;
    }

    if (lane->isInternal()) {
        WRITE_MESSAGEF(TL("The overheadWireSegment '%' not built as it is attached to internal lane. It will be build automatically."), id);
        return;
    }

    double frompos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    double topos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
    const bool voltageSource = attrs.getOpt<bool>(SUMO_ATTR_VOLTAGESOURCE, id.c_str(), ok, false);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);

    if (!ok || myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos) != SUMORouteHandler::StopPos::STOPPOS_VALID) {
        frompos = 0;
        topos = lane->getLength();
        WRITE_MESSAGEF(TL("The overheadWireSegment '%' has wrong position. Automatically set from 0 to the length of the lane."), id);
        //throw InvalidArgument("Invalid position for overheadWireSegment'" + id + "'.");
    }

    buildOverheadWireSegment(net, id, lane, frompos, topos, voltageSource);
#ifndef HAVE_EIGEN
    if (MSGlobals::gOverheadWireSolver && !myHaveWarnedAboutEigen) {
        myHaveWarnedAboutEigen = true;
        WRITE_WARNING(TL("Overhead wire solver (Eigen) not compiled in, expect errors in overhead wire simulation"))
    }
#endif // !HAVE_EIGEN
}

void
NLTriggerBuilder::parseAndBuildOverheadWireSection(MSNet& net, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string substationId = attrs.get<std::string>(SUMO_ATTR_SUBSTATIONID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }

    MSTractionSubstation* substation = MSNet::getInstance()->findTractionSubstation(substationId);
    if (substation == nullptr) {
        throw InvalidArgument("Traction substation '" + substationId + "' refereced by an OverheadWire Section is not known.");
    } else if (substation->isAnySectionPreviouslyDefined()) {
        throw InvalidArgument("Traction substation '" + substationId + "' refereced by an OverheadWire Section is probably referenced twice (a known limitation of the actual version of overhead wire simulation).");
    }

    // @todo This may be a relict of older approach to processing the attributes ...
    std::string segmentStrings = attrs.get<std::string>(SUMO_ATTR_OVERHEAD_WIRE_SEGMENTS, substationId.c_str(), ok);
    if (!ok) {
        throw InvalidArgument("Segments referenced by Traction substation '" + substationId + "' are not declared .");
    }

    // process forbidden internal lanes
    const std::vector<std::string>& forbiddenInnerLanesIDs = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN, substationId.c_str(), ok);
    /// @todo for cycle abbreviation?
    for (const std::string& laneID : forbiddenInnerLanesIDs) {
        MSLane* lane = MSLane::dictionary(laneID);
        if (lane != nullptr) {
            substation->addForbiddenLane(lane);
        } else {
            throw InvalidArgument("Unknown lane '" + laneID + "' in Traction substation '" + substationId + "'.");
        }
    }

    // @todo Check this as well ...
    // Original version from 2018
    // std::vector<std::string> segmentIDs;
    // SUMOSAXAttributes::parseStringVector(segmentStrings, segmentIDs);
    const std::vector<std::string>& segmentIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_OVERHEAD_WIRE_SEGMENTS, substationId.c_str(), ok);
    std::vector<MSOverheadWire*> segments;

    // ----------------------------------------------
    // Add overhead wire segments over internal lanes
    // ----------------------------------------------

    // Adding internal overhead wire segments (segments on neighboring inner lanes if a connection between two regular lane with overhead wire segment exists)
    for (const std::string& segmentID : segmentIDs) {
        const MSLane* connection = nullptr;
        MSOverheadWire* ovrhdSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(segmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        std::string neigboringOvrhdSegmentID;
        MSOverheadWire* neigboringOvrhdSegment;
        MSTractionSubstation* neigboringOvrhdSegmentTractionSubstation;
        if (ovrhdSegment == nullptr) {
            throw InvalidArgument("The OverheadWireSegment with id='" + segmentID + "' referenced by OverheadWireSegment for substation '" + substationId + "' was not defined.");
        }

        MSTractionSubstation* ts = ovrhdSegment->getTractionSubstation();
        if (!(ts == substation || ts == nullptr)) {
            std::string tsName = ts->getID();
            throw InvalidArgument("The OverheadWireSegment '" + segmentID + "' referenced by OverheadWireSegment for substation '" + substationId + "' is already assigned to substation '" + tsName + "'.");
        }
        ovrhdSegment->setTractionSubstation(substation);

        const MSLane* lane = &(ovrhdSegment->getLane());

        /* in version before SUMO 1.0.1 the function getOutgoingLanes() returning MSLane* exists,
        in new version of SUMO the funciton getOutgoingViaLanes() returning MSLane* and MSEdge* pair exists */
        const std::vector<std::pair<const MSLane*, const MSEdge*> > outgoingLanesAndEdges = lane->getOutgoingViaLanes();
        std::vector<const MSLane*> neigboringInnerLanes;
        neigboringInnerLanes.reserve(outgoingLanesAndEdges.size());
        for (size_t it = 0; it < outgoingLanesAndEdges.size(); ++it) {
            neigboringInnerLanes.push_back(outgoingLanesAndEdges[it].first);
        }

        // Check if an outgoing lane has an overhead wire segment. If not, do nothing, otherwise find connnecting internal lanes and
        // add overhead wire segments over all detected internal lanes
        for (std::vector<const MSLane*>::iterator it = neigboringInnerLanes.begin(); it != neigboringInnerLanes.end(); ++it) {
            // If the overhead wire segment is over the outgoing (not internal) lane
            neigboringOvrhdSegmentID = MSNet::getInstance()->getStoppingPlaceID(*it, NUMERICAL_EPS, SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
            if (neigboringOvrhdSegmentID != "") {
                neigboringOvrhdSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(neigboringOvrhdSegmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
                neigboringOvrhdSegmentTractionSubstation = neigboringOvrhdSegment->getTractionSubstation();
            } else {
                neigboringOvrhdSegment = nullptr;
                neigboringOvrhdSegmentTractionSubstation = nullptr;
            }

            if (neigboringOvrhdSegmentTractionSubstation == substation && !(*it)->isInternal()) {
                connection = lane->getInternalFollowingLane(*it);
                if (connection != nullptr) {
                    //is connection forbidden?
                    if (!(substation->isForbidden(connection) || substation->isForbidden(lane->getInternalFollowingLane(connection)) || substation->isForbidden(connection->getInternalFollowingLane(*it)))) {
                        buildInnerOverheadWireSegments(net, connection, lane->getInternalFollowingLane(connection), connection->getInternalFollowingLane(*it));
                    }
                }
            }
        }

        // Check if an incoming lane has an overhead wire segment. If not, do nothing, otherwise find connnecting internal lanes and
        // add overhead wire segments over all detected internal lanes
        neigboringInnerLanes = lane->getNormalIncomingLanes();
        for (std::vector<const MSLane*>::iterator it = neigboringInnerLanes.begin(); it != neigboringInnerLanes.end(); ++it) {
            // If the overhead wire segment is over the incoming (not internal) lane
            neigboringOvrhdSegmentID = MSNet::getInstance()->getStoppingPlaceID(*it, (*it)->getLength() - NUMERICAL_EPS, SUMO_TAG_OVERHEAD_WIRE_SEGMENT);
            if (neigboringOvrhdSegmentID != "") {
                neigboringOvrhdSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(neigboringOvrhdSegmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
                neigboringOvrhdSegmentTractionSubstation = neigboringOvrhdSegment->getTractionSubstation();
            } else {
                neigboringOvrhdSegment = nullptr;
                neigboringOvrhdSegmentTractionSubstation = nullptr;
            }

            if (neigboringOvrhdSegmentTractionSubstation == substation && !(*it)->isInternal()) {
                connection = (*it)->getInternalFollowingLane(lane);
                if (connection != nullptr) {
                    //is connection forbidden?
                    if (!(substation->isForbidden(connection) || substation->isForbidden((*it)->getInternalFollowingLane(connection)) || substation->isForbidden(connection->getInternalFollowingLane(lane)))) {
                        buildInnerOverheadWireSegments(net, connection, (*it)->getInternalFollowingLane(connection), connection->getInternalFollowingLane(lane));
                    }
                }
            }
        }
    }


    // ----- *** adding segments into the electric circuit*** -----

    // setting nullptr for substation (a fragment from old version of adding segments into the circuit)
    for (const std::string& segmentID : segmentIDs) {
        MSOverheadWire* ovrhdSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(segmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        ovrhdSegment->setTractionSubstation(nullptr);
    }

    for (const std::string& segmentID : segmentIDs) {
        if (segmentID == "") {
            continue;
        }
        MSOverheadWire* ovrhdSegment = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(segmentID, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        substation->addOverheadWireSegmentToCircuit(ovrhdSegment);
        segments.push_back(ovrhdSegment);
    }

    // adding overhead wire clamp
    std::string clampsString = attrs.getOpt<std::string>(SUMO_ATTR_OVERHEAD_WIRE_CLAMPS, nullptr, ok, "");
    if (clampsString != "" && MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
        const std::vector<std::string>& clampIDs = attrs.get<std::vector<std::string> >(SUMO_ATTR_OVERHEAD_WIRE_CLAMPS, nullptr, ok);
        for (const std::string& clampID : clampIDs) {
            MSTractionSubstation::OverheadWireClamp* clamp = substation->findClamp(clampID);
            if (clamp != nullptr) {
                if (clamp->start->getTractionSubstation() == substation && clamp->end->getTractionSubstation() == substation) {
                    substation->addOverheadWireClampToCircuit(clamp->id, clamp->start, clamp->end);
                    buildOverheadWireClamp(net, clamp->id, const_cast<MSLane*>(&clamp->start->getLane()), const_cast<MSLane*>(&clamp->end->getLane()));
                    clamp->usage = true;
                } else {
                    if (clamp->start->getTractionSubstation() != substation) {
                        WRITE_WARNINGF(TL("A connecting overhead wire start segment '%' defined for overhead wire clamp '%' is not assigned to the traction substation '%'."), clamp->start->getID(), clampID, substationId);
                    } else {
                        WRITE_WARNINGF(TL("A connecting overhead wire end segment '%' defined for overhead wire clamp '%' is not assigned to the traction substation '%'."), clamp->end->getID(), clampID, substationId);
                    }
                }
            } else {
                WRITE_WARNINGF(TL("The overhead wire clamp '%' defined in an overhead wire section was not assigned to the substation '%'. Please define proper <overheadWireClamp .../> in additional files before defining overhead wire section."), clampID, substationId);
            }
        }
#else
        WRITE_WARNING(TL("Overhead circuit solver requested, but solver support (Eigen) not compiled in."));
#endif
    }

    if (segments.size() == 0) {
        throw InvalidArgument("No segments found for overHeadWireSection '" + substationId + "'.");
    } else if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
        // check that the electric circuit makes sense
        segments[0]->getCircuit()->checkCircuit(substationId);
#else
        WRITE_WARNING(TL("Cannot check circuit, overhead circuit solver support (Eigen) not compiled in."));
#endif
    }
}

void
NLTriggerBuilder::parseAndBuildTractionSubstation(MSNet& net, const SUMOSAXAttributes& attrs) {
    bool ok = true;

    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (!ok) {
        throw ProcessError();
    }

    // RICE_TODO Limits are fixed, change them to some predefined constants ...
    const double voltage = attrs.getOpt<double>(SUMO_ATTR_VOLTAGE, id.c_str(), ok, 600);
    const double currentLimit = attrs.getOpt<double>(SUMO_ATTR_CURRENTLIMIT, id.c_str(), ok, 400);
    buildTractionSubstation(net, id, voltage, currentLimit);
}

void
NLTriggerBuilder::parseAndBuildOverheadWireClamp(MSNet& /*net*/, const SUMOSAXAttributes& attrs) {
    if (MSGlobals::gOverheadWireSolver) {
#ifdef HAVE_EIGEN
        bool ok = true;
        std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
        if (!ok) {
            throw ProcessError();
        }

        std::string substationId = attrs.get<std::string>(SUMO_ATTR_SUBSTATIONID, 0, ok);
        if (!ok) {
            throw ProcessError();
        }
        MSTractionSubstation* substation = MSNet::getInstance()->findTractionSubstation(substationId);
        if (substation == nullptr) {
            throw InvalidArgument("Traction substation '" + substationId + "' using within an overheadWireClamp '" + id + "' is not known.");
        }

        std::string overhead_fromItsStart = attrs.get<std::string>(SUMO_ATTR_OVERHEAD_WIRE_CLAMP_START, 0, ok);
        if (!ok) {
            throw ProcessError();
        }
        MSOverheadWire* ovrhdSegment_fromItsStart = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(overhead_fromItsStart, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        if (ovrhdSegment_fromItsStart == nullptr) {
            throw InvalidArgument("The overheadWireSegment '" + overhead_fromItsStart + "' to use within overheadWireClamp '" + id + "' is not known.");
        }
        /*if (ovrhdSegment_fromItsStart->getTractionSubstation() != substation) {
            throw InvalidArgument("The overheadWireSegment '" + overhead_fromItsStart + "' to use within overheadWireClamp is assign to a different overhead wire section or substation.");
        }
        */
        std::string overhead_fromItsEnd = attrs.get<std::string>(SUMO_ATTR_OVERHEAD_WIRE_CLAMP_END, 0, ok);
        if (!ok) {
            throw ProcessError();
        }
        MSOverheadWire* ovrhdSegment_fromItsEnd = dynamic_cast<MSOverheadWire*>(MSNet::getInstance()->getStoppingPlace(overhead_fromItsEnd, SUMO_TAG_OVERHEAD_WIRE_SEGMENT));
        if (ovrhdSegment_fromItsEnd == nullptr) {
            throw InvalidArgument("The overheadWireSegment '" + overhead_fromItsEnd + "' to use within overheadWireClamp '" + id + "' is not known.");
        }
        /*
        if (ovrhdSegment_fromItsEnd->getTractionSubstation() != substation) {
            throw InvalidArgument("The overheadWireSegment '" + overhead_fromItsEnd + "' to use within overheadWireClamp is assign to a different overhead wire section or substation.");
        }
        */
        if (substation->findClamp(id) == nullptr) {
            substation->addClamp(id, ovrhdSegment_fromItsStart, ovrhdSegment_fromItsEnd);
        } else {
            WRITE_ERROR("The overhead wire clamp '" + id + "' is probably declared twice.")
        }
#else
        UNUSED_PARAMETER(attrs);
        WRITE_WARNING(TL("Not building overhead wire clamps, overhead wire solver support (Eigen) not compiled in."));
#endif
    } else {
        WRITE_WARNING(TL("Ignoring overhead wire clamps, they make no sense when overhead wire circuit solver is off."));
    }
}


void
NLTriggerBuilder::parseAndBuildStoppingPlace(MSNet& net, const SUMOSAXAttributes& attrs, const SumoXMLTag element) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        throw ProcessError();
    }

    //get the name, leave blank if not given
    const std::string ptStopName = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok, "");

    //get the color, use default if not given
    // default color, copy from GUIVisualizationStoppingPlaceSettings::busStopColor / containerStopColor
    RGBColor color = attrs.getOpt<RGBColor>(SUMO_ATTR_COLOR, id.c_str(), ok, RGBColor::INVISIBLE);

    MSLane* lane = getLane(attrs, toString(element), id);
    // get the positions
    double frompos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0.);
    double topos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    if (!ok || (myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos) != SUMORouteHandler::StopPos::STOPPOS_VALID)) {
        throw InvalidArgument("Invalid position for " + toString(element) + " '" + id + "'.");
    }
    const std::vector<std::string>& lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), ok);
    int defaultCapacity;
    SumoXMLAttr capacityAttr;
    if (element != SUMO_TAG_CONTAINER_STOP) {
        defaultCapacity = MAX2(MSStoppingPlace::getTransportablesAbreast(topos - frompos, element) * 3, 6);
        capacityAttr = SUMO_ATTR_PERSON_CAPACITY;
    } else {
        defaultCapacity = MSStoppingPlace::getTransportablesAbreast(topos - frompos, element);
        capacityAttr = SUMO_ATTR_CONTAINER_CAPACITY;
    }
    const int transportableCapacity = attrs.getOpt<int>(capacityAttr, id.c_str(), ok, defaultCapacity);
    const double parkingLength = attrs.getOpt<double>(SUMO_ATTR_PARKING_LENGTH, id.c_str(), ok, 0);
    // build the bus stop
    buildStoppingPlace(net, id, lines, lane, frompos, topos, element, ptStopName, transportableCapacity, parkingLength, color);
}


void
NLTriggerBuilder::addAccess(MSNet& /* net */, const SUMOSAXAttributes& attrs) {
    if (myCurrentStop == nullptr) {
        throw InvalidArgument("Could not add access outside a stopping place.");
    }
    // get the lane
    MSLane* lane = getLane(attrs, "access", myCurrentStop->getID());
    if (!lane->allowsVehicleClass(SVC_PEDESTRIAN)) {
        WRITE_WARNINGF(TL("Ignoring invalid access from non-pedestrian lane '%' in busStop '%'."), lane->getID(), myCurrentStop->getID());
        return;
    }
    // get the positions
    bool ok = true;
    double pos = attrs.getOpt<double>(SUMO_ATTR_POSITION, "access", ok, 0);
    const double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, "access", ok, -1);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, "access", ok, false);
    if (!ok || (myHandler->checkStopPos(pos, pos, lane->getLength(), 0, friendlyPos) != SUMORouteHandler::StopPos::STOPPOS_VALID)) {
        throw InvalidArgument("Invalid position " + toString(pos) + " for access on lane '" + lane->getID() + "' in stop '" + myCurrentStop->getID() + "'.");
    }
    // add bus stop access
    if (!myCurrentStop->addAccess(lane, pos, length)) {
        throw InvalidArgument("Duplicate access on lane '" + lane->getID() + "' for stop '" + myCurrentStop->getID() + "'");
    }
}


void
NLTriggerBuilder::parseAndBeginParkingArea(MSNet& net, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        throw ProcessError();
    }
    // get the lane
    MSLane* lane = getLane(attrs, "parkingArea", id);
    // get the positions
    double frompos = attrs.getOpt<double>(SUMO_ATTR_STARTPOS, id.c_str(), ok, 0);
    double topos = attrs.getOpt<double>(SUMO_ATTR_ENDPOS, id.c_str(), ok, lane->getLength());
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, id.c_str(), ok, false);
    unsigned int capacity = attrs.getOpt<int>(SUMO_ATTR_ROADSIDE_CAPACITY, id.c_str(), ok, 0);
    bool onRoad = attrs.getOpt<bool>(SUMO_ATTR_ONROAD, id.c_str(), ok, false);
    double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, id.c_str(), ok, 0);
    double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, id.c_str(), ok, 0);
    double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, id.c_str(), ok, 0);
    const std::string name = attrs.getOpt<std::string>(SUMO_ATTR_NAME, id.c_str(), ok);
    const std::string departPos = attrs.getOpt<std::string>(SUMO_ATTR_DEPARTPOS, id.c_str(), ok);
    if (!ok || (myHandler->checkStopPos(frompos, topos, lane->getLength(), POSITION_EPS, friendlyPos) != SUMORouteHandler::StopPos::STOPPOS_VALID)) {
        throw InvalidArgument("Invalid position for parking area '" + id + "'.");
    }
    const std::vector<std::string>& lines = attrs.getOpt<std::vector<std::string> >(SUMO_ATTR_LINES, id.c_str(), ok);
    // build the parking area
    beginParkingArea(net, id, lines, lane, frompos, topos, capacity, width, length, angle, name, onRoad, departPos);
}



void
NLTriggerBuilder::parseAndAddLotEntry(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // Check for open parking area
    if (myParkingArea == nullptr) {
        throw ProcessError();
    }
    // get the positions
    double x = attrs.get<double>(SUMO_ATTR_X, "", ok);
    if (!ok) {
        throw InvalidArgument("Invalid x position for lot entry.");
    }
    double y = attrs.get<double>(SUMO_ATTR_Y, "", ok);
    if (!ok) {
        throw InvalidArgument("Invalid y position for lot entry.");
    }
    double z = attrs.getOpt<double>(SUMO_ATTR_Z, "", ok, 0.);
    double width = attrs.getOpt<double>(SUMO_ATTR_WIDTH, "", ok, myParkingArea->getWidth());
    double length = attrs.getOpt<double>(SUMO_ATTR_LENGTH, "", ok, myParkingArea->getLength());
    double angle = attrs.getOpt<double>(SUMO_ATTR_ANGLE, "", ok, myParkingArea->getAngle());
    double slope = attrs.getOpt<double>(SUMO_ATTR_SLOPE, "", ok, 0.);
    // add the lot entry
    addLotEntry(x, y, z, width, length, angle, slope);
}


void
NLTriggerBuilder::parseAndBuildCalibrator(MSNet& net, const SUMOSAXAttributes& attrs,
        const std::string& base) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        throw ProcessError();
    }
    MSLane* lane = nullptr;
    MSEdge* edge = nullptr;
    // get the file name to read further definitions from
    if (attrs.hasAttribute(SUMO_ATTR_EDGE)) {
        std::string edgeID = attrs.get<std::string>(SUMO_ATTR_EDGE, id.c_str(), ok);
        edge = MSEdge::dictionary(edgeID);
        if (edge == nullptr) {
            throw InvalidArgument("The edge " + edgeID + " to use within the calibrator '" + id + "' is not known.");
        }
        if (attrs.hasAttribute(SUMO_ATTR_LANE)) {
            lane = getLane(attrs, "calibrator", id);
            if (&lane->getEdge() != edge) {
                throw InvalidArgument("The edge " + edgeID + " to use within the calibrator '" + id
                                      + "' does not match the calibrator lane '" + lane->getID() + ".");
            }
        }
    } else {
        lane = getLane(attrs, "calibrator", id);
        edge = &lane->getEdge();
    }
    const double pos = getPosition(attrs, lane, "calibrator", id, edge);
    const SUMOTime period = attrs.getOptPeriod(id.c_str(), ok, DELTA_T); // !!! no error handling
    const std::string vTypes = attrs.getOpt<std::string>(SUMO_ATTR_VTYPES, id.c_str(), ok, "");
    std::string file = getFileName(attrs, base, true);
    std::string outfile = attrs.getOpt<std::string>(SUMO_ATTR_OUTPUT, id.c_str(), ok, "");
    std::string routeProbe = attrs.getOpt<std::string>(SUMO_ATTR_ROUTEPROBE, id.c_str(), ok, "");
    // differing defaults for backward compatibility, values are dimensionless
    double invalidJamThreshold = attrs.getOpt<double>(SUMO_ATTR_JAM_DIST_THRESHOLD, id.c_str(), ok, MSGlobals::gUseMesoSim ? 0.8 : 0.5);
    MSRouteProbe* probe = nullptr;
    if (routeProbe != "") {
        probe = dynamic_cast<MSRouteProbe*>(net.getDetectorControl().getTypedDetectors(SUMO_TAG_ROUTEPROBE).get(routeProbe));
        if (probe == nullptr) {
            throw InvalidArgument("The routeProbe '" + routeProbe + "' to use within the calibrator '" + id + "' is not known.");
        }
    }
    if (MSGlobals::gUseMesoSim) {
        if (lane != nullptr && edge->getLanes().size() > 1) {
            WRITE_WARNING("Meso calibrator '" + id
                          + "' defined for lane '" + lane->getID()
                          + "' will collect data for all lanes of edge '" + edge->getID() + "'.");
        }
        METriggeredCalibrator* trigger = buildMECalibrator(net, id, edge, pos, file, outfile, period, probe, invalidJamThreshold, vTypes);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
    } else {
        MSCalibrator* trigger = buildCalibrator(net, id, edge, lane, pos, file, outfile, period, probe, invalidJamThreshold, vTypes);
        if (file == "") {
            trigger->registerParent(SUMO_TAG_CALIBRATOR, myHandler);
        }
    }
}


void
NLTriggerBuilder::parseAndBuildRerouter(MSNet& net, const SUMOSAXAttributes& attrs) {
    bool ok = true;
    // get the id, throw if not given or empty...
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
    if (!ok) {
        throw ProcessError();
    }
    if (MSTriggeredRerouter::getInstances().count(id) > 0) {
        throw InvalidArgument("Could not build rerouter '" + id + "'; probably declared twice.");
    }
    MSEdgeVector edges;
    for (const std::string& edgeID : attrs.get<std::vector<std::string> >(SUMO_ATTR_EDGES, id.c_str(), ok)) {
        MSEdge* edge = MSEdge::dictionary(edgeID);
        if (edge == nullptr) {
            throw InvalidArgument("The edge '" + edgeID + "' to use within rerouter '" + id + "' is not known.");
        }
        edges.push_back(edge);
    }
    if (!ok) {
        throw InvalidArgument("The edge to use within rerouter '" + id + "' is not known.");
    }
    if (edges.size() == 0) {
        throw InvalidArgument("No edges found for rerouter '" + id + "'.");
    }
    double prob = attrs.getOpt<double>(SUMO_ATTR_PROB, id.c_str(), ok, 1);
    bool off = attrs.getOpt<bool>(SUMO_ATTR_OFF, id.c_str(), ok, false);
    SUMOTime timeThreshold = TIME2STEPS(attrs.getOpt<double>(SUMO_ATTR_HALTING_TIME_THRESHOLD, id.c_str(), ok, 0));
    const std::string vTypes = attrs.getOpt<std::string>(SUMO_ATTR_VTYPES, id.c_str(), ok, "");
    if (!ok) {
        throw InvalidArgument("Could not parse rerouter '" + id + "'.");
    }
    MSTriggeredRerouter* trigger = buildRerouter(net, id, edges, prob, off, timeThreshold, vTypes);
    // read in the trigger description
    trigger->registerParent(SUMO_TAG_REROUTER, myHandler);
}


// -------------------------


MSLaneSpeedTrigger*
NLTriggerBuilder::buildLaneSpeedTrigger(MSNet& /*net*/, const std::string& id,
                                        const std::vector<MSLane*>& destLanes,
                                        const std::string& file) {
    return new MSLaneSpeedTrigger(id, destLanes, file);
}


METriggeredCalibrator*
NLTriggerBuilder::buildMECalibrator(MSNet& /*net*/, const std::string& id,
                                    const MSEdge* edge,
                                    double pos,
                                    const std::string& file,
                                    const std::string& outfile,
                                    const SUMOTime freq,
                                    MSRouteProbe* probe,
                                    const double invalidJamThreshold,
                                    const std::string& vTypes) {
    return new METriggeredCalibrator(id, edge, pos, file, outfile, freq, MSGlobals::gMesoNet->getSegmentForEdge(*edge, pos)->getLength(), probe, invalidJamThreshold, vTypes);
}


MSCalibrator*
NLTriggerBuilder::buildCalibrator(MSNet& /*net*/, const std::string& id,
                                  MSEdge* edge,
                                  MSLane* lane,
                                  double pos,
                                  const std::string& file,
                                  const std::string& outfile,
                                  const SUMOTime freq,
                                  const MSRouteProbe* probe,
                                  const double invalidJamThreshold,
                                  const std::string& vTypes) {
    return new MSCalibrator(id, edge, lane, pos, file, outfile, freq, edge->getLength(), probe, invalidJamThreshold, vTypes);
}


MSTriggeredRerouter*
NLTriggerBuilder::buildRerouter(MSNet&, const std::string& id,
                                MSEdgeVector& edges, double prob, bool off,
                                SUMOTime timeThreshold, const std::string& vTypes) {
    return new MSTriggeredRerouter(id, edges, prob, off, timeThreshold, vTypes);
}


void
NLTriggerBuilder::buildStoppingPlace(MSNet& net, std::string id, std::vector<std::string> lines, MSLane* lane,
                                     double frompos, double topos, const SumoXMLTag element, std::string ptStopName,
                                     int personCapacity, double parkingLength, RGBColor& color) {
    myCurrentStop = new MSStoppingPlace(id, element, lines, *lane, frompos, topos, ptStopName, personCapacity, parkingLength, color);
    if (!net.addStoppingPlace(element, myCurrentStop)) {
        delete myCurrentStop;
        myCurrentStop = nullptr;
        throw InvalidArgument("Could not build " + toString(element) + " '" + id + "'; probably declared twice.");
    }
}


void
NLTriggerBuilder::beginParkingArea(MSNet& net, const std::string& id,
                                   const std::vector<std::string>& lines,
                                   MSLane* lane, double frompos, double topos,
                                   unsigned int capacity,
                                   double width, double length, double angle, const std::string& name,
                                   bool onRoad,
                                   const std::string& departPos) {
    // Close previous parking area if there are not lots inside
    MSParkingArea* stop = new MSParkingArea(id, lines, *lane, frompos, topos, capacity, width, length, angle, name, onRoad, departPos);
    if (!net.addStoppingPlace(SUMO_TAG_PARKING_AREA, stop)) {
        delete stop;
        throw InvalidArgument("Could not build parking area '" + id + "'; probably declared twice.");
    } else {
        myParkingArea = stop;
    }
}


void
NLTriggerBuilder::addLotEntry(double x, double y, double z,
                              double width, double length,
                              double angle, double slope) {
    if (myParkingArea != nullptr) {
        if (!myParkingArea->parkOnRoad()) {
            myParkingArea->addLotEntry(x, y, z, width, length, angle, slope);
        } else {
            throw InvalidArgument("Cannot not add lot entry to on-road parking area.");
        }
    } else {
        throw InvalidArgument("Could not add lot entry outside a parking area.");
    }
}


void
NLTriggerBuilder::endParkingArea() {
    if (myParkingArea != nullptr) {
        myParkingArea = nullptr;
    } else {
        throw InvalidArgument("Could not end a parking area that is not opened.");
    }
}


void
NLTriggerBuilder::endStoppingPlace() {
    if (myCurrentStop != nullptr) {
        myCurrentStop = nullptr;
    } else {
        throw InvalidArgument("Could not end a stopping place that is not opened.");
    }
}


void
NLTriggerBuilder::buildChargingStation(MSNet& net, const std::string& id, MSLane* lane, double frompos, double topos, const std::string& name,
                                       double chargingPower, double efficiency, bool chargeInTransit, SUMOTime chargeDelay) {
    MSChargingStation* chargingStation = new MSChargingStation(id, *lane, frompos, topos, name, chargingPower, efficiency, chargeInTransit, chargeDelay);
    if (!net.addStoppingPlace(SUMO_TAG_CHARGING_STATION, chargingStation)) {
        delete chargingStation;
        throw InvalidArgument("Could not build charging station '" + id + "'; probably declared twice.");
    }
    myCurrentStop = chargingStation;
}

void
NLTriggerBuilder::buildOverheadWireSegment(MSNet& net, const std::string& id, MSLane* lane, double frompos, double topos,
        bool voltageSource) {
    MSOverheadWire* overheadWireSegment = new MSOverheadWire(id, *lane, frompos, topos, voltageSource);
    if (!net.addStoppingPlace(SUMO_TAG_OVERHEAD_WIRE_SEGMENT, overheadWireSegment)) {
        delete overheadWireSegment;
        throw InvalidArgument("Could not build overheadWireSegment '" + id + "'; probably declared twice.");
    }
}

void
NLTriggerBuilder::buildInnerOverheadWireSegments(MSNet& net, const MSLane* connection, const MSLane* frontConnection, const MSLane* behindConnection) {
    if (frontConnection == NULL && behindConnection == NULL) {
        buildOverheadWireSegment(net, "ovrhd_inner_" + connection->getID(), const_cast<MSLane*>(connection), 0, connection->getLength(), false);
    } else if (frontConnection != NULL && behindConnection == NULL) {
        buildOverheadWireSegment(net, "ovrhd_inner_" + frontConnection->getID(), const_cast<MSLane*>(frontConnection), 0, frontConnection->getLength(), false);
        buildOverheadWireSegment(net, "ovrhd_inner_" + connection->getID(), const_cast<MSLane*>(connection), 0, connection->getLength(), false);
    } else if (frontConnection == NULL && behindConnection != NULL) {
        buildOverheadWireSegment(net, "ovrhd_inner_" + behindConnection->getID(), const_cast<MSLane*>(behindConnection), 0, behindConnection->getLength(), false);
        buildOverheadWireSegment(net, "ovrhd_inner_" + connection->getID(), const_cast<MSLane*>(connection), 0, connection->getLength(), false);
    } else if (frontConnection != NULL && behindConnection != NULL) {
        buildOverheadWireSegment(net, "ovrhd_inner_" + frontConnection->getID(), const_cast<MSLane*>(frontConnection), 0, frontConnection->getLength(), false);
        buildOverheadWireSegment(net, "ovrhd_inner_" + behindConnection->getID(), const_cast<MSLane*>(behindConnection), 0, behindConnection->getLength(), false);
        buildOverheadWireSegment(net, "ovrhd_inner_" + connection->getID(), const_cast<MSLane*>(connection), 0, connection->getLength(), false);
    }
}

void
NLTriggerBuilder::buildTractionSubstation(MSNet& net, std::string id, double voltage, double currentLimit) {
    MSTractionSubstation* myTractionSubstation = new MSTractionSubstation(id, voltage, currentLimit);
    if (!net.addTractionSubstation(myTractionSubstation)) {
        delete myTractionSubstation;
        throw InvalidArgument("Could not build traction substation '" + id + "'; probably declared twice.");
    }
}

void
NLTriggerBuilder::buildOverheadWireClamp(MSNet& /*net*/, const std::string& /*id*/, MSLane* /*lane_start*/, MSLane* /*lane_end*/) {
}

std::string
NLTriggerBuilder::getFileName(const SUMOSAXAttributes& attrs,
                              const std::string& base,
                              const bool allowEmpty) {
    // get the file name to read further definitions from
    bool ok = true;
    std::string file = attrs.getOpt<std::string>(SUMO_ATTR_FILE, nullptr, ok, "");
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
    if (lane == nullptr) {
        // Either a lane that is non-existent/broken, or a lane that is internal and has been ignored.
        // We assume that internal lane names start with ':'.
        if (objectid[0] == ':' && !MSGlobals::gUsingInternalLanes) {
            return nullptr;
        }
        // Throw the exception only in case that the lane really does not exist in the network file
        // or it is broken.
        throw InvalidArgument("The lane " + objectid + " to use within the " + tt + " '" + tid + "' is not known.");
    }
    return lane;
}


double
NLTriggerBuilder::getPosition(const SUMOSAXAttributes& attrs,
                              MSLane* lane,
                              const std::string& tt, const std::string& tid,
                              MSEdge* edge) {
    assert(lane != 0 || edge != 0);
    const double length = lane != nullptr ? lane->getLength() : edge->getLength();
    bool ok = true;
    double pos = attrs.get<double>(SUMO_ATTR_POSITION, nullptr, ok);
    const bool friendlyPos = attrs.getOpt<bool>(SUMO_ATTR_FRIENDLY_POS, nullptr, ok, false);
    if (!ok) {
        throw InvalidArgument("Error on parsing a position information.");
    }
    if (pos < 0) {
        pos = length + pos;
    }
    if (pos > length) {
        if (friendlyPos) {
            pos = length - (double) 0.1;
        } else {
            if (lane != nullptr) {
                throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane->getID() + "' length.");
            } else {
                throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the edges's '" + edge->getID() + "' length.");
            }
        }
    }
    return pos;
}

MSStoppingPlace*
NLTriggerBuilder::getCurrentStop() {
    return myParkingArea == nullptr ? myCurrentStop : myParkingArea;
}


/****************************************************************************/
