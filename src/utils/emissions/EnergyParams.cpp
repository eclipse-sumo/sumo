/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2026 German Aerospace Center (DLR) and others.
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
/// @author  Michael Behrisch
/// @date    Sept 2021
///
// A class for parameters used by the emission models
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/geom/GeomHelper.h>
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
    myCharacteristicMapMap.insert(std::pair<SumoXMLAttr, CharacteristicMap>(SUMO_ATTR_POWERLOSSMAP, CharacteristicMap("2,1|-1e9,1e9;-1e9,1e9|0,0,0,0")));  // P_loss_EM = 0 W for all operating points in the default EV power loss map

    if (typeParams == nullptr) {
        myMap[SUMO_ATTR_MASS] = DEFAULT_VEH_MASS;
        myHaveDefaultMass = true;
        myMap[SUMO_ATTR_FRONTSURFACEAREA] = DEFAULT_VEH_WIDTH * DEFAULT_VEH_HEIGHT * M_PI / 4.;
        myHaveDefaultFrontSurfaceArea = true;
    } else {
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
        myMap[SUMO_ATTR_MASS] = typeParams->mass;
        myHaveDefaultMass = !typeParams->wasSet(VTYPEPARS_MASS_SET);
        if (myHaveDefaultMass) {
            const double ecMass = PollutantsInterface::getWeight(typeParams->emissionClass);
            if (ecMass != -1.) {
                myMap[SUMO_ATTR_MASS] = ecMass;
            }
        }
        if (myMap.count(SUMO_ATTR_FRONTSURFACEAREA) == 0) {
            myHaveDefaultFrontSurfaceArea = true;
            myMap[SUMO_ATTR_FRONTSURFACEAREA] = typeParams->width * typeParams->height * M_PI / 4.;
        }
        const std::string& ecName = PollutantsInterface::getName(typeParams->emissionClass);
        if (typeParams->vehicleClass != SVC_IGNORING && (typeParams->vehicleClass & (SVC_PRIVATE | SVC_VIP | SVC_PASSENGER | SVC_HOV | SVC_TAXI | SVC_E_VEHICLE | SVC_CUSTOM1 | SVC_CUSTOM2)) == 0 && myHaveDefaultFrontSurfaceArea) {
            if (StringUtils::startsWith(ecName, "MMPEVEM") || StringUtils::startsWith(ecName, "Energy")) {
                WRITE_WARNINGF(TL("Vehicle type '%' uses the emission class '%' which does not have proper defaults for its vehicle class. "
                                  "Please use a different emission model or complete the vType definition with further parameters."), typeParams->id, ecName);
                if (!typeParams->wasSet(VTYPEPARS_MASS_SET)) {
                    WRITE_WARNING(TL(" And also set a vehicle mass!"));
                }
            }
        }
        if (!StringUtils::startsWith(ecName, "MMPEVEM")) {
            if (myMap.count(SUMO_ATTR_INTERNALMOMENTOFINERTIA) > 0) {
                WRITE_WARNINGF(TL("Vehicle type '%' uses the Energy model parameter 'internalMomentOfInertia' which is deprecated. Use 'rotatingMass' instead."), typeParams->id);
                if (!typeParams->hasParameter(toString(SUMO_ATTR_ROTATINGMASS))) {
                    myMap[SUMO_ATTR_ROTATINGMASS] = myMap[SUMO_ATTR_INTERNALMOMENTOFINERTIA];
                }
            }
        }
    }
}


EnergyParams::~EnergyParams() {}


void
EnergyParams::setDynamicValues(const SUMOTime stopDuration, const bool parking, const SUMOTime waitingTime, const double angle) {
    if ((stopDuration >= 0 && myStopDurationSeconds < 0.) || (stopDuration < 0 && myStopDurationSeconds >= 0.)) {
        myStopDurationSeconds = STEPS2TIME(stopDuration);
        myAmParking = parking;
    }
    myWaitingTimeSeconds = STEPS2TIME(waitingTime);
    myLastAngle = myAngle;
    myAngle = angle;
}


void
EnergyParams::setMass(const double mass) {
    myMap[SUMO_ATTR_MASS] = mass;
    myHaveDefaultMass = false;
}


void
EnergyParams::setTransportableMass(const double mass) {
    myTransportableMass = mass;
    assert(myTransportableMass >= 0);
}


double
EnergyParams::getTotalMass(const double defaultEmptyMass, const double defaultLoading) const {
    return getDoubleOptional(SUMO_ATTR_MASS, defaultEmptyMass) + getDoubleOptional(SUMO_ATTR_LOADING, defaultLoading) + getTransportableMass();
}


double
EnergyParams::getAngleDiff() const {
    return myLastAngle == INVALID_DOUBLE ? 0. : GeomHelper::angleDiff(myLastAngle, myAngle);
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
    throw UnknownElement("Unknown emission model parameter: " + toString(attr));
}


double
EnergyParams::getDoubleOptional(SumoXMLAttr attr, const double def) const {
    auto it = myMap.find(attr);
    if (it != myMap.end() && it->second != INVALID_DOUBLE) {
        if (attr == SUMO_ATTR_MASS) {
            if (!myHaveDefaultMass) {
                return it->second;
            }
        } else if (attr == SUMO_ATTR_FRONTSURFACEAREA) {
            if (!myHaveDefaultFrontSurfaceArea) {
                return it->second;
            }
        } else {
            return it->second;
        }
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
    throw UnknownElement("Unknown emission model parameter: " + toString(attr));
}


bool
EnergyParams::isEngineOff() const {
    return myStopDurationSeconds > getDoubleOptional(SUMO_ATTR_SHUT_OFF_STOP, DEFAULT_VEH_SHUT_OFF_STOP) ||
           myWaitingTimeSeconds > getDoubleOptional(SUMO_ATTR_SHUT_OFF_AUTO, std::numeric_limits<double>::max());
}


bool
EnergyParams::isOff() const {
    return myStopDurationSeconds > getDoubleOptional(SUMO_ATTR_SHUT_OFF_STOP, DEFAULT_VEH_SHUT_OFF_STOP);
}


/****************************************************************************/
