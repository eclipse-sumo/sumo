/****************************************************************************/
/// @file    MSDetectorCounterContainerWrapper.h
/// @author  Christian Roessel
/// @date    Mon Oct 6 17:52:18 2003
/// @version $Id$
///
// * @author Christian Roessel
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef MSDetectorCounterContainerWrapper_h
#define MSDetectorCounterContainerWrapper_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorContainerWrapperBase.h"
#include "MSDetectorOccupancyCorrection.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSVehicle;

struct MSDetectorCounterContainerWrapper
            : public MSDetectorContainerWrapperBase
{
    typedef SUMOReal Container;

    void enterDetectorByMove(MSVehicle*)
    {
        ++vehicleCountM;
    }

    void enterDetectorByEmitOrLaneChange(MSVehicle*)
    {
        ++vehicleCountM;
    }

    void leaveDetectorByMove(MSVehicle*)
    {
        --vehicleCountM;
    }

    void leaveDetectorByLaneChange(MSVehicle*)
    {
        --vehicleCountM;
    }

    void removeOnTripEnd(MSVehicle*)
    {
        --vehicleCountM;
    }


    MSDetectorCounterContainerWrapper()
            : MSDetectorContainerWrapperBase(),
            vehicleCountM(0)
    {}


    MSDetectorCounterContainerWrapper(
        const MSDetectorOccupancyCorrection& occupancyCorrection)
            : MSDetectorContainerWrapperBase(occupancyCorrection),
            vehicleCountM(0)
    {}

    virtual ~MSDetectorCounterContainerWrapper(void)
    {}

    Container vehicleCountM;
};

namespace DetectorContainer
{
typedef MSDetectorCounterContainerWrapper Count;
}


#endif

/****************************************************************************/

