/****************************************************************************/
/// @file    SysUtils.h
/// @author  Daniel Krajzewicz
/// @date    Fri, 29.04.2005
/// @version $Id$
///
// A few system-specific functions
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef SysUtils_h
#define SysUtils_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

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

