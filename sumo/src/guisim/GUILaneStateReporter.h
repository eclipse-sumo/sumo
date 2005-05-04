#ifndef GUILaneStateReporter_h
#define GUILaneStateReporter_h

/* =========================================================================
 * compiler pragmas
 * ======================================================================= */
#pragma warning(disable: 4786)


#include <microsim/output/e2_detectors/MSE2Collector.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <microsim/MSUpdateEachTimestep.h>


class GUILaneStateReporter : public MSE2Collector,
    public MSUpdateEachTimestep<GUILaneStateReporter>
{
public:
    GUILaneStateReporter(LoggedValue_TimeFloating<double> *densityRetriever,
        LoggedValue_TimeFloating<double> *speedRetriever,
        LoggedValue_TimeFloating<double> *haltingDurRetriever,
        float &floatingDensity, float &floatingSpeed, float &floatingHaltings,
        const std::string &id, MSLane *lane, SUMOTime interval);
    ~GUILaneStateReporter();
    bool updateEachTimestep( void );
protected:
    LoggedValue_TimeFloating<double> *myDensityRetriever;
    LoggedValue_TimeFloating<double> *mySpeedRetriever;
    LoggedValue_TimeFloating<double> *myHaltingDurRetriever;
    float &myFloatingDensity;
    float &myFloatingSpeed;
    float &myFloatingHaltings;

};

#endif

