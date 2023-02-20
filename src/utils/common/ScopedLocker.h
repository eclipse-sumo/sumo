/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2018-2023 German Aerospace Center (DLR) and others.
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
/// @file    ScopedLocker.h
/// @author  Michael Behrisch
/// @date    2018-11-14
///
// A scoped lock which only triggers on condition
/****************************************************************************/
#pragma once
#include <config.h>


// ===========================================================================
// class declarations
// ===========================================================================
namespace FX {
class FXMutex;
}


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class ScopedLocker
 * @brief A scoped lock which only triggers on condition
 */
template<typename T = FX::FXMutex, bool IGNORE_COND = false>
class ScopedLocker {

public:
    /// Construct & lock associated mutex if the condition is true
    ScopedLocker(T& m, const bool condition = true)
        : myMutex(m), myCondition(condition) {
        if (IGNORE_COND || condition) {
            m.lock();
        }
    }

    /// Destroy and unlock associated mutex
    ~ScopedLocker() {
        if (IGNORE_COND || myCondition) {
            myMutex.unlock();
        }
    }

private:
    T& myMutex;
    const bool myCondition;

private:
    ScopedLocker& operator=(const ScopedLocker&) = delete;
};
