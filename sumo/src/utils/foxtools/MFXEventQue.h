/****************************************************************************/
/// @file    MFXEventQue.h
/// @author  Daniel Krajzewicz
/// @date    2004-03-19
/// @version $Id$
///
// missing_desc
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2004-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
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

    size_t size() {
        myMutex.lock();
        const size_t ret = myItems.size();
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

