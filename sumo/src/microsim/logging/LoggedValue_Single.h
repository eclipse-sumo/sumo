/****************************************************************************/
/// @file    LoggedValue_Single.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 20 Feb 2003
/// @version $Id: $
///
// Logger over a single time step
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
#ifndef LoggedValue_Single_h
#define LoggedValue_Single_h
// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "LoggedValue.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LoggedValue_Single
 * This class simply stores a value and allows its retrieval
 */
template<typename _T>
class LoggedValue_Single
            : public LoggedValue<_T>
{
public:
    /// Constructor
    LoggedValue_Single(size_t dummy)
            : LoggedValue<_T>(dummy)
    {}

    /// Destructor
    ~LoggedValue_Single()
    { }


    /** @brief Adds a new value
        Simply sets the current value */
    void add(_T value)
    {
        this->myCurrentValue = value;
    }


    /** returns the average of previously set values
        (for and over the given sample interval) */
    _T getAvg() const
    {
        return this->myCurrentValue;
    }


    /** returns the sum of previously set values
        (for the given sample interval) */
    _T getAbs() const
    {
        return this->myCurrentValue;
    }


};


#endif

/****************************************************************************/

