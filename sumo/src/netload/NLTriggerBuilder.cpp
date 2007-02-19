/****************************************************************************/
/// @file    NLTriggerBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Thu, 17 Oct 2002
/// @version $Id$
///
// A building helper for triggers
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <fstream>
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
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include "NLHandler.h"
#include "NLTriggerBuilder.h"
#include <utils/sumoxml/SUMOXMLDefinitions.h>

#ifdef HAVE_MESOSIM
#include <mesosim/METriggeredCalibrator.h>
#include <mesosim/METriggeredScaler.h>
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#endif

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
NLTriggerBuilder::NLTriggerBuilder()
{}


NLTriggerBuilder::~NLTriggerBuilder()
{}


MSTrigger *
NLTriggerBuilder::buildTrigger(MSNet &net,
                               const Attributes &attrs,
                               const std::string &base,
                               const NLHandler &helper)
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
        t = parseAndBuildBusStop(net, attrs, base, helper);
    } else if (type=="vehicle_actor") {
        /*first check, that the depending lane realy exist. if not just forget this VehicleActor. */
        if (helper.getInt(attrs, SUMO_ATTR_TYPE) == 3) {
            unsigned int cell_id   = helper.getInt(attrs, SUMO_ATTR_ID);
            unsigned int interval  = helper.getInt(attrs, SUMO_ATTR_OBJECTID);
            unsigned int statcount = helper.getInt(attrs, SUMO_ATTR_POS);
//			unsigned int dyncount  = helper.getInt( attrs, SUMO_ATTR_TO );
            //insert in MSPhoneNet
            MSPhoneNet* pPhone = MSNet::getInstance()->getMSPhoneNet();
            if (pPhone->getMSPhoneCell(cell_id) != 0)
                pPhone->getMSPhoneCell(cell_id)->setStatParams(interval, statcount);    
        } else if (helper.getInt(attrs, SUMO_ATTR_TYPE) == 4) {
            /*this is the trigger for the duration for an interval for an hour*/
            unsigned int cell_id   = helper.getInt(attrs, SUMO_ATTR_ID);
            unsigned int interval  = helper.getInt(attrs, SUMO_ATTR_OBJECTID);
            unsigned int count = helper.getInt(attrs, SUMO_ATTR_POS);
            float duration = helper.getFloat(attrs, SUMO_ATTR_TO);
            float deviation  = helper.getFloat(attrs, SUMO_ATTR_XTO);
            //insert in MSPhoneNet
            MSPhoneNet* pPhone = MSNet::getInstance()->getMSPhoneNet();
            if (pPhone->getMSPhoneCell(cell_id) != 0)
                pPhone->getMSPhoneCell(cell_id)->setDynParams( interval, count, duration, deviation );
        } else {
            /*check that the depending lane realy exist. if not just forget this VehicleActor. */
            MSLane *tlane = MSLane::dictionary(helper.getString(attrs, SUMO_ATTR_OBJECTID));
            if (tlane!=0)
                t = parseAndBuildVehicleActor(net, attrs, base, helper);
        }
    }
#ifdef HAVE_MESOSIM
    else if (type=="calibrator"&&MSGlobals::gUseMesoSim) {
        t = parseAndBuildCalibrator(net, attrs, base, helper);
    } else if (type=="scaler"&&MSGlobals::gUseMesoSim) {
        t = parseAndBuildScaler(net, attrs, base, helper);
    }
#endif
    if (t!=0) {
        net.getTriggerControl().addTrigger(t);
    }
    return t;
}




MSLaneSpeedTrigger *
NLTriggerBuilder::parseAndBuildLaneSpeedTrigger(MSNet &net,
        const Attributes &attrs,
        const std::string &base,
        const NLHandler &helper)
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
            MsgHandler::getErrorInstance()->inform("The lane to use within MSLaneSpeedTrigger '" + id + "' is not known.");
            throw ProcessError();
        }
        lanes.push_back(lane);
    }
    if (lanes.size()==0) {
        MsgHandler::getErrorInstance()->inform("No lane defined for MSLaneSpeedTrigger '" + id + "'.");
        throw ProcessError();
    }
    return buildLaneSpeedTrigger(net, id, lanes, file);
}


MSEmitter *
NLTriggerBuilder::parseAndBuildLaneEmitTrigger(MSNet &net,
        const Attributes &attrs,
        const std::string &base,
        const NLHandler &helper)
{
    // get the file name to read further definitions from
    string file = getFileName(attrs, base, helper);
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "emitter", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "emitter", id);
    return buildLaneEmitTrigger(net, id, lane, pos, file);
}


MSBusStop *
NLTriggerBuilder::parseAndBuildBusStop(MSNet &net,
                                       const Attributes &attrs,
                                       const std::string &/*base*/,
                                       const NLHandler &helper)
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
        MsgHandler::getErrorInstance()->inform("Either the begin or the end position of busstop '" + id + "' is not given.");
        throw ProcessError();
    } catch (NumberFormatException&) {
        MsgHandler::getErrorInstance()->inform("Either the begin or the end position of busstop '" + id + "' is not numeric.");
        throw ProcessError();
    }
    if (frompos<0) {
        frompos = lane->length() + frompos;
    }
    if (topos<0) {
        topos = lane->length() + topos;
    }
    // get the lines
    std::string lineStr = helper.getStringSecure(attrs, "lines", "");
    std::vector<std::string> lines;
    if (lineStr.length()!=0) {
        StringTokenizer st(lineStr, ";");
        lines = st.getVector();
    }
    // build the bus stop
    return buildBusStop(net, id, lines, lane, frompos, topos);
}


MSE1VehicleActor *
NLTriggerBuilder::parseAndBuildVehicleActor(MSNet &net,
        const Attributes &attrs,
        const std::string &/*base*/,
        const NLHandler &helper)
{
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "vehicle_actor", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "vehicle_actor", id);
    unsigned int cellid = helper.getInt(attrs, SUMO_ATTR_TO);
    unsigned int laid = helper.getInt(attrs, SUMO_ATTR_XTO);
    unsigned int type = helper.getInt(attrs, SUMO_ATTR_TYPE);
    return buildVehicleActor(net, id, lane, pos, laid, cellid, type);
}


#ifdef HAVE_MESOSIM
METriggeredCalibrator *
NLTriggerBuilder::parseAndBuildCalibrator(MSNet &net,
        const Attributes &attrs,
        const std::string &base,
        const NLHandler &helper)
{
    // get the file name to read further definitions from
    string file = getFileName(attrs, base, helper);

    string rfile = helper.getStringSecure(attrs, "rfile", "");
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
NLTriggerBuilder::parseAndBuildScaler(MSNet &net,
                                      const Attributes &attrs,
                                      const std::string &/*base*/,
                                      const NLHandler &helper)
{
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "scaler", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "scaler", id);

    SUMOReal scale;
    try {
        scale = helper.getFloatSecure(attrs, SUMO_TAG_WEIGHT, 1);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid scale in definition of METriggeredScaler '" + id + "'.");
        throw ProcessError();
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
NLTriggerBuilder::parseAndBuildRerouter(MSNet &net,
                                        const Attributes &attrs,
                                        const std::string &base,
                                        const NLHandler &helper)
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
            MsgHandler::getErrorInstance()->inform("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
            throw ProcessError();
        }
        edges.push_back(edge);
    }
    if (edges.size()==0) {
        MsgHandler::getErrorInstance()->inform("No edges found for MSTriggeredRerouter '" + id + "'.");
        throw ProcessError();
    }

    SUMOReal prob;
    try {
        prob = helper.getFloatSecure(attrs, SUMO_ATTR_PROB, 1);
    } catch (NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid probability in definition of MSTriggeredRerouter '" + id + "'.");
        throw ProcessError();
    }
    MSTriggeredRerouter *ret = buildRerouter(net, id, edges, prob, file);
    if (helper.getBoolSecure(attrs, "off", false)) {
        ret->setUserMode(true);
        ret->setUserUsageProbability(0);
    }
    return ret;
}


// -------------------------


MSLaneSpeedTrigger *
NLTriggerBuilder::buildLaneSpeedTrigger(MSNet &net,
                                        const std::string &id,
                                        const std::vector<MSLane*> &destLanes,
                                        const std::string &file)
{
    return new MSLaneSpeedTrigger(id, net, destLanes, file);
}


MSEmitter *
NLTriggerBuilder::buildLaneEmitTrigger(MSNet &net,
                                       const std::string &id,
                                       MSLane *destLane,
                                       SUMOReal pos,
                                       const std::string &file)
{
    return new MSEmitter(id, net, destLane, pos, file);
}


#ifdef HAVE_MESOSIM
METriggeredCalibrator *
NLTriggerBuilder::buildCalibrator(MSNet &net,
                                  const std::string &id,
                                  MESegment *edge, SUMOReal pos,
                                  const std::string &rfile,
                                  const std::string &file)
{
    return new METriggeredCalibrator(id, net, edge, rfile, file);
}


METriggeredScaler *
NLTriggerBuilder::buildScaler(MSNet &net,
                              const std::string &id,
                              MESegment *edge, SUMOReal pos,
                              SUMOReal scale)
{
    return new METriggeredScaler(id, net, edge, scale);
}
#endif


MSTriggeredRerouter *
NLTriggerBuilder::buildRerouter(MSNet &, const std::string &id,
                                std::vector<MSEdge*> &edges,
                                SUMOReal prob, const std::string &file)
{
    return new MSTriggeredRerouter(id, edges, prob, file);
}


MSBusStop*
NLTriggerBuilder::buildBusStop(MSNet &, const std::string &id,
                               const std::vector<std::string> &/*lines*/,
                               MSLane *lane,
                               SUMOReal frompos, SUMOReal topos)
{
    return new MSBusStop(id, std::vector<std::string>(), *lane, frompos, topos);
}


MSE1VehicleActor *
NLTriggerBuilder::buildVehicleActor(MSNet &, const std::string &id,
                                    MSLane *lane, SUMOReal pos, unsigned int la,
                                    unsigned int cell, unsigned int type)
{
    return new MSE1VehicleActor(id, lane, pos, la, cell, type);
}




std::string
NLTriggerBuilder::getFileName(const Attributes &attrs,
                              const std::string &base,
                              const NLHandler &helper)
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
                          const std::string &tid)
{
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    MSLane *lane = MSLane::dictionary(objectid);
    if (lane==0) {
        MsgHandler::getErrorInstance()->inform("The lane to use within the " + tt + " '" + tid + "' is not known.");
        throw ProcessError();
    }
    return lane;
}


SUMOReal
NLTriggerBuilder::getPosition(const Attributes &attrs, const NLHandler &helper,
                              MSLane *lane,
                              const std::string &tt, const std::string &tid)
{

    SUMOReal pos = helper.getFloat(attrs, SUMO_ATTR_POS);
    bool friendly_pos = helper.getBoolSecure(attrs, "friendly_pos", false);
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

