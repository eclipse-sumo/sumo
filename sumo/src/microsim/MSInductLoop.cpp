/**
 * @file   MSInductLoop.cpp
 * @author Christian Roessel
 * @date   Mon Jul 21 16:12:01 2003
 * @version $Id$
 * @brief  Definition of class MSInductLoop.
 * 
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Id$


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include "MSInductLoop.h"
#include <cassert>
#include <numeric>
#include <utility>
#include <helpers/SimpleCommand.h>
#include <utils/convert/ToString.h>
#include "MSEventControl.h"
#include "MSLane.h"


using namespace std;


string MSInductLoop::xmlHeaderM(
"<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
"<!--\n"
"- nVehContrib is the number of vehicles that passed the detector during the\n"
"  current interval.\n"
"- flow [veh/h] denotes the same quantity in [veh/h]\n"
"- occupancy [%] is the time the detector was occupied by vehicles.\n"
"- speed [m/s] is the average speed of the nVehContib vehicles.\n"
"  If nVehContrib==0, speed is set to -1.\n"
"- speedsquare [(m/s)^2]\n"
"  If nVehContrib==0, speedsquare is set to -1.\n"
"- length [m] is the average vehicle length of the nVehContrib vehilces.\n"
"  If nVehContrib==0, length is set to -1.\n"
"-->\n\n");

string MSInductLoop::xmlDetectorInfoEndM( "</detector>\n" );

MSInductLoop::MSInductLoop( const string& id, 
                            MSLane* lane,
                            double position, 
                            MSNet::Time deleteDataAfterSeconds ) 
    : MSMoveReminder( lane, id ),
      posM( position ),
      deleteDataAfterStepsM( MSNet::getSteps( deleteDataAfterSeconds ) ),
      lastLeaveTimestepM( 0 ),
      vehiclesOnDetM(),
      vehicleDataContM()
{
    assert( posM >= 0 && posM <= laneM->length() );
    
    // insert object into dictionary
    if ( ! InductLoopDict::getInstance()->isInsertSuccess( idM,
                                                           this ) ) {
        assert( false );
    }
    
    laneM->addMoveReminder( this );
    
    // start old-data removal through MSEventControl
    Command* deleteOldData = new SimpleCommand< MSInductLoop >(
        this, &MSInductLoop::deleteOldData );
    MSEventControl::getEndOfTimestepEvents()->addEvent(
        deleteOldData,
        deleteDataAfterStepsM,
        MSEventControl::ADAPT_AFTER_EXECUTION );
}

MSInductLoop::~MSInductLoop()
{
    vehiclesOnDetM.clear();
    vehicleDataContM.clear();
}


bool
MSInductLoop::isStillActive( MSVehicle& veh,
                             double oldPos,
                             double newPos,
                             double newSpeed )
{
    double entryTimestep = MSNet::getInstance()->timestep();
    double leaveTimestep = entryTimestep;
    
    if ( newPos < posM ) {
        // detector not reached yet                
        return true;
    }
    double speed = newSpeed * MSNet::deltaT();
    if ( vehiclesOnDetM.find( &veh ) == vehiclesOnDetM.end() ) {
        // entered the detector by move
        entryTimestep -= 1 - ( posM - oldPos ) / speed;
        if ( newPos - veh.length() > posM ) {
            // entered and passed detector in a single timestep
            leaveTimestep -= ( newPos - veh.length() - posM ) / speed;
            enterDetectorByMove( veh, entryTimestep );
            leaveDetectorByMove( veh, leaveTimestep );
            return false;
        }
        // entered detector, but not passed
        enterDetectorByMove( veh, entryTimestep );
        return true;
    }
    else {
        // vehicle has been on the detector the previous timestep
        if ( newPos - veh.length() >= posM ) {
            // vehicle passed the detector
            leaveTimestep -= ( newPos - veh.length() - posM ) / speed;
            leaveDetectorByMove( veh, leaveTimestep );
            return false;
        }
        // vehicle stays on the detector
        return true;
    }
}


void
MSInductLoop::dismissByLaneChange( MSVehicle& veh )
{
    if ( veh.pos() > posM && veh.pos() - veh.length() <= posM ) {
        // vehicle is on detector during lane change
        leaveDetectorByLaneChange( veh );
    }
}    


bool
MSInductLoop::isActivatedByEmitOrLaneChange( MSVehicle& veh )
{
    if ( veh.pos() > posM ) {
        // vehicle-front is beyond detector. Ignore
        return false;
    }
    // vehicle is in front of detector
    return true;
}


double
MSInductLoop::getFlow( MSNet::Time lastNTimesteps ) const
{
    // unit is [veh/h]
    assert( lastNTimesteps > 0 );
    return getNVehContributed( lastNTimesteps ) * 3600.0 /
        ( lastNTimesteps * MSNet::deltaT() );
}


double
MSInductLoop::getMeanSpeed( MSNet::Time lastNTimesteps ) const
{
    // unit is [m/s]
    assert( lastNTimesteps > 0 );
    int nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return -1;
    }
    return accumulate( getStartIterator( lastNTimesteps ),
                       vehicleDataContM.end(),
                       0.0,
                       speedSum ) / nVeh;
}


double
MSInductLoop::getMeanSpeedSquare( MSNet::Time lastNTimesteps ) const
{
    // unit is [(m/s)^2]
    assert( lastNTimesteps > 0 );
    int nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return -1;
    }
    return accumulate( getStartIterator( lastNTimesteps ),
                       vehicleDataContM.end(),
                       0.0,
                       speedSquareSum ) / nVeh;
}


double
MSInductLoop::getOccupancy( MSNet::Time lastNTimesteps ) const
{
    // unit is [%]
    assert( lastNTimesteps > 0 );
    int nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return 0;
    }
    return accumulate( getStartIterator( lastNTimesteps ),
                       vehicleDataContM.end(),
                       0.0,
                       occupancySum ) /
        static_cast<double>( lastNTimesteps );
}


double
MSInductLoop::getMeanVehicleLength( MSNet::Time lastNTimesteps ) const
{
    assert( lastNTimesteps > 0 );
    int nVeh = getNVehContributed( lastNTimesteps );
    if ( nVeh == 0 ) {
        return -1;
    }
    return accumulate( getStartIterator( lastNTimesteps ),
                       vehicleDataContM.end(),
                       0.0,
                       lengthSum ) / nVeh;
}
    

double
MSInductLoop::getTimestepsSinceLastDetection() const
{
    // This method was formely called  getGap()
    if ( vehiclesOnDetM.size() != 0 ) {
        // detetctor is occupied
        return 0;
    }
    return MSNet::getInstance()->timestep() - lastLeaveTimestepM;
}


int
MSInductLoop::getNVehContributed( MSNet::Time lastNTimesteps ) const
{
    return distance( getStartIterator( lastNTimesteps ),
                     vehicleDataContM.end() );
}


string
MSInductLoop::getNamePrefix( void )
{
    return "MSInductLoop_" + idM;
}


string&
MSInductLoop::getXMLHeader( void )
{
    return xmlHeaderM;
}


string
MSInductLoop::getXMLDetectorInfoStart( void )
{
    string detectorInfo("<detector type=\"inductionloop\" id=\"" + idM +
                        "\" lane=\"" + laneM->id() + "\" pos=\"" +
                        toString(posM) + "\" >");
    return detectorInfo;
}


string&
MSInductLoop::getXMLDetectorInfoEnd( void )
{
    return xmlDetectorInfoEndM;
}


string
MSInductLoop::getXMLOutput( MSNet::Time lastNTimesteps )
{
    MSNet::Time t( lastNTimesteps );
    string nVehContrib = "nVehContrib=\"" +
        toString( getNVehContributed( t ) ) + "\" ";
    string flow = "flow=\"" + toString( getFlow( t ) ) + "\" ";
    string occup = "occupancy=\"" + toString( getOccupancy( t ) ) + "\" ";
    string speed = "speed=\"" + toString( getMeanSpeed( t ) ) + "\" ";
    string speedSquare = "speedsquare=\"" +
        toString( getMeanSpeedSquare( t ) ) + "\" ";
    string avgVehLength = "length=\"" + toString( getMeanVehicleLength( t ) ) +
        "\"";
    return nVehContrib + flow + occup + speed + speedSquare + avgVehLength;
}


GUIDetectorWrapper*
MSInductLoop::buildDetectorWrapper(GUIGlObjectStorage &,
                                   GUILaneWrapper &)
{
    throw "Only within the gui-version";
}

MSNet::Time
MSInductLoop::getDataCleanUpSteps( void ) const
{
    return deleteDataAfterStepsM;
}


void
MSInductLoop::enterDetectorByMove( MSVehicle& veh,
                                   double entryTimestep )
{
    vehiclesOnDetM.insert( make_pair( &veh, entryTimestep ) );
}


void
MSInductLoop::leaveDetectorByMove( MSVehicle& veh,
                                   double leaveTimestep )
{
    VehicleMap::iterator it = vehiclesOnDetM.find( &veh );
    assert( it != vehiclesOnDetM.end() );
    double entryTimestep = it->second;
    vehiclesOnDetM.erase( it );
    assert( entryTimestep < leaveTimestep );
    vehicleDataContM.push_back( VehicleData( veh, entryTimestep,
                                             leaveTimestep ) );
    lastLeaveTimestepM = leaveTimestep;
}    


void
MSInductLoop::leaveDetectorByLaneChange( MSVehicle& veh )
{
    // Discard entry data
    vehiclesOnDetM.erase( &veh );
}


MSNet::Time
MSInductLoop::deleteOldData( void ) 
{
    double deleteBeforeTimestep =
        MSNet::getInstance()->timestep() - deleteDataAfterStepsM;
    if ( deleteBeforeTimestep > 0 ) {
        vehicleDataContM.erase(
            vehicleDataContM.begin(),
            lower_bound( vehicleDataContM.begin(),
                         vehicleDataContM.end(),
                         deleteBeforeTimestep,
                         leaveTimeLesser() ) );
    }
    return static_cast< MSNet::Time >
        ( deleteDataAfterStepsM );
}   


MSInductLoop::VehicleDataCont::const_iterator
MSInductLoop::getStartIterator( MSNet::Time lastNTimesteps ) const
{
    double startTime = 0;
    if ( lastNTimesteps < MSNet::getInstance()->timestep() ) {
        startTime = static_cast< double >(
            MSNet::getInstance()->timestep() - lastNTimesteps ) *
            MSNet::deltaT();
    }            
    return lower_bound( vehicleDataContM.begin(),
                        vehicleDataContM.end(),
                        startTime,
                        leaveTimeLesser() );
}

// Local Variables:
// mode:C++
// End:
