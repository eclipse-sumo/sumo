/***************************************************************************
                          NLEdgeControlBuilder.cpp
			  Holds the edges while they are build
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
// Revision 1.3  2002/04/17 11:17:01  dkrajzew
// windows-newlines removed
//
// Revision 1.2  2002/04/15 07:05:35  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
//
// Revision 2.0  2002/02/14 14:43:21  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.3  2002/02/13 15:40:41  croessel
// Merge between SourgeForgeRelease and tesseraCVS.
//
// Revision 1.1  2001/12/06 13:36:05  traffic
// moved from netbuild
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include "../microsim/MSLane.h"
#include "../microsim/MSEdge.h"
#include "../microsim/MSEdgeControl.h"
#include "../utils/XMLBuildingExceptions.h"
#include "NLNetBuilder.h"
#include "NLEdgeControlBuilder.h"

/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;

/* =========================================================================
 * method definitions
 * ======================================================================= */
NLEdgeControlBuilder::NLEdgeControlBuilder(unsigned int storageSize) 
{
  m_pActiveEdge = (MSEdge*) 0;
  m_pLaneStorage = new MSEdge::LaneCont();
  m_pLaneStorage->reserve(storageSize);
  m_pLanes = (MSEdge::LaneCont*) 0;
  m_pAllowedLanes = (MSEdge::AllowedLanesCont*) 0;
  m_pDepartLane = (MSLane*) 0;
  m_iNoSingle = m_iNoMulti = 0;
}

NLEdgeControlBuilder::~NLEdgeControlBuilder() 
{
  delete m_pLaneStorage;
}

void 
NLEdgeControlBuilder::prepare(unsigned int no) 
{
  m_pEdges = new EdgeCont();
  m_pEdges->reserve(no);
}

void 
NLEdgeControlBuilder::addEdge(const string id) 
{
  MSEdge *edge = new MSEdge(id);
  if(!MSEdge::dictionary(id, edge))
    throw XMLIdAlreadyUsedException("Edge", id);
  m_pEdges->push_back(edge);
}

void 
NLEdgeControlBuilder::chooseEdge(const string &id) 
{
  m_pActiveEdge = MSEdge::dictionary(id);
  if(/* NLNetBuilder::check && */m_pActiveEdge==0) throw XMLIdNotKnownException("edge", id);
  m_pDepartLane = (MSLane*) 0;
  m_pAllowedLanes = new MSEdge::AllowedLanesCont();
}

MSEdge *
NLEdgeControlBuilder::getActiveReference() 
{
  return m_pActiveEdge;
}

void 
NLEdgeControlBuilder::addLane(MSLane *lane, bool isDepart) 
{
/*  if(NLNetBuilder::check) {*/
    // checks if the depart lane was set before
    if(isDepart&&m_pDepartLane!=0)
      throw XMLDepartLaneDuplicationException();
/*  }*/
  m_pLaneStorage->push_back(lane);
  if(isDepart)
    m_pDepartLane = lane;
}

void 
NLEdgeControlBuilder::closeLanes() 
{
  m_pLanes = new MSEdge::LaneCont();
  m_pLanes->reserve(m_pLaneStorage->size());
  for(MSEdge::LaneCont::iterator i1=m_pLaneStorage->begin(); i1!=m_pLaneStorage->end(); i1++)
    m_pLanes->push_back(*i1);
  if(m_pLaneStorage->size()==1)
    m_iNoSingle++;
  else
    m_iNoMulti++;
  m_pLaneStorage->clear();
}

void 
NLEdgeControlBuilder::openAllowedEdge(MSEdge *edge) 
{
  m_pCurrentDestination = edge;
}

void 
NLEdgeControlBuilder::addAllowed(MSLane *lane) 
{
/*  if(NLNetBuilder::check) {*/
    // checks if the lane is inside the edge
    MSEdge::LaneCont::iterator i1 = find(m_pLanes->begin(), m_pLanes->end(), lane);
    if(i1==m_pLanes->end())
      throw XMLInvalidChildException();
/*  }*/
  m_pLaneStorage->push_back(lane);
}

void 
NLEdgeControlBuilder::closeAllowedEdge() 
{
  MSEdge::LaneCont *lanes = new MSEdge::LaneCont();
  lanes->reserve(m_pLaneStorage->size());
  for(MSEdge::LaneCont::iterator i1=m_pLaneStorage->begin(); i1!=m_pLaneStorage->end(); i1++)
    lanes->push_back(*i1);
  m_pLaneStorage->clear();
  (*m_pAllowedLanes)[m_pCurrentDestination] = lanes;
}

void 
NLEdgeControlBuilder::closeEdge() 
{
  m_pActiveEdge->initialize(m_pAllowedLanes, m_pDepartLane, m_pLanes);
}

MSEdgeControl *
NLEdgeControlBuilder::build() 
{
  MSEdgeControl::EdgeCont *singleLanes = new MSEdgeControl::EdgeCont();
  MSEdgeControl::EdgeCont *multiLanes = new MSEdgeControl::EdgeCont();
  singleLanes->reserve(m_iNoSingle);
  multiLanes->reserve(m_iNoMulti);
  for(EdgeCont::iterator i1=m_pEdges->begin(); i1!=m_pEdges->end(); i1++) {
    if((*i1)->nLanes()==1)
      singleLanes->push_back(*i1);
    else
      multiLanes->push_back(*i1);
  }
  return new MSEdgeControl("", singleLanes, multiLanes); // !!! ID
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLEdgeControlBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:




