#include "GUILaneStateReporter.h"
#include <utils/logging/LoggedValue_TimeFloating.h>


GUILaneStateReporter::GUILaneStateReporter(
        LoggedValue_TimeFloating<double> *retriever,
        const std::string &id, MSLane *lane, MSNet::Time interval)
    : MS_E2_ZS_Collector(id, lane, 0.1, lane->length()-0.2, interval), // !!!
    myRetriever(retriever)
{
    assert(lane->length()>0.2);
    addDetector(MS_E2_ZS_Collector::DENSITY);
}


GUILaneStateReporter::~GUILaneStateReporter()
{
}


bool
GUILaneStateReporter::updateEachTimestep( void )
{
    myRetriever->add(getAggregate(MS_E2_ZS_Collector::DENSITY, 1));
    return true;
}
