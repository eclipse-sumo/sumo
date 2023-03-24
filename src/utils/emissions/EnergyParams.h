/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2002-2023 German Aerospace Center (DLR) and others.
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
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/emissions/CharacteristicMap.h>


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

    /**@brief Sets a parameter
     * @param[in] key The parameter's name
     * @param[in] value The parameter's value
     */
    void setDouble(SumoXMLAttr attr, double value);

    double getDouble(SumoXMLAttr attr) const;

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

    void checkParam(const SumoXMLAttr paramKey, const std::string& id, const double lower = 0., const double upper = std::numeric_limits<double>::infinity());

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
    std::map<SumoXMLAttr, std::vector<double> > myVecMap;
    std::map<SumoXMLAttr, CharacteristicMap> myCharacteristicMapMap;
    const EnergyParams* mySecondaryParams = nullptr;

    static const EnergyParams* myDefault;
};
