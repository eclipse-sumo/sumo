#ifndef MSInductLoop_H
#define MSInductLoop_H

/**
 * @file   MSInductLoop.h
 * @author Christian Roessel
 * @date   Mon Jul 21 16:14:05 2003
 * @version $Id$
 * @brief  Declaration of class MSInductLoop
 * 
 */

/* Copyright (C) 2003 by German Aerospace Center (http://www.dlr.de) */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

// $Id$

#include "MSMoveReminder.h"
#include "MSVehicle.h"
#include <string>
#include <deque>
#include "MSNet.h"
#include <SimpleCommand.h>
#include <SingletonDictionary.h>
#include "MSEventControl.h"

class MSLane;


/**
 * We count only the vehicles that passed the entire
 * detector. Vehicles that are emitted onto the detector or that
 * change lane while they are on the detector, will be omitted,
 * because we cannot determine a meaningful enter/leave-timestep.
 * 
 */
class MSInductLoop
    : public MSMoveReminder
{
public:
    typedef SingletonDictionary< std::string, MSInductLoop* > InductLoopDict;
    
    MSInductLoop( std::string id,
                  MSLane* lane,
                  double position,
                  MSNet::Time deleteDataAfterSeconds = 900 ) 
        : MSMoveReminder( lane, id ),
          posM( position ),
          deleteDataAfterSecondsM( deleteDataAfterSeconds )          
        {
            std::cout << "MSInductLoop " << id << std::endl;
            assert( posM >= 0 && posM <= laneM->length() );

            // insert object into dictionary
            if ( ! InductLoopDict::getInstance()->isInsertSuccess( idM,
                                                                   this ) ) {
                assert( false );
            }

            laneM->addMoveReminder( this );
    
            // start old-data removal through MSEventControl
            Command* deleteOldData = new SimpleCommand< MSInductLoop >(
                this, &MSInductLoop::deleteOldData );
            MSEventControl::getEndOfTimestepEvents()->addEvent(
                deleteOldData,
                deleteDataAfterSecondsM,
                MSEventControl::ADAPT_AFTER_EXECUTION );
        }
    
    /// Destructor.
    ~MSInductLoop(){};

    /**
     * @name MSMoveReminder methods.
     *
     * Methods in this group are inherited from MSMoveReminder. they are
     * called by the moving, entering and leaving vehicles.
     * 
     */
    //@{
    /** 
     * Indicator if the reminders is still active for the passed
     * vehicle/parameters. If false, the vehicle will erase this reminder
     * from it's reminder-container. This method will pass all neccessary
     * data to the corresponding detector. 
     * 
     * @param veh Vehicle that asks this remider.
     * @param oldPos Position before move.
     * @param newPos Position after move with newSpeed.
     * @param newSpeed Moving speed.
     * 
     * @return True if vehicle hasn't passed the detector completely.
     */
    bool isStillActive( MSVehicle& veh,
                        double oldPos,
                        double newPos,
                        double newSpeed );
    

    /** 
     *  Informs corresponding detector if vehicle leaves reminder
     *  by lanechange.
     * 
     * @param veh The leaving vehicle. 
     */
    void dismissByLaneChange( MSVehicle& veh );
 
    /** 
     * Informs corresponding detector if vehicle enters the reminder
     * by emit or lanechange.
     * 
     * @param veh The entering vehilcle.
     * 
     * @return True if vehicle is on or in front of the detector.
     */
    bool isActivatedByEmitOrLaneChange( MSVehicle& veh );
    //@}

    double getFlow( MSNet::Time lastNTimesteps ) const;


    double getMeanSpeed( MSNet::Time lastNTimesteps ) const;
    
  

    double getMeanSpeedSquare( MSNet::Time lastNTimesteps ) const;
    
   

    double getOccupancy( MSNet::Time lastNTimesteps ) const;
    
 

    double getMeanVehicleLength( MSNet::Time lastNTimesteps ) const;

    
    double getTimestepsSinceLastDetection() const;
    

    int getNVehContributed( MSNet::Time lastNTimesteps ) const;

    /** 
     * Returns a string indentifiing on object of the class. Used for
     * distinct filenames in MSDetector2File
     * 
     * @see MSDetector2File
     * @return String "MSInductLoop_" + idM
     */
    std::string getNamePrefix( void )
        {
            return "MSInductLoop_" + idM;
        }
    
    static std::string& getXMLHeader( void );

    std::string getXMLDetectorInfoStart( void );

    static std::string& getXMLDetectorInfoEnd( void );


    std::string getXMLOutput( MSNet::Time lastNTimesteps );


    struct VehicleData 
    {
        VehicleData( MSVehicle& veh,
                     double entryTimestep, double leaveTimestep )
            : lengthM( veh.length() ), 
              entryTimeM( entryTimestep * MSNet::deltaT() ),   // [s]
              leaveTimeM( leaveTimestep * MSNet::deltaT() ),   // [s]
              speedM( lengthM / ( leaveTimeM - entryTimeM ) ), // [m/s]
              speedSquareM( speedM * speedM ),
              occupancyM( leaveTimeM - entryTimeM ) // [s]
            {}
        double lengthM;
        double entryTimeM;
        double leaveTimeM;
        double speedM;
        double speedSquareM;
        double occupancyM;
    };

    typedef std::deque< VehicleData > VehicleDataCont;

    /**
     * Returns true if passed VehicleData's leaveTimeM is lesser 
     * than the passed leaveTimeBound.
     * 
     */
    struct leaveTimeLesser :
            public std::binary_function< VehicleData, double, bool >
    {
        bool operator()( const VehicleData& data,
                         double leaveTimeBound ) const
        {
            return data.leaveTimeM < leaveTimeBound;
        }
    };

protected:
    /**
     * @name Methods called by Reminder methods.
     *
     * Methods in this group are called by the MSMoveReminder methods 
     * only. They add the data to calculate the get* values.
     *
     */
    //@{
    void enterDetectorByMove( MSVehicle& veh,
                              double entryTimestep )
        {
            assert( vehOnDetectorM == 0 );
            vehOnDetectorM = &veh;
            entryTimestepM = entryTimestep;
        }

    void leaveDetectorByMove( MSVehicle& veh,
                              double leaveTimestep )
        {
            assert( vehOnDetectorM == &veh );
            leaveTimestepM = leaveTimestep;
            assert( entryTimestepM < leaveTimestepM );
            vehicleDataContM.push_back( VehicleData( veh, entryTimestepM,
                                                     leaveTimestepM ) );
            vehOnDetectorM = 0;
        }
    

    void leaveDetectorByLaneChange( MSVehicle& veh )
        {
            // Discard entry data
            vehOnDetectorM = 0;
            entryTimestepM = 0;
        }
    //@}

    /** 
     * Deletes data from vehicleDataContM if deleteDataAfterSecondsM
     * is over. Is called via MSEventControl.
     * 
     * @return deleteDataAfterSecondsM to recur the event. 
     */
    MSNet::Time deleteOldData( void ) 
        {
            double deleteBeforeTime =
                MSNet::getInstance()->timestep() * MSNet::deltaT() -
                deleteDataAfterSecondsM;
            if ( deleteBeforeTime > 0 ) {
                vehicleDataContM.erase(
                    vehicleDataContM.begin(),
                    lower_bound( vehicleDataContM.begin(),
                                 vehicleDataContM.end(),
                                 deleteBeforeTime,
                                 leaveTimeLesser() ) );
            }
            return static_cast< MSNet::Time >
                ( deleteDataAfterSecondsM * MSNet::deltaT() );
        }
    

    VehicleDataCont::const_iterator getStartIterator(
        MSNet::Time lastNTimesteps ) const
        {
            double startTime = 0;
            if ( lastNTimesteps < MSNet::getInstance()->timestep() ) {
                startTime = static_cast< double >(
                    MSNet::getInstance()->timestep() - lastNTimesteps ) *
                    MSNet::deltaT();
            }            
            return std::lower_bound( vehicleDataContM.begin(),
                                     vehicleDataContM.end(),
                                     startTime,
                                     leaveTimeLesser() );
        }

private:
   
    /// InductLoop's position on MSMoveReminder's lane.
    const double posM;

    MSNet::Time deleteDataAfterSecondsM;

    MSVehicle* vehOnDetectorM;
    double entryTimestepM;
    double leaveTimestepM;
    
    VehicleDataCont vehicleDataContM;

    static std::string xmlHeaderM;

    static std::string xmlDetectorInfoEndM;
    
    /// Hidden default constructor.
    MSInductLoop();

    /// Hidden copy constructor.
    MSInductLoop( const MSInductLoop& );

    /// Hidden assignment operator.
    MSInductLoop& operator=( const MSInductLoop& );

    
};

namespace
{
    inline double speedSum( double sumSoFar,
                            const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.speedM;
    }

    inline double speedSquareSum( double sumSoFar,
                                  const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.speedSquareM;
    }

    inline double occupancySum( double sumSoFar,
                                const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.occupancyM;
    }

    inline double lengthSum( double sumSoFar,
                             const MSInductLoop::VehicleData& data )
    {
        return sumSoFar + data.lengthM;
    }
}

#endif

// Local Variables:
// mode:C++
// End:
