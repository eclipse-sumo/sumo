/****************************************************************************/
/// @file    MSBatteryExport.h
/// @author  Mario Krumnow
/// @author  Tamas Kurczveil
/// @author  Pablo Alvarez Lopez
/// @date    20-12-13
/// @version $Id$
///
// Realises dumping Battery Data
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
#ifndef MSBatteryExport_h
#define MSBatteryExport_h


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSBatteryExport
 * @brief Realises dumping Battery Data
 *
 *  The class offers a static method, which writes all available Battery factors
 *  of each vehicles of the network into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSBatteryExport {
public:
    /** @brief Writes the complete network state of the given edges into the given device
     *
     *  Opens the current time step and export the Battery factors of all availabel vehicles
     *
     * @param[in] of The output device to use
     * @param[in] timestep The current time step
     * @param[in] precision The output precision
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void write(OutputDevice& of, SUMOTime timestep, int precision);


private:
    /// @brief Invalidated copy constructor.
    MSBatteryExport(const MSBatteryExport&);

    /// @brief Invalidated assignment operator.
    MSBatteryExport& operator=(const MSBatteryExport&);

};


#endif

