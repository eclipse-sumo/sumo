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
#include <string>
#include "MSOccupancyCorrection.h"
class MSVehicle;
class MSLane;

class MSDensity : virtual public MSOccupancyCorrection< double >
{
protected:
    typedef double DetectorAggregate;
    typedef MSVehicle* ContainerItem;
    typedef std::list< ContainerItem > VehicleCont;
    
    MSDensity( double lengthInMeters );
    
    virtual ~MSDensity( void )
        {}    

    ContainerItem getNewContainerItem( MSVehicle& veh )
        {
            return &veh;
        }
        
    DetectorAggregate getDetectorAggregate( const VehicleCont& cont );

    void clearVehicleCont( VehicleCont& cont )
        {
            cont.clear();
        }    

    static std::string getDetectorName( void )
        {
            return "density";
        }

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
