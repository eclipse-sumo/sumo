#ifndef ValueTimeLine_h
#define ValueTimeLine_h
//---------------------------------------------------------------------------//
//                        ValueTimeLine.h -
//  A list of time ranges with float values
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Sept 2002
//  copyright            : (C) 2002, 2003, 2004 by Daniel Krajzewicz &
//                       : Christian Roessel <christian.roessel@dlr.de>
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

/// @version $Id$

/* =========================================================================
 * included modules
 * ======================================================================= */
#include <vector>
#include <cassert>
#include <algorithm>
#include <functional>
#include <utility>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif // HAVE_CONFIG_H


/* =========================================================================
 * class definitions
 * ======================================================================= */
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
    typedef std::pair<unsigned int, unsigned int> TimeRange;

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
    void append(unsigned int begin, unsigned int end, T value)
    {
        assert(begin<=end);
        assert(myValues.size()==0||(*(myValues.end()-1)).first.first<=begin);
        myValues.push_back(ValuedTimeRange(TimeRange(begin, end), value));
    }

    /// Adds a ValuedTimeRange into the classes container. The
    /// container is sorted after the begin of the TimeRange.
    ///
    /// @param range TimeRange. Assure that begin < end.
    /// @param value Value to store.
    ///
    /*
    void addValue(TimeRange range, T value)
    {
        assert( range.first < range.second );
        unsigned intervalStart = range.first;
        TVVIt insertPos = std::upper_bound( myValues.begin(), myValues.end(),
                                            intervalStart, TimeRangeLess() );
        myValues.insert( insertPos, std::make_pair( range, value ) );
    }
    */

    /// Returns the value for the given time
    T getValue(unsigned int time) const
    {
        assert(myValues.size()>0);
        CTVVIt i = std::find_if(
            myValues.begin(), myValues.end(), range_finder(time));
        if(i==myValues.end()) {
            i = myValues.end() - 1;
        }
        return (*i).second;
    }

    /// Searches for a TimeRange in it's TimedValueVector myValues
    /// that contains the parameter time. If there are several
    /// possible TimeRanges, one of them will be returned, depending on
    /// the boundary conditions. But in this case, fix the input!
    ///
    /// @param time Search parameter.
    ///
    /// @return A pair SearchResult, indicating if the search was
    /// successful (first). In this case the associated value is
    /// returned as "second". In the other case, the value of the last
    /// element is returned.
    ///
    SearchResult getSearchStateAndValue( unsigned time ) const
        {
            if ( myValues.size() == 0 ) {
                return std::make_pair( false, T() );
            }
            CTVVIt retIt = std::lower_bound( myValues.begin(), myValues.end(),
                                             time, TimeRangeLess() );

            if ( time < retIt->first.first ) {
                if ( retIt == myValues.begin() ) {
                    return std::make_pair( false, retIt->second );
                }
                --retIt;
            }
            if ( time <= retIt->first.second ) {
                return std::make_pair( true, retIt->second );
            }
            if ( retIt == myValues.end() &&
                 time <= (--retIt)->first.second ) {
                return std::make_pair( true, retIt->second );
            }
            retIt = myValues.end() - 1;
            return std::make_pair( false, retIt->second );
        }

    /// Returns the number of known periods
    size_t noDefinitions() const
    {
        return myValues.size();
    }

    /// Returns the time period description at the given position //
    /// !!! should not be public
    const TimeRange &getRangeAtPosition(size_t pos) const
    {
        assert( pos < myValues.size() );
        return myValues[pos].first;
    }

    /// returns the information wehther the values for the given time are known
    bool describesTime(unsigned int time) const
    {
        CTVVIt i = std::find_if(
            myValues.begin(), myValues.end(), range_finder(time));
        return(i!=myValues.end());
    }

    bool hasOverlaps() {
        if(myValues.size()==0) {
            return false;
        }
        CTVVIt i = myValues.begin();
        unsigned int end = i->first.second;
        ++i;
        for(; i!=myValues.end(); ++i) {
            unsigned int beg = i->first.first;
            if(beg<=end) {
                return true;
            }
            end = i->first.second;
        }
        return false;
    }

    void sort() {
        std::sort(myValues.begin(), myValues.end(), time_sorter());
        assert(!hasOverlaps());
    }

    bool empty() const {
        return myValues.size()==0;
    }

    const ValuedTimeRange &getLastRange() const {
        assert(!empty());
        return *(myValues.end()-1);
    }

private:
    /// Searches for the range when a time point is given
    class range_finder {
    public:
        /** constructor */
        explicit range_finder(unsigned int time)
            : myTime(time) { }

        /** the comparing function */
        bool operator() (const ValuedTimeRange &vrange) {
            const TimeRange &range = vrange.first;
            return range.first<=myTime && range.second>=myTime;
        }

    private:
        /// The time to search for
        unsigned int myTime;

    };

    class time_sorter {
    public:
        /// constructor
        explicit time_sorter() { }

        int operator() (const ValuedTimeRange &p1, const ValuedTimeRange &p2) {
            return p1.first.first<p2.first.first;
        }
    };

    /// Predicates for (binary) ordering and searching in a
    /// TimedValueVector. The first value of TimeRange is used for
    /// ordering and searching. If neccessary, check for the second
    /// one yourself.
    struct TimeRangeLess :
        std::binary_function< unsigned, ValuedTimeRange, bool >
    {
        /// predicate to use with upper_bound.
        bool operator()( const unsigned searchTime,
                         const ValuedTimeRange& valuedRange ) const
            {
                return searchTime < valuedRange.first.first;
            }
        /// predicate to use with lower_bound
        bool operator()( const ValuedTimeRange& valuedRange,
                         const unsigned searchTime ) const
            {
                return searchTime > valuedRange.first.first;
            }
    };


private:
    /// The list of time periods (with values)
    TimedValueVector myValues;

};

#include "ValueTimeLine.cpp"

/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

