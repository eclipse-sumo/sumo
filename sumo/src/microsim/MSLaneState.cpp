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
/*
namespace
{
    const char rcsid[] =
    "$Id$";
}
*/
// $Log$
// Revision 1.17  2003/05/28 15:35:47  roessel
// deleteOldData implemented.
// Added argument MSNet::Time deleteDataAfterSeconds to constructor.
//
// Revision 1.16  2003/05/28 11:18:09  roessel
// Pass pointer instead of reference to MSLaneStateReminder ctor.
//
// Revision 1.15  2003/05/28 07:51:25  dkrajzew
// had to add a return value due to the usage of the mem_func-function in combination with for_each (MSVC++-reasons?)
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
#include "../helpers/SimpleCommand.h"
#include "../helpers/SingletonDictionary.h"
#include <sstream>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <iostream>
#include <cassert>


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * member definitions
 * ======================================================================= */

MSLaneState::~MSLaneState()
{
    timestepDataM.clear();
    vehicleDataM.clear();
    waitingQueueElemsM.clear();
    vehLeftLaneM.clear();
    delete reminderM;
}

//---------------------------------------------------------------------------//

MSLaneState::MSLaneState( string id,
                          MSLane* lane,
                          double begin,
                          double length,
                          MSNet::Time sampleInterval,
                          ofstream *file,
                          MSNet::Time deleteDataAfterSeconds ) :
    idM             ( id ),
    fileM           ( file ),
    timestepDataM   ( ),
    vehicleDataM    ( ),
    waitingQueueElemsM( ),
    laneM           ( lane ),
    posM            ( begin ),
    lengthM         ( length ),
    nIntervallsM    ( 0 ),
    sampleIntervalM ( sampleInterval ),
    deleteDataAfterSecondsM( deleteDataAfterSeconds )
{
     assert( posM >= 0 );
    assert( posM <= laneM->length() );
    assert( posM + lengthM <= laneM->length() );

    // insert object into dictionary
    if ( ! SingletonDictionary<
         std::string, MSLaneState* >::getInstance()->isInsertSuccess(
             idM, this ) ) {
        assert( false );
    }

    // add reminder to lane
    MSMoveReminder* reminderM =
        new MSLaneStateReminder( posM, posM + lengthM, this );
    laneM->addMoveReminder( reminderM );

    // return when just a part of a junction-logic (no file)
    if(file==0) {
        return;
    }

    // start file-output through MSEventControl
    Command* writeData = new SimpleCommand< MSLaneState >(
        this, &MSLaneState::writeData );
    MSEventControl::getInstance()->addEvent(
        writeData,
        sampleIntervalM,
        MSEventControl::ADAPT_AFTER_EXECUTION );

    // start old-data removal through MSEventControl
    Command* deleteOldData = new SimpleCommand< MSLaneState >(
        this, &MSLaneState::deleteOldData );
    MSEventControl::getInstance()->addEvent(
        deleteOldData,
        deleteDataAfterSecondsM,
        MSEventControl::ADAPT_AFTER_EXECUTION );
    
//     // Write header.
//     switch ( styleM ) {
//         case GNUPLOT:
//         {
//             ostringstream header;
//             header << "# Lane-state-detector ID = " << idM << endl;
//             header << "#   on Lane     " << laneM->id() << endl;
//             header << "#   at position " << posM << endl;
//             header << "#   sampleIntervall = "
//                    << sampleIntervalM << " seconds" << endl << endl;
//             header << "# n   endOfInterv nVehicles avgDensity avgFlow "
//                    << "avgSpeed avgOccup avgLength" << endl;
//             header << "#         [s]                [veh/km]  [veh/h] "
//                    << " [m/s]     [s]       [m]" << endl;

//             *fileM << header.str() << endl;
//             break;
//         }
//         case CSV:
//             // No header. CSV has no spec for comments etc.
//             break;
//         default:
//             assert( true );
//     }
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
    assert( lastNTimesteps > 0 );
    if ( timestepDataM.empty() ) {
        return 0;
    }
    double denominator =
        accumulate( getStartIterator( lastNTimesteps ),
                    timestepDataM.end(), 0.0, contTimestepSum ) *
        MSNet::deltaT();
    if ( denominator == 0 ) {
        return 0;
    }
    return accumulate( getStartIterator( lastNTimesteps ),
                       timestepDataM.end(), 0.0, speedSum ) /
        denominator;
}

double
MSLaneState::getCurrentMeanSpeed( void )
{
    if ( timestepDataM.empty() ) {
        return 0;
    }
    TimestepData& data = timestepDataM.back();
    if ( data.contTimestepSumM == 0 ) {
        return 0;
    }
    return data.speedSumM / ( data.contTimestepSumM * MSNet::deltaT() );
}

double
MSLaneState::getMeanSpeedSquare( MSNet::Time lastNTimesteps )
{
    assert( lastNTimesteps > 0 );
    if ( timestepDataM.empty() ) {
        return 0;
    }
    double denominator =
        accumulate( getStartIterator( lastNTimesteps ),
                    timestepDataM.end(), 0.0, contTimestepSum ) *
        MSNet::deltaT();
    if ( denominator == 0 ) {
        return 0;
    }
    return accumulate( getStartIterator( lastNTimesteps ),
                       timestepDataM.end(), 0.0, speedSquareSum ) /
        denominator;
}

double
MSLaneState::getCurrentMeanSpeedSquare( void )
{
    if ( timestepDataM.empty() ) {
        return 0;
    }
    TimestepData& data = timestepDataM.back();
    if ( data.contTimestepSumM == 0 ) {
        return 0;
    }
    return data.speedSquareSumM / ( data.contTimestepSumM * MSNet::deltaT() );
}

double
MSLaneState::getMeanDensity( MSNet::Time lastNTimesteps )
{
    assert( lastNTimesteps > 0 );
    if ( timestepDataM.empty() ) {
        return 0;
    }
    // unit is veh/km
    return accumulate( getStartIterator( lastNTimesteps ),
                       timestepDataM.end(), 0.0, contTimestepSum ) /
        ( lastNTimesteps * laneM->length() ) * 1000.0;
}

double
MSLaneState::getCurrentDensity( void )
{
    if ( timestepDataM.empty() ) {
        return 0;
    }
    // unit is veh/km
    return timestepDataM.back().contTimestepSumM / laneM->length() * 1000.0;
}

double
MSLaneState::getMeanTraveltime( MSNet::Time lastNTimesteps )
{
    if ( vehLeftLaneM.empty() ) {
        return -1;
    }
    double startTimestep = MSNet::getInstance()->timestep() - lastNTimesteps;
    return accumulate(
        lower_bound( vehLeftLaneM.begin(), vehLeftLaneM.end(), startTimestep,
                     VehicleData::leaveTimestepLesser() ),
        vehLeftLaneM.end(),
        0.0,
        traveltimeSum ) * MSNet::deltaT();
}

void
MSLaneState::addMoveData( MSVehicle& veh,
                          double timestepFraction )
{
    cout << "MSLaneState::addMoveData " << idM
         << endl;
    assert (timestepFraction >= 0);
    assert (timestepFraction <= MSNet::deltaT() );

    // update timestepDataM
    TimestepData& data = timestepDataM.back();
    data.speedSumM += veh.speed();
    data.speedSquareSumM += veh.speed() * veh.speed();
    data.contTimestepSumM += timestepFraction;
    ++data.timestepSumM;

    // update waitingQueueElemsM
    waitingQueueElemsM.push_back( WaitingQueueElem (veh.pos(), veh.length()));
}

void
MSLaneState::enterDetectorByMove( MSVehicle& veh,
                                  double enterTimestepFraction )
{
    cout << "MSLaneState::enterDetectorByMove" << endl;
    // update vehicleDataM
    assert ( vehicleDataM.find( veh.id() ) == vehicleDataM.end() ) ;
    vehicleDataM.insert(
        make_pair( veh.id(), VehicleData(
                       enterTimestepFraction +
                       MSNet::getInstance()->timestep(),
                       true )));
}


void
MSLaneState::enterDetectorByEmitOrLaneChange( MSVehicle& veh )
{
    assert ( vehicleDataM.find( veh.id() ) == vehicleDataM.end() ) ;
    vehicleDataM.insert(
        make_pair( veh.id(), VehicleData(
                       MSNet::getInstance()->timestep(),
                       false )));
}


void
MSLaneState::leaveDetectorByMove( MSVehicle& veh,
                                  double leaveTimestepFraction )
{
    cout << "MSLaneState::leaveDetectorByMove" << endl;
    // finalize vehicleDataM
    VehicleDataMap::iterator dataIt =
        vehicleDataM.find( veh.id() );
    assert ( dataIt != vehicleDataM.end() );
    dataIt->second.leaveContTimestepM =
        leaveTimestepFraction + MSNet::getInstance()->timestep();
    if ( ! dataIt->second.passedEntireDetectorM ) {
        dataIt->second.passedEntireDetectorM = false;
    }
    // insert so that container keeps being sorted
    vehLeftLaneM.insert(
        find_if( vehLeftLaneM.rend(), vehLeftLaneM.rbegin(),
                 bind2nd( VehicleData::leaveTimestepLesser(),
                          dataIt->second.leaveContTimestepM ) ).base(),
        dataIt->second );
    vehicleDataM.erase( dataIt );
}

void
MSLaneState::leaveDetectorByLaneChange( MSVehicle& veh )
{
    cout << "MSLaneState::leaveDetectorByLaneChange" << endl;
    // finalize vehicleData
    VehicleDataMap::iterator dataIt =
        vehicleDataM.find( veh.id() );
    assert( dataIt != vehicleDataM.end() );
    dataIt->second.leaveContTimestepM = MSNet::getInstance()->timestep();
    dataIt->second.passedEntireDetectorM = false;
    // insert so that container keeps being sorted
    vehLeftLaneM.insert(
        find_if( vehLeftLaneM.rend(), vehLeftLaneM.rbegin(),
                 bind2nd( VehicleData::leaveTimestepLesser(),
                          dataIt->second.leaveContTimestepM ) ).base(),
        dataIt->second );
    vehicleDataM.erase( dataIt );
}

bool
MSLaneState::actionBeforeMove( void )
{
    // create a TimestepData entry for every timestep. Not neccessary, but
    // makes live easier.
    timestepDataM.push_back(
        TimestepData( MSNet::getInstance()->timestep() ) );
    return true;
}


bool
MSLaneState::actionAfterMove( void )
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
    // delete vehLeftLaneM partly
    MSNet::Time deleteBeforeStep =
        MSNet::getInstance()->timestep() - deleteDataAfterSecondsM;
    if ( deleteBeforeStep > 0 ) {
        vehLeftLaneM.erase(
            vehLeftLaneM.begin(),
            lower_bound( vehLeftLaneM.begin(),
                         vehLeftLaneM.end(),
                         deleteBeforeStep,
                         VehicleData::leaveTimestepLesser() ) );
    }
    return deleteDataAfterSecondsM;
}

void
MSLaneState::calcWaitingQueueLength( void )
{
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


MSLaneState::TimestepDataCont::iterator
MSLaneState::getStartIterator( MSNet::Time lastNTimesteps )
{
    TimestepDataCont::iterator start = timestepDataM.begin();
    if ( timestepDataM.size() > lastNTimesteps ) {
        start =  timestepDataM.end() - lastNTimesteps;
    }
    return start;
}


MSNet::Time
MSLaneState::writeData()
{
//     double avgDensity = 0;
//     double avgFlow    = 0;
//     double avgSpeed   = 0;
//     double avgLength  = 0;

//     double NPassedVehicles = myVehicleNo.getAbs();
//     if ( NPassedVehicles > 0 ) {

//         if ( NPassedVehicles > 1 ) {

//             avgDensity = myLocalDensity.getAbs() /
//                 static_cast< double >( NPassedVehicles - 1 ) * 1000.0;
//             avgSpeed  = mySpeed.getAbs() /
// 	            static_cast< double >( NPassedVehicles ); // [m/s]
//             avgLength = myVehLengths.getAbs() /
// 	            static_cast< double >( NPassedVehicles ); // [m]
// 	        // [veh/km], first detected vehicle doesn't
// 	        // contribute.
// 	        assert( avgDensity > 0 );
//         }

//         avgFlow   = static_cast< double >( NPassedVehicles ) /
// 	        static_cast< double >( mySampleIntervall ) * 3600.0; // [veh/h]
//     }

//     MSNet::Time endOfInterv = myNIntervalls * mySampleIntervall; // [s]

//     switch ( myStyle ) {
//         case GNUPLOT:
//             writeGnuPlot( endOfInterv,
//                           avgDensity,
//                           avgFlow,
//                           avgSpeed,
//                           myOccup.getAbs(),
//                           avgLength );
//             break;
//         case CSV:
//             writeCSV( endOfInterv,
//                       avgDensity,
//                       avgFlow,
//                       avgSpeed,
//                       myOccup.getAbs(),
//                       avgLength );
//             break;
//         default:
//             assert( true );
//     }
	cout << "writeData for LaneState "<< idM << " at timestep " <<
        MSNet::getInstance()->timestep() << endl;
    return sampleIntervalM;
}

//---------------------------------------------------------------------------//


void
MSLaneState::writeGnuPlot( MSNet::Time endOfInterv,
                           double avgDensity,
                           double avgSpeed,
                           double avgSpeedSquare,
                           double avgTraveltime,
                           int avgNumberOfWaiting )
{
//     ( *myFile ).setf( ios::fixed, ios::floatfield );
//     *myFile << setw( 5 ) << setprecision( 0 ) << myNIntervalls << " "
//             << setw(11 ) << setprecision( 0 ) << endOfInterv << " "
//             << setw( 9 ) << setprecision( 0 ) << myVehicleNo.getAbs() << " "
// 	    << setw(10 ) << setprecision( 2 ) << avgDensity << " "
//             << setw( 7 ) << setprecision( 1 ) << avgFlow << " "
//             << setw( 8 ) << setprecision( 3 ) << avgSpeed << " "
//             << setw( 8 ) << setprecision( 2 ) << occup << " "
//             << setw( 9 ) << setprecision( 3 ) << avgLength << endl;
}

//---------------------------------------------------------------------------//


void
MSLaneState::writeCSV( MSNet::Time endOfInterv,
                       double avgDensity,
                       double avgSpeed,
                       double avgSpeedSquare,
                       double avgTraveltime,
                       int avgNumberOfWaiting )
{
//     ( *myFile ).setf( ios::fixed, ios::floatfield );
//     *myFile << setw( 4 ) << setprecision( 0 ) << myNIntervalls << ";"
//             << setw( 6 ) << setprecision( 0 ) << endOfInterv << ";"
//             << setw( 5 ) << setprecision( 0 ) << myVehicleNo.getAbs() << ";"
//             << setw( 6 ) << setprecision( 2 ) << avgDensity << ";"
//             << setw( 4 ) << setprecision( 1 ) << avgFlow << ";"
//             << setw( 6 ) << setprecision( 3 ) << avgSpeed << ";"
//             << setw( 7 ) << setprecision( 2 ) << occup << ";"
//             << setw( 6 ) << setprecision( 3 ) << avgLength << endl;
}

//---------------------------------------------------------------------------//



// Local Variables:
// mode:C++
// End:

