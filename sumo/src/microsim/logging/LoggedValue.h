/****************************************************************************/
/// @file    LoggedValue.h
/// @author  Daniel Krajzewicz
/// @date    Thu, 20 Feb 2003
/// @version $Id$
///
// An abstract (virtual) class for value logging
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
#ifndef LoggedValue_h
#define LoggedValue_h
// ===========================================================================
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class LoggedValue
 * As detectors may store values over a SUMORealing or a fixed interval and
 * a sample interval of one does not require any kind of remembering
 * previous values, we implemented this class to make a common access to
 * all those mentioned cases.
 */
template<typename _T>
class LoggedValue
{
public:
    /** @brief Constructor
        The dummy value is used to ensure, the derived class must
        remind the sample interval */
    LoggedValue(size_t dummy)
            : myCurrentValue(0)
    { }

    /// Destructor
    virtual ~LoggedValue()
    { }

    /// Adds a new value
    virtual void add(_T value) = 0;

    /** returns the average of previously set values
        (for and over the given sample interval) */
    virtual _T getAvg() const = 0;

    /** returns the sum of previously set values
        (for the given sample interval) */
    virtual _T getAbs() const = 0;

protected:
    /// The current value
    _T      myCurrentValue;

};


#endif

/****************************************************************************/

