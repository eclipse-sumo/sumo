#ifndef GUILaneStateReporter_h
#define GUILaneStateReporter_h

#include <microsim/MSE2Collector.h>
#include <microsim/logging/LoggedValue_TimeFloating.h>
#include <microsim/MSNet.h>
#include <microsim/MSUpdateEachTimestep.h>

class GUILaneStateBounderiesStorage;

class GUILaneStateReporter : public MSE2Collector,
    public MSUpdateEachTimestep<GUILaneStateReporter>
{
public:
    GUILaneStateReporter(LoggedValue_TimeFloating<double> *retriever,
        const std::string &id, MSLane *lane, MSNet::Time interval,
        GUILaneStateBounderiesStorage &storage);
    ~GUILaneStateReporter();
    bool updateEachTimestep( void );
protected:
    LoggedValue_TimeFloating<double> *myRetriever;
    GUILaneStateBounderiesStorage &myBounderiesStorage;

};

#endif

