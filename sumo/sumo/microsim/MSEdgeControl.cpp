/***************************************************************************
                          MSEdgeControl.C  -  Coordinates Edge
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSEdgeControl.h"
#include "MSEdge.h"
#include "MSNet.h"
#include "MSPerson.h"
#include <iostream>
#include <vector>

using namespace std;

// Init static member.
MSEdgeControl::DictType MSEdgeControl::myDict;


MSEdgeControl::MSEdgeControl()
{
}


MSEdgeControl::MSEdgeControl(string id, EdgeCont* singleLane, EdgeCont*
                             multiLane) : myID(id),
                                          mySingleLaneEdges(singleLane),
                                          myMultiLaneEdges(multiLane)
{
}


MSEdgeControl::~MSEdgeControl()
{
}


void
MSEdgeControl::moveExceptFirst()
{
    EdgeCont::iterator edge;
    // Move vehicles on lanes but hand command
    // over to the real lanes. 
    for (edge = mySingleLaneEdges->begin();
         edge != mySingleLaneEdges->end(); ++edge) {
        (*edge)->moveExceptFirstSingle();
    }
    for (edge = myMultiLaneEdges->begin();
         edge != myMultiLaneEdges->end(); ++edge) {
        (*edge)->moveExceptFirstMulti();
    }
}


void
MSEdgeControl::changeLanes()
{
    for ( EdgeCont::iterator edge = myMultiLaneEdges->begin();
          edge != myMultiLaneEdges->end(); ++edge ) {
        
          ( *edge )->changeLanes();
    }
}


void
MSEdgeControl::detectCollisions( MSNet::Time timestep )
{
    EdgeCont::iterator edge;
    // Detections is made by the edge's lanes, therefore hand over. 
    for (edge = mySingleLaneEdges->begin();
         edge != mySingleLaneEdges->end(); ++edge) {
        (*edge)->detectCollisions( timestep );
    }
    for (edge = myMultiLaneEdges->begin();
         edge != myMultiLaneEdges->end(); ++edge) {
        (*edge)->detectCollisions( timestep );
    }
}


void 
MSEdgeControl::loadPersons() 
{
    EdgeCont::iterator edge;
    // Detections is made by the edge's lanes, therefore hand over. 
    for (edge = mySingleLaneEdges->begin();
         edge != mySingleLaneEdges->end(); ++edge) {
        (*edge)->loadPersons();
    }
    for (edge = myMultiLaneEdges->begin();
         edge != myMultiLaneEdges->end(); ++edge) {
        (*edge)->loadPersons();
    }
}


void 
MSEdgeControl::unloadPersons(MSNet *net, unsigned int time) 
{
    EdgeCont::iterator edge;
    // Detections is made by the edge's lanes, therefore hand over. 
    for (edge = mySingleLaneEdges->begin();
         edge != mySingleLaneEdges->end(); ++edge) {
        (*edge)->unloadPersons(net, time);
    }
    for (edge = myMultiLaneEdges->begin();
         edge != myMultiLaneEdges->end(); ++edge) {
        (*edge)->unloadPersons(net, time);
    }
}


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

ostream&
operator<<( ostream& os, const MSEdgeControl& ec )
{
    os << "MSEdgeControll: ID = " << ec.myID << endl;
    return os;
}


MSEdgeControl::XMLOut::XMLOut( const MSEdgeControl& obj,
                               unsigned indentWidth ) :
    myObj( obj ),                           
    myIndentWidth( indentWidth )
{
}


ostream&
operator<<( ostream& os, const MSEdgeControl::XMLOut& obj )
{
    for ( MSEdgeControl::EdgeCont::iterator edg1 =
          obj.myObj.mySingleLaneEdges->begin();
          edg1 != obj.myObj.mySingleLaneEdges->end(); ++edg1 ) {
         
        os << MSEdge::XMLOut( **edg1, obj.myIndentWidth, true );
    } 
    for ( MSEdgeControl::EdgeCont::iterator edg2 =
          obj.myObj.myMultiLaneEdges->begin();
          edg2 != obj.myObj.myMultiLaneEdges->end(); ++edg2 ) {
          
        os << MSEdge::XMLOut( **edg2, obj.myIndentWidth, true );
    }
    return os;       
}


MSEdgeControl::MeanData::MeanData( const MSEdgeControl& obj,
                                   unsigned index, MSNet::Time interval ) :
    myObj( obj ),                           
    myIndex( index ),
    myInterval( interval )
{
}


ostream&
operator<<( ostream& os, const MSEdgeControl::MeanData& obj )
{
    for ( MSEdgeControl::EdgeCont::iterator edg1 =
          obj.myObj.mySingleLaneEdges->begin();
          edg1 != obj.myObj.mySingleLaneEdges->end(); ++edg1 ) {
         
        os << MSEdge::MeanData( **edg1, obj.myIndex, obj.myInterval );
    } 
    for ( MSEdgeControl::EdgeCont::iterator edg2 =
          obj.myObj.myMultiLaneEdges->begin();
          edg2 != obj.myObj.myMultiLaneEdges->end(); ++edg2 ) {
          
        os << MSEdge::MeanData( **edg2, obj.myIndex, obj.myInterval );
    }
    return os;       
}

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSEdgeControl.iC"
//#endif

// Local Variables:
// mode:C++
// End:



