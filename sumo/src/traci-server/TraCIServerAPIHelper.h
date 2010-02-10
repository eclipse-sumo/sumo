/****************************************************************************/
/// @file    TraCIServerAPIHelper.h
/// @author  Daniel Krajzewicz
/// @date    07.05.2009
/// @version $Id$
///
// Some helper functions for communication
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
#ifndef TraCIServerAPIHelper_h
#define TraCIServerAPIHelper_h


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
 * @class TraCIServerAPIHelper
 * @brief Some helper functions for communication
 */
class TraCIServerAPIHelper {
public:
    /** @brief Writes the status command
     *
     * @param[in] commandId The command id
     * @param[in] status The return status
     * @param[in] description The message
     * @param[out] outputStorage The storage to write the status to
     */
    static void writeStatusCmd(int commandId, int status, const std::string &description, tcpip::Storage &outputStorage);


private:
    /// @brief invalidated copy constructor
    TraCIServerAPIHelper(const TraCIServerAPIHelper &s);

    /// @brief invalidated assignment operator
    TraCIServerAPIHelper &operator=(const TraCIServerAPIHelper &s);


};


#endif

/****************************************************************************/

