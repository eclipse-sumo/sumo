#include "LoggedValue.h"
#include "LoggedValue_TimeFloating.h"

template<typename _T>
LoggedValue_TimeFloating<_T>::LoggedValue_TimeFloating(size_t sampleInterval)
    : LoggedValue<_T>(sampleInterval),
    myFloatingArray(new _T[sampleInterval]),
    mySampleInterval(sampleInterval),
    myCurrentValue(0),
    mySampledUnits(0),
    myBufferWasFull(false)
{
    for(size_t i=0; i<sampleInterval; i++) {
        myFloatingArray[i] = 0;
    }
}


template<typename _T>
LoggedValue_TimeFloating<_T>::~LoggedValue_TimeFloating()
{
    delete myFloatingArray;
}


template<typename _T>
void 
LoggedValue_TimeFloating<_T>::add(_T value)
{
    // remove the value lying some steps ahead
    myCurrentValue -= myFloatingArray[mySampledUnits];
    // add the current value
    myCurrentValue += value;
    // store the current value
    myFloatingArray[mySampledUnits++] = value;
    // check whether the number of sampled units exceeds the array
    if(mySampledUnits>mySampleInterval) {
        mySampledUnits = 0;
        // set the information that the buffer was full
        bool myBufferWasFull = true;
    }
}


template<typename _T>
_T 
LoggedValue_TimeFloating<_T>::getAvg() const
{
    // the list is complete
    if(myBufferWasFull) {
        return myCurrentValue / (double) mySampleInterval;
    }
    // return only the acquired values
    if(mySampledUnits!=0) {
        return myCurrentValue / (double) mySampledUnits;
    }
    // return 0 (or throw an exception when not initialised
    return 0;
}


template<typename _T>
_T 
LoggedValue_TimeFloating<_T>::getAbs() const
{
    return myCurrentValue;
}
