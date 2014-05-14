/****************************************************************************/
/// @file    SUMOVehicleParameter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 31.03.2009
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

#include "SUMOVehicleParameter.h"
#include <utils/common/ToString.h>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// member method definitions
// ===========================================================================
SUMOVehicleParameter::SUMOVehicleParameter()
    : vtypeid(DEFAULT_VTYPE_ID), color(RGBColor::DEFAULT_COLOR),
      depart(-1), departProcedure(DEPART_GIVEN),
      departLane(0), departLaneProcedure(DEPART_LANE_DEFAULT),
      departPos(0), departPosProcedure(DEPART_POS_DEFAULT),
      departSpeed(-1), departSpeedProcedure(DEPART_SPEED_DEFAULT),
      arrivalLane(0), arrivalLaneProcedure(ARRIVAL_LANE_DEFAULT),
      arrivalPos(0), arrivalPosProcedure(ARRIVAL_POS_DEFAULT),
      arrivalSpeed(-1), arrivalSpeedProcedure(ARRIVAL_SPEED_DEFAULT),
      repetitionNumber(-1), repetitionsDone(-1), repetitionOffset(-1), repetitionProbability(-1),
      line(), fromTaz(), toTaz(), personCapacity(0), personNumber(0), setParameter(0) {
}


bool
SUMOVehicleParameter::defaultOptionOverrides(const OptionsCont& oc, const std::string& optionName) const {
    return oc.isSet(optionName) && oc.getBool("defaults-override");
}


void
SUMOVehicleParameter::write(OutputDevice& dev, const OptionsCont& oc) const {
    dev.openTag(SUMO_TAG_VEHICLE).writeAttr(SUMO_ATTR_ID, id);
    if (wasSet(VEHPARS_VTYPE_SET)) {
        dev.writeAttr(SUMO_ATTR_TYPE, vtypeid);
    }
    if (departProcedure == DEPART_TRIGGERED) {
        dev.writeAttr(SUMO_ATTR_DEPART, "triggered");
    } else {
        dev.writeAttr(SUMO_ATTR_DEPART, time2string(depart));
    }

    // optional parameter
    //  departlane
    if (wasSet(VEHPARS_DEPARTLANE_SET) && !defaultOptionOverrides(oc, "departlane")) {
        std::string val;
        switch (departLaneProcedure) {
            case DEPART_LANE_GIVEN:
                val = toString(departLane);
                break;
            case DEPART_LANE_RANDOM:
                val = "random";
                break;
            case DEPART_LANE_FREE:
                val = "free";
                break;
            case DEPART_LANE_ALLOWED_FREE:
                val = "allowed";
                break;
            case DEPART_LANE_BEST_FREE:
                val = "best";
                break;
            case DEPART_LANE_DEFAULT:
            default:
                break;
        }
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTLANE, val);
    } else if (oc.isSet("departlane")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTLANE, oc.getString("departlane"));
    }
    //  departpos
    if (wasSet(VEHPARS_DEPARTPOS_SET) && !defaultOptionOverrides(oc, "departpos")) {
        std::string val;
        switch (departPosProcedure) {
            case DEPART_POS_GIVEN:
                val = toString(departPos);
                break;
            case DEPART_POS_RANDOM:
                val = "random";
                break;
            case DEPART_POS_RANDOM_FREE:
                val = "random_free";
                break;
            case DEPART_POS_FREE:
                val = "free";
                break;
            case DEPART_POS_PWAG_SIMPLE:
                val = "pwagSimple";
                break;
            case DEPART_POS_PWAG_GENERIC:
                val = "pwagGeneric";
                break;
            case DEPART_POS_MAX_SPEED_GAP:
                val = "maxSpeedGap";
                break;
            case DEPART_POS_BASE:
                val = "base";
                break;
            case DEPART_POS_DEFAULT:
            default:
                break;
        }
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTPOS, val);
    } else if (oc.isSet("departpos")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTPOS, oc.getString("departpos"));
    }
    //  departspeed
    if (wasSet(VEHPARS_DEPARTSPEED_SET) && !defaultOptionOverrides(oc, "departspeed")) {
        std::string val;
        switch (departSpeedProcedure) {
            case DEPART_SPEED_GIVEN:
                val = toString(departSpeed);
                break;
            case DEPART_SPEED_RANDOM:
                val = "random";
                break;
            case DEPART_SPEED_MAX:
                val = "max";
                break;
            case DEPART_SPEED_DEFAULT:
            default:
                break;
        }
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTSPEED, val);
    } else if (oc.isSet("departspeed")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTSPEED, oc.getString("departspeed"));
    }

    //  arrivallane
    if (wasSet(VEHPARS_ARRIVALLANE_SET) && !defaultOptionOverrides(oc, "arrivallane")) {
        std::string val;
        switch (arrivalLaneProcedure) {
            case ARRIVAL_LANE_GIVEN:
                val = toString(arrivalLane);
                break;
            case ARRIVAL_LANE_CURRENT:
                val = "current";
                break;
            case ARRIVAL_LANE_DEFAULT:
            default:
                break;
        }
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALLANE, val);
    } else if (oc.isSet("arrivallane")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALLANE, oc.getString("arrivallane"));
    }
    //  arrivalpos
    if (wasSet(VEHPARS_ARRIVALPOS_SET) && !defaultOptionOverrides(oc, "arrivalpos")) {
        std::string val;
        switch (arrivalPosProcedure) {
            case ARRIVAL_POS_GIVEN:
                val = toString(arrivalPos);
                break;
            case ARRIVAL_POS_RANDOM:
                val = "random";
                break;
            case ARRIVAL_POS_MAX:
                val = "max";
                break;
            case ARRIVAL_POS_DEFAULT:
            default:
                break;
        }
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALPOS, val);
    } else if (oc.isSet("arrivalpos")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALPOS, oc.getString("arrivalpos"));
    }
    //  arrivalspeed
    if (wasSet(VEHPARS_ARRIVALSPEED_SET) && !defaultOptionOverrides(oc, "arrivalspeed")) {
        std::string val;
        switch (arrivalSpeedProcedure) {
            case ARRIVAL_SPEED_GIVEN:
                val = toString(arrivalSpeed);
                break;
            case ARRIVAL_SPEED_CURRENT:
                val = "current";
                break;
            case ARRIVAL_SPEED_DEFAULT:
            default:
                break;
        }
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALSPEED, val);
    } else if (oc.isSet("arrivalspeed")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALSPEED, oc.getString("arrivalspeed"));
    }

    // color
    if (wasSet(VEHPARS_COLOR_SET)) {
        dev.writeAttr(SUMO_ATTR_COLOR, color);
    }
    if (wasSet(VEHPARS_LINE_SET)) {
        dev.writeAttr(SUMO_ATTR_LINE, line);
    }
    if (wasSet(VEHPARS_TAZ_SET)) {
        dev.writeAttr(SUMO_ATTR_FROM_TAZ, fromTaz).writeAttr(SUMO_ATTR_TO_TAZ, toTaz);
    }
    if (wasSet(VEHPARS_PERSON_CAPACITY_SET)) {
        dev.writeAttr(SUMO_ATTR_PERSON_CAPACITY, personCapacity);
    }
    if (wasSet(VEHPARS_PERSON_NUMBER_SET)) {
        dev.writeAttr(SUMO_ATTR_PERSON_NUMBER, personNumber);
    }
}


void
SUMOVehicleParameter::writeStops(OutputDevice& dev) const {
    for (std::vector<Stop>::const_iterator stop = stops.begin(); stop != stops.end(); ++stop) {
        dev.openTag(SUMO_TAG_STOP);
        if (stop->busstop != "") {
            dev.writeAttr(SUMO_ATTR_BUS_STOP, stop->busstop);
        } else {
            dev.writeAttr(SUMO_ATTR_LANE, stop->lane);
            if ((stop->setParameter & STOP_START_SET) != 0) {
                dev.writeAttr(SUMO_ATTR_STARTPOS, stop->startPos);
            }
            if ((stop->setParameter & STOP_END_SET) != 0) {
                dev.writeAttr(SUMO_ATTR_ENDPOS, stop->endPos);
            }
        }
        if (stop->duration >= 0) {
            dev.writeAttr(SUMO_ATTR_DURATION, STEPS2TIME(stop->duration));
        }
        if (stop->until >= 0) {
            dev.writeAttr(SUMO_ATTR_UNTIL, STEPS2TIME(stop->until));
        }
        if ((stop->setParameter & STOP_TRIGGER_SET) != 0) {
            dev.writeAttr(SUMO_ATTR_TRIGGERED, stop->triggered);
        }
        if ((stop->setParameter & STOP_PARKING_SET) != 0) {
            dev.writeAttr(SUMO_ATTR_PARKING, stop->parking);
        }
        // look, we are writing the set of expected persons in its current state...
        //  if this method is used somewhere in the simulation output,
        //  one should consider keeping the original values additionally,
        //  as the ones we write may hev changed.
        if ((stop->setParameter & STOP_EXPECTED_SET) != 0) {
            dev.writeAttr(SUMO_ATTR_EXPECTED, stop->awaitedPersons);
        }
        dev.closeTag();
    }
}


bool
SUMOVehicleParameter::parseDepart(const std::string& val, const std::string& element, const std::string& id,
                                  SUMOTime& depart, DepartDefinition& dd, std::string& error) {
    if (val == "triggered") {
        dd = DEPART_TRIGGERED;
    } else if (val == "now") {
        dd = DEPART_NOW;
    } else {
        try {
            depart = string2time(val);
            dd = DEPART_GIVEN;
            if (depart < 0) {
                error = "Negative departure time in the definition of '" + id + "'.";
                return false;
            }
        } catch (...) {
            error = "Invalid departure time for " + element + " '" + id + "';\n must be one of (\"triggered\", \"now\", or a float >= 0)";
            return false;
        }
    }
    return true;
}


bool
SUMOVehicleParameter::parseDepartLane(const std::string& val, const std::string& element, const std::string& id,
                                      int& lane, DepartLaneDefinition& dld, std::string& error) {
    bool ok = true;
    if (val == "random") {
        dld = DEPART_LANE_RANDOM;
    } else if (val == "free") {
        dld = DEPART_LANE_FREE;
    } else if (val == "allowed") {
        dld = DEPART_LANE_ALLOWED_FREE;
    } else if (val == "best") {
        dld = DEPART_LANE_BEST_FREE;
    } else {
        try {
            lane = TplConvert::_2int(val.c_str());
            dld = DEPART_LANE_GIVEN;
            if (lane < 0) {
                ok = false;
            }
        } catch (...) {
            ok = false;
        }
    }
    if (!ok) {
        error = "Invalid departLane definition for " + element + " '" + id + "';\n must be one of (\"random\", \"free\", \"allowed\", \"best\", or an int>=0)";
    }
    return ok;
}


bool
SUMOVehicleParameter::parseDepartPos(const std::string& val, const std::string& element, const std::string& id,
                                     SUMOReal& pos, DepartPosDefinition& dpd, std::string& error) {
    bool ok = true;
    if (val == "random") {
        dpd = DEPART_POS_RANDOM;
    } else if (val == "random_free") {
        dpd = DEPART_POS_RANDOM_FREE;
    } else if (val == "free") {
        dpd = DEPART_POS_FREE;
    } else if (val == "base") {
        dpd = DEPART_POS_BASE;
    } else if (val == "pwagSimple") {
        dpd = DEPART_POS_PWAG_SIMPLE;
    } else if (val == "pwagGeneric") {
        dpd = DEPART_POS_PWAG_GENERIC;
    } else if (val == "maxSpeedGap") {
        dpd = DEPART_POS_MAX_SPEED_GAP;
    } else {
        try {
            pos = TplConvert::_2SUMOReal(val.c_str());
            dpd = DEPART_POS_GIVEN;
        } catch (...) {
            ok = false;
        }
    }
    if (!ok) {
        error = "Invalid departPos definition for " + element + " '" + id + "';\n must be one of (\"random\", \"random_free\", \"free\", \"base\", \"pwagSimple\", \"pwagGeneric\", \"maxSpeedGap\", or a float)";
    }
    return ok;
}


bool
SUMOVehicleParameter::parseDepartSpeed(const std::string& val, const std::string& element, const std::string& id,
                                       SUMOReal& speed, DepartSpeedDefinition& dsd, std::string& error) {
    bool ok = true;
    if (val == "random") {
        dsd = DEPART_SPEED_RANDOM;
    } else if (val == "max") {
        dsd = DEPART_SPEED_MAX;
    } else {
        try {
            speed = TplConvert::_2SUMOReal(val.c_str());
            dsd = DEPART_SPEED_GIVEN;
            if (speed < 0) {
                ok = false;
            }
        } catch (...) {
            ok = false;
        }
    }
    if (!ok) {
        error = "Invalid departSpeed definition for " + element + " '" + id + "';\n must be one of (\"random\", \"max\", or a float>=0)";
    }
    return ok;
}


bool
SUMOVehicleParameter::parseArrivalLane(const std::string& val, const std::string& element, const std::string& id,
                                       int& lane, ArrivalLaneDefinition& ald, std::string& error) {
    bool ok = true;
    if (val == "current") {
        ald = ARRIVAL_LANE_CURRENT;
    } else {
        try {
            lane = TplConvert::_2int(val.c_str());
            ald = ARRIVAL_LANE_GIVEN;
            if (lane < 0) {
                ok = false;
            }
        } catch (...) {
            ok = false;
        }
    }
    if (!ok) {
        error = "Invalid arrivalLane definition for " + element + " '" + id + "';\n must be one of (\"current\", or an int>=0)";
    }
    return ok;
}


bool
SUMOVehicleParameter::parseArrivalPos(const std::string& val, const std::string& element, const std::string& id,
                                      SUMOReal& pos, ArrivalPosDefinition& apd, std::string& error) {
    bool ok = true;
    if (val == "random") {
        apd = ARRIVAL_POS_RANDOM;
    } else if (val == "max") {
        apd = ARRIVAL_POS_MAX;
    } else {
        try {
            pos = TplConvert::_2SUMOReal(val.c_str());
            apd = ARRIVAL_POS_GIVEN;
        } catch (...) {
            ok = false;
        }
    }
    if (!ok) {
        error = "Invalid arrivalPos definition for " + element + " '" + id + "';\n must be one of (\"random\", \"max\", or a float)";
    }
    return ok;
}


bool
SUMOVehicleParameter::parseArrivalSpeed(const std::string& val, const std::string& element, const std::string& id,
                                        SUMOReal& speed, ArrivalSpeedDefinition& asd, std::string& error) {
    bool ok = true;
    if (val == "current") {
        asd = ARRIVAL_SPEED_CURRENT;
    } else {
        try {
            speed = TplConvert::_2SUMOReal(val.c_str());
            if (speed < 0) {
                ok = false;
            }
            asd = ARRIVAL_SPEED_GIVEN;
        } catch (...) {
            ok = false;
        }
    }
    if (!ok) {
        error = "Invalid arrivalSpeed definition for " + element + " '" + id + "';\n must be one of (\"current\", or a float>=0)";
    }
    return ok;
}


SUMOReal
SUMOVehicleParameter::interpretEdgePos(SUMOReal pos, SUMOReal maximumValue, SumoXMLAttr attr, const std::string& id) {
    if (pos < 0) {
        pos = maximumValue + pos;
    }
    if (pos > maximumValue) {
        WRITE_WARNING("Invalid " + toString(attr) + " " + toString(pos) + " given for " + id + ". Inserting at edge end instead.");
        pos = maximumValue;
    }
    return pos;
}


/****************************************************************************/
