/****************************************************************************/
/// @file    SUMOSAXHandler.cpp
/// @author  Daniel Krajzewicz
/// @date    Sept 2002
/// @version $Id: SUMOSAXHandler.cpp 3736 2007-03-30 12:47:23 +0200 (Fr, 30 Mrz 2007) dkrajzew $
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
#include <utils/xml/GenericSAXHandler.h>
#include <utils/xml/XMLBuildingExceptions.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include <utils/common/ToString.h>
#include <utils/common/FileErrorReporter.h>
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
SUMOSAXHandler::SUMOSAXHandler(const std::string &filetype,
                               const std::string &file)
        : FileErrorReporter(filetype, file),
        GenericSAXHandler(sumotags, sumoattrs),
        myCurrentSnippet(0)
{}


SUMOSAXHandler::~SUMOSAXHandler()
{}


string
SUMOSAXHandler::buildErrorMessage(const SAXParseException& exception)
{
    ostringstream buf;
    buf << TplConvert<XMLCh>::_2str(exception.getMessage()) << endl;
    buf << " In file: " << getFileName() << endl;
    buf << " At line/column " << exception.getLineNumber()+1
    << '/' << exception.getColumnNumber() << ")." << endl;
    return buf.str();
}


void
SUMOSAXHandler::warning(const SAXParseException& exception)
{
    throw XMLBuildingException("Warning: " + buildErrorMessage(exception));
}


void
SUMOSAXHandler::error(const SAXParseException& exception)
{
    throw XMLBuildingException(buildErrorMessage(exception));
}


void
SUMOSAXHandler::fatalError(const SAXParseException& exception)
{
    throw XMLBuildingException(buildErrorMessage(exception));
}


void
SUMOSAXHandler::addUnknownSnippet(const std::string &name, const Attributes &attrs)
{
    if (myCurrentSnippet==0) {
        myCurrentSnippet = new XMLSnippletStorage(0, name);
    } else {
        myCurrentSnippet = myCurrentSnippet->addChild(name);
    }
    for (size_t i=0; i<attrs.getLength(); ++i) {
        string aName = TplConvert<XMLCh>::_2str(attrs.getQName(i));
        string aValue = TplConvert<XMLCh>::_2str(attrs.getValue(i));
        myCurrentSnippet->addAttribute(aName, aValue);
    }
}


void
SUMOSAXHandler::addSnippetCharacters(const std::string &chars)
{
    if (myCurrentSnippet!=0) {
        myCurrentSnippet->addCharacters(chars);
    }
}


void
SUMOSAXHandler::closeSnippet()
{
    if (myCurrentSnippet!=0&&myCurrentSnippet->getParent()!=0) {
        myCurrentSnippet = myCurrentSnippet->getParent();
    }
}


void
SUMOSAXHandler::flushSnippet(std::ostream &strm, int level)
{
    myCurrentSnippet->flush(strm, level);
}


void
SUMOSAXHandler::deleteSnippet()
{
    delete myCurrentSnippet;
    myCurrentSnippet = 0;
}


XMLSnippletStorage *
SUMOSAXHandler::extractSnippet()
{
    XMLSnippletStorage *ret = myCurrentSnippet;
    while (ret!=0&&ret->getParent()!=0) {
        ret = ret->getParent();
    }
    myCurrentSnippet = 0;
    return ret;
}



/****************************************************************************/

