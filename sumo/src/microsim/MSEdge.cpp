/***************************************************************************
                          MSEdge.C  -  Provides routing. Superior to Lanes.
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSEdge.h"
#include "MSLane.h"
#include "MSNet.h"
#include "MSLaneChanger.h"
#include <algorithm>
#include <iostream>
#include <cassert>

using namespace std;

// Init static member.
MSEdge::DictType MSEdge::myDict;


MSEdge::MSEdge(string id) : myID(id), myLaneChanger(0)
{
}


MSEdge::~MSEdge()
{
    delete myLaneChanger;
    for(AllowedLanesCont::iterator i1=myAllowed->begin(); i1!=myAllowed->end(); i1++) {
        delete (*i1).second;
    }
    delete myAllowed;
    delete myLanes;
    // Remark: Lanes are delete using MSLane::clear();
}


void
MSEdge::initialize(AllowedLanesCont* allowed, MSLane* departLane,
                   LaneCont* lanes)
{
    assert(allowed!=0);
    assert(departLane!=0);
    assert(lanes!=0);

    myAllowed = allowed;
    myDepartLane = departLane;
    myLanes = lanes;

    if ( myLanes->size() > 1 ) {

        myLaneChanger = new MSLaneChanger( myLanes );
    }
}


void
MSEdge::moveExceptFirstSingle()
{
    (*(myLanes->begin()))->moveExceptFirst();
}


void
MSEdge::moveExceptFirstMulti()
{
    // The ordering of the lanes is essential, because
    // (right-hand-traffic) vehicles are not allowed to overtake on
    // the right. So update from the right to the left.
    for (LaneCont::iterator lane = myLanes->begin();
         lane != myLanes->end() - 1; ++lane) {
        (*lane)->moveExceptFirst(lane+1, myLanes->end());
    }
    // The last one has no neighbour.
    (*(myLanes->end() - 1))->moveExceptFirst();
}


void
MSEdge::detectCollisions( MSNet::Time timestep )
{
    // Ask lanes about collisions.
    for (LaneCont::iterator lane = myLanes->begin();
         lane != myLanes->end() - 1; ++lane) {
        (*lane)->detectCollisions( timestep );
    }
}


const MSEdge::LaneCont*
MSEdge::allowedLanes(const MSEdge& destination) const
{
    AllowedLanesCont::const_iterator it =
        myAllowed->find(&destination);
    if (it != myAllowed->end()) {
        return it->second;
    }
    else {
        return 0; // Destination-edge not found.
    }
}


MSLane&
MSEdge::departLane() const
{
    return *myDepartLane;
}


MSLane*
MSEdge::leftLane(const MSLane* lane) const
{
    LaneCont::iterator laneIt = find(myLanes->begin(), myLanes->end(),
                                     lane);
    return (laneIt != myLanes->end()) ? *laneIt : 0;
}


bool
MSEdge::dictionary(string id, MSEdge* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
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
    assert( myLaneChanger != 0 );
    myLaneChanger->laneChange();
}


MSEdge::XMLOut::XMLOut( const MSEdge& obj,
                               unsigned indentWidth,
                               bool withChildElemes ) :
    myObj( obj ),
    myIndentWidth( indentWidth ),
    myWithChildElemes( withChildElemes )
{
}


ostream&
operator<<( ostream& os, const MSEdge::XMLOut& obj )
{
    string indent( obj.myIndentWidth , ' ' );
    os << indent << "<edge id=\"" << obj.myObj.myID << "\">" << endl;
    if ( obj.myWithChildElemes ) {
        for ( MSEdge::LaneCont::const_iterator lane =
              obj.myObj.myLanes->begin();
              lane != obj.myObj.myLanes->end(); ++lane) {

            os << MSLane::XMLOut( **lane, obj.myIndentWidth + 4, true );
        }
    }
    os << indent << "</edge>" << endl;
    return os;   
}


MSEdge::MeanData::MeanData( const MSEdge& obj,
                            unsigned index,
                            MSNet::Time interval ) :
    myObj( obj ),                           
    myIndex( index ),
    myInterval( interval )
{
}


ostream&
operator<<( ostream& os, const MSEdge::MeanData& obj )
{
    os << "<edge id=\"" << obj.myObj.myID << "\">\n";
    for ( MSEdge::LaneCont::const_iterator lane = 
              obj.myObj.myLanes->begin();
          lane != obj.myObj.myLanes->end(); ++lane) {
        
        os << MSLane::MeanData( **lane, obj.myIndex, obj.myInterval );
    }

    os << "</edge>\n";
    return os;
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSEdge.icc"
//#endif

// Local Variables:
// mode:C++
// End:












