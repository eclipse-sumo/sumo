#ifndef NBTrafficLightLogicCont_h
#define NBTrafficLightLogicCont_h

#include <map>
#include <string>
#include "NBTrafficLightLogicVector.h"

class NBTrafficLightLogicCont {
private:
    typedef std::map<std::string, NBTrafficLightLogicVector*> ContType;
    static ContType _cont;
public:
    static bool insert(const std::string &id, 
        NBTrafficLightLogicVector *logics);
    /// saves all known logics
    static void writeXML(std::ostream &into);
    /// destroys all stored logics
    static void clear();
};

#endif
