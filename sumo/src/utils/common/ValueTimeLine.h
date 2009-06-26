/****************************************************************************/
/// @file    ValueTimeLine.h
/// @author  Christian Roessel
/// @date    Sept 2002
/// @version $Id$
///
// A list of time ranges with SUMOReal values
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef ValueTimeLine_h
#define ValueTimeLine_h


// ===========================================================================
// included modules
// ===========================================================================
#include <map>
#include <cassert>
#include <utility>
#include <utils/common/SUMOTime.h>

#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ValueTimeLine
 *
 * A time line being a sorted container of time-ranges with assigned
 * values. The container is sorted by the first value of the
 * time-range while being filled. Ranges with equivalent first values
 * are sorted in the order in which they were inserted.
 */
template<typename T>
class ValueTimeLine {
public:
    /// Value of time line, indicating validity.
    typedef std::pair<bool, T> ValidValue;

    /// Sorted map from start of intervals to values.
    typedef std::map<SUMOTime, ValidValue> TimedValueMap;

public:
    /// Constructor
    ValueTimeLine() { }

    /// Destructor
    ~ValueTimeLine() { }

    /// Adds a value for a time interval into the container. Make
    /// sure that begin >= 0 and begin < end.
    ///
    /// @param begin Begin of TimeRange.
    /// @param end End of TimeRange.
    /// @param value Value to store.
    ///
    void add(SUMOTime begin, SUMOTime end, T value) {
        assert(begin>=0);
        assert(begin<end);
        // inserting strictly before the first or after the last interval (includes empty case)
        if (myValues.upper_bound(begin) == myValues.end() ||
            myValues.upper_bound(end) == myValues.begin()) {
            myValues[begin] = std::make_pair(true, value);
            myValues[end] = std::make_pair(false, value);
            return;
        }
        // our end already has a value
        typename TimedValueMap::iterator endIt = myValues.find(end);
        if (endIt != myValues.end()) {
            myValues.erase(myValues.upper_bound(begin), endIt);
            myValues[begin] = std::make_pair(true, value);
            return;
        }
        // we have at least one entry strictly before our end
        endIt = myValues.lower_bound(end);
        --endIt;
        ValidValue oldEndValue = endIt->second;
        myValues.erase(myValues.upper_bound(begin), myValues.lower_bound(end));
        myValues[begin] = std::make_pair(true, value);
        myValues[end] = oldEndValue;
    }

    /// Returns the value for the given time
    T getValue(SUMOTime time) const {
        assert(myValues.size()!=0);
        typename TimedValueMap::const_iterator it = myValues.upper_bound(time);
        assert(it != myValues.begin());
        --it;
        return it->second.second;
    }

    /// returns the information whether the values for the given time are known
    bool describesTime(SUMOTime time) const {
        typename TimedValueMap::const_iterator afterIt = myValues.upper_bound(time);
        if (afterIt == myValues.begin()) {
            return false;
        }
        --afterIt;
        return afterIt->second.first;
    }

    /// fills all intervals where no value is set with the given one
    void fillGaps(T value) {
        for (typename TimedValueMap::iterator it = myValues.begin(); it != myValues.end(); ++it) {
            if (!it->second.first) {
                it->second.second = value;
            }
        }
        myValues[-1] = std::make_pair(true, value);
    }

private:
    /// The list of time periods (with values)
    TimedValueMap myValues;

};


#endif

/****************************************************************************/
