/****************************************************************************/
// Eclipse SUMO, Simulation of Urban MObility; see https://eclipse.org/sumo
// Copyright (C) 2001-2022 German Aerospace Center (DLR) and others.
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
/// @file    TemplateHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2022
///
// The XML-Handler for templates
/****************************************************************************/
#include <config.h>

#include <utils/common/MsgHandler.h>
#include <utils/common/StringUtils.h>
#include <utils/xml/XMLSubSys.h>

#include "TemplateHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

TemplateHandler::TemplateHandler(const std::string& file) :
    SUMOSAXHandler(file) {
}


TemplateHandler::~TemplateHandler() {}


bool
TemplateHandler::parse() {
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void
TemplateHandler::parseSumoBaseObject(CommonXMLStructure::SumoBaseObject* obj) {
    // switch tag
    switch (obj->getTag()) {
        // Stopping Places
        case SUMO_TAG_INTERVAL:
            buildDataInterval(obj,
                              obj->getStringAttribute(SUMO_ATTR_ID),
                              obj->getDoubleAttribute(SUMO_ATTR_BEGIN),
                              obj->getDoubleAttribute(SUMO_ATTR_END));
            break;
        case SUMO_TAG_EDGE:
            buildEdgeData(obj,
                          obj->getStringAttribute(SUMO_ATTR_ID),
                          obj->getParameters());
            break;
        case SUMO_TAG_EDGEREL:
            buildEdgeRelationData(obj,
                                  obj->getStringAttribute(SUMO_ATTR_FROM),
                                  obj->getStringAttribute(SUMO_ATTR_TO),
                                  obj->getParameters());
            break;
        case SUMO_TAG_TAZREL:
            buildTAZRelationData(obj,
                                 obj->getStringAttribute(SUMO_ATTR_FROM),
                                 obj->getStringAttribute(SUMO_ATTR_TO),
                                 obj->getParameters());
            break;
        default:
            break;
    }
    // now iterate over childrens
    for (const auto& child : obj->getSumoBaseObjectChildren()) {
        // call this function recursively
        parseSumoBaseObject(child);
    }
}


void
TemplateHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {
    // obtain tag
    const SumoXMLTag tag = (element == 0) ? SUMO_TAG_ROOTFILE : static_cast<SumoXMLTag>(element);
    // open SUMOBaseOBject
    myCommonXMLStructure.openSUMOBaseOBject();
    // check tag
    try {
        switch (tag) {
            // interval
            case SUMO_TAG_INTERVAL:
                parseInterval(attrs);
                break;
            // datas
            case SUMO_TAG_EDGE:
                parseEdgeData(attrs);
                break;
            case SUMO_TAG_EDGEREL:
                parseEdgeRelationData(attrs);
                break;
            case SUMO_TAG_TAZREL:
                parseTAZRelationData(attrs);
                break;
            case SUMO_TAG_PARAM:
                WRITE_WARNING(TL("Data elements cannot load attributes as params"));
                break;
            default:
                break;
        }
    } catch (InvalidArgument& e) {
        writeError(e.what());
    }
}


void
TemplateHandler::myEndElement(int element) {
    // obtain tag
    const SumoXMLTag tag = static_cast<SumoXMLTag>(element);
    // get last inserted object
    CommonXMLStructure::SumoBaseObject* obj = myCommonXMLStructure.getCurrentSumoBaseObject();
    // close SUMOBaseOBject
    myCommonXMLStructure.closeSUMOBaseOBject();
    // check tag
    switch (tag) {
        // only interval
        case SUMO_TAG_INTERVAL:
            // parse object and all their childrens
            parseSumoBaseObject(obj);
            // delete object (and all of their childrens)
            delete obj;
            break;
        default:
            break;
    }
}


bool
TemplateHandler::isErrorCreatingElement() const {
    return myErrorCreatingElement;
}


void
TemplateHandler::writeError(const std::string& error) {
    WRITE_ERROR(error);
    myErrorCreatingElement = true;
}


void
TemplateHandler::parseInterval(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    const double begin = attrs.get<double>(SUMO_ATTR_BEGIN, "", parsedOk);
    const double end = attrs.get<double>(SUMO_ATTR_END, "", parsedOk);
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_INTERVAL);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_BEGIN, begin);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addDoubleAttribute(SUMO_ATTR_END, end);
    }
}


void
TemplateHandler::parseEdgeData(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string id = attrs.get<std::string>(SUMO_ATTR_ID, "", parsedOk);
    // fill attributes
    getAttributes(attrs, {SUMO_ATTR_ID});
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_EDGE);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_ID, id);
    }
}


void
TemplateHandler::parseEdgeRelationData(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string from = attrs.get<std::string>(SUMO_ATTR_FROM, "", parsedOk);
    const std::string to = attrs.get<std::string>(SUMO_ATTR_TO, "", parsedOk);
    // fill attributes
    getAttributes(attrs, {SUMO_ATTR_FROM, SUMO_ATTR_TO});
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_EDGEREL);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
    }
}


void
TemplateHandler::parseTAZRelationData(const SUMOSAXAttributes& attrs) {
    // declare Ok Flag
    bool parsedOk = true;
    // needed attributes
    const std::string from = attrs.get<std::string>(SUMO_ATTR_FROM, "", parsedOk);
    const std::string to = attrs.get<std::string>(SUMO_ATTR_TO, "", parsedOk);
    // fill attributes
    getAttributes(attrs, {SUMO_ATTR_FROM, SUMO_ATTR_TO});
    // continue if flag is ok
    if (parsedOk) {
        // set tag
        myCommonXMLStructure.getCurrentSumoBaseObject()->setTag(SUMO_TAG_TAZREL);
        // add all attributes
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_FROM, from);
        myCommonXMLStructure.getCurrentSumoBaseObject()->addStringAttribute(SUMO_ATTR_TO, to);
    }
}


void
TemplateHandler::getAttributes(const SUMOSAXAttributes& attrs, const std::vector<SumoXMLAttr> avoidAttributes) const {
    // transform avoidAttributes to strings
    std::vector<std::string> avoidAttributesStr;
    for (const SumoXMLAttr& avoidAttribute : avoidAttributes) {
        avoidAttributesStr.push_back(toString(avoidAttribute));
    }
    // iterate over attributes and fill parameters map
    for (const std::string& attribute : attrs.getAttributeNames()) {
        if (std::find(avoidAttributesStr.begin(), avoidAttributesStr.end(), attribute) == avoidAttributesStr.end()) {
            myCommonXMLStructure.getCurrentSumoBaseObject()->addParameter(attribute, attrs.getStringSecure(attribute, ""));
        }
    }
}


void
TemplateHandler::checkParent(const SumoXMLTag currentTag, const SumoXMLTag parentTag, bool& ok) {
    // check that parent SUMOBaseObject's tag is the parentTag
    if ((myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject() &&
            (myCommonXMLStructure.getCurrentSumoBaseObject()->getParentSumoBaseObject()->getTag() == parentTag)) == false) {
        writeError(toString(currentTag) + " must be defined within the definition of a " + toString(parentTag));
        ok = false;
    }
}

/****************************************************************************/
