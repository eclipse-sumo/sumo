#include "LoggedValue.h"
#include "LoggedValue_TimeFixed.h"

template<typename _T>
LoggedValue_TimeFixed<_T>::LoggedValue_TimeFixed<_T>(size_t sampleInterval)
    : LoggedValue<_T>(sampleInterval),
    mySampleInterval(sampleInterval),
    mySampledUnits(0)
{
}


template<typename _T>
LoggedValue_TimeFixed<_T>::~LoggedValue_TimeFixed<_T>()
{
}


template<typename _T>
void 
LoggedValue_TimeFixed<_T>::add(_T value)
{
    mySampledUnits++;
    // clear on a new interval
    if(mySampledUnits>mySampleInterval) {
        mySampledUnits = 1;
        myCurrentValue = 0;
    }
    // add new value
    myCurrentValue += value;
}


template<typename _T>
_T 
LoggedValue_TimeFixed<_T>::getAvg() const
{
    // Security check for false interval usage
    if(mySampledUnits==0) {
        return 0; // !!! You could also throw an exception here;
        // This is not meant to be done
    }
    return myCurrentValue / (double) mySampledUnits;
}


template<typename _T>
_T 
LoggedValue_TimeFixed<_T>::getAbs() const
{
    return myCurrentValue;
}


