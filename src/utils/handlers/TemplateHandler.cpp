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
#include <utils/options/OptionsCont.h>
#include <utils/xml/XMLSubSys.h>

#include "TemplateHandler.h"


// ===========================================================================
// method definitions
// ===========================================================================

TemplateHandler::TemplateHandler(OptionsCont &options, const std::string& file) :
    SUMOSAXHandler(file),
    myOptions(options) {
}


TemplateHandler::~TemplateHandler() {}


bool
TemplateHandler::parse() {
    // run parser and return result
    return XMLSubSys::runParser(*this, getFileName());
}


void
TemplateHandler::myStartElement(int element, const SUMOSAXAttributes& attrs) {   
    // declare new option
    Option option;
    // set name
    // iterate over attributes and fill parameters map
    for (const std::string& attribute : attrs.getAttributeNames()) {
        if (attribute == "value") {
            option.value = attrs.getStringSecure(attribute, "");
        } else if (attribute == "synonymes") { 
            option.synonymes = attrs.getStringSecure(attribute, "");
        } else if (attribute == "type") {
            option.type = attrs.getStringSecure(attribute, "");
        } else if (attribute == "help") { 
            option.help = attrs.getStringSecure(attribute, "");
        }
    }
    // if value and type was defined, add it to options
    if (option.value.size() > 0) {
    /*
        // check type
        if (option.type == "STR") {
            myOptions.doRegister(option.value, new Option_String());
        } else if (option.type == "FLOAT") {
            myOptions.doRegister(option.value, new Option_Float());
        } else if (option.type == "INT") {
            myOptions.doRegister(option.value, new Option_Integer());
        } else if (option.type == "FILE") {
            myOptions.doRegister(option.value, new Option_FileName());
        } else if (option.type == "BOOL") {
            myOptions.doRegister(option.value, new Option_Bool());
        } else if (option.type == "INT[]") {
            myOptions.doRegister(option.value, new Option_IntVector());
        } else if (option.type == "STR[]") {
            myOptions.doRegister(option.value, new Option_StringVector());
        } 
        */
        /*
        myOptions.

        myOptions.doRegister("additionals-output", new Option_String());
        myOptions.addDescription("additionals-output", "Netedit", "file in which additionals must be saved");
        */
    }
}


void
TemplateHandler::myEndElement(int /*element*/) {
}

/****************************************************************************/
