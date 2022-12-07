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
    parser.setValidationScheme(XERCES_CPP_NAMESPACE::SAXParser::Val_Never);
    parser.setDisableDefaultEntityResolution(true);
    // start the parsing
    TemplateHandler handler(options);
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
    myOptions(options), 
    myItem() {
}


TemplateHandler::~TemplateHandler() {}


void TemplateHandler::startElement(const XMLCh* const name, XERCES_CPP_NAMESPACE::AttributeList& attributes) {
    myItem = StringUtils::transcode(name);
    for (int i = 0; i < (int)attributes.getLength(); i++) {
        const std::string& key = StringUtils::transcode(attributes.getName(i));
        const std::string& value = StringUtils::transcode(attributes.getValue(i));
        if (key == "value" || key == "v") {
            setValue(myItem, value);
        }
        // could give a hint here about unsupported attributes in configuration files
    }
    myValue = "";
}


void TemplateHandler::setValue(const std::string& key, const std::string& value) {
    if (value.length() > 0) {
        try {
            if (!addOption(key, value)) {
                myError = true;
            }
        } catch (ProcessError& e) {
            WRITE_ERROR(e.what());
            myError = true;
        }
    }
}


void TemplateHandler::characters(const XMLCh* const chars, const XERCES3_SIZE_t length) {
    myValue = myValue + StringUtils::transcode(chars, (int) length);
}


bool
TemplateHandler::addOption(const std::string& name, const std::string& value) const {
    if (myOptions.exists(name)) {
        WRITE_WARNING(name + " already exists");
        return false;
    } else {
        myOptions.doRegister(name, new Option_String("bs"));
        //oc.addDescription("busStop-prefix", "Netedit", "prefix for busStop naming");
        return true;
    }

}


void
TemplateHandler::endElement(const XMLCh* const /*name*/) {
    if (myItem.length() == 0 || myValue.length() == 0) {
        return;
    }
    if (myValue.find_first_not_of("\n\t \a") == std::string::npos) {
        return;
    }
    setValue(myItem, myValue);
    myItem = "";
    myValue = "";
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
