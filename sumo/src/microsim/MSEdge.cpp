/***************************************************************************
                          MSEdge.cpp  -  Provides routing. Superior to Lanes.
                             -------------------
    begin                : Tue, 06 Mar 2001
    copyright            : (C) 2001 by ZAIK http://www.zaik.uni-koeln.de/AFS
    author               : Christian Roessel
    email                : roessel@zpr.uni-koeln.de
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
    const char rcsid[] =
    "$Id$";
}

// $Log$
// Revision 1.28  2006/10/12 10:14:27  dkrajzew
// synchronized with internal CVS (mainly the documentation has changed)
//
// Revision 1.27  2006/09/18 10:05:34  dkrajzew
// added vehicle class support to microsim
//
// Revision 1.26  2006/07/06 07:33:22  dkrajzew
// rertrieval-methods have the "get" prependix; EmitControl has no dictionary; MSVehicle is completely scheduled by MSVehicleControl; new lanechanging algorithm
//
// Revision 1.25  2006/03/28 06:20:43  dkrajzew
// removed the unneded slow lane changer
//
// Revision 1.24  2006/01/26 08:30:29  dkrajzew
// patched MSEdge in order to work with a generic router
//
// Revision 1.23  2005/11/09 06:39:38  dkrajzew
// usage of internal lanes is now optional at building
//
// Revision 1.22  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.21  2005/09/22 13:45:50  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.20  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.19  2005/07/13 10:22:46  dkrajzew
// debugging
//
// Revision 1.18  2005/07/12 12:21:57  dkrajzew
// debugging vehicle emission
//
// Revision 1.17  2005/05/04 08:23:52  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; speed-ups by checked emission and avoiding looping over all edges
//
// Revision 1.16  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.15  2004/11/23 10:20:09  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.14  2004/08/02 12:08:39  dkrajzew
// raw-output extracted; output device handling rechecked
//
// Revision 1.13  2004/07/02 09:55:13  dkrajzew
// MeanData refactored (moved to microsim/output)
//
// Revision 1.12  2004/02/16 14:19:51  dkrajzew
// getLane should be const
//
// Revision 1.11  2004/01/27 10:36:28  dkrajzew
// patched some linux-warnings
//
// Revision 1.10  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.9  2003/09/05 15:07:53  dkrajzew
// tried to improve the building/deletion usage
//
// Revision 1.8  2003/08/20 11:40:57  dkrajzew
// option for suppressing output of empty edges within the raw-output added
//
// Revision 1.7  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles; the edges do not go through the lanes, the EdgeControl does
//
// Revision 1.6  2003/06/04 16:12:05  roessel
// Added methods getEdgeVector and getLanes needed by MSTravelcostDetector.
//
// Revision 1.5  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.4  2002/10/28 12:58:01  dkrajzew
// some minor output changes (intending and backslash-n replaced by endl)
//
// Revision 1.3  2002/10/17 10:41:12  dkrajzew
// retrival of the id added
//
// Revision 1.2  2002/10/16 16:40:35  dkrajzew
// usage of MSPerson removed; will be reimplemented later
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.3  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.2  2002/04/24 13:06:47  croessel
// Changed signature of void detectCollisions() to void detectCollisions(
// MSNet::Time )
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.3  2002/03/13 16:56:34  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.2  2002/02/21 18:53:38  croessel
// #include <cassert> added.
//
// Revision 2.1  2002/02/14 15:06:05  croessel
// Fixed "there is no need for a lane-changer in single-lane edges".
//
// Revision 2.0  2002/02/14 14:43:13  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.7  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.6  2002/02/05 11:42:35  croessel
// Lane-change implemented.
//
// Revision 1.5  2001/12/19 16:50:12  croessel
// Copy-ctor and assignment-op removed.
//
// Revision 1.4  2001/11/15 17:12:12  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not
// needed.
//
// Revision 1.3  2001/11/14 15:47:33  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:06  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:01:07  traffic
// new extension
//
// Revision 1.8  2001/10/23 09:29:18  traffic
// parser bugs removed
//
// Revision 1.6  2001/09/06 15:29:55  croessel
// Added operator<< to class MSEdge for simple text output.
//
// Revision 1.5  2001/07/25 12:16:28  traffic
// CC problems with make_pair repaired
//
// Revision 1.4  2001/07/18 09:40:45  croessel
// Added public method nLanes() that returns the edge's number of lanes.
//
// Revision 1.3  2001/07/16 12:55:46  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.2  2001/07/13 17:03:34  croessel
// Just a test.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//
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

#include "MSEdge.h"
#include "MSLane.h"
#include "MSLaneChanger.h"
#include "MSGlobals.h"
#include <algorithm>
#include <iostream>
#include <cassert>
#include "MSVehicle.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * static member definitions
 * ======================================================================= */
MSEdge::DictType MSEdge::myDict;
std::vector<MSEdge*> MSEdge::myEdges;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSEdge::MSEdge(const std::string &id, size_t numericalID)
    : myID(id), myLanes(0), myAllowed(0), myLaneChanger(0),
    myLastFailedEmissionTime(-1), myNumericalID(numericalID)
{
}


MSEdge::~MSEdge()
{
    delete myLaneChanger;
    if(myAllowed!=0) {
        for(AllowedLanesCont::iterator i1=myAllowed->begin(); i1!=myAllowed->end(); i1++) {
            delete (*i1).second;
        }
        ClassedAllowedLanesCont::iterator i2;
        for(i2=myClassedAllowed.begin(); i2!=myClassedAllowed.end(); i2++) {
            for(AllowedLanesCont::iterator i1=(*i2).second.begin(); i1!=(*i2).second.end(); i1++) {
                delete (*i1).second;
            }
        }
        for(i2=myClassedNotAllowed.begin(); i2!=myClassedNotAllowed.end(); i2++) {
            for(AllowedLanesCont::iterator i1=(*i2).second.begin(); i1!=(*i2).second.end(); i1++) {
                delete (*i1).second;
            }
        }
    }
    delete myAllowed;
    delete myLanes;
    // Remark: Lanes are delete using MSLane::clear();
}


void
MSEdge::initialize(AllowedLanesCont* allowed, MSLane* departLane,
                   LaneCont* lanes, EdgeBasicFunction function)
{
    assert(allowed!=0);
//!!!!    assert(departLane!=0);
    assert(lanes!=0);

    myAllowed = allowed;
    myDepartLane = departLane;
    myLanes = lanes;
    _function = function;

    if ( myLanes->size() > 1 && function!=EDGEFUNCTION_INTERNAL ) {
        myLaneChanger = new MSLaneChanger( myLanes );
    }

    // build the classed allowed lanes
    myHaveClassConstraints = false;
    if(myAllowed!=0) {
        // build list of vehicle classes that are constrained
        // ... all others will be not regarded (allowed) ...
        std::vector<SUMOVehicleClass> vclasses;
        LaneCont::const_iterator i2;
        for(i2=myLanes->begin(); i2!=myLanes->end(); ++i2) {
            std::vector<SUMOVehicleClass>::const_iterator j;
            const std::vector<SUMOVehicleClass> &allowed = (*i2)->getAllowedClasses();
            for(j=allowed.begin(); j!=allowed.end(); j++) {
                if(find(vclasses.begin(), vclasses.end(), *j)==vclasses.end()) {
                    vclasses.push_back(*j);
                }
            }
            const std::vector<SUMOVehicleClass> &disallowed = (*i2)->getNotAllowedClasses();
            for(j=disallowed.begin(); j!=disallowed.end(); j++) {
                if(find(vclasses.begin(), vclasses.end(), *j)==vclasses.end()) {
                    vclasses.push_back(*j);
                }
            }
        }
        // go through these classes
        std::vector<SUMOVehicleClass>::const_iterator j;
        for(j=vclasses.begin(); j!=vclasses.end(); ++j) {
            // copy the possibilities first
            // - ok, this is still not the complete truth as a lane may be used
            //   differently for different vehicles classes, should be
            //   revisited one day
            AllowedLanesCont nallowed;
            AllowedLanesCont::iterator i1;
            for(i1=myAllowed->begin(); i1!=myAllowed->end(); ++i1) {
                LaneCont *nc = new LaneCont(*((*i1).second));
                nallowed[(*i1).first] = nc;
            }
            // now go through the lists of lanes
            // remove a lane if
            // a) this vehicle class is disallowed on this lane
            bool isAllowedSomewhere = false;
            bool wasRemoved = false;
            for(i1=nallowed.begin(); i1!=nallowed.end(); ++i1) {
                for(LaneCont::iterator i2=(*i1).second->begin(); i2!=(*i1).second->end(); ) {
                    // ... for later actions ...
                    const std::vector<SUMOVehicleClass> &allowed = (*i2)->getAllowedClasses();
                    if(find(allowed.begin(), allowed.end(), *j)!=allowed.end()) {
                        isAllowedSomewhere = true;
                    }
                    // remove in to remove
        	        const std::vector<SUMOVehicleClass> &disallowed = (*i2)->getNotAllowedClasses();
                    if(find(disallowed.begin(), disallowed.end(), *j)!=disallowed.end()) {
                        wasRemoved = true;
                        assert(find((*i1).second->begin(), (*i1).second->end(), (*i2))!=(*i1).second->end());
                        i2 = (*i1).second->erase(find((*i1).second->begin(), (*i1).second->end(), (*i2)));
                    } else {
                        ++i2;
                    }
                }
            }
            // b) is allowed on all other but not on this
            if(isAllowedSomewhere) {
                for(i1=nallowed.begin(); i1!=nallowed.end(); ++i1) {
                    for(LaneCont::iterator i2=(*i1).second->begin(); i2!=(*i1).second->end(); ) {
                        // ... for later actions ...
                        const std::vector<SUMOVehicleClass> &allowed = (*i2)->getAllowedClasses();
                        if(find(allowed.begin(), allowed.end(), *j)==allowed.end()) {
                            i2 = (*i1).second->erase(find((*i1).second->begin(), (*i1).second->end(), (*i2)));
                        } else {
                            ++i2;
                        }
                    }
                }
            }
            myClassedAllowed[*j] = nallowed;
            myHaveClassConstraints = true;
        }
    }
}


const MSEdge::LaneCont*
MSEdge::allowedLanes(const MSEdge& destination, SUMOVehicleClass vclass) const
{
	if(!myHaveClassConstraints||vclass==SVC_UNKNOWN||myClassedAllowed.find(vclass)==myClassedAllowed.end()) {
        AllowedLanesCont::const_iterator it = myAllowed->find(&destination);
        if(it!=myAllowed->end()) {
            return it->second;
        } else {
            return 0; // Destination-edge not found.
        }
    }
    // ok, we have vehicle class based prohibitions
    AllowedLanesCont::const_iterator it;
    if(myClassedAllowed.find(vclass)!=myClassedAllowed.end()) {
        it = myClassedAllowed.find(vclass)->second.find(&destination);
        if(it==myAllowed->end()) {
            it = myAllowed->find(&destination);
        }
    }
    if(it==myAllowed->end()/*||it->second->size()==0*/) { // it->second->size()==0 !!! clean up earlier
        return 0;
    }
    // !!! missing: what happens to prohibited classes?
    return it->second;
}


MSLane*
MSEdge::leftLane(const MSLane* lane) const
{
    LaneCont::iterator laneIt = find(myLanes->begin(), myLanes->end(), lane);
    if(laneIt==myLanes->end()||laneIt==myLanes->end()-1) {
        return 0;
    }
    return *(laneIt+1);
}


MSLane*
MSEdge::rightLane(const MSLane* lane) const
{
    LaneCont::iterator laneIt = find(myLanes->begin(), myLanes->end(), lane);
    if(laneIt==myLanes->end()||laneIt==myLanes->begin()) {
        return 0;
    }
    return *(laneIt-1);
}


bool
MSEdge::dictionary(string id, MSEdge* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        while(myEdges.size()<ptr->getNumericalID()+1) {
            myEdges.push_back(0);
        }
        myEdges[ptr->getNumericalID()] = ptr;
        return true;
    }
    return false;
}


MSEdge*
MSEdge::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}


MSEdge*
MSEdge::dictionary(size_t id)
{
    assert(myEdges.size()>id);
    return myEdges[id];
}


size_t
MSEdge::dictSize()
{
    return myDict.size();
}


void
MSEdge::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}


unsigned int
MSEdge::nLanes() const
{
    return myLanes->size();
}


ostream&
operator<<(ostream& os, const MSEdge& edge)
{
    os << "MSEdge: Id = " << edge.myID << endl;
    return os;
}


void
MSEdge::changeLanes()
{
    if(_function==EDGEFUNCTION_INTERNAL) {
        return;
    }
    assert( myLaneChanger != 0 );
    myLaneChanger->laneChange();
}


const std::string &
MSEdge::getID() const
{
    return myID;
}


MSEdge::EdgeBasicFunction
MSEdge::getPurpose() const
{
    return _function;
}


bool
MSEdge::isSource() const
{
    return _function==EDGEFUNCTION_SOURCE;
}


bool
MSEdge::emit(MSVehicle &v, SUMOTime time) const
{
    if(_function!=EDGEFUNCTION_SOURCE) {
        return myDepartLane->emit(v);
    } else {
        const LaneCont &lanes =  v.departLanes();
        int minI = 0;
        int ir = 0;
        unsigned int noCars = (unsigned int) (*getLanes())[0]->length();
        {
            for(LaneCont::const_iterator i=lanes.begin(); i!=lanes.end(); i++, ir++) {
                if((*i)->getVehicleNumber()<noCars) {
                    minI = ir;
                    noCars = (*i)->getVehicleNumber();
                }
            }
        }
        if(lanes[minI]->emit(v)) {
            return true;
        } else {
            ir = 0;
            for(LaneCont::const_iterator i=lanes.begin(); i!=lanes.end(); i++, ir++) {
                if(ir!=minI&&(*i)->emit(v)) {
                    return true;
                }
            }
        }

        return false;
    }
}


vector< MSEdge* >
MSEdge::getEdgeVector( void )
{
    vector< MSEdge* > edges;
    edges.reserve( myDict.size() );
    for ( DictType::iterator edge = myDict.begin(); edge != myDict.end();
          ++edge ) {
        edges.push_back( edge->second );
    }
    return edges;
}


const MSEdge::LaneCont * const
MSEdge::getLanes( void ) const
{
    return myLanes;
}


#ifdef HAVE_INTERNAL_LANES
const MSEdge *
MSEdge::getInternalFollowingEdge(MSEdge *followerAfterInternal) const
{
    //@ to be optimized
    for(LaneCont::const_iterator i=myLanes->begin(); i!=myLanes->end(); i++) {
        MSLane *l = *i;
        const MSLinkCont &lc = l->getLinkCont();
        for(MSLinkCont::const_iterator j=lc.begin(); j!=lc.end(); j++) {
            MSLink *link = *j;
            if(link->getLane()->getEdge()==followerAfterInternal) {
                return link->getViaLane()->getEdge();
            }
        }
    }
    return 0;
}
#endif

SUMOTime
MSEdge::getLastFailedEmissionTime() const
{
    return myLastFailedEmissionTime;
}


void
MSEdge::setLastFailedEmissionTime(SUMOTime time) const
{
    myLastFailedEmissionTime = time;
}


std::vector<MSEdge *>
MSEdge::getFollowingEdges() const
{
    std::vector<MSEdge*> ret;
    for(AllowedLanesCont::iterator i=myAllowed->begin(); i!=myAllowed->end(); ++i) {
        ret.push_back((MSEdge*) (*i).first);
    }
    return ret;
}


std::vector<MSEdge*>
MSEdge::getIncomingEdges() const
{
    std::vector<MSEdge*> ret;
    for ( DictType::iterator edge = myDict.begin(); edge != myDict.end();
          ++edge ) {

        const MSEdge::LaneCont *allowed = (*edge).second->allowedLanes(*this, SVC_UNKNOWN);
        if(allowed!=0) {
            ret.push_back( edge->second );
        }
    }
    return ret;
}


SUMOReal
MSEdge::getEffort(SUMOTime time) const
{
    return (*myLanes)[0]->length() / (*myLanes)[0]->maxSpeed();
}


// add a new vehicle into the container
void
MSEdge::addEquippedVehicle(string id, MSVehicle *vehicle) const
{

	map<std::string, MSVehicle *>::const_iterator cur  = myEquippedVeh.find(id);
	if(cur==myEquippedVeh.end()){
         myEquippedVeh[id]=vehicle;
	}


}

//remove a vehicle with this id from the container
void
MSEdge::removeEquippedVehicle(string id) const
{
	map<std::string, MSVehicle *>::iterator cur  = myEquippedVeh.find(id);
	if(cur!=myEquippedVeh.end()){
         myEquippedVeh.erase(cur);
	}
}


const MSEdge::DictTypeVeh &
MSEdge::getEquippedVehs() const
{
    return myEquippedVeh;
}


void
MSEdge::addNeighborEdge(std::string id, MSEdge *edge)
{
    std::map<std::string, MSEdge*>::iterator i = neighborEdges.find(id);
	if(i== neighborEdges.end()){
		neighborEdges[id]=edge;
	}

}


const std::map<std::string, MSEdge*> &
MSEdge::getNeighborEdges() const
{
	return neighborEdges;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
