/****************************************************************************/
/// @file    MSDetectorContainerWrapperBase.h
/// @author  Christian Roessel
/// @date    Fri Sep 26 19:11:26 2003
/// @version $Id$
///
//	»missingDescription«
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
#ifndef MSDetectorContainerWrapperBase_h
#define MSDetectorContainerWrapperBase_h

// ===========================================================================
// included modules
// ===========================================================================

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "MSDetectorOccupancyCorrection.h"

// ===========================================================================
// class declarations
// ===========================================================================
class MSVehicle;

struct MSDetectorContainerWrapperBase {
    virtual void enterDetectorByMove(MSVehicle* veh) = 0;
    virtual void enterDetectorByEmitOrLaneChange(MSVehicle* veh) = 0;
    virtual void leaveDetectorByMove(MSVehicle* veh) = 0;
    virtual void leaveDetectorByLaneChange(MSVehicle* veh) = 0;
    virtual void removeOnTripEnd(MSVehicle* veh) = 0;


    MSDetectorContainerWrapperBase()
            : occupancyCorrectionM(0) {}

    MSDetectorContainerWrapperBase(
        const MSDetectorOccupancyCorrection& occupancyCorrection)
            : occupancyCorrectionM(&occupancyCorrection) {}

    virtual ~MSDetectorContainerWrapperBase(void) {}

    const MSDetectorOccupancyCorrection* occupancyCorrectionM;
};


#endif

/****************************************************************************/

