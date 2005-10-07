#ifndef MSDETECTORCOUNTERCONTAINERWRAPPER_H
#define MSDETECTORCOUNTERCONTAINERWRAPPER_H

/**
 * @file   MSDetectorCounterContainerWrapper.h
 * @author Christian Roessel
 * @date   Started Mon Oct 6 17:52:18 2003
 * @version
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

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)

/* =========================================================================
 * included modules
 * ======================================================================= */

#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "MSDetectorContainerWrapperBase.h"
#include "MSDetectorOccupancyCorrection.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 *
 */
class MSVehicle;

struct MSDetectorCounterContainerWrapper
    : public MSDetectorContainerWrapperBase
{
    typedef SUMOReal Container;

    void enterDetectorByMove( MSVehicle* )
        {
            ++vehicleCountM;
        }

    void enterDetectorByEmitOrLaneChange( MSVehicle* )
        {
            ++vehicleCountM;
        }

    void leaveDetectorByMove( MSVehicle* )
        {
            --vehicleCountM;
        }

    void leaveDetectorByLaneChange( MSVehicle* )
        {
            --vehicleCountM;
        }

    void removeOnTripEnd( MSVehicle* veh )
        {
            --vehicleCountM;
        }


    MSDetectorCounterContainerWrapper()
        : MSDetectorContainerWrapperBase(),
          vehicleCountM( 0 )
        {}


    MSDetectorCounterContainerWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection )
        : MSDetectorContainerWrapperBase( occupancyCorrection ),
          vehicleCountM( 0 )
        {}

    virtual ~MSDetectorCounterContainerWrapper( void )
        {}

    Container vehicleCountM;
};

namespace DetectorContainer
{
    typedef MSDetectorCounterContainerWrapper Count;
}

#endif // MSDETECTORCOUNTERCONTAINERWRAPPER_H

// Local Variables:
// mode:C++
// End:
