#ifndef MSDiscreteEventControl_h
#define MSDiscreteEventControl_h
//---------------------------------------------------------------------------//
//                        MSDiscreteEventControl.h -
//
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Fri, 29.04.2005
//  copyright            : (C) 2005 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
// $Log$
// Revision 1.6  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.5  2005/09/15 11:07:14  dkrajzew
// LARGE CODE RECHECK
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

#include <vector>
#include <string>
#include <map>

class Action;

class MSDiscreteEventControl {
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
    bool hasAnyFor(EventType et);
    void execute(EventType et);
    friend class NLDiscreteEventBuilder;
private:
    void add(EventType et, Action *a);
    typedef std::vector<Action*> ActionVector;
    typedef std::map<EventType, ActionVector> TypedEvents;
    TypedEvents myEventsForAll;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
