#include "MSDiscreteEventControl.h"
#include "Action.h"

MSDiscreteEventControl::MSDiscreteEventControl()
{
}


MSDiscreteEventControl::~MSDiscreteEventControl()
{
}


bool
MSDiscreteEventControl::hasAnyFor(EventType et)
{
    return myEventsForAll.find(et)!=myEventsForAll.end()/*
        &&
        myEventsForSet.find(et)!=myEventsForSet.end()*/;
}


void
MSDiscreteEventControl::execute(EventType et/*, const std::string &id*/)
{
    TypedEvents::iterator i = myEventsForAll.find(et);
    if(i!=myEventsForAll.end()) {
        const ActionVector &av = (*i).second;
        for(ActionVector::const_iterator j=av.begin(); j!=av.end(); j++) {
            (*j)->execute();
        }
    }
}



void
MSDiscreteEventControl::add(EventType et, Action *a)
{
    if(myEventsForAll.find(et)==myEventsForAll.end()) {
        myEventsForAll[et] = ActionVector();
    }
    myEventsForAll[et].push_back(a);
}

/*
void
MSDiscreteEventControl::add(EventType et, const std::string &id, Action &act)
{
    if(myEventsForSet.find(et)==myEventsForAll.end()) {
        myEventsForSet[et] = TypedDiscreteEventCont(id, a);
    } else {
        TypedEvents::iterator i = myEventsForSet.find(et);
        while(i!=myEventsForSet.end()) {
            if(!(*i).second.matches(id)) {
                i =
                continue;
            }
            myEventsForSet[et].add(a);
        }
    }
}
*/

