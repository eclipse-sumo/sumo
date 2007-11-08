/****************************************************************************/
/// @file    InstancePool.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
//
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef InstancePool_h
#define InstancePool_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <vector>
#include <algorithm>
#include <cassert>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 *
 */
template<typename T>
class InstancePool
{
public:
    InstancePool(bool deleteOnQuit) : myDeleteOnQuit(deleteOnQuit) { }
    ~InstancePool() {
        typedef typename std::vector<T*>::iterator It;
        if (myDeleteOnQuit) {
            for (It i=myFreeInstances.begin(); i!=myFreeInstances.end(); i++) {
                delete *i;
            }
        }
    }

    T* getFreeInstance() {
        if (myFreeInstances.size()==0) {
            return 0;
        } else {
            T *instance = myFreeInstances.back();
            myFreeInstances.pop_back();
            return instance;
        }
    }

    void addFreeInstance(T *instance) {
        myFreeInstances.push_back(instance);
    }

    void addFreeInstances(const std::vector<T*> instances) {
        std::copy(instances.begin(), instances.end(),
                  std::back_inserter(myFreeInstances));
    }


private:
    std::vector<T*> myFreeInstances;
    bool myDeleteOnQuit;
};


#endif

/****************************************************************************/

