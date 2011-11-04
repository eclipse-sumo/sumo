/****************************************************************************/
/// @file    SUMOVehicleParserHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 07.04.2008
/// @version $Id$
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
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

#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/SUMOVehicleParameter.h>
#include <utils/options/OptionsCont.h>
#include "SUMOVehicleParserHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
SUMOVehicleParserHelper::CFAttrMap SUMOVehicleParserHelper::allowedCFModelAttrs;

bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedNumber = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedTazs = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedDepartLane = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedDepartPos = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedDepartSpeed = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedArrivalLane = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedArrivalPos = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedArrivalSpeed = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedMaxSpeed = false;
bool SUMOVehicleParserHelper::gHaveWarnedAboutDeprecatedVClass = false;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOVehicleParameter*
SUMOVehicleParserHelper::parseFlowAttributes(const SUMOSAXAttributes& attrs) {
    bool ok = true;
    std::string id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) && attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        throw ProcessError("At most one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                           "' and '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                           "' has to be given in the definition of flow '" + id + "'.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) || attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        if (attrs.hasAttribute(SUMO_ATTR_END) && (attrs.hasAttribute(SUMO_ATTR_NO__DEPRECATED) || attrs.hasAttribute(SUMO_ATTR_NUMBER))) {
            throw ProcessError("If '" + attrs.getName(SUMO_ATTR_PERIOD) +
                               "' or '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                               "' are given at most one of '" + attrs.getName(SUMO_ATTR_END) +
                               "' and '" + attrs.getName(SUMO_ATTR_NUMBER) +
                               "' are allowed in flow '" + id + "'.");
        }
    } else {
        if (!attrs.hasAttribute(SUMO_ATTR_NO__DEPRECATED) && !attrs.hasAttribute(SUMO_ATTR_NUMBER)) {
            throw ProcessError("At least one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                               "', '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                               "', and '" + attrs.getName(SUMO_ATTR_NUMBER) +
                               "' is needed in flow '" + id + "'.");
        }
    }
    SUMOVehicleParameter* ret = new SUMOVehicleParameter();
    ret->id = id;
    parseCommonAttributes(attrs, ret, "flow");

    // parse repetition information
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
#ifdef HAVE_SUBSECOND_TIMESTEPS
        ret->repetitionOffset = attrs.getSUMOTimeReporting(SUMO_ATTR_PERIOD, id.c_str(), ok);
#else
        ret->repetitionOffset = attrs.getSUMORealReporting(SUMO_ATTR_PERIOD, id.c_str(), ok);
#endif
    }
    if (attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        const SUMOReal vph = attrs.getSUMORealReporting(SUMO_ATTR_VEHSPERHOUR, id.c_str(), ok);
        if (ok && vph <= 0) {
            delete ret;
            throw ProcessError("Invalid repetition rate in the definition of flow '" + id + "'.");
        }
        if (ok && vph != 0) {
            ret->repetitionOffset = TIME2STEPS(3600. / vph);
        }
    }

    ret->depart = string2time(OptionsCont::getOptions().getString("begin"));
    if (attrs.hasAttribute(SUMO_ATTR_BEGIN)) {
        ret->depart = attrs.getSUMOTimeReporting(SUMO_ATTR_BEGIN, id.c_str(), ok);
    }
    if (ok && ret->depart < 0) {
        delete ret;
        throw ProcessError("Negative begin time in the definition of flow '" + id + "'.");
    }
    SUMOTime end = string2time(OptionsCont::getOptions().getString("end"));
    if (end < 0) {
        end = SUMOTime_MAX;
    }
    if (attrs.hasAttribute(SUMO_ATTR_END)) {
        end = attrs.getSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), ok);
    }
    if (ok && end <= ret->depart) {
        delete ret;
        throw ProcessError("Flow '" + id + "' ends before or at its begin time.");
    }
    if (attrs.hasAttribute(SUMO_ATTR_NO__DEPRECATED) || attrs.hasAttribute(SUMO_ATTR_NUMBER)) {
        ret->repetitionNumber = attrs.hasAttribute(SUMO_ATTR_NUMBER)
                                ? attrs.getIntReporting(SUMO_ATTR_NUMBER, id.c_str(), ok)
                                : attrs.getIntReporting(SUMO_ATTR_NO__DEPRECATED, id.c_str(), ok);
        if (!gHaveWarnedAboutDeprecatedNumber && attrs.hasAttribute(SUMO_ATTR_NO__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedNumber = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_NO__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_NUMBER) + "' instead.");
        }
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


SUMOVehicleParameter*
SUMOVehicleParserHelper::parseVehicleAttributes(const SUMOSAXAttributes& attrs,
        bool skipID, bool skipDepart) {
    bool ok = true;
    std::string id, errorMsg;
    if (!skipID) {
        id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) ^ attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        throw ProcessError("The attributes '" + attrs.getName(SUMO_ATTR_PERIOD) +
                           "' and '" + attrs.getName(SUMO_ATTR_REPNUMBER) +
                           "' have to be given both in the definition of '" + id + "'.");
    }
    SUMOVehicleParameter* ret = new SUMOVehicleParameter();
    ret->id = id;
    try {
        parseCommonAttributes(attrs, ret, "vehicle");
    } catch (ProcessError&) {
        delete ret;
        throw;
    }
    if (!skipDepart) {
        const std::string helper = attrs.getStringReporting(SUMO_ATTR_DEPART, 0, ok);
        if (helper == "triggered") {
            ret->departProcedure = DEPART_TRIGGERED;
        } else {
            ret->departProcedure = DEPART_GIVEN;
            ret->depart = attrs.getSUMOTimeReporting(SUMO_ATTR_DEPART, id.c_str(), ok);
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
        ret->repetitionOffset = attrs.getSUMOTimeReporting(SUMO_ATTR_PERIOD, id.c_str(), ok);
#else
        ret->repetitionOffset = attrs.getSUMORealReporting(SUMO_ATTR_PERIOD, id.c_str(), ok);
#endif
    }
    if (attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        ret->setParameter |= VEHPARS_PERIODNUM_SET;
        ret->repetitionNumber = attrs.getIntReporting(SUMO_ATTR_REPNUMBER, id.c_str(), ok);
    }

    if (!ok) {
        delete ret;
        throw ProcessError(errorMsg);
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
        ret->routeid = attrs.getStringReporting(SUMO_ATTR_ROUTE, 0, ok);
    }
    // parse type information
    if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        ret->setParameter |= VEHPARS_VTYPE_SET; // !!! needed?
        ret->vtypeid = attrs.getStringReporting(SUMO_ATTR_TYPE, 0, ok);
    }
    // parse line information
    if (attrs.hasAttribute(SUMO_ATTR_LINE)) {
        ret->setParameter |= VEHPARS_LINE_SET; // !!! needed?
        ret->line = attrs.getStringReporting(SUMO_ATTR_LINE, 0, ok);
    }
    // parse zone information
    if ((attrs.hasAttribute(SUMO_ATTR_FROM_TAZ) || attrs.hasAttribute(SUMO_ATTR_FROM_TAZ__DEPRECATED))
            &&
            (attrs.hasAttribute(SUMO_ATTR_TO_TAZ) || attrs.hasAttribute(SUMO_ATTR_TO_TAZ__DEPRECATED))) {
        ret->setParameter |= VEHPARS_TAZ_SET;
        ret->fromTaz = attrs.hasAttribute(SUMO_ATTR_FROM_TAZ)
                       ? attrs.getStringReporting(SUMO_ATTR_FROM_TAZ, 0, ok)
                       : attrs.getStringReporting(SUMO_ATTR_FROM_TAZ__DEPRECATED, 0, ok);
        ret->toTaz = attrs.hasAttribute(SUMO_ATTR_TO_TAZ)
                     ? attrs.getStringReporting(SUMO_ATTR_TO_TAZ, 0, ok)
                     : attrs.getStringReporting(SUMO_ATTR_TO_TAZ__DEPRECATED, 0, ok);
        if (!gHaveWarnedAboutDeprecatedTazs && (attrs.hasAttribute(SUMO_ATTR_TO_TAZ__DEPRECATED) || attrs.hasAttribute(SUMO_ATTR_FROM_TAZ__DEPRECATED))) {
            gHaveWarnedAboutDeprecatedTazs = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_FROM_TAZ__DEPRECATED)
                          + "'/'" + toString(SUMO_ATTR_TO_TAZ__DEPRECATED)
                          + "' is deprecated, please use '" + toString(SUMO_ATTR_FROM_TAZ)
                          + "'/'" + toString(SUMO_ATTR_TO_TAZ)
                          + "' instead.");
        }
    }
    // parse reroute information
    if (attrs.getOptBoolReporting(SUMO_ATTR_REROUTE, 0, ok, false)) {
        ret->setParameter |= VEHPARS_FORCE_REROUTE;
    }

    // parse depart lane information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTLANE) || attrs.hasAttribute(SUMO_ATTR_DEPARTLANE__DEPRECATED)) {
        ret->setParameter |= VEHPARS_DEPARTLANE_SET;
        const std::string helper = attrs.hasAttribute(SUMO_ATTR_DEPARTLANE)
                                   ? attrs.getStringReporting(SUMO_ATTR_DEPARTLANE, 0, ok)
                                   : attrs.getStringReporting(SUMO_ATTR_DEPARTLANE__DEPRECATED, 0, ok);
        if (!gHaveWarnedAboutDeprecatedDepartLane && attrs.hasAttribute(SUMO_ATTR_DEPARTLANE__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedDepartLane = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_DEPARTLANE__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_DEPARTLANE) + "' instead.");
        }
        if (helper == "random") {
            ret->departLaneProcedure = DEPART_LANE_RANDOM;
        } else if (helper == "free") {
            ret->departLaneProcedure = DEPART_LANE_FREE;
        } else if (helper == "allowed") {
            ret->departLaneProcedure = DEPART_LANE_ALLOWED_FREE;
        } else if (helper == "best") {
            ret->departLaneProcedure = DEPART_LANE_BEST_FREE;
        } else {
            try {
                ret->departLane = TplConvert<char>::_2int(helper.c_str());
                ret->departLaneProcedure = DEPART_LANE_GIVEN;
                if (ret->departLane < 0) {
                    throw ProcessError("Invalid departLane definition for " + element + " '" + ret->id + "'");
                }
            } catch (NumberFormatException&) {
                throw ProcessError("Invalid departLane definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData&) {
                throw ProcessError("Invalid departLane definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS) || attrs.hasAttribute(SUMO_ATTR_DEPARTPOS__DEPRECATED)) {
        ret->setParameter |= VEHPARS_DEPARTPOS_SET;
        const std::string helper = attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)
                                   ? attrs.getStringReporting(SUMO_ATTR_DEPARTPOS, 0, ok)
                                   : attrs.getStringReporting(SUMO_ATTR_DEPARTPOS__DEPRECATED, 0, ok);
        if (!gHaveWarnedAboutDeprecatedDepartPos && attrs.hasAttribute(SUMO_ATTR_DEPARTPOS__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedDepartPos = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_DEPARTPOS__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_DEPARTPOS) + "' instead.");
        }
        if (helper == "random") {
            ret->departPosProcedure = DEPART_POS_RANDOM;
        } else if (helper == "random_free") {
            ret->departPosProcedure = DEPART_POS_RANDOM_FREE;
        } else if (helper == "free") {
            ret->departPosProcedure = DEPART_POS_FREE;
        } else if (helper == "base") {
            ret->departPosProcedure = DEPART_POS_BASE;
        } else if (helper == "pwagSimple") {
            ret->departPosProcedure = DEPART_POS_PWAG_SIMPLE;
        } else if (helper == "pwagGeneric") {
            ret->departPosProcedure = DEPART_POS_PWAG_GENERIC;
        } else if (helper == "maxSpeedGap") {
            ret->departPosProcedure = DEPART_POS_MAX_SPEED_GAP;
        } else {
            try {
                ret->departPos = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->departPosProcedure = DEPART_POS_GIVEN;
            } catch (NumberFormatException&) {
                throw ProcessError("Invalid departPos definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData&) {
                throw ProcessError("Invalid departPos definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse depart speed information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED) || attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED__DEPRECATED)) {
        ret->setParameter |= VEHPARS_DEPARTSPEED_SET;
        std::string helper = attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED)
                             ? attrs.getStringReporting(SUMO_ATTR_DEPARTSPEED, 0, ok)
                             : attrs.getStringReporting(SUMO_ATTR_DEPARTSPEED__DEPRECATED, 0, ok);
        if (!gHaveWarnedAboutDeprecatedDepartSpeed && attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedDepartSpeed = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_DEPARTSPEED__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_DEPARTSPEED) + "' instead.");
        }
        if (helper == "random") {
            ret->departSpeedProcedure = DEPART_SPEED_RANDOM;
        } else if (helper == "max") {
            ret->departSpeedProcedure = DEPART_SPEED_MAX;
        } else {
            try {
                ret->departSpeed = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->departSpeedProcedure = DEPART_SPEED_GIVEN;
            } catch (NumberFormatException&) {
                throw ProcessError("Invalid departSpeed definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData&) {
                throw ProcessError("Invalid departSpeed definition for " + element + " '" + ret->id + "'");
            }
        }
    }

    // parse arrival lane information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE) || attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE__DEPRECATED)) {
        ret->setParameter |= VEHPARS_ARRIVALLANE_SET;
        std::string helper = attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE)
                             ? attrs.getStringReporting(SUMO_ATTR_ARRIVALLANE, 0, ok)
                             : attrs.getStringReporting(SUMO_ATTR_ARRIVALLANE__DEPRECATED, 0, ok);
        if (!gHaveWarnedAboutDeprecatedArrivalLane && attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedArrivalLane = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_ARRIVALLANE__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_ARRIVALLANE) + "' instead.");
        }
        if (helper == "current") {
            ret->arrivalLaneProcedure = ARRIVAL_LANE_CURRENT;
        } else {
            try {
                ret->arrivalLane = TplConvert<char>::_2int(helper.c_str());
                ret->arrivalLaneProcedure = ARRIVAL_LANE_GIVEN;
            } catch (NumberFormatException&) {
                throw ProcessError("Invalid arrivalLane definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData&) {
                throw ProcessError("Invalid arrivalLane definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS) || attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS__DEPRECATED)) {
        ret->setParameter |= VEHPARS_ARRIVALPOS_SET;
        std::string helper = attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)
                             ? attrs.getStringReporting(SUMO_ATTR_ARRIVALPOS, 0, ok)
                             : attrs.getStringReporting(SUMO_ATTR_ARRIVALPOS__DEPRECATED, 0, ok);
        if (!gHaveWarnedAboutDeprecatedArrivalPos && attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedArrivalPos = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_ARRIVALPOS__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_ARRIVALPOS) + "' instead.");
        }
        if (helper == "random") {
            ret->arrivalPosProcedure = ARRIVAL_POS_RANDOM;
        } else if (helper == "max") {
            ret->arrivalPosProcedure = ARRIVAL_POS_MAX;
        } else {
            try {
                ret->arrivalPos = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->arrivalPosProcedure = ARRIVAL_POS_GIVEN;
            } catch (NumberFormatException&) {
                throw ProcessError("Invalid arrivalPos definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData&) {
                throw ProcessError("Invalid arrivalPos definition for " + element + " '" + ret->id + "'");
            }
        }
    }
    // parse arrival speed information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED) || attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED__DEPRECATED)) {
        ret->setParameter |= VEHPARS_ARRIVALSPEED_SET;
        std::string helper = attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED)
                             ? attrs.getStringReporting(SUMO_ATTR_ARRIVALSPEED, 0, ok)
                             : attrs.getStringReporting(SUMO_ATTR_ARRIVALSPEED__DEPRECATED, 0, ok);
        if (!gHaveWarnedAboutDeprecatedArrivalSpeed && attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedArrivalSpeed = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_ARRIVALSPEED__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_ARRIVALSPEED) + "' instead.");
        }
        if (helper == "current") {
            ret->arrivalSpeedProcedure = ARRIVAL_SPEED_CURRENT;
        } else {
            try {
                ret->arrivalSpeed = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->arrivalSpeedProcedure = ARRIVAL_SPEED_GIVEN;
            } catch (NumberFormatException&) {
                throw ProcessError("Invalid arrivalSpeed definition for " + element + " '" + ret->id + "'");
            } catch (EmptyData&) {
                throw ProcessError("Invalid arrivalSpeed definition for " + element + " '" + ret->id + "'");
            }
        }
    }

    // parse color
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        ret->setParameter |= VEHPARS_COLOR_SET;
        try {
            ret->color = RGBColor::parseColor(attrs.getStringReporting(SUMO_ATTR_COLOR, 0, ok));
        } catch (NumberFormatException&) {
            throw ProcessError("Invalid color definition for " + element + " '" + ret->id + "'");
        } catch (EmptyData&) {
            throw ProcessError("Invalid color definition for " + element + " '" + ret->id + "'");
        }
    } else {
        ret->color = RGBColor::DEFAULT_COLOR;
    }
}


SUMOVTypeParameter*
SUMOVehicleParserHelper::beginVTypeParsing(const SUMOSAXAttributes& attrs) {
    SUMOVTypeParameter* vtype = new SUMOVTypeParameter();
    bool ok = true;
    vtype->id = attrs.getStringReporting(SUMO_ATTR_ID, 0, ok);
    if (attrs.hasAttribute(SUMO_ATTR_LENGTH)) {
        if (!attrs.hasAttribute(SUMO_ATTR_MINGAP)) {
            WRITE_WARNING("The length does not include the gap to the preceeding vehicle anymore! Please recheck your values.");
        }
        vtype->length = attrs.getSUMORealReporting(SUMO_ATTR_LENGTH, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_LENGTH_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_MINGAP)) {
        vtype->minGap = attrs.getSUMORealReporting(SUMO_ATTR_MINGAP, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MINGAP_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_GUIOFFSET)) {
        WRITE_WARNING("The guiOffset attribute is deprecated! Please use minGap instead.");
        vtype->minGap = attrs.getSUMORealReporting(SUMO_ATTR_GUIOFFSET, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MINGAP_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED)) {
        vtype->maxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MAXSPEED_SET;
    } else if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED__DEPRECATED)) {
        vtype->maxSpeed = attrs.getSUMORealReporting(SUMO_ATTR_MAXSPEED__DEPRECATED, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_MAXSPEED_SET;
        if (!gHaveWarnedAboutDeprecatedMaxSpeed) {
            gHaveWarnedAboutDeprecatedMaxSpeed = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_MAXSPEED__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_MAXSPEED) + "' instead.");
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPEEDFACTOR)) {
        vtype->speedFactor = attrs.getSUMORealReporting(SUMO_ATTR_SPEEDFACTOR, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_SPEEDFACTOR_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_SPEEDDEV)) {
        vtype->speedDev = attrs.getSUMORealReporting(SUMO_ATTR_SPEEDDEV, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_SPEEDDEVIATION_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_EMISSIONCLASS)) {
        vtype->emissionClass = parseEmissionClass(attrs, vtype->id);
        vtype->setParameter |= VTYPEPARS_EMISSIONCLASS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_VCLASS) || attrs.hasAttribute(SUMO_ATTR_VCLASS__DEPRECATED)) {
        if (!gHaveWarnedAboutDeprecatedVClass && attrs.hasAttribute(SUMO_ATTR_VCLASS__DEPRECATED)) {
            gHaveWarnedAboutDeprecatedVClass = true;
            WRITE_WARNING("'" + toString(SUMO_ATTR_VCLASS__DEPRECATED) + "' is deprecated, please use '" + toString(SUMO_ATTR_VCLASS) + "' instead.");
        }
        vtype->vehicleClass = parseVehicleClass(attrs, vtype->id);
        vtype->setParameter |= VTYPEPARS_VEHICLECLASS_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_GUIWIDTH)) {
        vtype->width = attrs.getSUMORealReporting(SUMO_ATTR_GUIWIDTH, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_WIDTH_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_GUISHAPE)) {
        vtype->shape = parseGuiShape(attrs, vtype->id);
        vtype->setParameter |= VTYPEPARS_SHAPE_SET;
    }
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        vtype->color = RGBColor::parseColorReporting(attrs.getString(SUMO_ATTR_COLOR), attrs.getObjectType(), vtype->id.c_str(), true, ok);
        vtype->setParameter |= VTYPEPARS_COLOR_SET;
    } else {
        vtype->color = RGBColor(1, 1, 0);
    }
    if (attrs.hasAttribute(SUMO_ATTR_PROB)) {
        vtype->defaultProbability = attrs.getSUMORealReporting(SUMO_ATTR_PROB, vtype->id.c_str(), ok);
        vtype->setParameter |= VTYPEPARS_PROBABILITY_SET;
    }
    try {
        parseVTypeEmbedded(*vtype, SUMO_TAG_CF_KRAUSS, attrs, true);
    } catch (ProcessError&) {
        throw;
    }
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
            WRITE_WARNING("Unknown cfmodel " + toString((SumoXMLTag)element) + " when parsing vtype '" + into.id + "'");
        } else {
            WRITE_WARNING("Unknown cfmodel when parsing vtype '" + into.id + "'");
        }
    }
    if (!fromVType) {
        into.cfModel = cf_it->first;
    }
    bool ok = true;
    for (std::set<SumoXMLAttr>::const_iterator it = cf_it->second.begin(); it != cf_it->second.end(); it++) {
        if (attrs.hasAttribute(*it)) {
            into.cfParameter[*it] = attrs.getSUMORealReporting(*it, into.id.c_str(), ok);
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


SUMOVehicleClass
SUMOVehicleParserHelper::parseVehicleClass(const SUMOSAXAttributes& attrs,
        const std::string& id) {
    SUMOVehicleClass vclass = SVC_UNKNOWN;
    try {
        bool ok = true;
        std::string vclassS = attrs.hasAttribute(SUMO_ATTR_VCLASS)
                              ? attrs.getOptStringReporting(SUMO_ATTR_VCLASS, id.c_str(), ok, "")
                              : attrs.getOptStringReporting(SUMO_ATTR_VCLASS__DEPRECATED, id.c_str(), ok, "");
        if (vclassS == "") {
            return vclass;
        }
        return getVehicleClassID(vclassS);
    } catch (...) {
        WRITE_ERROR("The class for " + attrs.getObjectType() + " '" + id + "' is not known.");
    }
    return vclass;
}


SUMOEmissionClass
SUMOVehicleParserHelper::parseEmissionClass(const SUMOSAXAttributes& attrs, const std::string& id) {
    SUMOEmissionClass vclass = SVE_UNKNOWN;
    try {
        bool ok = true;
        std::string vclassS = attrs.getOptStringReporting(SUMO_ATTR_EMISSIONCLASS, id.c_str(), ok, "");
        if (vclassS == "") {
            return vclass;
        }
        return getVehicleEmissionTypeID(vclassS);
    } catch (...) {
        WRITE_ERROR("The emission class for " + attrs.getObjectType() + " '" + id + "' is not known.");
    }
    return vclass;
}


SUMOVehicleShape
SUMOVehicleParserHelper::parseGuiShape(const SUMOSAXAttributes& attrs, const std::string& id) {
    bool ok = true;
    std::string vclassS = attrs.getOptStringReporting(SUMO_ATTR_GUISHAPE, id.c_str(), ok, "");
    if (SumoVehicleShapeStrings.hasString(vclassS)) {
        return SumoVehicleShapeStrings.get(vclassS);
    } else {
        WRITE_ERROR("The shape '" + vclassS + "' for " + attrs.getObjectType() + " '" + id + "' is not known.");
        return SVS_UNKNOWN;
    }
}


/****************************************************************************/

