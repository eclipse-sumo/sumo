/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2017-2018 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    VehicleType.cpp
/// @author  Gregor Laemmel
/// @date    04.04.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <microsim/MSNet.h>
#include <microsim/MSVehicleControl.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/xml/SUMOVehicleParserHelper.h>
#include "VehicleType.h"
#include "Helper.h"


// ===========================================================================
// method definitions
// ===========================================================================
namespace libsumo {
std::vector<std::string> VehicleType::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getVehicleControl().insertVTypeIDs(ids);
    return ids;
}


double VehicleType::getLength(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getLength();
}


double VehicleType::getMaxSpeed(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMaxSpeed();
}


double VehicleType::getActionStepLength(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getActionStepLengthSecs();
}


double VehicleType::getSpeedFactor(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getSpeedFactor().getParameter()[0];
}


double VehicleType::getSpeedDeviation(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getSpeedFactor().getParameter()[1];
}


double VehicleType::getAccel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getMaxAccel();
}


double VehicleType::getDecel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getMaxDecel();
}


double VehicleType::getEmergencyDecel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getEmergencyDecel();
}


double VehicleType::getApparentDecel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getApparentDecel();
}


double VehicleType::getImperfection(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getImperfection();
}


double VehicleType::getTau(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getHeadwayTime();
}


std::string VehicleType::getVehicleClass(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return toString(v->getVehicleClass());
}


std::string VehicleType::getEmissionClass(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return PollutantsInterface::getName(v->getEmissionClass());
}


std::string VehicleType::getShapeClass(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return getVehicleShapeName(v->getGuiShape());
}


double VehicleType::getMinGap(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMinGap();
}


double VehicleType::getWidth(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getWidth();
}


double VehicleType::getHeight(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getHeight();
}


TraCIColor VehicleType::getColor(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return Helper::makeTraCIColor(v->getColor());
}


double VehicleType::getMinGapLat(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMinGapLat();
}


double VehicleType::getMaxSpeedLat(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMaxSpeedLat();
}


std::string VehicleType::getLateralAlignment(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return toString(v->getPreferredLateralAlignment());
}


std::string
VehicleType::getParameter(const std::string& typeID, const std::string& key) {
    MSVehicleType* v = getVType(typeID);
    return v->getParameter().getParameter(key, "");
}


void VehicleType::setLength(const std::string& typeID, double length)  {
    MSVehicleType* v = getVType(typeID);
    v->setLength(length);
}


void VehicleType::setMaxSpeed(const std::string& typeID, double speed)  {
    MSVehicleType* v = getVType(typeID);
    v->setMaxSpeed(speed);
}


void VehicleType::setActionStepLength(const std::string& typeID, double actionStepLength, bool resetActionOffset)  {
    MSVehicleType* v = getVType(typeID);
    v->setActionStepLength(SUMOVehicleParserHelper::processActionStepLength(actionStepLength), resetActionOffset);
}


void VehicleType::setVehicleClass(const std::string& typeID, const std::string& clazz)  {
    MSVehicleType* v = getVType(typeID);
    v->setVClass(getVehicleClassID(clazz));
}


void VehicleType::setSpeedFactor(const std::string& typeID, double factor)  {
    MSVehicleType* v = getVType(typeID);
    v->setSpeedFactor(factor);
}


void VehicleType::setSpeedDeviation(const std::string& typeID, double deviation)  {
    MSVehicleType* v = getVType(typeID);
    v->setSpeedDeviation(deviation);
}


void VehicleType::setEmissionClass(const std::string& typeID, const std::string& clazz)  {
    MSVehicleType* v = getVType(typeID);
    v->setEmissionClass(PollutantsInterface::getClassByName(clazz));
}


void VehicleType::setShapeClass(const std::string& typeID, const std::string& shapeClass)  {
    MSVehicleType* v = getVType(typeID);
    v->setShape(getVehicleShapeID(shapeClass));
}


void VehicleType::setWidth(const std::string& typeID, double width)  {
    MSVehicleType* v = getVType(typeID);
    v->setWidth(width);
}


void VehicleType::setHeight(const std::string& typeID, double height)  {
    MSVehicleType* v = getVType(typeID);
    v->setHeight(height);
}


void VehicleType::setMinGap(const std::string& typeID, double minGap)  {
    MSVehicleType* v = getVType(typeID);
    v->setMinGap(minGap);
}


void VehicleType::setAccel(const std::string& typeID, double accel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setMaxAccel(accel);
}


void VehicleType::setDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setMaxDecel(decel);
    // automatically raise emergencyDecel to ensure it is at least as high as decel
    if (decel > v->getCarFollowModel().getEmergencyDecel()) {
        if (v->getParameter().cfParameter.count(SUMO_ATTR_EMERGENCYDECEL) > 0) {
            // notify user only if emergencyDecel was previously specified
            WRITE_WARNING("Automatically setting emergencyDecel to " + toString(decel) + " for vType '" + typeID + "' to match decel.");
        }
        v->getCarFollowModel().setEmergencyDecel(decel);
    }
}


void VehicleType::setEmergencyDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setEmergencyDecel(decel);
    if (decel < v->getCarFollowModel().getMaxDecel()) {
        WRITE_WARNING("New value of emergencyDecel (" + toString(decel) + ") is lower than decel (" + toString(v->getCarFollowModel().getMaxDecel()) + ")");
    }
}


void VehicleType::setApparentDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setApparentDecel(decel);
}


void VehicleType::setImperfection(const std::string& typeID, double imperfection)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setImperfection(imperfection);
}


void VehicleType::setTau(const std::string& typeID, double tau)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setHeadwayTime(tau);
}


void VehicleType::setColor(const std::string& typeID, const TraCIColor& c)  {
    MSVehicleType* v = getVType(typeID);
    v->setColor(Helper::makeRGBColor(c));

}


void VehicleType::setMinGapLat(const std::string& typeID, double minGapLat)  {
    MSVehicleType* v = getVType(typeID);
    v->setMinGapLat(minGapLat);
}


void VehicleType::setMaxSpeedLat(const std::string& typeID, double speed)  {
    MSVehicleType* v = getVType(typeID);
    v->setMaxSpeedLat(speed);
}


void VehicleType::setLateralAlignment(const std::string& typeID, const std::string& latAlignment)  {
    MSVehicleType* v = getVType(typeID);
    v->setPreferredLateralAlignment(SUMOXMLDefinitions::LateralAlignments.get(latAlignment));
}


void VehicleType::copy(const std::string& origTypeID, const std::string& newTypeID)  {
    getVType(origTypeID)->duplicateType(newTypeID, true);
}


void VehicleType::setParameter(const std::string& typeID, const std::string& name, const std::string& value) {
    MSVehicleType* v = getVType(typeID);
    ((SUMOVTypeParameter&)v->getParameter()).setParameter(name, value);
}


MSVehicleType* VehicleType::getVType(std::string id) {
    MSVehicleType* t = MSNet::getInstance()->getVehicleControl().getVType(id);
    if (t == 0) {
        throw TraCIException("Vehicle type '" + id + "' is not known");
    }
    return t;
}
}


/****************************************************************************/
