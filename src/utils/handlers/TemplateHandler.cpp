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
/// @file    TemplateHandler.cpp
/// @author  Pablo Alvarez Lopez
/// @date    Dec 2022
///
// A SAX-Handler for loading templates
/****************************************************************************/
#include <config.h>

#include <algorithm>
#include <string>
#include <vector>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <utils/common/StringUtils.h>
#include <utils/common/StringTokenizer.h>
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>
#include <utils/options/OptionsCont.h>
#include <xercesc/parsers/SAXParser.hpp>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/framework/LocalFileInputSource.hpp>
#include <xercesc/framework/MemBufInputSource.hpp>

#include "TemplateHandler.h"


const std::string TemplateHandler::INVALID_INT_STR = toString(INVALID_INT);
const std::string TemplateHandler::INVALID_DOUBLE_STR = toString(INVALID_DOUBLE);

// ===========================================================================
// method definitions
// ===========================================================================

void
TemplateHandler::parseTemplate(OptionsCont& options, const std::string& templateString) {
    // build parser
    XERCES_CPP_NAMESPACE::SAXParser parser;
    // disable validation
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // build TemplateHandler
    TemplateHandler handler(options);
    // start parsing
    try {
        parser.setDocumentHandler(&handler);
        parser.setErrorHandler(&handler);
        XERCES_CPP_NAMESPACE::MemBufInputSource memBufIS((const XMLByte*)templateString.c_str(), templateString.size(), "template");
        parser.parse(memBufIS);
        if (handler.myError) {
            throw ProcessError(TLF("Could not load template '%'.", templateString));
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        throw ProcessError("Could not load template '" + templateString + "':\n " + StringUtils::transcode(e.getMessage()));
    }
    // mark al loaded options as default
    options.resetDefault();
}

TemplateHandler::TemplateHandler(OptionsCont& options) :
    myError(false),
    myOptions(options) {
}


TemplateHandler::~TemplateHandler() {}


void
TemplateHandler::startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes) {
    // get current topic
    myTopic = StringUtils::transcode(name);
    if (myLevel++ == 0) {
        // skip root elemnt
        return;
    }
    // check if this is a subtopic
    if (attributes.getLength() == 0) {
        mySubTopic = myTopic;
        myOptions.addOptionSubTopic(mySubTopic);
    } else {
        std::vector<std::string> optionAttrs;
        optionAttrs.resize(5);
        for (int i = 0; i < (int)attributes.getLength(); i++) {
            if (StringUtils::transcode(attributes.getName(i)) == "value") {
                optionAttrs.at(0) = StringUtils::transcode(attributes.getValue(i));
            } else if (StringUtils::transcode(attributes.getName(i)) == "synonymes") {
                optionAttrs.at(1) = StringUtils::transcode(attributes.getValue(i));
            } else if (StringUtils::transcode(attributes.getName(i)) == "type") {
                optionAttrs.at(2) = StringUtils::transcode(attributes.getValue(i));
            } else if (StringUtils::transcode(attributes.getName(i)) == "help") {
                optionAttrs.at(3) = StringUtils::transcode(attributes.getValue(i));
            } else if (StringUtils::transcode(attributes.getName(i)) == "category") {
                optionAttrs.at(4) = StringUtils::transcode(attributes.getValue(i));
            }
        }
        // add option
        addOption(optionAttrs.at(0), optionAttrs.at(1), optionAttrs.at(2), optionAttrs.at(3), optionAttrs.at(4));
    }
}


bool
TemplateHandler::addOption(const std::string value, const std::string& synonymes,
                           const std::string& type, const std::string& help, const std::string& category) const {
    if (myOptions.exists(myTopic)) {
        WRITE_WARNING(myTopic + " already exists");
        return false;
    } else {
        // declare option
        Option* option = nullptr;
        // create register depending of type
        if ((type == "STR") || (type == "string")) {
            option = new Option_String(value);
        } else if ((type == "INT") || (type == "int")) {
            option = new Option_Integer(0);
            if (value.empty() || (value == "None")) {
                option->set(INVALID_INT_STR, "", true);
            }
        } else if ((type == "FLOAT") || (type == "float") || (type == "TIME") || (type == "time")) {
            option = new Option_Float(0);
            if (value.empty() || (value == "None")) {
                option->set(INVALID_DOUBLE_STR, "", true);
            }
        } else if ((type == "BOOL") || (type == "bool")) {
            option = new Option_Bool(false);
            if (value.empty() || (value == "None")) {
                option->set("false", "", true);
            }
        } else if (type == "INT[]") {
            option = new Option_IntVector();
        } else if (type == "STR[]") {
            option = new Option_StringVector();
        } else if ((type == "FILE") || (type == "file") || (type == "net_file") || (type == "additional_file") ||
                   (type == "route_file") || (type == "edgedata_file")) {
            option = new Option_FileName();
        } else if (type.size() > 0) {
            WRITE_WARNING(type + " is an invalid type");
        }
        // check if option was created
        if (option) {
            // set value
            if (!option->isSet()) {
                option->set(value, "", true);
            }
            myOptions.doRegister(myTopic, option);
            // check if add synonyme
            if (synonymes.size() > 0) {
                myOptions.addSynonyme(myTopic, synonymes);
            }
            // check if add help
            if (help.size() > 0) {
                myOptions.addDescription(myTopic, mySubTopic, help);
            }
            // check if add category
            if (category.size() > 0) {
                myOptions.addCategory(myTopic, mySubTopic, category);
            }
            return true;
        } else {
            return false;
        }
    }
}


void
TemplateHandler::endElement(const XMLCh* const /*name*/) {
    myLevel--;
    if (myTopic.length() == 0) {
        return;
    }
    myTopic = "";
}


void
TemplateHandler::warning(const XERCES_CPP_NAMESPACE::SAXParseException& exception) {
    WRITE_WARNING(StringUtils::transcode(exception.getMessage()));
    WRITE_WARNING(" (At line/column " \
                  + toString(exception.getLineNumber() + 1) + '/' \
                  + toString(exception.getColumnNumber()) + ").");
    myError = true;
}


void
TemplateHandler::error(const XERCES_CPP_NAMESPACE::SAXParseException& exception) {
    WRITE_ERROR(StringUtils::transcode(exception.getMessage()));
    WRITE_ERROR(" (At line/column "
                + toString(exception.getLineNumber() + 1) + '/'
                + toString(exception.getColumnNumber()) + ").");
    myError = true;
}


void
TemplateHandler::fatalError(const XERCES_CPP_NAMESPACE::SAXParseException& exception) {
    WRITE_ERROR(StringUtils::transcode(exception.getMessage()));
    WRITE_ERROR(" (At line/column "
                + toString(exception.getLineNumber() + 1) + '/'
                + toString(exception.getColumnNumber()) + ").");
    myError = true;
}

/****************************************************************************/
