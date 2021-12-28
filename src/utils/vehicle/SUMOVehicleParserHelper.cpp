/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2008-2021 German Aerospace Center (DLR) and others.
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
/// @file    SUMOVehicleParserHelper.cpp
/// @author  Daniel Krajzewicz
/// @author  Jakob Erdmann
/// @author  Axel Wegener
/// @author  Michael Behrisch
/// @author  Laura Bieker
/// @date    Mon, 07.04.2008
///
// Helper methods for parsing vehicle attributes
/****************************************************************************/
#include <config.h>

#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/RandHelper.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/StringUtils.h>
#include <utils/common/ToString.h>
#include <utils/common/UtilExceptions.h>
#include <utils/options/OptionsCont.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/vehicle/SUMOVTypeParameter.h>
#include <utils/vehicle/SUMOVehicleParameter.h>
#include <utils/xml/SUMOSAXAttributes.h>

#include "SUMOVehicleParserHelper.h"


// ===========================================================================
// static members
// ===========================================================================

SUMOVehicleParserHelper::CFAttrMap SUMOVehicleParserHelper::allowedCFModelAttrs;
SUMOVehicleParserHelper::LCAttrMap SUMOVehicleParserHelper::allowedLCModelAttrs;
std::set<SumoXMLAttr> SUMOVehicleParserHelper::allowedJMAttrs;


// ===========================================================================
// method definitions
// ===========================================================================

SUMOVehicleParameter*
SUMOVehicleParserHelper::parseFlowAttributes(SumoXMLTag tag, const SUMOSAXAttributes& attrs, const bool hardFail, const bool needID,
        const SUMOTime beginDefault, const SUMOTime endDefault) {
    // first parse ID
    std::string id = parseID(attrs, tag);
    // check if ID is valid
    if (!needID || !id.empty()) {
        if (needID && !SUMOXMLDefinitions::isValidVehicleID(id)) {
            return handleVehicleError(hardFail, nullptr, "Invalid flow id '" + id + "'.");
        }
        // declare flags
        const bool hasPeriod = attrs.hasAttribute(SUMO_ATTR_PERIOD);
        const bool hasVPH = attrs.hasAttribute(SUMO_ATTR_VEHSPERHOUR);
        const bool hasPPH = attrs.hasAttribute(SUMO_ATTR_PERSONSPERHOUR);
        const bool hasCPH = attrs.hasAttribute(SUMO_ATTR_CONTAINERSPERHOUR);
        const bool hasPH = attrs.hasAttribute(SUMO_ATTR_PERHOUR);
        const bool hasXPH = hasVPH || hasPPH || hasCPH || hasPH;
        const bool hasProb = attrs.hasAttribute(SUMO_ATTR_PROB);
        const bool hasNumber = attrs.hasAttribute(SUMO_ATTR_NUMBER);
        const bool hasBegin = attrs.hasAttribute(SUMO_ATTR_BEGIN);
        const bool hasEnd = attrs.hasAttribute(SUMO_ATTR_END);
        SumoXMLAttr PERHOUR = SUMO_ATTR_PERHOUR;
        if (hasVPH) {
            PERHOUR = SUMO_ATTR_VEHSPERHOUR;
        }
        if (hasPPH) {
            PERHOUR = SUMO_ATTR_PERSONSPERHOUR;
        }
        if (hasCPH) {
            PERHOUR = SUMO_ATTR_CONTAINERSPERHOUR;
        }
        if (hasXPH && !(hasVPH ^ hasPPH ^ hasCPH ^ hasPH)) {
            return handleVehicleError(hardFail, nullptr,
                                      "At most one of '" + attrs.getName(SUMO_ATTR_PERHOUR) +
                                      "', '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                                      "', '" + attrs.getName(SUMO_ATTR_PERSONSPERHOUR) +
                                      "' and '" + attrs.getName(SUMO_ATTR_CONTAINERSPERHOUR) +
                                      "' has to be given in the definition of " + toString(tag) + " '" + id + "'.");
        }
        if (hasPeriod && hasXPH) {
            return handleVehicleError(hardFail, nullptr,
                                      "At most one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                                      "' and '" + attrs.getName(PERHOUR) +
                                      "' has to be given in the definition of "
                                      + toString(tag) + " '" + id + "'.");
        }
        if (hasPeriod && hasProb) {
            return handleVehicleError(hardFail, nullptr,
                                      "At most one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                                      "' and '" + attrs.getName(SUMO_ATTR_PROB) +
                                      "' has to be given in the definition of "
                                      + toString(tag) + " '" + id + "'.");
        }
        if (hasProb && hasXPH) {
            return handleVehicleError(hardFail, nullptr,
                                      "At most one of '" + attrs.getName(SUMO_ATTR_PROB) +
                                      "' and '" + attrs.getName(PERHOUR) +
                                      "' has to be given in the definition of "
                                      + toString(tag) + " '" + id + "'.");
        }
        if (hasPeriod || hasXPH || hasProb) {
            if (hasEnd && hasNumber) {
                return handleVehicleError(hardFail, nullptr,
                                          "If '" + attrs.getName(SUMO_ATTR_PERIOD) +
                                          "', '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                                          "', '" + attrs.getName(SUMO_ATTR_PERSONSPERHOUR) +
                                          "', '" + attrs.getName(SUMO_ATTR_CONTAINERSPERHOUR) +
                                          "', '" + attrs.getName(SUMO_ATTR_PERHOUR) +
                                          "' or '" + attrs.getName(SUMO_ATTR_PROB) +
                                          "' are given at most one of '" + attrs.getName(SUMO_ATTR_END) +
                                          "' and '" + attrs.getName(SUMO_ATTR_NUMBER) +
                                          "' are allowed in "
                                          + toString(tag) + " '" + id + "'.");
            }
        } else {
            if (!hasNumber) {
                return handleVehicleError(hardFail, nullptr,
                                          "At least one of '" + attrs.getName(SUMO_ATTR_PERIOD) +
                                          "', '" + attrs.getName(SUMO_ATTR_VEHSPERHOUR) +
                                          "', '" + attrs.getName(SUMO_ATTR_PERSONSPERHOUR) +
                                          "', '" + attrs.getName(SUMO_ATTR_CONTAINERSPERHOUR) +
                                          "', '" + attrs.getName(SUMO_ATTR_PERHOUR) +
                                          "', '" + attrs.getName(SUMO_ATTR_PROB) +
                                          "', and '" + attrs.getName(SUMO_ATTR_NUMBER) +
                                          "' is needed in "
                                          + toString(tag) + " '" + id + "'.");
            }
        }
        // declare flow
        SUMOVehicleParameter* flowParameter = new SUMOVehicleParameter();
        // set tag
        flowParameter->tag = tag;
        // set id
        flowParameter->id = id;
        if (tag == SUMO_TAG_PERSONFLOW) {
            flowParameter->vtypeid = DEFAULT_PEDTYPE_ID;
        }
        if (tag == SUMO_TAG_CONTAINERFLOW) {
            flowParameter->vtypeid = DEFAULT_CONTAINERTYPE_ID;
        }
        // parse common vehicle attributes
        try {
            parseCommonAttributes(attrs, flowParameter, tag);
        } catch (ProcessError& attributeError) {
            // check if continue handling another vehicles or stop handling
            if (hardFail) {
                throw ProcessError(attributeError.what());
            } else {
                return nullptr;
            }
        }
        // parse period
        if (hasPeriod) {
            bool ok = true;
            flowParameter->repetitionOffset = attrs.getSUMOTimeReporting(SUMO_ATTR_PERIOD, id.c_str(), ok);
            if (!ok) {
                return handleVehicleError(hardFail, flowParameter);
            } else {
                flowParameter->parametersSet |= VEHPARS_PERIOD_SET;
            }
        }
        // parse vehicle/person/container/etc per hour
        if (hasXPH) {
            bool ok = true;
            const double vph = attrs.get<double>(PERHOUR, id.c_str(), ok);
            if (!ok) {
                return handleVehicleError(hardFail, flowParameter);
            } else if (vph <= 0) {
                return handleVehicleError(hardFail, flowParameter, "Invalid repetition rate in the definition of " + toString(tag) + " '" + id + "'.");
            } else {
                if (vph != 0) {
                    flowParameter->repetitionOffset = TIME2STEPS(3600. / vph);
                }
                flowParameter->parametersSet |= VEHPARS_VPH_SET;
            }
        }
        // parse probability
        if (hasProb) {
            bool ok = true;
            flowParameter->repetitionProbability = attrs.get<double>(SUMO_ATTR_PROB, id.c_str(), ok);
            if (!ok) {
                return handleVehicleError(hardFail, flowParameter);
            } else if (flowParameter->repetitionProbability <= 0 || flowParameter->repetitionProbability > 1) {
                return handleVehicleError(hardFail, flowParameter, "Invalid repetition probability in the definition of " + toString(tag) + " '" + id + "'.");
            } else {
                flowParameter->parametersSet |= VEHPARS_PROB_SET;
            }
        }
        // set default begin
        flowParameter->depart = beginDefault;
        // parse begin
        if (hasBegin) {
            // first get begin
            bool ok = true;
            const std::string begin = attrs.get<std::string>(SUMO_ATTR_BEGIN, id.c_str(), ok);
            if (!ok) {
                return handleVehicleError(hardFail, flowParameter);
            } else {
                // parse begin
                std::string errorMsg;
                if (!SUMOVehicleParameter::parseDepart(begin, toString(tag), id, flowParameter->depart, flowParameter->departProcedure, errorMsg, "begin")) {
                    return handleVehicleError(hardFail, flowParameter, errorMsg);
                }
            }
        }
        if (flowParameter->depart < 0) {
            return handleVehicleError(hardFail, flowParameter, "Negative begin time in the definition of " + toString(tag) + " '" + id + "'.");
        }
        // set default end
        flowParameter->repetitionEnd = endDefault;
        if (flowParameter->repetitionEnd < 0) {
            flowParameter->repetitionEnd = SUMOTime_MAX;
        }
        // parse end
        if (hasEnd) {
            bool ok = true;
            flowParameter->repetitionEnd = attrs.getSUMOTimeReporting(SUMO_ATTR_END, id.c_str(), ok);
            if (!ok) {
                return handleVehicleError(hardFail, flowParameter);
            } else {
                flowParameter->parametersSet |= VEHPARS_END_SET;
            }
        } else if (flowParameter->departProcedure == DEPART_TRIGGERED) {
            if (!hasNumber) {
                return handleVehicleError(hardFail, flowParameter, toString(tag) + " '" + id + "' with triggered begin must define 'number'.");
            } else {
                flowParameter->repetitionEnd = flowParameter->depart;
            }
        } else if ((endDefault == SUMOTime_MAX || endDefault < 0) && (!hasNumber || (!hasProb && !hasPeriod && !hasXPH))) {
            WRITE_WARNING("Undefined end for " + toString(tag) + " '" + id + "', defaulting to 24hour duration.");
            flowParameter->repetitionEnd = flowParameter->depart + TIME2STEPS(24 * 3600);
        }
        if (flowParameter->repetitionEnd < flowParameter->depart) {
            std::string flow = toString(tag);
            flow[0] = (char)::toupper((char)flow[0]);
            return handleVehicleError(hardFail, flowParameter, flow + " '" + id + "' ends before its begin time.");
        }
        // parse number
        if (hasNumber) {
            bool ok = true;
            flowParameter->repetitionNumber = attrs.get<int>(SUMO_ATTR_NUMBER, id.c_str(), ok);
            if (!ok) {
                return handleVehicleError(hardFail, flowParameter);
            } else {
                flowParameter->parametersSet |= VEHPARS_NUMBER_SET;
                if (flowParameter->repetitionNumber == 0) {
                    std::string flow = toString(tag);
                    flow[0] = (char)::toupper((char)flow[0]);
                    WRITE_WARNING(flow + " '" + id + "' has no instances; will skip it.");
                } else {
                    if (flowParameter->repetitionNumber < 0) {
                        return handleVehicleError(hardFail, flowParameter, "Negative repetition number in the definition of " + toString(tag) + " '" + id + "'.");
                    }
                    if (flowParameter->repetitionOffset < 0) {
                        flowParameter->repetitionOffset = (flowParameter->repetitionEnd - flowParameter->depart) / flowParameter->repetitionNumber;
                    }
                }
                flowParameter->repetitionEnd = flowParameter->depart + flowParameter->repetitionNumber * flowParameter->repetitionOffset;
            }
        } else {
            // interpret repetitionNumber
            if (flowParameter->repetitionProbability > 0) {
                flowParameter->repetitionNumber = std::numeric_limits<int>::max();
            } else {
                if (flowParameter->repetitionOffset <= 0) {
                    return handleVehicleError(hardFail, flowParameter, "Invalid repetition rate in the definition of " + toString(tag) + " '" + id + "'.");
                }
                if (flowParameter->repetitionEnd == SUMOTime_MAX) {
                    flowParameter->repetitionNumber = std::numeric_limits<int>::max();
                } else {
                    const double repLength = (double)(flowParameter->repetitionEnd - flowParameter->depart);
                    flowParameter->repetitionNumber = (int)ceil(repLength / flowParameter->repetitionOffset);
                }
            }
        }
        // all ok, then return flow parameter
        return flowParameter;
    } else {
        return handleVehicleError(hardFail, nullptr, toString(tag) + " cannot be created");
    }
}


SUMOVehicleParameter*
SUMOVehicleParserHelper::parseVehicleAttributes(int element, const SUMOSAXAttributes& attrs, const bool hardFail, const bool optionalID, const bool skipDepart) {
    // declare vehicle ID
    std::string id;
    // for certain vehicles, ID can be optional
    if (optionalID) {
        bool ok = true;
        id = attrs.getOpt<std::string>(SUMO_ATTR_ID, nullptr, ok, "");
        if (!ok) {
            return handleVehicleError(hardFail, nullptr);
        }
    } else {
        // parse ID
        id = parseID(attrs, (SumoXMLTag)element);
    }
    // only continue if id is valid, or if is optional
    if (optionalID || !id.empty()) {
        // declare vehicle parameter
        SUMOVehicleParameter* vehicleParameter = new SUMOVehicleParameter();
        vehicleParameter->id = id;
        if (element == SUMO_TAG_PERSON) {
            vehicleParameter->vtypeid = DEFAULT_PEDTYPE_ID;
        } else if (element == SUMO_TAG_CONTAINER) {
            vehicleParameter->vtypeid = DEFAULT_CONTAINERTYPE_ID;
        }
        // parse common attributes
        try {
            parseCommonAttributes(attrs, vehicleParameter, (SumoXMLTag)element);
        } catch (ProcessError& attributeError) {
            // check if continue handling another vehicles or stop handling
            if (hardFail) {
                throw ProcessError(attributeError.what());
            } else {
                return nullptr;
            }
        }
        // check depart
        if (!skipDepart) {
            bool ok = true;
            const std::string helper = attrs.get<std::string>(SUMO_ATTR_DEPART, vehicleParameter->id.c_str(), ok);
            if (!ok) {
                return handleVehicleError(hardFail, vehicleParameter);
            }
            // now parse depart
            std::string departErrorMsg;
            if (!SUMOVehicleParameter::parseDepart(helper, "vehicle", vehicleParameter->id, vehicleParameter->depart, vehicleParameter->departProcedure, departErrorMsg)) {
                return handleVehicleError(hardFail, vehicleParameter, departErrorMsg);
            }
        }
        // set tag
        vehicleParameter->tag = (SumoXMLTag)element;
        // all ok, then return vehicleParameter
        return vehicleParameter;
    } else {
        return handleVehicleError(hardFail, nullptr, toString((SumoXMLTag)element) + " cannot be created");
    }
}


std::string
SUMOVehicleParserHelper::parseID(const SUMOSAXAttributes& attrs, const SumoXMLTag element) {
    bool ok = true;
    std::string id;
    // first check if attrs contain an ID
    if (attrs.hasAttribute(SUMO_ATTR_ID)) {
        id = attrs.get<std::string>(SUMO_ATTR_ID, nullptr, ok);
        if (SUMOXMLDefinitions::isValidVehicleID(id)) {
            return id;
        } else if (id.empty()) {
            // add extra information for empty IDs
            WRITE_ERROR("Invalid " + toString(element) + " id '" + id + "'.");
        } else {
            WRITE_ERROR("Invalid " + toString(element) + " id '" + id + "'. Contains invalid characters.");
        }
    } else {
        WRITE_ERROR("Attribute '" + toString(SUMO_ATTR_ID) + "' is missing in definition of " + toString(element));
    }
    // return empty (invalid) ID
    return "";
}


void
SUMOVehicleParserHelper::parseCommonAttributes(const SUMOSAXAttributes& attrs, SUMOVehicleParameter* ret, SumoXMLTag tag) {
    const std::string element = toString(tag);
    //ret->refid = attrs.getStringSecure(SUMO_ATTR_REFID, "");
    // parse route information
    if (attrs.hasAttribute(SUMO_ATTR_ROUTE)) {
        bool ok = true;
        ret->routeid = attrs.get<std::string>(SUMO_ATTR_ROUTE, ret->id.c_str(), ok);
        if (ok) {
            ret->parametersSet |= VEHPARS_ROUTE_SET; // !!! needed?
        } else {
            handleVehicleError(true, ret);
        }
    }
    // parse type information
    if (attrs.hasAttribute(SUMO_ATTR_TYPE)) {
        bool ok = true;
        ret->vtypeid = attrs.get<std::string>(SUMO_ATTR_TYPE, ret->id.c_str(), ok);
        if (ok) {
            ret->parametersSet |= VEHPARS_VTYPE_SET; // !!! needed?
        } else {
            handleVehicleError(true, ret);
        }
    }
    // parse line information
    if (attrs.hasAttribute(SUMO_ATTR_LINE)) {
        bool ok = true;
        ret->line = attrs.get<std::string>(SUMO_ATTR_LINE, ret->id.c_str(), ok);
        if (ok) {
            ret->parametersSet |= VEHPARS_LINE_SET; // !!! needed?
        } else {
            handleVehicleError(true, ret);
        }
    }
    // parse zone information
    if (attrs.hasAttribute(SUMO_ATTR_FROM_TAZ)) {
        bool ok = true;
        ret->fromTaz = attrs.get<std::string>(SUMO_ATTR_FROM_TAZ, ret->id.c_str(), ok);
        if (ok) {
            ret->parametersSet |= VEHPARS_FROM_TAZ_SET;
        } else {
            handleVehicleError(true, ret);
        }
    }
    if (attrs.hasAttribute(SUMO_ATTR_TO_TAZ)) {
        bool ok = true;
        ret->toTaz = attrs.get<std::string>(SUMO_ATTR_TO_TAZ, ret->id.c_str(), ok);
        if (ok) {
            ret->parametersSet |= VEHPARS_TO_TAZ_SET;
        } else {
            handleVehicleError(true, ret);
        }
    }
    // parse reroute information
    if (attrs.hasAttribute(SUMO_ATTR_REROUTE)) {
        bool ok = true;
        if (attrs.get<bool>(SUMO_ATTR_REROUTE, ret->id.c_str(), ok)) {
            if (ok) {
                ret->parametersSet |= VEHPARS_FORCE_REROUTE;
            } else {
                handleVehicleError(true, ret);
            }
        }
    }
    // parse depart lane information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTLANE)) {
        bool ok = true;
        const std::string departLaneStr = attrs.get<std::string>(SUMO_ATTR_DEPARTLANE, ret->id.c_str(), ok);
        int lane;
        DepartLaneDefinition dld;
        std::string error;
        if (SUMOVehicleParameter::parseDepartLane(departLaneStr, element, ret->id, lane, dld, error)) {
            ret->parametersSet |= VEHPARS_DEPARTLANE_SET;
            ret->departLane = lane;
            ret->departLaneProcedure = dld;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS)) {
        bool ok = true;
        const std::string departPosStr = attrs.get<std::string>(SUMO_ATTR_DEPARTPOS, ret->id.c_str(), ok);
        double pos;
        DepartPosDefinition dpd;
        std::string error;
        if (SUMOVehicleParameter::parseDepartPos(departPosStr, element, ret->id, pos, dpd, error)) {
            ret->parametersSet |= VEHPARS_DEPARTPOS_SET;
            ret->departPos = pos;
            ret->departPosProcedure = dpd;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse lateral depart position information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTPOS_LAT)) {
        bool ok = true;
        const std::string departPosLatStr = attrs.get<std::string>(SUMO_ATTR_DEPARTPOS_LAT, ret->id.c_str(), ok);
        double pos;
        DepartPosLatDefinition dpd;
        std::string error;
        if (SUMOVehicleParameter::parseDepartPosLat(departPosLatStr, element, ret->id, pos, dpd, error)) {
            ret->parametersSet |= VEHPARS_DEPARTPOSLAT_SET;
            ret->departPosLat = pos;
            ret->departPosLatProcedure = dpd;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse depart speed information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTSPEED)) {
        bool ok = true;
        const std::string departSpeed = attrs.get<std::string>(SUMO_ATTR_DEPARTSPEED, ret->id.c_str(), ok);
        double speed;
        DepartSpeedDefinition dsd;
        std::string error;
        if (SUMOVehicleParameter::parseDepartSpeed(departSpeed, element, ret->id, speed, dsd, error)) {
            ret->parametersSet |= VEHPARS_DEPARTSPEED_SET;
            ret->departSpeed = speed;
            ret->departSpeedProcedure = dsd;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse depart edge information
    if (attrs.hasAttribute(SUMO_ATTR_DEPARTEDGE)) {
        bool ok = true;
        const std::string departEdgeStr = attrs.get<std::string>(SUMO_ATTR_DEPARTEDGE, ret->id.c_str(), ok);
        int edgeIndex;
        RouteIndexDefinition rid;
        std::string error;
        if (SUMOVehicleParameter::parseRouteIndex(departEdgeStr, element, ret->id, SUMO_ATTR_DEPARTEDGE, edgeIndex, rid, error)) {
            ret->parametersSet |= VEHPARS_DEPARTEDGE_SET;
            ret->departEdge = edgeIndex;
            ret->departEdgeProcedure = rid;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse arrival lane information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALLANE)) {
        bool ok = true;
        const std::string arrivalLaneStr = attrs.get<std::string>(SUMO_ATTR_ARRIVALLANE, ret->id.c_str(), ok);
        int lane;
        ArrivalLaneDefinition ald;
        std::string error;
        if (SUMOVehicleParameter::parseArrivalLane(arrivalLaneStr, element, ret->id, lane, ald, error)) {
            ret->parametersSet |= VEHPARS_ARRIVALLANE_SET;
            ret->arrivalLane = lane;
            ret->arrivalLaneProcedure = ald;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS)) {
        bool ok = true;
        const std::string arrivalPosStr = attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS, ret->id.c_str(), ok);
        double pos;
        ArrivalPosDefinition apd;
        std::string error;
        if (SUMOVehicleParameter::parseArrivalPos(arrivalPosStr, element, ret->id, pos, apd, error)) {
            ret->parametersSet |= VEHPARS_ARRIVALPOS_SET;
            ret->arrivalPos = pos;
            ret->arrivalPosProcedure = apd;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse lateral arrival position information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALPOS_LAT)) {
        bool ok = true;
        const std::string arrivalPosLatStr = attrs.get<std::string>(SUMO_ATTR_ARRIVALPOS_LAT, ret->id.c_str(), ok);
        double pos;
        ArrivalPosLatDefinition apd;
        std::string error;
        if (SUMOVehicleParameter::parseArrivalPosLat(arrivalPosLatStr, element, ret->id, pos, apd, error)) {
            ret->parametersSet |= VEHPARS_ARRIVALPOSLAT_SET;
            ret->arrivalPosLat = pos;
            ret->arrivalPosLatProcedure = apd;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse arrival speed information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALSPEED)) {
        bool ok = true;
        std::string arrivalSpeedStr = attrs.get<std::string>(SUMO_ATTR_ARRIVALSPEED, ret->id.c_str(), ok);
        double speed;
        ArrivalSpeedDefinition asd;
        std::string error;
        if (SUMOVehicleParameter::parseArrivalSpeed(arrivalSpeedStr, element, ret->id, speed, asd, error)) {
            ret->parametersSet |= VEHPARS_ARRIVALSPEED_SET;
            ret->arrivalSpeed = speed;
            ret->arrivalSpeedProcedure = asd;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse arrival edge information
    if (attrs.hasAttribute(SUMO_ATTR_ARRIVALEDGE)) {
        bool ok = true;
        std::string arrivalEdgeStr = attrs.get<std::string>(SUMO_ATTR_ARRIVALEDGE, ret->id.c_str(), ok);
        int edgeIndex;
        RouteIndexDefinition rid;
        std::string error;
        if (SUMOVehicleParameter::parseRouteIndex(arrivalEdgeStr, element, ret->id, SUMO_ATTR_ARRIVALEDGE, edgeIndex, rid, error)) {
            ret->parametersSet |= VEHPARS_ARRIVALEDGE_SET;
            ret->arrivalEdge = edgeIndex;
            ret->arrivalEdgeProcedure = rid;
        } else {
            handleVehicleError(true, ret, error);
        }
    }
    // parse color
    if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
        bool ok = true;
        ret->color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, ret->id.c_str(), ok);
        if (ok) {
            ret->parametersSet |= VEHPARS_COLOR_SET;
        } else {
            handleVehicleError(true, ret, "Invalid RGBColor format");
        }
    } else {
        ret->color = RGBColor::DEFAULT_COLOR;
    }
    // parse person number
    if (attrs.hasAttribute(SUMO_ATTR_PERSON_NUMBER)) {
        bool ok = true;
        int personNumber = attrs.get<int>(SUMO_ATTR_PERSON_NUMBER, ret->id.c_str(), ok);
        if (!ok) {
            handleVehicleError(true, ret);
        } else if (personNumber >= 0) {
            ret->parametersSet |= VEHPARS_PERSON_NUMBER_SET;
            ret->personNumber = personNumber;
        } else {
            handleVehicleError(true, ret, toString(SUMO_ATTR_PERSON_NUMBER) + " cannot be negative");
        }
    }
    // parse container number
    if (attrs.hasAttribute(SUMO_ATTR_CONTAINER_NUMBER)) {
        bool ok = true;
        int containerNumber = attrs.get<int>(SUMO_ATTR_CONTAINER_NUMBER, ret->id.c_str(), ok);
        if (!ok) {
            handleVehicleError(true, ret);
        } else if (containerNumber >= 0) {
            ret->parametersSet |= VEHPARS_CONTAINER_NUMBER_SET;
            ret->containerNumber = containerNumber;
        } else {
            handleVehicleError(true, ret, toString(SUMO_ATTR_CONTAINER_NUMBER) + " cannot be negative");
        }
    }
    // parse individual speedFactor
    if (attrs.hasAttribute(SUMO_ATTR_SPEEDFACTOR)) {
        bool ok = true;
        double speedFactor = attrs.get<double>(SUMO_ATTR_SPEEDFACTOR, ret->id.c_str(), ok);
        if (!ok) {
            handleVehicleError(true, ret);
        } else if (speedFactor > 0) {
            ret->parametersSet |= VEHPARS_SPEEDFACTOR_SET;
            ret->speedFactor = speedFactor;
        } else {
            handleVehicleError(true, ret, toString(SUMO_ATTR_SPEEDFACTOR) + " must be positive");
        }
    }
    // parse speed (only used by calibrators flow)
    // also used by vehicle in saved state but this is parsed elsewhere
    if (tag == SUMO_TAG_FLOW && attrs.hasAttribute(SUMO_ATTR_SPEED)) {
        bool ok = true;
        double calibratorSpeed = attrs.get<double>(SUMO_ATTR_SPEED, ret->id.c_str(), ok);
        if (!ok) {
            handleVehicleError(true, ret);
        } else if (calibratorSpeed >= 0 || calibratorSpeed == -1) {
            ret->parametersSet |= VEHPARS_CALIBRATORSPEED_SET;
            ret->calibratorSpeed = calibratorSpeed;
        } else {
            handleVehicleError(true, ret, toString(SUMO_ATTR_SPEED) + " may not be negative");
        }
    }
    /*/ parse via
    if (attrs.hasAttribute(SUMO_ATTR_VIA)) {
        ret->setParameter |= VEHPARS_VIA_SET;
        SUMOSAXAttributes::parseStringVector(attrs.get<std::string>(SUMO_ATTR_VIA, ret->id.c_str(), ok), ret->via);
    }
    */
}


SUMOVTypeParameter*
SUMOVehicleParserHelper::beginVTypeParsing(const SUMOSAXAttributes& attrs, const bool hardFail, const std::string& file) {
    // first obtain ID
    std::string id = parseID(attrs, SUMO_TAG_VTYPE);
    // check if ID is valid
    if (!id.empty()) {
        SUMOVehicleClass vClass = SVC_PASSENGER;
        if (attrs.hasAttribute(SUMO_ATTR_VCLASS)) {
            vClass = parseVehicleClass(attrs, id);
        }
        // create vType
        SUMOVTypeParameter* vType = new SUMOVTypeParameter(id, vClass);
        // parse attributes
        if (attrs.hasAttribute(SUMO_ATTR_VCLASS)) {
            vType->parametersSet |= VTYPEPARS_VEHICLECLASS_SET;
        }
        if (attrs.hasAttribute(SUMO_ATTR_LENGTH)) {
            bool ok = true;
            const double length = attrs.get<double>(SUMO_ATTR_LENGTH, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (length <= 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_LENGTH) + " must be greater than 0");
            } else {
                vType->length = length;
                vType->parametersSet |= VTYPEPARS_LENGTH_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_MINGAP)) {
            bool ok = true;
            const double minGap = attrs.get<double>(SUMO_ATTR_MINGAP, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (minGap < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_MINGAP) + " must be equal or greater than 0");
            } else {
                vType->minGap = minGap;
                vType->parametersSet |= VTYPEPARS_MINGAP_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED)) {
            bool ok = true;
            const double maxSpeed = attrs.get<double>(SUMO_ATTR_MAXSPEED, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (maxSpeed <= 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_MAXSPEED) + " must be greater than 0");
            } else {
                vType->maxSpeed = maxSpeed;
                vType->parametersSet |= VTYPEPARS_MAXSPEED_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_SPEEDFACTOR)) {
            bool ok = true;
            vType->speedFactor.parse(attrs.get<std::string>(SUMO_ATTR_SPEEDFACTOR, vType->id.c_str(), ok), hardFail);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else {
                vType->parametersSet |= VTYPEPARS_SPEEDFACTOR_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_SPEEDDEV)) {
            bool ok = true;
            const double speedDev = attrs.get<double>(SUMO_ATTR_SPEEDDEV, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (speedDev < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_SPEEDDEV) + " must be equal or greater than 0");
            } else {
                vType->speedFactor.getParameter()[1] = speedDev;
                vType->parametersSet |= VTYPEPARS_SPEEDFACTOR_SET;
            }
        }
        // validate speed distribution
        std::string error;
        if (!vType->speedFactor.isValid(error)) {
            return handleVehicleTypeError(hardFail, vType, "Invalid speed distribution when parsing vType '" + vType->id + "' (" + error + ")");
        }
        if (attrs.hasAttribute(SUMO_ATTR_ACTIONSTEPLENGTH)) {
            bool ok = true;
            const double actionStepLengthSecs = attrs.get<double>(SUMO_ATTR_ACTIONSTEPLENGTH, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else {
                // processActionStepLength(...) function includes warnings
                vType->actionStepLength = processActionStepLength(actionStepLengthSecs);
                vType->parametersSet |= VTYPEPARS_ACTIONSTEPLENGTH_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_EMISSIONCLASS)) {
            bool ok = true;
            const std::string parsedEmissionClass = attrs.getOpt<std::string>(SUMO_ATTR_EMISSIONCLASS, id.c_str(), ok, "");
            // check if emission class is correct
            try {
                vType->emissionClass = PollutantsInterface::getClassByName(parsedEmissionClass);
                vType->parametersSet |= VTYPEPARS_EMISSIONCLASS_SET;
            } catch (...) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_EMISSIONCLASS) + " with name '" + parsedEmissionClass + "' doesn't exist.");
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_IMPATIENCE)) {
            bool ok = true;
            const std::string impatienceStr = attrs.get<std::string>(SUMO_ATTR_IMPATIENCE, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (impatienceStr == "off") {
                vType->impatience = -std::numeric_limits<double>::max();
            } else {
                const double impatienceDouble = attrs.get<double>(SUMO_ATTR_IMPATIENCE, vType->id.c_str(), ok);
                if (!ok) {
                    return handleVehicleTypeError(hardFail, vType);
                } else {
                    vType->impatience = impatienceDouble;
                    vType->parametersSet |= VTYPEPARS_IMPATIENCE_SET;
                }
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_WIDTH)) {
            bool ok = true;
            const double width = attrs.get<double>(SUMO_ATTR_WIDTH, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (width <= 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_WIDTH) + " must be greater than 0");
            } else {
                vType->width = width;
                vType->parametersSet |= VTYPEPARS_WIDTH_SET;
                if (vClass == SVC_PEDESTRIAN
                        && OptionsCont::getOptions().exists("pedestrian.striping.stripe-width")
                        && OptionsCont::getOptions().getString("pedestrian.model") == "striping"
                        && OptionsCont::getOptions().getFloat("pedestrian.striping.stripe-width") < vType->width) {
                    WRITE_WARNINGF("Pedestrian vType '%' width % is larger than pedestrian.striping.stripe-width and this may cause collisions with vehicles.", id, vType->width);
                }
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_HEIGHT)) {
            bool ok = true;
            const double height = attrs.get<double>(SUMO_ATTR_HEIGHT, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (height < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_HEIGHT) + " must be equal or greater than 0");
            } else {
                vType->height = height;
                vType->parametersSet |= VTYPEPARS_HEIGHT_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_GUISHAPE)) {
            vType->shape = parseGuiShape(attrs, vType->id);
            if (vType->shape != SVS_UNKNOWN) {
                vType->parametersSet |= VTYPEPARS_SHAPE_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_OSGFILE)) {
            bool ok = true;
            const std::string osgFile = attrs.get<std::string>(SUMO_ATTR_OSGFILE, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else {
                vType->osgFile = osgFile;
                vType->parametersSet |= VTYPEPARS_OSGFILE_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_IMGFILE)) {
            bool ok = true;
            std::string imgFile = attrs.get<std::string>(SUMO_ATTR_IMGFILE, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else {
                // check relative path
                if ((imgFile != "") && !FileHelpers::isAbsolute(imgFile)) {
                    imgFile = FileHelpers::getConfigurationRelative(file, imgFile);
                }
                vType->imgFile = imgFile;
                vType->parametersSet |= VTYPEPARS_IMGFILE_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_COLOR)) {
            bool ok = true;
            const RGBColor color = attrs.get<RGBColor>(SUMO_ATTR_COLOR, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else {
                vType->color = color;
                vType->parametersSet |= VTYPEPARS_COLOR_SET;
            }
        } else {
            vType->color = RGBColor::YELLOW;
        }
        if (attrs.hasAttribute(SUMO_ATTR_PROB)) {
            bool ok = true;
            const double defaultProbability = attrs.get<double>(SUMO_ATTR_PROB, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (defaultProbability < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_PROB) + " must be equal or greater than 0");
            } else {
                vType->defaultProbability = defaultProbability;
                vType->parametersSet |= VTYPEPARS_PROBABILITY_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_LANE_CHANGE_MODEL)) {
            bool ok = true;
            std::string lcmS = attrs.get<std::string>(SUMO_ATTR_LANE_CHANGE_MODEL, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (lcmS == "JE2013") {
                WRITE_WARNING("Lane change model 'JE2013' is deprecated. Using default model instead.");
                lcmS = "default";
            }
            if (SUMOXMLDefinitions::LaneChangeModels.hasString(lcmS)) {
                vType->lcModel = SUMOXMLDefinitions::LaneChangeModels.get(lcmS);
                vType->parametersSet |= VTYPEPARS_LANE_CHANGE_MODEL_SET;
            } else {
                return handleVehicleTypeError(hardFail, vType, "Unknown lane change model '" + lcmS + "' when parsing vType '" + vType->id + "'");
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_CAR_FOLLOW_MODEL)) {
            bool ok = true;
            const std::string cfmValue = attrs.get<std::string>(SUMO_ATTR_CAR_FOLLOW_MODEL, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (SUMOXMLDefinitions::CarFollowModels.hasString(cfmValue)) {
                vType->cfModel = SUMOXMLDefinitions::CarFollowModels.get(cfmValue);
                vType->parametersSet |= VTYPEPARS_CAR_FOLLOW_MODEL;
            } else {
                return handleVehicleTypeError(hardFail, vType, "Unknown car following model '" + cfmValue + "' when parsing vType '" + vType->id + "'");
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_PERSON_CAPACITY)) {
            bool ok = true;
            const int personCapacity = attrs.get<int>(SUMO_ATTR_PERSON_CAPACITY, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (personCapacity < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_PERSON_CAPACITY) + " must be equal or greater than 0");
            } else {
                vType->personCapacity = personCapacity;
                vType->parametersSet |= VTYPEPARS_PERSON_CAPACITY;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_CONTAINER_CAPACITY)) {
            bool ok = true;
            const int containerCapacity = attrs.get<int>(SUMO_ATTR_CONTAINER_CAPACITY, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (containerCapacity < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_CONTAINER_CAPACITY) + " must be equal or greater than 0");
            } else {
                vType->containerCapacity = containerCapacity;
                vType->parametersSet |= VTYPEPARS_CONTAINER_CAPACITY;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_BOARDING_DURATION)) {
            bool ok = true;
            const SUMOTime boardingDuration = attrs.getSUMOTimeReporting(SUMO_ATTR_BOARDING_DURATION, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (boardingDuration < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_BOARDING_DURATION) + " must be equal or greater than 0");
            } else {
                vType->boardingDuration = boardingDuration;
                vType->parametersSet |= VTYPEPARS_BOARDING_DURATION;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_LOADING_DURATION)) {
            bool ok = true;
            const SUMOTime loadingDuration = attrs.getSUMOTimeReporting(SUMO_ATTR_LOADING_DURATION, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (loadingDuration < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_LOADING_DURATION) + " must be equal or greater than 0");
            } else {
                vType->loadingDuration = loadingDuration;
                vType->parametersSet |= VTYPEPARS_LOADING_DURATION;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_MAXSPEED_LAT)) {
            bool ok = true;
            const double maxSpeedLat = attrs.get<double>(SUMO_ATTR_MAXSPEED_LAT, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (maxSpeedLat <= 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_MAXSPEED_LAT) + " must be greater than 0");
            } else {
                vType->maxSpeedLat = maxSpeedLat;
                vType->parametersSet |= VTYPEPARS_MAXSPEED_LAT_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_MINGAP_LAT)) {
            bool ok = true;
            const double minGapLat = attrs.get<double>(SUMO_ATTR_MINGAP_LAT, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (minGapLat < 0) {
                return handleVehicleTypeError(hardFail, vType, toString(SUMO_ATTR_MINGAP_LAT) + " must be equal or greater than 0");
            } else {
                vType->minGapLat = minGapLat;
                vType->parametersSet |= VTYPEPARS_MINGAP_LAT_SET;
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_LATALIGNMENT)) {
            bool ok = true;
            const std::string alignS = attrs.get<std::string>(SUMO_ATTR_LATALIGNMENT, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else {
                double lao;
                LatAlignmentDefinition lad;
                if (SUMOVTypeParameter::parseLatAlignment(alignS, lao, lad)) {
                    vType->latAlignmentOffset = lao;
                    vType->latAlignmentProcedure = lad;
                    vType->parametersSet |= VTYPEPARS_LATALIGNMENT_SET;
                } else {
                    return handleVehicleTypeError(hardFail, vType, "Unknown lateral alignment '" + alignS + "' when parsing vType '" + vType->id + "';\n must be one of (\"right\", \"center\", \"arbitrary\", \"nice\", \"compact\", \"left\" or a float)");
                }
            }
        }
        if (attrs.hasAttribute(SUMO_ATTR_MANEUVER_ANGLE_TIMES)) {
            bool ok = true;
            const std::string angleTimesS = attrs.get<std::string>(SUMO_ATTR_MANEUVER_ANGLE_TIMES, vType->id.c_str(), ok);
            if (!ok) {
                return handleVehicleTypeError(hardFail, vType);
            } else if (parseAngleTimesMap(vType, angleTimesS)) {
                vType->parametersSet |= VTYPEPARS_MANEUVER_ANGLE_TIMES_SET;
            } else {
                return handleVehicleTypeError(hardFail, vType, "Invalid manoeuver angle times map for vType '" + vType->id + "'");
            }
        }
        // try to parse Car Following Model params
        if (!parseCFMParams(vType, vType->cfModel, attrs, false)) {
            return handleVehicleTypeError(hardFail, vType, "Invalid parsing embedded VType");
        }
        // try to parse Lane Change Model params
        if (!parseLCParams(vType, vType->lcModel, attrs)) {
            return handleVehicleTypeError(hardFail, vType, "Invalid Lane Change Model Parameters");
        }
        // try to Junction Model params
        if (!parseJMParams(vType, attrs)) {
            return handleVehicleTypeError(hardFail, vType, "Invalid Junction Model Parameters");
        }
        // all ok, then return vType
        return vType;
    } else {
        return handleVehicleTypeError(hardFail, nullptr, "VType cannot be created");
    }
}


bool
SUMOVehicleParserHelper::parseAngleTimesMap(SUMOVTypeParameter* vtype, const std::string atm) {
    StringTokenizer st(atm, ",");
    std::map<int, std::pair<SUMOTime, SUMOTime>> angleTimesMap;
    int tripletCount = 0;
    while (st.hasNext()) {
        StringTokenizer pos(st.next());
        if (pos.size() != 3) {
            WRITE_ERROR("manoeuverAngleTimes format for vType '" + vtype->id + "' " + atm + " contains an invalid triplet.");
            return false;
        } else {
            try {
                const int angle = StringUtils::toInt(pos.next());
                const SUMOTime t1 = static_cast<SUMOTime>(StringUtils::toDouble(pos.next()));
                const SUMOTime steps1 = TIME2STEPS(t1);
                const SUMOTime t2 = static_cast<SUMOTime>(StringUtils::toDouble(pos.next()));
                const SUMOTime steps2 = TIME2STEPS(t2);
                angleTimesMap.insert((std::pair<int, std::pair<SUMOTime, SUMOTime>>(angle, std::pair< SUMOTime, SUMOTime>(steps1, steps2))));
            } catch (...) {
                WRITE_ERROR("Triplet '" + st.get(tripletCount) + "' for vType '" + vtype->id + "' manoeuverAngleTimes cannot be parsed as 'int double double'");
                return false;
            }
            tripletCount++;
        }
    }
    if (angleTimesMap.size() > 0) {
        vtype->myManoeuverAngleTimes.clear();
        for (const auto& angleTime : angleTimesMap) {
            vtype->myManoeuverAngleTimes.insert(angleTime);
        }
        angleTimesMap.clear();
        return true;
    } else {
        return false;
    }
}


bool
SUMOVehicleParserHelper::parseCFMParams(SUMOVTypeParameter* into, const SumoXMLTag element, const SUMOSAXAttributes& attrs, const bool nestedCFM) {
    const CFAttrMap& allowedCFM = getAllowedCFModelAttrs();
    CFAttrMap::const_iterator cf_it = allowedCFM.find(element);
    // check if given CFM is allowed
    if (cf_it == allowedCFM.end()) {
        if (SUMOXMLDefinitions::Tags.has((int)element)) {
            WRITE_ERROR("Unknown car following model " + toString(element) + " when parsing vType '" + into->id + "'");
        } else {
            WRITE_ERROR("Unknown car following model when parsing vType '" + into->id + "'");
        }
        return false;
    }
    // check if we're parsing a nested CFM
    if (nestedCFM) {
        into->cfModel = cf_it->first;
        into->parametersSet |= VTYPEPARS_CAR_FOLLOW_MODEL;
    }
    // set CFM values
    for (const auto& it : cf_it->second) {
        if (attrs.hasAttribute(it)) {
            // first obtain  CFM attribute in string format
            bool ok = true;
            std::string parsedCFMAttribute = attrs.get<std::string>(it, into->id.c_str(), ok);
            // check CFM Attribute
            if (!ok) {
                return false;
            } else if (it == SUMO_ATTR_TRAIN_TYPE) {
                // check if train value is valid
                if (!SUMOXMLDefinitions::TrainTypes.hasString(parsedCFMAttribute)) {
                    WRITE_ERROR("Invalid train type '" + parsedCFMAttribute + "' used in Car-Following-Attribute " + toString(it));
                    return false;
                }
                // add parsedCFMAttribute to cfParameter
                into->cfParameter[it] = parsedCFMAttribute;
            } else if (it == SUMO_ATTR_CF_IDM_STEPPING) {
                // declare a int in wich save CFM int attribute
                double CFMDoubleAttribute = -1;
                try {
                    // obtain CFM attribute in int format
                    CFMDoubleAttribute = StringUtils::toDouble(parsedCFMAttribute);
                } catch (...) {
                    WRITE_ERROR("Invalid Car-Following-Model Attribute " + toString(it) + ". Cannot be parsed to float");
                    return false;
                }
                if (CFMDoubleAttribute <= 0) {
                    WRITE_ERROR("Invalid Car-Following-Model Attribute " + toString(it) + ". Must be greater than 0");
                    return false;
                }
                // add parsedCFMAttribute to cfParameter
                into->cfParameter[it] = parsedCFMAttribute;
            } else {
                // declare a double in wich save CFM float attribute
                double CFMDoubleAttribute = -1;
                try {
                    // obtain CFM attribute in double format
                    CFMDoubleAttribute = StringUtils::toDouble(parsedCFMAttribute);
                } catch (...) {
                    WRITE_ERROR("Invalid Car-Following-Model Attribute " + toString(it) + ". Cannot be parsed to float");
                    return false;
                }
                // check attributes of type "positiveFloatType" (> 0)
                switch (it) {
                    case SUMO_ATTR_ACCEL:
                    case SUMO_ATTR_DECEL:
                    case SUMO_ATTR_APPARENTDECEL:
                    case SUMO_ATTR_EMERGENCYDECEL:
                    case SUMO_ATTR_TAU:
                        if (CFMDoubleAttribute <= 0) {
                            WRITE_ERROR("Invalid Car-Following-Model Attribute " + toString(it) + ". Must be greater than 0");
                            return false;
                        }
                        break;
                    default:
                        break;
                }
                // check attributes restricted to [0-1]
                switch (it) {
                    case SUMO_ATTR_SIGMA:
                        if ((CFMDoubleAttribute < 0) || (CFMDoubleAttribute > 1)) {
                            WRITE_ERROR("Invalid Car-Following-Model Attribute " + toString(it) + ". Only values between [0-1] are allowed");
                            return false;
                        }
                        break;
                    default:
                        break;
                }
                // add parsedCFMAttribute to cfParameter
                into->cfParameter[it] = parsedCFMAttribute;
            }
        }
    }
    // all CFM sucesfully parsed, then return true
    return true;
}


const SUMOVehicleParserHelper::CFAttrMap&
SUMOVehicleParserHelper::getAllowedCFModelAttrs() {
    // init on first use
    if (allowedCFModelAttrs.size() == 0) {
        // Krauss
        std::set<SumoXMLAttr> kraussParams;
        kraussParams.insert(SUMO_ATTR_ACCEL);
        kraussParams.insert(SUMO_ATTR_DECEL);
        kraussParams.insert(SUMO_ATTR_APPARENTDECEL);
        kraussParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        kraussParams.insert(SUMO_ATTR_SIGMA);
        kraussParams.insert(SUMO_ATTR_TAU);
        allowedCFModelAttrs[SUMO_TAG_CF_KRAUSS] = kraussParams;
        allowedCFModelAttrs[SUMO_TAG_CF_KRAUSS_ORIG1] = kraussParams;
        allowedCFModelAttrs[SUMO_TAG_CF_KRAUSS_PLUS_SLOPE] = kraussParams;
        std::set<SumoXMLAttr> allParams(kraussParams);
        // KraussX
        std::set<SumoXMLAttr> kraussXParams(kraussParams);
        kraussXParams.insert(SUMO_ATTR_TMP1);
        kraussXParams.insert(SUMO_ATTR_TMP2);
        kraussXParams.insert(SUMO_ATTR_TMP3);
        kraussXParams.insert(SUMO_ATTR_TMP4);
        kraussXParams.insert(SUMO_ATTR_TMP5);
        allowedCFModelAttrs[SUMO_TAG_CF_KRAUSSX] = kraussXParams;
        allParams.insert(kraussXParams.begin(), kraussXParams.end());
        // SmartSK
        std::set<SumoXMLAttr> smartSKParams;
        smartSKParams.insert(SUMO_ATTR_ACCEL);
        smartSKParams.insert(SUMO_ATTR_DECEL);
        smartSKParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        smartSKParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        smartSKParams.insert(SUMO_ATTR_SIGMA);
        smartSKParams.insert(SUMO_ATTR_TAU);
        smartSKParams.insert(SUMO_ATTR_TMP1);
        smartSKParams.insert(SUMO_ATTR_TMP2);
        smartSKParams.insert(SUMO_ATTR_TMP3);
        smartSKParams.insert(SUMO_ATTR_TMP4);
        smartSKParams.insert(SUMO_ATTR_TMP5);
        allowedCFModelAttrs[SUMO_TAG_CF_SMART_SK] = smartSKParams;
        allParams.insert(smartSKParams.begin(), smartSKParams.end());
        // Daniel
        std::set<SumoXMLAttr> daniel1Params;
        daniel1Params.insert(SUMO_ATTR_ACCEL);
        daniel1Params.insert(SUMO_ATTR_DECEL);
        daniel1Params.insert(SUMO_ATTR_EMERGENCYDECEL);
        daniel1Params.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        daniel1Params.insert(SUMO_ATTR_SIGMA);
        daniel1Params.insert(SUMO_ATTR_TAU);
        daniel1Params.insert(SUMO_ATTR_TMP1);
        daniel1Params.insert(SUMO_ATTR_TMP2);
        daniel1Params.insert(SUMO_ATTR_TMP3);
        daniel1Params.insert(SUMO_ATTR_TMP4);
        daniel1Params.insert(SUMO_ATTR_TMP5);
        allowedCFModelAttrs[SUMO_TAG_CF_DANIEL1] = daniel1Params;
        allParams.insert(daniel1Params.begin(), daniel1Params.end());
        // Peter Wagner
        std::set<SumoXMLAttr> pwagParams;
        pwagParams.insert(SUMO_ATTR_ACCEL);
        pwagParams.insert(SUMO_ATTR_DECEL);
        pwagParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        pwagParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        pwagParams.insert(SUMO_ATTR_SIGMA);
        pwagParams.insert(SUMO_ATTR_TAU);
        pwagParams.insert(SUMO_ATTR_CF_PWAGNER2009_TAULAST);
        pwagParams.insert(SUMO_ATTR_CF_PWAGNER2009_APPROB);
        allowedCFModelAttrs[SUMO_TAG_CF_PWAGNER2009] = pwagParams;
        allParams.insert(pwagParams.begin(), pwagParams.end());
        // IDM params
        std::set<SumoXMLAttr> idmParams;
        idmParams.insert(SUMO_ATTR_ACCEL);
        idmParams.insert(SUMO_ATTR_DECEL);
        idmParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        idmParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        idmParams.insert(SUMO_ATTR_TAU);
        idmParams.insert(SUMO_ATTR_CF_IDM_DELTA);
        idmParams.insert(SUMO_ATTR_CF_IDM_STEPPING);
        allowedCFModelAttrs[SUMO_TAG_CF_IDM] = idmParams;
        allParams.insert(idmParams.begin(), idmParams.end());
        // EIDM
        std::set<SumoXMLAttr> eidmParams;
        eidmParams.insert(SUMO_ATTR_ACCEL);
        eidmParams.insert(SUMO_ATTR_DECEL);
        eidmParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        eidmParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        eidmParams.insert(SUMO_ATTR_TAU);
        eidmParams.insert(SUMO_ATTR_CF_IDM_DELTA);
        eidmParams.insert(SUMO_ATTR_CF_IDM_STEPPING);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_T_LOOK_AHEAD);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_T_PERSISTENCE_DRIVE);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_T_REACTION);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_T_PERSISTENCE_ESTIMATE);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_C_COOLNESS);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_SIG_LEADER);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_SIG_GAP);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_SIG_ERROR);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_JERK_MAX);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_EPSILON_ACC);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_T_ACC_MAX);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_M_FLATNESS);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_M_BEGIN);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_USEVEHDYNAMICS);
        eidmParams.insert(SUMO_ATTR_CF_EIDM_MAX_VEH_PREVIEW);
        allowedCFModelAttrs[SUMO_TAG_CF_EIDM] = eidmParams;
        allParams.insert(eidmParams.begin(), eidmParams.end());
        // IDMM
        std::set<SumoXMLAttr> idmmParams;
        idmmParams.insert(SUMO_ATTR_ACCEL);
        idmmParams.insert(SUMO_ATTR_DECEL);
        idmmParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        idmmParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        idmmParams.insert(SUMO_ATTR_TAU);
        idmmParams.insert(SUMO_ATTR_CF_IDMM_ADAPT_FACTOR);
        idmmParams.insert(SUMO_ATTR_CF_IDMM_ADAPT_TIME);
        idmmParams.insert(SUMO_ATTR_CF_IDM_STEPPING);
        allowedCFModelAttrs[SUMO_TAG_CF_IDMM] = idmmParams;
        allParams.insert(idmmParams.begin(), idmmParams.end());
        // Bieker
        std::set<SumoXMLAttr> bkernerParams;
        bkernerParams.insert(SUMO_ATTR_ACCEL);
        bkernerParams.insert(SUMO_ATTR_DECEL);
        bkernerParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        bkernerParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        bkernerParams.insert(SUMO_ATTR_TAU);
        bkernerParams.insert(SUMO_ATTR_K);
        bkernerParams.insert(SUMO_ATTR_CF_KERNER_PHI);
        allowedCFModelAttrs[SUMO_TAG_CF_BKERNER] = bkernerParams;
        allParams.insert(bkernerParams.begin(), bkernerParams.end());
        // Wiedemann
        std::set<SumoXMLAttr> wiedemannParams;
        wiedemannParams.insert(SUMO_ATTR_ACCEL);
        wiedemannParams.insert(SUMO_ATTR_DECEL);
        wiedemannParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        wiedemannParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        wiedemannParams.insert(SUMO_ATTR_CF_WIEDEMANN_SECURITY);
        wiedemannParams.insert(SUMO_ATTR_CF_WIEDEMANN_ESTIMATION);
        allowedCFModelAttrs[SUMO_TAG_CF_WIEDEMANN] = wiedemannParams;
        allParams.insert(wiedemannParams.begin(), wiedemannParams.end());
        // W99
        std::set<SumoXMLAttr> w99Params;
        w99Params.insert(SUMO_ATTR_DECEL); // used when patching speed during lane-changing
        w99Params.insert(SUMO_ATTR_EMERGENCYDECEL);
        w99Params.insert(SUMO_ATTR_CF_W99_CC1);
        w99Params.insert(SUMO_ATTR_CF_W99_CC2);
        w99Params.insert(SUMO_ATTR_CF_W99_CC3);
        w99Params.insert(SUMO_ATTR_CF_W99_CC4);
        w99Params.insert(SUMO_ATTR_CF_W99_CC5);
        w99Params.insert(SUMO_ATTR_CF_W99_CC6);
        w99Params.insert(SUMO_ATTR_CF_W99_CC7);
        w99Params.insert(SUMO_ATTR_CF_W99_CC8);
        w99Params.insert(SUMO_ATTR_CF_W99_CC9);
        allowedCFModelAttrs[SUMO_TAG_CF_W99] = w99Params;
        allParams.insert(w99Params.begin(), w99Params.end());
        // Rail
        std::set<SumoXMLAttr> railParams;
        railParams.insert(SUMO_ATTR_TRAIN_TYPE);
        railParams.insert(SUMO_ATTR_ACCEL);
        railParams.insert(SUMO_ATTR_DECEL);
        railParams.insert(SUMO_ATTR_APPARENTDECEL);
        railParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        railParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        allowedCFModelAttrs[SUMO_TAG_CF_RAIL] = railParams;
        allParams.insert(railParams.begin(), railParams.end());
        // ACC
        std::set<SumoXMLAttr> ACCParams;
        ACCParams.insert(SUMO_ATTR_ACCEL);
        ACCParams.insert(SUMO_ATTR_DECEL);
        ACCParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        ACCParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        ACCParams.insert(SUMO_ATTR_TAU);
        ACCParams.insert(SUMO_ATTR_SC_GAIN);
        ACCParams.insert(SUMO_ATTR_GCC_GAIN_SPEED);
        ACCParams.insert(SUMO_ATTR_GCC_GAIN_SPACE);
        ACCParams.insert(SUMO_ATTR_GC_GAIN_SPEED);
        ACCParams.insert(SUMO_ATTR_GC_GAIN_SPACE);
        ACCParams.insert(SUMO_ATTR_CA_GAIN_SPEED);
        ACCParams.insert(SUMO_ATTR_CA_GAIN_SPACE);
        ACCParams.insert(SUMO_ATTR_APPLYDRIVERSTATE);
        allowedCFModelAttrs[SUMO_TAG_CF_ACC] = ACCParams;
        allParams.insert(ACCParams.begin(), ACCParams.end());
        // CACC
        std::set<SumoXMLAttr> CACCParams;
        CACCParams.insert(SUMO_ATTR_ACCEL);
        CACCParams.insert(SUMO_ATTR_DECEL);
        CACCParams.insert(SUMO_ATTR_EMERGENCYDECEL);
        CACCParams.insert(SUMO_ATTR_COLLISION_MINGAP_FACTOR);
        CACCParams.insert(SUMO_ATTR_TAU);
        CACCParams.insert(SUMO_ATTR_SC_GAIN_CACC);
        CACCParams.insert(SUMO_ATTR_GCC_GAIN_GAP_CACC);
        CACCParams.insert(SUMO_ATTR_GCC_GAIN_GAP_DOT_CACC);
        CACCParams.insert(SUMO_ATTR_GC_GAIN_GAP_CACC);
        CACCParams.insert(SUMO_ATTR_GC_GAIN_GAP_DOT_CACC);
        CACCParams.insert(SUMO_ATTR_CA_GAIN_GAP_CACC);
        CACCParams.insert(SUMO_ATTR_CA_GAIN_GAP_DOT_CACC);
        CACCParams.insert(SUMO_ATTR_GCC_GAIN_SPEED);
        CACCParams.insert(SUMO_ATTR_GCC_GAIN_SPACE);
        CACCParams.insert(SUMO_ATTR_GC_GAIN_SPEED);
        CACCParams.insert(SUMO_ATTR_GC_GAIN_SPACE);
        CACCParams.insert(SUMO_ATTR_CA_GAIN_SPEED);
        CACCParams.insert(SUMO_ATTR_CA_GAIN_SPACE);
        CACCParams.insert(SUMO_ATTR_HEADWAY_TIME_CACC_TO_ACC);
        CACCParams.insert(SUMO_ATTR_APPLYDRIVERSTATE);
        allowedCFModelAttrs[SUMO_TAG_CF_CACC] = CACCParams;
        allParams.insert(CACCParams.begin(), CACCParams.end());
        // CC
        std::set<SumoXMLAttr> ccParams;
        ccParams.insert(SUMO_ATTR_ACCEL);
        ccParams.insert(SUMO_ATTR_DECEL);
        ccParams.insert(SUMO_ATTR_TAU);
        ccParams.insert(SUMO_ATTR_CF_CC_C1);
        ccParams.insert(SUMO_ATTR_CF_CC_CCDECEL);
        ccParams.insert(SUMO_ATTR_CF_CC_CONSTSPACING);
        ccParams.insert(SUMO_ATTR_CF_CC_KP);
        ccParams.insert(SUMO_ATTR_CF_CC_LAMBDA);
        ccParams.insert(SUMO_ATTR_CF_CC_OMEGAN);
        ccParams.insert(SUMO_ATTR_CF_CC_TAU);
        ccParams.insert(SUMO_ATTR_CF_CC_XI);
        ccParams.insert(SUMO_ATTR_CF_CC_LANES_COUNT);
        ccParams.insert(SUMO_ATTR_CF_CC_CCACCEL);
        ccParams.insert(SUMO_ATTR_CF_CC_PLOEG_KP);
        ccParams.insert(SUMO_ATTR_CF_CC_PLOEG_KD);
        ccParams.insert(SUMO_ATTR_CF_CC_PLOEG_H);
        ccParams.insert(SUMO_ATTR_CF_CC_FLATBED_KA);
        ccParams.insert(SUMO_ATTR_CF_CC_FLATBED_KV);
        ccParams.insert(SUMO_ATTR_CF_CC_FLATBED_KP);
        ccParams.insert(SUMO_ATTR_CF_CC_FLATBED_D);
        ccParams.insert(SUMO_ATTR_CF_CC_FLATBED_H);
        allowedCFModelAttrs[SUMO_TAG_CF_CC] = ccParams;
        allParams.insert(ccParams.begin(), ccParams.end());
        // last element
        allowedCFModelAttrs[SUMO_TAG_NOTHING] = allParams;
    }
    return allowedCFModelAttrs;
}


bool
SUMOVehicleParserHelper::parseLCParams(SUMOVTypeParameter* into, LaneChangeModel model, const SUMOSAXAttributes& attrs) {
    if (allowedLCModelAttrs.size() == 0) {
        // lc2013
        std::set<SumoXMLAttr> lc2013Params;
        lc2013Params.insert(SUMO_ATTR_LCA_STRATEGIC_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_COOPERATIVE_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_SPEEDGAIN_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_KEEPRIGHT_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_OPPOSITE_PARAM);
        lc2013Params.insert(SUMO_ATTR_LCA_LOOKAHEADLEFT);
        lc2013Params.insert(SUMO_ATTR_LCA_SPEEDGAINRIGHT);
        lc2013Params.insert(SUMO_ATTR_LCA_MAXSPEEDLATSTANDING);
        lc2013Params.insert(SUMO_ATTR_LCA_MAXSPEEDLATFACTOR);
        lc2013Params.insert(SUMO_ATTR_LCA_MAXDISTLATSTANDING);
        lc2013Params.insert(SUMO_ATTR_LCA_ASSERTIVE);
        lc2013Params.insert(SUMO_ATTR_LCA_SPEEDGAIN_LOOKAHEAD);
        lc2013Params.insert(SUMO_ATTR_LCA_COOPERATIVE_ROUNDABOUT);
        lc2013Params.insert(SUMO_ATTR_LCA_COOPERATIVE_SPEED);
        lc2013Params.insert(SUMO_ATTR_LCA_OVERTAKE_RIGHT);
        lc2013Params.insert(SUMO_ATTR_LCA_SIGMA);
        lc2013Params.insert(SUMO_ATTR_LCA_KEEPRIGHT_ACCEPTANCE_TIME);
        lc2013Params.insert(SUMO_ATTR_LCA_EXPERIMENTAL1);
        allowedLCModelAttrs[LCM_LC2013] = lc2013Params;
        // sl2015 (extension of lc2013)
        std::set<SumoXMLAttr> sl2015Params = lc2013Params;
        sl2015Params.insert(SUMO_ATTR_LCA_PUSHY);
        sl2015Params.insert(SUMO_ATTR_LCA_PUSHYGAP);
        sl2015Params.insert(SUMO_ATTR_LCA_SUBLANE_PARAM);
        sl2015Params.insert(SUMO_ATTR_LCA_IMPATIENCE);
        sl2015Params.insert(SUMO_ATTR_LCA_TIME_TO_IMPATIENCE);
        sl2015Params.insert(SUMO_ATTR_LCA_ACCEL_LAT);
        sl2015Params.insert(SUMO_ATTR_LCA_TURN_ALIGNMENT_DISTANCE);
        sl2015Params.insert(SUMO_ATTR_LCA_LANE_DISCIPLINE);
        allowedLCModelAttrs[LCM_SL2015] = sl2015Params;
        // DK2008
        std::set<SumoXMLAttr> noParams;
        allowedLCModelAttrs[LCM_DK2008] = noParams;
        // default model may be either LC2013 or SL2015
        // we allow both sets (sl2015 is a superset of lc2013Params)
        allowedLCModelAttrs[LCM_DEFAULT] = sl2015Params;
    }
    std::set<SumoXMLAttr> allowed = allowedLCModelAttrs[model];
    // iterate over LCM attributes
    for (const auto& it : allowed) {
        if (attrs.hasAttribute(it)) {
            // first obtain  CFM attribute in string format
            bool ok = true;
            std::string parsedLCMAttribute = attrs.get<std::string>(it, into->id.c_str(), ok);
            if (!ok) {
                return false;
            }
            // declare a double in wich save CFM attribute
            double LCMAttribute = -1;
            try {
                // obtain CFM attribute in double format
                LCMAttribute = StringUtils::toDouble(parsedLCMAttribute);
            } catch (...) {
                WRITE_ERROR("Invalid Lane-Change-Model Attribute " + toString(it) + ". Cannot be parsed to float");
                return false;
            }
            // check attributes of type "nonNegativeFloatType" (>= 0)
            switch (it) {
                case SUMO_ATTR_LCA_PUSHYGAP:
                case SUMO_ATTR_LCA_MAXSPEEDLATSTANDING:
                case SUMO_ATTR_LCA_IMPATIENCE:
                case SUMO_ATTR_LCA_OVERTAKE_RIGHT:
                case SUMO_ATTR_LCA_ASSERTIVE:
                case SUMO_ATTR_LCA_LOOKAHEADLEFT:
                case SUMO_ATTR_LCA_SPEEDGAINRIGHT:
                case SUMO_ATTR_LCA_TURN_ALIGNMENT_DISTANCE:
                case SUMO_ATTR_LCA_TIME_TO_IMPATIENCE:
                case SUMO_ATTR_LCA_LANE_DISCIPLINE:
                case SUMO_ATTR_LCA_SIGMA:
                    if (LCMAttribute < 0) {
                        WRITE_ERROR("Invalid Lane-Change-Model Attribute " + toString(it) + ". Must be equal or greater than 0");
                        return false;
                    }
                    break;
                default:
                    break;
            }
            // check attributes of type "positiveFloatType" (> 0)
            switch (it) {
                case SUMO_ATTR_LCA_ACCEL_LAT:
                    if (LCMAttribute <= 0) {
                        WRITE_ERROR("Invalid Lane-Change-Model Attribute " + toString(it) + ". Must be greater than 0");
                        return false;
                    }
                    break;
                default:
                    break;
            }
            // add parsedLCMAttribute to cfParameter
            into->lcParameter[it] = parsedLCMAttribute;
        }
    }
    // all LCM parsed ok, then return true
    return true;
}


bool
SUMOVehicleParserHelper::parseJMParams(SUMOVTypeParameter* into, const SUMOSAXAttributes& attrs) {
    if (allowedJMAttrs.size() == 0) {
        // init static set (there is only one model)
        allowedJMAttrs.insert(SUMO_ATTR_JM_CROSSING_GAP);
        allowedJMAttrs.insert(SUMO_ATTR_JM_DRIVE_AFTER_YELLOW_TIME);
        allowedJMAttrs.insert(SUMO_ATTR_JM_DRIVE_AFTER_RED_TIME);
        allowedJMAttrs.insert(SUMO_ATTR_JM_DRIVE_RED_SPEED);
        allowedJMAttrs.insert(SUMO_ATTR_JM_IGNORE_KEEPCLEAR_TIME);
        allowedJMAttrs.insert(SUMO_ATTR_JM_IGNORE_FOE_SPEED);
        allowedJMAttrs.insert(SUMO_ATTR_JM_IGNORE_FOE_PROB);
        allowedJMAttrs.insert(SUMO_ATTR_JM_IGNORE_JUNCTION_FOE_PROB);
        allowedJMAttrs.insert(SUMO_ATTR_JM_SIGMA_MINOR);
        allowedJMAttrs.insert(SUMO_ATTR_JM_STOPLINE_GAP);
        allowedJMAttrs.insert(SUMO_ATTR_JM_TIMEGAP_MINOR);
    }
    for (const auto& it : allowedJMAttrs) {
        if (attrs.hasAttribute(it)) {
            // first obtain  CFM attribute in string format
            bool ok = true;
            std::string parsedJMAttribute = attrs.get<std::string>(it, into->id.c_str(), ok);
            if (!ok) {
                return false;
            }
            // declare a double in wich save CFM attribute
            double JMAttribute = -1;
            try {
                // obtain CFM attribute in double format
                JMAttribute = StringUtils::toDouble(parsedJMAttribute);
            } catch (...) {
                WRITE_ERROR("Invalid Junction-Model Attribute " + toString(it) + ". Cannot be parsed to float");
                return false;
            }
            // now continue checking other properties (-1 is the default value)
            if (JMAttribute != -1) {
                // special case for sigma minor
                if (it == SUMO_ATTR_JM_SIGMA_MINOR) {
                    // check attributes sigma minor
                    if ((JMAttribute < 0) || (JMAttribute > 1)) {
                        WRITE_ERROR("Invalid Junction-Model Attribute " + toString(it) + ". Only values between [0-1] are allowed");
                        return false;
                    }
                } else {
                    // check attributes of type "nonNegativeFloatType" (>= 0)
                    if (JMAttribute < 0) {
                        WRITE_ERROR("Invalid Junction-Model Attribute " + toString(it) + ". Must be equal or greater than 0");
                        return false;
                    }
                }
                // add parsedJMAttribute to cfParameter
                into->jmParameter[it] = parsedJMAttribute;
            }
        }
    }
    // all JM parameters sucesfully parsed, then return true
    return true;
}


SUMOVehicleClass
SUMOVehicleParserHelper::parseVehicleClass(const SUMOSAXAttributes& attrs, const std::string& id) {
    SUMOVehicleClass vclass = SVC_IGNORING;
    bool ok = true;
    std::string vclassS = attrs.getOpt<std::string>(SUMO_ATTR_VCLASS, id.c_str(), ok, "");
    if (vclassS == "") {
        return vclass;
    }
    try {
        const SUMOVehicleClass result = getVehicleClassID(vclassS);
        const std::string& realName = SumoVehicleClassStrings.getString(result);
        if (realName != vclassS) {
            WRITE_WARNING("The vehicle class '" + vclassS + "' for " + attrs.getObjectType() + " '" + id + "' is deprecated, use '" + realName + "' instead.");
        }
        return result;
    } catch (...) {
        WRITE_ERROR("The vehicle class '" + vclassS + "' for " + attrs.getObjectType() + " '" + id + "' is not known.");
    }
    return vclass;
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


double
SUMOVehicleParserHelper::parseWalkPos(SumoXMLAttr attr, const bool hardFail, const std::string& id, double maxPos, const std::string& val, SumoRNG* rng) {
    double result;
    std::string error;
    ArrivalPosDefinition proc = ArrivalPosDefinition::DEFAULT;
    // only supports 'random' and 'max'
    if (!SUMOVehicleParameter::parseArrivalPos(val, toString(SUMO_TAG_WALK), id, result, proc, error)) {
        handleVehicleError(hardFail, nullptr, error);
    }
    if (proc == ArrivalPosDefinition::RANDOM) {
        result = RandHelper::rand(maxPos, rng);
    } else if (proc == ArrivalPosDefinition::CENTER) {
        result = maxPos / 2.;
    } else if (proc == ArrivalPosDefinition::MAX) {
        result = maxPos;
    }
    return SUMOVehicleParameter::interpretEdgePos(result, maxPos, attr, id);
}


SUMOTime
SUMOVehicleParserHelper::processActionStepLength(double given) {
    const std::string defaultError = "The parameter action-step-length must be a non-negative multiple of the simulation step-length. ";
    SUMOTime result = TIME2STEPS(given);
    if (result <= 0) {
        if (result < 0) {
            WRITE_WARNING(defaultError + "Ignoring given value (=" + toString(STEPS2TIME(result)) + " s.)");
        }
        result = DELTA_T;
    } else if (result % DELTA_T != 0) {
        result = (SUMOTime)(DELTA_T * floor(double(result) / double(DELTA_T)));
        result = MAX2(DELTA_T, result);
        if (fabs(given * 1000. - double(result)) > NUMERICAL_EPS) {
            WRITE_WARNING(defaultError + "Parsing given value (" + toString(given) + " s.) to the adjusted value " + toString(STEPS2TIME(result)) + " s.");
        }
    }
    return result;
}


SUMOVehicleParameter*
SUMOVehicleParserHelper::handleVehicleError(const bool hardFail, SUMOVehicleParameter* vehicleParameter, const std::string message) {
    if (vehicleParameter) {
        delete vehicleParameter;
    }
    if (hardFail) {
        throw ProcessError(message);
    } else if (message.size() > 0) {
        WRITE_ERROR(message);
    }
    return nullptr;
}


SUMOVTypeParameter*
SUMOVehicleParserHelper::handleVehicleTypeError(const bool hardFail, SUMOVTypeParameter* vehicleTypeParameter, const std::string message) {
    if (vehicleTypeParameter) {
        delete vehicleTypeParameter;
    }
    if (hardFail) {
        throw ProcessError(message);
    } else if (message.size() > 0) {
        WRITE_ERROR(message);
    }
    return nullptr;
}

/****************************************************************************/
