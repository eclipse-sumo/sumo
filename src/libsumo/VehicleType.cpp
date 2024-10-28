/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2017-2024 German Aerospace Center (DLR) and others.
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
/// @file    VehicleType.cpp
/// @author  Gregor Laemmel
/// @date    04.04.2017
///
// C++ TraCI client API implementation
/****************************************************************************/
#include <config.h>

#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <microsim/MSVehicleType.h>
#include <libsumo/TraCIConstants.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/vehicle/SUMOVehicleParserHelper.h>
#include "Helper.h"
#include "VehicleType.h"


namespace libsumo {
// ===========================================================================
// static member initializations
// ===========================================================================
SubscriptionResults VehicleType::mySubscriptionResults;
ContextSubscriptionResults VehicleType::myContextSubscriptionResults;


// ===========================================================================
// static member definitions
// ===========================================================================
std::vector<std::string>
VehicleType::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getVehicleControl().insertVTypeIDs(ids);
    return ids;
}


int
VehicleType::getIDCount() {
    return (int)getIDList().size();
}


double
VehicleType::getLength(const std::string& typeID) {
    return getVType(typeID)->getLength();
}


double
VehicleType::getMaxSpeed(const std::string& typeID) {
    return getVType(typeID)->getMaxSpeed();
}


double
VehicleType::getActionStepLength(const std::string& typeID) {
    return getVType(typeID)->getActionStepLengthSecs();
}


double
VehicleType::getSpeedFactor(const std::string& typeID) {
    return getVType(typeID)->getSpeedFactor().getParameter()[0];
}


double
VehicleType::getSpeedDeviation(const std::string& typeID) {
    return getVType(typeID)->getSpeedFactor().getParameter()[1];
}


double
VehicleType::getAccel(const std::string& typeID) {
    return getVType(typeID)->getCarFollowModel().getMaxAccel();
}


double
VehicleType::getDecel(const std::string& typeID) {
    return getVType(typeID)->getCarFollowModel().getMaxDecel();
}


double
VehicleType::getEmergencyDecel(const std::string& typeID) {
    return getVType(typeID)->getCarFollowModel().getEmergencyDecel();
}


double
VehicleType::getApparentDecel(const std::string& typeID) {
    return getVType(typeID)->getCarFollowModel().getApparentDecel();
}


double
VehicleType::getImperfection(const std::string& typeID) {
    return getVType(typeID)->getCarFollowModel().getImperfection();
}


double
VehicleType::getTau(const std::string& typeID) {
    return getVType(typeID)->getCarFollowModel().getHeadwayTime();
}


std::string
VehicleType::getVehicleClass(const std::string& typeID) {
    return toString(getVType(typeID)->getVehicleClass());
}


std::string
VehicleType::getEmissionClass(const std::string& typeID) {
    return PollutantsInterface::getName(getVType(typeID)->getEmissionClass());
}


std::string
VehicleType::getShapeClass(const std::string& typeID) {
    return getVehicleShapeName(getVType(typeID)->getGuiShape());
}


double
VehicleType::getMinGap(const std::string& typeID) {
    return getVType(typeID)->getMinGap();
}


double
VehicleType::getWidth(const std::string& typeID) {
    return getVType(typeID)->getWidth();
}


double
VehicleType::getHeight(const std::string& typeID) {
    return getVType(typeID)->getHeight();
}


double
VehicleType::getMass(const std::string& typeID) {
    return getVType(typeID)->getMass();
}


TraCIColor
VehicleType::getColor(const std::string& typeID) {
    return Helper::makeTraCIColor(getVType(typeID)->getColor());
}


double
VehicleType::getMinGapLat(const std::string& typeID) {
    return getVType(typeID)->getMinGapLat();
}


double
VehicleType::getMaxSpeedLat(const std::string& typeID) {
    return getVType(typeID)->getMaxSpeedLat();
}


std::string
VehicleType::getLateralAlignment(const std::string& typeID) {
    if (getVType(typeID)->getPreferredLateralAlignment() != LatAlignmentDefinition::GIVEN) {
        return toString(getVType(typeID)->getPreferredLateralAlignment());
    } else {
        return toString(getVType(typeID)->getPreferredLateralAlignmentOffset());
    }
}


std::string
VehicleType::getParameter(const std::string& typeID, const std::string& key) {
    if (StringUtils::startsWith(key, "junctionModel.")) {
        const std::string attrName = key.substr(14);
        if (!SUMOXMLDefinitions::Attrs.hasString(attrName)) {
            throw TraCIException("Invalid junctionModel parameter '" + key + "' for type '" + typeID + "'");
        }
        SumoXMLAttr attr = (SumoXMLAttr)SUMOXMLDefinitions::Attrs.get(attrName);
        if (SUMOVTypeParameter::AllowedJMAttrs.count(attr) == 0) {
            throw TraCIException("Invalid junctionModel parameter '" + key + "' for type '" + typeID + "'");
        }
        if (getVType(typeID)->getParameter().jmParameter.count(attr) != 0) {
            return getVType(typeID)->getParameter().jmParameter.find(attr)->second;
        } else {
            return "";
        }
    } else {
        return getVType(typeID)->getParameter().getParameter(key, "");
    }
}

LIBSUMO_GET_PARAMETER_WITH_KEY_IMPLEMENTATION(VehicleType)

int
VehicleType::getPersonCapacity(const std::string& typeID) {
    return getVType(typeID)->getPersonCapacity();
}

double
VehicleType::getScale(const std::string& typeID) {
    return getVType(typeID)->getParameter().scale;
}

double
VehicleType::getBoardingDuration(const std::string& typeID) {
    return STEPS2TIME(getVType(typeID)->getBoardingDuration(true));
}

double
VehicleType::getImpatience(const std::string& typeID) {
    return getVType(typeID)->getImpatience();
}

void
VehicleType::setLength(const std::string& typeID, double length)  {
    getVType(typeID)->setLength(length);
}


void
VehicleType::setMaxSpeed(const std::string& typeID, double speed)  {
    getVType(typeID)->setMaxSpeed(speed);
}


void
VehicleType::setActionStepLength(const std::string& typeID, double actionStepLength, bool resetActionOffset)  {
    getVType(typeID)->setActionStepLength(SUMOVehicleParserHelper::processActionStepLength(actionStepLength), resetActionOffset);
}


void
VehicleType::setBoardingDuration(const std::string& typeID, double boardingDuration)  {
    try {
        checkTimeBounds(boardingDuration);
    } catch (ProcessError&) {
        throw TraCIException("BoardingDuration parameter exceeds the time value range.");
    }
    getVType(typeID)->setBoardingDuration(TIME2STEPS(boardingDuration), true);
}


void
VehicleType::setImpatience(const std::string& typeID, double impatience)  {
    getVType(typeID)->setImpatience(impatience);
}


void
VehicleType::setVehicleClass(const std::string& typeID, const std::string& clazz)  {
    getVType(typeID)->setVClass(getVehicleClassID(clazz));
}


void
VehicleType::setSpeedFactor(const std::string& typeID, double factor)  {
    getVType(typeID)->setSpeedFactor(factor);
}


void
VehicleType::setSpeedDeviation(const std::string& typeID, double deviation)  {
    getVType(typeID)->setSpeedDeviation(deviation);
}


void
VehicleType::setEmissionClass(const std::string& typeID, const std::string& clazz)  {
    getVType(typeID)->setEmissionClass(PollutantsInterface::getClassByName(clazz));
}


void
VehicleType::setShapeClass(const std::string& typeID, const std::string& shapeClass)  {
    getVType(typeID)->setShape(getVehicleShapeID(shapeClass));
}


void
VehicleType::setWidth(const std::string& typeID, double width)  {
    getVType(typeID)->setWidth(width);
}


void
VehicleType::setHeight(const std::string& typeID, double height)  {
    getVType(typeID)->setHeight(height);
}


void
VehicleType::setMass(const std::string& typeID, double mass)  {
    getVType(typeID)->setMass(mass);
}


void
VehicleType::setMinGap(const std::string& typeID, double minGap)  {
    getVType(typeID)->setMinGap(minGap);
}


void
VehicleType::setAccel(const std::string& typeID, double accel)  {
    getVType(typeID)->setAccel(accel);
}


void
VehicleType::setDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->setDecel(decel);
    // automatically raise emergencyDecel to ensure it is at least as high as decel
    if (decel > v->getCarFollowModel().getEmergencyDecel()) {
        if (v->getParameter().cfParameter.count(SUMO_ATTR_EMERGENCYDECEL) > 0) {
            // notify user only if emergencyDecel was previously specified
            WRITE_WARNINGF(TL("Automatically setting emergencyDecel to % for vType '%' to match decel."), toString(decel), typeID);
        }
        v->setEmergencyDecel(decel);
    }
}


void
VehicleType::setEmergencyDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->setEmergencyDecel(decel);
    if (decel < v->getCarFollowModel().getMaxDecel()) {
        WRITE_WARNINGF(TL("New value of emergencyDecel (%) is lower than decel (%)"), toString(decel), toString(v->getCarFollowModel().getMaxDecel()));
    }
}


void
VehicleType::setApparentDecel(const std::string& typeID, double decel)  {
    getVType(typeID)->setApparentDecel(decel);
}


void
VehicleType::setImperfection(const std::string& typeID, double imperfection)  {
    getVType(typeID)->setImperfection(imperfection);
}


void
VehicleType::setTau(const std::string& typeID, double tau)  {
    getVType(typeID)->setTau(tau);
}


void
VehicleType::setColor(const std::string& typeID, const TraCIColor& c)  {
    getVType(typeID)->setColor(Helper::makeRGBColor(c));
}


void
VehicleType::setMinGapLat(const std::string& typeID, double minGapLat)  {
    getVType(typeID)->setMinGapLat(minGapLat);
}


void
VehicleType::setMaxSpeedLat(const std::string& typeID, double speed)  {
    getVType(typeID)->setMaxSpeedLat(speed);
}


void
VehicleType::setLateralAlignment(const std::string& typeID, const std::string& latAlignment)  {
    double lao;
    LatAlignmentDefinition lad;
    if (SUMOVTypeParameter::parseLatAlignment(latAlignment, lao, lad)) {
        getVType(typeID)->setPreferredLateralAlignment(lad, lao);
    } else {
        throw TraCIException("Unknown value '" + latAlignment + "' when setting latAlignment for vType '" + typeID + "';\n must be one of (\"right\", \"center\", \"arbitrary\", \"nice\", \"compact\", \"left\" or a float)");
    }
}

void
VehicleType::setScale(const std::string& typeID, double value)  {
    getVType(typeID)->setScale(value);
}

void
VehicleType::copy(const std::string& origTypeID, const std::string& newTypeID)  {
    getVType(origTypeID)->duplicateType(newTypeID, true);
}


void
VehicleType::setParameter(const std::string& typeID, const std::string& name, const std::string& value) {
    if (StringUtils::startsWith(name, "junctionModel.")) {
        const std::string attrName = name.substr(14);
        if (!SUMOXMLDefinitions::Attrs.hasString(attrName)) {
            throw TraCIException("Invalid junctionModel parameter '" + name + "' for type '" + typeID + "'");
        }
        SumoXMLAttr attr = (SumoXMLAttr)SUMOXMLDefinitions::Attrs.get(attrName);
        if (SUMOVTypeParameter::AllowedJMAttrs.count(attr) == 0) {
            throw TraCIException("Invalid junctionModel parameter '" + name + "' for type '" + typeID + "'");
        }
        try {
            StringUtils::toDouble(value); // check number format
            ((SUMOVTypeParameter&)getVType(typeID)->getParameter()).jmParameter[attr] = value;
        } catch (NumberFormatException&) {
            throw TraCIException("Invalid junctionModel parameter value '" + value + "' for type '" + typeID + " (should be numeric)'");
        }
    } else {
        ((SUMOVTypeParameter&)getVType(typeID)->getParameter()).setParameter(name, value);
    }
}


LIBSUMO_SUBSCRIPTION_IMPLEMENTATION(VehicleType, VEHICLETYPE)


MSVehicleType*
VehicleType::getVType(std::string id) {
    MSVehicleType* t = MSNet::getInstance()->getVehicleControl().getVType(id);
    if (t == nullptr) {
        throw TraCIException("Vehicle type '" + id + "' is not known");
    }
    return t;
}


std::shared_ptr<VariableWrapper>
VehicleType::makeWrapper() {
    return std::make_shared<Helper::SubscriptionWrapper>(handleVariable, mySubscriptionResults, myContextSubscriptionResults);
}


bool
VehicleType::handleVariable(const std::string& objID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    return handleVariableWithID(objID, objID, variable, wrapper, paramData);
}


bool
VehicleType::handleVariableWithID(const std::string& objID, const std::string& typeID, const int variable, VariableWrapper* wrapper, tcpip::Storage* paramData) {
    switch (variable) {
        case TRACI_ID_LIST:
            return wrapper->wrapStringList(objID, variable, getIDList());
        case ID_COUNT:
            return wrapper->wrapInt(objID, variable, getIDCount());
        case VAR_LENGTH:
            return wrapper->wrapDouble(objID, variable, getLength(typeID));
        case VAR_HEIGHT:
            return wrapper->wrapDouble(objID, variable, getHeight(typeID));
        case VAR_MASS:
            return wrapper->wrapDouble(objID, variable, getMass(typeID));
        case VAR_MINGAP:
            return wrapper->wrapDouble(objID, variable, getMinGap(typeID));
        case VAR_MAXSPEED:
            return wrapper->wrapDouble(objID, variable, getMaxSpeed(typeID));
        case VAR_ACCEL:
            return wrapper->wrapDouble(objID, variable, getAccel(typeID));
        case VAR_DECEL:
            return wrapper->wrapDouble(objID, variable, getDecel(typeID));
        case VAR_EMERGENCY_DECEL:
            return wrapper->wrapDouble(objID, variable, getEmergencyDecel(typeID));
        case VAR_APPARENT_DECEL:
            return wrapper->wrapDouble(objID, variable, getApparentDecel(typeID));
        case VAR_ACTIONSTEPLENGTH:
            return wrapper->wrapDouble(objID, variable, getActionStepLength(typeID));
        case VAR_IMPERFECTION:
            return wrapper->wrapDouble(objID, variable, getImperfection(typeID));
        case VAR_TAU:
            return wrapper->wrapDouble(objID, variable, getTau(typeID));
        case VAR_SPEED_FACTOR:
            return wrapper->wrapDouble(objID, variable, getSpeedFactor(typeID));
        case VAR_SPEED_DEVIATION:
            return wrapper->wrapDouble(objID, variable, getSpeedDeviation(typeID));
        case VAR_VEHICLECLASS:
            return wrapper->wrapString(objID, variable, getVehicleClass(typeID));
        case VAR_EMISSIONCLASS:
            return wrapper->wrapString(objID, variable, getEmissionClass(typeID));
        case VAR_SHAPECLASS:
            return wrapper->wrapString(objID, variable, getShapeClass(typeID));
        case VAR_WIDTH:
            return wrapper->wrapDouble(objID, variable, getWidth(typeID));
        case VAR_COLOR:
            return wrapper->wrapColor(objID, variable, getColor(typeID));
        case VAR_MINGAP_LAT:
            return wrapper->wrapDouble(objID, variable, getMinGapLat(typeID));
        case VAR_MAXSPEED_LAT:
            return wrapper->wrapDouble(objID, variable, getMaxSpeedLat(typeID));
        case VAR_LATALIGNMENT:
            return wrapper->wrapString(objID, variable, getLateralAlignment(typeID));
        case VAR_PERSON_CAPACITY:
            return wrapper->wrapInt(objID, variable, getPersonCapacity(typeID));
        case VAR_BOARDING_DURATION:
            return wrapper->wrapDouble(objID, variable, getBoardingDuration(typeID));
        case VAR_IMPATIENCE:
            return wrapper->wrapDouble(objID, variable, getImpatience(typeID));
        case VAR_SCALE:
            return wrapper->wrapDouble(objID, variable, getScale(typeID));
        case libsumo::VAR_PARAMETER:
            paramData->readUnsignedByte();
            return wrapper->wrapString(objID, variable, getParameter(objID, paramData->readString()));
        case libsumo::VAR_PARAMETER_WITH_KEY:
            paramData->readUnsignedByte();
            return wrapper->wrapStringPair(objID, variable, getParameterWithKey(objID, paramData->readString()));
        default:
            return false;
    }
}

}


/****************************************************************************/
