#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>
#include "NIArtemisTempSignal.h"




NIArtemisTempSignal::PhaseDescription::PhaseDescription(double perc, 
                                                        size_t start)
    : myPercentage(perc), myBegin(start)
{
}


NIArtemisTempSignal::PhaseDescription::~PhaseDescription()
{
}




void 
NIArtemisTempSignal::addPhase(const std::string &nodeid, 
                              char phaseName, double perc, int start)
{
    throw 1;
}

void 
NIArtemisTempSignal::addConnectionPhases(const Connection &c,
                                         char startPhase, char endPhase, 
                                         const std::string &group)
{
    throw 1;
}


void 
NIArtemisTempSignal::close()
{
    throw 1;
}
