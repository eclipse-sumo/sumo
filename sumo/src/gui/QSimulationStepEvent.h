#ifndef QSimulationStepEvent_h
#define QSimulationStepEvent_h

#include "QSUMOEvent.h"
#include "GUIEvents.h"

class QSimulationStepEvent : public QSUMOEvent {
public:
    QSimulationStepEvent() 
        : QSUMOEvent(EVENT_SIMULATION_STEP) { }
    ~QSimulationStepEvent() { }
};

#endif
