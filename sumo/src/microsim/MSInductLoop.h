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
// Revision 1.6  2003/05/20 09:31:46  dkrajzew
// emission debugged; movement model reimplemented (seems ok); detector output debugged; setting and retrieval of some parameter added
//
// Revision 1.5  2003/04/02 11:44:03  dkrajzew
// continuation of implementation of actuated traffic lights
//
// Revision 1.4  2003/03/17 14:14:07  dkrajzew
// Windows eol removed
//
// Revision 1.3  2003/03/03 14:56:20  dkrajzew
// some debugging; new detector types added; actuated traffic lights added
//
// Revision 1.2  2003/02/07 10:41:51  dkrajzew
// updated
//
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

/* =========================================================================
 * included modules
 * ======================================================================= */
#include "MSNet.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include "MSDetector.h"
#include <utils/logging/LoggedValue.h>
#include <string>
#include <functional>


/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * @class MSInductLoop
 * This detector is something like a virtual double - induct loop. It notifies
 * vehicles running over it and saves their values. This detector has no
 * size.
 */
template<class _T>
class MSInductLoop
    : public MSDetector
{
public:

    /** Constructor: InductLoop detects on lane at position pos. He collects
        during samplIntervall seconds data and writes them in style to file.
     */
    MSInductLoop( std::string id, MSLane* lane, double position,
        MSNet::Time sampleInterval, MSDetector::OutputStyle style,
        std::ofstream* file, bool floating);

    /// Destructor.
    ~MSInductLoop();

    /// Call sample every timestep to update the detector.
    void sample( double currSimSeconds );

    /// Returns the time the last vehicle was at the loop
    MSNet::Time getLastVehicleTime() const;

    /// Returns the timedifference the last vehicle was at the loop and the actual second
    double getGap() const;


protected:
    /// Increments the local density.
//    double localDensity( const MSVehicle& veh, double currSimSeconds );

    /// Write the data according to OutputStyle when the sampleIntervall is over.
    void writeData();

    /// Write in gnuplot-style to myFile.
    void writeGnuPlot( MSNet::Time endOfInterv,
                       double avgFlow,
                       double avgSpeed,
                       double Occup,
                       double avgLength );

    /// Write in CSV-style to myFile.
    void writeCSV( MSNet::Time endOfInterv,
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


//     /// The leader of the vehicle that passed the detector. We need this if
//     /// a vehicle occupies the detector several timesteps.
//     MSVehicle* myPassedVehLeader;

    /// Speed of the last vehicle, that has passed the detector.
    double myPassingSpeed;

//     /// Speed of the Leader of the vehicle that passed the detector
//     double myPassingVehLeaderSpeed;

    /// Time when last vehicle has passed the detector.
    double myPassingTime;

//     /// Time when the leader of the vehicle that passed the detector.
//     double myPassingVehLeaderTime;

    /// Number of finished sampleIntervalls.
    unsigned myNSamples;

    /** Number of vehicles which have already passed the detector */
    unsigned myNPassedVeh;

    /// local-densities sampled
//    _T myLocalDensity;

    /// Speeds sampled
    _T mySpeed;

    /// Occupancy-times sampled
    _T myOccup;

    /// Veh-lengthes
    _T myVehLengths;

    /// Number of already processed sampleIntervalls
    unsigned myNIntervalls;

private:
    /// Default constructor.
    MSInductLoop();

    /// Copy constructor.
    MSInductLoop( const MSInductLoop& );

    /// Assignment operator.
    MSInductLoop& operator=( const MSInductLoop& );
};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "MSInductLoop.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION




//----------- DO NOT DECLARE OR DEFINE ANYTHING AFTER THIS POINT ------------//

//#ifndef DISABLE_INLINE
//#include "MSInductLoop.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:
