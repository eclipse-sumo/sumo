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
// Revision 1.5  2003/05/21 16:20:44  dkrajzew
// further work detectors
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
#include "MSNet.h"
#include "MSVehicle.h"
#include "MSLane.h"
#include <string>
#include <fstream>
#include <functional>
#include <deque>
#include <map>

/* =========================================================================
 * class declarations
 * ======================================================================= */
/**
 * @class MSLaneState
 */

class MSMoveReminder;


class MSLaneState
{
public:
    /** We support two output-styles, gnuplot and "Comma Separated Variable"
        (CSV). */
    enum OutputStyle { GNUPLOT, CSV };

    /** Constructor: InductLoop detects on lane at position pos. He collects
        during samplIntervall seconds data and writes them in style to file.
     */
    MSLaneState( std::string     id,
                  MSLane*        lane,
                  double         begin,
                  double         length,
                  MSNet::Time    sampleIntervall,
                  OutputStyle    style,
                  std::ofstream* file );

    /// Destructor.
    ~MSLaneState();

    /**
     * Calculates the meanValue of the waiting-queue length during the
     * lastNTimesteps. Vehicles in a waiting-queue have a gap <= vehLength.
     *
     * @param lastNTimesteps consider data out of the intervall
     * [now-lastNTimesteps, now]
     *
     * @return mean waiting-queue length
     */
    int getNumberOfWaiting( MSNet::Time lastNTimesteps );

    int getCurrentNumberOfWaiting( void );

    double getMeanSpeed( MSNet::Time lastNTimesteps );

    double getCurrentMeanSpeed( void );

    double getMeanSpeedSquare( MSNet::Time lastNTimesteps );

    double getCurrentMeanSpeedSquare( void );

    double getMeanDensity( MSNet::Time lastNTimesteps );

    double getMeanTraveltime( MSNet::Time lastNTimesteps );


    void addMoveData( MSVehicle& veh, double timestepFraction );

    void enterDetectorByMove( MSVehicle& veh, double enterTimestepFraction );

    void enterDetectorByEmitOrLaneChange( MSVehicle& veh );

    void leaveDetectorByMove( MSVehicle& veh, double leaveTimestepFraction );

    void leaveDetectorByLaneChange( MSVehicle& veh );

    void calcWaitingQueueLength( void );

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
    /// Write the data according to OutputStyle when the sampleIntervall
    /// is over.
    MSNet::Time writeData();

    /// Write in gnuplot-style to myFile.
    void writeGnuPlot( MSNet::Time endOfInterv,
                       double avgDensity,
                       double avgSpeed,
                       double avgSpeedSquare,
                       double avgTraveltime,
                       int avgNumberOfWaiting );

    /// Write in CSV-style to myFile.
    void writeCSV( MSNet::Time endOfInterv,
                   double avgDensity,
                   double avgSpeed,
                   double avgSpeedSquare,
                   double avgTraveltime,
                   int avgNumberOfWaiting );


    struct TimestepData
    {
        TimestepData() :
            speedSumM(0),
            speedSquareSumM(0),
            contTimestepSumM(0),
            timestepSumM(0),
            queueLengthM(0)
            {}

        MSNet::Time timestepM;
        double speedSumM;
        double speedSquareSumM;
        double contTimestepSumM;
        int timestepSumM;
        int queueLengthM;
    };

    struct VehicleData
    {
        VehicleData( double entryContTimestep,
                     bool enteredDetectorByMove ) :
            entryContTimestepM ( entryContTimestep ),
            leaveContTimestepM ( -1 ),
            entireDetectorM ( enteredDetectorByMove )
            {}

        double entryContTimestepM;
        double leaveContTimestepM;
        bool entireDetectorM;
    };

    struct WaitingQueueElem
    {
        WaitingQueueElem( double pos, double vehLength ) :
            posM( pos ), vehLengthM( vehLength )
            {}

        double posM;
        double vehLengthM;
    };

private:
    std::string idM;

    OutputStyle styleM;

    /// File where output goes to.
    std::ofstream* fileM;

    std::deque< TimestepData > timestepDataM;
    std::map< std::string, VehicleData > vehicleDataM;
    std::vector< WaitingQueueElem > waitingQueueElemsM;
    std::deque< VehicleData > vehLeftLaneM;

    struct WaitingQueuePos : public std::binary_function<
        const WaitingQueueElem, const WaitingQueueElem, bool >
    {
        // Sort criterion for std::vector< WaitingQueueElem >
        // We sort in descending order
        bool operator() ( const WaitingQueueElem p1,
                          const WaitingQueueElem p2 ) const {
            return p1.posM > p2.posM;
        }
    };

    /// Lane where detector works on.
    MSLane* laneM;

    /// The begin on the lane
    double posM;

    /// The length on the lane
    double lengthM;

    /// Number of already processed sampleIntervalls
    unsigned nIntervallsM;

    MSNet::Time sampleIntervalM;

    bool createdCurrentTimestepDataM;

    MSMoveReminder* reminderM;

//     /// Number of finished sampleIntervalls.
//     unsigned myNSamples;

//     /// Sample-intervall in seconds.
//     MSNet::Time mySampleIntervall;

//     /// Last vehicle that passed the detector.
//     MSVehicle* myPassedVeh;

//     /// Speed of the last vehicle, that has passed the detector.
//     double myPassingSpeed;

//     /// Time when last vehicle has passed the detector.
//     double myPassingTime;

//     /** Number of vehicles which have already passed the detector */
//     _T myVehicleNo;

//     /// local-densities sampled
//     _T myLocalDensity;

//     /// Speeds sampled
//     _T mySpeed;

//     /// Occupancy-times sampled
//     _T myOccup;

//     /// Veh-lengths
//     _T myVehLengths;

//     /// The number of vehicles which are slower than 0.1 m/s
//     _T myNoSlow;

//     /** @brief The information in which time step the detector was asked the last time
//         Needed to reduce the update frequency of the iterators */
//     MSNet::Time myLastUpdateTime;

    /// Default constructor.
    MSLaneState();

    /// Copy constructor.
    MSLaneState( const MSLaneState& );

    /// Assignment operator.
    MSLaneState& operator=( const MSLaneState& );
};


#endif

// Local Variables:
// mode:C++
// End:
