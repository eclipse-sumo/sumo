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
template<class _T>
MSLaneState<_T>::~MSLaneState<_T>()
{
    delete myFile;
}

//---------------------------------------------------------------------------//

template<class _T>
MSLaneState<_T>::MSLaneState<_T>(string id, MSLane* lane, double begin,
                         double length, MSNet::Time sampleInterval,
                         MSDetector::OutputStyle style,
                         ofstream *file )
    : MSDetector( id, style, file ),
    myLane           ( lane ),
    myPos            ( begin ),
    myLength(length),
    mySampleIntervall( sampleInterval ),
    myPassedVeh      ( 0 ),
    myPassingSpeed   ( 0 ),
    myPassingTime    ( 0 ),
//    myNSamples       ( 0 ),
    myVehicleNo     ( sampleInterval ),
    myLocalDensity( sampleInterval ),
    mySpeed( sampleInterval ),
    myOccup( sampleInterval ),
    myVehLengths( sampleInterval ),
    myNoSlow( sampleInterval )
{
    // Make sure that vehicles will be detected even at lane-end.
//    assert( myPos < myLane->length() - myLane->maxSpeed() * MSNet::deltaT() );
    assert( myPos > 0 );

    // return when just a part of a logic (no file)
    if(file==0) {
        return;
    }
    // Write header.
    switch ( myStyle ) {
        case GNUPLOT:
        {
            ostringstream header;
            header << "# Lane-state-detector ID = " << myID << endl;
            header << "#   on Lane     " << myLane->id() << endl;
            header << "#   at position " << myPos << endl;
            header << "#   sampleIntervall = "
                   << mySampleIntervall << " seconds" << endl << endl;
            header << "# n   endOfInterv nVehicles avgDensity avgFlow "
                   << "avgSpeed avgOccup avgLength" << endl;
            header << "#         [s]                [veh/km]  [veh/h] "
                   << " [m/s]     [s]       [m]" << endl;

            *myFile << header.str() << endl;
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

template<class _T>
void
MSLaneState<_T>::sample( double simSec )
{

    // If sampleIntervall is over, write the data to file and reset the
    // detector.
    ++myNSamples;
    if ( static_cast< double >( myNSamples ) * MSNet::deltaT() >=
         mySampleIntervall ) {

        ++myNIntervalls;
        if(myFile!=0) {
            writeData();
        }
    }

    const MSLane::VehCont &vehs = myLane->getVehiclesSecure();
    MSLane::VehCont::const_iterator firstVehicle;
    if ( myLane->empty() ) {
        // no vehicles on lane
        firstVehicle = vehs.end();
    } else {
        // find the vehicle
        firstVehicle = find_if( vehs.begin(), vehs.end(),
            bind2nd( MSLane::VehPosition(), myPos ) );
    }
    // update interval with zero if no vehicle is on the loop
    if(firstVehicle==vehs.end()) {
        // no vehicle was found
        myVehicleNo.add(0);
        myLocalDensity.add(0);
        mySpeed.add(0);
        myOccup.add(0);
        myVehLengths.add(0);
        myNoSlow.add(0);
        return;
    }
    // We have now a valid beyond the detector.
    MSLane::VehCont::const_iterator lastVehicle =
        find_if( firstVehicle, vehs.end(),
            bind2nd( MSLane::VehPosition(), myPos+myLength ) );
    // go through the vehicles and compute the values
    size_t noVehicles = distance(firstVehicle, lastVehicle);
    double speeds = 0;
    double lengths = 0;
    size_t noSlow = 0;
    for(MSLane::VehCont::const_iterator i=firstVehicle; i!=lastVehicle; i++) {
        MSVehicle *veh = *i;
        double speed = veh->speed();
        speeds += speed;
        if(speed<0.1) {
            noSlow++;
        }
        lengths += veh->length();
    }
    // update values
    myNoSlow.add(noSlow);
    myVehicleNo.add(noVehicles);
    mySpeed.add(speeds / (double) noVehicles);
    myVehLengths.add(lengths / (double) noVehicles);
    if(speeds!=0) {
        myOccup.add( lengths / speeds );
    } else {
        myOccup.add( lengths / 0.000001 );
    }
    // this is just an approximation; The first and te last vehicles
    //  should compete only for the amount of time they are within the
    //  field
    myLocalDensity.add( 1.0 - ((myLength - lengths) / myLength) );
}

//---------------------------------------------------------------------------//

template<class _T>
void
MSLaneState<_T>::writeData()
{
    double avgDensity = 0;
    double avgFlow    = 0;
    double avgSpeed   = 0;
    double avgLength  = 0;

    double NPassedVehicles = myVehicleNo.getAbs();
    if ( NPassedVehicles > 0 ) {

        if ( NPassedVehicles > 1 ) {

            avgDensity = myLocalDensity.getAbs() /
                static_cast< double >( NPassedVehicles - 1 ) * 1000.0;
            avgSpeed  = mySpeed.getAbs() /
	            static_cast< double >( NPassedVehicles ); // [m/s]
            avgLength = myVehLengths.getAbs() /
	            static_cast< double >( NPassedVehicles ); // [m]
	        // [veh/km], first detected vehicle doesn't
	        // contribute.
	        assert( avgDensity > 0 );
        }

        avgFlow   = static_cast< double >( NPassedVehicles ) /
	        static_cast< double >( mySampleIntervall ) * 3600.0; // [veh/h]
    }

    MSNet::Time endOfInterv = myNIntervalls * mySampleIntervall; // [s]

    switch ( myStyle ) {
        case GNUPLOT:
            writeGnuPlot( endOfInterv,
                          avgDensity,
                          avgFlow,
                          avgSpeed,
                          myOccup.getAbs(),
                          avgLength );
            break;
        case CSV:
            writeCSV( endOfInterv,
                      avgDensity,
                      avgFlow,
                      avgSpeed,
                      myOccup.getAbs(),
                      avgLength );
            break;
        default:
            assert( true );
    }
}

//---------------------------------------------------------------------------//

template<class _T>
void
MSLaneState<_T>::writeGnuPlot( MSNet::Time endOfInterv,
                            double avgDensity,
                            double avgFlow,
                            double avgSpeed,
                            double occup,
                            double avgLength )
{
    ( *myFile ).setf( ios::fixed, ios::floatfield );
    *myFile << setw( 5 ) << setprecision( 0 ) << myNIntervalls << " "
            << setw(11 ) << setprecision( 0 ) << endOfInterv << " "
            << setw( 9 ) << setprecision( 0 ) << myVehicleNo.getAbs() << " "
	    << setw(10 ) << setprecision( 2 ) << avgDensity << " "
            << setw( 7 ) << setprecision( 1 ) << avgFlow << " "
            << setw( 8 ) << setprecision( 3 ) << avgSpeed << " "
            << setw( 8 ) << setprecision( 2 ) << occup << " "
            << setw( 9 ) << setprecision( 3 ) << avgLength << endl;
}

//---------------------------------------------------------------------------//

template<class _T>
void
MSLaneState<_T>::writeCSV( MSNet::Time endOfInterv,
                        double avgDensity,
                        double avgFlow,
                        double avgSpeed,
                        double occup,
                        double avgLength )
{
    ( *myFile ).setf( ios::fixed, ios::floatfield );
    *myFile << setw( 4 ) << setprecision( 0 ) << myNIntervalls << ";"
            << setw( 6 ) << setprecision( 0 ) << endOfInterv << ";"
            << setw( 5 ) << setprecision( 0 ) << myVehicleNo.getAbs() << ";"
            << setw( 6 ) << setprecision( 2 ) << avgDensity << ";"
            << setw( 4 ) << setprecision( 1 ) << avgFlow << ";"
            << setw( 6 ) << setprecision( 3 ) << avgSpeed << ";"
            << setw( 7 ) << setprecision( 2 ) << occup << ";"
            << setw( 6 ) << setprecision( 3 ) << avgLength << endl;
}

//---------------------------------------------------------------------------//


//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//

//#ifdef DISABLE_INLINE
//#include "MSLaneState.icc"
//#endif

// Local Variables:
// mode:C++
// End:
