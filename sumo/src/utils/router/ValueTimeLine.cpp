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
// $Log$
// Revision 1.3  2004/01/26 07:17:49  dkrajzew
// "specialisation" of the time line to handle interpolation added
//
// Revision 1.2  2003/02/07 10:52:57  dkrajzew
// updated
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


/* =========================================================================
 * used namespaces
 * ======================================================================= */
using namespace std;


/* =========================================================================
 * method definitions
 * ======================================================================= */
template<typename T>
ValueTimeLine<T>::ValueTimeLine<T>()
{
}


template<typename T>
ValueTimeLine<T>::~ValueTimeLine<T>()
{
}


template<typename T>
void
ValueTimeLine<T>::addValue(unsigned int begin, unsigned int end, T value)
{
    addValue(TimeRange(begin, end), value);
}


template<typename T>
void
ValueTimeLine<T>::addValue(TimeRange range, T value)
{
    ValuedTimeRange valued(range, value);
    myValues.push_back(valued);
}


template<typename T>
T
ValueTimeLine<T>::getValue(unsigned int time) const
{
    assert(myValues.size()>0);
    TimedValueVector::const_iterator i =
        find_if(myValues.begin(), myValues.end(), range_finder(time));
    if(i==myValues.end()) {
        i = myValues.end() - 1;
    }
    return (*i).second;
}


template<typename T>
size_t
ValueTimeLine<T>::noDefinitions() const
{
    return myValues.size();
}


template<typename T>
const ValueTimeLine<T>::TimeRange &
ValueTimeLine<T>::getRangeAtPosition(size_t pos) const
{
    return myValues[pos].first;
}


template<typename T>
bool
ValueTimeLine<T>::describesTime(unsigned int time) const
{
    TimedValueVector::const_iterator i =
        find_if(myValues.begin(), myValues.end(), range_finder(time));
    return(i!=myValues.end());
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

// Local Variables:
// mode:C++
// End:


