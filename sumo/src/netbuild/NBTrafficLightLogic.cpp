#include <vector>
#include <bitset>
#include <utility>
#include <iostream>
#include <string>
#include <sstream>
#include "NBTrafficLightLogic.h"

using namespace std;

NBTrafficLightLogic::NBTrafficLightLogic(const std::string &key,
                                         size_t noLinks)
    : _key(key), _noLinks(noLinks)
{
}


NBTrafficLightLogic::NBTrafficLightLogic(const NBTrafficLightLogic &s)
    : _key(s._key), _noLinks(s._noLinks), _phases(s._phases)
{
}


NBTrafficLightLogic::~NBTrafficLightLogic()
{
}


void
NBTrafficLightLogic::addStep(size_t duration, 
                             std::bitset<64> driveMask,
                             std::bitset<64> brakeMask)
{
    _phases.push_back(PhaseDefinition(duration, driveMask, brakeMask));
}


void
NBTrafficLightLogic::writeXML(ostream &into, size_t no) const
{
    into << "   <tl-logic>" << endl;
    into << "      <key>" << _key << "</key>" << endl;
    into << "      <logicno>" << no << "</logicno>" << endl;
    into << "      <phaseno>" << _phases.size() << "</phaseno>" << endl;
    for(PhaseDefinitionVector::const_iterator i=_phases.begin(); i!=_phases.end(); i++) {
        std::bitset<64> mask = (*i).driveMask;
    	stringstream tmp1;
	    tmp1 << mask;
        into << "      <phase duration=\"" << (*i).duration
            << "\" phase=\"" << tmp1.str().substr(64-_noLinks) << "\"";
        // by now, only the vehicles that are not allowed to drive are
        //  breaking; later the right-arrow - rule should be concerned
        stringstream tmp2;
        mask = (*i).brakeMask;
        tmp2 << mask;
        into << " brake=\"" << tmp2.str().substr(64-_noLinks) << "\"/>" << endl;
    }
    into << "   </tl-logic>" << endl << endl;
}


void 
NBTrafficLightLogic::_debugWritePhases() const
{
    for(PhaseDefinitionVector::const_iterator i=_phases.begin(); i!=_phases.end(); i++) {
        cout << (*i).duration << "s : " << (*i).driveMask << endl;
    }
}


bool 
NBTrafficLightLogic::equals(const NBTrafficLightLogic &logic) const
{
    if(_phases.size()!=logic._phases.size()) {
        return false;
    }
    PhaseDefinitionVector::const_iterator i, j;
    for(i=_phases.begin(), j=logic._phases.begin(); i!=_phases.end(); i++, j++) {
        if((*i)!=(*j)) {
            return false;
        }
    }
    return true;
}


