//---------------------------------------------------------------------------//
//                        ValueTimeLine.cpp -
//  A list of time ranges with float values
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002 by Daniel Krajzewicz
//  organisation         : IVF/DLR http://ivf.dlr.de
//  email                : Daniel.Krajzewicz@dlr.de
//---------------------------------------------------------------------------//

//---------------------------------------------------------------------------//
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//---------------------------------------------------------------------------//
namespace
{
    const char rcsid[] =
    "$Id$";
}
// $Log$
// Revision 1.2  2003/02/07 10:52:57  dkrajzew
// updated
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "ValueTimeLine.icc"
//#endif

// Local Variables:
// mode:C++
// End:


