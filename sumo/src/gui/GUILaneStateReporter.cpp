#include "GUILaneStateReporter.h"
#include <utils/logging/LoggedValue_TimeFloating.h>

GUILaneStateReporter::GUILaneStateReporter(
        LoggedValue_TimeFloating<double> *retriever,
        const std::string &id, MSLane *lane, double beginpos,
        double length, MSNet::Time interval)
    : MSLaneState(id, lane, beginpos, length, interval), myRetriever(retriever)
{
}


GUILaneStateReporter::~GUILaneStateReporter()
{

}



bool
GUILaneStateReporter::actionAfterMoveAndEmit( )
{
    MSLaneState::actionAfterMoveAndEmit();
    myRetriever->add(getCurrentDensity());
    return true;
}
