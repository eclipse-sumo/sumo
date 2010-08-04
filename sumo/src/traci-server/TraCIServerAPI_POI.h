/****************************************************************************/
/// @file    TraCIServerAPI_POI.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting POI values via TraCI
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
#ifndef TraCIServerAPI_POI_h
#define TraCIServerAPI_POI_h


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
 * @class TraCIServerAPI_POI
 * @brief APIs for getting/setting POI values via TraCI
 */
class TraCIServerAPI_POI {
public:
    /** @brief Processes a get value command (Command 0xa7: Get PoI Variable)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processGet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) throw(traci::TraCIException, std::invalid_argument);


    /** @brief Processes a set value command (Command 0xc7: Change PoI State)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) throw(traci::TraCIException, std::invalid_argument);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_POI(const TraCIServerAPI_POI &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_POI &operator=(const TraCIServerAPI_POI &s);


};


#endif

/****************************************************************************/

