//---------------------------------------------------------------------------//
//                        LoggedValue_TimeFloating.cpp -
//  This class logs values over a previously defined time with a moving window
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
/*
namespace
{
    const char rcsid[] =
    "$Id$";
}
*/
// $Log$
// Revision 1.1  2003/11/11 08:07:37  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.4  2003/06/05 14:29:39  dkrajzew
// class templates applied; documentation added
//
//


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef HAVE_CONFIG_H
#include "config.h"
#endif // HAVE_CONFIG_H


#include "LoggedValue.h"
#include "LoggedValue_TimeFloating.h"

template<typename _T>
LoggedValue_TimeFloating<_T>::LoggedValue_TimeFloating(size_t sampleInterval)
    : LoggedValue<_T>(sampleInterval),
    myFloatingArray(new _T[sampleInterval]),
    mySampleInterval(sampleInterval),
    mySampledUnits(0),
    myBufferWasFull(false)
{
    for(size_t i=0; i<sampleInterval; i++) {
        myFloatingArray[i] = 0;
    }
}


template<typename _T>
LoggedValue_TimeFloating<_T>::~LoggedValue_TimeFloating()
{
    delete myFloatingArray;
}


template<typename _T>
void
LoggedValue_TimeFloating<_T>::add(_T value)
{
    // remove the value lying some steps ahead
    myCurrentValue -= myFloatingArray[mySampledUnits];
    // add the current value
    myCurrentValue += value;
    // store the current value
    myFloatingArray[mySampledUnits++] = value;
    // check whether the number of sampled units exceeds the array
    if(mySampledUnits>mySampleInterval) {
        mySampledUnits = 0;
        // set the information that the buffer was full
        bool myBufferWasFull = true;
    }
}


template<typename _T>
_T
LoggedValue_TimeFloating<_T>::getAvg() const
{
    // the list is complete
    if(myBufferWasFull) {
        return myCurrentValue / (double) mySampleInterval;
    }
    // return only the acquired values
    if(mySampledUnits!=0) {
        return myCurrentValue / (double) mySampledUnits;
    }
    // return 0 (or throw an exception when not initialised
    return 0;
}


template<typename _T>
_T
LoggedValue_TimeFloating<_T>::getAbs() const
{
    return myCurrentValue;
}


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "LoggedValue_TimeFloating.icc"
//#endif

// Local Variables:
// mode:C++
// End:


