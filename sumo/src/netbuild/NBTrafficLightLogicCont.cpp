#include <map>
#include <string>
#include <algorithm>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"
#include "NBTrafficLightLogicCont.h"

using namespace std;

NBTrafficLightLogicCont::ContType NBTrafficLightLogicCont::_cont;

bool
NBTrafficLightLogicCont::insert(const std::string &id,
                                NBTrafficLightLogicVector *logics)
{
    ContType::iterator i=_cont.find(id);
    if(i!=_cont.end()) {
        _cont[id]->add(*logics);
    }
    _cont[id] = logics;
    return true;
}


void
NBTrafficLightLogicCont::writeXML(std::ostream &into)
{
    for(ContType::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i).second->writeXML(into);
    }
    into << endl;
}


void
NBTrafficLightLogicCont::clear()
{
    for(ContType::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i).second;
    }
    _cont.clear();
}
