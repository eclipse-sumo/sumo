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
// Revision 1.7  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.6  2003/03/20 16:21:12  dkrajzew
// windows eol removed; multiple vehicle emission added
//
// Revision 1.5  2003/03/03 14:56:21  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.4  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.3  2002/10/29 10:42:50  dkrajzew
// problems accured due to the deletion of a vehicle that reached his destination debugged
//
// Revision 1.2  2002/10/28 12:56:52  dkrajzew
// forgot reinitialisation of hopped vehicle added
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.13  2002/09/25 18:14:09  roessel
// Bugfix: advan2left(): a succLink was implicitly converted to
// bool. Thanks to g++-3.2
//
// Revision 1.12  2002/09/25 17:14:42  roessel
// MeanData calculation and output implemented.
//
// Revision 1.11  2002/07/31 17:33:00  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.12  2002/07/26 11:13:57  dkrajzew
// safeChange debugged
//
// Revision 1.11  2002/07/03 15:50:56  croessel
// safeChange uses now MSVehicle::isSafeChange instead of safeChange.
//
// Revision 1.10  2002/05/29 17:06:03  croessel
// Inlined some methods. See the .icc files.
//
// Revision 1.9  2002/05/22 16:39:35  croessel
// advan2left(): First vehicles going to use a prioritized link will not
// be allowed to change to the left because stayState calculation assumes
// deceleration.
//
// Revision 1.8  2002/05/17 12:36:37  croessel
// advan2left/right: Check if vehicle brakes too much because of laneChangers
// disability to look beyond a lane. This caused many crashes.
//
// Revision 1.7  2002/05/08 09:29:56  croessel
// Last change, i.e. test if vrh(target)!=0, moved from change2left/right
// to congested().
//
// Revision 1.6  2002/05/02 13:46:57  croessel
// change2left/right(): Check if veh(target) exists before passing target as
// argument.
//
// Revision 1.5  2002/04/24 13:41:57  croessel
// Changed the condition-order in change2left(). target-validity-ckeck
// must be the first one.
//
// Revision 1.4  2002/04/18 12:18:39  croessel
// Bug-fix: Problem was that a right and a left vehicle could change to a
// middle lane, even though they were overlapping. Solution: Introduction
// of hoppedVeh in ChangeElem and method overlapWithHopped().
//
// Revision 1.3  2002/04/18 11:37:33  croessel
// In updateLanes(): changed swap() against assignment.
//
// Revision 1.2  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
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

/* =========================================================================
 * included modules
 * ======================================================================= */
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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member method definitions
 * ======================================================================= */
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
        ce.follow    = 0;
        ce.lead      = 0;
        ce.lane      = *lane;
        ce.veh       = ( *lane )->myVehicles.begin();
        ce.hoppedVeh = 0;
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
        ce->hoppedVeh = 0;

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
    MSVehicle* vehicle = veh( myCandi );
    vehicle->_lcAction = MSVehicle::LCA_STRAIGHT;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime && (vehicle->id()==MSNet::searched1||vehicle->id()==MSNet::searched2)) {
        cout << "change:" << vehicle->id() << ": " << vehicle->pos() << ", " << vehicle->speed() << endl;
    }
#endif
    if ( candiOnAllowed( myCandi ) ) {
        if ( change2right() ) {
            vehicle->_lcAction = MSVehicle::LCA_RIGHT | MSVehicle::LCA_CHANGED;
            ( myCandi - 1 )->hoppedVeh = veh( myCandi );
            ( myCandi - 1 )->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( ( myCandi - 1 )->lane );
            vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime-5 && (vehicle->id()==MSNet::searched1||vehicle->id()==MSNet::searched2)) {
        cout << "changed2right" << endl;
    }
#endif
            return;
        }
        if ( change2left() ) {
            vehicle->_lcAction = MSVehicle::LCA_LEFT | MSVehicle::LCA_CHANGED;
            ( myCandi + 1 )->hoppedVeh = veh( myCandi );
            ( myCandi + 1 )->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( ( myCandi + 1 )->lane );
            vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime-5 && (vehicle->id()==MSNet::searched1||vehicle->id()==MSNet::searched2)) {
        cout << "changed2left" << endl;
    }
#endif
            return;
        }
    }
    else { // not on allowed
        vehicle->_lcAction = MSVehicle::LCA_URGENT;
        ChangerIt target = findTarget();
        if ( change2target( target ) ) {
            vehicle->_lcAction |= MSVehicle::LCA_CHANGED;
            target->hoppedVeh = veh( myCandi );
            target->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( target->lane );
            vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime-5 && (vehicle->id()==MSNet::searched1||vehicle->id()==MSNet::searched2)) {
        cout << "changed:" << vehicle->id() << ": " << vehicle->pos() << ", " << vehicle->speed() << endl;
    }
#endif
            return;
        }
    }
    // Candidate didn't change lane.
    myCandi->lane->myTmpVehicles.push_back( veh ( myCandi ) );
    vehicle->myLastLaneChangeOffset++;
#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime-5 && (vehicle->id()==MSNet::searched1||vehicle->id()==MSNet::searched2)) {
        cout << "kept" << endl;
    }
#endif
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

        ce->lane->myVehicles = ce->lane->myTmpVehicles;
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
MSLaneChanger::change2right()
{
    // Try to change to the right-lane if there is one. If this lane isn't
    // an allowed one, cancel the try. Otherwise, check some conditions. If
    // they are simultaniously fulfilled, a change is possible.
    if ( myCandi == myChanger.begin() ) {

        return false;
    }
    ChangerIt target = myCandi - 1;
    if ( overlapWithHopped( target ) ) {

        return false;
    }
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
    if ( target == myChanger.end()   ||
         overlapWithHopped( target ) ||
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
        veh(myCandi)->_lcAction |= MSVehicle::LCA_LEFT;

        assert( myCandi + 1 != myChanger.end() );
        return myCandi + 1;
    }
    veh(myCandi)->_lcAction |= MSVehicle::LCA_RIGHT;
    assert( myCandi != myChanger.begin() );
    return myCandi - 1;
}

//-------------------------------------------------------------------------//

bool
MSLaneChanger::change2target( ChangerIt target )
{
    // We are changing to an allowed lane. This change has to be a safe
    // one. Other conditions are irrelevant.
    if ( overlapWithHopped( target ) ) {

        return false;
    }
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
            // this vehicle is not able to change lanes, but the vehicle
            //  responsible for this, possibly too.
            // If so, don't let this vehicle stop - both would
            //  do it and cause a jam
            if(!neighFollow->onAllowed()) {
                vehicle->_lcAction |= MSVehicle::LCA_LANEBEGIN;
            }
            return true;
        }
    }
    if ( neighLead != 0 ) {

        if ( MSVehicle::overlap( vehicle, neighLead ) ) {
            return true;
        }
    }

    //
/*    if ( target->lane->length() - vehicle->pos() >
         MSVehicleType::maxLength() ) {
        return false;
    }
    return true;*/
    if(vehicle->speed()<vehicle->decelAbility()) {
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
    if ( veh( target ) != 0 ) {

        if ( veh( myCandi )->congested() &&
             veh( target  )->congested() ) {

            return true;
        }
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

    // the hopped vehicle possibly becomes the leader / follower
    if(target->hoppedVeh!=0) {
        double hoppedPos = target->hoppedVeh->pos();
        // the hopped vehicle possibly becomes the leader
        if(hoppedPos>vehicle->pos()) {
            if( neighLead==0 || neighLead->pos()>hoppedPos) {
                neighLead = target->hoppedVeh;
            }
        }
        // the hopped vehicle possibly becomes the follower
        else {
            if( neighFollow==0 || neighFollow->pos()<hoppedPos) {
                neighFollow = target->hoppedVeh;
            }
        }
    }

#ifdef ABS_DEBUG
    if(MSNet::globaltime>MSNet::searchedtime&&(vehicle->id()==MSNet::searched1||vehicle->id()==MSNet::searched2)) {
        if(neighFollow!=0) {
            cout << "NeighFollow:" << neighFollow->id() << endl;
        } else {
            cout << "No NeighFollow" << endl;
        }
    }
#endif

    // Check back gap
    if ( neighFollow == 0 ) {
        if(targetLane->myApproaching!=0) {
            // Check back gap to following vehicle
            if( vehicle->pos()<vehicle->length() ||
				!targetLane->myApproaching->isSafeChange_WithDistance(targetLane->myFirstDistance, *vehicle, targetLane)) {
                return false;
            }
        }
    } else {
        // Check gap to following vehicle
        if ( !neighFollow->isSafeChange( *vehicle, targetLane ) ) {
            // this vehicle is not able to change lanes, but the vehicle
            //  responsible for this, possibly too.
            // If so, don't let this vehicle stop - both would
            //  do it and cause a jam
            if(!neighFollow->onAllowed()) {
                vehicle->_lcAction |= MSVehicle::LCA_LANEBEGIN;
            }
            return false;
        }
    }

    // Check front gap
    if( neighLead==0 ) {
		double dist2LaneEnd = targetLane->length() - vehicle->pos();
		// check whether there is a vehicle on the following lane
		MSLinkCont::iterator link =
			targetLane->succLinkSec( *vehicle, 1, *targetLane );
		// if the lane to change to is a dead end
		if(targetLane->isLinkEnd(link)) {
			// check the headway on the tarhet lane only
	        return ( dist2LaneEnd > vehicle->brakeGap( targetLane ) );
		}
		// check the vehicle on the next lane to the approached lane
		const MSVehicle::State &nextLanePred = targetLane->myLastState;
        // Check front gap (no leading vehicle)
        return (
			vehicle->hasSafeGap(
				dist2LaneEnd+nextLanePred.pos(),
				nextLanePred.speed(),
				targetLane ) );
    } else {
        // Check gap to the leading vehicle
        return ( vehicle->isSafeChange( *neighLead, targetLane ) );
    }
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
    double gap = 0;
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


    // Calculate the compareState.
    if ( neighLead == 0 ) {

        // Vehicles has no neighbour, slow down towards lane-end.
        changeState = vehicle->nextState( neighLane,
                                          neighLane->length() -
                                          MSVehicleType::maxLength() -
                                          vehicle->pos() );
    }
    else { // There is a neigh-leader.

        double gap2lead = neighLead->pos() - neighLead->length() -
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

    // A first car using a prioritized link shouldn't change because
    // of the LaneChangers disability to look beyond the lane, i.e.
    // all vehicles are considered to brake towards the lane end. This
    // will cause some dangerous lanec-changes.
    bool linkPrio =
        (*(stayLane->succLinkSec( *vehicle, 1, *stayLane )))->myPrio;
    if ( pred == 0 && linkPrio == true ) {

        return false;
    }

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

        double gap2pred = pred->pos() - pred->length() - vehicle->pos();
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

        double gap2lead = neighLead->pos() - neighLead->length() -
                         vehicle->pos();
        changeState = vehicle->nextStateCompete( changeLane,
                                                 neighLead->state(),
                                                 gap2lead );
    }

    return MSVehicle::State::advantage( changeState, stayState );
}


//-------------------------------------------------------------------------//


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
#ifdef DISABLE_INLINE
#include "MSLaneChanger.icc"
#endif

// Local Variables:
// mode:C++
// End:
