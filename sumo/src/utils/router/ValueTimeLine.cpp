#include <utility>
#include <vector>
#include <algorithm>
#include <cassert>
#include "ValueTimeLine.h"

using namespace std;

ValueTimeLine::ValueTimeLine()
{
}


ValueTimeLine::~ValueTimeLine()
{
}


void
ValueTimeLine::addValue(long begin, long end, float value)
{
    addValue(TimeRange(begin, end), value);
}


void 
ValueTimeLine::addValue(TimeRange range, float value)
{
    ValuedTimeRange valued(range, value);
    _values.push_back(valued);
}

float 
ValueTimeLine::getValue(long time) const
{
    assert(_values.size()>0);
    TimedValueVector::const_iterator i = 
        find_if(_values.begin(), _values.end(), range_finder(time));
    if(i==_values.end()) {
        i = _values.end() - 1;
    }
    return (*i).second;
}


size_t 
ValueTimeLine::noDefinitions() const
{
    return _values.size();
}


const ValueTimeLine::TimeRange &
ValueTimeLine::getRangeAtPosition(size_t pos) const
{
    return _values[pos].first;
}
