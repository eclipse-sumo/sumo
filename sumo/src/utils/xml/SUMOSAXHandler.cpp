/****************************************************************************/
/// @file    SUMOSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id$
///
// SAX-handler base for SUMO-files
/****************************************************************************/
// SUMO, Simulation of Urban MObility; see http://sumo.sourceforge.net/
// copyright : (C) 2001-2007
//  by DLR (http://www.dlr.de/) and ZAIK (http://www.zaik.uni-koeln.de/AFS)
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
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXHandler::SUMOSAXHandler(const std::string &file) throw()
        : GenericSAXHandler(sumotags, sumoattrs), myFileName(file)
{}


SUMOSAXHandler::~SUMOSAXHandler() throw()
{}


void
SUMOSAXHandler::setFileName(const std::string &name) throw()
{
    myFileName = name;
}


const std::string &
SUMOSAXHandler::getFileName() const throw()
{
    return myFileName;
}


string
SUMOSAXHandler::buildErrorMessage(const SAXParseException& exception) throw()
{
    ostringstream buf;
    char *pMsg = XMLString::transcode(exception.getMessage());
    buf << pMsg << endl;
    buf << " In file '" << getFileName() << "'" << endl;
    buf << " At line/column " << exception.getLineNumber()+1
    << '/' << exception.getColumnNumber() << "." << endl;
    delete[]pMsg;
    return buf.str();
}


void
SUMOSAXHandler::warning(const SAXParseException& exception) throw()
{
    MsgHandler::getWarningInstance()->inform(buildErrorMessage(exception));
}


void
SUMOSAXHandler::error(const SAXParseException& exception) throw(ProcessError)
{
    throw ProcessError(buildErrorMessage(exception));
}


void
SUMOSAXHandler::fatalError(const SAXParseException& exception) throw(ProcessError)
{
    throw ProcessError(buildErrorMessage(exception));
}


/****************************************************************************/

