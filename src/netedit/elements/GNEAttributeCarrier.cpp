/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.dev/sumo
// Copyright (C) 2001-2025 German Aerospace Center (DLR) and others.
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
#include <netedit/GNETagPropertiesDatabase.h>
#include <netedit/GNEUndoList.h>
#include <netedit/GNEViewNet.h>
#include <netedit/changes/GNEChange_Attribute.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/ToString.h>
#include <utils/emissions/PollutantsInterface.h>
#include <utils/geom/GeomConvHelper.h>
#include <utils/gui/div/GUIGlobalSelection.h>
#include <utils/gui/images/VClassIcons.h>
#include <utils/iodevices/OutputDevice.h>
#include <utils/options/OptionsCont.h>
#include <utils/shapes/PointOfInterest.h>

#include "GNEAttributeCarrier.h"

// ===========================================================================
// static members
// ===========================================================================

const std::string GNEAttributeCarrier::FEATURE_LOADED = "loaded";
const std::string GNEAttributeCarrier::FEATURE_GUESSED = "guessed";
const std::string GNEAttributeCarrier::FEATURE_MODIFIED = "modified";
const std::string GNEAttributeCarrier::FEATURE_APPROVED = "approved";
const std::string GNEAttributeCarrier::TRUE_STR = toString(true);
const std::string GNEAttributeCarrier::FALSE_STR = toString(false);

// ===========================================================================
// method definitions
// ===========================================================================

GNEAttributeCarrier::GNEAttributeCarrier(const SumoXMLTag tag, GNENet* net, const std::string& filename, const bool isTemplate) :
    myTagProperty(net->getTagPropertiesDatabase()->getTagProperty(tag, true)),
    myNet(net),
    myFilename(filename),
    myIsTemplate(isTemplate) {
    // check if add this AC to saving file handler
    if (myFilename.size() > 0) {
        // add filename to saving files handler
        if (myTagProperty->isAdditionalElement()) {
            net->getSavingFilesHandler()->addAdditionalFilename(this);
        } else if (myTagProperty->isDemandElement()) {
            net->getSavingFilesHandler()->addDemandFilename(this);
        } else if (myTagProperty->isDataElement()) {
            net->getSavingFilesHandler()->addDataFilename(this);
        } else if (myTagProperty->isMeanData()) {
            net->getSavingFilesHandler()->addMeanDataFilename(this);
        }
    } else {
        // always avoid empty files
        if (myTagProperty->isAdditionalElement() && (net->getSavingFilesHandler()->getAdditionalFilenames().size() > 0)) {
            myFilename = net->getSavingFilesHandler()->getAdditionalFilenames().front();
        } else if (myTagProperty->isDemandElement() && (net->getSavingFilesHandler()->getDemandFilenames().size() > 0)) {
            myFilename = net->getSavingFilesHandler()->getDemandFilenames().front();
        } else if (myTagProperty->isDataElement() && (net->getSavingFilesHandler()->getDataFilenames().size() > 0)) {
            myFilename = net->getSavingFilesHandler()->getDataFilenames().front();
        } else if (myTagProperty->isMeanData() && (net->getSavingFilesHandler()->getMeanDataFilenames().size() > 0)) {
            myFilename = net->getSavingFilesHandler()->getMeanDataFilenames().front();
        }
    }
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


const std::string&
GNEAttributeCarrier::getFilename() const {
    return myFilename;
}


void
GNEAttributeCarrier::changeDefaultFilename(const std::string& file) {
    if (myFilename.empty()) {
        myFilename = file;
    }
}


void
GNEAttributeCarrier::selectAttributeCarrier() {
    auto glObject = getGUIGlObject();
    if (glObject && myTagProperty->isSelectable()) {
        gSelected.select(glObject->getGlID());
        mySelected = true;
    }
}


void
GNEAttributeCarrier::unselectAttributeCarrier() {
    auto glObject = getGUIGlObject();
    if (glObject && myTagProperty->isSelectable()) {
        gSelected.deselect(glObject->getGlID());
        mySelected = false;
    }
}


bool
GNEAttributeCarrier::isAttributeCarrierSelected() const {
    return mySelected;
}


bool
GNEAttributeCarrier::drawUsingSelectColor() const {
    // first check if element is selected
    if (mySelected) {
        // get flag for network element
        const bool networkElement = myTagProperty->isNetworkElement() || myTagProperty->isAdditionalElement();
        // check current supermode
        if (networkElement && myNet->getViewNet()->getEditModes().isCurrentSupermodeNetwork()) {
            return true;
        } else if (myTagProperty->isDemandElement() && myNet->getViewNet()->getEditModes().isCurrentSupermodeDemand()) {
            return true;
        } else if (myTagProperty->isGenericData() && myNet->getViewNet()->getEditModes().isCurrentSupermodeData()) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


bool
GNEAttributeCarrier::drawMovingGeometryPoints() const {
    // get modes
    const auto& modes = myNet->getViewNet()->getEditModes();
    // check conditions
    if (!myNet->getViewNet()->getMouseButtonKeyPressed().shiftKeyPressed()) {
        return false;
    } else if (modes.isCurrentSupermodeNetwork() && (modes.networkEditMode == NetworkEditMode::NETWORK_MOVE)) {
        return true;
    } else if (modes.isCurrentSupermodeDemand() && (modes.demandEditMode == DemandEditMode::DEMAND_MOVE)) {
        return true;
    } else {
        return false;
    }
}


void
GNEAttributeCarrier::markForDrawingFront() {
    myNet->getViewNet()->getMarkFrontElements().markAC(this);
    myDrawInFront = true;
}


void
GNEAttributeCarrier::unmarkForDrawingFront() {
    myNet->getViewNet()->getMarkFrontElements().unmarkAC(this);
    myDrawInFront = false;
}


bool
GNEAttributeCarrier::isMarkedForDrawingFront() const {
    return myDrawInFront;
}


void
GNEAttributeCarrier::drawInLayer(double typeOrLayer, const double extraOffset) const {
    if (myDrawInFront) {
        glTranslated(0, 0, GLO_FRONTELEMENT + extraOffset);
    } else {
        glTranslated(0, 0, typeOrLayer + extraOffset);
    }
}


void
GNEAttributeCarrier::setInGrid(bool value) {
    myInGrid = value;
}


bool
GNEAttributeCarrier::inGrid() const {
    return myInGrid;
}


bool
GNEAttributeCarrier::checkDrawInspectContour() const {
    return myNet->getViewNet()->getInspectedElements().isACInspected(this);
}


bool
GNEAttributeCarrier::checkDrawFrontContour() const {
    return myDrawInFront;
}


void
GNEAttributeCarrier::resetDefaultValues(const bool allowUndoRedo) {
    if (allowUndoRedo) {
        // reset within undo-redo
        const auto undoList = myNet->getViewNet()->getUndoList();
        undoList->begin(myTagProperty->getGUIIcon(), TLF("reset %", myTagProperty->getTagStr()));
        for (const auto& attrProperty : myTagProperty->getAttributeProperties()) {
            if (!attrProperty->isUnique() && attrProperty->hasDefaultValue()) {
                setAttribute(attrProperty->getAttr(), attrProperty->getDefaultStringValue(), undoList);
                if (attrProperty->isActivatable()) {
                    if (attrProperty->getDefaultActivated()) {
                        enableAttribute(attrProperty->getAttr(), undoList);
                    } else {
                        disableAttribute(attrProperty->getAttr(), undoList);
                    }
                }
            }
        }
        undoList->end();
    } else {
        // simply reset every
        for (const auto& attrProperty : myTagProperty->getAttributeProperties()) {
            if (attrProperty->hasDefaultValue()) {
                setAttribute(attrProperty->getAttr(), attrProperty->getDefaultStringValue());
                if (attrProperty->isActivatable()) {
                    toggleAttribute(attrProperty->getAttr(), attrProperty->getDefaultActivated());
                }
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


bool
GNEAttributeCarrier::hasAttribute(SumoXMLAttr key) const {
    return myTagProperty->hasAttribute(key);
}

// canParse functions

template<> bool
GNEAttributeCarrier::canParse<int>(const std::string& string) {
    if (string == "INVALID_INT") {
        return true;
    } else {
        return StringUtils::isInt(string);
    }
}


template<> bool
GNEAttributeCarrier::canParse<double>(const std::string& string) {
    if (string == "INVALID_DOUBLE") {
        return true;
    } else {
        return StringUtils::isDouble(string);
    }
}


template<> bool
GNEAttributeCarrier::canParse<SUMOTime>(const std::string& string) {
    return isTime(string);
}


template<> bool
GNEAttributeCarrier::canParse<bool>(const std::string& string) {
    return StringUtils::isBool(string);
}


template<> bool
GNEAttributeCarrier::canParse<Position>(const std::string& string) {
    bool ok = true;
    GeomConvHelper::parseShapeReporting(string, "position", 0, ok, true, false);
    return ok;
}


template<> bool
GNEAttributeCarrier::canParse<SUMOVehicleClass>(const std::string& string) {
    return SumoVehicleClassStrings.hasString(string);
}


template<> bool
GNEAttributeCarrier::canParse<RGBColor>(const std::string& string) {
    return RGBColor::isColor(string);
}


template<> bool
GNEAttributeCarrier::canParse<SumoXMLAttr>(const std::string& string) {
    return SUMOXMLDefinitions::Attrs.hasString(string);
}


template<> bool
GNEAttributeCarrier::canParse<SUMOVehicleShape>(const std::string& string) {
    if (string.empty()) {
        return true;
    } else {
        return SumoVehicleShapeStrings.hasString(string);
    }
}


template<> bool
GNEAttributeCarrier::canParse<PositionVector>(const std::string& string) {
    bool ok = true;
    GeomConvHelper::parseShapeReporting(string, "shape", 0, ok, true, false);
    return ok;
}


template<> bool
GNEAttributeCarrier::canParse<std::vector<int> >(const std::string& string) {
    if (string.empty()) {
        return true;
    }
    const auto values = StringTokenizer(string).getVector();
    for (const auto& value : values) {
        if (!canParse<int>(value)) {
            return false;
        }
    }
    return true;
}


template<> bool
GNEAttributeCarrier::canParse<std::vector<double> >(const std::string& string) {
    if (string.empty()) {
        return true;
    }
    const auto values = StringTokenizer(string).getVector();
    for (const auto& value : values) {
        if (!canParse<double>(value)) {
            return false;
        }
    }
    return true;
}


template<> bool
GNEAttributeCarrier::canParse<std::vector<bool> >(const std::string& string) {
    if (string.empty()) {
        return true;
    }
    const auto values = StringTokenizer(string).getVector();
    for (const auto& value : values) {
        if (!canParse<bool>(value)) {
            return false;
        }
    }
    return true;
}


template<> bool
GNEAttributeCarrier::canParse<std::vector<SumoXMLAttr> >(const std::string& string) {
    if (string.empty()) {
        return true;
    }
    const auto values = StringTokenizer(string).getVector();
    for (const auto& value : values) {
        if (!canParse<SumoXMLAttr>(value)) {
            return false;
        }
    }
    return true;
}

// parse functions

template<> int
GNEAttributeCarrier::parse(const std::string& string) {
    if (string == "INVALID_INT") {
        return INVALID_INT;
    } else {
        return StringUtils::toInt(string);
    }
}


template<> double
GNEAttributeCarrier::parse(const std::string& string) {
    if (string == "INVALID_DOUBLE") {
        return INVALID_DOUBLE;
    } else {
        return StringUtils::toDouble(string);
    }
}


template<> SUMOTime
GNEAttributeCarrier::parse(const std::string& string) {
    return string2time(string);
}


template<> bool
GNEAttributeCarrier::parse(const std::string& string) {
    return StringUtils::toBool(string);
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
    // we handle empty strings as position invalids
    if (string.size() == 0) {
        return Position::INVALID;
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
    if (string.empty()) {
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
    const auto vectorString = StringTokenizer(string).getVector();
    std::set<std::string> solution;
    for (const auto& stringValue : vectorString) {
        solution.insert(stringValue);
    }
    return solution;
}


template<> std::vector<int>
GNEAttributeCarrier::parse(const std::string& string) {
    const auto vectorInt = parse<std::vector<std::string> >(string);
    std::vector<int> parsedIntValues;
    for (const auto& intValue : vectorInt) {
        parsedIntValues.push_back(parse<int>(intValue));
    }
    return parsedIntValues;
}


template<> std::vector<double>
GNEAttributeCarrier::parse(const std::string& string) {
    const auto vectorDouble = parse<std::vector<std::string> >(string);
    std::vector<double> parsedDoubleValues;
    for (const auto& doubleValue : vectorDouble) {
        parsedDoubleValues.push_back(parse<double>(doubleValue));
    }
    return parsedDoubleValues;
}


template<> std::vector<bool>
GNEAttributeCarrier::parse(const std::string& string) {
    const auto vectorBool = parse<std::vector<std::string> >(string);
    std::vector<bool> parsedBoolValues;
    for (const auto& boolValue : vectorBool) {
        parsedBoolValues.push_back(parse<bool>(boolValue));
    }
    return parsedBoolValues;
}


template<> std::vector<SumoXMLAttr>
GNEAttributeCarrier::parse(const std::string& value) {
    // Declare string vector
    const auto attributesStr = parse<std::vector<std::string> > (value);
    std::vector<SumoXMLAttr> attributes;
    // Iterate over lanes IDs, retrieve Lanes and add it into parsedLanes
    for (const auto& attributeStr : attributesStr) {
        if (SUMOXMLDefinitions::Attrs.hasString(attributeStr)) {
            attributes.push_back(static_cast<SumoXMLAttr>(SUMOXMLDefinitions::Attrs.get(attributeStr)));
        } else {
            throw FormatException("Error parsing attributes. Attribute '" + attributeStr + "'  doesn't exist");
        }
    }
    return attributes;
}

// can parse (network) functions

template<> bool
GNEAttributeCarrier::canParse<std::vector<GNEEdge*> >(const GNENet* net, const std::string& value, const bool checkConsecutivity) {
    // Declare string vector
    const auto edgeIds = parse<std::vector<std::string> > (value);
    std::vector<GNEEdge*> parsedEdges;
    parsedEdges.reserve(edgeIds.size());
    for (const auto& edgeID : edgeIds) {
        const auto edge = net->getAttributeCarriers()->retrieveEdge(edgeID, false);
        if (edge == nullptr) {
            return false;
        } else if (checkConsecutivity) {
            if ((parsedEdges.size() > 0) && (parsedEdges.back()->getToJunction() != edge->getFromJunction())) {
                return false;
            }
            parsedEdges.push_back(edge);
        }
    }
    return true;
}


template<> bool
GNEAttributeCarrier::canParse<std::vector<GNELane*> >(const GNENet* net, const std::string& value, const bool checkConsecutivity) {
    // Declare string vector
    const auto laneIds = parse<std::vector<std::string> > (value);
    std::vector<GNELane*> parsedLanes;
    parsedLanes.reserve(laneIds.size());
    // Iterate over lanes IDs, retrieve Lanes and add it into parsedLanes
    for (const auto& laneID : laneIds) {
        const auto lane = net->getAttributeCarriers()->retrieveLane(laneID, false);
        if (lane == nullptr) {
            return false;
        } else if (checkConsecutivity) {
            if ((parsedLanes.size() > 0) && (parsedLanes.back()->getParentEdge()->getToJunction() != lane->getParentEdge()->getFromJunction())) {
                return false;
            }
            parsedLanes.push_back(lane);
        }
    }
    return true;
}

// parse (network) functions

template<> std::vector<GNEEdge*>
GNEAttributeCarrier::parse(const GNENet* net, const std::string& value) {
    // Declare string vector
    const auto edgeIds = parse<std::vector<std::string> > (value);
    std::vector<GNEEdge*> parsedEdges;
    parsedEdges.reserve(edgeIds.size());
    // Iterate over edges IDs, retrieve Edges and add it into parsedEdges
    for (const auto& edgeID : edgeIds) {
        parsedEdges.push_back(net->getAttributeCarriers()->retrieveEdge(edgeID));
    }
    return parsedEdges;
}


template<> std::vector<GNELane*>
GNEAttributeCarrier::parse(const GNENet* net, const std::string& value) {
    // Declare string vector
    const auto laneIds = parse<std::vector<std::string> > (value);
    std::vector<GNELane*> parsedLanes;
    parsedLanes.reserve(laneIds.size());
    // Iterate over lanes IDs, retrieve Lanes and add it into parsedLanes
    for (const auto& laneID : laneIds) {
        parsedLanes.push_back(net->getAttributeCarriers()->retrieveLane(laneID));
    }
    return parsedLanes;
}

// parse ID functions

template<> std::string
GNEAttributeCarrier::parseIDs(const std::vector<GNEEdge*>& ACs) {
    // obtain ID's of edges and return their join
    std::vector<std::string> edgeIDs;
    for (const auto& AC : ACs) {
        edgeIDs.push_back(AC->getID());
    }
    return joinToString(edgeIDs, " ");
}


template<> std::string
GNEAttributeCarrier::parseIDs(const std::vector<GNELane*>& ACs) {
    // obtain ID's of lanes and return their join
    std::vector<std::string> laneIDs;
    for (const auto& AC : ACs) {
        laneIDs.push_back(AC->getID());
    }
    return joinToString(laneIDs, " ");
}

void
GNEAttributeCarrier::setACParameters(const std::vector<std::pair<std::string, std::string> >& parameters) {
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
    setAttribute(GNE_ATTR_PARAMETERS, paramsStr);
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
    return myTagProperty->getTagStr();
}


FXIcon*
GNEAttributeCarrier::getACIcon() const {
    // special case for vClass icons
    if (myTagProperty->vClassIcon()) {
        return VClassIcons::getVClassIcon(SumoVehicleClassStrings.get(getAttribute(SUMO_ATTR_VCLASS)));
    } else {
        return GUIIconSubSys::getIcon(myTagProperty->getGUIIcon());
    }
}


bool
GNEAttributeCarrier::isTemplate() const {
    return myIsTemplate;
}


const GNETagProperties*
GNEAttributeCarrier::getTagProperty() const {
    return myTagProperty;
}


std::string
GNEAttributeCarrier::getCommonAttribute(SumoXMLAttr key) const {
    switch (key) {
        case GNE_ATTR_ADDITIONAL_FILE:
        case GNE_ATTR_DEMAND_FILE:
        case GNE_ATTR_DATA_FILE:
        case GNE_ATTR_MEANDATA_FILE:
            return myFilename;
        case GNE_ATTR_CENTER_AFTER_CREATION:
            return toString(myCenterAfterCreation);
        case GNE_ATTR_SELECTED:
            if (mySelected) {
                return TRUE_STR;
            } else {
                return FALSE_STR;
            }
        case GNE_ATTR_FRONTELEMENT:
            if (myDrawInFront) {
                return TRUE_STR;
            } else {
                return FALSE_STR;
            }
        case GNE_ATTR_PARAMETERS:
            if (getParameters()) {
                return getParameters()->getParametersStr();
            } else {
                throw InvalidArgument(getTagStr() + " doesn't support parameters");
            }
        default:
            throw InvalidArgument(getTagStr() + " doesn't have a common attribute of type '" + toString(key) + "'");
    }
}


double
GNEAttributeCarrier::getCommonAttributeDouble(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a common double attribute of type '" + toString(key) + "'");
}


Position
GNEAttributeCarrier::getCommonAttributePosition(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a common position attribute of type '" + toString(key) + "'");
}


PositionVector
GNEAttributeCarrier::getCommonAttributePositionVector(SumoXMLAttr key) const {
    throw InvalidArgument(getTagStr() + " doesn't have a common positionVector attribute of type '" + toString(key) + "'");
}


void
GNEAttributeCarrier::setCommonAttribute(SumoXMLAttr key, const std::string& value, GNEUndoList* undoList) {
    switch (key) {
        case GNE_ATTR_ADDITIONAL_FILE:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            // update filenames of all additional childrens
            for (auto additionalChild : getHierarchicalElement()->getChildAdditionals()) {
                additionalChild->setAttribute(key, value, undoList);
            }
            break;
        case GNE_ATTR_DEMAND_FILE:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            // update filenames of all demand childrens
            for (auto demandChild : getHierarchicalElement()->getChildDemandElements()) {
                demandChild->setAttribute(key, myFilename, undoList);
            }
            break;
        case GNE_ATTR_DATA_FILE:
        case GNE_ATTR_MEANDATA_FILE:
        case GNE_ATTR_CENTER_AFTER_CREATION:
        case GNE_ATTR_SELECTED:
        case GNE_ATTR_PARAMETERS:
            GNEChange_Attribute::changeAttribute(this, key, value, undoList);
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


bool
GNEAttributeCarrier::isCommonAttributeValid(SumoXMLAttr key, const std::string& value) const {
    switch (key) {
        case GNE_ATTR_ADDITIONAL_FILE:
        case GNE_ATTR_DEMAND_FILE:
        case GNE_ATTR_DATA_FILE:
        case GNE_ATTR_MEANDATA_FILE:
            return SUMOXMLDefinitions::isValidFilename(value);
        case GNE_ATTR_CENTER_AFTER_CREATION:
        case GNE_ATTR_SELECTED:
            return canParse<bool>(value);
        case GNE_ATTR_PARAMETERS:
            return Parameterised::areParametersValid(value);
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}


void
GNEAttributeCarrier::setCommonAttribute(SumoXMLAttr key, const std::string& value) {
    switch (key) {
        case GNE_ATTR_ADDITIONAL_FILE:
            myFilename = value;
            if (value.empty()) {
                // try to avoid empty files
                if (myNet->getSavingFilesHandler()->getAdditionalFilenames().size() > 0) {
                    myFilename = myNet->getSavingFilesHandler()->getAdditionalFilenames().front();
                }
            } else {
                myNet->getSavingFilesHandler()->addAdditionalFilename(this);
            }
            break;
        case GNE_ATTR_DEMAND_FILE:
            myFilename = value;
            if (value.empty()) {
                // try to avoid empty files
                if (myNet->getSavingFilesHandler()->getDemandFilenames().size() > 0) {
                    myFilename = myNet->getSavingFilesHandler()->getDemandFilenames().front();
                }
            } else {
                myNet->getSavingFilesHandler()->addDemandFilename(this);
            }
            break;
        case GNE_ATTR_DATA_FILE:
            myFilename = value;
            if (value.empty()) {
                // try to avoid empty files
                if (myNet->getSavingFilesHandler()->getDataFilenames().size() > 0) {
                    myFilename = myNet->getSavingFilesHandler()->getDataFilenames().front();
                }
            } else {
                myNet->getSavingFilesHandler()->addDataFilename(this);
            }
            break;
        case GNE_ATTR_MEANDATA_FILE:
            myFilename = value;
            if (value.empty()) {
                // try to avoid empty files
                if (myNet->getSavingFilesHandler()->getMeanDataFilenames().size() > 0) {
                    myFilename = myNet->getSavingFilesHandler()->getMeanDataFilenames().front();
                }
            } else {
                myNet->getSavingFilesHandler()->addMeanDataFilename(this);
            }
            break;
        case GNE_ATTR_CENTER_AFTER_CREATION:
            myCenterAfterCreation = parse<bool>(value);
            break;
        case GNE_ATTR_SELECTED:
            if (parse<bool>(value)) {
                selectAttributeCarrier();
            } else {
                unselectAttributeCarrier();
            }
            break;
        case GNE_ATTR_PARAMETERS:
            if (getParameters()) {
                getParameters()->setParametersStr(value);
            } else {
                throw InvalidArgument(getTagStr() + " doesn't support parameters");
            }
            break;
        default:
            throw InvalidArgument(getTagStr() + " doesn't have an attribute of type '" + toString(key) + "'");
    }
}

// ===========================================================================
// private
// ===========================================================================

void
GNEAttributeCarrier::toggleAttribute(SumoXMLAttr /*key*/, const bool /*value*/) {
    throw ProcessError(TL("Nothing to toggle, implement in Children"));
}

/****************************************************************************/
