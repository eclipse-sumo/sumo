/****************************************************************************/
/// @file    AbstractMutex.h
/// @author  Daniel Krajzewicz
/// @author  Michael Behrisch
/// @date    2005-07-12
/// @version $Id$
///
// An abstract class for encapsulating mutex implementations
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef AbstractMutex_h
#define AbstractMutex_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class AbstractMutex
 * @brief An abstract class for encapsulating mutex implementations
 *
 * This class defines access to a mutex. The implementation may differ.
 *
 * Within gui-applications, FXMutexes may be used while this is improper
 *  for command-line applications. Normally, they do not need mutexes unless
 *  a synchronized communication with an external application is established.
 *  In these cases, a further class should be implemented.
 */
class AbstractMutex {
public:
    /// @brief Constructor
    AbstractMutex() { }


    /// @brief Destructor
    virtual ~AbstractMutex() { }


    /// @brief Locks the mutex
    virtual void lock() = 0;


    /// @brief Unlocks the mutex
    virtual void unlock() = 0;



    /** @class ScopedLocker
     * @brief A mutex encapsulator which locks/unlocks the given mutex on construction/destruction, respectively
     */
    class ScopedLocker {
    public:
        /** @brief Constructor
         * @param[in] lock The mutex to lock
         *
         * Locks the mutex.
         */
        ScopedLocker(AbstractMutex& lock): myLock(lock) {
            myLock.lock();
        }


        /** @brief Destructor
         * Unlocks the mutex.
         */
        ~ScopedLocker() {
            myLock.unlock();
        }

    private:
        /// @brief The mutex to lock
        AbstractMutex& myLock;

    private:
        /// @brief Invalidated copy constructor.
        ScopedLocker(const ScopedLocker&);

        /// Invalidated assignment operator.
        ScopedLocker& operator=(const ScopedLocker&);


    };



};


#endif

/****************************************************************************/

