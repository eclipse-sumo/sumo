#ifndef MSE2EDDETECTORINTERFACE_H
#define MSE2EDDETECTORINTERFACE_H

///
/// @file    MSE2EDDetectorInterface.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Fri Nov 28 2003 18:21 CET
/// @version $Id$
///
/// @brief   
///
///

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

class MSE2EDDetectorInterface
{
public:
    virtual const std::string& getId( void ) const = 0;
    
    virtual double getAggregate( MSUnit::Seconds lastNSeconds ) = 0;
    
    virtual ~MSE2EDDetectorInterface( void )
        {}
    
protected:
    MSE2EDDetectorInterface( void )
        {}

private:

};


// Local Variables:
// mode:C++
// End:

#endif // MSE2EDDETECTORINTERFACE_H
