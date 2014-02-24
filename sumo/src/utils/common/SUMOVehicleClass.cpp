/****************************************************************************/
/// @file    SUMOVehicleClass.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @author  Walter Bamberger
/// @date    2006-01-24
/// @version $Id$
///
// Definitions of SUMO vehicle classes and helper functions
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

#include <string>
#include <map>
#include "SUMOVehicleClass.h"
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>


#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// static members
// ===========================================================================
StringBijection<SUMOVehicleClass>::Entry sumoVehicleClassStringInitializer[] = {
    {"unknown",           SVC_UNKNOWN},
    {"private",           SVC_PRIVATE},
    {"public_transport",  SVC_PUBLIC_TRANSPORT},
    {"public_emergency",  SVC_PUBLIC_EMERGENCY},
    {"public_authority",  SVC_PUBLIC_AUTHORITY},
    {"public_army",       SVC_PUBLIC_ARMY},
    {"vip",               SVC_VIP},
    {"ignoring",          SVC_IGNORING},
    {"passenger",         SVC_PASSENGER},
    {"hov",               SVC_HOV},
    {"taxi",              SVC_TAXI},
    {"bus",               SVC_BUS},
    {"delivery",          SVC_DELIVERY},
    {"transport",         SVC_TRANSPORT},
    {"lightrail",         SVC_LIGHTRAIL},
    {"cityrail",          SVC_CITYRAIL},
    {"rail_slow",         SVC_RAIL_SLOW},
    {"rail_fast",         SVC_RAIL_FAST},
    {"motorcycle",        SVC_MOTORCYCLE},
    {"bicycle",           SVC_BICYCLE},
    {"pedestrian",        SVC_PEDESTRIAN},
    {"custom1",           SVC_CUSTOM1},
    {"custom2",           SVC_CUSTOM2}
};

StringBijection<SUMOVehicleClass> SumoVehicleClassStrings(
    sumoVehicleClassStringInitializer, SVC_CUSTOM2);


StringBijection<SUMOVehicleShape>::Entry sumoVehicleShapeStringInitializer[] = {
    {"pedestrian",            SVS_PEDESTRIAN},
    {"bicycle",               SVS_BICYCLE},
    {"motorcycle",            SVS_MOTORCYCLE},
    {"passenger",             SVS_PASSENGER},
    {"passenger/sedan",       SVS_PASSENGER_SEDAN},
    {"passenger/hatchback",   SVS_PASSENGER_HATCHBACK},
    {"passenger/wagon",       SVS_PASSENGER_WAGON},
    {"passenger/van",         SVS_PASSENGER_VAN},
    {"delivery",              SVS_DELIVERY},
    {"transport",             SVS_TRANSPORT},
    {"transport/semitrailer", SVS_TRANSPORT_SEMITRAILER},
    {"transport/trailer",     SVS_TRANSPORT_1TRAILER},
    {"bus",                   SVS_BUS},
    {"bus/city",              SVS_BUS_CITY},
    {"bus/flexible",          SVS_BUS_CITY_FLEXIBLE},
    {"bus/overland",          SVS_BUS_OVERLAND},
    {"bus/trolley",           SVS_BUS_TROLLEY},
    {"rail",                  SVS_RAIL},
    {"rail/light",            SVS_RAIL_LIGHT},
    {"rail/city",             SVS_RAIL_CITY},
    {"rail/slow",             SVS_RAIL_SLOW},
    {"rail/fast",             SVS_RAIL_FAST},
    {"rail/cargo",            SVS_RAIL_CARGO},
    {"evehicle",              SVS_E_VEHICLE},
    {"ant",                   SVS_ANT},
    {"",                      SVS_UNKNOWN}
};


StringBijection<SUMOVehicleShape> SumoVehicleShapeStrings(
    sumoVehicleShapeStringInitializer, SVS_UNKNOWN);


StringBijection<SUMOEmissionClass>::Entry SumoEmissionClassStringInitializer[] = {
    {"unknown",                SVE_UNKNOWN},
    // heavy duty vehicles; 3 clusters
    {"HDV_3_1",                SVE_HDV_3_1},
    {"HDV_3_2",                SVE_HDV_3_2},
    {"HDV_3_3",                SVE_HDV_3_3},
    // heavy duty vehicles; 6 clusters
    {"HDV_6_1",                SVE_HDV_6_1},
    {"HDV_6_2",                SVE_HDV_6_2},
    {"HDV_6_3",                SVE_HDV_6_3},
    {"HDV_6_4",                SVE_HDV_6_4},
    {"HDV_6_5",                SVE_HDV_6_5},
    {"HDV_6_6",                SVE_HDV_6_6},
    // heavy duty vehicles; 12 clusters
    {"HDV_12_1",               SVE_HDV_12_1},
    {"HDV_12_2",               SVE_HDV_12_2},
    {"HDV_12_3",               SVE_HDV_12_3},
    {"HDV_12_4",               SVE_HDV_12_4},
    {"HDV_12_5",               SVE_HDV_12_5},
    {"HDV_12_6",               SVE_HDV_12_6},
    {"HDV_12_7",               SVE_HDV_12_7},
    {"HDV_12_8",               SVE_HDV_12_8},
    {"HDV_12_9",               SVE_HDV_12_9},
    {"HDV_12_10",              SVE_HDV_12_10},
    {"HDV_12_11",              SVE_HDV_12_11},
    {"HDV_12_12",              SVE_HDV_12_12},
    // passenger & light duty vehicles; 7 clusters
    {"P_7_1",                  SVE_P_LDV_7_1},
    {"P_7_2",                  SVE_P_LDV_7_2},
    {"P_7_3",                  SVE_P_LDV_7_3},
    {"P_7_4",                  SVE_P_LDV_7_4},
    {"P_7_5",                  SVE_P_LDV_7_5},
    {"P_7_6",                  SVE_P_LDV_7_6},
    {"P_7_7",                  SVE_P_LDV_7_7},
    // passenger & light duty vehicles; 14 clusters
    {"P_14_1",                 SVE_P_LDV_14_1},
    {"P_14_2",                 SVE_P_LDV_14_2},
    {"P_14_3",                 SVE_P_LDV_14_3},
    {"P_14_4",                 SVE_P_LDV_14_4},
    {"P_14_5",                 SVE_P_LDV_14_5},
    {"P_14_6",                 SVE_P_LDV_14_6},
    {"P_14_7",                 SVE_P_LDV_14_7},
    {"P_14_8",                 SVE_P_LDV_14_8},
    {"P_14_9",                 SVE_P_LDV_14_9},
    {"P_14_10",                SVE_P_LDV_14_10},
    {"P_14_11",                SVE_P_LDV_14_11},
    {"P_14_12",                SVE_P_LDV_14_12},
    {"P_14_13",                SVE_P_LDV_14_13},
    {"P_14_14",                SVE_P_LDV_14_14},
    // no emissions
    {"zero",                   SVE_ZERO_EMISSIONS},
    // heavy duty vehicles, no accel; 3 clusters
    {"HDV_A0_3_1",             SVE_HDV_A0_3_1},
    {"HDV_A0_3_2",             SVE_HDV_A0_3_2},
    {"HDV_A0_3_3",             SVE_HDV_A0_3_3},
    // heavy duty vehicles, no accel; 6 clusters
    {"HDV_A0_6_1",             SVE_HDV_A0_6_1},
    {"HDV_A0_6_2",             SVE_HDV_A0_6_2},
    {"HDV_A0_6_3",             SVE_HDV_A0_6_3},
    {"HDV_A0_6_4",             SVE_HDV_A0_6_4},
    {"HDV_A0_6_5",             SVE_HDV_A0_6_5},
    {"HDV_A0_6_6",             SVE_HDV_A0_6_6},
    // heavy duty vehicles, no accel; 12 clusters
    {"HDV_A0_12_1",            SVE_HDV_A0_12_1},
    {"HDV_A0_12_2",            SVE_HDV_A0_12_2},
    {"HDV_A0_12_3",            SVE_HDV_A0_12_3},
    {"HDV_A0_12_4",            SVE_HDV_A0_12_4},
    {"HDV_A0_12_5",            SVE_HDV_A0_12_5},
    {"HDV_A0_12_6",            SVE_HDV_A0_12_6},
    {"HDV_A0_12_7",            SVE_HDV_A0_12_7},
    {"HDV_A0_12_8",            SVE_HDV_A0_12_8},
    {"HDV_A0_12_9",            SVE_HDV_A0_12_9},
    {"HDV_A0_12_10",           SVE_HDV_A0_12_10},
    {"HDV_A0_12_11",           SVE_HDV_A0_12_11},
    {"HDV_A0_12_12",           SVE_HDV_A0_12_12},
    // passenger & light duty vehicles, no accel; 7 clusters
    {"P_A0_7_1",               SVE_P_LDV_A0_7_1},
    {"P_A0_7_2",               SVE_P_LDV_A0_7_2},
    {"P_A0_7_3",               SVE_P_LDV_A0_7_3},
    {"P_A0_7_4",               SVE_P_LDV_A0_7_4},
    {"P_A0_7_5",               SVE_P_LDV_A0_7_5},
    {"P_A0_7_6",               SVE_P_LDV_A0_7_6},
    {"P_A0_7_7",               SVE_P_LDV_A0_7_7},
    // passenger & light duty vehicles, no accel; 14 clusters
    {"P_A0_14_1",              SVE_P_LDV_A0_14_1},
    {"P_A0_14_2",              SVE_P_LDV_A0_14_2},
    {"P_A0_14_3",              SVE_P_LDV_A0_14_3},
    {"P_A0_14_4",              SVE_P_LDV_A0_14_4},
    {"P_A0_14_5",              SVE_P_LDV_A0_14_5},
    {"P_A0_14_6",              SVE_P_LDV_A0_14_6},
    {"P_A0_14_7",              SVE_P_LDV_A0_14_7},
    {"P_A0_14_8",              SVE_P_LDV_A0_14_8},
    {"P_A0_14_9",              SVE_P_LDV_A0_14_9},
    {"P_A0_14_10",             SVE_P_LDV_A0_14_10},
    {"P_A0_14_11",             SVE_P_LDV_A0_14_11},
    {"P_A0_14_12",             SVE_P_LDV_A0_14_12},
    {"P_A0_14_13",             SVE_P_LDV_A0_14_13},
    {"P_A0_14_14",             SVE_P_LDV_A0_14_14},

    // PHEMlight
    {"PKW_D_EU0",              SVE_PKW_D_EU0},
    {"PKW_D_EU1",              SVE_PKW_D_EU1},
    {"PKW_D_EU2",              SVE_PKW_D_EU2},
    {"PKW_D_EU3",              SVE_PKW_D_EU3},
    {"PKW_D_EU4",              SVE_PKW_D_EU4},
    {"PKW_D_EU5",              SVE_PKW_D_EU5},
    {"PKW_D_EU6",              SVE_PKW_D_EU6},
    {"PKW_G_EU0",              SVE_PKW_G_EU0},
    {"PKW_G_EU1",              SVE_PKW_G_EU1},
    {"PKW_G_EU2",              SVE_PKW_G_EU2},
    {"PKW_G_EU3",              SVE_PKW_G_EU3},
    {"PKW_G_EU4",              SVE_PKW_G_EU4},
    {"PKW_G_EU5",              SVE_PKW_G_EU5},
    {"PKW_G_EU6",              SVE_PKW_G_EU6},

    {"LNF_D_EU0",              SVE_LNF_D_EU0},
    {"LNF_D_EU1",              SVE_LNF_D_EU1},
    {"LNF_D_EU2",              SVE_LNF_D_EU2},
    {"LNF_D_EU3",              SVE_LNF_D_EU3},
    {"LNF_D_EU4",              SVE_LNF_D_EU4},
    {"LNF_D_EU5",              SVE_LNF_D_EU5},
    {"LNF_D_EU6",              SVE_LNF_D_EU6},
    {"LNF_G_EU0",              SVE_LNF_G_EU0},
    {"LNF_G_EU1",              SVE_LNF_G_EU1},
    {"LNF_G_EU2",              SVE_LNF_G_EU2},
    {"LNF_G_EU3",              SVE_LNF_G_EU3},
    {"LNF_G_EU4",              SVE_LNF_G_EU4},
    {"LNF_G_EU5",              SVE_LNF_G_EU5},
    {"LNF_G_EU6",              SVE_LNF_G_EU6},
    {"LSZ_D_EU0",              SVE_LSZ_D_EU0},
    {"LSZ_D_EU1",              SVE_LSZ_D_EU1},
    {"LSZ_D_EU2",              SVE_LSZ_D_EU2},
    {"LSZ_D_EU3",              SVE_LSZ_D_EU3},
    {"LSZ_D_EU4_EGR",          SVE_LSZ_D_EU4_EGR},
    {"LSZ_D_EU4_SCR",          SVE_LSZ_D_EU4_SCR},
    {"LSZ_D_EU5_EGR",          SVE_LSZ_D_EU5_EGR},
    {"LSZ_D_EU5_SCR",          SVE_LSZ_D_EU5_SCR},
    {"LSZ_D_EU6",              SVE_LSZ_D_EU6},
    {"RB_D_EU0",               SVE_RB_D_EU0}, 
    {"RB_D_EU1",               SVE_RB_D_EU1},
    {"RB_D_EU2",               SVE_RB_D_EU2},
    {"RB_D_EU3",               SVE_RB_D_EU3},
    {"RB_D_EU4_EGR",           SVE_RB_D_EU4_EGR},
    {"RB_D_EU4_SCR",           SVE_RB_D_EU4_SCR},
    {"RB_D_EU5_EGR",           SVE_RB_D_EU5_EGR},
    {"RB_D_EU5_SCR",           SVE_RB_D_EU5_SCR},
    {"RB_D_EU6",               SVE_RB_D_EU6},
    {"SB_D_EU0",               SVE_SB_D_EU0},
    {"SB_D_EU1",               SVE_SB_D_EU1},
    {"SB_D_EU2",               SVE_SB_D_EU2},
    {"SB_D_EU3",               SVE_SB_D_EU3},
    {"SB_D_EU4_EGR",           SVE_SB_D_EU4_EGR},
    {"SB_D_EU4_SCR",           SVE_SB_D_EU4_SCR},
    {"SB_D_EU5_EGR",           SVE_SB_D_EU5_EGR},
    {"SB_D_EU5_SCR",           SVE_SB_D_EU5_SCR},
    {"SB_D_EU6",               SVE_SB_D_EU6}

};

StringBijection<SUMOEmissionClass> SumoEmissionClassStrings(
    SumoEmissionClassStringInitializer, SVE_SB_D_EU6);


// ===========================================================================
// additional constants
// ===========================================================================

const int SUMOVehicleClass_MAX = SVC_CUSTOM2;
const SVCPermissions SVCFreeForAll = std::numeric_limits<SVCPermissions>::max(); // all bits set to 1

// ===========================================================================
// method definitions
// ===========================================================================
// ------------ Conversion of SUMOVehicleClass

std::string
getVehicleClassCompoundName(int id) {
    std::string ret;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if ((id & SumoVehicleClassStrings.get(*it))) {
            ret += ("|" + *it);
        }
    }
    if (ret.length() > 0) {
        return ret.substr(1);
    } else {
        return ret;
    }
}


std::string
getAllowedVehicleClassNames(SVCPermissions permissions) {
    return joinToString(getAllowedVehicleClassNamesList(permissions), ' ');
}


std::vector<std::string>
getAllowedVehicleClassNamesList(SVCPermissions permissions) {
    /// @todo cache values?
    const std::vector<std::string> classNames = SumoVehicleClassStrings.getStrings();
    std::vector<std::string> result;
    for (std::vector<std::string>::const_iterator it = classNames.begin(); it != classNames.end(); it++) {
        const int svc = (int)SumoVehicleClassStrings.get(*it);
        if ((svc & permissions) == svc && svc != SVC_UNKNOWN) {
            result.push_back(*it);
        }
    }
    return result;
}


std::pair<std::string, bool>
getPermissionEncoding(SVCPermissions permissions) {
    // shortcut the common cases
    if (permissions == SVCFreeForAll) {
        return std::pair<std::string, bool>("", false); // nothing disallowed
    }
    // figure out whether its shorter to write allow or disallow
    // @note: this code assumes that enum values are assigned contiguous powers of 2 from 1 to SUMOVehicleClass_MAX
    size_t num_allowed = 0;
    for (int mask = 1; mask <= SUMOVehicleClass_MAX; mask = mask << 1) {
        if ((mask & permissions) == mask) {
            ++num_allowed;
        }
    }
    if (num_allowed <= (SumoVehicleClassStrings.size() - num_allowed) && num_allowed > 0) {
        return std::pair<std::string, bool>(getAllowedVehicleClassNames(permissions), true);
    } else {
        return std::pair<std::string, bool>(getAllowedVehicleClassNames(~permissions), false);
    }
}


SUMOVehicleClass
getVehicleClassID(const std::string& name) {
    if (SumoVehicleClassStrings.hasString(name)) {
        return SumoVehicleClassStrings.get(name);
    }
    throw ProcessError("Unknown vehicle class '" + name + "'.");
}


int
getVehicleClassCompoundID(const std::string& name) {
    int ret = SVC_UNKNOWN;
    const std::vector<std::string> names = SumoVehicleClassStrings.getStrings();
    for (std::vector<std::string>::const_iterator it = names.begin(); it != names.end(); it++) {
        if (name.find(*it) != std::string::npos) {
            ret = ret | (int) SumoVehicleClassStrings.get(*it);
        }
    }
    return ret;
}


SVCPermissions
parseVehicleClasses(const std::string& allowedS) {
    SVCPermissions result = 0;
    StringTokenizer sta(allowedS, " ");
    while (sta.hasNext()) {
        result |= getVehicleClassID(sta.next());
    }
    return result;
}


bool
canParseVehicleClasses(const std::string& classes) {
    StringTokenizer sta(classes, " ");
    while (sta.hasNext()) {
        if (!SumoVehicleClassStrings.hasString(sta.next())) {
            return false;
        }
    }
    return true;
}


extern SVCPermissions parseVehicleClasses(const std::string& allowedS, const std::string& disallowedS) {
    if (allowedS.size() == 0 && disallowedS.size() == 0) {
        return SVCFreeForAll;
    } else if (allowedS.size() > 0 && disallowedS.size() > 0) {
        WRITE_WARNING("SVCPermissions must be specified either via 'allow' or 'disallow'. Ignoring 'disallow'");
        return parseVehicleClasses(allowedS);
    } else if (allowedS.size() > 0) {
        return parseVehicleClasses(allowedS);
    } else {
        return ~parseVehicleClasses(disallowedS);
    }
}


SVCPermissions
parseVehicleClasses(const std::vector<std::string>& allowedS) {
    SVCPermissions result = 0;
    for (std::vector<std::string>::const_iterator i = allowedS.begin(); i != allowedS.end(); ++i) {
        result |= getVehicleClassID(*i);
    }
    return result;
}


SUMOVehicleShape
getVehicleShapeID(const std::string& name) {
    if (SumoVehicleShapeStrings.hasString(name)) {
        return SumoVehicleShapeStrings.get(name);
    } else {
        throw ProcessError("Unknown vehicle shape '" + name + "'.");
    }
}


std::string
getVehicleShapeName(SUMOVehicleShape id) {
    return SumoVehicleShapeStrings.getString(id);
}


bool isRailway(SVCPermissions permissions) {
    const int anyRail = SVC_RAIL_FAST + SVC_RAIL_SLOW + SVC_CITYRAIL + SVC_LIGHTRAIL;
    return (permissions & anyRail) > 0 && (permissions & SVC_PASSENGER) == 0;
}

// ------------ Conversion of SUMOEmissionClass
SUMOEmissionClass
getVehicleEmissionTypeID(const std::string& name) {
    if (SumoEmissionClassStrings.hasString(name)) {
        return SumoEmissionClassStrings.get(name);
    } else {
        throw ProcessError("Unknown emission type '" + name + "'.");
    }
}

std::string
getVehicleEmissionTypeName(SUMOEmissionClass id) {
    return SumoEmissionClassStrings.getString(id);
}


const std::string DEFAULT_VTYPE_ID("DEFAULT_VEHTYPE");
const SUMOReal DEFAULT_VEH_MAXSPEED(70.0);
const SUMOReal DEFAULT_VEH_ACCEL(2.6);
const SUMOReal DEFAULT_VEH_DECEL(4.5);
const SUMOReal DEFAULT_VEH_SIGMA(0.5);
const SUMOReal DEFAULT_VEH_LENGTH(5.);
const SUMOReal DEFAULT_VEH_MINGAP(2.5);
const SUMOReal DEFAULT_VEH_TAU(1.);
const SUMOVehicleClass DEFAULT_VEH_CLASS(SVC_UNKNOWN);
const SUMOReal DEFAULT_VEH_PROB(1.);
const SUMOReal DEFAULT_VEH_SPEEDFACTOR(1.);
const SUMOReal DEFAULT_VEH_SPEEDDEV(0.);
const SUMOReal DEFAULT_VEH_WIDTH(2.);
const SUMOReal DEFAULT_VEH_HEIGHT(1.5);
const SumoXMLTag DEFAULT_VEH_FOLLOW_MODEL(SUMO_TAG_CF_KRAUSS);
const LaneChangeModel DEFAULT_VEH_LANE_CHANGE_MODEL(LCM_LC2013);
const SUMOVehicleShape DEFAULT_VEH_SHAPE(SVS_UNKNOWN);
const SUMOReal DEFAULT_VEH_TMP1(1.);
const SUMOReal DEFAULT_VEH_TMP2(1.);
const SUMOReal DEFAULT_VEH_TMP3(1.);
const SUMOReal DEFAULT_VEH_TMP4(1.);
const SUMOReal DEFAULT_VEH_TMP5(1.);

const SUMOReal DEFAULT_PERSON_SPEED(5. / 3.6);

/****************************************************************************/

