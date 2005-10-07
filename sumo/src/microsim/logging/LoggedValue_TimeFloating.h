#ifndef LoggedValue_TimeFloating_h
#define LoggedValue_TimeFloating_h
//---------------------------------------------------------------------------//
//                        LoggedValue_TimeFloating.h -
//  This class logs values over a previously defined time with a moving window
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
// Revision 1.7  2005/10/07 11:37:45  dkrajzew
// THIRD LARGE CODE RECHECK: patched problems on Linux/Windows configs
//
// Revision 1.6  2005/09/22 13:45:51  dkrajzew
// SECOND LARGE CODE RECHECK: converted doubles and floats to SUMOReal
//
// Revision 1.5  2005/09/15 11:07:54  dkrajzew
// LARGE CODE RECHECK
//
// Revision 1.4  2004/03/19 13:06:44  dkrajzew
// some work on the style
//
// Revision 1.3  2004/03/01 10:49:51  roessel
// Reintroduced formerly removed files.
//
// Revision 1.1  2003/11/11 08:07:37  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.3  2003/04/09 15:36:19  dkrajzew
// debugging of emitters: forgotten release of vehicles (gui) debugged; forgotten initialisation of logger-members debuggt; error managament corrected
//
// Revision 1.2  2003/03/18 13:16:57  dkrajzew
// windows eol removed
//
// Revision 1.1  2003/03/06 17:22:16  dkrajzew
// initial commit for value logging cleasses
//
/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif
#endif // HAVE_CONFIG_H

#include "LoggedValue.h"


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class LoggedValue_TimeFloating
 * This class allows a view at a SUMORealing window of a fixed size.
 * The values are stored in an array to allow faster adaption of their sum.
 */
template<typename _T>
class LoggedValue_TimeFloating
    : public LoggedValue<_T> {
public:
    /// Constructor
    LoggedValue_TimeFloating(size_t sampleInterval)
    : LoggedValue<_T>(sampleInterval),
        myFloatingArray(new _T[sampleInterval]), mySampleInterval(sampleInterval),
        mySampledUnits(0), myBufferWasFull(false)
    {
        for(size_t i=0; i<sampleInterval; i++) {
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
        myCurrentValue -= myFloatingArray[mySampledUnits];
        // add the current value
        myCurrentValue += value;
        // store the current value
        myFloatingArray[mySampledUnits] = value;
        // check whether the number of sampled units exceeds the array
        mySampledUnits++;
        if(mySampledUnits>=mySampleInterval) {
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
        if(myBufferWasFull) {
            return myCurrentValue / (SUMOReal) mySampleInterval;
        }
        // return only the acquired values
        if(mySampledUnits!=0) {
            return myCurrentValue / (SUMOReal) mySampledUnits;
        }
        // return 0 (or throw an exception when not initialised
        return 0;
    }


    /** returns the sum of previously set values
        (for the given sample interval) */
    _T getAbs() const
    {
        return myCurrentValue;
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


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/

#endif

// Local Variables:
// mode:C++
// End:

