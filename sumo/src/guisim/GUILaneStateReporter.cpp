#include "GUILaneStateReporter.h"
#include <guisim/GUILaneStateBounderiesStorage.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>


GUILaneStateReporter::GUILaneStateReporter(
        LoggedValue_TimeFloating<double> *retriever,
        const std::string &id, MSLane *lane, MSNet::Time interval,
        GUILaneStateBounderiesStorage &storage)
    : MS_E2_ZS_Collector(id, lane, 0.1, lane->length()-0.2, interval), // !!!
    myRetriever(retriever), myBounderiesStorage(storage)
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
    double val = getAggregate(MS_E2_ZS_Collector::DENSITY, 1);
    myBounderiesStorage.addValue(MS_E2_ZS_Collector::DENSITY, val);
    myRetriever->add(val);
    return true;
}
