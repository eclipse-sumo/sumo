//---------------------------------------------------------------------------//
//                        MSLaneState.cpp  -
//  Some kind of induct loops with a length
//                           -------------------
//  begin                : Tue, 18 Feb 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
//  organisation         : IVF/DLR
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//


// $Log$
// Revision 1.20  2003/06/05 15:48:36  roessel
// Added waitingQueueElemsM.push_back() for enterDetector-methods.
//
// Revision 1.19  2003/06/05 12:57:08  roessel
// Modified #includes.
// Changed calls to operator() of nested structs because of MSVC++ compile
// problems.
//
// Revision 1.18  2003/06/05 09:53:46  roessel
// Numerous changes and new methods/members.
//
// Revision 1.17  2003/05/28 15:35:47  roessel
// deleteOldData implemented.
// Added argument MSNet::Time deleteDataAfterSeconds to constructor.
//
// Revision 1.16  2003/05/28 11:18:09  roessel
// Pass pointer instead of reference to MSLaneStateReminder ctor.
//
// Revision 1.15  2003/05/28 07:51:25  dkrajzew
// had to add a return value due to the usage of the mem_func-function in
// combination with for_each (MSVC++-reasons?)
//
// Revision 1.14  2003/05/27 18:59:01  roessel
// Removed OutputStyle in ctor (output will be xml).
// Activated MSEventControl for regular file-output. Works now because
// MSEventControl is a singleton now.
//
// Revision 1.13  2003/05/26 15:24:15  roessel
// Removed warnings/errors. Changed return-type of getNumberOfWaiting to
// double.
//
// Revision 1.12  2003/05/26 13:56:57  roessel
// changed push_back to sorted-insert in leaveDetectorByMove and
// leaveDetectorByLaneChange.
//
// Revision 1.11  2003/05/26 13:19:20  roessel
// Completed all get* methods.
//
// Revision 1.10  2003/05/25 17:50:12  roessel
// Implemented getCurrentNumberOfWaiting.
// Added methods actionBeforeMove and actionAfterMove. actionBeforeMove creates
// a TimestepData entry in timestepDataM every timestep (makes live easier).
// actionAfterMove calculates the waitingQueueLength and updates the current
// TimestepData.
// These two methods must be called in the simulation loop.
//
// Revision 1.9  2003/05/23 16:42:22  roessel
// Added method getCurrentDensity().
//
// Revision 1.8  2003/05/22 12:41:00  roessel
// Two fixes (& and clear()) and many cout
//
// Revision 1.7  2003/05/21 16:20:44  dkrajzew
// further work detectors
//
// Revision 1.5  2003/04/04 15:29:09  roessel
// Reduced myLastUpdateTime (7457467564) to myLastUpdateTime (745746756) due
// to compiler warnings (number too long for unsigned long)
//
// Revision 1.4  2003/04/02 11:44:03  dkrajzew
// continuation of implementation of actuated traffic lights
//
// Revision 1.3  2003/03/19 08:02:02  dkrajzew
// debugging due to Linux-build errors
//
// Revision 1.2  2003/03/17 14:12:19  dkrajzew
// Windows eol removed
//
// Revision 1.1  2003/03/03 14:56:19  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSLaneState.h"
#include "MSLane.h"
#include "MSEdge.h"
#include "MSNet.h"
#include "MSEventControl.h"
#include "MSLaneStateReminder.h"
#include <utils/convert/ToString.h>
#include <helpers/SimpleCommand.h>
#include <helpers/SingletonDictionary.h>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <cassert>



/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member definitions
 * ======================================================================= */

// static member
vector< MSLaneState* > MSLaneState::laneStateDetectorsM;

string MSLaneState::xmlHeaderM(
"<!--\n"
"- noVehContrib is the number of vehicles have been on the lane for\n"
"  at least on timestep during the current intervall.\n"
"  They contribute to speed, speedsquare and density.\n"
"  They may not have passed the entire lane.\n"
"- noVehEntireLane is the number of vehicles that have passed the\n"
"  entire lane and left the lane during the current intervall. They\n"
"  may have started their journey on this lane in a previous intervall.\n"
"  Only those vehicles contribute to traveltime. \n"
"- noVehEntered is the number of vehicles that entered this lane\n"
"  during the current intervall either by move, emit or lanechange.\n"
"  Note that noVehEntered might be high if vehicles are emitted on\n"
"  this lane.\n"
"- noVehLeft is the number of vehicles that left this lane during\n"
"  the current intervall by move.\n"
"- traveltime [s]\n"
"  If noVehContrib==0 then traveltime is set to laneLength / laneMaxSpeed. \n"
"  If noVehContrib!=0 && noVehEntireLane==0 then traveltime is set to\n"
"  laneLength / speed.\n"
"  Else traveltime is calculated from the data of the vehicles that\n"
"  passed the entire lane.\n"
"- speed [m/s]\n"
"  If noVehContrib==0 then speed is set to laneMaxSpeed.\n"
"- speedsquare [(m/s)^2]\n"
"  If noVehContrib==0 then speedsquare is set to -1.\n"
"- density [veh/km]\n"
"  If noVehContrib==0 then density is set to 0.\n"
"-->\n");

MSLaneState::~MSLaneState()
{
    timestepDataM.clear();
    vehOnDetectorM.clear();
    waitingQueueElemsM.clear();
    vehLeftDetectorM.clear();
    delete reminderM;
}

//---------------------------------------------------------------------------//

MSLaneState::MSLaneState( string id,
                          MSLane* lane,
                          double begin,
                          double length,
                          MSNet::Time deleteDataAfterSeconds ) :
    idM               ( id ),
    timestepDataM     ( ),
    vehOnDetectorM    ( ),
    waitingQueueElemsM( ),
    vehLeftDetectorM  ( ),
    laneM             ( lane ),
    posM              ( begin ),
    lengthM           ( length ),
    deleteDataAfterSecondsM( deleteDataAfterSeconds ),
    modifiedSinceLastLookupM( true ),
    lookedUpLastNTimestepsM( 0 ),
    nVehContributedM( 0 )
 {
    assert( posM >= 0 );
    assert( posM + lengthM <= laneM->length() );

    // insert object into dictionary
    if ( ! SingletonDictionary<
         std::string, MSLaneState* >::getInstance()->isInsertSuccess(
             idM, this ) ) {
        assert( false );
    }

    // insert object in static container
    laneStateDetectorsM.push_back( this );

    // add reminder to lane
    MSMoveReminder* reminderM =
        new MSLaneStateReminder( posM, posM + lengthM, this, laneM );
    laneM->addMoveReminder( reminderM );

    // start old-data removal through MSEventControl
    Command* deleteOldData = new SimpleCommand< MSLaneState >(
        this, &MSLaneState::deleteOldData );
    MSEventControl::getEndOfTimestepEvents()->addEvent(
        deleteOldData,
        deleteDataAfterSecondsM,
        MSEventControl::ADAPT_AFTER_EXECUTION );
}


double
MSLaneState::getNumberOfWaiting( MSNet::Time lastNTimesteps )
{
    assert( lastNTimesteps > 0 );
    if ( timestepDataM.empty() ) {
        return 0;
    }
    TimestepDataCont::iterator start = timestepDataM.begin();
    TimestepDataCont::iterator end   = timestepDataM.end();
    if ( (end-1)->queueLengthM == -1 ) {
        // actionAfterMove has not bee called yet. Ignore last element
        --end;
    }
    if ( timestepDataM.size() - 1 > lastNTimesteps ) {
        start = end - lastNTimesteps;
    }
    return accumulate( start, end, 0.0, waitingQueueSum ) /
        static_cast< double >( lastNTimesteps );
}


int
MSLaneState::getCurrentNumberOfWaiting( void )
{
    if ( timestepDataM.empty() ) {
        return 0;
    }
    TimestepDataCont::iterator dataIt = timestepDataM.end() - 1;
    // a queueLengthM of -1 before actionAfterMove has been called indicates
    // that no veh contributed in this timestep. In this case actionAfterMove
    // sets queueLengthM to 0.
    if ( dataIt->queueLengthM == - 1 ) {
        if ( dataIt == timestepDataM.begin() ) {
            // container-size == 1
            return 0;
        }
        else {
            // actionAfterMove not called yet. Return previous timestep's data
            return (--dataIt)->queueLengthM;
        }
    }
    else {
        // actionAfterMove has bee called. Return current timestep's data
        return dataIt->queueLengthM;
    }
}

double
MSLaneState::getMeanSpeed( MSNet::Time lastNTimesteps )
{
    // unit is [m/s]
    assert( lastNTimesteps > 0 );
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return laneM->maxSpeed();
    }   
    double denominator =
        accumulate( getStartIterator( lastNTimesteps, timestepDataM ),
                    timestepDataM.end(), 0.0, contTimestepSum );
    assert ( denominator > 0 );
    return accumulate(
        getStartIterator( lastNTimesteps, timestepDataM ),
        timestepDataM.end(), 0.0, speedSum ) * MSNet::deltaT() /
        denominator;
}

double
MSLaneState::getCurrentMeanSpeed( void )
{
    // unit is [m/s]
    if ( timestepDataM.empty( ) ) {
        return 0;
    }
    TimestepData& data = timestepDataM.back();
    if ( data.contTimestepSumM == 0 ) {
        return 0;
    }
    return data.speedSumM * MSNet::deltaT() / data.contTimestepSumM;
}

double
MSLaneState::getMeanSpeedSquare( MSNet::Time lastNTimesteps )
{
    // unit is [(m/s)^2]
    assert( lastNTimesteps > 0 );
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return -1;
    }     
    double denominator =
        accumulate( getStartIterator( lastNTimesteps, timestepDataM ),
                    timestepDataM.end(), 0.0, contTimestepSum );
    assert( denominator > 0 );
    return accumulate( getStartIterator( lastNTimesteps, timestepDataM ),
                       timestepDataM.end(), 0.0, speedSquareSum ) *
        MSNet::deltaT() * MSNet::deltaT() / denominator;
}

double
MSLaneState::getCurrentMeanSpeedSquare( void )
{
    // unit is [(m/s)^2]
    if ( timestepDataM.empty() ) {
        return 0;
    }
    TimestepData& data = timestepDataM.back();
    if ( data.contTimestepSumM == 0 ) {
        return 0;
    }
    return data.speedSquareSumM * MSNet::deltaT() * MSNet::deltaT() /
        data.contTimestepSumM;
}

double
MSLaneState::getMeanDensity( MSNet::Time lastNTimesteps )
{
    // unit is veh/km
    assert( lastNTimesteps > 0 );
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return 0;
    }
    return accumulate( getStartIterator( lastNTimesteps, timestepDataM ),
                       timestepDataM.end(), 0.0, contTimestepSum ) /
        ( lastNTimesteps * laneM->length() ) * 1000.0;
}

double
MSLaneState::getCurrentDensity( void )
{
    // unit is veh/km
    if ( timestepDataM.empty() ) {
        return 0;
    }
    return timestepDataM.back().contTimestepSumM / laneM->length() * 1000.0;
}

double
MSLaneState::getMeanTraveltime( MSNet::Time lastNTimesteps )
{
    // unit is [s]
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return laneM->length() / laneM->maxSpeed();
    }
    int nVehPassedEntire = getNVehPassedEntireDetector( lastNTimesteps );
    if ( nVehPassedEntire == 0 ) {
        return laneM->length() / getMeanSpeedSquare( lastNTimesteps );
    }

    
    double traveltime = accumulate(
        lower_bound( vehLeftDetectorM.begin(), vehLeftDetectorM.end(),
                     getStartTimestep( lastNTimesteps ),
                     leaveTimestepLesser() ),
        vehLeftDetectorM.end(),
        0.0,
        traveltimeSum ) *
        MSNet::deltaT() / nVehPassedEntire;
    assert ( traveltime >= laneM->length() / laneM->maxSpeed() );
    return traveltime;
}


int
MSLaneState::getNVehContributed( MSNet::Time lastNTimesteps )
{
    if ( ! needsNewCalculation( lastNTimesteps ) ) {
        return nVehContributedM;
    }
    // get the nVehicles that left the detector after startTimestep
    int nVehLeft = distance(
        lower_bound( vehLeftDetectorM.begin(), vehLeftDetectorM.end(),
                     getStartTimestep( lastNTimesteps ),
                     leaveTimestepLesser() ),
        vehLeftDetectorM.end() );
    assert( nVehLeft >= 0 );
    nVehContributedM = nVehLeft + vehOnDetectorM.size();
    modifiedSinceLastLookupM = false;
    lookedUpLastNTimestepsM  = lastNTimesteps;
    return nVehContributedM;
}


int
MSLaneState::getNVehEnteredDetector( MSNet::Time lastNTimesteps )
{
    assert( lastNTimesteps > 0 );
    if ( timestepDataM.empty() ) {
        return 0;
    }
    return accumulate( getStartIterator( lastNTimesteps, timestepDataM ),
                       timestepDataM.end(), 0, nVehEnteredSum );
}


int
MSLaneState::getNVehLeftDetectorByMove( MSNet::Time lastNTimesteps )
{
    return accumulate(
        lower_bound( vehLeftDetectorM.begin(), vehLeftDetectorM.end(),
                     getStartTimestep( lastNTimesteps ),
                     leaveTimestepLesser() ),
        vehLeftDetectorM.end(), 0, leftByMoveSum );
}


int
MSLaneState::getNVehPassedEntireDetector( MSNet::Time lastNTimesteps )
{
    return accumulate(
        lower_bound( vehLeftDetectorM.begin(), vehLeftDetectorM.end(),
                     getStartTimestep( lastNTimesteps ),
                     leaveTimestepLesser() ),
        vehLeftDetectorM.end(), 0, passedEntireSum );
}

string&
MSLaneState::getXMLHeader( void )
{
    return xmlHeaderM;
}

string
MSLaneState::getXMLOutput( MSNet::Time lastNTimesteps )
{
    string traveltime = string("traveltime=\"") +
        toString( getMeanTraveltime( lastNTimesteps ) ) + string("\" ");
    string speed = string("speed=\"") +
        toString( getMeanSpeed( lastNTimesteps ) ) + string("\" ");
    string speedSquare = string("speedsquare=\"") +
        toString( getMeanSpeedSquare( lastNTimesteps ) ) + string("\" ");
    string density = string("density=\"") +
        toString( getMeanDensity( lastNTimesteps ) ) + string("\" ");
    string nVehContrib = string("noVehContrib=\"") +
        toString( getNVehContributed( lastNTimesteps ) ) + string("\" ");
    string nVehEntire = string("noVehEntire=\"") +
        toString( getNVehPassedEntireDetector( lastNTimesteps ) ) +
        string("\" ");
    string nVehEntered = string("noVehEntered=\"") +
        toString( getNVehEnteredDetector( lastNTimesteps ) ) + string("\" ");
    string nVehLeft = string("noVehLeftByMove=\"") +
        toString( getNVehLeftDetectorByMove( lastNTimesteps ) ) +
        string("\"");
    return traveltime + speed + speedSquare + density + nVehContrib +
        nVehEntire + nVehEntered + nVehLeft;
}
    

void
MSLaneState::addMoveData( MSVehicle& veh,
                          double newSpeed,
                          double timestepFraction )
{
    modifiedSinceLastLookupM = true;
    assert (timestepFraction >= 0);
    assert (timestepFraction <= MSNet::deltaT() );
    // update timestepDataM
    TimestepData& data = timestepDataM.back();
    data.speedSumM += newSpeed;
    data.speedSquareSumM += newSpeed * newSpeed;
    data.contTimestepSumM += timestepFraction;
    ++data.timestepSumM;   
    // update waitingQueueElemsM
    waitingQueueElemsM.push_back( WaitingQueueElem (veh.pos(), veh.length()));
}

void
MSLaneState::enterDetectorByMove( MSVehicle& veh,
                                  double enterTimestepFraction )
{
    modifiedSinceLastLookupM = true;    
    ++( timestepDataM.back().nVehEnteredDetectorM );
    // update vehOnDetectorM
    assert ( vehOnDetectorM.find( veh.id() ) == vehOnDetectorM.end() ) ;
    vehOnDetectorM.insert(
        make_pair( veh.id(), VehicleData(
                       enterTimestepFraction +
                       MSNet::getInstance()->timestep(),
                       true )));
    waitingQueueElemsM.push_back( WaitingQueueElem (veh.pos(), veh.length()));
}


void
MSLaneState::enterDetectorByEmitOrLaneChange( MSVehicle& veh )
{
    modifiedSinceLastLookupM = true;
    ++( timestepDataM.back().nVehEnteredDetectorM );
    assert ( vehOnDetectorM.find( veh.id() ) == vehOnDetectorM.end() );
    vehOnDetectorM.insert(
        make_pair( veh.id(), VehicleData(
                       MSNet::getInstance()->timestep(),
                       false )));
    waitingQueueElemsM.push_back( WaitingQueueElem (veh.pos(), veh.length()));
}


void
MSLaneState::leaveDetectorByMove( MSVehicle& veh,
                                  double leaveTimestepFraction )
{
    modifiedSinceLastLookupM = true;
    // finalize vehOnDetectorM
    VehicleDataMap::iterator dataIt =
        vehOnDetectorM.find( veh.id() );
    assert ( dataIt != vehOnDetectorM.end() );
    dataIt->second.leaveContTimestepM =
        leaveTimestepFraction + MSNet::getInstance()->timestep();
    if ( ! dataIt->second.passedEntireDetectorM ) {
        dataIt->second.passedEntireDetectorM = false;
    }
    dataIt->second.leftDetectorByMoveM = true;
    // insert so that container keeps being sorted
    vehLeftDetectorM.insert(
        find_if( vehLeftDetectorM.rend(), vehLeftDetectorM.rbegin(),
                 bind2nd( leaveTimestepLesser(),
                          dataIt->second.leaveContTimestepM ) ).base(),
        dataIt->second );
    vehOnDetectorM.erase( dataIt );
}

void
MSLaneState::leaveDetectorByLaneChange( MSVehicle& veh )
{
    modifiedSinceLastLookupM = true;
    // finalize vehicleData
    VehicleDataMap::iterator dataIt =
        vehOnDetectorM.find( veh.id() );
    assert( dataIt != vehOnDetectorM.end() );
    dataIt->second.leaveContTimestepM = MSNet::getInstance()->timestep();
    dataIt->second.passedEntireDetectorM = false;
    dataIt->second.leftDetectorByMoveM = false;
    // insert so that container keeps being sorted
    vehLeftDetectorM.insert(
        find_if( vehLeftDetectorM.rend(), vehLeftDetectorM.rbegin(),
                 bind2nd( leaveTimestepLesser(),
                          dataIt->second.leaveContTimestepM ) ).base(),
        dataIt->second );
    vehOnDetectorM.erase( dataIt );
}


void
MSLaneState::actionsBeforeMoveAndEmit( void )
{
    for_each( laneStateDetectorsM.begin(), laneStateDetectorsM.end(),
              mem_fun( &MSLaneState::actionBeforeMoveAndEmit ) );
}

void
MSLaneState::actionsAfterMoveAndEmit( void )
{
    for_each( laneStateDetectorsM.begin(), laneStateDetectorsM.end(),
              mem_fun( &MSLaneState::actionAfterMoveAndEmit ) );
}

string
MSLaneState::getNamePrefix( void )
{
    return string("MSLaneState");
}


bool
MSLaneState::actionBeforeMoveAndEmit( void )
{
    // create a TimestepData entry for every timestep. Not essential, but
    // makes live easier.
    timestepDataM.push_back(
        TimestepData( MSNet::getInstance()->timestep() ) );
    return true;
}

bool
MSLaneState::actionAfterMoveAndEmit( void )
{
    calcWaitingQueueLength();
    return true;
}

MSNet::Time
MSLaneState::deleteOldData( void )
{    
    // delete timestepDataM partly
    TimestepDataCont::iterator end = timestepDataM.end();
    if ( timestepDataM.size() > deleteDataAfterSecondsM ) {
        end -= deleteDataAfterSecondsM;
        timestepDataM.erase( timestepDataM.begin(), end );
    }
    // delete vehLeftDetectorM partly
    MSNet::Time deleteBeforeStep =
        MSNet::getInstance()->timestep() - deleteDataAfterSecondsM;
    if ( deleteBeforeStep > 0 ) {
        vehLeftDetectorM.erase(
            vehLeftDetectorM.begin(),
            lower_bound( vehLeftDetectorM.begin(),
                         vehLeftDetectorM.end(),
                         deleteBeforeStep,
                         leaveTimestepLesser() ) );
    }
    return deleteDataAfterSecondsM;
}

void
MSLaneState::calcWaitingQueueLength( void )
{
    modifiedSinceLastLookupM = true;
    int nVehQueuing = 0;
    if ( ! waitingQueueElemsM.empty() ) {
        // veh in waitingQueueElemsM are not sorted
        sort( waitingQueueElemsM.begin(), waitingQueueElemsM.end(),
              WaitingQueueElem::PosGreater() );

        WaitingQueueElemCont::iterator it =
            waitingQueueElemsM.begin();
        for (;;) {
            if ( it+1 == waitingQueueElemsM.end() ) {
                break;
            }
            if ( it->posM - it->vehLengthM - (it+1)->posM <
                 (it+1)->vehLengthM ) {
                ++nVehQueuing;
                ++it;
            }
            else {
                break;
            }
        }
        waitingQueueElemsM.clear();
    }
    // add nVehQueuing-value to current timestepDataM
    timestepDataM.begin()->queueLengthM = nVehQueuing;
}

double
MSLaneState::getStartTimestep( MSNet::Time lastNTimesteps )
{
    double timestep =
        static_cast< double >( MSNet::getInstance()->timestep() ) -
        static_cast< double >( lastNTimesteps );
    if ( timestep < 0 ) {
        return 0;
    }
    return timestep;
}


bool
MSLaneState::needsNewCalculation( MSNet::Time lastNTimesteps )
{
    if ( modifiedSinceLastLookupM ||
         lookedUpLastNTimestepsM != lastNTimesteps ) {
        return true;
    }
    return false;
}



// Local Variables:
// mode:C++
// End:


