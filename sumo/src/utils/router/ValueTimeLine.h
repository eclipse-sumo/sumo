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

/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class ValueTimeLine
 * A time line being a list of time period with assigned values.
 */
class ValueTimeLine {
public:
    /// Definition of a time period
    typedef std::pair<long, long> TimeRange;

    /// Definition of a time range with an assigned value
    typedef std::pair<TimeRange, double> ValuedTimeRange;

    /// A list of time periods having which have values assigned
    typedef std::vector<ValuedTimeRange> TimedValueVector;

public:
    /// Constructor
    ValueTimeLine();

    /// Destructor
    ~ValueTimeLine();

    /// Adds a time period together with a value
    void addValue(long begin, long end, float value);

    /// Adds a time period together with a value
    void addValue(TimeRange range, float value);

    /// Returns the value for the given time
    float getValue(long time) const;

    /// Returns the number of known periods
    size_t noDefinitions() const;

    /// Returns the time period description at the given position // !!! should not be public
    const TimeRange &getRangeAtPosition(size_t pos) const;

private:
    /// Searches for the range when a time point is given
    class range_finder {
    public:
        /** constructor */
        explicit range_finder(long time) : _time(time) { }

        /** the comparing function */
        bool operator() (ValuedTimeRange vrange) {
            TimeRange range = vrange.first;
            return range.first<=_time && range.second>=_time;
        }

    private:
        /// The time to search for
        long _time;

    };

private:
    /// The list of time periods (with values)
    TimedValueVector _values;
};



/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "ValueTimeLine.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

