#ifndef MSDENSITY_H
#define MSDENSITY_H

/**
 * @file   MSDensity.h
 * @author Christian Roessel
 * @date   Started Tue Sep  9 22:27:23 2003
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

#include <list>
#include <cassert>
#include <functional>
#include "MSVehicle.h"
#include "MSUnit.h"
class MSLane;


class MSDensity
{
public:
    typedef double DetAggregate; // [veh/km]
    typedef MSVehicle* ContainerItem;
    typedef std::list< ContainerItem > VehicleCont;
    
    MSDensity( const MSLane* lane,
               const double lengthInMeters );
    
    ~MSDensity( void );
    

    ContainerItem getNewContainerItem( MSVehicle& veh );
        
    DetAggregate getDetAggregate( const VehicleCont& cont ); // [veh/km]

    void occupancyEntryCorrection( const MSVehicle& veh,
                                   double occupancyFractionOnDet );

    void occupancyLeaveCorrection( const MSVehicle& veh,
                                   double occupancyFractionOnDet );
    
    void dismissOccupancyCorrection( const MSVehicle& veh );
    
protected:
    DetAggregate occupancyEntryCorrectionM;
    DetAggregate occupancyLeaveCorrectionM;
    MSVehicle const* entryCorrectionVehM;
    MSVehicle const* leaveCorrectionVehM;

    struct PosGreater :
        public std::binary_function< ContainerItem, double, bool >
    {
        bool operator() ( const ContainerItem& item, double pos ) const {
            return item->pos() > pos;
        }
    };

    struct VehEquals :
        public std::binary_function< ContainerItem, MSVehicle*, bool >
    {
        bool operator() ( const ContainerItem& item,
                          const MSVehicle* veh ) const {
            return item == veh;
        }
    };    

    
private:
    const double detectorLengthM; // in km
    MSDensity();
    MSDensity( const MSDensity& );
    MSDensity& operator=( const MSDensity& );

};


#endif

// Local Variables:
// mode:C++
// End:
