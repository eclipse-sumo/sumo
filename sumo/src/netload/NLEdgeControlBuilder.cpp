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
// Revision 1.3  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 10:33:29  dkrajzew
// error-handling instead of pure assertions added
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.6  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
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
#include <microsim/MSLane.h>
#include <microsim/MSSourceLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/SErrorHandler.h>
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
NLEdgeControlBuilder::addEdge(const string &id)
{
    MSEdge *edge = new MSEdge(id);
    if(!MSEdge::dictionary(id, edge)) {
        throw XMLIdAlreadyUsedException("Edge", id);
    }
    m_pEdges->push_back(edge);
}

void
NLEdgeControlBuilder::chooseEdge(const string &id,
                                 MSEdge::EdgeBasicFunction function)
{
    m_pActiveEdge = MSEdge::dictionary(id);
    if(m_pActiveEdge==0) {
        throw XMLIdNotKnownException("edge", id);
    }
    m_pDepartLane = (MSLane*) 0;
    m_pAllowedLanes = new MSEdge::AllowedLanesCont();
    m_Function = function;
}


MSLane *
NLEdgeControlBuilder::addLane(MSNet &net, const std::string &id,
                              double maxSpeed, double length, bool isDepart)
{
    // checks if the depart lane was set before
    if(isDepart&&m_pDepartLane!=0) {
      throw XMLDepartLaneDuplicationException();
    }
    MSLane *lane = 0;
    if(m_Function==MSEdge::EDGEFUNCTION_SOURCE) {
        lane = new MSSourceLane(net, id, maxSpeed, length, m_pActiveEdge);
    } else {
        lane = new MSLane(net, id, maxSpeed, length, m_pActiveEdge);
    }
    m_pLaneStorage->push_back(lane);
    if(isDepart) {
        m_pDepartLane = lane;
    }
    return lane;
}

void
NLEdgeControlBuilder::closeLanes()
{
    m_pLanes = new MSEdge::LaneCont();
    m_pLanes->reserve(m_pLaneStorage->size());
    for(MSEdge::LaneCont::iterator i1=m_pLaneStorage->begin(); i1!=m_pLaneStorage->end(); i1++) {
        m_pLanes->push_back(*i1);
    }
    if(m_pLaneStorage->size()==1) {
        m_iNoSingle++;
    } else {
        m_iNoMulti++;
    }
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
    // checks if the lane is inside the edge
    MSEdge::LaneCont::iterator i1 = find(m_pLanes->begin(), m_pLanes->end(), lane);
    if(i1==m_pLanes->end()) {
        throw XMLInvalidChildException();
    }
    m_pLaneStorage->push_back(lane);
}

void
NLEdgeControlBuilder::closeAllowedEdge()
{
    MSEdge::LaneCont *lanes = new MSEdge::LaneCont();
    lanes->reserve(m_pLaneStorage->size());
    for(MSEdge::LaneCont::iterator i1=m_pLaneStorage->begin(); i1!=m_pLaneStorage->end(); i1++) {
        lanes->push_back(*i1);
    }
    m_pLaneStorage->clear();
    (*m_pAllowedLanes)[m_pCurrentDestination] = lanes;
}

void
NLEdgeControlBuilder::closeEdge()
{
    if(m_pAllowedLanes==0 || m_pDepartLane==0 || m_pLanes==0) {
        SErrorHandler::add(
            string("Something is corrupt with the definition of lanes for the edge '")
            + m_pActiveEdge->id() + string("'."));
        return;
    }
    m_pActiveEdge->initialize(m_pAllowedLanes, m_pDepartLane,
        m_pLanes, m_Function);
}

MSEdgeControl *
NLEdgeControlBuilder::build()
{
    MSEdgeControl::EdgeCont *singleLanes = new MSEdgeControl::EdgeCont();
    MSEdgeControl::EdgeCont *multiLanes = new MSEdgeControl::EdgeCont();
    singleLanes->reserve(m_iNoSingle);
    multiLanes->reserve(m_iNoMulti);
    for(EdgeCont::iterator i1=m_pEdges->begin(); i1!=m_pEdges->end(); i1++) {
        if((*i1)->nLanes()==1) {
            singleLanes->push_back(*i1);
        } else {
            multiLanes->push_back(*i1);
        }
    }
    delete m_pEdges;
    return new MSEdgeControl("", singleLanes, multiLanes); // !!! ID
}



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLEdgeControlBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:




