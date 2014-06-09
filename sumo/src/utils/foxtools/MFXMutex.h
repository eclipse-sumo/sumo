/****************************************************************************/
/// @file    MFXMutex.h
/// @author  Mathew Robertson
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2007-02-19
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2007-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


#ifndef MFXMUTEX_H
#define MFXMUTEX_H


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <utils/common/AbstractMutex.h>

#ifndef WIN32
typedef void*		FXThreadMutex;        // handle to a mutex
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
typedef HANDLE	 	FXThreadMutex;        // handle to a mutex
#endif




/**
 * Recursive mutual exclusion object.
 * Unlocks on destruction
 */
class MFXMutex : public AbstractMutex {
public:
    /// create me a mutex :-)
    MFXMutex();

    /// lock mutex
    void lock();

    /// release mutex lock
    void unlock();

    /// dtor
    virtual ~MFXMutex();

    inline FXbool locked() {
        return lock_ ? TRUE : FALSE;
    }

protected:
    FXuint lock_;          // lock count

private:
    FXThreadMutex mutexHandle;

private:
    // dummy copy constructor and operator= to prevent copying
    MFXMutex(const MFXMutex&);
    MFXMutex& operator=(const MFXMutex&);

};

#endif // FXMUTEX_H
