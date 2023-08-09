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
/// @file    GNEDemandElementDistribution.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Aug 2023
///
// A abstract class for demand elements distributions
/****************************************************************************/
#include <config.h>

#include <netedit/GNEUndoList.h>
#include <netedit/changes/GNEChange_Distribution.h>

#include "GNEDemandElementDistribution.h"

// ===========================================================================
// member method definitions
// ===========================================================================

// ---------------------------------------------------------------------------
// GNEDemandElementDistribution - methods
// ---------------------------------------------------------------------------

GNEDemandElementDistribution::GNEDemandElementDistribution(GNEDemandElement* demandElement) :
    myDemandElement(demandElement) {
}


std::string
GNEDemandElementDistribution::getAttributeDistributionKeys() const {
    // first sort all keys by ID
    std::set<std::string> sortedKeys;
    for (const auto &values : myDistributionValues) {
        sortedKeys.insert(values.first->getID());
    }
    // return keySortd
    return toString(sortedKeys);
}


std::string
GNEDemandElementDistribution::getAttributeDistributionValues() const {
    // first sort all keys by ID
    std::map<std::string, const GNEDemandElement*> sortedKeys;
    for (const auto &values : myDistributionValues) {
        sortedKeys[values.first->getID()] = values.first;
    }
    // now obtain values sorted by ID
    std::vector<double> values;
    for (const auto &sortedKey : sortedKeys) {
        values.push_back(myDistributionValues.at(sortedKey.second));
    }
    return toString(values);
}


double
GNEDemandElementDistribution::getAttributeDistributionValue(const GNEDemandElement* key) {
    if (myDistributionValues.count(key) > 0) {
        return myDistributionValues.at(key);
    } else {
        throw ProcessError("Key doesn't exist");
    }
}


void
GNEDemandElementDistribution::addDistributionKey(const GNEDemandElement* key, const double value, GNEUndoList* undoList) {
    GNEChange_Distribution::addKey(myDemandElement, key, value, undoList);
}


void
GNEDemandElementDistribution::removeDistributionKey(const GNEDemandElement* key, GNEUndoList* undoList) {
    GNEChange_Distribution::removeKey(myDemandElement, key, undoList);
}


bool
GNEDemandElementDistribution::keyExists(const GNEDemandElement* key) const {
    return (myDistributionValues.count(key) > 0);
}


bool
GNEDemandElementDistribution::isValueValid(const GNEDemandElement* key, const double value) const {
    if (myDistributionValues.count(key) > 0) {
        return ((value >= 0) && (value <= 1));
    } else {
        return false;
    }
}


void
GNEDemandElementDistribution::addDistributionKey(const GNEDemandElement* key, const double value) {
    if (myDistributionValues.count(key) == 0) {
        myDistributionValues[key] = value;
    } else {
        throw ProcessError("Key already exist");
    }
}


void
GNEDemandElementDistribution::removeDistributionKey(const GNEDemandElement* key) {
    if (myDistributionValues.count(key) > 0) {
        myDistributionValues.erase(key);
    } else {
        throw ProcessError("Key doesn't exist");
    }
}


void
GNEDemandElementDistribution::editDistributionValue(const GNEDemandElement* key, const double newValue) {
    myDistributionValues[key] = newValue;
}

/****************************************************************************/
