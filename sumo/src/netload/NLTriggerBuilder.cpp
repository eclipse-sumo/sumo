/****************************************************************************/
/// @file    NLTriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Oct 2002
/// @version $Id$
///
// Builds trigger objects for microsim
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
#include <microsim/trigger/MSTriggerControl.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <microsim/trigger/MSBusStop.h>
#include <microsim/trigger/MSE1VehicleActor.h>
#include <microsim/MSPhoneCell.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include "NLHandler.h"
#include "NLTriggerBuilder.h"
#include <utils/xml/SUMOXMLDefinitions.h>
#include <microsim/trigger/MSCalibrator.h>


#ifdef HAVE_MESOSIM
#include <mesosim/METriggeredCalibrator.h>
#include <mesosim/METriggeredScaler.h>
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#endif

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NLTriggerBuilder::NLTriggerBuilder() throw()
{}


NLTriggerBuilder::~NLTriggerBuilder() throw()
{}


MSTrigger *
NLTriggerBuilder::buildTrigger(MSNet &net,
                               const Attributes &attrs,
                               const std::string &base,
                               const NLHandler &helper) throw(InvalidArgument)
{
    string type = helper.getString(attrs, SUMO_ATTR_OBJECTTYPE);
    string attr = helper.getStringSecure(attrs, SUMO_ATTR_ATTR, "");
    // check which type of a trigger shall be build
    MSTrigger *t = 0;
    if (type=="lane"&&attr=="speed") {
        t = parseAndBuildLaneSpeedTrigger(net, attrs, base, helper);
    } else if (type=="emitter") {
        t = parseAndBuildLaneEmitTrigger(net, attrs, base, helper);
    } else if (type=="rerouter") {
        t = parseAndBuildRerouter(net, attrs, base, helper);
    } else if (type=="bus_stop") {
        t = parseAndBuildBusStop(net, attrs, helper);
    } else if (type=="vehicle_actor") {
        /*first check, that the depending lane realy exist. if not just forget this VehicleActor. */
        if (helper.getInt(attrs, SUMO_ATTR_TYPE) == 3) {
            unsigned int cell_id   = helper.getInt(attrs, SUMO_ATTR_ID);
            unsigned int interval  = helper.getInt(attrs, SUMO_ATTR_OBJECTID);
            unsigned int statcount = helper.getInt(attrs, SUMO_ATTR_POSITION);
            MSPhoneNet* pPhone = MSNet::getInstance()->getMSPhoneNet();
            if (pPhone->getMSPhoneCell(cell_id) != 0)
                pPhone->getMSPhoneCell(cell_id)->setStatParams(interval, statcount);
        } else if (helper.getInt(attrs, SUMO_ATTR_TYPE) == 4) {
            /*this is the trigger for the duration for an interval for an hour*/
            unsigned int cell_id   = helper.getInt(attrs, SUMO_ATTR_ID);
            unsigned int interval  = helper.getInt(attrs, SUMO_ATTR_OBJECTID);
            unsigned int count = helper.getInt(attrs, SUMO_ATTR_POSITION);
            float duration = helper.getFloat(attrs, SUMO_ATTR_TO);
            float deviation  = helper.getFloat(attrs, SUMO_ATTR_XTO);
            unsigned int entering  = (unsigned int) helper.getFloat(attrs, SUMO_ATTR_ENTERING);
            //insert in MSPhoneNet
            MSPhoneNet* pPhone = MSNet::getInstance()->getMSPhoneNet();
            if (pPhone->getMSPhoneCell(cell_id) != 0)
                pPhone->getMSPhoneCell(cell_id)->setDynParams(interval, count, duration, deviation, entering);
        } else {
            /*check that the depending lane realy exist. if not just forget this VehicleActor. */
            MSLane *tlane = MSLane::dictionary(helper.getString(attrs, SUMO_ATTR_OBJECTID));
            if (tlane!=0)
                t = parseAndBuildVehicleActor(net, attrs, helper);
        }
    }
#ifndef HAVE_MESOSIM
    else if (type=="calibrator") {
        t = parseAndBuildCalibrator(net, attrs, base, helper);
    }
#endif

#ifdef HAVE_MESOSIM
    else if (type=="calibrator"&&MSGlobals::gUseMesoSim) {
        t = parseAndBuildCalibrator(net, attrs, base, helper);
    } else if (type=="scaler"&&MSGlobals::gUseMesoSim) {
        t = parseAndBuildScaler(net, attrs, helper);
    }
#endif
    if (t!=0) {
        net.getTriggerControl().addTrigger(t);
    }
    return t;
}




MSLaneSpeedTrigger *
NLTriggerBuilder::parseAndBuildLaneSpeedTrigger(MSNet &net, const Attributes &attrs,
        const std::string &base,
        const NLHandler &helper) throw(InvalidArgument)
{
    // get the file name to read further definitions from
    string file = getFileName(attrs, base, helper);
    // lane speed trigger
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    std::vector<MSLane*> lanes;
    StringTokenizer st(objectid, ";");
    while (st.hasNext()) {
        MSLane *lane = MSLane::dictionary(st.next());
        if (lane==0) {
            throw InvalidArgument("The lane to use within MSLaneSpeedTrigger '" + id + "' is not known.");
        }
        lanes.push_back(lane);
    }
    if (lanes.size()==0) {
        throw InvalidArgument("No lane defined for MSLaneSpeedTrigger '" + id + "'.");
    }
    try {
        return buildLaneSpeedTrigger(net, id, lanes, file);
    } catch (ProcessError &e) {
        throw InvalidArgument(e.what());
    }
}


MSEmitter *
NLTriggerBuilder::parseAndBuildLaneEmitTrigger(MSNet &net, const Attributes &attrs,
        const std::string &base,
        const NLHandler &helper) throw(InvalidArgument)
{
    // get the file name to read further definitions from
    string file = getFileName(attrs, base, helper);
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "emitter", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "emitter", id);
    return buildLaneEmitTrigger(net, id, lane, pos, file);
}


MSBusStop *
NLTriggerBuilder::parseAndBuildBusStop(MSNet &net, const Attributes &attrs,
                                       const NLHandler &helper) throw(InvalidArgument)
{
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    // get the lane
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    MSLane *lane = getLane(attrs, helper, "bus_stop", id);
    // get the positions
    SUMOReal frompos, topos;
    try {
        frompos = helper.getFloat(attrs, SUMO_ATTR_FROM);
        topos = helper.getFloat(attrs, SUMO_ATTR_TO);
    } catch (EmptyData&) {
        throw InvalidArgument("Either the begin or the end position of busstop '" + id + "' is not given.");
    } catch (NumberFormatException&) {
        throw InvalidArgument("Either the begin or the end position of busstop '" + id + "' is not numeric.");
    }
    if (frompos<0) {
        frompos = lane->length() + frompos;
    }
    if (topos<0) {
        topos = lane->length() + topos;
    }
    // get the lines
    std::string lineStr = helper.getStringSecure(attrs, SUMO_ATTR_LINES, "");
    std::vector<std::string> lines;
    if (lineStr.length()!=0) {
        StringTokenizer st(lineStr, ";");
        lines = st.getVector();
    }
    // build the bus stop
    return buildBusStop(net, id, lines, lane, frompos, topos);
}


MSE1VehicleActor *
NLTriggerBuilder::parseAndBuildVehicleActor(MSNet &net, const Attributes &attrs,
        const NLHandler &helper) throw(InvalidArgument)
{
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "vehicle_actor", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "vehicle_actor", id);
    unsigned int cellid = helper.getInt(attrs, SUMO_ATTR_TO);
    unsigned int laid = helper.getInt(attrs, SUMO_ATTR_XTO);
    unsigned int type = helper.getInt(attrs, SUMO_ATTR_TYPE);
    return buildVehicleActor(net, id, lane, pos, laid, cellid, type);
}


#ifndef HAVE_MESOSIM
MSCalibrator *
NLTriggerBuilder::parseAndBuildCalibrator(MSNet &net, const Attributes &attrs,
        const std::string &base,
        const NLHandler &helper) throw(InvalidArgument)
{
    // get the file name to read further definitions (route distributions) from
    string file = getFileName(attrs, base, helper);
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "calibrator", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "calibrator", id);
    return buildLaneCalibrator(net, id, lane, pos, file);
}
#endif


#ifdef HAVE_MESOSIM
METriggeredCalibrator *
NLTriggerBuilder::parseAndBuildCalibrator(MSNet &net, const Attributes &attrs,
        const std::string &base,
        const NLHandler &helper) throw(InvalidArgument)
{
    // get the file name to read further definitions from
    string file = getFileName(attrs, base, helper);
    string rfile = helper.getStringSecure(attrs, SUMO_ATTR_RFILE, "");
    if (rfile.length()!=0&&!FileHelpers::isAbsolute(rfile)) {
        rfile = FileHelpers::getConfigurationRelative(base, rfile);
    }
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "calibrator", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "calibrator", id);
    MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(lane->getEdge());
    MESegment *prev = s;
    SUMOReal cpos = 0;
    while (cpos<pos&&s!=0) {
        prev = s;
        cpos += s->getLength();
        s = s->getNextSegment();
    }
    SUMOReal rpos = pos-cpos-prev->getLength();
    return buildCalibrator(net, id, prev, rpos, rfile, file);
}


METriggeredScaler *
NLTriggerBuilder::parseAndBuildScaler(MSNet &net, const Attributes &attrs,
                                      const NLHandler &helper) throw(InvalidArgument)
{
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "scaler", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "scaler", id);
    SUMOReal scale;
    try {
        scale = helper.getFloatSecure(attrs, SUMO_ATTR_WEIGHT, (SUMOReal)1);
    } catch (NumberFormatException &) {
        throw InvalidArgument("Invalid scale in definition of METriggeredScaler '" + id + "'.");

    }
    MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(lane->getEdge());
    MESegment *prev = s;
    SUMOReal cpos = 0;
    while (cpos<pos&&s!=0) {
        prev = s;
        cpos += s->getLength();
        s = s->getNextSegment();
    }
    SUMOReal rpos = pos-cpos-prev->getLength();
    return buildScaler(net, id, prev, rpos, scale);
}
#endif


MSTriggeredRerouter *
NLTriggerBuilder::parseAndBuildRerouter(MSNet &net, const Attributes &attrs,
                                        const std::string &base,
                                        const NLHandler &helper) throw(InvalidArgument)
{
    // get the file name to read further definitions from
    string file = getFileName(attrs, base, helper);
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    std::vector<MSEdge*> edges;
    StringTokenizer st(objectid, ";");
    while (st.hasNext()) {
        MSEdge *edge = MSEdge::dictionary(st.next());
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
        prob = helper.getFloatSecure(attrs, SUMO_ATTR_PROB, 1);
    } catch (NumberFormatException &) {
        throw InvalidArgument("Invalid probability in definition of MSTriggeredRerouter '" + id + "'.");
    }
    MSTriggeredRerouter *ret = buildRerouter(net, id, edges, prob, file);
    if (helper.getBoolSecure(attrs, SUMO_ATTR_OFF, false)) {
        ret->setUserMode(true);
        ret->setUserUsageProbability(0);
    }
    return ret;
}


// -------------------------


MSLaneSpeedTrigger *
NLTriggerBuilder::buildLaneSpeedTrigger(MSNet &net, const std::string &id,
                                        const std::vector<MSLane*> &destLanes,
                                        const std::string &file) throw(ProcessError)
{
    return new MSLaneSpeedTrigger(id, net, destLanes, file);
}


MSEmitter *
NLTriggerBuilder::buildLaneEmitTrigger(MSNet &net, const std::string &id,
                                       MSLane *destLane, SUMOReal pos,
                                       const std::string &file) throw()
{
    return new MSEmitter(id, net, destLane, pos, file);
}


MSCalibrator *
NLTriggerBuilder::buildLaneCalibrator(MSNet &net, const std::string &id,
                                      MSLane *destLane, SUMOReal pos,
                                      const std::string &file) throw()
{
    return new MSCalibrator(id, net, destLane, pos, file);
}


#ifdef HAVE_MESOSIM
METriggeredCalibrator *
NLTriggerBuilder::buildCalibrator(MSNet &net, const std::string &id,
                                  MESegment *edge, SUMOReal pos,
                                  const std::string &rfile, const std::string &file) throw()
{
    return new METriggeredCalibrator(id, net, edge, rfile, file);
}


METriggeredScaler *
NLTriggerBuilder::buildScaler(MSNet &net, const std::string &id,
                              MESegment *edge, SUMOReal pos,
                              SUMOReal scale) throw()
{
    return new METriggeredScaler(id, edge, scale);
}
#endif


MSTriggeredRerouter *
NLTriggerBuilder::buildRerouter(MSNet &, const std::string &id,
                                std::vector<MSEdge*> &edges,
                                SUMOReal prob, const std::string &file) throw()
{
    return new MSTriggeredRerouter(id, edges, prob, file);
}


MSBusStop*
NLTriggerBuilder::buildBusStop(MSNet &, const std::string &id,
                               const std::vector<std::string> &lines,
                               MSLane *lane, SUMOReal frompos, SUMOReal topos) throw()
{
    return new MSBusStop(id, lines, *lane, frompos, topos);
}


MSE1VehicleActor *
NLTriggerBuilder::buildVehicleActor(MSNet &, const std::string &id,
                                    MSLane *lane, SUMOReal pos, unsigned int la,
                                    unsigned int cell, unsigned int type) throw()
{
    return new MSE1VehicleActor(id, lane, pos, la, cell, type);
}




std::string
NLTriggerBuilder::getFileName(const Attributes &attrs,
                              const std::string &base,
                              const NLHandler &helper) throw()
{
    // get the file name to read further definitions from
    string file = helper.getString(attrs, SUMO_ATTR_FILE);
    // check whether absolute or relative filenames are given
    if (!FileHelpers::isAbsolute(file)) {
        return FileHelpers::getConfigurationRelative(base, file);
    }
    return file;
}


MSLane *
NLTriggerBuilder::getLane(const Attributes &attrs,
                          const NLHandler &helper,
                          const std::string &tt,
                          const std::string &tid) throw(InvalidArgument)
{
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    MSLane *lane = MSLane::dictionary(objectid);
    if (lane==0) {
        throw InvalidArgument("The lane " + objectid + " to use within the " + tt + " '" + tid + "' is not known.");
    }
    return lane;
}


SUMOReal
NLTriggerBuilder::getPosition(const Attributes &attrs, const NLHandler &helper,
                              MSLane *lane,
                              const std::string &tt, const std::string &tid) throw(InvalidArgument)
{

    SUMOReal pos = helper.getFloat(attrs, SUMO_ATTR_POSITION);
    bool friendly_pos = helper.getBoolSecure(attrs, SUMO_ATTR_FRIENDLY_POS, false);
    if (pos<0) {
        pos = lane->length() + pos;
    }
    if (pos>lane->length()) {
        if (friendly_pos) {
            pos = lane->length() - (SUMOReal) 0.1;
        } else {
            throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane->getID() + "' length.");
        }
    }
    return pos;
}



/****************************************************************************/
