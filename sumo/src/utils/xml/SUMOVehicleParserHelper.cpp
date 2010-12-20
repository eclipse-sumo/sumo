/****************************************************************************/
/// @file    SUMOVehicleParserHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 07.04.2008
/// @version $Id$
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright 2001-2010 DLR (http://www.dlr.de/) and contributors
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
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) && attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        throw ProcessError("At most one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                           "' and '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                           "' has to be given in the definition of flow '" + id + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) || attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        if (attrs.hasAttribute(SUMO_ATTR_END) && attrs.hasAttribute(SUMO_ATTR_NO)) {
            throw ProcessError("If '" + attrs.getName(SUMO_ATTR_PERIOD) +
                               "' or '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                               "' are given at most one of '" + attrs.getName(SUMO_ATTR_END) +
                               "' and '" + attrs.getName(SUMO_ATTR_NO) +
                               "' are allowed in flow '" + id + "'.");
        }
    } else {
        if (!attrs.hasAttribute(SUMO_ATTR_NO)) {
            throw ProcessError("At least one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                               "', '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                               "', and '" + attrs.getName(SUMO_ATTR_NO) +
                               "' is needed in flow '" + id + "'.");
        }
    }
    bool ok = true;
    SUMOVehicleParameter *ret = new SUMOVehicleParameter();
    ret->id = id;
    parseCommonAttributes(attrs, ret, "flow");

    // parse repetition information
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
#ifdef HAVE_SUBSECOND_TIMESTEPS
        ret->repetitionOffset = attrs.getSUMOTimeReporting(SUMO_ATTR_PERIOD, "flow", id.c_str(), ok);
#else
        ret->repetitionOffset = attrs.getSUMORealReporting(SUMO_ATTR_PERIOD, "flow", id.c_str(), ok);
#endif
    }
    if (attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        const SUMOReal vph = attrs.getSUMORealReporting(SUMO_ATTR_VEHSPERHOUR, "flow", id.c_str(), ok);
        if (ok && vph <= 0) {
            delete ret;
            throw ProcessError("Invalid repetition rate in the definition of flow '" + id + "'.");
        }
        if (ok && vph != 0) {
            ret->repetitionOffset = TIME2STEPS(3600./vph);
        }
    }

    ret->depart = string2time(OptionsCont::getOptions().getString("begin"));
    if (attrs.hasAttribute(SUMO_ATTR_BEGIN)) {
        ret->depart = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, "flow", id.c_str(), ok);
    }
    if (ok && ret->depart < 0) {
        delete ret;
        throw ProcessError("Negative begin time in the definition of flow '" + id + "'.");
    }
    SUMOTime end = string2time(OptionsCont::getOptions().getString("end"));
    if (end<0) {
        end=SUMOTime_MAX;
    }
    if (attrs.hasAttribute(SUMO_ATTR_END)) {
        end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, "flow", id.c_str(), ok);
    }
    if (ok && end <= ret->depart) {
        delete ret;
        throw ProcessError("Flow '" + id + "' ends before or at its begin time.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_NO)) {
        ret->repetitionNumber = attrs.getIntReporting(SUMO_ATTR_NO, "flow", id.c_str(), ok);
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        if (ok && ret->repetitionNumber < 0) {
            delete ret;
            throw ProcessError("Negative repetition number in the definition of flow '" + id + "'.");
        }
        if (ok && ret->repetitionOffset < 0) {
            ret->repetitionOffset = (end - ret->depart) / ret->repetitionNumber;
        }
    } else {
        if (ok && ret->repetitionOffset <= 0) {
            delete ret;
            throw ProcessError("Invalid repetition rate in the definition of flow '" + id + "'.");
        }
        if (end == SUMOTime_MAX) {
            ret->repetitionNumber = INT_MAX;
        } else {
            ret->repetitionNumber = static_cast<int>(static_cast<SUMOReal>(end - ret->depart) / ret->repetitionOffset + 0.5);
        }
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
    std::string id, errorMsg;
    if (!skipID && !attrs.setIDFromAttributes("vehicle", id)) {
        throw ProcessError();
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) ^ attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        throw ProcessError("The attributes '" + attrs.getName(SUMO_ATTR_PERIOD) + "' and '" + attrs.getName(SUMO_ATTR_REPNUMBER) + "' have to be given both in the definition of '" + id + "'.");
    }
    SUMOVehicleParameter *ret = new SUMOVehicleParameter();
    ret->id = id;
    try {
        parseCommonAttributes(attrs, ret, "vehicle");
    } catch (ProcessError &) {
        delete ret;
        throw;
    }
    bool ok = true;
    if (!skipDepart) {
        const std::string helper = attrs.getStringReporting(SUMO_ATTR_DEPART, "vehicle", 0, ok);
        if (helper=="triggered") {
            ret->departProcedure = DEPART_TRIGGERED;
        } else {
            ret->departProcedure = DEPART_GIVEN;
            ret->depart = attrs.getSUMOTimeReporting(SUMO_ATTR_DEPART, "vehicle", id.c_str(), ok);
            if (ok && ret->depart < 0) {
                errorMsg = "Negative departure time in the definition of '" + id + "'.";
                ok = false;
            }
        }
    }
    // parse repetition information
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        WRITE_WARNING("period and repno are deprecated in vehicle '" + id + "', use flows instead.");
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
#ifdef HAVE_SUBSECOND_TIMESTEPS
        ret->repetitionOffset = attrs.getSUMOTimeReporting(SUMO_ATTR_PERIOD, "vehicle", id.c_str(), ok);
#else
        ret->repetitionOffset = attrs.getSUMORealReporting(SUMO_ATTR_PERIOD, "vehicle", id.c_str(), ok);
#endif
    }
    if (attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        ret->setParameter |= VEHPARS_PERIODNUM_SET;
        ret->repetitionNumber = attrs.getIntReporting(SUMO_ATTR_REPNUMBER, "vehicle", id.c_str(), ok);
    }

    if (!ok) {
        delete ret;
        throw ProcessError(errorMsg);
    }
    return ret;
}


void
SUMOVehicleParserHelper::parseCommonAttributes(const SUMOSAXAttributes &attrs,
        SUMOVehicleParameter *ret, std::string element) throw(ProcessError) {
    //ret->refid = attrs.getStringSecure(SUMO_ATTR_REFID, "");
    bool ok = true;
    // parse route information
    if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        ret->setParameter |= VEHPARS_ROUTE_SET; // !!! needed?
        ret->routeid = attrs.getStringReporting(SUMO_ATTR_ROUTE, "vehicle", 0, ok);
    }
    // parse type information
    if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        ret->setParameter |= VEHPARS_VTYPE_SET; // !!! needed?
        ret->vtypeid = attrs.getStringReporting(SUMO_ATTR_TYPE, "vehicle", 0, ok);
    }
    // parse line information
    if (attrs.hasAttribute(SUMO_ATTR_LINE)) {
        ret->setParameter |= VEHPARS_LINE_SET; // !!! needed?
        ret->line = attrs.getStringReporting(SUMO_ATTR_LINE, "vehicle", 0, ok);
    }
    // parse zone information
    if (attrs.hasAttribute(SUMO_ATTR_FROM_TAZ) && attrs.hasAttribute(SUMO_ATTR_TO_TAZ)) {
        ret->setParameter |= VEHPARS_TAZ_SET;
        ret->fromTaz = attrs.getStringReporting(SUMO_ATTR_FROM_TAZ, "vehicle", 0, ok);
        ret->toTaz = attrs.getStringReporting(SUMO_ATTR_TO_TAZ, "vehicle", 0, ok);
    }

    // parse depart lane information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
        ret->setParameter |= VEHPARS_DEPARTLANE_SET;
        const std::string helper = attrs.getStringReporting(SUMO_ATTR_DEPARTLANE, "vehicle", 0, ok);
        if (helper=="departlane") {
            ret->departLaneProcedure = DEPART_LANE_DEPARTLANE;
        } else if (helper=="random") {
            ret->departLaneProcedure = DEPART_LANE_RANDOM;
        } else if (helper=="free") {
            ret->departLaneProcedure = DEPART_LANE_FREE;
        } else if (helper=="allowed") {
            ret->departLaneProcedure = DEPART_LANE_ALLOWED_FREE;
        } else if (helper=="best") {
            ret->departLaneProcedure = DEPART_LANE_BEST_FREE;
        } else {
            try {
                ret->departLane = TplConvert<char>::_2int(helper.c_str());
                ret->departLaneProcedure = DEPART_LANE_GIVEN;
                if (ret->departLane < 0) {
                    throw ProcessError("Invalid departlane definition for " + element + " '" + ret->id + "'");
                }
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
        const std::string helper = attrs.getStringReporting(SUMO_ATTR_DEPARTPOS, "vehicle", 0, ok);
        if (helper=="random") {
            ret->departPosProcedure = DEPART_POS_RANDOM;
        } else if (helper=="random_free") {
            ret->departPosProcedure = DEPART_POS_RANDOM_FREE;
        } else if (helper=="free") {
            ret->departPosProcedure = DEPART_POS_FREE;
        } else if (helper=="base") {
            ret->departPosProcedure = DEPART_POS_BASE;
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
        std::string helper = attrs.getStringReporting(SUMO_ATTR_DEPARTSPEED, "vehicle", 0, ok);
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
        std::string helper = attrs.getStringReporting(SUMO_ATTR_ARRIVALLANE, "vehicle", 0, ok);
        if (helper=="current") {
            ret->arrivalLaneProcedure = ARRIVAL_LANE_CURRENT;
        } else {
            try {
                ret->arrivalLane = TplConvert<char>::_2int(helper.c_str());
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
        std::string helper = attrs.getStringReporting(SUMO_ATTR_ARRIVALPOS, "vehicle", 0, ok);
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
        std::string helper = attrs.getStringReporting(SUMO_ATTR_ARRIVALSPEED, "vehicle", 0, ok);
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
        try {
            ret->color = RGBColor::parseColor(attrs.getStringReporting(SUMO_ATTR_COLOR, "vehicle", 0, ok));
        } catch (NumberFormatException &) {
            throw ProcessError("Invalid color definition for " + element + " '" + ret->id + "'");
        } catch (EmptyData &) {
            throw ProcessError("Invalid color definition for " + element + " '" + ret->id + "'");
        }
    } else {
        ret->color = RGBColor::DEFAULT_COLOR;
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
        vtype->color = RGBColor::parseColorReporting(attrs.getString(SUMO_ATTR_COLOR), "vtype", vtype->id.c_str(), true, ok);
        vtype->setParameter |= VTYPEPARS_COLOR_SET;
    } else {
        vtype->color = RGBColor(1,1,0);
    }
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
    case SUMO_TAG_CF_KRAUSS_ORIG1:
        parseVTypeEmbedded_Krauss(into, attrs);
        break;
    case SUMO_TAG_CF_PWAGNER2009:
        parseVTypeEmbedded_Krauss(into, attrs);
        break;
    case SUMO_TAG_CF_BKERNER:
        parseVTypeEmbedded_BKerner(into, attrs);
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
    UNUSED_PARAMETER(fromVType);
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
    if (attrs.hasAttribute(SUMO_ATTR_ACCEL)) {
        into.cfParameter["accel"] = attrs.getSUMORealReporting(SUMO_ATTR_ACCEL, "IDM", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_DECEL)) {
        into.cfParameter["decel"] = attrs.getSUMORealReporting(SUMO_ATTR_DECEL, "IDM", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_TAU)) {
        into.cfParameter["tau"] = attrs.getSUMORealReporting(SUMO_ATTR_TAU, "IDM", into.id.c_str(), ok);
    }
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


void
SUMOVehicleParserHelper::parseVTypeEmbedded_BKerner(SUMOVTypeParameter &into,
        const SUMOSAXAttributes &attrs) throw(ProcessError) {
    bool ok = true;
    if (attrs.hasAttribute(SUMO_ATTR_ACCEL)) {
        into.cfParameter["accel"] = attrs.getSUMORealReporting(SUMO_ATTR_ACCEL, "BKerner", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_DECEL)) {
        into.cfParameter["decel"] = attrs.getSUMORealReporting(SUMO_ATTR_DECEL, "BKerner", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_TAU)) {
        into.cfParameter["tau"] = attrs.getSUMORealReporting(SUMO_ATTR_TAU, "BKerner", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_K)) {
        into.cfParameter["k"] = attrs.getSUMORealReporting(SUMO_ATTR_K, "BKerner", into.id.c_str(), ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_CF_KERNER_PHI)) {
        into.cfParameter["phi"] = attrs.getSUMORealReporting(SUMO_ATTR_CF_KERNER_PHI, "BKerner", into.id.c_str(), ok);
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
        bool ok = true;
        std::string vclassS = attrs.getOptStringReporting(SUMO_ATTR_VCLASS, type.c_str(), id.c_str(), ok, "");
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
        bool ok = true;
        std::string vclassS = attrs.getOptStringReporting(SUMO_ATTR_EMISSIONCLASS, type.c_str(), id.c_str(), ok, "");
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
        bool ok = true;
        std::string vclassS = attrs.getOptStringReporting(SUMO_ATTR_GUISHAPE, type.c_str(), id.c_str(), ok, "");
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

