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
#include "MSOccupancyCorrection.h"
#include "MSDetectorPredicates.h"
class MSVehicle;
class MSLane;

class MSDensity : public MSDetectorPredicates< MSVehicle* >,
                  public MSOccupancyCorrection< double >
{
public:
    typedef double DetectorAggregate;
    typedef MSVehicle* ContainerItem;
    typedef std::list< ContainerItem > VehicleCont;
    
    MSDensity( const MSLane* lane,
               const double lengthInMeters );
    
    ~MSDensity( void )
        {}    

    ContainerItem getNewContainerItem( MSVehicle& veh )
        {
            return &veh;
        }
        
    DetectorAggregate getDetectorAggregate( const VehicleCont& cont );
    
private:
    const double detectorLengthM; //  [km]
    MSDensity();
    MSDensity( const MSDensity& );
    MSDensity& operator=( const MSDensity& );
};


#endif

// Local Variables:
// mode:C++
// End:
