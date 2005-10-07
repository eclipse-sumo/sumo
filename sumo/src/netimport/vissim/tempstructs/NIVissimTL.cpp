//---------------------------------------------------------------------------//
//                        NIVissimTL.cpp -  ccc
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.16  2005/10/07 11:40:10  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.15  2005/09/23 06:02:57  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.14  2005/04/27 12:24:37  dkrajzew
// level3 warnings removed; made netbuild-containers non-static
//
// Revision 1.13  2004/11/23 10:23:53  dkrajzew
// debugging
//
// Revision 1.12  2003/07/18 12:35:05  dkrajzew
// removed some warnings
//
// Revision 1.11  2003/07/07 08:28:48  dkrajzew
// adapted the importer to the new node type description; some further work
//
// Revision 1.10  2003/06/18 11:35:29  dkrajzew
// message subsystem changes applied and some further work done; seems to be stable but is not perfect, yet
//
// Revision 1.9  2003/06/16 08:01:57  dkrajzew
// further work on Vissim-import
//
// Revision 1.8  2003/06/05 11:46:57  dkrajzew
// class templates applied; documentation added
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


#include <map>
#include <string>
#include <cassert>
#include <utils/geom/GeomHelper.h>
#include <utils/geom/Boundary.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include "NIVissimConnection.h"
#include <netbuild/NBLoadedTLDef.h>
#include <netbuild/NBEdge.h>
#include <netbuild/NBEdgeCont.h>
#include <netbuild/NBTrafficLightLogicCont.h>
#include <netbuild/NBLoadedTLDef.h>
#include "NIVissimConnection.h"
#include "NIVissimDisturbance.h"
#include "NIVissimNodeDef.h"
#include "NIVissimEdge.h"
#include "NIVissimTL.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;


NIVissimTL::SignalDictType NIVissimTL::NIVissimTLSignal::myDict;

NIVissimTL::NIVissimTLSignal::NIVissimTLSignal(int lsaid, int id,
                                               const std::string &name,
                                               const IntVector &groupids,
                                               int edgeid,
                                               int laneno,
                                               SUMOReal position,
                                               const IntVector &vehicleTypes)
    : myLSA(lsaid), myID(id), myName(name), myGroupIDs(groupids),
    myEdgeID(edgeid), myLane(laneno), myPosition(position),
    myVehicleTypes(vehicleTypes)
{
}


NIVissimTL::NIVissimTLSignal::~NIVissimTLSignal()
{
}

bool
NIVissimTL::NIVissimTLSignal::isWithin(const Position2DVector &poly) const
{
    return poly.around(getPosition());
}


Position2D
NIVissimTL::NIVissimTLSignal::getPosition() const
{
    return NIVissimAbstractEdge::dictionary(myEdgeID)->getGeomPosition(myPosition);
}


bool
NIVissimTL::NIVissimTLSignal::dictionary(int lsaid, int id,
        NIVissimTL::NIVissimTLSignal *o)
{
    SignalDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        myDict[lsaid] = SSignalDictType();
        i = myDict.find(lsaid);
    }
    SSignalDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        myDict[lsaid][id] = o;
        return true;
    }
    return false;
}


NIVissimTL::NIVissimTLSignal*
NIVissimTL::NIVissimTLSignal::dictionary(int lsaid, int id)
{
    SignalDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        return 0;
    }
    SSignalDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        return 0;
    }
    return (*j).second;
}


void
NIVissimTL::NIVissimTLSignal::clearDict()
{
    for(SignalDictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        for(SSignalDictType::iterator j=(*i).second.begin(); j!=(*i).second.end(); j++) {
            delete (*j).second;
        }
    }
    myDict.clear();
}


NIVissimTL::SSignalDictType
NIVissimTL::NIVissimTLSignal::getSignalsFor(int tlid)
{
    SignalDictType::iterator i = myDict.find(tlid);
    if(i==myDict.end()) {
        return SSignalDictType();
    }
    return (*i).second;
}


bool
NIVissimTL::NIVissimTLSignal::addTo(NBEdgeCont &ec, NBLoadedTLDef *tl) const
{
    NIVissimConnection *c = NIVissimConnection::dictionary(myEdgeID);
    NBConnectionVector assignedConnections;
    if(c==0) {
        // What to do if on an edge? -> close all outgoing connections
        NBEdge *edge = ec.retrievePossiblySplitted(
            toString<int>(myEdgeID), myPosition);
        assert(edge!=0);
        // Check whether it is already known, which edges are approached
        //  by which lanes
        // check whether to use the original lanes only
        if(edge->lanesWereAssigned()) {
            const EdgeLaneVector *connections = edge->getEdgeLanesFromLane(myLane-1);
            for(EdgeLaneVector::const_iterator i=connections->begin(); i!=connections->end(); i++) {
                const EdgeLane &conn = *i;
                assert(myLane-1<(int)edge->getNoLanes());
                assignedConnections.push_back(
                    NBConnection(edge, myLane-1, conn.edge, conn.lane));
            }
        } else {
            WRITE_WARNING("Edge : Lanes were not assigned(!)");
            for(size_t j=0; j<edge->getNoLanes(); j++) {
                const EdgeLaneVector *connections = edge->getEdgeLanesFromLane(j);
                for(EdgeLaneVector::const_iterator i=connections->begin(); i!=connections->end(); i++) {
                    const EdgeLane &conn = *i;
                    assignedConnections.push_back(
                        NBConnection(edge, j, conn.edge, conn.lane));
                }
            }
        }
    } else {
        // get the edges
		NBEdge *tmpFrom = ec.retrievePossiblySplitted(
            toString<int>(c->getFromEdgeID()),
            toString<int>(c->getToEdgeID()),
            true);
		NBEdge *tmpTo = ec.retrievePossiblySplitted(
            toString<int>(c->getToEdgeID()),
            toString<int>(c->getFromEdgeID()),
            false);
        // check whether the edges are known
		if(tmpFrom!=0&&tmpTo!=0) {
            // add connections this signal is responsible for
            assignedConnections.push_back(NBConnection(tmpFrom, -1, tmpTo, -1));
		} else {
			return false;
			// !!! one of the edges could not be build
		}
    }
    // add to the group
    assert(myGroupIDs.size()!=0);
    if(myGroupIDs.size()==1) {
        return tl->addToSignalGroup(toString<int>(*(myGroupIDs.begin())),
            assignedConnections);
    } else {
        // !!!
        return tl->addToSignalGroup(toString<int>(*(myGroupIDs.begin())),
            assignedConnections);
    }
	return true;
}








NIVissimTL::GroupDictType NIVissimTL::NIVissimTLSignalGroup::myDict;

NIVissimTL::NIVissimTLSignalGroup::NIVissimTLSignalGroup(
        int lsaid, int id,
        const std::string &name,
        bool isGreenBegin, const DoubleVector &times,
        SUMOTime tredyellow, SUMOTime tyellow)
    : myLSA(lsaid), myID(id), myName(name), myTimes(times),
    myFirstIsRed(!isGreenBegin), myTRedYellow(tredyellow),
    myTYellow(tyellow)
{
}


NIVissimTL::NIVissimTLSignalGroup::~NIVissimTLSignalGroup()
{
}


bool
NIVissimTL::NIVissimTLSignalGroup::dictionary(int lsaid, int id,
        NIVissimTL::NIVissimTLSignalGroup *o)
{
    GroupDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        myDict[lsaid] = SGroupDictType();
        i = myDict.find(lsaid);
    }
    SGroupDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        myDict[lsaid][id] = o;
        return true;
    }
    return false;
/*
    GroupDictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
    */
}


NIVissimTL::NIVissimTLSignalGroup*
NIVissimTL::NIVissimTLSignalGroup::dictionary(int lsaid, int id)
{
    GroupDictType::iterator i = myDict.find(lsaid);
    if(i==myDict.end()) {
        return 0;
    }
    SGroupDictType::iterator j = (*i).second.find(id);
    if(j==(*i).second.end()) {
        return 0;
    }
    return (*j).second;
}

void
NIVissimTL::NIVissimTLSignalGroup::clearDict()
{
    for(GroupDictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        for(SGroupDictType::iterator j=(*i).second.begin(); j!=(*i).second.end(); j++) {
            delete (*j).second;
        }
    }
    myDict.clear();
}


NIVissimTL::SGroupDictType
NIVissimTL::NIVissimTLSignalGroup::getGroupsFor(int tlid)
{
    GroupDictType::iterator i = myDict.find(tlid);
    if(i==myDict.end()) {
        return SGroupDictType();
    }
    return (*i).second;
}


bool
NIVissimTL::NIVissimTLSignalGroup::addTo(NBLoadedTLDef *tl) const
{
    // get the color at the begin
    NBTrafficLightDefinition::TLColor color = myFirstIsRed
        ? NBTrafficLightDefinition::TLCOLOR_RED : NBTrafficLightDefinition::TLCOLOR_GREEN;
    string id = toString<int>(myID);
    tl->addSignalGroup(id); // !!! myTimes als SUMOTime
    for(DoubleVector::const_iterator i=myTimes.begin(); i!=myTimes.end(); i++) {
        tl->addSignalGroupPhaseBegin(id, (SUMOTime) *i, color);
        color = color==NBTrafficLightDefinition::TLCOLOR_RED
            ? NBTrafficLightDefinition::TLCOLOR_GREEN : NBTrafficLightDefinition::TLCOLOR_RED;
    }
	if(myTimes.size()==0) {
		if(myFirstIsRed) {
			tl->addSignalGroupPhaseBegin(id, 0, NBTrafficLightDefinition::TLCOLOR_RED);
		} else {
			tl->addSignalGroupPhaseBegin(id, 0, NBTrafficLightDefinition::TLCOLOR_GREEN);
		}
	}
    tl->setSignalYellowTimes(id, myTRedYellow, myTYellow);
	return true;
}








NIVissimTL::DictType NIVissimTL::myDict;

NIVissimTL::NIVissimTL(int id, const std::string &type,
                       const std::string &name, SUMOTime absdur,
                       SUMOTime offset)
    : myID(id), myName(name), myAbsDuration(absdur), myOffset(offset),
    myCurrentGroup(0), myType(type)

{
}


NIVissimTL::~NIVissimTL()
{
}





bool
NIVissimTL::dictionary(int id, const std::string &type,
                       const std::string &name, SUMOTime absdur,
                       SUMOTime offset)
{
    NIVissimTL *o = new NIVissimTL(id, type, name, absdur, offset);
    if(!dictionary(id, o)) {
        delete o;
        return false;
    }
    return true;
}

bool
NIVissimTL::dictionary(int id, NIVissimTL *o)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        myDict[id] = o;
        return true;
    }
    return false;
}


NIVissimTL *
NIVissimTL::dictionary(int id)
{
    DictType::iterator i=myDict.find(id);
    if(i==myDict.end()) {
        return 0;
    }
    return (*i).second;
}

/*
IntVector
NIVissimTL::getWithin(const AbstractPoly &poly, SUMOReal offset)
{
    IntVector ret;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        if((*i).second->crosses(poly, offset)) {
            ret.push_back((*i).second->myID);
        }
    }
    return ret;
}
*/
/*
void
NIVissimTL::computeBounding()
{
    Boundary *bound = new Boundary();
    SSignalDictType signals = NIVissimTLSignal::getSignalsFor(myID);
    for(SSignalDictType::const_iterator j=signals.begin(); j!=signals.end(); j++) {
        bound->add((*j).second->getPosition());
    }
    myBoundary = bound;
//    cout << "TL " << myID << ":" << *myBoundary << endl;
}
*/

void
NIVissimTL::clearDict()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}





bool
NIVissimTL::dict_SetSignals(NBTrafficLightLogicCont &tlc,
                            NBEdgeCont &ec)
{
	size_t ref = 0;
	size_t ref_groups = 0;
	size_t ref_signals = 0;
	size_t no_signals = 0;
	size_t no_groups = 0;
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        NIVissimTL *tl = (*i).second;
/*		if(tl->myType!="festzeit") {
			cout << " Warning: The traffic light '" << tl->myID
				<< "' could not be assigned to a node." << endl;
			ref++;
			continue;
		}*/
        string id = toString<int>(tl->myID);
        NBLoadedTLDef *def = new NBLoadedTLDef(id);
        if(!tlc.insert(id, def)) {
            MsgHandler::getErrorInstance()->inform("Error on adding a traffic light");
            MsgHandler::getErrorInstance()->inform(string(" Must be a multiple id ('") + id + string("')"));
            continue;
        }
		def->setCycleDuration((size_t) tl->myAbsDuration);
//        node->setType(NBTrafficLightDefinition::TYPE_SIMPLE_TRAFFIC_LIGHT);
        // add each group to the node's container
        SGroupDictType sgs = NIVissimTLSignalGroup::getGroupsFor(tl->getID());
        for(SGroupDictType::const_iterator j=sgs.begin(); j!=sgs.end(); j++) {
            if(!(*j).second->addTo(def)) {
                WRITE_WARNING(string("The signal group '") + toString<int>((*j).first)+ string("' could not be assigned to tl '")+ toString<int>(tl->myID) + string("'."));
				ref_groups++;
			}
			no_groups++;
        }
        // add the signal group signals to the node
        SSignalDictType signals = NIVissimTLSignal::getSignalsFor(tl->getID());
        for(SSignalDictType::const_iterator k=signals.begin(); k!=signals.end(); k++) {
            if(!(*k).second->addTo(ec, def)) {
                WRITE_WARNING(string("The signal '") + toString<int>((*k).first)+ string("' could not be assigned to tl '")+ toString<int>(tl->myID) + string("'."));
				ref_signals++;
			}
			no_signals++;
        }
    }
	if(ref!=0) {
        WRITE_WARNING(string("Could not set ") + toString<size_t>(ref)+ string(" of ") + toString<size_t>(myDict.size())+ string(" traffic lights."));
	}
	if(ref_groups!=0) {
        WRITE_WARNING(string("Could not set ") + toString<size_t>(ref_groups)+ string(" of ") + toString<size_t>(no_groups)+ string(" groups."));
	}
	if(ref_signals!=0) {
        WRITE_WARNING(string("Could not set ") + toString<size_t>(ref_signals)+ string(" of ") + toString<size_t>(no_signals)+ string(" signals."));
	}
    return true;

}


std::string
NIVissimTL::getType() const
{
    return myType;
}


int
NIVissimTL::getID() const
{
    return myID;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


