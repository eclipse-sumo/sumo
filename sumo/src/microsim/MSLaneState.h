#ifndef MSLaneState_H
#define MSLaneState_H

//---------------------------------------------------------------------------//
//                        MSLaneState.h  -
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
#include "MSNet.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSDetector.h"
#include <string>
#include <functional>


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * @class MSLaneState
 */
template<class _T>
class MSLaneState : public MSDetector
{
public:

    /** Constructor: InductLoop detects on lane at position pos. He collects
        during samplIntervall seconds data and writes them in style to file.
     */
    MSLaneState( std::string    id,
                  MSLane*        lane,
                  double         begin,
                  double         length,
                  MSNet::Time    sampleIntervall,
                  MSDetector::OutputStyle style,
                  std::ofstream* file );

    /// Destructor.
    ~MSLaneState();

    /// Call sample every timestep to update the detector.
    void sample( double currSimSeconds );

    /** Function-object in order to find the vehicle, that has just
        passed the detector. */
    struct VehPosition : public std::binary_function< const MSVehicle*,
                         double, bool >
    {
        /// compares vehicle position to the detector position
        bool operator() ( const MSVehicle* cmp, double pos ) const {
            return cmp->pos() > pos;
        }
    };

protected:
    /// Write the data according to OutputStyle when the sampleIntervall is over.
    void writeData();

    /// Write in gnuplot-style to myFile.
    void writeGnuPlot( MSNet::Time endOfInterv,
                       double avgDensity,
                       double avgFlow,
                       double avgSpeed,
                       double Occup,
                       double avgLength );

    /// Write in CSV-style to myFile.
    void writeCSV( MSNet::Time endOfInterv,
                   double avgDensity,
                   double avgFlow,
                   double avgSpeed,
                   double Occup,
                   double avgLength );

private:
    /// Lane where detector works on.
    MSLane* myLane;

    /// The begin on the lane
    double myPos;

    /// The length on the lane
    double myLength;

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

    /** Number of vehicles which have already passed the detector */
    _T myVehicleNo;

    /// local-densities sampled
    _T myLocalDensity;

    /// Speeds sampled
    _T mySpeed;

    /// Occupancy-times sampled
    _T myOccup;

    /// Veh-lengths
    _T myVehLengths;

    /// The number of vehicles which are slower than 0.1 m/s
    _T myNoSlow;

    /// Number of already processed sampleIntervalls
    unsigned myNIntervalls;

    /** @brief The information in which time step the detector was asked the last time
        Needed to reduce the update frequency of the iterators */
    MSNet::Time myLastUpdateTime;

private:
    /// Default constructor.
    MSLaneState();

    /// Copy constructor.
    MSLaneState( const MSLaneState& );

    /// Assignment operator.
    MSLaneState& operator=( const MSLaneState& );
};


#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "MSLaneState.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION

//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "MSLaneState.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
