/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2026 German Aerospace Center (DLR) and others.
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
/// @file    EnergyParams.h
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Sept 2021
///
// A class for parameters used by the emission models
/****************************************************************************/
#pragma once
#include <config.h>
#include <map>
#include <string>
#include <vector>
#include <utils/common/SUMOVehicleClass.h>
#include <utils/common/ToString.h>
#include <utils/emissions/CharacteristicMap.h>
#include <utils/xml/SUMOXMLDefinitions.h>


// ===========================================================================
// class declarations
// ===========================================================================
class SUMOVTypeParameter;


// ===========================================================================
// class definitions
// ===========================================================================
/**
 * @class EnergyParams
 * @brief An upper class for objects with additional parameters
 */
class EnergyParams {
public:
    /// @brief Constructor
    EnergyParams(const SUMOVTypeParameter* typeParams = nullptr);

    /// @brief Constructor
    EnergyParams(const EnergyParams* secondaryParams) : mySecondaryParams(secondaryParams) {}

    /// @brief Destructor
    ~EnergyParams();

    /**@brief Set secondary params
     * @param[in] secondaryParams The secondary parameters
     */
    void setSecondary(const EnergyParams* secondaryParams) {
        mySecondaryParams = secondaryParams;
    }

    /**@brief Sets the values which change possibly in every simulation step and are relevant for emsssion calculation
     * @param[in] stopDuration the total duration of the current stop (-1 means no current stop)
     * @param[in] parking whether the current stop is a parking stop (only meaningful if stopDuration != -1)
     * @param[in] waitingTime the current total waiting time
     * @param[in] angle the current absolute angle of the vehicle
     */
    void setDynamicValues(const SUMOTime stopDuration, const bool parking, const SUMOTime waitingTime, const double angle);

    /**@brief Sets the empty mass of the vehicle (type)
     * This is to be used by traci/libsumo
     * @param[in] mass the new mass
     */
    void setMass(const double mass);

    /**@brief Returns the mass of all transportables in the vehicle
     * @return The total mass of persons and containers in kg
     */
    double getTransportableMass() const {
        return myTransportableMass;
    }

    /**@brief Sets the mass of all transportables in the vehicle
     * @param[in] mass the new mass
     */
    void setTransportableMass(const double mass);

    /**@brief Returns the sum of the empty mass (SUMO_ATTR_MASS), tthe loading (SUMO_ATTR_LOADING) and the mass of all transportables in the vehicle
     * @return The total mass in kg
     */
    double getTotalMass(const double defaultEmptyMass, const double defaultLoading) const;

    /**@brief Returns the angle difference between the last two calls of setDynamicValues (usually the last two time steps)
     * @return The angle difference in radians
     */
    double getAngleDiff() const;

    /**@brief Returns the value for a given key
     * @param[in] key The key to ask for
     * @return The value stored under the key
     */
    double getDouble(SumoXMLAttr attr) const;

    /**@brief Returns the value for a given key with an optional default.
     * SUMO_ATTR_MASS and SUMO_ATTR_FRONTSURFACEAREA get a special treatment and return the given def value
     *  also if the map has a value which is flagged as default.
     * @param[in] key The key to ask for
     * @param[in] def The default value if no value is stored or the stored value is flagged as default
     * @return The value stored under the key
     */
    double getDoubleOptional(SumoXMLAttr attr, const double def) const;

    /**
     * @brief Return the CharacteristicMap that belongs to a given attribute.
     *
     * @param[in] attr Name of an attribute
     * @return A CharacteristicMap
     */
    const CharacteristicMap& getCharacteristicMap(SumoXMLAttr attr) const;

    /// @brief Returns a complete inner description
    const std::string dump() const {
        return joinToString(myMap, ", ", ":") + (mySecondaryParams ? mySecondaryParams->dump() : "");
    }

    /** @brief Returns the state of the engine when the vehicle is not moving
     * @return whether the engine is running
     */
    bool isEngineOff() const;

    /** @brief Returns whether the vehicle is currently consuming any energy derived from the parking state
     * @return whether the vehicle has any consumption
     */
    bool isOff() const;

    static const EnergyParams* getDefault() {
        if (myDefault == nullptr) {
            myDefault = new EnergyParams();
        }
        return myDefault;
    }

private:
    /// @brief The key->value maps
    std::map<SumoXMLAttr, double> myMap;
    std::map<SumoXMLAttr, CharacteristicMap> myCharacteristicMapMap;
    const EnergyParams* mySecondaryParams = nullptr;
    bool myHaveDefaultMass = false;
    bool myHaveDefaultFrontSurfaceArea = false;
    double myStopDurationSeconds = -1.;
    bool myAmParking = false;
    double myWaitingTimeSeconds = -1.;
    double myLastAngle = INVALID_DOUBLE;
    double myAngle = INVALID_DOUBLE;
    double myTransportableMass = 0.;

    static const EnergyParams* myDefault;
    static const std::vector<SumoXMLAttr> myParamAttrs;

    /// @brief invalidate copy constructor
    EnergyParams(const EnergyParams& s) = delete;

    /// @brief invalidate assignment operator
    EnergyParams& operator=(const EnergyParams& s) = delete;
};
