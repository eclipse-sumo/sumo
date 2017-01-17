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
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2001-2017 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This file is part of SUMO.
//   SUMO is free software: you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation, either version 3 of the License, or
//   (at your option) any later version.
//
/****************************************************************************/


// ===========================================================================
// included modules
// ===========================================================================
#ifdef _MSC_VER
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <algorithm>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/xml/SUMOXMLDefinitions.h>
#include <utils/emissions/PollutantsInterface.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS

#define EMPREFIX std::string("HBEFA3/")


// ===========================================================================
// member method definitions
// ===========================================================================
SUMOVTypeParameter::SUMOVTypeParameter(const std::string& vtid, const SUMOVehicleClass vclass)
    : id(vtid), length(5./*4.3*/), minGap(2.5), maxSpeed(200. / 3.6),
      defaultProbability(DEFAULT_VEH_PROB),
      speedFactor(1.0), speedDev(0.0),
      emissionClass(PollutantsInterface::getClassByName(EMPREFIX + "PC_G_EU4", vclass)), color(RGBColor::DEFAULT_COLOR),
      vehicleClass(vclass), impatience(0.0), personCapacity(4), containerCapacity(0), boardingDuration(500),
      loadingDuration(90000), width(1.8), height(1.5), shape(SVS_UNKNOWN), osgFile("car-normal-citrus.obj"),
      cfModel(SUMO_TAG_CF_KRAUSS), lcModel(LCM_DEFAULT),
      maxSpeedLat(1.0), latAlignment(LATALIGN_CENTER), minGapLat(0.12),
      setParameter(0), saved(false), onlyReferenced(false) {
    switch (vclass) {
        case SVC_PEDESTRIAN:
            length = 0.215;
            minGap = 0.25;
            maxSpeed = DEFAULT_PEDESTRIAN_SPEED;
            width = 0.478;
            height = 1.719;
            shape = SVS_PEDESTRIAN;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
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
            break;
        case SVC_MOPED:
            length = 2.1;
            maxSpeed = 60. / 3.6;
            width = 0.8;
            height = 1.7;
            shape = SVS_MOPED;
            personCapacity = 1;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "LDV_G_EU6", vclass);
            break;
        case SVC_MOTORCYCLE:
            length = 2.2;
            width = 0.9;
            height = 1.5;
            shape = SVS_MOTORCYCLE;
            personCapacity = 1;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "LDV_G_EU6", vclass);
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
            break;
        case SVC_TRAILER:
            length = 16.5;
            maxSpeed = 130. / 3.6;
            width = 2.55;
            height = 4.;
            shape = SVS_TRUCK_SEMITRAILER;
            osgFile = "car-microcargo-citrus.obj";
            personCapacity = 2;
            containerCapacity = 2;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "HDV", vclass);
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
            break;
        case SVC_TRAM:
            length = 22.;
            maxSpeed = 80. / 3.6;
            width = 2.4;
            height = 3.2;
            shape = SVS_RAIL_CAR;
            personCapacity = 120;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            break;
        case SVC_RAIL_URBAN:
            length = 36.5 * 3;
            maxSpeed = 100. / 3.6;
            width = 3.0;
            height = 3.6;
            shape = SVS_RAIL_CAR;
            personCapacity = 300;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            break;
        case SVC_RAIL:
            length = 67.5 * 2;
            maxSpeed = 160. / 3.6;
            width = 2.84;
            height = 3.75;
            shape = SVS_RAIL;
            personCapacity = 434;
            // slight understatement (-:
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "HDV_D_EU0", vclass);
            break;
        case SVC_RAIL_ELECTRIC:
            length = 25. * 8;
            maxSpeed = 330. / 3.6;
            width = 2.95;
            height = 3.89;
            shape = SVS_RAIL;
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
            break;
        case SVC_EMERGENCY:
            length = 6.5;
            width = 2.16;
            height = 2.86;
            shape = SVS_DELIVERY;
            personCapacity = 2;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "LDV", vclass);
            break;
        case SVC_PASSENGER:
            shape = SVS_PASSENGER;
            break;
        case SVC_E_VEHICLE:
            shape = SVS_E_VEHICLE;
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "zero", vclass);
            break;
        case SVC_SHIP:
            length = 17;
            width = 4;
            maxSpeed = 8 / 1.94; // 8 knots
            height = 4;
            shape = SVS_SHIP;
            // slight understatement (-:
            emissionClass = PollutantsInterface::getClassByName(EMPREFIX + "HDV_D_EU0", vclass);
            break;
        default:
            break;
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
    if (wasSet(VTYPEPARS_SPEEDDEVIATION_SET)) {
        dev.writeAttr(SUMO_ATTR_SPEEDDEV, speedDev);
    }
    if (wasSet(VTYPEPARS_VEHICLECLASS_SET)) {
        dev.writeAttr(SUMO_ATTR_VCLASS, toString(vehicleClass));
    }
    if (wasSet(VTYPEPARS_EMISSIONCLASS_SET)) {
        dev.writeAttr(SUMO_ATTR_EMISSIONCLASS, PollutantsInterface::getName(emissionClass));
    }
    if (wasSet(VTYPEPARS_IMPATIENCE_SET)) {
        if (impatience == -std::numeric_limits<SUMOReal>::max()) {
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
    if (wasSet(VTYPEPARS_LANE_CHANGE_MODEL_SET) || lcParameter.size() != 0) {
        dev.writeAttr(SUMO_ATTR_LANE_CHANGE_MODEL, lcModel);
        std::vector<SumoXMLAttr> attrs;
        for (SubParams::const_iterator i = lcParameter.begin(); i != lcParameter.end(); ++i) {
            attrs.push_back(i->first);
        }
        std::sort(attrs.begin(), attrs.end());
        for (std::vector<SumoXMLAttr>::const_iterator i = attrs.begin(); i != attrs.end(); ++i) {
            dev.writeAttr(*i, lcParameter.find(*i)->second);
        }
    }
    if (wasSet(VTYPEPARS_CAR_FOLLOW_MODEL) || cfParameter.size() != 0) {
        dev.openTag(cfModel);
        std::vector<SumoXMLAttr> attrs;
        for (SubParams::const_iterator i = cfParameter.begin(); i != cfParameter.end(); ++i) {
            attrs.push_back(i->first);
        }
        std::sort(attrs.begin(), attrs.end());
        for (std::vector<SumoXMLAttr>::const_iterator i = attrs.begin(); i != attrs.end(); ++i) {
            dev.writeAttr(*i, cfParameter.find(*i)->second);
        }
        dev.closeTag();
    }
    writeParams(dev);
    dev.closeTag();
}


SUMOReal
SUMOVTypeParameter::getCFParam(const SumoXMLAttr attr, const SUMOReal defaultValue) const {
    if (cfParameter.count(attr)) {
        return cfParameter.find(attr)->second;
    } else {
        return defaultValue;
    }
}

SUMOReal
SUMOVTypeParameter::getLCParam(const SumoXMLAttr attr, const SUMOReal defaultValue) const {
    if (lcParameter.count(attr)) {
        return lcParameter.find(attr)->second;
    } else {
        return defaultValue;
    }
}



SUMOReal
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
            return 0.5;
        case SVC_SHIP:
            return 0.1;
        default:
            return 2.6;//2.9;
    }
}


SUMOReal
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
            return 4.;
        case SVC_TRAILER:
            return 4.;
        case SVC_BUS:
            return 4.;
        case SVC_COACH:
            return 4.;
        case SVC_TRAM:
            return 3.;
        case SVC_RAIL_URBAN:
            return 3.;
        case SVC_RAIL:
            return 1.3;
        case SVC_RAIL_ELECTRIC:
            return 1.3;
        case SVC_SHIP:
            return 0.15;
        default:
            return 4.5;//7.5;
    }
}


SUMOReal
SUMOVTypeParameter::getDefaultImperfection(const SUMOVehicleClass vc) {
    switch (vc) {
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
        case SVC_RAIL:
        case SVC_RAIL_ELECTRIC:
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
