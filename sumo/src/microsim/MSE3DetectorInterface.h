#ifndef MSE3DETECTORINTERFACE_H
#define MSE3DETECTORINTERFACE_H

/**
 * @file    MSE3DetectorInterface.h
 * @author  Christian Roessel <christian.roessel@dlr.de>
 * @date    Started Tue Nov 25 2003 18:37 CET
 * @version $Id$
 *
 * @brief   
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

#include <string>
#include "MSUnit.h"

class MSVehicle;

class MSE3DetectorInterface
{
public:
    virtual const std::string& getId( void ) const = 0;
    
    virtual double getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

    virtual void leave( MSVehicle& veh ) = 0;

    virtual ~MSE3DetectorInterface( void )
        {}
    
protected:

    MSE3DetectorInterface( void ) 
        {}

private:
    
};

// Local Variables:
// mode:C++
// End:

#endif // MSE3DETECTORINTERFACE_H
