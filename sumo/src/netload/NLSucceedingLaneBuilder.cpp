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
// Revision 1.7  2003/12/04 13:18:23  dkrajzew
// handling of internal links added
//
// Revision 1.6  2003/09/24 13:29:39  dkrajzew
// retrival of lanes by the position within the bitset added
//
// Revision 1.5  2003/09/05 15:20:19  dkrajzew
// loading of internal links added
//
// Revision 1.4  2003/07/30 09:25:17  dkrajzew
// loading of directions and priorities of links implemented
//
// Revision 1.3  2003/06/05 11:52:27  dkrajzew
// class templates applied; documentation added
//
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
#include <microsim/MSTrafficLightLogic.h>
#include "NLNetBuilder.h"
#include "NLSucceedingLaneBuilder.h"
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/options/OptionsSubSys.h>
#include <utils/options/OptionsCont.h>


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
NLSucceedingLaneBuilder::addSuccLane(bool yield, const string &laneId,
                                     const std::string &viaID,
                                     MSLink::LinkDirection dir,
                                     MSLink::LinkState state,
                                     bool internalEnd,
                                     const std::string &tlid, size_t linkNo)
{
    // check whether the link is a dead link
    if(laneId=="SUMO_NO_DESTINATION") {
        // build the dead link and add it to the container
        m_SuccLanes->push_back(new MSLink(0, 0, 0, dir, state, false));
        return;
    }
    // get the lane the link belongs to
    MSLane *lane = MSLane::dictionary(laneId);
    if(lane==0) {
        throw XMLIdNotKnownException("lane", laneId);
    }
    MSLane *via = 0;
    if(viaID!="" && OptionsSubSys::getOptions().getBool("use-internal-links")) {
        via = MSLane::dictionary(viaID);
        if(via==0) {
            throw XMLIdNotKnownException("lane", viaID);
        }
    }
    // check whether this link is controlled by a traffic light
    MSTrafficLightLogic *logic = 0;
    if(tlid!="") {
        logic = MSTrafficLightLogic::dictionary(tlid);
        if(logic==0) {
            throw XMLIdNotKnownException("tl-logic", tlid);
        }
    }
    // build the link
        // if internal lanes are used, the next lane of a normal edge
        // will be an internal lane
/*    if(via!=0&&OptionsSubSys::getOptions().getBool("use-internal-links")) {
        lane = via;
    } else {
        via = 0;
    }*/
    MSLink *link = new MSLink(lane, via, yield, dir, state, internalEnd);
    // if a traffic light is responsible for it, inform the traffic light
    if(logic!=0) {
        MSLane *current = MSLane::dictionary(m_CurrentLane);
        if(current==0) {
            throw XMLIdNotKnownException("lane", m_CurrentLane);
        }
        logic->addLink(link, current, linkNo);
    }
    // add the link to the container
    m_SuccLanes->push_back(link);
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
