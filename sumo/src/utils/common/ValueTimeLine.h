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
class ValueTimeLine
{
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

        /*

        // check whether the item to insert is beyond the current end
        if(myValues.end()==endP) {
        	// remove the tail in this case
        	if(begP!=myValues.end()-1) {
        		begP = myValues.erase(begP+1, myValues.end()) - 1;
        	}
        	// check whether the found begin shall be prunned
        	if(begP->first.second>begin) {
        		beg->first.second = begin;
        	}
        	myValues.insert(begP+1, ValuedTimeRange(TimeRange(begin, end), value));
        	assert(!hasOverlaps());
        	return;
        }
        endP--;
        if(endP==myValues.begin()||begP==endP) {
        	assert(begP==endP);
        	T oldVal = begP->second;
        	unsigned int oldend = begP->first.second;
        	if(begP->first.first<begin&&begP->first.second>end) {
        		// insert within
        		begP->first.second = begin;
        		begP = myValues.insert(begP+1, ValuedTimeRange(TimeRange(begin, end), value));
        		myValues.insert(begP+1, ValuedTimeRange(TimeRange(end, oldend), oldVal));
        		assert(!hasOverlaps());
        		return;
        	} else if(begP->first.first<begin) {
        		// insert leaving information in front
        		begP->first.second = second;
        		myValues.insert(begP+1, ValuedTimeRange(TimeRange(begin, end), value));
        		assert(!hasOverlaps());
        		return;
        	} else if(begP->first.second>end) {
        		// insert leaving information behind
        		begP->first.first = end;
        		myValues.insert(begP, ValuedTimeRange(TimeRange(begin, end), value));
        		assert(!hasOverlaps());
        		return;
        	}
        	assert(begP->first.first==begin&&begP->first.second==end);
        	begP->second = value;
        	assert(!hasOverlaps());
        	return;
        }
        if(begP->first.first<begin) {
        	// insert leaving information in front
        	begP->first.second = second;
        	myValues.insert(begP+1, ValuedTimeRange(TimeRange(begin, end), value));

        /		// check whether the new item is partially behind the end
        if(endP==myValues.end()) {
        	// crop the old item (which will be the end-2 item)
        	if(begP->first.second>begin) {
        		begP->first.second = begin;
        		begP++;
        	}
        	if(begP!=myValues.end()-1) {
        		myValues.erase(begP, myValues.end());
        	}
        	myValues.push_back(ValuedTimeRange(TimeRange(begin, end), value));
        	return;
        } else {*/
        // check whether the item to insert lies within another range
        /*
        if(begP==endP) {
        		// store temporary the real end of the larger range
        		ValuedTimeRange tmp(TimeRange(end, begP->first.second), begP->second);
        		begP->first.first = begin;
        		begP->first.second = end;
        		begP->second = value;
        		myValues.insert(begP+1, tmp);
        		assert(!hasOverlaps());
        	} else if(begP->first.second<=end) {
        		// store temporary the real end of the larger range
        		ValuedTimeRange tmp(TimeRange(end, begP->first.second), begP->second);
        		begP->first.first = begin;
        		begP->first.second = end;
        		begP->second = value;
        		myValues.insert(begP+1, tmp);
        		assert(!hasOverlaps());
        	} else {
        		// store temporary the real end of the larger range
        		ValuedTimeRange tmp(TimeRange(end, begP->first.second), begP->second);
        		begP->first.second = begin;
        		begP = myValues.insert(begP+1, ValuedTimeRange(TimeRange(begin, end), value));
        		myValues.insert(begP+1, tmp);
        		assert(!hasOverlaps());
        	}
        }
        // crop the old item in front if needed
        if(begP->first.second>begin) {
        	begP->first.second = begin;
        	begP++;
        }
        // ... and the one behind
        if(myValues.end()!=endP&&endP->first.first<end) {
        	endP->first.first = end;
        	endP--;
        }
        begP = myValues.erase(begP, endP);

        /
        	if(begP!=myValues.end()-1) {
        		myValues.erase(begP, endP);
        	}
        	*/
        // ... ok, the new item is somewhere in between
        /*		myValues.insert(begP+1, ValuedTimeRange(TimeRange(begin, end), value));
                assert(!hasOverlaps());
        		*/
//		}
        // hmmm, any other case?
//		throw 1;
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
    T getValue(SUMOTime time) const {
        assert(myValues.size()>0);
        CTVVIt i = std::find_if(
                       myValues.begin(), myValues.end(), range_finder(time));
        if (i==myValues.end()) {
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
    SearchResult getSearchStateAndValue(SUMOTime time) const {
        if (myValues.size() == 0) {
            return std::make_pair(false, T());
        }
        CTVVIt retIt = find_if(myValues.begin(), myValues.end(), min_finder(time));
        // use the last one if no matching range was found
        if (retIt==myValues.end()) {
            return std::make_pair(false, (--retIt)->second);
        }
        // check whether it's within a range
        if (retIt->first.first<=time) {
            return std::make_pair(true, (retIt)->second);
        } else {
            // no, it's within the next range
            if (retIt==myValues.begin()) {
                // use the first one if no other previous exist
                return std::make_pair(false, (retIt)->second);
            }
            // use the previous one otherwise
            return std::make_pair(false, (--retIt)->second);
        }
        throw 1; // any other case?
        /*
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
        */
    }

    /// Returns the number of known periods
    size_t noDefinitions() const {
        return myValues.size();
    }

    /// Returns the time period description at the given position
    const TimeRange &getRangeAtPosition(size_t pos) const {
        assert(pos < myValues.size());
        return myValues[pos].first;
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
    /*
        void sort() {
            std::sort(myValues.begin(), myValues.end(), time_sorter());
            assert(!hasOverlaps());
        }
    */
    bool empty() const {
        return myValues.size()==0;
    }

    const ValuedTimeRange &getLastRange() const {
        assert(!empty());
        return *(myValues.end()-1);
    }


    T *buildShortCut(SUMOTime &begin, SUMOTime &end, size_t &maxIndex, SUMOTime &interval) const {
        T *ret;
        // make it simple: assume we use only weights that have the same intervals
        //  in this case!
        assert(myValues.size()!=0);
        assert(!hasOverlaps());
        interval = myValues.begin()->first.second-myValues.begin()->first.first+1;
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
    class range_finder
    {
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

    class min_finder
    {
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

    class max_finder
    {
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

    class time_sorter
    {
    public:
        /// constructor
        explicit time_sorter() { }

        int operator()(const ValuedTimeRange &p1, const ValuedTimeRange &p2) {
            return p1.first.first<p2.first.first;
        }
    };

    /// Predicates for (binary) ordering and searching in a
    /// TimedValueVector. The first value of TimeRange is used for
    /// ordering and searching. If necessary, check for the second
    /// one yourself.
    struct TimeRangeLess :
                std::binary_function< unsigned, ValuedTimeRange, bool > {
        /// predicate to use with upper_bound.
        bool operator()(const unsigned searchTime,
                        const ValuedTimeRange& valuedRange) const {
            return searchTime < valuedRange.first.first;
        }
        /// predicate to use with lower_bound
        bool operator()(const ValuedTimeRange& valuedRange,
                        const unsigned searchTime) const {
            return searchTime > valuedRange.first.first;
        }
    };


private:
    /// The list of time periods (with values)
    TimedValueVector myValues;

};


#endif

/****************************************************************************/

