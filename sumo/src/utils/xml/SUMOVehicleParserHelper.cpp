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
#include "SUMOVehicleParserHelper.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOVehicleParameter *
SUMOVehicleParserHelper::parseVehicleAttributes(const SUMOSAXAttributes &attrs,
        bool skipID, bool skipDepart) throw(ProcessError) {
    string id;
    if (!skipID && !attrs.setIDFromAttributes("vehicle", id)) {
        throw ProcessError();
    }
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD) ^ attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        throw ProcessError("The attributes '" + attrs.getName(SUMO_ATTR_PERIOD) + "' and '" + attrs.getName(SUMO_ATTR_REPNUMBER) + "' have to be given both in the definition of '" + id + "'.");
    }
    bool ok = true;
    SUMOVehicleParameter *ret = new SUMOVehicleParameter();
    ret->id = id;
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

    if (!skipDepart) {
        ret->depart = attrs.getIntReporting(SUMO_ATTR_DEPART, "vehicle", id.c_str(), ok);
    }
    // parse depart lane information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
        ret->setParameter |= VEHPARS_DEPARTLANE_SET;
        string helper = attrs.getString(SUMO_ATTR_DEPARTLANE);
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
            } catch(NumberFormatException &) {
                throw ProcessError("Invalid departlane definition for vehicle '" + ret->id + "'");
            } catch(EmptyData &) {
                throw ProcessError("Invalid departlane definition for vehicle '" + ret->id + "'");
            }
        }
    }
    // parse depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
        ret->setParameter |= VEHPARS_DEPARTPOS_SET;
        string helper = attrs.getString(SUMO_ATTR_DEPARTPOS);
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
            } catch(NumberFormatException &) {
                throw ProcessError("Invalid departpos definition for vehicle '" + ret->id + "'");
            } catch(EmptyData &) {
                throw ProcessError("Invalid departpos definition for vehicle '" + ret->id + "'");
            }
        }
    }
    // parse depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED)) {
        ret->setParameter |= VEHPARS_DEPARTSPEED_SET;
        string helper = attrs.getString(SUMO_ATTR_DEPARTSPEED);
        if (helper=="random") {
            ret->departSpeedProcedure = DEPART_SPEED_RANDOM;
        } else if (helper=="max") {
            ret->departSpeedProcedure = DEPART_SPEED_MAX;
        } else {
            try {
                ret->departSpeed = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->departSpeedProcedure = DEPART_SPEED_GIVEN;
            } catch(NumberFormatException &) {
                throw ProcessError("Invalid departspeed definition for vehicle '" + ret->id + "'");
            } catch(EmptyData &) {
                throw ProcessError("Invalid departspeed definition for vehicle '" + ret->id + "'");
            }
        }
    }

    // parse arrival lane information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
        ret->setParameter |= VEHPARS_ARRIVALLANE_SET;
        string helper = attrs.getString(SUMO_ATTR_ARRIVALLANE);
        if (helper=="current") {
            ret->arrivalLaneProcedure = ARRIVAL_LANE_CURRENT;
        } else {
            try {
                ret->arrivalLane = TplConvert<char>::_2int(helper.c_str());;
                ret->arrivalLaneProcedure = ARRIVAL_LANE_GIVEN;
            } catch(NumberFormatException &) {
                throw ProcessError("Invalid arrivallane definition for vehicle '" + ret->id + "'");
            } catch(EmptyData &) {
                throw ProcessError("Invalid arrivallane definition for vehicle '" + ret->id + "'");
            }
        }
    }
    // parse arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
        ret->setParameter |= VEHPARS_ARRIVALPOS_SET;
        string helper = attrs.getString(SUMO_ATTR_ARRIVALPOS);
        if (helper=="random") {
            ret->arrivalPosProcedure = ARRIVAL_POS_RANDOM;
        } else if (helper=="max") {
            ret->arrivalPosProcedure = ARRIVAL_POS_MAX;
        } else {
            try {
                ret->arrivalPos = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->arrivalPosProcedure = ARRIVAL_POS_GIVEN;
            } catch(NumberFormatException &) {
                throw ProcessError("Invalid arrivalpos definition for vehicle '" + ret->id + "'");
            } catch(EmptyData &) {
                throw ProcessError("Invalid arrivalpos definition for vehicle '" + ret->id + "'");
            }
        }
    }
    // parse arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED)) {
        ret->setParameter |= VEHPARS_ARRIVALSPEED_SET;
        string helper = attrs.getString(SUMO_ATTR_ARRIVALSPEED);
        if (helper=="current") {
            ret->arrivalSpeedProcedure = ARRIVAL_SPEED_CURRENT;
        } else {
            try {
                ret->arrivalSpeed = TplConvert<char>::_2SUMOReal(helper.c_str());
                ret->arrivalSpeedProcedure = ARRIVAL_SPEED_GIVEN;
            } catch(NumberFormatException &) {
                throw ProcessError("Invalid arrivalspeed definition for vehicle '" + ret->id + "'");
            } catch(EmptyData &) {
                throw ProcessError("Invalid arrivalspeed definition for vehicle '" + ret->id + "'");
            }
        }
    }

    // parse repetition information
    if (attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        ret->repetitionOffset = attrs.getInt(SUMO_ATTR_PERIOD);
    }
    if (attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        ret->setParameter |= VEHPARS_PERIODNUM_SET;
        ret->repetitionNumber = attrs.getInt(SUMO_ATTR_REPNUMBER);
    }

    // parse color
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        ret->setParameter |= VEHPARS_COLOR_SET;
        ret->color = RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR));
    } else {
        ret->color = RGBColor(1,1,0);
    }

    if (!ok) {
        delete ret;
        throw ProcessError();
    }
    return ret;
}


SUMOVehicleClass
SUMOVehicleParserHelper::parseVehicleClass(const SUMOSAXAttributes &attrs,
        const std::string &type,
        const std::string &id) throw() {
    SUMOVehicleClass vclass = SVC_UNKNOWN;
    try {
        string vclassS = attrs.getStringSecure(SUMO_ATTR_VCLASS, "");
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
        string vclassS = attrs.getStringSecure(SUMO_ATTR_EMISSIONCLASS, "");
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
        string vclassS = attrs.getStringSecure(SUMO_ATTR_GUISHAPE, "");
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

