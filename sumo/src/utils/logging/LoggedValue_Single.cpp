#include "LoggedValue.h"
#include "LoggedValue_Single.h"

template<typename _T>
LoggedValue_Single<_T>::LoggedValue_Single<_T>(size_t dummy)
    : LoggedValue<_T>(dummy)
{
}


template<typename _T>
LoggedValue_Single<_T>::~LoggedValue_Single<_T>()
{
}


template<typename _T>
void 
LoggedValue_Single<_T>::add(_T value)
{
    myCurrentValue = value;
}


template<typename _T>
_T 
LoggedValue_Single<_T>::getAvg() const
{
    return myCurrentValue;
}


template<typename _T>
_T 
LoggedValue_Single<_T>::getAbs() const
{
    return myCurrentValue;
}


