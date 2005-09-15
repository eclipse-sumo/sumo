/***************************************************************************
                          MSSlowLaneChanger.cpp  -  Handles lane-changes within
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
// Revision 1.2  2005/09/15 11:10:46  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.1  2004/11/23 10:20:54  dkrajzew
// debugging
//
// Revision 1.2  2004/11/22 12:40:47  dksumo
// some adaptations
//
// Revision 1.1  2004/10/22 12:49:26  dksumo
// initial checkin into an internal, standalone SUMO CVS
//
// Revision 1.16  2004/03/19 13:09:40  dkrajzew
// debugging
//
// Revision 1.15  2004/02/18 05:29:11  dkrajzew
// false resetting of values after swap patched
//
// Revision 1.14  2003/12/12 12:55:59  dkrajzew
// looking back implemented
//
// Revision 1.13  2003/11/11 08:36:21  dkrajzew
// removed some debug-variables
//
// Revision 1.12  2003/10/16 08:33:49  dkrajzew
// new lane changing rules implemented
//
// Revision 1.11  2003/10/15 11:40:59  dkrajzew
// false rules removed; initial state for further tests
//
// Revision 1.10  2003/07/18 12:35:04  dkrajzew
// removed some warnings
//
// Revision 1.9  2003/07/16 15:29:51  dkrajzew
// myFirstState renamed to myLastState in MSLane
//
// Revision 1.8  2003/06/18 11:30:26  dkrajzew
// debug outputs now use a DEBUG_OUT macro instead of cout; this shall ease the search for further couts which must be redirected to the messaaging subsystem
//
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
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSSlowLaneChanger.h"
#include "MSVehicle.h"
#include "MSVehicleType.h"
#include <cassert>
#include <iterator>
#include <cstdlib>
#include <cmath>

#ifdef ABS_DEBUG
#include "MSDebugHelper.h"
#endif

#ifdef DISABLE_INLINE
#include "MSSlowLaneChanger.icc"
#endif


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * some definitions (debugging only)
 * ======================================================================= */
#define DEBUG_OUT cout


/* =========================================================================
 * member method definitions
 * ======================================================================= */
MSSlowLaneChanger::~MSSlowLaneChanger()
{

}

//-------------------------------------------------------------------------//

MSSlowLaneChanger::MSSlowLaneChanger( MSEdge::LaneCont* lanes )
	: MSLaneChanger(lanes)
{
}

//-------------------------------------------------------------------------//


//-------------------------------------------------------------------------//


//-------------------------------------------------------------------------//

bool
MSSlowLaneChanger::change()
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
	vehicle->getLaneChangeModel().setState(0);
/*    vehicle->getLaneChangeState(*this).setState(
        MSVehicle::LaneChangeState::LCACT_NONE);*/
//    vehicle->getLaneChangeState(*this).setIsOverlaping(false);
/*    MSVehicle::LaneChangeState &lcs = vehicle->getLaneChangeState(*this);
    double pos = vehicle->pos();
    // check whether the vehicle must change lane to contniue his route
    if(lcs.wants2Right4Direction(pos)) {
        if(change2RightPossible()) {
            ( myCandi - 1 )->hoppedVeh = veh( myCandi );
            ( myCandi - 1 )->lane->myTmpVehicles.push_back( vehicle );
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( ( myCandi - 1 )->lane );
            vehicle->myLastLaneChangeOffset = 0;
            lcs.admitChange2Right();
        } else {
            MSVehicle* neighFollow = ( myCandi - 1 )->follow;
            if(neighFollow!=0) {
                neighFollow->getLaneChangeState(*this).acceptBefore();
            }
        }
        return;
    }
    if(lcs.wants2Left4Direction(pos)) {
        if(change2LeftPossible()) {
            ( myCandi + 1 )->hoppedVeh = veh( myCandi );
            ( myCandi + 1 )->lane->myTmpVehicles.push_back( vehicle );// was veh(myCandi)
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( ( myCandi + 1 )->lane );
            vehicle->myLastLaneChangeOffset = 0;
            lcs.admitChange2Left();
        } else {
            MSVehicle* neighFollow = ( myCandi + 1 )->follow;
            if(neighFollow!=0) {
                neighFollow->getLaneChangeState(*this).acceptBefore();
            }
        }
        return;
    }
*/

//    vehicle->_lcAction = MSVehicle::LCA_STRAIGHT;
#ifdef ABS_DEBUG
    if(debug_globaltime>=debug_searchedtime && (vehicle->id()==debug_searched1||vehicle->id()==debug_searched2)) {
        DEBUG_OUT << "change:" << vehicle->id() << ": " << vehicle->pos() << ", " << vehicle->speed() << endl;
    }
#endif
    if ( candiOnAllowed( myCandi ) ) {
        if ( change2right() ) {
//            vehicle->_lcAction = MSVehicle::LCA_RIGHT | MSVehicle::LCA_CHANGED;
            ( myCandi - 1 )->hoppedVeh = veh( myCandi );
            ( myCandi - 1 )->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( ( myCandi - 1 )->lane );
//            vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime-5 && (vehicle->id()==debug_searched1||vehicle->id()==debug_searched2)) {
        DEBUG_OUT << "changed2right" << endl;
    }
#endif
            return true;
        }
        if ( change2left() ) {
//            vehicle->_lcAction = MSVehicle::LCA_LEFT | MSVehicle::LCA_CHANGED;
            ( myCandi + 1 )->hoppedVeh = veh( myCandi );
            ( myCandi + 1 )->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( ( myCandi + 1 )->lane );
//            vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime-5 && (vehicle->id()==debug_searched1||vehicle->id()==debug_searched2)) {
        DEBUG_OUT << "changed2left" << endl;
    }
#endif
            return true;
        }
    }
    else { // not on allowed
        vehicle->getLaneChangeModel().setState(
			vehicle->getLaneChangeModel().getState()|LCA_URGENT);
//            MSVehicle::LaneChangeState::LCACT_NEEDS_DIRECTION_CHANGE);
        ChangerIt target = findTarget();
        if ( change2target( target ) ) {
//            vehicle->_lcAction |= MSVehicle::LCA_CHANGED;
            target->hoppedVeh = veh( myCandi );
            target->lane->myTmpVehicles.push_back( veh ( myCandi ) );
            vehicle->leaveLaneAtLaneChange();
            vehicle->enterLaneAtLaneChange( target->lane );
//            vehicle->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime && (vehicle->id()==debug_searched1||vehicle->id()==debug_searched2)) {
        DEBUG_OUT << "changed:" << vehicle->id() << ": at" << vehicle->getLane().id() << ", " << vehicle->pos() << ", " << vehicle->speed() << endl;
    }
#endif
            return true;
        } else {
/*            MSVehicle* neighFollow = ( target )->follow;
            if(neighFollow!=0) {
//                neighFollow->getLaneChangeState(*this).acceptBefore();
            }*/

            // check whether the vehicles should be swapped
            MSVehicle *prohibitor = veh(target);
            if( prohibitor!=0
                &&
		        (vehicle->getLaneChangeModel().getState()&LCA_URGENT)!=0
                &&
                (vehicle->getLaneChangeModel().getState()&(LCA_LEFT|LCA_RIGHT))!=(prohibitor->getLaneChangeModel().getState()&(LCA_LEFT|LCA_RIGHT))
				) {
/*
                MSVehicle::LaneChangeState &lcs_veh =
                    vehicle->getLaneChangeState(*this);
                MSVehicle::LaneChangeState &lcs_proh =
                    prohibitor->getLaneChangeState(*this);*/
                // check for position and speed
                if( prohibitor->speed()<0.1&&vehicle->speed()<0.1
                    &&
                    fabs(prohibitor->pos()-vehicle->pos())<0.1
                    &&
                    prohibitor->length()-vehicle->length()==0
/*                    &&
                    lcs_veh.isNotOverlaping() && lcs_proh.isNotOverlaping()
                    */
                    // !!! ggf. hierhin auch die safe-gap Zusatzabfrage
                    ) {


                    // ok, may be swapped
                        // remove vehicle to swap with
                    target->lane->myTmpVehicles.erase(
                        target->lane->myTmpVehicles.end()-1);
                        // set this vehicle
                    target->hoppedVeh = vehicle;
                    target->lane->myTmpVehicles.push_back( vehicle );
                    myCandi->hoppedVeh = prohibitor;
                    myCandi->lane->myTmpVehicles.push_back( prohibitor );

                    // leave lane and detectors
                    vehicle->leaveLaneAtLaneChange();
                    prohibitor->leaveLaneAtLaneChange();
                    // patch position and speed
                    double p1 = vehicle->pos();
                    vehicle->myState.myPos = prohibitor->myState.myPos;
                    prohibitor->myState.myPos = p1;
                    p1 = vehicle->speed();
                    vehicle->myState.mySpeed = prohibitor->myState.mySpeed;
                    prohibitor->myState.mySpeed = p1;
                    // enter lane and detectors
                    vehicle->enterLaneAtLaneChange( target->lane );
                    prohibitor->enterLaneAtLaneChange( myCandi->lane );
                    // mark lane change
//                    vehicle->myLastLaneChangeOffset = 0;
//                    prohibitor->myLastLaneChangeOffset = 0;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime-5 && (vehicle->id()==debug_searched1||vehicle->id()==debug_searched2)) {
        DEBUG_OUT << "swapped:" << vehicle->id() << ": at" << vehicle->getLane().id() << ", " << vehicle->pos() << ", " << vehicle->speed() << endl;
    }
#endif
                    return true;
                }
            }
        }
    }
    // Candidate didn't change lane.
    myCandi->lane->myTmpVehicles.push_back( veh ( myCandi ) );
//    vehicle->myLastLaneChangeOffset++;
#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime-5 && (vehicle->id()==debug_searched1||vehicle->id()==debug_searched2)) {
        DEBUG_OUT << "kept" << endl;
    }
#endif
    return false;
}

/*

bool
MSSlowLaneChanger::change2RightPossible()
{
    ChangerIt target = myCandi - 1;
    if ( overlapWithHopped( target ) ) {

        return false;
    }
    if (

        ! overlap( target )     &&
//        ! congested( target )   &&
          safeChange( target )  &&
        ! predInteraction()     &&
          advan2right()

        ) {
        return true;
    }
    return false;
}


bool
MSSlowLaneChanger::change2LeftPossible()
{
    ChangerIt target = myCandi + 1;
    if ( target == myChanger.end()   ||
         overlapWithHopped( target ) ||
         ! candiOnAllowed( target ) ) {

        return false;
    }

    if (

        ! overlap( target )     &&
//        ! congested( target )   &&
          safeChange( target )  &&
          advan2left()

        ) {
        return true;
    }
    return false;
}
*/

//-------------------------------------------------------------------------//


//-------------------------------------------------------------------------//


//-------------------------------------------------------------------------//


//-------------------------------------------------------------------------//

bool
MSSlowLaneChanger::change2right()
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
MSSlowLaneChanger::change2left()
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

//-------------------------------------------------------------------------//

bool
MSSlowLaneChanger::change2target( ChangerIt target )
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
MSSlowLaneChanger::overlap( ChangerIt target )
{
    // If the candidate overlaps one potential neighbour on the target
    // lane, lane-change is forbidden.
    MSVehicle* vehicle     = veh( myCandi );

    MSVehicle* neighFollow = veh(target);
    if(target->hoppedVeh!=0) {
        double hoppedPos = target->hoppedVeh->pos();
        if( hoppedPos<=veh(myCandi)->pos() &&
            (neighFollow==0 || neighFollow->pos()>hoppedPos) ) {

            neighFollow = target->hoppedVeh;
        }
    }

    MSVehicle* neighLead   = target->lead;
    if(target->hoppedVeh!=0) {
        double hoppedPos = target->hoppedVeh->pos();
        if( hoppedPos>veh(myCandi)->pos() &&
            (neighLead==0 || neighLead->pos()>hoppedPos) ) {

            neighLead = target->hoppedVeh;
        }
    }

    if ( neighFollow != 0 ) {

        if ( MSVehicle::overlap( vehicle, neighFollow ) ) {
            // this vehicle is not able to change lanes, but the vehicle
            //  responsible for this, possibly too.
            // If so, don't let this vehicle stop - both would
            //  do it and cause a jam
/*            if(!neighFollow->onAllowed()) {
//                vehicle->_lcAction |= MSVehicle::LCA_LANEBEGIN;
            }*/
            vehicle->getLaneChangeModel().setState(
				vehicle->getLaneChangeModel().getState()|LCA_OVERLAPPING);//.setIsOverlaping(true);
            return true;
        }
    }
    if ( neighLead != 0 ) {

        if ( MSVehicle::overlap( vehicle, neighLead ) ) {
			vehicle->getLaneChangeModel().setState(
				vehicle->getLaneChangeModel().getState()|LCA_OVERLAPPING);//
//            vehicle->getLaneChangeState(*this).setIsOverlaping(true);
            return true;
        }
    }

    //
/*    if ( target->lane->length() - vehicle->pos() >
         MSVehicleType::maxLength() ) {
        return false;
    }
    return true;*/
    /*
    if(vehicle->speed()<vehicle->decelAbility()) {
        return false;
    }
    */
    return false;
}

//-------------------------------------------------------------------------//

bool
MSSlowLaneChanger::congested( ChangerIt target )
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
MSSlowLaneChanger::safeChange( ChangerIt target )
{
    // Check if candidate's change to target-lane will be safe, i.e. is there
    // enough back-gap to the neighFollow to drive collision-free (if there is
    // no neighFollow, keep a safe-gap to the beginning of the lane) and is
    // there enough gap for the candidate to neighLead to drive collision-
    // free (if there is no neighLead, be sure that candidate is able to slow-
    // down towards the lane end).
    MSVehicle* vehicle     = veh( myCandi );
    MSVehicle* neighLead   = target->lead;
    MSVehicle* neighFollow = veh(target);
    MSLane* targetLane     = target->lane;

    if(target->hoppedVeh!=0) {
        double hoppedPos = target->hoppedVeh->pos();
        if( hoppedPos<=veh(myCandi)->pos() &&
            (neighFollow==0 || neighFollow->pos()>hoppedPos) ) {

            neighFollow = target->hoppedVeh;
        }
        if( hoppedPos>veh(myCandi)->pos() &&
            (neighLead==0 || neighLead->pos()>hoppedPos) ) {

            neighLead = target->hoppedVeh;
        }
    }

#ifdef ABS_DEBUG
    if(debug_globaltime>debug_searchedtime&&(vehicle->id()==debug_searched1||vehicle->id()==debug_searched2)) {
        if(neighFollow!=0) {
            DEBUG_OUT << "NeighFollow:" << neighFollow->id() << endl;
        } else {
            DEBUG_OUT << "No NeighFollow" << endl;
        }
    }
#endif

    // Check back gap
    if ( neighFollow == 0 ) {
        if(targetLane->myApproaching!=0) {
            // Check back gap to following vehicle
            double backDist = targetLane->myBackDistance;
            if(backDist<0) {
                backDist = -backDist;
            } else {
                backDist = vehicle->pos() - backDist;
            }
            if(backDist<0) {
                return false; // !!! isn't this caught anywhere else?
            }
            if( vehicle->pos()<vehicle->length() ||
				!targetLane->myApproaching->isSafeChange_WithDistance(backDist, *vehicle, targetLane)) {
                return false;
            }
        }
    } else {
        // Check gap to following vehicle
        if ( !neighFollow->isSafeChange( *vehicle, targetLane ) ) {
            return false;
        }
    }

    // Check front gap
    if( neighLead==0 ) {
        MSLane* targetLane = target->lane;
        MSLinkCont::iterator link =
            targetLane->succLinkSec( *veh(myCandi), 1, *targetLane );
        if(targetLane->isLinkEnd(link)) {
            return true;
        }
        MSLane *nextLane = (*link)->getLane();
        if(nextLane==0) {
            return true;
        }
        neighLead = nextLane->getLastVehicle(*this);
        if(neighLead==0) {
            return true;
        }
        // Check front gap (no leading vehicle)
        return (
			vehicle->hasSafeGap(
				myCandi->lane->length()-veh(myCandi)->pos()+neighLead->pos(),
				neighLead->speed(),
				targetLane ) );
    } else {
        // Check gap to the leading vehicle
        return ( vehicle->isSafeChange( *neighLead, targetLane ) );
    }
}

//-------------------------------------------------------------------------//

bool
MSSlowLaneChanger::predInteraction()
{
    // No predecessor interaction is one of the change2right conditions.
    // Interaction means that vehicle's next state is affected by the prede-
    // cessor rather than by vehicle's accelerating abilities and maxSpeed.
    MSVehicle* vehicle = veh( myCandi );
    MSVehicle* pred    = myCandi->lead;
    MSLane*    lane    = myCandi->lane;



    // Is there interaction with a predecessor?
    if ( pred != 0 ) {

        if(pred->speed()<(80.0*3.6)) {
            return false;
        }
        double gap = pred->pos() - pred->length() - vehicle->pos();
        return gap < vehicle->interactionGap( lane, *pred );
    }
    // No predecessor. Does vehicle need to slow down because it "interacts"
    // with the lane end? (unfortunately it isn't able to look beyond the
    // lane yet.)
    double gap = lane->length() - vehicle->pos();
    // There may be a vehicle on the succeeding lane, touching this lane
    // partely
    gap -= MSVehicleType::maxLength();

    return gap < vehicle->brakeGap( lane );
}

//-------------------------------------------------------------------------//

bool
MSSlowLaneChanger::advan2right()
{
    // Candidate has no predInteraction, so it's coming state will be
    // it's accelState(). We call this stayState. We calculate the coming
    // state if candidate would change to the right. If this state is more
    // or equal advantageous, candidate will change.
    MSVehicle*           vehicle = veh( myCandi );
    MSVehicle*         neighLead = ( myCandi - 1 )->lead;
    MSVehicle*         thisLead  = ( myCandi )->lead;
    MSLane*            neighLane = ( myCandi - 1 )->lane;
    MSLane*            thisLane  = ( myCandi )->lane;
    MSVehicle::State   stayState = vehicle->accelState( myCandi->lane );
    MSVehicle::State changeState = MSVehicle::State();


    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead == 0 ) {
        neighLaneVSafe = vehicle->vsafe(vehicle->speed(), vehicle->decelAbility(),
            neighLane->length() - vehicle->pos(), 0);
    } else {
        neighLaneVSafe = vehicle->vsafe(vehicle->speed(), vehicle->decelAbility(),
            neighLead->pos() - neighLead->length() - vehicle->pos(),
            neighLead->speed());
    }
    if(thisLead==0) {
        thisLaneVSafe = vehicle->vsafe(vehicle->speed(),
            vehicle->decelAbility(),  thisLane->length() - vehicle->pos(),
            0);
    } else {
        thisLaneVSafe = vehicle->vsafe(vehicle->speed(), vehicle->decelAbility(),
            thisLead->pos() - thisLead->length() - vehicle->pos(),
            thisLead->speed());
    }
    return thisLaneVSafe<neighLaneVSafe;
/*
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
    */
}

//-------------------------------------------------------------------------//

bool
MSSlowLaneChanger::advan2left()
{
    // Vehicle will change to the left if this change is advantageous.
    // Calculate the staying-state and the changing-state and compare
    // them.
/*    MSVehicle*           vehicle = veh( myCandi );
    MSVehicle*              pred = myCandi->lead;
    MSVehicle*         neighLead = veh( myCandi + 1 );
    MSLane*             stayLane = myCandi->lane;
    MSLane*           changeLane = ( myCandi + 1 )->lane;
    MSVehicle::State   stayState = MSVehicle::State();
    MSVehicle::State changeState = MSVehicle::State();
*/
    MSVehicle*           vehicle = veh( myCandi );
    MSVehicle*         neighLead = ( myCandi + 1 )->lead;
    MSVehicle*         thisLead  = ( myCandi )->lead;
    MSLane*            neighLane = ( myCandi + 1 )->lane;
    MSLane*            thisLane  = ( myCandi )->lane;
    MSVehicle::State   stayState = vehicle->accelState( myCandi->lane );
    MSVehicle::State changeState = MSVehicle::State();

    double neighLaneVSafe, thisLaneVSafe;
    if ( neighLead == 0 ) {
        neighLaneVSafe = vehicle->vsafe(vehicle->speed(), vehicle->decelAbility(),
            neighLane->length() - vehicle->pos(), 0);
    } else {
        neighLaneVSafe = vehicle->vsafe(vehicle->speed(), vehicle->decelAbility(),
            neighLead->pos() - neighLead->length() - vehicle->pos(),
            neighLead->speed());
    }
    if(thisLead==0) {
        thisLaneVSafe = vehicle->vsafe(vehicle->speed(),
            vehicle->decelAbility(),  thisLane->length() - vehicle->pos(),
            0);
    } else {
        thisLaneVSafe = vehicle->vsafe(vehicle->speed(), vehicle->decelAbility(),
            thisLead->pos() - thisLead->length() - vehicle->pos(),
            thisLead->speed());
    }
    return thisLaneVSafe<neighLaneVSafe;
/*
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

    return MSVehicle::State::advantage( changeState, stayState );*/
}


//-------------------------------------------------------------------------//


bool
MSSlowLaneChanger::change2RightPossible()
{
    ChangerIt target = myCandi - 1;
    if ( overlapWithHopped( target ) ) {

        return false;
    }
    if (

        ! overlap( target )     &&
//        ! congested( target )   &&
          safeChange( target )  &&
        ! predInteraction()     &&
          advan2right()

        ) {
        return true;
    }
    return false;
}


bool
MSSlowLaneChanger::change2LeftPossible()
{
    ChangerIt target = myCandi + 1;
    if ( target == myChanger.end()   ||
         overlapWithHopped( target ) ||
         ! candiOnAllowed( target ) ) {

        return false;
    }

    if (

        ! overlap( target )     &&
//        ! congested( target )   &&
          safeChange( target )  &&
          advan2left()

        ) {
        return true;
    }
    return false;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

// Local Variables:
// mode:C++
// End:
