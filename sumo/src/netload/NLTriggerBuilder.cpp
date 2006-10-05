/***************************************************************************
                          NLTriggerBuilder.cpp
                          A building helper for triggers
                             -------------------
    begin                : Thu, 17 Oct 2002
    copyright            : (C) 2001 by DLR http://ivf.dlr.de/
    author               : Daniel Krajzewicz
    email                : Daniel.Krajzewicz@dlr.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
namespace
{
     const char rcsid[] =
         "$Id$";
}
// $Log$
// Revision 1.20  2006/10/05 09:29:22  ericnicolay
// in builldTrigger now the event actor at first check if the lane exist.
//
// Revision 1.19  2006/07/06 06:13:33  dkrajzew
// removed some old code
//
// Revision 1.18  2006/07/05 11:40:41  ericnicolay
// change code in buildVehicleActor
//
// Revision 1.17  2006/06/13 13:17:48  dkrajzew
// removed unneeded code
//
// Revision 1.16  2006/04/05 05:33:45  dkrajzew
// retrieval of microsim ids is now also done using getID() instead of id()
//
// Revision 1.15  2006/03/27 07:20:28  dkrajzew
// vehicle actors added, joined some commonly used functions, documentation added
//
// Revision 1.14  2006/03/08 13:15:00  dkrajzew
// friendly_pos usage debugged
//
// Revision 1.13  2006/01/09 12:00:28  dkrajzew
// bus stops implemented
//
// Revision 1.12  2005/11/09 06:35:03  dkrajzew
// Emitters reworked
//
// Revision 1.11  2005/10/10 12:11:33  dkrajzew
// debugging
//
// Revision 1.10  2005/10/07 11:41:49  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.9  2005/09/23 06:04:12  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.8  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.7  2005/05/04 08:43:09  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.6  2005/02/17 10:33:40  dkrajzew
// code beautifying;
// Linux building patched;
// warnings removed;
// new configuration usage within guisim
//
// Revision 1.5  2004/07/02 09:37:31  dkrajzew
// work on class derivation (for online-routing mainly)
//
// Revision 1.4  2003/06/18 11:18:05  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.3  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.2  2002/10/22 10:05:17  dkrajzew
// unsupported return value added
//
// Revision 1.1  2002/10/17 10:50:18  dkrajzew
// building of triggers added (initial)
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

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
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include "NLHandler.h"
#include "NLTriggerBuilder.h"
#include <utils/sumoxml/SUMOXMLDefinitions.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLTriggerBuilder::NLTriggerBuilder()
{
}


NLTriggerBuilder::~NLTriggerBuilder()
{
}


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
    if(type=="lane"&&attr=="speed") {
        t = parseAndBuildLaneSpeedTrigger(net, attrs, base, helper);
    } else if(type=="emitter") {
        t = parseAndBuildLaneEmitTrigger(net, attrs, base, helper);
    } else if(type=="rerouter") {
        t = parseAndBuildRerouter(net, attrs, base, helper);
    } else if(type=="bus_stop") {
        t = parseAndBuildBusStop(net, attrs, base, helper);
    } else if(type=="vehicle_actor") {
		/*first check, that the depending lane realy exist. if not just forget this VehicleActor. */
		MSLane *tlane = MSLane::dictionary(helper.getString(attrs, SUMO_ATTR_OBJECTID));
		if(tlane!=0)
			t = parseAndBuildVehicleActor(net, attrs, base, helper);
    }
    if(t!=0) {
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
    while(st.hasNext()) {
        MSLane *lane = MSLane::dictionary(st.next());
        if(lane==0) {
            MsgHandler::getErrorInstance()->inform("The lane to use within MSLaneSpeedTrigger '" + id + "' is not known.");
            throw ProcessError();
        }
        lanes.push_back(lane);
    }
    if(lanes.size()==0) {
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
                                       const std::string &base,
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
    if(frompos<0) {
        frompos = lane->length() + frompos;
    }
    if(topos<0) {
        topos = lane->length() + topos;
    }
    // get the lines
    std::string lineStr = helper.getStringSecure(attrs, "lines", "");
    std::vector<std::string> lines;
    if(lineStr.length()!=0) {
        StringTokenizer st(lineStr, ";");
        lines = st.getVector();
    }
    // build the bus stop
    return buildBusStop(net, id, lines, lane, frompos, topos);
}


MSE1VehicleActor *
NLTriggerBuilder::parseAndBuildVehicleActor(MSNet &net,
                                            const Attributes &attrs,
                                            const std::string &base,
                                            const NLHandler &helper)
{
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    MSLane *lane = getLane(attrs, helper, "vehicle_actor", id);
    SUMOReal pos = getPosition(attrs, helper, lane, "vehicle_actor", id);
	unsigned int areaid = helper.getInt( attrs, SUMO_ATTR_TO );
	unsigned int areatype = helper.getInt( attrs, SUMO_ATTR_TYPE );
    return buildVehicleActor(net, id, lane, pos, areatype, areaid);
}


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
    while(st.hasNext()) {
        MSEdge *edge = MSEdge::dictionary(st.next());
        if(edge==0) {
            MsgHandler::getErrorInstance()->inform("The edge to use within MSTriggeredRerouter '" + id + "' is not known.");
            throw ProcessError();
        }
        edges.push_back(edge);
    }
    if(edges.size()==0) {
        MsgHandler::getErrorInstance()->inform("No edges found for MSTriggeredRerouter '" + id + "'.");
        throw ProcessError();
    }

    SUMOReal prob;
    try {
        prob = helper.getFloatSecure(attrs, SUMO_ATTR_PROB, 1);
    } catch(NumberFormatException) {
        MsgHandler::getErrorInstance()->inform("Invalid probability in definition of MSTriggeredRerouter '" + id + "'.");
        throw ProcessError();
    }
    MSTriggeredRerouter *ret = buildRerouter(net, id, edges, prob, file);
    if(helper.getBoolSecure(attrs, "off", false)) {
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


MSTriggeredRerouter *
NLTriggerBuilder::buildRerouter(MSNet &net, const std::string &id,
                                std::vector<MSEdge*> &edges,
                                SUMOReal prob, const std::string &file)
{
    return new MSTriggeredRerouter(id, net, edges, prob, file);
}


MSBusStop *
NLTriggerBuilder::buildBusStop(MSNet &net, const std::string &id,
                               const std::vector<std::string> &lines,
                               MSLane *lane,
                               SUMOReal frompos, SUMOReal topos)
{
    return new MSBusStop(id, std::vector<std::string>(), *lane, frompos, topos);
}


MSE1VehicleActor *
NLTriggerBuilder::buildVehicleActor(MSNet &net, const std::string &id,
                                    MSLane *lane, SUMOReal pos, unsigned int type,
									unsigned int areaid)
{
	if ( type == 0 )
		return new MSE1VehicleActor(id, lane, pos, LA, areaid);
	else
		return new MSE1VehicleActor(id, lane, pos, CELL, areaid);

}




std::string
NLTriggerBuilder::getFileName(const Attributes &attrs,
                              const std::string &base,
                              const NLHandler &helper)
{
    // get the file name to read further definitions from
    string file = helper.getString(attrs, SUMO_ATTR_FILE);
        // check whether absolute or relative filenames are given
    if(!FileHelpers::isAbsolute(file)) {
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
    if(lane==0) {
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
    if(pos<0) {
        pos = lane->length() + pos;
    }
    if(pos>lane->length()) {
		if(friendly_pos) {
			pos = lane->length() - (SUMOReal) 0.1;
		} else {
			throw InvalidArgument("The position of " + tt + " '" + tid + "' lies beyond the lane's '" + lane->getID() + "' length.");
		}
    }
    return pos;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
