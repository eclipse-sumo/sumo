/****************************************************************************/
/// @file    NLJunctionControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Builder of microsim-junctions and tls
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
// method definitions
// ===========================================================================
NLJunctionControlBuilder::NLJunctionControlBuilder(MSNet &net,
        OptionsCont &oc) throw()
        : myNet(net), myOffset(0), myJunctions(0) {
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
    myJunctions = new MSJunctionControl();
}


NLJunctionControlBuilder::~NLJunctionControlBuilder() throw() {
    delete myLogicControl;
    delete myJunctions;
}

void
NLJunctionControlBuilder::openJunction(const std::string &id,
                                       const std::string &key,
                                       const std::string &type,
                                       SUMOReal x, SUMOReal y,
                                       const Position2DVector &shape) throw(InvalidArgument) {
#ifdef HAVE_INTERNAL_LANES
    myActiveInternalLanes.clear();
#endif
    myActiveIncomingLanes.clear();
    myActiveID = id;
    myActiveKey = key;
    myType = TYPE_UNKNOWN;
    if (type=="right_before_left") {
        myType = TYPE_RIGHT_BEFORE_LEFT;
    } else if (type=="priority") {
        myType = TYPE_PRIORITY_JUNCTION;
    } else if (type=="DEAD_END"||type=="district") {
        myType = TYPE_DEAD_END;
    } else if (type=="internal") {
        myType = TYPE_INTERNAL;
    } else if (type=="unregulated"||type=="none") {
        myType = TYPE_NOJUNCTION;
    }
    if (myType==TYPE_UNKNOWN) {
        throw InvalidArgument("An unknown or invalid junction type '" + type + "' occured in junction '" + id + "'.");
    }
    myPosition.set(x, y);
    myShape = shape;
}


#ifdef HAVE_INTERNAL_LANES
void
NLJunctionControlBuilder::addInternalLane(MSLane *lane) throw() {
    myActiveInternalLanes.push_back(lane);
}
#endif


void
NLJunctionControlBuilder::addIncomingLane(MSLane *lane) throw() {
    myActiveIncomingLanes.push_back(lane);
}


void
NLJunctionControlBuilder::addJunctionShape(const Position2DVector &shape) throw() {
    // @deprecated: at some time, all junctions should have a shape attribute (moved from characters)
    myShape = shape;
}


void
NLJunctionControlBuilder::closeJunction() throw(InvalidArgument, ProcessError) {
    if (myJunctions==0) {
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
        throw InvalidArgument("False junction logic type.");
    }
    if (junction!=0) {
        if (!myJunctions->add(myActiveID, junction)) {
            throw InvalidArgument("Another junction with the id '" + myActiveID + "' exists.");
        }
    }
}


MSJunctionControl *
NLJunctionControlBuilder::build() const throw() {
    MSJunctionControl *js = myJunctions;
    myJunctions = 0;
    return js;
}


MSJunction *
NLJunctionControlBuilder::buildNoLogicJunction() throw() {
    return new MSNoLogicJunction(myActiveID, myPosition, myShape, myActiveIncomingLanes
#ifdef HAVE_INTERNAL_LANES
                                 , myActiveInternalLanes
#endif
                                );
}


MSJunction *
NLJunctionControlBuilder::buildLogicJunction() throw(InvalidArgument) {
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    // build the junction
    return new MSRightOfWayJunction(myActiveID, myPosition, myShape, myActiveIncomingLanes,
#ifdef HAVE_INTERNAL_LANES
                                    myActiveInternalLanes,
#endif
                                    jtype);
}


#ifdef HAVE_INTERNAL_LANES
MSJunction *
NLJunctionControlBuilder::buildInternalJunction() throw() {
    // build the junction
    return new MSInternalJunction(myActiveID, myPosition, myShape, myActiveIncomingLanes,
                                  myActiveInternalLanes);
}
#endif


MSJunctionLogic *
NLJunctionControlBuilder::getJunctionLogicSecure() throw(InvalidArgument) {
    // get and check the junction logic
    if (myLogics.find(myActiveID)==myLogics.end()) {
        throw InvalidArgument("Missing junction logic '" + myActiveID + "'.");
    }
    return myLogics[myActiveID];
}


void
NLJunctionControlBuilder::initIncomingLanes() throw() {
    myActiveIncomingLanes.clear();
}


MSTLLogicControl::TLSLogicVariants &
NLJunctionControlBuilder::getTLLogic(const std::string &id) const throw(InvalidArgument) {
    return getTLLogicControlToUse().get(id);
}


void
NLJunctionControlBuilder::closeTrafficLightLogic() throw(InvalidArgument) {
    if (myAbsDuration==0) {
        throw InvalidArgument("TLS program '" + myActiveSubKey + "' for TLS '" + myActiveKey + "' has a duration of 0.");
    }
    // compute the initial step and first switch time of the tls-logic
    unsigned int step = 0;
    SUMOTime firstEventOffset = 0;
    SUMOTime offset = TMOD((myNet.getCurrentTimeStep() + TMOD(myOffset,myAbsDuration)),myAbsDuration);
    MSSimpleTrafficLightLogic::Phases::const_iterator i = myActivePhases.begin();
    while (offset>=(*i)->duration) {
        step++;
        offset -= (*i)->duration;
        ++i;
    }
    firstEventOffset = (*i)->duration - offset + myNet.getCurrentTimeStep();

    //
    if (myActiveSubKey=="") {
        myActiveSubKey = "default";
    }
    MSTrafficLightLogic *tlLogic = 0;
    // build the tls-logic in dependance to its type
    if (myLogicType=="actuated") {
        // build an actuated logic
        tlLogic =
            new MSActuatedTrafficLightLogic(getTLLogicControlToUse(),
                                            myActiveKey, myActiveSubKey,
                                            myActivePhases, step, firstEventOffset, myStdActuatedMaxGap,
                                            myStdActuatedPassingTime, myStdActuatedDetectorGap);
    } else if (myLogicType=="agentbased") {
        // build an agentbased logic
        tlLogic =
            new MSAgentbasedTrafficLightLogic(getTLLogicControlToUse(),
                                              myActiveKey, myActiveSubKey,
                                              myActivePhases, step, firstEventOffset, myStdLearnHorizon,
                                              myStdDecisionHorizon, myStdDeltaLimit, myStdTCycle);
    } else {
        // build a fixed tls-logic
        tlLogic =
            new MSSimpleTrafficLightLogic(getTLLogicControlToUse(),
                                          myActiveKey, myActiveSubKey,
                                          myActivePhases, step, firstEventOffset);
        tlLogic->setParameter(myAdditionalParameter);
    }
    TLInitInfo ii;
    ii.logic = tlLogic;
    ii.params = myAdditionalParameter;
    ii.params["detector_offset"] = toString(myDetectorOffset);
    myJunctions2PostLoadInit.push_back(ii);
    myActivePhases.clear();
    if (tlLogic!=0) {
        try {
            if (!getTLLogicControlToUse().add(myActiveKey, myActiveSubKey, tlLogic)) {
                throw InvalidArgument("Another logic with id '" + myActiveKey + "' and subid '" + myActiveSubKey + "' exists.");
            }
        } catch (InvalidArgument &e) {
            delete tlLogic;
            throw;
        }
    }
}


void
NLJunctionControlBuilder::initJunctionLogic(const std::string &id, int requestSize, int laneNumber) throw() {
    myActiveKey = id;
    myActiveSubKey = "";
    myActiveLogic = new MSBitsetLogic::Logic();
    myActiveFoes = new MSBitsetLogic::Foes();
    myActiveConts.reset(false);
    myRequestSize = requestSize;
    myLaneNumber = laneNumber;
    myRequestItemNumber = 0;
    myCurrentHasError = false;
    if (myRequestSize>0) {
        myActiveLogic->resize(myRequestSize);
        myActiveFoes->resize(myRequestSize);
    }
}


void
NLJunctionControlBuilder::addLogicItem(int request,
                                       const string &response,
                                       const std::string &foes,
                                       bool cont) throw(InvalidArgument) {
    if (myCurrentHasError) {
        // had an error
        return;
    }
    if (request>63) {
        // bad request
        myCurrentHasError = true;
        throw InvalidArgument("Junction logic '" + myActiveKey + "' is larger than allowed; recheck the network.");
    }
    if (myRequestSize<=0) {
        throw InvalidArgument("The request size, the response size or the number of lanes is not given! Contact your net supplier");
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
NLJunctionControlBuilder::initTrafficLightLogic(const std::string &id, const std::string &programID,
        const std::string &type, int offset, SUMOReal detectorOffset) throw() {
    myActiveKey = id;
    myActiveSubKey = programID;
    myActivePhases.clear();
    myAbsDuration = 0;
    myRequestSize = -1;
    initIncomingLanes();
    myLogicType = type;
    myDetectorOffset = detectorOffset;
    myOffset = offset;
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
NLJunctionControlBuilder::addPhase(SUMOTime duration, const std::string &state,
                                   int minDuration, int maxDuration) throw() {
    // build and add the phase definition to the list
    myActivePhases.push_back(new MSPhaseDefinition(duration, minDuration, maxDuration, state));
    // add phase duration to the absolute duration
    myAbsDuration += duration;
}


void
NLJunctionControlBuilder::setRequestSize(int size) throw() {
    // @deprecated: assuming a net could still use characters for the request size
    myRequestSize = size;
    myActiveLogic->resize(myRequestSize);
    myActiveFoes->resize(myRequestSize);
}



void
NLJunctionControlBuilder::setLaneNumber(int val) throw() {
    // @deprecated: assuming a net could still use characters for the lane number
    myLaneNumber = val;
}


void
NLJunctionControlBuilder::setOffset(int val) throw() {
    // @deprecated: assuming a net could still use characters for the offset
    myOffset = val;
}


void
NLJunctionControlBuilder::setKey(const std::string &key) throw() {
    // @deprecated: assuming a net could still use characters for the id
    myActiveKey = key;
}


void
NLJunctionControlBuilder::setSubKey(const std::string &subkey) throw() {
    // @deprecated: assuming a net could still use characters for the sub id
    myActiveSubKey = subkey;
}


void
NLJunctionControlBuilder::closeJunctionLogic() throw(InvalidArgument) {
    if (myCurrentHasError) {
        // had an error before...
        return;
    }
    if (myRequestItemNumber!=myRequestSize) {
        throw InvalidArgument("The description for the junction logic '" + myActiveKey + "' is malicious.");
    }
    MSJunctionLogic *logic =
        new MSBitsetLogic(myRequestSize, myLaneNumber, myActiveLogic, myActiveFoes, myActiveConts);
    if (myLogics.find(myActiveKey)!=myLogics.end()) {
        throw InvalidArgument("Junction logic '" + myActiveKey + "' was defined twice.");
    }
    myLogics[myActiveKey] = logic;
}


void
NLJunctionControlBuilder::closeJunctions(NLDetectorBuilder &db,
        const MSEdgeContinuations &edgeContinuations) throw() {
    for (std::vector<TLInitInfo>::iterator i=myJunctions2PostLoadInit.begin(); i!=myJunctions2PostLoadInit.end(); i++) {
        (*i).logic->setParameter((*i).params);
        (*i).logic->init(db, edgeContinuations);
    }
}


MSTLLogicControl *
NLJunctionControlBuilder::buildTLLogics() const throw() {
    MSTLLogicControl *ret = myLogicControl;
    myLogicControl = 0;
    ret->closeNetworkReading();
    return ret;
}


void
NLJunctionControlBuilder::addParam(const std::string &key,
                                   const std::string &value) throw() {
    myAdditionalParameter[key] = value;
}


MSTLLogicControl &
NLJunctionControlBuilder::getTLLogicControlToUse() const throw() {
    if (myLogicControl!=0) {
        return *myLogicControl;
    }
    return myNet.getTLSControl();
}


const string &
NLJunctionControlBuilder::getActiveID() const throw() {
    return myActiveID;
}


const string &
NLJunctionControlBuilder::getActiveKey() const throw() {
    return myActiveKey;
}


const string &
NLJunctionControlBuilder::getActiveSubKey() const throw() {
    return myActiveSubKey;
}


/****************************************************************************/
