/***************************************************************************
                          MSRightOfWayJunction.cpp  -  Usual right-of-way 
                          junction.
                             -------------------
    begin                : Wed, 12 Dez 2001
    copyright            : (C) 2001 by Christian Roessel
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
// Revision 1.1  2002/04/08 07:21:23  traffic
// Initial revision
//
// Revision 2.3  2002/03/06 10:56:36  croessel
// Bugfix: myRespond will have always the correct size before being passed to myLogic.
//
// Revision 2.2  2002/02/27 13:47:57  croessel
// Additional assert's because of parameter-passing-problems.
//
// Revision 2.1  2002/02/21 18:49:45  croessel
// Deadlock-killer implemented.
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.5  2002/02/01 15:48:26  croessel
// Changed condition in moveFirstVehicles() again.
//
// Revision 1.4  2002/02/01 14:14:33  croessel
// Changed condition in moveFirstVehicles(). Now vehicles with a
// BrakeRequest only will also be moved.
//
// Revision 1.3  2002/02/01 11:52:28  croessel
// Removed function-adaptor findCompetitor from inside the class to the
// outside to please MSVC++.
//
// Revision 1.2  2002/02/01 11:40:34  croessel
// Changed return-type of some void methods used in for_each-loops to
// bool in order to please MSVC++.
//
// Revision 1.1  2001/12/13 15:54:49  croessel
// Initial commit. Has been MSJunction.cpp before.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSRightOfWayJunction.h"
#include "MSLane.h"
#include "MSJunctionLogic.h"
#include <algorithm>
#include <cassert>
#include <cmath>
 
using namespace std;

//-------------------------------------------------------------------------//

MSRightOfWayJunction::InLane::InLane(MSLane* lane) :
    myLane( lane ),
    myDriveRequest( false ),
    myBrakeRequest( false )
{
}

//-------------------------------------------------------------------------//

findCompetitor::result_type
findCompetitor::operator() 
    ( first_argument_type  competeLane,
      second_argument_type inOut ) const
{
    // ignore the prioritised vehicle's lane and empty competeLanes
    if (inOut.first == competeLane->myLane ||
        competeLane->myLane->empty()) {

        return false;
    }
    
    // Possible competitors are vehicles that have the same targetLane as 
    // the prioritised vehicle (driving in inOut direction) will have in the
    // following timestep.
    if ( competeLane->myLane->firstVehSuccLane() == inOut.second ) {
            
        return true;
    }
    
    return false;
}

//-------------------------------------------------------------------------//

MSRightOfWayJunction::MSRightOfWayJunction( string id, 
                                            InLaneCont* in, 
                                            MSJunctionLogic* logic) :
    MSLogicJunction( id ),
    myInLanes( in ),
    myLogic( logic )
{
    myRequest.resize( myLogic->nLinks(), false );
    myRespond.resize( myLogic->nInLanes(), false );
    // If sizes are different, something went wrong during
    // logic-calculation.
    assert( myLogic->nInLanes() == myInLanes->size() );
}


bool
MSRightOfWayJunction::clearRequests()
{
    myRequest.clear();
    myRespond.clear();
    // Resize respond because it will be passed by reference to myLogic.
    // Instead of resizing myLogic could push_back into myRespond, but I
    // want the size be controlled the owner-class of myRespond.
    myRespond.resize( myInLanes->size(), false );
        
    for ( InLaneCont::iterator it = myInLanes->begin();
          it != myInLanes->end(); ++it ) {
        ( *it )->myLane->clearRequest();
    }
    return true;
}

//-------------------------------------------------------------------------//

MSRightOfWayJunction::~MSRightOfWayJunction()
{
}

//-------------------------------------------------------------------------//

bool
MSRightOfWayJunction::setFirstVehiclesRequests()
{
    for ( InLaneCont::iterator it = myInLanes->begin();
          it != myInLanes->end(); ++it ) {
        ( *it )->myLane->setRequest();
    }
    return true;
}

//-------------------------------------------------------------------------//

bool
MSRightOfWayJunction::moveFirstVehicles()
{
    collectRequests(); // Sets myRequest.

    // Get myRespond from logic and check for deadlocks.
    assert( myRespond.size() == myInLanes->size() );
    myLogic->respond( myRequest, myRespond );
    assert( myRespond.size() == myInLanes->size() );
    deadlockKiller();

    moveVehicles();
    return true;
}

//-------------------------------------------------------------------------//

bool
MSRightOfWayJunction::vehicles2targetLane()
{
    for ( InLaneCont::iterator it = myInLanes->begin();
          it != myInLanes->end(); ++it ) {
        ( *it )->myLane->integrateNewVehicle();
    }
    return true;    
}

//-------------------------------------------------------------------------//

void
MSRightOfWayJunction::collectRequests()
{
    for ( InLaneCont::iterator in = myInLanes->begin();
          in != myInLanes->end(); ++in) {
        
        // Get req from lane.
        DriveBrakeRequest dbr = ( *in )->myLane->request();
        
        // Copy link-dependant part to myRequest.
        myRequest.insert( myRequest.end(),
                          dbr.myRequest.begin(),
                          dbr.myRequest.end() );
        
        // Store lane-dependant part to myInLanes.
        ( *in )->myDriveRequest = dbr.myDriveRequest;
        ( *in )->myBrakeRequest = dbr.myBrakeRequest;
    }
}

//-------------------------------------------------------------------------//
     
void
MSRightOfWayJunction::moveVehicles()
{
    // Look for Brake-Request-Conflicts
    brakeReqConfl();
    
    for ( InLaneCont::iterator in = myInLanes->begin();
          in != myInLanes->end(); ++in) {

        // If driveRequest was set, move dependent upon respond, else
        // if brakeRequest was set, move because vehicle has right of way.
        // Vehicles without requests moved already.
        if ( ( *in )->myDriveRequest == true ) {
            
            unsigned int index = distance( myInLanes->begin(), in );
            bool respond = myRespond[ index ];
            ( *in )->myLane->moveFirst( respond );
        }
        else if ( ( *in )->myBrakeRequest == true ) {
            
            ( *in )->myLane->moveFirst( true );
        }
    }
}

//-------------------------------------------------------------------------//

void
MSRightOfWayJunction::brakeReqConfl()
{
    // Search for brake-request-conflicts and modify respond, if neccessary.
    
    for ( InLaneCont::iterator in = myInLanes->begin();
          in != myInLanes->end(); ++in) {
        
        // Check for a possible brake-request-conflict.
        if ( ( *in )->myDriveRequest == false &&
             ( *in )->myBrakeRequest == true) {
            
            // in's that satisfy the above condition are prioritised lanes.
            // Because they didn't set a drive-request, their respond is 
            // false. But of course they are allowed to drive. So, change
            // the respond.
            InLaneCont::iterator& prioLane = in; 
            ( *prioLane )->myDriveRequest = true; // Must be true, otherwise 
                                                  // no move.            
            unsigned int prioIndex = distance( myInLanes->begin(), prioLane );
            myRespond[ prioIndex ]   = true;            
                                                
            // Search conflicting vehicle, i.e. a vehicle on a yield lane that 
            // is allowed to drive onto the lane, the prioritised vehicle will
            // reach in the following timestep.
            const MSLane* outLane =
                ( *prioLane )->myLane->requestLane().firstVehSuccLane( 
                    ( *prioLane )->myLane );      
                       
            pair< const MSLane*, const MSLane* > inOut( 
                (*prioLane )->myLane, outLane );
            
            // Search as long in myInLanes until competitor is found or 
            // myInLanes is exhausted.
            InLaneCont::iterator start = myInLanes->begin();
            InLaneCont::iterator competeIt;
            unsigned int competeIndex;
            for (;;) {
            
                competeIt = find_if( start, myInLanes->end(),
                                     bind2nd( findCompetitor(), inOut ) );
                
                // Return if there is no conflicting vehicle.
                if ( competeIt ==  myInLanes->end() ) {
                
                    return;
                }
                
                // Exit loop if competing vehicle is allowed to drive. 
                competeIndex = distance( myInLanes->begin(), competeIt );    
                if ( myRespond[ competeIndex ] == true ) {
                
                    break;
                }
                
                // Continue loop
                start = competeIt + 1;
            }   
            
            // Check if prioritised vehicle doesn't allow competitor to enter 
            // it's desired lane.                            
            if ( ! drivePermit( ( *prioLane )->myLane, outLane, 
                                ( *competeIt )->myLane ) ) {                 
                                
                // Prioritised vehicle gave no permission, so competitor has 
                // to slow down.
                myRespond[ competeIndex ] = false;                    
            }      
            else {
                
                // Prioritised vehicle gave permission, it's desired speed
                // was adapted in MSLane::decel2much, called from drivePermit.
            }
        }
    } // for all inLanes
}

//-------------------------------------------------------------------------//

bool
MSRightOfWayJunction::drivePermit( const MSLane* prio, const MSLane* out, 
                                   const MSLane* compete ) 
{
    // TODO
    // Allow the conflicting vehicle to drive under some condition:    
    // Main vehicle is allowed to decelerate maxSpeedReduce*decFactor,

    // 0 <= decFactor <= 1
    // Adjust this to your purposes. Maybe random?
    float decFactor( 0 ); // No deceleration allowed, first try value.

    
    // ask the prio's lane request lane about allowed deceleration.
    return ! prio->requestLane().decel2much( compete, out, decFactor );
}

//-------------------------------------------------------------------------//


void 
MSRightOfWayJunction::deadlockKiller()
{
    // Check for a deadlock condition ( Request != 0 but respond == 0 )
    vector< bool > nullRequest = vector< bool >( myRequest.size(), false );
    if ( myRequest == nullRequest ) {

        return;
    }

    vector< bool > nullRespond = vector< bool >( myRespond.size(), false );
    if ( myRespond == nullRespond ) {

        // Handle deadlock: Create randomly a deadlock-free request out of 
        // myRequest, i.e. a "single bit" request. Then again, send it 
        // through myLogic (this is neccessary because we don't have a 
        // mapping between requests and lanes.)
        vector< unsigned > trueRequests;
        trueRequests.reserve( myRespond.size() );
        for ( unsigned i = 0; i < myRequest.size(); ++i ) {

            if ( myRequest[i] == true ) {

                trueRequests.push_back( i );
                assert( trueRequests.size() <= myRespond.size() );
            }
        }
        // Choose randamly an index out of [0,trueRequests.size()];
        unsigned noLockIndex = static_cast< unsigned > (
            floor( static_cast< double >( rand() ) /
                   static_cast< double >( RAND_MAX ) * 
                   trueRequests.size() 
                ) 
            );

        // Create deadlock-free request.
        vector< bool > noLockRequest = 
            vector< bool >( myRequest.size(), false );
        noLockRequest[ trueRequests[ noLockIndex ] ] = true;
        
        // Calculate respond with deadlock-free request.
        myLogic->respond( noLockRequest, myRespond );
    }
    return;
}                         

//-------------------------------------------------------------------------//

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSRightOfWayJunction.icc"
//#endif

// Local Variables:
// mode:C++
// End:




