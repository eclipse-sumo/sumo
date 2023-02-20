/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2013-2023 German Aerospace Center (DLR) and others.
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
/// @file    MSDevice_Friction.h
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @date    11.06.2013
///
// A device which stands as an implementation example and which outputs movereminder calls
/****************************************************************************/
#pragma once
#include <config.h>

#include "MSVehicleDevice.h"


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOTrafficObject;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class MSDevice_Friction
 * @brief A device which collects info on current friction Coefficient on the road
 *
 * Each device collects friction -> insert Measurment modell if applicapble
 *
 * @see MSDevice
 */
class MSDevice_Friction : public MSVehicleDevice {
public:
    /** @brief Inserts MSDevice_Friction-options
     * @param[filled] oc The options container to add the options to
     */
    static void insertOptions(OptionsCont& oc);

    /** @brief Build devices for the given vehicle, if needed
     *
     * The options are read and evaluated whether a example-device shall be built
     *  for the given vehicle.
     *
     * The built device is stored in the given vector.
     *
     * @param[in] v The vehicle for which a device may be built
     * @param[filled] into The vector to store the built device in
     */
    static void buildVehicleDevices(SUMOVehicle& v, std::vector<MSVehicleDevice*>& into);

public:
    /// @brief Destructor.
    ~MSDevice_Friction();

    /// @name Methods called on vehicle movement / state change, overwriting MSDevice
    /// @{

    /** @brief Checks for waiting steps when the vehicle moves
     *
     * @param[in] veh Vehicle that asks this reminder.
     * @param[in] oldPos Position before move.
     * @param[in] newPos Position after move with newSpeed.
     * @param[in] newSpeed speed update.
     *
     * @return True (always).
     */
    bool notifyMove(SUMOTrafficObject& veh, double oldPos,
                    double newPos, double newSpeed);
    /// @}

    /// @brief return the name for this type of device
    const std::string deviceName() const {
        return "friction";
    }

    /// @brief try to retrieve the given parameter from this device. Throw exception for unsupported key
    std::string getParameter(const std::string& key) const;

    /// @brief try to set the given parameter for this device. Throw exception for unsupported key
    void setParameter(const std::string& key, const std::string& value);

    inline double getMeasuredFriction() {
        return myMeasuredFrictionCoefficient;
    }

private:
    /** @brief Constructor
     *
     * @param[in] holder The vehicle that holds this device
     * @param[in] id The ID of the device
     */
    MSDevice_Friction(SUMOVehicle& holder, const std::string& id,
                      double stdDev, double offset);

private:
    // private state members of the Friction device

    /// @brief a value which is initialised based on a commandline/configuration option
    double myMeasuredFrictionCoefficient;

    /// @brief realValue from Road (without measurement model)
    double myRawFriction;

    /// @brief a value which is initialised based on a vehicle parameter
    double myStdDeviation;

    /// @brief a value which is initialised based on a vType parameter
    double myOffset;

private:
    /// @brief Invalidated copy constructor.
    MSDevice_Friction(const MSDevice_Friction&);

    /// @brief Invalidated assignment operator.
    MSDevice_Friction& operator=(const MSDevice_Friction&);

};
