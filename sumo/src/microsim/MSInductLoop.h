#ifndef MSInductLoop_H
#define MSInductLoop_H

//---------------------------------------------------------------------------//
//                        MSInductLoop.h  -  Simple detector that emulates 
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

// $Log$
// Revision 1.1  2002/10/16 14:48:26  dkrajzew
// ROOT/sumo moved to ROOT/src
//
// Revision 1.6  2002/07/31 17:33:00  roessel
// Changes since sourceforge cvs request.
//
// Revision 1.6  2002/07/24 16:28:17  croessel
// Made function-object VehPosition public, so that in can be used be
// other classes.
//
// Revision 1.5  2002/04/11 16:14:42  croessel
// Moved ofstream myFile from MSInductLoop to MSDetector. Removed double
// declaration of OutputStyle.
//
// Revision 1.4  2002/04/11 15:25:55  croessel
// Changed float to double.
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
// Revision 1.2  2002/04/10 15:16:15  croessel
// Get the job done version.
//
// Revision 1.2  2002/03/27 17:56:05  croessel
// Updated version.
//
// Revision 1.1  2002/03/14 18:48:54  croessel
// Initial commit.
//


#include "MSNet.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSDetector.h"
#include <string>
#include <functional>

/**
 */
class MSInductLoop : public MSDetector
{
public:

    /** We support two output-styles, gnuplot and "Comma Separated Variable" 
        (CSV). */
    enum OutputStyle { GNUPLOT, CSV };

    /** Constructor: InductLoop detects on lane at position pos. He collects
        during samplIntervall seconds data and writes them in style to file.
     */ 
    MSInductLoop( std::string    id,
                  MSLane*        lane,
                  double         position,
                  MSNet::Time    sampleIntervall,
                  MSDetector::OutputStyle style,
                  std::ofstream* file );
    
    /// Destructor.
    ~MSInductLoop();

    /// Call sample every timestep to update the detector.
    void sample( double currSimSeconds );

    // Function-object in order to find the vehicle, that has just
    // passed the detector.
    struct VehPosition : public std::binary_function< const MSVehicle*,
                         double, bool > 
    {
        bool operator() ( const MSVehicle* cmp, double pos ) const {
            return cmp->pos() > pos;
        }
    };
    
protected:
    // Add up the local density.
    double localDensity( const MSVehicle& veh, double currSimSeconds );

    // Write the data according to OutputStyle when the
    // sampleIntervall is over.
    void writeData();

    // Write in gnuplot-style to myFile.
    void writeGnuPlot( MSNet::Time endOfInterv,
                       double avgDensity,
                       double avgFlow,
                       double avgSpeed,
                       double Occup,
                       double avgLength );

    // Write in CSV-style to myFile.
    void writeCSV( MSNet::Time endOfInterv,
                   double avgDensity,
                   double avgFlow,
                   double avgSpeed,
                   double Occup,
                   double avgLength );

private:

    /// Lane where detector works on.
    MSLane* myLane;

    /// InductLoops position on myLane.
    double myPos;

    /// Sample-intervall in seconds.
    MSNet::Time mySampleIntervall;

    /// Last vehicle that passed the detector.
    MSVehicle* myPassedVeh;
    
    /// Speed of the last vehicle, that has passed the detector.
    double myPassingSpeed;
    
    /// Time when last vehicle has passed the detector.
    double myPassingTime;

    /// Number of finished sampleIntervalls.
    unsigned myNSamples;

    /** Number of vehicles which have already passed the detector in
        this sample intervall. */    
    unsigned myNPassedVeh;

    /// Sum of local-densities sampled during the current sample-intervall.
    double myLocalDensitySum;
    
    /// Sum of speeds sampled during the current sample-intervall.
    double mySpeedSum;

    /// Sum of occupancy-times sampled during the current sample-intervall.
    double myOccupSum;

    /// Sum of veh-lengthes sampled during the current sample-intervall.
    double myVehLengthSum;

    // Number of already processed sampleIntervalls
    unsigned myNIntervalls;

    /// Default constructor.
    MSInductLoop();
    
    /// Copy constructor.
    MSInductLoop( const MSInductLoop& );
    
    /// Assignment operator.
    MSInductLoop& operator=( const MSInductLoop& );     
};


//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "MSInductLoop.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:










