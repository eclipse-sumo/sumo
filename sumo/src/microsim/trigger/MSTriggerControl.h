#ifndef MSTriggerControl_h
#define MSTriggerControl_h


#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include <vector>

class MSTrigger;




class MSTriggerControl {
public:
    MSTriggerControl(/*const std::vector<MSTrigger*> &trigger*/);
    ~MSTriggerControl();
    /// adds a trigger
    void addTrigger(MSTrigger *t);
    void addTrigger(const std::vector<MSTrigger*> &all);

    MSTrigger *getTrigger(const std::string &id);

    /// Definition of the container for items to initialise before starting
    typedef std::vector<MSTrigger*> TriggerVector;

protected:
    TriggerVector myTrigger;
};

#endif
