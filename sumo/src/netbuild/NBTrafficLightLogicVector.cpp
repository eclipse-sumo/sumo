#include <vector>
#include <iostream>
#include "NBTrafficLightLogic.h"
#include "NBTrafficLightLogicVector.h"

NBTrafficLightLogicVector::NBTrafficLightLogicVector()
{
}

NBTrafficLightLogicVector::~NBTrafficLightLogicVector()
{
    for(LogicVector::iterator i=_cont.begin(); i!=_cont.end(); i++) {
        delete (*i);
    }
}


void 
NBTrafficLightLogicVector::add(NBTrafficLightLogic *logic)
{
    if(!contains(logic)) {
        _cont.push_back(logic);
    } else {
        delete logic;
    }
}


void 
NBTrafficLightLogicVector::add(const NBTrafficLightLogicVector &cont)
{
    for(LogicVector::const_iterator i=cont._cont.begin(); i!=cont._cont.end(); i++) {
        if(!contains(*i)) {
            NBTrafficLightLogic *logic = *i;
            add(new NBTrafficLightLogic(*logic));
        }
    }
}


void 
NBTrafficLightLogicVector::writeXML(std::ostream &os) const
{
    size_t pos = 0;
    for(LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
        (*i)->writeXML(os, pos++);
    }
}

bool 
NBTrafficLightLogicVector::contains(NBTrafficLightLogic *logic) const
{
    for(LogicVector::const_iterator i=_cont.begin(); i!=_cont.end(); i++) {
        if((*i)->equals(*logic)) {
            return true;
        }
    }
    return false;
}

