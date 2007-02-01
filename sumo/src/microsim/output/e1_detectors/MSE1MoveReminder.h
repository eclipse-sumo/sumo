/****************************************************************************/
/// @file    MSE1MoveReminder.h
/// @author  Christian Roessel
/// @date    Wed Jun 9 16:40:56 CEST 2004
/// @version $Id: $
///
// * @author  Christian Roessel <christian.roessel@dlr.de>
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
#ifndef MSE1MoveReminder_h
#define MSE1MoveReminder_h
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

#include <microsim/MSMoveReminder.h>
#include <microsim/MSUnit.h>
#include <microsim/output/MSCrossSection.h>
#include <string>
#ifdef WIN32
#include <utils/helpers/msvc6_TypeManip.h>
#endif
#ifndef WIN32
#include <utils/helpers/gcc_TypeManip.h>
#endif


// ===========================================================================
// class declarations
// ===========================================================================
class MSE1Collector;
class MSLane;
class MSVehicle;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template< bool isEntryReminder >
class MSE1MoveReminder : public MSMoveReminder
{
public:

    MSE1MoveReminder(
        std::string id
        , MSCrossSection crossSection
        , MSE1Collector& collector
    );

    bool isStillActive(
        MSVehicle& veh
        , SUMOReal oldPos
        , SUMOReal newPos
        , SUMOReal
    );

    void dismissByLaneChange(MSVehicle& veh);

    bool isActivatedByEmitOrLaneChange(MSVehicle& veh);

private:

    // EntryReminder overload
    bool isActive(MSVehicle& veh,
                  SUMOReal oldPos ,
                  SUMOReal newPos,
                  Loki::Int2Type< true >);

    // EntryReminder overload
    bool isActivated(MSVehicle& veh, Loki::Int2Type< true >);

    // EntryReminder overload
    void dismiss(MSVehicle& veh, Loki::Int2Type< true >);

    // LeaveReminder overload
    bool isActive(MSVehicle& veh,
                  SUMOReal oldPos ,
                  SUMOReal newPos,
                  Loki::Int2Type< false >);

    // LeaveReminder overload
    bool isActivated(MSVehicle& veh, Loki::Int2Type< false >);

    // LeaveReminder overload
    void dismiss(MSVehicle& veh, Loki::Int2Type< false >);

    MSE1Collector& collectorM;
    SUMOReal posM;
};

namespace Detector
{
typedef MSE1MoveReminder< true > E1EntryReminder;
typedef MSE1MoveReminder< false > E1LeaveReminder;
}


#endif

/****************************************************************************/

