#ifndef MSE3MOVEREMINDER_H
#define MSE3MOVEREMINDER_H

/**
 * @file    MSE3MoveReminder.h
 * @author  Christian Roessel <christian.roessel@dlr.de>
 * @date    Started Tue Nov 25 2003 17:04 CET
 * @version $Id$
 *
 * @brief
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

class MSE3Collector;
class MSLane;
class MSVehicle;

template< bool isEntryReminder >
class MSE3MoveReminder : public MSMoveReminder
{
public:

    MSE3MoveReminder(
        std::string id
        , MSCrossSection crossSection
        , MSE3Collector& collector
        )
        : MSMoveReminder(  crossSection.laneM, id )
          , collectorM( collector )
          , posM( MSUnit::getInstance()->getCells( crossSection.posM ) )
        {}

    bool isStillActive(
        MSVehicle& veh
        , double
        , double newPos
        , double

        )
        {
            if ( newPos <= posM ) {
                // crossSection not yet reached
                return true;
            }
            return isActive( veh, newPos,
                             Loki::Int2Type< isEntryReminder >() );
        }

    void dismissByLaneChange( MSVehicle& veh )
        {
            // nothing to do for E3
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

    bool isActive(MSVehicle& veh, double, Loki::Int2Type< true >)
        {
            // crossSection partially or completely entered
            collectorM.enter( veh );
            return false;
        }

    bool isActivated(MSVehicle& veh, Loki::Int2Type< true >)
        {
            return veh.pos() <= posM;
        }

    bool isActive(MSVehicle& veh, double newPos, Loki::Int2Type< false >)
        {
            if ( newPos - veh.length() > posM ) {
                // crossSection completely left
                collectorM.leave( veh );
                return false;
            }
            // crossSection partially left
            return true;
        }

    bool isActivated(MSVehicle& veh, Loki::Int2Type< false >)
        {
            return veh.pos() - veh.length() <= posM;
        }


    MSE3Collector& collectorM;
    MSUnit::Cells posM;
};

namespace Detector
{
    typedef MSE3MoveReminder< true > E3EntryReminder;
    typedef MSE3MoveReminder< false > E3LeaveReminder;
}



// Local Variables:
// mode:C++
// End:

#endif // MSE3MOVEREMINDER_H
