#ifndef MSLDDETECTORINTERFACE_H
#define MSLDDETECTORINTERFACE_H

///
/// @file    MSLDDetectorInterface.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 20:04 CET
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

class MSVehicle;

namespace LD
{
    
    class MSDetectorInterface
        :
        public MSDetectorInterfaceCommon
    {
    public:

        virtual void leave( MSVehicle& veh ) = 0;

        virtual ~MSDetectorInterface( void )
            {}
    
    protected:

        MSDetectorInterface( std::string id )
            : MSDetectorInterfaceCommon( id )
            {}
    
    private:

    };
 
} // end namespace LD


// Local Variables:
// mode:C++
// End:

#endif // MSLDDETECTORINTERFACE_H
