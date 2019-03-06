/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SysUtils.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// A few system-specific functions
/****************************************************************************/
#ifndef SysUtils_h
#define SysUtils_h


// ===========================================================================
// included modules
// ===========================================================================

#include <string>

// ===========================================================================
// class definitions
// ===========================================================================
/** @class SysUtils
 * @brief A few system-specific functions
 */
class SysUtils {
public:
    /** @brief Returns the current time in milliseconds
     * @return Current time
     */
    static long getCurrentMillis();


#ifdef _MSC_VER
    /** @brief Returns the CPU ticks (windows only)
     *
     * Used for random number initialisation, linux version
     *  uses a different method
     */
    static long getWindowsTicks();
#endif


    /// @brief run a shell command without popping up any windows (particuarly on win32)
    static unsigned long runHiddenCommand(const std::string& cmd);
};

#endif

/****************************************************************************/

