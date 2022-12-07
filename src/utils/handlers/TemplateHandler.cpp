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
#include "TemplateHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

void
TemplateHandler::parseTemplate(OptionsCont& options, const std::string &path) {
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
        parser.parse(StringUtils::transcodeToLocal(path).c_str());
        if (handler.myError) {
            throw ProcessError("Could not load template '" + path + "'.");
        }
    } catch (const XERCES_CPP_NAMESPACE::XMLException& e) {
        throw ProcessError("Could not load template '" + path + "':\n " + StringUtils::transcode(e.getMessage()));
    }
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
    if (attributes.getLength() == 4) {
        // needed for attributes.getValue
        int i = 0;
        // obtain all parameters
        const std::string value = StringUtils::transcode(attributes.getValue(i));
        const std::string synonymes = StringUtils::transcode(attributes.getValue(1));
        const std::string type = StringUtils::transcode(attributes.getValue(2));
        const std::string help = StringUtils::transcode(attributes.getValue(3));
        // add option
        addOption(value, synonymes, type, help);
    } else if (attributes.getLength() == 0) {
        mySubTopic = myTopic;
        myOptions.addOptionSubTopic(mySubTopic);
    }
}


bool
TemplateHandler::addOption(const std::string &value, const std::string &synonymes, 
        const std::string &type, const std::string &help) const {
    if (myOptions.exists(myTopic)) {
        WRITE_WARNING(myTopic + " already exists");
        return false;
    } else {
        // declare option
        Option* option = nullptr;
        // create register depending of type
        if (type == "STR") {
            option = new Option_String(value);
        } else if (type == "INT") {
            option = new Option_Integer(0);
        } else if ((type == "FLOAT") || (type == "TIME")) {
            option = new Option_Float(0);
        } else if (type == "BOOL") {
            option = new Option_Bool(false);
        } else if (type == "INT[]") {
            option = new Option_IntVector();
        } else if (type == "STR[]") {
            option = new Option_StringVector();
        } else if (type == "FILE") {
            option = new Option_FileName();
        } else {
            WRITE_WARNING(type + " is an invalid type");
        }
        // check if option was created
        if (option) {
            // set value
            option->set(value, "", true);
            myOptions.doRegister(myTopic, option);
            // check if add synonyme
            if (synonymes.size() > 0) {
                myOptions.addSynonyme(myTopic, synonymes);
            }
            // check if add help
            if (help.size() > 0) {
                myOptions.addDescription(myTopic, mySubTopic, help);
            }
            return true;
        } else {
            return false;
        }
    }
}


void
TemplateHandler::endElement(const XMLCh* const /*name*/) {
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
