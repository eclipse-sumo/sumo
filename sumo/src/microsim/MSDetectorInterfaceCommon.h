#ifndef MSDETECTORINTERFACECOMMON_H
#define MSDETECTORINTERFACECOMMON_H

///
/// @file    MSDetectorInterfaceCommon.h
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Mon Dec 01 2003 16:20 CET
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

class MSDetectorInterfaceCommon
{
public:
    
    const std::string& getId( void ) const
        {
            return idM;
        }

    const std::string& getName( void ) const
        {
            return detNameM;
        }

    virtual double getAggregate( MSUnit::Seconds lastNSeconds ) = 0;

    virtual ~MSDetectorInterfaceCommon( void )
        {}

protected:

    MSDetectorInterfaceCommon(
        std::string id
        //, std::string detName
        )
        :
        idM( id )
        , detNameM("")
        {}
    
    std::string idM;
    std::string detNameM;
    
private:

};


// Local Variables:
// mode:C++
// End:

#endif // MSDETECTORINTERFACECOMMON_H
