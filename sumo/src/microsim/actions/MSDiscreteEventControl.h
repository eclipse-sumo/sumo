/****************************************************************************/
/// @file    MSDiscreteEventControl.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
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
#ifndef MSDiscreteEventControl_h
#define MSDiscreteEventControl_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <string>
#include <map>
#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class Command;


// ===========================================================================
// class definitions
// ===========================================================================
class MSDiscreteEventControl
{
public:
    enum EventType {
        ET_SIMULATION_BEGIN,
        ET_SIMULATION_END,
        ET_SIMULATION_STEP_BEGIN,
        ET_SIMULATION_STEP_END,

        ET_VEHICLE_ON_EMIT,
        ET_VEHICLE_ON_ROUTE_END,
        ET_VEHICLE_ON_EDGE_ENTRY,
        ET_VEHICLE_ON_EDGE_LEAVING,

        ET_TLL_ON_STEP
    };

    enum EventElements {
        EE_ID,
        EE_VEHICLE_ROUTE_ID,
        EE_VEHICLE_SPEED
    };

    MSDiscreteEventControl();
    ~MSDiscreteEventControl();
    bool hasAnyFor(EventType et, SUMOTime currentTime);
    void execute(EventType et, SUMOTime currentTime);
    friend class NLDiscreteEventBuilder;
private:
    void add(EventType et, Command *a);
    typedef std::vector<Command*> CommandVector;
    typedef std::map<EventType, CommandVector> TypedEvents;
    TypedEvents myEventsForAll;

};


#endif

/****************************************************************************/

