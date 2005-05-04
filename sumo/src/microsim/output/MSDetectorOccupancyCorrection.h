#ifndef MSDETECTOROCCUPANCYCORRECTION_H
#define MSDETECTOROCCUPANCYCORRECTION_H

/**
 * @file   MSDetectorOccupancyCorrection.h
 * @author Christian Roessel
 * @date   Started Thu Sep 11 16:17:08 2003
 * @version $Id$
 * @brief
 *
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

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <cassert>
#include <microsim/MSUpdateEachTimestep.h>

class MSVehicle;

class MSDetectorOccupancyCorrection :
    public MSUpdateEachTimestep< MSDetectorOccupancyCorrection >
{
public:
    void setOccupancyEntryCorrection( const MSVehicle& veh,
                                      double occupancyFractionOnDet )
        {
            assert( occupancyFractionOnDet >= 0 &&
                    occupancyFractionOnDet <= 1 );
            occupancyEntryCorrectionM = occupancyFractionOnDet;
            entryCorrectionVehM = &veh;
        }

    void setOccupancyLeaveCorrection( const MSVehicle& veh,
                                      double occupancyFractionOnDet )
        {
            assert( occupancyFractionOnDet >= 0 &&
                    occupancyFractionOnDet <= 1 );
            occupancyLeaveCorrectionM = occupancyFractionOnDet;
            leaveCorrectionVehM = &veh;
        }

    void dismissOccupancyCorrection( const MSVehicle& veh )
        {
            // Necessary for leaveDetByLaneChange
            if ( &veh == entryCorrectionVehM ){
                occupancyEntryCorrectionM = 0;
                entryCorrectionVehM = 0;
            }
            if ( &veh == leaveCorrectionVehM ){
                occupancyLeaveCorrectionM = 0;
                leaveCorrectionVehM = 0;
            }
        }

    bool updateEachTimestep( void )
        {
            resetOccupancyCorrection();
            return false;
        }

    double getOccupancyEntryCorrection( void ) const
        {
            return occupancyEntryCorrectionM;
        }

    double getOccupancyLeaveCorrection( void ) const
        {
            return occupancyLeaveCorrectionM;
        }

    MSDetectorOccupancyCorrection( void )
        : MSUpdateEachTimestep< MSDetectorOccupancyCorrection >(),
          occupancyEntryCorrectionM(0),
          occupancyLeaveCorrectionM(0),
          entryCorrectionVehM(0),
          leaveCorrectionVehM(0)
        {}

    virtual ~MSDetectorOccupancyCorrection( void )
        {}

private:
    double occupancyEntryCorrectionM;
    double occupancyLeaveCorrectionM;
    MSVehicle const* entryCorrectionVehM;
    MSVehicle const* leaveCorrectionVehM;

    MSDetectorOccupancyCorrection( const MSDetectorOccupancyCorrection& );
    MSDetectorOccupancyCorrection& operator=(
        const MSDetectorOccupancyCorrection& );

    void resetOccupancyCorrection( void )
        {
            occupancyEntryCorrectionM = 0;
            occupancyLeaveCorrectionM = 0;
            entryCorrectionVehM = 0;
            leaveCorrectionVehM = 0;
        }
};

namespace Detector
{
    typedef MSUpdateEachTimestep< MSDetectorOccupancyCorrection >
    UpdateOccupancyCorrections;
}

#endif // MSDETECTOROCCUPANCYCORRECTION_H

// Local Variables:
// mode:C++
// End:
