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

class GNEDemandElementDistribution {

    /// @brief friend declaration
    friend class GNEChange_Distribution;
    friend class GNERouteHandler;

public:
    /// @brief Constructor
    GNEDemandElementDistribution(GNEDemandElement* demandElement);

    /// @brief check if distribution is empty
    bool isDistributionEmpty() const;

    /// @brief check if the given key can be added in distribution
    bool keyExists(const GNEDemandElement* key) const;

    /// @brief check if the given key-value can be added in distribution
    bool isValueValid(const GNEDemandElement* key, const double value) const;

    /// @brief get map with distribution keys and values
    const std::map<const GNEDemandElement*, double> &getDistributionKeyValues() const;

    /// @brief get list of possible keys sorted by ID
    std::map<std::string, GNEDemandElement*> getPossibleDistributionKeys(SumoXMLTag type) const;

    /// @brief get attribute distribution keys in string format sorted by ID
    std::string getAttributeDistributionKeys() const;

    /// @brief get attribute distribution keys in string format sorted by ID
    std::string getAttributeDistributionValues() const;

    /// @brief get attribute distribution value
    double getAttributeDistributionValue(const GNEDemandElement* key);

    /// @brief add distribution key
    void addDistributionKey(const GNEDemandElement* key, const double value, GNEUndoList* undoList);

    /// @brief remove distribution key
    void removeDistributionKey(const GNEDemandElement* key, GNEUndoList* undoList);

    /// @brief remove distribution (used in GNEDemandElementDistribution)
    void editDistributionValue(const GNEDemandElement* key, const double newValue, GNEUndoList* undoList);

private:
    /// @brief demand element
    GNEDemandElement* myDemandElement;
    
    /// @brief map with distribution keys and values
    std::map<const GNEDemandElement*, double> myDistributionValues;
    
    /// @brief add distribution (used in GNEDemandElementDistribution)
    void addDistributionKey(const GNEDemandElement* key, const double value);

    /// @brief remove distribution (used in GNEDemandElementDistribution)
    void removeDistributionKey(const GNEDemandElement* key);

    /// @brief remove distribution (used in GNEDemandElementDistribution)
    void editDistributionValue(const GNEDemandElement* key, const double newValue);

    /// @brief Invalidated copy constructor.
    GNEDemandElementDistribution(const GNEDemandElementDistribution&) = delete;

    /// @brief Invalidated assignment operator.
    GNEDemandElementDistribution& operator=(const GNEDemandElementDistribution&) = delete;
};
