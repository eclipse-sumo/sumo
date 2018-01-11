/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
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
typedef void* FXThreadMutex;        // handle to a mutex
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
typedef HANDLE FXThreadMutex;        // handle to a mutex
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
