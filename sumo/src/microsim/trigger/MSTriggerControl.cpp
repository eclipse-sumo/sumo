#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H

#include <vector>
#include <algorithm>
#include "MSTriggerControl.h"
#include "MSTrigger.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG

using namespace std;

MSTriggerControl::MSTriggerControl(/*const std::vector<MSTrigger*> &trigger*/)
//    : myTrigger(trigger)
{
}


MSTriggerControl::~MSTriggerControl()
{
    for(TriggerVector::iterator i=myTrigger.begin(); i!=myTrigger.end(); ++i) {
        delete (*i);
    }
}


void
MSTriggerControl::addTrigger(MSTrigger *t)
{
    myTrigger.push_back(t);
}


void
MSTriggerControl::addTrigger(const std::vector<MSTrigger*> &all)
{
    copy(all.begin(), all.end(), back_inserter(myTrigger));
}


MSTrigger *
MSTriggerControl::getTrigger(const std::string &id)
{
    for(TriggerVector::iterator i=myTrigger.begin(); i!=myTrigger.end(); ++i) {
        if((*i)->getID()==id) {
            return (*i);
        }
    }
    return 0;
}



