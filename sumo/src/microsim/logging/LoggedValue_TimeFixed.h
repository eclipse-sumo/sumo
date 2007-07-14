/****************************************************************************/
/// @file    LoggedValue_TimeFixed.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 20 Feb 2003
/// @version $Id$
///
// This class logs values over a previously defined, fixed time
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef LoggedValue_TimeFixed_h
#define LoggedValue_TimeFixed_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "LoggedValue.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LoggedValue_TimeFixed
 * To sum values (of the same semantic type) over a fixed time interval,
 * They are simply summed in the member variable myCurrentValue.
 * As possibly not all values have been stored on reading, "mySampledUnits"
 * has been added to allow a normalised reading in respect to the number
 * of values stored (or the time interval if smaller).
 */
template<typename T>
class LoggedValue_TimeFixed
            : public LoggedValue<T>
{
public:
    /// Constructor
    LoggedValue_TimeFixed(size_t sampleInterval)
            : LoggedValue<T>(sampleInterval), mySampleInterval(sampleInterval),
            mySampledUnits(0)
    {}

    /// Destructor
    ~LoggedValue_TimeFixed()
    { }


    /** @brief Adds a new value
        Adds the value to the sum; increases the number of read units.
        Possibly resets the buffer to zero if the fixed length has been exceeded */
    void add(T value)
    {
        mySampledUnits++;
        // clear on a new interval
        if (mySampledUnits>mySampleInterval) {
            mySampledUnits = 1;
            this->myCurrentValue = 0;
        }
        // add new value
        this->myCurrentValue += value;
    }


    /** returns the average of previously set values
        (for and over the given sample interval or the number of sampled units, if smaller) */
    T getAvg() const
    {
        // Security check for false interval usage
        if (mySampledUnits==0) {
            return 0; // !!! You could also throw an exception here;
            // This is not meant to be done
        }
        return this->myCurrentValue / (SUMOReal) mySampledUnits;
    }


    /** returns the sum of previously set values
        (for the given sample interval) */
    T getAbs() const
    {
        return this->myCurrentValue;
    }


private:
    /// The sample interval to sum the values over
    size_t  mySampleInterval;

    /** @brief the number of values sampled
        (set together with myCurrentValue to  zero if larger than mySampleInterval) */
    size_t  mySampledUnits;
};


#endif

/****************************************************************************/

