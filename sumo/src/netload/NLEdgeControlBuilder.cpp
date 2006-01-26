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
// Revision 1.17  2006/01/26 08:43:23  dkrajzew
// patched MSEdge in order to work with a generic router
//
// Revision 1.16  2005/11/09 06:42:54  dkrajzew
// TLS-API: MSEdgeContinuations added
//
// Revision 1.15  2005/10/17 09:20:12  dkrajzew
// segfaults on loading broken configs patched
//
// Revision 1.14  2005/10/10 12:11:33  dkrajzew
// debugging
//
// Revision 1.13  2005/10/07 11:41:49  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.12  2005/09/23 06:04:11  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.11  2005/09/15 12:04:36  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.10  2005/07/12 12:37:15  dkrajzew
// code style adapted
//
// Revision 1.9  2005/05/04 08:40:16  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; added the possibility to load lane shapes into the non-gui simulation
//
// Revision 1.8  2004/07/02 09:37:53  dkrajzew
// lanes now get a numerical id (for online-routing)
//
// Revision 1.7  2003/11/18 14:23:57  dkrajzew
// debugged and completed lane merging detectors
//
// Revision 1.6  2003/09/05 15:20:19  dkrajzew
// loading of internal links added
//
// Revision 1.5  2003/06/18 11:18:05  dkrajzew
// new message and error processing: output to user may be a message,
//  warning or an error now; it is reported to a Singleton (MsgHandler);
//  this handler puts it further to output instances.
//  changes: no verbose-parameter needed; messages are exported to singleton
//
// Revision 1.4  2003/06/16 14:43:34  dkrajzew
// documentation added
//
// Revision 1.3  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.2  2002/10/17 10:33:29  dkrajzew
// error-handling instead of pure assertions added
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload;
//  new format definition parseable in one step
//
// Revision 1.6  2002/06/11 14:39:26  dkrajzew
// windows eol removed
//
// Revision 1.5  2002/06/11 13:44:32  dkrajzew
// Windows eol removed
//
// Revision 1.4  2002/06/07 14:39:58  dkrajzew
// errors occured while building larger nets and adaption of new
//  netconverting methods debugged
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
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <microsim/MSLane.h>
#include <microsim/MSSourceLane.h>
#include <microsim/MSInternalLane.h>
#include <microsim/MSEdge.h>
#include <microsim/MSEdgeControl.h>
#include <utils/common/MsgHandler.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include "NLBuilder.h"
#include "NLEdgeControlBuilder.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLEdgeControlBuilder::NLEdgeControlBuilder(unsigned int storageSize)
    : myCurrentNumericalLaneID(0), myCurrentNumericalEdgeID(0), m_pEdges(0)
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
    delete m_pEdges;
}


void
NLEdgeControlBuilder::prepare(unsigned int no)
{
    m_pEdges = new EdgeCont();
    m_pEdges->reserve(no);
}


MSEdge *
NLEdgeControlBuilder::addEdge(const string &id)
{
    MSEdge *edge = new MSEdge(id, myCurrentNumericalEdgeID++);
    if(!MSEdge::dictionary(id, edge)) {
        throw XMLIdAlreadyUsedException("Edge", id);
    }
    m_pEdges->push_back(edge);
    return edge;
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
NLEdgeControlBuilder::addLane(/*MSNet &net, */const std::string &id,
                              SUMOReal maxSpeed, SUMOReal length, bool isDepart,
							  const Position2DVector &shape)
{
    // checks if the depart lane was set before
    if(isDepart&&m_pDepartLane!=0) {
      throw XMLDepartLaneDuplicationException();
    }
    MSLane *lane = 0;
    switch(m_Function) {
    case MSEdge::EDGEFUNCTION_SOURCE:
        lane = new MSSourceLane(/*net, */id, maxSpeed, length, m_pActiveEdge,
            myCurrentNumericalLaneID++, shape);
        break;
    case MSEdge::EDGEFUNCTION_INTERNAL:
        lane = new MSInternalLane(/*net, */id, maxSpeed, length, m_pActiveEdge,
            myCurrentNumericalLaneID++, shape);
        break;
    case MSEdge::EDGEFUNCTION_NORMAL:
    case MSEdge::EDGEFUNCTION_SINK:
        lane = new MSLane(/*net, */id, maxSpeed, length, m_pActiveEdge,
            myCurrentNumericalLaneID++, shape);
        break;
    default:
        throw 1;
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
    copy(m_pLaneStorage->begin(), m_pLaneStorage->end(),
        back_inserter(*m_pLanes));
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


MSEdge *
NLEdgeControlBuilder::closeEdge()
{
    if(m_pAllowedLanes==0 || /*m_pDepartLane==0 ||*/ m_pLanes==0) {
        MsgHandler::getErrorInstance()->inform(
            "Something is corrupt within the definition of lanes for the edge '"
            + m_pActiveEdge->getID() + "'.");
        return 0;
    }
    m_pActiveEdge->initialize(m_pAllowedLanes, m_pDepartLane,
        m_pLanes, m_Function);
    return m_pActiveEdge;
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
    return new MSEdgeControl(singleLanes, multiLanes);
}

MSEdge *
NLEdgeControlBuilder::getActiveEdge() const
{
    return m_pActiveEdge;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:




