#include <string>
#include <iostream>
#include <qthread.h>
#include <guisim/GUINet.h>
#include "QSimulationStepEvent.h"
#include "GUIApplicationWindow.h"
#include "GUIRunThread.h"

GUIRunThread::GUIRunThread(GUIApplicationWindow *parent, long sleepPeriod)
    : _parent(parent),
    _net(0), _craw(0), _quit(false), _simulationInProgress(false),
    _sleepPeriod(sleepPeriod)
{
}


GUIRunThread::~GUIRunThread()
{
}


void
GUIRunThread::init(GUINet *net, long start, long end, std::ostream *craw)
{
    deleteSim();
    _net = net;
    _craw = craw;
    _simStartTime = start;
    _simEndTime = end;
    _step = start;
}


void
GUIRunThread::run()
{
    while(!_quit) {
        while(_net==0) {
            msleep(500);
        }
        if(!_halting&&_net!=0) {
            _simulationInProgress = true;
            _net->simulationStep(_craw, _simStartTime, _step);
            QThread::postEvent( _parent, new QSimulationStepEvent() );
            msleep(_sleepPeriod);
            _step++;
            if(_step==_simEndTime) {
                _halting = true;
            }
            if(_single) {
                _halting = true;
            }
            _simulationInProgress = false;
        }
    }
    deleteSim();
}


void
GUIRunThread::resume()
{
    if(_step<_simEndTime) {
        _single = false;
        _halting = false;
    }
}


void 
GUIRunThread::singleStep()
{
    _single = true;
    _halting = false;
}


void
GUIRunThread::begin()
{
    _step = _simStartTime;
    _net->preStartInit();
    _single = false;
    _halting = false;
}


void
GUIRunThread::stop()
{
    _single = false;
    _halting = true;
}


bool
GUIRunThread::simulationAvailable() const
{
    return _net!=0;
}


void
GUIRunThread::deleteSim()
{
    _halting = true;
    while(_simulationInProgress);
    delete _net;
    _net = 0;
    delete _craw;
    _craw = 0;
}


GUINet &
GUIRunThread::getNet() const
{
    return *_net;
}


void
GUIRunThread::setSimulationDelay(int value)
{
    _sleepPeriod = value;
}


MSNet::Time 
GUIRunThread::getCurrentTimeStep() const
{
    return _step;
}


