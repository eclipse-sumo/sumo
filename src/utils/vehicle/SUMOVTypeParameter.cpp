/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2019 German Aerospace Center (DLR) and others.
// This program and the accompanying materials
// are made available under the terms of the Eclipse Public License v2.0
// which accompanies this distribution, and is available at
// http://www.eclipse.org/legal/epl-v20.html
// SPDX-License-Identifier: EPL-2.0
/****************************************************************************/
/// @file    SUMOVTypeParameter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    10.09.2009
/// @version $Id$
///
// Structure representing possible vehicle parameter
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#include <config.h>

#include <algorithm>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/common/ToString.h>
#include <utils/common/StringUtils.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/emissions/PollutantsInterface.h>

#define EMPREFIX std::string("HBEFA3/")


// ===========================================================================
// member method definitions
// ===========================================================================

SUMOVTypeParameter::VClassDefaultValues::VClassDefaultValues(SUMOVehicleClass vclass) :
    length(5./*4.3*/),
    minGap(2.5),
    maxSpeed(200. / 3.6),
    width(1.8),
    height(1.5),
    shape(SVS_UNKNOWN),
    emissionClass(PollutantsInterface::getClassByName(EMPREFIX + "PC_G_EU4", vclass)),
    speedFactor("normc", 1.0, 0.0, 0.2, 2.0),
    personCapacity(4),
    containerCapacity(0),
    osgFile("car-normal-citrus.obj"),
    carriageLength(-1),
    locomotiveLength(-1) {
    // update default values
    switch (vclass) {
        case SVC_PEDESTRIAN:
            length = 0.215;
            minGap = 0.25;
            maxSpeed = DEFAULT_PEDESTRIAN_SPEED;
            width = 0.478;
            height = 1.719;
            shape = SVS_PEDESTRIAN;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            speedFactor.getParameter()[1] = 0.1;
            break;
        case SVC_BICYCLE:
            length = 1.6;
            minGap = 0.5;
            maxSpeed = 20. / 3.6;
            width = 0.65;
            height = 1.7;
            shape = SVS_BICYCLE;
            personCapacity = 1;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            speedFactor.getParameter()[1] = 0.1;
            break;
        case SVC_MOPED:
            length = 2.1;
            maxSpeed = 60. / 3.6;
            width = 0.8;
            height = 1.7;
            shape = SVS_MOPED;
            personCapacity = 1;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "LDV_G_EU6", vclass);
            speedFactor.getParameter()[1] = 0.1;
            break;
        case SVC_MOTORCYCLE:
            length = 2.2;
            width = 0.9;
            height = 1.5;
            shape = SVS_MOTORCYCLE;
            personCapacity = 1;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "LDV_G_EU6", vclass);
            speedFactor.getParameter()[1] = 0.1;
            break;
        case SVC_TRUCK:
            length = 7.1;
            maxSpeed = 130. / 3.6;
            width = 2.4;
            height = 2.4;
            shape = SVS_TRUCK;
            osgFile = "car-microcargo-citrus.obj";
            personCapacity = 2;
            containerCapacity = 1;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "HDV", vclass);
            speedFactor.getParameter()[1] = 0.05;
            break;
        case SVC_TRAILER:
            length = 16.5;
            maxSpeed = 130. / 3.6;
            width = 2.55;
            height = 4.;
            shape = SVS_TRUCK_1TRAILER;
            osgFile = "car-microcargo-citrus.obj";
            personCapacity = 2;
            containerCapacity = 2;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "HDV", vclass);
            speedFactor.getParameter()[1] = 0.05;
            break;
        case SVC_BUS:
            length = 12.;
            maxSpeed = 100. / 3.6;
            width = 2.5;
            height = 3.4;
            shape = SVS_BUS;
            osgFile = "car-minibus-citrus.obj";
            personCapacity = 85;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "Bus", vclass);
            break;
        case SVC_COACH:
            length = 14.;
            maxSpeed = 100. / 3.6;
            width = 2.6;
            height = 4.;
            shape = SVS_BUS_COACH;
            osgFile = "car-minibus-citrus.obj";
            personCapacity = 70;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "Coach", vclass);
            speedFactor.getParameter()[1] = 0.05;
            break;
        case SVC_TRAM:
            length = 22.;
            maxSpeed = 80. / 3.6;
            width = 2.4;
            height = 3.2;
            shape = SVS_RAIL_CAR;
            carriageLength = 5.71; // http://de.wikipedia.org/wiki/Bombardier_Flexity_Berlin
            locomotiveLength = 5.71;
            personCapacity = 120;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            break;
        case SVC_RAIL_URBAN:
            length = 36.5 * 3;
            maxSpeed = 100. / 3.6;
            width = 3.0;
            height = 3.6;
            shape = SVS_RAIL_CAR;
            carriageLength = 18.4;  // https://en.wikipedia.org/wiki/DBAG_Class_481
            locomotiveLength = 18.4;
            personCapacity = 300;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            break;
        case SVC_RAIL:
            length = 67.5 * 2;
            maxSpeed = 160. / 3.6;
            width = 2.84;
            height = 3.75;
            shape = SVS_RAIL;
            carriageLength = 24.5; // http://de.wikipedia.org/wiki/UIC-Y-Wagen_%28DR%29
            locomotiveLength = 16.4; // https://en.wikipedia.org/wiki/DB_Class_218
            personCapacity = 434;
            // slight understatement (-:
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "HDV_D_EU0", vclass);
            break;
        case SVC_RAIL_ELECTRIC:
            length = 25. * 8;
            maxSpeed = 220. / 3.6;
            width = 2.95;
            height = 3.89;
            shape = SVS_RAIL;
            carriageLength = 24.775;
            locomotiveLength = 19.100; // https://en.wikipedia.org/wiki/DB_Class_101
            personCapacity = 425;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            break;
        case SVC_RAIL_FAST:
            length = 25. * 8;
            maxSpeed = 330. / 3.6;
            width = 2.95;
            height = 3.89;
            shape = SVS_RAIL;
            carriageLength = 24.775; // http://de.wikipedia.org/wiki/ICE_3
            locomotiveLength = 25.835;
            personCapacity = 425;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            break;
        case SVC_DELIVERY:
            length = 6.5;
            width = 2.16;
            height = 2.86;
            shape = SVS_DELIVERY;
            personCapacity = 2;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "LDV", vclass);
            speedFactor.getParameter()[1] = 0.05;
            break;
        case SVC_EMERGENCY:
            length = 6.5;
            width = 2.16;
            height = 2.86;
            shape = SVS_DELIVERY;
            personCapacity = 2;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "LDV", vclass);
            break;
        case SVC_PRIVATE:
        case SVC_VIP:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            shape = SVS_PASSENGER;
            speedFactor.getParameter()[1] = 0.1;
            break;
        case SVC_TAXI:
            shape = SVS_PASSENGER;
            speedFactor.getParameter()[1] = 0.05;
            break;
        case SVC_E_VEHICLE:
            shape = SVS_E_VEHICLE;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            speedFactor.getParameter()[1] = 0.1;
            break;
        case SVC_SHIP:
            length = 17;
            width = 4;
            maxSpeed = 8 / 1.94; // 8 knots
            height = 4;
            shape = SVS_SHIP;
            // slight understatement (-:
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "HDV_D_EU0", vclass);
            speedFactor.getParameter()[1] = 0.1;
            break;
        default:
            break;
    }
}


SUMOVTypeParameter::VClassDefaultValues::VClassDefaultValues() :
    speedFactor("normc", 1.0, 0.0, 0.2, 2.0) {}


SUMOVTypeParameter::SUMOVTypeParameter(const std::string& vtid, const SUMOVehicleClass vclass) :
    id(vtid), length(5./*4.3*/), minGap(2.5), maxSpeed(200. / 3.6),
    actionStepLength(0), defaultProbability(DEFAULT_VEH_PROB),
    speedFactor("normc", 1.0, 0.0, 0.2, 2.0),
    emissionClass(PollutantsInterface::getClassByName(EMPREFIX + "PC_G_EU4", vclass)), color(RGBColor::DEFAULT_COLOR),
    vehicleClass(vclass), impatience(0.0), personCapacity(4), containerCapacity(0), boardingDuration(500),
    loadingDuration(90000), width(1.8), height(1.5), shape(SVS_UNKNOWN), osgFile("car-normal-citrus.obj"),
    cfModel(SUMO_TAG_CF_KRAUSS),
    hasDriverState(false), lcModel(LCM_DEFAULT),
    maxSpeedLat(1.0), latAlignment(LATALIGN_CENTER), minGapLat(0.6),
    carriageLength(-1), locomotiveLength(-1), carriageGap(1),
    parametersSet(0), saved(false), onlyReferenced(false) {
    const OptionsCont& oc = OptionsCont::getOptions();
    if (oc.exists("carfollow.model")) {
        // check for valid value has been performed in MSFrame
        cfModel = SUMOXMLDefinitions::CarFollowModels.get(oc.getString("carfollow.model"));
    }
    // obtain default values depending of vclass
    VClassDefaultValues defaultValues(vclass);
    // overwritte SUMOVTypeParameter with VClassDefaultValues
    length = defaultValues.length;
    minGap = defaultValues.minGap;
    maxSpeed = defaultValues.maxSpeed;
    width = defaultValues.width;
    height = defaultValues.height;
    shape = defaultValues.shape;
    emissionClass = defaultValues.emissionClass;
    speedFactor = defaultValues.speedFactor;
    personCapacity = defaultValues.personCapacity;
    containerCapacity = defaultValues.containerCapacity;
    osgFile = defaultValues.osgFile;
    carriageLength = defaultValues.carriageLength;
    locomotiveLength = defaultValues.locomotiveLength;
    // check if default speeddev was defined
    if (oc.exists("default.speeddev")) {
        const double defaultSpeedDev = oc.getFloat("default.speeddev");
        if (defaultSpeedDev >= 0) {
            speedFactor.getParameter()[1] = defaultSpeedDev;
        }
    } else {
        speedFactor.getParameter()[1] = 0;
    }
}


void
SUMOVTypeParameter::write(OutputDevice& dev) const {
    if (onlyReferenced) {
        return;
    }
    dev.openTag(SUMO_TAG_VTYPE);
    dev.writeAttr(SUMO_ATTR_ID, id);
    if (wasSet(VTYPEPARS_LENGTH_SET)) {
        dev.writeAttr(SUMO_ATTR_LENGTH, length);
    }
    if (wasSet(VTYPEPARS_MINGAP_SET)) {
        dev.writeAttr(SUMO_ATTR_MINGAP, minGap);
    }
    if (wasSet(VTYPEPARS_MAXSPEED_SET)) {
        dev.writeAttr(SUMO_ATTR_MAXSPEED, maxSpeed);
    }
    if (wasSet(VTYPEPARS_PROBABILITY_SET)) {
        dev.writeAttr(SUMO_ATTR_PROB, defaultProbability);
    }
    if (wasSet(VTYPEPARS_SPEEDFACTOR_SET)) {
        dev.writeAttr(SUMO_ATTR_SPEEDFACTOR, speedFactor);
    }
    if (wasSet(VTYPEPARS_ACTIONSTEPLENGTH_SET)) {
        // Note: action step length is only exposed in seconds to the user
        dev.writeAttr(SUMO_ATTR_ACTIONSTEPLENGTH, STEPS2TIME(actionStepLength));
    }
    if (wasSet(VTYPEPARS_HASDRIVERSTATE_SET)) {
        dev.writeAttr(SUMO_ATTR_HASDRIVERSTATE, hasDriverState);
    }
    if (wasSet(VTYPEPARS_VEHICLECLASS_SET)) {
        dev.writeAttr(SUMO_ATTR_VCLASS, toString(vehicleClass));
    }
    if (wasSet(VTYPEPARS_EMISSIONCLASS_SET)) {
        dev.writeAttr(SUMO_ATTR_EMISSIONCLASS, PollutantsInterface::getName(emissionClass));
    }
    if (wasSet(VTYPEPARS_IMPATIENCE_SET)) {
        if (impatience == -std::numeric_limits<double>::max()) {
            dev.writeAttr(SUMO_ATTR_IMPATIENCE, "off");
        } else {
            dev.writeAttr(SUMO_ATTR_IMPATIENCE, impatience);
        }
    }
    if (wasSet(VTYPEPARS_SHAPE_SET)) {
        dev.writeAttr(SUMO_ATTR_GUISHAPE, getVehicleShapeName(shape));
    }
    if (wasSet(VTYPEPARS_WIDTH_SET)) {
        dev.writeAttr(SUMO_ATTR_WIDTH, width);
    }
    if (wasSet(VTYPEPARS_HEIGHT_SET)) {
        dev.writeAttr(SUMO_ATTR_HEIGHT, height);
    }
    if (wasSet(VTYPEPARS_COLOR_SET)) {
        dev.writeAttr(SUMO_ATTR_COLOR, color);
    }
    if (wasSet(VTYPEPARS_OSGFILE_SET)) {
        dev.writeAttr(SUMO_ATTR_OSGFILE, osgFile);
    }
    if (wasSet(VTYPEPARS_IMGFILE_SET)) {
        dev.writeAttr(SUMO_ATTR_IMGFILE, imgFile);
    }
    if (wasSet(VTYPEPARS_PERSON_CAPACITY)) {
        dev.writeAttr(SUMO_ATTR_PERSON_CAPACITY, personCapacity);
    }
    if (wasSet(VTYPEPARS_CONTAINER_CAPACITY)) {
        dev.writeAttr(SUMO_ATTR_CONTAINER_CAPACITY, containerCapacity);
    }
    if (wasSet(VTYPEPARS_BOARDING_DURATION)) {
        dev.writeAttr(SUMO_ATTR_BOARDING_DURATION, boardingDuration);
    }
    if (wasSet(VTYPEPARS_LOADING_DURATION)) {
        dev.writeAttr(SUMO_ATTR_LOADING_DURATION, loadingDuration);
    }
    if (wasSet(VTYPEPARS_MAXSPEED_LAT_SET)) {
        dev.writeAttr(SUMO_ATTR_MAXSPEED_LAT, maxSpeedLat);
    }
    if (wasSet(VTYPEPARS_LATALIGNMENT_SET)) {
        dev.writeAttr(SUMO_ATTR_LATALIGNMENT, latAlignment);
    }
    if (wasSet(VTYPEPARS_MINGAP_LAT_SET)) {
        dev.writeAttr(SUMO_ATTR_MINGAP_LAT, minGapLat);
    }
    if (wasSet(VTYPEPARS_LANE_CHANGE_MODEL_SET)) {
        dev.writeAttr(SUMO_ATTR_LANE_CHANGE_MODEL, lcModel);
    }
    // Write Lane Change Model parameters
    for (const auto& i : lcParameter) {
        dev.writeAttr(i.first, i.second);
    }
    // Write Junction Model parameter
    for (const auto& i : jmParameter) {
        dev.writeAttr(i.first, i.second);
    }
    if (wasSet(VTYPEPARS_CAR_FOLLOW_MODEL)) {
        dev.writeAttr(SUMO_ATTR_CAR_FOLLOW_MODEL, SUMOXMLDefinitions::CarFollowModels.getString(cfModel));
    }
    // Write Car Following Model parameters
    for (const auto& i : cfParameter) {
        dev.writeAttr(i.first, i.second);
    }
    // Write rest of parameters
    writeParams(dev);
    dev.closeTag();
}


double
SUMOVTypeParameter::getCFParam(const SumoXMLAttr attr, const double defaultValue) const {
    if (cfParameter.count(attr)) {
        return StringUtils::toDouble(cfParameter.find(attr)->second);
    } else {
        return defaultValue;
    }
}


std::string
SUMOVTypeParameter::getCFParamString(const SumoXMLAttr attr, const std::string defaultValue) const {
    if (cfParameter.count(attr)) {
        return cfParameter.find(attr)->second;
    } else {
        return defaultValue;
    }
}


double
SUMOVTypeParameter::getLCParam(const SumoXMLAttr attr, const double defaultValue) const {
    if (lcParameter.count(attr)) {
        return StringUtils::toDouble(lcParameter.find(attr)->second);
    } else {
        return defaultValue;
    }
}


std::string
SUMOVTypeParameter::getLCParamString(const SumoXMLAttr attr, const std::string& defaultValue) const {
    if (lcParameter.count(attr)) {
        return lcParameter.find(attr)->second;
    } else {
        return defaultValue;
    }
}


const SUMOVTypeParameter::SubParams&
SUMOVTypeParameter::getLCParams() const {
    return lcParameter;
}


double
SUMOVTypeParameter::getJMParam(const SumoXMLAttr attr, const double defaultValue) const {
    if (jmParameter.count(attr)) {
        return StringUtils::toDouble(jmParameter.find(attr)->second);
    } else {
        return defaultValue;
    }
}


std::string
SUMOVTypeParameter::getJMParamString(const SumoXMLAttr attr, const std::string defaultValue) const {
    if (jmParameter.count(attr)) {
        return jmParameter.find(attr)->second;
    } else {
        return defaultValue;
    }
}


double
SUMOVTypeParameter::getDefaultAccel(const SUMOVehicleClass vc) {
    switch (vc) {
        case SVC_PEDESTRIAN:
            return 1.5;
        case SVC_BICYCLE:
            return 1.2;
        case SVC_MOTORCYCLE:
            return 6.;
        case SVC_MOPED:
            return 1.1;
        case SVC_TRUCK:
            return 1.3;
        case SVC_TRAILER:
            return 1.1;
        case SVC_BUS:
            return 1.2;
        case SVC_COACH:
            return 2.;
        case SVC_TRAM:
            return 1.;
        case SVC_RAIL_URBAN:
            return 1.;
        case SVC_RAIL:
            return 0.25;
        case SVC_RAIL_ELECTRIC:
        case SVC_RAIL_FAST:
            return 0.5;
        case SVC_SHIP:
            return 0.1;
        default:
            return 2.6;//2.9;
    }
}


double
SUMOVTypeParameter::getDefaultDecel(const SUMOVehicleClass vc) {
    switch (vc) {
        case SVC_PEDESTRIAN:
            return 2.;
        case SVC_BICYCLE:
            return 3.;
        case SVC_MOPED:
            return 7.;
        case SVC_MOTORCYCLE:
            return 10.;
        case SVC_TRUCK:
        case SVC_TRAILER:
        case SVC_BUS:
        case SVC_COACH:
            return 4.;
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
            return 3.;
        case SVC_RAIL:
        case SVC_RAIL_ELECTRIC:
        case SVC_RAIL_FAST:
            return 1.3;
        case SVC_SHIP:
            return 0.15;
        default:
            return 4.5;//7.5;
    }
}


double
SUMOVTypeParameter::getDefaultEmergencyDecel(const SUMOVehicleClass vc, double decel, double defaultOption) {
    if (defaultOption == VTYPEPARS_DEFAULT_EMERGENCYDECEL_DEFAULT) {
        double vcDecel;
        switch (vc) {
            case SVC_PEDESTRIAN:
                vcDecel = 5.;
                break;
            case SVC_BICYCLE:
                vcDecel = 7.;
                break;
            case SVC_MOPED:
            case SVC_MOTORCYCLE:
                vcDecel = 10.;
                break;
            case SVC_TRUCK:
            case SVC_TRAILER:
            case SVC_BUS:
            case SVC_COACH:
            case SVC_TRAM:
            case SVC_RAIL_URBAN:
                vcDecel = 7.;
                break;
            case SVC_RAIL:
            case SVC_RAIL_ELECTRIC:
            case SVC_RAIL_FAST:
                vcDecel = 5.;
                break;
            case SVC_SHIP:
                vcDecel = 1.;
                break;
            default:
                vcDecel = 9.;
        }
        return MAX2(decel, vcDecel);
    } else if (defaultOption == VTYPEPARS_DEFAULT_EMERGENCYDECEL_DECEL) {
        return decel;
    } else {
        // value already checked in MSFrame::checkOptions
        return MAX2(decel, defaultOption);
    }
}



double
SUMOVTypeParameter::getDefaultImperfection(const SUMOVehicleClass vc) {
    switch (vc) {
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
        case SVC_RAIL:
        case SVC_RAIL_ELECTRIC:
        case SVC_RAIL_FAST:
        case SVC_SHIP:
            return 0.;
        default:
            return 0.5;
    }
}


const SUMOVTypeParameter&
SUMOVTypeParameter::getDefault() {
    static SUMOVTypeParameter defaultParams("");
    return defaultParams;
}


/****************************************************************************/
