//---------------------------------------------------------------------------//
//                        LoggedValue_Single.cpp -
//  Logger over a single time step
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
// Revision 1.3  2004/03/01 11:15:12  roessel
// Reintroduced formerly removed files.
//
// Revision 1.1  2003/11/11 08:07:37  dkrajzew
// logging (value passing) moved from utils to microsim
//
// Revision 1.3  2003/06/05 14:29:39  dkrajzew
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
#include "LoggedValue_Single.h"

template<typename _T>
LoggedValue_Single<_T>::LoggedValue_Single<_T>(size_t dummy)
    : LoggedValue<_T>(dummy)
{
}


template<typename _T>
LoggedValue_Single<_T>::~LoggedValue_Single<_T>()
{
}


template<typename _T>
void
LoggedValue_Single<_T>::add(_T value)
{
    myCurrentValue = value;
}


template<typename _T>
_T
LoggedValue_Single<_T>::getAvg() const
{
    return myCurrentValue;
}


template<typename _T>
_T
LoggedValue_Single<_T>::getAbs() const
{
    return myCurrentValue;
}




/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifdef DISABLE_INLINE
//#include "LoggedValue_Single.icc"
//#endif

// Local Variables:
// mode:C++
// End:


