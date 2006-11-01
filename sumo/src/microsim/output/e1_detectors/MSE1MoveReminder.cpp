///
/// @file    MSE1MoveReminder.cpp
/// @author  Christian Roessel <christian.roessel@dlr.de>
/// @date    Started Tue Dec 02 2003 22:17 CET
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
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.1  2006/11/01 23:29:18  behrisch
// MSE1MoveReminder needs complete definition of MSE1Collector
//
//
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

#include "MSE1MoveReminder.h"
#include "MSE1Collector.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


/* =========================================================================
 * method definitions
 * ======================================================================= */

template< bool isEntryReminder >
    MSE1MoveReminder< isEntryReminder >::MSE1MoveReminder(
        std::string id
        , MSCrossSection crossSection
        , MSE1Collector& collector
        )
        : MSMoveReminder(crossSection.laneM)
          , collectorM( collector )
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
            if ( newPos <= posM ) {
                // crossSection not yet reached
                return true;
            }
            return isActive( veh, oldPos, newPos,
                             Loki::Int2Type< isEntryReminder >() );
        }

template< bool isEntryReminder >
    void MSE1MoveReminder< isEntryReminder >::dismissByLaneChange( MSVehicle& veh )
        {
            dismiss( veh,  Loki::Int2Type< isEntryReminder >() );
        }

template< bool isEntryReminder >
    bool MSE1MoveReminder< isEntryReminder >::isActivatedByEmitOrLaneChange( MSVehicle& veh )
        {
            return isActivated( veh,
                                Loki::Int2Type< isEntryReminder >() );
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
                ( newPos - posM ) / ( newPos - oldPos ) );
            collectorM.enter( veh, entryTime );
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
    void MSE1MoveReminder< isEntryReminder >::dismiss(MSVehicle& veh, Loki::Int2Type< true >)
        {}

    // LeaveReminder overload
template< bool isEntryReminder >
    bool MSE1MoveReminder< isEntryReminder >::isActive(MSVehicle& veh,
                  SUMOReal oldPos ,
                  SUMOReal newPos,
                  Loki::Int2Type< false >)
        {
            if ( newPos - veh.getLength() > posM ) {
                // crossSection completely left
                MSUnit::Seconds leaveTime = MSUnit::getInstance()->getSeconds(
                    MSNet::getInstance()->getCurrentTimeStep() -
                    ( newPos - veh.getLength() - posM ) / ( newPos - oldPos ) );
                collectorM.leave( veh, leaveTime );
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
            collectorM.dismissByLaneChange( veh );
        }


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:
