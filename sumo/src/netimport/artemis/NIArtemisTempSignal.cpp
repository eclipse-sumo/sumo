#include <string>
#include <map>
#include <netbuild/NBConnectionDefs.h>
#include "NIArtemisTempSignal.h"

NIArtemisTempSignal::NodeToDescsMap     NIArtemisTempSignal::myDescDict;
NIArtemisTempSignal::ConnectionToPhases NIArtemisTempSignal::myPhasesDict;

NIArtemisTempSignal::PhaseDescription::PhaseDescription(double perc,
                                                        size_t start)
    : myPercentage(perc), myBegin(start)
{
}


NIArtemisTempSignal::PhaseDescription::~PhaseDescription()
{
}




NIArtemisTempSignal::ConnectionPhases::ConnectionPhases(char startPhase,
                                                        char endPhase)
    : myStartPhase(startPhase), myEndPhase(endPhase)
{
}


NIArtemisTempSignal::ConnectionPhases::~ConnectionPhases()
{
}





void
NIArtemisTempSignal::addPhase(const std::string &nodeid,
                              char phaseName, double perc, int start)
{
    myDescDict[nodeid][phaseName] =
        new PhaseDescription(perc, start);
}

void
NIArtemisTempSignal::addConnectionPhases(const Connection &c,
                                         char startPhase, char endPhase,
                                         const std::string &group)
{
    myPhasesDict[c] =
        new ConnectionPhases(startPhase, endPhase);
}


void
NIArtemisTempSignal::close()
{
    //throw 1;
}
