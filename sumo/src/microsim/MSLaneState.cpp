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
// Reduced myLastUpdateTime (7457467564) to myLastUpdateTime (745746756) due to compiler warnings (number too long for unsigned long)
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
                          OutputStyle style,
                          ofstream *file ) :
    idM             ( id ),
    styleM          ( style ),
    fileM           ( file ),
    timestepDataM   ( ),
    vehicleDataM    ( ),
    waitingQueueElemsM( ),
    laneM           ( lane ),
    posM            ( begin ),
    lengthM         ( length ),
    nIntervallsM    ( 0 ),
    sampleIntervalM ( sampleInterval )
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
        new MSLaneStateReminder( posM, posM + lengthM, *this );
    laneM->addMoveReminder( reminderM );

    // return when just a part of a junction-logic (no file)
    if(file==0) {
        return;
    }

    // start file-output through MSEventControl
    Command* writeData = new SimpleCommand< MSLaneState >(
        this, &MSLaneState::writeData );

    // !!!Diese Anweisung erzeugt einen Speicherzugriffsfehler
//     MSNet::getInstance()->getEventControl()->addEvent(
//         writeData,
//         sampleIntervalM,
//         MSEventControl::ADAPT_AFTER_EXECUTION );

    // Write header.
    switch ( styleM ) {
        case GNUPLOT:
        {
            ostringstream header;
            header << "# Lane-state-detector ID = " << idM << endl;
            header << "#   on Lane     " << laneM->id() << endl;
            header << "#   at position " << posM << endl;
            header << "#   sampleIntervall = "
                   << sampleIntervalM << " seconds" << endl << endl;
            header << "# n   endOfInterv nVehicles avgDensity avgFlow "
                   << "avgSpeed avgOccup avgLength" << endl;
            header << "#         [s]                [veh/km]  [veh/h] "
                   << " [m/s]     [s]       [m]" << endl;

            *fileM << header.str() << endl;
            break;
        }
        case CSV:
            // No header. CSV has no spec for comments etc.
            break;
        default:
            assert( true );
    }
}

//---------------------------------------------------------------------------//



//---------------------------------------------------------------------------//

int
MSLaneState::getNumberOfWaiting( MSNet::Time lastNTimesteps )
{

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
{}

double
MSLaneState::getCurrentMeanSpeed( void )
{}

double
MSLaneState::getMeanSpeedSquare( MSNet::Time lastNTimesteps )
{}

double
MSLaneState::getCurrentMeanSpeedSquare( void )
{}

double
MSLaneState::getMeanDensity( MSNet::Time lastNTimesteps )
{}

double
MSLaneState::getCurrentDensity( void )
{}

double
MSLaneState::getMeanTraveltime( MSNet::Time lastNTimesteps )
{}

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
    std::map< std::string, VehicleData >::iterator dataIt =
        vehicleDataM.find( veh.id() );
    assert ( dataIt != vehicleDataM.end() );
    dataIt->second.leaveContTimestepM =
        leaveTimestepFraction + MSNet::getInstance()->timestep();
    if ( ! dataIt->second.entireDetectorM ) {
        dataIt->second.entireDetectorM = false;
    }
    vehLeftLaneM.push_back( dataIt->second );
    vehicleDataM.erase( dataIt );
}

void
MSLaneState::leaveDetectorByLaneChange( MSVehicle& veh )
{
    cout << "MSLaneState::leaveDetectorByLaneChange" << endl;
    // finalize vehicleData
    std::map< std::string, VehicleData >::iterator dataIt =
        vehicleDataM.find( veh.id() );
    assert( dataIt != vehicleDataM.end() );
    dataIt->second.leaveContTimestepM = MSNet::getInstance()->timestep();
    dataIt->second.entireDetectorM = false;
    vehLeftLaneM.push_back( dataIt->second );
    vehicleDataM.erase( dataIt );
}

void
MSLaneState::actionBeforeMove( void )
{
    // create a TimestepData entry for every timestep. Not neccessary, but
    // makes live easier.
    timestepDataM.push_back(
        TimestepData( MSNet::getInstance()->timestep() ) );
}


void
MSLaneState::actionAfterMove( void )
{
    calcWaitingQueueLength();
}




void
MSLaneState::calcWaitingQueueLength( void )
{
    int nVehQueuing = 0;
    if ( ! waitingQueueElemsM.empty() ) {
        // veh in waitingQueueElemsM are not sorted
        sort( waitingQueueElemsM.begin(), waitingQueueElemsM.end(),
              WaitingQueuePos() );
    
        std::vector<WaitingQueueElem>::iterator it =
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


    // loesche Daten die aelter sind als vorhalteintervall

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


// void
// MSLaneState::sample( double simSec )
// {

//     // If sampleIntervall is over, write the data to file and reset the
//     // detector.
//     ++myNSamples;
//     if ( static_cast< double >( myNSamples ) * MSNet::deltaT() >=
//          mySampleIntervall ) {
// 		myNSamples = 0;
//         ++myNIntervalls;
//         if(myFile!=0) {
//             writeData();
//         }
//     }

//     const MSLane::VehCont &vehs = myLane->getVehiclesSecure();
//     MSLane::VehCont::const_iterator firstVehicle;
//     if ( myLane->empty() ) {
//         // no vehicles on lane
//         firstVehicle = vehs.end();
//     } else {
//         // find the vehicle
//         firstVehicle = find_if( vehs.begin(), vehs.end(),
//             bind2nd( MSLane::VehPosition(), myPos ) );
//     }
//     // update interval with zero if no vehicle is on the loop
//     if(firstVehicle==vehs.end()) {
//         // no vehicle was found
//         myVehicleNo.add(0);
//         myLocalDensity.add(0);
//         mySpeed.add(0);
//         myOccup.add(0);
//         myVehLengths.add(0);
//         myNoSlow.add(0);
//         return;
//     }
//     // We have now a valid beyond the detector.
//     MSLane::VehCont::const_iterator lastVehicle =
//         find_if( firstVehicle, vehs.end(),
//             bind2nd( MSLane::VehPosition(), myPos+myLength ) );
//     // go through the vehicles and compute the values
//     size_t noVehicles = distance(firstVehicle, lastVehicle);
//     double speeds = 0;
//     double lengths = 0;
//     size_t noSlow = 0;
//     for(MSLane::VehCont::const_iterator i=firstVehicle; i!=lastVehicle; i++) {
//         MSVehicle *veh = *i;
//         double speed = veh->speed();
//         speeds += speed;
//         if(speed<0.1) {
//             noSlow++;
//         }
//         lengths += veh->length();
//     }
//     // update values
//     myNoSlow.add(noSlow);
//     myVehicleNo.add(noVehicles);
//     mySpeed.add(speeds / (double) noVehicles);
//     myVehLengths.add(lengths / (double) noVehicles);
//     if(speeds!=0) {
//         myOccup.add( lengths / speeds );
//     } else {
//         myOccup.add( lengths / 0.000001 );
//     }
//     // this is just an approximation; The first and te last vehicles
//     //  should compete only for the amount of time they are within the
//     //  field
//     myLocalDensity.add( 1.0 - ((myLength - lengths) / myLength) );
// }
