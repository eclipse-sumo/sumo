/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2018-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    FXConditionalLock.h
/// @author  Michael Behrisch
/// @date    2018-11-14
/// @version $Id$
///
// A scoped lock which only triggers on condition
/****************************************************************************/

#ifndef FXConditionalLock_h
#define FXConditionalLock_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <fx.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class FXConditionalLock
 * @brief A scoped lock which only triggers on condition
 */
class FXConditionalLock {
public:
    /// Construct & lock associated mutex if the condition is true
    FXConditionalLock(FXMutex& m, const bool condition)
        : myMutex(m), myCondition(condition) {
        if (condition) {
            m.lock();
        }
    }

    /// Destroy and unlock associated mutex
    ~FXConditionalLock() {
        if (myCondition) {
            myMutex.unlock();
        }
    }

private:
    FXMutex& myMutex;
    const bool myCondition;

private:
    FXConditionalLock& operator=(const FXConditionalLock&) = delete;
};


#endif
