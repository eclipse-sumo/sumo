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
// Revision 1.1  2002/04/08 07:21:24  traffic
// Initial revision
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
#include "../microsim/MSJunctionLogic.h"
#include "../microsim/MSNoLogicJunction.h"
#include "../microsim/MSRightOfWayJunction.h"
#include "../microsim/MSJunctionControl.h"
#include "NLNetBuilder.h"
#include "../utils/XMLBuildingExceptions.h"
#include "../utils/UtilExceptions.h"
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

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLJunctionControlBuilder::NLJunctionControlBuilder(NLContainer *container) :
  m_Container(container)
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
NLJunctionControlBuilder::openJunction(const std::string id, const std::string key, std::string type) 
{
  m_pActiveInLanes.clear();
  m_CurrentId = id;
  m_Key = key;
  m_Type = -1;
  if(type=="none")
    m_Type = TYPE_NOJUNCTION;
  else if(type=="traffic_light")
    m_Type = TYPE_TRAFFIC_LIGHT;
  else if(type=="right_before_left")
    m_Type = TYPE_RIGHT_BEFORE_LEFT;
  else if(type=="priority")
    m_Type = TYPE_PRIORITY_JUNCTION;
  if(m_Type<0)
    throw exception();
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
    cout << "TrafficLightJunctions are not yet known. Using PriorityJunction";
    junction = buildLogicJunction();
    break;
  case TYPE_RIGHT_BEFORE_LEFT:
  case TYPE_PRIORITY_JUNCTION:
    junction = buildLogicJunction();
    break;
  default:
    cout << "False junction type." << endl;
    throw exception();
  }
  m_pJunctions->push_back(junction);
  if(!MSJunction::dictionary(m_CurrentId, junction))
	  if(NLNetBuilder::check)
      throw XMLIdAlreadyUsedException("junction", m_CurrentId);
}

MSJunctionControl *
NLJunctionControlBuilder::build() 
{
  return new MSJunctionControl("", m_pJunctions);
}

MSJunction *
NLJunctionControlBuilder::buildNoLogicJunction() 
{
  return buildLogicJunction();
  // !!! by now, use another method later
/*  MSNoLogicJunction::InLaneCont *cont = new MSNoLogicJunction::InLaneCont();
  cont->reserve(m_pActiveInLanes.size());
  for(LaneCont::iterator i=m_pActiveInLanes.begin(); i!=m_pActiveInLanes.end(); i++) {
    MSLane *lane = *i;
    cont->push_back(lane);
  }
  return new MSNoLogicJunction(m_CurrentId, cont);*/
}

MSJunction *
NLJunctionControlBuilder::buildLogicJunction() 
{
   MSJunctionLogic *jtype = MSJunctionLogic::dictionary(m_Key);
  if(jtype==0)
    throw XMLIdNotKnownException("junctiontype (key)", m_Key);
  MSRightOfWayJunction::InLaneCont *cont = new MSRightOfWayJunction::InLaneCont();
  cont->reserve(m_pActiveInLanes.size());
  for(LaneCont::iterator i=m_pActiveInLanes.begin(); i!=m_pActiveInLanes.end(); i++) {
    MSRightOfWayJunction::InLane *lane = new MSRightOfWayJunction::InLane(*i);
    cont->push_back(lane);
  }
  return new MSRightOfWayJunction(m_CurrentId, cont, jtype);
}
/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLJunctionControlBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:


