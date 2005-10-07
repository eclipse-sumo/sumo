#ifndef MSE1_H
#define MSE1_H

///
/// @file    MSE1.h
/// @author  Christian Roessel <christian.roessel@gmx.de>
/// @date    Started Fri Jul 09 2004 21:31 CEST
/// @version
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
// $Log$
// Revision 1.6  2005/10/07 11:37:46  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.4  2005/09/15 11:08:09  dkrajzew
// LARGE CODE RECHECK
//
//
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <microsim/MSUnit.h>

/* =========================================================================
 * class declarations
 * ======================================================================= */
class MSVehicle;

/* =========================================================================
 * namespace
 * ======================================================================= */
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
        SUMOReal vehLengthM;
    };
}


// Local Variables:
// mode:C++
// End:

#endif // MSE1_H
