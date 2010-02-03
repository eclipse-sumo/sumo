/****************************************************************************/
/// @file    SUMOVehicleParserHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 07.04.2008
/// @version $Id$
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2009 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/options/OptionsCont.h>
#include "SUMOVehicleParserHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOVehicleParameter *
SUMOVehicleParserHelper::parseFlowAttributes(const SUMOSAXAttributes &attrs) throw(ProcessError) {
    std::string id;
    if (!attrs.setIDFromAttributes("flow", id)) {
        throw ProcessError();
    }
    if (!(attrs.hasAttribute(SUMO_ATTR_PERIOD) ^ attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR))) {
        throw ProcessError("Exactly one of '" + attrs.getName(SUMO_ATTR_PERIOD) + "' and '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) + "' have to be given in the definition of flow '" + id + "'.");
    }
    bool ok = true;
    SUMOVehicleParameter *ret = new SUMOVehicleParameter();
    ret->id = id;
    parseCommonAttributes(attrs, ret, "flow");

    // parse repetition information
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        ret->repetitionOffset = attrs.getFloat(SUMO_ATTR_PERIOD);
    }
    if (attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        ret->repetitionOffset = 3600/attrs.getFloat(SUMO_ATTR_PERIOD);
    }

    ret->depart = attrs.getIntSecure(SUMO_ATTR_BEGIN, OptionsCont::getOptions().getInt("begin"));
    int end = attrs.getIntSecure(SUMO_ATTR_END, OptionsCont::getOptions().getInt("end"));
    if (end == INT_MAX) {
        ret->repetitionNumber = INT_MAX;
    } else {
        ret->repetitionNumber = (end - ret->depart) / ret->repetitionOffset;
    }


    if (!ok) {
        delete ret;
        throw ProcessError();
    }
    return ret;
}


SUMOVehicleParameter *
SUMOVehicleParserHelper::parseVehicleAttributes(const SUMOSAXAttributes &attrs,
        bool skipID, bool skipDepart) throw(ProcessError) {
    std::string id;
    if (!skipID && !attrs.setIDFromAttributes("vehicle", id)) {
        throw ProcessError();
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) ^ attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        throw ProcessError("The attributes '" + attrs.getName(SUMO_ATTR_PERIOD) + "' and '" + attrs.getName(SUMO_ATTR_REPNUMBER) + "' have to be given both in the definition of '" + id + "'.");
    }
    bool ok = true;
    SUMOVehicleParameter *ret = new SUMOVehicleParameter();
    ret->id = id;
    parseCommonAttributes(attrs, ret, "vehicle");
    if (!skipDepart) {
        ret->depart = attrs.getIntReporting(SUMO_ATTR_DEPART, "vehicle", id.c_str(), ok);
    }
    // parse repetition information
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        WRITE_WARNING("period and repno are deprecated in vehicle '" + id + "', use flows instead.");
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        ret->repetitionOffset = attrs.getInt(SUMO_ATTR_PERIOD);
    }
    if (attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        ret->setParameter |= VEHPARS_PERIODNUM_SET;
        ret->repetitionNumber = attrs.getInt(SUMO_ATTR_REPNUMBER);
    }

    if (!ok) {
        delete ret;
        throw ProcessError();
    }
    return ret;
}


void
SUMOVehicleParserHelper::parseCommonAttributes(const SUMOSAXAttributes &attrs,
                                               SUMOVehicleParameter *ret, std::string element) throw(ProcessError) {
    //ret->refid = attrs.getStringSecure(SUMO_ATTR_REFID, "");
    // parse route information
    if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        ret->setParameter |= VEHPARS_ROUTE_SET; // !!! needed?
        ret->routeid = attrs.getString(SUMO_ATTR_ROUTE);
    }
    // parse route information
    if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        ret->setParameter |= VEHPARS_VTYPE_SET; // !!! needed?
        ret->vtypeid = attrs.getString(SUMO_ATTR_TYPE);
    }

    // parse depart lane information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
        ret->setParameter |= VEHPARS_DEPARTLANE_SET;
        std::string helper = attrs.getString(SUMO_ATTR_DEPARTLANE);
        if (helper=="departlane") {
            ret->departLaneProcedure = DEPART_LANE_DEPARTLANE;
        } else if (helper=="random") {
            ret->departLaneProcedure = DEPART_LANE_RANDOM;
        } else if (helper=="free") {
            ret->departLaneProcedure = DEPART_LANE_FREE;
        } else {
            try {
                ret->departLane = TplConvert<char>::_2int(helper.c_str());;
                ret->departLaneProcedure = DEPART_LANE_GIVEN;
            } catch (NumberFormatException &) {
                throw ProcessError("Invalid departlane definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData &) {
                throw ProcessError("Invalid departlane definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
        ret->setParameter |= VEHPARS_DEPARTPOS_SET;
        std::string helper = attrs.getString(SUMO_ATTR_DEPARTPOS);
        if (helper=="random") {
            ret->departPosProcedure = DEPART_POS_RANDOM;
        } else if (helper=="random_free") {
            ret->departPosProcedure = DEPART_POS_RANDOM_FREE;
        } else if (helper=="free") {
            ret->departPosProcedure = DEPART_POS_FREE;
        } else {
            try {
                ret->departPos = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->departPosProcedure = DEPART_POS_GIVEN;
            } catch (NumberFormatException &) {
                throw ProcessError("Invalid departpos definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData &) {
                throw ProcessError("Invalid departpos definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED)) {
        ret->setParameter |= VEHPARS_DEPARTSPEED_SET;
        std::string helper = attrs.getString(SUMO_ATTR_DEPARTSPEED);
        if (helper=="random") {
            ret->departSpeedProcedure = DEPART_SPEED_RANDOM;
        } else if (helper=="max") {
            ret->departSpeedProcedure = DEPART_SPEED_MAX;
        } else {
            try {
                ret->departSpeed = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->departSpeedProcedure = DEPART_SPEED_GIVEN;
            } catch (NumberFormatException &) {
                throw ProcessError("Invalid departspeed definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData &) {
                throw ProcessError("Invalid departspeed definition for " + element + " '" + ret->id + "'");
            }
        }
    }

    // parse arrival lane information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
        ret->setParameter |= VEHPARS_ARRIVALLANE_SET;
        std::string helper = attrs.getString(SUMO_ATTR_ARRIVALLANE);
        if (helper=="current") {
            ret->arrivalLaneProcedure = ARRIVAL_LANE_CURRENT;
        } else {
            try {
                ret->arrivalLane = TplConvert<char>::_2int(helper.c_str());;
                ret->arrivalLaneProcedure = ARRIVAL_LANE_GIVEN;
            } catch (NumberFormatException &) {
                throw ProcessError("Invalid arrivallane definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData &) {
                throw ProcessError("Invalid arrivallane definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
        ret->setParameter |= VEHPARS_ARRIVALPOS_SET;
        std::string helper = attrs.getString(SUMO_ATTR_ARRIVALPOS);
        if (helper=="random") {
            ret->arrivalPosProcedure = ARRIVAL_POS_RANDOM;
        } else if (helper=="max") {
            ret->arrivalPosProcedure = ARRIVAL_POS_MAX;
        } else {
            try {
                ret->arrivalPos = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->arrivalPosProcedure = ARRIVAL_POS_GIVEN;
            } catch (NumberFormatException &) {
                throw ProcessError("Invalid arrivalpos definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData &) {
                throw ProcessError("Invalid arrivalpos definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED)) {
        ret->setParameter |= VEHPARS_ARRIVALSPEED_SET;
        std::string helper = attrs.getString(SUMO_ATTR_ARRIVALSPEED);
        if (helper=="current") {
            ret->arrivalSpeedProcedure = ARRIVAL_SPEED_CURRENT;
        } else {
            try {
                ret->arrivalSpeed = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->arrivalSpeedProcedure = ARRIVAL_SPEED_GIVEN;
            } catch (NumberFormatException &) {
                throw ProcessError("Invalid arrivalspeed definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData &) {
                throw ProcessError("Invalid arrivalspeed definition for " + element + " '" + ret->id + "'");
            }
        }
    }

    // parse color
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        ret->setParameter |= VEHPARS_COLOR_SET;
        ret->color = RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR));
    } else {
        ret->color = RGBColor(1,1,0);
    }
}


SUMOVTypeParameter *
SUMOVehicleParserHelper::beginVTypeParsing(const SUMOSAXAttributes &attrs) throw(ProcessError) {
    SUMOVTypeParameter *vtype = new SUMOVTypeParameter();
    if (!attrs.setIDFromAttributes("vtype", vtype->id)) {
        throw ProcessError();
    }
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_LENGTH)) {
        vtype->length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, "vtype", vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_LENGTH_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED)) {
        vtype->maxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED, "vtype", vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MAXSPEED_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPEEDFACTOR)) {
        vtype->speedFactor = attrs.getSUMORealReporting(SUMO_ATTR_SPEEDFACTOR, "vtype", vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_SPEEDFACTOR_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPEEDDEV)) {
        vtype->speedDev = attrs.getSUMORealReporting(SUMO_ATTR_SPEEDDEV, "vtype", vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_SPEEDDEVIATION_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EMISSIONCLASS)) {
        vtype->emissionClass = parseEmissionClass(attrs, "vtype", vtype->id);
        vtype->setParameter |= VTYPEPARS_EMISSIONCLASS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_VCLASS)) {
        vtype->vehicleClass = parseVehicleClass(attrs, "vtype", vtype->id);
        vtype->setParameter |= VTYPEPARS_VEHICLECLASS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_GUIWIDTH)) {
        vtype->width = attrs.getSUMORealReporting(SUMO_ATTR_GUIWIDTH, "vtype", vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_WIDTH_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_GUIOFFSET)) {
        vtype->offset = attrs.getSUMORealReporting(SUMO_ATTR_GUIOFFSET, "vtype", vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_OFFSET_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_GUISHAPE)) {
        vtype->shape = parseGuiShape(attrs, "vtype", vtype->id);
        vtype->setParameter |= VTYPEPARS_SHAPE_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        vtype->color = RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR));
        vtype->setParameter |= VTYPEPARS_COLOR_SET;
    } else {
        vtype->color = RGBColor(1,1,0);
    }
    /*
    if (attrs.hasAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL)) {
        vtype->cfModel = attrs.getStringReporting(SUMO_ATTR_CAR_FOLLOW_MODEL, "vtype", vtype->id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_LANE_CHANGE_MODEL)) {
        vtype->lcModel = attrs.getStringReporting(SUMO_ATTR_LANE_CHANGE_MODEL, "vtype", vtype->id.c_str(), ok);
    }
    */
    if (attrs.hasAttribute(SUMO_ATTR_PROB)) {
        vtype->defaultProbability = attrs.getSUMORealReporting(SUMO_ATTR_PROB, "vtype", vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_PROBABILITY_SET;
    }
    try {
        parseVTypeEmbedded(*vtype, SUMO_TAG_CF_KRAUSS, attrs, true);
    } catch (ProcessError &) {
        throw;
    }
    if (!ok) {
        throw ProcessError();
    }
    return vtype;
}


void
SUMOVehicleParserHelper::parseVTypeEmbedded(SUMOVTypeParameter &into,
        int element, const SUMOSAXAttributes &attrs,
        bool fromVType) throw(ProcessError) {
    switch (element) {
    case SUMO_TAG_CF_KRAUSS:
        parseVTypeEmbedded_Krauss(into, attrs, fromVType);
        break;
    case SUMO_TAG_CF_IDM:
        parseVTypeEmbedded_IDM(into, attrs);
        break;
    }
    if (!fromVType) {
        into.cfModel = element;
    }
}


void
SUMOVehicleParserHelper::parseVTypeEmbedded_Krauss(SUMOVTypeParameter &into,
        const SUMOSAXAttributes &attrs,
        bool fromVType) throw(ProcessError) {
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_ACCEL)) {
        into.cfParameter["accel"] = attrs.getSUMORealReporting(SUMO_ATTR_ACCEL, "krauss", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_DECEL)) {
        into.cfParameter["decel"] = attrs.getSUMORealReporting(SUMO_ATTR_DECEL, "krauss", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_SIGMA)) {
        into.cfParameter["sigma"] = attrs.getSUMORealReporting(SUMO_ATTR_SIGMA, "krauss", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_TAU)) {
        into.cfParameter["tau"] = attrs.getSUMORealReporting(SUMO_ATTR_TAU, "krauss", into.id.c_str(), ok);
    }
    if (!ok) {
        throw ProcessError();
    }
}


void
SUMOVehicleParserHelper::parseVTypeEmbedded_IDM(SUMOVTypeParameter &into,
        const SUMOSAXAttributes &attrs) throw(ProcessError) {
    bool ok = true;
    // !!! the next should be revisited!
    if (attrs.hasAttribute(SUMO_ATTR_ACCEL)) {
        into.cfParameter["accel"] = attrs.getSUMORealReporting(SUMO_ATTR_ACCEL, "IDM", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_DECEL)) {
        into.cfParameter["decel"] = attrs.getSUMORealReporting(SUMO_ATTR_DECEL, "IDM", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_SIGMA)) {
        into.cfParameter["sigma"] = attrs.getSUMORealReporting(SUMO_ATTR_SIGMA, "IDM", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_TAU)) {
        into.cfParameter["tau"] = attrs.getSUMORealReporting(SUMO_ATTR_TAU, "IDM", into.id.c_str(), ok);
    }
    // !!! the prior should be revisited!


    if (attrs.hasAttribute(SUMO_ATTR_CF_IDM_TIMEHEADWAY)) {
        into.cfParameter["timeHeadWay"] = attrs.getSUMORealReporting(SUMO_ATTR_CF_IDM_TIMEHEADWAY, "IDM", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_CF_IDM_MINGAP)) {
        into.cfParameter["minGap"] = attrs.getSUMORealReporting(SUMO_ATTR_CF_IDM_MINGAP, "IDM", into.id.c_str(), ok);
    }
    if (!ok) {
        throw ProcessError();
    }
}


SUMOVehicleClass
SUMOVehicleParserHelper::parseVehicleClass(const SUMOSAXAttributes &attrs,
        const std::string &type,
        const std::string &id) throw() {
    SUMOVehicleClass vclass = SVC_UNKNOWN;
    try {
        std::string vclassS = attrs.getStringSecure(SUMO_ATTR_VCLASS, "");
        if (vclassS=="") {
            return vclass;
        }
        return getVehicleClassID(vclassS);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("The class for " + type + " '" + id + "' is not known.");
    }
    return vclass;
}


SUMOEmissionClass
SUMOVehicleParserHelper::parseEmissionClass(const SUMOSAXAttributes &attrs,
        const std::string &type,
        const std::string &id) throw() {
    SUMOEmissionClass vclass = SVE_UNKNOWN;
    try {
        std::string vclassS = attrs.getStringSecure(SUMO_ATTR_EMISSIONCLASS, "");
        if (vclassS=="") {
            return vclass;
        }
        return getVehicleEmissionTypeID(vclassS);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("The emission class for " + type + " '" + id + "' is not known.");
    }
    return vclass;
}


SUMOVehicleShape
SUMOVehicleParserHelper::parseGuiShape(const SUMOSAXAttributes &attrs,
                                       const std::string &type,
                                       const std::string &id) throw() {
    SUMOVehicleShape vclass = SVS_UNKNOWN;
    try {
        std::string vclassS = attrs.getStringSecure(SUMO_ATTR_GUISHAPE, "");
        if (vclassS=="") {
            return vclass;
        }
        return getVehicleShapeID(vclassS);
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("The shape for " + type + " '" + id + "' is not known.");
    }
    return vclass;
}


/****************************************************************************/

