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
// Revision 1.10  2003/05/26 15:31:09  roessel
// Removed superflous #include "MSLane.h"
//
// Revision 1.9  2003/05/26 15:24:15  roessel
// Removed warnings/errors. Changed return-type of getNumberOfWaiting to
// double.
//
// Revision 1.8  2003/05/26 13:19:20  roessel
// Completed all get* methods.
//
// Revision 1.7  2003/05/25 17:50:31  roessel
// Implemented getCurrentNumberOfWaiting.
// Added methods actionBeforeMove and actionAfterMove. actionBeforeMove creates
// a TimestepData entry in timestepDataM every timestep (makes live easier).
// actionAfterMove calculates the waitingQueueLength and updates the current
// TimestepData.
// These two methods must be called in the simulation loop.
//
// Revision 1.6  2003/05/23 16:42:22  roessel
// Added method getCurrentDensity().
//
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
class MSLane;


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
    double getNumberOfWaiting( MSNet::Time lastNTimesteps );

    /** 
     * Returns the waitingQueueLength.
     * Vehicles in a waiting-queue have a gap <= vehLength.
     * If called before the vehicles are
     * moved, the value of the previous timestep is returned, if called
     * after move, the value of the current timestep is returned. Currently
     * the junctions do their job before the move, so if you call the method
     * by a junction, you will get the value of the previous timestep.
     * 
     * @return waitingQueueLength of the current or previous timestep
     */
    int getCurrentNumberOfWaiting( void );

    double getMeanSpeed( MSNet::Time lastNTimesteps );

    double getCurrentMeanSpeed( void );

    double getMeanSpeedSquare( MSNet::Time lastNTimesteps );

    double getCurrentMeanSpeedSquare( void );

    double getMeanDensity( MSNet::Time lastNTimesteps );

    double getCurrentDensity( void );

    double getMeanTraveltime( MSNet::Time lastNTimesteps );


    void addMoveData( MSVehicle& veh, double timestepFraction );

    void enterDetectorByMove( MSVehicle& veh, double enterTimestepFraction );

    void enterDetectorByEmitOrLaneChange( MSVehicle& veh );

    void leaveDetectorByMove( MSVehicle& veh, double leaveTimestepFraction );

    void leaveDetectorByLaneChange( MSVehicle& veh );

    void actionBeforeMove( void );

    void actionAfterMove( void );

    MSNet::Time deleteOldData( void );

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

    struct TimestepData;
    struct WaitingQueueElem;
    struct VehicleData;
    typedef std::deque< TimestepData >           TimestepDataCont;
    typedef std::vector< WaitingQueueElem >      WaitingQueueElemCont;
    typedef std::map< std::string, VehicleData > VehicleDataMap;
    typedef std::deque< VehicleData >            VehicleDataCont;

    struct TimestepData
    {
        TimestepData( MSNet::Time timestep ) :
            timestepM( timestep ),
            speedSumM(0),
            speedSquareSumM(0),
            contTimestepSumM(0),
            timestepSumM(0),
            queueLengthM(-1)
            {}

        MSNet::Time timestepM;
        double speedSumM;
        double speedSquareSumM;
        double contTimestepSumM;
        double timestepSumM;
        int queueLengthM;
    };

    struct VehicleData
    {
        struct leaveTimestepLesser :
            public std::binary_function< VehicleData, double, bool >
        {
            bool operator()( const VehicleData& data,
                             double leaveTimestep ) const
                {
                    return data.leaveContTimestepM < leaveTimestep;
                }
        };
        
        VehicleData( double entryContTimestep,
                     bool enteredDetectorByMove ) :
            entryContTimestepM ( entryContTimestep ),
            leaveContTimestepM ( -1 ),
            passedEntireDetectorM ( enteredDetectorByMove )
            {}

        double entryContTimestepM;
        double leaveContTimestepM;
        bool passedEntireDetectorM;
    };

    struct WaitingQueueElem 
    {
        struct PosGreater : public std::binary_function<
            const WaitingQueueElem, const WaitingQueueElem, bool >
        {
            // Sort criterion for std::vector< WaitingQueueElem >
            // We sort in descending order
            bool operator() ( const WaitingQueueElem p1,
                              const WaitingQueueElem p2 ) const {
                return p1.posM > p2.posM;
            }
        };
        
        WaitingQueueElem( double pos, double vehLength ) :
            posM( pos ), vehLengthM( vehLength )
            {}

        double posM;
        double vehLengthM;
    };

protected:
 
    void calcWaitingQueueLength( void );
    
    TimestepDataCont::iterator getStartIterator( MSNet::Time lastNTimesteps );
    
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

private:
    std::string idM;

    OutputStyle styleM;

    /// File where output goes to.
    std::ofstream* fileM;
    
    TimestepDataCont     timestepDataM;
    VehicleDataMap       vehicleDataM;
    WaitingQueueElemCont waitingQueueElemsM;
    VehicleDataCont      vehLeftLaneM;

    /// Lane where detector works on.
    MSLane* laneM;

    /// The begin on the lane
    double posM;

    /// The length on the lane
    double lengthM;

    /// Number of already processed sampleIntervalls
    unsigned nIntervallsM;

    MSNet::Time sampleIntervalM;

    MSMoveReminder* reminderM;

    const MSNet::Time deleteDataAfterSecondsM;

    /// Default constructor.
    MSLaneState();

    /// Copy constructor.
    MSLaneState( const MSLaneState& );

    /// Assignment operator.
    MSLaneState& operator=( const MSLaneState& );
};

inline double speedSum( double sumSoFar,
                 const MSLaneState::TimestepData& data ) 
{
    return sumSoFar + data.speedSumM;
}

inline double speedSquareSum( double sumSoFar,
                       const MSLaneState::TimestepData& data ) 
{
    return sumSoFar + data.speedSquareSumM;
}

inline double contTimestepSum( double sumSoFar,
                        const MSLaneState::TimestepData& data ) 
{
    return sumSoFar + data.contTimestepSumM;
}

inline double timestepSum( double sumSoFar,
                    const MSLaneState::TimestepData& data ) 
{
    return sumSoFar + data.timestepSumM;
}

inline double waitingQueueSum( double sumSoFar,
                        const MSLaneState::TimestepData& data ) 
{
    return sumSoFar + data.queueLengthM;
}
    
inline double traveltimeSum( double sumSoFar,
                      const MSLaneState::VehicleData& data )
{
    if ( data.passedEntireDetectorM ) {
        return sumSoFar + data.leaveContTimestepM - data.entryContTimestepM;
    }
    return sumSoFar;
}
         
#endif

// Local Variables:
// mode:C++
// End:
