/**
 * @file   MSDensity.cpp
 * @author Christian Roessel
 * @date   Started Thu Sep 11 13:35:55 2003
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H

#include "MSDensity.h"
#include "MSVehicle.h"
#include <cassert>

using namespace std;

MSDensity::MSDensity( const MSLane* lane,
                      const double lengthInMeters ) :
    occupancyEntryCorrectionM( DetAggregate(0) ),
    occupancyLeaveCorrectionM( DetAggregate(0) ),
    entryCorrectionVehM( 0 ),
    leaveCorrectionVehM( 0 ),
    detectorLengthM( lengthInMeters / 1000.0 )
{
    assert( detectorLengthM > 0 );
    assert( detectorLengthM <= MSUnit::getInstance()->getMeters(
                lane->length() ) );
}

MSDensity::~MSDensity( void )
{}

MSDensity::ContainerItem
MSDensity::getNewContainerItem( MSVehicle& veh )
{
    return &veh;
}

MSDensity::DetAggregate
MSDensity::getDetAggregate( const VehicleCont& cont )
{
    double nVehOnDet = cont.size() -
        occupancyEntryCorrectionM -
        occupancyLeaveCorrectionM;
    occupancyEntryCorrectionM = 0.0;
    occupancyLeaveCorrectionM = 0.0;
    entryCorrectionVehM = 0;
    leaveCorrectionVehM = 0;
    return nVehOnDet / detectorLengthM;
}

void
MSDensity::occupancyEntryCorrection( const MSVehicle& veh,
                                     double occupancyFractionOnDet )
{
    assert( occupancyFractionOnDet >= 0 &&
            occupancyFractionOnDet <= 1 );
    occupancyEntryCorrectionM = DetAggregate(occupancyFractionOnDet);
    entryCorrectionVehM = &veh;
}

void
MSDensity::occupancyLeaveCorrection( const MSVehicle& veh,
                                     double occupancyFractionOnDet )
{
    assert( occupancyFractionOnDet >= 0 &&
            occupancyFractionOnDet <= 1 );
    occupancyLeaveCorrectionM = DetAggregate(occupancyFractionOnDet);
    leaveCorrectionVehM = &veh;
}
    
void
MSDensity::dismissOccupancyCorrection( const MSVehicle& veh )
{
    // Necessary for leaveDetByLaneChange
    if ( &veh == entryCorrectionVehM ){
        occupancyEntryCorrectionM = DetAggregate(0);
        entryCorrectionVehM = 0;
    }
    if ( &veh == leaveCorrectionVehM ){
        occupancyLeaveCorrectionM = DetAggregate(0);
        leaveCorrectionVehM = 0;
    }
}
