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
//
/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


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
    void execute(EventType et/*, const std::string &id*/);
    friend class NLDiscreteEventBuilder;
private:
    void add(EventType et, Action *a);
//    void add(EventType et, const std::string &id, Action *a);

/*    class TypedDiscreteEventCont {
    public:
//        TypedDiscreteEventCont();
        TypedDiscreteEventCont(Action *a);
        TypedDiscreteEventCont(const std::string &id, Action *a);
        ~TypedDiscreteEventCont();
        void execute();
        void add(Action *a);
    private:
        bool myAllowAll;
        typedef std::set<std::string> AllowedDefinition;
        AllowedDefinition myAllowed;
        typedef std::vector<Action*> ActionVector;
        ActionVector myActions;
    };
*/
    typedef std::vector<Action*> ActionVector;
    typedef std::map<EventType, ActionVector> TypedEvents;
    TypedEvents myEventsForAll;
//    TypedEvents myEventsForSet;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:
