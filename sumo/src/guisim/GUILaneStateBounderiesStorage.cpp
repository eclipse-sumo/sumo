#include "GUILaneStateBounderiesStorage.h"


GUILaneStateBounderiesStorage::GUILaneStateBounderiesStorage()
    : myAmGlobalInitialised(false), myAmStepInitialised(false)
{
}


GUILaneStateBounderiesStorage::~GUILaneStateBounderiesStorage()
{
}


void
GUILaneStateBounderiesStorage::addValue(MS_E2_ZS_Collector::DetType type,
                                        double val)
{
    // for all-the-time values
    if(!myAmGlobalInitialised) {
        myAmGlobalInitialised = true;
        myTimeGlobalMaxes[type] = val;
        myTimeGlobalMins[type] = val;
    } else {
        if(myTimeGlobalMaxes[type]<val) {
            myTimeGlobalMaxes[type] = val;
        }
        if(myTimeGlobalMins[type]>val) {
            myTimeGlobalMins[type] = val;
        }
    }
    // for step values
    if(!myAmStepInitialised) {
        myAmStepInitialised = true;
        myTimeStepMaxes[type] = val;
        myTimeStepMins[type] = val;
    } else {
        if(myTimeStepMaxes[type]<val) {
            myTimeStepMaxes[type] = val;
        }
        if(myTimeStepMins[type]>val) {
            myTimeStepMins[type] = val;
        }
    }
}


double
GUILaneStateBounderiesStorage::timeStepNorm(double val,
                                            MS_E2_ZS_Collector::DetType type)
{
    val -= myTimeStepMins[type];
    return val /= (myTimeStepMaxes[type]-myTimeStepMins[type]);
}


void
GUILaneStateBounderiesStorage::initStep()
{
    myAmStepInitialised = false;
}
