#ifndef LoggedValue_TimeFixed_h
#define LoggedValue_TimeFixed_h
//---------------------------------------------------------------------------//
//                        LoggedValue_TimeFixed.h -
//  This class logs values over a previously defined, fixed time
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
// Revision 1.3  2003/04/09 15:36:18  dkrajzew
// debugging of emitters: forgotten release of vehicles (gui) debugged; forgotten initialisation of logger-members debuggt; error managament corrected
//
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
 * @class LoggedValue_TimeFixed
 * To sum values (of the same semantic type) over a fixed time interval,
 * They are simply summed in the member variable myCurrentValue.
 * As possibly not all values have been stored on reading, "mySampledUnits"
 * has been added to allow a normalised reading in respect to the number
 * of values stored (or the time interval if smaller).
 */
template<typename _T>
class LoggedValue_TimeFixed
    : public LoggedValue<_T> {
public:
    /// Constructor
    LoggedValue_TimeFixed(size_t sampleInterval);

    /// Destructor
    ~LoggedValue_TimeFixed();

    /** @brief Adds a new value
        Adds the value to the sum; increases the number of read units.
        Possibly resets the buffer to zero if the fixed length has been exceeded */
    void add(_T value);

    /** returns the average of previously set values
        (for and over the given sample interval or the number of sampled units, if smaller) */
    _T getAvg() const;

    /** returns the sum of previously set values
        (for the given sample interval) */
    _T getAbs() const;

private:
    /// The sample interval to sum the values over
    size_t  mySampleInterval;

    /** @brief the number of values sampled
        (set together with myCurrentValue to  zero if larger than mySampleInterval) */
    size_t  mySampledUnits;
};

#ifndef EXTERNAL_TEMPLATE_DEFINITION
#ifndef MSVC
#include "LoggedValue_TimeFixed.cpp"
#endif
#endif // EXTERNAL_TEMPLATE_DEFINITION


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "LoggedValue_TimeFixed.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

