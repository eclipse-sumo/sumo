#ifndef MSOCCUPANCYCORRECTION_H
#define MSOCCUPANCYCORRECTION_H

/**
 * @file   MSOccupancyCorrection.h
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

// $Id$

class MSVehicle;


// DetectorAggregate must provide a
// - default ctor DetectorAggregate() which results in a "0"
// - DetectorAggregate( double )

template< typename DetectorAggregate >
class MSOccupancyCorrection
{
public:
    MSOccupancyCorrection( void ) 
        : occupancyEntryCorrectionM(),
          occupancyLeaveCorrectionM(),
          entryCorrectionVehM(0),
          leaveCorrectionVehM(0)
        {}
    
    void setOccupancyEntryCorrection( const MSVehicle& veh,
                                      double occupancyFractionOnDet )
        {
            assert( occupancyFractionOnDet >= 0 &&
                    occupancyFractionOnDet <= 1 );
            occupancyEntryCorrectionM =
                DetectorAggregate(occupancyFractionOnDet);
            entryCorrectionVehM = &veh;
        }

    void setOccupancyLeaveCorrection( const MSVehicle& veh,
                                      double occupancyFractionOnDet )
        {
            assert( occupancyFractionOnDet >= 0 &&
                    occupancyFractionOnDet <= 1 );
            occupancyLeaveCorrectionM =
                DetectorAggregate(occupancyFractionOnDet);
            leaveCorrectionVehM = &veh;
        }
    
    void dismissOccupancyCorrection( const MSVehicle& veh )
        {
            // Necessary for leaveDetByLaneChange
            if ( &veh == entryCorrectionVehM ){
                occupancyEntryCorrectionM = DetectorAggregate();
                entryCorrectionVehM = 0;
            }
            if ( &veh == leaveCorrectionVehM ){
                occupancyLeaveCorrectionM = DetectorAggregate();
                leaveCorrectionVehM = 0;
            }
        }

    void resetOccupancyCorrection( void )
        {
            occupancyEntryCorrectionM = DetectorAggregate();
            occupancyLeaveCorrectionM = DetectorAggregate();
            entryCorrectionVehM = 0;
            leaveCorrectionVehM = 0;
        }

    DetectorAggregate getOccupancyEntryCorrection( void )
        {
            return occupancyEntryCorrectionM;
        }
    
    DetectorAggregate getOccupancyLeaveCorrection( void )
        {
            return occupancyLeaveCorrectionM;
        }
    
private:
    DetectorAggregate occupancyEntryCorrectionM;
    DetectorAggregate occupancyLeaveCorrectionM;
    MSVehicle const* entryCorrectionVehM;
    MSVehicle const* leaveCorrectionVehM;

    MSOccupancyCorrection( const MSOccupancyCorrection& );
    MSOccupancyCorrection& operator=( const MSOccupancyCorrection& );
};


#endif // MSOCCUPANCYCORRECTION_H

// Local Variables:
// mode:C++
// End:
