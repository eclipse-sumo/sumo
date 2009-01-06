/****************************************************************************/
/// @file    InstancePool.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// A pool of resuable instances
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
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
 * @class InstancePool
 * @brief A pool of resuable instances
 */
template<typename T>
class InstancePool
{
public:
    /** @brief Constructor
     *
     * @param[in] deleteOnQuit Information whether stored instances shall be deleted when this container is deleted
     */
    InstancePool(bool deleteOnQuit) : myDeleteOnQuit(deleteOnQuit) { }


    /// @brief Destructor
    ~InstancePool() {
        typedef typename std::vector<T*>::iterator It;
        if (myDeleteOnQuit) {
            for (It i=myFreeInstances.begin(); i!=myFreeInstances.end(); i++) {
                delete *i;
            }
        }
    }


    /** @brief Returns a free instance or 0 if no such exists
     *
     * If any free instance is stored, it is returned and removed from
     *  the storage. If no one is stored, 0 is returned.
     *
     * @return A free instance or 0 if no such exists
     */
    T* getFreeInstance() {
        if (myFreeInstances.size()==0) {
            return 0;
        } else {
            T *instance = myFreeInstances.back();
            myFreeInstances.pop_back();
            return instance;
        }
    }


    /** @brief Adds a free, reusable instance
     *
     * @param[in] instance An instance to add
     */
    void addFreeInstance(T *instance) {
        myFreeInstances.push_back(instance);
    }


    /** @brief Adds some free, reusable instances
     *
     * @param[in] instances A vector of instances to add
     */
    void addFreeInstances(const std::vector<T*> instances) {
        std::copy(instances.begin(), instances.end(),
                  std::back_inserter(myFreeInstances));
    }


private:
    /// @brief List of reusable instances
    std::vector<T*> myFreeInstances;

    /// @brief Information whether the stored instances shall be deleted
    bool myDeleteOnQuit;


};


#endif

/****************************************************************************/

