#ifndef NewQMutex_h
#define NewQMutex_h
//---------------------------------------------------------------------------//
//                        NewQMutex.h -
//  A QMutex-encapsulation needed due to some QMutex bugs
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
// Revision 1.3  2004/01/12 14:53:18  dkrajzew
// documentation added; patched some compilation problems
//
// Revision 1.2  2003/02/07 10:52:29  dkrajzew
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
 * class declarations
 * ======================================================================= */
class QMutex;


/* =========================================================================
 * class definitions
 * ======================================================================= */
/**
 * @class NewQMutex
 * A wrapper around a simple qt-mutex. We can not use it directly as there
 *  are some bugs within the definition that make it imposible to use with
 *  other files which use "bool"(!)
 */
class NewQMutex {
public:
    /// Constructor
    NewQMutex();

    /// Destructor
    ~NewQMutex();

    /// Locks the mutex
    void lock();

    /// Unlocks the mutex
    void unlock();

    /// Returns the information whether the mutex is locked
    bool locked ();

private:
    /// The wrapped mutex
    QMutex *_mutex;

};


/**************** DO NOT DEFINE ANYTHING AFTER THE INCLUDE *****************/
//#ifndef DISABLE_INLINE
//#include "NewQMutex.icc"
//#endif

#endif

// Local Variables:
// mode:C++
// End:

