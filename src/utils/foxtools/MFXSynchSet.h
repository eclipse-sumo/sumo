/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2004-2024 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    MFXSynchSet.h
/// @author  Jakob Erdmann
/// @date    2020-03-29
///
// missing_desc
/****************************************************************************/
#pragma once
#include <config.h>

#ifdef HAVE_FOX
#include "fxheader.h"
#endif
#include <list>
#include <cassert>
#include <algorithm>

//#define DEBUG_LOCKING

#ifdef DEBUG_LOCKING
#include <iostream>
#include "MFXWorkerThread.h"
#endif

template<class T, class Container = std::set<T> >
class MFXSynchSet {
public:
    MFXSynchSet(const bool condition = true):
#ifdef HAVE_FOX
        myMutex(true),
#endif
        myCondition(condition)
    {}

    // Attention! Removes locking behavior
    void unsetCondition() {
        myCondition = false;
    }

    // Attention! Retains the lock
    Container& getContainer() {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
#ifdef DEBUG_LOCKING
        if (debugflag) {
            std::cout << " MFXSynchSet::getContainer thread=" << MFXWorkerThread::current() << "\n";
        }
        myOwningThread = MFXWorkerThread::current();
#endif
        return myItems;
    }

    void unlock() {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
#ifdef DEBUG_LOCKING
        if (debugflag) {
            std::cout << " MFXSynchSet::unlock       thread=" << MFXWorkerThread::current() << "\n";
        }
        myOwningThread = 0;
#endif
    }

    void insert(T what) {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        myItems.insert(what);
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
    }

    void erase(T what) {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        myItems.erase(what);
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
    }

    size_t count(T what) {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        size_t result = myItems.count(what);
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
        return result;
    }

    bool empty() {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        const bool ret = myItems.size() == 0;
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
        return ret;
    }

    void clear() {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        myItems.clear();
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
    }

    size_t size() const {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        size_t res = myItems.size();
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
        return res;
    }

    bool contains(const T& item) const {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        bool res = std::find(myItems.begin(), myItems.end(), item) != myItems.end();
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
        return res;
    }

    bool isLocked() const {
#ifdef HAVE_FOX
        return myMutex.locked();
#else
        return false;
#endif
    }

private:
#ifdef HAVE_FOX
    mutable FXMutex myMutex;
#endif
    Container myItems;
    bool myCondition;

#ifdef DEBUG_LOCKING
    mutable long long int myOwningThread = 0;
public:
    mutable bool debugflag = false;
#endif

};
