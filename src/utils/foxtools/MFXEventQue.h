/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2004-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MFXEventQue.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
#ifndef MFXEventQue_h
#define MFXEventQue_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <fx.h>
#include <list>
#include <cassert>
#include <utils/foxtools/MFXMutex.h>

template<class T>
class MFXEventQue {
public:
    MFXEventQue() { }
    ~MFXEventQue() { }

    T top() {
        assert(size() != 0);
        myMutex.lock();
        T ret = myItems.front();
        myMutex.unlock();
        return ret;
    }


    void pop() {
        myMutex.lock();
        myItems.erase(myItems.begin());
        myMutex.unlock();
    }

    void add(T what) {
        myMutex.lock();
        myItems.push_back(what);
        myMutex.unlock();
    }

    int size() {
        myMutex.lock();
        const int ret = (int)myItems.size();
        myMutex.unlock();
        return ret;
    }

    bool empty() {
        myMutex.lock();
        const bool ret = myItems.size() == 0;
        myMutex.unlock();
        return ret;
    }

private:
    MFXMutex myMutex;
    std::list<T> myItems;
};


#endif

/****************************************************************************/

