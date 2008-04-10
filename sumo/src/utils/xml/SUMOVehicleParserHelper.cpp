/****************************************************************************/
/// @file    SUMOVehicleParserHelper.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 07.04.2008
/// @version $Id: SUMOVehicleParserHelper.cpp 5277 2008-03-31 09:27:36Z dkrajzew $
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
                                                bool skipID, bool skipDepart)
{
    bool ok = true;
    string id;
    if(!skipID) {
        if(!attrs.setIDFromAttribues("vehicle", id)) {
            return 0;
        }
    }
    SUMOVehicleParameter *ret = new SUMOVehicleParameter();
    ret->id = id;
    //ret->refid = attrs.getStringSecure(SUMO_ATTR_REFID, "");
    // parse route information
    if(attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        ret->setParameter |= VEHPARS_ROUTE_SET; // !!! needed?
        ret->routeid = attrs.getString(SUMO_ATTR_ROUTE);
    }
    // parse route information
    if(attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        ret->setParameter |= VEHPARS_VTYPE_SET; // !!! needed?
        ret->vtypeid = attrs.getString(SUMO_ATTR_TYPE);
    }

    if(!skipDepart) {
        ret->depart = attrs.getIntReporting(SUMO_ATTR_DEPART, "vehicle", id.c_str(), ok);
    }
    // parse depart lane information
    if(attrs.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
        ret->setParameter |= VEHPARS_DEPARTLANE_SET;
        string helper = attrs.getString(SUMO_ATTR_DEPARTLANE);
        if(helper=="departlane") {
            ret->departLaneProcedure = DEPART_LANE_DEPARTLANE;
        } else if(helper=="random") {
            ret->departLaneProcedure = DEPART_LANE_RANDOM;
        } else if(helper=="free") {
            ret->departLaneProcedure = DEPART_LANE_FREE;
        } else {
            ret->departLaneProcedure = DEPART_LANE_GIVEN;
            ret->departLane = TplConvert<char>::_2int(helper.c_str());;
        }
    }
    // parse depart position information
    if(attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
        ret->setParameter |= VEHPARS_DEPARTPOS_SET;
        string helper = attrs.getString(SUMO_ATTR_DEPARTPOS);
        if(helper=="random") {
            ret->departPosProcedure = DEPART_POS_RANDOM;
        } else if(helper=="free") {
            ret->departPosProcedure = DEPART_POS_FREE;
        } else {
            ret->departPosProcedure = DEPART_POS_GIVEN;
            ret->departPos = TplConvert<char>::_2SUMOReal(helper.c_str());
        }
    }
    // parse depart position information
    if(attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED)) {
        ret->setParameter |= VEHPARS_DEPARTSPEED_SET;
        string helper = attrs.getString(SUMO_ATTR_DEPARTSPEED);
        if(helper=="random") {
            ret->departSpeedProcedure = DEPART_SPEED_RANDOM;
        } else if(helper=="max") {
            ret->departSpeedProcedure = DEPART_SPEED_MAX;
        } else {
            ret->departSpeedProcedure = DEPART_SPEED_GIVEN;
            ret->departSpeed = TplConvert<char>::_2SUMOReal(helper.c_str());
        }
    }

    ret->arrivalLane = attrs.getStringSecure(SUMO_ATTR_ARRIVALLANE, "");
    ret->arrivalPos = attrs.getFloatSecure(SUMO_ATTR_ARRIVALPOS, -1); //!!! specs have strings
    ret->arrivalSpeed = attrs.getFloatSecure(SUMO_ATTR_ARRIVALSPEED, -1); //!!! specs have strings

    // parse depart position information
    if(attrs.hasAttribute(SUMO_ATTR_PERIOD)) {
        ret->setParameter |= VEHPARS_PERIODFREQ_SET;
        ret->repetitionOffset = attrs.getInt(SUMO_ATTR_PERIOD);
    }
    if(attrs.hasAttribute(SUMO_ATTR_REPNUMBER)) {
        ret->setParameter |= VEHPARS_PERIODNUM_SET;
        ret->repetitionNumber = attrs.getInt(SUMO_ATTR_REPNUMBER);
    }

    // parse color
    if(attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        ret->setParameter |= VEHPARS_COLOR_SET;
        ret->color = RGBColor::parseColor(attrs.getString(SUMO_ATTR_COLOR));
    }

    if(!ok) {
        delete ret;
        ret = 0;
    }
    return ret;
}


SUMOVehicleClass
SUMOVehicleParserHelper::parseVehicleClass(const SUMOSAXAttributes &attrs,
                                        const std::string &type,
                                        const std::string &id) throw()
{
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


/****************************************************************************/

