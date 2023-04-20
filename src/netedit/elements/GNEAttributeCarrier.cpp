/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2023 German Aerospace Center (DLR) and others.
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
/// @file    GNEAttributeCarrier.cpp
/// @author  Jakob Erdmann
/// @date    Feb 2011
///
// Abstract Base class for gui objects which carry attributes
/****************************************************************************/
#include <netedit/GNENet.h>
#include <netedit/GNEViewNet.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>

#include "GNEAttributeCarrier.h"


// ===========================================================================
// static members
// ===========================================================================

std::map<SumoXMLTag, GNETagProperties> GNEAttributeCarrier::myTagProperties;
const std::string GNEAttributeCarrier::FEATURE_LOADED = "loaded";
const std::string GNEAttributeCarrier::FEATURE_GUESSED = "guessed";
const std::string GNEAttributeCarrier::FEATURE_MODIFIED = "modified";
const std::string GNEAttributeCarrier::FEATURE_APPROVED = "approved";
const size_t GNEAttributeCarrier::MAXNUMBEROFATTRIBUTES = 128;
const Parameterised::Map GNEAttributeCarrier::PARAMETERS_EMPTY;
const std::string GNEAttributeCarrier::True = toString(true);
const std::string GNEAttributeCarrier::False = toString(false);


// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributeCarrier::GNEAttributeCarrier(const SumoXMLTag tag, GNENet* net) :
    myTagProperty(getTagProperty(tag)),
    myNet(net),
    mySelected(false),
    myIsTemplate(false) {
}


GNEAttributeCarrier::~GNEAttributeCarrier() {}


const std::string
GNEAttributeCarrier::getID() const {
    return getAttribute(SUMO_ATTR_ID);
}


GNENet*
GNEAttributeCarrier::getNet() const {
    return myNet;
}


void
GNEAttributeCarrier::selectAttributeCarrier(const bool changeFlag) {
    if (getGUIGlObject() && myTagProperty.isSelectable()) {
        gSelected.select(getGUIGlObject()->getGlID());
        if (changeFlag) {
            mySelected = true;
        }
    }
}


void
GNEAttributeCarrier::unselectAttributeCarrier(const bool changeFlag) {
    if (getGUIGlObject() && myTagProperty.isSelectable()) {
        gSelected.deselect(getGUIGlObject()->getGlID());
        if (changeFlag) {
            mySelected = false;
        }
    }
}


bool
GNEAttributeCarrier::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEAttributeCarrier::drawUsingSelectColor() const {
    // get flag for network element
    const bool networkElement = myTagProperty.isNetworkElement() || myTagProperty.isAdditionalElement();
    // check supermode network
    if ((networkElement && myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) ||
            (myTagProperty.isDemandElement() && myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) ||
            (myTagProperty.isGenericData() && myNet->getViewNet()->getEditModes().isCurrentSupermodeData())) {
        return mySelected;
    } else {
        return false;
    }
}


void
GNEAttributeCarrier::resetDefaultValues() {
    for (const auto& attrProperty : myTagProperty) {
        if (attrProperty.hasDefaultValue()) {
            setAttribute(attrProperty.getAttr(), attrProperty.getDefaultValue());
            if (attrProperty.isActivatable()) {
                toggleAttribute(attrProperty.getAttr(), attrProperty.getDefaultActivated());
            }
        }
    }
}


void
GNEAttributeCarrier::enableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    throw ProcessError(TL("Nothing to enable, implement in Children"));

}


void
GNEAttributeCarrier::disableAttribute(SumoXMLAttr /*key*/, GNEUndoList* /*undoList*/) {
    throw ProcessError(TL("Nothing to disable, implement in Children"));
}


bool
GNEAttributeCarrier::isAttributeEnabled(SumoXMLAttr /*key*/) const {
    // by default, all attributes are enabled
    return true;
}


bool
GNEAttributeCarrier::isAttributeComputed(SumoXMLAttr /*key*/) const {
    // by default, all attributes aren't computed
    return false;
}


template<> int
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toInt(string);
}


template<> double
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toDouble(string);
}


template<> SUMOTime
GNEAttributeCarrier::parse(const std::string& string) {
    return string2time(string);
}


template<> bool
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toBool(string);
}


template<> std::string
GNEAttributeCarrier::parse(const std::string& string) {
    return string;
}


template<> SUMOVehicleClass
GNEAttributeCarrier::parse(const std::string& string) {
    if (string.size() == 0) {
        throw EmptyData();
    } else if (!SumoVehicleClassStrings.hasString(string)) {
        return SVC_IGNORING;
    } else {
        return SumoVehicleClassStrings.get(string);
    }
}


template<> RGBColor
GNEAttributeCarrier::parse(const std::string& string) {
    if (string.empty()) {
        return RGBColor::INVISIBLE;
    } else {
        return RGBColor::parseColor(string);
    }
}


template<> Position
GNEAttributeCarrier::parse(const std::string& string) {
    if (string.size() == 0) {
        throw EmptyData();
    } else {
        bool ok = true;
        PositionVector pos = GeomConvHelper::parseShapeReporting(string, "user-supplied position", 0, ok, false, false);
        if (!ok || (pos.size() != 1)) {
            throw NumberFormatException("(Position) " + string);
        } else {
            return pos[0];
        }
    }
}


template<> PositionVector
GNEAttributeCarrier::parse(const std::string& string) {
    PositionVector posVector;
    // empty string are allowed (It means empty position vector)
    if (string.empty()) {
        return posVector;
    } else {
        bool ok = true;
        posVector = GeomConvHelper::parseShapeReporting(string, "user-supplied shape", 0, ok, false, true);
        if (!ok) {
            throw NumberFormatException("(Position List) " + string);
        } else {
            return posVector;
        }
    }
}


template<> SUMOVehicleShape
GNEAttributeCarrier::parse(const std::string& string) {
    if ((string == "unknown") || (!SumoVehicleShapeStrings.hasString(string))) {
        return SUMOVehicleShape::UNKNOWN;
    } else {
        return SumoVehicleShapeStrings.get(string);
    }
}


template<> std::vector<std::string>
GNEAttributeCarrier::parse(const std::string& string) {
    return StringTokenizer(string).getVector();
}


template<> std::set<std::string>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> vectorString = StringTokenizer(string).getVector();
    std::set<std::string> solution;
    for (const auto& i : vectorString) {
        solution.insert(i);
    }
    return solution;
}


template<> std::vector<int>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<int> parsedIntValues;
    for (const auto& i : parsedValues) {
        parsedIntValues.push_back(parse<int>(i));
    }
    return parsedIntValues;
}


template<> std::vector<double>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<double> parsedDoubleValues;
    for (const auto& i : parsedValues) {
        parsedDoubleValues.push_back(parse<double>(i));
    }
    return parsedDoubleValues;
}


template<> std::vector<bool>
GNEAttributeCarrier::parse(const std::string& string) {
    std::vector<std::string> parsedValues = parse<std::vector<std::string> >(string);
    std::vector<bool> parsedBoolValues;
    for (const auto& i : parsedValues) {
        parsedBoolValues.push_back(parse<bool>(i));
    }
    return parsedBoolValues;
}


template<> std::vector<SumoXMLAttr>
GNEAttributeCarrier::parse(const std::string& value) {
    // Declare string vector
    std::vector<std::string> attributesStr = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    std::vector<SumoXMLAttr> attributes;
    // Iterate over lanes IDs, retrieve Lanes and add it into parsedLanes
    for (const auto& attributeStr : attributesStr) {
        if (SUMOXMLDefinitions::Tags.hasString(attributeStr)) {
            attributes.push_back(static_cast<SumoXMLAttr>(SUMOXMLDefinitions::Attrs.get(attributeStr)));
        } else {
            throw InvalidArgument("Error parsing attributes. Attribute '" + attributeStr + "'  doesn't exist");
        }
    }
    return attributes;
}


template<> std::vector<GNEEdge*>
GNEAttributeCarrier::parse(GNENet* net, const std::string& value) {
    // Declare string vector
    std::vector<std::string> edgeIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    std::vector<GNEEdge*> parsedEdges;
    // Iterate over edges IDs, retrieve Edges and add it into parsedEdges
    for (const auto& edgeID : edgeIds) {
        GNEEdge* retrievedEdge = net->getAttributeCarriers()->retrieveEdge(edgeID, false);
        if (retrievedEdge) {
            parsedEdges.push_back(net->getAttributeCarriers()->retrieveEdge(edgeID));
        } else {
            throw FormatException("Error parsing parameter " + toString(SUMO_ATTR_EDGES) + ". " +
                                  toString(SUMO_TAG_EDGE) + " '" + edgeID + "' doesn't exist");
        }
    }
    return parsedEdges;
}


template<> std::vector<GNELane*>
GNEAttributeCarrier::parse(GNENet* net, const std::string& value) {
    // Declare string vector
    std::vector<std::string> laneIds = GNEAttributeCarrier::parse<std::vector<std::string> > (value);
    std::vector<GNELane*> parsedLanes;
    // Iterate over lanes IDs, retrieve Lanes and add it into parsedLanes
    for (const auto& laneID : laneIds) {
        GNELane* retrievedLane = net->getAttributeCarriers()->retrieveLane(laneID, false);
        if (retrievedLane) {
            parsedLanes.push_back(net->getAttributeCarriers()->retrieveLane(laneID));
        } else {
            throw FormatException("Error parsing parameter " + toString(SUMO_ATTR_LANES) + ". " +
                                  toString(SUMO_TAG_LANE) + " '" + laneID + "'  doesn't exist");
        }
    }
    return parsedLanes;
}


template<> std::string
GNEAttributeCarrier::parseIDs(const std::vector<GNEEdge*>& ACs) {
    // obtain ID's of edges and return their join
    std::vector<std::string> edgeIDs;
    for (const auto& i : ACs) {
        edgeIDs.push_back(i->getID());
    }
    return joinToString(edgeIDs, " ");
}


template<> std::string
GNEAttributeCarrier::parseIDs(const std::vector<GNELane*>& ACs) {
    // obtain ID's of lanes and return their join
    std::vector<std::string> laneIDs;
    for (const auto& i : ACs) {
        laneIDs.push_back(i->getID());
    }
    return joinToString(laneIDs, " ");
}


bool
GNEAttributeCarrier::lanesConsecutives(const std::vector<GNELane*>& lanes) {
    // we need at least two lanes
    if (lanes.size() > 1) {
        // now check that lanes are consecutives (not neccesary connected)
        int currentLane = 0;
        while (currentLane < ((int)lanes.size() - 1)) {
            int nextLane = -1;
            // iterate over outgoing edges of destiny juntion of edge's lane
            for (int i = 0; (i < (int)lanes.at(currentLane)->getParentEdge()->getToJunction()->getGNEOutgoingEdges().size()) && (nextLane == -1); i++) {
                // iterate over lanes of outgoing edges of destiny juntion of edge's lane
                for (int j = 0; (j < (int)lanes.at(currentLane)->getParentEdge()->getToJunction()->getGNEOutgoingEdges().at(i)->getLanes().size()) && (nextLane == -1); j++) {
                    // check if lane correspond to the next lane of "lanes"
                    if (lanes.at(currentLane)->getParentEdge()->getToJunction()->getGNEOutgoingEdges().at(i)->getLanes().at(j) == lanes.at(currentLane + 1)) {
                        nextLane = currentLane;
                    }
                }
            }
            if (nextLane == -1) {
                return false;
            } else {
                currentLane++;
            }
        }
        return true;
    } else {
        return false;
    }
}


template<> std::string
GNEAttributeCarrier::getACParameters() const {
    std::string result;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : getACParametersMap()) {
        result += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!result.empty()) {
        result.pop_back();
    }
    return result;
}


template<> std::vector<std::pair<std::string, std::string> >
GNEAttributeCarrier::getACParameters() const {
    std::vector<std::pair<std::string, std::string> > result;
    // Generate a vector string using the following structure: "<key1,value1>, <key2, value2>,...
    for (const auto& parameter : getACParametersMap()) {
        result.push_back(std::make_pair(parameter.first, parameter.second));
    }
    return result;
}


void
GNEAttributeCarrier::setACParameters(const std::string& parameters, GNEUndoList* undoList) {
    // declare map
    Parameterised::Map parametersMap;
    // separate value in a vector of string using | as separator
    StringTokenizer parametersTokenizer(parameters, "|", true);
    // iterate over all values
    while (parametersTokenizer.hasNext()) {
        // obtain key and value and save it in myParameters
        const std::vector<std::string> keyValue = StringTokenizer(parametersTokenizer.next(), "=", true).getVector();
        if (keyValue.size() == 2) {
            parametersMap[keyValue.front()] = keyValue.back();
        }
    }
    // set setACParameters map
    setACParameters(parametersMap, undoList);
}


void
GNEAttributeCarrier::setACParameters(const std::vector<std::pair<std::string, std::string> >& parameters, GNEUndoList* undoList) {
    // declare parametersMap
    Parameterised::Map parametersMap;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : parameters) {
        parametersMap[parameter.first] = parameter.second;
    }
    // set setACParameters map
    setACParameters(parametersMap, undoList);
}


void
GNEAttributeCarrier::setACParameters(const Parameterised::Map& parameters, GNEUndoList* undoList) {
    // declare result string
    std::string paramsStr;
    // Generate an string using the following structure: "key1=value1|key2=value2|...
    for (const auto& parameter : parameters) {
        paramsStr += parameter.first + "=" + parameter.second + "|";
    }
    // remove the last "|"
    if (!paramsStr.empty()) {
        paramsStr.pop_back();
    }
    // set parameters
    setAttribute(GNE_ATTR_PARAMETERS, paramsStr, undoList);
}


void
GNEAttributeCarrier::addACParameters(const std::string& key, const std::string& attribute, GNEUndoList* undoList) {
    // get parametersMap
    Parameterised::Map parametersMap = getACParametersMap();
    // add (or update) attribute
    parametersMap[key] = attribute;
    // set attribute
    setACParameters(parametersMap, undoList);
}


void
GNEAttributeCarrier::removeACParametersKeys(const std::vector<std::string>& keepKeys, GNEUndoList* undoList) {
    // declare parametersMap
    Parameterised::Map newParametersMap;
    // iterate over parameters map
    for (const auto& parameter : getACParametersMap()) {
        // copy to newParametersMap if key is in keepKeys
        if (std::find(keepKeys.begin(), keepKeys.end(), parameter.first) != keepKeys.end()) {
            newParametersMap.insert(parameter);
        }
    }
    // set newParametersMap map
    setACParameters(newParametersMap, undoList);
}


std::string
GNEAttributeCarrier::getAlternativeValueForDisabledAttributes(SumoXMLAttr key) const {
    switch (key) {
        // Crossings
        case SUMO_ATTR_TLLINKINDEX:
        case SUMO_ATTR_TLLINKINDEX2:
            return "No TLS";
        // connections
        case SUMO_ATTR_DIR: {
            // special case for connection directions
            std::string direction = getAttribute(key);
            if (direction == "s") {
                return "Straight (s)";
            } else if (direction ==  "t") {
                return "Turn (t))";
            } else if (direction ==  "l") {
                return "Left (l)";
            } else if (direction ==  "r") {
                return "Right (r)";
            } else if (direction ==  "L") {
                return "Partially left (L)";
            } else if (direction ==  "R") {
                return "Partially right (R)";
            } else if (direction ==  "invalid") {
                return "No direction (Invalid))";
            } else {
                return "undefined";
            }
        }
        case SUMO_ATTR_STATE: {
            // special case for connection states
            std::string state = getAttribute(key);
            if (state == "-") {
                return "Dead end (-)";
            } else if (state == "=") {
                return "equal (=)";
            } else if (state == "m") {
                return "Minor link (m)";
            } else if (state == "M") {
                return "Major link (M)";
            } else if (state == "O") {
                return "TLS controller off (O)";
            } else if (state == "o") {
                return "TLS yellow flashing (o)";
            } else if (state == "y") {
                return "TLS yellow minor link (y)";
            } else if (state == "Y") {
                return "TLS yellow major link (Y)";
            } else if (state == "r") {
                return "TLS red (r)";
            } else if (state == "g") {
                return "TLS green minor (g)";
            } else if (state == "G") {
                return "TLS green major (G)";
            } else if (state == "Z") {
                return "Zipper (Z)";
            } else {
                return "undefined";
            }
        }
        default:
            return getAttribute(key);
    }
}


std::string
GNEAttributeCarrier::getAttributeForSelection(SumoXMLAttr key) const {
    return getAttribute(key);
}


const std::string&
GNEAttributeCarrier::getTagStr() const {
    return myTagProperty.getTagStr();
}


FXIcon*
GNEAttributeCarrier::getACIcon() const {
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    // special case for vClass icons
    if (myTagProperty.vClassIcon()) {
        return VClassIcons::getVClassIcon(SumoVehicleClassStrings.get(getAttribute(SUMO_ATTR_VCLASS)));
    } else {
        return GUIIconSubSys::getIcon(myTagProperty.getGUIIcon());
    }
}


bool
GNEAttributeCarrier::isTemplate() const {
    return myIsTemplate;
}


const GNETagProperties&
GNEAttributeCarrier::getTagProperty() const {
    return myTagProperty;
}

// ===========================================================================
// static methods
// ===========================================================================

const GNETagProperties&
GNEAttributeCarrier::getTagProperty(SumoXMLTag tag) {
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    // check that tag is defined
    if (myTagProperties.count(tag) == 0) {
        throw ProcessError(TLF("TagProperty for tag '%' not defined", toString(tag)));
    } else {
        return myTagProperties.at(tag);
    }
}


const std::vector<GNETagProperties>
GNEAttributeCarrier::getTagPropertiesByType(const int tagPropertyCategory) {
    std::vector<GNETagProperties> allowedTags;
    // define on first access
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    if (tagPropertyCategory & GNETagProperties::NETWORKELEMENT) {
        // fill networkElements tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isNetworkElement()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::ADDITIONALELEMENT) {
        // fill additional tags (only with pure additionals)
        for (const auto& tagProperty : myTagProperties) {
            // avoid symbols (It will be implemented in #7355)
            if (tagProperty.second.isAdditionalPureElement() && !tagProperty.second.isSymbol()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::SYMBOL) {
        // fill symbol tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isSymbol()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::SHAPE) {
        // fill shape tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isShapeElement()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::TAZELEMENT) {
        // fill taz tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isTAZElement()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::WIRE) {
        // fill wire tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isWireElement()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::DEMANDELEMENT) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isDemandElement()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::ROUTE) {
        // fill route tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isRoute()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::VEHICLE) {
        // fill vehicle tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isVehicle()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::STOP) {
        // fill stop tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isStop()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::PERSON) {
        // fill person tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isPerson()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::PERSONPLAN) {
        // fill person plan tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isPersonPlan()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::PERSONTRIP) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isPersonTrip()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::WALK) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isWalk()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::RIDE) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isRide()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::STOPPERSON) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isStopPerson()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::GENERICDATA) {
        // fill generic data tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isGenericData()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::MEANDATA) {
        // fill generic data tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isMeanData()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::CONTAINER) {
        // fill container tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isContainer()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::CONTAINERPLAN) {
        // fill container plan tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isContainerPlan()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::TRANSPORT) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isTransportPlan()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::TRANSHIP) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isTranshipPlan()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    if (tagPropertyCategory & GNETagProperties::STOPCONTAINER) {
        // fill demand tags
        for (const auto& tagProperty : myTagProperties) {
            if (tagProperty.second.isStopContainer()) {
                allowedTags.push_back(tagProperty.second);
            }
        }
    }
    return allowedTags;
}

// ===========================================================================
// private
// ===========================================================================

void
GNEAttributeCarrier::resetAttributes() {
    for (const auto& attrProperty : myTagProperty) {
        if (attrProperty.hasDefaultValue()) {
            setAttribute(attrProperty.getAttr(), attrProperty.getDefaultValue());
        }
    }
}


void
GNEAttributeCarrier::toggleAttribute(SumoXMLAttr /*key*/, const bool /*value*/) {
    throw ProcessError(TL("Nothing to toggle, implement in Children"));
}


void
GNEAttributeCarrier::fillAttributeCarriers() {
    // fill all groups of ACs
    fillNetworkElements();
    fillAdditionalElements();
    fillShapeElements();
    fillTAZElements();
    fillWireElements();
    // demand
    fillDemandElements();
    fillVehicleElements();
    fillStopElements();
    fillWaypointElements();
    // persons
    fillPersonElements();
    fillPersonPlanTrips();
    fillPersonPlanWalks();
    fillPersonPlanRides();
    fillStopPersonElements();
    // containers
    fillContainerElements();
    fillContainerTransportElements();
    fillContainerTranshipElements();
    fillContainerStopElements();
    //data
    fillDataElements();
    // check integrity of all Tags (function checkTagIntegrity() throws an exception if there is an inconsistency)
    for (const auto& tagProperty : myTagProperties) {
        tagProperty.second.checkTagIntegrity();
    }
}


void
GNEAttributeCarrier::fillNetworkElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // obtain Node Types except SumoXMLNodeType::DEAD_END_DEPRECATED
    const auto& neteditOptions = OptionsCont::getOptions();
    std::vector<std::string> nodeTypes = SUMOXMLDefinitions::NodeTypes.getStrings();
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(SumoXMLNodeType::DEAD_END_DEPRECATED)));
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(SumoXMLNodeType::DEAD_END)));
    nodeTypes.erase(std::find(nodeTypes.begin(), nodeTypes.end(), toString(SumoXMLNodeType::INTERNAL)));
    // obtain TLTypes (note: avoid insert all TLTypes because some of them are experimental and not documented)
    std::vector<std::string> TLTypes;
    TLTypes.push_back(toString(TrafficLightType::STATIC));
    TLTypes.push_back(toString(TrafficLightType::ACTUATED));
    TLTypes.push_back(toString(TrafficLightType::DELAYBASED));
    TLTypes.push_back(toString(TrafficLightType::NEMA));
    // fill networkElement ACs
    SumoXMLTag currentTag = SUMO_TAG_JUNCTION;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      GNETagProperties::RTREE,
                                      GUIIcon::JUNCTION, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the node"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::POSITION | GNEAttributeProperties::UPDATEGEOMETRY, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                                              TL("The x-y-z position of the node on the plane in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("An optional type for the node"));
        attrProperty.setDiscreteValues(nodeTypes, true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("A custom shape for that node"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_RADIUS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Optional turning radius (for all corners) for that node in meters"),
                                              "1.5");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_KEEP_CLEAR,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Whether the junction-blocking-heuristic should be activated at this node"),
                                              "1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_RIGHT_OF_WAY,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("How to compute right of way rules at this node"),
                                              SUMOXMLDefinitions::RightOfWayValues.getString(RightOfWay::DEFAULT));
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::RightOfWayValues.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRINGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Whether this junction is at the fringe of the network"),
                                              SUMOXMLDefinitions::FringeTypeValues.getString(FringeType::DEFAULT));
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::FringeTypeValues.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Optional name of junction"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLTYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("An optional type for the traffic light algorithm"));
        attrProperty.setDiscreteValues(TLTypes, true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLLAYOUT,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("An optional layout for the traffic light plan"));
        attrProperty.setDiscreteValues({toString(TrafficLightLayout::DEFAULT),
                                        toString(TrafficLightLayout::OPPOSITES),
                                        toString(TrafficLightLayout::INCOMING),
                                        toString(TrafficLightLayout::ALTERNATE_ONEWAY)}, true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("An optional id for the traffic light program"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TYPE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::EDGETYPE, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("The id of the edge"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NUMLANES,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The number of lanes of the edge"),
                                              toString(neteditOptions.getInt("default.lanenumber")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The maximum speed allowed on the edge in m/s"),
                                              toString(neteditOptions.getFloat("default.speed")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly allows the given vehicle classes (not given will be not allowed)"),
                                              "all");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly disallows the given vehicle classes (not given will be allowed)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPREADTYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The spreadType defines how to compute the lane geometry from the edge geometry (used for visualization)"),
                                              SUMOXMLDefinitions::LaneSpreadFunctions.getString(LaneSpreadFunction::RIGHT));
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::LaneSpreadFunctions.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PRIORITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The priority of the edge"),
                                              toString(neteditOptions.getInt("default.priority")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Lane width for all lanes of this edge in meters (used for visualization)"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SIDEWALKWIDTH,
                                              GNEAttributeProperties::FLOAT,
                                              TL("The width of the sidewalk that should be added as an additional lane"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_BIKELANEWIDTH,
                                              GNEAttributeProperties::FLOAT,
                                              TL("The width of the bike lane that should be added as an additional lane"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_LANETYPE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::LANETYPE, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The maximum speed allowed on the lane in m/s"),
                                              toString(neteditOptions.getFloat("default.speed")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly allows the given vehicle classes (not given will be not allowed)"),
                                              "all");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly disallows the given vehicle classes (not given will be allowed)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Lane width for all lanes of this lane in meters (used for visualization)"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      GNETagProperties::RTREE,
                                      GUIIcon::EDGE, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Edge ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of a node within the nodes-file the edge shall start at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of a node within the nodes-file the edge shall end at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The maximum speed allowed on the edge in m/s"),
                                              toString(neteditOptions.getFloat("default.speed")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PRIORITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The priority of the edge"),
                                              toString(neteditOptions.getInt("default.priority")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NUMLANES,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The number of lanes of the edge"),
                                              toString(neteditOptions.getInt("default.lanenumber")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The name of a type within the SUMO edge type file"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly allows the given vehicle classes (not given will be not allowed)"),
                                              "all");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly disallows the given vehicle classes (not given will be allowed)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("If the shape is given it should start and end with the positions of the from-node and to-node"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The length of the edge in meter"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPREADTYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The spreadType defines how to compute the lane geometry from the edge geometry (used for visualization)"),
                                              SUMOXMLDefinitions::LaneSpreadFunctions.getString(LaneSpreadFunction::RIGHT));
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::LaneSpreadFunctions.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("street name (does not need to be unique, used for visualization)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Lane width for all lanes of this edge in meters (used for visualization)"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDOFFSET,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Move the stop line back from the intersection by the given amount"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_SHAPE_START,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY, // virtual attribute used to define an endPoint
                                              TL("Custom position in which shape start (by default position of junction from)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_SHAPE_END,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY, // virtual attribute from to define an endPoint
                                              TL("Custom position in which shape end (by default position of junction from)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_BIDIR,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE, // virtual attribute to check of this edge is part of a bidirectional railway (cannot be edited)
                                              TL("Show if edge is bidirectional"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISTANCE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UNIQUE,
                                              TL("Distance"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_STOPOFFSET,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The stop offset as positive value in meters"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_STOPOEXCEPTION,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Specifies, for which vehicle classes the stopOffset does NOT apply."));
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings(), false);
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      0,
                                      GUIIcon::LANE, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Lane ID (Automatic, non editable)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_INDEX,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The enumeration index of the lane (0 is the rightmost lane, <NUMBER_LANES>-1 is the leftmost one)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Speed in meters per second"),
                                              "13.89");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly allows the given vehicle classes (not given will be not allowed)"),
                                              "all");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly disallows the given vehicle classes (not given will be allowed)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Width in meters (used for visualization)"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDOFFSET,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Move the stop line back from the intersection by the given amount"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ACCELERATION,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Enable or disable lane as acceleration lane"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("If the shape is given it overrides the computation based on edge shape"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_OPPOSITE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UNIQUE,
                                              TL("If given, this defines the opposite direction lane"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHANGE_LEFT,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Permit changing left only for to the given vehicle classes"),
                                              "all");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHANGE_RIGHT,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Permit changing right only for to the given vehicle classes"),
                                              "all");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Lane type description (optional)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_STOPOFFSET,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The stop offset as positive value in meters"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_STOPOEXCEPTION,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Specifies, for which vehicle classes the stopOffset does NOT apply."));
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings(), false);
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CROSSING;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      0,
                                      GUIIcon::CROSSING, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Crossing ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The (road) edges which are crossed"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PRIORITY,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Whether the pedestrians have priority over the vehicles (automatically set to true at tls-controlled intersections)"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The width of the crossings"),
                                              toString(OptionsCont::getOptions().getFloat("default.crossing-width")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLLINKINDEX,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("sets the tls-index for this crossing"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLLINKINDEX2,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("sets the opposite-direction tls-index for this crossing"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Overrides default shape of pedestrian crossing"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_WALKINGAREA;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WALKINGAREA, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Walking Area ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The width of the WalkingArea"),
                                              toString(OptionsCont::getOptions().getFloat("default.sidewalk-width")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The length of the WalkingArea in meter"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Overrides default shape of pedestrian sidewalk"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CONNECTION;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::NETWORKELEMENT,
                                      0,
                                      GUIIcon::CONNECTION, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the vehicles leave"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the vehicles may reach when leaving 'from'"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM_LANE,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("the lane index of the incoming lane (numbers starting with 0)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO_LANE,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("the lane index of the outgoing lane (numbers starting with 0)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PASS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("if set, vehicles which pass this (lane-2-lane) connection) will not wait"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_KEEP_CLEAR,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("if set to false, vehicles which pass this (lane-2-lane) connection) will not worry about blocking the intersection"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CONTPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set to a more than 0 value, an internal junction will be built at this position (in m)/n from the start of the internal lane for this connection"),
                                              toString(NBEdge::UNSPECIFIED_CONTPOS));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_UNCONTROLLED,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set to true, This connection will not be TLS-controlled despite its node being controlled"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VISIBILITY_DISTANCE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Vision distance between vehicles"),
                                              toString(NBEdge::UNSPECIFIED_VISIBILITY_DISTANCE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLLINKINDEX,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("sets index of this connection within the controlling traffic light"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLLINKINDEX2,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("sets index for the internal junction of this connection within the controlling traffic light"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly allows the given vehicle classes (not given will be not allowed)"),
                                              "all");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Explicitly disallows the given vehicle classes (not given will be allowed)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("sets custom speed limit for the connection"),
                                              toString(NBEdge::UNSPECIFIED_SPEED));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("sets custom length for the connection"),
                                              toString(NBEdge::UNSPECIFIED_LOADED_LENGTH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CUSTOMSHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("sets custom shape for the connection"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHANGE_LEFT,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Permit changing left only for to the given vehicle classes"),
                                              "all");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHANGE_RIGHT,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("Permit changing right only for to the given vehicle classes"),
                                              "all");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_INDIRECT,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("if set to true, vehicles will make a turn in 2 steps"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("set a custom edge type (for applying vClass-specific speed restrictions)"));
        myTagProperties[currentTag].addAttribute(attrProperty);


        attrProperty = GNEAttributeProperties(SUMO_ATTR_DIR,
                                              GNEAttributeProperties::STRING,
                                              TL("turning direction for this connection (computed)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STATE,
                                              GNEAttributeProperties::STRING,
                                              TL("link state for this connection (computed)"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_INTERNAL_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::INTERNALLANE,
                                      0,
                                      GUIIcon::JUNCTION, currentTag);
        //  internal lanes does't have attributes
    }
}


void
GNEAttributeCarrier::fillAdditionalElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill additional elements
    SumoXMLTag currentTag = SUMO_TAG_BUS_STOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::STOPPINGPLACE,
                                      GNETagProperties::MASKSTARTENDPOS,
                                      GUIIcon::BUSSTOP, currentTag, {}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of bus stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the lane the bus stop shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The begin position on the lane (the lower position on the lane) in meters"));

        myTagProperties[currentTag].addAttribute(attrProperty);
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of busStop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Meant to be the names of the bus lines that stop at this bus stop. This is only used for visualization purposes"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERSON_CAPACITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Larger numbers of persons trying to enter will create an upstream jam on the sidewalk"),
                                              "6");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PARKING_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Optional space definition for vehicles that park at this stop"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The RGBA color with which the busStop shall be displayed"));
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_TRAIN_STOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::STOPPINGPLACE,
                                      GNETagProperties::MASKSTARTENDPOS,
                                      GUIIcon::TRAINSTOP, currentTag, {}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of train stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the lane the train stop shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The begin position on the lane (the lower position on the lane) in meters"));

        myTagProperties[currentTag].addAttribute(attrProperty);
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of trainStop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Meant to be the names of the train lines that stop at this train stop. This is only used for visualization purposes"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERSON_CAPACITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Larger numbers of persons trying to enter will create an upstream jam on the sidewalk"),
                                              "6");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PARKING_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Optional space definition for vehicles that park at this stop"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The RGBA color with which the trainStop shall be displayed"));
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_ACCESS;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::REPARENT,
                                      GUIIcon::ACCESS, currentTag, {SUMO_TAG_BUS_STOP, SUMO_TAG_TRAIN_STOP}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the lane the stop access shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the lane (the lower position on the lane) in meters"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The walking length of the access in meters"),
                                              "-1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_CONTAINER_STOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::STOPPINGPLACE,
                                      GNETagProperties::MASKSTARTENDPOS,
                                      GUIIcon::CONTAINERSTOP, currentTag, {}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of container stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the lane the container stop shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The begin position on the lane (the lower position on the lane) in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of containerStop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("meant to be the names of the bus lines that stop at this container stop. This is only used for visualization purposes"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CONTAINER_CAPACITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Larger numbers of container trying to enter will create an upstream jam on the sidewalk"),
                                              "6");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PARKING_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Optional space definition for vehicles that park at this stop"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The RGBA color with which the containerStop shall be displayed"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CHARGING_STATION;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::STOPPINGPLACE,
                                      GNETagProperties::MASKSTARTENDPOS,
                                      GUIIcon::CHARGINGSTATION, currentTag, {}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of charging station"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Lane of the charging station location"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Begin position in the specified lane"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("End position in the specified lane"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of chargingStation"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHARGINGPOWER,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Charging power in W"),
                                              "22000.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_EFFICIENCY,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::RANGE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Charging efficiency [0,1]"),
                                              "0.95");
        attrProperty.setRange(0, 1);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHARGEINTRANSIT,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Enable or disable charge in transit, i.e. vehicle must or must not to stop for charging"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHARGEDELAY,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Time delay after the vehicles has reached / stopped on the charging station, before the energy transfer (charging) begins"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_PARKING_AREA;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::STOPPINGPLACE,
                                      GNETagProperties::MASKSTARTENDPOS,
                                      GUIIcon::PARKINGAREA, currentTag, {}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of ParkingArea"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the lane the Parking Area shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The begin position on the lane (the lower position on the lane) in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTPOS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Lane position in that vehicle must depart when leaves parkingArea"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of parkingArea"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROADSIDE_CAPACITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL(" The number of parking spaces for road-side parking"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ONROAD,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, vehicles will park on the road lane and thereby reducing capacity"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The width of the road-side parking spaces"),
                                              toString(SUMO_const_laneWidth));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The length of the road-side parking spaces. By default (endPos - startPos) / roadsideCapacity"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ANGLE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::ANGLE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The angle of the road-side parking spaces relative to the lane angle, positive means clockwise"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_PARKING_SPACE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::REPARENT | GNETagProperties::RTREE,
                                      GUIIcon::PARKINGSPACE, currentTag, {SUMO_TAG_PARKING_AREA}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::POSITION | GNEAttributeProperties::UPDATEGEOMETRY, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                                              TL("The x-y-z position of the node on the plane in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of parking space"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The width of the road-side parking spaces"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The length of the road-side parking spaces"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ANGLE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The angle of the road-side parking spaces relative to the lane angle, positive means clockwise"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SLOPE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::ANGLE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The slope of the road-side parking spaces"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_INDUCTION_LOOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::DETECTOR,
                                      0,
                                      GUIIcon::E1, currentTag, {}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of E1"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the lane the detector shall be laid on. The lane must be a part of the network used"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The aggregation period the values the detector collects shall be summed up"),
                                              "300.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of induction loop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The path to the output file"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Space separated list of vehicle type ids to consider"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_LANE_AREA_DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::DETECTOR,
                                      0,
                                      GUIIcon::E2, currentTag, {}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of E2"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the lane the detector shall be laid on. The lane must be a part of the network used"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the lane the detector shall be laid on in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The length of the detector in meters"),
                                              "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The aggregation period the values the detector collects shall be summed up"),
                                              "300.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The traffic light that triggers aggregation when switching"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of lane area detector"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The path to the output file"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Space separated list of vehicle type ids to consider"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s"),
                                              "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m"),
                                              "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_MULTI_LANE_AREA_DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::DETECTOR,
                                      0,
                                      GUIIcon::E2, SUMO_TAG_LANE_AREA_DETECTOR, {}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of Multilane E2"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::SECUENCIAL | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The sequence of lane ids in which the detector shall be laid on"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the lane the detector shall be laid on in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane the detector shall be laid on in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The aggregation period the values the detector collects shall be summed up"),
                                              "300.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TLID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The traffic light that triggers aggregation when switching"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of Multilane E2 detector"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The path to the output file"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Space separated list of vehicle type ids to consider"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting)"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s"),
                                              "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The minimum distance to the next standing vehicle in order to make this vehicle count as a participant to the jam) in m"),
                                              "10.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_ENTRY_EXIT_DETECTOR;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::DETECTOR,
                                      GNETagProperties::RTREE,
                                      GUIIcon::E3, currentTag, {}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of E3"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("X-Y position of detector in editor (Only used in netedit)"),
                                              "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The aggregation period the values the detector collects shall be summed up"),
                                              "300.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of Entry Exit detector"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The path to the output file"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Space separated list of vehicle type ids to consider"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time-based threshold that describes how much time has to pass until a vehicle is recognized as halting) in s"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The speed-based threshold that describes how slow a vehicle has to be to be recognized as halting) in m/s"),
                                              "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_DET_ENTRY;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::DETECTOR,
                                      GNETagProperties::CHILD | GNETagProperties::REPARENT,
                                      GUIIcon::E3ENTRY, currentTag, {SUMO_TAG_ENTRY_EXIT_DETECTOR}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("The id of the lane the detector shall be laid on. The lane must be a part of the network used"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the lane the detector shall be laid on in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_DET_EXIT;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::DETECTOR,
                                      GNETagProperties::CHILD | GNETagProperties::REPARENT,
                                      GUIIcon::E3EXIT, currentTag, {SUMO_TAG_ENTRY_EXIT_DETECTOR}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the lane the detector shall be laid on. The lane must be a part of the network used"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the lane the detector shall be laid on in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_INSTANT_INDUCTION_LOOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::DETECTOR,
                                      0,
                                      GUIIcon::E1INSTANT, currentTag, {}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of Instant Induction Loop (E1Instant)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the lane the detector shall be laid on. The lane must be a part of the network used"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the lane the detector shall be laid on in meters. The position must be a value between -1*lane's length and the lane's length"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of instant induction loop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The path to the output file"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Space separated list of vehicle type ids to consider"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ROUTEPROBE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CENTERAFTERCREATION,
                                      GUIIcon::ROUTEPROBE, currentTag, {}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of RouteProbe"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of an edge in the simulation network"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The frequency in which to report the distribution"),
                                              "3600.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of route probe"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The file for generated output"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_BEGIN,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time at which to start generating output"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VSS;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::RTREE | GNETagProperties::DIALOG,
                                      GUIIcon::VARIABLESPEEDSIGN, currentTag, {}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of Variable Speed Signal"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("X-Y position of detector in editor (Only used in netedit)"),
                                              "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("List of Variable Speed Sign lanes"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of Variable Speed Signal"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Space separated list of vehicle type ids to consider (empty to affect all types)"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_VSS_SYMBOL;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::SYMBOL,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::LANE, currentTag, {SUMO_TAG_VSS}, FXRGBA(210, 233, 255, 255));
    }
    currentTag = SUMO_TAG_STEP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::VSSSTEP, currentTag, {SUMO_TAG_VSS}, FXRGBA(210, 233, 255, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TIME,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::UNIQUE,
                                              TL("Time"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Speed"),
                                              "13.89");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CALIBRATOR;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::CALIBRATOR,
                                      GNETagProperties::DIALOG | GNETagProperties::CENTERAFTERCREATION,
                                      GUIIcon::CALIBRATOR, currentTag, {}, FXRGBA(253, 255, 206, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of Calibrator"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of edge in the simulation network"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position of the calibrator on the specified lane"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The aggregation interval in which to calibrate the flows. Default is step-length"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of Calibrator"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROUTEPROBE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The id of the routeProbe element from which to determine the route distribution for generated vehicles"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OUTPUT,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The output file for writing calibrator information or NULL"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A threshold value to detect and clear unexpected jamming"),
                                              "0.50");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("space separated list of vehicle type ids to consider (empty to affect all types)"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_CALIBRATOR_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::CALIBRATOR,
                                      GNETagProperties::DIALOG | GNETagProperties::CENTERAFTERCREATION,
                                      GUIIcon::CALIBRATOR, SUMO_TAG_CALIBRATOR, {}, FXRGBA(253, 255, 206, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of Calibrator"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of lane in the simulation network"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position of the calibrator on the specified lane"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The aggregation interval in which to calibrate the flows. Default is step-length"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of calibrator lane"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROUTEPROBE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The id of the routeProbe element from which to determine the route distribution for generated vehicles"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OUTPUT,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The output file for writing calibrator information or NULL"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_JAM_DIST_THRESHOLD,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A threshold value to detect and clear unexpected jamming"),
                                              "0.50");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("space separated list of vehicle type ids to consider (empty to affect all types)"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_CALIBRATOR_FLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::CALIBRATOR,
                                      GNETagProperties::CHILD,
                                      GUIIcon::FLOW, SUMO_TAG_FLOW, {SUMO_TAG_CALIBRATOR}, FXRGBA(253, 255, 206, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROUTE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the route the vehicle shall drive along"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_BEGIN,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("First calibrator flow departure time"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_END,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("End of departure interval"),
                                              "3600");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common vehicle attributes
        fillCommonVehicleAttributes(currentTag);

        // optional attributes (at least one must be defined)
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::VTYPE,
                                              TL("The id of the vehicle type to use for this calibrator flow"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VEHSPERHOUR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::ACTIVATABLE,
                                              TL("Number of vehicles per hour, equally spaced"),
                                              "1800");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::ACTIVATABLE,
                                              TL("Vehicle's speed"),
                                              "15.0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_REROUTER;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::RTREE | GNETagProperties::DIALOG,
                                      GUIIcon::REROUTER, currentTag, {}, FXRGBA(255, 213, 213, 255));

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of Rerouter"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("An edge id or a list of edge ids where vehicles shall be rerouted"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("X,Y position in editor (Only used in netedit)"),
                                              "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of Rerouter"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PROB,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::PROBABILITY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The probability for vehicle rerouting (0-1)"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_TIME_THRESHOLD,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The waiting time threshold (in s) that must be reached to activate rerouting (default -1 which disables the threshold)"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The list of vehicle types that shall be affected by this rerouter (empty to affect all types)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OFF,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Whether the router should be inactive initially (and switched on in the gui)"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_REROUTER_SYMBOL;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::SYMBOL,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::EDGE, currentTag, {GNE_TAG_REROUTER_SYMBOL}, FXRGBA(255, 213, 213, 255));
    }
    currentTag = SUMO_TAG_INTERVAL;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::REROUTERINTERVAL, currentTag, {SUMO_TAG_REROUTER}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_BEGIN,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Begin"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_END,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("End"),
                                              "3600");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CLOSING_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::CLOSINGREROUTE, currentTag, {SUMO_TAG_INTERVAL}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::SYNONYM | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Edge ID"));
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("allowed vehicles"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("disallowed vehicles"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CLOSING_LANE_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::CLOSINGLANEREROUTE, currentTag, {SUMO_TAG_INTERVAL}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::SYNONYM | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Lane ID"));
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("allowed vehicles"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DISALLOW,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::VCLASSES,
                                              TL("disallowed vehicles"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_DEST_PROB_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::DESTPROBREROUTE, currentTag, {SUMO_TAG_INTERVAL}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::SYNONYM | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Edge ID"));
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PROB,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("SUMO Probability"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_PARKING_AREA_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::PARKINGZONEREROUTE, currentTag, {SUMO_TAG_INTERVAL}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_PARKING,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::SYNONYM,
                                              TL("ParkingArea ID"));
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PROB,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("SUMO Probability"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VISIBLE,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Enable or disable visibility for parking area reroutes"),
                                              "1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_ROUTE_PROB_REROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::ROUTEPROBREROUTE, currentTag, {SUMO_TAG_INTERVAL}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROUTE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::SYNONYM | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Route"));
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PROB,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("SUMO Probability"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VAPORIZER;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT,
                                      GNETagProperties::CENTERAFTERCREATION,
                                      GUIIcon::VAPORIZER, currentTag, {}, FXRGBA(253, 255, 206, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::AUTOMATICID,
                                              TL("Edge in which vaporizer is placed"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_BEGIN,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Start Time"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_END,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("End Time"),
                                              "3600");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of vaporizer"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillShapeElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill shape ACs
    SumoXMLTag currentTag = SUMO_TAG_POLY;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::SHAPE,
                                      GNETagProperties::RTREE | GNETagProperties::CLOSESHAPE | GNETagProperties::GEOSHAPE,
                                      GUIIcon::POLY, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the polygon"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE,
                                              TL("The shape of the polygon"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The RGBA color with which the polygon shall be displayed"),
                                              "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILL,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("An information whether the polygon shall be filled"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINEWIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The default line width for drawing an unfilled polygon"),
                                              "1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LAYER,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The layer in which the polygon lies"),
                                              toString(Shape::DEFAULT_LAYER));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A typename for the polygon"),
                                              toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Polygon's name"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_IMGFILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A bitmap to use for rendering this polygon"),
                                              toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_RELATIVEPATH,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Enable or disable use image file as a relative path"),
                                              toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ANGLE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::ANGLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Angle of rendered image in degree"),
                                              toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_POI;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::SHAPE,
                                      GNETagProperties::RTREE,
                                      GUIIcon::POI, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the POI"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY, // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
                                              TL("The position in view"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The color with which the POI shall be displayed"),
                                              "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A typename for the POI"),
                                              toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of POI"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LAYER,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The layer of the POI for drawing and selecting"),
                                              toString(Shape::DEFAULT_LAYER_POI));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Width of rendered image in meters"),
                                              toString(Shape::DEFAULT_IMG_WIDTH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HEIGHT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Height of rendered image in meters"),
                                              toString(Shape::DEFAULT_IMG_HEIGHT));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_IMGFILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A bitmap to use for rendering this POI"),
                                              toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_RELATIVEPATH,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Enable or disable use image file as a relative path"),
                                              toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ANGLE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::ANGLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Angle of rendered image in degree"),
                                              toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_POILANE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::SHAPE,
                                      0,
                                      GUIIcon::POILANE, SUMO_TAG_POI);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the POI"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("The name of the lane at which the POI is located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The position on the named lane or in the net in meters at which the POI is located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION_LAT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The lateral offset on the named lane at which the POI is located at"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The color with which the POI shall be displayed"),
                                              "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A typename for the POI"),
                                              toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of POI"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LAYER,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The layer of the POI for drawing and selecting"),
                                              toString(Shape::DEFAULT_LAYER_POI));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Width of rendered image in meters"),
                                              toString(Shape::DEFAULT_IMG_WIDTH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HEIGHT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Height of rendered image in meters"),
                                              toString(Shape::DEFAULT_IMG_HEIGHT));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_IMGFILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A bitmap to use for rendering this POI"),
                                              toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_RELATIVEPATH,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Enable or disable use image file as a relative path"),
                                              toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ANGLE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::ANGLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Angle of rendered image in degree"),
                                              toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_POIGEO;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::SHAPE,
                                      GNETagProperties::RTREE | GNETagProperties::REQUIRE_PROJ,
                                      GUIIcon::POIGEO, SUMO_TAG_POI);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the POI"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LON,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The longitude position of the parking vehicle on the view"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LAT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The latitude position of the parking vehicle on the view"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The color with which the POI shall be displayed"),
                                              "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A typename for the POI"),
                                              toString(Shape::DEFAULT_TYPE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of POI"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LAYER,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The layer of the POI for drawing and selecting"),
                                              toString(Shape::DEFAULT_LAYER_POI));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Width of rendered image in meters"),
                                              toString(Shape::DEFAULT_IMG_WIDTH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HEIGHT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Height of rendered image in meters"),
                                              toString(Shape::DEFAULT_IMG_HEIGHT));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_IMGFILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("A bitmap to use for rendering this POI"),
                                              toString(Shape::DEFAULT_IMG_FILE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_RELATIVEPATH,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Enable or disable use image file as a relative path"),
                                              toString(Shape::DEFAULT_RELATIVEPATH));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ANGLE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::ANGLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Angle of rendered image in degree"),
                                              toString(Shape::DEFAULT_ANGLE));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillTAZElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill TAZ ACs
    SumoXMLTag currentTag = SUMO_TAG_TAZ;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::TAZELEMENT,
                                      GNETagProperties::RTREE,
                                      GUIIcon::TAZ, currentTag);
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the TAZ"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The shape of the TAZ"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CENTER,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("TAZ center"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FILL,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("An information whether the TAZ shall be filled"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The RGBA color with which the TAZ shall be displayed"),
                                              "red");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_NAME,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Name of POI"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TAZSOURCE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::TAZELEMENT,
                                      GNETagProperties::CHILD,
                                      GUIIcon::TAZEDGE, currentTag, {SUMO_TAG_TAZ});
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::SYNONYM | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of edge in the simulation network"));
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WEIGHT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Depart weight associated to this Edge"),
                                              "1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TAZSINK;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::TAZELEMENT,
                                      GNETagProperties::CHILD,
                                      GUIIcon::TAZEDGE, currentTag, {SUMO_TAG_TAZ});
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::SYNONYM | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of edge in the simulation network"));
        attrProperty.setSynonym(SUMO_ATTR_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WEIGHT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Arrival weight associated to this Edget"),
                                              "1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillWireElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    // fill wire elements
    SumoXMLTag currentTag = SUMO_TAG_TRACTION_SUBSTATION;
    {
        // set tag properties
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::WIRE,
                                      GNETagProperties::RTREE,
                                      GUIIcon::TRACTION_SUBSTATION, currentTag);
        // set attribute properties
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Traction substation ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::POSITION | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("X-Y position of detector in editor (Only used in netedit)"),
                                              "0,0"); // virtual attribute from the combination of the actually attributes SUMO_ATTR_X, SUMO_ATTR_Y
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VOLTAGE,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Voltage of at connection point for the overhead wire"),
                                              "600");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CURRENTLIMIT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Current limit of the feeder line"),
                                              "400");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_OVERHEAD_WIRE_SECTION;
    {
        // set tag properties
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::WIRE,
                                      0,
                                      GUIIcon::OVERHEADWIRE, currentTag);
        // set attribute properties
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Overhead wire segment ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SUBSTATIONID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("Substation to which the circuit is connected"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE,
                                              TL("List of consecutive lanes of the circuit"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Starting position in the specified lane"),
                                              "0.0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Ending position in the specified lane"),
                                              toString(INVALID_DOUBLE));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OVERHEAD_WIRE_FORBIDDEN,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST,
                                              TL("Inner lanes, where placing of overhead wire is restricted"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_OVERHEAD_WIRE_CLAMP;
    {
        // set tag properties
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::ADDITIONALELEMENT | GNETagProperties::WIRE,
                                      0,
                                      GUIIcon::OVERHEADWIRE_CLAMP, currentTag);
        // set attribute properties
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("Overhead wire clamp ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OVERHEAD_WIRECLAMP_START,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("ID of the overhead wire segment, to the start of which the overhead wire clamp is connected"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OVERHEAD_WIRECLAMP_LANESTART,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("ID of the overhead wire segment lane of overheadWireIDStartClamp"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OVERHEAD_WIRECLAMP_END,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("ID of the overhead wire segment, to the end of which the overhead wire clamp is connected"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OVERHEAD_WIRECLAMP_LANEEND,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("ID of the overhead wire segment lane of overheadWireIDEndClamp"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillDemandElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    // fill demand elements
    SumoXMLTag currentTag = SUMO_TAG_ROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::ROUTE,
                                      0,
                                      GUIIcon::ROUTE, currentTag);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of Route"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The edges the vehicle shall drive along, given as their ids, separated using spaces"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("This route's color"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_REPEAT,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The number of times that the edges of this route shall be repeated"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CYCLETIME,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("When defining a repeating route with stops and those stops use the until attribute,") + std::string("\n") +
                                              TL("the times will be shifted forward by 'cycleTime' on each repeat"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_ROUTE_EMBEDDED;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::ROUTE,
                                      GNETagProperties::CHILD,
                                      GUIIcon::ROUTE, SUMO_TAG_ROUTE, {GNE_TAG_VEHICLE_WITHROUTE, GNE_TAG_FLOW_WITHROUTE});

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The edges the vehicle shall drive along, given as their ids, separated using spaces"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("This route's color"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_REPEAT,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The number of times that the edges of this route shall be repeated"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CYCLETIME,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("When defining a repeating route with stops and those stops use the until attribute,") + std::string("\n") +
                                              TL("the times will be shifted forward by 'cycleTime' on each repeat"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VTYPE_DISTRIBUTION;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VTYPE,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOTSELECTABLE | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::VTYPEDISTRIBUTION, currentTag);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("The id of VehicleType distribution"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        /* Disabled until next release
        attrProperty = GNEAttributeProperties(SUMO_ATTR_DETERMINISTIC,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("makes number of assigned cars exact for any multiples of N"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
        */
    }
    currentTag = SUMO_TAG_VTYPE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VTYPE,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOTSELECTABLE | GNETagProperties::VCLASS_ICON,
                                      GUIIcon::VTYPE, currentTag);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                              TL("The id of VehicleType"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(GNE_ATTR_VTYPE_DISTRIBUTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::VTYPE,
                                              TL("Vehicle Type Distribution"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VCLASS,
                                              GNEAttributeProperties::VCLASS | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("An abstract vehicle class"),
                                              "passenger");
        attrProperty.setDiscreteValues(SumoVehicleClassStrings.getStrings(), false);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("This vehicle type's color"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE,
                                              TL("The vehicle's netto-length (length) [m]"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_MINGAP,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE,
                                              TL("Empty space after leader [m]"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_MAXSPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE,
                                              TL("The vehicle's maximum velocity [m/s]"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEEDFACTOR,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::EXTENDED,
                                              TL("The vehicle's expected multiplicator for lane speed limits (or a distribution specifier)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DESIRED_MAXSPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE,
                                              TL("The vehicle's desired maximum velocity (interacts with speedFactor).") + std::string("\n") +
                                              TL("Applicable when no speed limit applies (bicycles, some motorways) [m/s]"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_EMISSIONCLASS,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::EXTENDED,
                                              TL("An abstract emission class"));
        attrProperty.setDiscreteValues(PollutantsInterface::getAllClassesStr(), false);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_GUISHAPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::EXTENDED,
                                              TL("How this vehicle is rendered"));
        attrProperty.setDiscreteValues(SumoVehicleShapeStrings.getStrings(), false);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_WIDTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The vehicle's width [m] (only used for drawing)"),
                                              "1.8");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_HEIGHT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The vehicle's height [m] (only used for drawing)"),
                                              "1.5");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_IMGFILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("Image file for rendering vehicles of this type (should be grayscale to allow functional coloring)"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE_CHANGE_MODEL,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The model used for changing lanes"),
                                              "default");
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::LaneChangeModels.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CAR_FOLLOW_MODEL,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The model used for car-following"),
                                              "Krauss");
        attrProperty.setDiscreteValues(SUMOXMLDefinitions::CarFollowModels.getStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PERSON_CAPACITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::EXTENDED,
                                              TL("The number of persons (excluding an autonomous driver) the vehicle can transport"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CONTAINER_CAPACITY,
                                              GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::EXTENDED,
                                              TL("The number of containers the vehicle can transport"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_BOARDING_DURATION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The time required by a person to board the vehicle"),
                                              "0.50");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LOADING_DURATION,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The time required to load a container onto the vehicle"),
                                              "90.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LATALIGNMENT,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The preferred lateral alignment when using the sublane-model"),
                                              "center");
        attrProperty.setDiscreteValues(SUMOVTypeParameter::getLatAlignmentStrings(), true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_MINGAP_LAT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The minimum lateral gap at a speed difference of 50km/h when using the sublane-model"),
                                              "0.12");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_MAXSPEED_LAT,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The maximum lateral speed when using the sublane-model"),
                                              "1.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ACTIONSTEPLENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The interval length for which vehicle performs its decision logic (acceleration and lane-changing)"),
                                              toString(OptionsCont::getOptions().getFloat("default.action-step-length")));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_PROB,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("The probability when being added to a distribution without an explicit probability"),
                                              toString(DEFAULT_VEH_PROB));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_OSGFILE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("3D model file for this class"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CARRIAGE_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::EXTENDED,
                                              TL("Carriage lengths"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_LOCOMOTIVE_LENGTH,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::EXTENDED,
                                              TL("Locomotive lengths"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_CARRIAGE_GAP,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                              TL("Gap between carriages"),
                                              "1");
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill VType Car Following Model Values (implemented in a separated function to improve code legibility)
        fillCarFollowingModelAttributes(currentTag);

        // fill VType Junction Model Parameters (implemented in a separated function to improve code legibility)
        fillJunctionModelAttributes(currentTag);

        // fill VType Lane Change Model Parameters (implemented in a separated function to improve code legibility)
        fillLaneChangingModelAttributes(currentTag);
    }
}


void
GNEAttributeCarrier::fillVehicleElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = SUMO_TAG_TRIP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE,
                                      0,
                                      GUIIcon::TRIP, currentTag, {}, FXRGBA(253, 255, 206, 255));
        myTagProperties[currentTag].setFieldString("trip (from-to edges)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The ID of trip"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the vehicle type to use for this trip"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the trip starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the trip ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VIA,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::LIST,
                                              TL("List of intermediate edge ids which shall be part of the trip"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPART,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The departure time of the (first) trip which is generated using this trip definition"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_TRIP_JUNCTIONS;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE,
                                      0,
                                      GUIIcon::TRIP_JUNCTIONS, SUMO_TAG_TRIP, {}, FXRGBA(255, 213, 213, 255));
        myTagProperties[currentTag].setFieldString("trip (from-to junctions)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of trip"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the vehicle type to use for this trip"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROMJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the trip starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TOJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the trip ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPART,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The departure time of the (first) trip which is generated using this trip definition"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_VEHICLE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE,
                                      0,
                                      GUIIcon::VEHICLE, currentTag, {}, FXRGBA(210, 233, 255, 255));
        myTagProperties[currentTag].setFieldString("vehicle (over route)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The ID of the vehicle"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the vehicle type to use for this vehicle"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROUTE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the route the vehicle shall drive along"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the vehicle starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the vehicle ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPART,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the vehicle shall enter the network"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_VEHICLE_WITHROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE,
                                      GNETagProperties::EMBEDDED_ROUTE,
                                      GUIIcon::VEHICLE, SUMO_TAG_VEHICLE, {}, FXRGBA(210, 233, 255, 255));
        myTagProperties[currentTag].setFieldString("vehicle (embedded route)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The ID of the vehicle"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the vehicle type to use for this vehicle"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the vehicle starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the vehicle ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPART,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the vehicle shall enter the network"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_FLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE | GNETagProperties::FLOW,
                                      0,
                                      GUIIcon::FLOW, currentTag, {}, FXRGBA(253, 255, 206, 255));
        myTagProperties[currentTag].setFieldString("flow (from-to edges)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The ID of the flow"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the flow type to use for this flow"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the flow starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the flow ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_VIA,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::LIST,
                                              TL("List of intermediate edge ids which shall be part of the flow"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag, SUMO_ATTR_VEHSPERHOUR);
    }
    currentTag = GNE_TAG_FLOW_JUNCTIONS;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE | GNETagProperties::FLOW,
                                      0,
                                      GUIIcon::FLOW_JUNCTIONS, SUMO_TAG_FLOW, {}, FXRGBA(255, 213, 213, 255));
        myTagProperties[currentTag].setFieldString("flow (from-to junctions)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the flow"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the flow type to use for this flow"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROMJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the flow starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TOJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the flow ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag, SUMO_ATTR_VEHSPERHOUR);
    }
    currentTag = GNE_TAG_FLOW_ROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE | GNETagProperties::FLOW,
                                      0,
                                      GUIIcon::ROUTEFLOW, SUMO_TAG_FLOW, {}, FXRGBA(210, 233, 255, 255));
        myTagProperties[currentTag].setFieldString("flow (over route)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The id of the flow"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the flow type to use for this flow"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROUTE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the route the flow shall drive along"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the flow starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the flow ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag, SUMO_ATTR_VEHSPERHOUR);
    }
    currentTag = GNE_TAG_FLOW_WITHROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE | GNETagProperties::FLOW,
                                      GNETagProperties::EMBEDDED_ROUTE,
                                      GUIIcon::ROUTEFLOW, SUMO_TAG_FLOW, {}, FXRGBA(210, 233, 255, 255));
        myTagProperties[currentTag].setFieldString("flow (embedded route)");

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("The name of the flow"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                              TL("The id of the flow type to use for this flow"),
                                              DEFAULT_VTYPE_ID);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the flow starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALEDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The index of the edge within route the flow ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // add common attributes
        fillCommonVehicleAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag, SUMO_ATTR_VEHSPERHOUR);
    }
    /* currently disabled. See #5259
    currentTag = SUMO_TAG_TRIP_TAZ;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
            GNETagProperties::DEMANDELEMENT | GNETagProperties::VEHICLE,
            GNETagProperties::DRAWABLE,
            GUIIcon::TRIP);
    }
    */
}


void
GNEAttributeCarrier::fillStopElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill stops ACs
    SumoXMLTag currentTag = SUMO_TAG_STOP_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP,
                                      GNETagProperties::CHILD | GNETagProperties::MASKSTARTENDPOS | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the lane the stop shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The begin position on the lane (the lower position on the lane) in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION_LAT,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The lateral offset on the named lane at which the vehicle must stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag, false);
    }
    currentTag = SUMO_TAG_STOP_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_BUS_STOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("BusStop associated with this stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag, false);
    }
    currentTag = SUMO_TAG_STOP_CONTAINERSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_CONTAINER_STOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ContainerStop associated with this stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag, false);
    }
    currentTag = SUMO_TAG_STOP_CHARGINGSTATION;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHARGING_STATION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ChargingStation associated with this stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes
        fillCommonStopAttributes(currentTag, false);
    }
    currentTag = SUMO_TAG_STOP_PARKINGAREA;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(255, 213, 213, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_PARKING_AREA,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ParkingArea associated with this stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common stop attributes (no parking)
        fillCommonStopAttributes(currentTag, false);
    }
}


void
GNEAttributeCarrier::fillWaypointElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill waypoints ACs
    SumoXMLTag currentTag = GNE_TAG_WAYPOINT_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP | GNETagProperties::WAYPOINT,
                                      GNETagProperties::CHILD | GNETagProperties::MASKSTARTENDPOS | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WAYPOINT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LANE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the lane the waypoint shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_STARTPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The begin position on the lane (the lower position on the lane) in meters"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_POSITION_LAT,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The lateral offset on the named lane at which the vehicle must waypoint"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common waypoint (stop) attributes
        fillCommonStopAttributes(currentTag, true);
    }
    currentTag = GNE_TAG_WAYPOINT_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP | GNETagProperties::WAYPOINT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WAYPOINT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_BUS_STOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("BusWaypoint associated with this waypoint"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common waypoint (stop) attributes
        fillCommonStopAttributes(currentTag, true);
    }
    currentTag = GNE_TAG_WAYPOINT_CONTAINERSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP | GNETagProperties::WAYPOINT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WAYPOINT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_CONTAINER_STOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ContainerWaypoint associated with this waypoint"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common waypoint (stop) attributes
        fillCommonStopAttributes(currentTag, true);
    }
    currentTag = GNE_TAG_WAYPOINT_CHARGINGSTATION;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP | GNETagProperties::WAYPOINT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WAYPOINT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_CHARGING_STATION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ChargingStation associated with this waypoint"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common waypoint (stop) attributes
        fillCommonStopAttributes(currentTag, true);
    }
    currentTag = GNE_TAG_WAYPOINT_PARKINGAREA;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::STOP | GNETagProperties::WAYPOINT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WAYPOINT, SUMO_TAG_STOP, {SUMO_TAG_ROUTE, SUMO_TAG_TRIP, SUMO_TAG_FLOW}, FXRGBA(240, 255, 205, 255));
        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_PARKING_AREA,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ParkingArea associated with this waypoint"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // fill common waypoint (stop) attributes
        fillCommonStopAttributes(currentTag, true);
    }
}


void
GNEAttributeCarrier::fillPersonElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = SUMO_TAG_PERSON;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSON,
                                      0,
                                      GUIIcon::PERSON, currentTag);

        // add flow attributes
        fillCommonPersonAttributes(currentTag);

        // set specific attribute depart (note: Persons doesn't support triggered and containerTriggered values)
        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPART,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the person shall enter the network"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    currentTag = SUMO_TAG_PERSONFLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSON | GNETagProperties::FLOW,
                                      0,
                                      GUIIcon::PERSONFLOW, currentTag);

        // add flow attributes
        fillCommonPersonAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag, SUMO_ATTR_PERSONSPERHOUR);
    }
}


void
GNEAttributeCarrier::fillContainerElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = SUMO_TAG_CONTAINER;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINER,
                                      0,
                                      GUIIcon::CONTAINER, currentTag);

        // add flow attributes
        fillCommonContainerAttributes(currentTag);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPART,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the container shall enter the network"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_CONTAINERFLOW;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINER | GNETagProperties::FLOW,
                                      0,
                                      GUIIcon::CONTAINERFLOW, currentTag);

        // add common container attribute
        fillCommonContainerAttributes(currentTag);

        // add flow attributes
        fillCommonFlowAttributes(currentTag, SUMO_ATTR_CONTAINERSPERHOUR);
    }
}


void
GNEAttributeCarrier::fillContainerTransportElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill walks
    SumoXMLTag currentTag = GNE_TAG_TRANSPORT_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINERPLAN | GNETagProperties::TRANSPORT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::TRANSPORT_CONTAINERSTOP, SUMO_TAG_TRANSPORT, { SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW }, FXRGBA(240, 255, 205, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the transport starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the transport ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival position
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // lines
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("list of vehicle alternatives to take for the transport"),
                                              "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_TRANSPORT_CONTAINERSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINERPLAN | GNETagProperties::TRANSPORT,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::TRANSPORT_CONTAINERSTOP, SUMO_TAG_TRANSPORT, { SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW }, FXRGBA(240, 255, 205, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the transport starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to busStop
        attrProperty = GNEAttributeProperties(GNE_ATTR_TO_CONTAINERSTOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ID of the destination container stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // lines
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("list of vehicle alternatives to take for the transport"),
                                              "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillContainerTranshipElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill walks
    SumoXMLTag currentTag = GNE_TAG_TRANSHIP_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINERPLAN | GNETagProperties::TRANSHIP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::TRANSHIP_CONTAINERSTOP, SUMO_TAG_TRANSHIP, { SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW }, FXRGBA(210, 233, 255, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the tranship starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the tranship ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // depart pos
        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The position at which the tranship shall enter the net"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival position
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // speed
        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("speed of the container for this tranship in m/s"),
                                              "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_TRANSHIP_CONTAINERSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINERPLAN | GNETagProperties::TRANSHIP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::TRANSHIP_CONTAINERSTOP, SUMO_TAG_TRANSHIP, { SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW }, FXRGBA(210, 233, 255, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the edge the tranship starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to busStop
        attrProperty = GNEAttributeProperties(GNE_ATTR_TO_CONTAINERSTOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Id of the destination container stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // depart pos
        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The position at which the tranship shall enter the net"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // speed
        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("speed of the container for this tranship in m/s"),
                                              "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_TRANSHIP_EDGES;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINERPLAN | GNETagProperties::TRANSHIP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::TRANSHIP_EDGES, SUMO_TAG_TRANSHIP, {SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW}, FXRGBA(210, 233, 255, 255));
        // edges
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("id of the edges to walk"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // depart pos
        attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The position at which the tranship shall enter the net"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival pos
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // speed
        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("speed of the container for this tranship in m/s"),
                                              "1.39");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillContainerStopElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = GNE_TAG_STOPCONTAINER_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINERPLAN | GNETagProperties::STOPCONTAINER,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW}, FXRGBA(255, 213, 213, 255));

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the stop shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DURATION,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Minimum duration for stopping"),
                                              "60");
        attrProperty.setDefaultActivated(true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_UNTIL,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the route continues"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ACTTYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Activity displayed for stopped container in GUI and output files "));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_STOPCONTAINER_CONTAINERSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::CONTAINERPLAN | GNETagProperties::STOPCONTAINER,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_CONTAINER, SUMO_TAG_CONTAINERFLOW}, FXRGBA(255, 213, 213, 255));

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_CONTAINER_STOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ContainerStop associated with this stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DURATION,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Minimum duration for stopping"),
                                              "60");
        attrProperty.setDefaultActivated(true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_UNTIL,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the route continues"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ACTTYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Activity displayed for stopped container in GUI and output files "));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillPersonPlanTrips() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill person trips
    SumoXMLTag currentTag = GNE_TAG_PERSONTRIP_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::PERSONTRIP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::PERSONTRIP_FROMTO, SUMO_TAG_PERSONTRIP, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW}, FXRGBA(253, 255, 206, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the person trip starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the person trip ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival position
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // vTypes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("List of possible vehicle types to take"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // modes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_MODES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("List of possible traffic modes. Walking is always possible regardless of this value"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // lines
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("list of vehicle alternatives to take for the person trip"),
                                              "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_PERSONTRIP_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::PERSONTRIP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::PERSONTRIP_BUSSTOP, SUMO_TAG_PERSONTRIP, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW}, FXRGBA(253, 255, 206, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the person trip starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to busStop
        attrProperty = GNEAttributeProperties(GNE_ATTR_TO_BUSSTOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Id of the destination busStop"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // vTypes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("List of possible vehicle types to take"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // modes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_MODES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("List of possible traffic modes. Walking is always possible regardless of this value"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // lines
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("list of vehicle alternatives to take for the person trip"),
                                              "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_PERSONTRIP_JUNCTIONS;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::PERSONTRIP,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::PERSONTRIP_JUNCTIONS, SUMO_TAG_PERSONTRIP, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW}, FXRGBA(253, 255, 206, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROMJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the person trip starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TOJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the person trip ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // vTypes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("List of possible vehicle types to take"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // modes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_MODES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("List of possible traffic modes. Walking is always possible regardless of this value"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // lines
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("list of vehicle alternatives to take for the person trip"),
                                              "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillPersonPlanWalks() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill walks
    SumoXMLTag currentTag = GNE_TAG_WALK_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::WALK,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WALK_FROMTO, SUMO_TAG_WALK, { SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW }, FXRGBA(240, 255, 205, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the walk starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the walk ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival position
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_WALK_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::WALK,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WALK_BUSSTOP, SUMO_TAG_WALK, { SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW }, FXRGBA(240, 255, 205, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the walk starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to busStop
        attrProperty = GNEAttributeProperties(GNE_ATTR_TO_BUSSTOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("Id of the destination bus stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_WALK_EDGES;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::WALK,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WALK_EDGES, SUMO_TAG_WALK, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW}, FXRGBA(240, 255, 205, 255));
        // edges
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("id of the edges to walk"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival pos
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_WALK_ROUTE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::WALK,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WALK_ROUTE, SUMO_TAG_WALK, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW}, FXRGBA(240, 255, 205, 255));
        // route
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ROUTE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The id of the route to walk"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival pos
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }

    currentTag = GNE_TAG_WALK_JUNCTIONS;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::WALK,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::WALK_JUNCTIONS, SUMO_TAG_WALK, { SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW }, FXRGBA(240, 255, 205, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROMJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the walk starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TOJUNCTION,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the junction the walk ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillPersonPlanRides() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill rides
    SumoXMLTag currentTag = GNE_TAG_RIDE_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::RIDE,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::RIDE_BUSSTOP, SUMO_TAG_RIDE, { SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW }, FXRGBA(210, 233, 255, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the ride starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the ride ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // arrival position
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("arrival position on the destination edge"),
                                              "-1");
        myTagProperties[currentTag].addAttribute(attrProperty);
        // lines
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("list of vehicle alternatives to take for the ride"),
                                              "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_RIDE_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::RIDE,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::RIDE_BUSSTOP, SUMO_TAG_RIDE, { SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW }, FXRGBA(210, 233, 255, 255));
        // from edge
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the ride starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // to busStop
        attrProperty = GNEAttributeProperties(GNE_ATTR_TO_BUSSTOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("ID of the destination bus stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);
        // lines
        attrProperty = GNEAttributeProperties(SUMO_ATTR_LINES,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("list of vehicle alternatives to take for the ride"),
                                              "ANY");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillStopPersonElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill vehicle ACs
    SumoXMLTag currentTag = GNE_TAG_STOPPERSON_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::STOPPERSON,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW}, FXRGBA(255, 213, 213, 255));

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the stop shall be located at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ENDPOS,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The end position on the lane (the higher position on the lane) in meters, must be larger than startPos by more than 0.1m"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_FRIENDLY_POS,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("If set, no error will be reported if element is placed behind the lane.") + std::string("\n") +
                                              TL("Instead, it will be placed 0.1 meters from the lanes end or at position 0.1,") + std::string("\n") +
                                              TL("if the position was negative and larger than the lanes length after multiplication with - 1"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DURATION,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Minimum duration for stopping"),
                                              "60");
        attrProperty.setDefaultActivated(true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_UNTIL,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the route continues"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ACTTYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Activity displayed for stopped person in GUI and output files "));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = GNE_TAG_STOPPERSON_BUSSTOP;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DEMANDELEMENT | GNETagProperties::PERSONPLAN | GNETagProperties::STOPPERSON,
                                      GNETagProperties::CHILD | GNETagProperties::NOPARAMETERS,
                                      GUIIcon::STOPELEMENT, SUMO_TAG_STOP, {SUMO_TAG_PERSON, SUMO_TAG_PERSONFLOW}, FXRGBA(255, 213, 213, 255));

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_BUS_STOP,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("BusStop associated with this stop"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_DURATION,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Minimum duration for stopping"),
                                              "60");
        attrProperty.setDefaultActivated(true);
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_UNTIL,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("The time step at which the route continues"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_ACTTYPE,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Activity displayed for stopped person in GUI and output files "));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
}


void
GNEAttributeCarrier::fillCommonVehicleAttributes(SumoXMLTag currentTag) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("This vehicle's color"),
                                          "yellow");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTLANE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE |  GNEAttributeProperties::UPDATEGEOMETRY,
                                          TL("The lane on which thevehicle shall be inserted"),
                                          "first");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTPOS,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE |  GNEAttributeProperties::UPDATEGEOMETRY,
                                          TL("The position at which the vehicle shall enter the net"),
                                          "base");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTSPEED,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The speed with which the vehicle shall enter the network"),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALLANE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE |  GNEAttributeProperties::UPDATEGEOMETRY,
                                          TL("The lane at which the vehicle shall leave the network"),
                                          "current");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE |  GNEAttributeProperties::UPDATEGEOMETRY,
                                          TL("The position at which the vehicle shall leave the network"),
                                          "max");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALSPEED,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The speed with which the vehicle shall leave the network"),
                                          "current");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LINE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("A string specifying the id of a public transport line which can be used when specifying person rides"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_PERSON_NUMBER,
                                          GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The number of occupied seats when the vehicle is inserted"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CONTAINER_NUMBER,
                                          GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The number of occupied container places when the vehicle is inserted"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTPOS_LAT,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE |  GNEAttributeProperties::UPDATEGEOMETRY,
                                          TL("The lateral position on the departure lane at which the vehicle shall enter the net"),
                                          "center");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ARRIVALPOS_LAT,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE |  GNEAttributeProperties::UPDATEGEOMETRY,
                                          TL("The lateral position on the arrival lane at which the vehicle shall arrive"),
                                          "center");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_INSERTIONCHECKS,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Insertion checks"),
                                          SUMOXMLDefinitions::InsertionChecks.getString(InsertionCheck::ALL));
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillCommonFlowAttributes(SumoXMLTag currentTag, SumoXMLAttr perHour) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    attrProperty = GNEAttributeProperties(SUMO_ATTR_BEGIN,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("First flow departure time"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_END,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::FLOWDEFINITION,
                                          TL("End of departure interval"),
                                          "3600");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_NUMBER,
                                          GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::FLOWDEFINITION,
                                          TL("probability for emitting a flow each second") + std::string("\n") +
                                          TL("(not together with vehsPerHour or period)"),
                                          "1800");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(perHour,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::FLOWDEFINITION,
                                          TL("Number of flows per hour, equally spaced") + std::string("\n") +
                                          TL("(not together with period or probability or poisson)"),
                                          "1800");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::FLOWDEFINITION,
                                          TL("Insert equally spaced flows at that period") + std::string("\n") +
                                          TL("(not together with vehsPerHour or probability or poisson)"),
                                          "2");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_PROB,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::FLOWDEFINITION,
                                          TL("probability for emitting a flow each second") + std::string("\n") +
                                          TL("(not together with vehsPerHour or period or poisson)"),
                                          "0.5");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(GNE_ATTR_POISSON,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::FLOWDEFINITION,
                                          TL("Insert flow expected vehicles per second with poisson distributed insertion rate") + std::string("\n") +
                                          TL("(not together with period or vehsPerHour or probability)"),
                                          "0.5");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillCarFollowingModelAttributes(SumoXMLTag currentTag) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ACCEL,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The acceleration ability of vehicles of this type [m/s^2]"),
                                          "2.60");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DECEL,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The deceleration ability of vehicles of this type [m/s^2]"),
                                          "4.50");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_APPARENTDECEL,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The apparent deceleration of the vehicle as used by the standard model [m/s^2]"),
                                          "4.50");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_EMERGENCYDECEL,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The maximal physically possible deceleration for the vehicle [m/s^2]"),
                                          "4.50");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_SIGMA,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::RANGE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Car-following model parameter"),
                                          "0.50");
    attrProperty.setRange(0, 1);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TAU,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Car-following model parameter"),
                                          "1.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TMP1,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("SKRAUSSX parameter 1"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TMP2,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("SKRAUSSX parameter 2"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TMP3,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("SKRAUSSX parameter 3"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TMP4,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("SKRAUSSX parameter 4"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TMP5,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("SKRAUSSX parameter 5"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_T_LOOK_AHEAD,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM Look ahead / preview parameter [s]"),
                                          "4.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_T_REACTION,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM AP Reaction Time parameter [s]"),
                                          "0.50");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_T_PERSISTENCE_DRIVE,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM Wiener Process parameter for the Driving Error [s]"),
                                          "3.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_T_PERSISTENCE_ESTIMATE,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM Wiener Process parameter for the Estimation Error [s]"),
                                          "10.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_C_COOLNESS,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::RANGE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM Coolness parameter of the Enhanced IDM [-]"),
                                          "0.99");
    attrProperty.setRange(0, 1);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_SIG_LEADER,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM leader speed estimation error parameter [-]"),
                                          "0.02");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_SIG_GAP,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM gap estimation error parameter [-]"),
                                          "0.10");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_SIG_ERROR,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM driving error parameter [-]"),
                                          "0.04");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_JERK_MAX,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM maximal jerk parameter [m/s^3]"),
                                          "3.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_EPSILON_ACC,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM maximal negative acceleration between two Action Points (threshold) [m/s^2]"),
                                          "1.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_T_ACC_MAX,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM Time parameter until vehicle reaches amax after startup/driveoff [s]"),
                                          "1.20");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_M_FLATNESS,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM Flatness parameter of startup/driveoff curve [-]"),
                                          "2.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_M_BEGIN,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM Shift parameter of startup/driveoff curve [-]"),
                                          "0.70");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_USEVEHDYNAMICS,
                                          GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM parameter if model shall include vehicle dynamics into the acceleration calculation [0/1]"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_EIDM_MAX_VEH_PREVIEW,
                                          GNEAttributeProperties::INT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("EIDM parameter how many vehicles are taken into the preview calculation of the driver (at least always 1!) [-]"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_PWAGNER2009_TAULAST,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Peter Wagner 2009 parameter"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_PWAGNER2009_APPROB,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Peter Wagner 2009 parameter"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_IDMM_ADAPT_FACTOR,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("IDMM parameter"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_IDMM_ADAPT_TIME,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("IDMM parameter"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC1,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "1.3");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC2,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "8.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC3,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "-12.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC4,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "-0.25");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC5,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "0.35");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC6,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "6.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC7,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "0.25");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC8,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "2.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_W99_CC9,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("W99 parameter"),
                                          "1.5");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_WIEDEMANN_SECURITY,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Wiedemann parameter"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_WIEDEMANN_ESTIMATION,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Wiedemann parameter"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_COLLISION_MINGAP_FACTOR,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("MinGap factor parameter"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_K,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("K parameter"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_KERNER_PHI,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Kerner Phi parameter"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_IDM_DELTA,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("IDM Delta parameter"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_CF_IDM_STEPPING,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("IDM Stepping parameter"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TRAIN_TYPE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Train Types"),
                                          "NGT400");
    attrProperty.setDiscreteValues(SUMOXMLDefinitions::TrainTypes.getStrings(), true);
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillJunctionModelAttributes(SumoXMLTag currentTag) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_CROSSING_GAP,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Minimum distance to pedestrians that are walking towards the conflict point with the ego vehicle."),
                                          "10");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_IGNORE_KEEPCLEAR_TIME,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The accumulated waiting time after which a vehicle will drive onto an intersection even though this might cause jamming."),
                                          "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_DRIVE_AFTER_YELLOW_TIME,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("This value causes vehicles to violate a yellow light if the duration of the yellow phase is lower than the given threshold."),
                                          "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_DRIVE_AFTER_RED_TIME,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("This value causes vehicles to violate a red light if the duration of the red phase is lower than the given threshold."),
                                          "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_DRIVE_RED_SPEED,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("This value causes vehicles affected by jmDriveAfterRedTime to slow down when violating a red light."),
                                          "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_IGNORE_FOE_PROB,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("This value causes vehicles to ignore foe vehicles that have right-of-way with the given probability."),
                                          "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_IGNORE_FOE_SPEED,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("This value is used in conjunction with jmIgnoreFoeProb.") + std::string("\n") +
                                          TL("Only vehicles with a speed below or equal to the given value may be ignored."),
                                          "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_SIGMA_MINOR,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("This value configures driving imperfection (dawdling) while passing a minor link."),
                                          "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JM_TIMEGAP_MINOR,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("This value defines the minimum time gap when passing ahead of a prioritized vehicle. "),
                                          "1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_IMPATIENCE,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Willingess of drivers to impede vehicles with higher priority"),
                                          "0.0");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillLaneChangingModelAttributes(SumoXMLTag currentTag) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_STRATEGIC_PARAM,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The eagerness for performing strategic lane changing. Higher values result in earlier lane-changing."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_COOPERATIVE_PARAM,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The willingness for performing cooperative lane changing. Lower values result in reduced cooperation."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_SPEEDGAIN_PARAM,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The eagerness for performing lane changing to gain speed. Higher values result in more lane-changing."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_KEEPRIGHT_PARAM,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The eagerness for following the obligation to keep right. Higher values result in earlier lane-changing."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_SUBLANE_PARAM,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The eagerness for using the configured lateral alignment within the lane.") + std::string("\n") +
                                          TL("Higher values result in increased willingness to sacrifice speed for alignment."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_OPPOSITE_PARAM,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The eagerness for overtaking through the opposite-direction lane. Higher values result in more lane-changing."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_PUSHY,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Willingness to encroach laterally on other drivers."),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_PUSHYGAP,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Minimum lateral gap when encroaching laterally on other drives (alternative way to define lcPushy)"),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_ASSERTIVE,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Willingness to accept lower front and rear gaps on the target lane."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_IMPATIENCE,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Dynamic factor for modifying lcAssertive and lcPushy."),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_TIME_TO_IMPATIENCE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Time to reach maximum impatience (of 1). Impatience grows whenever a lane-change manoeuvre is blocked."),
                                          "infinity");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_ACCEL_LAT,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Maximum lateral acceleration per second."),
                                          "1.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_LOOKAHEADLEFT,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Factor for configuring the strategic lookahead distance when a change to the left is necessary (relative to right lookahead)."),
                                          "2.0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_SPEEDGAINRIGHT,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Factor for configuring the threshold asymmetry when changing to the left or to the right for speed gain."),
                                          "0.1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_MAXSPEEDLATSTANDING,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Upper bound on lateral speed when standing."),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_MAXSPEEDLATFACTOR,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Upper bound on lateral speed while moving computed as lcMaxSpeedLatStanding + lcMaxSpeedLatFactor * getSpeed()"),
                                          "1.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_TURN_ALIGNMENT_DISTANCE,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Distance to an upcoming turn on the vehicles route, below which the alignment") + std::string("\n") +
                                          TL("should be dynamically adapted to match the turn direction."),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_OVERTAKE_RIGHT,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("The probability for violating rules gainst overtaking on the right."),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_KEEPRIGHT_ACCEPTANCE_TIME,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Time threshold for the willingness to change right."),
                                          "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_OVERTAKE_DELTASPEED_FACTOR,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::RANGE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
                                          TL("Speed difference factor for the eagerness of overtaking a neighbor vehicle before changing lanes (threshold = factor*speedlimit)."),
                                          "0.00");
    attrProperty.setRange(-1, 1);
    myTagProperties[currentTag].addAttribute(attrProperty);

    /*
    attrProperty = GNEAttributeProperties(SUMO_ATTR_LCA_EXPERIMENTAL1,
        GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::EXTENDED,
        "XXXXX"),
        "0.00"));
    myTagProperties[currentTag].addAttribute(attrProperty);
    */
}


void
GNEAttributeCarrier::fillCommonPersonAttributes(SumoXMLTag currentTag) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                          TL("The name of the person"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                          TL("The id of the person type to use for this person"),
                                          DEFAULT_VTYPE_ID);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("This person's color"),
                                          "yellow");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DEPARTPOS,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE |  GNEAttributeProperties::UPDATEGEOMETRY,
                                          TL("The position at which the person shall enter the net"),
                                          "base");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillCommonContainerAttributes(SumoXMLTag currentTag) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                          TL("The name of the container"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TYPE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::VTYPE,
                                          TL("The id of the container type to use for this container"),
                                          DEFAULT_CONTAINERTYPE_ID);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_COLOR,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::COLOR | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("This container's color"),
                                          "yellow");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillCommonStopAttributes(SumoXMLTag currentTag, const bool waypoint) {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DURATION,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Minimum duration for stopping"),
                                          "60");
    attrProperty.setDefaultActivated(true);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_UNTIL,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The time step at which the route continues"),
                                          "0.00");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_EXTENSION,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::ACTIVATABLE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("If set to a non-negative time value, then the stop duration can be extended at most by the extension value in seconds"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TRIGGERED,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Whether a person or container or bth may end the stop"),
                                          "0");
    attrProperty.setDiscreteValues({"0", "person", "container", "join"}, true);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_EXPECTED,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("List of elements that must board the vehicle before it may continue"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_PERMITTED,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("List of elements that can board the vehicle before it may continue"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_PARKING,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Whether the vehicle stops on the road or beside"),
                                          "0");
    attrProperty.setDiscreteValues({"1", "0", "opportunistic"}, true);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_ACTTYPE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Activity displayed for stopped person in GUI and output files"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TRIP_ID,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Parameter to be applied to the vehicle to track the trip id within a cyclical public transport route"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_LINE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("New line attribute to be set on the vehicle when reaching this stop (for cyclical public transport route)"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    if (waypoint) {
        attrProperty = GNEAttributeProperties(SUMO_ATTR_SPEED,
                                              GNEAttributeProperties::FLOAT | GNEAttributeProperties::POSITIVE | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Speed to be kept while driving between startPos and endPos"),
                                              "0.00");
        myTagProperties[currentTag].addAttribute(attrProperty);
    } else {
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ONDEMAND,
                                              GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Whether the stop may be skipped if no passengers wants to embark or disembark"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }

    attrProperty = GNEAttributeProperties(SUMO_ATTR_JUMP,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("transfer time if there shall be a jump from this stop to the next route edge"),
                                          "-1");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::fillDataElements() {
    // declare empty GNEAttributeProperties
    GNEAttributeProperties attrProperty;
    // fill data set element
    SumoXMLTag currentTag = SUMO_TAG_DATASET;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DATAELEMENT,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOPARAMETERS | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::DATASET, currentTag);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Data set ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

    }
    // fill data interval element
    currentTag = SUMO_TAG_DATAINTERVAL;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DATAELEMENT,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOPARAMETERS | GNETagProperties::CHILD | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::DATAINTERVAL, currentTag, {SUMO_TAG_DATASET});

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::AUTOMATICID,
                                              TL("Interval ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_BEGIN,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Data interval begin time"),
                                              "0");
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_END,
                                              GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                              TL("Data interval end time"),
                                              "3600");
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    // fill edge data element
    currentTag = GNE_TAG_EDGEREL_SINGLE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DATAELEMENT | GNETagProperties::GENERICDATA,
                                      0,
                                      GUIIcon::EDGEDATA, SUMO_TAG_EDGE);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY | GNEAttributeProperties::AUTOMATICID,
                                              TL("edge ID"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_EDGEREL;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DATAELEMENT | GNETagProperties::GENERICDATA,
                                      0,
                                      GUIIcon::EDGERELDATA, currentTag);

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the edgeRel starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The ID of the edge the edgeRel ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_TAZREL;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::DATAELEMENT | GNETagProperties::GENERICDATA,
                                      GNETagProperties::RTREE | GNETagProperties::CHILD,
                                      GUIIcon::TAZRELDATA, currentTag, {SUMO_TAG_DATAINTERVAL});

        // set values of attributes
        attrProperty = GNEAttributeProperties(SUMO_ATTR_FROM,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the TAZ the TAZRel starts at"));
        myTagProperties[currentTag].addAttribute(attrProperty);

        attrProperty = GNEAttributeProperties(SUMO_ATTR_TO,
                                              GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE | GNEAttributeProperties::UPDATEGEOMETRY,
                                              TL("The name of the TAZ the TAZRel ends at"));
        myTagProperties[currentTag].addAttribute(attrProperty);
    }
    currentTag = SUMO_TAG_MEANDATA_EDGE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::MEANDATA,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOPARAMETERS | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::MEANDATAEDGE, currentTag);

        // set values of attributes
        fillCommonMeanDataAttributes(currentTag);

    }
    currentTag = SUMO_TAG_MEANDATA_LANE;
    {
        // set values of tag
        myTagProperties[currentTag] = GNETagProperties(currentTag,
                                      GNETagProperties::MEANDATA,
                                      GNETagProperties::NOTDRAWABLE | GNETagProperties::NOPARAMETERS | GNETagProperties::NOTSELECTABLE,
                                      GUIIcon::MEANDATALANE, currentTag);

        // set values of attributes
        fillCommonMeanDataAttributes(currentTag);
    }
}


void
GNEAttributeCarrier::fillCommonMeanDataAttributes(SumoXMLTag currentTag) {
    GNEAttributeProperties attrProperty;
    // fill all meanData attributes
    attrProperty = GNEAttributeProperties(SUMO_ATTR_ID,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::UNIQUE,
                                          TL("The id of this set of measurements"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_FILE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The path to the output file. The path may be relative"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_PERIOD,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The aggregation period the values the detector collects shall be summed up"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_BEGIN,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The time to start writing. If not given, the simulation's begin is used."));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_END,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The time to end writing. If not given the simulation's end is used."));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_EXCLUDE_EMPTY,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::DISCRETE | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("If set to true, edges/lanes which were not use by a vehicle during this period will not be written"),
                                          "default");
    attrProperty.setDiscreteValues({"1", "0", "default"}, true);
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_WITH_INTERNAL,
                                          GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("If set, junction internal edges/lanes will be written as well"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_MAX_TRAVELTIME,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The maximum travel time in seconds to write if only very small movements occur"),
                                          "100000");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_MIN_SAMPLES,
                                          GNEAttributeProperties::SUMOTIME | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Consider an edge/lane unused if it has at most this many sampled seconds"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_HALTING_SPEED_THRESHOLD,
                                          GNEAttributeProperties::FLOAT | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("The maximum speed to consider a vehicle halting;"),
                                          "0.1");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_VTYPES,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("space separated list of vehicle type ids to consider"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_TRACK_VEHICLES,
                                          GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("whether aggregation should be performed over all vehicles that entered the edge/lane in the aggregation interval"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_DETECT_PERSONS,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Whether pedestrians shall be recorded instead of vehicles. Allowed value is walk"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_WRITE_ATTRIBUTES,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("List of attribute names that shall be written"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGES,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::LIST | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Restrict output to the given list of edge ids"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_EDGESFILE,
                                          GNEAttributeProperties::STRING | GNEAttributeProperties::FILENAME | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Restrict output to the given the list of edges given in file"));
    myTagProperties[currentTag].addAttribute(attrProperty);

    attrProperty = GNEAttributeProperties(SUMO_ATTR_AGGREGATE,
                                          GNEAttributeProperties::BOOL | GNEAttributeProperties::DEFAULTVALUE,
                                          TL("Whether the traffic statistic of all edges shall be aggregated into a single value"),
                                          "0");
    myTagProperties[currentTag].addAttribute(attrProperty);
}


void
GNEAttributeCarrier::writeAttributeHelp() {
    if (myTagProperties.size() == 0) {
        fillAttributeCarriers();
    }
    // merge "virtual" netedit tags like  '<walk: edge->edge'
    static std::map<SumoXMLTag, GNETagProperties> xmlTagProperties;
    for (const auto& item : myTagProperties) {
        if (xmlTagProperties.count(item.second.getXMLTag()) == 0) {
            xmlTagProperties[item.second.getXMLTag()] = item.second;
        } else {
            std::set<SumoXMLAttr> attrs;
            auto& old = xmlTagProperties[item.second.getXMLTag()];
            for (auto it = old.begin(); it != old.end(); it++) {
                attrs.insert(it->getAttr());
            }
            for (auto it = item.second.begin(); it != item.second.end(); it++) {
                if (attrs.count(it->getAttr()) == 0) {
                    old.addAttribute(*it);
                }
            }
        }
    }
    const std::string opt = "attribute-help-output";
    OutputDevice::createDeviceByOption(opt);
    OutputDevice& dev = OutputDevice::getDeviceByOption(opt);
    dev << "# Netedit attribute help\n";
    for (const auto& item : xmlTagProperties) {
        if (item.second.begin() == item.second.end()) {
            // don't write elements without attributes, they are only used for internal purposes
            continue;
        }
        if (item.second.getParentTags().empty()) {
            dev << "\n## " << toString(item.first) << "\n";
        } else {
            if (item.first == SUMO_TAG_FLOW) {
                dev << "\n## " << toString(item.first) << "\n";
                dev << "also child element of ";
            } else {
                dev << "\n### " << toString(item.first) << "\n";
                dev << "child element of ";
            }
            bool sep = false;
            for (const auto& pTag : item.second.getParentTags()) {
                if (sep) {
                    dev << ", ";
                } else {
                    sep = true;
                }
                dev << "[" << toString(pTag) << "](#" << StringUtils::to_lower_case(toString(pTag)) << ")";
            }
            dev << "\n\n";
        }
        dev << "| Attribute | Type | Description |\n";
        dev << "|-----------|------|-------------|\n";
        for (const auto& attr : item.second) {
            dev << "|" << toString(attr.getAttr()) << "|"
                << attr.getDescription() << "|"
                << StringUtils::replace(attr.getDefinition(), "\n", " ");
            if (attr.getDefaultValue() != "") {
                dev << " *default:* **" << attr.getDefaultValue() << "**";
            }
            dev << "|\n";
        }
    }
}


/****************************************************************************/
