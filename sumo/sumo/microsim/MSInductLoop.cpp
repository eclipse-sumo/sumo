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

namespace
{
    const char rcsid[] =
    "$Id$";
}

// $Log$
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


#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSInductLoop.h"
#include "MSLane.h"
#include "MSEdge.h"
#include <sstream>
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <cassert>

using namespace std;

//---------------------------------------------------------------------------//

MSInductLoop::~MSInductLoop()
{
    delete myFile;
}

//---------------------------------------------------------------------------//

MSInductLoop::MSInductLoop( string         id,
                            MSLane*        lane,
                            double          position,
                            MSNet::Time    sampleIntervall,
                            MSDetector::OutputStyle    style,
                            ofstream*      file ) :
    MSDetector( id, style ),
    myLane           ( lane ),
    myPos            ( position ),
    mySampleIntervall( sampleIntervall ),
    myFile           ( file ),
    myPassedVeh      ( 0 ),
    myPassingSpeed   ( 0 ),
    myPassingTime    ( 0 ),
    myNSamples       ( 0 ),
    myNPassedVeh     ( 0 ),
    myLocalDensitySum( 0 ),
    mySpeedSum       ( 0 ),
    myOccupSum       ( 0 ),
    myVehLengthSum   ( 0 ),
    myNIntervalls    ( 0 )
{
    assert( myPos > myLane->length() );

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
            header << "# n   endOfInterv avgDensity avgFlow avgSpeed "
                   << "avgOccup avgLength" << endl;
            header << "#         [s]      [veh/km]  [veh/h]   [m/s]  "
                   << "   [s]      [m]" << endl;

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

void
MSInductLoop::sample( double simSec )
{
    // If sampleIntervall is over, write the data to file and reset the 
    // detector.
    ++myNSamples;
    if ( static_cast< double >( myNSamples ) * MSNet::deltaT() >= 
         mySampleIntervall ) {

        writeData();

        // Reinitialize the detector completely.
        myPassedVeh       = 0;
        myPassingSpeed    = 0;
        myPassingTime     = 0;
        myNSamples        = 0;
        myNPassedVeh      = 0;
        myLocalDensitySum = 0;
        mySpeedSum        = 0;
        myOccupSum        = 0;
        myVehLengthSum    = 0;
        
        ++myNIntervalls;
    }

    if ( myLane->empty() ) {

        return;
    }

    // Search the vehicle that is closest to the detector with vehPos >
    // detectorPos (the vehicle already passed the detector).
    MSLane::VehCont::iterator currVeh =
        find_if( myLane->myVehicles.begin(),
                 myLane->myVehicles.end(),
                 bind2nd( VehPosition(), myPos ) );

    if ( currVeh == myLane->myVehicles.end() ||
         *currVeh == myPassedVeh ) {

        return;
    }

    // We have now a valid and not sampled vehicle. Update all data.
    myLocalDensitySum += localDensity( **currVeh, simSec );
    mySpeedSum        += ( *currVeh )->speed();
    if ( ( *currVeh )->speed() > 0 ) {
        myOccupSum += ( *currVeh )->length() / ( *currVeh )->speed();
    }
    myVehLengthSum += ( *currVeh )->length();
    
    // Remember the vehicle.
    myPassedVeh = *currVeh;
    ++myNPassedVeh;
}

//---------------------------------------------------------------------------//

double 
MSInductLoop::localDensity( const MSVehicle& veh, double simSec )
{
    // Local Density is calculated via the timeheadway and the speed
    // of the leading vehicle. After the first detection there will
    // always be a myPassedVeh.
    double localDens = 0.0;
    double currPassTime = 0.0;

    if ( myPassedVeh != 0 ) {

        currPassTime = simSec + ( veh.pos() - myPos ) / veh.speed();
        double timeHeadWay = currPassTime - myPassingTime;
        localDens = 1 / ( myPassingSpeed * timeHeadWay );
        assert( localDens >= 0 );
    }

    // update members
    myPassingSpeed = veh.speed();
    if ( myPassingTime == 0.0 ) {
        myPassingTime = simSec + ( veh.pos() - myPos ) / veh.speed();
    }

    return localDens;
}

//---------------------------------------------------------------------------//


void   
MSInductLoop::writeData()
{

    double avgDensity = myLocalDensitySum / ( myNPassedVeh - 1 ) 
        * 1000; // [veh/km], first detectect vehicle doesn't contribute.
    double avgFlow    = myNPassedVeh / mySampleIntervall * 60; // [veh/h]
    double avgSpeed   = mySpeedSum / myNPassedVeh;  // [m/s]
    double avgLength  = myVehLengthSum / myNPassedVeh; // [m]
//    double avgOccup = myOccupancySum / myNPassedVeh;

    switch ( myStyle ) {
        case GNUPLOT: 
            writeGnuPlot( myNIntervalls * mySampleIntervall,
                          avgDensity,
                          avgFlow,
                          avgSpeed,
                          myOccupSum,
                          avgLength );
            break;
        case CSV:
            writeCSV( myNIntervalls * mySampleIntervall,
                      avgDensity,
                      avgFlow,
                      avgSpeed,
                      myOccupSum,
                      avgLength );
            break;
        default:
            assert( true );
    }
}

//---------------------------------------------------------------------------//

void
MSInductLoop::writeGnuPlot( MSNet::Time endOfInterv,
                            double avgDensity,
                            double avgFlow,
                            double avgSpeed,
                            double occup,
                            double avgLength )
{
    *myFile << setw( 5 ) << setprecision( 0 ) << myNIntervalls << " "
            << setw(11 ) << setprecision( 0 ) << endOfInterv << " "
            << setw(10 ) << setprecision( 2 ) << avgDensity << " "
            << setw( 7 ) << setprecision( 1 ) << avgFlow << " "
            << setw( 8 ) << setprecision( 3 ) << avgSpeed << " "
            << setw( 8 ) << setprecision( 2 ) << occup << " "
            << setw( 9 ) << setprecision( 3 ) << avgLength << endl;
}

//---------------------------------------------------------------------------//

void
MSInductLoop::writeCSV( MSNet::Time endOfInterv,
                        double avgDensity,
                        double avgFlow,
                        double avgSpeed,
                        double occup,
                        double avgLength )
{
    *myFile << setw( 4 ) << setprecision( 0 ) << myNIntervalls << ";"
            << setw( 6 ) << setprecision( 0 ) << endOfInterv << ";"
            << setw( 6 ) << setprecision( 2 ) << avgDensity << ";"
            << setw( 4 ) << setprecision( 1 ) << avgFlow << ";"
            << setw( 6 ) << setprecision( 3 ) << avgSpeed << ";"
            << setw( 7 ) << setprecision( 2 ) << occup << ";"
            << setw( 6 ) << setprecision( 3 ) << avgLength << endl;
}

//---------------------------------------------------------------------------//

//--------------- DO NOT DEFINE ANYTHING AFTER THIS POINT -------------------//

//#ifdef DISABLE_INLINE
//#include "MSInductLoop.icc"
//#endif

// Local Variables:
// mode:C++
// End:







