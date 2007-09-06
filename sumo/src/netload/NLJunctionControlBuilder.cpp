/****************************************************************************/
/// @file    NLJunctionControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Container for MSJunctionControl-structures during
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

#include <map>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <microsim/MSJunctionLogic.h>
#include <microsim/MSNoLogicJunction.h>
#include <microsim/MSRightOfWayJunction.h>
#include <microsim/MSInternalJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/traffic_lights/MSTrafficLightLogic.h>
#include <microsim/traffic_lights/MSSimpleTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include <microsim/MSGlobals.h>
#include "NLBuilder.h"
#include <microsim/traffic_lights/MSAgentbasedTrafficLightLogic.h>
#include <utils/common/UtilExceptions.h>
#include "NLJunctionControlBuilder.h"
#include <microsim/traffic_lights/MSTLLogicControl.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// static variables
// ===========================================================================
const int NLJunctionControlBuilder::TYPE_NOJUNCTION = 0;
const int NLJunctionControlBuilder::TYPE_RIGHT_BEFORE_LEFT = 1;
const int NLJunctionControlBuilder::TYPE_PRIORITY_JUNCTION = 2;
const int NLJunctionControlBuilder::TYPE_DEAD_END = 3;
const int NLJunctionControlBuilder::TYPE_INTERNAL = 4;


// ===========================================================================
// method definitions
// ===========================================================================
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
    if (type=="none") {
        myType = TYPE_NOJUNCTION;
    } else if (type=="right_before_left") {
        myType = TYPE_RIGHT_BEFORE_LEFT;
    } else if (type=="priority") {
        myType = TYPE_PRIORITY_JUNCTION;
    } else if (type=="DEAD_END"||type=="district") {
        myType = TYPE_DEAD_END;
    } else if (type=="internal") {
        myType = TYPE_INTERNAL;
    }
    if (myType<0) {
        throw InvalidArgument("An unknown junction type occured: '" + type + "' on junction '" + id + "'.");

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
    if(myJunctions==0) {
        throw ProcessError("Information about the number of nodes was missing.");
    }
    MSJunction *junction = 0;
    switch (myType) {
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
    case TYPE_INTERNAL:
#ifdef HAVE_INTERNAL_LANES
        if (MSGlobals::gUsingInternalLanes) {
            junction = buildInternalJunction();
        }
#endif
        break;
    default:
        throw InvalidArgument("False junction type.");

    }
    if (junction!=0) {
        myJunctions->push_back(junction);
        if (!MSJunction::dictionary(myActiveID, junction)) {
            throw InvalidArgument("Another junction with the id '" + myActiveID + "' exists.");
        }
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


#ifdef HAVE_INTERNAL_LANES
MSJunction *
NLJunctionControlBuilder::buildInternalJunction()
{
    // build the junction
    return new MSInternalJunction(myActiveID, myPosition, myActiveIncomingLanes,
                                  myActiveInternalLanes);
}
#endif


MSJunctionLogic *
NLJunctionControlBuilder::getJunctionLogicSecure()
{
    // get and check the junction logic
    MSJunctionLogic *jtype = MSJunctionLogic::dictionary(myActiveID);
    if (jtype==0) {
        throw InvalidArgument("Missing junction logic '" + myActiveID + "'.");
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
NLJunctionControlBuilder::getTLLogic(const std::string &id) const throw(InvalidArgument)
{
    return getTLLogicControlToUse().get(id);
}


void
NLJunctionControlBuilder::addJunctionInitInfo(MSTrafficLightLogic *tl)
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
    if (myActiveSubKey=="") {
        myActiveSubKey = "default";
    }
    MSTrafficLightLogic *tlLogic = 0;
    // build the tls-logic in dependance to its type
    if (myLogicType=="actuated") {
        // build an actuated logic
        tlLogic =
            new MSActuatedTrafficLightLogic(myNet, getTLLogicControlToUse(),
                                            myActiveKey, myActiveSubKey,
                                            myActivePhases, step, firstEventOffset, myStdActuatedMaxGap,
                                            myStdActuatedPassingTime, myStdActuatedDetectorGap);
    } else if (myLogicType=="agentbased") {
        // build an agentbased logic
        tlLogic =
            new MSAgentbasedTrafficLightLogic(myNet, getTLLogicControlToUse(),
                                              myActiveKey, myActiveSubKey,
                                              myActivePhases, step, firstEventOffset, myStdLearnHorizon,
                                              myStdDecisionHorizon, myStdDeltaLimit, myStdTCycle);
    } else {
        // build an uncontrolled (fix) tls-logic
        tlLogic =
            new MSSimpleTrafficLightLogic(myNet, getTLLogicControlToUse(),
                                          myActiveKey, myActiveSubKey,
                                          myActivePhases, step, firstEventOffset);
        tlLogic->setParameter(myAdditionalParameter);
    }
    addJunctionInitInfo(tlLogic);
    myActivePhases.clear();
    if (tlLogic!=0) {
        if (!getTLLogicControlToUse().add(myActiveKey, myActiveSubKey, tlLogic)) {
            delete tlLogic;
            throw InvalidArgument("Another logic with id '" + myActiveKey + "' and subid '" + myActiveSubKey + "' exists.");
        }
    }
}


SUMOTime
NLJunctionControlBuilder::computeInitTLSStep()  const
{
    assert(myActivePhases.size()!=0);
    SUMOTime offset = myOffset % myAbsDuration;
    MSSimpleTrafficLightLogic::Phases::const_iterator i = myActivePhases.begin();
    SUMOTime step = 0;
    while (true) {
        if (offset<(*i)->duration) {
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
    MSSimpleTrafficLightLogic::Phases::const_iterator i = myActivePhases.begin();
    while (true) {
        if (offset<(*i)->duration) {
            return (*i)->duration - offset;
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
    myActiveConts.reset(false);
    myRequestSize = -1;
    myLaneNumber = -1;
    myRequestItemNumber = 0;
}


void
NLJunctionControlBuilder::addLogicItem(int request,
                                       const string &response,
                                       const std::string &foes,
                                       bool cont)
{
    if (myRequestSize<=0) {
        throw InvalidArgument("The request size,  the response size or the number of lanes is not given! Contact your net supplier");
    }
    // add the read response for the given request index
    bitset<64> use(response);
    assert(myActiveLogic->size()>(size_t) request);
    (*myActiveLogic)[request] = use;
    // add the read junction-internal foes for the given request index
    bitset<64> use2(foes);
    assert(myActiveFoes->size()>(size_t) request);
    (*myActiveFoes)[request] = use2;
    // add whether the vehicle may drive a little bit further
    myActiveConts.set(request, cont);
    // increse number of set information
    myRequestItemNumber++;
}


void
NLJunctionControlBuilder::initTrafficLightLogic(const std::string &type,
        size_t absDuration,
        int requestSize,
        SUMOReal detectorOffset)
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
    if (myDetectorOffset==-1) {
        // agentbased
        if (myLogicType=="agentbased") {
            myDetectorOffset = myStdDetectorLengths;
        }
        // actuated
        if (myLogicType=="actuated") {
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
    if (myLogicType=="actuated"||myLogicType=="agentbased") {
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
    if (myRequestItemNumber!=myRequestSize) {
        throw InvalidArgument("The description for the junction logic '" + myActiveKey + "' is malicious.");
    }
    MSJunctionLogic *logic =
        new MSBitsetLogic(myRequestSize, myLaneNumber, myActiveLogic, myActiveFoes, myActiveConts);
    MSJunctionLogic::dictionary(myActiveKey, logic); // !!! replacement within the dictionary
}


void
NLJunctionControlBuilder::closeJunctions(NLDetectorBuilder &db,
        const MSEdgeContinuations &edgeContinuations)
{
    for (std::vector<TLInitInfo>::iterator i=myJunctions2PostLoadInit.begin(); i!=myJunctions2PostLoadInit.end(); i++) {
        (*i).logic->setParameter((*i).params);
    }
}


MSTLLogicControl *
NLJunctionControlBuilder::buildTLLogics() const
{
    MSTLLogicControl *ret = myLogicControl;
    myLogicControl = 0;
    ret->closeNetworkReading();
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
    if (myLogicControl!=0) {
        return *myLogicControl;
    }
    return myNet.getTLSControl();
}



void
NLJunctionControlBuilder::addWAUT(SUMOTime refTime, const std::string &id,
                                  const std::string &startProg)
{
    getTLLogicControlToUse().addWAUT(refTime, id, startProg);
}


void
NLJunctionControlBuilder::addWAUTSwitch(const std::string &wautid,
                                        SUMOTime when, const std::string &to)
{
    getTLLogicControlToUse().addWAUTSwitch(wautid, when, to);
}


void
NLJunctionControlBuilder::addWAUTJunction(const std::string &wautid,
        const std::string &junc,
        const std::string &proc, bool sync)
{
    getTLLogicControlToUse().addWAUTJunction(wautid, junc, proc, sync);
}


const string &
NLJunctionControlBuilder::getActiveID() const
{
    return myActiveID;
}


const string &
NLJunctionControlBuilder::getActiveKey() const
{
    return myActiveKey;
}


const string &
NLJunctionControlBuilder::getActiveSubKey() const
{
    return myActiveSubKey;
}


/****************************************************************************/
