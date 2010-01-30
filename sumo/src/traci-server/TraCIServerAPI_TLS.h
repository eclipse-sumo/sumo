/****************************************************************************/
/// @file    TraCIServerAPI_TLS.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id: TraCIServerAPI_TLS.h 6907 2009-03-13 12:13:38Z dkrajzew $
///
// APIs for getting/setting traffic light values via TraCI
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
#ifndef TraCIServerAPI_TLS_h
#define TraCIServerAPI_TLS_h


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
 * @class TraCIServerAPI_TLS
 * @brief APIs for getting/setting traffic light values via TraCI
 */
class TraCIServerAPI_TLS {
public:
    /** @brief Processes a get value command (Command 0xa2: Get Traffic Lights Variable)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     * @param[in] withStatus Whether the status message shall be written (not in subscription)
     */
    static bool processGet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage,
        bool withStatus=true) throw(traci::TraCIException);


    /** @brief Processes a set value command (Command 0xc2: Change Traffic Lights State)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) throw(traci::TraCIException);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_TLS(const TraCIServerAPI_TLS &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_TLS &operator=(const TraCIServerAPI_TLS &s);


};


#endif

/****************************************************************************/

