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
#include <vector>
#include <cassert>
#include <algorithm>
#include <functional>
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
 * values. The container is sorted after the the first value of the
 * time-range while being filled. Ranges with equivalent first values
 * are sorted in the order in which they were inserted.
 */
template<typename T>
class ValueTimeLine {
public:
    /// Definition of a time period
    typedef std::pair<SUMOTime, SUMOTime> TimeRange;

    /// Definition of a time range with an assigned value
    typedef std::pair<TimeRange, T> ValuedTimeRange;

    /// A list of time periods having which have values assigned
    typedef std::vector<ValuedTimeRange> TimedValueVector;

    typedef typename TimedValueVector::const_iterator CTVVIt;
    typedef typename TimedValueVector::iterator TVVIt;

    /// Return-type of a search, indicating if search was successful.
    typedef std::pair< bool, T > SearchResult;

public:
    /// Constructor
    ValueTimeLine() { }

    /// Destructor
    ~ValueTimeLine() { }

    /// Adds a ValuedTimeRange into the classes container. This is
    /// just a wrapper to addValue(TimeRange range, T value). Make
    /// sure that begin < end.
    ///
    /// @param begin Begin of TimeRange.
    /// @param end End of TimeRange.
    /// @param value Value to store.
    ///
    void add(SUMOTime begin, SUMOTime end, T value) {
        assert(value>=0);
        assert(begin<=end);
        end = end - 1;
        // check whether this is the first entry
        if (myValues.size()==0) {
            // ... simply append it in this case and leave
            myValues.push_back(ValuedTimeRange(TimeRange(begin, end), value));
            assert(!hasOverlaps());
            return;
        }
        // search for the item in front of the current range
        TVVIt begP = find_if(myValues.begin(), myValues.end(), min_finder(begin));
        // check whether it is the last item
        if (myValues.end()==begP) {
            // ... simply append it in this case and leave
            myValues.push_back(ValuedTimeRange(TimeRange(begin, end), value));
            assert(!hasOverlaps());
            return;
        }
        // check whether the time range is the same
        if (begP->first.first==begin&&begP->first.second==end) {
            // ... replace the value and leave then
            begP->second = value;
            assert(!hasOverlaps());
            return;
        }
        // search for the item behind
        TVVIt endP = find_if(begP, myValues.end(), max_finder(end));
        TVVIt insP = begP;
        TVVIt delBegP = begP;
        TVVIt delEndP = endP;
        if (begP->first.first<begin&&begP->first.second>begin) {
            begP->first.second = begin;
            ++delBegP;
        }
        if (endP!=myValues.begin()) {
            --endP;
            if (endP->first.first<end&&endP->first.second>end) {
                endP->first.second = end;
                --delEndP;
            }
        }
        if (distance(delBegP, delEndP)>0) {
            delBegP = myValues.erase(delBegP, delEndP);
        }
        myValues.insert(delBegP, ValuedTimeRange(TimeRange(begin, end), value));
        assert(!hasOverlaps());
        return;

    }

    /// Returns the value for the given time
    T getValue(SUMOTime time) const {
        assert(myValues.size()>0);
        CTVVIt i = std::find_if(
                       myValues.begin(), myValues.end(), range_finder(time));
        if (i==myValues.end()) {
            i = myValues.end() - 1;
        }
        return (*i).second;
    }

    /// returns the information wehther the values for the given time are known
    bool describesTime(SUMOTime time) const {
        CTVVIt i = std::find_if(
                       myValues.begin(), myValues.end(), range_finder(time));
        return(i!=myValues.end());
    }

    bool hasOverlaps() const {
        if (myValues.size()==0) {
            return false;
        }
        CTVVIt i = myValues.begin();
        SUMOTime end = i->first.second;
        ++i;
        for (; i!=myValues.end(); ++i) {
            SUMOTime beg = i->first.first;
            if (beg<end) {
                return true;
            }
            end = i->first.second;
        }
        return false;
    }

    bool empty() const {
        return myValues.size()==0;
    }

    T *buildShortCut(SUMOTime &begin, SUMOTime &end, size_t &maxIndex, SUMOTime &interval) const {
        T *ret;
        // make it simple: assume we use only weights that have the same intervals
        //  in this case!
        assert(myValues.size()!=0);
        assert(!hasOverlaps());
        interval = myValues.begin()->first.second-myValues.begin()->first.first;
        ret = new T[myValues.size()];
        begin = myValues.begin()->first.first;
        end = (myValues.end()-1)->first.second;//begin + myValues.size() * interval;
        for (size_t i=0; i<myValues.size(); i++) {
            ret[i] = myValues[i].second;
        }
        maxIndex = myValues.size() - 1;
        return ret;
    }

private:
    /// Searches for the range when a time point is given
    class range_finder {
    public:
        /** constructor */
        explicit range_finder(SUMOTime time)
                : myTime(time) { }

        /** the comparing function */
        bool operator()(const ValuedTimeRange &vrange) {
            const TimeRange &range = vrange.first;
            return range.first<=myTime && range.second>=myTime;
        }

    private:
        /// The time to search for
        SUMOTime myTime;

    };

    class min_finder {
    public:
        /** constructor */
        explicit min_finder(SUMOTime time)
                : myTime(time) { }

        /** the comparing function */
        bool operator()(const ValuedTimeRange &vrange) {
            const TimeRange &range = vrange.first;
            return range.first<=myTime&&range.second>=myTime || range.first>=myTime;
        }

    private:
        /// The time to search for
        SUMOTime myTime;

    };

    class max_finder {
    public:
        /** constructor */
        explicit max_finder(SUMOTime time)
                : myTime(time) { }

        /** the comparing function */
        bool operator()(const ValuedTimeRange &vrange) {
            const TimeRange &range = vrange.first;
            return range.first>=myTime;
        }

    private:
        /// The time to search for
        SUMOTime myTime;

    };

    class time_sorter {
    public:
        /// constructor
        explicit time_sorter() { }

        int operator()(const ValuedTimeRange &p1, const ValuedTimeRange &p2) {
            return p1.first.first<p2.first.first;
        }
    };


private:
    /// The list of time periods (with values)
    TimedValueVector myValues;

};


#endif

/****************************************************************************/

