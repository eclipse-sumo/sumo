/****************************************************************************/
/// @file    TraCIServerAPI_Polygon.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// APIs for getting/setting polygon values via TraCI
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
#ifndef TraCIServerAPI_Polygon_h
#define TraCIServerAPI_Polygon_h


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
 * @class TraCIServerAPI_Polygon
 * @brief APIs for getting/setting polygon values via TraCI
 */
class TraCIServerAPI_Polygon {
public:
    /** @brief Processes a get value command (Command 0xa8: Get Polygon Variable)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     * @param[in] withStatus Whether the status message shall be written (not in subscription)
     */
    static bool processGet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage,
                           bool withStatus=true) throw(traci::TraCIException);


    /** @brief Processes a set value command (Command 0xc8: Change Polygon State)
     *
     * @param[in] inputStorage The storage to read the command from
     * @param[out] outputStorage The storage to write the result to
     */
    static bool processSet(tcpip::Storage &inputStorage, tcpip::Storage &outputStorage) throw(traci::TraCIException);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPI_Polygon(const TraCIServerAPI_Polygon &s);

    /// @brief invalidated assignment operator
    TraCIServerAPI_Polygon &operator=(const TraCIServerAPI_Polygon &s);


};


#endif

/****************************************************************************/

