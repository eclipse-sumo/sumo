#ifndef MSE2DETECTORINTERFACE_H
#define MSE2DETECTORINTERFACE_H

/**
 * @file   MSE2DetectorInterface.h
 * @author Christian Roessel
 * @date   Started Fri Sep 19 13:50:49 2003
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

#include "MSOccupancyCorrection.h"
#include "MSUnit.h"

class MSVehicle;

class MSE2DetectorInterface : virtual public MSOccupancyCorrection< double >
{
public:
    virtual const std::string& getId( void ) const = 0;
    
    // call every timestep. Stores detector-data in a container
    virtual void update( void ) = 0;
    
    // returns the last aggregated data value
    virtual double getCurrent( void ) const = 0;
      
    virtual double getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

    virtual void enterDetectorByMove( MSVehicle& ) = 0;
    
    virtual void leaveDetectorByMove( MSVehicle& ) = 0;

    virtual void leaveDetectorByLaneChange( MSVehicle& ) = 0;

    virtual void enterDetectorByEmitOrLaneChange( MSVehicle& ) = 0;

    
protected:
    MSE2DetectorInterface( void ) 
        : MSOccupancyCorrection< double >()
        {}
    
private:
    
    
};







#endif // MSE2DETECTORINTERFACE_H

// Local Variables:
// mode:C++
// End:
