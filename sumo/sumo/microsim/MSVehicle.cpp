/***************************************************************************
                          MSVehicle.C  -  Base for all
                          micro-simulation Vehicles. 
                             -------------------
    begin                : Mon, 05 Mar 2001
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
// Revision 1.3  2002/04/17 10:58:24  croessel
// Introduced dontMoveGap to handle floating-point-inaccuracy. Vehicles
// will keep their state if gap2pred is smaller.
//
// Revision 1.2  2002/04/11 15:25:56  croessel
// Changed float to double.
//
// Revision 1.1.1.1  2002/04/08 07:21:23  traffic
// new project name
//
// Revision 2.4  2002/03/20 16:02:23  croessel
// Initialisation and update of allowedLanes-container fixed.
//
// Revision 2.3  2002/03/13 17:44:38  croessel
// Assert removed because it causes not intended crashes for
// YIELD_ON_SUCC.
//
// Revision 2.2  2002/03/13 17:42:19  croessel
// In nextState() we need to dawdle _before_ we set the new state.
//
// Revision 2.1  2002/03/13 16:56:35  croessel
// Changed the simpleOutput to XMLOutput by introducing nested classes
// XMLOut. Output is now indented.
//
// Revision 2.0  2002/02/14 14:43:19  croessel
// Bringing all files to revision 2.0. This is just cosmetics.
//
// Revision 1.14  2002/02/05 13:51:53  croessel
// GPL-Notice included.
// In *.cpp files also config.h included.
//
// Revision 1.13  2002/02/05 11:52:11  croessel
// Changed method vAccel() to accelState().
// Introduced new mezhods for lane-change:
// State::advantage()
// onAllowed()
// overlap()
// congested()
//
// Revision 1.12  2002/02/01 13:57:07  croessel
// Changed methods and members bmax/dmax/sigma to more meaningful names
// accel/decel/dawdle.
//
// Revision 1.11  2002/01/31 13:53:08  croessel
// Assertion in move() added.
//
// Revision 1.10  2002/01/24 18:29:43  croessel
// New and modified gap-methods: interactionDist renamed to brakeGap,
// interactionGap introduced and calculation of gaps fixed.
//
// Revision 1.9  2002/01/16 10:07:50  croessel
// Modified some speed calculations to the use of MSNet::deltaT() instead
// of myTau.
//
// Revision 1.8  2001/12/20 14:50:07  croessel
// new method: id() (moved from .h)
//
// Revision 1.7  2001/12/13 14:11:58  croessel
// Introduction of MSVehicle::State methods.
//
// Revision 1.6  2001/12/06 13:16:41  traffic
// speed computation added (use -D _SPEEDCHECK)
//
// Revision 1.5  2001/11/21 15:25:13  croessel
// Numerous changes during debug session.
//
// Revision 1.4  2001/11/15 17:17:36  croessel
// Outcommented the inclusion of the inline *.iC files. Currently not needed.
// Vehicle-State introduced. Simulation is now independant of vehicle's
// speed. Still, the position is fundamental, also a gap between
// vehicles. But that's it.
//
// Revision 1.3  2001/11/14 15:47:34  croessel
// Merged the diffs between the .C and .cpp versions. Numerous changes
// in MSLane, MSVehicle and MSJunction.
//
// Revision 1.2  2001/11/14 10:49:07  croessel
// CR-line-end removed.
//
// Revision 1.1  2001/10/24 07:17:13  traffic
// new extension
//
// Revision 1.6  2001/10/23 09:31:41  traffic
// parser bugs removed
//
// Revision 1.4  2001/09/06 15:35:50  croessel
// Added operator<< to class MSVehicle for simple text output and minor 
// changes.
//
// Revision 1.3  2001/07/25 12:17:59  traffic
// CC problems with make_pair repaired
//                
// Revision 1.2  2001/07/16 12:55:47  croessel
// Changed id type from unsigned int to string. Added string-pointer 
// dictionaries and dictionary methods.
//
// Revision 1.1.1.1  2001/07/11 15:51:13  traffic
// new start
//

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSVehicle.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSVehicleType.h"
#include "MSNet.h"
#include <iostream>
#include <cassert>
#include <cmath>
#include <cstdlib>
#include <algorithm>


using namespace std;


// Init static member.
MSVehicle::DictType MSVehicle::myDict;
double MSVehicle::myTau = 1;

// It is possible to get collisions because of arithmetic-inaccuracy
// at small gaps. Therefore we introduce "dontMoveGap"; if gap2pred is
// smaller, than vehicle will keep it's state.
const double dontMoveGap = 0.01; 

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State::State() : myPos( 0 ), mySpeed( 0 ) 
{ 
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State::State( const State& state ) 
{ 
    myPos = state.myPos; 
    mySpeed = state.mySpeed;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State& 
MSVehicle::State::operator=( const State& state ) 
{
    myPos   = state.myPos;
    mySpeed = state.mySpeed;
    return *this;
}

/////////////////////////////////////////////////////////////////////////////

bool 
MSVehicle::State::operator!=( const State& state ) 
{
    return ( myPos   != state.myPos ||
             mySpeed != state.mySpeed );
}

/////////////////////////////////////////////////////////////////////////////

double 
MSVehicle::State::pos() const 
{ 
    return myPos; 
}

/////////////////////////////////////////////////////////////////////////////

void 
MSVehicle::State::setPos( double pos ) 
{ 
    assert( pos >= 0 ); 
    myPos = pos; 
}

/////////////////////////////////////////////////////////////////////////////

bool 
MSVehicle::State::advantage( const State& preferState, 
                             const State& compareState )
{
    return preferState.mySpeed >= compareState.mySpeed;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State::State( double pos, double speed ) : 
    myPos( pos ), mySpeed( speed ) 
{
}

/////////////////////////////////////////////////////////////////////////////

bool departTimeSortCrit( const MSVehicle* x, const MSVehicle* y )
{
    return x->myDesiredDepart < y->myDesiredDepart;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::~MSVehicle()
{
    for(DestinationCont::iterator i=myPersons.begin(); i!=myPersons.end(); i++) {
        MSNet::PersonCont *cont = (*i).second;
        for(MSNet::PersonCont::iterator j=cont->begin(); j!=cont->end(); j++) {
            delete *j;
        }
        delete cont;
    }
    //myWaitingPersons.clear();
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::MSVehicle(string id, MSNet::Route* route, MSNet::Time departTime,
                     const MSVehicleType* type) :
    myID(id),
    myState(),
    myRoute(route),
    myCurrEdge(0),
    myAllowedLanes(0),
    myDesiredDepart(departTime),
    myType(type)
{
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State
MSVehicle::state() const
{
    return myState;
}

/////////////////////////////////////////////////////////////////////////////

MSLane&
MSVehicle::departLane() 
{
    myCurrEdge = myRoute->begin();
    myAllowedLanes = ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );  
    return (*myCurrEdge)->departLane();
}

/////////////////////////////////////////////////////////////////////////////
 
MSNet::Time
MSVehicle::desiredDepart() const
{
    return myDesiredDepart;
}

/////////////////////////////////////////////////////////////////////////////

const MSEdge*
MSVehicle::succEdge(unsigned int nSuccs) const
{
    if ( hasSuccEdge( nSuccs ) ) {
        return *( myCurrEdge + nSuccs );
    }
    else {
        return 0;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::hasSuccEdge(unsigned int nSuccs) const
{
    // MSNet::RouteIterator myCurrEdge must be a random access iterator.
    if (myCurrEdge + nSuccs >= myRoute->end()) {
        return false;
    }
    else {
        return true;
    }
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::destReached( const MSEdge* targetEdge )
{
    MSNet::Route::const_iterator lastEdgeIt = myRoute->end() - 1;

    if ( targetEdge == *lastEdgeIt ) {

        myCurrEdge = lastEdgeIt;
        myAllowedLanes = 
            ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );                
        return true;
    }

    else {
        
        // search for the target in the vehicle's route. Usually there is
        // only one iteration. Only for very short edges a vehicle can
        // "jump" over one ore more edges in one timestep.
        MSNet::Route::const_iterator edgeIt = myCurrEdge;

        while ( *edgeIt != targetEdge ) {
     
            ++edgeIt;
            assert( edgeIt != myRoute->end() );
        }

        myCurrEdge = edgeIt;
        myAllowedLanes = 
            ( *myCurrEdge )->allowedLanes( **( myCurrEdge + 1 ) );          
        return false;
    }
}

/////////////////////////////////////////////////////////////////////////////

double 
MSVehicle::brakeGap( const MSLane* lane ) const
{
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed == 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    double vAccel = myState.mySpeed + myType->accel();
    double vNext = min( vAccel, min( myType->maxSpeed(), lane->maxSpeed() ) );
    double gap = vNext * ( myState.mySpeed / ( 2 * myType->decel() ) + myTau );
    
    // If we are very slow, the distance driven with an accelerated speed
    // might be longer.
    double accelDist = ( myState.mySpeed + myType->accel() ) * MSNet::deltaT();
    
    return max( gap, accelDist );
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::interactionGap( const MSLane* lane, const MSVehicle& pred ) const
{
    // Resolve the vsafe equation to gap. Assume predecessor has
    // speed != 0 and that vsafe will be the current speed plus acceleration,
    // i.e that with this gap there will be no interaction.
    double vF = myState.mySpeed;
    double vL = pred.myState.mySpeed;
    double dF = myType->decel();    
    double vAccel = myState.mySpeed + myType->accel();
    double vNext = min( vAccel, min( myType->maxSpeed(), lane->maxSpeed() ) );
    double gap = ( vNext - vL  ) *
                ( ( vF + vL ) / ( 2 * dF ) + myTau ) + vL * myTau;
                
    // safeGap my be negative ( no interaction between vehicles )
    return max( static_cast<double>( 0 ), gap );            
}

/////////////////////////////////////////////////////////////////////////////

double 
MSVehicle::safeGap( const MSVehicle& pred ) const
{
    // Resolve the vsafe equation to gap. Assume that this vehicle will brake,
    // i.e. vsafe = v - d*deltaT.
    double vF = myState.mySpeed;
    double vL = pred.myState.mySpeed;
    double dF = myType->decel();
    double vDecel = vF - dF * MSNet::deltaT();
    double gap = ( vDecel - vL  ) *
                ( ( vF + vL ) / ( 2 * dF ) + myTau ) + vL * myTau;
                    
    // safeGap my be negative ( no interaction between vehicles )
    return max( static_cast<double>( 0 ), gap );
}

/////////////////////////////////////////////////////////////////////////////
    
double 
MSVehicle::driveDist( State state ) const
{
    return state.mySpeed * MSNet::deltaT();
}
    
/////////////////////////////////////////////////////////////////////////////

double 
MSVehicle::decelDist() const
{
    return myType->decel() * pow( MSNet::deltaT() , 2 );
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State 
MSVehicle::accelState( const MSLane* lane ) const
{
    return State( 0, vaccel( lane ) ); 
} 

/////////////////////////////////////////////////////////////////////////////

void
MSVehicle::move( MSLane* lane,
                 MSVehicle* pred,
                 MSVehicle* neigh, double gap2neigh )
{
#ifdef _SPEEDCHECK
  novehicles++;
#endif

    // Pragmatic solution: ignore neighbours
    double vAccel   = vaccel( lane );
    double vMax     = myType->maxSpeed();
    double vLaneMax = lane->maxSpeed();
    
    double gap2pred = pred->myState.myPos - pred->myType->myLength -
                     myState.myPos;
    assert( gap2pred >= 0 );
    double vSafe   = vsafe( myState.mySpeed, myType->decel(), 
                           gap2pred, pred->myState.mySpeed );
     
    // min{ v+a, vmax, lanevmax, vsafe }   
    double vNext = vMin( vAccel, vMax, vLaneMax, vSafe );
    
    vNext = dawdle( vNext );    
    
    // update position and speed
    myState.myPos  += vNext * MSNet::deltaT();
    assert( myState.myPos < lane->length() );
    myState.mySpeed = vNext;
}

////////////////////////////////////////////////////////////////////////////

void
MSVehicle::moveDecel2laneEnd( MSLane* lane )
{
    double gap = lane->length() - myState.myPos;
    assert( gap <= brakeGap( lane ) );
    
    // Slow down and dawdle.   
    double vSafe  = vsafe( myState.mySpeed, myType->decel(), gap, 0 );  
    double vNext  = dawdle( vSafe );        
    
    // update position and speed
    myState.myPos  += vNext * MSNet::deltaT();
    assert( myState.myPos < lane->length() );
    myState.mySpeed = vNext;    
}

////////////////////////////////////////////////////////////////////////////

void
MSVehicle::moveUpdateState( const State newState )
{
    myState.myPos  += newState.mySpeed * MSNet::deltaT();
    assert( myState.myPos >= 0 );
    
    myState.mySpeed = newState.mySpeed;
    assert( myState.mySpeed >= 0 );
}

////////////////////////////////////////////////////////////////////////////

void
MSVehicle::moveSetState( const State newState )
{
    myState = newState;
    assert( myState.myPos >= 0 );
    assert( myState.mySpeed >= 0 );        
}

////////////////////////////////////////////////////////////////////////////

MSVehicle::State
MSVehicle::nextState( MSLane* lane,
                      State predState,  double gap2pred,
                      State neighState, double gap2neigh ) const
{
    // Don't move if gap2pred < dontMoveGap to handle arithmetic inaccuracy.
    if ( gap2pred < dontMoveGap ) {

        return myState;
    }

    // Pragmatic solution: ignore neighbours
    double vAccel   = vaccel( lane );
    double vMax     = myType->maxSpeed();
    double vLaneMax = lane->maxSpeed();
    
    double vSafe = 0;
    if ( predState != State() ) {
    
        assert( gap2pred >= 0 );
        vSafe = vsafe( myState.mySpeed, myType->decel(), 
                       gap2pred, predState.mySpeed );
    }
    else {
        
        vSafe = vLaneMax; // Don't confuse vMin() with a not set vSafe.
    }
    
    // min{ v+a, vmax, lanevmax, vsafe }   
    double vNext   = dawdle( vMin( vAccel, vMax, vLaneMax, vSafe ) );
    double nextPos = myState.myPos + vNext * MSNet::deltaT(); // Will be 
    // overridden if veh leaves lane.     
    
    return State( nextPos, vNext );   
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::State 
MSVehicle::nextState( MSLane* lane, double gap ) const
{
    // Don't move if gap < dontMoveGap to handle arithmetic inaccuracy.
    if ( gap < dontMoveGap ) {

        return myState;
    }

    // TODO
    // If we know that we will slow down, is there still the need to dawdle?
    
    double vSafe   = vsafe( myState.mySpeed, myType->decel(), gap, 0 );
    vSafe         = dawdle( vSafe );
    double nextPos = myState.myPos + vSafe * MSNet::deltaT(); // Will be 
    // overridden if veh leaves lane. 
       
    return State( nextPos, vSafe );  
}   

/////////////////////////////////////////////////////////////////////////////
    
MSVehicle::State
MSVehicle::nextStateCompete( MSLane* lane, 
                             State predState, 
                             double gap2pred ) const
{
    double vAccel   = vaccel( lane );
    double vMax     = myType->maxSpeed();
    double vLaneMax = lane->maxSpeed();
    double vSafe    = vsafe( myState.mySpeed, myType->decel(), 
                            gap2pred, predState.mySpeed );
    
    double vNext    = vMin( vAccel, vMax, vLaneMax, vSafe );    
    return State( 0, dawdle( vNext ) ); 
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::onAllowed( const MSLane* lane ) const
{
     MSEdge::LaneCont::const_iterator compare = 
        find( myAllowedLanes->begin(), myAllowedLanes->end(), lane );
     return ( compare != myAllowedLanes->end() );
}

/////////////////////////////////////////////////////////////////////////////

bool
MSVehicle::overlap( const MSVehicle* veh1, const MSVehicle* veh2 )
{
    if ( veh1->myState.myPos < veh2->myState.myPos ) {
        
        return veh2->myState.myPos - veh2->myType->myLength < 
               veh1->myState.myPos;
    }
    return veh1->myState.myPos - veh1->myType->myLength < 
           veh2->myState.myPos;
}
 
/////////////////////////////////////////////////////////////////////////////

bool 
MSVehicle::congested()
{
    return myState.mySpeed < 60.0 / 3.6;
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::vsafe( double speed, 
                  double decel, 
                  double gap2pred, 
                  double predSpeed ) const 
{
    // Calculate the Stefan Krauss (SK) vsafe.

    assert( speed >= 0 );
    assert( gap2pred >= 0 );
    assert( predSpeed >= 0 );
    double vsafe = predSpeed + 
        ( ( gap2pred - predSpeed * myTau ) / 
          ( ( ( predSpeed + speed ) / ( 2 * decel ) ) + myTau ) );

// This assertion has not the intended meaning because of doubleing inaccuracy.
//    assert( vsafe <= gap2pred ); // Collision-free condition.
// This is not the solution, because vsafe allows this vehicle to overlap at 
// time t+1 with pred at time t.
//    assert( vsafe <= gap2pred + 0.01 ); 
    assert( vsafe >= 0 );

    return vsafe;
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::vaccel( const MSLane* lane ) const
{
    // Accelerate until vehicle's max speed reached.
    double vVehicle = min( myState.mySpeed + myType->accel() * MSNet::deltaT(),
                          myType->myMaxSpeed );
    
    // But don't drive faster than max lane speed.
    return min( vVehicle, lane->maxSpeed() ); 
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::dawdle( double speed ) const
{
    // generate random number out of [0,1]
    double random = static_cast< double >( rand() ) /
        static_cast< double >( RAND_MAX );
                   
    // Dawdle. 
    // TODO:
    // We already have a safe speed, if we dawdle max, is it possible
    // to reduce the speed 2*decel?
    speed -= myType->dawdle() * myType->decel() * MSNet::deltaT() * random;
    
    return max( static_cast< double >( 0 ), speed );
}

/////////////////////////////////////////////////////////////////////////////

double 
MSVehicle::vMin( double v1, double v2, double v3, double v4 ) const
{
    return min( min( v1, v2 ), min ( v3, v4 ) );
}

/////////////////////////////////////////////////////////////////////////////

double 
MSVehicle::tau()
{ 
    return myTau;
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::pos() const
{
    return myState.myPos;
}

/////////////////////////////////////////////////////////////////////////////

double
MSVehicle::length() const
{
    return myType->length();
}

/////////////////////////////////////////////////////////////////////////////

void 
MSVehicle::addPerson( MSPerson* person, MSEdge* destinationEdge ) 
{
    DestinationCont::iterator i = myPersons.find( destinationEdge );
    if( i == myPersons.end() ) {
        MSNet::PersonCont* cont = new MSNet::PersonCont();
        cont->push_back(person);
        myPersons.insert( DestinationCont::value_type( destinationEdge, 
                                                       cont ) );   
    } 
    else {
        ( *i ).second->push_back( person );
    }
}

/////////////////////////////////////////////////////////////////////////////

MSNet::PersonCont*
MSVehicle::leavingAt( MSEdge* edge ) 
{
    DestinationCont::iterator i = myPersons.find( edge );
    if( i == myPersons.end() ) {
        return 0;
    }
    return ( ( *i ).second );
}

/////////////////////////////////////////////////////////////////////////////

MSNet::PersonCont* 
MSVehicle::unloadPersons( MSEdge *edge ) {
    DestinationCont::iterator i = myPersons.find( edge );
    if ( i == myPersons.end() ) {
        return 0;
    }
    MSNet::PersonCont* cont = ( *i ).second;
    myPersons.erase( i );
    return cont;
}

///////////////////////////////////////////////////////////////////////////

bool
MSVehicle::dictionary(string id, MSVehicle* ptr)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        myDict.insert(DictType::value_type(id, ptr));
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle*
MSVehicle::dictionary(string id)
{
    DictType::iterator it = myDict.find(id);
    if (it == myDict.end()) {
        // id not in myDict.
        return 0;
    }
    return it->second;
}

/////////////////////////////////////////////////////////////////////////////

double 
MSVehicle::speed() const
{
    return myState.mySpeed;
}

/////////////////////////////////////////////////////////////////////////////
    
string
MSVehicle::id()
{
    return myID;
}

/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSVehicle& veh )
{
    os << "MSVehicle: Id = " << veh.myID << endl;
    return os;
}

/////////////////////////////////////////////////////////////////////////////

MSVehicle::XMLOut::XMLOut( const MSVehicle& obj,
                           unsigned indentWidth,
                           bool withChildElemes ) :
    myObj( obj ),                           
    myIndentWidth( indentWidth ),
    myWithChildElemes( withChildElemes )
{
}

/////////////////////////////////////////////////////////////////////////////

ostream&
operator<<( ostream& os, const MSVehicle::XMLOut& obj )
{
    string indent( obj.myIndentWidth , ' ' );

    os << indent << "<vehicle id=\"" << obj.myObj.myID << "\" pos=\"" 
       << obj.myObj.pos() << "\" speed=\"" << obj.myObj.speed() 
       << "\"/>" << endl; 

// Currently there are no child elements. Maybe persons will be in future.                      
//    if ( obj.myWithChildElemes ) {        
// 
//    }                      
        
    return os;   
}

/////////////////////////////////////////////////////////////////////////////

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

//#ifdef DISABLE_INLINE
//#include "MSVehicle.iC"
//#endif

// Local Variables:
// mode:C++
// End:

