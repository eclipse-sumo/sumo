#ifndef MSDOUBLEDETECTORCONTAINER_H
#define MSDOUBLEDETECTORCONTAINER_H

/**
 * @file   MSDoubleDetectorContainer.h
 * @author Christian Roessel
 * @date   Started Mon Oct  6 17:52:18 2003
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

#include "MSDetectorContainerBase.h"

class MSVehicle;

struct MSDoubleDetectorContainer : public MSDetectorContainerBase
{
    typedef double Container;

    void enterDetectorByMove( MSVehicle* veh )
        {
            ++containerM;
        }

    void enterDetectorByEmitOrLaneChange( MSVehicle* veh )
        {
            ++containerM;
        }

    void leaveDetectorByMove( MSVehicle* veh )
        {
            --containerM;
        }

    void leaveDetectorByLaneChange( MSVehicle* veh )
        {
            --containerM;
        }

    void update( void )
        {}

    MSDoubleDetectorContainer( void )
        : containerM( 0 )
        {}

    virtual ~MSDoubleDetectorContainer( void )
        {}

    Container containerM;
};

namespace DetectorContainer
{
    typedef MSDoubleDetectorContainer Count;
}

#endif // MSDOUBLEDETECTORCONTAINER_H

// Local Variables:
// mode:C++
// End:
