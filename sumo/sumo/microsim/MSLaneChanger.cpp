/***************************************************************************
                          MSLaneChanger.cpp  -  Handles lane-changes within 
                          the edge's lanes.
                             -------------------
    begin                : Fri, 01 Feb 2002
    copyright            : (C) 2002 by Christian Roessel
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
// Revision 2.2  2002/03/21 14:00:06  croessel
// Bug-fix in change(): Vehicles that keep to their lane must also be
// inserted in the tmpVehicles-container.
// Error-prone assert() in candiOnAllowed removed.
//
// Revision 2.1  2002/03/21 11:31:53  croessel
// Changed onAllowed to candiOnAllowed. Now the changeCandidate checks
// if a given lane is suitable for him. In change2right/left it is
// checked, that the target is a valid lane.
//
// Revision 2.0  2002/02/14 14:43:17  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.2  2002/02/13 10:31:46  croessel
// Modified the lane-changing conditions by using
// MSVehicleType::maxLength() and MSVehicleType::minDecel() in cases,
// where the change-candidate has no successor or predecessor.
//
// Revision 1.1  2002/02/05 11:53:02  croessel
// Initial commit.
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSLaneChanger.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cmath>

using namespace std;

//-------------------------------------------------------------------------//

MSLaneChanger::~MSLaneChanger()
{

}

//-------------------------------------------------------------------------//

MSLaneChanger::MSLaneChanger( MSEdge::LaneCont* lanes )
{
    assert( lanes->size() > 1 );
    
    // Fill the changer with the lane-data.
    myChanger.reserve( lanes->size() );
    for ( MSEdge::LaneCont::iterator lane = lanes->begin();
          lane != lanes->end(); ++lane ) {
        ChangeElem ce;
        ce.follow = 0;
        ce.lead   = 0;
        ce.lane   = *lane;
        ce.veh    = ( *lane )->myVehicles.begin();
        myChanger.push_back( ce );
    }   
}

//-------------------------------------------------------------------------//

void 
MSLaneChanger::laneChange()
{
    // This is what happens in one timestep. After initialization of the
    // changer, each vehicle will try to change. After that the changer
    // nedds an update to prevent multiple changes of one vehicle.
    // Finally, the change-result has to be given back to the lanes.
    initChanger();
    while( vehInChanger() ) {

        change();
        updateChanger();
    }
    
    updateLanes();
}

//-------------------------------------------------------------------------//

void 
MSLaneChanger::initChanger()
{
    // Prepare myChanger with a safe state.
    for ( ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce ) {

        ce->follow = 0;
        
        MSLane::VehCont& vehicles = ce->lane->myVehicles;
        if ( vehicles.empty() ) {
            
            ce->veh  = vehicles.end();
            ce->lead = 0;
            continue;
        }
        ce->veh  = vehicles.begin(); 
        if ( vehicles.size() == 1 ) {
            
            ce->lead = 0;
            continue;
        }
        ce->lead = *( vehicles.begin() + 1 );
    }
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::vehInChanger()
{
    // If there is at least one valid vehicle under the veh's in myChanger
    // return true.
    for ( ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce ) {

        if ( veh( ce ) != 0 ) {

            return true;
        }
    }
    return false;
}

//-------------------------------------------------------------------------//

MSVehicle* 
MSLaneChanger::veh( ChangerIt ce )
{
    // If ce has a valid vehicle, return it. Otherwise return 0.
    if ( ce->veh != ce->lane->myVehicles.end() ) {

        return *( ce->veh );
    }
    return 0;
}

//-------------------------------------------------------------------------//

void 
MSLaneChanger::change()
{
    // Find change-candidate. If it is on an allowed lane, try to change 
    // to the right (there is a rule in Germany that you have to change
    // to the right, unless you are overtaking). If change to the right 
    // isn't possible, check if there is a possibility to overtake (on the
    // left. 
    // If candidate isn't on an allowed lane, changing to an allowed has
    // priority.
    myCandi = findCandidate();
    if ( candiOnAllowed( myCandi ) ) {

        if ( change2right() ) {
            
            ( myCandi - 1 )->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            return;
        }
        if ( change2left() ) {

            ( myCandi + 1 )->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            return;
        }
    }
    else { // not on allowed
        
        ChangerIt target = findTarget();
        if ( change2target( target ) ) {
            
            target->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            return;
        }
    }
    
    // Candidate didn't change lane.
    myCandi->lane->myTmpVehicles.push_back( veh ( myCandi ) );
    return;
}

//-------------------------------------------------------------------------//

void 
MSLaneChanger::updateChanger()
{
    assert(  myCandi->veh != myCandi->lane->myVehicles.end() );

    // "Push" the vehicles to the back, i.e. follower becomes vehicle,
    // vehicle becomes leader, and leader becomes predecessor of vehicle,
    // if it exists.
    myCandi->follow = veh( myCandi );
    myCandi->veh    = myCandi->veh + 1;

    if ( veh( myCandi ) == 0 ) {

        assert( myCandi->lead == 0 );
        // leader already 0.
        return;
    }
    if ( myCandi->veh + 1 == myCandi->lane->myVehicles.end() ) {

        myCandi->lead = 0;
    }
    else {
        
        myCandi->lead = *( myCandi->veh + 1 ) ;
    }
    return;
}

//-------------------------------------------------------------------------//

void 
MSLaneChanger::updateLanes()
{

    // Update the lane's vehicle-container. 
    // First: it is bad style to change other classes members, but for
    // this release, other attempts were too time-consuming. In a next
    // release we will change from this lane-centered design to a vehicle-
    // centered. This will solve many problems.
    // Second: this swap would be faster if vehicle-containers would have
    // been pointers, but then I had to change too much of the MSLane code.
    for ( ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce ) {

        ce->lane->myVehicles.swap( ce->lane->myTmpVehicles );
        ce->lane->myTmpVehicles.clear();
    }
}

//-------------------------------------------------------------------------//

MSLaneChanger::ChangerIt 
MSLaneChanger::findCandidate()
{
    // Find the vehicle in myChanger with the smallest position. If there
    // is no vehicle in myChanger (shouldn't happen) , return
    // myChanger.end().
    ChangerIt min = myChanger.end();
    
    for ( ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce ) {

        if ( veh( ce ) == 0 ) {
            
            continue;
        }
        if ( min == myChanger.end() ) {
            
            min = ce;
            continue;
        }

        assert( veh( ce )  != 0 );
        assert( veh( min ) != 0 );
        if ( veh( min )->pos() > veh( ce )->pos() ) {
            
            min = ce;
        }
    }
    assert( min != myChanger.end() );
    assert( veh( min ) != 0 );    
    return min;
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::candiOnAllowed( ChangerIt target )
{
    assert( veh( myCandi ) != 0 );
    return veh( myCandi )->onAllowed( target->lane );
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::change2right()
{
    // Try to change to the right-lane if there is one. If this lane isn't 
    // an allowed one, cancel the try. Otherwise, check some conditions. If 
    // they are simultaniously fulfilled, a change is possible.
    if ( myCandi == myChanger.begin() ) {
    
        return false;    
    }
    ChangerIt target = myCandi - 1;
    if ( ! candiOnAllowed( target ) ) {

        return false;
    }
    if ( 

        ! overlap( target )     &&
        ! congested( target )   &&
          safeChange( target )  &&
        ! predInteraction()     &&
          advan2right() 

        ) {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::change2left()
{
    // Try to change to the left-lane, if there is one. If this lane isn't 
    // an allowed one, cancel the try. Otherwise, check some conditions.
    // If they are simultaniously fulfilled, a change is possible.
    ChangerIt target = myCandi + 1;
    if ( target == myChanger.end() ||
         ! candiOnAllowed( target ) ) {

        return false;
    }
    if ( 

        ! overlap( target )     &&
        ! congested( target )   &&
          safeChange( target )  &&
          advan2left() 

        ) {
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------//

MSLaneChanger::ChangerIt 
MSLaneChanger::findTarget()
{
    // If candidate is on a not allowed lane, it has to search for a 
    // neighboured lane that will bring it closer to an allowed one.
    // Certainly there is a more elegant way than looping over all lanes.
    ChangerIt nearestTarget = myChanger.end();
    unsigned int minDist    = myChanger.size();

    for ( ChangerIt ce = myChanger.begin(); ce != myChanger.end(); ++ce ) {

        if ( ce == myCandi ) {
            
            continue;
        }
        if ( candiOnAllowed( ce ) ) {
            
            unsigned int dist = abs( distance( myCandi, ce ) );
            assert( dist > 0 );
            assert( dist < myChanger.size() );
            if ( dist < minDist ) {
                
                minDist       = dist;
                nearestTarget = ce;
            }
        }
    }
    assert( nearestTarget != myChanger.end() );
    
    if ( distance( myCandi, nearestTarget ) > 0 ) {

        assert( myCandi + 1 != myChanger.end() );
        return myCandi + 1;
    }
    assert( myCandi != myChanger.begin() );
    return myCandi - 1;
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::change2target( ChangerIt target )
{
    // We are changing to an allowed lane. This change has to be a safe
    // one. Other conditions are irrelevant.
    if ( 
         ! overlap( target ) &&
           safeChange( target )
        ) {

        return true;
    }
    return false;
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::overlap( ChangerIt target )
{
    // If the candidate overlaps one potential neighbour on the target
    // lane, lane-change is forbidden.
    MSVehicle* vehicle     = veh( myCandi );
    MSVehicle* neighLead   = veh( target );
    MSVehicle* neighFollow = target->follow;

    if ( neighFollow != 0 ) {
        
        if ( MSVehicle::overlap( vehicle, neighFollow ) ) {

            return true;
        }
    }
    if ( neighLead != 0 ) {

        if ( MSVehicle::overlap( vehicle, neighLead ) ) {

            return true;
        }
    }
    
    // Check for a neighLead on the succeeding lane.
    if ( target->lane->length() - vehicle->pos() > 
         MSVehicleType::maxLength() ) {
         
         return false;
    }
    return true;
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::congested( ChangerIt target )
{
    // Congested situation are relevant only on highways (maxSpeed > 70km/h)
    // and congested on German Highways means that the vehicles have speeds
    // below 60km/h. Overtaking on the right is allowed then.
    if ( ( myCandi->lane->maxSpeed() <= 70.0 / 3.6 ) ||
         (  target->lane->maxSpeed() <= 70.0 / 3.6 ) ) {
        
        return false;
    }
    if ( veh( myCandi )->congested() &&
         veh( target  )->congested() ) {
        
        return true;
    }
    return false;
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::safeChange( ChangerIt target )
{
    // Check if candidate's change to target-lane will be safe, i.e. is there
    // enough back-gap to the neighFollow to drive collision-free (if there is
    // no neighFollow, keep a safe-gap to the beginning of the lane) and is
    // there enough gap for the candidate to neighLead to drive collision-
    // free (if there is no neighLead, be sure that candidate is able to slow-
    // down towards the lane end).
    MSVehicle* vehicle     = veh( myCandi );
    MSVehicle* neighLead   = veh( target );
    MSVehicle* neighFollow = target->follow;
    MSLane* targetLane     = target->lane;    

    if ( neighFollow == 0 ) { // Empty target, no neighLead, no neighFollow.

        // Check back gap.
        float safeBackGap = pow( targetLane->maxSpeed(), 2 ) / 
                            ( 2 * MSVehicleType::minDecel() ) + 
                            MSVehicle::tau() + vehicle->length();
        if ( vehicle->pos() > safeBackGap ) {

            // Check front gap.
            return ( targetLane->length() - vehicle->pos() > 
                     vehicle->brakeGap( targetLane ) );
        }
        return false;
    }
    if ( neighLead == 0 ) { // there is a neighFollow, but no neighLead.
        
        // Check back gap.
        float gap2follow = vehicle->pos() - vehicle->length() -
                           neighFollow->pos();
        if ( gap2follow > neighFollow->safeGap( *vehicle ) ) {

            // Check front gap.
            return ( targetLane->length() -
                     MSVehicleType::maxLength() -
                     vehicle->pos() > 
                     vehicle->brakeGap( targetLane ) );            
        }
        return false;
    }
    // NeighLead and neighFollow on target lane.
    // Check back gap.
    float gap2follow = vehicle->pos() - vehicle->length() - 
                       neighFollow->pos();
    if ( gap2follow > neighFollow->safeGap( *vehicle ) ) {

        // Check front gap.
        float gap2lead = neighLead->pos() - neighLead->length() -
                         vehicle->pos();
        return ( gap2lead > vehicle->safeGap( *neighLead ) );
    }
    return false;
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::predInteraction()
{
    // No predecessor interaction is one of the change2right conditions.
    // Interaction means that vehicle's next state is affected by the prede-
    // cessor rather than by vehicle's accelerating abilities and maxSpeed.
    MSVehicle* vehicle = veh( myCandi );
    MSVehicle* pred    = myCandi->lead;
    MSLane*    lane    = myCandi->lane;  

    // Is there interaction with a predecessor?
    float gap = 0;
    if ( pred != 0 ) {

        gap = pred->pos() - pred->length() - vehicle->pos();
        return gap < vehicle->interactionGap( lane, *pred );
    }
    // No predecessor. Does vehicle need to slow down because it "interacts"
    // with the lane end? (unfortunately it isn't able to look beyond the
    // lane yet.)
    gap = lane->length() - vehicle->pos();
    // There may be a vehicle on the succeeding lane, touching this lane 
    // partely     
    gap -= MSVehicleType::maxLength();
    
    return gap < vehicle->brakeGap( lane );
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::advan2right()
{
    // Candidate has no predInteraction, so it's coming state will be
    // it's accelState(). We call this stayState. We calculate the coming
    // state if candidate would change to the right. If this state is more
    // or equal advantageous, candidate will change.
    MSVehicle*           vehicle = veh( myCandi );
    MSVehicle*         neighLead = veh( myCandi - 1 );
    MSLane*            neighLane = ( myCandi - 1 )->lane;
    MSVehicle::State   stayState = vehicle->accelState( myCandi->lane );
    MSVehicle::State changeState = MSVehicle::State();

    // Calculate the compareStates.
    if ( neighLead == 0 ) { 

        // Vehicles has no neighbour, slow down towards lane-end.
        changeState = vehicle->nextState( neighLane, 
                                          neighLane->length() - 
                                          MSVehicleType::maxLength() -
                                          vehicle->pos() );
    }
    else { // There is a neigh-leader.

        float gap2lead = neighLead->pos() - neighLead->length() - 
                         vehicle->pos();
        changeState = vehicle->nextStateCompete( neighLane,
                                                 neighLead->state(),
                                                 gap2lead );
    }
    return MSVehicle::State::advantage( changeState, stayState );
}

//-------------------------------------------------------------------------//

bool 
MSLaneChanger::advan2left()
{
    // Vehicle will change to the left if this change is advantageous. 
    // Calculate the staying-state and the changing-state and compare
    // them.
    MSVehicle*           vehicle = veh( myCandi );
    MSVehicle*              pred = myCandi->lead;
    MSVehicle*         neighLead = veh( myCandi + 1 );
    MSLane*             stayLane = myCandi->lane;
    MSLane*           changeLane = ( myCandi + 1 )->lane;    
    MSVehicle::State   stayState = MSVehicle::State();
    MSVehicle::State changeState = MSVehicle::State();

    // If lanes in front of vehicle are empty, there is no need to change.
    // This can change if vehicle looks beyond it's lane in a later release.
    if ( pred == 0 && neighLead == 0 ) {

        return false;
    }
    
    // Calculate the staying-state.
    if ( pred == 0 ) {

        // Hopefully there is no need to subtract MSVehicleType::maxLength()
        // here because vehicle drove safe in the past and will stay on this
        // lane (if this condition wins).
        stayState = vehicle->nextState( stayLane, 
                                        stayLane->length() - vehicle->pos() );
    }
    else {

        float gap2pred = pred->pos() - pred->length() - vehicle->pos();
        stayState = vehicle->nextStateCompete( stayLane,
                                               pred->state(),
                                               gap2pred );
    }

    // Calculate the changing-state.
    if ( neighLead == 0 ) {

        changeState = vehicle->nextState( changeLane, 
                                          changeLane->length() - 
                                          MSVehicleType::maxLength() -
                                          vehicle->pos() );
    }
    else {
        
        float gap2lead = neighLead->pos() - neighLead->length() -
                         vehicle->pos();
        changeState = vehicle->nextStateCompete( changeLane,
                                                 neighLead->state(),
                                                 gap2lead );
    }

    // Compare the states.
    return MSVehicle::State::advantage( changeState, stayState );
}

//-------------------------------------------------------------------------//

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "MSLaneChanger.icc"
//#endif

// Local Variables:
// mode:C++
// End:












