/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEDemandElementDistribution.h
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// A abstract class for demand elements distributions
/****************************************************************************/
#pragma once
#include <config.h>

#include <utils/xml/SUMOXMLDefinitions.h>

// ===========================================================================
// class declaration
// ===========================================================================

class GNEUndoList;

// ===========================================================================
// class definitions
// ===========================================================================

/**
 * @class GNEDemandElementDistribution
 * @brief A
 */
class GNEDemandElementDistribution {

public:
    /// @brief Constructor
    GNEDemandElementDistribution();

    /// @brief get attribute distribution (only keys)
    std::string getAttributeDistribution() const;

    /**@brief add distribution
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void addDistribution(const std::string& key, const std::string& value, GNEUndoList* undoList);

    /**@brief remove distribution
     * @param[in] key The attribute key
     * @param[in] value The new value
     * @param[in] undoList The undoList on which to register changes
     */
    void removeDistribution(const std::string& key, GNEUndoList* undoList);

    /**@brief method for checking if the key and their conrrespond attribute are valids
     * @param[in] key The attribute key
     * @param[in] value The value associated to key key
     * @return true if the value is valid, false in other case
     */
    bool isValidDistribution(const std::string& key, const std::string& value);

    /// @}

protected:

    

private:
    /// @brief add distribution (used in GNEChange_Attribute)
    void addDistribution(const std::string& key, const std::string& value);

    /// @brief remove distribution
    void removeDistribution(const std::string& key);

    /// @brief Invalidated copy constructor.
    GNEDemandElementDistribution(const GNEDemandElementDistribution&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementDistribution& operator=(const GNEDemandElementDistribution&) = delete;
};
