/****************************************************************************/
/// @file    MSE1MoveReminder.cpp
/// @author  Christian Roessel
/// @date    Tue Dec 02 2003 22:17 CET
/// @version $Id$
///
// / @author  Christian Roessel <christian.roessel@dlr.de>
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

#include "MSE1MoveReminder.h"
#include "MSE1Collector.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================

template< bool isEntryReminder >
MSE1MoveReminder< isEntryReminder >::MSE1MoveReminder(
    std::string /*id*/
    , MSCrossSection crossSection
    , MSE1Collector& collector
)
        : MSMoveReminder(crossSection.laneM)
        , collectorM(collector)
        , posM(crossSection.posM)
{}

template< bool isEntryReminder >
bool MSE1MoveReminder< isEntryReminder >::isStillActive(
    MSVehicle& veh
    , SUMOReal oldPos
    , SUMOReal newPos
    , SUMOReal

)
{
    if (newPos <= posM) {
        // crossSection not yet reached
        return true;
    }
    return isActive(veh, oldPos, newPos,
                    Loki::Int2Type< isEntryReminder >());
}

template< bool isEntryReminder >
void MSE1MoveReminder< isEntryReminder >::dismissByLaneChange(MSVehicle& veh)
{
    dismiss(veh,  Loki::Int2Type< isEntryReminder >());
}

template< bool isEntryReminder >
bool MSE1MoveReminder< isEntryReminder >::isActivatedByEmitOrLaneChange(MSVehicle& veh)
{
    return isActivated(veh,
                       Loki::Int2Type< isEntryReminder >());
    /*            if ( veh.pos() <= posM ) {
                    // crossSection not yet reached
                    return true;
                }
                return false;*/
}

// EntryReminder overload
template< bool isEntryReminder >
bool MSE1MoveReminder< isEntryReminder >::isActive(MSVehicle& veh,
        SUMOReal oldPos ,
        SUMOReal newPos,
        Loki::Int2Type< true >)
{
    // crossSection partially or completely entered
    MSUnit::Seconds entryTime = MSUnit::getInstance()->getSeconds(
                                    MSNet::getInstance()->getCurrentTimeStep() -
                                    (newPos - posM) / (newPos - oldPos));
    collectorM.enter(veh, entryTime);
    return false;
}

// EntryReminder overload
template< bool isEntryReminder >
bool MSE1MoveReminder< isEntryReminder >::isActivated(MSVehicle& veh, Loki::Int2Type< true >)
{
    // activate if veh is in front on crossSection. Don't measure
    // 'partially' entered vehicles.
    return veh.getPositionOnLane() <= posM;
}

// EntryReminder overload
template< bool isEntryReminder >
void MSE1MoveReminder< isEntryReminder >::dismiss(MSVehicle& , Loki::Int2Type< true >)
{}

// LeaveReminder overload
template< bool isEntryReminder >
bool MSE1MoveReminder< isEntryReminder >::isActive(MSVehicle& veh,
        SUMOReal oldPos ,
        SUMOReal newPos,
        Loki::Int2Type< false >)
{
    if (newPos - veh.getLength() > posM) {
        // crossSection completely left
        MSUnit::Seconds leaveTime = MSUnit::getInstance()->getSeconds(
                                        MSNet::getInstance()->getCurrentTimeStep() -
                                        (newPos - veh.getLength() - posM) / (newPos - oldPos));
        collectorM.leave(veh, leaveTime);
        return false;
    }
    // crossSection partially left
    return true;
}

// LeaveReminder overload
template< bool isEntryReminder >
bool MSE1MoveReminder< isEntryReminder >::isActivated(MSVehicle& veh, Loki::Int2Type< false >)
{
    // activate if veh is in front on crossSection. Don't measure
    // 'partially' entered vehicles.
    return veh.getPositionOnLane() <= posM;
}

// LeaveReminder overload
template< bool isEntryReminder >
void MSE1MoveReminder< isEntryReminder >::dismiss(MSVehicle& veh, Loki::Int2Type< false >)
{
    collectorM.dismissByLaneChange(veh);
}



/****************************************************************************/

