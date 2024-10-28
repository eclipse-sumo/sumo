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


// ===========================================================================
// method definitions
// ===========================================================================
EnergyParams::EnergyParams(const SUMOVTypeParameter* typeParams) :
    EnergyParams(typeParams != nullptr ? typeParams->emissionClass : EMISSION_CLASS_UNSPECIFIED) {
    if (typeParams != nullptr) {
        for (auto item : myMap) {
            myMap[item.first] = typeParams->getDouble(toString(item.first), item.second);
        }
        for (auto item : myVecMap) {
            myVecMap[item.first] = typeParams->getDoubles(toString(item.first), item.second);
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
        myMap[SUMO_ATTR_LOADING] = typeParams->getDouble("loading", INVALID_DOUBLE);
        myMap[SUMO_ATTR_WIDTH] = typeParams->width;
        myMap[SUMO_ATTR_HEIGHT] = typeParams->height;
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


EnergyParams::EnergyParams(const SUMOEmissionClass c) {
    myMap[SUMO_ATTR_SHUT_OFF_STOP] = 300.;
    myMap[SUMO_ATTR_SHUT_OFF_AUTO] = std::numeric_limits<double>::max();
    myMap[SUMO_ATTR_DURATION] = -1.;
    myMap[SUMO_ATTR_PARKING] = 0.;
    myMap[SUMO_ATTR_WAITINGTIME] = -1.;

    if (c != EMISSION_CLASS_UNSPECIFIED && StringUtils::startsWith(PollutantsInterface::getName(c), "PHEMlight5/")) {
        myMap[SUMO_ATTR_MASS] = INVALID_DOUBLE;
        myMap[SUMO_ATTR_LOADING] = INVALID_DOUBLE;
        myMap[SUMO_ATTR_FRONTSURFACEAREA] = INVALID_DOUBLE;
        myMap[SUMO_ATTR_AIRDRAGCOEFFICIENT] = INVALID_DOUBLE;
        myMap[SUMO_ATTR_CONSTANTPOWERINTAKE] = INVALID_DOUBLE;
        myMap[SUMO_ATTR_WHEELRADIUS] = INVALID_DOUBLE;
        myMap[SUMO_ATTR_ROLLDRAGCOEFFICIENT] = INVALID_DOUBLE;
        myMap[SUMO_ATTR_ROTATINGMASS] = INVALID_DOUBLE;
        return;
    }
    const SUMOVTypeParameter::VClassDefaultValues defaultValues(SVC_PASSENGER);
    myMap[SUMO_ATTR_MASS] = defaultValues.mass;
    myMap[SUMO_ATTR_LOADING] = INVALID_DOUBLE;
    myMap[SUMO_ATTR_WIDTH] = defaultValues.width;
    myMap[SUMO_ATTR_HEIGHT] = defaultValues.height;

    // default values from
    // https://sumo.dlr.de/docs/Models/Electric.html#kia_soul_ev_2020
    if (c != EMISSION_CLASS_UNSPECIFIED && StringUtils::startsWith(PollutantsInterface::getName(c), "Energy/")) {
        myMap[SUMO_ATTR_MASS] = 1830.;
    }
    myMap[SUMO_ATTR_FRONTSURFACEAREA] = 2.6;
    myMap[SUMO_ATTR_AIRDRAGCOEFFICIENT] = 0.35;
    myMap[SUMO_ATTR_ROTATINGMASS] = 40.;
    myMap[SUMO_ATTR_RADIALDRAGCOEFFICIENT] = 0.1;
    myMap[SUMO_ATTR_ROLLDRAGCOEFFICIENT] = 0.01;
    myMap[SUMO_ATTR_CONSTANTPOWERINTAKE] = 100.;
    myMap[SUMO_ATTR_PROPULSIONEFFICIENCY] = 0.98;
    myMap[SUMO_ATTR_RECUPERATIONEFFICIENCY] = 0.96;
    myMap[SUMO_ATTR_RECUPERATIONEFFICIENCY_BY_DECELERATION] = 0.0;
    myMap[SUMO_ATTR_ANGLE] = 0.;  // actually angleDiff in the last step
    // @todo set myVecMap defaults as needed

    // Default values for the MMPEVEM, taken from the VW_ID3
    myMap[SUMO_ATTR_WHEELRADIUS] = 0.3588;                // [m]
    myMap[SUMO_ATTR_MAXIMUMTORQUE] = 310.0;               // [Nm]
    // @todo SUMO_ATTR_MAXIMUMPOWER predates the MMPEVEM emission model. Do you want to set this somewhere else or to another value?
    myMap[SUMO_ATTR_MAXIMUMPOWER] = 107000.0;             // [W]
    myMap[SUMO_ATTR_GEAREFFICIENCY] = 0.96;               // [1]
    myMap[SUMO_ATTR_GEARRATIO] = 10.0;                    // [1]
    myMap[SUMO_ATTR_MAXIMUMRECUPERATIONTORQUE] = 95.5;    // [Nm]
    myMap[SUMO_ATTR_MAXIMUMRECUPERATIONPOWER] = 42800.0;  // [W]
    myMap[SUMO_ATTR_INTERNALBATTERYRESISTANCE] = 0.1142;  // [Ohm]
    myMap[SUMO_ATTR_NOMINALBATTERYVOLTAGE] = 396.0;       // [V]
    myMap[SUMO_ATTR_INTERNALMOMENTOFINERTIA] = 12.5;      // [kgm^2]
    myCharacteristicMapMap.insert(std::pair<SumoXMLAttr, CharacteristicMap>(SUMO_ATTR_POWERLOSSMAP, CharacteristicMap("2,1|-1e9,1e9;-1e9,1e9|0,0,0,0")));  // P_loss_EM = 0 W for all operating points in the default EV power loss map
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


const std::vector<double>&
EnergyParams::getDoubles(SumoXMLAttr attr) const {
    auto it = myVecMap.find(attr);
    if (it != myVecMap.end()) {
        return it->second;
    }
    if (mySecondaryParams != nullptr) {
        return mySecondaryParams->getDoubles(attr);
    }
    throw UnknownElement("Unknown Energy Model parameter: " + toString(attr));
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


void
EnergyParams::checkParam(const SumoXMLAttr paramKey, const std::string& id, const double lower, const double upper) {
    const auto& p = myMap.find(paramKey);
    if (p != myMap.end() && (p->second < lower || p->second > upper)) {
        WRITE_WARNINGF(TL("Vehicle device '%' doesn't have a valid value for parameter % (%)."), id, toString(paramKey), p->second);
        setDouble(paramKey, EnergyParams::getDefault()->getDouble(paramKey));
    }
}


bool
EnergyParams::isEngineOff() const {
    // they all got a default in the constructor so getDouble is safe here
    return getDouble(SUMO_ATTR_DURATION) > getDouble(SUMO_ATTR_SHUT_OFF_STOP) ||
           getDouble(SUMO_ATTR_WAITINGTIME) > getDouble(SUMO_ATTR_SHUT_OFF_AUTO);
}


bool
EnergyParams::isOff() const {
    // they all got a default in the constructor so getDouble is safe here
    return getDouble(SUMO_ATTR_DURATION) > getDouble(SUMO_ATTR_SHUT_OFF_STOP) && getDouble(SUMO_ATTR_PARKING) > 0.;
}


/****************************************************************************/
