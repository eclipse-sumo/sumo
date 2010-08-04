/****************************************************************************/
/// @file    TraCIServerAPI_MeMeDetector.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting multi-entry/multi-exit detector values via TraCI
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/
#ifndef TraCIServerAPI_MeMeDetector_h
#define TraCIServerAPI_MeMeDetector_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include "TraCIException.h"
#include <foreign/tcpip/storage.h>


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class TraCIServerAPI_MeMeDetector
 * @brief APIs for getting/setting multi-entry/multi-exit detector values via TraCI
 */
class TraCIServerAPI_MeMeDetector {
public:
    /** @brief Processes a get value command (Command 0xa1: Get MeMeDetector Variable)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     * @param[in] withStatus Whether the status message shall be written (not in subscription)
     */
    static bool processGet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage,
                           bool withStatus=true) throw(traci::TraCIException, std::invalid_argument);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_MeMeDetector(const TraCIServerAPI_MeMeDetector &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_MeMeDetector &operator=(const TraCIServerAPI_MeMeDetector &s);


};


#endif

/****************************************************************************/

