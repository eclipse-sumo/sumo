/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    FXSynchQue.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
#ifndef FXSynchQue_h
#define FXSynchQue_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#ifdef HAVE_FOX
#include <fx.h>
#endif
#include <list>
#include <cassert>

template<class T, class Container = std::list<T> >
class FXSynchQue {
public:
    FXSynchQue(const bool condition = true): myCondition(condition) {}

    T top() {
        assert(myItems.size() != 0);
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        T ret = myItems.front();
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
        return ret;
    }

    void pop() {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        myItems.erase(myItems.begin());
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
    }

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
        return myItems;
    }

    void unlock() {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
    }

    void push_back(T what) {
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.lock();
        }
#endif
        myItems.push_back(what);
#ifdef HAVE_FOX
        if (myCondition) {
            myMutex.unlock();
        }
#endif
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

private:
#ifdef HAVE_FOX
    mutable FXMutex myMutex;
#endif
    Container myItems;
    bool myCondition;
};


#endif

/****************************************************************************/
