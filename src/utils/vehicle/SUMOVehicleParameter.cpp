/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2020 German Aerospace Center (DLR) and others.
// This program and the accompanying materials are made available under the
// terms of the Eclipse Public License 2.0 which is available at
// https://www.eclipse.org/legal/epl-2.0/
// This Source Code may also be made available under the following Secondary
// Licenses when the conditions for such availability set forth in the Eclipse
// Public License 2.0 are satisfied: GNU General Public License, version 2
// or later which is available at
// https://www.gnu.org/licenses/old-licenses/gpl-2.0-standalone.html
// SPDX-License-Identifier: EPL-2.0 OR GPL-2.0-or-later
/****************************************************************************/
/// @file    SUMOVehicleParameter.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Michael Behrisch
/// @date    Tue, 31.03.2009
///
// Structure representing possible vehicle parameter
/****************************************************************************/
#include <config.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#include "SUMOVehicleParameter.h"

// ===========================================================================
// member method definitions
// ===========================================================================

SUMOVehicleParameter::SUMOVehicleParameter()
    : tag(SUMO_TAG_NOTHING), vtypeid(DEFAULT_VTYPE_ID), color(RGBColor::DEFAULT_COLOR),
      depart(-1), departProcedure(DEPART_GIVEN),
      departLane(0), departLaneProcedure(DEPART_LANE_DEFAULT),
      departPos(0), departPosProcedure(DEPART_POS_DEFAULT),
      departPosLat(0), departPosLatProcedure(DEPART_POSLAT_DEFAULT),
      departSpeed(-1), departSpeedProcedure(DEPART_SPEED_DEFAULT),
      arrivalLane(0), arrivalLaneProcedure(ARRIVAL_LANE_DEFAULT),
      arrivalPos(0), arrivalPosProcedure(ARRIVAL_POS_DEFAULT),
      arrivalPosLat(0), arrivalPosLatProcedure(ARRIVAL_POSLAT_DEFAULT),
      arrivalSpeed(-1), arrivalSpeedProcedure(ARRIVAL_SPEED_DEFAULT),
      repetitionNumber(-1), repetitionsDone(-1), repetitionOffset(-1), repetitionProbability(-1), repetitionEnd(-1),
      line(), fromTaz(), toTaz(), personNumber(0), containerNumber(0),
      speedFactor(-1),
      parametersSet(0)
{ }


SUMOVehicleParameter::~SUMOVehicleParameter() {
}


bool
SUMOVehicleParameter::defaultOptionOverrides(const OptionsCont& oc, const std::string& optionName) const {
    return oc.exists(optionName) && oc.isSet(optionName) && oc.getBool("defaults-override");
}


void
SUMOVehicleParameter::write(OutputDevice& dev, const OptionsCont& oc, const SumoXMLTag tag, const std::string& typeID) const {
    dev.openTag(tag).writeAttr(SUMO_ATTR_ID, id);
    if (typeID == "") {
        if (wasSet(VEHPARS_VTYPE_SET)) {
            dev.writeAttr(SUMO_ATTR_TYPE, vtypeid);
        }
    } else {
        dev.writeAttr(SUMO_ATTR_TYPE, typeID);
    }
    // write depart depending of tag
    if ((tag == SUMO_TAG_FLOW) || (tag == SUMO_TAG_PERSONFLOW)) {
        dev.writeAttr(SUMO_ATTR_BEGIN, getDepart());
    } else {
        dev.writeAttr(SUMO_ATTR_DEPART, getDepart());
    }
    // optional parameter
    //  departlane
    if (wasSet(VEHPARS_DEPARTLANE_SET) && !defaultOptionOverrides(oc, "departlane")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTLANE, getDepartLane());
    } else if (oc.exists("departlane") && oc.isSet("departlane")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTLANE, oc.getString("departlane"));
    }
    //  departpos
    if (wasSet(VEHPARS_DEPARTPOS_SET) && !defaultOptionOverrides(oc, "departpos")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTPOS, getDepartPos());
    } else if (oc.exists("departpos") && oc.isSet("departpos")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTPOS, oc.getString("departpos"));
    }
    //  departPosLat
    if (wasSet(VEHPARS_DEPARTPOSLAT_SET)) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTPOS_LAT, getDepartPosLat());
    }

    //  departspeed
    if (wasSet(VEHPARS_DEPARTSPEED_SET) && !defaultOptionOverrides(oc, "departspeed")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTSPEED, getDepartSpeed());
    } else if (oc.exists("departspeed") && oc.isSet("departspeed")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_DEPARTSPEED, oc.getString("departspeed"));
    }
    //  arrivallane
    if (wasSet(VEHPARS_ARRIVALLANE_SET) && !defaultOptionOverrides(oc, "arrivallane")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALLANE, getArrivalLane());
    } else if (oc.exists("arrivallane") && oc.isSet("arrivallane")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALLANE, oc.getString("arrivallane"));
    }
    //  arrivalpos
    if (wasSet(VEHPARS_ARRIVALPOS_SET) && !defaultOptionOverrides(oc, "arrivalpos")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALPOS, getArrivalPos());
    } else if (oc.exists("arrivalpos") && oc.isSet("arrivalpos")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALPOS, oc.getString("arrivalpos"));
    }
    //  arrivalPosLat
    if (wasSet(VEHPARS_ARRIVALPOSLAT_SET)) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALPOS_LAT, getArrivalPosLat());
    }
    //  arrivalspeed
    if (wasSet(VEHPARS_ARRIVALSPEED_SET) && !defaultOptionOverrides(oc, "arrivalspeed")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALSPEED, getArrivalSpeed());
    } else if (oc.exists("arrivalspeed") && oc.isSet("arrivalspeed")) {
        dev.writeNonEmptyAttr(SUMO_ATTR_ARRIVALSPEED, oc.getString("arrivalspeed"));
    }
    // color
    if (wasSet(VEHPARS_COLOR_SET)) {
        dev.writeAttr(SUMO_ATTR_COLOR, color);
    }
    // line
    if (wasSet(VEHPARS_LINE_SET)) {
        dev.writeAttr(SUMO_ATTR_LINE, line);
    }
    // from TAZ
    if (wasSet(VEHPARS_FROM_TAZ_SET)) {
        dev.writeAttr(SUMO_ATTR_FROM_TAZ, fromTaz);
    }
    // to TAZ
    if (wasSet(VEHPARS_TO_TAZ_SET)) {
        dev.writeAttr(SUMO_ATTR_TO_TAZ, toTaz);
    }
    // person number
    if (wasSet(VEHPARS_PERSON_NUMBER_SET)) {
        dev.writeAttr(SUMO_ATTR_PERSON_NUMBER, personNumber);
    }
    // container number
    if (wasSet(VEHPARS_CONTAINER_NUMBER_SET)) {
        dev.writeAttr(SUMO_ATTR_CONTAINER_NUMBER, containerNumber);
    }
    // individual speedFactor
    if (wasSet(VEHPARS_SPEEDFACTOR_SET)) {
        dev.writeAttr(SUMO_ATTR_SPEEDFACTOR, speedFactor);
    }
}


SUMOVehicleParameter::Stop::Stop() :
    Parameterised(),
    startPos(0),
    endPos(0),
    duration(-1),
    until(-1),
    extension(-1),
    triggered(false),
    containerTriggered(false),
    joinTriggered(false),
    parking(false),
    friendlyPos(false),
    speed(0) {
}


void
SUMOVehicleParameter::Stop::write(OutputDevice& dev, bool close) const {
    dev.openTag(SUMO_TAG_STOP);
    if (busstop != "") {
        dev.writeAttr(SUMO_ATTR_BUS_STOP, busstop);
    }
    if (containerstop != "") {
        dev.writeAttr(SUMO_ATTR_CONTAINER_STOP, containerstop);
    }
    if (chargingStation != "") {
        dev.writeAttr(SUMO_ATTR_CHARGING_STATION, chargingStation);
    }
    if (parkingarea != "") {
        dev.writeAttr(SUMO_ATTR_PARKING_AREA, parkingarea);
    }
    if ((busstop == "") && (containerstop == "") && (parkingarea == "") && (chargingStation == "")) {
        dev.writeAttr(SUMO_ATTR_LANE, lane);
        if ((parametersSet & STOP_START_SET) != 0) {
            dev.writeAttr(SUMO_ATTR_STARTPOS, startPos);
        }
        if ((parametersSet & STOP_END_SET) != 0) {
            dev.writeAttr(SUMO_ATTR_ENDPOS, endPos);
        }
    }
    if ((parametersSet & STOP_ARRIVAL_SET) && (arrival >= 0)) {
        dev.writeAttr(SUMO_ATTR_ARRIVAL, time2string(arrival));
    }
    if ((parametersSet & STOP_DURATION_SET) && (duration >= 0)) {
        dev.writeAttr(SUMO_ATTR_DURATION, time2string(duration));
    }
    if ((parametersSet & STOP_UNTIL_SET) && (until >= 0)) {
        dev.writeAttr(SUMO_ATTR_UNTIL, time2string(until));
    }
    if ((parametersSet & STOP_EXTENSION_SET) && (extension >= 0)) {
        dev.writeAttr(SUMO_ATTR_EXTENSION, time2string(extension));
    }
    writeTriggers(dev);
    if ((parametersSet & STOP_PARKING_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_PARKING, parking);
    }
    if ((parametersSet & STOP_EXPECTED_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_EXPECTED, awaitedPersons);
    }
    if ((parametersSet & STOP_EXPECTED_CONTAINERS_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_EXPECTED_CONTAINERS, awaitedContainers);
    }
    if ((parametersSet & STOP_TRIP_ID_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_TRIP_ID, tripId);
    }
    if ((parametersSet & STOP_LINE_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_LINE, line);
    }
    if ((parametersSet & STOP_SPLIT_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_SPLIT, split);
    }
    if ((parametersSet & STOP_JOIN_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_JOIN, join);
    }
    if ((parametersSet & STOP_SPEED_SET) != 0) {
        dev.writeAttr(SUMO_ATTR_SPEED, speed);
    }
    // only write friendly position if is true
    if (friendlyPos == true) {
        dev.writeAttr(SUMO_ATTR_FRIENDLY_POS, friendlyPos);
    }
    // only write act type if isn't empty
    if (!actType.empty()) {
        dev.writeAttr(SUMO_ATTR_ACTTYPE, actType);
    }
    if (close) {
        dev.closeTag();
    }
}


bool
SUMOVehicleParameter::parseDepart(const std::string& val, const std::string& element, const std::string& id,
                                  SUMOTime& depart, DepartDefinition& dd, std::string& error) {
    if (val == "triggered") {
        dd = DEPART_TRIGGERED;
    } else if (val == "containerTriggered") {
        dd = DEPART_CONTAINER_TRIGGERED;
    } else if (val == "split") {
        dd = DEPART_SPLIT;
    } else if (val == "now") {
        // only used via TraCI. depart must be set by the calling code
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
            if (id.empty()) {
                error = "Invalid departure time for " + element + ". Must be one of (\"triggered\", \"containerTriggered\", \"now\", or a float >= 0)";
            } else {
                error = "Invalid departure time for " + element + " '" + id + "';\n must be one of (\"triggered\", \"containerTriggered\", \"now\", or a float >= 0)";
            }
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
    } else if (val == "first") {
        dld = DEPART_LANE_FIRST_ALLOWED;
    } else {
        try {
            lane = StringUtils::toInt(val);
            dld = DEPART_LANE_GIVEN;
            if (lane < 0) {
                ok = false;
            }
        } catch (...) {
            ok = false;
            lane = 0;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid departLane definition for " + element + ". Must be one of (\"random\", \"free\", \"allowed\", \"best\", \"first\", or an int>=0)";
        } else {
            error = "Invalid departLane definition for " + element + " '" + id + "';\n must be one of (\"random\", \"free\", \"allowed\", \"best\", \"first\", or an int>=0)";
        }
    }
    return ok;
}


bool
SUMOVehicleParameter::parseDepartPos(const std::string& val, const std::string& element, const std::string& id,
                                     double& pos, DepartPosDefinition& dpd, std::string& error) {
    bool ok = true;
    if (val == "random") {
        dpd = DEPART_POS_RANDOM;
    } else if (val == "random_free") {
        dpd = DEPART_POS_RANDOM_FREE;
    } else if (val == "free") {
        dpd = DEPART_POS_FREE;
    } else if (val == "base") {
        dpd = DEPART_POS_BASE;
    } else if (val == "last") {
        dpd = DEPART_POS_LAST;
    } else if (val == "stop") {
        dpd = DEPART_POS_STOP;
    } else {
        try {
            pos = StringUtils::toDouble(val);
            dpd = DEPART_POS_GIVEN;
        } catch (...) {
            ok = false;
            pos = 0;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid departPos definition for " + element + ". Must be one of (\"random\", \"random_free\", \"free\", \"base\", \"last\" or a float)";
        } else {
            error = "Invalid departPos definition for " + element + " '" + id + "';\n must be one of (\"random\", \"random_free\", \"free\", \"base\", \"last\" or a float)";
        }
    }
    return ok;
}


bool
SUMOVehicleParameter::parseDepartPosLat(const std::string& val, const std::string& element, const std::string& id,
                                        double& pos, DepartPosLatDefinition& dpd, std::string& error) {
    bool ok = true;
    if (val == "random") {
        dpd = DEPART_POSLAT_RANDOM;
    } else if (val == "random_free") {
        dpd = DEPART_POSLAT_RANDOM_FREE;
    } else if (val == "free") {
        dpd = DEPART_POSLAT_FREE;
    } else if (val == "right") {
        dpd = DEPART_POSLAT_RIGHT;
    } else if (val == "center") {
        dpd = DEPART_POSLAT_CENTER;
    } else if (val == "left") {
        dpd = DEPART_POSLAT_LEFT;
    } else {
        try {
            pos = StringUtils::toDouble(val);
            dpd = DEPART_POSLAT_GIVEN;
        } catch (...) {
            ok = false;
            pos = 0;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid departPosLat definition for " + element + ". Must be one of (\"random\", \"random_free\", \"free\", \"right\", \"center\", \"left\", or a float)";
        } else {
            error = "Invalid departPosLat definition for " + element + " '" + id + "';\n must be one of (\"random\", \"random_free\", \"free\", \"right\", \"center\", \"left\", or a float)";
        }
    }
    return ok;
}


bool
SUMOVehicleParameter::parseDepartSpeed(const std::string& val, const std::string& element, const std::string& id,
                                       double& speed, DepartSpeedDefinition& dsd, std::string& error) {
    bool ok = true;
    if (val == "random") {
        dsd = DEPART_SPEED_RANDOM;
    } else if (val == "max") {
        dsd = DEPART_SPEED_MAX;
    } else if (val == "desired") {
        dsd = DEPART_SPEED_DESIRED;
    } else if (val == "speedLimit") {
        dsd = DEPART_SPEED_LIMIT;
    } else {
        try {
            speed = StringUtils::toDouble(val);
            dsd = DEPART_SPEED_GIVEN;
            if (speed < 0) {
                ok = false;
            }
        } catch (...) {
            ok = false;
            speed = -1;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid departSpeed definition for " + element + ". Must be one of (\"random\", \"max\", or a float>=0)";
        } else {
            error = "Invalid departSpeed definition for " + element + " '" + id + "';\n must be one of (\"random\", \"max\", or a float>=0)";
        }
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
            lane = StringUtils::toInt(val);
            ald = ARRIVAL_LANE_GIVEN;
            if (lane < 0) {
                ok = false;
            }
        } catch (...) {
            ok = false;
            lane = 0;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid arrivalLane definition for " + element + ". Must be one of (\"current\", or an int>=0)";
        } else {
            error = "Invalid arrivalLane definition for " + element + " '" + id + "';\n must be one of (\"current\", or an int>=0)";
        }
    }
    return ok;
}


bool
SUMOVehicleParameter::parseArrivalPos(const std::string& val, const std::string& element, const std::string& id,
                                      double& pos, ArrivalPosDefinition& apd, std::string& error) {
    bool ok = true;
    if (val == "random") {
        apd = ARRIVAL_POS_RANDOM;
    } else if (val == "center") {
        apd = ARRIVAL_POS_CENTER;
    } else if (val == "max") {
        apd = ARRIVAL_POS_MAX;
    } else {
        try {
            pos = StringUtils::toDouble(val);
            apd = ARRIVAL_POS_GIVEN;
        } catch (...) {
            ok = false;
            pos = 0;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid arrivalPos definition for " + element + ". Must be one of (\"random\", \"max\", or a float)";
        } else {
            error = "Invalid arrivalPos definition for " + element + " '" + id + "';\n must be one of (\"random\", \"max\", or a float)";
        }
    }
    return ok;
}


bool
SUMOVehicleParameter::parseArrivalPosLat(const std::string& val, const std::string& element, const std::string& id,
        double& pos, ArrivalPosLatDefinition& apd, std::string& error) {
    bool ok = true;
    if (val == "right") {
        apd = ARRIVAL_POSLAT_RIGHT;
    } else if (val == "center") {
        apd = ARRIVAL_POSLAT_CENTER;
    } else if (val == "left") {
        apd = ARRIVAL_POSLAT_LEFT;
    } else {
        try {
            pos = StringUtils::toDouble(val);
            apd = ARRIVAL_POSLAT_GIVEN;
        } catch (...) {
            ok = false;
            pos = 0;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid arrivalPosLat definition for " + element + ". Must be one of (\"right\", \"center\", \"left\", or a float)";
        } else {
            error = "Invalid arrivalPosLat definition for " + element + " '" + id + "';\n must be one of (\"right\", \"center\", \"left\", or a float)";
        }
    }
    return ok;
}


bool
SUMOVehicleParameter::parseArrivalSpeed(const std::string& val, const std::string& element, const std::string& id,
                                        double& speed, ArrivalSpeedDefinition& asd, std::string& error) {
    bool ok = true;
    if (val == "current") {
        asd = ARRIVAL_SPEED_CURRENT;
    } else {
        try {
            speed = StringUtils::toDouble(val);
            if (speed < 0) {
                ok = false;
            }
            asd = ARRIVAL_SPEED_GIVEN;
        } catch (...) {
            ok = false;
            speed = -1;
        }
    }
    if (!ok) {
        if (id.empty()) {
            error = "Invalid arrivalSpeed definition for " + element + ". Must be one of (\"current\", or a float>=0)";
        } else {
            error = "Invalid arrivalSpeed definition for " + element + " '" + id + "';\n must be one of (\"current\", or a float>=0)";
        }
    }
    return ok;
}


double
SUMOVehicleParameter::interpretEdgePos(double pos, double maximumValue, SumoXMLAttr attr, const std::string& id) {
    if (pos < 0) {
        pos = maximumValue + pos;
    }
    if (pos > maximumValue) {
        WRITE_WARNING("Invalid " + toString(attr) + " " + toString(pos) + " given for " + id + ". Using edge end instead.");
        pos = maximumValue;
    }
    return pos;
}


bool
SUMOVehicleParameter::parsePersonModes(const std::string& modes, const std::string& element, const std::string& id, SVCPermissions& modeSet, std::string& error) {
    // separte modes in different strings, and check if modes are valid
    for (StringTokenizer st(modes); st.hasNext();) {
        const std::string mode = st.next();
        if (mode == "car") {
            modeSet |= SVC_PASSENGER;
        } else if (mode == "taxi") {
            modeSet |= SVC_TAXI;
        } else if (mode == "bicycle") {
            modeSet |= SVC_BICYCLE;
        } else if (mode == "public") {
            modeSet |= SVC_BUS;
        } else {
            if (id.empty()) {
                error = "Unknown person mode '" + mode + "'. Must be a combination of (\"car\", \"bicycle\" or \"public\")";
            } else {
                error = "Unknown person mode '" + mode + "' for " + element + " '" + id + "';\n must be a combination of (\"car\", \"bicycle\" or \"public\")";
            }
            return false;
        }
    }
    return true;
}


void
SUMOVehicleParameter::parseStopTriggers(const std::vector<std::string>& triggers, bool expectTrigger, Stop& stop) {
    if (triggers.size() == 0 && expectTrigger) {
        stop.triggered = true;
    }
    for (std::string val : triggers) {
        if (val == toString(SUMO_TAG_PERSON)) {
            stop.triggered = true;
        } else if (val == toString(SUMO_TAG_CONTAINER)) {
            stop.containerTriggered = true;
        } else if (val == toString(SUMO_ATTR_JOIN)) {
            stop.joinTriggered = true;
        } else {
            try {
                stop.triggered = StringUtils::toBool(val);
            } catch (BoolFormatException&) {
                WRITE_ERROR("Value of stop attribute 'trigger' must be 'person', 'container', 'join' or a boolean");
            }
        }
    }
}


void
SUMOVehicleParameter::Stop::writeTriggers(OutputDevice& dev) const {
    if ((parametersSet & STOP_TRIGGER_SET) != 0) {
        std::vector<std::string> triggers;
        if (triggered) {
            triggers.push_back(toString(SUMO_TAG_PERSON));
        }
        if (containerTriggered) {
            triggers.push_back(toString(SUMO_TAG_CONTAINER));
        }
        if (joinTriggered) {
            triggers.push_back(toString(SUMO_ATTR_JOIN));
        }
        dev.writeAttr(SUMO_ATTR_TRIGGERED, triggers);
    }
}


std::string
SUMOVehicleParameter::getDepart() const {
    if (departProcedure == DEPART_TRIGGERED) {
        return "triggered";
    } else if (departProcedure == DEPART_CONTAINER_TRIGGERED) {
        return "containerTriggered";
    } else if (departProcedure == DEPART_SPLIT) {
        return "split";
    } else {
        return time2string(depart);
    }
}


std::string
SUMOVehicleParameter::getDepartLane() const {
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
        case DEPART_LANE_FIRST_ALLOWED:
            val = "first";
            break;
        case DEPART_LANE_DEFAULT:
        default:
            break;
    }
    return val;
}


std::string
SUMOVehicleParameter::getDepartPos() const {
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
        case DEPART_POS_LAST:
            val = "last";
            break;
        case DEPART_POS_BASE:
            val = "base";
            break;
        case DEPART_POS_STOP:
            val = "stop";
            break;
        case DEPART_POS_DEFAULT:
        default:
            break;
    }
    return val;
}


std::string
SUMOVehicleParameter::getDepartPosLat() const {
    std::string val;
    switch (departPosLatProcedure) {
        case DEPART_POSLAT_GIVEN:
            val = toString(departPos);
            break;
        case DEPART_POSLAT_RANDOM:
            val = "random";
            break;
        case DEPART_POSLAT_RANDOM_FREE:
            val = "random_free";
            break;
        case DEPART_POSLAT_FREE:
            val = "free";
            break;
        case DEPART_POSLAT_RIGHT:
            val = "right";
            break;
        case DEPART_POSLAT_CENTER:
            val = "center";
            break;
        case DEPART_POSLAT_LEFT:
            val = "left";
            break;
        case DEPART_POSLAT_DEFAULT:
        default:
            break;
    }
    return val;
}


std::string
SUMOVehicleParameter::getDepartSpeed() const {
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
        case DEPART_SPEED_DESIRED:
            val = "desired";
            break;
        case DEPART_SPEED_LIMIT:
            val = "speedLimit";
            break;
        case DEPART_SPEED_DEFAULT:
        default:
            break;
    }
    return val;
}


std::string
SUMOVehicleParameter::getArrivalLane() const {
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
    return val;
}


std::string
SUMOVehicleParameter::getArrivalPos() const {
    std::string val;
    switch (arrivalPosProcedure) {
        case ARRIVAL_POS_GIVEN:
            val = toString(arrivalPos);
            break;
        case ARRIVAL_POS_RANDOM:
            val = "random";
            break;
        case ARRIVAL_POS_CENTER:
            val = "center";
            break;
        case ARRIVAL_POS_MAX:
            val = "max";
            break;
        case ARRIVAL_POS_DEFAULT:
        default:
            break;
    }
    return val;
}


std::string
SUMOVehicleParameter::getArrivalPosLat() const {
    std::string val;
    switch (arrivalPosLatProcedure) {
        case ARRIVAL_POSLAT_GIVEN:
            val = toString(arrivalPos);
            break;
        case ARRIVAL_POSLAT_RIGHT:
            val = "right";
            break;
        case ARRIVAL_POSLAT_CENTER:
            val = "center";
            break;
        case ARRIVAL_POSLAT_LEFT:
            val = "left";
            break;
        case ARRIVAL_POSLAT_DEFAULT:
        default:
            break;
    }
    return val;
}


std::string
SUMOVehicleParameter::getArrivalSpeed() const {
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
    return val;
}


/****************************************************************************/
