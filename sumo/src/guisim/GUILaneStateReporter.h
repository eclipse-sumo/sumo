#ifndef GUILaneStateReporter_h
#define GUILaneStateReporter_h

#include <microsim/MSLaneState.h>
#include <utils/logging/LoggedValue_TimeFloating.h>
#include <microsim/MSNet.h>

class GUILaneStateReporter : public MSLaneState
{
public:
    GUILaneStateReporter(LoggedValue_TimeFloating<double> *retriever,
        const std::string &id, MSLane *lane, double beginpos, double length,
        MSNet::Time interval);
    ~GUILaneStateReporter();
protected:
    bool actionAfterMoveAndEmit( void );

protected:
    LoggedValue_TimeFloating<double> *myRetriever;
};

#endif

