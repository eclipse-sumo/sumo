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
// Revision 1.14  2003/06/05 09:53:46  roessel
// Numerous changes and new methods/members.
//
// Revision 1.13  2003/05/28 15:36:32  roessel
// Added argument MSNet::Time deleteDataAfterSeconds with default value 900
// timesteps to constructor.
//
// Revision 1.12  2003/05/28 07:51:25  dkrajzew
// had to add a return value due to the usage of the mem_func-function in
// combination with for_each (MSVC++-reasons?)
//
// Revision 1.11  2003/05/27 19:01:26  roessel
// Removed OutputStyle in ctor (output will be xml).
//
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
#include <string>
#include <functional>
#include <deque>
#include <map>
#include <iostream>

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
    /** Constructor: InductLoop detects on lane at position pos. He collects
        during samplIntervall seconds data and writes them in style to file.
     */
    MSLaneState( std::string    id,
                 MSLane*        lane,
                 double         begin,
                 double         length,
                 MSNet::Time    deleteDataAfterSeconds = 900 );

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

    int getNVehContributed( MSNet::Time lastNTimesteps );

    int getNVehEnteredDetector( MSNet::Time lastNTimesteps );

    int getNVehLeftDetectorByMove( MSNet::Time lastNTimesteps );

    int getNVehPassedEntireDetector( MSNet::Time lastNTimesteps );

    static std::string& getXMLHeader( void );
    
    std::string getXMLOutput( MSNet::Time lastNTimesteps );
    
    void addMoveData( MSVehicle& veh, double newSpeed,
                      double timestepFraction );

    void enterDetectorByMove( MSVehicle& veh, double enterTimestepFraction );

    void enterDetectorByEmitOrLaneChange( MSVehicle& veh );

    void leaveDetectorByMove( MSVehicle& veh, double leaveTimestepFraction );

    void leaveDetectorByLaneChange( MSVehicle& veh );

    static void actionsBeforeMoveAndEmit( void );

    static void actionsAfterMoveAndEmit( void );

    static std::string getNamePrefix( void );


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
            queueLengthM(-1),
            nVehEnteredDetectorM(0)
            {}

        MSNet::Time timestepM;
        double speedSumM;
        double speedSquareSumM;
        double contTimestepSumM;
        double timestepSumM;
        int queueLengthM;
        int nVehEnteredDetectorM;
    };

    struct VehicleData
    {
        struct entryTimestepLesser :
            public std::binary_function< VehicleData, double, bool >
        {
            bool operator()( const VehicleData& data,
                             double entryTimestepBound ) const
                {
                    return data.entryContTimestepM < entryTimestepBound;
                }
        };
        
        struct entryTimestepLesserMap :
            public std::binary_function< VehicleDataMap::value_type,
            double, bool >
        {
            bool operator()( const VehicleDataMap::value_type& data,
                             double entryTimestepBound ) const
                {
                    return data.second.entryContTimestepM < entryTimestepBound;
                }
        };

        struct leaveTimestepLesser :
            public std::binary_function< VehicleData, double, bool >
        {
            bool operator()( const VehicleData& data,
                             double leaveTimestepBound ) const
                {
                    return data.leaveContTimestepM < leaveTimestepBound;
                }
        };

        VehicleData( double entryContTimestep,
                     bool enteredDetectorByMove ) :
            entryContTimestepM ( entryContTimestep ),
            leaveContTimestepM ( -1 ),
            passedEntireDetectorM ( enteredDetectorByMove ),
            leftDetectorByMoveM ( false )
            {}

        double entryContTimestepM;
        double leaveContTimestepM;
        bool passedEntireDetectorM;
        bool leftDetectorByMoveM;
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
    bool actionBeforeMoveAndEmit( void );

    bool actionAfterMoveAndEmit( void );

    MSNet::Time deleteOldData( void );
    
    void calcWaitingQueueLength( void );

    double getStartTimestep( MSNet::Time lastNTimesteps );    

    bool needsNewCalculation( MSNet::Time lastNTimesteps );

    template< class T >
    typename T::iterator getStartIterator( MSNet::Time lastNTimesteps,
                                           T& container )
        {
            typename T::iterator start = container.begin();
            if ( container.size() > lastNTimesteps ) {
                start =  container.end() - lastNTimesteps;
            }
            return start;
        }
    

private:
    std::string idM;

    TimestepDataCont     timestepDataM;
    VehicleDataMap       vehOnDetectorM;
    WaitingQueueElemCont waitingQueueElemsM;
    VehicleDataCont      vehLeftDetectorM;

    /// Lane where detector works on.
    MSLane* laneM;

    /// The begin on the lane
    double posM;

    /// The length on the lane
    double lengthM;

    MSMoveReminder* reminderM;

    const MSNet::Time deleteDataAfterSecondsM;

    bool modifiedSinceLastLookupM;

    MSNet::Time lookedUpLastNTimestepsM;

    int nVehContributedM;

    static std::vector< MSLaneState* > laneStateDetectorsM;

    static std::string xmlHeaderM;

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

inline int leftByMoveSum( int sumSoFar,
                          const MSLaneState::VehicleData& data )
{
    return sumSoFar + data.leftDetectorByMoveM;
}

inline int passedEntireSum( int sumSoFar,
                            const MSLaneState::VehicleData& data )
{
    return sumSoFar + data.passedEntireDetectorM;
}

inline int nVehEnteredSum( int sumSoFar,
                           const MSLaneState::TimestepData& data )
{
    return sumSoFar + data.nVehEnteredDetectorM;
}

#endif

// Local Variables:
// mode:C++
// End:
