#ifndef QSimulationLoadedEvent_h
#define QSimulationLoadedEvent_h

#include <string>
#include <qevent.h>
#include <iostream>
#include <microsim/MSNet.h>
#include "QSUMOEvent.h"
#include "GUIEvents.h"

class GUINet;

class QSimulationLoadedEvent : public QSUMOEvent {
public:
    GUINet          *_net;
    std::ostream    *_craw;
    MSNet::Time     _begin;
    MSNet::Time     _end;
    std::string     _file;
public:
    QSimulationLoadedEvent(GUINet *net, std::ostream *craw,
        MSNet::Time startTime, MSNet::Time endTime, 
        const std::string &file) 
        : QSUMOEvent(EVENT_SIMULATION_LOADED),
        _net(net), _craw(craw), _begin(startTime), _file(file) { }
    ~QSimulationLoadedEvent() { }
};

#endif
