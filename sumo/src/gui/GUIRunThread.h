#ifndef GUIRunThread_h
#define GUIRunThread_h

#include <string>
#include <iostream>
#include <qthread.h>
#include <microsim/MSNet.h>

class GUINet;
class GUIApplicationWindow;

class GUIRunThread : public QThread
{
private:
    GUIApplicationWindow    *_parent;
    GUINet                  *_net;
    MSNet::Time             _simStartTime, _simEndTime;
    std::ostream            *_craw;
    bool                    _halting;
    MSNet::Time             _step;
    bool                    _quit;
    bool                    _simulationInProgress;
    long                    _sleepPeriod;
    bool                    _single;
public:
    GUIRunThread(GUIApplicationWindow *mw, long sleepPeriod);
    ~GUIRunThread();
    void init(GUINet *net, long start, long end, std::ostream *craw);
    void run();
    void resume();
    void singleStep();
    void begin();
    void stop();
    bool simulationAvailable() const;
    void deleteSim();
    MSNet::Time getCurrentTimeStep() const;
    GUINet &getNet() const;
public slots:
    void setSimulationDelay(int value);

};

#endif
