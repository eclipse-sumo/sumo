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
#include <config.h>

#include <fx.h>
#include <utils/common/AbstractMutex.h>

#ifndef WIN32
// handle to a mutex
typedef void* FXThreadMutex;
#else
#define NOMINMAX
#include <windows.h>
#undef NOMINMAX
// handle to a mutex
typedef HANDLE FXThreadMutex;
#endif

/**
 * Recursive mutual exclusion object.
 * Unlocks on destruction
 */
class MFXMutex : public AbstractMutex {
public:
    /// @brief constructor
    MFXMutex();

    /// @brief destructor
    virtual ~MFXMutex();

    /// @brief lock mutex
    void lock();

    /// @brief release mutex lock
    void unlock();

    /// @brief check if mutex is locked
    inline FXbool locked() {
        return lock_ ? TRUE : FALSE;
    }

    /// @brief return current lock value
    inline FXuint lockCount() {
        return lock_;
    }

protected:
    /// @brief lock count
    FXuint lock_;

private:
    /// @brief mutex handler
    FXThreadMutex mutexHandle;

    // @brief invalidate copy constructor
    MFXMutex(const MFXMutex&) = delete ;

    // @brief invalidate asignment operators
    MFXMutex& operator=(const MFXMutex&) = delete ;
};

#endif // FXMUTEX_H
