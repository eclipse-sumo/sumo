/***************************************************************************
                          NLSucceedingLaneBuilder.h
			  Container for the succeding lanes of a lane during their building
                             -------------------
    project              : SUMO
    begin                : Mon, 22 Oct 2001
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
// Revision 2.0  2002/02/14 14:43:25  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:46  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:11  traffic
// moved from netbuild
//
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#include <string>
#include <map>
#include <vector>
#include "../microsim/MSLane.h"
#include "NLNetBuilder.h"
#include "NLSucceedingLaneBuilder.h"
#include "../utils/XMLBuildingExceptions.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLSucceedingLaneBuilder::NLSucceedingLaneBuilder() 
{
  m_SuccLanes = new MSLane::LinkCont();
  m_SuccLanes->reserve(10);
}

NLSucceedingLaneBuilder::~NLSucceedingLaneBuilder() 
{
  delete m_SuccLanes;
}

void
NLSucceedingLaneBuilder::openSuccLane(string laneId) 
{
  m_CurrentLane = laneId;
}

void 
NLSucceedingLaneBuilder::setSuccJunction(string junctionId) 
{
  m_JunctionId = junctionId;
}

void
NLSucceedingLaneBuilder::addSuccLane(bool yield, string laneId) 
{
   if(laneId=="SUMO_NO_DESTINATION") {
      m_SuccLanes->push_back(new MSLane::Link(0, 0));
   } else {
      MSLane *lane = MSLane::dictionary(laneId);
      if(NLNetBuilder::check && lane==0) throw XMLIdNotKnownException("lane", laneId);
      m_SuccLanes->push_back(new MSLane::Link(lane, yield));
   }
}

void 
NLSucceedingLaneBuilder::closeSuccLane() 
{
  MSLane *current = MSLane::dictionary(m_CurrentLane);
  MSJunction *junction = MSJunction::dictionary(m_JunctionId);
  if(NLNetBuilder::check)  {
    if(current==0) throw XMLIdNotKnownException("lane", m_CurrentLane);
    if(junction==0) 
       throw XMLIdNotKnownException("junction", m_JunctionId);
  }
  MSLane::LinkCont *cont = new MSLane::LinkCont();
  cont->reserve(m_SuccLanes->size());
  copy(m_SuccLanes->begin(), m_SuccLanes->end(), back_inserter(*cont));
  current->initialize(junction, cont);
  m_SuccLanes->clear();
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLSucceedingLaneBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:
