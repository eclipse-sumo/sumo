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
// Revision 1.2  2003/02/07 11:18:56  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 15:36:50  dkrajzew
// moved from ROOT/sumo/netload to ROOT/src/netload; new format definition parseable in one step
//
// Revision 1.5  2002/06/11 14:39:27  dkrajzew
// windows eol removed
//
// Revision 1.4  2002/06/11 13:44:33  dkrajzew
// Windows eol removed
//
// Revision 1.3  2002/06/07 14:39:59  dkrajzew
// errors occured while building larger nets and adaption of new netconverting methods debugged
//
// Revision 1.2  2002/04/15 07:07:56  dkrajzew
// new loading paradigm implemented
//
// Revision 1.1.1.1  2002/04/08 07:21:24  traffic
// new project name
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
#include <microsim/MSLane.h>
#include <microsim/MSLink.h>
#include <microsim/MSLinkCont.h>
#include "NLNetBuilder.h"
#include "NLSucceedingLaneBuilder.h"
#include <utils/xml/XMLBuildingExceptions.h>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
NLSucceedingLaneBuilder::NLSucceedingLaneBuilder()
    //: m_Junction(0)
{
    m_SuccLanes = new MSLinkCont();
    m_SuccLanes->reserve(10);
}

NLSucceedingLaneBuilder::~NLSucceedingLaneBuilder()
{
    delete m_SuccLanes;
}

void
NLSucceedingLaneBuilder::openSuccLane(const string &laneId)
{
    m_CurrentLane = laneId;
}

void
NLSucceedingLaneBuilder::addSuccLane(bool yield, const string &laneId)
{
    if(laneId=="SUMO_NO_DESTINATION") {
        m_SuccLanes->push_back(new MSLink(0, 0));
    } else {
        MSLane *lane = MSLane::dictionary(laneId);
        if(lane==0) {
            throw XMLIdNotKnownException("lane", laneId);
        }
        m_SuccLanes->push_back(new MSLink(lane, yield));
    }
}

void
NLSucceedingLaneBuilder::closeSuccLane()
{
    MSLane *current = MSLane::dictionary(m_CurrentLane);
    if(current==0) {
        throw XMLIdNotKnownException("lane", m_CurrentLane);
    }
    MSLinkCont *cont = new MSLinkCont();
    cont->reserve(m_SuccLanes->size());
    copy(m_SuccLanes->begin(), m_SuccLanes->end(), back_inserter(*cont));
    current->initialize(/*m_Junction, */cont);
    m_SuccLanes->clear();
//    m_Junction = 0;
}

std::string
NLSucceedingLaneBuilder::getSuccingLaneName() const
{
    return m_CurrentLane;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "NLSucceedingLaneBuilder.icc"
//#endif

// Local Variables:
// mode:C++
// End:
