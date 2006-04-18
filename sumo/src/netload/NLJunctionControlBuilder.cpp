/***************************************************************************
                          NLJunctionControlBuilder.cpp
              Container for MSJunctionControl-structures during
              their building
                             -------------------
    project              : SUMO
    begin                : Mon, 9 Jul 2001
    copyright            : (C) 2001 by DLR/IVF http://ivf.dlr.de/
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
// Revision 1.23  2006/04/18 08:05:45  dkrajzew
// beautifying: output consolidation
//
// Revision 1.22  2006/02/27 12:10:41  dkrajzew
// WAUTs added
//
// Revision 1.21  2006/02/23 11:27:57  dkrajzew
// tls may have now several programs
//
// Revision 1.20  2005/11/09 06:43:20  dkrajzew
// TLS-API: MSEdgeContinuations added
//
// Revision 1.19  2005/10/17 09:20:12  dkrajzew
// segfaults on loading broken configs patched
//
// Revision 1.18  2005/10/10 12:11:23  dkrajzew
// reworking the tls-API: made tls-control non-static; made net an element of traffic lights
//
// Revision 1.17  2005/10/07 11:41:49  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.16  2005/09/23 06:04:11  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.15  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.14  2005/05/04 08:41:33  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added
//
// Revision 1.2  2004/12/10 11:43:57  dksumo
// parametrisation of actuated traffic lights added
//
// Revision 1.1  2004/10/22 12:50:19  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.11  2004/08/02 12:47:30  dkrajzew
// using Position2D instead of two SUMOReals
//
// Revision 1.10  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.9  2003/11/18 14:23:57  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.8  2003/07/07 08:35:10  dkrajzew
// changes due to loading of geometry applied from the gui-version
//  (no major drawbacks in loading speed)
//
// Revision 1.7  2003/06/18 11:18:05  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.6  2003/06/05 11:52:27  dkrajzew
// class templates applied; documentation added
//
// Revision 1.5  2003/04/01 15:17:45  dkrajzew
// district-typed junctions added
//
// Revision 1.4  2003/03/17 14:28:09  dkrajzew
// debugging
//
// Revision 1.3  2003/03/03 15:06:33  dkrajzew
// new import format applied; new detectors applied
//
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition
//  parseable in one step
//
// Revision 1.6  2002/06/11 14:39:27  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new
//  netconverting methods debugged
//
// Revision 1.3  2002/04/17 11:18:47  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:01:15  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:24  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:44  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:09  traffic
// moved from netbuild
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

#include <map>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <microsim/MSJunctionLogic.h>
#include <microsim/MSNoLogicJunction.h>
#include <microsim/MSRightOfWayJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include "NLBuilder.h"
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include "NLJunctionControlBuilder.h"
#include <microsim/traffic_lights/MSTLLogicControl.h>

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static variables
 * ======================================================================= */
const int NLJunctionControlBuilder::TYPE_NOJUNCTION = 0;
const int NLJunctionControlBuilder::TYPE_RIGHT_BEFORE_LEFT = 1;
const int NLJunctionControlBuilder::TYPE_PRIORITY_JUNCTION = 2;
const int NLJunctionControlBuilder::TYPE_DEAD_END = 3;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLJunctionControlBuilder::NLJunctionControlBuilder(MSNet &net,
                                                   OptionsCont &oc)
    : myNet(net), myOffset(0), myJunctions(0)
{
    myStdDetectorPositions = oc.getFloat("actuated-tl.detector-pos");
    myStdDetectorLengths = oc.getFloat("agent-tl.detector-len");
    myStdLearnHorizon = oc.getInt("agent-tl.learn-horizon");
    myStdDecisionHorizon = oc.getInt("agent-tl.decision-horizon");
    myStdDeltaLimit = oc.getFloat("agent-tl.min-diff");
    myStdTCycle = oc.getInt("agent-tl.tcycle");
    myStdActuatedMaxGap = oc.getFloat("actuated-tl.max-gap");
    myStdActuatedPassingTime = oc.getFloat("actuated-tl.passing-time");
    myStdActuatedDetectorGap = oc.getFloat("actuated-tl.detector-gap");

    myLogicControl = new MSTLLogicControl();
}


NLJunctionControlBuilder::~NLJunctionControlBuilder()
{
    delete myLogicControl;
    delete myJunctions;
}

void
NLJunctionControlBuilder::prepare(unsigned int no)
{
    myJunctions = new MSJunctionControl::JunctionCont();
    myJunctions->reserve(no);
}


void
NLJunctionControlBuilder::openJunction(const std::string &id,
                                       const std::string &key,
                                       const std::string &type,
                                       SUMOReal x, SUMOReal y)
{
#ifdef HAVE_INTERNAL_LANES
    myActiveInternalLanes.clear();
#endif
    myActiveIncomingLanes.clear();
    myActiveID = id;
    myActiveKey = key;
    myType = -1;
    if(type=="none") {
        myType = TYPE_NOJUNCTION;
    } else if(type=="right_before_left") {
        myType = TYPE_RIGHT_BEFORE_LEFT;
    } else if(type=="priority") {
        myType = TYPE_PRIORITY_JUNCTION;
    } else if(type=="DEAD_END"||type=="district") {
        myType = TYPE_DEAD_END;
    }
    if(myType<0) {
        MsgHandler::getErrorInstance()->inform("An unknown junction type occured: '" + type + "' on junction '" + id + "'.");
        throw ProcessError();
    }
    myPosition.set(x, y);
}


#ifdef HAVE_INTERNAL_LANES
void
NLJunctionControlBuilder::addInternalLane(MSLane *lane)
{
    myActiveInternalLanes.push_back(lane);
}
#endif


void
NLJunctionControlBuilder::addIncomingLane(MSLane *lane)
{
    myActiveIncomingLanes.push_back(lane);
}


void
NLJunctionControlBuilder::closeJunction()
{
    MSJunction *junction;
    switch(myType) {
    case TYPE_NOJUNCTION:
        junction = buildNoLogicJunction();
        break;
    case TYPE_RIGHT_BEFORE_LEFT:
    case TYPE_PRIORITY_JUNCTION:
        junction = buildLogicJunction();
        break;
    case TYPE_DEAD_END:
        junction = buildNoLogicJunction();
        break;
    default:
        MsgHandler::getErrorInstance()->inform("False junction type.");
        throw ProcessError();
    }
    myJunctions->push_back(junction);
    if(!MSJunction::dictionary(myActiveID, junction)) {
        throw XMLIdAlreadyUsedException("junction", myActiveID);
    }
}


MSJunctionControl *
NLJunctionControlBuilder::build() const
{
    MSJunctionControl::JunctionCont *js = myJunctions;
    myJunctions = 0;
    return new MSJunctionControl("", js);
}


MSJunction *
NLJunctionControlBuilder::buildNoLogicJunction()
{
    return new MSNoLogicJunction(myActiveID, myPosition, myActiveIncomingLanes
#ifdef HAVE_INTERNAL_LANES
        , myActiveInternalLanes
#endif
        );
}


MSJunction *
NLJunctionControlBuilder::buildLogicJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    // build the junction
    return new MSRightOfWayJunction(myActiveID, myPosition, myActiveIncomingLanes,
#ifdef HAVE_INTERNAL_LANES
        myActiveInternalLanes,
#endif
        jtype);
}


MSJunctionLogic *
NLJunctionControlBuilder::getJunctionLogicSecure()
{
    // get and check the junction logic
    MSJunctionLogic *jtype = MSJunctionLogic::dictionary(myActiveID);
    if(jtype==0) {
        throw XMLIdNotKnownException("junctiontype (key)", myActiveID);
    }
    return jtype;
}


const NLJunctionControlBuilder::LaneVector &
NLJunctionControlBuilder::getIncomingLanes() const
{
    return myActiveIncomingLanes;
}


void
NLJunctionControlBuilder::initIncomingLanes()
{
    myActiveIncomingLanes.clear();
}


const MSTLLogicControl::TLSLogicVariants &
NLJunctionControlBuilder::getTLLogic(const std::string &id) const
{
    return getTLLogicControlToUse().get(id);
}


void
NLJunctionControlBuilder::addJunctionInitInfo(MSExtendedTrafficLightLogic *tl)
{
    TLInitInfo ii;
    ii.logic = tl;
    ii.params = myAdditionalParameter;
    ii.params["detector_offset"] = toString(myDetectorOffset);
    myJunctions2PostLoadInit.push_back(ii);
}


void
NLJunctionControlBuilder::closeTrafficLightLogic()
{
    // compute the initial step of the tls-logic
    size_t step = computeInitTLSStep();
    size_t firstEventOffset = computeInitTLSEventOffset();
    if(myActiveSubKey=="") {
        myActiveSubKey = "<default>";
    }
    MSTrafficLightLogic *tlLogic = 0;
    // build the tls-logic in dependance to its type
    if(myLogicType=="actuated") {
        // build an actuated logic
        tlLogic =
            new MSActuatedTrafficLightLogic(myNet, getTLLogicControlToUse(),
                myActiveKey, myActiveSubKey,
                myActivePhases, step, firstEventOffset, myStdActuatedMaxGap,
                myStdActuatedPassingTime, myStdActuatedDetectorGap);
        addJunctionInitInfo(static_cast<MSExtendedTrafficLightLogic*>(tlLogic));
    } else if(myLogicType=="agentbased") {
        // build an agentbased logic
        tlLogic =
            new MSAgentbasedTrafficLightLogic(myNet, getTLLogicControlToUse(),
                myActiveKey, myActiveSubKey,
                myActivePhases, step, firstEventOffset, myStdLearnHorizon,
                myStdDecisionHorizon, myStdDeltaLimit, myStdTCycle);
        addJunctionInitInfo(static_cast<MSExtendedTrafficLightLogic*>(tlLogic));
    } else {
        // build an uncontrolled (fix) tls-logic
        tlLogic =
            new MSSimpleTrafficLightLogic(myNet, getTLLogicControlToUse(),
                myActiveKey, myActiveSubKey,
                myActivePhases, step, firstEventOffset);
        tlLogic->setParameter(myAdditionalParameter);
    }
    myActivePhases.clear();
    if(tlLogic!=0) {
        if(!getTLLogicControlToUse().add(myActiveKey, myActiveSubKey, tlLogic)) {
            MsgHandler::getErrorInstance()->inform("Another logic with id '" + myActiveKey + "' and subid '" + myActiveSubKey + "' exists.");
        }
    }
}


SUMOTime
NLJunctionControlBuilder::computeInitTLSStep()  const
{
    assert(myActivePhases.size()!=0);
    SUMOTime offset = myOffset % myAbsDuration;
    MSSimpleTrafficLightLogic::Phases::const_iterator i
        = myActivePhases.begin();
    SUMOTime step = 0;
    while(true) {
        if(offset<(*i)->duration) {
            return step;
        }
        step++;
        offset -= (*i)->duration;
        ++i;
    }
}


SUMOTime
NLJunctionControlBuilder::computeInitTLSEventOffset()  const
{
    assert(myActivePhases.size()!=0);
    SUMOTime offset = myOffset % myAbsDuration;
    MSSimpleTrafficLightLogic::Phases::const_iterator i
        = myActivePhases.begin();
    while(true) {
        if(offset<(*i)->duration) {
            return offset;
        }
        offset -= (*i)->duration;
        ++i;
    }
}



void
NLJunctionControlBuilder::initJunctionLogic()
{
    myActiveKey = "";
    myActiveSubKey = "";
    myActiveLogic = new MSBitsetLogic::Logic();
    myActiveFoes = new MSBitsetLogic::Foes();
    myRequestSize = -1;
    myLaneNumber = -1;
    myRequestItemNumber = 0;
}


void
NLJunctionControlBuilder::addLogicItem(int request,
                                       const string &response,
                                       const std::string &foes)
{
    if(myRequestSize<=0) {
        MsgHandler::getErrorInstance()->inform("The request size,  the response size or the number of lanes is not given! Contact your net supplier");
        return;
    }
    // add the read response for the given request index
    bitset<64> use(response);
    assert(myActiveLogic->size()>(size_t) request);
    (*myActiveLogic)[request] = use;
    // add the read junction-internal foes for the given request index
    //  ...but only if junction-internal lanes shall be loaded
    if(MSGlobals::gUsingInternalLanes) {
        bitset<64> use2(foes);
        assert(myActiveFoes->size()>(size_t) request);
        (*myActiveFoes)[request] = use2;
    }
    // increse number of set information
    myRequestItemNumber++;
}


void
NLJunctionControlBuilder::initTrafficLightLogic(const std::string &type,
                                                size_t absDuration,
                                                int requestSize,
                                                int detectorOffset)
{
    myActiveKey = "";
    myActiveSubKey = "";
    myActivePhases.clear();
    myAbsDuration = absDuration;
    myRequestSize = requestSize;
    initIncomingLanes();
    myLogicType = type;
    myDetectorOffset = detectorOffset;
    myAdditionalParameter.clear();
    if(myDetectorOffset==-1) {
        // agentbased
        if(myLogicType=="agentbased") {
            myDetectorOffset = myStdDetectorLengths;
        }
        // actuated
        if(myLogicType=="actuated") {
            myDetectorOffset = myStdDetectorPositions;
        }
    }
}


void
NLJunctionControlBuilder::addPhase(size_t duration, const std::bitset<64> &phase,
                                   const std::bitset<64> &prios,
                                   const std::bitset<64> &yellow,
                                   int min, int max)
{
    // build and add the phase definition to the list
    if(myLogicType=="actuated"||myLogicType=="agentbased") {
        // for a controlled tls-logic
        myActivePhases.push_back(
            new MSActuatedPhaseDefinition(duration, phase, prios, yellow, min, max));
    } else {
        // for an controlled tls-logic
        myActivePhases.push_back(
            new MSPhaseDefinition(duration, phase, prios, yellow));
    }
    // add phase duration to the absolute duration
    myAbsDuration += duration;
}


void
NLJunctionControlBuilder::setRequestSize(int size)
{
    myRequestSize = size;
    myActiveLogic->resize(myRequestSize);
    myActiveFoes->resize(myRequestSize);
}



void
NLJunctionControlBuilder::setLaneNumber(int val)
{
    myLaneNumber = val;
}


void
NLJunctionControlBuilder::setOffset(int val)
{
    myOffset = val;
}


void
NLJunctionControlBuilder::setKey(const std::string &key)
{
    myActiveKey = key;
}


void
NLJunctionControlBuilder::setSubKey(const std::string &key)
{
    myActiveSubKey = key;
}


void
NLJunctionControlBuilder::closeJunctionLogic()
{
    if(myRequestItemNumber!=myRequestSize) {
        MsgHandler::getErrorInstance()->inform("The description for the junction logic '" + myActiveKey + "' is malicious.");
    }
    MSJunctionLogic *logic =
        new MSBitsetLogic(myRequestSize, myLaneNumber, myActiveLogic, myActiveFoes);
    MSJunctionLogic::dictionary(myActiveKey, logic); // !!! replacement within the dictionary
}


void
NLJunctionControlBuilder::closeJunctions(NLDetectorBuilder &db,
                                         const MSEdgeContinuations &edgeContinuations)
{
    for(std::vector<TLInitInfo>::iterator i=myJunctions2PostLoadInit.begin(); i!=myJunctions2PostLoadInit.end(); i++) {
        (*i).logic->setParameter((*i).params);
        (*i).logic->init(db, edgeContinuations);
    }
}


MSTLLogicControl *
NLJunctionControlBuilder::buildTLLogics() const
{
    MSTLLogicControl *ret = myLogicControl;
    myLogicControl = 0;
    ret->markNetLoadingClosed();
    return ret;
}


void
NLJunctionControlBuilder::addParam(const std::string &key,
                                   const std::string &value)
{
    myAdditionalParameter[key] = value;
}


MSTLLogicControl &
NLJunctionControlBuilder::getTLLogicControlToUse() const
{
    if(myLogicControl!=0) {
        return *myLogicControl;
    }
    return myNet.getTLSControl();
}



void
NLJunctionControlBuilder::addWAUT(SUMOTime refTime, const std::string &id,
                                  const std::string &startProg)
{
    if(!getTLLogicControlToUse().addWAUT(refTime, id, startProg)) {
        MsgHandler::getErrorInstance()->inform("WAUT '" + id + "' was already defined.");
    }
}


void
NLJunctionControlBuilder::addWAUTSwitch(const std::string &wautid,
                                        SUMOTime when, const std::string &to)
{
    if(!getTLLogicControlToUse().addWAUTSwitch(wautid, when, to)) {
        MsgHandler::getErrorInstance()->inform("WAUT '" + wautid + "' was not defined.");
    }
}


void
NLJunctionControlBuilder::addWAUTJunction(const std::string &wautid,
                                          const std::string &junc,
                                          const std::string &proc, bool sync)
{
    if(!getTLLogicControlToUse().addWAUTJunction(wautid, junc, proc, sync)) {
        MsgHandler::getErrorInstance()->inform("WAUT '" + wautid + "' or tls '" + junc + "' was not defined.");
    }
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


