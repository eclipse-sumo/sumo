#ifndef GUILaneStateBounderiesStorage_h
#define GUILaneStateBounderiesStorage_h

#include <map>
#include <microsim/MS_E2_ZS_Collector.h>

class GUILaneStateBounderiesStorage {
public:
    GUILaneStateBounderiesStorage();
    ~GUILaneStateBounderiesStorage();
    void addValue(MS_E2_ZS_Collector::DetType type, double val);
    double timeStepNorm(double val, MS_E2_ZS_Collector::DetType type);
    void initStep();
private:
    typedef std::map<MS_E2_ZS_Collector::DetType, double> ValueMap;
    ValueMap myTimeGlobalMaxes;
    ValueMap myTimeGlobalMins;
    ValueMap myTimeStepMaxes;
    ValueMap myTimeStepMins;
    bool myAmGlobalInitialised;
    bool myAmStepInitialised;

};

#endif

