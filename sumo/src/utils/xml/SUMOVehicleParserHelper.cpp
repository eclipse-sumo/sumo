/****************************************************************************/
/// @file    SUMOVehicleParserHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 07.04.2008
/// @version $Id$
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.dlr.de/
// Copyright (C) 2008-2017 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/common/FileHelpers.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/options/OptionsCont.h>
#include "SUMOVehicleParserHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
SUMOVehicleParserHelper::CFAttrMap SUMOVehicleParserHelper::allowedCFModelAttrs;
SUMOVehicleParserHelper::LCAttrMap SUMOVehicleParserHelper::allowedLCModelAttrs;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOVehicleParameter*
SUMOVehicleParserHelper::parseFlowAttributes(const SUMOSAXAttributes& attrs, const SUMOTime beginDefault, const SUMOTime endDefault) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) && attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        throw ProcessError("At most one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                           "' and '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                           "' has to be given in the definition of flow '" + id + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) && attrs.hasAttribute(SUMO_ATTR_PROB)) {
        throw ProcessError("At most one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                           "' and '" + attrs.getName(SUMO_ATTR_PROB) +
                           "' has to be given in the definition of flow '" + id + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_PROB) && attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        throw ProcessError("At most one of '" + attrs.getName(SUMO_ATTR_PROB) +
                           "' and '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                           "' has to be given in the definition of flow '" + id + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) || attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR) || attrs.hasAttribute(SUMO_ATTR_PROB)) {
        if (attrs.hasAttribute(SUMO_ATTR_END) && attrs.hasAttribute(SUMO_ATTR_NUMBER)) {
            throw ProcessError("If '" + attrs.getName(SUMO_ATTR_PERIOD) +
                               "', '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                               "' or '" + attrs.getName(SUMO_ATTR_PROB) +
                               "' are given at most one of '" + attrs.getName(SUMO_ATTR_END) +
                               "' and '" + attrs.getName(SUMO_ATTR_NUMBER) +
                               "' are allowed in flow '" + id + "'.");
        }
    } else {
        if (!attrs.hasAttribute(SUMO_ATTR_NUMBER)) {
            throw ProcessError("At least one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                               "', '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                               "', '" + attrs.getName(SUMO_ATTR_PROB) +
                               "', and '" + attrs.getName(SUMO_ATTR_NUMBER) +
                               "' is needed in flow '" + id + "'.");
        }
    }
    SUMOVehicleParameter* ret = new SUMOVehicleParameter();
    ret->id = id;
    try {
        parseCommonAttributes(attrs, ret, "flow");
    } catch (ProcessError&) {
        delete ret;
        throw;
    }

    // parse repetition information
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        ret->repetitionOffset = attrs.getSUMOTimeReporting(SUMO_ATTR_PERIOD, id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        const SUMOReal vph = attrs.get<SUMOReal>(SUMO_ATTR_VEHSPERHOUR, id.c_str(), ok);
        if (ok && vph <= 0) {
            delete ret;
            throw ProcessError("Invalid repetition rate in the definition of flow '" + id + "'.");
        }
        if (ok && vph != 0) {
            ret->repetitionOffset = TIME2STEPS(3600. / vph);
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_PROB)) {
        ret->repetitionProbability = attrs.get<SUMOReal>(SUMO_ATTR_PROB, id.c_str(), ok);
        if (ok && (ret->repetitionProbability <= 0 || ret->repetitionProbability > 1)) {
            delete ret;
            throw ProcessError("Invalid repetition probability in the definition of flow '" + id + "'.");
        }
    }

    ret->depart = beginDefault;
    if (attrs.hasAttribute(SUMO_ATTR_BEGIN)) {
        ret->depart = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), ok);
    }
    if (ok && ret->depart < 0) {
        delete ret;
        throw ProcessError("Negative begin time in the definition of flow '" + id + "'.");
    }
    ret->repetitionEnd = endDefault;
    if (ret->repetitionEnd < 0) {
        ret->repetitionEnd = SUMOTime_MAX;
    }
    if (attrs.hasAttribute(SUMO_ATTR_END)) {
        ret->repetitionEnd = attrs.getSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), ok);
    }
    if (ok && ret->repetitionEnd < ret->depart) {
        delete ret;
        throw ProcessError("Flow '" + id + "' ends before its begin time.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_NUMBER)) {
        ret->repetitionNumber = attrs.get<int>(SUMO_ATTR_NUMBER, id.c_str(), ok);
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        if (ret->repetitionNumber == 0) {
            WRITE_WARNING("Flow '" + id + "' has 0 vehicles; will skip it.");
        } else {
            if (ok && ret->repetitionNumber < 0) {
                delete ret;
                throw ProcessError("Negative repetition number in the definition of flow '" + id + "'.");
            }
            if (ok && ret->repetitionOffset < 0) {
                ret->repetitionOffset = (ret->repetitionEnd - ret->depart) / ret->repetitionNumber;
            }
        }
        ret->repetitionEnd = ret->depart + ret->repetitionNumber * ret->repetitionOffset;
    } else {
        // interpret repetitionNumber
        if (ok && ret->repetitionProbability > 0) {
            ret->repetitionNumber = std::numeric_limits<int>::max();
        } else {
            if (ok && ret->repetitionOffset <= 0) {
                delete ret;
                throw ProcessError("Invalid repetition rate in the definition of flow '" + id + "'.");
            }
            if (ret->repetitionEnd == SUMOTime_MAX) {
                ret->repetitionNumber = std::numeric_limits<int>::max();
            } else {
                const SUMOReal repLength = ret->repetitionEnd - ret->depart;
                ret->repetitionNumber = (int)ceil(repLength / ret->repetitionOffset);
            }
        }
    }
    if (!ok) {
        delete ret;
        throw ProcessError();
    }
    return ret;
}


SUMOVehicleParameter*
SUMOVehicleParserHelper::parseVehicleAttributes(const SUMOSAXAttributes& attrs,
        const bool optionalID, const bool skipDepart, const bool isPerson) {
    bool ok = true;
    std::string id, errorMsg;
    if (optionalID) {
        id = attrs.getOpt<std::string>(SUMO_ATTR_ID, 0, ok, "");
    } else {
        id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    }
    SUMOVehicleParameter* ret = new SUMOVehicleParameter();
    ret->id = id;
    if (isPerson) {
        ret->vtypeid = DEFAULT_PEDTYPE_ID;
    }
    try {
        parseCommonAttributes(attrs, ret, "vehicle");
        if (!skipDepart) {
            const std::string helper = attrs.get<std::string>(SUMO_ATTR_DEPART, ret->id.c_str(), ok);
            if (!ok || !SUMOVehicleParameter::parseDepart(helper, "vehicle", ret->id, ret->depart, ret->departProcedure, errorMsg)) {
                throw ProcessError(errorMsg);
            }
        }
    } catch (ProcessError&) {
        delete ret;
        throw;
    }
    return ret;
}


void
SUMOVehicleParserHelper::parseCommonAttributes(const SUMOSAXAttributes& attrs,
        SUMOVehicleParameter* ret, std::string element) {
    //ret->refid = attrs.getStringSecure(SUMO_ATTR_REFID, "");
    bool ok = true;
    // parse route information
    if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        ret->setParameter |= VEHPARS_ROUTE_SET; // !!! needed?
        ret->routeid = attrs.get<std::string>(SUMO_ATTR_ROUTE, ret->id.c_str(), ok);
    }
    // parse type information
    if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        ret->setParameter |= VEHPARS_VTYPE_SET; // !!! needed?
        ret->vtypeid = attrs.get<std::string>(SUMO_ATTR_TYPE, ret->id.c_str(), ok);
    }
    // parse line information
    if (attrs.hasAttribute(SUMO_ATTR_LINE)) {
        ret->setParameter |= VEHPARS_LINE_SET; // !!! needed?
        ret->line = attrs.get<std::string>(SUMO_ATTR_LINE, ret->id.c_str(), ok);
    }
    // parse zone information
    if (attrs.hasAttribute(SUMO_ATTR_FROM_TAZ)) {
        ret->setParameter |= VEHPARS_FROM_TAZ_SET;
        ret->fromTaz = attrs.get<std::string>(SUMO_ATTR_FROM_TAZ, ret->id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_TO_TAZ)) {
        ret->setParameter |= VEHPARS_TO_TAZ_SET;
        ret->toTaz = attrs.get<std::string>(SUMO_ATTR_TO_TAZ, ret->id.c_str(), ok);
    }
    // parse reroute information
    if (attrs.getOpt<bool>(SUMO_ATTR_REROUTE, 0, ok, false)) {
        ret->setParameter |= VEHPARS_FORCE_REROUTE;
    }

    std::string error;
    // parse depart lane information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
        ret->setParameter |= VEHPARS_DEPARTLANE_SET;
        const std::string helper = attrs.get<std::string>(SUMO_ATTR_DEPARTLANE, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseDepartLane(helper, element, ret->id, ret->departLane, ret->departLaneProcedure, error)) {
            throw ProcessError(error);
        }
    }
    // parse depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
        ret->setParameter |= VEHPARS_DEPARTPOS_SET;
        const std::string helper = attrs.get<std::string>(SUMO_ATTR_DEPARTPOS, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseDepartPos(helper, element, ret->id, ret->departPos, ret->departPosProcedure, error)) {
            throw ProcessError(error);
        }
    }
    // parse lateral depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS_LAT)) {
        ret->setParameter |= VEHPARS_DEPARTPOSLAT_SET;
        const std::string helper = attrs.get<std::string>(SUMO_ATTR_DEPARTPOS_LAT, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseDepartPosLat(helper, element, ret->id, ret->departPosLat, ret->departPosLatProcedure, error)) {
            throw ProcessError(error);
        }
    }
    // parse depart speed information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED)) {
        ret->setParameter |= VEHPARS_DEPARTSPEED_SET;
        std::string helper = attrs.get<std::string>(SUMO_ATTR_DEPARTSPEED, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseDepartSpeed(helper, element, ret->id, ret->departSpeed, ret->departSpeedProcedure, error)) {
            throw ProcessError(error);
        }
    }

    // parse arrival lane information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
        ret->setParameter |= VEHPARS_ARRIVALLANE_SET;
        std::string helper = attrs.get<std::string>(SUMO_ATTR_ARRIVALLANE, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseArrivalLane(helper, element, ret->id, ret->arrivalLane, ret->arrivalLaneProcedure, error)) {
            throw ProcessError(error);
        }
    }
    // parse arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
        ret->setParameter |= VEHPARS_ARRIVALPOS_SET;
        std::string helper = attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseArrivalPos(helper, element, ret->id, ret->arrivalPos, ret->arrivalPosProcedure, error)) {
            throw ProcessError(error);
        }
    }
    // parse lateral arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS_LAT)) {
        ret->setParameter |= VEHPARS_ARRIVALPOSLAT_SET;
        std::string helper = attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS_LAT, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseArrivalPosLat(helper, element, ret->id, ret->arrivalPosLat, ret->arrivalPosLatProcedure, error)) {
            throw ProcessError(error);
        }
    }
    // parse arrival speed information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED)) {
        ret->setParameter |= VEHPARS_ARRIVALSPEED_SET;
        std::string helper = attrs.get<std::string>(SUMO_ATTR_ARRIVALSPEED, ret->id.c_str(), ok);
        if (!SUMOVehicleParameter::parseArrivalSpeed(helper, element, ret->id, ret->arrivalSpeed, ret->arrivalSpeedProcedure, error)) {
            throw ProcessError(error);
        }
    }

    // parse color
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        ret->setParameter |= VEHPARS_COLOR_SET;
        ret->color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, ret->id.c_str(), ok);
    } else {
        ret->color = RGBColor::DEFAULT_COLOR;
    }
    // parse person number
    if (attrs.hasAttribute(SUMO_ATTR_PERSON_NUMBER)) {
        ret->setParameter |= VEHPARS_PERSON_NUMBER_SET;
        ret->personNumber = attrs.get<int>(SUMO_ATTR_PERSON_NUMBER, ret->id.c_str(), ok);
    }
    // parse container number
    if (attrs.hasAttribute(SUMO_ATTR_CONTAINER_NUMBER)) {
        ret->setParameter |= VEHPARS_CONTAINER_NUMBER_SET;
        ret->containerNumber = attrs.get<int>(SUMO_ATTR_CONTAINER_NUMBER, ret->id.c_str(), ok);
    }
}


SUMOVTypeParameter*
SUMOVehicleParserHelper::beginVTypeParsing(const SUMOSAXAttributes& attrs, const std::string& file) {
    bool ok = true;
    std::string id = attrs.get<std::string>(SUMO_ATTR_ID, 0, ok);
    SUMOVehicleClass vClass = SVC_PASSENGER;
    if (attrs.hasAttribute(SUMO_ATTR_VCLASS)) {
        vClass = parseVehicleClass(attrs, id);
    }
    SUMOVTypeParameter* vtype = new SUMOVTypeParameter(id, vClass);
    if (attrs.hasAttribute(SUMO_ATTR_VCLASS)) {
        vtype->setParameter |= VTYPEPARS_VEHICLECLASS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_LENGTH)) {
        vtype->length = attrs.get<SUMOReal>(SUMO_ATTR_LENGTH, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_LENGTH_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_MINGAP)) {
        vtype->minGap = attrs.get<SUMOReal>(SUMO_ATTR_MINGAP, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MINGAP_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED)) {
        vtype->maxSpeed = attrs.get<SUMOReal>(SUMO_ATTR_MAXSPEED, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MAXSPEED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPEEDFACTOR)) {
        vtype->speedFactor = attrs.get<SUMOReal>(SUMO_ATTR_SPEEDFACTOR, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_SPEEDFACTOR_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPEEDDEV)) {
        vtype->speedDev = attrs.get<SUMOReal>(SUMO_ATTR_SPEEDDEV, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_SPEEDDEVIATION_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EMISSIONCLASS)) {
        vtype->emissionClass = parseEmissionClass(attrs, vtype->id);
        vtype->setParameter |= VTYPEPARS_EMISSIONCLASS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_IMPATIENCE)) {
        if (attrs.get<std::string>(SUMO_ATTR_IMPATIENCE, vtype->id.c_str(), ok) == "off") {
            vtype->impatience = -std::numeric_limits<SUMOReal>::max();
        } else {
            vtype->impatience = attrs.get<SUMOReal>(SUMO_ATTR_IMPATIENCE, vtype->id.c_str(), ok);
        }
        vtype->setParameter |= VTYPEPARS_IMPATIENCE_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_WIDTH)) {
        vtype->width = attrs.get<SUMOReal>(SUMO_ATTR_WIDTH, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_WIDTH_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_HEIGHT)) {
        vtype->height = attrs.get<SUMOReal>(SUMO_ATTR_HEIGHT, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_HEIGHT_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_GUISHAPE)) {
        vtype->shape = parseGuiShape(attrs, vtype->id);
        vtype->setParameter |= VTYPEPARS_SHAPE_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_OSGFILE)) {
        vtype->osgFile = attrs.get<std::string>(SUMO_ATTR_OSGFILE, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_OSGFILE_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_IMGFILE)) {
        vtype->imgFile = attrs.get<std::string>(SUMO_ATTR_IMGFILE, vtype->id.c_str(), ok);
        if (vtype->imgFile != "" && !FileHelpers::isAbsolute(vtype->imgFile)) {
            vtype->imgFile = FileHelpers::getConfigurationRelative(file, vtype->imgFile);
        }
        vtype->setParameter |= VTYPEPARS_IMGFILE_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        vtype->color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_COLOR_SET;
    } else {
        vtype->color = RGBColor::YELLOW;
    }
    if (attrs.hasAttribute(SUMO_ATTR_PROB)) {
        vtype->defaultProbability = attrs.get<SUMOReal>(SUMO_ATTR_PROB, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_PROBABILITY_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_LANE_CHANGE_MODEL)) {
        std::string lcmS = attrs.get<std::string>(SUMO_ATTR_LANE_CHANGE_MODEL, vtype->id.c_str(), ok);
        if (lcmS == "JE2013") {
            WRITE_WARNING("Lane change model 'JE2013' is deprecated. Using default model instead.");
            lcmS = "default";
        }
        if (SUMOXMLDefinitions::LaneChangeModels.hasString(lcmS)) {
            vtype->lcModel = SUMOXMLDefinitions::LaneChangeModels.get("default");
        } else {
            WRITE_ERROR("Unknown lane change model '" + lcmS + "' when parsing vtype '" + vtype->id + "'");
            throw ProcessError();
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL)) {
        const std::string cfmS = attrs.get<std::string>(SUMO_ATTR_CAR_FOLLOW_MODEL, vtype->id.c_str(), ok);
        if (SUMOXMLDefinitions::CarFollowModels.hasString(cfmS)) {
            vtype->cfModel = SUMOXMLDefinitions::CarFollowModels.get(cfmS);
            vtype->setParameter |= VTYPEPARS_CAR_FOLLOW_MODEL;
        } else {
            WRITE_ERROR("Unknown car following model '" + cfmS + "' when parsing vtype '" + vtype->id + "'");
            throw ProcessError();
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERSON_CAPACITY)) {
        vtype->personCapacity = attrs.get<int>(SUMO_ATTR_PERSON_CAPACITY, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_PERSON_CAPACITY;
    }
    if (attrs.hasAttribute(SUMO_ATTR_CONTAINER_CAPACITY)) {
        vtype->containerCapacity = attrs.get<int>(SUMO_ATTR_CONTAINER_CAPACITY, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_CONTAINER_CAPACITY;
    }
    if (attrs.hasAttribute(SUMO_ATTR_BOARDING_DURATION)) {
        vtype->boardingDuration = attrs.getSUMOTimeReporting(SUMO_ATTR_BOARDING_DURATION, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_BOARDING_DURATION;
    }
    if (attrs.hasAttribute(SUMO_ATTR_LOADING_DURATION)) {
        vtype->loadingDuration = attrs.getSUMOTimeReporting(SUMO_ATTR_LOADING_DURATION, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_LOADING_DURATION;
    }
    if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED_LAT)) {
        vtype->maxSpeedLat = attrs.get<SUMOReal>(SUMO_ATTR_MAXSPEED_LAT, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MAXSPEED_LAT_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_MINGAP_LAT)) {
        vtype->minGapLat = attrs.get<SUMOReal>(SUMO_ATTR_MINGAP_LAT, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MINGAP_LAT_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_LATALIGNMENT)) {
        const std::string alignS = attrs.get<std::string>(SUMO_ATTR_LATALIGNMENT, vtype->id.c_str(), ok);
        if (SUMOXMLDefinitions::LateralAlignments.hasString(alignS)) {
            vtype->latAlignment = SUMOXMLDefinitions::LateralAlignments.get(alignS);
            vtype->setParameter |= VTYPEPARS_LATALIGNMENT_SET;
        } else {
            WRITE_ERROR("Unknown lateral alignment '" + alignS + "' when parsing vtype '" + vtype->id + "'");
            throw ProcessError();
        }
    }
    parseVTypeEmbedded(*vtype, vtype->cfModel, attrs, true);
    parseLCParams(*vtype, vtype->lcModel, attrs);
    if (!ok) {
        delete vtype;
        throw ProcessError();
    }
    return vtype;
}


void
SUMOVehicleParserHelper::parseVTypeEmbedded(SUMOVTypeParameter& into,
        int element, const SUMOSAXAttributes& attrs,
        bool fromVType) {
    const CFAttrMap& allowedAttrs = getAllowedCFModelAttrs();
    CFAttrMap::const_iterator cf_it;
    for (cf_it = allowedAttrs.begin(); cf_it != allowedAttrs.end(); cf_it++) {
        if (cf_it->first == element) {
            break;
        }
    }
    if (cf_it == allowedAttrs.end()) {
        if (SUMOXMLDefinitions::Tags.has(element)) {
            WRITE_ERROR("Unknown cfmodel " + toString((SumoXMLTag)element) + " when parsing vtype '" + into.id + "'");
        } else {
            WRITE_ERROR("Unknown cfmodel when parsing vtype '" + into.id + "'");
        }
        throw ProcessError();
        return;
    }
    if (!fromVType) {
        into.cfModel = cf_it->first;
        into.setParameter |= VTYPEPARS_CAR_FOLLOW_MODEL;
    }
    bool ok = true;
    for (std::set<SumoXMLAttr>::const_iterator it = cf_it->second.begin(); it != cf_it->second.end(); it++) {
        if (attrs.hasAttribute(*it)) {
            into.cfParameter[*it] = attrs.get<SUMOReal>(*it, into.id.c_str(), ok);
            if (*it == SUMO_ATTR_TAU && TIME2STEPS(into.cfParameter[*it]) < DELTA_T) {
                WRITE_WARNING("Value of tau=" + toString(into.cfParameter[*it])
                              + " in car following model '" + toString(into.cfModel) + "' lower than simulation step size may cause collisions");
            }
        }
    }
    if (!ok) {
        throw ProcessError();
    }
}


const SUMOVehicleParserHelper::CFAttrMap&
SUMOVehicleParserHelper::getAllowedCFModelAttrs() {
    // init on first use
    if (allowedCFModelAttrs.size() == 0) {
        std::set<SumoXMLAttr> krausParams;
        krausParams.insert(SUMO_ATTR_ACCEL);
        krausParams.insert(SUMO_ATTR_DECEL);
        krausParams.insert(SUMO_ATTR_SIGMA);
        krausParams.insert(SUMO_ATTR_TAU);
        allowedCFModelAttrs[SUMO_TAG_CF_KRAUSS] = krausParams;
        allowedCFModelAttrs[SUMO_TAG_CF_KRAUSS_ORIG1] = krausParams;
        allowedCFModelAttrs[SUMO_TAG_CF_KRAUSS_PLUS_SLOPE] = krausParams;

        std::set<SumoXMLAttr> smartSKParams;
        smartSKParams.insert(SUMO_ATTR_ACCEL);
        smartSKParams.insert(SUMO_ATTR_DECEL);
        smartSKParams.insert(SUMO_ATTR_SIGMA);
        smartSKParams.insert(SUMO_ATTR_TAU);
        smartSKParams.insert(SUMO_ATTR_TMP1);
        smartSKParams.insert(SUMO_ATTR_TMP2);
        smartSKParams.insert(SUMO_ATTR_TMP3);
        smartSKParams.insert(SUMO_ATTR_TMP4);
        smartSKParams.insert(SUMO_ATTR_TMP5);
        allowedCFModelAttrs[SUMO_TAG_CF_SMART_SK] = smartSKParams;

        std::set<SumoXMLAttr> daniel1Params;
        daniel1Params.insert(SUMO_ATTR_ACCEL);
        daniel1Params.insert(SUMO_ATTR_DECEL);
        daniel1Params.insert(SUMO_ATTR_SIGMA);
        daniel1Params.insert(SUMO_ATTR_TAU);
        daniel1Params.insert(SUMO_ATTR_TMP1);
        daniel1Params.insert(SUMO_ATTR_TMP2);
        daniel1Params.insert(SUMO_ATTR_TMP3);
        daniel1Params.insert(SUMO_ATTR_TMP4);
        daniel1Params.insert(SUMO_ATTR_TMP5);
        allowedCFModelAttrs[SUMO_TAG_CF_DANIEL1] = daniel1Params;

        std::set<SumoXMLAttr> pwagParams;
        pwagParams.insert(SUMO_ATTR_ACCEL);
        pwagParams.insert(SUMO_ATTR_DECEL);
        pwagParams.insert(SUMO_ATTR_SIGMA);
        pwagParams.insert(SUMO_ATTR_TAU);
        pwagParams.insert(SUMO_ATTR_CF_PWAGNER2009_TAULAST);
        pwagParams.insert(SUMO_ATTR_CF_PWAGNER2009_APPROB);
        allowedCFModelAttrs[SUMO_TAG_CF_PWAGNER2009] = pwagParams;

        std::set<SumoXMLAttr> idmParams;
        idmParams.insert(SUMO_ATTR_ACCEL);
        idmParams.insert(SUMO_ATTR_DECEL);
        idmParams.insert(SUMO_ATTR_TAU);
        idmParams.insert(SUMO_ATTR_CF_IDM_DELTA);
        idmParams.insert(SUMO_ATTR_CF_IDM_STEPPING);
        allowedCFModelAttrs[SUMO_TAG_CF_IDM] = idmParams;

        std::set<SumoXMLAttr> idmmParams;
        idmmParams.insert(SUMO_ATTR_ACCEL);
        idmmParams.insert(SUMO_ATTR_DECEL);
        idmmParams.insert(SUMO_ATTR_TAU);
        idmmParams.insert(SUMO_ATTR_CF_IDMM_ADAPT_FACTOR);
        idmmParams.insert(SUMO_ATTR_CF_IDMM_ADAPT_TIME);
        idmmParams.insert(SUMO_ATTR_CF_IDM_STEPPING);
        allowedCFModelAttrs[SUMO_TAG_CF_IDMM] = idmmParams;

        std::set<SumoXMLAttr> bkernerParams;
        bkernerParams.insert(SUMO_ATTR_ACCEL);
        bkernerParams.insert(SUMO_ATTR_DECEL);
        bkernerParams.insert(SUMO_ATTR_TAU);
        bkernerParams.insert(SUMO_ATTR_K);
        bkernerParams.insert(SUMO_ATTR_CF_KERNER_PHI);
        allowedCFModelAttrs[SUMO_TAG_CF_BKERNER] = bkernerParams;

        std::set<SumoXMLAttr> wiedemannParams;
        wiedemannParams.insert(SUMO_ATTR_ACCEL);
        wiedemannParams.insert(SUMO_ATTR_DECEL);
        wiedemannParams.insert(SUMO_ATTR_CF_WIEDEMANN_SECURITY);
        wiedemannParams.insert(SUMO_ATTR_CF_WIEDEMANN_ESTIMATION);
        allowedCFModelAttrs[SUMO_TAG_CF_WIEDEMANN] = wiedemannParams;
    }
    return allowedCFModelAttrs;
}


void
SUMOVehicleParserHelper::parseLCParams(SUMOVTypeParameter& into, LaneChangeModel model, const SUMOSAXAttributes& attrs) {
    if (allowedLCModelAttrs.size() == 0) {
        // init static map
        std::set<SumoXMLAttr> lc2013Params;
        lc2013Params.insert(SUMO_ATTR_LCA_STRATEGIC_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_COOPERATIVE_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_SPEEDGAIN_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_KEEPRIGHT_PARAM);
        allowedLCModelAttrs[LCM_LC2013] = lc2013Params;

        std::set<SumoXMLAttr> sl2015Params = lc2013Params;
        sl2015Params.insert(SUMO_ATTR_LCA_PUSHY);
        sl2015Params.insert(SUMO_ATTR_LCA_SUBLANE_PARAM);
        sl2015Params.insert(SUMO_ATTR_LCA_ASSERTIVE);
        allowedLCModelAttrs[LCM_SL2015] = sl2015Params;

        std::set<SumoXMLAttr> noParams;
        allowedLCModelAttrs[LCM_DK2008] = noParams;

        // default model may be either LC2013 or SL2013
        // we allow both sets (sl2015 is a superset of lc2013Params)
        allowedLCModelAttrs[LCM_DEFAULT] = sl2015Params;
    }
    bool ok = true;
    std::set<SumoXMLAttr> allowed = allowedLCModelAttrs[model];
    for (std::set<SumoXMLAttr>::const_iterator it = allowed.begin(); it != allowed.end(); it++) {
        if (attrs.hasAttribute(*it)) {
            into.lcParameter[*it] = attrs.get<SUMOReal>(*it, into.id.c_str(), ok);
        }
    }
    if (!ok) {
        throw ProcessError();
    }
}


SUMOVehicleClass
SUMOVehicleParserHelper::parseVehicleClass(const SUMOSAXAttributes& attrs,
        const std::string& id) {
    SUMOVehicleClass vclass = SVC_IGNORING;
    try {
        bool ok = true;
        std::string vclassS = attrs.getOpt<std::string>(SUMO_ATTR_VCLASS, id.c_str(), ok, "");
        if (vclassS == "") {
            return vclass;
        }
        const SUMOVehicleClass result = getVehicleClassID(vclassS);
        const std::string& realName = SumoVehicleClassStrings.getString(result);
        if (realName != vclassS) {
            WRITE_WARNING("The vehicle class '" + vclassS + "' for " + attrs.getObjectType() + " '" + id + "' is deprecated, use '" + realName + "' instead.");
        }
        return result;
    } catch (...) {
        WRITE_ERROR("The class for " + attrs.getObjectType() + " '" + id + "' is not known.");
    }
    return vclass;
}


SUMOEmissionClass
SUMOVehicleParserHelper::parseEmissionClass(const SUMOSAXAttributes& attrs, const std::string& id) {
    try {
        bool ok = true;
        std::string eClassS = attrs.getOpt<std::string>(SUMO_ATTR_EMISSIONCLASS, id.c_str(), ok, "");
        return PollutantsInterface::getClassByName(eClassS);
    } catch (...) {
        WRITE_ERROR("The emission class for " + attrs.getObjectType() + " '" + id + "' is not known.");
        return 0;
    }
}


SUMOVehicleShape
SUMOVehicleParserHelper::parseGuiShape(const SUMOSAXAttributes& attrs, const std::string& id) {
    bool ok = true;
    std::string vclassS = attrs.getOpt<std::string>(SUMO_ATTR_GUISHAPE, id.c_str(), ok, "");
    if (SumoVehicleShapeStrings.hasString(vclassS)) {
        const SUMOVehicleShape result = SumoVehicleShapeStrings.get(vclassS);
        const std::string& realName = SumoVehicleShapeStrings.getString(result);
        if (realName != vclassS) {
            WRITE_WARNING("The shape '" + vclassS + "' for " + attrs.getObjectType() + " '" + id + "' is deprecated, use '" + realName + "' instead.");
        }
        return result;
    } else {
        WRITE_ERROR("The shape '" + vclassS + "' for " + attrs.getObjectType() + " '" + id + "' is not known.");
        return SVS_UNKNOWN;
    }
}

/****************************************************************************/

