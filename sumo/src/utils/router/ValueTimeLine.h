#ifndef ValueTimeLine_h
#define ValueTimeLine_h
//---------------------------------------------------------------------------//
//                        ValueTimeLine.h -
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
#include <vector>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ValueTimeLine
 * A time line being a list of time period with assigned values.
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

public:
    /// Constructor
    ValueTimeLine();

    /// Destructor
    ~ValueTimeLine();

    /// Adds a time period together with a value
    void addValue(unsigned int begin, unsigned int end, T value);

    /// Adds a time period together with a value
    void addValue(TimeRange range, T value);

    /// Returns the value for the given time
    T getValue(unsigned int time) const;

    /// Returns the number of known periods
    size_t noDefinitions() const;

    /// Returns the time period description at the given position // !!! should not be public
    const TimeRange &getRangeAtPosition(size_t pos) const;

    /// returns the information wehther the values for the given time are known
    bool describesTime(unsigned int time) const;

private:
    /// Searches for the range when a time point is given
    class range_finder {
    public:
        /** constructor */
        explicit range_finder(unsigned int time)
            : myTime(time) { }

        /** the comparing function */
        bool operator() (ValuedTimeRange vrange) {
            TimeRange range = vrange.first;
            return range.first<=myTime && range.second>=myTime;
        }

    private:
        /// The time to search for
        unsigned int myTime;

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

