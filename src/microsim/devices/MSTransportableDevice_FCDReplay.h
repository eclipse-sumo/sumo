/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2013-2024 German Aerospace Center (DLR) and others.
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
/// @file    MSTransportableDevice_FCDReplay.h
/// @author  Michael Behrisch
/// @date    01.03.2024
///
// A device which replays recorded floating car data
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/common/Command.h>
#include "MSTransportableDevice.h"
#include "MSDevice_FCDReplay.h"


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSTransportableDevice_FCDReplay
 * @brief A device which collects info on the vehicle trip (mainly on departure and arrival)
 *
 * Each device collects departure time, lane and speed and the same for arrival.
 *
 * @see MSTransportableDevice
 */
class MSTransportableDevice_FCDReplay : public MSTransportableDevice {
public:
    /** @brief Inserts MSTransportableDevice_FCDReplay-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);


    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a FCD-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildDevices(MSTransportable& t, std::vector<MSTransportableDevice*>& into);

public:
    /// @brief Destructor.
    ~MSTransportableDevice_FCDReplay();

    /// @brief return the name for this type of device
    const std::string deviceName() const {
        return "fcd-replay";
    }

    void setTrajectory(MSDevice_FCDReplay::Trajectory* const t) {
        myTrajectory = t;
    }

    bool move(SUMOTime currentTime);

private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSTransportableDevice_FCDReplay(MSTransportable& holder, const std::string& id);

    class MovePedestrians : public Command {
    public:
        SUMOTime execute(SUMOTime currentTime);
    private:
        /// @brief Invalidated assignment operator.
        MovePedestrians& operator=(const MovePedestrians&) = delete;
    };

private:
    MSDevice_FCDReplay::Trajectory* myTrajectory = nullptr;

    /// @brief whether an event for pedestrian processing was added
    static bool myAmActive;

private:
    /// @brief Invalidated copy constructor.
    MSTransportableDevice_FCDReplay(const MSTransportableDevice_FCDReplay&);

    /// @brief Invalidated assignment operator.
    MSTransportableDevice_FCDReplay& operator=(const MSTransportableDevice_FCDReplay&);


};
