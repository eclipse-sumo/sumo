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

#include "MSDetectorInterfaceCommon.h"
#include <string>

class MSE2EDDetectorInterface
    :
    public MSDetectorInterfaceCommon
{
public:
    
    virtual ~MSE2EDDetectorInterface( void )
        {}
    
protected:
    MSE2EDDetectorInterface( std::string id )
        : MSDetectorInterfaceCommon( id )
        {}

private:

};


// Local Variables:
// mode:C++
// End:

#endif // MSE2EDDETECTORINTERFACE_H
