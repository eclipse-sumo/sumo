#ifndef MSE1MOVEREMINDER_H
#define MSE1MOVEREMINDER_H

/**
 * @file    MSE1MoveReminder.h
 * @author  Christian Roessel <christian.roessel@dlr.de>
 * @date    Started Wed Jun 9 16:40:56 CEST 2004
 * @version $Id$
 *
 * @brief
 *
 */

/* Copyright (C) 2004 by Christian Roessel <christian.roessel@gmx.de> */

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//

#include <microsim/MSMoveReminder.h>
#include <microsim/MSUnit.h>
#include <microsim/output/MSCrossSection.h>
#include <string>
#ifdef WIN32
#include "helpers/msvc6_TypeManip.h"
#endif
#ifndef WIN32
#include "helpers/gcc_TypeManip.h"
#endif

class MSE1Collector;
class MSLane;
class MSVehicle;

template< bool isEntryReminder >
class MSE1MoveReminder : public MSMoveReminder
{
public:

    MSE1MoveReminder(
        std::string id
        , MSCrossSection crossSection
        , MSE1Collector& collector
        )
        : MSMoveReminder(  crossSection.laneM, id )
          , collectorM( collector )
          , posM( MSUnit::getInstance()->getCells( crossSection.posM ) )
        {}

    bool isStillActive(
        MSVehicle& veh
        , double oldPos
        , double newPos
        , double

        )
        {
            if ( newPos <= posM ) {
                // crossSection not yet reached
                return true;
            }
            return isActive( veh, oldPos, newPos,
                             Loki::Int2Type< isEntryReminder >() );
        }

    void dismissByLaneChange( MSVehicle& veh )
        {
            dismiss( veh,  Loki::Int2Type< isEntryReminder >() );
        }

    bool isActivatedByEmitOrLaneChange( MSVehicle& veh )
        {
            return isActivated( veh,
                                Loki::Int2Type< isEntryReminder >() );
/*            if ( veh.pos() <= posM ) {
                // crossSection not yet reached
                return true;
            }
            return false;*/
        }

private:

    // EntryReminder overload
    bool isActive(MSVehicle& veh,
                  double oldPos ,
                  double newPos,
                  Loki::Int2Type< true >)
        {
            // crossSection partially or completely entered
            MSUnit::Seconds entryTime = MSUnit::getInstance()->getSeconds(
                MSNet::getInstance()->timestep() -
                ( newPos - posM ) / ( newPos - oldPos ) );
            collectorM.enter( veh, entryTime );
            return false;
        }

    // EntryReminder overload
    bool isActivated(MSVehicle& veh, Loki::Int2Type< true >)
        {
            // activate if veh is in front on crossSection. Don't measure
            // 'partially' entered vehicles.
            return veh.pos() <= posM;
        }

    // EntryReminder overload
    void dismiss(MSVehicle& veh, Loki::Int2Type< true >)
        {}

    // LeaveReminder overload
    bool isActive(MSVehicle& veh,
                  double oldPos ,
                  double newPos,
                  Loki::Int2Type< false >)
        {
            if ( newPos - veh.length() > posM ) {
                // crossSection completely left
                MSUnit::Seconds leaveTime = MSUnit::getInstance()->getSeconds(
                MSNet::getInstance()->timestep() -
                ( newPos - veh.length() - posM ) / ( newPos - oldPos ) );
                collectorM.leave( veh, leaveTime );
                return false;
            }
            // crossSection partially left
            return true;
        }

    // LeaveReminder overload
    bool isActivated(MSVehicle& veh, Loki::Int2Type< false >)
        {
            // activate if veh is in front on crossSection. Don't measure
            // 'partially' entered vehicles.
            return veh.pos() <= posM;
        }

     // LeaveReminder overload
     void dismiss(MSVehicle& veh, Loki::Int2Type< false >)
        {
            collectorM.dismissByLaneChange( veh );
        }

    MSE1Collector& collectorM;
    MSUnit::Cells posM;
};

namespace Detector
{
    typedef MSE1MoveReminder< true > E1EntryReminder;
    typedef MSE1MoveReminder< false > E1LeaveReminder;
}



// Local Variables:
// mode:C++
// End:

#endif // MSE1MOVEREMINDER_H
