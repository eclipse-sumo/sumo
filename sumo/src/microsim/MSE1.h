#ifndef MSE1_H
#define MSE1_H

///
/// @file    MSE1.h
/// @author  Christian Roessel <christian.roessel@gmx.de>
/// @date    Started Fri Jul 09 2004 21:31 CEST
/// @version $Id$
///
/// @brief   
///
///

/* Copyright (C) 2004 by Christian Roessel <christian.roessel@gmx.de> */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

class MSVehicle;
#include "MSUnit.h"

///
/// Introduces some enums and consts for use in MSE1Collector. 
///
namespace E1
{
    ///
    /// Collection of all possible E1-detectors. Names should be
    /// self-explanatory. All E1-detectors are LD-detectors (leave-data).
    ///    
    enum DetType { MEAN_SPEED = 0
                   , NUMBER_OF_VEHICLES
                   , FLOW
                   , MEAN_OCCUPANCY
                   , MEAN_VEHICLE_LENGTH
                   , ALL        ///< Use this to generate all possible
                                ///detectors in
                                ///MSE1Collector::addDetector().
    };

    /// Increment operator that allows us to iterate over the
    /// E1::DetType detectors.
    ///
    /// @param det A detector out of E1::DetType (an integral type).
    ///
    /// @return The detector that follows det.
    ///    
    DetType& operator++( DetType& det );

//     ///
//     /// Collection of different "vehicle" containers used by MSE1Collector. 
//     ///    
//     enum Containers { VEHICLES = 0
//                       , HALTINGS
//                       , TRAVELTIME
//     };
    
//     /// Increment operator that allows us to iterate over the
//     /// E2::Containers detectors.
//     ///
//     /// @param cont A container out of E2::Containers (an integral type).
//     ///
//     /// @return The container that follows cont.
//     Containers& operator++( Containers& cont );


    struct E1Container
    {
        E1Container() 
            : vehOnDetectorM(0),
              entryTimeM(-1),
              leaveTimeM(-1),
              vehLengthM(-1)
            {}
        void enter( MSVehicle* enteringVeh,
                    MSUnit::Seconds entryTime )
            {
                assert( vehOnDetectorM == 0 );
                vehOnDetectorM = enteringVeh;
                entryTimeM = entryTime;
            }
        void setLeaveTime( MSVehicle* veh,
                           MSUnit::Seconds leaveTime )
            {
                assert( vehOnDetectorM == veh );
                leaveTimeM = leaveTime;
            }
        void clear( MSVehicle* veh )
            {
                assert( vehOnDetectorM == veh );
                vehOnDetectorM = 0;
                entryTimeM = -1;
                leaveTimeM = -1;
                vehLengthM = -1;
            }
        bool hasVehicle( MSVehicle* veh ) const
            {
                return vehOnDetectorM == veh;
            }
        
        MSVehicle* vehOnDetectorM;
        MSUnit::Seconds entryTimeM;
        MSUnit::Seconds leaveTimeM;
        double vehLengthM;
    };    
}


// Local Variables:
// mode:C++
// End:

#endif // MSE1_H
