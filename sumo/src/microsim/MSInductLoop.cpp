//---------------------------------------------------------------------------//
//                        MSInductLoop.cpp  -  Simple detector that emulates
//                        induction loops.
//                           -------------------
//  begin                : Thu, 14 Mar 2002
//  copyright            : (C) 2002 by Christian Roessel
//  organisation         : ZAIK http://www.zaik.uni-koeln.de/AFS
//  email                : roessel@zpr.uni-koeln.de
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
// Revision 1.4  2003/03/17 14:14:06  dkrajzew
// Windows eol removed
//
// Revision 1.3  2003/03/03 14:56:19  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:50  dkrajzew
// updated
//
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.11  2002/06/12 10:12:47  croessel
// There was a problem with infinite densities because myPassingSpeed
// could have the value 0 after reinitializing the detector and detection
// of a just emitted vehicle with speed = 0. I added a if (speed>0)
// condition in sample() to detect only the vehicles, that really passed
// the detector (speed > 0).
// Set floating-point output to ios::fixed.
//
// Revision 1.10  2002/06/06 17:50:55  croessel
// Calculation of local density refined.
// New assertions for localDensity.
// avgFlow is now veh/hour.
//
// Revision 1.9  2002/05/16 14:12:42  croessel
// In constructor: Make sure that vehicles will be detected even at lane end.
//
// Revision 1.8  2002/04/18 13:49:29  croessel
// Added some static_casts in writeData. Added the output of the number of
// vehicles that contributed to the sampling.
//
// Revision 1.7  2002/04/17 14:50:36  croessel
// Modified assert in constructor. Added assert in constructor.
//
// Revision 1.6  2002/04/11 16:14:42  croessel
// Moved ofstream myFile from MSInductLoop to MSDetector. Removed double
// declaration of OutputStyle.
//
// Revision 1.5  2002/04/11 15:25:55  croessel
// Changed float to double.
//
// Revision 1.4  2002/04/11 10:07:22  croessel
// #include <cassert> added.
//
// Revision 1.3  2002/04/11 10:04:12  croessel
// Changed myFile-type from reference to pointer.
//
// Revision 1.2  2002/04/10 15:50:55  croessel
// Changeg cless name from MSDetector to MSInductLoop.
//
// Revision 1.1  2002/04/10 15:34:21  croessel
// Renamed MSDetector into MSInductLoop.
//
// Revision 1.2  2002/04/10 15:02:46  croessel
// Get the job done version.
//
// Revision 1.1  2002/03/14 18:48:54  croessel
// Initial commit.
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSInductLoop.h"
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
MSInductLoop<_T>::~MSInductLoop<_T>()
{
    delete myFile;
}

//---------------------------------------------------------------------------//

template<class _T>
MSInductLoop<_T>::MSInductLoop<_T>( string id, MSLane* lane,
                                   double position,
                                   MSNet::Time sampleInterval,
                                   MSDetector::OutputStyle style,
                                   ofstream* file, bool floating) :
    MSDetector( id, style, file, sampleInterval, floating),
    myLane           ( lane ),
    myPos            ( position ),
    mySampleIntervall( sampleInterval ),
    myPassedVeh      ( 0 ),
    myPassingSpeed   ( 0 ),
    myPassingTime    ( 0 ),
//    myNSamples       ( 0 ),
    myNPassedVeh     ( sampleInterval ),
    myLocalDensity( sampleInterval ),
    mySpeed( sampleInterval ),
    myOccup( sampleInterval ),
    myVehLengths( sampleInterval )
{
    // Make sure that vehicles will be detected even at lane-end.
//    assert( myPos < myLane->length() - myLane->maxSpeed() * MSNet::deltaT() );
    assert( myPos > 0 );

    // return when just a part of a logic (no file)
    if(myFile==0) {
        return;
    }
    // Write header.
    switch ( myStyle ) {
        case GNUPLOT:
        {
            ostringstream header;
            header << "# Induction-loop-detector ID = " << myID << endl;
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
MSInductLoop<_T>::sample( double simSec )
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
    MSLane::VehCont::const_iterator currVeh;
    if ( myLane->empty() ) {
        // no vehicles on lane
        currVeh = vehs.end();
    } else {
        // find the vehicle
        currVeh = find_if( vehs.begin(), vehs.end(),
            bind2nd( MSLane::VehPosition(), myPos ) );
        // do not use a vehicle twice
        if ( currVeh!=vehs.end() && *currVeh == myPassedVeh ) {
            currVeh = vehs.end();
        }
    }
    // update values
    if(currVeh==vehs.end()) {
        // no vehicle was found
        myNPassedVeh.add(0);
        myLocalDensity.add(0);
        mySpeed.add(0);
        myOccup.add(0);
        myVehLengths.add(0);
        return;
    }
    // We have now a valid beyond the detector. If its speed is > 0 it
    // had passed the detector and should be sampled, otherwise it
    // might be just emitted and should not be sampled.
    if ( ( *currVeh )->speed() > 0 ) {

	    myLocalDensity.add(localDensity( **currVeh, simSec ));
        double speed = ( *currVeh )->speed();
	    mySpeed.add(speed);
        if(speed!=0) {
	        myOccup.add(( *currVeh )->length() / speed);
        } else {
            // lets make it great enough
	        myOccup.add(( *currVeh )->length() / 0.00001);
        }
	    myVehLengths.add(( *currVeh )->length());

        // Remember the vehicle.
	    myPassedVeh = *currVeh;
	    myNPassedVeh.add(1);
    }
}

//---------------------------------------------------------------------------//

template<class _T>
double
MSInductLoop<_T>::localDensity( const MSVehicle& veh, double simSec )
{
    assert( myPassingTime <= simSec );

    // Local Density is calculated via the timeheadway and the speed
    // of the leading vehicle. After the first detection there will
    // always be a myPassedVeh.
    double localDens    = 0.0;
    double currPassTime = 0.0;

    if ( myPassedVeh != 0 ) {

	    assert( veh.speed() > 0 );

        currPassTime = simSec - ( veh.pos() - myPos ) / veh.speed();
	    double timeHeadWay = currPassTime - myPassingTime;
	    localDens = 1 / ( myPassingSpeed * timeHeadWay );

	    assert( localDens >= 0 );
    }

    // update members
    myPassingSpeed = veh.speed();
    if ( myPassedVeh != 0 ) {
	    myPassingTime  = currPassTime;
    }
    else {
	    myPassingTime  = simSec - ( veh.pos() - myPos ) / veh.speed();
    }
    return localDens;
}

//---------------------------------------------------------------------------//

template<class _T>
void
MSInductLoop<_T>::writeData()
{
    double avgDensity = 0;
    double avgFlow    = 0;
    double avgSpeed   = 0;
    double avgLength  = 0;

    double NPassedVehicles = myNPassedVeh.getAbs();
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
MSInductLoop<_T>::writeGnuPlot( MSNet::Time endOfInterv,
                            double avgDensity,
                            double avgFlow,
                            double avgSpeed,
                            double occup,
                            double avgLength )
{
    ( *myFile ).setf( ios::fixed, ios::floatfield );
    *myFile << setw( 5 ) << setprecision( 0 ) << myNIntervalls << " "
            << setw(11 ) << setprecision( 0 ) << endOfInterv << " "
            << setw( 9 ) << setprecision( 0 ) << myNPassedVeh.getAbs() << " "
	    << setw(10 ) << setprecision( 2 ) << avgDensity << " "
            << setw( 7 ) << setprecision( 1 ) << avgFlow << " "
            << setw( 8 ) << setprecision( 3 ) << avgSpeed << " "
            << setw( 8 ) << setprecision( 2 ) << occup << " "
            << setw( 9 ) << setprecision( 3 ) << avgLength << endl;
}

//---------------------------------------------------------------------------//

template<class _T>
void
MSInductLoop<_T>::writeCSV( MSNet::Time endOfInterv,
                        double avgDensity,
                        double avgFlow,
                        double avgSpeed,
                        double occup,
                        double avgLength )
{
    ( *myFile ).setf( ios::fixed, ios::floatfield );
    *myFile << setw( 4 ) << setprecision( 0 ) << myNIntervalls << ";"
            << setw( 6 ) << setprecision( 0 ) << endOfInterv << ";"
            << setw( 5 ) << setprecision( 0 ) << myNPassedVeh.getAbs() << ";"
            << setw( 6 ) << setprecision( 2 ) << avgDensity << ";"
            << setw( 4 ) << setprecision( 1 ) << avgFlow << ";"
            << setw( 6 ) << setprecision( 3 ) << avgSpeed << ";"
            << setw( 7 ) << setprecision( 2 ) << occup << ";"
            << setw( 6 ) << setprecision( 3 ) << avgLength << endl;
}



template<class _T>
MSNet::Time
MSInductLoop<_T>::getLastVehicleTime() const
{
    return myPassingTime;
}

//---------------------------------------------------------------------------//

//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//

//#ifdef DISABLE_INLINE
//#include "MSInductLoop.icc"
//#endif

// Local Variables:
// mode:C++
// End:
