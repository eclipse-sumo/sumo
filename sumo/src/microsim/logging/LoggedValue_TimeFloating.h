/****************************************************************************/
/// @file    LoggedValue_TimeFloating.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 20 Feb 2003
/// @version $Id$
///
// This class logs values over a previously defined time with a moving window
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
#ifndef LoggedValue_TimeFloating_h
#define LoggedValue_TimeFloating_h


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
 * @class LoggedValue_TimeFloating
 * This class allows a view at a SUMORealing window of a fixed size.
 * The values are stored in an array to allow faster adaption of their sum.
 */
template<typename _T>
class LoggedValue_TimeFloating
            : public LoggedValue<_T>
{
public:
    /// Constructor
    LoggedValue_TimeFloating(size_t sampleInterval)
            : LoggedValue<_T>(sampleInterval),
            myFloatingArray(new _T[sampleInterval]), mySampleInterval(sampleInterval),
            mySampledUnits(0), myBufferWasFull(false)
    {
        for (size_t i=0; i<sampleInterval; i++) {
            myFloatingArray[i] = 0;
        }
    }


    /// Destructor
    ~LoggedValue_TimeFloating()
    {
        delete[] myFloatingArray;
    }


    /** @brief Adds a new value
        See source code */
    void add(_T value)
    {
        // remove the value lying some steps ahead
        this->myCurrentValue -= myFloatingArray[mySampledUnits];
        // add the current value
        this->myCurrentValue += value;
        // store the current value
        myFloatingArray[mySampledUnits] = value;
        // check whether the number of sampled units exceeds the array
        mySampledUnits++;
        if (mySampledUnits>=mySampleInterval) {
            mySampledUnits = 0;
            // set the information that the buffer was full
            myBufferWasFull = true;
        }
    }


    /** returns the average of previously set values
        (for and over the given sample interval) */
    _T getAvg() const
    {
        // the list is complete
        if (myBufferWasFull) {
            return this->myCurrentValue / (SUMOReal) mySampleInterval;
        }
        // return only the acquired values
        if (mySampledUnits!=0) {
            return this->myCurrentValue / (SUMOReal) mySampledUnits;
        }
        // return 0 (or throw an exception when not initialised
        return 0;
    }


    /** returns the sum of previously set values
        (for the given sample interval) */
    _T getAbs() const
    {
        return this->myCurrentValue;
    }


private:
    /// The array of number within the moving window
    _T      *myFloatingArray;

    /// The window (sample interval) size
    size_t  mySampleInterval;

    /** The number of seen values
        Resetted to zero if larger or equal to "mySampleInterval" */
    size_t  mySampledUnits;

    /** @brief Information whether the buffer was already filled
        (whether there are already "mySampleInterval" values in the array) */
    bool    myBufferWasFull;

};


#endif

/****************************************************************************/

