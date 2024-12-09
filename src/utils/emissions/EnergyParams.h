/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2002-2024 German Aerospace Center (DLR) and others.
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

    void setDynamicValues(const SUMOTime stopDuration, const bool parking, const SUMOTime waitingTime, const double angle);
    void setMass(const double mass);

    double getTransportableMass() const {
        return myTransportableMass;
    }

    void setTransportableMass(const double mass);

    double getAngleDiff() const;

    double getDouble(SumoXMLAttr attr) const;
    double getDoubleOptional(SumoXMLAttr attr, const double def, const bool useStoredDefault = true) const;

    /**@brief Returns the value for a given key
     * @param[in] key The key to ask for
     * @return The value stored under the key
     */
    const std::vector<double>& getDoubles(SumoXMLAttr attr) const;

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
