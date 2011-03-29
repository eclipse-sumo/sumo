/****************************************************************************/
/// @file    OptionsLoader.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 17 Dec 2001
/// @version $Id$
///
// A SAX-Handler for loading options
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// Copyright (C) 2001-2011 DLR (http://www.dlr.de/) and contributors
/****************************************************************************/
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
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

#include <algorithm>
#include <string>
#include <vector>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/AttributeList.hpp>
#include <xercesc/sax/SAXParseException.hpp>
#include <xercesc/sax/SAXException.hpp>
#include <utils/common/TplConvert.h>
#include <utils/common/StringTokenizer.h>
#include "OptionsLoader.h"
#include "OptionsCont.h"
#include <utils/common/UtilExceptions.h>
#include <utils/common/FileHelpers.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/ToString.h>


// ===========================================================================
// method definitions
// ===========================================================================
OptionsLoader::OptionsLoader()
        : myError(false), myOptions(OptionsCont::getOptions()), myItem() {}


OptionsLoader::~OptionsLoader() {}


void OptionsLoader::startElement(const XMLCh* const name,
                                 AttributeList& attributes) {
    myItem = TplConvert<XMLCh>::_2str(name);
    for (int i = 0; i < (int) attributes.getLength(); i++) {
        std::string key = TplConvert<XMLCh>::_2str(attributes.getName(i));
        std::string value = TplConvert<XMLCh>::_2str(attributes.getValue(i));
        if (key == "value" || key == "v") {
            setValue(myItem, value);
        }
        // could give a hint here about unsupported attributes in configuration files
    }
    myValue = "";
}


void OptionsLoader::setValue(const std::string &key,
                             std::string &value) {
    if (value.length()>0) {
        try {
            if (!setSecure(key, value)) {
                MsgHandler::getErrorInstance()->inform("Could not set option '" + key + "' (probably defined twice).");
                myError = true;
            }
        } catch (ProcessError &e) {
            MsgHandler::getErrorInstance()->inform(e.what());
            myError = true;
        }
    }
}


void OptionsLoader::characters(const XMLCh* const chars,
                               const XERCES3_SIZE_t length) {
    myValue = myValue + TplConvert<XMLCh>::_2str(chars, (unsigned int) length);
}


bool
OptionsLoader::setSecure(const std::string &name,
                         const std::string &value) const {
    if (myOptions.isWriteable(name)) {
        myOptions.set(name, value);
        return true;
    }
    return false;
}


void
OptionsLoader::endElement(const XMLCh* const /*name*/) {
    if (myItem.length()==0 || myValue.length()==0) {
        return;
    }
    if (myValue.find_first_not_of("\n\t \a")==std::string::npos) {
        return;
    }
    setValue(myItem, myValue);
    myItem = "";
    myValue = "";
}


void
OptionsLoader::warning(const SAXParseException& exception) {
    WRITE_WARNING(TplConvert<XMLCh>::_2str(exception.getMessage()));
    WRITE_WARNING(" (At line/column " \
                  + toString(exception.getLineNumber()+1) + '/' \
                  + toString(exception.getColumnNumber()) + ").");
    myError = true;
}


void
OptionsLoader::error(const SAXParseException& exception) {
    MsgHandler::getErrorInstance()->inform(
        TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(
        " (At line/column "
        + toString(exception.getLineNumber()+1) + '/'
        + toString(exception.getColumnNumber()) + ").");
    myError = true;
}


void
OptionsLoader::fatalError(const SAXParseException& exception) {
    MsgHandler::getErrorInstance()->inform(
        TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(
        " (At line/column "
        + toString(exception.getLineNumber()+1) + '/'
        + toString(exception.getColumnNumber()) + ").");
    myError = true;
}


bool
OptionsLoader::errorOccured() const {
    return myError;
}



/****************************************************************************/

