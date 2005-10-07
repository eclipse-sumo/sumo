/***************************************************************************
                          MSEdgeControl.cpp  -  Coordinates Edge
                          operations.
                             -------------------
    begin                : Mon, 09 Apr 2001
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
// Revision 1.11  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.10  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.9  2005/05/04 08:24:24  dkrajzew
// level 3 warnings removed; a certain SUMOTime time description added; speed-ups by checked emission and avoiding looping over all edges
//
// Revision 1.8  2005/02/01 10:10:40  dkrajzew
// got rid of MSNet::Time
//
// Revision 1.7  2004/08/02 12:08:39  dkrajzew
// raw-output extracted; output device handling rechecked
//
// Revision 1.6  2004/07/02 09:55:13  dkrajzew
// MeanData refactored (moved to microsim/output)
//
// Revision 1.5  2003/12/04 13:30:41  dkrajzew
// work on internal lanes
//
// Revision 1.4  2003/07/16 15:28:00  dkrajzew
// MSEmitControl now only simulates lanes which do have vehicles; the edges
//  do not go through the lanes, the EdgeControl does
//
// Revision 1.3  2003/02/07 10:41:50  dkrajzew
// updated
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
// Revision 2.1  2002/03/13 16:56:34  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.0  2002/02/14 14:43:14  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.8  2002/02/05 13:51:51  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.7  2002/02/05 11:43:09  croessel
// Lane-change implemented.
//
// Revision 1.6  2001/12/19 16:56:27  croessel
// Copy-ctor and assignment-operator removed.
//
// Revision 1.5  2001/12/12 18:21:33  croessel
// in operator<<: MSVC++ shortcoming fixed.
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
// Revision 1.1  2001/10/24 07:02:10  traffic
// new extension
//
// Revision 1.7  2001/10/23 09:29:40  traffic
// parser bugs removed
//
// Revision 1.5  2001/09/06 15:31:25  croessel
// Added operator<< to class MSEdgeControl for simple text output.
//
// Revision 1.4  2001/08/16 13:07:21  traffic
// minor MSVC++-problems solved
//
// Revision 1.3  2001/07/25 12:16:36  traffic
// CC problems with make_pair repaired
//
// Revision 1.2  2001/07/16 12:55:46  croessel
// Changed id type from unsigned int to string. Added string-pointer
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
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

#include "MSEdgeControl.h"
#include "MSEdge.h"
#include "MSLane.h"
#include <iostream>
#include <vector>

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
//MSEdgeControl::DictType MSEdgeControl::myDict;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSEdgeControl::MSEdgeControl()
{
}


MSEdgeControl::MSEdgeControl(/*string id, */EdgeCont* singleLane,
                             EdgeCont *multiLane)
    : //myID(id),
    mySingleLaneEdges(singleLane),
    myMultiLaneEdges(multiLane),
    myLanes(MSLane::dictSize())
{
    // build the usage defintions for lanes
        // for lanes with no neighbors
    size_t pos = 0;
    EdgeCont::iterator i;
    for(i=singleLane->begin(); i!=singleLane->end(); i++) {
        MSEdge::LaneCont *lanes = (*i)->getLanes();
        myLanes[pos].lane = *(lanes->begin());
        myLanes[pos].noVehicles = 0;
        myLanes[pos].firstNeigh = lanes->end();
        myLanes[pos].lastNeigh = lanes->end();
        pos++;
    }
//    myFirstMultiPos = pos;
        // for lanes with neighbors
    for(i=multiLane->begin(); i!=multiLane->end(); i++) {
        MSEdge::LaneCont *lanes = (*i)->getLanes();
        for(MSEdge::LaneCont::iterator j=lanes->begin(); j!=lanes->end(); j++) {
            myLanes[pos].lane = *j;
            myLanes[pos].noVehicles = 0;
            myLanes[pos].firstNeigh = (j+1);
            myLanes[pos].lastNeigh = lanes->end();
            pos++;
        }
    }
    // assign lane usage definitions to lanes
    for(size_t j=0; j<pos; j++) {
        myLanes[j].lane->init(*this, &(myLanes[j]));
    }
}


MSEdgeControl::~MSEdgeControl()
{
    delete mySingleLaneEdges;
    delete myMultiLaneEdges;
}


void
MSEdgeControl::moveNonCritical()
{
    LaneUsageVector::iterator i;
    // reset the approaching vehicle distance, first
    for(i=myLanes.begin(); i!=myLanes.end(); i++) {
        (*i).lane->resetApproacherDistance();
    }
    // move non-critical vehicles
    for(i=myLanes.begin(); i!=myLanes.end(); i++) {
        if((*i).noVehicles!=0) {
            (*i).lane->moveNonCritical();
        }
    }
}


void
MSEdgeControl::moveCritical()
{
    for(LaneUsageVector::iterator i=myLanes.begin(); i!=myLanes.end(); i++) {
        if((*i).noVehicles!=0) {
            (*i).lane->moveCritical(/*(*i).firstNeigh, (*i).lastNeigh*/);
        }
    }
}


void
MSEdgeControl::moveFirst()
{
    LaneUsageVector::iterator i;
    for(i=myLanes.begin(); i!=myLanes.end(); i++) {
        if((*i).noVehicles!=0) {
            (*i).lane->setCritical();
        }
    }
    for(i=myLanes.begin(); i!=myLanes.end(); i++) {
        (*i).lane->integrateNewVehicle();
    }
}

void
MSEdgeControl::changeLanes()
{
    for ( EdgeCont::iterator edge = myMultiLaneEdges->begin();
          edge != myMultiLaneEdges->end(); ++edge ) {
        assert((*edge)->getLanes()->size()>1);
        ( *edge )->changeLanes();
    }
}


void
MSEdgeControl::detectCollisions( SUMOTime timestep )
{
    LaneUsageVector::iterator i;
//    EdgeCont::iterator edge;
    // Detections is made by the edge's lanes, therefore hand over.
    for (i = myLanes.begin(); i != myLanes.end(); ++i) {
        if((*i).noVehicles>1) {
            (*i).lane->detectCollisions( timestep );
        }
    }
    /*
    for (i = myMultiLaneEdges->begin();
         i != myMultiLaneEdges->end(); ++i) {
        if((*i).noVehicles>1) {
            (*i)->detectCollisions( timestep );
        }
    }
    */
}

/*
bool
MSEdgeControl::dictionary(string id, MSEdgeControl* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}


MSEdgeControl*
MSEdgeControl::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}
*/
/*
void
MSEdgeControl::clear()
{
    for(DictType::iterator i=myDict.begin(); i!=myDict.end(); i++) {
        delete (*i).second;
    }
    myDict.clear();
}
*/
/*
ostream&
operator<<( ostream& os, const MSEdgeControl& ec )
{
    os << "MSEdgeControll: ID = " << ec.myID << endl;
    return os;
}
*/

void
MSEdgeControl::insertMeanData(unsigned int number)
{
    LaneUsageVector::iterator i;
    for(i=myLanes.begin(); i!=myLanes.end(); i++) {
        (*i).lane->insertMeanData(number);
    }
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getSingleLaneEdges() const
{
    return *mySingleLaneEdges;
}


const MSEdgeControl::EdgeCont &
MSEdgeControl::getMultiLaneEdges() const
{
    return *myMultiLaneEdges;
}


void
MSEdgeControl::saveState(std::ostream &os, long what)
{
}


void
MSEdgeControl::loadState(BinaryInputDevice &bis, long what)
{
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
