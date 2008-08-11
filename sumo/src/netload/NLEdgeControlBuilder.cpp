/****************************************************************************/
/// @file    NLEdgeControlBuilder.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 9 Jul 2001
/// @version $Id$
///
// Interface for building edges
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

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <microsim/MSLane.h>
#include <microsim/MSInternalLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include "NLBuilder.h"
#include "NLEdgeControlBuilder.h"
#include <utils/options/OptionsCont.h>

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
NLEdgeControlBuilder::NLEdgeControlBuilder(unsigned int storageSize)
        : myCurrentNumericalLaneID(0), myCurrentNumericalEdgeID(0), myEdges(0),
        myEdgesLaneChangeOutputDevice(0)
{
    myActiveEdge = (MSEdge*) 0;
    m_pLaneStorage = new MSEdge::LaneCont();
    m_pLaneStorage->reserve(storageSize);
    m_pLanes = (MSEdge::LaneCont*) 0;
    m_pDepartLane = (MSLane*) 0;
    m_iNoSingle = m_iNoMulti = 0;
    if(OptionsCont::getOptions().isSet("lanechange-output")) {
        if(!OutputDevice::createDeviceByOption("lanechange-output", "lane_changes")) {
            throw 1;
        }
        myEdgesLaneChangeOutputDevice = &OutputDevice::getDeviceByOption("lanechange-output");
    }
    myEdges = new EdgeCont();
}


NLEdgeControlBuilder::~NLEdgeControlBuilder()
{
    delete m_pLaneStorage;
    delete myEdges;
    delete m_pLanes;
}


void
NLEdgeControlBuilder::chooseEdge(const string &id,
                                 MSEdge::EdgeBasicFunction function,
                                 bool inner)
{
    if (myEdges==0) {
        throw ProcessError();
    }
    myActiveEdge = buildEdge(id);
    if (!MSEdge::dictionary(id, myActiveEdge)) {
        throw InvalidArgument("Another edge with the id '" + id + "' exists.");
    }
    myEdges->push_back(myActiveEdge);
    /*
    if (myActiveEdge==0) {
        throw InvalidArgument("Trying to define a not declared edge ('" + id + "').");
    }
    */
    m_pDepartLane = (MSLane*) 0;
    m_Function = function;
    if (inner) {
        m_Function = MSEdge::EDGEFUNCTION_INNERJUNCTION;
    }
    myIsInner = inner;
}


MSLane *
NLEdgeControlBuilder::addLane(/*MSNet &net, */const std::string &id,
        SUMOReal maxSpeed, SUMOReal length, bool isDepart,
        const Position2DVector &shape,
        const std::string &vclasses)
{
    // checks if the depart lane was set before
    if (isDepart&&m_pDepartLane!=0) {
        throw InvalidArgument("Lane's '" + id + "' edge already has a depart lane.");
    }
    std::vector<SUMOVehicleClass> allowed, disallowed;
    parseVehicleClasses(vclasses, allowed, disallowed);
    MSLane *lane = 0;
    switch (m_Function) {
    case MSEdge::EDGEFUNCTION_INTERNAL:
        lane = new MSInternalLane(/*net, */id, maxSpeed, length, myActiveEdge,
                                           myCurrentNumericalLaneID++, shape, allowed, disallowed);
        break;
    case MSEdge::EDGEFUNCTION_SOURCE:
    case MSEdge::EDGEFUNCTION_NORMAL:
    case MSEdge::EDGEFUNCTION_SINK:
    case MSEdge::EDGEFUNCTION_INNERJUNCTION:
        lane = new MSLane(/*net, */id, maxSpeed, length, myActiveEdge,
                                   myCurrentNumericalLaneID++, shape, allowed, disallowed);
        break;
    default:
        throw InvalidArgument("Unrecognised edge type.");
    }
    m_pLaneStorage->push_back(lane);
    if (isDepart) {
        m_pDepartLane = lane;
    }
    return lane;
}


void
NLEdgeControlBuilder::parseVehicleClasses(const std::string &allowedS,
        std::vector<SUMOVehicleClass> &allowed,
        std::vector<SUMOVehicleClass> &disallowed)
{
    if (allowedS.length()!=0) {
        StringTokenizer st(allowedS, ";");
        while (st.hasNext()) {
            string next = st.next();
            if (next[0]=='-') {
                disallowed.push_back(getVehicleClassID(next.substr(1)));
            } else {
                allowed.push_back(getVehicleClassID(next));
            }
        }
    }
}


void
NLEdgeControlBuilder::closeLanes()
{
    m_pLanes = new MSEdge::LaneCont();
    m_pLanes->reserve(m_pLaneStorage->size());
    copy(m_pLaneStorage->begin(), m_pLaneStorage->end(),
         back_inserter(*m_pLanes));
    if (m_pLaneStorage->size()==1) {
        m_iNoSingle++;
    } else {
        m_iNoMulti++;
    }
    m_pLaneStorage->clear();
}


MSEdge *
NLEdgeControlBuilder::closeEdge()
{
    if (m_pLanes==0) {
        throw InvalidArgument("Something is corrupt within the definition of lanes for the edge '" + myActiveEdge->getID() + "'.");
    }
    myActiveEdge->initialize(m_pDepartLane,
                             m_pLanes, m_Function, myEdgesLaneChangeOutputDevice);
    m_pLanes = 0;
    return myActiveEdge;
}


MSEdgeControl *
NLEdgeControlBuilder::build()
{
    MSEdgeControl::EdgeCont *singleLanes = new MSEdgeControl::EdgeCont();
    MSEdgeControl::EdgeCont *multiLanes = new MSEdgeControl::EdgeCont();
    singleLanes->reserve(m_iNoSingle);
    multiLanes->reserve(m_iNoMulti);
    for (EdgeCont::iterator i1=myEdges->begin(); i1!=myEdges->end(); i1++) {
        (*i1)->closeBuilding();
        if ((*i1)->nLanes()==1) {
            singleLanes->push_back(*i1);
        } else {
            multiLanes->push_back(*i1);
        }
    }
    return new MSEdgeControl(singleLanes, multiLanes);
}


MSEdge *
NLEdgeControlBuilder::getActiveEdge() const
{
    return myActiveEdge;
}


size_t
NLEdgeControlBuilder::getEdgeCapacity() const
{
    return myEdges==0 ? 0 : myEdges->capacity();
}


MSEdge *
NLEdgeControlBuilder::buildEdge(const std::string &id)
{
    return new MSEdge(id, myCurrentNumericalEdgeID++);
}


/****************************************************************************/

