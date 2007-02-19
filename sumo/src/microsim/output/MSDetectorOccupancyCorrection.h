/****************************************************************************/
/// @file    MSDetectorOccupancyCorrection.h
/// @author  Christian Roessel
/// @date    Thu Sep 11 16:17:08 2003
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
#ifndef MSDetectorOccupancyCorrection_h
#define MSDetectorOccupancyCorrection_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif

// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <cassert>
#include <microsim/MSUpdateEachTimestep.h>


// ===========================================================================
// class declarations
// ===========================================================================

class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
class MSDetectorOccupancyCorrection :
            public MSUpdateEachTimestep< MSDetectorOccupancyCorrection >
{
public:
    void setOccupancyEntryCorrection(const MSVehicle& veh,
                                     SUMOReal occupancyFractionOnDet)
    {
        assert(occupancyFractionOnDet >= 0 &&
               occupancyFractionOnDet <= 1);
        occupancyEntryCorrectionM = occupancyFractionOnDet;
        entryCorrectionVehM = &veh;
    }

    void setOccupancyLeaveCorrection(const MSVehicle& veh,
                                     SUMOReal occupancyFractionOnDet)
    {
        assert(occupancyFractionOnDet >= 0 &&
               occupancyFractionOnDet <= 1);
        occupancyLeaveCorrectionM = occupancyFractionOnDet;
        leaveCorrectionVehM = &veh;
    }

    void dismissOccupancyCorrection(const MSVehicle& veh)
    {
        // Necessary for leaveDetByLaneChange
        if (&veh == entryCorrectionVehM) {
            occupancyEntryCorrectionM = 0;
            entryCorrectionVehM = 0;
        }
        if (&veh == leaveCorrectionVehM) {
            occupancyLeaveCorrectionM = 0;
            leaveCorrectionVehM = 0;
        }
    }

    bool updateEachTimestep(void)
    {
        resetOccupancyCorrection();
        return false;
    }

    SUMOReal getOccupancyEntryCorrection(void) const
    {
        return occupancyEntryCorrectionM;
    }

    SUMOReal getOccupancyLeaveCorrection(void) const
    {
        return occupancyLeaveCorrectionM;
    }

    MSDetectorOccupancyCorrection(void)
            : MSUpdateEachTimestep< MSDetectorOccupancyCorrection >(),
            occupancyEntryCorrectionM(0),
            occupancyLeaveCorrectionM(0),
            entryCorrectionVehM(0),
            leaveCorrectionVehM(0)
    {}

    virtual ~MSDetectorOccupancyCorrection(void)
    {}

private:
    SUMOReal occupancyEntryCorrectionM;
    SUMOReal occupancyLeaveCorrectionM;
    MSVehicle const* entryCorrectionVehM;
    MSVehicle const* leaveCorrectionVehM;

    MSDetectorOccupancyCorrection(const MSDetectorOccupancyCorrection&);
    MSDetectorOccupancyCorrection& operator=(
        const MSDetectorOccupancyCorrection&);

    void resetOccupancyCorrection(void)
    {
        occupancyEntryCorrectionM = 0;
        occupancyLeaveCorrectionM = 0;
        entryCorrectionVehM = 0;
        leaveCorrectionVehM = 0;
    }
};

namespace Detector
{
typedef MSUpdateEachTimestep< MSDetectorOccupancyCorrection >
UpdateOccupancyCorrections;
}


#endif

/****************************************************************************/

