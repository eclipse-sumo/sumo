/****************************************************************************/
/// @file    TraCI_VehicleType.cpp
/// @author  Gregor Laemmel
/// @date    04.04.2017
/// @version $Id$
///
// C++ TraCI client API implementation
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2017 - 2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/

#include <microsim/MSNet.h>
#include <utils/emissions/PollutantsInterface.h>
#include "TraCI_VehicleType.h"
#include "TraCI.h"
std::vector<std::string> TraCI_VehicleType::getIDList() {
    std::vector<std::string> ids;
    MSNet::getInstance()->getVehicleControl().insertVTypeIDs(ids);
    return ids;
}
double TraCI_VehicleType::getLength(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getLength();
}
double TraCI_VehicleType::getMaxSpeed(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMaxSpeed();
}
double TraCI_VehicleType::getSpeedFactor(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getSpeedFactor().getParameter()[0];
}
double TraCI_VehicleType::getSpeedDeviation(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getSpeedFactor().getParameter()[1];
}
double TraCI_VehicleType::getAccel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getMaxAccel();
}
double TraCI_VehicleType::getDecel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getMaxDecel();
}
double TraCI_VehicleType::getEmergencyDecel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getEmergencyDecel();
}
double TraCI_VehicleType::getApparentDecel(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getApparentDecel();
}
double TraCI_VehicleType::getImperfection(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getImperfection();
}
double TraCI_VehicleType::getTau(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getCarFollowModel().getHeadwayTime();
}
std::string TraCI_VehicleType::getVehicleClass(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return toString(v->getVehicleClass());
}
std::string TraCI_VehicleType::getEmissionClass(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return PollutantsInterface::getName(v->getEmissionClass());
}
std::string TraCI_VehicleType::getShapeClass(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return getVehicleShapeName(v->getGuiShape());
}
double TraCI_VehicleType::getMinGap(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMinGap();
}
double TraCI_VehicleType::getWidth(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getWidth();
}
double TraCI_VehicleType::getHeight(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getHeight();
}
TraCIColor TraCI_VehicleType::getColor(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return TraCI::makeTraCIColor(v->getColor());
}
double TraCI_VehicleType::getMinGapLat(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMinGapLat();
}
double TraCI_VehicleType::getMaxSpeedLat(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return v->getMaxSpeedLat();
}
std::string TraCI_VehicleType::getLateralAlignment(const std::string& typeID) {
    MSVehicleType* v = getVType(typeID);
    return toString(v->getPreferredLateralAlignment());
}


std::string
TraCI_VehicleType::getParameter(const std::string& typeID, const std::string& key) {
    MSVehicleType* v = getVType(typeID);
    return v->getParameter().getParameter(key, "");
}


void TraCI_VehicleType::setLength(const std::string& typeID, double length)  {
    MSVehicleType* v = getVType(typeID);
    v->setLength(length);
}
void TraCI_VehicleType::setMaxSpeed(const std::string& typeID, double speed)  {
    MSVehicleType* v = getVType(typeID);
    v->setMaxSpeed(speed);
}
void TraCI_VehicleType::setVehicleClass(const std::string& typeID, const std::string& clazz)  {
    MSVehicleType* v = getVType(typeID);
    v->setVClass(getVehicleClassID(clazz));
}
void TraCI_VehicleType::setSpeedFactor(const std::string& typeID, double factor)  {
    MSVehicleType* v = getVType(typeID);
    v->setSpeedFactor(factor);
}
void TraCI_VehicleType::setSpeedDeviation(const std::string& typeID, double deviation)  {
    MSVehicleType* v = getVType(typeID);
    v->setSpeedDeviation(deviation);
}
void TraCI_VehicleType::setEmissionClass(const std::string& typeID, const std::string& clazz)  {
    MSVehicleType* v = getVType(typeID);
    v->setEmissionClass(PollutantsInterface::getClassByName(clazz));
}
void TraCI_VehicleType::setShapeClass(const std::string& typeID, const std::string& shapeClass)  {
    MSVehicleType* v = getVType(typeID);
    v->setShape(getVehicleShapeID(shapeClass));
}
void TraCI_VehicleType::setWidth(const std::string& typeID, double width)  {
    MSVehicleType* v = getVType(typeID);
    v->setWidth(width);
}
void TraCI_VehicleType::setHeight(const std::string& typeID, double height)  {
    MSVehicleType* v = getVType(typeID);
    v->setHeight(height);
}
void TraCI_VehicleType::setMinGap(const std::string& typeID, double minGap)  {
    MSVehicleType* v = getVType(typeID);
    v->setMinGap(minGap);
}
void TraCI_VehicleType::setAccel(const std::string& typeID, double accel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setMaxAccel(accel);
}
void TraCI_VehicleType::setDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setMaxDecel(decel);
}
void TraCI_VehicleType::setEmergencyDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setEmergencyDecel(decel);
}
void TraCI_VehicleType::setApparentDecel(const std::string& typeID, double decel)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setApparentDecel(decel);
}
void TraCI_VehicleType::setImperfection(const std::string& typeID, double imperfection)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setImperfection(imperfection);
}
void TraCI_VehicleType::setTau(const std::string& typeID, double tau)  {
    MSVehicleType* v = getVType(typeID);
    v->getCarFollowModel().setHeadwayTime(tau);
}
void TraCI_VehicleType::setColor(const std::string& typeID, const TraCIColor& c)  {
    MSVehicleType* v = getVType(typeID);
    v->setColor(TraCI::makeRGBColor(c));

}
void TraCI_VehicleType::setMinGapLat(const std::string& typeID, double minGapLat)  {
    MSVehicleType* v = getVType(typeID);
    v->setMinGapLat(minGapLat);
}
void TraCI_VehicleType::setMaxSpeedLat(const std::string& typeID, double speed)  {
    MSVehicleType* v = getVType(typeID);
    v->setMaxSpeedLat(speed);
}
void TraCI_VehicleType::setLateralAlignment(const std::string& typeID, const std::string& latAlignment)  {
    MSVehicleType* v = getVType(typeID);
    v->setPreferredLateralAlignment(SUMOXMLDefinitions::LateralAlignments.get(latAlignment));
}

void TraCI_VehicleType::addParameter(const std::string& typeID, const std::string& name, const std::string& value) {
    MSVehicleType* v = getVType(typeID);
    ((SUMOVTypeParameter&) v->getParameter()).addParameter(name, value);
}
MSVehicleType* TraCI_VehicleType::getVType(std::string id) {
    MSVehicleType* t = MSNet::getInstance()->getVehicleControl().getVType(id);
    if (t == 0) {
        throw TraCIException("Vehicle type '" + id + "' is not known");
    }
    return t;
}
