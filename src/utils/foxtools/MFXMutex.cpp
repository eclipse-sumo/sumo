/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2007-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXMutex.cpp
/// @author  Mathew Robertson
/// @author  Michael Behrisch
/// @author  Jakob Erdmann
/// @date    2007-02-19
/// @version $Id$
///
//
/****************************************************************************/


/* =========================================================================
 * included modules
 * ======================================================================= */
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fxver.h>
#define NOMINMAX
#include <xincs.h>
#undef NOMINMAX
#include <fxdefs.h>

using namespace FX;

#include "MFXMutex.h"

#ifndef WIN32
#include <pthread.h>
#endif

// MFXMutex constructor
MFXMutex::MFXMutex() : lock_(0) {
#ifndef WIN32
    FXint status = 0;
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    status = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    FXASSERT(status == 0);
    FXMALLOC(&mutexHandle, pthread_mutex_t, 1);
    status = pthread_mutex_init((pthread_mutex_t*)mutexHandle, &attr);
    FXASSERT(status == 0);
    (void)status; // only used for assertion
    pthread_mutexattr_destroy(&attr);
#else
    mutexHandle = CreateMutex(NULL, FALSE, NULL);
    FXASSERT(mutexHandle != NULL);
#endif
}

// Note: lock_ is not safe here because it is not protected, but
//       if you are causing the destructor to be executed while
//       some other thread is accessing the mutexHandle, then you have
//       a design flaw in your program, and so it should crash!
MFXMutex::~MFXMutex() {
    if (lock_) {
        fxerror("MFXMutex: mutex still locked\n");
    }
#if !defined(WIN32)
    pthread_mutex_destroy((pthread_mutex_t*)mutexHandle);
    FXFREE(&mutexHandle);
#else
    CloseHandle(mutexHandle);
#endif
}

// lock_ is safe because we dont increment it until we
// have entered the locked state - cha-ching, correct
void MFXMutex::lock() {
#if !defined(WIN32)
    pthread_mutex_lock((pthread_mutex_t*)mutexHandle);
#else
    WaitForSingleObject(mutexHandle, INFINITE);
#endif
    lock_++;
}

// lock_ is safe because we decrement it, before leaving the locked state
void MFXMutex::unlock() {
    lock_--;
#if !defined(WIN32)
    pthread_mutex_unlock((pthread_mutex_t*)mutexHandle);
#else
    ReleaseMutex(mutexHandle);
#endif
}

