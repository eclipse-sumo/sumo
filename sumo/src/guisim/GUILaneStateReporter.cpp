#include "GUILaneStateReporter.h"
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <gui/GUIGlobals.h>


GUILaneStateReporter::GUILaneStateReporter(
        LoggedValue_TimeFloating<double> *densityRetriever,
        LoggedValue_TimeFloating<double> *speedRetriever,
        LoggedValue_TimeFloating<double> *haltingDurRetriever,
        float &floatingDensity, float &floatingSpeed, float &floatingHaltings,
        const std::string &id, MSLane *lane, MSNet::Time interval)
    : MSE2Collector(id, DU_SUMO_INTERNAL, lane, 0.1, lane->length()-0.2, interval), // !!!
    myDensityRetriever(densityRetriever), mySpeedRetriever(speedRetriever),
    myHaltingDurRetriever(haltingDurRetriever),
    myFloatingDensity(floatingDensity),
    myFloatingSpeed(floatingSpeed),
    myFloatingHaltings(floatingHaltings)

{
    assert(lane->length()>0.2);
    addDetector(E2::DENSITY);
    addDetector(E2::SPACE_MEAN_SPEED);
//    addDetector(E2::HALTING_DURATION_MEAN);
}


GUILaneStateReporter::~GUILaneStateReporter()
{
}


bool
GUILaneStateReporter::updateEachTimestep( void )
{
    // density
    double val = getAggregate(E2::DENSITY, 1);
    if(myDensityRetriever!=0) {
        myDensityRetriever->add(val);
    }
    myFloatingDensity =
        myFloatingDensity * gAggregationRememberingFactor
        + val * (1.0 - gAggregationRememberingFactor);
    // speed
    val = getAggregate(E2::SPACE_MEAN_SPEED, 1);
    if(mySpeedRetriever!=0) {
        mySpeedRetriever->add(val);
    }
    myFloatingSpeed =
        myFloatingSpeed * gAggregationRememberingFactor
        + val * (1.0 - gAggregationRememberingFactor);
    // halts
    val = getAggregate(E2::HALTING_DURATION_MEAN, 1);
    if(myHaltingDurRetriever!=0) {
        myHaltingDurRetriever->add(val);
    }
    myFloatingHaltings =
        myFloatingHaltings * gAggregationRememberingFactor
        + val * (1.0 - gAggregationRememberingFactor);
    return true;
}
