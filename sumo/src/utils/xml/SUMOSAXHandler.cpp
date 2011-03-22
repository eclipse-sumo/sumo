/****************************************************************************/
/// @file    SUMOSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// SAX-handler base for SUMO-files
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

#include <string>
#include <iostream>
#include <utils/xml/GenericSAXHandler.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXHandler.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXHandler::SUMOSAXHandler(const std::string &file) throw()
        : GenericSAXHandler(SUMOXMLDefinitions::tags, SUMOXMLDefinitions::attrs, file) {}


SUMOSAXHandler::~SUMOSAXHandler() throw() {}


std::string
SUMOSAXHandler::buildErrorMessage(const SAXParseException& exception) throw() {
    std::ostringstream buf;
    char *pMsg = XMLString::transcode(exception.getMessage());
    buf << pMsg << std::endl;
    buf << " In file '" << getFileName() << "'" << std::endl;
    buf << " At line/column " << exception.getLineNumber()+1
    << '/' << exception.getColumnNumber() << "." << std::endl;
    XMLString::release(&pMsg);
    return buf.str();
}


void
SUMOSAXHandler::warning(const SAXParseException& exception) throw() {
    MsgHandler::getWarningInstance()->inform(buildErrorMessage(exception));
}


void
SUMOSAXHandler::error(const SAXParseException& exception) throw(ProcessError) {
    throw ProcessError(buildErrorMessage(exception));
}


void
SUMOSAXHandler::fatalError(const SAXParseException& exception) throw(ProcessError) {
    throw ProcessError(buildErrorMessage(exception));
}


/****************************************************************************/

