#ifndef GUILaneStateReporter_h
#define GUILaneStateReporter_h

#include <microsim/MS_E2_ZS_Collector.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <microsim/MSNet.h>
#include <microsim/MSUpdateEachTimestep.h>

class GUILaneStateReporter : public MS_E2_ZS_Collector,
    public MSUpdateEachTimestep<GUILaneStateReporter>
{
public:
    GUILaneStateReporter(LoggedValue_TimeFloating<double> *retriever,
        const std::string &id, MSLane *lane, MSNet::Time interval);
    ~GUILaneStateReporter();
    bool updateEachTimestep( void );
protected:
    LoggedValue_TimeFloating<double> *myRetriever;

};

#endif

