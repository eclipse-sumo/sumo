/****************************************************************************/
/// @file    XMLSubSys.cpp
/// @author  Daniel Krajzewicz
/// @date    Mon, 1 Jul 2002
/// @version $Id$
///
// Utility methods for initialising, closing and using the XML-subsystem
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

#include <iostream>
#include <xercesc/sax2/XMLReaderFactory.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <utils/common/TplConvert.h>
#include <utils/common/MsgHandler.h>
#include <utils/common/TplConvert.h>
#include "SUMOSAXHandler.h"
#include "XMLSubSys.h"

#ifdef CHECK_MEMORY_LEAKS
#include <foreign/nvwa/debug_new.h>
#endif // CHECK_MEMORY_LEAKS


// ===========================================================================
// xerces 2.2 compatibility
// ===========================================================================
#if defined(XERCES_HAS_CPP_NAMESPACE)
using namespace XERCES_CPP_NAMESPACE;
#endif


// ===========================================================================
// used namespaces
// ===========================================================================
using namespace std;


// ===========================================================================
// method definitions
// ===========================================================================
void
XMLSubSys::init() throw(ProcessError)
{
    try {
        XMLPlatformUtils::Initialize();
    } catch (const XMLException& e) {
        throw ProcessError ("Error during XML-initialization:\n " + TplConvert<XMLCh>::my2str(e.getMessage()));
    }
}


void
XMLSubSys::close() throw()
{
    XMLPlatformUtils::Terminate();
}


SAX2XMLReader *
XMLSubSys::getSAXReader(SUMOSAXHandler &handler) throw()
{
    SAX2XMLReader *reader = XMLReaderFactory::createXMLReader();
    if (reader==0) {
        MsgHandler::getErrorInstance()->inform("The XML-parser could not be build");
        return 0;
    }
    setFeature(*reader,
               "http://xml.org/sax/features/namespaces", false);
    setFeature(*reader,
               "http://apache.org/xml/features/validation/schema", false);
    setFeature(*reader,
               "http://apache.org/xml/features/validation/schema-full-checking", false);
    setFeature(*reader,
               "http://xml.org/sax/features/validation", false);
    setFeature(*reader,
               "http://apache.org/xml/features/validation/dynamic" , false);
    reader->setContentHandler(&handler);
    reader->setErrorHandler(&handler);
    return reader;
}


bool
XMLSubSys::runParser(SUMOSAXHandler &handler,
                     const std::string &file) throw()
{
    SAX2XMLReader *reader = getSAXReader(handler);
    if (reader==0) {
        MsgHandler::getErrorInstance()->inform("Could not build reader to parse '" + file + "'.");
        return false;
    }
    try {
        reader->parse(file.c_str());
    } catch (ProcessError &e) {
        MsgHandler::getErrorInstance()->inform(e.what());
        return false;
    } catch (...) {
        MsgHandler::getErrorInstance()->inform("An error occured.");
        return false;
    }
    delete reader;
    return !MsgHandler::getErrorInstance()->wasInformed();
}


void
XMLSubSys::setFeature(XERCES_CPP_NAMESPACE_QUALIFIER SAX2XMLReader &reader,
                      const std::string &feature, bool value) throw()
{
    XMLCh *xmlFeature = XMLString::transcode(feature.c_str());
    reader.setFeature(xmlFeature, false);
    XMLString::release(&xmlFeature);
}




/****************************************************************************/

