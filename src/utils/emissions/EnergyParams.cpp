/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2021 German Aerospace Center (DLR) and others.
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
/// @file    EnergyParams.cpp
/// @author  Jakob Erdmann
/// @date    Sept 2021
///
// A class for parameters used by the emission models
/****************************************************************************/
#include <config.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/vehicle/SUMOVTypeParameter.h>

#include "EnergyParams.h"


// ===========================================================================
// method definitions
// ===========================================================================

EnergyParams::EnergyParams(const SUMOVTypeParameter* typeParams) {

    // default values from
    // Kurczveil, T., López, P.Á., & Schnieder, E. (2014). Implementation of an Energy Model and a Charging Infrastructure in SUMO.
    myMap[SUMO_ATTR_VEHICLEMASS] = 1000.;
    myMap[SUMO_ATTR_FRONTSURFACEAREA] = 5.;
    myMap[SUMO_ATTR_AIRDRAGCOEFFICIENT] = 0.6;
    myMap[SUMO_ATTR_INTERNALMOMENTOFINERTIA] = 0.01;
    myMap[SUMO_ATTR_RADIALDRAGCOEFFICIENT] = 0.5;
    myMap[SUMO_ATTR_ROLLDRAGCOEFFICIENT] = 0.01;
    myMap[SUMO_ATTR_CONSTANTPOWERINTAKE] = 100.;
    myMap[SUMO_ATTR_PROPULSIONEFFICIENCY] = 0.9;
    myMap[SUMO_ATTR_RECUPERATIONEFFICIENCY] = 0.8;
    myMap[SUMO_ATTR_RECUPERATIONEFFICIENCY_BY_DECELERATION] = 0.0;
    myMap[SUMO_ATTR_ANGLE] = 0.;  // actually angleDiff in the last step
    // @todo set myVecMap defaults as needed

    if (typeParams != nullptr) {
        for (auto item : myMap) {
            myMap[item.first] = typeParams->getDouble(toString(item.first), item.second);
        }
        for (auto item : myVecMap) {
            myVecMap[item.first] = typeParams->getDoubles(toString(item.first), item.second);
        }
    }
}




EnergyParams::~EnergyParams() {}


void
EnergyParams::setDouble(SumoXMLAttr attr, double value) {
    myMap[attr] = value;
}

double
EnergyParams::getDouble(SumoXMLAttr attr) const {
    auto it = myMap.find(attr);
    if (it != myMap.end()) {
        return it->second;
    } else {
        throw UnknownElement("Unknown Energy Model parameter: " + toString(attr));
    }
}

const std::vector<double>&
EnergyParams::getDoubles(SumoXMLAttr attr) const {
    auto it = myVecMap.find(attr);
    if (it != myVecMap.end()) {
        return it->second;
    } else {
        throw UnknownElement("Unknown Energy Model parameter: " + toString(attr));
    }
}


bool
EnergyParams::knowsParameter(SumoXMLAttr attr) const {
    return myMap.find(attr) != myMap.end();
}

/****************************************************************************/
