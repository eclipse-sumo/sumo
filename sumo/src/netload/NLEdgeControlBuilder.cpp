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
NLEdgeControlBuilder::NLEdgeControlBuilder()
        : myCurrentNumericalLaneID(0), myCurrentNumericalEdgeID(0), myEdges(0),
        myEdgesLaneChangeOutputDevice(0)
{
    myActiveEdge = (MSEdge*) 0;
    m_pLaneStorage = new MSEdge::LaneCont();
    m_pDepartLane = (MSLane*) 0;
    m_iNoSingle = m_iNoMulti = 0;
    if(OptionsCont::getOptions().isSet("lanechange-output")) {
        if(!OutputDevice::createDeviceByOption("lanechange-output", "lane_changes")) {
            throw 1;
        }
        myEdgesLaneChangeOutputDevice = &OutputDevice::getDeviceByOption("lanechange-output");
    }
}


NLEdgeControlBuilder::~NLEdgeControlBuilder()
{
    delete m_pLaneStorage;
}


void
NLEdgeControlBuilder::beginEdgeParsing(const string &id,
                                 MSEdge::EdgeBasicFunction function,
                                 bool inner) throw(InvalidArgument)
{
    myActiveEdge = buildEdge(id);
    if (!MSEdge::dictionary(id, myActiveEdge)) {
        throw InvalidArgument("Another edge with the id '" + id + "' exists.");
    }
    myEdges.push_back(myActiveEdge);
    m_pDepartLane = (MSLane*) 0;
    m_Function = function;
    if (inner) {
        m_Function = MSEdge::EDGEFUNCTION_INNERJUNCTION;
    }
    myIsInner = inner;
}


MSLane *
NLEdgeControlBuilder::addLane(const std::string &id,
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
        lane = new MSInternalLane(id, maxSpeed, length, myActiveEdge,
            myCurrentNumericalLaneID++, shape, allowed, disallowed);
        break;
    case MSEdge::EDGEFUNCTION_SOURCE:
    case MSEdge::EDGEFUNCTION_NORMAL:
    case MSEdge::EDGEFUNCTION_SINK:
    case MSEdge::EDGEFUNCTION_INNERJUNCTION:
        lane = new MSLane(id, maxSpeed, length, myActiveEdge,
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


MSEdge *
NLEdgeControlBuilder::closeEdge()
{
    MSEdge::LaneCont *lanes = new MSEdge::LaneCont();
    lanes->reserve(m_pLaneStorage->size());
    copy(m_pLaneStorage->begin(), m_pLaneStorage->end(), back_inserter(*lanes));
    if (m_pLaneStorage->size()==1) {
        m_iNoSingle++;
    } else {
        m_iNoMulti++;
    }
    m_pLaneStorage->clear();
    myActiveEdge->initialize(m_pDepartLane,
                             lanes, m_Function, myEdgesLaneChangeOutputDevice);
    return myActiveEdge;
}


MSEdgeControl *
NLEdgeControlBuilder::build()
{
    MSEdgeControl::EdgeCont *singleLanes = new MSEdgeControl::EdgeCont();
    MSEdgeControl::EdgeCont *multiLanes = new MSEdgeControl::EdgeCont();
    singleLanes->reserve(m_iNoSingle);
    multiLanes->reserve(m_iNoMulti);
    for (EdgeCont::iterator i1=myEdges.begin(); i1!=myEdges.end(); i1++) {
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
NLEdgeControlBuilder::buildEdge(const std::string &id) throw()
{
    return new MSEdge(id, myCurrentNumericalEdgeID++);
}


void
NLEdgeControlBuilder::parseVehicleClasses(const std::string &allowedS,
        std::vector<SUMOVehicleClass> &allowed,
        std::vector<SUMOVehicleClass> &disallowed) throw()
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



/****************************************************************************/

