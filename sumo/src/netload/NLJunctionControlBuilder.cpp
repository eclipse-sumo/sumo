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
#include <config.h>
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
NLJunctionControlBuilder::NLJunctionControlBuilder(OptionsCont &oc)
    : _tlLogicNo(-1), m_Offset(0)
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
}


NLJunctionControlBuilder::~NLJunctionControlBuilder()
{
}

void
NLJunctionControlBuilder::prepare(unsigned int no)
{
    m_pJunctions = new MSJunctionControl::JunctionCont();
    m_pJunctions->reserve(no);
}

void
NLJunctionControlBuilder::openJunction(const std::string &id,
                                       const std::string &key,
                                       const std::string &type,
                                       SUMOReal x, SUMOReal y)
{
    m_pActiveInternalLanes.clear();
    m_pActiveIncomingLanes.clear();
    m_CurrentId = id;
    m_Key = key;
    m_TLKey = key;
    m_Type = -1;
    m_Delay = 0;
    m_InitStep = 0;
    if(type=="none") {
        m_Type = TYPE_NOJUNCTION;
    } else if(type=="right_before_left") {
        m_Type = TYPE_RIGHT_BEFORE_LEFT;
    } else if(type=="priority") {
        m_Type = TYPE_PRIORITY_JUNCTION;
    } else if(type=="DEAD_END"||type=="district") {
        m_Type = TYPE_DEAD_END;
    }
    if(m_Type<0) {
        MsgHandler::getErrorInstance()->inform(
            string("An unknown junction type occured: '") + type
            + string("' on junction '") + id + string("'."));
        throw ProcessError();
    }
    myPosition.set(x, y);
}


void
NLJunctionControlBuilder::addInternalLane(MSLane *lane)
{
    m_pActiveInternalLanes.push_back(lane);
}


void
NLJunctionControlBuilder::addIncomingLane(MSLane *lane)
{
    m_pActiveIncomingLanes.push_back(lane);
}


void
NLJunctionControlBuilder::closeJunction()
{
    MSJunction *junction;
    switch(m_Type) {
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
    m_pJunctions->push_back(junction);
    if(!MSJunction::dictionary(m_CurrentId, junction)) {
        throw XMLIdAlreadyUsedException("junction", m_CurrentId);
    }
}


MSJunctionControl *
NLJunctionControlBuilder::build() const
{
    return new MSJunctionControl("", m_pJunctions);
}


MSJunction *
NLJunctionControlBuilder::buildNoLogicJunction()
{
    return new MSNoLogicJunction(m_CurrentId, myPosition,
        m_pActiveIncomingLanes, m_pActiveInternalLanes);
}


MSJunction *
NLJunctionControlBuilder::buildLogicJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    // build the junction
    return new MSRightOfWayJunction(m_CurrentId, myPosition,
        m_pActiveIncomingLanes, m_pActiveInternalLanes, jtype);
}


MSJunctionLogic *
NLJunctionControlBuilder::getJunctionLogicSecure()
{
    // get and check the junction logic
    MSJunctionLogic *jtype = MSJunctionLogic::dictionary(m_CurrentId);//m_Key);
    if(jtype==0) {
        throw XMLIdNotKnownException("junctiontype (key)", m_CurrentId);//m_Key);
    }
    return jtype;
}


const NLJunctionControlBuilder::LaneVector &
NLJunctionControlBuilder::getIncomingLanes() const
{
    return m_pActiveIncomingLanes;
}


void
NLJunctionControlBuilder::initIncomingLanes()
{
    m_pActiveIncomingLanes.clear();
}


MSTrafficLightLogic * const
NLJunctionControlBuilder::getTLLogic(const std::string &id) const
{
    std::map<std::string, MSTrafficLightLogic*>::const_iterator i =
        myLogics.find(id);
    if(i==myLogics.end()) {
        return 0;
    }
    return (*i).second;
}




void
NLJunctionControlBuilder::addJunctionInitInfo(
            MSExtendedTrafficLightLogic *key,
            const LaneVector &lv, SUMOReal det_offset)
{
    if(myJunctions2PostLoadInit.find(key)!=myJunctions2PostLoadInit.end()) {
        throw XMLIdNotKnownException("junction", key->id());
    }
    myJunctions2PostLoadInit[key] = TLInitInfo(lv, det_offset);
}


void
NLJunctionControlBuilder::addTLLogic(MSTrafficLightLogic *logic)
{
    myLogics[logic->id()] = logic;
}


void
NLJunctionControlBuilder::closeTrafficLightLogic()
{
    if(_tlLogicNo!=0) {
        return;
    }
    // compute the initial step of the tls-logic
    size_t step = computeInitTLSStep();
    size_t firstEventOffset = computeInitTLSEventOffset();
    // build the tls-logic in dependance to its type
    if(m_LogicType=="actuated") {
        // build an actuated logic
        MSActuatedTrafficLightLogic *tlLogic =
            new MSActuatedTrafficLightLogic(m_Key, m_ActivePhases,
                step, firstEventOffset,
                myStdActuatedMaxGap,
                myStdActuatedPassingTime,
                myStdActuatedDetectorGap);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        addTLLogic(tlLogic);
        addJunctionInitInfo(tlLogic,
            getIncomingLanes(), m_DetectorOffset);
    } else if(m_LogicType=="agentbased") {
        // build an agentbased logic
        MSAgentbasedTrafficLightLogic *tlLogic =
            new MSAgentbasedTrafficLightLogic(m_Key, m_ActivePhases,
                step, firstEventOffset,
                myStdLearnHorizon, myStdDecisionHorizon,
                myStdDeltaLimit, myStdTCycle);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        addTLLogic(tlLogic);
        addJunctionInitInfo(tlLogic,
            getIncomingLanes(), m_DetectorOffset);
    } else {
        // build an uncontrolled (fix) tls-logic
        MSTrafficLightLogic *tlLogic =
            new MSSimpleTrafficLightLogic(
                m_Key, m_ActivePhases, step, firstEventOffset);
        MSTrafficLightLogic::dictionary(m_Key, tlLogic);
        // !!! replacement within the dictionary
        m_ActivePhases.clear();
        addTLLogic(tlLogic);
    }
}


SUMOTime
NLJunctionControlBuilder::computeInitTLSStep()  const
{
    assert(m_ActivePhases.size()!=0);
    SUMOTime offset = m_Offset % myAbsDuration;
    MSSimpleTrafficLightLogic::Phases::const_iterator i
        = m_ActivePhases.begin();
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
    assert(m_ActivePhases.size()!=0);
    SUMOTime offset = m_Offset % myAbsDuration;
    MSSimpleTrafficLightLogic::Phases::const_iterator i
        = m_ActivePhases.begin();
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
    m_Key = "";
    m_pActiveLogic = new MSBitsetLogic::Logic();
    m_pActiveFoes = new MSBitsetLogic::Foes();
    _requestSize = -1;
    _laneNo = -1;
    _requestItems = 0;
}


void
NLJunctionControlBuilder::addLogicItem(int request,
                                       const string &response,
                                       const std::string &foes)
{
    if(_requestSize<=0) {
        MsgHandler::getErrorInstance()->inform(
            "The request size,  the response size or the number of lanes is not given! Contact your net supplier");
        return;
    }
    // add the read response for the given request index
    bitset<64> use(response);
    assert(m_pActiveLogic->size()>(size_t) request);
    (*m_pActiveLogic)[request] = use;
    // add the read junction-internal foes for the given request index
    //  ...but only if junction-internal lanes shall be loaded
    if(MSGlobals::gUsingInternalLanes) {
        bitset<64> use2(foes);
        assert(m_pActiveFoes->size()>(size_t) request);
        (*m_pActiveFoes)[request] = use2;
    }
    // increse number of set information
    _requestItems++;
}


void
NLJunctionControlBuilder::initTrafficLightLogic(const std::string &type,
                                                size_t absDuration,
                                                int requestSize, int tlLogicNo,
                                                int detectorOffset)
{
    m_Key = "";
    m_ActivePhases.clear();
    myAbsDuration = absDuration;
    _requestSize = requestSize;
    _tlLogicNo = tlLogicNo;
    initIncomingLanes();
    m_LogicType = type;
    if(m_DetectorOffset==-1) {
        // agentbased
        if(m_LogicType=="agentbased") {
            m_DetectorOffset = myStdDetectorLengths;
        }
        // actuated
        if(m_LogicType=="actuated") {
            m_DetectorOffset = myStdDetectorPositions;
        }
    }
}


void
NLJunctionControlBuilder::addPhase(size_t duration, const std::bitset<64> &phase,
                                   const std::bitset<64> &prios,
                                   const std::bitset<64> &yellow,
                                   int min, int max)
{
    if(_tlLogicNo!=0) {
        return;
    }
    // build and add the phase definition to the list
    if(m_LogicType=="actuated"||m_LogicType=="agentbased") {
        // for a controlled tls-logic
        m_ActivePhases.push_back(
            new MSActuatedPhaseDefinition(
                duration, phase, prios, yellow, min, max));
    } else {
        // for an controlled tls-logic
        m_ActivePhases.push_back(
            new MSPhaseDefinition(
                duration, phase, prios, yellow));
    }
    // add phase duration to the absolute duration
    myAbsDuration += duration;
}


void
NLJunctionControlBuilder::setRequestSize(int size)
{
    _requestSize = size;
    m_pActiveLogic->resize(_requestSize);
    m_pActiveFoes->resize(_requestSize);
}



void
NLJunctionControlBuilder::setLaneNumber(int val)
{
    _laneNo = val;
}


void
NLJunctionControlBuilder::setOffset(int val)
{
    m_Offset = val;
}


void
NLJunctionControlBuilder::setTLLogicNo(int val)
{
    _tlLogicNo = val;
    if(_tlLogicNo<0) {
        MsgHandler::getErrorInstance()->inform("Somenthing is wrong with a traffic light logic number.");
        MsgHandler::getErrorInstance()->inform(
            string(" In logic '") + m_Key + string("'."));
    }
}


void
NLJunctionControlBuilder::setKey(const std::string &key)
{
    m_Key = key;
}


void
NLJunctionControlBuilder::closeJunctionLogic()
{
    if(_requestItems!=_requestSize) {
        MsgHandler::getErrorInstance()->inform(
            string("The description for the junction logic '") +
            m_Key +
            string("' is malicious."));
    }
    MSJunctionLogic *logic =
        new MSBitsetLogic(_requestSize, _laneNo,
            m_pActiveLogic, m_pActiveFoes);
    MSJunctionLogic::dictionary(m_Key, logic); // !!! replacement within the dictionary
}


void
NLJunctionControlBuilder::closeJunctions(NLDetectorBuilder &db,
                                         const SSVMap &continuations)
{
    for(TLLogicInitInfoMap::iterator i=myJunctions2PostLoadInit.begin(); i!=myJunctions2PostLoadInit.end(); i++) {
        (*i).first->init(
            db, (*i).second.first, continuations, (*i).second.second);
    }
}


MSTLLogicControl *
NLJunctionControlBuilder::buildTLLogics() const
{
    std::vector<MSTrafficLightLogic*> logics;
    logics.reserve(myLogics.size());
    std::map<std::string, MSTrafficLightLogic*>::const_iterator i;
    for(i=myLogics.begin(); i!=myLogics.end(); i++) {
        logics.push_back((*i).second);
    }
    return new MSTLLogicControl(logics);
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


