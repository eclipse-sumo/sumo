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
// SUMO, Simulation of Urban MObility; see http://sumo-sim.org/
// Copyright (C) 2001-2014 DLR (http://www.dlr.de/) and contributors
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
#include <utils/common/SUMOVTypeParameter.h>
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


// ===========================================================================
// member method definitions
// ===========================================================================
SUMOVTypeParameter::SUMOVTypeParameter(const std::string& vtid, const SUMOVehicleClass vclass)
    : id(vtid), length(getDefaultLength(vclass)),
      minGap(getDefaultMinGap(vclass)), maxSpeed(getDefaultMaxSpeed(vclass)),
      defaultProbability(DEFAULT_VEH_PROB),
      speedFactor(getDefaultSpeedFactor(vclass)), speedDev(getDefaultSpeedDev(vclass)),
      emissionClass(getDefaultEmissionClass(vclass)), color(RGBColor::DEFAULT_COLOR),
      vehicleClass(vclass),
      impatience(getDefaultImpatience(vclass)),
      width(getDefaultWidth(vclass)),
      height(getDefaultHeight(vclass)), shape(getDefaultShape(vclass)),
      cfModel(getDefaultFollowModel(vclass)), lcModel(getDefaultLaneChangeModel(vclass)),
      setParameter(0), saved(false), onlyReferenced(false) {
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
    if (wasSet(VTYPEPARS_LANE_CHANGE_MODEL_SET)) {
        dev.writeAttr(SUMO_ATTR_LANE_CHANGE_MODEL, lcModel);
    }

    if (cfParameter.size() != 0) {
        dev.openTag(cfModel);
        std::vector<SumoXMLAttr> attrs;
        for (CFParams::const_iterator i = cfParameter.begin(); i != cfParameter.end(); ++i) {
            attrs.push_back(i->first);
        }
        std::sort(attrs.begin(), attrs.end());
        for (std::vector<SumoXMLAttr>::const_iterator i = attrs.begin(); i != attrs.end(); ++i) {
            dev.writeAttr(*i, cfParameter.find(*i)->second);
        }
        dev.closeTag();
        dev.closeTag();
    } else {
        dev.closeTag();
    }
}


SUMOReal
SUMOVTypeParameter::get(const SumoXMLAttr attr, const SUMOReal defaultValue) const {
    if (cfParameter.count(attr)) {
        return cfParameter.find(attr)->second;
    } else {
        return defaultValue;
    }
}


SUMOReal
SUMOVTypeParameter::getDefaultMaxSpeed(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
            return 5./3.6;
        case SVC_BICYCLE:
            return 20./3.6;
        case SVC_MOPED:
            return 60./3.6;
        case SVC_TRUCK:
            return 160./3.6;
        case SVC_TRAILER:
        case SVC_BUS:
            return 140./3.6;
        case SVC_TRAM:
            return 70./3.6;
        case SVC_RAIL_URBAN:
            return 100./3.6;
        case SVC_RAIL:
            return 160./3.6;
        case SVC_RAIL_ELECTRIC:
            return 330./3.6;
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_DELIVERY:
        case SVC_MOTORCYCLE:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 200./3.6;
}


SUMOReal
SUMOVTypeParameter::getDefaultAccel(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
            return 1.;
        case SVC_BICYCLE:
            return 1.2;
        case SVC_MOTORCYCLE:
            return 6.;
        case SVC_TRUCK:
            return 2.;
        case SVC_TRAILER:
            return 1.5;
        case SVC_BUS:
            return 2.;
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
        case SVC_RAIL:
            return 1.;
        case SVC_RAIL_ELECTRIC:
        case SVC_MOPED:
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_DELIVERY:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 2.6;
}


SUMOReal
SUMOVTypeParameter::getDefaultDecel(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
            return 1.;
        case SVC_BICYCLE:
            return 3.;
        case SVC_MOTORCYCLE:
            return 4.;
        case SVC_TRUCK:
            return 3.;
        case SVC_TRAILER:
            return 2.5;
        case SVC_BUS:
            return 3.;
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
            return 1.3;
        case SVC_RAIL:
            return 1.1;
        case SVC_RAIL_ELECTRIC:
        case SVC_MOPED:
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_DELIVERY:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 4.5;
}


SUMOReal
SUMOVTypeParameter::getDefaultSigma(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
        case SVC_RAIL:
        case SVC_RAIL_ELECTRIC:
            return 0.;
        case SVC_PEDESTRIAN:
        case SVC_BICYCLE:
        case SVC_MOTORCYCLE:
        case SVC_TRUCK:
        case SVC_TRAILER:
        case SVC_BUS:
        case SVC_MOPED:
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_DELIVERY:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 0.5;
}


SUMOReal
SUMOVTypeParameter::getDefaultLength(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
            return 0.215;
        case SVC_BICYCLE:
            return 1.6;
        case SVC_MOPED:
        case SVC_MOTORCYCLE:
            return 2.1;
        case SVC_TRUCK:
            return 8.05;
        case SVC_TRAILER:
            return 16.5;
        case SVC_BUS:
            return 14.;
        case SVC_TRAM:
            return 22.;
        case SVC_RAIL_URBAN:
            return 36.5*3;
        case SVC_RAIL:
            return 67.5*2;
        case SVC_RAIL_ELECTRIC:
            return 25.*8;
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_DELIVERY:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 5.;
}


SUMOReal
SUMOVTypeParameter::getDefaultMinGap(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
        case SVC_BICYCLE:
            return 0.3;
        case SVC_MOPED:
        case SVC_MOTORCYCLE:
        case SVC_TRUCK:
        case SVC_TRAILER:
        case SVC_BUS:
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
        case SVC_RAIL:
        case SVC_RAIL_ELECTRIC:
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_DELIVERY:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 2.5;
}


SUMOReal
SUMOVTypeParameter::getDefaultTau(const SUMOVehicleClass vc) {
    return 1.;
}


SUMOReal
SUMOVTypeParameter::getDefaultImpatience(const SUMOVehicleClass vc) {
    return 0.;
}


SUMOReal
SUMOVTypeParameter::getDefaultSpeedFactor(const SUMOVehicleClass vc) {
    return 1.;
}


SUMOReal
SUMOVTypeParameter::getDefaultSpeedDev(const SUMOVehicleClass vc) {
    return 0.;
}


SUMOReal
SUMOVTypeParameter::getDefaultWidth(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
            return 0.478;
        case SVC_BICYCLE:
            return 0.65;
        case SVC_MOPED:
        case SVC_MOTORCYCLE:
            return 0.8;
        case SVC_TRUCK:
            return 2.5;
        case SVC_TRAILER:
        case SVC_BUS:
            return 2.55;
        case SVC_TRAM:
            return 2.4;
        case SVC_RAIL_URBAN:
            return 3.0;
        case SVC_RAIL:
            return 2.84;
        case SVC_RAIL_ELECTRIC:
            return 2.95;
        case SVC_DELIVERY:
            return 1.9;
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 1.7;
}


SUMOReal
SUMOVTypeParameter::getDefaultHeight(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
            return 1.719;
        case SVC_BICYCLE:
            return 1.7;
        case SVC_MOPED:
        case SVC_MOTORCYCLE:
            return 1.2;
        case SVC_TRUCK:
            return 3.;
        case SVC_TRAILER:
        case SVC_BUS:
            return 4.;
        case SVC_TRAM:
            return 3.2;
        case SVC_RAIL_URBAN:
            return 3.6;
        case SVC_RAIL:
            return 3.75;
        case SVC_RAIL_ELECTRIC:
            return 3.89;
        case SVC_DELIVERY:
            return 2.2;
        case SVC_PRIVATE:
        case SVC_COACH:
        case SVC_EMERGENCY:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_PASSENGER:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_E_VEHICLE:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return 1.5;
}


SumoXMLTag
SUMOVTypeParameter::getDefaultFollowModel(const SUMOVehicleClass vc) {
    return SUMO_TAG_CF_KRAUSS;
}


LaneChangeModel
SUMOVTypeParameter::getDefaultLaneChangeModel(const SUMOVehicleClass vc) {
    return LCM_LC2013;
}


SUMOVehicleShape
SUMOVTypeParameter::getDefaultShape(const SUMOVehicleClass vc) {
    switch(vc) {
        case SVC_PEDESTRIAN:
            return SVS_PEDESTRIAN;
        case SVC_BICYCLE:
            return SVS_BICYCLE;
        case SVC_MOPED:
            return SVS_MOPED;
        case SVC_MOTORCYCLE:
            return SVS_MOTORCYCLE;
        case SVC_TRUCK:
            return SVS_TRUCK;
        case SVC_TRAILER:
            return SVS_TRUCK_SEMITRAILER;
        case SVC_BUS:
        case SVC_COACH:
            return SVS_BUS;
        case SVC_TRAM:
        case SVC_RAIL_URBAN:
            return SVS_RAIL_CAR;
        case SVC_RAIL:
        case SVC_RAIL_ELECTRIC:
            return SVS_RAIL;
        case SVC_PASSENGER:
            return SVS_PASSENGER;
        case SVC_DELIVERY:
        case SVC_EMERGENCY:
            return SVS_DELIVERY;
        case SVC_E_VEHICLE:
            return SVS_E_VEHICLE;
        case SVC_PRIVATE:
        case SVC_AUTHORITY:
        case SVC_ARMY:
        case SVC_VIP:
        case SVC_IGNORING:
        case SVC_HOV:
        case SVC_TAXI:
        case SVC_CUSTOM1:
        case SVC_CUSTOM2:
            break;
    }
    return SVS_UNKNOWN;
}


SUMOEmissionClass
SUMOVTypeParameter::getDefaultEmissionClass(const SUMOVehicleClass vc) {
    return PollutantsInterface::getClassByName("unknown", vc);
}


SUMOReal
SUMOVTypeParameter::getDefaultTmp1(const SUMOVehicleClass vc) {
    return 1.;
}


SUMOReal
SUMOVTypeParameter::getDefaultTmp2(const SUMOVehicleClass vc) {
    return 1.;
}


SUMOReal
SUMOVTypeParameter::getDefaultTmp3(const SUMOVehicleClass vc) {
    return 1.;
}


SUMOReal
SUMOVTypeParameter::getDefaultTmp4(const SUMOVehicleClass vc) {
    return 1.;
}


SUMOReal
SUMOVTypeParameter::getDefaultTmp5(const SUMOVehicleClass vc) {
    return 1.;
}


/****************************************************************************/
