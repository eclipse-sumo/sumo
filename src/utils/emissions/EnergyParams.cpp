/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2024 German Aerospace Center (DLR) and others.
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

#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/vehicle/SUMOVTypeParameter.h>

#include "PollutantsInterface.h"
#include "HelpersEnergy.h"
#include "EnergyParams.h"


// ===========================================================================
// static definitions
// ===========================================================================
const EnergyParams* EnergyParams::myDefault = nullptr;
const std::vector<SumoXMLAttr> EnergyParams::myParamAttrs = {
    SUMO_ATTR_SHUT_OFF_STOP, SUMO_ATTR_SHUT_OFF_AUTO,
    SUMO_ATTR_LOADING, SUMO_ATTR_FRONTSURFACEAREA, SUMO_ATTR_AIRDRAGCOEFFICIENT,
    SUMO_ATTR_CONSTANTPOWERINTAKE, SUMO_ATTR_WHEELRADIUS, SUMO_ATTR_ROLLDRAGCOEFFICIENT, SUMO_ATTR_ROTATINGMASS,
    SUMO_ATTR_RADIALDRAGCOEFFICIENT, SUMO_ATTR_PROPULSIONEFFICIENCY, SUMO_ATTR_RECUPERATIONEFFICIENCY,
    SUMO_ATTR_RECUPERATIONEFFICIENCY_BY_DECELERATION,
    SUMO_ATTR_MAXIMUMTORQUE, SUMO_ATTR_MAXIMUMPOWER, SUMO_ATTR_GEAREFFICIENCY, SUMO_ATTR_GEARRATIO,
    SUMO_ATTR_MAXIMUMRECUPERATIONTORQUE, SUMO_ATTR_MAXIMUMRECUPERATIONPOWER,
    SUMO_ATTR_INTERNALBATTERYRESISTANCE, SUMO_ATTR_NOMINALBATTERYVOLTAGE, SUMO_ATTR_INTERNALMOMENTOFINERTIA
};


// ===========================================================================
// method definitions
// ===========================================================================
EnergyParams::EnergyParams(const SUMOVTypeParameter* typeParams) {
    myMap[SUMO_ATTR_DURATION] = -1.;
    myMap[SUMO_ATTR_PARKING] = 0.;
    myMap[SUMO_ATTR_WAITINGTIME] = -1.;
    myMap[SUMO_ATTR_ANGLE] = 0.;  // actually angleDiff in the last step
    myCharacteristicMapMap.insert(std::pair<SumoXMLAttr, CharacteristicMap>(SUMO_ATTR_POWERLOSSMAP, CharacteristicMap("2,1|-1e9,1e9;-1e9,1e9|0,0,0,0")));  // P_loss_EM = 0 W for all operating points in the default EV power loss map

    if (typeParams != nullptr) {
        for (SumoXMLAttr attr : myParamAttrs) {
            if (typeParams->hasParameter(toString(attr))) {
                myMap[attr] = typeParams->getDouble(toString(attr), INVALID_DOUBLE);
            }
        }
        for (auto item : myCharacteristicMapMap) {
            std::string characteristicMapString = typeParams->getParameter(toString(item.first), "");
            if (characteristicMapString != "") {
                myCharacteristicMapMap.at(item.first) = CharacteristicMap(typeParams->getParameter(toString(item.first)));
            }
        }
        if (typeParams->wasSet(VTYPEPARS_MASS_SET)) {
            myMap[SUMO_ATTR_MASS] = typeParams->mass;
        }
        if (typeParams->wasSet(VTYPEPARS_WIDTH_SET)) {
            myMap[SUMO_ATTR_WIDTH] = typeParams->width;
        }
        if (typeParams->wasSet(VTYPEPARS_HEIGHT_SET)) {
            myMap[SUMO_ATTR_HEIGHT] = typeParams->height;
        }
        if (!StringUtils::startsWith(PollutantsInterface::getName(typeParams->emissionClass), "MMPEVEM")) {
            if (typeParams->hasParameter(toString(SUMO_ATTR_INTERNALMOMENTOFINERTIA))) {
                WRITE_WARNINGF(TL("Vehicle type '%' uses the Energy model with parameter 'internalMomentOfInertia' which is deprecated. Use 'rotatingMass' instead."), typeParams->id);
                if (!typeParams->hasParameter(toString(SUMO_ATTR_ROTATINGMASS))) {
                    myMap[SUMO_ATTR_ROTATINGMASS] = myMap[SUMO_ATTR_INTERNALMOMENTOFINERTIA];
                }
            }
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
    }
    if (mySecondaryParams != nullptr) {
        return mySecondaryParams->getDouble(attr);
    }
    throw UnknownElement("Unknown Energy Model parameter: " + toString(attr));
}


double
EnergyParams::getDoubleOptional(SumoXMLAttr attr, const double def) const {
    auto it = myMap.find(attr);
    if (it != myMap.end() && it->second != INVALID_DOUBLE) {
        return it->second;
    }
    if (mySecondaryParams != nullptr) {
        return mySecondaryParams->getDoubleOptional(attr, def);
    }
    return def;
}


const CharacteristicMap&
EnergyParams::getCharacteristicMap(SumoXMLAttr attr) const {
    auto it = myCharacteristicMapMap.find(attr);
    if (it != myCharacteristicMapMap.end()) {
        return it->second;
    }
    if (mySecondaryParams != nullptr) {
        return mySecondaryParams->getCharacteristicMap(attr);
    }
    throw UnknownElement("Unknown Energy Model parameter: " + toString(attr));
}


bool
EnergyParams::isEngineOff() const {
    // they all got a default in the constructor so getDouble is safe here
    return getDouble(SUMO_ATTR_DURATION) > getDoubleOptional(SUMO_ATTR_SHUT_OFF_STOP, DEFAULT_VEH_SHUT_OFF_STOP) ||
           getDouble(SUMO_ATTR_WAITINGTIME) > getDoubleOptional(SUMO_ATTR_SHUT_OFF_AUTO, std::numeric_limits<double>::max());
}


bool
EnergyParams::isOff() const {
    // they all got a default in the constructor so getDouble is safe here
    return getDouble(SUMO_ATTR_DURATION) > getDoubleOptional(SUMO_ATTR_SHUT_OFF_STOP, DEFAULT_VEH_SHUT_OFF_STOP) && getDouble(SUMO_ATTR_PARKING) > 0.;
}


/****************************************************************************/
