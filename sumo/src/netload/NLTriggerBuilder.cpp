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
#include <microsim/trigger/MSTriggeredEmitter.h>
#include <microsim/trigger/MSTriggerControl.h>
#include <microsim/trigger/MSTriggeredRerouter.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/UtilExceptions.h>
#include "NLHandler.h"
#include "NLTriggerBuilder.h"
#include <utils/sumoxml/SUMOXMLDefinitions.h>

#ifdef HAVE_MESOSIM
#include <mesosim/METriggeredCalibrator.h>
#include <mesosim/MESegment.h>
#include <mesosim/MELoop.h>
#endif

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
    // check which typ of a trigger shall be build
    MSTrigger *t = 0;
    if(type=="lane"&&attr=="speed") {
        t = parseAndBuildLaneSpeedTrigger(net, attrs, base, helper);
    } else if(type=="emitter") {
        t = parseAndBuildLaneEmitTrigger(net, attrs, base, helper);
    } else if(type=="rerouter") {
        t = parseAndBuildRerouter(net, attrs, base, helper);
    }
#ifdef HAVE_MESOSIM
    else if(type=="calibrator") {
        t = parseAndBuildCalibrator(net, attrs, base, helper);
    }
#endif
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
    // check whether absolute or relative filenames are given
    string file = helper.getString(attrs, SUMO_ATTR_FILE);
    if(!FileHelpers::isAbsolute(file)) {
        file = FileHelpers::getConfigurationRelative(base, file);
    }
    // lane speed trigger
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    std::vector<MSLane*> lanes;
    StringTokenizer st(objectid, ";");
    while(st.hasNext()) {
        MSLane *lane = MSLane::dictionary(st.next());
        if(lane==0) {
            MsgHandler::getErrorInstance()->inform(
                string("The lane to use within MSLaneSpeedTrigger '")
                + id + string("' is not known."));
            throw ProcessError();
        }
        lanes.push_back(lane);
    }
    if(lanes.size()==0) {
        MsgHandler::getErrorInstance()->inform(
            string("No lane defined for MSLaneSpeedTrigger '")
            + id + string("'."));
        throw ProcessError();
    }
    return buildLaneSpeedTrigger(net, id, lanes, file);
}


MSTriggeredEmitter *
NLTriggerBuilder::parseAndBuildLaneEmitTrigger(MSNet &net,
                                               const Attributes &attrs,
                                               const std::string &base,
                                               const NLHandler &helper)
{
    // check whether absolute or relative filenames are given
    string file = helper.getString(attrs, SUMO_ATTR_FILE);
    if(!FileHelpers::isAbsolute(file)) {
        file = FileHelpers::getConfigurationRelative(base, file);
    }
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    MSLane *lane = MSLane::dictionary(objectid);
    if(lane==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The lane to use within MSTriggeredEmitter '")
            + id + string("' is not known."));
        throw ProcessError();
    }
    SUMOReal pos = helper.getFloat(attrs, SUMO_ATTR_POS);
    if(pos<0) {
        pos = lane->length() + pos;
    }
    return buildLaneEmitTrigger(net, id, lane, pos, file);
}


#ifdef HAVE_MESOSIM
METriggeredCalibrator *
NLTriggerBuilder::parseAndBuildCalibrator(MSNet &net,
                                          const Attributes &attrs,
                                          const std::string &base,
                                          const NLHandler &helper)
{
    // check whether absolute or relative filenames are given
    string file = helper.getString(attrs, SUMO_ATTR_FILE);
    if(!FileHelpers::isAbsolute(file)) {
        file = FileHelpers::getConfigurationRelative(base, file);
    }

    string rfile = helper.getStringSecure(attrs, "rfile", "");
    if(rfile.length()!=0&&!FileHelpers::isAbsolute(rfile)) {
        rfile = FileHelpers::getConfigurationRelative(base, rfile);
    }

    string id = helper.getString(attrs, SUMO_ATTR_ID);
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    MSLane *lane = MSLane::dictionary(objectid);
    if(lane==0) {
        MsgHandler::getErrorInstance()->inform(
            string("The lane to use within MSTriggeredEmitter '")
            + id + string("' is not known."));
        throw ProcessError();
    }
    SUMOReal pos = helper.getFloat(attrs, SUMO_ATTR_POS);


        MESegment *s = MSGlobals::gMesoNet->getSegmentForEdge(&(lane->edge()));
        MESegment *prev = s;
        SUMOReal cpos = 0;
        while(cpos<pos&&s!=0) {
            prev = s;
            cpos += s->getLength();
            s = s->getNextSegment();
        }
        SUMOReal rpos = pos-cpos-prev->getLength();

    return buildCalibrator(net, id, prev, rpos, rfile, file);
}
#endif


MSTriggeredRerouter *
NLTriggerBuilder::parseAndBuildRerouter(MSNet &net,
                                        const Attributes &attrs,
                                        const std::string &base,
                                        const NLHandler &helper)
{
    // check whether absolute or relative filenames are given
    string file = helper.getString(attrs, SUMO_ATTR_FILE);
    if(!FileHelpers::isAbsolute(file)) {
        file = FileHelpers::getConfigurationRelative(base, file);
    }
    string id = helper.getString(attrs, SUMO_ATTR_ID);
    string objectid = helper.getString(attrs, SUMO_ATTR_OBJECTID);
    std::vector<MSEdge*> edges;
    StringTokenizer st(objectid, ";");
    while(st.hasNext()) {
        MSEdge *edge = MSEdge::dictionary(st.next());
        if(edge==0) {
            MsgHandler::getErrorInstance()->inform(
                string("The edge to use within MSTriggeredRerouter '")
                + id + string("' is not known."));
            throw ProcessError();
        }
        edges.push_back(edge);
    }
    if(edges.size()==0) {
        MsgHandler::getErrorInstance()->inform(
            string("No edges found for MSTriggeredRerouter '")
            + id + string("'."));
        throw ProcessError();
    }

    SUMOReal prob;
    try {
        prob = helper.getFloatSecure(attrs, SUMO_ATTR_PROB, 1);
    } catch(NumberFormatException) {
        MsgHandler::getErrorInstance()->inform(
            string("Invalid probability in definition of MSTriggeredRerouter '")
            + id + string("'."));
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


MSTriggeredEmitter *
NLTriggerBuilder::buildLaneEmitTrigger(MSNet &net,
                                       const std::string &id,
                                       MSLane *destLane,
                                       SUMOReal pos,
                                       const std::string &file)
{
    return new MSTriggeredEmitter(id, net, destLane, pos, file);
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
#endif


MSTriggeredRerouter *
NLTriggerBuilder::buildRerouter(MSNet &net, const std::string &id,
                                std::vector<MSEdge*> &edges,
                                SUMOReal prob, const std::string &file)
{
    return new MSTriggeredRerouter(id, net, edges, prob, file);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
