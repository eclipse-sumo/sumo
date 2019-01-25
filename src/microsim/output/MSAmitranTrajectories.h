/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2014-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    MSAmitranTrajectories.h
/// @author  Michael Behrisch
/// @date    13.03.2014
/// @version $Id$
///
// Realises dumping the complete network state
/****************************************************************************/
#ifndef MSAmitranTrajectories_h
#define MSAmitranTrajectories_h


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <utils/common/SUMOTime.h>


// ===========================================================================
// class declarations
// ===========================================================================
class OutputDevice;
class MSVehicleControl;
class MSEdge;
class MSBaseVehicle;
class MSLane;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSAmitranTrajectories
 * @brief Realises dumping the complete network state
 *
 * The class offers a static method, which writes the complete dump of
 *  the given network into the given OutputDevice.
 *
 * @todo consider error-handling on write (using IOError)
 */
class MSAmitranTrajectories {
public:
    /** @brief Writes the complete network state into the given device
     *
     * Opens the current time step, goes through the vehicles and writes each using
     *  writeVehicle.
     *
     * @param[in] of The output device to use
     * @param[in] timestep The current time step
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void write(OutputDevice& of, const SUMOTime timestep);


private:
    /** @brief Writes the dump of the given vehicle into the given device
     *
     * @param[in] of The output device to use
     * @param[in] veh The vehicle to dump
     * @param[in] timestep The current time step
     * @exception IOError If an error on writing occurs (!!! not yet implemented)
     */
    static void writeVehicle(OutputDevice& of, const SUMOVehicle& veh, const SUMOTime timestep);


private:
    static std::set<std::string> myWrittenTypes;
    static std::map<std::string, int> myWrittenVehicles;


private:
    /// @brief Invalidated copy constructor.
    MSAmitranTrajectories(const MSAmitranTrajectories&);

    /// @brief Invalidated assignment operator.
    MSAmitranTrajectories& operator=(const MSAmitranTrajectories&);


};


#endif

/****************************************************************************/

