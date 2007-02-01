/****************************************************************************/
/// @file    SUMOSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: $
///
// The basic SAX-handler for SUMO-files
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
// compiler pragmas
// ===========================================================================
#ifdef _MSC_VER
#pragma warning(disable: 4786)
#endif


// ===========================================================================
// included modules
// ===========================================================================
#ifdef WIN32
#include <windows_config.h>
#else
#include <config.h>
#endif

#include <string>
#include <iostream>
#include <utils/xml/AttributesReadingGenericSAX2Handler.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/FileErrorReporter.h>
#include "SUMOXMLDefinitions.h"
#include "SUMOSAXHandler.h"

#ifdef _DEBUG
#include <utils/dev/debug_new.h>
#endif // _DEBUG


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
SUMOSAXHandler::SUMOSAXHandler(const std::string &filetype,
                               const std::string &file)
        : FileErrorReporter(filetype, file),
        AttributesReadingGenericSAX2Handler(sumotags, noSumoTags,
                                            sumoattrs, noSumoAttrs)
{}


SUMOSAXHandler::~SUMOSAXHandler()
{}


void
SUMOSAXHandler::warning(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform("Warning: " + TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(" (At line/column " + toString<int>(exception.getLineNumber()+1) + "/" + toString<int>(exception.getColumnNumber()) + ").");
    throw XMLBuildingException();
}


void
SUMOSAXHandler::error(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(" (At line/column " + toString<int>(exception.getLineNumber()+1) + "/" + toString<int>(exception.getColumnNumber()) + ").");
    throw XMLBuildingException();
}


void
SUMOSAXHandler::fatalError(const SAXParseException& exception)
{
    MsgHandler::getErrorInstance()->inform(TplConvert<XMLCh>::_2str(exception.getMessage()));
    MsgHandler::getErrorInstance()->inform(" (At line/column " + toString<int>(exception.getLineNumber()+1) + "/" + toString<int>(exception.getColumnNumber()) + ").");
    throw XMLBuildingException();
}



/****************************************************************************/

