#ifndef LoggedValue_Single_h
#define LoggedValue_Single_h
//---------------------------------------------------------------------------//
//                        LoggedValue_Single.h -
//  Logger over a single time step
//                           -------------------
//  project              : SUMO - Simulation of Urban MObility
//  begin                : Thu, 20 Feb 2003
//  copyright            : (C) 2003 by Daniel Krajzewicz
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
// Revision 1.2  2003/03/18 13:16:57  dkrajzew
// windows eol removed
//
// Revision 1.1  2003/03/06 17:22:15  dkrajzew
// initial commit for value logging cleasses
//
//

/* =========================================================================
 * included modules
 * ======================================================================= */
#include "LoggedValue.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class LoggedValue_Single
 * This class simply stores a value and allows its retrieval
 */
template<typename _T>
class LoggedValue_Single
    : public LoggedValue<_T> {
public:
    /// Constructor
    LoggedValue_Single(size_t dummy);

    /// Destructor
    ~LoggedValue_Single();

    /** @brief Adds a new value
        Simply sets the current value */
    void add(_T value);

    /** returns the average of previously set values
        (for and over the given sample interval) */
    _T getAvg() const;

    /** returns the sum of previously set values
        (for the given sample interval) */
    _T getAbs() const;

private:
    /// The current value
    _T      myCurrentValue;

};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "LoggedValue_Single.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "LoggedValue_Single.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

