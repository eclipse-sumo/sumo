#ifndef MSDiscreteEventControl_h
#define MSDiscreteEventControl_h

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

#endif
