/**
 * @file   MSLaneState.cpp
 * @author Christian Roessel
 * @date   Started Tue, 18 Feb 2003
 *
 * $Id$
 *
 * @brief  Definition of class MSLaneState.
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.36  2004/11/23 10:20:10  dkrajzew
// new detectors and tls usage applied; debugging
//
// Revision 1.35  2004/07/02 09:59:56  dkrajzew
// code stye applied
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
#include "MSNet.h"
#include "MSEventControl.h"
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
 * static member definitions
 * ======================================================================= */
vector< MSLaneState* > MSLaneState::laneStateDetectorsM;

string MSLaneState::xmlHeaderM(
"<?xml version=\"1.0\" standalone=\"yes\"?>\n\n"
"<!--\n"
"- noVehContrib is the number of vehicles have been on the lane for\n"
"  at least one timestep during the current intervall.\n"
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
"-->\n\n");

string MSLaneState::detectorInfoEndM( "</detector>\n" );


/* =========================================================================
 * method definitions
 * ======================================================================= */
MSLaneState::~MSLaneState()
{
    timestepDataM.clear();
    vehOnDetectorM.clear();
    waitingQueueElemsM.clear();
    vehLeftDetectorM.clear();
}


MSLaneState::MSLaneState( string id,
                          MSLane* lane,
                          double beginInMeters,
                          double lengthInMeters,
                          MSNet::Time deleteDataAfterSeconds ) :
    MSMoveReminder( lane, id ),
    timestepDataM     ( ),
    vehOnDetectorM    ( ),
    waitingQueueElemsM( ),
    vehLeftDetectorM  ( ),
    startPosM   ( MSNet::getCells( beginInMeters ) ),
    deleteDataAfterStepsM( MSNet::getSteps( deleteDataAfterSeconds ) ),
    modifiedSinceLastLookupM( true ),
    lookedUpLastNTimestepsM( 0 ),
    nVehContributedM( 0 )
 {
    assert( startPosM >= 0 );
    assert( startPosM + MSNet::getCells( lengthInMeters ) <= laneM->length() );
    endPosM = startPosM + MSNet::getCells( lengthInMeters );

    // insert object into dictionary
    if ( ! SingletonDictionary<
         std::string, MSLaneState* >::getInstance()->isInsertSuccess(
             idM, this ) ) {
        assert( false );
    }

    // insert object in static container
    laneStateDetectorsM.push_back( this );

//     // add reminder to lane
//     laneM->addMoveReminder( this );

    // start old-data removal through MSEventControl
    Command* deleteOldData = new SimpleCommand< MSLaneState >(
        this, &MSLaneState::deleteOldData );
    MSEventControl::getEndOfTimestepEvents()->addEvent(
        deleteOldData,
        deleteDataAfterStepsM,
        MSEventControl::ADAPT_AFTER_EXECUTION );
}


bool
MSLaneState::isStillActive( MSVehicle& veh,
                            double oldPos,
                            double newPos,
                            double newSpeed )
{
    // if vehicle has passed the detector completely we shouldn't
    // be here.
    // fraction of timestep the vehicle is on the detector after entry.
    double timestepFraction = MSNet::deltaT();
    if ( newPos <= startPosM ) {
        return true;
    }
    if ( oldPos <= startPosM && newPos > startPosM ) {
        // vehicle will enter detector
        timestepFraction = ( newPos-startPosM ) / newSpeed;
        assert( timestepFraction <= MSNet::deltaT() &&
                timestepFraction >= 0 );
        enterDetectorByMove( veh, 1.0 - timestepFraction );
    }
    if ( newPos - veh.length() > endPosM ) {
        // vehicle will leave detector
        // fraction of timestep the vehicle is not on the detector
        // after leave.
        double fractionReduce = ( newPos - veh.length() - endPosM ) /
            newSpeed;
        assert( fractionReduce <= MSNet::deltaT() &&
                fractionReduce >= 0 &&
                timestepFraction - fractionReduce > 0 );
        addMoveData(  veh, newSpeed, timestepFraction - fractionReduce );
        leaveDetectorByMove( veh, MSNet::deltaT() - fractionReduce );
        return false;
    }
    addMoveData( veh, newSpeed, timestepFraction );
    return true;
}


void
MSLaneState::dismissByLaneChange( MSVehicle& veh )
{
    if ( veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM ) {
        // vehicle is on detector
        leaveDetectorByLaneChange( veh );
    }
}


bool
MSLaneState::isActivatedByEmitOrLaneChange( MSVehicle& veh )
{
    if ( veh.pos() >= startPosM && veh.pos() - veh.length() < endPosM ) {
        // vehicle is on detector
        enterDetectorByEmitOrLaneChange( veh );
        return true;
    }
    if ( veh.pos() - veh.length() > endPosM ){
        // vehicle is beyond detector
        return false;
    }
    // vehicle is in front of detector
    return true;
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
    // return unit is [m/s]
    assert( lastNTimesteps > 0 );
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return laneM->maxSpeed();
    }
    double denominator = // [cells/step]
        accumulate( getStartIterator( lastNTimesteps, timestepDataM ),
                    timestepDataM.end(), 0.0, contTimestepSum );
    assert ( denominator > 0 );
    double speedS = accumulate( // [cells/step]
            getStartIterator( lastNTimesteps, timestepDataM ),
            timestepDataM.end(), 0.0, speedSum );
    return MSNet::getMeterPerSecond( speedS / denominator );
}


double
MSLaneState::getCurrentMeanSpeed( void )
{
    // return unit is [m/s]
    if ( timestepDataM.empty( ) ) {
        return 0;
    }
    TimestepData& data = timestepDataM.back();
    if ( data.contTimestepSumM == 0 ) {
        return 0;
    }
    return MSNet::getMeterPerSecond(
        data.speedSumM / data.contTimestepSumM );
}


double
MSLaneState::getMeanSpeedSquare( MSNet::Time lastNTimesteps )
{
    // return unit is [(m/s)^2]
    assert( lastNTimesteps > 0 );
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return -1;
    }
    double denominator = // [cells/step]
        accumulate( getStartIterator( lastNTimesteps, timestepDataM ),
                    timestepDataM.end(), 0.0, contTimestepSum );
    assert( denominator > 0 );
    double speedSSum = accumulate( // [(cells/step)^2]
        getStartIterator( lastNTimesteps, timestepDataM ),
        timestepDataM.end(), 0.0, speedSquareSum );
    return MSNet::getMeterPerSecond(
        MSNet::getMeterPerSecond( speedSSum / denominator ) );
}


double
MSLaneState::getCurrentMeanSpeedSquare( void )
{
    // return unit is [(m/s)^2]
    if ( timestepDataM.empty() ) {
        return 0;
    }
    TimestepData& data = timestepDataM.back();
    if ( data.contTimestepSumM == 0 ) {
        return 0;
    }
    return MSNet::getMeterPerSecond(
        MSNet::getMeterPerSecond(
            data.speedSquareSumM / data.contTimestepSumM ) );
}


double
MSLaneState::getMeanDensity( MSNet::Time lastNTimesteps )
{
    // return unit is veh/km
    assert( lastNTimesteps > 0 );
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return 0;
    }
    double stepsOnDetDuringlastNTimesteps  = accumulate(
        getStartIterator( lastNTimesteps, timestepDataM ),
        timestepDataM.end(), 0.0, contTimestepSum );
    return MSNet::getVehPerKm(
        stepsOnDetDuringlastNTimesteps / lastNTimesteps / laneM->length() );
}


double
MSLaneState::getCurrentDensity( void )
{
    // return unit is veh/km
    if ( timestepDataM.empty() ) {
        return 0;
    }
    return MSNet::getVehPerKm(
        timestepDataM.back().contTimestepSumM / laneM->length() );
}


double
MSLaneState::getMeanTraveltime( MSNet::Time lastNTimesteps )
{
    // return unit is [s]
    if ( getNVehContributed( lastNTimesteps ) == 0 ) {
        return MSNet::getSeconds( laneM->length() / laneM->maxSpeed() );
    }
    int nVehPassedEntire = getNVehPassedEntireDetector( lastNTimesteps );
    if ( nVehPassedEntire == 0 ) {
        return MSNet::getMeters( laneM->length() ) /
            getMeanSpeed( lastNTimesteps );
    }
    double traveltimeS = accumulate(
        lower_bound( vehLeftDetectorM.begin(), vehLeftDetectorM.end(),
                     getStartTimestep( lastNTimesteps ),
                     leaveTimestepLesser() ),
        vehLeftDetectorM.end(),
        0.0,
        traveltimeSum );
    double traveltime =  traveltimeS / nVehPassedEntire;
    assert ( traveltime >= laneM->length() / laneM->maxSpeed() );
    return MSNet::getSeconds( traveltime );
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


string
MSLaneState::getNamePrefix( void ) const
{
    return string("MSLaneState");
}


void
MSLaneState::writeXMLHeader( XMLDevice &dev ) const
{
    dev.writeString(xmlHeaderM);
}


void
MSLaneState::writeXMLOutput( XMLDevice &dev, MSNet::Time startTime, MSNet::Time stopTime )
{
    MSNet::Time lastNTimesteps = stopTime-startTime;
    dev.writeString("<interval start=\"").writeString(
        toString(startTime)).writeString("\" stop=\"").writeString(
        toString(stopTime)).writeString("\" ");
    dev.writeString("traveltime=\"").writeString(
        toString( getMeanTraveltime( lastNTimesteps ))).writeString("\" ");
    dev.writeString("speed=\"").writeString(
        toString( getMeanSpeed( lastNTimesteps ))).writeString("\" ");
    dev.writeString("speedsquare=\"").writeString(
        toString( getMeanSpeedSquare( lastNTimesteps ))).writeString("\" ");
    dev.writeString("density=\"").writeString(
        toString( getMeanDensity( lastNTimesteps ))).writeString("\" ");
    dev.writeString("noVehContrib=\"").writeString(
        toString( getNVehContributed( lastNTimesteps ))).writeString("\" ");
    dev.writeString("noVehEntire=\"").writeString(
        toString( getNVehPassedEntireDetector( lastNTimesteps ))).writeString("\" ");
    dev.writeString("noVehEntered=\"").writeString(
        toString( getNVehEnteredDetector( lastNTimesteps ))).writeString("\" ");
    dev.writeString("noVehLeftByMove=\"").writeString(
        toString( getNVehLeftDetectorByMove( lastNTimesteps ))).writeString("\"");
    dev.writeString("\"/>");
}


void
MSLaneState::writeXMLDetectorInfoStart( XMLDevice &dev ) const
{
    dev.writeString("<detector type=\"lanestate\" id=\"" + idM +
        "\" lane=\"" + laneM->id() + "\" startpos=\"" +
        toString(startPosM) + "\" length=\"" +
        toString(endPosM - startPosM) + "\" >\n");
}


void
MSLaneState::writeXMLDetectorInfoEnd( XMLDevice &dev ) const
{
    dev.writeString(detectorInfoEndM);
}


MSNet::Time
MSLaneState::getDataCleanUpSteps( void ) const
{
    return deleteDataAfterStepsM;
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
        find_if( vehLeftDetectorM.rbegin(), vehLeftDetectorM.rend(),
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
        find_if( vehLeftDetectorM.rbegin(), vehLeftDetectorM.rend(),
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
    if ( timestepDataM.size() > deleteDataAfterStepsM ) {
        end -= deleteDataAfterStepsM;
        timestepDataM.erase( timestepDataM.begin(), end );
    }
    // delete vehLeftDetectorM partly
    MSNet::Time deleteBeforeStep =
        MSNet::getInstance()->timestep() - deleteDataAfterStepsM;
    if ( deleteBeforeStep > 0 ) {
        vehLeftDetectorM.erase(
            vehLeftDetectorM.begin(),
            lower_bound( vehLeftDetectorM.begin(),
                         vehLeftDetectorM.end(),
                         deleteBeforeStep,
                         leaveTimestepLesser() ) );
    }
    return deleteDataAfterStepsM;
}


void
MSLaneState::calcWaitingQueueLength( void )
{
    modifiedSinceLastLookupM = true;
    int nVehQueuing = 0;
    if ( ! waitingQueueElemsM.empty() ) {
        // veh in waitingQueueElemsM are not sorted
        sort( waitingQueueElemsM.begin(), waitingQueueElemsM.end(),
              PosGreater() );

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
    (timestepDataM.end()-1)->queueLengthM = nVehQueuing;
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


