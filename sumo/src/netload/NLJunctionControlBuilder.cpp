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
     const char rcsid[] = "$Id$";
}
// $Log$
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.6  2002/06/11 14:39:27  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
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
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <map>
#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <microsim/MSJunctionLogic.h>
#include <microsim/MSNoLogicJunction.h>
#include <microsim/MSRightOfWayJunction.h>
#include <microsim/MSJunctionControl.h>
#include <microsim/MSTrafficLightJunction.h>
#include <microsim/MSTrafficLightLogic.h>
#include <microsim/MSSimpleTrafficLightLogic.h>
#include <microsim/MSEventControl.h>
#include "NLNetBuilder.h"
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/SErrorHandler.h>
#include "NLJunctionControlBuilder.h"
#include "NLContainer.h"


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static variables
 * ======================================================================= */
const int NLJunctionControlBuilder::TYPE_NOJUNCTION = 0;
const int NLJunctionControlBuilder::TYPE_TRAFFIC_LIGHT = 1;
const int NLJunctionControlBuilder::TYPE_RIGHT_BEFORE_LEFT = 2;
const int NLJunctionControlBuilder::TYPE_PRIORITY_JUNCTION = 3;
const int NLJunctionControlBuilder::TYPE_DEAD_END = 4;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLJunctionControlBuilder::NLJunctionControlBuilder(NLContainer *container)
    : m_Container(container)
{
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
                                       double x, double y)
{
    m_pActiveInLanes.clear();
    m_CurrentId = id;
    m_Key = key;
    m_TLKey = key;
    m_Type = -1;
    m_Delay = 0;
    m_InitStep = 0;
    if(type=="none") {
        m_Type = TYPE_NOJUNCTION;
    } else if(type=="traffic_light") {
        m_Type = TYPE_TRAFFIC_LIGHT;
    } else if(type=="right_before_left") {
        m_Type = TYPE_RIGHT_BEFORE_LEFT;
    } else if(type=="priority") {
        m_Type = TYPE_PRIORITY_JUNCTION;
    } else if(type=="DEAD_END") {
        m_Type = TYPE_DEAD_END;
    }
    if(m_Type<0) {
        SErrorHandler::add(
            string("An unknown junction type occured: '") + type
            + string("' on junction '") + id + string("'."));
        throw ProcessError();
    }
    m_X = x;
    m_Y = y;
}


void
NLJunctionControlBuilder::addInLane(MSLane *lane)
{
    m_pActiveInLanes.push_back(lane);
}


void
NLJunctionControlBuilder::closeJunction()
{
    MSJunction *junction;
    switch(m_Type) {
    case TYPE_NOJUNCTION:
        cout << "NoJunctions are not yet known. Using PriorityJunction";
        junction = buildLogicJunction();
        break;
    case TYPE_TRAFFIC_LIGHT:
        junction = buildTrafficLightJunction();
        break;
    case TYPE_RIGHT_BEFORE_LEFT:
    case TYPE_PRIORITY_JUNCTION:
        junction = buildLogicJunction();
        break;
    case TYPE_DEAD_END:
        junction = buildNoLogicJunction();
        break;
    default:
        cout << "False junction type." << endl;
        throw exception();
    }
    m_pJunctions->push_back(junction);
    if(!MSJunction::dictionary(m_CurrentId, junction)) {
        throw XMLIdAlreadyUsedException("junction", m_CurrentId);
    }
}


MSJunctionControl *
NLJunctionControlBuilder::build()
{
    return new MSJunctionControl("", m_pJunctions);
}


MSJunction *
NLJunctionControlBuilder::buildNoLogicJunction()
{
    MSNoLogicJunction::InLaneCont *cont =
        new MSNoLogicJunction::InLaneCont();
    cont->reserve(m_pActiveInLanes.size());
    for(LaneCont::iterator i=m_pActiveInLanes.begin();
            i!=m_pActiveInLanes.end(); i++) {
        cont->push_back(*i);
    }
    return new MSNoLogicJunction(m_CurrentId, m_X, m_Y, cont);
}


MSJunction *
NLJunctionControlBuilder::buildLogicJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    MSRightOfWayJunction::InLaneCont cont = getInLaneContSecure();
    // build the junction
    return new MSRightOfWayJunction(m_CurrentId, m_X, m_Y,
        cont, jtype);
}


MSJunction *
NLJunctionControlBuilder::buildTrafficLightJunction()
{
    MSJunctionLogic *jtype = getJunctionLogicSecure();
    MSRightOfWayJunction::InLaneCont cont = getInLaneContSecure();
    // get the traffic light logic
    MSTrafficLightLogic *tlLogic = MSTrafficLightLogic::dictionary(m_CurrentId);//m_TLKey);
    if(tlLogic==0) {
        throw XMLIdNotKnownException("trafficlight (key)", m_CurrentId);//m_TLKey);
    }
    // build the junction
    return new MSTrafficLightJunction(m_CurrentId, m_X, m_Y, cont, jtype,
        tlLogic, m_Delay, m_InitStep, m_Container->getEventControl());
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

MSRightOfWayJunction::InLaneCont
NLJunctionControlBuilder::getInLaneContSecure()
{
    // build the inlane container
    MSRightOfWayJunction::InLaneCont cont;
    cont.reserve(m_pActiveInLanes.size());
    for(LaneCont::iterator i=m_pActiveInLanes.begin(); i!=m_pActiveInLanes.end(); i++) {
        MSRightOfWayJunction::InLane lane(*i);
        cont.push_back(lane);
    }
    return cont;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLJunctionControlBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:


